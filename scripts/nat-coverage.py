#!/usr/bin/env python3
"""Line and branch coverage of the machine layer over the suite (own-backend
roadmap RL12): compiler/nat_ir.fd, nat_enc.fd, rules.fd and egraph.fd by
default (the four modules of RL12's 90% goal).

    # everything: build a counter compiler, run the suite through it, report
    python3 scripts/nat-coverage.py --base-fid release/fid.exe [-j 4] [--gen 20]
    # with a counter compiler built before (its map beside it)
    python3 scripts/nat-coverage.py --fid build/nat-coverage/fid-cov.exe \
        --map build/nat-coverage/fid-cov.c.inst.map
    python3 scripts/nat-coverage.py --report-only [--out build/nat-coverage]
    python3 scripts/nat-coverage.py --self-test

The counter compiler is the tree's compiler built through C by
scripts/verify/build-variant.sh with scripts/nat-cov-instrument.py over its
C and FID_C_LINE_MARKS=1, so the C generator marks the source line of each
instruction (compiler/codegen_c.fd) and every marked line gets a counter
beside every block. Each test is then compiled once per mode (default
`emit --target` x86_64-linux, aarch64-linux and riscv64-linux, and
`emit --nat`: only the compiler runs, no target binary). Then the
variants (VARIANTS below; --variants none: off) compile samples of the
tests with the switches a plain build never sets (FID_VERIFY, FID_NAT_PAR,
the opt-in passes, the cache, --obj, --cpu levels, macOS, -g, each
FID_NAT_OFF pass of docs/verification/kill-switches.md) and `fid
test-rules [--smt]` runs once. The bitmaps the counter compiler writes are
merged, and per module:

  lines      source lines with a counter that ran / source lines with a
             counter, plus the lines of the module's functions that have no
             C at all (never reached by the C generator: counted as not run)
  blocks     IR basic blocks entered / blocks (every branch target is a
             block: branch coverage at the IR level)
  functions  functions entered / functions

When the C carries no line marks (a compiler without FID_C_LINE_MARKS),
"lines" is the estimate of scripts/verify/coverage.py (the lines of entered
functions, weighted by the share of their blocks that ran) and is labelled
so. --goal P exits 1 when a module's line or block coverage is below P%.
--record <file.md> appends the result as a table row (date, tests, modes,
each module's figures).

Files in --out: merged.cov (the merged bitmap; --report-only re-reads it),
report.txt, uncovered-<module>.txt (the line numbers no test reached),
gaps-<module>.txt (the functions with lines or blocks not reached, the
largest gap first: where the next test or fuzzer program should aim).
"""
import argparse
import datetime
import glob
import os
import subprocess
import sys
import tempfile
from concurrent.futures import ThreadPoolExecutor

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
sys.path.insert(0, os.path.join(HERE, "verify"))
import coverage as cv  # noqa: E402  (scripts/verify/coverage.py)

# the four modules RL12's goal names (coverage.py GOAL_MODULES)
DEFAULT_MODULES = cv.GOAL_MODULES
DEFAULT_MODES = "emit:x86_64-linux,emit:aarch64-linux,emit:riscv64-linux,nat"
BASH = os.environ.get("BASH_EXE") or next(
    (p for p in (r"C:\Program Files\Git\bin\bash.exe", r"C:\Program Files\Git\usr\bin\bash.exe")
     if os.name == "nt" and os.path.exists(p)), "bash")


def load_map(path):
    """[(id, function, kind)] with kind 'entry', 'bbN' or 'L<n>'."""
    out = []
    with open(path) as f:
        for line in f:
            p = line.split()
            if len(p) == 3:
                out.append((int(p[0]), p[1], p[2]))
    return out


def src_of(fn, lines_of):
    """The source function a C function comes from (monomorphised copies
    carry suffixes), or None."""
    if fn in lines_of:
        return fn
    base = fn
    while "_" in base:
        base = base.rsplit("_", 1)[0]
        if base in lines_of:
            return base
    return None


def figures(acc, entries, owner, lines_of, modules):
    """{module: dict(lines_hit, lines_all, lines_absent, blocks_hit, blocks,
    fns_hit, fns, real_lines, uncovered)}."""
    hit = lambda i: (acc[i >> 3] >> (i & 7)) & 1 if (i >> 3) < len(acc) else 0
    res = {m: dict(lh=set(), la=set(), absent=0, bh=0, b=0, fh=0, f=0) for m in modules}
    present = set()
    for i, fn, kind in entries:
        s = src_of(fn, lines_of)
        if s is not None:
            present.add(s)
        mod = cv.module_of(fn, owner)
        if mod not in res:
            continue
        r = res[mod]
        if kind.startswith("L"):
            ln = int(kind[1:])
            r["la"].add(ln)
            if hit(i):
                r["lh"].add(ln)
        else:
            r["b"] += 1
            r["bh"] += hit(i)
            if kind == "entry":
                r["f"] += 1
                r["fh"] += hit(i)
    real = any(k.startswith("L") for _, _, k in entries)
    # functions of the module that have no C: their lines were never run
    for fn, (mod, n) in lines_of.items():
        if mod in res and fn not in present:
            res[mod]["absent"] += n
    out = {}
    est = cv.line_report(acc, [(i, f, k) for i, f, k in entries if not k.startswith("L")], lines_of) if not real else {}
    for m, r in res.items():
        d = dict(blocks_hit=r["bh"], blocks=r["b"], fns_hit=r["fh"], fns=r["f"], real_lines=real)
        if real:
            d["lines_hit"] = len(r["lh"])
            d["lines_all"] = len(r["la"]) + r["absent"]
            d["lines_absent"] = r["absent"]
            d["uncovered"] = sorted(r["la"] - r["lh"])
        else:
            n, reached, weighted = est.get(m, (0, 0, 0.0))
            d["lines_hit"] = int(weighted)
            d["lines_all"] = n
            d["lines_absent"] = 0
            d["uncovered"] = []
        out[m] = d
    return out


def pct(a, b):
    return 100.0 * a / b if b else 0.0


def render(figs, ntests, modes):
    rows = [f"{'module':24} {'lines':>22} {'blocks (branches)':>22} {'functions':>18}"]
    for m, d in figs.items():
        lab = "" if d["real_lines"] else " est."
        rows.append(f"{m:24} {d['lines_hit']:>7}/{d['lines_all']:<7} {pct(d['lines_hit'], d['lines_all']):5.1f}%{lab:5}"
                    f" {d['blocks_hit']:>7}/{d['blocks']:<7} {pct(d['blocks_hit'], d['blocks']):5.1f}%"
                    f" {d['fns_hit']:>6}/{d['fns']:<6} {pct(d['fns_hit'], d['fns']):5.1f}%")
    if figs and not next(iter(figs.values()))["real_lines"]:
        rows.append("lines est.: the C has no line marks (build the counter compiler with FID_C_LINE_MARKS=1);")
        rows.append("  the figure is coverage.py's block-weighted estimate, not a line count")
    else:
        for m, d in figs.items():
            if d["lines_absent"]:
                rows.append(f"{m}: {d['lines_absent']} lines in functions with no C, counted as not run")
    rows.append(f"{ntests} tests x {modes}")
    return "\n".join(rows) + "\n"


def build_counter_compiler(base_fid, out):
    exe = os.path.join(out, "fid-cov.exe")
    env = dict(os.environ, INSTRUMENT=os.path.join(HERE, "nat-cov-instrument.py").replace("\\", "/"),
               FID=os.path.abspath(base_fid).replace("\\", "/"), FID_C_LINE_MARKS="1")
    print(f"building the counter compiler from the tree with {base_fid} ...", flush=True)
    r = subprocess.run([BASH, os.path.join(HERE, "verify", "build-variant.sh").replace("\\", "/"), ".",
                        exe.replace("\\", "/")], env=env, cwd=ROOT)
    if r.returncode != 0 or not os.path.exists(exe):
        sys.exit("the counter compiler did not build (see build-variant.sh's output above)")
    mp = os.path.join(out, "fid-cov.c.inst.map")
    if not os.path.exists(mp):
        sys.exit(f"no counter map {mp}")
    return exe, mp


# --- Variants: the code a plain build never reaches -------------------------
# A default `emit` never runs the machine layer's checkers (FID_VERIFY), its
# worker threads (FID_NAT_PAR), the fast allocator, the opt-in passes
# (FID_NAT_ON), the function cache (FIDES_NO_CACHE is set for the plain
# runs), the object writer (--obj), the other CPU levels' encodings (--cpu:
# BMI on x86-64-v3, LSE on armv8.1-a, Zba/Zbb and V on RISC-V), the Mach-O
# writer (macOS targets), the dumps, nor the fallbacks taken when a pass is
# off (FID_NAT_OFF, the pass matrix's switches). Each variant compiles a
# sample of the tests with its switch; `test-rules` (with and without
# --smt) runs once. All of it is the suite's own programs and the compiler's
# own commands: the coverage stays "from the suite and the fuzzers".
#
# (name, modes or None for the run's modes, env, extra args, sample, repeat)
# sample "v": --variant-tests tests, "o": --off-tests tests. In env, {cache}
# is a cache file of the job's own and {dir} a directory of its own.
OPT_IN = ["bparams", "fregs", "greedy", "greedycmp", "pie", "rvc", "splitall", "vzdump", "crashfree"]
VARIANTS = [
    ("verify", None, {"FID_VERIFY": "1"}, [], "v", 1),
    ("par", None, {"FID_NAT_PAR": "4"}, [], "v", 1),
    ("fast", None, {"FID_NAT_FAST": "1"}, [], "v", 1),
    ("debug", None, {}, ["--debug"], "v", 1),
    ("g", None, {}, ["-g"], "v", 1),
    ("frames", None, {"FID_NAT_FRAMES": "1"}, [], "v", 1),
    ("stats", None, {}, ["--stats", "--remarks"], "o", 1),
    ("dump", None, {"FID_NAT_DUMP": "*"}, [], "o", 1),
    ("obj", None, {}, ["--obj"], "o", 1),
    ("cache", None, {"FID_FNCACHE": "{cache}"}, [], "o", 2),
    ("readcse", None, {"FID_RULES_ON": "readcse"}, [], "v", 1),
    ("iropt-all", None, {"FID_IROPT_ON": "all"}, [], "v", 1),
    ("egstats", None, {"FID_EGRAPH_STATS": "1", "FID_RULE_STATS": "1"}, [], "o", 1),
    ("x86-v3", ["emit:x86_64-linux", "nat"], {}, ["--cpu", "x86-64-v3"], "v", 1),
    ("x86-v4", ["emit:x86_64-linux"], {}, ["--cpu", "x86-64-v4"], "o", 1),
    ("arm-v8.1", ["emit:aarch64-linux"], {}, ["--cpu", "armv8.1-a"], "v", 1),
    ("arm-v9", ["emit:aarch64-linux"], {}, ["--cpu", "armv9-a"], "o", 1),
    ("rv-zbb", ["emit:riscv64-linux"], {}, ["--cpu", "rv64gc_zba_zbb"], "v", 1),
    ("rv-v", ["emit:riscv64-linux"], {}, ["--cpu", "rv64gcv"], "v", 1),
    ("rv-g", ["emit:riscv64-linux"], {}, ["--cpu", "rv64g"], "o", 1),
    ("rv-rva23", ["emit:riscv64-linux"], {}, ["--cpu", "rva23u64"], "o", 1),
    ("macos", ["emit:x86_64-macos", "emit:aarch64-macos"], {}, [], "v", 1),
    ("universal", ["emit:universal"], {}, [], "o", 1),
    ("tools", ["cmd:dump-mir+--target+aarch64-linux", "cmd:dump-mir+--target+riscv64-linux",
               "cmd:disasm+--target+x86_64-linux"], {}, [], "o", 1),
] + [("on-" + p, None, {"FID_NAT_ON": p}, [], "v", 1) for p in OPT_IN]
# commands run once, not per test
ONCE = ["test-rules", "test-rules+--smt+{dir}"]


def off_passes(path=os.path.join(ROOT, "docs", "verification", "kill-switches.md")):
    """The FID_NAT_OFF passes of kill-switches.md's backend table (the list
    the pass matrix uses), in order."""
    out = []
    on = False
    try:
        with open(path, encoding="utf-8") as f:
            for line in f:
                if line.startswith("## "):
                    on = line.startswith("## Backend passes")
                    continue
                if on and line.startswith("| `FID_NAT_OFF="):
                    name = line.split("=", 1)[1].split("`", 1)[0]
                    if name and name not in out:
                        out.append(name)
    except OSError:
        pass
    return out


# the machine layer's own tests: always in a sample (up to its size)
FOCUS = ("test_verify_nat_", "test_natabi_", "test_native_", "test_simd_native", "test_verify_rl12_",
         "test_nat_")


def sample(tests, n):
    """n tests: the machine layer's own (FOCUS) first, the rest spread
    evenly over the sorted list (all of them when fewer); expected-fail
    programs are left out (they stop before the backend)."""
    pool = [t for t in tests if not t.endswith("_fail.fd")]
    if n <= 0 or not pool:
        return []
    if n >= len(pool):
        return pool
    focus = [t for t in pool if os.path.basename(t).startswith(FOCUS)][:n]
    rest = [t for t in pool if t not in focus]
    k = n - len(focus)
    return focus + ([rest[(i * len(rest)) // k] for i in range(k)] if k > 0 and rest else [])


def variant_jobs(tests, base_modes, names, nv, no):
    """(mode, test, env, extra, repeat, tag) of every variant run."""
    jobs = []
    table = VARIANTS + [("off-" + p, None, {"FID_NAT_OFF": p}, [], "o", 1) for p in off_passes()]
    want = None if names in ("all", "") else set(names.split(","))
    vt, ot = sample(tests, nv), sample(tests, no)
    for name, modes, env, extra, which, rep in table:
        if want is not None and name not in want and not (name.startswith("off-") and "off" in want):
            continue
        for t in (vt if which == "v" else ot):
            for m in (modes or base_modes):
                jobs.append((m, t, env, extra, rep, name))
    if want is None or "once" in want:
        for c in ONCE:
            jobs.append(("once:" + c, "", {}, [], 1, "once"))
    return jobs


def job_cmd(fid, mode, test, exe):
    """The command line of one job (scripts/verify/coverage.py run_one's
    modes, plus once:<command>[+<arg>...])."""
    kind, _, target = mode.partition(":")
    if kind == "once":
        sub, *extra = target.split("+")
        return [fid, sub] + [x.replace("{dir}", exe + ".d") for x in extra]
    if mode in ("run", "check", "dump-c"):
        return [fid, mode, test]
    if kind == "cmd":
        sub, *extra = target.split("+")
        return [fid, sub, test] + [x.replace("{out}", exe) for x in extra]
    if kind == "nat":
        return [fid, "emit", test, "--nat", "-o", exe]
    if target:
        return [fid, kind, test, "--target", target, "-o", exe]
    return [fid, mode, test, "-o", exe]


def run_job(fid, job, covdir, work):
    """Run one job with its switches; the counter compiler writes its bitmap
    into covdir. Returns (test, mode, tag, exit status)."""
    mode, test, env_add, extra, repeat, tag = job
    if not env_add and not extra and repeat == 1 and tag == "base":
        t, m, rc = cv.run_one(fid, mode, test, covdir, work)
        return t, m, tag, rc
    env = dict(os.environ, FID_COV_DIR=covdir, FIDES_NO_CACHE="1")
    env.setdefault("CC", "zig cc")
    safe = "".join(c if c.isalnum() or c in "_.-" else "-" for c in f"{os.path.basename(test) or 'once'}.{mode}.{tag}")
    exe = os.path.join(work, safe + ".exe")
    for k, v in env_add.items():
        env[k] = v.replace("{cache}", exe + ".fncache").replace("{dir}", exe + ".d")
    if any(k == "FID_FNCACHE" for k in env_add):
        env.pop("FIDES_NO_CACHE", None)  # the cache is what this run is for
    cmd = job_cmd(fid, mode, test, exe) + list(extra)
    if mode.startswith("once:") and "{dir}" in mode:
        os.makedirs(exe + ".d", exist_ok=True)
    rc = 0
    for _ in range(repeat):  # the cache: the first run fills it, the next reads it
        try:
            p = subprocess.run(cmd, capture_output=True, timeout=900, env=env)
            rc = p.returncode
        except subprocess.TimeoutExpired:
            rc = "timeout"
    for e in (exe, exe.replace(".exe", ".pdb"), exe + ".fncache", exe + ".o", exe + ".fides-stamp"):
        if os.path.exists(e):
            try:
                os.remove(e)
            except OSError:
                pass
    return test, mode, tag, rc


def gaps(acc, entries, owner, lines_of, modules):
    """{module: [(lines not run, blocks not run, blocks, function, first line
    not run)]}, largest first: where a new test pays most. A function with
    no C at all counts all its lines and has no blocks."""
    hit = lambda i: (acc[i >> 3] >> (i & 7)) & 1 if (i >> 3) < len(acc) else 0
    per = {}
    for i, fn, kind in entries:
        mod = cv.module_of(fn, owner)
        if mod not in modules:
            continue
        src = src_of(fn, lines_of) or fn
        r = per.setdefault((mod, src), dict(lines=set(), lines_hit=set(), b=0, bh=0))
        if kind.startswith("L"):
            r["lines"].add(int(kind[1:]))
            if hit(i):
                r["lines_hit"].add(int(kind[1:]))
        else:
            r["b"] += 1
            r["bh"] += hit(i)
    out = {m: [] for m in modules}
    for (mod, fn), r in per.items():
        miss = sorted(r["lines"] - r["lines_hit"])
        if miss or r["bh"] < r["b"]:
            out[mod].append((len(miss), r["b"] - r["bh"], r["b"], fn, miss[0] if miss else 0))
    present = {s for s in (src_of(fn, lines_of) for _, fn, _ in entries) if s}
    for fn, (mod, n) in lines_of.items():
        if mod in out and fn not in present:
            out[mod].append((n, 0, 0, fn, 0))
    for m in out:
        out[m].sort(key=lambda x: (-x[0], -x[1], x[3]))
    return out


def self_test_variants():
    offs = off_passes()
    assert "mir" in offs and "egraph" in offs and len(offs) >= 20, offs
    tests = [f"t{i:02}.fd" for i in range(20)] + ["x_fail.fd"]
    s = sample(tests, 5)
    assert s == ["t00.fd", "t04.fd", "t08.fd", "t12.fd", "t16.fd"], s
    assert sample(tests, 50) == tests[:20] and sample(tests, 0) == []
    ft = tests + ["d/test_verify_nat_a.fd", "d/test_natabi_b.fd"]
    assert sample(ft, 4) == ["d/test_verify_nat_a.fd", "d/test_natabi_b.fd", "t00.fd", "t10.fd"], sample(ft, 4)
    assert sample(ft, 1) == ["d/test_verify_nat_a.fd"], sample(ft, 1)
    jobs = variant_jobs(tests, ["nat", "emit:x86_64-linux"], "verify,once", 5, 2)
    assert len(jobs) == 5 * 2 + len(ONCE), len(jobs)
    assert jobs[0] == ("nat", "t00.fd", {"FID_VERIFY": "1"}, [], 1, "verify"), jobs[0]
    offj = variant_jobs(tests, ["nat"], "off", 5, 2)
    assert len(offj) == 2 * len(offs) and offj[0][2] == {"FID_NAT_OFF": offs[0]}, offj[:2]
    assert job_cmd("fid", "once:test-rules+--smt+{dir}", "", "w/x.exe") == ["fid", "test-rules", "--smt", "w/x.exe.d"]
    assert job_cmd("fid", "emit:aarch64-linux", "a.fd", "o") == ["fid", "emit", "a.fd", "--target", "aarch64-linux", "-o", "o"]
    assert job_cmd("fid", "cmd:dump-mir+--target+riscv64-linux", "a.fd", "o") == ["fid", "dump-mir", "a.fd", "--target", "riscv64-linux"]
    allj = variant_jobs(tests, ["nat"], "all", 3, 1)
    names = {j[5] for j in allj}
    assert {"verify", "cache", "x86-v3", "macos", "on-greedy", "once", "off-mir"} <= names, names
    assert all(j[4] == 2 for j in allj if j[5] == "cache")
    print(f"variants: {len(VARIANTS)} switches, {len(offs)} passes off, {len(ONCE)} commands once")


def self_test():
    d = tempfile.mkdtemp(prefix="natcov")
    p = os.path.join(d, "m.fd")
    with open(p, "w", newline="\n") as f:
        f.write("fn a(x: i64) -> i64 {\n    var y: i64 = x;\n    if y > 0 {\n        return y;\n    }\n    return 0;\n}\n\n"
                "fn never(x: i64) -> i64 {\n    var z: i64 = x + 1;\n    return z;\n}\n")
    mod = os.path.relpath(p).replace("\\", "/")
    owner = cv.fd_functions([p])
    lines_of = cv.fd_line_counts([p])
    # `a` has C: entry, two blocks, lines 2, 3, 4, 6; `never` has no C
    entries = [(0, "a", "entry"), (1, "a", "bb0"), (2, "a", "L2"), (3, "a", "L3"),
               (4, "a", "bb1"), (5, "a", "L4"), (6, "a", "bb2"), (7, "a", "L6")]
    acc = bytes([0b11001111])  # all but 4 and 5 ran: bb1 and its line 4 did not
    f = figures(acc, entries, owner, lines_of, [mod])[mod]
    assert f["real_lines"], f
    assert (f["blocks_hit"], f["blocks"]) == (3, 4), f
    assert (f["fns_hit"], f["fns"]) == (1, 1), f
    absent = lines_of["never"][1]
    assert absent > 0 and f["lines_absent"] == absent, (f, lines_of)
    assert (f["lines_hit"], f["lines_all"]) == (3, 4 + absent), f
    assert f["uncovered"] == [4], f
    # without marks: the estimate, labelled
    g = figures(acc, [e for e in entries if not e[2].startswith("L")], owner, lines_of, [mod])[mod]
    assert not g["real_lines"] and g["lines_all"] == lines_of["a"][1] + lines_of["never"][1], g
    # the gap list: `never` (no C) first with all its lines, then `a` (line 4, bb1)
    gp = gaps(acc, entries, owner, lines_of, [mod])[mod]
    assert [x[3] for x in gp] == ["never", "a"], gp
    assert gp[0] == (absent, 0, 0, "never", 0), gp
    assert gp[1] == (1, 1, 4, "a", 4), gp
    self_test_variants()
    print(render({mod: f}, 1, "nat"), end="")
    print("nat-coverage self-test: ok")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--fid", help="a counter compiler (default: build one)")
    ap.add_argument("--map", help="its counter map (<fid>.c.inst.map)")
    ap.add_argument("--base-fid", default=os.path.join("release", "fid.exe" if os.name == "nt" else "fid"))
    ap.add_argument("--out", default=os.path.join("build", "nat-coverage"))
    ap.add_argument("--modes", default=DEFAULT_MODES)
    ap.add_argument("--tests", default="tests/bootstrap/*.fd")
    ap.add_argument("--gen", type=int, default=0, help="N programs of each fuzzer (scripts/fuzz) too")
    ap.add_argument("--limit", type=int, default=0)
    ap.add_argument("-j", type=int, default=4)
    ap.add_argument("--modules", default=DEFAULT_MODULES)
    ap.add_argument("--sources", default="", help="more .fd files whose functions the map may name (comma list)")
    ap.add_argument("--goal", type=float, default=0)
    ap.add_argument("--record", default="")
    ap.add_argument("--report-only", action="store_true")
    ap.add_argument("--self-test", action="store_true")
    ap.add_argument("--variants", default="all",
                    help="switch runs (VARIANTS: names, `off` for every FID_NAT_OFF pass, `once` for "
                         "test-rules), `all` (default) or `none`")
    ap.add_argument("--variant-tests", type=int, default=40, help="tests per variant of sample v")
    ap.add_argument("--off-tests", type=int, default=6, help="tests per variant of sample o (passes off, ...)")
    a = ap.parse_args()
    if a.self_test:
        self_test()
        return 0
    os.chdir(ROOT)
    os.makedirs(a.out, exist_ok=True)
    fid, mp = a.fid, a.map
    if not a.report_only and not fid:
        fid, mp = build_counter_compiler(a.base_fid, a.out)
    if not mp:
        mp = os.path.join(a.out, "fid-cov.c.inst.map")
        if fid and os.path.exists(os.path.splitext(fid)[0] + ".c.inst.map"):
            mp = os.path.splitext(fid)[0] + ".c.inst.map"
    entries = load_map(mp)
    if not entries:
        sys.exit(f"empty or missing counter map {mp}")
    nbits = max(i for i, _, _ in entries) + 1
    merged_path = os.path.join(a.out, "merged.cov")
    acc = None
    ntests = 0
    if not a.report_only:
        tests = sorted(glob.glob(a.tests))
        if a.limit:
            tests = tests[: a.limit]
        if a.gen:
            tests += cv.gen_tests(a.gen, a.out)
        ntests = len(tests)
        covdir = tempfile.mkdtemp(prefix="cov", dir=a.out)
        work = tempfile.mkdtemp(prefix="work", dir=a.out)
        base_modes = a.modes.split(",")
        jobs = [(m, t, {}, [], 1, "base") for t in tests for m in base_modes]
        nbase = len(jobs)
        if a.variants != "none":
            jobs += variant_jobs(tests, base_modes, a.variants, a.variant_tests, a.off_tests)
        print(f"{nbase} plain jobs, {len(jobs) - nbase} variant jobs", flush=True)
        done = 0
        acc = bytes((nbits + 7) // 8)
        with ThreadPoolExecutor(a.j) as ex:
            for _t, _m, _tag, _rc in ex.map(lambda jb: run_job(fid, jb, covdir, work), jobs):
                done += 1
                if done % 100 == 0:
                    print(f"  {done}/{len(jobs)}", flush=True)
                    acc = bytes(cv.merge(covdir, nbits, acc))
                    for p in glob.glob(os.path.join(covdir, "*.cov")):
                        os.remove(p)
        acc = bytes(cv.merge(covdir, nbits, acc))
        for p in glob.glob(os.path.join(covdir, "*.cov")):
            os.remove(p)
        with open(merged_path, "wb") as f:
            f.write(acc)
    else:
        with open(merged_path, "rb") as f:
            acc = f.read()
    srcs = [s for s in a.sources.split(",") if s] + sorted(glob.glob("compiler/*.fd")) + ["stdlib/core.fd"]
    owner = cv.fd_functions(srcs)
    lines_of = cv.fd_line_counts(srcs)
    modules = [m for m in a.modules.split(",") if m]
    figs = figures(acc, entries, owner, lines_of, modules)
    modes_label = a.modes + ("" if a.variants == "none" or a.report_only else f" + variants ({a.variants})")
    text = render(figs, ntests, modes_label)
    print(text, end="")
    with open(os.path.join(a.out, "report.txt"), "w") as f:
        f.write(text)
    for m, d in figs.items():
        with open(os.path.join(a.out, "uncovered-" + os.path.basename(m) + ".txt"), "w") as f:
            f.write("".join(f"{m}:{ln}\n" for ln in d["uncovered"]))
    # where a new test or fuzzer program pays most (RL12's last step)
    for m, rows in gaps(acc, entries, owner, lines_of, modules).items():
        with open(os.path.join(a.out, "gaps-" + os.path.basename(m) + ".txt"), "w") as f:
            f.write("# lines-not-run blocks-not-run/blocks function first-line-not-run\n")
            f.write("".join(f"{n} {bm}/{b} {fn} {m}:{ln}\n" for n, bm, b, fn, ln in rows))
    if a.record:
        head = "| date | tests | modes | module | lines | blocks | functions |"
        have = ""
        if os.path.exists(a.record):
            with open(a.record, encoding="utf-8") as f:
                have = f.read()
        with open(a.record, "a", encoding="utf-8", newline="") as f:
            if head not in have:
                f.write(("\n" if have and not have.endswith("\n\n") else "") + head + "\n|---|---|---|---|---|---|---|\n")
            for m, d in figs.items():
                lab = "" if d["real_lines"] else " (est.)"
                f.write(f"| {datetime.date.today()} | {ntests} | {modes_label} | {m} | {pct(d['lines_hit'], d['lines_all']):.1f}%{lab}"
                        f" ({d['lines_hit']}/{d['lines_all']}) | {pct(d['blocks_hit'], d['blocks']):.1f}% ({d['blocks_hit']}/{d['blocks']})"
                        f" | {pct(d['fns_hit'], d['fns']):.1f}% |\n")
    if a.goal:
        low = [(m, d) for m, d in figs.items()
               if pct(d["lines_hit"], d["lines_all"]) < a.goal or pct(d["blocks_hit"], d["blocks"]) < a.goal]
        for m, d in low:
            print(f"below the {a.goal:g}% goal: {m} (lines {pct(d['lines_hit'], d['lines_all']):.1f}%, blocks {pct(d['blocks_hit'], d['blocks']):.1f}%)")
        return 1 if low else 0
    return 0


if __name__ == "__main__":
    sys.exit(main())
