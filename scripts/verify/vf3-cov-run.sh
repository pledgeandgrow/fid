#!/bin/bash
# vf3-cov-run.sh — the coverage-guided long run of roadmap VF3: no input may
# crash the compiler (lexer, parser, checkers, the IR optimizer's passes and
# the machine layer's).
#
#   FID=release/fid.exe scripts/verify/vf3-cov-run.sh
#
# 1. Builds a compiler with block counters (scripts/verify/cov_instrument.py
#    through scripts/verify/build-variant.sh, from a snapshot of the tree)
#    into $OUT/fid-cov.exe, unless it is there already. SPLIT (default 8)
#    units at -O2 when $FID has `split-c`; build-variant.sh falls back to one
#    unit at -O0 (about 17x slower to run) when the split fails.
# 2. Runs scripts/fuzz/fuzz_frontend.py with --cov-fid in each mode of MODES
#    (default: check, emit --nat, emit --target x86_64-linux), BATCHES
#    batches of BATCH inputs each, seeds SEED, SEED+1, ... The corpus that
#    reaches new IR blocks of the compiler is kept in $OUT/corpus-<mode>/
#    (with seen.bin, the blocks reached so far), so every batch, and a later
#    run of this script, goes on from it.
# 3. Writes $OUT/summary.txt: per mode, inputs, findings, blocks reached at
#    the start and the end, corpus kept; and every finding of
#    $OUT/find-<mode>/findings.tsv (crash-<n>.fd is the input, crash-<n>.out
#    the compiler's output).
#
# Environment: OUT (default build/vf3), MODES, BATCH (default 500), BATCHES
# (default: 6 for check, 2 for each emit mode), SEED (default 20261117),
# TIMEOUT (seconds per input; default 60 for check, 240 for emit), TREE (a
# snapshot to build from; default: a copy in $OUT/tree).
#
# Exit status 1 when a mode has a finding (a crash, an internal panic or a
# hang that happened twice), 2 when the instrumented compiler did not build.
# Record the run in docs/verification/ and file each finding in
# docs/verification/miscompile-log.md with a tests/verify/bugs/ program.
set -u
cd "$(dirname "$0")/../.."
FID="${FID:-release/fid.exe}"
case "$FID" in /*|?:*) ;; *) FID="$(pwd)/$FID" ;; esac
OUT="${OUT:-build/vf3}"
case "$OUT" in /*|?:*) ;; *) OUT="$(pwd)/$OUT" ;; esac
case "$(uname -s)" in
  MINGW*|MSYS*|CYGWIN*) MODES="${MODES:-check emit:nat emit:x86_64-linux}" ;;
  *) MODES="${MODES:-check emit:x86_64-linux}" ;;  # --nat is the Windows machine layer
esac
BATCH="${BATCH:-500}"
SEED="${SEED:-20261117}"
PY="${PYTHON:-python}"
command -v "$PY" > /dev/null 2>&1 || PY=python3
mkdir -p "$OUT"
COV="$OUT/fid-cov.exe"
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) ;; *) COV="$OUT/fid-cov" ;; esac

if [ ! -x "$COV" ]; then
  if [ -z "${TREE:-}" ]; then
    TREE="$OUT/tree"
    if [ ! -d "$TREE/compiler" ]; then
      mkdir -p "$TREE"
      cp -r compiler stdlib "$TREE/"
    fi
  fi
  echo "building the instrumented compiler from $TREE ..."
  INSTRUMENT=scripts/verify/cov_instrument.py SPLIT="${SPLIT:-8}" FID="$FID" \
    bash scripts/verify/build-variant.sh "$TREE" "$COV" || { echo "the instrumented build failed"; exit 2; }
fi
# the counters of this build (scripts/verify/coverage.py maps them back)
MAP="${COV%.exe}.c.inst.map"
[ -f "$MAP" ] && echo "counters: $(wc -l < "$MAP") ($MAP)"

status=0
for m in $MODES; do
  mode="${m%%:*}"; target=""
  [ "$m" != "$mode" ] && target="${m#*:}"
  tag="${mode}${target:+-$target}"
  if [ "$mode" = check ]; then n_b="${BATCHES:-6}"; to="${TIMEOUT:-60}"; else n_b="${BATCHES:-2}"; to="${TIMEOUT:-240}"; fi
  for b in $(seq 0 $((n_b - 1))); do
    s=$((SEED + b))
    echo "== $tag batch $b (seed $s, $BATCH inputs)"
    "$PY" scripts/fuzz/fuzz_frontend.py --fid "$FID" --cov-fid "$COV" -n "$BATCH" --seed "$s" \
      --mode "$mode" ${target:+--target "$target"} --timeout "$to" \
      --keep "$OUT/corpus-$tag" --out "$OUT/find-$tag" --summary "$OUT/sum-$tag-$s.json" \
      | tail -n 3
    rc=${PIPESTATUS[0]}
    [ "$rc" -ne 0 ] && status=1
  done
done

"$PY" - "$OUT" <<'PY' | tee "$OUT/summary.txt"
import glob, json, os, sys
out = sys.argv[1]
rows = {}
for p in sorted(glob.glob(os.path.join(out, "sum-*.json"))):
    d = json.load(open(p))
    tag = d["mode"] + ("-" + d["target"] if d["target"] else "")
    r = rows.setdefault(tag, {"inputs": 0, "batches": 0, "seconds": 0, "new": 0, "hangs": 0,
                              "unconfirmed": 0, "start": None, "end": 0, "grown": 0})
    r["inputs"] += d["inputs"]; r["batches"] += 1; r["seconds"] += d["seconds"]
    r["new"] += len(d["new_findings"]); r["hangs"] += d["hangs"]; r["unconfirmed"] += d["unconfirmed"]
    if r["start"] is None:
        r["start"] = d["blocks_at_start"]
    r["end"] = max(r["end"], d["blocks_reached"]); r["grown"] += d["corpus_grown"]
print(f"{'mode':22} {'inputs':>7} {'batches':>7} {'hours':>6} {'findings':>8} {'hangs':>5} "
      f"{'blocks start':>12} {'end':>7} {'corpus kept':>11}")
for tag, r in rows.items():
    print(f"{tag:22} {r['inputs']:7} {r['batches']:7} {r['seconds'] / 3600:6.1f} {r['new']:8} {r['hangs']:5} "
          f"{r['start'] or 0:12} {r['end']:7} {r['grown']:11}")
for f in sorted(glob.glob(os.path.join(out, "find-*", "findings.tsv"))):
    print(f"\n{os.path.relpath(f, out)}:")
    for line in open(f, encoding="utf-8"):
        print("  " + line.rstrip("\n"))
PY
echo "summary: $OUT/summary.txt (exit $status)"
exit "$status"
