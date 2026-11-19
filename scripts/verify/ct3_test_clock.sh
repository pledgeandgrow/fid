#!/usr/bin/env bash
# Complaint CT3 (stdlib half): with FIDES_TEST_NOW and FIDES_TEST_SEED set,
# dt_now() and random_seed_time() are fixed. Runs
# tests/bootstrap/test_stdlib_test_clock.fd with both variables set (it
# panics when either is ignored) and once without them.
#
#   FID=release/fid.exe bash scripts/verify/ct3_test_clock.sh
set -euo pipefail
FID="${FID:-release/fid.exe}"
TEST=tests/bootstrap/test_stdlib_test_clock.fd
want="now ok
seed ok
test clock ok"
got=$(FIDES_TEST_NOW=86400 FIDES_TEST_SEED=7 "$FID" run "$TEST" | tr -d '\r')
if [ "$got" != "$want" ]; then
    echo "FAIL with FIDES_TEST_NOW/FIDES_TEST_SEED set:"
    echo "$got"
    exit 1
fi
got=$(env -u FIDES_TEST_NOW -u FIDES_TEST_SEED "$FID" run "$TEST" | tr -d '\r')
if [ "$got" != "$want" ]; then
    echo "FAIL without them:"
    echo "$got"
    exit 1
fi
echo "ct3 test clock: ok (fixed and real clock)"
