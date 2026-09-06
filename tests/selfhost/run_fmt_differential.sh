#!/bin/bash
# run_fmt_differential.sh — Differential test for compiler/fmt.fd vs src/fmt.rs
#
# Compares the Fides formatter (fmt.fd) against the Rust formatter (fmt.rs)
# on all .fd test files. Any mismatch indicates a bug in fmt.fd.
#
# Usage:
#   ./tests/selfhost/run_fmt_differential.sh           # run on all test files
#   ./tests/selfhost/run_fmt_differential.sh --native  # use native-compiled harness (faster)
#   ./tests/selfhost/run_fmt_differential.sh <file.fd> # test a single file

set -e
cd "$(dirname "$0")/../.."

FIDESC="./target/release/fidesc"
HARNESS="tests/selfhost/test_fmt_harness.fd"
TMPDIR=$(mktemp -d)
trap "rm -rf $TMPDIR" EXIT

# Determine mode
NATIVE=0
SINGLE_FILE=""
if [[ "$1" == "--native" ]]; then
    NATIVE=1
    shift
fi
if [[ -n "$1" ]]; then
    SINGLE_FILE="$1"
fi

# Build the native harness if requested
HARNESS_BIN=""
if [[ $NATIVE -eq 1 ]]; then
    echo "Building native harness..."
    HARNESS_BIN="$TMPDIR/fmt_harness"
    $FIDESC ccompile "$HARNESS" -o "$HARNESS_BIN" 2>/dev/null
fi

# Collect test files
if [[ -n "$SINGLE_FILE" ]]; then
    TEST_FILES=("$SINGLE_FILE")
else
    TEST_FILES=()
    for f in tests/*.fd tests/selfhost/*.fd tests/bootstrap/capability/*.fd tests/bootstrap/effects/*.fd tests/bootstrap/regions/*.fd tests/bootstrap/safety/*.fd; do
        [[ -f "$f" ]] && TEST_FILES+=("$f")
    done
fi

NUM=${#TEST_FILES[@]}
echo "Testing $NUM files..."
echo ""

PASS=0
FAIL=0
MISMATCHES=()

for f in "${TEST_FILES[@]}"; do
    name=$(basename "$f")

    # Get Rust formatter output: copy file, format in place, read
    cp "$f" "$TMPDIR/rust_out.fd"
    $FIDESC fmt "$TMPDIR/rust_out.fd" 2>/dev/null
    RUST_OUT="$TMPDIR/rust_out.fd"

    # Get Fides formatter output
    if [[ $NATIVE -eq 1 ]]; then
        "$HARNESS_BIN" "$f" > "$TMPDIR/fides_out.fd" 2>/dev/null
    else
        $FIDESC run "$HARNESS" -- "$f" > "$TMPDIR/fides_out.fd" 2>/dev/null
    fi

    # Compare
    if diff "$RUST_OUT" "$TMPDIR/fides_out.fd" > "$TMPDIR/diff.txt" 2>&1; then
        PASS=$((PASS + 1))
        printf "  \033[0;32mPASS\033[0m %s\n" "$name"
    else
        FAIL=$((FAIL + 1))
        MISMATCHES+=("$f")
        printf "  \033[0;31mFAIL\033[0m %s\n" "$name"
        # Show first few lines of diff
        head -10 "$TMPDIR/diff.txt" | sed 's/^/    /'
    fi
done

echo ""
echo "=========================================="
echo "RESULTS: $PASS/$NUM passed, $FAIL failed"
echo "=========================================="

if [[ $FAIL -gt 0 ]]; then
    echo ""
    echo "Mismatches:"
    for m in "${MISMATCHES[@]}"; do
        echo "  $m"
    done
    exit 1
fi
