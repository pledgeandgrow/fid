#!/usr/bin/env bash
# B8 (docs/roadmap/inc-roadmap.md): a program built per module (`fid
# ccompile --inc`) prints what the same program built whole prints. Two
# bugs of compiler/compiler.fd split_c_units broke every program with a
# module-level global:
#   - main's unit defined each global without its initializer (`var g:
#     i64 = 7` read 0 in the --inc build);
#   - the globals' initializer function (`__init_globals`, codegen_c.fd
#     emit_global_init_function: Vec, map and computed initial values) is
#     `static`, so the split copied it into every unit's prelude, before
#     the globals and the prototypes it names: "use of undeclared
#     identifier '_g_...'" / "call to undeclared function 'Vec_new'" in
#     the core unit (any program importing a stdlib module with such a
#     global failed the same way).
# Now main's unit defines every global with its initializer and holds
# `__init_globals` after them; another unit declares only the globals it
# names (B6).
# Checks, each built single-TU and with --inc (output and exit code equal):
#   1. tests/bootstrap/test_inc_global_inits.fd and test_global_init.fd;
#   2. a three-module fixture: lib1 with simple and computed globals, lib2
#      reading lib1's globals through lib1's functions and holding its own
#      Vec global, main with a str global;
#   3. no unit but main's defines __init_globals, and core's unit declares
#      none of the fixture's globals.
# Usage: bash tests/tooling/inc-globals-check.sh
#   FID=<fid> (default release/fid.exe), CC=<cc> (default "zig cc")
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT=$(pwd)
FID="${FID:-release/fid.exe}"
case "$FID" in /*|?:*) ;; *) FID="$ROOT/$FID" ;; esac
export CC="${CC:-zig cc}"
export FID_HOME="${FID_HOME:-$ROOT}"
unset FIDES_INC_IR FIDES_INC_FLAT FIDES_SEED_STORE
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
if command -v cygpath >/dev/null 2>&1; then TMP=$(cygpath -m "$TMP"); fi
fail=0
bad() { echo "  FAIL  $1"; fail=1; }
ok() { echo "  ok    $1"; }
EXE=""
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) EXE=".exe" ;; esac

# both <dir> <file.fd> <label>: build single-TU and --inc in <dir>, compare
both() {
  local d="$1" f="$2" label="$3"
  if ! (cd "$d" && "$FID" ccompile "$f" --no-cache -o "ref$EXE" >"$d/ref.log" 2>&1); then
    bad "$label: single-TU build failed"; tail -8 "$d/ref.log"; return
  fi
  if ! (cd "$d" && "$FID" ccompile "$f" --inc --no-cache -o "inc$EXE" >"$d/inc.log" 2>&1); then
    bad "$label: --inc build failed"
    grep -m3 -E 'error:|undeclared|duplicate symbol|failed' "$d/inc.log" | sed 's/^/          /'
    return
  fi
  local r i
  r=$(cd "$d" && timeout 60 "./ref$EXE" </dev/null 2>&1; echo "exit=$?")
  i=$(cd "$d" && timeout 60 "./inc$EXE" </dev/null 2>&1; echo "exit=$?")
  if [ "$r" = "$i" ]; then ok "$label: --inc prints what the single TU prints ($(grep -E '^Compiling|^All' "$d/inc.log" | tail -1))"
  else bad "$label: output differs"; diff <(echo "$r") <(echo "$i") | head -8 | sed 's/^/          /'; fi
}

echo "globals in a per-module build (B8):"
for t in test_inc_global_inits test_global_init; do
  mkdir -p "$TMP/$t"
  cp "tests/bootstrap/$t.fd" "$TMP/$t/main.fd"
  both "$TMP/$t" main.fd "1. $t"
done

F="$TMP/fix"
mkdir -p "$F"
cat > "$F/lib1.fd" <<'EOI'
module lib1;
var counter: i64 = 10;
var label: str = "lib1";
var seen: Vec<i64> = Vec::new();
var scale: i64 = 3 * 4;
pub fn bump() -> i64 {
    counter = counter + 1;
    seen.push(counter);
    return counter * scale;
}
pub fn report() -> str {
    return label + " " + int_to_str(seen.len());
}
EOI
cat > "$F/lib2.fd" <<'EOI'
module lib2;
use lib1;
var log: Vec<str> = Vec::new();
pub fn step() -> i64 {
    val v = lib1::bump();
    log.push(int_to_str(v));
    return v;
}
pub fn joined() -> str {
    var s: str = "";
    var i: i64 = 0;
    while i < log.len() {
        if i > 0 {
            s = s + ",";
        }
        s = s + log.get(i);
        i = i + 1;
    }
    return s;
}
EOI
cat > "$F/main.fd" <<'EOI'
use lib1;
use lib2;
var title: str = "globals";
var rounds: i64 = 2;
fn main() {
    println(title);
    var i: i64 = 0;
    while i < rounds {
        lib2::step();
        i = i + 1;
    }
    println(lib2::joined());
    println(lib1::report());
}
EOI
both "$F" main.fd "2. three modules with globals"

# 3. where __init_globals and the globals' declarations went
TU="$F/inc.fides-tu"
if [ -d "$TU" ]; then
  defs=$(grep -l 'static void __init_globals(void) {' "$TU"/*.c 2>/dev/null | xargs -n1 basename 2>/dev/null | tr '\n' ' ')
  case "$defs" in
    *main*) if [ "$(echo $defs | wc -w)" = 1 ]; then ok "3. only main's unit defines __init_globals ($defs)"
            else bad "3. __init_globals defined in: $defs"; fi ;;
    *) bad "3. __init_globals defined in: '${defs:-no unit}'" ;;
  esac
  core=$(ls "$TU"/*core*.c 2>/dev/null | head -1)
  if [ -n "$core" ] && ! grep -qE '_g_(counter|label|seen|scale|log|title|rounds)\b' "$core"; then
    ok "3. core's unit declares none of the program's globals"
  else bad "3. core's unit ($core) names a global of the program"; fi
else bad "3. no $TU (the --inc build keeps its units' C there)"; fi

if [ "$fail" = 0 ]; then echo "inc-globals-check: PASS"; else echo "inc-globals-check: FAIL"; fi
exit "$fail"
