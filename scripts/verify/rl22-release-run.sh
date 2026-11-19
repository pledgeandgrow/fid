#!/bin/bash
# rl22-release-run.sh — the RL22 release run and its record (own-backend
# roadmap RL22: a release ships only when the C path and our own backend
# agree on the suite; docs/reference/releasing.md step 2).
#
# 1. `AGREE=1 scripts/suite-all.sh` with $FID on every lane this host runs
#    (or the lanes given), the whole suite unless TESTS is set;
# 2. scripts/verify/rl22-paths.py on that run: each C-path lane against the
#    own-backend lanes of its target, each disagreement with its cause and
#    whether scripts/verify/rl22-recorded.txt records it;
# 3. one page, $MD (default $OUT/rl22-release-<date>.md), holding how it was
#    run, the per-lane report (scripts/verify/suite-all-report.py) and the
#    path comparison: copy it to docs/verification/ to record the run.
#
#   FID=release/fid.exe JOBS=4 CC="zig cc" bash scripts/verify/rl22-release-run.sh
#   FID=$FID TESTS="hello.fd test_closure.fd" bash scripts/verify/rl22-release-run.sh \
#       c:x86_64-windows native:x86_64-windows-pe          # a smoke run
#
# Environment: FID (default release/fid.exe), OUT (default build/rl22), MD,
# JOBS, CC, WSL_DISTRO, TESTS, REUSE as for scripts/suite-all.sh and
# scripts/cross-test.sh.
# Exit status: suite-all.sh's (0 only when every lane passes and no two lanes
# disagree: the release gate), or with PAIRS=1 rl22-paths.py's (0 when every
# C-path/own-backend disagreement is recorded).
set -u
cd "$(dirname "$0")/../.." || exit 2
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/rl22}"
DATE=$(date -u +%Y-%m-%d)
MD="${MD:-$OUT/rl22-release-$DATE.md}"
[ -x "$FID" ] || { echo "rl22-release-run: no compiler $FID (set FID)"; exit 2; }
mkdir -p "$OUT"
PY=""
for p in python3 python; do
    if command -v "$p" > /dev/null 2>&1 && "$p" -c 'import sys; sys.exit(sys.version_info[0] < 3)' 2> /dev/null; then PY=$p; break; fi
done
[ -n "$PY" ] || { echo "rl22-release-run: needs Python 3"; exit 2; }

ver=$("$FID" version 2> /dev/null | tr -d '\r' | head -n 1)
host="$(uname -s) $(uname -m)"
t0=$(date +%s)
echo "rl22-release-run: $FID (${ver:-no version}), OUT=$OUT, ${TESTS:+TESTS=$TESTS, }lanes: ${*:-every lane this host runs}"
AGREE=1 FID="$FID" OUT="$OUT" bash scripts/suite-all.sh "$@" 2>&1 | tee "$OUT/suite-all.log"
st=${PIPESTATUS[0]}
t1=$(date +%s)
lanes=$(tr -d '\r' < "$OUT/suite-all.log" | sed -n 's/^lanes: //p' | tail -n 1)
"$PY" scripts/verify/rl22-paths.py "$OUT" --lanes "$lanes" --md "$OUT/paths.md" \
    --compiler "$FID (${ver:-?}), $DATE" > "$OUT/paths.txt" 2>&1
pst=$?
cat "$OUT/paths.txt"

{
    echo "# RL22 release run, $DATE"
    echo
    echo "Own-backend roadmap RL22: the suite on every lane with \`AGREE=1\`, then"
    echo "each C-path lane compared with the own-backend lanes of its target."
    echo
    echo "- Compiler: \`$FID\` (${ver:-no version string})"
    echo "- Host: $host; JOBS=${JOBS:-8}; CC=${CC:-default}; ${TESTS:+TESTS=$TESTS; }$(( (t1 - t0) / 60 )) minutes"
    echo "- Lanes: $lanes"
    echo "- \`AGREE=1 scripts/suite-all.sh\`: exit $st ($( [ "$st" = 0 ] && echo "every lane passes, no disagreement: the release gate passes" || echo "the release gate fails" ))"
    echo "- \`scripts/verify/rl22-paths.py\`: exit $pst ($( [ "$pst" = 0 ] && echo "every C-path/own-backend disagreement is recorded" || echo "some disagreement is neither fixed nor recorded" ))"
    echo
    echo "Command: \`AGREE=1 FID=$FID OUT=$OUT bash scripts/suite-all.sh $*\`"
    echo
    # the two reports, their headings one level down
    if [ -f "$OUT/paths.md" ]; then sed 's/^# /## /' "$OUT/paths.md"; echo; fi
    if [ -f "$OUT/report.md" ]; then sed -e 's/^### /#### /' -e 's/^## /### /' -e 's/^# /## /' "$OUT/report.md"; fi
} > "$MD"
echo "rl22-release-run: suite-all exit $st, paths exit $pst; record: $MD"
echo "  (to record it: cp $MD docs/verification/)"
if [ "${PAIRS:-}" = 1 ]; then exit "$pst"; fi
exit "$st"
