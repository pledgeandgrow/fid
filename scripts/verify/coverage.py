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
coverage is branch coverage at the IR level) and function entries; and
source lines when the C carries them (VF8, Nov 18): build the counter
compiler with FID_C_LINE_MARKS=1 (the C generator marks each instruction's
source line, `/*@L<n>*/`) or from `dump-c -g` C (`#line`); cov_instrument.py
gives every marked line a counter (`L<n>` in the map). The line table is
then real: per module, the statement lines that ran / the lines with a
counter, plus the source lines of the module's functions that have no C at
all (never lowered: counted as not run; those are counted as non-blank,
non-comment lines, a slight over-count against statement lines).
<out>/uncovered-lines.txt lists `module:line` for every line no test
reached. Without line counters the second table is the Nov 16 estimate: the
lines of functions entered (an upper bound) and a block-weighted figure.
`--goal 90 --goal-modules all` checks every module's block coverage,
`--goal-lines 90` its line coverage (exit 2 when the map has no line
counters); `--self-test` checks the mapping on a made-up module.

--report-only re-reads the merged bitmap (<out>/merged.cov) without
running anything. --record <file.md> appends the report, dated, to a
results file (VF8: docs/verification/vf8-coverage.md). --sources a.fd,b.fd
adds source files whose functions the map may name (a test program, for
tests/tooling/vf8-coverage-check.sh). Mode `dump-c` runs the C generator
without the C compiler; `cmd:<command>[+<arg>...]` runs any other command
on each test ({out}: a scratch output file), for the tool modules:
`cmd:fmt+--check`, `cmd:lint`, `cmd:explain`, `cmd:wasm+-o+{out}`. A run
without --fresh adds to <out>/merged.cov, so a second run with other
modes and tests adds its blocks to the first's.

Own-backend roadmap RL12 (the machine layer and every target over 90%, from
the suite and the fuzzers):

    --modes ...,emit:aarch64-linux,emit:riscv64-linux,emit:x86_64-linux,nat
        the machine layer for each Linux target (`emit --target`) and
        `emit --nat`; only the compiler runs, so no target binary is executed
    --gen N   N random programs of scripts/fuzz/gen_vs_c.py and N of
              gen_programs.py are added to the tests (the fuzzers' inputs)
    --goal 90 --goal-modules compiler/nat_ir.fd,compiler/nat_enc.fd,...
              exit 1 when a listed module's block coverage is below the goal
              (default modules: the machine layer and its rules)
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


ITEM_RE = re.compile(r"^\s*(?:pub\s+)?(?:async\s+|comptime\s+)?(?:fn|impl|struct|enum|const|trait|type|extern|import|use)\b")


def fd_line_counts(paths):
    """Source lines of each function: {C name: (module, lines)} (VF8).

    A function's lines run from its `fn` line to the line before the next
    item (fn, impl, struct, ...) or doc comment; blank lines, comment-only
    lines and lines holding only braces are not counted. The keys are the
    C names fd_functions gives (methods as Type_name)."""
    out = {}
    fn_re = re.compile(r"^\s*(?:pub\s+)?(?:async\s+|comptime\s+)?fn\s+([A-Za-z_][A-Za-z0-9_]*)")
    impl_re = re.compile(r"^\s*impl\s*(?:<[^>]*>\s*)?(?:([A-Za-z_][A-Za-z0-9_]*)\s+for\s+)?([A-Za-z_][A-Za-z0-9_]*)")
    for p in paths:
        mod = os.path.relpath(p).replace("\\", "/")
        impl = None
        cur = None
        n = 0

        def close():
            if cur is not None:
                for k in cur:
                    out.setdefault(k, (mod, n))
        with open(p, encoding="utf-8", errors="replace") as f:
            for line in f:
                s = line.strip()
                m = impl_re.match(line)
                if m and not line[0].isspace():
                    close()
                    cur, n = None, 0
                    impl = m.group(2)
                    continue
                m = fn_re.match(line)
                if m:
                    close()
                    name = m.group(1)
                    cur = []
                    if not line[0].isspace():
                        cur.append(name)
                    if impl and (line[0].isspace() or "(self" in line):
                        cur.append(f"{impl}_{name}")
                    n = 1
                    continue
                if (not line[0].isspace() and (ITEM_RE.match(line) or line.startswith(("@", "var ", "val ", "pub var ", "pub val ")))) \
                        or s.startswith("///"):
                    close()
                    cur, n = None, 0
                    continue
                if cur is not None and s and not s.startswith("//") and s.strip("{}; ") != "":
                    n += 1
        close()
    return out


def line_report(acc, entries, lines_of):
    """Per module: (source lines, lines in functions entered, block-weighted
    lines). The generated C has no line table, so a line counts as reached
    when its function was entered (an upper bound of line coverage), and the
    weighted figure gives each function its lines times the share of its
    blocks that ran (an estimate). Functions with no C (never lowered)
    count as not reached."""
    hit = lambda i: (acc[i >> 3] >> (i & 7)) & 1

    def src_of(fn):
        return src_fn(fn, lines_of)
    best = {}  # source function -> (entered, best block ratio over its copies)
    per = {}
    for i, fn, blk in entries:
        if blk.startswith("L"):
            continue  # a source-line counter, not a block
        k = src_of(fn)
        if k is None:
            continue
        t = per.setdefault(fn, [k, 0, 0, 0])
        t[1] += 1
        t[2] += hit(i)
        if blk == "entry":
            t[3] = hit(i)
    for fn, (k, b, h, entered) in per.items():
        e, r = best.get(k, (0, 0.0))
        best[k] = (max(e, entered), max(r, h / max(1, b)))
    mods = {}
    for k, (mod, n) in lines_of.items():
        m = mods.setdefault(mod, [0, 0, 0.0])
        e, r = best.get(k, (0, 0.0))
        m[0] += n
        m[1] += n * e
        m[2] += n * r
    return mods


USE_RE = re.compile(r"\buse\s+([A-Za-z_][A-Za-z0-9_]*(?:::[A-Za-z_][A-Za-z0-9_]*)*)\s*;")


def strip_comments_strings(text):
    """The text without string literals and // comments (a `use x;` inside
    either is not an import)."""
    text = re.sub(r'"(?:\\.|[^"\\\n])*"', '""', text)
    return re.sub(r"//[^\n]*", "", text)


def reached_sources(root="compiler/main.fd"):
    """The .fd files `root` reaches through `use` (VF8): `use compiler::x;`
    is compiler/x.fd, `use x;` stdlib/x.fd. Only their functions can be in
    the compiler's C, so only they own C names: a module nothing reaches
    (compiler/embedded.fd) defining a function core also defines
    (int_to_str) would otherwise take core's counters. Order: compiler
    modules, the other stdlib modules, core (a program's own definition
    shadows the library's)."""
    seen = set()
    todo = [root]
    while todo:
        p = todo.pop()
        if p in seen or not os.path.exists(p):
            continue
        seen.add(p)
        with open(p, encoding="utf-8", errors="replace") as f:
            text = strip_comments_strings(f.read())
        for m in USE_RE.finditer(text):
            parts = m.group(1).split("::")
            if parts[0] == "compiler" and len(parts) == 2:
                todo.append(f"compiler/{parts[1]}.fd")
            elif len(parts) == 1:
                todo.append(f"stdlib/{parts[0]}.fd")
    comp = sorted(s for s in seen if s.startswith("compiler/"))
    std = sorted(s for s in seen if s.startswith("stdlib/") and s != "stdlib/core.fd")
    return comp + std + ["stdlib/core.fd"]


def src_fn(fn, lines_of):
    """The source function a C function comes from (monomorphised copies
    carry suffixes: Name_T, Name_T_U), or None."""
    if fn in lines_of:
        return fn
    base = fn
    while "_" in base:
        base = base.rsplit("_", 1)[0]
        if base in lines_of:
            return base
    return None


def has_line_counters(entries):
    return any(k.startswith("L") for _, _, k in entries)


def real_lines(acc, entries, owner, lines_of):
    """Line coverage from the map's source-line counters (`L<n>`, VF8).

    {module: [lines reached, lines with a counter, source lines of the
    module's functions that have no C, sorted uncovered line numbers]} for
    every module with at least one function in the C; {} when the map has no
    line counters. A line with several counters (one per block it spans, or
    per monomorphised copy) is reached when any of them ran."""
    if not has_line_counters(entries):
        return {}
    hit = lambda i: (acc[i >> 3] >> (i & 7)) & 1 if (i >> 3) < len(acc) else 0
    seen = {}  # module -> {line: reached}
    present = set()
    in_c = set()  # modules with a function in the C
    for i, fn, kind in entries:
        s = src_fn(fn, lines_of)
        if s is not None:
            present.add(s)
        mod = module_of(fn, owner)
        if mod is None:
            continue
        in_c.add(mod)
        if not kind.startswith("L"):
            continue
        d = seen.setdefault(mod, {})
        ln = int(kind[1:])
        d[ln] = d.get(ln, 0) | hit(i)
    absent = {}
    for fn, (mod, n) in lines_of.items():
        if fn not in present:
            absent[mod] = absent.get(mod, 0) + n
    out = {}
    for mod in sorted(in_c):
        d = seen.get(mod, {})
        out[mod] = [sum(d.values()), len(d), absent.get(mod, 0), sorted(ln for ln, r in d.items() if not r)]
    return out


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
    exe = os.path.join(work, os.path.basename(test) + "." + re.sub(r"[^A-Za-z0-9_.-]+", "-", mode) + ".exe")
    kind, _, target = mode.partition(":")
    if mode in ("run", "check", "dump-c"):
        cmd = [fid, mode, test]
    elif kind == "cmd":
        # VF8: any other command on the test, `cmd:<command>[+<arg>...]`,
        # {out} standing for a scratch output file: `cmd:fmt+--check`,
        # `cmd:wasm+-o+{out}` (the tool modules: fmt, lint, explain, ...)
        sub, *extra = target.split("+")
        cmd = [fid, sub, test] + [x.replace("{out}", exe) for x in extra]
    elif kind == "nat":
        cmd = [fid, "emit", test, "--nat", "-o", exe]
    elif target:
        cmd = [fid, kind, test, "--target", target, "-o", exe]
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


GOAL_MODULES = "compiler/nat_ir.fd,compiler/nat_enc.fd,compiler/rules.fd,compiler/egraph.fd"


def gen_tests(n, out):
    """The fuzzers' programs as extra inputs (deterministic seeds)."""
    import random
    here = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    sys.path.insert(0, os.path.join(here, "fuzz"))
    import gen_vs_c
    import gen_programs
    d = os.path.join(out, "gen")
    os.makedirs(d, exist_ok=True)
    files = []
    for k in range(n):
        for name, text in ((f"gvc_{k}.fd", gen_vs_c.pair(5000 + k)[0]),
                           (f"gp_{k}.fd", gen_programs.Gen(random.Random(6000 + k)).program())):
            path = os.path.join(d, name)
            with open(path, "w", newline="\n") as f:
                f.write(text)
            files.append(path)
    return files


def below_goal(acc, entries, owner, goal, modules):
    """Modules among `modules` whose block coverage is below `goal` percent."""
    hit = lambda i: (acc[i >> 3] >> (i & 7)) & 1
    tot = {}
    for i, fn, kind in entries:
        if kind.startswith("L"):
            continue  # source-line counters: below_goal_lines
        mod = module_of(fn, owner)
        if mod in modules:
            t = tot.setdefault(mod, [0, 0])
            t[0] += 1
            t[1] += hit(i)
    out = []
    for mod in modules:
        b, h = tot.get(mod, [0, 0])
        pct = 100.0 * h / max(1, b)
        if pct < goal:
            out.append((mod, pct, h, b))
    return out


def below_goal_lines(acc, entries, owner, lines_of, goal, modules):
    """Modules among `modules` whose line coverage (real_lines: reached /
    lines with a counter + lines with no C) is below `goal` percent, as
    (module, pct, reached, lines); None when the map has no line counters."""
    rl = real_lines(acc, entries, owner, lines_of)
    if not rl:
        return None
    out = []
    for mod in modules:
        h, n, ab, _ = rl.get(mod, [0, 0, 0, []])
        pct = 100.0 * h / max(1, n + ab)
        if pct < goal:
            out.append((mod, pct, h, n + ab))
    return out


def report(acc, entries, owner, out, lines_of=None):
    hit = lambda i: (acc[i >> 3] >> (i & 7)) & 1
    mods = {}
    for i, fn, blk in entries:
        if blk.startswith("L"):
            continue  # source-line counters: the line table
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
    rl = real_lines(acc, entries, owner, lines_of) if lines_of else {}
    if rl:
        lines.append("")
        lines.append("Lines (VF8: the statement lines the C marks, each with a counter; 'no C' =")
        lines.append("source lines of functions never lowered, counted as not run)")
        lines.append(f"{'module':34} {'reached':>15} {'with counter':>12} {'no C':>7}")
        ta = tn = tab = 0
        for mod in sorted(rl):
            h, n, ab, _ = rl[mod]
            ta += h; tn += n; tab += ab
            lines.append(f"{mod:34} {h:>7}/{n + ab:<7} {100 * h / max(1, n + ab):5.1f}% {n:>6} {ab:>7}")
        lines.append(f"{'TOTAL':34} {ta:>7}/{tn + tab:<7} {100 * ta / max(1, tn + tab):5.1f}% {tn:>6} {tab:>7}")
        with open(os.path.join(out, "uncovered-lines.txt"), "w") as f:
            for mod in sorted(rl):
                for ln in rl[mod][3]:
                    f.write(f"{mod}:{ln}\n")
    elif lines_of:
        lm = line_report(acc, entries, lines_of)
        lines.append("")
        lines.append("Lines (the C has no line table: 'reached' = the line's function was entered,")
        lines.append("an upper bound; 'weighted' = its lines x the share of its blocks that ran)")
        lines.append(f"{'module':34} {'lines':>7} {'reached':>15} {'weighted':>15}")
        ta = tr = tw = 0
        for mod in sorted(lm):
            n, r, w = lm[mod]
            ta += n; tr += r; tw += w
            lines.append(f"{mod:34} {n:>7} {r:>7} {100 * r / max(1, n):5.1f}% {int(w):>7} {100 * w / max(1, n):5.1f}%")
        lines.append(f"{'TOTAL':34} {ta:>7} {tr:>7} {100 * tr / max(1, ta):5.1f}% {int(tw):>7} {100 * tw / max(1, ta):5.1f}%")
    text = "\n".join(lines) + "\n"
    with open(os.path.join(out, "report.txt"), "w") as f:
        f.write(text)
    with open(os.path.join(out, "never-called.txt"), "w") as f:
        for mod in sorted(mods):
            if not mod.startswith("("):
                for fn in mods[mod]["missed"]:
                    f.write(f"{mod} {fn}\n")
    return text


def self_test():
    """The mapping and the line figures on a made-up module (no compiler)."""
    d = tempfile.mkdtemp(prefix="covtest")
    p = os.path.join(d, "m.fd")
    with open(p, "w", newline="\n") as f:
        f.write("/// doc\nfn a(x: i64) -> i64 {\n    var y: i64 = x;\n    // note\n\n    return y;\n}\n\n"
                "struct P {\n    v: i64,\n}\n\nimpl P {\n    fn get(self: & P) -> i64 {\n        return self.v;\n    }\n}\n"
                "fn b() {\n    println(1);\n    println(2);\n}\n")
    lo = fd_line_counts([p])
    mod = os.path.relpath(p).replace("\\", "/")
    want = {"a": (mod, 3), "P_get": (mod, 2), "b": (mod, 3)}
    got = {k: v for k, v in lo.items() if k in want}
    assert got == want, f"line counts: {got} != {want}"
    # a: entered, 2 of 4 blocks; P_get: never entered; b: entered, every block
    entries = [(0, "a", "entry"), (1, "a", "bb1"), (2, "a", "bb2"), (3, "a", "bb3"),
               (4, "P_get", "entry"), (5, "b", "entry"), (6, "b", "bb1")]
    acc = bytes([0b01100011])
    n, r, w = line_report(acc, entries, lo)[mod]
    assert (n, r) == (8, 6), (n, r)
    assert abs(w - (3 * 0.5 + 3)) < 1e-9, w
    assert real_lines(acc, entries, fd_functions([p]), lo) == {}
    # VF8: source-line counters (`L<n>`, cov_instrument.py). a: lines 3 and
    # 6 marked, 3 ran twice over (two counters), 6 never; b: line 19 ran;
    # P_get has no C, so its 2 lines count as not run. The line counters
    # change neither the block figures nor the estimate.
    lentries = entries + [(7, "a", "L3"), (8, "a", "L3"), (9, "a", "L6"), (10, "b", "L19"), (11, "b", "L20")]
    lacc = bytes([0b11100011, 0b00001101])  # 7 ran, 8 ran, 9 not, 10 ran, 11 ran
    owner = fd_functions([p])
    rl = real_lines(lacc, lentries, owner, lo)
    assert rl[mod][:3] == [3, 4, 0] and rl[mod][3] == [6], rl
    # with P_get dropped from the C, its 2 lines are 'no C'
    rl2 = real_lines(lacc, [e for e in lentries if e[1] != "P_get"], owner, lo)
    assert rl2[mod][:3] == [3, 4, 2], rl2
    assert line_report(lacc, lentries, lo)[mod] == line_report(acc, entries, lo)[mod]
    assert below_goal(lacc, lentries, owner, 0.1, [mod]) == below_goal(acc, entries, owner, 0.1, [mod])
    low = below_goal_lines(lacc, lentries, owner, lo, 80, [mod])
    assert len(low) == 1 and low[0][2:] == (3, 4), low
    assert below_goal_lines(lacc, lentries, owner, lo, 75, [mod]) == []
    assert below_goal_lines(acc, entries, owner, lo, 75, [mod]) is None
    rep = report(lacc, lentries, owner, d, lo)
    assert "Lines (VF8" in rep, rep
    with open(os.path.join(d, "uncovered-lines.txt")) as f:
        assert f.read() == f"{mod}:6\n"
    # reached_sources: `use` lines only, not ones in comments or strings
    t = tempfile.mkdtemp(prefix="covreach")
    os.makedirs(os.path.join(t, "compiler"))
    os.makedirs(os.path.join(t, "stdlib"))
    files = {"compiler/main.fd": 'module compiler::main; use core; use compiler::a;\n// use compiler::c;\nfn f() { var s: str = "use compiler::d;"; }\n',
             "compiler/a.fd": "use vec; use compiler::main;\n", "compiler/c.fd": "", "compiler/d.fd": "",
             "stdlib/core.fd": "", "stdlib/vec.fd": ""}
    for k, v in files.items():
        with open(os.path.join(t, k), "w", newline="\n") as f:
            f.write(v)
    here = os.getcwd()
    try:
        os.chdir(t)
        got = reached_sources()
    finally:
        os.chdir(here)
    assert got == ["compiler/a.fd", "compiler/main.fd", "stdlib/vec.fd", "stdlib/core.fd"], got
    print("coverage.py self-test: ok")


def main():
    if "--self-test" in sys.argv:
        self_test()
        return
    ap = argparse.ArgumentParser()
    ap.add_argument("--fid", help="the counter compiler (not needed with --report-only)")
    ap.add_argument("--map", required=True)
    ap.add_argument("--modes", default="check,run,ccompile,emit")
    ap.add_argument("--tests", default="tests/bootstrap/*.fd", help="glob, or a comma list of globs")
    ap.add_argument("--out", default=os.path.join("build", "cov"))
    ap.add_argument("-j", type=int, default=4)
    ap.add_argument("--limit", type=int, default=0)
    ap.add_argument("--report-only", action="store_true")
    ap.add_argument("--gen", type=int, default=0)
    ap.add_argument("--goal", type=float, default=0)
    ap.add_argument("--goal-modules", default=GOAL_MODULES,
                    help="comma list, or 'all': every module of the report (VF8: 90%% each)")
    ap.add_argument("--goal-lines", type=float, default=0,
                    help="line coverage goal of the goal modules (needs line counters in the map)")
    ap.add_argument("--sources", default="", help="more .fd files whose functions the map may name (comma list)")
    ap.add_argument("--record", default="", help="append the dated report to this .md file")
    ap.add_argument("--note", default="", help="the --record entry's heading (what was run)")
    ap.add_argument("--all-sources", action="store_true",
                    help="map C names to every compiler/*.fd, not only the modules main.fd reaches")
    ap.add_argument("--fresh", action="store_true",
                    help="start from no coverage: ignore <out>/merged.cov (a bitmap of an older counter compiler)")
    a = ap.parse_args()
    if not a.report_only and not a.fid:
        ap.error("--fid is needed unless --report-only")
    os.makedirs(a.out, exist_ok=True)
    entries = load_map(a.map)
    if not entries:
        sys.exit(f"empty or missing counter map {a.map}")
    nbits = max(i for i, _, _ in entries) + 1
    # the modules main.fd reaches own the C names (reached_sources);
    # --all-sources: every compiler/*.fd, as before Nov 18
    base = reached_sources() if os.path.exists("compiler/main.fd") and not a.all_sources \
        else sorted(glob.glob("compiler/*.fd")) + ["stdlib/core.fd"]
    srcs = [s for s in a.sources.split(",") if s] + base
    owner = fd_functions(srcs)
    lines_of = fd_line_counts(srcs)
    merged_path = os.path.join(a.out, "merged.cov")
    acc = None
    if os.path.exists(merged_path) and not a.fresh:
        with open(merged_path, "rb") as f:
            acc = f.read()
        if len(acc) != (nbits + 7) // 8:
            sys.exit(f"{merged_path} is {len(acc)} bytes, the map needs {(nbits + 7) // 8}: "
                     "a bitmap of another counter compiler (--fresh starts over)")
    if acc is None and a.report_only:
        sys.exit(f"no {merged_path} to report on")
    ntests = 0
    if not a.report_only:
        # a comma list of globs (VF8: the suite's subfolders, tests/conformance)
        tests = sorted({t for g in a.tests.split(",") if g for t in glob.glob(g)})
        if a.limit:
            tests = tests[: a.limit]
        if a.gen:
            tests += gen_tests(a.gen, a.out)
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
        ntests = len(tests)
        print(f"{len(tests)} tests x {a.modes}")
    text = report(acc, entries, owner, a.out, lines_of)
    print(text)
    if not has_line_counters(entries):
        print("(no source-line counters in the map: build the counter compiler with FID_C_LINE_MARKS=1;"
              " the line table is an estimate)")
    if a.record:
        import datetime
        have = ""
        if os.path.exists(a.record):
            with open(a.record, encoding="utf-8") as f:
                have = f.read()
        what = (f"{ntests} tests x {a.modes}" + (f" (+ {a.gen} programs of each fuzzer)" if a.gen else "")) \
            if not a.report_only else "report of an earlier run's bitmap"
        if a.note:
            what = a.note
        with open(a.record, "a", encoding="utf-8", newline="\n") as f:
            if have and not have.endswith("\n"):
                f.write("\n")
            f.write(f"\n## {datetime.datetime.now():%Y-%m-%d %H:%M}: {what}\n\n"
                    f"Counter compiler: {a.fid or '(not run)'}, map {a.map} "
                    f"({nbits} counters, {'with' if has_line_counters(entries) else 'without'} source-line counters).\n\n"
                    "```\n" + text + "```\n")
        print(f"recorded in {a.record}")
    rc = 0
    if a.goal_lines:
        if a.goal_modules == "all":
            lmods = sorted({module_of(fn, owner) for _, fn, _ in entries} - {None})
        else:
            lmods = [m for m in a.goal_modules.split(",") if m]
        lowl = below_goal_lines(acc, entries, owner, lines_of, a.goal_lines, lmods)
        if lowl is None:
            print("--goal-lines: the map has no source-line counters (FID_C_LINE_MARKS=1 when building the counter compiler)")
            rc = 2
        else:
            for mod, pct, h, nl in lowl:
                print(f"below the {a.goal_lines:g}% line goal: {mod} {pct:.1f}% ({h}/{nl} lines)")
            if lowl:
                rc = 1
            else:
                print(f"every goal module at or above {a.goal_lines:g}% of its lines")
    if a.goal:
        if a.goal_modules == "all":
            mods = sorted({module_of(fn, owner) for _, fn, _ in entries} - {None})
        else:
            mods = [m for m in a.goal_modules.split(",") if m]
        low = below_goal(acc, entries, owner, a.goal, mods)
        for mod, pct, h, b in low:
            print(f"below the {a.goal:g}% goal: {mod} {pct:.1f}% ({h}/{b} blocks)")
        if low:
            sys.exit(1)
        print(f"every goal module at or above {a.goal:g}%: {', '.join(mods)}")
    if rc:
        sys.exit(rc)


if __name__ == "__main__":
    main()
