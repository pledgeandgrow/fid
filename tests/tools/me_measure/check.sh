#!/usr/bin/env bash
# tools/me_measure.fd (roadmap AD4) does what scripts/me-measure.sh does:
# both run with the same arguments and environment in a scratch copy of
# the tree (the script writes docs/perf/mid-end/<label>.txt under the
# current folder), and the output, exit status and that file are compared
# with the times masked (instruction counts and same/DIFF are compared).
#
#   bash tests/tools/me_measure/check.sh     # about 3 min
#
# FID: the compiler (default release/fid.exe); FID_HOME tells a compiler
# outside the tree where stdlib/ is. OUT: where the tool, the copy and the
# outputs go. Cases: hello, test_hashmap, a _fail test that does not build
# and a program that prints its own executable's name (DIFF) with the
# default OFF, then with OFF=dce,gvn; no label (bash's ${1:?} message).
set -u
FID="${FID:-release/fid.exe}"
export CC="${CC:-zig cc}"
OUT="${OUT:-${TMPDIR:-/tmp}/fid-me-measure-check}"
mkdir -p "$OUT"
X=""
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) X=".exe" ;; esac
exe="$OUT/me_measure$X"
if ! "$FID" ccompile tools/me_measure.fd -o "$exe" > "$OUT/build.txt" 2>&1; then
    echo "FAIL: tools/me_measure.fd does not build"; tail -5 "$OUT/build.txt"; exit 1
fi
FID="$(cd "$(dirname "$FID")" && pwd)/$(basename "$FID")"
T="$OUT/tree"
rm -rf "$T"
mkdir -p "$T/scripts" "$T/tests/bootstrap"
cp scripts/me-measure.sh "$T/scripts/"
cp tests/bootstrap/hello.fd tests/bootstrap/test_hashmap.fd tests/bootstrap/test_arg_type_fail.fd "$T/tests/bootstrap/" 2>/dev/null
cp tests/tools/expect/sample/noexpect_differs.fd "$T/tests/bootstrap/test_own_name.fd"
FILES="tests/bootstrap/hello.fd tests/bootstrap/test_hashmap.fd tests/bootstrap/test_own_name.fd"
[ -f "$T/tests/bootstrap/test_arg_type_fail.fd" ] && FILES="$FILES tests/bootstrap/test_arg_type_fail.fd"
pass=0
fail=0
# the times: columns 4-7 of a program line, and the ms of the totals
mask() {
    tr -d '\r' < "$1" | sed -E '/^#/!s/^([^ ]+ [^ ]+ [^ ]+) [^ ]+ [^ ]+ [^ ]+ [^ ]+ /\1 T T T T /' \
        | sed -E 's/iropt [0-9]+ ms vs [0-9]+ ms; builds [0-9]+ ms vs [0-9]+ ms/iropt T ms vs T ms; builds T ms vs T ms/'
}
compare() {   # name, label, then the arguments after it (environment from the caller)
    local name="$1"; shift
    rm -rf "$T/docs"
    (cd "$T" && bash scripts/me-measure.sh "$@") > "$OUT/$name.sh" 2>&1
    echo "exit $?" >> "$OUT/$name.sh"
    [ -n "${1:-}" ] && { cp "$T/docs/perf/mid-end/$1.txt" "$OUT/$name.sh.txt" 2>/dev/null || echo none > "$OUT/$name.sh.txt"; }
    rm -rf "$T/docs"
    (cd "$T" && "$exe" "$@") > "$OUT/$name.fd" 2>&1
    echo "exit $?" >> "$OUT/$name.fd"
    [ -n "${1:-}" ] && { cp "$T/docs/perf/mid-end/$1.txt" "$OUT/$name.fd.txt" 2>/dev/null || echo none > "$OUT/$name.fd.txt"; }
    [ -n "${1:-}" ] || { echo none > "$OUT/$name.sh.txt"; echo none > "$OUT/$name.fd.txt"; }
    if diff <(mask "$OUT/$name.sh") <(mask "$OUT/$name.fd") > "$OUT/$name.diff" \
       && diff <(mask "$OUT/$name.sh.txt") <(mask "$OUT/$name.fd.txt") >> "$OUT/$name.diff"; then
        pass=$((pass + 1)); echo "ok   $name ($(tail -1 "$OUT/$name.fd"), $(wc -l < "$OUT/$name.fd.txt") lines in the file)"
    else
        fail=$((fail + 1)); echo "FAIL $name"; head -20 "$OUT/$name.diff"
    fi
}
compare default sample "$FID" $FILES
OFF=dce,gvn compare off-dce-gvn sample2 "$FID" $FILES
compare no-label
echo "me_measure port: $pass same, $fail differ"
[ $fail = 0 ]
