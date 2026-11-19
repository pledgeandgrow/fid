#!/bin/bash
# version-default-check.sh — ME25 (llvm-roadmap): loop versioning
# (compiler/iropt.fd iro_version, pass `version`) is on by default since
# Nov 17 (`version` in compiler/iropt_me.fd IME_DEFAULT_ON;
# FID_IROPT_OFF=version turns it off).
#
# Usage: tests/tooling/version-default-check.sh
#   FID=<compiler>   default release/fid.exe
#   OUT=<dir>        default build/version
#   NAT=1            also every output check through `emit --nat`
#   SUITE=1          then the whole suite with version on and off
#                    (midend-pass-suite-check.sh PASS=version; with NAT=1
#                    also EXTRA=--nat)
#   BENCH=1          then bench/ timed with version on and off
#
# 1. On by default: FID_IROPT_STATS=1 prints "... versioned loops N
#    functions changed" with N > 0 for test_irswitch_version,
#    test_counted_loop_step, test_version_oob_fail and
#    test_version_negstart_fail without FID_IROPT_ON; N = 0 with
#    FID_IROPT_OFF=version.
# 2. The same output and exit status with version on (the default,
#    FID_VERIFY=1) and off; the tests' "Expected output" blocks; the _fail
#    tests panic (non-zero exit) after the lines given below and never print
#    "unreachable": input the guard refuses runs the checked loop.
# 3. `fid explain test_irswitch_version.fd sum_first` lists a bounds check
#    removed by the counted loop with version on (the copy's) and one kept
#    (the original's); with FID_IROPT_OFF=version none is removed. Needs
#    main.fd's explain to call iropt.fd iro_explain_loops (see the
#    roadmap's ME25 entry); without it this check fails.
# Exit status: the number of failed checks (+ the suite's and bench's).
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/version}"
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
  env -u FID_IROPT_ON "${envs[@]}" "$FID" emit "$@" -o "$exe" > "$exe.log" 2>&1 < /dev/null && [ -f "$exe" ]
}

nver() { tr -d '\r' < "$1" | sed -n 's/.*versioned loops \([0-9]*\).*/\1/p' | tail -1; }

# 1. counters
for nm in test_irswitch_version test_counted_loop_step test_version_oob_fail test_version_negstart_fail; do
  t="tests/bootstrap/$nm.fd"
  if build "$OUT/$nm.st.exe" FID_IROPT_STATS=1 -- "$t"; then
    v=$(nver "$OUT/$nm.st.exe.log")
    if [ "${v:-0}" -gt 0 ]; then ok "stats $nm: $v loops versioned by default"; else no "stats $nm: '${v}' loops versioned (want > 0 by default)"; fi
  else
    no "stats $nm: does not build ($(grep -m1 -i -E 'error|panic' "$OUT/$nm.st.exe.log"))"
  fi
  if build "$OUT/$nm.st0.exe" FID_IROPT_OFF=version FID_IROPT_STATS=1 -- "$t"; then
    v=$(nver "$OUT/$nm.st0.exe.log")
    if [ "${v:-1}" = 0 ]; then ok "stats $nm: none versioned with version off"; else no "stats $nm off: '$v' (want 0)"; fi
  fi
done

# 2. outputs, on vs off
rounds=("pe_ir:")
[ "$NAT" = 1 ] && rounds+=("nat:--nat")
for r in "${rounds[@]}"; do
  label="${r%%:*}"; extra="${r#*:}"
  for nm in test_irswitch_version test_counted_loop_step test_version_oob_fail test_version_negstart_fail; do
    t="tests/bootstrap/$nm.fd"
    for m in off on; do
      if [ $m = on ]; then e=(FID_VERIFY=1); else e=(FID_IROPT_OFF=version); fi
      exe="$OUT/$nm.$label.$m.exe"
      if build "$exe" "${e[@]}" -- "$t" $extra; then
        timeout 60 "$exe" > "$exe.out" 2>&1 < /dev/null
        echo "$?" > "$exe.rc"
      else
        echo BUILD > "$exe.rc"
      fi
    done
    off="$OUT/$nm.$label.off.exe"; on="$OUT/$nm.$label.on.exe"
    if [ "$(cat "$on.rc")" = BUILD ]; then no "$label $nm: does not build with version on"; continue; fi
    if [ "$(cat "$off.rc")" = BUILD ]; then no "$label $nm: does not build with version off"; continue; fi
    if grep -q "IR verifier" "$on.log"; then no "$label $nm: $(grep -m1 'IR verifier' "$on.log")"; continue; fi
    if [ "$(cat "$off.rc")" != "$(cat "$on.rc")" ] || ! cmp -s <(tr -d '\r' < "$off.out" | grep -v -i 'panic\|stack\|called from\|line ') <(tr -d '\r' < "$on.out" | grep -v -i 'panic\|stack\|called from\|line '); then
      no "$label $nm: on and off differ (rc $(cat "$off.rc") / $(cat "$on.rc"))"
      diff <(tr -d '\r' < "$off.out") <(tr -d '\r' < "$on.out") | head -6 | sed 's/^/      /'
      continue
    fi
    case "$nm" in
      test_version_oob_fail|test_version_negstart_fail)
        want="1 2 3 4 5 "
        [ "$nm" = test_version_negstart_fail ] && want="12 "
        got=$(tr -d '\r' < "$on.out" | grep -E '^-?[0-9]+$' | tr '\n' ' ')
        if [ "$(cat "$on.rc")" = 0 ] || grep -q unreachable "$on.out"; then
          no "$label $nm: no panic, or it went past it"
        elif [ "$got" != "$want" ]; then
          no "$label $nm: lines before the panic: '$got' (want '$want')"
        elif ! grep -q -i 'out of bounds' "$on.out"; then
          no "$label $nm: the panic is not the bounds check's"
        else
          ok "$label $nm: '$want', then the bounds panic, on and off (rc $(cat "$on.rc"))"
        fi
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

# 3. fid explain: the copy's checks removed, the original's kept
t=tests/bootstrap/test_irswitch_version.fd
env -u FID_IROPT_ON "$FID" explain "$t" sum_first > "$OUT/explain.on.txt" 2>&1
env -u FID_IROPT_ON FID_IROPT_OFF=version "$FID" explain "$t" sum_first > "$OUT/explain.off.txt" 2>&1
r_on=$(tr -d '\r' < "$OUT/explain.on.txt" | grep -c 'removed bounds check.*counted loop')
k_on=$(tr -d '\r' < "$OUT/explain.on.txt" | grep -c 'kept bounds check')
r_off=$(tr -d '\r' < "$OUT/explain.off.txt" | grep -c 'removed bounds check.*counted loop')
if [ "$r_on" -gt 0 ] && [ "$k_on" -gt 0 ] && [ "$r_off" = 0 ]; then
  ok "explain sum_first: $r_on bounds check(s) removed in the copy, $k_on kept in the original; none removed with version off"
else
  no "explain sum_first: removed $r_on / kept $k_on with version on, removed $r_off off (want >0 / >0 / 0)"
fi

if [ "$SUITE" = 1 ]; then
  FID="$FID" PASS=version OUT="$OUT/suite" bash tests/tooling/midend-pass-suite-check.sh || bad=$((bad + $?))
  if [ "$NAT" = 1 ]; then
    FID="$FID" PASS=version EXTRA=--nat OUT="$OUT/suite-nat" bash tests/tooling/midend-pass-suite-check.sh || bad=$((bad + $?))
  fi
fi
if [ "$BENCH" = 1 ]; then
  FID="$FID" PASS=version BENCH=1 OUT="$OUT/bench" bash tests/tooling/midend-pass-suite-check.sh || bad=$((bad + $?))
fi
echo "version-default-check: $bad failed"
exit $bad
