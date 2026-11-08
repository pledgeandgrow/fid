#!/usr/bin/env python3
"""Optimizer passes switched off one at a time (VF4) and broken on purpose (VF7).

    # VF4: every pass off in turn; the output of every test must not change
    python scripts/verify/variants.py toggle --fid <base fid> --tree <snapshot> \
        [--tests 'tests/bootstrap/*.fd'] [--gen 40] [-j 4] [--out build/variants] [--only name,...]

    # VF7: each mutant is a compiler with one deliberate bug in a pass; some
    # test must notice (its output or exit status changes)
    python scripts/verify/variants.py mutate --fid <base fid> --tree <snapshot> [...]

    python scripts/verify/variants.py list

<tree> is a copy of the source tree (compiler/, stdlib/) — the base compiler
must have been built from it, so the only difference is the switch or the
mutation. Switches of compiler/iropt.fd are read at run time
(FID_IROPT_OFF=<pass>), so they need no build; the others are constants,
and each variant is built with scripts/verify/build-variant.sh (several GB
of memory and a few minutes each; built once, kept in <out>).

For each variant and each test, the test is compiled (emit, or ccompile
for a switch the C path reads too) by the base and the variant compiler.
When the two executables are byte-identical the pass did nothing there;
otherwise both run and their exit status and output are compared.
--gen N adds N programs of scripts/fuzz/gen_vs_c.py and gen_programs.py.
"""
import argparse
import glob
import hashlib
import os
import random
import subprocess
import sys
import threading
from concurrent.futures import ThreadPoolExecutor

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

# On Windows `bash` on the PATH can be WSL's (System32\bash.exe), which
# cannot run the scripts on Windows paths: prefer Git's
BASH = os.environ.get("BASH_EXE") or next(
    (p for p in (r"C:\Program Files\Git\bin\bash.exe", r"C:\Program Files\Git\usr\bin\bash.exe")
     if os.name == "nt" and os.path.exists(p)), "bash")

# (name, kind, how, backends)
#   kind env:   FID_IROPT_OFF value (compiler/iropt.fd iro_pass_on)
#   kind const: (file, from, to) edit of the tree
TOGGLES = [
    ("iropt-inline", "env", "inline", ["emit"]),
    ("iropt-fold", "env", "fold", ["emit"]),
    ("iropt-dce", "env", "dce", ["emit"]),
    ("iropt-tailcall", "env", "tailcall", ["emit"]),
    ("iropt-gvn", "env", "gvn", ["emit"]),
    ("iropt-licm", "env", "licm", ["emit"]),
    ("peir-global-ra", "const", ("compiler/pe_ir.fd", "const PEIR_GLOBAL_RA: bool = true;",
                                  "const PEIR_GLOBAL_RA: bool = false;"), ["emit"]),
    ("peir-coalesce", "const", ("compiler/pe_ir.fd", "const PEIR_COALESCE: bool = true;",
                                 "const PEIR_COALESCE: bool = false;"), ["emit"]),
    ("peir-volatile-ra", "const", ("compiler/pe_ir.fd", "const PEIR_VOLATILE_RA: bool = true;",
                                    "const PEIR_VOLATILE_RA: bool = false;"), ["emit"]),
    ("peir-xmm-ra", "const", ("compiler/pe_ir.fd", "const PEIR_XMM_RA: bool = true;",
                               "const PEIR_XMM_RA: bool = false;"), ["emit"]),
    ("peir-rotate", "const", ("compiler/pe_ir.fd", "const PEIR_ROTATE: bool = true;",
                               "const PEIR_ROTATE: bool = false;"), ["emit"]),
    ("ranges", "const", ("compiler/checks.fd", "const PEIR_RANGES: bool = true;",
                          "const PEIR_RANGES: bool = false;"), ["emit", "ccompile"]),
]

# Deliberate bugs, one per pass (VF7). Each edit must match exactly once.
MUTANTS = [
    ("fold-lt-as-le", ("compiler/iropt.fd",
                       "if binop_eq(op, BinOp::Lt) { return OptInst::Some(Inst::ConstBool(a < c)); }",
                       "if binop_eq(op, BinOp::Lt) { return OptInst::Some(Inst::ConstBool(a <= c)); }"), ["emit"]),
    ("fold-sub-swapped", ("compiler/iropt.fd",
                          "return OptInst::Some(Inst::ConstInt(a - c, t));",
                          "return OptInst::Some(Inst::ConstInt(c - a, t));"), ["emit"]),
    ("gvn-ignores-rhs", ("compiler/iropt.fd",
                         'return "B" + iro_binop_code(op) + "|" + int_to_str(a) + "|" + int_to_str(b) + "|" + ir_type_to_str(t);',
                         'return "B" + iro_binop_code(op) + "|" + int_to_str(a) + "|" + ir_type_to_str(t);'), ["emit"]),
    ("dce-all-pure", ("compiler/iropt.fd",
                      "        Inst::BitCast(_, _, _) => { return true; }\n        _ => { return false; }",
                      "        Inst::BitCast(_, _, _) => { return true; }\n        Inst::Call(_, _, _, _) => { return true; }\n        _ => { return false; }"),
     ["emit"]),
    ("inline-max-huge", ("compiler/iropt.fd", "const IRO_INLINE_MAX: i64 = 16;",
                         "const IRO_INLINE_MAX: i64 = 0;"), ["emit"]),
    ("ranges-len-bigger", ("compiler/checks.fd",
                           "peir_rng_put(r, id, 0, 281474976710656);",
                           "peir_rng_put(r, id, 1, 281474976710656);"), ["emit", "ccompile"]),
]


def sha(p):
    with open(p, "rb") as f:
        return hashlib.sha256(f.read()).hexdigest()


def run(cmd, env=None, timeout=300):
    try:
        p = subprocess.run(cmd, capture_output=True, timeout=timeout, env=env)
        return p.returncode, p.stdout.decode("utf-8", "replace").replace("\r\n", "\n"), p.stderr.decode("utf-8", "replace")
    except subprocess.TimeoutExpired:
        return "timeout", "", ""


def build(tree, name, edit, fid, out):
    exe = os.path.join(out, f"fid-{name}.exe")
    if os.path.exists(exe):
        return exe
    arg = f"{edit[0]}:{edit[1]}=>{edit[2]}" if edit else None
    fw = lambda p: p.replace("\\", "/")  # bash on Windows reads backslashes as escapes
    cmd = [BASH, fw(os.path.join(ROOT, "scripts", "verify", "build-variant.sh")), fw(tree), fw(exe)] + ([arg] if arg else [])
    env = dict(os.environ, FID=fw(fid))
    print(f"building {name} ...", flush=True)
    rc, out_, err = run(cmd, env, timeout=3600)
    if rc != 0 or not os.path.exists(exe):
        print(f"  build of {name} failed: {(out_ + err)[-600:]}")
        return None
    return exe


def compile_and_run(fid, env, backend, test, exe_path, cwd):
    if os.path.exists(exe_path):
        os.remove(exe_path)
    rc, out, err = run([fid, backend, test, "-o", exe_path], env)
    if rc != 0 or not os.path.exists(exe_path):
        # once more on its own: under load the C compiler's shared cache, or
        # memory, fails now and then
        with RETRY_LOCK:
            rc, out, err = run([fid, backend, test, "-o", exe_path], env)
    if rc != 0 or not os.path.exists(exe_path):
        return ("no-build", "", sha_text(out + err))
    return (sha(exe_path), exe_path, "")


def sha_text(t):
    return hashlib.sha256(t.encode("utf-8", "replace")).hexdigest()[:12]


def gen_tests(n, out):
    sys.path.insert(0, os.path.join(ROOT, "scripts", "fuzz"))
    import gen_vs_c
    import gen_programs
    d = os.path.join(out, "gen")
    os.makedirs(d, exist_ok=True)
    files = []
    for s in range(n):
        p = os.path.join(d, f"gvc_{s}.fd")
        with open(p, "w", newline="\n") as f:
            f.write(gen_vs_c.pair(7000 + s)[0])
        files.append(p)
        p = os.path.join(d, f"gp_{s}.fd")
        with open(p, "w", newline="\n") as f:
            f.write(gen_programs.Gen(random.Random(9000 + s)).program())
        files.append(p)
    return files


RETRY_LOCK = threading.Lock()
BASE = {}  # (test, backend) -> (hash, exe kept for running, (rc, out))


def compare(base_fid, var_fid, env_var, backends, tests, out, jobs):
    """Returns (changed binaries, differing outputs, details)."""
    work = os.path.join(out, "work")
    os.makedirs(work, exist_ok=True)
    benv = dict(os.environ)
    venv = dict(os.environ)
    if env_var:
        venv["FID_IROPT_OFF"] = env_var
    jobs_list = [(t, b) for t in tests for b in backends]

    def one(tb):
        t, b = tb
        tag = hashlib.md5((t + b + (env_var or var_fid)).encode()).hexdigest()[:10]
        bt = hashlib.md5((t + b).encode()).hexdigest()[:10]
        e1 = os.path.join(work, f"{bt}-base.exe")
        e2 = os.path.join(work, f"{tag}-var.exe")
        # the base build of a test is the same for every variant: once
        if (t, b) not in BASE:
            h = compile_and_run(base_fid, benv, b, t, e1, out)
            r = run([e1], timeout=60) if h[0] != "no-build" else None
            BASE[(t, b)] = (h[0], r)
        h1 = (BASE[(t, b)][0],)
        h2 = compile_and_run(var_fid, venv, b, t, e2, out)
        res = None
        if h1[0] == "no-build" or h2[0] == "no-build":
            if h1[0] != h2[0]:
                res = (t, b, "builds with one compiler only")
        elif h1[0] != h2[0]:
            r1 = BASE[(t, b)][1]
            r2 = run([e2], timeout=60)
            if (r1[0], r1[1]) != (r2[0], r2[1]):
                res = (t, b, f"exit {r1[0]} vs {r2[0]}; output {len(r1[1])} vs {len(r2[1])} bytes")
            else:
                res = (t, b, None)
        for e in (e1, e2, e2[:-4] + ".pdb", e1[:-4] + ".pdb"):
            if os.path.exists(e):
                try:
                    os.remove(e)
                except OSError:
                    pass
        return h1[0] != h2[0], res

    changed = 0
    diffs = []
    with ThreadPoolExecutor(jobs) as ex:
        for ch, res in ex.map(one, jobs_list):
            changed += 1 if ch else 0
            if res and res[2]:
                diffs.append(res)
    return changed, diffs, len(jobs_list)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("cmd", choices=["toggle", "mutate", "list"])
    ap.add_argument("--fid", default=os.path.join("release", "fid.exe"))
    ap.add_argument("--tree", default=".")
    ap.add_argument("--tests", default="tests/bootstrap/*.fd")
    ap.add_argument("--gen", type=int, default=0)
    ap.add_argument("--sample", type=int, default=0, help="a random sample of this many --tests files")
    ap.add_argument("--out", default=os.path.join("build", "variants"))
    ap.add_argument("--only", default="")
    ap.add_argument("--build-only", action="store_true")
    ap.add_argument("-j", type=int, default=4)
    a = ap.parse_args()
    if a.cmd == "list":
        for n, k, how, b in TOGGLES:
            print(f"toggle {n:18} {k:5} {b}")
        for n, e, b in MUTANTS:
            print(f"mutant {n:18} {e[0]} {b}")
        return
    os.environ.setdefault("CC", "zig cc")
    os.environ["FIDES_NO_CACHE"] = "1"
    out = os.path.abspath(a.out)
    os.makedirs(out, exist_ok=True)
    fid = os.path.abspath(a.fid)
    tree = os.path.abspath(a.tree)
    tests = []
    for g in a.tests.split(","):
        tests += sorted(glob.glob(g))
    if a.sample and len(tests) > a.sample:
        tests = sorted(random.Random(1).sample(tests, a.sample))
    if a.gen:
        tests += gen_tests(a.gen, out)
    only = set(x for x in a.only.split(",") if x)
    rows = []
    if a.cmd == "toggle":
        items = [(n, k, how, b) for n, k, how, b in TOGGLES if not only or n in only]
    else:
        items = [(n, "const", e, b) for n, e, b in MUTANTS if not only or n in only]
    # build every variant that needs it first (one at a time: memory). The
    # base is the unedited tree built the same way, so a const variant
    # differs from it by the edit alone; an env switch runs on the base.
    exes = {}
    base = fid
    if any(k == "const" for _, k, _, _ in items):
        base = build(tree, "base", None, fid, out)
        if not base:
            sys.exit("could not build the base compiler from the tree")
    for n, k, how, b in items:
        exes[n] = base if k == "env" else build(tree, n, how, fid, out)
    if a.build_only:
        return
    fid = base
    report = os.path.join(out, f"{a.cmd}-report.txt")
    with open(report, "w") as rep:
        for n, k, how, b in items:
            if not exes[n]:
                rows.append((n, "build failed", ""))
                continue
            changed, diffs, total = compare(fid, exes[n], how if k == "env" else None, b, tests, out, a.j)
            if a.cmd == "toggle":
                verdict = "ok" if not diffs else f"{len(diffs)} DIFFER"
            else:
                verdict = "killed" if diffs else "SURVIVED"
            line = f"{n:18} {verdict:12} binaries changed in {changed}/{total} ({','.join(b)})"
            print(line, flush=True)
            rep.write(line + "\n")
            for t, bk, why in diffs[:30]:
                rep.write(f"    {bk} {os.path.relpath(t)}: {why}\n")
                print(f"    {bk} {os.path.relpath(t)}: {why}")
            rep.flush()
    print(f"report: {report}")


if __name__ == "__main__":
    main()
