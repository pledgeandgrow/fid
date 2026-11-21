#!/usr/bin/env bash
# OP13 (known-bugs 2g.98) on pe_ir, the default Windows `fid emit`: a tail
# call stored in an inlined callee's result slot (`store call -> slot;
# jump B`, B = `load slot; return`) is a jump, as a call the next
# instruction returns is (compiler/pe_ir.fd peir_tail_via_slot).
#   bash tests/nattools/pe_tail_slot_check.sh [fid]   (default release/fid.exe)
# From the project root, on a Windows host.
# 1. tests/nattools/pe_tail_slot.fd and scripts/pe-win/tail_deep.fd (100
#    million levels of mutual recursion) print their expected lines, also
#    built with FID_VERIFY=1 and with the per-function cache off.
# 2. Built with FID_PE_OFF=tail (every call a call) pe_tail_slot.fd
#    overflows the stack: the test needs the jumps.
# 3. tests/bootstrap/test_windows_tail_sibling.fd prints its lines.
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-pe-tail-slot"
mkdir -p "$OUT"
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

case "$(uname -s)" in
    MINGW*|MSYS*|CYGWIN*|Windows_NT) ;;
    *) echo "skip pe_tail_slot_check: not a Windows host"; exit 0 ;;
esac

# label, source, then VAR=value settings for the build
build_run() {
    local label="$1" src="$2"
    shift 2
    if env "$@" "$FID" emit "$src" -o "$OUT/$label.exe" > "$OUT/$label.build" 2>&1; then
        "$OUT/$label.exe" > "$OUT/$label.out" 2> "$OUT/$label.err"
        echo $? > "$OUT/$label.code"
        return 0
    fi
    bad "$label: build failed: $(tail -3 "$OUT/$label.build")"
    return 1
}

expect() {   # label, source
    if [ "$(tr -d '\r' < "$OUT/$1.out")" = "$(expected "$2")" ] && [ "$(cat "$OUT/$1.code")" = 0 ]; then
        ok "$1"
    else
        bad "$1: output (exit $(cat "$OUT/$1.code")): $(head -c 300 "$OUT/$1.out") $(head -c 200 "$OUT/$1.err")"
    fi
}

S=tests/nattools/pe_tail_slot.fd
D=scripts/pe-win/tail_deep.fd
build_run slot "$S" && expect slot "$S"
build_run slot_verify "$S" FID_VERIFY=1 && expect slot_verify "$S"
build_run slot_nocache "$S" FID_FNCACHE=0 && expect slot_nocache "$S"
build_run deep "$D" && expect deep "$D"
build_run deep_verify "$D" FID_VERIFY=1 && expect deep_verify "$D"
build_run sibling tests/bootstrap/test_windows_tail_sibling.fd && expect sibling tests/bootstrap/test_windows_tail_sibling.fd
if build_run slot_off "$S" FID_PE_OFF=tail FID_FNCACHE=0; then
    if grep -q "Stack overflow" "$OUT/slot_off.err" "$OUT/slot_off.out"; then
        ok "slot_off: as calls the recursion overflows (the test needs the jumps)"
    else
        bad "slot_off: no stack overflow without tail jumps (the test proves nothing): $(head -c 200 "$OUT/slot_off.out")"
    fi
fi

if [ "$fails" -ne 0 ]; then
    echo "$fails failure(s)"
    exit 1
fi
echo "all OP13 result-slot tail call checks passed"
