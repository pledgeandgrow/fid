#!/bin/bash
# gen_golden.sh — Generate golden files for intermediate compiler stages
#
# For each key test, captures:
#   .ir       — SSA IR after lowering
#   .pir_l1   — PIR Level 1 (semantic)
#   .pir_l2   — PIR Level 2 (mid-level)
#   .pir_l3   — PIR Level 3 (low-level)
#   .asm      — x86-64 assembly after codegen
#   .peephole — assembly after peephole optimization
#   .out      — program output (interpreter)
#
# When a test fails, diff these against the current output to find which
# layer introduced the regression:
#   diff tests/golden/strings.ir <(pledgec ir tests/strings.p)
#   diff tests/golden/strings.pir_l3 <(pledgec pir tests/strings.p 3)
#   diff tests/golden/strings.asm <(pledgec codegen tests/strings.p --target x86-64)

set -e
cd "$(dirname "$0")/.."

PLEDGEC="./target/release/pledgec"
GOLDEN_DIR="tests/golden"

if [[ ! -x "$PLEDGEC" ]]; then
    echo "Building pledgec..."
    cargo build --release 2>&1 | grep -E "^(error|Finished)" || true
fi

# Key tests that cover the main language features
# Chosen to exercise: arithmetic, structs, closures, generics, Vec, match, refs, defer
KEY_TESTS=(
    hello
    arithmetic
    struct
    test_closure
    test_closure_capture
    test_generics
    test_vec_loop
    test_match
    test_match_edge
    test_refs_mut
    test_defer
    test_traits
    test_hashmap
    test_option
    test_try
)

echo "Generating golden files for ${#KEY_TESTS[@]} tests..."

for name in "${KEY_TESTS[@]}"; do
    src="tests/${name}.p"
    if [[ ! -f "$src" ]]; then
        echo "  SKIP $name (file not found: $src)"
        continue
    fi

    out_dir="$GOLDEN_DIR/${name}"
    mkdir -p "$out_dir"

    # IR
    $PLEDGEC ir "$src" > "$out_dir/${name}.ir" 2>/dev/null || true

    # PIR L1, L2, L3
    $PLEDGEC pir "$src" 1 > "$out_dir/${name}.pir_l1" 2>/dev/null || true
    $PLEDGEC pir "$src" 2 > "$out_dir/${name}.pir_l2" 2>/dev/null || true
    $PLEDGEC pir "$src" 3 > "$out_dir/${name}.pir_l3" 2>/dev/null || true

    # Assembly (x86-64, no peephole)
    $PLEDGEC codegen "$src" --target x86-64 > "$out_dir/${name}.asm" 2>/dev/null || true

    # Assembly (x86-64, with peephole — this is what `build` uses)
    $PLEDGEC codegen "$src" --target x86-64 --peephole > "$out_dir/${name}.peephole" 2>/dev/null || true

    # Interpreter output
    $PLEDGEC run "$src" > "$out_dir/${name}.out" 2>/dev/null || true

    echo "  OK   $name ($(wc -l < "$out_dir/${name}.ir") IR lines, $(wc -l < "$out_dir/${name}.pir_l3") PIR L3 lines, $(wc -l < "$out_dir/${name}.asm") asm lines)"
done

echo ""
echo "Golden files saved to $GOLDEN_DIR/"
echo ""
echo "To check for regressions at a specific layer:"
echo "  diff tests/golden/strings/strings.pir_l3 <(./target/release/pledgec pir tests/strings.p 3)"
echo "  diff tests/golden/strings/strings.asm <(./target/release/pledgec codegen tests/strings.p --target x86-64)"
