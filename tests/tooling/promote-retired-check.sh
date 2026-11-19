#!/bin/bash
# promote-retired-check.sh — llvm-roadmap ME3: iropt's `promote`
# (compiler/iropt.fd iro_promote) is retired where the SSA round trip
# (compiler/ssa.fd, `ssa`, on by default) runs; it still runs on the
# functions ssa does not take (ssa_takes: @constant_time, TryUnwrap/Asm, an
# entry block with predecessors), everywhere with FID_IROPT_OFF=ssa, and
# everywhere when FID_IROPT_ON names `promote` (iro_promote_here).
#
# For each test, `emit` (pe_ir) three ways, FID_VERIFY=1 and
# FID_IROPT_STATS=1:
#   def   no switch                         (ssa on, promote retired)
#   prom  FID_IROPT_ON=promote              (promote on every function too)
#   nossa FID_IROPT_OFF=ssa,ssaexit         (the pipeline before ssa)
# All three must build, print the same and exit the same, with no IR
# verifier error. Then:
#   - RETIRED: on the programs in RETIRED_TESTS (no @constant_time, asm!
#     or TryUnwrap function), the default build's `promote N functions
#     changed` must be 0 (it was 2 on test_collections and test_hashmap and
#     4 on test_lib_bigint with the Nov 16 compiler, ssa named on);
#   - COST: the default's instruction count after iropt ("iropt:
#     instructions A -> B") must be within 1% of the `prom` build's (ssa
#     subsumes promote: what promote still found after ssa was 5-6
#     instructions of 6,000-10,000).
# NAT=1 also runs the three builds through `emit --nat` (output only).
#
# Usage: tests/tooling/promote-retired-check.sh [test.fd ...]
#   FID=<compiler>  default release/fid.exe
#   OUT=<dir>       default build/promote-retired
# One line per test (OK, DIFF, BUILD, VERIFY, RETIRED, COST); exit status:
# the number of failing tests.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/promote-retired}"
NAT="${NAT:-0}"
mkdir -p "$OUT"
RETIRED_TESTS=" test_collections test_hashmap test_lib_bigint test_mem2reg_promote test_ssa_merge test_ssa_exit_loop_tree test_matrix_mul test_vec_loop "
TESTS=("$@")
if [ ${#TESTS[@]} -eq 0 ]; then
  for t in mem2reg_retired mem2reg_promote ssa_merge ssa_exit_loop_tree ssa_validate \
           collections hashmap lib_bigint matrix_mul vec_loop constant_time \
           counted_loop_step midend_loop_body_shared midend_loop_tree lang_features; do
    [ -f "tests/bootstrap/test_$t.fd" ] && TESTS+=("tests/bootstrap/test_$t.fd")
  done
fi

# stat_of <log> : "promote N" count of the iropt summary line ("" if none)
promote_of() { sed -n 's/^iropt: fold.* promote \([0-9]*\),.*/\1/p' "$1" | tail -1; }
# insts_of <log> : the instruction count after iropt
insts_of() { sed -n 's/^iropt: instructions [0-9]* -> \([0-9]*\) .*/\1/p' "$1" | tail -1; }

bad=0
# one <label> <test> <emit args...>
one() {
  local label="$1" t="$2"; shift 2
  local n; n=$(basename "$t" .fd)
  local d="$OUT/$n.$label.def.exe" p="$OUT/$n.$label.prom.exe" s="$OUT/$n.$label.nossa.exe"
  rm -f "$d" "$p" "$s"
  env -u FID_IROPT_ON -u FID_IROPT_OFF FID_VERIFY=1 FID_IROPT_STATS=1 "$FID" emit "$t" "$@" -o "$d" > "$d.log" 2>&1
  env -u FID_IROPT_OFF FID_IROPT_ON=promote FID_VERIFY=1 FID_IROPT_STATS=1 "$FID" emit "$t" "$@" -o "$p" > "$p.log" 2>&1
  env -u FID_IROPT_ON FID_IROPT_OFF=ssa,ssaexit FID_VERIFY=1 FID_IROPT_STATS=1 "$FID" emit "$t" "$@" -o "$s" > "$s.log" 2>&1
  if [ ! -f "$d" ] || [ ! -f "$p" ] || [ ! -f "$s" ]; then
    echo "BUILD   $label $n (default $([ -f "$d" ] && echo ok || echo fails), promote named $([ -f "$p" ] && echo ok || echo fails), ssa off $([ -f "$s" ] && echo ok || echo fails))"
    return 1
  fi
  if grep -q "IR verifier" "$d.log" "$p.log" "$s.log"; then
    echo "VERIFY  $label $n ($(grep -h "IR verifier" "$d.log" "$p.log" "$s.log" | head -1))"
    return 1
  fi
  local r1 r2 r3
  timeout 60 "$d" > "$d.out" 2>&1; r1=$?
  timeout 60 "$p" > "$p.out" 2>&1; r2=$?
  timeout 60 "$s" > "$s.out" 2>&1; r3=$?
  if [ "$r1" != "$r2" ] || [ "$r1" != "$r3" ] || ! cmp -s "$d.out" "$p.out" || ! cmp -s "$d.out" "$s.out"; then
    echo "DIFF    $label $n (output or exit status: default $r1, promote named $r2, ssa off $r3)"
    return 1
  fi
  if [ "$label" = "pe" ]; then
    local pd pp id ip
    pd=$(promote_of "$d.log"); pp=$(promote_of "$p.log")
    id=$(insts_of "$d.log"); ip=$(insts_of "$p.log")
    case "$RETIRED_TESTS" in
      *" $n "*)
        if [ "${pd:-x}" != "0" ]; then
          echo "RETIRED $label $n (promote changed ${pd:-?} functions in the default build: not retired where ssa runs)"
          return 1
        fi ;;
    esac
    if [ -n "$id" ] && [ -n "$ip" ] && [ $((id * 100)) -gt $((ip * 101)) ]; then
      echo "COST    $label $n (instructions after iropt: default $id, promote named $ip: more than 1% more)"
      return 1
    fi
    echo "OK      $label $n (rc $r1; promote: default ${pd:-?} / named ${pp:-?} functions; instructions $id vs $ip)"
  else
    echo "OK      $label $n (rc $r1)"
  fi
  return 0
}

for t in "${TESTS[@]}"; do
  one pe "$t" || bad=$((bad + 1))
  if [ "$NAT" = "1" ]; then
    one nat "$t" --nat || bad=$((bad + 1))
  fi
done
echo "promote-retired-check: ${#TESTS[@]} tests, $bad failing"
exit "$bad"
