#!/usr/bin/env bash
# Checks of competitive-roadmap OP21's integer SIMD: the simd module's
# Vec<i64> functions (stdlib/simd.fd add_i64 .. xor_i64, Fides loops
# simd_lanes_*_i64 wrapping at 64 bits) compiled by `fid emit` to vector
# loops (compiler/nat_ir.fd ng_simd_call, mode 3), run after the compiler
# is built:
#   bash tests/nattools/simd_i64_check.sh [fid]   (default release/fid.exe)
# - tests/bootstrap/test_simd_i64.fd through `fid run` and ccompile prints
#   its expected lines.
# - built with FID_VERIFY=1 for x86-64 Linux (SSE2), x86-64 Linux --cpu
#   x86-64-v3 (AVX2), AArch64 Linux (NEON), RISC-V Linux --cpu rv64gcv
#   (RVV) and --nat Windows; FID_NAT_DUMP='*' has vld/vbin/vst (the test
#   has no f64 intrinsics, so every vbin is an integer lane operation),
#   32-byte vbin and vend under AVX2.
# - every image that can run here prints the expected lines (WSL for
#   x86-64, qemu-user for AArch64 and RISC-V at VLEN 128 and 256, --nat
#   natively on Windows); FID_NAT_OFF=simd gives no vbin and the same lines;
#   RISC-V without --cpu has no vbin.
# - the disassembly (llvm-objdump, when there is one) has paddq and pxor,
#   vpaddq ymm, add .2d and eor .16b, vadd.vv and vxor.vv.
# Lines marked "skip" (no runner or objdump here) are not failures.
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-simd-i64-check"
mkdir -p "$OUT"
T=tests/bootstrap/test_simd_i64.fd
RUN=""
if command -v wsl.exe >/dev/null 2>&1; then RUN="wsl.exe -e"; fi
WIN=0
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; esac
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }
WANT=$(expected "$T")

# 1. the interpreter and the C path (the Fides loops)
got=$("$FID" run "$T" 2>&1 | grep -v '^\[[a-z]*\] ')
if [ "$got" = "$WANT" ]; then ok "fid run"; else bad "fid run output"; diff <(echo "$WANT") <(echo "$got") | head -10; fi
cexe="$OUT/c-simd-i64"
[ "$WIN" = 1 ] && cexe="$cexe.exe"
if "$FID" ccompile "$T" -o "$cexe" >"$OUT/c.log" 2>&1; then
    if [ "$("$cexe" 2>&1)" = "$WANT" ]; then ok "ccompile"; else bad "ccompile output"; fi
else
    bad "ccompile build"; tail -5 "$OUT/c.log"
fi
# pe_ir, the default Windows emit (calls of the Fides loops)
if [ "$WIN" = 1 ]; then
    if "$FID" emit "$T" -o "$OUT/pe.exe" >"$OUT/pe.log" 2>&1; then
        if [ "$("$OUT/pe.exe" 2>&1)" = "$WANT" ]; then ok "emit (pe_ir)"; else bad "emit (pe_ir) output"; fi
    else
        bad "emit (pe_ir) build"; tail -5 "$OUT/pe.log"
    fi
fi

# name target cpu
LANES="sse x86_64-linux - avx2 x86_64-linux x86-64-v3 neon aarch64-linux - rvv riscv64-linux rv64gcv nat nat -"

build() {
    # build <image> <target> <cpu> [env...]: FID_VERIFY=1, then every
    # function's MIR dumped to <image>.mir
    local img="$1" tgt="$2" cpu="$3"
    shift 3
    local args=()
    if [ "$tgt" = nat ]; then args=(--nat); else args=(--target "$tgt"); fi
    [ "$cpu" != - ] && args+=(--cpu "$cpu")
    env FID_VERIFY=1 "$@" "$FID" emit "$T" "${args[@]}" -o "$img" >"$img.log" 2>&1 || return 1
    env FID_NAT_DUMP='*' "$@" "$FID" emit "$T" "${args[@]}" -o "$img.dump" >"$img.mir" 2>&1 || return 1
    return 0
}

has_op() { grep -q "^\[mir\] [0-9]*: $2 " "$1"; }

run_img() {
    local img="$1" tgt="$2" extra="${3:-}"
    case "$tgt" in
        nat) "$img" 2>&1 ;;
        x86_64-linux)
            if [ -n "$RUN" ]; then $RUN "$img" 2>&1; else "$img" 2>&1; fi ;;
        aarch64-linux) $RUN qemu-aarch64 "$img" 2>&1 ;;
        riscv64-linux) $RUN qemu-riscv64 -cpu "rv64,v=true,vlen=${extra:-128}" "$img" 2>&1 ;;
    esac
}

can_run() {
    local tgt="$1" cpu="$2"
    case "$tgt" in
        nat) [ "$WIN" = 1 ] ;;
        x86_64-linux)
            if [ -z "$RUN" ] && { [ "$(uname -s)" != Linux ] || [ "$(uname -m)" != x86_64 ]; }; then return 1; fi
            if [ "$cpu" = x86-64-v3 ]; then $RUN grep -q avx2 /proc/cpuinfo; else return 0; fi ;;
        aarch64-linux) $RUN sh -c 'command -v qemu-aarch64' >/dev/null 2>&1 ;;
        riscv64-linux) $RUN sh -c 'command -v qemu-riscv64' >/dev/null 2>&1 ;;
    esac
}

set -- $LANES
while [ $# -ge 3 ]; do
    name="$1" tgt="$2" cpu="$3"
    shift 3
    img="$OUT/$name"
    [ "$tgt" = nat ] && img="$img.exe"
    if build "$img" "$tgt" "$cpu"; then ok "$name builds (FID_VERIFY=1)"; else bad "$name build"; tail -5 "$img.log"; continue; fi
    if has_op "$img.mir" vbin && has_op "$img.mir" vld && has_op "$img.mir" vst; then ok "$name: vld/vbin/vst"; else bad "$name has no integer vector operations"; fi
    if [ "$name" = avx2 ]; then
        if grep -q "^\[mir\] [0-9]*: vbin .* 32$\|^\[mir\] [0-9]*: vbin .* 32 " "$img.mir" && has_op "$img.mir" vend; then ok "avx2: 32-byte vbin and vend"; else bad "avx2: no 32-byte vector operations"; fi
    fi
    if can_run "$tgt" "$cpu"; then
        if [ "$(run_img "$img" "$tgt")" = "$WANT" ]; then ok "$name runs"; else bad "$name output differs"; run_img "$img" "$tgt" | diff <(echo "$WANT") - | head -10; fi
        if [ "$tgt" = riscv64-linux ]; then
            if [ "$(run_img "$img" "$tgt" 256)" = "$WANT" ]; then ok "$name runs at VLEN 256"; else bad "$name output differs at VLEN 256"; fi
        fi
    else
        echo "skip $name run (no runner here)"
    fi
    # the switch off: no vector operations, same output
    off="$OUT/$name-off"
    [ "$tgt" = nat ] && off="$off.exe"
    if build "$off" "$tgt" "$cpu" FID_NAT_OFF=simd; then
        if has_op "$off.mir" vbin; then bad "$name FID_NAT_OFF=simd still has vbin"; else ok "$name FID_NAT_OFF=simd: calls"; fi
        if can_run "$tgt" "$cpu"; then
            if [ "$(run_img "$off" "$tgt")" = "$WANT" ]; then ok "$name FID_NAT_OFF=simd runs"; else bad "$name FID_NAT_OFF=simd output differs"; fi
        fi
    else
        bad "$name FID_NAT_OFF=simd build"
    fi
done

# RISC-V without V: no vector operations, same output
nov="$OUT/rv-nov"
if build "$nov" riscv64-linux -; then
    if has_op "$nov.mir" vbin; then bad "riscv64 rv64gc has vector operations"; else ok "riscv64 rv64gc: calls"; fi
    if can_run riscv64-linux -; then
        if [ "$(run_img "$nov" riscv64-linux)" = "$WANT" ]; then ok "riscv64 rv64gc runs"; else bad "riscv64 rv64gc output differs"; fi
    fi
else
    bad "riscv64 rv64gc build"
fi

# the instructions in the images
OBJDUMP=""
for c in llvm-objdump llvm-objdump-18 llvm-objdump-17 llvm-objdump-16; do
    if command -v $c >/dev/null 2>&1; then OBJDUMP=$c; break; fi
done
if [ -n "$OBJDUMP" ]; then
    dis() { $OBJDUMP -d "$1" 2>/dev/null; }
    dis "$OUT/sse" | grep -q "paddq" && dis "$OUT/sse" | grep -q "pxor" && ok "sse: paddq, pxor" || bad "sse: no paddq/pxor"
    dis "$OUT/avx2" | grep -q "vpaddq.*ymm" && ok "avx2: vpaddq ymm" || bad "avx2: no vpaddq ymm"
    dis "$OUT/neon" | grep -q "add.*v28.2d" && dis "$OUT/neon" | grep -q "eor.*v28.16b" && ok "neon: add .2d, eor .16b" || bad "neon: no add .2d / eor .16b"
    $OBJDUMP -d --mattr=+v "$OUT/rvv" 2>/dev/null | grep -q "vadd.vv" && $OBJDUMP -d --mattr=+v "$OUT/rvv" 2>/dev/null | grep -q "vxor.vv" && ok "rvv: vadd.vv, vxor.vv" || bad "rvv: no vadd.vv / vxor.vv"
else
    echo "skip disassembly (no llvm-objdump)"
fi

echo
if [ "$fails" = 0 ]; then echo "simd_i64_check: all passed"; else echo "simd_i64_check: $fails failed"; exit 1; fi
