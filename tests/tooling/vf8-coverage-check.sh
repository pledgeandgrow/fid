#!/bin/bash
# vf8-coverage-check.sh — VF8 (competitive roadmap): the line coverage of
# scripts/verify/cov_instrument.py + coverage.py counts real source lines,
# checked on a small program (no compiler build); BUILD=1 also runs
# scripts/verify/vf8-run.sh on a few tests.
#
# Usage: tests/tooling/vf8-coverage-check.sh
#   FID=<compiler>  default release/fid.exe (release/fid off Windows)
#   CC=<c compiler> default `zig cc`
#   OUT=<dir>       default build/vf8-check
#   BUILD=1         check 7: the counter compiler from the tree and LIMIT
#                   (default 5) tests through it (several GB, an hour at -O0)
#
# Checks, one line each (PASS/FAIL), exit status the number of FAILs:
#  1. cov_instrument.py --self-test and coverage.py --self-test pass.
#  2. the C generator can carry source lines for build-variant.sh's
#     `dump-c compiler/main.fd`: FID_C_LINE_MARKS=1 dump-c writes
#     `/*@L<n>*/` marks, or `dump-c -g` writes `#line <n> "<file>.fd"`
#     (one of the two hunks in codegen_c.fd / main.fd).
#  3. tests/bootstrap/test_mutation_kill.fd's C (ccompile -g: #line, the
#     form that exists since TL5), instrumented and built with $CC, prints
#     the test's expected lines and writes a bitmap; the map has a counter
#     on each of cmps's source lines.
#  4. coverage.py --report-only on that bitmap: a real line table (not the
#     estimate); in cmps the line of `if x != x` (never true) uncovered and
#     the one of `if x == x` covered; uncovered-lines.txt names them as
#     module:line.
#  5. --goal-lines 101 exits 1; --goal-lines on a map without line
#     counters (cov_instrument.py over C without #line) exits 2.
#  6. the marks of check 2 (when the C generator writes them) give the same
#     covered/uncovered verdicts for those two lines.
#  7. (BUILD=1) vf8-run.sh with LIMIT tests: a report with the line table
#     and a row for compiler/lexer.fd.
set -u
cd "$(dirname "$0")/../.." || exit 1
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; *) WIN=0 ;; esac
if [ $WIN = 1 ]; then FID="${FID:-release/fid.exe}"; LIBS="-lws2_32"; else FID="${FID:-release/fid}"; LIBS="-lm -lpthread"; fi
export CC="${CC:-zig cc}"
CC_CMD=($CC)
OUT="${OUT:-build/vf8-check}"
mkdir -p "$OUT"
PY=python3; command -v python3 > /dev/null 2>&1 || PY=python
T=tests/bootstrap/test_mutation_kill.fd
CFLAGS=(-O0 -w -Wno-error -Wno-int-conversion -Wno-incompatible-pointer-types
  -Wno-implicit-function-declaration -Wno-return-type)
case "$CC" in *zig*) CFLAGS+=(-fno-sanitize=undefined) ;; esac
bad=0
pass() { echo "PASS $1"; }
fail() { echo "FAIL $1"; bad=$((bad + 1)); }
L_EQ=$(awk '/^fn cmps/ {f = 1} f && /if x == x/ {print NR + 1; exit}' "$T")
L_NE=$(awk '/^fn cmps/ {f = 1} f && /if x != x/ {print NR + 1; exit}' "$T")
exp=$(sed -n '/^\/\/ Expected output:/,/^$/p' "$T" | sed -n 's|^// ||p' | sed '1d')

# 1
if "$PY" scripts/verify/cov_instrument.py --self-test > "$OUT/s1.log" 2>&1 &&
   "$PY" scripts/verify/coverage.py --self-test > "$OUT/s2.log" 2>&1; then
  pass "1 self-tests"
else
  fail "1 self-tests: $(tail -2 "$OUT/s1.log" "$OUT/s2.log" | tr '\n' ' ')"
fi

# 2
FID_C_LINE_MARKS=1 "$FID" dump-c "$T" > "$OUT/marks.c" 2> /dev/null
nm=$(grep -c '^[[:space:]]*/\*@L[0-9]*\*/$' "$OUT/marks.c")
"$FID" dump-c "$T" -g > "$OUT/g.c" 2> /dev/null
ng=$(grep -c '^#line [0-9]* ".*\.fd"$' "$OUT/g.c")
if [ "$nm" -gt 0 ] || [ "$ng" -gt 0 ]; then
  pass "2 dump-c carries source lines: $nm marks (FID_C_LINE_MARKS=1), $ng #line (dump-c -g)"
else
  fail "2 dump-c writes neither line marks (FID_C_LINE_MARKS=1) nor #line (-g): the counter compiler would have block coverage only (codegen_c.fd / main.fd hunk of VF8/RL12 missing)"
fi

# 3
rm -f "$OUT/k.exe" "$OUT/merged.cov"
"$FID" ccompile "$T" -g -o "$OUT/kg.exe" > "$OUT/kg.log" 2>&1
if [ -s "$OUT/kg.c" ] && "$PY" scripts/verify/cov_instrument.py "$OUT/kg.c" "$OUT/k.inst.c" > "$OUT/inst.log" 2>&1 &&
   "${CC_CMD[@]}" "${CFLAGS[@]}" "$OUT/k.inst.c" -o "$OUT/k.exe" $LIBS > "$OUT/cc.log" 2>&1; then
  got=$(FID_COV_OUT="$OUT/merged.cov" timeout 60 "$OUT/k.exe" < /dev/null 2>&1 | tr -d '\r')
  nl_eq=$(grep -c " cmps L$L_EQ\$" "$OUT/k.inst.c.map")
  nl_ne=$(grep -c " cmps L$L_NE\$" "$OUT/k.inst.c.map")
  if [ "$got" = "$exp" ] && [ -s "$OUT/merged.cov" ] && [ "$nl_eq" -gt 0 ] && [ "$nl_ne" -gt 0 ]; then
    pass "3 instrumented program: expected output, bitmap written, line counters on cmps:$L_EQ and cmps:$L_NE ($(head -1 "$OUT/inst.log"))"
  else
    fail "3 instrumented program: output $(echo "$got" | head -3 | tr '\n' ' ')..., bitmap $(wc -c < "$OUT/merged.cov" 2>/dev/null), counters on L$L_EQ/L$L_NE: $nl_eq/$nl_ne"
  fi
else
  fail "3 ccompile -g, instrument or C build failed: $(tail -2 "$OUT/kg.log" "$OUT/inst.log" "$OUT/cc.log" 2>/dev/null | tr '\n' ' ')"
fi

# 4, 5
if [ -s "$OUT/merged.cov" ]; then
  "$PY" scripts/verify/coverage.py --report-only --out "$OUT" --map "$OUT/k.inst.c.map" --sources "$T" \
    --goal-modules "$T" > "$OUT/report.log" 2>&1
  rc=$?
  row=$(sed -n '/^Lines (VF8/,$p' "$OUT/report.log" | grep "^$T ")
  unc="$OUT/uncovered-lines.txt"
  if [ $rc = 0 ] && [ -n "$row" ] && grep -qx "$T:$L_NE" "$unc" && ! grep -qx "$T:$L_EQ" "$unc"; then
    pass "4 line table: $(echo "$row" | tr -s ' '); line $L_NE uncovered, $L_EQ covered"
  else
    fail "4 report (exit $rc): line row '$row'; $T:$L_NE uncovered: $(grep -cx "$T:$L_NE" "$unc" 2>/dev/null), $T:$L_EQ uncovered: $(grep -cx "$T:$L_EQ" "$unc" 2>/dev/null)"
  fi
  "$PY" scripts/verify/coverage.py --report-only --out "$OUT" --map "$OUT/k.inst.c.map" --sources "$T" \
    --goal-modules "$T" --goal-lines 101 > /dev/null 2>&1
  r1=$?
  # the same C without its #line directives: blocks only
  mkdir -p "$OUT/nolines"
  grep -v '^#line ' "$OUT/kg.c" > "$OUT/nolines/k.c"
  "$PY" scripts/verify/cov_instrument.py "$OUT/nolines/k.c" "$OUT/nolines/k.inst.c" > /dev/null 2>&1
  "$PY" - "$OUT/nolines/k.inst.c.map" "$OUT/nolines/merged.cov" <<'PY'
import sys
n = sum(1 for _ in open(sys.argv[1]))
open(sys.argv[2], "wb").write(bytes((n + 7) // 8))
PY
  "$PY" scripts/verify/coverage.py --report-only --out "$OUT/nolines" --map "$OUT/nolines/k.inst.c.map" --sources "$T" \
    --goal-modules "$T" --goal-lines 50 > "$OUT/nolines/report.log" 2>&1
  r2=$?
  if [ $r1 = 1 ] && [ $r2 = 2 ] && ! grep -q ' L[0-9]*$' "$OUT/nolines/k.inst.c.map"; then
    pass "5 --goal-lines 101 exits 1; without line counters exits 2"
  else
    fail "5 --goal-lines: 101 gave $r1 (want 1), no line counters gave $r2 (want 2)"
  fi
else
  fail "4 no bitmap to report on"
  fail "5 no bitmap to report on"
fi

# 6
if [ "$nm" -gt 0 ]; then
  M="$OUT/marks"
  mkdir -p "$M"
  rm -f "$M/merged.cov"
  if "$PY" scripts/verify/cov_instrument.py "$OUT/marks.c" "$M/k.inst.c" > /dev/null 2>&1 &&
     "${CC_CMD[@]}" "${CFLAGS[@]}" "$M/k.inst.c" -o "$M/k.exe" $LIBS > "$M/cc.log" 2>&1; then
    FID_COV_OUT="$M/merged.cov" timeout 60 "$M/k.exe" < /dev/null > /dev/null 2>&1
    "$PY" scripts/verify/coverage.py --report-only --out "$M" --map "$M/k.inst.c.map" --sources "$T" \
      --goal-modules "$T" > "$M/report.log" 2>&1
    if grep -qx "$T:$L_NE" "$M/uncovered-lines.txt" && ! grep -qx "$T:$L_EQ" "$M/uncovered-lines.txt"; then
      pass "6 line marks: the same verdicts ($L_NE uncovered, $L_EQ covered)"
    else
      fail "6 line marks: $T:$L_NE uncovered $(grep -cx "$T:$L_NE" "$M/uncovered-lines.txt" 2>/dev/null), $T:$L_EQ uncovered $(grep -cx "$T:$L_EQ" "$M/uncovered-lines.txt" 2>/dev/null)"
    fi
  else
    fail "6 line marks: instrument or C build failed: $(tail -2 "$M/cc.log" | tr '\n' ' ')"
  fi
else
  echo "SKIP 6 (the C generator writes no line marks)"
fi

# 7
if [ "${BUILD:-}" = 1 ]; then
  OUT="$OUT/full" LIMIT="${LIMIT:-5}" GEN=0 RECORD=none MODES="${MODES:-check,run,emit:x86_64-linux}" \
    bash scripts/verify/vf8-run.sh > "$OUT/full.log" 2>&1
  rc=$?
  if [ $rc -le 1 ] && grep -q '^Lines (VF8' "$OUT/full/report.txt" && grep -q '^compiler/lexer.fd ' "$OUT/full/report.txt"; then
    pass "7 vf8-run.sh (exit $rc): $(grep '^TOTAL' "$OUT/full/report.txt" | tr -s ' ' | tr '\n' ';')"
  else
    fail "7 vf8-run.sh (exit $rc): $(tail -4 "$OUT/full.log" | tr '\n' ' ')"
  fi
fi
echo "vf8-coverage-check: $bad failed"
exit $bad
