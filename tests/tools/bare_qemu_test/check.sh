#!/usr/bin/env bash
# tools/bare_qemu_test.fd (roadmap AD4) does what scripts/bare-qemu-test.sh
# does: both are run with the same tests and environment, and their output,
# exit status and the .expected files they write are compared (the OUT
# folders' paths written as <OUT>, "\r" dropped).
#
#   bash tests/tools/bare_qemu_test/check.sh
#
# FID: the compiler (default release/fid.exe; on Windows a C:/... path);
# the tool is built with `$FID ccompile`. WORK: where both write.
# Lanes: RUN=0 on bare tests with an Expected output block and the kernel
# tests whose expected output the script holds (built only); BUILD=0 RUN=0;
# RUN=1 with a QEMU that does not exist (the error, exit 2). When
# qemu-system-x86_64 is on PATH a last lane boots the sample (RUN=1).
# Run from the repository root.
set -u
FID="${FID:-release/fid.exe}"
WORK="${WORK:-${TMPDIR:-/tmp}/fid-bare-qemu-check}"
TESTS="${TESTS:-tests/bootstrap/test_bare_print.fd tests/bootstrap/test_bare_vec.fd tests/bootstrap/test_bare_fault.fd tests/bootstrap/test_kernel_serial.fd tests/bootstrap/test_serial_hello.fd tests/bootstrap/test_bare.fd}"
mkdir -p "$WORK"
exe="$WORK/bare_qemu_test"
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) exe="$exe.exe" ;; esac
if ! CC="${CC:-zig cc}" "$FID" ccompile tools/bare_qemu_test.fd -o "$exe" > "$WORK/build.txt" 2>&1; then
    echo "FAIL: tools/bare_qemu_test.fd does not build"; tail -5 "$WORK/build.txt"; exit 1
fi
pass=0
fail=0
norm() { tr -d '\r' < "$1" | sed "s#$2#<OUT>#g"; }
compare() {   # name, then VAR=value settings
    local name="$1"; shift
    local so="$WORK/$name-sh" po="$WORK/$name-fd"
    rm -rf "$so" "$po"
    # shellcheck disable=SC2086
    env "$@" OUT="$so" bash scripts/bare-qemu-test.sh $TESTS < /dev/null > "$WORK/$name.sh.txt" 2>&1
    local sc=$?
    # shellcheck disable=SC2086
    env "$@" OUT="$po" "$exe" $TESTS < /dev/null > "$WORK/$name.fd.txt" 2>&1
    local fc=$?
    local d=0
    diff <(norm "$WORK/$name.sh.txt" "$so") <(norm "$WORK/$name.fd.txt" "$po") > "$WORK/$name.diff" || d=1
    for f in "$so"/*.expected; do
        [ -f "$f" ] || continue
        cmp -s "$f" "$po/$(basename "$f")" || { echo "differs: $(basename "$f")" >> "$WORK/$name.diff"; d=1; }
    done
    if [ $d = 0 ] && [ $sc = $fc ]; then
        pass=$((pass + 1)); echo "ok   $name (exit $fc, $(wc -l < "$WORK/$name.fd.txt") lines)"
    else
        fail=$((fail + 1)); echo "FAIL $name (exit $sc vs $fc)"; head -20 "$WORK/$name.diff"
    fi
}
compare build-only RUN=0 FID="$FID"
compare no-build BUILD=0 RUN=0 FID="$FID"
compare no-qemu RUN=1 QEMU=no-such-qemu-here FID="$FID"
# a test that does not build (FAIL ...: build, the log's last lines)
printf '// Expected output:\n// x\nfn main() -> i64 {\n    return "no";\n}\n' > "$WORK/bad_bare.fd"
TESTS_SAVE="$TESTS"; TESTS="$WORK/bad_bare.fd"
compare build-fails RUN=0 FID="$FID"
TESTS="$TESTS_SAVE"
if command -v qemu-system-x86_64 > /dev/null 2>&1; then
    compare boot RUN=1 FID="$FID"
fi
echo "bare_qemu_test port: $pass same, $fail differ"
[ $fail = 0 ]
