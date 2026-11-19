#!/bin/bash
# rl15-sanitize-compiler-check.sh — RL15 (llvm-roadmap: the compiler itself
# under ASan/UBSan through the C path). Checks the tooling, one line each
# (OK / FAIL / SKIP); the exit status is the number of FAILs:
#
#  1. scripts/verify/sanitize-compiler-wsl.sh and scripts/sanitize.sh parse
#     (bash -n), and sanitize.sh still has its COMPILER=1 and SELF=1 paths.
#  2. On Windows with WSL and a C compiler in it: the WSL script's SELFTEST
#     (a stand-in compiler built plain and with -fsanitize=address,undefined
#     inside WSL; a clean file passes check/dump-c/run, a heap overrun and a
#     division by zero in `run` are both reported as sanitizer findings, a
#     SELF=1 step passes). This needs no fid.
#  3. FID=<compiler> given: the real thing on two small tests
#     (hello, test_closure) with STEPS="check dump-c" and SPLIT=8: fid's C
#     is built with the sanitizers in WSL and must stay quiet. Slow (the
#     sanitized build of fid's C is several minutes).
#
# Usage: tests/tooling/rl15-sanitize-compiler-check.sh   (FID=..., OUT=<dir>)
set -u
cd "$(dirname "$0")/../.." || exit 1
OUT="${OUT:-build/rl15-check}"
mkdir -p "$OUT"
bad=0
ok()   { echo "OK   $*"; }
fail() { echo "FAIL $*"; bad=$((bad + 1)); }
skip() { echo "SKIP $*"; }

# 1
if bash -n scripts/verify/sanitize-compiler-wsl.sh && bash -n scripts/sanitize.sh &&
   grep -q 'COMPILER:-' scripts/sanitize.sh && grep -q 'SELF:-' scripts/sanitize.sh; then
    ok "1 scripts parse; sanitize.sh has COMPILER=1 and SELF=1"
else
    fail "1 scripts/verify/sanitize-compiler-wsl.sh or scripts/sanitize.sh"
fi

# 2
if ! command -v wsl.exe > /dev/null 2>&1; then
    skip "2 no WSL (on Linux: COMPILER=1 CC=clang scripts/sanitize.sh is the check)"
elif ! MSYS_NO_PATHCONV=1 wsl.exe -- sh -c 'command -v gcc || command -v clang' > /dev/null 2>&1; then
    skip "2 no gcc or clang inside WSL"
else
    if OUT="$OUT/selftest" SELF=1 SELFTEST=1 bash scripts/verify/sanitize-compiler-wsl.sh > "$OUT/selftest.log" 2>&1; then
        ok "2 selftest: overrun and division by zero reported, clean steps pass ($OUT/selftest.log)"
    else
        fail "2 selftest ($OUT/selftest.log)"
        tail -5 "$OUT/selftest.log" | sed 's/^/     /'
    fi
fi

# 3
if [ -z "${FID:-}" ]; then
    skip "3 FID not set (the sanitized compiler on hello and test_closure)"
elif ! command -v wsl.exe > /dev/null 2>&1; then
    skip "3 no WSL"
else
    if OUT="$OUT/real" STEPS="check dump-c" bash scripts/verify/sanitize-compiler-wsl.sh hello test_closure > "$OUT/real.log" 2>&1; then
        ok "3 fid built with ASan+UBSan: check and dump-c of hello, test_closure clean and equal to fid-plain"
    else
        fail "3 the sanitized compiler ($OUT/real.log)"
        tail -8 "$OUT/real.log" | sed 's/^/     /'
    fi
fi
exit $bad
