#!/usr/bin/env bash
# tools/fuzz_frontend.fd (roadmap AD4) does what
# scripts/fuzz/fuzz_frontend.py does: both run with the same arguments, one
# after the other, into the same --out folder, and their output, exit
# status, findings (crash-<n>.fd/.out, findings.tsv) and --summary JSON are
# compared ("\r\n" from Python on Windows counts as "\n"; the scratch
# folder's name, the compiler's timings and "seconds" are blurred). A
# second run on the same --out with --keep files the same findings as
# known.
#
#   bash tests/tools/fuzz_frontend/check.sh
#
# The mutants themselves: mutants.sh writes the first COUNT (default 500)
# mutants of a seed over a corpus of the suite's tests (non-ASCII
# included) with both and compares them byte for byte.
# FID: the compiler both drive and that builds the tool (default
# release/fid.exe; on Windows a path Python can start, not /c/...). Not
# compared: --cov-fid (it needs a compiler built with
# scripts/verify/cov_instrument.py). Run from the repository root; needs
# python3 (or python).
set -u
FID="${FID:-release/fid.exe}"
export CC="${CC:-zig cc}"
OUT="${OUT:-${TMPDIR:-/tmp}/fid-fuzz-frontend-check}"
COUNT="${COUNT:-500}"
mkdir -p "$OUT"
PY=python3
command -v "$PY" > /dev/null 2>&1 || PY=python
exe="$OUT/fuzz_frontend"
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) exe="$exe.exe" ;; esac
if ! "$FID" ccompile tools/fuzz_frontend.fd -o "$exe" > "$OUT/build.txt" 2>&1; then
    echo "FAIL: tools/fuzz_frontend.fd does not build"; tail -5 "$OUT/build.txt"; exit 1
fi
pass=0
fail=0
norm() {   # dir -> dir.n
    rm -rf "$1.n"; mkdir -p "$1.n"
    (cd "$1" && find . -type f) | while read -r f; do
        mkdir -p "$(dirname "$1.n/$f")"
        sed -E 's/[0-9]+ ?ms/N ms/g; s/in[A-Za-z0-9_]+([\/])input\.fd/inX\1input.fd/g; s/"seconds": [0-9]+/"seconds": N/' "$1/$f" > "$1.n/$f"
    done
}
run_both() {   # name, then the arguments both get (--out is added)
    local name="$1"; shift
    local o="$OUT/out-$name"
    # the second run of a pair starts from the first one's --out
    cp -r "$o.base.py" "$o" 2>/dev/null || mkdir -p "$o"
    timeout 1800 "$PY" -X utf8 scripts/fuzz/fuzz_frontend.py "$@" --fid "$FID" --out "$o" < /dev/null > "$OUT/$name.py" 2>&1
    local pc=$?
    rm -rf "$o.py"; mv "$o" "$o.py"
    cp -r "$o.base.fd" "$o" 2>/dev/null || mkdir -p "$o"
    timeout 1800 "$exe" "$@" --fid "$FID" --out "$o" < /dev/null > "$OUT/$name.fd" 2>&1
    local fc=$?
    rm -rf "$o.fd"; mv "$o" "$o.fd"
    norm "$o.py"; norm "$o.fd"
    sed -E 's/[0-9]+ ?ms/N ms/g' "$OUT/$name.py" > "$OUT/$name.py.n"
    sed -E 's/[0-9]+ ?ms/N ms/g' "$OUT/$name.fd" > "$OUT/$name.fd.n"
    if diff --strip-trailing-cr "$OUT/$name.py.n" "$OUT/$name.fd.n" > "$OUT/$name.diff" \
        && diff -r --strip-trailing-cr "$o.py.n" "$o.fd.n" >> "$OUT/$name.diff" && [ $pc = $fc ]; then
        pass=$((pass + 1)); echo "ok   $name (exit $fc, $(tail -1 "$OUT/$name.fd" | tr -d '\r'))"
    else
        fail=$((fail + 1)); echo "FAIL $name (exit $pc vs $fc)"; head -20 "$OUT/$name.diff"
    fi
}
rm -rf "$OUT"/out-*
C='tests/tools/reduce/sample/*.fd'
# --mode run: the mutants are run, and those of sample/panic.fd still
# panic, so findings are filed (the compiler's own crashes are rare)
run_both run -n 40 --seed 5 --corpus "$C" --mode run --summary "$OUT/summary-$$.json" --timeout 60
mv "$OUT/out-run.py" "$OUT/out-again.base.py"; mv "$OUT/out-run.fd" "$OUT/out-again.base.fd"
run_both again -n 40 --seed 5 --corpus "$C" --mode run --keep "$OUT/keep" --summary "$OUT/summary-$$.json" --timeout 60
run_both check -n 40 --seed 9 --corpus "$C" --timeout 60
run_both ir -n 20 --seed 11 --corpus "$C" --mode ir --timeout 60
if FID="$FID" bash tests/tools/fuzz_frontend/mutants.sh "$OUT/mutants" 'tests/bootstrap/test_s*.fd' 3 "$COUNT"; then
    pass=$((pass + 1)); echo "ok   mutants ($COUNT identical)"
else
    fail=$((fail + 1)); echo "FAIL mutants"; head -20 "$OUT/mutants/diff.txt"
fi
echo "fuzz_frontend port: $pass same, $fail differ"
[ $fail = 0 ]
