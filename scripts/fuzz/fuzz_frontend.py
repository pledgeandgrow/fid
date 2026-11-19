#!/usr/bin/env python3
"""Coverage-guided fuzzing of the lexer, parser and checkers (roadmap VF3).

    python scripts/fuzz/fuzz_frontend.py --fid <fid> [--cov-fid <instrumented fid>]
        [-n 2000] [--seed 1] [--corpus 'tests/bootstrap/*.fd'] [--out build/fuzz-fe]
        [--mode check|ir|emit] [--target x86_64-linux|nat|...] [--timeout 30]

Each input is a mutant of a corpus file: bytes deleted, duplicated or
inserted, tokens replaced by other tokens of the language, lines spliced in
from another file, the text cut short. It is given to `fid check` (lexer,
parser, type, borrow and effect checks; `--mode ir` also lowers to IR;
`--mode emit` runs the whole native pipeline, the IR optimizer's passes
and, with --target, the machine layer's: the optimizer passes of VF3).

No input may crash the compiler: a finding is an access violation or other
abnormal exit, a stack overflow, a `Panic:` inside the compiler (an
internal error, not a diagnostic about the program), or a hang (--timeout).
A normal error report with exit status 1 or 2 is the expected answer to a
broken program.

With --cov-fid (a compiler built with scripts/verify/cov_instrument.py) each
input also runs on the instrumented compiler, and an input that reaches IR
blocks no earlier input reached joins the corpus: the fuzzer then mutates
what got further. Without it the corpus stays the seed files (plain
mutational fuzzing). Findings go to <out>/crash-<n>.fd with the output.

Long runs (Nov 17): --keep <dir> keeps the grown corpus and the blocks
reached (<dir>/seen.bin) across runs, so a run of many batches (or one
cut short) goes on from where the last one stopped: the inputs kept by
earlier batches are mutated too, and only blocks no batch reached count
as new. A finding already filed in <out>/findings.tsv by an earlier run is
not filed again, and new ones are numbered after the crash-<n> files
already there. --summary <file> writes the run's numbers as JSON
(scripts/verify/vf3-cov-run.sh reads them).
"""
import argparse
import glob
import os
import random
import re
import subprocess
import sys
import tempfile
import time
import json
import shutil

TOKENS = """fn val var mut const return if else match struct enum impl pub use module
while for in loop break continue defer spawn async await comptime unsafe sandbox region
as is true false Vec HashMap Option Result Some None Ok Err i8 i16 i32 i64 u8 u16 u32 u64
f64 bool str void self Self cap dyn try guard select parallel txn handle do resume
+ - * / % = == != < <= > >= << >> & && | || ^ ! ~ ? ?? ?. -> => . .. ..= :: : ; , ( ) { } [ ]
@ # |> @wrapping @no-panic @linear @send @secret 0 1 -1 255 9223372036854775807 0xFF 1.5
"x" "" 'a' r"raw" f"{x}" x y z _ main println print""".split()

TOK_RE = re.compile(r'"(?:[^"\\]|\\.)*"|\'(?:[^\'\\]|\\.)*\'|[A-Za-z_][A-Za-z0-9_]*|\d+(?:\.\d+)?|'
                    r'\.\.=|\?\?|\?\.|->|=>|::|\|>|<<|>>|<=|>=|==|!=|&&|\|\||\S')


def mutate(r, src, corpus):
    k = r.randint(0, 9)
    if k <= 3:
        toks = [(m.start(), m.end()) for m in TOK_RE.finditer(src)]
        if not toks:
            return src
        for _ in range(r.randint(1, 4)):
            s, e = r.choice(toks)
            op = r.randint(0, 2)
            if op == 0:
                src = src[:s] + src[e:]
            elif op == 1:
                src = src[:s] + r.choice(TOKENS) + src[e:]
            else:
                src = src[:e] + " " + r.choice(TOKENS) + src[e:]
            toks = [(m.start(), m.end()) for m in TOK_RE.finditer(src)]
            if not toks:
                break
        return src
    if k <= 5:
        n = len(src)
        if n < 2:
            return src
        a = r.randrange(n)
        b = min(n, a + r.randint(1, 64))
        if r.random() < 0.5:
            return src[:a] + src[b:]
        return src[:b] + src[a:b] + src[b:]
    if k == 6:
        other = r.choice(corpus)
        ol = other.split("\n")
        sl = src.split("\n")
        i = r.randrange(len(sl) + 1)
        j = r.randrange(len(ol))
        return "\n".join(sl[:i] + ol[j:j + r.randint(1, 8)] + sl[i:])
    if k == 7:
        return src[: r.randrange(len(src) + 1)]
    if k == 8:
        # deep nesting: parsers that recurse must report, not overflow
        d = r.choice([50, 500, 5000])
        piece = r.choice([("(" * d, ")" * d), ("{" * d, "}" * d), ("[" * d, "]" * d), ("-" * d, ""), ("!" * d, "")])
        i = r.randrange(len(src) + 1)
        return src[:i] + piece[0] + "1" + piece[1] + src[i:]
    n = len(src)
    i = r.randrange(n + 1)
    return src[:i] + chr(r.choice([0, 1, 9, 10, 13, 34, 39, 92, 123, 125, 127, 0xe9, 0x2028])) + src[i:]


# "stack overflow" in lower case is also a diagnostic of @no-recursion, so
# only the runtime's own messages count
CRASH_MARKS = ("=== CRASH", "Access violation", "Stack overflow: the recursion",
               "Panic:", "Segmentation fault", "p_malloc: OOM")


def classify(rc, out):
    if rc == "timeout":
        return "hang"
    if isinstance(rc, int) and (rc < 0 or rc > 255):
        return f"abnormal exit {rc:#x}" if rc > 0 else f"signal {rc}"
    for m in CRASH_MARKS:
        if m in out:
            # a Panic in the compiler's own code shows a stack trace of
            # compiler functions; a diagnostic does not say Panic:
            return m.strip("= :")
    return None


def run(cmd, timeout, env=None):
    try:
        # stdin closed: a compiler that waited for input would be a hang
        p = subprocess.run(cmd, capture_output=True, timeout=timeout, env=env,
                           stdin=subprocess.DEVNULL)
        out = (p.stdout + p.stderr).decode("utf-8", "replace")
        return p.returncode, out
    except subprocess.TimeoutExpired:
        return "timeout", ""


def mode_cmd(fid, mode, path, target, tmp):
    """The command that gives `path` to the compiler in `mode`.

    check / ir: `fid <mode> <file>`. emit: the whole native pipeline,
    including the IR optimizer (compiler/iropt.fd) and, with --target
    <linux target> or --target nat, the machine layer's passes; the
    executable is written to a scratch file and never run (only the
    compiler is under test)."""
    if mode != "emit":
        return [fid, mode, path]
    cmd = [fid, "emit", path]
    if target == "nat":
        cmd.append("--nat")
    elif target:
        cmd += ["--target", target]
    return cmd + ["-o", os.path.join(tmp, "out.exe")]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--fid", required=True)
    ap.add_argument("--cov-fid", default=None)
    ap.add_argument("-n", type=int, default=500)
    ap.add_argument("--seed", type=int, default=1)
    ap.add_argument("--corpus", default="tests/bootstrap/*.fd")
    ap.add_argument("--out", default=os.path.join("build", "fuzz-fe"))
    ap.add_argument("--mode", default="check")
    ap.add_argument("--target", default="",
                    help="--mode emit: the target (e.g. x86_64-linux); default the host's")
    ap.add_argument("--timeout", type=int, default=30)
    ap.add_argument("--keep", default=None,
                    help="a directory that keeps the grown corpus and seen.bin across runs")
    ap.add_argument("--summary", default=None, help="write the run's numbers here as JSON")
    a = ap.parse_args()
    os.makedirs(a.out, exist_ok=True)
    started = time.time()
    r = random.Random(a.seed)
    files = sorted(glob.glob(a.corpus))
    corpus = []
    for p in files:
        with open(p, encoding="utf-8", errors="replace") as f:
            t = f.read()
        if len(t) < 20000:
            corpus.append(t)
    seeds = len(corpus)
    tmp = tempfile.mkdtemp(prefix="in", dir=a.out)
    seen = None
    covfile = os.path.join(tmp, "cov.bin")
    finds = {}
    grown = 0
    # a long run in batches: the corpus and the blocks earlier batches kept
    kept = 0
    seen_path = None
    if a.keep:
        os.makedirs(a.keep, exist_ok=True)
        for p in sorted(glob.glob(os.path.join(a.keep, "corpus-*.fd"))):
            with open(p, encoding="utf-8", errors="surrogateescape") as f:
                corpus.append(f.read())
            kept += 1
        seen_path = os.path.join(a.keep, "seen.bin")
        if os.path.exists(seen_path):
            with open(seen_path, "rb") as f:
                seen = bytearray(f.read())
    blocks_at_start = sum(bin(b).count("1") for b in seen) if seen else 0
    # findings earlier runs filed in this --out: not filed again
    ftab = os.path.join(a.out, "findings.tsv")
    old_keys = set()
    if os.path.exists(ftab):
        with open(ftab, encoding="utf-8") as f:
            for line in f:
                old_keys.add(line.rstrip("\n").split("\t")[0])
    next_crash = len(glob.glob(os.path.join(a.out, "crash-*.fd")))
    hangs = 0
    unconfirmed = 0

    def save_seen():
        if seen_path and seen is not None:
            with open(seen_path, "wb") as f:
                f.write(bytes(seen))
    for i in range(a.n):
        src = mutate(r, r.choice(corpus), corpus)
        if r.random() < 0.3:
            src = mutate(r, src, corpus)
        path = os.path.join(tmp, "input.fd")
        with open(path, "w", encoding="utf-8", errors="surrogateescape", newline="\n") as f:
            f.write(src)
        cmd = mode_cmd(a.fid, a.mode, path, a.target, tmp)
        rc, out = run(cmd, a.timeout)
        kind = classify(rc, out)
        if kind:
            # a finding must happen again (an OOM can be the machine's)
            rc2, out2 = run(cmd, a.timeout)
            if not classify(rc2, out2):
                print(f"[{i}] not reproducible ({kind}), dropped", flush=True)
                unconfirmed += 1
                kind = None
        if kind == "hang":
            hangs += 1
        if kind:
            # the first line that names the problem is the key
            key = kind + ":" + next((l for l in out.split("\n") if any(m in l for m in CRASH_MARKS)), "")[:80]
            if "Stack overflow" not in key:
                # one bug per place in the compiler; a stack overflow from
                # deep nesting is one bug wherever the recursion is
                # the trace's lines: "→ f" / "↳ called from f" (older) and
                # "    - f" / "    -| called from f" (the C runtime's, Nov 16)
                trace = re.findall(r"(?:→|↳ called from|-\| called from|^[ \t]+-)[ \t]+([A-Za-z_][A-Za-z0-9_]*)",
                                   out, re.M)
                key += ":" + "/".join(trace[:3])
            key = key.replace("\t", " ").replace("\r", "")
            if key in old_keys and key not in finds:
                # filed by an earlier batch of this run
                finds[key] = -1
                print(f"[{i}] known finding: {key}", flush=True)
            if key not in finds:
                n = next_crash
                next_crash += 1
                finds[key] = n
                with open(ftab, "a", encoding="utf-8") as f:
                    f.write(f"{key}\tcrash-{n}.fd\t{a.mode}{(':' + a.target) if a.target else ''}\tseed {a.seed} input {i}\n")
                with open(os.path.join(a.out, f"crash-{n}.fd"), "w", encoding="utf-8",
                          errors="surrogateescape", newline="\n") as f:
                    f.write(src)
                with open(os.path.join(a.out, f"crash-{n}.out"), "w", encoding="utf-8") as f:
                    f.write(f"exit: {rc}\nkind: {kind}\n{out[-6000:]}")
                print(f"[{i}] new finding {n}: {key}", flush=True)
        if a.cov_fid and not kind:
            env = dict(os.environ, FID_COV_OUT=covfile)
            if os.path.exists(covfile):
                os.remove(covfile)
            run(mode_cmd(a.cov_fid, a.mode, path, a.target, tmp), a.timeout, env)
            if os.path.exists(covfile):
                with open(covfile, "rb") as f:
                    bits = f.read()
                if seen is not None and len(seen) != len(bits):
                    # --keep from a different compiler build: its block
                    # numbers mean other blocks, start the bitmap again
                    print(f"[{i}] seen.bin has {len(seen)} bytes, the compiler {len(bits)}: reset", flush=True)
                    seen = None
                    blocks_at_start = 0
                if seen is None:
                    seen = bytearray(len(bits))
                new = sum(bin(b & ~s & 0xFF).count("1") for b, s in zip(bits, seen))
                if new:
                    for j, b in enumerate(bits):
                        seen[j] |= b
                    # the first input of a fresh run only sets the baseline
                    if i > 0 or blocks_at_start:
                        corpus.append(src)
                        grown += 1
                        name = (f"corpus-{a.mode}{a.target}-s{a.seed}-{i}.fd" if a.keep
                                else f"corpus-{grown}.fd")
                        with open(os.path.join(a.keep or a.out, name), "w", encoding="utf-8",
                                  errors="surrogateescape", newline="\n") as f:
                            f.write(src)
        if (i + 1) % 50 == 0:
            cov = sum(bin(b).count("1") for b in seen) if seen else 0
            save_seen()
            print(f"[{i + 1}/{a.n}] findings {len(finds)}, corpus {seeds}+{kept}+{grown}, blocks reached {cov}",
                  flush=True)
    save_seen()
    shutil.rmtree(tmp, ignore_errors=True)
    cov = sum(bin(b).count("1") for b in seen) if seen else 0
    new_finds = sorted(k for k, n in finds.items() if n >= 0)
    print(f"{a.n} inputs: {len(finds)} distinct findings ({len(new_finds)} new), "
          f"corpus {seeds}+{kept}+{grown}, blocks reached {cov}")
    if a.summary:
        with open(a.summary, "w", encoding="utf-8") as f:
            json.dump({"mode": a.mode, "target": a.target, "seed": a.seed, "inputs": a.n,
                       "coverage": bool(a.cov_fid), "seconds": round(time.time() - started),
                       "findings": len(finds), "new_findings": new_finds,
                       "known_findings": sorted(k for k, n in finds.items() if n < 0),
                       "hangs": hangs, "unconfirmed": unconfirmed,
                       "corpus_seeds": seeds, "corpus_kept": kept, "corpus_grown": grown,
                       "blocks_at_start": blocks_at_start, "blocks_reached": cov}, f, indent=1)
    sys.exit(1 if finds else 0)


if __name__ == "__main__":
    main()
