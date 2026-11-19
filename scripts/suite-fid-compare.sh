#!/usr/bin/env bash
# suite-fid-compare.sh — compare the suite run with fid as the compiler
# (`fidesc test --compiler <fid>`, release-roadmap A4) with fidesc's saved
# baseline (tests/bootstrap/baseline/test_results.txt).
#
# Usage: scripts/suite-fid-compare.sh <normal.txt> [<fail.txt>] [<baseline>]
#   normal.txt  output of `fidesc test --compiler <fid>`
#   fail.txt    output of `fidesc test --fail --compiler <fid>` (optional)
#   baseline    default tests/bootstrap/baseline/test_results.txt
#
# For each backend (Interpreter, C Codegen, PE Emitter) and each half
# (normal, expected-fail) it prints fid's "passed/total" beside the
# baseline's, and the tests fid failed. Exit 1 when fid fails more tests on
# a backend than the baseline does (the totals may differ: the suite grows),
# or when a result line is missing (the run stopped early).
#
# SELFTEST=1: check the parser on built-in samples and exit.
set -u

# counts <file> <section>: "Backend<TAB>passed<TAB>total" lines of one
# "=== Results (<section>) ===" block
counts() {
  awk -v sec="=== Results ($2) ===" '
    index($0, sec) == 1 { on = 1; next }
    /^=== Results/ { on = 0 }
    /^--- / { on = 0 }
    on && match($0, /^(Interpreter|C Codegen|PE Emitter): *[0-9]+\/[0-9]+/) {
      name = $0; sub(/:.*/, "", name)
      n = $0; sub(/^[^:]*: */, "", n); split(n, a, /[\/ ]/)
      printf "%s\t%s\t%s\n", name, a[1], a[2]
    }' "$1" | tr -d '\r'
}

# compare <fid file> <baseline file> <section>: prints, returns 1 on a
# regression
compare() {
  local f="$1" b="$2" sec="$3" bad=0 line name fp ft bp bt
  local fc bc
  fc=$(counts "$f" "$sec"); bc=$(counts "$b" "$sec")
  if [ -z "$fc" ]; then echo "  $sec: no result lines in $f (run stopped?)"; return 1; fi
  printf '  %-12s %-14s %-14s %s\n' "$sec" "fid" "baseline" ""
  for name in "Interpreter" "C Codegen" "PE Emitter"; do
    line=$(printf '%s\n' "$fc" | awk -F'\t' -v n="$name" '$1 == n')
    if [ -z "$line" ]; then echo "  $name: missing in $f"; bad=1; continue; fi
    fp=$(echo "$line" | cut -f2); ft=$(echo "$line" | cut -f3)
    line=$(printf '%s\n' "$bc" | awk -F'\t' -v n="$name" '$1 == n')
    if [ -n "$line" ]; then bp=$(echo "$line" | cut -f2); bt=$(echo "$line" | cut -f3); else bp=0; bt=0; fi
    local verdict=ok
    if [ $((ft - fp)) -gt $((bt - bp)) ]; then verdict="WORSE ($((ft - fp)) failing, baseline $((bt - bp)))"; bad=1; fi
    printf '  %-12s %-14s %-14s %s\n' "$name" "$fp/$ft" "$bp/$bt" "$verdict"
  done
  return $bad
}

if [ "${SELFTEST:-0}" = 1 ]; then
  T=$(mktemp -d); trap 'rm -rf "$T"' EXIT
  printf '%s\n' '=== Results (normal) ===' 'Interpreter:  334/334 passed' \
    'C Codegen:    338/338 matched' 'PE Emitter:   341/341 matched, 0 unsupported' \
    '' '--- Known-fail tests (not counted above) ---' 'Interpreter:  0 fail, 0 bonus pass' \
    '=== Results (expected-fail) ===' 'Interpreter:  89/89 correctly failed' \
    'C Codegen:    89/89 correctly failed' 'PE Emitter:   89/89 correctly failed' > "$T/base"
  printf '%s\r\n' '  [FAIL] test_x' '=== Results (normal) ===' 'Interpreter:  350/350 passed' \
    'C Codegen:    352/354 matched' 'PE Emitter:   355/355 matched, 0 unsupported' > "$T/same"
  printf '%s\n' '=== Results (normal) ===' 'Interpreter:  350/350 passed' \
    'C Codegen:    354/354 matched' 'PE Emitter:   355/355 matched' > "$T/good"
  printf '%s\n' '=== Results (expected-fail) ===' 'Interpreter:  95/95 correctly failed' \
    'C Codegen:    95/95 correctly failed' 'PE Emitter:   95/95 correctly failed' > "$T/fail"
  ok=1
  [ "$(counts "$T/base" normal | wc -l)" = 3 ] || { echo "selftest: baseline normal not parsed"; ok=0; }
  [ "$(counts "$T/base" expected-fail | cut -f2 | tr '\n' ' ')" = "89 89 89 " ] || { echo "selftest: expected-fail not parsed"; ok=0; }
  [ "$(counts "$T/same" normal | awk -F'\t' '$1 == "C Codegen"' | cut -f2,3)" = "352	354" ] || { echo "selftest: CRLF line not parsed"; ok=0; }
  compare "$T/same" "$T/base" normal >/dev/null && { echo "selftest: 2 C failures not reported"; ok=0; }
  compare "$T/good" "$T/base" normal >/dev/null || { echo "selftest: a clean run reported as worse"; ok=0; }
  compare "$T/fail" "$T/base" expected-fail >/dev/null || { echo "selftest: expected-fail run reported as worse"; ok=0; }
  : > "$T/empty"
  compare "$T/empty" "$T/base" normal >/dev/null && { echo "selftest: an empty run not reported"; ok=0; }
  if [ "$ok" = 1 ]; then echo "suite-fid-compare selftest: PASS"; exit 0; fi
  echo "suite-fid-compare selftest: FAIL"; exit 1
fi

cd "$(dirname "$0")/.." || exit 1
NORMAL="${1:-}"; FAILS="${2:-}"; BASE="${3:-tests/bootstrap/baseline/test_results.txt}"
if [ -z "$NORMAL" ]; then sed -n '2,16p' "$0"; exit 2; fi
rc=0
echo "suite with fid as the compiler vs $BASE:"
compare "$NORMAL" "$BASE" normal || rc=1
if [ -n "$FAILS" ]; then compare "$FAILS" "$BASE" expected-fail || rc=1; fi
fl=$(cat "$NORMAL" ${FAILS:+"$FAILS"} | tr -d '\r' | grep -E '^\s*\[(FAIL|UNEXPECTED[^]]*)\]' | sed 's/^ */  /')
if [ -n "$fl" ]; then echo "failing with fid:"; echo "$fl"; fi
if [ "$rc" = 0 ]; then echo "suite-fid-compare: no backend worse than the baseline"; else echo "suite-fid-compare: WORSE than the baseline"; fi
exit "$rc"
