#!/bin/bash
# roadmap L7E (unused variables): `fid lint` reports a local or parameter
# that its function never reads (compiler/compiler.fd lint_source_scan,
# lx_lint_fn), and nothing else of the kind.
#   FID=release/fid.exe bash tests/tooling/lint-unused-check.sh
# Run from the repository root. Checks:
#   1. tests/bootstrap/test_unused_var_lint.fd: exactly the three warnings
#      its header lists, in that order, each with its Fix: line, and
#      "3 lint warning(s)."; none for the names that are read or start
#      with `_`; the program still runs and prints its expected output
#   2. tests/tooling/lint_unused/trait_params.fd: a trait impl's method
#      parameter is not reported, a plain function's is (one warning)
#   3. scripts/verify/lint-scan-predict.py (the scan copied in Python, used
#      to list the warnings stdlib/ and examples/ will get) gives the same
#      scan warnings as fid on both files and on PREDICT_FILES (default:
#      the five examples of scripts/examples-check.sh and a few stdlib
#      modules), so its predictions can be trusted
#   4. scripts/examples-check.sh's five examples stay at "No lint warnings"
# Exit 0 when every check holds.
FID=${FID:-release/fid.exe}
PY=${PY:-python3}
command -v "$PY" > /dev/null 2>&1 || PY=python
T=tests/bootstrap/test_unused_var_lint.fd
F2=tests/tooling/lint_unused/trait_params.fd
fail=0
chk() {
    if [ "$2" = 1 ]; then echo "ok   $1"; else echo "FAIL $1"; fail=1; fi
}
lint_of() { "$FID" lint "$1" 2>&1 | tr -d '\r'; }

# 1. the test's three warnings
OUT=$("$FID" lint "$T" 2>&1); code=$?
OUT=$(echo "$OUT" | tr -d '\r')
chk "$T: lint succeeds" "$([ $code = 0 ] && echo 1)"
W=$(echo "$OUT" | grep "^warning: ")
chk "$T: exactly three warnings" "$([ "$(echo "$W" | grep -c .)" = 3 ] && echo 1)"
chk "$T: the summary says 3" "$(echo "$OUT" | grep -q "^3 lint warning(s)" && echo 1)"
L1=$(echo "$W" | sed -n 1p); L2=$(echo "$W" | sed -n 2p); L3=$(echo "$W" | sed -n 3p)
chk "$T: first: line 15 unused variable 'leftover' in 'weighted'" \
    "$(echo "$L1" | grep -qF "warning: line 15: unused variable 'leftover' in 'weighted'" && echo 1)"
chk "$T: second: line 13 unused parameter 'scale' of 'weighted'" \
    "$(echo "$L2" | grep -qF "warning: line 13: unused parameter 'scale' of 'weighted'" && echo 1)"
chk "$T: third: line 29 unused variable 'k' in 'count_up'" \
    "$(echo "$L3" | grep -qF "warning: line 29: unused variable 'k' in 'count_up'" && echo 1)"
chk "$T: each warning has its Fix: line" \
    "$([ "$(echo "$OUT" | grep -c "^    Fix: remove it, or name it _")" = 3 ] && echo 1)"
chk "$T: the fix names _leftover, _scale and _k" \
    "$(echo "$OUT" | grep -q "name it _leftover " && echo "$OUT" | grep -q "name it _scale " && echo "$OUT" | grep -q "name it _k " && echo 1)"
quiet=1
for nm in sum i xs _unused_ok _ignored j n c total; do
    if echo "$W" | grep -qE "(variable|parameter) '$nm'"; then quiet=0; echo "     (reported: $nm)"; fi
done
chk "$T: nothing about names that are read or start with _" "$quiet"
RUN=$("$FID" run "$T" 2>/dev/null | tr -d '\r' | grep -v '^\[')
chk "$T: still runs (14, 3, 6)" "$([ "$RUN" = "$(printf '14\n3\n6')" ] && echo 1)"

# 2. trait impl parameters
OUT2=$(lint_of "$F2")
W2=$(echo "$OUT2" | grep "^warning: ")
chk "$F2: one warning" "$([ "$(echo "$W2" | grep -c .)" = 1 ] && echo 1)"
chk "$F2: the plain function's parameter (line 32)" \
    "$(echo "$W2" | grep -qF "warning: line 32: unused parameter 'scale' of 'plain_area'" && echo 1)"
chk "$F2: not the trait method's" "$(echo "$W2" | grep -qF "of 'area'" || echo 1)"

# 3. the Python copy agrees with fid
scan_lines() { grep -E "^warning: line [0-9]+: (unused (variable|parameter) |(function|variable|parameter|struct|enum|cap|trait|constant) '[^']*'.* should be |imports are not sorted)"; }
PREDICT_FILES=${PREDICT_FILES:-"examples/lexer.fd examples/parser.fd examples/json_parser.fd examples/codegen.fd examples/mini_compiler.fd stdlib/math.fd stdlib/shape.fd stdlib/dns.fd tests/bootstrap/test_style_lint.fd"}
for f in "$T" "$F2" $PREDICT_FILES; do
    [ -f "$f" ] || { echo "skip $f (missing)"; continue; }
    A=$(lint_of "$f" | scan_lines)
    B=$("$PY" scripts/verify/lint-scan-predict.py "$f" | tr -d '\r' | scan_lines)
    if [ "$A" = "$B" ]; then
        chk "predictor = fid on $f ($(echo "$A" | grep -c .) scan warnings)" 1
    else
        chk "predictor = fid on $f" 0
        diff <(echo "$A") <(echo "$B") | head -10 | sed 's/^/     /'
    fi
done

# 4. the examples gate
for e in lexer parser json_parser codegen mini_compiler; do
    chk "examples/$e.fd: No lint warnings" "$(lint_of "examples/$e.fd" | grep -q "^No lint warnings" && echo 1)"
done

if [ $fail = 0 ]; then echo "lint-unused-check: PASS"; else echo "lint-unused-check: FAIL"; fi
exit $fail
