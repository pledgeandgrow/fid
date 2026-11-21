#!/usr/bin/env bash
# lg15-extern-core-name-check.sh — an extern C function named like a core
# function does not stop core's C rename (known-bugs 2g.107, CI1/LG15).
#
# tests/bootstrap/at_import/test_at_import_system.fd imports stdlib.h, whose
# `abs` became an extern beside core's `pub fn abs`; codegen_c's
# sanitize_c_name then skipped the abs -> p_abs rename and emitted core's
# abs as `int64_t abs(void*, int64_t)`: zig cc said "conflicting types for
# 'abs'". Now `fid ccompile` builds it and the program prints 5, as `emit`.
# On Windows the headers are zig's any-windows-any (FID_INCLUDE); elsewhere
# /usr/include. Skips (exit 0) without zig or without a stdlib.h.
#
# Usage: tests/tooling/lg15-extern-core-name-check.sh   (FID=<fid> to choose)
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT=$(pwd)
FID="${FID:-$ROOT/release/fid.exe}"
[ -x "$FID" ] || FID="$ROOT/release/fid"
export FID_HOME="${FID_HOME:-$ROOT}"
if ! command -v zig > /dev/null 2>&1; then
  echo "SKIP lg15-extern-core-name-check: no zig on PATH"
  exit 0
fi
case "$(uname -s)" in
  MINGW*|MSYS*|CYGWIN*)
    lib=$(zig env | sed -n 's/^ *\.lib_dir = "\(.*\)",$/\1/p' | sed -e 's#\\#/#g' -e 's#//*#/#g')
    inc="$lib/libc/include/any-windows-any"
    if [ ! -f "$inc/stdlib.h" ]; then echo "SKIP lg15-extern-core-name-check: no $inc/stdlib.h"; exit 0; fi
    export FID_INCLUDE="$inc" ;;
  *)
    if [ ! -f /usr/include/stdlib.h ]; then echo "SKIP lg15-extern-core-name-check: no /usr/include/stdlib.h"; exit 0; fi ;;
esac
T=$(mktemp -d)
trap 'rm -rf "$T"' EXIT
SRC=tests/bootstrap/at_import/test_at_import_system.fd
fail=0
if CC="zig cc" "$FID" ccompile "$SRC" -o "$T/sys" > "$T/cc.log" 2>&1; then
  exe="$T/sys"; [ -f "$exe.exe" ] && exe="$exe.exe"
  out=$("$exe" 2>&1 | tr -d '\r')
  if [ "$out" = "5" ]; then echo "ok   ccompile: builds, prints 5"; else echo "FAIL ccompile: prints '$out'"; fail=1; fi
else
  echo "FAIL ccompile:"; grep -i "error" "$T/cc.log" | head -5; fail=1
fi
if grep -q "conflicting types for 'abs'" "$T/cc.log"; then echo "FAIL the abs clash is back"; fail=1; fi
[ "$fail" = 0 ] && echo "PASS lg15-extern-core-name-check" || { echo "FAILED lg15-extern-core-name-check"; exit 1; }
