#!/usr/bin/env bash
# CS5: the parallel front-end passes (FID_PAR=N) give the sequential output
# byte for byte, and how long each takes.
#
#   FID=release/fid.exe bash tests/par/par_check.sh          # outputs
#   FID=release/fid.exe TIME=1 bash tests/par/par_check.sh   # + compiler timings
#
# 1. `fid check` of the reference-error and @secret tests with 64+ functions
#    (tests/bootstrap/test_escape_par_fail.fd, test_secret_par_fail.fd):
#    the same messages with FID_PAR unset, 2, 4 and 16 (timing lines dropped).
# 2. tests/selfhost/test_par_escape.fd (escape_check, secrecy_check and the borrow check called
#    on a parsed program of 271 functions), built with `ccompile`: the same
#    output with FID_PAR unset, 2, 4 and 16, and the output its header gives.
# 3. TIME=1: `check compiler/main.fd --time` with FID_PAR unset and 4: the
#    same messages, and the rows of the passes FID_PAR splits.
# Passes when it prints "par_check: ok".
set -u
FID="${FID:-release/fid.exe}"
OUT="${OUT:-$(mktemp -d)}"
fail=0
export FID_HOME="${FID_HOME:-$(pwd)}"

# Drop what differs between two runs of the same check: times.
norm() {
    grep -v -E '^\[prof\]|[0-9]+ ?ms\b|^\[time\]' "$1" || true
}

check_same() {
    local name="$1"; shift
    local f="$1"
    "$FID" check "$f" > "$OUT/$name.seq" 2>&1
    local n
    for n in 2 4 16; do
        FID_PAR=$n "$FID" check "$f" > "$OUT/$name.par$n" 2>&1
        if ! diff <(norm "$OUT/$name.seq") <(norm "$OUT/$name.par$n") > "$OUT/$name.diff$n"; then
            echo "FAIL: $name: FID_PAR=$n output differs (see $OUT/$name.diff$n)"
            fail=1
        fi
    done
    local errs
    errs=$(grep -c '^error' "$OUT/$name.seq" || true)
    echo "$name: $errs messages, the same with FID_PAR=2/4/16"
}

check_same escape tests/bootstrap/test_escape_par_fail.fd
check_same secret tests/bootstrap/test_secret_par_fail.fd
if [ "$(grep -c '^error' "$OUT/escape.seq")" -lt 75 ]; then
    echo "FAIL: escape: expected 75 reference errors"; fail=1
fi
if [ "$(grep -c '^error' "$OUT/secret.seq")" -lt 70 ]; then
    echo "FAIL: secret: expected 70 @secret errors"; fail=1
fi

# The checks called on a parsed program, built to a native executable.
exe="$OUT/test_par_escape.exe"
if "$FID" ccompile tests/selfhost/test_par_escape.fd -o "$exe" > "$OUT/build.log" 2>&1; then
    "$exe" > "$OUT/sh.seq" 2>&1
    sed -n '/^\/\/ Expected output:/,/^$/p' tests/selfhost/test_par_escape.fd \
        | sed -e '1d' -e 's|^// \{0,1\}||' | sed '/^$/d' > "$OUT/sh.expected"
    if ! diff "$OUT/sh.expected" "$OUT/sh.seq" > "$OUT/sh.diff"; then
        echo "FAIL: test_par_escape: output is not the expected one (see $OUT/sh.diff)"
        fail=1
    fi
    for n in 2 4 16; do
        FID_PAR=$n "$exe" > "$OUT/sh.par$n" 2>&1
        if ! cmp -s "$OUT/sh.seq" "$OUT/sh.par$n"; then
            echo "FAIL: test_par_escape: FID_PAR=$n output differs"
            fail=1
        fi
    done
    echo "test_par_escape: the same with FID_PAR=2/4/16"
else
    echo "FAIL: could not build tests/selfhost/test_par_escape.fd (see $OUT/build.log)"
    fail=1
fi

if [ "${TIME:-0}" = "1" ]; then
    "$FID" check compiler/main.fd --time > "$OUT/main.seq" 2>&1
    FID_PAR=4 "$FID" check compiler/main.fd --time > "$OUT/main.par4" 2>&1
    if ! diff <(norm "$OUT/main.seq") <(norm "$OUT/main.par4") > "$OUT/main.diff"; then
        echo "FAIL: check compiler/main.fd: FID_PAR=4 output differs (see $OUT/main.diff)"
        fail=1
    fi
    echo "--- check compiler/main.fd --time, sequential:"
    grep -E 'escape|secrecy|borrow|typecheck|total' "$OUT/main.seq" || true
    echo "--- FID_PAR=4:"
    grep -E 'escape|secrecy|borrow|typecheck|total' "$OUT/main.par4" || true
fi

if [ "$fail" = "0" ]; then
    echo "par_check: ok"
else
    echo "par_check: FAILED (outputs in $OUT)"
    exit 1
fi
