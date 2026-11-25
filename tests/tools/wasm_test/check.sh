#!/usr/bin/env bash
# tools/wasm_test.fd (roadmap AD4) does what scripts/wasm-test.sh does:
# both build the same tests with `fid wasm`, run them under Node and
# compare with `fid run`; their output, exit status and results.txt are
# compared (the OUT folders' paths written as <OUT>, "\r" dropped).
#
#   bash tests/tools/wasm_test/check.sh
#
# FID: the compiler (default release/fid.exe; on Windows a C:/... path);
# the tool is built with `$FID ccompile`. TESTS: the sample (default: four
# wasm tests, a panic test, the integer semantics test, one that starts a
# process (SKIP: not available in WebAssembly), an expected-fail test
# (BUILD), one that reads Windows DLLs (FAIL under WASI)). WORK: where both
# write. Lanes: the sample; the sample again with REUSE=1 (nothing rerun,
# compared from the files left). Needs node. Run from the repository root.
set -u
FID="${FID:-release/fid.exe}"
WORK="${WORK:-${TMPDIR:-/tmp}/fid-wasm-test-check}"
TESTS="${TESTS:-tests/bootstrap/test_wasm_globals.fd tests/bootstrap/test_wasm_structs.fd tests/bootstrap/test_wasm_control_flow.fd tests/bootstrap/test_wasm_strings_floats.fd tests/bootstrap/test_panic_div_zero_fail.fd tests/bootstrap/test_int_semantics.fd tests/wasm/test_wasm_process.fd tests/bootstrap/test_return_type_fail.fd tests/bootstrap/test_cparser_dll_exports.fd}"
mkdir -p "$WORK"
exe="$WORK/wasm_test"
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) exe="$exe.exe" ;; esac
if ! CC="${CC:-zig cc}" "$FID" ccompile tools/wasm_test.fd -o "$exe" > "$WORK/build.txt" 2>&1; then
    echo "FAIL: tools/wasm_test.fd does not build"; tail -5 "$WORK/build.txt"; exit 1
fi
pass=0
fail=0
norm() { tr -d '\r' < "$1" | sed "s#$2#<OUT>#g"; }
compare() {   # name, then VAR=value settings
    local name="$1"; shift
    local so="$WORK/out-sh" po="$WORK/out-fd"
    # shellcheck disable=SC2086
    env "$@" OUT="$so" FID="$FID" JOBS=2 bash scripts/wasm-test.sh $TESTS < /dev/null > "$WORK/$name.sh.txt" 2>&1
    local sc=$?
    # shellcheck disable=SC2086
    env "$@" OUT="$po" FID="$FID" JOBS=2 "$exe" $TESTS < /dev/null > "$WORK/$name.fd.txt" 2>&1
    local fc=$?
    local d=0
    diff <(norm "$WORK/$name.sh.txt" "$so") <(norm "$WORK/$name.fd.txt" "$po") > "$WORK/$name.diff" || d=1
    diff <(norm "$so/results.txt" "$so") <(norm "$po/results.txt" "$po") >> "$WORK/$name.diff" || d=1
    for f in "$so"/*_rc; do
        b=$(basename "$f")
        cmp -s "$f" "$po/$b" || { echo "differs: $b" >> "$WORK/$name.diff"; d=1; }
    done
    if [ $d = 0 ] && [ $sc = $fc ]; then
        pass=$((pass + 1)); echo "ok   $name (exit $fc, $(wc -l < "$WORK/$name.fd.txt") lines)"
    else
        fail=$((fail + 1)); echo "FAIL $name (exit $sc vs $fc)"; head -20 "$WORK/$name.diff"
    fi
}
rm -rf "$WORK/out-sh" "$WORK/out-fd"
compare sample
compare reuse REUSE=1
echo "wasm_test port: $pass same, $fail differ"
[ $fail = 0 ]
