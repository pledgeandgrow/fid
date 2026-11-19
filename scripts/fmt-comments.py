#!/usr/bin/env python3
"""fmt-comments.py -- release-roadmap N4: does a formatted Fides file keep
every comment of the original?

  scripts/fmt-comments.py <original.fd> <formatted.fd>

Both files are scanned the same way: line comments (// ...) and block
comments (/* ... */), skipping string literals ("...", f"...") and char
literals ('x'). The comments must be the same, in the same order; inside a
comment only the amount of white space may differ (the formatter may
re-indent a block comment, or move a trailing comment to its own line).
Exit status 0 when they match, 1 (with the first difference) when not.
Used by scripts/fmt-check.sh.
"""
import re
import sys


def comments(text):
    out = []
    i = 0
    n = len(text)
    while i < n:
        c = text[i]
        if c == '"':
            # a string literal (f"..." too: the f was a word byte before it)
            i += 1
            while i < n and text[i] != '"':
                if text[i] == "\\":
                    i += 1
                i += 1
            i += 1
        elif c == "'":
            # a char literal: 'x' or '\n'; a lone quote is left alone
            m = re.match(r"'(\\.|[^\\'\n])'", text[i:i + 4])
            i += len(m.group(0)) if m else 1
        elif text.startswith("//", i):
            j = text.find("\n", i)
            j = n if j < 0 else j
            out.append(text[i:j])
            i = j
        elif text.startswith("/*", i):
            j = text.find("*/", i + 2)
            j = n if j < 0 else j + 2
            out.append(text[i:j])
            i = j
        else:
            i += 1
    return [" ".join(c.split()) for c in out]


def main():
    if len(sys.argv) != 3:
        print(__doc__)
        return 2
    a = comments(open(sys.argv[1], encoding="utf-8", errors="replace").read())
    b = comments(open(sys.argv[2], encoding="utf-8", errors="replace").read())
    if a == b:
        return 0
    k = 0
    while k < min(len(a), len(b)) and a[k] == b[k]:
        k += 1
    print("  FAIL  %s: formatting changed its comments (%d before, %d after)" % (sys.argv[1], len(a), len(b)))
    print("          comment %d was: %s" % (k + 1, a[k] if k < len(a) else "(none)"))
    print("          is now:        %s" % (b[k] if k < len(b) else "(none)"))
    return 1


if __name__ == "__main__":
    sys.exit(main())
