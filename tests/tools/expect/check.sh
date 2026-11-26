#!/usr/bin/env bash
# tools/expect.fd (roadmap AD4) does what scripts/verify/expect.py does:
# both run with the same arguments and the same compiler, and their output
# and exit status are compared ("\r\n" from Python on Windows counts as
# "\n").
#
#   bash tests/tools/expect/check.sh             # sample/ (about 4 min)
#   FULL=1 bash tests/tools/expect/check.sh      # + 40 conformance tests
#
# FID: the compiler (default release/fid.exe; a path Python can start too,
# so on Windows not a /c/... one); the tool is built with `$FID ccompile`
# (CC, default "zig cc"). OUT: where the tool and the outputs go.
# sample/ has //> lines that hold and ones that do not (an exit status, a
# line with a quote and a tab, a missing line), an `// Expected output:`
# block (trimmed, a `...` line, `// Expected exit:`), a panic, //! error:
# with the right message, another one and none, a `_fail.fd` file, an
# `expect-error` file that compiles, a file with no expectations on which
# the backends agree and one on which they do not, `//@ env:` and
# `//@ backends:` (the `checked` backend), a subfolder and a `_modules`
# folder (left out). Run from the repository root; needs python3 (or
# python).
set -u
FID="${FID:-release/fid.exe}"
export CC="${CC:-zig cc}"
OUT="${OUT:-${TMPDIR:-/tmp}/fid-expect-check}"
mkdir -p "$OUT"
PY=python3
command -v "$PY" > /dev/null 2>&1 || PY=python
"$PY" -c "import sys" > /dev/null 2>&1 || PY=python
exe="$OUT/expect"
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) exe="$exe.exe" ;; esac
if ! "$FID" ccompile tools/expect.fd -o "$exe" > "$OUT/build.txt" 2>&1; then
    echo "FAIL: tools/expect.fd does not build"; tail -5 "$OUT/build.txt"; exit 1
fi
S=tests/tools/expect/sample
pass=0
fail=0
compare() {   # name, then the arguments both get
    local name="$1"; shift
    "$PY" -X utf8 scripts/verify/expect.py "$@" > "$OUT/$name.py" 2>&1
    local pc=$?
    "$exe" "$@" > "$OUT/$name.fd" 2>&1
    local fc=$?
    if diff --strip-trailing-cr "$OUT/$name.py" "$OUT/$name.fd" > "$OUT/$name.diff" && [ $pc = $fc ]; then
        pass=$((pass + 1)); echo "ok   $name (exit $fc, $(wc -l < "$OUT/$name.fd") lines)"
    else
        fail=$((fail + 1)); echo "FAIL $name (exit $pc vs $fc)"; head -20 "$OUT/$name.diff"
    fi
}
compare sample --fid "$FID" -j 2 "$S"
compare sample-q-j1 --fid "$FID" --backends run -q -j 1 "$S"
compare glob --fid "$FID" --backends=run,ccompile -j2 "$S/ok_*.fd" "$S/wrong_out.fd" "$S/none*.fd"
compare one-file --fid "$FID" --back run "$S/sub/nested.fd"
compare no-paths --fid "$FID"
compare bad-j --fid "$FID" -j x "$S"
compare no-value "$S" --fid
compare unknown --fid "$FID" --nope "$S"
if [ "${FULL:-0}" = 1 ]; then
    compare conformance --fid "$FID" -j 2 $(ls tests/conformance/s1_*.fd | head -40)
fi
echo "expect port: $pass same, $fail differ"
[ $fail = 0 ]
