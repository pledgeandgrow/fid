#!/bin/bash
# scripts/service-bench-check.sh — the service benchmarks (competitive
# roadmap BM5, bench/service/) give right answers in every server.
#
#   FID=release/fid-dev bash scripts/service-bench-check.sh
#
#   1. each server's --selftest (no socket) gives the same bodies: the
#      routes, the World table and the row picker agree, so the load
#      client's checks mean the same for every server (Fides built with
#      `fid ccompile`, Go and Node when installed);
#   2. bench/service/run.sh, 2 s per test, every available server: every
#      cell a number (load.py checks each reply's status and body; a wrong
#      reply makes the cell WRONG or errors).
# Exit 0 when both hold, 1 otherwise, 2 when no fid is found.
set -u
cd "$(dirname "$0")/.." || exit 1
X=""
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) X=".exe" ;; esac
if [ -z "${FID:-}" ]; then
  for c in release/stage0/fid$X release/fid$X release/fid-dev$X; do
    if [ -x "$c" ]; then FID="$c"; break; fi
  done
fi
[ -n "${FID:-}" ] || { echo "no fid compiler found (set FID)"; exit 2; }
FID="$(cd "$(dirname "$FID")" && pwd)/$(basename "$FID")"
export FID
[ -d "/c/Program Files/Go/bin" ] && PATH="$PATH:/c/Program Files/Go/bin"
EMIT_FLAGS=""
case "$(uname -s)" in
  Linux) case "$(uname -m)" in
           x86_64) EMIT_FLAGS="--target x86_64-linux" ;;
           aarch64|arm64) EMIT_FLAGS="--target aarch64-linux" ;;
         esac ;;
esac
OUT=bench/out/service
mkdir -p "$OUT"
fails=0

# the bodies every server's selftest gives, in order: /plaintext, /json,
# /db, /queries?queries=3 (the first four ids of the LCG from 42)
EXPECT=$(cat <<'EOF'
Hello, World!
{"message":"Hello, World!"}
{"id":4751,"randomNumber":3183}
[{"id":556,"randomNumber":2978},{"id":8894,"randomNumber":1600},{"id":8261,"randomNumber":8873}]
EOF
)

echo "== 1. selftests agree"
# through C: the Windows native backend (pe_ir) has no p_signal_check /
# p_signal_register, which stdlib/http.fd's accept loop calls (Nov 16), so
# `fid emit` of any http server fails there; run.sh's native column is
# checked in step 2 where it builds
if [ -z "${CC:-}" ] && command -v zig > /dev/null 2>&1; then export CC="zig cc"; fi
if ! "$FID" ccompile bench/service/fides/server.fd -o "$OUT/server-check$X" > "$OUT/server-check.log" 2>&1; then
  echo "FAIL  the Fides server does not build ($OUT/server-check.log)"; tail -5 "$OUT/server-check.log"; exit 1
fi
# the Fides selftest prints whole responses: keep the line after each
# head's blank line (every body here is one line), and the clamp check
fides=$("$OUT/server-check$X" --selftest | tr -d '\r' | awk 'blank { print; blank = 0; next } /^$/ { blank = 1 }')
got=$(echo "$fides" | head -4)
if [ "$got" = "$EXPECT" ]; then echo "ok    fides selftest"; else
  echo "FAIL  fides selftest:"; echo "$got" | sed 's/^/      /'; fails=$((fails + 1)); fi
# /queries with 0 and with "abc" give one row; 900 is clamped to 500
q0=$(echo "$fides" | sed -n 5p); qa=$(echo "$fides" | sed -n 6p)
clamp=$("$OUT/server-check$X" --selftest | tr -d '\r' | grep -xE '[0-9]+' | head -1)
if [[ "$q0" =~ ^\[\{[^]]*\}\]$ ]] && [[ "$qa" =~ ^\[\{[^]]*\}\]$ ]] && [ "$clamp" = 500 ]; then
  echo "ok    fides queries clamp (0 -> 1 row, abc -> 1 row, 900 -> 500)"
else
  echo "FAIL  fides queries clamp: '$q0' '$qa' '$clamp'"; fails=$((fails + 1))
fi
if command -v go > /dev/null 2>&1; then
  got=$( (cd bench/service/go && go run server.go --selftest) | tr -d '\r' | sed 's/^200 //')
  if [ "$got" = "$EXPECT" ]; then echo "ok    go selftest"; else
    echo "FAIL  go selftest:"; echo "$got" | sed 's/^/      /'; fails=$((fails + 1)); fi
fi
if command -v node > /dev/null 2>&1; then
  got=$(node bench/service/node/server.mjs --selftest | tr -d '\r' | sed 's/^200 //')
  if [ "$got" = "$EXPECT" ]; then echo "ok    node selftest"; else
    echo "FAIL  node selftest:"; echo "$got" | sed 's/^/      /'; fails=$((fails + 1)); fi
fi

echo "== 2. a short run of every server, every reply checked"
servers="${SERVERS:-fides fides-c go node}"
case "$(uname -s)" in
  MINGW*|MSYS*|CYGWIN*)
    if [ -z "${SERVERS:-}" ]; then
      servers="fides-c go node"
      echo "      (Windows: native fides left out, pe_ir lacks p_signal_check/p_signal_register)"
    fi ;;
esac
SERVERS="$servers" DURATION=2 WARMUP=1 CONNECTIONS=8 LOADGEN=py bash bench/service/run.sh > "$OUT/check-table.md" 2>&1
cat "$OUT/check-table.md"
bad=$(grep '^| [a-z]' "$OUT/check-table.md" | grep -v '^| Test' | grep -cvE '^\| [a-z]+ \|( [0-9]+ \|)+$')
rows=$(grep -cE '^\| (plaintext|json|db|queries) \|' "$OUT/check-table.md")
if [ "$rows" != 4 ] || [ "$bad" != 0 ]; then
  echo "FAIL  run.sh: $rows test rows, $bad with a cell that is not a number"; fails=$((fails + 1))
else
  echo "ok    run.sh: every server answered every test right"
fi

if [ "$fails" -gt 0 ]; then echo "service bench check: $fails FAILED"; exit 1; fi
echo "service bench check: the servers agree and answer under load"
