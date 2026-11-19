#!/bin/bash
# egplace-default-check.sh — ME8 (docs/roadmap/llvm-roadmap.md): placement
# in the e-graph pass (compiler/egraph.fd eg_place, Click's global code
# motion) is on by default on the native paths since Nov 17; FID_NAT_OFF=
# egplace or FID_IROPT_OFF=egplace turns it off, FID_NAT_OFF=egraph turns
# the whole e-graph (and so placement) off.
#
# Checks, one line each (PASS/FAIL), exit status the number of FAILs:
#  1. test_midend_egplace.fd built with no switch prints its expected lines,
#     FID_VERIFY=1 finds nothing, and FID_EGRAPH_STATS=1 prints
#     "[egplace] functions F, placed N, hoisted H, sunk S, skipped K" with
#     N > 0 and H > 0 (it hoisted something out of a loop);
#  2. with FID_NAT_OFF=egplace and with FID_IROPT_OFF=egplace: the same
#     lines, "[egplace] off", and the two executables are byte-identical;
#     the default one differs from them (placement did something);
#  3. with FID_IROPT_ON=egplace (its old opt-in name): byte-identical to
#     the default build (the default is the "on" build);
#  4. the suite: every program of PROGS (or ALL=1: every program of
#     tests/bootstrap that me-measure.sh would take) built three ways —
#     default (e-graph and placement), FID_NAT_OFF=egplace (e-graph, no
#     placement) and FID_NAT_OFF=egraph (no e-graph) — with FID_VERIFY=1;
#     the three must build, print the same and exit the same. A program
#     none of the three builds is SKIP (the machine layer cannot build it).
#     One line per program goes to $OUT/me8-egplace.txt: values placed,
#     hoisted, sunk, build ms and run ms of each build; SAVE=1 also writes
#     it to docs/perf/mid-end/me8-egplace.txt (the ME8 numbers).
#
# Usage: tests/tooling/egplace-default-check.sh [test.fd ...]
#   FID=<compiler>   default release/fid.exe
#   OUT=<dir>        default build/egplace
#   TARGET=<flags>   emit flags, default `--nat` (Windows x64 on the machine
#                    layer, where the e-graph runs); on Linux
#                    TARGET="--target x86_64-linux"
#   ALL=1            the whole suite instead of PROGS
#   SAVE=1           write docs/perf/mid-end/me8-egplace.txt
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-release/fid.exe}"
OUT="${OUT:-build/egplace}"
TARGET="${TARGET:---nat}"
ALL="${ALL:-0}"
SAVE="${SAVE:-0}"
mkdir -p "$OUT"
bad=0
pass() { echo "PASS $1"; }
fail() { echo "FAIL $1"; bad=$((bad + 1)); }

# The "Expected output:" lines of a test's header comment
want() {
  awk '/^\/\/ Expected output:/ {on=1; next} on && /^\/\/ / {sub(/^\/\/ /, ""); print; next} on {exit}' "$1"
}

# build <exe> <log> <env...>: emit $T with the given environment
build() {
  local exe="$1" log="$2"; shift 2
  rm -f "$exe"
  env -u FID_IROPT_ON -u FID_IROPT_OFF -u FID_NAT_OFF "$@" FID_VERIFY=1 FID_EGRAPH_STATS=1 \
    timeout 900 "$FID" emit "$T" $TARGET -o "$exe" > "$log" 2>&1
}

# run <exe> <out>: its output (no CR) and exit status
run() {
  timeout 120 "$1" < /dev/null 2>&1 | tr -d '\r' > "$2"
  echo "exit ${PIPESTATUS[0]}" >> "$2"
}

stat_of() { # stat_of <field> <log>: a number of the [egplace] line
  grep -F "[egplace] " "$2" | head -1 | sed -n "s/.*$1 \([0-9][0-9]*\).*/\1/p"
}

T=tests/bootstrap/test_midend_egplace.fd
want "$T" > "$OUT/want"
echo "exit 0" >> "$OUT/want"

# 1. default
build "$OUT/def.exe" "$OUT/def.log" X=1
if [ -f "$OUT/def.exe" ]; then
  run "$OUT/def.exe" "$OUT/def.out"
  if cmp -s "$OUT/def.out" "$OUT/want"; then pass "default build prints the expected lines"; else fail "default build output (see $OUT/def.out)"; fi
  placed=$(stat_of placed "$OUT/def.log"); hoisted=$(stat_of hoisted "$OUT/def.log")
  if [ "${placed:-0}" -gt 0 ] && [ "${hoisted:-0}" -gt 0 ]; then
    pass "placement on by default: placed $placed, hoisted $hoisted"
  else
    fail "placement on by default (no [egplace] line with placed > 0 and hoisted > 0 in $OUT/def.log)"
  fi
else
  fail "default build (see $OUT/def.log)"
fi

# 2. off, two ways
build "$OUT/off_nat.exe" "$OUT/off_nat.log" FID_NAT_OFF=egplace
build "$OUT/off_iro.exe" "$OUT/off_iro.log" FID_IROPT_OFF=egplace
for m in off_nat off_iro; do
  if [ -f "$OUT/$m.exe" ]; then
    run "$OUT/$m.exe" "$OUT/$m.out"
    if cmp -s "$OUT/$m.out" "$OUT/want"; then pass "$m prints the expected lines"; else fail "$m output (see $OUT/$m.out)"; fi
    if grep -qF "[egplace] off" "$OUT/$m.log"; then pass "$m: placement off"; else fail "$m: no '[egplace] off' in $OUT/$m.log"; fi
  else
    fail "$m build (see $OUT/$m.log)"
  fi
done
if [ -f "$OUT/off_nat.exe" ] && cmp -s "$OUT/off_nat.exe" "$OUT/off_iro.exe"; then
  pass "FID_NAT_OFF=egplace and FID_IROPT_OFF=egplace build the same executable"
else
  fail "FID_NAT_OFF=egplace and FID_IROPT_OFF=egplace differ"
fi
if [ -f "$OUT/def.exe" ] && [ -f "$OUT/off_nat.exe" ] && ! cmp -s "$OUT/def.exe" "$OUT/off_nat.exe"; then
  pass "placement changes test_midend_egplace's executable"
else
  fail "the default and egplace-off executables are the same (placement did nothing)"
fi

# 3. the old opt-in name
build "$OUT/on.exe" "$OUT/on.log" FID_IROPT_ON=egplace
if [ -f "$OUT/on.exe" ] && cmp -s "$OUT/on.exe" "$OUT/def.exe"; then
  pass "FID_IROPT_ON=egplace builds what the default builds"
else
  fail "FID_IROPT_ON=egplace differs from the default build"
fi

# 4. the suite
PROGS=("$@")
if [ ${#PROGS[@]} -eq 0 ]; then
  if [ "$ALL" = 1 ]; then
    mapfile -t PROGS < <(ls tests/bootstrap/*.fd | grep -v -e '_fail' -e 'sqlite' -e 'sdl' -e 'socket' -e 'http' -e '_ws' -e 'bare' -e 'bench_')
  else
    for n in midend_egplace midend_egraph_cast ssa_merge mem2reg_promote unroll counted_loop_step \
             midend_iv_bounded matrix_mul collections match_range struct_copy_vec vec_loop hashmap \
             closure_capture opt_dce lib_bigint midend_shared_analyses midend_quietcall checked_arith \
             crypto_vectors; do
      [ -f "tests/bootstrap/test_$n.fd" ] && PROGS+=("tests/bootstrap/test_$n.fd")
    done
  fi
fi
TAB="$OUT/me8-egplace.txt"
{
  echo "# me8-egplace: e-graph placement (compiler/egraph.fd eg_place) on by default"
  echo "# vs FID_NAT_OFF=egplace vs FID_NAT_OFF=egraph ($(date +%F), $(basename "$FID"), emit $TARGET)"
  echo "# program  placed  hoisted  sunk  build-ms(def/noplace/noegraph)  run-ms(def/noplace/noegraph)  result"
} > "$TAB"
ndiff=0; nskip=0; nsame=0; tpl=0; tho=0; tsu=0
for T in "${PROGS[@]}"; do
  n=$(basename "$T" .fd)
  bms=(); rms=(); ok=1; built=0
  for mode in def noplace noegraph; do
    case $mode in
      def) envs=(X=1) ;;
      noplace) envs=(FID_NAT_OFF=egplace) ;;
      noegraph) envs=(FID_NAT_OFF=egraph) ;;
    esac
    s=$(date +%s%N)
    build "$OUT/$n.$mode.exe" "$OUT/$n.$mode.log" "${envs[@]}"
    e=$(date +%s%N)
    bms+=($(( (e - s) / 1000000 )))
    if [ -f "$OUT/$n.$mode.exe" ]; then
      built=$((built + 1))
      s=$(date +%s%N)
      run "$OUT/$n.$mode.exe" "$OUT/$n.$mode.out"
      e=$(date +%s%N)
      rms+=($(( (e - s) / 1000000 )))
    else
      echo "no build" > "$OUT/$n.$mode.out"
      rms+=(-)
    fi
  done
  pl=$(stat_of placed "$OUT/$n.def.log"); ho=$(stat_of hoisted "$OUT/$n.def.log"); su=$(stat_of sunk "$OUT/$n.def.log")
  if [ $built -eq 0 ]; then
    res=SKIP; nskip=$((nskip + 1))
  elif [ $built -ne 3 ]; then
    res=BUILD; ok=0
  elif cmp -s "$OUT/$n.def.out" "$OUT/$n.noplace.out" && cmp -s "$OUT/$n.def.out" "$OUT/$n.noegraph.out"; then
    res=same; nsame=$((nsame + 1))
  else
    res=DIFF; ok=0
  fi
  # (a verifier error after the e-graph stops the build: it shows as BUILD)
  [ $ok = 0 ] && ndiff=$((ndiff + 1))
  tpl=$((tpl + ${pl:-0})); tho=$((tho + ${ho:-0})); tsu=$((tsu + ${su:-0}))
  echo "$n ${pl:--} ${ho:--} ${su:--} ${bms[0]}/${bms[1]}/${bms[2]} ${rms[0]}/${rms[1]}/${rms[2]} $res" >> "$TAB"
  echo "  $n: $res (placed ${pl:--})"
done
echo "# total: ${#PROGS[@]} programs, $nsame same, $ndiff DIFF/BUILD, $nskip skipped; placed $tpl, hoisted $tho, sunk $tsu" >> "$TAB"
if [ $ndiff -eq 0 ]; then
  pass "suite: $nsame programs print the same with placement on, off and with the e-graph off ($nskip skipped)"
else
  fail "suite: $ndiff programs differ or fail to build (FID_VERIFY=1) (see $TAB)"
fi
if [ "$SAVE" = 1 ]; then
  mkdir -p docs/perf/mid-end
  cp "$TAB" docs/perf/mid-end/me8-egplace.txt
  echo "wrote docs/perf/mid-end/me8-egplace.txt"
fi
exit $bad
