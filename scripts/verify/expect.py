#!/usr/bin/env python3
"""Run Fides test files on every backend and check what they must do.

    python scripts/verify/expect.py [--fid release/fid.exe] [--backends run,ccompile,emit]
                                    [-j 3] <file.fd | dir> ...

The runner of the conformance suite (tests/conformance/, roadmap VF10) and
of the optimization tests (tests/bootstrap/test_opt_*.fd, VF5).

What a file must do comes from comments in it:

    //> text          the next line of standard output is `text`
    //! panic         the program panics (exit status 1) after the //> lines
    //! error: words  the program is rejected at compile time with a message
                      containing `words` (checked once, with `fid check`)
    //! exit: N       the program exits with status N
    //@ backends: run,ccompile    only these backends (a feature some lack)

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
    want = {"out": None, "panic": False, "error": None, "exit": None, "backends": None}
    for line in text.split("\n"):
        s = line.strip()
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
    if out or want["panic"] or want["exit"] is not None:
        want["out"] = "".join(o + "\n" for o in out)
    if want["out"] is None and want["error"] is None:
        g = os.path.join("tests", "bootstrap", "golden", os.path.basename(path)[:-3] + ".txt")
        if os.path.exists(g):
            with open(g, encoding="utf-8") as f:
                want["out"] = f.read().replace("\r\n", "\n")
    return want


def run(cmd, timeout=300):
    try:
        p = subprocess.run(cmd, capture_output=True, timeout=timeout)
        return p.returncode, p.stdout.decode("utf-8", "replace").replace("\r\n", "\n"), \
            p.stderr.decode("utf-8", "replace")
    except subprocess.TimeoutExpired:
        return "timeout", "", ""


def on_backend(fid, b, path, work):
    if b == "run":
        return run([fid, "run", path])
    exe = os.path.join(work, os.path.basename(path)[:-3] + f".{b}.exe")
    rc, out, err = run([fid, b, path, "-o", exe])
    if rc != 0 or not os.path.exists(exe):
        # once more on its own: under load the C compiler's shared cache
        # fails now and then (zig: "file_open Unexpected")
        with RETRY_LOCK:
            rc, out, err = run([fid, b, path, "-o", exe])
    if rc != 0 or not os.path.exists(exe):
        return "compile-error", "", out + err
    res = run([exe], 120)
    for e in (exe, exe[:-4] + ".pdb"):
        if os.path.exists(e):
            try:
                os.remove(e)
            except OSError:
                pass
    return res


def check_file(fid, path, backends, work):
    want = spec_of(path)
    problems = []
    if want["error"] is not None:
        rc, out, err = run([fid, "check", path])
        msg = out + err
        if rc == 0:
            problems.append("accepted, but must be rejected")
        elif want["error"] not in msg:
            problems.append(f"rejected with another message: {error_line(msg)}")
        return path, problems, {}
    bs = [b for b in backends if not want["backends"] or b in want["backends"]]
    results = {b: on_backend(fid, b, path, work) for b in bs}
    for b, (rc, out, err) in results.items():
        if rc == "compile-error" or (b == "run" and isinstance(rc, int) and rc != 0 and "rror" in err and not out):
            if not path.endswith("_fail.fd"):
                problems.append(f"{b}: does not compile: {error_line(err)}")
            continue
        if want["out"] is not None:
            want_rc = 1 if want["panic"] else (want["exit"] if want["exit"] is not None else 0)
            if rc != want_rc:
                problems.append(f"{b}: exit {rc}, expected {want_rc}")
            if out != want["out"]:
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
    os.environ.setdefault("CC", "zig cc")
    os.environ.setdefault("FIDES_NO_CACHE", "1")
    files = []
    for p in a.paths:
        if os.path.isdir(p):
            files += sorted(glob.glob(os.path.join(p, "**", "*.fd"), recursive=True))
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
