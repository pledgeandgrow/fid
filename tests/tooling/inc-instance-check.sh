#!/usr/bin/env bash
# B3 (docs/roadmap/inc-roadmap.md): with `fid ccompile --inc`, a generic
# instance is placed in the unit of its caller (compiler/compiler.fd
# c_place_instances_at_callers), not in the unit of the generic's module.
# A fixture whose lib1 calls core's assert_eq<T>; lib1 then calls it with a
# new type (a new instance, assert_eq__bool). Checks:
#   1. the cold --inc build prints what a single-TU build prints;
#   2. the new instantiation rebuilds lib1 (and main's unit, whose tail
#      lists every function for panic traces) but not core's unit;
#   3. the output still matches the single-TU build;
#   4. FIDES_INC_INSTANCES=home (the old placement) also links and matches.
# Usage: bash tests/tooling/inc-instance-check.sh
#   FID=<fid> (default release/fid.exe), CC=<cc> (default "zig cc")
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT=$(pwd)
FID="${FID:-release/fid.exe}"
case "$FID" in /*|?:*) ;; *) FID="$ROOT/$FID" ;; esac
export CC="${CC:-zig cc}"
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
fail=0
bad() { echo "  FAIL  $1"; fail=1; }
ok() { echo "  ok    $1"; }
F="$TMP/fix"
mkdir -p "$F"
cat > "$F/lib1.fd" <<'EOI'
module lib1;
pub fn check_one() -> i64 {
    assert_eq(1, 1);
    return 1;
}
EOI
cat > "$F/lib2.fd" <<'EOI'
module lib2;
pub fn two() -> i64 { return 2; }
EOI
cat > "$F/main.fd" <<'EOI'
use lib1;
use lib2;
fn main() -> i64 {
    println(lib1::check_one() + lib2::two());
    return 0;
}
EOI
inc_build() { (cd "$F" && "$FID" ccompile main.fd --inc -o main_inc.exe >"$1" 2>&1); }
ref_out() {
  (cd "$F" && "$FID" ccompile main.fd --no-cache -o main_ref.exe >/dev/null 2>&1) || { echo "<ref build failed>"; return; }
  (cd "$F" && ./main_ref.exe 2>&1; echo "exit=$?")
}
inc_out() { (cd "$F" && ./main_inc.exe 2>&1; echo "exit=$?"); }

if inc_build "$TMP/b1.log"; then
  if [ "$(inc_out)" = "$(ref_out)" ]; then ok "1. cold --inc build matches single TU"
  else bad "1. --inc output differs from single TU"; fi
else bad "1. cold --inc build failed"; tail -15 "$TMP/b1.log"; fi

sed -i 's/    assert_eq(1, 1);/    assert_eq(1, 1);\n    assert_eq(true, true);/' "$F/lib1.fd"
if inc_build "$TMP/b2.log"; then
  line=$(grep -E '^Compiling [0-9]+ of [0-9]+ units' "$TMP/b2.log" || true)
  units=$(echo "$line" | sed 's/^[^:]*: //; s/,//g')
  if echo " $units " | grep -q ' lib1 ' && ! echo " $units " | grep -q ' core '; then
    ok "2. a new instance of core's assert_eq rebuilds its caller's unit, not core's ($line)"
  else bad "2. expected lib1 rebuilt and core not, got '$line'"; fi
  if [ "$(inc_out)" = "$(ref_out)" ]; then ok "3. output after the new instance matches single TU"
  else bad "3. output after the new instance differs from single TU"; fi
else bad "2. --inc build after the new instance failed"; tail -15 "$TMP/b2.log"; fi

if (cd "$F" && FIDES_INC_INSTANCES=home "$FID" ccompile main.fd --inc --no-cache -o main_inc.exe >"$TMP/b4.log" 2>&1) \
   && [ "$(inc_out)" = "$(ref_out)" ]; then ok "4. FIDES_INC_INSTANCES=home links and matches"
else bad "4. FIDES_INC_INSTANCES=home"; tail -15 "$TMP/b4.log"; fi

if [ "$fail" = 0 ]; then echo "inc-instance-check: PASS"; else echo "inc-instance-check: FAIL"; fi
exit $fail
