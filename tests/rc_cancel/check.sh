#!/usr/bin/env bash
# LF15 (llvm-roadmap): reference counts removed across if/else joins and
# branches (compiler/lower.fd lw_rc_cancel_param_slots: copies of
# parameters, LOWER_CANCEL_PARAM_SLOTS, and of local variables,
# LOWER_CANCEL_LOCAL_SLOTS; FID_RC_SLOTS_OFF=1 turns both off).
#
# For tests/bootstrap/test_rc_cancel_diamond.fd and test_rc_cancel_local.fd:
#   1. `dump-c` with the pass on and off. A copy that is only read loses
#      its count (Str_dup / Vec_share / HashMap_share) and the drops of its
#      variable ("gone": no count left, fewer drops; "fewer": fewer counts).
#      A copy that is moved, changed, reassigned or outlives its source
#      keeps them ("same": counts and drops as with the pass off).
#   2. `run --leaks` with the pass on and off: the expected output written
#      in the test, and every block freed.
#
# usage: tests/rc_cancel/check.sh [fid]   (default: release/fid.exe; run from
# the repository root, one at a time: `run --leaks` builds in a fixed file
# there; it needs a C compiler, e.g. CC="zig cc")
set -u
FID="${1:-release/fid.exe}"
OUT=$(mktemp -d)
fail=0
removed=0
ok() { echo "ok    $1"; }
bad() { echo "FAIL  $1"; fail=1; }

# count FILE FN PATTERN: lines of function FN in the C that match PATTERN
count() {
    awk -v fn="$2" 'p==0 && index($0, " " fn "(void* v0") && substr($0,length($0))=="{" {p=1} p{print} p && $0=="}"{exit}' "$1" | grep -c -e "$3"
}

# check NAME FN KIND EXPECT  (KIND: str|vec|map; EXPECT: gone|fewer|same)
check() {
    local name="$1" fn="$2" kind="$3" expect="$4"
    local cp dp
    case "$kind" in
        str) cp='Str_dup(\|P_STR_DUP('; dp='Str_drop(\|P_STR_DROP(' ;;
        vec) cp='Vec_share('; dp='Vec_drop(' ;;
        map) cp='HashMap_share('; dp='HashMap_drop(' ;;
    esac
    local on="$OUT/$name.on.c" off="$OUT/$name.off.c"
    if ! grep -q " $fn(void\* v0" "$on"; then
        bad "$name $fn: function not found in the C"
        return
    fi
    local c_on c_off d_on d_off
    c_on=$(count "$on" "$fn" "$cp"); c_off=$(count "$off" "$fn" "$cp")
    d_on=$(count "$on" "$fn" "$dp"); d_off=$(count "$off" "$fn" "$dp")
    local what="$name $fn ($kind): counts $c_off -> $c_on, drops $d_off -> $d_on"
    case "$expect" in
        gone)
            if [ "$c_off" -eq 0 ]; then
                echo "skip  $what (no count to remove: lowering borrows already)"
            elif [ "$c_on" -eq 0 ] && [ "$d_on" -lt "$d_off" ]; then
                ok "$what"; removed=$((removed + c_off))
            else
                bad "$what (expected no count left and fewer drops)"
            fi ;;
        fewer)
            if [ "$c_on" -lt "$c_off" ]; then
                ok "$what"; removed=$((removed + c_off - c_on))
            else
                bad "$what (expected fewer counts)"
            fi ;;
        same)
            if [ "$c_on" -eq "$c_off" ] && [ "$d_on" -eq "$d_off" ]; then
                ok "$what"
            else
                bad "$what (this copy must keep its count and drops)"
            fi ;;
    esac
}

# expected NAME FILE: the lines after "// Expected output:" in the test
expected() {
    awk '/^\/\/ Expected output:/{p=1; next} p && /^\/\/ /{sub(/^\/\/ /, ""); print; next} p{exit}' "$2" > "$OUT/$1.expected"
}

for name in diamond local; do
    T="tests/bootstrap/test_rc_cancel_$name.fd"
    if ! "$FID" dump-c "$T" > "$OUT/$name.on.c" 2> "$OUT/$name.on.err"; then
        bad "$name: dump-c failed: $(head -c 300 "$OUT/$name.on.err")"
    fi
    if ! FID_RC_SLOTS_OFF=1 "$FID" dump-c "$T" > "$OUT/$name.off.c" 2> "$OUT/$name.off.err"; then
        bad "$name: dump-c (FID_RC_SLOTS_OFF=1) failed: $(head -c 300 "$OUT/$name.off.err")"
    fi
done

# copies of parameters (LOWER_CANCEL_PARAM_SLOTS)
check diamond pick_len str gone
check diamond both_arms str gone
check diamond loop_copy str gone
check diamond vec_arms vec gone
check diamond map_arms map gone
check diamond kept str fewer
check diamond moved str same
check diamond grown str same
check diamond pushed vec same
# copies of local variables (LOWER_CANCEL_LOCAL_SLOTS)
check local local_arms str gone
check local vec_local vec gone
check local map_local map gone
check local rounds str gone
check local reassigned str same
check local moved_src str same
check local grown_copy str same
check local pushed_copy vec same
check local outlives str same

if [ "$removed" -lt 4 ]; then
    bad "only $removed counts removed in all (expected at least 4: the tests would not exercise the pass)"
else
    ok "$removed counts removed in all"
fi

# the programs: same output, nothing leaked, with the pass on and off
for name in diamond local; do
    T="tests/bootstrap/test_rc_cancel_$name.fd"
    expected "$name" "$T"
    for mode in on off; do
        if [ "$mode" = off ]; then
            FID_RC_SLOTS_OFF=1 "$FID" run "$T" --leaks > "$OUT/$name.$mode.out" 2> "$OUT/$name.$mode.runerr"
        else
            "$FID" run "$T" --leaks > "$OUT/$name.$mode.out" 2> "$OUT/$name.$mode.runerr"
        fi
        grep -v -e '^Built native executable' -e '^Reused cached artifact' -e '^\[' "$OUT/$name.$mode.out" > "$OUT/$name.$mode.clean"
        if ! diff -q --strip-trailing-cr "$OUT/$name.expected" "$OUT/$name.$mode.clean" > /dev/null; then
            bad "$name ($mode): output differs"
            diff --strip-trailing-cr "$OUT/$name.expected" "$OUT/$name.$mode.clean" | head -10 | sed 's/^/      /'
        elif ! grep -q "every block was freed" "$OUT/$name.$mode.runerr" "$OUT/$name.$mode.out"; then
            bad "$name ($mode): leaks: $(grep -h -m1 'fid --leaks' "$OUT/$name.$mode.runerr" "$OUT/$name.$mode.out" || echo 'no leak report')"
        else
            ok "$name ($mode): output and every block freed"
        fi
    done
done

rm -rf "$OUT"
if [ "$fail" -ne 0 ]; then
    echo "rc_cancel: FAILED"
    exit 1
fi
echo "rc_cancel: all passed"
