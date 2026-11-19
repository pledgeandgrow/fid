#!/bin/bash
# prof-check.sh — the sampling profiler of native Linux executables
# (docs/roadmap/llvm-roadmap.md DB9; stdlib/linux_runtime/sys.fd pe_prof_*).
#
# Usage: scripts/prof-check.sh [target ...]
#   target: x86_64-linux | aarch64-linux | riscv64-linux | x86_64-windows |
#           aarch64-macos | x86_64-macos
#           (default: the three Linux targets, plus x86_64-windows on a
#           Windows host; on a Mac, only the Mac's own macOS target)
#   x86_64-windows is `fid emit --nat` (win.fd's sampler thread), run
#   directly on a Windows host, no WSL; a macOS target is run directly on a
#   Mac of that architecture (setitimer). Either on another host: SKIP.
#   FID=<path to fid>   default release/fid.exe (release/fid off Windows)
#   OUT=<dir>           default build/prof-check
#
# Builds a program that spends its time in `fib` (called from `work`, from
# main) with `fid emit --target <target>`, runs it twice (as cross-test.sh
# does: natively on x86_64 Linux, qemu-user for the others, through WSL on
# Windows) and checks:
#   - without FID_PROF: the output and the exit code (main returns 3, which
#     pe_rt_exit passes on), nothing on stderr;
#   - with FID_PROF=1: the same output and exit code, and on stderr the
#     report: "profile (FID_PROF): N samples", N > 0, fib first by self
#     time, and main on the stack ("total") of most samples (`work` may be
#     inlined into it).
set -u
cd "$(dirname "$0")/.." || exit 1
ROOT=$(pwd)
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; Darwin) HOST=macos ;; *) HOST=linux ;; esac
if [ -z "${FID:-}" ]; then
  if [ "$HOST" = windows ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
OUT="${OUT:-build/prof-check}"
mkdir -p "$OUT"
TARGETS=("$@")
case "$(uname -m)" in arm64|aarch64) MAC_TGT=aarch64-macos ;; *) MAC_TGT=x86_64-macos ;; esac
if [ ${#TARGETS[@]} -eq 0 ]; then
  case "$HOST" in
    windows) TARGETS=(x86_64-linux aarch64-linux riscv64-linux x86_64-windows) ;;
    macos)   TARGETS=("$MAC_TGT") ;;
    *)       TARGETS=(x86_64-linux aarch64-linux riscv64-linux) ;;
  esac
fi

SRC="$OUT/prof_hot.fd"
cat > "$SRC" <<'FD'
fn fib(n: i64) -> i64 {
    if n < 2 {
        return n;
    }
    return fib(n - 1) + fib(n - 2);
}

fn work(k: i64) -> i64 {
    var s: i64 = 0;
    var i: i64 = 0;
    while i < k {
        s = s + fib(24 + i % 4);
        i = i + 1;
    }
    return s;
}

fn main() -> i64 {
    println(work(16));
    return 3;
}
FD
EXPECT="1756816"

fails=0
for T in "${TARGETS[@]}"; do
  case "$T" in
    aarch64-linux) RUNNER=qemu-aarch64 ;;
    riscv64-linux) RUNNER=qemu-riscv64 ;;
    x86_64-linux)  RUNNER="" ;;
    x86_64-windows) RUNNER="" ;;
    aarch64-macos|x86_64-macos) RUNNER="" ;;
    *) echo "unknown target: $T"; fails=$((fails+1)); continue ;;
  esac
  # LOCAL: the binary runs on this host directly (no WSL, no qemu)
  LOCAL=0
  case "$T" in
    x86_64-windows)
      if [ "$HOST" != windows ]; then echo "SKIP $T: runs on a Windows host only"; continue; fi
      LOCAL=1 ;;
    *-macos)
      if [ "$HOST" != macos ] || [ "$T" != "$MAC_TGT" ]; then echo "SKIP $T: runs on a $T Mac only"; continue; fi
      LOCAL=1 ;;
  esac
  BIN="$OUT/prof_hot-$T"
  if [ "$T" = x86_64-windows ]; then
    BIN="$BIN.exe"
    EMIT_ARGS=(--nat)
  else
    EMIT_ARGS=(--target "$T")
  fi
  if ! "$FID" emit "$SRC" "${EMIT_ARGS[@]}" -o "$BIN" > "$OUT/build-$T.log" 2>&1; then
    echo "FAIL $T: build (see $OUT/build-$T.log)"; fails=$((fails+1)); continue
  fi
  [ "$LOCAL" = 1 ] && chmod +x "$BIN" 2>/dev/null
  TO=""
  command -v timeout >/dev/null 2>&1 && TO="timeout 300"
  RUN="$OUT/run-$T.sh"
  {
    echo '#!/bin/bash'
    echo "cd \"\$(dirname \"\$0\")/$(echo "${OUT%/}" | sed -E 's#^\./##; s#[^/]+#..#g')\""
    echo "timeout 300 $RUNNER $BIN > $OUT/plain-$T.out 2> $OUT/plain-$T.err; echo \$? > $OUT/plain-$T.rc"
    echo "FID_PROF=1 timeout 300 $RUNNER $BIN > $OUT/prof-$T.out 2> $OUT/prof-$T.err; echo \$? > $OUT/prof-$T.rc"
  } > "$RUN"
  if [ "$LOCAL" = 1 ]; then
    # the same two runs on this host (OUT may be relative to the repository)
    case "$BIN" in /*|[A-Za-z]:*) EXE="$BIN" ;; *) EXE="./$BIN" ;; esac
    $TO "$EXE" > "$OUT/plain-$T.out" 2> "$OUT/plain-$T.err"; echo $? > "$OUT/plain-$T.rc"
    FID_PROF=1 $TO "$EXE" > "$OUT/prof-$T.out" 2> "$OUT/prof-$T.err"; echo $? > "$OUT/prof-$T.rc"
  elif [ "$HOST" = windows ]; then
    WROOT=$(cygpath -m "$ROOT" | sed -E 's#^([A-Za-z]):#/mnt/\L\1#')
    MSYS_NO_PATHCONV=1 wsl ${WSL_DISTRO:+-d "$WSL_DISTRO"} bash "$WROOT/$RUN"
  else
    bash "$RUN"
  fi
  bad=""
  for mode in plain prof; do
    [ "$(tr -d '\r' < "$OUT/$mode-$T.out")" = "$EXPECT" ] || bad="$bad $mode:output"
    [ "$(cat "$OUT/$mode-$T.rc")" = 3 ] || bad="$bad $mode:exit-code-$(cat "$OUT/$mode-$T.rc")"
  done
  [ -s "$OUT/plain-$T.err" ] && bad="$bad plain:stderr"
  # without the CRs a Windows executable writes
  tr -d '\r' < "$OUT/prof-$T.err" > "$OUT/prof-$T.err.txt"
  ERR="$OUT/prof-$T.err.txt"
  n=$(sed -n 's/^profile (FID_PROF): \([0-9]*\) samples.*/\1/p' "$ERR")
  if [ -z "$n" ] || [ "$n" -le 0 ]; then
    bad="$bad prof:no-samples"
  else
    # the first row after the header is the most self time
    first=$(sed -n '3p' "$ERR" | awk '{print $NF}')
    [ "$first" = fib ] || bad="$bad prof:first-is-$first"
    for f in main; do
      tot=$(awk -v f="$f" '$NF == f {print $4}' "$ERR")
      if [ -z "$tot" ] || [ $((tot * 2)) -lt "$n" ]; then bad="$bad prof:$f-total-${tot:-none}"; fi
    done
  fi
  if [ -n "$bad" ]; then
    echo "FAIL $T:$bad"; sed -n '1,8p' "$ERR"; fails=$((fails+1))
  else
    echo "PASS $T: $n samples, fib first"; sed -n '1,5p' "$ERR"
  fi
done
echo "prof-check: $fails failures"
[ "$fails" = 0 ]
