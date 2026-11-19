#!/usr/bin/env python3
"""pe-syms-check.py [--fid FID] [FILE.fd]: the COFF symbol table of `fid emit`
executables (compiler/pe_ir_enc.fd pe_append_coff_symbols; own-backend
roadmap DB3, first step).

Builds FILE (default tests/bootstrap/hello.fd) twice with the default
Windows backend (pe_ir): once as is, once with FID_PE_SYMS=0. Checks that
  - the image with symbols is the image without them plus a trailing COFF
    symbol table: the bytes are equal except PointerToSymbolTable and
    NumberOfSymbols (COFF header, file offsets 76..83), and the table starts
    where the other file ends;
  - every symbol is a function in section 1 (.text) at an offset inside it,
    and `main` and `__fid_entry` are there, `__fid_entry` at the entry point;
  - the string table's size word matches what follows.
Prints the symbol count and the first few names. Windows host.
"""
import os
import struct
import subprocess
import sys
import tempfile


def emit(fid, src, out, syms):
    env = dict(os.environ)
    # the build cache does not see FID_PE_SYMS: never reuse a cached build
    env["FIDES_NO_CACHE"] = "1"
    if not syms:
        env["FID_PE_SYMS"] = "0"
    else:
        env.pop("FID_PE_SYMS", None)
    r = subprocess.run([fid, "emit", src, "-o", out], env=env,
                       capture_output=True, text=True)
    if r.returncode != 0:
        sys.exit("emit failed (%s):\n%s%s" % (out, r.stdout, r.stderr))
    with open(out, "rb") as f:
        return f.read()


def main():
    args = sys.argv[1:]
    fid = "release/fid.exe"
    if len(args) >= 2 and args[0] == "--fid":
        fid = args[1]
        args = args[2:]
    src = args[0] if args else "tests/bootstrap/hello.fd"
    tmp = tempfile.mkdtemp()
    with_syms = emit(fid, src, os.path.join(tmp, "with.exe"), True)
    without = emit(fid, src, os.path.join(tmp, "without.exe"), False)
    fail = []

    ptr, nsyms = struct.unpack_from("<II", with_syms, 76)
    ptr0, nsyms0 = struct.unpack_from("<II", without, 76)
    if ptr0 != 0 or nsyms0 != 0:
        fail.append("FID_PE_SYMS=0 image still has a symbol table")
    if ptr != len(without):
        fail.append("symbol table at %d, expected the end of the plain image %d" % (ptr, len(without)))
    if with_syms[:76] != without[:76] or with_syms[84:len(without)] != without[84:]:
        fail.append("the images differ outside the symbol table fields")

    # .text bounds from the first section header (pe_build_file layout)
    opt_size = struct.unpack_from("<H", with_syms, 68 + 16)[0]
    sec = 68 + 20 + opt_size
    text_vsize = struct.unpack_from("<I", with_syms, sec + 8)[0]
    text_rva = struct.unpack_from("<I", with_syms, sec + 12)[0]
    entry_rva = struct.unpack_from("<I", with_syms, 68 + 20 + 16)[0]

    strtab_at = ptr + 18 * nsyms
    if strtab_at + 4 > len(with_syms):
        fail.append("symbol table runs past the end of the file")
        print("\n".join("FAIL: " + x for x in fail))
        return 1
    strtab_size = struct.unpack_from("<I", with_syms, strtab_at)[0]
    if strtab_at + strtab_size != len(with_syms):
        fail.append("string table size %d does not match the %d bytes after the symbols"
                    % (strtab_size, len(with_syms) - strtab_at))
    names = {}
    for i in range(nsyms):
        rec = with_syms[ptr + 18 * i: ptr + 18 * i + 18]
        raw, value, secno, typ, cls, naux = struct.unpack("<8sIhHBB", rec)
        if raw[:4] == b"\0\0\0\0":
            off = struct.unpack_from("<I", raw, 4)[0]
            end = with_syms.index(b"\0", strtab_at + off)
            name = with_syms[strtab_at + off:end].decode("ascii", "replace")
        else:
            name = raw.rstrip(b"\0").decode("ascii", "replace")
        if secno != 1 or typ != 0x20 or cls != 2 or naux != 0:
            fail.append("symbol %s: section %d type %#x class %d aux %d" % (name, secno, typ, cls, naux))
        if value >= text_vsize:
            fail.append("symbol %s at %#x is outside .text (%#x bytes)" % (name, value, text_vsize))
        if name in names:
            fail.append("symbol %s twice" % name)
        names[name] = value
    for want in ("main", "__fid_entry"):
        if want not in names:
            fail.append("no symbol %s" % want)
    if "__fid_entry" in names and text_rva + names["__fid_entry"] != entry_rva:
        fail.append("__fid_entry at %#x, entry point at %#x" % (text_rva + names["__fid_entry"], entry_rva))

    print("pe-syms-check: %s: %d symbols, %d bytes of table (%s, ...)"
          % (src, nsyms, len(with_syms) - ptr, ", ".join(sorted(names)[:5])))
    for x in fail:
        print("FAIL: " + x)
    return 1 if fail else 0


if __name__ == "__main__":
    sys.exit(main())
