#!/bin/bash
# tests/wasm/size-check.sh — XT39: the size of a hello world built with
# `fid wasm` (compiler/wasm.fd). Builds tests/wasm/size_hello.fd, checks
# that it runs (its output), prints its sections and largest functions
# (tests/wasm/size.mjs), and fails when the module is over LIMIT bytes
# (default 20480: 20 KB). The "name" section (function names, for traces
# and tools) counts; NONAMES=1 builds without it (FID_NAT_OFF=debug).
#
#   FID=release/fid.exe bash tests/wasm/size-check.sh
#   LIMIT=<bytes>  OUT=<dir> (default build/wasm-size)  TOP=<n> (default 15)
#
# Measured Nov 27 with a fid built before XT39's compiler changes (the
# runtime of the Nov 27 tree, XT31/XT32 included): 178,274 bytes, 215
# functions, 167,331 of code (174,837 with NONAMES=1); before XT31/XT32's
# runtime changes tests/bootstrap/hello.fd was 171,658.
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT=$(pwd)
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) DEF=release/fid.exe ;; *) DEF=release/fid ;; esac
FID="${FID:-$DEF}"
case "$FID" in /*|[A-Za-z]:*) ;; *) FID="$ROOT/$FID" ;; esac
LIMIT="${LIMIT:-20480}"
OUT="${OUT:-build/wasm-size}"
TOP="${TOP:-15}"
mkdir -p "$OUT"
src=tests/wasm/size_hello.fd
wasm="$OUT/size_hello.wasm"
if [ "${NONAMES:-}" = 1 ]; then
  FID_NAT_OFF="debug${FID_NAT_OFF:+,$FID_NAT_OFF}" timeout 600 "$FID" wasm "$src" -o "$wasm" > "$OUT/build.log" 2>&1
else
  timeout 600 "$FID" wasm "$src" -o "$wasm" > "$OUT/build.log" 2>&1
fi
if [ $? != 0 ]; then
  echo "BUILD size_hello: $(grep -a -m1 -i 'error\|not available' "$OUT/build.log" | cut -c1-160)"
  exit 1
fi
out=$(node scripts/wasm-run.mjs "$wasm" < /dev/null 2>&1 | tr -d '\r')
if [ "$out" != "Hello, world!" ]; then
  echo "FAIL  size_hello runs: got '$out'"
  exit 1
fi
node tests/wasm/size.mjs "$wasm" "$TOP"
size=$(wc -c < "$wasm" | tr -d ' ')
if [ "$size" -gt "$LIMIT" ]; then
  echo "FAIL  hello world is $size bytes, over $LIMIT"
  exit 1
fi
echo "PASS  hello world is $size bytes (at most $LIMIT)"
