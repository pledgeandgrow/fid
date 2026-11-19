#!/bin/bash
# me-measure-all.sh [FID] [FILE...]: one docs/perf/mid-end/<label>.txt per
# opt-in mid-end pass (docs/roadmap/llvm-roadmap.md ME30).
#
# The opt-in passes are the names compiler/*.fd tests with `ime_on("...")`
# (FID_IROPT_ON); each is measured by scripts/me-measure.sh with
# FID_IROPT_ON=<pass> OFF=<pass>, so the "on" build runs it and the "off"
# build does not, everything else the same. The label is optin-<pass>.
#
#   ./scripts/me-measure-all.sh release/fid.exe                 the whole suite
#   ./scripts/me-measure-all.sh release/fid.exe tests/bootstrap/test_gvn*.fd
#   PASSES="branchfold icf" ./scripts/me-measure-all.sh release/fid.exe
#
# With no files, me-measure.sh takes the suite (every program of
# tests/bootstrap/ that runs alone): about 2 x 500 builds per pass.
set -u
cd "$(dirname "$0")/.." || exit 1
FID="${1:-release/fid.exe}"
shift 2>/dev/null
if [ -z "${PASSES:-}" ]; then
  PASSES=$(grep -oh 'ime_on("[a-z0-9_]*")' compiler/*.fd | sed 's/ime_on("\(.*\)")/\1/' | sort -u | tr '\n' ' ')
fi
if [ -z "$PASSES" ]; then
  echo "me-measure-all: no opt-in passes found in compiler/*.fd" >&2
  exit 1
fi
# Since Nov 18 the run is scripts/me-measure-suite.py: the same files, with
# the default build shared by every pass (N + P x N builds, not 2 x P x N),
# JOBS builds at once, results cached under build/me-measure/ so a stopped
# run goes on, and the table for docs/perf/mid-end/README.md written to
# docs/perf/mid-end/optin-suite.md. SERIAL=1 keeps the old loop (two builds
# per program and pass through scripts/me-measure.sh).
if [ "${SERIAL:-0}" != 1 ]; then
  PY=python3
  command -v python3 > /dev/null 2>&1 || PY=python
  exec "$PY" scripts/me-measure-suite.py --fid "$FID" --passes "$PASSES" --jobs "${JOBS:-1}" \
    ${CHECK_SHARE:+--check-share "$CHECK_SHARE"} "$@"
fi
for p in $PASSES; do
  echo "== $p"
  FID_IROPT_ON="$p" OFF="$p" bash scripts/me-measure.sh "optin-$p" "$FID" "$@"
done
