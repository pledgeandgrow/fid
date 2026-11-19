#!/bin/bash
# check_c_ct.sh [FID]: LF18, @constant_time on the C path (ccompile, dump-c).
#
# compiler/codegen_c.fd cg_ct_check runs the IR-level check of
# pe_ir.fd's peir_ct_check on the IR the C generator is handed, and turns a
# violation into an `#error` line at the top of the generated C (and an
# "error: C backend: @constant_time violated: ..." line on stderr), so the
# C never compiles. Each rejected program here is accepted by the
# source-level check (TypeChecker_verify_constant_time); only the backend
# check rejects it. For each one this script checks that
#   1. with FID_C_CT_OFF=1, `fid dump-c` writes C with no #error, and
#   2. without it, the C starts with an #error naming the expected
#      violation (the "expected:" text in the program's header comment),
#   3. when a C compiler is found (CC, or zig cc / gcc / clang), `fid
#      ccompile` fails.
# The accepted programs (loops on public counters and lengths, secrets in
# memory, masks) must give C with no #error.
#
#   bash tests/ct/check_c_ct.sh                     # release/fid.exe
#   bash tests/ct/check_c_ct.sh release/fid-dev.exe
set -u
FID="${1:-release/fid.exe}"
export FIDES_NO_CACHE=1
OUT=$(mktemp -d)
fail=0

REJECT="scripts/pe-ct/ct_outparam_branch.fd scripts/pe-ct/ct_indirect_call.fd tests/ct/c_ct_outparam_div.fd"
ACCEPT="tests/bootstrap/test_windows_ct_public_loops.fd tests/bootstrap/test_constant_time.fd tests/bootstrap/test_ct_asm.fd tests/bootstrap/test_midend_ct_inline.fd"

have_cc=0
if [ -n "${CC:-}" ] || command -v zig > /dev/null 2>&1 || command -v gcc > /dev/null 2>&1 || command -v clang > /dev/null 2>&1; then
    have_cc=1
fi

for f in $REJECT; do
    name=$(basename "$f" .fd)
    want=$(sed -n 's/.*expected: "\(.*\)".*/\1/p' "$f" | head -1)
    if ! FID_C_CT_OFF=1 "$FID" dump-c "$f" > "$OUT/$name.off.c" 2> "$OUT/$name.off.log"; then
        echo "  FAIL  $name: dump-c fails with the C check off (rejected earlier: see $OUT/$name.off.log)"
        fail=$((fail + 1))
        continue
    fi
    if grep -q '^#error' "$OUT/$name.off.c"; then
        echo "  FAIL  $name: #error in the C with FID_C_CT_OFF=1"
        fail=$((fail + 1))
        continue
    fi
    "$FID" dump-c "$f" > "$OUT/$name.c" 2> "$OUT/$name.log"
    if ! grep -q '^#error "C backend: @constant_time violated' "$OUT/$name.c"; then
        echo "  FAIL  $name: no @constant_time #error in the C (see $OUT/$name.c, $OUT/$name.log)"
        fail=$((fail + 1))
        continue
    fi
    if ! grep '^#error' "$OUT/$name.c" | grep -q -- "$want"; then
        echo "  FAIL  $name: #error without the expected \"$want\" (see $OUT/$name.c)"
        fail=$((fail + 1))
        continue
    fi
    if [ $have_cc -eq 1 ]; then
        if "$FID" ccompile "$f" -o "$OUT/$name.exe" > "$OUT/$name.cc.log" 2>&1; then
            echo "  FAIL  $name: ccompile built it (see $OUT/$name.cc.log)"
            fail=$((fail + 1))
            continue
        fi
    fi
    echo "  ok    $name: $(grep -m1 '^#error' "$OUT/$name.c")"
done

for f in $ACCEPT; do
    name=$(basename "$f" .fd)
    if ! "$FID" dump-c "$f" > "$OUT/$name.c" 2> "$OUT/$name.log"; then
        echo "  FAIL  $name: dump-c failed (see $OUT/$name.log)"
        fail=$((fail + 1))
    elif grep -q '^#error' "$OUT/$name.c"; then
        echo "  FAIL  $name: rejected: $(grep -m1 '^#error' "$OUT/$name.c")"
        fail=$((fail + 1))
    else
        echo "  ok    $name: accepted"
    fi
done

echo "check_c_ct: $fail failed (logs in $OUT)"
[ $fail -eq 0 ]
