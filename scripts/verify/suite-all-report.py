#!/usr/bin/env python3
"""suite-all-report.py - the record and first triage of a scripts/suite-all.sh
run (own-backend roadmap RL2: the suite on every backend and target).

scripts/suite-all.sh leaves one folder per lane under its OUT (default
build/suite-all): c-<target>/ for the C path, native-<target>-native/ for our
own backend, each holding cross-test.sh's results.txt and, per test, the
reference (<t>.expected, .expected_rc, .expected_err), the build (<t>.build_rc,
<t>.build.log, the binary) and the run (<t>.actual, .actual_rc, .stderr).
This script reads those folders only (no compiler, no target) and writes:

  1. one row per lane: passed, failed, did not build, skipped, built only;
  2. every failure and build failure with its cause, from the files above:
     a runtime name bound from a shared library ("symbol lookup error:
     undefined symbol: nat_tls_set", the riscv64 lane of Nov 16), a missing
     dynamic loader, a timeout, a crash (signal or Windows exception), a
     panic that differs, an exit code that differs, the first output line
     that differs; for a build, the first error or compiler panic of its log
     (a zig cache race and running out of memory are named: rerun those);
  3. the failing tests grouped by the set of lanes they fail on, with where
     to look first (every lane: the test or the interpreter's reference;
     every native lane and no C lane: the mid-end or nat_ir's common code;
     one native target: its selection or encoder; C lanes only: codegen_c
     or the C runtime; ...);
  4. for the Linux lanes, every built binary that imports one of the
     runtime's own names from a shared library (scripts/bin-target-check.py:
     a compiler bug even where the test passed or was skipped);
  5. the tests whose lanes disagree (a PASS on one lane, FAIL or BUILD on
     another: what AGREE=1 fails on).

Usage:
  python3 scripts/verify/suite-all-report.py [OUT] [--md FILE] [--title TEXT]
                                             [--compiler TEXT] [--max N]
                                             [--lanes "c:x86_64-linux ..."]
  python3 scripts/verify/suite-all-report.py --selftest
    OUT        suite-all.sh's OUT folder (default build/suite-all)
    --lanes    only these lanes (suite-all.sh passes the run's own: OUT may
               hold lane folders of an earlier run)
    --md FILE  also write the report as Markdown (docs/verification/
               rl2-suite-all-<date>.md is where a recorded run goes)
    --compiler which compiler ran (fid --version, the build's date)
    --max N    at most N tests listed per cause (default 30)
    --selftest made-up lane folders, each cause recognised (no compiler;
               tests/tooling/rl2-suite-report-check.sh runs it)

Exit status: 0 when every lane passed and no two lanes disagree, 1 otherwise,
2 on bad usage (no lane folder found).
"""

import importlib.util
import os
import re
import sys
import tempfile

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))


# --- reading a lane ---------------------------------------------------------

def lane_of_dir(name):
    """'c-x86_64-linux' -> ('c', 'x86_64-linux'); 'native-riscv64-linux-native'
    -> ('native', 'riscv64-linux'); anything else -> None"""
    if name.startswith("native-") and name.endswith("-native") and len(name) > 14:
        return "native", name[len("native-"):-len("-native")]
    if name.startswith("c-") and not name.endswith("-native"):
        return "c", name[2:]
    return None


def read(path, limit=200000):
    try:
        with open(path, "rb") as f:
            return f.read(limit).decode("utf-8", "replace").replace("\r\n", "\n")
    except OSError:
        return None


def read_rc(path):
    t = read(path)
    if t is None:
        return None
    t = t.strip()
    try:
        return int(t)
    except ValueError:
        return None


RES_RE = re.compile(r"^(PASS|FAIL|BUILD|SKIP|BUILT)\s+(\S+)\s*(.*)$")


def read_results(path):
    """[(verdict, test, rest)] of a results.txt, in its order"""
    t = read(path)
    out = []
    if t is None:
        return None
    for line in t.split("\n"):
        m = RES_RE.match(line.strip())
        if m:
            out.append((m.group(1), m.group(2), m.group(3)))
    return out


# --- causes ----------------------------------------------------------------

def short(s, n=100):
    s = s.strip()
    return s if len(s) <= n else s[:n - 3] + "..."


def panic_key(text):
    """'Panic: Integer overflow on addition' without operands or line"""
    if not text:
        return ""
    for line in text.split("\n"):
        if line.startswith("Panic:"):
            k = re.sub(r"^(Panic: [^:]*).*", r"\1", line.strip())
            return re.sub(r" at line [0-9]+$", "", k)
    return ""


WIN_EXC = {
    0xC0000005: "access violation",
    0xC00000FD: "stack overflow",
    0xC0000094: "integer division by zero",
    0xC0000095: "integer overflow",
    0xC000001D: "illegal instruction",
    0xC0000409: "stack buffer overrun / fail-fast",
    0x80000003: "breakpoint",
}
SIGNALS = {4: "SIGILL", 5: "SIGTRAP", 6: "SIGABRT", 7: "SIGBUS", 8: "SIGFPE",
           9: "SIGKILL", 11: "SIGSEGV", 13: "SIGPIPE", 15: "SIGTERM"}


def crash_of(rc):
    """a crash's name from an exit code, or None"""
    if rc is None:
        return None
    u = rc & 0xFFFFFFFF
    if u in WIN_EXC:
        return "Windows exception 0x%08X (%s)" % (u, WIN_EXC[u])
    if u >= 0xC0000000:
        return "Windows exception 0x%08X" % u
    if 128 < rc < 160 and (rc - 128) in SIGNALS:
        return "killed by %s (exit %d)" % (SIGNALS[rc - 128], rc)
    if rc < 0 and -rc in SIGNALS:
        return "killed by %s" % SIGNALS[-rc]
    return None


def first_diff(exp, act):
    """(line number, expected line, actual line) of the first difference"""
    e = (exp or "").split("\n")
    a = (act or "").split("\n")
    for i in range(max(len(e), len(a))):
        el = e[i] if i < len(e) else "<end of output>"
        al = a[i] if i < len(a) else "<end of output>"
        if el != al:
            return i + 1, el, al
    return 0, "", ""


def fail_cause(d, t):
    """(group key, detail) of a FAIL: the run differs from the interpreter"""
    p = os.path.join(d, t)
    err = read(p + ".stderr") or ""
    out = read(p + ".actual")
    exp = read(p + ".expected") or ""
    eerr = read(p + ".expected_err") or ""
    rc = read_rc(p + ".actual_rc")
    erc = read_rc(p + ".expected_rc")
    m = re.search(r"symbol lookup error:.*undefined symbol: (\S+)", err)
    if m:
        return ("runtime name bound from a shared library (compiler/nat_ir.fd "
                "ng_is_own_prim; scripts/bin-target-check.py)",
                "undefined symbol: %s" % m.group(1))
    if re.search(r"Could not open .*ld-|cannot open shared object|"
                 r"No such file or directory.*ld-linux|error while loading shared libraries", err):
        return ("dynamic loader or C library missing on the runner "
                "(QEMU_LD_PREFIX, libc6-<arch>-cross)", short(err.split("\n")[0]))
    if out is None and rc is None:
        return ("did not run (no output files: the runner failed to start it)", "")
    if rc == 124:
        return ("timed out (60 s under cross-test.sh)", "")
    crash = crash_of(rc)
    if crash and crash_of(erc) is None:
        pk = panic_key(err)
        return ("crashed: " + crash, short(pk or (err.split("\n")[0] if err else "")))
    pe, pa = panic_key(eerr), panic_key(err)
    if pe != pa:
        return ("panic differs from the interpreter's",
                "interpreter: %s / here: %s" % (pe or "(none)", pa or "(none)"))
    if (out or "") == exp and rc != erc:
        return ("exit code differs, output the same", "rc %s, interpreter %s" % (rc, erc))
    n, el, al = first_diff(exp, out)
    if n:
        return ("output differs from the interpreter's",
                "line %d: want %r, got %r%s" % (n, short(el, 60), short(al, 60),
                                                "" if rc == erc else " (rc %s, interpreter %s)" % (rc, erc)))
    return ("differs (output equal after reading: look at results.txt)", "rc %s, interpreter %s" % (rc, erc))


ERR_RE = re.compile(r"(?i)\b(error|undefined|unresolved|cannot|failed|panic)\b")


def norm(line):
    """an error line without paths and numbers, for grouping"""
    s = re.sub(r"(?:[A-Za-z]:)?[^\s:'\"`]*[\\/][^\s:'\"`]*", "<path>", line)
    s = re.sub(r"\b\d+\b", "N", s)
    return short(s, 90)


def build_cause(d, t):
    """(group key, detail) of a BUILD: the test did not build on this lane"""
    p = os.path.join(d, t)
    rc = read_rc(p + ".build_rc")
    log = read(p + ".build.log") or ""
    if rc == 124:
        return ("build timed out (600 s)", "")
    if "CacheCheckFailed" in log or re.search(r"zig.*cache", log, re.I):
        return ("zig cache race (parallel first builds): rerun the lane with REUSE=1", "")
    if re.search(r"(?i)out of memory|OutOfMemory|cannot allocate memory|std::bad_alloc", log):
        return ("ran out of memory while building: rerun with fewer JOBS", "")
    lines = [l.strip() for l in log.split("\n") if l.strip()]
    for l in lines:
        if l.startswith("Panic:") or "panicked" in l:
            return ("compiler panic: " + norm(l), short(l, 140))
    for l in lines:
        if l.startswith("[") and not ERR_RE.search(l):
            continue  # progress lines ([build], [prof])
        if ERR_RE.search(l):
            return ("build error: " + norm(l), short(l, 140))
    crash = crash_of(rc)
    if crash:
        return ("compiler crashed: " + crash, "")
    if rc is None:
        return ("not built (no build_rc: the lane stopped before this test)", "")
    return ("build failed with no error line (rc %s)" % rc, short(lines[-1], 140) if lines else "")


# --- where to look ----------------------------------------------------------

def pattern_of(fail_lanes, all_lanes):
    """a name for the set of lanes a test fails on, and where to look first"""
    f = set(fail_lanes)
    run = set(all_lanes)
    c = {l for l in run if l.startswith("c:")}
    nat = {l for l in run if l.startswith("native:")}
    natl = {l for l in nat if "-linux" in l}
    if f == run and len(run) > 1:
        return ("every lane", "the test itself or the interpreter's reference "
                "(fid run): compare with the test's Expected output")
    if nat and f == nat and len(f) > 1:
        return ("every native lane, no C lane", "the mid-end (iropt, rules, "
                "egraph: try scripts/pass-matrix.sh on the test) or code nat_ir "
                "and pe_ir share (lowering for emit, core's Vec)")
    if natl and f == natl and len(f) > 1:
        return ("every native Linux lane", "compiler/nat_ir.fd's common code or "
                "stdlib/linux_runtime (scripts/pass-matrix.sh <target> <test>)")
    if c and f == c and len(f) > 1:
        return ("every C lane, no native lane", "compiler/codegen_c.fd or the C "
                "runtime it writes")
    if f <= c and len(f) == 1:
        lane = next(iter(f))
        if "wasm" in lane:
            return ("C to wasm32-wasi only", "the WASI part of the C runtime "
                    "(codegen_c.fd #ifdef __wasi__) or scripts/wasm-run.mjs")
        if "windows" in lane:
            return ("C on Windows only", "the Windows part of the C runtime")
        return ("one C target: " + lane, "zig's libc for that target, or "
                "target-dependent C (sizes, alignment)")
    if f <= nat and len(f) == 1:
        lane = next(iter(f))
        if lane == "native:x86_64-windows-pe":
            return ("pe_ir only (default emit on Windows)", "compiler/pe_ir*.fd "
                    "or stdlib/pe_runtime")
        if lane == "native:x86_64-windows":
            return ("emit --nat only", "nat_ir's Windows part: "
                    "stdlib/linux_runtime/win.fd, the PE writer of nat_ir")
        return ("one native target: " + lane, "that target's selection and "
                "encoder (compiler/nat_ir.fd, nat_enc.fd: FID_VERIFY=1, "
                "scripts/enc-oracle.py)")
    return ("lanes: " + " ".join(sorted(f)), "compare the lanes' outputs")


# --- runtime imports of Linux binaries --------------------------------------

def bin_checker():
    path = os.path.join(ROOT, "scripts", "bin-target-check.py")
    try:
        spec = importlib.util.spec_from_file_location("bin_target_check", path)
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        return mod
    except (OSError, ImportError, SyntaxError, AttributeError):
        return None


def runtime_imports_of_lane(d, results):
    """[(test, [names])] of the lane's binaries importing runtime names"""
    mod = bin_checker()
    if mod is None:
        return []
    out = []
    for verdict, t, _rest in results:
        if verdict == "BUILD":
            continue
        b = os.path.join(d, t + ".bin")
        if not os.path.isfile(b):
            continue
        try:
            with open(b, "rb") as f:
                data = f.read()
        except OSError:
            continue
        if data[:4] != b"\x7fELF":
            continue
        names = mod.runtime_imports(data)
        if names:
            out.append((t, names))
    return out


# --- the report -------------------------------------------------------------

def collect(out_dir, only=None):
    lanes = []
    for name in sorted(os.listdir(out_dir)):
        d = os.path.join(out_dir, name)
        lo = lane_of_dir(name)
        if not lo or not os.path.isdir(d):
            continue
        if only and "%s:%s" % lo not in only:
            continue  # a lane of an earlier run in the same OUT
        res = read_results(os.path.join(d, "results.txt"))
        if res is None:
            continue
        kind, target = lo
        lanes.append({"lane": "%s:%s" % (kind, target), "dir": d, "results": res})
    return lanes


def analyse(lanes):
    for L in lanes:
        n = {"PASS": 0, "FAIL": 0, "BUILD": 0, "SKIP": 0, "BUILT": 0}
        causes = {}
        for verdict, t, rest in L["results"]:
            n[verdict] += 1
            if verdict == "FAIL":
                k, det = fail_cause(L["dir"], t)
            elif verdict == "BUILD":
                k, det = build_cause(L["dir"], t)
            else:
                continue
            causes.setdefault(k, []).append((t, det))
        L["n"] = n
        L["causes"] = causes
        L["imports"] = runtime_imports_of_lane(L["dir"], L["results"]) if "-linux" in L["lane"] else []
    # test -> lanes it fails on / passes on (SKIP and BUILT do not count)
    fails, passes, ran = {}, {}, {}
    for L in lanes:
        for verdict, t, _ in L["results"]:
            if verdict in ("SKIP", "BUILT"):
                continue
            ran.setdefault(t, []).append(L["lane"])
            if verdict == "PASS":
                passes.setdefault(t, []).append(L["lane"])
            else:
                fails.setdefault(t, []).append(L["lane"])
    groups = {}
    for t, fl in fails.items():
        name, hint = pattern_of(fl, ran[t])
        groups.setdefault((name, hint), []).append(t)
    disagree = sorted(t for t in fails if t in passes)
    return groups, disagree, fails, passes


def render(lanes, groups, disagree, fails, title, compiler, maxn, md):
    o = []
    h1, h2 = ("# ", "## ") if md else ("", "== ")
    o.append(h1 + (title or "Suite on every lane (RL2)"))
    o.append("")
    if compiler:
        o.append("Compiler: " + compiler)
        o.append("")
    if md:
        o.append("| lane | passed | failed | did not build | skipped | built only |")
        o.append("|---|---|---|---|---|---|")
    for L in lanes:
        n = L["n"]
        if md:
            o.append("| `%s` | %d | %d | %d | %d | %d |" % (L["lane"], n["PASS"], n["FAIL"], n["BUILD"], n["SKIP"], n["BUILT"]))
        else:
            o.append("%-28s %4d passed, %3d failed, %3d did not build, %3d skipped, %3d built only"
                     % (L["lane"], n["PASS"], n["FAIL"], n["BUILD"], n["SKIP"], n["BUILT"]))
    o.append("")
    bad_lanes = [L for L in lanes if L["causes"]]
    o.append(h2 + "Failures by cause")
    o.append("")
    if not bad_lanes:
        o.append("No lane has a failure or a build failure.")
        o.append("")
    for L in bad_lanes:
        o.append(("### `%s`" % L["lane"]) if md else ("-- " + L["lane"]))
        o.append("")
        for k in sorted(L["causes"], key=lambda k: (-len(L["causes"][k]), k)):
            ts = L["causes"][k]
            o.append("%s%s: %d" % ("- " if md else "  ", k, len(ts)))
            for t, det in ts[:maxn]:
                o.append("%s%s%s" % ("  - " if md else "      ", ("`%s`" % t) if md else t, (": " + det) if det else ""))
            if len(ts) > maxn:
                o.append("%s... and %d more" % ("  - " if md else "      ", len(ts) - maxn))
        o.append("")
    o.append(h2 + "Failing tests by the lanes they fail on")
    o.append("")
    if not groups:
        o.append("None.")
        o.append("")
    for (name, hint) in sorted(groups, key=lambda g: (-len(groups[g]), g[0])):
        ts = sorted(groups[(name, hint)])
        o.append("%s%s: %d test%s. Look first at: %s." % ("- " if md else "  ", name, len(ts), "" if len(ts) == 1 else "s", hint))
        shown = ", ".join(("`%s`" % t) if md else t for t in ts[:maxn])
        o.append("%s%s%s" % ("  " if md else "      ", shown, " ..." if len(ts) > maxn else ""))
    o.append("")
    imp = [L for L in lanes if L["imports"]]
    o.append(h2 + "Linux binaries importing the runtime's own names")
    o.append("")
    if not imp:
        o.append("None (scripts/bin-target-check.py over every Linux lane's binaries).")
        o.append("")
    for L in imp:
        names = {}
        for t, ns in L["imports"]:
            for x in ns:
                names[x] = names.get(x, 0) + 1
        top = ", ".join("%s (%d)" % (x, c) for x, c in sorted(names.items(), key=lambda kv: (-kv[1], kv[0]))[:12])
        o.append("%s%s: %d binaries; names: %s" % ("- " if md else "  ", ("`%s`" % L["lane"]) if md else L["lane"], len(L["imports"]), top))
        o.append("%sA runtime primitive bound by ld.so is a compiler bug (compiler/nat_ir.fd, the CI2 branch of an `extern \"C\"` call; ng_is_own_prim)." % ("  " if md else "      "))
    o.append("")
    o.append(h2 + "Tests whose lanes disagree (AGREE=1)")
    o.append("")
    if not disagree:
        o.append("None: every test passes on all its lanes or fails on all of them.")
    for t in disagree[:max(maxn, 100)]:
        o.append("%s%s: fails on %s" % ("- " if md else "  ", ("`%s`" % t) if md else t, " ".join(sorted(fails[t]))))
    o.append("")
    return "\n".join(o)


def report(out_dir, md_path=None, title=None, compiler=None, maxn=30, quiet=False, only=None):
    if not os.path.isdir(out_dir):
        sys.stderr.write("suite-all-report: no folder %s (run scripts/suite-all.sh first)\n" % out_dir)
        return 2, None
    lanes = collect(out_dir, only)
    if not lanes:
        sys.stderr.write("suite-all-report: no lane folder with a results.txt in %s\n" % out_dir)
        return 2, None
    groups, disagree, fails, passes = analyse(lanes)
    text = render(lanes, groups, disagree, fails, title, compiler, maxn, False)
    if not quiet:
        sys.stdout.write(text)
    if md_path:
        with open(md_path, "w", encoding="utf-8", newline="\n") as f:
            f.write(render(lanes, groups, disagree, fails, title, compiler, maxn, True))
    bad = any(L["causes"] for L in lanes) or bool(disagree) or any(L["imports"] for L in lanes)
    return (1 if bad else 0), {"lanes": lanes, "groups": groups, "disagree": disagree}


# --- selftest ----------------------------------------------------------------

def selftest():
    """made-up lanes; every cause and pattern must be recognised"""
    tmp = tempfile.mkdtemp(prefix="suite-report-")

    def lane(name, results, files):
        d = os.path.join(tmp, name)
        os.makedirs(d, exist_ok=True)
        with open(os.path.join(d, "results.txt"), "w", newline="\n") as f:
            f.write(results)
        for fn, content in files.items():
            mode = "wb" if isinstance(content, bytes) else "w"
            with open(os.path.join(d, fn), mode) as f:
                f.write(content)

    ref = {"t_out.expected": "1\n2\n", "t_out.expected_rc": "0\n",
           "t_imp.expected": "hi\n", "t_imp.expected_rc": "0\n",
           "t_all.expected": "x\n", "t_all.expected_rc": "0\n",
           "t_rc.expected": "a\n", "t_rc.expected_rc": "0\n",
           "t_pan.expected": "", "t_pan.expected_rc": "101\n",
           "t_pan.expected_err": "Panic: Integer overflow on addition: 1 + 2\n"}

    def with_ref(d):
        x = dict(ref)
        x.update(d)
        return x

    # the C lanes pass t_out, t_imp, t_rc, t_pan; fail t_all
    for tg in ("x86_64-linux", "riscv64-linux"):
        lane("c-" + tg,
             "PASS  t_out\nPASS  t_imp\nPASS  t_rc\nPASS  t_pan\nFAIL  t_all (rc 0, interpreter 0)\nSKIP  t_skip (no reference)\n",
             with_ref({"t_all.actual": "y\n", "t_all.actual_rc": "0\n", "t_all.stderr": ""}))
    elf = None
    try:
        spec = importlib.util.spec_from_file_location(
            "btc_selftest", os.path.join(ROOT, "tests", "tooling", "bin-target-check-selftest.py"))
        m = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(m)
        elf = m.make_elf("runtime", "aarch64")
    except (OSError, ImportError, AttributeError, SyntaxError):
        elf = None
    rv = with_ref({
        "t_imp.actual": "", "t_imp.actual_rc": "127\n",
        "t_imp.stderr": "/x/t_imp.bin: symbol lookup error: /x/t_imp.bin: undefined symbol: nat_tls_set\n",
        "t_out.actual": "1\n3\n", "t_out.actual_rc": "0\n", "t_out.stderr": "",
        "t_all.actual": "z\n", "t_all.actual_rc": "0\n", "t_all.stderr": "",
        "t_rc.actual": "a\n", "t_rc.actual_rc": "139\n", "t_rc.stderr": "",
        "t_pan.actual": "", "t_pan.actual_rc": "101\n", "t_pan.stderr": "Panic: Index out of bounds\n",
        "t_bld.build_rc": "1\n", "t_bld.build.log": "[build] Parsing...\nPanic: Vec index out of bounds: 7 >= 3\n",
        "t_zig.build_rc": "1\n", "t_zig.build.log": "error: CacheCheckFailed\n",
    })
    if elf:
        rv["t_out.bin"] = elf
    lane("native-riscv64-linux-native",
         "FAIL  t_imp (rc 127, interpreter 0)\nFAIL  t_out (rc 0, interpreter 0)\nFAIL  t_all (rc 0, interpreter 0)\n"
         "FAIL  t_rc (rc 139, interpreter 0)\nFAIL  t_pan (rc 101, interpreter 101)\nBUILD t_bld\nBUILD t_zig\n",
         rv)
    lane("native-x86_64-linux-native",
         "PASS  t_imp\nFAIL  t_out (rc 0, interpreter 0)\nFAIL  t_all (rc 0, interpreter 0)\nPASS  t_rc\nPASS  t_pan\n",
         with_ref({"t_out.actual": "1\n3\n", "t_out.actual_rc": "0\n", "t_out.stderr": "",
                   "t_all.actual": "z\n", "t_all.actual_rc": "0\n", "t_all.stderr": ""}))
    os.makedirs(os.path.join(tmp, "unrelated"), exist_ok=True)
    md = os.path.join(tmp, "report.md")
    st, data = report(tmp, md_path=md, title="selftest", quiet=True)
    bad = []
    if st != 1 or data is None:
        bad.append("status %s, want 1" % st)
        data = data or {"lanes": [], "groups": {}, "disagree": []}
    names = [L["lane"] for L in data["lanes"]]
    if names != ["c:riscv64-linux", "c:x86_64-linux", "native:riscv64-linux", "native:x86_64-linux"]:
        bad.append("lanes %s" % names)
    rv_l = [L for L in data["lanes"] if L["lane"] == "native:riscv64-linux"]
    causes = rv_l[0]["causes"] if rv_l else {}
    flat = {t: (k, det) for k, ts in causes.items() for t, det in ts}

    def want(t, key_part, det_part=""):
        if t not in flat:
            bad.append("%s: no cause" % t)
        elif key_part not in flat[t][0] or det_part not in flat[t][1]:
            bad.append("%s: cause %r / %r, want %r / %r" % (t, flat[t][0], flat[t][1], key_part, det_part))

    want("t_imp", "shared library", "nat_tls_set")
    want("t_out", "output differs", "line 2")
    want("t_rc", "SIGSEGV")
    want("t_pan", "panic differs", "Index out of bounds")
    want("t_bld", "compiler panic", "Vec index out of bounds")
    want("t_zig", "zig cache race")
    pats = {t: name for (name, _h), ts in data["groups"].items() for t in ts}
    if pats.get("t_all") != "every lane":
        bad.append("t_all pattern %r, want 'every lane'" % pats.get("t_all"))
    if pats.get("t_out") != "every native lane, no C lane":
        bad.append("t_out pattern %r" % pats.get("t_out"))
    if pats.get("t_imp") != "one native target: native:riscv64-linux":
        bad.append("t_imp pattern %r" % pats.get("t_imp"))
    if set(data["disagree"]) != {"t_imp", "t_out", "t_rc", "t_pan"}:
        bad.append("disagree %s" % data["disagree"])
    if elf is not None:
        imps = rv_l[0]["imports"] if rv_l else []
        if imps != [("t_out", ["nat_tls_set"])]:
            bad.append("runtime imports %s, want t_out: nat_tls_set" % imps)
    if crash_of(0xC0000005) is None or crash_of(-1073741819) is None:
        bad.append("Windows access violation not recognised")
    t = read(md) or ""
    if "| `native:riscv64-linux` | 0 | 5 | 2 | 0 | 0 |" not in t:
        bad.append("markdown lane row missing")
    # a clean run: status 0
    tmp2 = tempfile.mkdtemp(prefix="suite-report-ok-")
    os.makedirs(os.path.join(tmp2, "c-x86_64-linux"))
    with open(os.path.join(tmp2, "c-x86_64-linux", "results.txt"), "w") as f:
        f.write("PASS  a\nPASS  b\nSKIP  c (no reference)\n")
    st2, _ = report(tmp2, quiet=True)
    if st2 != 0:
        bad.append("a clean run gave status %s" % st2)
    # --lanes: only the run's own lanes count (the failing ones are left out)
    st3, d3 = report(tmp, quiet=True, only={"c:x86_64-linux"})
    if d3 is None or [L["lane"] for L in d3["lanes"]] != ["c:x86_64-linux"]:
        bad.append("--lanes did not keep only c:x86_64-linux")
    for b in bad:
        print("suite-all-report selftest: " + b)
    if not bad:
        print("suite-all-report selftest: ok (%d causes, %d lane patterns, runtime imports %s)"
              % (len(flat), len(data["groups"]), "checked" if elf is not None else "not checked (no ELF maker)"))
    return 1 if bad else 0


def main(argv):
    if "--selftest" in argv:
        return selftest()
    out_dir = os.path.join(ROOT, "build", "suite-all")
    md = title = compiler = None
    only = None
    maxn = 30
    i = 1
    while i < len(argv):
        a = argv[i]
        if a in ("--md", "--title", "--compiler", "--max", "--lanes") and i + 1 < len(argv):
            v = argv[i + 1]
            if a == "--lanes":
                only = set(v.split()) or None
            elif a == "--md":
                md = v
            elif a == "--title":
                title = v
            elif a == "--compiler":
                compiler = v
            else:
                maxn = int(v)
            i += 2
            continue
        if a.startswith("-"):
            sys.stderr.write(__doc__)
            return 2
        out_dir = a
        i += 1
    st, _ = report(out_dir, md, title, compiler, maxn, only=only)
    return st


if __name__ == "__main__":
    sys.exit(main(sys.argv))
