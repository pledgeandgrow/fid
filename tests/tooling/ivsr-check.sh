#!/bin/bash
# ivsr-check.sh — OP10 (competitive-roadmap): the mid-end's strength
# reduction (compiler/iropt_me.fd ime_strength_reduce, pass `ivsr`, opt-in)
# also outside @wrapping code, where checks.fd iro_iv_bounded proves that
# no value of the stepped copy of `i * c` can overflow.
#
# Usage: tests/tooling/ivsr-check.sh
#   FID=<compiler>   default release/fid.exe
#   OUT=<dir>        default build/ivsr
#   NAT=1            also every check through `emit --nat`
#   SUITE=1          then the whole suite with ivsr on and off
#                    (midend-pass-suite-check.sh PASS=ivsr; JOBS, EXTRA)
#
# 1. It does something: with FID_IROPT_ON=ivsr FID_IROPT_OFF=iv (iropt's own
#    `iv` off, so the products are left to ivsr) FID_IROPT_STATS=1 prints
#    "iropt: ivsr N products (M outside @wrapping), ..." with M > 0 for
#    test_midend_ivsr_bounded.fd; with ivbound off too, M = 0 and N > 0
#    (the @wrapping function is still reduced).
# 2. The same output and exit status with ivsr on (FID_VERIFY=1) and off,
#    and the output each test's "Expected output" block lists; the _fail
#    tests exit non-zero, print their lines up to the panic and never
#    "unreachable".
# Exit status: the number of failed checks (+ the suite's).
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/ivsr}"
NAT="${NAT:-0}"
SUITE="${SUITE:-0}"
mkdir -p "$OUT"
bad=0
ok() { echo "OK   $*"; }
no() { echo "FAIL $*"; bad=$((bad + 1)); }

# expected <test.fd>: the lines of its "// Expected output:" block
expected() {
  sed -n '/^\/\/ Expected output:/,/^[^\/]/p' "$1" | sed -n 's/^\/\/ \(.*\)$/\1/p' | grep -v '^Expected output:'
}

# build <exe> <env...> -- <emit args...>
build() {
  local exe="$1"; shift
  local envs=()
  while [ "$1" != "--" ]; do envs+=("$1"); shift; done
  shift
  rm -f "$exe"
  env "${envs[@]}" "$FID" emit "$@" -o "$exe" > "$exe.log" 2>&1 < /dev/null && [ -f "$exe" ]
}

stats() { # <log> -> "N M"
  tr -d '\r' < "$1" | sed -n 's/^iropt: ivsr \([0-9]*\) products (\([0-9]*\) outside @wrapping).*/\1 \2/p' | tail -1
}

# 1. counters
t=tests/bootstrap/test_midend_ivsr_bounded.fd
if build "$OUT/stats.exe" FID_IROPT_ON=ivsr FID_IROPT_OFF=iv FID_IROPT_STATS=1 -- "$t"; then
  read -r n m <<< "$(stats "$OUT/stats.exe.log")"
  if [ "${m:-0}" -gt 0 ]; then ok "stats ivsr: ${n:-0} products, $m outside @wrapping"; else no "stats ivsr: '${n:-} ${m:-}' (want M > 0)"; fi
else
  no "stats build: $(grep -m1 -i -E 'error|panic' "$OUT/stats.exe.log")"
fi
if build "$OUT/stats2.exe" FID_IROPT_ON=ivsr FID_IROPT_OFF=iv,ivbound FID_IROPT_STATS=1 -- "$t"; then
  read -r n m <<< "$(stats "$OUT/stats2.exe.log")"
  if [ "${m:--1}" = 0 ] && [ "${n:-0}" -gt 0 ]; then ok "stats ivbound off: $n products, 0 outside @wrapping"; else no "stats ivbound off: '${n:-} ${m:-}' (want N > 0, M = 0)"; fi
else
  no "stats build (ivbound off): $(grep -m1 -i -E 'error|panic' "$OUT/stats2.exe.log")"
fi

# 2. outputs, on vs off
rounds=("pe_ir:")
[ "$NAT" = 1 ] && rounds+=("nat:--nat")
for r in "${rounds[@]}"; do
  label="${r%%:*}"; extra="${r#*:}"
  for t in tests/bootstrap/test_midend_ivsr_bounded.fd tests/bootstrap/test_midend_iv_bounded.fd \
           tests/bootstrap/test_midend_ivsr_bounded_fail.fd tests/bootstrap/test_midend_iv_bounded_fail.fd; do
    nm=$(basename "$t" .fd)
    for m in off on; do
      if [ $m = on ]; then e=(FID_IROPT_ON=ivsr FID_VERIFY=1); else e=(FID_IROPT_OFF=ivsr); fi
      exe="$OUT/$nm.$label.$m.exe"
      if build "$exe" "${e[@]}" -- "$t" $extra; then
        timeout 60 "$exe" > "$exe.out" 2>&1 < /dev/null
        echo "$?" > "$exe.rc"
      else
        echo BUILD > "$exe.rc"
      fi
    done
    off="$OUT/$nm.$label.off.exe"; on="$OUT/$nm.$label.on.exe"
    if [ "$(cat "$on.rc")" = BUILD ]; then no "$label $nm: does not build with ivsr on"; continue; fi
    if [ "$(cat "$off.rc")" = BUILD ]; then no "$label $nm: does not build with ivsr off"; continue; fi
    if [ "$(cat "$off.rc")" != "$(cat "$on.rc")" ] || ! cmp -s <(tr -d '\r' < "$off.out") <(tr -d '\r' < "$on.out"); then
      no "$label $nm: on and off differ (rc $(cat "$off.rc") / $(cat "$on.rc"))"
      continue
    fi
    case "$nm" in
      *_fail)
        if [ "$(cat "$on.rc")" = 0 ] || grep -q unreachable "$on.out"; then
          no "$label $nm: no panic, or it went past it"
        else
          ok "$label $nm: same panic on and off (rc $(cat "$on.rc"))"
        fi
        ;;
      *)
        if cmp -s <(expected "$t") <(tr -d '\r' < "$on.out" | grep -v '^\[') ; then
          ok "$label $nm: expected output, on and off"
        else
          no "$label $nm: not the expected output"
          diff <(expected "$t") <(tr -d '\r' < "$on.out") | head -6 | sed 's/^/      /'
        fi
        ;;
    esac
  done
done
# the _fail tests' lines before the panic
for t in tests/bootstrap/test_midend_ivsr_bounded_fail.fd; do
  nm=$(basename "$t" .fd)
  got=$(tr -d '\r' < "$OUT/$nm.pe_ir.on.exe.out" 2>/dev/null | grep -E '^[0-9]+$' | tr '\n' ' ')
  if [ "$got" = "0 1000 2000 3000 4000 5000 " ]; then ok "$nm: the six lines before the panic"; else no "$nm: lines before the panic: '$got'"; fi
done

if [ "$SUITE" = 1 ]; then
  FID="$FID" PASS=ivsr OUT="$OUT/suite" bash tests/tooling/midend-pass-suite-check.sh || bad=$((bad + $?))
fi
echo "ivsr-check: $bad failed"
exit $bad
