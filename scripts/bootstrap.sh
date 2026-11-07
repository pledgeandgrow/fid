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
# The generated C is one ~50MB translation unit; build it at -O0 (optimizing
# it needs >16GB of RAM with clang).
set -u
cd "$(dirname "$0")/.." || exit 1
OUT="${1:-build/bootstrap}"
STAGES="${2:-3}"
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) X=".exe" ;; *) X="" ;; esac
PINNED="release/stage0/fid$X"
if [ -z "${STAGE0:-}" ]; then
  if [ -x "$PINNED" ]; then
    STAGE0="$PINNED"
    if [ -f scripts/stage0.sha256 ] && command -v sha256sum >/dev/null; then
      want=$(cut -d' ' -f1 scripts/stage0.sha256)
      have=$(sha256sum "$PINNED" | cut -d' ' -f1)
      if [ "$want" != "$have" ]; then
        echo "warning: $PINNED does not match scripts/stage0.sha256 (pin it again with PIN=1)"
      fi
    fi
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
CFLAGS=(-O0 -w)
case "${CC_CMD[*]}" in
  *tcc*) ;;
  *) CFLAGS+=(-fno-sanitize=all -Wno-error -Wno-int-conversion -Wno-incompatible-pointer-types
              -Wno-implicit-function-declaration -Wno-return-type) ;;
esac
LIBS=()
EXE=""
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) LIBS=(-lws2_32); EXE=".exe" ;; Darwin) LIBS=(-lm -lpthread); CFLAGS+=(-D_XOPEN_SOURCE=600 -D_DARWIN_C_SOURCE) ;; *) LIBS=(-lm -lpthread); case "${CC_CMD[*]}${CC:-}" in *zig*) LIBS+=(-lunwind) ;; esac ;; esac

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

prev="$STAGE0"
if [ -n "${NATIVE:-}" ]; then
  for n in $(seq 1 "$STAGES"); do
    start=$(date +%s)
    if ! "$prev" emit compiler/main.fd -o "$OUT/fid-n$n$EXE" > "$OUT/n$n.log" 2>&1; then
      echo "[stage $n] emit failed:"; grep -iE "error" "$OUT/n$n.log" | head -20; exit 1
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
for n in $(seq 1 "$STAGES"); do
  start=$(date +%s)
  if ! "$prev" dump-c compiler/main.fd > "$OUT/c$n.c" 2> "$OUT/c$n.log"; then
    echo "[stage $n] dump-c failed:"; grep -E "error" "$OUT/c$n.log" | head -20; exit 1
  fi
  echo "[stage $n] dump-c ok in $(( $(date +%s) - start ))s ($(wc -c < "$OUT/c$n.c") bytes)"
  wrap "$OUT/c$n.c" "$OUT/c${n}_wrapped.c"
  start=$(date +%s)
  if ! "${CC_CMD[@]}" "${CFLAGS[@]}" "$OUT/c${n}_wrapped.c" -o "$OUT/fides-c$n$EXE" "${LIBS[@]}" > "$OUT/c$n.cc.log" 2>&1; then
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
# it was built from are recorded in scripts/stage0.sha256.
if [ -n "${PIN:-}" ]; then
  if [ "$STAGES" -lt 3 ]; then echo "PIN needs 3 stages (a checked fixpoint)"; exit 1; fi
  mkdir -p release/stage0
  cp "$LAST" "$PINNED"
  echo "$(sha256sum "$PINNED" | cut -d' ' -f1)  fid$X  built from $(git rev-parse --short HEAD 2>/dev/null)$(git diff --quiet 2>/dev/null || echo '+changes') on $(date +%F)" > scripts/stage0.sha256
  echo "PINNED: $PINNED ($(cut -c1-16 scripts/stage0.sha256)...)"
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
fi
