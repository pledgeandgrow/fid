#!/usr/bin/env bash
# TG2 (docs/roadmap/llvm-roadmap.md): every selection rule of the machine
# layer tested on random operands against the generic lowering, and seen
# to fail when the rule is broken on purpose. Run after the compiler is
# built:
#   bash tests/nattools/rule_mutation_check.sh [fid]   (default release/fid.exe)
# 1. tests/bootstrap/test_machine_rules_random.fd is built with
#    FID_VERIFY=1 for x86_64-linux (and --cpu x86-64-v3: BMI1),
#    aarch64-linux, riscv64-linux (and --cpu rva22u64: Zbb) and --nat
#    Windows; where the image runs (natively, WSL, qemu-aarch64 /
#    qemu-riscv64 when installed) its output must be the test's expected
#    lines, with every rule on and with FID_NAT_OFF=<the rules> (the
#    generic lowering).
# 2. The rules were selected: FID_NAT_DUMP of tb3 shows `tbnz`
#    (TG8/TG7), of gap_lt a `brcc` and no `setcc` (RA19), of main on
#    riscv64 an `opw` (TG9); with FID_NAT_OFF=tbz,brgap,opw none of them.
# 3. Mutation: for each rule, FID_NAT_MUTATE=<rule> (a deliberately wrong
#    rule, compiler/nat_ir.fd ng_mutant) on a target where the rule
#    applies must change that rule's line (its count of disagreeing trials
#    is no longer 0). A target that cannot run the image is skipped.
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
T=tests/bootstrap/test_machine_rules_random.fd
OUT="${TMPDIR:-/tmp}/fid-rule-mutation"
mkdir -p "$OUT"
WSL=""
if command -v wsl.exe >/dev/null 2>&1; then WSL="wsl.exe -e"; fi
WIN=0
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; esac
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }
EXP=$(expected "$T")

# runner <target>: the command prefix that runs an image of <target>
# here, or "no"
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

# build <image> <target> [args...]
build() {
    local img="$1" tgt="$2"
    shift 2
    if [ "$tgt" = nat ]; then
        FID_VERIFY=1 "$FID" emit "$T" --nat "$@" -o "$img" >"$img.log" 2>&1
    else
        FID_VERIFY=1 "$FID" emit "$T" --target "$tgt" "$@" -o "$img" >"$img.log" 2>&1
    fi
}

# run <image> <target>: prints the output, or returns 2 when it cannot run
run() {
    local r
    r=$(runner "$2")
    [ "$r" = no ] && return 2
    $r "$1" 2>&1 | tr -d '\r'
}

line_of() { echo "$1" | grep "^$2 " ; }

RULES_OFF="tbz,brgap,opw,memop,andn,blsr,addi,brfuse,deadmir,shi,isel,cmpslot,rmwslot,blsmsk,blsi,wbidx"

# target, extra arguments (a --cpu), name
CONFIGS=(
    "x86_64-linux||x64"
    "x86_64-linux|--cpu x86-64-v3|x64v3"
    "nat||nat"
    "nat|--cpu x86-64-v3|natv3"
    "aarch64-linux||a64"
    "riscv64-linux||rv"
    "riscv64-linux|--cpu rva22u64|rvzb"
)

# 1. outputs, rules on and off
for c in "${CONFIGS[@]}"; do
    IFS='|' read -r tgt extra name <<<"$c"
    for mode in on off; do
        img="$OUT/$name-$mode"
        [ "$tgt" = nat ] && img="$img.exe"
        if [ "$mode" = off ]; then
            FID_NAT_OFF="$RULES_OFF" build "$img" "$tgt" $extra
        else
            build "$img" "$tgt" $extra
        fi
        if [ $? -ne 0 ]; then
            bad "$name rules $mode: build failed ($(tail -1 "$img.log"))"
            continue
        fi
        got=$(run "$img" "$tgt")
        rc=$?
        if [ $rc -eq 2 ]; then
            ok "$name rules $mode: built (cannot run here)"
        elif [ "$got" = "$EXP" ]; then
            ok "$name rules $mode: output matches"
        else
            bad "$name rules $mode: output differs"
            echo "$got" | head -10
        fi
    done
done

# 2. the rules were selected (MIR dumps)
dump_has() {
    # dump_has <fn> <target> <extra> <word>
    local d
    d=$(FID_NAT_DUMP="$1" "$FID" emit "$T" --target "$2" $3 -o "$OUT/dump.img" 2>&1)
    echo "$d" | grep -q "$4"
}
for tgt in x86_64-linux aarch64-linux riscv64-linux; do
    if dump_has tb3 "$tgt" "" "tbnz"; then ok "$tgt: tb3 has tbnz"; else bad "$tgt: tb3 has no tbnz"; fi
    d=$(FID_NAT_DUMP=gap_lt "$FID" emit "$T" --target "$tgt" -o "$OUT/dump.img" 2>&1 | sed -n '/after branch fusion/,$p')
    if echo "$d" | grep -q "brcc" && ! echo "$d" | grep -q "setcc"; then
        ok "$tgt: gap_lt branches on the compare (no setcc)"
    else
        bad "$tgt: gap_lt keeps a setcc or has no brcc"
    fi
    d=$(FID_NAT_OFF=tbz,brgap FID_NAT_DUMP=tb3 "$FID" emit "$T" --target "$tgt" -o "$OUT/dump.img" 2>&1)
    if echo "$d" | grep -q "tbnz"; then bad "$tgt: tbnz with FID_NAT_OFF=tbz"; else ok "$tgt: no tbnz with FID_NAT_OFF=tbz"; fi
done
if FID_NAT_DUMP=main "$FID" emit "$T" --target riscv64-linux -o "$OUT/dump.img" 2>&1 | grep -q "opw"; then
    ok "riscv64-linux: main has opw (ADDW / SUBW / MULW)"
else
    bad "riscv64-linux: main has no opw"
fi

# 3. mutations: rule, then the configurations where it applies
MUTS=(
    "tbz|x64 nat a64 rv"
    "brgap|x64 nat a64 rv"
    "opw|rv"
    "memop|x64 nat"
    "andn|x64v3 natv3 a64 rvzb"
    "addi|x64 nat a64 rv"
    "shi|x64 nat a64 rv"
)
for mu in "${MUTS[@]}"; do
    IFS='|' read -r rule where <<<"$mu"
    seen=0
    for c in "${CONFIGS[@]}"; do
        IFS='|' read -r tgt extra name <<<"$c"
        case " $where " in *" $name "*) ;; *) continue ;; esac
        img="$OUT/$name-mut-$rule"
        [ "$tgt" = nat ] && img="$img.exe"
        if ! FID_NAT_MUTATE="$rule" build "$img" "$tgt" $extra; then
            bad "$name: build with FID_NAT_MUTATE=$rule failed"
            continue
        fi
        got=$(run "$img" "$tgt")
        [ $? -eq 2 ] && continue
        seen=1
        want=$(line_of "$EXP" "$rule")
        have=$(line_of "$got" "$rule")
        if [ "$have" != "$want" ]; then
            ok "$name: FID_NAT_MUTATE=$rule caught ($have)"
        else
            bad "$name: FID_NAT_MUTATE=$rule not caught (the test still prints: $have)"
        fi
    done
    [ $seen = 0 ] && echo "skip $rule: no target where it applies runs here"
done

if [ $fails -ne 0 ]; then
    echo "$fails check(s) failed"
    exit 1
fi
echo "all checks passed"
