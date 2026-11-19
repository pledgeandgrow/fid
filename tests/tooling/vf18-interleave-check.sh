#!/bin/bash
# vf18-interleave-check.sh — VF18 (competitive roadmap): checks the
# interleaving harness scripts/verify/interleave.py and its programs.
#
# Usage: tests/tooling/vf18-interleave-check.sh
#   FID=<compiler>   default release/fid.exe (release/fid off Windows)
#   OUT=<dir>        default build/vf18-check
#   BACKENDS=<list>  default ccompile,emit
#   N=<runs>         runs per program and backend in checks 1 and 4, default 40
#
# Checks, one line each (PASS/FAIL), exit status the number of FAILs:
#  1. tests/verify/interleave.fd (threads, atomics, a mutex, channels,
#     parallel branches, green tasks, a task pipeline, async fns awaited out
#     of order, select over two channels) passes N runs on every backend:
#     every run prints its '// Expected output' block
#  2. `fid run` of interleave.fd prints the same block (tasks run to their
#     end there: one schedule)
#  3. tests/verify/interleave_racy.fd (a lost-update race on purpose) FAILS
#     on ccompile: the harness sees a race; the failing-seeds line and the
#     replay command are printed
#  4. --sched-random with N runs of tests/bootstrap/test_runtime_sched_random.fd
#     (all tasks, the runtime's seeded scheduler) passes on every backend
#  5. --seeds replays: `--seeds 5,9` runs exactly those two seeds, each named
#     on its own line, and --record appends one table row per backend
set -u
cd "$(dirname "$0")/../.." || exit 1
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) FID="${FID:-release/fid.exe}" ;; *) FID="${FID:-release/fid}" ;; esac
OUT="${OUT:-build/vf18-check}"
BACKENDS="${BACKENDS:-ccompile,emit}"
N="${N:-40}"
export CC="${CC:-zig cc}"
PY=python3
command -v "$PY" > /dev/null 2>&1 || PY=python
mkdir -p "$OUT"
fails=0
pass() { echo "PASS $1"; }
fail() { echo "FAIL $1"; fails=$((fails + 1)); }

# 1
log="$OUT/c1.log"
if "$PY" scripts/verify/interleave.py --fid "$FID" -n "$N" --shards 4 --backends "$BACKENDS" \
     --out "$OUT/c1" > "$log" 2>&1; then
  pass "1 interleave.fd: $N runs on $BACKENDS, every run prints the expected block"
else
  fail "1 interleave.fd: $(grep -E '^(FAIL|PASS)|build failed' "$log" | tr '\n' ' ' | cut -c1-400)"
fi

# 2
want=$(sed -n '/^\/\/ Expected output/,/^$/p' tests/verify/interleave.fd | sed -n 's|^// ||p' | grep -v 'Expected output')
got=$("$FID" run tests/verify/interleave.fd 2> "$OUT/c2.err" | tr -d '\r')
if [ -n "$want" ] && [ "$got" = "$want" ]; then
  pass "2 fid run interleave.fd prints the expected block"
else
  fail "2 fid run interleave.fd: got $(echo "$got" | tr '\n' ' ' | cut -c1-200)"
fi

# 3
log="$OUT/c3.log"
"$PY" scripts/verify/interleave.py --fid "$FID" -n 12 --shards 4 --backends ccompile \
  --programs tests/verify/interleave_racy.fd --out "$OUT/c3" > "$log" 2>&1
rc=$?
if [ "$rc" != 0 ] && grep -q '^FAIL interleave_racy ccompile' "$log" \
   && grep -q 'failing seeds: .*replay with: .*--seeds ' "$log"; then
  pass "3 the racy program fails, with its failing seeds and a replay command"
else
  fail "3 the racy program was not reported (exit $rc): $(head -3 "$log" | tr '\n' ' ' | cut -c1-300)"
fi

# 4
log="$OUT/c4.log"
if "$PY" scripts/verify/interleave.py --fid "$FID" -n "$N" --shards 8 --sched-random \
     --backends "$BACKENDS" --programs tests/bootstrap/test_runtime_sched_random.fd \
     --timeout 120 --out "$OUT/c4" > "$log" 2>&1; then
  pass "4 --sched-random: $N seeds of test_runtime_sched_random on $BACKENDS"
else
  fail "4 --sched-random: $(grep -E '^(FAIL|PASS)|build failed' "$log" | tr '\n' ' ' | cut -c1-400)"
fi

# 5
log="$OUT/c5.log"
rec="$OUT/c5-record.md"
rm -f "$rec"
"$PY" scripts/verify/interleave.py --fid "$FID" --seeds 5,9 --shards 2 --backends "$BACKENDS" \
  --out "$OUT/c5" --record "$rec" > "$log" 2>&1
rc=$?
nb=$(echo "$BACKENDS" | tr ',' '\n' | grep -c .)
s5=$(grep -c '^     seed 5: ' "$log")
s9=$(grep -c '^     seed 9: ' "$log")
other=$(grep '^     seed ' "$log" | grep -vc '^     seed [59]: ')
rows=$(grep -c '^| 20' "$rec" 2> /dev/null || echo 0)
if [ "$rc" = 0 ] && [ "$s5" = "$nb" ] && [ "$s9" = "$nb" ] && [ "$other" = 0 ] && [ "$rows" = "$nb" ]; then
  pass "5 --seeds 5,9 ran those seeds only; --record wrote $rows row(s)"
else
  fail "5 --seeds: exit $rc, seed 5 x$s5, seed 9 x$s9, others $other, rows $rows (want $nb)"
fi

echo "vf18-interleave-check: $fails failure(s)"
exit "$fails"
