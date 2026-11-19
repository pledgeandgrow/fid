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
# before compiling it (coverage, VF8), EXTRA_CFLAGS="..." is added after the
# fixed flags (sanitizers: scripts/sanitize.sh COMPILER=1, roadmap RL15).
#
# The generated C is split into SPLIT units at -O2 (default 8, see below)
# or built as one ~70 MB translation unit at -O0 (optimizing one unit needs
# more than 16 GB of RAM).
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
# DUMP_C_FLAGS (VF8): more flags for that dump-c, e.g. `-g` for #line
# directives where dump-c takes it (line coverage; FID_C_LINE_MARKS=1 in
# the environment is the other way, read by the C generator itself)
( cd "$WORK" && "$FID" dump-c compiler/main.fd ${DUMP_C_FLAGS:-} > "$C" 2> "${OUT%.exe}.log" ) || {
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
WFLAGS=(-w -fno-sanitize=all -Wno-error -Wno-int-conversion
  -Wno-incompatible-pointer-types -Wno-implicit-function-declaration -Wno-return-type)
# Nov 17 (VF4): SPLIT=n (default 8 for a plain variant) splits the C with
# `$FID split-c` into n units built at -O2, JOBS (default 4) at a time, as
# scripts/build-selfhost.sh does: the variant then runs about as fast as
# release/fid.exe instead of ~17x slower at -O0, which is what makes a
# whole-suite run of each PEIR_* variant possible. An instrumented
# (INSTRUMENT) or EXTRA_CFLAGS build stays one unit at -O0 unless SPLIT is
# set; a failed split falls back to one unit at -O0. The generated C is the
# same either way.
if [ -n "${INSTRUMENT:-}" ] || [ -n "${EXTRA_CFLAGS:-}" ]; then SPLIT="${SPLIT:-1}"; else SPLIT="${SPLIT:-8}"; fi
JOBS="${JOBS:-4}"
built=0
: > "${OUT%.exe}.cc.log"
U="${OUT%.exe}_u"
case "${CC_CMD[*]}" in *tcc*) SPLIT=1 ;; esac
if [ "$SPLIT" -gt 1 ] && "$FID" split-c "$W" "$U" "$SPLIT" >> "${OUT%.exe}.cc.log" 2>&1 && [ -f "$U.h" ]; then
  objs=(); pids=(); ok=1
  for k in $(seq 0 $((SPLIT - 1))); do
    "${CC_CMD[@]}" -O2 -fno-strict-aliasing -fwrapv "${WFLAGS[@]}" ${EXTRA_CFLAGS:-} \
      -c "${U}_$k.c" -o "${U}_$k.o" >> "${OUT%.exe}.cc.log" 2>&1 &
    pids+=($!)
    objs+=("${U}_$k.o")
    if [ ${#pids[@]} -ge "$JOBS" ]; then
      wait "${pids[0]}" || ok=0
      pids=("${pids[@]:1}")
    fi
  done
  for pid in "${pids[@]}"; do wait "$pid" || ok=0; done
  if [ "$ok" = 1 ] && "${CC_CMD[@]}" ${EXTRA_CFLAGS:-} "${objs[@]}" -o "$OUT" "${LIBS[@]}" >> "${OUT%.exe}.cc.log" 2>&1; then
    built=1
    echo "($SPLIT units at -O2)"
  else
    echo "split build failed (see ${OUT%.exe}.cc.log); building one unit at -O0"
  fi
  rm -f "${U}"_*.c "${U}"_*.o "$U.h"
fi
if [ "$built" = 0 ]; then
  "${CC_CMD[@]}" -O0 "${WFLAGS[@]}" \
    ${EXTRA_CFLAGS:-} "$W" -o "$OUT" "${LIBS[@]}" >> "${OUT%.exe}.cc.log" 2>&1 || {
    echo "C compile failed: $(grep -m5 error "${OUT%.exe}.cc.log")"; exit 1; }
fi
rm -f "$W" "$C.inst"
[ -n "${KEEP_TREE:-}" ] || rm -rf "$WORK"
echo "built $OUT"
