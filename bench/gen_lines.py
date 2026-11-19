#!/usr/bin/env python3
"""Generate one program of about N lines in each language (roadmap BM6).

    python bench/gen_lines.py <lines> <out_dir>
    -> <out_dir>/prog.fd prog.rs prog.go prog.cpp prog.zig

The same program in Fides, Rust, Go, C++ and Zig: K functions of about 15
lines each (integer arithmetic, a counted loop, an if/else, a call), in
chains of 100 (each function calls the next of its chain, so the call depth
stays at 100), and a main that calls the first of every chain and prints a
checksum. Every function is reachable from main, so no compiler can skip
one. Every language prints the same number (bench/compile-speed.sh checks).
The functions are w0, w1, ...: `f32`, `f64`, `f80`, `f128` are type names in
Fides and Zig, so the first recorded run (Nov 16) failed at 10k lines when
they were f0, f1, ...

python bench/gen_lines.py --touch <out_dir> changes one line of each
program (the constant of the first function) for the one-line-change
build; the output changes too, the same in every language.
"""
import os
import sys

M = 1000003
CHAIN = 100


def fides(k, last):
    tail = "return a;" if last else f"return (a + w{k + 1}(a % 1000)) % {M};"
    return f"""fn w{k}(x: i64) -> i64 {{
    var a: i64 = x + {k % 1000};
    var i: i64 = 0;
    while i < 3 {{
        a = (a * 31 + i) % {M};
        i = i + 1;
    }}
    if a % 2 == 0 {{
        a = a + 7;
    }} else {{
        a = a * 3 % {M};
    }}
    {tail}
}}
"""


def rust(k, last):
    tail = "a" if last else f"(a + w{k + 1}(a % 1000)) % {M}"
    return f"""fn w{k}(x: i64) -> i64 {{
    let mut a: i64 = x + {k % 1000};
    let mut i: i64 = 0;
    while i < 3 {{
        a = (a * 31 + i) % {M};
        i += 1;
    }}
    if a % 2 == 0 {{
        a += 7;
    }} else {{
        a = a * 3 % {M};
    }}
    {tail}
}}
"""


def go(k, last):
    tail = "return a" if last else f"return (a + w{k + 1}(a%1000)) % {M}"
    return f"""func w{k}(x int64) int64 {{
	a := x + {k % 1000}
	var i int64 = 0
	for i < 3 {{
		a = (a*31 + i) % {M}
		i++
	}}
	if a%2 == 0 {{
		a += 7
	}} else {{
		a = a * 3 % {M}
	}}
	{tail}
}}
"""


def cpp(k, last):
    tail = "return a;" if last else f"return (a + w{k + 1}(a % 1000)) % {M};"
    return f"""long long w{k}(long long x) {{
    long long a = x + {k % 1000};
    long long i = 0;
    while (i < 3) {{
        a = (a * 31 + i) % {M};
        i += 1;
    }}
    if (a % 2 == 0) {{
        a += 7;
    }} else {{
        a = a * 3 % {M};
    }}
    {tail}
}}
"""


def zig(k, last):
    # Zig has no % on signed integers: @mod (the values are never negative)
    tail = "return a;" if last else f"return @mod(a + w{k + 1}(@mod(a, 1000)), {M});"
    return f"""fn w{k}(x: i64) i64 {{
    var a: i64 = x + {k % 1000};
    var i: i64 = 0;
    while (i < 3) : (i += 1) {{
        a = @mod(a * 31 + i, {M});
    }}
    if (@mod(a, 2) == 0) {{
        a += 7;
    }} else {{
        a = @mod(a * 3, {M});
    }}
    {tail}
}}
"""


def write(n, out):
    k_total = max(1, (n - 10) // 15)
    os.makedirs(out, exist_ok=True)
    firsts = list(range(0, k_total, CHAIN))
    parts = {"fd": [], "rs": [], "go": ["package main", "", 'import "fmt"', ""],
             "cpp": ["#include <cstdio>", ""] +
                    [f"long long w{k}(long long x);" for k in range(k_total)] + [""],
             "zig": ['const std = @import("std");', ""]}
    for k in range(k_total):
        last = (k + 1) % CHAIN == 0 or k + 1 == k_total
        parts["fd"].append(fides(k, last))
        parts["rs"].append(rust(k, last))
        parts["go"].append(go(k, last))
        parts["cpp"].append(cpp(k, last))
        parts["zig"].append(zig(k, last))
    calls = [(f, j % 9 + 1) for j, f in enumerate(firsts)]
    parts["fd"] += ["fn main() -> i64 {", "    var s: i64 = 0;"] + \
        [f"    s = (s + w{f}({x})) % {M};" for f, x in calls] + ["    println(s);", "    return 0;", "}"]
    parts["rs"] += ["fn main() {", "    let mut s: i64 = 0;"] + \
        [f"    s = (s + w{f}({x})) % {M};" for f, x in calls] + ['    println!("{}", s);', "}"]
    parts["go"] += ["func main() {", "\tvar s int64 = 0"] + \
        [f"\ts = (s + w{f}({x})) % {M}" for f, x in calls] + ["\tfmt.Println(s)", "}"]
    parts["cpp"] += ["int main() {", "    long long s = 0;"] + \
        [f"    s = (s + w{f}({x})) % {M};" for f, x in calls] + ['    std::printf("%lld\\n", s);', "}"]
    parts["zig"] += ["pub fn main() void {", "    var s: i64 = 0;"] + \
        [f"    s = @mod(s + w{f}({x}), {M});" for f, x in calls] + ['    std.debug.print("{d}\\n", .{s});', "}"]
    for ext, lines in parts.items():
        with open(os.path.join(out, "prog." + ext), "w", newline="\n") as f:
            f.write("\n".join(lines) + "\n")
    print(f"{k_total} functions, {n} lines asked; prog.fd has "
          f"{sum(p.count(chr(10)) + 1 for p in parts['fd'])} lines")


def touch(out):
    """Change the constant of w0 (`x + 0`) to `x + 1` in every language."""
    for ext, old, new in (("fd", "var a: i64 = x + 0;", "var a: i64 = x + 1;"),
                          ("rs", "let mut a: i64 = x + 0;", "let mut a: i64 = x + 1;"),
                          ("go", "a := x + 0\n", "a := x + 1\n"),
                          ("cpp", "long long a = x + 0;", "long long a = x + 1;"),
                          ("zig", "var a: i64 = x + 0;", "var a: i64 = x + 1;")):
        p = os.path.join(out, "prog." + ext)
        with open(p, encoding="utf-8", newline="") as f:
            src = f.read()
        if src.count(old) < 1:
            sys.exit(f"{p}: the line to change is not there")
        with open(p, "w", encoding="utf-8", newline="") as f:
            f.write(src.replace(old, new, 1))


if __name__ == "__main__":
    if len(sys.argv) == 3 and sys.argv[1] == "--touch":
        touch(sys.argv[2])
    elif len(sys.argv) == 3:
        write(int(sys.argv[1]), sys.argv[2])
    else:
        sys.exit(__doc__)
