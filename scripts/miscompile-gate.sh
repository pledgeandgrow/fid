#!/usr/bin/env bash
# RL4: a miscompile is fixed only with a test that fails without the fix.
#
# Two checks against docs/verification/miscompile-log.md:
#   1. Every row whose status contains "fixed" must name at least one test
#      file that exists in tests/bootstrap/ (repros are promoted there when
#      fixed, per VF20) — or the named repro must still exist and pass.
#   2. Every repro file in tests/verify/bugs/ must be named in the log, so
#      no repro is added without a log entry.
#
# Usage: bash scripts/miscompile-gate.sh [--fid path/to/fid] [--run]
#   --run also executes the repros of FIXED entries and asserts they pass
#   (the test would have failed without the fix).
set -u
cd "$(dirname "$0")/.."

LOG=docs/verification/miscompile-log.md
BUGS=tests/verify/bugs
BOOT=tests/bootstrap
FID=release/fid.exe
RUN=0
while [ $# -gt 0 ]; do
    case "$1" in
        --fid) FID="$2"; shift 2 ;;
        --run) RUN=1; shift ;;
        *) echo "unknown arg: $1" >&2; exit 2 ;;
    esac
done

fail=0

# 1. Fixed entries must name an existing promoted test or repro.
python3 - "$LOG" "$BOOT" "$BUGS" <<'PY' || fail=1
import re, sys, os
log, boot, bugs = sys.argv[1], sys.argv[2], sys.argv[3]
missing = []
for line in open(log, encoding='utf-8'):
    if '|' not in line or 'fixed' not in line:
        continue
    cells = [c.strip() for c in line.split('|')]
    if len(cells) < 7:
        continue
    status = cells[-2].lower()
    if 'fixed' not in status:
        continue
    bug = cells[1]
    named = re.findall(r'`([^`]+\.fd)`', ' '.join(cells))
    ok = any(os.path.exists(n) or
             os.path.exists(os.path.join(boot, os.path.basename(n))) or
             os.path.exists(os.path.join(bugs, os.path.basename(n)))
             for n in named)
    if not ok:
        missing.append(bug)
        print(f"RL4 gate: {bug} marked fixed but names no test in "
              f"tests/bootstrap/ or tests/verify/bugs/")
sys.exit(1 if missing else 0)
PY

# 2. Every repro file must appear in the log.
for f in "$BUGS"/*.fd; do
    name=$(basename "$f")
    if ! grep -q "$name" "$LOG"; then
        echo "RL4 gate: repro $name exists but is not named in $LOG" >&2
        fail=1
    fi
done

# 3. Optionally run the fixed bugs' repros — they must pass now.
if [ "$RUN" = 1 ]; then
    python3 scripts/verify/expect.py --fid "$FID" "$BUGS" || fai