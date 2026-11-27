#!/bin/bash
# bootstrap.sh — build the self-hosted compiler from scratch and check the fixpoint.
#
# Usage:
#   ./scripts/bootstrap.sh [out_dir] [stages]     (defaults: build/bootstrap, 3)
#   RELEASE=1 ./scripts/bootstrap.sh              also install the verified
#                                                 stage-3 compiler as
#                                                 release/fid(.exe)
#   NATIVE=1 ./scripts/bootstrap.sh               stages through `fid emit`
#                                                 (Windows x64): no C compiler
#   PIN=1 ./scripts/bootstrap.sh                  after a fixpoint, pin the last
#                                                 stage as the next stage 0
#
# Stage 0 is a pinned self-hosted compiler, release/stage0/fid(.exe), checked
# against the hash in scripts/stage0.sha256 (ST1: the Rust compiler is frozen
# and only kept as the test harness). Without a pinned stage 0 the build
# starts cold from the Rust compiler (target/release/fidesc); STAGE0=<exe>
# overrides both. The compiler's own source must stay compilable by the
# pinned stage 0: a language change lands in two steps, the compiler first,
# its own source after the next pin.
#
# C mode (default): each stage N runs the previous compiler's
# `dump-c compiler/main.fd`, wraps `main` in a thread with a 256MB stack (the
# compiler recurses deeply), and compiles the C. The fixpoint holds when stage
# 2 and stage 3 emit identical C (compared with line endings normalized —
# stdout text mode differs between builds on Windows).
# Native mode: each stage runs `emit compiler/main.fd`; the fixpoint holds when
# the stage 2 and stage 3 executables are byte-identical.
#
# C compiler: $CC if set, else the first of tcc, gcc, clang, `zig cc`.
# The generated C is one ~60MB file; optimizing it as one unit needs >16GB
# of RAM. When the previous stage has `fid split-c`, the C is split into
# SPLIT units (default 8) and a header, compiled at -O2 JOBS at a time
# (default 4), and linked; otherwise (or with SPLIT=1, or tcc) it is built
# as one unit at -O0. A split build that fails falls back to one unit.
set -u
cd "$(dirname "$0")/.." || exit 1
OUT="${1:-build/bootstrap}"
STAGES="${2:-3}"
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) X=".exe" ;; *) X="" ;; esac
PINNED="release/stage0/fid$X"
# The repository's pin for this host (stage0-fetch.sh knows the asset name:
# scripts/<asset>.sha256, scripts/stage0.sha256 for windows-x64)
PIN_FILE=$(./scripts/stage0-fetch.sh --pin-file 2>/dev/null) || PIN_FILE="scripts/stage0.sha256"
[ -f "$PIN_FILE" ] || PIN_FILE="scripts/stage0.sha256"
sha256_of() {
  if command -v sha256sum >/dev/null; then sha256sum "$1" | cut -d' ' -f1
  elif command -v shasum >/dev/null; then shasum -a 256 "$1" | cut -d' ' -f1
  fi
}
if [ -z "${STAGE0:-}" ]; then
  if [ -x "$PINNED" ]; then
    STAGE0="$PINNED"
    # BG359: the binary is checked against the repository's pin, not the
    # copy stage0-fetch.sh left beside it (that copy always matches): after
    # a pin update in git the old stage 0 is fetched again, or a warning
    # says it is stale. (PIN_FILE, not PIN: PIN=1 is the request to pin.)
    if [ -f "$PIN_FILE" ]; then
      want=$(cut -d' ' -f1 "$PIN_FILE")
      have=$(sha256_of "$PINNED")
      if [ -n "$have" ] && [ "$want" != "$have" ]; then
        if [ "${FID_STAGE0_FETCH:-1}" != 0 ] && ./scripts/stage0-fetch.sh; then
          echo "note: $PINNED did not match $PIN_FILE; fetched the pinned stage 0 again"
        else
          echo "warning: $PINNED does not match $PIN_FILE (fetch it with scripts/stage0-fetch.sh, or pin it again with PIN=1)"
        fi
      fi
    fi
  elif [ "${FID_STAGE0_FETCH:-1}" != 0 ] && ./scripts/stage0-fetch.sh; then
    # stage0-fetch.sh places the verified binary at $PINNED
    STAGE0="$PINNED"
  else
    STAGE0=./target/release/fidesc
    echo "note: no pinned stage 0 ($PINNED); cold start from the frozen Rust compiler"
  fi
fi
echo "stage 0: $STAGE0"
mkdir -p "$OUT"

if [ -n "${CC:-}" ]; then
  CC_CMD=($CC)
elif command -v tcc >/dev/null; then CC_CMD=(tcc)
elif command -v gcc >/dev/null; then CC_CMD=(gcc)
elif command -v clang >/dev/null; then CC_CMD=(clang)
elif command -v zig >/dev/null; then CC_CMD=(zig cc)
else echo "error: no C compiler found (set CC)"; exit 1; fi
# -DP_NO_ENV_LIMITS: FID_MEM_LIMIT / FID_CPU_LIMIT in the environment limit
# the programs fid builds, not fid itself while it compiles
CFLAGS=(-O0 -w -DP_NO_ENV_LIMITS)
case "${CC_CMD[*]}" in
  *tcc*) ;;
  *) CFLAGS+=(-fno-sanitize=all -Wno-error -Wno-int-conversion -Wno-incompatible-pointer-types
              -Wno-implicit-function-declaration -Wno-return-type) ;;
esac
SPLIT="${SPLIT:-8}"
JOBS="${JOBS:-4}"
# -O2 on generated C: no strict aliasing (it reads memory through casts)
# and wrapping signed arithmetic (@wrapping code relies on it)
SPLIT_CFLAGS=(-O2 -fno-strict-aliasing -fwrapv)
for f in "${CFLAGS[@]}"; do [ "$f" != "-O0" ] && SPLIT_CFLAGS+=("$f"); done
SPLIT_OPT=-O2
. scripts/build-units.sh
LIBS=()
EXE=""
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) LIBS=(-lws2_32); EXE=".exe" ;; Darwin) LIBS=(-lm -lpthread); CFLAGS+=(-D_XOPEN_SOURCE=600 -D_DARWIN_C_SOURCE) ;; *) LIBS=(-lm -lpthread -ldl); case "${CC_CMD[*]}${CC:-}" in *zig*) LIBS+=(-lunwind) ;; esac ;; esac

# Rename main and run it on a thread with a 256MB stack.
wrap() {
  sed 's/^int main(int argc, char\*\* argv)/int fides_main(int argc, char** argv)/' "$1" > "$2"
  cat >> "$2" <<'EOF'

// === Stack wrapper (added by scripts/bootstrap.sh) ===
typedef struct { int argc; char** argv; int ret; } MainArgs;
#ifdef _WIN32
#include <windows.h>
static DWORD WINAPI thread_main(LPVOID p) { MainArgs* a = (MainArgs*)p; a->ret = fides_main(a->argc, a->argv); return 0; }
int main(int argc, char** argv) {
    MainArgs a = { argc, argv, 0 };
    HANDLE h = CreateThread(NULL, 256*1024*1024, thread_main, &a, 0, NULL);
    if (!h) return 1;
    WaitForSingleObject(h, INFINITE);
    CloseHandle(h);
    return a.ret;
}
#else
#include <pthread.h>
static void* thread_main(void* p) { MainArgs* a = (MainArgs*)p; a->ret = fides_main(a->argc, a->argv); return 0; }
int main(int argc, char** argv) {
    MainArgs a = { argc, argv, 0 };
    pthread_attr_t attr; pthread_t t;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, (size_t)256*1024*1024);
    if (pthread_create(&t, &attr, thread_main, &a) != 0) return 1;
    pthread_join(t, NULL);
    return a.ret;
}
#endif
EOF
}

# compile_c <wrapped.c> <exe> <log> <splitter fid>: the stage's C as SPLIT
# units at -O2 when the splitter has `split-c`, else as one unit at -O0
compile_c() {
  local c="$1" exe="$2" log="$3" fid="$4" base="${1%.c}_u"
  : > "$log"
  case "${CC_CMD[*]}" in *tcc*) ;; *)
  if [ "$SPLIT" -gt 1 ] && "$fid" split-c "$c" "$base" "$SPLIT" >> "$log" 2>&1 && [ -f "$base.h" ]; then
    # scripts/build-units.sh: the units within the runner's memory (on a
    # hosted runner the heavy ones alone, every compile with a limit), a
    # failed one again alone, then at -O1 and -O0
    if units_compile "$base" "$log" && "${CC_CMD[@]}" "${UNITS_OBJS[@]}" -o "$exe" "${LIBS[@]}" >> "$log" 2>&1; then
      units_summary
      return 0
    fi
  echo "  split build failed (see $log); building one unit at -O0"
  fi ;;
  esac
  "${CC_CMD[@]}" "${CFLAGS[@]}" "$c" -o "$exe" "${LIBS[@]}" >> "$log" 2>&1
}

# N12: a release build names its commit and date in `fid version`
# (compiler/build_info.fd, written for the stages and put back on exit)
if [ -n "${RELEASE:-}" ]; then
  bash scripts/build-info.sh
  trap 'bash scripts/build-info.sh --reset' EXIT
  # D9: the stages share one source hash and differ only in size; keyed by
  # their bytes (inc.fd inc_compiler_identity "exe") they never share a
  # build cache. The installed fid keeps the cheap source key.
  export FIDES_SELF_HASH=exe
fi

prev="$STAGE0"
  if [ -n "${NATIVE:-}" ]; then
  # how a stage's compiler ended: "exit 1", or "killed by signal 11" for a death
  # by signal (bash prints those themselves)
  ended() { if [ "$1" -gt 128 ]; then echo "killed by signal $(( $1 - 128 ))"; else echo "exit $1"; fi; }
  for n in $(seq 1 "$STAGES"); do
    start=$(date +%s)
    erc=0
    "$prev" emit compiler/main.fd -o "$OUT/fid-n$n$EXE" > "$OUT/n$n.log" 2>&1 || erc=$?
    if [ "$erc" -ne 0 ]; then
      echo "[stage $n] emit failed ($(ended "$erc")):"; grep -iE "error" "$OUT/n$n.log" | head -20; tail -3 "$OUT/n$n.log" | sed 's/^/    /'; exit 1
    fi
    echo "[stage $n] emit ok in $(( $(date +%s) - start ))s ($(wc -c < "$OUT/fid-n$n$EXE") bytes)"
    prev="$OUT/fid-n$n$EXE"
  done
  if [ "$STAGES" -ge 3 ]; then
    if cmp -s "$OUT/fid-n2$EXE" "$OUT/fid-n3$EXE"; then
      echo "FIXPOINT OK: stage 2 and stage 3 executables are byte-identical"
    else
      echo "FIXPOINT FAILED: stage 2 and stage 3 executables differ"; exit 1
    fi
  fi
  LAST="$OUT/fid-n$STAGES$EXE"
else
# how a stage's compiler ended: "exit 1", or "killed by signal 11" for a death
# by signal (bash prints those itself, as "Segmentation fault")
ended() { if [ "$1" -gt 128 ]; then echo "killed by signal $(( $1 - 128 ))"; else echo "exit $1"; fi; }
for n in $(seq 1 "$STAGES"); do
  start=$(date +%s)
  drc=0
  "$prev" dump-c compiler/main.fd > "$OUT/c$n.c" 2> "$OUT/c$n.log" || drc=$?
  if [ "$drc" -ne 0 ]; then
    # A death by signal leaves no error line (Nov 27, the Windows release
    # job: the pinned stage 0 "Segmentation fault" in dump-c): say how it
    # ended and how far it got, and try such a death once more
    echo "[stage $n] dump-c failed ($(ended "$drc"))"
    grep -E "error" "$OUT/c$n.log" | head -20
    tail -3 "$OUT/c$n.log" | sed 's/^/    /'
    if [ "$drc" -gt 128 ]; then
      echo "[stage $n] dump-c once more"
      drc=0
      "$prev" dump-c compiler/main.fd > "$OUT/c$n.c" 2> "$OUT/c$n.log" || drc=$?
    fi
    if [ "$drc" -ne 0 ]; then
      echo "[stage $n] dump-c failed ($(ended "$drc"))"
      grep -E "error" "$OUT/c$n.log" | head -20
      tail -3 "$OUT/c$n.log" | sed 's/^/    /'
      exit 1
    fi
  fi
  echo "[stage $n] dump-c ok in $(( $(date +%s) - start ))s ($(wc -c < "$OUT/c$n.c") bytes)"
  wrap "$OUT/c$n.c" "$OUT/c${n}_wrapped.c"
  start=$(date +%s)
  if ! compile_c "$OUT/c${n}_wrapped.c" "$OUT/fides-c$n$EXE" "$OUT/c$n.cc.log" "$prev"; then
    echo "[stage $n] C compile failed:"; grep -E "error" "$OUT/c$n.cc.log" | head -20; exit 1
  fi
  echo "[stage $n] C compile ok in $(( $(date +%s) - start ))s"
  prev="$OUT/fides-c$n$EXE"
done

if [ "$STAGES" -ge 3 ]; then
  if cmp -s <(tr -d '\r' < "$OUT/c2.c") <(tr -d '\r' < "$OUT/c3.c"); then
    echo "FIXPOINT OK: stage 2 and stage 3 emit identical C"
  else
    echo "FIXPOINT FAILED: stage 2 and stage 3 differ"
    diff <(tr -d '\r' < "$OUT/c2.c") <(tr -d '\r' < "$OUT/c3.c") | head -20
    exit 1
  fi
fi
LAST="$OUT/fides-c$STAGES$EXE"
fi

# PIN=1: the last stage (it reached the fixpoint) becomes the next stage 0.
# The binary stays out of git (release/ is ignored); its hash and the commit
# it was built from are recorded in this host's pin file ($PIN_FILE).
if [ -n "${PIN:-}" ]; then
  if [ "$STAGES" -lt 3 ]; then echo "PIN needs 3 stages (a checked fixpoint)"; exit 1; fi
  mkdir -p release/stage0
  cp "$LAST" "$PINNED"
  # BG359: this host's pin file (scripts/stage0.sha256 on windows-x64), the
  # one the stage-0 check above and stage0-fetch.sh read; the copy beside
  # the binary follows it
  echo "$(sha256_of "$PINNED")  fid$X  built from $(git rev-parse --short HEAD 2>/dev/null)$(git diff --quiet 2>/dev/null || echo '+changes') on $(date +%F)" > "$PIN_FILE"
  cp "$PIN_FILE" "$PINNED.sha256"
  echo "PINNED: $PINNED in $PIN_FILE ($(cut -c1-16 "$PIN_FILE")...)"
fi

# RELEASE=1: the stage-3 compiler (it compiled itself to the fixpoint) is
# the release binary. Check it runs and builds a program outside the
# repository too: an installed compiler finds stdlib/ next to itself.
if [ -n "${RELEASE:-}" ]; then
  if [ "$STAGES" -lt 3 ]; then echo "RELEASE needs 3 stages"; exit 1; fi
  mkdir -p release
  cp "$LAST" "release/fid$EXE"
  # The installed layout: fides(.exe) with stdlib/ beside it, started by
  # name from PATH, in a directory with no stdlib/
  smoke="$(pwd)/$OUT/smoke"
  rm -rf "$smoke"; mkdir -p "$smoke/install" "$smoke/work"
  cp "release/fid$EXE" "$smoke/install/fid$EXE"
  cp -r stdlib "$smoke/install/stdlib"
  cp tests/bootstrap/hello.fd "$smoke/work/hello.fd"
  expected=$("$STAGE0" run tests/bootstrap/hello.fd 2>/dev/null)
  got=$(cd "$smoke/work" && PATH="$smoke/install:$PATH" fid run hello.fd 2>/dev/null)
  if [ "$got" != "$expected" ]; then
    echo "RELEASE FAILED: release/fid$EXE run hello.fd printed '$got', expected '$expected'"
    exit 1
  fi
  echo "RELEASE OK: release/fid$EXE ($(wc -c < "release/fid$EXE") bytes)"
  # release-roadmap A10: stamp it (binary and source hashes, commit), so a
  # stale release/fid is told apart from this build (scripts/release-fresh.sh)
  bash scripts/release-fresh.sh --write "release/fid$EXE"
fi
