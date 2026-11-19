#!/bin/bash
# linear-alias-check.sh — OP26 (competitive roadmap): @linear parameters as
# alias facts (compiler/iropt_me.fd ime_unescaped / ime_may_alias /
# ime_call_writes, `linear`) and loads of one place through two address
# instructions forwarded (ime_forward, `placefwd`); both on by default
# since Nov 17 (FID_IROPT_OFF=linear,placefwd turns them off).
#
# Usage: tests/tooling/linear-alias-check.sh   (FID=<compiler>, default
#        release/fid.exe; OUT=<dir>, default build/linearalias;
#        TARGET=<emit flags>, default `--nat`)
#
# Checks, one line each (PASS/FAIL), exit status the number of FAILs:
#  1. test_midend_linear_alias.fd emitted with neither, placefwd alone, and
#     both: each prints its expected lines (FID_VERIFY=1);
#  2. the mid-end's instruction count (FID_IROPT_STATS) with both is below
#     the count with placefwd alone: the linear fact let a reload after a
#     call go.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/linearalias}"
TARGET="${TARGET:---nat}"
mkdir -p "$OUT"
T=tests/bootstrap/test_midend_linear_alias.fd
bad=0
awk '/^\/\/ Expected output:/ {on=1; next} on && /^\/\/ / {sub(/^\/\/ /, ""); print; next} on {exit}' "$T" > "$OUT/want"
same() { cmp -s <(tr -d '\r' < "$1") "$OUT/want"; }
pass() { echo "PASS $1"; }
fail() { echo "FAIL $1"; bad=$((bad + 1)); }

one() { # one <label> <env...>
  local label="$1"; shift
  rm -f "$OUT/$label.exe"
  # inline2 (on by default since OP5) is off: it would copy the calls the
  # linear fact is about into main, and the counts would compare other code
  env FID_IROPT_OFF=inline2 "$@" FID_VERIFY=1 FID_IROPT_STATS=1 "$FID" emit "$T" $TARGET -o "$OUT/$label.exe" > "$OUT/$label.log" 2>&1
  if [ -f "$OUT/$label.exe" ] && "$OUT/$label.exe" > "$OUT/$label.out" 2>&1 && same "$OUT/$label.out"; then
    pass "emit $label"
  else
    fail "emit $label"
  fi
  grep "^iropt: instructions" "$OUT/$label.log" | sed 's/.* -> \([0-9]*\).*/\1/' > "$OUT/$label.count"
}
# linear and placefwd are on by default since Nov 17 (IME_DEFAULT_ON):
# the first two builds name them off (inline2 stays off in all three)
one plain FID_IROPT_OFF=inline2,linear,placefwd
one placefwd FID_IROPT_OFF=inline2,linear
one linear FID_IROPT_OFF=inline2
a=$(cat "$OUT/placefwd.count" 2>/dev/null)
b=$(cat "$OUT/linear.count" 2>/dev/null)
if [ -n "$a" ] && [ -n "$b" ] && [ "$b" -lt "$a" ]; then
  pass "linear fact removes reloads ($a -> $b instructions)"
else
  fail "linear fact removes reloads (placefwd: ${a:-?}, with linear: ${b:-?})"
fi
exit $bad
