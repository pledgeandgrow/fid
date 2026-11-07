#!/usr/bin/env python3
"""Random Fides programs, each with an equivalent C program (roadmap VF2).

    python scripts/fuzz/gen_vs_c.py [-n 100] [--seed 1] [--fid release/fid.exe]
                                    [--backends run,ccompile,emit]
                                    [--cc clang,gcc] [--keep build/fuzz-c]
    python scripts/fuzz/gen_vs_c.py --print <seed>      # show one pair

Each seed gives a Fides program and a C program with the same meaning,
written from one random syntax tree. The C program is the oracle: it is
built with Clang (`zig cc`, at -O0 and -O2) and, when WSL has it, GCC (-O0
and -O2, run in WSL), and every C build must agree with the others before
it is trusted. Each Fides backend must then print the same text and exit
with the same status.

What the programs use (the reference's rules, docs/reference/reference.md):

- the eight integer types i8 … u64; `+ - *` and negation panic on overflow
  at the declared width (§3.5), except in `@wrapping` functions where they
  wrap; `/` and `%` by zero panic, and so does MIN / -1 (and MIN % -1);
  unsigned types divide, compare, shift and print unsigned;
- `as` between integer types: truncation and sign extension, as in C;
- `<<` keeps the low bits of the declared width; `>>` is arithmetic on
  signed types; the shift amount is masked to the width in the source;
- the precedence table of §4.1: expressions are printed with the fewest
  parentheses Fides needs (C gets them all), so a precedence bug shows;
- if/else, while with break/continue, for over a range, match on integers
  with literal and range patterns, `if` used as a value;
- structs of integers: literals, field reads and writes, copies (values,
  §8.1), passing by value, `==`;
- Vec<i64>: push, len, get and indexing (out of bounds panics, §8.2);
- helper functions that call earlier helpers (no recursion).

A panic is compared as its exit status 1 (the message goes to stderr and is
not compared); everything printed before it must match. Failures are kept in
--keep as <seed>.fd, <seed>.c and each backend's output.
"""
import argparse
import os
import random
import shutil
import subprocess
import sys

TYPES = ["i8", "i16", "i32", "i64", "u8", "u16", "u32", "u64"]
BITS = {"i8": 8, "i16": 16, "i32": 32, "i64": 64, "u8": 8, "u16": 16, "u32": 32, "u64": 64}
CTY = {t: ("int" if t[0] == "i" else "uint") + str(BITS[t]) + "_t" for t in TYPES}


def signed(t):
    return t[0] == "i"


# Precedence levels of §4.1 (higher binds tighter)
PREC = {"||": 4, "|": 5, "^": 6, "&": 7, "<<": 8, ">>": 8, "&&": 9,
        "==": 10, "!=": 10, "<": 11, ">": 11, "<=": 11, ">=": 11,
        "+": 12, "-": 12, "*": 13, "/": 13, "%": 13}
UNARY = 14
POSTFIX = 15
ATOM = 16


class E:
    """An expression: Fides text with its precedence, and C text."""
    def __init__(self, fd, prec, c, lit=False):
        self.fd, self.prec, self.c, self.lit = fd, prec, c, lit


def wrap(e, need):
    return e.fd if e.prec >= need else "(" + e.fd + ")"


# Known bugs the generator can step around, so they do not hide new ones
# (--avoid; docs/verification/miscompile-log.md has each one).
KNOWN = {
    "same-width-cast": "M1: `u8 as i8` (same width, other signedness) is a BitCast that "
                       "`run` and `emit` treat as identity",
    "nested-if-value": "M2: an `if` value whose branch value is another `if` is typed void",
    "println-u64": "M3: println of a u64 above i64::MAX prints it signed",
}
AVOID = set(KNOWN)


def cast_ok(u, t):
    if "same-width-cast" not in AVOID:
        return True
    return u == t or BITS[u] != BITS[t] or BITS[t] == 64


class Gen:
    def __init__(self, rng):
        self.r = rng
        self.structs = []      # (name, [(field, type)])
        self.helpers = []      # (name, [(param, type)], ret type, wrapping)
        self.wrapping = False
        self.counter = 0
        self.used_ops = set()

    def fresh(self, p):
        self.counter += 1
        return f"{p}{self.counter}"

    # ---- expressions ---------------------------------------------------
    def lit(self, t):
        v = self.r.choice([0, 1, 2, 3, 5, 7, 9, 15, 16, 31, 63, 64, 100, 127])
        return E(str(v), ATOM, f"(({CTY[t]}){v})", lit=True)

    def leaf(self, t, sc):
        vs = [n for n, ty in sc.ints() if ty == t]
        fs = [(s, f) for s, sname in sc.structs() for f, fty in self.struct_fields(sname) if fty == t]
        k = self.r.random()
        if vs and k < 0.55:
            v = self.r.choice(vs)
            return E(v, ATOM, v)
        if fs and k < 0.7:
            s, f = self.r.choice(fs)
            return E(f"{s}.{f}", POSTFIX, f"{s}.{f}")
        if t == "i64" and sc.has_vec() and k < 0.75:
            return E("xs.len()", POSTFIX, "vlen(&xs)")
        others = [(v, u) for v, u in sc.ints() if cast_ok(u, t)]
        if others and k < 0.85:
            v, u = self.r.choice(others)
            return E(f"{v} as {t}", POSTFIX, f"(({CTY[t]}){v})")
        return self.lit(t)

    def struct_fields(self, sname):
        for n, fs in self.structs:
            if n == sname:
                return fs
        return []

    def expr(self, t, sc, d):
        r = self.r
        if d <= 0 or r.random() < 0.25:
            return self.leaf(t, sc)
        k = r.random()
        if k < 0.45:
            return self.binop(t, sc, d)
        if k < 0.55:
            u = r.choice([u for u in TYPES if cast_ok(u, t)])
            a = self.expr(u, sc, d - 1)
            if a.lit:
                a = E(f"({a.fd} as {u})", ATOM, a.c)
            return E(f"{wrap(a, POSTFIX)} as {t}", POSTFIX, f"(({CTY[t]})({a.c}))")
        if k < 0.62:
            a = self.expr(t, sc, d - 1)
            if a.lit:
                return a
            if signed(t) and r.random() < 0.5:
                fn = f"wneg_{t}" if self.wrapping else f"neg_{t}"
                self.used_ops.add(("neg", t))
                return E("-" + wrap(a, UNARY), UNARY, f"{fn}({a.c})")
            return E("~" + wrap(a, UNARY), UNARY, f"(({CTY[t]})~({a.c}))")
        if k < 0.70 and any(h[2] == t for h in self.helpers):
            h = r.choice([h for h in self.helpers if h[2] == t])
            args = []
            cargs = []
            for _, pt in h[1]:
                if pt in TYPES:
                    a = self.expr(pt, sc, d - 1)
                    args.append(a.fd)
                    cargs.append(a.c)
                else:
                    ss = [s for s, sn in sc.structs() if sn == pt]
                    if not ss:
                        return self.leaf(t, sc)
                    s = r.choice(ss)
                    args.append(s)
                    cargs.append(s)
            return E(f"{h[0]}({', '.join(args)})", POSTFIX, f"{h[0]}({', '.join(cargs)})")
        if k < 0.76 and t == "i64" and sc.has_vec():
            i = self.expr("i64", sc, d - 1)
            if r.random() < 0.9:
                # in range: main starts with four elements and only pushes
                i = E(f"({wrap(i, PREC['&'] + 1)} & 3)", ATOM, f"(({i.c}) & 3)")
            if r.random() < 0.5:
                return E(f"xs.get({i.fd})", POSTFIX, f"vget(&xs, {i.c})")
            return E(f"xs[{i.fd}]", POSTFIX, f"vget(&xs, {i.c})")
        if k < 0.84:
            c = self.cond(sc, d - 1)
            a = self.expr(t, sc, d - 1)
            b = self.expr(t, sc, d - 1)
            if a.lit:
                a = E(f"({a.fd} as {t})", ATOM, a.c)
            if b.lit:
                b = E(f"({b.fd} as {t})", ATOM, b.c)
            if "nested-if-value" in AVOID:
                a = E(wrap(a, 1), ATOM, a.c)
                b = E(wrap(b, 1), ATOM, b.c)
            # prec 0: parenthesised wherever it is an operand
            return E(f"if {c.fd} {{ {a.fd} }} else {{ {b.fd} }}", 0,
                     f"(({c.c}) ? ({a.c}) : ({b.c}))")
        return self.binop(t, sc, d)

    def binop(self, t, sc, d):
        r = self.r
        op = r.choice(["+", "-", "*", "/", "%", "&", "|", "^", "<<", ">>", "+", "*", "-"])
        a = self.expr(t, sc, d - 1)
        b = self.expr(t, sc, d - 1)
        if a.lit and b.lit:
            a = E(f"({a.fd} as {t})", ATOM, a.c)
        bits = BITS[t]
        if op in ("<<", ">>"):
            # the shift amount is masked to the width in the source
            m = bits - 1
            b = E(f"({wrap(b, PREC['&'] + 1)} & {m})", ATOM, f"(({CTY[t]})(({b.c}) & {m}))")
            sfx = "shl" if op == "<<" else "shr"
            c = f"{sfx}_{t}({a.c}, {b.c})"
            self.used_ops.add((sfx, t))
        elif op in ("/", "%") and (self.wrapping or r.random() < 0.85):
            # mostly a divisor in 1..15: no division by zero, no MIN / -1
            b = E(f"(({wrap(b, PREC['&'] + 1)} & 15) | 1)", ATOM, f"(({CTY[t]})((({b.c}) & 15) | 1))")
            name = "div" if op == "/" else "rem"
            c = f"{name}_{t}({a.c}, {b.c})"
            self.used_ops.add((name, t))
        else:
            name = {"+": "add", "-": "sub", "*": "mul", "/": "div", "%": "rem",
                    "&": "and", "|": "or", "^": "xor"}[op]
            if self.wrapping and name in ("add", "sub", "mul"):
                name = "w" + name
            c = f"{name}_{t}({a.c}, {b.c})"
            self.used_ops.add((name, t))
        p = PREC[op]
        fd = f"{wrap(a, p)} {op} {wrap(b, p + 1)}"
        return E(fd, p, c)

    def cond(self, sc, d):
        r = self.r
        k = r.random()
        if d > 0 and k < 0.2:
            a = self.cond(sc, d - 1)
            b = self.cond(sc, d - 1)
            op = r.choice(["&&", "||"])
            p = PREC[op]
            return E(f"{wrap(a, p)} {op} {wrap(b, p + 1)}", p, f"(({a.c}) {op} ({b.c}))")
        if d > 0 and k < 0.27:
            a = self.cond(sc, d - 1)
            return E("!" + wrap(a, UNARY), UNARY, f"(!({a.c}))")
        if k < 0.33 and self.structs and len(sc.structs()) >= 2:
            ss = sc.structs()
            s1, n1 = r.choice(ss)
            same = [s for s, n in ss if n == n1 and s != s1]
            if same:
                s2 = r.choice(same)
                op = r.choice(["==", "!="])
                fs = self.struct_fields(n1)
                eq = " && ".join(f"({s1}.{f} == {s2}.{f})" for f, _ in fs)
                c = f"({eq})" if op == "==" else f"(!({eq}))"
                return E(f"{s1} {op} {s2}", PREC[op], c)
        t = r.choice(TYPES)
        a = self.expr(t, sc, max(0, d - 1))
        b = self.expr(t, sc, max(0, d - 1))
        if a.lit and b.lit:
            a = E(f"({a.fd} as {t})", ATOM, a.c)
        op = r.choice(["<", ">", "<=", ">=", "==", "!="])
        p = PREC[op]
        return E(f"{wrap(a, p + 1)} {op} {wrap(b, p + 1)}", p, f"(({a.c}) {op} ({b.c}))")

    def print_stmt(self, n, t, pad):
        if t == "u64" and "println-u64" in AVOID:
            fd = f'{pad}print({n}); print("\\n");'
        else:
            fd = f"{pad}println({n});"
        c = (f"{pad}printf(\"%{'lld' if signed(t) else 'llu'}\\n\", "
             f"({'long long' if signed(t) else 'unsigned long long'}){n});")
        return fd, c

    # ---- statements ----------------------------------------------------
    def block(self, sc, depth, ind, nstmts, in_loop=False):
        r = self.r
        fd, c = [], []
        pad = "    " * ind
        for _ in range(nstmts):
            k = r.random()
            if k < 0.22 or not sc.ints():
                t = r.choice(TYPES)
                e = self.expr(t, sc, 3)
                n = self.fresh("v")
                kw = "var" if r.random() < 0.8 else "val"
                fd.append(f"{pad}{kw} {n}: {t} = {e.fd};")
                c.append(f"{pad}{CTY[t]} {n} = {e.c};")
                sc.add(n, t, kw == "var")
            elif k < 0.40:
                vs = sc.mutable_ints()
                if not vs:
                    continue
                n, t = r.choice(vs)
                e = self.expr(t, sc, 3)
                if r.random() < 0.3:
                    op = r.choice(["+", "-", "*", "&", "|", "^"])
                    name = {"+": "add", "-": "sub", "*": "mul", "&": "and", "|": "or", "^": "xor"}[op]
                    if self.wrapping and name in ("add", "sub", "mul"):
                        name = "w" + name
                    self.used_ops.add((name, t))
                    fd.append(f"{pad}{n} {op}= {e.fd};")
                    c.append(f"{pad}{n} = {name}_{t}({n}, {e.c});")
                else:
                    fd.append(f"{pad}{n} = {e.fd};")
                    c.append(f"{pad}{n} = {e.c};")
            elif k < 0.50:
                ints = sc.ints()
                n, t = r.choice(ints)
                f1, c1 = self.print_stmt(n, t, pad)
                fd.append(f1)
                c.append(c1)
            elif k < 0.60 and depth > 0:
                cnd = self.cond(sc, 2)
                f1, c1 = self.block(sc.child(), depth - 1, ind + 1, r.randint(1, 3), in_loop)
                f2, c2 = self.block(sc.child(), depth - 1, ind + 1, r.randint(1, 3), in_loop)
                fd += [f"{pad}if {cnd.fd} {{"] + f1 + [f"{pad}}} else {{"] + f2 + [f"{pad}}}"]
                c += [f"{pad}if ({cnd.c}) {{"] + c1 + [f"{pad}}} else {{"] + c2 + [f"{pad}}}"]
            elif k < 0.68 and depth > 0:
                i = self.fresh("k")
                n = r.randint(1, 6)
                inner = sc.child()
                inner.add(i, "i64", False)
                f1, c1 = self.block(inner, depth - 1, ind + 1, r.randint(1, 4), True)
                if r.random() < 0.5:
                    fd += [f"{pad}for {i} in 0..{n} {{"] + f1 + [f"{pad}}}"]
                    c += [f"{pad}for (int64_t {i} = 0; {i} < {n}; {i}++) {{"] + c1 + [f"{pad}}}"]
                else:
                    fd += [f"{pad}var {i}: i64 = 0;", f"{pad}while {i} < {n} {{",
                           f"{pad}    {i} = {i} + 1;"] + f1 + [f"{pad}}}"]
                    c += [f"{pad}int64_t {i} = 0;", f"{pad}while ({i} < {n}) {{",
                          f"{pad}    {i} = {i} + 1;"] + c1 + [f"{pad}}}"]
            elif k < 0.72 and in_loop:
                cnd = self.cond(sc, 1)
                w = r.choice(["break", "continue"])
                fd.append(f"{pad}if {cnd.fd} {{ {w}; }}")
                c.append(f"{pad}if ({cnd.c}) {{ {w}; }}")
            elif k < 0.78 and depth > 0:
                t = r.choice(TYPES)
                e = self.expr(t, sc, 2)
                fd.append(f"{pad}match {wrap(e, POSTFIX) if e.prec < ATOM else e.fd} {{")
                tmp = self.fresh("m")
                c.append(f"{pad}{{ {CTY[t]} {tmp} = {e.c};")
                arms = []
                used = set()
                for _ in range(r.randint(1, 3)):
                    lo = r.randint(0, 9)
                    if lo in used:
                        continue
                    if r.random() < 0.4:
                        hi = lo + r.randint(1, 5)
                        if any(lo <= u <= hi for u in used):
                            continue
                        used.update(range(lo, hi + 1))
                        arms.append((f"{lo}..={hi}", f"({tmp} >= {lo} && {tmp} <= {hi})"))
                    else:
                        used.add(lo)
                        arms.append((str(lo), f"({tmp} == {lo})"))
                first = True
                for pat, test in arms:
                    f1, c1 = self.block(sc.child(), depth - 1, ind + 1, r.randint(1, 2), in_loop)
                    fd += [f"{pad}    {pat} => {{"] + ["    " + l for l in f1] + [f"{pad}    }}"]
                    c += [f"{pad}  {'if' if first else 'else if'} {test} {{"] + c1 + [f"{pad}  }}"]
                    first = False
                f1, c1 = self.block(sc.child(), depth - 1, ind + 1, r.randint(1, 2), in_loop)
                fd += [f"{pad}    _ => {{"] + ["    " + l for l in f1] + [f"{pad}    }}", f"{pad}}}"]
                if first:
                    c += [f"{pad}  {{"] + c1 + [f"{pad}  }}", f"{pad}}}"]
                else:
                    c += [f"{pad}  else {{"] + c1 + [f"{pad}  }}", f"{pad}}}"]
            elif k < 0.84 and self.structs:
                sname, fs = r.choice(self.structs)
                existing = [s for s, n in sc.structs() if n == sname]
                n = self.fresh("s")
                if existing and r.random() < 0.5:
                    src = r.choice(existing)
                    fd.append(f"{pad}var {n}: {sname} = {src};")
                    c.append(f"{pad}{sname} {n} = {src};")
                else:
                    inits = [(f, self.expr(ft, sc, 2)) for f, ft in fs]
                    fd.append(f"{pad}var {n}: {sname} = {sname} {{ " +
                              ", ".join(f"{f}: {e.fd}" for f, e in inits) + " };")
                    c.append(f"{pad}{sname} {n} = {{ " + ", ".join(e.c for _, e in inits) + " };")
                sc.add_struct(n, sname)
            elif k < 0.90 and sc.structs():
                s, sname = r.choice(sc.structs())
                if not sc.is_mutable(s):
                    continue
                f, ft = r.choice(self.struct_fields(sname))
                e = self.expr(ft, sc, 2)
                fd.append(f"{pad}{s}.{f} = {e.fd};")
                c.append(f"{pad}{s}.{f} = {e.c};")
            elif k < 0.95 and sc.has_vec():
                e = self.expr("i64", sc, 2)
                fd.append(f"{pad}xs.push({e.fd});")
                c.append(f"{pad}vpush(&xs, {e.c});")
            else:
                ints = sc.ints()
                n, t = r.choice(ints)
                f1, c1 = self.print_stmt(n, t, pad)
                fd.append(f1)
                c.append(c1)
        return fd, c

    # ---- items ---------------------------------------------------------
    def program(self):
        r = self.r
        fd, c = [], []
        for si in range(r.randint(0, 2)):
            name = f"S{si}"
            fs = [(f"f{j}", r.choice(TYPES)) for j in range(r.randint(1, 3))]
            self.structs.append((name, fs))
            fd += [f"struct {name} {{"] + [f"    {f}: {t}," for f, t in fs] + ["}", ""]
            c += [f"typedef struct {{ " + " ".join(f"{CTY[t]} {f};" for f, t in fs) + f" }} {name};"]
        protos = []
        for hi in range(r.randint(0, 4)):
            name = f"h{hi}"
            params = [(f"p{j}", r.choice(TYPES)) for j in range(r.randint(1, 3))]
            if self.structs and r.random() < 0.3:
                params.append(("q", r.choice(self.structs)[0]))
            ret = r.choice(TYPES)
            self.wrapping = r.random() < 0.3
            sc = Scope()
            for p, t in params:
                if t in TYPES:
                    sc.add(p, t, False)
                else:
                    sc.add_struct(p, t, mutable=False)
            body_fd, body_c = self.block(sc, 2, 1, r.randint(1, 5))
            # helpers do not print: their calls may sit anywhere in an expression
            body_fd = [l for l in body_fd if "print" not in l]
            body_c = [l for l in body_c if "printf" not in l]
            e = self.expr(ret, sc, 3)
            ps = ", ".join(f"{p}: {t}" for p, t in params)
            cps = ", ".join(f"{CTY.get(t, t)} {p}" for p, t in params)
            if self.wrapping:
                fd.append("@wrapping")
            fd += [f"fn {name}({ps}) -> {ret} {{"] + body_fd + [f"    return {e.fd};", "}", ""]
            c += [f"static {CTY[ret]} {name}({cps}) {{"] + body_c + [f"    return {e.c};", "}"]
            self.helpers.append((name, params, ret, self.wrapping))
            self.wrapping = False
        sc = Scope()
        sc.vec = True
        body_fd, body_c = self.block(sc, 3, 1, r.randint(4, 12))
        fd += ["fn main() -> i64 {", "    var xs: Vec<i64> = Vec::new();"]
        fd += [f"    xs.push({k * 7 + 1});" for k in range(4)] + body_fd
        fd += ["    println(xs.len());", "    return 0;", "}"]
        c += ["int main(void) {", "    Vec xs = {0, 0, 0};"]
        c += [f"    vpush(&xs, {k * 7 + 1});" for k in range(4)] + body_c
        c += ["    printf(\"%lld\\n\", (long long)xs.len);", "    return 0;", "}"]
        return "\n".join(fd) + "\n", self.c_prelude() + "\n".join(c) + "\n"

    def c_prelude(self):
        out = ["#include <stdio.h>", "#include <stdint.h>", "#include <stdlib.h>", "",
               "static void panic(void) { fflush(stdout); exit(1); }",
               "typedef struct { int64_t* d; int64_t len, cap; } Vec;",
               "static void vpush(Vec* v, int64_t x) { if (v->len == v->cap) { v->cap = v->cap ? 2 * v->cap : 8;"
               " v->d = realloc(v->d, v->cap * sizeof(int64_t)); } v->d[v->len++] = x; }",
               "static int64_t vget(Vec* v, int64_t i) { if (i < 0 || i >= v->len) panic(); return v->d[i]; }",
               "static int64_t vlen(Vec* v) { return v->len; }"]
        for t in TYPES:
            ct = CTY[t]
            ut = "uint64_t"
            mn = {8: "INT8_MIN", 16: "INT16_MIN", 32: "INT32_MIN", 64: "INT64_MIN"}[BITS[t]]
            out += [
                f"static inline {ct} add_{t}({ct} a, {ct} b) {{ {ct} r; if (__builtin_add_overflow(a, b, &r)) panic(); return r; }}",
                f"static inline {ct} sub_{t}({ct} a, {ct} b) {{ {ct} r; if (__builtin_sub_overflow(a, b, &r)) panic(); return r; }}",
                f"static inline {ct} mul_{t}({ct} a, {ct} b) {{ {ct} r; if (__builtin_mul_overflow(a, b, &r)) panic(); return r; }}",
                f"static inline {ct} wadd_{t}({ct} a, {ct} b) {{ return ({ct})(({ut})a + ({ut})b); }}",
                f"static inline {ct} wsub_{t}({ct} a, {ct} b) {{ return ({ct})(({ut})a - ({ut})b); }}",
                f"static inline {ct} wmul_{t}({ct} a, {ct} b) {{ return ({ct})(({ut})a * ({ut})b); }}",
                f"static inline {ct} and_{t}({ct} a, {ct} b) {{ return ({ct})(a & b); }}",
                f"static inline {ct} or_{t}({ct} a, {ct} b) {{ return ({ct})(a | b); }}",
                f"static inline {ct} xor_{t}({ct} a, {ct} b) {{ return ({ct})(a ^ b); }}",
                f"static inline {ct} shl_{t}({ct} a, {ct} s) {{ return ({ct})(({ut})a << s); }}",
                f"static inline {ct} shr_{t}({ct} a, {ct} s) {{ return ({ct})(a >> s); }}",
            ]
            if signed(t):
                out += [
                    f"static inline {ct} div_{t}({ct} a, {ct} b) {{ if (b == 0 || (a == {mn} && b == -1)) panic(); return ({ct})(a / b); }}",
                    f"static inline {ct} rem_{t}({ct} a, {ct} b) {{ if (b == 0 || (a == {mn} && b == -1)) panic(); return ({ct})(a % b); }}",
                    f"static inline {ct} neg_{t}({ct} a) {{ if (a == {mn}) panic(); return ({ct})(-a); }}",
                    f"static inline {ct} wneg_{t}({ct} a) {{ return ({ct})(0 - ({ut})a); }}",
                ]
            else:
                out += [
                    f"static inline {ct} div_{t}({ct} a, {ct} b) {{ if (b == 0) panic(); return ({ct})(a / b); }}",
                    f"static inline {ct} rem_{t}({ct} a, {ct} b) {{ if (b == 0) panic(); return ({ct})(a % b); }}",
                ]
        return "\n".join(out) + "\n\n"


class Scope:
    def __init__(self, parent=None):
        self.parent = parent
        self.vars = []          # (name, type, mutable)
        self.svars = []         # (name, struct, mutable)
        self.vec = parent.vec if parent else False

    def child(self):
        return Scope(self)

    def add(self, n, t, mutable):
        self.vars.append((n, t, mutable))

    def add_struct(self, n, s, mutable=True):
        self.svars.append((n, s, mutable))

    def all_vars(self):
        return (self.parent.all_vars() if self.parent else []) + self.vars

    def all_svars(self):
        return (self.parent.all_svars() if self.parent else []) + self.svars

    def ints(self):
        return [(n, t) for n, t, _ in self.all_vars()]

    def mutable_ints(self):
        return [(n, t) for n, t, m in self.all_vars() if m]

    def structs(self):
        return [(n, s) for n, s, _ in self.all_svars()]

    def is_mutable(self, n):
        return any(m for x, _, m in self.all_svars() if x == n)

    def has_vec(self):
        return self.vec


def pair(seed):
    return Gen(random.Random(seed)).program()


def run(cmd, timeout, cwd=None):
    try:
        p = subprocess.run(cmd, capture_output=True, text=True, timeout=timeout, cwd=cwd,
                           encoding="utf-8", errors="replace")
        return p.returncode, p.stdout.replace("\r", ""), p.stderr
    except subprocess.TimeoutExpired:
        return "timeout", "", ""


def wsl_path(p):
    p = os.path.abspath(p).replace("\\", "/")
    return "/mnt/" + p[0].lower() + p[2:]


def c_results(csrc, work, seed, ccs):
    """Build and run the C oracle with each C compiler; all must agree."""
    path = os.path.join(work, f"{seed}.c")
    with open(path, "w", newline="\n") as f:
        f.write(csrc)
    res = {}
    for cc in ccs:
        for opt in ("-O0", "-O2"):
            key = f"{cc}{opt}"
            if cc == "clang":
                exe = os.path.join(work, f"{seed}-c{opt}.exe")
                for _ in range(3):  # zig's shared cache fails now and then under load
                    rc, out, err = run(["zig", "cc", opt, "-w", "-fno-sanitize=all", path, "-o", exe], 300)
                    if rc == 0:
                        break
                if rc != 0:
                    res[key] = ("cc-failed", err[:2000])
                    continue
                rc, out, _ = run([exe], 60)
                res[key] = (rc, out)
                try:
                    os.remove(exe)
                    os.remove(exe.replace(".exe", ".pdb"))
                except OSError:
                    pass
            elif cc == "gcc":
                exe = wsl_path(os.path.join(work, f"{seed}-g{opt}"))
                rc, out, err = run(["wsl", "-d", os.environ.get("WSL_DISTRO", "kali-linux"), "-e",
                                    "sh", "-c", f"gcc {opt} -w '{wsl_path(path)}' -o '{exe}' && '{exe}'; s=$?; rm -f '{exe}'; exit $s"], 120)
                res[key] = (rc, out)
    return res


def fides_results(fid, src, work, seed, backends):
    res = {}
    for b in backends:
        exe = os.path.join(work, f"{seed}-{b}.exe")
        if b == "run":
            rc, out, err = run([fid, "run", src], 120)
            res[b] = (rc, out, err)
            continue
        rc, out, err = run([fid, b, src, "-o", exe], 300)
        if rc != 0 or not os.path.exists(exe):
            res[b] = ("build-failed", "", (out + err)[-3000:])
            continue
        rc, out, err = run([exe], 60)
        res[b] = (rc, out, err)
        for e in (exe, exe.replace(".exe", ".pdb")):
            if os.path.exists(e):
                os.remove(e)
    return res


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("-n", type=int, default=50)
    ap.add_argument("--seed", type=int, default=1)
    ap.add_argument("--fid", default=os.path.join("release", "fid.exe" if os.name == "nt" else "fid"))
    ap.add_argument("--backends", default="run,ccompile,emit")
    ap.add_argument("--cc", default="clang,gcc")
    ap.add_argument("--keep", default=os.path.join("build", "fuzz-c"))
    ap.add_argument("--print", type=int, default=None)
    ap.add_argument("--avoid", default="all",
                    help="known bugs to step around: all, none, or a comma list of " + ", ".join(KNOWN))
    a = ap.parse_args()
    AVOID.clear()
    if a.avoid == "all":
        AVOID.update(KNOWN)
    elif a.avoid != "none":
        AVOID.update(x for x in a.avoid.split(",") if x)
    os.environ.setdefault("CC", "zig cc")
    os.environ.setdefault("FIDES_NO_CACHE", "1")
    if a.print is not None:
        fd, c = pair(a.print)
        print(fd)
        print("// ---- C ----")
        print(c)
        return
    ccs = [c for c in a.cc.split(",") if c]
    if "gcc" in ccs and not shutil.which("wsl"):
        ccs.remove("gcc")
    backends = a.backends.split(",")
    os.makedirs(a.keep, exist_ok=True)
    fails = oracle_bad = 0
    for seed in range(a.seed, a.seed + a.n):
        fd, csrc = pair(seed)
        src = os.path.join(a.keep, f"{seed}.fd")
        with open(src, "w", newline="\n") as f:
            f.write(fd)
        cres = c_results(csrc, a.keep, seed, ccs)
        couts = {v for v in cres.values()}
        if len(couts) != 1 or any(v[0] == "cc-failed" for v in cres.values()):
            oracle_bad += 1
            print(f"seed {seed}: C oracle disagrees with itself: {cres}")
            continue
        want = next(iter(couts))
        fres = fides_results(a.fid, src, a.keep, seed, backends)
        bad = [b for b, (rc, out, _) in fres.items() if (rc, out) != want]
        if not bad:
            os.remove(src)
            os.remove(os.path.join(a.keep, f"{seed}.c"))
            print(f"seed {seed}: ok ({'panic' if want[0] == 1 else 'exit ' + str(want[0])}, {want[1].count(chr(10))} lines)", flush=True)
            continue
        fails += 1
        print(f"seed {seed}: differs from C on {','.join(bad)} -> {src}", flush=True)
        with open(os.path.join(a.keep, f"{seed}.want"), "w") as f:
            f.write(f"exit: {want[0]}\n{want[1]}")
        for b, (rc, out, err) in fres.items():
            with open(os.path.join(a.keep, f"{seed}.{b}.out"), "w", encoding="utf-8") as f:
                f.write(f"exit: {rc}\n{out}\n--- stderr ---\n{err[-3000:]}")
    print(f"{a.n} programs: {fails} differ from C, {oracle_bad} with a C oracle that disagrees with itself")
    sys.exit(1 if fails else 0)


if __name__ == "__main__":
    main()
