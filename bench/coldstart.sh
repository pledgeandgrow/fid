#!/bin/bash
# bench/coldstart.sh — cold start of a serverless handler (roadmap Phase 9,
# F32): the time from starting the process to the first byte of its
# response, for a handler built as the entry point (`--entry`) by
#   - fid emit --entry     the native backend (pe_ir on Windows, the machine
#                          layer elsewhere): no C compiler
#   - fid ccompile --entry the same handler through C (CC, default zig cc)
#   - fid emit             the whole program (its main), for the size
#   - C                    the handler written in C (bench/coldstart/
#                          handler.c, zig cc -O2 or cc -O2): the floor
#
#   ./bench/coldstart.sh                         default handler and request
#   RUNS=200 ./bench/coldstart.sh                more runs (default 50)
#   FID=path/to/fid ./bench/coldstart.sh         the compiler to measure
#   SRC=<file.fd> ENTRY=<fn> REQ=<request> ./bench/coldstart.sh
#
# Each row: the executable's size and the min / median / p90 milliseconds
# from process creation to the first byte on stdout
# (bench/coldstart/first_byte.py), after one warm-up run. A response that
# differs from the C one is reported as WRONG. Results go to stdout as a
# markdown table; record them in docs/perf/serverless-coldstart.md.
# Exit 0 when every build answered as the C handler does.
set -u
cd "$(dirname "$0")" || exit 1
RUNS="${RUNS:-50}"
SRC="${SRC:-tests/bootstrap/test_serverless_entry.fd}"
ENTRY="${ENTRY:-handle_request}"
REQ="${REQ:-/hello}"
X=""
EMIT_FLAGS=""
case "$(uname -s)" in
  MINGW*|MSYS*|CYGWIN*) X=".exe" ;;
  Linux) case "$(uname -m)" in
           x86_64) EMIT_FLAGS="--target x86_64-linux" ;;
           aarch64|arm64) EMIT_FLAGS="--target aarch64-linux" ;;
         esac ;;
esac
if [ -z "${FID:-}" ]; then
  for c in ../release/fid$X ../release/fid-dev$X ../release/stage0/fid$X; do
    if [ -x "$c" ]; then FID="$c"; break; fi
  done
fi
[ -n "${FID:-}" ] || { echo "no fid: set FID"; exit 2; }
FID="$(cd "$(dirname "$FID")" && pwd)/$(basename "$FID")"
PY=python3
command -v "$PY" >/dev/null 2>&1 || PY=python
if [ -z "${CC:-}" ] && command -v zig >/dev/null 2>&1; then export CC="zig cc"; fi
OUT=out/coldstart
mkdir -p "$OUT"
# Git Bash would turn a request like /hello into a Windows path
export MSYS_NO_PATHCONV=1

fail=0
built=()
names=()
# fid finds stdlib/ in the directory it runs in: build from the root
# shellcheck disable=SC2086
if (cd .. && "$FID" emit "$SRC" --entry "$ENTRY" $EMIT_FLAGS -o "bench/$OUT/entry-native$X") >"$OUT/entry-native.log" 2>&1; then
  built+=("$OUT/entry-native$X"); names+=("fid emit --entry $ENTRY")
else
  echo "fid emit --entry failed (bench/$OUT/entry-native.log)"; fail=1
fi
if (cd .. && "$FID" ccompile "$SRC" --entry "$ENTRY" -o "bench/$OUT/entry-c$X") >"$OUT/entry-c.log" 2>&1; then
  built+=("$OUT/entry-c$X"); names+=("fid ccompile --entry $ENTRY (${CC:-cc})")
else
  echo "fid ccompile --entry failed (bench/$OUT/entry-c.log; needs a C compiler)"
fi
# shellcheck disable=SC2086
if (cd .. && "$FID" emit "$SRC" $EMIT_FLAGS -o "bench/$OUT/whole-native$X") >"$OUT/whole-native.log" 2>&1; then
  built+=("$OUT/whole-native$X"); names+=("fid emit (whole program, main)")
fi
CREF=""
if [ "$SRC" = "tests/bootstrap/test_serverless_entry.fd" ]; then
  if command -v zig >/dev/null 2>&1; then
    zig cc -O2 coldstart/handler.c -o "$OUT/handler-c$X" >"$OUT/handler-c.log" 2>&1 && CREF="$OUT/handler-c$X"
  elif command -v cc >/dev/null 2>&1; then
    cc -O2 coldstart/handler.c -o "$OUT/handler-c$X" >"$OUT/handler-c.log" 2>&1 && CREF="$OUT/handler-c$X"
  fi
  if [ -n "$CREF" ]; then built+=("$CREF"); names+=("C (handler.c, -O2)"); fi
fi
[ ${#built[@]} -gt 0 ] || { echo "nothing built"; exit 1; }

"$PY" coldstart/first_byte.py "$RUNS" "$REQ" "${built[@]}" >"$OUT/times.txt"
rc=$?
want=""
if [ -n "$CREF" ]; then
  want=$(grep -F "$CREF " "$OUT/times.txt" | cut -d' ' -f5-)
fi
echo "Cold start, $RUNS runs, request '$REQ' ($SRC, entry $ENTRY):"
echo
echo "| build | size (bytes) | min ms | median ms | p90 ms | response |"
echo "|---|---:|---:|---:|---:|---|"
k=0
while [ $k -lt ${#built[@]} ]; do
  exe=${built[$k]}
  line=$(grep -F "$exe " "$OUT/times.txt" | head -1)
  size=$(wc -c <"$exe" | tr -d ' ')
  set -- $line
  if [ $# -ge 4 ]; then
    resp=$(echo "$line" | cut -d' ' -f5-)
    case "$exe" in *whole-native*) ;; *)
      if [ -n "$want" ] && [ "$resp" != "$want" ]; then resp="WRONG: $resp"; fail=1; fi ;;
    esac
    echo "| ${names[$k]} | $size | $2 | $3 | $4 | $resp |"
  else
    echo "| ${names[$k]} | $size | - | - | - | failed: $line |"
    fail=1
  fi
  k=$((k + 1))
done
echo
echo "fid=$FID; host $(uname -s) $(uname -m); $(date -u +%Y-%m-%d)"
[ $rc -eq 0 ] || fail=1
exit $fail
