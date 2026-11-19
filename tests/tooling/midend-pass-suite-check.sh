#!/bin/bash
# midend-pass-suite-check.sh — a mid-end pass of compiler/iropt_me.fd (or
# iropt.fd), on and off, must not change what any program of the suite
# does (OP10 `ivsr`, OP11 `unroll`, or any other pass name), and with
# BENCH=1 must not make the benchmarks of bench/ slower.
#
# Usage: tests/tooling/midend-pass-suite-check.sh [test.fd ...]
#   PASS=<names>     the pass (a comma list), required: unroll, ivsr, ...
#   FID=<compiler>   default release/fid.exe
#   OUT=<dir>        default build/pass-suite-<PASS>
#   JOBS=<n>         programs at a time (default 1; each is two builds)
#   EXTRA="--nat"    more `emit` arguments (another backend)
#   BENCH=1          the programs of bench/fides and bench/game/fides
#                    instead of the suite, timed (best of RUNS, default 3;
#                    SLACK=<pct> slower before it counts, default 10)
#   no test files: every tests/bootstrap/*.fd that is not expected to fail
#   (_fail.fd, `//! error`) and needs no outside library (sqlite, SDL, GL)
#
# "off" builds with FID_IROPT_OFF=$PASS, "on" with FID_IROPT_ON=$PASS (a
# pass of IME_DEFAULT_ON is on unless named off, an opt-in one when named
# on), both FID_VERIFY=1 (the IR verifier after iropt). Each executable runs
# with stdin from /dev/null and a 60 s limit. One line per program:
#   SAME    same output and exit status
#   DIFF    output or exit status differ (the first lines of the diff follow)
#   BUILD   builds with the pass off, not with it on
#   VERIFY  the IR verifier complains with the pass on and not off
#   SKIP    builds with neither (or only on)
# BENCH=1 lines: OK / SLOW / WRONG / BUILD with the best times off and on.
# Exit status: DIFF + BUILD + VERIFY (+ SLOW + WRONG with BENCH=1).
set -u
cd "$(dirname "$0")/../.." || exit 1
PASS="${PASS:-}"
if [ -z "$PASS" ]; then
  echo "midend-pass-suite-check: set PASS (e.g. PASS=unroll)"
  exit 2
fi
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/pass-suite-$(echo "$PASS" | tr ',' '-')}"
JOBS="${JOBS:-1}"
EXTRA="${EXTRA:-}"
BENCH="${BENCH:-0}"
RUNS="${RUNS:-3}"
SLACK="${SLACK:-10}"
mkdir -p "$OUT"

if [ "$BENCH" = 1 ]; then
  now_ms() { echo $(( $(date +%s%N) / 1000000 )); }
  best() { # best <exe>: the fastest of RUNS runs, in ms; output in <exe>.out
    local exe="$1" b=-1 i t0 t1 d
    for ((i = 0; i < RUNS; i++)); do
      t0=$(now_ms)
      timeout 300 "$exe" > "$exe.out" 2>&1 < /dev/null
      t1=$(now_ms)
      d=$((t1 - t0))
      if [ $b -lt 0 ] || [ $d -lt $b ]; then b=$d; fi
    done
    echo $b
  }
  bad=0
  count=0
  printf '%-14s %-6s %10s %10s\n' program result off-ms on-ms
  for src in bench/fides/*.fd bench/game/fides/*.fd; do
    [ -f "$src" ] || continue
    n=$(basename "$src" .fd)
    count=$((count + 1))
    rm -f "$OUT/$n.off.exe" "$OUT/$n.on.exe"
    FID_IROPT_OFF="$PASS" "$FID" emit "$src" $EXTRA -o "$OUT/$n.off.exe" > "$OUT/$n.off.log" 2>&1
    FID_IROPT_ON="$PASS" FID_VERIFY=1 "$FID" emit "$src" $EXTRA -o "$OUT/$n.on.exe" > "$OUT/$n.on.log" 2>&1
    if [ ! -f "$OUT/$n.off.exe" ]; then
      printf '%-14s %-6s (does not build with %s off either)\n' "$n" SKIP "$PASS"
      continue
    fi
    if [ ! -f "$OUT/$n.on.exe" ]; then
      printf '%-14s %-6s %s\n' "$n" BUILD "$(grep -m1 -i -E 'error|panic|verifier' "$OUT/$n.on.log")"
      bad=$((bad + 1))
      continue
    fi
    toff=$(best "$OUT/$n.off.exe")
    ton=$(best "$OUT/$n.on.exe")
    r=OK
    if ! cmp -s <(tr -d '\r' < "$OUT/$n.off.exe.out") <(tr -d '\r' < "$OUT/$n.on.exe.out"); then
      r=WRONG
    elif [ "$ton" -gt $((toff + toff * SLACK / 100 + 5)) ]; then
      r=SLOW
    fi
    [ "$r" = OK ] || bad=$((bad + 1))
    printf '%-14s %-6s %10s %10s\n' "$n" "$r" "$toff" "$ton"
  done
  echo "midend-pass-suite-check: BENCH $PASS: $count programs, $bad slower, wrong or not built with it on"
  exit $bad
fi

TESTS=("$@")
if [ ${#TESTS[@]} -eq 0 ]; then
  for t in tests/bootstrap/*.fd; do
    case "$t" in *_fail.fd) continue ;; esac
    grep -q '^//! error' "$t" && continue
    grep -q -E 'sqlite3|SDL_|sdl2_h|glClear|use gl;|use sdl' "$t" && continue
    TESTS+=("$t")
  done
fi

# one <test>: writes $OUT/<name>.res (the line)
one() {
  local t="$1" n
  n=$(basename "$t" .fd)
  local rcs=() m
  for m in off on; do
    local exe="$OUT/$n.$m.exe"
    rm -f "$exe" "$exe.out"
    if [ $m = off ]; then
      FID_IROPT_OFF="$PASS" FID_VERIFY=1 "$FID" emit "$t" $EXTRA -o "$exe" > "$exe.log" 2>&1 < /dev/null
    else
      FID_IROPT_ON="$PASS" FID_VERIFY=1 "$FID" emit "$t" $EXTRA -o "$exe" > "$exe.log" 2>&1 < /dev/null
    fi
    if [ -f "$exe" ]; then
      timeout 60 "$exe" > "$exe.out" 2>&1 < /dev/null
      rcs+=("$?")
    else
      rcs+=("BUILD")
    fi
  done
  local a="${rcs[0]}" b="${rcs[1]}" line voff von
  voff=$(grep -c "IR verifier" "$OUT/$n.off.exe.log" 2>/dev/null)
  von=$(grep -c "IR verifier" "$OUT/$n.on.exe.log" 2>/dev/null)
  if [ "$a" = "BUILD" ] && [ "$b" = "BUILD" ]; then
    line=$(printf 'SKIP   %s (builds with neither)' "$n")
  elif [ "$a" = "BUILD" ]; then
    line=$(printf 'SKIP   %s (builds only with %s on)' "$n" "$PASS")
  elif [ "$b" = "BUILD" ]; then
    line=$(printf 'BUILD  %s (on: %s)' "$n" "$(grep -m1 -i -E 'error|panic|invalid|verifier' "$OUT/$n.on.exe.log")")
  elif [ "${von:-0}" -gt "${voff:-0}" ]; then
    line=$(printf 'VERIFY %s (%s)' "$n" "$(grep -m1 "IR verifier" "$OUT/$n.on.exe.log")")
  elif [ "$a" = "$b" ] && cmp -s <(tr -d '\r' < "$OUT/$n.off.exe.out") <(tr -d '\r' < "$OUT/$n.on.exe.out"); then
    line=$(printf 'SAME   %s (rc %s)' "$n" "$a")
  else
    line=$(printf 'DIFF   %s (rc off %s, on %s)\n%s' "$n" "$a" "$b" "$(diff "$OUT/$n.off.exe.out" "$OUT/$n.on.exe.out" | head -6 | sed 's/^/       /')")
  fi
  echo "$line" > "$OUT/$n.res"
}
export -f one
export FID OUT EXTRA PASS

printf '%s\n' "${TESTS[@]}" | xargs -P "$JOBS" -I{} bash -c 'one "$1"' _ {}

: > "$OUT/results.txt"
for t in "${TESTS[@]}"; do
  n=$(basename "$t" .fd)
  [ -f "$OUT/$n.res" ] && cat "$OUT/$n.res" >> "$OUT/results.txt"
done
cat "$OUT/results.txt"
bad=$(grep -c -E '^(DIFF|BUILD|VERIFY)' "$OUT/results.txt")
same=$(grep -c '^SAME' "$OUT/results.txt")
echo "midend-pass-suite-check: $PASS: ${#TESTS[@]} programs, $same the same, $bad differ, fail to build or to verify with it on"
exit "$bad"
