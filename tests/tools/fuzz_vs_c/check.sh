#!/usr/bin/env bash
# tools/fuzz_vs_c.fd (roadmap AD4) does what scripts/fuzz/gen_vs_c.py
# does: both run with the same arguments, one after the other, into the
# same --keep folder and log, and their output, exit status, kept files and
# log are compared ("\r\n" from Python on Windows counts as "\n"; with
# --jobs the seeds finish in any order, so those lines are sorted first).
#
#   bash tests/tools/fuzz_vs_c/check.sh
#   PAIRS=300 bash tests/tools/fuzz_vs_c/check.sh   # pairs of more seeds
#
# The pairs themselves: the Fides and C programs of seeds 1..PAIRS
# (default 100) with each of the default, --ext, --ext2 and --avoid none
# are compared byte for byte (`--print <seed>` against the script's
# pair()); the port draws its numbers from tools/py_random.fd, Python's
# MT19937, so a seed is the same pair in both.
#
# FID: the compiler both drive and that builds the tool (default
# release/fid.exe; on Windows a path Python can start, not /c/...).
# GEN_VS_C_MUTATE=run makes every seed differ (the script's self-test), so
# the kept .want/.out files and the log's "differs" lines are compared
# too; --cc gcc on a host without gcc (or, on Windows, without a WSL
# distribution) gives the "C oracle disagrees with itself" message with
# the results' repr(). Run from the repository root; needs python3 (or
# python) and zig. Python runs in UTF-8 mode (-X utf8: what it is on Linux
# and macOS): on Windows it otherwise decodes the children's output and
# writes the kept files in the console's code page (and fails with
# UnicodeEncodeError on a U+FFFD in a C oracle's output), where the port
# keeps UTF-8.
set -u
FID="${FID:-release/fid.exe}"
export CC="${CC:-zig cc}"
OUT="${OUT:-${TMPDIR:-/tmp}/fid-fuzz-vs-c-check}"
PAIRS="${PAIRS:-100}"
mkdir -p "$OUT"
PY=python3
command -v "$PY" > /dev/null 2>&1 || PY=python
exe="$OUT/fuzz_vs_c"
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) exe="$exe.exe" ;; esac
if ! "$FID" ccompile tools/fuzz_vs_c.fd -o "$exe" > "$OUT/build.txt" 2>&1; then
    echo "FAIL: tools/fuzz_vs_c.fd does not build"; tail -5 "$OUT/build.txt"; exit 1
fi
pass=0
fail=0
norm() {   # dir -> dir.n, without the compiler's timings
    rm -rf "$1.n"; mkdir -p "$1.n"
    (cd "$1" && find . -type f) | while read -r f; do
        mkdir -p "$(dirname "$1.n/$f")"
        sed -E 's/[0-9]+ ?ms/N ms/g' "$1/$f" > "$1.n/$f"
    done
}
compare() {   # name, sort (0/1), then the arguments both get
    local name="$1" srt="$2"; shift 2
    local k="$OUT/keep-$name" log="$OUT/$name.log"
    rm -rf "$k" "$k.py" "$k.fd" "$k.py.n" "$k.fd.n"; rm -f "$log"
    timeout 1800 "$PY" -X utf8 scripts/fuzz/gen_vs_c.py "$@" --fid "$FID" --keep "$k" --log "$log" > "$OUT/$name.py" 2>&1
    local pc=$?
    mv "$k" "$k.py"; [ -f "$log" ] && mv "$log" "$k.py/run.log"
    timeout 1800 "$exe" "$@" --fid "$FID" --keep "$k" --log "$log" > "$OUT/$name.fd" 2>&1
    local fc=$?
    mv "$k" "$k.fd"; [ -f "$log" ] && mv "$log" "$k.fd/run.log"
    if [ "$srt" = 1 ]; then
        for x in py fd; do
            tr -d '\r' < "$OUT/$name.$x" | sort > "$OUT/$name.$x.s"; mv "$OUT/$name.$x.s" "$OUT/$name.$x"
            [ -f "$k.$x/run.log" ] && { tr -d '\r' < "$k.$x/run.log" | sort > "$k.$x/run.s"; mv "$k.$x/run.s" "$k.$x/run.log"; }
        done
    fi
    norm "$k.py"; norm "$k.fd"
    if diff --strip-trailing-cr "$OUT/$name.py" "$OUT/$name.fd" > "$OUT/$name.diff" \
        && diff -r "$k.py.n" "$k.fd.n" >> "$OUT/$name.diff" && [ $pc = $fc ]; then
        pass=$((pass + 1)); echo "ok   $name (exit $fc, $(wc -l < "$OUT/$name.fd") lines, $(find "$k.fd" -type f | wc -l) kept files)"
    else
        fail=$((fail + 1)); echo "FAIL $name (exit $pc vs $fc)"; head -20 "$OUT/$name.diff"
    fi
}
compare ok 0 -n 2 --seed 1 --backends run --cc clang
GEN_VS_C_MUTATE=run compare differs 0 -n 2 --seed 3 --backends run,ccompile --cc clang --ext2
compare oracle 0 -n 1 --seed 9 --backends run --cc clang,gcc --ext
compare jobs 1 -n 4 --seed 20 --backends run --cc clang --jobs 2 --ext
compare all 0 -n 1 --seed 2 --backends all --cc clang --avoid none
# the pairs
p="$OUT/pairs"
rm -rf "$p"; mkdir -p "$p/py" "$p/fd"
"$PY" - "$p/py" "$PAIRS" <<'PY'
import sys
sys.path.insert(0, "scripts/fuzz")
import gen_vs_c as g
out, n = sys.argv[1], int(sys.argv[2])
for mode in ["plain", "ext", "ext2", "none"]:
    g.EXT[0] = mode in ("ext", "ext2")
    g.EXT2[0] = mode == "ext2"
    g.AVOID.clear()
    if mode != "none":
        g.AVOID.update(g.KNOWN)
    for s in range(1, n + 1):
        fd, c = g.pair(s)
        open(f"{out}/{mode}-{s}.txt", "w", newline="\n").write(fd + "\n// ---- C ----\n" + c + "\n")
PY
for s in $(seq 1 "$PAIRS"); do
    "$exe" --print "$s" > "$p/fd/plain-$s.txt"
    "$exe" --print "$s" --ext > "$p/fd/ext-$s.txt"
    "$exe" --print "$s" --ext2 > "$p/fd/ext2-$s.txt"
    "$exe" --print "$s" --avoid none > "$p/fd/none-$s.txt"
done
if diff -r "$p/py" "$p/fd" > "$p/diff.txt" && [ "$(ls "$p/fd" | wc -l)" = $((PAIRS * 4)) ]; then
    pass=$((pass + 1)); echo "ok   pairs ($((PAIRS * 4)) identical)"
else
    fail=$((fail + 1)); echo "FAIL pairs"; head -20 "$p/diff.txt"
fi
echo "fuzz_vs_c port: $pass same, $fail differ"
[ $fail = 0 ]
