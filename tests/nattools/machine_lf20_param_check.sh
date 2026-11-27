#!/usr/bin/env bash
# LF20 (docs/roadmap/llvm-roadmap.md) on the machine layer: the secret
# values the allocator keeps in registers first (compiler/nat_ir.fd
# ng_secret_ids) start at the front end's per-value list
# IrFunction.secret_values (compiler/lower.fd: the values of @secret /
# @sensitive parameters, the slots of @secret / @sensitive / @zeroize
# locals; compiler/ir_ser.fd writes it, IR file version 3), not only at
# @zeroize locals. Run after the compiler is built:
#   bash tests/nattools/machine_lf20_param_check.sh [fid]   (default release/fid.exe)
#
# For x86_64-linux, aarch64-linux, riscv64-linux (and --nat Windows on a
# Windows host), with tests/bootstrap/test_machine_secret_param.fd:
#  1. Built with FID_VERIFY=1 and --stats: "[lf20] press_param: S secret
#     values, R in registers" with S > 0 and R == S (a @secret parameter
#     alone seeds it: before the list S was 0), the same for press_local
#     (a @sensitive local) and S > 0 for assigned (an assigned @secret
#     parameter); the expected output where the image runs.
#  2. FID_NAT_OFF=secretregs: builds, same output (R printed, not checked).
# Exit 1 on a failure; a target the host cannot run is build-only.
set -u
FID="${1:-release/fid.exe}"
SRC=tests/bootstrap/test_machine_secret_param.fd
OUT="${TMPDIR:-/tmp}/fid-machine-lf20-param"
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

# run_exe <dir> <exe> <target>: the program's output; status 99 when this
# host cannot run it
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

# build <name> <target> [env...]: emit with --stats into $OUT/<name>
build() {
    local name="$1" t="$2"
    shift 2
    local targ=(--target "$t") exe="$name"
    if [ "$t" = windows ]; then targ=(--nat); exe="$name.exe"; fi
    env "$@" "$FID" emit "$SRC" "${targ[@]}" --stats -o "$OUT/$exe" >"$OUT/$name.log" 2>&1
}
exe_of() { if [ "$2" = windows ]; then echo "$1.exe"; else echo "$1"; fi; }

# lf20 <log> <fn> <field>: field 1 secret values, 2 in registers
lf20() {
    sed -n "s/^\[lf20\] $2: \([0-9]*\) secret values, \([0-9]*\) in registers;.*/\1 \2/p" "$1" | head -1 | awk -v f="$3" '{print $f}'
}

TARGETS="x86_64-linux aarch64-linux riscv64-linux"
if [ "$HOST" = windows ]; then TARGETS="$TARGETS windows"; fi

for t in $TARGETS; do
    on="lfp-$t-on"; nosec="lfp-$t-nosec"
    if build "$on" "$t" FID_VERIFY=1; then ok "$t builds (FID_VERIFY=1)"; else bad "$t builds (FID_VERIFY=1): $(tail -3 "$OUT/$on.log")"; continue; fi
    for fn in press_param press_local; do
        s=$(lf20 "$OUT/$on.log" "$fn" 1); r=$(lf20 "$OUT/$on.log" "$fn" 2)
        if [ -n "$s" ] && [ "$s" -gt 0 ] && [ "$r" = "$s" ]; then
            ok "$t: $fn keeps all $s secret values in registers"
        else
            bad "$t: [lf20] $fn line: S=$s R=$r (want S > 0, R == S)"
        fi
    done
    sa=$(lf20 "$OUT/$on.log" assigned 1)
    if [ -n "$sa" ] && [ "$sa" -gt 0 ]; then ok "$t: assigned has $sa secret values"; else bad "$t: [lf20] assigned line: S=$sa (want S > 0)"; fi
    if build "$nosec" "$t" FID_VERIFY=1 FID_NAT_OFF=secretregs; then
        ok "$t builds with FID_NAT_OFF=secretregs (press_param: $(lf20 "$OUT/$nosec.log" press_param 2) of $(lf20 "$OUT/$nosec.log" press_param 1) in registers)"
    else
        bad "$t builds with FID_NAT_OFF=secretregs: $(tail -3 "$OUT/$nosec.log")"; continue
    fi
    got=$(run_exe "$OUT" "$(exe_of "$on" "$t")" "$t"); st=$?
    if [ "$st" = 99 ]; then
        echo "skip $t: cannot run here (build-only)"
    else
        if [ "$got" = "$WANT" ]; then ok "$t: expected output"; else bad "$t: output differs"; diff <(echo "$WANT") <(echo "$got") | head -8; fi
        got2=$(run_exe "$OUT" "$(exe_of "$nosec" "$t")" "$t")
        if [ "$got2" = "$WANT" ]; then ok "$t: expected output (secretregs off)"; else bad "$t: output differs (secretregs off)"; fi
    fi
done

if [ "$fails" -gt 0 ]; then echo "$fails failure(s)"; exit 1; fi
echo "machine_lf20_param_check: all passed"
