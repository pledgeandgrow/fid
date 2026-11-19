#!/usr/bin/env python3
"""dbg-check.py [--fid FID] [--keep DIR] [FILE.fd]: the debug information of
the machine layer's executables (docs/roadmap/llvm-roadmap.md DB2, DB6).

Builds FILE (default tests/bootstrap/test_machine_dbg.fd) with `fid emit`
for x86_64-linux, aarch64-linux and riscv64-linux, and checks the
.debug_frame CFI (compiler/nat_enc.fd nat_elf_debug) by running each FDE's
program, as a debugger or a profiler's unwinder does:
  - every function of .symtab has an FDE covering exactly its range;
  - a function that starts with the frame prologue (nat_has_frame): at its
    first byte the CFA is the CIE's (rsp + 8 / sp + 0), after the prologue
    it is the frame pointer's (rbp + 16, x29 + 16, s0 + 0), and at every
    `ret` of its epilogues (ng_cfi_row) the CFA is sp-based again with the
    frame pointer restored; after a ret that more code follows the body's
    rule is back;
  - a function without a frame record (a WP8 leaf) has the CIE's rules
    throughout (an empty FDE program).
Then, for x86_64-macos and aarch64-macos, the Mach-O symbol table (nat_macho):
LC_SYMTAB holds a local N_SECT symbol per function (`_main` among them) at
an address inside __text, LC_DYSYMTAB counts them all as local, and
scripts/macho-check.py passes. And for `--target x86_64-windows --nat` the
COFF symbol table after the sections (nat_pe_symbols, DB10): a function
symbol in .text per function, `main` among them.
And the variables (DB2/DB4): tests/bootstrap/test_machine_dbg_vars.fd is
built for each ELF target and tests/nattools/dbg_vars_check.py checks its
.debug_info (frame base, decl lines, parameters and variables, locations),
and tests/nattools/dbg_decl_lines_check.py checks each function's decl
line is its `fn` line (DB2, nat_enc.fd nat_dbg_decl_line).

No program is run: any host with `fid` and Python 3. Exit 0 when every
check passes.
"""
import os
import struct
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)

ELF_TARGETS = ["x86_64-linux", "aarch64-linux", "riscv64-linux"]
MACHO_TARGETS = ["x86_64-macos", "aarch64-macos"]

# Per machine: the prologue's first bytes and its length, the CFA (DWARF
# register, offset) at entry and in the body, the frame pointer, an
# epilogue's bytes, where its ret is in them and how long it is, the CFA at
# the ret
X64, A64, RV = "x86_64", "aarch64", "riscv64"


def le32(words):
    return b"".join(struct.pack("<I", w) for w in words)


ARCH = {
    X64: dict(prologue=bytes([0x55, 0x48, 0x89, 0xE5]), plen=4,
              entry=(7, 8), body=(6, 16), fp=6,
              # mov rsp, rbp; pop rbp; ret
              ret=bytes([0x48, 0x89, 0xEC, 0x5D, 0xC3]), ret_at=4, ret_len=1,
              at_ret=(7, 8)),
    A64: dict(prologue=le32([0xA9BF7BFD]), plen=8,
              entry=(31, 0), body=(29, 16), fp=29,
              # mov sp, x29; ldp x29, x30, [sp], #16; ret
              ret=le32([0x910003BF, 0xA8C17BFD, 0xD65F03C0]), ret_at=8, ret_len=4,
              at_ret=(31, 0)),
    RV: dict(prologue=le32([0xFF010113]), plen=16,
             entry=(2, 0), body=(8, 0), fp=8,
             # addi sp, s0, -16; ld ra, 8(sp); ld s0, 0(sp); addi sp, sp, 16; ret
             ret=le32([0xFF040113, 0x00813083, 0x00013403, 0x01010113, 0x00008067]),
             ret_at=16, ret_len=4, at_ret=(2, 0)),
}


def target_arch(t):
    if t.startswith("x86_64"):
        return X64
    if t.startswith("aarch64"):
        return A64
    return RV


def emit(fid, src, target, out, extra=()):
    env = dict(os.environ)
    env["FIDES_NO_CACHE"] = "1"
    r = subprocess.run([fid, "emit", src, "--target", target] + list(extra) + ["-o", out],
                       capture_output=True, text=True, env=env)
    if r.returncode != 0 or not os.path.exists(out):
        raise RuntimeError("fid emit --target %s failed:\n%s%s" % (target, r.stdout, r.stderr))
    with open(out, "rb") as f:
        return f.read()


# ---------------------------------------------------------------- ELF

def elf_sections(d):
    shoff = struct.unpack_from("<Q", d, 0x28)[0]
    shnum, shstrndx = struct.unpack_from("<HH", d, 0x3C)
    secs = []
    for i in range(shnum):
        o = shoff + i * 64
        name, typ, flags, addr, off, size = struct.unpack_from("<IIQQQQ", d, o)
        secs.append(dict(name=name, type=typ, addr=addr, off=off, size=size))
    sh = secs[shstrndx]

    def nm(n):
        e = d.index(b"\0", sh["off"] + n)
        return d[sh["off"] + n:e].decode()
    return {nm(s["name"]): s for s in secs[1:]}


def elf_funcs(d, secs):
    sym, st = secs[".symtab"], secs[".strtab"]
    out = []
    for o in range(sym["off"] + 24, sym["off"] + sym["size"], 24):
        name, info, _, shndx, value, size = struct.unpack_from("<IBBHQQ", d, o)
        if info & 15 != 2:
            continue
        e = d.index(b"\0", st["off"] + name)
        out.append((d[st["off"] + name:e].decode(), value, size))
    return out


def uleb(d, p):
    v = sh = 0
    while True:
        b = d[p]
        p += 1
        v |= (b & 0x7F) << sh
        sh += 7
        if not b & 0x80:
            return v, p


def sleb(d, p):
    v = sh = 0
    while True:
        b = d[p]
        p += 1
        v |= (b & 0x7F) << sh
        sh += 7
        if not b & 0x80:
            if b & 0x40:
                v -= 1 << sh
            return v, p


class Rows:
    """The CFA rows of one FDE: [(address, (cfa reg, cfa off), rules)]."""

    def __init__(self, cie, prog, start, caf, daf):
        self.rows = []
        loc = start
        cfa, rules = cie
        rules = dict(rules)
        init = dict(rules)
        stack = []
        p = 0
        while p < len(prog):
            op = prog[p]
            p += 1
            hi, lo = op & 0xC0, op & 0x3F
            if hi == 0x40:
                self.rows.append((loc, cfa, dict(rules)))
                loc += lo * caf
                continue
            if hi == 0x80:
                off, p = uleb(prog, p)
                rules[lo] = ("off", off * daf)
                continue
            if hi == 0xC0:
                if lo in init:
                    rules[lo] = init[lo]
                else:
                    rules.pop(lo, None)
                continue
            if op == 0:
                continue
            if op in (2, 3, 4):
                n = {2: 1, 3: 2, 4: 4}[op]
                delta = int.from_bytes(prog[p:p + n], "little")
                p += n
                self.rows.append((loc, cfa, dict(rules)))
                loc += delta * caf
            elif op == 0x0C:
                r, p = uleb(prog, p)
                o, p = uleb(prog, p)
                cfa = (r, o)
            elif op == 0x0D:
                r, p = uleb(prog, p)
                cfa = (r, cfa[1])
            elif op == 0x0E:
                o, p = uleb(prog, p)
                cfa = (cfa[0], o)
            elif op == 0x08:
                r, p = uleb(prog, p)
                rules[r] = ("same",)
            elif op == 0x0A:
                stack.append((cfa, dict(rules)))
            elif op == 0x0B:
                cfa, rules = stack.pop()
                rules = dict(rules)
            else:
                raise ValueError("CFA op 0x%x not expected" % op)
        self.rows.append((loc, cfa, dict(rules)))

    def at(self, addr):
        """The row in force at addr."""
        best = self.rows[0]
        for r in self.rows:
            if r[0] <= addr:
                best = r
        return best


def parse_cie(d, p):
    caf, p = uleb(d, p)
    daf, p = sleb(d, p)
    p += 1  # return address register (version 1: one byte)
    return caf, daf, p


def check_elf(fid, src, target, keep):
    arch = target_arch(target)
    A = ARCH[arch]
    out = os.path.join(keep, "dbg-" + target)
    d = emit(fid, src, target, out)
    secs = elf_sections(d)
    fails = []
    for need in (".symtab", ".debug_frame", ".text"):
        if need not in secs:
            return ["%s: no %s" % (target, need)]
    text = secs[".text"]
    fr = secs[".debug_frame"]
    fd = d[fr["off"]:fr["off"] + fr["size"]]
    # the CIE at 0
    clen = struct.unpack_from("<I", fd, 0)[0]
    if struct.unpack_from("<I", fd, 4)[0] != 0xFFFFFFFF or fd[8] != 1:
        return ["%s: .debug_frame does not start with a version 1 CIE" % target]
    aug_end = fd.index(b"\0", 9)
    caf, daf, ip = parse_cie(fd, aug_end + 1)
    cie0 = Rows(((0, 0), {}), fd[ip:4 + clen], 0, caf, daf)
    cie = (cie0.rows[-1][1], cie0.rows[-1][2])
    if cie[0] != A["entry"]:
        fails.append("%s: CIE CFA %s, want %s" % (target, cie[0], A["entry"]))
    fdes = {}
    p = 4 + clen
    while p + 4 <= len(fd):
        ln = struct.unpack_from("<I", fd, p)[0]
        if ln == 0:
            break
        start, rng = struct.unpack_from("<QQ", fd, p + 8)
        fdes[start] = (rng, fd[p + 24:p + 4 + ln])
        p += 4 + ln

    def code(addr, n):
        o = text["off"] + (addr - text["addr"])
        return d[o:o + n]

    funcs = elf_funcs(d, secs)
    framed = leaves = epis = 0
    for name, start, size in funcs:
        if start not in fdes:
            fails.append("%s: %s has no FDE" % (target, name))
            continue
        rng, prog = fdes[start]
        if rng != size:
            fails.append("%s: %s FDE range %d, symbol size %d" % (target, name, rng, size))
        rows = Rows(cie, prog, start, caf, daf)
        body = code(start, size)
        if body.startswith(A["prologue"]):
            framed += 1
            if rows.at(start)[1] != A["entry"]:
                fails.append("%s: %s: CFA at entry %s" % (target, name, rows.at(start)[1]))
            if rows.at(start + A["plen"])[1] != A["body"]:
                fails.append("%s: %s: CFA after the prologue %s, want %s" % (target, name, rows.at(start + A["plen"])[1], A["body"]))
            k = body.find(A["ret"])
            while k >= 0:
                if arch == X64 or k % 4 == 0:
                    epis += 1
                    ra = start + k + A["ret_at"]
                    row = rows.at(ra)
                    if row[1] != A["at_ret"]:
                        fails.append("%s: %s: CFA at the ret +%d is %s, want %s" % (target, name, ra - start, row[1], A["at_ret"]))
                    if row[2].get(A["fp"]) not in (("same",), None):
                        fails.append("%s: %s: frame pointer not restored at the ret +%d" % (target, name, ra - start))
                    after = ra + A["ret_len"]
                    if after < start + size and any(code(after, 4)):
                        if rows.at(after)[1] != A["body"]:
                            fails.append("%s: %s: body rule not back after the ret +%d" % (target, name, ra - start))
                k = body.find(A["ret"], k + 1)
        else:
            leaves += 1
            if any(r[1] != cie[0] for r in rows.rows):
                fails.append("%s: %s has no frame record but its CFA moves" % (target, name))
    print("%s: %d functions, %d framed (%d epilogues checked), %d without a frame record"
          % (target, len(funcs), framed, epis, leaves))
    if not any(n == "main" for n, _, _ in funcs):
        fails.append("%s: no symbol main" % target)
    return fails


# ------------------------------------------------------------- Mach-O

def check_macho(fid, src, target, keep):
    out = os.path.join(keep, "dbg-" + target)
    d = emit(fid, src, target, out)
    fails = []
    ncmds = struct.unpack_from("<I", d, 16)[0]
    off = 32
    symtab = dysym = None
    text = None
    for _ in range(ncmds):
        cmd, size = struct.unpack_from("<II", d, off)
        if cmd == 0x19:
            nsects = struct.unpack_from("<I", d, off + 64)[0]
            for s in range(nsects):
                so = off + 72 + s * 80
                sect = d[so:so + 16].rstrip(b"\0").decode()
                addr, sz = struct.unpack_from("<QQ", d, so + 32)
                if sect == "__text":
                    text = (addr, sz)
        elif cmd == 0x2:
            symtab = struct.unpack_from("<4I", d, off + 8)
        elif cmd == 0xB:
            dysym = struct.unpack_from("<6I", d, off + 8)
        off += size
    if symtab is None or text is None:
        return ["%s: no LC_SYMTAB or __text" % target]
    so, n, sto, ss = symtab
    names = []
    for i in range(n):
        strx, typ, sect, desc, value = struct.unpack_from("<IBBHQ", d, so + i * 16)
        e = d.index(b"\0", sto + strx)
        nm = d[sto + strx:e].decode()
        names.append(nm)
        if typ != 0x0E or sect != 1:
            fails.append("%s: %s: n_type 0x%x n_sect %d" % (target, nm, typ, sect))
        if not (text[0] <= value < text[0] + text[1]):
            fails.append("%s: %s at 0x%x outside __text" % (target, nm, value))
    if "_main" not in names:
        fails.append("%s: no _main among %d symbols" % (target, n))
    if dysym is None or dysym[0] != 0 or dysym[1] != n:
        fails.append("%s: LC_DYSYMTAB does not count the %d symbols as local: %s" % (target, n, dysym))
    r = subprocess.run([sys.executable, os.path.join(HERE, "macho-check.py"), out, "--quiet"],
                       capture_output=True, text=True)
    if r.returncode != 0:
        fails.append("%s: macho-check.py: %s%s" % (target, r.stdout, r.stderr))
    print("%s: %d symbols (%s)" % (target, n, ", ".join(names[:6])))
    return fails


# --------------------------------------------------------- --nat PE

def check_pe(fid, src, keep):
    """`fid emit --target x86_64-windows --nat`: the COFF symbol table
    nat_enc.fd nat_pe_symbols appends (DB10)."""
    out = os.path.join(keep, "dbg-x86_64-windows-nat.exe")
    d = emit(fid, src, "x86_64-windows", out, ["--nat"])
    fails = []
    ptr, n = struct.unpack_from("<II", d, 76)
    if ptr == 0 or n == 0:
        return ["--nat PE: no COFF symbol table"]
    code_size = struct.unpack_from("<I", d, 0x40 + 24 + 4)[0]
    strtab = ptr + n * 18
    names = []
    for i in range(n):
        o = ptr + i * 18
        raw = d[o:o + 8]
        if raw[:4] == bytes(4):
            so = strtab + struct.unpack_from("<I", raw, 4)[0]
            nm = d[so:d.index(bytes(1), so)].decode()
        else:
            nm = raw.rstrip(bytes(1)).decode()
        value, sect, typ, cls, aux = struct.unpack_from("<IhHBB", d, o + 8)
        names.append(nm)
        if sect != 1 or typ != 0x20 or cls != 2 or aux != 0:
            fails.append("--nat PE: %s: section %d type 0x%x class %d aux %d" % (nm, sect, typ, cls, aux))
        if value >= code_size:
            fails.append("--nat PE: %s at 0x%x past .text (0x%x)" % (nm, value, code_size))
    if "main" not in names:
        fails.append("--nat PE: no symbol main among %d" % n)
    if struct.unpack_from("<I", d, strtab)[0] != len(d) - strtab:
        fails.append("--nat PE: the string table's size word is not what follows")
    print("x86_64-windows --nat: %d symbols (%s)" % (n, ", ".join(names[:6])))
    return fails


def check_dbg_vars(fid, target, keep):
    """DB2/DB4: test_machine_dbg_vars.fd's .debug_info, by
    tests/nattools/dbg_vars_check.py (skipped when either is missing)."""
    src = os.path.join(ROOT, "tests", "bootstrap", "test_machine_dbg_vars.fd")
    checker = os.path.join(ROOT, "tests", "nattools", "dbg_vars_check.py")
    if not (os.path.exists(src) and os.path.exists(checker)):
        return []
    out = os.path.join(keep, "dbg_vars-" + target)
    emit(fid, src, target, out)
    r = subprocess.run([sys.executable, checker, out, src], capture_output=True, text=True)
    if r.returncode != 0:
        return ["%s dbg_vars_check.py:\n%s%s" % (target, r.stdout, r.stderr)]
    print("%s: dbg_vars_check.py passes" % target)
    return []


def check_decl_lines(fid, target, keep):
    """DB2: each function's DW_AT_decl_line is its `fn` line
    (compiler/nat_enc.fd nat_dbg_decl_line), by
    tests/nattools/dbg_decl_lines_check.py (skipped when it is missing).
    Returns the checker's FAIL lines."""
    checker = os.path.join(ROOT, "tests", "nattools", "dbg_decl_lines_check.py")
    if not os.path.exists(checker):
        return []
    work = os.path.join(keep, "decl_lines-" + target)
    r = subprocess.run([sys.executable, checker, "--fid", fid, "--targets", target, "--keep", work],
                       cwd=ROOT, capture_output=True, text=True)
    fails = ["%s dbg_decl_lines_check.py: %s" % (target, line[5:])
             for line in r.stdout.splitlines() if line.startswith("FAIL ")]
    if r.returncode != 0 and not fails:
        fails.append("%s dbg_decl_lines_check.py exited %d:\n%s%s" % (target, r.returncode, r.stdout[-600:], r.stderr[-600:]))
    if not fails:
        print("%s: dbg_decl_lines_check.py passes" % target)
    return fails


def main():
    args = sys.argv[1:]
    fid = os.path.join(ROOT, "release", "fid.exe" if os.name == "nt" else "fid")
    keep = None
    src = os.path.join(ROOT, "tests", "bootstrap", "test_machine_dbg.fd")
    while args:
        a = args.pop(0)
        if a == "--fid":
            fid = args.pop(0)
        elif a == "--keep":
            keep = args.pop(0)
        else:
            src = a
    tmp = keep or tempfile.mkdtemp(prefix="fid-dbg-")
    fails = []
    for t in ELF_TARGETS:
        try:
            fails += check_elf(fid, src, t, tmp)
        except Exception as e:  # a failed build is a failed check
            fails.append("%s: %s" % (t, e))
        try:
            fails += check_dbg_vars(fid, t, tmp)
        except Exception as e:
            fails.append("%s dbg_vars: %s" % (t, e))
        try:
            fails += check_decl_lines(fid, t, tmp)
        except Exception as e:
            fails.append("%s decl lines: %s" % (t, e))
    for t in MACHO_TARGETS:
        try:
            fails += check_macho(fid, src, t, tmp)
        except Exception as e:
            fails.append("%s: %s" % (t, e))
    try:
        fails += check_pe(fid, src, tmp)
    except Exception as e:
        fails.append("x86_64-windows --nat: %s" % e)
    for f in fails:
        print("FAIL " + f)
    print("dbg-check: %s" % ("ok" if not fails else "%d failures" % len(fails)))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
