#!/usr/bin/env python3
# bin-target-check-selftest.py - scripts/bin-target-check.py on ELF files
# made here (PL2; no compiler is built or run):
#   1. a static executable passes, --loader says "-" (no loader)
#   2. a dynamic one that imports a C function (cos from libc) passes,
#      --loader names its interpreter and no runtime import
#   3. a dynamic one that imports nat_tls_set (the runtime primitive
#      compiler/nat_ir.fd's CI2 path bound through the dynamic linker; the
#      loader then stops with "symbol lookup error") FAILS, and --loader
#      lists the import
#   4. file names on stdin ("-") are read as on the command line
# Also: --make <out> <static|c|runtime> [aarch64|x86_64] writes one such
# file (tests/tooling/pl2-lane-skips-check.sh uses it).
# Exit 0 when every check holds.

import os, struct, subprocess, sys, tempfile

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
CHECK = os.path.join(ROOT, "scripts", "bin-target-check.py")
MACHINE = {"aarch64": 183, "x86_64": 62}
INTERP = {"aarch64": "/lib/ld-linux-aarch64.so.1", "x86_64": "/lib64/ld-linux-x86-64.so.2"}


def make_elf(kind, arch="aarch64"):
    """bytes of a small ELF64 executable: static, or dynamic importing `cos`
    (kind "c") or `nat_tls_set` (kind "runtime") from libc.so.6"""
    base = 0x400000
    dyn = kind != "static"
    nph = 3 if dyn else 1
    body = bytearray()
    off0 = 64 + 56 * nph

    def put(b, align=8):
        while (off0 + len(body)) % align:
            body.append(0)
        o = off0 + len(body)
        body.extend(b)
        return o

    code = put(b"\x00" * 16)
    if dyn:
        interp = put(INTERP[arch].encode() + b"\0", 1)
        imp = "cos" if kind == "c" else "nat_tls_set"
        strtab = b"\0libc.so.6\0" + imp.encode() + b"\0"
        s_off = put(strtab, 1)
        sym = bytes(24) + struct.pack("<IBBHQQ", 11, 0x12, 0, 0, 0, 0)
        sym_off = put(sym)
        hash_off = put(struct.pack("<IIIII", 1, 2, 1, 0, 0))
        dyns = [(1, 1), (4, base + hash_off), (5, base + s_off), (6, base + sym_off),
                (10, len(strtab)), (11, 24), (0, 0)]
        dyn_off = put(b"".join(struct.pack("<qQ", t, v) for t, v in dyns))
    size = off0 + len(body)
    ph = struct.pack("<IIQQQQQQ", 1, 5, 0, base, base, size, size, 0x1000)
    if dyn:
        ilen = len(INTERP[arch]) + 1
        ph += struct.pack("<IIQQQQQQ", 3, 4, interp, base + interp, base + interp, ilen, ilen, 1)
        ph += struct.pack("<IIQQQQQQ", 2, 6, dyn_off, base + dyn_off, base + dyn_off, 7 * 16, 7 * 16, 8)
    eh = b"\x7fELF" + bytes([2, 1, 1, 0]) + bytes(8)
    eh += struct.pack("<HHIQQQIHHHHHH", 2, MACHINE[arch], 1, base + code, 64, 0, 0, 64, 56, nph, 64, 0, 0)
    return eh + ph + bytes(body)


def run(args, stdin=None):
    p = subprocess.run([sys.executable, CHECK] + args, input=stdin, capture_output=True, text=True)
    return p.returncode, p.stdout.replace("\r", "")


def main():
    if len(sys.argv) >= 4 and sys.argv[1] == "--make":
        arch = sys.argv[4] if len(sys.argv) > 4 else "aarch64"
        with open(sys.argv[2], "wb") as f:
            f.write(make_elf(sys.argv[3], arch))
        return 0
    fails = 0

    def ok(cond, what):
        nonlocal fails
        print("  %s  %s" % ("ok  " if cond else "FAIL", what))
        if not cond:
            fails += 1

    with tempfile.TemporaryDirectory() as d:
        paths = {}
        for k in ("static", "c", "runtime"):
            paths[k] = os.path.join(d, k + ".bin")
            with open(paths[k], "wb") as f:
                f.write(make_elf(k))
        rc, out = run(["aarch64-linux", paths["static"]])
        ok(rc == 0, "1. static executable passes")
        rc, out = run(["--loader", paths["static"]])
        ok(out.strip().split("\t")[1:] == ["-", "-"], "1. --loader: no interpreter, no runtime import")
        rc, out = run(["aarch64-linux", paths["c"]])
        ok(rc == 0, "2. dynamic executable importing cos passes")
        rc, out = run(["--loader", paths["c"]])
        ok(out.strip().split("\t")[1:] == ["/lib/ld-linux-aarch64.so.1", "-"],
           "2. --loader: its interpreter, no runtime import (got %r)" % out.strip())
        rc, out = run(["aarch64-linux", paths["runtime"]])
        ok(rc == 1 and "nat_tls_set" in out and "symbol lookup error" in out,
           "3. importing nat_tls_set fails (got %r)" % out.strip().splitlines()[:1])
        rc, out = run(["--loader", paths["runtime"]])
        ok(out.strip().endswith("\tnat_tls_set"), "3. --loader lists nat_tls_set")
        rc, out = run(["aarch64-linux", "-", "--quiet"], stdin=paths["static"] + "\n" + paths["c"] + "\n")
        ok(rc == 0 and "2 files, 0 bad" in out, "4. names on stdin: 2 files, 0 bad (got %r)" % out.strip())
        rc, out = run(["aarch64-linux", "-", "--quiet"], stdin=paths["runtime"] + "\n")
        ok(rc == 1 and "1 files, 1 bad" in out, "4. names on stdin: the bad one fails")
    print("bin-target-check-selftest: %s" % ("PASS" if fails == 0 else "FAIL"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
