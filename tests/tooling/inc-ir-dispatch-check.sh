#!/usr/bin/env bash
# inc-ir-dispatch-check.sh — 2g.92 (docs/roadmap/inc-roadmap.md B1): on the
# IR-unit path of `fid ccompile --inc` (compiler/compiler.fd
# ccompile_inc_prepare), a unit's dynamic calls and calls through a
# function value dispatch over functions that other units define. The C
# generator takes a dyn call's candidates among the dyn types its module
# knows (trait impls, MakeDyn) and a function-value call's among the
# functions it has; a unit had neither, so lib2's `d.area()` on a type of
# main and lib2's `f(x)` on a function of main fell to `abort()` (exit 127).
#
# Fixture: lib2 makes the calls (show: a dyn call, apply: a function value),
# lib1 and main implement the capability, main passes a named function and
# a closure.
#   1. the --inc build prints what the single-TU build prints, exit 0;
#   2. lib2's unit dispatches to Sq (main) and Tri (lib1) and to `double`;
#   3. a body edit in lib3 still rebuilds lib3 only;
#   4. control: FIDES_INC_IR=off builds the same program.
# Usage: bash tests/tooling/inc-ir-dispatch-check.sh
#   FID=<fid> (default release/fid.exe), CC=<cc> (default "zig cc")
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT=$(pwd)
FID="${FID:-release/fid.exe}"
case "$FID" in /*|?:*) ;; *) FID="$ROOT/$FID" ;; esac
export CC="${CC:-zig cc}"
export FID_HOME="${FID_HOME:-$ROOT}"
unset FIDES_INC_IR FIDES_INC_FLAT
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
fail=0
bad() { echo "  FAIL  $1"; fail=1; }
ok() { echo "  ok    $1"; }
F="$TMP/fix"
mkdir -p "$F"
cat > "$F/lib2.fd" <<'EOI'
module lib2;
pub cap Shape {
    fn area(self) -> i64;
}
pub fn show(d: dyn Shape) -> i64 {
    return d.area();
}
pub fn apply(f: fn(i64) -> i64, x: i64) -> i64 {
    return f(x);
}
EOI
cat > "$F/lib1.fd" <<'EOI'
module lib1;
use lib2;
pub struct Tri { pub b: i64, pub h: i64 }
impl Shape for Tri {
    pub fn area(self: Tri) -> i64 { return self.b * self.h / 2; }
}
pub fn tri(b: i64, h: i64) -> Tri { return Tri { b: b, h: h }; }
EOI
cat > "$F/lib3.fd" <<'EOI'
module lib3;
pub fn greet() -> str { return "hi"; }
EOI
cat > "$F/main.fd" <<'EOI'
use lib1;
use lib2;
use lib3;
struct Sq { s: i64 }
impl Shape for Sq {
    pub fn area(self: Sq) -> i64 { return self.s * self.s; }
}
fn double(x: i64) -> i64 {
    return x * 2;
}
fn make_adder(n: i64) -> fn(i64) -> i64 {
    return fn(x: i64) { x + n };
}
fn main() -> i64 {
    val q = Sq { s: 5 };
    println(lib2::show(q as dyn Shape));
    val t = lib1::tri(4, 3);
    println(lib2::show(t as dyn Shape));
    println(lib2::apply(double, 5));
    println(lib2::apply(make_adder(3), 4));
    println(lib3::greet());
    return 0;
}
EOI
WANT=$'25\n6\n10\n7\nhi\nexit=0'

inc_build() { (cd "$F" && FIDES_INC_DEBUG=1 "$FID" ccompile main.fd --inc -o main_inc.exe >"$1" 2>&1); }
ref_out() {
  (cd "$F" && "$FID" ccompile main.fd --no-cache -o main_ref.exe >/dev/null 2>&1) || { echo "<ref build failed>"; return; }
  (cd "$F" && ./main_ref.exe 2>&1; echo "exit=$?")
}
inc_out() { (cd "$F" && ./main_inc.exe 2>&1; echo "exit=$?"); }
unit_c() { ls "$F"/main_inc.fides-tu/*_"$1".c 2>/dev/null | head -1; }

echo "cross-unit dispatch on the IR units (ccompile --inc):"
if inc_build "$TMP/b1.log"; then
  ref=$(ref_out); got=$(inc_out)
  if [ "$got" = "$ref" ] && [ "$ref" = "$WANT" ]; then ok "1. --inc prints what the single TU prints (dyn calls into main and lib1, function values of main)"
  else bad "1. --inc output '$(echo "$got" | tr '\n' '|')', single TU '$(echo "$ref" | tr '\n' '|')', want '$(echo "$WANT" | tr '\n' '|')'"; fi
  c2=$(unit_c lib2)
  if [ -n "$c2" ] && grep -q 'strcmp(_t, "Sq")' "$c2" && grep -q 'strcmp(_t, "Tri")' "$c2"; then ok "2. lib2's dyn call dispatches to Sq (main) and Tri (lib1)"
  else bad "2. lib2's dyn call candidates: $(grep -o 'strcmp(_t, "[A-Za-z_0-9]*")' "$c2" 2>/dev/null | tr '\n' ' ')"; fi
  if [ -n "$c2" ] && grep -q 'strcmp(_fn_name, "double")' "$c2"; then ok "2. lib2's call through a function value dispatches to main's double"
  else bad "2. lib2's function-value candidates: $(grep -o 'strcmp(_fn_name, "[A-Za-z_0-9]*")' "$c2" 2>/dev/null | tr '\n' ' ')"; fi
else bad "1. cold --inc build failed"; tail -15 "$TMP/b1.log"; fi

sed -i 's/"hi"/"hey"/' "$F/lib3.fd"
if inc_build "$TMP/b2.log"; then
  line=$(grep -E '^Compiling [0-9]+ of [0-9]+ units' "$TMP/b2.log" || true)
  if echo "$line" | grep -qE ': lib3$'; then ok "3. body edit in lib3 rebuilds lib3 only ($line)"
  else bad "3. body edit in lib3: '$line'"; fi
  [ "$(inc_out)" = "$(ref_out)" ] || bad "3. output after the body edit differs from single TU"
else bad "3. --inc build after a body edit failed"; tail -15 "$TMP/b2.log"; fi

if (cd "$F" && FIDES_INC_IR=off "$FID" ccompile main.fd --inc --no-cache -o main_inc.exe >"$TMP/b4.log" 2>&1) \
   && [ "$(inc_out)" = "$(ref_out)" ]; then ok "4. control: FIDES_INC_IR=off matches"
else bad "4. FIDES_INC_IR=off"; tail -15 "$TMP/b4.log"; fi

if [ "$fail" = 0 ]; then echo "inc-ir-dispatch-check: PASS"; else echo "inc-ir-dispatch-check: FAIL"; fi
exit $fail
