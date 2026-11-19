#!/usr/bin/env bash
# irfuzz-mirror-check.sh — RL10 (docs/roadmap/llvm-roadmap.md): the values
# `fid ir-fuzz` expects are the language's own.
#
#   tests/tooling/irfuzz-mirror-check.sh [first_seed] [count]
#     FID=<fid>   the compiler (default release/fid.exe, else release/fid)
#
# scripts/fuzz/fuzz_ir.sh trusts compiler/ir_ser.fd irf_eval_unit (the
# value of the random fz_target / fz_h<k> of a seed, worked out from its
# description). This check runs tests/tooling/irfuzz_mirror_gen.fd, which
# prints each seed's functions as source (irfuzz_unit_source: the same
# description, fz_target's heap buffer as a Vec<i64>) and the values the
# evaluator expects; it renames each seed's functions apart (s<n>_fz_...),
# runs that program with `fid run` and compares line by line. A
# difference is a bug in the evaluator or in the description's meaning
# (f64 rounding, truncation, shifts, wrapping), not in the backend.
# Exit 1 on any difference.
set -u
cd "$(dirname "$0")/../.." || exit 1
FIRST="${1:-1}"
COUNT="${2:-40}"
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
T="build/irfuzz-mirror"
mkdir -p "$T"
if ! IRFUZZ_FIRST="$FIRST" IRFUZZ_COUNT="$COUNT" "$FID" run tests/tooling/irfuzz_mirror_gen.fd > "$T/gen.raw" 2>&1; then
  echo "irfuzz-mirror-check: FAIL: the generator did not run (see $T/gen.raw)"
  exit 1
fi
tr -d '\r' < "$T/gen.raw" | grep -v '^\[' > "$T/gen.out"
awk '/^\/\/SEED /{p="s"$2"_"; next} /^\/\/MAIN/{p=""; next} /^\/\/EXPECT /{next} {if (p != "") gsub(/fz_/, p "fz_"); print}' "$T/gen.out" > "$T/mirror.fd"
grep '^//EXPECT ' "$T/gen.out" | sed 's#^//EXPECT ##' > "$T/want.txt"
want_n=$(wc -l < "$T/want.txt" | tr -d ' ')
if [ "$want_n" -eq 0 ]; then
  echo "irfuzz-mirror-check: FAIL: no expected values (see $T/gen.out)"
  exit 1
fi
if ! "$FID" run "$T/mirror.fd" > "$T/got.raw" 2>&1; then
  echo "irfuzz-mirror-check: FAIL: the mirror program did not run (see $T/got.raw, $T/mirror.fd)"
  exit 1
fi
tr -d '\r' < "$T/got.raw" | grep -v '^\[' > "$T/got.txt"
if diff "$T/want.txt" "$T/got.txt" > "$T/diff.txt"; then
  echo "irfuzz-mirror-check: PASS ($want_n values, seeds $FIRST..$((FIRST + COUNT - 1)))"
  exit 0
fi
echo "irfuzz-mirror-check: FAIL: the evaluator and the language differ (lines: want, got):"
head -20 "$T/diff.txt"
echo "  (each seed gives $(( $(grep -c '^//SEED ' "$T/gen.out") > 0 ? want_n / $(grep -c '^//SEED ' "$T/gen.out") : 0 )) lines: fz_h0, fz_h1, fz_target; program in $T/mirror.fd)"
exit 1
