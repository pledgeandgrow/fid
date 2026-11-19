#!/usr/bin/env python3
"""SP16 (llvm-roadmap): can the machine layer build the compiler itself?

A static precondition of `fid emit compiler/main.fd --target <arch>-linux`
(or `--nat`, `--target *-macos`): every runtime builtin the compiler and the
stdlib modules it reaches call must be something nat_ir can run. nat_ir
resolves a call to builtin `p_X` / `__X` (compiler/nat_ir.fd ng_resolve):

  1. a pe_runtime_alias (compiler/ir.fd: p_vec_* -> core's Vec_*),
  2. the runtime's Fides `pert_X` (stdlib/linux_runtime/*.fd plus
     pe_runtime/data.fd and tensor.fd, merged by compiler.fd),
  3. a function of that name in the program (core defines many p_*),
  4. code nat_ir emits itself for that name (its string literals: inline
     builtins and primitives such as nat_syscall).

Anything else becomes "not available in native Linux executables yet" at
link time, after the whole compiler was lowered. This check finds those
names in a second, without building anything.

It also checks the `extern "C"` functions reachable from main (a call
graph over the reached modules, as compiler/reach.fd keeps for `emit`):
each must be the runtime's pert_c_<C name> or a primitive nat_ir emits
(nat_c_<C name>, nat_syscall, ...); anything else would only be a dynamic
import of the system's C library (x86-64 Linux), which a `pledge_*` name of
the C runtime's own headers is not in. Negative case:
tests/nattools/selfhost_extern_probe.fd (exit 1, pledge_net_socket).

Usage: python3 tests/nattools/selfhost_builtins_check.py [--layer win|mac]
  [--root <repo>] [--main compiler/main.fd] [-v]
Exit 0 when every builtin resolves, 1 otherwise (the missing ones listed
with a file that calls each).
"""
import os
import re
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
MAIN = "compiler/main.fd"
LAYER = ""
VERBOSE = False
args = sys.argv[1:]
i = 0
while i < len(args):
    a = args[i]
    if a == "--root":
        ROOT = os.path.abspath(args[i + 1]); i += 1
    elif a == "--main":
        MAIN = args[i + 1]; i += 1
    elif a == "--layer":
        LAYER = args[i + 1]; i += 1
    elif a == "-v":
        VERBOSE = True
    else:
        print("usage: selfhost_builtins_check.py [--layer win|mac] [--root dir] [--main file] [-v]")
        sys.exit(2)
    i += 1


def read(path):
    with open(os.path.join(ROOT, path), "r", encoding="utf-8", errors="replace") as f:
        return f.read()


def strip(src):
    """Source without comments and string literals (kept as "")."""
    out = []
    i, n = 0, len(src)
    while i < n:
        c = src[i]
        if c == "/" and i + 1 < n and src[i + 1] == "/":
            j = src.find("\n", i)
            i = n if j < 0 else j
            continue
        if c == "/" and i + 1 < n and src[i + 1] == "*":
            j = src.find("*/", i + 2)
            i = n if j < 0 else j + 2
            continue
        if c == '"':
            j = i + 1
            while j < n and src[j] != '"':
                j += 2 if src[j] == "\\" else 1
            out.append('""')
            i = j + 1
            continue
        if c == "'" and i + 2 < n and (src[i + 2] == "'" or (src[i + 1] == "\\" and i + 3 < n and src[i + 3] == "'")):
            j = i + (4 if src[i + 1] == "\\" else 3)
            out.append("0")
            i = j
            continue
        out.append(c)
        i += 1
    return "".join(out)


def literals(src):
    return set(re.findall(r'"((?:p_|__|pert_|nat_)[A-Za-z0-9_]+)"', src))


USE_RE = re.compile(r"\buse\s+([A-Za-z_][A-Za-z0-9_:]*)\s*;")


def module_path(name):
    if name.startswith("compiler::"):
        return "compiler/" + name[len("compiler::"):].replace("::", "/") + ".fd"
    return "stdlib/" + name.replace("::", "/") + ".fd"


# 1. the modules the compiler reaches (core is always merged)
seen = {}
todo = [MAIN, "stdlib/core.fd"]
while todo:
    p = todo.pop()
    if p in seen or not os.path.exists(os.path.join(ROOT, p)):
        continue
    s = strip(read(p))
    seen[p] = s
    for m in USE_RE.findall(s):
        todo.append(module_path(m))

# 2. the runtime merged into a machine-layer build (compiler.fd: rt_files)
rt = ["stdlib/linux_runtime/base.fd", "stdlib/linux_runtime/conc.fd",
      "stdlib/linux_runtime/sys.fd", "stdlib/linux_runtime/math.fd",
      "stdlib/pe_runtime/data.fd", "stdlib/pe_runtime/tensor.fd"]
if LAYER == "win":
    rt.append("stdlib/linux_runtime/win.fd")
elif LAYER == "mac":
    rt.append("stdlib/linux_runtime/darwin.fd")
rt_src = {p: strip(read(p)) for p in rt if os.path.exists(os.path.join(ROOT, p))}

FN_RE = re.compile(r"\bfn\s+([A-Za-z_][A-Za-z0-9_]*)\s*[(<]")
defined = set()
for s in list(seen.values()) + list(rt_src.values()):
    defined.update(FN_RE.findall(s))

# 3. aliases (compiler/ir.fd pe_runtime_alias) and what nat_ir emits itself
ir_src = read("compiler/ir.fd")
alias = set()
m = re.search(r"pub fn pe_runtime_alias\(name: str\) -> str \{(.*?)\n\}", ir_src, re.S)
if m:
    alias.update(re.findall(r'name == "([A-Za-z0-9_]+)"', m.group(1)))
inline = literals(read("compiler/nat_ir.fd")) | literals(read("compiler/nat_enc.fd"))

# 4. every builtin-shaped call of the compiler's modules
CALL_RE = re.compile(r"(?<![A-Za-z0-9_.:])((?:p_|__)[A-Za-z0-9_]+)\s*\(")
calls = {}
for p, s in seen.items():
    for name in CALL_RE.findall(s):
        calls.setdefault(name, p)
# runtime functions call builtins too (pert_X calling p_Y)
for p, s in rt_src.items():
    for name in CALL_RE.findall(s):
        calls.setdefault(name, p)


def pert(name):
    if name.startswith("p_"):
        return "pert_" + name[2:]
    if name.startswith("__"):
        return "pert_" + name[2:]
    return ""


missing = []
how = {"alias": 0, "pert": 0, "defined": 0, "inline": 0}
for name in sorted(calls):
    if name in alias:
        how["alias"] += 1
    elif pert(name) in defined:
        how["pert"] += 1
    elif name in defined:
        how["defined"] += 1
    elif name in inline:
        how["inline"] += 1
    else:
        missing.append(name)

label = {"": "Linux", "win": "--nat Windows", "mac": "macOS"}.get(LAYER, LAYER)
print("selfhost builtins (%s): %d modules, %d builtin names called: %d alias, %d pert_*, %d defined, %d emitted by nat_ir"
      % (label, len(seen), len(calls), how["alias"], how["pert"], how["defined"], how["inline"]))
if VERBOSE:
    for p in sorted(seen):
        print("  module " + p)
if missing:
    print("MISSING (%d): no runtime function, alias or nat_ir code:" % len(missing))
    for name in missing:
        print("  %-36s called in %s" % (name, calls[name]))

# 5. extern "C" functions the compiler reaches. nat_ir (ng_resolve's
# extern case) runs one as the runtime's pert_c_<C name>, else as a
# primitive it emits (nat_c_<C name>: sqrt, floor, ...), else (x86-64
# Linux only, CI2) as a dynamic import from the system's C library, which
# a compiler that must start anywhere cannot use (and a `pledge_*` name
# of the C runtime's own headers is in no system library). Only functions
# reachable from main count (as compiler/reach.fd lowers only those for
# `emit`): stdlib/dns.fd's UDP path calls pledge_net_* but the compiler
# reaches dns_lookup only. Reachability over-approximates: any name of a
# function in a reached body, `.m(` reaches every method m, `T::m` T_m.
WORD_RE = re.compile(r"[A-Za-z_][A-Za-z0-9_]*")
FNDEF_RE = re.compile(r"\bfn\s+([A-Za-z_][A-Za-z0-9_]*)\s*(?:<[^>{;]*>)?\s*\(")
IMPL_RE = re.compile(r"\bimpl\s*(?:<[^>{]*>)?\s*([A-Za-z_][A-Za-z0-9_]*)(?:\s*<[^>{]*>)?(?:\s+for\s+([A-Za-z_][A-Za-z0-9_]*))?[^{;]*\{")
EXTERN_RE = re.compile(r'\bextern\s+""[^{;]*\{')
EXTFN_RE = re.compile(r"\bfn\s+([A-Za-z_][A-Za-z0-9_]*)(?:\s+as\s+([A-Za-z_][A-Za-z0-9_]*))?\s*\(")


def match_brace(s, i):
    """Index just past the '}' closing the '{' at s[i]."""
    d = 0
    n = len(s)
    while i < n:
        c = s[i]
        if c == "{":
            d += 1
        elif c == "}":
            d -= 1
            if d == 0:
                return i + 1
        i += 1
    return n


bodies = {}        # function name -> list of body texts
methods = {}       # method short name -> qualified names
externs = {}       # Fides name -> (C name, module)
for p, s in list(seen.items()) + list(rt_src.items()):
    # extern "C" blocks: no bodies, Fides name -> C name
    ext_spans = []
    for em in EXTERN_RE.finditer(s):
        e = match_brace(s, em.end() - 1)
        ext_spans.append((em.start(), e))
        for fm in EXTFN_RE.finditer(s, em.end(), e):
            cname = fm.group(1)
            fname = fm.group(2) or cname
            externs.setdefault(fname, (cname, p))
    # impl blocks: which type each method belongs to
    impl_spans = []
    for im in IMPL_RE.finditer(s):
        e = match_brace(s, im.end() - 1)
        impl_spans.append((im.start(), e, im.group(2) or im.group(1)))
    for fm in FNDEF_RE.finditer(s):
        if any(a <= fm.start() < b for a, b in ext_spans):
            continue
        j = fm.end()
        while j < len(s) and s[j] not in "{;":
            j += 1
        if j >= len(s) or s[j] == ";":
            continue
        body = s[j:match_brace(s, j)]
        name = fm.group(1)
        owner = ""
        for a, b, t in impl_spans:
            if a <= fm.start() < b:
                owner = t
        if owner:
            q = owner + "_" + name
            bodies.setdefault(q, []).append(body)
            methods.setdefault(name, set()).add(q)
        else:
            bodies.setdefault(name, []).append(body)
            # a free `T_m` of a type T is reached by `.m(` too (core's
            # Vec_get, HashMap_insert, ...)
            k = name.find("_")
            if k > 0 and name[0].isupper():
                methods.setdefault(name[k + 1:], set()).add(name)

reached = set()
parent = {"main": ""}   # how each function was reached (for -v)


def reach(f, by):
    if f not in reached:
        parent.setdefault(f, by)
        work.append(f)


work = ["main"]
while work:
    f = work.pop()
    if f in reached:
        continue
    reached.add(f)
    for body in bodies.get(f, []):
        for wm in WORD_RE.finditer(body):
            w = wm.group(0)
            prev = body[wm.start() - 1] if wm.start() > 0 else " "
            if prev == ".":
                # a method call (a field read `.w` without "(" reaches nothing)
                if body[wm.end():wm.end() + 1] == "(" or body[wm.end():wm.end() + 2] == "::":
                    for q in methods.get(w, ()):
                        reach(q, f)
                continue
            if prev == ":" and wm.start() > 1 and body[wm.start() - 2] == ":":
                t = WORD_RE.findall(body[max(0, wm.start() - 80):wm.start() - 2])
                if t and (t[-1] + "_" + w) in bodies:
                    reach(t[-1] + "_" + w, f)
                continue
            if w in bodies or w in externs:
                reach(w, f)
            # a builtin runs the runtime's pert_X; an extern its pert_c_<C name>
            if pert(w) in bodies:
                reach(pert(w), f)
            if w in externs and ("pert_c_" + externs[w][0]) in bodies:
                reach("pert_c_" + externs[w][0], f)


def path_to(f):
    out = [f]
    while parent.get(out[-1], "") and len(out) < 40:
        out.append(parent[out[-1]])
    return " <- ".join(out)

ext_missing = []
ext_ok = []
for fname in sorted(externs):
    if fname not in reached:
        continue
    cname, p = externs[fname]
    if ("pert_c_" + cname) in defined:
        ext_ok.append(fname + " -> pert_c_" + cname)
    elif ("nat_c_" + cname) in inline:
        ext_ok.append(fname + " -> nat_c_" + cname)
    elif cname in inline:
        # a primitive of the machine layer itself (nat_syscall, nat_arch, ...)
        ext_ok.append(fname + " -> emitted by nat_ir")
    else:
        ext_missing.append((fname, cname, p))
print("selfhost extern \"C\" (%s): %d functions reachable from main, %d extern \"C\" among them: %d resolve, %d not"
      % (label, len(reached), len(ext_ok) + len(ext_missing), len(ext_ok), len(ext_missing)))
if VERBOSE:
    for e in ext_ok:
        print("  extern " + e)
if ext_missing:
    print("MISSING extern \"C\" (%d): no pert_c_/nat_c_ (only a dynamic import of the C library, x86-64 Linux):" % len(ext_missing))
    for fname, cname, p in ext_missing:
        print("  %-36s C %-24s declared in %s" % (fname, cname, p))
        if VERBOSE:
            print("    reached: " + path_to(fname))
if missing or ext_missing:
    sys.exit(1)
print("ok: every builtin the compiler calls resolves on the machine layer")
sys.exit(0)
