#!/usr/bin/env bash
# tools/doctest.fd (roadmap AD4) prints what scripts/doctest.py prints:
# both run over the same folders with the same compiler and their outputs
# are compared ("\r\n" from Python on Windows counts as "\n").
#
#   bash tests/tools/doctest/check.sh            # sample/ (seconds)
#   FULL=1 bash tests/tools/doctest/check.sh     # + all of stdlib/ (long)
#
# FID: the compiler the examples run with and that builds the tool
# (default release/fid.exe; a path Python can start too, so on Windows not
# a /c/... one); the tool is built with `$FID ccompile`.
# sample/samples.fd has an example that passes, one whose assertion fails,
# one that panics, one with its own main and a use line, one that does not
# compile, a native-only one (run with --emit) and one that returns early.
# Run from the repository root; needs python3 (or python).
set -u
FID="${FID:-release/fid.exe}"
OUT="${OUT:-${TMPDIR:-/tmp}/fid-doctest-check}"
mkdir -p "$OUT"
PY=python3
command -v "$PY" > /dev/null 2>&1 || PY=python
exe="$OUT/doctest"
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) exe="$exe.exe" ;; esac
if ! "$FID" ccompile tools/doctest.fd -o "$exe" > "$OUT/build.txt" 2>&1; then
    echo "FAIL: tools/doctest.fd does not build"; tail -5 "$OUT/build.txt"; exit 1
fi
pass=0
fail=0
compare() {   # name, then the arguments both get
    local name="$1"; shift
    "$PY" scripts/doctest.py "$@" > "$OUT/$name.py" 2>&1
    local pc=$?
    "$exe" "$@" > "$OUT/$name.fd" 2>&1
    local fc=$?
    if diff --strip-trailing-cr "$OUT/$name.py" "$OUT/$name.fd" > "$OUT/$name.diff" && [ $pc = $fc ]; then
        pass=$((pass + 1)); echo "ok   $name (exit $fc, $(wc -l < "$OUT/$name.fd") lines)"
    else
        fail=$((fail + 1)); echo "FAIL $name (exit $pc vs $fc)"; head -20 "$OUT/$name.diff"
    fi
}
compare sample tests/tools/doctest/sample --fid "$FID" -v
compare sample-emit tests/tools/doctest/sample --fid "$FID" --emit
compare sample-native-only tests/tools/doctest/sample --fid "$FID" --emit --native-only
if [ "${FULL:-0}" = 1 ]; then
    compare stdlib stdlib --fid "$FID"
fi
echo "doctest port: $pass same, $fail differ"
[ $fail = 0 ]
