#!/bin/bash
# inline2-check.sh — ME20/ME21 (llvm-roadmap): the cost-model inliner
# (compiler/iropt_me.fd ime_inline, on by default since OP5;
# FID_IROPT_OFF=inline2 turns it off) gives
# the same program as without it.
#
# Usage: tests/tooling/inline2-check.sh   (FID=<compiler>, default
#        release/fid.exe; OUT=<dir>, default build/inline2)
#
# Checks, one line each (PASS/FAIL), exit status the number of FAILs:
#  1. `fid emit` of test_midend_inline2.fd with FID_IROPT_STATS=1 and
#     FID_IROPT_ON=inline2: more instructions reach iropt's function passes
#     than without it (callees copied in; FID_VERIFY=1: the IR verifier
#     runs after the passes); with FID_IROPT_OFF=inline2 its pass table
#     has no inline2 row; with neither (OP5: on by default) it has one and
#     the same count as when named;
#  2. both programs print the expected lines.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/inline2}"
mkdir -p "$OUT"
T=tests/bootstrap/test_midend_inline2.fd
bad=0
pass() { echo "PASS $1"; }
fail() { echo "FAIL $1"; bad=$((bad + 1)); }
sed -n 's|^// \([0-9][0-9 ]*\)$|\1|p' "$T" > "$OUT/want"
same() { cmp -s <(tr -d '\r' < "$1") "$OUT/want"; }

rm -f "$OUT/on.exe" "$OUT/off.exe" "$OUT/default.exe"
FID_IROPT_STATS=1 FID_IROPT_ON=inline2 FID_VERIFY=1 "$FID" emit "$T" -o "$OUT/on.exe" > "$OUT/on.log" 2>&1
FID_IROPT_STATS=1 FID_IROPT_OFF=inline2 "$FID" emit "$T" -o "$OUT/off.exe" > "$OUT/off.log" 2>&1
# OP5: on by default since Nov 16 (iropt_me.fd IME_DEFAULT_ON)
FID_IROPT_STATS=1 FID_VERIFY=1 "$FID" emit "$T" -o "$OUT/default.exe" > "$OUT/default.log" 2>&1
# iropt's instruction count before its function passes: inline2 (run
# before them) copied callees in only when on
before() { tr -d '\r' < "$1" | sed -n 's/^iropt: instructions \([0-9]*\) -> .*/\1/p' | tail -1; }
bon=$(before "$OUT/on.log"); boff=$(before "$OUT/off.log"); bdef=$(before "$OUT/default.log")
if [ "${bon:-0}" -gt "${boff:-0}" ]; then pass "inline2 runs when named (${bon:-?} vs ${boff:-?} instructions)"; else fail "inline2 runs when named (${bon:-?} vs ${boff:-?} instructions)"; fi
if grep -q "iropt: pass inline2:" "$OUT/off.log"; then fail "FID_IROPT_OFF=inline2 turns it off"; else pass "FID_IROPT_OFF=inline2 turns it off"; fi
if grep -q "iropt: pass inline2:" "$OUT/default.log" && [ "${bdef:-0}" = "${bon:-x}" ]; then pass "inline2 is on by default (${bdef:-?} instructions)"; else fail "inline2 is on by default (${bdef:-?} vs ${bon:-?} instructions)"; fi
for m in on off default; do
  if [ -f "$OUT/$m.exe" ] && "$OUT/$m.exe" > "$OUT/$m.out" 2>&1 && same "$OUT/$m.out"; then
    pass "emitted ($m) prints the expected lines"
  else
    fail "emitted ($m) prints the expected lines"
  fi
done
# 3. OP5: a void callee whose call's value is used (a function ending in
#    `panic(..)` returns it): with inline2 the IR verifier still passes
V=tests/bootstrap/test_midend_inline2_void.fd
sed -n 's|^// \([0-9][0-9 ]*\)$|\1|p' "$V" > "$OUT/void.want"
for m in on off; do
  rm -f "$OUT/void_$m.exe"
  if [ $m = on ]; then
    FID_VERIFY=1 FID_IROPT_ON=inline2 "$FID" emit "$V" -o "$OUT/void_$m.exe" > "$OUT/void_$m.log" 2>&1
  else
    FID_VERIFY=1 FID_IROPT_OFF=inline2 "$FID" emit "$V" -o "$OUT/void_$m.exe" > "$OUT/void_$m.log" 2>&1
  fi
  if [ -f "$OUT/void_$m.exe" ] && ! grep -q "IR verifier" "$OUT/void_$m.log" && "$OUT/void_$m.exe" > "$OUT/void_$m.out" 2>&1 && cmp -s <(tr -d '\r' < "$OUT/void_$m.out") "$OUT/void.want"; then
    pass "void callee value ($m): verifier passes, expected lines"
  else
    fail "void callee value ($m): see $OUT/void_$m.log"
  fi
done
exit $bad
