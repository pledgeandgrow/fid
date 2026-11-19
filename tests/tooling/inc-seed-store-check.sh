#!/usr/bin/env bash
# B6 (docs/roadmap/inc-roadmap.md): the stdlib ships prebuilt. An installed
# toolchain has a read-only seeded store of unit objects, `cache/` beside
# fid.exe (compiler.fd fid_seed_store_dir), looked up after the local
# `.fidcache` and before FIDES_CAS (inc.fd inc_artifact_get,
# inc_seed_get). `fid ccompile --inc --seed-store <dir>` fills one with
# every unit's object but main's (main.fd build_c_units_ir); the installer
# runs it on the examples (installer/build-installer.sh) and fides.nsi
# installs it. For a fresh project's core unit to be found there, its key
# must not depend on the rest of the program: the unit key leaves out the
# input's and output's names (inc.fd inc_unit_build_key), a unit other than
# main's carries only the globals and C functions it names (compiler.fd
# ccompile_inc_prepare, FIDES_INC_SCOPE_DATA) and core's dispatches over the
# program's types live in main's unit (FIDES_INC_DISPATCH).
#
# Checks:
#   1. seeding from tests/bootstrap/hello.fd puts core's object in the
#      store ("Seeded 1 units into <store>: core"), not main's;
#   2. seeding from a second program (structs, a global, Option) adds no
#      second core object (core's key is the same);
#   3. a fresh project (other code: a struct in a Vec, a global with an
#      initializer, a map) built with FIDES_SEED_STORE=<store> takes core
#      from the store ("Took 1 of 2 units from the seeded store", the
#      "[inc] seeded store:" line) and compiles main only, and prints what
#      its single-TU build prints;
#   4. installed layout: fid.exe copied to <inst>/ with the store as
#      <inst>/cache, no FIDES_SEED_STORE: a second fresh project takes core
#      from <inst>/cache;
#   5. FIDES_SEED_STORE=off: core is compiled ("Compiling 2 of 2 units");
#   6. an ordinary build never writes into the store (same file list).
# Usage: bash tests/tooling/inc-seed-store-check.sh
#   FID=<fid> (default release/fid.exe), CC=<cc> (default "zig cc")
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT=$(pwd)
FID="${FID:-release/fid.exe}"
case "$FID" in /*|?:*) ;; *) FID="$ROOT/$FID" ;; esac
export CC="${CC:-zig cc}"
export FID_HOME="${FID_HOME:-$ROOT}"
unset FIDES_SEED_STORE FIDES_INC_SCOPE_DATA FIDES_INC_DISPATCH FIDES_INC_IR FIDES_INC_FLAT FIDES_CAS
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
# a path a native fid.exe reads the same in an argument and in FIDES_SEED_STORE
if command -v cygpath >/dev/null 2>&1; then TMP=$(cygpath -m "$TMP"); fi
fail=0
bad() { echo "  FAIL  $1"; fail=1; }
ok() { echo "  ok    $1"; }
EXE=""
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) EXE=".exe" ;; esac
STORE="$TMP/store"
mkdir -p "$STORE" "$TMP/seed1" "$TMP/seed2" "$TMP/fresh1" "$TMP/fresh2" "$TMP/inst"

store_list() { (cd "$STORE" && ls -1 | sort); }

echo "seeded store (B6):"
# 1. seed from hello
if "$FID" ccompile tests/bootstrap/hello.fd --inc -o "$TMP/seed1/hello$EXE" --seed-store "$STORE" >"$TMP/s1.log" 2>&1; then
  line=$(grep -E '^Seeded [0-9]+ units into ' "$TMP/s1.log" | tail -1)
  n=$(store_list | grep -vc '\.tmp$')
  units=${line##*: }
  if echo ", $units," | grep -q ', core,' && ! echo ", $units," | grep -q ', main,'; then
    ok "1. $line ($n file(s) in the store)"
  else bad "1. expected 'Seeded N units into ...: core' without main, got '$line'"; tail -5 "$TMP/s1.log"; fi
else bad "1. ccompile --inc --seed-store tests/bootstrap/hello.fd failed"; tail -15 "$TMP/s1.log"; fi
before=$(store_list)

# 2. a second seed program: core's key is the same
cat > "$TMP/seed2/main.fd" <<'EOI'
struct Pair {
    a: i64,
    b: i64,
}

var g_calls: i64 = 0;

fn find(v: i64) -> Option<Pair> {
    g_calls = g_calls + 1;
    if v > 0 {
        return Option::Some(Pair { a: v, b: v + 1 });
    }
    return Option::None;
}

fn main() {
    match find(3) {
        Option::Some(p) => { println(int_to_str(p.a + p.b)); }
        Option::None => { println("none"); }
    }
    println(int_to_str(g_calls));
}
EOI
if (cd "$TMP/seed2" && "$FID" ccompile main.fd --inc -o "seed2$EXE" --seed-store "$STORE" >"$TMP/s2.log" 2>&1); then
  added=$(comm -13 <(echo "$before") <(store_list) | grep -vc '\.tmp$')
  if [ "$added" = 0 ]; then ok "2. a second program's core object is the one already there ($(grep -E '^Seeded' "$TMP/s2.log"))"
  else bad "2. the second program added $added object(s): core's key depends on the program ($(grep -E '^Seeded' "$TMP/s2.log"))"; fi
else bad "2. seeding from a second program failed"; tail -15 "$TMP/s2.log"; fi

before2=$(store_list)

# 3. a fresh project takes core from the store
fresh() {
  cat > "$1/main.fd" <<'EOI'
struct Item {
    name: str,
    qty: i64,
}

var g_names: Vec<str> = Vec::new();
var g_base: i64 = 40;

fn add(items: & mut Vec<Item>, name: str, qty: i64) {
    items.push(Item { name: name, qty: qty });
    g_names.push(name);
}

fn main() {
    var items: Vec<Item> = Vec::new();
    add(& mut items, "bolt", 2);
    add(& mut items, "nut", 3);
    var total: i64 = g_base;
    var i: i64 = 0;
    while i < items.len() {
        total = total + items.get(i).qty;
        i = i + 1;
    }
    var m: HashMap<str, i64> = HashMap::new();
    m.insert("total", total);
    println("total " + int_to_str(m.get("total")));
    println(int_to_str(g_names.len()) + " " + g_names.get(1));
}
EOI
}
fresh "$TMP/fresh1"
want=$'total 45\n2 nut'
if (cd "$TMP/fresh1" && FIDES_SEED_STORE="$STORE" FIDES_INC_DEBUG=1 "$FID" ccompile main.fd --inc -o "fresh$EXE" >"$TMP/f1.log" 2>&1); then
  took=$(grep -E '^Took [0-9]+ of [0-9]+ units from the seeded store' "$TMP/f1.log")
  comp=$(grep -E '^Compiling|^All' "$TMP/f1.log" | tail -1)
  if [ -n "$took" ] && grep -q '^\[inc\] seeded store: ' "$TMP/f1.log" && [ "$comp" = "Compiling 1 of 2 units: main" ]; then
    ok "3. fresh project: $took; $comp"
  else bad "3. fresh project did not take core from the store: '${took:-no Took line}', '$comp'"; fi
  out=$(cd "$TMP/fresh1" && "./fresh$EXE" 2>&1)
  ref=""
  if (cd "$TMP/fresh1" && "$FID" ccompile main.fd --no-cache -o "ref$EXE" >"$TMP/f1r.log" 2>&1); then ref=$(cd "$TMP/fresh1" && "./ref$EXE" 2>&1); fi
  if [ "$out" = "$want" ] && [ "$out" = "$ref" ]; then ok "3. the program with core from the store prints what its single-TU build prints"
  else bad "3. output '$out', single TU '$ref', expected '$want'"; fi
else bad "3. --inc build of the fresh project failed"; tail -15 "$TMP/f1.log"; fi

# 4. installed layout: <inst>/fid.exe with <inst>/cache
cp "$FID" "$TMP/inst/fid$EXE" 2>/dev/null || cp "$FID$EXE" "$TMP/inst/fid$EXE"
cp -r "$STORE" "$TMP/inst/cache"
fresh "$TMP/fresh2"
if (cd "$TMP/fresh2" && "$TMP/inst/fid$EXE" ccompile main.fd --inc -o "fresh$EXE" >"$TMP/f2.log" 2>&1); then
  took=$(grep -E '^Took [0-9]+ of [0-9]+ units from the seeded store' "$TMP/f2.log")
  out=$(cd "$TMP/fresh2" && "./fresh$EXE" 2>&1)
  if [ -n "$took" ] && [ "$out" = "$want" ]; then ok "4. installed fid reads <fid dir>/cache: $took"
  else bad "4. installed fid: '${took:-no Took line}', output '$out'"; fi
else bad "4. --inc build with the installed fid failed"; tail -15 "$TMP/f2.log"; fi

# 5. FIDES_SEED_STORE=off (the local store emptied first; --no-cache would
# skip every store)
rm -rf "$TMP/fresh1/.fidcache" "$TMP/fresh1/fresh$EXE" "$TMP/fresh1"/*.fides-stamp "$TMP/fresh1/fresh.fides-tu"
if (cd "$TMP/fresh1" && FIDES_SEED_STORE=off "$FID" ccompile main.fd --inc -o "fresh$EXE" >"$TMP/f3.log" 2>&1); then
  comp=$(grep -E '^Compiling|^All' "$TMP/f3.log" | tail -1)
  if [ "$comp" = "Compiling 2 of 2 units: core, main" ] && ! grep -q '^Took ' "$TMP/f3.log"; then ok "5. FIDES_SEED_STORE=off: $comp"
  else bad "5. FIDES_SEED_STORE=off: '$comp'"; fi
else bad "5. build with FIDES_SEED_STORE=off failed"; tail -15 "$TMP/f3.log"; fi

# 6. read-only: the builds of 3-5 wrote nothing into either store
if [ "$(store_list)" = "$before2" ] && [ "$(cd "$TMP/inst/cache" && ls -1 | sort)" = "$before2" ]; then
  ok "6. builds that read the store wrote nothing into it ($(echo "$before2" | grep -c .) file(s))"
else bad "6. a store changed during builds that only read it"; fi

if [ "$fail" = 0 ]; then echo "inc-seed-store-check: PASS"; else echo "inc-seed-store-check: FAIL"; fi
exit "$fail"
