#!/usr/bin/env python3
"""pe_pdb_check.py: the PDB of a Windows image (own-backend roadmap DB3,
compiler/pdb.fd), read three ways: an MSF/PDB reader written here from the
format (LLVM's llvm/docs/PDB), dbghelp.dll (what WinDbg and Visual Studio's
symbol engine use; Windows only) and llvm-pdbutil / llvm-symbolizer when on
PATH.

  python3 tests/nattools/pe_pdb_check.py --unit --fid FID [SRC]
      emits SRC (default tests/bootstrap/hello.fd) with FID, runs
      tests/nattools/pdb_unit.fd on it (pdb.fd's pdb_attach on the image's
      COFF functions, with made-up line rows) and checks that the PDB holds
      exactly what the unit printed: every procedure (S_GPROC32 at its
      offset and size, S_PROCREF in the globals), every public (S_PUB32),
      every line row (C13, with its file), the RSDS record of the new image
      naming the PDB with the same GUID and age, and that the new image
      still runs with the same output.
  python3 tests/nattools/pe_pdb_check.py --emit --fid FID [SRC]
      `FID emit SRC -g -o X.exe` (the backend's hook: pe_ir, or with --nat
      the machine layer) must write X.pdb; checks the debug directory, the
      PDB's procedures against the image's COFF symbols and its line rows
      against the image's DWARF .debug_line (same address -> same line),
      and that X.exe runs as `FID emit SRC` without -g does.
  python3 tests/nattools/pe_pdb_check.py IMAGE [PDB]
      checks an existing image and its PDB (as --emit, without building).

Exit 0 when every check holds; prints "ok ..." / "FAIL ..." lines, and
"SKIP ..." for a reader that is not available (dbghelp off Windows,
llvm-pdbutil not on PATH).
"""
import ctypes
import os
import shutil
import struct
import subprocess
import sys
import tempfile

FAILS = []


def ok(msg):
    print("ok   " + msg)


def bad(msg):
    print("FAIL " + msg)
    FAILS.append(msg)


def u16(b, o):
    return struct.unpack_from("<H", b, o)[0]


def u32(b, o):
    return struct.unpack_from("<I", b, o)[0]


def cstr(b, o):
    e = b.index(b"\0", o)
    return b[o:e].decode("latin-1"), e + 1


def hash_v1(s):
    """hashStringV1 of the PDB reference implementation."""
    data = s.encode("latin-1") if isinstance(s, str) else s
    r = 0
    n = len(data)
    i = 0
    while i + 4 <= n:
        r ^= struct.unpack_from("<I", data, i)[0]
        i += 4
    if n - i >= 2:
        r ^= struct.unpack_from("<H", data, i)[0]
        i += 2
    if n - i == 1:
        r ^= data[i]
    r |= 0x20202020
    r ^= r >> 11
    return (r ^ (r >> 16)) & 0xFFFFFFFF


def guid_text(g):
    d1, d2, d3 = struct.unpack_from("<IHH", g, 0)
    return "%08X-%04X-%04X-%s-%s" % (d1, d2, d3, g[8:10].hex().upper(), g[10:16].hex().upper())


# --- PE ----------------------------------------------------------------------

class Pe:
    def __init__(self, data):
        self.data = data
        e = u32(data, 0x3C)
        assert data[e:e + 4] == b"PE\0\0", "not a PE image"
        self.nsec = u16(data, e + 6)
        self.symptr, self.nsyms = struct.unpack_from("<II", data, e + 12)
        optsz = u16(data, e + 20)
        self.opt = e + 24
        self.image_base = struct.unpack_from("<Q", data, self.opt + 24)[0]
        self.size_of_image = u32(data, self.opt + 56)
        self.sections = []
        at = self.opt + optsz
        strtab = self.symptr + 18 * self.nsyms if self.symptr else 0
        for _ in range(self.nsec):
            raw = data[at:at + 8].rstrip(b"\0").decode("latin-1")
            vsize, rva, rawsize, rawoff = struct.unpack_from("<IIII", data, at + 8)
            name = raw
            if raw.startswith("/") and strtab:
                try:
                    name, _ = cstr(data, strtab + int(raw[1:]))
                except ValueError:
                    pass
            self.sections.append((name, vsize, rva, rawsize, rawoff, u32(data, at + 36)))
            at += 40

    def section(self, want):
        for s in self.sections:
            if s[0] == want:
                n, vsize, rva, rawsize, rawoff, _ = s
                return self.data[rawoff:rawoff + min(vsize, rawsize) if vsize else rawsize], rva
        return None, None

    def rva_to_off(self, rva):
        for n, vsize, srva, rawsize, rawoff, _ in self.sections:
            if srva <= rva < srva + max(vsize, rawsize):
                return rawoff + rva - srva
        return None

    def debug_dir(self):
        rva, size = struct.unpack_from("<II", self.data, self.opt + 112 + 6 * 8)
        if rva == 0:
            return None
        o = self.rva_to_off(rva)
        out = []
        for k in range(size // 28):
            ch, ts, maj, mn, typ, sz, addr, ptr = struct.unpack_from("<IIHHIIII", self.data, o + 28 * k)
            out.append((typ, sz, addr, ptr))
        return out

    def rsds(self):
        for typ, sz, addr, ptr in self.debug_dir() or []:
            if typ == 2:
                rec = self.data[ptr:ptr + sz]
                if rec[:4] != b"RSDS":
                    return None
                path = rec[24:].split(b"\0")[0].decode("latin-1")
                return rec[4:20], u32(rec, 20), path, addr, ptr
        return None

    def coff_functions(self):
        out = {}
        if not self.symptr:
            return out
        strtab = self.symptr + 18 * self.nsyms
        i = 0
        while i < self.nsyms:
            at = self.symptr + 18 * i
            if u32(self.data, at) == 0:
                name, _ = cstr(self.data, strtab + u32(self.data, at + 4))
            else:
                name = self.data[at:at + 8].split(b"\0")[0].decode("latin-1")
            value = u32(self.data, at + 8)
            sec = u16(self.data, at + 12)
            naux = self.data[at + 17]
            if sec == 1:
                out.setdefault(name, value)
            i += 1 + naux
        return out


# --- DWARF .debug_line (version 2-4) ------------------------------------------

def uleb(b, o):
    r = 0
    s = 0
    while True:
        x = b[o]
        o += 1
        r |= (x & 0x7F) << s
        s += 7
        if x < 0x80:
            return r, o


def sleb(b, o):
    r = 0
    s = 0
    while True:
        x = b[o]
        o += 1
        r |= (x & 0x7F) << s
        s += 7
        if x < 0x80:
            if x & 0x40:
                r -= 1 << s
            return r, o


def dwarf_rows(line):
    """[(address, line, file name, end_sequence)] of every unit."""
    rows = []
    o = 0
    while o + 4 <= len(line):
        ulen = u32(line, o)
        end = o + 4 + ulen
        ver = u16(line, o + 4)
        hlen = u32(line, o + 6)
        p = o + 10
        prog = p + hlen
        min_inst = line[p]
        p += 1
        if ver >= 4:
            p += 1
        p += 1
        line_base = struct.unpack_from("<b", line, p)[0]
        line_range = line[p + 1]
        opcode_base = line[p + 2]
        p += 3
        p += opcode_base - 1
        while line[p] != 0:
            _, p = cstr(line, p)
        p += 1
        files = []
        while line[p] != 0:
            n, p = cstr(line, p)
            _, p = uleb(line, p)
            _, p = uleb(line, p)
            _, p = uleb(line, p)
            files.append(n)
        p = prog
        addr, fileno, ln = 0, 1, 1
        while p < end:
            op = line[p]
            p += 1
            if op >= opcode_base:
                adj = op - opcode_base
                addr += (adj // line_range) * min_inst
                ln += line_base + adj % line_range
                rows.append((addr, ln, files[fileno - 1] if 0 < fileno <= len(files) else "?", False))
            elif op == 0:
                n, p = uleb(line, p)
                sub = line[p]
                if sub == 1:
                    rows.append((addr, ln, files[fileno - 1] if 0 < fileno <= len(files) else "?", True))
                    addr, fileno, ln = 0, 1, 1
                elif sub == 2:
                    addr = struct.unpack_from("<Q", line, p + 1)[0] if n == 9 else u32(line, p + 1)
                p += n
            elif op == 1:
                rows.append((addr, ln, files[fileno - 1] if 0 < fileno <= len(files) else "?", False))
            elif op == 2:
                v, p = uleb(line, p)
                addr += v * min_inst
            elif op == 3:
                v, p = sleb(line, p)
                ln += v
            elif op == 4:
                fileno, p = uleb(line, p)
            elif op == 5:
                _, p = uleb(line, p)
            elif op == 8:
                addr += ((255 - opcode_base) // line_range) * min_inst
            elif op == 9:
                addr += u16(line, p)
                p += 2
            elif op in (6, 7, 10, 11):
                pass
            elif op == 12:
                _, p = uleb(line, p)
        o = end
    return rows


# --- PDB ---------------------------------------------------------------------

MAGIC = b"Microsoft C/C++ MSF 7.00\r\n\x1aDS\0\0\0"


class Pdb:
    """A PDB read from the format alone; raises AssertionError on what a
    reader would reject."""

    def __init__(self, data):
        self.data = data
        assert data[:32] == MAGIC, "MSF magic"
        bs, fpm, nblocks, dirbytes, _, mapaddr = struct.unpack_from("<IIIIII", data, 32)
        assert bs == 4096, "block size %d" % bs
        assert fpm in (1, 2), "free block map block %d" % fpm
        assert len(data) == nblocks * bs, "file size %d != %d blocks" % (len(data), nblocks)
        assert 0 < mapaddr < nblocks, "block map address"
        assert dirbytes % 4 == 0, "directory size"
        ndir = (dirbytes + bs - 1) // bs
        dblocks = struct.unpack_from("<%dI" % ndir, data, mapaddr * bs)
        d = b"".join(data[b * bs:(b + 1) * bs] for b in dblocks)[:dirbytes]
        nst = u32(d, 0)
        sizes = struct.unpack_from("<%dI" % nst, d, 4)
        p = 4 + 4 * nst
        self.streams = []
        used = {0, 1, 2, mapaddr} | set(dblocks)
        for sz in sizes:
            if sz == 0xFFFFFFFF:
                self.streams.append(None)
                continue
            nb = (sz + bs - 1) // bs
            bl = struct.unpack_from("<%dI" % nb, d, p)
            p += 4 * nb
            for b in bl:
                assert 3 <= b < nblocks and b % bs not in (1, 2), "stream block %d" % b
                assert b not in used, "block %d used twice" % b
                used.add(b)
            self.streams.append(b"".join(data[b * bs:(b + 1) * bs] for b in bl)[:sz])
        assert p == dirbytes, "directory has %d bytes left" % (dirbytes - p)
        # free block map: a 0 bit for each used block
        fpmb = data[fpm * bs:(fpm + 1) * bs]
        for b in used:
            if b < 8 * bs:
                assert not (fpmb[b // 8] >> (b % 8)) & 1, "block %d marked free" % b
        self.read_info()
        self.read_dbi()
        self.read_names()
        self.read_modules()
        self.read_symrecs()

    def read_info(self):
        s = self.streams[1]
        ver, self.signature, self.age = struct.unpack_from("<III", s, 0)
        assert ver == 20000404, "info version %d" % ver
        self.guid = s[12:28]
        p = 28
        n = u32(s, p)
        buf = s[p + 4:p + 4 + n]
        p += 4 + n
        size, cap = struct.unpack_from("<II", s, p)
        p += 8
        assert cap > 0 and size <= cap * 2 // 3 + 1, "named stream map size"
        pw = u32(s, p)
        present = struct.unpack_from("<%dI" % pw, s, p + 4)
        p += 4 + 4 * pw
        dw = u32(s, p)
        p += 4 + 4 * dw
        self.named = {}
        bits = [i for i in range(32 * pw) if (present[i // 32] >> (i % 32)) & 1]
        assert len(bits) == size, "present bits"
        for bucket in bits:
            k, v = struct.unpack_from("<II", s, p)
            p += 8
            name, _ = cstr(buf, k)
            self.named[name] = v
            assert (hash_v1(name) & 0xFFFF) % cap == bucket, "named stream %s in bucket %d" % (name, bucket)
        self.features = list(struct.unpack_from("<%dI" % ((len(s) - p) // 4), s, p))
        assert 20140508 in self.features, "feature VC140"
        for idx in (2, 4):
            t = self.streams[idx]
            (tver, hsz, tb, te, trb, hs, has, hks, nbk) = struct.unpack_from("<IIIIIHHII", t, 0)
            assert tver == 20040203 and hsz == 56 and hks == 4 and 0x1000 <= nbk < 0x40000, "TPI/IPI header %d" % idx
            assert len(t) == 56 + trb, "TPI/IPI size %d" % idx

    def read_dbi(self):
        s = self.streams[3]
        h = struct.unpack_from("<iIIHHHHHHiiiiiIiiHHI", s, 0)
        (sig, ver, age, self.gsi, build, self.psi, _, self.symrec, _, modi, sc, smap, finfo, tsm, _, dbgh, ec, flags, machine, _) = h
        assert sig == -1 and ver == 19990903, "DBI version"
        assert age == self.age, "DBI age %d != PDB age %d" % (age, self.age)
        assert build & 0x8000, "DBI new format bit"
        assert machine == 0x8664, "machine"
        assert len(s) == 64 + modi + sc + smap + finfo + tsm + dbgh + ec, "DBI length is not the sum of its substreams"
        for v in (modi, sc, smap, finfo, tsm):
            assert v % 4 == 0, "DBI substream not aligned"
        p = 64
        self.modules = []
        end = p + modi
        while p < end:
            sc_sec, _, sc_off, sc_size, sc_ch, sc_mod = struct.unpack_from("<HHiiIH", s, p + 4)
            flags_, stream, symb, c11, c13, nfiles = struct.unpack_from("<HHIIIH", s, p + 32)
            name, q = cstr(s, p + 64)
            obj, q = cstr(s, q)
            q = (q + 3) & ~3
            self.modules.append(dict(name=name, obj=obj, stream=stream, sym=symb, c11=c11, c13=c13, nfiles=nfiles, sc=(sc_sec, sc_off, sc_size)))
            p = q
        assert p == end, "module info"
        ver_sc = u32(s, p)
        assert ver_sc == 0xEFFE0000 + 19970605, "section contribution version"
        self.contribs = []
        q = p + 4
        while q < p + sc:
            sec, _, off, size, ch, mod = struct.unpack_from("<HHiiIH", s, q)
            self.contribs.append((sec, off, size, mod))
            q += 28
        p += sc
        cnt, cntlog = struct.unpack_from("<HH", s, p)
        assert smap == 4 + 20 * cnt, "section map size"
        p += smap
        nm, nsrc = struct.unpack_from("<HH", s, p)
        assert nm == len(self.modules), "file info module count"
        firsts = struct.unpack_from("<%dH" % nm, s, p + 4)
        counts = struct.unpack_from("<%dH" % nm, s, p + 4 + 2 * nm)
        q = p + 4 + 4 * nm
        offs = struct.unpack_from("<%dI" % sum(counts), s, q)
        namebuf = q + 4 * sum(counts)
        self.mod_files = []
        for m in range(nm):
            self.mod_files.append([cstr(s, namebuf + offs[firsts[m] + k])[0] for k in range(counts[m])])
        p += finfo + tsm
        # EC names: a string table
        self.check_string_table(s[p:p + ec])
        p += ec
        hdr = struct.unpack_from("<%dH" % (dbgh // 2), s, p)
        self.sechdr = None
        if len(hdr) > 5 and hdr[5] != 0xFFFF:
            sh = self.streams[hdr[5]]
            self.sechdr = [struct.unpack_from("<8sIIIIIIHHI", sh, 40 * i) for i in range(len(sh) // 40)]

    def check_string_table(self, t):
        sig, hv, n = struct.unpack_from("<III", t, 0)
        assert sig == 0xEFFEEFFE and hv in (1, 2), "string table header"
        buf = t[12:12 + n]
        p = 12 + n
        nb = u32(t, p)
        assert nb > 0, "string table buckets"
        buckets = struct.unpack_from("<%dI" % nb, t, p + 4)
        p += 4 + 4 * nb
        count = u32(t, p)
        assert p + 4 == len(t), "string table: bytes after the name count"
        assert count == sum(1 for b in buckets if b), "string table name count"
        for b in buckets:
            if b:
                name, _ = cstr(buf, b)
                # the name must be found by probing from its hash
                h = hash_v1(name) % nb
                k = h
                while buckets[k] != b:
                    assert buckets[k] != 0, "string table: %s not reachable" % name
                    k = (k + 1) % nb
        return buf

    def read_names(self):
        assert "/names" in self.named, "no /names stream"
        self.names_buf = self.check_string_table(self.streams[self.named["/names"]])

    def read_modules(self):
        self.procs = {}
        self.lines = []          # (section, offset, line, file)
        for mi, m in enumerate(self.modules):
            s = self.streams[m["stream"]]
            assert len(s) == m["sym"] + m["c11"] + m["c13"] + 4, "module stream size"
            assert u32(s, 0) == 4, "module signature"
            p = 4
            ends = []
            while p < m["sym"]:
                rl, kind = struct.unpack_from("<HH", s, p)
                assert (rl + 2) % 4 == 0, "symbol record not aligned"
                if kind == 0x1110:
                    parent, end, nxt, size, dbs, dbe, typ, off, seg, fl = struct.unpack_from("<IIIIIIIIHB", s, p + 4)
                    name, _ = cstr(s, p + 39)
                    ends.append(end)
                    rl2, k2 = struct.unpack_from("<HH", s, end)
                    assert k2 == 6, "S_GPROC32 %s: end does not point at S_END" % name
                    self.procs[name] = dict(seg=seg, off=off, size=size, mod_off=p, mod=mi)
                p += rl + 2
            assert p == m["sym"], "module symbols overrun"
            c13 = s[m["sym"] + m["c11"]:m["sym"] + m["c11"] + m["c13"]]
            q = 0
            checks = {}
            frags = []
            while q < len(c13):
                kind, ln = struct.unpack_from("<II", c13, q)
                data = c13[q + 8:q + 8 + ln]
                if kind == 0xF4:
                    r = 0
                    while r < len(data):
                        noff, csz, ckind = struct.unpack_from("<IBB", data, r)
                        checks[r] = cstr(self.names_buf, noff)[0]
                        r = (r + 6 + csz + 3) & ~3
                elif kind == 0xF2:
                    frags.append(data)
                q = (q + 8 + ln + 3) & ~3
            for data in frags:
                off, seg, fl, csize = struct.unpack_from("<IHHI", data, 0)
                r = 12
                while r < len(data):
                    nidx, nl, bsz = struct.unpack_from("<III", data, r)
                    assert nidx in checks, "line block names a file checksum that is not there"
                    for k in range(nl):
                        lo, lf = struct.unpack_from("<II", data, r + 12 + 8 * k)
                        assert lo < csize, "line offset past the code"
                        self.lines.append((seg, off + lo, lf & 0xFFFFFF, checks[nidx]))
                    r += bsz

    def read_gsi(self, s, p):
        vs, vh, hr, nb = struct.unpack_from("<IIII", s, p)
        assert vs == 0xFFFFFFFF and vh == 0xEFFE0000 + 19990810, "GSI header"
        n = hr // 8
        recs = [struct.unpack_from("<II", s, p + 16 + 8 * i) for i in range(n)]
        q = p + 16 + hr
        bitmap = struct.unpack_from("<129I", s, q)
        chains = struct.unpack_from("<%dI" % ((nb - 516) // 4), s, q + 516)
        assert nb == 516 + 4 * len(chains), "GSI bucket bytes"
        buckets = [b for b in range(4096) if (bitmap[b // 32] >> (b % 32)) & 1]
        assert len(buckets) == len(chains), "GSI bitmap vs chains"
        out = []
        for bi, b in enumerate(buckets):
            start = chains[bi] // 12
            stop = chains[bi + 1] // 12 if bi + 1 < len(chains) else n
            for k in range(start, stop):
                off = recs[k][0] - 1
                name = self.record_name(off)
                assert hash_v1(name) % 4096 == b, "GSI: %s in bucket %d" % (name, b)
                out.append(off)
        assert len(out) == n, "GSI chains do not cover the records"
        return out, p + 16 + hr + nb

    def record_name(self, off):
        rl, kind = struct.unpack_from("<HH", self.symrecs, off)
        if kind == 0x110E:
            return cstr(self.symrecs, off + 14)[0]
        if kind == 0x1125:
            return cstr(self.symrecs, off + 14)[0]
        raise AssertionError("record kind %#x at %d" % (kind, off))

    def read_symrecs(self):
        self.symrecs = self.streams[self.symrec]
        g, end = self.read_gsi(self.streams[self.gsi], 0)
        assert end == len(self.streams[self.gsi]), "globals stream size"
        self.procrefs = {}
        for off in g:
            rl, kind, sumn, symoff, mod = struct.unpack_from("<HHIIH", self.symrecs, off)
            assert kind == 0x1125, "global is not S_PROCREF"
            name = cstr(self.symrecs, off + 14)[0]
            self.procrefs[name] = (symoff, mod)
        ps = self.streams[self.psi]
        symhash, amap = struct.unpack_from("<II", ps, 0)
        pubs, end = self.read_gsi(ps, 28)
        assert end == 28 + symhash, "publics hash size"
        addrs = struct.unpack_from("<%dI" % (amap // 4), ps, end)
        assert sorted(addrs) == sorted(pubs), "address map vs hash records"
        self.publics = {}
        last = None
        for off in addrs:
            rl, kind, fl, o, seg = struct.unpack_from("<HHIIH", self.symrecs, off)
            assert kind == 0x110E, "public is not S_PUB32"
            name = cstr(self.symrecs, off + 14)[0]
            key = (seg, o, name.encode("latin-1"))
            assert last is None or last <= key, "address map not sorted"
            last = key
            self.publics[name] = (seg, o, fl)


# --- dbghelp --------------------------------------------------------------------

class SYMBOL_INFOW(ctypes.Structure):
    _fields_ = [("SizeOfStruct", ctypes.c_ulong), ("TypeIndex", ctypes.c_ulong),
                ("Reserved", ctypes.c_ulonglong * 2), ("Index", ctypes.c_ulong),
                ("Size", ctypes.c_ulong), ("ModBase", ctypes.c_ulonglong),
                ("Flags", ctypes.c_ulong), ("Value", ctypes.c_ulonglong),
                ("Address", ctypes.c_ulonglong), ("Register", ctypes.c_ulong),
                ("Scope", ctypes.c_ulong), ("Tag", ctypes.c_ulong),
                ("NameLen", ctypes.c_ulong), ("MaxNameLen", ctypes.c_ulong),
                ("Name", ctypes.c_wchar * 1)]


class IMAGEHLP_LINEW64(ctypes.Structure):
    _fields_ = [("SizeOfStruct", ctypes.c_ulong), ("Key", ctypes.c_void_p),
                ("LineNumber", ctypes.c_ulong), ("FileName", ctypes.c_wchar_p),
                ("Address", ctypes.c_ulonglong)]


class GUID(ctypes.Structure):
    _fields_ = [("b", ctypes.c_ubyte * 16)]


class IMAGEHLP_MODULEW64(ctypes.Structure):
    _fields_ = [("SizeOfStruct", ctypes.c_ulong), ("BaseOfImage", ctypes.c_ulonglong),
                ("ImageSize", ctypes.c_ulong), ("TimeDateStamp", ctypes.c_ulong),
                ("CheckSum", ctypes.c_ulong), ("NumSyms", ctypes.c_ulong),
                ("SymType", ctypes.c_int), ("ModuleName", ctypes.c_wchar * 32),
                ("ImageName", ctypes.c_wchar * 256), ("LoadedImageName", ctypes.c_wchar * 256),
                ("LoadedPdbName", ctypes.c_wchar * 256), ("CVSig", ctypes.c_ulong),
                ("CVData", ctypes.c_wchar * 780), ("PdbSig", ctypes.c_ulong),
                ("PdbSig70", GUID), ("PdbAge", ctypes.c_ulong),
                ("PdbUnmatched", ctypes.c_int), ("DbgUnmatched", ctypes.c_int),
                ("LineNumbers", ctypes.c_int), ("GlobalSymbols", ctypes.c_int),
                ("TypeInfo", ctypes.c_int), ("SourceIndexed", ctypes.c_int),
                ("Publics", ctypes.c_int), ("MachineType", ctypes.c_ulong),
                ("Reserved", ctypes.c_ulong)]


class DbgHelp:
    """dbghelp.dll on an image: the engine WinDbg and Visual Studio use."""

    def __init__(self, image, base):
        self.d = ctypes.WinDLL("dbghelp.dll")
        self.h = ctypes.c_void_p(0x0F1D0DB3)
        # UNDNAME | LOAD_LINES | FAIL_CRITICAL_ERRORS | EXACT_SYMBOLS (the
        # PDB's GUID and age must match the image's RSDS record)
        self.d.SymSetOptions(0x00000002 | 0x00000010 | 0x00000200 | 0x00000400)
        search = os.path.dirname(os.path.abspath(image))
        if not self.d.SymInitializeW(self.h, ctypes.c_wchar_p(search), False):
            raise OSError("SymInitializeW failed")
        self.d.SymLoadModuleExW.restype = ctypes.c_ulonglong
        self.d.SymLoadModuleExW.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_wchar_p, ctypes.c_wchar_p,
                                            ctypes.c_ulonglong, ctypes.c_ulong, ctypes.c_void_p, ctypes.c_ulong]
        self.base = self.d.SymLoadModuleExW(self.h, None, os.path.abspath(image), None, base, 0, None, 0)
        if not self.base:
            raise OSError("SymLoadModuleExW failed: %d" % ctypes.GetLastError())

    def module(self):
        m = IMAGEHLP_MODULEW64()
        m.SizeOfStruct = ctypes.sizeof(m)
        self.d.SymGetModuleInfoW64.argtypes = [ctypes.c_void_p, ctypes.c_ulonglong, ctypes.c_void_p]
        if not self.d.SymGetModuleInfoW64(self.h, ctypes.c_ulonglong(self.base), ctypes.byref(m)):
            raise OSError("SymGetModuleInfoW64 failed: %d" % ctypes.GetLastError())
        return m

    def _info(self):
        buf = ctypes.create_string_buffer(ctypes.sizeof(SYMBOL_INFOW) + 2 * 1024)
        si = ctypes.cast(buf, ctypes.POINTER(SYMBOL_INFOW)).contents
        si.SizeOfStruct = ctypes.sizeof(SYMBOL_INFOW)
        si.MaxNameLen = 1000
        return buf, si

    def _name(self, buf, si):
        off = SYMBOL_INFOW.Name.offset
        return ctypes.wstring_at(ctypes.addressof(si) + off, si.NameLen)

    def from_name(self, name):
        buf, si = self._info()
        self.d.SymFromNameW.argtypes = [ctypes.c_void_p, ctypes.c_wchar_p, ctypes.c_void_p]
        if not self.d.SymFromNameW(self.h, name, ctypes.byref(si)):
            return None
        return si.Address

    def from_addr(self, addr):
        buf, si = self._info()
        disp = ctypes.c_ulonglong(0)
        self.d.SymFromAddrW.argtypes = [ctypes.c_void_p, ctypes.c_ulonglong, ctypes.c_void_p, ctypes.c_void_p]
        if not self.d.SymFromAddrW(self.h, ctypes.c_ulonglong(addr), ctypes.byref(disp), ctypes.byref(si)):
            return None, None
        return self._name(buf, si), disp.value

    def line(self, addr):
        ln = IMAGEHLP_LINEW64()
        ln.SizeOfStruct = ctypes.sizeof(ln)
        disp = ctypes.c_ulong(0)
        self.d.SymGetLineFromAddrW64.argtypes = [ctypes.c_void_p, ctypes.c_ulonglong, ctypes.c_void_p, ctypes.c_void_p]
        if not self.d.SymGetLineFromAddrW64(self.h, ctypes.c_ulonglong(addr), ctypes.byref(disp), ctypes.byref(ln)):
            return None, None, None
        return ln.LineNumber, ln.FileName, ln.Address

    def close(self):
        self.d.SymCleanup(self.h)


# --- Checks -------------------------------------------------------------------

def check_pdb_matches_image(pe, pdb_path):
    """The RSDS record and the PDB: same GUID and age; returns the Pdb."""
    rs = pe.rsds()
    if not rs:
        bad("no RSDS (CodeView) debug directory entry in the image")
        return None
    guid, age, path, addr, ptr = rs
    ok("debug directory: RSDS %s age %d path %s" % (guid_text(guid), age, path))
    bid = [s for s in pe.sections if s[0] == ".buildid"]
    if bid and bid[0][2] <= addr < bid[0][2] + bid[0][1]:
        ok("RSDS record inside .buildid")
    else:
        bad("RSDS record not inside a .buildid section")
    try:
        data = open(pdb_path, "rb").read()
    except OSError as e:
        bad("PDB %s: %s" % (pdb_path, e))
        return None
    try:
        pdb = Pdb(data)
    except AssertionError as e:
        bad("PDB rejected by the reader: %s" % e)
        return None
    ok("PDB parses: %d streams, %d procedures, %d publics, %d line rows" % (len(pdb.streams), len(pdb.procs), len(pdb.publics), len(pdb.lines)))
    if pdb.guid == guid and pdb.age == age:
        ok("PDB GUID and age match the image")
    else:
        bad("PDB GUID %s age %d, image %s age %d" % (guid_text(pdb.guid), pdb.age, guid_text(guid), age))
    for name, p in pdb.procs.items():
        if name not in pdb.procrefs or pdb.procrefs[name][0] != p["mod_off"] or pdb.procrefs[name][1] != p["mod"] + 1:
            bad("S_PROCREF of %s missing or wrong" % name)
            break
    else:
        ok("every procedure has its S_PROCREF in the globals")
    if pdb.sechdr is not None and len(pdb.sechdr) == pe.nsec:
        ok("section header stream: %d sections, as the image" % pe.nsec)
    else:
        bad("section header stream does not match the image")
    return pdb


def text_rva(pe):
    return pe.sections[0][2]


def check_dbghelp(image, pe, expect_procs, expect_lines, expect_publics):
    if os.name != "nt":
        print("SKIP dbghelp (not Windows)")
        return
    try:
        dh = DbgHelp(image, pe.image_base)
    except OSError as e:
        bad("dbghelp: %s" % e)
        return
    try:
        m = dh.module()
        if m.SymType == 3 and not m.PdbUnmatched:
            ok("dbghelp loaded the PDB: %s (lines %d, globals %d, publics %d)" % (m.LoadedPdbName, m.LineNumbers, m.GlobalSymbols, m.Publics))
        else:
            bad("dbghelp did not load the PDB (SymType %d, unmatched %d)" % (m.SymType, m.PdbUnmatched))
            return
        base = dh.base + text_rva(pe)
        wrong = 0
        for name, (off, size) in expect_procs.items():
            a = dh.from_name(name)
            if a != base + off:
                wrong += 1
                if wrong <= 5:
                    bad("dbghelp SymFromName(%s) = %s, want %#x" % (name, hex(a) if a else None, base + off))
            n, disp = dh.from_addr(base + off + (size // 2))
            if n != name:
                wrong += 1
                if wrong <= 5:
                    bad("dbghelp SymFromAddr(%s + %d) = %s" % (name, size // 2, n))
        if wrong == 0:
            ok("dbghelp: %d procedures by name and by address" % len(expect_procs))
        wrong = 0
        for name, off in expect_publics.items():
            a = dh.from_name(name)
            if a is None and name.startswith("_"):
                # dbghelp undecorates public names (SYMOPT_UNDNAME) as x86 C
                # names: one leading underscore goes ("__fid_entry" is found
                # as "_fid_entry")
                a = dh.from_name(name[1:])
            if a != base + off:
                wrong += 1
                if wrong <= 5:
                    bad("dbghelp SymFromName(public %s) = %s, want %#x" % (name, hex(a) if a else None, base + off))
        if wrong == 0 and expect_publics:
            ok("dbghelp: %d publics by name" % len(expect_publics))
        wrong = 0
        for off, line, fname in expect_lines:
            ln, fn, addr = dh.line(base + off)
            if ln != line or (fname and (fn is None or not fn.replace("\\", "/").endswith(fname.replace("\\", "/")))):
                wrong += 1
                if wrong <= 5:
                    bad("dbghelp line at .text+%#x = %s %s, want %d %s" % (off, ln, fn, line, fname))
        if wrong == 0:
            ok("dbghelp: %d line rows" % len(expect_lines))
    finally:
        dh.close()


def check_llvm(image, pdb_path, pe, expect_procs, expect_lines):
    tool = shutil.which("llvm-pdbutil")
    if tool:
        r = subprocess.run([tool, "dump", "-all", pdb_path], capture_output=True, text=True)
        if r.returncode == 0 and "error" not in r.stderr.lower():
            ok("llvm-pdbutil dump -all")
        else:
            bad("llvm-pdbutil dump -all: %s" % (r.stderr.strip()[:400]))
    else:
        print("SKIP llvm-pdbutil (not on PATH)")
    sym = shutil.which("llvm-symbolizer")
    if sym and expect_lines:
        base = pe.image_base + text_rva(pe)
        addrs = [hex(base + off) for off, _, _ in expect_lines[:20]]
        r = subprocess.run([sym, "--obj=" + image] + addrs, capture_output=True, text=True)
        out = r.stdout
        miss = [ln for _, ln, _ in expect_lines[:20] if (":%d:" % ln) not in out]
        if r.returncode == 0 and not miss:
            ok("llvm-symbolizer: %d addresses" % len(addrs))
        else:
            bad("llvm-symbolizer missed lines %s" % miss[:5])
    elif not sym:
        print("SKIP llvm-symbolizer (not on PATH)")


def expected_lines_from_pdb_rows(rows):
    return rows


def run_unit(fid, src):
    tmp = tempfile.mkdtemp(prefix="pdbunit")
    exe = os.path.join(tmp, "in.exe")
    r = subprocess.run([fid, "emit", src, "-o", exe], capture_output=True, text=True)
    if r.returncode != 0:
        sys.exit("emit failed:\n" + r.stdout + r.stderr)
    out_exe = os.path.join(tmp, "out.exe")
    out_pdb = os.path.join(tmp, "out.pdb")
    env = dict(os.environ, PDB_IN=exe, PDB_OUT_EXE=out_exe, PDB_OUT=out_pdb)
    # the unit built natively (the interpreter takes minutes on an image's
    # bytes); `fid run` when PDB_UNIT_RUN=1
    if os.environ.get("PDB_UNIT_RUN") == "1":
        cmd = [fid, "run", "tests/nattools/pdb_unit.fd"]
    else:
        unit = os.path.join(tmp, "pdb_unit.exe")
        r = subprocess.run([fid, "emit", "tests/nattools/pdb_unit.fd", "-o", unit], capture_output=True, text=True)
        if r.returncode != 0:
            sys.exit("emit of pdb_unit.fd failed:\n" + r.stdout[-2000:] + r.stderr[-2000:])
        cmd = [unit]
    r = subprocess.run(cmd, capture_output=True, text=True, env=env)
    lines = [ln.strip() for ln in r.stdout.splitlines()]
    if "done" not in lines:
        sys.exit("pdb_unit.fd failed:\n" + r.stdout[-2000:] + r.stderr[-2000:])
    check_unit_output(lines, exe, out_exe, out_pdb)


def check_unit_output(lines, exe, out_exe, out_pdb):
    procs, publics, rows, guid = {}, {}, [], None
    for ln in lines:
        f = ln.split()
        if not f:
            continue
        if f[0] == "proc":
            procs[f[1]] = (int(f[2]), int(f[3]))
        elif f[0] == "public":
            publics[f[1]] = int(f[2])
        elif f[0] == "row":
            rows.append((procs[f[1]][0] + int(f[2]), int(f[3]), "pdb_unit_main.fd" if f[4] == "0" else "lib_extra.fd"))
        elif f[0] == "guid":
            guid = f[1]
    ok("pdb_unit.fd: %d procedures, %d publics, %d rows" % (len(procs), len(publics), len(rows)))
    pe = Pe(open(out_exe, "rb").read())
    pdb = check_pdb_matches_image(pe, out_pdb)
    if pdb is None:
        return
    rs = pe.rsds()
    if guid_text(rs[0]) == guid:
        ok("GUID as pdb_guid_text prints it")
    else:
        bad("GUID %s, pdb_guid_text %s" % (guid_text(rs[0]), guid))
    # the PDB holds exactly the unit's procedures, publics and rows
    got = {n: (p["off"], p["size"]) for n, p in pdb.procs.items()}
    if got == procs and all(p["seg"] == 1 for p in pdb.procs.values()):
        ok("S_GPROC32: every procedure at its offset and size in section 1")
    else:
        bad("procedures differ: %s" % sorted(set(got.items()) ^ set(procs.items()))[:5])
    want_pub = dict(publics)
    for n, (o, _) in procs.items():
        want_pub[n] = o
    gotpub = {n: o for n, (seg, o, fl) in pdb.publics.items()}
    if gotpub == want_pub:
        ok("S_PUB32: every function and COFF symbol")
    else:
        bad("publics differ: %s" % sorted(set(gotpub.items()) ^ set(want_pub.items()))[:5])
    gotrows = sorted((o, l, f) for _, o, l, f in pdb.lines)
    wantrows = sorted(rows)
    if gotrows == wantrows:
        ok("C13 lines: %d rows with their files" % len(rows))
    else:
        bad("line rows differ: got %s want %s" % (gotrows[:6], wantrows[:6]))
    # the new image still runs as the old one
    a = subprocess.run([exe], capture_output=True)
    b = subprocess.run([out_exe], capture_output=True)
    if a.returncode == b.returncode and a.stdout == b.stdout:
        ok("the image with its debug directory runs as before")
    else:
        bad("the image with its debug directory runs differently (%d vs %d)" % (a.returncode, b.returncode))
    check_dbghelp(out_exe, pe, procs, rows, publics)
    check_llvm(out_exe, out_pdb, pe, procs, rows)


def check_image(image, pdb_path=None, run_ref=None):
    pe = Pe(open(image, "rb").read())
    rs = pe.rsds()
    if pdb_path is None:
        cand = os.path.splitext(image)[0] + ".pdb"
        pdb_path = rs[2] if rs and os.path.exists(rs[2]) else cand
    pdb = check_pdb_matches_image(pe, pdb_path)
    if pdb is None:
        return
    coff = pe.coff_functions()
    procs = {}
    wrong = 0
    for n, p in pdb.procs.items():
        procs[n] = (p["off"], p["size"])
        if coff.get(n) != p["off"]:
            wrong += 1
            if wrong <= 5:
                bad("procedure %s at %#x, COFF symbol at %s" % (n, p["off"], coff.get(n)))
    if wrong == 0 and procs:
        ok("%d procedures where the COFF symbols put them" % len(procs))
    elif not procs:
        bad("no procedures in the PDB")
    if "main" in procs:
        ok("main is a procedure")
    else:
        bad("main is not a procedure")
    publics = {n: o for n, (seg, o, fl) in pdb.publics.items() if n not in procs}
    # DWARF rows: the PDB must give the same line at each row's address
    line_sec, _ = pe.section(".debug_line")
    rows = []
    if line_sec:
        base = pe.image_base + text_rva(pe)
        for addr, ln, fname, end in dwarf_rows(line_sec):
            if not end and addr >= base:
                rows.append((addr - base, ln, os.path.basename(fname)))
        # a row the DWARF table repeats at one address: keep the last
        last = {}
        for off, ln, f in rows:
            last[off] = (ln, f)
        rows = sorted((o, l, f) for o, (l, f) in last.items())
        pdbrows = {o: (l, os.path.basename(f.replace("\\", "/"))) for _, o, l, f in pdb.lines}
        miss = [(o, l, f) for o, l, f in rows if pdbrows.get(o, (None, None))[0] != l]
        if rows and not miss:
            ok("C13 lines agree with .debug_line at %d addresses" % len(rows))
        else:
            bad("C13 lines differ from .debug_line at %d of %d addresses: %s" % (len(miss), len(rows), miss[:5]))
    else:
        print("SKIP .debug_line comparison (no DWARF in the image)")
    if run_ref is not None:
        a = subprocess.run([run_ref], capture_output=True)
        b = subprocess.run([image], capture_output=True)
        if a.returncode == b.returncode and a.stdout == b.stdout:
            ok("the debug image runs as the image without -g")
        else:
            bad("the debug image runs differently from the image without -g")
    check_dbghelp(image, pe, procs, rows, publics)
    check_llvm(image, pdb_path, pe, procs, rows)


def main():
    args = sys.argv[1:]
    fid = None
    if "--fid" in args:
        i = args.index("--fid")
        fid = args[i + 1]
        del args[i:i + 2]
    nat = "--nat" in args
    if nat:
        args.remove("--nat")
    if "--unit" in args:
        args.remove("--unit")
        run_unit(fid or "release/fid.exe", args[0] if args else "tests/bootstrap/hello.fd")
    elif "--emit" in args:
        args.remove("--emit")
        fid = fid or "release/fid.exe"
        src = args[0] if args else "tests/bootstrap/hello.fd"
        tmp = tempfile.mkdtemp(prefix="pdbemit")
        exe = os.path.join(tmp, "prog.exe")
        ref = os.path.join(tmp, "ref.exe")
        extra = ["--nat"] if nat else []
        env = dict(os.environ, FIDES_NO_CACHE="1")
        if nat:
            # the `--nat` PE's DWARF (and so its PDB) is behind
            # ng_dbg_win_dwarf until `-g` turns it on by itself
            env["FID_NAT_DWARF"] = "1"
        r = subprocess.run([fid, "emit", src, "-g", "-o", exe] + extra, capture_output=True, text=True, env=env)
        if r.returncode != 0:
            sys.exit("emit -g failed:\n" + r.stdout + r.stderr)
        r = subprocess.run([fid, "emit", src, "-o", ref] + extra, capture_output=True, text=True, env=env)
        if r.returncode != 0:
            sys.exit("emit failed:\n" + r.stdout + r.stderr)
        if os.path.exists(os.path.join(tmp, "prog.pdb")):
            ok("emit -g wrote prog.pdb beside prog.exe")
        else:
            bad("emit -g wrote no prog.pdb")
        check_image(exe, None, ref)
    elif args:
        check_image(args[0], args[1] if len(args) > 1 else None)
    else:
        print(__doc__)
        return 2
    print("FAILED: %d" % len(FAILS) if FAILS else "PASS")
    return 1 if FAILS else 0


if __name__ == "__main__":
    sys.exit(main())
