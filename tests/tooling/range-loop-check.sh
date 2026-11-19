#!/bin/bash
# range-loop-check.sh — LF8 (llvm-roadmap): facts over a loop's back edge.
# A loop header gets an assumed fact about one local, kept only when every
# way in and every store in the loop's body stays inside it
# (compiler/checks.fd peir_lf_update; FID_IROPT_OFF=rangeloop turns it off).
#
# Usage: tests/tooling/range-loop-check.sh   (FID=<compiler>, default
#        release/fid.exe; OUT=<dir>, default build/rangeloop)
#
# Checks, one line each (PASS/FAIL), exit status the number of FAILs:
#  1. `fid explain` of test_midend_range_loop.fd: in `mix`, `j * 1000` and
#     `j + 7` lose their checks; in `walk`, `x * 100000`; with rangeloop
#     off each function removes fewer;
#  2. the fail variant keeps mix's multiply check;
#  3. both programs emitted: the first prints its expected lines, the
#     second exits with a failure; the first prints the same with
#     rangeloop off.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/rangeloop}"
mkdir -p "$OUT"
T=tests/bootstrap
bad=0
check() { # check <label> <command...>: PASS when the command succeeds
  local label="$1"; shift
  if "$@" > /dev/null 2>&1; then echo "PASS $label"; else echo "FAIL $label"; bad=$((bad + 1)); fi
}
removed() { sed -n "s/^fn $2: \([0-9]*\) check(s) removed, \([0-9]*\) kept.*/\1/p" "$1" | head -1; }
# section <file> <fn>: the lines of fn's part of an explain report
section() { awk -v h="fn $2:" 'index($0, h) == 1 { on = 1; print; next } /^fn [A-Za-z_0-9]*:/ { on = 0 } on' "$1"; }

for fn in mix walk; do
  "$FID" explain $T/test_midend_range_loop.fd "$fn" > "$OUT/on.$fn.txt" 2>&1
  FID_IROPT_OFF=rangeloop "$FID" explain $T/test_midend_range_loop.fd "$fn" > "$OUT/off.$fn.txt" 2>&1
  on_r=$(removed "$OUT/on.$fn.txt" "$fn"); off_r=$(removed "$OUT/off.$fn.txt" "$fn")
  check "explain $fn: rangeloop off removes fewer (on ${on_r:-?}, off ${off_r:-?})" test "${on_r:-0}" -gt "${off_r:-0}"
done
section "$OUT/on.mix.txt" mix > "$OUT/on.mix.sec"
check "explain mix: mul check removed" grep -q 'removed mul overflow check' "$OUT/on.mix.sec"
check "explain mix: add checks removed (j + 7, k + 1)" test "$(grep -c 'removed add overflow check' "$OUT/on.mix.sec")" -ge 2
section "$OUT/on.walk.txt" walk > "$OUT/on.walk.sec"
check "explain walk: mul check removed" grep -q 'removed mul overflow check' "$OUT/on.walk.sec"

"$FID" explain $T/test_midend_range_loop_fail.fd mix > "$OUT/fail.txt" 2>&1
check "explain fail variant: mul kept" grep -q "kept mul overflow check" "$OUT/fail.txt"

rm -f "$OUT/ok.exe" "$OUT/off.exe" "$OUT/fail.exe"
"$FID" emit $T/test_midend_range_loop.fd -o "$OUT/ok.exe" > "$OUT/ok.log" 2>&1
"$OUT/ok.exe" > "$OUT/ok.out" 2>&1
sed -n 's|^// \([0-9-][0-9]*\)$|\1|p' $T/test_midend_range_loop.fd > "$OUT/ok.want"
check "emitted program prints its expected lines" cmp -s <(tr -d '\r' < "$OUT/ok.out") "$OUT/ok.want"
FID_IROPT_OFF=rangeloop "$FID" emit $T/test_midend_range_loop.fd -o "$OUT/off.exe" > "$OUT/off.log" 2>&1
"$OUT/off.exe" > "$OUT/off.out" 2>&1
check "rangeloop off prints the same" cmp -s "$OUT/ok.out" "$OUT/off.out"
"$FID" emit $T/test_midend_range_loop_fail.fd -o "$OUT/fail.exe" > "$OUT/fail.log" 2>&1
if [ -f "$OUT/fail.exe" ] && ! "$OUT/fail.exe" > "$OUT/fail.out" 2>&1; then
  echo "PASS fail variant panics"
else
  echo "FAIL fail variant panics"; bad=$((bad + 1))
fi
exit $bad
