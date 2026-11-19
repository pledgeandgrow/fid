#!/bin/bash
# bench/check.sh — fail on a slowdown (release D7).
#
#   ./bench/check.sh            compare with bench/baseline.txt; exit 1 when a
#                               Fides program got more than 10% slower
#   ./bench/check.sh --save     record the current times as the baseline
#   LIMIT_PCT=5 ./bench/check.sh   a tighter threshold
#   FID=path/to/fid             the compiler to measure (default: as run.sh)
#   BENCH_BASE=file             baseline file (default bench/baseline.txt)
#   BASE_FID=base/fid ./bench/check.sh --ab
#                               the base compiler and FID measured in turn
#                               on the same machine, re-measured before a
#                               failure (see --ab below); what CI runs, with
#                               LIMIT_PCT=5 (.github/workflows/bench.yml)
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

# --ab (roadmap BM9, the 5% gate): the base compiler (BASE_FID) and this one
# (FID) on the same machine at the same time. Each program is built by both;
# then the two executables run in turn, base, head, base, head, ... RUNS
# times each, so a machine that slows down (another job, thermal limits)
# slows both alike, and the best of each is compared. A program over the
# limit is measured again the same way, and fails only if it is over the
# limit both times: one noisy round does not fail a change. SLACK_MS
# (default 5) is added to the limit, so a 40 ms program is not failed on
# 2 ms of timer noise.
#   BASE_FID=base/fid FID=head/fid LIMIT_PCT=5 ./bench/check.sh --ab
#   SUITES="game" adds bench/game/fides/*.fd (default: bench/fides/ only)
if [ "${1:-}" = "--ab" ]; then
  [ -n "${BASE_FID:-}" ] || { echo "--ab needs BASE_FID (the base commit's compiler)"; exit 2; }
  BASE_FID="$(cd "$(dirname "$BASE_FID")" && pwd)/$(basename "$BASE_FID")"
  SLACK_MS="${SLACK_MS:-5}"
  # ab_round <base exe> <head exe> -> "base_ms head_ms", the best of RUNS
  # runs of each, run in turn
  ab_round() {
    local bb="" hb=""
    for _ in $(seq 1 "$RUNS"); do
      local t0 t1 ms
      t0=$(date +%s%N); "$1" > /dev/null 2>&1; t1=$(date +%s%N)
      ms=$(( (t1 - t0) / 1000000 ))
      if [ -z "$bb" ] || [ "$ms" -lt "$bb" ]; then bb=$ms; fi
      t0=$(date +%s%N); "$2" > /dev/null 2>&1; t1=$(date +%s%N)
      ms=$(( (t1 - t0) / 1000000 ))
      if [ -z "$hb" ] || [ "$ms" -lt "$hb" ]; then hb=$ms; fi
    done
    echo "$bb $hb"
  }
  srcs=(fides/*.fd)
  for s in ${SUITES:-}; do srcs+=("$s"/fides/*.fd); done
  slow=0
  for src in "${srcs[@]}"; do
    b=$(basename "$src" .fd)
    [ "$src" = "fides/$b.fd" ] || b="${src%%/*}-$b"
    for mode in emit ccompile; do
      flags=""; [ "$mode" = emit ] && flags="$EMIT_FLAGS"
      ok=1
      for who in base head; do
        c="$FID"; [ "$who" = base ] && c="$BASE_FID"
        exe="$OUT/ab-$b-$mode-$who$X"
        # AB_REUSE=1: measure the executables a previous --ab built
        # (scripts/bench-noise-check.sh measures one build many times)
        [ "${AB_REUSE:-0}" = 1 ] && [ -x "$exe" ] && continue
        # shellcheck disable=SC2086
        if ! (cd .. && "$c" $mode "bench/$src" $flags -o "bench/$exe") > "$OUT/ab-$b-$mode-$who.log" 2>&1; then
          if [ "$who" = head ]; then
            echo "  FAIL  $b $mode: the head compiler cannot build it (bench/$OUT/ab-$b-$mode-$who.log)"; slow=1
          else
            echo "  new   $b $mode: the base compiler cannot build it"
          fi
          ok=0; break
        fi
      done
      [ "$ok" = 1 ] || continue
      be="$OUT/ab-$b-$mode-base$X"; he="$OUT/ab-$b-$mode-head$X"
      read -r bms hms <<< "$(ab_round "$be" "$he")"
      limit=$(( bms + bms * LIMIT_PCT / 100 + SLACK_MS ))
      if [ "$hms" -gt "$limit" ]; then
        # once more before failing
        read -r bms2 hms2 <<< "$(ab_round "$be" "$he")"
        limit2=$(( bms2 + bms2 * LIMIT_PCT / 100 + SLACK_MS ))
        if [ "$hms2" -gt "$limit2" ]; then
          echo "  SLOW  $b $mode: ${hms} ms and ${hms2} ms, base ${bms} ms and ${bms2} ms (limit +${LIMIT_PCT}%)"; slow=1
          continue
        fi
        echo "  ok    $b $mode ${hms2} ms (base ${bms2} ms; first round ${hms} vs ${bms} ms was noise)"
        continue
      fi
      echo "  ok    $b $mode ${hms} ms (base ${bms} ms)"
    done
  done
  exit $slow
fi

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
