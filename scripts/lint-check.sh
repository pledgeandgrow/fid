#!/bin/bash
# lint-check.sh — release-roadmap N5: `fid lint` on every stdlib module.
#
# Each module stdlib/<m>.fd ends in one of three states:
#   clean      lint printed "No lint warnings."
#   warnings   lint printed warnings (unused private functions, unused
#              imports, unwrap()/expect() outside tests): the count
#   unchecked  lint could not check it: lint resolves the module's `use`
#              imports (lint_with_imports), so this means the module does
#              not pass `fid check` (a type or name error of its own)
# The gate compares with scripts/lint-check.baseline (one `module|state|count`
# line each): a module may not get more warnings than the baseline records,
# nor move from clean to warnings or unchecked. Without a baseline the gate
# is the roadmap's: every module clean or unchecked. `--save` records the
# current state as the baseline (do it only after fixing, or to accept the
# state a release starts from).
#
# Usage:
#   scripts/lint-check.sh [--save] [fid]
#   FID=<fid>   the compiler (default: release/fid-dev, else release/fid.exe,
#               else release/fid)
#   VERBOSE=1   print every warning
set -u
cd "$(dirname "$0")/.." || exit 1
SAVE=0
if [ "${1:-}" = "--save" ]; then SAVE=1; shift; fi
FID="${1:-${FID:-}}"
if [ -z "$FID" ]; then
  for f in release/fid-dev release/fid-dev.exe release/fid.exe release/fid; do
    if [ -x "$f" ]; then FID="$f"; break; fi
  done
fi
if [ -z "$FID" ]; then echo "lint-check: no compiler (give its path)"; exit 1; fi
BASE=scripts/lint-check.baseline
WORK=build/lint-check
mkdir -p "$WORK"
: > "$WORK/now.txt"
n_clean=0; n_warn=0; n_unchecked=0
for f in stdlib/*.fd; do
  m=$(basename "$f" .fd)
  "$FID" lint "$f" > "$WORK/$m.log" 2>&1
  code=$?
  tr -d '\r' < "$WORK/$m.log" > "$WORK/$m.txt"
  if grep -q "^No lint warnings" "$WORK/$m.txt"; then
    echo "$m|clean|0" >> "$WORK/now.txt"; n_clean=$((n_clean + 1))
  elif [ "$code" = 0 ] && grep -q "lint warning(s)" "$WORK/$m.txt"; then
    w=$(grep -c "^warning: " "$WORK/$m.txt")
    echo "$m|warnings|$w" >> "$WORK/now.txt"; n_warn=$((n_warn + 1))
    if [ -n "${VERBOSE:-}" ]; then grep "^warning: " "$WORK/$m.txt" | sed "s/^/  $m: /"; fi
  else
    echo "$m|unchecked|0" >> "$WORK/now.txt"; n_unchecked=$((n_unchecked + 1))
  fi
done
echo "lint-check: $n_clean clean, $n_warn with warnings, $n_unchecked unchecked (they do not pass fid check) of $((n_clean + n_warn + n_unchecked)) stdlib modules"

if [ "$SAVE" = 1 ]; then
  cp "$WORK/now.txt" "$BASE"
  echo "lint-check: baseline saved to $BASE"
  exit 0
fi

fail=0
while IFS='|' read -r m state count; do
  if [ -f "$BASE" ]; then
    line=$(grep "^$m|" "$BASE" | head -1)
    if [ -z "$line" ]; then
      # a new module: it must be clean (or uncheckable)
      if [ "$state" = warnings ]; then echo "  FAIL  $m (new module): $count lint warning(s)"; fail=1; fi
      continue
    fi
    bstate=$(echo "$line" | cut -d'|' -f2)
    bcount=$(echo "$line" | cut -d'|' -f3)
    if [ "$bstate" = clean ] && [ "$state" != clean ]; then
      echo "  FAIL  $m: was clean, now $state ($count)"; fail=1
    elif [ "$state" = warnings ] && [ "$count" -gt "${bcount:-0}" ]; then
      echo "  FAIL  $m: $count lint warning(s), the baseline allows $bcount"; fail=1
    fi
  elif [ "$state" = warnings ]; then
    echo "  FAIL  $m: $count lint warning(s)"; fail=1
  fi
done < "$WORK/now.txt"
if [ "$fail" != 0 ]; then
  echo "  (the warnings: build/lint-check/<module>.txt, or VERBOSE=1)"
  echo "lint-check: FAIL"
  exit 1
fi
echo "lint-check: PASS"
exit 0
