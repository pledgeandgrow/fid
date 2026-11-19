#!/usr/bin/env python3
"""TG14 (docs/roadmap/llvm-roadmap.md): the program headers of a Linux
executable written by compiler/nat_enc.fd nat_elf.

  python3 tests/nattools/elf_segments_check.py [--dyn|--exec] [--rwx] <elf>...

Default (the split, FID_NAT_OFF=wxsplit not set): every PT_LOAD is either
read/execute (the headers and the code, file offset 0) or read/write (the
data, from a page boundary), none is read/write/execute, there is exactly
one of each, and a PT_GNU_STACK without execute. --rwx: the old layout
instead (one RWX PT_LOAD, FID_NAT_OFF=wxsplit). --dyn / --exec: e_type must
be ET_DYN (FID_NAT_ON=pie, --pie) / ET_EXEC. An ET_DYN with PT_INTERP
(position-independent and dynamically linked) must start its program
headers with a PT_PHDR covering them all. Exit 1 on the first failure.
"""
import struct
import sys

PT_LOAD = 1
PT_INTERP = 3
PT_PHDR = 6
PT_GNU_STACK = 0x6474E551
PF_X, PF_W, PF_R = 1, 2, 4


def phdrs(data):
    if data[:4] != b"\x7fELF" or data[4] != 2 or data[5] != 1:
        raise ValueError("not a little-endian ELF64 file")
    e_type, e_machine = struct.unpack_from("<HH", data, 16)
    e_phoff = struct.unpack_from("<Q", data, 32)[0]
    e_phentsize, e_phnum = struct.unpack_from("<HH", data, 54)
    out = []
    for i in range(e_phnum):
        o = e_phoff + i * e_phentsize
        p_type, p_flags, p_offset, p_vaddr, p_paddr, p_filesz, p_memsz, p_align = struct.unpack_from("<IIQQQQQQ", data, o)
        out.append((p_type, p_flags, p_offset, p_vaddr, p_filesz, p_memsz, p_align))
    return e_type, e_machine, out


def check(path, want_type, rwx):
    data = open(path, "rb").read()
    e_type, mach, ph = phdrs(data)
    errs = []
    if want_type == "dyn" and e_type != 3:
        errs.append("e_type %d, want ET_DYN" % e_type)
    if want_type == "exec" and e_type != 2:
        errs.append("e_type %d, want ET_EXEC" % e_type)
    loads = [p for p in ph if p[0] == PT_LOAD]
    page = 0x10000 if mach == 183 else 0x1000
    if rwx:
        if len(loads) != 1 or loads[0][1] != PF_R | PF_W | PF_X:
            errs.append("want one RWX PT_LOAD, got %s" % [(hex(p[1])) for p in loads])
    else:
        if any(p[1] & PF_W and p[1] & PF_X for p in loads):
            errs.append("a PT_LOAD is writable and executable")
        rx = [p for p in loads if p[1] == PF_R | PF_X]
        rw = [p for p in loads if p[1] == PF_R | PF_W]
        if len(rx) != 1 or len(rw) != 1 or len(loads) != 2:
            errs.append("want one R+X and one R+W PT_LOAD, got flags %s" % [p[1] for p in loads])
        else:
            if rx[0][2] != 0:
                errs.append("the code segment does not start at file offset 0")
            if rw[0][3] % page != 0:
                errs.append("the data segment's address %#x is not on a %#x page" % (rw[0][3], page))
            if rx[0][3] + rx[0][4] > rw[0][3]:
                errs.append("the code segment runs into the data segment")
            if rw[0][2] + rw[0][4] > len(data):
                errs.append("the data segment runs past the file")
            if (rw[0][3] - rw[0][2]) % rw[0][6] != 0:
                errs.append("the data segment's address and offset disagree modulo its alignment")
        stacks = [p for p in ph if p[0] == PT_GNU_STACK]
        if len(stacks) != 1 or stacks[0][1] & PF_X:
            errs.append("want one non-executable PT_GNU_STACK")
    # A position-independent executable with a dynamic loader: glibc's
    # ld.so takes the load bias from PT_PHDR (AT_PHDR - its p_vaddr), so it
    # must come first and cover every program header
    if e_type == 3 and any(p[0] == PT_INTERP for p in ph):
        if not ph or ph[0][0] != PT_PHDR:
            errs.append("ET_DYN with PT_INTERP but no PT_PHDR first")
        elif ph[0][4] != 56 * len(ph) or ph[0][2] != 64:
            errs.append("PT_PHDR at %#x, %d bytes: not the %d program headers" % (ph[0][2], ph[0][4], len(ph)))
    for e in errs:
        print("FAIL %s: %s" % (path, e))
    if not errs:
        kinds = ",".join("%s/%d" % ("LOAD" if p[0] == PT_LOAD else hex(p[0]), p[1]) for p in ph)
        print("ok   %s: e_type %d, %s" % (path, e_type, kinds))
    return not errs


def main(argv):
    want = None
    rwx = False
    files = []
    for a in argv:
        if a == "--dyn":
            want = "dyn"
        elif a == "--exec":
            want = "exec"
        elif a == "--rwx":
            rwx = True
        else:
            files.append(a)
    if not files:
        print(__doc__)
        return 2
    ok = True
    for f in files:
        ok = check(f, want, rwx) and ok
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
