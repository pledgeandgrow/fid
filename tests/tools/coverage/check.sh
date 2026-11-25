#!/usr/bin/env bash
# tools/coverage.fd (roadmap AD4) prints and writes what
# scripts/verify/coverage.py does: both run with the same arguments and
# their stdout+stderr, exit status and the files of --out (report.txt,
# never-called.txt, uncovered-lines.txt, merged.cov) are compared ("\r\n"
# from Python on Windows counts as "\n").
#
#   bash tests/tools/coverage/check.sh       # a minute or two
#
# The counter compiler is a stand-in, tests/tools/coverage/fakefid.fd: its
# C (dump-c, FID_C_LINE_MARKS=1 for line counters, or without for blocks
# only) instrumented by tools/cov_instrument.fd and built with $CC, so the
# runs are real (each child writes its bitmap into FID_COV_DIR) and take
# seconds; the tests are the three programs of sample/. The cases: a run
# with every kind of mode (check, run, emit, emit:<target>, cmd:<cmd>+<arg>)
# and the line and block goals; a second run adding to the first's
# merged.cov; 51 runs (the progress lines at 50); --limit and -j 1;
# --report-only with --goal-modules all, --record and --note; --all-sources;
# a map without line counters (the estimate table, --goal-lines exit 2);
# --self-test; and argparse's errors.
#
# FID: the compiler that builds the tools and the stand-in (default
# release/fid.exe; a path Python can start too, so on Windows not a /c/...
# one). Run from the repository root; needs python3 (or python) and a C
# compiler ($CC, default zig cc).
set -u
FID="${FID:-release/fid.exe}"
OUT="${OUT:-${TMPDIR:-/tmp}/fid-coverage-check}"
CCMD=(${CC:-zig cc})
mkdir -p "$OUT"
rm -f "$OUT/record.md"
PY=python3
command -v "$PY" > /dev/null 2>&1 || PY=python
X=""
LIBS=(-lm -lpthread)
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) X=".exe"; LIBS=(-lws2_32) ;; esac
exe="$OUT/coverage$X"
inst="$OUT/cov_instrument$X"
for t in coverage:"$exe" cov_instrument:"$inst"; do
    name="${t%%:*}"; out="${t#*:}"
    if ! "$FID" ccompile "tools/$name.fd" -o "$out" > "$OUT/build-$name.txt" 2>&1; then
        echo "FAIL: tools/$name.fd does not build"; tail -5 "$OUT/build-$name.txt"; exit 1
    fi
done
# the stand-in counter compiler, with and without line counters
for kind in lines blocks; do
    if [ $kind = lines ]; then
        FID_C_LINE_MARKS=1 "$FID" dump-c tests/tools/coverage/fakefid.fd > "$OUT/fake-$kind.c" 2> /dev/null
    else
        "$FID" dump-c tests/tools/coverage/fakefid.fd > "$OUT/fake-$kind.c" 2> /dev/null
    fi
    "$inst" "$OUT/fake-$kind.c" "$OUT/fake-$kind.inst.c" > /dev/null || { echo "FAIL: instrumenting"; exit 1; }
    "${CCMD[@]}" -O0 -w -Wno-error -Wno-int-conversion -Wno-incompatible-pointer-types \
        -Wno-implicit-function-declaration -Wno-return-type "$OUT/fake-$kind.inst.c" \
        -o "$OUT/fakecov-$kind$X" "${LIBS[@]}" > "$OUT/cc-$kind.txt" 2>&1 \
        || { echo "FAIL: the stand-in does not build"; tail -5 "$OUT/cc-$kind.txt"; exit 1; }
done
pass=0
fail=0
# compare <name> <args...>: both get the args, each with its own --out
# (<OUT>/<name>.py.d, <OUT>/<name>.fd.d, made fresh unless KEEP=1)
compare() {
    local name="$1"; shift
    if [ "${KEEP:-0}" != 1 ]; then rm -rf "$OUT/$name.py.d" "$OUT/$name.fd.d"; fi
    "$PY" scripts/verify/coverage.py "$@" --out "$OUT/$name.py.d" > "$OUT/$name.py" 2>&1
    local pc=$?
    "$exe" "$@" --out "$OUT/$name.fd.d" > "$OUT/$name.fd" 2>&1
    local fc=$?
    # the out dirs' names appear in some messages; the --record date too
    for f in "$OUT/$name.py" "$OUT/$name.fd"; do
        sed -i -e "s#$name\.py\.d#OUTDIR#g; s#$name\.fd\.d#OUTDIR#g" "$f"
    done
    local bad=0
    diff --strip-trailing-cr "$OUT/$name.py" "$OUT/$name.fd" > "$OUT/$name.diff" || bad=1
    [ $pc = $fc ] || bad=1
    for f in report.txt never-called.txt uncovered-lines.txt merged.cov; do
        if [ -f "$OUT/$name.py.d/$f" ] || [ -f "$OUT/$name.fd.d/$f" ]; then
            diff --strip-trailing-cr "$OUT/$name.py.d/$f" "$OUT/$name.fd.d/$f" >> "$OUT/$name.diff" 2>&1 \
                || { bad=1; echo "  ($f differs)" >> "$OUT/$name.diff"; }
        fi
    done
    if [ $bad = 0 ]; then
        pass=$((pass + 1)); echo "ok   $name (exit $fc, $(wc -l < "$OUT/$name.fd") lines)"
    else
        fail=$((fail + 1)); echo "FAIL $name (exit $pc vs $fc)"; head -20 "$OUT/$name.diff"
    fi
}
L="$OUT/fakecov-lines$X"
LM="$OUT/fake-lines.inst.c.map"
B="$OUT/fakecov-blocks$X"
BM="$OUT/fake-blocks.inst.c.map"
T='tests/tools/coverage/sample/*.fd'
SRC=tests/tools/coverage/fakefid.fd
# -j 1 with the stand-in's one-second runs: see fakefid.fd
compare run --fid "$L" --map "$LM" --tests "$T" --sources "$SRC" -j 1 \
    --modes check,run,emit,emit:aarch64-linux,cmd:fmt+--check,cmd:lint,dump-c,nat \
    --goal-lines 60 --goal 50 --goal-modules "$SRC"
# a second run adds to the first's merged.cov
KEEP=1 compare run --fid "$L" --map "$LM" --tests "$T" --sources "$SRC" -j 1 --modes ccompile
# 51 runs of one test in one mode, 3 at a time (their bitmaps are all the
# same, so the stand-in need not wait between them)
many=run
for k in $(seq 50); do many="$many,run"; done
FAKE_FID_QUICK=1 compare progress --fid "$L" --map "$LM" --tests tests/tools/coverage/sample/short.fd \
    --sources "$SRC" -j 3 --modes "$many" --fresh
compare limit --fid "$L" --map "$LM" --tests "$T,tests/tools/coverage/sample/short.fd" --sources "$SRC" \
    -j 1 --limit 2 --modes run --goal 1 --goal-modules all
KEEP=1 compare run --map "$LM" --report-only --sources "$SRC" --goal 3 --goal-modules all \
    --goal-lines 2.5 --record "$OUT/record.md" --note "a check"
KEEP=1 compare run --map "$LM" --report-only --sources "$SRC" --all-sources --goal 99.95
compare blocks --fid "$B" --map "$BM" --tests "$T" --sources "$SRC" -j 1 --modes run,check \
    --goal-lines 50 --goal-modules all
compare noacc --map "$LM" --report-only
printf '' > "$OUT/empty.map"
compare emptymap --map "$OUT/empty.map" --report-only
"$PY" scripts/verify/coverage.py --self-test > "$OUT/self.py" 2>&1; pc=$?
"$exe" --self-test > "$OUT/self.fd" 2>&1; fc=$?
if diff --strip-trailing-cr "$OUT/self.py" "$OUT/self.fd" > /dev/null && [ $pc = $fc ]; then
    pass=$((pass + 1)); echo "ok   self-test (exit $fc)"
else
    fail=$((fail + 1)); echo "FAIL self-test (exit $pc vs $fc)"; cat "$OUT/self.fd"
fi
# argparse: usage, errors, abbreviations, -h
compare noargs
compare help -h
compare nofid --map "$LM"
compare badgoal --map "$LM" --report-only --goal x
compare ambiguous --map "$LM" --f x
compare stray --map "$LM" --report-only stray
compare abbrev --ma "$LM" --report-o -j4 --goal=0.5
# the two --record entries (Python's, then the port's) have the same text
# but for the minute each was written in
if "$PY" - "$OUT/record.md" <<'PY'
import re, sys
t = open(sys.argv[1], encoding="utf-8", newline="").read().replace("\r\n", "\n")
e = re.split(r"\n(?=## )", t)
e = [re.sub(r"^## \d{4}-\d\d-\d\d \d\d:\d\d:", "## DATE:", x) for x in e if x.startswith("## ")]
sys.exit(0 if len(e) >= 2 and e[-1] == e[-2] and "Counter compiler: (not run)" in e[-1] else 1)
PY
then
    pass=$((pass + 1)); echo "ok   record (the two --record entries agree)"
else
    fail=$((fail + 1)); echo "FAIL record: see $OUT/record.md"
fi
echo "coverage port: $pass same, $fail differ"
[ $fail = 0 ]
