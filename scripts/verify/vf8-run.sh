#!/bin/bash
# vf8-run.sh — VF8 (docs/roadmap/competitive-roadmap.md): line and branch
# coverage of compiler/*.fd from the suite, per module, recorded.
#
#   bash scripts/verify/vf8-run.sh
#
# 1. the no-build self-tests of cov_instrument.py and coverage.py;
# 2. the counter compiler: scripts/verify/build-variant.sh builds the tree's
#    compiler with FID_C_LINE_MARKS=1 (the C generator marks each
#    instruction's source line) and scripts/verify/cov_instrument.py over
#    its C (a counter at every function entry, IR block and marked line),
#    SPLIT units at -O2 (it falls back to one unit at -O0 by itself);
# 3. a smoke test of the counter compiler (hello.fd runs, a bitmap is
#    written);
# 4. coverage.py: every test in TESTS through it in each of MODES, plus
#    GEN programs of each fuzzer, then the tool commands (TOOL_MODES) on a
#    sample; the report (blocks = branches at the IR level, functions,
#    source lines) goes to $OUT/report.txt and, dated, to RECORD;
# 5. the goal: every module of the map (the modules main.fd reaches, and
#    core) at GOAL% of its blocks and of its lines. $OUT/goal.txt lists the
#    modules below it; $OUT/low-modules.txt has, for each, its uncovered
#    line count and never-called functions: where tests are missing.
#
# Environment:
#   FID      the compiler that builds the counter compiler (default
#            release/fid.exe; release/fid off Windows)
#   OUT      work folder (default build/vf8); the counter compiler is
#            $OUT/fid-cov.exe, its map $OUT/fid-cov.c.inst.map
#   SKIP_BUILD=1   reuse $OUT/fid-cov.exe (and keep $OUT/merged.cov: a
#            second run adds to the first, e.g. more MODES)
#   TESTS    comma list of globs (default the suite, its subfolders and
#            tests/conformance)
#   MODES    default check,run,ccompile,emit,nat and emit for the three
#            Linux targets (only the compiler runs: no target binary)
#   GEN      fuzzer programs of each generator (default 50)
#   LIMIT    only the first LIMIT tests (default 0: all)
#   TOOL_MODES, TOOL_TESTS, TOOL_LIMIT  the tool commands (coverage.py's
#            `cmd:` modes: fmt --check, lint, explain, audit, wasm, the IR
#            and machine-IR views, ...) on the first TOOL_LIMIT (default
#            80) tests of TOOL_TESTS; TOOL_MODES=none: not run
#   J        tests at a time (default 4)
#   GOAL     percent (default 90)
#   RECORD   results file (default docs/verification/vf8-coverage.md;
#            RECORD=none: not recorded)
#   SPLIT, JOBS, CC, DUMP_C_FLAGS: passed to build-variant.sh
#
# Exit status: 0 when every module reaches GOAL (blocks and lines), 1 when
# some do not (the run itself worked), 2 when a step failed.
set -u
cd "$(dirname "$0")/../.." || exit 2
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WIN=1 ;; *) WIN=0 ;; esac
if [ $WIN = 1 ]; then FID="${FID:-release/fid.exe}"; else FID="${FID:-release/fid}"; fi
OUT="${OUT:-build/vf8}"
TESTS="${TESTS:-tests/bootstrap/*.fd,tests/bootstrap/*/*.fd,tests/conformance/*.fd}"
MODES="${MODES:-check,run,ccompile,emit,nat,emit:x86_64-linux,emit:aarch64-linux,emit:riscv64-linux}"
GEN="${GEN:-50}"
LIMIT="${LIMIT:-0}"
# (single quotes: a `}` inside ${..:-..} would end it)
DEFAULT_TOOLS='cmd:dump-tokens,cmd:dump-ast,cmd:dump-ir,cmd:opt,cmd:lint,cmd:explain,cmd:explain+--alloc,cmd:audit,cmd:fmt+--check,cmd:fix+--check,cmd:sourcemap,cmd:qualify+-o+{out},cmd:wasm+-o+{out},cmd:dump-mir,cmd:disasm'
TOOL_MODES="${TOOL_MODES:-$DEFAULT_TOOLS}"
TOOL_TESTS="${TOOL_TESTS:-tests/bootstrap/test_*.fd}"
TOOL_LIMIT="${TOOL_LIMIT:-80}"
J="${J:-4}"
GOAL="${GOAL:-90}"
RECORD="${RECORD:-docs/verification/vf8-coverage.md}"
export CC="${CC:-zig cc}"
PY=python3; command -v python3 > /dev/null 2>&1 || PY=python
mkdir -p "$OUT"
EXE="$OUT/fid-cov.exe"
MAP="$OUT/fid-cov.c.inst.map"
step() { echo "== $*"; }

step "1 self-tests"
"$PY" scripts/verify/cov_instrument.py --self-test || exit 2
"$PY" scripts/verify/coverage.py --self-test || exit 2

FRESH=--fresh
if [ "${SKIP_BUILD:-}" = 1 ] && [ -x "$EXE" ] && [ -s "$MAP" ]; then
  step "2 counter compiler: reusing $EXE"
  FRESH=
else
  step "2 counter compiler from the tree, built by $FID"
  t0=$(date +%s)
  FID_C_LINE_MARKS=1 INSTRUMENT=scripts/verify/cov_instrument.py FID="$FID" SPLIT="${SPLIT:-8}" \
    bash scripts/verify/build-variant.sh . "$EXE" || { echo "the counter compiler did not build"; exit 2; }
  echo "built in $(( $(date +%s) - t0 )) s"
fi
[ -s "$MAP" ] || { echo "no counter map $MAP"; exit 2; }
nlines=$(grep -c ' L[0-9]*$' "$MAP")
echo "$(wc -l < "$MAP") counters, $nlines on source lines"
if [ "$nlines" = 0 ]; then
  echo "WARNING: no source-line counters: the C generator wrote no line marks (FID_C_LINE_MARKS, codegen_c.fd)"
  echo "         and no #line (DUMP_C_FLAGS=-g with a dump-c that takes -g). Lines are an estimate."
fi

step "3 smoke test"
rm -f "$OUT/smoke.cov"
smoke=$(FID_COV_OUT="$OUT/smoke.cov" "$EXE" run tests/bootstrap/hello.fd 2>&1 | tr -d '\r')
if [ -z "$smoke" ] || [ ! -s "$OUT/smoke.cov" ]; then
  echo "the counter compiler does not run hello.fd or writes no bitmap: '$smoke'"
  exit 2
fi
echo "hello.fd: $(echo "$smoke" | head -1); bitmap $(wc -c < "$OUT/smoke.cov") bytes"

step "4 the suite through the counter compiler ($MODES)"
args=(--fid "$EXE" --map "$MAP" --modes "$MODES" --tests "$TESTS" --gen "$GEN" -j "$J" --out "$OUT")
[ "$LIMIT" != 0 ] && args+=(--limit "$LIMIT")
[ -n "$FRESH" ] && args+=("$FRESH")
t0=$(date +%s)
"$PY" scripts/verify/coverage.py "${args[@]}" > "$OUT/run.log" 2>&1 || { tail -20 "$OUT/run.log"; exit 2; }
echo "ran in $(( $(date +%s) - t0 )) s"

# the tool modules (fmt, lint, explain, audit, the IR and machine-IR
# views, wasm, ...) on a sample: they add to the bitmap of step 4
if [ "$TOOL_MODES" != none ]; then
  step "4b the tools on $TOOL_LIMIT tests ($TOOL_MODES)"
  t0=$(date +%s)
  "$PY" scripts/verify/coverage.py --fid "$EXE" --map "$MAP" --modes "$TOOL_MODES" --tests "$TOOL_TESTS" \
    --limit "$TOOL_LIMIT" -j "$J" --out "$OUT" > "$OUT/run-tools.log" 2>&1 || { tail -20 "$OUT/run-tools.log"; exit 2; }
  echo "ran in $(( $(date +%s) - t0 )) s"
fi
note="$( [ "$LIMIT" != 0 ] && echo "the first $LIMIT of " )$TESTS x $MODES, $GEN programs of each fuzzer"
[ "$TOOL_MODES" != none ] && note="$note; $TOOL_LIMIT tests of $TOOL_TESTS x $TOOL_MODES"
rargs=(--fid "$EXE" --map "$MAP" --out "$OUT" --report-only --note "$note")
[ "$RECORD" != none ] && rargs+=(--record "$RECORD")
"$PY" scripts/verify/coverage.py "${rargs[@]}" > "$OUT/report.log" 2>&1 || { tail -20 "$OUT/report.log"; exit 2; }
cat "$OUT/report.txt"

step "5 the $GOAL% goal, every module of the map"
gargs=(--map "$MAP" --out "$OUT" --report-only --goal-modules all --goal "$GOAL")
[ "$nlines" != 0 ] && gargs+=(--goal-lines "$GOAL")
"$PY" scripts/verify/coverage.py "${gargs[@]}" > "$OUT/goal.txt" 2>&1
grc=$?
grep 'below the\|every goal\|no source-line' "$OUT/goal.txt"
# where tests are missing: the low modules, worst first, with their
# uncovered lines and never-called functions
"$PY" - "$OUT" <<'PY'
import os, re, sys
out = sys.argv[1]
low = {}
for line in open(os.path.join(out, "goal.txt")):
    m = re.match(r"below the [0-9.]+% (line )?goal: (\S+) ([0-9.]+)%", line)
    if m:
        low.setdefault(m.group(2), {})["lines" if m.group(1) else "blocks"] = float(m.group(3))
unc = {}
p = os.path.join(out, "uncovered-lines.txt")
if os.path.exists(p):
    for line in open(p):
        mod = line.rsplit(":", 1)[0]
        unc[mod] = unc.get(mod, 0) + 1
never = {}
p = os.path.join(out, "never-called.txt")
if os.path.exists(p):
    for line in open(p):
        mod, fn = line.split()
        never.setdefault(mod, []).append(fn)
rows = sorted(low.items(), key=lambda kv: min(kv[1].values()))
with open(os.path.join(out, "low-modules.txt"), "w") as f:
    for mod, d in rows:
        fns = never.get(mod, [])
        f.write(f"{mod}: blocks {d.get('blocks', 'ok')}%, lines {d.get('lines', 'ok')}%, "
                f"{unc.get(mod, 0)} uncovered lines, {len(fns)} functions never called: {' '.join(fns[:40])}"
                f"{' ...' if len(fns) > 40 else ''}\n")
print(f"{len(rows)} modules below the goal (low-modules.txt)")
PY
case $grc in
  0) echo "vf8: every module at or above $GOAL%"; exit 0 ;;
  1) echo "vf8: modules below $GOAL% (see $OUT/low-modules.txt)"; exit 1 ;;
  *) echo "vf8: goal check failed ($grc): $(tail -3 "$OUT/goal.txt" | tr '\n' ' ')"; exit 2 ;;
esac
