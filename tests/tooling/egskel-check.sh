#!/bin/bash
# egskel-check.sh — ME7 (own-backend roadmap): the e-graph's side-effect
# skeleton (compiler/egraph.fd eg_skeleton, Cranelift's aegraph), opt-in
# FID_IROPT_ON=egskel.
#
# Usage: tests/tooling/egskel-check.sh   (FID=<compiler>, default
#        release/fid.exe; OUT=<dir>, default build/egskel; TARGET=<emit
#        flags>, default `--nat` (Windows x64 on the machine layer, where
#        the e-graph runs); on Linux use TARGET="--target x86_64-linux";
#        SUITE=1 also runs check 4; JOBS is not used: one build at a time)
#
# Checks, one line each (PASS/FAIL), exit status the number of FAILs:
#  1. tests/bootstrap/test_midend_egskel.fd emitted with the e-graph off,
#     on, with the skeleton on, and with the skeleton and placement (egplace)
#     on, each with FID_VERIFY=1 (the IR verifier after every mid-end stage):
#     each prints its expected lines;
#  2. with the skeleton on, FID_EGRAPH_STATS=1 reports it: at least 3
#     functions, at least 5 values floated, none put back ("undone 0");
#  3. the other e-graph tests (unary, cast, rule families, budget) give
#     their expected lines with the skeleton on;
#  4. SUITE=1: every tests/bootstrap/test_*.fd that is not expected to fail,
#     emitted with the skeleton off and on: the same output and exit code
#     (a build that fails only with the skeleton on is a FAIL; the verifier
#     runs on the skeleton builds). The tests that differ are listed in
#     $OUT/suite.txt; a test whose output changes from run to run (time,
#     random seeds, addresses) differs both ways and must be triaged by
#     running it twice with the skeleton off. Each run stops after 60 s.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/egskel}"
TARGET="${TARGET:---nat}"
mkdir -p "$OUT"
bad=0
pass() { echo "PASS $1"; }
fail() { echo "FAIL $1"; bad=$((bad + 1)); }

# The "Expected output:" lines of a test's header comment
want() {
  awk '/^\/\/ Expected output:/ {on=1; next} on && /^\/\/ / {sub(/^\/\/ /, ""); print; next} on {exit}' "$1"
}

# run <exe> <out>: run a built program (a Linux target through WSL)
run_exe() {
  case "$TARGET" in
    *linux*) timeout 60 wsl -- "./$1" > "$2" 2>&1 ;;
    *) timeout 60 "./$1" > "$2" 2>&1 ;;
  esac
}

one() { # one <test> <label> <env...>
  local t="$1" label="$2"; shift 2
  local exe="$OUT/$label.exe"
  rm -f "$exe"
  want "$t" > "$OUT/$label.want"
  env "$@" FID_VERIFY=1 FID_EGRAPH_STATS=1 "$FID" emit "$t" $TARGET -o "$exe" > "$OUT/$label.log" 2>&1
  if [ -f "$exe" ] && run_exe "$exe" "$OUT/$label.out" && cmp -s <(tr -d '\r' < "$OUT/$label.out") "$OUT/$label.want"; then
    pass "emit $label"
  else
    fail "emit $label (see $OUT/$label.log, $OUT/$label.out)"
  fi
}

T=tests/bootstrap/test_midend_egskel.fd
one "$T" eg_off FID_NAT_OFF=egraph
one "$T" eg_on FID_IROPT_ON=
one "$T" skel FID_IROPT_ON=egskel
one "$T" skel_place FID_IROPT_ON=egskel,egplace

line=$(grep -o "\[egskel\] functions [0-9]*, floated [0-9]*, originals placed [0-9]*, new [0-9]*, dead [0-9]*, undone [0-9]*" "$OUT/skel.log" | tail -n 1)
fns=$(echo "$line" | sed -E 's/.*functions ([0-9]+),.*/\1/')
fl=$(echo "$line" | sed -E 's/.*floated ([0-9]+),.*/\1/')
und=$(echo "$line" | sed -E 's/.*undone ([0-9]+).*/\1/')
if [ -n "$line" ] && [ "${fns:-0}" -ge 3 ] && [ "${fl:-0}" -ge 5 ] && [ "${und:-1}" -eq 0 ]; then
  pass "skeleton stats: $line"
else
  fail "skeleton stats: '${line:-no [egskel] line}' (want functions >= 3, floated >= 5, undone 0; see $OUT/skel.log)"
fi

for t in test_midend_egraph_unary test_midend_egraph_cast test_egraph_rule_families test_egraph_budget; do
  f="tests/bootstrap/$t.fd"
  [ -f "$f" ] || continue
  one "$f" "skel_$t" FID_IROPT_ON=egskel
done

if [ "${SUITE:-0}" = 1 ]; then
  : > "$OUT/suite.txt"
  n=0; diff=0
  for f in tests/bootstrap/test_*.fd; do
    case "$f" in *_fail.fd) continue ;; esac
    b=$(basename "$f" .fd)
    rm -f "$OUT/s_off.exe" "$OUT/s_on.exe"
    "$FID" emit "$f" $TARGET -o "$OUT/s_off.exe" > "$OUT/s_off.log" 2>&1
    [ -f "$OUT/s_off.exe" ] || continue
    n=$((n + 1))
    env FID_IROPT_ON=egskel FID_VERIFY=1 "$FID" emit "$f" $TARGET -o "$OUT/s_on.exe" > "$OUT/s_on.log" 2>&1
    if [ ! -f "$OUT/s_on.exe" ]; then
      echo "BUILD $b: $(tail -n 3 "$OUT/s_on.log" | tr '\n' ' ')" >> "$OUT/suite.txt"
      diff=$((diff + 1)); continue
    fi
    run_exe "$OUT/s_off.exe" "$OUT/s_off.out" < /dev/null; r1=$?
    run_exe "$OUT/s_on.exe" "$OUT/s_on.out" < /dev/null; r2=$?
    if [ $r1 -ne $r2 ] || ! cmp -s "$OUT/s_off.out" "$OUT/s_on.out"; then
      echo "DIFF $b: exit $r1 / $r2" >> "$OUT/suite.txt"
      diff=$((diff + 1))
    fi
  done
  if [ $diff -eq 0 ] && [ $n -gt 0 ]; then
    pass "suite: $n tests, the same with the skeleton on"
  else
    fail "suite: $diff of $n tests differ with the skeleton on (see $OUT/suite.txt)"
  fi
fi
exit $bad
