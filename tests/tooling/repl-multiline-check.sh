#!/bin/bash
# L7G multi-line input (docs/roadmap/roadmap.md, REPL improvements): `fid repl`
# keeps reading lines while a `(`, `[` or `{` is open (compiler/main.fd
# cmd_repl, repl_open_depth), so a function can be typed over several lines.
#   FID=release/fid.exe bash tests/tooling/repl-multiline-check.sh
# Run from the repository root. Exit 0 when every expected line is there.
# Calling the function runs it through C: that part is checked only when tcc
# (Windows) or gcc (elsewhere) is on PATH.
FID=${FID:-release/fid.exe}
OUT=$(mktemp)
ERR=$(mktemp)
fail=0
printf '%s\n' \
    'fn tri(n: i64) -> i64 {' \
    '    var s: i64 = 0;' \
    '    var i: i64 = 1;' \
    '    while i <= n {' \
    '        s = s + i; // a } in a comment does not close' \
    '        i = i + 1;' \
    '    }' \
    '    return s;' \
    '}' \
    'fn label(n: i64) -> str { return "n=(" + int_to_str(n) + "{"; }' \
    ':type tri(4)' \
    ':type label(' \
    ':defs' \
    'tri(' \
    '  10' \
    ')' \
    'label(3)' \
    'fn broken(x: i64) -> i64 {' \
    '' \
    ':type tri(1) + 1' \
    ':quit' | "$FID" repl > "$OUT" 2> "$ERR"
has() {
    if grep -qF -- "$2" "$OUT"; then echo "ok   $1"; else echo "FAIL $1: no line with: $2"; fail=1; fi
}
if [ "$(grep -cxF defined "$OUT")" -eq 2 ]; then echo "ok   two definitions kept"; else echo "FAIL two definitions kept"; fail=1; fi
has "multi-line function checks" "tri(4) : i64"
has "brackets in a string do not count" 'fn label(n: i64) -> str { return "n=(" + int_to_str(n) + "{"; }'
has ":defs keeps the lines" "    while i <= n {"
has ":defs keeps the closing brace" "    return s;"
has "session goes on after an abandoned input" "tri(1) + 1 : i64"
if grep -qF "input abandoned" "$ERR"; then echo "ok   empty line abandons an open input"; else echo "FAIL empty line abandons an open input"; fail=1; fi
if grep -qF "Usage: :type" "$ERR" || grep -qF "label( :" "$OUT" || grep -qi "error" "$ERR"; then echo "ok   a command stays one line"; else echo "FAIL a command stays one line"; fail=1; fi
cc=gcc
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) cc=tcc ;; esac
if command -v $cc > /dev/null 2>&1; then
    if grep -qxF 55 "$OUT"; then echo "ok   multi-line call runs: 55"; else echo "FAIL multi-line call runs: no 55"; fail=1; fi
    if grep -qxF "n=(3{" "$OUT"; then echo "ok   string with brackets runs"; else echo "FAIL string with brackets runs"; fail=1; fi
else
    echo "skip running the calls: no $cc"
fi
if [ $fail -ne 0 ]; then
    echo "--- repl output ---"
    cat "$OUT"
    echo "--- repl stderr ---"
    cat "$ERR"
fi
rm -f "$OUT" "$ERR"
exit $fail
