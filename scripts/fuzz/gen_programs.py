#!/usr/bin/env python3
"""Random Fides programs, run on every backend and compared (roadmap VF1).

    python scripts/fuzz/gen_programs.py [-n 200] [--seed 1] [--fid release/fid.exe]
                                        [--backends run,emit,ccompile,checked] [--keep out/dir]

Backends: run (interpreter), ccompile (C), emit (native Windows), checked
(the C path under --checked-alloc). scripts/fuzz/nightly.sh runs them all.

Each program is generated from a seed, so a failure is reproduced with that
seed alone. A program uses i64 arithmetic kept in range (every value is
reduced modulo a prime after each operation, so the checked arithmetic never
overflows), if/else, while loops, helper functions with recursion bounded
by a counter, Vec<i64> pushes and reads, and prints a checksum at the end.

A program counts as a failure when the backends disagree on its output or
exit status, or when one of them crashes. Failures are written to --keep
(default: build/fuzz) as <seed>.fd with each backend's output beside it.

--rich (Nov 15) adds what the plain programs leave out: a struct of two
i64 fields (literal, field reads and writes, copies), an enum with payloads
taken apart by `match`, strings (concatenation, int_to_str, str_len, kept
short) and f64 arithmetic (values that are exact or rounded the IEEE way,
read back through `as i64`; floats are never printed as text, so only the
arithmetic is compared). A seed gives the same program as before without
--rich, so the seeds recorded in logs stay valid.

--batch K (Nov 17) compiles K seeds' programs as one file, so a long run
pays the compiler's start-up once per K programs: each program's functions
are renamed p<seed>_f<i>, its main becomes p<seed>_main, and main calls
them in seed order, printing `== seed <s>` before each one's output. When
the backends disagree on a batch, the seeds whose sections differ (all of
them when a backend fails to build the batch) are run again alone (when
several differ, only the first runs alone and the seeds after it run again
as a smaller batch: a panic stops one backend's run and makes every later
seed differ); those
that still disagree are kept as <seed>.fd like any other failure, and a
batch whose seeds all agree alone is kept as batch-<first>.fd (a finding
that needs a large module). --batch 1 (the default) is the old behaviour.

Each kept <seed>.<backend>.out holds `exit: <status>`, the program's
output, and after a `--- stderr ---` line what the compiler and the program
wrote to stderr; scripts/fuzz/triage.py <keep dir> groups them. A program
whose backends disagree is run a second time first: if they agree then (a
C compiler or fid out of memory on a busy machine), it goes to
<keep>/flaky/ and is not counted. --retries K (Nov 17, default 1) allows K
such runs, each of only the backends that failed or gave the minority's
result; a batch's backend that could not build or finish it is likewise run
again before the batch is split. A failed build's stderr ends with
`(compiler exit N)`, so triage.py can tell a crash of fid from a rejection,
and triage.py puts failures that point at the machine (out of memory, a C
compiler that could not start, a timeout) in groups of their own.
"""
import argparse
import os
import random
import re
import subprocess
import sys

MOD = 1000003
# seconds a program may run (times the batch scale); --run-timeout. The
# interpreter needs ~9 ms per call that makes a Vec (Nov 17, stage 0): some
# rich programs take 8 minutes on `run` and under a second compiled
RUN_TIMEOUT = 60


class Gen:
    def __init__(self, rng, rich=False):
        self.r = rng
        self.funcs = []
        # rich: struct Pt, enum Sh, strings and floats (see the header)
        self.rich = rich
        self.nq = 0  # struct copies made so far (each gets a fresh name)

    def rich_stmt(self, local, pad):
        """One statement on the rich-mode state (p: Pt, s: str, fv: f64)."""
        r = self.r
        e = self.expr(local, 2)
        k = r.randint(0, 6)
        if k == 0:
            return [f"{pad}p.a = (p.a + {e}) % {MOD};"]
        if k == 1:
            return [f"{pad}p.b = (p.b * 3 + p.a + {e}) % {MOD};"]
        if k == 2:
            # a copy of the struct, changed, must not change the original
            self.nq += 1
            q = f"q{self.nq}"
            return [f"{pad}var {q}: Pt = p;",
                    f"{pad}{q}.a = ({q}.a + 1) % {MOD};",
                    f"{pad}acc = (acc + {q}.a + p.a) % {MOD};"]
        if k == 3:
            ctor = r.choice([f"Sh::One({e})", f"Sh::Two({e}, {self.expr(local, 1)})", "Sh::Zero"])
            return [f"{pad}acc = (acc + sh_val({ctor})) % {MOD};"]
        if k == 4:
            return [f"{pad}s = s + int_to_str({e} % 1000);",
                    f"{pad}if str_len(s) > 40 {{ s = \"\"; }}"]
        if k == 5:
            return [f"{pad}acc = (acc + str_len(s) * 7) % {MOD};"]
        return [f"{pad}fv = (({e} % 1000) as f64) * 0.25 + fv * 0.5;",
                f"{pad}acc = (acc + (fv as i64)) % {MOD};"]

    def expr(self, vars_, depth):
        r = self.r
        if depth <= 0 or r.random() < 0.3:
            if vars_ and r.random() < 0.7:
                return r.choice(vars_)
            return str(r.randint(0, 999))
        k = r.randint(0, 6)
        a = self.expr(vars_, depth - 1)
        b = self.expr(vars_, depth - 1)
        if k == 0:
            return f"(({a} + {b}) % {MOD})"
        if k == 1:
            return f"(({a} * {b}) % {MOD})"
        if k == 2:
            return f"(({a} - {b} + {MOD}) % {MOD})"
        if k == 3:
            return f"({a} / ({b} % 97 + 1))"
        if k == 4:
            return f"(({a} ^ {b}) % {MOD})"
        if k == 5 and self.funcs:
            f = r.choice(self.funcs)
            return f"{f}({a} % 50)"
        return f"(({a} & 1023) + ({b} >> 3))"

    def cond(self, vars_):
        ops = ["<", ">", "==", "!=", "<=", ">="]
        return f"{self.expr(vars_, 2)} {self.r.choice(ops)} {self.expr(vars_, 2)}"

    @staticmethod
    def target(v):
        """The variable an assignment writes: never a loop counter (i<d>_<n>).
        Before Nov 17 a body could set its loop's counter back (seed
        20261220: `i2_0 = ...` inside `while i2_0 < 12`), so the program
        never ended and every backend timed out or ran out of memory. The
        assignment goes to `acc` instead; the random numbers drawn stay the
        same, so a seed whose program wrote no counter is unchanged."""
        return "acc" if re.match(r"i\d+_\d+$", v) else v

    def block(self, vars_, depth, indent):
        r = self.r
        pad = "    " * indent
        out = []
        local = list(vars_)
        for _ in range(r.randint(1, 5)):
            k = r.randint(0, 6)
            if k <= 1 or not local:
                name = f"v{len(local)}_{indent}"
                while name in local:
                    name += "x"
                out.append(f"{pad}var {name}: i64 = {self.expr(local, 3)};")
                local.append(name)
            elif k == 2:
                v = self.target(r.choice(local))
                out.append(f"{pad}{v} = {self.expr(local, 3)};")
            elif k == 3 and depth > 0:
                out.append(f"{pad}if {self.cond(local)} {{")
                out += self.block(local, depth - 1, indent + 1)
                out.append(f"{pad}}} else {{")
                out += self.block(local, depth - 1, indent + 1)
                out.append(f"{pad}}}")
            elif k == 4 and depth > 0:
                i = f"i{indent}_{len(out)}"
                out.append(f"{pad}var {i}: i64 = 0;")
                out.append(f"{pad}while {i} < {r.randint(1, 20)} {{")
                out += self.block(local + [i], depth - 1, indent + 1)
                out.append(f"{pad}    {i} = {i} + 1;")
                out.append(f"{pad}}}")
            elif k == 5:
                out.append(f"{pad}xs.push({self.expr(local, 2)});")
            elif self.rich and r.random() < 0.5:
                # rich mode only: the plain programs consume no extra random
                # numbers here, so a seed gives the same program as before
                out += self.rich_stmt(local, pad)
            else:
                v = self.target(r.choice(local))
                out.append(f"{pad}if xs.len() > 0 {{ {v} = (xs.get({self.expr(local, 1)} % xs.len()) + {v}) % {MOD}; }}")
        out.append(f"{pad}acc = (acc * 31 + {r.choice(local) if local else '7'}) % {MOD};")
        return out

    RICH_DECLS = [
        "struct Pt {",
        "    a: i64,",
        "    b: i64,",
        "}",
        "",
        "enum Sh {",
        "    One(i64),",
        "    Two(i64, i64),",
        "    Zero,",
        "}",
        "",
        "fn sh_val(v: Sh) -> i64 {",
        "    match v {",
        f"        One(x) => {{ return x % {MOD}; }}",
        f"        Two(x, y) => {{ return (x * 2 + y) % {MOD}; }}",
        "        Zero => { return 7; }",
        "    }",
        "    return 0;",
        "}",
        "",
    ]
    # the rich-mode state every function and main starts with
    RICH_LOCALS = [
        "    var p: Pt = Pt { a: 1, b: 2 };",
        "    var s: str = \"\";",
        "    var fv: f64 = 0.0;",
    ]
    # ... and folds into its result (so each local is written and read)
    RICH_FOLD = [
        f"    p.b = (p.b + acc) % {MOD};",
        "    s = s + \".\";",
        "    fv = fv + 1.0;",
        f"    acc = (acc + p.a + p.b + str_len(s) + (fv as i64)) % {MOD};",
    ]

    def program(self, decls=True):
        # decls=False leaves out the rich-mode struct/enum declarations
        # (batch_program writes them once for the whole batch)
        lines = list(self.RICH_DECLS) if self.rich and decls else []
        for fi in range(self.r.randint(0, 3)):
            name = f"f{fi}"
            body = self.block(["n"], 1, 1)
            lines.append(f"fn {name}(n: i64) -> i64 {{")
            lines.append("    var acc: i64 = n;")
            lines.append("    var xs: Vec<i64> = Vec::new();")
            if self.rich:
                lines += self.RICH_LOCALS
            lines += body
            if fi > 0:
                lines.append(f"    if n > 0 {{ acc = (acc + f{fi - 1}(n - 1)) % {MOD}; }}")
            if self.rich:
                lines += self.RICH_FOLD
            lines.append("    return acc;")
            lines.append("}")
            lines.append("")
            self.funcs.append(name)
        lines.append("fn main() -> i64 {")
        lines.append("    var acc: i64 = 1;")
        lines.append("    var xs: Vec<i64> = Vec::new();")
        if self.rich:
            lines += self.RICH_LOCALS
        lines += self.block([], 3, 1)
        if self.rich:
            lines += self.RICH_FOLD
        lines.append("    println(acc);")
        lines.append("    println(xs.len());")
        if self.rich:
            lines.append("    println(p.a);")
            lines.append("    println(p.b);")
            lines.append("    println(s);")
            lines.append("    println((fv * 1000.0) as i64);")
        lines.append("    return 0;")
        lines.append("}")
        return "\n".join(lines) + "\n"


def _clean(exe):
    """Remove what one build left beside `exe` (the executable, the C file,
    the PDB, the cache stamp); the kept .fd and .out files stay."""
    base = os.path.splitext(exe)[0]
    for p in (exe, exe + ".fides-stamp", base + ".c", base + ".c.err", base + ".pdb", base + ".ilk"):
        if os.path.exists(p):
            try:
                os.remove(p)
            except OSError:
                pass


class _Done:
    def __init__(self, rc, out, err):
        self.returncode, self.stdout, self.stderr = rc, out, err


def _run(cmd, timeout, cwd=None):
    """subprocess.run(cmd, capture_output=True, text=True, timeout=...), but
    a timeout kills the whole process tree: `fid run --checked-alloc` starts
    the program as a child of its own, which kept the pipes open after
    subprocess.run killed fid, so the harness waited for ever."""
    kw = {}
    if os.name != "nt":
        kw["start_new_session"] = True
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                         text=True, errors="replace", cwd=cwd, **kw)
    try:
        out, err = p.communicate(timeout=timeout)
    except subprocess.TimeoutExpired:
        if os.name == "nt":
            subprocess.run(["taskkill", "/F", "/T", "/PID", str(p.pid)],
                           capture_output=True)
        else:
            import signal
            try:
                os.killpg(p.pid, signal.SIGKILL)
            except OSError:
                p.kill()
        p.communicate()
        raise
    return _Done(p.returncode, out, err)


def run_backend(fid, backend, src, workdir, seed, scale=1):
    """(exit status, stdout, stderr) of one backend on `src`. The status is
    "build-failed" when the compiler rejects or crashes on the program and
    "timeout" when a step runs out of time; `scale` multiplies the time
    limits (a batch holds several programs)."""
    exe = os.path.join(workdir, f"{seed}-{backend.replace(':', '-')}.exe")
    t_build, t_run = 300 * scale, RUN_TIMEOUT * scale
    src = os.path.abspath(src)
    try:
        if backend.startswith("emit:"):
            # the machine layer for a target (release M9 native on Linux):
            # emit:x86_64-linux, emit:aarch64-linux (runs under qemu-user)
            target = backend.split(":", 1)[1]
            p = _run([fid, "emit", src, "--target", target, "-o", exe], t_build)
            if p.returncode != 0 or not os.path.exists(exe):
                return "build-failed", "", p.stdout + p.stderr + f"\n(compiler exit {p.returncode})"
            runner = []
            if target.startswith("aarch64") and os.uname().machine != "aarch64":
                runner = ["qemu-aarch64"]
            elif target.startswith("riscv64"):
                runner = ["qemu-riscv64"]
            q = _run(runner + [exe], t_run)
            return q.returncode, q.stdout, q.stderr
        if backend == "run":
            p = _run([fid, "run", src], t_run)
            return p.returncode, p.stdout, p.stderr
        if backend == "checked":
            # The C path under the checked allocator (release M7): a use after
            # free or an overrun faults instead of reading stale memory.
            # `fid run --checked-alloc` builds _fides_memcheck_tmp.exe in the
            # current directory: run it in workdir, and drop the line that
            # names the executable from the program's output.
            p = _run([fid, "run", src, "--checked-alloc"], t_build + t_run, cwd=workdir)
            out = "".join(l for l in p.stdout.splitlines(True)
                          if not l.startswith("Built native executable:"))
            return p.returncode, out, p.stderr
        p = _run([fid, backend, src, "-o", exe], t_build)
        if p.returncode != 0 or not os.path.exists(exe):
            # the compiler's exit status tells a crash of fid (a Windows
            # status such as 3221225725, stack overflow) from a rejection (1)
            return "build-failed", "", p.stdout + p.stderr + f"\n(compiler exit {p.returncode})"
        q = _run([exe], t_run)
        return q.returncode, q.stdout, p.stderr + q.stderr
    except subprocess.TimeoutExpired:
        return "timeout", "", ""
    finally:
        _clean(exe)
        if backend == "checked":
            _clean(os.path.join(workdir, "_fides_memcheck_tmp.exe"))


def batch_program(seeds, rich):
    """The programs of `seeds` as one file (--batch, see the header)."""
    lines = list(Gen.RICH_DECLS) if rich else []
    calls = []
    for s in seeds:
        text = Gen(random.Random(s), rich).program(decls=False)
        text = re.sub(r"\bf(\d+)\(", rf"p{s}_f\1(", text)
        text = text.replace("fn main() -> i64 {", f"fn p{s}_main() -> i64 {{", 1)
        lines.append(text)
        calls.append(f"    println(\"== seed {s}\");")
        calls.append(f"    var r{s}: i64 = p{s}_main();")
        calls.append(f"    if r{s} != 0 {{ println(r{s}); }}")
    lines.append("fn main() -> i64 {")
    lines += calls
    lines.append("    return 0;")
    lines.append("}")
    return "\n".join(lines) + "\n"


def sections(out):
    """seed -> that program's lines of a batch's output."""
    res, cur = {}, None
    for l in out.replace("\r", "").split("\n"):
        if l.startswith("== seed "):
            cur = int(l[8:])
            res[cur] = []
        elif cur is not None:
            res[cur].append(l)
    return res


def write_outs(keep, name, results):
    for b, (rc, out, err) in results.items():
        with open(os.path.join(keep, f"{name}.{b.replace(':', '-')}.out"), "w",
                  encoding="utf-8", newline="\n") as f:
            f.write(f"exit: {rc}\n{out.replace(chr(13), '')}\n--- stderr ---\n{err.replace(chr(13), '')}")


def agree(results):
    # a generated program is valid: when no backend builds it, that is a
    # failure too (with --backends emit,ccompile both said "build-failed"
    # and the program counted as agreeing)
    if any(rc == "build-failed" for rc, _, _ in results.values()):
        return False
    return len({(rc, out.replace("\r", "")) for rc, out, _ in results.values()}) == 1


def error_seed(src, seeds, results):
    """The seed of a batch whose text holds the line a compile error names
    (`Parse error at 195:21`, `--> file:195:21`), or None."""
    line = None
    for rc, out, err in results.values():
        m = re.search(r"(?:error[^\n]*? at |--> [^\n]*?:)(\d+):\d+", out + err)
        if m:
            line = int(m.group(1))
            break
    if line is None:
        return None
    owner = None
    with open(src, encoding="utf-8") as f:
        for i, l in enumerate(f, 1):
            if i > line:
                break
            m = re.match(r"fn p(\d+)_", l)
            if m and int(m.group(1)) in seeds:
                owner = int(m.group(1))
    return owner


def check_one(a, backends, seed):
    """Run one seed's program alone; keep it when the backends disagree."""
    src = os.path.join(a.keep, f"{seed}.fd")
    with open(src, "w", newline="\n") as f:
        f.write(Gen(random.Random(seed), a.rich).program())
    results = {b: run_backend(a.fid, b, src, a.keep, seed) for b in backends}
    if agree(results):
        os.remove(src)
        return False
    # once more: on a busy machine the C compiler or fid itself can run out
    # of memory. A program that agrees the second time is kept under
    # flaky/ and not counted; run it again on a quiet machine.
    # --retries k (default 1) runs it up to k more times; only the backends
    # that did not give the majority's result are run again (Nov 17: on a
    # machine running several builds, zig cc ran out of memory twice in a row)
    again = dict(results)
    for _ in range(max(1, getattr(a, "retries", 1))):
        votes = {}
        for rc, out, _e in again.values():
            k = (rc, out.replace("\r", ""))
            votes[k] = votes.get(k, 0) + 1
        best = max(votes, key=votes.get)
        no_majority = votes[best] * 2 <= len(backends)
        for b in backends:
            if (no_majority or again[b][0] in ("build-failed", "timeout")
                    or (again[b][0], again[b][1].replace("\r", "")) != best):
                again[b] = run_backend(a.fid, b, src, a.keep, seed)
        if agree(again):
            break
    if agree(again):
        fl = os.path.join(a.keep, "flaky")
        os.makedirs(fl, exist_ok=True)
        os.replace(src, os.path.join(fl, f"{seed}.fd"))
        write_outs(fl, str(seed), results)
        print(f"seed {seed}: disagreed once, agrees on a later run -> {fl}", flush=True)
        return False
    print(f"seed {seed}: backends disagree -> {src}", flush=True)
    # the last run's results: what still disagrees after the retries
    write_outs(a.keep, str(seed), again)
    return True


def split_batch(a, backends, seeds, src, results, s0, rest):
    """Seed `s0` of the batch at `src` alone, then `rest` as a batch of its
    own; returns the seeds that fail. The batch file is set aside while the
    smaller batch runs (that one may start with the same seed, so have the
    same name) and kept, with `results`, when no seed fails alone."""
    with open(src, encoding="utf-8") as f:
        text = f.read()
    os.remove(src)
    bad = [s0] if check_one(a, backends, s0) else []
    if rest:
        bad += check_batch(a, backends, rest)
    if not bad:
        name = f"batch-{seeds[0]}-{len(seeds)}"
        keep = os.path.join(a.keep, name + ".fd")
        with open(keep, "w", newline="\n") as f:
            f.write(text)
        print(f"batch {seeds[0]}..{seeds[-1]}: no seed fails alone -> {keep}", flush=True)
        write_outs(a.keep, name, results)
        a.batch_only = getattr(a, "batch_only", 0) + 1
    return bad


def check_batch(a, backends, seeds):
    """Run `seeds` as one batch; returns the seeds that disagree alone."""
    first = seeds[0]
    src = os.path.join(a.keep, f"batch-{first}.fd")
    with open(src, "w", newline="\n") as f:
        f.write(batch_program(seeds, a.rich))
    scale = max(1, len(seeds) // 4)
    results = {b: run_backend(a.fid, b, src, a.keep, f"batch-{first}", scale) for b in backends}
    # a backend that could not build or finish the batch is tried again
    # (--retries) before the batch is split: on a busy machine zig cc runs
    # out of memory on a large file, and splitting runs every seed alone
    for _ in range(max(1, getattr(a, "retries", 1))):
        if agree(results):
            break
        failed = [b for b in backends if results[b][0] in ("build-failed", "timeout")]
        if not failed:
            break
        for b in failed:
            results[b] = run_backend(a.fid, b, src, a.keep, f"batch-{first}", scale)
    if agree(results):
        os.remove(src)
        return []
    s_err = None
    if any(rc in ("build-failed", "timeout") or (b == "run" and rc == 1 and not out)
           for b, (rc, out, _) in results.items()):
        s_err = error_seed(src, seeds, results)
    if s_err is not None and len(seeds) > 1:
        # Nov 17: the compiler rejected one seed's program (stage 0 has no
        # `q4` variable names) and all 50 seeds ran alone; the seed the
        # error's line falls in runs alone, the others as a batch again
        rest = [s for s in seeds if s != s_err]
        print(f"batch {first}..{seeds[-1]}: does not compile at seed {s_err}; "
              f"it runs alone, the {len(rest)} others as a batch", flush=True)
        return split_batch(a, backends, seeds, src, results, s_err, rest)
    if any(rc in ("build-failed", "timeout") for rc, _, _ in results.values()):
        suspects = list(seeds)
    else:
        secs = {b: sections(out) for b, (rc, out, _) in results.items()}
        suspects = [s for s in seeds
                    if len({tuple(sec.get(s, ["<missing>"])) for sec in secs.values()}) > 1]
        if not suspects:
            suspects = list(seeds)  # only the exit status differs
        elif len(suspects) > 1:
            # Nov 17: one seed that panics on a backend (M25 on `checked`)
            # stops that backend's run, so every later seed looked like a
            # suspect and ran alone (29 of 50). Run the first suspect alone
            # and the seeds after it again as a smaller batch.
            s0 = suspects[0]
            rest = seeds[seeds.index(s0) + 1:]
            print(f"batch {first}..{seeds[-1]}: backends disagree from seed {s0}; "
                  f"it runs alone, the {len(rest)} after it as a batch", flush=True)
            return split_batch(a, backends, seeds, src, results, s0, rest)
    print(f"batch {first}..{seeds[-1]}: backends disagree; {len(suspects)} seed(s) run alone",
          flush=True)
    bad = [s for s in suspects if check_one(a, backends, s)]
    if bad:
        os.remove(src)
    else:
        print(f"batch {first}..{seeds[-1]}: no seed fails alone -> {src}", flush=True)
        write_outs(a.keep, f"batch-{first}", results)
        a.batch_only = getattr(a, "batch_only", 0) + 1
    return bad


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("-n", type=int, default=100)
    ap.add_argument("--seed", type=int, default=1)
    ap.add_argument("--fid", default=os.path.join("release", "fid.exe" if os.name == "nt" else "fid"))
    ap.add_argument("--backends", default="run,emit,ccompile")
    ap.add_argument("--keep", default=os.path.join("build", "fuzz"))
    ap.add_argument("--rich", action="store_true",
                    help="also structs, enums, strings and floats (see the header)")
    ap.add_argument("--batch", type=int, default=1,
                    help="programs compiled together as one file (see the header)")
    ap.add_argument("--retries", type=int, default=1,
                    help="runs again of a disagreeing program before it is kept")
    ap.add_argument("--run-timeout", type=int, default=60,
                    help="seconds a program may run (times the batch scale)")
    a = ap.parse_args()
    global RUN_TIMEOUT
    RUN_TIMEOUT = a.run_timeout
    backends = a.backends.split(",")
    os.makedirs(a.keep, exist_ok=True)
    fails = 0
    batch_only = 0
    seeds = list(range(a.seed, a.seed + a.n))
    k = max(1, a.batch)
    for i in range(0, len(seeds), k):
        group = seeds[i:i + k]
        if k == 1:
            fails += check_one(a, backends, group[0])
            continue
        bad = check_batch(a, backends, group)
        fails += len(bad)
    # counted by check_batch (also for the smaller batches it splits off)
    batch_only = getattr(a, "batch_only", 0)
    extra = f", {batch_only} batch-only disagreements" if k > 1 else ""
    print(f"{a.n} programs, {fails} disagreements{extra}", flush=True)
    sys.exit(1 if fails or batch_only else 0)


if __name__ == "__main__":
    main()
