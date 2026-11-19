#!/bin/bash
# roadmap L7E (style lints: naming conventions, import ordering): `fid lint`
# reports names that break the conventions and unsorted `use` lines
# (compiler/compiler.fd lint_source_scan, lx_lint_uses, lx_lint_fn,
# lx_to_snake/lx_to_camel/lx_upper), and nothing for the names the
# conventions allow.
#   FID=release/fid.exe bash tests/tooling/lint-style-check.sh
#   STDLIB=1    also run scripts/lint-check.sh (every stdlib module against
#               scripts/lint-check.baseline; a few minutes)
#   EXAMPLES=1  also run scripts/examples-check.sh (lint + run of the five
#               examples)
# Run from the repository root. Checks:
#   1. tests/bootstrap/test_style_lint.fd: exactly the five style warnings
#      its header lists, in that order; the unused-import warning for
#      'string'; nothing about PointPair_first (a Type_method free
#      function), LIMIT_DEFAULT() (a constant-like function) or the names
#      that follow the conventions; "6 lint warning(s)."; the program still
#      prints 7 and 3
#   2. tests/tooling/lint_style/edge.fd: exactly the style warnings its
#      header lists (HTTPServer -> http_server, a cap, params and loop
#      variables, a second import group, @repr(C), `_` names, cap method
#      parameters); the program prints 16 5 17 86 17
#   3. scripts/verify/lint-scan-predict.py gives the same style warnings as
#      fid on both files and on the five examples
#   4. the five examples of scripts/examples-check.sh have no lint warnings
# Exit 0 when every check holds.
FID=${FID:-release/fid.exe}
PY=${PY:-python3}
command -v "$PY" > /dev/null 2>&1 || PY=python
T=tests/bootstrap/test_style_lint.fd
E=tests/tooling/lint_style/edge.fd
fail=0
chk() {
    if [ "$2" = 1 ]; then echo "ok   $1"; else echo "FAIL $1"; fail=1; fi
}
style_lines() {
    grep -E "^warning: line [0-9]+: ((function|variable|parameter|struct|enum|cap|trait|constant) '[^']*'.* should be |imports are not sorted)"
}
# the header's "//   line N: ..." lines, as `fid lint` prints them
header_lines() {
    tr -d '\r' < "$1" | grep -E "^//   line [0-9]+: " | sed 's#^//   #warning: #'
}
run_out() { "$FID" run "$1" 2>/dev/null | tr -d '\r' | grep -v '^\['; }

# 1. test_style_lint.fd
OUT=$("$FID" lint "$T" 2>&1); code=$?
OUT=$(echo "$OUT" | tr -d '\r')
chk "$T: lint succeeds" "$([ $code = 0 ] && echo 1)"
S=$(echo "$OUT" | style_lines)
H=$(header_lines "$T")
chk "$T: five style warnings" "$([ "$(echo "$S" | grep -c .)" = 5 ] && echo 1)"
# the header writes the import line without its final '.' and Fix: text:
# each header line must start the warning of the same rank
same=1
for n in 1 2 3 4 5; do
    h=$(echo "$H" | sed -n "${n}p"); s=$(echo "$S" | sed -n "${n}p")
    case "$s" in
        "$h"*) ;;
        *) same=0; echo "     expected: $h"; echo "     got:      $s" ;;
    esac
done
chk "$T: the header's five warnings, in order" "$same"
chk "$T: the import warning has its Fix: line" \
    "$(echo "$OUT" | grep -qF "    Fix: sort the use lines of this group by path (core, vec and string first)." && echo 1)"
chk "$T: the unused import 'string' is reported" \
    "$(echo "$OUT" | grep -q "^warning: unused import: 'string'" && echo 1)"
quiet=1
for nm in PointPair_first LIMIT_DEFAULT main "'p'" "'a'" "'b'"; do
    if echo "$S" | grep -qF "$nm"; then quiet=0; echo "     (reported: $nm)"; fi
done
chk "$T: nothing about Type_method, constant-like functions or good names" "$quiet"
chk "$T: the summary says 6" "$(echo "$OUT" | grep -q "^6 lint warning(s)" && echo 1)"
chk "$T: still runs (7, 3)" "$([ "$(run_out "$T")" = "$(printf '7\n3')" ] && echo 1)"

# 2. edge.fd
OUT2=$("$FID" lint "$E" 2>&1); code2=$?
OUT2=$(echo "$OUT2" | tr -d '\r')
chk "$E: lint succeeds" "$([ $code2 = 0 ] && echo 1)"
S2=$(echo "$OUT2" | style_lines)
H2=$(header_lines "$E")
if [ "$S2" = "$H2" ]; then
    chk "$E: exactly the header's $(echo "$H2" | grep -c .) style warnings, in order" 1
else
    chk "$E: exactly the header's style warnings, in order" 0
    diff <(echo "$H2") <(echo "$S2") | head -20 | sed 's/^/     /'
fi
quiet2=1
for nm in "'HTTPServer'" "HTTPServer_port'" DEFAULT_PORT _InternalHelper _skipMe "'c_point'" "'Shade'" "'MAX_LEN'" "scaleBy" "'use bits'"; do
    if echo "$S2" | grep -qF "$nm"; then quiet2=0; echo "     (reported: $nm)"; fi
done
chk "$E: nothing about the names the conventions allow" "$quiet2"
chk "$E: still runs (16 5 17 86 17)" "$([ "$(run_out "$E")" = "$(printf '16\n5\n17\n86\n17')" ] && echo 1)"

# 3. the Python copy of the scan agrees with fid
for f in "$T" "$E" examples/lexer.fd examples/parser.fd examples/json_parser.fd examples/codegen.fd examples/mini_compiler.fd; do
    [ -f "$f" ] || { echo "skip $f (missing)"; continue; }
    A=$("$FID" lint "$f" 2>&1 | tr -d '\r' | style_lines)
    B=$("$PY" scripts/verify/lint-scan-predict.py --kinds naming,imports "$f" | tr -d '\r' | style_lines)
    if [ "$A" = "$B" ]; then
        chk "predictor = fid on $f ($(echo "$A" | grep -c .) style warnings)" 1
    else
        chk "predictor = fid on $f" 0
        diff <(echo "$A") <(echo "$B") | head -10 | sed 's/^/     /'
    fi
done

# 4. the examples gate (lint part)
for e in lexer parser json_parser codegen mini_compiler; do
    chk "examples/$e.fd: No lint warnings" \
        "$("$FID" lint "examples/$e.fd" 2>&1 | tr -d '\r' | grep -q "^No lint warnings" && echo 1)"
done

if [ -n "${EXAMPLES:-}" ]; then
    chk "scripts/examples-check.sh" "$(bash scripts/examples-check.sh "$FID" > /dev/null 2>&1 && echo 1)"
fi
if [ -n "${STDLIB:-}" ]; then
    LC=$(bash scripts/lint-check.sh "$FID" 2>&1); lc=$?
    echo "$LC" | tail -40 | sed 's/^/     /'
    chk "scripts/lint-check.sh (stdlib against the baseline)" "$([ $lc = 0 ] && echo 1)"
fi

if [ $fail = 0 ]; then echo "lint-style-check: PASS"; else echo "lint-style-check: FAIL"; fi
exit $fail
