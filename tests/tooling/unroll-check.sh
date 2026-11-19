#!/bin/bash
# unroll-check.sh — OP11 (competitive-roadmap): loop unrolling of the
# mid-end (compiler/iropt_me.fd ime_unroll / ime_unroll_one, pass `unroll`,
# on by default since Nov 17: IME_DEFAULT_ON; FID_IROPT_OFF=unroll turns it
# off): full unrolling of a few trips, partial by 4 or 2, the remainder
# peeled in front of the header, trip counts known only at run time.
#
# Usage: tests/tooling/unroll-check.sh
#   FID=<compiler>   default release/fid.exe
#   OUT=<dir>        default build/unroll
#   NAT=1            also every output check through `emit --nat`
#   SUITE=1          then the whole suite with unroll on and off
#                    (midend-pass-suite-check.sh PASS=unroll; JOBS, EXTRA)
#   BENCH=1          then bench/ timed with unroll on and off
#                    (midend-pass-suite-check.sh BENCH=1 PASS=unroll)
#
# 1. It is on by default and does each kind: FID_IROPT_STATS=1 prints
#    "iropt: ivsr .., unroll F full, P partial, R peeled, T run-time";
#    without FID_IROPT_ON, F > 0 for test_unroll.fd, P > 0 for
#    test_midend_unroll_partial.fd, R > 0 for test_midend_unroll_remainder.fd
#    and test_midend_unroll_peel_fail.fd, T > 0 for
#    test_midend_unroll_runtime.fd and test_midend_unroll_runtime_fail.fd;
#    with FID_IROPT_OFF=unroll all four are 0.
# 2. The same output and exit status with unroll on (the default,
#    FID_VERIFY=1) and off, and the output of each test's "Expected output"
#    block; the _fail tests print 0 to 9, panic (non-zero exit) and never
#    print "unreachable".
# Exit status: the number of failed checks (+ the suite's and bench's).
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/unroll}"
NAT="${NAT:-0}"
SUITE="${SUITE:-0}"
BENCH="${BENCH:-0}"
mkdir -p "$OUT"
bad=0
ok() { echo "OK   $*"; }
no() { echo "FAIL $*"; bad=$((bad + 1)); }

expected() {
  sed -n '/^\/\/ Expected output:/,/^[^\/]/p' "$1" | sed -n 's/^\/\/ \(.*\)$/\1/p' | grep -v '^Expected output:'
}

build() { # <exe> <env...> -- <emit args...>
  local exe="$1"; shift
  local envs=()
  while [ "$1" != "--" ]; do envs+=("$1"); shift; done
  shift
  rm -f "$exe"
  env "${envs[@]}" "$FID" emit "$@" -o "$exe" > "$exe.log" 2>&1 < /dev/null && [ -f "$exe" ]
}

# stats <log> -> "F P R T"
stats() {
  tr -d '\r' < "$1" | sed -n 's/^iropt: ivsr .* unroll \([0-9]*\) full, \([0-9]*\) partial, \([0-9]*\) peeled, \([0-9]*\) run-time.*/\1 \2 \3 \4/p' | tail -1
}

# 1. counters: <test> <field 1-4> <name>
check_kind() {
  local t="tests/bootstrap/$1.fd" k="$2" what="$3" nm="$1"
  if ! build "$OUT/$nm.st.exe" X_UNROLL=default FID_IROPT_STATS=1 -- "$t"; then
    no "stats $nm: does not build ($(grep -m1 -i -E 'error|panic' "$OUT/$nm.st.exe.log"))"
    return
  fi
  local s v
  s=$(stats "$OUT/$nm.st.exe.log")
  v=$(echo "$s" | cut -d' ' -f"$k")
  if [ -n "$s" ] && [ "${v:-0}" -gt 0 ]; then ok "stats $nm: $what $v (default on)"; else no "stats $nm: '$s' (want $what > 0 by default)"; fi
  if build "$OUT/$nm.st0.exe" FID_IROPT_OFF=unroll FID_IROPT_STATS=1 -- "$t"; then
    s=$(stats "$OUT/$nm.st0.exe.log")
    if [ "$s" = "0 0 0 0" ]; then ok "stats $nm: nothing unrolled with unroll off"; else no "stats $nm off: '$s' (want 0 0 0 0)"; fi
  fi
}
check_kind test_unroll 1 full
check_kind test_midend_unroll_partial 2 partial
check_kind test_midend_unroll_remainder 3 peeled
check_kind test_midend_unroll_peel_fail 3 peeled
check_kind test_midend_unroll_runtime 4 run-time
check_kind test_midend_unroll_runtime_fail 4 run-time

# 2. outputs, on vs off
rounds=("pe_ir:")
[ "$NAT" = 1 ] && rounds+=("nat:--nat")
for r in "${rounds[@]}"; do
  label="${r%%:*}"; extra="${r#*:}"
  for t in tests/bootstrap/test_unroll.fd tests/bootstrap/test_midend_unroll_partial.fd \
           tests/bootstrap/test_midend_unroll_remainder.fd tests/bootstrap/test_midend_unroll_runtime.fd \
           tests/bootstrap/test_midend_unroll_peel_fail.fd tests/bootstrap/test_midend_unroll_runtime_fail.fd; do
    nm=$(basename "$t" .fd)
    for m in off on; do
      if [ $m = on ]; then e=(FID_VERIFY=1); else e=(FID_IROPT_OFF=unroll); fi
      exe="$OUT/$nm.$label.$m.exe"
      if build "$exe" "${e[@]}" -- "$t" $extra; then
        timeout 60 "$exe" > "$exe.out" 2>&1 < /dev/null
        echo "$?" > "$exe.rc"
      else
        echo BUILD > "$exe.rc"
      fi
    done
    off="$OUT/$nm.$label.off.exe"; on="$OUT/$nm.$label.on.exe"
    if [ "$(cat "$on.rc")" = BUILD ]; then no "$label $nm: does not build with unroll on"; continue; fi
    if [ "$(cat "$off.rc")" = BUILD ]; then no "$label $nm: does not build with unroll off"; continue; fi
    if grep -q "IR verifier" "$on.log"; then no "$label $nm: $(grep -m1 'IR verifier' "$on.log")"; continue; fi
    if [ "$(cat "$off.rc")" != "$(cat "$on.rc")" ] || ! cmp -s <(tr -d '\r' < "$off.out") <(tr -d '\r' < "$on.out"); then
      no "$label $nm: on and off differ (rc $(cat "$off.rc") / $(cat "$on.rc"))"
      diff <(tr -d '\r' < "$off.out") <(tr -d '\r' < "$on.out") | head -6 | sed 's/^/      /'
      continue
    fi
    case "$nm" in
      *_fail)
        got=$(tr -d '\r' < "$on.out" | grep -E '^[0-9]+$' | tr '\n' ' ')
        if [ "$(cat "$on.rc")" = 0 ] || grep -q unreachable "$on.out"; then
          no "$label $nm: no panic, or it went past it"
        elif [ "$got" != "0 1 2 3 4 5 6 7 8 9 " ]; then
          no "$label $nm: lines before the panic: '$got'"
        else
          ok "$label $nm: 0 to 9, then the panic, on and off (rc $(cat "$on.rc"))"
        fi
        ;;
      test_unroll)
        if [ "$(cat "$on.rc")" = 0 ] && grep -q 42 "$on.out"; then ok "$label $nm: 42, on and off"; else no "$label $nm: rc $(cat "$on.rc")"; fi
        ;;
      *)
        if cmp -s <(expected "$t") <(tr -d '\r' < "$on.out"); then
          ok "$label $nm: expected output, on and off"
        else
          no "$label $nm: not the expected output"
          diff <(expected "$t") <(tr -d '\r' < "$on.out") | head -6 | sed 's/^/      /'
        fi
        ;;
    esac
  done
done

if [ "$SUITE" = 1 ]; then
  FID="$FID" PASS=unroll OUT="$OUT/suite" bash tests/tooling/midend-pass-suite-check.sh || bad=$((bad + $?))
fi
if [ "$BENCH" = 1 ]; then
  FID="$FID" PASS=unroll BENCH=1 OUT="$OUT/bench" bash tests/tooling/midend-pass-suite-check.sh || bad=$((bad + $?))
fi
echo "unroll-check: $bad failed"
exit $bad
