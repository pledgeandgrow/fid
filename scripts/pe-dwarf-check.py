#!/usr/bin/env python3
"""pe-dwarf-check.py IMAGE [SOURCE_NAME]: the DWARF line tables of a debug
build of the default Windows backend (llvm-roadmap DB3, release N9;
compiler/pe_ir_enc.fd pe_add_debug_sections).

Checks that the PE image has .debug_abbrev, .debug_info and .debug_line
sections (long names: "/<offset>" into the COFF string table), that the
headers stay consistent (sections in file and address order, SizeOfImage
covers them, SizeOfHeaders 1024), that .debug_info's compile unit is DWARF
4 and names SOURCE_NAME (when given), and that the line program decodes
to rows whose addresses fall in .text, with at least one sequence. When
llvm-dwarfdump is on PATH it also has to read the line table.
"""
import shutil
import struct
import subprocess
import sys

IMAGE_BASE = 0x140000000


def u16(b, o):
    return struct.unpack_from("<H", b, o)[0]


def u32(b, o):
    return struct.unpack_from("<I", b, o)[0]


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


def fail(msg):
    print("  pe-dwarf-check: " + msg)
    sys.exit(1)


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        sys.exit(2)
    img = open(sys.argv[1], "rb").read()
    want_name = sys.argv[2] if len(sys.argv) > 2 else ""
    pe = u32(img, 0x3C)
    if img[pe:pe + 4] != b"PE\0\0":
        fail("not a PE image")
    nsec = u16(img, pe + 6)
    symptr = u32(img, pe + 12)
    nsyms = u32(img, pe + 16)
    opt = pe + 24
    size_of_image = u32(img, opt + 56)
    size_of_headers = u32(img, opt + 60)
    if size_of_headers != 1024:
        fail("SizeOfHeaders is %d, not 1024" % size_of_headers)
    if symptr == 0:
        fail("no COFF symbol/string table (the section names live there)")
    strtab = symptr + 18 * nsyms
    secs = {}
    last_va = 0
    last_raw = 0
    sh = opt + u16(img, pe + 20)
    for i in range(nsec):
        h = sh + 40 * i
        raw_name = img[h:h + 8].rstrip(b"\0").decode("latin-1")
        if raw_name.startswith("/"):
            off = int(raw_name[1:])
            end = img.index(b"\0", strtab + off)
            name = img[strtab + off:end].decode("latin-1")
        else:
            name = raw_name
        vsize, va, rsize, rptr = struct.unpack_from("<IIII", img, h + 8)
        if va < last_va or rptr < last_raw:
            fail("section %s out of order" % name)
        last_va, last_raw = va, rptr
        if va + vsize > size_of_image:
            fail("section %s ends past SizeOfImage" % name)
        if rptr + rsize > len(img):
            fail("section %s ends past the file" % name)
        secs[name] = (va, vsize, img[rptr:rptr + vsize])
    for need in (".text", ".rdata", ".debug_abbrev", ".debug_info", ".debug_line"):
        if need not in secs:
            fail("no %s section (have %s)" % (need, ", ".join(sorted(secs))))
    text_va, text_size, _ = secs[".text"]
    lo, hi = IMAGE_BASE + text_va, IMAGE_BASE + text_va + text_size

    info = secs[".debug_info"][2]
    if u16(info, 4) != 4 or info[10] != 8:
        fail(".debug_info is not a DWARF 4 unit with 8-byte addresses")
    o = 11
    if info[o] != 1:
        fail(".debug_info does not start with the compile unit")
    o += 1
    end = info.index(b"\0", o)
    o = end + 1
    o += 2
    end = info.index(b"\0", o)
    cu_name = info[o:end].decode("latin-1")
    if want_name and not cu_name.replace("\\", "/").endswith(want_name):
        fail("the compile unit is %r, not %r" % (cu_name, want_name))

    line = secs[".debug_line"][2]
    if u16(line, 4) != 4:
        fail(".debug_line is not version 4")
    hlen = u32(line, 6)
    opbase = line[10 + 5]
    o = 10 + hlen
    rows, seqs = 0, 0
    addr, ln = 0, 1
    while o < len(line):
        op = line[o]
        o += 1
        if op == 0:
            n, o = uleb(line, o)
            sub = line[o]
            if sub == 1:
                seqs += 1
                addr, ln = 0, 1
            elif sub == 2:
                addr = struct.unpack_from("<Q", line, o + 1)[0]
            o += n
        elif op == 1:
            if not (lo <= addr < hi):
                fail("a line row at 0x%x is outside .text" % addr)
            rows += 1
        elif op == 2:
            d, o = uleb(line, o)
            addr += d
        elif op == 3:
            d, o = sleb(line, o)
            ln += d
            if ln <= 0:
                fail("a line number below 1")
        elif op < opbase:
            fail("standard opcode %d not written by pe_dwarf_line" % op)
        else:
            fail("special opcode %d not written by pe_dwarf_line" % op)
    if rows == 0 or seqs == 0:
        fail("the line program has %d rows in %d sequences" % (rows, seqs))
    print("  pe-dwarf-check: %s, %d line rows in %d sequences" % (cu_name, rows, seqs))

    dd = shutil.which("llvm-dwarfdump")
    if dd:
        r = subprocess.run([dd, "--debug-line", sys.argv[1]], capture_output=True, text=True)
        if r.returncode != 0 or "0x0000000140" not in r.stdout:
            fail("llvm-dwarfdump did not read the line table:\n" + r.stdout[-2000:] + r.stderr[-2000:])
        print("  pe-dwarf-check: llvm-dwarfdump reads it")


if __name__ == "__main__":
    main()
