#!/usr/bin/env python3
# check_book.py -- run the complete examples of docs/book/ and compare them
# with the output the book shows (competitive-roadmap AD6).
#
# The book's conventions (docs/book/README.md):
#
#   - a ```fides block whose first line is `// name.fd` is a complete file.
#     Every such file of a chapter is written into one directory, so an
#     example may `use` a module another example of the chapter defines.
#   - a ``` block whose first line starts with `$ ` is a terminal session:
#     each `$ command` line is followed by what it printed (stdout and
#     stderr together). A line `...` in the expected text stands for any
#     number of lines (traces whose detail depends on the build), and a line
#     ending in `...` any line that starts with the text before it. `$ echo $?`
#     is followed by the previous command's exit status.
#   - a file named `*_fail.fd` must be rejected by `fid check`; any other
#     complete file that no session runs must pass `fid check`.
#
# Sessions run the commands `fid run|check|version ...`, `CC=... fid ...`, `printf`,
# `echo` and `cat`; a session with any other command (`./x`, `node`,
# `fid emit --target ...`, ...) is reported as skipped, since it depends on
# the host. With --native, each `fid run x.fd args` is also built with
# `fid ccompile` (CC, default "zig cc") and `fid emit`, and the binary's
# stdout and exit status must be the same as under `fid run`.
#
# Usage:  python3 tests/book/check_book.py [chapter.md ...] --fid <fid>
#                                          [--native] [--keep] [-v]
#         --keep leaves each chapter's directory of files and builds
#         (default: every docs/book/[0-9]*.md)
# Exit:   1 if any example fails, else 0.

import glob
import os
import re
import shlex
import shutil
import subprocess
import sys
import tempfile

NOISE = re.compile(r"^\[(build|prof|main|ccompile|bench|lower|iropt)\]")
FILE_RE = re.compile(r"^//\s*([A-Za-z0-9_\-]+\.fd)\b")
KEEP = False
SAFE_CMD = re.compile(r"^((CC|FID_\w+)=(\"[^\"]*\"|\S+)\s+)*(fid\s+(run|check|version)\b|printf\b|echo\b|cat\b)")


def blocks(path):
    """Fenced blocks of a Markdown file: (lang, lines, first line number)."""
    out = []
    lines = open(path, encoding="utf-8").read().splitlines()
    i = 0
    while i < len(lines):
        m = re.match(r"^```\s*(\w*)\s*$", lines[i])
        if m:
            lang = m.group(1)
            start = i + 1
            j = start
            while j < len(lines) and not re.match(r"^```\s*$", lines[j]):
                j += 1
            out.append((lang, lines[start:j], start + 1))
            i = j + 1
            continue
        i += 1
    return out


def clean(text):
    res = []
    for ln in text.replace("\r\n", "\n").split("\n"):
        if NOISE.match(ln):
            continue
        res.append(ln.rstrip())
    while res and res[-1] == "":
        res.pop()
    return res


def matches(expected, actual):
    """Line match: a line `...` stands for any run of lines, and a line
    ending in `...` for any line that starts with the text before it."""
    def rec(e, a):
        if e == len(expected):
            return a == len(actual)
        if expected[e].strip() == "...":
            for k in range(a, len(actual) + 1):
                if rec(e + 1, k):
                    return True
            return False
        want = expected[e].rstrip()
        if a < len(actual) and (want == actual[a] or
                                (want.endswith("...") and actual[a].startswith(want[:-3]))):
            return rec(e + 1, a + 1)
        return False
    return rec(0, 0)


def fid_words(cmd):
    """The words of a command without its leading VAR=value settings."""
    try:
        words = shlex.split(cmd, comments=True)
    except ValueError:
        return []
    while words and re.match(r"^[A-Z_][A-Z0-9_]*=", words[0]):
        words.pop(0)
    return words


def sessions(lines):
    """Split a terminal block into [(command, expected lines)]."""
    out = []
    for ln in lines:
        if ln.startswith("$ "):
            out.append((ln[2:].strip(), []))
        elif out:
            out[-1][1].append(ln)
    return out


def run_shell(cmd, cwd, fid, env):
    """Run one book command; `fid` is the compiler under test."""
    words = shlex.split(cmd, comments=True)
    extra = {}
    while words and re.match(r"^[A-Z_][A-Z0-9_]*=", words[0]):
        k, v = words.pop(0).split("=", 1)
        extra[k] = v
    if words and words[0] == "fid":
        words[0] = fid
        e = dict(env)
        e.update(extra)
        p = subprocess.run(words, cwd=cwd, env=e, stdout=subprocess.PIPE,
                           stderr=subprocess.STDOUT, timeout=600)
        return p.returncode, p.stdout.decode("utf-8", "replace")
    # shutil.which follows PATH: on Windows a bare "bash" would start WSL's
    # bash.exe from System32 rather than the Git Bash running this check
    p = subprocess.run([shutil.which("bash") or "bash", "-c", cmd], cwd=cwd, env=env,
                       stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                       timeout=120)
    return p.returncode, p.stdout.decode("utf-8", "replace")


def native_same(cmd, cwd, fid, env, verbose):
    """Build `fid run x.fd args` with ccompile and emit; compare stdout."""
    words = shlex.split(cmd, comments=True)
    extra = {}
    while words and re.match(r"^[A-Z_][A-Z0-9_]*=", words[0]):
        k, v = words.pop(0).split("=", 1)
        extra[k] = v
    src = words[2]
    args = words[3:]
    e = dict(env)
    e.update(extra)
    e.setdefault("CC", "zig cc")
    ref = subprocess.run([fid, "run", src] + args, cwd=cwd, env=e,
                         stdout=subprocess.PIPE, stderr=subprocess.DEVNULL,
                         timeout=600)
    want = clean(ref.stdout.decode("utf-8", "replace"))
    fails = []
    exe_suffix = ".exe" if sys.platform == "win32" else ""
    for mode in ("ccompile", "emit"):
        exe = os.path.join(cwd, src[:-3] + "_" + mode + exe_suffix)
        b = subprocess.run([fid, mode, src, "-o", exe], cwd=cwd, env=e,
                           stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                           timeout=900)
        if b.returncode != 0 or not os.path.exists(exe):
            fails.append("%s: %s did not build: %s" % (src, mode,
                         "\n".join(clean(b.stdout.decode("utf-8", "replace"))[-5:])))
            continue
        r = subprocess.run([exe] + args, cwd=cwd, env=e, stdout=subprocess.PIPE,
                           stderr=subprocess.DEVNULL, timeout=120)
        got = clean(r.stdout.decode("utf-8", "replace"))
        if got != want or r.returncode != ref.returncode:
            fails.append("%s: %s output differs from fid run (status %d vs %d):\n  %s" %
                         (src, mode, r.returncode, ref.returncode, "\n  ".join(got[:20])))
        elif verbose:
            print("    %s %s: same as fid run" % (src, mode))
        # binaries and generated C are megabytes each: drop them at once
        base = exe[:-len(exe_suffix)] if exe_suffix else exe
        for junk in (exe, exe + ".c", base + ".c", base + ".pdb", exe + ".fides-stamp"):
            try:
                os.remove(junk)
            except OSError:
                pass
    return fails


def check_chapter(path, fid, native, verbose):
    name = os.path.basename(path)
    bl = blocks(path)
    tmp = tempfile.mkdtemp(prefix="book_" + name[:2] + "_")
    files = {}
    for lang, lines, at in bl:
        if lang == "fides" and lines:
            m = FILE_RE.match(lines[0])
            if m:
                if m.group(1) in files:
                    print("  %s:%d: %s defined twice" % (name, at, m.group(1)))
                files[m.group(1)] = lines
                with open(os.path.join(tmp, m.group(1)), "w", encoding="utf-8", newline="\n") as f:
                    f.write("\n".join(lines) + "\n")
    env = dict(os.environ)
    env.setdefault("FID_HOME", os.getcwd())
    env.setdefault("CC", "zig cc")
    fails, ran, skipped = [], set(), 0
    passed = 0
    for lang, lines, at in bl:
        if not lines or not lines[0].startswith("$ "):
            continue
        sess = sessions(lines)
        if not all(SAFE_CMD.match(c) for c, _ in sess if c != "echo $?"):
            skipped += 1
            if verbose:
                print("  %s:%d: skipped (host-specific commands)" % (name, at))
            continue
        status = 0
        ok = True
        for cmd, expected in sess:
            if cmd == "echo $?":
                actual = [str(status)]
            else:
                status, text = run_shell(cmd, tmp, fid, env)
                actual = clean(text)
                w = fid_words(cmd)
                if len(w) >= 3 and w[0] == "fid" and w[1] in ("run", "check"):
                    ran.add(w[2])
            exp = [e.rstrip() for e in expected]
            while exp and exp[-1] == "":
                exp.pop()
            if not matches(exp, actual):
                ok = False
                fails.append("%s:%d: `%s` printed:\n    %s\n  the book shows:\n    %s" %
                             (name, at, cmd, "\n    ".join(actual[:40]), "\n    ".join(exp)))
                break
            w = fid_words(cmd)
            # `--leaks`, `--checked-alloc`, `--max-recursion` change how
            # `fid run` runs the program: nothing to compare a plain build with
            if (native and len(w) >= 3 and w[0] == "fid" and w[1] == "run" and
                    not any(x in ("--leaks", "--checked-alloc", "--max-recursion") for x in w[3:])):
                nf = native_same(cmd, tmp, fid, env, verbose)
                if nf:
                    ok = False
                    fails.extend("%s:%d: %s" % (name, at, x) for x in nf)
        if ok:
            passed += 1
            if verbose:
                print("  %s:%d: ok" % (name, at))
    # complete files no session runs: they must type-check (or be rejected)
    for fname in sorted(files):
        if fname in ran:
            continue
        code, text = run_shell("fid check " + fname, tmp, fid, env)
        want_fail = fname.endswith("_fail.fd")
        if want_fail and code == 0:
            fails.append("%s: %s should be rejected by fid check, and was accepted" % (name, fname))
        elif not want_fail and code != 0:
            fails.append("%s: %s does not pass fid check:\n    %s" %
                         (name, fname, "\n    ".join(clean(text)[:12])))
        else:
            passed += 1
            if verbose:
                print("  %s: %s %s by fid check" % (name, fname,
                      "rejected" if want_fail else "accepted"))
    print("%s: %d checked, %d failed, %d skipped (%d files)" %
          (name, passed, len(fails), skipped, len(files)))
    for f in fails:
        print("  FAIL " + f)
    if KEEP:
        print("  files kept in " + tmp)
    else:
        shutil.rmtree(tmp, ignore_errors=True)
    return len(fails)


def main():
    try:
        # the book and fid's messages are UTF-8 (arrows in traces); a Windows
        # console's default code page cannot print them
        sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    except AttributeError:
        pass
    argv = sys.argv[1:]
    fid = None
    native = False
    verbose = False
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
        elif a == "--keep":
            global KEEP
            KEEP = True
        else:
            paths.append(a)
        i += 1
    if fid is None:
        fid = os.environ.get("FID") or ("release/fid.exe" if os.path.exists("release/fid.exe") else "release/fid")
    fid = os.path.abspath(fid)
    if not paths:
        paths = sorted(glob.glob("docs/book/[0-9]*.md"))
    bad = 0
    for p in paths:
        bad += check_chapter(p, fid, native, verbose)
    print("book: %d failure(s)" % bad)
    sys.exit(1 if bad else 0)


if __name__ == "__main__":
    main()
