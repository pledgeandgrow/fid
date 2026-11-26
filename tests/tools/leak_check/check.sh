#!/usr/bin/env bash
# tools/leak_check.fd (roadmap AD4) does what scripts/leak-check.sh does:
# both run with the same arguments, environment and compiler; their output
# (the report folder's name masked) and exit status are compared, and so
# are the .out and .status files each leaves in its report folder.
#
#   bash tests/tools/leak_check/check.sh         # about 3 min
#
# FID: the compiler (default release/fid.exe); the tool is built with
# `$FID ccompile` (CC, default "zig cc"). OUT: where the tool and the
# outputs go. Cases: two of tests/leaks, sample/leaky.fd (a block never
# freed), sample/wrong.fd (output differs from a CRLF .expected),
# sample/crlf.fd (a CRLF .expected that matches); every tests/leaks
# program; NATIVE=x86_64-windows with JOBS=2 (emit --leaks, the machine
# layer's runtime; with `fid run --leaks` JOBS>1 races in both, as the
# script says) on a Windows host. Run from the repository root (as the
# script: `fid run --leaks` builds in the current folder).
set -u
FID="${FID:-release/fid.exe}"
export CC="${CC:-zig cc}"
OUT="${OUT:-${TMPDIR:-/tmp}/fid-leak-check-check}"
mkdir -p "$OUT"
exe="$OUT/leak_check"
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) exe="$exe.exe"; WIN=1 ;; *) WIN=0 ;; esac
if ! "$FID" ccompile tools/leak_check.fd -o "$exe" > "$OUT/build.txt" 2>&1; then
    echo "FAIL: tools/leak_check.fd does not build"; tail -5 "$OUT/build.txt"; exit 1
fi
pass=0
fail=0
mask() { sed 's/(reports in .*)/(reports in X)/' "$1"; }
compare() {   # name, then the arguments both get (environment from the caller)
    local name="$1"; shift
    bash scripts/leak-check.sh "$@" > "$OUT/$name.sh" 2>&1
    local sc=$?
    "$exe" "$@" > "$OUT/$name.fd" 2>&1
    local fc=$?
    local d1 d2 ok=1
    d1=$(sed -n 's/.*(reports in \(.*\))$/\1/p' "$OUT/$name.sh")
    d2=$(sed -n 's/.*(reports in \(.*\))$/\1/p' "$OUT/$name.fd")
    diff <(mask "$OUT/$name.sh") <(mask "$OUT/$name.fd") > "$OUT/$name.diff" || ok=0
    [ $sc = $fc ] || ok=0
    # the files each left: .status and .out of every program
    if [ -n "$d1" ] && [ -n "$d2" ]; then
        for f in "$d1"/*.status "$d1"/*.out; do
            b=$(basename "$f")
            [ -f "$d2/$b" ] || [ -f "$(cygpath -u "$d2" 2>/dev/null)/$b" ] || { echo "  missing $b" >> "$OUT/$name.diff"; ok=0; continue; }
            g="$d2/$b"; [ -f "$g" ] || g="$(cygpath -u "$d2")/$b"
            cmp -s <(tr -d '\r' < "$f") <(tr -d '\r' < "$g") || { echo "  $b differs" >> "$OUT/$name.diff"; ok=0; }
        done
    fi
    if [ $ok = 1 ]; then
        pass=$((pass + 1)); echo "ok   $name (exit $fc, $(wc -l < "$OUT/$name.fd") lines)"
    else
        fail=$((fail + 1)); echo "FAIL $name (exit $sc vs $fc)"; head -20 "$OUT/$name.diff"
    fi
}
S=tests/tools/leak_check/sample
compare sample "$FID" tests/leaks/boxes.fd tests/leaks/int_strings.fd $S/leaky.fd $S/wrong.fd $S/crlf.fd
compare all "$FID"
if [ $WIN = 1 ]; then
    NATIVE=x86_64-windows JOBS=2 compare native-jobs2 "$FID" tests/leaks/boxes.fd tests/leaks/int_strings.fd tests/leaks/regions.fd $S/leaky.fd $S/wrong.fd
fi
echo "leak_check port: $pass same, $fail differ"
[ $fail = 0 ]
