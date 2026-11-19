#!/usr/bin/env python3
"""SP7 (docs/reference/incremental-relink.md): the function symbols of two
machine-layer ELF images (compiler/nat_enc.fd nat_elf_debug: .symtab of
FUNC symbols at their addresses), compared.

    python3 tests/nattools/relink_syms.py IMAGE1 IMAGE2

prints "common N moved M" and then one line per function present in both
whose address differs ("moved NAME 0xA -> 0xB"). Exit 2 when an image has
no .symtab."""
import struct
import sys


def func_syms(path):
    data = open(path, "rb").read()
    if data[:4] != b"\x7fELF" or data[4] != 2:
        return None
    shoff = struct.unpack_from("<Q", data, 0x28)[0]
    shentsize, shnum = struct.unpack_from("<HH", data, 0x3A)
    secs = []
    for i in range(shnum):
        base = shoff + i * shentsize
        (name, typ, flags, addr, off, size, link, info, align, entsize) = struct.unpack_from("<IIQQQQIIQQ", data, base)
        secs.append((typ, off, size, link, entsize))
    out = {}
    found = False
    for (typ, off, size, link, entsize) in secs:
        if typ != 2:  # SHT_SYMTAB
            continue
        found = True
        stroff = secs[link][1]
        n = size // (entsize or 24)
        for k in range(n):
            (st_name, st_info, st_other, st_shndx, st_value, st_size) = struct.unpack_from("<IBBHQQ", data, off + k * 24)
            if (st_info & 15) != 2:  # STT_FUNC
                continue
            end = data.index(b"\0", stroff + st_name)
            nm = data[stroff + st_name:end].decode("utf-8", "replace")
            out[nm] = st_value
    return out if found else None


def main():
    if len(sys.argv) != 3:
        print(__doc__)
        return 2
    a = func_syms(sys.argv[1])
    b = func_syms(sys.argv[2])
    if a is None or b is None:
        print("no .symtab")
        return 2
    common = sorted(set(a) & set(b))
    moved = [n for n in common if a[n] != b[n]]
    print("common %d moved %d" % (len(common), len(moved)))
    for n in moved:
        print("moved %s 0x%x -> 0x%x" % (n, a[n], b[n]))
    return 0


if __name__ == "__main__":
    sys.exit(main())
