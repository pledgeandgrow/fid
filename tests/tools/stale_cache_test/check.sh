#!/usr/bin/env bash
# tools/stale_cache_test.fd (roadmap AD4) does what
# scripts/stale-cache-test.sh does: both run with the same compiler and
# environment, and their output and exit status are compared.
#
#   bash tests/tools/stale_cache_test/check.sh       # about 2 min
#
# FID: the compiler (default release/fid.exe), given to both as a path
# relative to the repository root (the script runs "$OLDPWD/$FID"); the
# tool is built with `$FID ccompile` (CC, default "zig cc"). OUT: where the
# tool and the outputs go. FID_HOME, when set, is how a compiler outside
# the tree finds stdlib/. Cases: the real compiler (PASS); fakefid.fd, a
# compiler whose every build is the same program printing 42 (the cache
# never invalidates: "FAIL: dep change not picked up"), the same saying
# "up to date"; one that fails at once (exit 3, no message: set -e); and
# a compiler that does not exist. Run from the repository root.
set -u
FID="${FID:-release/fid.exe}"
export CC="${CC:-zig cc}"
OUT="${OUT:-${TMPDIR:-/tmp}/fid-stale-cache-check}"
mkdir -p "$OUT"
X=""
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) X=".exe" ;; esac
exe="$OUT/stale_cache_test$X"
for t in "tools/stale_cache_test.fd:$exe" "tests/tools/stale_cache_test/fakefid.fd:$OUT/fakefid$X" \
         "tests/tools/stale_cache_test/print42.fd:$OUT/print42$X"; do
    if ! "$FID" ccompile "${t%%:*}" -o "${t#*:}" > "$OUT/build.txt" 2>&1; then
        echo "FAIL: ${t%%:*} does not build"; tail -5 "$OUT/build.txt"; exit 1
    fi
done
rel() { realpath --relative-to=. "$1"; }
pass=0
fail=0
compare() {   # name, then the compiler both get (environment from the caller)
    local name="$1"; shift
    bash scripts/stale-cache-test.sh "$@" > "$OUT/$name.sh" 2>&1
    local sc=$?
    "$exe" "$@" > "$OUT/$name.fd" 2>&1
    local fc=$?
    # the fake compiler names the temporary folder's main.exe: masked
    if diff <(sed 's|fake build of .*|fake build of X|' "$OUT/$name.sh") \
            <(sed 's|fake build of .*|fake build of X|' "$OUT/$name.fd") > "$OUT/$name.diff" && [ $sc = $fc ]; then
        pass=$((pass + 1)); echo "ok   $name (exit $fc, $(wc -l < "$OUT/$name.fd") lines)"
    else
        fail=$((fail + 1)); echo "FAIL $name (exit $sc vs $fc)"; head -20 "$OUT/$name.diff"
    fi
}
compare real "$(rel "$FID")"
FAKE_EXE="$OUT/print42$X" compare stale "$(rel "$OUT/fakefid$X")"
FAKE_EXE="$OUT/print42$X" FAKE_UPTODATE=1 compare stale-uptodate "$(rel "$OUT/fakefid$X")"
FAKE_FAIL=3 compare fails "$(rel "$OUT/fakefid$X")"
compare missing "no/such/fid$X"
echo "stale_cache_test port: $pass same, $fail differ"
[ $fail = 0 ]
