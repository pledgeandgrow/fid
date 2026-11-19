#!/usr/bin/env python3
"""The conformance suite against its list of known gaps.

    python scripts/verify/conf-known.py [--known tests/conformance/known-gaps.txt]
                                        [--strict] --log <expect.py output>
    python scripts/verify/conf-known.py [--known ...] [--strict] -- <expect.py arguments>

tests/conformance/ (VF10) checks every rule of docs/reference/reference.md;
some fail because the compiler does not do what the reference says yet.
Those are listed, each with its ID from
docs/verification/vf10-conformance-2026-11-17.md, in
tests/conformance/known-gaps.txt. This script reads what
scripts/verify/expect.py printed (a saved log with --log, or a run of it
with the arguments after `--`) and sorts each file:

    NEW    fails and is not listed: a regression, or a gap to fix or list
    KNOWN  fails and is listed
    FIXED  passes and is listed: take its line out of the list

Exit status: 1 when a file is NEW, or (with --strict) FIXED, or when the
log holds no result at all; 0 otherwise. A listed name with no test file
is reported (STALE) and fails under --strict.
"""
import argparse
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))


def read_known(path):
    """name -> (id, what) from the list; '#' starts a comment line."""
    known = {}
    with open(path, encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            parts = line.split(None, 2)
            name = parts[0][:-3] if parts[0].endswith(".fd") else parts[0]
            known[name] = (parts[1] if len(parts) > 1 else "?",
                           parts[2] if len(parts) > 2 else "")
    return known


def parse_log(text):
    """expect.py's lines -> ({name: [problems]} of failures, set of passes, dir names)."""
    fails, passes, dirs = {}, set(), set()
    cur = None
    for raw in text.splitlines():
        line = raw.rstrip("\r")
        m = re.match(r"^(FAIL|ok)\s+(\S.*)$", line)
        if m:
            path = m.group(2).strip().replace("\\", "/")
            name = os.path.basename(path)
            if name.endswith(".fd"):
                name = name[:-3]
            dirs.add(os.path.dirname(path))
            if m.group(1) == "FAIL":
                fails[name] = []
                cur = name
            else:
                passes.add(name)
                cur = None
        elif cur is not None and line.startswith("     "):
            fails[cur].append(line.strip())
        else:
            cur = None
    return fails, passes, dirs


def main():
    argv = sys.argv[1:]
    rest = []
    if "--" in argv:
        i = argv.index("--")
        argv, rest = argv[:i], argv[i + 1:]
    ap = argparse.ArgumentParser()
    ap.add_argument("--known", default=os.path.join(ROOT, "tests", "conformance", "known-gaps.txt"))
    ap.add_argument("--log", help="a saved output of scripts/verify/expect.py")
    ap.add_argument("--strict", action="store_true",
                    help="also fail on FIXED (a listed test passes) and STALE names")
    a = ap.parse_args(argv)
    try:
        sys.stdout.reconfigure(errors="replace")
    except (AttributeError, ValueError):
        pass
    if a.log and rest:
        ap.error("give --log or expect.py arguments after --, not both")
    if a.log:
        with open(a.log, encoding="utf-8", errors="replace") as f:
            text = f.read()
    elif rest:
        cmd = [sys.executable, os.path.join(HERE, "expect.py")] + rest
        p = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                           encoding="utf-8", errors="replace")
        text = p.stdout
    else:
        ap.error("give --log <file> or -- <expect.py arguments>")
    known = read_known(a.known)
    fails, passes, dirs = parse_log(text)
    if not fails and not passes:
        print("conf-known: no expect.py result in the output")
        print(text[-2000:])
        sys.exit(1)
    new = sorted(n for n in fails if n not in known)
    kept = sorted(n for n in fails if n in known)
    fixed = sorted(n for n in passes if n in known)
    # a listed name that no file of the run's directories has
    stale = []
    for n in sorted(known):
        if n in fails or n in passes:
            continue
        if not any(os.path.isfile(os.path.join(d if os.path.isabs(d) else os.path.join(ROOT, d), n + ".fd"))
                   for d in dirs if d):
            stale.append(n)
    for n in new:
        print(f"NEW    {n}")
        for pr in fails[n]:
            print(f"       {pr}")
    for n in kept:
        print(f"KNOWN  {n} ({known[n][0]})")
    for n in fixed:
        print(f"FIXED  {n} ({known[n][0]}): passes now, take it out of {os.path.relpath(a.known, ROOT)}")
    for n in stale:
        print(f"STALE  {n} ({known[n][0]}): listed, but no such test")
    total = len(fails) + len(passes)
    print(f"{len(passes)} of {total} pass; {len(kept)} known gaps, {len(new)} new failures, "
          f"{len(fixed)} listed gaps fixed{', %d stale names' % len(stale) if stale else ''}")
    bad = bool(new) or (a.strict and (fixed or stale))
    sys.exit(1 if bad else 0)


if __name__ == "__main__":
    main()
