#!/usr/bin/env bash
# str_to_float_exact_check.sh — the Linux runtime's pert_str_to_float is
# correctly rounded (BG206, docs/roadmap/bugs-roadmap.md): builds
# tests/nattools/str_to_float_exact.fd with `fid emit --nat` and compares its
# output with tests/nattools/str_to_float_exact.expected (Python's float()).
#
#   FID=release/fid.exe bash tests/nattools/str_to_float_exact_check.sh
#   OUT=<dir>   default build/str-to-float-exact
# Exit status 1 on a difference.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
OUT="${OUT:-build/str-to-float-exact}"
mkdir -p "$OUT"
"$FID" emit tests/nattools/str_to_float_exact.fd --nat -o "$OUT/str_to_float_exact.exe" > "$OUT/build.log" 2>&1
if [ ! -f "$OUT/str_to_float_exact.exe" ]; then
  echo "str_to_float_exact_check: does not build (see $OUT/build.log)"
  exit 1
fi
"$OUT/str_to_float_exact.exe" > "$OUT/actual.txt" 2>&1
rc=$?
if [ $rc -ne 0 ]; then
  echo "str_to_float_exact_check: exit $rc"
  tail -5 "$OUT/actual.txt"
  exit 1
fi
if cmp -s <(tr -d '\r' < "$OUT/actual.txt") tests/nattools/str_to_float_exact.expected; then
  echo "$(head -1 "$OUT/actual.txt"); $(($(wc -l < tests/nattools/str_to_float_exact.expected) - 1)) texts: all as Python's float()"
  echo "str_to_float_exact_check: all passed"
  exit 0
fi
echo "str_to_float_exact_check: differences (got, want):"
diff <(tr -d '\r' < "$OUT/actual.txt") tests/nattools/str_to_float_exact.expected | head -20
exit 1
