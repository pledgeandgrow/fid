#!/bin/bash
# range-dom-check.sh — OP24 (competitive roadmap): facts of dominating
# blocks reach loads, and `p * p < n` is a fact on p (compiler/checks.fd
# peir_dom_narrow, peir_square_local; FID_IROPT_OFF=rangedom turns it off).
#
# Usage: tests/tooling/range-dom-check.sh      (FID=<compiler>, default
#        release/fid.exe; OUT=<dir>, default build/rangedom)
#
# Checks, one line each (PASS/FAIL), exit status the number of FAILs:
#  1. `fid explain` of test_opt_range_dom.fd: sieve's two `p * p` and its
#     `m + p` and `p + 1` are removed (sieve keeps at most the count's add);
#     with rangedom off it removes fewer;
#  2. the fail variant keeps main's add check (a store to p under the fact);
#  3. both programs emitted: the first prints its expected lines, the
#     second exits with a failure;
#  4. the block's fact set (peir_dom_vfact, peir_dom_enter, peir_mul_fits):
#     test_opt_range_facts.fd's inner_sq, sq_big and filled_scale keep no
#     check (inner_sq/sq_big keep one or more with rangedom and the range
#     set both off — the range set can prove those nested-branch cases by
#     itself; filled_scale keeps one or more with rangedom alone off), the
#     two fail variants keep their multiply, and the three programs run as
#     expected.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/rangedom}"
mkdir -p "$OUT"
T=tests/bootstrap
bad=0
check() { # check <label> <command...>: PASS when the command succeeds
  local label="$1"; shift
  if "$@" > /dev/null 2>&1; then echo "PASS $label"; else echo "FAIL $label"; bad=$((bad + 1)); fi
}
# removed <file> <fn>: the removed count of fn's heading line
removed() { sed -n "s/^fn $2: \([0-9]*\) check(s) removed, \([0-9]*\) kept.*/\1/p" "$1" | head -1; }
kept() { sed -n "s/^fn $2: \([0-9]*\) check(s) removed, \([0-9]*\) kept.*/\2/p" "$1" | head -1; }

"$FID" explain $T/test_opt_range_dom.fd sieve > "$OUT/on.txt" 2>&1
FID_IROPT_OFF=rangedom "$FID" explain $T/test_opt_range_dom.fd sieve > "$OUT/off.txt" 2>&1
on_r=$(removed "$OUT/on.txt" sieve); off_r=$(removed "$OUT/off.txt" sieve)
on_k=$(kept "$OUT/on.txt" sieve)
check "explain on: two mul checks removed" test "$(grep -c 'removed mul overflow check' "$OUT/on.txt")" -ge 2
check "explain on: add checks removed (m + p, p + 1)" test "$(grep -c 'removed add overflow check' "$OUT/on.txt")" -ge 2
# the bounds checks stay (flags.get(p) needs p < n, a relation, not a
# range); of the overflow checks only the count's add may stay
on_ko=$(grep -c 'kept [a-z]* overflow check' "$OUT/on.txt")
check "explain on: sieve keeps at most one overflow check (got $on_ko, $on_k kept in all)" test "$on_ko" -le 1
check "explain off removes fewer (on ${on_r:-?}, off ${off_r:-?})" test "${on_r:-0}" -gt "${off_r:-0}"

"$FID" explain $T/test_opt_range_dom_fail.fd main > "$OUT/fail.txt" 2>&1
check "explain fail variant: add kept" grep -q "kept add overflow check" "$OUT/fail.txt"

# The block's fact set (test_opt_range_facts.fd): facts on values reach
# dominated blocks and meet, Vec::filled bounds its length argument, and
# a multiply whose corner products fit is unchecked
for fn in inner_sq sq_big filled_scale; do
  "$FID" explain $T/test_opt_range_facts.fd $fn > "$OUT/facts_$fn.txt" 2>&1
  FID_IROPT_OFF=rangedom "$FID" explain $T/test_opt_range_facts.fd $fn > "$OUT/facts_off_$fn.txt" 2>&1
  check "facts: $fn keeps no check" test "$(kept "$OUT/facts_$fn.txt" $fn)" = 0
done
# LF8's range set also carries these nested branch refinements, so turning
# rangedom off alone no longer reintroduces inner_sq/sq_big's checks.
for fn in inner_sq sq_big; do
  FID_IROPT_OFF=rangedom,rangeset "$FID" explain $T/test_opt_range_facts.fd $fn > "$OUT/facts_offboth_$fn.txt" 2>&1
  check "facts: $fn with rangedom+rangeset off keeps one or more" test "$(kept "$OUT/facts_offboth_$fn.txt" $fn)" -ge 1
done
check "facts: filled_scale with rangedom off keeps one or more" test "$(kept "$OUT/facts_off_filled_scale.txt" filled_scale)" -ge 1
"$FID" explain $T/test_opt_range_facts_fail.fd after_join > "$OUT/facts_fail.txt" 2>&1
check "facts fail variant: mul kept after the join" grep -q "kept mul overflow check" "$OUT/facts_fail.txt"
"$FID" explain $T/test_opt_range_filled_fail.fd scale > "$OUT/filled_fail.txt" 2>&1
check "filled fail variant: mul kept after the join" grep -q "kept mul overflow check" "$OUT/filled_fail.txt"
rm -f "$OUT/facts.exe" "$OUT/facts_fail.exe" "$OUT/filled_fail.exe"
"$FID" emit $T/test_opt_range_facts.fd -o "$OUT/facts.exe" > "$OUT/facts.log" 2>&1
"$OUT/facts.exe" > "$OUT/facts.out" 2>&1
printf '998001\n0\n9223372030926249001\n0\n5005\n' > "$OUT/facts.want"
check "facts program prints its expected lines" cmp -s <(tr -d '\r' < "$OUT/facts.out") "$OUT/facts.want"
for t in facts_fail filled_fail; do
  src=$T/test_opt_range_$t.fd
  "$FID" emit $src -o "$OUT/$t.exe" > "$OUT/$t.log" 2>&1
  if [ -f "$OUT/$t.exe" ] && ! "$OUT/$t.exe" > "$OUT/$t.out" 2>&1 && ! grep -q unreachable "$OUT/$t.out"; then
    echo "PASS $t panics"
  else
    echo "FAIL $t panics"; bad=$((bad + 1))
  fi
done

rm -f "$OUT/ok.exe" "$OUT/fail.exe"
"$FID" emit $T/test_opt_range_dom.fd -o "$OUT/ok.exe" > "$OUT/ok.log" 2>&1
"$OUT/ok.exe" > "$OUT/ok.out" 2>&1
printf '168\n1229\n60\n55\n' > "$OUT/ok.want"
check "emitted program prints its expected lines" cmp -s <(tr -d '\r' < "$OUT/ok.out") "$OUT/ok.want"
"$FID" emit $T/test_opt_range_dom_fail.fd -o "$OUT/fail.exe" > "$OUT/fail.log" 2>&1
if [ -f "$OUT/fail.exe" ] && ! "$OUT/fail.exe" > "$OUT/fail.out" 2>&1 && ! grep -q unreachable "$OUT/fail.out"; then
  echo "PASS fail variant panics"
else
  echo "FAIL fail variant panics"; bad=$((bad + 1))
fi
exit $bad
