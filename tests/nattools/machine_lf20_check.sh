#!/usr/bin/env bash
# LF20 (docs/roadmap/llvm-roadmap.md) on the machine layer: a function
# holding a secret zeroes, at every return, the registers no call keeps
# (compiler/nat_ir.fd ng_wipe_regs, ng_wipe_setup), and its secret values
# are the last the allocator spills (ng_secret_ids, ng_secret_weights).
# Run after the compiler is built:
#   bash tests/nattools/machine_lf20_check.sh [fid]   (default release/fid.exe)
#
# For x86_64-linux, aarch64-linux, riscv64-linux (and --nat Windows on a
# Windows host), with tests/bootstrap/test_machine_secret_regs.fd:
#  1. Built with FID_VERIFY=1 and --stats: an "[lf20] pressure: S secret
#     values, R in registers; each return zeroes W registers" line with
#     S > 0, R == S and W > 0; "[lf20] ct_pick: ..." (a leaf) with W > 0;
#     the expected output where the image runs.
#  2. FID_NAT_OFF=secretregs: the pressure line has R < S (the test does
#     see the preference: without it the secret value spills); same output.
#  3. FID_NAT_OFF=wiperegs: every [lf20] line says "zeroes 0 registers";
#     same output; the image of 1 is larger.
#  4. When llvm-objdump / objdump can read the image: the build of 1 holds
#     more zeroings of the last float register (x86-64 `pxor xmm15, xmm15`,
#     AArch64 `fmov d31, xzr`, RISC-V `fmv.d.x ft11, zero`) than that of 3.
#  5. tests/bootstrap/test_natabi_wipe_secret.fd with FID_VERIFY=1: its
#     expected output (the frame wipe and the register wipe together).
# Exit 1 on a failure; a target the host cannot run is build-only.
set -u
FID="${1:-release/fid.exe}"
SRC=tests/bootstrap/test_machine_secret_regs.fd
WSRC=tests/bootstrap/test_natabi_wipe_secret.fd
OUT="${TMPDIR:-/tmp}/fid-machine-lf20"
mkdir -p "$OUT"
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }
WANT="$(expected "$SRC")"
WWANT="$(expected "$WSRC")"

HOST=linux
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; esac
WSL=""
if [ "$HOST" = windows ] && command -v wsl.exe >/dev/null 2>&1; then WSL="wsl.exe -e"; fi

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

OBJDUMP=""
for c in llvm-objdump objdump; do
    if command -v "$c" >/dev/null 2>&1; then OBJDUMP="$c"; break; fi
done

# build <name> <src> <target> [env...]: emit with --stats into $OUT/<name>
build() {
    local name="$1" src="$2" t="$3"
    shift 3
    local targ=(--target "$t") exe="$name"
    if [ "$t" = windows ]; then targ=(--nat); exe="$name.exe"; fi
    env "$@" "$FID" emit "$src" "${targ[@]}" --stats -o "$OUT/$exe" >"$OUT/$name.log" 2>&1
}
exe_of() { if [ "$2" = windows ]; then echo "$1.exe"; else echo "$1"; fi; }

# lf20 <log> <fn> <field>: field 1 secret values, 2 in registers, 3 zeroed
lf20() {
    sed -n "s/^\[lf20\] $2: \([0-9]*\) secret values, \([0-9]*\) in registers; each return zeroes \([0-9]*\) registers.*/\1 \2 \3/p" "$1" | head -1 | awk -v f="$3" '{print $f}'
}

TARGETS="x86_64-linux aarch64-linux riscv64-linux"
if [ "$HOST" = windows ]; then TARGETS="$TARGETS windows"; fi

for t in $TARGETS; do
    on="lf-$t-on"; nosec="lf-$t-nosec"; noregs="lf-$t-noregs"; wp="lf-$t-wipe"
    if build "$on" "$SRC" "$t" FID_VERIFY=1; then ok "$t builds (FID_VERIFY=1)"; else bad "$t builds (FID_VERIFY=1): $(tail -3 "$OUT/$on.log")"; continue; fi
    s=$(lf20 "$OUT/$on.log" pressure 1); r=$(lf20 "$OUT/$on.log" pressure 2); w=$(lf20 "$OUT/$on.log" pressure 3)
    if [ -n "$s" ] && [ "$s" -gt 0 ] && [ "$r" = "$s" ] && [ "$w" -gt 0 ]; then
        ok "$t: pressure keeps all $s secret values in registers, each return zeroes $w registers"
    else
        bad "$t: [lf20] pressure line: S=$s R=$r W=$w (want S > 0, R == S, W > 0)"
    fi
    wl=$(lf20 "$OUT/$on.log" ct_pick 3)
    if [ -n "$wl" ] && [ "$wl" -gt 0 ]; then ok "$t: the leaf ct_pick zeroes $wl registers"; else bad "$t: [lf20] ct_pick line: W=$wl"; fi
    if build "$nosec" "$SRC" "$t" FID_VERIFY=1 FID_NAT_OFF=secretregs; then ok "$t builds with FID_NAT_OFF=secretregs"; else bad "$t builds with FID_NAT_OFF=secretregs"; continue; fi
    s2=$(lf20 "$OUT/$nosec.log" pressure 1); r2=$(lf20 "$OUT/$nosec.log" pressure 2)
    if [ -n "$s2" ] && [ -n "$r2" ] && [ "$r2" -lt "$s2" ]; then ok "$t: without the preference $r2 of $s2 secret values keep a register"; else bad "$t: FID_NAT_OFF=secretregs: S=$s2 R=$r2 (want R < S: the test must put the secret under pressure)"; fi
    if build "$noregs" "$SRC" "$t" FID_VERIFY=1 FID_NAT_OFF=wiperegs; then ok "$t builds with FID_NAT_OFF=wiperegs"; else bad "$t builds with FID_NAT_OFF=wiperegs"; continue; fi
    nz=$(grep -c '^\[lf20\] .* zeroes [1-9][0-9]* registers' "$OUT/$noregs.log")
    if [ "$nz" -eq 0 ]; then ok "$t: no register wipe with the switch off"; else bad "$t: $nz [lf20] lines still zero registers with FID_NAT_OFF=wiperegs"; fi
    e_on="$OUT/$(exe_of "$on" "$t")"; e_off="$OUT/$(exe_of "$noregs" "$t")"
    if [ "$(wc -c <"$e_on")" -gt "$(wc -c <"$e_off")" ]; then ok "$t: the wipe adds code"; else bad "$t: image with the wipe is not larger ($(wc -c <"$e_on") vs $(wc -c <"$e_off"))"; fi
    got=$(run_exe "$OUT" "$(exe_of "$on" "$t")" "$t"); st=$?
    if [ "$st" = 99 ]; then
        echo "skip $t: cannot run here (build-only)"
    else
        if [ "$got" = "$WANT" ]; then ok "$t: expected output"; else bad "$t: output differs"; diff <(echo "$WANT") <(echo "$got") | head -8; fi
        got2=$(run_exe "$OUT" "$(exe_of "$nosec" "$t")" "$t")
        if [ "$got2" = "$WANT" ]; then ok "$t: expected output (secretregs off)"; else bad "$t: output differs (secretregs off)"; fi
        got3=$(run_exe "$OUT" "$(exe_of "$noregs" "$t")" "$t")
        if [ "$got3" = "$WANT" ]; then ok "$t: expected output (wiperegs off)"; else bad "$t: output differs (wiperegs off)"; fi
    fi
    if [ -n "$OBJDUMP" ] && [ "$t" != windows ]; then
        if "$OBJDUMP" -d "$e_on" >"$OUT/$on.dis" 2>/dev/null && [ -s "$OUT/$on.dis" ]; then
            "$OBJDUMP" -d "$e_off" >"$OUT/$noregs.dis" 2>/dev/null
            case "$t" in
                x86_64-linux) pat='pxor[[:space:]]+%?xmm15,[[:space:]]*%?xmm15' ;;
                aarch64-linux) pat='fmov[[:space:]]+d31,[[:space:]]*xzr' ;;
                *) pat='fmv\.d\.x[[:space:]]+(ft11|f31),[[:space:]]*(zero|x0)' ;;
            esac
            c_on=$(grep -cE "$pat" "$OUT/$on.dis")
            c_off=$(grep -cE "$pat" "$OUT/$noregs.dis")
            if [ "$c_on" -gt "$c_off" ]; then ok "$t: $c_on zeroings of the last float register ($c_off with the switch off)"; else bad "$t: zeroings of the last float register: $c_on on, $c_off off"; fi
        else
            echo "skip $t: $OBJDUMP cannot read the image"
        fi
    fi
    if build "$wp" "$WSRC" "$t" FID_VERIFY=1; then
        ok "$t builds $WSRC (FID_VERIFY=1)"
        gw=$(run_exe "$OUT" "$(exe_of "$wp" "$t")" "$t"); sw=$?
        if [ "$sw" != 99 ]; then
            if [ "$gw" = "$WWANT" ]; then ok "$t: $WSRC expected output"; else bad "$t: $WSRC output differs"; fi
        fi
    else
        bad "$t builds $WSRC (FID_VERIFY=1): $(tail -3 "$OUT/$wp.log")"
    fi
done

if [ "$fails" -gt 0 ]; then echo "$fails failure(s)"; exit 1; fi
echo "machine_lf20_check: all passed"
