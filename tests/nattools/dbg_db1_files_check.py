#!/usr/bin/env python3
"""DB1 (docs/roadmap/llvm-roadmap.md): every instruction has a line and a
file, the files a program does not name included. Builds
tests/bootstrap/test_nattools_lines_files.fd for the native Linux targets
and reads the image's .debug_line (DWARF 2-4, decoded here) and symbol
table:

  core     the rows of str_trim_start (stdlib/core.fd) are against a file
           named core.fd. Checked twice with a fresh binary artifact
           (FID_ASTCACHE): the first build parses the library and writes
           the artifact, the second reads it (its stored file records).
  mono     the rows of lgf_count__i64, the instance of a generic of
           nattools/lines_files_mod.fd, are against lines_files_mod.fd.
  inline   main's rows include one against lines_files_mod.fd at the line
           of lgf_add1's body (inlined into main), besides rows against the
           test file itself.

    python3 tests/nattools/dbg_db1_files_check.py [--fid FID] [--keep DIR]
           [--targets x86_64-linux,aarch64-linux,riscv64-linux]

Prints one line per check ("ok" / "FAIL") and exits 1 on any failure."""
import argparse
import os
import struct
import subprocess
import sys
import tempfile

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
SRC = "tests/bootstrap/test_nattools_lines_files.fd"
MOD = "tests/bootstrap/nattools/lines_files_mod.fd"


def elf_sections(d):
    out = {}
    shoff = struct.unpack_from("<Q", d, 0x28)[0]
    shentsize, shnum, shstrndx = struct.unpack_from("<HHH", d, 0x3A)

    def hdr(i):
        return struct.unpack_from("<IIQQQQIIQQ", d, shoff + i * shentsize)
    stroff = hdr(shstrndx)[4]
    for i in range(shnum):
        h = hdr(i)
        name = d[stroff + h[0]:d.index(b"\0", stroff + h[0])].decode("latin-1")
        out[name] = (h[4], h[5], h[3])
    return out


def elf_symbols(d, secs):
    """name -> address of every symbol with a nonzero address."""
    out = {}
    if ".symtab" not in secs or ".strtab" not in secs:
        return out
    off, size, _ = secs[".symtab"]
    soff = secs[".strtab"][0]
    for k in range(size // 24):
        name, info, other, shndx, value, sz = struct.unpack_from("<IBBHQQ", d, off + k * 24)
        if value == 0 or name == 0:
            continue
        nm = d[soff + name:d.index(b"\0", soff + name)].decode("latin-1")
        out[nm] = value
    return out


def uleb(d, p):
    r = 0
    s = 0
    while True:
        b = d[p]
        p += 1
        r |= (b & 0x7F) << s
        s += 7
        if b < 0x80:
            return r, p


def sleb(d, p):
    r = 0
    s = 0
    while True:
        b = d[p]
        p += 1
        r |= (b & 0x7F) << s
        s += 7
        if b < 0x80:
            if b & 0x40:
                r -= 1 << s
            return r, p


def cstr(d, p):
    e = d.index(b"\0", p)
    return d[p:e].decode("latin-1"), e + 1


def decode_lines(d):
    """Rows (address, file name, line) of every unit of a .debug_line."""
    rows = []
    p = 0
    while p + 4 <= len(d):
        unit_len = struct.unpack_from("<I", d, p)[0]
        end = p + 4 + unit_len
        ver = struct.unpack_from("<H", d, p + 4)[0]
        hlen = struct.unpack_from("<I", d, p + 6)[0]
        q = p + 10
        prog = q + hlen
        min_inst = d[q]
        q += 1
        if ver >= 4:
            q += 1  # maximum operations per instruction
        q += 1  # default is_stmt
        line_base = struct.unpack_from("<b", d, q)[0]
        line_range = d[q + 1]
        opcode_base = d[q + 2]
        q += 3
        std_len = list(d[q:q + opcode_base - 1])
        q += opcode_base - 1
        while d[q] != 0:  # include directories
            _, q = cstr(d, q)
        q += 1
        files = [None]
        while d[q] != 0:
            name, q = cstr(d, q)
            _, q = uleb(d, q)
            _, q = uleb(d, q)
            _, q = uleb(d, q)
            files.append(name)
        q = prog
        addr = 0
        fileno = 1
        line = 1

        def fname(n):
            return files[n] if 0 < n < len(files) else "?%d" % n
        while q < end:
            op = d[q]
            q += 1
            if op >= opcode_base:
                adj = op - opcode_base
                addr += (adj // line_range) * min_inst
                line += line_base + adj % line_range
                rows.append((addr, fname(fileno), line))
            elif op == 0:
                n, q = uleb(d, q)
                sub = d[q]
                if sub == 1:
                    addr, fileno, line = 0, 1, 1
                elif sub == 2:
                    addr = struct.unpack_from("<Q", d, q + 1)[0]
                elif sub == 3:
                    name, _ = cstr(d, q + 1)
                    files.append(name)
                q += n
            elif op == 1:
                rows.append((addr, fname(fileno), line))
            elif op == 2:
                v, q = uleb(d, q)
                addr += v * min_inst
            elif op == 3:
                v, q = sleb(d, q)
                line += v
            elif op == 4:
                fileno, q = uleb(d, q)
            elif op == 8:
                addr += ((255 - opcode_base) // line_range) * min_inst
            elif op == 9:
                addr += struct.unpack_from("<H", d, q)[0]
                q += 2
            else:
                for _ in range(std_len[op - 1]):
                    _, q = uleb(d, q)
        p = end
    return rows


def fn_rows(rows, syms, name):
    """The rows inside function `name` (from its address to the next symbol's)."""
    if name not in syms:
        return None
    start = syms[name]
    later = sorted(a for a in syms.values() if a > start)
    stop = later[0] if later else 1 << 64
    return [r for r in rows if start <= r[0] < stop]


def base(path):
    return path.replace("\\", "/").rsplit("/", 1)[-1]


def mod_line(text):
    lines = open(os.path.join(ROOT, MOD), encoding="utf-8").read().split("\n")
    for i, ln in enumerate(lines):
        if text in ln:
            return i + 1
    return 0


def emit(fid, target, out, env):
    cmd = [fid, "emit", SRC, "--target", target, "-o", out]
    r = subprocess.run(cmd, cwd=ROOT, env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    if r.returncode != 0 or not os.path.exists(out):
        tail = r.stdout.decode("latin-1", "replace").strip().split("\n")[-3:]
        return "emit failed: " + " | ".join(tail)
    return ""


def check_image(path, label, results):
    d = open(path, "rb").read()
    secs = elf_sections(d)
    if ".debug_line" not in secs:
        results.append((False, label + ": no .debug_line"))
        return
    off, size, _ = secs[".debug_line"]
    rows = decode_lines(d[off:off + size])
    syms = elf_symbols(d, secs)
    # core
    cr = fn_rows(rows, syms, "str_trim_start")
    if cr is None:
        results.append((False, label + " core: no symbol str_trim_start"))
    elif len(cr) == 0:
        results.append((False, label + " core: str_trim_start has no line rows"))
    else:
        files = sorted(set(base(r[1]) for r in cr))
        results.append((files == ["core.fd"], label + " core: str_trim_start rows against " + ",".join(files)))
    # mono
    inst = [n for n in syms if n.startswith("lgf_count__")]
    if not inst:
        results.append((False, label + " mono: no symbol lgf_count__*"))
    else:
        mr = fn_rows(rows, syms, inst[0])
        files = sorted(set(base(r[1]) for r in mr))
        ok = len(mr) > 0 and files == ["lines_files_mod.fd"]
        results.append((ok, label + " mono: " + inst[0] + " rows (%d) against %s" % (len(mr), ",".join(files) or "nothing")))
    # inline
    want = mod_line("val y: i64 = x * 3 + 1;")
    mn = fn_rows(rows, syms, "main")
    if mn is None:
        results.append((False, label + " inline: no symbol main"))
    else:
        own = [r for r in mn if base(r[1]) == base(SRC)]
        inl = [r for r in mn if base(r[1]) == "lines_files_mod.fd"]
        # a row against the test file past its last line is inlined code
        # written against the caller's file (what DB1 fixes)
        nlines = len(open(os.path.join(ROOT, SRC), encoding="utf-8").read().rstrip("\n").split("\n"))
        stray = [r for r in own if r[2] > nlines]
        ok = len(own) > 0 and any(r[2] == want for r in inl) and not stray
        got = ",".join(sorted(set("%s:%d" % (base(r[1]), r[2]) for r in mn)))
        results.append((ok, label + " inline: main rows " + got + " (want lines_files_mod.fd:%d)" % want))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--fid", default=os.environ.get("FID", "release/fid.exe"))
    ap.add_argument("--keep", default="")
    ap.add_argument("--targets", default="x86_64-linux,aarch64-linux")
    a = ap.parse_args()
    fid = a.fid if os.path.isabs(a.fid) else os.path.join(ROOT, a.fid)
    work = a.keep or tempfile.mkdtemp(prefix="db1-files-")
    os.makedirs(work, exist_ok=True)
    env = dict(os.environ)
    env["FIDES_NO_CACHE"] = "1"
    results = []
    for target in a.targets.split(","):
        art = os.path.join(work, "ast-" + target + ".bin")
        if os.path.exists(art):
            os.remove(art)
        env["FID_ASTCACHE"] = art
        for run in ("parse", "artifact"):
            out = os.path.join(work, "lf-%s-%s" % (target, run))
            err = emit(fid, target, out, env)
            label = "%s (%s)" % (target, run)
            if err:
                results.append((False, label + ": " + err))
                continue
            check_image(out, label, results)
        if not os.path.exists(art):
            results.append((False, target + ": no artifact written at " + art))
    bad = 0
    for ok, msg in results:
        print(("  ok    " if ok else "  FAIL  ") + msg)
        if not ok:
            bad += 1
    print("DB1 files: %d checks, %d failed" % (len(results), bad))
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
