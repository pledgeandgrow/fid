#!/bin/bash
# cross-test.sh — run the test suite on another target through the C backend
# (release F5, F8): every test is built with `fid ccompile --target <target>`
# (zig cc cross-compiles from any host) and its output compared with the
# interpreter's (`fid run`), the reference.
#
# Usage:
#   scripts/cross-test.sh <target> [test.fd ...]
#     target: aarch64-linux | riscv64-linux | x86_64-linux   (built and run)
#             x86_64-macos | aarch64-macos                    (built only here;
#             run them on a Mac with scripts/cross-run.sh)
#             wasm32-wasi                                     (run with Node,
#             scripts/wasm-run.mjs)
#             aarch64-windows | x86_64-freebsd                (tier 2, PL3:
#             built here; run on Windows ARM64 / FreeBSD)
#             x86_64-windows | x86_64-windows-pe              (on Windows; TL6:
#             from Linux/macOS too, run under wine when installed)
#   A target this host cannot run is built only: its results say "BUILT"
#   for each test that built (BUILD_ONLY=1 forces that on any target).
#   FID=<path to fid>   default release/fid.exe (release/fid on Linux)
#   JOBS=<n>            parallel builds, default 8
#   OUT=<dir>           default build/cross/<target> (-native with NATIVE=1)
#   NATIVE=1            build with the native backend (`fid emit --target`:
#                       x86_64-linux, aarch64-linux, riscv64-linux; on Windows
#                       x86_64-windows is `emit --nat`, the machine layer, and
#                       x86_64-windows-pe is plain `emit`, the pe_ir backend)
#                       instead of C
#
# Running: on Linux natively (x86_64) or with qemu-user (qemu-aarch64,
# qemu-riscv64, Debian/Ubuntu package qemu-user). On Windows the binaries run
# in WSL (`wsl qemu-aarch64 ...`); set WSL_DISTRO to pick a distribution.
# The Linux binaries are static (musl), so no sysroot is needed.
# Built only (PL3): copy $OUT to the target machine, run `bash
# $OUT/run-all.sh` there (from the checkout's root or anywhere: it cds back),
# bring $OUT back and rerun with COMPARE_ONLY=1 (no build, no run: compare).
# HOST_ARCH=aarch64|x86_64|riscv64 overrides this host's architecture.
#
# Results: $OUT/results.txt (PASS/FAIL/BUILD/SKIP per test) and a summary.
# SKIP also marks a test scripts/lane-skips.txt leaves out on this target
# (LANE_SKIPS=<file>), and under qemu-user a dynamic executable whose
# loader is missing (QEMU_LD_PREFIX, else /usr/<arch>-linux-gnu: PL2).
# A test the interpreter cannot run because it calls C (an extern "C"
# function, a C function pointer) is compared with its own
# `// Expected output:` block instead, or SKIPped when it has none.
set -u
cd "$(dirname "$0")/.." || exit 1
ROOT=$(pwd)
TARGET="${1:-}"
if [ -z "$TARGET" ]; then sed -n '2,22p' "$0"; exit 1; fi
shift
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; Darwin) HOST=macos ;; FreeBSD) HOST=freebsd ;; *) HOST=linux ;; esac
# The host's architecture as targets write it (MSYS on Windows ARM64 runs
# as x86_64 under emulation: PROCESSOR_ARCHITEW6432/ARCHITECTURE say ARM64)
case "${PROCESSOR_ARCHITEW6432:-${PROCESSOR_ARCHITECTURE:-}}-$(uname -m)" in
  ARM64-*|*-aarch64|*-arm64) HARCH=aarch64 ;;
  *-riscv64) HARCH=riscv64 ;;
  *) HARCH=x86_64 ;;
esac
# HOST_ARCH=aarch64|x86_64|riscv64: what this host runs natively, when the
# shell cannot tell (an x86-64 Git Bash emulated on Windows ARM64, PL3)
case "${HOST_ARCH:-}" in
  aarch64|arm64) HARCH=aarch64 ;;
  x86_64|x64|amd64) HARCH=x86_64 ;;
  riscv64) HARCH=riscv64 ;;
esac
if [ -z "${FID:-}" ]; then
  if [ "$HOST" = windows ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
JOBS="${JOBS:-8}"
OUT="${OUT:-build/cross/$TARGET}"
if [ "${NATIVE:-}" = 1 ]; then OUT="${OUT}-native"; fi
EXT=bin
case "$TARGET" in wasm*|wasi) EXT=wasm ;; *-windows*) EXT=exe ;; esac
mkdir -p "$OUT"
export CC="${CC:-zig cc}"

case "$TARGET" in
  aarch64-linux*|arm64-linux*) RUNNER=qemu-aarch64; TOS=linux ;;
  riscv64-linux*)              RUNNER=qemu-riscv64; TOS=linux ;;
  x86_64-linux*)               RUNNER="";           TOS=linux ;;
  x86_64-windows*)             RUNNER="";           TOS=windows ;;   # NATIVE=1: emit --nat, run here
  aarch64-windows*|arm64-windows*) RUNNER="";       TOS=windows ;;   # PL3: through C (aarch64-windows-gnu)
  *-freebsd*)                  RUNNER="";           TOS=freebsd ;;   # PL3: through C, dynamic against FreeBSD's libc
  *-macos*)                    RUNNER="";           TOS=macos ;;
  wasm32-wasi*|wasm*|wasi)     RUNNER="node scripts/wasm-run.mjs"; TOS=wasi ;;
  *) echo "unknown target: $TARGET"; exit 1 ;;
esac
# A Linux host of the target's own architecture (an ARM64 runner for
# aarch64-linux, PL2) runs the binaries itself, without qemu. HARCH, not
# uname -m: HOST_ARCH says what the host runs natively when the shell
# cannot tell (the pl2 lane-skips check forces x86_64 to keep qemu on an
# ARM64 runner)
if [ "$HOST" = linux ]; then
  case "$HARCH-$RUNNER" in
    aarch64-qemu-aarch64|arm64-qemu-aarch64|riscv64-qemu-riscv64) RUNNER="" ;;
  esac
fi
# Can this host run the target's binaries? (PL2/PL3) Linux ones run here,
# in WSL on Windows, or under qemu-user; Windows ones on Windows (x86-64
# also on Windows ARM64); macOS ones on a Mac (x86-64 on Apple silicon
# through Rosetta 2, when installed); FreeBSD ones on FreeBSD. Anything
# else is built only.
case "$TARGET" in aarch64*|arm64*) TARCH=aarch64 ;; riscv64*) TARCH=riscv64 ;; wasm*|wasi) TARCH=wasm32 ;; *) TARCH=x86_64 ;; esac
RUN_HERE=1
case "$TOS" in
  windows) { [ "$HOST" = windows ] && { [ "$TARCH" = "$HARCH" ] || [ "$TARCH" = x86_64 ]; }; } || RUN_HERE=0 ;;
  macos)   if [ "$HOST" != macos ]; then RUN_HERE=0
           elif [ "$TARCH" != "$HARCH" ]; then
             { [ "$TARCH" = x86_64 ] && /usr/bin/arch -x86_64 /usr/bin/true > /dev/null 2>&1; } || RUN_HERE=0
           fi ;;
  freebsd) { [ "$HOST" = freebsd ] && [ "$TARCH" = "$HARCH" ]; } || RUN_HERE=0 ;;
  linux)   case "$HOST" in linux|windows) ;; *) RUN_HERE=0 ;; esac ;;
esac
# TL6: Windows x64 executables built on a Linux or macOS host (pe_ir with
# NATIVE=1 x86_64-windows-pe, the machine layer with x86_64-windows: no C
# compiler either way) run under wine when it is installed (WINE=<command>,
# else wine, else wine64). WSL's interop is not used: it does not pass the
# exit code back (scripts/cross-from-linux.sh builds in WSL with BUILD_ONLY=1
# and runs the executables on Windows itself).
WINE_CMD=""
if [ "$TOS" = windows ] && [ "$HOST" != windows ] && [ "$TARCH" = x86_64 ]; then
  WINE_CMD="${WINE:-}"
  if [ -z "$WINE_CMD" ]; then
    if command -v wine > /dev/null 2>&1; then WINE_CMD=wine
    elif command -v wine64 > /dev/null 2>&1; then WINE_CMD=wine64; fi
  fi
  if [ -n "$WINE_CMD" ]; then RUN_HERE=1; RUNNER="$WINE_CMD"; fi
fi
[ "${BUILD_ONLY:-}" = 1 ] && RUN_HERE=0

# Tests: the suite's normal tests (expected failures and tests that need
# external libraries, hardware or a bare machine are left out, as in the
# Rust harness), or the files given.
skip_re='^(test_comptime_basic|test_comptime_func|test_comptime_reflect|test_comptime_serialize|test_comptime_gen|test_http|test_json|test_tensor|test_autodiff|test_kv_cache|test_quantization|test_checkpoint|test_shard|test_lora|test_simd|test_shape|test_shape_verify|test_wasm|test_allocator|test_effect_handler|test_asm|test_asm_pe|test_bare.*|test_boot|test_interrupt|test_mmu|test_gat|test_ffi_header|test_dlopen|test_game_stdlib|test_kernel_serial|test_serial_hello|test_os_gaps|test_phase8b|test_vec_math|test_sqlite|test_pe_.*|test_parse_header_sqlite|test_lib_(sdl|gl|font|image|audio|audio_mixer|shader|mesh)|test_elf_native|test_dll_.*)\.fd$'
if [ $# -gt 0 ]; then
  TESTS=("$@")
else
  TESTS=()
  for f in tests/bootstrap/*.fd; do
    b=$(basename "$f")
    # Run-time panic tests (test_panic_*_fail) are compared too: the
    # output before the panic, the exit code and the panic's message
    case "$b" in test_panic_*_fail.fd) TESTS+=("$f"); continue ;; esac
    case "$b" in *_fail.fd|*_fail2.fd) continue ;; esac
    if [[ "$b" =~ $skip_re ]]; then continue; fi
    if head -3 "$f" | grep -q 'expect-error'; then continue; fi
    TESTS+=("$f")
  done
fi
# Tests this target cannot run by their nature (PL2, scripts/lane-skips.txt:
# "<target pattern> <test> <reason>", a leading ! negates the pattern): not
# built, listed as "SKIP  <test> (<reason>)". LANE_SKIPS=<file> another
# list, LANE_SKIPS=/dev/null none.
LANE_SKIPS="${LANE_SKIPS:-scripts/lane-skips.txt}"
LSKIP=$'\n'   # "<test>|<reason>" lines of the tests skipped on $TARGET
if [ -f "$LANE_SKIPS" ]; then
  while read -r spat stest swhy; do
    case "$spat" in ''|'#'*) continue ;; esac
    stest="${stest%$'\r'}"; swhy="${swhy%$'\r'}"   # a CRLF checkout
    sneg=0
    case "$spat" in '!'*) sneg=1; spat="${spat#!}" ;; esac
    smatch=0
    # shellcheck disable=SC2254
    case "$TARGET" in $spat) smatch=1 ;; esac
    [ "$smatch" != "$sneg" ] && LSKIP="$LSKIP$stest|${swhy:-skipped on $TARGET}"$'\n'
  done < "$LANE_SKIPS"
fi
# A test that imports the compiler's own modules (`use compiler::ast`)
# finds them in compiler/ of the project root (compiler.fd
# find_project_root: the directory holding stdlib/core.fd). A copy of
# tests/ and stdlib/ without compiler/ (a snapshot made for a suite run)
# cannot build it: "SKIP (needs compiler/ ...)", not BUILD. Run the suite
# from the repository, or a snapshot with compiler/, to check these.
if [ ! -f compiler/main.fd ]; then
  for f in "${TESTS[@]+"${TESTS[@]}"}"; do
    if tr -d '\r' < "$f" 2> /dev/null | grep -aqE '^[[:space:]]*use compiler::'; then
      b="${f##*/}"; b="${b%.fd}"
      LSKIP="$LSKIP$b|needs compiler/ in $ROOT: it imports compiler:: modules"$'\n'
    fi
  done
fi
ALL_TESTS=("${TESTS[@]+"${TESTS[@]}"}")
TESTS=()
lskip=0
for f in "${ALL_TESTS[@]+"${ALL_TESTS[@]}"}"; do
  b="${f##*/}"; b="${b%.fd}"
  if [[ "$LSKIP" == *$'\n'"$b|"* ]]; then lskip=$((lskip + 1)); else TESTS+=("$f"); fi
done
echo "target $TARGET: ${#TESTS[@]} tests, $JOBS builds at a time, output in $OUT$([ "$lskip" = 0 ] || echo " ($lskip skipped on this target: $LANE_SKIPS)")"

# 1. The reference output (interpreter) and the cross build of each test
build_one() {
  local f="$1" b; b=$(basename "$f" .fd)
  # REUSE=1: keep what built before (rebuild only failures)
  if [ "${REUSE:-}" = 1 ] && [ "$(cat "$OUT/$b.build_rc" 2>/dev/null)" = 0 ]; then return; fi
  timeout 120 "$FID" run "$f" < /dev/null > "$OUT/$b.expected" 2> "$OUT/$b.expected_err"
  echo $? > "$OUT/$b.expected_rc"
  if [ "${NATIVE:-}" = 1 ]; then
    # The native backend (compiler/nat_ir.fd): no C compiler
    local nat="" ntarget=(--target "$TARGET")
    case "$TARGET" in
      *-windows-pe)                 # pe_ir: `fid emit` with no target on Windows;
        ntarget=()                  # TL6: `--target x86_64-windows` elsewhere
        [ "$HOST" = windows ] || ntarget=(--target x86_64-windows) ;;
      *-windows*) nat=--nat ;;
    esac
    timeout 600 "$FID" emit "$f" "${ntarget[@]+"${ntarget[@]}"}" $nat -o "$OUT/$b.$EXT" > "$OUT/$b.build.log" 2>&1
  else
    # on a Mac its own macOS target is a host build (CC, not zig's cross
    # target); everywhere else `--target` cross-compiles (x86_64-macos on
    # Apple silicon too: a host build would be arm64)
    local tflag=(--target "$TARGET")
    if [ "$HOST" = macos ] && [ "$TOS" = macos ] && [ "$TARCH" = "$HARCH" ]; then tflag=(); fi
    # bash 3.2 (macOS): an empty array expanded under set -u is an unbound variable
    timeout 600 "$FID" ccompile "$f" ${tflag[@]+"${tflag[@]}"} --cc-opt 0 --no-cache -o "$OUT/$b.$EXT" > "$OUT/$b.build.log" 2>&1
  fi
  echo $? > "$OUT/$b.build_rc"
}
export -f build_one
export FID TARGET OUT EXT REUSE NATIVE HOST TOS TARCH HARCH   # not OS: Windows sets OS=Windows_NT, and fid reads it
# One build first: zig builds the target's libc into its cache on first use,
# and parallel first builds race on that cache (CacheCheckFailed)
if [ "${COMPARE_ONLY:-}" != 1 ] && [ "${#TESTS[@]}" -gt 0 ]; then
build_one "${TESTS[0]}"
printf '%s\n' "${TESTS[@]:1}" | xargs -P "$JOBS" -I{} bash -c 'build_one "$@"' _ {}
fi

# Dynamic executables under qemu-user (PL2). A native Linux executable that
# calls C functions names the system's dynamic loader (compiler/nat_ir.fd
# ng_ldyn_interp, CI2) and is bound against the target's C library at
# load. qemu-user finds an AArch64 or RISC-V loader under QEMU_LD_PREFIX:
# run-all.sh sets it to /usr/<arch>-linux-gnu when that holds one (Debian
# and Ubuntu: apt install libc6-arm64-cross / libc6-riscv64-cross). Where
# there is none such a test is "SKIP (needs <loader> ...)", not a FAIL.
# A binary that imports one of the runtime's own names (nat_*, pert_*,
# p_*: a compiler bug, scripts/bin-target-check.py) is not spared: it runs
# and fails. Needs python 3 to read the ELF; without it every test runs.
LOADERS=$'\n'   # "<test>|<loader>" lines
case "$RUNNER" in
  qemu-*)
    LPY=""
    for p in python3 python; do
      command -v "$p" > /dev/null 2>&1 && "$p" -c 'import sys; sys.exit(sys.version_info[0] < 3)' 2> /dev/null && { LPY=$p; break; }
    done
    if [ -n "$LPY" ]; then
      while IFS=$'\t' read -r lbin linterp lrt; do
        [ -n "$lbin" ] || continue
        if [ "$linterp" != - ] && [ "$lrt" = - ]; then
          lb="${lbin##*/}"; lb="${lb%.$EXT}"
          LOADERS="$LOADERS$lb|$linterp"$'\n'
        fi
      done < <(for f in "${TESTS[@]+"${TESTS[@]}"}"; do
                 b="${f##*/}"; b="${b%.fd}"
                 [ "$(cat "$OUT/$b.build_rc" 2>/dev/null)" = 0 ] && [ -f "$OUT/$b.$EXT" ] && echo "$OUT/$b.$EXT"
               done | "$LPY" scripts/bin-target-check.py --loader - 2> /dev/null | tr -d '\r')
    fi ;;
esac
loader_of() {  # test -> the dynamic loader its binary needs under qemu, or nothing
  local rest
  case "$LOADERS" in *$'\n'"$1|"*) ;; *) return 0 ;; esac
  rest="${LOADERS#*$'\n'"$1|"}"
  echo "${rest%%$'\n'*}"
}

# 2. Run every binary that built (a script, so it can run in WSL)
RUNS="$OUT/run-all.sh"
# run_view <path>: the path as the runner sees it. Binaries for Linux run in
# WSL on a Windows host: C:/x/y (or /c/x/y) -> /mnt/c/x/y; MSYS must not
# rewrite the path given to wsl
run_view() {
  if [ "$HOST" = windows ] && [ "$TOS" = linux ]; then
    cygpath -m "$1" | sed -E 's#^([A-Za-z]):#/mnt/\L\1#'
  else
    echo "$1"
  fi
}
case "$OUT" in /*|?:*) OUTR=$(run_view "$OUT") ;; *) OUTR="$OUT" ;; esac
{
  echo '#!/bin/bash'
  if [ "$OUTR" = "$OUT" ] && case "$OUT" in /*|?:*) false ;; *) true ;; esac; then
    # Back from $OUT (relative to the root) to the root
    echo "cd \"\$(dirname \"\$0\")/$(echo "${OUT%/}" | sed -E 's#^\./##; s#[^/]+#..#g')\""
  else
    # $OUT is absolute (OUT=/tmp/x, OUT=C:/x): paths in the script are too;
    # the root is still the working directory (scripts/wasm-run.mjs)
    echo "cd \"$(run_view "$ROOT")\""
  fi
  case "$RUNNER" in
    qemu-*)  # the target's C library for dynamic executables (above)
      echo "if [ -z \"\${QEMU_LD_PREFIX:-}\" ] && [ -d /usr/$TARCH-linux-gnu/lib ]; then export QEMU_LD_PREFIX=/usr/$TARCH-linux-gnu; fi" ;;
  esac
  for f in "${TESTS[@]+"${TESTS[@]}"}"; do
    b=$(basename "$f" .fd)
    [ "$(cat "$OUT/$b.build_rc" 2>/dev/null)" = 0 ] || continue
    runl="timeout 60 $RUNNER \"$OUTR/$b.$EXT\" < /dev/null > \"$OUTR/$b.actual\" 2> \"$OUTR/$b.stderr\"; echo \$? > \"$OUTR/$b.actual_rc\""
    if [[ "$LOADERS" == *$'\n'"$b|"* ]]; then
      li=$(loader_of "$b")
      echo "rm -f \"$OUTR/$b.noloader\"; if [ -e \"\${QEMU_LD_PREFIX:-}$li\" ]; then $runl; else echo \"$li\" > \"$OUTR/$b.noloader\"; fi"
    else
      echo "$runl"
    fi
  done
} > "$RUNS"
chmod +x "$RUNS"
if [ "${COMPARE_ONLY:-}" = 1 ]; then
  # PL3: the binaries ran on the target (run-all.sh there); compare what
  # they left in $OUT with the references made when they were built
  echo "$TARGET: comparing the results run-all.sh left in $OUT"
  RAN=1
elif [ "$RUN_HERE" = 0 ]; then
  echo "$TARGET binaries built only (this host cannot run them); run $RUNS on a $TOS $TARCH machine, then rerun this script there"
  RAN=0
elif [ "$HOST" = windows ] && [ "$TOS" != wasi ] && [ "$TOS" != windows ]; then
  # C:/x/y -> /mnt/c/x/y; MSYS must not rewrite the path given to wsl
  case "$RUNS" in /*|?:*) WRUNS=$(run_view "$RUNS") ;; *) WRUNS="$(run_view "$ROOT")/$RUNS" ;; esac
  MSYS_NO_PATHCONV=1 wsl ${WSL_DISTRO:+-d "$WSL_DISTRO"} bash "$WRUNS"
  RAN=1
else
  if [ -n "$WINE_CMD" ]; then
    # TL6: wine quiet, its prefix made before the first (timed) test
    export WINEDEBUG="${WINEDEBUG:--all}"
    timeout 300 "$WINE_CMD" wineboot --init > /dev/null 2>&1 || true
  fi
  bash "$RUNS"
  RAN=1
fi

# 3. Compare
# A panic's message without its details: "Panic: Integer overflow on
# addition" (the interpreter also prints the operands, and the source line:
# "Panic: Division by zero at line 6", which compiled code does not; the
# native and C runtimes end "Invalid memory access ..." with " at 0x<addr>")
panic_key() { grep -a -m1 '^Panic:' "$1" 2>/dev/null | tr -d '\r' | sed -E 's/^(Panic: [^:]*).*/\1/; s/ at line [0-9]+$//; s/ at 0x[0-9A-Fa-f]+$//'; }
# The interpreter cannot run a program that calls C: an extern "C" function
# other than the few libc calls it has (compiler/interp.fd "FFI function
# ... is not available in the interpreter") or a C function pointer made
# with `as fn(..)` ("... can only be called in a C build"). Its panic is no
# reference. The test's own `// Expected output:` block is then (each
# `// ` stripped, a line ending in `...` matching any line that starts with
# the rest, as in scripts/bare-qemu-test.sh), with `// Expected exit: N`
# (default 0); lines are compared without their leading and trailing
# blanks, since the blocks indent them. With no block: no reference.
interp_cannot_call_c() {
  grep -aq -e '^Panic: FFI function .* is not available in the interpreter' \
           -e '^Panic: a C function pointer .* can only be called in a C build' "$1" 2>/dev/null
}
block_of() {   # $1 = test file: its expected lines, trimmed
  awk '
    { sub(/\r$/, "") }
    /^\/\/ Expected output:/ { on = 1; next }
    on && /^\/\/ Expected exit:/ { next }
    on && /^\/\// { sub(/^\/\/ ?/, ""); sub(/^[ \t]+/, ""); sub(/[ \t]+$/, ""); print; next }
    on { exit }
  ' "$1"
}
has_block() { grep -q '^// Expected output:' "$1" 2>/dev/null; }
block_exit_of() {
  local e
  e=$(tr -d '\r' < "$1" | sed -n 's#^// Expected exit: *\([0-9][0-9]*\).*#\1#p' | head -1)
  echo "${e:-0}"
}
same_as_block() {   # $1 = actual output, $2 = expected lines (block_of)
  tr -d '\r' < "$1" 2>/dev/null | awk -v exp_file="$2" '
    BEGIN { n = 0; while ((getline l < exp_file) > 0) { n++; e[n] = l } }
    { sub(/^[ \t]+/, ""); sub(/[ \t]+$/, ""); m++; a[m] = $0 }
    END {
      if (m != n) exit 1
      for (i = 1; i <= n; i++) {
        if (e[i] ~ /\.\.\.$/) {
          p = substr(e[i], 1, length(e[i]) - 3)
          if (substr(a[i], 1, length(p)) != p) exit 1
        } else if (a[i] != e[i]) exit 1
      }
    }
  '
}
pass=0; fail=0; berr=0; skip=0; built=0; noload=0
: > "$OUT/results.txt"
# the tests scripts/lane-skips.txt leaves out on this target, with why
printf '%s' "$LSKIP" | while IFS='|' read -r sb swhy; do
  [ -n "$sb" ] || continue
  for f in "${ALL_TESTS[@]+"${ALL_TESTS[@]}"}"; do
    case "$f" in */"$sb.fd"|"$sb.fd") echo "SKIP  $sb ($swhy)" >> "$OUT/results.txt"; break ;; esac
  done
done
for f in "${TESTS[@]+"${TESTS[@]}"}"; do
  b=$(basename "$f" .fd)
  # The reference is the interpreter's output and exit code; a program may
  # return a code of its own (ret42), but a compile error or a timeout
  # leaves no reference
  erc=$(cat "$OUT/$b.expected_rc" 2>/dev/null)
  # An interpreter slower than the timeout (rc 124) leaves no output, but a
  # test with its own `// Expected output:` block is still checked against
  # that block (as for a program the interpreter cannot run: below)
  byblock=0; whyblock="fid run cannot call C"
  if [ "$erc" = 124 ] && has_block "$f"; then
    byblock=1; whyblock="fid run timed out"
  elif [ "$erc" = 124 ] || grep -q '^Compilation error\|^Compilation errors\|^error: ' "$OUT/$b.expected" 2>/dev/null; then
    echo "SKIP  $b (fid run gives no reference: rc $erc)" >> "$OUT/results.txt"; skip=$((skip+1)); continue
  fi
  if [ "$byblock" = 0 ] && interp_cannot_call_c "$OUT/$b.expected_err"; then
    if ! has_block "$f"; then
      echo "SKIP  $b (fid run cannot call C, and the test has no Expected output)" >> "$OUT/results.txt"; skip=$((skip+1)); continue
    fi
    byblock=1
  fi
  if [ "$(cat "$OUT/$b.build_rc" 2>/dev/null)" != 0 ]; then
    echo "BUILD $b" >> "$OUT/results.txt"; berr=$((berr+1)); continue
  fi
  if [ "$RAN" != 1 ]; then
    echo "BUILT $b (not run here)" >> "$OUT/results.txt"; built=$((built+1)); continue
  fi
  if [[ "$LOADERS" == *$'\n'"$b|"* ]] && [ -f "$OUT/$b.noloader" ]; then
    echo "SKIP  $b (dynamic: needs $(loader_of "$b") for $RUNNER; install the target's libc, e.g. libc6-arm64-cross, or set QEMU_LD_PREFIX)" >> "$OUT/results.txt"
    noload=$((noload+1)); continue
  fi
  arc=$(cat "$OUT/$b.actual_rc" 2>/dev/null)
  if [ "$byblock" = 1 ]; then
    block_of "$f" > "$OUT/$b.expected_block"
    xrc=$(block_exit_of "$f")
    if [ "$arc" = "$xrc" ] && same_as_block "$OUT/$b.actual" "$OUT/$b.expected_block"; then
      echo "PASS  $b" >> "$OUT/results.txt"; pass=$((pass+1))
    else
      echo "FAIL  $b (rc $arc, its Expected output: rc $xrc; $whyblock)" >> "$OUT/results.txt"; fail=$((fail+1))
    fi
    continue
  fi
  if [ "$arc" = "$erc" ] && [ "$(panic_key "$OUT/$b.expected_err")" = "$(panic_key "$OUT/$b.stderr")" ] \
     && cmp -s <(tr -d '\r' < "$OUT/$b.expected") <(tr -d '\r' < "$OUT/$b.actual" 2>/dev/null); then
    echo "PASS  $b" >> "$OUT/results.txt"; pass=$((pass+1))
  else
    echo "FAIL  $b (rc $arc, interpreter $erc)" >> "$OUT/results.txt"; fail=$((fail+1))
  fi
done
echo "$TARGET: $pass passed, $fail failed, $berr did not build, $skip without reference$([ "$RAN" = 1 ] || echo ", $built built only (not run here)")$([ "$lskip" = 0 ] || echo ", $lskip skipped on this target")$([ "$noload" = 0 ] || echo ", $noload dynamic not run (no loader for $RUNNER)")"
grep -v '^PASS\|^BUILT' "$OUT/results.txt" | head -50
[ "$fail" = 0 ] && [ "$berr" = 0 ]
