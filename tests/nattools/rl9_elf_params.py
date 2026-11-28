#!/usr/bin/env python3
"""RL9 (llvm roadmap): what compiler/nat_enc.fd nat_elf_debug needs to write
an ELF executable of the machine layer again, read from one it wrote.

    python3 tests/nattools/rl9_elf_params.py <elf> > params.txt

Output (read by tests/nattools/rl9_rewrite.fd), decimal numbers:
    line 1: arch entry pie data_off interp interp_len dyn dyn_len rvc img_len
            arch 3 x86-64, 1 AArch64, 2 RISC-V (nat_enc.fd NAT_*); every
            offset an image offset (the image is at file offset 0x1000);
            -1 for what the image has not (no data split, static)
    then one line per function symbol of .text: "<name> <start> <end>"
            (image offsets), in address order
The image's length is .text's size, plus .data's when section 2 is a .data
right after it (nat_elf_debug_sections makes .text the loaded image, or,
BG234, .text the code segment's part and .data the data segment's; bytes
appended after the image, .eh_frame, are left out).
"""
import struct
import sys

ARCH = {62: 3, 183: 1, 243: 2}


def main():
    d = open(sys.argv[1], "rb").read()
    if d[:4] != b"\x7fELF" or d[4] != 2:
        sys.exit("not an ELF64 file")
    e_type, e_machine = struct.unpack_from("<HH", d, 16)
    e_entry, e_phoff, e_shoff = struct.unpack_from("<QQQ", d, 24)
    e_flags, _, _, e_phnum, _, e_shnum, e_shstrndx = struct.unpack_from("<IHHHHHH", d, 48)
    if e_machine not in ARCH:
        sys.exit("machine %d is not one of the machine layer's" % e_machine)
    loads, interp, dyn = [], None, None
    for i in range(e_phnum):
        p = struct.unpack_from("<IIQQQQQQ", d, e_phoff + 56 * i)
        if p[0] == 1:
            loads.append(p)
        elif p[0] == 3:
            interp = p
        elif p[0] == 2:
            dyn = p
    if not loads or loads[0][2] != 0:
        sys.exit("the first PT_LOAD does not start at file offset 0")
    base = loads[0][3] + 0x1000
    img_len = None
    secs = [struct.unpack_from("<IIQQQQIIQQ", d, e_shoff + 64 * i) for i in range(e_shnum)] if e_shnum else []
    if len(secs) > 1 and secs[1][3] == base and secs[1][4] == 0x1000:
        img_len = secs[1][5]
        # BG234: the data segment's own section, adjoining .text
        if len(secs) > 2 and secs[2][1] == 1 and secs[2][3] == base + img_len and secs[2][4] == 0x1000 + img_len:
            img_len += secs[2][5]
    if img_len is None:
        img_len = max(p[2] + p[5] for p in loads) - 0x1000
    data_off = loads[1][2] - 0x1000 if len(loads) > 1 else -1
    out = [ARCH[e_machine], e_entry - base, 1 if e_type == 3 else 0, data_off]
    out += [interp[3] - base, interp[5]] if interp else [-1, 0]
    out += [dyn[3] - base, dyn[5]] if dyn else [-1, 0]
    out += [e_flags & 1, img_len]
    print(" ".join(str(x) for x in out))
    funcs = []
    for s in secs:
        if s[1] != 2:  # SYMTAB
            continue
        strtab = secs[s[6]]
        for k in range(s[5] // 24):
            st_name, st_info, _, st_shndx, st_value, st_size = struct.unpack_from("<IBBHQQ", d, s[4] + 24 * k)
            if st_info & 15 != 2 or st_shndx != 1:
                continue
            o = strtab[4] + st_name
            name = d[o:d.index(b"\0", o)].decode("latin-1")
            if name and " " not in name:
                funcs.append((st_value - base, st_value - base + st_size, name))
    for a, b, n in sorted(funcs):
        print("%s %d %d" % (n, a, b))


if __name__ == "__main__":
    main()
