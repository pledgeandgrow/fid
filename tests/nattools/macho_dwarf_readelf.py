#!/usr/bin/env python3
"""DB10 (docs/roadmap/llvm-roadmap.md): the DWARF of a Mach-O executable
of the machine layer (compiler/nat_enc.fd nat_macho_debug_cfi,
nat_macho_dwarf, nat_macho_dwarf_seg) read by a second, independent
decoder: GNU readelf, where llvm-dwarfdump (macOS's reader) is not at hand.

Usage: macho_dwarf_readelf.py <image> [--fn NAME]... [--file NAME]...
       [--keep <elf>]

The __DWARF segment's sections (__debug_abbrev, __debug_info,
__debug_line, and __debug_frame / __debug_loc when present) are copied
into a minimal ELF64 (no program headers; a NOBITS .text at the address
of the Mach-O's __text, so the addresses read the same), which `readelf
--debug-dump=info,decodedline[,frames]` decodes (readelf on PATH, or
`wsl.exe -e readelf` on Windows). It checks that readelf exits 0 with no
warning or error, that each --fn NAME is a DW_AT_name of a
DW_TAG_subprogram, that each --file NAME has decoded rows, that every
row's address lies in __text, and, with __debug_frame, that every FDE
decodes. Exit 0 when all hold; 2 when no readelf can be run.
"""
import os
import re
import shutil
import struct
import subprocess
import sys
import tempfile

LC_SEGMENT_64 = 0x19
MACHINES = {0x01000007: 62, 0x0100000C: 183}


def macho_sections(d):
    if struct.unpack_from("<I", d, 0)[0] != 0xFEEDFACF:
        raise ValueError("not a 64-bit little-endian Mach-O image")
    cpu = struct.unpack_from("<I", d, 4)[0]
    ncmds = struct.unpack_from("<I", d, 16)[0]
    at = 32
    secs = {}
    for _ in range(ncmds):
        cmd, size = struct.unpack_from("<II", d, at)
        if cmd == LC_SEGMENT_64:
            nsects = struct.unpack_from("<I", d, at + 64)[0]
            for k in range(nsects):
                s = at + 72 + 80 * k
                sect = d[s:s + 16].rstrip(b"\0").decode()
                seg = d[s + 16:s + 32].rstrip(b"\0").decode()
                addr, sz = struct.unpack_from("<QQ", d, s + 32)
                off = struct.unpack_from("<I", d, s + 48)[0]
                secs[(seg, sect)] = (addr, sz, off)
        at += size
    return MACHINES.get(cpu, 62), secs


def to_elf(d, machine, secs):
    names = [""]
    datas = [b""]
    hdrs = [(0, 0, 0, 0, 0, 0)]  # name index, type, flags, addr, size, align
    if ("__TEXT", "__text") in secs:
        a, sz, _ = secs[("__TEXT", "__text")]
        names.append(".text")
        datas.append(b"")
        hdrs.append((len(names) - 1, 8, 6, a, sz, 16))  # NOBITS, ALLOC|EXEC
    for sect in ("__debug_abbrev", "__debug_info", "__debug_line", "__debug_frame", "__debug_loc"):
        if ("__DWARF", sect) in secs:
            _, sz, off = secs[("__DWARF", sect)]
            names.append("." + sect[2:])
            datas.append(d[off:off + sz])
            hdrs.append((len(names) - 1, 1, 0, 0, sz, 1))
    names.append(".shstrtab")
    shstr = b"\0"
    name_off = [0]
    for nm in names[1:]:
        name_off.append(len(shstr))
        shstr += nm.encode() + b"\0"
    datas.append(shstr)
    hdrs.append((len(names) - 1, 3, 0, 0, len(shstr), 1))
    out = bytearray(64)
    offs = []
    for i, data in enumerate(datas):
        while len(out) % 8:
            out.append(0)
        offs.append(len(out))
        out += data
    while len(out) % 8:
        out.append(0)
    shoff = len(out)
    for i, (ni, typ, flags, addr, size, align) in enumerate(hdrs):
        if i == 0:
            out += bytes(64)
            continue
        out += struct.pack("<IIQQQQIIQQ", name_off[ni], typ, flags, addr, offs[i], size, 0, 0, align, 0)
    out[0:16] = b"\x7fELF\x02\x01\x01" + bytes(9)
    struct.pack_into("<HHIQQQIHHHHHH", out, 16, 2, machine, 1, 0, 0, shoff, 0, 64, 0, 0, 64, len(hdrs), len(hdrs) - 1)
    return bytes(out)


def readelf_cmd(path):
    if shutil.which("readelf"):
        return ["readelf", path]
    if os.name == "nt" and shutil.which("wsl.exe"):
        p = os.path.abspath(path).replace("\\", "/")
        m = re.match(r"^([A-Za-z]):/(.*)$", p)
        if m:
            p = "/mnt/%s/%s" % (m.group(1).lower(), m.group(2))
        return ["wsl.exe", "-e", "readelf", p]
    return None


def main():
    args = sys.argv[1:]
    fns, files, keep, image = [], [], None, None
    while args:
        a = args.pop(0)
        if a == "--fn":
            fns.append(args.pop(0))
        elif a == "--file":
            files.append(args.pop(0))
        elif a == "--keep":
            keep = args.pop(0)
        else:
            image = a
    if image is None:
        print(__doc__)
        return 2
    d = open(image, "rb").read()
    machine, secs = macho_sections(d)
    fails = []
    for need in ("__debug_abbrev", "__debug_info", "__debug_line"):
        if ("__DWARF", need) not in secs:
            fails.append("no __DWARF,%s section" % need)
    if fails:
        for f in fails:
            print("FAIL " + f)
        return 1
    elf = to_elf(d, machine, secs)
    path = keep or os.path.join(tempfile.mkdtemp(), "macho-dwarf.elf")
    with open(path, "wb") as f:
        f.write(elf)
    base = readelf_cmd(path)
    if base is None:
        print("skip: no readelf here")
        return 2
    dumps = "info,decodedline"
    if ("__DWARF", "__debug_frame") in secs:
        dumps += ",frames"
    r = subprocess.run(base[:-1] + ["--debug-dump=" + dumps, base[-1]], capture_output=True, text=True)
    text = r.stdout + r.stderr
    if r.returncode != 0:
        fails.append("readelf exits %d" % r.returncode)
    for line in text.splitlines():
        if re.search(r"\b(Warning|Error|error|warning)\b", line):
            fails.append("readelf: " + line.strip())
            break
    # the subprograms' names
    sub_names = []
    in_sub = False
    for line in text.splitlines():
        if "DW_TAG_" in line:
            in_sub = "DW_TAG_subprogram" in line
        elif in_sub and "DW_AT_name" in line:
            sub_names.append(line.split(":", 1)[-1].split(")")[-1].strip())
            in_sub = False
    for fn in fns:
        if not any(n == fn or n.endswith(" " + fn) or n.endswith(fn) for n in sub_names):
            fails.append("no subprogram named %s (readelf shows %s)" % (fn, sub_names[:8]))
    # the decoded rows: "file  line  address ..."
    tlo, tsz = (secs[("__TEXT", "__text")][0], secs[("__TEXT", "__text")][1]) if ("__TEXT", "__text") in secs else (0, 1 << 62)
    rows = 0
    per_file = {}
    for line in text.split("Contents of the .debug_line section", 1)[-1].splitlines():
        m = re.match(r"^(\S+)\s+(\d+|-)\s+(0x[0-9a-f]+)", line)
        if m:
            rows += 1
            per_file[os.path.basename(m.group(1))] = per_file.get(os.path.basename(m.group(1)), 0) + 1
            a = int(m.group(3), 16)
            if not (tlo <= a <= tlo + tsz):
                fails.append("row at %#x outside __text [%#x, %#x]" % (a, tlo, tlo + tsz))
                break
    if rows == 0:
        fails.append("readelf decodes no line rows")
    for fl in files:
        if per_file.get(os.path.basename(fl), 0) == 0:
            fails.append("no rows for %s (rows per file: %s)" % (fl, per_file))
    fdes = len(re.findall(r" FDE cie=", text))
    for f in fails:
        print("FAIL " + f)
    if fails:
        return 1
    extra = (", %d FDEs" % fdes) if ("__DWARF", "__debug_frame") in secs else ""
    print("ok: readelf decodes %d subprograms and %d line rows (%s)%s" % (len(sub_names), rows, ", ".join("%s %d" % kv for kv in sorted(per_file.items())), extra))
    return 0


if __name__ == "__main__":
    sys.exit(main())
