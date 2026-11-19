#!/usr/bin/env bash
# RA19 (docs/roadmap/llvm-roadmap.md), the cases added on Nov 17: `!` across
# blocks and in the compare's block, in place or not, and EXT in a branch's
# chain (compiler/nat_ir.fd ng_fuse_branches, ng_fuse_xblock; switches
# FID_NAT_OFF=brnot, brext, brxblock), plus the spill measurement of
# ng_fuse_xblock's longer ranges. Run after the compiler is built:
#   bash tests/nattools/machine_ra19_more_check.sh [fid]   (default release/fid.exe)
#
# 1. tests/bootstrap/test_machine_brflags2.fd built with FID_VERIFY=1 and
#    FID_NAT_DUMP for x86_64-linux, aarch64-linux, riscv64-linux (and
#    --nat on Windows), with the rules on and with
#    FID_NAT_OFF=brnot,brext,brxblock. Passing: the output is the file's
#    expected block in both builds wherever the image runs; in the
#    `[mir] <fn> after branch fusion` listing, xb_not, xb_nval and xb_nn
#    have fewer SETCC with the rules on than off on every target; ext_if
#    and xb_ext have no more.
# 2. FID_NAT_MUTATE=brnot,brxblock (the branch tests the wrong way, on
#    purpose): the output differs wherever the image runs.
# 3. tests/bootstrap/test_machine_brflags.fd (the earlier RA19 cases) still
#    prints its expected output with the rules on.
# 4. Spills: test_machine_brflags.fd, test_machine_brflags2.fd and
#    bench/fides/{fib,loop,sieve}.fd built with --stats with and without
#    FID_NAT_OFF=brxblock: the sums of `[stats] <fn>: spilled N, reloads M`
#    are printed per target ("xblock spills: on S/R, off S/R"). Recorded,
#    not judged; a rise of more than 5% in reloads is printed as a NOTE.
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-machine-ra19-more"
mkdir -p "$OUT"
WIN=0
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; esac
WSL=0
if [ "$WIN" = 1 ] && command -v wsl.exe >/dev/null 2>&1; then WSL=1; fi
fails=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fails=$((fails + 1)); }
expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | sed -n 's|^// \(.*\)|\1|p' | sed '1d'; }
wslp() { MSYS_NO_PATHCONV=1 wsl.exe -e wslpath -a "$(cygpath -w "$1")" 2>/dev/null | tr -d '\r' | grep -v '^wsl: ' | tail -1; }

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
    case "$got" in *"Could not open '/lib/ld-linux"*) return 2 ;; esac
    printf '%s\n' "$got"
    return 0
}

# build <image> <test> <target> [env...] -- [args...]
build() {
    local img="$1" t="$2" tgt="$3"
    shift 3
    local envs=()
    while [ $# -gt 0 ] && [ "$1" != -- ]; do envs+=("$1"); shift; done
    [ $# -gt 0 ] && shift
    if [ "$tgt" = nat ]; then
        env "${envs[@]+"${envs[@]}"}" "$FID" emit "$t" --nat "$@" -o "$img" >"$img.log" 2>&1
    else
        env "${envs[@]+"${envs[@]}"}" "$FID" emit "$t" --target "$tgt" "$@" -o "$img" >"$img.log" 2>&1
    fi
}

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
[ "$WIN" = 1 ] && TARGETS="$TARGETS nat"
T=tests/bootstrap/test_machine_brflags2.fd
T1=tests/bootstrap/test_machine_brflags.fd
WANT="$(expected "$T")"
OFF="brnot,brext,brxblock"
FNS="xb_not,xb_nval,xb_nn,ext_if,xb_ext"

for tgt in $TARGETS; do
    ext=""; [ "$tgt" = nat ] && ext=".exe"
    on="$OUT/bf2-$tgt-on$ext"; off="$OUT/bf2-$tgt-off$ext"; mu="$OUT/bf2-$tgt-mut$ext"
    if ! build "$on" "$T" "$tgt" FID_VERIFY=1 "FID_NAT_DUMP=$FNS"; then bad "$tgt builds: $(tail -3 "$on.log")"; continue; fi
    if ! build "$off" "$T" "$tgt" FID_VERIFY=1 "FID_NAT_DUMP=$FNS" "FID_NAT_OFF=$OFF"; then bad "$tgt builds with $OFF off"; continue; fi
    ok "$tgt builds (FID_VERIFY=1, rules on and off)"
    for fn in xb_not xb_nval xb_nn; do
        a=$(count_op "$on.log" "$fn" "branch fusion" setcc)
        b=$(count_op "$off.log" "$fn" "branch fusion" setcc)
        if [ "$a" -lt "$b" ]; then ok "$tgt: $fn has $a SETCC after branch fusion ($b with $OFF off)"; else bad "$tgt: $fn has $a SETCC after branch fusion ($b with $OFF off)"; fi
    done
    for fn in ext_if xb_ext; do
        a=$(count_op "$on.log" "$fn" "branch fusion" setcc)
        b=$(count_op "$off.log" "$fn" "branch fusion" setcc)
        if [ "$a" -le "$b" ]; then ok "$tgt: $fn has $a SETCC ($b off)"; else bad "$tgt: $fn has more SETCC with the rules on ($a > $b)"; fi
    done
    got=$(run_img "$on" "$tgt"); rc=$?
    if [ $rc -eq 2 ]; then
        echo "skip $tgt: cannot run here (build-only)"
    else
        if [ "$got" = "$WANT" ]; then ok "$tgt: expected output"; else bad "$tgt: output differs"; diff <(echo "$WANT") <(echo "$got") | head -6; fi
        got=$(run_img "$off" "$tgt")
        if [ "$got" = "$WANT" ]; then ok "$tgt: expected output (rules off)"; else bad "$tgt: output differs (rules off)"; fi
        if build "$mu" "$T" "$tgt" FID_NAT_MUTATE=brnot,brxblock; then
            got=$(run_img "$mu" "$tgt")
            if [ "$got" != "$WANT" ]; then ok "$tgt: the mutation changes the output"; else bad "$tgt: FID_NAT_MUTATE=brnot,brxblock prints the expected output"; fi
        else
            bad "$tgt builds with the mutation"
        fi
        old="$OUT/bf1-$tgt$ext"
        if build "$old" "$T1" "$tgt" FID_VERIFY=1; then
            got=$(run_img "$old" "$tgt")
            if [ "$got" = "$(expected "$T1")" ]; then ok "$tgt: test_machine_brflags.fd still passes"; else bad "$tgt: test_machine_brflags.fd output differs"; fi
        else
            bad "$tgt: test_machine_brflags.fd builds"
        fi
    fi
done

# 4. spills with and without ng_fuse_xblock
for tgt in $TARGETS; do
    ext=""; [ "$tgt" = nat ] && ext=".exe"
    sp_on=0; rl_on=0; sp_off=0; rl_off=0
    for src in "$T1" "$T" bench/fides/fib.fd bench/fides/loop.fd bench/fides/sieve.fd; do
        [ -f "$src" ] || continue
        b=$(basename "$src" .fd)
        i1="$OUT/sp-$b-$tgt-on$ext"; i2="$OUT/sp-$b-$tgt-off$ext"
        build "$i1" "$src" "$tgt" -- --stats || { bad "$tgt $b builds (--stats)"; continue; }
        build "$i2" "$src" "$tgt" FID_NAT_OFF=brxblock -- --stats || { bad "$tgt $b builds (brxblock off)"; continue; }
        s=$(sed -n 's/^\[stats\] [^:]*: spilled \([0-9]*\), reloads \([0-9]*\),.*/\1 \2/p' "$i1.log" | awk '{a += $1; b += $2} END {print a + 0, b + 0}')
        sp_on=$((sp_on + ${s% *})); rl_on=$((rl_on + ${s#* }))
        s=$(sed -n 's/^\[stats\] [^:]*: spilled \([0-9]*\), reloads \([0-9]*\),.*/\1 \2/p' "$i2.log" | awk '{a += $1; b += $2} END {print a + 0, b + 0}')
        sp_off=$((sp_off + ${s% *})); rl_off=$((rl_off + ${s#* }))
    done
    echo "xblock spills $tgt: on $sp_on/$rl_on, off $sp_off/$rl_off (spilled/reloads)"
    if [ "$rl_off" -gt 0 ] && [ $((rl_on * 100)) -gt $((rl_off * 105)) ]; then echo "NOTE $tgt: ng_fuse_xblock adds more than 5% reloads"; fi
done

if [ "$fails" -eq 0 ]; then echo "machine_ra19_more_check: 0 failure(s)"; else echo "machine_ra19_more_check: $fails failure(s)"; exit 1; fi
