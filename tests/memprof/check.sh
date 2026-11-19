#!/bin/bash
# tests/memprof/check.sh [FID]: the run-time reports of the C path.
#
#   CM8      fid run --leaks names the line that allocated a leaked block
#   L7C      fid run --mem prints the peak and the bytes by function and line
#   CA9      a panic in a task prints the chain of tasks it ran for
#   L6C      p_dl_load / p_dl_find resolve a real symbol, or give 0 (None)
#
# Run from the project root (fid run --leaks/--mem build a temporary
# program in the current folder). Needs a C compiler (CC, as for ccompile).
#
#   bash tests/memprof/check.sh                      # release/fid.exe
#   bash tests/memprof/check.sh release/fid-dev.exe
set -u
FID="${1:-release/fid.exe}"
OUT=$(mktemp -d)
EXE=""
case "$(uname -s 2>/dev/null)" in
    MINGW*|MSYS*|CYGWIN*) EXE=".exe" ;;
esac
fail=0

ok()   { echo "  ok    $1"; }
bad()  { echo "  FAIL  $1: $2"; fail=$((fail + 1)); }
# Windows text-mode stderr ends lines with CR LF
nocr() { for f in "$@"; do tr -d '\015' < "$f" > "$f.t" && mv "$f.t" "$f"; done; }

# --- CM8: the allocating line in the leak report -------------------------
"$FID" run tests/memprof/leak_line.fd --leaks > "$OUT/leak.out" 2> "$OUT/leak.err"
nocr "$OUT/leak.err"
if grep -Eq "120 bytes in +3 block\(s\): make_buf <- main, allocated at [^ ]*leak_line\.fd:8$" "$OUT/leak.err"; then
    ok "leaks: line of the allocation"
else
    bad "leaks: line of the allocation" "$(grep -m2 'bytes in' "$OUT/leak.err" || head -c 300 "$OUT/leak.err")"
fi

# --- L7C: memory and allocation profile ------------------------------------
"$FID" run tests/memprof/mem_profile.fd --mem > "$OUT/mem.out" 2> "$OUT/mem.err"
nocr "$OUT/mem.out" "$OUT/mem.err"
if ! grep -q "^filled 5" "$OUT/mem.out"; then
    bad "mem: program output" "$(head -c 300 "$OUT/mem.out")"
fi
if grep -Eq "^fid --mem: [0-9]+ allocation\(s\), [0-9]+ bytes; [0-9]+ freed \([0-9]+ bytes\); peak [0-9]+ bytes live" "$OUT/mem.err"; then
    ok "mem: totals and peak"
else
    bad "mem: totals and peak" "$(head -c 300 "$OUT/mem.err")"
fi
for row in "512 bytes in +2 allocation\(s\): keep$" \
           "500 bytes in +5 allocation\(s\): fill$" \
           "500 bytes in +5 allocation\(s\): [^ ]*mem_profile\.fd:12$" \
           "512 bytes in +2 allocation\(s\): [^ ]*mem_profile\.fd:23$"; do
    if grep -Eq " $row" "$OUT/mem.err"; then
        ok "mem: row /$row/"
    else
        bad "mem: row /$row/" "not in the report"
    fi
done
# the peak: at least keep's two blocks and one of fill's at once
peak=$(sed -n 's/.*peak \([0-9]*\) bytes live.*/\1/p' "$OUT/mem.err" | head -1)
if [ -n "$peak" ] && [ "$peak" -ge 612 ]; then
    ok "mem: peak $peak >= 612"
else
    bad "mem: peak" "${peak:-none}"
fi

# --- CA9: the task chain of a panic -----------------------------------------
if "$FID" ccompile tests/bootstrap/test_task_chain_fail.fd -o "$OUT/chain$EXE" > "$OUT/chain.build" 2>&1; then
    "$OUT/chain$EXE" > "$OUT/chain.out" 2> "$OUT/chain.err"
    code=$?
    nocr "$OUT/chain.out" "$OUT/chain.err"
    if [ "$code" -ne 0 ] && grep -q "^task chain: b <- a <- main$" "$OUT/chain.err" && ! grep -q "after" "$OUT/chain.out"; then
        ok "tasks: chain b <- a <- main"
    else
        bad "tasks: chain" "exit $code; $(cat "$OUT/chain.err" | head -c 300)"
    fi
else
    bad "tasks: build" "$(tail -c 300 "$OUT/chain.build")"
fi

# --- L6C: run-time symbol lookup --------------------------------------------
if "$FID" ccompile tests/bootstrap/test_dl_find.fd -o "$OUT/dl$EXE" > "$OUT/dl.build" 2>&1; then
    "$OUT/dl$EXE" > "$OUT/dl.out" 2> "$OUT/dl.err"
    sed -n 's#^//     ##p' tests/bootstrap/test_dl_find.fd > "$OUT/dl.expected"
    if diff -q --strip-trailing-cr "$OUT/dl.expected" "$OUT/dl.out" > /dev/null; then
        ok "dl: load, find, call, missing symbol"
    else
        bad "dl: output" "$(diff --strip-trailing-cr "$OUT/dl.expected" "$OUT/dl.out" | head -c 300)"
    fi
else
    bad "dl: build" "$(tail -c 300 "$OUT/dl.build")"
fi

rm -rf "$OUT"
if [ "$fail" -gt 0 ]; then
    echo "$fail check(s) failed"
    exit 1
fi
echo "all checks passed"
