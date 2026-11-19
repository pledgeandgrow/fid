#!/usr/bin/env python3
"""CT9 check: the .debug_info of a native Linux build of
tests/bootstrap/test_machine_dbg_names.fd names the method as Fides writes
it (Point::len2, linkage name Point_len2) and holds struct Point and struct
Seg with their fields (x, y; a, b, tag), Seg.a referring to Point's entry.

    fid emit tests/bootstrap/test_machine_dbg_names.fd --target x86_64-linux -o t
    python3 tests/nattools/dbg_names_check.py t

Reads the ELF section table only (no pyelftools); exit 0 when every check
holds, 1 otherwise."""
import struct
import sys


def section(data, want):
    if data[:4] != b"\x7fELF":
        # CT9: a Mach-O (__DWARF) or `--nat` PE (COFF) image
        import os
        sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
        import dbg_sections
        return dbg_sections.section(data, want)
    shoff = struct.unpack_from("<Q", data, 0x28)[0]
    shentsize, shnum, shstrndx = struct.unpack_from("<HHH", data, 0x3A)
    def hdr(i):
        return struct.unpack_from("<IIQQQQIIQQ", data, shoff + i * shentsize)
    stroff = hdr(shstrndx)[4]
    for i in range(shnum):
        h = hdr(i)
        name = data[stroff + h[0]:data.index(b"\0", stroff + h[0])].decode()
        if name == want:
            return data[h[4]:h[4] + h[5]]
    return None


def main():
    data = open(sys.argv[1], "rb").read()
    info = section(data, ".debug_info")
    abbrev = section(data, ".debug_abbrev")
    fails = []
    if info is None or abbrev is None:
        print("FAIL: no .debug_info/.debug_abbrev")
        return 1
    # abbreviation 6: a function with a linkage name (0x6E); with DB2/DB4
    # (variables, frame base) on, abbreviation 8: the same with children
    if bytes([6, 0x2E, 0, 0x03, 0x08, 0x6E, 0x08]) not in abbrev and bytes([8, 0x2E, 1, 0x03, 0x08, 0x6E, 0x08]) not in abbrev:
        fails.append("abbreviation 6 or 8 (subprogram + linkage_name) missing")
    shown = b"\x06Point::len2\0Point_len2\0"
    if shown not in info and b"\x08Point::len2\0Point_len2\0" not in info:
        fails.append("Point::len2 with linkage name Point_len2 missing")
    for st, fields in ((b"Point", [b"x", b"y"]), (b"Seg", [b"a", b"b", b"tag"])):
        at = info.find(b"\x03" + st + b"\0")
        if at < 0:
            fails.append("struct %s missing" % st.decode())
            continue
        p = at + 1 + len(st) + 1 + 4
        for f in fields:
            if info[p] != 4 or info[p + 1:p + 2 + len(f)] != f + b"\0":
                fails.append("struct %s: member %s missing" % (st.decode(), f.decode()))
                break
            p += 1 + len(f) + 1 + 4 + 4
    seg = info.find(b"\x03Seg\0")
    point = info.find(b"\x03Point\0")
    if seg >= 0 and point >= 0:
        ref = struct.unpack_from("<I", info, seg + 5 + 4 + 1 + 2)[0]
        if ref != point:
            fails.append("Seg.a does not refer to struct Point (%d, Point at %d)" % (ref, point))
    for f in fails:
        print("FAIL: " + f)
    if not fails:
        print("dbg names OK")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
