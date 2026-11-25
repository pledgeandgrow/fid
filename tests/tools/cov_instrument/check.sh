#!/usr/bin/env bash
# tools/cov_instrument.fd (roadmap AD4) writes what
# scripts/verify/cov_instrument.py writes: both instrument the same C and
# the instrumented C (byte for byte), the counter map and the summary line
# are compared ("\r\n" from Python on Windows counts as "\n").
#
#   bash tests/tools/cov_instrument/check.sh     # seconds
#
# The C is `fid dump-c` of tests/bootstrap/hello.fd (core included, about
# 1,000 functions): plain (blocks only), with FID_C_LINE_MARKS=1 (`/*@L<n>*/`
# marks), the marks turned into `#line <n> "x.fd"` directives with every
# 997th line a `#line` back to a .c file (dump-c -g's form), and that with
# the directive after each block label dropped (a block starting on the
# line in effect gets a counter for it). Then --self-test and a missing
# argument (exit status only: the script prints a traceback).
#
# FID: the compiler that builds the tool and writes the C (default
# release/fid.exe; a path Python can start too). Run from the repository
# root; needs python3 (or python).
set -u
FID="${FID:-release/fid.exe}"
OUT="${OUT:-${TMPDIR:-/tmp}/fid-cov-instrument-check}"
mkdir -p "$OUT"
PY=python3
command -v "$PY" > /dev/null 2>&1 || PY=python
exe="$OUT/cov_instrument"
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) exe="$exe.exe" ;; esac
if ! "$FID" ccompile tools/cov_instrument.fd -o "$exe" > "$OUT/build.txt" 2>&1; then
    echo "FAIL: tools/cov_instrument.fd does not build"; tail -5 "$OUT/build.txt"; exit 1
fi
"$FID" dump-c tests/bootstrap/hello.fd > "$OUT/plain.c" 2> /dev/null
FID_C_LINE_MARKS=1 "$FID" dump-c tests/bootstrap/hello.fd > "$OUT/marks.c" 2> /dev/null
sed -E 's|^(\s*)/\*@L([0-9]+)\*/\s*$|#line \2 "tests/bootstrap/hello.fd"|' "$OUT/marks.c" \
    | awk 'NR % 997 == 0 { print "#line " NR " \"C:/build/out.c\"" } { print }' > "$OUT/hashline.c"
awk '/^ *bb[0-9]+: *$/ { print; skip = 1; next } skip && /^#line / { skip = 0; next } { skip = 0; print }' \
    "$OUT/hashline.c" > "$OUT/sparse.c"
pass=0
fail=0
for name in plain marks hashline sparse; do
    "$PY" scripts/verify/cov_instrument.py "$OUT/$name.c" "$OUT/$name.py.c" > "$OUT/$name.py.txt" 2>&1
    pc=$?
    "$exe" "$OUT/$name.c" "$OUT/$name.fd.c" > "$OUT/$name.fd.txt" 2>&1
    fc=$?
    bad=0
    cmp -s "$OUT/$name.py.c" "$OUT/$name.fd.c" || { bad=1; echo "  the instrumented C differs"; }
    diff --strip-trailing-cr "$OUT/$name.py.c.map" "$OUT/$name.fd.c.map" > /dev/null || { bad=1; echo "  the map differs"; }
    diff --strip-trailing-cr "$OUT/$name.py.txt" "$OUT/$name.fd.txt" > /dev/null || { bad=1; echo "  the output differs"; }
    [ $pc = $fc ] || bad=1
    if [ $bad = 0 ]; then
        pass=$((pass + 1)); echo "ok   $name (exit $fc): $(head -1 "$OUT/$name.fd.txt")"
    else
        fail=$((fail + 1)); echo "FAIL $name (exit $pc vs $fc)"
    fi
done
"$PY" scripts/verify/cov_instrument.py --self-test > "$OUT/self.py" 2>&1; pc=$?
"$exe" --self-test > "$OUT/self.fd" 2>&1; fc=$?
if diff --strip-trailing-cr "$OUT/self.py" "$OUT/self.fd" > /dev/null && [ $pc = $fc ]; then
    pass=$((pass + 1)); echo "ok   self-test (exit $fc)"
else
    fail=$((fail + 1)); echo "FAIL self-test (exit $pc vs $fc)"
fi
"$PY" scripts/verify/cov_instrument.py "$OUT/plain.c" > /dev/null 2>&1; pc=$?
"$exe" "$OUT/plain.c" > /dev/null 2>&1; fc=$?
if [ $pc = $fc ]; then
    pass=$((pass + 1)); echo "ok   missing argument (exit $fc)"
else
    fail=$((fail + 1)); echo "FAIL missing argument (exit $pc vs $fc)"
fi
echo "cov_instrument port: $pass same, $fail differ"
[ $fail = 0 ]
