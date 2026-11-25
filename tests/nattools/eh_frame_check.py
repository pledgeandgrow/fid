#!/usr/bin/env python3
"""DB6 (docs/roadmap/llvm-roadmap.md): .eh_frame and .eh_frame_hdr in a
Linux executable of the machine layer (`fid emit --target
x86_64-linux|aarch64-linux|riscv64-linux`; compiler/nat_enc.fd
nat_elf_eh_frame, nat_eh_frame, nat_eh_frame_hdr), read the way an
unwinder (libgcc, libunwind, perf) finds a frame's rules.

Usage: eh_frame_check.py <elf>... [--none] [--readelf]

For each image it checks:
  - one PT_GNU_EH_FRAME, naming the section .eh_frame_hdr, inside a
    PT_LOAD whose file offset and address agree with the section's;
    .eh_frame inside a PT_LOAD the same way;
  - the header: version 1, encodings pcrel|sdata4, udata4,
    datarel|sdata4; its .eh_frame pointer is .eh_frame's address;
  - the search table: sorted by address, each entry pointing at an FDE
    of .eh_frame whose pc_begin (pcrel sdata4) is the entry's address;
    every FDE start has an entry;
  - the CIE: version 1, augmentation "zR", FDE encoding 0x1B;
  - each FDE's CIE pointer leads back to that CIE, and its rules are
    those of the .debug_frame FDE of the same function (same start,
    same length, same program but for the DW_CFA_nop padding);
  - every function symbol (.symtab, STT_FUNC, size > 0) is found by a
    binary search of the table, and the FDE found covers it;
  - .eh_frame ends with a zero terminator.
--none: instead, the image has no PT_GNU_EH_FRAME and no .eh_frame
(FID_NAT_OFF=ehframe). --readelf: also run `readelf --debug-dump=
frames-interp` (or `wsl.exe -e readelf` on Windows) and require its
.eh_frame tables to equal its .debug_frame tables, with no warnings.
Exit 0 when every check holds, 1 otherwise.
"""
import os
import re
import shutil
import struct
import subprocess
import sys

PT_LOAD, PT_GNU_EH_FRAME = 1, 0x6474E550


def u(fmt, d, o):
    return struct.unpack_from(fmt, d, o)[0]


def phdrs(d):
    phoff = u("<Q", d, 0x20)
    phnum = u("<H", d, 0x38)
    out = []
    for i in range(phnum):
        o = phoff + 56 * i
        out.append(struct.unpack_from("<IIQQQQQQ", d, o))
    return out


def sections(d):
    shoff = u("<Q", d, 0x28)
    shnum = u("<H", d, 0x3C)
    shstrndx = u("<H", d, 0x3E)
    hdrs = []
    for i in range(shnum):
        o = shoff + 64 * i
        name, typ, flags, addr, off, size = struct.unpack_from("<IIQQQQ", d, o)
        link = u("<I", d, o + 40)
        hdrs.append((name, typ, flags, addr, off, size, link))
    stro = hdrs[shstrndx][4] if hdrs else 0
    out = {}
    for h in hdrs:
        end = d.index(b"\0", stro + h[0])
        out[d[stro + h[0]:end].decode()] = h
    return out


def funcs(d, secs):
    if ".symtab" not in secs or ".strtab" not in secs:
        return []
    _, _, _, _, off, size, _ = secs[".symtab"]
    stro = secs[".strtab"][4]
    out = []
    for o in range(off, off + size, 24):
        name, info, other, shndx, value, sz = struct.unpack_from("<IBBHQQ", d, o)
        if info & 15 == 2 and sz > 0:
            end = d.index(b"\0", stro + name)
            out.append((d[stro + name:end].decode(), value, sz))
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


def strip_nops(b):
    e = len(b)
    while e > 0 and b[e - 1] == 0:
        e -= 1
    return bytes(b[:e])


def debug_fdes(d, secs):
    """{start: (length, program)} of .debug_frame."""
    out = {}
    if ".debug_frame" not in secs:
        return out
    off, size = secs[".debug_frame"][4], secs[".debug_frame"][5]
    f = d[off:off + size]
    p = 0
    while p + 4 <= len(f):
        ln = u("<I", f, p)
        if ln == 0:
            break
        cid = u("<I", f, p + 4)
        if cid != 0xFFFFFFFF:
            start, rng = struct.unpack_from("<QQ", f, p + 8)
            out[start] = (rng, strip_nops(f[p + 24:p + 4 + ln]))
        p += 4 + ln
    return out


def in_load(loads, addr, off, size):
    for (typ, flags, poff, vaddr, paddr, filesz, memsz, align) in loads:
        if vaddr <= addr and addr + size <= vaddr + filesz:
            return off - poff == addr - vaddr
    return False


def check(path, want_none, run_readelf):
    d = open(path, "rb").read()
    fails = []
    ph = phdrs(d)
    secs = sections(d)
    loads = [p for p in ph if p[0] == PT_LOAD]
    ehp = [p for p in ph if p[0] == PT_GNU_EH_FRAME]
    if want_none:
        if ehp or ".eh_frame" in secs or ".eh_frame_hdr" in secs:
            fails.append("FID_NAT_OFF=ehframe still wrote .eh_frame / PT_GNU_EH_FRAME")
        return fails, 0
    if len(ehp) != 1:
        return ["%d PT_GNU_EH_FRAME program headers, want 1" % len(ehp)], 0
    for need in (".eh_frame_hdr", ".eh_frame", ".debug_frame"):
        if need not in secs:
            return ["no %s section" % need], 0
    _, _, hflags, haddr, hoff, hsize, _ = secs[".eh_frame_hdr"]
    _, _, eflags, eaddr, eoff, esize, _ = secs[".eh_frame"]
    p = ehp[0]
    if p[2] != hoff or p[3] != haddr or p[5] != hsize:
        fails.append("PT_GNU_EH_FRAME (offset %#x, address %#x, size %#x) is not .eh_frame_hdr (%#x, %#x, %#x)" % (p[2], p[3], p[5], hoff, haddr, hsize))
    if not (hflags & 2) or not (eflags & 2):
        fails.append(".eh_frame_hdr / .eh_frame not SHF_ALLOC")
    if not in_load(loads, haddr, hoff, hsize):
        fails.append(".eh_frame_hdr is not inside a PT_LOAD at its file offset")
    if not in_load(loads, eaddr, eoff, esize):
        fails.append(".eh_frame is not inside a PT_LOAD at its file offset")
    h = d[hoff:hoff + hsize]
    e = d[eoff:eoff + esize]
    if len(h) < 12 or h[0] != 1 or h[1] != 0x1B or h[2] != 0x03 or h[3] != 0x3B:
        return fails + ["header version/encodings %s, want 01 1b 03 3b" % h[:4].hex()], 0
    if haddr + 4 + u("<i", h, 4) != eaddr:
        fails.append("the header's .eh_frame pointer is %#x, .eh_frame is at %#x" % (haddr + 4 + u("<i", h, 4), eaddr))
    n = u("<I", h, 8)
    if 12 + 8 * n > len(h):
        return fails + ["the search table (%d entries) runs past the header" % n], 0
    table = [(haddr + u("<i", h, 12 + 8 * i), haddr + u("<i", h, 16 + 8 * i)) for i in range(n)]
    for a, b in zip(table, table[1:]):
        if a[0] >= b[0]:
            fails.append("search table not sorted: %#x then %#x" % (a[0], b[0]))
            break
    # .eh_frame: the CIE and the FDEs
    if len(e) < 8 or u("<I", e, 4) != 0:
        return fails + [".eh_frame does not start with a CIE"], n
    clen = u("<I", e, 0)
    if e[8] != 1:
        fails.append("CIE version %d, want 1" % e[8])
    aug_end = e.index(b"\0", 9)
    aug = e[9:aug_end].decode()
    if aug != "zR":
        fails.append("CIE augmentation %r, want 'zR'" % aug)
    q = aug_end + 1
    _, q = uleb(e, q)
    _, q = uleb(e, q)
    _, q = uleb(e, q)
    alen, q = uleb(e, q)
    if alen != 1 or e[q] != 0x1B:
        fails.append("CIE augmentation data %s, want 1b" % e[q:q + alen].hex())
    fdes = {}
    pos = 4 + clen
    terminated = False
    while pos + 4 <= len(e):
        ln = u("<I", e, pos)
        if ln == 0:
            terminated = True
            break
        cptr = u("<I", e, pos + 4)
        if pos + 4 - cptr != 0:
            fails.append("FDE at %#x: CIE pointer %#x does not lead to the CIE" % (pos, cptr))
        start = eaddr + pos + 8 + u("<i", e, pos + 8)
        rng = u("<I", e, pos + 12)
        al, r = uleb(e, pos + 16)
        prog = strip_nops(e[r + al:pos + 4 + ln])
        fdes[eaddr + pos] = (start, rng, prog)
        pos += 4 + ln
    if not terminated:
        fails.append(".eh_frame has no zero terminator")
    starts = set()
    for (loc, fde) in table:
        if fde not in fdes:
            fails.append("table entry %#x points at %#x, no FDE there" % (loc, fde))
        elif fdes[fde][0] != loc:
            fails.append("table entry %#x points at an FDE for %#x" % (loc, fdes[fde][0]))
        starts.add(loc)
    for (start, rng, prog) in fdes.values():
        if start not in starts:
            fails.append("FDE for %#x has no search table entry" % start)
    dbg = debug_fdes(d, secs)
    same = 0
    for (start, rng, prog) in fdes.values():
        if start not in dbg:
            fails.append("FDE for %#x: no .debug_frame FDE for that start" % start)
        elif dbg[start][0] != rng or dbg[start][1] != prog:
            fails.append("FDE for %#x: rules differ from .debug_frame's" % start)
        else:
            same += 1
    if len(dbg) != len(fdes):
        fails.append(".debug_frame has %d FDEs, .eh_frame %d" % (len(dbg), len(fdes)))
    # what an unwinder does for a pc: the last entry at or below it
    for (name, addr, size) in funcs(d, secs):
        lo, hi = 0, len(table)
        while lo < hi:
            mid = (lo + hi) // 2
            if table[mid][0] <= addr:
                lo = mid + 1
            else:
                hi = mid
        if lo == 0:
            fails.append("%s (%#x): no table entry at or below it" % (name, addr))
            continue
        fde = table[lo - 1][1]
        if fde in fdes:
            st, rng, _ = fdes[fde]
            if not (st <= addr and addr + size <= st + rng):
                fails.append("%s (%#x+%#x): the FDE found covers %#x+%#x" % (name, addr, size, st, rng))
    if run_readelf:
        fails += readelf_check(path)
    return fails, same


def readelf_check(path):
    cmd = None
    if shutil.which("readelf"):
        cmd = ["readelf", "--debug-dump=frames-interp", path]
    elif os.name == "nt" and shutil.which("wsl.exe"):
        # The wsl.exe launcher can exist without any installed distribution;
        # probe it rather than treating its error text as readelf output.
        probe = subprocess.run(["wsl.exe", "-e", "true"], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        if probe.returncode == 0:
            p = os.path.abspath(path).replace("\\", "/")
            m = re.match(r"^([A-Za-z]):/(.*)$", p)
            if m:
                p = "/mnt/%s/%s" % (m.group(1).lower(), m.group(2))
            cmd = ["wsl.exe", "-e", "readelf", "--debug-dump=frames-interp", p]
    if cmd is None:
        print("skip readelf: not found")
        return []
    r = subprocess.run(cmd, capture_output=True, text=True)
    text = r.stdout + r.stderr
    if r.returncode != 0 or "Warning" in text or "warning" in text:
        return ["readelf: %s" % text.strip().splitlines()[-1:] if text.strip() else "readelf failed"]
    parts = text.split("Contents of the ")
    tabs = {}
    for part in parts[1:]:
        name = part.split(" section", 1)[0]
        rows = {}
        cur = None
        for line in part.splitlines():
            m = re.search(r"FDE cie=\w+ pc=([0-9a-f]+)\.\.([0-9a-f]+)", line)
            if m:
                cur = int(m.group(1), 16)
                rows[cur] = []
            elif cur is not None and re.match(r"^[0-9a-f]{16} ", line):
                rows[cur].append(line.split())
        tabs[name] = rows
    if ".eh_frame" not in tabs or ".debug_frame" not in tabs:
        return ["readelf shows no .eh_frame or no .debug_frame tables"]
    bad = [hex(s) for s in tabs[".debug_frame"] if tabs[".eh_frame"].get(s) != tabs[".debug_frame"][s]]
    if bad:
        return ["readelf: the .eh_frame tables differ from .debug_frame's for %s" % ", ".join(bad[:5])]
    print("     readelf: %d FDE tables, .eh_frame equal to .debug_frame" % len(tabs[".eh_frame"]))
    return []


def main():
    args = sys.argv[1:]
    want_none = "--none" in args
    run_readelf = "--readelf" in args
    files = [a for a in args if not a.startswith("--")]
    if not files:
        print(__doc__)
        return 2
    bad = 0
    for path in files:
        fails, same = check(path, want_none, run_readelf)
        for f in fails:
            print("FAIL %s: %s" % (path, f))
        if fails:
            bad += 1
        elif want_none:
            print("ok   %s: no .eh_frame (FID_NAT_OFF=ehframe)" % path)
        else:
            print("ok   %s: .eh_frame_hdr and .eh_frame, %d FDEs as .debug_frame's" % (path, same))
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
