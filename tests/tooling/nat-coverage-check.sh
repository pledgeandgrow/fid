#!/bin/bash
# nat-coverage-check.sh — RL12 (llvm roadmap): the line and branch coverage
# of scripts/nat-coverage.py counts real source lines, checked on a small
# program (no compiler build); BUILD=1 also runs the whole thing on the
# machine layer.
#
# Usage: tests/tooling/nat-coverage-check.sh
#   FID=<compiler>  default release/fid.exe (release/fid off Windows)
#   CC=<c compiler> default `zig cc`
#   OUT=<dir>       default build/nat-coverage-check
#   BUILD=1         check 6: build the counter compiler from the tree and run
#                   LIMIT (default 10) tests through it (several GB, minutes)
#
# Checks, one line each (PASS/FAIL), exit status the number of FAILs:
#  1. scripts/nat-cov-instrument.py --self-test and
#     scripts/nat-coverage.py --self-test pass.
#  2. FID_C_LINE_MARKS=1 makes `fid dump-c` mark source lines (`/*@L<n>*/`,
#     compiler/codegen_c.fd); without it there is no mark, and with the
#     marks taken out the two C files are the same.
#  3. tests/bootstrap/test_mutation_kill.fd's marked C, instrumented and
#     built with $CC, prints the test's expected lines and writes a bitmap.
#  4. nat-coverage.py --report-only on that bitmap: real lines (not the
#     estimate), every function of the test entered (main's C wrapper
#     too), the line inside
#     `if x != x` (never true) uncovered and the one inside `if x == x`
#     covered.
#  5. nat-coverage.py --goal 101 exits 1 (the goal is checked).
#  6. (BUILD=1) nat-coverage.py --base-fid $FID --limit $LIMIT: a table for
#     compiler/nat_ir.fd and compiler/nat_enc.fd with real lines.
set -u
cd "$(dirname "$0")/../.." || exit 1
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; *) WIN=0 ;; esac
if [ $WIN = 1 ]; then FID="${FID:-release/fid.exe}"; LIBS="-lws2_32"; else FID="${FID:-release/fid}"; LIBS="-lm -lpthread"; fi
CC_CMD=(${CC:-zig cc})
OUT="${OUT:-build/nat-coverage-check}"
mkdir -p "$OUT"
PY=python3; command -v python3 > /dev/null 2>&1 || PY=python
T=tests/bootstrap/test_mutation_kill.fd
bad=0
pass() { echo "PASS $1"; }
fail() { echo "FAIL $1"; bad=$((bad + 1)); }

# 1
if "$PY" scripts/nat-cov-instrument.py --self-test > "$OUT/s1.log" 2>&1 && "$PY" scripts/nat-coverage.py --self-test > "$OUT/s2.log" 2>&1; then
  pass "1 self-tests"
else
  fail "1 self-tests: $(tail -2 "$OUT/s1.log" "$OUT/s2.log" | tr '\n' ' ')"
fi

# 2
FID_C_LINE_MARKS=1 "$FID" dump-c "$T" > "$OUT/k.c" 2> "$OUT/k.err"
rc1=$?
"$FID" dump-c "$T" > "$OUT/plain.c" 2> "$OUT/plain.err"
rc2=$?
nm=$(grep -c '^[[:space:]]*/\*@L[0-9]*\*/$' "$OUT/k.c")
np=$(grep -c '/\*@L[0-9]*\*/' "$OUT/plain.c")
grep -v '^[[:space:]]*/\*@L[0-9]*\*/$' "$OUT/k.c" > "$OUT/k-nomarks.c"
if [ $rc1 = 0 ] && [ $rc2 = 0 ] && [ "$nm" -gt 0 ] && [ "$np" = 0 ] && cmp -s "$OUT/k-nomarks.c" "$OUT/plain.c"; then
  pass "2 FID_C_LINE_MARKS=1: $nm line marks; none without it; otherwise the same C"
else
  fail "2 line marks: dump-c exit $rc1/$rc2, $nm marks with FID_C_LINE_MARKS=1, $np without (codegen_c.fd hunk of RL12 missing?)$(cmp -s "$OUT/k-nomarks.c" "$OUT/plain.c" || echo '; the C differs beyond the marks')"
fi

# 3
exp=$(sed -n '/^\/\/ Expected output:/,/^$/p' "$T" | sed -n 's|^// ||p' | sed '1d')
EXE="$OUT/k.exe"
rm -f "$EXE" "$OUT/merged.cov"
if "$PY" scripts/nat-cov-instrument.py "$OUT/k.c" "$OUT/k.inst.c" > "$OUT/inst.log" 2>&1 &&
   "${CC_CMD[@]}" -O0 -w -Wno-error -Wno-int-conversion -Wno-incompatible-pointer-types \
     -Wno-implicit-function-declaration -Wno-return-type "$OUT/k.inst.c" -o "$EXE" $LIBS > "$OUT/cc.log" 2>&1; then
  got=$(FID_COV_OUT="$OUT/merged.cov" timeout 60 "$EXE" < /dev/null 2>&1 | tr -d '\r')
  if [ "$got" = "$exp" ] && [ -s "$OUT/merged.cov" ]; then
    pass "3 instrumented program: expected output, bitmap written ($(cat "$OUT/inst.log" | head -1))"
  else
    fail "3 instrumented program: output $(echo "$got" | head -3 | tr '\n' ' ')..., bitmap $(ls -la "$OUT/merged.cov" 2>&1 | cut -c1-60)"
  fi
else
  fail "3 instrument or C build failed: $(tail -2 "$OUT/inst.log" "$OUT/cc.log" | tr '\n' ' ')"
fi

# 4
L_EQ=$(awk '/^fn cmps/ {f = 1} f && /r = r \+ 1;/ {print NR; exit}' "$T")
L_NE=$(awk '/^fn cmps/ {f = 1} f && /r = r \+ 2;/ {print NR; exit}' "$T")
NFN=$(grep -c '^fn ' "$T")
if [ -s "$OUT/merged.cov" ]; then
  "$PY" scripts/nat-coverage.py --report-only --out "$OUT" --map "$OUT/k.inst.c.map" --sources "$T" --modules "$T" > "$OUT/report.log" 2>&1
  rc=$?
  row=$(grep "^$T " "$OUT/report.log")
  unc="$OUT/uncovered-$(basename "$T").txt"
  fns=$(echo "$row" | awk '{print $(NF-1)}')
  if [ $rc = 0 ] && [ -n "$row" ] && ! echo "$row" | grep -q 'est\.' && [ -n "$fns" ] && [ "${fns%/*}" = "${fns#*/}" ] && [ "${fns#*/}" -ge "$NFN" ] &&
     grep -qx "$T:$L_NE" "$unc" && ! grep -qx "$T:$L_EQ" "$unc"; then
    pass "4 report: $row; line $L_NE uncovered, $L_EQ covered"
  else
    fail "4 report (exit $rc): '$row' (functions: every one of at least $NFN entered); line $L_NE uncovered: $(grep -cx "$T:$L_NE" "$unc" 2>/dev/null), line $L_EQ uncovered: $(grep -cx "$T:$L_EQ" "$unc" 2>/dev/null)"
  fi
  # 5
  "$PY" scripts/nat-coverage.py --report-only --out "$OUT" --map "$OUT/k.inst.c.map" --sources "$T" --modules "$T" --goal 101 > /dev/null 2>&1
  if [ $? = 1 ]; then pass "5 --goal 101 fails the run"; else fail "5 --goal 101 did not fail the run"; fi
else
  fail "4 no bitmap to report on"
  fail "5 no bitmap to report on"
fi

# 6
if [ "${BUILD:-}" = 1 ]; then
  # the switch runs (variants) only with VARIANTS=all: thousands of builds
  "$PY" scripts/nat-coverage.py --base-fid "$FID" --out "$OUT/full" --limit "${LIMIT:-10}" -j 2 \
    --variants "${VARIANTS:-none}" > "$OUT/full.log" 2>&1
  rc=$?
  if [ $rc = 0 ] && grep -q '^compiler/nat_ir.fd ' "$OUT/full.log" && grep -q '^compiler/nat_enc.fd ' "$OUT/full.log" && ! grep -q 'est\.' "$OUT/full.log"; then
    pass "6 machine layer: $(grep -E '^compiler/nat_(ir|enc).fd ' "$OUT/full.log" | tr -s ' ' | tr '\n' ';')"
  else
    fail "6 full run (exit $rc): $(tail -4 "$OUT/full.log" | tr '\n' ' ')"
  fi
fi
echo "nat-coverage-check: $bad failed"
exit $bad
