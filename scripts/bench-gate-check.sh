#!/bin/bash
# scripts/bench-gate-check.sh — the slowdown gate fails on a slowdown
# (complaint roadmap CU8, release D7).
#
#   FID=release/fid-dev bash scripts/bench-gate-check.sh
#
# bench/check.sh is the gate CI runs (bench.yml: `--ab`, base and head
# compiler in turn). A gate that never fails proves nothing, so this script
# feeds it a known slowdown and checks that it fails, and feeds it no
# slowdown and checks that it passes:
#
#   1. --ab, FID as both base and head          -> must pass (exit 0)
#   2. --ab, head = a "slow fid" wrapper        -> must fail (exit 1, SLOW)
#   3. baseline mode: --save with FID into a scratch baseline, then the
#      slow wrapper against it                  -> must fail (exit 1, SLOW)
#
# The slow wrapper is FID itself, given a copy of each benchmark whose main
# runs the original main twice: every program takes about twice as long, far
# over the 10% limit (LIMIT_PCT), while the build is the same compiler.
# RUNS (default 3) runs per measurement; SUITES as for bench/check.sh.
set -u
cd "$(dirname "$0")/.." || exit 1
ROOT=$(pwd)
X=""
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) X=".exe" ;; esac
if [ -z "${FID:-}" ]; then
  for c in release/stage0/fid$X release/fid$X release/fid-dev$X; do
    if [ -x "$c" ]; then FID="$c"; break; fi
  done
fi
[ -n "${FID:-}" ] || { echo "no fid compiler found (set FID)"; exit 2; }
FID="$(cd "$(dirname "$FID")" && pwd)/$(basename "$FID")"
export RUNS="${RUNS:-3}"
export LIMIT_PCT="${LIMIT_PCT:-10}"

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT
mkdir -p "$ROOT/bench/out"

# the slow wrapper: `fid <mode> bench/<x>.fd <flags> -o <exe>` builds a copy
# of bench/<x>.fd whose main is the original main run twice
SLOW="$WORK/slowfid"
cat > "$SLOW" <<EOF
#!/bin/bash
args=()
for a in "\$@"; do
  case "\$a" in
    bench/*.fd)
      copy="bench/out/slow-\$(basename "\$a")"
      sed 's/^fn main() -> i64 {/fn bench_main_once() -> i64 {/' "\$a" > "\$copy"
      printf '\nfn main() -> i64 {\n    val first: i64 = bench_main_once();\n    return first + bench_main_once();\n}\n' >> "\$copy"
      grep -q '^fn bench_main_once() -> i64 {' "\$copy" || { echo "slowfid: no 'fn main() -> i64 {' in \$a"; exit 3; }
      a="\$copy" ;;
  esac
  args+=("\$a")
done
exec "$FID" "\${args[@]}"
EOF
chmod +x "$SLOW"

fails=0
expect() { # expect <want exit> <label> <log>
  local want=$1 got=$2 label=$3 log=$4
  if [ "$got" != "$want" ]; then
    echo "FAIL  $label: exit $got, wanted $want"; sed 's/^/      /' "$log" | tail -20; fails=$((fails + 1))
  elif [ "$want" = 1 ] && ! grep -q 'SLOW' "$log"; then
    echo "FAIL  $label: exit 1 but no SLOW line"; sed 's/^/      /' "$log" | tail -20; fails=$((fails + 1))
  else
    echo "ok    $label (exit $got)"
  fi
}

echo "== 1. --ab, the same compiler as base and head: must pass"
BASE_FID="$FID" FID="$FID" bash bench/check.sh --ab > "$WORK/1.log" 2>&1
expect 0 $? "--ab, no slowdown" "$WORK/1.log"

echo "== 2. --ab, head runs every program twice: must fail"
BASE_FID="$FID" FID="$SLOW" bash bench/check.sh --ab > "$WORK/2.log" 2>&1
r=$?
expect 1 $r "--ab, injected 2x slowdown" "$WORK/2.log"
n=$(grep -c 'SLOW' "$WORK/2.log"); echo "      $n programs reported SLOW"

echo "== 3. baseline mode: a scratch baseline, then the slow wrapper: must fail"
BENCH_BASE="$WORK/baseline.txt" FID="$FID" bash bench/check.sh --save > "$WORK/3a.log" 2>&1
r=$?
if [ "$r" != 0 ]; then
  echo "FAIL  --save: exit $r"; tail -20 "$WORK/3a.log"; fails=$((fails + 1))
else
  BENCH_BASE="$WORK/baseline.txt" FID="$SLOW" bash bench/check.sh > "$WORK/3.log" 2>&1
  expect 1 $? "baseline, injected 2x slowdown" "$WORK/3.log"
fi
rm -f "$ROOT"/bench/out/slow-*.fd

if [ "$fails" -gt 0 ]; then echo "bench gate check: $fails FAILED"; exit 1; fi
echo "bench gate check: the gate passes an unchanged compiler and fails a 2x slowdown"
