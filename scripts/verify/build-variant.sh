#!/bin/bash
# build-variant.sh — build a compiler from a copy of the source tree, with
# optional source edits, into one executable. The tool behind the pass
# on/off runner (VF4), mutation testing (VF7), coverage (VF8) and the
# reproducibility check (VF12).
#
#   scripts/verify/build-variant.sh <tree> <out.exe> [edit ...]
#
#   <tree>    a directory holding compiler/ and stdlib/ (a snapshot; it is
#             copied, never edited in place)
#   <out.exe> the compiler to write; <out>.c keeps the generated C
#   edit      FILE:FROM=>TO — replace the text FROM with TO in FILE
#             (relative to the tree), once, or the build fails. Example:
#             compiler/iropt.fd:const IRO_LICM: bool = true;=>const IRO_LICM: bool = false;
#
# Environment: FID (the compiler that builds this one; default release/fid.exe),
# CC (default `zig cc`), KEEP_TREE=1 keeps the edited copy beside <out>,
# INSTRUMENT=<script> runs `python <script> <in.c> <out.c>` over the C
# before compiling it (coverage, VF8).
#
# The generated C is built at -O0 like scripts/bootstrap.sh (one ~50 MB
# translation unit; optimizing it needs more than 16 GB of RAM).
set -u
TREE="$1"; OUT="$2"; shift 2
FID="${FID:-release/fid.exe}"
case "$FID" in /*|?:*) ;; *) FID="$(pwd)/$FID" ;; esac
case "$OUT" in /*|?:*) ;; *) OUT="$(pwd)/$OUT" ;; esac
CC_CMD=(${CC:-zig cc})
WORK="${OUT%.exe}.tree"
rm -rf "$WORK"
mkdir -p "$WORK"
cp -r "$TREE/compiler" "$TREE/stdlib" "$WORK/"

for e in "$@"; do
  file="${e%%:*}"; rest="${e#*:}"; from="${rest%%=>*}"; to="${rest#*=>}"
  python - "$WORK/$file" "$from" "$to" <<'PY' || { echo "edit failed: $e"; exit 2; }
import sys
path, frm, to = sys.argv[1], sys.argv[2], sys.argv[3]
src = open(path, encoding="utf-8", newline="").read()
n = src.count(frm)
if n != 1:
    sys.exit(f"{path}: '{frm}' found {n} times (need exactly 1)")
open(path, "w", encoding="utf-8", newline="").write(src.replace(frm, to))
PY
done

C="${OUT%.exe}.c"
( cd "$WORK" && "$FID" dump-c compiler/main.fd > "$C" 2> "${OUT%.exe}.log" ) || {
  echo "dump-c failed: $(grep -m5 -i error "${OUT%.exe}.log")"; exit 1; }
if [ -n "${INSTRUMENT:-}" ]; then
  python "$INSTRUMENT" "$C" "$C.inst" || exit 1
  SRC="$C.inst"
else
  SRC="$C"
fi
W="${OUT%.exe}_wrapped.c"
sed 's/^int main(int argc, char\*\* argv)/int fides_main(int argc, char** argv)/' "$SRC" > "$W"
cat >> "$W" <<'EOF'

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
LIBS=()
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) LIBS=(-lws2_32) ;; *) LIBS=(-lm -lpthread) ;; esac
"${CC_CMD[@]}" -O0 -w -fno-sanitize=all -Wno-error -Wno-int-conversion \
  -Wno-incompatible-pointer-types -Wno-implicit-function-declaration -Wno-return-type \
  "$W" -o "$OUT" "${LIBS[@]}" > "${OUT%.exe}.cc.log" 2>&1 || {
  echo "C compile failed: $(grep -m5 error "${OUT%.exe}.cc.log")"; exit 1; }
rm -f "$W" "$C.inst"
[ -n "${KEEP_TREE:-}" ] || rm -rf "$WORK"
echo "built $OUT"
