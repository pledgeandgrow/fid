#!/bin/bash
# ssaexit-check.sh — ME23 (llvm-roadmap): exit values of counted loops on
# the mid-end's SSA form (compiler/ssa.fd ssa_exit_values, pass `ssaexit`
# with `ssa`, both on by default: compiler/iropt_me.fd IME_DEFAULT_ON).
# Since Nov 17 a bound that is not a constant counts too: after
# `while i != n` i is n; after `while i < n` (`<= n`) it is n (n + 1) when a
# guard on the loop's way in proves i starts at most n (ssa_guard_le).
#
# Usage: tests/tooling/ssaexit-check.sh
#   FID=<compiler>   default release/fid.exe
#   OUT=<dir>        default build/ssaexit
#   NAT=1            also every output check through `emit --nat`
#   SUITE=1          then the whole suite with ssaexit on and off
#                    (midend-pass-suite-check.sh PASS=ssaexit; with NAT=1
#                    also EXTRA=--nat)
#
# 1. FID_IROPT_STATS=1 prints "iropt: ssaexit C constant, B from a bound":
#    by default B > 0 for test_ssa_exit_bound.fd and C > 0 for
#    test_ssa_merge.fd; with FID_IROPT_OFF=ssaexit, "0 constant, 0".
# 2. test_ssa_exit_bound, test_ssa_exit_loop_tree, test_ssa_merge: the
#    same output and exit status by default (FID_VERIFY=1), with
#    FID_IROPT_OFF=ssaexit and with FID_IROPT_OFF=ssa,ssaexit, and the
#    test's "Expected output" block.
# Exit status: the number of failed checks (+ the suite's).
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/ssaexit}"
NAT="${NAT:-0}"
SUITE="${SUITE:-0}"
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

# stats <log> -> "C B"
stats() { tr -d '\r' < "$1" | sed -n 's/^iropt: ssaexit \([0-9]*\) constant, \([0-9]*\) from a bound.*/\1 \2/p' | tail -1; }

check_stat() { # <test> <field> <what>
  local nm="$1" k="$2" what="$3" t="tests/bootstrap/$1.fd" s v
  if ! build "$OUT/$nm.st.exe" FID_IROPT_STATS=1 -- "$t"; then
    no "stats $nm: does not build ($(grep -m1 -i -E 'error|panic' "$OUT/$nm.st.exe.log"))"
    return
  fi
  s=$(stats "$OUT/$nm.st.exe.log")
  v=$(echo "$s" | cut -d' ' -f"$k")
  if [ -n "$s" ] && [ "${v:-0}" -gt 0 ]; then ok "stats $nm: $what $v (default)"; else no "stats $nm: '$s' (want $what > 0)"; fi
  if build "$OUT/$nm.st0.exe" FID_IROPT_OFF=ssaexit FID_IROPT_STATS=1 -- "$t"; then
    s=$(stats "$OUT/$nm.st0.exe.log")
    if [ "$s" = "0 0" ]; then ok "stats $nm: none with ssaexit off"; else no "stats $nm off: '$s' (want 0 0)"; fi
  fi
}
check_stat test_ssa_exit_bound 2 "from a bound"
check_stat test_ssa_merge 1 "constant"

rounds=("pe_ir:")
[ "$NAT" = 1 ] && rounds+=("nat:--nat")
for r in "${rounds[@]}"; do
  label="${r%%:*}"; extra="${r#*:}"
  for nm in test_ssa_exit_bound test_ssa_exit_loop_tree test_ssa_merge; do
    t="tests/bootstrap/$nm.fd"
    [ -f "$t" ] || continue
    fail=0
    for m in def noexit nossa; do
      case $m in
        def) e=(FID_VERIFY=1) ;;
        noexit) e=(FID_IROPT_OFF=ssaexit) ;;
        nossa) e=(FID_IROPT_OFF=ssa,ssaexit) ;;
      esac
      exe="$OUT/$nm.$label.$m.exe"
      if build "$exe" "${e[@]}" -- "$t" $extra; then
        timeout 60 "$exe" > "$exe.out" 2>&1 < /dev/null
        echo "$?" > "$exe.rc"
      else
        echo BUILD > "$exe.rc"
        no "$label $nm: does not build ($m)"
        fail=1
      fi
    done
    [ $fail = 1 ] && continue
    d="$OUT/$nm.$label.def.exe"
    if grep -q "IR verifier" "$d.log"; then no "$label $nm: $(grep -m1 'IR verifier' "$d.log")"; continue; fi
    same=1
    for m in noexit nossa; do
      o="$OUT/$nm.$label.$m.exe"
      if [ "$(cat "$d.rc")" != "$(cat "$o.rc")" ] || ! cmp -s <(tr -d '\r' < "$d.out") <(tr -d '\r' < "$o.out"); then
        no "$label $nm: default and $m differ (rc $(cat "$d.rc") / $(cat "$o.rc"))"
        diff <(tr -d '\r' < "$d.out") <(tr -d '\r' < "$o.out") | head -6 | sed 's/^/      /'
        same=0
      fi
    done
    [ $same = 0 ] && continue
    if expected "$t" | grep -q .; then
      if cmp -s <(expected "$t") <(tr -d '\r' < "$d.out"); then
        ok "$label $nm: expected output, default / ssaexit off / ssa off"
      else
        no "$label $nm: not the expected output"
        diff <(expected "$t") <(tr -d '\r' < "$d.out") | head -6 | sed 's/^/      /'
      fi
    else
      ok "$label $nm: the same output, default / ssaexit off / ssa off (rc $(cat "$d.rc"))"
    fi
  done
done

if [ "$SUITE" = 1 ]; then
  FID="$FID" PASS=ssaexit OUT="$OUT/suite" bash tests/tooling/midend-pass-suite-check.sh || bad=$((bad + $?))
  if [ "$NAT" = 1 ]; then
    FID="$FID" PASS=ssaexit EXTRA=--nat OUT="$OUT/suite-nat" bash tests/tooling/midend-pass-suite-check.sh || bad=$((bad + $?))
  fi
fi
echo "ssaexit-check: $bad failed"
exit $bad
