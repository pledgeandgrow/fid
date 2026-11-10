#!/usr/bin/env python3
# doctest.py — extract `/// Example:` blocks from .fd sources and run them.
#
# Convention: a `///` doc comment on a pub item may contain a `/// Example:`
# line; every following `///` line indented by at least two spaces is code.
# The block ends at the first `///` line that is not indented (or at `pub`).
#
# Each example becomes a standalone program: `use <module>;` plus every `use`
# line in the snippet is hoisted to the top; the rest runs inside
# `fn main() -> i64`. A block whose marker reads `/// Example (native only):`
# is skipped on the interpreter and only built/run with --emit.
#
# Usage:  python3 scripts/doctest.py [dir] [--fid <path>] [--emit] [-v]
# Exit:   1 if any runnable example fails, else 0. Prints a coverage line.

import os
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
        m = re.match(r"///\s*Example(\s*\(native only\))?\s*:\s*$", lines[i])
        if m:
            native = bool(m.group(1))
            code = []
            j = i + 1
            while j < len(lines):
                cm = re.match(r"///(  +)(.*)$", lines[j])
                if cm:
                    code.append(cm.group(2))
                    j += 1
                elif re.match(r"///\s*$", lines[j]):
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

def main():
    root = "stdlib"
    fid = "release/fid.exe"
    emit = False
    verbose = "-v" in sys.argv
    args = [a for a in sys.argv[1:] if not a.startswith("-")]
    flags = [a for a in sys.argv[1:] if a.startswith("-")]
    if "--emit" in flags:
        emit = True
    if "--fid" in flags:
        fid = args[flags.index("--fid")]
    elif args:
        root = args[0]
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
            prog = os.path.join(tmp, "%s_%d.fd" % (mod, k))
            open(prog, "w", encoding="utf-8").write(build_prog(mod, code))
            cmd = [fid, "run", prog] if not emit else [fid, "run", prog]
            try:
                r = subprocess.run(cmd, capture_output=True, text=True, timeout=60)
                ok = r.returncode == 0
            except subprocess.TimeoutExpired:
                ok = False
                r = None
            n_run += 1
            if not ok:
                n_fail += 1
                msg = (r.stderr or r.stdout).strip().splitlines() if r else ["timeout"]
                fails.append("%s:%d  %s" % (path, k, (msg or ["?"])[0]))
            elif verbose:
                print("ok  %s #%d" % (path, k))
    print("doctest: %d pub items | %d examples | %d run | %d failed | %d skipped(native)"
          % (total_items, n_doc, n_run, n_fail, n_skip))
    for f in fails:
        print("  FAIL " + f)
    return 1 if n_fail else 0

if __name__ == "__main__":
    sys.exit(main())
