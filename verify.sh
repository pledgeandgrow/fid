
#!/bin/bash
# verify.sh — P compiler verification script
#
# Usage:
#   ./verify.sh              Build + run all tests, compare to baseline
#   ./verify.sh --save       Build + run all tests, save as new baseline
#   ./verify.sh --quick      Build only (no tests) — fast syntax/type check
#   ./verify.sh --golden     Regenerate golden files for intermediate IR/PIR/asm
#   ./verify.sh --help       Show this help
#
# This script prevents the "fix one thing, break everything" problem by:
#   1. Capturing a known-good baseline of test results
#   2. Diffing current results against baseline after every change
#   3. Showing exactly which tests regressed (not just "N failed")
#
# ALWAYS run this before and after making changes to the compiler.

set -e

cd "$(dirname "$0")"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

PLEDGEC="./target/release/pledgec"
BASELINE_DIR="tests/baseline"
BASELINE_FILE="$BASELINE_DIR/test_results.txt"

# --- Help ---
if [[ "$1" == "--help" || "$1" == "-h" ]]; then
    cat <<'EOF'
verify.sh — P compiler verification

Commands:
  (default)  Build release + run pledgec test + diff against baseline
  --save     Build release + run pledgec test + save results as new baseline
  --quick    Build release only (no tests) — fast check
  --golden   Regenerate golden files (IR/PIR/asm) for key tests
  --help     This help message

Baseline is stored in tests/baseline/test_results.txt
Golden files are stored in tests/golden/

Workflow:
  1. Before making changes:    ./verify.sh          (confirm baseline passes)
  2. Make your change
  3. After making changes:     ./verify.sh          (see what regressed)
  4. If regressions:           fix them
  5. If intentional changes:   ./verify.sh --save   (update baseline)
EOF
    exit 0
fi

# --- Quick mode: build only ---
if [[ "$1" == "--quick" ]]; then
    echo -e "${YELLOW}Building (release)...${NC}"
    cargo build --release 2>&1 | tail -3
    echo -e "${GREEN}Build OK${NC}"
    exit 0
fi

# --- Golden mode: regenerate golden files ---
if [[ "$1" == "--golden" ]]; then
    echo -e "${YELLOW}Regenerating golden files...${NC}"
    ./scripts/gen_golden.sh
    echo -e "${GREEN}Golden files regenerated${NC}"
    exit 0
fi

# --- Build ---
echo -e "${YELLOW}Building (release)...${NC}"
cargo build --release 2>&1 | grep -E "^(error|Finished)" || true

if [[ ! -x "$PLEDGEC" ]]; then
    echo -e "${RED}ERROR: pledgec binary not found at $PLEDGEC${NC}"
    exit 1
fi

# --- Run tests ---
echo -e "${YELLOW}Running normal tests...${NC}"
TEST_OUTPUT=$($PLEDGEC test 2>&1 || true)
echo "$TEST_OUTPUT"

echo -e "${YELLOW}Running expected-fail tests...${NC}"
FAIL_OUTPUT=$($PLEDGEC test --fail 2>&1 || true)
echo "$FAIL_OUTPUT"

# Combine for baseline storage
COMBINED_OUTPUT="${TEST_OUTPUT}

${FAIL_OUTPUT}"

# --- Save mode: just save and exit ---
if [[ "$1" == "--save" ]]; then
    mkdir -p "$BASELINE_DIR"
    echo "$COMBINED_OUTPUT" > "$BASELINE_FILE"
    echo -e "${GREEN}Baseline saved to $BASELINE_FILE${NC}"
    exit 0
fi

# --- Diff mode: compare to baseline ---
if [[ ! -f "$BASELINE_FILE" ]]; then
    echo ""
    echo -e "${YELLOW}No baseline found. Run: ./verify.sh --save${NC}"
    echo -e "${YELLOW}to capture the current results as baseline.${NC}"
    exit 0
fi

# Extract failures for comparison (includes [FAIL] and [UNEXPECTED PASS])
CURRENT_FAILS=$(echo "$COMBINED_OUTPUT" | grep -E "^\s*\[FAIL\]|^\s*\[UNEXPECTED" || true)
BASELINE_FAILS=$(grep -E "^\s*\[FAIL\]|^\s*\[UNEXPECTED" "$BASELINE_FILE" || true)

# Extract summary lines
CURRENT_SUMMARY=$(echo "$COMBINED_OUTPUT" | grep -E "^(Interpreter|C Codegen|PE Emitter):")
BASELINE_SUMMARY=$(grep -E "^(Interpreter|C Codegen|PE Emitter):" "$BASELINE_FILE")

echo ""
echo "=========================================="
echo "BASELINE COMPARISON"
echo "=========================================="
echo ""
echo "--- Baseline ---"
echo "$BASELINE_SUMMARY"
echo ""
echo "--- Current ---"
echo "$CURRENT_SUMMARY"
echo ""

# Compare failures
if [[ "$CURRENT_FAILS" == "$BASELINE_FAILS" ]]; then
    echo -e "${GREEN}✓ No regressions — same failures as baseline${NC}"
    exit 0
else
    # Show new failures (in current but not baseline)
    NEW_FAILS=""
    while IFS= read -r line; do
        if [[ -n "$line" ]] && ! echo "$BASELINE_FAILS" | grep -qF "$line"; then
            NEW_FAILS="$NEW_FAILS$line\n"
        fi
    done <<< "$CURRENT_FAILS"

    # Show fixed tests (in baseline but not current)
    FIXED=""
    while IFS= read -r line; do
        if [[ -n "$line" ]] && ! echo "$CURRENT_FAILS" | grep -qF "$line"; then
            FIXED="$FIXED$line\n"
        fi
    done <<< "$BASELINE_FAILS"

    if [[ -n "$FIXED" ]]; then
        echo -e "${GREEN}Fixed (no longer failing):${NC}"
        echo -e "$FIXED"
    fi

    if [[ -n "$NEW_FAILS" ]]; then
        echo -e "${RED}REGRESSIONS (new failures):${NC}"
        echo -e "$NEW_FAILS"
        echo ""
        echo -e "${RED}⚠  Regressions detected! Fix these before saving a new baseline.${NC}"
        exit 1
    else
        echo -e "${GREEN}✓ No regressions — some tests fixed!${NC}"
        echo -e "${YELLOW}Run ./verify.sh --save to update the baseline.${NC}"
        exit 0
    fi
fi
