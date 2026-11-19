#!/usr/bin/env bash
# RA9 (docs/roadmap/llvm-roadmap.md): the greedy allocator of the machine
# layer (compiler/nat_ir.fd ng_greedy_assign) over many programs, run after
# the compiler is built:
#   bash tests/nattools/greedy_compare.sh [fid]   (default release/fid.exe)
#
# For each program (FILES, default every tests/bootstrap/test_*.fd that is
# not an expected-fail test) and each target (TARGETS, default
# "x86_64-linux aarch64-linux riscv64-linux"):
#  1. FID_VERIFY=1 FID_NAT_ON=greedy `fid emit` must succeed: the MIR
#     verifier and the allocation checker (ng_mir_check) accept every
#     function the greedy allocator assigned. A program whose build fails
#     the same way with the linear scan (FID_VERIFY=1, no switch) is
#     counted apart, as not the allocator's.
#  2. FID_NAT_ON=greedycmp `fid emit --stats`: the linear scan allocates
#     and each function prints "[ra9] f: N ranges; linear scan spills L
#     (weight WL), greedy G (weight WG, E evictions)" (the greedy
#     assignment on the same ranges, before RA2's second chance). The
#     totals are summed per target and the programs where the two differ
#     most are listed.
# Passing: no checker failure with the greedy allocator (1). The table (2)
# is the comparison RA9 asks for; it is reported, not judged.
# Exit 1 on a failure of (1).
set -u
FID="${1:-release/fid.exe}"
OUT="${TMPDIR:-/tmp}/fid-greedy-compare"
mkdir -p "$OUT"
TARGETS="${TARGETS:-x86_64-linux aarch64-linux riscv64-linux}"
if [ -z "${FILES:-}" ]; then
    FILES=$(ls tests/bootstrap/test_*.fd | grep -v '_fail[0-9]*\.fd$')
fi
fails=0
skipped=0
: >"$OUT/rows.txt"
for t in $TARGETS; do
    for src in $FILES; do
        base=$(basename "$src" .fd)
        img="$OUT/$base-$t"
        if ! FID_VERIFY=1 FID_NAT_ON=greedy "$FID" emit "$src" --target "$t" -o "$img" >"$img.greedy.log" 2>&1; then
            if FID_VERIFY=1 "$FID" emit "$src" --target "$t" -o "$img.lin" >"$img.lin.log" 2>&1; then
                echo "FAIL $t $base: greedy build: $(grep -v '^\[' "$img.greedy.log" | tail -1)"
                fails=$((fails + 1))
            else
                skipped=$((skipped + 1))
            fi
            continue
        fi
        if FID_NAT_ON=greedycmp "$FID" emit "$src" --target "$t" --stats -o "$img.cmp" >"$img.cmp.log" 2>&1; then
            sed -n 's/^\[ra9\] [^:]*: \([0-9]*\) ranges; linear scan spills \([0-9]*\) (weight \([0-9]*\)), greedy \([0-9]*\) (weight \([0-9]*\), \([0-9]*\) evictions).*/\1 \2 \3 \4 \5 \6/p' "$img.cmp.log" |
                awk -v t="$t" -v f="$base" '{r += $1; ls += $2; lw += $3; gs += $4; gw += $5; ev += $6} END {print t, f, r + 0, ls + 0, lw + 0, gs + 0, gw + 0, ev + 0}' >>"$OUT/rows.txt"
        fi
    done
done
echo
echo "RA9: linear scan against greedy, spills before RA2 (fid emit --stats, FID_NAT_ON=greedycmp)"
printf '%-14s %8s %10s %12s %10s %12s %10s\n' target ranges lin-spills lin-weight gr-spills gr-weight evictions
for t in $TARGETS; do
    awk -v t="$t" '$1 == t {r += $3; ls += $4; lw += $5; gs += $6; gw += $7; ev += $8} END {printf "%-14s %8d %10d %12d %10d %12d %10d\n", t, r, ls, lw, gs, gw, ev}' "$OUT/rows.txt"
done
echo
echo "programs where the spill weights differ most (target, program, linear weight, greedy weight):"
awk '{d = $5 - $7; if (d < 0) d = -d; if (d > 0) print d, $1, $2, $5, $7}' "$OUT/rows.txt" | sort -rn | head -15 | awk '{print "  " $2, $3, $4, $5}'
echo
echo "$skipped program build(s) failing with the linear scan too (not counted)"
if [ "$fails" -gt 0 ]; then echo "$fails greedy checker failure(s)"; exit 1; fi
echo "all greedy builds passed the allocation checker"
