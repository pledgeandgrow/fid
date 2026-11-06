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
"""
import argparse
import os
import random
import subprocess
import sys

MOD = 1000003


class Gen:
    def __init__(self, rng):
        self.r = rng
        self.funcs = []

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
                v = r.choice(local)
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
            else:
                v = r.choice(local)
                out.append(f"{pad}if xs.len() > 0 {{ {v} = (xs.get({self.expr(local, 1)} % xs.len()) + {v}) % {MOD}; }}")
        out.append(f"{pad}acc = (acc * 31 + {r.choice(local) if local else '7'}) % {MOD};")
        return out

    def program(self):
        lines = []
        for fi in range(self.r.randint(0, 3)):
            name = f"f{fi}"
            body = self.block(["n"], 1, 1)
            lines.append(f"fn {name}(n: i64) -> i64 {{")
            lines.append("    var acc: i64 = n;")
            lines.append("    var xs: Vec<i64> = Vec::new();")
            lines += body
            if fi > 0:
                lines.append(f"    if n > 0 {{ acc = (acc + f{fi - 1}(n - 1)) % {MOD}; }}")
            lines.append("    return acc;")
            lines.append("}")
            lines.append("")
            self.funcs.append(name)
        lines.append("fn main() -> i64 {")
        lines.append("    var acc: i64 = 1;")
        lines.append("    var xs: Vec<i64> = Vec::new();")
        lines += self.block([], 3, 1)
        lines.append("    println(acc);")
        lines.append("    println(xs.len());")
        lines.append("    return 0;")
        lines.append("}")
        return "\n".join(lines) + "\n"


def run_backend(fid, backend, src, workdir, seed):
    exe = os.path.join(workdir, f"{seed}-{backend}.exe")
    try:
        if backend == "run":
            p = subprocess.run([fid, "run", src], capture_output=True, text=True, timeout=60)
            return p.returncode, p.stdout
        if backend == "checked":
            # The C path under the checked allocator (release M7): a use after
            # free or an overrun faults instead of reading stale memory
            p = subprocess.run([fid, "run", src, "--checked-alloc"], capture_output=True, text=True, timeout=300)
            return p.returncode, p.stdout
        p = subprocess.run([fid, backend, src, "-o", exe], capture_output=True, text=True, timeout=300)
        if p.returncode != 0 or not os.path.exists(exe):
            return "build-failed", p.stdout + p.stderr
        q = subprocess.run([exe], capture_output=True, text=True, timeout=60)
        return q.returncode, q.stdout
    except subprocess.TimeoutExpired:
        return "timeout", ""
    finally:
        if os.path.exists(exe):
            os.remove(exe)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("-n", type=int, default=100)
    ap.add_argument("--seed", type=int, default=1)
    ap.add_argument("--fid", default=os.path.join("release", "fid.exe" if os.name == "nt" else "fid"))
    ap.add_argument("--backends", default="run,emit,ccompile")
    ap.add_argument("--keep", default=os.path.join("build", "fuzz"))
    a = ap.parse_args()
    backends = a.backends.split(",")
    os.makedirs(a.keep, exist_ok=True)
    fails = 0
    for seed in range(a.seed, a.seed + a.n):
        src = os.path.join(a.keep, f"{seed}.fd")
        with open(src, "w", newline="\n") as f:
            f.write(Gen(random.Random(seed)).program())
        results = {b: run_backend(a.fid, b, src, a.keep, seed) for b in backends}
        outs = {(rc, out.replace("\r", "")) for rc, out in results.values()}
        if len(outs) == 1:
            os.remove(src)
            continue
        fails += 1
        print(f"seed {seed}: backends disagree -> {src}")
        for b, (rc, out) in results.items():
            with open(os.path.join(a.keep, f"{seed}.{b}.out"), "w") as f:
                f.write(f"exit: {rc}\n{out}")
    print(f"{a.n} programs, {fails} disagreements")
    sys.exit(1 if fails else 0)


if __name__ == "__main__":
    main()
