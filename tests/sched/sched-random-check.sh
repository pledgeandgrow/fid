#!/bin/bash
# Roadmap RL17: the seeded random scheduler (FIDES_SCHED_RANDOM=<seed>) of
# the C runtime (compiler/codegen_c.fd "Roadmap RL17": a shuffled pass of
# __p_co_round, random yields at atomics, mutexes and channels, random
# sleeps off tasks) and of the native runtimes (pe_runtime/conc.fd,
# linux_runtime/conc.fd "Random-choice scheduling").
#   FID=release/fid.exe [N=300] [BACKENDS="ccompile emit"] bash tests/sched/sched-random-check.sh
# Run from the repository root. For each backend it builds
#   tests/sched/sched_order.fd            (its output is the schedule) and
#   tests/bootstrap/test_runtime_sched_random.fd (its output must not change)
# and checks:
#  1. ccompile only: without FIDES_SCHED_RANDOM, and with it 0 (off), the
#     order is the C scheduler's round-robin 012345012345012345.
#  2. N seeds: test_runtime_sched_random prints its expected lines (tasks: 8
#     / atomic total: 8000 / sum of results: 36) for every seed (native:
#     the first NV_NATIVE=10, ~16 s a run there); sched_order
#     prints "records: 18" for every seed and at least N/4 distinct orders
#     (a scheduler without the random mode gives one, or a handful).
#  3. ccompile only: the same seed twice gives the same order (seeds 1-20):
#     a failing seed replays its schedule. (Native runtimes: a worker thread
#     beside main, so the order also depends on timing: not checked.)
# Exit 0 when every check passes. CC defaults to "zig cc" for ccompile.
FID=${FID:-release/fid.exe}
N=${N:-300}
BACKENDS=${BACKENDS:-"ccompile emit"}
export CC=${CC:-zig cc}
D=$(mktemp -d ./_sched_random.XXXXXX)
D=${D#./}
trap 'rm -rf "$D"' EXIT
fail=0
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1"; fail=1; }
EXE=""
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) EXE=".exe" ;; esac
ORD=tests/sched/sched_order.fd
INV=tests/bootstrap/test_runtime_sched_random.fd
WANT_INV="tasks: 8|atomic total: 8000|sum of results: 36|"
RR="order: 012345012345012345"

run1() {   # exe, seed (empty: unset) -> stdout with lines joined by |
    if [ -z "$2" ]; then
        "./$1" 2>/dev/null | tr -d '\r' | tr '\n' '|'
    else
        FIDES_SCHED_RANDOM=$2 "./$1" 2>/dev/null | tr -d '\r' | tr '\n' '|'
    fi
}

for B in $BACKENDS; do
    o="$D/order-$B$EXE"; v="$D/inv-$B$EXE"
    if ! "$FID" "$B" "$ORD" -o "$o" > "$D/build-o-$B.log" 2>&1 || [ ! -f "$o" ]; then
        bad "$B: build of $ORD"; tail -5 "$D/build-o-$B.log"; continue
    fi
    if ! "$FID" "$B" "$INV" -o "$v" > "$D/build-v-$B.log" 2>&1 || [ ! -f "$v" ]; then
        bad "$B: build of $INV"; tail -5 "$D/build-v-$B.log"; continue
    fi
    if [ "$B" = ccompile ]; then
        r0=$(run1 "$o" ""); rz=$(run1 "$o" 0)
        if [ "$r0" = "records: 18|$RR|" ] && [ "$rz" = "$r0" ]; then
            ok "$B: default and seed 0 run round-robin"
        else
            bad "$B: default '$r0', seed 0 '$rz' (want records: 18|$RR|)"
        fi
    fi
    badinv=0; badrec=0
    # native random mode sleeps 1 ms at one atomic in eight (OS threads):
    # test_runtime_sched_random takes ~16 s a run there, so fewer seeds
    NV=$N
    [ "$B" != ccompile ] && NV=${NV_NATIVE:-10}
    : > "$D/orders-$B.txt"
    for s in $(seq 1 "$N"); do
        if [ "$s" -le "$NV" ]; then
            r=$(run1 "$v" "$s")
            if [ "$r" != "$WANT_INV" ]; then
                badinv=$((badinv + 1)); [ $badinv -le 3 ] && echo "     seed $s: $r"
            fi
        fi
        r=$(run1 "$o" "$s")
        case "$r" in
            "records: 18|order: "*) echo "$r" >> "$D/orders-$B.txt" ;;
            *) badrec=$((badrec + 1)); [ $badrec -le 3 ] && echo "     seed $s: $r" ;;
        esac
    done
    if [ $badinv -eq 0 ]; then ok "$B: $INV the same for $NV seeds"; else bad "$B: $INV differs for $badinv of $NV seeds"; fi
    if [ $badrec -eq 0 ]; then ok "$B: $ORD 18 records for $N seeds"; else bad "$B: $ORD wrong for $badrec of $N seeds"; fi
    distinct=$(sort -u "$D/orders-$B.txt" | wc -l | tr -d ' ')
    if [ "$distinct" -ge $((N / 4)) ]; then
        ok "$B: $distinct distinct schedules in $N seeds"
    else
        bad "$B: only $distinct distinct schedules in $N seeds (random mode off?)"
    fi
    if [ "$B" = ccompile ]; then
        same=1
        for s in $(seq 1 20); do
            [ "$(run1 "$o" "$s")" = "$(run1 "$o" "$s")" ] || { same=0; echo "     seed $s differs between two runs"; }
        done
        if [ $same = 1 ]; then ok "$B: a seed replays its schedule (seeds 1-20)"; else bad "$B: a seed gave two schedules"; fi
    fi
done
[ $fail = 0 ] && echo "sched-random-check: all passed"
exit $fail
