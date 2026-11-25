#!/usr/bin/env bash
# The mutants of tools/fuzz_frontend.fd and scripts/fuzz/fuzz_frontend.py
# for one seed and corpus, compared byte for byte (check.sh runs it):
#   bash tests/tools/fuzz_frontend/mutants.sh <out dir> <corpus glob> <seed> <count>
# FID builds the dumper (the port with a main that writes the mutants
# instead of running them).
set -u
FID="${FID:-release/fid.exe}"
d="$1"; corpus="$2"; seed="$3"; count="$4"
PY=python3
command -v "$PY" > /dev/null 2>&1 || PY=python
rm -rf "$d"; mkdir -p "$d/py" "$d/fd"
"$PY" -X utf8 - "$d/py" "$corpus" "$seed" "$count" <<'PY'
import glob, random, sys
sys.path.insert(0, "scripts/fuzz")
from fuzz_frontend import mutate
out, pat, seed, n = sys.argv[1], sys.argv[2], int(sys.argv[3]), int(sys.argv[4])
r = random.Random(seed)
corpus = []
for p in sorted(glob.glob(pat)):
    with open(p, encoding="utf-8", errors="replace") as f:
        t = f.read()
    if len(t) < 20000:
        corpus.append(t)
for i in range(n):
    src = mutate(r, r.choice(corpus), corpus)
    if r.random() < 0.3:
        src = mutate(r, src, corpus)
    with open(f"{out}/{i}.fd", "w", encoding="utf-8", errors="surrogateescape", newline="\n") as f:
        f.write(src)
PY
sed 's/^fn main() -> i64 {/fn fe_main() -> i64 {/' tools/fuzz_frontend.fd > "$d/dump.fd"
cp tools/py_random.fd tools/fuzz_lib.fd "$d/"
cat >> "$d/dump.fd" <<'FD'

fn main() -> i64 {
    val av: Vec<str> = fl_args();
    val out: str = av.get(0);
    var r: PyRng = py_rng(fl_int(av.get(2), 1));
    val words: Vec<str> = tokens();
    val files: Vec<str> = glob_files(av.get(1));
    var corpus: Vec<Vec<i64>> = Vec::new();
    var fi: i64 = 0;
    while fi < files.len() {
        val t: Vec<i64> = read_text(files.get(fi));
        if t.len() < 20000 { corpus.push(t); }
        fi = fi + 1;
    }
    val n: i64 = fl_int(av.get(3), 0);
    var i: i64 = 0;
    while i < n {
        val base: Vec<i64> = corpus.get(py_below(&mut r, corpus.len()));
        var src: Vec<i64> = mutate(&mut r, &base, &corpus, &words);
        if py_random(&mut r) < 0.3 {
            src = mutate(&mut r, &src, &corpus, &words);
        }
        write_cps(out + "/" + int_to_str(i) + ".fd", &src);
        i = i + 1;
    }
    return 0;
}
FD
CC="${CC:-zig cc}" "$FID" ccompile "$d/dump.fd" -o "$d/dump.exe" > "$d/build.txt" 2>&1 || { echo "FAIL: the dumper does not build"; tail -5 "$d/build.txt"; exit 1; }
"$d/dump.exe" "$d/fd" "$corpus" "$seed" "$count"
diff -r "$d/py" "$d/fd" > "$d/diff.txt" && [ "$(ls "$d/fd" | wc -l)" = "$count" ]
