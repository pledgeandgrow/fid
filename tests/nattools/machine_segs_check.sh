#!/usr/bin/env bash
# Checks of RA2 (docs/roadmap/llvm-roadmap.md): liveness from use lists and
# the main allocation over live ranges with holes (compiler/nat_ir.fd
# ng_use_liveness, ng_class_segs, ng_seg_scan, ng_greedy_assign), run after
# the compiler is built:
#   bash tests/nattools/machine_segs_check.sh [fid]   (default release/fid.exe)
# FILES="a.fd b.fd" runs the build checks over other programs too (e.g.
# FILES="$(ls tests/bootstrap/test_*.fd)" for the suite; only the builds
# with FID_VERIFY=1 and the runs are checked for those).
# - tests/nattools/seg_unit.fd (`fid run`): random functions, the use-list
#   liveness against the block fixed point, segments against exact
#   liveness, the scan and the greedy allocator over segments against the
#   checker's rules; a designed function with no spill over segments.
# - tests/bootstrap/test_machine_segs.fd and test_machine_holes.fd on
#   x86-64, AArch64, RISC-V (and --nat on Windows), each built with
#   FID_VERIFY=1 (MIR verifier, allocation checker, use-list liveness
#   compared with the block fixed point) and --stats in six ways: default;
#   FID_NAT_OFF=segscan (hull scan, segments as a second chance only);
#   FID_NAT_OFF=holes (hulls only); FID_NAT_OFF=uselive (fixed point);
#   FID_NAT_ON=greedy; FID_NAT_ON=greedy FID_NAT_OFF=holes. Every build
#   succeeds; where the image runs (x86-64 Linux through WSL on Windows or
#   natively on Linux, --nat natively on Windows) its output equals the
#   test's expected lines.
# - `[segs]` lines: `far` has ranges with holes.
# - reloads (the `[stats] <fn>: spilled N, reloads N` lines) of `far`,
#   `reuse`, `alt` and `holes`: default no more than FID_NAT_OFF=holes;
#   on x86-64 (five pool registers) `far` and `holes` strictly fewer.
#   Greedy over segments no more than greedy over hulls.
# - FID_NAT_OFF=uselive gives the same `[stats]` lines as the default (the
#   two liveness computations agree, so the allocation is the same).
# Exit 1 on a failure.
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-machine-segs"
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
    [ "$1" = x86_64-linux ] || [ "$1" = nat ] || return 1
    if [ "$1" = nat ]; then [ "$WIN" = 1 ]; return; fi
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
    runs_here "$4" || return 0
    if [ "$4" = nat ]; then
        got=$("$2" 2>&1)
    else
        got=$($RUN "$2" 2>&1)
    fi
    if [ "$got" = "$(expected "$3")" ]; then ok "$1 runs"; else bad "$1 runs (output differs)"; fi
}

reloads() { sed -n "s/^\[stats\] $2: spilled [0-9]*, reloads \([0-9]*\),.*/\1/p" "$1" | head -1; }
holed() { sed -n "s/^\[segs\] $2: [0-9]* ranges, \([0-9]*\) with holes.*/\1/p" "$1" | head -1; }

# --- unit ---------------------------------------------------------------------
got=$("$FID" run tests/nattools/seg_unit.fd 2>"$OUT/seg_unit.err")
if [ "$got" = "$(expected tests/nattools/seg_unit.fd)" ]; then
    ok "RA2 seg_unit"
else
    bad "RA2 seg_unit:"
    echo "$got" | sed 's/^/     /'
fi

TARGETS="x86_64-linux aarch64-linux riscv64-linux"
if [ "$WIN" = 1 ]; then TARGETS="$TARGETS nat"; fi

# build_mode <mode> <image> <test> <target> [args...]: build() under a mode
build_mode() {
    local md="$1"
    shift
    case "$md" in
        on) build "$@" ;;
        segscan) FID_NAT_OFF=segscan build "$@" ;;
        holes) FID_NAT_OFF=holes build "$@" ;;
        uselive) FID_NAT_OFF=uselive build "$@" ;;
        greedy) FID_NAT_ON=greedy build "$@" ;;
        greedyhull) FID_NAT_ON=greedy FID_NAT_OFF=holes build "$@" ;;
    esac
}
MODES="on segscan holes uselive greedy greedyhull"

for T in tests/bootstrap/test_machine_segs.fd tests/bootstrap/test_machine_holes.fd; do
    base=$(basename "$T" .fd)
    for tgt in $TARGETS; do
        img="$OUT/$base-$tgt"
        built=1
        for md in $MODES; do
            if ! build_mode "$md" "$img-$md" "$T" "$tgt" --stats; then
                bad "RA2 $base $tgt builds ($md)"
                sed -n '1,5p' "$img-$md.log" | sed 's/^/     /'
                built=0
                continue
            fi
            if grep -q "RA2:" "$img-$md.log"; then bad "RA2 $base $tgt ($md): liveness error"; fi
            run_check "RA2 $base $tgt ($md)" "$img-$md" "$T" "$tgt"
        done
        [ "$built" = 1 ] || continue
        if [ "$base" = test_machine_segs ]; then
            fns="far reuse alt"
            hf=$(holed "$img-on.log" far)
            if [ -n "$hf" ] && [ "$hf" -gt 0 ]; then ok "RA2 $tgt: far has $hf ranges with holes"; else bad "RA2 $tgt: no [segs] line with holes for far ('$hf')"; fi
        else
            fns="holes"
        fi
        for fn in $fns; do
            a=$(reloads "$img-on.log" "$fn")
            b=$(reloads "$img-holes.log" "$fn")
            c=$(reloads "$img-greedy.log" "$fn")
            d=$(reloads "$img-greedyhull.log" "$fn")
            if [ -z "$a" ] || [ -z "$b" ] || [ -z "$c" ] || [ -z "$d" ]; then bad "RA2 $tgt: no [stats] line for $fn"; continue; fi
            strict=0
            if [ "$tgt" = x86_64-linux ] && { [ "$fn" = far ] || [ "$fn" = holes ]; }; then strict=1; fi
            if [ "$a" -le "$b" ] && { [ "$strict" = 0 ] || [ "$a" -lt "$b" ]; }; then
                ok "RA2 $tgt $fn: reloads $a over segments (FID_NAT_OFF=holes: $b)"
            else
                bad "RA2 $tgt $fn: reloads $a over segments (FID_NAT_OFF=holes: $b)"
            fi
            if [ "$c" -le "$d" ]; then ok "RA2 $tgt $fn: greedy reloads $c over segments (hulls: $d)"; else bad "RA2 $tgt $fn: greedy reloads $c over segments (hulls: $d)"; fi
        done
        if [ "$(grep '^\[stats\]' "$img-on.log")" = "$(grep '^\[stats\]' "$img-uselive.log")" ]; then
            ok "RA2 $base $tgt: same allocation with FID_NAT_OFF=uselive"
        else
            bad "RA2 $base $tgt: [stats] differ with FID_NAT_OFF=uselive"
        fi
    done
done

# --- other programs (FILES=...) -----------------------------------------------
for T in ${FILES:-}; do
    base=$(basename "$T" .fd)
    for tgt in $TARGETS; do
        img="$OUT/files-$base-$tgt"
        if build "$img" "$T" "$tgt"; then
            run_check "RA2 $base $tgt" "$img" "$T" "$tgt"
        else
            if grep -q "allocation\|RA2:\|writes register\|clobbers register" "$img.log"; then
                bad "RA2 $base $tgt: $(grep -m1 'allocation\|RA2:\|writes register\|clobbers register' "$img.log")"
            fi
        fi
    done
done

if [ "$fails" -gt 0 ]; then
    echo "$fails check(s) failed"
    exit 1
fi
echo "all RA2 checks passed"
