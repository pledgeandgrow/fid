#!/usr/bin/env python3
"""RA12 / DB6 (docs/roadmap/llvm-roadmap.md): unwinding through functions
whose callee-saved registers shrink wrapping saves late, checked by running
the program under a debugger stub.

    fid emit tests/bootstrap/test_machine_sw_unwind.fd --target <t> -o img
    python3 tests/nattools/sw_unwind_check.py img [--fns mid,rare2]
        [--probe probe] [--expect-fail]

Runs the executable under qemu-user's gdb stub (qemu-x86_64, qemu-aarch64,
qemu-riscv64 -g PORT), talking the GDB remote protocol itself (no gdb
needed), with a breakpoint at the entry of each function of --fns and of
the probe function. At a function's entry it records the registers: these
are its caller's values of the callee-saved registers, and its CFA (the
stack pointer before the call). At each entry of the probe, it unwinds two
frames with .debug_frame's rules, as a debugger or a profiler does:
the probe's frame at its first instruction (the CIE's rules), then the
function that called it at the return address (its FDE's rules at
ra - 1, with remember/restore_state, DW_CFA_restore and DW_CFA_offset of
the deferred saves: compiler/nat_ir.fd ng_dbg_sw_to, nat_enc.fd
nat_dwarf_build). The registers and CFA found for that function's caller
must equal the ones recorded at its entry, for every callee-saved register
(x86-64 rbx rbp r12-r15; AArch64 x19-x29; RISC-V s0-s11).

It also reports, for the stops checked, how many had a callee-saved
register whose rule came from a row after the prologue (a deferred save in
effect) and how many had a register without a rule that the prologue
of an unshrinkwrapped function would have saved; with neither, nothing
deferred was exercised (a note, not a failure: the allocation may differ
by target).

--expect-fail: exit 0 only when some unwound register is wrong (the image
was built with FID_NAT_OFF=swcfi: the deferred saves have no rows, so
the check must catch it). Exit 0 when the check holds, 1 when it does not,
2 when nothing could be checked (no qemu, or no stop).
"""
import os
import random
import shutil
import socket
import struct
import subprocess
import sys
import time

EM_X86_64 = 0x3E
EM_AARCH64 = 0xB7
EM_RISCV = 0xF3

# DWARF numbers of the callee-saved registers (frame pointer included)
CALLEE_SAVED = {
    EM_X86_64: [3, 6, 12, 13, 14, 15],
    EM_AARCH64: list(range(19, 30)),
    EM_RISCV: [8, 9] + list(range(18, 28)),
}
# DWARF number of the stack pointer and of the return address column
SP_REG = {EM_X86_64: 7, EM_AARCH64: 31, EM_RISCV: 2}
RA_COL = {EM_X86_64: 16, EM_AARCH64: 30, EM_RISCV: 1}
FP_REG = {EM_X86_64: 6, EM_AARCH64: 29, EM_RISCV: 8}


def uleb(b, p):
    r = s = 0
    while True:
        x = b[p]
        p += 1
        r |= (x & 0x7F) << s
        s += 7
        if not x & 0x80:
            return r, p


def sleb(b, p):
    r = s = 0
    while True:
        x = b[p]
        p += 1
        r |= (x & 0x7F) << s
        s += 7
        if not x & 0x80:
            if x & 0x40:
                r -= 1 << s
            return r, p


# --- ELF ---------------------------------------------------------------------

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


def functions(d, secs):
    """name -> (start, end) of every function symbol with a size."""
    _, off, size = secs[".symtab"]
    stro = secs[".strtab"][1]
    out = {}
    for o in range(off, off + size, 24):
        name, info, other, shndx, value, sz = struct.unpack_from("<IBBHQQ", d, o)
        if info & 15 == 2 and sz > 0:
            end = d.index(b"\0", stro + name)
            out[d[stro + name:end].decode()] = (value, value + sz)
    return out


def find_fn(fns, name):
    if name in fns:
        return fns[name]
    for k, v in fns.items():
        if k.endswith("::" + name) or k.endswith("." + name):
            return v
    return None


# --- .debug_frame --------------------------------------------------------------

def fde_list(b):
    """[(start, end, cie initial instructions, fde instructions, code
    alignment, data alignment)] of .debug_frame."""
    cies = {}
    fdes = []
    p = 0
    while p + 4 <= len(b):
        ln = struct.unpack_from("<I", b, p)[0]
        if ln == 0:
            break
        end = p + 4 + ln
        cid = struct.unpack_from("<I", b, p + 4)[0]
        q = p + 8
        if cid == 0xFFFFFFFF:
            ver = b[q]
            q += 1
            e = b.index(b"\0", q)
            q = e + 1
            if ver >= 4:
                q += 2
            ca, q = uleb(b, q)
            da, q = sleb(b, q)
            if ver == 1:
                q += 1
            else:
                _, q = uleb(b, q)
            cies[p] = (ca, da, b[q:end])
        else:
            start, rng = struct.unpack_from("<QQ", b, q)
            q += 16
            fdes.append((start, start + rng, cid, b[q:end]))
        p = end
    out = []
    for start, stop, cid, ins in fdes:
        ca, da, init = cies.get(cid, (1, -8, b""))
        out.append((start, stop, init, ins, ca, da))
    return out


class Rules:
    """The rules in effect at one address: cfa = (reg, offset); regs[r] =
    ("off", byte offset from the CFA) | ("same",) | ("undef",) |
    ("reg", r2) | ("valoff", byte offset); late = the registers whose
    DW_CFA_offset came from a row after the FDE's first location
    advance past the prologue (deferred saves)."""

    def __init__(self):
        self.cfa = (None, 0)
        self.regs = {}
        self.row_locs = {}


def rules_at(fdes, pc):
    """(Rules, fde start, whether a remember_state is open) for pc, or
    None when no FDE covers it."""
    for start, stop, init, ins, ca, da in fdes:
        if not (start <= pc < stop):
            continue
        r = Rules()
        stack = []
        initial = {}
        state = {"loc": start, "stop": False}

        def run(code, is_init):
            q = 0
            while q < len(code) and not state["stop"]:
                op = code[q]
                q += 1
                hi = op & 0xC0
                lo = op & 0x3F

                def advance(n):
                    nl = state["loc"] + n * ca
                    if nl > pc:
                        state["stop"] = True
                    else:
                        state["loc"] = nl

                if hi == 0x40:
                    advance(lo)
                elif hi == 0x80:
                    o, q = uleb(code, q)
                    r.regs[lo] = ("off", o * da)
                    r.row_locs[lo] = state["loc"]
                elif hi == 0xC0:
                    if lo in initial:
                        r.regs[lo] = initial[lo]
                    else:
                        r.regs.pop(lo, None)
                    r.row_locs[lo] = state["loc"]
                elif op == 0x00:
                    pass
                elif op == 0x01:
                    nl = struct.unpack_from("<Q", code, q)[0]
                    q += 8
                    if nl > pc:
                        state["stop"] = True
                    else:
                        state["loc"] = nl
                elif op in (0x02, 0x03, 0x04):
                    w = {0x02: 1, 0x03: 2, 0x04: 4}[op]
                    n = int.from_bytes(code[q:q + w], "little")
                    q += w
                    advance(n)
                elif op == 0x05:
                    reg, q = uleb(code, q)
                    o, q = uleb(code, q)
                    r.regs[reg] = ("off", o * da)
                    r.row_locs[reg] = state["loc"]
                elif op == 0x06:
                    reg, q = uleb(code, q)
                    if reg in initial:
                        r.regs[reg] = initial[reg]
                    else:
                        r.regs.pop(reg, None)
                    r.row_locs[reg] = state["loc"]
                elif op == 0x07:
                    reg, q = uleb(code, q)
                    r.regs[reg] = ("undef",)
                elif op == 0x08:
                    reg, q = uleb(code, q)
                    r.regs[reg] = ("same",)
                elif op == 0x09:
                    reg, q = uleb(code, q)
                    r2, q = uleb(code, q)
                    r.regs[reg] = ("reg", r2)
                elif op == 0x0A:
                    stack.append((r.cfa, dict(r.regs)))
                elif op == 0x0B:
                    if stack:
                        r.cfa, r.regs = stack.pop()
                elif op == 0x0C:
                    reg, q = uleb(code, q)
                    o, q = uleb(code, q)
                    r.cfa = (reg, o)
                elif op == 0x0D:
                    reg, q = uleb(code, q)
                    r.cfa = (reg, r.cfa[1])
                elif op == 0x0E:
                    o, q = uleb(code, q)
                    r.cfa = (r.cfa[0], o)
                elif op == 0x11:
                    reg, q = uleb(code, q)
                    o, q = sleb(code, q)
                    r.regs[reg] = ("off", o * da)
                    r.row_locs[reg] = state["loc"]
                elif op == 0x12:
                    reg, q = uleb(code, q)
                    o, q = sleb(code, q)
                    r.cfa = (reg, o * da)
                elif op == 0x13:
                    o, q = sleb(code, q)
                    r.cfa = (r.cfa[0], o * da)
                elif op == 0x14:
                    reg, q = uleb(code, q)
                    o, q = uleb(code, q)
                    r.regs[reg] = ("valoff", o * da)
                elif op in (0x0F, 0x10, 0x16):
                    raise ValueError("DWARF expressions in the CFI (op 0x%x) are not handled" % op)
                else:
                    raise ValueError("CFA op 0x%x not handled" % op)
            if is_init:
                initial.update(r.regs)

        run(init, True)
        state["loc"] = start
        state["stop"] = False
        run(ins, False)
        return r, start, len(stack) > 0
    return None


def save_rows(fdes, fstart, saved_set):
    """For the FDE starting at fstart: the location of its first
    DW_CFA_offset of a register of saved_set (the prologue's saves, or the
    first deferred one when the prologue saves none), and every register
    of saved_set the FDE gives an offset rule anywhere."""
    for start, stop, init, ins, ca, da in fdes:
        if start != fstart:
            continue
        loc = start
        first = None
        ever = set()
        q = 0
        while q < len(ins):
            op = ins[q]
            q += 1
            hi = op & 0xC0
            lo = op & 0x3F
            if hi == 0x40:
                loc += lo * ca
            elif hi == 0x80:
                _, q = uleb(ins, q)
                if lo in saved_set:
                    ever.add(lo)
                    if first is None:
                        first = loc
            elif hi == 0xC0 or op in (0x00, 0x0A, 0x0B):
                pass
            elif op == 0x01:
                loc = struct.unpack_from("<Q", ins, q)[0]
                q += 8
            elif op in (0x02, 0x03, 0x04):
                w = {0x02: 1, 0x03: 2, 0x04: 4}[op]
                loc += int.from_bytes(ins[q:q + w], "little") * ca
                q += w
            elif op in (0x05, 0x11, 0x09, 0x0C, 0x12, 0x14):
                reg, q = uleb(ins, q)
                _, q = uleb(ins, q) if op not in (0x11, 0x12) else sleb(ins, q)
                if op in (0x05, 0x11) and reg in saved_set:
                    ever.add(reg)
                    if first is None:
                        first = loc
            elif op in (0x06, 0x07, 0x08, 0x0D, 0x0E):
                _, q = uleb(ins, q)
            elif op == 0x13:
                _, q = sleb(ins, q)
            else:
                break
        return (first if first is not None else stop), ever
    return 0, set()


# --- the remote protocol ---------------------------------------------------------

class Remote:
    def __init__(self, port):
        self.s = None
        for _ in range(200):
            try:
                self.s = socket.create_connection(("127.0.0.1", port), timeout=60)
                break
            except OSError:
                time.sleep(0.1)
        if self.s is None:
            raise SystemExit("FAIL: could not connect to the gdb stub")
        self.buf = b""

    def _recv(self):
        d = self.s.recv(65536)
        if not d:
            raise EOFError("stub closed")
        self.buf += d

    def send(self, pkt):
        cs = sum(pkt.encode()) & 0xFF
        self.s.sendall(("$%s#%02x" % (pkt, cs)).encode())
        return self.reply()

    def reply(self):
        while True:
            while b"$" not in self.buf:
                self._recv()
            i = self.buf.index(b"$")
            while b"#" not in self.buf[i:] or len(self.buf) < self.buf.index(b"#", i) + 3:
                self._recv()
            j = self.buf.index(b"#", i)
            pkt = self.buf[i + 1:j].decode("latin-1")
            self.buf = self.buf[j + 3:]
            self.s.sendall(b"+")
            return pkt

    def mem64(self, addr):
        r = self.send("m%x,8" % addr)
        if len(r) != 16 or r.startswith("E"):
            raise ValueError("cannot read memory at 0x%x (%s)" % (addr, r))
        return struct.unpack("<Q", bytes.fromhex(r))[0]


def reg_layout(em):
    """DWARF register number -> index in the `g` packet (8 bytes each), and
    the pc's index."""
    if em == EM_X86_64:
        m = {0: 0, 1: 3, 2: 2, 3: 1, 4: 4, 5: 5, 6: 6, 7: 7}
        for r in range(8, 16):
            m[r] = r
        return m, 16
    return {r: r for r in range(32)}, 32


QEMU = {EM_X86_64: "qemu-x86_64", EM_AARCH64: "qemu-aarch64", EM_RISCV: "qemu-riscv64"}


# --- unwinding -----------------------------------------------------------------

def unwind(fdes, em, rem, pc, regs, lookup_pc):
    """One frame: from the registers `regs` (DWARF number -> value) of a
    frame stopped at pc (its rules looked up at lookup_pc), the caller's
    registers and the CFA. Returns (caller regs, cfa, return address,
    rules, fde start)."""
    got = rules_at(fdes, lookup_pc)
    if got is None:
        raise ValueError("no FDE covers 0x%x" % lookup_pc)
    rules, fstart, _ = got
    creg, coff = rules.cfa
    if creg is None:
        raise ValueError("no CFA rule at 0x%x" % lookup_pc)
    cfa = (regs[creg] + coff) & 0xFFFFFFFFFFFFFFFF
    out = dict(regs)
    for reg, rule in rules.regs.items():
        if rule[0] == "off":
            out[reg] = rem.mem64((cfa + rule[1]) & 0xFFFFFFFFFFFFFFFF)
        elif rule[0] == "valoff":
            out[reg] = (cfa + rule[1]) & 0xFFFFFFFFFFFFFFFF
        elif rule[0] == "reg":
            out[reg] = regs[rule[1]]
        elif rule[0] == "undef":
            out[reg] = None
    # the caller's stack pointer is the CFA
    out[SP_REG[em]] = cfa
    ra_col = RA_COL[em]
    ra = out.get(ra_col)
    if em == EM_X86_64:
        rr = rules.regs.get(ra_col)
        if rr is None or rr[0] != "off":
            raise ValueError("no rule for the return address at 0x%x" % lookup_pc)
    return out, cfa, ra, rules, fstart


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    expect_fail = "--expect-fail" in sys.argv
    fn_names = ["mid", "rare2"]
    probe_name = "probe"
    for a in sys.argv[1:]:
        if a.startswith("--fns="):
            fn_names = [x for x in a[6:].split(",") if x]
        if a.startswith("--probe="):
            probe_name = a[8:]
    if len(args) < 1:
        print(__doc__)
        return 2
    exe = args[0]
    data = open(exe, "rb").read()
    em = struct.unpack_from("<H", data, 0x12)[0]
    if em not in QEMU:
        print("FAIL: machine %d not handled" % em)
        return 1
    secs = sections(data)
    for need in (".symtab", ".strtab", ".debug_frame"):
        if need not in secs:
            print("FAIL: no %s section" % need)
            return 1
    fns = functions(data, secs)
    _, foff, fsize = secs[".debug_frame"]
    fdes = fde_list(data[foff:foff + fsize])
    probe = find_fn(fns, probe_name)
    if probe is None:
        print("FAIL: no function %s" % probe_name)
        return 1
    targets = {}
    for n in fn_names:
        r = find_fn(fns, n)
        if r is None:
            print("FAIL: no function %s" % n)
            return 1
        targets[n] = r
    qemu = QEMU[em]
    if shutil.which(qemu) is None:
        print("skip: %s not found" % qemu)
        return 2
    # a dynamic executable needs its loader (QEMU_LD_PREFIX, as
    # scripts/cross-test.sh)
    phoff = struct.unpack_from("<Q", data, 0x20)[0]
    phnum = struct.unpack_from("<H", data, 0x38)[0]
    for i in range(phnum):
        ptype, _, poff, _, _, pfs = struct.unpack_from("<IIQQQQ", data, phoff + 56 * i)
        if ptype == 3:
            loader = data[poff:poff + pfs].rstrip(b"\0").decode()
            prefix = os.environ.get("QEMU_LD_PREFIX", "")
            if not os.path.exists(prefix + loader):
                print("skip: the executable is dynamic and %s%s is missing (set QEMU_LD_PREFIX)" % (prefix, loader))
                return 2
    layout, pc_ix = reg_layout(em)
    port = random.randint(20000, 60000)
    proc = subprocess.Popen([qemu, "-g", str(port), os.path.abspath(exe)], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    rem = Remote(port)
    rem.send("?")
    kind = 1 if em == EM_X86_64 else 4
    bps = {probe[0]: probe_name}
    for n, (s, e) in targets.items():
        bps[s] = n
    for a in bps:
        r = rem.send("Z0,%x,%d" % (a, kind))
        if r != "OK":
            print("FAIL: breakpoint at 0x%x refused: %r" % (a, r))
            return 1
    entry = {}
    fails = []
    checked = 0
    late_rule_stops = 0
    no_rule_stops = 0
    per_fn = {}
    stops = 0
    while stops < 20000:
        r = rem.send("c")
        if r.startswith("W") or r.startswith("X"):
            break
        stops += 1
        regs_hex = rem.send("g")
        vals = [struct.unpack_from("<Q", bytes.fromhex(regs_hex[16 * i:16 * i + 16]))[0] for i in range(pc_ix + 1)]
        pc = vals[pc_ix]
        regs = {d: vals[g] for d, g in layout.items()}
        if pc not in bps:
            fails.append("stopped at 0x%x, not a breakpoint (%s)" % (pc, r))
            break
        name = bps[pc]
        try:
            if name != probe_name:
                # the caller's values, and the CFA (the probe's rules at
                # the entry: the CIE's)
                got = rules_at(fdes, pc)
                if got is None:
                    raise ValueError("no FDE covers %s" % name)
                cr, co = got[0].cfa
                entry[name] = ({k: regs[k] for k in CALLEE_SAVED[em]}, (regs[cr] + co) & 0xFFFFFFFFFFFFFFFF)
            else:
                # frame 0: the probe at its first instruction
                f1, cfa0, ra, _, _ = unwind(fdes, em, rem, pc, regs, pc)
                caller = None
                for n, (s, e) in targets.items():
                    if s <= ra < e:
                        caller = n
                if caller is not None and caller in entry:
                    # frame 1: the function that called the probe, at the
                    # call (ra - 1)
                    f2, cfa1, ra2, rules, fstart = unwind(fdes, em, rem, ra, f1, ra - 1)
                    want, want_cfa = entry[caller]
                    checked += 1
                    per_fn[caller] = per_fn.get(caller, 0) + 1
                    if cfa1 != want_cfa:
                        fails.append("%s, call at +0x%x: CFA 0x%x, its entry had 0x%x" % (caller, ra - fstart, cfa1, want_cfa))
                    for reg in CALLEE_SAVED[em]:
                        if f2.get(reg) != want[reg]:
                            fails.append("%s, call at +0x%x: register %d unwinds to %s, the caller had 0x%x" % (caller, ra - fstart, reg, "?" if f2.get(reg) is None else "0x%x" % f2[reg], want[reg]))
                    # a deferred save in effect (its rule from a row
                    # after the function's first save row), or a register
                    # the function saves somewhere left without a rule here
                    # (before its deferred save, or after its early restore)
                    # (not the frame pointer: the frame record's rule,
                    # nat_fde_body, comes before every save)
                    pool = [k for k in CALLEE_SAVED[em] if k != FP_REG[em]]
                    first_save, ever = save_rows(fdes, fstart, pool)
                    late = [k for k in pool if k in rules.regs and rules.regs[k][0] == "off" and rules.row_locs.get(k, fstart) > first_save]
                    if late:
                        late_rule_stops += 1
                    if [k for k in ever if k not in rules.regs or rules.regs[k][0] != "off"]:
                        no_rule_stops += 1
        except (ValueError, EOFError) as e:
            fails.append("at 0x%x (%s): %s" % (pc, name, e))
            break
        rem.send("z0,%x,%d" % (pc, kind))
        rem.send("s")
        rem.send("Z0,%x,%d" % (pc, kind))
        if len(fails) > 20:
            break
    try:
        rem.s.close()
    except OSError:
        pass
    try:
        proc.wait(timeout=60)
    except subprocess.TimeoutExpired:
        proc.kill()
    summary = "%d probe stops unwound (%s), %d with a save made after the prologue in effect, %d with a saved register not (yet or any more) saved there" % (
        checked, ", ".join("%s %d" % kv for kv in sorted(per_fn.items())), late_rule_stops, no_rule_stops)
    if checked == 0:
        print("nothing checked: no stop in %s from %s" % (probe_name, ", ".join(fn_names)))
        for f in fails[:10]:
            print("FAIL: " + f)
        return 2 if not fails else 1
    if expect_fail:
        if fails:
            print("caught as expected: %s (%s)" % (fails[0], summary))
            return 0
        print("FAIL: every register unwound correctly although the deferred saves should have no rows (%s)" % summary)
        return 1
    for f in fails[:20]:
        print("FAIL: " + f)
    if late_rule_stops == 0:
        print("note: no stop had a save made after the prologue in effect")
    print(summary)
    if not fails:
        print("sw unwind OK")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
