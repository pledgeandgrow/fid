#!/bin/bash
# miscompile-gate-check.sh — RL4 (llvm roadmap): a miscompile is fixed only
# with a test that fails without the fix. Checks scripts/miscompile-gate.sh
# itself on copies of docs/verification/miscompile-log.md (MISCOMPILE_LOG),
# then the real log.
#
# Usage: tests/tooling/miscompile-gate-check.sh
#   FID=<compiler>  for checks 7 and 8 (default release/fid.exe, release/fid
#                   off Windows); FID=none skips them
#   OUT=<dir>       default build/miscompile-gate-check
#
# Checks, one line each (PASS/FAIL), exit status the number of FAILs:
#  1. the real log passes the gate (every fixed entry names an existing test,
#     every repro in tests/verify/bugs/ is named)
#  2. a fixed entry naming no test fails it
#  3. a fixed entry naming only a compiler source file fails it
#  4. a repro the log does not name fails it
#  5. --new-since HEAD lists an entry fixed in the copy and not at HEAD, and
#     not the entries HEAD already had fixed
#  6. --before with a compiler that does not start: exit 2, not a pass
#  7. --run --only M8 with FID: the entry's test passes (exit 0)
#  8. --before FID --only M8 with the same FID: the test passes there too,
#     so the gate must say it "does not catch the bug" and fail; this is the
#     check that a fix's test fails on the compiler without the fix
#  9. every row of "Earlier miscompiles" names a test (10 of 10 or more),
#     and a copy with one row's test removed fails the gate
# 10. R1 names its check script tests/tooling/r1-ccompile-repro-check.sh,
#     and a copy naming a script that does not exist fails the gate
set -u
cd "$(dirname "$0")/../.." || exit 1
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) FID="${FID:-release/fid.exe}" ;; *) FID="${FID:-release/fid}" ;; esac
OUT="${OUT:-build/miscompile-gate-check}"
mkdir -p "$OUT"
LOG=docs/verification/miscompile-log.md
G=scripts/miscompile-gate.sh
bad=0
pass() { echo "PASS $1"; }
fail() { echo "FAIL $1"; bad=$((bad + 1)); }
gate() {  # gate <log copy> <args...>: output in $OUT/gate.log, exit status
  local l="$1"; shift
  MISCOMPILE_LOG="$l" bash "$G" "$@" > "$OUT/gate.log" 2>&1
}
# A row of "Wrong code" marked fixed, put after the table's header
add_row() {  # add_row <copy> <row>
  awk -v row="$2" '{print} /^\|----\|----------\|/ && !done {print row; done=1}' "$LOG" > "$1"
}

# 1
if bash "$G" > "$OUT/real.log" 2>&1; then
  pass "the real log passes the gate ($(grep 'fixed entries checked' "$OUT/real.log"))"
else
  fail "the real log fails the gate: $(grep -v '^miscompile gate' "$OUT/real.log" | tr '\n' ' ')"
fi

# 2
add_row "$OUT/no-test.md" '| MX1 | emit | made up | made up | made up | nothing | fixed today |'
if ! gate "$OUT/no-test.md" && grep -q 'MX1 is marked fixed but names no test file' "$OUT/gate.log"; then
  pass "a fixed entry without a test fails the gate"
else
  fail "a fixed entry without a test passes: $(cat "$OUT/gate.log")"
fi

# 3
add_row "$OUT/src-only.md" '| MX2 | emit | made up | made up | made up | `compiler/iropt.fd` | fixed today |'
if ! gate "$OUT/src-only.md" && grep -q 'MX2 is marked fixed but names no test file' "$OUT/gate.log"; then
  pass "a fixed entry naming only compiler source fails the gate"
else
  fail "a fixed entry naming only compiler/iropt.fd passes: $(cat "$OUT/gate.log")"
fi

# 4
R=$(ls tests/verify/bugs/*.fd 2>/dev/null | head -n 1)
if [ -n "$R" ]; then
  r=$(basename "$R")
  grep -vF "$r" "$LOG" > "$OUT/no-repro.md"
  if ! gate "$OUT/no-repro.md" && grep -q "repro $r exists but is not named" "$OUT/gate.log"; then
    pass "a repro the log does not name fails the gate ($r)"
  else
    fail "a repro missing from the log passes ($r): $(cat "$OUT/gate.log")"
  fi
else
  fail "no repro in tests/verify/bugs/ to check with"
fi

# 5
add_row "$OUT/new.md" '| MX3 | emit | made up | made up | made up | `tests/bootstrap/hello.fd` | fixed today |'
gate "$OUT/new.md" --new-since HEAD
line=$(grep 'entries fixed since the base' "$OUT/gate.log")
if echo "$line" | grep -q 'MX3' && ! echo "$line" | grep -qE '(: |, )M8(,|$)'; then
  pass "--new-since HEAD: $line"
else
  fail "--new-since HEAD: '$line' (want MX3 and not M8)"
fi

# 6
gate "$LOG" --only M8 --before "$OUT/no-such-fid.exe"
rc=$?
if [ "$rc" = 2 ] && grep -q 'does not run' "$OUT/gate.log"; then
  pass "--before a compiler that does not start: exit 2"
else
  fail "--before a compiler that does not start: exit $rc ($(tail -n 2 "$OUT/gate.log" | tr '\n' ' '))"
fi

# 9 (before 7 and 8: no compiler needed)
line=$(grep 'earlier entries' "$OUT/real.log")
n_old=$(echo "$line" | sed -n 's/.* \([0-9][0-9]*\) earlier entries.*/\1/p')
n_tested=$(echo "$line" | sed -n 's/.*, \([0-9][0-9]*\) of them name a test.*/\1/p')
if [ -n "$n_old" ] && [ "$n_old" -ge 10 ] && [ "$n_old" = "$n_tested" ]; then
  pass "every earlier entry names a test ($line)"
else
  fail "earlier entries without a test: '$line'"
fi
sed 's#`tests/bootstrap/test_verify_legacy_vec_mut_arg.fd`#nothing#' "$LOG" > "$OUT/old-no-test.md"
if ! gate "$OUT/old-no-test.md" && grep -q 'earlier entry AGENTS_4 names no test file' "$OUT/gate.log"; then
  pass "an earlier entry without a test fails the gate"
else
  fail "an earlier entry without a test passes: $(cat "$OUT/gate.log")"
fi
# 10. a check script names a test only when it exists under tests/tooling/
sed 's#`tests/tooling/r1-ccompile-repro-check.sh`#`tests/tooling/no-such-check.sh`#' "$LOG" > "$OUT/no-sh.md"
if grep -q 'r1-ccompile-repro-check.sh' "$LOG" && ! gate "$OUT/no-sh.md" \
   && grep -q 'R1 is marked fixed but names no test file' "$OUT/gate.log"; then
  pass "R1's check script counts as its test; a missing one does not"
else
  fail "check scripts as tests: $(cat "$OUT/gate.log")"
fi

# 7, 8
if [ "$FID" != none ] && [ -x "$FID" ]; then
  if gate "$LOG" --fid "$FID" --run --only M8 && grep -q 'test_mem2reg_promote.fd' "$OUT/gate.log"; then
    pass "--run --only M8: test_mem2reg_promote.fd passes with $FID"
  else
    cp "$OUT/gate.log" "$OUT/run.log"
    fail "--run --only M8 with $FID (see $OUT/run.log)"
  fi
  gate "$LOG" --fid "$FID" --only M8 --before "$FID"
  rc=$?
  if [ "$rc" = 1 ] && grep -q 'M8: .* passes with .* too: it does not catch the bug' "$OUT/gate.log"; then
    pass "--before with a compiler that has the fix: the gate fails (the test must fail without the fix)"
  else
    fail "--before with the fixed compiler: exit $rc ($(grep 'RL4 gate: M8' "$OUT/gate.log"))"
  fi
else
  echo "SKIP 7, 8 (no compiler at $FID)"
fi
exit $bad
