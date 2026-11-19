#!/bin/bash
# rotate-check.sh — ME23 (llvm-roadmap): loop rotation on the IR
# (compiler/iropt_me.fd ime_rotate, pass `rotate`, opt-in:
# FID_IROPT_ON=rotate), run by iropt.fd last on each function, so pe_ir
# and nat_ir (Linux targets and --nat) share it; checks.fd's counted-loop
# proof knows the rotated shape (peir_rotated_test).
#
# Usage: tests/tooling/rotate-check.sh
#   FID=<compiler>   default release/fid.exe
#   OUT=<dir>        default build/rotate
#   NAT=1            also every output check through `emit --nat`
#   SUITE=1          then the whole suite with rotate on and off
#                    (midend-pass-suite-check.sh PASS=rotate; with NAT=1
#                    also EXTRA=--nat)
#   BENCH=1          then bench/ timed with rotate on and off (pe_ir; with
#                    NAT=1 also --nat, whose vectorizer may not take a
#                    rotated loop)
#
# 1. FID_IROPT_STATS=1 prints "iropt: rotate N loops in M functions": N > 0
#    for test_midend_rotate.fd with FID_IROPT_ON=rotate, N = 0 without.
# 2. test_midend_rotate and loop-heavy tests print the same and exit the
#    same with rotate on (FID_VERIFY=1) and off; test_midend_rotate prints
#    its "Expected output" block.
# 3. With FID_IROPT_ON=rotate, `fid explain test_midend_rotate.fd sum_vec`
#    still lists the bounds check as removed by the counted loop (the
#    rotated shape is proved). Needs main.fd's explain to call iropt.fd
#    iro_explain_loops (see the roadmap's ME23/ME25 entries).
# Exit status: the number of failed checks (+ the suite's and bench's).
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/rotate}"
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
  env -u FID_IROPT_ON -u FID_IROPT_OFF "${envs[@]}" "$FID" emit "$@" -o "$exe" > "$exe.log" 2>&1 < /dev/null && [ -f "$exe" ]
}

nrot() { tr -d '\r' < "$1" | sed -n 's/^iropt: rotate \([0-9]*\) loops.*/\1/p' | tail -1; }

# 1. counters
t=tests/bootstrap/test_midend_rotate.fd
if build "$OUT/rot.st.exe" FID_IROPT_ON=rotate FID_IROPT_STATS=1 -- "$t"; then
  v=$(nrot "$OUT/rot.st.exe.log")
  if [ "${v:-0}" -gt 0 ]; then ok "stats: $v loops rotated with rotate on"; else no "stats: '$v' loops rotated (want > 0)"; fi
else
  no "stats: test_midend_rotate does not build with rotate on ($(grep -m1 -i -E 'error|panic' "$OUT/rot.st.exe.log"))"
fi
if build "$OUT/rot.st0.exe" FID_IROPT_STATS=1 -- "$t"; then
  v=$(nrot "$OUT/rot.st0.exe.log")
  if [ "${v:-1}" = 0 ]; then ok "stats: none rotated by default (opt-in)"; else no "stats default: '$v' (want 0)"; fi
fi

# 2. outputs, on vs off
rounds=("pe_ir:")
[ "$NAT" = 1 ] && rounds+=("nat:--nat")
for r in "${rounds[@]}"; do
  label="${r%%:*}"; extra="${r#*:}"
  for nm in test_midend_rotate test_counted_loop_step test_irswitch_version test_ssa_exit_bound \
            test_midend_unroll_runtime test_matrix_mul test_vec_loop test_vec_math test_collections \
            test_lib_bigint; do
    t="tests/bootstrap/$nm.fd"
    [ -f "$t" ] || continue
    for m in off on; do
      if [ $m = on ]; then e=(FID_IROPT_ON=rotate FID_VERIFY=1); else e=(FID_IROPT_OFF=rotate); fi
      exe="$OUT/$nm.$label.$m.exe"
      if build "$exe" "${e[@]}" -- "$t" $extra; then
        timeout 60 "$exe" > "$exe.out" 2>&1 < /dev/null
        echo "$?" > "$exe.rc"
      else
        echo BUILD > "$exe.rc"
      fi
    done
    off="$OUT/$nm.$label.off.exe"; on="$OUT/$nm.$label.on.exe"
    if [ "$(cat "$on.rc")" = BUILD ] && [ "$(cat "$off.rc")" = BUILD ]; then echo "SKIP $label $nm: builds with neither"; continue; fi
    if [ "$(cat "$on.rc")" = BUILD ]; then no "$label $nm: does not build with rotate on"; continue; fi
    if [ "$(cat "$off.rc")" = BUILD ]; then echo "SKIP $label $nm: builds only with rotate on"; continue; fi
    if grep -q "IR verifier" "$on.log"; then no "$label $nm: $(grep -m1 'IR verifier' "$on.log")"; continue; fi
    if [ "$(cat "$off.rc")" != "$(cat "$on.rc")" ] || ! cmp -s <(tr -d '\r' < "$off.out") <(tr -d '\r' < "$on.out"); then
      no "$label $nm: on and off differ (rc $(cat "$off.rc") / $(cat "$on.rc"))"
      diff <(tr -d '\r' < "$off.out") <(tr -d '\r' < "$on.out") | head -6 | sed 's/^/      /'
      continue
    fi
    if [ "$nm" = test_midend_rotate ]; then
      if cmp -s <(expected "$t") <(tr -d '\r' < "$on.out"); then ok "$label $nm: expected output, on and off"; else
        no "$label $nm: not the expected output"
        diff <(expected "$t") <(tr -d '\r' < "$on.out") | head -6 | sed 's/^/      /'
      fi
    else
      ok "$label $nm: the same, on and off (rc $(cat "$on.rc"))"
    fi
  done
done

# 3. the counted-loop proof on the rotated shape
env -u FID_IROPT_OFF FID_IROPT_ON=rotate "$FID" explain tests/bootstrap/test_midend_rotate.fd sum_vec > "$OUT/explain.on.txt" 2>&1
r_on=$(tr -d '\r' < "$OUT/explain.on.txt" | grep -c 'removed bounds check.*counted loop')
if [ "$r_on" -gt 0 ]; then
  ok "explain sum_vec: the bounds check removed by the counted loop with rotate on"
else
  no "explain sum_vec: no bounds check removed with rotate on ($(tr -d '\r' < "$OUT/explain.on.txt" | grep -m1 'bounds check'))"
fi

if [ "$SUITE" = 1 ]; then
  FID="$FID" PASS=rotate OUT="$OUT/suite" bash tests/tooling/midend-pass-suite-check.sh || bad=$((bad + $?))
  if [ "$NAT" = 1 ]; then
    FID="$FID" PASS=rotate EXTRA=--nat OUT="$OUT/suite-nat" bash tests/tooling/midend-pass-suite-check.sh || bad=$((bad + $?))
  fi
fi
if [ "$BENCH" = 1 ]; then
  FID="$FID" PASS=rotate BENCH=1 OUT="$OUT/bench" bash tests/tooling/midend-pass-suite-check.sh || bad=$((bad + $?))
  if [ "$NAT" = 1 ]; then
    FID="$FID" PASS=rotate BENCH=1 EXTRA=--nat OUT="$OUT/bench-nat" bash tests/tooling/midend-pass-suite-check.sh || bad=$((bad + $?))
  fi
fi
echo "rotate-check: $bad failed"
exit $bad
