#!/bin/bash
# pgo-block-check.sh — OP30 (competitive roadmap): block-level profiles.
# `fid run --profile` counts how often each block ran (compiler/pgo.fd
# pgo_instrument: a probe call "<function>@<block id>" at each block's start
# of every function of more than one block; pgo_write: "fides-profile 2",
# then per function "<calls> <function>" and "<1 + times entered>
# <function>@<block id>" per block), and `fid emit --profile-use` weighs
# blocks: the default inliner by the call's block (iropt.fd
# iro_inline_limit, pgo_site_calls), unrolling skips a loop the run never
# ran (iropt_me.fd ime_unroll_one, "cold (profile)" in FID_IROPT_STATS),
# nat_ir lays never-entered blocks last (ng_pgo_cold_labels).
#
# Usage: tests/tooling/pgo-block-check.sh   (FID=<compiler>, default
#        release/fid.exe; OUT=<dir>, default build/pgoblock; TARGET=<emit
#        flags>, default `--nat` on Windows, `--target x86_64-linux` on
#        Linux x86-64)
#
# Checks, one line each (PASS/FAIL); exit status the number of FAILs:
#  1. run --profile prints the expected lines; the profile is version 2,
#     counts scan 3000 times, its entry block 3001 (1 + 3000), a block of
#     scan and one of rare as never entered (1), and for every function
#     with block lines its entry block (@0) as 1 + its calls;
#  2. FID_PGO_BLOCKS=0: the same output, no block lines;
#  3. emit with the profile (FID_VERIFY=1, inline2 off so only iro_inline
#     decides): prints the expected lines; the remark says scan's call to
#     mid is not inlined with "the limit is 8" (a cold site), and no remark
#     names sum_mid's call to mid (a hot site: inlined);
#  4. emit without the profile and with FID_IROPT_OFF=inlinepgo: no remark
#     for scan's call to mid (mid fits the default limit, 16);
#  5. FID_IROPT_STATS: "cold (profile)" > 0 with the profile (rare's loop
#     left alone), 0 without it and with FID_IROPT_OFF=unrollpgo;
#  6. a version 1 profile (no block lines) is still read: emit prints the
#     expected lines, no "not a fides profile" warning.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/pgoblock}"
if [ -z "${TARGET+x}" ]; then
  TARGET="--nat"
  case "$(uname -s)" in
    Linux) case "$(uname -m)" in
             x86_64) TARGET="--target x86_64-linux" ;;
             aarch64|arm64) TARGET="--target aarch64-linux" ;;
           esac ;;
  esac
fi
mkdir -p "$OUT"
T=tests/bootstrap/test_pgo_block_profile.fd
bad=0
awk '/^\/\/ Expected output:/ {on=1; next} on && /^\/\/ / {sub(/^\/\/ /, ""); print; next} on {exit}' "$T" > "$OUT/want"
same() { cmp -s <(tr -d '\r' < "$1" | grep -v '^\[' | grep -v '^profile written') "$OUT/want"; }
pass() { echo "PASS $1"; }
fail() { echo "FAIL $1"; bad=$((bad + 1)); }
has() { tr -d '\r' < "$2" | grep -q -- "$1"; }

# 1. the profile
P="$OUT/run.prof"
rm -f "$P"
"$FID" run "$T" --profile "$P" > "$OUT/run.out" 2> "$OUT/run.log"
if same "$OUT/run.out"; then pass "run --profile output"; else fail "run --profile output"; fi
if [ -f "$P" ] && [ "$(head -1 "$P" | tr -d '\r')" = "fides-profile 2" ]; then pass "profile version 2"; else fail "profile version 2"; fi
if has "^3000 scan$" "$P"; then pass "scan called 3000 times"; else fail "scan called 3000 times"; fi
if has "^3001 scan@0$" "$P"; then pass "scan's entry block entered 3000 times"; else fail "scan's entry block entered 3000 times"; fi
if has "^1 scan@[0-9]*$" "$P"; then pass "a block of scan never entered"; else fail "a block of scan never entered"; fi
if has "^1 rare@[0-9]*$" "$P"; then pass "a block of rare never entered"; else fail "a block of rare never entered"; fi
# the test's functions of more than one block: entry block = 1 + calls
# (their entry block is no loop header)
badentry=$(tr -d '\r' < "$P" | awk 'NR > 1 {
    n = index($2, "@");
    if (n == 0) { calls[$2] = $1 } else if (substr($2, n + 1) == "0") { entry[substr($2, 1, n - 1)] = $1 }
  } END { split("main scan rare sum_mid", fs, " "); for (i = 1; i <= 4; i++) { f = fs[i]; if (!(f in entry) || entry[f] != calls[f] + 1) print "  " f ": calls " calls[f] ", entry block " entry[f] } }')
if [ -z "$badentry" ]; then pass "entry blocks = 1 + calls"; else fail "entry blocks = 1 + calls"; echo "$badentry" | head -5; fi

# 2. calls only
rm -f "$OUT/calls.prof"
FID_PGO_BLOCKS=0 "$FID" run "$T" --profile "$OUT/calls.prof" > "$OUT/calls.out" 2> "$OUT/calls.log"
if same "$OUT/calls.out" && [ -f "$OUT/calls.prof" ] && ! grep -q "@" "$OUT/calls.prof"; then
  pass "FID_PGO_BLOCKS=0: no block lines"
else
  fail "FID_PGO_BLOCKS=0: no block lines"
fi

one() { # one <label> <profile or ""> <env...>
  local label="$1" prof="$2"; shift 2
  local pu=""
  if [ -n "$prof" ]; then pu="--profile-use $prof"; fi
  rm -f "$OUT/$label.exe"
  env FID_IROPT_OFF=inline2 "$@" FID_VERIFY=1 FID_IROPT_STATS=1 "$FID" emit "$T" $TARGET $pu --remarks -o "$OUT/$label.exe" > "$OUT/$label.log" 2>&1 < /dev/null
  if [ -f "$OUT/$label.exe" ] && "$OUT/$label.exe" > "$OUT/$label.out" 2>&1 && same "$OUT/$label.out"; then
    pass "emit $label"
  else
    fail "emit $label"
  fi
}
cold() { # the "cold (profile)" count of a log
  tr -d '\r' < "$1" | sed -n 's/^iropt: ivsr .* run-time, \([0-9]*\) cold (profile).*/\1/p' | tail -1
}

# 3. with the profile
one profile "$P" X=1
if has "fn scan: call to mid not inlined: too large (.*the limit is 8)" "$OUT/profile.log"; then
  pass "cold site: scan keeps its call to mid"
else
  fail "cold site: scan keeps its call to mid"
fi
if has "fn sum_mid: call to mid not inlined" "$OUT/profile.log"; then
  fail "hot site: sum_mid inlines mid"
else
  pass "hot site: sum_mid inlines mid"
fi

# 4. without it, and ignored
one noprofile "" X=1
one ignored "$P" FID_IROPT_OFF=inline2,inlinepgo,unrollpgo
for l in noprofile ignored; do
  if has "fn scan: call to mid not inlined" "$OUT/$l.log"; then fail "$l: scan inlines mid"; else pass "$l: scan inlines mid"; fi
done

# 5. unrolling
c1=$(cold "$OUT/profile.log"); c2=$(cold "$OUT/noprofile.log"); c3=$(cold "$OUT/ignored.log")
if [ "${c1:-0}" -gt 0 ] && [ "${c2:-x}" = "0" ] && [ "${c3:-x}" = "0" ]; then
  pass "unroll leaves the cold loop (profile ${c1}, none ${c2}, unrollpgo off ${c3})"
else
  fail "unroll leaves the cold loop (profile '${c1:-}', none '${c2:-}', unrollpgo off '${c3:-}')"
fi

# 6. a version 1 profile
{ echo "fides-profile 1"; tr -d '\r' < "$P" | sed 1d | grep -v "@"; } > "$OUT/v1.prof"
one v1 "$OUT/v1.prof" X=1
if has "not a fides profile" "$OUT/v1.log"; then fail "version 1 read"; else pass "version 1 read"; fi
exit $bad
