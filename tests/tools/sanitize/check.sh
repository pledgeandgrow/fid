#!/usr/bin/env bash
# tools/sanitize.fd (roadmap AD4) does what scripts/sanitize.sh does: both
# are run on the same tests with the same environment, and their output,
# exit status and summary.san are compared (the OUT folders' paths written
# as <OUT>, "\r" dropped).
#
#   bash tests/tools/sanitize/check.sh
#
# FID: the compiler (default release/fid.exe; on Windows a C:/... path);
# the tool is built with `$FID ccompile`. TESTS: the sample (test names).
# WORK: where both write. Lanes:
#   SAN=undefined CC="zig cc"   UBSan through zig cc (its own runtime): built,
#                               run and compared with `fid run`;
#   the defaults (CC=clang, ASan+UBSan): on a host without clang every
#                               build fails and each test counts as skipped;
#   SAN=thread,address          rejected (exit 2).
# ASan and TSan need clang or gcc with their runtimes (Linux, macOS): set
# SAN_CC=clang to add a lane with the default SAN and that compiler.
# COMPILER=1 is not compared (it builds the whole compiler sanitized).
# Run from the repository root.
set -u
FID="${FID:-release/fid.exe}"
WORK="${WORK:-${TMPDIR:-/tmp}/fid-sanitize-check}"
TESTS="${TESTS:-test_vec_builtins test_int_semantics test_return_order test_panic_div_zero_fail test_closure}"
mkdir -p "$WORK"
exe="$WORK/sanitize"
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) exe="$exe.exe" ;; esac
if ! CC="${CC:-zig cc}" "$FID" ccompile tools/sanitize.fd -o "$exe" > "$WORK/build.txt" 2>&1; then
    echo "FAIL: tools/sanitize.fd does not build"; tail -5 "$WORK/build.txt"; exit 1
fi
pass=0
fail=0
norm() { tr -d '\r' < "$1" | sed "s#$2#<OUT>#g"; }
compare() {   # name, then VAR=value settings
    local name="$1"; shift
    local so="$WORK/$name-sh" po="$WORK/$name-fd"
    rm -rf "$so" "$po"
    # shellcheck disable=SC2086
    env "$@" OUT="$so" FID="$FID" bash scripts/sanitize.sh $TESTS < /dev/null > "$WORK/$name.sh.txt" 2>&1
    local sc=$?
    # shellcheck disable=SC2086
    env "$@" OUT="$po" FID="$FID" "$exe" $TESTS < /dev/null > "$WORK/$name.fd.txt" 2>&1
    local fc=$?
    local d=0
    diff <(norm "$WORK/$name.sh.txt" "$so") <(norm "$WORK/$name.fd.txt" "$po") > "$WORK/$name.diff" || d=1
    if [ -f "$so/summary.san" ] || [ -f "$po/summary.san" ]; then
        diff <(norm "$so/summary.san" "$so") <(norm "$po/summary.san" "$po") >> "$WORK/$name.diff" 2>&1 || d=1
    fi
    if [ $d = 0 ] && [ $sc = $fc ]; then
        pass=$((pass + 1)); echo "ok   $name (exit $fc: $(head -1 "$WORK/$name.fd.txt"))"
    else
        fail=$((fail + 1)); echo "FAIL $name (exit $sc vs $fc)"; head -20 "$WORK/$name.diff"
    fi
}
compare ubsan SAN=undefined CC="zig cc"
compare defaults
compare tsan-asan SAN=thread,address
if [ -n "${SAN_CC:-}" ]; then
    compare asan CC="$SAN_CC"
fi
echo "sanitize port: $pass same, $fail differ"
[ $fail = 0 ]
