#!/usr/bin/env bash
# Checks of the machine-layer goals RA6, RA11 and RA12
# (docs/roadmap/llvm-roadmap.md), run after the compiler is built:
#   bash tests/nattools/machine_batch5_check.sh [fid]   (default release/fid.exe)
# - RA6: tests/bootstrap/test_machine_counter_step.fd: `count_up`'s MIR
#   after allocation (FID_NAT_DUMP) has an `addi` whose destination and
#   source are one register (the loop step `i = i + 1` as one
#   instruction) on x86-64, AArch64, RISC-V and --nat Windows; none with
#   FID_NAT_OFF=addi; `fid emit --stats` counts fewer moves for
#   `count_up` than with FID_NAT_OFF=livecoal,addi.
# - RA12: tests/bootstrap/test_machine_shrinkwrap.fd built with --stats:
#   a `[shrinkwrap] fib:` line (the early exit saves fewer callee-saved
#   registers than the function uses) on every target, none with
#   FID_NAT_OFF=shrinkwrap.
# - RA11: tests/nattools/par_moves_unit.fd (nat_par_moves) through
#   `fid run` prints its expected lines.
# - every build with FID_VERIFY=1 (MIR verifier, allocation checker);
#   where the image runs (x86-64 Linux through WSL on Windows, natively on
#   Linux; the --nat Windows image natively on Windows), its output equals
#   the test's expected lines, each switch on and off.
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-machine-batch5"
mkdir -p "$OUT"
RUN=""
if command -v wsl.exe >/dev/null 2>&1; then RUN="wsl.exe -e"; fi
WIN=0
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; esac
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }

expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }

runs_here() {
    [ "$1" = x86_64-linux ] || return 1
    if [ -n "$RUN" ]; then return 0; fi
    [ "$(uname -m)" = x86_64 ] && [ "$(uname -s)" = Linux ]
}

build() {
    local img="$1" t="$2" tgt="$3"
    shift 3
    if [ "$tgt" = nat ]; then
        FID_VERIFY=1 "$FID" emit "$t" --nat "$@" -o "$img" >"$img.log" 2>&1
    else
        FID_VERIFY=1 "$FID" emit "$t" --target "$tgt" "$@" -o "$img" >"$img.log" 2>&1
    fi
}

run_check() {
    local got
    if [ "$4" = nat ]; then
        got=$("$2" 2>&1)
    elif runs_here "$4"; then
        got=$($RUN "$2" 2>&1)
    else
        return 0
    fi
    if [ "$got" = "$(expected "$3")" ]; then ok "$1 runs"; else bad "$1 runs (output differs)"; fi
}

# same_reg_addi <dump log> <function>: addi lines of the function's dump
# after allocation whose two register operands are one register
same_reg_addi() {
    awk -v fn="$2" '
        /^\[mir\] [^ ]+ after .*: [0-9]+ operations$/ { cur = ""; next }
        /^\[mir\] [^ ]+: [0-9]+ operations, frame/ { cur = $2; sub(":", "", cur); next }
        /^\[mir\] [0-9]+: addi / {
            if (cur != fn) next
            a = $4; b = $5
            sub(/^v[0-9]+=r/, "", a); sub(/\[.*/, "", a)
            sub(/^v[0-9]+=r/, "", b); sub(/\[.*/, "", b)
            if (a == b && a !~ /^-/) n++
        }
        END { print n + 0 }' "$1"
}
moves() { sed -n "s/^\[stats\] $2: spilled [0-9]*, reloads [0-9]*, moves \([0-9]*\),.*/\1/p" "$1" | head -1; }

TARGETS="x86_64-linux aarch64-linux riscv64-linux"
NATS="$TARGETS"
if [ "$WIN" = 1 ]; then NATS="$TARGETS nat"; fi

# --- RA6 --------------------------------------------------------------------
T=tests/bootstrap/test_machine_counter_step.fd
for tgt in $NATS; do
    img="$OUT/step-$tgt"
    FID_NAT_DUMP=count_up build "$img-on" "$T" "$tgt" --stats || { bad "RA6 $tgt builds"; continue; }
    FID_NAT_OFF=addi FID_NAT_DUMP=count_up build "$img-noaddi" "$T" "$tgt" --stats || { bad "RA6 $tgt builds (addi off)"; continue; }
    FID_NAT_OFF=livecoal,addi build "$img-off" "$T" "$tgt" --stats || { bad "RA6 $tgt builds (livecoal,addi off)"; continue; }
    run_check "RA6 $tgt" "$img-on" "$T" "$tgt"
    run_check "RA6 $tgt (addi off)" "$img-noaddi" "$T" "$tgt"
    run_check "RA6 $tgt (livecoal,addi off)" "$img-off" "$T" "$tgt"
    n=$(same_reg_addi "$img-on.log" count_up)
    if [ "$n" -ge 1 ]; then ok "RA6 $tgt: count_up steps its counter with one addi in place"
    else bad "RA6 $tgt: no addi in place in count_up"; fi
    n0=$(same_reg_addi "$img-noaddi.log" count_up)
    if [ "$n0" -eq 0 ]; then ok "RA6 $tgt (addi off): no addi"; else bad "RA6 $tgt (addi off): $n0 addi"; fi
    a=$(moves "$img-on.log" count_up)
    b=$(moves "$img-off.log" count_up)
    if [ -z "$a" ] || [ -z "$b" ]; then bad "RA6 $tgt: no [stats] line for count_up"; continue; fi
    if [ "$a" -lt "$b" ]; then ok "RA6 $tgt: count_up moves $a (FID_NAT_OFF=livecoal,addi: $b)"
    else bad "RA6 $tgt: count_up moves $a (FID_NAT_OFF=livecoal,addi: $b)"; fi
done

# --- RA12 -------------------------------------------------------------------
T=tests/bootstrap/test_machine_shrinkwrap.fd
for tgt in $NATS; do
    img="$OUT/sw-$tgt"
    build "$img-on" "$T" "$tgt" --stats || { bad "RA12 $tgt builds"; continue; }
    FID_NAT_OFF=shrinkwrap build "$img-off" "$T" "$tgt" --stats || { bad "RA12 $tgt builds (off)"; continue; }
    run_check "RA12 $tgt" "$img-on" "$T" "$tgt"
    run_check "RA12 $tgt (off)" "$img-off" "$T" "$tgt"
    if grep -q '^\[shrinkwrap\] fib: ' "$img-on.log"; then ok "RA12 $tgt: $(grep '^\[shrinkwrap\] fib: ' "$img-on.log" | head -1)"
    else bad "RA12 $tgt: fib's early exit saves everything"; fi
    if grep -q '^\[shrinkwrap\]' "$img-off.log"; then bad "RA12 $tgt: FID_NAT_OFF=shrinkwrap still defers saves"
    else ok "RA12 $tgt (off): no deferred saves"; fi
done

# --- RA11 -------------------------------------------------------------------
T=tests/nattools/par_moves_unit.fd
got=$("$FID" run "$T" 2>/dev/null | grep -v '^\[')
if [ "$got" = "$(expected "$T")" ]; then ok "RA11 nat_par_moves unit"; else bad "RA11 nat_par_moves unit (output differs)"; fi

if [ "$fails" -gt 0 ]; then
    echo "$fails check(s) failed"
    exit 1
fi
echo "all machine batch-5 checks passed"
