#!/bin/bash
# vecalias-check.sh — OP26 / LF3 (competitive roadmap, llvm roadmap): a
# Vec<i64>'s buffer in the alias query (compiler/iropt_me.fd: ime_addr
# kind 5, ime_vb_owners, ime_vb_call_writes, ime_same_elem; pass name
# `vecalias`, on by default). It acts on the element loads of the opt-in
# `vecdata` (OP9), after which ime_function_passes runs forwarding again.
#
# Usage: tests/tooling/vecalias-check.sh   (FID=<compiler>, default
#        release/fid.exe; OUT=<dir>, default build/vecalias;
#        TARGET=<emit flags>, default none (pe_ir); TARGET=--nat for the
#        machine layer)
#
# Checks, one line each (PASS/FAIL), exit status the number of FAILs:
#  1. test_midend_vec_alias.fd emitted three ways prints its expected
#     lines (FID_VERIFY=1): default passes; vecdata on; vecdata on with
#     vecalias off.
#  2. FID_IROPT_STATS's "iropt: vecalias N element reads reused": N > 0
#     with vecdata on, N = 0 with vecalias off (the reads it reuses each
#     follow a w.set(..), which without the Vec facts writes everything).
#  3. The instructions after iropt (FID_IROPT_STATS "iropt: instructions
#     A -> B") with vecalias on are fewer than with it off: the loads went.
# inline2 and unroll are named off in 1-3 (inline2 would copy the
# functions into main, where their headers are stored into result slots,
# and the counts would compare other code).
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/vecalias}"
TARGET="${TARGET:-}"
mkdir -p "$OUT"
T=tests/bootstrap/test_midend_vec_alias.fd
bad=0
awk '/^\/\/ Expected output:/ {on=1; next} on && /^\/\/ / {sub(/^\/\/ /, ""); print; next} on {exit}' "$T" > "$OUT/want"
same() { cmp -s <(tr -d '\r' < "$1") "$OUT/want"; }
pass() { echo "PASS $1"; }
fail() { echo "FAIL $1"; bad=$((bad + 1)); }

one() { # one <label> <env...>
  local label="$1"; shift
  rm -f "$OUT/$label.exe"
  env "$@" FID_VERIFY=1 FID_IROPT_STATS=1 "$FID" emit "$T" $TARGET -o "$OUT/$label.exe" > "$OUT/$label.log" 2>&1
  if [ -f "$OUT/$label.exe" ] && timeout 60 "$OUT/$label.exe" > "$OUT/$label.out" 2>&1 < /dev/null && same "$OUT/$label.out"; then
    pass "emit $label"
  else
    fail "emit $label"
  fi
  grep -m1 "^iropt: instructions" "$OUT/$label.log" | sed 's/.* -> \([0-9]*\).*/\1/' > "$OUT/$label.count"
  grep -m1 "^iropt: vecalias" "$OUT/$label.log" | sed -n 's/^iropt: vecalias \([0-9]*\) .*/\1/p' > "$OUT/$label.reused"
}
one default X=1
one on FID_IROPT_ON=vecdata FID_IROPT_OFF=inline2,unroll
one off FID_IROPT_ON=vecdata FID_IROPT_OFF=inline2,unroll,vecalias

ron=$(cat "$OUT/on.reused" 2>/dev/null)
roff=$(cat "$OUT/off.reused" 2>/dev/null)
if [ -n "$ron" ] && [ "$ron" -gt 0 ] 2>/dev/null; then
  pass "vecalias reuses element reads ($ron)"
else
  fail "vecalias reuses element reads ('${ron}', expected > 0)"
fi
if [ "$roff" = "0" ]; then
  pass "vecalias off reuses none"
else
  fail "vecalias off reuses none ('${roff}', expected 0)"
fi
a=$(cat "$OUT/off.count" 2>/dev/null)
b=$(cat "$OUT/on.count" 2>/dev/null)
if [ -n "$a" ] && [ -n "$b" ] && [ "$b" -lt "$a" ]; then
  pass "Vec buffer facts remove loads ($a -> $b instructions)"
else
  fail "Vec buffer facts remove loads (off: ${a:-?}, on: ${b:-?})"
fi
exit $bad
