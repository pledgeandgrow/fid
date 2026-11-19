#!/bin/bash
# range-set-check.sh — LF8 (llvm-roadmap, Nov 17): the range set. Each
# block of compiler/checks.fd's range scan carries several facts (locals
# and values), a join keeps every fact all its ways in share, a store
# bounds its local until the next store, and a loop header holds several
# assumed facts (PeirRanges.sf_*, peir_sf_inherit, peir_join_set,
# peir_xs_save, PeirBackFacts.m_*, peir_lf_step). FID_IROPT_OFF=rangeset
# turns it off (rangejoin turns it off too).
#
# Usage: tests/tooling/range-set-check.sh
#   FID=<compiler>   default release/fid.exe
#   OUT=<dir>        default build/rangeset
#   CC=<c compiler>  for the harness and the C-path checks (default zig cc)
#   NAT=1            also every output check through `emit --nat`
#   SUITE=1          then the whole suite with rangeset on and off
#                    (midend-pass-suite-check.sh PASS=rangeset; with NAT=1
#                    also EXTRA=--nat)
#
# Checks, one line each (PASS/FAIL); exit status the number of FAILs:
#  1. tests/tooling/range_set_main.fd (the analysis on hand-built IR, C
#     backend): its lines with the set on equal its "Expected output", and
#     with FID_IROPT_OFF=rangeset hull/hull_v/pick/walk2 y are "checked"
#     (the set is what removes them);
#  2. `fid explain` of test_midend_range_set.fd: hull, pick and walk2 each
#     remove a mul overflow check, and more checks with the set than
#     without; the fail variants keep the multiply's check;
#  3. test_midend_range_set.fd emitted (pe_ir; --nat with NAT=1) and
#     ccompiled prints its expected lines with the set on and off; the two
#     fail variants exit non-zero after their first line;
#  4. the earlier range tests print the same with the set on and off.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/rangeset}"
NAT="${NAT:-0}"
SUITE="${SUITE:-0}"
export CC="${CC:-zig cc}"
mkdir -p "$OUT"
T=tests/bootstrap
bad=0
pass() { echo "PASS $*"; }
fail() { echo "FAIL $*"; bad=$((bad + 1)); }
check() { # check <label> <command...>
  local label="$1"; shift
  if "$@" > /dev/null 2>&1; then pass "$label"; else fail "$label"; fi
}
expected() {
  sed -n '/^\/\/ Expected output/,/^[^\/]/p' "$1" | sed -n 's/^\/\/ \(.*\)$/\1/p' | grep -v '^Expected output'
}
removed() { tr -d '\r' < "$1" | sed -n "s/^fn $2: \([0-9]*\) check(s) removed, \([0-9]*\) kept.*/\1/p" | head -1; }
section() { tr -d '\r' < "$1" | awk -v h="fn $2:" 'index($0, h) == 1 { on = 1; print; next } /^fn [A-Za-z_0-9]*:/ { on = 0 } on'; }

# 1. the harness
h="$OUT/range_set_main.exe"
rm -f "$h"
if "$FID" ccompile tests/tooling/range_set_main.fd -o "$h" > "$OUT/harness.build.log" 2>&1 && [ -f "$h" ]; then
  "$h" > "$OUT/harness.on.out" 2>&1
  FID_IROPT_OFF=rangeset "$h" > "$OUT/harness.off.out" 2>&1
  expected tests/tooling/range_set_main.fd > "$OUT/harness.want"
  check "harness: set on gives its expected lines" cmp -s <(tr -d '\r' < "$OUT/harness.on.out") "$OUT/harness.want"
  for c in "hull mul" "hull_v mul" "pick mul" "walk2 y"; do
    check "harness: set off leaves '$c' checked" grep -q "^$c: checked" <(tr -d '\r' < "$OUT/harness.off.out")
  done
  for c in "hull_fail mul" "walk2_fail y"; do
    if tr -d '\r' < "$OUT/harness.on.out" | grep -q "^$c: checked" && tr -d '\r' < "$OUT/harness.off.out" | grep -q "^$c: checked"; then
      pass "harness: '$c' checked on and off"
    else
      fail "harness: '$c' not checked on and off"
    fi
  done
else
  fail "harness does not build: $(grep -m1 -i -E 'error|panic' "$OUT/harness.build.log")"
fi

# 2. explain
for fn in hull pick walk2; do
  env -u FID_IROPT_OFF "$FID" explain $T/test_midend_range_set.fd "$fn" > "$OUT/on.$fn.txt" 2>&1
  FID_IROPT_OFF=rangeset "$FID" explain $T/test_midend_range_set.fd "$fn" > "$OUT/off.$fn.txt" 2>&1
  on_r=$(removed "$OUT/on.$fn.txt" "$fn"); off_r=$(removed "$OUT/off.$fn.txt" "$fn")
  check "explain $fn: the set removes more (on ${on_r:-?}, off ${off_r:-?})" test "${on_r:-0}" -gt "${off_r:-0}"
  check "explain $fn: mul check removed" grep -q 'removed mul overflow check' <(section "$OUT/on.$fn.txt" "$fn")
done
env -u FID_IROPT_OFF "$FID" explain $T/test_midend_range_set_fail.fd hull > "$OUT/fail.hull.txt" 2>&1
check "explain fail variant hull: mul kept" grep -q 'kept mul overflow check' <(section "$OUT/fail.hull.txt" hull)
env -u FID_IROPT_OFF "$FID" explain $T/test_midend_range_set_loop_fail.fd walk2 > "$OUT/fail.walk2.txt" 2>&1
check "explain loop fail variant walk2: a mul kept" grep -q 'kept mul overflow check' <(section "$OUT/fail.walk2.txt" walk2)

# 3. outputs
expected $T/test_midend_range_set.fd > "$OUT/want"
build_run() { # <tag> <env-or-empty> <cmd: emit|ccompile> <extra> <file>
  local tag="$1" e="$2" cmd="$3" extra="$4" file="$5" exe="$OUT/$1.exe"
  rm -f "$exe"
  if [ -n "$e" ]; then
    env "$e" "$FID" "$cmd" "$file" $extra -o "$exe" > "$exe.log" 2>&1
  else
    env -u FID_IROPT_OFF "$FID" "$cmd" "$file" $extra -o "$exe" > "$exe.log" 2>&1
  fi
  if [ ! -f "$exe" ]; then echo BUILD > "$exe.rc"; return; fi
  timeout 60 "$exe" > "$exe.out" 2>&1 < /dev/null
  echo $? > "$exe.rc"
}
modes=("pe:emit:" "c:ccompile:")
[ "$NAT" = 1 ] && modes+=("nat:emit:--nat")
for m in "${modes[@]}"; do
  tag="${m%%:*}"; rest="${m#*:}"; cmd="${rest%%:*}"; extra="${rest#*:}"
  for s in on off; do
    e=""; [ $s = off ] && e="FID_IROPT_OFF=rangeset"
    build_run "set.$tag.$s" "$e" "$cmd" "$extra" $T/test_midend_range_set.fd
    check "$tag $s: test_midend_range_set prints its expected lines" cmp -s <(tr -d '\r' < "$OUT/set.$tag.$s.exe.out") "$OUT/want"
    for fv in test_midend_range_set_fail test_midend_range_set_loop_fail; do
      build_run "$fv.$tag.$s" "$e" "$cmd" "$extra" $T/$fv.fd
      rc=$(cat "$OUT/$fv.$tag.$s.exe.rc")
      first=$(tr -d '\r' < "$OUT/$fv.$tag.$s.exe.out" 2>/dev/null | head -1)
      want1=7000000000000000; [ $fv = test_midend_range_set_loop_fail ] && want1=1
      if [ "$rc" != 0 ] && [ "$rc" != BUILD ] && [ "$first" = "$want1" ]; then
        pass "$tag $s: $fv prints $want1 then fails (rc $rc)"
      else
        fail "$tag $s: $fv (rc $rc, first line '$first')"
      fi
    done
  done
done

# 4. the earlier range tests: same output with the set on and off
for t in test_midend_range_loop test_midend_range_join test_opt_range_dom test_opt_range_facts test_opt_ranges test_midend_param_range test_midend_range_args; do
  [ -f "$T/$t.fd" ] || continue
  for m in "${modes[@]}"; do
    tag="${m%%:*}"; rest="${m#*:}"; cmd="${rest%%:*}"; extra="${rest#*:}"
    build_run "$t.$tag.on" "" "$cmd" "$extra" "$T/$t.fd"
    build_run "$t.$tag.off" "FID_IROPT_OFF=rangeset" "$cmd" "$extra" "$T/$t.fd"
    a="$OUT/$t.$tag.on.exe"; b="$OUT/$t.$tag.off.exe"
    if [ "$(cat "$a.rc")" = "$(cat "$b.rc")" ] && [ "$(cat "$a.rc")" != BUILD ] && cmp -s <(tr -d '\r' < "$a.out") <(tr -d '\r' < "$b.out"); then
      pass "$tag $t: same with the set on and off"
    else
      fail "$tag $t: on (rc $(cat "$a.rc")) and off (rc $(cat "$b.rc")) differ"
    fi
  done
done

if [ "$SUITE" = 1 ]; then
  FID="$FID" PASS=rangeset OUT="$OUT/suite" bash tests/tooling/midend-pass-suite-check.sh || bad=$((bad + $?))
  if [ "$NAT" = 1 ]; then
    FID="$FID" PASS=rangeset EXTRA=--nat OUT="$OUT/suite-nat" bash tests/tooling/midend-pass-suite-check.sh || bad=$((bad + $?))
  fi
fi
echo "range-set-check: $bad failed"
exit $bad
