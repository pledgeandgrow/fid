#!/bin/bash
# inlineclo-check.sh — WP16 (llvm roadmap): a closure that closure
# devirtualization made a direct call and that is called at one site is
# copied into its caller. inline2's once-called rule does it when inline2
# is on (the default); `inlineclo` (compiler/iropt_me.fd
# ime_inline_closures, on by default since Nov 17: IME_DEFAULT_ON) does it
# alone when inline2 is off (compiler/iropt.fd iropt_native).
#
# Usage: tests/tooling/inlineclo-check.sh   (FID=<compiler>, default
#        release/fid.exe; OUT=<dir>, default build/inlineclo;
#        TARGET=<emit flags>, default none (pe_ir); TARGET=--nat for the
#        machine layer; SUITE=1 also runs check 4; JOBS=<n> for it)
#
# Checks, one line each (PASS/FAIL), exit status the number of FAILs:
#  1. test_midend_inline_closure.fd emitted three ways prints its expected
#     lines (FID_VERIFY=1): default; inline2 off (inlineclo by default);
#     inline2 and inlineclo off.
#  2. FID_IROPT_STATS's "iropt: inline2 N sites" (it counts inlineclo's
#     sites too): N >= 2 by default and with inline2 off (sum_to and pick
#     are called once each), N = 0 with both off.
#  3. The closures' bodies left their calls: with inline2 off, the
#     instructions after iropt with inlineclo on differ from off (a site
#     inlined), and the build with it on passes the IR verifier.
#  4. (SUITE=1) every program of tests/bootstrap (not expected to fail, no
#     outside library) built with inline2 off, inlineclo on and off
#     (FID_VERIFY=1 both): same output and exit status. One line per
#     program, SAME / DIFF / BUILD / VERIFY / SKIP, as
#     midend-pass-suite-check.sh; DIFF, BUILD and VERIFY are failures.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/inlineclo}"
TARGET="${TARGET:-}"
SUITE="${SUITE:-0}"
JOBS="${JOBS:-1}"
mkdir -p "$OUT"
T=tests/bootstrap/test_midend_inline_closure.fd
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
  if grep -q "IR verifier" "$OUT/$label.log"; then
    fail "IR verifier ($label): $(grep -m1 'IR verifier' "$OUT/$label.log")"
  fi
  grep -m1 "^iropt: instructions" "$OUT/$label.log" | sed 's/.* -> \([0-9]*\).*/\1/' > "$OUT/$label.count"
  grep -m1 "^iropt: inline2 " "$OUT/$label.log" | sed -n 's/^iropt: inline2 \([0-9]*\) sites.*/\1/p' > "$OUT/$label.sites"
}
one default X=1
one clo FID_IROPT_OFF=inline2
one none FID_IROPT_OFF=inline2,inlineclo

sd=$(cat "$OUT/default.sites" 2>/dev/null)
sc=$(cat "$OUT/clo.sites" 2>/dev/null)
sn=$(cat "$OUT/none.sites" 2>/dev/null)
if [ -n "$sd" ] && [ "$sd" -ge 2 ] 2>/dev/null; then
  pass "inline2 (default) inlines the once-called closures ($sd sites)"
else
  fail "inline2 (default) inlines the once-called closures ('${sd}', expected >= 2)"
fi
if [ -n "$sc" ] && [ "$sc" -ge 2 ] 2>/dev/null; then
  pass "inlineclo (inline2 off) inlines the once-called closures ($sc sites)"
else
  fail "inlineclo (inline2 off) inlines the once-called closures ('${sc}', expected >= 2)"
fi
if [ "$sn" = "0" ]; then
  pass "inline2 and inlineclo off inline nothing"
else
  fail "inline2 and inlineclo off inline nothing ('${sn}', expected 0)"
fi
a=$(cat "$OUT/none.count" 2>/dev/null)
b=$(cat "$OUT/clo.count" 2>/dev/null)
if [ -n "$a" ] && [ -n "$b" ] && [ "$a" != "$b" ]; then
  pass "inlineclo changes the code ($a -> $b instructions after iropt)"
else
  fail "inlineclo changes the code (off: ${a:-?}, on: ${b:-?})"
fi

if [ "$SUITE" = 1 ]; then
  TESTS=()
  for t in tests/bootstrap/*.fd; do
    case "$t" in *_fail.fd) continue ;; esac
    grep -q '^//! error' "$t" && continue
    grep -q -E 'sqlite3|SDL_|sdl2_h|glClear|use gl;|use sdl' "$t" && continue
    TESTS+=("$t")
  done
  mkdir -p "$OUT/suite"
  suite_one() {
    local t="$1" n m exe
    n=$(basename "$t" .fd)
    local rcs=()
    for m in off on; do
      exe="$OUT/suite/$n.$m.exe"
      rm -f "$exe" "$exe.out"
      if [ $m = off ]; then
        FID_IROPT_OFF=inline2,inlineclo FID_VERIFY=1 "$FID" emit "$t" $TARGET -o "$exe" > "$exe.log" 2>&1 < /dev/null
      else
        FID_IROPT_OFF=inline2 FID_VERIFY=1 "$FID" emit "$t" $TARGET -o "$exe" > "$exe.log" 2>&1 < /dev/null
      fi
      if [ -f "$exe" ]; then
        timeout 60 "$exe" > "$exe.out" 2>&1 < /dev/null
        rcs+=("$?")
      else
        rcs+=("BUILD")
      fi
    done
    local a="${rcs[0]}" b="${rcs[1]}" line voff von
    voff=$(grep -c "IR verifier" "$OUT/suite/$n.off.exe.log" 2>/dev/null)
    von=$(grep -c "IR verifier" "$OUT/suite/$n.on.exe.log" 2>/dev/null)
    if [ "$a" = "BUILD" ] && [ "$b" = "BUILD" ]; then
      line="SKIP   $n (builds with neither)"
    elif [ "$a" = "BUILD" ]; then
      line="SKIP   $n (builds only with inlineclo on)"
    elif [ "$b" = "BUILD" ]; then
      line="BUILD  $n (on: $(grep -m1 -i -E 'error|panic|invalid|verifier' "$OUT/suite/$n.on.exe.log"))"
    elif [ "${von:-0}" -gt "${voff:-0}" ]; then
      line="VERIFY $n ($(grep -m1 'IR verifier' "$OUT/suite/$n.on.exe.log"))"
    elif [ "$a" = "$b" ] && cmp -s <(tr -d '\r' < "$OUT/suite/$n.off.exe.out") <(tr -d '\r' < "$OUT/suite/$n.on.exe.out"); then
      line="SAME   $n (rc $a)"
    else
      line="DIFF   $n (rc off $a, on $b)"
    fi
    echo "$line" > "$OUT/suite/$n.res"
  }
  export -f suite_one
  export FID OUT TARGET
  printf '%s\n' "${TESTS[@]}" | xargs -P "$JOBS" -I{} bash -c 'suite_one "$1"' _ {}
  : > "$OUT/suite/results.txt"
  for t in "${TESTS[@]}"; do
    n=$(basename "$t" .fd)
    [ -f "$OUT/suite/$n.res" ] && cat "$OUT/suite/$n.res" >> "$OUT/suite/results.txt"
  done
  cat "$OUT/suite/results.txt"
  sbad=$(grep -c -E '^(DIFF|BUILD|VERIFY)' "$OUT/suite/results.txt")
  ssame=$(grep -c '^SAME' "$OUT/suite/results.txt")
  if [ "$sbad" = 0 ]; then
    pass "suite: ${#TESTS[@]} programs, $ssame the same with inlineclo on and off (inline2 off)"
  else
    fail "suite: $sbad programs differ, fail to build or to verify with inlineclo on (inline2 off)"
  fi
fi
exit $bad
