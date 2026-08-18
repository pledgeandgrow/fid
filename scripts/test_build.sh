#!/bin/bash
# test_build.sh — Run all non-skip tests through `fidesc build` and compare
# output against the interpreter. Identifies native codegen bugs.

cd "$(dirname "$0")/.."
FIDESC="./target/release/fidesc"
TMPDIR=$(mktemp -d)
trap 'rm -rf "$TMPDIR"' EXIT

# Skip list (same as harness.rs is_skip_test)
SKIP="test_comptime_basic.fd test_comptime_func.fd test_comptime_reflect.fd test_comptime_serialize.fd test_comptime_gen.fd test_http.fd test_json.fd test_tensor.fd test_simd.fd test_parallel.fd test_shape.fd test_wasm.fd test_allocator.fd test_effect_handler.fd test_asm.fd test_bare.fd test_boot.fd test_interrupt.fd test_mmu.fd test_gat.fd test_ffi_header.fd"

# Expected-fail tests (should fail to compile — skip for output comparison)
EXPECT_FAIL="test_trait_verify_fail.fd test_budget_fail.fd test_sandbox_fail.fd"

PASS=0
FAIL=0
COMPILE_FAIL=0
FAILURES=""

for testfile in tests/*.fd; do
    name=$(basename "$testfile")

    # Check skip list
    skip=false
    for s in $SKIP; do
        if [[ "$name" == "$s" ]]; then skip=true; break; fi
    done
    if $skip; then continue; fi

    # Check expect-fail list
    for s in $EXPECT_FAIL; do
        if [[ "$name" == "$s" ]]; then skip=true; break; fi
    done
    if $skip; then continue; fi

    # Get interpreter output
    interp_out=$($FIDESC run "$testfile" 2>/dev/null || true)
    interp_code=$?

    # Build native executable
    exe="$TMPDIR/${name%.fd}"
    build_out=$($FIDESC build "$testfile" -o "$exe" 2>&1)
    build_code=$?

    if [[ $build_code -ne 0 ]] || [[ ! -f "$exe" ]]; then
        COMPILE_FAIL=$((COMPILE_FAIL + 1))
        FAILURES="$FAILURES\n  [COMPILE FAIL] $name (exit=$build_code)"
        continue
    fi

    # Run native executable
    native_out=$("$exe" 2>/dev/null || true)
    native_code=$?

    # Compare
    if [[ "$interp_out" == "$native_out" ]]; then
        PASS=$((PASS + 1))
    else
        FAIL=$((FAIL + 1))
        FAILURES="$FAILURES\n  [MISMATCH] $name\n    interp: $(echo "$interp_out" | head -3 | tr '\n' '|')\n    native: $(echo "$native_out" | head -3 | tr '\n' '|')"
    fi
done

echo ""
echo "=========================================="
echo "BUILD TEST RESULTS"
echo "=========================================="
echo "Passed:        $PASS"
echo "Mismatches:    $FAIL"
echo "Compile fails: $COMPILE_FAIL"
echo "Total:         $((PASS + FAIL + COMPILE_FAIL))"
echo ""
if [[ -n "$FAILURES" ]]; then
    echo -e "Failures:$FAILURES"
fi
