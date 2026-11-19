#!/bin/bash
# egraph-smt-check.sh — ME7 (own-backend roadmap): SMT proofs of the
# e-graph's own rewrites (compiler/egraph.fd egraph_smt, written by
# `fid test-rules --smt <dir>` beside the obligations of rules.fd).
#
# Usage: tests/tooling/egraph-smt-check.sh   (FID=<compiler>, default
#        release/fid.exe; OUT=<dir>, default build/egraph-smt; needs python
#        with the z3 module: pip install z3-solver)
#
# Checks, one line each (PASS/FAIL), exit status the number of FAILs:
#  1. `fid test-rules --smt` writes the e-graph's files: every rule family
#     (comm flip ident zero double shift reassoc same invol unaryfold fold
#     castfold float) has at least one, at least 100 in all;
#  2. z3 proves every one (`unsat`);
#  3. for each RL11 mutant (FID_NAT_MUTATE=egcomm, egflip, egfold, egident,
#     egzero, egdouble, egsame) at least one obligation of that family is
#     no longer proved: the obligations follow the rules the pass applies,
#     so a broken rule is caught by the solver and not only by the suite.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/egraph-smt}"
SOLVE="tests/tooling/egraph_smt_solve.py"
bad=0
pass() { echo "PASS $1"; }
fail() { echo "FAIL $1"; bad=$((bad + 1)); }

PY=""
for p in python3 python; do
  if command -v $p > /dev/null 2>&1 && $p -c "import z3" > /dev/null 2>&1; then PY=$p; break; fi
done

rm -rf "$OUT"
mkdir -p "$OUT/base"
FID_NAT_MUTATE= "$FID" test-rules --smt "$OUT/base" > "$OUT/base.log" 2>&1
rc=$?
n=$(ls "$OUT/base"/eg_*.smt2 2> /dev/null | wc -l)
missing=""
for fam in comm flip ident zero double shift reassoc same invol unaryfold fold castfold float; do
  ls "$OUT/base"/eg_${fam}_*.smt2 > /dev/null 2>&1 || missing="$missing $fam"
done
if [ $rc -eq 0 ] && [ "$n" -ge 100 ] && [ -z "$missing" ] && grep -q "e-graph SMT-LIB files in" "$OUT/base.log"; then
  pass "test-rules --smt wrote $n e-graph obligation files"
else
  fail "test-rules --smt: exit $rc, $n e-graph files, families missing:${missing:- none} (see $OUT/base.log)"
fi

if [ -z "$PY" ]; then
  fail "no python with z3 (pip install z3-solver): checks 2 and 3 not run"
  exit $bad
fi

if "$PY" "$SOLVE" "$OUT/base" eg_ > "$OUT/base.solve" 2>&1; then
  pass "z3 proves every e-graph obligation ($(tail -n 1 "$OUT/base.solve"))"
else
  fail "z3: $(tail -n 1 "$OUT/base.solve") (see $OUT/base.solve)"
fi

# mutant -> the family whose obligations it must break
for m in egcomm:comm egflip:flip egfold:fold egident:ident egzero:zero egdouble:double egsame:same; do
  mu="${m%%:*}"; fam="${m#*:}"
  mkdir -p "$OUT/$mu"
  FID_NAT_MUTATE="$mu" "$FID" test-rules --smt "$OUT/$mu" > "$OUT/$mu.log" 2>&1
  "$PY" "$SOLVE" "$OUT/$mu" "eg_${fam}_" > "$OUT/$mu.solve" 2>&1
  if grep -q "^NOT PROVED (sat)" "$OUT/$mu.solve"; then
    pass "FID_NAT_MUTATE=$mu: z3 finds a counterexample in eg_${fam}_*"
  else
    fail "FID_NAT_MUTATE=$mu: every eg_${fam}_* still proved (see $OUT/$mu.solve)"
  fi
done
exit $bad
