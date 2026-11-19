#!/usr/bin/env bash
# Checks of release-roadmap K10, `@simd` on the native paths (compiler/
# nat_ir.fd, section "OP22: automatic loop vectorization": vz.simd,
# ng_vz_simd_kind, ng_vz_guard, ng_vz_body), run after the compiler is
# built:
#   bash tests/nattools/simd_native_check.sh [fid] [runs]
#                                        (default release/fid.exe, 3)
# 1. tests/bootstrap/test_simd_native.fd and test_simd_annotation.fd print
#    their expected lines through `fid run` and ccompile.
# 2. test_simd_native.fd built with FID_VERIFY=1 and --stats for x86-64
#    Linux (SSE2), x86-64 Linux --cpu x86-64-v3 (AVX2), AArch64 Linux
#    (NEON), RISC-V Linux --cpu rv64gcv (RVV) and --nat Windows:
#    - the @simd loops with checked + / - (TAKEN) are vectorized, their
#      `[vectorize]` line says "@simd: N checked" and "unrolled x2", and
#      their MIR has 4 vld in sum_i (pre-scan 1, unrolled 2, single 1);
#    - the same loop without @simd and an f64 sum under @simd (LEFT) are not;
#    - the images print the expected lines (WSL for x86-64, qemu-user for
#      AArch64 and RISC-V at VLEN 128 and 256, --nat natively on Windows);
#    - FID_NAT_OFF=vzsimd: no TAKEN loop vectorized, the same lines;
#      FID_NAT_OFF=vzunroll: vectorized, no "unrolled", 2 vld in sum_i,
#      the same lines.
#    test_simd_annotation.fd through the same builds prints its lines (its
#    loops stay scalar: an i64 *, Vec_push, the counter in the sum, an if;
#    their `[vectorize]` reasons are printed).
# 3. tests/nattools/simd_ovf_{sum,start,add}.fd: `fid run` and every image
#    that runs here print "start" and then panic on the overflow (exit code
#    not 0, "overflow" on stderr), never the line after it: the pre-scan
#    kept the vector loop from wrapping past the panic.
# 4. speed: tests/nattools/simd_bench.fd built as is, with
#    FID_NAT_OFF=vzsimd and with FID_NAT_OFF=vectorize, best of RUNS runs
#    each (x86-64 Linux, AVX2 when the CPU has it, --nat Windows); the
#    outputs must be the same and the @simd build must not be slower than
#    the vzsimd one by more than 10%. Also the time of test_simd_annotation.
# Lines marked "skip" (no runner here) are not failures. Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
RUNS="${2:-3}"
OUT="${TMPDIR:-/tmp}/fid-simd-native-check"
mkdir -p "$OUT"
T=tests/bootstrap/test_simd_native.fd
A=tests/bootstrap/test_simd_annotation.fd
TAKEN="sum_i add_into sub_k diff"
LEFT="sum_plain fsum"
RUN=""
if command -v wsl.exe >/dev/null 2>&1; then RUN="wsl.exe -e"; fi
WIN=0
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; esac
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }
WANT=$(expected "$T")
WANTA=$(expected "$A")
strip() { grep -v '^\[[a-z0-9]*\] ' | tr -d '\r'; }

# 1. the interpreter and the C path
for f in "$T" "$A"; do
    want=$(expected "$f")
    got=$("$FID" run "$f" 2>&1 | strip)
    if [ "$got" = "$want" ]; then ok "fid run $f"; else bad "fid run $f output"; diff <(echo "$want") <(echo "$got") | head -10; fi
    cexe="$OUT/c-$(basename "$f" .fd)"
    [ "$WIN" = 1 ] && cexe="$cexe.exe"
    if "$FID" ccompile "$f" -o "$cexe" >"$OUT/c.log" 2>&1; then
        if [ "$("$cexe" 2>&1 | tr -d '\r')" = "$want" ]; then ok "ccompile $f"; else bad "ccompile $f output"; fi
    else
        bad "ccompile $f build"; tail -5 "$OUT/c.log"
    fi
done

# name target cpu
LANES="sse x86_64-linux - avx2 x86_64-linux x86-64-v3 neon aarch64-linux - rvv riscv64-linux rv64gcv nat nat -"

emit_args() {
    # emit_args <target> <cpu>: the emit arguments of a lane
    if [ "$1" = nat ]; then printf '%s\n' --nat; else printf '%s\n' --target "$1"; fi
    [ "$2" != - ] && printf '%s\n' --cpu "$2"
    return 0
}

build() {
    # build <src> <image> <target> <cpu> [env...]: FID_VERIFY=1 and --stats
    # (the [vectorize] lines to <image>.log), then the MIR of every
    # function to <image>.mir
    local src="$1" img="$2" tgt="$3" cpu="$4"
    shift 4
    local args=()
    mapfile -t args < <(emit_args "$tgt" "$cpu")
    env FID_VERIFY=1 "$@" "$FID" emit "$src" "${args[@]}" --stats -o "$img" >"$img.log" 2>&1 || return 1
    env FID_NAT_DUMP='*' "$@" "$FID" emit "$src" "${args[@]}" -o "$img.dump" >"$img.mir" 2>&1 || return 1
    return 0
}

taken() { grep -q "^\[vectorize\] $2: bb[0-9]* vectorized" "$1"; }
line_of() { grep "^\[vectorize\] $2: bb[0-9]* vectorized" "$1" | head -1; }
op_count() {
    # op_count <mir> <fn> <op>: how many times the function's listing has
    # the operation (a listing starts with "[mir] <fn>: N operations" or
    # "[mir] <fn> after <stage>: N operations", then "[mir] <k>: <op> ..."
    # lines; the last listing of the function counts)
    awk -v f="$2" -v op="$3" '
        /^\[mir\] / {
            if ($2 ~ /^[0-9]+:$/) { if (cur == f && $3 == op) n++ }
            else { cur = $2; sub(":$", "", cur); if (cur == f) n = 0 }
        }
        END { print n + 0 }' "$1"
}

run_img() {
    local img="$1" tgt="$2" extra="${3:-}"
    case "$tgt" in
        nat) "$img" ;;
        x86_64-linux)
            if [ -n "$RUN" ]; then $RUN "$img"; else "$img"; fi ;;
        aarch64-linux) $RUN qemu-aarch64 "$img" ;;
        riscv64-linux)
            if [ -n "$extra" ]; then $RUN qemu-riscv64 -cpu "rv64,v=true,vlen=$extra" "$img"
            else $RUN qemu-riscv64 "$img"; fi ;;
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

vlen_of() { [ "$1" = riscv64-linux ] && echo 128; return 0; }

same_out() {
    # same_out <image> <target> <want>
    [ "$(run_img "$1" "$2" "$(vlen_of "$2")" 2>&1 | strip)" = "$3" ]
}

set -- $LANES
while [ $# -ge 3 ]; do
    name="$1" tgt="$2" cpu="$3"
    shift 3
    ext=""
    [ "$tgt" = nat ] && ext=".exe"
    img="$OUT/$name$ext"
    if build "$T" "$img" "$tgt" "$cpu"; then ok "$name builds (FID_VERIFY=1)"; else bad "$name build"; tail -5 "$img.log"; continue; fi
    grep "^\[vectorize\]" "$img.log" > "$OUT/$name.vz"
    miss=""
    for f in $TAKEN; do
        l=$(line_of "$img.log" "$f")
        case "$l" in *"@simd: "*" checked"*"unrolled x2"*) ;; *) miss="$miss $f" ;; esac
    done
    if [ -z "$miss" ]; then ok "$name: every TAKEN @simd loop vectorized, checked and unrolled"; else bad "$name: not as expected:$miss"; grep -E "\] ($(echo $miss | tr ' ' '|')):" "$img.log" | head -10; fi
    wrong=""
    for f in $LEFT; do taken "$img.log" "$f" && wrong="$wrong $f"; done
    if [ -z "$wrong" ]; then ok "$name: no LEFT loop vectorized"; else bad "$name: vectorized what it must not:$wrong"; fi
    nv=$(op_count "$img.mir" sum_i vld)
    if [ "$nv" = 4 ]; then ok "$name: sum_i has 4 vld (pre-scan, unrolled 2, single)"; else bad "$name: sum_i has $nv vld, not 4"; fi
    if can_run "$tgt" "$cpu"; then
        if same_out "$img" "$tgt" "$WANT"; then ok "$name runs"; else bad "$name output differs"; run_img "$img" "$tgt" "$(vlen_of "$tgt")" 2>&1 | strip | diff <(echo "$WANT") - | head -10; fi
        if [ "$tgt" = riscv64-linux ]; then
            if [ "$(run_img "$img" "$tgt" 256 2>&1 | strip)" = "$WANT" ]; then ok "$name runs at VLEN 256"; else bad "$name output differs at VLEN 256"; fi
        fi
    else
        echo "skip $name run (no runner here)"
    fi
    # FID_NAT_OFF=vzsimd: @simd changes nothing
    off="$OUT/$name-nosimd$ext"
    if build "$T" "$off" "$tgt" "$cpu" FID_NAT_OFF=vzsimd; then
        still=""
        for f in $TAKEN; do taken "$off.log" "$f" && still="$still $f"; done
        if [ -z "$still" ]; then ok "$name FID_NAT_OFF=vzsimd: no TAKEN loop vectorized"; else bad "$name FID_NAT_OFF=vzsimd still vectorizes:$still"; fi
        if can_run "$tgt" "$cpu"; then
            if same_out "$off" "$tgt" "$WANT"; then ok "$name FID_NAT_OFF=vzsimd runs"; else bad "$name FID_NAT_OFF=vzsimd output differs"; fi
        fi
    else
        bad "$name FID_NAT_OFF=vzsimd build"
    fi
    # FID_NAT_OFF=vzunroll: checked, not unrolled
    nou="$OUT/$name-nounroll$ext"
    if build "$T" "$nou" "$tgt" "$cpu" FID_NAT_OFF=vzunroll; then
        if grep -q "unrolled x2" "$nou.log"; then bad "$name FID_NAT_OFF=vzunroll still unrolls"; else ok "$name FID_NAT_OFF=vzunroll: not unrolled"; fi
        taken "$nou.log" sum_i && ok "$name FID_NAT_OFF=vzunroll: sum_i still vectorized" || bad "$name FID_NAT_OFF=vzunroll: sum_i not vectorized"
        nv2=$(op_count "$nou.mir" sum_i vld)
        if [ "$nv2" = 2 ]; then ok "$name FID_NAT_OFF=vzunroll: sum_i has 2 vld"; else bad "$name FID_NAT_OFF=vzunroll: sum_i has $nv2 vld, not 2"; fi
        if can_run "$tgt" "$cpu"; then
            if same_out "$nou" "$tgt" "$WANT"; then ok "$name FID_NAT_OFF=vzunroll runs"; else bad "$name FID_NAT_OFF=vzunroll output differs"; fi
        fi
    else
        bad "$name FID_NAT_OFF=vzunroll build"
    fi
    # test_simd_annotation.fd natively
    ann="$OUT/$name-ann$ext"
    if build "$A" "$ann" "$tgt" "$cpu"; then
        ok "$name builds test_simd_annotation (FID_VERIFY=1)"
        grep "^\[vectorize\]" "$ann.log" | sed 's/^/     /'
        if can_run "$tgt" "$cpu"; then
            if same_out "$ann" "$tgt" "$WANTA"; then ok "$name runs test_simd_annotation"; else bad "$name test_simd_annotation output differs"; fi
        fi
    else
        bad "$name test_simd_annotation build"; tail -5 "$ann.log"
    fi
    # 3. the overflow panics
    for o in sum start add; do
        src="tests/nattools/simd_ovf_$o.fd"
        oimg="$OUT/$name-ovf-$o$ext"
        if ! build "$src" "$oimg" "$tgt" "$cpu"; then bad "$name simd_ovf_$o build"; tail -5 "$oimg.log"; continue; fi
        if ! can_run "$tgt" "$cpu"; then continue; fi
        run_img "$oimg" "$tgt" "$(vlen_of "$tgt")" >"$oimg.out" 2>"$oimg.err"
        code=$?
        sout=$(strip <"$oimg.out")
        if [ "$sout" = "start" ] && [ "$code" != 0 ] && grep -qi overflow "$oimg.err" "$oimg.out"; then
            ok "$name simd_ovf_$o panics on the overflow (exit $code)"
        else
            bad "$name simd_ovf_$o: exit $code, stdout '$sout'"; head -3 "$oimg.err"
        fi
    done
done

# 3. the overflow programs through `fid run`
for o in sum start add; do
    src="tests/nattools/simd_ovf_$o.fd"
    "$FID" run "$src" >"$OUT/run-ovf-$o.out" 2>"$OUT/run-ovf-$o.err"
    code=$?
    sout=$(strip <"$OUT/run-ovf-$o.out")
    if [ "$sout" = "start" ] && [ "$code" != 0 ] && grep -qi overflow "$OUT/run-ovf-$o.err" "$OUT/run-ovf-$o.out"; then
        ok "fid run simd_ovf_$o panics on the overflow"
    else
        bad "fid run simd_ovf_$o: exit $code, stdout '$sout'"
    fi
done

# 4. speed
ms() { date +%s%N | cut -c1-13; }
best() {
    # best <runner...>: the fastest of RUNS runs in ms; the output to $OUT/last
    local b=999999999 k=0 t0 t1 d
    while [ $k -lt "$RUNS" ]; do
        t0=$(ms)
        "$@" >"$OUT/last" 2>&1
        t1=$(ms)
        d=$((t1 - t0))
        [ $d -lt $b ] && b=$d
        k=$((k + 1))
    done
    echo $b
}
bench() {
    # bench <name> <src> <runner prefix> <target> <cpu>
    local name="$1" src="$2" pre="$3" tgt="$4" cpu="$5" ext="" args=()
    [ "$tgt" = nat ] && ext=".exe"
    mapfile -t args < <(emit_args "$tgt" "$cpu")
    local img="$OUT/bench-$name"
    if ! "$FID" emit "$src" "${args[@]}" -o "$img$ext" >"$img.log" 2>&1 \
        || ! env FID_NAT_OFF=vzsimd "$FID" emit "$src" "${args[@]}" -o "$img-nosimd$ext" >"$img-nosimd.log" 2>&1 \
        || ! env FID_NAT_OFF=vectorize "$FID" emit "$src" "${args[@]}" -o "$img-novec$ext" >"$img-novec.log" 2>&1; then
        bad "bench $name: build failed"; return
    fi
    local on nos nov
    on=$(best $pre "$img$ext"); tr -d '\r' <"$OUT/last" >"$img.out"
    nos=$(best $pre "$img-nosimd$ext"); tr -d '\r' <"$OUT/last" >"$img-nosimd.out"
    nov=$(best $pre "$img-novec$ext"); tr -d '\r' <"$OUT/last" >"$img-novec.out"
    if cmp -s "$img.out" "$img-nosimd.out" && cmp -s "$img.out" "$img-novec.out"; then
        ok "bench $name: the same output three ways"
    else
        bad "bench $name: OUTPUT DIFFERS"
    fi
    printf "     %-22s @simd %6d ms   vzsimd off %6d ms   vectorize off %6d ms\n" "$name" "$on" "$nos" "$nov"
    if [ "$(basename "$src")" = simd_bench.fd ]; then
        if [ $((on * 10)) -le $((nos * 11)) ]; then ok "bench $name: @simd not slower"; else bad "bench $name: @simd slower than without ($on > $nos ms)"; fi
    fi
}
for src in tests/nattools/simd_bench.fd "$A"; do
    nm=$(basename "$src" .fd)
    if [ -n "$RUN" ] || { [ "$(uname -s)" = Linux ] && [ "$(uname -m)" = x86_64 ]; }; then
        bench "$nm-sse" "$src" "$RUN" x86_64-linux -
        if $RUN grep -q avx2 /proc/cpuinfo 2>/dev/null; then bench "$nm-avx2" "$src" "$RUN" x86_64-linux x86-64-v3; fi
    fi
    if [ "$WIN" = 1 ]; then bench "$nm-nat" "$src" "" nat -; fi
done

echo
echo "the [vectorize] lines of the x86-64 build: $OUT/sse.vz"
if [ "$fails" = 0 ]; then echo "simd_native_check: all passed"; else echo "simd_native_check: $fails failed"; exit 1; fi
