#!/usr/bin/env bash
# RL4: a miscompile is fixed only with a test that fails without the fix.
#
# Checks against docs/verification/miscompile-log.md:
#   1. Every row whose status says "fixed" must name at least one test file
#      that exists: a path under tests/, or a bare name found in
#      tests/bootstrap/, tests/verify/bugs/ or tests/conformance/ (repros
#      are promoted to tests/bootstrap/ when fixed, VF20), or a check
#      script `tests/tooling/<name>.sh` for an entry that is not one
#      program's output (R1: two builds compared; run with FID=<compiler>,
#      exit status 0 = pass). A compiler or
#      stdlib source file (`compiler/iropt.fd`) is not a test and does not
#      count. Rows of the section "Earlier miscompiles" (fixed before the
#      log existed) must name a test as well.
#   2. Every repro file in tests/verify/bugs/ must be named in the log, so
#      no repro is added without a log entry.
#   3. --run: the fixed entries' tests and every repro are run with
#      scripts/verify/expect.py; the fixed entries' tests must pass.
#   4. --before <old fid>: the fixed entries' tests are run with a compiler
#      built WITHOUT the fix (the commit before it, or the release before);
#      each must FAIL there, which is what shows the test catches the bug.
#      Use it when an entry is marked fixed: `--before` with the old
#      compiler, `--run` with the new one. --only M9,M10 limits 3 and 4 to
#      those entries.
#   5. --new-since <git rev>: 3 and 4 only for the entries that are fixed
#      now and were not fixed (or not in the log) at <rev>: the entries this
#      change fixes. With none, 3 and 4 have nothing to run. CI runs
#      `--run --before <base fid> --new-since <base>` with the base commit's
#      compiler (.github/workflows/bench.yml, which builds both), so the
#      before-step is automatic for every fix (RL4).
#
# Rows of "Earlier miscompiles" that name a test are checked like the others
# (the test must exist) and run by --run; --before skips them (no recent
# compiler has those bugs). A row there without a test fails the gate.
#
# Checks 1 and 2 read files only (no compiler), so CI runs them on every
# push (.github/workflows/ci.yml, job `gates`).
#
# Usage: bash scripts/miscompile-gate.sh [--fid path/to/fid] [--run]
#                                        [--before path/to/old-fid] [--only IDS]
#                                        [--new-since REV]
set -u
cd "$(dirname "$0")/.."

LOG=${MISCOMPILE_LOG:-docs/verification/miscompile-log.md}   # (a copy, to test the gate)
BUGS=tests/verify/bugs
FID=release/fid.exe
[ -x "$FID" ] || FID=release/fid
RUN=0
BEFORE=""
ONLY=""
NEW_SINCE=""
PY=python3
export PYTHONIOENCODING=utf-8   # expect.py prints compiler messages (Windows console)
command -v python3 > /dev/null 2>&1 || PY=python
while [ $# -gt 0 ]; do
    case "$1" in
        --fid) FID="$2"; shift 2 ;;
        --run) RUN=1; shift ;;
        --before) BEFORE="$2"; shift 2 ;;
        --only) ONLY="$2"; shift 2 ;;
        --new-since) NEW_SINCE="$2"; shift 2 ;;
        *) echo "unknown arg: $1" >&2; exit 2 ;;
    esac
done

fail=0
FIXED_TESTS=$(mktemp)
BASE_LOG=$(mktemp)
trap 'rm -f "$FIXED_TESTS" "$BASE_LOG"' EXIT
# The log as it was at --new-since (empty when it did not exist then).
NEW_FLAG=""
if [ -n "${NEW_SINCE:-}" ]; then
    NEW_FLAG=1
    git show "$NEW_SINCE:docs/verification/miscompile-log.md" > "$BASE_LOG" 2> /dev/null || : > "$BASE_LOG"
fi

# 1. Fixed entries must name an existing test. Writes "<id> <test path>"
#    lines for each fixed entry's tests to $FIXED_TESTS.
"$PY" - "$LOG" "$FIXED_TESTS" "$ONLY" "$NEW_FLAG" "$BASE_LOG" <<'PY' || fail=1
import os
import re
import sys

log, out_path, only = sys.argv[1], sys.argv[2], sys.argv[3]
new_since, base_log = sys.argv[4] == "1", sys.argv[5]
only = {x.strip() for x in only.split(",") if x.strip()}
DIRS = ["tests/bootstrap", "tests/verify/bugs", "tests/conformance"]


def test_path(name):
    """The test file a backquoted name refers to, or None."""
    if name.startswith(("compiler/", "stdlib/", "src/", "scripts/")):
        return None  # source of the compiler, not a test
    if name.endswith(".sh"):
        # a check script (an entry that is not one program's output, R1):
        # only under tests/tooling/, run with FID=<compiler>, exit 0 = pass
        ok = name.startswith("tests/tooling/") and os.path.exists(name)
        return name if ok else None
    if name.startswith("tests/") and os.path.exists(name):
        return name
    for d in DIRS:
        p = os.path.join(d, os.path.basename(name))
        if os.path.exists(p):
            return p.replace(os.sep, "/")
    return None


def fixed_rows(path):
    """(section, id, cells) of each row of the log whose status is fixed."""
    section = ""
    out = []
    for line in open(path, encoding="utf-8"):
        if line.startswith("## "):
            section = line[3:].strip()
            continue
        if not line.startswith("|") or "fixed" not in line.lower():
            continue
        cells = [c.strip() for c in line.strip().strip("|").split("|")]
        if len(cells) < 3 or set(cells[0]) <= set("-: "):
            continue
        if not cells[-1].lower().startswith("fixed"):
            continue
        # ids of the earlier rows have spaces ("AGENTS 4"): one word here
        out.append((section, cells[0].replace(" ", "_"), cells))
    return out


missing, checked = [], 0
legacy, legacy_tested = 0, 0
rows = []
# --new-since: only the entries fixed now and not fixed at that revision
new_ids = None
if new_since:
    before = {b for _, b, _ in fixed_rows(base_log)} if os.path.getsize(base_log) else set()
    new_ids = {b for _, b, _ in fixed_rows(log)} - before
    print("RL4 gate: entries fixed since the base: "
          + (", ".join(sorted(new_ids)) if new_ids else "none"))
for section, bug, cells in fixed_rows(log):
    old = section.startswith("Earlier miscompiles")
    tests = []
    for n in re.findall(r"`([^`\s]+\.(?:fd|sh))`", " ".join(cells)):
        p = test_path(n)
        if p and p not in tests:
            tests.append(p)
    if old:
        legacy += 1
        if tests:
            legacy_tested += 1
        else:
            # every earlier entry names a test since Nov 18 (RL4): keep it so
            missing.append(bug)
            print(f"RL4 gate: earlier entry {bug} names no test file")
    else:
        checked += 1
        if not tests:
            missing.append(bug)
            print(f"RL4 gate: {bug} is marked fixed but names no test file "
                  f"(tests/bootstrap/, tests/verify/bugs/, tests/conformance/)")
    if only and bug not in only:
        continue
    if new_ids is not None and bug not in new_ids:
        continue
    rows += [f"{bug} {t} {'earlier' if old else 'logged'}" for t in tests]
with open(out_path, "w", encoding="utf-8", newline="\n") as f:  # no \r for `read`
    f.write("".join(r + "\n" for r in rows))
print(f"RL4 gate: {checked} fixed entries checked, {len(missing)} without a test; "
      f"{legacy} earlier entries (before the log), {legacy_tested} of them name a test")
sys.exit(1 if missing else 0)
PY

# 2. Every repro file must appear in the log.
for f in "$BUGS"/*.fd; do
    [ -e "$f" ] || continue
    name=$(basename "$f")
    if ! grep -q "$name" "$LOG"; then
        echo "RL4 gate: repro $name exists but is not named in $LOG" >&2
        fail=1
    fi
done

# 3. The fixed entries' tests must pass now; the open repros are run too
#    (their result is reported, an open bug's repro may still fail).
#    With --new-since and no newly fixed entry there is nothing to run.
if [ "$RUN" = 1 ] && [ -n "$NEW_FLAG" ] && [ ! -s "$FIXED_TESTS" ]; then
    echo "RL4 gate: no entry fixed since $NEW_SINCE: nothing to run"
elif [ "$RUN" = 1 ]; then
    tests=$(cut -d' ' -f2 "$FIXED_TESTS" | sort -u | grep -v '\.sh$')
    if [ -n "$tests" ]; then
        # shellcheck disable=SC2086
        "$PY" scripts/verify/expect.py --fid "$FID" $tests || fail=1
    fi
    # check scripts (tests/tooling/*.sh) of entries that are not one program
    for t in $(cut -d' ' -f2 "$FIXED_TESTS" | sort -u | grep '\.sh$'); do
        if FID="$FID" bash "$t"; then
            echo "ok   $t"
        else
            echo "FAIL $t"
            fail=1
        fi
    done
    # --only names the entries to check: the other repros are not asked for
    # (some run long or use much memory: p3)
    if [ -z "$ONLY" ]; then
        echo "== open repros (informational)"
        "$PY" scripts/verify/expect.py --fid "$FID" -q "$BUGS" || true
    fi
fi

# 4. With the compiler from before the fix, each fixed entry's test fails.
#    Rows of "Earlier miscompiles" are skipped: no recent compiler has them.
if [ -n "$BEFORE" ] && [ -s "$FIXED_TESTS" ]; then
    # a compiler that does not start would make every test "fail"
    if ! "$BEFORE" version > /dev/null 2>&1; then
        echo "RL4 gate: --before $BEFORE does not run" >&2
        exit 2
    fi
    while read -r bug t kind; do
        [ -n "$t" ] || continue
        [ "$kind" = earlier ] && continue
        case "$t" in
            *.sh) FID="$BEFORE" bash "$t" > /dev/null 2>&1 < /dev/null ;;
            *) "$PY" scripts/verify/expect.py --fid "$BEFORE" -q "$t" > /dev/null 2>&1 < /dev/null ;;
        esac
        if [ $? = 0 ]; then
            echo "RL4 gate: $bug: $t passes with $BEFORE too: it does not catch the bug"
            fail=1
        else
            echo "RL4 gate: $bug: $t fails without the fix, as it must"
        fi
    done < "$FIXED_TESTS"
fi

if [ "$fail" = 0 ]; then
    echo "miscompile gate: ok"
fi
exit $fail
