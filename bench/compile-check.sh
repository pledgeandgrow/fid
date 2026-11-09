#!/bin/bash
# bench/compile-check.sh — compile-time regression gate (SP15).
#
#   ./bench/compile-check.sh            compare per-stage compile times with
#                                       bench/compile-baseline.txt; exit 1 when
#                                       a stage got more than LIMIT_PCT slower
#   ./bench/compile-check.sh --save     record the current times as baseline
#   LIMIT_PCT=5 ./bench/compile-check.sh   tighter threshold
#   FID=path/to/fid                     compiler to measure
#   FILES="a.fd b.fd"                   files to measure
#                                       (default: a tiny and a mid-size test)
#
# Uses `fid emit --time`, which prints one `[time] <stage>: <ms> ms` line per
# pipeline stage. Times depend on the machine: record the baseline on the
# machine that checks it. Best of RUNS (default 3) runs per stage.
set -u
cd "$(dirname "$0")" || exit 1
RUNS="${RUNS:-3}"
LIMIT_PCT="${LIMIT_PCT:-10}"
BASE=compile-baseline.txt
X=""
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) X=".exe" ;; esac
if [ -z "${FID:-}" ]; then
  for c in ../release/stage0/fid$X ../release/fid$X ../release/fid-dev$X; do
    if [ -x "$c" ]; then FID="$c"; break; fi
  done
fi
[ -n "${FID:-}" ] || { echo "no fid compiler found (set FID)"; exit 2; }
FID="$(cd "$(dirname "$FID")" && pwd)/$(basename "$FID")"
FILES="${FILES:-../tests/bootstrap/hello.fd ../tests/bootstrap/test_compiler_sized.fd}"
OUT=out
mkdir -p "$OUT"

# one run: emit --time, print "file.stage ms" lines (stderr -> stdout)
run_once() {
  local src="$1" exe="$OUT/cc-$(basename "$src" .fd)$X"
  rm -f "$exe" "$exe.fides-stamp"
  (cd .. && "$FID" emit "${src#../}" -o "bench/$exe" --time) 2>&1 \
    | sed -n "s/^\[time\] \(.*\): \([0-9]*\) ms/\1 \2/p"
}

# accumulate "stage file" -> min ms across RUNS
declare -A best=()
declare -a stages=()
for src in $FILES; do
  b=$(basename "$src")
  for _ in $(seq 1 "$RUNS"); do
    while read -r stage ms; do
      key="$b|$stage"
      cur=${best[$key]:-}
      if [ -z "$cur" ] || [ "$ms" -lt "$cur" ]; then best[$key]=$ms; fi
    done < <(run_once "$src")
  done
  for key in "${!best[@]}"; do
    [ "${key%%|*}" = "$b" ] && stages+=("$key")
  done
done

lines=()
for key in $(printf '%s\n' "${stages[@]}" | sort); do
  lines+=("$key ${best[$key]}")
done

if [ "${1:-}" = "--save" ]; then
  printf '%s\n' "${lines[@]}" > "$BASE"
  echo "baseline saved to bench/$BASE:"; cat "$BASE"; exit 0
fi

[ -f "$BASE" ] || { echo "no baseline: run with --save first"; exit 2; }

fail=0
for line in "${lines[@]}"; do
  key="${line% *}"; now="${line##* }"
  base_ms=$(awk -v k="$key" 'index($0, k" ")==1 {print $NF}' "$BASE")
  if [ -z "$base_ms" ]; then
    echo "  new stage (no baseline): $key = ${now}ms"
    continue
  fi
  if [ "$base_ms" -gt 0 ]; then pct=$(( (now - base_ms) * 100 / base_ms )); else pct=0; fi
  if [ "$pct" -gt "$LIMIT_PCT" ]; then
    echo "SLOW: $key ${base_ms}ms -> ${now}ms (+${pct}%)"
    fail=1
  else
    printf "  ok: %s %sms -> %sms (%+d%%)\n" "$key" "$base_ms" "$now" "$pct"
  fi
done
exit "$fail"
