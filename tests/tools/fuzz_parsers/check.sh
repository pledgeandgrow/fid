#!/usr/bin/env bash
# tools/fuzz_parsers.fd (roadmap AD4) does what scripts/fuzz/fuzz_parsers.py
# does: both build tests/fuzz/parsers.fd and run the same inputs on it,
# one after the other, into the same --out folder, and their output, exit
# status and kept findings are compared ("\r\n" from Python on Windows
# counts as "\n").
#
#   bash tests/tools/fuzz_parsers/check.sh
#
# The inputs themselves: inputs.sh writes the first COUNT (default 2000)
# inputs of a seed over every parser with both and compares them byte for
# byte. FID: the compiler both drive and that builds the tool (default
# release/fid.exe; on Windows a path Python can start, not /c/...). Run
# from the repository root; needs python3 (or python).
set -u
FID="${FID:-release/fid.exe}"
export CC="${CC:-zig cc}"
OUT="${OUT:-${TMPDIR:-/tmp}/fid-fuzz-parsers-check}"
COUNT="${COUNT:-2000}"
N="${N:-90}"
mkdir -p "$OUT"
PY=python3
command -v "$PY" > /dev/null 2>&1 || PY=python
exe="$OUT/fuzz_parsers"
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) exe="$exe.exe" ;; esac
if ! "$FID" ccompile tools/fuzz_parsers.fd -o "$exe" > "$OUT/build.txt" 2>&1; then
    echo "FAIL: tools/fuzz_parsers.fd does not build"; tail -5 "$OUT/build.txt"; exit 1
fi
pass=0
fail=0
compare() {   # name, then the arguments both get (--out is added)
    local name="$1"; shift
    local o="$OUT/out-$name"
    rm -rf "$o" "$o.py" "$o.fd"
    timeout 3600 "$PY" -X utf8 scripts/fuzz/fuzz_parsers.py "$@" --fid "$FID" --out "$o" < /dev/null > "$OUT/$name.py" 2>&1
    local pc=$?
    mv "$o" "$o.py"
    timeout 3600 "$exe" "$@" --fid "$FID" --out "$o" < /dev/null > "$OUT/$name.fd" 2>&1
    local fc=$?
    mv "$o" "$o.fd"
    # the built driver and its cache files are not compared
    rm -rf "$o".py/parsers.* "$o".fd/parsers.* "$o".py/.fidcache "$o".fd/.fidcache
    # the compiler's timings in a failed build's message
    sed -i -E 's/[0-9]+ ?ms/N ms/g' "$OUT/$name.py" "$OUT/$name.fd"
    if diff --strip-trailing-cr "$OUT/$name.py" "$OUT/$name.fd" > "$OUT/$name.diff" \
        && diff -r "$o.py" "$o.fd" >> "$OUT/$name.diff" && [ $pc = $fc ]; then
        pass=$((pass + 1)); echo "ok   $name (exit $fc, $(grep -c . "$OUT/$name.fd") lines, $(find "$o.fd" -type f | wc -l) files)"
    else
        fail=$((fail + 1)); echo "FAIL $name (exit $pc vs $fc)"; head -20 "$OUT/$name.diff"
    fi
}
compare all -n "$N" --seed 3
compare some -n 30 --seed 8 --parsers tls,json,url --timeout 30
# --timeout 0: every run is out of time at once, so every input is a
# finding ("<parser>: timeout: "), one kept per parser
compare hangs -n 6 --seed 2 --parsers json,tls --timeout 0
# a backend that builds no driver: the build's error, exit status 2
compare nobuild -n 3 --backend check
if FID="$FID" bash tests/tools/fuzz_parsers/inputs.sh "$OUT/inputs" 4 "$COUNT"; then
    pass=$((pass + 1)); echo "ok   inputs ($COUNT identical)"
else
    fail=$((fail + 1)); echo "FAIL inputs"; head -20 "$OUT/inputs/diff.txt"
fi
echo "fuzz_parsers port: $pass same, $fail differ"
[ $fail = 0 ]
