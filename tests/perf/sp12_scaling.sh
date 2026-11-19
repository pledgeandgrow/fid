#!/usr/bin/env bash
# SP12 (llvm-roadmap / competitive-roadmap CS4): the lowering of one long
# function must not grow with the square of its length.
#
# tests/perf/sp12_gen.py writes a program whose `work` function has N
# groups of statements (strings kept in a second variable, Vecs that stay in
# the function, a Vec copied and only read, strings copied from a place).
# Each of those used to make compiler/lower.fd walk the rest of the function
# once per variable (Lowerer_last_use, Lowerer_readonly_copy_in, the
# count-cancelling and stack passes) and copy the block's instruction list
# once per instruction (ir.fd IrFunction_emit).
#
# For N and 2N the script
#   1. runs `$FID dump-c --time` and reads the `[time] lower:` row;
#   2. builds and runs the 2N program (`$FID ccompile`) and compares its
#      output with what sp12_gen.py --expect computes.
# Passing: the outputs match, and (lower(2N) - lower(1)) / (lower(N) -
# lower(1)) is below MAX_RATIO (default 3.5: twice the statements in linear
# time is about 2x, in quadratic time about 4x; lower(1) is the fixed cost of
# lowering core and the runtime, taken out).
#
# usage: FID=release/fid.exe bash tests/perf/sp12_scaling.sh [N]
#        (default N=400; MAX_RATIO=3.5; run from the repository root on a
#        quiet machine, one fid at a time; ccompile needs CC, e.g. "zig cc")
set -u
FID="${FID:-release/fid.exe}"
N="${1:-400}"
MAX_RATIO="${MAX_RATIO:-3.5}"
OUT=$(mktemp -d)
fail=0

lower_ms() {
    "$FID" dump-c "$1" --time 2>"$OUT/time.txt" >"$OUT/out.c"
    local rc=$?
    if [ $rc -ne 0 ]; then
        echo "FAIL  dump-c $1 exited $rc" >&2
        tail -5 "$OUT/time.txt" >&2
        echo -1
        return
    fi
    grep '^\[time\] lower:' "$OUT/time.txt" | head -1 | sed 's/[^0-9]*\([0-9][0-9]*\) ms.*/\1/'
}

N2=$((N * 2))
python3 tests/perf/sp12_gen.py 1 > "$OUT/g0.fd" || python tests/perf/sp12_gen.py 1 > "$OUT/g0.fd"
python3 tests/perf/sp12_gen.py "$N" > "$OUT/g1.fd" || python tests/perf/sp12_gen.py "$N" > "$OUT/g1.fd"
python3 tests/perf/sp12_gen.py "$N2" > "$OUT/g2.fd" || python tests/perf/sp12_gen.py "$N2" > "$OUT/g2.fd"
t0=$(lower_ms "$OUT/g0.fd")
t1=$(lower_ms "$OUT/g1.fd")
t2=$(lower_ms "$OUT/g2.fd")
echo "lower N=1: $t0 ms, N=$N: $t1 ms, N=$N2: $t2 ms"
if [ "$t0" = "-1" ] || [ "$t1" = "-1" ] || [ "$t2" = "-1" ] || [ -z "$t0" ] || [ -z "$t1" ] || [ -z "$t2" ]; then
    echo "FAIL  no lowering time"
    fail=1
else
    ok=$(awk -v z="$t0" -v a="$t1" -v b="$t2" -v m="$MAX_RATIO" 'BEGIN { a = a - z; b = b - z; if (a < 1) a = 1; r = b / a; printf "%.2f %d", r, (r < m) }')
    ratio=${ok% *}
    if [ "${ok#* }" = "1" ]; then
        echo "ok    ratio $ratio (< $MAX_RATIO)"
    else
        echo "FAIL  ratio $ratio (>= $MAX_RATIO): the lowering grows faster than linearly"
        fail=1
    fi
fi

# the program still means what it did
"$FID" ccompile "$OUT/g2.fd" -o "$OUT/g2.exe" >"$OUT/cc.txt" 2>&1
if [ ! -x "$OUT/g2.exe" ] && [ ! -f "$OUT/g2.exe" ]; then
    echo "FAIL  ccompile of the N=$N2 program"
    tail -5 "$OUT/cc.txt"
    fail=1
else
    got=$("$OUT/g2.exe" | tr -d '\r')
    want=$(python3 tests/perf/sp12_gen.py "$N2" --expect 2>/dev/null || python tests/perf/sp12_gen.py "$N2" --expect)
    want=$(echo "$want" | tr -d '\r\n')
    got=$(echo "$got" | tr -d '\n')
    if [ "$got" = "$want" ]; then
        echo "ok    output: $got"
    else
        echo "FAIL  output: got '$got', want '$want'"
        fail=1
    fi
fi
rm -rf "$OUT"
exit $fail
