#!/usr/bin/env bash
# Checks of llvm-roadmap TG11's vector spill slots and vector liveness on the
# machine layer (compiler/nat_ir.fd: ng_vz_pins, ng_vz_vslot, ng_vz_vget /
# ng_vz_vput, ng_vz_frame_end; ng_vec_live_check, nat_vlive_unit_check),
# run after the compiler is built:
#   bash tests/nattools/vectorize_spill_check.sh [fid]   (default release/fid.exe)
# - tests/bootstrap/test_vectorize_spill.fd through `fid run` and ccompile
#   prints its expected lines.
# - built with FID_VERIFY=1 (the MIR verifier with the vector liveness check,
#   and nat_sel_check with nat_vlive_unit_check) and --stats for x86-64
#   Linux (SSE2), x86-64 Linux --cpu x86-64-v3 (AVX2), AArch64 Linux (NEON),
#   RISC-V Linux --cpu rv64gcv (RVV) and --nat Windows: every TAKEN loop has
#   a `[vectorize] <fn>: ... vectorized ... spilled to N vector slots` line
#   with N > 0; `deep` (18 values at once) has a `not vectorized` line
#   naming the 16 homes; the TAKEN functions' MIR has vld, vst and vbin and,
#   for wide/red6, a slotaddr (the slots' addresses).
# - every image that can run here prints the expected lines (WSL for
#   x86-64, qemu-user for AArch64 and RISC-V at VLEN 128 and 256, --nat
#   natively on Windows); so do FID_NAT_OFF=vzspill (the TAKEN loops then
#   stay scalar: no `vectorized` line for them) and FID_NAT_OFF=vectorize.
# Lines marked "skip" (no runner here) are not failures. Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-vectorize-spill-check"
mkdir -p "$OUT"
T=tests/bootstrap/test_vectorize_spill.fd
TAKEN="wide splats red6 redk many"
SLOTADDR="wide red6"
RUN=""
# A working WSL only: wsl.exe is on PATH on every Windows, with no
# distribution installed too, and then every image "fails" with no output
if command -v wsl.exe >/dev/null 2>&1 && wsl.exe -e sh -c 'echo wsl-ok' 2>/dev/null | grep -q 'wsl-ok'; then RUN="wsl.exe -e"; fi
WIN=0
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; esac
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }
WANT=$(expected "$T")
strip() { grep -v '^\[[a-z0-9]*\] '; }

# 0. the vector liveness check on small MIR sequences
u=$("$FID" run tests/nattools/vlive_unit.fd 2>&1 | grep -v '^\[' | tr -d '\r')
if [ "$u" = "vlive ok" ]; then ok "vlive_unit"; else bad "vlive_unit: $u"; fi

# 1. the interpreter and the C path
got=$("$FID" run "$T" 2>&1 | strip)
if [ "$got" = "$WANT" ]; then ok "fid run"; else bad "fid run output"; diff <(echo "$WANT") <(echo "$got") | head -10; fi
cexe="$OUT/c-spill"
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
    # [vectorize] lines to <image>.log), then the MIR to <image>.mir
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
spilled() { grep -E -q "^\[vectorize\] $2: bb[0-9]* vectorized.*spilled to [1-9][0-9]* vector slots" "$1"; }
# Since the Nov 27 inliner takes small functions whole, wide, splats and
# deep sit inside their caller `one` in the [vectorize] lines: the loops
# that are vectorized are counted over the whole program, and only the
# ones that stay functions (red6, redk, many) are looked for by name
spilled_any() { grep -c -E "^\[vectorize\] [A-Za-z0-9_]+: bb[0-9]+ vectorized.*spilled to [1-9][0-9]* vector slots" "$1"; }
# per function of a MIR dump that has vld: its vst, vbin and slotaddr counts
vec_fns() {
    awk '/^\[mir\] / {
            if ($2 ~ /^[0-9]+:$/) { op = $3; if (!(cur in seen)) { order[++n] = cur; seen[cur] = 1 } cnt[cur "," op]++ }
            else { cur = $2; sub(":$", "", cur) }
         }
         END { for (i = 1; i <= n; i++) { f = order[i]; if (cnt[f "," "vld"] > 0) print f, cnt[f "," "vst"] + 0, cnt[f "," "vbin"] + 0, cnt[f "," "slotaddr"] + 0 } }' "$1"
}
fn_has_op() {
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

check_run() {
    # check_run <label> <image> <target> <cpu>
    local label="$1" img="$2" tgt="$3" cpu="$4"
    if can_run "$tgt" "$cpu"; then
        if [ "$(run_img "$img" "$tgt" "$([ "$tgt" = riscv64-linux ] && echo 128)" | strip)" = "$WANT" ]; then ok "$label runs"; else bad "$label output differs"; run_img "$img" "$tgt" 128 | strip | diff <(echo "$WANT") - | head -10; fi
        if [ "$tgt" = riscv64-linux ]; then
            if [ "$(run_img "$img" "$tgt" 256 | strip)" = "$WANT" ]; then ok "$label runs at VLEN 256"; else bad "$label output differs at VLEN 256"; fi
        fi
    else
        echo "skip $label run (no runner here)"
    fi
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
    for f in $TAKEN; do spilled "$img.log" "$f" || miss="$miss $f"; done
    nsp=$(spilled_any "$img.log")
    if [ "$nsp" -ge 3 ]; then
        # wide, splats and many: the first two wherever the inliner put them
        for f in wide splats many; do miss="${miss/ $f}"; done
    fi
    if [ -z "$miss" ]; then ok "$name: every TAKEN loop vectorized through spill slots"; else bad "$name: not vectorized with spill slots:$miss"; grep -E "\] ($(echo $miss | tr ' ' '|')):" "$img.log" | head -10; fi
    if taken "$img.log" deep; then bad "$name: deep vectorized (it needs 18 homes)"; else
        if grep -q -E "^\[vectorize\] [A-Za-z0-9_]+: bb[0-9]+ not vectorized: needs more than 4 vector registers \(with spill slots: needs more than 16" "$img.log"; then ok "$name: deep stays scalar (more than 16 homes)"; else bad "$name: no 'not vectorized ... 16' line for deep"; fi
    fi
    # the functions with vector loads: one with stores and vector
    # arithmetic too (and, wide or red6 when they are vectorized, a slot
    # address: every one that spills has it)
    vecinfo=$(vec_fns "$img.mir")
    nvec=$(echo "$vecinfo" | grep -c -v '^$')
    nfull=$(echo "$vecinfo" | awk '$2 > 0 && $3 > 0' | grep -c -v '^$')
    nslot=$(echo "$vecinfo" | awk '$4 > 0' | grep -c -v '^$')
    if [ "$nfull" -gt 0 ] && [ "$nslot" -gt 0 ]; then
        ok "$name: vld/vst/vbin in $nfull function(s), $nslot of $nvec with vector loads also address slots"
    else
        bad "$name: missing vector operations (functions with vector loads: $nvec, with stores and arithmetic: $nfull, with slot addresses: $nslot)"
    fi
    if [ "$name" = avx2 ]; then
        grep -q -E "^\[vectorize\] [A-Za-z0-9_]+: bb[0-9]+ vectorized.* 32-byte vectors.*spilled" "$img.log" && ok "avx2: wide spills 32-byte vectors" || bad "avx2: wide not at 32 bytes"
        grep -q "^\[vectorize\] red6: .* 16-byte vectors" "$img.log" && ok "avx2: red6 at 16 bytes (a reduction)" || bad "avx2: red6 not at 16 bytes (a reduction)"
    fi
    check_run "$name" "$img" "$tgt" "$cpu"
    for sw in vzspill vectorize; do
        off="$OUT/$name-$sw"
        [ "$tgt" = nat ] && off="$off.exe"
        if build "$off" "$tgt" "$cpu" FID_NAT_OFF=$sw; then
            left=""
            if [ "$sw" = vectorize ] && grep -q -E "^\[vectorize\] [A-Za-z0-9_]+: bb[0-9]+ vectorized" "$off.log"; then left=" a loop"; fi
            if [ "$sw" = vzspill ] && [ "$(spilled_any "$off.log")" -gt 0 ]; then left=" a loop through spill slots"; fi
            if [ -z "$left" ]; then ok "$name FID_NAT_OFF=$sw: the TAKEN loops stay scalar"; else bad "$name FID_NAT_OFF=$sw still vectorizes:$left"; fi
            check_run "$name FID_NAT_OFF=$sw" "$off" "$tgt" "$cpu"
        else
            bad "$name FID_NAT_OFF=$sw build"; tail -5 "$off.log"
        fi
    done
done

echo
echo "the [vectorize] lines of the x86-64 build: $OUT/sse.vz"
if [ "$fails" = 0 ]; then echo "vectorize_spill_check: all passed"; else echo "vectorize_spill_check: $fails failed"; exit 1; fi
