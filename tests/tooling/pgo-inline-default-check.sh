#!/bin/bash
# pgo-inline-default-check.sh — OP30 (competitive roadmap): the default
# inliner (compiler/iropt.fd iro_inline, iro_inline_limit) weighs a real
# run's profile: a callee the run called 1000 times or more may be twice
# the size limit, one it never called only half (FID_IROPT_OFF=inlinepgo
# ignores the profile).
#
# Usage: tests/tooling/pgo-inline-default-check.sh   (FID=<compiler>,
#        default release/fid.exe; OUT=<dir>, default build/pgoinline;
#        TARGET=<emit flags>, default `--nat`)
#
# Checks, one line each (PASS/FAIL), exit status the number of FAILs:
#  1. `fid run --profile` of test_pgo_inline_default.fd prints the expected
#     lines and counts `hot` 5000 times;
#  2. emitted with the profile, without it, and with it ignored: each
#     prints the expected lines (FID_VERIFY=1);
#  3. the mid-end's instruction count (FID_IROPT_STATS) differs with the
#     profile, and is the same without it and with it ignored: the
#     profile, and only it, changed what was inlined.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/pgoinline}"
TARGET="${TARGET:---nat}"
mkdir -p "$OUT"
T=tests/bootstrap/test_pgo_inline_default.fd
bad=0
awk '/^\/\/ Expected output:/ {on=1; next} on && /^\/\/ / {sub(/^\/\/ /, ""); print; next} on {exit}' "$T" > "$OUT/want"
same() { cmp -s <(tr -d '\r' < "$1") "$OUT/want"; }
pass() { echo "PASS $1"; }
fail() { echo "FAIL $1"; bad=$((bad + 1)); }

rm -f "$OUT/run.prof"
"$FID" run "$T" --profile "$OUT/run.prof" > "$OUT/run.out" 2> "$OUT/run.log"
grep -v '^\[' "$OUT/run.out" | grep -v '^profile written' > "$OUT/run.clean"
if same "$OUT/run.clean"; then pass "run --profile output"; else fail "run --profile output"; fi
if grep -q "^5000 hot$" "$OUT/run.prof" 2>/dev/null; then pass "profile counts hot"; else fail "profile counts hot"; fi

one() { # one <label> <env...>
  local label="$1"; shift
  rm -f "$OUT/$label.exe"
  # inline2 (on by default since OP5, and weighing the profile itself:
  # tests/tooling/inline-pgo-check.sh) is off here: only iro_inline decides
  env FID_IROPT_OFF=inline2 "$@" FID_VERIFY=1 FID_IROPT_STATS=1 "$FID" emit "$T" $TARGET $PGO -o "$OUT/$label.exe" > "$OUT/$label.log" 2>&1
  if [ -f "$OUT/$label.exe" ] && "$OUT/$label.exe" > "$OUT/$label.out" 2>&1 && same "$OUT/$label.out"; then
    pass "emit $label"
  else
    fail "emit $label"
  fi
  grep "^iropt: instructions" "$OUT/$label.log" | sed 's/ in [0-9]* ms//' > "$OUT/$label.count"
}
PGO="--profile-use $OUT/run.prof" one profile X=1
PGO="" one noprofile X=1
PGO="--profile-use $OUT/run.prof" one ignored FID_IROPT_OFF=inlinepgo,inline2
if [ -s "$OUT/profile.count" ] && ! cmp -s "$OUT/profile.count" "$OUT/noprofile.count"; then
  pass "the profile changes inlining"
else
  fail "the profile changes inlining ($(cat "$OUT/profile.count" 2>/dev/null) vs $(cat "$OUT/noprofile.count" 2>/dev/null))"
fi
if [ -s "$OUT/ignored.count" ] && cmp -s "$OUT/ignored.count" "$OUT/noprofile.count"; then
  pass "inlinepgo off ignores it"
else
  fail "inlinepgo off ignores it"
fi
exit $bad
