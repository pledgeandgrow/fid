#!/usr/bin/env bash
# Checks of the machine-layer goals RA12 (shrink wrapping placed by
# dominators) and RA19 (branch conditions kept in the flags), from
# docs/roadmap/llvm-roadmap.md, run after the compiler is built:
#   bash tests/nattools/machine_ra12_ra19_check.sh [fid]   (default release/fid.exe)
# Every build has FID_VERIFY=1 (MIR verifier, allocation checker). Where an
# image runs (x86-64 Linux natively or through WSL; AArch64 / RISC-V
# natively or through qemu-aarch64 / qemu-riscv64, here or inside WSL; the
# --nat Windows image on Windows) its output must be the test's expected
# lines, each switch on and off.
# - RA12: tests/bootstrap/test_machine_shrinkwrap2.fd and
#   tests/bootstrap/test_machine_shrinkwrap.fd built with --stats:
#   `[shrinkwrap] <fn>:` lines. `rare` (its first block ends at a loop
#   head), `tailpath` (its early path leaves through a tail call) and
#   `midrare` (a rare region in the middle, restored where it ends) could
#   not defer saves with the early-exit plan of before: on AArch64 and
#   RISC-V each must have a line (midrare's with `restored early at` 1 or
#   more), on x86-64 (a pool of five callee-saved registers, so hot values
#   sit in them too) at least one; `fib` of the older test keeps its line.
#   None with FID_NAT_OFF=shrinkwrap, no early restore with
#   FID_NAT_OFF=swrestore (and the same output).
# - RA19: tests/bootstrap/test_machine_brflags.fd with FID_NAT_DUMP='*':
#   after branch fusion, `not_if` and `xb` have fewer SETCCs than with
#   FID_NAT_OFF=brnot,brandor,brxblock; on the targets with a select
#   (x86-64, --nat, AArch64) `and_if` and `or_if` have no CSEL and fewer
#   SETCCs than with the switches off, and two BRCCs or more.
#   FID_NAT_MUTATE=brnot,brandor,brxblock (each rule tests the wrong way)
#   must change the output where the image runs.
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-machine-ra12-ra19"
mkdir -p "$OUT"
WIN=0
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; esac
WSL=0
if [ "$WIN" = 1 ] && command -v wsl.exe >/dev/null 2>&1; then WSL=1; fi
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

# the image's path inside WSL
wslp() { MSYS_NO_PATHCONV=1 wsl.exe -e wslpath -a "$(cygpath -w "$1")" 2>/dev/null | tr -d '\r' | grep -v '^wsl: ' | tail -1; }

# run_img <image> <target>: prints the output; exit 2 when it cannot run here
run_img() {
    local img="$1" tgt="$2" q="" arch="" got w
    case "$tgt" in
        nat)
            [ "$WIN" = 1 ] || return 2
            "$img" 2>&1 | tr -d '\r'
            return 0 ;;
        x86_64-linux) arch=x86_64 ;;
        aarch64-linux) arch=aarch64; q=qemu-aarch64 ;;
        riscv64-linux) arch=riscv64; q=qemu-riscv64 ;;
    esac
    if [ "$(uname -s)" = Linux ] && [ "$(uname -m)" = "$arch" ]; then
        got=$("$img" 2>&1)
    elif [ -n "$q" ] && [ "$WIN" = 0 ] && command -v "$q" >/dev/null 2>&1; then
        got=$("$q" "$img" 2>&1)
    elif [ "$WSL" = 1 ]; then
        w=$(wslp "$img")
        [ -n "$w" ] || return 2
        if [ -n "$q" ]; then
            MSYS_NO_PATHCONV=1 wsl.exe -e sh -c "command -v $q" 2>/dev/null | grep -q "$q" || return 2
            got=$(MSYS_NO_PATHCONV=1 wsl.exe -e "$q" "$w" 2>&1 | tr -d '\r' | grep -v '^wsl: ')
        else
            got=$(MSYS_NO_PATHCONV=1 wsl.exe -e "$w" 2>&1 | tr -d '\r' | grep -v '^wsl: ')
        fi
    else
        return 2
    fi
    # (a dynamically linked image under qemu-user without a sysroot)
    case "$got" in *"Could not open '/lib/ld-linux"*) return 2 ;; esac
    printf '%s\n' "$got"
    return 0
}

# build <image> <test> <target> [args...]
build() {
    local img="$1" t="$2" tgt="$3"
    shift 3
    if [ "$tgt" = nat ]; then
        FID_VERIFY=1 "$FID" emit "$t" --nat "$@" -o "$img" >"$img.log" 2>&1
    else
        FID_VERIFY=1 "$FID" emit "$t" --target "$tgt" "$@" -o "$img" >"$img.log" 2>&1
    fi
}

# run_check <name> <image> <test> <target>
run_check() {
    local got rc
    got=$(run_img "$2" "$4")
    rc=$?
    if [ $rc -eq 2 ]; then
        ok "$1 built (cannot run here)"
    elif [ "$got" = "$(expected "$3")" ]; then
        ok "$1 runs"
    else
        bad "$1 runs (output differs)"
        printf '%s\n' "$got" | head -20 | sed 's/^/     /'
    fi
}

# count_op <log> <function> <stage> <op>: operations `op` in the dump of
# the function after that stage (`[mir] <fn> after <stage>: N operations`)
count_op() {
    awk -v fn="$2" -v st="$3" -v op="$4" '
        /^\[mir\] [^ ]+ after .*: [0-9]+ operations$/ {
            line = $0
            sub(/^\[mir\] /, "", line)
            name = line; sub(/ after .*/, "", name)
            rest = line; sub(/^[^ ]+ after /, "", rest); sub(/: [0-9]+ operations$/, "", rest)
            cur = (name == fn && rest == st) ? 1 : 0
            next
        }
        /^\[mir\] [^ ]+: [0-9]+ operations, frame/ { cur = 0; next }
        cur && $2 ~ /^[0-9]+:$/ && $3 == op { n++ }
        END { print n + 0 }' "$1"
}

TARGETS="x86_64-linux aarch64-linux riscv64-linux"
NATS="$TARGETS"
if [ "$WIN" = 1 ]; then NATS="$TARGETS nat"; fi

# --- RA12 -------------------------------------------------------------------
T=tests/bootstrap/test_machine_shrinkwrap2.fd
T0=tests/bootstrap/test_machine_shrinkwrap.fd
for tgt in $NATS; do
    img="$OUT/sw2-$tgt"
    img0="$OUT/sw-$tgt"
    if [ "$tgt" = nat ]; then img="$img.exe"; img0="$img0.exe"; fi
    build "$img" "$T" "$tgt" --stats || { bad "RA12 $tgt builds ($(tail -1 "$img.log"))"; continue; }
    FID_NAT_OFF=shrinkwrap build "$img.off" "$T" "$tgt" --stats || { bad "RA12 $tgt builds (off)"; continue; }
    build "$img0" "$T0" "$tgt" --stats || { bad "RA12 $tgt builds the early-exit test"; continue; }
    run_check "RA12 $tgt" "$img" "$T" "$tgt"
    run_check "RA12 $tgt (off)" "$img.off" "$T" "$tgt"
    run_check "RA12 $tgt early-exit test" "$img0" "$T0" "$tgt"
    seen=0
    for fn in rare tailpath midrare; do
        line=$(grep "^\[shrinkwrap\] $fn: " "$img.log" | head -1)
        if [ "$fn" = midrare ] && [ -n "$line" ] && ! echo "$line" | grep -q 'restored early at [1-9]'; then
            bad "RA12 $tgt: midrare defers saves but restores nothing early: $line"
        elif [ -n "$line" ]; then
            ok "RA12 $tgt: $line"
            seen=$((seen + 1))
        elif [ "$tgt" = aarch64-linux ] || [ "$tgt" = riscv64-linux ]; then
            bad "RA12 $tgt: $fn saves every callee-saved register in its prologue"
        else
            echo "note RA12 $tgt: no [shrinkwrap] line for $fn"
        fi
    done
    if [ "$seen" -eq 0 ]; then bad "RA12 $tgt: none of rare, tailpath, midrare defers a save"; fi
    FID_NAT_OFF=swrestore build "$img.nore" "$T" "$tgt" --stats || { bad "RA12 $tgt builds (swrestore off)"; continue; }
    run_check "RA12 $tgt (swrestore off)" "$img.nore" "$T" "$tgt"
    if grep -q '^\[shrinkwrap\] .*restored early at [1-9]' "$img.nore.log"; then bad "RA12 $tgt: FID_NAT_OFF=swrestore still restores early"
    else ok "RA12 $tgt (swrestore off): no early restores"; fi
    for fn in inloop twoguard; do
        line=$(grep "^\[shrinkwrap\] $fn: " "$img.log" | head -1)
        [ -n "$line" ] && echo "info $tgt: $line"
    done
    if grep -q '^\[shrinkwrap\] fib: ' "$img0.log"; then ok "RA12 $tgt: $(grep '^\[shrinkwrap\] fib: ' "$img0.log" | head -1)"
    else bad "RA12 $tgt: fib's early exit saves everything"; fi
    if grep -q '^\[shrinkwrap\]' "$img.off.log"; then bad "RA12 $tgt: FID_NAT_OFF=shrinkwrap still defers saves"
    else ok "RA12 $tgt (off): no deferred saves"; fi
done

# --- RA19 -------------------------------------------------------------------
T=tests/bootstrap/test_machine_brflags.fd
OFF=brnot,brandor,brxblock
for tgt in $NATS; do
    img="$OUT/bf-$tgt"
    [ "$tgt" = nat ] && img="$img.exe"
    FID_NAT_DUMP='*' build "$img" "$T" "$tgt" || { bad "RA19 $tgt builds ($(tail -1 "$img.log"))"; continue; }
    FID_NAT_OFF=$OFF FID_NAT_DUMP='*' build "$img.off" "$T" "$tgt" || { bad "RA19 $tgt builds (off)"; continue; }
    FID_NAT_MUTATE=$OFF build "$img.mut" "$T" "$tgt" || { bad "RA19 $tgt builds (mutated)"; continue; }
    run_check "RA19 $tgt" "$img" "$T" "$tgt"
    run_check "RA19 $tgt (off)" "$img.off" "$T" "$tgt"
    got=$(run_img "$img.mut" "$tgt")
    if [ $? -ne 2 ]; then
        if [ "$got" != "$(expected "$T")" ]; then ok "RA19 $tgt: the mutated rules change the output"
        else bad "RA19 $tgt: FID_NAT_MUTATE=$OFF did not change the output"; fi
    fi
    for fn in not_if xb; do
        on=$(count_op "$img.log" "$fn" "branch fusion" setcc)
        off=$(count_op "$img.off.log" "$fn" "branch fusion" setcc)
        if [ "$on" -lt "$off" ]; then ok "RA19 $tgt: $fn has $on SETCC after branch fusion ($off with $OFF off)"
        else bad "RA19 $tgt: $fn has $on SETCC after branch fusion ($off with $OFF off)"; fi
    done
    if [ "$tgt" != riscv64-linux ]; then
        for fn in and_if or_if; do
            cs=$(count_op "$img.log" "$fn" "branch fusion" csel)
            csoff=$(count_op "$img.off.log" "$fn" "branch fusion" csel)
            on=$(count_op "$img.log" "$fn" "branch fusion" setcc)
            off=$(count_op "$img.off.log" "$fn" "branch fusion" setcc)
            nb=$(count_op "$img.log" "$fn" "branch fusion" brcc)
            if [ "$cs" -eq 0 ] && [ "$csoff" -gt 0 ] && [ "$on" -lt "$off" ] && [ "$nb" -ge 2 ]; then
                ok "RA19 $tgt: $fn: $nb BRCC, no CSEL, $on SETCC ($csoff CSEL, $off SETCC with $OFF off)"
            else
                bad "RA19 $tgt: $fn: $nb BRCC, $cs CSEL, $on SETCC ($csoff CSEL, $off SETCC with $OFF off)"
            fi
        done
    fi
done

echo
if [ "$fails" -eq 0 ]; then echo "all RA12 / RA19 checks passed"; exit 0; fi
echo "$fails check(s) failed"
exit 1
