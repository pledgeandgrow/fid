#!/bin/bash
# bench/check.sh — fail on a slowdown (release D7).
#
#   ./bench/check.sh            compare with bench/baseline.txt; exit 1 when a
#                               Fides program got more than 10% slower
#   ./bench/check.sh --save     record the current times as the baseline
#   LIMIT_PCT=5 ./bench/check.sh   a tighter threshold
#   FID=path/to/fid             the compiler to measure (default: as run.sh)
#   BENCH_BASE=file             baseline file (default bench/baseline.txt); CI
#                               saves one with the base commit's compiler and
#                               checks the head's against it on the same runner
#
# Measures every bench/fides/*.fd natively (`fid emit`) and through C
# (`fid ccompile`), best of RUNS (default 5) runs each. Meant for CI and for a
# quick check before a change to code generation lands. Times depend on the
# machine: record the baseline on the machine that checks it.
set -u
cd "$(dirname "$0")" || exit 1
RUNS="${RUNS:-5}"
LIMIT_PCT="${LIMIT_PCT:-10}"
BASE="${BENCH_BASE:-baseline.txt}"
X=""
# `emit` is native: pe_ir on Windows, the machine layer on Linux (a plain
# `emit` on Linux would go through C, the same as ccompile)
EMIT_FLAGS=""
case "$(uname -s)" in
  MINGW*|MSYS*|CYGWIN*) X=".exe" ;;
  Linux) case "$(uname -m)" in
           x86_64) EMIT_FLAGS="--target x86_64-linux" ;;
           aarch64|arm64) EMIT_FLAGS="--target aarch64-linux" ;;
         esac ;;
esac
if [ -z "${FID:-}" ]; then
  for c in ../release/stage0/fid$X ../release/fid$X ../release/fid-dev$X; do
    if [ -x "$c" ]; then FID="$c"; break; fi
  done
fi
[ -n "${FID:-}" ] || { echo "no fid compiler found (set FID)"; exit 2; }
FID="$(cd "$(dirname "$FID")" && pwd)/$(basename "$FID")"
if [ -z "${CC:-}" ] && command -v zig >/dev/null 2>&1; then export CC="zig cc"; fi
OUT=out
mkdir -p "$OUT"

# best_ms <exe> -> best wall time in milliseconds
best_ms() {
  local best=""
  for _ in $(seq 1 "$RUNS"); do
    local t0 t1
    t0=$(date +%s%N)
    "$1" > /dev/null 2>&1
    t1=$(date +%s%N)
    local ms=$(( (t1 - t0) / 1000000 ))
    if [ -z "$best" ] || [ "$ms" -lt "$best" ]; then best=$ms; fi
  done
  echo "$best"
}

results=()
for src in fides/*.fd; do
  b=$(basename "$src" .fd)
  for mode in emit ccompile; do
    exe="$OUT/check-$b-$mode$X"
    flags=""; [ "$mode" = emit ] && flags="$EMIT_FLAGS"
    # shellcheck disable=SC2086
    if ! (cd .. && "$FID" $mode "bench/$src" $flags -o "bench/$exe") > "$OUT/check-$b-$mode.log" 2>&1; then
      echo "build failed: $b ($mode), see bench/$OUT/check-$b-$mode.log"; exit 2
    fi
    results+=("$b $mode $(best_ms "$exe")")
  done
done

if [ "${1:-}" = "--save" ]; then
  printf '%s\n' "${results[@]}" > "$BASE"
  echo "baseline saved to bench/$BASE:"; cat "$BASE"; exit 0
fi
[ -f "$BASE" ] || { echo "no bench/$BASE: run ./bench/check.sh --save first"; exit 2; }

slow=0
for r in "${results[@]}"; do
  read -r b mode ms <<< "$r"
  base=$(awk -v b="$b" -v m="$mode" '$1 == b && $2 == m { print $3 }' "$BASE")
  if [ -z "$base" ]; then echo "  new   $b $mode ${ms} ms"; continue; fi
  limit=$(( base + base * LIMIT_PCT / 100 ))
  if [ "$ms" -gt "$limit" ]; then
    echo "  SLOW  $b $mode ${ms} ms (baseline ${base} ms, limit ${limit} ms)"; slow=1
  else
    echo "  ok    $b $mode ${ms} ms (baseline ${base} ms)"
  fi
done
exit $slow
