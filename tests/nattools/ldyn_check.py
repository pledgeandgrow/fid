#!/usr/bin/env python3
"""TG15 check: a native Linux executable that imports C functions
(tests/bootstrap/test_machine_ldyn_targets.fd, test_machine_cimport.fd)
carries the dynamic linker's tables for its machine
(compiler/nat_ir.fd ng_gen_ldyn, ng_ldyn_rel_type, ng_ldyn_interp).

    fid emit tests/bootstrap/test_machine_ldyn_targets.fd --target aarch64-linux -o x
    python3 tests/nattools/ldyn_check.py x [name ...]

Checks, reading the program headers only:
  - PT_INTERP holds the loader of e_machine's glibc;
  - PT_DYNAMIC has DT_NEEDED libc.so.6 (and libm.so.6 when `hypot` or
    `ldexp` is imported), DT_STRTAB, DT_SYMTAB, DT_HASH, DT_RELA with
    DT_RELAENT 24, DT_FLAGS with DF_BIND_NOW;
  - every relocation is the machine's GOT binding (R_X86_64_GLOB_DAT 6,
    R_AARCH64_GLOB_DAT 1025, R_RISCV_64 2) against an undefined global
    function symbol, and every name given on the command line (default:
    atoi hypot ldexp) is one of them.
With readelf on PATH also runs `readelf -d -r` (it must parse the file).
Exit 0 when every check holds."""
import shutil
import struct
import subprocess
import sys

MACH = {62: ("/lib64/ld-linux-x86-64.so.2", 6),
        183: ("/lib/ld-linux-aarch64.so.1", 1025),
        243: ("/lib/ld-linux-riscv64-lp64d.so.1", 2)}


def main():
    path = sys.argv[1]
    want = sys.argv[2:] or ["atoi", "hypot", "ldexp"]
    d = open(path, "rb").read()
    fails = []
    if d[:4] != b"\x7fELF":
        print("FAIL: not an ELF file")
        return 1
    mach = struct.unpack_from("<H", d, 18)[0]
    if mach not in MACH:
        print("FAIL: e_machine %d" % mach)
        return 1
    interp_want, rel_want = MACH[mach]
    phoff = struct.unpack_from("<Q", d, 32)[0]
    phentsize, phnum = struct.unpack_from("<HH", d, 54)
    loads = []
    interp = None
    dyn = None
    for i in range(phnum):
        p_type, _, p_off, p_vaddr, _, p_filesz, p_memsz, _ = struct.unpack_from("<IIQQQQQQ", d, phoff + i * phentsize)
        if p_type == 1:
            loads.append((p_vaddr, p_off, p_filesz))
        elif p_type == 3:
            interp = d[p_off:p_off + p_filesz].rstrip(b"\0").decode()
        elif p_type == 2:
            dyn = (p_off, p_filesz)

    def at(vaddr):
        for va, off, sz in loads:
            if va <= vaddr < va + sz:
                return off + vaddr - va
        raise ValueError("address 0x%x outside every PT_LOAD" % vaddr)

    if interp != interp_want:
        fails.append("PT_INTERP %r, want %r" % (interp, interp_want))
    if dyn is None:
        print("FAIL: no PT_DYNAMIC")
        return 1
    tags = {}
    needed = []
    p = dyn[0]
    while p + 16 <= dyn[0] + dyn[1]:
        tag, val = struct.unpack_from("<qQ", d, p)
        p += 16
        if tag == 0:
            break
        if tag == 1:
            needed.append(val)
        else:
            tags[tag] = val
    for t, n in ((5, "DT_STRTAB"), (6, "DT_SYMTAB"), (4, "DT_HASH"), (7, "DT_RELA"), (8, "DT_RELASZ"), (10, "DT_STRSZ")):
        if t not in tags:
            fails.append("no " + n)
    if tags.get(9) != 24:
        fails.append("DT_RELAENT %r" % tags.get(9))
    if not (tags.get(30, 0) & 8):
        fails.append("DT_FLAGS without DF_BIND_NOW")
    if fails:
        for f in fails:
            print("FAIL: " + f)
        return 1
    strtab = at(tags[5])

    def cstr(off):
        s = strtab + off
        return d[s:d.index(b"\0", s)].decode()

    libs = [cstr(o) for o in needed]
    if "libc.so.6" not in libs:
        fails.append("DT_NEEDED %r without libc.so.6" % libs)
    if ("hypot" in want or "ldexp" in want) and "libm.so.6" not in libs:
        fails.append("DT_NEEDED %r without libm.so.6" % libs)
    symtab = at(tags[6])
    rela = at(tags[7])
    names = []
    for k in range(tags[8] // 24):
        r_off, r_info, r_add = struct.unpack_from("<QQq", d, rela + 24 * k)
        rtype = r_info & 0xFFFFFFFF
        rsym = r_info >> 32
        if rtype != rel_want:
            fails.append("relocation %d: type %d, want %d" % (k, rtype, rel_want))
        st_name, st_info, _, st_shndx = struct.unpack_from("<IBBH", d, symtab + 24 * rsym)
        if st_info != 0x12 or st_shndx != 0:
            fails.append("relocation %d: symbol %d not an undefined global function" % (k, rsym))
        names.append(cstr(st_name))
        try:
            at(r_off)
        except ValueError as e:
            fails.append("relocation %d: GOT slot %s" % (k, e))
    for w in want:
        if w not in names:
            fails.append("%s is not imported (imports: %r)" % (w, names))
    re_ = shutil.which("readelf")
    if re_:
        r = subprocess.run([re_, "-d", "-r", "-W", path], capture_output=True, text=True)
        if r.returncode != 0 or "Error" in r.stderr:
            fails.append("readelf: " + r.stderr.strip()[-300:])
    for f in fails:
        print("FAIL: " + f)
    if not fails:
        print("ldyn OK: %s, %d imports (%s), needs %s" % (interp, len(names), " ".join(names), " ".join(libs)))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
