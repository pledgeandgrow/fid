#!/bin/bash
# ark-selfbuild.sh -- AD1 (competitive-roadmap), the ark part: tools/ark.fd
# (compiler/ark.fd's ark_main) built with fid through C into a standalone
# ark executable, and every ark check of tests/ark/ run against it.
#
# Usage: tests/ark/ark-selfbuild.sh [fid]
#   default: release/fid.exe (else release/fid-dev.exe, release/fid, ...)
#   CC: the C compiler fid's ccompile uses (CC="zig cc" when none is on PATH)
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT="$(pwd)"
FID="${1:-}"
if [ -z "$FID" ]; then
  # prefer the newest working compiler: release/fid.exe can be weeks old
  # and predate the very features these scripts check (2g.63's lesson)
  for f in release/fid-dev.exe release/fid.exe release/fid-dev release/fid; do
    if [ -x "$f" ]; then FID="$ROOT/$f"; break; fi
  done
fi
if [ -z "$FID" ]; then echo "ark-selfbuild: no fid (give its path)"; exit 1; fi
case "$FID" in /*|[A-Za-z]:*) ;; *) FID="$ROOT/$FID" ;; esac

WORK="$(mktemp -d 2>/dev/null || echo "${TMPDIR:-/tmp}/ark-selfbuild.$$")"
mkdir -p "$WORK"
trap 'rm -rf "$WORK"' EXIT
EXE="$WORK/ark"
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) EXE="$WORK/ark.exe" ;; esac

echo "== ark-selfbuild ($FID)"
if ! "$FID" ccompile tools/ark.fd -o "$EXE" > "$WORK/build.log" 2>&1 || [ ! -x "$EXE" ]; then
  echo "  FAIL  fid ccompile tools/ark.fd"
  grep -v '^\[' "$WORK/build.log" | tail -20 | sed 's/^/        /'
  exit 1
fi
echo "  ok    fid ccompile tools/ark.fd -> $(basename "$EXE")"
out=$("$EXE" version 2>&1)
case "$out" in
  "ark "*) echo "  ok    ark version: ${out%%$'\n'*}" ;;
  *) echo "  FAIL  ark version: '$out'"; exit 1 ;;
esac

fail=0
for t in tests/ark/*.sh; do
  case "$t" in */ark-selfbuild.sh) continue ;; esac
  if bash "$t" "$EXE" > "$WORK/$(basename "$t").log" 2>&1; then
    echo "  ok    $(basename "$t")"
  else
    echo "  FAIL  $(basename "$t")"
    grep -F 'FAIL' "$WORK/$(basename "$t").log" | head -20 | sed 's/^/        /'
    fail=1
  fi
done
if [ $fail -ne 0 ]; then echo "ark-selfbuild: FAILED"; exit 1; fi
echo "ark-selfbuild: PASS"
