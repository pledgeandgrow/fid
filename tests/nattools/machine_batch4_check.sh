#!/usr/bin/env bash
# Checks of the machine-layer goals MI24, MI26, MI28 and RA2
# (docs/roadmap/llvm-roadmap.md), run after the compiler is built:
#   bash tests/nattools/machine_batch4_check.sh [fid]   (default release/fid.exe)
# - MI24: tests/bootstrap/test_machine_select.fd: `max2`, `clamp` and
#   `pick_bits` have a `csel` in their MIR (FID_NAT_DUMP) on x86-64,
#   AArch64 and RISC-V with --cpu rva23u64 (Zicond); none on plain RISC-V,
#   none with FID_NAT_OFF=csel; `safe_div` (the division is what the
#   branch guards) never.
# - MI26: tests/bootstrap/test_machine_extstate.fd: `low_byte` and
#   `top_byte` have fewer `ext` in their MIR than with
#   FID_NAT_OFF=extstate, on every target.
# - MI28: tests/bootstrap/test_machine_tailcall_stack.fd and
#   tests/nattools/tailcall_deep.fd (three million levels, arguments on the
#   stack) run where the image runs here, with and without
#   FID_IROPT_OFF=tailcall; with FID_NAT_OFF=tailcall the deep one must
#   overflow its stack (it is the jumps that make it fit).
# - RA2: tests/bootstrap/test_machine_holes.fd built with --stats: `holes`
#   reloads (the "[stats] holes: ..., reloads N" line) no more than with
#   FID_NAT_OFF=holes on any target, fewer on x86-64 (five pool
#   registers).
# - every build with FID_VERIFY=1 (MIR verifier, allocation checker);
#   where the image runs (x86-64 Linux through WSL on Windows, natively on
#   Linux; the --nat Windows image natively on Windows), the output equals
#   the test's expected lines, with each switch on and off.
# - tests/nattools/enc-oracle-mi24.py --only when zig is on PATH (cmov,
#   csel, czero against clang).
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-machine-batch4"
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

# build <image> <test> <target> [extra emit args...]: FID_VERIFY=1 build
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

# ops <dump log> <function> <op>: how many times the function's dump after
# allocation has op (the per-pass dumps, "[mir] f after <stage>: ...", are
# skipped)
ops() {
    awk -v fn="$2" -v op="$3" '
        /^\[mir\] [^ ]+ after .*: [0-9]+ operations$/ { cur = ""; next }
        /^\[mir\] [^ ]+: [0-9]+ operations, frame/ { cur = $2; sub(":", "", cur); next }
        /^\[mir\] [0-9]+: / { if (cur == fn && $3 == op) n++ }
        END { print n + 0 }' "$1"
}

TARGETS="x86_64-linux aarch64-linux riscv64-linux"
NATS="$TARGETS"
if [ "$WIN" = 1 ]; then NATS="$TARGETS nat"; fi

# --- MI24 -------------------------------------------------------------------
T=tests/bootstrap/test_machine_select.fd
for tgt in $NATS rva23; do
    real="$tgt"
    extra=""
    if [ "$tgt" = rva23 ]; then real=riscv64-linux; extra="--cpu rva23u64"; fi
    img="$OUT/select-$tgt"
    for sw in on off; do
        log="$img-$sw.dump"
        if [ "$sw" = on ]; then
            FID_NAT_DUMP='*' build "$img-$sw" "$T" "$real" $extra
        else
            FID_NAT_OFF=csel FID_NAT_DUMP='*' build "$img-$sw" "$T" "$real" $extra
        fi
        rc=$?
        cp "$img-$sw.log" "$log"
        if [ $rc -ne 0 ]; then bad "MI24 $tgt ($sw) builds"; continue; fi
        run_check "MI24 $tgt ($sw)" "$img-$sw" "$T" "$real"
        n=0
        for fn in max2 clamp pick_bits; do n=$((n + $(ops "$log" "$fn" csel))); done
        nd=$(ops "$log" safe_div csel)
        want_sel=1
        if [ "$sw" = off ] || [ "$tgt" = riscv64-linux ]; then want_sel=0; fi
        if [ "$want_sel" = 1 ] && [ "$n" -ge 3 ]; then ok "MI24 $tgt: $n selects in max2/clamp/pick_bits"
        elif [ "$want_sel" = 0 ] && [ "$n" -eq 0 ]; then ok "MI24 $tgt ($sw): no select"
        else bad "MI24 $tgt ($sw): $n selects in max2/clamp/pick_bits"; fi
        if [ "$nd" -eq 0 ]; then ok "MI24 $tgt ($sw): safe_div keeps its branch"; else bad "MI24 $tgt ($sw): safe_div has a select"; fi
    done
done

# --- MI26 -------------------------------------------------------------------
T=tests/bootstrap/test_machine_extstate.fd
for tgt in $NATS; do
    img="$OUT/ext-$tgt"
    FID_NAT_DUMP='*' build "$img-on" "$T" "$tgt" || { bad "MI26 $tgt builds"; continue; }
    FID_NAT_OFF=extstate FID_NAT_DUMP='*' build "$img-off" "$T" "$tgt" || { bad "MI26 $tgt builds (off)"; continue; }
    run_check "MI26 $tgt" "$img-on" "$T" "$tgt"
    run_check "MI26 $tgt (off)" "$img-off" "$T" "$tgt"
    for fn in low_byte top_byte; do
        a=$(ops "$img-on.log" "$fn" ext)
        b=$(ops "$img-off.log" "$fn" ext)
        if [ "$a" -lt "$b" ]; then ok "MI26 $tgt: $fn has $a ext (FID_NAT_OFF=extstate: $b)"
        else bad "MI26 $tgt: $fn has $a ext (FID_NAT_OFF=extstate: $b)"; fi
    done
done

# --- MI28 -------------------------------------------------------------------
for T in tests/bootstrap/test_machine_tailcall_stack.fd tests/nattools/tailcall_deep.fd; do
    base=$(basename "$T" .fd)
    for tgt in $NATS; do
        for io in on off; do
            img="$OUT/$base-$tgt-$io"
            if [ "$io" = on ]; then
                build "$img" "$T" "$tgt" || { bad "MI28 $base $tgt builds"; continue; }
            else
                FID_IROPT_OFF=tailcall build "$img" "$T" "$tgt" || { bad "MI28 $base $tgt builds (iropt tailcall off)"; continue; }
            fi
            run_check "MI28 $base $tgt (iropt tailcall $io)" "$img" "$T" "$tgt"
        done
    done
done
# the deep test needs the jumps: without them it overflows
for tgt in $NATS; do
    img="$OUT/deep-nojump-$tgt"
    FID_NAT_OFF=tailcall FID_IROPT_OFF=tailcall build "$img" tests/nattools/tailcall_deep.fd "$tgt" || continue
    got=""
    if [ "$tgt" = nat ]; then got=$("$img" 2>&1)
    elif runs_here "$tgt"; then got=$($RUN "$img" 2>&1)
    else continue; fi
    if [ "$got" != "$(expected tests/nattools/tailcall_deep.fd)" ]; then ok "MI28 $tgt: without jumps the deep test does not fit the stack"
    else bad "MI28 $tgt: the deep test fits without jumps (it proves nothing)"; fi
done

# --- RA2 --------------------------------------------------------------------
T=tests/bootstrap/test_machine_holes.fd
reloads() { sed -n "s/^\[stats\] $2: spilled [0-9]*, reloads \([0-9]*\),.*/\1/p" "$1" | head -1; }
for tgt in $NATS; do
    img="$OUT/holes-$tgt"
    build "$img-on" "$T" "$tgt" --stats || { bad "RA2 $tgt builds"; continue; }
    FID_NAT_OFF=holes build "$img-off" "$T" "$tgt" --stats || { bad "RA2 $tgt builds (off)"; continue; }
    run_check "RA2 $tgt" "$img-on" "$T" "$tgt"
    run_check "RA2 $tgt (off)" "$img-off" "$T" "$tgt"
    a=$(reloads "$img-on.log" holes)
    b=$(reloads "$img-off.log" holes)
    if [ -z "$a" ] || [ -z "$b" ]; then bad "RA2 $tgt: no [stats] line for holes"; continue; fi
    if [ "$a" -le "$b" ] && { [ "$tgt" != x86_64-linux ] || [ "$a" -lt "$b" ]; }; then
        ok "RA2 $tgt: holes reloads $a (FID_NAT_OFF=holes: $b)"
    else
        bad "RA2 $tgt: holes reloads $a (FID_NAT_OFF=holes: $b)"
    fi
done

# --- encoders ---------------------------------------------------------------
if command -v zig >/dev/null 2>&1; then
    if python3 tests/nattools/enc-oracle-mi24.py --only --fid "$FID" >"$OUT/oracle.log" 2>&1; then
        ok "MI24 encoders agree with clang"
    else
        bad "MI24 encoders (see $OUT/oracle.log)"
    fi
fi

if [ "$fails" -gt 0 ]; then
    echo "$fails check(s) failed"
    exit 1
fi
echo "all machine batch-4 checks passed"
