#!/usr/bin/env bash
# tools/fuzz_gen.fd (roadmap AD4) does what scripts/fuzz/gen_programs.py
# does: both run with the same arguments, one after the other, into the
# same --keep folder (so the paths they print and the compiler messages
# they keep are the same), and their output, exit status and kept files
# are compared ("\r\n" from Python on Windows counts as "\n").
#
#   bash tests/tools/fuzz_gen/check.sh
#
# The programs themselves: GEN=1 also writes the programs of seeds 1..400,
# plain and --rich, and two batches with both and compares them byte for
# byte (the port draws its numbers from tools/py_random.fd, Python's
# MT19937, so a seed is the same program in both).
#
# FID: the compiler both drive and that builds the tool (default
# release/fid.exe; on Windows a path Python can start, not /c/...). The
# `check` backend builds no executable, so every program it gets counts as
# a disagreement: that is how the keep, retry and batch-splitting paths
# are exercised without a miscompile. Run from the repository root; needs
# python3 (or python).
set -u
FID="${FID:-release/fid.exe}"
export CC="${CC:-zig cc}"
OUT="${OUT:-${TMPDIR:-/tmp}/fid-fuzz-gen-check}"
mkdir -p "$OUT"
PY=python3
command -v "$PY" > /dev/null 2>&1 || PY=python
exe="$OUT/fuzz_gen"
case "$(uname -s 2>/dev/null)" in MINGW*|MSYS*|CYGWIN*) exe="$exe.exe" ;; esac
if ! CC="${CC:-zig cc}" "$FID" ccompile tools/fuzz_gen.fd -o "$exe" > "$OUT/build.txt" 2>&1; then
    echo "FAIL: tools/fuzz_gen.fd does not build"; tail -5 "$OUT/build.txt"; exit 1
fi
pass=0
fail=0
# the kept files without the compiler's timings ("[prof] mono: 5ms",
# "no errors (101 ms)"), which differ from run to run
norm() {   # dir -> dir.n
    rm -rf "$1.n"; mkdir -p "$1.n"
    (cd "$1" && find . -type f) | while read -r f; do
        mkdir -p "$(dirname "$1.n/$f")"
        sed -E 's/[0-9]+ ?ms/N ms/g' "$1/$f" > "$1.n/$f"
    done
}
compare() {   # name, then the arguments both get (--keep is added)
    local name="$1"; shift
    local k="$OUT/keep-$name"
    rm -rf "$k" "$k.py" "$k.fd"
    timeout 1800 "$PY" scripts/fuzz/gen_programs.py "$@" --fid "$FID" --keep "$k" > "$OUT/$name.py" 2>&1
    local pc=$?
    mv "$k" "$k.py"
    timeout 1800 "$exe" "$@" --fid "$FID" --keep "$k" > "$OUT/$name.fd" 2>&1
    local fc=$?
    mv "$k" "$k.fd"
    norm "$k.py"; norm "$k.fd"
    if diff --strip-trailing-cr "$OUT/$name.py" "$OUT/$name.fd" > "$OUT/$name.diff" \
        && diff -r "$k.py.n" "$k.fd.n" >> "$OUT/$name.diff" && [ $pc = $fc ]; then
        pass=$((pass + 1)); echo "ok   $name (exit $fc, $(wc -l < "$OUT/$name.fd") lines, $(find "$k.fd" -type f | wc -l) kept files)"
    else
        fail=$((fail + 1)); echo "FAIL $name (exit $pc vs $fc)"; head -20 "$OUT/$name.diff"
    fi
}
compare agree -n 2 --seed 1 --backends run,ccompile
compare keep -n 2 --seed 5 --backends run,check --retries 2
compare rich-batch -n 5 --seed 11 --backends run,check --rich --batch 3
compare batch-agree -n 4 --seed 30 --backends run,run --batch 2
if [ "${GEN:-0}" = 1 ]; then
    g="$OUT/gen"
    rm -rf "$g"; mkdir -p "$g/py" "$g/fd"
    "$PY" - "$g/py" <<'PY'
import random, sys
sys.path.insert(0, "scripts/fuzz")
from gen_programs import Gen, batch_program
out = sys.argv[1]
for s in range(1, 401):
    open(f"{out}/p{s}.fd", "w", newline="\n").write(Gen(random.Random(s)).program())
    open(f"{out}/r{s}.fd", "w", newline="\n").write(Gen(random.Random(s), True).program())
open(f"{out}/b.fd", "w", newline="\n").write(batch_program([7, 8, 9, 10], False))
open(f"{out}/br.fd", "w", newline="\n").write(batch_program([7, 8, 9, 10], True))
PY
    # the port with a main that writes the same files
    sed 's/^fn main() -> i64 {/fn gen_main() -> i64 {/' tools/fuzz_gen.fd > "$g/gd.fd"
    cp tools/py_random.fd tools/fuzz_lib.fd "$g/"
    cat >> "$g/gd.fd" <<'FD'

fn main() -> i64 {
    val out: str = fl_args().get(0);
    var s: i64 = 1;
    while s <= 400 {
        fl_write(out + "/p" + int_to_str(s) + ".fd", gen_program(s, false));
        fl_write(out + "/r" + int_to_str(s) + ".fd", gen_program(s, true));
        s = s + 1;
    }
    var b: Vec<i64> = Vec::new();
    b.push(7); b.push(8); b.push(9); b.push(10);
    fl_write(out + "/b.fd", batch_program(&b, false));
    fl_write(out + "/br.fd", batch_program(&b, true));
    return 0;
}
FD
    if CC="${CC:-zig cc}" "$FID" ccompile "$g/gd.fd" -o "$g/gd.exe" > "$g/build.txt" 2>&1 \
        && "$g/gd.exe" "$g/fd" && diff -r "$g/py" "$g/fd" > "$g/diff.txt"; then
        pass=$((pass + 1)); echo "ok   programs (802 files identical)"
    else
        fail=$((fail + 1)); echo "FAIL programs"; head -20 "$g/diff.txt" "$g/build.txt" 2>/dev/null
    fi
fi
echo "fuzz_gen port: $pass same, $fail differ"
[ $fail = 0 ]
