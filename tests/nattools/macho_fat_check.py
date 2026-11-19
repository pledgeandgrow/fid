#!/usr/bin/env python3
"""macho_fat_check.py <file> — checks a macOS universal binary the way
`lipo -info` reads it (L2B, docs/roadmap/roadmap.md): a big-endian fat
header (0xCAFEBABE), one x86_64 and one arm64 slice, each at an offset
aligned to 2^align inside the file, each a 64-bit Mach-O (0xFEEDFACF)
whose own header names the CPU type its fat_arch entry gives. Prints
"x86_64 arm64" (lipo's architecture list) and exits 0 when it holds."""
import struct
import sys

NAMES = {0x01000007: 'x86_64', 0x0100000C: 'arm64'}


def main(path):
    data = open(path, 'rb').read()
    if len(data) < 8:
        return 'too short'
    magic, n = struct.unpack_from('>II', data, 0)
    if magic != 0xCAFEBABE:
        return 'not a fat binary: magic %08x' % magic
    if n != 2:
        return '%d architectures, not 2' % n
    seen = []
    end = 8 + 20 * n
    for k in range(n):
        cpu, sub, off, size, align = struct.unpack_from('>IIIII', data, 8 + 20 * k)
        if cpu not in NAMES:
            return 'unknown cputype %08x' % cpu
        if off % (1 << align) != 0:
            return '%s at %d, not aligned to 2^%d' % (NAMES[cpu], off, align)
        if off < end or off + size > len(data):
            return '%s slice [%d, %d) outside the file or over the header' % (NAMES[cpu], off, off + size)
        m_magic, m_cpu = struct.unpack_from('<II', data, off)
        if m_magic != 0xFEEDFACF:
            return '%s slice is not a 64-bit Mach-O (magic %08x)' % (NAMES[cpu], m_magic)
        if m_cpu != cpu:
            return '%s slice says cputype %08x' % (NAMES[cpu], m_cpu)
        end = off + size
        seen.append(NAMES[cpu])
    if sorted(seen) != ['arm64', 'x86_64']:
        return 'architectures ' + ' '.join(seen)
    print(' '.join(seen))
    return None


if __name__ == '__main__':
    err = main(sys.argv[1])
    if err:
        print('macho_fat_check: ' + err)
        sys.exit(1)
