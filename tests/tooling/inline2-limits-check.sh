#!/bin/bash
# inline2-limits-check.sh — ME20/ME21 (llvm-roadmap): inline2's limits
# (compiler/iropt_me.fd ime_inline_limits_load, FID_INLINE2_LIMITS), its
# stats line (ime_inline_stats_line, FID_IROPT_STATS=1) and its resumed
# scan (ime_inline_function; FID_IROPT_OFF=inlinescan rescans from the
# first block as before).
#
# Usage: tests/tooling/inline2-limits-check.sh [more.fd ...]
#   FID=<compiler>   default release/fid.exe
#   OUT=<dir>        default build/inline2-limits
#   EXTRA="--nat"    more `emit` arguments (another backend)
#
# Checks, one PASS/FAIL line each; exit status the number of FAILs:
#  1. test_midend_inline2_limits.fd with the default limits: the stats
#     line is printed and inlines sites, both at once-called callees and at
#     multi-site ones (charged to the budget);
#  2. FID_INLINE2_LIMITS=max=0,once=0,flat=0,bonus=0: no site inlined, and
#     iropt starts from as many instructions as with inline2 off;
#  3. FID_INLINE2_LIMITS=growth=0,growmin=0: no multi-site inlining (no
#     budget), sites refused by growth, once-called still inlined;
#  4. FID_INLINE2_LIMITS=caller=1: no site inlined, sites refused by size;
#  5. FID_IROPT_OFF=inlinescan: the same sites, the same charge and the same
#     instruction counts as the resumed scan, which weighs no more sites;
#     the same for each further program named on the command line
#     (e.g. tests/bootstrap/test_matrix_mul.fd test_lib_bigint.fd);
#  6. every build passes the IR verifier (FID_VERIFY=1) and prints the
#     expected lines.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/inline2-limits}"
EXTRA="${EXTRA:-}"
mkdir -p "$OUT"
T=tests/bootstrap/test_midend_inline2_limits.fd
bad=0
pass() { echo "PASS $1"; }
fail() { echo "FAIL $1"; bad=$((bad + 1)); }
sed -n 's|^// \([0-9][0-9 ]*\)$|\1|p' "$T" > "$OUT/want"

# build <tag> <file> <env...>: emit with FID_VERIFY=1 FID_IROPT_STATS=1
build() {
  local tag="$1" src="$2"
  shift 2
  rm -f "$OUT/$tag.exe"
  env "$@" FID_VERIFY=1 FID_IROPT_STATS=1 "$FID" emit "$src" $EXTRA -o "$OUT/$tag.exe" > "$OUT/$tag.log" 2>&1 < /dev/null
}
# the numbers of inline2's line: sites once multi charged allow budget-refused size-refused weighed
nums() {
  tr -d '\r' < "$OUT/$1.log" | sed -n 's/^iropt: inline2 \([0-9]*\) sites (\([0-9]*\) once, \([0-9]*\) multi), charged \([0-9]*\) of \([0-9]*\), refused \([0-9]*\) by growth, \([0-9]*\) by caller size, weighed \([0-9]*\)$/\1 \2 \3 \4 \5 \6 \7 \8/p' | tail -1
}
before() { tr -d '\r' < "$OUT/$1.log" | sed -n 's/^iropt: instructions \([0-9]*\) -> .*/\1/p' | tail -1; }
after() { tr -d '\r' < "$OUT/$1.log" | sed -n 's/^iropt: instructions [0-9]* -> \([0-9]*\) .*/\1/p' | tail -1; }
runs_ok() { # runs_ok <tag> [want]: built, verified, prints want
  local w="${2:-$OUT/want}"
  [ -f "$OUT/$1.exe" ] || return 1
  grep -q "IR verifier" "$OUT/$1.log" && return 1
  "$OUT/$1.exe" > "$OUT/$1.out" 2>&1 < /dev/null
  cmp -s <(tr -d '\r' < "$OUT/$1.out") "$w"
}

build def "$T" X_INLINE2=default
build off "$T" FID_IROPT_OFF=inline2
build none "$T" FID_INLINE2_LIMITS=max=0,once=0,flat=0,bonus=0
build nogrow "$T" FID_INLINE2_LIMITS=growth=0,growmin=0
build tiny "$T" FID_INLINE2_LIMITS=caller=1
build rescan "$T" FID_IROPT_OFF=inlinescan

# 1
read -r s o m c a rb rc w <<< "$(nums def)"
if [ -n "${s:-}" ] && [ "$o" -gt 0 ] && [ "$m" -gt 0 ] && [ "$c" -gt 0 ] && [ "$c" -le "$a" ]; then
  pass "default limits: $s sites ($o once, $m multi), charged $c of $a"
else
  fail "default limits: stats line '$(nums def)' (want sites at once- and multi-site callees)"
fi
# 2
read -r s o m c a rb rc w <<< "$(nums none)"
if [ "${s:-x}" = 0 ] && [ "$(before none)" = "$(before off)" ]; then
  pass "max=0,once=0,flat=0,bonus=0: nothing inlined ($(before none) = $(before off) instructions)"
else
  fail "max=0,once=0,flat=0,bonus=0: '$(nums none)', $(before none) vs $(before off) instructions"
fi
# 3
read -r s o m c a rb rc w <<< "$(nums nogrow)"
if [ "${m:-x}" = 0 ] && [ "${c:-x}" = 0 ] && [ "${rb:-0}" -gt 0 ] && [ "${o:-0}" -gt 0 ]; then
  pass "growth=0,growmin=0: no multi-site inlining, $rb refused by growth, $o once"
else
  fail "growth=0,growmin=0: '$(nums nogrow)'"
fi
# 4
read -r s o m c a rb rc w <<< "$(nums tiny)"
if [ "${s:-x}" = 0 ] && [ "${rc:-0}" -gt 0 ]; then
  pass "caller=1: nothing inlined, $rc refused by caller size"
else
  fail "caller=1: '$(nums tiny)'"
fi
# 5 (the test, then each program named)
scan_same() { # scan_same <defTag> <rescanTag> <label>
  local d r
  d=$(nums "$1"); r=$(nums "$2")
  read -r s1 o1 m1 c1 a1 rb1 rc1 w1 <<< "$d"
  read -r s2 o2 m2 c2 a2 rb2 rc2 w2 <<< "$r"
  if [ -n "${s1:-}" ] && [ "$s1 $o1 $m1 $c1 $a1" = "$s2 $o2 $m2 $c2 $a2" ] && [ "$(before "$1")" = "$(before "$2")" ] && [ "$(after "$1")" = "$(after "$2")" ] && [ "$w1" -le "$w2" ]; then
    pass "$3: resumed scan = rescan ($s1 sites, $(before "$1") -> $(after "$1") instructions; weighed $w1 vs $w2)"
  else
    fail "$3: resumed '$d' $(before "$1")->$(after "$1"), rescan '$r' $(before "$2")->$(after "$2")"
  fi
}
scan_same def rescan "$(basename "$T" .fd)"
for p in "$@"; do
  n=$(basename "$p" .fd)
  build "$n.def" "$p" X_INLINE2=default
  build "$n.rescan" "$p" FID_IROPT_OFF=inlinescan
  scan_same "$n.def" "$n.rescan" "$n"
  if [ -f "$OUT/$n.def.exe" ] && [ -f "$OUT/$n.rescan.exe" ]; then
    "$OUT/$n.rescan.exe" > "$OUT/$n.rescan.out" 2>&1 < /dev/null
    if runs_ok "$n.def" <(tr -d '\r' < "$OUT/$n.rescan.out"); then
      pass "$n: same output with either scan"
    else
      fail "$n: output differs between the scans (or the verifier complained)"
    fi
  fi
done
# 6
for tag in def off none nogrow tiny rescan; do
  if runs_ok "$tag"; then
    pass "$tag build: verifier passes, expected lines"
  else
    fail "$tag build: see $OUT/$tag.log / $OUT/$tag.out"
  fi
done
exit $bad
