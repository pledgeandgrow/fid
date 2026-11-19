#!/usr/bin/env python3
"""guide-check.py -- release-roadmap N2: follow the getting-started guide
(docs/reference/quickstart.md) step by step, as a new user would, in a
fresh directory, with the compiler under test.

The guide's own text is the test, so a guide that drifts from the compiler
fails CI:
  * a ```fides block is a program, saved under the name the nearest
    "Create `<name>.fd`" above it gives (else guide_<n>.fd);
  * a ```bash block is a step when every line of it is a `fides`/`fid`
    command or runs a program the guide built (`./<name>`): its lines run in
    order, `fides` and `fid` meaning the compiler under test. A block with
    any other line (git clone, cargo, scripts/bootstrap.sh) builds from
    source and is left to the release jobs (.github/workflows/release.yml);
  * a fenced block right after a line "Output:" is what the step before it
    must print: the last lines of its standard output, CRs dropped;
  * `fid emit` writes a native Windows executable: on another host a failing
    emit is reported as skipped (the guide's section says "Windows").

Usage:
  scripts/guide-check.py [--fid <fid>] [--guide <file.md>] [--keep]
  (default fid: release/fid-dev, release/fid.exe, release/fid)
Exit status: 0 when every step ran and printed what the guide says.
"""
import os
import re
import shlex
import shutil
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def find_fid():
    for name in ("release/fid-dev", "release/fid-dev.exe", "release/fid.exe", "release/fid"):
        p = os.path.join(ROOT, name)
        if os.path.isfile(p):
            return p
    return None


def parse(guide):
    """The guide as a list of items in order:
    ("program", name, text), ("step", [lines]), ("output", text)."""
    items = []
    lines = open(guide, encoding="utf-8").read().replace("\r", "").split("\n")
    create = None
    after_output = False
    n_prog = 0
    i = 0
    while i < len(lines):
        line = lines[i]
        m = re.search(r"Create `([^`]+\.fd)`", line)
        if m:
            create = m.group(1)
        fence = re.match(r"^\s*```\s*([A-Za-z0-9_+-]*)\s*$", line)
        if not fence:
            if line.strip():
                after_output = line.strip() == "Output:"
            i += 1
            continue
        lang = fence.group(1).lower()
        body = []
        i += 1
        while i < len(lines) and not re.match(r"^\s*```\s*$", lines[i]):
            body.append(lines[i])
            i += 1
        i += 1  # the closing fence
        if after_output:
            items.append(("output", "\n".join(body).strip("\n")))
        elif lang == "fides":
            n_prog += 1
            name = create or "guide_%d.fd" % n_prog
            items.append(("program", name, "\n".join(body) + "\n"))
            create = None
        elif lang in ("bash", "sh", "shell", "console"):
            cmds = [l.strip() for l in body if l.strip() and not l.strip().startswith("#")]
            if cmds and all(is_step(c) for c in cmds):
                items.append(("step", cmds))
        after_output = False
    return items


def is_step(cmd):
    try:
        words = shlex.split(cmd)
    except ValueError:
        return False
    if not words:
        return False
    w = words[0]
    if w in ("fides", "fid"):
        return True
    # ./name: a program the guide built (not ./scripts/... or ./release/...)
    return w.startswith("./") and "/" not in w[2:]


def run_step(cmd, fid, work):
    words = shlex.split(cmd)
    if words[0] in ("fides", "fid"):
        argv = [fid] + words[1:]
    else:
        prog = os.path.join(work, words[0][2:])
        if not os.path.exists(prog) and os.path.exists(prog + ".exe"):
            prog += ".exe"
        argv = [prog] + words[1:]
    try:
        p = subprocess.run(argv, cwd=work, capture_output=True, timeout=600)
    except (OSError, subprocess.TimeoutExpired) as e:
        return 127, "", str(e)
    out = p.stdout.decode("utf-8", "replace").replace("\r", "")
    err = p.stderr.decode("utf-8", "replace").replace("\r", "")
    return p.returncode, out, err


def main():
    args = sys.argv[1:]
    fid = None
    guide = os.path.join(ROOT, "docs", "reference", "quickstart.md")
    keep = False
    while args:
        a = args.pop(0)
        if a == "--fid" and args:
            fid = args.pop(0)
        elif a == "--guide" and args:
            guide = args.pop(0)
        elif a == "--keep":
            keep = True
        else:
            print(__doc__)
            return 2
    fid = fid or find_fid()
    if not fid or not (os.path.isfile(fid) or os.path.isfile(fid + ".exe")):
        print("guide-check: no compiler (--fid <path>)")
        return 2
    fid = os.path.abspath(fid)
    if not os.path.isfile(fid) and os.path.isfile(fid + ".exe"):
        fid += ".exe"
    items = parse(guide)
    work = tempfile.mkdtemp(prefix="guide-check-")
    windows = os.name == "nt" or sys.platform.startswith(("msys", "cygwin"))
    fails = 0
    steps = 0
    last = None  # (cmd, stdout) of the last step, for an Output: block
    print("guide-check: %s with %s (in %s)" % (os.path.relpath(guide, ROOT), fid, work))
    for it in items:
        if it[0] == "program":
            with open(os.path.join(work, it[1]), "w", encoding="utf-8", newline="\n") as f:
                f.write(it[2])
            print("  file  %s" % it[1])
            continue
        if it[0] == "output":
            if last is None:
                print("  FAIL  an Output: block with no step before it")
                fails += 1
                continue
            want = [l.rstrip() for l in it[1].split("\n")]
            got = [l.rstrip() for l in last[1].rstrip("\n").split("\n")]
            if got[-len(want):] == want:
                print("  ok    output of '%s'" % last[0])
            else:
                print("  FAIL  '%s' printed:" % last[0])
                for l in got[-10:]:
                    print("          | " + l)
                print("        the guide says:")
                for l in want:
                    print("          | " + l)
                fails += 1
            last = None
            continue
        for cmd in it[1]:
            steps += 1
            code, out, err = run_step(cmd, fid, work)
            words = shlex.split(cmd)
            if code != 0 and len(words) > 1 and words[1] == "emit" and not windows:
                print("  skip  %s (native Windows executable; this host is not Windows)" % cmd)
                last = None
                continue
            if code == 0:
                print("  ok    %s" % cmd)
            else:
                print("  FAIL  %s (exit %d)" % (cmd, code))
                for l in (err or out).strip("\n").split("\n")[-10:]:
                    print("          | " + l)
                fails += 1
            last = (cmd, out)
    if steps == 0:
        print("  FAIL  the guide has no step this check can follow")
        fails += 1
    if keep:
        print("guide-check: kept %s" % work)
    else:
        shutil.rmtree(work, ignore_errors=True)
    print("guide-check: %s (%d steps)" % ("PASS" if fails == 0 else "FAIL", steps))
    return 0 if fails == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
