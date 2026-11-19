#!/bin/bash
# bench/mixed.sh — pe-roadmap item 89: "benchmarks within 2x of the C build at
# -O0", measured on the mixed workload bench/mixed/mixed.fd (200k
# StringBuilder appends + 2M Vec push/get + 100k HashMap insert/get, ten
# rounds of it: one round is too close to the start-up time to compare).
#
#   bash bench/mixed.sh                  build both ways, best of RUNS (3)
#   FID=release/fid.exe RUNS=5 bash bench/mixed.sh
#   PHASES=0 bash bench/mixed.sh         the whole program only
#   O2=1 bash bench/mixed.sh             also a C -O2 column
#
# The two builds: `fid ccompile --cc-opt 0` (C at -O0; CC, default zig cc)
# and `fid emit` (pe_ir on Windows; on Linux x86-64 `--target x86_64-linux`,
# the machine layer, since a plain emit goes through C there). Each prints
# one checksum, which must be bench/mixed/reference.py's (404332460 for the
# whole program): a build that prints another is WRONG, not timed. With
# PHASES=1 (default) each part is also timed alone (MIXED_PHASE=sb|vec|map),
# so the table says which part a gap over 2x comes from.
#
# Output: a markdown table (time = best of RUNS, spread = (slowest -
# fastest) / fastest), then the ratio native / C -O0 of the whole program
# and "item 89: within 2x" or "item 89: OVER 2x". Exit 0 within 2x, 1 over,
# 2 when a build failed or printed a wrong value. A spread over 10% means
# the machine was not quiet: the footer gives the CPU load and how many
# other fid processes ran before and after.
set -u
cd "$(dirname "$0")/.." || exit 2
RUNS="${RUNS:-3}"
PHASES="${PHASES:-1}"
LIMIT="${LIMIT:-600}"
X=""
EMIT_FLAGS=""
case "$(uname -s)" in
  MINGW*|MSYS*|CYGWIN*) X=".exe" ;;
  Linux) [ "$(uname -m)" = x86_64 ] && EMIT_FLAGS="--target x86_64-linux" ;;
esac
if [ -z "${FID:-}" ]; then
  for c in release/fid$X release/fid-dev$X; do
    if [ -x "$c" ]; then FID="$c"; break; fi
  done
fi
[ -n "${FID:-}" ] || { echo "mixed.sh: no compiler (FID=<fid>)"; exit 2; }
if [ -z "${CC:-}" ] && command -v zig > /dev/null 2>&1; then export CC="zig cc"; fi
OUT=bench/out
mkdir -p "$OUT"
SRC=bench/mixed/mixed.fd
WANT_ALL=404332460
WANT_SB=673069247
WANT_VEC=577661634
WANT_MAP=514424807

cpu_load() {
  if [ -r /proc/loadavg ]; then cut -d' ' -f1-3 /proc/loadavg
  elif [ -n "$X" ] && command -v powershell.exe > /dev/null 2>&1; then
    echo "CPU $(powershell.exe -NoProfile -Command '(Get-CimInstance Win32_Processor | Measure-Object -Property LoadPercentage -Average).Average' 2>/dev/null | tr -d '\r')%"
  else echo "n/a"; fi
}
other_fids() {
  if [ -n "$X" ]; then
    tasklist 2> /dev/null | tr -d '\r' | grep -ciE '^(fid|fid-dev|stage[0-9]|fid[0-9a-z_-]*)\.exe' || true
  else
    pgrep -c -f '(^|/)fid( |$)' 2> /dev/null || echo 0
  fi
}

# best <exe> <phase> -> "secs spread output"
best() {
  local exe=$1 phase=$2 bestns="" worstns="" out="" t0 t1 ns o
  for _ in $(seq 1 "$RUNS"); do
    t0=$(date +%s%N)
    o=$(MIXED_PHASE="$phase" timeout "$LIMIT" "$exe" 2>&1 | tr -d '\r' | tail -1)
    t1=$(date +%s%N)
    ns=$((t1 - t0))
    if [ $((ns / 1000000000)) -ge "$LIMIT" ]; then echo "TIMEOUT 0 TIMEOUT"; return; fi
    out="$o"
    if [ -z "$bestns" ] || [ "$ns" -lt "$bestns" ]; then bestns=$ns; fi
    if [ -z "$worstns" ] || [ "$ns" -gt "$worstns" ]; then worstns=$ns; fi
  done
  local spread=0
  [ "$bestns" -gt 0 ] && spread=$(( (worstns - bestns) * 100 / bestns ))
  printf '%d.%03d %d %s\n' $((bestns / 1000000000)) $(((bestns / 1000000) % 1000)) "$spread" "$out"
}

load_before=$(cpu_load); fids_before=$(other_fids)
status=0
declare -A EXE
build() {
  # build <column> <command> [flags...]: fid takes the file before its flags
  local col=$1 cmd=$2; shift 2
  local exe="$OUT/mixed-$col$X"
  rm -f "$exe"
  if "$FID" "$cmd" "$SRC" "$@" -o "$exe" > "$OUT/mixed-$col.log" 2>&1 && [ -x "$exe" ]; then
    EXE[$col]=$exe
  else
    echo "mixed.sh: the $col build failed (see $OUT/mixed-$col.log):"; tail -5 "$OUT/mixed-$col.log"
    status=2
  fi
}
build c0 ccompile --cc-opt 0
# shellcheck disable=SC2086
build native emit $EMIT_FLAGS
COLS="c0 native"
if [ "${O2:-0}" = 1 ]; then build c2 ccompile --cc-opt 2; COLS="c0 c2 native"; fi
[ $status = 0 ] || exit 2

PH="all"
[ "$PHASES" = 1 ] && PH="all sb vec map"
declare -A T
header="| Part | C -O0 |"
sep="|---|---|"
[ "${O2:-0}" = 1 ] && { header="$header C -O2 |"; sep="$sep---|"; }
header="$header fid emit | native / C -O0 |"
sep="$sep---|---|"
rows=""
for ph in $PH; do
  case $ph in all) want=$WANT_ALL ;; sb) want=$WANT_SB ;; vec) want=$WANT_VEC ;; map) want=$WANT_MAP ;; esac
  row="| $ph |"
  for col in $COLS; do
    read -r secs spread o < <(best "${EXE[$col]}" "$ph")
    if [ "$o" = TIMEOUT ]; then
      T[$col.$ph]=""; row="$row over ${LIMIT} s |"; status=2
    elif [ "$o" != "$want" ]; then
      T[$col.$ph]=""; row="$row WRONG ($o) |"; status=2
    else
      T[$col.$ph]=$secs; row="$row $secs ±$spread% |"
    fi
  done
  a=${T[c0.$ph]}; b=${T[native.$ph]}
  if [ -n "$a" ] && [ -n "$b" ]; then
    r=$(awk -v a="$a" -v b="$b" 'BEGIN { if (a > 0) printf "%.2fx", b / a; else print "-" }')
  else r="-"; fi
  row="$row $r |"
  rows="$rows$row"$'\n'
done

echo "Mixed workload (pe-roadmap 89), $(date +%Y-%m-%d), $(uname -s) $(uname -m); FID=$FID; best of $RUNS; seconds"
echo
echo "$header"
echo "$sep"
printf '%s' "$rows"
echo
a=${T[c0.all]:-}; b=${T[native.all]:-}
if [ -n "$a" ] && [ -n "$b" ]; then
  ratio=$(awk -v a="$a" -v b="$b" 'BEGIN { printf "%.2f", b / a }')
  if awk -v r="$ratio" 'BEGIN { exit !(r <= 2.0) }'; then
    echo "item 89: within 2x (native / C -O0 = ${ratio}x)"
  else
    echo "item 89: OVER 2x (native / C -O0 = ${ratio}x): the part with the largest ratio above is where to profile"
    [ $status = 0 ] && status=1
  fi
fi
echo
echo "Load before: $(echo "$load_before"), other fid processes: $fids_before; after: $(cpu_load), other fid processes: $(other_fids)"
exit $status
