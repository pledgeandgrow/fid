#!/usr/bin/env python3
"""pass-matrix-report.py - the record and first triage of scripts/pass-matrix.sh
runs (own-backend roadmap RL3: the suite with each mid-end pass and each rule
group disabled gives the same output).

pass-matrix.sh writes $OUT/matrix.txt per target (default build/pm/<target>):
the all-on summary, then one line per switch, "<- differs" with the tests
whose verdict changed ("on:  PASS t" / "off: FAIL t") or whose output
changed ("output differs: t"). This reads those files and writes, per
target, how many switches were tried and which differ, each differing
switch with its tests (PASS -> FAIL, FAIL -> PASS, output), and across all
targets the tests that differ under many switches (more likely a flaky test
or a base run that went wrong than a pass: rerun them with
scripts/flake-repeat.sh before blaming a switch), and the switches that
differ on every target (more likely the mid-end than one backend).

Usage:
  python3 scripts/verify/pass-matrix-report.py [DIR | matrix.txt ...]
         [--md FILE] [--title TEXT] [--compiler TEXT] [--many N]
  python3 scripts/verify/pass-matrix-report.py --selftest
    DIR        a folder of <target>/matrix.txt (default build/pm)
    --many N   a test differing under N or more switches of a target is
               listed as suspect (default 4)

Exit status: 0 when every matrix ran to its end and no switch differs,
1 otherwise, 2 when no matrix.txt was found.
"""

import os
import re
import sys
import tempfile

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))

LABEL_RE = re.compile(
    r"^((?:iropt|nat|opt-in|egraph rule|rules\.fd rule) \S+ (?:off|on):|all on:)(.*)$")
MARK_RE = re.compile(r"^(.*?)\s*<- (differs|no results)\s*$")
DETAIL_RE = re.compile(r"^\s+(on|off):\s+([A-Z]+)\s+(\S+)")
OUTPUT_RE = re.compile(r"^\s+output differs:\s+(\S+)")
END_RE = re.compile(r"^pass matrix on (\S+): (\d+) switches differ")
START_RE = re.compile(r"^pass matrix on (\S+): (\d+) files given")


def parse(path):
    """{'target', 'switches': [(label, summary, state)], 'tests': {label:
    {test: change}}, 'finished', 'given'}"""
    with open(path, "rb") as f:
        text = f.read().decode("utf-8", "replace").replace("\r\n", "\n")
    m = {"path": path, "target": os.path.basename(os.path.dirname(path)) or "?",
         "switches": [], "tests": {}, "finished": False, "given": None, "differ": None}
    cur = None
    for line in text.split("\n"):
        s = START_RE.match(line)
        if s:
            m["target"], m["given"] = s.group(1), int(s.group(2))
            continue
        e = END_RE.match(line)
        if e:
            m["target"], m["finished"], m["differ"] = e.group(1), True, int(e.group(2))
            continue
        lm = LABEL_RE.match(line)
        if lm:
            label = lm.group(1)[:-1]  # without the colon
            rest, state = lm.group(2), "same"
            mk = MARK_RE.match(rest)
            if mk:
                rest, state = mk.group(1), mk.group(2)
            m["switches"].append((label, rest.strip(), state))
            cur = label
            m["tests"].setdefault(label, {})
            continue
        if cur is None:
            continue
        d = DETAIL_RE.match(line)
        if d:
            side, verdict, t = d.group(1), d.group(2), d.group(3)
            ch = m["tests"][cur].setdefault(t, {})
            ch[side] = verdict
            continue
        o = OUTPUT_RE.match(line)
        if o:
            m["tests"][cur].setdefault(o.group(1), {})["output"] = True
    return m


def change_text(ch):
    on, off = ch.get("on"), ch.get("off")
    if on or off:
        s = "%s -> %s" % (on or "absent", off or "absent")
    else:
        s = ""
    if ch.get("output"):
        s = (s + ", " if s else "") + "output differs"
    return s


def find(args):
    paths = []
    for a in args or [os.path.join(ROOT, "build", "pm")]:
        if os.path.isfile(a):
            paths.append(a)
        elif os.path.isdir(a):
            if os.path.isfile(os.path.join(a, "matrix.txt")):
                paths.append(os.path.join(a, "matrix.txt"))
            for n in sorted(os.listdir(a)):
                p = os.path.join(a, n, "matrix.txt")
                if os.path.isfile(p):
                    paths.append(p)
    return paths


def render(ms, title, compiler, many, md):
    o = []
    h1, h2 = ("# ", "## ") if md else ("", "== ")
    o.append(h1 + (title or "Pass on/off matrix (RL3)"))
    o.append("")
    if compiler:
        o.append("Compiler: " + compiler)
        o.append("")
    if md:
        o.append("| target | tests | switches tried | differ | no results | finished |")
        o.append("|---|---|---|---|---|---|")
    for m in ms:
        sw = [x for x in m["switches"] if x[0] != "all on"]
        nd = sum(1 for x in sw if x[2] == "differs")
        nr = sum(1 for x in sw if x[2] == "no results")
        given = "whole suite" if m["given"] == 0 else ("%s files" % m["given"] if m["given"] is not None else "?")
        if md:
            o.append("| `%s` | %s | %d | %d | %d | %s |" % (m["target"], given, len(sw), nd, nr, "yes" if m["finished"] else "**no**"))
        else:
            o.append("%-16s %-12s %3d switches, %2d differ, %2d no results%s"
                     % (m["target"], given, len(sw), nd, nr, "" if m["finished"] else ", NOT FINISHED"))
    o.append("")
    for m in ms:
        base = [x for x in m["switches"] if x[0] == "all on"]
        o.append(h2 + "%s%s" % (m["target"], (": all on " + base[0][1]) if base else ""))
        o.append("")
        bad = [x for x in m["switches"] if x[2] != "same"]
        if not bad:
            o.append("Every switch gives the same results as its base." if m["switches"] else "No switch ran.")
            o.append("")
        for label, summ, state in bad:
            o.append("%s%s: %s (%s)" % ("- " if md else "  ", ("`%s`" % label) if md else label, state, summ))
            for t, ch in sorted(m["tests"].get(label, {}).items()):
                o.append("%s%s: %s" % ("  - " if md else "      ", ("`%s`" % t) if md else t, change_text(ch)))
        if bad:
            o.append("")
        # tests differing under many switches of this target
        count = {}
        for label, _s, state in m["switches"]:
            if state != "differs":
                continue
            for t in m["tests"].get(label, {}):
                count[t] = count.get(t, 0) + 1
        sus = sorted((t for t, c in count.items() if c >= many), key=lambda t: (-count[t], t))
        if sus:
            o.append("Suspect (differs under %d or more switches: rerun with scripts/flake-repeat.sh, "
                     "look at the all-on run): %s" % (many, ", ".join("%s (%d)" % (t, count[t]) for t in sus)))
            o.append("")
    if len(ms) > 1:
        per = {}
        for m in ms:
            for label, _s, state in m["switches"]:
                if state == "differs":
                    per.setdefault(label, set()).add(m["target"])
        allt = {m["target"] for m in ms}
        every = sorted(l for l, ts in per.items() if ts == allt)
        o.append(h2 + "Across targets")
        o.append("")
        if every:
            o.append("Differ on every target (the mid-end or nat_ir's common code, not one encoder): "
                     + ", ".join(every))
        else:
            o.append("No switch differs on every target.")
        o.append("")
    return "\n".join(o)


def report(args, md=None, title=None, compiler=None, many=4, quiet=False):
    paths = find(args)
    if not paths:
        sys.stderr.write("pass-matrix-report: no matrix.txt found (run scripts/pass-matrix.sh)\n")
        return 2, []
    ms = [parse(p) for p in paths]
    if not quiet:
        sys.stdout.write(render(ms, title, compiler, many, False))
    if md:
        with open(md, "w", encoding="utf-8", newline="\n") as f:
            f.write(render(ms, title, compiler, many, True))
    bad = any((not m["finished"]) or any(x[2] != "same" for x in m["switches"]) for m in ms)
    return (1 if bad else 0), ms


def selftest():
    tmp = tempfile.mkdtemp(prefix="pm-report-")
    a = os.path.join(tmp, "x86_64-linux")
    b = os.path.join(tmp, "aarch64-linux")
    os.makedirs(a)
    os.makedirs(b)
    with open(os.path.join(a, "matrix.txt"), "w", newline="\n") as f:
        f.write("pass matrix on x86_64-linux: 0 files given (0: the whole suite)\n"
                "all on:                    x86_64-linux: 300 passed, 2 failed, 0 did not build, 1 without reference\n"
                "iropt gvn off:             x86_64-linux: 300 passed, 2 failed, 0 did not build, 1 without reference\n"
                "iropt licm off:            x86_64-linux: 299 passed, 3 failed, 0 did not build, 1 without reference   <- differs\n"
                "      on:  PASS test_loop\n"
                "      off: FAIL test_loop\n"
                "nat promote off:           x86_64-linux: 300 passed, 2 failed, 0 did not build, 1 without reference   <- differs\n"
                "      output differs: test_float\n"
                "rules.fd rule 3 off:       x86_64-linux: 299 passed, 3 failed, 0 did not build, 1 without reference   <- differs\n"
                "      on:  PASS test_loop\n"
                "      off: FAIL test_loop\n"
                "egraph rule comm off:                                 <- no results\n"
                "pass matrix on x86_64-linux: 3 switches differ from their base (summary: build/pm/x/matrix.txt)\n")
    with open(os.path.join(b, "matrix.txt"), "w", newline="\n") as f:
        f.write("pass matrix on aarch64-linux: 2 files given (0: the whole suite)\n"
                "all on:                    aarch64-linux: 2 passed, 0 failed, 0 did not build, 0 without reference\n"
                "iropt licm off:            aarch64-linux: 1 passed, 1 failed, 0 did not build, 0 without reference   <- differs\n"
                "      on:  PASS test_loop\n"
                "      off: FAIL test_loop\n")
    md = os.path.join(tmp, "r.md")
    st, ms = report([tmp], md=md, many=2, quiet=True)
    bad = []
    if st != 1:
        bad.append("status %s, want 1" % st)
    byt = {m["target"]: m for m in ms}
    x = byt.get("x86_64-linux")
    y = byt.get("aarch64-linux")
    if not x or not y:
        bad.append("targets %s" % sorted(byt))
    else:
        states = {l: s for l, _m, s in x["switches"]}
        want = {"all on": "same", "iropt gvn off": "same", "iropt licm off": "differs",
                "nat promote off": "differs", "rules.fd rule 3 off": "differs",
                "egraph rule comm off": "no results"}
        if states != want:
            bad.append("x86_64 states %s" % states)
        if change_text(x["tests"]["iropt licm off"]["test_loop"]) != "PASS -> FAIL":
            bad.append("licm change %r" % change_text(x["tests"]["iropt licm off"]["test_loop"]))
        if change_text(x["tests"]["nat promote off"]["test_float"]) != "output differs":
            bad.append("promote change wrong")
        if not x["finished"] or x["given"] != 0 or y["finished"]:
            bad.append("finished/given: %s %s %s" % (x["finished"], x["given"], y["finished"]))
    t = open(md, encoding="utf-8").read()
    for need in ("| `aarch64-linux` | 2 files | 1 | 1 | 0 | **no** |",
                 "Suspect (differs under 2 or more switches",
                 "test_loop (2)",
                 "Differ on every target (the mid-end or nat_ir's common code, not one encoder): iropt licm off"):
        if need not in t:
            bad.append("report lacks %r" % need)
    # a clean, finished matrix: 0
    c = os.path.join(tmp, "clean")
    os.makedirs(c)
    with open(os.path.join(c, "matrix.txt"), "w") as f:
        f.write("pass matrix on riscv64-linux: 0 files given\nall on:   riscv64-linux: 3 passed\n"
                "iropt gvn off:   riscv64-linux: 3 passed\npass matrix on riscv64-linux: 0 switches differ from their base\n")
    st2, _ = report([os.path.join(c, "matrix.txt")], quiet=True)
    if st2 != 0:
        bad.append("a clean matrix gave %s" % st2)
    for x in bad:
        print("pass-matrix-report selftest: " + x)
    if not bad:
        print("pass-matrix-report selftest: ok")
    return 1 if bad else 0


def main(argv):
    if "--selftest" in argv:
        return selftest()
    args, md, title, compiler, many = [], None, None, None, 4
    i = 1
    while i < len(argv):
        a = argv[i]
        if a in ("--md", "--title", "--compiler", "--many") and i + 1 < len(argv):
            v = argv[i + 1]
            if a == "--md":
                md = v
            elif a == "--title":
                title = v
            elif a == "--compiler":
                compiler = v
            else:
                many = int(v)
            i += 2
            continue
        if a.startswith("-"):
            sys.stderr.write(__doc__)
            return 2
        args.append(a)
        i += 1
    st, _ = report(args, md, title, compiler, many)
    return st


if __name__ == "__main__":
    sys.exit(main(sys.argv))
