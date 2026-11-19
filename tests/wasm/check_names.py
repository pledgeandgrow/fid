#!/usr/bin/env python3
"""DB10: a `fid wasm` module carries a "name" custom section naming its
functions (wasm-objdump -x / wasm-tools print them; Node's traces use them).

    python3 tests/wasm/check_names.py [--fid release/fid.exe] [file.fd]

Builds the program with `fid wasm`, walks the module's sections, decodes the
function-names subsection and checks that `_start` and `main` are named and
that every index is below the number of functions (imports + bodies). With
FID_NAT_OFF=debug the section must be absent. Exit status 0 when it holds.
"""
import os
import subprocess
import sys
import tempfile


def uleb(b, i):
    v = 0
    shift = 0
    while True:
        x = b[i]
        i += 1
        v |= (x & 0x7F) << shift
        shift += 7
        if x & 0x80 == 0:
            return v, i


def name(b, i):
    n, i = uleb(b, i)
    return b[i:i + n].decode("utf-8"), i + n


def sections(b):
    assert b[:4] == b"\0asm", "not a wasm module"
    i = 8
    while i < len(b):
        sid = b[i]
        size, j = uleb(b, i + 1)
        yield sid, b[j:j + size]
        i = j + size


def count_vec(body):
    n, _ = uleb(body, 0)
    return n


def build(fid, src, out, env):
    r = subprocess.run([fid, "wasm", src, "-o", out], env=env,
                       capture_output=True, text=True)
    if r.returncode != 0 or not os.path.exists(out):
        sys.exit("fid wasm failed: " + r.stderr)
    return open(out, "rb").read()


def main():
    fid = "release/fid.exe"
    src = "tests/bootstrap/hello.fd"
    args = sys.argv[1:]
    while args:
        a = args.pop(0)
        if a == "--fid":
            fid = args.pop(0)
        else:
            src = a
    tmp = tempfile.mkdtemp()
    env = dict(os.environ)
    env.pop("FID_NAT_OFF", None)
    mod = build(fid, src, os.path.join(tmp, "a.wasm"), env)
    nimports = nfuncs = 0
    names = None
    for sid, body in sections(mod):
        if sid == 2:
            nimports = count_vec(body)
        elif sid == 3:
            nfuncs = count_vec(body)
        elif sid == 0:
            sname, k = name(body, 0)
            if sname != "name":
                continue
            names = {}
            while k < len(body):
                sub = body[k]
                size, k = uleb(body, k + 1)
                end = k + size
                if sub == 1:
                    cnt, k = uleb(body, k)
                    for _ in range(cnt):
                        idx, k = uleb(body, k)
                        fname, k = name(body, k)
                        names[idx] = fname
                k = end
    fails = []
    if names is None:
        fails.append("no name section")
    else:
        total = nimports + nfuncs
        if len(names) != total:
            fails.append("%d names for %d functions" % (len(names), total))
        if any(i >= total for i in names):
            fails.append("a name for an index past the functions")
        if names.get(nimports) != "_start":
            fails.append("function %d is not _start" % nimports)
        if "main" not in names.values():
            fails.append("main is not named")
    env["FID_NAT_OFF"] = "debug"
    mod2 = build(fid, src, os.path.join(tmp, "b.wasm"), env)
    for sid, body in sections(mod2):
        if sid == 0 and name(body, 0)[0] == "name":
            fails.append("FID_NAT_OFF=debug still writes the name section")
    if fails:
        print("FAIL: " + "; ".join(fails))
        return 1
    print("ok: %d functions named (%d imports)" % (len(names), nimports))
    return 0


if __name__ == "__main__":
    sys.exit(main())
