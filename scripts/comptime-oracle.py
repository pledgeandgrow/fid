#!/usr/bin/env python3
"""Comptime oracle: what `comptime` computes is what `run` computes.

    python scripts/comptime-oracle.py [--fid release/fid.exe] [--seed N]
                                      [--cases N] [--keep]

Generates a seeded program of `comptime fn`s (arithmetic, loops, strings,
tables — the evaluator's whole surface) and a main that, for every case,
compares the compile-time evaluation (`comptime (f(k))`) with the same
function at run time (`f(a)`, a copy of the literal the aggressive pass
cannot fold). The program prints `P/N` and exits with the failure count;
the oracle fails unless it printed `N/N` and exited 0.

The same pattern as scripts/enc-oracle.py: a checked theorem (comptime
result == run result), not a hope. Differences are comptime_eval.fd bugs.
"""
import argparse
import os
import random
import subprocess
import sys
import tempfile


def gen_body(rng, idx):
    kind = idx % 5
    a, b, c = rng.randint(2, 40), rng.randint(2, 12), rng.randint(1, 9)
    if kind == 0:  # polynomial with checked arithmetic
        return f"""comptime fn ct_f{idx}(n: i64) -> i64 {{
    return n * {a} + {b} * (n - {c});
}}"""
    if kind == 1:  # while loop accumulating (OP28)
        return f"""comptime fn ct_f{idx}(n: i64) -> i64 {{
    var s: i64 = 0;
    var i: i64 = 0;
    while i < n {{
        s = s + i * {a} - {c};
        i = i + 1;
    }}
    return s;
}}"""
    if kind == 2:  # nested ifs and shifts
        return f"""comptime fn ct_f{idx}(n: i64) -> i64 {{
    var v: i64 = n;
    if (v & 1) != 0 {{
        v = (v << {min(b, 4)}) ^ {a};
    }} else {{
        v = (v >> 1) + {c};
    }}
    if v > {a * 30} {{
        v = v - {a * 20};
    }}
    return v;
}}"""
    if kind == 3:  # string building (pure core string functions)
        return f"""comptime fn ct_f{idx}(n: i64) -> i64 {{
    var s: str = "x";
    var i: i64 = 0;
    while i < n {{
        s = s + "ab";
        i = i + 1;
    }}
    return str_len(s) + str_find(s, "ba");
}}"""
    # kind == 4: a comptime table (OP28)
    return f"""comptime fn ct_f{idx}(n: i64) -> i64 {{
    var t: Vec<i64> = Vec::new();
    var i: i64 = 0;
    while i < n {{
        t.push(i * {a} + {c});
        i = i + 1;
    }}
    var s: i64 = 0;
    var j: i64 = 0;
    while j < t.len() {{
        s = s + t.get(j);
        j = j + 1;
    }}
    return s;
}}"""


def gen_program(rng, cases):
    fns = "\n\n".join(gen_body(rng, i) for i in range(cases))
    checks = []
    for i in range(cases):
        k = rng.randint(0, 30)
        # a is a runtime copy of the literal: the aggressive comptime pass
        # only folds calls whose arguments are all literals, so f(a) runs
        checks.append(
            f"    val a{i}: i64 = {k};\n"
            f"    if comptime (ct_f{i}({k})) != ct_f{i}(a{i}) {{\n"
            f'        println("FAIL ct_f{i}: comptime " + int_to_str(comptime (ct_f{i}({k})))'
            f' + " run " + int_to_str(ct_f{i}(a{i})));\n'
            f"        fails = fails + 1;\n"
            f"    }}"
        )
    return f"""// comptime-oracle.py generated (seeded): comptime == run
{fns}

fn main() -> i64 {{
    var fails: i64 = 0;
{chr(10).join(checks)}
    println(int_to_str({cases} - fails) + "/" + int_to_str({cases}));
    return fails;
}}
"""


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--fid", default="release/fid.exe")
    ap.add_argument("--seed", type=int, default=1)
    ap.add_argument("--cases", type=int, default=40)
    ap.add_argument("--keep", action="store_true")
    args = ap.parse_args()

    rng = random.Random(args.seed)
    src = gen_program(rng, args.cases)
    path = os.path.join(tempfile.gettempdir(), f"comptime-oracle-{args.seed}.fd")
    with open(path, "w", encoding="utf-8", newline="\n") as f:
        f.write(src)
    if args.keep:
        print(path)

    r = subprocess.run([args.fid, "run", path], capture_output=True, text=True,
                       timeout=600)
    out = [l for l in r.stdout.splitlines() if l.strip()]
    fails = [l for l in out if l.startswith("FAIL")]
    last = out[-1] if out else ""
    ok = r.returncode == 0 and last == f"{args.cases}/{args.cases}"
    for l in fails:
        print(l)
    print(f"{'ok' if ok else 'FAILED'}: comptime == run on {last or 'no output'} "
          f"(seed {args.seed}, exit {r.returncode})")
    if not ok and r.returncode != 0:
        err = [l for l in r.stderr.splitlines() if "error" in l.lower()][:5]
        for l in err:
            print(l)
    if not args.keep:
        try:
            os.remove(path)
        except OSError:
            pass
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
