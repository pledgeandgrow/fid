#!/bin/bash
# inline2-vecfast-check.sh — OP5 (competitive roadmap): inline2 (compiler/
# iropt_me.fd ime_inline, on by default) must leave the calls the backends
# know by name as calls (ime_backend_named): Vec_get/Vec_set/Vec_push/
# Vec_len have their own fast lowering in pe_ir and nat_ir and their checks
# are proved away by checks.fd; copying core's body in their place made
# Vec-heavy programs 2x-4x slower.
#
# Usage: tests/tooling/inline2-vecfast-check.sh   (FID=<compiler>, default
#        release/fid.exe; OUT=<dir>, default build/inline2-vecfast)
#
# Checks, one line each (PASS/FAIL), exit status the number of FAILs:
#  1. test_midend_inline2_vecfast.fd emitted through `emit --nat` with
#     inline2 named on (FID_VERIFY=1), with it off, and with neither (the
#     default): main has the same number of machine operations after
#     selection in all three (FID_NAT_DUMP=main), since main calls nothing
#     inline2 may copy;
#  2. every executable, --nat and pe_ir (`emit`), prints the expected
#     lines; the pe_ir sizes are printed (INFO), not compared.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/inline2-vecfast}"
mkdir -p "$OUT"
T=tests/bootstrap/test_midend_inline2_vecfast.fd
bad=0
pass() { echo "PASS $1"; }
fail() { echo "FAIL $1"; bad=$((bad + 1)); }
sed -n 's|^// \([0-9][0-9 ]*\)$|\1|p' "$T" > "$OUT/want"
same() { cmp -s <(tr -d '\r' < "$1") "$OUT/want"; }
ops() { tr -d '\r' < "$1" | sed -n 's/^\[mir\] main after selection: \([0-9]*\) operations.*/\1/p' | head -1; }

# build <label> <extra emit args...>; the label's env from its name
build() {
  local label="$1"; shift
  rm -f "$OUT/$label.exe"
  case "$label" in
    *on) env -u FID_IROPT_OFF FID_IROPT_ON=inline2 FID_VERIFY=1 FID_NAT_DUMP=main "$FID" emit "$T" "$@" -o "$OUT/$label.exe" > "$OUT/$label.log" 2>&1 ;;
    *off) env -u FID_IROPT_ON FID_IROPT_OFF=inline2 FID_NAT_DUMP=main "$FID" emit "$T" "$@" -o "$OUT/$label.exe" > "$OUT/$label.log" 2>&1 ;;
    *) env -u FID_IROPT_ON -u FID_IROPT_OFF FID_VERIFY=1 FID_NAT_DUMP=main "$FID" emit "$T" "$@" -o "$OUT/$label.exe" > "$OUT/$label.log" 2>&1 ;;
  esac
  if [ -f "$OUT/$label.exe" ] && "$OUT/$label.exe" > "$OUT/$label.out" 2>&1 && same "$OUT/$label.out"; then
    pass "emit $label prints the expected lines"
  else
    fail "emit $label prints the expected lines (see $OUT/$label.log)"
  fi
}

build nat_on --nat
build nat_off --nat
build nat_def --nat
on=$(ops "$OUT/nat_on.log"); off=$(ops "$OUT/nat_off.log"); def=$(ops "$OUT/nat_def.log")
if [ -n "$on" ] && [ "$on" = "$off" ] && [ "$def" = "$off" ]; then
  pass "--nat: main keeps its Vec calls with inline2 ($off operations off, $on on, $def default)"
else
  fail "--nat: main keeps its Vec calls with inline2 (${off:-?} operations off, ${on:-?} on, ${def:-?} default)"
fi

build pe_on
build pe_off
build pe_def
son=$(wc -c < "$OUT/pe_on.exe" 2>/dev/null); soff=$(wc -c < "$OUT/pe_off.exe" 2>/dev/null); sdef=$(wc -c < "$OUT/pe_def.exe" 2>/dev/null)
# informational: core functions main reaches may still take inlined
# callees of their own, so a size difference is not a failure by itself
echo "INFO pe_ir executable sizes: ${soff:-?} off, ${son:-?} on, ${sdef:-?} default"
exit $bad
