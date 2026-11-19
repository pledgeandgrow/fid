#!/bin/bash
# mutation-switch-check.sh — RL11 (llvm roadmap): the run-time mutation
# switches of the backend work, and scripts/mutation-check.sh judges them
# right.
#
# Usage: tests/tooling/mutation-switch-check.sh
#   FID=<compiler>  default release/fid.exe (release/fid off Windows)
#   TARGET=<emit flags>  default `--nat` on Windows, `--target x86_64-linux`
#           elsewhere (the machine layer, where the rules and the e-graph run)
#   OUT=<dir>       default build/mutation-switch
#   SCRIPT=0        skip check 6 (a scripts/mutation-check.sh run)
#
# Checks, one line each (PASS/FAIL), exit status the number of FAILs:
#  1. `fid test-rules` passes without a mutant.
#  2. FID_NAT_MUTATE=rule<N> makes `fid test-rules` fail on rule N alone,
#     named "MUTANT <rule>", for every rule; so does rulec<N> for every
#     `x op c` / `c op x` rule (compiler/rules.fd ru_mutate_rules).
#  3. tests/bootstrap/test_mutation_kill.fd built without a mutant prints
#     its expected lines; with FID_NAT_MUTATE=nosuch (a name nothing
#     reads) the image is the same bytes.
#  4. With rule0 (`x + 0 = 0`), rule21 (`x == x` false), rulec0
#     (`x + 1 = x`) and rulec4 (`x * 2 = x`) the program prints other lines
#     (or hangs or crashes: rulec0 also breaks every `i = i + 1` of the
#     runtime): the rules really rewrite with the broken rule.
#  5. The e-graph switch is read: with FID_NAT_MUTATE=egident (x + 1 = x)
#     the image is other bytes than the default one (compiler/egraph.fd
#     eg_mutate_mask); its output is printed for information.
#  6. scripts/mutation-check.sh MUTANTS="rulec0 nosuch" on that test:
#     rulec0 killed, nosuch inert, exit 0 without STRICT and 1 with
#     STRICT=1.
set -u
cd "$(dirname "$0")/../.." || exit 1
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; *) WIN=0 ;; esac
if [ $WIN = 1 ]; then
  FID="${FID:-release/fid.exe}"; TARGET="${TARGET:---nat}"; EXE=.exe; MT=nat
else
  FID="${FID:-release/fid}"; TARGET="${TARGET:---target x86_64-linux}"; EXE=""; MT=x86_64-linux
fi
OUT="${OUT:-build/mutation-switch}"
mkdir -p "$OUT"
export FIDES_NO_CACHE=1
T=tests/bootstrap/test_mutation_kill.fd
bad=0
pass() { echo "PASS $1"; }
fail() { echo "FAIL $1"; bad=$((bad + 1)); }

# 1
rules=$("$FID" test-rules 2>&1)
rc=$?
rules=$(echo "$rules" | tr -d '\r')
n=$(echo "$rules" | grep -cE '^(ok|FAIL) ')
if [ $rc = 0 ] && [ "$n" -gt 0 ] && ! echo "$rules" | grep -q '^FAIL '; then
  pass "1 fid test-rules: $n rules pass"
else
  fail "1 fid test-rules without a mutant: exit $rc, $(echo "$rules" | tail -1)"
fi

# 2
wrong=""
i=0
nc=0
while [ "$i" -lt "$n" ]; do
  name=$(echo "$rules" | grep -E '^(ok|FAIL) ' | sed -n "$((i + 1))p" | sed -E 's/^(ok|FAIL) +//')
  kinds="rule"
  if ! echo "$name" | grep -qE '^x [^ ]+ x( |$)'; then kinds="rule rulec"; nc=$((nc + 1)); fi
  for kind in $kinds; do
    got=$(FID_NAT_MUTATE="$kind$i" "$FID" test-rules 2>&1)
    grc=$?
    got=$(echo "$got" | tr -d '\r')
    nf=$(echo "$got" | grep -c '^FAIL ')
    if [ $grc = 0 ] || [ "$nf" != 1 ] || ! echo "$got" | grep -qF -e "FAIL  MUTANT $name"; then
      wrong="$wrong $kind$i(exit $grc, $nf failed)"
    fi
  done
  i=$((i + 1))
done
if [ "$n" -gt 0 ] && [ -z "$wrong" ]; then
  pass "2 each of $n rule<N> and $nc rulec<N> mutants fails its rule alone in fid test-rules"
else
  fail "2 mutants fid test-rules did not see:$wrong"
fi

# emit <out> [VAR=value...]: build $T
emit() {
  local o="$1"; shift
  rm -f "$o"
  env "$@" "$FID" emit "$T" $TARGET -o "$o" > "$o.log" 2>&1 && [ -s "$o" ]
}
run() { timeout 30 "$1" < /dev/null 2>&1 | head -c 100000 | tr -d '\r'; }
exp=$(sed -n '/^\/\/ Expected output:/,/^$/p' "$T" | sed -n 's|^// ||p' | sed '1d')

# 3
base="$OUT/base$EXE"
if [ $WIN = 0 ] && [ "$(uname -m)" != x86_64 ]; then
  fail "3 this check runs x86-64 images (Windows or x86-64 Linux)"
elif ! emit "$base"; then
  fail "3 build of $T failed: $(tail -1 "$base.log")"
else
  got=$(run "$base")
  if [ "$got" = "$exp" ]; then pass "3a $T prints its expected lines"; else fail "3a $T printed: $(echo "$got" | tr '\n' ' ')"; fi
  if emit "$OUT/nosuch$EXE" FID_NAT_MUTATE=nosuch && cmp -s "$base" "$OUT/nosuch$EXE"; then
    pass "3b FID_NAT_MUTATE=nosuch: same bytes"
  else
    fail "3b FID_NAT_MUTATE=nosuch changed the image (or did not build)"
  fi
  # 4
  # rule0 and rule21 leave the runtime alone: the line they change is known
  for spec in "rule0|15|" "rule21|18|" "rulec0||" "rulec4||"; do
    IFS='|' read -r mu line want <<< "$spec"
    o="$OUT/$mu$EXE"
    if ! emit "$o" FID_NAT_MUTATE="$mu"; then fail "4 $mu: build failed: $(tail -1 "$o.log")"; continue; fi
    got=$(run "$o")
    if [ "$got" = "$exp" ]; then
      fail "4 $mu: the program printed its expected lines (the broken rule did not rewrite)"
    elif [ -n "$line" ] && [ "$(echo "$got" | sed -n "${line}p")" = "$(echo "$exp" | sed -n "${line}p")" ]; then
      fail "4 $mu: line $line is still $(echo "$got" | sed -n "${line}p")"
    else
      pass "4 $mu: other output${line:+ (line $line: $(echo "$got" | sed -n "${line}p"))}"
    fi
  done
  # 5
  o="$OUT/egident$EXE"
  if emit "$o" FID_NAT_MUTATE=egident; then
    if cmp -s "$base" "$o"; then
      fail "5 FID_NAT_MUTATE=egident: same bytes (eg_mutate_mask not read?)"
    else
      pass "5 FID_NAT_MUTATE=egident: other bytes; output: $(run "$o" | tr '\n' ' ' | cut -c1-120)"
    fi
  else
    fail "5 egident: build failed: $(tail -1 "$o.log")"
  fi
fi

# 6
if [ "${SCRIPT:-1}" != 0 ]; then
  log="$OUT/mc.log"
  FID="$FID" TARGET=$MT JOBS=2 OUT="$OUT/mc" MUTANTS="rulec0 nosuch" bash scripts/mutation-check.sh "$T" > "$log" 2>&1
  rc=$?
  if [ $rc = 0 ] && grep -qE '^rulec0 +killed' "$log" && grep -qE '^nosuch +inert' "$log"; then
    pass "6a mutation-check: rulec0 killed, nosuch inert, exit 0"
  else
    fail "6a mutation-check (exit $rc): $(grep -E '^(rulec0|nosuch) ' "$log" | tr '\n' ' ') $(tail -1 "$log")"
  fi
  STRICT=1 FID="$FID" TARGET=$MT JOBS=2 OUT="$OUT/mc" MUTANTS="nosuch" bash scripts/mutation-check.sh "$T" > "$log.strict" 2>&1
  rc=$?
  if [ $rc = 1 ]; then pass "6b STRICT=1: an inert mutant fails the run"; else fail "6b STRICT=1 with an inert mutant: exit $rc"; fi
fi
echo "mutation-switch-check: $bad failed"
exit $bad
