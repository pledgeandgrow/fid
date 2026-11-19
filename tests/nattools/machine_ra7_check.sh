#!/usr/bin/env bash
# Checks of RA7 (register hints for arguments, results and call operands)
# of the machine layer (compiler/nat_ir.fd ng_arg_hints, the implicit
# register uses of ng_implicit_regs, the checker ng_mir_check_args), run
# after the compiler is built:
#   bash tests/nattools/machine_ra7_check.sh [fid]   (default release/fid.exe)
#
# For x86_64-linux, aarch64-linux, riscv64-linux and --nat Windows,
# tests/bootstrap/test_machine_arghint.fd is built with FID_VERIFY=1 (MIR
# verifier and allocation checker) twice: with FID_NAT_ON=arghint (the
# default since Nov 17) and with FID_NAT_OFF=arghint. Both must build and, where the image runs, print the file's
# expected output. With the switch `--stats` must report ranges placed in
# argument registers ("[arghint] f: N ranges in argument registers", N > 0
# summed over the functions) and the moves it counts ("[stats] f: ...,
# moves N") must not grow on any target and must shrink on AArch64 and
# RISC-V; without it no [arghint] line may appear.
# Then FID_NAT_MUTATE=arghint (the allocator ignores the implicit uses, so
# a call operand of `across` is left in an argument register across
# another call) must make the FID_VERIFY=1 build fail with the checker's
# "argument register" message on every target.
# FILES="a.fd b.fd" builds other programs with FID_VERIFY=1 and the switch
# too, and compares their output with `fid run` where the image runs
# (FILES="$(ls tests/bootstrap/test_*.fd | grep -v _fail)" for the suite).
#
# Linux executables run directly on Linux (AArch64 / RISC-V under
# qemu-aarch64 / qemu-riscv64), through WSL on Windows (`wsl.exe -e`); a
# target whose runner is missing is built and checked only. Exit 1 on a
# failure.
set -u
FID="${1:-release/fid.exe}"
SRC=tests/bootstrap/test_machine_arghint.fd
OUT="${TMPDIR:-/tmp}/fid-machine-ra7"
mkdir -p "$OUT"
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }
WANT="$(expected "$SRC")"

HOST=linux
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; esac
WSL=""
if [ "$HOST" = windows ] && command -v wsl.exe >/dev/null 2>&1; then WSL="wsl.exe -e"; fi

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

moves() { sed -n 's/^\[stats\] [^:]*: .*moves \([0-9]*\).*/\1/p' "$1" | awk '{s += $1} END {print s + 0}'; }

for t in x86_64-linux aarch64-linux riscv64-linux windows; do
    sfx=""
    [ "$t" = windows ] && sfx=".exe"
    on="ra7-$t-on$sfx"
    off="ra7-$t-off$sfx"
    if ! emit_one "$SRC" "$t" "$OUT/$on" "$OUT/$on.log" FID_NAT_ON=arghint; then
        bad "$t on: emit failed: $(grep -v '^\[' "$OUT/$on.log" | tail -2)"
        continue
    fi
    # (the hints are on by default since Nov 17: "off" turns them off)
    if ! emit_one "$SRC" "$t" "$OUT/$off" "$OUT/$off.log" FID_NAT_OFF=arghint; then
        bad "$t off: emit failed: $(grep -v '^\[' "$OUT/$off.log" | tail -2)"
        continue
    fi
    ok "$t: both build with FID_VERIFY=1"
    nh=$(sed -n 's/^\[arghint\] [^:]*: \([0-9]*\) ranges.*/\1/p' "$OUT/$on.log" | awk '{s += $1} END {print s + 0}')
    if [ "$nh" -gt 0 ]; then ok "$t on: $nh range(s) in argument registers"; else bad "$t on: no [arghint] line with ranges"; fi
    if grep -q '^\[arghint\]' "$OUT/$off.log"; then bad "$t off: [arghint] line without the switch"; else ok "$t off: no [arghint] line"; fi
    mon=$(moves "$OUT/$on.log")
    moff=$(moves "$OUT/$off.log")
    if [ "$mon" -gt "$moff" ]; then
        bad "$t: more moves with the hints ($mon) than without ($moff)"
    elif [ "$mon" -eq "$moff" ] && { [ "$t" = aarch64-linux ] || [ "$t" = riscv64-linux ]; }; then
        bad "$t: the hints removed no move ($mon)"
    else
        ok "$t: moves $moff -> $mon with the hints"
    fi
    for mode in on off; do
        exe="ra7-$t-$mode$sfx"
        got=$(run_exe "$OUT" "$exe" "$t")
        rc=$?
        if [ $rc -eq 99 ]; then
            ok "$t $mode: not run on this host"
        elif [ "$got" = "$WANT" ]; then
            ok "$t $mode: output as expected"
        else
            bad "$t $mode: output differs"
            diff <(echo "$WANT") <(echo "$got") | head -10
        fi
    done
    # the mutation: implicit uses ignored; the checker must refuse it
    mut="ra7-$t-mut$sfx"
    if emit_one "$SRC" "$t" "$OUT/$mut" "$OUT/$mut.log" FID_NAT_ON=arghint FID_NAT_MUTATE=arghint; then
        bad "$t: FID_NAT_MUTATE=arghint built under FID_VERIFY=1 (the checker missed it)"
    elif grep -q 'argument register' "$OUT/$mut.log"; then
        ok "$t: the checker rejects the mutation ($(grep -o '[a-z_0-9]*: [^:]*argument register [0-9]*[^.]*' "$OUT/$mut.log" | head -1))"
    else
        bad "$t: the mutated build failed without the checker's message: $(grep -v '^\[' "$OUT/$mut.log" | tail -2)"
    fi
done

# other programs (FILES=...), with the switch, against `fid run`
for src in ${FILES:-}; do
    want=$("$FID" run "$src" 2>&1 | tr -d '\r')
    for t in x86_64-linux aarch64-linux riscv64-linux windows; do
        sfx=""
        [ "$t" = windows ] && sfx=".exe"
        exe="f-$(basename "$src" .fd)-$t$sfx"
        if ! emit_one "$src" "$t" "$OUT/$exe" "$OUT/$exe.log" FID_NAT_ON=arghint; then
            bad "$src $t: emit failed: $(grep -v '^\[' "$OUT/$exe.log" | tail -1)"
            continue
        fi
        got=$(run_exe "$OUT" "$exe" "$t")
        rc=$?
        if [ $rc -eq 99 ]; then
            ok "$src $t: built with FID_VERIFY=1"
        elif [ "$got" = "$want" ]; then
            ok "$src $t: output as with fid run"
        else
            bad "$src $t: output differs from fid run"
        fi
    done
done

if [ "$fails" -gt 0 ]; then echo "$fails failure(s)"; exit 1; fi
echo "all passed"
