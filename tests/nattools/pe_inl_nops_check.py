#!/usr/bin/env python3
"""BG243: read the images tests/nattools/pe_inl_nops_unit.fd writes
(pe_ir's debug build of a `main` whose inlined call of `twice`, lines 7
and 8, was folded away):

    PE_INL_OUT=a.exe PE_INL_OUT_PLAIN=b.exe fid run tests/nattools/pe_inl_nops_unit.fd
    python tests/nattools/pe_inl_nops_check.py a.exe b.exe

  a.exe: .debug_line has a row for line 7 and one for line 8 at the next
         byte, each byte a nop (0x90), and the next row is on another line
         (the code that was kept); .debug_info has an abstract `twice`
         (DW_AT_inline, decl line 6) and, in main, a
         DW_TAG_inlined_subroutine of it over those two bytes, called from
         line 3; llvm-dwarfdump --verify passes when installed.
  b.exe: the same module without the inliner's records: no row for 7 or 8,
         no entry for twice (the build of before).
Exit 0 when every check holds."""
import os
import shutil
import struct
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import dbg_vars_check as dv  # noqa: E402
import dbg_run_check as dr  # noqa: E402

BASE = 0x140000000


def text_byte(data, addr):
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    n = struct.unpack_from("<H", data, pe + 6)[0]
    opt = struct.unpack_from("<H", data, pe + 20)[0]
    at = pe + 24 + opt
    for k in range(n):
        h = at + 40 * k
        va, size = struct.unpack_from("<II", data, h + 12)[0], struct.unpack_from("<I", data, h + 8)[0]
        raw = struct.unpack_from("<I", data, h + 20)[0]
        rva = addr - BASE
        if va <= rva < va + size:
            return data[raw + rva - va]
    return None


def main():
    if len(sys.argv) < 3:
        print(__doc__)
        return 2
    a, b = sys.argv[1], sys.argv[2]
    fails = []
    data = open(a, "rb").read()
    dbg = dr.Debug(a)
    rows = sorted((r[0], r[2]) for r in dbg.rows if not r[3])
    r7 = [ad for ad, ln in rows if ln == 7]
    r8 = [ad for ad, ln in rows if ln == 8]
    if len(r7) != 1 or len(r8) != 1:
        fails.append("rows of line 7 %r and 8 %r, want one each" % (r7, r8))
    else:
        if r8[0] != r7[0] + 1:
            fails.append("line 8's row at 0x%x, want the byte after line 7's (0x%x)" % (r8[0], r7[0]))
        for ad in (r7[0], r8[0]):
            if text_byte(data, ad) != 0x90:
                fails.append("the byte of the row at 0x%x is %r, not a nop" % (ad, text_byte(data, ad)))
        after = [ln for ad, ln in rows if ad > r8[0]]
        if not after or after[0] in (7, 8):
            fails.append("the row after line 8's is %r, want the kept code's line" % (after[:1],))
        if not fails:
            print("ok   lines 7 and 8: a nop each at 0x%x, 0x%x, then line %d" % (r7[0], r8[0], after[0]))
    ab = [d for d in dbg.dies if d[1] == dv.DW_TAG_subprogram and d[2].get(0x03) == "twice"]
    mains = [d for d in dbg.dies if d[1] == dv.DW_TAG_subprogram and d[2].get(0x03) == "main" and 0x11 in d[2]]
    if len(ab) != 1 or ab[0][2].get(0x20) != 1 or ab[0][2].get(0x3B) != 6 or 0x11 in ab[0][2]:
        fails.append("no abstract entry for twice (DW_AT_inline 1, decl line 6): %r" % ([d[2] for d in ab],))
    elif len(mains) != 1:
        fails.append("%d subprograms main" % len(mains))
    else:
        calls = [(k[2].get(0x11), k[2].get(0x12), k[2].get(0x59)) for k in dbg.dies if k[1] == 0x1D and k[4] == mains[0][0] and k[2].get(0x31) == ab[0][0]]
        want = [(r7[0] if r7 else None, 2, 3)]
        if calls != want:
            fails.append("main's inlined calls of twice %r, want %r (low_pc, length, call line)" % (calls, want))
        else:
            print("ok   twice: abstract entry, inlined into main over its two nops, called from line 3")
    dd = os.environ.get("LLVM_DWARFDUMP") or shutil.which("llvm-dwarfdump")
    if not dd:
        cand = os.path.join("C:\\Users", os.environ.get("USERNAME", ""), "AppData", "Local", "Programs", "llvm-21.1.0", "bin", "llvm-dwarfdump.exe")
        if os.path.exists(cand):
            dd = cand
    if dd:
        r = subprocess.run([dd, "--verify", a], capture_output=True, text=True)
        if r.returncode != 0:
            fails.append("llvm-dwarfdump --verify: %s" % (r.stdout + r.stderr)[-600:])
        else:
            print("ok   llvm-dwarfdump --verify")
    else:
        print("skip llvm-dwarfdump (not installed)")
    # the build without records
    dbg2 = dr.Debug(b)
    lines2 = set(r[2] for r in dbg2.rows if not r[3])
    if 7 in lines2 or 8 in lines2:
        fails.append("without records: rows of line 7/8")
    if any(d[1] == dv.DW_TAG_subprogram and d[2].get(0x03) == "twice" for d in dbg2.dies):
        fails.append("without records: an entry for twice")
    if not (7 in lines2 or 8 in lines2):
        print("ok   without records: no row of 7 or 8, no entry for twice")
    for f in fails:
        print("FAIL " + f)
    print("pe_inl_nops_check: %s" % ("ok" if not fails else "FAILED"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
