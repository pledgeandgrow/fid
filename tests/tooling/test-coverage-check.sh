#!/bin/bash
# CT8 (docs/roadmap/complaint-roadmap.md): `fid test <file> --coverage` on a
# suite with a panicking test prints the test results and veil's report, and
# does not crash (compiler/main.fd cmd_test_file runs the coverage in a child
# `fid veil`; known-bugs 2g.30 was the in-process double free).
#   FID=release/fid.exe bash tests/tooling/test-coverage-check.sh
# Run from the repository root. Exit 0 when every check passes.
FID=${FID:-release/fid.exe}
OUT=$(mktemp)
fail=0
"$FID" test tests/projects/ct8_coverage/suite.fd --coverage > "$OUT" 2>&1
code=$?
has() {
    if grep -qF -- "$2" "$OUT"; then echo "ok   $1"; else echo "FAIL $1: no line with: $2"; fail=1; fi
}
has "the coverage report" "veil -- Coverage Report"
has "line coverage" "Lines:"
has "function coverage of the suite" "sign"
has "the suite's summary after the report" "2 passed, 1 failed"
if [ $code -eq 1 ]; then echo "ok   exit 1 for one failing test"; else echo "FAIL exit code $code (want 1: one test fails, nothing crashes)"; fail=1; fi
if grep -qiE "double free|invalid free|segmentation|access violation" "$OUT"; then echo "FAIL a crash in the output"; fail=1; else echo "ok   no crash"; fi
# The coverage run ran the tests (veil gives the covered program its own
# command line: before, the harness read `veil` as the test to run and ran
# none), up to the panicking one, whose panic veil reports
has "the coverage run reached the panic" "panic: division by zero"
if grep -qE "^test_sign +[1-9][0-9]* +[1-9][0-9]* +100\.0% OK" "$OUT"; then echo "ok   test_sign covered"; else echo "FAIL test_sign not covered (the harness ran no test)"; fail=1; fi
# branch coverage (veil.fd vl_cover_module -> Interpreter_branch_report)
has "the branch summary" "Branches:"
has "the list of ways never taken" "Branches never taken:"
has "a branch never taken" "sign line 6: true never taken"
if grep -qF "sign line 9: true never taken" "$OUT" || grep -qF "sign line 9: false never taken" "$OUT"; then
    echo "FAIL sign line 9 went both ways (sign(5), sign(0)) but is listed"; fail=1
else
    echo "ok   a branch taken both ways is not listed"
fi
# known-bugs 2g.30: veil itself on a program that panics, twice the
# report (text, then html) and no crash
"$FID" veil tests/projects/ct8_coverage/panics.fd --output "$OUT.cov" > "$OUT.veil" 2>&1
vcode=$?
if [ $vcode -eq 0 ] && grep -qF "panic: division by zero" "$OUT.veil" && grep -qF "Branches:" "$OUT.veil"; then echo "ok   fid veil on a panicking program"; else echo "FAIL fid veil on a panicking program (exit $vcode)"; cat "$OUT.veil"; fail=1; fi
"$FID" veil tests/projects/ct8_coverage/panics.fd --format html --output "$OUT.cov" > "$OUT.veil" 2>&1
vcode=$?
if [ $vcode -eq 0 ] && grep -qF "<h2>Branches</h2>" "$OUT.cov/coverage.html"; then echo "ok   html report with branches"; else echo "FAIL html report (exit $vcode)"; cat "$OUT.veil"; fail=1; fi
if grep -qiE "double free|invalid free|segmentation|access violation" "$OUT.veil"; then echo "FAIL a crash in fid veil"; fail=1; fi
rm -rf "$OUT.cov" "$OUT.veil"
if [ $fail -ne 0 ]; then echo "--- output ---"; cat "$OUT"; fi
rm -f "$OUT"
exit $fail
