#!/usr/bin/env python3
"""DB2 / DB4 check: the .debug_info of a native Linux build of
tests/bootstrap/test_machine_dbg_vars.fd (compiler/nat_enc.fd
nat_elf_debug, compiler/nat_ir.fd ng_dbg_vars_note / ng_dbg_vars_resolve).

    fid emit tests/bootstrap/test_machine_dbg_vars.fd --target x86_64-linux -o t
    python3 tests/nattools/dbg_vars_check.py t [tests/bootstrap/test_machine_dbg_vars.fd]

Parses .debug_abbrev and .debug_info (DWARF 4, table-driven: the forms the
writer uses) and checks, for scale, area, mix, count_bytes and main:
  - DW_AT_frame_base is DW_OP_call_frame_cfa;
  - DW_AT_decl_file names a file and DW_AT_decl_line lies inside the
    function in the source;
  - the parameters (DW_TAG_formal_parameter) and variables
    (DW_TAG_variable) the source declares are its children, each with a
    DW_AT_type that refers to a type entry of the unit;
  - every DW_AT_location is a register (DW_OP_reg*, DW_OP_regx), a frame
    slot under the CFA (DW_OP_fbreg -off) or a constant
    (DW_OP_consts; DW_OP_stack_value), and scale's are all present (its
    values live in registers or slots throughout).
With llvm-dwarfdump on PATH also runs `llvm-dwarfdump --verify`.
Exit 0 when every check holds."""
import os
import re
import shutil
import struct
import subprocess
import sys

DW_TAG_subprogram = 0x2E
DW_TAG_formal_parameter = 0x05
DW_TAG_variable = 0x34
TYPE_TAGS = (0x24, 0x13, 0x0F, 0x16)  # base, structure, pointer, typedef (str)

WANT = {
    "scale": (["base", "factor"], ["acc", "step"]),
    "area": (["r"], ["a"]),
    "mix": (["x", "flag"], ["out"]),
    "count_bytes": (["s"], ["n", "i"]),
    "main": ([], ["total", "box", "big"]),
}


def section(data, want):
    if data[:4] != b"\x7fELF":
        # CT9: a Mach-O (__DWARF) or `--nat` PE (COFF) image
        sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
        import dbg_sections
        return dbg_sections.section(data, want)
    shoff = struct.unpack_from("<Q", data, 0x28)[0]
    shentsize, shnum, shstrndx = struct.unpack_from("<HHH", data, 0x3A)

    def hdr(i):
        return struct.unpack_from("<IIQQQQIIQQ", data, shoff + i * shentsize)
    stroff = hdr(shstrndx)[4]
    for i in range(shnum):
        h = hdr(i)
        name = data[stroff + h[0]:data.index(b"\0", stroff + h[0])].decode()
        if name == want:
            return data[h[4]:h[4] + h[5]]
    return None


def uleb(b, p):
    v = 0
    s = 0
    while True:
        c = b[p]
        p += 1
        v |= (c & 0x7F) << s
        s += 7
        if c < 0x80:
            return v, p


def sleb(b, p):
    v = 0
    s = 0
    while True:
        c = b[p]
        p += 1
        v |= (c & 0x7F) << s
        s += 7
        if c < 0x80:
            if c & 0x40:
                v -= 1 << s
            return v, p


def abbrevs(b):
    out = {}
    p = 0
    while p < len(b):
        code, p = uleb(b, p)
        if code == 0:
            break
        tag, p = uleb(b, p)
        children = b[p]
        p += 1
        attrs = []
        while True:
            a, p = uleb(b, p)
            f, p = uleb(b, p)
            if a == 0 and f == 0:
                break
            attrs.append((a, f))
        out[code] = (tag, children, attrs)
    return out


def read_form(b, p, form):
    if form == 0x08:  # string
        e = b.index(b"\0", p)
        return b[p:e].decode(), e + 1
    if form == 0x0B:
        return b[p], p + 1
    if form == 0x05:
        return struct.unpack_from("<H", b, p)[0], p + 2
    if form in (0x06, 0x13, 0x17):
        return struct.unpack_from("<I", b, p)[0], p + 4
    if form in (0x07, 0x01):
        return struct.unpack_from("<Q", b, p)[0], p + 8
    if form == 0x18:
        n, p = uleb(b, p)
        return bytes(b[p:p + n]), p + n
    if form == 0x19:
        return True, p
    raise ValueError("form 0x%x not handled" % form)


def dies(info, ab):
    """Every entry of the unit: (offset, tag, attrs dict, depth, parent)."""
    out = []
    p = 11
    end = struct.unpack_from("<I", info, 0)[0] + 4
    depth = 0
    stack = [None]
    while p < end:
        off = p
        code, p = uleb(info, p)
        if code == 0:
            depth -= 1
            stack.pop()
            if depth < 0:
                break
            continue
        tag, children, attrs = ab[code]
        vals = {}
        for a, f in attrs:
            v, p = read_form(info, p, f)
            vals[a] = v
        out.append((off, tag, vals, depth, stack[-1]))
        if children:
            depth += 1
            stack.append(off)
    return out


def loc_ok(e, loc_sec=None):
    """A location the writer makes: a register, a frame slot under the
    CFA, a constant; (CT9) a struct reached through its address: the
    memory at a register (DW_OP_breg*/bregx 0), at a slot's word
    (DW_OP_fbreg; DW_OP_deref) or at a constant address; (CT6) an int is
    a location list in .debug_loc (`loc_sec`), each entry one of those."""
    if isinstance(e, int):
        if loc_sec is None:
            return False
        p = e
        n_ent = 0
        while p + 16 <= len(loc_sec):
            s, t = struct.unpack_from("<QQ", loc_sec, p)
            p += 16
            if s == 0 and t == 0:
                return True
            n = struct.unpack_from("<H", loc_sec, p)[0]
            p += 2
            if t <= s or not loc_ok(bytes(loc_sec[p:p + n])):
                return False
            p += n
            n_ent += 1
        return False
    if len(e) == 0:
        return False
    if len(e) == 1 and 0x50 <= e[0] <= 0x6F:
        return True
    if e[0] == 0x90:
        _, q = uleb(e, 1)
        return q == len(e)
    if 0x70 <= e[0] <= 0x8F:
        off, q = sleb(e, 1)
        return q == len(e) and off == 0
    if e[0] == 0x92:
        _, q = uleb(e, 1)
        off, q = sleb(e, q)
        return q == len(e) and off == 0
    if e[0] == 0x91:
        off, q = sleb(e, 1)
        if q + 1 == len(e) and e[q] == 0x06:
            q += 1
        return q == len(e) and -65536 < off < 0
    if e[0] == 0x11:
        _, q = sleb(e, 1)
        return q == len(e) or (q + 1 == len(e) and e[q] == 0x9F)
    return False


def fn_lines(src):
    """name -> (first line, last line) of each `fn` in the source."""
    out = {}
    lines = open(src, encoding="utf-8").read().split("\n")
    i = 0
    while i < len(lines):
        m = re.match(r"fn (\w+)\(", lines[i])
        if m:
            j = i
            while j < len(lines) and lines[j] != "}":
                j += 1
            out[m.group(1)] = (i + 1, j + 1)
            i = j
        i += 1
    return out


def main():
    exe = sys.argv[1]
    here = os.path.dirname(os.path.abspath(__file__))
    src = sys.argv[2] if len(sys.argv) > 2 else os.path.join(here, "..", "bootstrap", "test_machine_dbg_vars.fd")
    data = open(exe, "rb").read()
    info = section(data, ".debug_info")
    ab_b = section(data, ".debug_abbrev")
    if info is None or ab_b is None:
        print("FAIL: no .debug_info/.debug_abbrev")
        return 1
    ab = abbrevs(ab_b)
    all_dies = dies(info, ab)
    by_off = {d[0]: d for d in all_dies}
    spans = fn_lines(src)
    fails = []
    for fname, (params, vars_) in WANT.items():
        fn = [d for d in all_dies if d[1] == DW_TAG_subprogram and d[2].get(0x03) == fname]
        if not fn:
            # a small function the IR optimizer inlined everywhere has no
            # code of its own (area, mix): only scale and main must exist
            if fname in ("scale", "main"):
                fails.append("%s: no subprogram" % fname)
            else:
                print("note: %s has no subprogram (inlined)" % fname)
            continue
        off, _, a, depth, _ = fn[0]
        if a.get(0x40) != b"\x9c":
            fails.append("%s: DW_AT_frame_base is not DW_OP_call_frame_cfa (%r)" % (fname, a.get(0x40)))
        lo, hi = spans.get(fname, (0, 0))
        if not a.get(0x3A):
            fails.append("%s: no DW_AT_decl_file" % fname)
        dl = a.get(0x3B, 0)
        if not (lo <= dl <= hi):
            fails.append("%s: DW_AT_decl_line %d not in lines %d-%d" % (fname, dl, lo, hi))
        kids = [d for d in all_dies if d[4] == off]
        got_p = [d[2].get(0x03) for d in kids if d[1] == DW_TAG_formal_parameter]
        got_v = [d[2].get(0x03) for d in kids if d[1] == DW_TAG_variable]
        if got_p != params:
            fails.append("%s: parameters %r, want %r" % (fname, got_p, params))
        for v in vars_:
            if v not in got_v:
                fails.append("%s: variable %s missing (has %r)" % (fname, v, got_v))
        for d in kids:
            nm = d[2].get(0x03)
            t = d[2].get(0x49)
            if t is None or t not in by_off or by_off[t][1] not in TYPE_TAGS:
                fails.append("%s.%s: DW_AT_type does not refer to a type entry" % (fname, nm))
            loc = d[2].get(0x02)
            if loc is not None and not loc_ok(loc, section(data, ".debug_loc")):
                fails.append("%s.%s: location %s not a register, CFA slot or constant" % (fname, nm, loc.hex() if isinstance(loc, bytes) else "list@%d" % loc))
            if fname == "scale" and loc is None:
                fails.append("scale.%s: no location" % nm)
    dd = shutil.which("llvm-dwarfdump")
    if dd:
        r = subprocess.run([dd, "--verify", exe], capture_output=True, text=True)
        if r.returncode != 0:
            fails.append("llvm-dwarfdump --verify: " + (r.stdout + r.stderr).strip()[-400:])
    for f in fails:
        print("FAIL: " + f)
    if not fails:
        print("dbg vars OK")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
