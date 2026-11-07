#!/usr/bin/env python3
"""Block and function coverage of compiler/*.fd over a test run (roadmap VF8).

    # 1. build a compiler with counters (scripts/verify/cov_instrument.py):
    INSTRUMENT=scripts/verify/cov_instrument.py FID=release/fid.exe \
        scripts/verify/build-variant.sh . build/cov/fid-cov.exe
    # 2. run the tests through it and report:
    python scripts/verify/coverage.py --fid build/cov/fid-cov.exe \
        --map build/cov/fid-cov.c.inst.map [--modes check,run,ccompile,emit] [-j 4]
        [--tests 'tests/bootstrap/*.fd'] [--out build/cov]

Each test is run once per mode with FID_COV_DIR set; the instrumented
compiler writes the blocks it reached; they are merged and mapped back to
the module that defines each function (compiler/*.fd, stdlib/core.fd).

What is measured: IR basic blocks (every branch target is one, so block
coverage is branch coverage at the IR level) and function entries. The
generated C carries no source lines, so this is not line coverage.

--report-only re-reads the merged bitmap (<out>/merged.cov) without
running anything.
"""
import argparse
import glob
import os
import re
import subprocess
import sys
import tempfile
from concurrent.futures import ThreadPoolExecutor


def fd_functions(paths):
    """Map each function's C name to the .fd file that defines it."""
    owner = {}
    fn_re = re.compile(r"^\s*(?:pub\s+)?(?:async\s+|comptime\s+)?fn\s+([A-Za-z_][A-Za-z0-9_]*)")
    impl_re = re.compile(r"^\s*impl\s*(?:<[^>]*>\s*)?(?:([A-Za-z_][A-Za-z0-9_]*)\s+for\s+)?([A-Za-z_][A-Za-z0-9_]*)")
    # Items start in column 0 (the compiler's own style): a top-level `fn`,
    # or an `impl` block whose methods are indented and which ends at a `}`
    # in column 0. (Counting braces is fooled by braces in strings.)
    for p in paths:
        mod = os.path.relpath(p).replace("\\", "/")
        # Some files (parser.fd) write an impl block's methods in column 0
        # too, so a method is also any fn taking `self` after an `impl`.
        impl = None
        with open(p, encoding="utf-8", errors="replace") as f:
            for line in f:
                m = impl_re.match(line)
                if m and not line[0].isspace():
                    impl = m.group(2)
                    continue
                m = fn_re.match(line)
                if not m:
                    continue
                name = m.group(1)
                if not line[0].isspace():
                    owner.setdefault(name, mod)
                if impl and (line[0].isspace() or "(self" in line):
                    owner.setdefault(f"{impl}_{name}", mod)
    return owner


def module_of(fn, owner):
    if fn in owner:
        return owner[fn]
    # monomorphised copies: Name__T, Name_T_U
    base = fn
    while "_" in base:
        base = base.rsplit("_", 1)[0]
        if base in owner:
            return owner[base]
    return None


def load_map(path):
    entries = []
    with open(path) as f:
        for line in f:
            parts = line.split()
            if len(parts) == 3:
                entries.append((int(parts[0]), parts[1], parts[2]))
    return entries


def run_one(fid, mode, test, covdir, work):
    env = dict(os.environ, FID_COV_DIR=covdir, FIDES_NO_CACHE="1")
    env.setdefault("CC", "zig cc")
    exe = os.path.join(work, os.path.basename(test) + f".{mode}.exe")
    if mode in ("run", "check"):
        cmd = [fid, mode, test]
    else:
        cmd = [fid, mode, test, "-o", exe]
    try:
        p = subprocess.run(cmd, capture_output=True, timeout=600, env=env)
        rc = p.returncode
    except subprocess.TimeoutExpired:
        rc = "timeout"
    for e in (exe, exe.replace(".exe", ".pdb")):
        if os.path.exists(e):
            try:
                os.remove(e)
            except OSError:
                pass
    return test, mode, rc


def merge(covdir, nbits, into=None):
    acc = bytearray(into) if into else bytearray((nbits + 7) // 8)
    for p in glob.glob(os.path.join(covdir, "*.cov")):
        with open(p, "rb") as f:
            b = f.read()
        for i in range(min(len(b), len(acc))):
            acc[i] |= b[i]
    return acc


def report(acc, entries, owner, out):
    hit = lambda i: (acc[i >> 3] >> (i & 7)) & 1
    mods = {}
    for i, fn, blk in entries:
        mod = module_of(fn, owner) or "(runtime / unmapped)"
        m = mods.setdefault(mod, {"blocks": 0, "bhit": 0, "fns": 0, "fhit": 0, "missed": []})
        m["blocks"] += 1
        m["bhit"] += hit(i)
        if blk == "entry":
            m["fns"] += 1
            m["fhit"] += hit(i)
            if not hit(i):
                m["missed"].append(fn)
    lines = []
    tb = th = tf = tfh = 0
    lines.append(f"{'module':34} {'blocks':>15} {'functions':>15}")
    for mod in sorted(mods):
        m = mods[mod]
        if mod.startswith("("):
            continue
        tb += m["blocks"]; th += m["bhit"]; tf += m["fns"]; tfh += m["fhit"]
        lines.append(f"{mod:34} {m['bhit']:>6}/{m['blocks']:<6} {100 * m['bhit'] / max(1, m['blocks']):5.1f}%"
                     f" {m['fhit']:>5}/{m['fns']:<5} {100 * m['fhit'] / max(1, m['fns']):5.1f}%")
    lines.append(f"{'TOTAL (compiler/*.fd + core)':34} {th:>6}/{tb:<6} {100 * th / max(1, tb):5.1f}%"
                 f" {tfh:>5}/{tf:<5} {100 * tfh / max(1, tf):5.1f}%")
    u = mods.get("(runtime / unmapped)")
    if u:
        lines.append(f"(not counted: {u['fns']} C functions with no .fd source: the C runtime, generated helpers)")
    text = "\n".join(lines) + "\n"
    with open(os.path.join(out, "report.txt"), "w") as f:
        f.write(text)
    with open(os.path.join(out, "never-called.txt"), "w") as f:
        for mod in sorted(mods):
            if not mod.startswith("("):
                for fn in mods[mod]["missed"]:
                    f.write(f"{mod} {fn}\n")
    return text


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--fid", required=True)
    ap.add_argument("--map", required=True)
    ap.add_argument("--modes", default="check,run,ccompile,emit")
    ap.add_argument("--tests", default="tests/bootstrap/*.fd")
    ap.add_argument("--out", default=os.path.join("build", "cov"))
    ap.add_argument("-j", type=int, default=4)
    ap.add_argument("--limit", type=int, default=0)
    ap.add_argument("--report-only", action="store_true")
    a = ap.parse_args()
    os.makedirs(a.out, exist_ok=True)
    entries = load_map(a.map)
    nbits = max(i for i, _, _ in entries) + 1
    owner = fd_functions(sorted(glob.glob("compiler/*.fd")) + ["stdlib/core.fd"])
    merged_path = os.path.join(a.out, "merged.cov")
    acc = None
    if os.path.exists(merged_path):
        with open(merged_path, "rb") as f:
            acc = f.read()
    if not a.report_only:
        tests = sorted(glob.glob(a.tests))
        if a.limit:
            tests = tests[: a.limit]
        covdir = tempfile.mkdtemp(prefix="fidcov", dir=a.out)
        work = tempfile.mkdtemp(prefix="work", dir=a.out)
        jobs = [(m, t) for t in tests for m in a.modes.split(",")]
        done = 0
        with ThreadPoolExecutor(a.j) as ex:
            for t, m, rc in ex.map(lambda mt: run_one(a.fid, mt[0], mt[1], covdir, work), jobs):
                done += 1
                if done % 50 == 0:
                    print(f"  {done}/{len(jobs)}", flush=True)
                    acc = bytes(merge(covdir, nbits, acc))
                    for p in glob.glob(os.path.join(covdir, "*.cov")):
                        os.remove(p)
        acc = bytes(merge(covdir, nbits, acc))
        for p in glob.glob(os.path.join(covdir, "*.cov")):
            os.remove(p)
        with open(merged_path, "wb") as f:
            f.write(acc)
        print(f"{len(tests)} tests x {a.modes}")
    print(report(acc, entries, owner, a.out))


if __name__ == "__main__":
    main()
