#!/usr/bin/env bash
# ir-unit-layout-key-check.sh — the per-unit layout key of the lowered IR
# (docs/roadmap/inc-roadmap.md B2; compiler/lower.fd lw_unit_layout_key).
#
# FID_IR_UNITS=<dir> splits the lowered program into one IrUnit per source
# module and writes the link table to <dir>/link.txt: a line
# `layouts <unit> <hash>` per unit (the layouts that unit's functions use,
# closed over their fields), then `layouts <hash>` for the whole table.
#   1. link.txt has a per-unit layouts line for lib1 and lib3, and the
#      whole-table line; FID_IR_UNITS reports no problem.
#   2. adding a struct used only by lib1 changes lib1's layout key and the
#      whole table's hash, and leaves lib3's layout key as it was.
#   3. the program still prints the same with the split and link.
#
# Usage: tests/tooling/ir-unit-layout-key-check.sh
#   FID=<fid> (default release/fid.exe, else release/fid)
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT=$(pwd)
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
case "$FID" in /*|?:*) ;; *) FID="$ROOT/$FID" ;; esac
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
fail=0
bad() { echo "  FAIL  $1"; fail=1; }
ok() { echo "  ok    $1"; }

F="$TMP/fix"
mkdir -p "$F" "$TMP/u1" "$TMP/u2"
cat > "$F/lib1.fd" <<'EOI'
module lib1;
pub fn one() -> i64 { return 42; }
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

# the key of `unit` in a link.txt ("" when there is no such line)
unit_key() { awk -v u="$2" '$1 == "layouts" && NF == 3 && $2 == u { print $3 }' "$1"; }
table_key() { awk '$1 == "layouts" && NF == 2 { print $2 }' "$1"; }

echo "== 1. per-unit layout keys"
(cd "$F" && FID_IR_UNITS="$TMP/u1" "$FID" dump-c main.fd > "$TMP/c1.c" 2> "$TMP/e1.txt")
if [ ! -f "$TMP/u1/link.txt" ]; then
  bad "no link.txt (see $TMP/e1.txt)"; cat "$TMP/e1.txt" | tail -5
  exit 1
fi
k1a=$(unit_key "$TMP/u1/link.txt" lib1)
k3a=$(unit_key "$TMP/u1/link.txt" lib3)
ta=$(table_key "$TMP/u1/link.txt")
if [ -n "$k1a" ] && [ -n "$k3a" ]; then ok "lib1 and lib3 have a layout key"; else bad "per-unit layouts lines missing:"; cat "$TMP/u1/link.txt"; fi
if [ -n "$ta" ]; then ok "the whole table's hash is still there"; else bad "no 'layouts <hash>' line"; fi
if grep -q "0 problems" "$TMP/e1.txt"; then ok "the split and link report no problem"; else bad "FID_IR_UNITS reported problems: $(grep '\[ir-units\]' "$TMP/e1.txt" | tail -3)"; fi

echo "== 2. a struct added for lib1 changes only lib1's key"
cat > "$F/lib1.fd" <<'EOI'
module lib1;
pub struct Pt { pub x: i64, pub y: i64 }
pub fn one() -> i64 { var p: Pt = Pt { x: 1, y: 41 }; return p.x + p.y; }
EOI
(cd "$F" && FID_IR_UNITS="$TMP/u2" "$FID" dump-c main.fd > "$TMP/c2.c" 2> "$TMP/e2.txt")
k1b=$(unit_key "$TMP/u2/link.txt" lib1)
k3b=$(unit_key "$TMP/u2/link.txt" lib3)
tb=$(table_key "$TMP/u2/link.txt")
if [ -n "$k1b" ] && [ "$k1a" != "$k1b" ]; then ok "lib1's layout key changed"; else bad "lib1's layout key did not change ($k1a -> $k1b)"; fi
if [ -n "$k3b" ] && [ "$k3a" = "$k3b" ]; then ok "lib3's layout key did not change"; else bad "lib3's layout key changed ($k3a -> $k3b)"; fi
if [ -n "$tb" ] && [ "$ta" != "$tb" ]; then ok "the whole table's hash changed"; else bad "the whole table's hash did not change"; fi

echo "== 3. the program still runs the same"
out=$(cd "$F" && FID_IR_UNITS=1 "$FID" run main.fd 2>/dev/null | tr -d '\r')
want=$(printf '42\nhi')
if [ "$out" = "$want" ]; then ok "prints 42 and hi"; else bad "printed: $out"; fi

if [ "$fail" = 0 ]; then echo "PASS"; else echo "FAIL"; fi
exit $fail
