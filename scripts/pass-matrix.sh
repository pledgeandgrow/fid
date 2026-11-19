#!/bin/bash
# pass-matrix.sh — every optimization switch off in turn must give the same
# results (own-backend roadmap RL3).
#
# Usage: scripts/pass-matrix.sh <target> [test.fd ...]
#   target: x86_64-linux | aarch64-linux | riscv64-linux | x86_64-windows
#           (the native backend: scripts/cross-test.sh with NATIVE=1;
#           x86_64-windows is `emit --nat`)
#   no test files: the whole suite, as cross-test.sh picks it
#   FID, JOBS, WSL_DISTRO as for scripts/cross-test.sh
#   IROPT="a b" NAT="a b" IME="a b": the switches to try. Default: every
#           switch the compiler's source reads (scripts/switch-check.sh
#           --list iropt|nat|ime), so a new pass is in the matrix at once.
#           IME="" skips round 3.
#   EGRAPH="a b": the e-graph's rule families for round 4 (default: every
#           name eg_rules_off_mask in compiler/egraph.fd reads; "" skips it)
#   RULES="0 1 ..": the rules of compiler/rules.fd for round 5, by index in
#           ru_rules (default: all of them, when rules.fd reads
#           FID_RULES_OFF; "" skips it)
#   OUT=<dir>   default build/pm/<target>
#   RESUME=1    keep the runs of an earlier, interrupted matrix in the same
#           OUT (each run's summary is $OUT/<tag>.line): only the missing
#           ones are run. Use the same FID and tests, or the comparison
#           mixes compilers.
#   SELFTEST=1  no compiler, no target: checks the comparison itself on
#           made-up result folders (tests/tooling/pass-matrix-check.sh)
#
# Rounds (docs/verification/kill-switches.md has every switch):
#   1. each default-on mid-end pass off   FID_IROPT_OFF=<pass>, against all on
#   2. each backend pass and rule group   FID_NAT_OFF=<name>,   against all on
#   3. the opt-in mid-end: FID_IROPT_ON=all, then each of its passes off
#      (FID_IROPT_ON=all FID_IROPT_OFF=<pass>), against FID_IROPT_ON=all;
#      the FID_IROPT_ON=all run is also compared with all on
#   4. each rule family of the e-graph     FID_EGRAPH_OFF=<family>, against all on
#   5. each rewrite rule of rules.fd       FID_RULES_OFF=<index>,  against all on
#
# Besides the verdicts, the output and exit code of every test that built in
# both runs are compared (`<test>.actual`, `.actual_rc` of cross-test.sh): a
# test failing with and without the switch, but printing something else,
# differs too ("output:" lines).
#
# Each run's results.txt (one PASS/FAIL/BUILD/SKIP line per test, the
# output and exit code compared with the interpreter's) is compared with
# its base run test by test, not only by its count: a test that passes with
# the switch and fails without it, and one that fails the other way, are
# both listed. A summary is in $OUT/matrix.txt.
#
# Exit status: the number of switches whose results differ from their base.
set -u
cd "$(dirname "$0")/.." || exit 1
TARGET="${1:-}"
if [ "${SELFTEST:-}" = 1 ]; then
  TARGET=selftest
  OUT=$(mktemp -d)
  trap 'rm -rf "$OUT"' EXIT
elif [ -z "$TARGET" ]; then
  sed -n '2,/^set -u/p' "$0" | sed '$d'; exit 1
fi
[ $# -gt 0 ] && shift

# The e-graph's rule families (FID_EGRAPH_OFF names read by
# eg_rules_off_mask) and the rules of rules.fd (indexes into ru_rules, when
# rules.fd reads FID_RULES_OFF), from the source like scripts/switch-check.sh
egraph_families() {
  grep -o 'eg_rule_named(off, "[a-z0-9_]*")' compiler/egraph.fd 2>/dev/null \
    | sed -E 's/.*"([a-z0-9_]*)".*/\1/' | grep -vx all | tr '\n' ' ' | sed 's/ $//'
}
rule_indexes() {
  grep -q 'FID_RULES_OFF' compiler/rules.fd 2>/dev/null || return 0
  local n
  n=$(sed -n '/^pub fn ru_rules()/,/^}/p' compiler/rules.fd | grep -c 'v\.push(ru(')
  [ "$n" -gt 0 ] && seq 0 $((n - 1)) | tr '\n' ' ' | sed 's/ $//'
}
if [ "$TARGET" != selftest ]; then
  IROPT="${IROPT-$(bash scripts/switch-check.sh --list iropt)}"
  NAT="${NAT-$(bash scripts/switch-check.sh --list nat)}"
  IME="${IME-$(bash scripts/switch-check.sh --list ime)}"
  EGRAPH="${EGRAPH-$(egraph_families)}"
  RULES="${RULES-$(rule_indexes)}"
  OUT="${OUT:-build/pm/$TARGET}"
fi
mkdir -p "$OUT"
SUM="$OUT/matrix.txt"
: > "$SUM"
differ=0

# run <tag> [VAR=value ...]: the suite with those variables; prints the
# summary line of cross-test.sh. Results in $OUT/<tag>-native/results.txt.
run() {
  local tag="$1"; shift
  # RESUME=1: a run that finished before (its summary line saved, its
  # results.txt there) is not run again, so the whole suite on every switch
  # (hours per target) can be done in several sittings
  if [ "${RESUME:-}" = 1 ] && [ -s "$OUT/$tag.line" ] && [ -s "$OUT/$tag-native/results.txt" ]; then
    cat "$OUT/$tag.line"; return
  fi
  rm -f "$OUT/$tag.line"
  env "$@" NATIVE=1 OUT="$OUT/$tag" bash scripts/cross-test.sh "$TARGET" "${TESTS[@]+"${TESTS[@]}"}" 2>&1 \
    | grep -E "passed," | tail -n 1 | tee "$OUT/$tag.line"
}

# One line per test, sorted, without the details after the test's name
# that change from run to run (none today; kept so a run with a different
# rc text still compares by verdict and name)
results() { sed -E 's/^([A-Z]+) +([^ ]+).*/\1 \2/' "$OUT/$1-native/results.txt" 2>/dev/null | sort; }

# outputs <base tag> <tag>: "output: <test>" for each test that ran in both
# runs (PASS or FAIL in both results.txt) whose exit code or standard output
# differs between them. A verdict that changed is already in `results`; this
# finds a test that fails both ways but prints something else.
outputs() {
  local a="$OUT/$1-native" b="$OUT/$2-native" t
  for t in $(comm -12 <(results "$1" | grep -E '^(PASS|FAIL) ' | cut -d' ' -f2 | sort -u) \
                      <(results "$2" | grep -E '^(PASS|FAIL) ' | cut -d' ' -f2 | sort -u)); do
    if [ "$(tr -d '\r' < "$a/$t.actual_rc" 2>/dev/null)" != "$(tr -d '\r' < "$b/$t.actual_rc" 2>/dev/null)" ] \
       || ! cmp -s <(tr -d '\r' < "$a/$t.actual" 2>/dev/null) <(tr -d '\r' < "$b/$t.actual" 2>/dev/null); then
      echo "output: $t"
    fi
  done
}

# compare <label> <base tag> <tag> <summary line>
compare() {
  local label="$1" base="$2" tag="$3" line="$4" d o
  d=$(diff <(results "$base") <(results "$tag") | grep -E '^[<>]' || true)
  o=$(outputs "$base" "$tag")
  [ -n "$o" ] && d="${d:+$d
}$o"
  if [ -z "$(results "$tag")" ]; then
    printf '%-26s %s   <- no results\n' "$label" "$line" | tee -a "$SUM"
    differ=$((differ + 1))
  elif [ -n "$d" ]; then
    printf '%-26s %s   <- differs\n' "$label" "$line" | tee -a "$SUM"
    # "< PASS x" with the switch on became "> FAIL x" with it off, and so on
    echo "$d" | sed -E 's/^< /      on:  /; s/^> /      off: /; s/^output: /      output differs: /' | head -40 | tee -a "$SUM"
    differ=$((differ + 1))
  else
    printf '%-26s %s\n' "$label" "$line" | tee -a "$SUM"
  fi
}

if [ "$TARGET" = selftest ]; then
  # Made-up runs: base "all" (a, b pass; c fails printing 1), then one run
  # per case. Each must count as differing, or not, as its name says.
  mk() {  # mk <tag> <results lines> then "test rc output" triples
    local d="$OUT/$1-native"; mkdir -p "$d"; printf '%b' "$2" > "$d/results.txt"; shift 2
    while [ $# -ge 3 ]; do
      printf '%s\n' "$2" > "$d/$1.actual_rc"; printf '%b' "$3" > "$d/$1.actual"; shift 3
    done
  }
  ok3='PASS  a\nPASS  b\nFAIL  c (rc 0, interpreter 0)\n'
  mk all "$ok3" a 0 'x\n' b 0 'y\n' c 0 '1\n'
  mk same "$ok3" a 0 'x\r\n' b 0 'y\n' c 0 '1\n'         # CRLF only: the same
  mk verdict 'PASS  a\nFAIL  b (rc 1, interpreter 0)\nFAIL  c (rc 0, interpreter 0)\n' \
     a 0 'x\n' b 1 '' c 0 '1\n'
  mk output "$ok3" a 0 'x\n' b 0 'y\n' c 0 '2\n'          # c fails both ways, prints 2
  mk rc "$ok3" a 0 'x\n' b 0 'y\n' c 3 '1\n'              # c's exit code changed
  mk build 'PASS  a\nBUILD b\nFAIL  c (rc 0, interpreter 0)\n' a 0 'x\n' c 0 '1\n'
  bad=0
  for want in same:0 verdict:1 output:1 rc:1 build:1 none:1; do
    t="${want%%:*}"; w="${want#*:}"
    differ=0
    compare "selftest $t:" all "$t" "(made up)" > /dev/null
    if [ "$differ" != "$w" ]; then
      echo "pass-matrix selftest: case $t counted $differ differences, want $w"; bad=$((bad + 1))
    fi
  done
  # The source lists: every family the e-graph reads, with no duplicates
  fams=$(egraph_families)
  if [ -z "$fams" ] || [ "$(echo "$fams" | tr ' ' '\n' | sort | uniq -d)" != "" ]; then
    echo "pass-matrix selftest: no e-graph rule families found in compiler/egraph.fd ($fams)"; bad=$((bad + 1))
  fi
  # RESUME=1: a run that finished (summary line and results kept) is not
  # run again; without RESUME it is (here: cross-test.sh refuses the
  # made-up target, so the summary comes back empty)
  mkdir -p "$OUT/res-native"
  printf 'PASS  a\n' > "$OUT/res-native/results.txt"
  echo 'selftest: 1 passed, 0 failed' > "$OUT/res.line"
  if [ "$(RESUME=1 run res)" != "selftest: 1 passed, 0 failed" ]; then
    echo "pass-matrix selftest: RESUME=1 ran a finished run again"; bad=$((bad + 1))
  fi
  TESTS=()
  if [ -n "$(RESUME= run res)" ]; then
    echo "pass-matrix selftest: without RESUME a finished run was not run again"; bad=$((bad + 1))
  fi
  [ $bad = 0 ] && echo "pass-matrix selftest: ok (6 cases; e-graph families: $fams; rules.fd rules: $(rule_indexes | wc -w))"
  exit $bad
fi

TESTS=("$@")
echo "pass matrix on $TARGET: ${#TESTS[@]} files given (0: the whole suite)" | tee -a "$SUM"
line=$(run all)
printf '%-26s %s\n' "all on:" "$line" | tee -a "$SUM"

for p in $IROPT; do
  compare "iropt $p off:" all "iropt-$p" "$(run "iropt-$p" FID_IROPT_OFF="$p")"
done
for p in $NAT; do
  compare "nat $p off:" all "nat-$p" "$(run "nat-$p" FID_NAT_OFF="$p")"
done
if [ -n "$IME" ]; then
  compare "opt-in all on:" all ime-all "$(run ime-all FID_IROPT_ON=all)"
  for p in $IME; do
    compare "opt-in $p off:" ime-all "ime-$p" "$(run "ime-$p" FID_IROPT_ON=all FID_IROPT_OFF="$p")"
  done
fi
for p in $EGRAPH; do
  compare "egraph rule $p off:" all "eg-$p" "$(run "eg-$p" FID_EGRAPH_OFF="$p")"
done
for p in $RULES; do
  compare "rules.fd rule $p off:" all "rule-$p" "$(run "rule-$p" FID_RULES_OFF="$p")"
done
echo "pass matrix on $TARGET: $differ switches differ from their base (summary: $SUM)" | tee -a "$SUM"
exit $differ
