#!/usr/bin/env bash
# OP18 (competitive-roadmap): memory operands in compiler/nat_ir.fd beyond
# MO_ALUM, run after the compiler is built:
#   bash tests/nattools/machine_memop_check.sh [fid] [zig]
#   (defaults release/fid.exe, zig)
# - tests/nattools/memop_unit.fd: the folds on small MIR sequences (every
#   target) print "ok".
# - tests/bootstrap/test_machine_memop.fd on x86_64-linux with
#   FID_VERIFY=1 and FID_NAT_DUMP='*': each compare function's final MIR
#   has a `brccm` (or `setccm`), each read-modify-write function an
#   `alumst`, each extending-load function no `ext`; the cases that must
#   stay (after_store, sub_right, ld_twice, ld_store) do; with
#   FID_NAT_OFF=memcmp,rmw,ldext none of the new operations appears.
# - aarch64-linux and riscv64-linux: no brccm / setccm / alumst anywhere,
#   and the extending-load functions have no `ext`.
# - The images print the test's expected lines with the folds on and off
#   (x86-64 in WSL or on a Linux host, AArch64 / RISC-V under qemu-user;
#   --nat on a Windows host), and each of FID_NAT_MUTATE=memcmp, rmw,
#   ldext (the fold made wrong on purpose) changes the x86-64 output.
# - tests/nattools/enc-oracle-op18.py --only: the encoders against clang.
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
ZIG="${2:-zig}"
OUT="${TMPDIR:-/tmp}/fid-machine-memop"
mkdir -p "$OUT"
PY=python3
command -v python3 >/dev/null 2>&1 || PY=python
HOST=linux
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; Darwin) HOST=macos ;; esac
WSL=""
if [ "$HOST" = windows ] && command -v wsl.exe >/dev/null 2>&1; then WSL="wsl.exe -e"; fi
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

T=tests/bootstrap/test_machine_memop.fd
expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }
WANT=$(expected "$T" | tr -d '\r')

wslpath_of() {
    local p
    p=$(cd "$(dirname "$1")" && pwd)/$(basename "$1")
    echo "/mnt/$(echo "$p" | sed -E 's|^/([a-zA-Z])/|\L\1/|')"
}

# run_img <image> <target>: the image's output, "" when it cannot run here
run_img() {
    local q=""
    case "$2" in
        aarch64-linux) q=qemu-aarch64 ;;
        riscv64-linux) q=qemu-riscv64 ;;
    esac
    if [ -n "$WSL" ]; then
        MSYS_NO_PATHCONV=1 $WSL $q "$(wslpath_of "$1")" 2>&1
    elif [ "$HOST" = linux ]; then
        if [ -n "$q" ]; then
            command -v "$q" >/dev/null 2>&1 || return 0
            "$q" "$1" 2>&1
        else
            "$1" 2>&1
        fi
    fi
}

# count <dump> <function> <op>: operations `op` in the function's final MIR
# (-1: the function has no MIR of its own)
count() {
    awk -v fn="$2" -v op="$3" '
        index($0, "[mir] " fn ": ") == 1 && index($0, " operations, frame ") > 0 { p = 1; seen = 1; next }
        p && /^\[mir\] [^0-9]/ { p = 0 }
        p && index($0, ": " op " ") > 0 { n++ }
        END { if (!seen) print -1; else print n + 0 }' "$1"
}

# --- the unit checks ---------------------------------------------------------
u=$("$FID" run tests/nattools/memop_unit.fd 2>&1 | grep -v '^\[' | tr -d '\r')
if [ "$u" = "$(printf 'a64 ok\nrv64 ok\nx64 ok')" ]; then ok "memop_unit: every target"; else bad "memop_unit: $(echo "$u" | tr '\n' ' ')"; fi

# --- x86-64: the operations in the MIR -----------------------------------------
D="$OUT/x64-on.log"
FID_VERIFY=1 FID_NAT_DUMP='*' "$FID" emit "$T" --target x86_64-linux -o "$OUT/x64-on" >"$D" 2>&1 || bad "x86_64-linux build (FID_VERIFY=1) failed: see $D"
expect_op() {  # <dump> <op> <want: some|none> <function>...
    local d=$1 op=$2 want=$3
    shift 3
    for fn in "$@"; do
        local n
        n=$(count "$d" "$fn" "$op")
        if [ "$n" = -1 ]; then
            echo "skip $fn: no MIR of its own (inlined)"
        elif [ "$want" = some ] && [ "$n" -gt 0 ]; then
            ok "$fn: $n $op"
        elif [ "$want" = none ] && [ "$n" -eq 0 ]; then
            ok "$fn: no $op"
        else
            bad "$fn: $n $op (wanted $want; see $d)"
        fi
    done
}
expect_op "$D" brccm some below ge_left eq42 below_neg is_big tiny_neg byte_high half_m3 uhalf_ge small_gt word_gt flag_val
expect_op "$D" setccm some more_than same_b
expect_op "$D" brccm none after_store
expect_op "$D" alumst some bump bump5 take set_bits flip keep_mid bump_big mix_in
expect_op "$D" alumst none sub_right
expect_op "$D" ext none low8 low8s small16 low32u low32s byte_signed
expect_op "$D" ext some ld_twice ld_store

D0="$OUT/x64-off.log"
FID_NAT_OFF=memcmp,rmw,ldext FID_NAT_DUMP='*' "$FID" emit "$T" --target x86_64-linux -o "$OUT/x64-off" >"$D0" 2>&1 || bad "x86_64-linux build with the folds off failed"
for op in brccm setccm alumst; do
    if grep -q "^\[mir\] [0-9]*: $op " "$D0"; then bad "FID_NAT_OFF=memcmp,rmw,ldext: $op formed"; else ok "FID_NAT_OFF: no $op"; fi
done
expect_op "$D0" ext some low8 small16

# --- AArch64 / RISC-V: extending loads only ------------------------------------
for tgt in aarch64-linux riscv64-linux; do
    Dt="$OUT/$tgt.log"
    FID_VERIFY=1 FID_NAT_DUMP='*' "$FID" emit "$T" --target $tgt -o "$OUT/$tgt-on" >"$Dt" 2>&1 || bad "$tgt build failed: see $Dt"
    if grep -q "^\[mir\] [0-9]*: \(brccm\|setccm\|alumst\) " "$Dt"; then bad "$tgt: a memory operand formed"; else ok "$tgt: no memory operands"; fi
    expect_op "$Dt" ext none low8 low8s small16 low32u low32s byte_signed
    FID_NAT_OFF=ldext "$FID" emit "$T" --target $tgt -o "$OUT/$tgt-off" >"$OUT/$tgt-off.log" 2>&1 || bad "$tgt build with ldext off failed"
done

# --- the results ---------------------------------------------------------------
for tgt in x86_64-linux aarch64-linux riscv64-linux; do
    for mode in on off; do
        img="$OUT/$tgt-$mode"
        [ "$tgt" = x86_64-linux ] && img="$OUT/x64-$mode"
        got=$(run_img "$img" $tgt | tr -d '\r')
        if [ -z "$got" ]; then
            echo "skip $tgt $mode: cannot run here"
        elif [ "$got" = "$WANT" ]; then
            ok "$tgt $mode: expected output"
        else
            bad "$tgt $mode: got $(echo "$got" | tr '\n' '|')"
        fi
    done
done
for mu in memcmp rmw ldext; do
    FID_NAT_MUTATE=$mu "$FID" emit "$T" --target x86_64-linux -o "$OUT/x64-mu-$mu" >"$OUT/x64-mu-$mu.log" 2>&1
    got=$(run_img "$OUT/x64-mu-$mu" x86_64-linux | tr -d '\r')
    if [ -z "$got" ]; then
        echo "skip mutant $mu: cannot run here"
    elif [ "$got" != "$WANT" ]; then
        ok "FID_NAT_MUTATE=$mu: the output changes"
    else
        bad "FID_NAT_MUTATE=$mu: the output did not change (the fold is not taken)"
    fi
done
if [ "$HOST" = windows ]; then
    for mode in on off; do
        img="$OUT/memop-nat-$mode.exe"
        if [ $mode = on ]; then
            FID_VERIFY=1 "$FID" emit "$T" --nat -o "$img" >"$img.log" 2>&1
        else
            FID_NAT_OFF=memcmp,rmw,ldext "$FID" emit "$T" --nat -o "$img" >"$img.log" 2>&1
        fi
        got=$("$img" 2>&1 | tr -d '\r')
        if [ "$got" = "$WANT" ]; then ok "--nat $mode: expected output"; else bad "--nat $mode: got $(echo "$got" | tr '\n' '|')"; fi
    done
fi

# --- the encoders against clang ---------------------------------------------------
if r=$($PY tests/nattools/enc-oracle-op18.py --only --fid "$FID" --zig "$ZIG" 2>&1 | tail -1); then
    ok "OP18 encoders: $r"
else
    bad "OP18 encoders: $r"
fi

echo "$fails failure(s)"
[ $fails -eq 0 ]
