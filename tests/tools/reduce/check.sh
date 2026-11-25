#!/usr/bin/env bash
# tools/reduce.fd (roadmap AD4) does what scripts/fuzz/reduce.py does:
# both reduce the same programs (sample/) in the same mode with the same
# compiler, and their output, exit status and reduced files are compared
# ("\r\n" from Python on Windows counts as "\n").
#
#   bash tests/tools/reduce/check.sh
#
# sample/panic.fd divides by zero after unrelated work (--fails run),
# typeerr.fd has one type error (--cmd "<fid> check {}" --match error),
# clock.fd prints the clock, so two interpreter runs disagree (--bad run
# --good run), and fine.fd fails nowhere (both say so and stop).
# FID: the compiler both drive and that builds the tool (default
# release/fid.exe; on Windows a path Python can start, not /c/...). Run
# from the repository root; needs python3 (or python).
set -u
FID="${FID:-release/fid.exe}"
export CC="${CC:-zig cc}"
OUT="${OUT:-${TMPDIR:-/tmp}/fid-reduce-check}"
mkdir -p "$OUT"
PY=python3
command -v "$PY" > /dev/null 2>&1 || PY=python
exe="$OUT/reduce"
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) exe="$exe.exe" ;; esac
if ! "$FID" ccompile tools/reduce.fd -o "$exe" > "$OUT/build.txt" 2>&1; then
    echo "FAIL: tools/reduce.fd does not build"; tail -5 "$OUT/build.txt"; exit 1
fi
S=tests/tools/reduce/sample
pass=0
fail=0
compare() {   # name, input, then the arguments both get
    local name="$1" in="$2"; shift 2
    local dst="$OUT/$name.reduced.fd"
    rm -f "$dst" "$dst.py" "$dst.fd"
    timeout 1800 "$PY" -X utf8 scripts/fuzz/reduce.py "$in" "$dst" --fid "$FID" "$@" > "$OUT/$name.py" 2>&1
    local pc=$?
    [ -f "$dst" ] && mv "$dst" "$dst.py"
    timeout 1800 "$exe" "$in" "$dst" --fid "$FID" "$@" > "$OUT/$name.fd" 2>&1
    local fc=$?
    [ -f "$dst" ] && mv "$dst" "$dst.fd"
    local same=1
    diff --strip-trailing-cr "$OUT/$name.py" "$OUT/$name.fd" > "$OUT/$name.diff" || same=0
    if [ -f "$dst.py" ] || [ -f "$dst.fd" ]; then
        cmp "$dst.py" "$dst.fd" >> "$OUT/$name.diff" 2>&1 || same=0
    fi
    if [ $same = 1 ] && [ $pc = $fc ]; then
        pass=$((pass + 1)); echo "ok   $name (exit $fc, $(tail -1 "$OUT/$name.fd" | tr -d '\r'))"
    else
        fail=$((fail + 1)); echo "FAIL $name (exit $pc vs $fc)"; head -20 "$OUT/$name.diff"
    fi
}
compare panic "$S/panic.fd" --fails run
compare panic-match "$S/panic.fd" --fails run --match "Division"
compare typeerr "$S/typeerr.fd" --cmd "$FID check {}" --match error
compare clock "$S/clock.fd" --bad run --good run
compare fine "$S/fine.fd" --fails run
echo "reduce port: $pass same, $fail differ"
[ $fail = 0 ]
