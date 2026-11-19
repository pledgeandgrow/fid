#!/usr/bin/env python3
"""TG12: check a Mach-O program linked from nat_object_unit.fd's Mach-O
object (compiler/nat_enc.fd nat_macho_object) without running it.

Usage: tg12_macho_check.py <linked binary> x64|a64

The entry (LC_MAIN) must be the object's fid_main; its first reference to
the data (x86-64: lea rsi, [rip + d] after push rbp; AArch64: adrp + add
x1) must reach "macho object ok\n" after the linker moved the data, and on
AArch64 the adrp + add x10 inside the code (no relocation: same section)
must still reach the word 60 bytes after the entry. The imports _write and
_exit must be named in the binary (bound by dyld). Prints "ok ..." lines;
exit status 1 on the first failure.
"""
import struct
import sys

MSG = b"macho object ok\n"


def fail(why):
    print("FAIL " + why)
    sys.exit(1)


def main():
    path, arch = sys.argv[1], sys.argv[2]
    b = open(path, "rb").read()
    magic, cpu, _sub, ftype, ncmds, _size, _flags, _r = struct.unpack_from("<IiiIIIII", b, 0)
    if magic != 0xFEEDFACF or ftype != 2:
        fail("not a 64-bit Mach-O executable")
    segs = []
    entryoff = None
    at = 32
    for _ in range(ncmds):
        cmd, size = struct.unpack_from("<II", b, at)
        if cmd == 0x19:
            name = b[at + 8:at + 24].rstrip(b"\0").decode()
            vmaddr, vmsize, fileoff, filesize = struct.unpack_from("<QQQQ", b, at + 24)
            segs.append((name, vmaddr, vmsize, fileoff, filesize))
        elif cmd == 0x80000028:
            entryoff = struct.unpack_from("<Q", b, at + 8)[0]
        at += size
    if entryoff is None:
        fail("no LC_MAIN")

    def vm_of(off):
        for _n, vm, _vs, fo, fs in segs:
            if fo <= off < fo + fs:
                return vm + off - fo
        fail("file offset %#x outside the segments" % off)

    def off_of(vm):
        for _n, v, vs, fo, fs in segs:
            if v <= vm < v + min(vs, fs):
                return fo + vm - v
        fail("address %#x outside the file" % vm)

    entry_vm = vm_of(entryoff)
    print("ok entry at %#x" % entry_vm)

    def adrp_add(pos):
        adrp, add = struct.unpack_from("<II", b, pos)
        if (adrp & 0x9F000000) != 0x90000000:
            fail("no adrp at %#x" % pos)
        imm = ((adrp >> 29) & 3) | (((adrp >> 5) & 0x7FFFF) << 2)
        if imm & (1 << 20):
            imm -= 1 << 21
        pc = vm_of(pos)
        return (pc & ~0xFFF) + (imm << 12) + ((add >> 10) & 0xFFF)

    if arch == "x64":
        if b[entryoff] != 0x55 or b[entryoff + 1:entryoff + 4] != b"\x48\x8d\x35":
            fail("entry does not start with push rbp; lea rsi, [rip + d]")
        d = struct.unpack_from("<i", b, entryoff + 4)[0]
        target = vm_of(entryoff + 8) + d
    else:
        target = adrp_add(entryoff)
        tab = adrp_add(entryoff + 32)
        if tab != entry_vm + 60:
            fail("the in-code adrp reaches %#x, not %#x" % (tab, entry_vm + 60))
        print("ok in-code adrp unchanged")
    got = b[off_of(target):off_of(target) + len(MSG)]
    if got != MSG:
        fail("the data reference reaches %r" % got)
    print("ok code reaches the data")
    for name in (b"_write", b"_exit"):
        if name not in b:
            fail("import %s not named" % name.decode())
    print("ok imports named")


if __name__ == "__main__":
    main()
