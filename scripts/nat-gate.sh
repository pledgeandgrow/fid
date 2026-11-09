#!/bin/bash
# nat-gate.sh — the gate of the own backend (docs/roadmap/llvm-roadmap.md
# RA16, ME4, ME16, ME17): every native target built with the IR verifier and
# the register-allocation checker on (FID_VERIFY=1), the rewrite rules on
# values (`fid test-rules`) and, when z3 is there, proved (rules-smt.sh).
# A build the verifier or the checker rejects fails the gate, as does a
# test whose output or exit code differs from the interpreter's.
#
# Usage: scripts/nat-gate.sh [targets...]
#   FID=<fid>        the compiler (default release/fid.exe, else release/fid)
#   ALL=1            the whole suite (default: the subset below)
#   JOBS=<n>         parallel builds (default 4)
# Default targets: x86_64-linux aarch64-linux riscv64-linux, and
# x86_64-windows (`emit --nat`) on a Windows host. Linux targets run under
# qemu-user (WSL on Windows: set WSL_DISTRO), as in cross-test.sh.
#
# CI: one job per host that runs `bash scripts/nat-gate.sh` after the build
# of fid; it exits non-zero on any failure.
set -u
cd "$(dirname "$0")/.." || exit 1
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
export FID
mkdir -p build
JOBS="${JOBS:-4}"
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) HOST=windows ;; *) HOST=linux ;; esac
TARGETS=("$@")
if [ ${#TARGETS[@]} -eq 0 ]; then
  TARGETS=(x86_64-linux aarch64-linux riscv64-linux)
  if [ "$HOST" = windows ]; then TARGETS+=(x86_64-windows); fi
fi
SUBSET=()
if [ "${ALL:-}" != 1 ]; then
  for t in test_int_semantics test_checked_arith test_closure test_generics test_hashmap \
           test_match test_float test_tasks test_threads test_fs test_str test_vec_builtins \
           test_str_return test_enum_payload_types test_recursive_box test_struct_reassign; do
    [ -f "tests/bootstrap/$t.fd" ] && SUBSET+=("tests/bootstrap/$t.fd")
  done
fi

bad=0
echo "== rules on values"
if ! "$FID" test-rules > build/nat-gate-rules.log 2>&1; then
  echo "FAIL fid test-rules (build/nat-gate-rules.log)"; bad=$((bad + 1))
else
  tail -n 1 build/nat-gate-rules.log
fi
echo "== rules proved (z3)"
bash scripts/rules-smt.sh "$FID"
rc=$?
if [ $rc = 2 ]; then echo "(z3 missing: not proved)"; elif [ $rc != 0 ]; then bad=$((bad + 1)); fi

for tg in "${TARGETS[@]}"; do
  echo "== $tg (FID_VERIFY=1)"
  out="build/nat-gate/$tg"
  FID_VERIFY=1 NATIVE=1 OUT="$out" JOBS="$JOBS" bash scripts/cross-test.sh "$tg" "${SUBSET[@]}" > "build/nat-gate-$tg.log" 2>&1
  tail -n 1 "build/nat-gate-$tg.log" | grep -a "passed," || tail -n 3 "build/nat-gate-$tg.log"
  # Results: anything but PASS (cross-test.sh's own list), and any build the
  # verifier or the checker stopped
  if grep -aq "^FAIL\|^BUILD" "$out-native/results.txt" 2>/dev/null; then
    grep -a "^FAIL\|^BUILD" "$out-native/results.txt"
    bad=$((bad + 1))
  fi
  if grep -alq "IR verifier\|allocation check failed" "$out-native"/*.build.log 2>/dev/null; then
    grep -al "IR verifier\|allocation check failed" "$out-native"/*.build.log
    bad=$((bad + 1))
  fi
done
if [ $bad = 0 ]; then echo "nat-gate: passed"; else echo "nat-gate: $bad failing parts"; fi
exit $bad
