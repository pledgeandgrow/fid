#!/usr/bin/env python3
"""disasm_roundtrip.py -- `fid disasm` text back through clang's assembler (TG5).

    python3 tests/nattools/disasm_roundtrip.py <listing> aarch64|riscv64|x86_64
        [--cc "zig cc"] [--max-show 20]

Reads a `fid disasm` listing, writes every decoded instruction as one line
of assembly, assembles the file with clang's assembler (`zig cc -c`, as
scripts/enc-oracle.py does) and compares each instruction's bytes with the
bytes in the listing. A decoder that names the wrong form or reads an
operand from the wrong bits gives different bytes: this checks the
operands, where scripts/disasm-check.sh checks only that the code is
decoded (and, on x86-64, where each instruction starts).

  AArch64 / RISC-V  branch targets (absolute in the listing) become
                    `. + n`; adr/adrp/auipc are skipped (their target needs
                    a relocation in an object file).
  RISC-V            each instruction is assembled at a 4-byte boundary
                    (`.p2align 2`) under `.option rvc` when the listing
                    shows 2 bytes (an RVC instruction, listed as the 32-bit
                    one it stands for: TG9) and `.option norvc` when it
                    shows 4, so a compressed form round-trips too.
  x86-64           Intel syntax; each instruction is assembled at a
                    16-byte boundary (`.p2align 4, 0xcc` between them) so
                    that its bytes can be found; direct jumps and calls
                    are skipped (the assembler picks rel8 or rel32 by
                    itself). x86-64 has more than one encoding of some
                    instructions: a difference there is shown as ALT when
                    the assembler's bytes decode (in the same listing) to
                    the same text, which is not a decoder error.

A line the assembler rejects is reported and left out. Exit status 0 when
every instruction assembles back to its own bytes (ALT allowed).
"""
import os
import re
import subprocess
import sys
import tempfile

BRANCH_A64 = re.compile(r"^(b|bl|b\.[a-z]+|cbz|cbnz|tbz|tbnz)\b")
BRANCH_RV = re.compile(r"^(beq|bne|blt|bge|bltu|bgeu|beqz|bnez|j|jal)\b")
BRANCH_X64 = re.compile(r"^(jmp|call|j[a-z]+) 0x[0-9a-f]+$")


def parse(path):
    """(address, bytes, text) of each decoded instruction."""
    out = []
    line_re = re.compile(r"^  ([0-9a-f]{8}):  ((?:[0-9a-f]{2} )*[0-9a-f]{2}(?: \.\.)?)\s+(.*)$")
    for line in open(path, encoding="utf-8", errors="replace"):
        m = line_re.match(line.rstrip("\n"))
        if not m:
            continue
        text = m.group(3).strip()
        if text.startswith(".word") or text.startswith(".half") or text.startswith(".byte") or "padding" in text:
            continue
        hexs = m.group(2)
        if hexs.endswith(".."):
            continue  # more than 10 bytes: the listing cut them
        bs = bytes(int(x, 16) for x in hexs.split())
        out.append((int(m.group(1), 16), bs, text))
    return out


def source_line(addr, bs, text, arch):
    """The assembler's line for an instruction; None to skip it."""
    mnem = text.split()[0]
    if arch == "x86_64":
        if BRANCH_X64.match(text):
            return None
        t = text.split(";")[0].strip()
        t = re.sub(r"\b(byte|word|dword|qword) \[", r"\1 ptr [", t)
        t = re.sub(r"\b(byte|word|dword|qword) (fs|gs):\[", r"\1 ptr \2:[", t)
        if mnem == "mov" and len(bs) == 10:
            t = "movabs" + t[3:]
        return t
    if mnem in ("adr", "adrp", "auipc"):
        return None
    pat = BRANCH_A64 if arch == "aarch64" else BRANCH_RV
    if not pat.match(text):
        return text
    m = re.search(r"0x([0-9a-f]+)$", text)
    if not m:
        return text
    off = int(m.group(1), 16) - addr
    if arch == "riscv64":
        # a plain offset: clang compresses `j 8` / `beqz a0, 8` under
        # `.option rvc`, never `j . + 8` (an expression it leaves long)
        return text[: m.start()] + "%d" % off
    rel = ". + %d" % off if off >= 0 else ". - %d" % (-off)
    return text[: m.start()] + rel


def text_section(obj):
    """The bytes of .text in an ELF64 relocatable object."""
    import struct
    shoff = struct.unpack_from("<Q", obj, 0x28)[0]
    shentsize, shnum, shstrndx = struct.unpack_from("<HHH", obj, 0x3A)
    def sh(i):
        return struct.unpack_from("<IIQQQQIIQQ", obj, shoff + i * shentsize)
    stroff = sh(shstrndx)[4]
    for i in range(shnum):
        name, kind, flags, addr, off, size = sh(i)[:6]
        end = obj.index(b"\0", stroff + name)
        if obj[stroff + name:end] == b".text":
            return obj[off:off + size]
    return b""


HEADS = {
    "aarch64": ("aarch64-linux-musl", ".text\n"),
    "riscv64": ("riscv64-linux-musl", ".text\n.option norelax\n.option arch, +zba, +zbb, +zicond, +v\n"),
    "x86_64": ("x86_64-linux-musl", ".text\n.intel_syntax noprefix\n"),
}


def assemble(lines, arch, cc, sizes):
    """The bytes of each line, or (None, {line index: error}). `sizes`:
    each instruction's length in the listing (RISC-V: 2 for RVC)."""
    target, head = HEADS[arch]
    hl = head.count("\n")
    step = {"x86_64": 2, "riscv64": 3}.get(arch, 1)
    with tempfile.TemporaryDirectory() as d:
        src = os.path.join(d, "x.s")
        obj = os.path.join(d, "x.o")
        with open(src, "w") as f:
            f.write(head)
            for i, l in enumerate(lines):
                if arch == "x86_64":
                    f.write(".p2align 4, 0xcc\n")
                elif arch == "riscv64":
                    # each at a 4-byte boundary, so a size that differs
                    # from the listing's shifts nothing after it
                    f.write(".p2align 2\n")
                    f.write(".option rvc\n" if sizes[i] == 2 else ".option norvc\n")
                f.write(l + "\n")
        cmd = cc.split() + ["-c", "-target", target, src, "-o", obj]
        r = subprocess.run(cmd, capture_output=True, text=True)
        if r.returncode != 0:
            errs = {}
            for m in re.finditer(r"x\.s:(\d+):\d+: error: (.*)", r.stderr):
                n = int(m.group(1)) - hl - 1
                errs[n // step] = m.group(2)
            if not errs:
                errs[-1] = r.stderr.strip()[:400]
            return None, errs
        code = text_section(open(obj, "rb").read())
        if arch == "riscv64":
            return [code[4 * i:4 * i + sizes[i]] for i in range(len(lines))], {}
        if arch != "x86_64":
            return [code[4 * i:4 * i + 4] for i in range(len(code) // 4)], {}
        out = []
        for i in range(len(lines)):
            chunk = code[16 * i:16 * i + 16]
            out.append(chunk.rstrip(b"\xcc") if len(chunk) else b"")
        return out, {}


def main():
    args = sys.argv[1:]
    if len(args) < 2:
        print(__doc__)
        return 2
    path, arch = args[0], args[1]
    if arch not in HEADS:
        print("disasm_roundtrip: arch is aarch64, riscv64 or x86_64")
        return 2
    cc = "zig cc"
    show = 20
    if "--cc" in args:
        cc = args[args.index("--cc") + 1]
    if "--max-show" in args:
        show = int(args[args.index("--max-show") + 1])
    insns = parse(path)
    # the listing's own decodings, to tell an alternative encoding apart
    text_of = {}
    for addr, bs, text in insns:
        text_of.setdefault(bs, text)
    items = []
    skipped = 0
    for addr, bs, text in insns:
        t = source_line(addr, bs, text, arch)
        if t is None:
            skipped += 1
            continue
        items.append((addr, bs, text, t))
    # unique by (bytes, source line): a listing repeats most instructions
    seen = {}
    for it in items:
        seen.setdefault((it[1], it[3]), it)
    uniq = list(seen.values())
    rejected = []
    while True:
        got, errs = assemble([u[3] for u in uniq], arch, cc, [len(u[1]) for u in uniq])
        if got is not None:
            break
        if -1 in errs:
            print("disasm_roundtrip: the assembler failed: " + errs[-1])
            return 1
        keep = []
        for i, u in enumerate(uniq):
            if i in errs:
                rejected.append((u, errs[i]))
            else:
                keep.append(u)
        uniq = keep
    bad = []
    alt = []
    for u, g in zip(uniq, got):
        if arch == "x86_64" and g != u[1] and u[1].endswith(b"\xcc"):
            g = g + b"\xcc" * (len(u[1]) - len(g))
        if u[1] != g:
            if arch == "x86_64" and text_of.get(g) == u[2]:
                alt.append((u, g))
            else:
                bad.append((u, g))
    print("disasm_roundtrip: %s %s: %d instructions (%d forms), %d assembled back to their bytes, %d other encodings of the same text, %d differ, %d rejected, %d skipped" % (
        os.path.basename(path), arch, len(insns), len(seen), len(uniq) - len(bad) - len(alt), len(alt), len(bad), len(rejected), skipped))
    for u, g in bad[:show]:
        print("  DIFF  %08x  %-24s %-44s assembles to %s" % (u[0], u[1].hex(), u[2], g.hex()))
    for u, e in rejected[:show]:
        print("  REJECTED  %08x  %-24s %-44s %s" % (u[0], u[1].hex(), u[3], e))
    return 1 if bad or rejected else 0


if __name__ == "__main__":
    sys.exit(main())
