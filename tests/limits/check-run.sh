#!/bin/bash
# tests/limits/check-run.sh [FID]: roadmap P12, the run-time resource limits
# under `fid run` (the interpreter, compiler/interp.fd "Run-time resource
# limits"), the same variables, messages and exit codes as tests/limits/
# check.sh checks for the C path:
#
#   FID_MEM_LIMIT=<bytes>[K|M|G]  live heap bytes of the interpreted program;
#                                 past it: exit 125 and "fid: memory limit of
#                                 N bytes exceeded (FID_MEM_LIMIT)"
#   FID_CPU_LIMIT=<seconds>       CPU time of the interpreted program; past
#                                 it: exit 124 and "fid: CPU time limit of S
#                                 seconds exceeded (FID_CPU_LIMIT)"
#   fid run --max-mem / --cpu-limit  the limits when the variable is not set
#
# fid itself must ignore the variables (built with -DP_NO_ENV_LIMITS, as
# scripts/bootstrap.sh and build-selfhost.sh build it): the first check.
# No C compiler needed. Run from the project root.
#
#   bash tests/limits/check-run.sh                      # release/fid.exe
#   bash tests/limits/check-run.sh release/fid-dev.exe
#   LIMIT_RUN_FULL=1 bash tests/limits/check-run.sh  # + FID_MEM_LIMIT=20M on
#                                 mem_hog and mem_hog unlimited (minutes)
set -u
FID="${1:-release/fid.exe}"
OUT=$(mktemp -d)
# a run that goes wrong ends here, not at the spinner's minute (or more)
T="${LIMIT_RUN_TIMEOUT:-240}"
fail=0
ok()   { echo "  ok    $1"; }
bad()  { echo "  FAIL  $1: $2"; fail=$((fail + 1)); }
nocr() { for f in "$@"; do tr -d '\015' < "$f" > "$f.t" && mv "$f.t" "$f"; done; }
now()  { date +%s; }

# fid's own runtime ignores the variables: compiling is not limited
FID_MEM_LIMIT=1K FID_CPU_LIMIT=0.001 timeout "$T" "$FID" check tests/limits/small.fd > "$OUT/self.out" 2>&1
code=$?
if [ $code -eq 0 ]; then
    ok "fid itself ignores FID_MEM_LIMIT / FID_CPU_LIMIT"
else
    bad "fid itself is limited (build it with -DP_NO_ENV_LIMITS)" "exit $code: $(head -c 300 "$OUT/self.out")"
fi

# memory: mem_hog keeps 1 KB strings (1536 bytes each as a compiled program
# counts them): 256 KB holds about 170 of its 20000
FID_MEM_LIMIT=256K timeout "$T" "$FID" run tests/limits/mem_hog.fd > "$OUT/mem.out" 2> "$OUT/mem.err"
code=$?
nocr "$OUT/mem.out" "$OUT/mem.err"
if [ $code -eq 125 ] && grep -q "^fid: memory limit of 262144 bytes exceeded (FID_MEM_LIMIT): [0-9]* bytes in use, [0-9]* more asked for" "$OUT/mem.err"; then
    ok "FID_MEM_LIMIT=256K stops mem_hog (exit 125)"
else
    bad "FID_MEM_LIMIT=256K" "exit $code: $(head -c 300 "$OUT/mem.err")"
fi
if grep -q "^kept" "$OUT/mem.out"; then
    bad "FID_MEM_LIMIT=256K" "the program finished: $(cat "$OUT/mem.out")"
fi
if grep -q "main" "$OUT/mem.err"; then
    ok "the memory-limit stop prints the interpreted stack"
else
    bad "memory-limit stack trace" "$(head -c 300 "$OUT/mem.err")"
fi

# LIMIT_RUN_FULL=1: the roadmap's case, FID_MEM_LIMIT=20M on mem_hog (it
# stops at about the 13,600th string: minutes, as the interpreter takes them
# to build that many), and mem_hog without a limit (all 20000)
if [ "${LIMIT_RUN_FULL:-0}" = 1 ]; then
    t0=$(now)
    FID_MEM_LIMIT=20M timeout 1800 "$FID" run tests/limits/mem_hog.fd > "$OUT/m20.out" 2> "$OUT/m20.err"
    code=$?
    t1=$(now)
    nocr "$OUT/m20.out" "$OUT/m20.err"
    if [ $code -eq 125 ] && grep -q "^fid: memory limit of 20971520 bytes exceeded (FID_MEM_LIMIT)" "$OUT/m20.err" && ! grep -q "^kept" "$OUT/m20.out"; then
        ok "FID_MEM_LIMIT=20M stops mem_hog (exit 125, $((t1 - t0)) s)"
    else
        bad "FID_MEM_LIMIT=20M" "exit $code after $((t1 - t0)) s: $(head -c 300 "$OUT/m20.err")"
    fi
    timeout 1800 "$FID" run tests/limits/mem_hog.fd > "$OUT/mem2.out" 2> "$OUT/mem2.err"
    code=$?
    nocr "$OUT/mem2.out"
    if [ $code -eq 0 ] && grep -q "^kept 20000$" "$OUT/mem2.out"; then
        ok "no limit: mem_hog finishes"
    else
        bad "no limit" "exit $code: $(head -c 300 "$OUT/mem2.err")"
    fi
fi

# freed blocks come off the count: 1.6 MB allocated in all, 32 KB live
FID_MEM_LIMIT=1M timeout "$T" "$FID" run tests/limits/mem_churn.fd > "$OUT/churn.out" 2> "$OUT/churn.err"
code=$?
nocr "$OUT/churn.out"
if [ $code -eq 0 ] && grep -q "^churn 100000$" "$OUT/churn.out"; then
    ok "FID_MEM_LIMIT=1M: allocating and freeing in a loop runs"
else
    bad "FID_MEM_LIMIT=1M on mem_churn" "exit $code: $(head -c 300 "$OUT/churn.err")"
fi
# ... and its Vec's buffers are counted: its 16 KB buffer does not fit in 16 KB
FID_MEM_LIMIT=16K timeout "$T" "$FID" run tests/limits/mem_churn.fd > "$OUT/churn2.out" 2> "$OUT/churn2.err"
code=$?
nocr "$OUT/churn2.err"
if [ $code -eq 125 ] && grep -q "^fid: memory limit of 16384 bytes exceeded (FID_MEM_LIMIT)" "$OUT/churn2.err"; then
    ok "FID_MEM_LIMIT=16K: a Vec's buffer counts"
else
    bad "FID_MEM_LIMIT=16K on mem_churn" "exit $code: $(head -c 300 "$OUT/churn2.err")"
fi

# CPU time: a minute of work (much more interpreted) against 2 seconds;
# compiling the program is not counted
t0=$(now)
FID_CPU_LIMIT=2 timeout "$T" "$FID" run tests/limits/cpu_spin.fd > "$OUT/cpu.out" 2> "$OUT/cpu.err"
code=$?
t1=$(now)
nocr "$OUT/cpu.err"
if [ $code -eq 124 ] && grep -q "^fid: CPU time limit of 2 seconds exceeded (FID_CPU_LIMIT)" "$OUT/cpu.err"; then
    ok "FID_CPU_LIMIT=2 stops cpu_spin (exit 124, $((t1 - t0)) s)"
else
    bad "FID_CPU_LIMIT=2" "exit $code after $((t1 - t0)) s: $(head -c 300 "$OUT/cpu.err")"
fi

# under both limits: normal output and exit code
FID_MEM_LIMIT=64M FID_CPU_LIMIT=30 timeout "$T" "$FID" run tests/limits/small.fd > "$OUT/small.out" 2> "$OUT/small.err"
code=$?
nocr "$OUT/small.out"
if [ $code -eq 0 ] && grep -q "^small ok 0$" "$OUT/small.out"; then
    ok "under the limits: runs normally"
else
    bad "under the limits" "exit $code: $(head -c 300 "$OUT/small.err")"
fi

# a malformed limit is an error (exit 2), not ignored
FID_MEM_LIMIT=lots timeout "$T" "$FID" run tests/limits/small.fd > "$OUT/b1.out" 2> "$OUT/b1.err"
code=$?
nocr "$OUT/b1.err"
if [ $code -eq 2 ] && grep -q "^fid: FID_MEM_LIMIT is not a size (bytes, or with K, M or G): 'lots'" "$OUT/b1.err" && ! grep -q "small ok" "$OUT/b1.out"; then
    ok "malformed FID_MEM_LIMIT rejected (exit 2)"
else
    bad "malformed FID_MEM_LIMIT" "exit $code: $(head -c 300 "$OUT/b1.err")"
fi
FID_CPU_LIMIT=2x timeout "$T" "$FID" run tests/limits/small.fd > "$OUT/b2.out" 2> "$OUT/b2.err"
code=$?
nocr "$OUT/b2.err"
if [ $code -eq 2 ] && grep -q "^fid: FID_CPU_LIMIT is not a number of seconds: '2x'" "$OUT/b2.err" && ! grep -q "small ok" "$OUT/b2.out"; then
    ok "malformed FID_CPU_LIMIT rejected (exit 2)"
else
    bad "malformed FID_CPU_LIMIT" "exit $code: $(head -c 300 "$OUT/b2.err")"
fi
# empty or 0: no limit
FID_MEM_LIMIT= FID_CPU_LIMIT=0 timeout "$T" "$FID" run tests/limits/small.fd > "$OUT/z.out" 2> "$OUT/z.err"
code=$?
nocr "$OUT/z.out"
if [ $code -eq 0 ] && grep -q "^small ok 0$" "$OUT/z.out"; then
    ok "empty / 0: no limit"
else
    bad "empty / 0" "exit $code: $(head -c 300 "$OUT/z.err")"
fi

# --max-mem / --cpu-limit: the limits when the variable is not set (as
# ccompile's -DP_LIM_*_DEFAULT); a set variable wins
env -u FID_MEM_LIMIT -u FID_CPU_LIMIT timeout "$T" "$FID" run tests/limits/mem_hog.fd --max-mem 256K > "$OUT/f1.out" 2> "$OUT/f1.err"
code=$?
nocr "$OUT/f1.err"
if [ $code -eq 125 ] && grep -q "^fid: memory limit of 262144 bytes exceeded (FID_MEM_LIMIT)" "$OUT/f1.err"; then
    ok "fid run --max-mem 256K"
else
    bad "fid run --max-mem 256K" "exit $code: $(head -c 300 "$OUT/f1.err")"
fi
# small.fd holds about 50 KB at its peak: stopped at 16 KB, not at 1 MB
env -u FID_MEM_LIMIT timeout "$T" "$FID" run tests/limits/small.fd --max-mem 16K > "$OUT/f2.out" 2> "$OUT/f2.err"
code=$?
nocr "$OUT/f2.err"
if [ $code -eq 125 ] && grep -q "^fid: memory limit of 16384 bytes exceeded" "$OUT/f2.err"; then
    ok "fid run --max-mem 16K stops small"
else
    bad "fid run --max-mem 16K" "exit $code: $(head -c 300 "$OUT/f2.err")"
fi
FID_MEM_LIMIT=1M timeout "$T" "$FID" run tests/limits/small.fd --max-mem 16K > "$OUT/f2b.out" 2> "$OUT/f2b.err"
code=$?
nocr "$OUT/f2b.out"
if [ $code -eq 0 ] && grep -q "^small ok 0$" "$OUT/f2b.out"; then
    ok "FID_MEM_LIMIT overrides --max-mem"
else
    bad "FID_MEM_LIMIT over --max-mem" "exit $code: $(head -c 300 "$OUT/f2b.err")"
fi
env -u FID_CPU_LIMIT timeout "$T" "$FID" run tests/limits/cpu_spin.fd --cpu-limit 0.5 > "$OUT/f3.out" 2> "$OUT/f3.err"
code=$?
nocr "$OUT/f3.err"
if [ $code -eq 124 ] && grep -q "^fid: CPU time limit of 0.5 seconds exceeded (FID_CPU_LIMIT)" "$OUT/f3.err"; then
    ok "fid run --cpu-limit 0.5"
else
    bad "fid run --cpu-limit 0.5" "exit $code: $(head -c 300 "$OUT/f3.err")"
fi
timeout "$T" "$FID" run tests/limits/small.fd --max-mem lots > "$OUT/f4.out" 2>&1
code=$?
if [ $code -ne 0 ] && grep -q "Invalid --max-mem 'lots'" "$OUT/f4.out"; then
    ok "fid run --max-mem lots rejected"
else
    bad "fid run --max-mem lots" "exit $code: $(head -c 300 "$OUT/f4.out")"
fi

rm -rf "$OUT"
if [ $fail -ne 0 ]; then
    echo "limits (fid run): $fail check(s) failed"
    exit 1
fi
echo "limits (fid run): all checks passed"
