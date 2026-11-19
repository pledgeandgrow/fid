#!/usr/bin/env python3
# check_labs.py -- check docs/course/labs/ (roadmap L10I, lab exercises):
# one lab per week of the course, each a starter with its tasks, a link
# that opens the starter in the browser playground, and a solution whose
# output is checked against the lab's expected.txt.
#
# A lab is a directory docs/course/labs/labNN-<name>/ (NN = the week of
# docs/course/weekNN-*.md it belongs to) holding
#   README.md     the lab sheet: tasks, playground links, how to check
#   starter.fd    what students start from
#   solution.fd   a model solution
#   expected.txt  what the solution prints, line by line
#   *_fail.fd     (optional) programs the lab shows being rejected
#   other *.fd    (optional) more programs, which must pass `fid check`
#
# A .fd file states how it is checked in `//! key: value` comment lines:
#   //! lab: N            the lab's number (starter and solution)
#   //! check: runs       must pass `fid check` (a starter must also run and
#                         print something other than expected.txt: the
#                         tasks are not already done)
#   //! check: rejected   `fid check` must fail
#   //! error: <text>     ...with <text> in its message (case-insensitive)
#   //! playground: yes   the solution runs in the browser playground (whose
#                         compiler is the WASI build of fidesc) and prints
#                         expected.txt there too; `no` says it does not
#   //! run: ccompile     run the solution through C (`fid ccompile`, CC or
#                         "zig cc"), not the interpreter (needed for FFI)
#   //! env: NAME=VALUE   environment of the solution's run
#   //! forbid: <word>    <word> must not appear in the solution's code
#
# Structure checks (no compiler needed; fail without the labs):
#   - docs/course/labs/README.md links every lab, and the syllabus
#     (docs/course/README.md) links docs/course/labs/README.md;
#   - one lab per week: lab numbers are exactly the weeks' numbers;
#   - each lab has its four files and consistent `//! lab:` numbers; each
#     week's notes link their lab;
#   - each lab README has a playground link
#     (https://fides-lang.org/playground#<share hash>) and an iframe
#     embed (.../playground/embed.html#<share hash>) whose hash decodes to
#     starter.fd exactly (the playground's share form:
#     btoa(encodeURIComponent(code))), and says whether the solution runs in
#     the playground, matching the solution's `//! playground:`, and shows
#     expected.txt in a ```text block.
# Run checks (need fid): every file as its directives say; the solution's
# output equals expected.txt.
# --playground also runs each solution marked `playground: yes` through the
# playground's own compiler (tests/course/playground_run.mjs, Node and the
# website checkout's fidesc.wasm) and requires expected.txt there too.
#
# Usage:  python3 tests/course/check_labs.py --fid <fid> [--playground] [-v]
#                                            [--structure-only] [--links]
#                                            [labNN ...]
#   --links  print each lab's playground link (to paste into a README)
# Exit:   1 if any check fails, else 0.

import base64
import glob
import os
import re
import shutil
import subprocess
import sys
import tempfile
import json
import urllib.parse

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
COURSE = os.path.join(ROOT, "docs", "course")
LABS = os.path.join(COURSE, "labs")
PLAYGROUND = "https://fides-lang.org/playground"
LINK_RE = re.compile(r"https://fides-lang\.org/playground#([A-Za-z0-9+/=]+)")
EMBED_RE = re.compile(r"https://fides-lang\.org/playground/embed\.html#([A-Za-z0-9+/=]+)")
DIRECTIVE_RE = re.compile(r"^//!\s*([a-z-]+):\s*(.*?)\s*$")
# lines the compiler prints about itself, not the program's output
NOISE = re.compile(r"^\[(build|prof|region|time)\]")


def read(path):
    with open(path, encoding="utf-8") as f:
        return f.read().replace("\r\n", "\n")


def share_hash(code):
    """The playground's share form: btoa(encodeURIComponent(code))."""
    quoted = urllib.parse.quote(code, safe="-_.!~*'()")
    return base64.b64encode(quoted.encode("ascii")).decode("ascii")


def unshare(h):
    try:
        return urllib.parse.unquote(base64.b64decode(h).decode("ascii"), encoding="utf-8",
                                    errors="strict")
    except Exception:
        return None


def directives(text):
    d = {}
    for line in text.split("\n"):
        m = DIRECTIVE_RE.match(line)
        if m:
            d.setdefault(m.group(1), []).append(m.group(2))
    return d


def one(d, key, default=None):
    v = d.get(key)
    return v[0] if v else default


def code_only(text):
    """The source without `//` comments (string contents are kept)."""
    out = []
    for line in text.split("\n"):
        in_str = False
        cut = len(line)
        i = 0
        while i < len(line):
            c = line[i]
            if c == '"' and (i == 0 or line[i - 1] != "\\"):
                in_str = not in_str
            elif not in_str and line.startswith("//", i):
                cut = i
                break
            i += 1
        out.append(line[:cut])
    return "\n".join(out)


def clean(text):
    res = [ln.rstrip() for ln in text.replace("\r\n", "\n").split("\n") if not NOISE.match(ln)]
    while res and res[-1] == "":
        res.pop()
    return res


def lab_dirs():
    return sorted(d for d in glob.glob(os.path.join(LABS, "lab[0-9][0-9]-*")) if os.path.isdir(d))


def lab_num(d):
    return int(re.match(r"lab(\d\d)-", os.path.basename(d)).group(1))


def structure(only, verbose):
    fails = []
    index = os.path.join(LABS, "README.md")
    itext = read(index) if os.path.exists(index) else ""
    if not itext:
        fails.append("docs/course/labs/README.md is missing")
    syllabus = os.path.join(COURSE, "README.md")
    if not os.path.exists(syllabus) or "labs/README.md" not in read(syllabus):
        fails.append("docs/course/README.md does not link labs/README.md")
    weeks = sorted(glob.glob(os.path.join(COURSE, "week[0-9][0-9]-*.md")))
    wnums = [int(re.match(r"week(\d\d)-", os.path.basename(w)).group(1)) for w in weeks]
    dirs = lab_dirs()
    lnums = [lab_num(d) for d in dirs]
    if lnums != wnums:
        fails.append("labs %s do not match the weeks %s (one lab per week)" % (lnums, wnums))
    for d in dirs:
        name = os.path.basename(d)
        if only and not any(name.startswith(o) for o in only):
            continue
        n = lab_num(d)
        missing = [f for f in ("README.md", "starter.fd", "solution.fd", "expected.txt")
                   if not os.path.exists(os.path.join(d, f))]
        if missing:
            fails.append("%s: missing %s" % (name, ", ".join(missing)))
            continue
        if "(%s/README.md)" % name not in itext:
            fails.append("labs/README.md does not link %s/README.md" % name)
        for w in weeks:
            if int(re.match(r"week(\d\d)-", os.path.basename(w)).group(1)) == n and \
                    "labs/%s/README.md" % name not in read(w):
                fails.append("%s does not link labs/%s/README.md" % (os.path.basename(w), name))
        starter = read(os.path.join(d, "starter.fd"))
        solution = read(os.path.join(d, "solution.fd"))
        sd, ld = directives(starter), directives(solution)
        for fname, dd in (("starter.fd", sd), ("solution.fd", ld)):
            if one(dd, "lab") != str(n):
                fails.append("%s/%s: `//! lab: %d` is missing or wrong" % (name, fname, n))
        if one(sd, "check") not in ("runs", "rejected"):
            fails.append("%s/starter.fd: no `//! check: runs|rejected`" % name)
        if one(ld, "check") != "runs":
            fails.append("%s/solution.fd: a solution must be `//! check: runs`" % name)
        pg = one(ld, "playground")
        if pg not in ("yes", "no"):
            fails.append("%s/solution.fd: no `//! playground: yes|no`" % name)
        readme = read(os.path.join(d, "README.md"))
        want = share_hash(starter)
        links = LINK_RE.findall(readme)
        embeds = EMBED_RE.findall(readme)
        if not links:
            fails.append("%s/README.md: no playground link (%s#...)" % (name, PLAYGROUND))
        if not embeds:
            fails.append("%s/README.md: no embed link (%s/embed.html#...)" % (name, PLAYGROUND))
        for h in links + embeds:
            if h != want:
                got = unshare(h)
                why = "does not decode" if got is None else "decodes to other code than starter.fd"
                fails.append("%s/README.md: a playground link %s; the starter's link is %s#%s"
                             % (name, why, PLAYGROUND, want))
        shown = "\n".join(clean(read(os.path.join(d, "expected.txt"))))
        if "```text\n" + shown + "\n```" not in readme:
            fails.append("%s/README.md does not show expected.txt (in a ```text block)" % name)
        m = re.search(r"\*\*Runs in the playground:\*\*\s*(yes|no)\b", readme)
        if not m:
            fails.append("%s/README.md: no `**Runs in the playground:** yes|no` line" % name)
        elif pg in ("yes", "no") and m.group(1) != pg:
            fails.append("%s/README.md says the playground %s, solution.fd says playground: %s"
                         % (name, "runs it" if m.group(1) == "yes" else "does not", pg))
        if verbose:
            print("  %s: structure ok so far (%d link(s))" % (name, len(links) + len(embeds)))
    print("labs structure: %d lab(s), %d failure(s)" % (len(dirs), len(fails)))
    for f in fails:
        print("  FAIL " + f)
    return len(fails)


def run_fid(fid, args, cwd, env, timeout=900):
    p = subprocess.run([fid] + args, cwd=cwd, env=env, stdout=subprocess.PIPE,
                       stderr=subprocess.PIPE, timeout=timeout)
    return (p.returncode, p.stdout.decode("utf-8", "replace"),
            p.stderr.decode("utf-8", "replace"))


def run_program(fid, fname, d, cwd, env):
    """Run fname as its directives say; (exit code, stdout, stderr)."""
    e = dict(env)
    for kv in d.get("env", []):
        k, _, v = kv.partition("=")
        e[k] = v
    if one(d, "run") == "ccompile":
        exe = os.path.join(cwd, os.path.splitext(fname)[0] + (".exe" if os.name == "nt" else ""))
        code, out, err = run_fid(fid, ["ccompile", fname, "-o", exe], cwd, e)
        if code != 0 or not os.path.exists(exe):
            return code or 1, "", "fid ccompile failed:\n" + out + err
        p = subprocess.run([exe], cwd=cwd, env=e, stdout=subprocess.PIPE,
                           stderr=subprocess.PIPE, timeout=300)
        return p.returncode, p.stdout.decode("utf-8", "replace"), p.stderr.decode("utf-8", "replace")
    return run_fid(fid, ["run", fname], cwd, e)


def check_lab(d, fid, verbose):
    name = os.path.basename(d)
    fails = []
    tmp = tempfile.mkdtemp(prefix="lab_" + name[:5] + "_")
    env = dict(os.environ)
    env.setdefault("FID_HOME", ROOT)
    env.setdefault("CC", "zig cc")
    expected = clean(read(os.path.join(d, "expected.txt")))
    try:
        for f in glob.glob(os.path.join(d, "*.fd")):
            with open(os.path.join(tmp, os.path.basename(f)), "w", encoding="utf-8", newline="\n") as o:
                o.write(read(f))
        for f in sorted(glob.glob(os.path.join(d, "*.fd"))):
            fname = os.path.basename(f)
            text = read(f)
            dd = directives(text)
            mode = one(dd, "check", "rejected" if fname.endswith("_fail.fd") else "runs")
            code, out, err = run_fid(fid, ["check", fname], tmp, env)
            msg = out + err
            if mode == "rejected":
                if code == 0:
                    fails.append("%s: %s should be rejected by fid check, and was accepted" % (name, fname))
                    continue
                want = one(dd, "error")
                if want and want.lower() not in msg.lower():
                    fails.append("%s: %s was rejected, but the message does not mention '%s':\n    %s"
                                 % (name, fname, want, "\n    ".join(clean(msg)[:8])))
                    continue
                if verbose:
                    print("  %s: %s rejected" % (name, fname))
                continue
            if code != 0:
                fails.append("%s: %s does not pass fid check:\n    %s"
                             % (name, fname, "\n    ".join(clean(msg)[:12])))
                continue
            if fname == "solution.fd":
                for word in dd.get("forbid", []):
                    if word in code_only(text):
                        fails.append("%s: solution.fd uses `%s`, which the lab forbids" % (name, word))
                code, out, err = run_program(fid, fname, dd, tmp, env)
                actual = clean(out)
                if code != 0:
                    fails.append("%s: solution.fd exited with %d:\n    %s"
                                 % (name, code, "\n    ".join((actual + clean(err))[:12])))
                elif actual != expected:
                    fails.append("%s: solution.fd printed:\n    %s\n  expected.txt has:\n    %s"
                                 % (name, "\n    ".join(actual[:40]), "\n    ".join(expected)))
                elif verbose:
                    print("  %s: solution.fd prints expected.txt (%d lines)" % (name, len(expected)))
            elif fname == "starter.fd":
                # the starter must not already be the answer
                code, out, err = run_program(fid, fname, dd, tmp, env)
                if code == 0 and clean(out) == expected:
                    fails.append("%s: starter.fd already prints expected.txt: the tasks are done" % name)
                elif verbose:
                    print("  %s: starter.fd compiles and is not yet the answer" % name)
            elif verbose:
                print("  %s: %s passes fid check" % (name, fname))
    finally:
        shutil.rmtree(tmp, ignore_errors=True)
    print("%s: %d failure(s)" % (name, len(fails)))
    for f in fails:
        print("  FAIL " + f)
    return len(fails)


def check_playground(dirs, verbose):
    sols, want = [], {}
    for d in dirs:
        s = os.path.join(d, "solution.fd")
        if one(directives(read(s)), "playground") == "yes":
            sols.append(s)
            want[s] = clean(read(os.path.join(d, "expected.txt")))
    if not sols:
        return 0
    node = shutil.which("node")
    if not node:
        print("playground: FAIL node is not installed")
        return 1
    p = subprocess.run([node, os.path.join(HERE, "playground_run.mjs")] + sols, cwd=ROOT,
                       stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=1800)
    if p.returncode == 2:
        print("playground: FAIL the playground's assets are missing: " +
              p.stderr.decode("utf-8", "replace").strip())
        return 1
    try:
        res = json.loads(p.stdout.decode("utf-8", "replace"))
    except ValueError:
        print("playground: FAIL playground_run.mjs printed no result: " +
              p.stderr.decode("utf-8", "replace").strip())
        return 1
    fails = []
    for s in sols:
        name = os.path.basename(os.path.dirname(s))
        r = res.get(s)
        if r is None:
            fails.append("%s: not run" % name)
        elif "error" in r:
            fails.append("%s: the playground failed: %s" % (name, r["error"]))
        elif r.get("exitCode") != 0 or clean(r.get("stdout", "")) != want[s]:
            fails.append("%s: in the playground, solution.fd exited with %s and printed:\n    %s\n    %s"
                         % (name, r.get("exitCode"), "\n    ".join(clean(r.get("stdout", ""))[:30]),
                            "\n    ".join(clean(r.get("stderr", ""))[:6])))
        elif verbose:
            print("  %s: the playground prints expected.txt" % name)
    print("playground: %d solution(s), %d failure(s)" % (len(sols), len(fails)))
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
    verbose = only_structure = playground = links = False
    only = []
    i = 0
    while i < len(argv):
        a = argv[i]
        if a == "--fid":
            fid = argv[i + 1]
            i += 2
            continue
        if a == "-v":
            verbose = True
        elif a == "--structure-only":
            only_structure = True
        elif a == "--playground":
            playground = True
        elif a == "--links":
            links = True
        else:
            only.append(os.path.basename(a.rstrip("/\\")))
        i += 1
    dirs = [d for d in lab_dirs() if not only or any(os.path.basename(d).startswith(o) for o in only)]
    if links:
        for d in dirs:
            h = share_hash(read(os.path.join(d, "starter.fd")))
            print("%s\n  %s#%s\n  %s/embed.html#%s" % (os.path.basename(d), PLAYGROUND, h, PLAYGROUND, h))
        return
    bad = structure(only, verbose)
    if not only_structure:
        if fid is None:
            fid = os.environ.get("FID") or ("release/fid.exe" if os.path.exists("release/fid.exe")
                                            else "release/fid")
        fid = os.path.abspath(fid)
        for d in dirs:
            bad += check_lab(d, fid, verbose)
        if playground:
            bad += check_playground(dirs, verbose)
    elif playground:
        bad += check_playground(dirs, verbose)
    print("labs: %d failure(s)" % bad)
    sys.exit(1 if bad else 0)


if __name__ == "__main__":
    main()
