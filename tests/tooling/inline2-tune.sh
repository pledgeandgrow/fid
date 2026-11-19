#!/bin/bash
# inline2-tune.sh — ME20 (llvm-roadmap): tune inline2's limits
# (compiler/iropt_me.fd IME_INLINE_MAX, IME_INLINE_ONCE_MAX, IME_CALLER_MAX,
# IME_SITES_MAX and OP5's IME_INLINE_FLAT_MAX, IME_CONST_ARG_BONUS,
# IME_GROWTH_PCT, IME_GROWTH_MIN, IME_CALLER_SLACK) against instruction
# growth, iropt time and run time, with one compiler: each setting is a
# FID_INLINE2_LIMITS value (ime_inline_limits_load), "default" the
# constants as they are, "off" inline2 off (FID_IROPT_OFF=inline2).
#
# Usage: tests/tooling/inline2-tune.sh [program.fd ...]
#   FID=<compiler>     default release/fid.exe
#   OUT=<dir>          default build/inline2-tune
#   REPORT=<file>      default docs/perf/mid-end/me20-inline2-tune.txt
#   SETTINGS="default;caller=2000;..."  the settings, `;` between them
#   RUNS=<n>           best of n runs of each executable (default 3)
#   GROW_PCT=<pct>     instructions after iropt at most this % of off (140)
#   TIME_PCT=<pct>     iropt time at most this % of off (150)
#   SLOW_PCT=<pct>     run time at most this % of off, plus 5 ms a program
#                      for runs too short to time (default 105)
#   EXTRA="--nat"      more `emit` arguments
#   no programs: the 13 of docs/perf/mid-end/me20-inline2.txt and the
#   benchmarks of bench/fides/
#
# For each setting and program: `fid emit` with FID_IROPT_STATS=1, then the
# executable's best run time; its output must equal the "off" build's. The
# report has one line per setting: DIFF (programs with different output),
# instructions after iropt, iropt ms, build ms, run ms, each total and as %
# of off, inline2's sites, and OK when within GROW_PCT, TIME_PCT and
# SLOW_PCT with no DIFF. Then the setting recommended: the fastest-running
# OK setting (fewest instructions on a tie). Exit status: 0 when "default"
# is OK, 1 otherwise (then set the constants to the recommended setting's
# values, or take inline2 out of IME_DEFAULT_ON when no setting is OK).
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/inline2-tune}"
REPORT="${REPORT:-docs/perf/mid-end/me20-inline2-tune.txt}"
RUNS="${RUNS:-3}"
GROW_PCT="${GROW_PCT:-140}"
TIME_PCT="${TIME_PCT:-150}"
SLOW_PCT="${SLOW_PCT:-105}"
EXTRA="${EXTRA:-}"
SETTINGS="${SETTINGS:-default;caller=2000;caller=1500,once=200;max=40,flat=16;growth=25;growth=25,caller=2000,once=250;max=40,flat=16,growth=25,caller=1500,once=200,sites=32}"
mkdir -p "$OUT" "$(dirname "$REPORT")"
PROGS=("$@")
if [ ${#PROGS[@]} -eq 0 ]; then
  for n in test_midend_inline_closure test_midend_spec_hot test_matrix_mul test_vec_math test_vec_loop test_struct_eq test_generics_full test_hashmap test_closure_capture test_lang_features test_lib_bigint test_opt_dce test_midend_effects_deep test_midend_inline2_limits; do
    [ -f "tests/bootstrap/$n.fd" ] && PROGS+=("tests/bootstrap/$n.fd")
  done
  for f in bench/fides/*.fd; do [ -f "$f" ] && PROGS+=("$f"); done
fi
now_ms() { echo $(( $(date +%s%N) / 1000000 )); }
best() { # best <exe>: the fastest of RUNS runs, in ms; output in <exe>.out
  local exe="$1" b=-1 i t0 t1 d
  for ((i = 0; i < RUNS; i++)); do
    t0=$(now_ms)
    timeout 120 "$exe" > "$exe.out" 2>&1 < /dev/null
    t1=$(now_ms)
    d=$((t1 - t0))
    if [ $b -lt 0 ] || [ $d -lt $b ]; then b=$d; fi
  done
  echo $b
}
# one <setting-index> <setting>: totals "insts iropt_ms build_ms run_ms sites diffs nobuild"
one() {
  local si="$1" set="$2" ti=0 tm=0 tb=0 tr=0 ts=0 nd=0 nb=0 p n tag s0 s1 l
  for p in "${PROGS[@]}"; do
    n=$(basename "$p" .fd)
    tag="$OUT/$n.s$si"
    rm -f "$tag.exe"
    s0=$(now_ms)
    case "$set" in
      off) FID_IROPT_OFF=inline2 FID_IROPT_STATS=1 "$FID" emit "$p" $EXTRA -o "$tag.exe" > "$tag.log" 2>&1 < /dev/null ;;
      default) FID_IROPT_STATS=1 "$FID" emit "$p" $EXTRA -o "$tag.exe" > "$tag.log" 2>&1 < /dev/null ;;
      *) FID_INLINE2_LIMITS="$set" FID_IROPT_STATS=1 "$FID" emit "$p" $EXTRA -o "$tag.exe" > "$tag.log" 2>&1 < /dev/null ;;
    esac
    s1=$(now_ms)
    if [ ! -f "$tag.exe" ]; then
      nb=$((nb + 1))
      echo "$n NOBUILD" >> "$OUT/s$si.lines"
      continue
    fi
    l=$(tr -d '\r' < "$tag.log" | sed -n 's/^iropt: instructions [0-9]* -> \([0-9]*\) in \([0-9]*\) ms.*/\1 \2/p' | tail -1)
    read -r ia im <<< "${l:-0 0}"
    local st
    st=$(tr -d '\r' < "$tag.log" | sed -n 's/^iropt: inline2 \([0-9]*\) sites.*/\1/p' | tail -1)
    local rt
    rt=$(best "$tag.exe")
    local same=same
    if [ "$set" != off ] && [ -f "$OUT/$n.s0.exe.out" ] && ! cmp -s <(tr -d '\r' < "$tag.exe.out") <(tr -d '\r' < "$OUT/$n.s0.exe.out"); then
      same=DIFF
      nd=$((nd + 1))
    fi
    echo "$n $ia $im $((s1 - s0)) $rt ${st:-0} $same" >> "$OUT/s$si.lines"
    ti=$((ti + ia)); tm=$((tm + im)); tb=$((tb + s1 - s0)); tr=$((tr + rt)); ts=$((ts + ${st:-0}))
  done
  echo "$ti $tm $tb $tr $ts $nd $nb"
}
IFS=';' read -r -a SETS <<< "$SETTINGS"
ALL=("off" "${SETS[@]}")
declare -a TOT
for i in "${!ALL[@]}"; do
  : > "$OUT/s$i.lines"
  TOT[$i]=$(one "$i" "${ALL[$i]}")
  echo "inline2-tune: ${ALL[$i]}: ${TOT[$i]}" >&2
done
read -r oi om ob orr os ond onb <<< "${TOT[0]}"
pct() { if [ "$2" -gt 0 ]; then echo $(( $1 * 100 / $2 )); else echo 0; fi; }
{
  echo "# me20-inline2-tune: inline2's limits (FID_INLINE2_LIMITS) vs inline2 off ($(date +%F), $(basename "$FID") $EXTRA)"
  echo "# ${#PROGS[@]} programs; OK = no DIFF, instructions <= $GROW_PCT%, iropt time <= $TIME_PCT%, run time <= $SLOW_PCT% of off"
  echo "# setting  diff  nobuild  insts(%off)  iropt-ms(%off)  build-ms(%off)  run-ms(%off)  sites  verdict"
  printf '%-60s %4s %4s %8s(%3s%%) %8s(%3s%%) %8s(%3s%%) %8s(%3s%%) %6s %s\n' off 0 "$onb" "$oi" 100 "$om" 100 "$ob" 100 "$orr" 100 0 base
} > "$REPORT"
rec=""; rec_rt=-1; rec_i=-1; def_ok=1
for i in "${!ALL[@]}"; do
  [ "$i" = 0 ] && continue
  read -r ti tm tb tr ts nd nb <<< "${TOT[$i]}"
  v=OK
  if [ "$nd" -gt 0 ] || [ "$nb" -gt "$onb" ]; then v=WRONG
  elif [ "$(pct "$ti" "$oi")" -gt "$GROW_PCT" ]; then v=GROWS
  elif [ "$(pct "$tm" "$om")" -gt "$TIME_PCT" ]; then v=SLOWBUILD
  elif [ "$tr" -gt $(( orr * SLOW_PCT / 100 + 5 * ${#PROGS[@]} )) ]; then v=SLOWRUN
  fi
  printf '%-60s %4s %4s %8s(%3s%%) %8s(%3s%%) %8s(%3s%%) %8s(%3s%%) %6s %s\n' "${ALL[$i]}" "$nd" "$nb" "$ti" "$(pct "$ti" "$oi")" "$tm" "$(pct "$tm" "$om")" "$tb" "$(pct "$tb" "$ob")" "$tr" "$(pct "$tr" "$orr")" "$ts" "$v" >> "$REPORT"
  if [ "${ALL[$i]}" = default ] && [ "$v" = OK ]; then def_ok=0; fi
  if [ "$v" = OK ]; then
    if [ "$rec_rt" -lt 0 ] || [ "$tr" -lt "$rec_rt" ] || { [ "$tr" -eq "$rec_rt" ] && [ "$ti" -lt "$rec_i" ]; }; then
      rec="${ALL[$i]}"; rec_rt=$tr; rec_i=$ti
    fi
  fi
done
{
  if [ -n "$rec" ]; then
    echo "# recommended: $rec (run $rec_rt ms, $rec_i instructions)"
  else
    echo "# recommended: none OK: take inline2 out of IME_DEFAULT_ON (compiler/iropt_me.fd)"
  fi
  echo "# per program (setting index: 0 off, then SETTINGS in order): name insts iropt-ms build-ms run-ms sites same"
  for i in "${!ALL[@]}"; do
    echo "## $i ${ALL[$i]}"
    cat "$OUT/s$i.lines"
  done
} >> "$REPORT"
cat "$REPORT" | sed -n '1,/^# per program/p'
exit $def_ok
