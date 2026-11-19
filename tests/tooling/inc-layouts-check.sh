#!/usr/bin/env bash
# inc-layouts-check.sh — per-unit struct layouts in `fid ccompile --inc`
# (docs/roadmap/inc-roadmap.md B2; compiler/compiler.fd split_c_units).
#
# Before B2 every unit carried every struct definition of the program in
# its prelude, so a struct added to one module changed the text (and the
# object key) of every unit: core's unit and every other module rebuilt.
#   1. a cold --inc build matches a single-TU build.
#   2. adding a struct used only by lib1 does not rebuild lib3 or core
#      (only lib1 and the unit owning main, whose tail lists every function).
#   3. the output still matches a single-TU build.
#   4. FIDES_INC_DEBUG=1 prints "[inc] layouts <unit>: a of b structs" and
#      lib3 carries fewer structs than there are.
#   5. FIDES_INC_LAYOUTS=off (every unit carries every struct): the same
#      edit rebuilds lib3 too — the check above would fail without B2.
#
# Usage: tests/tooling/inc-layouts-check.sh
#   FID=<fid> (default release/fid.exe, else release/fid); CC (default zig cc)
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT=$(pwd)
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
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
pub fn one() -> i64 { return 1; }
EOI
cat > "$F/lib3.fd" <<'EOI'
module lib3;
pub fn greet() -> str { return "hi"; }
EOI
cat > "$F/main.fd" <<'EOI'
use lib1;
use lib3;
fn main() -> i64 {
    println(lib1::one());
    println(lib3::greet());
    return 0;
}
EOI
LIB1_WITH_STRUCT='module lib1;
pub struct Pt { pub x: i64, pub y: i64 }
pub fn one() -> i64 { var p: Pt = Pt { x: 1, y: 41 }; return p.x + p.y; }'

inc_build() { (cd "$F" && "$FID" ccompile main.fd --inc -o main_inc.exe >"$1" 2>&1); }
ref_out() {
  (cd "$F" && "$FID" ccompile main.fd --no-cache -o main_ref.exe >/dev/null 2>&1) || { echo "<ref build failed>"; return; }
  (cd "$F" && ./main_ref.exe 2>&1; echo "exit=$?")
}
inc_out() { (cd "$F" && ./main_inc.exe 2>&1; echo "exit=$?"); }
compiled() { grep -E '^Compiling [0-9]+ of [0-9]+ units|^All [0-9]+ units' "$1" || true; }

if inc_build "$TMP/b1.log"; then
  if [ "$(inc_out)" = "$(ref_out)" ]; then ok "1. cold --inc build matches single TU"
  else bad "1. --inc output differs from single TU"; fi
else bad "1. cold --inc build failed"; tail -15 "$TMP/b1.log"; fi

printf '%s\n' "$LIB1_WITH_STRUCT" > "$F/lib1.fd"
if inc_build "$TMP/b2.log"; then
  line=$(compiled "$TMP/b2.log")
  if echo "$line" | grep -qE 'units: .*(lib3|core)'; then bad "2. a struct used only by lib1 rebuilt other units: $line"
  elif echo "$line" | grep -qE 'units: .*lib1'; then ok "2. new struct in lib1 leaves lib3 and core alone ($line)"
  else bad "2. expected lib1 to be rebuilt, got '$line'"; fi
  if [ "$(inc_out)" = "$(ref_out)" ]; then ok "3. output after the struct edit matches single TU"
  else bad "3. output after the struct edit differs from single TU"; fi
else bad "2. --inc build after adding a struct failed"; tail -15 "$TMP/b2.log"; fi

if (cd "$F" && FIDES_INC_DEBUG=1 "$FID" ccompile main.fd --inc --no-cache -o main_inc.exe >"$TMP/b3.log" 2>&1); then
  l3=$(grep -E '^\[inc\] layouts [^ ]*lib3: [0-9]+ of [0-9]+ structs' "$TMP/b3.log" | head -1)
  if [ -n "$l3" ]; then
    a=$(echo "$l3" | sed -E 's/.*: ([0-9]+) of ([0-9]+) structs.*/\1/')
    b=$(echo "$l3" | sed -E 's/.*: ([0-9]+) of ([0-9]+) structs.*/\2/')
    if [ "$a" -lt "$b" ]; then ok "4. FIDES_INC_DEBUG: $l3"
    else bad "4. lib3 carries every struct: $l3"; fi
  else bad "4. FIDES_INC_DEBUG=1 printed no '[inc] layouts' line for lib3"; fi
else bad "4. FIDES_INC_DEBUG build failed"; fi

# 5: the same edit with layouts unscoped rebuilds lib3 (the control)
printf 'module lib1;\npub fn one() -> i64 { return 1; }\n' > "$F/lib1.fd"
(cd "$F" && FIDES_INC_LAYOUTS=off "$FID" ccompile main.fd --inc -o main_inc.exe >/dev/null 2>&1)
printf '%s\n' "$LIB1_WITH_STRUCT" > "$F/lib1.fd"
if (cd "$F" && FIDES_INC_LAYOUTS=off "$FID" ccompile main.fd --inc -o main_inc.exe >"$TMP/b5.log" 2>&1); then
  line=$(compiled "$TMP/b5.log")
  if echo "$line" | grep -qE 'units: .*lib3'; then ok "5. control: FIDES_INC_LAYOUTS=off rebuilds lib3 ($line)"
  else bad "5. control: with layouts unscoped lib3 was not rebuilt ($line)"; fi
else bad "5. FIDES_INC_LAYOUTS=off build failed"; fi

if [ "$fail" = 0 ]; then echo "inc-layouts-check: PASS"; else echo "inc-layouts-check: FAIL"; fi
exit "$fail"
