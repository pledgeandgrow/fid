#!/bin/bash
# lendref-check.sh — LF2 (llvm roadmap): a local passed by `&` / `&mut` to
# a reference parameter that its callee never lets go of is reached by
# that call alone (compiler/iropt_me.fd ime_note_ref_params,
# ime_unescaped = 2; pass name `lendref`, on by default since Nov 17:
# IME_DEFAULT_ON). A later call not given the local cannot change it, so
# forward / loadhoist / dse keep their facts across it.
#
# Usage: tests/tooling/lendref-check.sh   (FID=<compiler>, default
#        release/fid.exe; OUT=<dir>, default build/lendref;
#        TARGET=<emit flags>, default none (pe_ir); TARGET=--nat for the
#        machine layer; SUITE=1 also runs check 4; JOBS=<n> for it)
#
# Checks, one line each (PASS/FAIL), exit status the number of FAILs:
#  1. test_midend_lend_ref.fd emitted three ways prints its expected lines
#     (FID_VERIFY=1): default; lendref off; lendref on with inline2,
#     inlineclo and unroll off (so the calls it reasons about stay calls).
#  2. With inline2, inlineclo and unroll off, the instructions after iropt
#     (FID_IROPT_STATS "iropt: instructions A -> B") are fewer with lendref
#     on than off (Nov 16 compiler: 1029 -> 1026).
#  3. test_midend_lend_ref_escape.fd (callees that let go of the reference
#     through unsafe code) prints its expected lines by default and with
#     inline2, inlineclo and unroll off: a wrong lending fact prints 5 / 6
#     / 2 in place of 9 / 8 / 4.
#  4. (SUITE=1) tests/tooling/midend-pass-suite-check.sh PASS=lendref on
#     the whole suite (lendref named off vs on, FID_VERIFY=1, EXTRA from
#     TARGET): no DIFF / BUILD / VERIFY line.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/lendref}"
TARGET="${TARGET:-}"
SUITE="${SUITE:-0}"
JOBS="${JOBS:-1}"
mkdir -p "$OUT"
bad=0
pass() { echo "PASS $1"; }
fail() { echo "FAIL $1"; bad=$((bad + 1)); }
want() { # want <test.fd> <file>: its expected lines
  awk '/^\/\/ Expected output:/ {on=1; next} on && /^\/\/ / {sub(/^\/\/ /, ""); print; next} on {exit}' "$1" > "$2"
}

one() { # one <test.fd> <label> <env...>
  local t="$1" label="$2"; shift 2
  want "$t" "$OUT/$label.want"
  rm -f "$OUT/$label.exe"
  env "$@" FID_VERIFY=1 FID_IROPT_STATS=1 "$FID" emit "$t" $TARGET -o "$OUT/$label.exe" > "$OUT/$label.log" 2>&1
  if [ -f "$OUT/$label.exe" ] && timeout 60 "$OUT/$label.exe" > "$OUT/$label.out" 2>&1 < /dev/null && cmp -s <(tr -d '\r' < "$OUT/$label.out") "$OUT/$label.want"; then
    pass "emit $(basename "$t" .fd) $label"
  else
    fail "emit $(basename "$t" .fd) $label"
  fi
  if grep -q "IR verifier" "$OUT/$label.log"; then
    fail "IR verifier ($label): $(grep -m1 'IR verifier' "$OUT/$label.log")"
  fi
  grep -m1 "^iropt: instructions" "$OUT/$label.log" | sed 's/.* -> \([0-9]*\).*/\1/' > "$OUT/$label.count"
}
L=tests/bootstrap/test_midend_lend_ref.fd
E=tests/bootstrap/test_midend_lend_ref_escape.fd
one $L default X=1
one $L off FID_IROPT_OFF=lendref
one $L on FID_IROPT_OFF=inline2,inlineclo,unroll
one $L plain FID_IROPT_OFF=inline2,inlineclo,unroll,lendref
one $E esc-default X=1
one $E esc-plain FID_IROPT_OFF=inline2,inlineclo,unroll

a=$(cat "$OUT/plain.count" 2>/dev/null)
b=$(cat "$OUT/on.count" 2>/dev/null)
if [ -n "$a" ] && [ -n "$b" ] && [ "$b" -lt "$a" ]; then
  pass "lent locals keep their facts across calls ($a -> $b instructions)"
else
  fail "lent locals keep their facts across calls (off: ${a:-?}, on: ${b:-?})"
fi

if [ "$SUITE" = 1 ]; then
  if FID="$FID" PASS=lendref EXTRA="$TARGET" JOBS="$JOBS" OUT="$OUT/suite" bash tests/tooling/midend-pass-suite-check.sh > "$OUT/suite.txt" 2>&1; then
    pass "suite: $(tail -1 "$OUT/suite.txt")"
  else
    grep -E '^(DIFF|BUILD|VERIFY)' "$OUT/suite.txt" | head -20
    fail "suite: $(tail -1 "$OUT/suite.txt")"
  fi
fi
exit $bad
