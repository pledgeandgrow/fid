#!/usr/bin/env python3
"""bin-diff-locate.py -- where two builds of one program differ (RL13, RL14).

    python3 scripts/verify/bin-diff-locate.py <a> <b> [--max N] [--json]

When scripts/native-fixpoint.sh finds stage 2 != stage 3, or
scripts/verify/repro.sh finds two builds that differ, `cmp -l` gives byte
offsets and nothing else. This script names the place: it reads the section
table of a PE (Windows) or ELF64 (Linux) image, and for an ELF its symbol
table too, groups the differing bytes into runs and reports, per section,
how many bytes differ and the first runs with their offset, address and
(ELF) the function or object that holds them. Two files of different
lengths are compared over the common part, and the length difference is
reported (a drift that moves everything after it shows as one long run).

Text files (generated C) are compared line by line instead: the first
differing lines, and the C function each one is in (the last line before it
that starts a function definition at column 0).

Exit status: 0 when the files are equal, 1 when they differ, 2 on bad use.
"""
import json
import struct
import sys


def read(path):
    with open(path, "rb") as f:
        return f.read()


def pe_sections(d):
    """[(name, file_off, file_size, rva)] of a PE image, or None."""
    if len(d) < 0x40 or d[:2] != b"MZ":
        return None
    pe = struct.unpack_from("<I", d, 0x3C)[0]
    if pe + 24 > len(d) or d[pe:pe + 4] != b"PE\0\0":
        return None
    nsec = struct.unpack_from("<H", d, pe + 6)[0]
    opt_size = struct.unpack_from("<H", d, pe + 20)[0]
    sh = pe + 24 + opt_size
    out = [("headers", 0, min(sh + 40 * nsec, len(d)), 0)]
    for i in range(nsec):
        o = sh + 40 * i
        if o + 40 > len(d):
            break
        name = d[o:o + 8].rstrip(b"\0").decode("latin-1")
        vsize, rva, rsize, roff = struct.unpack_from("<IIII", d, o + 8)
        out.append((name, roff, rsize, rva))
    return out


def elf_sections(d):
    """([(name, file_off, file_size, addr)], [(addr, size, name)]) of an
    ELF64 little-endian image, or None."""
    if len(d) < 64 or d[:4] != b"\x7fELF" or d[4] != 2 or d[5] != 1:
        return None
    shoff = struct.unpack_from("<Q", d, 0x28)[0]
    shentsize, shnum, shstrndx = struct.unpack_from("<HHH", d, 0x3A)
    if shoff == 0 or shentsize < 64 or shoff + shnum * shentsize > len(d):
        return [("headers", 0, 64, 0)], []
    raw = []
    for i in range(shnum):
        o = shoff + i * shentsize
        nm, typ, flags, addr, off, size, link, info, align, entsize = \
            struct.unpack_from("<IIQQQQIIQQ", d, o)
        raw.append((nm, typ, addr, off, size, link, entsize))
    strtab = raw[shstrndx] if shstrndx < len(raw) else None

    def cstr(base, at):
        if base is None:
            return ""
        s = base[3] + at
        e = d.find(b"\0", s)
        return d[s:e if e >= 0 else len(d)].decode("latin-1")

    secs = [("ELF header", 0, 64, 0)]
    syms = []
    for (nm, typ, addr, off, size, link, entsize) in raw:
        name = cstr(strtab, nm)
        if typ != 8:  # SHT_NOBITS holds no file bytes
            secs.append((name or "?", off, size, addr))
        if typ == 2 and entsize >= 24 and link < len(raw):  # SHT_SYMTAB
            st = raw[link]
            for k in range(size // entsize):
                so = off + k * entsize
                if so + 24 > len(d):
                    break
                sn, sinfo, sother, shndx, sval, ssz = struct.unpack_from("<IBBHQQ", d, so)
                if sval and shndx:
                    syms.append((sval, ssz, cstr(st, sn)))
    secs.append(("section headers", shoff, shnum * shentsize, 0))
    syms.sort()
    return secs, syms


def runs_of(a, b):
    """Runs [start, end) of the common part where a and b differ; two
    differing bytes at most 8 apart are one run."""
    n = min(len(a), len(b))
    out = []
    i = 0
    while i < n:
        if a[i] != b[i]:
            s = i
            last = i
            j = i + 1
            while j < n and j - last <= 8:
                if a[j] != b[j]:
                    last = j
                j += 1
            out.append((s, last + 1))
            i = last + 1
        else:
            i += 1
    return out


def section_of(secs, off):
    best = None
    for (name, fo, fs, addr) in secs:
        if fs and fo <= off < fo + fs:
            # the smallest section holding it (headers overlap nothing else)
            if best is None or fs < best[2]:
                best = (name, fo, fs, addr)
    return best


def symbol_of(syms, addr):
    lo, hi = 0, len(syms)
    while lo < hi:
        m = (lo + hi) // 2
        if syms[m][0] <= addr:
            lo = m + 1
        else:
            hi = m
    if lo == 0:
        return None
    sval, ssz, name = syms[lo - 1]
    if ssz == 0 or addr < sval + ssz:
        return "%s+0x%x" % (name, addr - sval)
    return None


def is_text(d):
    return b"\0" not in d[:65536]


def text_diff(a, b, maxn):
    la = a.decode("latin-1").split("\n")
    lb = b.decode("latin-1").split("\n")
    out = []
    fn = ""
    for i in range(max(len(la), len(lb))):
        x = la[i] if i < len(la) else None
        y = lb[i] if i < len(lb) else None
        if x is not None and x[:1].isalpha() and x.rstrip().endswith("{") and "(" in x:
            fn = x.split("(")[0].split()[-1] if x.split("(")[0].split() else ""
        if x != y:
            out.append({"line": i + 1, "function": fn,
                        "a": ("<end>" if x is None else x)[:160],
                        "b": ("<end>" if y is None else y)[:160]})
            if len(out) >= maxn:
                break
    return {"kind": "text", "lines_a": len(la), "lines_b": len(lb), "first": out}


def locate(a, b, maxn=8):
    if a == b:
        return {"equal": True}
    if is_text(a) and is_text(b):
        r = text_diff(a, b, maxn)
        r["equal"] = False
        return r
    secs, syms, kind = None, [], "raw"
    pe = pe_sections(a)
    if pe is not None:
        secs, kind = pe, "pe"
    else:
        el = elf_sections(a)
        if el is not None:
            secs, syms = el
            kind = "elf"
    runs = runs_of(a, b)
    per = {}
    first = []
    for (s, e) in runs:
        sec = section_of(secs, s) if secs else None
        sname = sec[0] if sec else "(outside any section)"
        per[sname] = per.get(sname, 0) + sum(1 for k in range(s, e) if a[k] != b[k])
        if len(first) < maxn:
            item = {"offset": s, "length": e - s, "section": sname}
            if sec:
                addr = sec[3] + (s - sec[1])
                item["address"] = addr
                if syms:
                    sym = symbol_of(syms, addr)
                    if sym:
                        item["symbol"] = sym
            item["a"] = a[s:min(e, s + 16)].hex()
            item["b"] = b[s:min(e, s + 16)].hex()
            first.append(item)
    return {"equal": False, "kind": kind, "size_a": len(a), "size_b": len(b),
            "differing_bytes": sum(per.values()), "runs": len(runs),
            "per_section": per, "first": first}


def show(r):
    if r["equal"]:
        print("equal")
        return
    if r["kind"] == "text":
        print("text: %d and %d lines; first differing lines:" % (r["lines_a"], r["lines_b"]))
        for x in r["first"]:
            print("  line %d (in %s)" % (x["line"], x["function"] or "?"))
            print("    a: " + x["a"])
            print("    b: " + x["b"])
        return
    print("%s image: %d and %d bytes; %d bytes differ in the common part, in %d runs"
          % (r["kind"], r["size_a"], r["size_b"], r["differing_bytes"], r["runs"]))
    if r["size_a"] != r["size_b"]:
        print("  the lengths differ by %d bytes: everything after the first run may only be moved"
              % (r["size_b"] - r["size_a"]))
    for name, n in sorted(r["per_section"].items(), key=lambda kv: -kv[1]):
        print("  %-24s %d bytes" % (name, n))
    print("first runs:")
    for x in r["first"]:
        where = x["section"]
        if "address" in x:
            where += " @0x%x" % x["address"]
        if "symbol" in x:
            where += " (" + x["symbol"] + ")"
        print("  offset 0x%x, %d bytes, %s: %s -> %s" % (x["offset"], x["length"], where, x["a"], x["b"]))


def main(argv):
    args = [x for x in argv if not x.startswith("--")]
    maxn = 8
    if "--max" in argv:
        i = argv.index("--max")
        if i + 1 < len(argv):
            maxn = int(argv[i + 1])
            args = [x for x in args if x != argv[i + 1]]
    if len(args) != 2:
        print(__doc__.strip().splitlines()[2].strip())
        return 2
    r = locate(read(args[0]), read(args[1]), maxn)
    if "--json" in argv:
        print(json.dumps(r, indent=1))
    else:
        show(r)
    return 0 if r["equal"] else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
