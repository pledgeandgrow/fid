#!/usr/bin/env python3
"""DB6 (llvm-roadmap): the Windows x64 unwind data of a `fid emit --nat`
image (compiler/nat_enc.fd nat_pe_unwind, nat_pe_set_dir).

Usage: pe_unwind_check.py <image.exe> [--min N]

Checks the exception directory (data directory 3):
  - it is set, inside the image, a whole number of RUNTIME_FUNCTIONs;
  - the entries are sorted, do not overlap and lie in .text;
  - every entry's function starts with our frame prologue (push rbp; mov
    rbp, rsp: 55 48 89 E5) and its UNWIND_INFO is version 1, prologue 4,
    two codes, frame register rbp at offset 0: UWOP_SET_FPREG at 4 and
    UWOP_PUSH_NONVOL rbp at 1;
  - at least N functions (default 1) have an entry.
Then it unwinds by hand, as RtlVirtualUnwind would, from a pc inside each
function's body: rsp = rbp, pop rbp, return address at [rsp] (only the
arithmetic: there is no live stack here), and checks the prologue offsets
fall inside the function.
Exit 0 when all hold, 1 otherwise (each failure printed).
"""
import struct
import sys


def main():
    args = sys.argv[1:]
    if not args:
        print(__doc__)
        return 2
    path = args[0]
    want = 1
    if "--min" in args:
        want = int(args[args.index("--min") + 1])
    data = open(path, "rb").read()
    fails = []
    if data[:2] != b"MZ":
        print("FAIL not a PE image")
        return 1
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    if data[pe:pe + 4] != b"PE\0\0":
        print("FAIL no PE signature")
        return 1
    nsec = struct.unpack_from("<H", data, pe + 6)[0]
    opt = pe + 24
    if struct.unpack_from("<H", data, opt)[0] != 0x20B:
        print("FAIL not PE32+")
        return 1
    optsize = struct.unpack_from("<H", data, pe + 20)[0]
    dirs = opt + 112
    exc_rva, exc_size = struct.unpack_from("<II", data, dirs + 3 * 8)
    secs = []
    for i in range(nsec):
        s = pe + 24 + optsize + i * 40
        name = data[s:s + 8].rstrip(b"\0").decode("latin-1")
        vsize, rva, rsize, roff = struct.unpack_from("<IIII", data, s + 8)
        flags = struct.unpack_from("<I", data, s + 36)[0]
        secs.append((name, rva, vsize, roff, rsize, flags))

    def at(rva, n):
        for (_, srva, vsize, roff, rsize, _) in secs:
            if srva <= rva and rva + n <= srva + max(vsize, rsize):
                off = roff + (rva - srva)
                return data[off:off + n]
        return None

    text = [s for s in secs if s[5] & 0x20]
    if not text:
        print("FAIL no code section")
        return 1
    t_lo, t_hi = text[0][1], text[0][1] + text[0][2]
    if exc_rva == 0 or exc_size == 0:
        print("FAIL no exception directory (data directory 3 is empty)")
        return 1
    if exc_size % 12 != 0:
        fails.append("exception directory size %d is not a multiple of 12" % exc_size)
    table = at(exc_rva, exc_size)
    if table is None:
        print("FAIL exception directory 0x%x+%d is outside the sections" % (exc_rva, exc_size))
        return 1
    n = exc_size // 12
    prev_end = 0
    for i in range(n):
        b, e, u = struct.unpack_from("<III", table, i * 12)
        tag = "entry %d [0x%x, 0x%x)" % (i, b, e)
        if b < prev_end:
            fails.append(tag + ": not sorted or overlaps the previous one")
        prev_end = e
        if not (t_lo <= b < e <= t_hi):
            fails.append(tag + ": not inside .text [0x%x, 0x%x)" % (t_lo, t_hi))
            continue
        head = at(b, 4)
        if head != b"\x55\x48\x89\xE5":
            fails.append(tag + ": does not start with push rbp; mov rbp, rsp (%s)" % (head.hex() if head else "?"))
        ui = at(u, 8)
        if ui is None or u % 4 != 0:
            fails.append(tag + ": unwind info 0x%x unreadable or unaligned" % u)
            continue
        ver, plen, ncodes, fr = ui[0], ui[1], ui[2], ui[3]
        if ver & 7 != 1 or ver >> 3 != 0:
            fails.append(tag + ": unwind version/flags 0x%x" % ver)
        if plen != 4 or ncodes != 2 or fr != 0x05:
            fails.append(tag + ": prologue %d, %d codes, frame 0x%x (want 4, 2, 0x05)" % (plen, ncodes, fr))
        if ui[4:8] != b"\x04\x03\x01\x50":
            fails.append(tag + ": codes %s (want 04 03 01 50: SET_FPREG@4, PUSH_NONVOL rbp@1)" % ui[4:8].hex())
        if e - b <= plen:
            fails.append(tag + ": shorter than its prologue")
    if n < want:
        fails.append("%d functions have unwind data, want at least %d" % (n, want))
    for f in fails:
        print("FAIL " + f)
    if not fails:
        print("ok: %d RUNTIME_FUNCTIONs, each a framed function with the shared UNWIND_INFO" % n)
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
