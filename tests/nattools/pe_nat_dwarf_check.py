#!/usr/bin/env python3
"""CT6: the DWARF of a `--nat` Windows debug build (compiler/nat_enc.fd
nat_pe_debug; compiler/nat_ir.fd ng_dbg_win_dwarf).

    FID_NAT_DWARF=1 fid emit <test.fd> --nat -o t.exe        (or --debug / -g)
    python3 tests/nattools/pe_nat_dwarf_check.py t.exe [<test.fd>] [--vars] [--plain]

Reads the PE as gdb and lldb do and checks:
  - the headers: SizeOfHeaders 1024, .text and .data first, then
    .debug_abbrev, .debug_info, .debug_line, .debug_frame (and .debug_loc
    when a value has a location list), each named "/<n>" into the COFF
    string table, in file and address order, discardable, SizeOfImage
    covering them, the COFF symbol table after the last one;
  - .debug_line: rows whose addresses lie in .text (ImageBase 0x400000,
    .text at 0x401000), at least one sequence; with <test.fd>, every line
    the test marks `// @name` (tests/bootstrap/test_machine_dbg_run.fd) has
    a row;
  - .debug_info: a DWARF 4 compile unit, subprograms inside .text, each
    with DW_AT_frame_base; every variable's location a register, a CFA
    slot or a constant, or a location list whose ranges lie inside its
    function (.debug_loc);
  - .debug_frame: an FDE per framed function, inside .text;
  - with --vars and <test.fd> = tests/bootstrap/test_machine_dbg_vars.fd:
    dbg_vars_check.py's checks (parameters, variables, types, locations of
    scale, area, mix, count_bytes, main) on this image;
  - --plain: the image is a plain `--nat` build instead: no debug sections,
    SizeOfHeaders 512 (the CT6 switch left it as it was).
When x86_64-w64-mingw32-objdump (binutils) or llvm-dwarfdump is on PATH,
it must decode the line table and the info without complaint.
Exit 0 when every check holds."""
import os
import re
import shutil
import struct
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import dbg_vars_check as dv  # noqa: E402
import dbg_run_check as dr  # noqa: E402

IMAGE_BASE = 0x400000
DEBUG_NAMES = [".debug_abbrev", ".debug_info", ".debug_line", ".debug_frame"]


def u16(b, o):
    return struct.unpack_from("<H", b, o)[0]


def u32(b, o):
    return struct.unpack_from("<I", b, o)[0]


def pe_sections(img):
    """[(name, vsize, rva, raw_size, raw_off, flags)] with long names
    resolved through the COFF string table."""
    pe = u32(img, 0x3C)
    if img[pe:pe + 4] != b"PE\0\0":
        raise SystemExit("FAIL: not a PE image")
    nsec = u16(img, pe + 6)
    symptr = u32(img, pe + 12)
    nsyms = u32(img, pe + 16)
    optsz = u16(img, pe + 20)
    sh = pe + 24 + optsz
    strtab = symptr + 18 * nsyms
    out = []
    for k in range(nsec):
        h = sh + 40 * k
        raw = img[h:h + 8].rstrip(b"\0").decode("latin-1")
        if raw.startswith("/") and symptr:
            off = int(raw[1:])
            e = img.index(b"\0", strtab + off)
            raw = img[strtab + off:e].decode("latin-1")
        vsize, rva, rsize, roff = struct.unpack_from("<IIII", img, h + 8)
        flags = u32(img, h + 36)
        out.append((raw, vsize, rva, rsize, roff, flags))
    return out


def make_section_reader(img):
    secs = pe_sections(img)

    def section(_data, want):
        for name, vsize, rva, rsize, roff, flags in secs:
            if name == want:
                return img[roff:roff + vsize]
        return None
    return section


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    flags = [a for a in sys.argv[1:] if a.startswith("--")]
    if not args:
        print(__doc__)
        return 2
    exe = args[0]
    src = args[1] if len(args) > 1 else None
    img = open(exe, "rb").read()
    pe = u32(img, 0x3C)
    opt = pe + 24
    size_of_image = u32(img, opt + 56)
    size_of_headers = u32(img, opt + 60)
    symptr = u32(img, pe + 12)
    secs = pe_sections(img)
    names = [s[0] for s in secs]
    fails = []
    if "--plain" in flags:
        if any(n.startswith(".debug") or n.startswith("/") for n in names):
            fails.append("a plain --nat build has debug sections: %r" % names)
        if size_of_headers != 512:
            fails.append("a plain --nat build has SizeOfHeaders %d, not 512" % size_of_headers)
        for f in fails:
            print("FAIL: " + f)
        if not fails:
            print("pe nat plain OK (%s)" % ", ".join(names))
        return 1 if fails else 0
    # --- headers
    if size_of_headers != 1024:
        fails.append("SizeOfHeaders is %d, not 1024" % size_of_headers)
    if names[:2] != [".text", ".data"]:
        fails.append("the first sections are %r, not .text, .data" % names[:2])
    want = list(DEBUG_NAMES)
    if ".debug_loc" in names:
        want.append(".debug_loc")
    if names[2:] != want:
        fails.append("the debug sections are %r, want %r" % (names[2:], want))
    last_va = 0
    last_off = 0
    for name, vsize, rva, rsize, roff, fl in secs:
        if rva < last_va or roff < last_off:
            fails.append("%s out of order (rva 0x%x, file 0x%x)" % (name, rva, roff))
        if roff % 512 or rsize % 512 or rsize < min(vsize, rsize) or roff + rsize > len(img):
            fails.append("%s: raw data 0x%x+0x%x not aligned or past the file" % (name, roff, rsize))
        if name.startswith(".debug") and fl != 0x42000040:
            fails.append("%s: flags 0x%x, not 0x42000040" % (name, fl))
        last_va = rva + ((vsize + 0xFFF) & ~0xFFF)
        last_off = roff + rsize
    if size_of_image < last_va:
        fails.append("SizeOfImage 0x%x does not cover the sections (0x%x)" % (size_of_image, last_va))
    if symptr != last_off:
        fails.append("the symbol table is at 0x%x, not after the sections (0x%x)" % (symptr, last_off))
    text = secs[0]
    tlo = IMAGE_BASE + text[2]
    thi = tlo + text[1]
    section = make_section_reader(img)
    # --- .debug_line
    rows = dr.line_rows(section(img, ".debug_line") or b"")
    if not any(r[3] for r in rows):
        fails.append(".debug_line has no sequence")
    bad = [r for r in rows if not (tlo <= r[0] <= thi)]
    if bad:
        fails.append("%d line rows outside .text, first at 0x%x" % (len(bad), bad[0][0]))
    if src:
        lines = open(src, encoding="utf-8").read().split("\n")
        have = {r[2] for r in rows if not r[3]}
        for i, l in enumerate(lines):
            m = re.search(r"// @(\w+)$", l)
            if m and (i + 1) not in have:
                fails.append("@%s: line %d has no row" % (m.group(1), i + 1))
    # --- .debug_info
    info = section(img, ".debug_info")
    ab = section(img, ".debug_abbrev")
    loc_sec = section(img, ".debug_loc")
    if info is None or ab is None:
        fails.append("no .debug_info / .debug_abbrev")
    else:
        if u16(info, 4) != 4:
            fails.append(".debug_info version %d, not 4" % u16(info, 4))
        dies = dv.dies(info, dv.abbrevs(ab))
        cu_base = dies[0][2].get(0x11, 0)
        nsub = 0
        for d in dies:
            if d[1] != dv.DW_TAG_subprogram:
                continue
            nsub += 1
            lo = d[2].get(0x11, 0)
            hi = lo + d[2].get(0x12, 0)
            nm = d[2].get(0x03)
            if not (tlo <= lo <= hi <= thi):
                fails.append("%s: [0x%x, 0x%x) outside .text" % (nm, lo, hi))
            if 0x40 in d[2] and d[2][0x40] != b"\x9c":
                fails.append("%s: frame base is not DW_OP_call_frame_cfa" % nm)
            for k in [k for k in dies if k[4] == d[0]]:
                loc = k[2].get(0x02)
                if loc is None:
                    continue
                if not dv.loc_ok(loc, loc_sec):
                    fails.append("%s.%s: location not a register, CFA slot or constant" % (nm, k[2].get(0x03)))
                if isinstance(loc, int):
                    for s, e, _ in dr.loc_list(loc_sec, loc, cu_base):
                        if not (lo <= s < e <= hi):
                            fails.append("%s.%s: range [0x%x, 0x%x) outside the function" % (nm, k[2].get(0x03), s, e))
        if nsub == 0:
            fails.append(".debug_info has no subprogram")
    # --- .debug_frame
    fdes = dr.cfi_entries(section(img, ".debug_frame") or b"")
    if not fdes:
        fails.append(".debug_frame has no FDE")
    for f in fdes:
        if not (tlo <= f[0] <= f[1] <= thi):
            fails.append("FDE [0x%x, 0x%x) outside .text" % (f[0], f[1]))
    # --- the DB4 entries of test_machine_dbg_vars.fd, through dbg_vars_check
    if "--vars" in flags and src:
        dv.section = make_section_reader(img)
        old = sys.argv
        sys.argv = [old[0], exe, src]
        try:
            if dv.main() != 0:
                fails.append("dbg_vars_check.py's checks failed (above)")
        finally:
            sys.argv = old
    # --- an independent reader
    for tool, targs in (("x86_64-w64-mingw32-objdump", ["--dwarf=decodedline,info,loc"]), ("llvm-dwarfdump", ["--debug-line", "--debug-info"])):
        path = shutil.which(tool)
        if not path:
            continue
        r = subprocess.run([path] + targs + [exe], capture_output=True, text=True)
        text_out = r.stdout + r.stderr
        if r.returncode != 0 or re.search(r"(?i)warning|error", text_out):
            fails.append("%s: %s" % (tool, text_out.strip()[-400:]))
        elif src and os.path.basename(src) not in text_out:
            fails.append("%s does not name %s" % (tool, os.path.basename(src)))
        else:
            print("%s reads the DWARF" % tool)
    for f in fails:
        print("FAIL: " + f)
    if not fails:
        print("pe nat dwarf OK (%d line rows, sections %s)" % (len(rows), ", ".join(names[2:])))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
