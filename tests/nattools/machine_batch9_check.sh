#!/usr/bin/env bash
# Checks of the machine-layer goals of batch 9, run after the compiler is
# built:
#   bash tests/nattools/machine_batch9_check.sh [fid] [zig]
#   (defaults release/fid.exe, zig)
# - OP18 (competitive-roadmap): tests/bootstrap/test_machine_ldop.fd
#   * x86_64-linux with FID_VERIFY=1: the MIR dump of xor_y, and_z, wsum,
#     val_and_mask shows an `alum` operation (ng_fuse_ldop: a load read
#     once by an ALU operation as its memory operand); wsub_left (SUB with
#     the load on the left) and checked_add (a checked add reads its
#     operands twice) show none;
#   * aarch64-linux and riscv64-linux never form it (load/store machines);
#   * the program prints the test's expected lines with the pass on and
#     with FID_NAT_OFF=ldop (x86-64 under WSL or on a Linux host; --nat on
#     a Windows host);
#   * FID_NAT_MUTATE=ldop (the wrong operation on purpose) changes the
#     output: the test sees the fold.
# - OP21 / PL5: tests/nattools/enc-oracle-op21.py — the vector forms (SSE2,
#   AVX2, NEON, RVV) and the Thumb-2 forms of compiler/nat_enc.fd agree
#   with clang's assembler byte for byte.
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
ZIG="${2:-zig}"
OUT="${TMPDIR:-/tmp}/fid-machine-batch9"
mkdir -p "$OUT"
PY=python3
command -v python3 >/dev/null 2>&1 || PY=python
HOST=linux
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; Darwin) HOST=macos ;; esac
RUN=""
if [ "$HOST" = windows ] && command -v wsl.exe >/dev/null 2>&1; then RUN="wsl.exe -e"; fi
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

wslpath_of() {
    local p
    p=$(cd "$(dirname "$1")" && pwd)/$(basename "$1")
    echo "/mnt/$(echo "$p" | sed -E 's|^/([a-zA-Z])/|\L\1/|')"
}

run_img() {
    if [ -n "$RUN" ]; then
        MSYS_NO_PATHCONV=1 $RUN "$(wslpath_of "$1")"
    else
        "$1"
    fi
}

T=tests/bootstrap/test_machine_ldop.fd
WANT=$(expected "$T" | tr -d '\r')

# --- OP18: the alum operations in the MIR -----------------------------------
for fn in xor_y and_z wsum val_and_mask hash_node; do
    FID_VERIFY=1 FID_NAT_DUMP=$fn "$FID" emit "$T" --target x86_64-linux -o "$OUT/ldop-x64" >"$OUT/dump-$fn.log" 2>&1
    if ! grep -q "^\[mir\] $fn: [0-9]* operations, frame" "$OUT/dump-$fn.log"; then
        # (a small function that is not @wrapping is inlined into main)
        echo "skip OP18 x86-64 $fn: inlined (no MIR of its own)"
    elif grep -q "after memory-operand selection" "$OUT/dump-$fn.log" && grep -q "^\[mir\] [0-9]*: alum" "$OUT/dump-$fn.log"; then
        ok "OP18 x86-64 $fn: a load read as a memory operand"
    else
        bad "OP18 x86-64 $fn: no alum operation (see $OUT/dump-$fn.log)"
    fi
done
for fn in wsub_left checked_add; do
    FID_NAT_DUMP=$fn "$FID" emit "$T" --target x86_64-linux -o "$OUT/ldop-x64" >"$OUT/dump-$fn.log" 2>&1
    if ! grep -q "^\[mir\] $fn: [0-9]* operations, frame" "$OUT/dump-$fn.log"; then
        echo "skip OP18 x86-64 $fn: inlined (no MIR of its own)"
    elif grep -q "^\[mir\] [0-9]*: alum" "$OUT/dump-$fn.log"; then
        bad "OP18 x86-64 $fn: formed an alum it must not"
    else
        ok "OP18 x86-64 $fn: no memory operand (as required)"
    fi
done
for tgt in aarch64-linux riscv64-linux; do
    FID_NAT_DUMP=xor_y "$FID" emit "$T" --target $tgt -o "$OUT/ldop-$tgt" >"$OUT/dump-$tgt.log" 2>&1
    if grep -q "^\[mir\] [0-9]*: alum" "$OUT/dump-$tgt.log"; then
        bad "OP18 $tgt: formed an alum (x86-64 only)"
    else
        ok "OP18 $tgt: no memory operands"
    fi
done

# --- OP18: the results, on and off ------------------------------------------
if [ "$HOST" = linux ] || [ -n "$RUN" ]; then
    for mode in on off mutant; do
        img="$OUT/ldop-$mode"
        case $mode in
            on) FID_VERIFY=1 "$FID" emit "$T" --target x86_64-linux -o "$img" >"$img.log" 2>&1 ;;
            off) FID_NAT_OFF=ldop "$FID" emit "$T" --target x86_64-linux -o "$img" >"$img.log" 2>&1 ;;
            mutant) FID_NAT_MUTATE=ldop "$FID" emit "$T" --target x86_64-linux -o "$img" >"$img.log" 2>&1 ;;
        esac
        got=$(run_img "$img" 2>&1 | tr -d '\r')
        if [ $mode = mutant ]; then
            if [ "$got" != "$WANT" ]; then ok "OP18 x86-64 mutant: the output changes"; else bad "OP18 x86-64 mutant: the output did not change"; fi
        elif [ "$got" = "$WANT" ]; then
            ok "OP18 x86-64 ldop $mode: expected output"
        else
            bad "OP18 x86-64 ldop $mode: got $(echo "$got" | tr '\n' ' ')"
        fi
    done
fi
if [ "$HOST" = windows ]; then
    img="$OUT/ldop-nat.exe"
    FID_VERIFY=1 "$FID" emit "$T" --nat -o "$img" >"$img.log" 2>&1
    got=$("$img" 2>&1 | tr -d '\r')
    if [ "$got" = "$WANT" ]; then ok "OP18 --nat: expected output"; else bad "OP18 --nat: got $(echo "$got" | tr '\n' ' ')"; fi
fi

# --- OP21 / PL5: vector and Thumb-2 encoders against clang --------------------
if r=$($PY tests/nattools/enc-oracle-op21.py --fid "$FID" --zig "$ZIG" 2>&1 | tail -1); then
    ok "OP21/PL5 encoders: $r"
else
    bad "OP21/PL5 encoders: $r"
fi

echo "$fails failure(s)"
[ $fails -eq 0 ]
