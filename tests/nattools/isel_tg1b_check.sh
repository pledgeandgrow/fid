#!/usr/bin/env bash
# TG1 (docs/roadmap/llvm-roadmap.md), the last families moved into the
# selection table (compiler/nat_ir.fd ng_isel_rows_tg1b /
# ng_isel_emit_tg1b): MO_DIV, MO_REM, MO_MULH, MO_ASR63, MO_I2F, MO_F2I,
# MO_ANDN, MO_BLSR, MO_CSEL, MO_TBNZ, MO_OPW, MO_ALUM. Run after the
# compiler is built:
#   bash tests/nattools/isel_tg1b_check.sh [fid]   (default release/fid.exe)
# For every machine-layer target, with each --cpu that changes the rules
# (x86-64-v3: BMI1 ANDN / BLSR; rva23u64: Zbb ANDN, Zicond CSEL), float
# registers off and on (FID_NAT_ON=fregs):
# 1. Byte identity: the image of each program in PROGS is the same with
#    the table (default) and with FID_NAT_OFF=isel (the hand-written
#    selection).
# 2. FID_VERIFY=1 builds pass (nat_sel_check, which now runs
#    nat_isel_tg1b_check: each new rule against the hand-written
#    selection, byte for byte, on many register shapes and every --cpu).
# 3. FID_NAT_MUTATE=iseldiv (the division rules take the other result
#    register or signedness) makes the FID_VERIFY=1 build fail with the
#    selection check's error.
# 4. Where the image runs here (x86-64 Linux through WSL or natively; the
#    --nat Windows image on Windows; others under qemu-user when
#    qemu-aarch64 / qemu-riscv64 are on PATH), test_machine_isel_tg1b
#    prints its expected lines.
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-isel-tg1b"
mkdir -p "$OUT"
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

T=tests/bootstrap/test_machine_isel_tg1b.fd
PROGS="$T tests/bootstrap/test_machine_bits.fd tests/bootstrap/test_machine_select.fd tests/bootstrap/test_machine_ldop.fd tests/bootstrap/test_machine_rules_random.fd tests/bootstrap/test_machine_isel_tg1.fd tests/bootstrap/test_machine_isel.fd tests/bootstrap/hello.fd"

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

WIN=0
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; esac
WSL=""
if command -v wsl.exe >/dev/null 2>&1; then WSL="wsl.exe -e"; fi

runner() {   # how to run an image of target $1 here ("no": not at all)
    case "$1" in
        nat) [ "$WIN" = 1 ] && echo "" || echo no ;;
        x86_64-linux)
            if [ -n "$WSL" ]; then echo "$WSL"
            elif [ "$(uname -s)" = Linux ] && [ "$(uname -m)" = x86_64 ]; then echo ""
            else echo no; fi ;;
        aarch64-linux)
            if [ -n "$WSL" ]; then echo "$WSL qemu-aarch64"
            elif command -v qemu-aarch64 >/dev/null 2>&1; then echo qemu-aarch64
            else echo no; fi ;;
        riscv64-linux)
            if [ -n "$WSL" ]; then echo "$WSL qemu-riscv64"
            elif command -v qemu-riscv64 >/dev/null 2>&1; then echo qemu-riscv64
            else echo no; fi ;;
        *) echo no ;;
    esac
}

emit() {   # emit <prog> <target|nat> <image> [extra args...]
    local p="$1" t="$2" img="$3"
    shift 3
    if [ "$t" = nat ]; then
        "$FID" emit "$p" --nat "$@" -o "$img" >"$img.log" 2>&1
    else
        "$FID" emit "$p" --target "$t" "$@" -o "$img" >"$img.log" 2>&1
    fi
}

CONFIGS=(
    "x86_64-linux||x64"
    "x86_64-linux|--cpu x86-64-v3|x64v3"
    "nat||nat"
    "aarch64-linux||a64"
    "riscv64-linux||rv"
    "riscv64-linux|--cpu rva23u64|rv23"
)

for c in "${CONFIGS[@]}"; do
    IFS='|' read -r tgt extra name <<<"$c"
    ext=""
    [ "$tgt" = nat ] && ext=".exe"
    for fr in off on; do
        env_on=""
        [ $fr = on ] && env_on="fregs"
        # 1. byte identity
        for p in $PROGS; do
            [ -f "$p" ] || continue
            b=$(basename "$p" .fd)
            ti="$OUT/$b-$name-$fr-table$ext"
            hi="$OUT/$b-$name-$fr-hand$ext"
            if ! FID_NAT_ON="$env_on" FID_NAT_OFF= emit "$p" "$tgt" "$ti" $extra; then
                bad "$name $b fregs $fr: build failed ($(tail -1 "$ti.log"))"
                continue
            fi
            if ! FID_NAT_ON="$env_on" FID_NAT_OFF=isel emit "$p" "$tgt" "$hi" $extra; then
                bad "$name $b fregs $fr: build with FID_NAT_OFF=isel failed"
                continue
            fi
            if cmp -s "$ti" "$hi"; then
                ok "$name $b fregs $fr: table and hand-written selection give the same image"
            else
                bad "$name $b fregs $fr: the images differ"
            fi
        done
        # 2. FID_VERIFY=1
        vi="$OUT/$name-verify-$fr$ext"
        if FID_VERIFY=1 FID_NAT_ON="$env_on" emit "$T" "$tgt" "$vi" $extra; then
            ok "$name fregs $fr: FID_VERIFY=1 clean (nat_isel_tg1b_check)"
        else
            bad "$name fregs $fr: FID_VERIFY=1 build failed: $(grep -m1 -i 'selection\|error' "$vi.log")"
        fi
        # 4. the program's output
        r=$(runner "$tgt")
        if [ "$r" != no ] && [ -f "$OUT/$(basename "$T" .fd)-$name-$fr-table$ext" ]; then
            got=$(timeout 120 $r "$OUT/$(basename "$T" .fd)-$name-$fr-table$ext" 2>&1 | tr -d '\r')
            if [ "$got" = "$(expected "$T")" ]; then
                ok "$name fregs $fr: test_machine_isel_tg1b prints its expected output"
            else
                bad "$name fregs $fr: test_machine_isel_tg1b output differs"
            fi
        fi
    done
    # 3. the mutation is caught
    mi="$OUT/$name-mut-iseldiv$ext"
    if FID_VERIFY=1 FID_NAT_MUTATE=iseldiv emit "$T" "$tgt" "$mi" $extra; then
        bad "$name: FID_NAT_MUTATE=iseldiv passed the selection checks"
    elif grep -q "instruction selection check" "$mi.log"; then
        ok "$name: FID_NAT_MUTATE=iseldiv caught: $(grep -m1 -o 'TG1[^(]*' "$mi.log" | head -1)"
    else
        bad "$name: FID_NAT_MUTATE=iseldiv failed for another reason: $(tail -1 "$mi.log")"
    fi
done

if [ "$fails" -gt 0 ]; then
    echo "$fails check(s) failed"
    exit 1
fi
echo "all TG1 (tg1b) selection checks passed"
