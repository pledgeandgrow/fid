#!/usr/bin/env python3
"""DB6 (docs/roadmap/llvm-roadmap.md): the CFI of callee-saved registers
whose save RA12 shrink wrapping defers (compiler/nat_ir.fd ng_sw_plan,
ng_dbg_sw_states, ng_dbg_sw_to; nat_enc.fd nat_dwarf_build).

Usage: sw_cfi_check.py <on.elf> <off.elf> <on-stats.log> [--expect-missing]

<on.elf> is `fid emit --target <linux> --stats` of a program (its
`[shrinkwrap] <fn>:` lines in <on-stats.log>), <off.elf> the same with
FID_NAT_OFF=shrinkwrap, where every prologue saves all of its function's
callee-saved registers and .debug_frame gives each its rule (DW_CFA_offset
reg, n). Shrink wrapping moves saves, not slots, so for each function the
stats name, the on image's FDE must give the same (register, n) pairs
somewhere (a deferred save's rule comes where the store is made) and,
for every register it restores early (DW_CFA_restore outside an
epilogue's remember/restore_state), that register must have had a rule.
The rules at the end of the prologue must be fewer than the off image's
(something was deferred) for at least one function.
--expect-missing: the opposite (FID_NAT_OFF=swcfi): some function's
deferred registers have no rule. Exit 0 when the check holds, 1
otherwise.
"""
import re
import struct
import sys

FRAME_REGS = {62: {6, 16}, 183: {29, 30}, 243: {1, 8}}


def sections(d):
    shoff = struct.unpack_from("<Q", d, 0x28)[0]
    shnum, shstrndx = struct.unpack_from("<HH", d, 0x3C)
    hdrs = []
    for i in range(shnum):
        o = shoff + 64 * i
        name, typ, flags, addr, off, size = struct.unpack_from("<IIQQQQ", d, o)
        hdrs.append((name, addr, off, size))
    stro = hdrs[shstrndx][2]
    out = {}
    for (name, addr, off, size) in hdrs:
        end = d.index(b"\0", stro + name)
        out[d[stro + name:end].decode()] = (addr, off, size)
    return out


def funcs(d, secs):
    _, off, size = secs[".symtab"]
    stro = secs[".strtab"][1]
    out = {}
    for o in range(off, off + size, 24):
        name, info, other, shndx, value, sz = struct.unpack_from("<IBBHQQ", d, o)
        if info & 15 == 2 and sz > 0:
            end = d.index(b"\0", stro + name)
            out[d[stro + name:end].decode()] = value
    return out


def uleb(b, p):
    r = s = 0
    while True:
        x = b[p]
        p += 1
        r |= (x & 0x7F) << s
        s += 7
        if not x & 0x80:
            return r, p


def fde_rules(d, secs, machine):
    """{start: (all (reg, n) pairs, pairs at the first rule's location,
    registers restored outside an epilogue, registers that had a rule
    before being restored)} of .debug_frame."""
    caf = 1 if machine == 62 else 4
    _, off, size = secs[".debug_frame"]
    f = d[off:off + size]
    out = {}
    p = 0
    while p + 4 <= len(f):
        ln = struct.unpack_from("<I", f, p)[0]
        if ln == 0:
            break
        cid = struct.unpack_from("<I", f, p + 4)[0]
        body = f[p + 8:p + 4 + ln]
        p += 4 + ln
        if cid == 0xFFFFFFFF:
            continue
        start = struct.unpack_from("<Q", body, 0)[0]
        prog = body[16:]
        loc = start
        pairs = set()
        first_loc = None
        first = set()
        rules = {}
        depth = 0
        # (2g.103: the rules an epilogue's DW_CFA_remember_state keeps and
        # DW_CFA_restore_state brings back; without them a register an
        # epilogue restored looked rule-less to the code after it)
        stack = []
        early = set()
        bad_restore = set()
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
                    pairs.add((lo, n))
                    rules[lo] = n
                    if first_loc is None:
                        first_loc = loc
                    if loc == first_loc:
                        first.add((lo, n))
            elif hi == 0xC0:
                if lo not in FRAME_REGS[machine] and depth == 0:
                    early.add(lo)
                    if lo not in rules:
                        bad_restore.add(lo)
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
                depth += 1
                stack.append(dict(rules))
            elif op == 0x0B:
                depth = max(0, depth - 1)
                if stack:
                    rules = stack.pop()
        out[start] = (pairs, first, early, bad_restore)
    return out


def load(path):
    d = open(path, "rb").read()
    machine = struct.unpack_from("<H", d, 18)[0]
    secs = sections(d)
    return machine, funcs(d, secs), fde_rules(d, secs, machine)


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    expect_missing = "--expect-missing" in sys.argv
    if len(args) < 3:
        print(__doc__)
        return 2
    m_on, f_on, r_on = load(args[0])
    m_off, f_off, r_off = load(args[1])
    names = []
    for line in open(args[2], encoding="utf-8", errors="replace"):
        m = re.match(r"^\[shrinkwrap\] (\S+): ", line)
        if m:
            names.append(m.group(1))
    fails = []
    checked = deferred = missing = 0
    for nm in names:
        if nm not in f_on or nm not in f_off:
            continue
        a_on, a_off = f_on[nm], f_off[nm]
        if a_on not in r_on or a_off not in r_off:
            continue
        pairs, first, early, bad_restore = r_on[a_on]
        want = r_off[a_off][0]
        if not want:
            continue
        checked += 1
        if first != want:
            deferred += 1
        if pairs != want:
            missing += 1
            if not expect_missing:
                fails.append("%s: rules for %s, the prologue-saves-all build has %s" % (nm, sorted(pairs), sorted(want)))
        if bad_restore and not expect_missing:
            fails.append("%s: DW_CFA_restore of r%s before any rule" % (nm, sorted(bad_restore)))
    if checked == 0:
        fails.append("no function of the stats log has callee-saved rules in both images")
    if expect_missing:
        if missing == 0:
            fails.append("FID_NAT_OFF=swcfi: every deferred register still has its rule")
    elif deferred == 0:
        fails.append("no function defers a save (the prologue's rules are all the rules)")
    for f in fails:
        print("FAIL " + f)
    if not fails:
        if expect_missing:
            print("ok: without the rows, %d of %d shrink-wrapped functions miss rules" % (missing, checked))
        else:
            print("ok: %d shrink-wrapped functions (%d defer saves) describe every saved register" % (checked, deferred))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
