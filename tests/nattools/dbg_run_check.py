#!/usr/bin/env python3
"""CT6 / DB4 / CT9: what a debugger sees in a native Linux build, checked
by running it under a debugger.

    fid emit <test.fd> --target x86_64-linux [--debug] -o t
    python3 tests/nattools/dbg_run_check.py t <test.fd> [--strict]

Runs the executable under qemu-user's gdb stub (qemu-x86_64, qemu-aarch64,
qemu-riscv64 -g PORT) and talks the GDB remote protocol itself, as gdb
does, so no gdb is needed: breakpoints from .debug_line, the frame's CFA
from .debug_frame, variables from .debug_info (DW_AT_location as an
expression or a .debug_loc location list), their types from the type DIEs.

The test file says where to stop and what to expect:

    acc = acc + base;   // @loop          a marker: break on this line
    // dbg loop#3 base=6 acc=12 step=2    at its 3rd hit, these values
    // dbg ret acc=42 !base=6             `!`: must be available even in
                                          an optimized build
    // dbg-fn Point::len2                 a function a debugger finds by
                                          this name (CT9: DW_AT_name)
    // dbg-type Seg a.x b.y tag           a struct type with these members

A value is a decimal integer, true/false, or "text" (a str: the bytes at
the pointer). `name.field` reads a field of a struct variable (held in
memory, or through a pointer to one).

Without --strict (an optimized build), a variable with no location at the
stop is "optimized out", which is allowed unless marked `!`; a variable
with a location must hold the expected value (a wrong value always
fails). --strict (a debug build, `fid emit --debug`): every expected
variable must be available. Each marker's line must be in the line table
and each breakpoint must be hit as often as the expectations say.
Exit 0 when every check holds."""
import os
import random
import re
import socket
import struct
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import dbg_vars_check as dv  # noqa: E402  (ELF sections, LEB128, DIEs)

EM_X86_64 = 0x3E
EM_AARCH64 = 0xB7
EM_RISCV = 0xF3


# --- ELF and DWARF ---------------------------------------------------------

def machine(data):
    return struct.unpack_from("<H", data, 0x12)[0]


def line_rows(b):
    """Every row of every unit of .debug_line: (address, file, line, end)."""
    rows = []
    p = 0
    while p + 4 <= len(b):
        ulen = struct.unpack_from("<I", b, p)[0]
        end = p + 4 + ulen
        ver = struct.unpack_from("<H", b, p + 4)[0]
        hlen = struct.unpack_from("<I", b, p + 6)[0]
        q = p + 10
        prog = q + hlen
        min_len = b[q]
        q += 1
        if ver >= 4:
            q += 1
        q += 1  # default_is_stmt
        line_base = struct.unpack_from("<b", b, q)[0]
        q += 1
        line_range = b[q]
        q += 1
        op_base = b[q]
        q += 1
        std_len = list(b[q:q + op_base - 1])
        q = prog
        addr = 0
        fil = 1
        line = 1
        while q < end:
            op = b[q]
            q += 1
            if op >= op_base:
                adj = op - op_base
                addr += (adj // line_range) * min_len
                line += line_base + adj % line_range
                rows.append((addr, fil, line, False))
            elif op == 0:
                n, q = dv.uleb(b, q)
                sub = b[q]
                body = q + 1
                q += n
                if sub == 1:
                    rows.append((addr, fil, line, True))
                    addr = 0
                    fil = 1
                    line = 1
                elif sub == 2:
                    addr = struct.unpack_from("<Q", b, body)[0]
            elif op == 1:
                rows.append((addr, fil, line, False))
            elif op == 2:
                v, q = dv.uleb(b, q)
                addr += v * min_len
            elif op == 3:
                v, q = dv.sleb(b, q)
                line += v
            elif op == 4:
                fil, q = dv.uleb(b, q)
            elif op == 5:
                _, q = dv.uleb(b, q)
            elif op in (6, 7, 10, 11):
                pass
            elif op == 8:
                addr += ((255 - op_base) // line_range) * min_len
            elif op == 9:
                addr += struct.unpack_from("<H", b, q)[0]
                q += 2
            else:
                for _ in range(std_len[op - 1]):
                    _, q = dv.uleb(b, q)
        p = end
    return rows


def cfi_entries(b):
    """The FDEs of .debug_frame: (start, end, CIE initial instructions,
    FDE instructions, code alignment, data alignment)."""
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
            ca, q = dv.uleb(b, q)
            da, q = dv.sleb(b, q)
            if ver == 1:
                q += 1
            else:
                _, q = dv.uleb(b, q)
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


def cfa_rule(fdes, pc):
    """(register, offset) of the CFA at pc, or None."""
    for start, stop, init, ins, ca, da in fdes:
        if not (start <= pc < stop):
            continue
        rule = [None, 0]
        state = []
        loc = start

        def run(code, limit):
            nonlocal loc
            q = 0
            while q < len(code):
                op = code[q]
                q += 1
                hi = op & 0xC0
                lo = op & 0x3F
                if hi == 0x40:
                    loc += lo * ca
                    if loc > limit:
                        return
                elif hi == 0x80:
                    _, q = dv.uleb(code, q)
                elif hi == 0xC0:
                    pass
                elif op == 0x00:
                    pass
                elif op in (0x02, 0x03, 0x04):
                    w = {0x02: 1, 0x03: 2, 0x04: 4}[op]
                    d = int.from_bytes(code[q:q + w], "little")
                    q += w
                    loc += d * ca
                    if loc > limit:
                        return
                elif op == 0x0C:
                    r, q = dv.uleb(code, q)
                    o, q = dv.uleb(code, q)
                    rule[0] = r
                    rule[1] = o
                elif op == 0x0D:
                    r, q = dv.uleb(code, q)
                    rule[0] = r
                elif op == 0x0E:
                    o, q = dv.uleb(code, q)
                    rule[1] = o
                elif op == 0x0A:
                    state.append(list(rule))
                elif op == 0x0B:
                    if state:
                        s = state.pop()
                        rule[0], rule[1] = s[0], s[1]
                elif op in (0x05, 0x10, 0x11, 0x14, 0x16):
                    _, q = dv.uleb(code, q)
                    if op in (0x10, 0x16):
                        n, q = dv.uleb(code, q)
                        q += n
                    else:
                        _, q = dv.uleb(code, q)
                elif op in (0x06, 0x07, 0x08):
                    _, q = dv.uleb(code, q)
                elif op == 0x09:
                    _, q = dv.uleb(code, q)
                    _, q = dv.uleb(code, q)
                else:
                    raise ValueError("CFA op 0x%x not handled" % op)
        run(init, pc)
        loc = start
        run(ins, pc)
        if rule[0] is None:
            return None
        return (rule[0], rule[1])
    return None


def loc_list(b, off, base):
    """A DWARF 4 location list: [(begin, end, expression)]."""
    out = []
    p = off
    while p + 16 <= len(b):
        s, e = struct.unpack_from("<QQ", b, p)
        p += 16
        if s == 0 and e == 0:
            break
        if s == 0xFFFFFFFFFFFFFFFF:
            base = e
            continue
        n = struct.unpack_from("<H", b, p)[0]
        p += 2
        out.append((base + s, base + e, bytes(b[p:p + n])))
        p += n
    return out


class Debug:
    def __init__(self, exe):
        self.data = open(exe, "rb").read()
        self.em = machine(self.data)
        info = dv.section(self.data, ".debug_info")
        ab = dv.section(self.data, ".debug_abbrev")
        if info is None or ab is None:
            raise SystemExit("FAIL: no .debug_info / .debug_abbrev")
        self.dies = dv.dies(info, dv.abbrevs(ab))
        self.by_off = {d[0]: d for d in self.dies}
        self.rows = line_rows(dv.section(self.data, ".debug_line") or b"")
        self.fdes = cfi_entries(dv.section(self.data, ".debug_frame") or b"")
        self.loc = dv.section(self.data, ".debug_loc") or b""
        cu = self.dies[0][2]
        self.cu_base = cu.get(0x11, 0)
        self.subs = []
        for d in self.dies:
            if d[1] == dv.DW_TAG_subprogram and 0x11 in d[2]:
                lo = d[2][0x11]
                self.subs.append((lo, lo + d[2].get(0x12, 0), d))

    def sub_at(self, pc):
        for lo, hi, d in self.subs:
            if lo <= pc < hi:
                return d
        return None

    def sub_named(self, name):
        for lo, hi, d in self.subs:
            if d[2].get(0x03) == name:
                return (lo, hi, d)
        return None

    def line_addr(self, line, lo, hi):
        """The lowest address of a row of `line` inside [lo, hi)."""
        best = None
        for a, f, l, end in self.rows:
            if not end and l == line and lo <= a < hi:
                if best is None or a < best:
                    best = a
        return best

    def any_line_addr(self, line):
        for lo, hi, d in self.subs:
            a = self.line_addr(line, lo, hi)
            if a is not None:
                return a, d
        return None, None

    def kids(self, d):
        return [k for k in self.dies if k[4] == d[0]]

    def scopes_at(self, sub, pc):
        """DB4: the scopes a debugger searches for a name at `pc`,
        innermost first: the DW_TAG_lexical_block entries holding pc,
        nested, then the function itself."""
        chain = [sub]
        cur = sub
        while True:
            nxt = None
            for k in self.kids(cur):
                if k[1] == 0x0B and 0x11 in k[2]:
                    lo = k[2][0x11]
                    if lo <= pc < lo + k[2].get(0x12, 0):
                        nxt = k
                        break
            if nxt is None:
                break
            chain.append(nxt)
            cur = nxt
        chain.reverse()
        return chain

    def expr_at(self, var, pc):
        loc = var[2].get(0x02)
        if loc is None:
            return None
        if isinstance(loc, bytes):
            return loc
        for s, e, ex in loc_list(self.loc, loc, self.cu_base):
            if s <= pc < e:
                return ex
        return None


# --- the GDB remote protocol (qemu-user's stub) ------------------------------

class Remote:
    def __init__(self, port):
        self.s = None
        for _ in range(100):
            try:
                self.s = socket.create_connection(("127.0.0.1", port), timeout=30)
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


def reg_layout(em):
    """DWARF register number -> index in the `g` packet (8 bytes each), and
    the pc's index."""
    if em == EM_X86_64:
        # g: rax rbx rcx rdx rsi rdi rbp rsp r8..r15 rip
        # DWARF: rax rdx rcx rbx rsi rdi rbp rsp r8..r15
        m = {0: 0, 1: 3, 2: 2, 3: 1, 4: 4, 5: 5, 6: 6, 7: 7}
        for r in range(8, 16):
            m[r] = r
        return m, 16
    if em == EM_AARCH64:
        return {r: r for r in range(32)}, 32
    return {r: r for r in range(32)}, 32


def qemu_for(em):
    return {EM_X86_64: "qemu-x86_64", EM_AARCH64: "qemu-aarch64", EM_RISCV: "qemu-riscv64"}[em]


def bp_kind(em):
    return 1 if em == EM_X86_64 else 4


# --- the test file's expectations ------------------------------------------

vtypes = []  # CT9: (function, variable, type name as written), dbg-vtype


def parse_test(src):
    lines = open(src, encoding="utf-8").read().split("\n")
    markers = {}
    expects = []
    fns = []
    types = []
    for i, ln in enumerate(lines):
        m = re.search(r"//\s*@(\w+)\s*$", ln)
        if m and not ln.lstrip().startswith("//"):
            markers[m.group(1)] = i + 1
        m = re.match(r"\s*//\s*dbg\s+(\w+)(?:#(\d+))?\s+(.*)$", ln)
        if m:
            vals = []
            for tok in re.findall(r'!?[\w.]+=(?:"[^"]*"|\S+)', m.group(3)):
                must = tok.startswith("!")
                k, v = tok.lstrip("!").split("=", 1)
                vals.append((k, v, must))
            expects.append((m.group(1), int(m.group(2) or 1), vals))
        m = re.match(r"\s*//\s*dbg-fn\s+(\S+)", ln)
        if m:
            fns.append(m.group(1))
        m = re.match(r"\s*//\s*dbg-type\s+(\w+)\s*(.*)$", ln)
        if m:
            types.append((m.group(1), m.group(2).split()))
        m = re.match(r"\s*//\s*dbg-vtype\s+(\S+)\s+(\w+)\s+(\S+)\s*$", ln)
        if m:
            vtypes.append((m.group(1), m.group(2), m.group(3)))
    return markers, expects, fns, types


# --- reading a variable -----------------------------------------------------

class Frame:
    def __init__(self, dbg, rem, regs, pc):
        self.dbg = dbg
        self.rem = rem
        self.regs = regs
        self.pc = pc
        self.map, _ = reg_layout(dbg.em)
        rule = cfa_rule(dbg.fdes, pc)
        self.cfa = None
        if rule is not None:
            self.cfa = self.reg(rule[0]) + rule[1]

    def reg(self, r):
        return self.regs[self.map[r]]

    def mem(self, addr, n):
        h = self.rem.send("m%x,%x" % (addr, n))
        if h.startswith("E") or len(h) != 2 * n:
            raise ValueError("cannot read %d bytes at 0x%x" % (n, addr))
        return bytes.fromhex(h)

    def place(self, expr):
        """('reg', n) / ('mem', addr) / ('val', v) of a location expression."""
        op = expr[0]
        if 0x50 <= op <= 0x6F and len(expr) == 1:
            return ("reg", op - 0x50)
        if op == 0x90:
            r, _ = dv.uleb(expr, 1)
            return ("reg", r)
        if op == 0x91:
            off, q = dv.sleb(expr, 1)
            if self.cfa is None:
                raise ValueError("no CFA at 0x%x" % self.pc)
            if q < len(expr) and expr[q] == 0x06:
                # CT9: DW_OP_deref, the struct at the address the slot holds
                a = int.from_bytes(self.mem(self.cfa + off, 8), "little")
                return ("mem", a)
            return ("mem", self.cfa + off)
        if 0x70 <= op <= 0x8F:
            off, _ = dv.sleb(expr, 1)
            return ("mem", (self.reg(op - 0x70) + off) & 0xFFFFFFFFFFFFFFFF)
        if op == 0x92:
            r, q = dv.uleb(expr, 1)
            off, _ = dv.sleb(expr, q)
            return ("mem", (self.reg(r) + off) & 0xFFFFFFFFFFFFFFFF)
        if op == 0x11 and expr[-1] == 0x9F:
            v, _ = dv.sleb(expr, 1)
            return ("val", v & 0xFFFFFFFFFFFFFFFF)
        if op == 0x11:
            v, _ = dv.sleb(expr, 1)
            return ("mem", v & 0xFFFFFFFFFFFFFFFF)
        raise ValueError("location %s not handled" % expr.hex())

    def word(self, place, size):
        k, v = place
        if k == "reg":
            x = self.reg(v)
        elif k == "val":
            x = v
        else:
            x = int.from_bytes(self.mem(v, size), "little")
        return x & ((1 << (8 * size)) - 1)


def type_die(dbg, d):
    """The type entry `d` refers to, through typedefs (CT9: str)."""
    t = dbg.by_off.get(d[2].get(0x49))
    n = 0
    while t is not None and t[1] == 0x16 and n < 8:
        t = dbg.by_off.get(t[2].get(0x49))
        n += 1
    return t


def shown(dbg, frame, tdie, place):
    """The value a debugger prints for a scalar at `place`, as text."""
    a = tdie[2]
    size = a.get(0x0B, 8)
    enc = a.get(0x3E, 8)
    if tdie[1] == 0x0F:
        size = 8
    x = frame.word(place, size)
    if enc == 2:
        return "true" if x & 0xFF else "false"
    if enc == 5 and x >> (8 * size - 1):
        x -= 1 << (8 * size)
    return str(x)


def read_var(dbg, frame, var, path):
    """The text of variable `var` (field path `path`), or None when it is
    optimized out at the stop."""
    expr = dbg.expr_at(var, frame.pc)
    if expr is None or len(expr) == 0:
        return None
    place = frame.place(expr)
    t = type_die(dbg, var)
    for fld in path:
        if t is not None and t[1] == 0x0F:
            # a pointer to the struct: follow it
            ptr = frame.word(place, 8)
            place = ("mem", ptr)
            t = type_die(dbg, t)
        if t is None or t[1] != 0x13 or place[0] != "mem":
            raise ValueError("%s: not a struct in memory" % fld)
        mem = [k for k in dbg.kids(t) if k[1] == 0x0D and k[2].get(0x03) == fld]
        if not mem:
            raise ValueError("no member %s" % fld)
        place = ("mem", place[1] + mem[0][2].get(0x38, 0))
        t = type_die(dbg, mem[0])
    if t is None:
        raise ValueError("no type")
    return shown(dbg, frame, t, place)


def read_str(frame, var_text):
    ptr = int(var_text) & 0xFFFFFFFFFFFFFFFF
    out = b""
    while len(out) < 256:
        c = frame.mem(ptr + len(out), 16)
        if b"\0" in c:
            out += c[:c.index(b"\0")]
            break
        out += c
    return out.decode("latin-1")


# --- the run ----------------------------------------------------------------

def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    strict = "--strict" in sys.argv
    if len(args) < 2:
        print(__doc__)
        return 2
    exe, src = args[0], args[1]
    dbg = Debug(exe)
    markers, expects, fns, types = parse_test(src)
    fails = []
    notes = []
    # CT9: functions and types by their Fides names
    for fn in fns:
        if dbg.sub_named(fn) is None:
            fails.append("no function named %s in .debug_info" % fn)
    for tname, members in types:
        sts = [d for d in dbg.dies if d[1] == 0x13 and d[2].get(0x03) == tname]
        if not sts:
            fails.append("no struct type %s" % tname)
            continue
        for mpath in members:
            t = sts[0]
            for fld in mpath.split("."):
                mem = [k for k in dbg.kids(t) if k[1] == 0x0D and k[2].get(0x03) == fld] if t else []
                if not mem:
                    fails.append("struct %s: no member %s" % (tname, mpath))
                    t = None
                    break
                t = type_die(dbg, mem[0])
    # CT9: a variable's type by the name it is written with (a typedef's
    # name counts: str)
    for fname, vname, tname in vtypes:
        sub = dbg.sub_named(fname)
        if sub is None:
            fails.append("dbg-vtype: no function %s" % fname)
            continue
        cands = [k for k in dbg.kids(sub[2]) if k[1] in (dv.DW_TAG_formal_parameter, dv.DW_TAG_variable) and k[2].get(0x03) == vname]
        if not cands:
            fails.append("dbg-vtype: %s has no variable %s" % (fname, vname))
            continue
        t = dbg.by_off.get(cands[0][2].get(0x49))
        names = []
        while t is not None and len(names) < 8:
            names.append(t[2].get(0x03))
            if t[1] != 0x16:
                break
            t = dbg.by_off.get(t[2].get(0x49))
        if tname not in names:
            fails.append("%s.%s: type %r, want %s" % (fname, vname, names, tname))
    # breakpoints
    bps = {}
    for name, line in markers.items():
        a, d = dbg.any_line_addr(line)
        if a is None:
            fails.append("@%s: line %d has no row in .debug_line" % (name, line))
            continue
        bps[a] = (name, d)
    want_hits = {}
    for name, hit, _ in expects:
        want_hits[name] = max(want_hits.get(name, 0), hit)
    if not bps:
        for f in fails:
            print("FAIL: " + f)
        print("FAIL: no breakpoints" if expects else ("dbg run OK" if not fails else ""))
        return 1 if (fails or expects) else 0
    port = random.randint(20000, 60000)
    qemu = subprocess.Popen([qemu_for(dbg.em), "-g", str(port), os.path.abspath(exe)], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    rem = Remote(port)
    rem.send("?")
    kind = bp_kind(dbg.em)
    for a in bps:
        r = rem.send("Z0,%x,%d" % (a, kind))
        if r != "OK":
            fails.append("breakpoint at 0x%x refused: %r" % (a, r))
    _, pc_ix = reg_layout(dbg.em)
    hits = {}
    checked = 0
    avail = 0
    total = 0
    stops = 0
    while stops < 10000:
        r = rem.send("c")
        if r.startswith("W") or r.startswith("X"):
            break
        stops += 1
        regs_hex = rem.send("g")
        nregs = pc_ix + 1
        regs = [struct.unpack_from("<Q", bytes.fromhex(regs_hex[16 * i:16 * i + 16]))[0] for i in range(nregs)]
        pc = regs[pc_ix]
        if pc not in bps:
            fails.append("stopped at 0x%x, not a breakpoint (%s)" % (pc, r))
            break
        name, sub = bps[pc]
        hits[name] = hits.get(name, 0) + 1
        frame = Frame(dbg, rem, regs, pc)
        for ename, ehit, vals in expects:
            if ename != name or ehit != hits[name]:
                continue
            fsub = dbg.sub_at(pc)
            fname = fsub[2].get(0x03) if fsub else "?"
            # DB4: the innermost scope that binds a name hides the others
            scopes = dbg.scopes_at(fsub, pc) if fsub else []
            for key, want, must in vals:
                total += 1
                parts = key.split(".")
                cands = []
                for sc in scopes:
                    cands = [k for k in dbg.kids(sc) if k[1] in (dv.DW_TAG_formal_parameter, dv.DW_TAG_variable) and k[2].get(0x03) == parts[0]]
                    if cands:
                        break
                where = "%s@%s#%d %s" % (fname, name, ehit, key)
                if not cands:
                    fails.append("%s: no such variable in .debug_info" % where)
                    continue
                got = None
                err = None
                for c in cands:
                    try:
                        got = read_var(dbg, frame, c, parts[1:])
                    except ValueError as e:
                        err = str(e)
                        got = None
                    if got is not None:
                        break
                if got is None:
                    if err:
                        fails.append("%s: %s" % (where, err))
                    elif strict or must:
                        fails.append("%s: optimized out (want %s)" % (where, want))
                    else:
                        notes.append("%s: optimized out" % where)
                    continue
                avail += 1
                if want.startswith('"'):
                    try:
                        got = '"' + read_str(frame, got) + '"'
                    except ValueError as e:
                        fails.append("%s: %s" % (where, e))
                        continue
                checked += 1
                if got != want:
                    fails.append("%s = %s, want %s" % (where, got, want))
        # step over the breakpoint and put it back
        rem.send("z0,%x,%d" % (pc, kind))
        rem.send("s")
        rem.send("Z0,%x,%d" % (pc, kind))
    try:
        rem.s.close()
    except OSError:
        pass
    try:
        qemu.wait(timeout=30)
    except subprocess.TimeoutExpired:
        qemu.kill()
    for name, n in want_hits.items():
        if hits.get(name, 0) < n:
            fails.append("@%s hit %d times, expectations need %d" % (name, hits.get(name, 0), n))
    for n in notes:
        print("note: " + n)
    for f in fails:
        print("FAIL: " + f)
    print("%d of %d expected values available, %d checked" % (avail, total, checked))
    if not fails:
        print("dbg run OK")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
