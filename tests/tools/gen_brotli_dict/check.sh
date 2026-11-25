#!/usr/bin/env bash
# tools/gen_brotli_dict.fd (roadmap AD4) writes and prints what
# scripts/gen-brotli-dict.py does: both run on the same inputs, each
# writing its own output file, and the files, stdout, stderr and exit
# status are compared ("\r\n" from Python on Windows counts as "\n").
#
#   bash tests/tools/gen_brotli_dict/check.sh      # seconds
#
# The dictionary is not in the repository: it is decoded from
# stdlib/brotli_dict.fd's literals (the script's own round trip, in
# Python). Cases: the dictionary (the file both write must be the same
# byte for byte), one byte changed (the sha256 warning, then the file), a
# file of the wrong size, no argument (the docstring), a missing file and
# a directory (the script's traceback ends in the port's one line: only
# that last line is compared). The default output (stdlib/brotli_dict.fd)
# is not tried: it would write into the tree.
#
# FID: the compiler that builds the tool (default release/fid.exe).
# Run from the repository root; needs python3 (or python).
set -u
FID="${FID:-release/fid.exe}"
OUT="${OUT:-${TMPDIR:-/tmp}/fid-gen-brotli-dict-check}"
mkdir -p "$OUT"
PY=python3
command -v "$PY" > /dev/null 2>&1 || PY=python
"$PY" -c "import sys" > /dev/null 2>&1 || PY=python
exe="$OUT/gen_brotli_dict"
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) exe="$exe.exe" ;; esac
if ! "$FID" ccompile tools/gen_brotli_dict.fd -o "$exe" > "$OUT/build.txt" 2>&1; then
    echo "FAIL: tools/gen_brotli_dict.fd does not build"; tail -5 "$OUT/build.txt"; exit 1
fi
# the inputs: the dictionary, one byte changed, a short file
"$PY" - "$OUT" <<'PY'
import base64, re, sys
t = open("stdlib/brotli_dict.fd").read()
b = b"".join(base64.b64decode(x) for x in re.findall(r'return "([A-Za-z0-9+/=]+)";', t))
open(sys.argv[1] + "/dictionary.bin", "wb").write(b)
open(sys.argv[1] + "/changed.bin", "wb").write(b[:1000] + bytes([b[1000] ^ 1]) + b[1001:])
open(sys.argv[1] + "/short.bin", "wb").write(b[:5000])
PY
mkdir -p "$OUT/adir"
pass=0
fail=0
# compare <name> <last-line-of-stderr-only: 0|1> <args...>; "@OUT@" in an
# argument becomes <OUT>/<name>.py.fd or <name>.fd.fd for each side
compare() {
    local name="$1" lastline="$2"; shift 2
    local d args pc fc bad=0
    for d in py fd; do
        rm -f "$OUT/$name.$d.fd"
        args=()
        for a in "$@"; do args+=("${a//@OUT@/$OUT/$name.$d.fd}"); done
        if [ $d = py ]; then
            "$PY" scripts/gen-brotli-dict.py "${args[@]}" > "$OUT/$name.$d.out" 2> "$OUT/$name.$d.err"; pc=$?
        else
            "$exe" "${args[@]}" > "$OUT/$name.$d.out" 2> "$OUT/$name.$d.err"; fc=$?
        fi
        sed -i -e 's/\r$//' -e "s#$name\.$d\.fd#OUTFILE#g" "$OUT/$name.$d.out" "$OUT/$name.$d.err"
        if [ "$lastline" = 1 ]; then tail -1 "$OUT/$name.$d.err" > "$OUT/$name.$d.err1"; mv "$OUT/$name.$d.err1" "$OUT/$name.$d.err"; fi
    done
    diff "$OUT/$name.py.out" "$OUT/$name.fd.out" > "$OUT/$name.diff" || bad=1
    diff "$OUT/$name.py.err" "$OUT/$name.fd.err" >> "$OUT/$name.diff" || bad=1
    [ $pc = $fc ] || bad=1
    if [ -f "$OUT/$name.py.fd" ] || [ -f "$OUT/$name.fd.fd" ]; then
        cmp "$OUT/$name.py.fd" "$OUT/$name.fd.fd" >> "$OUT/$name.diff" 2>&1 || { bad=1; echo "  (written files differ)" >> "$OUT/$name.diff"; }
    fi
    if [ $bad = 0 ]; then
        pass=$((pass + 1)); echo "ok   $name (exit $fc, $(cat "$OUT/$name.fd.out" "$OUT/$name.fd.err" | wc -l) lines)"
    else
        fail=$((fail + 1)); echo "FAIL $name (exit $pc vs $fc)"; head -20 "$OUT/$name.diff"
    fi
}
compare dict 0 "$OUT/dictionary.bin" @OUT@
compare changed 0 "$OUT/changed.bin" @OUT@ extra-ignored
compare short 0 "$OUT/short.bin" @OUT@
compare noargs 0
compare missing 1 "$OUT/no-such.bin" @OUT@
compare directory 1 "$OUT/adir" @OUT@
compare no-out-dir 1 "$OUT/dictionary.bin" "$OUT/no-such-dir/x.fd"
echo "gen_brotli_dict port: $pass same, $fail differ"
[ $fail = 0 ]
