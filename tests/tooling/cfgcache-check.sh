#!/bin/bash
# cfgcache-check.sh — the mid-end's shared analyses (competitive-roadmap OP3)
# must change nothing but compile time.
#
# compiler/checks.fd caches the CFG of a function shape (iro_cfg) and the
# loop tree found on it (iro_loops); compiler/iropt.fd caches the use lists
# between the passes of one function (iro_uses_entry), forgotten by every
# pass that changes the function. FID_IROPT_OFF=cfgcache turns all three
# off. For each test this emits the program with the caches on (the
# default) and off: the two executables must be byte-identical. With
# FID_IROPT_STATS=1 the "on" build prints
#   iropt: cfg built N, reused M
#   iropt: loops built N, reused M
#   iropt: uses built N, reused M
# and the totals over the tests are printed at the end; with the caches off
# every "reused" must be 0, and with them on each must be above 0 (else the
# cache is never hit, and the goal is not met).
#
# Usage: tests/tooling/cfgcache-check.sh [test.fd ...]
#   FID=<compiler>   default release/fid.exe
#   OUT=<dir>        default build/cfgcache
#   NAT=1            also compare `emit --nat` (the machine layer)
# Exit status: the number of tests whose executables differ or that build
# with one setting only, plus 1 when a cache was never hit / hit while off.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/cfgcache}"
NAT="${NAT:-0}"
mkdir -p "$OUT"
TESTS=("$@")
if [ ${#TESTS[@]} -eq 0 ]; then
  for t in midend_shared_analyses midend_loop_tree midend_unroll_partial \
           unroll counted_loop_step midend_iv_bounded midend_licm_guard \
           matrix_mul collections match_range struct_copy_vec generic_struct \
           vec_loop vec_math hashmap closure_capture lang_features opt_dce \
           verify_opt_dse_sccp_iv lib_bigint ssa_merge mem2reg_promote; do
    [ -f "tests/bootstrap/test_$t.fd" ] && TESTS+=("tests/bootstrap/test_$t.fd")
  done
fi
bad=0
declare -A on_tot off_tot
for k in cfg_b cfg_r loops_b loops_r uses_b uses_r; do on_tot[$k]=0; off_tot[$k]=0; done

# stat <log> <what> <field 1=built 2=reused>
stat() {
  local v
  v=$(grep -m1 "^iropt: $2 built" "$1" | sed -E 's/.*built ([0-9]+), reused ([0-9]+).*/\1 \2/' | cut -d' ' -f"$3")
  echo "${v:-0}"
}

add() {  # add <assoc name> <log>
  local -n acc="$1"
  acc[cfg_b]=$(( ${acc[cfg_b]} + $(stat "$2" cfg 1) ))
  acc[cfg_r]=$(( ${acc[cfg_r]} + $(stat "$2" cfg 2) ))
  acc[loops_b]=$(( ${acc[loops_b]} + $(stat "$2" loops 1) ))
  acc[loops_r]=$(( ${acc[loops_r]} + $(stat "$2" loops 2) ))
  acc[uses_b]=$(( ${acc[uses_b]} + $(stat "$2" uses 1) ))
  acc[uses_r]=$(( ${acc[uses_r]} + $(stat "$2" uses 2) ))
}

# one <label> <test> <emit args...>
one() {
  local label="$1" t="$2"; shift 2
  local n; n=$(basename "$t" .fd)
  local a="$OUT/$n.$label.on.exe" b="$OUT/$n.$label.off.exe"
  rm -f "$a" "$b"
  FID_IROPT_STATS=1 "$FID" emit "$t" "$@" -o "$a" > "$a.log" 2>&1
  FID_IROPT_STATS=1 FID_IROPT_OFF=cfgcache "$FID" emit "$t" "$@" -o "$b" > "$b.log" 2>&1
  if [ ! -f "$a" ] && [ ! -f "$b" ]; then
    printf 'SKIP  %-5s %s (builds with neither)\n' "$label" "$n"
    return
  fi
  if [ ! -f "$a" ] || [ ! -f "$b" ]; then
    printf 'BUILD %-5s %s (on: %s, off: %s)\n' "$label" "$n" "$([ -f "$a" ] && echo ok || echo fails)" "$([ -f "$b" ] && echo ok || echo fails)"
    bad=$((bad + 1))
    return
  fi
  if cmp -s "$a" "$b"; then
    printf 'SAME  %-5s %-34s cfg %s/%s loops %s/%s uses %s/%s (built/reused)\n' "$label" "$n" \
      "$(stat "$a.log" cfg 1)" "$(stat "$a.log" cfg 2)" "$(stat "$a.log" loops 1)" "$(stat "$a.log" loops 2)" \
      "$(stat "$a.log" uses 1)" "$(stat "$a.log" uses 2)"
  else
    printf 'DIFF  %-5s %s (executables differ)\n' "$label" "$n"
    bad=$((bad + 1))
  fi
}

for t in "${TESTS[@]}"; do
  one pe "$t"
  if [ "$NAT" = "1" ]; then
    one nat "$t" --nat
  fi
done | tee "$OUT/results.txt"
bad=$(grep -c -E '^(DIFF|BUILD)' "$OUT/results.txt")

# totals over this run's logs (the loop above ran in the pipe's subshell)
for t in "${TESTS[@]}"; do
  n=$(basename "$t" .fd)
  for f in "$OUT/$n".*.on.exe.log; do [ -f "$f" ] && add on_tot "$f"; done
  for f in "$OUT/$n".*.off.exe.log; do [ -f "$f" ] && add off_tot "$f"; done
done
echo "cache on:  cfg built ${on_tot[cfg_b]}, reused ${on_tot[cfg_r]}; loops built ${on_tot[loops_b]}, reused ${on_tot[loops_r]}; uses built ${on_tot[uses_b]}, reused ${on_tot[uses_r]}"
echo "cache off: cfg built ${off_tot[cfg_b]}, reused ${off_tot[cfg_r]}; loops built ${off_tot[loops_b]}, reused ${off_tot[loops_r]}; uses built ${off_tot[uses_b]}, reused ${off_tot[uses_r]}"
hit=0
if [ "${on_tot[cfg_r]}" -eq 0 ] || [ "${on_tot[loops_r]}" -eq 0 ] || [ "${on_tot[uses_r]}" -eq 0 ]; then
  echo "cfgcache-check: a cache was never hit with the caches on"
  hit=1
fi
if [ "${off_tot[cfg_r]}" -ne 0 ] || [ "${off_tot[loops_r]}" -ne 0 ] || [ "${off_tot[uses_r]}" -ne 0 ]; then
  echo "cfgcache-check: a cache was hit with FID_IROPT_OFF=cfgcache"
  hit=1
fi
echo "cfgcache-check: ${#TESTS[@]} tests, $bad differ or build with one setting only"
exit $((bad + hit))
