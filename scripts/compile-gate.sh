#!/bin/bash
# compile-gate.sh — the compile-time gate of the own backend
# (docs/roadmap/llvm-roadmap.md SP15): fid's per-stage times on a fixed set
# of programs, compared with a baseline file; a stage slower than the
# baseline by more than LIMIT_PCT percent fails the gate, as does a build
# that fails outright.
#
# Times come from `fid emit --time`, which prints `[time] <stage>: <ms> ms`
# per stage (compiler.fd's time_report). The baseline is
# scripts/compile-gate.baseline, one `name|stage|ms` triple per line.
#
# Usage:
#   scripts/compile-gate.sh [--save]
#   FID=<fid>        the compiler (default release/fid.exe, else release/fid)
#   TARGET=<triple>  the emit target (default x86_64-linux)
#   LIMIT_PCT=<n>    allowed slowdown over the baseline (default 10)
#   RUNS=<n>         best-of runs per program (default 3)
#   MIN_MS=<n>       stages under this many ms are noise and skipped
#                    (default 40; the `total` line is always compared)
#   COMPILE_GATE_BASE=<file>  the baseline file (default
#                    scripts/compile-gate.baseline)
#   --save           replace the baseline with this run's numbers
#
# Times depend on the machine. CI (.github/workflows/bench.yml) builds the
# base commit's fid and this commit's on the same runner: the base one
# records a scratch baseline (COMPILE_GATE_BASE, --save), this one is
# checked against it. The checked-in baseline is for one local machine.
set -u
cd "$(dirname "$0")/.." || exit 1
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
TARGET="${TARGET:-x86_64-linux}"
LIMIT_PCT="${LIMIT_PCT:-10}"
RUNS="${RUNS:-3}"
MIN_MS="${MIN_MS:-40}"
BASE="${COMPILE_GATE_BASE:-scripts/compile-gate.baseline}"
SAVE=0
[ "${1:-}" = "--save" ] && SAVE=1
mkdir -p build/compile-gate

# The measured set: a small program and two medium ones, all in the suite.
PROGS="tests/bootstrap/hello.fd tests/bootstrap/test_hashmap.fd tests/bootstrap/test_tasks.fd"

out=build/compile-gate/run.txt
: > "$out"
for prog in $PROGS; do
  name=$(basename "$prog" .fd)
  best=""
  r=0
  while [ "$r" -lt "$RUNS" ]; do
    log="build/compile-gate/$name.$r.log"
    if ! "$FID" emit "$prog" --target "$TARGET" -o "build/compile-gate/$name" --time > "$log" 2>&1; then
      echo "FAIL $prog: emit failed"
      cat "$log"
      exit 1
    fi
    # Best of RUNS by the `[time] total` line.
    total=$(grep -a '^\[time\] total:' "$log" | sed 's/[^0-9]//g')
    if [ -z "$best" ] || { [ -n "$total" ] && [ "$total" -lt "$best" ]; }; then
      best=${total:-999999999}
      cp "$log" "build/compile-gate/$name.log"
    fi
    r=$((r + 1))
  done
  # Stage lines of the best run, as name|stage|ms triples.
  grep -a '^\[time\]' "build/compile-gate/$name.log" \
    | sed "s/^\[time\] \(.*\): \([0-9]*\) ms/$name|\1|\2/" >> "$out"
done

if [ "$SAVE" = 1 ] || [ ! -f "$BASE" ]; then
  cp "$out" "$BASE"
  echo "compile-gate: baseline saved to $BASE"
  exit 0
fi

# Compare per stage: now vs baseline, fail over LIMIT_PCT.
bad=0
while IFS='|' read -r name stage ms; do
  base=$(awk -F'|' -v n="$name" -v s="$stage" '$1 == n && $2 == s {print $3}' "$BASE")
  if [ -z "$base" ]; then
    echo "note: $name $stage not in the baseline (skipped)"
    continue
  fi
  if [ "$base" -lt "$MIN_MS" ] && [ "$stage" != "total" ]; then continue; fi
  if [ "$base" -eq 0 ]; then base=1; fi
  pct=$(( (ms - base) * 100 / base ))
  if [ "$pct" -gt "$LIMIT_PCT" ]; then
    echo "FAIL $name $stage: ${ms}ms vs ${base}ms baseline (+${pct}%, limit ${LIMIT_PCT}%)"
    bad=$((bad + 1))
  fi
done < "$out"

if [ "$bad" = 0 ]; then echo "compile-gate: passed (limit ${LIMIT_PCT}%)"; else echo "compile-gate: $bad stages over the limit"; fi
exit $bad
