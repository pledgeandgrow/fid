#!/usr/bin/env bash
# inc-check.sh — per-module C units (`fid ccompile --inc`) stay correct and
# rebuild only what changed (docs/roadmap/inc-roadmap.md B7, B8, B9).
#
# Always (a 4-module fixture in a temporary directory):
#   1. a cold `--inc` build links and prints what a single-TU build prints.
#      lib1::one and lib2::two have the same body: before Nov 15 their units
#      had one canonical hash, so lib2 reused lib1's object (link error).
#   2. a body edit in lib3 rebuilds exactly one unit (lib3).
#   3. renaming an exported function (lib1::one -> lib1::uno) links: before
#      Nov 15 lib1's object was reused under its old symbol name.
#   4. a new string literal in lib1 (which renumbers later literals)
#      still gives the single-TU output.
#   5. FIDES_INC_DEBUG=1 prints the per-unit prototype table (C1).
# SUITE=1 (B8): every top-level tests/bootstrap/test_*.fd that is not an
#   expected-fail test is built both ways; program output and exit code
#   must match.
# FIXPOINT=1 (B9): the compiler itself is built with `--inc`; that compiler
#   must generate the same C for compiler/main.fd as $FID does (so $FID must
#   be built from the current compiler/ sources), and building it again
#   with itself must reuse every unit's object ("All N units up to date").
#
# Usage: scripts/inc-check.sh
#   FID=<fid>   the compiler (default release/fid.exe, else release/fid)
#   CC=<cc>     C compiler (default "zig cc"); OPT=<0..3> C opt level (0)
set -u
cd "$(dirname "$0")/.." || exit 1
ROOT=$(pwd)
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
case "$FID" in /*|?:*) ;; *) FID="$ROOT/$FID" ;; esac
export CC="${CC:-zig cc}"
# The fixture builds in a temporary directory: fid finds stdlib/ there only
# through FID_HOME (or beside its own exe, which a FID outside the tree is not)
export FID_HOME="${FID_HOME:-$ROOT}"
OPT="${OPT:-0}"
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
fail=0
bad() { echo "  FAIL  $1"; fail=1; }
ok() { echo "  ok    $1"; }

# --- fixture --------------------------------------------------------------
F="$TMP/fix"
mkdir -p "$F"
cat > "$F/lib1.fd" <<'EOI'
module lib1;
pub fn one() -> i64 { return 1; }
EOI
cat > "$F/lib2.fd" <<'EOI'
module lib2;
pub fn two() -> i64 { return 1; }
EOI
cat > "$F/lib3.fd" <<'EOI'
module lib3;
pub fn greet() -> str { return "hi"; }
EOI
cat > "$F/main.fd" <<'EOI'
use lib1;
use lib2;
use lib3;
fn main() -> i64 {
    println(lib1::one() + lib2::two());
    println(lib3::greet());
    return 0;
}
EOI

# inc_build <log>: --inc build of main.fd into main_inc.exe
inc_build() { (cd "$F" && "$FID" ccompile main.fd --inc --cc-opt "$OPT" -o main_inc.exe >"$1" 2>&1); }
# ref_out: output of a fresh single-TU build
ref_out() {
  (cd "$F" && "$FID" ccompile main.fd --no-cache --cc-opt "$OPT" -o main_ref.exe >/dev/null 2>&1) || { echo "<ref build failed>"; return; }
  (cd "$F" && ./main_ref.exe 2>&1; echo "exit=$?")
}
inc_out() { (cd "$F" && ./main_inc.exe 2>&1; echo "exit=$?"); }

echo "fixture (4 modules):"
if inc_build "$TMP/b1.log"; then
  if [ "$(inc_out)" = "$(ref_out)" ]; then ok "1. cold --inc build matches single TU (same-body units kept apart)"
  else bad "1. --inc output differs from single TU"; fi
else bad "1. cold --inc build failed"; sed 's/^/        /' "$TMP/b1.log" | tail -15; fi

sed -i 's/"hi"/"hey"/' "$F/lib3.fd"
if inc_build "$TMP/b2.log"; then
  line=$(grep -E '^Compiling [0-9]+ of [0-9]+ units' "$TMP/b2.log" || true)
  if echo "$line" | grep -qE '^Compiling 1 of [0-9]+ units: lib3$'; then ok "2. body edit rebuilds one unit ($line)"
  else bad "2. body edit: expected 'Compiling 1 of N units: lib3', got '$line'"; fi
  [ "$(inc_out)" = "$(ref_out)" ] || bad "2. output after the body edit differs from single TU"
else bad "2. --inc build after a body edit failed"; tail -15 "$TMP/b2.log"; fi

sed -i 's/fn one()/fn uno()/' "$F/lib1.fd"
sed -i 's/lib1::one()/lib1::uno()/' "$F/main.fd"
if inc_build "$TMP/b3.log"; then
  if [ "$(inc_out)" = "$(ref_out)" ]; then ok "3. renamed exported fn links ($(grep -E '^Compiling|^All' "$TMP/b3.log"))"
  else bad "3. output after the rename differs from single TU"; fi
else bad "3. --inc build after renaming lib1::one failed (stale object?)"; tail -15 "$TMP/b3.log"; fi

cat >> "$F/lib1.fd" <<'EOI'
pub fn label() -> str { return "label"; }
EOI
sed -i 's/println(lib3::greet());/println(lib1::label()); println(lib3::greet());/' "$F/main.fd"
if inc_build "$TMP/b4.log"; then
  if [ "$(inc_out)" = "$(ref_out)" ]; then ok "4. new literal (later ones renumbered) matches single TU ($(grep -E '^Compiling|^All' "$TMP/b4.log"))"
  else bad "4. output after adding a literal differs from single TU"; fi
else bad "4. --inc build after adding a literal failed"; tail -15 "$TMP/b4.log"; fi

# --no-cache: with the stamp the unchanged C would stop before the split
if (cd "$F" && FIDES_INC_DEBUG=1 "$FID" ccompile main.fd --inc --no-cache --cc-opt "$OPT" -o main_inc.exe >"$TMP/b5.log" 2>&1) \
   && grep -qE '^\[inc\] unit [^ ]+: [0-9]+ of [0-9]+ protos' "$TMP/b5.log"; then
  ok "5. FIDES_INC_DEBUG prints the unit table ($(grep -c '^\[inc\] unit' "$TMP/b5.log") units)"
else bad "5. FIDES_INC_DEBUG=1 printed no '[inc] unit' lines"; fi

# --- SUITE=1: every test both ways (B8) -------------------------------------
if [ "${SUITE:-0}" = 1 ]; then
  echo "suite (single TU vs --inc):"
  S="$TMP/suite"; mkdir -p "$S"
  pass=0; diffs=0; skipped=0
  for t in tests/bootstrap/test_*.fd; do
    if head -1 "$t" | grep -q 'expect-error'; then continue; fi
    b=$(basename "$t" .fd)
    if ! "$FID" ccompile "$t" --no-cache --cc-opt "$OPT" -o "$S/$b.ref.exe" >/dev/null 2>&1; then
      skipped=$((skipped + 1)); continue   # not buildable through C at all
    fi
    if ! "$FID" ccompile "$t" --inc --cc-opt "$OPT" -o "$S/$b.inc.exe" >"$S/$b.log" 2>&1; then
      echo "  FAIL  $b: --inc build failed"
      # the first error says which (an undeclared function: a prototype the
      # unit lacks; a duplicate symbol: a reused object)
      grep -m2 -E 'error:|undeclared|duplicate symbol|failed' "$S/$b.log" | sed 's/^/          /'
      diffs=$((diffs + 1)); continue
    fi
    r=$(cd "$S" && timeout 60 "./$b.ref.exe" </dev/null 2>&1; echo "exit=$?")
    i=$(cd "$S" && timeout 60 "./$b.inc.exe" </dev/null 2>&1; echo "exit=$?")
    if [ "$r" = "$i" ]; then pass=$((pass + 1)); else echo "  FAIL  $b: output differs"; diffs=$((diffs + 1)); fi
  done
  echo "  suite: $pass same, $diffs different, $skipped not buildable through C"
  [ "$diffs" = 0 ] || fail=1
fi

# --- FIXPOINT=1: the compiler built per module (B9) -------------------------
if [ "${FIXPOINT:-0}" = 1 ]; then
  echo "fixpoint (compiler built with --inc):"
  X="$TMP/fx"; mkdir -p "$X"
  if "$FID" ccompile compiler/main.fd --inc --cc-opt "$OPT" -o "$X/fid-inc.exe" >"$X/b1.log" 2>&1; then
    ok "compiler built from $(grep -oE '[0-9]+ units' "$X/b1.log" | tail -1)"
    "$FID" dump-c compiler/main.fd >"$X/ref.c" 2>/dev/null
    "$X/fid-inc.exe" dump-c compiler/main.fd >"$X/inc.c" 2>/dev/null
    if [ -s "$X/ref.c" ] && cmp -s "$X/ref.c" "$X/inc.c"; then ok "the --inc compiler generates the same C ($(wc -c <"$X/ref.c") bytes)"
    else bad "the --inc compiler's C for compiler/main.fd differs from $FID's"; fi
    if "$X/fid-inc.exe" ccompile compiler/main.fd --inc --cc-opt "$OPT" -o "$X/fid-inc2.exe" >"$X/b2.log" 2>&1 \
       && grep -q '^All [0-9]* units up to date' "$X/b2.log"; then ok "self-build reused every unit's object (hash-identical artifacts)"
    else bad "self-build with the --inc compiler: $(grep -E '^Compiling|^All|failed' "$X/b2.log" | head -3)"; fi
  else bad "ccompile --inc compiler/main.fd failed"; tail -15 "$X/b1.log"; fi
fi

if [ "$fail" = 0 ]; then echo "inc-check: PASS"; else echo "inc-check: FAIL"; fi
exit "$fail"
