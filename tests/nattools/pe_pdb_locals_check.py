#!/usr/bin/env python3
"""DB4 / TL5 for the PDB (compiler/pdb.fd, "DB4: the variables"): what
Visual Studio and WinDbg see of a debug build's variables, checked by
running the program and reading every variable through dbghelp.dll from
the PDB alone (not the DWARF). Windows only.

    fid emit tests/bootstrap/test_pe_dbg_vars.fd -g -o t.exe        (t.pdb)
    python tests/nattools/pe_pdb_locals_check.py t.exe tests/bootstrap/test_pe_dbg_vars.fd

The test file's markers and `// dbg <marker>#<hit> name=value ...` lines
are those tests/nattools/pe_dbg_run_check.py reads (a value is a decimal
integer, true/false or "text"; `name.field` reads a field of a struct
variable or of the struct a pointer variable points at). The program runs
under the Windows debugging API (pe_dbg_run_check.WinTarget); breakpoints
come from the image's .debug_line (where a marker's line starts); at each
stop dbghelp is asked for the scope's symbols (SymSetScopeFromAddr,
SymEnumSymbolsW): each must be a local or parameter of the PDB with a
location (S_REGREL32: register + offset; S_REGISTER; S_LOCAL with a
range) and a type (SymGetTypeInfo: base types, char * strings, structs
and pointers to them from the TPI stream), and its value read from the
process must be the expected one. A variable the DWARF itself gives no
location at that address (an optimized build: `--nat`) is reported as
optimized out, not a failure: the PDB says what the DWARF says. Works for
pe_ir images (base 0x140000000) and `--nat` ones (0x400000).
Also checks, without running, that `llvm-pdbutil dump --symbols` (when on
PATH) lists an S_REGREL32/S_REGISTER/S_LOCAL record for every variable the
DWARF names.
Exit 0 when every check holds; 2 when not on Windows."""
import ctypes
import os
import shutil
import struct
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import dbg_run_check as dr  # noqa: E402
import dbg_vars_check as dv  # noqa: E402

IMAGE_BASE = 0x140000000  # pe_ir's; nat_pe's is 0x400000: image_base() reads it
SYMFLAG_REGISTER = 0x8
SYMFLAG_REGREL = 0x10
SYMFLAG_PARAMETER = 0x40
SYMFLAG_LOCAL = 0x80
SYMFLAG_NULL = 0x80000
TI_GET_SYMTAG = 0
TI_GET_SYMNAME = 1
TI_GET_LENGTH = 2
TI_GET_TYPE = 4
TI_GET_BASETYPE = 5
TI_FINDCHILDREN = 7
TI_GET_OFFSET = 10
TI_GET_CHILDRENCOUNT = 13
TAG_UDT = 11
TAG_POINTER = 14
TAG_BASE = 16
# CodeView AMD64 registers -> the CONTEXT names pe_dbg_run_check reads
CV_REGS = {328: "rax", 329: "rbx", 330: "rcx", 331: "rdx", 332: "rsi", 333: "rdi",
           334: "rbp", 335: "rsp", 336: "r8", 337: "r9", 338: "r10", 339: "r11",
           340: "r12", 341: "r13", 342: "r14", 343: "r15"}


class SYMBOL_INFOW(ctypes.Structure):
    _fields_ = [("SizeOfStruct", ctypes.c_ulong), ("TypeIndex", ctypes.c_ulong),
                ("Reserved", ctypes.c_ulonglong * 2), ("Index", ctypes.c_ulong),
                ("Size", ctypes.c_ulong), ("ModBase", ctypes.c_ulonglong),
                ("Flags", ctypes.c_ulong), ("Value", ctypes.c_ulonglong),
                ("Address", ctypes.c_ulonglong), ("Register", ctypes.c_ulong),
                ("Scope", ctypes.c_ulong), ("Tag", ctypes.c_ulong),
                ("NameLen", ctypes.c_ulong), ("MaxNameLen", ctypes.c_ulong),
                ("Name", ctypes.c_wchar * 1)]


ENUM_CB = ctypes.WINFUNCTYPE(ctypes.c_int, ctypes.POINTER(SYMBOL_INFOW), ctypes.c_ulong, ctypes.c_void_p) if os.name == "nt" else None


def image_base(exe):
    """The preferred base of a PE32+ image (its optional header)."""
    data = open(exe, "rb").read(4096)
    e = struct.unpack_from("<I", data, 0x3C)[0]
    return struct.unpack_from("<Q", data, e + 24 + 24)[0]


class Pdb:
    """dbghelp on the image, symbols only (a made-up process handle)."""

    def __init__(self, image):
        self.d = ctypes.WinDLL("dbghelp.dll")
        self.h = ctypes.c_void_p(0x0F1D0DB4)
        # UNDNAME | DEFERRED_LOADS off | LOAD_LINES | FAIL_CRITICAL_ERRORS |
        # EXACT_SYMBOLS: the PDB's GUID and age must match the image
        self.d.SymSetOptions(0x2 | 0x10 | 0x200 | 0x400)
        search = os.path.dirname(os.path.abspath(image))
        if not self.d.SymInitializeW(self.h, ctypes.c_wchar_p(search), False):
            raise OSError("SymInitializeW failed")
        self.d.SymLoadModuleExW.restype = ctypes.c_ulonglong
        self.d.SymLoadModuleExW.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_wchar_p, ctypes.c_wchar_p,
                                            ctypes.c_ulonglong, ctypes.c_ulong, ctypes.c_void_p, ctypes.c_ulong]
        self.base = self.d.SymLoadModuleExW(self.h, None, os.path.abspath(image), None, image_base(image), 0, None, 0)
        if not self.base:
            raise OSError("SymLoadModuleExW failed: %d" % ctypes.GetLastError())
        self.d.SymSetScopeFromAddr.argtypes = [ctypes.c_void_p, ctypes.c_ulonglong]
        self.d.SymEnumSymbolsW.argtypes = [ctypes.c_void_p, ctypes.c_ulonglong, ctypes.c_wchar_p, ENUM_CB, ctypes.c_void_p]
        self.d.SymGetTypeInfo.argtypes = [ctypes.c_void_p, ctypes.c_ulonglong, ctypes.c_ulong, ctypes.c_int, ctypes.c_void_p]

    def scope(self, pc):
        """The symbols of the scope at `pc` (a preferred-base address):
        [(name, flags, register, address, type index)]."""
        if not self.d.SymSetScopeFromAddr(self.h, ctypes.c_ulonglong(pc)):
            return None
        out = []

        def cb(p, size, ctx):
            si = p.contents
            name = ctypes.wstring_at(ctypes.addressof(si) + SYMBOL_INFOW.Name.offset, si.NameLen).rstrip(chr(0))
            out.append((name, si.Flags, si.Register, si.Address, si.TypeIndex))
            return 1
        self.d.SymEnumSymbolsW(self.h, 0, None, ENUM_CB(cb), None)
        return out

    def ti(self, t, what, ctype=ctypes.c_ulong):
        v = ctype(0)
        if not self.d.SymGetTypeInfo(self.h, ctypes.c_ulonglong(self.base), t, what, ctypes.byref(v)):
            return None
        return v.value

    def name(self, t):
        p = ctypes.c_void_p(0)
        if not self.d.SymGetTypeInfo(self.h, ctypes.c_ulonglong(self.base), t, TI_GET_SYMNAME, ctypes.byref(p)):
            return None
        s = ctypes.wstring_at(p.value)
        ctypes.windll.kernel32.LocalFree(p)
        return s

    def members(self, t):
        """A struct type's fields: {name: (offset, type index)}."""
        n = self.ti(t, TI_GET_CHILDRENCOUNT)
        if not n:
            return {}

        class FIND(ctypes.Structure):
            _fields_ = [("Count", ctypes.c_ulong), ("Start", ctypes.c_ulong), ("ChildId", ctypes.c_ulong * n)]
        f = FIND()
        f.Count = n
        if not self.d.SymGetTypeInfo(self.h, ctypes.c_ulonglong(self.base), t, TI_FINDCHILDREN, ctypes.byref(f)):
            return {}
        out = {}
        for c in f.ChildId:
            out[self.name(c)] = (self.ti(c, TI_GET_OFFSET), self.ti(c, TI_GET_TYPE))
        return out


def read_scalar(tgt, pdb, addr, t):
    """The value of type `t` at `addr` as the test file writes it."""
    tag = pdb.ti(t, TI_GET_SYMTAG)
    size = pdb.ti(t, TI_GET_LENGTH, ctypes.c_ulonglong) or 8
    raw = int.from_bytes(tgt.read(addr, size), "little")
    if tag == TAG_POINTER:
        inner = pdb.ti(t, TI_GET_TYPE)
        if pdb.ti(inner, TI_GET_SYMTAG) == TAG_BASE and pdb.ti(inner, TI_GET_LENGTH, ctypes.c_ulonglong) == 1:
            bs = b""
            while len(bs) < 4096:
                c = tgt.read(raw + len(bs), 1)
                if c == b"\0":
                    break
                bs += c
            return '"' + bs.decode("latin-1") + '"'
        return raw
    if tag == TAG_BASE:
        bt = pdb.ti(t, TI_GET_BASETYPE)
        if bt == 10:  # btBool
            return "true" if raw else "false"
        if bt in (6, 13):  # btInt, btLong: signed
            if raw >= 1 << (8 * size - 1):
                raw -= 1 << (8 * size)
        return str(raw)
    return raw


def value_of(tgt, pdb, regs, sym, path):
    name, flags, reg, addr, t = sym
    if flags & SYMFLAG_REGREL:
        r = CV_REGS.get(reg)
        if r is None:
            raise ValueError("register %d not known" % reg)
        off = addr - (1 << 64) if addr >= 1 << 63 else addr
        off = off - (1 << 32) if off >= 1 << 31 else off
        place = regs[r] + off
    elif flags & SYMFLAG_REGISTER:
        r = CV_REGS.get(reg)
        if r is None:
            raise ValueError("register %d not known" % reg)
        place = None
        if not path:
            tag = pdb.ti(t, TI_GET_SYMTAG)
            if tag == TAG_BASE and pdb.ti(t, TI_GET_BASETYPE) == 10:
                return "true" if regs[r] & 0xFF else "false"
            v = regs[r]
            return str(v - (1 << 64) if v >= 1 << 63 else v)
        place = regs[r]  # a pointer in a register
        t = pdb.ti(t, TI_GET_TYPE)
        return read_path(tgt, pdb, place, t, path, through_ptr=True)
    else:
        raise ValueError("no location kind in flags %#x" % flags)
    return read_path(tgt, pdb, place, t, path)


def read_path(tgt, pdb, place, t, path, through_ptr=False):
    for field in path:
        tag = pdb.ti(t, TI_GET_SYMTAG)
        if tag == TAG_POINTER and not through_ptr:
            place = int.from_bytes(tgt.read(place, 8), "little")
            t = pdb.ti(t, TI_GET_TYPE)
            tag = pdb.ti(t, TI_GET_SYMTAG)
        through_ptr = False
        if tag != TAG_UDT:
            raise ValueError("%s: not a struct (tag %r)" % (field, tag))
        ms = pdb.members(t)
        if field not in ms:
            raise ValueError("no field %s (fields %s)" % (field, sorted(ms)))
        off, ft = ms[field]
        place += off
        t = ft
    return read_scalar(tgt, pdb, place, t)


def dwarf_located(dbg, pc, name):
    """Whether the DWARF gives variable `name` of the function at `pc` a
    location there."""
    sub = dbg.sub_at(pc)
    if sub is None:
        return False
    for sc in dbg.scopes_at(sub, pc):
        for k in dbg.kids(sc):
            if k[1] in (dv.DW_TAG_formal_parameter, dv.DW_TAG_variable) and k[2].get(0x03) == name:
                if dbg.expr_at(k, pc):
                    return True
    return False


def pdbutil_check(pdb_path, dbg, fns, fails):
    tool = shutil.which("llvm-pdbutil")
    if tool is None:
        print("SKIP llvm-pdbutil (not on PATH)")
        return
    out = subprocess.run([tool, "dump", "--symbols", pdb_path], capture_output=True, text=True).stdout
    names = set()
    for ln in out.split("\n"):
        for kind in ("S_REGREL32", "S_REGISTER", "S_LOCAL"):
            if " %s " % kind in ln and "`" in ln:
                names.add(ln.split("`")[1])
    # the variables of the test file's functions that have a location
    by = {d[0]: d for d in dbg.dies}
    want = set()
    for d in dbg.dies:
        if d[1] in (dv.DW_TAG_formal_parameter, dv.DW_TAG_variable) and 0x02 in d[2] and d[2].get(0x03):
            p = by.get(d[4])
            while p is not None and p[1] != 0x2E:
                p = by.get(p[4])
            if p is not None and p[2].get(0x03) in fns:
                want.add(d[2][0x03])
    missing = sorted(want - names)
    if missing:
        fails.append("llvm-pdbutil: no local record for %s" % ", ".join(missing))
    else:
        print("ok   llvm-pdbutil: a local record for each of %d variable names of the test's functions in the DWARF" % len(want))


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    if len(args) < 2:
        print(__doc__)
        return 2
    exe, src = args[0], args[1]
    if os.name != "nt":
        print("not on Windows: nothing checked")
        return 2
    import pe_dbg_run_check as pr
    dbg = dr.Debug(exe)
    markers, expects, _fns, _types = dr.parse_test(src)
    fails = []
    pdb_path = os.path.splitext(exe)[0] + ".pdb"
    pdbutil_check(pdb_path, dbg, set(pr.fn_decl_lines(src)), fails)
    pdb = Pdb(exe)
    # a marker's address: its line's row inside one of the test file's
    # functions (a runtime file's row of the same line number is not it)
    fns = set(pr.fn_decl_lines(src))
    bps = {}
    for name, line in markers.items():
        for lo, hi, sd in dbg.subs:
            if sd[2].get(0x03) in fns:
                a = dbg.line_addr(line, lo, hi)
                if a is not None:
                    bps[a] = name
                    break
        else:
            fails.append("@%s: line %d has no row in the test's functions" % (name, line))
    tgt = pr.WinTarget(exe)
    delta = tgt.base - image_base(exe)
    for a in bps:
        tgt.set_bp(a + delta)
    hits = {}
    notes = []
    checked = [0, 0]
    try:
        while True:
            kind, at = tgt.resume()
            if kind != "bp":
                if kind == "exit":
                    if at != 0:
                        fails.append("the program exited with %d" % at)
                    break
                if kind == "crash":
                    fails.append("the program raised exception 0x%x" % at)
                break
            pc = at - delta
            name = bps.get(pc)
            if name is not None:
                hits[name] = hits.get(name, 0) + 1
                for ename, ehit, vals in expects:
                    if ename != name or ehit != hits[name]:
                        continue
                    syms = pdb.scope(pc)
                    if syms is None:
                        fails.append("@%s: dbghelp has no scope at %#x" % (name, pc))
                        continue
                    by = {}
                    for s in syms:
                        by.setdefault(s[0], s)
                    vals_g = tgt.regs()
                    regs = dict(zip(pr.WinTarget.G_ORDER, vals_g))
                    for key, want, _must in vals:
                        checked[0] += 1
                        parts = key.split(".")
                        s = by.get(parts[0])
                        where = "@%s#%d %s" % (name, ehit, key)
                        dwarf_has = dwarf_located(dbg, pc, parts[0])
                        if s is None or (s[1] & SYMFLAG_NULL):
                            if not dwarf_has:
                                # the DWARF has no location here either: the
                                # build keeps no value there (optimized out)
                                notes.append("%s: no location in the DWARF either (optimized out)" % where)
                            elif s is None:
                                fails.append("%s: not among the scope's symbols %s" % (where, sorted(by)))
                            else:
                                fails.append("%s: no value at this address, the DWARF has one" % where)
                            continue
                        if not (s[1] & (SYMFLAG_LOCAL | SYMFLAG_PARAMETER)):
                            fails.append("%s: not a local or parameter (flags %#x)" % (where, s[1]))
                        try:
                            got = value_of(tgt, pdb, regs, s, parts[1:])
                        except (ValueError, OSError) as e:
                            fails.append("%s: %s" % (where, e))
                            continue
                        checked[1] += 1
                        if str(got) != want:
                            fails.append("%s = %s, want %s" % (where, got, want))
            tgt.clear_bp(at)
            r = tgt.step()
            if r[0] != "step":
                if r[0] == "exit":
                    break
                fails.append("single step at 0x%x: %r" % (at, r))
                break
            tgt.set_bp(at)
    finally:
        tgt.kill()
    for n in notes:
        print("note: " + n)
    for f in fails:
        print("FAIL: " + f)
    print("%d of %d expected values read through the PDB (%d optimized out in the DWARF too)" % (checked[1], checked[0], len(notes)))
    if not fails and checked[0] > 0:
        print("pe pdb locals OK")
        return 0
    return 1


if __name__ == "__main__":
    sys.exit(main())
