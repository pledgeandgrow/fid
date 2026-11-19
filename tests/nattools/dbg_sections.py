#!/usr/bin/env python3
"""The DWARF sections of a machine-layer image, whatever its format (CT9,
docs/roadmap/complaint-roadmap.md): an ELF (`.debug_info`, nat_enc.fd
nat_elf_debug), a `--nat` PE (COFF sections named "/<n>" in the string
table, nat_pe_debug) or a Mach-O (`__debug_info` in the __DWARF segment,
nat_macho_debug_cfi). Used by dbg_names_check.py and dbg_vars_check.py.

    python3 tests/nattools/dbg_sections.py IMAGE

prints the format and each debug section with its size."""
import struct
import sys


def image_format(data):
    if data[:4] == b"\x7fELF":
        return "elf"
    if data[:2] == b"MZ":
        return "pe"
    if len(data) >= 4 and struct.unpack_from("<I", data, 0)[0] == 0xFEEDFACF:
        return "macho"
    return "?"


def elf_sections(data):
    out = {}
    shoff = struct.unpack_from("<Q", data, 0x28)[0]
    shentsize, shnum, shstrndx = struct.unpack_from("<HHH", data, 0x3A)

    def hdr(i):
        return struct.unpack_from("<IIQQQQIIQQ", data, shoff + i * shentsize)
    stroff = hdr(shstrndx)[4]
    for i in range(shnum):
        h = hdr(i)
        name = data[stroff + h[0]:data.index(b"\0", stroff + h[0])].decode("latin-1")
        out[name] = data[h[4]:h[4] + h[5]]
    return out


def pe_sections(data):
    out = {}
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    nsec = struct.unpack_from("<H", data, pe + 6)[0]
    symptr, nsyms = struct.unpack_from("<II", data, pe + 12)
    optsize = struct.unpack_from("<H", data, pe + 20)[0]
    strtab = symptr + 18 * nsyms if symptr else 0
    at = pe + 24 + optsize
    for i in range(nsec):
        raw = data[at:at + 8].rstrip(b"\0").decode("latin-1")
        vsize, _rva, rawsize, rawoff = struct.unpack_from("<IIII", data, at + 8)
        name = raw
        if raw.startswith("/") and strtab:
            try:
                o = strtab + int(raw[1:])
                name = data[o:data.index(b"\0", o)].decode("latin-1")
            except ValueError:
                pass
        size = vsize if vsize and vsize <= rawsize else rawsize
        out[name] = data[rawoff:rawoff + size]
        at += 40
    return out


def macho_sections(data):
    out = {}
    ncmds = struct.unpack_from("<I", data, 16)[0]
    at = 32
    for _ in range(ncmds):
        cmd, size = struct.unpack_from("<II", data, at)
        if cmd == 0x19:
            nsec = struct.unpack_from("<I", data, at + 64)[0]
            for k in range(nsec):
                so = at + 72 + 80 * k
                sn = data[so:so + 16].rstrip(b"\0").decode("latin-1")
                _addr, ssz, soff = struct.unpack_from("<QQI", data, so + 32)
                # __debug_info -> .debug_info, as the ELF names it
                name = "." + sn[2:] if sn.startswith("__") else sn
                out[name] = data[soff:soff + ssz]
        at += size
    return out


def sections(data):
    f = image_format(data)
    if f == "elf":
        return elf_sections(data)
    if f == "pe":
        return pe_sections(data)
    if f == "macho":
        return macho_sections(data)
    return {}


def section(data, want):
    """The bytes of the section named `want` (ELF spelling, ".debug_info"),
    None when the image has none."""
    return sections(data).get(want)


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        return 2
    data = open(sys.argv[1], "rb").read()
    print("format " + image_format(data))
    for name, body in sorted(sections(data).items()):
        if name.startswith(".debug"):
            print("%s %d" % (name, len(body)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
