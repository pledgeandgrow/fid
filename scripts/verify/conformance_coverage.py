#!/usr/bin/env python3
"""How much of the language reference the conformance suite covers (VF10).

    python scripts/verify/conformance_coverage.py [--min N] [--list]

Each test of tests/conformance/ names its section on its first line,
`// Conformance: docs/reference/reference.md §4.6.`, and quotes the sentence
it checks. This counts the tests per section of docs/reference/reference.md
(its `##`/`###` headings), and the rejections (`//! error:`), panics
(`//! panic`) and outputs (`//>`) among them.

Exit status 1 when a section with rules has no test, or (with --min N) when
the suite has fewer than N tests; sections 14 and 15 (differences and docs
that disagree) hold no rules of their own and are not required.
"""
import argparse
import glob
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
REF = os.path.join(ROOT, "docs", "reference", "reference.md")
SUITE = os.path.join(ROOT, "tests", "conformance")
NOT_REQUIRED = {"14", "15"}


def sections():
    """[(number, title)] of the reference, in order."""
    out = []
    for line in open(REF, encoding="utf-8"):
        m = re.match(r"^(##|###) (\d+(?:\.\d+)?)\.? (.*)", line.strip())
        if m:
            out.append((m.group(2), m.group(3).strip()))
    return out


def tests():
    """{section: [(file, kind)]} where kind is error, panic or output."""
    by = {}
    for path in sorted(glob.glob(os.path.join(SUITE, "*.fd"))):
        text = open(path, encoding="utf-8", errors="replace").read()
        m = re.search(r"reference\.md §(\d+(?:\.\d+)?)", text.split("\n", 1)[0])
        if not m:
            m = re.search(r"§(\d+(?:\.\d+)?)", text)
        sec = m.group(1) if m else "?"
        kind = "error" if "\n//! error:" in "\n" + text else ("panic" if "//! panic" in text else "output")
        by.setdefault(sec, []).append((os.path.basename(path), kind))
    return by


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--min", type=int, default=0, help="fail below this many tests")
    ap.add_argument("--list", action="store_true", help="list the files of each section")
    a = ap.parse_args()
    secs = sections()
    by = tests()
    total = sum(len(v) for v in by.values())
    missing = []
    print(f"{'section':<8} {'tests':>5} {'error':>5} {'panic':>5}  title")
    for num, title in secs:
        # a top-level section with subsections is covered by them
        subs = [n for n, _ in secs if n.startswith(num + ".")]
        own = by.get(num, [])
        n = len(own) + sum(len(by.get(s, [])) for s in subs)
        if not subs or own:
            ne = sum(1 for _, k in own if k == "error")
            np_ = sum(1 for _, k in own if k == "panic")
            print(f"§{num:<7} {len(own):>5} {ne:>5} {np_:>5}  {title}")
            if a.list:
                for f, k in own:
                    print(f"           {f} ({k})")
        if n == 0 and num.split(".")[0] not in NOT_REQUIRED:
            missing.append(num)
    known = {n for n, _ in secs}
    stray = sorted(s for s in by if s not in known)
    if stray:
        print("tests naming no section of the reference: " + ", ".join(
            f"§{s} ({len(by[s])})" for s in stray))
    print(f"{total} tests; sections without a test: {', '.join(missing) if missing else 'none'}")
    bad = bool(missing) or (a.min and total < a.min)
    sys.exit(1 if bad else 0)


if __name__ == "__main__":
    main()
