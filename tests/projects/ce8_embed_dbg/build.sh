#!/usr/bin/env bash
# CE8: build dbglib.fd with `fid ccompile --lib -g`, check that the
# generated C restores the frame record in fides_call on both return paths
# (the longjmp of a caught panic and the normal return), link host.c, run
# it and compare with expected.txt. FID: the compiler (default fid), CC:
# the C compiler (default cc). Run from anywhere. Exit 1 on a failure.
set -euo pipefail
FID="${FID:-fid}"
CC="${CC:-cc}"
cd "$(dirname "$0")"
case "$(uname -s)" in
    MINGW*|MSYS*|CYGWIN*) lib=dbglib.dll; link="dbglib.dll" ;;
    Darwin*) lib=libdbglib.dylib; link="-L. -ldbglib" ;;
    *) lib=libdbglib.so; link="-L. -ldbglib -Wl,-rpath,." ;;
esac
"$FID" ccompile dbglib.fd --lib -g -o "$lib"
test -f dbglib.h
c="$lib.c"
test -f "$c"
grep -q '^#define P_DEBUG_FRAMES 1' "$c" || { echo "FAIL: -g did not turn the frame records on"; exit 1; }
grep -q 'P_DBG_EMBED_SAVE;' "$c" || { echo "FAIL: fides_call does not keep the frame record"; exit 1; }
grep -q 'if (setjmp(jb) != 0) { P_DBG_EMBED_RESTORE;' "$c" || { echo "FAIL: a caught panic does not restore the frame record"; exit 1; }
n=$(grep -c 'P_DBG_EMBED_RESTORE;' "$c")
[ "$n" -ge 2 ] || { echo "FAIL: the normal return does not restore the frame record"; exit 1; }
$CC -o host host.c $link
./host > out.txt
diff -u --strip-trailing-cr expected.txt out.txt
echo "ce8_embed_dbg: ok"
