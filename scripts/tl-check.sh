#!/bin/bash
# tl-check.sh — the tools of docs/roadmap/competitive-roadmap.md track TL:
#   TL1  fid fuzz <file.fd> [--seeds N]: the fuzz targets of a test file
#   TL2  an undefined name gets "did you mean" (tests/bootstrap/
#        test_tooling_did_you_mean_fail.fd)
#   TL3  fid fix: applies the suggested fixes, keeps only those that help
#        (tests/bootstrap/test_tooling_fix_fail.fd on a copy)
#   TL4  fid lsp: inlay hints and code actions (scripts/tl/lsp_probe.py)
#   TL6  --target linux-arm64 / linux-x64 / macos-arm64 / windows-x64 from
#        this host through the native backends (no C compiler)
#   TL9  ark: a path pact installed, found by `use`, locked and audited
#   TL11 fid doc <dir>: a page per module and an index with search
#
# Usage:
#   scripts/tl-check.sh
#   FID=<fid>   the compiler (default release/fid.exe, else release/fid)
set -u
cd "$(dirname "$0")/.." || exit 1
ROOT="$(pwd)"
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
case "$FID" in
  /*|[A-Za-z]:*) ;;
  *) FID="$ROOT/$FID" ;;
esac
DIR="$ROOT/build/tl-check"
mkdir -p "$DIR"
fail=0

ok() { echo "  ok    $1"; }
bad() { echo "  FAIL  $1"; fail=1; }
has() { # has <label> <file> <fixed text>
  if grep -qF -- "$3" "$2"; then ok "$1"; else bad "$1: no '$3' in $2"; fi
}

echo "== TL1 fid fuzz"
cp scripts/tl/fuzz_target.fd "$DIR/fuzz_target.fd"
if "$FID" fuzz "$DIR/fuzz_target.fd" --seeds 50 > "$DIR/fuzz.out" 2> "$DIR/fuzz.err"; then
  ok "fuzz run passed"
else
  bad "fuzz run failed (see $DIR/fuzz.out)"
fi
has "50 seeds per target" "$DIR/fuzz.out" "ok    fuzz_sum_in_range (50 seeds)"
if grep -qF "test_empty_sum" "$DIR/fuzz.out"; then
  bad "fid fuzz ran a test_ function"
else
  ok "fid fuzz runs only fuzz targets"
fi

echo "== TL2 did you mean"
"$FID" check tests/bootstrap/test_tooling_did_you_mean_fail.fd > "$DIR/dym.out" 2>&1
has "suggestion for countr" "$DIR/dym.out" "did you mean 'counter'?"

echo "== TL3 fid fix"
cp tests/bootstrap/test_tooling_fix_fail.fd "$DIR/fix_me.fd"
"$FID" fix "$DIR/fix_me.fd" --check > "$DIR/fix-check.out" 2> "$DIR/fix-check.err"
if cmp -s tests/bootstrap/test_tooling_fix_fail.fd "$DIR/fix_me.fd"; then
  ok "--check leaves the file as it was"
else
  bad "--check changed the file"
fi
"$FID" fix "$DIR/fix_me.fd" > "$DIR/fix.out" 2> "$DIR/fix.err"
has "val -> var" "$DIR/fix.out" "declare 'total' with var"
has "totl -> total" "$DIR/fix.out" "change 'totl' to 'total'"
has "unused import removed" "$DIR/fix.out" "remove the unused import string"
if "$FID" run "$DIR/fix_me.fd" > "$DIR/fixed.out" 2> /dev/null && [ "$(tr -d '\r' < "$DIR/fixed.out")" = "6" ]; then
  ok "the fixed program prints 6"
else
  bad "the fixed program does not print 6 (see $DIR/fix_me.fd)"
fi
# the by-value parameter and the `for` over a changed Vec (tests/projects/tl3_fix)
FID="$FID" bash tests/tooling/fix-tl3-check.sh || fail=1

echo "== TL4 fid lsp"
if command -v python3 > /dev/null; then
  python3 scripts/tl/lsp_probe.py "$FID" || fail=1
  # rename/references by scope and across files, inlay hints of types
  python3 tests/tooling/lsp_scope_probe.py "$FID" || fail=1
else
  echo "  skip  python3 not found"
fi

echo "== TL6 cross targets by OS-first names"
for t in linux-x64 linux-arm64 linux-riscv64 macos-arm64; do
  rm -f "$DIR/cross-$t"
  "$FID" emit tests/bootstrap/hello.fd --target "$t" -o "$DIR/cross-$t" > "$DIR/cross-$t.log" 2>&1
  if [ -s "$DIR/cross-$t" ]; then ok "--target $t"; else bad "--target $t wrote nothing (see $DIR/cross-$t.log)"; fi
done
rm -f "$DIR/cross-win.exe"
"$FID" emit tests/bootstrap/hello.fd --target windows-x64 -o "$DIR/cross-win" > "$DIR/cross-win.log" 2>&1
if [ -s "$DIR/cross-win.exe" ] && [ "$(head -c 2 "$DIR/cross-win.exe")" = "MZ" ]; then
  ok "--target windows-x64 (a PE)"
else
  bad "--target windows-x64 (see $DIR/cross-win.log)"
fi

echo "== TL9 ark"
P="$DIR/ark"
mkdir -p "$P/app" "$P/pacts"
# a fresh project each run (only the files this check writes)
rm -f "$P/app/ark.toml" "$P/app/ark.lock" "$P/app/ark_modules/greet.fd"
cat > "$P/pacts/greet.fd" << 'EOF'
module greet;

pub fn greet_twice(n: i64) -> i64 {
    return n * 2;
}
EOF
cat > "$P/app/main.fd" << 'EOF'
use greet;

fn main() -> i64 {
    print(greet_twice(21));
    print("\n");
    return 0;
}
EOF
(
  cd "$P/app" || exit 1
  "$FID" ark init app > ../init.out 2>&1
  "$FID" ark add greet --path ../pacts/greet.fd > ../add.out 2>&1
  "$FID" ark install > ../install.out 2>&1
  "$FID" run main.fd > ../run.out 2> ../run.err
  "$FID" ark audit > ../audit.out 2>&1
  echo "audit=$?" >> ../audit.out
  echo "// changed" >> ark_modules/greet.fd
  "$FID" ark audit > ../audit2.out 2>&1
  echo "audit=$?" >> ../audit2.out
)
has "installed" "$P/install.out" "OK (path)"
has "lock file" "$P/app/ark.lock" "resolved = \"fnv:"
has "use finds the pact" "$P/run.out" "42"
has "audit clean" "$P/audit.out" "audit=0"
has "audit lists the pact" "$P/audit.out" "1 file(s)"
has "audit sees a change" "$P/audit2.out" "CHANGED"
has "audit fails on it" "$P/audit2.out" "audit=1"

echo "== TL11 fid doc <dir>"
"$FID" doc "$P/pacts" --output "$DIR/doc" > "$DIR/doc.out" 2>&1
has "site written" "$DIR/doc.out" "1 module(s)"
has "index lists the module" "$DIR/doc/index.html" "greet.html"
has "index searches the items" "$DIR/doc/index.html" "\"greet_twice\""
has "module page links back" "$DIR/doc/greet.html" "All modules"

if [ "$fail" = 0 ]; then echo "tl-check: all passed"; else echo "tl-check: FAILED"; fi
exit "$fail"
