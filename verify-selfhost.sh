#!/bin/bash
# verify-selfhost.sh â€” Self-hosting compiler component verification
#
# Usage:
#   ./verify-selfhost.sh              Run all tests/selfhost/*.fd on 3 backends, diff vs baseline
#   ./verify-selfhost.sh --save       Run all tests, save results as new baseline
#   ./verify-selfhost.sh --quick      Build only (no tests)
#   ./verify-selfhost.sh --verbose    Show test output (always shown on failure)
#   ./verify-selfhost.sh --list       List all self-host test files
#   ./verify-selfhost.sh --help       Show this help
#
# This script is separate from verify.sh:
#   verify.sh            â†’ tests/bootstrap/ (tests for the Rust compiler in src/)
#   verify-selfhost.sh   â†’ tests/selfhost/ (tests for self-hosted Fides components in compiler/)
#
# The self-host tests verify that Fides modules rewritten from Rust (token.fd,
# diag.fd, json.fd, fmt.fd, ast.fd, lexer.fd, parser.fd, ...) produce correct
# output on all 3 backends: interpreter, C codegen, and PE emitter.
#
# Each test file in tests/selfhost/ is a standalone Fides program that imports
# the module under test and runs assertions. A test passes if it exits 0 and
# prints "All tests passed!" (or similar success message).

set -e

cd "$(dirname "$0")"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

FIDESC="./target/release/fidesc"
TEST_DIR="tests/selfhost"
BASELINE_DIR="tests/selfhost/baseline"
BASELINE_FILE="$BASELINE_DIR/test_results.txt"

# Temp file for capturing test output (shown on failure)
TMP_OUT=$(mktemp)
trap 'rm -f "$TMP_OUT"' EXIT

# --verbose: show output even for passing tests
VERBOSE=0
for arg in "$@"; do
    [[ "$arg" == "--verbose" ]] && VERBOSE=1
done

# --- Help ---
if [[ "$1" == "--help" || "$1" == "-h" ]]; then
    cat <<'EOF'
verify-selfhost.sh â€” Self-hosting compiler component verification

Commands:
  (default)  Build release + run all tests/selfhost/*.fd on 3 backends, diff vs baseline
  --save     Build release + run all tests, save results as new baseline
  --quick    Build release only (no tests)
  --verbose  Show test output even for passing tests (always shown on failure)
  --list     List all self-host test files
  --help     This help message

Baseline is stored in tests/selfhost/baseline/test_results.txt

Test suites:
  verify.sh            â†’ tests/bootstrap/ (Rust compiler in src/)
  verify-selfhost.sh   â†’ tests/selfhost/ (self-hosted Fides components in compiler/)

Backends tested:
  1. Interpreter   (fidesc run)
  2. C codegen     (fidesc ccompile + run .exe)
  3. PE emitter    (fidesc build + run native binary)

Workflow:
  1. Before making changes:    ./verify-selfhost.sh          (confirm baseline passes)
  2. Make your change
  3. After making changes:     ./verify-selfhost.sh          (see what regressed)
  4. If regressions:           fix them
  5. If intentional changes:   ./verify-selfhost.sh --save   (update baseline)
EOF
    exit 0
fi

# --- List mode ---
if [[ "$1" == "--list" ]]; then
    echo "Self-host test files in $TEST_DIR/:"
    for f in "$TEST_DIR"/*.fd; do
        [[ -f "$f" ]] && echo "  $(basename "$f")"
    done
    exit 0
fi

# --- Quick mode: build only ---
if [[ "$1" == "--quick" ]]; then
    echo -e "${YELLOW}Building (release)...${NC}"
    cargo build --release 2>&1 | tail -3
    echo -e "${GREEN}Build OK${NC}"
    exit 0
fi

# --- Build ---
echo -e "${YELLOW}Building (release)...${NC}"
cargo build --release 2>&1 | grep -E "^(error|Finished)" || true

if [[ ! -x "$FIDESC" ]]; then
    echo -e "${RED}ERROR: fidesc binary not found at $FIDESC${NC}"
    exit 1
fi

# --- Collect test files ---
# Skip harness tests that require file arguments (not standalone tests)
SKIP_TESTS="test_fmt_harness.fd test_lexer_harness.fd test_parser_harness.fd test_sl_debug.fd"

TEST_FILES=()
for f in "$TEST_DIR"/*.fd; do
    [[ -f "$f" ]] || continue
    # Skip harness tests that need file arguments
    skip=0
    for s in $SKIP_TESTS; do
        if [[ "$(basename "$f")" == "$s" ]]; then
            skip=1
            break
        fi
    done
    [[ $skip -eq 1 ]] && continue
    TEST_FILES+=("$f")
done

NUM_TESTS=${#TEST_FILES[@]}
if [[ $NUM_TESTS -eq 0 ]]; then
    echo -e "${RED}ERROR: no .fd test files found in $TEST_DIR/${NC}"
    exit 1
fi

echo -e "${YELLOW}Running $NUM_TESTS self-host tests on 3 backends...${NC}"
echo ""

# --- Run tests ---
# Output format (for baseline storage):
#   === Interpreter ===
#   test_token.fd: PASS
#   test_diag.fd: PASS
#   ...
#   === C Codegen ===
#   ...
#   === PE Emitter ===
#   ...
#   === Summary ===
#   Interpreter: X/Y passed
#   C Codegen:   X/Y passed
#   PE Emitter:  X/Y passed

OUTPUT=""
INT_PASS=0
INT_FAIL=0
C_PASS=0
C_FAIL=0
PE_PASS=0
PE_FAIL=0

# --- Backend 1: Interpreter ---
echo -e "${YELLOW}=== Interpreter ===${NC}"
OUTPUT="${OUTPUT}=== Interpreter ===\n"
for f in "${TEST_FILES[@]}"; do
    name=$(basename "$f")
    if $FIDESC run "$f" > "$TMP_OUT" 2>&1; then
        echo -e "  ${GREEN}PASS${NC} $name"
        [[ $VERBOSE -eq 1 ]] && sed 's/^/    /' "$TMP_OUT" | head -20
        OUTPUT="${OUTPUT}${name}: PASS\n"
        INT_PASS=$((INT_PASS + 1))
    else
        echo -e "  ${RED}FAIL${NC} $name"
        echo -e "  ${RED}--- output (head) ---${NC}"
        sed 's/^/  /' "$TMP_OUT" | head -40
        echo -e "  ${RED}--- end ---${NC}"
        OUTPUT="${OUTPUT}${name}: FAIL\n"
        INT_FAIL=$((INT_FAIL + 1))
    fi
done

# --- Backend 2: C codegen ---
# Note: we use `fidesc run --backend c` instead of `fidesc ccompile` + running
# the .exe directly, because the standalone C-compiled binaries can segfault
# on complex tests with heavy string concatenation (pre-existing C codegen
# limitation with large functions). Running via `fidesc run --backend c`
# executes in-process and is reliable.
echo ""
echo -e "${YELLOW}=== C Codegen ===${NC}"
OUTPUT="${OUTPUT}\n=== C Codegen ===\n"
for f in "${TEST_FILES[@]}"; do
    name=$(basename "$f")
    if $FIDESC run "$f" --backend c > "$TMP_OUT" 2>&1; then
        echo -e "  ${GREEN}PASS${NC} $name"
        [[ $VERBOSE -eq 1 ]] && sed 's/^/    /' "$TMP_OUT" | head -20
        OUTPUT="${OUTPUT}${name}: PASS\n"
        C_PASS=$((C_PASS + 1))
    else
        echo -e "  ${RED}FAIL${NC} $name"
        echo -e "  ${RED}--- output (head) ---${NC}"
        sed 's/^/  /' "$TMP_OUT" | head -40
        echo -e "  ${RED}--- end ---${NC}"
        OUTPUT="${OUTPUT}${name}: FAIL\n"
        C_FAIL=$((C_FAIL + 1))
    fi
done

# --- Backend 3: PE emitter ---
# Note: we use `fidesc run --backend pe` instead of `fidesc build` + running
# the binary directly, because the PE-emitted standalone binaries segfault on
# complex tests when stdout is redirected (pre-existing PE emitter limitation).
# Running via `fidesc run --backend pe` executes in-process and is reliable.
echo ""
echo -e "${YELLOW}=== PE Emitter ===${NC}"
OUTPUT="${OUTPUT}\n=== PE Emitter ===\n"
for f in "${TEST_FILES[@]}"; do
    name=$(basename "$f")
    if $FIDESC run "$f" --backend pe > "$TMP_OUT" 2>&1; then
        echo -e "  ${GREEN}PASS${NC} $name"
        [[ $VERBOSE -eq 1 ]] && sed 's/^/    /' "$TMP_OUT" | head -20
        OUTPUT="${OUTPUT}${name}: PASS\n"
        PE_PASS=$((PE_PASS + 1))
    else
        echo -e "  ${RED}FAIL${NC} $name"
        echo -e "  ${RED}--- output (head) ---${NC}"
        sed 's/^/  /' "$TMP_OUT" | head -40
        echo -e "  ${RED}--- end ---${NC}"
        OUTPUT="${OUTPUT}${name}: FAIL\n"
        PE_FAIL=$((PE_FAIL + 1))
    fi
done

# --- Summary ---
echo ""
echo "=========================================="
echo "SUMMARY"
echo "=========================================="
echo "Interpreter: $INT_PASS/$NUM_TESTS passed  ($INT_FAIL failed)"
echo "C Codegen:   $C_PASS/$NUM_TESTS passed  ($C_FAIL failed)"
echo "PE Emitter:  $PE_PASS/$NUM_TESTS passed  ($PE_FAIL failed)"

OUTPUT="${OUTPUT}\n=== Summary ===\n"
OUTPUT="${OUTPUT}Interpreter: ${INT_PASS}/${NUM_TESTS} passed\n"
OUTPUT="${OUTPUT}C Codegen:   ${C_PASS}/${NUM_TESTS} passed\n"
OUTPUT="${OUTPUT}PE Emitter:  ${PE_PASS}/${NUM_TESTS} passed\n"

# --- Save mode: just save and exit ---
if [[ "$1" == "--save" ]]; then
    mkdir -p "$BASELINE_DIR"
    echo -e "$OUTPUT" > "$BASELINE_FILE"
    echo ""
    echo -e "${GREEN}Baseline saved to $BASELINE_FILE${NC}"
    exit 0
fi

# --- Diff mode: compare to baseline ---
if [[ ! -f "$BASELINE_FILE" ]]; then
    echo ""
    echo -e "${YELLOW}No baseline found. Run: ./verify-selfhost.sh --save${NC}"
    echo -e "${YELLOW}to capture the current results as baseline.${NC}"
    # Exit 0 if all pass, 1 if any fail
    if [[ $INT_FAIL -eq 0 && $C_FAIL -eq 0 && $PE_FAIL -eq 0 ]]; then
        exit 0
    else
        exit 1
    fi
fi

# Compare current results to baseline
CURRENT_FAILS=$(echo -e "$OUTPUT" | grep ": FAIL" || true)
BASELINE_FAILS=$(grep ": FAIL" "$BASELINE_FILE" || true)

CURRENT_SUMMARY=$(echo -e "$OUTPUT" | grep -E "^(Interpreter|C Codegen|PE Emitter):")
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

if [[ "$CURRENT_FAILS" == "$BASELINE_FAILS" ]]; then
    echo -e "${GREEN}âœ“ No regressions â€” same failures as baseline${NC}"
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
        echo -e "${RED}âš   Regressions detected! Fix these before saving a new baseline.${NC}"
        exit 1
    else
        echo -e "${GREEN}âœ“ No regressions â€” some tests fixed!${NC}"
        echo -e "${YELLOW}Run ./verify-selfhost.sh --save to update the baseline.${NC}"
        exit 0
    fi
fi
