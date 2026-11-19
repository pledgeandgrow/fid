#!/usr/bin/env bash
# inc-layouts-ffi-check.sh — per-unit FFI externs and `==` helpers in
# `fid ccompile --inc` (docs/roadmap/inc-roadmap.md B2; compiler/compiler.fd
# split_c_units, record_func_modules).
#
# The fixture: lib2 already declares an extern from <math.h> (so the
# #include and `// PLINK: m` lines of the shared prelude are there before the
# edits), lib3 has nothing to do with either.
#   1. a cold --inc build matches a single-TU build.
#   2. lib1 declares and calls a second <math.h> extern: lib1 is rebuilt,
#      lib2, lib3 and core are not (the extern's `#define lib1_floor floor`
#      is in lib1's unit only).
#   3. the output still matches a single-TU build.
#   4. FIDES_INC_DEBUG=1: lib3 carries fewer externs than there are, and
#      lib3's unit file has no lib1_floor while lib1's has.
#   5. lib1 adds a struct (its `==` helper __eq_Pt is synthesized): lib1 is
#      rebuilt, main's unit, lib3 and core are not; __eq_Pt is defined in
#      lib1's unit file.
#   6. the output still matches a single-TU build.
#   7. control: FIDES_INC_EQ=main (the helper in main's unit, as before)
#      puts __eq_Pt in main's unit file.
#
# Usage: bash tests/tooling/inc-layouts-ffi-check.sh
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
cat > "$F/lib2.fd" <<'EOI'
module lib2;
extern "C" include "<math.h>" link "m" {
    fn sqrt as lib2_sqrt(x: f64) -> f64;
}
@unsafe
pub fn root() -> f64 { return lib2_sqrt(16.0); }
EOI
cat > "$F/lib3.fd" <<'EOI'
module lib3;
pub fn greet() -> str { return "hi"; }
EOI
cat > "$F/main.fd" <<'EOI'
use lib1;
use lib2;
use lib3;
@unsafe
fn main() -> i64 {
    println(lib1::one());
    println(lib2::root());
    println(lib3::greet());
    return 0;
}
EOI
LIB1_WITH_EXTERN='module lib1;
extern "C" include "<math.h>" link "m" {
    fn floor as lib1_floor(x: f64) -> f64;
}
@unsafe
pub fn one() -> i64 { var f: f64 = lib1_floor(1.75); return f as i64; }'
LIB1_WITH_STRUCT='module lib1;
extern "C" include "<math.h>" link "m" {
    fn floor as lib1_floor(x: f64) -> f64;
}
pub struct Pt { pub x: i64, pub y: i64 }
@unsafe
pub fn one() -> i64 { var f: f64 = lib1_floor(1.75); var p: Pt = Pt { x: f as i64, y: 0 }; return p.x + p.y; }'

inc_build() { (cd "$F" && "$FID" ccompile main.fd --inc -o main_inc.exe >"$1" 2>&1); }
ref_out() {
  (cd "$F" && "$FID" ccompile main.fd --no-cache -o main_ref.exe >/dev/null 2>&1) || { echo "<ref build failed>"; return; }
  (cd "$F" && ./main_ref.exe 2>&1; echo "exit=$?")
}
inc_out() { (cd "$F" && ./main_inc.exe 2>&1; echo "exit=$?"); }
compiled() { grep -E '^Compiling [0-9]+ of [0-9]+ units|^All [0-9]+ units' "$1" || true; }
# the rebuilt unit names, space-separated, padded
rebuilt() { echo " $(compiled "$1" | sed 's/^[^:]*: //; s/,//g') "; }
# the definition line of the struct's `==` helper (not its prototype)
EQ_DEF='^[A-Za-z_0-9 ]*__eq_[A-Za-z0-9_]*Pt\(.*\) \{'
# the C file of a unit (<index>_<name>.c in main_inc.fides-tu)
unit_c() { ls "$F"/main_inc.fides-tu/*_"$1".c 2>/dev/null | head -1; }

if inc_build "$TMP/b1.log"; then
  if [ "$(inc_out)" = "$(ref_out)" ]; then ok "1. cold --inc build matches single TU"
  else bad "1. --inc output differs from single TU"; fi
else bad "1. cold --inc build failed"; tail -15 "$TMP/b1.log"; fi

printf '%s\n' "$LIB1_WITH_EXTERN" > "$F/lib1.fd"
if inc_build "$TMP/b2.log"; then
  r=$(rebuilt "$TMP/b2.log")
  if echo "$r" | grep -qE ' (lib2|lib3|core) '; then bad "2. an extern used only by lib1 rebuilt other units: $(compiled "$TMP/b2.log")"
  elif echo "$r" | grep -q ' lib1 '; then ok "2. new extern in lib1 leaves lib2, lib3 and core alone ($(compiled "$TMP/b2.log"))"
  else bad "2. expected lib1 to be rebuilt, got '$(compiled "$TMP/b2.log")'"; fi
  if [ "$(inc_out)" = "$(ref_out)" ]; then ok "3. output after the extern edit matches single TU"
  else bad "3. output after the extern edit differs from single TU"; fi
else bad "2. --inc build after adding an extern failed"; tail -15 "$TMP/b2.log"; fi

if (cd "$F" && FIDES_INC_DEBUG=1 "$FID" ccompile main.fd --inc --no-cache -o main_inc.exe >"$TMP/b4.log" 2>&1); then
  l3=$(grep -E '^\[inc\] layouts [^ ]*lib3: [0-9]+ of [0-9]+ structs, [0-9]+ of [0-9]+ externs' "$TMP/b4.log" | head -1)
  if [ -n "$l3" ]; then
    a=$(echo "$l3" | sed -E 's/.*, ([0-9]+) of ([0-9]+) externs.*/\1/')
    b=$(echo "$l3" | sed -E 's/.*, ([0-9]+) of ([0-9]+) externs.*/\2/')
    if [ "$a" -lt "$b" ]; then ok "4a. FIDES_INC_DEBUG: $l3"
    else bad "4a. lib3 carries every extern: $l3"; fi
  else bad "4a. FIDES_INC_DEBUG=1 printed no '[inc] layouts' line for lib3"; fi
  c1=$(unit_c lib1); c3=$(unit_c lib3)
  if [ -n "$c1" ] && [ -n "$c3" ] && grep -q 'define lib1_floor' "$c1" && ! grep -q 'lib1_floor' "$c3"; then
    ok "4b. '#define lib1_floor floor' is in lib1's unit, not in lib3's"
  else bad "4b. lib1_floor placement (lib1: '$c1', lib3: '$c3')"; fi
else bad "4. FIDES_INC_DEBUG build failed"; tail -15 "$TMP/b4.log"; fi

# 5: a struct (and its synthesized __eq_Pt) in lib1 only
inc_build "$TMP/b5a.log" >/dev/null 2>&1
printf '%s\n' "$LIB1_WITH_STRUCT" > "$F/lib1.fd"
if inc_build "$TMP/b5.log"; then
  r=$(rebuilt "$TMP/b5.log")
  if echo "$r" | grep -qE ' (main|lib3|core) '; then bad "5. a struct of lib1 rebuilt other units: $(compiled "$TMP/b5.log")"
  elif echo "$r" | grep -q ' lib1 '; then ok "5. new struct in lib1 rebuilds lib1 only ($(compiled "$TMP/b5.log"))"
  else bad "5. expected lib1 to be rebuilt, got '$(compiled "$TMP/b5.log")'"; fi
  c1=$(unit_c lib1)
  if [ -n "$c1" ] && grep -qE "$EQ_DEF" "$c1"; then ok "5b. __eq_Pt is defined in lib1's unit"
  else bad "5b. __eq_Pt is not defined in lib1's unit ('$c1')"; fi
  if [ "$(inc_out)" = "$(ref_out)" ]; then ok "6. output after the struct edit matches single TU"
  else bad "6. output after the struct edit differs from single TU"; fi
else bad "5. --inc build after adding a struct failed"; tail -15 "$TMP/b5.log"; fi

# 7: control, the old placement
if (cd "$F" && FIDES_INC_EQ=main "$FID" ccompile main.fd --inc --no-cache -o main_inc.exe >"$TMP/b7.log" 2>&1); then
  cm=$(unit_c main)
  if [ -n "$cm" ] && grep -qE "$EQ_DEF" "$cm"; then ok "7. control: FIDES_INC_EQ=main puts __eq_Pt in main's unit"
  else bad "7. control: FIDES_INC_EQ=main did not put __eq_Pt in main's unit ('$cm')"; fi
else bad "7. FIDES_INC_EQ=main build failed"; tail -15 "$TMP/b7.log"; fi

if [ "$fail" = 0 ]; then echo "inc-layouts-ffi-check: PASS"; else echo "inc-layouts-ffi-check: FAIL"; fi
exit "$fail"
