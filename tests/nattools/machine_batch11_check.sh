#!/usr/bin/env bash
# Checks of the machine layer's batch 11 (docs/roadmap/llvm-roadmap.md):
#   DB2/DB4  frame bases, declaration lines, parameters and variables in
#            .debug_info (tests/bootstrap/test_machine_dbg_vars.fd,
#            tests/nattools/dbg_vars_check.py)
#   TG15     C imports through our own dynamic tables on every Linux target
#            (tests/bootstrap/test_machine_ldyn_targets.fd,
#            tests/nattools/ldyn_check.py)
# Usage: FID=release/fid.exe bash tests/nattools/machine_batch11_check.sh
# With RUN=1 also runs the executables (natively for x86-64, qemu-aarch64 /
# qemu-riscv64 for the others; the dynamic ones need the target's glibc,
# QEMU_LD_PREFIX) and compares their output with the test's expected lines.
set -u
FID="${FID:-release/fid.exe}"
OUT="$(mktemp -d)"
fail=0
expected() { sed -n '/^\/\/ Expected output:/,/^[^/]/p' "$1" | grep '^// ' | grep -v 'Expected output' | sed 's#^// ##'; }
for t in x86_64-linux aarch64-linux riscv64-linux; do
  "$FID" emit tests/bootstrap/test_machine_dbg_vars.fd --target "$t" -o "$OUT/dv-$t" > "$OUT/dv-$t.log" 2>&1 || { echo "FAIL: build dbg_vars $t"; tail -3 "$OUT/dv-$t.log"; fail=1; continue; }
  echo "== dbg_vars $t"
  python3 tests/nattools/dbg_vars_check.py "$OUT/dv-$t" tests/bootstrap/test_machine_dbg_vars.fd || fail=1
  "$FID" emit tests/bootstrap/test_machine_ldyn_targets.fd --target "$t" -o "$OUT/ld-$t" > "$OUT/ld-$t.log" 2>&1 || { echo "FAIL: build ldyn $t"; tail -3 "$OUT/ld-$t.log"; fail=1; continue; }
  echo "== ldyn $t"
  python3 tests/nattools/ldyn_check.py "$OUT/ld-$t" atoi hypot ldexp || fail=1
  if [ "${RUN:-}" = 1 ]; then
    runner=""
    case "$t" in aarch64-linux) runner=qemu-aarch64 ;; riscv64-linux) runner=qemu-riscv64 ;; esac
    for p in dv ld; do
      src=tests/bootstrap/test_machine_dbg_vars.fd
      [ "$p" = ld ] && src=tests/bootstrap/test_machine_ldyn_targets.fd
      $runner "$OUT/$p-$t" > "$OUT/$p-$t.out" 2>&1
      if ! diff <(expected "$src") "$OUT/$p-$t.out" > /dev/null; then
        echo "FAIL: $p $t output differs"; diff <(expected "$src") "$OUT/$p-$t.out" | head -5; fail=1
      fi
    done
  fi
done
rm -rf "$OUT"
[ $fail = 0 ] && echo "batch 11 checks OK"
exit $fail
