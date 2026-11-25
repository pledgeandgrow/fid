#!/usr/bin/env bash
# ME28: a `match` on a narrow integer (u8, u16, i16, i32, u32) becomes a
# Switch like one on an i64 (compiler/ir.fd ir_sw_word), on the C path (a C
# `switch`) and on the machine layer (MI27's jump tables, bit tests or a
# tree: compiler/nat_ir.fd ng_gen_switch). Run after the compiler is built:
#   bash tests/nattools/machine_switch_narrow_check.sh [fid]   (default release/fid.exe)
#
# 1. C path: `dump-c` of tests/bootstrap/test_irswitch_narrow.fd holds a
#    C `switch ((int64_t)` in each of digit, is_space, sparse16, opcode,
#    small_dense, big32 and signed32 (7 or more); with FID_SWITCH=off none.
#    `ccompile` (when a C compiler is found) prints the expected output.
# 2. Machine layer, for x86_64-linux, aarch64-linux, riscv64-linux and
#    --nat Windows, built with FID_VERIFY=1 by default and with
#    FID_SWITCH=off: the default MIR (FID_NAT_DUMP='*') holds a `jtab` in
#    digit, opcode and small_dense (dense keys) and none in is_space (a bit
#    test), sparse16, big32 and signed32 (a tree); FID_SWITCH=off none at
#    all. Both print the file's expected output where this host can run
#    them (WSL / qemu-user on Windows; a target without a runner is built
#    and checked only).
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-machine-switch-narrow"
mkdir -p "$OUT"
SRC=tests/bootstrap/test_irswitch_narrow.fd
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }
WANT="$(expected "$SRC")"

HOST=linux
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; esac
WSL=""
# command -v wsl.exe alone is not enough: the launcher stays on PATH when
# no distribution is installed, and a run then prints WSL's error instead
# of the program's output. Probe that a command really runs inside WSL.
if [ "$HOST" = windows ] && command -v wsl.exe >/dev/null 2>&1 && wsl.exe -e true >/dev/null 2>&1; then WSL="wsl.exe -e"; fi

# run_exe <exe name> <target>: the program's output; status 99 when this
# host cannot run it
run_exe() {
    local exe="$1" t="$2" q=""
    case "$t" in
        aarch64-linux) q=qemu-aarch64 ;;
        riscv64-linux) q=qemu-riscv64 ;;
    esac
    case "$t" in
        *-linux)
            if [ "$HOST" = windows ] && [ -z "$WSL" ]; then return 99; fi
            if [ -n "$q" ]; then
                if [ -n "$WSL" ]; then
                    (cd "$OUT" && $WSL sh -c "command -v $q >/dev/null") || return 99
                elif ! command -v "$q" >/dev/null 2>&1; then
                    if [ "$(uname -m)" = "${t%%-*}" ]; then q=""; else return 99; fi
                fi
            fi
            (cd "$OUT" && $WSL $q "./$exe" 2>&1 | tr -d '\r')
            ;;
        windows)
            [ "$HOST" = windows ] || return 99
            (cd "$OUT" && "./$exe" 2>&1 | tr -d '\r')
            ;;
    esac
}

# the functions whose MIR (last dump of each) holds a jtab
jtab_fns() {
    awk '/^\[mir\] [^ ]+ after .*: [0-9]+ operations/ { fn = $2; next }
         /^\[mir\] [0-9]+: jtab / { j[fn] = 1 }
         END { for (f in j) print f }' "$1" | sort
}

# --- 1. the C path -----------------------------------------------------
if "$FID" dump-c "$SRC" >"$OUT/on.c" 2>"$OUT/on.log"; then
    n_on=$(grep -c 'switch ((int64_t)' "$OUT/on.c")
    if [ "$n_on" -ge 7 ]; then
        ok "C: $n_on switches formed on narrow values"
    else
        bad "C: $n_on switches (want 7 or more: digit, is_space, sparse16, opcode, small_dense, big32, signed32)"
    fi
else
    bad "C: dump-c failed: $(grep -v '^\[' "$OUT/on.log" | tail -2)"
fi
if FID_SWITCH=off "$FID" dump-c "$SRC" >"$OUT/off.c" 2>"$OUT/off.log"; then
    n_off=$(grep -c 'switch ((int64_t)' "$OUT/off.c")
    if [ "$n_off" -eq 0 ]; then
        ok "C: no switch with FID_SWITCH=off"
    else
        bad "C: $n_off switches with FID_SWITCH=off"
    fi
else
    bad "C: dump-c (FID_SWITCH=off) failed"
fi
cexe="$OUT/narrow_c"
[ "$HOST" = windows ] && cexe="$OUT/narrow_c.exe"
if "$FID" ccompile "$SRC" -o "$cexe" >"$OUT/cc.log" 2>&1; then
    got=$("$cexe" 2>&1 | tr -d '\r')
    if [ "$got" = "$WANT" ]; then
        ok "C: output as expected"
    else
        bad "C: output differs"
        diff <(echo "$WANT") <(echo "$got") | head -10
    fi
else
    echo "skip C run (ccompile failed: no C compiler?)"
fi

# --- 2. the machine layer ------------------------------------------------
for t in x86_64-linux aarch64-linux riscv64-linux windows; do
    for mode in default noswitch; do
        name="narrow-$t-$mode"
        exe="$name"
        targ=(--target "$t")
        sw=(FID_NAT_DUMP='*')
        [ "$t" = windows ] && exe="$name.exe" && targ=(--nat)
        [ "$mode" = noswitch ] && sw+=(FID_SWITCH=off)
        # FID_IROPT_OFF=inline2: keep the test functions out of the
        # inliner — the MIR dump is what is checked, and a folded
        # digit/opcode/small_dense shows no jtab however it was emitted
        if ! env FID_VERIFY=1 FID_IROPT_OFF=inline2 "${sw[@]}" "$FID" emit "$SRC" "${targ[@]}" -o "$OUT/$exe" >"$OUT/$name.out" 2>"$OUT/$name.log"; then
            bad "$t $mode: emit failed: $(grep -v '^\[' "$OUT/$name.log" | tail -2)"
            continue
        fi
        have="$(jtab_fns "$OUT/$name.log" | tr '\n' ' ')"
        if [ "$mode" = default ]; then
            miss=""
            for f in digit opcode small_dense; do
                case " $have " in *" $f "*) ;; *) miss="$miss $f" ;; esac
            done
            extra=""
            for f in is_space sparse16 big32 signed32; do
                case " $have " in *" $f "*) extra="$extra $f" ;; esac
            done
            if [ -z "$miss" ] && [ -z "$extra" ]; then
                ok "$t: jump tables on narrow values in the dense matches only ($have)"
            else
                bad "$t: jump tables missing in:$miss; unexpected in:$extra"
            fi
        elif [ -n "$have" ]; then
            bad "$t $mode: a jump table with the switch off: $have"
        else
            ok "$t $mode: no jump table"
        fi
        got=$(run_exe "$exe" "$t")
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

echo "$fails failure(s)"
[ "$fails" -eq 0 ]
