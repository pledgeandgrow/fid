#!/usr/bin/env python3
"""pe_imports_check.py — TG15 (docs/roadmap/llvm-roadmap.md): the import
table of a PE32+ executable, as the Windows loader reads it.

Usage: python3 tests/nattools/pe_imports_check.py <exe> [dll!function ...]
       python3 tests/nattools/pe_imports_check.py <exe> --list

Walks the import directory (data directory 1): each descriptor's DLL name,
its lookup table (by name: hint/name RVA; by ordinal: the high bit) and its
address table, which must have one 8-byte entry per lookup entry, zero
ended, inside the image. Each `dll!function` argument (DLL name compared
without case) must be imported. --list prints every import as dll!name.
Exit 0 and "pe_imports_check: ok (N imports from M DLLs)" when the table is
well formed and every named import is there; else the problems, exit 1.
"""
import struct
import sys


def imports(path):
    b = open(path, 'rb').read()
    if b[:2] != b'MZ':
        raise SystemExit('pe_imports_check: %s is not a PE file' % path)
    pe = struct.unpack_from('<I', b, 0x3C)[0]
    if b[pe:pe + 4] != b'PE\0\0':
        raise SystemExit('pe_imports_check: no PE signature')
    nsec = struct.unpack_from('<H', b, pe + 6)[0]
    optsz = struct.unpack_from('<H', b, pe + 20)[0]
    opt = pe + 24
    if struct.unpack_from('<H', b, opt)[0] != 0x20B:
        raise SystemExit('pe_imports_check: not PE32+')
    dd = opt + 112
    irva, isize = struct.unpack_from('<II', b, dd + 8)
    secs = []
    so = opt + optsz
    for i in range(nsec):
        vs, va, rs, ro = struct.unpack_from('<IIII', b, so + 40 * i + 8)
        secs.append((va, max(vs, rs), ro, rs))

    def off(rva):
        for va, sz, ro, rs in secs:
            if va <= rva < va + sz:
                if rva - va >= rs:
                    raise SystemExit('pe_imports_check: RVA %#x is past the raw data' % rva)
                return rva - va + ro
        raise SystemExit('pe_imports_check: RVA %#x is in no section' % rva)

    def cstr(rva):
        o = off(rva)
        return b[o:b.index(b'\0', o)].decode('latin-1')

    if irva == 0 or isize == 0:
        return []
    out = []
    d = off(irva)
    while True:
        ilt, _, _, name, iat = struct.unpack_from('<IIIII', b, d)
        if ilt == 0 and name == 0 and iat == 0:
            break
        dll = cstr(name)
        lo = off(ilt if ilt else iat)
        ao = off(iat)
        k = 0
        while True:
            e = struct.unpack_from('<Q', b, lo + 8 * k)[0]
            a = struct.unpack_from('<Q', b, ao + 8 * k)[0]
            if e == 0:
                if a != 0:
                    raise SystemExit('pe_imports_check: %s: the address table is longer than the lookup table' % dll)
                break
            if a != e:
                raise SystemExit('pe_imports_check: %s entry %d: the address table (%#x) differs from the lookup table (%#x) before binding' % (dll, k, a, e))
            if e & (1 << 63):
                out.append((dll, '#%d' % (e & 0xFFFF)))
            else:
                out.append((dll, cstr((e & 0x7FFFFFFF) + 2)))
            k += 1
        d += 20
    return out


def main():
    if len(sys.argv) < 2:
        raise SystemExit(__doc__)
    imps = imports(sys.argv[1])
    if '--list' in sys.argv[2:]:
        for dll, fn in imps:
            print('%s!%s' % (dll, fn))
    have = set((dll.lower(), fn) for dll, fn in imps)
    bad = 0
    for want in sys.argv[2:]:
        if want == '--list':
            continue
        dll, _, fn = want.partition('!')
        if (dll.lower(), fn) not in have:
            print('missing import %s' % want)
            bad += 1
    if bad:
        raise SystemExit(1)
    print('pe_imports_check: ok (%d imports from %d DLLs)' % (len(imps), len(set(d.lower() for d, _ in imps))))


if __name__ == '__main__':
    main()
