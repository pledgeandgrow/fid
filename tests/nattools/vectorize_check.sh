#!/usr/bin/env bash
# Checks of competitive-roadmap OP22, automatic loop vectorization on the
# machine layer (compiler/nat_ir.fd, section "OP22: automatic loop
# vectorization", ng_vz_*), run after the compiler is built:
#   bash tests/nattools/vectorize_check.sh [fid]   (default release/fid.exe)
# - tests/bootstrap/test_vectorize.fd through `fid run` and ccompile prints
#   its expected lines (every loop as written).
# - built with FID_VERIFY=1 and --stats for x86-64 Linux (SSE2), x86-64
#   Linux --cpu x86-64-v3 (AVX2), AArch64 Linux (NEON), RISC-V Linux --cpu
#   rv64gcv (RVV) and --nat Windows: a `[vectorize] <fn>: ... vectorized`
#   line for each loop the vectorizer must take (TAKEN below) and none for
#   those it must leave alone (LEFT: a checked +, an f64 sum, a read after
#   a write, the counter in the arithmetic); their MIR (FID_NAT_DUMP) has
#   vld/vbin/vst; under AVX2 the elementwise loops use 32-byte vectors and
#   the reductions 16-byte ones.
# - every image that can run here prints the expected lines (WSL for
#   x86-64, qemu-user for AArch64 and RISC-V at VLEN 128 and 256, --nat
#   natively on Windows); FID_NAT_OFF=vectorize: no `vectorized` line, no
#   vbin in the MIR of TAKEN functions, the same lines; RISC-V without V:
#   no `vectorized` line.
# Lines marked "skip" (no runner here) are not failures. Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-vectorize-check"
mkdir -p "$OUT"
T=tests/bootstrap/test_vectorize.fd
TAKEN="xor_all or_range and_all sum_wrap add_f saxpy div_sub scale mix bits fill"
LEFT="add_checked fsum raw ctr hash fhash"
ELEMWISE="add_f saxpy div_sub scale mix bits fill"
REDUCE="xor_all or_range and_all sum_wrap"
RUN=""
if command -v wsl.exe >/dev/null 2>&1; then RUN="wsl.exe -e"; fi
WIN=0
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; esac
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }
WANT=$(expected "$T")
strip() { grep -v '^\[[a-z0-9]*\] '; }

# 1. the interpreter and the C path
got=$("$FID" run "$T" 2>&1 | strip)
if [ "$got" = "$WANT" ]; then ok "fid run"; else bad "fid run output"; diff <(echo "$WANT") <(echo "$got") | head -10; fi
cexe="$OUT/c-vec"
[ "$WIN" = 1 ] && cexe="$cexe.exe"
if "$FID" ccompile "$T" -o "$cexe" >"$OUT/c.log" 2>&1; then
    if [ "$("$cexe" 2>&1)" = "$WANT" ]; then ok "ccompile"; else bad "ccompile output"; fi
else
    bad "ccompile build"; tail -5 "$OUT/c.log"
fi

# name target cpu
LANES="sse x86_64-linux - avx2 x86_64-linux x86-64-v3 neon aarch64-linux - rvv riscv64-linux rv64gcv nat nat -"

build() {
    # build <image> <target> <cpu> [env...]: FID_VERIFY=1 and --stats (the
    # [vectorize] lines to <image>.log), then the TAKEN functions' MIR to
    # <image>.mir
    local img="$1" tgt="$2" cpu="$3"
    shift 3
    local args=()
    if [ "$tgt" = nat ]; then args=(--nat); else args=(--target "$tgt"); fi
    [ "$cpu" != - ] && args+=(--cpu "$cpu")
    env FID_VERIFY=1 "$@" "$FID" emit "$T" "${args[@]}" --stats -o "$img" >"$img.log" 2>&1 || return 1
    env FID_NAT_DUMP='*' "$@" "$FID" emit "$T" "${args[@]}" -o "$img.dump" >"$img.mir" 2>&1 || return 1
    return 0
}

taken() { grep -q "^\[vectorize\] $2: bb[0-9]* vectorized" "$1"; }
fn_has_op() {
    # fn_has_op <mir> <fn> <op>: the function's listing has the operation
    # (a listing starts with "[mir] <fn>: N operations" or "[mir] <fn>
    # after <stage>: N operations", then "[mir] <k>: <op> ..." lines)
    awk -v f="$2" -v op="$3" '
        /^\[mir\] / {
            if ($2 ~ /^[0-9]+:$/) { if (cur == f && $3 == op) found = 1 }
            else { cur = $2; sub(":$", "", cur) }
        }
        END { exit found ? 0 : 1 }' "$1"
}

run_img() {
    local img="$1" tgt="$2" extra="${3:-}"
    case "$tgt" in
        nat) "$img" 2>&1 ;;
        x86_64-linux)
            if [ -n "$RUN" ]; then $RUN "$img" 2>&1; else "$img" 2>&1; fi ;;
        aarch64-linux) $RUN qemu-aarch64 "$img" 2>&1 ;;
        riscv64-linux)
            if [ -n "$extra" ]; then $RUN qemu-riscv64 -cpu "rv64,v=true,vlen=$extra" "$img" 2>&1
            else $RUN qemu-riscv64 "$img" 2>&1; fi ;;
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
    grep "^\[vectorize\]" "$img.log" > "$OUT/$name.vz"
    miss=""
    for f in $TAKEN; do taken "$img.log" "$f" || miss="$miss $f"; done
    if [ -z "$miss" ]; then ok "$name: every TAKEN loop vectorized"; else bad "$name: not vectorized:$miss"; grep -E "\] ($(echo $miss | tr ' ' '|')):" "$img.log" | head -10; fi
    wrong=""
    for f in $LEFT; do taken "$img.log" "$f" && wrong="$wrong $f"; done
    if [ -z "$wrong" ]; then ok "$name: no LEFT loop vectorized"; else bad "$name: vectorized what it must not:$wrong"; fi
    nomir=""
    for f in $TAKEN; do
        fn_has_op "$img.mir" "$f" vld && fn_has_op "$img.mir" "$f" vbin || nomir="$nomir $f"
    done
    if [ -z "$nomir" ]; then ok "$name: vld/vbin in every TAKEN function"; else bad "$name: no vector operations in:$nomir"; fi
    if [ "$name" = avx2 ]; then
        w32=""
        for f in $ELEMWISE; do grep -q "^\[vectorize\] $f: .* 32-byte vectors" "$img.log" || w32="$w32 $f"; done
        w16=""
        for f in $REDUCE; do grep -q "^\[vectorize\] $f: .* 16-byte vectors" "$img.log" || w16="$w16 $f"; done
        if [ -z "$w32$w16" ]; then ok "avx2: 32-byte elementwise, 16-byte reductions"; else bad "avx2 widths:$w32 /$w16"; fi
        fn_has_op "$img.mir" add_f vend && ok "avx2: vend after 256-bit code" || bad "avx2: no vend in add_f"
    fi
    if can_run "$tgt" "$cpu"; then
        if [ "$(run_img "$img" "$tgt" "$([ "$tgt" = riscv64-linux ] && echo 128)" | strip)" = "$WANT" ]; then ok "$name runs"; else bad "$name output differs"; run_img "$img" "$tgt" 128 | strip | diff <(echo "$WANT") - | head -10; fi
        if [ "$tgt" = riscv64-linux ]; then
            if [ "$(run_img "$img" "$tgt" 256 | strip)" = "$WANT" ]; then ok "$name runs at VLEN 256"; else bad "$name output differs at VLEN 256"; fi
        fi
    else
        echo "skip $name run (no runner here)"
    fi
    # the switch off: nothing vectorized, the same output
    off="$OUT/$name-off"
    [ "$tgt" = nat ] && off="$off.exe"
    if build "$off" "$tgt" "$cpu" FID_NAT_OFF=vectorize; then
        if grep -q "^\[vectorize\] .* vectorized" "$off.log"; then bad "$name FID_NAT_OFF=vectorize still vectorizes"; else ok "$name FID_NAT_OFF=vectorize: none"; fi
        if fn_has_op "$off.mir" add_f vbin || fn_has_op "$off.mir" xor_all vbin; then bad "$name FID_NAT_OFF=vectorize: vbin left"; else ok "$name FID_NAT_OFF=vectorize: no vbin"; fi
        if can_run "$tgt" "$cpu"; then
            if [ "$(run_img "$off" "$tgt" "$([ "$tgt" = riscv64-linux ] && echo 128)" | strip)" = "$WANT" ]; then ok "$name FID_NAT_OFF=vectorize runs"; else bad "$name FID_NAT_OFF=vectorize output differs"; fi
        fi
    else
        bad "$name FID_NAT_OFF=vectorize build"
    fi
done

# RISC-V without V: nothing vectorized, the same output
nov="$OUT/rv-nov"
if build "$nov" riscv64-linux -; then
    if grep -q "^\[vectorize\] .* vectorized" "$nov.log"; then bad "riscv64 rv64gc vectorized"; else ok "riscv64 rv64gc: nothing vectorized"; fi
    if can_run riscv64-linux -; then
        if [ "$(run_img "$nov" riscv64-linux | strip)" = "$WANT" ]; then ok "riscv64 rv64gc runs"; else bad "riscv64 rv64gc output differs"; fi
    fi
else
    bad "riscv64 rv64gc build"
fi

echo
echo "the [vectorize] lines of the x86-64 build: $OUT/sse.vz"
if [ "$fails" = 0 ]; then echo "vectorize_check: all passed"; else echo "vectorize_check: $fails failed"; exit 1; fi
