#!/bin/bash
# tests/limits/check.sh [FID]: roadmap P12, run-time resource limits of the
# C path (fid ccompile). The program reads them from the environment:
#
#   FID_MEM_LIMIT=<bytes>[K|M|G]  live heap bytes; past it: exit 125 and
#                                 "fid: memory limit of N bytes exceeded"
#   FID_CPU_LIMIT=<seconds>       CPU time; past it: exit 124 and
#                                 "fid: CPU time limit of S seconds exceeded"
#
# A program under both limits runs as without them. Needs a C compiler (CC,
# as for ccompile). Run from the project root.
#
#   bash tests/limits/check.sh                      # release/fid.exe
#   bash tests/limits/check.sh release/fid-dev.exe
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
nocr() { for f in "$@"; do tr -d '\015' < "$f" > "$f.t" && mv "$f.t" "$f"; done; }

for p in mem_hog cpu_spin small; do
    if ! "$FID" ccompile "tests/limits/$p.fd" -o "$OUT/$p$EXE" > "$OUT/$p.build" 2>&1; then
        bad "build $p" "$(tail -3 "$OUT/$p.build")"
    fi
done

# memory: 20000 strings of 1 KB do not fit in 8 MB
FID_MEM_LIMIT=8M "$OUT/mem_hog$EXE" > "$OUT/mem.out" 2> "$OUT/mem.err"
code=$?
nocr "$OUT/mem.err"
if [ $code -eq 125 ] && grep -q "^fid: memory limit of 8388608 bytes exceeded (FID_MEM_LIMIT)" "$OUT/mem.err"; then
    ok "memory limit stops the program (exit 125)"
else
    bad "memory limit" "exit $code: $(head -c 300 "$OUT/mem.err")"
fi
if grep -q "^kept" "$OUT/mem.out"; then
    bad "memory limit" "the program finished: $(cat "$OUT/mem.out")"
fi

# the same program without a limit finishes
"$OUT/mem_hog$EXE" > "$OUT/mem2.out" 2> "$OUT/mem2.err"
code=$?
nocr "$OUT/mem2.out"
if [ $code -eq 0 ] && grep -q "^kept 20000$" "$OUT/mem2.out"; then
    ok "no limit: the program finishes"
else
    bad "no limit" "exit $code: $(head -c 300 "$OUT/mem2.err")"
fi

# CPU time: a minute of work against a 1-second limit
FID_CPU_LIMIT=1 "$OUT/cpu_spin$EXE" > "$OUT/cpu.out" 2> "$OUT/cpu.err"
code=$?
nocr "$OUT/cpu.err"
if [ $code -eq 124 ] && grep -q "^fid: CPU time limit of 1 seconds exceeded (FID_CPU_LIMIT)" "$OUT/cpu.err"; then
    ok "CPU time limit stops the program (exit 124)"
else
    bad "CPU time limit" "exit $code: $(head -c 300 "$OUT/cpu.err")"
fi

# under both limits: normal output and exit code
FID_MEM_LIMIT=64M FID_CPU_LIMIT=30 "$OUT/small$EXE" > "$OUT/small.out" 2> "$OUT/small.err"
code=$?
nocr "$OUT/small.out"
if [ $code -eq 0 ] && grep -q "^small ok 0$" "$OUT/small.out"; then
    ok "under the limits: runs normally"
else
    bad "under the limits" "exit $code: $(head -c 300 "$OUT/small.err")"
fi

# a malformed limit is an error, not ignored
FID_MEM_LIMIT=lots "$OUT/small$EXE" > /dev/null 2> "$OUT/bad.err"
code=$?
if [ $code -eq 2 ] && grep -q "FID_MEM_LIMIT is not a size" "$OUT/bad.err"; then
    ok "malformed FID_MEM_LIMIT rejected (exit 2)"
else
    bad "malformed FID_MEM_LIMIT" "exit $code"
fi

# limits baked in when the program is built (FIDES_CFLAGS reaches the C
# compiler): the default when the variable is not set; a set one wins
if FIDES_CFLAGS="-DP_LIM_MEM_DEFAULT=8388608" "$FID" ccompile tests/limits/mem_hog.fd -o "$OUT/mem_baked$EXE" > "$OUT/baked.build" 2>&1; then
    "$OUT/mem_baked$EXE" > "$OUT/mb.out" 2> "$OUT/mb.err"
    code=$?
    nocr "$OUT/mb.err"
    if [ $code -eq 125 ] && grep -q "^fid: memory limit of 8388608 bytes exceeded" "$OUT/mb.err"; then
        ok "baked-in memory limit (-DP_LIM_MEM_DEFAULT)"
    else
        bad "baked-in memory limit" "exit $code: $(head -c 300 "$OUT/mb.err")"
    fi
    FID_MEM_LIMIT=256M "$OUT/mem_baked$EXE" > "$OUT/mb2.out" 2> /dev/null
    code=$?
    nocr "$OUT/mb2.out"
    if [ $code -eq 0 ] && grep -q "^kept 20000$" "$OUT/mb2.out"; then
        ok "FID_MEM_LIMIT overrides the baked-in limit"
    else
        bad "override of the baked-in limit" "exit $code"
    fi
else
    bad "build with -DP_LIM_MEM_DEFAULT" "$(tail -3 "$OUT/baked.build")"
fi
if FIDES_CFLAGS="-DP_LIM_CPU_DEFAULT_MS=1000" "$FID" ccompile tests/limits/cpu_spin.fd -o "$OUT/cpu_baked$EXE" > "$OUT/cbaked.build" 2>&1; then
    "$OUT/cpu_baked$EXE" > /dev/null 2> "$OUT/cb.err"
    code=$?
    nocr "$OUT/cb.err"
    if [ $code -eq 124 ] && grep -q "^fid: CPU time limit of 1 seconds exceeded" "$OUT/cb.err"; then
        ok "baked-in CPU limit (-DP_LIM_CPU_DEFAULT_MS)"
    else
        bad "baked-in CPU limit" "exit $code: $(head -c 300 "$OUT/cb.err")"
    fi
else
    bad "build with -DP_LIM_CPU_DEFAULT_MS" "$(tail -3 "$OUT/cbaked.build")"
fi
# a program built with -DP_NO_ENV_LIMITS (as fid itself should be) ignores them
if FIDES_CFLAGS="-DP_NO_ENV_LIMITS" "$FID" ccompile tests/limits/mem_hog.fd -o "$OUT/mem_free$EXE" > "$OUT/free.build" 2>&1; then
    FID_MEM_LIMIT=8M "$OUT/mem_free$EXE" > "$OUT/mf.out" 2> /dev/null
    code=$?
    nocr "$OUT/mf.out"
    if [ $code -eq 0 ] && grep -q "^kept 20000$" "$OUT/mf.out"; then
        ok "-DP_NO_ENV_LIMITS: the variables are ignored"
    else
        bad "-DP_NO_ENV_LIMITS" "exit $code"
    fi
else
    bad "build with -DP_NO_ENV_LIMITS" "$(tail -3 "$OUT/free.build")"
fi

rm -rf "$OUT"
if [ $fail -ne 0 ]; then
    echo "limits: $fail check(s) failed"
    exit 1
fi
echo "limits: all checks passed"
