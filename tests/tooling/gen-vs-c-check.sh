#!/bin/bash
# gen-vs-c-check.sh — RL1 (llvm roadmap): differential testing against
# LLVM (clang through `zig cc`) and GCC with scripts/fuzz/gen_vs_c.py. Checks
# the harness itself; the release run is the long one below (validate).
#
# Usage: tests/tooling/gen-vs-c-check.sh
#   FID=<compiler>  default release/fid.exe (release/fid off Windows)
#   OUT=<dir>       default build/gen-vs-c-check
#   N=<programs>    for check 3, default 3
#
# Checks, one line each (PASS/FAIL), exit status the number of FAILs:
#  1. a seed gives the same pair of programs every time (`--print`), and the
#     pair has a Fides `fn main` and a C `int main`; --ext gives another
#     program for the same seed, the plain one is unchanged by it
#  2. GEN_VS_C_MUTATE=run (a made-up miscompile of the interpreter): every
#     seed is reported as differing on `run` and the exit status is 1, so a
#     real difference cannot pass unnoticed
#  3. N random programs on run, ccompile and emit against clang -O0/-O2:
#     the run completes and every C build agrees with the others. A backend
#     that differs from C is a compiler bug, listed on the PASS line to be
#     triaged as in docs/verification/miscompile-log.md (the program is kept
#     in $OUT/run); STRICT=1 fails on it
#
# The release run (VF1/VF2, every backend this host has, Linux targets in
# WSL or qemu-user):
#   python scripts/fuzz/gen_vs_c.py -n 100 --seed 1 --fid "$FID" --backends all
#   python scripts/fuzz/gen_vs_c.py -n 100 --seed 1000 --fid "$FID" --backends all --ext
set -u
cd "$(dirname "$0")/../.." || exit 1
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) FID="${FID:-release/fid.exe}" ;; *) FID="${FID:-release/fid}" ;; esac
OUT="${OUT:-build/gen-vs-c-check}"
N="${N:-3}"
mkdir -p "$OUT"
PY=python3
"$PY" -c 1 > /dev/null 2>&1 || PY=python
export PYTHONIOENCODING=utf-8
G=scripts/fuzz/gen_vs_c.py
bad=0
pass() { echo "PASS $1"; }
fail() { echo "FAIL $1"; bad=$((bad + 1)); }

# 1
"$PY" "$G" --print 7 > "$OUT/p7a.txt" 2>&1
"$PY" "$G" --print 7 > "$OUT/p7b.txt" 2>&1
"$PY" "$G" --print 7 --ext > "$OUT/p7x.txt" 2>&1
if cmp -s "$OUT/p7a.txt" "$OUT/p7b.txt" && grep -q '^fn main' "$OUT/p7a.txt" && grep -q 'int main' "$OUT/p7a.txt"; then
  pass "seed 7 gives the same Fides/C pair twice"
else
  fail "seed 7: the pairs differ or lack main (see $OUT/p7a.txt, $OUT/p7b.txt)"
fi
if ! cmp -s "$OUT/p7a.txt" "$OUT/p7x.txt"; then
  pass "--ext changes the program of a seed"
else
  fail "--ext gives the plain program"
fi

# 2
GEN_VS_C_MUTATE=run "$PY" "$G" -n 2 --seed 11 --fid "$FID" --backends run --cc clang \
  --keep "$OUT/mut" > "$OUT/mut.log" 2>&1
rc=$?
nd=$(grep -c 'differs from C on run' "$OUT/mut.log")
if [ "$rc" = 1 ] && [ "$nd" = 2 ]; then
  pass "a made-up miscompile of run is reported on both seeds (exit 1)"
else
  fail "a made-up miscompile: exit $rc, $nd of 2 seeds reported (see $OUT/mut.log)"
fi

# 3
"$PY" "$G" -n "$N" --seed 101 --fid "$FID" --backends run,ccompile,emit --cc clang \
  --keep "$OUT/run" > "$OUT/run.log" 2>&1
rc=$?
summary=$(grep -E "^$N programs: [0-9]+ differ from C, [0-9]+ with a C oracle" "$OUT/run.log")
if [ "$rc" = 0 ] && [ -n "$summary" ]; then
  pass "$N random programs agree with clang on run, ccompile and emit"
elif [ -n "$summary" ] && echo "$summary" | grep -q ", 0 with a C oracle" && [ "${STRICT:-0}" != 1 ]; then
  # The harness worked; what differs is a compiler bug to triage (or a known
  # one: M7 on emit; `run` with a stdlib newer than the compiler). STRICT=1
  # makes it a failure.
  pass "$N random programs ran; differences to triage: $(grep 'differs from C' "$OUT/run.log" | tr '\n' ';')"
else
  fail "random programs: exit $rc; $(tail -n 1 "$OUT/run.log") (see $OUT/run.log, programs in $OUT/run)"
fi
exit $bad
