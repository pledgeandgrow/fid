#!/usr/bin/env python3
"""DB2 check (docs/roadmap/llvm-roadmap.md): each function's
DW_AT_decl_line is the line of its `fn` keyword (compiler/nat_enc.fd
nat_dbg_decl_line), not the first line of its body.

    python3 tests/nattools/dbg_decl_lines_check.py --fid FID [--targets T,...] [--keep DIR]
    python3 tests/nattools/dbg_decl_lines_check.py IMAGE [SOURCE]

With --fid, builds tests/bootstrap/test_nattools_decl_lines.fd with
`FID emit --target T` for each target (default x86_64-linux,
aarch64-linux, riscv64-linux, aarch64-macos, x86_64-macos; add
`x86_64-windows-nat` for `emit --nat` with FID_NAT_DWARF=1) and checks the
image's .debug_info (ELF), __debug_info (Mach-O __DWARF segment) or COFF
debug section (--nat PE). The expected line of each function is the source
line that declares it and ends in `// DECL <symbol>`; sum_to, weighted and
main must have an entry, the others (a method, a generic instance) are
checked when the optimizer left them one. A function's entry is found by
its name or linkage name (`Tally::add_upto` / `Tally_add_upto`,
`repeat_len<i64>` / `repeat_len__i64`).

Then, for the first target only, the same build with
`FID_NAT_OFF=declline` must give sum_to the line of its body's first
statement instead: the check sees the difference (it fails on a compiler
without nat_dbg_decl_line, whose decl lines are always the body's).

Exit 0 when every check holds."""
import os
import re
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import dbg_vars_check as dv  # noqa: E402

DW_TAG_subprogram = 0x2E
SRC = os.path.join(ROOT, "tests", "bootstrap", "test_nattools_decl_lines.fd")
REQUIRED = ("sum_to", "weighted", "main")


def decl_marks(src):
    """symbol -> the line (from 1) of its `fn` that ends in `// DECL symbol`."""
    out = {}
    for i, line in enumerate(open(src, encoding="utf-8").read().split("\n")):
        m = re.match(r"^\s*(pub\s+)?fn\s.*//\s*DECL\s+(\w+)\s*$", line)
        if m:
            out[m.group(2)] = i + 1
    return out


def first_stmt_line(src, decl_line):
    """The first line after `decl_line` that holds a statement (not blank,
    not a comment, not the rest of the signature)."""
    lines = open(src, encoding="utf-8").read().split("\n")
    i = decl_line - 1
    while i < len(lines) and "{" not in lines[i].split("//")[0]:
        i += 1
    i += 1
    while i < len(lines):
        t = lines[i].strip()
        if t and not t.startswith("//"):
            return i + 1
        i += 1
    return 0


def subprograms(image):
    data = open(image, "rb").read()
    info = dv.section(data, ".debug_info")
    ab_b = dv.section(data, ".debug_abbrev")
    if info is None or ab_b is None:
        return None
    ab = dv.abbrevs(ab_b)
    return [d for d in dv.dies(info, ab) if d[1] == DW_TAG_subprogram]


def find(subs, sym):
    """The entry of `sym`: by its name or its linkage name, or (a generic
    instance) by a name that starts with `sym<` / a linkage name `sym__`."""
    for d in subs:
        a = d[2]
        nm = a.get(0x03)
        link = a.get(0x6E)
        if nm == sym or link == sym:
            return d
    for d in subs:
        a = d[2]
        nm = a.get(0x03) or ""
        link = a.get(0x6E) or ""
        if nm.startswith(sym + "<") or link.startswith(sym + "__") or nm.startswith(sym + "__"):
            return d
    # a method: Type_method shown as Type::method
    if "_" in sym:
        t, m = sym.split("_", 1)
        for d in subs:
            if d[2].get(0x03) == t + "::" + m:
                return d
    return None


def check_image(image, src, label):
    fails = []
    oks = []
    subs = subprograms(image)
    if subs is None:
        return ["%s: no .debug_info/.debug_abbrev" % label], oks
    marks = decl_marks(src)
    if not marks:
        return ["%s: no `// DECL` lines in %s" % (label, src)], oks
    for sym, want in sorted(marks.items()):
        d = find(subs, sym)
        if d is None:
            if sym in REQUIRED:
                fails.append("%s: %s has no subprogram" % (label, sym))
            else:
                oks.append("%s: %s has no subprogram (inlined): not checked" % (label, sym))
            continue
        got = d[2].get(0x3B)
        if got == 0 and sym not in REQUIRED:
            # no line rows at all (DB1's business, not DB2's)
            oks.append("%s: %s has no line rows (decl line 0): not checked" % (label, sym))
            continue
        if got is None:
            fails.append("%s: %s has no DW_AT_decl_line" % (label, sym))
        elif got != want:
            fails.append("%s: %s DW_AT_decl_line %d, want %d (its `fn` line)" % (label, sym, got, want))
        else:
            oks.append("%s: %s decl line %d" % (label, sym, got))
    return fails, oks


def emit(fid, target, out, env_extra=None):
    env = dict(os.environ)
    if env_extra:
        env.update(env_extra)
    args = [fid, "emit", SRC]
    if target == "x86_64-windows-nat":
        args += ["--nat"]
        env["FID_NAT_DWARF"] = "1"
    else:
        args += ["--target", target]
    args += ["-o", out]
    r = subprocess.run(args, cwd=ROOT, env=env, capture_output=True, text=True)
    if r.returncode != 0 or not os.path.exists(out):
        return "emit --target %s failed: %s" % (target, (r.stdout + r.stderr)[-600:])
    return ""


def main():
    argv = sys.argv[1:]
    if argv and not argv[0].startswith("--"):
        image = argv[0]
        src = argv[1] if len(argv) > 1 else SRC
        fails, oks = check_image(image, src, os.path.basename(image))
        for o in oks:
            print("ok   " + o)
        for f in fails:
            print("FAIL " + f)
        return 1 if fails else 0
    fid = None
    targets = ["x86_64-linux", "aarch64-linux", "riscv64-linux", "aarch64-macos", "x86_64-macos"]
    keep = None
    i = 0
    while i < len(argv):
        if argv[i] == "--fid":
            fid = argv[i + 1]
            i += 2
        elif argv[i] == "--targets":
            targets = [t for t in argv[i + 1].split(",") if t]
            i += 2
        elif argv[i] == "--keep":
            keep = argv[i + 1]
            i += 2
        else:
            print("unknown argument " + argv[i])
            return 2
    if fid is None:
        fid = os.environ.get("FID", "fid")
    work = keep or tempfile.mkdtemp(prefix="db2decl")
    os.makedirs(work, exist_ok=True)
    fails = []
    for t in targets:
        out = os.path.join(work, "decl-" + t + (".exe" if "windows" in t else ""))
        err = emit(fid, t, out)
        if err:
            fails.append(err)
            continue
        f, oks = check_image(out, SRC, t)
        for o in oks:
            print("ok   " + o)
        fails += f
    # FID_NAT_OFF=declline: the body's first line, as before DB2
    if targets:
        t = targets[0]
        out = os.path.join(work, "decl-off-" + t + (".exe" if "windows" in t else ""))
        err = emit(fid, t, out, {"FID_NAT_OFF": "declline"})
        if err:
            fails.append("FID_NAT_OFF=declline: " + err)
        else:
            subs = subprograms(out) or []
            d = find(subs, "sum_to")
            want_fn = decl_marks(SRC).get("sum_to", 0)
            body = first_stmt_line(SRC, want_fn)
            got = d[2].get(0x3B) if d else None
            if got == body and body != want_fn:
                print("ok   FID_NAT_OFF=declline: sum_to decl line %d, its body's first line" % got)
            else:
                fails.append("FID_NAT_OFF=declline: sum_to decl line %r, want %d (the body's first line)" % (got, body))
    for f in fails:
        print("FAIL " + f)
    n = len(targets)
    print("DB2 decl lines: %d target(s), %d failed check(s)" % (n, len(fails)))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
