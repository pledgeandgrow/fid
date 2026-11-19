#!/bin/bash
# quietcall-egplace-check.sh — OP25 (competitive roadmap: unused quiet
# calls removed, quiet calls moved out of any block of a loop;
# compiler/iropt.fd iro_quiet_fns, opt-in FID_IROPT_ON=quietcall), ME7
# (llvm roadmap: casts and float values in the e-graph, compiler/egraph.fd)
# and ME8 (placement, compiler/egraph.fd eg_place, opt-in
# FID_IROPT_ON=egplace).
#
# Usage: tests/tooling/quietcall-egplace-check.sh   (FID=<compiler>, default
#        release/fid.exe; OUT=<dir>, default build/quietcall; TARGET=<emit
#        flags>, default `--nat` (Windows x64 on the machine layer, where
#        the e-graph runs); on Linux use TARGET="--target x86_64-linux")
#
# Checks, one line each (PASS/FAIL), exit status the number of FAILs:
#  1. test_midend_quietcall.fd emitted with the pass off, on, and on with
#     LICM off: each prints its expected lines (FID_VERIFY=1: the IR
#     verifier runs after every mid-end stage);
#  2. with it on, FID_IROPT_STATS reports quiet functions and at least one
#     function whose dead calls went;
#  3. test_midend_egraph_cast.fd emitted with the e-graph on, off, with
#     placement on, and with placement and quietcall on: each prints its
#     expected lines.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/quietcall}"
TARGET="${TARGET:---nat}"
mkdir -p "$OUT"
bad=0
pass() { echo "PASS $1"; }
fail() { echo "FAIL $1"; bad=$((bad + 1)); }

# The "Expected output:" lines of a test's header comment
want() {
  awk '/^\/\/ Expected output:/ {on=1; next} on && /^\/\/ / {sub(/^\/\/ /, ""); print; next} on {exit}' "$1"
}

one() { # one <test> <label> <env...>
  local t="$1" label="$2"; shift 2
  local exe="$OUT/$label.exe"
  rm -f "$exe"
  want "$t" > "$OUT/$label.want"
  # inline2 (on by default since OP5) is off: inlined, the quiet calls
  # this checks would not be calls any more
  env FID_IROPT_OFF=inline2 "$@" FID_VERIFY=1 FID_IROPT_STATS=1 "$FID" emit "$t" $TARGET -o "$exe" > "$OUT/$label.log" 2>&1
  if [ -f "$exe" ] && "$exe" > "$OUT/$label.out" 2>&1 && cmp -s <(tr -d '\r' < "$OUT/$label.out") "$OUT/$label.want"; then
    pass "emit $label"
  else
    fail "emit $label (see $OUT/$label.log, $OUT/$label.out)"
  fi
}

Q=tests/bootstrap/test_midend_quietcall.fd
# quietcall is on by default since Nov 17 (OP25): "off" names it off
one "$Q" qc_off FID_IROPT_OFF=quietcall,inline2
one "$Q" qc_on FID_IROPT_ON=quietcall
one "$Q" qc_on_nolicm FID_IROPT_ON=quietcall FID_IROPT_OFF=licm,inline2
if grep -Eq "iropt: quietcall [1-9][0-9]* quiet functions, dead calls removed in [1-9]" "$OUT/qc_on.log"; then
  pass "quietcall removed dead calls"
else
  fail "quietcall removed dead calls (no stats line in $OUT/qc_on.log)"
fi

E=tests/bootstrap/test_midend_egraph_cast.fd
one "$E" eg_on X=1
one "$E" eg_off FID_NAT_OFF=egraph
one "$E" eg_place FID_IROPT_ON=egplace
one "$E" eg_place_quiet FID_IROPT_ON=egplace,quietcall
exit $bad
