#!/bin/bash
# midend-optin-check.sh — the opt-in mid-end passes on and off must give the
# same output and exit status (llvm-roadmap ME22, ME24, ME25, ME26, WP10).
#
# Usage: tests/tooling/midend-optin-check.sh [test.fd ...]
#   FID=<compiler>       default release/fid.exe
#   OUT=<dir>            default build/optin
#   IME="loops,unroll,sra,version,paramrange"  FID_IROPT_ON of the "on"
#                        pe_ir build (paramrange is on by default: the
#                        "off" build names it in FID_IROPT_OFF, compiler/
#                        checks.fd)
#   RULES="readcse"      FID_RULES_ON of the "on" `emit --nat` build
#                        (RULES="" skips the --nat rounds: they take twice
#                        as long)
#   no test files: the default list below (loop- and struct-heavy programs)
#
# For each test: `emit` with every pass of IME named off (FID_IROPT_OFF) and
# on (FID_IROPT_ON, FID_VERIFY=1 so the IR verifier runs after them), then
# the same for `emit --nat` with RULES off (FID_NAT_OFF) and on; the two
# executables of a round must print the same and exit with the same status.
# Since the passes may be on by default, "off" names them in FID_IROPT_OFF /
# FID_NAT_OFF rather than leaving FID_IROPT_ON empty.
#
# One line per test and round (SAME, DIFF, BUILD = a build failed with the
# pass on only); exit status: the number of DIFF and BUILD lines.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/optin}"
IME="${IME-loops,unroll,sra,version,paramrange}"
RULES="${RULES-readcse}"
mkdir -p "$OUT"
TESTS=("$@")
if [ ${#TESTS[@]} -eq 0 ]; then
  for t in unroll midend_unroll_partial midend_unroll_runtime midend_unroll_remainder \
           irswitch_version counted_loop_step midend_effects_deep midend_loop_body_shared \
           midend_iv_bounded midend_licm_guard matrix_mul collections \
           match_range struct_copy_vec struct_eq struct_reassign generic_struct \
           generics_full vec_loop vec_math vec_builtins vec_filled vec_f64 \
           hashmap hashmap_int_keys closure_capture lang_features grid_safe \
           opt_dce verify_opt_dse_sccp_iv lib_bigint str_reclaim xml_spec; do
    [ -f "tests/bootstrap/test_$t.fd" ] && TESTS+=("tests/bootstrap/test_$t.fd")
  done
fi
bad=0

# build_run <tag> <exe> <emit args...> : prints "<rc>" and leaves output in <exe>.out
build_run() {
  local exe="$1"; shift
  rm -f "$exe" "$exe.out"
  if ! "$FID" emit "$@" -o "$exe" > "$exe.log" 2>&1 || [ ! -f "$exe" ]; then
    echo "BUILD"
    return
  fi
  timeout 60 "$exe" > "$exe.out" 2>&1
  echo "$?"
}

# round <label> <offvars> <onvars> <test> <extra emit args...>
round() {
  local label="$1" offv="$2" onv="$3" t="$4"; shift 4
  local n; n=$(basename "$t" .fd)
  local a b
  a=$(env $offv bash -c "$(declare -f build_run); FID='$FID' build_run '$OUT/$n.$label.off.exe' '$t' $*")
  b=$(env $onv FID_VERIFY=1 bash -c "$(declare -f build_run); FID='$FID' build_run '$OUT/$n.$label.on.exe' '$t' $*")
  if [ "$a" = "BUILD" ] && [ "$b" = "BUILD" ]; then
    printf 'SKIP  %-8s %s (builds with neither)\n' "$label" "$n"
  elif [ "$b" = "BUILD" ]; then
    printf 'BUILD %-8s %s (on: %s)\n' "$label" "$n" "$(grep -m1 -i -E 'error|panic|invalid' "$OUT/$n.$label.on.exe.log")"
    bad=$((bad + 1))
  elif [ "$a" = "BUILD" ]; then
    printf 'SKIP  %-8s %s (builds only with the passes on)\n' "$label" "$n"
  elif [ "$a" = "$b" ] && cmp -s "$OUT/$n.$label.off.exe.out" "$OUT/$n.$label.on.exe.out"; then
    printf 'SAME  %-8s %s (rc %s)\n' "$label" "$n" "$a"
  else
    printf 'DIFF  %-8s %s (rc off %s, on %s)\n' "$label" "$n" "$a" "$b"
    diff "$OUT/$n.$label.off.exe.out" "$OUT/$n.$label.on.exe.out" | head -6 | sed 's/^/      /'
    bad=$((bad + 1))
  fi
}

for t in "${TESTS[@]}"; do
  if [ -n "$IME" ]; then
    round pe_ir "FID_IROPT_OFF=$IME" "FID_IROPT_ON=$IME" "$t"
  fi
  if [ -n "$RULES" ]; then
    round nat "FID_NAT_OFF=$RULES" "FID_RULES_ON=$RULES" "$t" --nat
  fi
done | tee "$OUT/results.txt"
bad=$(grep -c -E '^(DIFF|BUILD)' "$OUT/results.txt")
echo "midend-optin-check: ${#TESTS[@]} tests, $bad differ or fail to build"
exit "$bad"
