#!/usr/bin/env bash
# CE6 (complaint roadmap): one mistake, one error. Each cascade test in
# tests/bootstrap makes a single mistake and then uses its result many
# ways; `fid check` must print exactly one error for it. The expected-fail
# harness checks only that the first error is the right one, so the count
# is checked here.
#
# usage: tests/cascade/check.sh [fid]   (default: release/fid.exe, run from
# the repository root)
set -u
FID="${1:-release/fid.exe}"
fail=0
# An error line is `error: ...` or, with a code, `error[E044]: ...`.
ERR='^error(\[[A-Z0-9]+\])?:'
for t in tests/bootstrap/test_frontend_cascade_never_fail.fd \
         tests/bootstrap/test_frontend_cascade_call_fail.fd \
         tests/bootstrap/test_frontend_cascade_struct_fail.fd \
         tests/bootstrap/test_frontend_cascade_type_fail.fd; do
    out=$("$FID" check "$t" 2>&1)
    n=$(printf '%s\n' "$out" | grep -cE "$ERR")
    want=$(sed -n 's,^// expect-error: ,,p' "$t" | head -1)
    if [ "$n" = "1" ] && printf '%s\n' "$out" | grep -qF "$want"; then
        echo "ok    $t"
    else
        echo "FAIL  $t: $n errors, expected 1 ($want)"
        printf '%s\n' "$out" | grep -E "$ERR" | sed 's/^/      /'
        fail=1
    fi
done
exit $fail
