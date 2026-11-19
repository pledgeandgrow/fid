#!/bin/bash
# flake-repeat.sh — release-roadmap A12: is a test flaky, and why?
#
# Builds one test program once, then runs the binary RUNS times, optionally
# beside LOAD busy processes (a loaded CI runner), and reports each run's
# exit status, wall time and output hash: a failure that comes with a time
# near the harness limit (LIMIT, the 30 s of `fidesc test`'s
# run_with_timeout) is a timeout under load; one with a different output at
# a normal time is a real nondeterminism (scheduling, uninitialised memory).
#
# A12's case: tests/selfhost/test_spawn_gt.fd, which runs the whole
# self-hosted front end and its interpreter inside the test, takes ~19 s
# alone on the native Windows path (pe-roadmap item 92), so the 30 s limit
# leaves it a margin of only 1.6x.
#
# Usage:
#   scripts/flake-repeat.sh <test.fd> [runs]      (default runs: 20)
#   FID=<fid>      the compiler (default release/fid.exe, release/fid-dev)
#   HOW=emit|c     build natively (fid emit, the default on Windows) or
#                  through C (fid ccompile, the default elsewhere)
#   LOAD=<n>       busy processes beside each run (default 0)
#   LIMIT=<s>      the harness limit to compare with (default 30)
# Exit status: 1 when any run failed or ran over LIMIT.
set -u
cd "$(dirname "$0")/.." || exit 1
SRC="${1:-}"
RUNS="${2:-20}"
if [ -z "$SRC" ] || [ ! -f "$SRC" ]; then
  echo "usage: $0 <test.fd> [runs]"; exit 2
fi
FID="${FID:-}"
if [ -z "$FID" ]; then
  for f in release/fid.exe release/fid release/fid-dev release/fid-dev.exe; do
    if [ -e "$f" ]; then FID="$f"; break; fi
  done
fi
if [ -z "$FID" ]; then echo "flake-repeat: no compiler (FID=...)"; exit 2; fi
EXE=""
HOW_DEFAULT=c
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) EXE=".exe"; HOW_DEFAULT=emit ;; esac
HOW="${HOW:-$HOW_DEFAULT}"
LOAD="${LOAD:-0}"
LIMIT="${LIMIT:-30}"
name=$(basename "$SRC" .fd)
DIR="build/flake-repeat/$name"
mkdir -p "$DIR"
BIN="$DIR/$name$EXE"
rm -f "$BIN"

echo "== build $SRC ($HOW)"
if [ "$HOW" = emit ]; then
  "$FID" emit "$SRC" -o "$BIN" > "$DIR/build.log" 2>&1
else
  "$FID" ccompile "$SRC" -o "$BIN" > "$DIR/build.log" 2>&1
fi
if [ ! -f "$BIN" ]; then echo "flake-repeat: the build failed (see $DIR/build.log)"; exit 1; fi

now_ms() { date +%s%3N 2> /dev/null || echo $(( $(date +%s) * 1000 )); }
sha() { if command -v sha256sum > /dev/null; then sha256sum | cut -c1-12; else shasum -a 256 | cut -c1-12; fi; }

loaders=()
start_load() {
  local k=0
  while [ "$k" -lt "$LOAD" ]; do
    ( while :; do :; done ) &
    loaders+=($!)
    k=$((k + 1))
  done
}
stop_load() {
  for p in "${loaders[@]+"${loaders[@]}"}"; do kill "$p" 2> /dev/null; done
  loaders=()
}
trap stop_load EXIT

echo "== $RUNS runs, $LOAD busy processes beside each, limit ${LIMIT} s"
start_load
fails=0
over=0
ref=""
times=()
r=1
while [ "$r" -le "$RUNS" ]; do
  t0=$(now_ms)
  "./$BIN" > "$DIR/out.$r" 2> "$DIR/err.$r" < /dev/null
  code=$?
  t1=$(now_ms)
  ms=$((t1 - t0))
  times+=("$ms")
  h=$(tr -d '\r' < "$DIR/out.$r" | sha)
  [ -z "$ref" ] && ref="$h"
  note=""
  if [ "$code" != 0 ]; then note="$note exit $code"; fails=$((fails + 1)); fi
  if [ "$h" != "$ref" ]; then note="$note output differs from run 1"; fails=$((fails + 1)); fi
  if [ "$ms" -gt $((LIMIT * 1000)) ]; then note="$note OVER the ${LIMIT} s limit"; over=$((over + 1)); fi
  printf '  run %3d  %7d ms  exit %s  out %s%s\n' "$r" "$ms" "$code" "$h" "$note"
  r=$((r + 1))
done
stop_load

sorted=$(printf '%s\n' "${times[@]}" | sort -n)
min=$(echo "$sorted" | head -1)
max=$(echo "$sorted" | tail -1)
med=$(echo "$sorted" | sed -n "$(( (RUNS + 1) / 2 ))p")
echo "flake-repeat: min $min ms, median $med ms, max $max ms; margin to the limit $(( LIMIT * 1000 * 10 / (max > 0 ? max : 1) ))/10 x"
echo "flake-repeat: $fails failed run(s), $over over the limit (outputs in $DIR)"
[ "$fails" = 0 ] && [ "$over" = 0 ]
