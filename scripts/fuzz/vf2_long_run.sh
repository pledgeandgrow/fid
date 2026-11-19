#!/bin/bash
# vf2_long_run.sh — VF2 (competitive roadmap): the long differential run of
# scripts/fuzz/gen_vs_c.py, recorded. Random programs in three kinds (plain
# integers/structs/Vec, --ext, --ext2), each against its C program built by
# zig cc (Clang) and GCC (WSL on Windows, the host's elsewhere) at -O0 and
# -O2, on every Fides backend asked for; one line per seed goes to the log,
# and a summary table to $OUT/summary.md.
#
# Usage: bash scripts/fuzz/vf2_long_run.sh
#   FID=<compiler>       default release/fid.exe (release/fid off Windows)
#   N=<programs>         per kind, default 400 (3 kinds: 1200 programs)
#   SEED=<first seed>    default 20000 (plain), +100000 for --ext, +200000 for --ext2
#   BACKENDS=<list>      default run,ccompile,ccompile-O2,nat on Windows,
#                        run,ccompile,ccompile-O2,emit:x86_64-linux elsewhere
#                        ("all": every backend this host can run)
#   JOBS=<n>             seeds checked at once (default 4); each fid process
#                        needs ~0.5 GB
#   OUT=<dir>            default build/vf2-long-run
#   KINDS=<list>         default plain,ext,ext2
#
# Known bugs are stepped around (--avoid all, the default of gen_vs_c.py:
# M1, M2, M3, M12, M13, M14), so what differs is new or a V1 cause filed in
# docs/verification/miscompile-log.md. Group the kept failures with
#   python scripts/fuzz/triage.py $OUT/keep-<kind>
# and shrink one with scripts/fuzz/reduce.py.
#
# Exit status: 0 when every kind ran to its summary line (differences are
# findings, not a harness failure), 1 otherwise.
set -u
cd "$(dirname "$0")/../.." || exit 1
case "$(uname -s)" in
  MINGW*|MSYS*|CYGWIN*) FID="${FID:-release/fid.exe}"; DEFB="run,ccompile,ccompile-O2,nat" ;;
  *) FID="${FID:-release/fid}"; DEFB="run,ccompile,ccompile-O2,emit:x86_64-linux" ;;
esac
N="${N:-400}"
SEED="${SEED:-20000}"
BACKENDS="${BACKENDS:-$DEFB}"
JOBS="${JOBS:-4}"
OUT="${OUT:-build/vf2-long-run}"
KINDS="${KINDS:-plain,ext,ext2}"
PY=python3
"$PY" -c 1 > /dev/null 2>&1 || PY=python
export PYTHONIOENCODING=utf-8
mkdir -p "$OUT"
G=scripts/fuzz/gen_vs_c.py
SUM="$OUT/summary.md"
{
  echo "# VF2 long run, $(date -u +%Y-%m-%dT%H:%MZ)"
  echo
  echo "Compiler: \`$FID\` ($( ( "$FID" --version 2>/dev/null || echo "no --version" ) | head -n 1))."
  echo "Backends: $BACKENDS. C oracle: zig cc and gcc at -O0 and -O2. Jobs: $JOBS."
  echo
  echo "| Kind | Seeds | Programs | Differ from C | C oracle disagrees | By backend | Time |"
  echo "|------|-------|----------|---------------|--------------------|------------|------|"
} > "$SUM"
bad=0
IFS=',' read -r -a kinds <<< "$KINDS"
for k in "${kinds[@]}"; do
  case "$k" in
    plain) flag=""; s=$SEED ;;
    ext) flag="--ext"; s=$((SEED + 100000)) ;;
    ext2) flag="--ext2"; s=$((SEED + 200000)) ;;
    *) echo "unknown kind $k" >&2; bad=1; continue ;;
  esac
  log="$OUT/$k.log"
  t0=$(date +%s)
  # shellcheck disable=SC2086
  "$PY" "$G" -n "$N" --seed "$s" $flag --fid "$FID" --backends "$BACKENDS" --cc clang,gcc \
    --jobs "$JOBS" --log "$log" --keep "$OUT/keep-$k" > "$OUT/$k.out" 2>&1
  t1=$(date +%s)
  line=$(grep -E "^$N programs: " "$OUT/$k.out" | tail -n 1)
  if [ -z "$line" ]; then
    echo "$k: no summary line (see $OUT/$k.out)"
    echo "| $k | $s.. | $N | (did not finish: $OUT/$k.out) | | | $((t1 - t0)) s |" >> "$SUM"
    bad=1
    continue
  fi
  echo "$k: $line"
  d=$(echo "$line" | sed -E 's/^[0-9]+ programs: ([0-9]+) differ.*/\1/')
  o=$(echo "$line" | sed -E 's/.*differ from C, ([0-9]+) with a C oracle.*/\1/')
  by=$(echo "$line" | sed -E 's/.*\(by backend: (.*)\)$/\1/')
  echo "| $k | $s..$((s + N - 1)) | $N | $d | $o | $by | $((t1 - t0)) s |" >> "$SUM"
done
{
  echo
  echo "Seeds that differ (\`<seed> differs <backends>\`), from the logs:"
  echo
  for k in "${kinds[@]}"; do
    [ -f "$OUT/$k.log" ] || continue
    grep -h " differs " "$OUT/$k.log" | sed "s/^/- $k: /"
  done
} >> "$SUM"
echo "summary: $SUM"
exit $bad
