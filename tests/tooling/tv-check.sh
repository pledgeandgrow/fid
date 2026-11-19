#!/bin/bash
# tv-check.sh — translation validation of the mid-end (compiler/tv.fd,
# competitive roadmap VF6; docs/verification/translation-validation.md).
#
# Usage: tests/tooling/tv-check.sh
#   FID=<compiler>   default release/fid.exe
#   OUT=<dir>        default build/tv
#   SOLVER=<cmd>     default z3 (the z3-solver Python wheel when there is no
#                    z3 command, as scripts/rules-smt.sh does)
#   SUITE=1          then every program of the suite with FID_VALIDATE=1
#                    (JOBS=<n> at a time, EXTRA="--nat" another backend)
#
# 1. The checks themselves: tests/tooling/tv_selftest_main.fd, built through
#    C, prints "tv selftest ok" and on stderr "tv selftest: N cases, N as
#    expected" (pairs before/after a pass each check must accept or reject:
#    fold, dce, gvn, licm, inline, gconst, icf, ipcp, devirt, an injected
#    fault, the SMT-LIB writer). The compiler runs the same cases with
#    FID_TV_SELFTEST=1: its line must say the same.
# 1b. RL5: the before/after pairs of tests/verify/tv/*.tvp (the format is
#    described above tv_pairs_text in compiler/tv.fd), through
#    tests/tooling/tv_pairs_main.fd and inside the compiler with
#    FID_TV_PAIRS=tests/verify/tv: "tv pairs: N pairs, N as expected";
#    tests/tooling/tv_pairs_wrong.tvp (wrong on purpose): 3 pairs, 0.
# 2. SMT for straight-line code: the selftest program with
#    FID_VALIDATE_SMT=<dir> writes example pairs (tv_smt_examples), each
#    saying on its first line the answer the solver must give ("expect
#    unsat": the two are the same; "expect sat": a mistake, the overflow
#    and signedness traps among them); every answer must be that one.
#    Then the obligations the compiler writes for the suite's own
#    straight-line functions while validating (FID_VALIDATE_SMT on the
#    programs of step 3): each the validator accepted must be `unsat`.
# 3. Validation on: test_ssa_validate.fd and test_tv_inject.fd (no fault)
#    build with FID_VALIDATE=1, print their expected output, and the
#    report says "N pass results checked, 0 rejected" with N > 0.
# 4. Faults put in on purpose (the negative tests): test_tv_inject.fd with
#    the FID_TV_INJECT, FID_TV_INJECT_IN of its //@ env lines prints its
#    expected output, and the compiler's stderr has at least one "injected
#    a fault ... caught" and no "NOT caught".
# 5. SUITE=1: every tests/bootstrap program that is not expected to fail,
#    built with FID_VALIDATE=1 and without: one line each,
#      SAME    same output and exit status, nothing rejected
#      REJECT  a pass result was rejected (the lines follow: each is a
#              miscompile the validator stopped, or a false alarm of it)
#      DIFF    output or exit status differ
#      BUILD   builds without validation, not with it
#    Totals of "pass results checked / rejected / not checkable" are summed.
# Exit status: the number of failed checks (+ REJECT + DIFF + BUILD).
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/tv}"
SOLVER="${SOLVER:-z3}"
SUITE="${SUITE:-0}"
JOBS="${JOBS:-1}"
EXTRA="${EXTRA:-}"
mkdir -p "$OUT"
# the build cache's key holds FID_VALIDATE but not FID_TV_INJECT,
# FID_TV_SELFTEST or FID_VALIDATE_SMT: every build here is a fresh one
export FIDES_NO_CACHE=1
bad=0
ok() { echo "OK   $*"; }
no() { echo "FAIL $*"; bad=$((bad + 1)); }

expected() {
  sed -n '/^\/\/ Expected output:/,/^[^\/]/p' "$1" | sed -n 's/^\/\/ \(.*\)$/\1/p' | grep -v '^Expected output:'
}
# the //@ env: lines of a test, as NAME=value words
envs_of() {
  sed -n 's|^//@ env: *\(.*\)$|\1|p' "$1" | tr -d '\r'
}

PY=""
for p in python3 python; do
  if command -v $p > /dev/null 2>&1 && $p -c "import z3" > /dev/null 2>&1; then PY=$p; break; fi
done
smt_py() {
  "$PY" -c "import sys, z3
s = z3.Solver(); s.set('timeout', 60000); s.from_file(sys.argv[1]); print(s.check())" "$1"
}
HAVE_SOLVER=1
if ! command -v "$SOLVER" > /dev/null 2>&1; then
  if [ -z "$PY" ]; then HAVE_SOLVER=0; else SOLVER=smt_py; fi
fi
solve() { # <file.smt2> -> sat / unsat / unknown / ...
  "$SOLVER" "$1" 2>&1 | tr -d '\r' | head -n 1
}

# 1. the self-test, as a program and inside the compiler
sel="$OUT/tv_selftest.exe"
rm -f "$sel"
if "$FID" ccompile tests/tooling/tv_selftest_main.fd -o "$sel" > "$OUT/selftest.build.log" 2>&1 && [ -f "$sel" ]; then
  rm -rf "$OUT/smt-examples"; mkdir -p "$OUT/smt-examples"
  FID_VALIDATE_SMT="$OUT/smt-examples" timeout 120 "$sel" > "$OUT/selftest.out" 2> "$OUT/selftest.err"
  line=$(tr -d '\r' < "$OUT/selftest.err" | grep -m1 '^tv selftest: [0-9]* cases')
  n=$(echo "$line" | sed -n 's/^tv selftest: \([0-9]*\) cases, \([0-9]*\) as expected.*/\1/p')
  m=$(echo "$line" | sed -n 's/^tv selftest: \([0-9]*\) cases, \([0-9]*\) as expected.*/\2/p')
  if grep -q '^tv selftest ok' <(tr -d '\r' < "$OUT/selftest.out") && [ -n "$n" ] && [ "$n" = "$m" ] && [ "$n" -ge 20 ]; then
    ok "selftest program: $line"
  else
    no "selftest program: '$line'"
    tr -d '\r' < "$OUT/selftest.err" | grep '^tv selftest: ' | sed 's/^/      /'
  fi
else
  no "selftest program does not build: $(grep -m1 -i -E 'error|panic' "$OUT/selftest.build.log")"
fi
rm -f "$OUT/hello.exe"
FID_VALIDATE=1 FID_TV_SELFTEST=1 "$FID" emit tests/bootstrap/hello.fd -o "$OUT/hello.exe" > "$OUT/hello.log" 2>&1
line=$(tr -d '\r' < "$OUT/hello.log" | grep -m1 '^tv selftest: [0-9]* cases')
n=$(echo "$line" | sed -n 's/^tv selftest: \([0-9]*\) cases, \([0-9]*\) as expected.*/\1/p')
m=$(echo "$line" | sed -n 's/^tv selftest: \([0-9]*\) cases, \([0-9]*\) as expected.*/\2/p')
if [ -n "$n" ] && [ "$n" = "$m" ]; then ok "selftest in the compiler: $line"; else no "selftest in the compiler: '$line'"; fi

# 1b. RL5: the before/after pairs of tests/verify/tv (*.tvp), as a program
#     (tests/tooling/tv_pairs_main.fd) and inside the compiler
#     (FID_TV_PAIRS): every pair as expected; and the pairs of
#     tests/tooling/tv_pairs_wrong.tvp, whose expectations are wrong on
#     purpose, all reported as not as expected
pairs_line() { # <log> -> "N M" of "tv pairs: N pairs, M as expected"
  tr -d '\r' < "$1" | sed -n 's/^tv pairs: \([0-9]*\) pairs, \([0-9]*\) as expected.*/\1 \2/p' | head -n 1
}
tvp="$OUT/tv_pairs.exe"
rm -f "$tvp"
if "$FID" ccompile tests/tooling/tv_pairs_main.fd -o "$tvp" > "$OUT/pairs.build.log" 2>&1 && [ -f "$tvp" ]; then
  timeout 120 "$tvp" tests/verify/tv > "$OUT/pairs.out" 2> "$OUT/pairs.err"
  rc=$?
  read -r pn pm <<< "$(pairs_line "$OUT/pairs.err")"
  if [ "$rc" = 0 ] && [ -n "$pn" ] && [ "$pn" = "$pm" ] && [ "$pn" -ge 50 ] && grep -q '^tv pairs ok' <(tr -d '\r' < "$OUT/pairs.out"); then
    ok "pairs program: $pn pairs of tests/verify/tv, all as expected"
  else
    no "pairs program: rc $rc, '$pn pairs, $pm as expected'"
    tr -d '\r' < "$OUT/pairs.err" | grep '^tv pairs: FAIL' | head -10 | sed 's/^/      /'
  fi
  timeout 120 "$tvp" tests/tooling/tv_pairs_wrong.tvp > /dev/null 2> "$OUT/pairs.wrong.err"
  rc=$?
  read -r wn wm <<< "$(pairs_line "$OUT/pairs.wrong.err")"
  if [ "$rc" = 3 ] && [ "$wn" = 3 ] && [ "$wm" = 0 ]; then
    ok "pairs program on wrong expectations: 3 pairs, none as expected (exit 3)"
  else
    no "pairs program on wrong expectations: rc $rc, '$wn pairs, $wm as expected' (want 3, 0, exit 3)"
  fi
else
  no "pairs program does not build: $(grep -m1 -i -E 'error|panic' "$OUT/pairs.build.log")"
fi
rm -f "$OUT/hello.pairs.exe"
FID_TV_PAIRS=tests/verify/tv "$FID" emit tests/bootstrap/hello.fd -o "$OUT/hello.pairs.exe" > "$OUT/hello.pairs.log" 2>&1
read -r cn cm <<< "$(pairs_line "$OUT/hello.pairs.log")"
if [ -n "$cn" ] && [ "$cn" = "$cm" ] && [ "$cn" -ge 50 ]; then
  ok "pairs in the compiler: $cn pairs, all as expected"
else
  no "pairs in the compiler: '$cn pairs, $cm as expected'"
  tr -d '\r' < "$OUT/hello.pairs.log" | grep '^tv pairs: FAIL' | head -10 | sed 's/^/      /'
fi

# 2a. SMT examples
if [ "$HAVE_SOLVER" = 0 ]; then
  no "no $SOLVER and no z3 Python module: SMT not checked (pip install z3-solver)"
else
  ne=0; nebad=0
  for f in "$OUT"/smt-examples/*.smt2; do
    [ -f "$f" ] || continue
    ne=$((ne + 1))
    want=$(head -n 1 "$f" | sed -n 's/.*; expect \(unsat\|sat\);.*/\1/p')
    got=$(solve "$f")
    if [ "$got" != "$want" ]; then
      nebad=$((nebad + 1))
      echo "      $got (want $want): $(head -n 1 "$f")"
    fi
  done
  if [ "$ne" -ge 9 ] && [ "$nebad" = 0 ]; then ok "SMT examples: $ne, every answer as expected"; else no "SMT examples: $ne written, $nebad not as expected"; fi
fi

# 3. validation on, no fault: expected output, nothing rejected
for t in tests/bootstrap/test_ssa_validate.fd tests/bootstrap/test_tv_inject.fd; do
  nm=$(basename "$t" .fd)
  exe="$OUT/$nm.valid.exe"
  rm -f "$exe"
  rm -rf "$OUT/smt-$nm"; mkdir -p "$OUT/smt-$nm"
  envs=(FID_VALIDATE=1 "FID_VALIDATE_SMT=$OUT/smt-$nm")
  for e in $(envs_of "$t"); do
    case "$e" in FID_TV_INJECT*|FID_VALIDATE=*) ;; *) envs+=("$e") ;; esac
  done
  env "${envs[@]}" "$FID" emit "$t" $EXTRA -o "$exe" > "$exe.log" 2>&1 < /dev/null
  if [ ! -f "$exe" ]; then no "$nm: does not build with validation on: $(grep -m1 -i -E 'error|panic' "$exe.log")"; continue; fi
  timeout 60 "$exe" > "$exe.out" 2>&1 < /dev/null
  rep=$(tr -d '\r' < "$exe.log" | grep -m1 '^tv: [0-9]* pass results checked')
  chk=$(echo "$rep" | sed -n 's/^tv: \([0-9]*\) pass results checked, \([0-9]*\) rejected.*/\1/p')
  rej=$(echo "$rep" | sed -n 's/^tv: \([0-9]*\) pass results checked, \([0-9]*\) rejected.*/\2/p')
  if ! cmp -s <(expected "$t") <(tr -d '\r' < "$exe.out"); then
    no "$nm: not the expected output with validation on"
    diff <(expected "$t") <(tr -d '\r' < "$exe.out") | head -6 | sed 's/^/      /'
  elif [ "${chk:-0}" -gt 0 ] && [ "${rej:-1}" = 0 ]; then
    ok "$nm: expected output; $rep"
  else
    no "$nm: report '$rep' (want N > 0 checked, 0 rejected)"
    tr -d '\r' < "$exe.log" | grep 'rejected:' | head -5 | sed 's/^/      /'
  fi
done

# 2b. the compiler's own obligations: every one the validator accepted is unsat
if [ "$HAVE_SOLVER" = 1 ]; then
  no_=0; nobad=0
  for f in "$OUT"/smt-test_*/*.smt2; do
    [ -f "$f" ] || continue
    head -n 1 "$f" | grep -q 'validator accepted' || continue
    no_=$((no_ + 1))
    got=$(solve "$f")
    if [ "$got" != unsat ]; then
      nobad=$((nobad + 1))
      echo "      $got: $(head -n 1 "$f") ($f)"
    fi
  done
  if [ "$nobad" = 0 ]; then ok "SMT obligations of the tests: $no_ accepted pairs, all proved"; else no "SMT obligations of the tests: $no_ accepted pairs, $nobad not proved"; fi
fi

# 4. faults put in on purpose
t=tests/bootstrap/test_tv_inject.fd
exe="$OUT/test_tv_inject.inj.exe"
rm -f "$exe"
envs=()
for e in $(envs_of "$t"); do envs+=("$e"); done
env "${envs[@]}" "$FID" emit "$t" $EXTRA -o "$exe" > "$exe.log" 2>&1 < /dev/null
if [ ! -f "$exe" ]; then
  no "test_tv_inject (faults): does not build: $(grep -m1 -i -E 'error|panic' "$exe.log")"
else
  timeout 60 "$exe" > "$exe.out" 2>&1 < /dev/null
  rc=$?
  caught=$(tr -d '\r' < "$exe.log" | grep -c 'injected a fault into .*: caught')
  missed=$(tr -d '\r' < "$exe.log" | grep -c 'NOT caught')
  if [ "$rc" = 0 ] && cmp -s <(expected "$t") <(tr -d '\r' < "$exe.out") && [ "$caught" -gt 0 ] && [ "$missed" = 0 ]; then
    ok "test_tv_inject (faults): expected output, $caught faults caught, none missed"
  else
    no "test_tv_inject (faults): rc $rc, $caught caught, $missed not caught"
    tr -d '\r' < "$exe.log" | grep 'NOT caught' | head -8 | sed 's/^/      /'
    diff <(expected "$t") <(tr -d '\r' < "$exe.out") | head -6 | sed 's/^/      /'
  fi
fi

# 5. the suite with validation on
if [ "$SUITE" = 1 ]; then
  TESTS=()
  for t in tests/bootstrap/*.fd; do
    case "$t" in *_fail.fd) continue ;; esac
    grep -q '^//! error' "$t" && continue
    grep -q -E 'sqlite3|SDL_|sdl2_h|glClear|use gl;|use sdl' "$t" && continue
    TESTS+=("$t")
  done
  mkdir -p "$OUT/suite"
  one() {
    local t="$1" n m exe rcs=()
    n=$(basename "$t" .fd)
    for m in off on; do
      exe="$OUT/suite/$n.$m.exe"
      rm -f "$exe" "$exe.out"
      if [ $m = on ]; then
        env $(sed -n 's|^//@ env: *\(.*\)$|\1|p' "$t" | tr -d '\r' | grep -v '^FID_TV_INJECT') FID_VALIDATE=1 "$FID" emit "$t" $EXTRA -o "$exe" > "$exe.log" 2>&1 < /dev/null
      else
        env $(sed -n 's|^//@ env: *\(.*\)$|\1|p' "$t" | tr -d '\r' | grep -v '^FID_TV_INJECT\|^FID_VALIDATE') "$FID" emit "$t" $EXTRA -o "$exe" > "$exe.log" 2>&1 < /dev/null
      fi
      if [ -f "$exe" ]; then
        timeout 60 "$exe" > "$exe.out" 2>&1 < /dev/null
        rcs+=("$?")
      else
        rcs+=("BUILD")
      fi
    done
    local a="${rcs[0]}" b="${rcs[1]}" line
    local on="$OUT/suite/$n.on.exe"
    if [ "$a" = BUILD ] && [ "$b" = BUILD ]; then
      line="SKIP   $n (builds with neither)"
    elif [ "$b" = BUILD ]; then
      line="BUILD  $n ($(grep -m1 -i -E 'error|panic' "$on.log"))"
    elif grep -q 'rejected:' "$on.log"; then
      line="REJECT $n
$(tr -d '\r' < "$on.log" | grep 'rejected:' | head -5 | sed 's/^/       /')"
    elif [ "$a" = "$b" ] && cmp -s <(tr -d '\r' < "$OUT/suite/$n.off.exe.out") <(tr -d '\r' < "$on.out"); then
      line="SAME   $n (rc $a)"
    else
      line="DIFF   $n (rc off $a, on $b)
$(diff "$OUT/suite/$n.off.exe.out" "$on.out" | head -6 | sed 's/^/       /')"
    fi
    echo "$line" > "$OUT/suite/$n.res"
  }
  export -f one
  export FID OUT EXTRA
  printf '%s\n' "${TESTS[@]}" | xargs -P "$JOBS" -I{} bash -c 'one "$1"' _ {}
  : > "$OUT/suite/results.txt"
  tc=0; tr_=0; tu=0
  for t in "${TESTS[@]}"; do
    n=$(basename "$t" .fd)
    [ -f "$OUT/suite/$n.res" ] && cat "$OUT/suite/$n.res" >> "$OUT/suite/results.txt"
    rep=$(tr -d '\r' < "$OUT/suite/$n.on.exe.log" 2>/dev/null | grep -m1 '^tv: [0-9]* pass results checked')
    read -r c r u <<< "$(echo "$rep" | sed -n 's/^tv: \([0-9]*\) pass results checked, \([0-9]*\) rejected, \([0-9]*\) not checkable.*/\1 \2 \3/p')"
    tc=$((tc + ${c:-0})); tr_=$((tr_ + ${r:-0})); tu=$((tu + ${u:-0}))
  done
  cat "$OUT/suite/results.txt"
  sb=$(grep -c -E '^(REJECT|DIFF|BUILD)' "$OUT/suite/results.txt")
  same=$(grep -c '^SAME' "$OUT/suite/results.txt")
  echo "tv-check: suite: ${#TESTS[@]} programs, $same the same with nothing rejected, $sb rejected, differ or fail to build"
  echo "tv-check: suite: $tc pass results checked, $tr_ rejected, $tu not checkable"
  bad=$((bad + sb))
fi

echo "tv-check: $bad failed"
exit $bad
