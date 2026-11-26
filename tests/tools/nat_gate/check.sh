#!/usr/bin/env bash
# tools/nat_gate.fd (roadmap AD4) does what scripts/nat-gate.sh does: both
# run with the same arguments and environment in a scratch copy of the
# tree (scripts/ and three of the gate's subset of tests, so that a gate
# takes a minute), and their output and exit status are compared.
#
#   bash tests/tools/nat_gate/check.sh       # about 5 min
#
# FID: the compiler (default release/fid.exe); FID_HOME tells a compiler
# outside the tree where stdlib/ is. OUT: where the tool, the copy and the
# outputs go. A stand-in z3 (tests/tools/rules_smt/fakesolver.fd) is put
# first on PATH, so the rules are "proved" in seconds, 27 of them not.
# Cases: x86_64-windows (`emit --nat`); x86_64-windows with REQUIRE_Z3=1
# and neither z3 nor Python on PATH (and a test whose `// Expected
# output:` is wrong: cross-test.sh compares with the interpreter, so it
# still passes); ALL=1 JOBS=1. On a Linux host the target is x86_64-linux.
set -u
FID="${FID:-release/fid.exe}"
export CC="${CC:-zig cc}"
OUT="${OUT:-${TMPDIR:-/tmp}/fid-nat-gate-check}"
mkdir -p "$OUT/z3"
X=""
TG=x86_64-linux
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) X=".exe"; TG=x86_64-windows ;; esac
exe="$OUT/nat_gate$X"
for t in "tools/nat_gate.fd:$exe" "tests/tools/rules_smt/fakesolver.fd:$OUT/z3/z3$X"; do
    if ! "$FID" ccompile "${t%%:*}" -o "${t#*:}" > "$OUT/build.txt" 2>&1; then
        echo "FAIL: ${t%%:*} does not build"; tail -5 "$OUT/build.txt"; exit 1
    fi
done
FID="$(cd "$(dirname "$FID")" && pwd)/$(basename "$FID")"
T="$OUT/tree"
rm -rf "$T"
mkdir -p "$T/tests/bootstrap"
cp -r scripts "$T/"
cp tests/bootstrap/test_hashmap.fd tests/bootstrap/test_match.fd tests/bootstrap/test_str.fd "$T/tests/bootstrap/"
pass=0
fail=0
compare() {   # name, then the arguments both get (environment from the caller)
    local name="$1"; shift
    rm -rf "$T/build"
    (cd "$T" && FID="$FID" bash scripts/nat-gate.sh "$@") > "$OUT/$name.sh" 2>&1
    echo "exit $?" >> "$OUT/$name.sh"
    rm -rf "$T/build"
    (cd "$T" && FID="$FID" "$exe" "$@") > "$OUT/$name.fd" 2>&1
    echo "exit $?" >> "$OUT/$name.fd"
    if diff <(tr -d '\r' < "$OUT/$name.sh") <(tr -d '\r' < "$OUT/$name.fd") > "$OUT/$name.diff"; then
        pass=$((pass + 1)); echo "ok   $name ($(tail -1 "$OUT/$name.fd"), $(wc -l < "$OUT/$name.fd") lines)"
    else
        fail=$((fail + 1)); echo "FAIL $name"; head -20 "$OUT/$name.diff"
    fi
}
PATH="$OUT/z3:$PATH" JOBS=2 compare fake-z3 $TG
cp tests/tools/nat_gate/wrong_str.fd "$T/tests/bootstrap/test_str.fd"
PATH=/usr/bin REQUIRE_Z3=1 JOBS=2 compare no-z3-fail $TG
PATH="$OUT/z3:$PATH" ALL=1 JOBS=1 compare all $TG
echo "nat_gate port: $pass same, $fail differ"
[ $fail = 0 ]
