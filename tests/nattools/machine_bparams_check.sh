#!/usr/bin/env bash
# Checks of MI1 (block parameters of the machine IR, compiler/mir.fd;
# docs/reference/mir.md), after the compiler is built:
#   bash tests/nattools/machine_bparams_check.sh [fid]   (default release/fid.exe)
#
#  1. tests/nattools/bparam_unit.fd (compiler/mir.fd alone: the form of a
#     loop, a cyclic parallel copy, verifier mutants, 300 random functions
#     run before the form, in it and after the lowering) through C (CC or
#     zig cc; `fid run` without one) must print its expected lines.
#  2. For x86_64-linux, aarch64-linux, riscv64-linux and --nat Windows,
#     tests/bootstrap/test_machine_bparams.fd is built with FID_VERIFY=1
#     and --stats, without and with FID_NAT_ON=bparams: both must build
#     (the form and the lowered MIR pass the verifier, the form is SSA,
#     the allocation checker passes), the second must print `[bparams]`
#     lines with parameters, FID_NAT_DUMP=sum_to must show `bparam` and
#     `barg` operations after "block parameters" and none after "block
#     parameters taken out", and each image is run where this host can
#     (WSL / qemu-user for Linux, Windows directly) and must print the
#     file's expected output.
#  3. FILES="a.fd b.fd" builds other programs the same way with
#     FID_NAT_ON=bparams FID_VERIFY=1 and compares them with `fid run`
#     (FILES="$(ls tests/bootstrap/test_*.fd | grep -v _fail)" for the
#     suite). Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
SRC=tests/bootstrap/test_machine_bparams.fd
OUT="${TMPDIR:-/tmp}/fid-machine-bparams"
mkdir -p "$OUT"
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

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

# 1. the unit test of compiler/mir.fd
UNIT=tests/nattools/bparam_unit.fd
uwant="$(expected "$UNIT")"
UEXE="$OUT/bparam_unit"
[ "$HOST" = windows ] && UEXE="$UEXE.exe"
if [ -z "${CC:-}" ] && command -v zig >/dev/null 2>&1; then export CC="zig cc"; fi
if [ -n "${CC:-}" ] && "$FID" ccompile "$UNIT" -o "$UEXE" >"$OUT/bparam_unit.log" 2>&1; then
    ugot=$("$UEXE" 2>&1 | tr -d '\r' | grep -v '^\[')
else
    echo "info bparam_unit.fd: not built through C (CC unset or ccompile failed: $OUT/bparam_unit.log); running it"
    ugot=$("$FID" run "$UNIT" 2>&1 | tr -d '\r' | grep -v '^\[')
fi
if [ "$ugot" = "$uwant" ]; then
    ok "bparam_unit.fd: as expected"
else
    bad "bparam_unit.fd differs"
    diff <(echo "$uwant") <(echo "$ugot") | head -20
fi

# 2. the test program on every machine-layer target, without and with
WANT="$(expected "$SRC")"
for t in x86_64-linux aarch64-linux riscv64-linux windows; do
    sfx=""
    [ "$t" = windows ] && sfx=".exe"
    for mode in off on; do
        exe="bp-$t-$mode$sfx"
        envs=()
        [ "$mode" = on ] && envs=(FID_NAT_ON=bparams)
        if ! emit_one "$SRC" "$t" "$OUT/$exe" "$OUT/$exe.log" ${envs[@]+"${envs[@]}"}; then
            bad "$t $mode: emit failed: $(grep -v '^\[' "$OUT/$exe.log" | tail -2)"
            continue
        fi
        if [ "$mode" = on ]; then
            np=$(sed -n 's/^\[bparams\] [^:]*: \([0-9]*\) parameters.*/\1/p' "$OUT/$exe.log" | awk '{s += $1} END {print s + 0}')
            if [ "$np" -gt 0 ]; then ok "$t on: $np block parameter(s) formed"; else bad "$t on: no [bparams] line with parameters"; fi
        elif grep -q '^\[bparams\]' "$OUT/$exe.log"; then
            bad "$t off: a [bparams] line without FID_NAT_ON=bparams"
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
    # every function's MIR in the form and after it (FID_NAT_DUMP=*):
    # count bparam / barg lines in each listing
    dexe="bp-$t-dump$sfx"
    if emit_one "$SRC" "$t" "$OUT/$dexe" "$OUT/$dexe.log" FID_NAT_ON=bparams 'FID_NAT_DUMP=*'; then
        counts=$(awk '
            /^\[mir\] [^ ]* after block parameters taken out:/ { st = 2; next }
            /^\[mir\] [^ ]* after block parameters:/ { st = 1; next }
            /^\[mir\] [^ ]* after / || /^\[mir\] [^ ]*: [0-9]* operations/ { st = 0; next }
            / bparam | barg / { if (st == 1) f++; if (st == 2) o++ }
            END { print f + 0, o + 0 }' "$OUT/$dexe.log")
        nform=${counts% *}
        nout=${counts#* }
        if [ "$nform" -gt 0 ]; then
            ok "$t: $nform bparam/barg operations in the form"
        else
            bad "$t: no bparam/barg in the MIR after block parameters"
        fi
        if [ "$nout" -eq 0 ]; then
            ok "$t: none left after the lowering"
        else
            bad "$t: $nout bparam/barg left after block parameters taken out"
        fi
    else
        bad "$t: dump build failed: $(grep -v '^\[' "$OUT/$dexe.log" | tail -2)"
    fi
done

# 3. other programs (FILES=...) with block parameters, against `fid run`
for src in ${FILES:-}; do
    want=$("$FID" run "$src" 2>&1 | tr -d '\r')
    for t in x86_64-linux aarch64-linux riscv64-linux windows; do
        sfx=""
        [ "$t" = windows ] && sfx=".exe"
        exe="f-$(basename "$src" .fd)-$t$sfx"
        if ! emit_one "$src" "$t" "$OUT/$exe" "$OUT/$exe.log" FID_NAT_ON=bparams; then
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

if [ "$fails" -eq 0 ]; then
    echo "all passed"
    exit 0
fi
echo "$fails failed"
exit 1
