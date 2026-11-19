#!/usr/bin/env python3
"""Our own reader of the images our writers produce (own-backend roadmap RL9).

    python3 scripts/image-lint.py <image> [<image> ...] [--quiet]
    python3 scripts/image-lint.py --self-test

scripts/image-check.sh runs it on every image, beside the system's readers
(readelf, llvm-readobj, dumpbin, otool, ...), so an image is checked even
where none of those is installed. It knows the formats compiler/
image_writer.fd writes:

  ELF64     executables (ET_EXEC, ET_DYN) and relocatable objects (ET_REL)
            for x86-64, AArch64 and RISC-V: the identification, the header
            sizes, every program header and section header inside the file,
            PT_LOAD segments ordered, aligned (offset and address congruent
            to the alignment), not overlapping, filesz <= memsz; the entry
            point in an executable segment; every allocated section inside
            the segment that maps it (file offset and address agree); the
            section names, symbol tables (entry size, sh_info, names, section
            indexes) and relocation tables (entry size, symbol index, offset
            inside the section they patch) of the file
  PE32+     executables and UEFI applications (x86-64, AArch64): the DOS
            stub, the signature, machine, optional header magic and size,
            file and section alignment, SizeOfHeaders, SizeOfImage, sections
            (aligned, in order, not overlapping, raw data inside the file),
            the entry point in an executable section, every data directory
            inside the image, the import table (descriptors, DLL names,
            lookup and address tables, hint/name entries), base relocations
            (block sizes, types, targets inside the image), the exception
            table (.pdata: sorted, inside executable sections, unwind info
            version 1 or 2), the checksum when one is set, the COFF symbol
            and string tables
  COFF      objects (`fid codegen --coff`, the machine layer's object
            writer nat_emit_object): sections, their relocations
            (inside the file, symbol indexes in range), the symbol and
            string tables
  Mach-O    thin and fat: handed to scripts/macho-check.py (what the kernel
            and dyld check, with the code signature)

Each finding is printed to stderr as "error: <image>: <what>"; the exit
status is the number of images with a finding (0: all clean). --quiet
prints nothing for a clean image. --self-test builds small images in
memory, checks that they pass and that each of a list of corruptions is
found (exit 1 otherwise).
"""
import os
import struct
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))


class Lint:
    def __init__(self, data, name):
        self.d = data
        self.name = name
        self.errors = []
        self.checks = 0
        self.kind = "?"

    def err(self, msg):
        self.errors.append(msg)

    def ok(self, cond, msg):
        self.checks += 1
        if not cond:
            self.err(msg)
        return cond

    def u8(self, o):
        return self.d[o] if 0 <= o < len(self.d) else None

    def u16(self, o):
        return struct.unpack_from("<H", self.d, o)[0] if 0 <= o and o + 2 <= len(self.d) else None

    def u32(self, o):
        return struct.unpack_from("<I", self.d, o)[0] if 0 <= o and o + 4 <= len(self.d) else None

    def u64(self, o):
        return struct.unpack_from("<Q", self.d, o)[0] if 0 <= o and o + 8 <= len(self.d) else None

    def cstr(self, o, limit=4096):
        if o is None or o < 0 or o >= len(self.d):
            return None
        e = self.d.find(b"\0", o, min(len(self.d), o + limit))
        if e < 0:
            return None
        return self.d[o:e]


def pow2(x):
    return x > 0 and (x & (x - 1)) == 0


# --------------------------------------------------------------------- ELF
ELF_MACHINES = {62: "x86-64", 183: "AArch64", 243: "RISC-V"}


def lint_elf(L):
    d = L.d
    n = len(d)
    L.kind = "ELF"
    if not L.ok(n >= 64, "shorter than an ELF header"):
        return
    L.ok(d[4] == 2, "EI_CLASS %d, not ELFCLASS64" % d[4])
    L.ok(d[5] == 1, "EI_DATA %d, not little-endian" % d[5])
    L.ok(d[6] == 1, "EI_VERSION %d" % d[6])
    e_type, e_machine, e_version = struct.unpack_from("<HHI", d, 16)
    e_entry, e_phoff, e_shoff = struct.unpack_from("<QQQ", d, 24)
    e_flags, e_ehsize, e_phentsize, e_phnum, e_shentsize, e_shnum, e_shstrndx = struct.unpack_from("<IHHHHHH", d, 48)
    L.ok(e_type in (1, 2, 3), "e_type %d is not REL, EXEC or DYN" % e_type)
    L.ok(e_machine in ELF_MACHINES, "e_machine %d is not x86-64, AArch64 or RISC-V" % e_machine)
    L.kind = "ELF %s %s" % ({1: "object", 2: "executable", 3: "PIE/shared"}.get(e_type, "?"), ELF_MACHINES.get(e_machine, "?"))
    L.ok(e_version == 1, "e_version %d" % e_version)
    L.ok(e_ehsize == 64, "e_ehsize %d, not 64" % e_ehsize)
    if e_phnum:
        L.ok(e_phentsize == 56, "e_phentsize %d, not 56" % e_phentsize)
        L.ok(e_phoff + e_phnum * 56 <= n, "program headers past the end of the file")
    if e_shnum:
        L.ok(e_shentsize == 64, "e_shentsize %d, not 64" % e_shentsize)
        L.ok(e_shoff + e_shnum * 64 <= n, "section headers past the end of the file")
        L.ok(e_shstrndx < e_shnum, "e_shstrndx %d >= e_shnum %d" % (e_shstrndx, e_shnum))
    if e_type in (2, 3):
        L.ok(e_phnum > 0, "an executable without program headers")
    if L.errors:
        return
    # Program headers
    loads = []
    dyn_ph = []
    for i in range(e_phnum):
        o = e_phoff + i * 56
        p_type, p_flags, p_offset, p_vaddr, p_paddr, p_filesz, p_memsz, p_align = struct.unpack_from("<IIQQQQQQ", d, o)
        if p_type == 0:
            continue
        L.ok(p_offset + p_filesz <= n, "segment %d (type %#x): file bytes %#x+%#x past the end of the file" % (i, p_type, p_offset, p_filesz))
        if p_type == 2:  # PT_DYNAMIC
            dyn_ph.append((p_vaddr, p_offset, p_filesz, i))
            L.ok(p_filesz % 16 == 0, "PT_DYNAMIC (segment %d): size %#x is not a multiple of 16" % (i, p_filesz))
        if p_type == 3:  # PT_INTERP: a NUL-terminated path
            L.ok(p_filesz > 1 and p_offset + p_filesz <= n and d[p_offset + p_filesz - 1] == 0, "PT_INTERP (segment %d): not a NUL-terminated path" % i)
        if p_type == 1:  # PT_LOAD
            L.ok(p_filesz <= p_memsz, "segment %d: filesz %#x > memsz %#x" % (i, p_filesz, p_memsz))
            if L.ok(p_align == 0 or pow2(p_align), "segment %d: alignment %#x is not a power of two" % (i, p_align)):
                if p_align > 1:
                    L.ok(p_vaddr % p_align == p_offset % p_align, "segment %d: address %#x and offset %#x not congruent modulo %#x" % (i, p_vaddr, p_offset, p_align))
            loads.append((p_vaddr, p_memsz, p_offset, p_filesz, p_flags, i))
    for a, b in zip(loads, loads[1:]):
        L.ok(a[0] <= b[0], "PT_LOAD segments %d and %d not in address order" % (a[5], b[5]))
        L.ok(a[0] + a[1] <= b[0] or b[1] == 0 or a[1] == 0, "PT_LOAD segments %d and %d overlap in memory" % (a[5], b[5]))
    if e_type == 2 or (e_type == 3 and e_entry != 0):
        inx = [s for s in loads if s[0] <= e_entry < s[0] + s[1]]
        if L.ok(bool(inx), "entry point %#x is in no PT_LOAD segment" % e_entry):
            L.ok(inx[0][4] & 1, "entry point %#x is in segment %d, which is not executable" % (e_entry, inx[0][5]))
    # Section headers
    if not e_shnum:
        return
    secs = []
    for i in range(e_shnum):
        o = e_shoff + i * 64
        secs.append(struct.unpack_from("<IIQQQQIIQQ", d, o))
    shstr = secs[e_shstrndx]
    L.ok(shstr[1] == 3, "the section-name table (index %d) is not SHT_STRTAB" % e_shstrndx)
    names = []
    for i, s in enumerate(secs):
        sh_name, sh_type, sh_flags, sh_addr, sh_offset, sh_size, sh_link, sh_info, sh_align, sh_entsize = s
        nm = L.cstr(shstr[4] + sh_name) if sh_name < shstr[5] else None
        if i == 0:
            L.ok(sh_type == 0, "section 0 is not SHT_NULL")
            names.append("")
            continue
        L.ok(nm is not None, "section %d: name offset %d outside the name table" % (i, sh_name))
        label = (nm or b"?").decode("latin-1")
        names.append(label)
        if sh_type != 8:  # not NOBITS
            L.ok(sh_offset + sh_size <= n, "section %d (%s): bytes %#x+%#x past the end of the file" % (i, label, sh_offset, sh_size))
        L.ok(sh_align == 0 or pow2(sh_align), "section %d (%s): alignment %#x is not a power of two" % (i, label, sh_align))
        if sh_align > 1 and (sh_flags & 2) and e_type != 1:
            L.ok(sh_addr % sh_align == 0, "section %d (%s): address %#x not aligned to %#x" % (i, label, sh_addr, sh_align))
        L.ok(sh_link < e_shnum, "section %d (%s): sh_link %d out of range" % (i, label, sh_link))
        if (sh_flags & 2) and e_type != 1 and sh_size:  # SHF_ALLOC in an image: mapped by PT_LOADs
            # The machine layer's .text is the whole loaded image, so it
            # may run over several adjoining segments: every byte must be in
            # one, at the file offset the section says
            seg = [g for g in loads if g[0] < sh_addr + sh_size and sh_addr < g[0] + g[1]]
            covered = sh_addr
            for g in sorted(seg):
                if g[0] <= covered:
                    covered = max(covered, g[0] + g[1])
            if L.ok(bool(seg) and covered >= sh_addr + sh_size, "section %d (%s): %#x+%#x is not inside the PT_LOAD segments" % (i, label, sh_addr, sh_size)):
                for g in seg:
                    if sh_type != 8:
                        a = max(sh_addr, g[0])
                        if a - g[0] < g[3]:  # a byte the segment takes from the file
                            L.ok(sh_offset + (a - sh_addr) == g[2] + (a - g[0]), "section %d (%s): file offset %#x does not map to its address %#x in segment %d" % (i, label, sh_offset, sh_addr, g[5]))
                if sh_flags & 4:  # SHF_EXECINSTR
                    L.ok(any(g[4] & 1 for g in seg), "section %d (%s) is executable but no segment holding it is" % (i, label))
    # A dynamic segment needs its section (readelf: "no .dynamic section in
    # the dynamic segment"), at the same address and file offset
    for (dv, do_, dsz, di) in dyn_ph:
        ds = [s for s in secs if s[1] == 6]
        if L.ok(bool(ds), "PT_DYNAMIC (segment %d) but no SHT_DYNAMIC section (readelf reports it)" % di):
            L.ok(any(s[3] == dv and s[4] == do_ for s in ds), "the SHT_DYNAMIC section is not at PT_DYNAMIC's address %#x / offset %#x" % (dv, do_))
    # Symbol tables
    nsyms = {}
    for i, s in enumerate(secs):
        sh_type, sh_offset, sh_size, sh_link, sh_info, sh_entsize = s[1], s[4], s[5], s[6], s[7], s[9]
        if sh_type not in (2, 11):  # SYMTAB, DYNSYM
            continue
        label = names[i]
        if not L.ok(sh_entsize == 24, "symbol table %s: entry size %d, not 24" % (label, sh_entsize)):
            continue
        cnt = sh_size // 24
        nsyms[i] = cnt
        L.ok(sh_size % 24 == 0, "symbol table %s: size %d is not a multiple of 24" % (label, sh_size))
        L.ok(sh_info <= cnt, "symbol table %s: sh_info %d > %d symbols" % (label, sh_info, cnt))
        strtab = secs[sh_link] if sh_link < len(secs) else None
        L.ok(strtab is not None and strtab[1] == 3, "symbol table %s: sh_link %d is not a string table" % (label, sh_link))
        bad_name = bad_shndx = bad_local = 0
        for k in range(cnt):
            o = sh_offset + k * 24
            st_name, st_info, st_other, st_shndx, st_value, st_size = struct.unpack_from("<IBBHQQ", d, o)
            if strtab is not None and (st_name >= strtab[5] or L.cstr(strtab[4] + st_name) is None):
                bad_name += 1
            if st_shndx != 0 and st_shndx < 0xff00 and st_shndx >= e_shnum:
                bad_shndx += 1
            if k < sh_info and (st_info >> 4) != 0:
                bad_local += 1
            if k >= sh_info and (st_info >> 4) == 0:
                bad_local += 1
        L.ok(bad_name == 0, "symbol table %s: %d symbols with a name outside the string table" % (label, bad_name))
        L.ok(bad_shndx == 0, "symbol table %s: %d symbols with a section index out of range" % (label, bad_shndx))
        L.ok(bad_local == 0, "symbol table %s: %d symbols on the wrong side of sh_info (locals first)" % (label, bad_local))
    # Relocation tables
    for i, s in enumerate(secs):
        sh_type, sh_flags, sh_offset, sh_size, sh_link, sh_info, sh_entsize = s[1], s[2], s[4], s[5], s[6], s[7], s[9]
        if sh_type not in (4, 9):  # RELA, REL
            continue
        label = names[i]
        want = 24 if sh_type == 4 else 16
        if not L.ok(sh_entsize == want, "relocations %s: entry size %d, not %d" % (label, sh_entsize, want)):
            continue
        L.ok(sh_size % want == 0, "relocations %s: size not a multiple of %d" % (label, want))
        cnt = nsyms.get(sh_link)
        target = secs[sh_info] if (sh_info and sh_info < len(secs)) else None
        bad_sym = bad_off = 0
        for k in range(sh_size // want):
            o = sh_offset + k * want
            r_offset, r_info = struct.unpack_from("<QQ", d, o)
            sym = r_info >> 32
            if cnt is not None and sym >= cnt:
                bad_sym += 1
            if e_type == 1 and target is not None and target[1] != 8 and r_offset >= target[5]:
                bad_off += 1
        L.ok(sh_link == 0 or sh_link in nsyms, "relocations %s: sh_link %d is not a symbol table" % (label, sh_link))
        L.ok(bad_sym == 0, "relocations %s: %d with a symbol index out of range" % (label, bad_sym))
        L.ok(bad_off == 0, "relocations %s: %d with an offset outside section %d" % (label, bad_off, sh_info))


# ---------------------------------------------------------------- PE / COFF
PE_MACHINES = {0x8664: "x86-64", 0xAA64: "AArch64"}
SCN_CNT_CODE, SCN_MEM_EXECUTE, SCN_MEM_WRITE = 0x20, 0x20000000, 0x80000000


def pe_checksum(d, csum_off):
    s = 0
    n = len(d)
    for o in range(0, n - (n % 2), 2):
        if o == csum_off or o == csum_off + 2:
            continue
        s += d[o] | (d[o + 1] << 8)
        s = (s & 0xFFFF) + (s >> 16)
    if n % 2:
        s += d[n - 1]
        s = (s & 0xFFFF) + (s >> 16)
    s = (s & 0xFFFF) + (s >> 16)
    return (s + n) & 0xFFFFFFFF


def coff_symbols(L, ptr, nsym, what):
    """The symbol and string tables after `ptr`: their sizes checked; the
    string table's size (or None)."""
    d = L.d
    if ptr == 0 and nsym == 0:
        return None
    if not L.ok(ptr + nsym * 18 + 4 <= len(d), "%s: symbol table (%d symbols at %#x) past the end of the file" % (what, nsym, ptr)):
        return None
    st = ptr + nsym * 18
    strsize = L.u32(st)
    L.ok(strsize is not None and strsize >= 4 and st + strsize <= len(d), "%s: string table size %s past the end of the file" % (what, strsize))
    k = 0
    bad = 0
    while k < nsym:
        o = ptr + k * 18
        if L.u32(o) == 0:
            off = L.u32(o + 4)
            if strsize is None or off < 4 or off >= strsize:
                bad += 1
        naux = d[o + 17]
        k += 1 + naux
    L.ok(k == nsym, "%s: auxiliary records run past the symbol count" % what)
    L.ok(bad == 0, "%s: %d symbol names outside the string table" % (what, bad))
    return strsize


def lint_pe(L):
    d = L.d
    n = len(d)
    L.kind = "PE"
    if not L.ok(n >= 0x40, "shorter than a DOS header"):
        return
    lfanew = L.u32(0x3C)
    if not L.ok(lfanew is not None and lfanew + 24 <= n, "e_lfanew %s points past the file" % lfanew):
        return
    if not L.ok(d[lfanew:lfanew + 4] == b"PE\0\0", "no PE signature at e_lfanew %#x" % lfanew):
        return
    fh = lfanew + 4
    machine, nsec, stamp, symptr, nsym, opt_size, chars = struct.unpack_from("<HHIIIHH", d, fh)
    L.ok(machine in PE_MACHINES, "machine %#x is not x86-64 or AArch64" % machine)
    L.ok(chars & 0x2, "IMAGE_FILE_EXECUTABLE_IMAGE not set")
    oh = fh + 20
    if not L.ok(oh + opt_size <= n and opt_size >= 112, "optional header (%d bytes) past the file or too short" % opt_size):
        return
    magic = L.u16(oh)
    if not L.ok(magic == 0x20B, "optional header magic %#x, not PE32+ (0x20b)" % magic):
        return
    entry = L.u32(oh + 16)
    image_base = L.u64(oh + 24)
    sec_align, file_align = L.u32(oh + 32), L.u32(oh + 36)
    size_image, size_headers, checksum = L.u32(oh + 56), L.u32(oh + 60), L.u32(oh + 64)
    subsystem, dll_chars = L.u16(oh + 68), L.u16(oh + 70)
    ndirs = L.u32(oh + 108)
    L.kind = "PE32+ %s %s" % ({3: "console", 2: "GUI", 10: "EFI application", 11: "EFI boot driver", 12: "EFI runtime driver"}.get(subsystem, "subsystem %d" % subsystem), PE_MACHINES.get(machine, "?"))
    L.ok(subsystem in (2, 3, 10, 11, 12), "subsystem %d" % subsystem)
    L.ok(opt_size == 112 + 8 * ndirs, "SizeOfOptionalHeader %d does not hold %d data directories" % (opt_size, ndirs))
    L.ok(pow2(file_align) and 512 <= file_align <= 65536, "FileAlignment %#x" % file_align)
    L.ok(pow2(sec_align) and sec_align >= file_align, "SectionAlignment %#x (FileAlignment %#x)" % (sec_align, file_align))
    L.ok(image_base % 65536 == 0, "ImageBase %#x not a multiple of 64 KiB" % image_base)
    st = oh + opt_size
    hdr_end = st + nsec * 40
    L.ok(hdr_end <= size_headers, "section table ends at %#x, past SizeOfHeaders %#x" % (hdr_end, size_headers))
    L.ok(size_headers % file_align == 0 if file_align else False, "SizeOfHeaders %#x not a multiple of FileAlignment" % size_headers)
    if not L.ok(hdr_end <= n, "section table past the end of the file"):
        return
    secs = []
    for i in range(nsec):
        o = st + i * 40
        nm = d[o:o + 8].rstrip(b"\0").decode("latin-1")
        vsize, va, rawsize, rawptr, relptr, lnptr, nrel, nln, sch = struct.unpack_from("<IIIIIIHHI", d, o + 8)
        secs.append((nm, va, vsize, rawptr, rawsize, sch))
        L.ok(va % sec_align == 0, "section %s: address %#x not aligned to %#x" % (nm, va, sec_align))
        if rawsize:
            L.ok(rawptr % file_align == 0, "section %s: raw data at %#x not aligned to %#x" % (nm, rawptr, file_align))
            L.ok(rawptr + rawsize <= n, "section %s: raw data %#x+%#x past the end of the file" % (nm, rawptr, rawsize))
            L.ok(rawptr >= size_headers, "section %s: raw data at %#x inside the headers" % (nm, rawptr))
        L.ok(not (sch & SCN_MEM_EXECUTE and sch & SCN_MEM_WRITE), "section %s is writable and executable" % nm)
    for a, b in zip(secs, secs[1:]):
        L.ok(a[1] < b[1], "sections %s and %s not in address order" % (a[0], b[0]))
        L.ok(a[1] + max(a[2], 1) <= b[1], "sections %s and %s overlap" % (a[0], b[0]))
    if secs:
        last = secs[-1]
        want = (last[1] + max(last[2], last[4]) + sec_align - 1) // sec_align * sec_align
        L.ok(size_image == want, "SizeOfImage %#x, the sections end at %#x" % (size_image, want))
        L.ok(secs[0][1] >= size_headers, "first section at %#x inside the headers (%#x)" % (secs[0][1], size_headers))

    def sec_of(rva):
        for s in secs:
            if s[1] <= rva < s[1] + max(s[2], s[4]):
                return s
        return None

    def off_of(rva):
        s = sec_of(rva)
        if s is None:
            return rva if rva < size_headers else None
        if rva - s[1] >= s[4]:
            return None  # in the zero-filled part
        return s[3] + rva - s[1]

    es = sec_of(entry)
    if L.ok(es is not None, "entry point %#x is in no section" % entry):
        L.ok(es[5] & (SCN_MEM_EXECUTE | SCN_CNT_CODE), "entry point %#x is in %s, which is not executable" % (entry, es[0]))
    dirs = [struct.unpack_from("<II", d, oh + 112 + 8 * k) for k in range(min(ndirs, 16))]
    for k, (rva, sz) in enumerate(dirs):
        if k == 4:  # the certificate table is a file offset
            if sz:
                L.ok(rva + sz <= n, "certificate table past the end of the file")
            continue
        if sz:
            L.ok(rva + sz <= size_image, "data directory %d (%#x+%#x) past SizeOfImage" % (k, rva, sz))
    # Imports
    if len(dirs) > 1 and dirs[1][1]:
        o = off_of(dirs[1][0])
        if L.ok(o is not None, "import directory %#x is not in the file" % dirs[1][0]):
            k = 0
            while True:
                if not L.ok(o + 20 <= n, "import descriptors run past the end of the file"):
                    break
                ilt, _, _, name_rva, iat = struct.unpack_from("<IIIII", d, o)
                if ilt == 0 and name_rva == 0 and iat == 0:
                    break
                dll = L.cstr(off_of(name_rva)) if off_of(name_rva) is not None else None
                if not L.ok(dll is not None and dll.lower().endswith(b".dll"), "import %d: DLL name at %#x is not a .dll name" % (k, name_rva)):
                    break
                for table, tname in ((ilt, "lookup"), (iat, "address")):
                    if table == 0:
                        continue
                    t = off_of(table)
                    if not L.ok(t is not None, "import %s: %s table %#x not in the file" % (dll, tname, table)):
                        continue
                    cnt = 0
                    while cnt < 100000:
                        v = L.u64(t + cnt * 8)
                        if not L.ok(v is not None, "import %s: %s table runs past the file" % (dll, tname)):
                            break
                        if v == 0:
                            break
                        if not v >> 63:
                            ho = off_of(v & 0x7FFFFFFF)
                            nm = L.cstr(ho + 2) if ho is not None else None
                            L.ok(nm is not None and len(nm) > 0, "import %s: hint/name entry %#x unreadable" % (dll, v))
                        cnt += 1
                k += 1
                o += 20
    # Base relocations
    if len(dirs) > 5 and dirs[5][1]:
        o = off_of(dirs[5][0])
        end = (o or 0) + dirs[5][1]
        if L.ok(o is not None and end <= n, "base relocation directory not in the file"):
            badt = 0
            while o < end:
                page, bsize = struct.unpack_from("<II", d, o)
                if not L.ok(bsize >= 8 and bsize % 4 == 0 and o + bsize <= end, "base relocation block at %#x: size %d" % (o, bsize)):
                    break
                for e in range((bsize - 8) // 2):
                    w = L.u16(o + 8 + 2 * e)
                    typ, off = w >> 12, w & 0xFFF
                    if typ == 0:
                        continue
                    if typ not in (3, 10) or page + off + (8 if typ == 10 else 4) > size_image:
                        badt += 1
                o += bsize
            L.ok(badt == 0, "%d base relocations of an unknown type or outside the image" % badt)
    # Exception table (x86-64 .pdata)
    if machine == 0x8664 and len(dirs) > 3 and dirs[3][1]:
        o = off_of(dirs[3][0])
        sz = dirs[3][1]
        if L.ok(o is not None and o + sz <= n and sz % 12 == 0, "exception table %#x+%#x" % (dirs[3][0], sz)):
            prev = -1
            bad = 0
            for e in range(sz // 12):
                b, en, ui = struct.unpack_from("<III", d, o + 12 * e)
                s = sec_of(b)
                if b < prev or en <= b or s is None or not (s[5] & (SCN_MEM_EXECUTE | SCN_CNT_CODE)) or en > s[1] + max(s[2], s[4]):
                    bad += 1
                elif not ui & 1:
                    uo = off_of(ui)
                    if uo is None or L.u8(uo) is None or (L.u8(uo) & 7) not in (1, 2):
                        bad += 1
                prev = b
            L.ok(bad == 0, "exception table: %d entries unsorted, outside code, or with bad unwind info" % bad)
    if checksum:
        L.ok(checksum == pe_checksum(d, oh + 64), "CheckSum %#x, the file sums to %#x" % (checksum, pe_checksum(d, oh + 64)))
    if symptr or nsym:
        coff_symbols(L, symptr, nsym, "COFF symbols")


def lint_coff(L):
    d = L.d
    n = len(d)
    machine, nsec, stamp, symptr, nsym, opt_size, chars = struct.unpack_from("<HHIIIHH", d, 0)
    L.kind = "COFF object %s" % PE_MACHINES.get(machine, "?")
    L.ok(machine in PE_MACHINES, "machine %#x" % machine)
    L.ok(opt_size == 0, "an object with an optional header (%d bytes)" % opt_size)
    st = 20 + opt_size
    if not L.ok(st + nsec * 40 <= n, "section table past the end of the file"):
        return
    for i in range(nsec):
        o = st + i * 40
        nm = d[o:o + 8].rstrip(b"\0").decode("latin-1")
        vsize, va, rawsize, rawptr, relptr, lnptr, nrel, nln, sch = struct.unpack_from("<IIIIIIHHI", d, o + 8)
        if rawsize and not (sch & 0x80):  # not .bss-like
            L.ok(rawptr + rawsize <= n, "section %s: raw data %#x+%#x past the end of the file" % (nm, rawptr, rawsize))
        if nrel:
            if L.ok(relptr + nrel * 10 <= n, "section %s: relocations past the end of the file" % nm):
                bad = 0
                for k in range(nrel):
                    va_, si, typ = struct.unpack_from("<IIH", d, relptr + 10 * k)
                    if si >= nsym or (rawsize and va_ >= rawsize):
                        bad += 1
                L.ok(bad == 0, "section %s: %d relocations with a symbol index or offset out of range" % (nm, bad))
    coff_symbols(L, symptr, nsym, "symbols")


# ------------------------------------------------------------------- driver
def lint_bytes(data, name):
    L = Lint(data, name)
    if data[:4] == b"\x7fELF":
        lint_elf(L)
    elif data[:2] == b"MZ":
        lint_pe(L)
    elif len(data) >= 20 and struct.unpack_from("<H", data, 0)[0] in PE_MACHINES:
        lint_coff(L)
    else:
        L.kind = "unknown"
        L.err("not an ELF, PE, COFF or Mach-O file")
    return L


MACHO_MAGICS = (b"\xcf\xfa\xed\xfe", b"\xca\xfe\xba\xbe", b"\xca\xfe\xba\xbf")


def lint_file(path, quiet):
    try:
        with open(path, "rb") as f:
            data = f.read()
    except OSError as e:
        sys.stderr.write("error: %s: %s\n" % (path, e))
        return 1
    if data[:4] in MACHO_MAGICS:
        args = [sys.executable, os.path.join(HERE, "macho-check.py"), path] + (["--quiet"] if quiet else [])
        r = subprocess.run(args)
        return 1 if r.returncode else 0
    L = lint_bytes(data, path)
    for e in L.errors:
        sys.stderr.write("error: %s: %s\n" % (path, e))
    if not L.errors and not quiet:
        print("%s: ok (%s, %d checks)" % (path, L.kind, L.checks))
    return 1 if L.errors else 0


# ---------------------------------------------------------------- self-test
def tiny_elf():
    """An x86-64 static executable: one PT_LOAD (R+X) holding .text, a
    .symtab of two symbols, .strtab, .shstrtab."""
    text = b"\x31\xff\xb8\x3c\x00\x00\x00\x0f\x05"  # xor edi,edi; mov eax,60; syscall
    base = 0x400000
    text_off = 0x1000
    shstr = b"\0.text\0.symtab\0.strtab\0.shstrtab\0"
    strtab = b"\0_start\0"
    sym = struct.pack("<IBBHQQ", 0, 0, 0, 0, 0, 0) + struct.pack("<IBBHQQ", 1, 0x12, 0, 1, base + text_off, len(text))
    body = bytearray(text_off) + text
    symoff = len(body)
    body += sym
    stroff = len(body)
    body += strtab
    shsoff = len(body)
    body += shstr
    while len(body) % 8:
        body += b"\0"
    shoff = len(body)
    eh = bytearray(b"\x7fELF\x02\x01\x01" + bytes(9))
    eh += struct.pack("<HHIQQQIHHHHHH", 2, 62, 1, base + text_off, 64, shoff, 0, 64, 56, 1, 64, 5, 4)
    ph = struct.pack("<IIQQQQQQ", 1, 5, 0, base, base, text_off + len(text), text_off + len(text), 0x1000)
    body[0:64] = eh
    body[64:64 + 56] = ph
    shs = [
        bytes(64),
        struct.pack("<IIQQQQIIQQ", 1, 1, 6, base + text_off, text_off, len(text), 0, 0, 16, 0),
        struct.pack("<IIQQQQIIQQ", 7, 2, 0, 0, symoff, len(sym), 3, 1, 8, 24),
        struct.pack("<IIQQQQIIQQ", 15, 3, 0, 0, stroff, len(strtab), 0, 0, 1, 0),
        struct.pack("<IIQQQQIIQQ", 23, 3, 0, 0, shsoff, len(shstr), 0, 0, 1, 0),
    ]
    for s in shs:
        body += s
    return bytes(body)


def tiny_pe():
    """An x86-64 console PE32+: .text (entry), .rdata with one import of
    kernel32.dll!ExitProcess, no relocations."""
    fa, sa = 0x200, 0x1000
    text = b"\x48\x83\xec\x28\x31\xc9\xff\x15\x00\x00\x00\x00"  # sub rsp,40; xor ecx,ecx; call [rip+x]
    # .rdata at RVA 0x2000: descriptor (20) + null (20), ILT (16), IAT (16), name, hint/name
    rd = bytearray(0x100)
    ilt, iat, dllname, hn = 0x2000 + 40, 0x2000 + 56, 0x2000 + 72, 0x2000 + 88
    struct.pack_into("<IIIII", rd, 0, ilt, 0, 0, dllname, iat)
    struct.pack_into("<Q", rd, 40, hn)
    struct.pack_into("<Q", rd, 56, hn)
    rd[72:72 + 13] = b"kernel32.dll\0"
    rd[88:88 + 14] = b"\0\0ExitProcess\0"
    dos = bytearray(0x80)
    dos[0:2] = b"MZ"
    struct.pack_into("<I", dos, 0x3C, 0x80)
    fh = struct.pack("<HHIIIHH", 0x8664, 2, 0, 0, 0, 240, 0x22)
    oh = bytearray(240)
    struct.pack_into("<HBBIIIII", oh, 0, 0x20B, 14, 0, fa, fa, 0, 0x1000, 0x1000)
    struct.pack_into("<QII", oh, 24, 0x140000000, sa, fa)
    struct.pack_into("<HHHHHHI", oh, 40, 6, 0, 0, 0, 6, 0, 0)
    struct.pack_into("<III", oh, 56, 0x3000, 0x200, 0)
    struct.pack_into("<HH", oh, 68, 3, 0x8160)
    struct.pack_into("<QQQQII", oh, 72, 0x100000, 0x1000, 0x100000, 0x1000, 0, 16)
    struct.pack_into("<II", oh, 112 + 8, 0x2000, 40)  # import directory
    struct.pack_into("<II", oh, 112 + 8 * 12, iat, 16)  # IAT
    sec = struct.pack("<8sIIIIIIHHI", b".text", len(text), 0x1000, fa, 0x200, 0, 0, 0, 0, 0x60000020)
    sec += struct.pack("<8sIIIIIIHHI", b".rdata", len(rd), 0x2000, fa, 0x400, 0, 0, 0, 0, 0x40000040)
    hdr = bytes(dos) + b"PE\0\0" + fh + bytes(oh) + sec
    img = bytearray(hdr + bytes(0x200 - len(hdr)))
    img += text + bytes(fa - len(text))
    img += rd + bytes(fa - len(rd))
    return bytes(img)


def self_test():
    fails = 0

    def expect(data, clean, what):
        nonlocal fails
        L = lint_bytes(data, what)
        if bool(L.errors) == clean:
            fails += 1
            print("FAIL %s: %s" % (what, "; ".join(L.errors) if L.errors else "no finding"))
        else:
            print("ok   %s%s" % (what, "" if clean else " -> " + L.errors[0]))

    def patch(data, off, fmt, val):
        b = bytearray(data)
        struct.pack_into(fmt, b, off, val)
        return bytes(b)

    elf = tiny_elf()
    expect(elf, True, "ELF executable")
    expect(patch(elf, 24, "<Q", 0x300000), False, "ELF entry outside every segment")
    expect(patch(elf, 64 + 4, "<I", 4), False, "ELF entry segment not executable")
    expect(patch(elf, 64 + 32, "<Q", len(elf) + 0x100000), False, "ELF segment filesz past the file")
    expect(patch(elf, 64 + 48, "<Q", 0x1001), False, "ELF segment alignment not a power of two")
    shoff = struct.unpack_from("<Q", elf, 40)[0]
    expect(patch(elf, shoff + 64 * 2 + 56, "<Q", 16), False, "ELF symbol entry size 16")
    expect(patch(elf, shoff + 64 * 1 + 24, "<Q", 0x500000), False, "ELF .text outside its segment")
    expect(patch(elf, 62, "<H", 9), False, "ELF e_shstrndx out of range")
    pe = tiny_pe()
    expect(pe, True, "PE executable")
    expect(patch(pe, 0x3C, "<I", 0x7FFF0), False, "PE e_lfanew past the file")
    oh = 0x80 + 4 + 20
    expect(patch(pe, oh + 16, "<I", 0x2010), False, "PE entry point in a data section")
    expect(patch(pe, oh + 56, "<I", 0x5000), False, "PE SizeOfImage wrong")
    expect(patch(pe, oh + 36, "<I", 0x300), False, "PE FileAlignment not a power of two")
    expect(patch(pe, oh + 64, "<I", 0x1234), False, "PE checksum wrong")
    st = oh + 240
    expect(patch(pe, st + 40 + 20, "<I", 0x10000), False, "PE section raw data past the file")
    expect(patch(pe, st + 36, "<I", 0xE0000020), False, "PE section writable and executable")
    expect(patch(pe, 0x400 + 12, "<I", 0x2000 + 0x80), False, "PE import DLL name not a name")
    print("image-lint self-test: %d failed" % fails)
    return 1 if fails else 0


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    if "--self-test" in sys.argv:
        return self_test()
    if not args:
        print(__doc__)
        return 2
    quiet = "--quiet" in sys.argv
    return sum(lint_file(p, quiet) for p in args)


if __name__ == "__main__":
    sys.exit(main())
