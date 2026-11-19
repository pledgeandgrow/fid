#!/bin/bash
# pe-ct-check.sh [FID]: release J7 on the default Windows backend (pe_ir).
#
# compiler/pe_ir.fd peir_ct_check rejects a @constant_time function whose
# code would branch or divide on a secret, or call through a function value.
# Each program in scripts/pe-ct/ is accepted by the source-level check and
# rejected only by the backend, so for each one this script checks that
#   1. with FID_PE_CT_OFF=1 `fid emit` builds it (the source check passes:
#      what rejects it below is the backend), and
#   2. without it, `fid emit` fails and names the violation it expects
#      (the "expected:" text in the program's header comment).
# It also checks that tests/bootstrap/test_windows_ct_public_loops.fd (loops
# on public counters and lengths, secrets in memory) still builds and prints
# its "Expected output" block.
#
#   ./scripts/pe-ct-check.sh                     # release/fid.exe
#   ./scripts/pe-ct-check.sh release/fid-dev.exe
# Windows host (pe_ir is the default `emit` there). EMIT_FLAGS adds flags.
set -u
FID="${1:-release/fid.exe}"
EMIT_FLAGS="${EMIT_FLAGS:-}"
# Never reuse a cached build: the two builds of a program differ only in
# FID_PE_CT_OFF, which the build cache does not see.
export FIDES_NO_CACHE=1
OUT=$(mktemp -d)
fail=0

for f in scripts/pe-ct/*.fd; do
    name=$(basename "$f" .fd)
    want=$(sed -n 's/.*expected: "\(.*\)".*/\1/p' "$f" | head -1)
    if ! FID_PE_CT_OFF=1 "$FID" emit "$f" $EMIT_FLAGS -o "$OUT/$name.off.exe" > "$OUT/$name.off.log" 2>&1; then
        echo "  FAIL  $name: does not build with the backend check off (rejected earlier: see $OUT/$name.off.log)"
        fail=$((fail + 1))
        continue
    fi
    if "$FID" emit "$f" $EMIT_FLAGS -o "$OUT/$name.exe" > "$OUT/$name.log" 2>&1; then
        echo "  FAIL  $name: built, but its @constant_time code ${want:-violates the rules}"
        fail=$((fail + 1))
        continue
    fi
    if ! grep -q "@constant_time violated" "$OUT/$name.log" || ! grep -q -- "$want" "$OUT/$name.log"; then
        echo "  FAIL  $name: rejected without the expected message \"$want\" (see $OUT/$name.log)"
        fail=$((fail + 1))
        continue
    fi
    echo "  ok    $name: $(grep -m1 '@constant_time violated' "$OUT/$name.log")"
done

pos=tests/bootstrap/test_windows_ct_public_loops.fd
name=$(basename "$pos" .fd)
sed -n '/^\/\/ Expected output:/,/^[^\/]/p' "$pos" | sed -n 's|^// ||p' | grep -v '^Expected output:' > "$OUT/$name.want"
if ! "$FID" emit "$pos" $EMIT_FLAGS -o "$OUT/$name.exe" > "$OUT/$name.log" 2>&1; then
    echo "  FAIL  $name: rejected (see $OUT/$name.log)"
    fail=$((fail + 1))
elif ! "$OUT/$name.exe" > "$OUT/$name.got" 2>&1 || ! diff -q --strip-trailing-cr "$OUT/$name.want" "$OUT/$name.got" > /dev/null; then
    echo "  FAIL  $name: output differs from its Expected output block (see $OUT/$name.got)"
    fail=$((fail + 1))
else
    echo "  ok    $name: builds and prints its expected output"
fi

echo "pe-ct-check: $fail failed (logs in $OUT)"
[ $fail -eq 0 ]
