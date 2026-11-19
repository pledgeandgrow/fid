#!/bin/bash
# vf4-run.sh — the whole-suite runs of roadmap VF4 (every optimizer pass on
# and off: no output may change), on the host (Windows: pe_ir for `emit`,
# the machine layer for `nat`).
#
#   FID=release/fid.exe scripts/verify/vf4-run.sh [peir|iropt|all]
#
#   peir   builds the base and the five PEIR_* variants of compiler/pe_ir.fd
#          from a snapshot of the tree (scripts/verify/build-variant.sh,
#          8 units at -O2 when $FID has `split-c`), then runs
#          `variants.py toggle --only 'peir-*'` over every test of
#          tests/bootstrap that is not an expected failure, plus 20
#          generated programs (--gen 10: ten of gen_vs_c.py, ten of
#          gen_programs.py)
#   iropt  every FID_IROPT_OFF switch the tree's compiler reads
#          (`variants.py list`: iro_pass_on and IME_DEFAULT_ON) over the
#          same tests, on $FID itself (no build), pruned on the
#          FID_IROPT_STATS counters (--prune-stats, 5% of the pruned builds
#          made anyway to check them)
#   all    both (default)
#
# Environment: OUT (default build/vf4), J (parallel builds, default 4),
# TESTS (default 'tests/bootstrap/*.fd'), GEN (default 10: 2 x GEN
# programs), IROPT_BACKENDS (default emit; emit,nat adds the machine
# layer), TREE (a snapshot to build from; default: one is copied into
# $OUT/tree, AGENTS.md "Known Bugs" 26: other sessions edit the tree).
#
# Every result is kept in <out>/toggle-results.jsonl (--resume), so a run of
# many hours can be stopped and started again with the same command; it
# goes on with the same base compiler. Reports: $OUT/peir/toggle-report.txt,
# $OUT/iropt/toggle-report.txt, and $OUT/summary.txt (both, with the
# command lines). Exit status: 0 when no output changed, 1 when one did (or
# a pruned counter was wrong), 2 when a variant did not build.
#
# Record each run in docs/verification/ (vf3-vf4-2026-11-17.md has the
# first) and file each output change in docs/verification/miscompile-log.md
# with a tests/verify/bugs/ program.
set -u
cd "$(dirname "$0")/../.."
PART="${1:-all}"
FID="${FID:-release/fid.exe}"
case "$FID" in /*|?:*) ;; *) FID="$(pwd)/$FID" ;; esac
OUT="${OUT:-build/vf4}"
J="${J:-4}"
TESTS="${TESTS:-tests/bootstrap/*.fd}"
GEN="${GEN:-10}"
IROPT_BACKENDS="${IROPT_BACKENDS:-emit}"
PY="${PYTHON:-python}"
command -v "$PY" > /dev/null 2>&1 || PY=python3
mkdir -p "$OUT"
[ -x "$FID" ] || [ -x "$FID.exe" ] || { echo "no compiler at $FID (set FID)"; exit 2; }

if [ -z "${TREE:-}" ]; then
  TREE="$OUT/tree"
  if [ ! -d "$TREE/compiler" ]; then
    mkdir -p "$TREE"
    cp -r compiler stdlib "$TREE/"
    echo "snapshot of compiler/ and stdlib/ in $TREE"
  fi
fi

status=0
: > "$OUT/summary.txt"
run_part() {  # name, then the arguments of variants.py
  local name="$1"; shift
  echo "== $name: $PY scripts/verify/variants.py $*" | tee -a "$OUT/summary.txt"
  "$PY" scripts/verify/variants.py "$@"
  local rc=$?
  [ -f "$OUT/$name/toggle-report.txt" ] && cat "$OUT/$name/toggle-report.txt" >> "$OUT/summary.txt"
  echo "== $name: exit $rc" | tee -a "$OUT/summary.txt"
  [ "$rc" -gt "$status" ] && status=$rc
}

if [ "$PART" = peir ] || [ "$PART" = all ]; then
  run_part peir toggle --fid "$FID" --tree "$TREE" --only 'peir-*' --backends emit \
    --tests "$TESTS" --no-fail-tests --gen "$GEN" --resume -j "$J" --out "$OUT/peir"
fi
if [ "$PART" = iropt ] || [ "$PART" = all ]; then
  run_part iropt toggle --fid "$FID" --tree "$TREE" --only 'iropt-*' --backends "$IROPT_BACKENDS" \
    --tests "$TESTS" --no-fail-tests --gen "$GEN" --prune-stats --resume -j "$J" --out "$OUT/iropt"
fi
echo "summary: $OUT/summary.txt (exit $status)"
exit "$status"
