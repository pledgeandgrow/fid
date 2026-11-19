#!/usr/bin/env bash
# inc-instance-copies-check.sh — B3 (docs/roadmap/inc-roadmap.md,
# docs/inc-instances.md): with `fid ccompile --inc`, a generic instance that
# two or more units call, and that its generic's own unit does not call,
# has its definition in the calling unit first by name and a `static` copy
# in every other calling unit (compiler/compiler.fd
# c_copy_instances_to_callers), so the generic's unit is not rebuilt.
#
# Fixture: gen defines pick<T> and twice<T> (twice calls pick); lib1 and
# lib2 both call pick with i64. The edit makes both call twice with bool:
# two new instances, twice__bool and pick__bool, the second one reached
# only through the first (a copy's callee is placed the same way).
#   1. the cold --inc build prints what a single-TU build prints;
#   2. after the edit lib1 and lib2 are rebuilt, gen and core are not;
#   3. lib1's unit defines twice__bool and pick__bool (external), lib2's
#      has `static` copies (definition and prototype), gen's has neither;
#   4. the output matches the single-TU build;
#   5. FIDES_INC_DEBUG=1 prints "[inc] instance copies: N static copies";
#   6. control: FIDES_INC_INSTANCES=move (no copies) rebuilds gen for the
#      same edit, and links and matches;
#   7. FIDES_INC_INSTANCES=home links and matches.
# Usage: bash tests/tooling/inc-instance-copies-check.sh
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
cat > "$F/gen.fd" <<'EOI'
module gen;
pub fn pick<T>(a: T, b: T, first: bool) -> T {
    if first {
        return a;
    }
    return b;
}
pub fn twice<T>(a: T, b: T, first: bool) -> T {
    return pick(pick(a, b, first), b, true);
}
pub fn base() -> i64 { return 7; }
EOI
cat > "$F/main.fd" <<'EOI'
use gen;
use lib1;
use lib2;
fn main() -> i64 {
    println(gen::base());
    println(lib1::l1());
    println(lib2::l2());
    return 0;
}
EOI
before() {
  printf 'module lib1;\nuse gen;\npub fn l1() -> i64 {\n    return gen::pick(10, 20, true);\n}\n' > "$F/lib1.fd"
  printf 'module lib2;\nuse gen;\npub fn l2() -> i64 {\n    return gen::pick(30, 40, false);\n}\n' > "$F/lib2.fd"
}
after() {
  printf 'module lib1;\nuse gen;\npub fn l1() -> i64 {\n    var b: bool = gen::twice(true, false, false);\n    if b {\n        return 0;\n    }\n    return gen::pick(10, 20, true);\n}\n' > "$F/lib1.fd"
  printf 'module lib2;\nuse gen;\npub fn l2() -> i64 {\n    var b: bool = gen::twice(false, true, true);\n    if b {\n        return 1;\n    }\n    return gen::pick(30, 40, false);\n}\n' > "$F/lib2.fd"
}

inc_build() { (cd "$F" && "$FID" ccompile main.fd --inc -o main_inc.exe >"$1" 2>&1); }
ref_out() {
  (cd "$F" && "$FID" ccompile main.fd --no-cache -o main_ref.exe >/dev/null 2>&1) || { echo "<ref build failed>"; return; }
  (cd "$F" && ./main_ref.exe 2>&1; echo "exit=$?")
}
inc_out() { (cd "$F" && ./main_inc.exe 2>&1; echo "exit=$?"); }
compiled() { grep -E '^Compiling [0-9]+ of [0-9]+ units|^All [0-9]+ units' "$1" || true; }
rebuilt() { echo " $(compiled "$1" | sed 's/^[^:]*: //; s/,//g') "; }
unit_c() { ls "$F"/main_inc.fides-tu/*_"$1".c 2>/dev/null | head -1; }
# definition lines of instance $2 in unit file $1 (static or not)
defs() { grep -E "^[A-Za-z_0-9 ]*[^A-Za-z_0-9]$2\(.*\) \{" "$1" 2>/dev/null || true; }

before
if inc_build "$TMP/b1.log"; then
  if [ "$(inc_out)" = "$(ref_out)" ]; then ok "1. cold --inc build matches single TU"
  else bad "1. --inc output differs from single TU"; fi
else bad "1. cold --inc build failed"; tail -15 "$TMP/b1.log"; fi

after
if inc_build "$TMP/b2.log"; then
  r=$(rebuilt "$TMP/b2.log")
  if echo "$r" | grep -qE ' (gen|core) '; then bad "2. new instances called from lib1 and lib2 rebuilt gen or core: $(compiled "$TMP/b2.log")"
  elif echo "$r" | grep -q ' lib1 ' && echo "$r" | grep -q ' lib2 '; then ok "2. lib1 and lib2 rebuilt, gen and core not ($(compiled "$TMP/b2.log"))"
  else bad "2. expected lib1 and lib2 rebuilt, got '$(compiled "$TMP/b2.log")'"; fi
  c1=$(unit_c lib1); c2=$(unit_c lib2); cg=$(unit_c gen)
  for inst in twice__bool pick__bool; do
    d1=$(defs "$c1" "$inst"); d2=$(defs "$c2" "$inst"); dg=$(defs "$cg" "$inst")
    if [ -n "$d1" ] && ! echo "$d1" | grep -q '^static ' \
       && echo "$d2" | grep -q '^static ' \
       && grep -qE "^static [^{]*[^A-Za-z_0-9]$inst\(.*\);" "$c2" \
       && [ -z "$dg" ]; then
      ok "3. $inst: defined in lib1, a static copy in lib2, not in gen"
    else bad "3. $inst placement: lib1='$d1' lib2='$d2' gen='$dg'"; fi
  done
  if [ "$(inc_out)" = "$(ref_out)" ]; then ok "4. output after the edit matches single TU"
  else bad "4. output after the edit differs from single TU"; fi
else bad "2. --inc build after the edit failed"; tail -15 "$TMP/b2.log"; fi

if (cd "$F" && FIDES_INC_DEBUG=1 "$FID" ccompile main.fd --inc --no-cache -o main_inc.exe >"$TMP/b5.log" 2>&1) \
   && grep -qE '^\[inc\] instance copies: [1-9][0-9]* static copies' "$TMP/b5.log"; then
  ok "5. $(grep -E '^\[inc\] instance copies' "$TMP/b5.log" | head -1)"
else bad "5. FIDES_INC_DEBUG=1 printed no '[inc] instance copies: N static copies' line"; fi

# 6: control, the placement without copies
before
(cd "$F" && FIDES_INC_INSTANCES=move "$FID" ccompile main.fd --inc -o main_inc.exe >/dev/null 2>&1)
after
if (cd "$F" && FIDES_INC_INSTANCES=move "$FID" ccompile main.fd --inc -o main_inc.exe >"$TMP/b6.log" 2>&1); then
  r=$(rebuilt "$TMP/b6.log")
  if echo "$r" | grep -q ' gen '; then ok "6a. control: FIDES_INC_INSTANCES=move rebuilds gen ($(compiled "$TMP/b6.log"))"
  else bad "6a. control: without copies gen was not rebuilt ($(compiled "$TMP/b6.log"))"; fi
  if [ "$(inc_out)" = "$(ref_out)" ]; then ok "6b. FIDES_INC_INSTANCES=move matches single TU"
  else bad "6b. FIDES_INC_INSTANCES=move output differs"; fi
else bad "6. FIDES_INC_INSTANCES=move build failed"; tail -15 "$TMP/b6.log"; fi

if (cd "$F" && FIDES_INC_INSTANCES=home "$FID" ccompile main.fd --inc --no-cache -o main_inc.exe >"$TMP/b7.log" 2>&1) \
   && [ "$(inc_out)" = "$(ref_out)" ]; then ok "7. FIDES_INC_INSTANCES=home links and matches"
else bad "7. FIDES_INC_INSTANCES=home"; tail -15 "$TMP/b7.log"; fi

if [ "$fail" = 0 ]; then echo "inc-instance-copies-check: PASS"; else echo "inc-instance-copies-check: FAIL"; fi
exit "$fail"
