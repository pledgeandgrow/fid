#!/bin/bash
# param-range-check.sh — LF10 (llvm-roadmap): ranges through function
# boundaries (compiler/checks.fd peir_note_param_ranges, FID_IROPT_ON=
# paramrange) and LF7's new counted-loop shapes (peir_counted_down,
# peir_le_bound).
#
# Usage: tests/tooling/param-range-check.sh      (FID=<compiler>, default
#        release/fid.exe; OUT=<dir>, default build/paramrange)
#
# Checks, one line each (PASS/FAIL), exit status the number of FAILs:
#  1. `fid explain` of test_midend_param_range.fd with paramrange on lists
#     both of bump's checks removed and scale's kept; with it off, bump's
#     two are kept;
#  2. the fail variant keeps bump's mul check with paramrange on;
#  3. both programs emitted with paramrange on: the first prints its
#     expected lines, the second exits with a failure;
#  4. `fid explain` of test_counted_loop_down.fd lists the bounds checks of
#     sum_down, rev_fill, sum_le, sum_le_direct, digits_down and keep_down
#     as removed (counted loop); the fail variant keeps its one;
#  5. the same for test_counted_loop_shapes.fd (every function but
#     keep_odd, which keeps its check) and its two fail variants
#     (test_counted_loop_shapes_fail, test_counted_loop_step_down_fail),
#     and the programs run (interpreter and emit) with the expected output,
#     the fail variants failing.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/paramrange}"
mkdir -p "$OUT"
T=tests/bootstrap
bad=0
check() { # check <label> <command...>: PASS when the command succeeds
  local label="$1"; shift
  if "$@" > /dev/null 2>&1; then echo "PASS $label"; else echo "FAIL $label"; bad=$((bad + 1)); fi
}

FID_IROPT_ON=paramrange "$FID" explain $T/test_midend_param_range.fd > "$OUT/on.txt" 2>&1
FID_IROPT_OFF=paramrange "$FID" explain $T/test_midend_param_range.fd > "$OUT/off.txt" 2>&1
check "explain on: bump 2 removed" grep -q "^fn bump: 2 check(s) removed, 0 kept" "$OUT/on.txt"
check "explain on: scale kept" grep -q "^fn scale: 0 check(s) removed, 1 kept" "$OUT/on.txt"
check "explain off: bump 2 kept" grep -q "^fn bump: 0 check(s) removed, 2 kept" "$OUT/off.txt"

FID_IROPT_ON=paramrange "$FID" explain $T/test_midend_param_range_fail.fd bump > "$OUT/fail.txt" 2>&1
check "explain fail variant: mul kept" grep -q "kept mul overflow check" "$OUT/fail.txt"

rm -f "$OUT/pr.exe" "$OUT/prf.exe"
FID_IROPT_ON=paramrange "$FID" emit $T/test_midend_param_range.fd -o "$OUT/pr.exe" > "$OUT/pr.log" 2>&1
FID_IROPT_ON=paramrange "$FID" emit $T/test_midend_param_range_fail.fd -o "$OUT/prf.exe" > "$OUT/prf.log" 2>&1
sed -n '/^\/\/ Expected output:/,/^$/{s#^// ##p}' $T/test_midend_param_range.fd | sed '/^$/d;/^Expected output:/d' > "$OUT/expected.txt"
"$OUT/pr.exe" > "$OUT/pr.out" 2>&1
check "emit on: output as expected" cmp -s "$OUT/pr.out" "$OUT/expected.txt"
check "emit on: fail variant fails" bash -c "! '$OUT/prf.exe' > '$OUT/prf.out' 2>&1"

"$FID" explain $T/test_counted_loop_down.fd > "$OUT/down.txt" 2>&1
# keep_down (test `i > 1`) is proved since Nov 17: reads at i + o, any K
for fn in sum_down rev_fill sum_le sum_le_direct digits_down keep_down; do
  check "counted down: $fn bounds removed" bash -c "awk '/^fn /{f=\$2} f==\"$fn:\" && /removed bounds check/' '$OUT/down.txt' | grep -q ."
  check "counted down: $fn no bounds kept" bash -c "! awk '/^fn /{f=\$2} f==\"$fn:\" && /kept bounds check/' '$OUT/down.txt' | grep -q ."
done
"$FID" explain $T/test_counted_loop_down_fail.fd sum_down_early > "$OUT/downf.txt" 2>&1
check "counted down fail variant: bounds kept" grep -q "kept bounds check" "$OUT/downf.txt"

# LF7, Nov 17: mirrored tests, bounds L - d with reads at i + e, steps
# down other than 1, tests against constants other than 0
"$FID" explain $T/test_counted_loop_shapes.fd > "$OUT/shapes.txt" 2>&1
for fn in sum_zero_lt sum_len_gt sum_last_ge pair_products window3 step_down2 pre_down down3 for_pairs; do
  check "shapes: $fn bounds removed" bash -c "awk '/^fn /{f=\$2} f==\"$fn:\" && /removed bounds check/' '$OUT/shapes.txt' | grep -q ."
  check "shapes: $fn no bounds kept" bash -c "! awk '/^fn /{f=\$2} f==\"$fn:\" && /kept bounds check/' '$OUT/shapes.txt' | grep -q ."
done
check "shapes: keep_odd keeps bounds" bash -c "awk '/^fn /{f=\$2} f==\"keep_odd:\" && /kept bounds check/' '$OUT/shapes.txt' | grep -q ."
"$FID" explain $T/test_counted_loop_shapes_fail.fd pairs_far > "$OUT/shapesf.txt" 2>&1
check "shapes fail variant: bounds kept" grep -q "kept bounds check" "$OUT/shapesf.txt"
"$FID" explain $T/test_counted_loop_step_down_fail.fd sum_by_two > "$OUT/stepdf.txt" 2>&1
check "step-down fail variant: bounds kept" grep -q "kept bounds check" "$OUT/stepdf.txt"

# The programs: same output on the interpreter and the native path; the
# fail variants fail
sed -n '/^\/\/ Expected output:/,/^$/{s#^// ##p}' $T/test_counted_loop_shapes.fd | sed '/^$/d;/^Expected output:/d' > "$OUT/shapes.expected"
"$FID" run $T/test_counted_loop_shapes.fd 2>/dev/null | grep -v '^\[' > "$OUT/shapes.run"
check "shapes: run output as expected" cmp -s "$OUT/shapes.run" "$OUT/shapes.expected"
rm -f "$OUT/shapes.exe" "$OUT/shapesf.exe" "$OUT/stepdf.exe"
"$FID" emit $T/test_counted_loop_shapes.fd -o "$OUT/shapes.exe" > "$OUT/shapes.log" 2>&1
"$OUT/shapes.exe" > "$OUT/shapes.out" 2>&1
check "shapes: emit output as expected" cmp -s "$OUT/shapes.out" "$OUT/shapes.expected"
"$FID" emit $T/test_counted_loop_shapes_fail.fd -o "$OUT/shapesf.exe" > "$OUT/shapesf.log" 2>&1
check "shapes fail variant: emitted program fails" bash -c "! '$OUT/shapesf.exe' > /dev/null 2>&1"
"$FID" emit $T/test_counted_loop_step_down_fail.fd -o "$OUT/stepdf.exe" > "$OUT/stepdf.log" 2>&1
check "step-down fail variant: emitted program fails" bash -c "! '$OUT/stepdf.exe' > /dev/null 2>&1"

echo "param-range-check: $bad failed"
exit "$bad"
