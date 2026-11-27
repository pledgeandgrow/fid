#!/usr/bin/env bash
# Checks of MI25 (floats in float registers, f32 kept as f32) and RA10
# (register classes) of the machine layer (compiler/nat_ir.fd), run after
# the compiler is built:
#   bash tests/nattools/machine_fregs_check.sh [fid]   (default release/fid.exe)
#
# For x86_64-linux, aarch64-linux, riscv64-linux and --nat Windows, the
# test program tests/bootstrap/test_machine_fregs.fd is built twice with
# FID_VERIFY=1 (the allocation checker: each value's register in its
# class's pool, no call clobbering a float register that holds a live
# value): with FID_NAT_ON=fregs (floats in float registers) and with
# FID_NAT_OFF=fregs (every float in a general register). Both must print
# the file's expected output. With the switch on, `--stats` must report
# float ranges given float registers ("[fregs] f: N float ranges, M in
# float registers" with M > 0 summed over the functions); with it off,
# no such line. When llvm-objdump (or the
# target's objdump) is found, the x86-64 binary built with the switch must
# hold SSE arithmetic on xmm2..xmm11 and the one without it none.
#
# Linux executables run directly on Linux (AArch64 / RISC-V under
# qemu-aarch64 / qemu-riscv64), through WSL on Windows (`wsl.exe -e`);
# a target whose runner is missing is built and checked only. The
# Windows executable runs on a Windows host. Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
SRC=tests/bootstrap/test_machine_fregs.fd
OUT="${TMPDIR:-/tmp}/fid-machine-fregs"
mkdir -p "$OUT"
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }
WANT="$(expected "$SRC")"

HOST=linux
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; esac
WSL=""
# A working WSL only: wsl.exe is on PATH on every Windows, with no
# distribution installed too, and then every image "fails" with no output
if [ "$HOST" = windows ] && command -v wsl.exe >/dev/null 2>&1 && wsl.exe -e sh -c 'echo wsl-ok' 2>/dev/null | grep -q 'wsl-ok'; then WSL="wsl.exe -e"; fi

# run <dir> <exe name> <target>: prints the program's output (empty and
# status 99 when this host cannot run it)
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

OBJDUMP=""
for c in llvm-objdump objdump; do
    if command -v "$c" >/dev/null 2>&1; then OBJDUMP="$c"; break; fi
done

for t in x86_64-linux aarch64-linux riscv64-linux windows; do
    for mode in on off default; do
        name="fregs-$t-$mode"
        exe="$name"
        targ=(--target "$t")
        if [ "$t" = windows ]; then exe="$name.exe"; targ=(--nat); fi
        if [ "$mode" = on ]; then
            sw=(FID_NAT_ON=fregs)
        elif [ "$mode" = off ]; then
            sw=(FID_NAT_OFF=fregs)
        else
            # OP17 / MI14: no switch. Floats get float registers by
            # default on every target, --nat Windows included (its pool
            # xmm2..xmm11 as on Linux, the vectors in xmm12..xmm15)
            sw=(FID_NAT_DEFAULT=1)
            mode=on
        fi
        if ! env FID_VERIFY=1 "${sw[@]}" "$FID" emit "$SRC" "${targ[@]}" --stats -o "$OUT/$exe" >"$OUT/$name.log" 2>&1; then
            bad "$t $mode: emit failed: $(grep -v '^\[' "$OUT/$name.log" | tail -2)"
            continue
        fi
        # the float ranges the allocator placed (switch on) / none (off)
        acc=$(grep '^\[fregs\]' "$OUT/$name.log" | sed -n 's/.*, \([0-9]*\) in float registers.*/\1/p' | awk '{s += $1} END {print s + 0}')
        nfr=$(grep -c '^\[fregs\]' "$OUT/$name.log")
        if [ "$mode" = on ]; then
            if [ "$acc" -gt 0 ]; then
                ok "$t on: $acc float range(s) in float registers ($nfr functions with float ranges)"
            else
                bad "$t on: no float range got a float register (stats: $(grep '^\[fregs\]' "$OUT/$name.log" | head -3 | tr '\n' ' '))"
            fi
        elif [ "$nfr" -ne 0 ]; then
            bad "$t off: FID_NAT_OFF=fregs still placed floats in float registers"
        else
            ok "$t off: no float register handed out"
        fi
        got=$(run_exe "$OUT" "$exe" "$t")
        rc=$?
        if [ $rc -eq 99 ]; then
            ok "$t $mode: built with FID_VERIFY=1 (not run on this host)"
        elif [ "$got" = "$WANT" ]; then
            ok "$t $mode: output as expected"
        else
            bad "$t $mode: output differs"
            diff <(echo "$WANT") <(echo "$got") | head -10
        fi
    done
done

# x86-64: SSE arithmetic (addsd/subsd/mulsd/divsd/addss/..) on a pool
# register with the switch, never without it. Counted from the first
# function symbol on (the section's own header `<.text>` is none): nat_ir's
# float primitives (the round stub) come before, with xmm2/xmm3 as scratch
# whatever the switch says.
if [ -n "$OBJDUMP" ]; then
    pool_count() {
        "$OBJDUMP" -d "$1" 2>/dev/null | awk '/^[0-9a-f]+ <[^.][^>]*>:/ { named = 1 } named && /(add|sub|mul|div)s[sd][[:space:]].*%xmm([2-9]|1[01])([^0-9]|$)/ { c++ } END { print c + 0 }'
    }
    don=$(pool_count "$OUT/fregs-x86_64-linux-on")
    doff=$(pool_count "$OUT/fregs-x86_64-linux-off")
    if [ "$don" -gt 0 ] && [ "$doff" -eq 0 ]; then
        ok "x86-64: $don float operations on xmm2..xmm11 with the switch, none without"
    else
        bad "x86-64: float operations on xmm2..xmm11: $don with the switch, $doff without"
    fi
else
    echo "skip disassembly check (no objdump)"
fi

echo "$fails failure(s)"
[ "$fails" -eq 0 ]
