#!/usr/bin/env python3
"""DB6 (llvm-roadmap): the callee-saved registers in .debug_frame of a
native Linux executable (`fid emit --target x86_64-linux|aarch64-linux|
riscv64-linux`; compiler/nat_ir.fd ng_cfi_saves, nat_enc.fd nat_elf_debug).

Usage: cfi_saves_check.py <elf> [--min N]

For every FDE it runs the CFA program and collects the DW_CFA_offset
rules for registers other than the frame record's (x86-64 rbp/rip,
AArch64 x29/x30, RISC-V s0/ra). It checks:
  - at least N FDEs (default 1) give a callee-saved register a rule;
  - each such register is callee-saved on the target (x86-64 rbx,
    r12-r15; AArch64 x19-x28; RISC-V s1-s11);
  - x86-64: the prologue really stores the register at CFA - 8n: after
    `push rbp; mov rbp, rsp; sub rsp, F` a `mov [rsp + 16 + F - 8n], reg`
    comes before the rule's location.
Exit 0 when all hold, 1 otherwise.
"""
import struct
import sys

EM_X64, EM_A64, EM_RV = 62, 183, 243
FRAME_REGS = {EM_X64: {6, 16}, EM_A64: {29, 30}, EM_RV: {1, 8}}
CALLEE = {
    EM_X64: {3, 12, 13, 14, 15},
    EM_A64: set(range(19, 29)),
    EM_RV: {9} | set(range(18, 28)),
}
# DWARF number -> x86-64 hardware number
X64_HW = {0: 0, 1: 2, 2: 1, 3: 3, 4: 6, 5: 7, 6: 5, 7: 4}


def uleb(b, p):
    r = s = 0
    while True:
        x = b[p]
        p += 1
        r |= (x & 0x7F) << s
        s += 7
        if not x & 0x80:
            return r, p


def sections(data):
    shoff = struct.unpack_from("<Q", data, 0x28)[0]
    shentsize, shnum, shstrndx = struct.unpack_from("<HHH", data, 0x3A)
    hdrs = []
    for i in range(shnum):
        o = shoff + i * shentsize
        name, typ, flags, addr, off, size = struct.unpack_from("<IIQQQQ", data, o)
        hdrs.append((name, addr, off, size))
    stro = hdrs[shstrndx][2]
    out = {}
    for (name, addr, off, size) in hdrs:
        end = data.index(b"\0", stro + name)
        out[data[stro + name:end].decode()] = (addr, off, size)
    return out


def x64_frame(code):
    """F of `push rbp; mov rbp, rsp; sub rsp, F` at code[0], and where the
    body starts; None when the prologue is another shape."""
    if code[:4] != b"\x55\x48\x89\xE5":
        return None
    if code[4:7] == b"\x48\x83\xEC":
        return code[7], 8
    if code[4:7] == b"\x48\x81\xEC":
        return struct.unpack_from("<I", code, 7)[0], 11
    return None


def x64_stores(code, reg_hw, disp):
    """Is there a `mov [rsp + disp], reg` (REX.W 89 /r, SIB 24) in code?"""
    rex = 0x48 | (0x04 if reg_hw >= 8 else 0)
    r = reg_hw & 7
    pats = []
    if -128 <= disp <= 127:
        pats.append(bytes([rex, 0x89, 0x44 | (r << 3), 0x24, disp & 0xFF]))
    pats.append(bytes([rex, 0x89, 0x84 | (r << 3), 0x24]) + struct.pack("<i", disp))
    if disp == 0:
        pats.append(bytes([rex, 0x89, 0x04 | (r << 3), 0x24]))
    return any(p in code for p in pats)


def main():
    args = sys.argv[1:]
    if not args:
        print(__doc__)
        return 2
    want = 1
    if "--min" in args:
        want = int(args[args.index("--min") + 1])
    data = open(args[0], "rb").read()
    machine = struct.unpack_from("<H", data, 18)[0]
    secs = sections(data)
    if ".debug_frame" not in secs:
        print("FAIL no .debug_frame")
        return 1
    taddr, toff, tsize = secs[".text"]
    _, foff, fsize = secs[".debug_frame"]
    frame = data[foff:foff + fsize]
    caf = 1 if machine == EM_X64 else 4
    fails = []
    with_saves = 0
    p = 0
    while p < len(frame):
        length = struct.unpack_from("<I", frame, p)[0]
        if length == 0:
            break
        cie_id = struct.unpack_from("<I", frame, p + 4)[0]
        body = frame[p + 8:p + 4 + length]
        p += 4 + length
        if cie_id == 0xFFFFFFFF:
            continue
        start, rng = struct.unpack_from("<QQ", body, 0)
        prog = body[16:]
        loc = start
        rules = {}
        stack = []
        first_at = {}
        q = 0
        while q < len(prog):
            op = prog[q]
            q += 1
            hi, lo = op & 0xC0, op & 0x3F
            if hi == 0x40:
                loc += lo * caf
            elif hi == 0x80:
                n, q = uleb(prog, q)
                if lo not in FRAME_REGS[machine]:
                    rules[lo] = n
                    first_at.setdefault(lo, (loc, n))
            elif hi == 0xC0:
                rules.pop(lo, None)
            elif op in (2, 3, 4):
                k = {2: 1, 3: 2, 4: 4}[op]
                loc += int.from_bytes(prog[q:q + k], "little") * caf
                q += k
            elif op == 0x0C:
                _, q = uleb(prog, q)
                _, q = uleb(prog, q)
            elif op in (0x0D, 0x0E, 0x08):
                _, q = uleb(prog, q)
            elif op == 0x0A:
                stack.append(dict(rules))
            elif op == 0x0B:
                rules = stack.pop() if stack else {}
            elif op == 0:
                pass
            else:
                fails.append("FDE 0x%x: unexpected CFA op 0x%x" % (start, op))
                break
        if not first_at:
            continue
        first_loc = min(a for (a, _) in first_at.values())
        with_saves += 1
        tag = "FDE 0x%x" % start
        for reg, (at_loc, n) in first_at.items():
            if reg not in CALLEE[machine]:
                fails.append("%s: rule for r%d, not a callee-saved register" % (tag, reg))
            if machine == EM_X64:
                code = data[toff + (start - taddr):toff + (start - taddr) + rng]
                fr = x64_frame(code)
                if fr is None:
                    continue
                f, body_at = fr
                disp = 16 + f - 8 * n
                hw = X64_HW.get(reg, reg)
                # 2g.103: a save RA12 defers (shrink wrapping) gets its
                # rule where its block starts, and a block reached only
                # after the store may be laid out before it (hm_mem_slot:
                # `jmp L2; L1: jmp out; L2: mov [rsp+72], r15 ... jmp L1`,
                # L1's rows say r15 is saved). The prologue's rules (the
                # first location with a rule) need the store before them;
                # a later one, the store somewhere in the function (which
                # rows say what where is sw_cfi_check.py's part, against
                # the FID_NAT_OFF=shrinkwrap image).
                upto = at_loc - start
                if at_loc != first_loc:
                    upto = len(code)
                if not x64_stores(code[body_at:upto], hw, disp):
                    fails.append("%s: r%d said saved at CFA-%d, but no mov [rsp+%d] of it before 0x%x"
                                 % (tag, reg, 8 * n, disp, at_loc))
    if with_saves < want:
        fails.append("%d FDEs give callee-saved registers a rule, want at least %d" % (with_saves, want))
    for f in fails:
        print("FAIL " + f)
    if not fails:
        print("ok: %d FDEs describe the callee-saved registers their prologue stores" % with_saves)
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
