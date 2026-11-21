#!/bin/bash
# verify-clean-check.sh — known-bugs 2g.80, 2g.89, 2g.94, 2g.109: programs
# the IR verifier or the machine layer's allocation checker (FID_VERIFY=1)
# rejected, or that the default `fid emit` built wrong, built with the
# checks on through pe_ir (`fid emit`) and the machine layer (`--nat` on
# Windows, x86_64-linux elsewhere), and their output compared with
# `fid run`'s.
#   2g.80   test_unroll_dead_store: pe_ir's allocator gave a value live
#           into a block laid out before its definition (unrolled copies)
#           the register of a dead product (9900 for the sum 0..99)
#   2g.89   test_sccp_bitnot_min: iro_sccp folded ~i64::MIN by `0 - x - 1`
#           and the compiler panicked "Integer overflow on subtraction"
#   2g.109  test_ml_full: `forward` renamed a loss's load everywhere but in
#           its `backward`; test_verify_semantics_ext and
#           test_select_case_slots: a select's case variable in a block
#           that does not dominate a later case or select
#   2g.94   tests/nattools/selfhost_front_unit.fd: the zeroing of struct
#           results kept in registers (nat_ir ng_rf_promote) sat before the
#           moves of the incoming arguments and took an argument register
#           ("defines vN in argument register"), so the natively built
#           parser crashed in Parser_parse_unary_inner
#
# Usage: tests/tooling/verify-clean-check.sh
#   FID=<compiler>   default release/fid.exe
#   OUT=<dir>        default build/verify-clean
# Exit status: the number of failed checks.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/verify-clean}"
mkdir -p "$OUT"
bad=0
ok() { echo "OK   $*"; }
no() { echo "FAIL $*"; bad=$((bad + 1)); }

case "$(uname -s)" in
  MINGW*|MSYS*|CYGWIN*) NAT=(--nat); EXT=.exe ;;
  *) NAT=(--target x86_64-linux); EXT="" ;;
esac
[ "$(uname -m)" = x86_64 ] || [ -n "$EXT" ] || NAT=()

build() { # <exe> <file> [emit args...]: FID_VERIFY=1 emit
  local exe="$1" f="$2"; shift 2
  rm -f "$exe"
  FID_VERIFY=1 "$FID" emit "$f" "$@" -o "$exe" > "$exe.log" 2>&1 < /dev/null && [ -f "$exe" ]
}
clean() { tr -d '\r' | grep -v '^\[build\]\|^\[prof\]'; }

for t in test_unroll_dead_store test_sccp_bitnot_min test_select_case_slots test_ml_full test_verify_semantics_ext; do
  f=tests/bootstrap/$t.fd
  "$FID" run "$f" < /dev/null 2> /dev/null | clean > "$OUT/$t.run"
  rrc=${PIPESTATUS[0]}
  for side in pe nat; do
    args=()
    if [ $side = nat ]; then
      [ ${#NAT[@]} -gt 0 ] || continue
      args=("${NAT[@]}")
    fi
    exe="$OUT/$t.$side$EXT"
    if ! build "$exe" "$f" "${args[@]}"; then
      no "$t ($side): FID_VERIFY=1 build: $(grep -v '^\[' "$exe.log" | head -2 | tr '\n' ' ')"
      continue
    fi
    if [ -n "$EXT" ] || [ $side = pe ] || [ "$(uname -m)" = x86_64 ]; then
      "$exe" < /dev/null 2> /dev/null | clean > "$OUT/$t.$side.out"
      erc=${PIPESTATUS[0]}
      if [ "$erc" = "$rrc" ] && diff "$OUT/$t.run" "$OUT/$t.$side.out" > /dev/null; then
        ok "$t ($side): FID_VERIFY=1 builds it, output and exit status = fid run"
      else
        no "$t ($side): exit $erc (run $rrc), $(diff "$OUT/$t.run" "$OUT/$t.$side.out" | head -4 | tr '\n' ' ')"
      fi
    fi
  done
done

# 2g.94: the compiler's own front end, natively built with the checks on
F=tests/nattools/selfhost_front_unit.fd
IN=(tests/bootstrap/hello.fd compiler/token.fd)
if [ ${#NAT[@]} -gt 0 ] && [ -n "$EXT" ]; then
  if build "$OUT/front_nat$EXT" "$F" "${NAT[@]}" && build "$OUT/front_pe$EXT" "$F"; then
    "$OUT/front_nat$EXT" "${IN[@]}" 2>&1 | tr -d '\r' > "$OUT/front_nat.txt"
    "$OUT/front_pe$EXT" "${IN[@]}" 2>&1 | tr -d '\r' > "$OUT/front_pe.txt"
    if [ -s "$OUT/front_pe.txt" ] && grep -q ' fns, ' "$OUT/front_nat.txt" && diff "$OUT/front_pe.txt" "$OUT/front_nat.txt" > /dev/null; then
      ok "front end: FID_VERIFY=1 --nat build = the pe_ir image ($(head -1 "$OUT/front_nat.txt"))"
    else
      no "front end: --nat '$(head -3 "$OUT/front_nat.txt" | tr '\n' ' ')' vs pe_ir '$(head -1 "$OUT/front_pe.txt")'"
    fi
  else
    no "front end: FID_VERIFY=1 build: $(grep -hv '^\[' "$OUT/front_nat$EXT.log" "$OUT/front_pe$EXT.log" 2> /dev/null | head -2 | tr '\n' ' ')"
  fi
fi

echo "verify-clean-check: $bad failure(s)"
exit $bad
