#!/usr/bin/env python3
"""Self-test of the VF1 harness (scripts/fuzz/gen_programs.py, triage.py).

    python scripts/fuzz/test_gen_programs.py            # no compiler needed
    python scripts/fuzz/test_gen_programs.py --fid release/fid.exe
                                                        # also run a batch

Checks, without a compiler:
  - a seed gives the program it gave when the runs of
    docs/verification/fuzz-runs.md were recorded (so their seeds stay
    valid): pinned SHA-256 prefixes of seeds 1, 2, 3, 42, 1000000;
  - no program assigns a loop counter (such a program may never end);
  - --rich programs differ from the plain ones and declare Pt and Sh;
  - batch_program renames every helper and main (no `f0(` or `fn main`
    of a program is left) and writes the rich declarations once;
  - sections() splits a batch's output back into seeds;
  - triage.py's majority vote picks the expected output when there is no
    C oracle (.want), and the interpreter's when there is no majority;
    env_reason() tells out-of-memory and timeouts from compiler failures;
  - check_one --retries runs only the failing backend again and sends a
    program that agrees later to flaky/; check_batch runs the first
    differing seed alone and the seeds after it as a smaller batch
    (a fake backend, no compiler).
With --fid: a batch of six seeds (three plain, three rich) is run on the
interpreter, and each seed's section must equal that seed's program run
alone.
Exit status 0 when every check holds.
"""
import argparse
import hashlib
import os
import random
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import gen_programs  # noqa: E402
import triage  # noqa: E402

PINNED = {  # sha256(program)[:16] of the plain programs (generator of Nov 17:
    # seeds 2, 42 and 1000000 are as on Nov 7; 1 and 3 wrote a loop counter
    # then, which Gen.target now sends to `acc`)
    1: "6ac834c3f70c38bb",
    2: "b1a4503140f7a59e",
    3: "7bbfbaa7f3b2eab2",
    42: "c62a38abb28480d0",
    1000000: "0d8489d9b6051bf3",
}

fails = []


def check(cond, what):
    print(("ok   " if cond else "FAIL ") + what)
    if not cond:
        fails.append(what)


def prog(seed, rich=False, decls=True):
    return gen_programs.Gen(random.Random(seed), rich).program(decls)


def offline():
    for s, h in PINNED.items():
        got = hashlib.sha256(prog(s).encode()).hexdigest()[:16]
        check(got == h, f"plain seed {s} is the recorded program ({got})")
    import re
    bad = [s for s in list(range(1, 300)) + [20261220]
           for rich in (False, True)
           if re.search(r"^\s*(i\d+_\d+) = (?!\1 \+ 1;)", prog(s, rich), re.M)]
    check(not bad, f"no program assigns a loop counter {bad[:5]}")
    r = prog(7, rich=True)
    check(r != prog(7) and "struct Pt {" in r and "enum Sh {" in r,
          "a --rich program declares Pt and Sh")
    check("struct Pt {" not in prog(7, rich=True, decls=False),
          "decls=False leaves the declarations out")
    seeds = [11, 12, 13, 14]
    b = gen_programs.batch_program(seeds, True)
    check(b.count("struct Pt {") == 1 and b.count("enum Sh {") == 1,
          "a rich batch declares Pt and Sh once")
    check(b.count("fn main() -> i64 {") == 1, "a batch has one main")
    for s in seeds:
        check(f"fn p{s}_main() -> i64 {{" in b and f"== seed {s}" in b,
              f"seed {s}'s main is p{s}_main and is announced")
    bare = re.findall(r"\b(f\d+)\(", b)
    check(not bare, "no helper keeps its unprefixed name")
    out = "== seed 11\n1\n2\n== seed 12\n3\n== seed 13\n"
    sec = gen_programs.sections(out)
    check(sec.get(11) == ["1", "2"] and sec.get(12) == ["3"] and sec.get(13) == [""],
          "sections() splits a batch's output by seed")
    # triage: majority without a .want, the interpreter's on a tie
    with tempfile.TemporaryDirectory() as d:
        def w(name, rc, body):
            with open(os.path.join(d, name), "w", encoding="utf-8", newline="\n") as f:
                f.write(f"exit: {rc}\n{body}\n--- stderr ---\n")
        w("5.run.out", 0, "10")
        w("5.ccompile.out", 0, "10")
        w("5.emit.out", 0, "11")
        w("5.checked.out", 0, "10")
        w("6.run.out", 0, "7")
        w("6.emit.out", 1, "")
        got = {s: (rc, body) for s, rc, body in triage.wants(d)}
        check(got.get("5") == ("0", "10"), "triage: the majority is the expected output")
        check(got.get("6") == ("0", "7"), "triage: no majority -> the interpreter's output")
    # triage: a failure of the machine is told apart from one of the compiler
    check(triage.env_reason("build-failed", "C compilation failed\nLLVM ERROR: out of memory\n") != "",
          "triage: zig out of memory is the machine's")
    check(triage.env_reason("timeout", "") == "timeout", "triage: a timeout is the machine's")
    check(triage.env_reason("1", "Panic: Index out of bounds\n") == "",
          "triage: a panic is the compiler's")
    check("M25" in triage.known_hint(".", "0", "checked",
                                     "Panic: Invalid memory access (null or dangling pointer) at 0x1C8EE1DFFE8\n"),
          "triage: the checked-alloc probe fault is named M25")
    with tempfile.TemporaryDirectory() as d:
        with open(os.path.join(d, "9.fd"), "w") as f:
            f.write("fn main() -> i64 {\n    var x: i64 = 1;\n    x = x;\n    return 0;\n}\n")
        check("M24" in triage.known_hint(d, "9", "emit", ""),
              "triage: an emit difference in a program with `x = x;` points at M24")
        check(triage.known_hint(d, "9", "run", "") == "", "triage: no hint for run")
    # check_one --retries: only the backend that failed runs again, and a
    # program that agrees on a later run goes to flaky/ (not counted)
    calls = []
    plan = {"emit": [("build-failed", "", "LLVM ERROR: out of memory"),
                     ("build-failed", "", "LLVM ERROR: out of memory"), (0, "5\n", "")]}
    real = gen_programs.run_backend

    def fake(fid, b, src, workdir, seed, scale=1):
        calls.append(b)
        if b in plan and plan[b]:
            return plan[b].pop(0)
        return 0, "5\n", ""
    gen_programs.run_backend = fake
    try:
        with tempfile.TemporaryDirectory() as d:
            class A:
                pass
            a = A()
            a.keep, a.fid, a.rich, a.retries = d, "fid", False, 2
            kept = gen_programs.check_one(a, ["run", "ccompile", "emit"], 77)
            check(not kept and os.path.exists(os.path.join(d, "flaky", "77.fd")),
                  "check_one: agrees on the 3rd run -> flaky/, not kept")
            check(calls == ["run", "ccompile", "emit", "emit", "emit"],
                  f"check_one: only the failing backend is run again ({calls})")
            calls.clear()
            plan["emit"] = [(0, "6\n", "")] * 3
            a.retries = 1
            kept = gen_programs.check_one(a, ["run", "ccompile", "emit"], 78)
            check(kept and os.path.exists(os.path.join(d, "78.emit.out")),
                  "check_one: a disagreement that stays is kept with its outputs")
        # check_batch: seed 3 panics on `checked`, which stops that run, so
        # 4 and 5 are missing there too; 3 runs alone, 4..5 as a batch
        srcs = []

        def fake2(fid, b, src, workdir, seed, scale=1):
            name = os.path.basename(src)
            srcs.append((name, b))
            if name.startswith("batch-"):
                first = int(name[6:-3])
                ss = list(range(first, 6))
                if b == "checked" and 3 in ss:
                    return 1, "".join(f"== seed {s}\n1\n" for s in ss if s < 3) + "== seed 3\n", "Panic"
                return 0, "".join(f"== seed {s}\n1\n" for s in ss), ""
            if b == "checked" and name == "3.fd":
                return 1, "", "Panic"
            return 0, "1\n", ""
        gen_programs.run_backend = fake2
        with tempfile.TemporaryDirectory() as d:
            class A2:
                pass
            a = A2()
            a.keep, a.fid, a.rich, a.retries = d, "fid", False, 1
            bad = gen_programs.check_batch(a, ["run", "checked"], [1, 2, 3, 4, 5])
            alone = sorted({n for n, _ in srcs if not n.startswith("batch-")})
            batches = sorted({n for n, _ in srcs if n.startswith("batch-")})
            check(bad == [3], f"check_batch: only seed 3 fails ({bad})")
            check(alone == ["3.fd"], f"check_batch: only the first suspect runs alone ({alone})")
            check(batches == ["batch-1.fd", "batch-4.fd"],
                  f"check_batch: the seeds after it run again as a batch ({batches})")
        # check_batch: the compiler rejects seed 12's program; the error's
        # line names it, so 12 runs alone and 11, 13 as a batch again
        runs = []

        def fake3(fid, b, src, workdir, seed, scale=1):
            name = os.path.basename(src)
            runs.append(name)
            text = open(src).read()
            if "fn p12_main" in text or name == "12.fd":
                lines = text.split("\n")
                at = next((i + 1 for i, l in enumerate(lines) if l.startswith("fn p12_")), 1)
                if name == "12.fd":
                    at = 1
                msg = f"error[E020]: Parse error at {at}:5: expected identifier\n"
                if b == "run":
                    return 1, "", msg
                return "build-failed", "", msg
            ss = [int(l.split("== seed ")[1].split('"')[0]) for l in text.split("\n")
                  if "== seed " in l]
            return 0, "".join(f"== seed {s}\n1\n" for s in ss) if ss else "1\n", ""
        gen_programs.run_backend = fake3
        with tempfile.TemporaryDirectory() as d:
            a = A2()
            a.keep, a.fid, a.rich, a.retries = d, "fid", False, 1
            bad = gen_programs.check_batch(a, ["run", "emit"], [11, 12, 13])
            check(bad == [12], f"check_batch: the rejected seed is the one kept ({bad})")
            check(sorted(set(runs)) == ["12.fd", "batch-11.fd"],
                  f"check_batch: 12 alone, then 11 and 13 as a batch ({sorted(set(runs))})")
            check(os.path.exists(os.path.join(d, "12.fd"))
                  and not os.path.exists(os.path.join(d, "batch-11.fd")),
                  "check_batch: the failing seed is kept, the batch file is not")
    finally:
        gen_programs.run_backend = real


def online(fid):
    seeds = [101, 102, 103]
    env = dict(os.environ)
    with tempfile.TemporaryDirectory() as d:
        for rich in (False, True):
            bp = os.path.join(d, f"batch{int(rich)}.fd")
            with open(bp, "w", newline="\n") as f:
                f.write(gen_programs.batch_program(seeds, rich))
            p = subprocess.run([fid, "run", bp], capture_output=True, text=True, timeout=600,
                               cwd=d, env=env)
            check(p.returncode == 0, f"batch (rich={rich}) runs (exit {p.returncode})")
            sec = gen_programs.sections(p.stdout)
            for s in seeds:
                sp = os.path.join(d, f"{s}.fd")
                with open(sp, "w", newline="\n") as f:
                    f.write(prog(s, rich))
                q = subprocess.run([fid, "run", sp], capture_output=True, text=True,
                                   timeout=300, cwd=d, env=env)
                alone = q.stdout.replace("\r", "").rstrip("\n").split("\n")
                inb = [l for l in sec.get(s, ["<missing>"])]
                while inb and inb[-1] == "":
                    inb.pop()
                check(q.returncode == 0 and inb == alone,
                      f"seed {s} (rich={rich}) prints the same in the batch and alone")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--fid", default="")
    a = ap.parse_args()
    offline()
    if a.fid:
        online(a.fid)
    print(f"{len(fails)} failed" if fails else "all checks passed")
    sys.exit(1 if fails else 0)


if __name__ == "__main__":
    main()
