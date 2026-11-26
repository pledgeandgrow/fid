#!/usr/bin/env bash
# tools/interleave.fd (roadmap AD4) prints and writes what
# scripts/verify/interleave.py does: both run with the same arguments, each
# with its own --out (and --record file in it), and their stdout, stderr,
# exit status, the <out>/<name>-<backend>-seed<seed>.out files kept for
# wrong runs and the --record rows are compared. Masked: the --out
# directories' names, the seconds ("runs in N s", a row's time) and a
# row's date ("\r\n" from Python on Windows counts as "\n").
#
#   bash tests/tools/interleave/check.sh      # a few minutes
#   ONLY=broken bash tests/tools/interleave/check.sh   # one case
#
# A seed names the same CPU set and load in both (Python's
# random.Random(seed) drawn the same way): --seeds prints them, and they
# must be the same. Cases: tests/verify/interleave.fd with --seeds on
# ccompile; -n 6 on ccompile and emit with --record; seeded.fd under
# --sched-random (it prints its FIDES_SCHED_RANDOM: every run differs from
# the reference, each in its own way); wrong.fd, which never prints its
# '// Expected output' block (twelve wrong runs: five kept, ten seeds and
# " ..." in the replay line); exit3.fd (exit status 3 every time) beside
# wrong.fd; hang.fd, killed at --timeout 2; broken.fd, which does not
# compile, with --record; -h, a bad -n and a bad --seeds (Python's
# traceback ends in the port's one line: only that line is compared).
#
# FID: the compiler both build the programs with and that builds the tool
# (default release/fid.exe; a path Python can start too, so on Windows not
# a /c/... one). Run from the repository root; needs python3 (or python)
# with psutil.
set -u
FID="${FID:-release/fid.exe}"
OUT="${OUT:-${TMPDIR:-/tmp}/fid-interleave-check}"
mkdir -p "$OUT"
PY=python3
command -v "$PY" > /dev/null 2>&1 || PY=python
"$PY" -c "import psutil" > /dev/null 2>&1 || PY=python
exe="$OUT/interleave"
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) exe="$exe.exe" ;; esac
if ! "$FID" ccompile tools/interleave.fd -o "$exe" > "$OUT/build.txt" 2>&1; then
    echo "FAIL: tools/interleave.fd does not build"; tail -5 "$OUT/build.txt"; exit 1
fi
T=tests/tools/interleave
pass=0
fail=0
# compare <name> <last-line-of-stderr-only: 0|1> <args...>: both get the
# args ("@OUT@" in one becomes that side's --out) and --out
# <OUT>/<name>.py.d or <name>.fd.d
compare() {
    [ -n "${ONLY:-}" ] && [ "$ONLY" != "$1" ] && return 0
    local name="$1" lastline="$2"; shift 2
    local d pc fc bad=0 args a
    rm -f "$OUT/$name.diff"
    for d in py fd; do
        rm -rf "$OUT/$name.$d.d"
        args=()
        for a in "$@"; do args+=("${a//@OUT@/$OUT/$name.$d.d}"); done
        if [ $d = py ]; then
            "$PY" scripts/verify/interleave.py "${args[@]}" --out "$OUT/$name.$d.d" > "$OUT/$name.$d.out" 2> "$OUT/$name.$d.err"; pc=$?
        else
            "$exe" "${args[@]}" --out "$OUT/$name.$d.d" > "$OUT/$name.$d.out" 2> "$OUT/$name.$d.err"; fc=$?
        fi
        if [ "$lastline" = 1 ]; then tail -1 "$OUT/$name.$d.err" > "$OUT/$name.$d.e1"; mv "$OUT/$name.$d.e1" "$OUT/$name.$d.err"; fi
    done
    norm() {
        sed -E -e 's/\r$//' -e "s#$name\.(py|fd)\.d#OUTDIR#g" -e 's/runs in [0-9]+ s/runs in N s/' -e 's/: [0-9]+ms$/: Nms/' \
            -e 's/^\| [0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2} \|/| DATE |/' -e 's/\| [0-9]+ s \|$/| N s |/' "$1"
    }
    for s in out err; do
        diff <(norm "$OUT/$name.py.$s") <(norm "$OUT/$name.fd.$s") >> "$OUT/$name.diff" || bad=1
    done
    [ $pc = $fc ] || bad=1
    # the files each left in its --out (the executables and the
    # compiler's .fides-stamp files aside)
    local lp lf f
    lp=$(cd "$OUT/$name.py.d" 2>/dev/null && ls | grep -v '\.exe$\|\.fides-stamp$' | sort)
    lf=$(cd "$OUT/$name.fd.d" 2>/dev/null && ls | grep -v '\.exe$\|\.fides-stamp$' | sort)
    [ "$lp" = "$lf" ] || { bad=1; echo "  (files in --out: [$lp] vs [$lf])" >> "$OUT/$name.diff"; }
    for f in $lp; do
        [ -f "$OUT/$name.fd.d/$f" ] || continue
        # a kept run's stderr is its own (a crash's address): its header and
        # stdout are compared
        diff <(norm "$OUT/$name.py.d/$f" | sed '/^--- stderr ---$/q') <(norm "$OUT/$name.fd.d/$f" | sed '/^--- stderr ---$/q') \
            >> "$OUT/$name.diff" || { bad=1; echo "  ($f differs)" >> "$OUT/$name.diff"; }
    done
    if [ $bad = 0 ]; then
        pass=$((pass + 1)); echo "ok   $name (exit $fc, $(cat "$OUT/$name.fd.out" "$OUT/$name.fd.err" | wc -l) lines)"
    else
        fail=$((fail + 1)); echo "FAIL $name (exit $pc vs $fc)"; head -20 "$OUT/$name.diff"
    fi
}
compare seeds 0 --fid "$FID" --backends ccompile --seeds 1,2,3,17,42,99 --shards 2
compare default 0 --fid "$FID" -n 6 --shards 3 --record @OUT@/runs.md
compare sched-random 0 --fid "$FID" --backends ccompile --programs "$T/seeded.fd" --sched-random -n 7 --shards 2
compare wrong 0 --fid "$FID" --backends ccompile --programs "$T/wrong.fd" -n 12 --shards 4
compare exit3 0 --fid "$FID" --backends ccompile --programs "$T/exit3.fd,$T/wrong.fd" -n 3 --seeds 5,6
compare hang 0 --fid "$FID" --backends ccompile --programs "$T/hang.fd" -n 2 --shards 2 --timeout 2
compare broken 0 --fid "$FID" --backends ccompile --programs "$T/broken.fd,$T/exit3.fd" -n 2 --record @OUT@/runs.md
compare help 0 -h
compare bad-n 0 -n x
compare bad-seeds 1 --fid "$FID" --seeds 1,x
echo "interleave port: $pass same, $fail differ"
[ $fail = 0 ]
