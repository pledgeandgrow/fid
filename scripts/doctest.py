#!/usr/bin/env python3
# doctest.py — extract `/// Example:` blocks from .fd sources and run them.
#
# Convention: a `///` doc comment on a pub item may contain a `/// Example:`
# line; every following `///` line indented by at least two spaces is code.
# The block ends at the first `///` line that is not indented (or at `pub`).
# The `///` may itself be indented: examples on methods in `impl` blocks
# (`    /// Example:` then `    ///   let v = T::new();`) are read the same.
#
# Each example becomes a standalone program: `use <module>;` plus every `use`
# line in the snippet is hoisted to the top; the rest runs inside
# `fn main() -> i64`. A block whose marker reads `/// Example (native only):`
# is skipped on the interpreter and only built/run with --emit: then it is
# built natively (`fid emit --target <host>-linux` on Linux, `fid emit` on
# Windows, `fid ccompile` elsewhere; when `emit` fails, as for a runtime
# function its backend lacks, `fid ccompile`) and the binary is run. --native-only
# runs those examples alone (complaint-roadmap CU7's CI step).
#
# Usage:  python3 scripts/doctest.py [dir] [--fid <path>] [--emit]
#                                    [--native-only] [--emit-only]
#                                    [--target <t>] [-v]
# --target x86_64-linux (aarch64-linux, riscv64-linux): build for that
# target with `emit --target` (nat_ir) and run the binary there; from
# Windows in WSL (`wsl [qemu-<arch>] /mnt/c/...`).
# --emit-only: no C route when `emit` fails (the example fails), to see what
# the own backend alone builds; the summary lists the examples that needed
# the C route either way.
# Exit:   1 if any runnable example fails, else 0. Prints a coverage line.

import os
import platform
import re
import subprocess
import sys
import tempfile

def examples(path):
    mod = None
    out = []
    lines = open(path, encoding="utf-8", errors="replace").read().splitlines()
    i = 0
    while i < len(lines):
        m = re.match(r"\s*module\s+(\w+)", lines[i])
        if m:
            mod = m.group(1)
        m = re.match(r"\s*///\s*Example(\s*\(native only\))?\s*:\s*$", lines[i])
        if m:
            native = bool(m.group(1))
            code = []
            j = i + 1
            while j < len(lines):
                cm = re.match(r"\s*///(  +)(.*)$", lines[j])
                if cm:
                    code.append(cm.group(2))
                    j += 1
                elif re.match(r"\s*///\s*$", lines[j]):
                    break
                else:
                    break
            if code:
                out.append((mod or os.path.basename(path)[:-3], native, code))
            i = j
            continue
        i += 1
    return out

def build_prog(mod, code):
    uses, body = [], []
    for ln in code:
        (uses if re.match(r"\s*use\s+\w+\s*;", ln) else body).append(ln)
    head = "use %s;\n" % mod
    head += "\n".join(u.strip() for u in uses)
    if head and uses:
        head += "\n"
    if any(re.match(r"\s*fn\s+main\b", ln) for ln in body):
        return head + "\n" + "\n".join(body) + "\n"
    inner = "\n".join("    " + ln for ln in body)
    if not any("return" in ln for ln in body):
        inner += "\n    return 0;"
    return "%s\nfn main() -> i64 {\n%s\n}\n" % (head, inner)

def opt_value(name):
    """The value after option `name` on the command line, or None."""
    if name in sys.argv:
        i = sys.argv.index(name)
        if i + 1 < len(sys.argv):
            return sys.argv[i + 1]
    return None

# --target <t>: build the native-only examples for another target with
# `fid emit --target <t>` (the C route `fid ccompile --target <t>`) and run
# them there: a Linux binary on a Windows host in WSL, through qemu-user for
# aarch64/riscv64 (as scripts/cross-test.sh does)
TARGET = opt_value("--target")

def target_os():
    if TARGET:
        return "windows" if "windows" in TARGET else ("linux" if "linux" in TARGET else "other")
    return "windows" if sys.platform == "win32" else "host"

def run_cmd(exe):
    """How to run a built example."""
    if TARGET and target_os() == "linux" and sys.platform == "win32":
        p = os.path.abspath(exe).replace("\\", "/")
        p = re.sub(r"^([A-Za-z]):", lambda m: "/mnt/" + m.group(1).lower(), p)
        qemu = {"aarch64": ["qemu-aarch64"], "riscv64": ["qemu-riscv64"]}.get(TARGET.split("-")[0], [])
        return ["wsl"] + qemu + [p]
    if TARGET and target_os() == "linux":
        qemu = {"aarch64": ["qemu-aarch64"], "riscv64": ["qemu-riscv64"]}.get(TARGET.split("-")[0], [])
        if qemu and platform.machine().lower() not in ("aarch64", "arm64", "riscv64"):
            return qemu + [exe]
    return [exe]

def native_build_cmd(fid, prog, exe):
    """The command that builds `prog` natively for this host (or --target)."""
    if TARGET:
        return [fid, "emit", prog, "--target", TARGET, "-o", exe]
    if sys.platform.startswith("linux"):
        arch = {"x86_64": "x86_64", "amd64": "x86_64", "aarch64": "aarch64",
                "arm64": "aarch64", "riscv64": "riscv64"}.get(platform.machine().lower())
        if arch:
            return [fid, "emit", prog, "--target", arch + "-linux", "-o", exe]
    elif sys.platform == "win32":
        return [fid, "emit", prog, "-o", exe]
    return [fid, "ccompile", prog, "-o", exe]

def first_error(b):
    """The first line of a failed build that says what failed, not the
    compiler's [build]/[prof] progress."""
    msg = ((b.stderr or "") + "\n" + (b.stdout or "")).strip().splitlines()
    errs = [ln for ln in msg if not ln.startswith("[") and not ln.rstrip().endswith(":")
            and re.search(r"error|panic|fail|not available|undefined", ln, re.I)]
    return ((errs or msg) or ["no binary"])[0]

# How each native-only example was built: "emit" (the own backend), or
# "ccompile" when emit failed and the C route built it (the reason is kept:
# a runtime function the own backend lacks shows up here, not as a pass).
ROUTES = []

def run_native(fid, prog):
    """Build `prog` natively and run it: (ok, first line of what went wrong)."""
    exe = prog[:-3] + (".exe" if target_os() == "windows" else "")
    try:
        b = subprocess.run(native_build_cmd(fid, prog, exe), capture_output=True,
                           text=True, timeout=300)
    except subprocess.TimeoutExpired:
        return False, "build timeout"
    cmd = native_build_cmd(fid, prog, exe)
    route = cmd[1]
    if (b.returncode != 0 or not os.path.exists(exe)) and cmd[1] == "emit" and "--emit-only" not in sys.argv:
        emit_why = first_error(b)
        # the own backend lacks some runtime functions (p_mmap_*, sysconf):
        # take the C route, as `fid build` falls back to it (needs $CC)
        try:
            c = subprocess.run([fid, "ccompile", prog] + (["--target", TARGET] if TARGET else [])
                               + ["-o", exe], capture_output=True, text=True, timeout=300)
        except subprocess.TimeoutExpired:
            c = None
        if c is not None and c.returncode == 0 and os.path.exists(exe):
            b = c
            route = "ccompile (emit: %s)" % emit_why
    ROUTES.append((prog, route))
    if b.returncode != 0 or not os.path.exists(exe):
        msg = ((b.stderr or "") + "\n" + (b.stdout or "")).strip().splitlines()
        # the first error line, not the compiler's [build]/[prof] progress
        errs = [ln for ln in msg if not ln.startswith("[") and not ln.rstrip().endswith(":")
                and re.search(r"error|panic|fail|not available|undefined", ln, re.I)]
        return False, "build: " + ((errs or msg) or ["no binary"])[0]
    try:
        r = subprocess.run(run_cmd(exe), capture_output=True, text=True, timeout=60)
    except subprocess.TimeoutExpired:
        return False, "timeout"
    if r.returncode != 0:
        msg = (r.stderr or r.stdout).strip().splitlines()
        return False, "exit %d: %s" % (r.returncode, (msg or ["?"])[0])
    return True, ""

def main():
    root = "stdlib"
    fid = "release/fid.exe"
    emit = False
    verbose = "-v" in sys.argv
    native_only = "--native-only" in sys.argv
    args = [a for a in sys.argv[1:] if not a.startswith("-")]
    if TARGET:
        args = [a for a in args if a != TARGET]
    flags = [a for a in sys.argv[1:] if a.startswith("-")]
    if "--emit" in flags:
        emit = True
    if "--fid" in flags:
        i = sys.argv.index("--fid")
        fid = sys.argv[i + 1]
        args = [a for a in args if a != fid]
        if args:
            root = args[0]
    elif args:
        root = args[0]
    fid = os.path.abspath(fid)
    files = sorted(
        os.path.join(dp, f)
        for dp, _, fs in os.walk(root)
        for f in fs if f.endswith(".fd")
    )
    total_items = 0
    n_doc = n_run = n_fail = n_skip = 0
    fails = []
    tmp = tempfile.mkdtemp(prefix="fiddoctest")
    for path in files:
        src = open(path, encoding="utf-8", errors="replace").read()
        total_items += len(re.findall(r"^\s*pub\s+(?:fn|struct|const|enum)", src, re.M))
        for k, (mod, native, code) in enumerate(examples(path)):
            n_doc += 1
            if native and not emit:
                n_skip += 1
                continue
            if native_only and not native:
                continue
            prog = os.path.join(tmp, "%s_%d.fd" % (mod, k))
            open(prog, "w", encoding="utf-8").write(build_prog(mod, code))
            if native:
                ok, why = run_native(fid, prog)
                n_run += 1
                if not ok:
                    n_fail += 1
                    fails.append("%s:%d  %s" % (path, k, why))
                elif verbose:
                    print("ok  %s #%d (native, %s)" % (path, k, ROUTES[-1][1] if ROUTES else "?"))
                continue
            cmd = [fid, "run", prog]
            try:
                r = subprocess.run(cmd, capture_output=True, text=True, timeout=60)
                ok = r.returncode == 0
            except subprocess.TimeoutExpired:
                ok = False
                r = None
            n_run += 1
            if not ok:
                n_fail += 1
                msg = ((r.stderr or "") + "\n" + (r.stdout or "")).strip().splitlines() if r else ["timeout"]
                # the first line that says what failed, not [build] progress
                errs = [ln for ln in msg if not ln.startswith("[")
                        and re.search(r"error|panic|assert|fail|undefined", ln, re.I)]
                fails.append("%s:%d  %s" % (path, k, ((errs or msg) or ["?"])[0]))
            elif verbose:
                print("ok  %s #%d" % (path, k))
    print("doctest: %d pub items | %d examples | %d run | %d failed | %d skipped(native)"
          % (total_items, n_doc, n_run, n_fail, n_skip))
    for f in fails:
        print("  FAIL " + f)
    if ROUTES:
        by_c = [r for r in ROUTES if r[1].startswith("ccompile (emit")]
        print("native: %d built by %s, %d by the C route after emit failed"
              % (len(ROUTES) - len(by_c), native_build_cmd(fid, "x.fd", "x")[1], len(by_c)))
        for prog, route in by_c:
            print("  C ROUTE %s  %s" % (os.path.basename(prog), route))
    return 1 if n_fail else 0

if __name__ == "__main__":
    sys.exit(main())
