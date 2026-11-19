#!/bin/bash
# inline2-bench-check.sh — OP5 (competitive roadmap): the benchmarks of
# bench/ (bench/run.sh's programs) built with inline2 on (the default,
# compiler/iropt_me.fd IME_DEFAULT_ON) and off (FID_IROPT_OFF=inline2):
# the same number printed, and no program slower with it on.
#
# Usage: tests/tooling/inline2-bench-check.sh [bench ...]
#   FID=<compiler>   default release/fid.exe
#   OUT=<dir>        default build/inline2-bench
#   RUNS=<n>         best of n runs (default 5)
#   SUITE=game       bench/game/fides/ instead of bench/fides/
#   SLACK=<pct>      on may be this much slower before it counts (default 10)
#   EXTRA=...        more `emit` arguments (--nat, --target x86_64-linux)
#   ONENV="FID_IROPT_ON=inline2"  environment of the "on" build (a
#                    compiler older than OP5, where inline2 is opt-in)
#
# One line per program: OK / SLOW / WRONG (output differs) / BUILD (does
# not build with inline2 on), the best times off and on in ms, and the
# executables' sizes. Exit status: SLOW + WRONG + BUILD.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/inline2-bench}"
RUNS="${RUNS:-5}"
SLACK="${SLACK:-10}"
EXTRA="${EXTRA:-}"
SUITE="${SUITE:-}"
ONENV="${ONENV:-X_INLINE2=default}"
DIR=bench/fides
[ -n "$SUITE" ] && DIR="bench/$SUITE/fides"
mkdir -p "$OUT"
BENCHES=("$@")
if [ ${#BENCHES[@]} -eq 0 ]; then
  for f in "$DIR"/*.fd; do BENCHES+=("$(basename "$f" .fd)"); done
fi
now_ms() { echo $(( $(date +%s%N) / 1000000 )); }
best() { # best <exe>: the fastest of RUNS runs, in ms; output in <exe>.out
  local exe="$1" b=-1 i t0 t1 d
  for ((i = 0; i < RUNS; i++)); do
    t0=$(now_ms)
    "$exe" > "$exe.out" 2>&1 < /dev/null
    t1=$(now_ms)
    d=$((t1 - t0))
    if [ $b -lt 0 ] || [ $d -lt $b ]; then b=$d; fi
  done
  echo $b
}
bad=0
printf '%-14s %-6s %10s %10s %10s %10s\n' program result off-ms on-ms off-bytes on-bytes
for n in "${BENCHES[@]}"; do
  src="$DIR/$n.fd"
  [ -f "$src" ] || { echo "no $src"; bad=$((bad + 1)); continue; }
  rm -f "$OUT/$n.off.exe" "$OUT/$n.on.exe"
  FID_IROPT_OFF=inline2 "$FID" emit "$src" $EXTRA -o "$OUT/$n.off.exe" > "$OUT/$n.off.log" 2>&1
  env $ONENV FID_VERIFY=1 "$FID" emit "$src" $EXTRA -o "$OUT/$n.on.exe" > "$OUT/$n.on.log" 2>&1
  if [ ! -f "$OUT/$n.off.exe" ]; then
    printf '%-14s %-6s (does not build with inline2 off either)\n' "$n" SKIP
    continue
  fi
  if [ ! -f "$OUT/$n.on.exe" ]; then
    printf '%-14s %-6s %s\n' "$n" BUILD "$(grep -m1 -i -E 'error|panic|verifier' "$OUT/$n.on.log")"
    bad=$((bad + 1))
    continue
  fi
  toff=$(best "$OUT/$n.off.exe")
  ton=$(best "$OUT/$n.on.exe")
  soff=$(wc -c < "$OUT/$n.off.exe")
  son=$(wc -c < "$OUT/$n.on.exe")
  r=OK
  if ! cmp -s <(tr -d '\r' < "$OUT/$n.off.exe.out") <(tr -d '\r' < "$OUT/$n.on.exe.out"); then
    r=WRONG
  elif [ "$ton" -gt $((toff + toff * SLACK / 100 + 5)) ]; then
    # 5 ms more for programs whose run is too short to time
    r=SLOW
  fi
  [ "$r" = OK ] || bad=$((bad + 1))
  printf '%-14s %-6s %10s %10s %10s %10s\n' "$n" "$r" "$toff" "$ton" "$soff" "$son"
done
echo "inline2-bench-check: ${#BENCHES[@]} programs, $bad slower, wrong or not built with inline2 on"
exit $bad
