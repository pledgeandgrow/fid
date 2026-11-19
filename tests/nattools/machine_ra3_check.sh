#!/usr/bin/env bash
# Checks of RA3 (the caller-saved argument registers for ranges no call
# crosses) of the machine layer (compiler/nat_ir.fd ng_arg_free over RA7's
# model ng_ra7_info / ng_implicit_regs, the checker ng_mir_check_args), run
# after the compiler is built:
#   bash tests/nattools/machine_ra3_check.sh [fid]   (default release/fid.exe)
#
# 1. tests/nattools/argfree_unit.fd through `fid run`: its expected output
#    (fill, trade, a trade kept back by a copy, the result word in rdi,
#    the mutation rejected).
# 2. For x86_64-linux, aarch64-linux, riscv64-linux and --nat Windows,
#    tests/bootstrap/test_machine_argfree.fd is built with FID_VERIFY=1
#    (MIR verifier and allocation checker) twice: as by default (RA3 on)
#    and with FID_NAT_OFF=argfree. Both must build and, where the image
#    runs, print the file's expected output. With RA3 `--stats` must
#    report ranges moved ("[argfree] f: N spilled ranges in argument
#    registers, M ranges moved off K callee-saved registers, ...",
#    N + M > 0 summed over the functions) on x86-64 (Linux and Windows);
#    without it no [argfree] line may appear. The reloads `--stats`
#    counts ("[stats] f: spilled S, reloads R, ...", summed) must not grow
#    on any target and must shrink on x86-64.
# 3. FID_NAT_MUTATE=argfree (the implicit uses ignored, so a range may take
#    an argument register across a call or a division) must make the
#    FID_VERIFY=1 build fail with the checker's "argument register"
#    message on every target.
# FILES="a.fd b.fd" builds other programs with FID_VERIFY=1 (RA3 on) too
# and compares their output with `fid run` where the image runs
# (FILES="$(ls tests/bootstrap/test_*.fd | grep -v _fail)" for the suite).
#
# Linux executables run directly on Linux (AArch64 / RISC-V under
# qemu-aarch64 / qemu-riscv64), through WSL on Windows (`wsl.exe -e`); a
# target whose runner is missing is built and checked only. Exit 1 on a
# failure.
set -u
FID="${1:-release/fid.exe}"
SRC=tests/bootstrap/test_machine_argfree.fd
UNIT=tests/nattools/argfree_unit.fd
OUT="${TMPDIR:-/tmp}/fid-machine-ra3"
mkdir -p "$OUT"
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

HOST=linux
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; esac
WSL=""
if [ "$HOST" = windows ] && command -v wsl.exe >/dev/null 2>&1; then WSL="wsl.exe -e"; fi

# 1. the unit test
UWANT="$(expected "$UNIT")"
UGOT="$("$FID" run "$UNIT" 2>&1 | tr -d '\r' | grep -v '^\[')"
if [ "$UGOT" = "$UWANT" ]; then
    ok "argfree_unit: output as expected"
else
    bad "argfree_unit: output differs"
    diff <(echo "$UWANT") <(echo "$UGOT") | head -20
fi

WANT="$(expected "$SRC")"

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

reloads() { sed -n 's/^\[stats\] [^:]*: .*reloads \([0-9]*\).*/\1/p' "$1" | awk '{s += $1} END {print s + 0}'; }
moved() { sed -n 's/^\[argfree\] [^:]*: \([0-9]*\) spilled ranges in argument registers, \([0-9]*\) ranges moved.*/\1 \2/p' "$1" | awk '{s += $1 + $2} END {print s + 0}'; }

for t in x86_64-linux aarch64-linux riscv64-linux windows; do
    sfx=""
    [ "$t" = windows ] && sfx=".exe"
    on="ra3-$t-on$sfx"
    off="ra3-$t-off$sfx"
    if ! emit_one "$SRC" "$t" "$OUT/$on" "$OUT/$on.log"; then
        bad "$t on: emit failed: $(grep -v '^\[' "$OUT/$on.log" | tail -2)"
        continue
    fi
    if ! emit_one "$SRC" "$t" "$OUT/$off" "$OUT/$off.log" FID_NAT_OFF=argfree; then
        bad "$t off: emit failed: $(grep -v '^\[' "$OUT/$off.log" | tail -2)"
        continue
    fi
    ok "$t: both build with FID_VERIFY=1"
    nm=$(moved "$OUT/$on.log")
    case "$t" in
        x86_64-linux|windows)
            if [ "$nm" -gt 0 ]; then ok "$t on: $nm range(s) moved to argument registers"; else bad "$t on: no [argfree] line with ranges"; fi
            ;;
        *)
            ok "$t on: $nm range(s) moved to argument registers"
            ;;
    esac
    if grep -q '^\[argfree\]' "$OUT/$off.log"; then bad "$t off: [argfree] line with FID_NAT_OFF=argfree"; else ok "$t off: no [argfree] line"; fi
    ron=$(reloads "$OUT/$on.log")
    roff=$(reloads "$OUT/$off.log")
    if [ "$ron" -gt "$roff" ]; then
        bad "$t: more reloads with RA3 ($ron) than without ($roff)"
    elif [ "$ron" -eq "$roff" ] && { [ "$t" = x86_64-linux ] || [ "$t" = windows ]; }; then
        bad "$t: RA3 removed no reload ($ron)"
    else
        ok "$t: reloads $roff -> $ron with RA3"
    fi
    for mode in on off; do
        exe="ra3-$t-$mode$sfx"
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
    mut="ra3-$t-mut$sfx"
    if emit_one "$SRC" "$t" "$OUT/$mut" "$OUT/$mut.log" FID_NAT_MUTATE=argfree; then
        bad "$t: FID_NAT_MUTATE=argfree built under FID_VERIFY=1 (the checker missed it)"
    elif grep -q 'argument register' "$OUT/$mut.log"; then
        ok "$t: the checker rejects the mutation ($(grep -o '[a-z_0-9]*: [^:]*argument register [0-9]*[^.]*' "$OUT/$mut.log" | head -1))"
    else
        bad "$t: the mutated build failed without the checker's message: $(grep -v '^\[' "$OUT/$mut.log" | tail -2)"
    fi
done

# other programs (FILES=...), RA3 on, against `fid run`
for src in ${FILES:-}; do
    want=$("$FID" run "$src" 2>&1 | tr -d '\r')
    for t in x86_64-linux aarch64-linux riscv64-linux windows; do
        sfx=""
        [ "$t" = windows ] && sfx=".exe"
        exe="f-$(basename "$src" .fd)-$t$sfx"
        if ! emit_one "$src" "$t" "$OUT/$exe" "$OUT/$exe.log"; then
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
