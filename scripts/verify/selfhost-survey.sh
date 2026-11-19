#!/bin/bash
# selfhost-survey.sh — release A11: every test of tests/selfhost/ that
# skip.txt does not leave out, on each backend, with its time and the first
# error, so that a failure of ./verify-selfhost.sh can be told apart from a
# slow test (the interpreter-in-interpreter ones) before its baseline is
# saved with --save.
#
#   FID=release/fid.exe bash scripts/verify/selfhost-survey.sh
#   MODES="run" LIMIT=900 bash scripts/verify/selfhost-survey.sh
#   ONLY=test_lower bash scripts/verify/selfhost-survey.sh
#
# MODES: any of run (fid run), c (fid ccompile + the exe), emit (fid emit +
# the exe); default all three, the backends verify-selfhost.sh runs.
# LIMIT: seconds per step (default 300, verify-selfhost.sh's). ROOT: the
# tree to run in (default the repository; a snapshot of compiler/, stdlib/
# and tests/selfhost/ when other sessions are editing them). OUT: where the
# outputs go (default build/selfhost-survey). CC defaults to zig cc.
# Writes $OUT/summary.txt ("mode test PASS|FAIL|TIMEOUT secs first-error")
# and prints it with the counts per mode. Exit 0 when every test passed.
set -u
REPO="$(cd "$(dirname "$0")/../.." && pwd)"
ROOT="${ROOT:-$REPO}"
FID="${FID:-$REPO/release/fid.exe}"
case "$FID" in /*) ;; *) FID="$REPO/$FID" ;; esac
LIMIT="${LIMIT:-300}"
MODES="${MODES:-run c emit}"
OUT="${OUT:-$REPO/build/selfhost-survey}"
export CC="${CC:-zig cc}"
mkdir -p "$OUT"
cd "$ROOT" || exit 2
if [ -z "${FID_HOME:-}" ]; then
  if command -v cygpath > /dev/null 2>&1; then export FID_HOME="$(cygpath -w "$ROOT")"; else export FID_HOME="$ROOT"; fi
fi
SKIP="$(grep -v '^#' tests/selfhost/skip.txt 2> /dev/null | tr -d '\r' | awk 'NF {print $1}')"
: > "$OUT/summary.txt"
fails=0
first_error() {
  grep -v '^\[build\]\|^\[prof\]' "$1" | grep -m1 -iE 'error|panic|FAIL|assert' | tr -d '\r' | cut -c1-140
}
for mode in $MODES; do
  for f in tests/selfhost/*.fd; do
    n=$(basename "$f" .fd)
    if [ -n "$SKIP" ] && echo "$SKIP" | grep -qxF "$n.fd"; then continue; fi
    if [ -n "${ONLY:-}" ] && ! echo "$n" | grep -q -- "$ONLY"; then continue; fi
    log="$OUT/$n.$mode.out"
    t0=$(date +%s)
    case $mode in
      run)  timeout "$LIMIT" "$FID" run "$f" > "$log" 2>&1 < /dev/null; rc=$? ;;
      c)    timeout "$LIMIT" "$FID" ccompile "$f" -o "$OUT/$n-c.exe" > "$log" 2>&1 \
              && timeout "$LIMIT" "$OUT/$n-c.exe" > "$log" 2>&1 < /dev/null; rc=$? ;;
      emit) timeout "$LIMIT" "$FID" emit "$f" -o "$OUT/$n-native.exe" > "$log" 2>&1 \
              && timeout "$LIMIT" "$OUT/$n-native.exe" > "$log" 2>&1 < /dev/null; rc=$? ;;
      *) echo "selfhost-survey: unknown mode $mode"; exit 2 ;;
    esac
    secs=$(( $(date +%s) - t0 ))
    if [ $rc = 0 ]; then st=PASS; why=""
    elif [ $rc = 124 ]; then st=TIMEOUT; why="over ${LIMIT} s"; fails=$((fails + 1))
    else st=FAIL; why=$(first_error "$log"); fails=$((fails + 1)); fi
    echo "$mode $n $st ${secs}s $why" | tee -a "$OUT/summary.txt"
  done
done
echo
for mode in $MODES; do
  p=$(grep -c "^$mode [^ ]* PASS" "$OUT/summary.txt")
  t=$(grep -c "^$mode " "$OUT/summary.txt")
  echo "selfhost-survey $mode: $p/$t passed"
done
[ $fails = 0 ]
