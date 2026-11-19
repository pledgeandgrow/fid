#!/usr/bin/env python3
# check_course.py -- check docs/course/ (roadmap L10I, university course):
# the syllabus lists every week, every week has runnable examples, and
# every example runs through fid as the notes show.
#
# The notes follow the book's conventions (docs/book/README.md), so the
# examples are run by tests/book/check_book.py: a ```fides block whose
# first line is `// name.fd` is a complete file, a ``` block starting with
# `$ ` is a terminal session whose output must match, a file whose name
# ends in `_fail.fd` must be rejected by `fid check`, and any other file
# no session runs must pass it. On top of the book's commands, a course
# session may run `fid audit` and `fid lint` (static, host-independent).
#
# Structure checks, which fail without the course material:
#   - docs/course/README.md exists and links each docs/course/weekNN-*.md;
#   - there are 12 to 14 weeks, numbered 01.. without gaps;
#   - each week has a "## Exercises" section, at least one complete
#     example and at least one session that runs `fid run` or `fid check`;
#   - the weeks cover the course's four themes: systems programming
#     (layouts, FFI, native back ends), memory safety (ownership, borrow
#     checker, regions), capability security (capabilities, fid audit,
#     sandbox) and effect systems (effects, @crashfree, reference §9).
#
# Usage:  python3 tests/course/check_course.py --fid <fid> [--native] [-v]
#                                              [--structure-only] [week.md ...]
# Exit:   1 if any check fails, else 0.

import glob
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, os.path.join(ROOT, "tests", "book"))
import check_book  # noqa: E402

# the book's safe commands, plus the course's static ones
check_book.SAFE_CMD = re.compile(
    r"^((CC|FID_\w+)=(\"[^\"]*\"|\S+)\s+)*"
    r"(fid\s+(run|check|version|audit|lint|explain)\b|printf\b|echo\b|cat\b)")

COURSE = os.path.join(ROOT, "docs", "course")

# theme -> words that must all appear (case-insensitive) somewhere in the weeks
THEMES = {
    "systems programming": ["@repr(C)", "@packed", "struct_size", "extern \"C\"",
                            "fid emit", "ccompile", "--target"],
    "memory safety": ["ownership", "proposal 0004", "borrow checker", "&mut",
                      "region", "@linear", "--leaks"],
    "capability security": ["capabilit", "with { ", "sandbox", "fid audit",
                            "@secret"],
    "effect systems": ["effect", "!{}", "!IO", "@crashfree", "@no-alloc",
                       "reference §9", "handle"],
}


def structure(verbose):
    fails = []
    readme = os.path.join(COURSE, "README.md")
    weeks = sorted(glob.glob(os.path.join(COURSE, "week[0-9][0-9]-*.md")))
    if not os.path.exists(readme):
        fails.append("docs/course/README.md is missing")
        text = ""
    else:
        text = open(readme, encoding="utf-8").read()
    if not 12 <= len(weeks) <= 14:
        fails.append("%d week files; a term is 12 to 14 weeks" % len(weeks))
    nums = [int(re.match(r"week(\d\d)-", os.path.basename(w)).group(1)) for w in weeks]
    if nums != list(range(1, len(weeks) + 1)):
        fails.append("weeks are not numbered 01..%02d without gaps: %s" % (len(weeks), nums))
    alltext = ""
    for w in weeks:
        name = os.path.basename(w)
        body = open(w, encoding="utf-8").read()
        alltext += body + "\n"
        if "(" + name + ")" not in text:
            fails.append("README.md does not link %s" % name)
        if not re.search(r"^## Exercises\s*$", body, re.M):
            fails.append("%s has no '## Exercises' section" % name)
        bl = check_book.blocks(w)
        files = [l for lang, l, _ in bl if lang == "fides" and l and check_book.FILE_RE.match(l[0])]
        runs = [l for lang, l, _ in bl if l and l[0].startswith("$ ")
                and any(re.match(r"^\$ (\S+=\S+\s+)*fid (run|check) ", x) for x in l)]
        if not files:
            fails.append("%s has no complete example (a ```fides block starting `// name.fd`)" % name)
        if not runs:
            fails.append("%s has no session that runs `fid run` or `fid check`" % name)
        if verbose:
            print("  %s: %d file(s), %d session(s) with fid run/check" % (name, len(files), len(runs)))
    low = alltext.lower()
    for theme, words in THEMES.items():
        missing = [x for x in words if x.lower() not in low]
        if missing:
            fails.append("theme '%s' is not covered: no mention of %s" % (theme, ", ".join(missing)))
    print("course structure: %d week(s), %d failure(s)" % (len(weeks), len(fails)))
    for f in fails:
        print("  FAIL " + f)
    return len(fails)


def main():
    try:
        sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    except AttributeError:
        pass
    argv = sys.argv[1:]
    fid = None
    native = verbose = only_structure = False
    paths = []
    i = 0
    while i < len(argv):
        a = argv[i]
        if a == "--fid":
            fid = argv[i + 1]
            i += 2
            continue
        if a == "--native":
            native = True
        elif a == "-v":
            verbose = True
        elif a == "--structure-only":
            only_structure = True
        else:
            paths.append(a)
        i += 1
    bad = structure(verbose)
    if not only_structure:
        if fid is None:
            fid = os.environ.get("FID") or ("release/fid.exe" if os.path.exists("release/fid.exe")
                                            else "release/fid")
        fid = os.path.abspath(fid)
        if not paths:
            paths = sorted(glob.glob(os.path.join(COURSE, "week[0-9][0-9]-*.md")))
        for p in paths:
            bad += check_book.check_chapter(p, fid, native, verbose)
    print("course: %d failure(s)" % bad)
    sys.exit(1 if bad else 0)


if __name__ == "__main__":
    main()
