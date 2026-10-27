#!/bin/bash
# bootstrap.sh — build the self-hosted compiler from scratch and check the fixpoint.
#
# Usage:
#   ./scripts/bootstrap.sh [out_dir] [stages]     (defaults: build/bootstrap, 3)
#   RELEASE=1 ./scripts/bootstrap.sh              also install the verified
#                                                 stage-3 compiler as
#                                                 release/fid(.exe) (what the
#                                                 installer ships)
#
# Stage 0 is the Rust bootstrap compiler (target/release/fidesc). Each stage N
# runs the previous compiler's `dump-c compiler/main.fd`, wraps `main` in a
# thread with a 256MB stack (the compiler recurses deeply), and compiles the C.
# The fixpoint holds when stage 2 and stage 3 emit identical C (compared with
# line endings normalized — stdout text mode differs between builds on Windows).
#
# C compiler: $CC if set, else the first of tcc, gcc, clang, `zig cc`.
# The generated C is one ~30MB translation unit; build it at -O0 (optimizing
# it needs >16GB of RAM with clang).
set -u
cd "$(dirname "$0")/.." || exit 1
OUT="${1:-build/bootstrap}"
STAGES="${2:-3}"
STAGE0="${STAGE0:-./target/release/fidesc}"
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
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) LIBS=(-lws2_32); EXE=".exe" ;; *) LIBS=(-lm -lpthread) ;; esac

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

# RELEASE=1: the stage-3 compiler (it compiled itself to the fixpoint) is
# the release binary. Check it runs and builds a program outside the
# repository too: an installed compiler finds stdlib/ next to itself.
if [ -n "${RELEASE:-}" ]; then
  if [ "$STAGES" -lt 3 ]; then echo "RELEASE needs 3 stages"; exit 1; fi
  mkdir -p release
  cp "$OUT/fides-c3$EXE" "release/fid$EXE"
  # The installed layout: fid(.exe) with stdlib/ beside it, started by
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
