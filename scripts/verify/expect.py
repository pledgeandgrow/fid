#!/usr/bin/env python3
"""Run Fides test files on every backend and check what they must do.

    python scripts/verify/expect.py [--fid release/fid.exe] [--backends run,ccompile,emit]
                                    [-j 3] <file.fd | dir> ...

The runner of the conformance suite (tests/conformance/, roadmap VF10) and
of the optimization tests (tests/bootstrap/test_opt_*.fd, VF5).

Backends: run, ccompile, emit, checked (below), nat (`emit --nat`: the
machine layer on Windows) and the Linux targets x86_64-linux,
aarch64-linux, riscv64-linux (`emit --target`; run in WSL on Windows, the
last two under qemu-user).

What a file must do comes from comments in it:

    //> text          the next line of standard output is `text`
    //! panic         the program panics (exit status 1) after the //> lines
    //! error: words  the program is rejected at compile time with a message
                      containing `words` (checked once, with `fid check`)
    //! exit: N       the program exits with status N
    //@ backends: run,ccompile    only these backends (a feature some lack);
                      `checked` (the C path under --checked-alloc) is run
                      only for a file that names it here, always then
    //@ env: NAME=value   set for every compile and run of the file (a pass
                          switch, docs/verification/kill-switches.md); one per line

The suite's own form is also read (tests/bootstrap/, scripts/bare-qemu-test.sh):

    // Expected output:
    // line one          each comment line after it is a line of output, up to
    // line two          the first line that is not a comment; a line ending
                         in `...` matches any line that starts with the rest
    // Expected exit: N  the exit status (default 0)

A file with no //> or //! lines is checked against
tests/bootstrap/golden/<name>.txt when that exists, and otherwise only for
agreement between the backends. A `_fail.fd` file with no //! line must
fail (a compile error or a nonzero exit) on every backend.

Exit status 0 when every file does what it says on every backend.
"""
import argparse
import glob
import os
import re
import subprocess
import sys
import tempfile
import threading
from concurrent.futures import ThreadPoolExecutor


RETRY_LOCK = threading.Lock()


def error_line(text):
    lines = [l.strip() for l in text.split("\n") if l.strip() and not l.startswith(("[", "  -->", "   |"))]
    pick = next((l for l in lines if "rror" in l), lines[-1] if lines else "")
    return pick[:160]


def spec_of(path):
    with open(path, encoding="utf-8", errors="replace") as f:
        text = f.read()
    out = []
    want = {"out": None, "panic": False, "error": None, "exit": None, "backends": None, "env": {}}
    block = None  # lines of a "// Expected output:" block, while in it
    blocks = []
    for line in text.split("\n"):
        s = line.strip()
        if block is not None:
            if s.startswith("// Expected exit:"):
                want["exit"] = int(s[len("// Expected exit:"):].strip().split()[0])
                continue
            if s.startswith("//"):
                block.append(s[3:] if s.startswith("// ") else s[2:])
                continue
            blocks.append(block)
            block = None
        if s == "// Expected output:":
            block = []
            continue
        if s.startswith("// Expected exit:"):
            want["exit"] = int(s[len("// Expected exit:"):].strip().split()[0])
            continue
        if s.startswith("//@ env:"):
            k, _, v = s[len("//@ env:"):].strip().partition("=")
            want["env"][k.strip()] = v.strip()
            continue
        if s.startswith("//>"):
            out.append(s[3:][1:] if s[3:].startswith(" ") else s[3:])
        elif s.startswith("//! panic"):
            want["panic"] = True
        elif s.startswith("//! error:"):
            want["error"] = s[len("//! error:"):].strip()
        elif s.startswith("//! exit:"):
            want["exit"] = int(s[len("//! exit:"):].strip())
        elif s.startswith("//@ backends:"):
            want["backends"] = [b.strip() for b in s[len("//@ backends:"):].split(",")]
    if block is not None:
        blocks.append(block)
    if not out and blocks:
        out = blocks[0]
    if out or want["panic"] or want["exit"] is not None:
        want["out"] = "".join(o + "\n" for o in out)
    if want["out"] is None and want["error"] is None:
        g = os.path.join("tests", "bootstrap", "golden", os.path.basename(path)[:-3] + ".txt")
        if os.path.exists(g):
            with open(g, encoding="utf-8") as f:
                want["out"] = f.read().replace("\r\n", "\n")
    return want


def run(cmd, timeout=300, env=None):
    try:
        p = subprocess.run(cmd, capture_output=True, timeout=timeout,
                           env=dict(os.environ, **env) if env else None)
        return p.returncode, p.stdout.decode("utf-8", "replace").replace("\r\n", "\n"), \
            p.stderr.decode("utf-8", "replace")
    except subprocess.TimeoutExpired:
        return "timeout", "", ""


LINUX_RUNNERS = {"x86_64-linux": [], "aarch64-linux": ["qemu-aarch64"],
                 "riscv64-linux": ["qemu-riscv64"]}


def backend_cmd(fid, b, path, exe):
    """The compile command of a backend and what runs its executable.

    run, ccompile, emit: the commands of those names. nat: the machine layer
    on the host (`emit --nat`, compiler/nat_ir.fd; Windows only). A Linux
    target name (x86_64-linux, aarch64-linux, riscv64-linux): `emit --target`
    to it (the machine layer), run on Linux directly or under qemu-user, on
    Windows in WSL (WSL_DISTRO picks the distribution)."""
    if b == "nat":
        return [fid, "emit", path, "--nat", "-o", exe], []
    if b in LINUX_RUNNERS:
        r = list(LINUX_RUNNERS[b])
        if sys.platform.startswith("linux") and b == "x86_64-linux" and os.uname().machine == "x86_64":
            r = []
        if os.name == "nt":
            r = ["wsl"] + (["-d", os.environ["WSL_DISTRO"]] if os.environ.get("WSL_DISTRO") else []) + ["--"] + r
        return [fid, "emit", path, "--target", b, "-o", exe], r
    return [fid, b, path, "-o", exe], []


def linux_path(p):
    """C:\\x\\y -> /mnt/c/x/y for a program run in WSL; unchanged elsewhere."""
    if os.name != "nt":
        return p
    p = os.path.abspath(p).replace("\\", "/")
    if len(p) > 1 and p[1] == ":":
        p = "/mnt/" + p[0].lower() + p[2:]
    return p


def on_backend(fid, b, path, work, env=None):
    if b == "run":
        return run([fid, "run", path], env=env)
    if b == "checked":
        # the C path under the checked allocator (`fid run --checked-alloc`,
        # release M7; scripts/fuzz/gen_programs.py's backend): each block on
        # pages of its own, freed pages made inaccessible. It builds
        # _fides_memcheck_tmp.exe in the current directory: run it in a
        # directory of its own and drop the line naming the executable
        cw = os.path.join(work, "checked-" + os.path.basename(path)[:-3])
        os.makedirs(cw, exist_ok=True)
        try:
            p = subprocess.run([fid, "run", os.path.abspath(path), "--checked-alloc"],
                               capture_output=True, timeout=600, cwd=cw,
                               env=dict(os.environ, **env) if env else None)
        except subprocess.TimeoutExpired:
            return "timeout", "", ""
        out = p.stdout.decode("utf-8", "replace").replace("\r\n", "\n")
        out = "".join(l for l in out.splitlines(True) if not l.startswith("Built native executable:"))
        return p.returncode, out, p.stderr.decode("utf-8", "replace")
    exe = os.path.join(work, os.path.basename(path)[:-3] + f".{b}.exe")
    cmd, runner = backend_cmd(fid, b, path, exe)
    rc, out, err = run(cmd, env=env)
    if rc != 0 or not os.path.exists(exe):
        # once more on its own: under load the C compiler's shared cache
        # fails now and then (zig: "file_open Unexpected")
        with RETRY_LOCK:
            rc, out, err = run(cmd, env=env)
    if rc != 0 or not os.path.exists(exe):
        return "compile-error", "", out + err
    res = run(runner + [linux_path(exe) if runner else exe], 300 if runner else 120, env=env)
    for e in (exe, exe[:-4] + ".pdb"):
        if os.path.exists(e):
            try:
                os.remove(e)
            except OSError:
                pass
    return res


def same_output(want, got):
    """Line by line; an expected line ending in `...` matches by prefix."""
    if want == got:
        return True
    wl, gl = want.split("\n"), got.split("\n")
    if len(wl) != len(gl):
        return False
    return all(w == g or (w.endswith("...") and g.startswith(w[:-3])) for w, g in zip(wl, gl))


def check_file(fid, path, backends, work):
    want = spec_of(path)
    problems = []
    if want["error"] is not None:
        rc, out, err = run([fid, "check", path], env=want["env"])
        msg = out + err
        if rc == 0:
            problems.append("accepted, but must be rejected")
        elif want["error"] not in msg:
            problems.append(f"rejected with another message: {error_line(msg)}")
        return path, problems, {}
    bs = [b for b in backends if not want["backends"] or b in want["backends"]]
    # `checked` is opt-in: a file that names it in //@ backends: is run on it
    # whatever --backends says (a test of the checked allocator, VF1 M25)
    if want["backends"] and "checked" in want["backends"] and "checked" not in bs:
        bs.append("checked")
    results = {b: on_backend(fid, b, path, work, want["env"]) for b in bs}
    for b, (rc, out, err) in results.items():
        if rc == "compile-error" or (b == "run" and isinstance(rc, int) and rc != 0 and "rror" in err and not out):
            if not path.endswith("_fail.fd"):
                problems.append(f"{b}: does not compile: {error_line(err)}")
            continue
        if want["out"] is not None:
            want_rc = 1 if want["panic"] else (want["exit"] if want["exit"] is not None else 0)
            if rc != want_rc:
                problems.append(f"{b}: exit {rc}, expected {want_rc}")
            if not same_output(want["out"], out):
                wl, gl = want["out"].split("\n"), out.split("\n")
                k = next((i for i in range(max(len(wl), len(gl)))
                          if i >= len(wl) or i >= len(gl) or wl[i] != gl[i]), 0)
                problems.append(f"{b}: output line {k + 1}: expected {wl[k] if k < len(wl) else '<end>'!r}, "
                                f"got {gl[k] if k < len(gl) else '<end>'!r}")
        elif path.endswith("_fail.fd") and rc == 0:
            problems.append(f"{b}: succeeded, but must fail")
    if want["out"] is None and not path.endswith("_fail.fd"):
        outs = {(rc, out) for rc, out, _ in results.values() if rc != "compile-error"}
        if len(outs) > 1:
            problems.append("backends disagree: " + "; ".join(
                f"{b}: exit {rc}, {len(out)} bytes" for b, (rc, out, _) in results.items()))
    return path, problems, results


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("paths", nargs="+")
    ap.add_argument("--fid", default=os.path.join("release", "fid.exe"))
    ap.add_argument("--backends", default="run,ccompile,emit")
    ap.add_argument("-j", type=int, default=3)
    ap.add_argument("-q", action="store_true", help="print failures only")
    a = ap.parse_args()
    # a panic message or a test's text may hold characters the console's
    # code page lacks (cp1252 on Windows): print them replaced, not crash
    try:
        sys.stdout.reconfigure(errors="replace")
    except (AttributeError, ValueError):
        pass
    os.environ.setdefault("CC", "zig cc")
    os.environ.setdefault("FIDES_NO_CACHE", "1")
    files = []
    for p in a.paths:
        if os.path.isdir(p):
            # a directory named `_modules` holds modules the tests `use`
            # (tests/conformance/_modules/, reference §2.9): not programs
            files += sorted(f for f in glob.glob(os.path.join(p, "**", "*.fd"), recursive=True)
                            if "_modules" not in f.replace("\\", "/").split("/"))
        else:
            files += sorted(glob.glob(p))
    work = tempfile.mkdtemp(prefix="expect")
    backends = a.backends.split(",")
    bad = 0
    with ThreadPoolExecutor(a.j) as ex:
        for path, problems, _ in ex.map(lambda f: check_file(a.fid, f, backends, work), files):
            if problems:
                bad += 1
                print(f"FAIL {path}")
                for pr in problems:
                    print(f"     {pr}")
            elif not a.q:
                print(f"ok   {path}")
            sys.stdout.flush()
    print(f"{len(files) - bad} of {len(files)} files pass on {a.backends}")
    sys.exit(1 if bad else 0)


if __name__ == "__main__":
    main()
