#!/bin/bash
# build-selfhost.sh — build the self-hosted Fides compiler into release/.
#
# Output:
#   release/fid-dev.c    — generated C source (fidesc dump-c compiler/main.fd)
#   release/fid-dev.exe  — compiled binary (with 256MB-stack wrapper)
#
# Usage:
#   ./scripts/build-selfhost.sh            # dump-c + compile
#   STAGE0=./build/bootstrap/fides-c3 ./scripts/build-selfhost.sh   # self-compile
#
# C compiler: $CC if set, else the first of tcc, gcc, clang, `zig cc`.
# The generated C is one ~30MB translation unit; build at -O0 (optimizing
# needs >16GB RAM with clang).
set -u
cd "$(dirname "$0")/.." || exit 1
OUT=release
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
OUT_EXE=fid-dev
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) LIBS=(-lws2_32); OUT_EXE=fid-dev.exe ;; Darwin) LIBS=(-lm -lpthread); CFLAGS+=(-D_XOPEN_SOURCE=600 -D_DARWIN_C_SOURCE) ;; *) LIBS=(-lm -lpthread); case "${CC_CMD[*]}${CC:-}" in *zig*) LIBS+=(-lunwind) ;; esac ;; esac

echo "[1/3] $STAGE0 dump-c compiler/main.fd > $OUT/fid-dev.c"
if ! "$STAGE0" dump-c compiler/main.fd > "$OUT/fid-dev.c" 2> "$OUT/fid-dev.log"; then
  echo "dump-c failed:"; grep -E "error" "$OUT/fid-dev.log" | head -20; exit 1
fi
echo "      ($(wc -c < "$OUT/fid-dev.c") bytes)"

# Rename main and run it on a thread with a 256MB stack (compiler recurses deeply).
echo "[2/3] wrapping main with 256MB-stack thread"
sed 's/^int main(int argc, char\*\* argv)/int fides_main(int argc, char** argv)/' "$OUT/fid-dev.c" > "$OUT/fid-dev_wrapped.c"
cat >> "$OUT/fid-dev_wrapped.c" <<'EOF'

// === Stack wrapper (added by scripts/build-selfhost.sh) ===
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

echo "[3/3] ${CC_CMD[*]} -> $OUT/$OUT_EXE"
if ! "${CC_CMD[@]}" "${CFLAGS[@]}" "$OUT/fid-dev_wrapped.c" -o "$OUT/$OUT_EXE" "${LIBS[@]}" > "$OUT/fid-dev.cc.log" 2>&1; then
  echo "C compile failed:"; grep -E "error" "$OUT/fid-dev.cc.log" | head -20; exit 1
fi
rm -f "$OUT/fid-dev_wrapped.c"
echo "OK: $OUT/$OUT_EXE"
