#!/usr/bin/env bash
# The inputs of tools/fuzz_parsers.fd and scripts/fuzz/fuzz_parsers.py for
# one seed, written as each writes them to input.txt, compared byte for
# byte (check.sh runs it):
#   bash tests/tools/fuzz_parsers/inputs.sh <out dir> <seed> <count> [parsers]
# FID builds the dumper (the port with a main that writes the inputs
# instead of running them).
set -u
FID="${FID:-release/fid.exe}"
d="$1"; seed="$2"; count="$3"; parsers="${4:-json,regex,http,csv,toml,xml,url,uuid,tls}"
PY=python3
command -v "$PY" > /dev/null 2>&1 || PY=python
rm -rf "$d"; mkdir -p "$d/py" "$d/fd"
"$PY" -X utf8 - "$d/py" "$seed" "$count" "$parsers" <<'PY'
import random, sys
sys.path.insert(0, "scripts/fuzz")
from fuzz_parsers import SEEDS, BINARY, mutate
out, seed, n, parsers = sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), sys.argv[4].split(",")
r = random.Random(seed)
for i in range(n):
    which = parsers[i % len(parsers)]
    s = mutate(r, r.choice(SEEDS[which]))
    with open(f"{out}/{i}.txt", "wb") as f:
        if which in BINARY:
            f.write(s.encode("latin-1", "replace"))
        else:
            f.write(s.encode("utf-8", "surrogatepass"))
PY
sed 's/^fn main() -> i64 {/fn fp_main() -> i64 {/' tools/fuzz_parsers.fd > "$d/dump.fd"
cp tools/py_random.fd tools/fuzz_lib.fd "$d/"
cat >> "$d/dump.fd" <<'FD'

fn main() -> i64 {
    val av: Vec<str> = fl_args();
    val out: str = av.get(0);
    var r: PyRng = py_rng(fl_int(av.get(1), 1));
    val n: i64 = fl_int(av.get(2), 0);
    val parsers: Vec<str> = str_split(av.get(3), ",");
    val ints: Vec<Vec<i64>> = interesting();
    var i: i64 = 0;
    while i < n {
        val which: str = parsers.get(i % parsers.len());
        val sv: Vec<Vec<i64>> = seeds_of(which);
        val base: Vec<i64> = sv.get(py_below(&mut r, sv.len()));
        val s: Vec<i64> = mutate(&mut r, &base, &ints);
        val p: str = out + "/" + int_to_str(i) + ".txt";
        if which == "tls" { fs_write_bytes(p, latin1(&s)); } else { fl_write(p, fl_encode_all(&s)); }
        i = i + 1;
    }
    return 0;
}
FD
CC="${CC:-zig cc}" "$FID" ccompile "$d/dump.fd" -o "$d/dump.exe" > "$d/build.txt" 2>&1 || { echo "FAIL: the dumper does not build"; tail -5 "$d/build.txt"; exit 1; }
"$d/dump.exe" "$d/fd" "$seed" "$count" "$parsers"
diff -r "$d/py" "$d/fd" > "$d/diff.txt" && [ "$(ls "$d/fd" | wc -l)" = "$count" ]
