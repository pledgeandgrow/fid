#!/usr/bin/env bash
# WP6 (docs/roadmap/llvm-roadmap.md): arguments in the registers the caller
# already has them in, on the machine layer (compiler/nat_ir.fd). The goal
# is met by RA7's argument-register hints (ng_arg_hints) and RA3's free
# argument registers (ng_arg_free), both on by default, over WP6's internal
# convention (ng_argreg_i / ng_nregargs_i, `internal_conv`). Run after the
# compiler is built:
#   bash tests/nattools/wp6_args_check.sh [fid]   (default release/fid.exe)
#
# For x86_64-linux, aarch64-linux, riscv64-linux and --nat Windows,
# tests/bootstrap/test_machine_wp6_args.fd is built with FID_VERIFY=1 (MIR
# verifier, allocation checker, ng_mir_check_args) and --stats twice: with
# the defaults and with FID_NAT_OFF=arghint,argfree. Both must build and,
# where the image runs, print the file's expected output.
# `fwd3` and `fwd5` pass their parameters on in the same positions after a
# loop that needs no argument register: on the three Linux targets each
# must count fewer moves ("[stats] f: ..., moves N") with the defaults than
# without the hints (the parameter copy and the operand copy become moves
# to themselves); on --nat Windows not more. Summed over fwd3, fwd5, swap2,
# rot3 and twice the moves must not grow on any target.
#
# Linux executables run directly on Linux (AArch64 / RISC-V under
# qemu-aarch64 / qemu-riscv64), through WSL on Windows (`wsl.exe -e`); a
# target whose runner is missing is built and checked only. Exit 1 on a
# failure.
set -u
FID="${1:-release/fid.exe}"
SRC=tests/bootstrap/test_machine_wp6_args.fd
OUT="${TMPDIR:-/tmp}/fid-wp6-args"
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

# moves <log> <function>: the moves --stats counts in that function (and
# its specialized copies, whose names start with it), summed
moves() {
    sed -n "s/^\[stats\] $2[^:]*: .*moves \([0-9]*\).*/\1/p" "$1" | awk '{s += $1} END {print s + 0}'
}
has_fn() { grep -q "^\[stats\] $2[^:]*: " "$1"; }

for t in x86_64-linux aarch64-linux riscv64-linux windows; do
    sfx=""
    [ "$t" = windows ] && sfx=".exe"
    on="wp6-$t-on$sfx"
    off="wp6-$t-off$sfx"
    if ! emit_one "$SRC" "$t" "$OUT/$on" "$OUT/$on.log"; then
        bad "$t defaults: emit failed: $(grep -v '^\[' "$OUT/$on.log" | tail -2)"
        continue
    fi
    if ! emit_one "$SRC" "$t" "$OUT/$off" "$OUT/$off.log" FID_NAT_OFF=arghint,argfree; then
        bad "$t off: emit failed: $(grep -v '^\[' "$OUT/$off.log" | tail -2)"
        continue
    fi
    ok "$t: both build with FID_VERIFY=1"
    for f in fwd3 fwd5; do
        if ! has_fn "$OUT/$on.log" "$f"; then
            bad "$t: no [stats] line for $f (inlined or renamed?)"
            continue
        fi
        mon=$(moves "$OUT/$on.log" "$f")
        moff=$(moves "$OUT/$off.log" "$f")
        if [ "$mon" -gt "$moff" ]; then
            bad "$t $f: more moves with the hints ($mon) than without ($moff)"
        elif [ "$mon" -eq "$moff" ] && [ "$t" != windows ]; then
            bad "$t $f: the hints removed no move ($mon): its parameters are not passed on in place"
        else
            ok "$t $f: moves $moff -> $mon with the hints"
        fi
    done
    son=0
    soff=0
    for f in fwd3 fwd5 swap2 rot3 twice; do
        son=$((son + $(moves "$OUT/$on.log" "$f")))
        soff=$((soff + $(moves "$OUT/$off.log" "$f")))
    done
    if [ "$son" -gt "$soff" ]; then
        bad "$t: more moves in all with the hints ($son) than without ($soff)"
    else
        ok "$t: moves in all $soff -> $son"
    fi
    if grep -q '^\[arghint\]\|^\[argfree\]' "$OUT/$off.log"; then
        bad "$t off: an [arghint] or [argfree] line with both off"
    fi
    for mode in on off; do
        exe="wp6-$t-$mode$sfx"
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
done

if [ "$fails" -gt 0 ]; then echo "$fails failure(s)"; exit 1; fi
echo "all passed"
