#!/bin/bash
# tests/memprof/check_folded.sh [FID]: the memory profile as collapsed
# stacks (roadmap L7 exit criterion "CPU and memory profiler with flame
# graphs", complaint CO5).
#
# `fid run <file> --mem` with FID_MEM_FOLDED=<path> in the environment
# writes, at exit, the bytes allocated under each whole stack of Fides
# functions, one line per stack, outermost function first:
#     main;keep 512
#     main;fill 500
# the format flamegraph.pl, inferno-flamegraph and speedscope read.
# (compiler/codegen_c.fd __p_ms_note / __p_ms_write, under P_MEMPROF.)
#
# Run from the project root; needs a C compiler (CC, as for ccompile).
#   bash tests/memprof/check_folded.sh [release/fid.exe]
set -u
FID="${1:-release/fid.exe}"
OUT=$(mktemp -d)
fail=0
ok()  { echo "  ok    $1"; }
bad() { echo "  FAIL  $1: $2"; fail=$((fail + 1)); }

FID_MEM_FOLDED="$OUT/mem.folded" "$FID" run tests/memprof/mem_profile.fd --mem > "$OUT/out" 2> "$OUT/err"
if [ ! -s "$OUT/mem.folded" ]; then
    bad "folded: file written" "$(tail -c 300 "$OUT/err")"
else
    tr -d '\015' < "$OUT/mem.folded" > "$OUT/f"
    # every line: frames joined by ';', a space, a byte count
    if grep -Evq '^[^ ;]+(;[^ ;]+)* [0-9]+$|^\(no Fides frame\) [0-9]+$|^\(other stacks\) [0-9]+$' "$OUT/f"; then
        bad "folded: format" "$(grep -Ev '^[^ ;]+(;[^ ;]+)* [0-9]+$' "$OUT/f" | head -3)"
    else
        ok "folded: format"
    fi
    # keep's two 256-byte blocks under main;keep, fill's five 100-byte ones
    # under main;fill (a frame or more of core may follow the function)
    keep=$(grep -E '(^|;)main;keep(;[^ ]*)? [0-9]+$' "$OUT/f" | awk '{s += $NF} END {print s + 0}')
    fillb=$(grep -E '(^|;)main;fill(;[^ ]*)? [0-9]+$' "$OUT/f" | awk '{s += $NF} END {print s + 0}')
    if [ "$keep" -eq 512 ]; then ok "folded: main;keep 512"; else bad "folded: main;keep" "$keep bytes"; fi
    if [ "$fillb" -eq 500 ]; then ok "folded: main;fill 500"; else bad "folded: main;fill" "$fillb bytes"; fi
    # the same bytes as the --mem totals
    total=$(awk '{s += $NF} END {print s + 0}' "$OUT/f")
    want=$(tr -d '\015' < "$OUT/err" | sed -n 's/^fid --mem: [0-9]* allocation(s), \([0-9]*\) bytes;.*/\1/p' | head -1)
    if [ -n "$want" ] && [ "$total" -eq "$want" ]; then ok "folded: total $total bytes"; else bad "folded: total" "$total vs ${want:-none}"; fi
fi
# without the variable: no file, the report as before
"$FID" run tests/memprof/mem_profile.fd --mem > /dev/null 2> "$OUT/err2"
if grep -q "collapsed stacks" "$OUT/err2"; then bad "folded: off by default" "written without FID_MEM_FOLDED"; else ok "folded: off by default"; fi
rm -rf "$OUT"
if [ "$fail" -ne 0 ]; then echo "check_folded: $fail failed"; exit 1; fi
echo "check_folded: all passed"
