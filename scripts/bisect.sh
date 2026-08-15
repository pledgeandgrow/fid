#!/bin/bash
# bisect.sh — Find which compiler layer introduced a regression
#
# Usage:
#   ./scripts/bisect.sh <test_name>
#
# Example:
#   ./scripts/bisect.sh strings
#
# This diffs the current output of each compiler stage against the golden
# files to pinpoint exactly where a regression was introduced:
#
#   Layer         Command                    Golden file
#   ─────────    ─────────                 ─────────────
#   IR            fidesc ir                .ir
#   PIR L1        fidesc pir 1             .pir_l1
#   PIR L2        fidesc pir 2             .pir_l2
#   PIR L3        fidesc pir 3             .pir_l3
#   Codegen       fidesc codegen           .asm
#   Peephole      fidesc codegen --peephole .peephole
#   Output        fidesc run               .out
#
# The first layer that differs from golden is where the regression lives.

set -e
cd "$(dirname "$0")/.."

PLEDGEC="./target/release/fidesc"

if [[ -z "$1" ]]; then
    echo "Usage: $0 <test_name>"
    echo "Example: $0 strings"
    echo ""
    echo "Available golden tests:"
    ls -1 tests/golden/ 2>/dev/null || echo "  (none — run ./scripts/gen_golden.sh first)"
    exit 1
fi

name="$1"
golden_dir="tests/golden/${name}"
src="tests/${name}.fd"

if [[ ! -d "$golden_dir" ]]; then
    echo "ERROR: No golden files for '$name' in $golden_dir"
    echo "Run: ./scripts/gen_golden.sh"
    exit 1
fi

if [[ ! -f "$src" ]]; then
    echo "ERROR: Source file not found: $src"
    exit 1
fi

# Make sure fidesc is built
if [[ ! -x "$PLEDGEC" ]]; then
    echo "Building fidesc..."
    cargo build --release 2>&1 | grep -E "^(error|Finished)" || true
fi

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo "Bisecting regression for test: $name"
echo "Source: $src"
echo ""

regression_found=false

# normalize — sort lines for stages where order is non-deterministic
# (IR and PIR have HashMap/HashSet-based ordering that varies between runs:
#  struct definition order, string constant numbering, effect list order,
#  stack slot assignment)
# Usage: normalize <sort|exact|asm>
normalize() {
    local mode="$1"
    if [[ "$mode" == "sort" ]]; then
        # 1. Sort comma-separated effects in !annotations: !write,alloc,read → !alloc,read,write
        # 2. Remove blank lines
        # 3. Sort all lines
        perl -pe 's/!([a-zA-Z,]+)/"!" . join(",", sort split \/,\/, $1)/ge' | \
        sed -e '/^$/d' | \
        LC_ALL=C sort
    elif [[ "$mode" == "asm" ]]; then
        # Normalize assembly: replace non-deterministic parts with placeholders
        # 1. Stack offsets: -N(%rbp) → SLOT(%rbp) (HashMap-based slot assignment)
        # 2. String constant names: .str_N → .str_X (HashMap-based numbering)
        # 3. Sort all lines (data section order is non-deterministic; code section
        #    order is deterministic so sorting both sides still catches real changes)
        sed -E 's/-?[0-9]+\(%rbp\)/SLOT(%rbp)/g; s/\.str_[0-9]+/.str_X/g' | \
        sed -e '/^$/d' | \
        LC_ALL=C sort
    elif [[ "$mode" == "output" ]]; then
        # For program output, ignore trailing newline differences
        sed -e '/^$/d' -e 's/\n$//'
    else
        cat
    fi
}

check_layer() {
    local label="$1"
    local cmd="$2"
    local golden="$3"
    local mode="${4:-exact}"  # "sort" or "exact"

    if [[ ! -f "$golden" ]]; then
        echo -e "  ${YELLOW}? $label${NC} (no golden file: $golden)"
        return
    fi

    local current
    current=$(eval "$cmd" 2>/dev/null || true)

    if [[ "$mode" == "sort" ]]; then
        # For non-deterministic stages, normalize and sort both sides before comparing
        if diff -q <(echo "$current" | normalize sort) <(normalize sort < "$golden") > /dev/null 2>&1; then
            echo -e "  ${GREEN}✓ $label${NC} — matches golden (normalized)"
            return 0
        else
            echo -e "  ${RED}✗ $label${NC} — DIFFERS from golden (normalized)"
            regression_found=true
            echo ""
            echo "    First differences (normalized):"
            diff <(echo "$current" | normalize sort) <(normalize sort < "$golden") | head -20 | sed 's/^/    /'
            echo ""
            return 1
        fi
    elif [[ "$mode" == "asm" ]]; then
        # For assembly, normalize stack offsets (non-deterministic slot assignment)
        # but preserve instruction order (matters for codegen correctness)
        if diff -q <(echo "$current" | normalize asm) <(normalize asm < "$golden") > /dev/null 2>&1; then
            echo -e "  ${GREEN}✓ $label${NC} — matches golden (offset-normalized)"
            return 0
        else
            echo -e "  ${RED}✗ $label${NC} — DIFFERS from golden (offset-normalized)"
            regression_found=true
            echo ""
            echo "    First differences (offset-normalized):"
            diff <(echo "$current" | normalize asm) <(normalize asm < "$golden") | head -20 | sed 's/^/    /'
            echo ""
            return 1
        fi
    else
        # For deterministic stages (output), compare with normalization
        if diff -q <(echo "$current" | normalize "$mode") <(normalize "$mode" < "$golden") > /dev/null 2>&1; then
            echo -e "  ${GREEN}✓ $label${NC} — matches golden"
            return 0
        else
            echo -e "  ${RED}✗ $label${NC} — DIFFERS from golden"
            regression_found=true
            echo ""
            echo "    First differences:"
            diff <(echo "$current" | normalize "$mode") <(normalize "$mode" < "$golden") | head -20 | sed 's/^/    /'
            echo ""
            return 1
        fi
    fi
}

echo "Checking each compiler layer:"
echo ""

check_layer "IR (lowering)" \
    "$PLEDGEC ir $src" \
    "$golden_dir/${name}.ir" \
    "sort"

check_layer "PIR L1 (semantic)" \
    "$PLEDGEC pir $src 1" \
    "$golden_dir/${name}.pir_l1" \
    "sort"

check_layer "PIR L2 (mid-level)" \
    "$PLEDGEC pir $src 2" \
    "$golden_dir/${name}.pir_l2" \
    "sort"

check_layer "PIR L3 (low-level)" \
    "$PLEDGEC pir $src 3" \
    "$golden_dir/${name}.pir_l3" \
    "sort"

check_layer "Codegen (x86-64 asm)" \
    "$PLEDGEC codegen $src --target x86-64" \
    "$golden_dir/${name}.asm" \
    "asm"

check_layer "Peephole (x86-64 asm + opts)" \
    "$PLEDGEC codegen $src --target x86-64 --peephole" \
    "$golden_dir/${name}.peephole" \
    "asm"

check_layer "Output (interpreter)" \
    "$PLEDGEC run $src" \
    "$golden_dir/${name}.out" \
    "output"

echo ""
if [[ "$regression_found" == "true" ]]; then
    echo -e "${RED}⚠  Regression detected!${NC}"
    echo ""
    echo "The FIRST layer marked ✗ is where the regression was introduced."
    echo "Focus your debugging on the source file for that layer:"
    echo ""
    echo "  IR       → src/lower.rs, src/ir.rs"
    echo "  PIR L1   → src/pir.rs (L1 lowering)"
    echo "  PIR L2   → src/pir.rs (L2 transforms)"
    echo "  PIR L3   → src/pir.rs (L3 lowering)"
    echo "  Codegen  → src/codegen_x64.rs, src/regalloc.rs"
    echo "  Peephole → src/peephole.rs"
    echo "  Output   → src/interp.rs (or a later layer if output differs but asm doesn't)"
    exit 1
else
    echo -e "${GREEN}✓ All layers match golden files — no regression${NC}"
    exit 0
fi
