#!/usr/bin/env python3
"""disasm_pad.py -- a copy of an image with its padding as NOPs (TG5).

    python3 tests/nattools/disasm_pad.py <image> <fid disasm listing> <out>

`fid disasm` shows each run of alignment padding (the zeros after a jump up
to an aligned loop head, and between functions) as one line
`  <addr>:  (N zero bytes of padding)`. A linear disassembler such as
llvm-objdump / objdump -z decodes those zeros as `add [rax], al` pairs and
can run into the next instruction. scripts/disasm-check.sh compares the
instruction start addresses of the two, so it gives objdump this copy, in
which every padding byte `fid disasm` named is 0x90 (nop, one byte): both
listings then start the code after the padding at the same address.

Every byte called padding must be zero in the image (exit status 1
otherwise: the listing hid code). ELF (by PT_LOAD) and PE (by section).
"""
import re
import struct
import sys


def segments(img):
    """(vaddr, size, file offset) of each loaded part of the image."""
    out = []
    if img[:4] == b"\x7fELF":
        phoff = struct.unpack_from("<Q", img, 0x20)[0]
        phentsize, phnum = struct.unpack_from("<HH", img, 0x36)
        for i in range(phnum):
            p = phoff + i * phentsize
            kind = struct.unpack_from("<I", img, p)[0]
            if kind != 1:
                continue
            off, vaddr = struct.unpack_from("<QQ", img, p + 8)
            filesz = struct.unpack_from("<Q", img, p + 32)[0]
            out.append((vaddr, filesz, off))
    elif img[:2] == b"MZ":
        pe = struct.unpack_from("<I", img, 0x3C)[0]
        nsec = struct.unpack_from("<H", img, pe + 6)[0]
        opt = struct.unpack_from("<H", img, pe + 20)[0]
        base = struct.unpack_from("<Q", img, pe + 24 + 24)[0]
        sec = pe + 24 + opt
        for i in range(nsec):
            s = sec + i * 40
            vsize, rva, rawsize, raw = struct.unpack_from("<IIII", img, s + 8)
            out.append((base + rva, min(vsize, rawsize), raw))
    return out


def main():
    if len(sys.argv) != 4:
        print(__doc__)
        return 2
    img = bytearray(open(sys.argv[1], "rb").read())
    segs = segments(img)
    pad = re.compile(r"^\s+([0-9a-f]+):\s+\((\d+) zero bytes of padding\)")
    bad = 0
    n = 0
    for line in open(sys.argv[2], encoding="utf-8", errors="replace"):
        m = pad.match(line)
        if not m:
            continue
        addr = int(m.group(1), 16)
        count = int(m.group(2))
        for vaddr, size, off in segs:
            if vaddr <= addr and addr + count <= vaddr + size:
                at = off + (addr - vaddr)
                if any(img[at:at + count]):
                    print("disasm_pad: %x: %d bytes called padding are not all zero" % (addr, count))
                    bad = 1
                else:
                    img[at:at + count] = b"\x90" * count
                    n += 1
                break
        else:
            print("disasm_pad: %x: not in a loaded part of the image" % addr)
            bad = 1
    open(sys.argv[3], "wb").write(img)
    print("disasm_pad: %d padding runs as nops" % n)
    return bad


if __name__ == "__main__":
    sys.exit(main())
