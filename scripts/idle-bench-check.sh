#!/bin/bash
# scripts/idle-bench-check.sh — the idle-connections benchmark works
# (competitive roadmap RT11): bench/idle/server.fd builds through C, answers
# its selftest, and bench/idle/idle.py holds 200 then 1000 idle connections
# against it, reads the server's own count back (it must equal what the
# client holds) and gets every ping answered. Small enough for any machine;
# bench/idle/run.sh is the measurement.
#
#   FID=release/fid.exe bash scripts/idle-bench-check.sh
set -u
cd "$(dirname "$0")/.." || exit 2
FID="${FID:-release/fid}"
[ -x "$FID" ] || FID="release/fid.exe"
export CC="${CC:-zig cc}"
T=$(mktemp -d)
trap 'rm -rf "$T"' EXIT
EXE=""; case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) EXE=".exe" ;; esac
PY=python3; command -v python3 > /dev/null 2>&1 || PY=python
fail() { echo "FAIL $1"; exit 1; }

if [ "${CBUILD:-ccompile}" = dumpc ]; then
    # Linux from a Windows fid (WSL): the C from fid, compiled here
    { "$FID" dump-c bench/idle/server.fd > "$T/server.c" \
        && ${HOSTCC:-cc} -O2 -w -Wno-int-conversion -Wno-incompatible-pointer-types \
            -o "$T/server$EXE" "$T/server.c" -lm -lpthread; } > "$T/build.log" 2>&1 \
        || { tail -5 "$T/build.log"; fail "bench/idle/server.fd does not build through C"; }
else
    "$FID" ccompile bench/idle/server.fd -o "$T/server$EXE" > "$T/build.log" 2>&1 \
        || { tail -5 "$T/build.log"; fail "bench/idle/server.fd does not build through C"; }
fi
st=$("$T/server$EXE" --selftest 2>&1 | tr -d '\r' | paste -sd, -)
[ "$st" = "pong 0,pong 3" ] || fail "selftest printed '$st'"
echo "ok   server builds, selftest"
"$PY" bench/idle/idle.py --server "$T/server$EXE" --steps 200,1000 --samples 50 \
    --ports 4 --addrs 1 --port 9370 --out "$T/t.md" > /dev/null 2>&1 \
    || { cat "$T/t.md" 2>/dev/null; fail "idle.py reported unanswered pings"; }
for n in 200 1000; do
    row=$(grep "^| $n | " "$T/t.md") || fail "no row for $n connections"
    cnt=$(echo "$row" | awk -F'|' '{ gsub(/ /, "", $3); print $3 }')
    # the server counts the sampling connection being pinged too: n or n+1
    [ "$cnt" -ge "$n" ] && [ "$cnt" -le $((n + 1)) ] || fail "server counted $cnt connections, client held $n"
done
echo "ok   200 and 1000 idle connections held, counted by the server, every ping answered"

# RT11: an idle ping costs the same with 1000 connections held or 100,000.
# A task parked on the poller is out of the scheduler's list (C runtime
# __p_co_round / __p_co_unpark), so a pass does not scan every connection:
# before, the ping's median grew from 0.11 ms at 1k to 1.9 ms at 100k. Linux
# only by default (Windows' dynamic ports stop near 16k; SCALE=1 forces it,
# SCALE=0 skips it); the step is as large as the descriptor limit allows,
# 100k at most, and at least 20k.
scale="${SCALE:-}"
[ -z "$scale" ] && { [ "$(uname -s)" = Linux ] && scale=1 || scale=0; }
if [ "$scale" = 1 ]; then
    lim=$(ulimit -n)
    [ "$lim" = unlimited ] && lim=1048576
    big=$((lim - 2000))
    [ "$big" -gt 100000 ] && big=100000
    if [ "$big" -lt 20000 ]; then
        echo "skip idle latency against connections held: descriptor limit $lim (needs 22000)"
    else
        "$PY" bench/idle/idle.py --server "$T/server$EXE" --steps "1000,$big" --samples 200 \
            --port 9380 --out "$T/s.md" > /dev/null 2>&1 \
            || { cat "$T/s.md" 2>/dev/null; fail "idle.py did not complete 1000 and $big connections"; }
        p1=$(grep "^| 1000 | " "$T/s.md" | awk -F'|' '{ gsub(/ /, "", $9); print $9 }')
        pb=$(grep "^| $big | " "$T/s.md" | awk -F'|' '{ gsub(/ /, "", $9); print $9 }')
        [ -n "$p1" ] && [ -n "$pb" ] || { cat "$T/s.md"; fail "no idle p50 for 1000 or $big connections"; }
        # within 3x the median at 1000 plus 0.2 ms (the client is Python)
        awk -v a="$p1" -v b="$pb" 'BEGIN { exit !(b <= 3 * a + 0.2) }' \
            || { cat "$T/s.md"; fail "idle ping median $pb ms at $big connections, $p1 ms at 1000: it grows with the connections held"; }
        echo "ok   idle ping median $p1 ms at 1000 connections, $pb ms at $big"
    fi
fi
echo "idle-bench-check: all passed"
