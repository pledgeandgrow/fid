#!/usr/bin/env bash
# Checks of MI14 (docs/roadmap/llvm-roadmap.md: what keeps --nat from
# becoming the Windows default) in the machine layer, compiler/nat_ir.fd,
# run after the compiler is built:
#   bash tests/nattools/machine_mi14_check.sh [fid]   (default release/fid.exe)
#
# 1. Floats split at calls (RA4 for the float classes, ng_mir_alloc_split):
#    tests/bootstrap/test_machine_split_float.fd on x86_64-linux,
#    aarch64-linux, riscv64-linux and --nat Windows, each built with
#    FID_VERIFY=1 --stats (the MIR verifier, the allocation checker, the
#    split checker) by default, with FID_NAT_OFF=splitfloat, with
#    FID_NAT_OFF=split and with FID_NAT_ON=splitall. Every build must
#    succeed and every image that runs here must print the expected lines.
#    By default `between` (floats live across calls) must have a [split]
#    line with pieces, kept; with FID_NAT_OFF=splitfloat it has none kept
#    and spills more.
# 2. Floats in float registers on --nat Windows by default (ng_fregs_on):
#    the default build reports "[fregs] ...: N float ranges, M in float
#    registers" with M > 0 summed; FID_NAT_OFF=winfregs reports none.
# 3. --nat Windows keeps its vector operations with the float pool
#    (vectors in xmm12..xmm15): test_machine_simd.fd built by default with
#    FID_VERIFY=1 (ng_vec_verify: no vector register in the float pool)
#    has `vbin` in its machine IR (FID_NAT_DUMP), and runs.
# 4. The vectored exception handler keeps xmm6..xmm15 (Windows' callee-
#    saved float registers, which the pool and the vectors now use): with
#    llvm-objdump / objdump, the --nat image has movdqu stores of xmm6 and
#    xmm15 to the stack and loads back.
# 5. test_machine_fregs.fd and test_machine_split.fd through --nat Windows
#    with FID_VERIFY=1 print their expected lines.
#
# Linux executables run directly on Linux (AArch64 / RISC-V under
# qemu-user), through WSL on Windows (`wsl.exe -e`); a target whose runner
# is missing is built and checked only. Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
SRC=tests/bootstrap/test_machine_split_float.fd
OUT="${TMPDIR:-/tmp}/fid-machine-mi14"
mkdir -p "$OUT"
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

# the `// ` lines right after `// Expected output:` (up to the first other
# line, so a comment after the blank line that ends the block is not taken)
expected() { awk '/^\/\/ Expected output:/ { f = 1; next } f && /^\/\/ / { sub(/^\/\/ /, ""); print; next } f { exit }' "$1" | tr -d '\r'; }

HOST=linux
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; esac
WSL=""
# A working WSL only: wsl.exe is on PATH on every Windows, with no
# distribution installed too, and then every image "fails" with no output
if [ "$HOST" = windows ] && command -v wsl.exe >/dev/null 2>&1 && wsl.exe -e sh -c 'echo wsl-ok' 2>/dev/null | grep -q 'wsl-ok'; then WSL="wsl.exe -e"; fi

# run_exe <dir> <exe name> <target>: the program's output (status 99 when
# this host cannot run it)
run_exe() {
    local dir="$1" exe="$2" t="$3" q=""
    case "$t" in
        aarch64-linux) q=qemu-aarch64 ;;
        riscv64-linux) q=qemu-riscv64 ;;
    esac
    case "$t" in
        *-linux)
            if [ "$HOST" = windows ] && [ -z "$WSL" ]; then return 99; fi
            if [ -n "$q" ]; then
                if [ -n "$WSL" ]; then
                    (cd "$dir" && $WSL sh -c "command -v $q >/dev/null") || return 99
                elif ! command -v "$q" >/dev/null 2>&1; then
                    if [ "$(uname -m)" = "${t%%-*}" ]; then q=""; else return 99; fi
                fi
            fi
            (cd "$dir" && $WSL $q "./$exe" 2>&1 | tr -d '\r')
            ;;
        windows)
            [ "$HOST" = windows ] || return 99
            (cd "$dir" && "./$exe" 2>&1 | tr -d '\r')
            ;;
    esac
}

# emit_one <src> <target> <exe path> <log> [env assignments...]
emit_one() {
    local src="$1" t="$2" exe="$3" log="$4"
    shift 4
    local targ=(--target "$t")
    if [ "$t" = windows ]; then targ=(--nat); fi
    env FID_VERIFY=1 "$@" "$FID" emit "$src" "${targ[@]}" --stats -o "$exe" >"$log" 2>&1
}

# check_run <src> <target> <exe name> <label>
check_run() {
    local src="$1" t="$2" exe="$3" label="$4" want got rc
    want="$(expected "$src")"
    got=$(run_exe "$OUT" "$exe" "$t")
    rc=$?
    if [ $rc -eq 99 ]; then
        ok "$label: built with FID_VERIFY=1 (not run on this host)"
    elif [ "$got" = "$want" ]; then
        ok "$label: output as expected"
    else
        bad "$label: output differs"
        diff <(echo "$want") <(echo "$got") | head -10
    fi
}

# 1. floats split at calls
for t in x86_64-linux aarch64-linux riscv64-linux windows; do
    sfx=""
    [ "$t" = windows ] && sfx=".exe"
    for mode in on nofloat off all; do
        exe="mi14-$t-$mode$sfx"
        envs=()
        # RA18: on AArch64 / RISC-V a float across a call takes a
        # callee-saved float register and never spills, so there is
        # nothing to split: these builds turn that off (FID_NAT_OFF=fcallee)
        # to check the split itself (tests/nattools/machine_fcallee_check.sh
        # checks the registers)
        fc=""
        case "$t" in aarch64-linux|riscv64-linux) fc=",fcallee" ;; esac
        case "$mode" in
            on) [ -n "$fc" ] && envs=(FID_NAT_OFF=fcallee) ;;
            nofloat) envs=(FID_NAT_OFF=splitfloat$fc) ;;
            off) envs=(FID_NAT_OFF=split$fc) ;;
            all) envs=(FID_NAT_ON=splitall); [ -n "$fc" ] && envs+=(FID_NAT_OFF=fcallee) ;;
        esac
        if ! emit_one "$SRC" "$t" "$OUT/$exe" "$OUT/$exe.log" ${envs[@]+"${envs[@]}"}; then
            bad "$t $mode: emit failed: $(grep -v '^\[' "$OUT/$exe.log" | tail -2)"
            continue
        fi
        check_run "$SRC" "$t" "$exe" "$t $mode"
    done
    on="$OUT/mi14-$t-on$sfx.log"
    nofl="$OUT/mi14-$t-nofloat$sfx.log"
    if [ -f "$on" ] && [ -f "$nofl" ]; then
        # between (floats live across calls): by default its float split is
        # kept and costs less; with FID_NAT_OFF=splitfloat it is not kept
        # and between spills more. around (a loop over floats read across
        # the calls) is no candidate on any target since RA18's
        # callee-saved float registers: printed, not required.
        bp=$(sed -n 's/^\[split\] between: \([0-9]*\) pieces.*/\1/p' "$on" | head -1)
        sp_on=$(sed -n 's/^\[stats\] between: spilled \([0-9]*\),.*/\1/p' "$on" | head -1)
        sp_off=$(sed -n 's/^\[stats\] between: spilled \([0-9]*\),.*/\1/p' "$nofl" | head -1)
        if [ -n "$bp" ] && [ "$bp" -gt 0 ] && grep -q '^\[split\] between: .*, kept$' "$on"; then
            ok "$t: between's floats split into $bp piece(s), kept ($(sed -n 's/^\[split\] between: .*\(cost [0-9]* -> [0-9]*\).*/\1/p' "$on" | head -1))"
        else
            bad "$t: between's float split is not kept by default ($(grep '^\[split\] between' "$on" | head -1))"
        fi
        if grep -q '^\[split\] between: .*, kept$' "$nofl"; then
            bad "$t: FID_NAT_OFF=splitfloat still keeps between's split"
        elif [ -n "$sp_on" ] && [ -n "$sp_off" ] && [ "$sp_off" -gt "$sp_on" ]; then
            ok "$t: FID_NAT_OFF=splitfloat: between not split, $sp_off values spilled (default: $sp_on)"
        else
            bad "$t: FID_NAT_OFF=splitfloat: between not split but spilled $sp_off (default: $sp_on): the float split changes nothing"
        fi
        echo "info $t around: $(grep '^\[split\] around' "$on" | head -1)"
        echo "info $t between: $(grep '^\[split\] between' "$on" | head -1)"
        echo "info $t many: $(grep '^\[split\] many' "$on" | head -1)"
    fi
done

# 2. float registers on --nat Windows by default; FID_NAT_OFF=winfregs: none
fsum() { grep '^\[fregs\]' "$1" | sed -n 's/.*, \([0-9]*\) in float registers.*/\1/p' | awk '{s += $1} END {print s + 0}'; }
won="$OUT/mi14-windows-on.exe.log"
if [ -f "$won" ]; then
    n=$(fsum "$won")
    if [ "$n" -gt 0 ]; then ok "--nat Windows: $n float range(s) in float registers by default"; else bad "--nat Windows: no float range in a float register by default"; fi
fi
wo="mi14-windows-nofregs.exe"
if emit_one "$SRC" windows "$OUT/$wo" "$OUT/$wo.log" FID_NAT_OFF=winfregs; then
    if grep -q '^\[fregs\]' "$OUT/$wo.log"; then bad "--nat Windows: FID_NAT_OFF=winfregs still hands out float registers"; else ok "--nat Windows: FID_NAT_OFF=winfregs: no float register"; fi
    check_run "$SRC" windows "$wo" "--nat Windows winfregs off"
else
    bad "--nat Windows FID_NAT_OFF=winfregs: emit failed: $(grep -v '^\[' "$OUT/$wo.log" | tail -2)"
fi
# the Linux targets are not changed by winfregs
lx="mi14-x86_64-linux-winfregs"
if emit_one "$SRC" x86_64-linux "$OUT/$lx" "$OUT/$lx.log" FID_NAT_OFF=winfregs; then
    n=$(fsum "$OUT/$lx.log")
    if [ "$n" -gt 0 ]; then ok "x86_64-linux: FID_NAT_OFF=winfregs leaves Linux's float registers ($n)"; else bad "x86_64-linux: FID_NAT_OFF=winfregs turned Linux's float registers off"; fi
else
    bad "x86_64-linux FID_NAT_OFF=winfregs: emit failed"
fi

# 3. vector operations beside the float pool on --nat Windows
simd=tests/bootstrap/test_machine_simd.fd
if [ -f "$simd" ]; then
    se="mi14-simd-nat.exe"
    if env FID_VERIFY=1 FID_NAT_DUMP='*' "$FID" emit "$simd" --nat -o "$OUT/$se" >"$OUT/$se.log" 2>&1; then
        if grep -q '^\[mir\] [0-9]*: vbin ' "$OUT/$se.log"; then ok "--nat Windows: vector operations kept beside the float pool"; else bad "--nat Windows: no vbin in test_machine_simd's machine IR"; fi
        check_run "$simd" windows "$se" "--nat Windows test_machine_simd"
    else
        bad "--nat Windows test_machine_simd: emit failed: $(grep -v '^\[' "$OUT/$se.log" | tail -2)"
    fi
fi

# 4. the vectored exception handler keeps xmm6..xmm15
OBJDUMP=""
for c in llvm-objdump objdump; do
    if command -v "$c" >/dev/null 2>&1; then OBJDUMP="$c"; break; fi
done
if [ -n "$OBJDUMP" ] && [ -f "$OUT/mi14-windows-on.exe" ]; then
    dis="$OUT/mi14-windows-on.dis"
    "$OBJDUMP" -d "$OUT/mi14-windows-on.exe" >"$dis" 2>/dev/null
fi
if [ -n "$OBJDUMP" ] && [ -s "${dis:-/nonexistent}" ] && grep -q '%rsp' "$dis"; then
    st6=$(grep -cE 'movdqu[[:space:]]+%xmm6,[[:space:]]*\(%rsp\)' "$dis")
    st15=$(grep -cE 'movdqu[[:space:]]+%xmm15,[[:space:]]*0x90\(%rsp\)' "$dis")
    ld15=$(grep -cE 'movdqu[[:space:]]+0x90\(%rsp\),[[:space:]]*%xmm15' "$dis")
    if [ "$st6" -gt 0 ] && [ "$st15" -gt 0 ] && [ "$ld15" -gt 0 ]; then
        ok "--nat Windows: the exception handler saves and restores xmm6..xmm15"
    else
        bad "--nat Windows: no save/restore of xmm6..xmm15 found (xmm6 stores $st6, xmm15 stores $st15, loads $ld15)"
    fi
else
    echo "skip disassembly check (no objdump or no Windows image)"
fi

# 5. other float / split programs through --nat Windows
for p in tests/bootstrap/test_machine_fregs.fd tests/bootstrap/test_machine_split.fd; do
    [ -f "$p" ] || continue
    n=$(basename "$p" .fd)
    e="mi14-$n-nat.exe"
    if emit_one "$p" windows "$OUT/$e" "$OUT/$e.log"; then
        check_run "$p" windows "$e" "--nat Windows $n"
    else
        bad "--nat Windows $n: emit failed: $(grep -v '^\[' "$OUT/$e.log" | tail -2)"
    fi
done

echo "$fails failure(s)"
if [ "$fails" -eq 0 ]; then echo "machine_mi14_check: all passed"; fi
[ "$fails" -eq 0 ]
