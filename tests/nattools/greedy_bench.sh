#!/usr/bin/env bash
# RA9 (docs/roadmap/llvm-roadmap.md): run-time comparison of the greedy
# allocator (compiler/nat_ir.fd ng_greedy_assign, FID_NAT_ON=greedy) with
# the default linear scan, on the benchmark programs. Run after the
# compiler is built:
#   bash tests/nattools/greedy_bench.sh [fid]     (default release/fid.exe)
#
# PROGS: the programs (default bench/fides/*.fd and bench/game/fides/*.fd;
# each prints one number and reads no input). TARGETS: default "nat" on
# Windows (the --nat PE, run natively) plus "x86_64-linux" when wsl.exe is
# there (run inside WSL); on Linux the host's own target. RUNS: runs per
# image, the best wall time counts (default 5).
#
# For each program and target both images are built with `fid emit
# --stats` and FID_VERIFY=1 (so the allocation checker also judges the
# greedy assignment of every function of the benchmarks), then:
#  - the outputs must be equal (else WRONG, and the script exits 1);
#  - the table gives the best time of each, greedy / linear, and the sums
#    over all functions of the `[stats]` lines' spilled values and reloads
#    (after RA2, RA3, RA4 and RA7: what the program finally runs with).
# The markdown table goes to stdout and to $OUT/greedy-bench.md, ready to
# paste into docs/reference/greedy-allocator.md ("Results").
set -u
FID="${1:-release/fid.exe}"
# (under the repository, gitignored: WSL reaches the Linux images by a
# path relative to the repository root)
OUT="${OUT:-bench/out/greedy}"
mkdir -p "$OUT"
RUNS="${RUNS:-5}"
WIN=0
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; esac
HAVE_WSL=0
if command -v wsl.exe >/dev/null 2>&1; then HAVE_WSL=1; fi
if [ -z "${TARGETS:-}" ]; then
    if [ "$WIN" = 1 ]; then
        TARGETS="nat"
        [ "$HAVE_WSL" = 1 ] && TARGETS="nat x86_64-linux"
    else
        case "$(uname -m)" in
            aarch64|arm64) TARGETS="aarch64-linux" ;;
            riscv64) TARGETS="riscv64-linux" ;;
            *) TARGETS="x86_64-linux" ;;
        esac
    fi
fi
if [ -z "${PROGS:-}" ]; then PROGS=$(ls bench/fides/*.fd bench/game/fides/*.fd 2>/dev/null); fi
wrong=0

build() {
    local img="$1" src="$2" tgt="$3"
    shift 3
    if [ "$tgt" = nat ]; then
        FID_VERIFY=1 "$FID" emit "$src" --nat --stats "$@" -o "$img" >"$img.log" 2>&1
    else
        FID_VERIFY=1 "$FID" emit "$src" --target "$tgt" --stats "$@" -o "$img" >"$img.log" 2>&1
    fi
}

# best <img> <tgt> -> "<best seconds> <last output line>"
best() {
    local img="$1" tgt="$2" b="" o="" i t0 t1 ns
    if [ "$tgt" != nat ] && [ "$WIN" = 1 ]; then
        # inside WSL, one start for all runs (its start-up is not timed)
        MSYS_NO_PATHCONV=1 wsl.exe -e bash -c 'b=""; o=""; for _ in $(seq 1 "$1"); do t0=$(date +%s%N); o=$("$2" 2>&1 | tail -1); t1=$(date +%s%N); ns=$((t1 - t0)); if [ -z "$b" ] || [ "$ns" -lt "$b" ]; then b=$ns; fi; done; echo "$b $o"' run "$RUNS" "./$img" 2>/dev/null | tr -d '\r' |
            awk '{printf "%.3f", $1 / 1e9; $1 = ""; print}'
        return
    fi
    for i in $(seq 1 "$RUNS"); do
        t0=$(date +%s%N)
        o=$("$img" 2>&1 | tr -d '\r' | tail -1)
        t1=$(date +%s%N)
        ns=$((t1 - t0))
        if [ -z "$b" ] || [ "$ns" -lt "$b" ]; then b=$ns; fi
    done
    awk -v n="$b" -v o="$o" 'BEGIN {printf "%.3f %s\n", n / 1e9, o}'
}

# sums of the [stats] lines: "<spilled> <reloads>"
spills() { sed -n 's/^\[stats\] [^:]*: spilled \([0-9]*\), reloads \([0-9]*\),.*/\1 \2/p' "$1" | awk '{s += $1; r += $2} END {print s + 0, r + 0}'; }
# the allocator's compile time: "[prof] native allocation: N ms, ..."
alloc_ms() { v=$(sed -n 's/^\[prof\] native allocation: \([0-9]*\)ms.*/\1/p' "$1" | tail -1); echo "${v:--}"; }

{
echo "| program | target | linear (s) | greedy (s) | greedy / linear | spilled lin / gr | reloads lin / gr | alloc ms lin / gr |"
echo "|---|---|---:|---:|---:|---:|---:|---:|"
} >"$OUT/greedy-bench.md"
for src in $PROGS; do
    b=$(basename "$src" .fd)
    for tgt in $TARGETS; do
        ext=""
        [ "$tgt" = nat ] && ext=".exe"
        lin="$OUT/$b-$tgt-lin$ext"
        gre="$OUT/$b-$tgt-gr$ext"
        if ! build "$lin" "$src" "$tgt"; then echo "| $b | $tgt | build failed | | | | |" >>"$OUT/greedy-bench.md"; continue; fi
        if ! FID_NAT_ON=greedy build "$gre" "$src" "$tgt"; then
            echo "| $b | $tgt | | greedy build failed: $(grep -v '^\[' "$gre.log" | tail -1) | | | |" >>"$OUT/greedy-bench.md"
            wrong=$((wrong + 1))
            continue
        fi
        read -r lt lo < <(best "$lin" "$tgt")
        read -r gt go < <(best "$gre" "$tgt")
        read -r ls lr < <(spills "$lin.log")
        read -r gs gr < <(spills "$gre.log")
        if [ "${lo:-}" != "${go:-}" ]; then
            echo "| $b | $tgt | $lt | WRONG ($go, linear $lo) | | $ls / $gs | $lr / $gr |" >>"$OUT/greedy-bench.md"
            wrong=$((wrong + 1))
            continue
        fi
        ratio=$(awk -v a="$gt" -v b="$lt" 'BEGIN {if (b > 0) printf "%.3f", a / b; else print "-"}')
        echo "| $b | $tgt | $lt | $gt | $ratio | $ls / $gs | $lr / $gr | $(alloc_ms "$lin.log") / $(alloc_ms "$gre.log") |" >>"$OUT/greedy-bench.md"
    done
done
cat "$OUT/greedy-bench.md"
echo
echo "(best of $RUNS runs each; FID_VERIFY=1 builds; $OUT/greedy-bench.md)"
if [ "$wrong" -gt 0 ]; then echo "$wrong program(s) wrong or not built with the greedy allocator"; exit 1; fi
echo "all greedy images built and printed what the linear scan's print"
