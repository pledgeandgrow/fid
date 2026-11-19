#!/bin/bash
# site-memory-check.sh -- AD2 (competitive-roadmap): the website program
# (tools/site.fd) serves through http_serve_scoped, so its memory stays flat
# over repeated requests (known-bugs 2g.35 fixed; http_serve kept about
# 130 KB per request of the largest page).
#
# Builds tools/site.fd with `fid ccompile`, serves the repository on a local
# port, asks for the largest pages ROUNDS x N times and reads the server's
# private memory after each round. Passes when every answer was 200 and the
# memory after the last round is within LIMIT_KB of the memory after the
# first (the first round warms the allocator's pools).
#
# Usage: tests/tooling/site-memory-check.sh [fid]
#   default fid: release/fid.exe (else release/fid-dev.exe, release/fid, ...)
#   CC: the C compiler ccompile uses (default "zig cc" when zig is on PATH)
#   PORT (default 18734), N (requests per round, default 150), ROUNDS (4),
#   LIMIT_KB (default 2048: with http_serve, 450 more requests of
#   docs/reference/reference.html grow it by tens of MB)
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT="$(pwd)"
FID="${1:-}"
if [ -z "$FID" ]; then
  for f in release/fid.exe release/fid-dev.exe release/fid release/fid-dev; do
    if [ -x "$f" ]; then FID="$ROOT/$f"; break; fi
  done
fi
if [ -z "$FID" ]; then echo "site-memory-check: no fid (give its path)"; exit 1; fi
case "$FID" in /*|[A-Za-z]:*) ;; *) FID="$ROOT/$FID" ;; esac
if [ -z "${CC:-}" ] && command -v zig > /dev/null 2>&1; then export CC="zig cc"; fi
if ! command -v curl > /dev/null 2>&1; then echo "site-memory-check: SKIP (no curl)"; exit 0; fi
PORT="${PORT:-18734}"
N="${N:-150}"
ROUNDS="${ROUNDS:-4}"
LIMIT_KB="${LIMIT_KB:-2048}"

WORK="$(mktemp -d 2>/dev/null || echo "${TMPDIR:-/tmp}/site-memory.$$")"
mkdir -p "$WORK"
SITE="$WORK/site"
WIN=0
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) SITE="$WORK/site.exe"; WIN=1 ;; esac
SPID=""
cleanup() {
  if [ -n "$SPID" ]; then kill "$SPID" 2>/dev/null; wait "$SPID" 2>/dev/null; fi
  rm -rf "$WORK"
}
trap cleanup EXIT

echo "== site-memory-check ($FID)"
if ! grep -q 'http_serve_scoped(port, site_handler)' tools/site.fd; then
  echo "  FAIL  tools/site.fd does not serve through http_serve_scoped"
  exit 1
fi
if ! "$FID" ccompile tools/site.fd -o "$SITE" > "$WORK/build.log" 2>&1 || [ ! -f "$SITE" ]; then
  echo "  FAIL  fid ccompile tools/site.fd"
  grep -v '^\[' "$WORK/build.log" | tail -20 | sed 's/^/        /'
  exit 1
fi
echo "  ok    fid ccompile tools/site.fd"

"$SITE" --port "$PORT" --root "$ROOT" > "$WORK/server.log" 2>&1 &
SPID=$!

# The server's private memory in KB (Windows: the process behind the MSYS
# pid; Linux/macOS: resident set size).
mem_kb() {
  if [ $WIN -eq 1 ]; then
    local wpid
    wpid=$(cat "/proc/$SPID/winpid" 2>/dev/null)
    [ -z "$wpid" ] && wpid=$SPID
    powershell -NoProfile -Command "[int]((Get-Process -Id $wpid).PrivateMemorySize64 / 1KB)" 2>/dev/null | tr -d '\r'
  elif [ -r "/proc/$SPID/status" ]; then
    awk '/^VmRSS:/ {print $2}' "/proc/$SPID/status"
  else
    ps -o rss= -p "$SPID" | tr -d ' '
  fi
}

up=0
for _ in $(seq 1 100); do
  if curl -s -o /dev/null "http://127.0.0.1:$PORT/style.css"; then up=1; break; fi
  if ! kill -0 "$SPID" 2>/dev/null; then break; fi
  sleep 0.1
done
if [ $up -ne 1 ]; then
  echo "  FAIL  the server did not answer on port $PORT"
  tail -20 "$WORK/server.log" | sed 's/^/        /'
  exit 1
fi
echo "  ok    serving on port $PORT"

PAGES="/docs/reference/reference.html / /docs/book/03-tour.html /pages.html /docs/reference/"
first=""
last=""
bad=0
r=1
while [ $r -le "$ROUNDS" ]; do
  i=0
  while [ $i -lt "$N" ]; do
    for p in $PAGES; do
      code=$(curl -s -o /dev/null -w '%{http_code}' "http://127.0.0.1:$PORT$p")
      if [ "$code" != 200 ]; then bad=$((bad + 1)); fi
      i=$((i + 1))
      [ $i -ge "$N" ] && break
    done
  done
  if ! kill -0 "$SPID" 2>/dev/null; then
    echo "  FAIL  the server died in round $r"
    tail -20 "$WORK/server.log" | sed 's/^/        /'
    exit 1
  fi
  kb=$(mem_kb)
  echo "  ..    after $((r * N)) requests: ${kb} KB"
  [ -z "$first" ] && first=$kb
  last=$kb
  r=$((r + 1))
done
if [ $bad -ne 0 ]; then
  echo "  FAIL  $bad answers were not 200"
  exit 1
fi
if [ -z "$first" ] || [ -z "$last" ]; then
  echo "  FAIL  could not read the server's memory"
  exit 1
fi
grow=$((last - first))
if [ $grow -gt "$LIMIT_KB" ]; then
  echo "  FAIL  memory grew ${grow} KB over $(((ROUNDS - 1) * N)) requests (limit ${LIMIT_KB} KB)"
  exit 1
fi
echo "  ok    memory flat: ${first} KB -> ${last} KB (+${grow} KB, limit ${LIMIT_KB})"
echo "site-memory-check: PASS"
