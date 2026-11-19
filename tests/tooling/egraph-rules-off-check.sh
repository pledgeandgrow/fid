#!/bin/bash
# egraph-rules-off-check.sh — RL3 (llvm roadmap): the pass matrix turns each
# rule family of the e-graph off on its own (FID_EGRAPH_OFF, compiler/egraph.fd
# eg_rules_off_mask), and its comparison of two runs is right.
#
# Usage: tests/tooling/egraph-rules-off-check.sh
#   FID=<compiler>  default release/fid.exe (release/fid off Windows)
#   TARGET=<emit flags>  default `--nat` on Windows, `--target x86_64-linux`
#           elsewhere (the machine layer, where the e-graph runs)
#   OUT=<dir>       default build/egraph-rules-off
#   MATRIX=0        skip check 5 (a pass-matrix run, one build per family)
#
# Checks, one line each (PASS/FAIL), exit status the number of FAILs:
#  1. scripts/pass-matrix.sh SELFTEST=1: its comparison counts a changed
#     verdict, a changed output or exit code of a failing test, a test that
#     did not build and a missing run as differences, and CRLF alone not;
#     it finds the e-graph's families in the source.
#  2. tests/bootstrap/test_egraph_rule_families.fd emitted with the e-graph
#     as it is, with FID_EGRAPH_OFF=all and with each family off: each
#     prints its expected lines (FID_VERIFY=1: the IR verifier runs after
#     the e-graph).
#  3. two default builds are the same bytes (so 4 can compare them).
#  4. FID_EGRAPH_OFF=all gives other bytes than the default, or
#     FID_EGRAPH_STATS reports fewer rewrites: the compiler reads the
#     switch (without eg_rules_off_mask both are the same).
#  5. scripts/pass-matrix.sh round 4 alone (IROPT, NAT, IME, RULES empty) on
#     that test: exit 0, one "egraph rule <family> off" line per family.
set -u
cd "$(dirname "$0")/../.." || exit 1
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; *) WIN=0 ;; esac
if [ $WIN = 1 ]; then
  FID="${FID:-release/fid.exe}"; TARGET="${TARGET:---nat}"; PMT=x86_64-windows; EXE=.exe; RUNX=""
else
  FID="${FID:-release/fid}"; TARGET="${TARGET:---target x86_64-linux}"; PMT=x86_64-linux; EXE=""; RUNX=""
fi
OUT="${OUT:-build/egraph-rules-off}"
mkdir -p "$OUT"
export FIDES_NO_CACHE=1
bad=0
pass() { echo "PASS $1"; }
fail() { echo "FAIL $1"; bad=$((bad + 1)); }

T=tests/bootstrap/test_egraph_rule_families.fd
want() {
  awk '/^\/\/ Expected output:/ {on=1; next} on && /^\/\/ / {sub(/^\/\/ /, ""); print; next} on {exit}' "$1"
}
want "$T" > "$OUT/want.txt"

# 1
if SELFTEST=1 bash scripts/pass-matrix.sh > "$OUT/selftest.log" 2>&1; then
  pass "pass-matrix selftest ($(tail -n 1 "$OUT/selftest.log"))"
else
  fail "pass-matrix selftest: $(cat "$OUT/selftest.log")"
fi
FAMS=$(grep -o 'eg_rule_named(off, "[a-z0-9_]*")' compiler/egraph.fd | sed -E 's/.*"([a-z0-9_]*)".*/\1/' | grep -vx all)
if [ "$(echo "$FAMS" | wc -w)" -lt 10 ]; then
  fail "compiler/egraph.fd names fewer than 10 rule families: $FAMS"
fi

# 2
build() {  # build <label> [VAR=value ...]: emit the test; 0 if it built
  local label="$1"; shift
  rm -f "$OUT/$label$EXE"
  # shellcheck disable=SC2086
  env "$@" FID_VERIFY=1 FID_EGRAPH_STATS=1 "$FID" emit "$T" $TARGET -o "$OUT/$label$EXE" > "$OUT/$label.log" 2>&1
  [ -f "$OUT/$label$EXE" ]
}
runs_right() {  # runs_right <label>
  $RUNX "$OUT/$1$EXE" > "$OUT/$1.out" 2>&1 && cmp -s <(tr -d '\r' < "$OUT/$1.out") "$OUT/want.txt"
}
one() {  # one <label> [VAR=value ...]
  local label="$1"
  if build "$@" && runs_right "$label"; then
    pass "emit $label"
  else
    fail "emit $label (see $OUT/$label.log, $OUT/$label.out)"
  fi
}
one default X=1
one default2 X=1
one all_off FID_EGRAPH_OFF=all
for f in $FAMS; do
  one "off_$f" FID_EGRAPH_OFF="$f"
done

# 3, 4
# The e-graph's rewrites of a build: the "[prof] native mid-end: ..., N
# rewritten by the e-graph" line, else FID_EGRAPH_STATS's "rewritten N"
rewrites() {
  local n
  n=$(grep -oE '[0-9]+ rewritten by the e-graph' "$OUT/$1.log" 2>/dev/null | awk '{s += $1} END {print s + 0}')
  [ "$n" = 0 ] && n=$(grep -oE 'rewritten [0-9]+' "$OUT/$1.log" 2>/dev/null | awk '{s += $2} END {print s + 0}')
  echo "${n:-0}"
}
if [ -f "$OUT/default$EXE" ] && [ -f "$OUT/default2$EXE" ] && [ -f "$OUT/all_off$EXE" ]; then
  if cmp -s "$OUT/default$EXE" "$OUT/default2$EXE"; then
    pass "two default builds are the same bytes"
  else
    fail "two default builds differ (RL13): check 4 compares the stats only"
  fi
  rd=$(rewrites default); ra=$(rewrites all_off)
  if ! cmp -s "$OUT/default$EXE" "$OUT/all_off$EXE" || [ "$ra" -lt "$rd" ]; then
    pass "FID_EGRAPH_OFF=all changes the build (rewritten: $rd on, $ra off)"
  else
    fail "FID_EGRAPH_OFF=all changes nothing (same bytes, rewritten $rd on, $ra off): the switch is not read"
  fi
else
  fail "checks 3 and 4 need the default and all_off builds"
fi

# 5
if [ "${MATRIX:-1}" != 0 ]; then
  if IROPT="" NAT="" IME="" RULES="" OUT="$OUT/pm" JOBS=1 FID="$FID" \
       bash scripts/pass-matrix.sh "$PMT" "$T" > "$OUT/pm.log" 2>&1; then
    n=$(grep -c '^egraph rule .* off:' "$OUT/pm.log")
    if [ "$n" = "$(echo "$FAMS" | wc -w)" ]; then
      pass "pass-matrix round 4: $n families, none differs"
    else
      fail "pass-matrix round 4: $n family lines for $(echo "$FAMS" | wc -w) families (see $OUT/pm.log)"
    fi
  else
    fail "pass-matrix round 4 found differences (see $OUT/pm.log)"
  fi
fi
exit $bad
