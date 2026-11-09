#!/bin/bash
# rules-smt.sh — prove the rewrite rules of compiler/rules.fd with an SMT
# solver (own-backend roadmap ME17): `fid test-rules --smt <dir>` writes one
# obligation per rule and width, asserting the two sides differ; the solver
# must answer `unsat` for every one.
# Usage: scripts/rules-smt.sh [fid] [solver]   (defaults: release/fid.exe, z3)
set -u
cd "$(dirname "$0")/.." || exit 1
FID="${1:-release/fid.exe}"
SOLVER="${2:-z3}"
DIR=build/rules-smt
mkdir -p "$DIR"
"$FID" test-rules --smt "$DIR" > /dev/null || { echo "fid test-rules failed"; exit 1; }
# The z3-solver wheel has the library but no z3 command: fall back to it.
PY=""
for p in python3 python; do
  if command -v $p > /dev/null 2>&1 && $p -c "import z3" > /dev/null 2>&1; then PY=$p; break; fi
done
if ! command -v "$SOLVER" > /dev/null 2>&1; then
  if [ -z "$PY" ]; then
    echo "no $SOLVER: the obligations are in $DIR (install z3, e.g. pip install z3-solver)"; exit 2
  fi
  SOLVER="smt_py"
fi
smt_py() {
  "$PY" -c "import sys, z3
s = z3.Solver(); s.from_file(sys.argv[1]); print(s.check())" "$1"
}
bad=0; n=0
for f in "$DIR"/*.smt2; do
  n=$((n + 1))
  r=$("$SOLVER" "$f" 2>&1 | tr -d '\r' | head -n 1)
  if [ "$r" != "unsat" ]; then echo "NOT PROVED ($r): $(head -n 1 "$f")"; bad=$((bad + 1)); fi
done
echo "$n obligations, $((n - bad)) proved, $bad not"
exit $bad
