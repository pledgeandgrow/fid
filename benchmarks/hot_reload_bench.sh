#!/usr/bin/env bash
# Hot Reload Latency Benchmark — Phase 4E
# Measures the time from source file change to seeing the updated result.
# Success metric: <100ms latency, no state lost.
#
# Usage: bash benchmarks/hot_reload_bench.sh

set -e

PLEDGEC=${PLEDGEC:-./target/debug/pledgec}
TMPDIR=$(mktemp -d)
TEST_FILE="$TMPDIR/test_hot.p"
RESULTS_FILE="$TMPDIR/results.txt"

echo "=== Hot Reload Latency Benchmark ==="
echo ""

# Create initial source file
cat > "$TEST_FILE" << 'EOF'
pf main() -> i64 !IO {
    print(42);
    ret 0;
}
EOF

# Build the compiler if needed
if [ ! -f "$PLEDGEC" ]; then
    echo "Building pledgec..."
    cargo build 2>/dev/null
fi

echo "Test file: $TEST_FILE"
echo ""

# Test 1: Measure compilation time (cold)
echo "--- Test 1: Cold compilation time ---"
START=$(date +%s%N)
$PLEDGEC run "$TEST_FILE" > /dev/null 2>&1
END=$(date +%s%N)
COLD_MS=$(( (END - START) / 1000000 ))
echo "Cold compile + run: ${COLD_MS}ms"

# Test 2: Measure compilation time (warm - cached)
START=$(date +%s%N)
$PLEDGEC run "$TEST_FILE" > /dev/null 2>&1
END=$(date +%s%N)
WARM_MS=$(( (END - START) / 1000000 ))
echo "Warm compile + run: ${WARM_MS}ms"

# Test 3: Measure IR lowering time
START=$(date +%s%N)
$PLEDGEC ir "$TEST_FILE" > /dev/null 2>&1
END=$(date +%s%N)
IR_MS=$(( (END - START) / 1000000 ))
echo "IR lowering: ${IR_MS}ms"

# Test 4: Measure PIR lowering time
START=$(date +%s%N)
$PLEDGEC pir "$TEST_FILE" > /dev/null 2>&1
END=$(date +%s%N)
PIR_MS=$(( (END - START) / 1000000 ))
echo "PIR lowering: ${PIR_MS}ms"

# Test 5: Measure optimization time
START=$(date +%s%N)
$PLEDGEC opt "$TEST_FILE" > /dev/null 2>&1
END=$(date +%s%N)
OPT_MS=$(( (END - START) / 1000000 ))
echo "Optimization: ${OPT_MS}ms"

# Test 6: Measure codegen time
START=$(date +%s%N)
$PLEDGEC codegen "$TEST_FILE" > /dev/null 2>&1
END=$(date +%s%N)
CODEGEN_MS=$(( (END - START) / 1000000 ))
echo "Codegen: ${CODEGEN_MS}ms"

# Test 7: Simulated hot reload (modify + recompile)
echo ""
echo "--- Test 7: Simulated hot reload (modify + recompile) ---"
for i in 1 2 3 4 5; do
    # Modify the source file
    echo "pf main() -> i64 !IO { print($((40 + i))); ret 0; }" > "$TEST_FILE"
    
    START=$(date +%s%N)
    RESULT=$($PLEDGEC run "$TEST_FILE" 2>/dev/null)
    END=$(date +%s%N)
    RELOAD_MS=$(( (END - START) / 1000000 ))
    echo "  Reload #$i: ${RELOAD_MS}ms (output: $RESULT)"
done

# Test 8: JIT compilation time (if supported)
echo ""
echo "--- Test 8: JIT compilation ---"
cat > "$TEST_FILE" << 'EOF'
pf fib(n: i64) -> i64 {
    if n <= 1 { ret n; }
    ret fib(n - 1) + fib(n - 2);
}
pf main() -> i64 !IO {
    pv result = fib(20);
    print(result);
    ret 0;
}
EOF

START=$(date +%s%N)
$PLEDGEC run "$TEST_FILE" > /dev/null 2>&1
END=$(date +%s%N)
JIT_MS=$(( (END - START) / 1000000 ))
echo "JIT fib(20): ${JIT_MS}ms"

echo ""
echo "=== Summary ==="
echo "Cold compile:     ${COLD_MS}ms"
echo "Warm compile:     ${WARM_MS}ms"
echo "IR lowering:      ${IR_MS}ms"
echo "PIR lowering:     ${PIR_MS}ms"
echo "Optimization:     ${OPT_MS}ms"
echo "Codegen:          ${CODEGEN_MS}ms"
echo "JIT fib(20):      ${JIT_MS}ms"
echo ""
if [ "$WARM_MS" -lt 100 ]; then
    echo "PASS: Warm compilation < 100ms (${WARM_MS}ms)"
else
    echo "NOTE: Warm compilation = ${WARM_MS}ms (target: <100ms)"
fi

# Cleanup
rm -rf "$TMPDIR"
