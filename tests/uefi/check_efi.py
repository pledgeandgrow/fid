#!/usr/bin/env python3
"""Structural check of a UEFI application written by `fid emit --uefi`
(compiler/pe_ir_enc.fd pe_build_efi, roadmap Phase 16).

    python3 tests/uefi/check_efi.py <image.efi> [<string the image must hold>...]

Checks what a UEFI loader (EDK2's PeCoffLoader, OVMF) relies on:
  - MZ header, e_lfanew, "PE\\0\\0", machine AMD64, PE32+ magic 0x20B
  - subsystem 10 (EFI_APPLICATION), relocations not stripped
  - section and file alignment, SizeOfImage, SizeOfHeaders
  - sections in RVA order, inside the image and the file, non-overlapping;
    .text executable and not writable, .data writable and not executable
  - the entry point inside .text; at it a jmp rel32 to the UEFI entry,
    whose first bytes save rbx, rbp, rdi, rsi (push 53 55 57 56)
  - the base relocation directory: blocks of 4-byte multiples, every entry
    ABSOLUTE (0) or DIR64 (10), every DIR64 slot inside .data and holding an
    address (linked at ImageBase) inside the image
  - each string given on the command line is in .data, NUL-terminated
Exit status 0 when every check passes; the failures are printed.
"""
import struct
import sys


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        return 2
    path = sys.argv[1]
    want = sys.argv[2:]
    b = open(path, "rb").read()
    bad = []

    def need(cond, msg):
        if not cond:
            bad.append(msg)
        return cond

    if not need(len(b) >= 512 and b[:2] == b"MZ", "no MZ header"):
        return report(bad)
    lfanew = struct.unpack_from("<I", b, 60)[0]
    if not need(b[lfanew:lfanew + 4] == b"PE\0\0", "no PE signature"):
        return report(bad)
    coff = lfanew + 4
    machine, nsec, _ts, _sym, _nsym, optsize, chars = struct.unpack_from("<HHIIIHH", b, coff)
    need(machine == 0x8664, "machine %#x, not AMD64" % machine)
    need(optsize == 240, "optional header size %d, not 240" % optsize)
    need(chars & 0x0002, "not EXECUTABLE_IMAGE")
    need(not chars & 0x0001, "RELOCS_STRIPPED set: the firmware could not move it")
    need(not chars & 0x2000, "marked as a DLL")
    opt = coff + 20
    magic = struct.unpack_from("<H", b, opt)[0]
    need(magic == 0x20B, "magic %#x, not PE32+" % magic)
    entry = struct.unpack_from("<I", b, opt + 16)[0]
    image_base = struct.unpack_from("<Q", b, opt + 24)[0]
    salign, falign = struct.unpack_from("<II", b, opt + 32)
    size_of_image, size_of_headers = struct.unpack_from("<II", b, opt + 56)
    subsystem = struct.unpack_from("<H", b, opt + 68)[0]
    nrva = struct.unpack_from("<I", b, opt + 108)[0]
    need(subsystem == 10, "subsystem %d, not 10 (EFI_APPLICATION)" % subsystem)
    need(salign >= 0x1000 and salign & (salign - 1) == 0, "section alignment %#x" % salign)
    need(512 <= falign <= salign and falign & (falign - 1) == 0, "file alignment %#x" % falign)
    need(image_base % 0x10000 == 0, "ImageBase %#x not 64 KB aligned" % image_base)
    need(size_of_image % salign == 0, "SizeOfImage %#x not section aligned" % size_of_image)
    need(size_of_headers % falign == 0, "SizeOfHeaders not file aligned")
    need(nrva == 16, "%d data directories" % nrva)
    dirs = [struct.unpack_from("<II", b, opt + 112 + 8 * i) for i in range(min(nrva, 16))]
    need(dirs[1] == (0, 0), "an import table: a UEFI image imports nothing")

    sh = opt + optsize
    need(size_of_headers >= sh + 40 * nsec, "section headers past SizeOfHeaders")
    secs = []
    for i in range(nsec):
        h = sh + 40 * i
        name = b[h:h + 8].rstrip(b"\0").decode("latin-1")
        vsize, va, rsize, roff = struct.unpack_from("<IIII", b, h + 8)
        flags = struct.unpack_from("<I", b, h + 36)[0]
        secs.append((name, vsize, va, rsize, roff, flags))
    prev_end = (size_of_headers + salign - 1) // salign * salign
    for name, vsize, va, rsize, roff, flags in secs:
        need(va % salign == 0, "%s at %#x not aligned" % (name, va))
        need(va >= prev_end, "%s overlaps the one before" % name)
        need(va + vsize <= size_of_image, "%s past SizeOfImage" % name)
        need(roff % falign == 0 and rsize % falign == 0, "%s raw data not file aligned" % name)
        need(roff + rsize <= len(b), "%s raw data past the end of the file" % name)
        prev_end = va + (max(vsize, 1) + salign - 1) // salign * salign
    names = [s[0] for s in secs]
    need(names == [".text", ".data", ".reloc"], "sections %s" % names)
    by = {s[0]: s for s in secs}

    def off(rva):
        for _n, vsize, va, rsize, roff, _f in secs:
            if va <= rva < va + vsize and rva - va < rsize:
                return roff + rva - va
        return -1

    if ".text" in by:
        t = by[".text"]
        need(t[5] & 0x20000000 and not t[5] & 0x80000000, ".text not executable, or writable")
        need(t[2] <= entry < t[2] + t[1], "entry %#x not in .text" % entry)
        eo = off(entry)
        if need(eo >= 0 and b[eo] == 0xE9, "the entry is not a jmp rel32"):
            rel = struct.unpack_from("<i", b, eo + 1)[0]
            to = off(entry + 5 + rel)
            need(to >= 0 and b[to:to + 4] == b"\x53\x55\x57\x56",
                 "the UEFI entry does not save rbx, rbp, rdi, rsi")
    if ".data" in by:
        d = by[".data"]
        need(d[5] & 0x80000000 and not d[5] & 0x20000000, ".data not writable, or executable")
        data = b[d[4]:d[4] + d[1]]
        for s in want:
            need(s.encode() + b"\0" in data, "%r not in .data" % s)

    rva, rsz = dirs[5]
    n64 = 0
    if need(rsz >= 8 and ".reloc" in by and by[".reloc"][2] == rva, "no base relocation directory in .reloc"):
        ro = off(rva)
        p = 0
        while p < rsz:
            page, bsize = struct.unpack_from("<II", b, ro + p)
            if not need(bsize >= 8 and bsize % 4 == 0 and p + bsize <= rsz, "bad relocation block at %#x" % p):
                break
            for q in range(8, bsize, 2):
                e = struct.unpack_from("<H", b, ro + p + q)[0]
                typ, at = e >> 12, page + (e & 0xFFF)
                if typ == 0:
                    continue
                if not need(typ == 10, "relocation type %d" % typ):
                    continue
                n64 += 1
                d = by.get(".data")
                need(d is not None and d[2] <= at and at + 8 <= d[2] + d[1], "DIR64 slot %#x not in .data" % at)
                so = off(at)
                if so >= 0:
                    v = struct.unpack_from("<Q", b, so)[0] - image_base
                    need(0 < v < size_of_image, "DIR64 slot %#x holds %#x, outside the image" % (at, v + image_base))
            p += bsize
    if not bad:
        print("ok: %s (%d bytes, EFI_APPLICATION, %d sections, %d DIR64 relocations)" % (path, len(b), nsec, n64))
    return report(bad)


def report(bad):
    for m in bad:
        print("FAIL: " + m)
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
