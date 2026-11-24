#!/bin/bash
# tools-2g62-check.sh -- known bug 2g.62, three tools:
#   fmt           a prefix operator sticks to its operand (`&v`, `&mut v`,
#                 `-1`, `*p`), a postfix `?` to what it follows, a binary
#                 one keeps its spaces; a declaration after an annotation and
#                 a blank line starts its own line with no stray space
#                 (`@pure\n fn`); `while` after `}` starts a new line; the
#                 fixture tests/fmt/prefix_in.fd formats to tests/fmt/prefix.fd,
#                 which is stable, and both print the same
#   bench         builds through `fid ccompile`, so it uses $CC: with a C
#                 compiler that does not exist the build fails (it used to
#                 call tcc whatever CC said); with a real one (CC set when the
#                 script starts) it times n runs: one line per run, then
#                 min/mean/max
#   parse-header  C `int` parameters and results are both i32 (parameters
#                 were i64), unsigned u32, short i16, unsigned char u8, long
#                 long i64; a program passing i64 values and the generated
#                 i64 constants to them type-checks; with CC, a call of the
#                 C library's abs() through such a binding returns 7 for -7
#
# Usage: tests/tooling/tools-2g62-check.sh [fid]
#   default: release/fid-dev(.exe), else release/fid(.exe)
#   CC="zig cc" (or any C compiler) enables the bench timing and the abs()
#   run; without CC those two are skipped (reported as such).
set -u
cd "$(dirname "$0")/../.." || exit 1
ROOT="$(pwd)"
FID="${1:-${FID:-}}"
if [ -z "$FID" ]; then
  for f in release/fid-dev.exe release/fid-dev release/fid.exe release/fid; do
    if [ -x "$f" ]; then FID="$f"; break; fi
  done
fi
if [ -z "$FID" ]; then echo "tools-2g62: no compiler (give its path)"; exit 1; fi
case "$FID" in /*|[A-Za-z]:*) ;; *) FID="$ROOT/$FID" ;; esac
REAL_CC="${CC:-}"
W="$ROOT/build/tools-2g62"
rm -rf "$W"
mkdir -p "$W"
fail=0
ok()   { echo "  ok    $1"; }
bad()  { echo "  FAIL  $1"; fail=1; }
skip() { echo "  skip  $1"; }

echo "== fmt"
cp tests/fmt/prefix_in.fd "$W/p.fd"
if "$FID" fmt -w "$W/p.fd" > "$W/fmt.log" 2>&1; then
  if [ "$(tr -d '\r' < "$W/p.fd")" = "$(tr -d '\r' < tests/fmt/prefix.fd)" ]; then
    ok "tests/fmt/prefix_in.fd formats to tests/fmt/prefix.fd"
  else
    bad "prefix_in.fd formats differently:"
    diff <(tr -d '\r' < tests/fmt/prefix.fd) <(tr -d '\r' < "$W/p.fd") | head -20 | sed 's/^/        /'
  fi
else
  bad "fid fmt failed: $(head -3 "$W/fmt.log")"
fi
tr -d '\r' < tests/fmt/prefix.fd > "$W/stable.fd"
if "$FID" fmt "$W/stable.fd" --check > /dev/null 2>&1; then ok "prefix.fd passes fmt --check (stable)"; else bad "prefix.fd: fmt --check would reformat it"; fi
for bad_text in '& v' '& mut' '(- 1)' 'return - x' ') ?' '^ fn ' '^ use '; do
  if grep -q -- "$bad_text" "$W/p.fd"; then bad "formatted text still holds '$bad_text'"; fi
done
grep -q 'fn total(v: &Vec<i64>) -> i64 {' "$W/p.fd" && ok "&Vec in a parameter type" || bad "no '&Vec<i64>' parameter"
grep -q 'bump(&mut v);' "$W/p.fd" && ok "&mut v as an argument" || bad "no 'bump(&mut v);'"
grep -q 'val a: i64 = total(&v) & 3;' "$W/p.fd" && ok "&v prefix next to a binary &" || bad "no 'total(&v) & 3'"
grep -q 'val b: i64 = a \* -neg(2);' "$W/p.fd" && ok "a * -neg(2)" || bad "no 'a * -neg(2)'"
grep -q 'val h: i64 = half(x)?;' "$W/p.fd" && ok "postfix ? sticks" || bad "no 'half(x)?;'"
if tr -d '\r' < "$W/p.fd" | grep -q -x '@deterministic' && tr -d '\r' < "$W/p.fd" | grep -q -x 'fn neg(x: i64) -> i64 {'; then
  ok "annotation, blank line, then 'fn' at column 0"
else
  bad "annotation/blank line/fn not kept apart"
fi
printf 'fn main() {\n    var i: i64 = 0;\n    while i < 2 { i = i + 1; } while i < 4 { i = i + 1; }\n    println(int_to_str(i));\n}\n' > "$W/w.fd"
"$FID" fmt -w "$W/w.fd" > /dev/null 2>&1
if tr -d '\r' < "$W/w.fd" | grep -q '} while'; then bad "'} while' kept on one line"; else ok "a while after } starts its own line"; fi
printf '@pure\nfn one() -> i64 { return 1; }\n@cfg(target = "linux")\n@inline\npub fn two() -> i64 { return 2; }\nfn main() { println(int_to_str(one())); }\n' > "$W/a.fd"
"$FID" fmt -w "$W/a.fd" > /dev/null 2>&1
if tr -d '\r' < "$W/a.fd" | grep -q -x '@pure' && tr -d '\r' < "$W/a.fd" | grep -q -x 'fn one() -> i64 {' && tr -d '\r' < "$W/a.fd" | grep -q -x '@inline' && tr -d '\r' < "$W/a.fd" | grep -q -x 'pub fn two() -> i64 {'; then
  ok "an annotation keeps its own line (@pure / fn, @cfg(..) / @inline / pub fn)"
else
  bad "annotation joined to its item: $(tr -d '\r' < "$W/a.fd" | head -6 | tr '\n' '|')"
fi
want=$("$FID" run tests/fmt/prefix_in.fd 2> /dev/null | tr -d '\r')
got=$("$FID" run tests/fmt/prefix.fd 2> /dev/null | tr -d '\r')
if [ -n "$want" ] && [ "$want" = "$got" ]; then ok "prefix_in.fd and prefix.fd print the same"; else bad "prefix_in.fd and prefix.fd print differently ('$want' / '$got')"; fi

echo "== bench"
printf 'fn main() {\n    var s: i64 = 0;\n    var i: i64 = 0;\n    while i < 100000 { s = s + i; i = i + 1; }\n    println(int_to_str(s));\n}\n' > "$W/b.fd"
cd "$W" || exit 1
out=$(CC=no-such-cc-2g62 "$FID" bench b.fd -n 2 2>&1); st=$?
if [ $st -ne 0 ] && echo "$out" | grep -q "CC=no-such-cc-2g62" && echo "$out" | grep -q "the build failed"; then
  ok "bench uses CC (a missing C compiler fails the build)"
else
  bad "bench with CC=no-such-cc-2g62: exit $st: $(echo "$out" | tail -3)"
fi
if [ -n "$REAL_CC" ]; then
  out=$(CC="$REAL_CC" "$FID" bench b.fd -n 3 2>&1); st=$?
  runs=$(echo "$out" | grep -c '  run [0-9]/3: [0-9]*\.[0-9][0-9][0-9] ms')
  if [ $st -eq 0 ] && [ "$runs" = "3" ] && echo "$out" | grep -q "min .* ms  mean .* ms  max .* ms"; then
    ok "bench with CC=$REAL_CC: 3 timed runs and min/mean/max"
  else
    bad "bench with CC=$REAL_CC: exit $st, $runs timed runs: $(echo "$out" | tail -5)"
  fi
  [ -e _fides_bench_tmp.c ] && bad "bench left _fides_bench_tmp.c" || ok "bench removed its C file"
else
  skip "bench timing (CC not set)"
fi
cd "$ROOT" || exit 1

echo "== parse-header"
cat > "$W/geo.h" <<'EOF'
#define GEO_K 5
int geo_add(int a, int b);
double geo_scale(double x, double k);
unsigned int geo_mask(unsigned int m);
long long geo_big(long long v);
short geo_small(short s, unsigned char c);
EOF
if "$FID" parse-header "$W/geo.h" --link geo -o "$W/geo.fd" > /dev/null 2>&1; then
  for line in 'fn geo_add(a: i32, b: i32) -> i32;' 'fn geo_scale(x: f64, k: f64) -> f64;' 'fn geo_mask(m: u32) -> u32;' 'fn geo_big(v: i64) -> i64;' 'fn geo_small(s: i16, c: u8) -> i16;'; do
    if grep -q -F -- "$line" "$W/geo.fd"; then ok "$line"; else bad "missing '$line': $(grep -F "$(echo "$line" | cut -d'(' -f1)" "$W/geo.fd")"; fi
  done
  printf 'use core;\nuse geo;\n@unsafe\nfn main() {\n    var x: i64 = 7;\n    val r: i32 = geo_add(GEO_K, x);\n    val m: u32 = geo_mask(x);\n    val s: i16 = geo_small(x, 65);\n    val t: i64 = geo_add(1, 2);\n    println(int_to_str(r + t));\n}\n' > "$W/usegeo.fd"
  if "$FID" check "$W/usegeo.fd" > "$W/check.log" 2>&1; then
    ok "i64 values and constants pass to the i32/u32/i16/u8 parameters (fid check)"
  else
    bad "fid check of a program using the bindings: $(grep -i error "$W/check.log" | head -3)"
  fi
else
  bad "fid parse-header failed"
fi
if [ -n "$REAL_CC" ]; then
  printf 'int abs(int x);\n' > "$W/myabs.h"
  "$FID" parse-header "$W/myabs.h" -o "$W/myabs.fd" > /dev/null 2>&1
  if grep -q -F 'fn abs(x: i32) -> i32;' "$W/myabs.fd"; then
    printf 'use core;\nuse myabs;\n@unsafe\nfn main() {\n    var v: i64 = 0 - 7;\n    println(int_to_str(abs(v)));\n}\n' > "$W/runabs.fd"
    cd "$W" || exit 1
    if CC="$REAL_CC" "$FID" ccompile runabs.fd -o runabs.exe > ccabs.log 2>&1; then
      got=$(./runabs.exe 2> /dev/null | tr -d '\r')
      [ "$got" = "7" ] && ok "abs(-7) through the generated i32 binding prints 7" || bad "abs(-7) printed '$got'"
    else
      bad "ccompile of the abs() program failed: $(tail -3 ccabs.log)"
    fi
    cd "$ROOT" || exit 1
  else
    bad "myabs.fd: $(grep abs "$W/myabs.fd")"
  fi
else
  skip "abs() through the binding (CC not set)"
fi

if [ $fail -eq 0 ]; then echo "tools-2g62: all passed"; else echo "tools-2g62: FAILED"; fi
exit $fail
