#!/usr/bin/env bash
# B1 (docs/roadmap/inc-roadmap.md): `fid ccompile --inc` generates C per
# IrUnit (compiler/compiler.fd ccompile_inc_prepare, incu_unit_c; main.fd
# build_c_units_ir) and keys each object by the unit's IR, so a unit whose
# IR did not change is neither generated nor compiled. Before B1 the whole
# program's C was generated on every build and cut by split_c_units.
#
# Fixture: lib1 (a function, an async function spawned by name from main),
# lib2 (a struct only it uses, a capability and a function making a dynamic
# call), lib3 (a string), main (implements lib2's capability: lib2's
# dynamic call must dispatch to a method of another unit).
# Checks:
#   1. a cold --inc build prints what a single-TU build prints, and
#      FIDES_INC_DEBUG reports every unit generated ("[inc] ir units:");
#   2. a body edit in lib3 generates and compiles lib3 only;
#   3. a struct added to lib1 (used in lib1's body only) generates lib1 and
#      not core: each unit carries only the layouts it names (B2);
#   4. a comment at the end of main.fd changes no unit's key: "C
#      translation unchanged";
#   5. FIDES_INC_IR=off (the C text cut per module) builds the same program.
# Usage: bash tests/tooling/inc-ir-units-check.sh
#   FID=<fid> (default release/fid.exe), CC=<cc> (default "zig cc")
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT=$(pwd)
FID="${FID:-release/fid.exe}"
case "$FID" in /*|?:*) ;; *) FID="$ROOT/$FID" ;; esac
export CC="${CC:-zig cc}"
export FID_HOME="${FID_HOME:-$ROOT}"
OPT="${OPT:-0}"
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
pub async fn work(x: i64) -> Future<i64> { return x * 2; }
EOI
cat > "$F/lib2.fd" <<'EOI'
module lib2;
struct Pair { a: i64, b: i64 }
pub cap Shape {
    fn area(self) -> i64;
}
pub fn two() -> i64 {
    val p = Pair { a: 1, b: 1 };
    return p.a * p.b;
}
pub fn show(d: dyn Shape) -> i64 {
    return d.area();
}
EOI
cat > "$F/lib3.fd" <<'EOI'
module lib3;
pub fn greet() -> str { return "hi"; }
EOI
cat > "$F/main.fd" <<'EOI'
use sync;
use lib1;
use lib2;
use lib3;
struct Sq { s: i64 }
impl Shape for Sq {
    pub fn area(self: Sq) -> i64 { return self.s * self.s; }
}
fn main() -> i64 {
    println(lib1::one() + lib2::two());
    println(lib3::greet());
    val t = spawn lib1::work(21);
    println(await t);
    val q = Sq { s: 5 };
    println(lib2::show(q as dyn Shape));
    return 0;
}
EOI

inc_build() { (cd "$F" && FIDES_INC_DEBUG=1 "$FID" ccompile main.fd --inc --cc-opt "$OPT" -o main_inc.exe >"$1" 2>&1); }
ref_out() {
  (cd "$F" && "$FID" ccompile main.fd --no-cache --cc-opt "$OPT" -o main_ref.exe >/dev/null 2>&1) || { echo "<ref build failed>"; return; }
  (cd "$F" && ./main_ref.exe 2>&1; echo "exit=$?")
}
inc_out() { (cd "$F" && ./main_inc.exe 2>&1; echo "exit=$?"); }
# generated <log>: the units the build generated C for ("a, b")
generated() { sed -n 's/^\[inc\] ir units: [0-9]* units, [0-9]* generated (\([^;]*\);.*/\1/p' "$1" | tail -1; }

echo "C per IrUnit (ccompile --inc):"
if inc_build "$TMP/b1.log"; then
  ref=$(ref_out); got=$(inc_out)
  if [ "$got" = "$ref" ] && echo "$ref" | grep -q '^25$'; then ok "1. cold build matches single TU (spawn by name, dynamic call into main's unit)"
  else bad "1. --inc output differs from single TU: got '$got', want '$ref'"; fi
  if grep -qE '^\[inc\] ir units: ([0-9]+) units, \1 generated' "$TMP/b1.log"; then ok "1. every unit generated on a cold build ($(generated "$TMP/b1.log"))"
  else bad "1. no '[inc] ir units: N units, N generated' line (is --inc still cutting the program's C?)"; grep '^\[inc\]' "$TMP/b1.log" | head -5; fi
else bad "1. cold --inc build failed"; tail -15 "$TMP/b1.log"; fi

sed -i 's/"hi"/"hey"/' "$F/lib3.fd"
if inc_build "$TMP/b2.log"; then
  g=$(generated "$TMP/b2.log")
  line=$(grep -E '^Compiling [0-9]+ of [0-9]+ units' "$TMP/b2.log" || true)
  if [ "$g" = "lib3" ] && echo "$line" | grep -qE ': lib3$'; then ok "2. body edit in lib3: only lib3 generated and compiled ($line)"
  else bad "2. body edit in lib3: generated '$g', '$line'"; fi
  [ "$(inc_out)" = "$(ref_out)" ] || bad "2. output after the body edit differs from single TU"
else bad "2. --inc build after a body edit failed"; tail -15 "$TMP/b2.log"; fi

cat > "$F/lib1.fd" <<'EOI'
module lib1;
struct Tmp { v: i64, w: i64 }
pub fn one() -> i64 {
    val t = Tmp { v: 1, w: 0 };
    return t.v + t.w;
}
pub async fn work(x: i64) -> Future<i64> { return x * 2; }
EOI
if inc_build "$TMP/b3.log"; then
  g=$(generated "$TMP/b3.log")
  if echo ", $g, " | grep -q ', lib1, ' && ! echo ", $g, " | grep -q ', core, '; then ok "3. a struct only lib1 uses: lib1 generated, core not ($g)"
  else bad "3. a struct only lib1 uses: generated '$g' (expected lib1, not core)"; fi
  [ "$(inc_out)" = "$(ref_out)" ] || bad "3. output after the new struct differs from single TU"
else bad "3. --inc build after adding a struct failed"; tail -15 "$TMP/b3.log"; fi

echo "// a comment" >> "$F/main.fd"
if inc_build "$TMP/b4.log"; then
  if grep -q 'C translation unchanged' "$TMP/b4.log"; then ok "4. a comment changes no unit's key (C translation unchanged)"
  else bad "4. a comment in main.fd: $(grep -E '^\[inc\] ir units|^Compiling|^All' "$TMP/b4.log" | tr '\n' ' ')"; fi
else bad "4. --inc build after a comment failed"; tail -15 "$TMP/b4.log"; fi

if (cd "$F" && FIDES_INC_IR=off FIDES_INC_DEBUG=1 "$FID" ccompile main.fd --inc --no-cache --cc-opt "$OPT" -o main_inc.exe >"$TMP/b5.log" 2>&1) \
   && ! grep -q '^\[inc\] ir units' "$TMP/b5.log" && [ "$(inc_out)" = "$(ref_out)" ]; then ok "5. FIDES_INC_IR=off cuts the program's C and matches"
else bad "5. FIDES_INC_IR=off"; tail -15 "$TMP/b5.log"; fi

if [ "$fail" = 0 ]; then echo "inc-ir-units-check: PASS"; else echo "inc-ir-units-check: FAIL"; fi
exit $fail
