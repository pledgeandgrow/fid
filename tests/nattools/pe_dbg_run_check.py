#!/usr/bin/env python3
"""Release N9 / llvm-roadmap DB4 for pe_ir: what a debugger sees in a debug
build of the default Windows backend, checked by running it under a
debugger (Windows only).

    fid emit tests/bootstrap/test_pe_dbg_vars.fd --debug -o t.exe
    python tests/nattools/pe_dbg_run_check.py t.exe tests/bootstrap/test_pe_dbg_vars.fd [--strict]

No gdb or lldb is needed (neither ships with Windows): this script is the
debugger. It starts the program with the Windows debugging API
(CreateProcess with DEBUG_ONLY_THIS_PROCESS, WaitForDebugEvent, int3
breakpoints written with WriteProcessMemory, single steps with the trap
flag, registers from GetThreadContext) and reads the image's DWARF the way
gdb does: breakpoints from .debug_line, the frame's CFA from .debug_frame,
variables from .debug_info (location and type), as
tests/nattools/dbg_run_check.py does for Linux builds under qemu (whose
DWARF readers and test-file syntax it reuses):

    acc = acc + step; // @scale_add     a marker: break on this line
    // dbg scale_add#1 base=6 acc=42     at its 1st hit, these values
    // dbg-step scale_add scale_ret      from the 1st stop at @scale_add,
                                         a step over the line ("next")
                                         stops on @scale_ret's line
    // dbg-break scale base=6 factor=7   a breakpoint on the function (as
                                         `break scale`: after its prologue,
                                         the address of its second line row;
                                         the first must be at its first
                                         byte, on its `fn` line) stops with
                                         these values
    // dbg-inlined scale                 BG243: when scale has no code of
                                         its own (-g inlined it), its
                                         calls are described: an abstract
                                         DW_TAG_subprogram (DW_AT_inline,
                                         its `fn` line) and a
                                         DW_TAG_inlined_subroutine per call
                                         (DW_AT_call_line a line calling
                                         it) holding its markers' rows

It also checks, without running: every function of the test file has a
DW_TAG_subprogram whose DW_AT_decl_line is its `fn` line and whose
DW_AT_frame_base is DW_OP_call_frame_cfa, and .debug_frame has an FDE for
it. --strict (a debug build): every expected variable must be available.
Exit 0 when every check holds; 2 when not on Windows (nothing checked)."""
import os
import re
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import dbg_vars_check as dv  # noqa: E402
import dbg_run_check as dr  # noqa: E402

IMAGE_BASE = 0x140000000


# --- the Windows debugging API (ctypes) --------------------------------------

class WinTarget:
    """One process under the Windows debugger API, stopped between calls.
    `send("m<addr>,<n>")` answers as a GDB stub does, for
    dbg_run_check.Frame."""

    DEBUG_ONLY_THIS_PROCESS = 0x2
    DBG_CONTINUE = 0x00010002
    DBG_EXCEPTION_NOT_HANDLED = 0x80010001
    BREAKPOINT = 0x80000003
    SINGLE_STEP = 0x80000004
    CONTEXT_FLAGS = 0x100003  # CONTEXT_AMD64 | CONTROL | INTEGER
    # CONTEXT (x64) offsets
    OFF_FLAGS = 0x30
    OFF_EFLAGS = 0x44
    REGS = {"rax": 0x78, "rcx": 0x80, "rdx": 0x88, "rbx": 0x90, "rsp": 0x98,
            "rbp": 0xA0, "rsi": 0xA8, "rdi": 0xB0, "r8": 0xB8, "r9": 0xC0,
            "r10": 0xC8, "r11": 0xD0, "r12": 0xD8, "r13": 0xE0, "r14": 0xE8,
            "r15": 0xF0, "rip": 0xF8}
    # the order of a GDB `g` reply (dbg_run_check.reg_layout)
    G_ORDER = ["rax", "rbx", "rcx", "rdx", "rsi", "rdi", "rbp", "rsp", "r8",
               "r9", "r10", "r11", "r12", "r13", "r14", "r15", "rip"]

    def __init__(self, exe):
        import ctypes
        import ctypes.wintypes as wt
        self.ct = ctypes
        k = ctypes.WinDLL("kernel32", use_last_error=True)
        self.k = k
        H = wt.HANDLE

        class STARTUPINFOW(ctypes.Structure):
            _fields_ = [("cb", wt.DWORD), ("lpReserved", wt.LPWSTR), ("lpDesktop", wt.LPWSTR),
                        ("lpTitle", wt.LPWSTR), ("dwX", wt.DWORD), ("dwY", wt.DWORD),
                        ("dwXSize", wt.DWORD), ("dwYSize", wt.DWORD), ("dwXCountChars", wt.DWORD),
                        ("dwYCountChars", wt.DWORD), ("dwFillAttribute", wt.DWORD),
                        ("dwFlags", wt.DWORD), ("wShowWindow", wt.WORD), ("cbReserved2", wt.WORD),
                        ("lpReserved2", ctypes.c_void_p), ("hStdInput", H), ("hStdOutput", H),
                        ("hStdError", H)]

        class PROCESS_INFORMATION(ctypes.Structure):
            _fields_ = [("hProcess", H), ("hThread", H), ("dwProcessId", wt.DWORD),
                        ("dwThreadId", wt.DWORD)]

        k.CreateProcessW.argtypes = [wt.LPCWSTR, wt.LPWSTR, ctypes.c_void_p, ctypes.c_void_p, wt.BOOL,
                                     wt.DWORD, ctypes.c_void_p, wt.LPCWSTR,
                                     ctypes.POINTER(STARTUPINFOW), ctypes.POINTER(PROCESS_INFORMATION)]
        k.CreateProcessW.restype = wt.BOOL
        k.WaitForDebugEvent.argtypes = [ctypes.c_void_p, wt.DWORD]
        k.WaitForDebugEvent.restype = wt.BOOL
        k.ContinueDebugEvent.argtypes = [wt.DWORD, wt.DWORD, wt.DWORD]
        k.ContinueDebugEvent.restype = wt.BOOL
        k.ReadProcessMemory.argtypes = [H, ctypes.c_void_p, ctypes.c_void_p, ctypes.c_size_t,
                                        ctypes.POINTER(ctypes.c_size_t)]
        k.ReadProcessMemory.restype = wt.BOOL
        k.WriteProcessMemory.argtypes = [H, ctypes.c_void_p, ctypes.c_void_p, ctypes.c_size_t,
                                         ctypes.POINTER(ctypes.c_size_t)]
        k.WriteProcessMemory.restype = wt.BOOL
        k.FlushInstructionCache.argtypes = [H, ctypes.c_void_p, ctypes.c_size_t]
        k.GetThreadContext.argtypes = [H, ctypes.c_void_p]
        k.GetThreadContext.restype = wt.BOOL
        k.SetThreadContext.argtypes = [H, ctypes.c_void_p]
        k.SetThreadContext.restype = wt.BOOL
        k.TerminateProcess.argtypes = [H, wt.UINT]
        k.CloseHandle.argtypes = [H]
        k.CreateFileW.argtypes = [wt.LPCWSTR, wt.DWORD, wt.DWORD, ctypes.c_void_p, wt.DWORD,
                                  wt.DWORD, H]
        k.CreateFileW.restype = H

        # the program's output goes to NUL (an inheritable handle)
        class SA(ctypes.Structure):
            _fields_ = [("nLength", wt.DWORD), ("lpSecurityDescriptor", ctypes.c_void_p),
                        ("bInheritHandle", wt.BOOL)]
        sa = SA(ctypes.sizeof(SA), None, True)
        nul = k.CreateFileW("NUL", 0x40000000 | 0x80000000, 3, ctypes.byref(sa), 3, 0, None)
        si = STARTUPINFOW()
        si.cb = ctypes.sizeof(STARTUPINFOW)
        si.dwFlags = 0x100  # STARTF_USESTDHANDLES
        si.hStdInput = nul
        si.hStdOutput = nul
        si.hStdError = nul
        pi = PROCESS_INFORMATION()
        cmd = ctypes.create_unicode_buffer('"%s"' % os.path.abspath(exe))
        if not k.CreateProcessW(os.path.abspath(exe), cmd, None, None, True,
                                self.DEBUG_ONLY_THIS_PROCESS, None, None,
                                ctypes.byref(si), ctypes.byref(pi)):
            raise SystemExit("FAIL: CreateProcess: error %d" % ctypes.get_last_error())
        self.nul = nul
        self.hproc = pi.hProcess
        self.pid = pi.dwProcessId
        self.threads = {pi.dwThreadId: pi.hThread}
        self.cur_tid = pi.dwThreadId
        self.ev = ctypes.create_string_buffer(512)
        self.ctx_raw = ctypes.create_string_buffer(0x4D0 + 32)
        self.ctx = (ctypes.addressof(self.ctx_raw) + 15) & ~15
        self.bps = {}
        self.pending = None
        self.exited = None
        self.base = IMAGE_BASE
        # the first event: the process created (suspended until continued)
        while True:
            code, pid, tid = self._wait()
            if code == 3:
                self.base = struct.unpack_from("<Q", self.ev.raw, 40)[0]
                self.cur_tid = tid
                self.pending = (pid, tid)
                hfile = struct.unpack_from("<Q", self.ev.raw, 16)[0]
                if hfile:
                    k.CloseHandle(hfile)
                break
            k.ContinueDebugEvent(pid, tid, self.DBG_CONTINUE)

    # -- events --

    def _wait(self):
        if not self.k.WaitForDebugEvent(self.ev, 60000):
            raise SystemExit("FAIL: no debug event in 60 s (error %d)" % self.ct.get_last_error())
        return struct.unpack_from("<III", self.ev.raw, 0)

    def resume(self, stepping=False):
        """Run until a breakpoint of ours, a single step (`stepping`) or the
        end: ("bp", address) / ("step", None) / ("exit", code) /
        ("crash", exception code)."""
        status = self.DBG_CONTINUE
        while True:
            if self.pending is not None:
                self.k.ContinueDebugEvent(self.pending[0], self.pending[1], status)
                self.pending = None
            status = self.DBG_CONTINUE
            code, pid, tid = self._wait()
            self.pending = (pid, tid)
            raw = self.ev.raw
            if code == 1:
                exc, = struct.unpack_from("<I", raw, 16)
                addr, = struct.unpack_from("<Q", raw, 32)
                first, = struct.unpack_from("<I", raw, 168)
                if exc == self.BREAKPOINT and addr in self.bps:
                    self.cur_tid = tid
                    self.set_reg("rip", addr)
                    return ("bp", addr)
                if exc == self.BREAKPOINT:
                    continue  # the loader's breakpoint
                if exc == self.SINGLE_STEP and stepping:
                    self.cur_tid = tid
                    return ("step", None)
                if exc == self.SINGLE_STEP:
                    continue
                status = self.DBG_EXCEPTION_NOT_HANDLED
                if not first:
                    return ("crash", exc)
            elif code == 2:
                self.threads[tid] = struct.unpack_from("<Q", raw, 16)[0]
            elif code == 5:
                self.exited = struct.unpack_from("<I", raw, 16)[0]
                self.k.ContinueDebugEvent(pid, tid, self.DBG_CONTINUE)
                self.pending = None
                return ("exit", self.exited)
            elif code == 6:
                hfile = struct.unpack_from("<Q", raw, 16)[0]
                if hfile:
                    self.k.CloseHandle(hfile)

    def step(self):
        self.ct.memset(self.ctx, 0, 0x4D0)
        self._get()
        fl = struct.unpack_from("<I", self.ct.string_at(self.ctx + self.OFF_EFLAGS, 4))[0]
        self.ct.memmove(self.ctx + self.OFF_EFLAGS, struct.pack("<I", fl | 0x100), 4)
        self._set()
        return self.resume(stepping=True)

    # -- registers and memory --

    def _thread(self):
        return self.threads[self.cur_tid]

    def _get(self):
        self.ct.memmove(self.ctx + self.OFF_FLAGS, struct.pack("<I", self.CONTEXT_FLAGS), 4)
        if not self.k.GetThreadContext(self._thread(), self.ctx):
            raise SystemExit("FAIL: GetThreadContext: error %d" % self.ct.get_last_error())

    def _set(self):
        if not self.k.SetThreadContext(self._thread(), self.ctx):
            raise SystemExit("FAIL: SetThreadContext: error %d" % self.ct.get_last_error())

    def reg(self, name):
        self._get()
        return struct.unpack_from("<Q", self.ct.string_at(self.ctx + self.REGS[name], 8))[0]

    def set_reg(self, name, v):
        self._get()
        self.ct.memmove(self.ctx + self.REGS[name], struct.pack("<Q", v), 8)
        self._set()

    def regs(self):
        self._get()
        blob = self.ct.string_at(self.ctx, 0x100)
        return [struct.unpack_from("<Q", blob, self.REGS[r])[0] for r in self.G_ORDER]

    def read(self, addr, n):
        buf = self.ct.create_string_buffer(n)
        got = self.ct.c_size_t(0)
        if not self.k.ReadProcessMemory(self.hproc, addr, buf, n, self.ct.byref(got)) or got.value != n:
            raise ValueError("cannot read %d bytes at 0x%x" % (n, addr))
        return buf.raw

    def write(self, addr, data):
        got = self.ct.c_size_t(0)
        if not self.k.WriteProcessMemory(self.hproc, addr, data, len(data), self.ct.byref(got)):
            raise SystemExit("FAIL: WriteProcessMemory at 0x%x: error %d" % (addr, self.ct.get_last_error()))
        self.k.FlushInstructionCache(self.hproc, addr, len(data))

    def set_bp(self, addr):
        if addr not in self.bps:
            self.bps[addr] = self.read(addr, 1)
            self.write(addr, b"\xcc")

    def clear_bp(self, addr):
        if addr in self.bps:
            self.write(addr, self.bps.pop(addr))

    def send(self, pkt):
        """The GDB packet dbg_run_check.Frame sends: m<addr>,<n>."""
        m = re.match(r"m([0-9a-f]+),([0-9a-f]+)$", pkt)
        if not m:
            return "E01"
        try:
            return self.read(int(m.group(1), 16), int(m.group(2), 16)).hex()
        except ValueError:
            return "E01"

    def kill(self):
        if self.exited is None:
            self.k.TerminateProcess(self.hproc, 1)
            if self.pending is not None:
                self.k.ContinueDebugEvent(self.pending[0], self.pending[1], self.DBG_CONTINUE)
                self.pending = None
            try:
                for _ in range(64):
                    code, pid, tid = self._wait()
                    self.k.ContinueDebugEvent(pid, tid, self.DBG_CONTINUE)
                    if code == 5:
                        break
            except SystemExit:
                pass
        self.k.CloseHandle(self.nul)


# --- the test file -------------------------------------------------------------

def parse_extra(src):
    steps = []
    breaks = []
    for ln in open(src, encoding="utf-8").read().split("\n"):
        m = re.match(r"\s*//\s*dbg-step\s+(\w+)\s+(\w+)\s*$", ln)
        if m:
            steps.append((m.group(1), m.group(2)))
        m = re.match(r"\s*//\s*dbg-break\s+(\w+)\s*(.*)$", ln)
        if m:
            vals = []
            for tok in re.findall(r'!?[\w.]+=(?:"[^"]*"|\S+)', m.group(2)):
                k, v = tok.lstrip("!").split("=", 1)
                vals.append((k, v, True))
            breaks.append((m.group(1), vals))
    return steps, breaks


def fn_decl_lines(src):
    out = {}
    for i, ln in enumerate(open(src, encoding="utf-8").read().split("\n")):
        m = re.match(r"\s*(?:pub\s+)?fn\s+(\w+)\s*\(", ln)
        if m:
            out.setdefault(m.group(1), i + 1)
    return out


def fn_of_line(decls, line):
    """The function of the test file whose body holds `line`: the last one
    declared at or above it."""
    best = None
    for fname, dline in decls.items():
        if dline <= line and (best is None or dline > decls[best]):
            best = fname
    return best


def inlined_calls(dbg):
    """BG243: name -> (its abstract DW_TAG_subprogram (DW_AT_inline, no
    code), [(low, high, call_line, die) of each DW_TAG_inlined_subroutine
    whose DW_AT_abstract_origin it is])."""
    abstract = {}
    for d in dbg.dies:
        if d[1] == dv.DW_TAG_subprogram and 0x20 in d[2] and 0x11 not in d[2]:
            abstract[d[0]] = d
    out = {}
    for d in abstract.values():
        out.setdefault(d[2].get(0x03), (d, []))
    for d in dbg.dies:
        if d[1] == 0x1D and d[2].get(0x31) in abstract:
            ab = abstract[d[2][0x31]]
            lo = d[2].get(0x11, 0)
            out[ab[2].get(0x03)][1].append((lo, lo + d[2].get(0x12, 0), d[2].get(0x59, 0), d))
    return out


# --- reading values at a stop -------------------------------------------------

def check_values(dbg, tgt, delta, pc_dwarf, fname, where0, vals, strict, fails, notes, tally):
    regs = tgt.regs()
    frame = dr.Frame(dbg, tgt, regs, pc_dwarf)
    sub = dbg.sub_at(pc_dwarf)
    scopes = dbg.scopes_at(sub, pc_dwarf) if sub else []
    for key, want, must in vals:
        tally[0] += 1
        parts = key.split(".")
        cands = []
        for sc in scopes:
            cands = [k for k in dbg.kids(sc) if k[1] in (dv.DW_TAG_formal_parameter, dv.DW_TAG_variable) and k[2].get(0x03) == parts[0]]
            if cands:
                break
        where = "%s %s" % (where0, key)
        if not cands:
            # without --strict (-g), a variable of a function inlined here
            # is not described: optimized out
            (fails if strict or must else notes).append("%s: no such variable in .debug_info" % where)
            continue
        got = None
        err = None
        for c in cands:
            try:
                got = dr.read_var(dbg, frame, c, parts[1:])
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
        tally[1] += 1
        if want.startswith('"'):
            try:
                got = '"' + dr.read_str(frame, got) + '"'
            except ValueError as e:
                fails.append("%s: %s" % (where, e))
                continue
        tally[2] += 1
        if got != want:
            fails.append("%s = %s, want %s" % (where, got, want))


def row_line(dbg, a):
    for ra, f, l, end in dbg.rows:
        if ra == a and not end:
            return l
    return None


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    strict = "--strict" in sys.argv
    if len(args) < 2:
        print(__doc__)
        return 2
    exe, src = args[0], args[1]
    dbg = dr.Debug(exe)
    dbg.em = dr.EM_X86_64
    markers, expects, fns, types = dr.parse_test(src)
    steps, breaks = parse_extra(src)
    fails = []
    notes = []
    # --- without running: subprograms, declaration lines, frames
    decls = fn_decl_lines(src)
    for fname, dline in decls.items():
        sub = dbg.sub_named(fname)
        if sub is None:
            # -g keeps the mid-end, which may inline a small function
            # everywhere: it has no code of its own then
            (fails if strict else notes).append("%s: no DW_TAG_subprogram" % fname)
            continue
        lo, hi, d = sub
        if d[2].get(0x40) != b"\x9c":
            fails.append("%s: DW_AT_frame_base %r, not DW_OP_call_frame_cfa" % (fname, d[2].get(0x40)))
        if d[2].get(0x3B) != dline:
            fails.append("%s: DW_AT_decl_line %r, want %d" % (fname, d[2].get(0x3B), dline))
        if not any(s <= lo < e for s, e, _, _, _, _ in dbg.fdes):
            fails.append("%s: no FDE in .debug_frame" % fname)
        if dr.cfa_rule(dbg.fdes, lo + 4) != (6, 16):
            fails.append("%s: CFA after the prologue is %r, not rbp + 16" % (fname, dr.cfa_rule(dbg.fdes, lo + 4)))
        if row_line(dbg, lo) != dline:
            fails.append("%s: no line row at its first byte on its fn line %d (%r)" % (fname, dline, row_line(dbg, lo)))
    # BG243: a function -g inlined everywhere (no code of its own) is
    # described where it was inlined: an abstract DW_TAG_subprogram with
    # DW_AT_inline and its `fn` line, and a DW_TAG_inlined_subroutine at
    # each call that kept code, whose DW_AT_call_line is a line calling it;
    # each marker on one of its lines has a line row inside such a range
    # (the rows of a body folded away are one-byte nops: pe_ir.fd
    # peir_inl_setup). FID_PE_OFF=dbginl: none of this, as before BG243.
    inl = inlined_calls(dbg)
    src_lines = open(src, encoding="utf-8").read().split("\n")
    # `// dbg-inlined f`: f must be described so when it has no code of
    # its own (iropt.fd's inliner records its calls; iropt_me.fd's inline2
    # does not yet, so a function only inline2 inlines is a note)
    want_inl = set()
    for ln in src_lines:
        m = re.match(r"\s*//\s*dbg-inlined\s+(\w+)\s*$", ln)
        if m:
            want_inl.add(m.group(1))
    for fname, dline in decls.items():
        if dbg.sub_named(fname) is not None:
            continue
        ab, calls = inl.get(fname, (None, []))
        if ab is None:
            (fails if fname in want_inl else notes).append("%s: no code of its own and no abstract DW_TAG_subprogram (DW_AT_inline) for it (BG243)" % fname)
            continue
        if ab[2].get(0x3B) != dline:
            fails.append("%s: the abstract entry's DW_AT_decl_line %r, want %d" % (fname, ab[2].get(0x3B), dline))
        if not calls:
            fails.append("%s: no DW_TAG_inlined_subroutine refers to its abstract entry (BG243)" % fname)
        for lo_i, hi_i, cline, _d in calls:
            if not (1 <= cline <= len(src_lines)) or (fname + "(") not in src_lines[cline - 1]:
                fails.append("%s: inlined at DW_AT_call_line %r, a line that does not call it" % (fname, cline))
        for mname, mline in markers.items():
            if fn_of_line(decls, mline) != fname:
                continue
            a, _d = dbg.any_line_addr(mline)
            if a is None:
                continue  # reported below with the breakpoints
            if not any(lo_i <= a < hi_i for lo_i, hi_i, _c, _x in calls):
                fails.append("@%s (line %d of %s): its row at 0x%x is in none of %s's inlined ranges" % (mname, mline, fname, a, fname))
    if os.name != "nt":
        for f in fails:
            print("FAIL: " + f)
        print("not on Windows: the static checks only (%s)" % ("OK" if not fails else "failed"))
        return 1 if fails else 2
    # --- breakpoints (DWARF addresses; the process may load elsewhere)
    bps = {}
    for name, line in markers.items():
        a, d = dbg.any_line_addr(line)
        if a is None:
            fails.append("@%s: line %d has no row in .debug_line" % (name, line))
            continue
        bps[a] = ("@" + name, d)
    fn_bps = {}
    for fname, vals in breaks:
        sub = dbg.sub_named(fname)
        if sub is None:
            (fails if strict else notes).append("dbg-break %s: no such function" % fname)
            continue
        lo, hi, d = sub
        rows = sorted(a for a, f, l, end in dbg.rows if not end and lo < a < hi)
        if not rows:
            fails.append("dbg-break %s: no line row after its first" % fname)
            continue
        bps.setdefault(rows[0], ("fn:" + fname, d))
        fn_bps[rows[0]] = (fname, vals)
    want_hits = {}
    for name, hit, _ in expects:
        want_hits[name] = max(want_hits.get(name, 0), hit)
    step_from = {}
    for a, b in steps:
        if a not in markers or b not in markers:
            fails.append("dbg-step %s %s: no such marker" % (a, b))
        else:
            step_from[a] = markers[b]
    tgt = WinTarget(exe)
    delta = tgt.base - IMAGE_BASE
    for a in bps:
        tgt.set_bp(a + delta)
    hits = {}
    fn_done = set()
    steps_done = set()
    tally = [0, 0, 0]
    stops = 0
    try:
        while stops < 10000:
            kind, at = tgt.resume()
            if kind == "exit":
                if at != 0:
                    fails.append("the program exited with %d" % at)
                break
            if kind == "crash":
                fails.append("the program raised exception 0x%x" % at)
                break
            stops += 1
            pc = at - delta
            label, _ = bps.get(pc, ("?", None))
            if pc in fn_bps and fn_bps[pc][0] not in fn_done:
                fname, vals = fn_bps[pc]
                fn_done.add(fname)
                check_values(dbg, tgt, delta, pc, fname, "break %s" % fname, vals, True, fails, notes, tally)
            if label.startswith("@"):
                name = label[1:]
                hits[name] = hits.get(name, 0) + 1
                sub = dbg.sub_at(pc)
                fname = sub[2].get(0x03) if sub else "?"
                for ename, ehit, vals in expects:
                    if ename == name and ehit == hits[name]:
                        check_values(dbg, tgt, delta, pc, fname, "%s@%s#%d" % (fname, name, ehit), vals, strict, fails, notes, tally)
                if name in step_from and name not in steps_done:
                    steps_done.add(name)
                    got = next_line(dbg, tgt, delta, pc, fails)
                    if got != step_from[name]:
                        (fails if strict else notes).append("step from @%s (line %d) stopped on line %r, want %d" % (name, markers[name], got, step_from[name]))
                    # the step left the breakpoint behind: go on from here
                    continue
            # step over the breakpoint and put it back
            tgt.clear_bp(at)
            r = tgt.step()
            if r[0] != "step":
                if r[0] == "exit":
                    break
                fails.append("single step at 0x%x: %r" % (at, r))
                break
            tgt.set_bp(at)
    finally:
        tgt.kill()
    for name, n in want_hits.items():
        if hits.get(name, 0) < n:
            fails.append("@%s hit %d times, expectations need %d" % (name, hits.get(name, 0), n))
    for fname, _ in breaks:
        if fname not in fn_done and fname in [b[0] for b in fn_bps.values()]:
            fails.append("break %s: never stopped there" % fname)
    for a, _ in steps:
        if a in step_from and a not in steps_done:
            fails.append("dbg-step from @%s: never stopped there" % a)
    for n in notes:
        print("note: " + n)
    for f in fails:
        print("FAIL: " + f)
    print("%d of %d expected values available, %d checked; %d steps, %d function breakpoints" % (tally[1], tally[0], tally[2], len(steps_done), len(fn_done)))
    if not fails:
        print("pe dbg run OK")
    return 1 if fails else 0


def next_line(dbg, tgt, delta, pc0, fails):
    """`next` from the breakpoint at `pc0` (DWARF address): single steps
    (over calls: a breakpoint at the return address) until the pc is at
    a line row of the same function on another line; that line, or None.
    The breakpoints stay out of the way meanwhile."""
    sub = dbg.sub_at(pc0)
    if sub is None:
        return None
    lo = sub[2][0x11]
    hi = lo + sub[2].get(0x12, 0)
    line0 = row_line(dbg, pc0)
    rows = {}
    for a, f, l, end in dbg.rows:
        if not end and lo <= a < hi:
            rows[a] = l
    saved = list(tgt.bps.keys())
    for a in saved:
        tgt.clear_bp(a)
    got = None
    try:
        n = 0
        while n < 200000:
            n += 1
            r = tgt.step()
            if r[0] != "step":
                fails.append("step: %r" % (r,))
                return None
            pc = tgt.reg("rip") - delta
            if not (lo <= pc < hi):
                # a call: run to its return
                ret = struct.unpack_from("<Q", tgt.read(tgt.reg("rsp"), 8))[0]
                if not (lo + delta <= ret < hi + delta):
                    return None  # left the function (returned)
                tgt.set_bp(ret)
                r = tgt.resume()
                tgt.clear_bp(ret)
                if r != ("bp", ret):
                    fails.append("step over a call: %r" % (r,))
                    return None
                pc = ret - delta
            if pc in rows and rows[pc] != line0:
                got = rows[pc]
                break
    finally:
        for a in saved:
            tgt.set_bp(a)
    return got


if __name__ == "__main__":
    sys.exit(main())
