#!/bin/bash
# bench/service/run.sh — TechEmpower-style service benchmarks (competitive
# roadmap BM5): plaintext, JSON, one row, 20 rows, for the Fides server and
# its rivals, requests per second for each test type.
#
#   bash bench/service/run.sh                    every server, here, in turn
#   SERVERS="fides go" bash bench/service/run.sh just these
#   bash bench/service/run.sh serve fides 8080   build one server and run it
#                                                in the foreground (on the
#                                                server machine)
#   HOST=10.0.0.5 PORT=8080 bash bench/service/run.sh load fides
#                                                drive a server on another
#                                                machine (on the client
#                                                machine); the label names
#                                                the table's column
#
# Servers: fides (bench/service/fides/server.fd: `fid emit`, native; FIDES_MODE=
# ccompile builds it through C), fides-c (through C), go (net/http), node
# (node:http). One not installed is skipped. Each test runs DURATION seconds
# (default 10) after WARMUP (2) with CONNECTIONS keep-alive connections
# (default 64). The load client is wrk when installed (LOADGEN=wrk, the
# default then; THREADS wrk threads, default 4), else bench/service/load.py
# (LOADGEN=py). Before measuring, every test's reply is checked once by
# load.py (wrk does not look at bodies); load.py checks every reply. A
# server whose reply is wrong gets WRONG in its cells.
#
# TechEmpower runs the load client on a separate machine: `serve` and `load`
# split the two. Run on one machine (the default) the client takes CPU from
# the server, which the table's footer says. The "db" and "queries" tests read
# an in-process table of 10,000 rows, not a database server (README.md).
#
# Output: a Markdown table (requests/s per test, server per column) on stdout
# and each run's JSON lines in bench/out/service/results.jsonl.
set -u
cd "$(dirname "$0")" || exit 1
HERE=$(pwd)
ROOT=$(cd ../.. && pwd)
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
  for c in "$ROOT"/release/stage0/fid$X "$ROOT"/release/fid$X "$ROOT"/release/fid-dev$X; do
    if [ -x "$c" ]; then FID="$c"; break; fi
  done
fi
[ -n "${FID:-}" ] && FID="$(cd "$(dirname "$FID")" && pwd)/$(basename "$FID")"
if [ -z "${CC:-}" ] && command -v zig >/dev/null 2>&1; then export CC="zig cc"; fi
[ -d "/c/Program Files/Go/bin" ] && PATH="$PATH:/c/Program Files/Go/bin"
PY=python3; command -v python3 >/dev/null 2>&1 || PY=python
have() { command -v "$1" >/dev/null 2>&1; }
DURATION="${DURATION:-10}"
WARMUP="${WARMUP:-2}"
CONNECTIONS="${CONNECTIONS:-64}"
THREADS="${THREADS:-4}"
TESTS="${TESTS:-plaintext json db queries}"
if [ -z "${LOADGEN:-}" ]; then LOADGEN=py; have wrk && LOADGEN=wrk; fi
OUT="$ROOT/bench/out/service"
mkdir -p "$OUT"
RESULTS="$OUT/results.jsonl"

available() {
  case $1 in
    fides|fides-c) [ -n "${FID:-}" ] ;;
    go) have go ;;
    node) have node ;;
    *) return 1 ;;
  esac
}

# build <server> -> the command line that starts it (with the port appended)
build() {
  local s=$1 log="$OUT/build-$1.log"
  case $s in
    fides|fides-c)
      local cmd=emit flags="$EMIT_FLAGS" exe="$OUT/server-$s$X"
      [ "$s" = fides-c ] && { cmd=ccompile; flags=""; }
      # shellcheck disable=SC2086
      (cd "$ROOT" && "$FID" $cmd bench/service/fides/server.fd $flags -o "$exe") > "$log" 2>&1 \
        && echo "$exe" ;;
    go) (cd go && go build -o "$OUT/server-go$X" server.go) > "$log" 2>&1 && echo "$OUT/server-go$X" ;;
    node) echo "node $HERE/node/server.mjs" ;;
  esac
}

# wait_up <host> <port>: the server answers /plaintext within 30 s
wait_up() {
  local i
  for i in $(seq 1 150); do
    if "$PY" load.py --url "http://$1:$2" --test plaintext --check-only > /dev/null 2>&1; then return 0; fi
    sleep 0.2
  done
  return 1
}

# measure <label> <host> <port> <test> -> requests/s (or WRONG / failed)
measure() {
  local label=$1 host=$2 port=$3 t=$4 line rps
  if ! "$PY" load.py --url "http://$host:$port" --test "$t" --check-only > "$OUT/check-$label-$t.txt" 2>&1; then
    echo "WRONG"; return
  fi
  if [ "$LOADGEN" = wrk ]; then
    local path; case $t in queries) path="/queries?queries=20" ;; *) path="/$t" ;; esac
    wrk -t"$THREADS" -c"$CONNECTIONS" -d"${WARMUP}s" "http://$host:$port$path" > /dev/null 2>&1
    wrk -t"$THREADS" -c"$CONNECTIONS" -d"${DURATION}s" --latency "http://$host:$port$path" > "$OUT/wrk-$label-$t.txt" 2>&1
    rps=$(awk '/Requests\/sec:/ { print $2 }' "$OUT/wrk-$label-$t.txt")
    local bad; bad=$(awk '/Non-2xx|Socket errors/ { print }' "$OUT/wrk-$label-$t.txt")
    printf '{"server": "%s", "test": "%s", "rps": %s, "client": "wrk", "errors": "%s"}\n' \
      "$label" "$t" "${rps:-0}" "$bad" >> "$RESULTS"
    if [ -n "$bad" ]; then echo "errors"; elif [ -n "$rps" ]; then printf '%.0f' "$rps"; else echo "failed"; fi
  else
    line=$("$PY" load.py --url "http://$host:$port" --test "$t" --connections "$CONNECTIONS" \
      --duration "$DURATION" --warmup "$WARMUP" 2> "$OUT/load-$label-$t.err")
    local rc=$?
    echo "$line" | sed "s/^{/{\"server\": \"$label\", /" >> "$RESULTS"
    rps=$(echo "$line" | "$PY" -c 'import json,sys; print(round(json.loads(sys.stdin.read())["rps"]))' 2>/dev/null)
    if [ $rc -ne 0 ]; then echo "errors"; else echo "${rps:-failed}"; fi
  fi
}

load_avg() { [ -r /proc/loadavg ] && cut -d' ' -f1-3 /proc/loadavg || echo "n/a"; }

# stop_server <pid>: on Windows the native server's own process id is in
# /proc/<pid>/winpid (Git Bash's kill may only end its wrapper)
stop_server() {
  local w=""
  [ -r "/proc/$1/winpid" ] && w=$(cat "/proc/$1/winpid")
  kill "$1" 2>/dev/null
  [ -n "$w" ] && taskkill //F //PID "$w" > /dev/null 2>&1
  wait "$1" 2>/dev/null
}

case "${1:-}" in
  serve)
    s="${2:?serve <server> [port]}"; port="${3:-8080}"
    available "$s" || { echo "server $s is not available here"; exit 2; }
    cmd=$(build "$s") || { echo "build of $s failed: $OUT/build-$s.log"; exit 2; }
    [ -n "$cmd" ] || { echo "build of $s failed: $OUT/build-$s.log"; exit 2; }
    echo "serving $s on port $port (Ctrl-C to stop)"
    # shellcheck disable=SC2086
    exec $cmd "$port" ;;
  load)
    label="${2:?load <label>}"; host="${HOST:?HOST=<server machine>}"; port="${PORT:-8080}"
    wait_up "$host" "$port" || { echo "no server answers on $host:$port"; exit 2; }
    echo "| Test | $label |"; echo "|---|---|"
    for t in $TESTS; do echo "| $t | $(measure "$label" "$host" "$port" "$t") |"; done
    echo
    echo "Requests/s, $DURATION s after $WARMUP s warm-up, $CONNECTIONS connections, client $LOADGEN on $(uname -n) (separate machine), server $host; $(date +%F)."
    exit 0 ;;
esac

SERVERS="${SERVERS:-fides fides-c go node}"
PORT0="${PORT:-18080}"
cols=(); declare -A cell
port=$PORT0
la_before=$(load_avg)
for s in $SERVERS; do
  available "$s" || continue
  cols+=("$s")
  cmd=$(build "$s")
  if [ -z "$cmd" ]; then
    for t in $TESTS; do cell[$s,$t]="build failed"; done; continue
  fi
  port=$((port + 1))
  # shellcheck disable=SC2086
  $cmd "$port" > "$OUT/server-$s.log" 2>&1 &
  pid=$!
  if ! wait_up 127.0.0.1 "$port"; then
    for t in $TESTS; do cell[$s,$t]="no answer"; done
    stop_server "$pid"; continue
  fi
  for t in $TESTS; do cell[$s,$t]=$(measure "$s" 127.0.0.1 "$port" "$t"); done
  stop_server "$pid"
done
la_after=$(load_avg)

hdr="| Test |"; sep="|---|"
for s in "${cols[@]}"; do hdr="$hdr $s |"; sep="$sep---|"; done
echo "$hdr"; echo "$sep"
for t in $TESTS; do
  row="| $t |"
  for s in "${cols[@]}"; do row="$row ${cell[$s,$t]} |"; done
  echo "$row"
done
echo
echo "Requests/s, $DURATION s after $WARMUP s warm-up, $CONNECTIONS keep-alive connections, client $LOADGEN on the same machine as the server (not TechEmpower's separate client: see bench/service/README.md); $(date +%F), $(uname -s) $(uname -m), load average $la_before -> $la_after; fid=${FID:-none}."
