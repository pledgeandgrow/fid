#!/usr/bin/env bash
# tools/cross_test.fd (roadmap AD4) does what scripts/cross-test.sh does:
# both run on the same targets and tests with the same environment, and
# their output, exit status, results.txt and run-all.sh are compared (the
# two OUT folders' paths written as <OUT>, "\r" dropped).
#
#   bash tests/tools/cross_test/check.sh
#
# FID: the compiler (default release/fid.exe; on Windows a C:/... path, not
# /c/..., since the port starts it without a shell); the tool is built with
# `$FID ccompile`. TESTS: the sample (default below: a plain test, one that
# panics, one with an exit code of its own, one scripts/lane-skips.txt
# leaves out off Windows). WORK: where both write (default a temp folder).
# The lanes: no target (usage), an unknown target, then
#   x86_64-windows NATIVE=1 and x86_64-windows-pe NATIVE=1   built and run
#     here (Windows only; emit --nat and emit);
#   x86_64-linux BUILD_ONLY=1                                built through C
#     (zig cc), not run;
#   aarch64-linux NATIVE=1 BUILD_ONLY=1                      the machine
#     layer, not run; the qemu runner's loader list (bin-target-check.py);
#   COMPARE_ONLY=1 on the first lane's results.
# Run from the repository root.
set -u
FID="${FID:-release/fid.exe}"
WORK="${WORK:-${TMPDIR:-/tmp}/fid-cross-test-check}"
TESTS="${TESTS:-tests/bootstrap/hello.fd tests/bootstrap/test_panic_div_zero_fail.fd tests/bootstrap/test_return_order.fd tests/bootstrap/test_cparser_dll_exports.fd}"
mkdir -p "$WORK"
exe="$WORK/cross_test"
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) exe="$exe.exe"; WIN=1 ;; *) WIN=0 ;; esac
if ! CC="${CC:-zig cc}" "$FID" ccompile tools/cross_test.fd -o "$exe" > "$WORK/build.txt" 2>&1; then
    echo "FAIL: tools/cross_test.fd does not build"; tail -5 "$WORK/build.txt"; exit 1
fi
pass=0
fail=0
norm() {   # file, the OUT to hide (also as WSL sees it: /mnt/c/...)
    local m
    m=$(echo "$2" | sed -E 's#^([A-Za-z]):#/mnt/\L\1#')
    tr -d '\r' < "$1" | sed "s#$2#<OUT>#g; s#$m#<OUT>#g"
}
compare() {   # name, target, then VAR=value settings; uses $TESTS
    local name="$1" target="$2"; shift 2
    local so="$WORK/$name-sh" po="$WORK/$name-fd"
    rm -rf "$so" "$po" "$so-native" "$po-native"
    # shellcheck disable=SC2086
    env "$@" OUT="$so" FID="$FID" JOBS=2 bash scripts/cross-test.sh $target $TESTS < /dev/null > "$WORK/$name.sh.txt" 2>&1
    local sc=$?
    # shellcheck disable=SC2086
    env "$@" OUT="$po" FID="$FID" JOBS=2 "$exe" $target $TESTS < /dev/null > "$WORK/$name.fd.txt" 2>&1
    local fc=$?
    local d=0
    norm "$WORK/$name.sh.txt" "$so" > "$WORK/$name.a"; norm "$WORK/$name.fd.txt" "$po" > "$WORK/$name.b"
    diff "$WORK/$name.a" "$WORK/$name.b" > "$WORK/$name.diff" || d=1
    local sd="$so" pd="$po"   # NATIVE=1 adds -native to OUT
    [ -d "$so-native" ] && sd="$so-native"
    [ -d "$po-native" ] && pd="$po-native"
    for f in results.txt run-all.sh; do
        if [ -f "$sd/$f" ] || [ -f "$pd/$f" ]; then
            norm "$sd/$f" "$so" > "$WORK/$name.$f.a" 2>/dev/null
            norm "$pd/$f" "$po" > "$WORK/$name.$f.b" 2>/dev/null
            diff "$WORK/$name.$f.a" "$WORK/$name.$f.b" >> "$WORK/$name.diff" || d=1
        fi
    done
    if [ $d = 0 ] && [ $sc = $fc ]; then
        pass=$((pass + 1)); echo "ok   $name (exit $fc, $(wc -l < "$WORK/$name.b") lines)"
    else
        fail=$((fail + 1)); echo "FAIL $name (exit $sc vs $fc)"; head -20 "$WORK/$name.diff"
    fi
}
# usage and an unknown target (both make $OUT, as the script does)
TESTS_SAVE="$TESTS"; TESTS=""
compare usage ""
compare unknown no-such-target
TESTS="$TESTS_SAVE"
if [ $WIN = 1 ]; then
    compare nat x86_64-windows NATIVE=1
    compare pe x86_64-windows-pe NATIVE=1
fi
compare linux-c x86_64-linux BUILD_ONLY=1
compare arm-native aarch64-linux NATIVE=1 BUILD_ONLY=1
if [ $WIN = 1 ]; then
    # COMPARE_ONLY on what the nat lane left: each tool on a copy of its own
    for t in sh fd; do rm -rf "$WORK/co-$t-native"; cp -r "$WORK/nat-$t-native" "$WORK/co-$t-native"; done
    so="$WORK/co-sh"; po="$WORK/co-fd"
    COMPARE_ONLY=1 NATIVE=1 OUT="$so" FID="$FID" bash scripts/cross-test.sh x86_64-windows $TESTS > "$WORK/co.sh.txt" 2>&1; sc=$?
    COMPARE_ONLY=1 NATIVE=1 OUT="$po" FID="$FID" "$exe" x86_64-windows $TESTS > "$WORK/co.fd.txt" 2>&1; fc=$?
    if diff <(norm "$WORK/co.sh.txt" "$so") <(norm "$WORK/co.fd.txt" "$po") > "$WORK/co.diff" && [ $sc = $fc ]; then
        pass=$((pass + 1)); echo "ok   compare-only (exit $fc)"
    else
        fail=$((fail + 1)); echo "FAIL compare-only (exit $sc vs $fc)"; head -20 "$WORK/co.diff"
    fi
fi
echo "cross_test port: $pass same, $fail differ"
[ $fail = 0 ]
