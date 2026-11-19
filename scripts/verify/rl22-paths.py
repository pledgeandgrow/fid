#!/usr/bin/env python3
"""rl22-paths.py - the two independent paths of a scripts/suite-all.sh run,
compared target by target (own-backend roadmap RL22: a release ships only
when the C path and our own backend agree on the suite).

`AGREE=1 scripts/suite-all.sh` fails when any two lanes disagree on a test,
across targets too. This script reads the same run's folder (default
build/suite-all; no compiler, no target) and pairs each C-path lane with the
own-backend lanes of the same target:

    c:x86_64-linux    native:x86_64-linux
    c:aarch64-linux   native:aarch64-linux
    c:riscv64-linux   native:riscv64-linux
    c:x86_64-windows  native:x86_64-windows-pe  (pe_ir, the default `fid emit`)
    c:x86_64-windows  native:x86_64-windows     (`emit --nat`)
    c:<t>-macos       native:<t>-macos

For each pair it lists the tests that pass on one path and fail or do not
build on the other, each with its cause (scripts/verify/suite-all-report.py
reads the lane files), and the tests that fail on both (the suite's own
failures, RL2). Lanes without a partner (c:wasm32-wasi) are named but not
compared.

A disagreement is either fixed (it no longer shows) or recorded in
scripts/verify/rl22-recorded.txt with why and where it is tracked:

    <pair> <test> <reason ...>
      pair  `<c lane>|<native lane>`, shell-style wildcards allowed
            (`c:*-linux|native:*-linux`)

Exit status: 0 when every disagreement of every pair is recorded (or there
is none), 1 when one is not, 2 on bad usage. A recorded entry that no longer
shows (the test agrees now, or is gone) is listed as stale; --strict also
fails on stale entries, so the record stays the list of open gaps.

Usage:
  python3 scripts/verify/rl22-paths.py [OUT] [--md FILE] [--recorded FILE]
                                       [--compiler TEXT] [--lanes "..."]
                                       [--strict] [--max N]
  python3 scripts/verify/rl22-paths.py --selftest
"""

import fnmatch
import importlib.util
import os
import sys
import tempfile

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
DEFAULT_RECORDED = os.path.join(ROOT, "scripts", "verify", "rl22-recorded.txt")


def load_report_module():
    p = os.path.join(os.path.dirname(os.path.abspath(__file__)), "suite-all-report.py")
    spec = importlib.util.spec_from_file_location("suite_all_report", p)
    m = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(m)
    return m


SAR = load_report_module()


def pairs_of(lane_names):
    """[(c lane, native lane)] for the lanes present"""
    have = set(lane_names)
    out = []
    for c in sorted(l for l in have if l.startswith("c:")):
        t = c[2:]
        for n in ("native:" + t, "native:" + t + "-pe"):
            if n in have:
                out.append((c, n))
    return out


def read_recorded(path):
    """[(pair pattern, test, reason, line number)]"""
    rec = []
    if not path or not os.path.exists(path):
        return rec
    with open(path, encoding="utf-8") as f:
        for i, line in enumerate(f, 1):
            s = line.strip()
            if not s or s.startswith("#"):
                continue
            parts = s.split(None, 2)
            if len(parts) < 3:
                rec.append(("", "", "", i))  # malformed: no reason
                continue
            rec.append((parts[0], parts[1], parts[2], i))
    return rec


def recorded_for(rec, c, n, t):
    key = c + "|" + n
    for pat, test, reason, i in rec:
        if test == t and pat and fnmatch.fnmatchcase(key, pat):
            return (pat, test, reason, i)
    return None


def compare(lanes):
    """per pair: {'c','n','c_fails','n_fails','both','only_c','only_n','compared'}"""
    by = {L["lane"]: L for L in lanes}
    verdicts = {L["lane"]: {t: v for v, t, _ in L["results"]} for L in lanes}
    out = []
    for c, n in pairs_of(by):
        vc, vn = verdicts[c], verdicts[n]
        P = {"c": c, "n": n, "c_fails": [], "n_fails": [], "both": [], "unpaired": [], "compared": 0}
        for t in sorted(set(vc) | set(vn)):
            a, b = vc.get(t), vn.get(t)
            real_a, real_b = a in ("PASS", "FAIL", "BUILD"), b in ("PASS", "FAIL", "BUILD")
            if not (real_a and real_b):
                # ran on one lane only (BUILT, or missing from a results.txt);
                # a SKIP is the lane's own choice (scripts/lane-skips.txt)
                if real_a != real_b and "SKIP" not in (a, b):
                    P["unpaired"].append((t, a or "-", b or "-"))
                continue
            P["compared"] += 1
            if a == "PASS" and b != "PASS":
                cause = SAR.fail_cause(by[n]["dir"], t) if b == "FAIL" else SAR.build_cause(by[n]["dir"], t)
                P["n_fails"].append((t, b, cause))
            elif b == "PASS" and a != "PASS":
                cause = SAR.fail_cause(by[c]["dir"], t) if a == "FAIL" else SAR.build_cause(by[c]["dir"], t)
                P["c_fails"].append((t, a, cause))
            elif a != "PASS" and b != "PASS":
                P["both"].append(t)
        out.append(P)
    return out


def analyse(out_dir, only=None, recorded=None):
    if not os.path.isdir(out_dir):
        return None, "no folder %s (run scripts/suite-all.sh first)" % out_dir
    lanes = SAR.collect(out_dir, only)
    if not lanes:
        return None, "no lane folder with a results.txt in %s" % out_dir
    pairs = compare(lanes)
    rec = read_recorded(recorded)
    used = set()
    open_n = 0
    for P in pairs:
        for key in ("c_fails", "n_fails"):
            marked = []
            for t, v, cause in P[key]:
                r = recorded_for(rec, P["c"], P["n"], t)
                if r:
                    used.add(r[3])
                else:
                    open_n += 1
                marked.append((t, v, cause, r))
            P[key] = marked
    lane_names = [L["lane"] for L in lanes]
    paired = {x for P in pairs for x in (P["c"], P["n"])}
    alone = [l for l in lane_names if l not in paired]
    stale = [r for r in rec if r[3] not in used and r[0]]
    malformed = [r for r in rec if not r[0]]
    return {"lanes": lanes, "pairs": pairs, "alone": alone, "open": open_n,
            "stale": stale, "malformed": malformed}, None


def render(d, compiler, maxn, md, recorded):
    o = []
    h1, h2, h3 = ("# ", "## ", "### ") if md else ("", "== ", "-- ")
    o.append(h1 + "RL22: the C path and our own backend, target by target")
    o.append("")
    if compiler:
        o.append("Compiler: " + compiler)
        o.append("")
    if md:
        o.append("| C path | own backend | compared | own backend fails | C path fails | both fail | open |")
        o.append("|---|---|---|---|---|---|---|")
    for P in d["pairs"]:
        op = sum(1 for k in ("c_fails", "n_fails") for x in P[k] if not x[3])
        if md:
            o.append("| `%s` | `%s` | %d | %d | %d | %d | %d |" % (
                P["c"], P["n"], P["compared"], len(P["n_fails"]), len(P["c_fails"]), len(P["both"]), op))
        else:
            o.append("%-18s %-26s %4d compared, %3d fail on the own backend only, %3d on the C path only, "
                     "%3d on both, %3d not recorded" % (P["c"], P["n"], P["compared"], len(P["n_fails"]),
                                                        len(P["c_fails"]), len(P["both"]), op))
    o.append("")
    if not d["pairs"]:
        o.append("No pair: the run has no target with both a C-path and an own-backend lane.")
        o.append("")
    if d["alone"]:
        o.append("Lanes without a partner (not compared here; AGREE=1 still compares them): "
                 + ", ".join(("`%s`" % l) if md else l for l in d["alone"]))
        o.append("")
    for P in d["pairs"]:
        for key, title in (("n_fails", "pass through C, fail on the own backend"),
                           ("c_fails", "pass on the own backend, fail through C")):
            if not P[key]:
                continue
            o.append(h3 + "%s vs %s: %d %s" % (P["c"], P["n"], len(P[key]), title))
            o.append("")
            for t, v, (k, det), r in P[key][:maxn]:
                tag = ("recorded (%s:%d): %s" % (os.path.basename(recorded or ""), r[3], r[2])) if r else "NOT RECORDED"
                verdict = "did not build" if v == "BUILD" else "failed"
                o.append("%s%s: %s, %s%s. %s" % ("- " if md else "  ", ("`%s`" % t) if md else t, verdict, k,
                                                (" (" + det + ")") if det and det not in k else "", tag))
            if len(P[key]) > maxn:
                o.append("%s... and %d more" % ("- " if md else "  ", len(P[key]) - maxn))
            o.append("")
        if P["unpaired"]:
            o.append("%s%s vs %s: %d tests ran on one lane only (BUILT, or missing from one results.txt): %s" % (
                "" if md else "  ", P["c"], P["n"], len(P["unpaired"]),
                ", ".join("%s (%s/%s)" % u for u in P["unpaired"][:maxn])))
            o.append("")
    if d["stale"]:
        o.append(h2 + "Stale records (the test agrees now, or is gone: remove the line)")
        o.append("")
        for pat, t, reason, i in d["stale"]:
            o.append("%sline %d: %s %s" % ("- " if md else "  ", i, pat, t))
        o.append("")
    if d["malformed"]:
        o.append("Malformed lines (need `<pair> <test> <reason>`): "
                 + ", ".join(str(r[3]) for r in d["malformed"]))
        o.append("")
    o.append("Open (neither fixed nor recorded): %d" % d["open"])
    o.append("")
    return "\n".join(o)


def run(out_dir, md_path=None, recorded=DEFAULT_RECORDED, compiler=None, only=None,
        strict=False, maxn=40, quiet=False):
    d, err = analyse(out_dir, only, recorded)
    if d is None:
        sys.stderr.write("rl22-paths: %s\n" % err)
        return 2, None
    if not quiet:
        sys.stdout.write(render(d, compiler, maxn, False, recorded))
    if md_path:
        with open(md_path, "w", encoding="utf-8", newline="\n") as f:
            f.write(render(d, compiler, maxn, True, recorded))
    bad = d["open"] > 0 or bool(d["malformed"]) or (strict and bool(d["stale"]))
    return (1 if bad else 0), d


# --- selftest ----------------------------------------------------------------

def selftest():
    bad = []
    with tempfile.TemporaryDirectory() as tmp:
        def lane(dirname, results, files=None):
            d = os.path.join(tmp, dirname)
            os.makedirs(d)
            with open(os.path.join(d, "results.txt"), "w") as f:
                for v, t in results:
                    f.write("%-5s %s\n" % (v, t))
            for name, text in (files or {}).items():
                with open(os.path.join(d, name), "w") as f:
                    f.write(text)

        def ref(t, out="1\n", rc="0"):
            return {t + ".expected": out, t + ".expected_rc": rc, t + ".expected_err": ""}

        # x86_64-linux: t_nat fails on native (output), t_c fails through C
        # (does not build), t_both fails on both, t_ok passes on both, t_rec
        # fails on native and is recorded, t_skip is skipped on native
        f_nat = dict(ref("t_nat"), **{"t_nat.actual": "2\n", "t_nat.actual_rc": "0", "t_nat.stderr": ""})
        f_rec = dict(ref("t_rec"), **{"t_rec.actual": "", "t_rec.actual_rc": "139", "t_rec.stderr": ""})
        f_both_n = dict(ref("t_both"), **{"t_both.actual": "x\n", "t_both.actual_rc": "0", "t_both.stderr": ""})
        lane("native-x86_64-linux-native",
             [("PASS", "t_ok"), ("FAIL", "t_nat"), ("PASS", "t_c"), ("FAIL", "t_both"),
              ("FAIL", "t_rec"), ("SKIP", "t_skip")], dict(f_nat, **dict(f_rec, **f_both_n)))
        f_c = {"t_c.build_rc": "1", "t_c.build.log": "error: use of undeclared identifier 'SO_ERROR'\n"}
        f_both_c = dict(ref("t_both"), **{"t_both.actual": "y\n", "t_both.actual_rc": "0", "t_both.stderr": ""})
        lane("c-x86_64-linux",
             [("PASS", "t_ok"), ("PASS", "t_nat"), ("BUILD", "t_c"), ("FAIL", "t_both"),
              ("PASS", "t_rec"), ("PASS", "t_skip")], dict(f_c, **f_both_c))
        # Windows: c:x86_64-windows pairs with both own-backend lanes
        lane("c-x86_64-windows", [("PASS", "w1"), ("PASS", "w2")])
        lane("native-x86_64-windows-pe-native", [("PASS", "w1"), ("PASS", "w2")])
        f_w = dict(ref("w2"), **{"w2.actual": "1\n", "w2.actual_rc": "3", "w2.stderr": ""})
        lane("native-x86_64-windows-native", [("PASS", "w1"), ("FAIL", "w2")], f_w)
        # a lane with no partner
        lane("c-wasm32-wasi", [("PASS", "t_ok")])
        rec = os.path.join(tmp, "rec.txt")
        with open(rec, "w") as f:
            f.write("# comment\n")
            f.write("c:*-linux|native:*-linux t_rec segfault on the own backend, miscompile-log M99\n")
            f.write("c:x86_64-windows|native:x86_64-windows-pe gone_test fixed long ago\n")

        st, d = run(tmp, recorded=rec, quiet=True)
        if d is None:
            print("rl22-paths selftest: no result")
            return 1
        names = [(P["c"], P["n"]) for P in d["pairs"]]
        want = [("c:x86_64-linux", "native:x86_64-linux"),
                ("c:x86_64-windows", "native:x86_64-windows"),
                ("c:x86_64-windows", "native:x86_64-windows-pe")]
        if sorted(names) != sorted(want):
            bad.append("pairs %s" % names)
        if d["alone"] != ["c:wasm32-wasi"]:
            bad.append("alone %s" % d["alone"])
        P = [p for p in d["pairs"] if p["n"] == "native:x86_64-linux"][0]
        nf = {t: (v, c, r) for t, v, c, r in P["n_fails"]}
        cf = {t: (v, c, r) for t, v, c, r in P["c_fails"]}
        if set(nf) != {"t_nat", "t_rec"}:
            bad.append("own-backend fails %s" % sorted(nf))
        if set(cf) != {"t_c"}:
            bad.append("C-path fails %s" % sorted(cf))
        if P["both"] != ["t_both"]:
            bad.append("both %s" % P["both"])
        if "t_skip" in nf or "t_skip" in cf:
            bad.append("a SKIP counted")
        if "t_nat" in nf and "output differs" not in nf["t_nat"][1][0]:
            bad.append("t_nat cause %s" % (nf["t_nat"][1],))
        if "t_rec" in nf and (not nf["t_rec"][2] or "crashed" not in nf["t_rec"][1][0]):
            bad.append("t_rec recorded/cause %s" % (nf["t_rec"],))
        if "t_c" in cf and "SO_ERROR" not in cf["t_c"][1][0] + cf["t_c"][1][1]:
            bad.append("t_c cause %s" % (cf["t_c"][1],))
        W = [p for p in d["pairs"] if p["n"] == "native:x86_64-windows"][0]
        if [x[0] for x in W["n_fails"]] != ["w2"] or "exit code" not in W["n_fails"][0][2][0]:
            bad.append("w2 %s" % W["n_fails"])
        if d["open"] != 3:   # t_nat, t_c, w2
            bad.append("open %d (want 3)" % d["open"])
        if st != 1:
            bad.append("status %d with open disagreements (want 1)" % st)
        if [r[1] for r in d["stale"]] != ["gone_test"]:
            bad.append("stale %s" % d["stale"])
        md = os.path.join(tmp, "r.md")
        run(tmp, md_path=md, recorded=rec, quiet=True)
        text = open(md, encoding="utf-8").read()
        for s in ("| `c:x86_64-linux` | `native:x86_64-linux` |", "NOT RECORDED", "recorded (rec.txt:2)",
                  "Stale records", "`c:wasm32-wasi`"):
            if s not in text:
                bad.append("markdown lacks %r" % s)
        # everything recorded: status 0; --strict with a stale line: 1
        with open(rec, "a") as f:
            f.write("c:*-linux|native:*-linux t_nat own backend prints 2, tracked as M98\n")
            f.write("c:*-linux|native:*-linux t_c WASI-like header gap, tracked\n")
            f.write("c:x86_64-windows|native:x86_64-windows w2 exit code, tracked\n")
        st2, d2 = run(tmp, recorded=rec, quiet=True)
        if st2 != 0 or d2["open"] != 0:
            bad.append("all recorded: status %d, open %d (want 0, 0)" % (st2, d2["open"]))
        st3, _ = run(tmp, recorded=rec, quiet=True, strict=True)
        if st3 != 1:
            bad.append("--strict with a stale record: status %d (want 1)" % st3)
        with open(rec, "a") as f:
            f.write("c:x86_64-linux|native:x86_64-linux t_ok\n")
        st4, d4 = run(tmp, recorded=rec, quiet=True)
        if st4 != 1 or not d4["malformed"]:
            bad.append("a line with no reason is not caught")
    for b in bad:
        print("rl22-paths selftest: " + b)
    if not bad:
        print("rl22-paths selftest: ok (pairs, causes, records, stale and malformed lines)")
    return 1 if bad else 0


def main(argv):
    if "--selftest" in argv:
        return selftest()
    out_dir = os.path.join(ROOT, "build", "suite-all")
    md = compiler = only = None
    recorded = DEFAULT_RECORDED
    strict = False
    maxn = 40
    i = 1
    while i < len(argv):
        a = argv[i]
        if a in ("--md", "--recorded", "--compiler", "--lanes", "--max") and i + 1 < len(argv):
            v = argv[i + 1]
            if a == "--md":
                md = v
            elif a == "--recorded":
                recorded = v
            elif a == "--compiler":
                compiler = v
            elif a == "--lanes":
                only = set(v.split()) or None
            else:
                maxn = int(v)
            i += 2
            continue
        if a == "--strict":
            strict = True
            i += 1
            continue
        if a.startswith("-"):
            sys.stderr.write(__doc__)
            return 2
        out_dir = a
        i += 1
    st, _ = run(out_dir, md, recorded, compiler, only, strict, maxn)
    return st


if __name__ == "__main__":
    sys.exit(main(sys.argv))
