#!/usr/bin/env bash
# TG1 / TG2 (docs/roadmap/llvm-roadmap.md): instruction selection as rules
# past the integer ALU (compiler/nat_ir.fd ng_isel_rows_tg1: constants,
# shifts by a constant, element addresses, compares, loads and stores with
# their addressing modes, float operations), each rule tested alone on
# random operands (nat_isel_rule_check). Run after the compiler is built:
#   bash tests/nattools/isel_rules_check.sh [fid]   (default release/fid.exe)
# 1. FID_VERIFY=1 builds of tests/bootstrap/test_machine_isel_tg1.fd pass on
#    every machine-layer target and --cpu (nat_sel_check: values and table
#    = hand-written bytes; nat_isel_mem_check: memory, branch and float
#    rules = hand-written bytes; nat_isel_rule_check: each rule alone on
#    random operands), also with FID_NAT_ON=fregs (float registers), and
#    --stats prints "[isel] TG2: N rules ..." with N > 0.
# 2. Byte identity: the image of each program is the same with the table
#    and with FID_NAT_OFF=isel (the hand-written selection).
# 3. Mutations: FID_NAT_MUTATE=iselimm (LI / ADDI / SHI rules off by one),
#    iselcc (SETCC / BRCC rules on the inverse condition) and isel (SUB's
#    operands swapped) make the FID_VERIFY=1 build fail with the selection
#    check's error.
# 4. Shifts by a constant are one MO_SHI (FID_NAT_DUMP shows `shi` for
#    shl13; none with FID_NAT_OFF=shi), and FID_NAT_MUTATE=shi changes the
#    `shi` line of test_machine_rules_random.fd where the image runs.
# 5. Where an image runs here (native, WSL, qemu-user), its output is the
#    test's expected lines with the table, without it (FID_NAT_OFF=isel)
#    and without shift fusion (FID_NAT_OFF=shi).
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-isel-rules"
mkdir -p "$OUT"
T1=tests/bootstrap/test_machine_isel_tg1.fd
T2=tests/bootstrap/test_machine_rules_random.fd
WSL=""
if command -v wsl.exe >/dev/null 2>&1; then WSL="wsl.exe -e"; fi
WIN=0
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; esac
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

runner() {
    case "$1" in
        nat) if [ "$WIN" = 1 ]; then echo ""; else echo no; fi ;;
        x86_64-linux)
            if [ "$(uname -s)" = Linux ] && [ "$(uname -m)" = x86_64 ]; then echo ""
            elif [ -n "$WSL" ]; then echo "$WSL"
            else echo no; fi ;;
        aarch64-linux)
            if [ "$(uname -s)" = Linux ] && [ "$(uname -m)" = aarch64 ]; then echo ""
            elif command -v qemu-aarch64 >/dev/null 2>&1; then echo qemu-aarch64
            else echo no; fi ;;
        riscv64-linux)
            if command -v qemu-riscv64 >/dev/null 2>&1; then echo qemu-riscv64; else echo no; fi ;;
    esac
}

# emit <prog> <target> <image> [extra args...]   (environment from the caller)
emit() {
    local p="$1" t="$2" img="$3"
    shift 3
    if [ "$t" = nat ]; then
        "$FID" emit "$p" --nat "$@" -o "$img" >"$img.log" 2>&1
    else
        "$FID" emit "$p" --target "$t" "$@" -o "$img" >"$img.log" 2>&1
    fi
}

run() {
    local r
    r=$(runner "$2")
    [ "$r" = no ] && return 2
    timeout 120 $r "$1" 2>&1 | tr -d '\r'
}

CONFIGS=(
    "x86_64-linux||x64"
    "x86_64-linux|--cpu x86-64-v3|x64v3"
    "nat||nat"
    "aarch64-linux||a64"
    "riscv64-linux||rv"
    "riscv64-linux|--cpu rva22u64|rvzb"
)

for c in "${CONFIGS[@]}"; do
    IFS='|' read -r tgt extra name <<<"$c"
    ext=""
    [ "$tgt" = nat ] && ext=".exe"
    # 1. the checks of FID_VERIFY=1, float registers off and on
    for fr in off on; do
        img="$OUT/$name-verify-$fr$ext"
        if [ $fr = on ]; then
            FID_VERIFY=1 FID_NAT_ON=fregs emit "$T1" "$tgt" "$img" $extra --stats
        else
            FID_VERIFY=1 emit "$T1" "$tgt" "$img" $extra --stats
        fi
        if [ $? -ne 0 ]; then
            bad "$name fregs $fr: FID_VERIFY=1 build failed: $(grep -m1 -i 'selection\|error' "$img.log")"
            continue
        fi
        n=$(sed -n 's/.*\[isel\] TG2: \([0-9]*\) rules.*/\1/p' "$img.log" | head -1)
        if [ -n "$n" ] && [ "$n" -gt 0 ]; then
            ok "$name fregs $fr: FID_VERIFY=1 clean, $n rules checked alone on random operands"
        else
            bad "$name fregs $fr: no '[isel] TG2' line with --stats"
        fi
    done
    # 2. byte identity, table against hand-written selection
    for p in "$T1" "$T2" tests/bootstrap/test_machine_isel.fd tests/bootstrap/test_machine_fregs.fd tests/bootstrap/hello.fd; do
        [ -f "$p" ] || continue
        b=$(basename "$p" .fd)
        for fr in off on; do
            env_on=""
            [ $fr = on ] && env_on="fregs"
            if ! FID_NAT_ON="$env_on" FID_NAT_OFF= emit "$p" "$tgt" "$OUT/$b-$name-$fr-table$ext" $extra; then
                bad "$name $b fregs $fr: build failed ($(tail -1 "$OUT/$b-$name-$fr-table$ext.log"))"
                continue
            fi
            if ! FID_NAT_ON="$env_on" FID_NAT_OFF=isel emit "$p" "$tgt" "$OUT/$b-$name-$fr-hand$ext" $extra; then
                bad "$name $b fregs $fr: build with FID_NAT_OFF=isel failed"
                continue
            fi
            if cmp -s "$OUT/$b-$name-$fr-table$ext" "$OUT/$b-$name-$fr-hand$ext"; then
                ok "$name $b fregs $fr: table and hand-written selection give the same image"
            else
                bad "$name $b fregs $fr: the images differ"
            fi
        done
    done
    # 3. mutations caught by the selection checks
    for mu in iselimm iselcc isel; do
        img="$OUT/$name-mut-$mu$ext"
        if FID_VERIFY=1 FID_NAT_MUTATE=$mu emit "$T1" "$tgt" "$img" $extra; then
            bad "$name: FID_NAT_MUTATE=$mu passed the selection checks"
        elif grep -q "instruction selection check" "$img.log"; then
            ok "$name: FID_NAT_MUTATE=$mu caught: $(grep -m1 -o 'TG[12][^(]*' "$img.log" | head -1)"
        else
            bad "$name: FID_NAT_MUTATE=$mu failed for another reason: $(tail -1 "$img.log")"
        fi
    done
    # 5. outputs where the image runs
    for p in "$T1" "$T2"; do
        b=$(basename "$p" .fd)
        exp=$(expected "$p")
        for mode in table hand noshi; do
            img="$OUT/$b-$name-run-$mode$ext"
            case $mode in
                table) FID_NAT_OFF= emit "$p" "$tgt" "$img" $extra ;;
                hand) FID_NAT_OFF=isel emit "$p" "$tgt" "$img" $extra ;;
                noshi) FID_NAT_OFF=shi emit "$p" "$tgt" "$img" $extra ;;
            esac
            if [ $? -ne 0 ]; then
                bad "$name $b $mode: build failed"
                continue
            fi
            got=$(run "$img" "$tgt")
            rc=$?
            if [ $rc -eq 2 ]; then
                ok "$name $b $mode: built (cannot run here)"
            elif [ "$got" = "$exp" ]; then
                ok "$name $b $mode: output matches"
            else
                bad "$name $b $mode: output differs"
                diff <(echo "$exp") <(echo "$got") | head -8
            fi
        done
    done
    # 4. the shift mutation changes the shi line
    img="$OUT/$name-mut-shi$ext"
    if FID_NAT_MUTATE=shi emit "$T2" "$tgt" "$img" $extra; then
        got=$(run "$img" "$tgt")
        if [ $? -ne 2 ]; then
            want=$(expected "$T2" | grep '^shi ')
            have=$(echo "$got" | grep '^shi ')
            if [ "$have" != "$want" ]; then
                ok "$name: FID_NAT_MUTATE=shi caught (${have:-no shi line})"
            else
                bad "$name: FID_NAT_MUTATE=shi not caught (still: $have)"
            fi
        fi
    else
        bad "$name: build with FID_NAT_MUTATE=shi failed"
    fi
done

# 4. the MIR has the shift by a constant as one operation
for tgt in x86_64-linux aarch64-linux riscv64-linux; do
    d=$(FID_NAT_DUMP=shl13 "$FID" emit "$T2" --target "$tgt" -o "$OUT/dump.img" 2>&1 | sed -n '/after add-immediate selection/,$p')
    if echo "$d" | grep -q ": shi "; then
        ok "$tgt: shl13 is one shi"
    else
        bad "$tgt: shl13 has no shi"
    fi
    d=$(FID_NAT_OFF=shi FID_NAT_DUMP=shl13 "$FID" emit "$T2" --target "$tgt" -o "$OUT/dump.img" 2>&1)
    if echo "$d" | grep -q ": shi "; then
        bad "$tgt: shi with FID_NAT_OFF=shi"
    else
        ok "$tgt: no shi with FID_NAT_OFF=shi"
    fi
done

if [ $fails -ne 0 ]; then
    echo "$fails check(s) failed"
    exit 1
fi
echo "all TG1/TG2 rule checks passed"
