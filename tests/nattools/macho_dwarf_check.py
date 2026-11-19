#!/usr/bin/env python3
"""macho_dwarf_check.py IMAGE [--file NAME]... [--fn NAME]...: the DWARF of
a Mach-O executable of the machine layer (TL5, docs/roadmap/
competitive-roadmap.md; compiler/nat_enc.fd nat_macho_debug,
nat_macho_dwarf, nat_macho_dwarf_seg).

Checks, as lldb and dsymutil read them:
  - scripts/macho-check.py passes (structure, binds, code signature over
    the whole file including the DWARF);
  - a __DWARF segment lies between __DATA and __LINKEDIT, page aligned,
    read-only, its vmsize covering its file size, with __debug_abbrev,
    __debug_info and __debug_line inside it (flags S_ATTR_DEBUG);
  - __debug_info is one DWARF 4 compile unit whose functions
    (DW_TAG_subprogram) start inside __text; each --fn NAME is among them;
  - the __debug_line program decodes (standard, extended and special
    opcodes), every row's address is inside __text, every sequence ends,
    and each --file NAME (a base name) is in the file table with at least
    one row against it;
  - when llvm-dwarfdump is on PATH, `llvm-dwarfdump --debug-line` reads
    the table without an error and `--verify` passes.
Exit 0 when every check passes.
"""
import os
import shutil
import struct
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))

FAILS = []


def bad(msg):
    FAILS.append(msg)
    print("  FAIL " + msg)


def u16(b, o):
    return struct.unpack_from("<H", b, o)[0]


def u32(b, o):
    return struct.unpack_from("<I", b, o)[0]


def u64(b, o):
    return struct.unpack_from("<Q", b, o)[0]


def uleb(b, o):
    v, s = 0, 0
    while True:
        x = b[o]
        o += 1
        v |= (x & 0x7F) << s
        s += 7
        if x < 0x80:
            return v, o


def sleb(b, o):
    v, s = 0, 0
    while True:
        x = b[o]
        o += 1
        v |= (x & 0x7F) << s
        s += 7
        if x < 0x80:
            if x & 0x40:
                v -= 1 << s
            return v, o


def cstr(b, o):
    e = b.index(b"\0", o)
    return b[o:e].decode("utf-8", "replace"), e + 1


def segments(d):
    ncmds = u32(d, 16)
    at = 32
    segs = []
    for _ in range(ncmds):
        cmd, size = u32(d, at), u32(d, at + 4)
        if cmd == 0x19:
            name = d[at + 8:at + 24].rstrip(b"\0").decode("latin-1")
            vmaddr, vmsize, fo, fs, maxp, initp, nsec, _ = struct.unpack_from("<QQQQiiII", d, at + 24)
            sects = []
            for k in range(nsec):
                so = at + 72 + 80 * k
                sn = d[so:so + 16].rstrip(b"\0").decode("latin-1")
                gn = d[so + 16:so + 32].rstrip(b"\0").decode("latin-1")
                addr, ssz, soff, al, _, _, fl = struct.unpack_from("<QQIIIII", d, so + 32)
                sects.append(dict(name=sn, seg=gn, addr=addr, size=ssz, offset=soff, flags=fl))
            segs.append(dict(name=name, vmaddr=vmaddr, vmsize=vmsize, fileoff=fo, filesize=fs,
                             maxp=maxp, initp=initp, sects=sects))
        at += size
    return segs


# DWARF 4 forms: how many bytes (or how to skip) each takes
def skip_form(b, o, form, addr_size=8):
    fixed = {0x01: addr_size, 0x05: 2, 0x06: 4, 0x07: 8, 0x0B: 1, 0x0C: 1, 0x11: 1,
             0x12: 2, 0x13: 4, 0x14: 8, 0x17: 4, 0x10: 4, 0x0E: 4, 0x19: 0, 0x20: 8}
    if form in fixed:
        return o + fixed[form]
    if form == 0x08:
        return cstr(b, o)[1]
    if form in (0x0F, 0x15):
        return uleb(b, o)[1]
    if form == 0x0D:
        return sleb(b, o)[1]
    if form in (0x0A, 0x18):
        n, o = uleb(b, o)
        return o + n
    if form == 0x09:
        n, o = uleb(b, o)
        return o + n
    if form == 0x03:
        return o + 2 + u16(b, o)
    if form == 0x04:
        return o + 4 + u32(b, o)
    raise ValueError("form 0x%x" % form)


def read_abbrevs(b):
    out, o = {}, 0
    while o < len(b):
        code, o = uleb(b, o)
        if code == 0:
            break
        tag, o = uleb(b, o)
        children = b[o]
        o += 1
        attrs = []
        while True:
            a, o = uleb(b, o)
            f, o = uleb(b, o)
            if a == 0 and f == 0:
                break
            attrs.append((a, f))
        out[code] = (tag, children, attrs)
    return out


def read_attr(b, o, form):
    if form == 0x08:
        return cstr(b, o)
    if form == 0x01 or form == 0x07:
        return u64(b, o), o + 8
    if form == 0x05:
        return u16(b, o), o + 2
    if form in (0x06, 0x17):
        return u32(b, o), o + 4
    if form == 0x0B:
        return b[o], o + 1
    if form == 0x19:
        return True, o
    return None, skip_form(b, o, form)


def read_info(b, abbrevs):
    if len(b) < 11:
        bad("__debug_info too short")
        return None, []
    unit_len, ver = u32(b, 0), u16(b, 4)
    if unit_len + 4 != len(b):
        bad("__debug_info unit length %d for %d bytes" % (unit_len, len(b)))
    if ver != 4:
        bad("__debug_info version %d, not 4" % ver)
    if b[10] != 8:
        bad("address size %d" % b[10])
    o, cu, fns = 11, None, []
    end = min(len(b), unit_len + 4)
    while o < end:
        code, o = uleb(b, o)
        if code == 0:
            continue
        if code not in abbrevs:
            bad("abbrev code %d not in __debug_abbrev" % code)
            return cu, fns
        tag, _, attrs = abbrevs[code]
        die = {}
        for a, f in attrs:
            v, o = read_attr(b, o, f)
            die[a] = v
        if tag == 0x11:
            cu = die
        elif tag == 0x2E:
            fns.append(die)
    return cu, fns


def read_lines(b):
    """[(addr, file, line, end)], [file names]"""
    unit_len, ver, hlen = u32(b, 0), u16(b, 4), u32(b, 6)
    if ver not in (2, 3, 4):
        bad("__debug_line version %d" % ver)
    o = 10
    min_inst, o = b[o], o + 1
    if ver >= 4:
        o += 1
    o += 1
    line_base = struct.unpack_from("b", b, o)[0]
    line_range, opcode_base = b[o + 1], b[o + 2]
    o += 3
    std_len = list(b[o:o + opcode_base - 1])
    o += opcode_base - 1
    while b[o] != 0:
        _, o = cstr(b, o)
    o += 1
    files = []
    while b[o] != 0:
        name, o = cstr(b, o)
        for _ in range(3):
            _, o = uleb(b, o)
        files.append(name)
    prog = 10 + hlen
    end = unit_len + 4
    rows = []
    o = prog
    addr, file, line = 0, 1, 1
    while o < end:
        op = b[o]
        o += 1
        if op >= opcode_base:
            adj = op - opcode_base
            addr += (adj // line_range) * min_inst
            line += line_base + adj % line_range
            rows.append((addr, file, line, False))
        elif op == 0:
            n, o = uleb(b, o)
            eop = b[o]
            if eop == 1:
                rows.append((addr, file, line, True))
                addr, file, line = 0, 1, 1
            elif eop == 2:
                addr = u64(b, o + 1)
            o += n
        elif op == 1:
            rows.append((addr, file, line, False))
        elif op == 2:
            v, o = uleb(b, o)
            addr += v * min_inst
        elif op == 3:
            v, o = sleb(b, o)
            line += v
        elif op == 4:
            file, o = uleb(b, o)
        elif op == 8:
            addr += ((255 - opcode_base) // line_range) * min_inst
        elif op == 9:
            addr += u16(b, o)
            o += 2
        else:
            for _ in range(std_len[op - 1]):
                _, o = uleb(b, o)
    return rows, files


def main():
    args = sys.argv[1:]
    if not args:
        print(__doc__)
        return 2
    image = args[0]
    want_files, want_fns = [], []
    i = 1
    while i < len(args):
        if args[i] == "--file" and i + 1 < len(args):
            want_files.append(args[i + 1])
            i += 2
        elif args[i] == "--fn" and i + 1 < len(args):
            want_fns.append(args[i + 1])
            i += 2
        else:
            print("unknown argument " + args[i])
            return 2
    d = open(image, "rb").read()
    print("macho_dwarf_check: " + image)
    r = subprocess.run([sys.executable, os.path.join(ROOT, "scripts", "macho-check.py"), image, "--quiet"],
                       capture_output=True, text=True)
    if r.returncode != 0:
        bad("macho-check.py: " + (r.stdout + r.stderr).strip().replace("\n", "; "))
    if len(d) < 32 or u32(d, 0) != 0xFEEDFACF:
        bad("not a 64-bit Mach-O")
        return 1
    page = 0x4000 if u32(d, 4) == 0x0100000C else 0x1000
    segs = segments(d)
    names = [g["name"] for g in segs]
    dw = [g for g in segs if g["name"] == "__DWARF"]
    if not dw:
        bad("no __DWARF segment (segments: %s)" % ", ".join(names))
        return 1
    dw = dw[0]
    if "__DATA" in names and "__LINKEDIT" in names:
        if not names.index("__DATA") < names.index("__DWARF") < names.index("__LINKEDIT"):
            bad("__DWARF not between __DATA and __LINKEDIT: %s" % ", ".join(names))
    le = [g for g in segs if g["name"] == "__LINKEDIT"]
    if le and dw["fileoff"] + dw["filesize"] > le[0]["fileoff"]:
        bad("__DWARF overlaps __LINKEDIT in the file")
    if dw["fileoff"] % page or dw["vmaddr"] % page:
        bad("__DWARF not page aligned")
    if dw["vmsize"] < dw["filesize"]:
        bad("__DWARF vmsize < filesize (dyld refuses it)")
    if dw["initp"] & 6 or dw["maxp"] & 6:
        bad("__DWARF writable or executable")
    sect = {}
    for x in dw["sects"]:
        sect[x["name"]] = x
        if not x["flags"] & 0x02000000:
            bad("%s without S_ATTR_DEBUG" % x["name"])
        if x["offset"] < dw["fileoff"] or x["offset"] + x["size"] > dw["fileoff"] + dw["filesize"]:
            bad("%s outside __DWARF" % x["name"])
    for n in ("__debug_abbrev", "__debug_info", "__debug_line"):
        if n not in sect:
            bad("no " + n)
    if FAILS:
        return 1
    text = None
    for g in segs:
        for x in g["sects"]:
            if x["name"] == "__text":
                text = (x["addr"], x["addr"] + x["size"])
    if text is None:
        bad("no __text")
        return 1

    def body(n):
        x = sect[n]
        return d[x["offset"]:x["offset"] + x["size"]]

    abbrevs = read_abbrevs(body("__debug_abbrev"))
    cu, fns = read_info(body("__debug_info"), abbrevs)
    if cu is None:
        bad("no compile unit")
    else:
        print("  compile unit %s (%s), %d functions" % (cu.get(0x03), cu.get(0x25), len(fns)))
        if cu.get(0x10) != 0:
            bad("stmt_list %r, not 0" % cu.get(0x10))
    fn_names = set()
    for f in fns:
        nm = f.get(0x03)
        fn_names.add(nm)
        if 0x6E in f:
            fn_names.add(f[0x6E])
        lo = f.get(0x11)
        if lo is None or not text[0] <= lo < text[1]:
            bad("function %s low_pc %r outside __text" % (nm, lo))
    for w in want_fns:
        if w not in fn_names:
            bad("function %s not in __debug_info" % w)
    rows, files = read_lines(body("__debug_line"))
    print("  line table: %d rows, files %s" % (len(rows), ", ".join(files)))
    if not rows:
        bad("no line rows")
    open_seq = False
    per_file = {}
    for addr, fi, ln, end in rows:
        if not text[0] <= addr <= text[1]:
            bad("row address 0x%x outside __text" % addr)
            break
        if end:
            open_seq = False
            continue
        open_seq = True
        if fi < 1 or fi > len(files):
            bad("row file %d not in the file table" % fi)
            break
        if ln <= 0:
            bad("row line %d" % ln)
            break
        base = os.path.basename(files[fi - 1].replace("\\", "/"))
        per_file[base] = per_file.get(base, 0) + 1
    if open_seq:
        bad("the last sequence has no end_sequence")
    for w in want_files:
        if per_file.get(w, 0) == 0:
            bad("no rows for file %s (rows per file: %r)" % (w, per_file))
    dd = shutil.which("llvm-dwarfdump")
    if dd:
        r = subprocess.run([dd, "--debug-line", image], capture_output=True, text=True)
        if r.returncode != 0 or "error" in (r.stdout + r.stderr).lower():
            bad("llvm-dwarfdump --debug-line: " + (r.stderr or r.stdout)[:400])
        r = subprocess.run([dd, "--verify", image], capture_output=True, text=True)
        if r.returncode != 0:
            bad("llvm-dwarfdump --verify: " + (r.stdout + r.stderr)[-400:])
    else:
        print("  (llvm-dwarfdump not on PATH: not run)")
    if FAILS:
        print("macho_dwarf_check: %d failed" % len(FAILS))
        return 1
    print("macho_dwarf_check: ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
