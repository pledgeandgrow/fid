#!/bin/bash
# scripts/bench-noise-check.sh — the 5% run-time gate does not fail an
# unchanged compiler (competitive roadmap BM9).
#
#   FID=release/fid-dev bash scripts/bench-noise-check.sh [rounds]
#   SUITES="game work" ROUNDS=5 LIMIT_PCT=5 bash scripts/bench-noise-check.sh
#
# bench.yml fails a change when a program is more than LIMIT_PCT (5) slower
# than on the base commit (bench/check.sh --ab: base and head run in turn,
# a program over the limit measured again before it fails). A limit that
# tight is only useful if timer and machine noise stay under it: this script
# builds every program once with FID as both base and head (the two
# executables are the same program) and runs the --ab comparison ROUNDS times
# (default 3) on those executables (AB_REUSE=1). Any SLOW line is a false
# failure. It prints, per program and mode, the largest head/base difference
# seen, and exits 1 when a round failed.
#
# scripts/bench-gate-check.sh is the other half: an injected 2x slowdown
# must fail the gate.
set -u
cd "$(dirname "$0")/.." || exit 1
X=""
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) X=".exe" ;; esac
if [ -z "${FID:-}" ]; then
  for c in release/stage0/fid$X release/fid$X release/fid-dev$X; do
    if [ -x "$c" ]; then FID="$c"; break; fi
  done
fi
[ -n "${FID:-}" ] || { echo "no fid compiler found (set FID)"; exit 2; }
FID="$(cd "$(dirname "$FID")" && pwd)/$(basename "$FID")"
ROUNDS="${1:-${ROUNDS:-3}}"
export RUNS="${RUNS:-5}"
export LIMIT_PCT="${LIMIT_PCT:-5}"
export SUITES="${SUITES:-}"
LOG=$(mktemp -d)
trap 'rm -rf "$LOG"' EXIT

# a fresh build for round 1: drop the --ab executables of an earlier run
for src in bench/fides/*.fd; do
  b=$(basename "$src" .fd)
  rm -f bench/out/ab-"$b"-*-base$X bench/out/ab-"$b"-*-head$X
done
for s in $SUITES; do
  for src in bench/"$s"/fides/*.fd; do
    rm -f bench/out/ab-"$s-$(basename "$src" .fd)"-*-base$X bench/out/ab-"$s-$(basename "$src" .fd)"-*-head$X
  done
done

false_fail=0
for r in $(seq 1 "$ROUNDS"); do
  reuse=1; [ "$r" = 1 ] && reuse=0
  AB_REUSE=$reuse BASE_FID="$FID" FID="$FID" bash bench/check.sh --ab > "$LOG/$r.log" 2>&1
  rc=$?
  n=$(grep -c 'SLOW' "$LOG/$r.log")
  b=$(grep -c 'FAIL' "$LOG/$r.log")
  echo "round $r: exit $rc, $n SLOW, $(grep -c 'noise' "$LOG/$r.log") re-measured after a noisy first round"
  if [ "$b" -gt 0 ]; then echo "  a build failed:"; grep 'FAIL' "$LOG/$r.log" | sed 's/^/  /'; exit 2; fi
  [ "$rc" != 0 ] && { false_fail=$((false_fail + 1)); grep 'SLOW' "$LOG/$r.log" | sed 's/^/  /'; }
done

# the largest difference per program, from the "ok"/"SLOW" lines
echo
echo "largest head/base difference per program over $ROUNDS rounds (same executable, LIMIT_PCT=$LIMIT_PCT):"
cat "$LOG"/*.log | awk '
  /^  ok / { name = $2 " " $3; h = $4; s = $0; sub(/.*base /, "", s); b = s + 0 }
  /^  SLOW / { name = $2 " " $3; sub(/:$/, "", name); h = $4 + 0; s = $0; sub(/.*base /, "", s); b = s + 0 }
  /^  (ok|SLOW) / && b > 0 { d = (h - b) * 100.0 / b; if (!(name in m) || d > m[name]) m[name] = d }
  END { for (k in m) printf "  %-28s %+6.1f%%\n", k, m[k] }' | sort
if [ "$false_fail" -gt 0 ]; then
  echo "bench noise check: $false_fail of $ROUNDS rounds failed an unchanged compiler at ${LIMIT_PCT}%"
  exit 1
fi
echo "bench noise check: $ROUNDS rounds, no false failure at ${LIMIT_PCT}%"
