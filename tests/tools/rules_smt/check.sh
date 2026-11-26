#!/usr/bin/env bash
# tools/rules_smt.fd (roadmap AD4) does what scripts/rules-smt.sh does:
# both run with the same arguments in a scratch copy of the tree (the
# script writes build/rules-smt beside its scripts/ folder), and their
# output and exit status are compared.
#
#   bash tests/tools/rules_smt/check.sh      # about 3 min (z3 through Python)
#
# FID: the compiler (default release/fid.exe, as an absolute path); the
# tool is built with `$FID ccompile` (CC, default "zig cc"). OUT: where the
# tool, the copy and the outputs go. Cases: the default solver (z3, or
# z3's Python module when there is no z3 command: every obligation is run;
# skipped when neither is there); fakesolver.fd, a solver that leaves some
# unproved (unknown, sat with CRLF; the exit status is their number); no
# solver and no Python on PATH (exit 2); a compiler that fails (`false`:
# "fid test-rules failed"); one that writes no obligations (the glob's
# pattern itself is then the one file, as in bash).
set -u
FID="${FID:-release/fid.exe}"
export CC="${CC:-zig cc}"
OUT="${OUT:-${TMPDIR:-/tmp}/fid-rules-smt-check}"
mkdir -p "$OUT"
X=""
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) X=".exe" ;; esac
exe="$OUT/rules_smt$X"
for t in "tools/rules_smt.fd:$exe" "tests/tools/rules_smt/fakesolver.fd:$OUT/fakesolver$X"; do
    if ! "$FID" ccompile "${t%%:*}" -o "${t#*:}" > "$OUT/build.txt" 2>&1; then
        echo "FAIL: ${t%%:*} does not build"; tail -5 "$OUT/build.txt"; exit 1
    fi
done
FID="$(cd "$(dirname "$FID")" && pwd)/$(basename "$FID")"
T="$OUT/tree"
mkdir -p "$T/scripts"
cp scripts/rules-smt.sh "$T/scripts/"
pass=0
fail=0
compare() {   # name, then the arguments both get (environment from the caller)
    local name="$1"; shift
    rm -rf "$T/build"
    (cd "$T" && bash scripts/rules-smt.sh "$@") > "$OUT/$name.sh" 2>&1
    local sc=$?
    rm -rf "$T/build"
    (cd "$T" && "$exe" "$@") > "$OUT/$name.fd" 2>&1
    local fc=$?
    if diff --strip-trailing-cr "$OUT/$name.sh" "$OUT/$name.fd" > "$OUT/$name.diff" && [ $sc = $fc ]; then
        pass=$((pass + 1)); echo "ok   $name (exit $fc, $(wc -l < "$OUT/$name.fd") lines)"
    else
        fail=$((fail + 1)); echo "FAIL $name (exit $sc vs $fc)"; head -20 "$OUT/$name.diff"
    fi
}
if command -v z3 > /dev/null 2>&1 || python3 -c "import z3" > /dev/null 2>&1 || python -c "import z3" > /dev/null 2>&1; then
    compare z3 "$FID"
else
    echo "skip z3 (no z3 and no z3 module)"
fi
compare fake-solver "$FID" "$OUT/fakesolver$X"
PATH=/usr/bin compare no-solver "$FID" no-such-solver
compare fid-fails false
compare no-obligations "$OUT/fakesolver$X" "$OUT/fakesolver$X"
echo "rules_smt port: $pass same, $fail differ"
[ $fail = 0 ]
