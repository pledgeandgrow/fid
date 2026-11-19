#!/usr/bin/env bash
# Build the library with `fid ccompile --lib` (CI7), link the C host, run it
# and compare with expected.txt. FID: the compiler (default fid), CC: the C
# compiler (default cc). Run from this directory.
set -euo pipefail
FID="${FID:-fid}"
CC="${CC:-cc}"
cd "$(dirname "$0")"
case "$(uname -s)" in
    MINGW*|MSYS*|CYGWIN*) lib=mathlib.dll; link="mathlib.dll" ;;
    Darwin*) lib=libmathlib.dylib; link="-L. -lmathlib" ;;
    *) lib=libmathlib.so; link="-L. -lmathlib -Wl,-rpath,." ;;
esac
"$FID" ccompile mathlib.fd --lib -o "$lib"
test -f mathlib.h
$CC -o host host.c $link
# (Windows' C runtime writes CR LF: compared without the CRs)
./host | tr -d '\r' > out.txt
diff -u expected.txt out.txt
echo "embed: ok"
