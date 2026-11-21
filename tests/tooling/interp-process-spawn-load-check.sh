#!/usr/bin/env bash
# interp-process-spawn-load-check.sh — under `fid run`, the exit codes of
# children started at once are read whole, also on a busy machine
# (known-bugs 2g.113).
#
# The interpreter's process_wait (compiler/interp.fd ipx_wait) reads the
# child's code from a file. On Windows cmd's `echo %errorlevel%> file`
# creates it before it writes the code; one more look after 1 ms was too
# soon under load and the code read as -1 (test_process_spawn printed
# `parallel codes: 0 1 -1`, 1 of 4 runs). Now it looks until the file holds
# the code. This runs test_process_spawn ROUNDS times (default 3), PAR
# copies at once (default 4), and requires `parallel codes: 0 1 2` and
# `done` from every run.
#
# Usage: tests/tooling/interp-process-spawn-load-check.sh   (FID=<fid>)
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT=$(pwd)
FID="${FID:-$ROOT/release/fid.exe}"
[ -x "$FID" ] || FID="$ROOT/release/fid"
export FID_HOME="${FID_HOME:-$ROOT}"
ROUNDS="${ROUNDS:-3}"
PAR="${PAR:-4}"
T=$(mktemp -d)
trap 'rm -rf "$T"' EXIT
fail=0
runs=0
r=0
while [ "$r" -lt "$ROUNDS" ]; do
  pids=()
  j=0
  while [ "$j" -lt "$PAR" ]; do
    "$FID" run tests/bootstrap/test_process_spawn.fd > "$T/out_${r}_$j.txt" 2>&1 &
    pids+=($!)
    j=$((j + 1))
  done
  for p in "${pids[@]}"; do wait "$p"; done
  j=0
  while [ "$j" -lt "$PAR" ]; do
    f="$T/out_${r}_$j.txt"
    runs=$((runs + 1))
    if ! tr -d '\r' < "$f" | grep -qx "parallel codes: 0 1 2" || ! tr -d '\r' < "$f" | grep -qx "done"; then
      echo "FAIL round $r copy $j:"; tr -d '\r' < "$f" | grep -v '^\[' | tail -6
      fail=1
    fi
    j=$((j + 1))
  done
  r=$((r + 1))
done
[ "$fail" = 0 ] && echo "PASS interp-process-spawn-load-check ($runs runs)" || { echo "FAILED interp-process-spawn-load-check"; exit 1; }
