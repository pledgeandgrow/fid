#!/usr/bin/env python3
"""Release N9 / llvm-roadmap DB4 for pe_ir: decodes the PE that
tests/nattools/pe_dwarf_vars_unit.fd writes (compiler/pe_ir_enc.fd
pe_add_dwarf) and checks its DWARF the way a debugger reads it.

    PE_DWARF_OUT=u.exe fid run tests/nattools/pe_dwarf_vars_unit.fd
    python tests/nattools/pe_dwarf_vars_check.py u.exe

Checks: the four sections .debug_abbrev/.debug_info/.debug_line/
.debug_frame (long names through the COFF string table); per function
(helper_one, helper_two) a DW_TAG_subprogram at its address with
DW_AT_frame_base DW_OP_call_frame_cfa, DW_AT_decl_file 1 and
DW_AT_decl_line its `fn` line of the unit file (28, 32); its parameters
and variables in order with the locations given (DW_OP_fbreg off, a
struct reached through a pointer as DW_OP_fbreg off; DW_OP_deref, a
constant as DW_OP_consts v; DW_OP_stack_value, none for `gone`) and their
types (i64 base type signed 8, bool boolean 1, str a typedef of a pointer
to u8 characters, Point a struct with x at 0 and y at 8); an FDE per
function whose CFA after `push rbp; mov rbp, rsp` is rbp + 16; the line
table's first row of each function at its first byte. Exit 0 when all
hold."""
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import dbg_vars_check as dv  # noqa: E402
import dbg_run_check as dr  # noqa: E402

BASE = 0x140001000

WANT = {
    "helper_one": (0, 28, [
        (0x05, "base", b"\x91\x68", "i64"),
        (0x34, "acc", b"\x91\x60", "i64"),
        (0x34, "k", b"\x11\x07\x9f", "i64"),
        (0x34, "gone", None, "i64"),
    ]),
    "helper_two": (64, 32, [
        (0x05, "n", b"\x91\x68\x06", "Point"),
        (0x34, "name", b"\x91\x60", "str"),
        (0x34, "q", b"\x91\x50", "Point"),
        (0x34, "ok", b"\x91\x48", "bool"),
    ]),
}


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        return 2
    data = open(sys.argv[1], "rb").read()
    fails = []
    for s in (".debug_abbrev", ".debug_info", ".debug_line", ".debug_frame"):
        if dv.section(data, s) is None:
            fails.append("no %s section" % s)
    if fails:
        for f in fails:
            print("FAIL: " + f)
        return 1
    dbg = dr.Debug(sys.argv[1])
    for fname, (off, dline, kids) in WANT.items():
        sub = dbg.sub_named(fname)
        if sub is None:
            fails.append("%s: no subprogram" % fname)
            continue
        lo, hi, d = sub
        a = d[2]
        if lo != BASE + off:
            fails.append("%s: low_pc 0x%x, want 0x%x" % (fname, lo, BASE + off))
        if a.get(0x40) != b"\x9c":
            fails.append("%s: frame_base %r" % (fname, a.get(0x40)))
        if a.get(0x3A) != 1 or a.get(0x3B) != dline:
            fails.append("%s: decl %r:%r, want 1:%d" % (fname, a.get(0x3A), a.get(0x3B), dline))
        got = [k for k in dbg.kids(d) if k[1] in (0x05, 0x34)]
        if [(k[1], k[2].get(0x03)) for k in got] != [(t, n) for t, n, _, _ in kids]:
            fails.append("%s: children %r" % (fname, [(k[1], k[2].get(0x03)) for k in got]))
            continue
        for k, (tag, name, loc, tname) in zip(got, kids):
            if k[2].get(0x02) != loc:
                fails.append("%s.%s: location %r, want %r" % (fname, name, k[2].get(0x02), loc))
            t = dbg.by_off.get(k[2].get(0x49))
            if t is None or t[2].get(0x03) != tname:
                fails.append("%s.%s: type %r, want %s" % (fname, name, t and t[2].get(0x03), tname))
                continue
            if tname == "i64" and (t[1] != 0x24 or t[2].get(0x3E) != 5 or t[2].get(0x0B) != 8):
                fails.append("%s.%s: i64 is not a signed 8-byte base type" % (fname, name))
            if tname == "bool" and (t[1] != 0x24 or t[2].get(0x3E) != 2):
                fails.append("%s.%s: bool is not a boolean base type" % (fname, name))
            if tname == "str":
                p = dbg.by_off.get(t[2].get(0x49))
                c = p and dbg.by_off.get(p[2].get(0x49))
                if t[1] != 0x16 or p is None or p[1] != 0x0F or c is None or c[2].get(0x3E) != 8:
                    fails.append("%s.%s: str is not a typedef of a pointer to characters" % (fname, name))
            if tname == "Point":
                mem = [(m[2].get(0x03), m[2].get(0x38)) for m in dbg.kids(t) if m[1] == 0x0D]
                if t[1] != 0x13 or t[2].get(0x0B) != 16 or mem != [("x", 0), ("y", 8)]:
                    fails.append("%s.%s: Point is %r %r" % (fname, name, t[1], mem))
        if not any(s <= lo < e for s, e, _, _, _, _ in dbg.fdes):
            fails.append("%s: no FDE" % fname)
        rule = dr.cfa_rule(dbg.fdes, lo + 4)
        if rule != (6, 16):
            fails.append("%s: CFA after the prologue %r, want rbp + 16" % (fname, rule))
        first = sorted((r for r in dbg.rows if not r[3] and lo <= r[0] < hi), key=lambda r: r[0])
        if not first or first[0][0] != lo or first[0][2] != dline:
            fails.append("%s: first line row %r, want line %d at 0x%x" % (fname, first[:1], dline, lo))
    for f in fails:
        print("FAIL: " + f)
    if not fails:
        print("pe dwarf vars OK")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
