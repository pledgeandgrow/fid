#!/bin/bash
# rl3-rules-off-check.sh — RL3 (llvm-roadmap: the pass on/off matrix), the
# single-rule switches the matrix's rounds 4 and 5 use. One line per check
# (OK / FAIL / SKIP); the exit status is the number of FAILs.
#
# Source (no compiler):
#  1. compiler/rules.fd reads FID_RULES_OFF, folds it into the rules' memo
#     key (CS1: a memo entry made with every rule on must not be replayed
#     with one off), and nat_ir's cache key (nc_start) holds it and
#     FID_EGRAPH_OFF.
#  2. docs/verification/kill-switches.md documents FID_RULES_OFF and every
#     e-graph family compiler/egraph.fd reads (`FID_EGRAPH_OFF=<family>`).
#  3. scripts/pass-matrix.sh SELFTEST=1, and the number of rules its round 5
#     tries is the number of rules in ru_rules.
# With FID=<compiler> (the machine layer: `emit --nat` on Windows,
# `--target x86_64-linux` on an x86-64 Linux host; elsewhere TARGET=<target>
# with BUILD_ONLY behaviour, no run):
#  4. `fid test-rules` lists as many rules as round 5 tries, none failed.
#  5. tests/bootstrap/test_verify_rl3_rule_identities.fd with
#     FID_RULE_STATS=1 and every rule on: each rule fires (> 0 hits), and the
#     program prints its Expected output.
#  6. For each rule index i (RULES="0 1 .." to choose; default all):
#     FID_RULES_OFF=i gives rule i 0 hits, every other rule still fires, and
#     the output is the same. Then FID_RULES_OFF=all: every rule 0 hits,
#     same output.
#  7. The memo (FID_MECACHE, a fresh file): with FID_NAT_OFF=egraph (so the
#     e-graph does not redo the rules' work), a build with
#     FID_RULES_OFF=all after one with every rule on, sharing the memo,
#     gives the same executable as FID_NAT_OFF=egraph,rules and not the
#     all-on one (a memo key without FID_RULES_OFF replays the rewrites).
#  8. Each e-graph family off (FID_EGRAPH_OFF=<family>, and `all`): the
#     program's output is the same.
#
# Usage: tests/tooling/rl3-rules-off-check.sh
#   FID=<compiler>, OUT=<dir> (build/rl3-rules-off), RULES="..", TARGET=..
set -u
cd "$(dirname "$0")/../.." || exit 1
OUT="${OUT:-build/rl3-rules-off}"
mkdir -p "$OUT"
P=tests/bootstrap/test_verify_rl3_rule_identities.fd
DOC=docs/verification/kill-switches.md
bad=0
ok() { echo "OK    $1"; }
no() { echo "FAIL  $1"; bad=$((bad + 1)); }
skip() { echo "SKIP  $1"; }

# 1
if grep -q 'p_getenv("FID_RULES_OFF")\|env_or_empty("FID_RULES_OFF")' compiler/rules.fd; then
  ok "rules.fd reads FID_RULES_OFF"
else
  no "rules.fd does not read FID_RULES_OFF (round 5 of scripts/pass-matrix.sh is skipped)"
fi
if sed -n '/^pub fn rules_run(/,/^}/p' compiler/rules.fd | grep -q 'cfg_h = .*FID_RULES_OFF'; then
  ok "the rules' memo key holds FID_RULES_OFF"
else
  no "rules_run's memo key (cfg_h) does not hold FID_RULES_OFF: an all-on memo entry would be replayed"
fi
for v in FID_RULES_OFF FID_EGRAPH_OFF; do
  if grep -q "env_or_empty(\"$v\")" compiler/nat_ir.fd; then ok "nat_ir's cache key holds $v"; else no "nat_ir's cache key lacks $v"; fi
done

# 2
if grep -q '`FID_RULES_OFF=' "$DOC"; then ok "$DOC documents FID_RULES_OFF"; else no "$DOC does not document FID_RULES_OFF"; fi
fams=$(grep -o 'eg_rule_named(off, "[a-z0-9_]*")' compiler/egraph.fd | sed -E 's/.*"([a-z0-9_]*)".*/\1/' | grep -vx all | sort -u)
miss=""
for f in $fams; do grep -qF "\`FID_EGRAPH_OFF=$f\`" "$DOC" || miss="$miss $f"; done
if [ -z "$fams" ]; then
  no "no e-graph rule families found in compiler/egraph.fd"
elif [ -z "$miss" ]; then
  ok "$DOC documents all $(echo "$fams" | wc -w) e-graph families"
else
  no "$DOC lacks FID_EGRAPH_OFF=<family> for:$miss"
fi
# and names no family egraph.fd does not read
extra=""
for f in $(grep -oE '`FID_EGRAPH_OFF=[a-z0-9_]+`' "$DOC" | sed -E 's/`FID_EGRAPH_OFF=([a-z0-9_]+)`/\1/' | sort -u); do
  [ "$f" = all ] && continue
  echo "$fams" | grep -qx "$f" || extra="$extra $f"
done
if [ -n "$extra" ]; then no "$DOC documents families egraph.fd does not read:$extra"; fi

# 3
r=$(SELFTEST=1 bash scripts/pass-matrix.sh 2>&1); st=$?
if [ $st = 0 ]; then ok "pass-matrix SELFTEST: $r"; else no "pass-matrix SELFTEST: $r"; fi
nrules=$(sed -n '/^pub fn ru_rules()/,/^}/p' compiler/rules.fd | grep -c 'v\.push(ru(')
round5=$(echo "$r" | sed -n 's/.*rules\.fd rules: \([0-9]*\).*/\1/p')
if [ "$round5" = "$nrules" ] && [ "$nrules" -gt 0 ]; then
  ok "round 5 tries all $nrules rules of ru_rules"
else
  no "round 5 tries '${round5:-?}' rules, ru_rules has $nrules"
fi
for py in python3 python; do
  if command -v "$py" > /dev/null 2>&1 && "$py" -c 'import sys; sys.exit(sys.version_info[0] < 3)' 2> /dev/null; then
    r=$("$py" scripts/verify/pass-matrix-report.py --selftest 2>&1)
    if [ "$r" = "pass-matrix-report selftest: ok" ]; then ok "$r"; else no "$r"; fi
    break
  fi
done
if bash -n scripts/verify/rl3-matrix-all.sh; then ok "scripts/verify/rl3-matrix-all.sh parses"; else no "scripts/verify/rl3-matrix-all.sh does not parse"; fi

if [ -z "${FID:-}" ]; then
  skip "FID not set: checks 4-8 (the compiler) not run"
  echo "rl3-rules-off-check: $bad failed"
  exit $bad
fi

# How to build for the machine layer here, and whether the result runs
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; Darwin) HOST=macos ;; *) HOST=linux ;; esac
RUN=1
if [ -n "${TARGET:-}" ]; then
  EMIT=(--target "$TARGET"); RUN=0
elif [ "$HOST" = windows ]; then
  EMIT=(--nat)
elif [ "$HOST" = linux ] && [ "$(uname -m)" = x86_64 ]; then
  EMIT=(--target x86_64-linux)
elif [ "$HOST" = linux ] && [ "$(uname -m)" = aarch64 ]; then
  EMIT=(--target aarch64-linux)
else
  EMIT=(--target x86_64-linux); RUN=0
fi
X=bin; [ "$HOST" = windows ] && [ -z "${TARGET:-}" ] && X=exe

want() {  # the "Expected output:" lines of the test's header
  awk '/^\/\/ Expected output:/ {on=1; next} on && /^\/\/ / {sub(/^\/\/ /, ""); print; next} on {exit}' "$1"
}
want "$P" > "$OUT/want.txt"

# build <tag> <env...>: 0 when built; the log is $OUT/<tag>.log
build() {
  local tag="$1"; shift
  rm -f "$OUT/$tag.$X"
  env "$@" "$FID" emit "$P" "${EMIT[@]}" -o "$OUT/$tag.$X" > "$OUT/$tag.log" 2>&1 && [ -f "$OUT/$tag.$X" ]
}
# same_output <tag>: the executable prints the Expected output (or RUN=0)
same_output() {
  [ "$RUN" = 1 ] || return 0
  "$OUT/$1.$X" < /dev/null 2> /dev/null | tr -d '\r' > "$OUT/$1.out"
  cmp -s "$OUT/want.txt" "$OUT/$1.out"
}
# hits <tag>: one count per "[rules] <rule>: N" line, in ru_rules' order
hits() { tr -d '\r' < "$OUT/$1.log" | grep '^\[rules\] ' | grep -v '^\[rules\] readcse:' | sed -E 's/.*: ([0-9]+)$/\1/'; }

# 4
tr=$("$FID" test-rules 2>&1 | tr -d '\r' | tail -n 1)
if [ "$tr" = "$nrules rules, 0 failed" ]; then ok "fid test-rules: $tr"; else no "fid test-rules: '$tr', want '$nrules rules, 0 failed'"; fi

# 5
if build on FID_RULE_STATS=1; then
  base=$(hits on)
  n=$(echo "$base" | grep -c .)
  zero=$(echo "$base" | grep -cx 0)
  if [ "$n" = "$nrules" ] && [ "$zero" = 0 ]; then
    ok "every rule on: all $n rules fire"
  else
    no "every rule on: $n rule lines (want $nrules), $zero with 0 hits (want 0)"
  fi
  if same_output on; then ok "every rule on: the Expected output"; else no "every rule on: output differs ($OUT/on.out)"; fi
else
  no "the program did not build ($OUT/on.log)"; base=""
fi

# 6
if [ -n "$base" ]; then
  RULES="${RULES-$(seq 0 $((nrules - 1)))}"
  for i in $RULES all; do
    if ! build "off-$i" FID_RULE_STATS=1 FID_RULES_OFF="$i"; then
      no "FID_RULES_OFF=$i: did not build ($OUT/off-$i.log)"; continue
    fi
    h=$(hits "off-$i")
    # the rule (or every rule) at 0, every other one still firing
    got=$(echo "$h" | awk -v i="$i" '
      { r = NR - 1; if (i == "all" || r == i) { if ($1 != 0) bad = bad " rule " r " still fires (" $1 ")" }
        else if ($1 == 0) bad = bad " rule " r " stopped firing" }
      END { print bad }')
    nl=$(echo "$h" | grep -c .)
    if [ "$nl" != "$nrules" ]; then
      no "FID_RULES_OFF=$i: $nl rule lines, want $nrules"
    elif [ -n "$got" ]; then
      no "FID_RULES_OFF=$i:$got"
    elif ! same_output "off-$i"; then
      no "FID_RULES_OFF=$i: output differs ($OUT/off-$i.out)"
    else
      ok "FID_RULES_OFF=$i: off alone, same output"
    fi
  done
fi

# 7
M="$OUT/memo.bin"
rm -f "$M"
if build memo-on FID_MECACHE="$M" FID_MECACHE_STATS=1 FID_NAT_OFF=egraph \
   && build memo-off FID_MECACHE="$M" FID_MECACHE_STATS=1 FID_NAT_OFF=egraph FID_RULES_OFF=all \
   && build memo-none FID_MECACHE=off FID_NAT_OFF=egraph,rules \
   && build memo-on2 FID_MECACHE=off FID_NAT_OFF=egraph; then
  if ! cmp -s "$OUT/memo-on.$X" "$OUT/memo-on2.$X"; then
    skip "memo: two builds with the same switches differ (not reproducible here): not compared"
  elif ! grep -q '^\[mecache\] rules:' "$OUT/memo-on.log"; then
    skip "memo: no [mecache] rules line (the memo is off in this build)"
  elif cmp -s "$OUT/memo-off.$X" "$OUT/memo-on.$X"; then
    no "memo: FID_RULES_OFF=all after an all-on build gave the all-on executable (the memo key lacks FID_RULES_OFF?)"
  elif cmp -s "$OUT/memo-off.$X" "$OUT/memo-none.$X"; then
    ok "memo: FID_RULES_OFF=all after an all-on build = the rules pass off"
  else
    no "memo: FID_RULES_OFF=all differs both from all on and from FID_NAT_OFF=rules"
  fi
else
  no "memo: a build failed ($OUT/memo-*.log)"
fi

# 8
for f in $fams all; do
  if ! build "eg-$f" FID_EGRAPH_OFF="$f"; then
    no "FID_EGRAPH_OFF=$f: did not build ($OUT/eg-$f.log)"
  elif same_output "eg-$f"; then
    ok "FID_EGRAPH_OFF=$f: same output"
  else
    no "FID_EGRAPH_OFF=$f: output differs ($OUT/eg-$f.out)"
  fi
done
echo "rl3-rules-off-check: $bad failed"
exit $bad
