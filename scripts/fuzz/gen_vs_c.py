#!/usr/bin/env python3
"""Random Fides programs, each with an equivalent C program (roadmap VF2).

    python scripts/fuzz/gen_vs_c.py [-n 100] [--seed 1] [--fid release/fid.exe]
                                    [--backends run,ccompile,emit | all]
                                    [--cc clang,gcc] [--keep build/fuzz-c]
    python scripts/fuzz/gen_vs_c.py --print <seed>      # show one pair

Backends (own-backend roadmap RL1: every backend against the C oracle):

    run                 the interpreter
    ccompile            the C path at the default level; ccompile-O2 at -O2
    emit                the host's native backend (pe_ir on Windows)
    nat                 `emit --nat`: the machine layer on Windows x64
    emit:<target>       the machine layer for a Linux target
                        (x86_64-linux, aarch64-linux, riscv64-linux), run
                        natively, under qemu-user, or in WSL on Windows
                        (WSL_DISTRO picks the distribution)
    ccompile:<target>   the C path cross-compiled by zig cc, run the same way
    all                 every one of these this host can run

Each seed gives a Fides program and a C program with the same meaning,
written from one random syntax tree. The C program is the oracle: it is
built with Clang (`zig cc`, at -O0 and -O2) and, when WSL has it, GCC (-O0
and -O2, run in WSL; on Linux and macOS the host's gcc), and every C build must agree with the others before
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
- helper functions that call earlier helpers (no recursion);
- with --ext (Nov 16): two f64s in main (+ - * and / by a literal, `as
  f64` of signed integers, float comparisons, read back by `(f * 1000.0)
  as i64`), a str grown by `+` with literals and `int_to_str` (printed,
  and its `str_len` pushed), and `enum Sh { One(i64), Two(i64, i32), Zero }`
  built and taken apart by `match` with payload bindings;
- with --ext2 (Nov 17, implies --ext; its own random draws, so --ext seeds
  are unchanged): floats printed to a fixed precision by `ffix` (three
  decimals, rounded half up, written in Fides from `as i64`, `/`, `%` and
  `int_to_str`; C prints the same digits with "%lld.%03lld"), of fa, fb,
  fa + fb, fa - fb, a scaled product and `x as f64 / 7.0` of i8/i16/i32
  values; strings cut by `str_slice` (bounds clamped as core does), read by
  `str_byte_at`, compared with `==` to a literal and grown by an f-string
  `f"{st}<{v}>"`; and a `match` on `en` with a `_` arm.

--jobs N checks N seeds at once (each seed its own files): a long run of
1000+ programs on every backend is hours at one at a time. --log FILE
appends one line per seed (seed, verdict, backends that differ) and a
summary line, the record of a long run.

A panic is compared as its exit status 1 (the message goes to stderr and is
not compared); everything printed before it must match. Failures are kept in
--keep as <seed>.fd, <seed>.c and each backend's output.

GEN_VS_C_MUTATE=<backend> appends a line to that backend's output before the
comparison (a made-up miscompile): tests/tooling/gen-vs-c-check.sh uses it
to check that a difference is reported (exit 1).
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
    "bitnot-narrow": "M12: `~x` on u8/u16/u32 is not masked to the width in an expression "
                     "(`(~3u8) as i64` is -4); the generator masks it (`~x & 255`)",
    "for-continue": "M13: `continue` in a `for` loop jumps past the increment (the loop never "
                    "ends); a for body with a `continue` is written as the equal `while`",
    "self-assign": "M14 (Nov 16): `x = x;` makes x read as garbage on emit and --nat "
                   "(iropt promote forwards the store of a load of the same slot); the "
                   "generator leaves such a statement out",
    "for-match-break": "V1d (Nov 17): `break` in a `match` arm inside a `for` leaves only the "
                       "match, on every backend; a for body with a `break` and a `match` is "
                       "written as the equal `while`",
}
AVOID = set(KNOWN)
# --ext (Nov 16, VF2): main also keeps two f64s, a str and an enum with
# payloads (see Gen.ext_stmt). Off, no random number is drawn for it, so a
# seed gives the same pair as before and the seeds in the logs stay valid.
EXT = [False]
# --ext2 (Nov 17, VF2): --ext plus fixed-precision float printing, str_slice,
# str_byte_at, str ==, f-strings and a `_` arm (Gen.ext2_stmt). Its random
# numbers are drawn only when it is on: --ext seeds stay the same.
EXT2 = [False]


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
        self.ext = EXT[0] or EXT2[0]
        self.ext2 = EXT2[0]

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
            if "bitnot-narrow" in AVOID and not signed(t) and BITS[t] < 64:
                # the mask typed as `t`: a bare 65535 is an i32 to the checker
                # ("a i32 may not fit in u16", seeds 300, 309, 310 --ext)
                return E(f"(~{wrap(a, UNARY)} & ({(1 << BITS[t]) - 1} as {t}))", ATOM, f"(({CTY[t]})~({a.c}))")
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
            if self.ext and sc.ext and r.random() < 0.2:
                f1, c1 = self.ext_stmt(sc, depth, ind, in_loop)
                fd += f1
                c += c1
                continue
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
                elif e.fd == n and "self-assign" in AVOID:
                    pass  # M14: `n = n;` (no random number drawn, seeds stay valid)
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
                as_for = r.random() < 0.5
                if as_for and "for-continue" in AVOID and any("continue;" in ln for ln in f1):
                    as_for = False  # same meaning, same random choices after it
                if as_for and "for-match-break" in AVOID and any("break;" in ln for ln in f1) \
                        and any(ln.lstrip().startswith("match ") for ln in f1):
                    as_for = False  # V1d: the same, as a while
                if as_for:
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

    # ---- --ext: f64, str and enum Sh (main only) ------------------------
    # fa and fb stay within [-1e6, 1e6] (clamped after every write), int
    # leaves are below 2^63, a tree has at most four leaves and / only takes
    # a non-zero literal: no value is ever inf or NaN, and `(fa * 1000.0) as
    # i64` is in range. C does each float operation in its own function and
    # with FP_CONTRACT off, so no multiply-add is fused and both sides round
    # every operation the IEEE way (reference: f64 is an IEEE double).
    FLITS = ["0.5", "0.25", "1.5", "3.0", "0.1", "10.0", "2.75", "0.001"]

    def fexpr(self, sc, d):
        r = self.r
        if d <= 0 or r.random() < 0.3:
            k = r.random()
            if k < 0.4:
                v = r.choice(["fa", "fb"])
                return v, v
            ivs = [n for n, t in sc.ints() if signed(t)]
            if ivs and k < 0.7:
                v = r.choice(ivs)
                return f"({v} as f64)", f"((double){v})"
            lit = r.choice(self.FLITS)
            return lit, lit
        op = r.choice(["+", "-", "*", "/"])
        a = self.fexpr(sc, d - 1)
        if op == "/":
            lit = r.choice(self.FLITS)
            return f"({a[0]} / {lit})", f"fdiv({a[1]}, {lit})"
        b = self.fexpr(sc, d - 1)
        name = {"+": "fadd", "-": "fsub", "*": "fmul"}[op]
        return f"({a[0]} {op} {b[0]})", f"{name}({a[1]}, {b[1]})"

    def ext_stmt(self, sc, depth, ind, in_loop):
        r = self.r
        pad = "    " * ind
        if self.ext2 and r.random() < 0.4:
            return self.ext2_stmt(sc, depth, ind, in_loop)
        k = r.random()
        if k < 0.25:
            v = r.choice(["fa", "fb"])
            e = self.fexpr(sc, 2)
            return ([f"{pad}{v} = {e[0]};",
                     f"{pad}if {v} > 1000000.0 {{ {v} = 1000000.0; }}",
                     f"{pad}if {v} < -1000000.0 {{ {v} = -1000000.0; }}"],
                    [f"{pad}{v} = {e[1]};",
                     f"{pad}if ({v} > 1000000.0) {v} = 1000000.0;",
                     f"{pad}if ({v} < -1000000.0) {v} = -1000000.0;"])
        if k < 0.35:
            # a float read back as an integer (truncation toward zero)
            v = r.choice(["fa", "fb"])
            return ([f"{pad}xs.push(({v} * 1000.0) as i64);"],
                    [f"{pad}vpush(&xs, (int64_t)fmul({v}, 1000.0));"])
        if k < 0.45 and depth > 0:
            a = self.fexpr(sc, 1)
            b = self.fexpr(sc, 1)
            op = r.choice(["<", ">", "<=", ">=", "==", "!="])
            f1, c1 = self.block(sc.child(), depth - 1, ind + 1, r.randint(1, 2), in_loop)
            return ([f"{pad}if {a[0]} {op} {b[0]} {{"] + f1 + [f"{pad}}}"],
                    [f"{pad}if ({a[1]} {op} {b[1]}) {{"] + c1 + [f"{pad}}}"])
        reset_fd = f"{pad}if str_len(st) > 40 {{ st = \"\"; }}"
        reset_c = f"{pad}if (st.n > 40) st.n = 0;"
        if k < 0.57:
            e = self.expr("i64", sc, 2)
            return ([f"{pad}st = st + int_to_str({e.fd});", reset_fd],
                    [f"{pad}sapp_int(&st, {e.c});", reset_c])
        if k < 0.64:
            lit = r.choice(["ab", "x", "-", "Fid", " "])
            return ([f"{pad}st = st + \"{lit}\";", reset_fd],
                    [f"{pad}sapp(&st, \"{lit}\");", reset_c])
        if k < 0.70:
            return [f"{pad}println(st);"], [f"{pad}printf(\"%.*s\\n\", (int)st.n, st.b);"]
        if k < 0.75:
            return [f"{pad}xs.push(str_len(st));"], [f"{pad}vpush(&xs, st.n);"]
        if k < 0.87 and not sc.en_locked():
            kk = r.random()
            if kk < 0.4:
                e = self.expr("i64", sc, 2)
                return ([f"{pad}en = Sh::One({e.fd});"],
                        [f"{pad}{{ int64_t t1_ = {e.c}; en = (Sh){{0, t1_, 0}}; }}"])
            if kk < 0.8:
                e1 = self.expr("i64", sc, 2)
                e2 = self.expr("i32", sc, 2)
                return ([f"{pad}en = Sh::Two({e1.fd}, {e2.fd});"],
                        [f"{pad}{{ int64_t t1_ = {e1.c}; int32_t t2_ = {e2.c}; en = (Sh){{1, t1_, t2_}}; }}"])
            return [f"{pad}en = Sh::Zero;"], [f"{pad}en = (Sh){{2, 0, 0}};"]
        if depth <= 0:
            return [f"{pad}xs.push(str_len(st));"], [f"{pad}vpush(&xs, st.n);"]
        # take en apart; its arms do not write en (they read its payload)
        x = self.fresh("x")
        y = self.fresh("y")
        self.used_ops.add(("add", "i64"))
        s1 = sc.child()
        s1.lock_en = True
        s1.add(x, "i64", False)
        f1, c1 = self.block(s1, depth - 1, ind + 2, r.randint(1, 2), in_loop)
        s2 = sc.child()
        s2.lock_en = True
        s2.add(x, "i64", False)
        s2.add(y, "i32", False)
        f2, c2 = self.block(s2, depth - 1, ind + 2, r.randint(1, 2), in_loop)
        s3 = sc.child()
        s3.lock_en = True
        f3, c3 = self.block(s3, depth - 1, ind + 2, r.randint(1, 2), in_loop)
        p2 = pad + "        "
        fd = ([f"{pad}match en {{", f"{pad}    Sh::One({x}) => {{", f"{p2}xs.push({x});"] + f1 +
              [f"{pad}    }}", f"{pad}    Sh::Two({x}, {y}) => {{", f"{p2}xs.push({x} + ({y} as i64));"] + f2 +
              [f"{pad}    }}", f"{pad}    Sh::Zero => {{"] + f3 + [f"{pad}    }}", f"{pad}}}"])
        c = ([f"{pad}if (en.tag == 0) {{", f"{p2}int64_t {x} = en.a;", f"{p2}vpush(&xs, {x});"] + c1 +
             [f"{pad}}} else if (en.tag == 1) {{", f"{p2}int64_t {x} = en.a; int32_t {y} = en.b;",
              f"{p2}vpush(&xs, add_i64({x}, (int64_t){y}));"] + c2 +
             [f"{pad}}} else {{"] + c3 + [f"{pad}}}"])
        return fd, c

    # ---- --ext2: fixed-precision floats, more of str, a `_` arm ---------
    # Every float given to ffix is within about 3.1e8 (fa, fb are clamped to
    # 1e6; i8/i16/i32 / 7.0), so `a * 1000.0 + 0.5` is far below 2^63 and
    # the `as i64` is in range (F-ftoi). C rounds each operation in its own
    # noinline function, as for --ext.
    def ext2_stmt(self, sc, depth, ind, in_loop):
        r = self.r
        pad = "    " * ind
        k = r.random()
        reset_fd = f"{pad}if str_len(st) > 40 {{ st = \"\"; }}"
        reset_c = f"{pad}if (st.n > 40) st.n = 0;"
        if k < 0.22:
            forms = [("fa", "fa"), ("fb", "fb"), ("fa + fb", "fadd(fa, fb)"),
                     ("fa - fb", "fsub(fa, fb)"),
                     ("(fa * fb) / 1000000.0", "fdiv(fmul(fa, fb), 1000000.0)")]
            small = [n for n, t in sc.ints() if t in ("i8", "i16", "i32")]
            if small and r.random() < 0.4:
                v = r.choice(small)
                f, cf = f"({v} as f64) / 7.0", f"fdiv((double){v}, 7.0)"
            else:
                f, cf = r.choice(forms)
            return [f"{pad}println(ffix({f}));"], [f"{pad}pfix({cf});"]
        if k < 0.40:
            def bound(mask):
                if r.random() < 0.3:
                    n = r.randint(0, 9)
                    return str(n), str(n)
                e = self.expr("i64", sc, 2)
                if r.random() < 0.2:
                    return e.fd, e.c  # negative or past the end: clamped
                return f"({wrap(e, PREC['&'] + 1)} & {mask})", f"(({e.c}) & {mask})"
            a = bound(15)
            b = bound(31)
            return ([f"{pad}st = str_slice(st, {a[0]}, {b[0]});"],
                    [f"{pad}{{ int64_t a_ = {a[1]}; int64_t b_ = {b[1]}; sslice(&st, a_, b_); }}"])
        if k < 0.52:
            e = self.expr("i64", sc, 2)
            return ([f"{pad}if str_len(st) > 0 {{",
                     f"{pad}    xs.push(str_byte_at(st, ({wrap(e, PREC['&'] + 1)} & 1023) % str_len(st)));",
                     f"{pad}}}"],
                    [f"{pad}if (st.n > 0) {{ int64_t i_ = (({e.c}) & 1023) % st.n;"
                     f" vpush(&xs, (int64_t)(unsigned char)st.b[i_]); }}"])
        if k < 0.64:
            lit = r.choice(["", "ab", "x", "-", "Fid", "0"])
            return ([f"{pad}if st == \"{lit}\" {{ xs.push(1); }} else {{ xs.push(2); }}"],
                    [f"{pad}if (st.n == {len(lit)} && memcmp(st.b, \"{lit}\", {len(lit)}) == 0)"
                     f" vpush(&xs, 1); else vpush(&xs, 2);"])
        if k < 0.80:
            vs = [(n, t) for n, t in sc.ints() if t != "u64"]
            if not vs:
                return [f"{pad}st = st + \"<>\";", reset_fd], [f"{pad}sapp(&st, \"<>\");", reset_c]
            v, _ = r.choice(vs)
            return ([f"{pad}st = f\"{{st}}<{{{v}}}>\";", reset_fd],
                    [f"{pad}sapp(&st, \"<\"); sapp_int(&st, (int64_t){v}); sapp(&st, \">\");", reset_c])
        if depth <= 0 or sc.en_locked():
            return [f"{pad}xs.push(str_len(st));"], [f"{pad}vpush(&xs, st.n);"]
        # a match on en with a wildcard arm; the arms read en, never write it
        x = self.fresh("x")
        y = self.fresh("y")
        s1 = sc.child()
        s1.lock_en = True
        s1.add(x, "i64", False)
        s1.add(y, "i32", False)
        f1, c1 = self.block(s1, depth - 1, ind + 2, r.randint(1, 2), in_loop)
        s2 = sc.child()
        s2.lock_en = True
        f2, c2 = self.block(s2, depth - 1, ind + 2, r.randint(1, 2), in_loop)
        p2 = pad + "        "
        fd = ([f"{pad}match en {{", f"{pad}    Sh::Two({x}, {y}) => {{", f"{p2}xs.push({x} - ({y} as i64));"] + f1 +
              [f"{pad}    }}", f"{pad}    _ => {{"] + f2 + [f"{pad}    }}", f"{pad}}}"])
        c = ([f"{pad}if (en.tag == 1) {{", f"{p2}int64_t {x} = en.a; int32_t {y} = en.b;",
              f"{p2}vpush(&xs, sub_i64({x}, (int64_t){y}));"] + c1 +
             [f"{pad}}} else {{"] + c2 + [f"{pad}}}"])
        return fd, c

    EXT2_DECL_FD = [
        "fn ffix(x: f64) -> str {",
        "    val neg: bool = x < 0.0;",
        "    var a: f64 = x;",
        "    if neg { a = 0.0 - x; }",
        "    val m: i64 = (a * 1000.0 + 0.5) as i64;",
        "    val f: i64 = m % 1000;",
        "    var s: str = int_to_str(m / 1000) + \".\";",
        "    if f < 100 { s = s + \"0\"; }",
        "    if f < 10 { s = s + \"0\"; }",
        "    s = s + int_to_str(f);",
        "    if neg && m != 0 { s = \"-\" + s; }",
        "    return s;",
        "}",
        ""]
    EXT2_PRELUDE_C = [
        "static void sslice(Str* s, int64_t a, int64_t b) { if (a < 0) a = 0; if (b > s->n) b = s->n;"
        " if (a >= b) { s->n = 0; return; } memmove(s->b, s->b + a, (size_t)(b - a)); s->n = b - a; }",
        "static void pfix(double x) { int neg = x < 0.0; double a = neg ? fsub(0.0, x) : x;"
        " int64_t m = (int64_t)fadd(fmul(a, 1000.0), 0.5);"
        " printf(\"%s%lld.%03lld\\n\", (neg && m != 0) ? \"-\" : \"\", (long long)(m / 1000), (long long)(m % 1000)); }",
    ]
    EXT2_FINAL_FD = ["    println(ffix(fa));", "    println(ffix(fb));", "    println(str_len(st));"]
    EXT2_FINAL_C = ["    pfix(fa);", "    pfix(fb);", "    printf(\"%lld\\n\", (long long)st.n);"]

    EXT_DECL_FD = ["enum Sh {", "    One(i64),", "    Two(i64, i32),", "    Zero,", "}", ""]
    EXT_LOCALS_FD = ["    var fa: f64 = 0.5;", "    var fb: f64 = 2.0;",
                     "    var st: str = \"\";", "    var en: Sh = Sh::Zero;"]
    EXT_LOCALS_C = ["    double fa = 0.5; double fb = 2.0;", "    Str st; st.n = 0;",
                    "    Sh en = {2, 0, 0};"]
    EXT_FINAL_FD = ["    println((fa * 1000.0) as i64);", "    println((fb * 1000.0) as i64);",
                    "    println(st);",
                    "    match en {",
                    "        Sh::One(ex) => { println(1); println(ex); }",
                    "        Sh::Two(ex, ey) => { println(2); println(ex); println(ey); }",
                    "        Sh::Zero => { println(0); }",
                    "    }"]
    EXT_FINAL_C = ["    printf(\"%lld\\n\", (long long)(int64_t)fmul(fa, 1000.0));",
                   "    printf(\"%lld\\n\", (long long)(int64_t)fmul(fb, 1000.0));",
                   "    printf(\"%.*s\\n\", (int)st.n, st.b);",
                   "    if (en.tag == 0) printf(\"1\\n%lld\\n\", (long long)en.a);",
                   "    else if (en.tag == 1) printf(\"2\\n%lld\\n%d\\n\", (long long)en.a, (int)en.b);",
                   "    else printf(\"0\\n\");"]
    EXT_PRELUDE_C = [
        "#pragma STDC FP_CONTRACT OFF",
        "#include <string.h>",
        "typedef struct { int64_t tag; int64_t a; int32_t b; } Sh;",
        "typedef struct { char b[256]; int64_t n; } Str;",
        "static void sapp(Str* s, const char* t) { size_t k = strlen(t); memcpy(s->b + s->n, t, k); s->n += (int64_t)k; }",
        "static void sapp_int(Str* s, int64_t v) { char t[32]; snprintf(t, sizeof t, \"%lld\", (long long)v); sapp(s, t); }",
        "static __attribute__((noinline)) double fadd(double a, double b) { return a + b; }",
        "static __attribute__((noinline)) double fsub(double a, double b) { return a - b; }",
        "static __attribute__((noinline)) double fmul(double a, double b) { return a * b; }",
        "static __attribute__((noinline)) double fdiv(double a, double b) { return a / b; }",
    ]

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
        sc.ext = self.ext
        body_fd, body_c = self.block(sc, 3, 1, r.randint(4, 12))
        if self.ext:
            fd = self.EXT_DECL_FD + (self.EXT2_DECL_FD if self.ext2 else []) + fd
        fd += ["fn main() -> i64 {", "    var xs: Vec<i64> = Vec::new();"]
        fd += [f"    xs.push({k * 7 + 1});" for k in range(4)]
        fd += (self.EXT_LOCALS_FD if self.ext else []) + body_fd
        fd += (self.EXT_FINAL_FD if self.ext else [])
        fd += (self.EXT2_FINAL_FD if self.ext2 else [])
        fd += ["    println(xs.len());", "    return 0;", "}"]
        c += ["int main(void) {", "    Vec xs = {0, 0, 0};"]
        c += [f"    vpush(&xs, {k * 7 + 1});" for k in range(4)]
        c += (self.EXT_LOCALS_C if self.ext else []) + body_c
        c += (self.EXT_FINAL_C if self.ext else [])
        c += (self.EXT2_FINAL_C if self.ext2 else [])
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
        if self.ext:
            out += self.EXT_PRELUDE_C
        if self.ext2:
            out += self.EXT2_PRELUDE_C
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
        self.ext = parent.ext if parent else False  # --ext locals in scope
        self.lock_en = False                        # inside a match on en

    def en_locked(self):
        return self.lock_en or (self.parent is not None and self.parent.en_locked())

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
            elif cc == "gcc" and os.name != "nt" and shutil.which("gcc"):
                # Linux and macOS (CI): the host's own gcc (VF2)
                exe = os.path.join(work, f"{seed}-g{opt}")
                rc, out, err = run(["gcc", opt, "-w", path, "-o", exe], 300)
                if rc != 0:
                    res[key] = ("cc-failed", err[:2000])
                    continue
                rc, out, _ = run([exe], 60)
                res[key] = (rc, out)
                try:
                    os.remove(exe)
                except OSError:
                    pass
            elif cc == "gcc":
                exe = wsl_path(os.path.join(work, f"{seed}-g{opt}"))
                rc, out, err = run(["wsl", "-d", os.environ.get("WSL_DISTRO", "kali-linux"), "-e",
                                    "sh", "-c", f"gcc {opt} -w '{wsl_path(path)}' -o '{exe}' && '{exe}'; s=$?; rm -f '{exe}'; exit $s"], 120)
                res[key] = (rc, out)
    return res


QEMU = {"aarch64-linux": "qemu-aarch64", "riscv64-linux": "qemu-riscv64", "x86_64-linux": ""}


def linux_runner(target):
    """The command prefix that runs a static Linux binary of `target` here,
    and whether paths must be given as WSL paths; None if it cannot run."""
    q = QEMU.get(target)
    if q is None:
        return None
    if os.name == "nt":
        if not shutil.which("wsl"):
            return None
        pre = ["wsl"] + (["-d", os.environ["WSL_DISTRO"]] if os.environ.get("WSL_DISTRO") else []) + ["-e"]
        return (pre + ([q] if q else []), True)
    import platform
    host = {"x86_64": "x86_64-linux", "amd64": "x86_64-linux", "aarch64": "aarch64-linux",
            "arm64": "aarch64-linux", "riscv64": "riscv64-linux"}.get(platform.machine().lower())
    if sys.platform.startswith("linux") and host == target:
        return ([], False)
    if q and shutil.which(q):
        return ([q], False)
    return None


def backend_cmd(fid, b, src, exe):
    """(build command, run command) of backend `b`, or None if this host
    cannot run it."""
    kind, _, target = b.partition(":")
    if kind == "ccompile-O2":
        return [fid, "ccompile", src, "--cc-opt", "2", "-o", exe], [exe]
    if kind == "nat":
        if os.name != "nt":
            return None
        return [fid, "emit", src, "--nat", "-o", exe], [exe]
    if kind in ("emit", "ccompile") and target:
        r = linux_runner(target)
        if r is None:
            return None
        pre, wsl = r
        opt = ["--cc-opt", "0"] if kind == "ccompile" else []
        build = [fid, kind, src, "--target", target] + opt + ["-o", exe]
        return build, pre + [wsl_path(exe) if wsl else exe]
    return [fid, kind, src, "-o", exe], [exe]


def all_backends():
    bs = ["run", "ccompile", "ccompile-O2", "emit"]
    if os.name == "nt":
        bs.append("nat")
    for t in QEMU:
        if linux_runner(t) is not None:
            bs += [f"emit:{t}", f"ccompile:{t}"]
    return bs


def fides_results(fid, src, work, seed, backends):
    res = {}
    for b in backends:
        exe = os.path.join(work, f"{seed}-{b.replace(':', '-')}.exe")
        if b == "run":
            rc, out, err = run([fid, "run", src], 120)
            res[b] = (rc, out, err)
            continue
        cmds = backend_cmd(fid, b, src, exe)
        if cmds is None:
            continue  # this host cannot run it (main() reports it once)
        rc, out, err = run(cmds[0], 300)
        if rc != 0 or not os.path.exists(exe):
            res[b] = ("build-failed", "", (out + err)[-3000:])
            continue
        rc, out, err = run(cmds[1], 60)
        res[b] = (rc, out, err)
        for e in (exe, exe.replace(".exe", ".pdb")):
            if os.path.exists(e):
                os.remove(e)
    # Self-test of the comparison (tests/tooling/gen-vs-c-check.sh, RL1):
    # GEN_VS_C_MUTATE=<backend> adds a line to that backend's output, as a
    # miscompile would; every seed must then be reported as differing.
    m = os.environ.get("GEN_VS_C_MUTATE", "")
    if m in res and res[m][0] != "build-failed":
        rc, out, err = res[m]
        res[m] = (rc, out + "mutated\n", err)
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
    ap.add_argument("--ext", action="store_true",
                    help="main also uses f64, str and an enum with payloads (Gen.ext_stmt)")
    ap.add_argument("--ext2", action="store_true",
                    help="--ext plus fixed-precision float printing, str_slice, str_byte_at, "
                         "str ==, f-strings and a `_` arm (Gen.ext2_stmt)")
    ap.add_argument("--jobs", type=int, default=1, help="seeds checked at once")
    ap.add_argument("--log", default="", help="append one line per seed and a summary to this file")
    a = ap.parse_args()
    EXT[0] = a.ext or a.ext2
    EXT2[0] = a.ext2
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
    # gcc: WSL's on Windows, the host's elsewhere (c_results)
    if "gcc" in ccs and not (shutil.which("gcc") if os.name != "nt" else shutil.which("wsl")):
        ccs.remove("gcc")
    backends = all_backends() if a.backends == "all" else [b for b in a.backends.split(",") if b]
    for b in backends:
        if b != "run" and backend_cmd(a.fid, b, "x.fd", "x.exe") is None:
            print(f"backend {b}: cannot run on this host (no WSL, qemu-user or --nat here): left out")
    backends = [b for b in backends if b == "run" or backend_cmd(a.fid, b, "x.fd", "x.exe") is not None]
    print(f"backends: {','.join(backends)}", flush=True)
    os.makedirs(a.keep, exist_ok=True)

    def check(seed):
        """(verdict, message, backends that differ) of one seed; its files
        are its own, so seeds can be checked at once (--jobs)."""
        fd, csrc = pair(seed)
        src = os.path.join(a.keep, f"{seed}.fd")
        with open(src, "w", newline="\n") as f:
            f.write(fd)
        cres = c_results(csrc, a.keep, seed, ccs)
        couts = {v for v in cres.values()}
        if len(couts) != 1 or any(v[0] == "cc-failed" for v in cres.values()):
            return "oracle", f"seed {seed}: C oracle disagrees with itself: {cres}", []
        want = next(iter(couts))
        fres = fides_results(a.fid, src, a.keep, seed, backends)
        bad = [b for b, (rc, out, _) in fres.items() if (rc, out) != want]
        if not bad:
            os.remove(src)
            os.remove(os.path.join(a.keep, f"{seed}.c"))
            return "ok", (f"seed {seed}: ok ({'panic' if want[0] == 1 else 'exit ' + str(want[0])}, "
                          f"{want[1].count(chr(10))} lines)"), []
        with open(os.path.join(a.keep, f"{seed}.want"), "w") as f:
            f.write(f"exit: {want[0]}\n{want[1]}")
        for b, (rc, out, err) in fres.items():
            # `emit:aarch64-linux` -> `emit-aarch64-linux`: on Windows a `:` in
            # a file name writes an NTFS stream of `<seed>.emit`, not a file
            with open(os.path.join(a.keep, f"{seed}.{b.replace(':', '-')}.out"), "w", encoding="utf-8") as f:
                f.write(f"exit: {rc}\n{out}\n--- stderr ---\n{err[-3000:]}")
        return "differs", f"seed {seed}: differs from C on {','.join(bad)} -> {src}", bad

    fails = oracle_bad = 0
    per_backend = {b: 0 for b in backends}
    log = open(a.log, "a", encoding="utf-8") if a.log else None
    if log:
        log.write(f"# gen_vs_c seeds {a.seed}..{a.seed + a.n - 1} fid={a.fid} backends={','.join(backends)} "
                  f"cc={','.join(ccs)} ext={a.ext} ext2={a.ext2} avoid={a.avoid}\n")

    def record(seed, verdict, msg, bad):
        nonlocal fails, oracle_bad
        print(msg, flush=True)
        if verdict == "oracle":
            oracle_bad += 1
        elif verdict == "differs":
            fails += 1
            for b in bad:
                per_backend[b] = per_backend.get(b, 0) + 1
        if log:
            log.write(f"{seed} {verdict} {','.join(bad)}\n")
            log.flush()

    seeds = range(a.seed, a.seed + a.n)
    if a.jobs <= 1:
        for seed in seeds:
            record(seed, *check(seed))
    else:
        import concurrent.futures
        with concurrent.futures.ThreadPoolExecutor(max_workers=a.jobs) as ex:
            futs = {ex.submit(check, s): s for s in seeds}
            for fu in concurrent.futures.as_completed(futs):
                record(futs[fu], *fu.result())
    by = ", ".join(f"{b} {n}" for b, n in per_backend.items())
    summary = (f"{a.n} programs: {fails} differ from C, {oracle_bad} with a C oracle that disagrees "
               f"with itself (by backend: {by})")
    print(summary)
    if log:
        log.write(f"# {summary}\n")
        log.close()
    sys.exit(1 if fails else 0)


if __name__ == "__main__":
    main()
