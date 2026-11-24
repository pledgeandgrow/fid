#!/usr/bin/env python3
"""DB4 check: the lexical scopes (DW_TAG_lexical_block) of a native build
of a test such as tests/bootstrap/test_machine_dbg_scopes.fd
(compiler/nat_ir.fd ng_dbg_scope_lines / ng_dbg_scope_blocks,
compiler/nat_enc.fd nat_dbg_put_vars).

    fid emit tests/bootstrap/test_machine_dbg_scopes.fd --target x86_64-linux -o t
    python3 tests/nattools/dbg_scopes_check.py t tests/bootstrap/test_machine_dbg_scopes.fd

Reads .debug_abbrev / .debug_info (an ELF, or through dbg_sections.py a
Mach-O or PE) and checks:
  - every lexical block has a non-empty range inside its parent's (the
    enclosing block or function), and sibling blocks do not overlap (what
    `llvm-dwarfdump --verify` requires);
  - for each `// dbg-scope fn name N` line of the test: function `fn` has
    at most one variable `name` of its own (the others are in blocks: two
    of the function itself is the shadowing DB4 removes), and at least N
    blocks of it hold a variable `name`;
  - for each `// @marker` line and `// dbg-in marker name depth` line:
    at the marker's address the innermost scope that binds `name` is
    `depth` blocks deep (0: the function itself).
With llvm-dwarfdump on PATH also runs `llvm-dwarfdump --verify`.
Exit 0 when every check holds."""
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import dbg_vars_check as dv  # noqa: E402
import dbg_run_check as dr  # noqa: E402

DW_TAG_lexical_block = 0x0B
VAR_TAGS = (dv.DW_TAG_formal_parameter, dv.DW_TAG_variable)


def rng(d):
    lo = d[2].get(0x11)
    if lo is None:
        return None
    return (lo, lo + d[2].get(0x12, 0))


def main():
    if len(sys.argv) < 3:
        print(__doc__)
        return 2
    exe, src = sys.argv[1], sys.argv[2]
    data = open(exe, "rb").read()
    info = dv.section(data, ".debug_info")
    ab = dv.section(data, ".debug_abbrev")
    if info is None or ab is None:
        print("FAIL: no .debug_info/.debug_abbrev")
        return 1
    dies = dv.dies(info, dv.abbrevs(ab))
    by_off = {d[0]: d for d in dies}
    kids = {}
    for d in dies:
        kids.setdefault(d[4], []).append(d)
    fails = []
    nblocks = 0
    # structure of every block
    for d in dies:
        if d[1] != DW_TAG_lexical_block:
            continue
        nblocks += 1
        r = rng(d)
        par = by_off.get(d[4])
        if r is None or r[1] <= r[0]:
            fails.append("block at 0x%x: empty or no range" % d[0])
            continue
        pr = rng(par) if par is not None else None
        if par is None or par[1] not in (dv.DW_TAG_subprogram, DW_TAG_lexical_block) or pr is None:
            fails.append("block at 0x%x: not inside a function or block" % d[0])
        elif not (pr[0] <= r[0] and r[1] <= pr[1]):
            fails.append("block at 0x%x: [0x%x, 0x%x) outside its parent's [0x%x, 0x%x)" % (d[0], r[0], r[1], pr[0], pr[1]))
    for p, ks in kids.items():
        bl = sorted([rng(k) for k in ks if k[1] == DW_TAG_lexical_block and rng(k) is not None])
        for a, b in zip(bl, bl[1:]):
            if b[0] < a[1]:
                fails.append("sibling blocks under 0x%x overlap: [0x%x, 0x%x) and [0x%x, 0x%x)" % (p or 0, a[0], a[1], b[0], b[1]))
    text = open(src, encoding="utf-8").read().split("\n")
    subs = {d[2].get(0x03): d for d in dies if d[1] == dv.DW_TAG_subprogram}

    def blocks_under(d):
        out = []
        for k in kids.get(d[0], []):
            if k[1] == DW_TAG_lexical_block:
                out.append(k)
                out.extend(blocks_under(k))
        return out
    for ln in text:
        m = re.match(r"\s*//\s*dbg-scope\s+(\S+)\s+(\w+)\s+(\d+)\s*$", ln)
        if not m:
            continue
        fname, vname, want = m.group(1), m.group(2), int(m.group(3))
        sub = subs.get(fname)
        if sub is None:
            print("note: %s has no subprogram (inlined?)" % fname)
            continue
        own = [k for k in kids.get(sub[0], []) if k[1] in VAR_TAGS and k[2].get(0x03) == vname]
        if len(own) > 1:
            fails.append("%s: %d variables %s of the function itself (shadowed names not in scopes)" % (fname, len(own), vname))
        holding = [b for b in blocks_under(sub) if any(k[1] in VAR_TAGS and k[2].get(0x03) == vname for k in kids.get(b[0], []))]
        if len(holding) < want:
            fails.append("%s: %d lexical blocks hold %s, want at least %d" % (fname, len(holding), vname, want))
        else:
            print("ok %s: %s in %d blocks" % (fname, vname, len(holding)))
    # where a marker resolves a name
    markers = {}
    for i, ln in enumerate(text):
        m = re.search(r"//\s*@(\w+)\s*$", ln)
        if m and not ln.lstrip().startswith("//"):
            markers[m.group(1)] = i + 1
    rows = dr.line_rows(dv.section(data, ".debug_line") or b"")
    # a marker's rows are the test file's: the same line number in a
    # runtime or stdlib file (a row inside pe_ld was seen at line 59) must
    # not take the address. The file index is per line-program unit; our
    # images carry one, so the basename of `src` names it.
    want_fil = None
    files = dr.line_files(dv.section(data, ".debug_line") or b"")
    base = os.path.basename(src)
    for ft in files:
        if base in ft:
            want_fil = ft.index(base) + 1
            break
    for ln in text:
        m = re.match(r"\s*//\s*dbg-in\s+(\w+)\s+(\w+)\s+(\d+)\s*$", ln)
        if not m:
            continue
        mk, vname, want = m.group(1), m.group(2), int(m.group(3))
        line = markers.get(mk)
        addrs = sorted(a for a, f, l, end in rows if not end and l == line and (want_fil is None or f == want_fil))
        if not addrs:
            fails.append("@%s: line %s has no row" % (mk, line))
            continue
        pc = addrs[0]
        sub = None
        for d in dies:
            r = rng(d)
            if d[1] == dv.DW_TAG_subprogram and r and r[0] <= pc < r[1]:
                sub = d
        if sub is None:
            fails.append("@%s: no function at 0x%x" % (mk, pc))
            continue
        chain = [sub]
        cur = sub
        while True:
            nxt = None
            for k in kids.get(cur[0], []):
                r = rng(k)
                if k[1] == DW_TAG_lexical_block and r and r[0] <= pc < r[1]:
                    nxt = k
            if nxt is None:
                break
            chain.append(nxt)
            cur = nxt
        depth = None
        for i in range(len(chain) - 1, -1, -1):
            if any(k[1] in VAR_TAGS and k[2].get(0x03) == vname for k in kids.get(chain[i][0], [])):
                depth = i
                break
        if depth != want:
            fails.append("@%s (0x%x): %s found %s blocks deep, want %d" % (mk, pc, vname, depth, want))
        else:
            print("ok @%s: %s %d deep" % (mk, vname, depth))
    dd = shutil.which("llvm-dwarfdump")
    if dd:
        r = subprocess.run([dd, "--verify", exe], capture_output=True, text=True)
        if r.returncode != 0:
            fails.append("llvm-dwarfdump --verify: " + (r.stdout + r.stderr).strip()[-400:])
        else:
            print("ok llvm-dwarfdump --verify")
    for f in fails:
        print("FAIL: " + f)
    if not fails:
        print("dbg scopes OK (%d blocks)" % nblocks)
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
