#!/usr/bin/env bash
# tools/compile_gate.fd (roadmap AD4) does what scripts/compile-gate.sh and
# bench/compile-check.sh do: each pair runs with the same arguments and
# environment in a scratch copy of the tree (the scripts write their logs
# and baselines beside themselves), and the output, exit status and
# baseline files are compared.
#
#   bash tests/tools/compile_gate/check.sh       # about 3 min
#
# FID: the compiler (default release/fid.exe) that builds the tool and the
# stand-in fakefid.fd, whose `[time]` lines (times.txt: stages of one and
# of several words, a line that is not a time) change from build to build
# so that the best of three counts; FID_HOME tells a compiler outside the
# tree where stdlib/ is. OUT: where the tool, the copy and the outputs go.
# Cases, compile-gate.sh: no baseline (saved), --save, a baseline with
# stages over the limit, under MIN_MS, missing and 0 (gate.baseline), a
# build that fails; the real compiler with RUNS=1 (--save; times masked).
# compile-check.sh: --save, a baseline with a slow stage, new stages and a
# faster total (bench.baseline), no baseline, other FILES, no compiler;
# the real compiler with RUNS=2 (--save, times masked: its stages of
# several words give bash's "integer expression expected" lines).
set -u
FID="${FID:-release/fid.exe}"
export CC="${CC:-zig cc}"
OUT="${OUT:-${TMPDIR:-/tmp}/fid-compile-gate-check}"
mkdir -p "$OUT"
X=""
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) X=".exe" ;; esac
exe="$OUT/compile_gate$X"
for t in "tools/compile_gate.fd:$exe" "tests/tools/compile_gate/fakefid.fd:$OUT/fakefid$X"; do
    if ! "$FID" ccompile "${t%%:*}" -o "${t#*:}" > "$OUT/build.txt" 2>&1; then
        echo "FAIL: ${t%%:*} does not build"; tail -5 "$OUT/build.txt"; exit 1
    fi
done
FID="$(cd "$(dirname "$FID")" && pwd)/$(basename "$FID")"
H=$(pwd)/tests/tools/compile_gate
T="$OUT/tree"
rm -rf "$T"
mkdir -p "$T/scripts" "$T/bench" "$T/tests/bootstrap" "$OUT/count"
cp scripts/compile-gate.sh "$T/scripts/"
cp bench/compile-check.sh "$T/bench/"
cp tests/bootstrap/{hello,test_hashmap,test_tasks,test_compiler_sized}.fd "$T/tests/bootstrap/"
export FAKE_TIMES="$H/times.txt" FAKE_DIR="$OUT/count"
pass=0
fail=0
# compare <name> <baseline to start from, or -> <gate|bench> [args...]
compare() {
    local name="$1" start="$2" which="$3"; shift 3
    local script=scripts/compile-gate.sh bfile=scripts/compile-gate.baseline flag=""
    if [ "$which" = bench ]; then script=bench/compile-check.sh; bfile=bench/compile-baseline.txt; flag=--bench; fi
    local side
    for side in sh fd; do
        rm -f "$T/$bfile" "$OUT"/count/*.count
        rm -rf "$T/build" "$T/bench/out"
        [ "$start" = - ] || cp "$start" "$T/$bfile"
        if [ $side = sh ]; then
            (cd "$T" && bash "$script" "$@") > "$OUT/$name.sh" 2>&1
        else
            (cd "$T" && "$exe" $flag "$@") > "$OUT/$name.fd" 2>&1
        fi
        echo "exit $?" >> "$OUT/$name.$side"
        if [ -f "$T/$bfile" ]; then tr -d '\r' < "$T/$bfile" > "$OUT/$name.$side.base"; else echo none > "$OUT/$name.$side.base"; fi
        if [ -f "$T/build/compile-gate/run.txt" ]; then cp "$T/build/compile-gate/run.txt" "$OUT/$name.$side.run"; else echo none > "$OUT/$name.$side.run"; fi
    done
    local m=cat
    [ "${MASK:-}" = 1 ] && m="sed -E s/[0-9]+/N/g"
    if diff <(tr -d '\r' < "$OUT/$name.sh" | $m) <(tr -d '\r' < "$OUT/$name.fd" | $m) > "$OUT/$name.diff" \
       && diff <($m "$OUT/$name.sh.base") <($m "$OUT/$name.fd.base") >> "$OUT/$name.diff" \
       && diff <($m "$OUT/$name.sh.run") <($m "$OUT/$name.fd.run") >> "$OUT/$name.diff"; then
        pass=$((pass + 1)); echo "ok   $name ($(tail -1 "$OUT/$name.fd"), $(wc -l < "$OUT/$name.fd") lines)"
    else
        fail=$((fail + 1)); echo "FAIL $name"; head -20 "$OUT/$name.diff"
    fi
}
FF="$OUT/fakefid$X"
FID="$FF" compare gate-no-baseline - gate
FID="$FF" compare gate-save "$H/gate.baseline" gate --save
FID="$FF" compare gate-compare "$H/gate.baseline" gate
FID="$FF" LIMIT_PCT=30 MIN_MS=10 compare gate-limits "$H/gate.baseline" gate
FID="$FF" FAKE_FAIL=test_hashmap compare gate-fails "$H/gate.baseline" gate
FID="$FID" RUNS=1 MASK=1 compare gate-real - gate --save
FID="$FF" compare bench-save - bench --save
FID="$FF" compare bench-compare "$H/bench.baseline" bench
FID="$FF" LIMIT_PCT=30 compare bench-limit "$H/bench.baseline" bench
FID="$FF" compare bench-no-baseline - bench
FID="$FF" FILES="../tests/bootstrap/test_tasks.fd" compare bench-files "$H/bench.baseline" bench
FID="$FID" RUNS=2 MASK=1 FILES=../tests/bootstrap/hello.fd compare bench-real - bench --save
FID= compare bench-no-fid - bench
echo "compile_gate port: $pass same, $fail differ"
[ $fail = 0 ]
