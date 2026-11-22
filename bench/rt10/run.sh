#!/bin/bash
# bench/rt10/run.sh — competitive roadmap RT10 on the C path: the M:N
# scheduler's per-worker rings with work stealing against one shared-lock
# run queue. Builds two programs through C (`fid ccompile`) and runs each
# RUNS times in three modes, printing the median of each as a Markdown table:
#   shared    FIDES_RUNQ=shared: every push and pop through the global queue
#             under its lock (the queues the rings replaced)
#   rings     the default: each worker's lock-free ring, steal-half
#   1 worker  FIDES_WORKERS=1: one cooperative scheduler, as before RT10
# The programs:
#   bench/rt10/runq_bench.fd      tasks made once, each yielding many times:
#                                 ns per yield (one push and one pop each),
#                                 the queues alone
#   tests/ctx_switch/steal_bench.fd  short tasks spawned three ways (fan-out,
#                                 tree, from main): ns per task, which also
#                                 counts making and ending each task
#
#   bash bench/rt10/run.sh
#   RUNS=5 WORKERS=8 TASKS=50000 bash bench/rt10/run.sh
#
# FID (default release/fid, or release/fid.exe), CC (default zig cc),
# WORKERS (default: FIDES_WORKERS as set, else one per CPU), TASKS/WORK for
# steal_bench (200000/100), RQ="tasks yields work" for runq_bench
# (2000 500 50). STEAL_EXE / RUNQ_EXE: run these instead of building.
# Exit 1 when a run failed or counted wrong.
set -u
cd "$(dirname "$0")/../.." || exit 2
FID="${FID:-release/fid}"
[ -x "$FID" ] || FID="release/fid.exe"
export CC="${CC:-zig cc}"
TASKS="${TASKS:-200000}"
WORK="${WORK:-100}"
RQ="${RQ:-2000 500 50}"
RUNS="${RUNS:-3}"
OUT="${OUT:-build/bench/rt10}"
mkdir -p "$OUT"
build() {
    "$FID" ccompile "$1" -o "$2" > "$OUT/build.log" 2>&1 || { echo "build of $1 failed:"; tail -5 "$OUT/build.log"; exit 1; }
}
STEAL_EXE="${STEAL_EXE:-}"
RUNQ_EXE="${RUNQ_EXE:-}"
[ -n "$STEAL_EXE" ] || { STEAL_EXE="$OUT/steal_bench.exe"; build tests/ctx_switch/steal_bench.fd "$STEAL_EXE"; }
[ -n "$RUNQ_EXE" ] || { RUNQ_EXE="$OUT/runq_bench.exe"; build bench/rt10/runq_bench.fd "$RUNQ_EXE"; }
[ -n "${WORKERS:-}" ] && export FIDES_WORKERS="$WORKERS"

bad=0
# run <file> <exe and args> -- <env...>: RUNS runs appended to $OUT/<file>
run() {
    local f="$OUT/$1"; shift
    local cmd=()
    while [ "$1" != "--" ]; do cmd+=("$1"); shift; done
    shift
    : > "$f"
    for i in $(seq "$RUNS"); do
        env "$@" "${cmd[@]}" >> "$f" 2>&1 || { echo "$f: run $i failed"; tail -3 "$f"; bad=1; return; }
    done
    grep -q WRONG "$f" && { echo "$f: wrong result"; bad=1; }
}
# median <file> <phase>: the median number before " ns per" on that phase's lines
median() {
    grep "^$2:" "$OUT/$1" | sed 's/.* \([0-9][0-9]*\) ns per.*/\1/' | sort -n \
        | awk '{ v[NR] = $1 } END { if (NR == 0) print "-"; else print v[int((NR + 1) / 2)] }'
}
for m in shared rings one; do
    case $m in
        shared) envs="FIDES_RUNQ=shared" ;;
        rings) envs="FIDES_RUNQ=" ;;
        one) envs="FIDES_WORKERS=1 FIDES_RUNQ=" ;;
    esac
    # shellcheck disable=SC2086
    run "runq_$m.txt" "$RUNQ_EXE" $RQ -- $envs
    # shellcheck disable=SC2086
    run "steal_$m.txt" "$STEAL_EXE" "$TASKS" "$WORK" -- $envs
done
echo "RT10, C path: median of $RUNS runs (workers: ${FIDES_WORKERS:-one per CPU});"
echo "runq_bench $RQ: ns per yield; steal_bench $TASKS $WORK: ns per task"
echo
echo "| program, phase       | shared lock | rings + stealing | 1 worker |"
echo "|----------------------|-------------|------------------|----------|"
printf "| %-20s | %11s | %16s | %8s |\n" "runq_bench, yield" "$(median runq_shared.txt yield)" "$(median runq_rings.txt yield)" "$(median runq_one.txt yield)"
for ph in fan-out tree global; do
    printf "| %-20s | %11s | %16s | %8s |\n" "steal_bench, $ph" "$(median steal_shared.txt $ph)" "$(median steal_rings.txt $ph)" "$(median steal_one.txt $ph)"
done
exit $bad
