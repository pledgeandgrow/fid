#!/bin/bash
# me-measure.sh LABEL [FID] [FILE...]: what a mid-end change does
# (docs/roadmap/llvm-roadmap.md ME30), measured, kept in a file per change.
#
# Each program is built natively twice by the same compiler (`fid emit`,
# FID_IROPT_STATS=1): once with every iropt pass, once with the passes in
# $OFF off (FID_IROPT_OFF; default the passes of that change). From iropt's
# own report it takes the IR instructions after iropt and the time iropt
# took, and the time of the whole build; one line per program and the
# totals go to docs/perf/mid-end/LABEL.txt. The two builds' programs must
# print the same: a line says DIFF when they do not.
#
#   OFF=simplify,branchfold,dse ./scripts/me-measure.sh me12-14-15 release/fid.exe
#   ./scripts/me-measure.sh gvn release/fid.exe tests/bootstrap/test_gvn*.fd
set -u
LABEL="${1:?a label for the change}"
FID="${2:-release/fid.exe}"
shift 2 2>/dev/null || shift $#
OFF="${OFF:-simplify,branchfold,dse}"
FILES=("$@")
if [ ${#FILES[@]} -eq 0 ]; then
  # programs with output and no failure expected, no network or devices
  mapfile -t FILES < <(ls tests/bootstrap/*.fd | grep -v -e '_fail' -e 'sqlite' -e 'sdl' -e 'socket' -e 'http' -e '_ws' -e 'bare' -e 'bench_')
fi
mkdir -p docs/perf/mid-end
OUTF="docs/perf/mid-end/$LABEL.txt"
TMP=$(mktemp -d)
{
  echo "# $LABEL: iropt with every pass vs with $OFF off ($(date +%F), $(basename "$FID"))"
  echo "# program  instructions(on)  instructions(off)  iropt-ms(on)  iropt-ms(off)  build-ms(on)  build-ms(off)  same-output"
} > "$OUTF"
ton=0; toff=0; mon=0; moff=0; bon=0; boff=0; ndiff=0; nprog=0
for f in "${FILES[@]}"; do
  n=$(basename "$f" .fd)
  for mode in on off; do
    envoff=""
    [ $mode = off ] && envoff="$OFF"
    s=$(date +%s%N)
    FID_IROPT_OFF="$envoff" FID_IROPT_STATS=1 timeout 600 "$FID" emit "$f" -o "$TMP/$n.$mode.exe" > "$TMP/$n.$mode.log" 2>&1
    e=$(date +%s%N)
    echo $(( (e - s) / 1000000 )) > "$TMP/$n.$mode.ms"
    if [ -f "$TMP/$n.$mode.exe" ]; then
      timeout 60 "$TMP/$n.$mode.exe" > "$TMP/$n.$mode.out" 2>&1 < /dev/null
      echo "exit $?" >> "$TMP/$n.$mode.out"
    else
      echo "no build" > "$TMP/$n.$mode.out"
    fi
  done
  ion=$(grep -o 'instructions [0-9]* -> [0-9]*' "$TMP/$n.on.log" | tail -1 | awk '{print $4}')
  ioff=$(grep -o 'instructions [0-9]* -> [0-9]*' "$TMP/$n.off.log" | tail -1 | awk '{print $4}')
  tmon=$(grep -o 'in [0-9]* ms' "$TMP/$n.on.log" | tail -1 | awk '{print $2}')
  tmoff=$(grep -o 'in [0-9]* ms' "$TMP/$n.off.log" | tail -1 | awk '{print $2}')
  same="same"
  if ! cmp -s "$TMP/$n.on.out" "$TMP/$n.off.out"; then same="DIFF"; ndiff=$((ndiff + 1)); fi
  echo "$n ${ion:--} ${ioff:--} ${tmon:--} ${tmoff:--} $(cat "$TMP/$n.on.ms") $(cat "$TMP/$n.off.ms") $same" >> "$OUTF"
  nprog=$((nprog + 1))
  ton=$((ton + ${ion:-0})); toff=$((toff + ${ioff:-0}))
  mon=$((mon + ${tmon:-0})); moff=$((moff + ${tmoff:-0}))
  bon=$((bon + $(cat "$TMP/$n.on.ms"))); boff=$((boff + $(cat "$TMP/$n.off.ms")))
done
{
  echo "# totals over $nprog programs: instructions $ton (on) vs $toff (off); iropt $mon ms vs $moff ms; builds $bon ms vs $boff ms; $ndiff with different output"
} >> "$OUTF"
tail -1 "$OUTF"
rm -rf "${TMP:?}"
