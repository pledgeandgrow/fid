#!/bin/bash
# inline-pgo-check.sh — OP5 (competitive roadmap): inline2's decisions
# weighed by a profile (compiler/iropt_me.fd ime_inline_weigh;
# FID_IROPT_OFF=inlinepgo ignores the profile).
#
# Usage: tests/tooling/inline-pgo-check.sh      (FID=<compiler>, default
#        release/fid.exe; OUT=<dir>, default build/inlinepgo)
#
# Checks, one line each (PASS/FAIL), exit status the number of FAILs:
#  1. `fid run --profile` of test_midend_inline_pgo.fd prints the expected
#     lines and writes a profile that counts `hot` 5000 times;
#  2. emitted with FID_IROPT_ON=inline2 and the profile, with inline2 and
#     no profile, and with the profile ignored (inlinepgo off): each
#     program prints the expected lines (FID_VERIFY=1: the IR verifier runs);
#  3. iropt's instruction count (FID_IROPT_STATS) differs with the profile
#     and is the same without it and with it ignored.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/inlinepgo}"
mkdir -p "$OUT"
T=tests/bootstrap/test_midend_inline_pgo.fd
bad=0
printf '37492500\n0\n3\n' > "$OUT/want"
same() { cmp -s <(tr -d '\r' < "$1") "$OUT/want"; }
pass() { echo "PASS $1"; }
fail() { echo "FAIL $1"; bad=$((bad + 1)); }

rm -f "$OUT/run.prof"
"$FID" run "$T" --profile "$OUT/run.prof" > "$OUT/run.out" 2> "$OUT/run.log"
if same "$OUT/run.out"; then pass "run --profile output"; else fail "run --profile output"; fi
if grep -q "^5000 hot$" "$OUT/run.prof" 2>/dev/null; then pass "profile counts hot"; else fail "profile counts hot"; fi

one() { # one <label> <env...>
  local label="$1"; shift
  rm -f "$OUT/$label.exe"
  env "$@" FID_IROPT_ON=inline2 FID_VERIFY=1 FID_IROPT_STATS=1 "$FID" emit "$T" $PGO -o "$OUT/$label.exe" > "$OUT/$label.log" 2>&1
  if [ -f "$OUT/$label.exe" ] && "$OUT/$label.exe" > "$OUT/$label.out" 2>&1 && same "$OUT/$label.out"; then
    pass "emit $label"
  else
    fail "emit $label"
  fi
  # iropt's instruction count before its function passes (after inlining)
  tr -d '\r' < "$OUT/$label.log" | sed -n 's/^iropt: instructions \([0-9]*\) -> .*/\1/p' | tail -1 > "$OUT/$label.count"
}
PGO="--profile-use $OUT/run.prof" one profile X=1
PGO="" one noprofile X=1
PGO="--profile-use $OUT/run.prof" one ignored FID_IROPT_OFF=inlinepgo
# 3. the profile, and only it, changes what inline2 copies in: `hot` (in
#    the loop, over IME_INLINE_MAX) only with it, `cold` (never called)
#    only without it. iro_inline does not take either (several blocks).
cp=$(cat "$OUT/profile.count" 2>/dev/null); cn=$(cat "$OUT/noprofile.count" 2>/dev/null); ci=$(cat "$OUT/ignored.count" 2>/dev/null)
if [ -n "$cp" ] && [ -n "$cn" ] && [ "$cp" != "$cn" ]; then pass "the profile changes inlining ($cn -> $cp instructions)"; else fail "the profile changes inlining (${cn:-?} vs ${cp:-?})"; fi
if [ -n "$ci" ] && [ "$ci" = "$cn" ]; then pass "inlinepgo off ignores the profile"; else fail "inlinepgo off ignores the profile (${ci:-?} vs ${cn:-?})"; fi
exit $bad
