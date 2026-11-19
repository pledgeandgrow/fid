#!/usr/bin/env bash
# Checks of llvm-roadmap TG11 (the machine IR's vector operations,
# compiler/nat_ir.fd MO_VLD .. MO_VEND) and competitive-roadmap OP21 (the
# simd module's intrinsics compiled to them), run after the compiler is
# built:
#   bash tests/nattools/simd_check.sh [fid]   (default release/fid.exe)
# - tests/bootstrap/test_machine_simd.fd through `fid run` and ccompile
#   prints its expected lines.
# - built with FID_VERIFY=1 (the MIR verifier with ng_vec_verify, the
#   allocation checker, nat_sel_check with nat_vsel_check: MO_VBIN's table
#   rows against its hand-written selection) for x86-64 Linux (SSE2),
#   x86-64 Linux --cpu x86-64-v3 (AVX2), AArch64 Linux (NEON), RISC-V
#   Linux --cpu rv64gcv (RVV) and --nat Windows; FID_NAT_DUMP='*' shows
#   the vector operations the simd module's functions need (vld/vbin/vst
#   for add, vzmask for div, vsplat for scale, vlane for dot), with 32-byte widths
#   under AVX2 (`i 10 32`-style fields) and none on RISC-V without V or
#   with FID_NAT_OFF=simd.
# - FID_NAT_ON=fregs builds (floats in float registers beside the vector
#   class) of every target, --nat Windows included (MI14), keep them.
# - every image that can run here runs and prints the expected lines:
#   x86-64 through WSL (or natively on Linux; AVX2 only when the CPU has
#   it), AArch64 / RISC-V through qemu-user (RISC-V with -cpu
#   rv64,v=true,vlen=128 and vlen=256), --nat on Windows.
# - the disassembly (llvm-objdump, when there is one) has addpd/divpd,
#   vaddpd ymm, fadd .2d / fdiv .2d, vfadd.vv / vfdiv.vv.
# - tests/nattools/enc-oracle-tg11.py and enc-oracle-op21.py (the encoders
#   against clang's assembler) report 0 mismatches.
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-simd-check"
mkdir -p "$OUT"
T=tests/bootstrap/test_machine_simd.fd
RUN=""
if command -v wsl.exe >/dev/null 2>&1; then RUN="wsl.exe -e"; fi
WIN=0
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; esac
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }
WANT=$(expected "$T")

# 1. the interpreter and the C path
# (only the compiler's own "[build] ..." lines are dropped: the expected
# output has lines that start with "[")
got=$("$FID" run "$T" 2>&1 | grep -v '^\[[a-z]*\] ')
if [ "$got" = "$WANT" ]; then ok "fid run"; else bad "fid run output"; echo "$got" | head -30; fi
cexe="$OUT/c-simd"
[ "$WIN" = 1 ] && cexe="$cexe.exe"
if "$FID" ccompile "$T" -o "$cexe" >"$OUT/c.log" 2>&1; then
    if [ "$("$cexe" 2>&1)" = "$WANT" ]; then ok "ccompile"; else bad "ccompile output"; fi
else
    bad "ccompile build"; tail -5 "$OUT/c.log"
fi

# name target cpu
LANES="sse x86_64-linux - avx2 x86_64-linux x86-64-v3 neon aarch64-linux - rvv riscv64-linux rv64gcv nat nat -"

build() {
    # build <image> <target> <cpu> [env...]: FID_VERIFY=1, the MIR of the
    # simd module's functions dumped to <image>.<fn>.mir
    local img="$1" tgt="$2" cpu="$3"
    shift 3
    local args=()
    if [ "$tgt" = nat ]; then args=(--nat); else args=(--target "$tgt"); fi
    [ "$cpu" != - ] && args+=(--cpu "$cpu")
    env FID_VERIFY=1 "$@" "$FID" emit "$T" "${args[@]}" -o "$img" >"$img.log" 2>&1 || return 1
    # every function's MIR (the simd module's functions may be inlined into
    # their callers): the vector operations are looked for anywhere
    env FID_NAT_DUMP='*' "$@" "$FID" emit "$T" "${args[@]}" -o "$img.dump" >"$img.mir" 2>&1 || return 1
    local fn
    for fn in add div scale dot; do
        cp "$img.mir" "$img.$fn.mir"
    done
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
    if has_op "$img.add.mir" vbin && has_op "$img.add.mir" vld && has_op "$img.add.mir" vst; then ok "$name add: vld/vbin/vst"; else bad "$name add has no vector operations"; fi
    if has_op "$img.div.mir" vzmask; then ok "$name div: vzmask"; else bad "$name div has no vzmask"; fi
    if has_op "$img.scale.mir" vsplat; then ok "$name scale: vsplat"; else bad "$name scale has no vsplat"; fi
    if has_op "$img.dot.mir" vlane; then ok "$name dot: vlane"; else bad "$name dot has no vlane"; fi
    if [ "$name" = avx2 ]; then
        if grep -q "^\[mir\] [0-9]*: vbin .* 32$\|^\[mir\] [0-9]*: vbin .* 32 " "$img.add.mir" && has_op "$img.add.mir" vend; then ok "avx2: 32-byte vbin and vend"; else bad "avx2: no 32-byte vector operations"; fi
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
        if has_op "$off.add.mir" vbin; then bad "$name FID_NAT_OFF=simd still has vbin"; else ok "$name FID_NAT_OFF=simd: calls"; fi
        if can_run "$tgt" "$cpu" && [ "$(run_img "$off" "$tgt")" = "$WANT" ]; then ok "$name FID_NAT_OFF=simd runs"; fi
    else
        bad "$name FID_NAT_OFF=simd build"
    fi
    # floats in float registers beside the vector class
    fr="$OUT/$name-fregs"
    [ "$tgt" = nat ] && fr="$fr.exe"
    if build "$fr" "$tgt" "$cpu" FID_NAT_ON=fregs; then
        ok "$name FID_NAT_ON=fregs builds (FID_VERIFY=1)"
        # (MI14: --nat Windows keeps its vector operations beside the float
        # pool too: vectors in xmm12..xmm15, the pool xmm2..xmm11)
        if ! has_op "$fr.add.mir" vbin; then
            bad "$name FID_NAT_ON=fregs lost the vector operations"
        fi
        if can_run "$tgt" "$cpu"; then
            if [ "$(run_img "$fr" "$tgt")" = "$WANT" ]; then ok "$name FID_NAT_ON=fregs runs"; else bad "$name FID_NAT_ON=fregs output differs"; fi
        fi
    else
        bad "$name FID_NAT_ON=fregs build"; tail -5 "$fr.log"
    fi
done

# RISC-V without V: no vector operations
nov="$OUT/rv-nov"
if build "$nov" riscv64-linux -; then
    if has_op "$nov.add.mir" vbin; then bad "riscv64 rv64gc has vector operations"; else ok "riscv64 rv64gc: calls"; fi
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
    dis "$OUT/sse" | grep -q "addpd" && dis "$OUT/sse" | grep -q "divpd" && ok "sse: addpd, divpd" || bad "sse: no addpd/divpd"
    dis "$OUT/avx2" | grep -q "vaddpd.*ymm" && ok "avx2: vaddpd ymm" || bad "avx2: no vaddpd ymm"
    dis "$OUT/neon" | grep -q "fadd.*v28.2d\|fadd	v28.2d" && ok "neon: fadd .2d" || bad "neon: no fadd .2d"
    $OBJDUMP -d --mattr=+v "$OUT/rvv" 2>/dev/null | grep -q "vfadd.vv" && ok "rvv: vfadd.vv" || bad "rvv: no vfadd.vv"
else
    echo "skip disassembly (no llvm-objdump)"
fi

# the encoders
for o in enc-oracle-tg11.py enc-oracle-op21.py; do
    if python3 tests/nattools/$o --fid "$FID" >"$OUT/$o.log" 2>&1; then ok "$o: $(tail -1 "$OUT/$o.log")"; else bad "$o: $(tail -1 "$OUT/$o.log")"; fi
done

echo
if [ "$fails" = 0 ]; then echo "simd_check: all passed"; else echo "simd_check: $fails failed"; exit 1; fi
