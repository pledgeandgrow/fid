#!/usr/bin/env python3
# bin-target-check.py - is each file an executable for the given target?
# (competitive-roadmap PL2/PL3: a target this host cannot run is built
# only; this reads what the target's loader reads first and fails on a
# binary for the wrong system or architecture.)
#
# Usage: python3 scripts/bin-target-check.py <target> <file> [<file> ...]
#   target: <arch>-<os> as `fid ccompile --target` writes it:
#           x86_64-windows aarch64-windows        PE32+ console executable,
#                                                 machine AMD64 / ARM64
#           x86_64-linux aarch64-linux riscv64-linux
#                                                 ELF64 LE executable
#                                                 (ET_EXEC or PIE ET_DYN with
#                                                 an entry), no FreeBSD tag
#           x86_64-freebsd aarch64-freebsd        ELF64 tagged FreeBSD (OSABI
#                                                 9 or the FreeBSD ABI note),
#                                                 interpreter
#                                                 /libexec/ld-elf.so.1 if any
#           x86_64-macos aarch64-macos            Mach-O 64 MH_EXECUTE of the
#                                                 cpu (tools/macho_check.fd
#                                                 checks the rest)
#           wasm32-wasi                           a WebAssembly module
#   A Linux or FreeBSD ELF that imports one of the runtime's own names
#   from a shared library (nat_*, pert_*, pe_*, p_*, ...: every executable
#   defines them) fails too: the loader stops it with "symbol lookup error"
#   (PL2: nat_tls_set, a primitive of compiler/nat_ir.fd, was bound
#   dynamically by the CI2 path).
#   --quiet  print only failures and the summary
#   -        read the file names from stdin, one per line, after any given
#            (a lane's binaries are more than Windows' command line holds)
#   --loader print, per file, "<file>\t<interpreter or ->\t<runtime imports>"
#            and nothing else (scripts/cross-test.sh: which binaries need
#            the target's dynamic loader under qemu-user)
# Exit 0 when every file passes, 1 when one fails, 2 on bad usage.

import struct, sys

ELF_MACHINE = {"x86_64": 62, "aarch64": 183, "riscv64": 243}
# Names every Fides executable defines itself: never a shared library's
RUNTIME_PREFIXES = ("nat_", "pert_", "pet_", "ped_", "pe_", "p_", "__p_")
PE_MACHINE = {"x86_64": 0x8664, "aarch64": 0xAA64}
MACHO_CPU = {"x86_64": 0x01000007, "aarch64": 0x0100000C}


def arch_os(target):
    t = target.lower()
    a, _, rest = t.partition("-")
    a = {"arm64": "aarch64", "x64": "x86_64", "amd64": "x86_64"}.get(a, a)
    for os_name in ("windows", "linux", "freebsd", "macos", "wasi"):
        if os_name in rest or (os_name == "wasi" and a == "wasm32"):
            return a, os_name
    if "darwin" in rest:
        return a, "macos"
    return a, ""


def check_pe(d, arch):
    if d[:2] != b"MZ" or len(d) < 0x40:
        return "no MZ header"
    pe = struct.unpack_from("<I", d, 0x3C)[0]
    if pe + 24 + 0x48 > len(d) or d[pe:pe + 4] != b"PE\0\0":
        return "no PE signature"
    machine = struct.unpack_from("<H", d, pe + 4)[0]
    if machine != PE_MACHINE[arch]:
        return "machine 0x%x, want 0x%x (%s)" % (machine, PE_MACHINE[arch], arch)
    chars = struct.unpack_from("<H", d, pe + 22)[0]
    if not chars & 0x0002:
        return "not an executable image (characteristics 0x%x)" % chars
    opt = pe + 24
    magic = struct.unpack_from("<H", d, opt)[0]
    if magic != 0x20B:
        return "optional header magic 0x%x, want PE32+ 0x20b" % magic
    entry = struct.unpack_from("<I", d, opt + 16)[0]
    if entry == 0:
        return "no entry point"
    subsystem = struct.unpack_from("<H", d, opt + 68)[0]
    if subsystem != 3:
        return "subsystem %d, want console (3)" % subsystem
    return None


def elf_notes_and_interp(d):
    """(names of PT_NOTE notes as (name, type), PT_INTERP string or None)"""
    phoff = struct.unpack_from("<Q", d, 32)[0]
    phentsize, phnum = struct.unpack_from("<HH", d, 54)
    notes, interp = [], None
    for i in range(phnum):
        o = phoff + i * phentsize
        if o + 56 > len(d):
            raise ValueError("program header %d past the end" % i)
        p_type, _flags, p_off, _va, _pa, p_filesz = struct.unpack_from("<IIQQQQ", d, o)
        if p_off + p_filesz > len(d):
            raise ValueError("segment %d past the end of the file" % i)
        if p_type == 3:  # PT_INTERP
            interp = d[p_off:p_off + p_filesz].split(b"\0")[0].decode("latin-1")
        elif p_type == 4:  # PT_NOTE
            q, end = p_off, p_off + p_filesz
            while q + 12 <= end:
                namesz, descsz, ntype = struct.unpack_from("<III", d, q)
                name = d[q + 12:q + 12 + namesz].split(b"\0")[0].decode("latin-1")
                notes.append((name, ntype))
                q += 12 + ((namesz + 3) & ~3) + ((descsz + 3) & ~3)
    return notes, interp


def elf_dynamic(d):
    """(DT_NEEDED libraries, undefined dynamic symbols) of an ELF64 LE file,
    read through PT_DYNAMIC (section headers may be absent); ([], []) for a
    static executable"""
    phoff = struct.unpack_from("<Q", d, 32)[0]
    phentsize, phnum = struct.unpack_from("<HH", d, 54)
    loads, dyn = [], None
    for i in range(phnum):
        o = phoff + i * phentsize
        if o + 56 > len(d):
            break
        p_type, _flags, p_off, p_va, _pa, p_filesz = struct.unpack_from("<IIQQQQ", d, o)
        if p_type == 1:  # PT_LOAD
            loads.append((p_va, p_off, p_filesz))
        elif p_type == 2:  # PT_DYNAMIC
            dyn = (p_off, p_filesz)
    if dyn is None:
        return [], []

    def off(va):
        for v, o, n in loads:
            if v <= va < v + n:
                return o + (va - v)
        return None

    tags, needed = {}, []
    q, end = dyn[0], min(dyn[0] + dyn[1], len(d))
    while q + 16 <= end:
        tag, val = struct.unpack_from("<qQ", d, q)
        if tag == 0:  # DT_NULL
            break
        if tag == 1:  # DT_NEEDED
            needed.append(val)
        else:
            tags.setdefault(tag, val)
        q += 16
    strtab = off(tags[5]) if 5 in tags else None  # DT_STRTAB
    if strtab is None:
        return [], []
    strsz = tags.get(10, len(d) - strtab)  # DT_STRSZ

    def name(i):
        if i >= strsz or strtab + i >= len(d):
            return ""
        z = d.find(b"\0", strtab + i)
        return d[strtab + i:z if z >= 0 else len(d)].decode("latin-1")

    libs = [name(v) for v in needed]
    syms = []
    symtab = off(tags[6]) if 6 in tags else None  # DT_SYMTAB
    if symtab is not None:
        syment = tags.get(11, 24) or 24  # DT_SYMENT
        count = None
        if 4 in tags:  # DT_HASH: nchain is the number of symbols
            h = off(tags[4])
            if h is not None and h + 8 <= len(d):
                count = struct.unpack_from("<I", d, h + 4)[0]
        if count is None and strtab > symtab:
            count = (strtab - symtab) // syment
        for k in range(1, min(count or 0, 1 << 20)):
            so = symtab + k * syment
            if so + 24 > len(d):
                break
            st_name, _info, _other, st_shndx = struct.unpack_from("<IBBH", d, so)
            if st_shndx == 0 and st_name:
                syms.append(name(st_name))
    return libs, syms


def runtime_imports(d):
    """The runtime's own names an ELF imports from a shared library"""
    try:
        _libs, syms = elf_dynamic(d)
    except (struct.error, ValueError):
        return []
    return [s for s in syms if s.startswith(RUNTIME_PREFIXES)]


def check_elf(d, arch, os_name):
    if d[:4] != b"\x7fELF":
        return "no ELF magic"
    if d[4] != 2 or d[5] != 1:
        return "not ELF64 little-endian (class %d, data %d)" % (d[4], d[5])
    e_type, e_machine = struct.unpack_from("<HH", d, 16)
    if e_machine != ELF_MACHINE[arch]:
        return "e_machine %d, want %d (%s)" % (e_machine, ELF_MACHINE[arch], arch)
    entry = struct.unpack_from("<Q", d, 24)[0]
    if e_type not in (2, 3) or entry == 0:
        return "not an executable (e_type %d, entry 0x%x)" % (e_type, entry)
    try:
        notes, interp = elf_notes_and_interp(d)
    except ValueError as e:
        return str(e)
    freebsd = d[7] == 9 or ("FreeBSD", 1) in notes
    if os_name == "freebsd":
        if not freebsd:
            return "no FreeBSD tag (OSABI %d, no FreeBSD ABI note)" % d[7]
        if interp is not None and interp != "/libexec/ld-elf.so.1":
            return "interpreter %s, want /libexec/ld-elf.so.1" % interp
    else:
        if freebsd:
            return "tagged FreeBSD, not Linux"
        if d[7] not in (0, 3):
            return "OSABI %d, want SYSV (0) or GNU (3)" % d[7]
    rt = runtime_imports(d)
    if rt:
        return "imports the runtime's own %s from a shared library (%s%s): " \
               "the loader stops it with 'symbol lookup error'" % (
                   "name" if len(rt) == 1 else "names", ", ".join(rt[:6]),
                   ", ..." if len(rt) > 6 else "")
    return None


def check_macho(d, arch):
    if len(d) < 32 or struct.unpack_from("<I", d, 0)[0] != 0xFEEDFACF:
        return "no Mach-O 64 magic"
    cpu, _sub, filetype = struct.unpack_from("<iII", d, 4)
    if cpu & 0xFFFFFFFF != MACHO_CPU[arch]:
        return "cputype 0x%x, want 0x%x (%s)" % (cpu & 0xFFFFFFFF, MACHO_CPU[arch], arch)
    if filetype != 2:
        return "filetype %d, want MH_EXECUTE (2)" % filetype
    return None


def check(path, arch, os_name):
    try:
        with open(path, "rb") as f:
            d = f.read()
    except OSError as e:
        return "cannot read: %s" % e
    if os_name == "wasi":
        return None if d[:4] == b"\0asm" else "not a WebAssembly module"
    if os_name == "windows":
        return check_pe(d, arch)
    if os_name in ("linux", "freebsd"):
        return check_elf(d, arch, os_name)
    if os_name == "macos":
        return check_macho(d, arch)
    return "unknown target os"


def loader_line(path):
    """'<file>\t<interpreter or ->\t<runtime imports, comma-separated or ->'"""
    try:
        with open(path, "rb") as f:
            d = f.read()
    except OSError:
        return "%s\t-\t-" % path
    interp, rt = None, []
    if d[:4] == b"\x7fELF" and len(d) >= 64 and d[4] == 2 and d[5] == 1:
        try:
            _notes, interp = elf_notes_and_interp(d)
        except (struct.error, ValueError):
            interp = None
        rt = runtime_imports(d)
    return "%s\t%s\t%s" % (path, interp or "-", ",".join(rt) or "-")


def main(argv):
    quiet = "--quiet" in argv
    loader = "--loader" in argv
    args = [a for a in argv[1:] if a not in ("--quiet", "--loader")]
    if "-" in args:
        args = [a for a in args if a != "-"]
        args += [line.strip() for line in sys.stdin if line.strip()]
    if loader:
        for p in args:
            print(loader_line(p))
        return 0
    if len(args) < 2:
        sys.stderr.write("usage: bin-target-check.py <target> <file> [<file> ...] [-] [--quiet]\n"
                         "       bin-target-check.py --loader <file> ... [-]\n")
        return 2
    arch, os_name = arch_os(args[0])
    known = {"windows": PE_MACHINE, "linux": ELF_MACHINE, "freebsd": ELF_MACHINE,
             "macos": MACHO_CPU, "wasi": {"wasm32": 0}}
    if os_name not in known or arch not in known[os_name]:
        sys.stderr.write("bin-target-check: unknown target %s\n" % args[0])
        return 2
    bad = 0
    for p in args[1:]:
        why = check(p, arch, os_name)
        if why:
            bad += 1
            print("FAIL: %s: %s" % (p, why))
        elif not quiet:
            print("OK: %s" % p)
    print("bin-target-check %s: %d files, %d bad" % (args[0], len(args) - 1, bad))
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
