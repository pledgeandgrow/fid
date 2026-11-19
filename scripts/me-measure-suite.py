#!/usr/bin/env python3
"""me-measure-suite.py: every opt-in (and default-on) mid-end pass measured
on the whole suite, one docs/perf/mid-end/optin-<pass>.txt per pass
(docs/roadmap/llvm-roadmap.md ME30). scripts/me-measure-all.sh runs this.

What one file holds is what scripts/me-measure.sh writes for one change: each
program of the suite built natively (`fid emit`, FID_IROPT_STATS=1) with the
pass on and with it off; the IR instructions after iropt, iropt's time and the
whole build's time of both, and whether the two programs print the same.

What makes a suite run fit in a night instead of days:

  * One build is shared. For a pass that is off by default (an opt-in pass,
    turned on with FID_IROPT_ON=<pass>), the "off" build of
    `FID_IROPT_ON=<pass> OFF=<pass>` (what me-measure-all.sh used to run) is
    the default build: compiler/iropt_me.fd `ime_on` gives false for it
    either way. For a pass in IME_DEFAULT_ON, the "on" build is the default
    build. So each program is built once with no FID_IROPT_ON/OFF ("default")
    and once per pass with the non-default side: N + P x N builds instead of
    2 x P x N. `--check-share K` builds the first K programs with
    FID_IROPT_ON=<p> FID_IROPT_OFF=<p> as well and checks that this gives the
    default build's instruction count and output (the claim above).
  * --jobs N builds N programs at once. Instruction counts do not depend on
    it; times do (compare times only within one run, at one --jobs).
  * Each build's result is kept in the cache (build/me-measure/<compiler
    hash>/), so a run that is stopped goes on where it was, and
    --report-only writes the files from what is there. A different compiler
    has another hash, so its results never mix with an older one's.

The files are written after the default builds and again after each pass, so
a run stopped half-way still leaves complete files for the passes it did.
Besides the per-pass files it writes docs/perf/mid-end/optin-suite.md, the
table for docs/perf/mid-end/README.md.

  python scripts/me-measure-suite.py --fid release/fid.exe --jobs 4
  python scripts/me-measure-suite.py --fid release/fid.exe --passes "inline2 spec" \
      tests/bootstrap/test_counted_loop.fd
  python scripts/me-measure-suite.py --fid release/fid.exe --report-only
"""
import argparse
import concurrent.futures
import glob
import hashlib
import json
import os
import re
import subprocess
import sys
import tempfile
import time

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
# the programs scripts/me-measure.sh takes with no files: output and no
# failure expected, no network or devices
EXCLUDE = ["_fail", "sqlite", "sdl", "socket", "http", "_ws", "bare", "bench_"]
# --build-cwd: the directory fid runs in (default the repository, so its
# stdlib/ is the one used). fid takes stdlib/ from its working directory
# before FID_HOME: set it elsewhere, with FID_HOME, to measure another stdlib.
BUILD_CWD = [""]


def suite_files():
    fs = sorted(glob.glob(os.path.join("tests", "bootstrap", "*.fd")))
    return [f.replace(os.sep, "/") for f in fs if not any(x in os.path.basename(f) for x in EXCLUDE)]


def compiler_passes():
    """The names compiler/*.fd tests with `ime_on("...")`, sorted."""
    names = set()
    for f in glob.glob(os.path.join("compiler", "*.fd")):
        with open(f, encoding="utf-8", errors="replace") as h:
            names.update(re.findall(r'ime_on\("([a-z0-9_]+)"\)', h.read()))
    return sorted(names)


def default_on():
    """The passes of IME_DEFAULT_ON (compiler/iropt_me.fd)."""
    p = os.path.join("compiler", "iropt_me.fd")
    try:
        text = open(p, encoding="utf-8", errors="replace").read()
    except OSError:
        return set()
    m = re.search(r'const\s+IME_DEFAULT_ON\s*:\s*str\s*=\s*"([^"]*)"', text)
    return {x for x in m.group(1).split(",") if x} if m else set()


def file_hash(path):
    h = hashlib.sha1()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()[:12]


def variant_env(variant):
    """The environment of one build. variant: default, on:<p>, off:<p>, both:<p>."""
    env = dict(os.environ)
    env.pop("FID_IROPT_ON", None)
    env.pop("FID_IROPT_OFF", None)
    env["FID_IROPT_STATS"] = "1"
    kind, _, p = variant.partition(":")
    if kind in ("on", "both"):
        env["FID_IROPT_ON"] = p
    if kind in ("off", "both"):
        env["FID_IROPT_OFF"] = p
    return env


def cache_path(cache, variant, prog):
    return os.path.join(cache, variant.replace(":", "_"), os.path.basename(prog)[:-3] + ".json")


def last_int(pattern, text, group):
    m = re.findall(pattern, text)
    if not m:
        return None
    v = m[-1]
    return int(v[group] if isinstance(v, tuple) else v)


def build_one(fid, cache, variant, prog, tmp):
    """Build and run one program with one variant; keep the result in the cache."""
    out_json = cache_path(cache, variant, prog)
    if os.path.exists(out_json):
        return False
    os.makedirs(os.path.dirname(out_json), exist_ok=True)
    name = os.path.basename(prog)[:-3]
    exe = os.path.join(tmp, f"{name}.{variant.replace(':', '_')}.exe")
    t0 = time.time()
    try:
        r = subprocess.run([fid, "emit", os.path.abspath(prog), "-o", exe], env=variant_env(variant),
                           stdin=subprocess.DEVNULL, capture_output=True, timeout=600,
                           cwd=BUILD_CWD[0] or None)
        log = (r.stdout + r.stderr).decode("utf-8", "replace")
    except subprocess.TimeoutExpired:
        log = "build timeout (600 s)"
    ms = int((time.time() - t0) * 1000)
    if os.path.exists(exe):
        try:
            rr = subprocess.run([exe], stdin=subprocess.DEVNULL, capture_output=True, timeout=60)
            out = (rr.stdout + rr.stderr).decode("utf-8", "replace") + f"exit {rr.returncode}\n"
        except subprocess.TimeoutExpired:
            out = "run timeout (60 s)\n"
        except OSError as e:
            out = f"cannot run: {e}\n"
    else:
        out = "no build\n"
    for e in (exe, exe[:-4] + ".pdb"):
        try:
            os.remove(e)
        except OSError:
            pass
    res = {
        "prog": prog,
        "variant": variant,
        "insts": last_int(r"instructions (\d+) -> (\d+)", log, 1),
        "iropt_ms": last_int(r"instructions \d+ -> \d+ in (\d+) ms", log, 0),
        "build_ms": ms,
        "built": out != "no build\n",
        "out_sha": hashlib.sha1(out.encode("utf-8")).hexdigest(),
        "out_head": out[:400],
        "log_tail": log[-600:] if out == "no build\n" else "",
    }
    tmpj = out_json + ".tmp"
    with open(tmpj, "w", encoding="utf-8") as f:
        json.dump(res, f)
    os.replace(tmpj, out_json)
    return True


def load(cache, variant, prog):
    p = cache_path(cache, variant, prog)
    if not os.path.exists(p):
        return None
    with open(p, encoding="utf-8") as f:
        return json.load(f)


def sides(p, on_default):
    """(variant of the "on" build, variant of the "off" build) of pass p."""
    return ("default", f"off:{p}") if p in on_default else (f"on:{p}", "default")


def num(v):
    return "-" if v is None else str(v)


def write_pass_file(out_dir, cache, p, on_default, files, fid, jobs):
    von, voff = sides(p, on_default)
    rows, tot = [], dict(n=0, ion=0, ioff=0, mon=0, moff=0, bon=0, boff=0, diff=0, nob=0)
    diffs = []
    for f in files:
        a, b = load(cache, von, f), load(cache, voff, f)
        if a is None or b is None:
            continue
        name = os.path.basename(f)[:-3]
        same = "same" if a["out_sha"] == b["out_sha"] else "DIFF"
        if same == "DIFF":
            tot["diff"] += 1
            diffs.append(name)
        if a["built"] != b["built"]:
            tot["nob"] += 1
        rows.append(f"{name} {num(a['insts'])} {num(b['insts'])} {num(a['iropt_ms'])} {num(b['iropt_ms'])} "
                    f"{a['build_ms']} {b['build_ms']} {same}")
        tot["n"] += 1
        tot["ion"] += a["insts"] or 0
        tot["ioff"] += b["insts"] or 0
        tot["mon"] += a["iropt_ms"] or 0
        tot["moff"] += b["iropt_ms"] or 0
        tot["bon"] += a["build_ms"]
        tot["boff"] += b["build_ms"]
    if tot["n"] == 0:
        return None
    kind = "on by default (IME_DEFAULT_ON): on = default build, off = FID_IROPT_OFF=" + p \
        if p in on_default else "opt-in: on = FID_IROPT_ON=" + p + ", off = default build"
    path = os.path.join(out_dir, f"optin-{p}.txt")
    with open(path, "w", encoding="utf-8", newline="\n") as h:
        h.write(f"# optin-{p}: iropt with {p} on vs off ({time.strftime('%Y-%m-%d')}, {os.path.basename(fid)}, "
                f"compiler {file_hash(fid)}, suite run: scripts/me-measure-suite.py --jobs {jobs})\n")
        h.write(f"# {kind}\n")
        h.write("# program  instructions(on)  instructions(off)  iropt-ms(on)  iropt-ms(off)  "
                "build-ms(on)  build-ms(off)  same-output\n")
        for r in rows:
            h.write(r + "\n")
        h.write(f"# totals over {tot['n']} programs: instructions {tot['ion']} (on) vs {tot['ioff']} (off); "
                f"iropt {tot['mon']} ms vs {tot['moff']} ms; builds {tot['bon']} ms vs {tot['boff']} ms; "
                f"{tot['diff']} with different output\n")
    tot["diffs"] = diffs
    return tot


def write_reports(out_dir, cache, passes, on_default, files, fid, jobs, share_bad):
    os.makedirs(out_dir, exist_ok=True)
    totals = {}
    for p in passes:
        t = write_pass_file(out_dir, cache, p, on_default, files, fid, jobs)
        if t is not None:
            totals[p] = t
    nbuilt = sum(1 for f in files if (load(cache, "default", f) or {}).get("built"))
    path = os.path.join(out_dir, "optin-suite.md")
    with open(path, "w", encoding="utf-8", newline="\n") as h:
        h.write("# Mid-end passes on the suite (ME30)\n\n")
        h.write(f"Written by `scripts/me-measure-suite.py` on {time.strftime('%Y-%m-%d %H:%M')}: compiler "
                f"`{os.path.basename(fid)}` (sha1 {file_hash(fid)}), `fid emit`, {len(files)} programs of "
                f"the suite ({nbuilt} built with the default passes), `--jobs {jobs}` (times are comparable "
                f"within this run only). On / off per pass; a pass marked * is on by default, so its \"on\" "
                f"column is the default build. Each line is `docs/perf/mid-end/optin-<pass>.txt`.\n\n")
        h.write("| Pass | Programs | Instructions on / off | Change | iropt ms on / off | Build ms on / off | "
                "Different output | Built on one side only |\n")
        h.write("|---|---|---|---|---|---|---|---|\n")
        for p in passes:
            t = totals.get(p)
            if t is None:
                h.write(f"| {p}{'*' if p in on_default else ''} | not run | | | | | | |\n")
                continue
            ch = "-" if t["ioff"] == 0 else f"{100.0 * (t['ion'] - t['ioff']) / t['ioff']:+.2f}%"
            d = str(t["diff"]) + (" (" + ", ".join(t["diffs"][:8]) + (", ..." if len(t["diffs"]) > 8 else "") + ")"
                                  if t["diffs"] else "")
            h.write(f"| {p}{'*' if p in on_default else ''} | {t['n']} | {t['ion']} / {t['ioff']} | {ch} | "
                    f"{t['mon']} / {t['moff']} | {t['bon']} / {t['boff']} | {d} | {t['nob']} |\n")
        if share_bad is not None:
            h.write("\nShared-build check (`--check-share`): ")
            h.write("every `FID_IROPT_ON=<p> FID_IROPT_OFF=<p>` build gave the default build's instructions "
                    "and output.\n" if not share_bad else
                    "these builds differ from the default build, so their passes' shared side is not exact: "
                    + ", ".join(share_bad) + "\n")
    return totals


def main():
    os.chdir(ROOT)
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("files", nargs="*", help="programs (default: the suite, as scripts/me-measure.sh)")
    ap.add_argument("--fid", default=os.path.join("release", "fid.exe" if os.name == "nt" else "fid"))
    ap.add_argument("--passes", default=os.environ.get("PASSES", ""),
                    help="space or comma list (default: every ime_on name of compiler/*.fd)")
    ap.add_argument("--jobs", type=int, default=int(os.environ.get("JOBS", "1") or 1))
    ap.add_argument("--cache", default="", help="default build/me-measure/<compiler hash>")
    ap.add_argument("--out-dir", default=os.path.join("docs", "perf", "mid-end"))
    ap.add_argument("--check-share", type=int, default=0, metavar="K",
                    help="also build the first K programs with FID_IROPT_ON=p FID_IROPT_OFF=p per pass "
                         "and check they equal the default build")
    ap.add_argument("--report-only", action="store_true", help="write the files from the cache, build nothing")
    ap.add_argument("--build-cwd", default="", help="directory fid runs in (default: the repository)")
    a = ap.parse_args()
    if a.build_cwd:
        BUILD_CWD[0] = os.path.abspath(a.build_cwd)
    a.fid = os.path.abspath(a.fid) if os.path.exists(a.fid) else a.fid
    if not os.path.exists(a.fid):
        print(f"me-measure-suite: no compiler at {a.fid}", file=sys.stderr)
        return 2
    passes = [p for p in re.split(r"[ ,]+", a.passes) if p] or compiler_passes()
    if not passes:
        print("me-measure-suite: no passes (no ime_on names in compiler/*.fd)", file=sys.stderr)
        return 1
    on_default = default_on()
    files = [f.replace(os.sep, "/") for f in a.files] or suite_files()
    cache = a.cache or os.path.join("build", "me-measure", file_hash(a.fid))
    os.makedirs(cache, exist_ok=True)
    jobs = max(1, a.jobs)
    print(f"me-measure-suite: {len(passes)} passes, {len(files)} programs, jobs {jobs}, cache {cache}", flush=True)
    print("  on by default: " + " ".join(p for p in passes if p in on_default), flush=True)

    tmp = tempfile.mkdtemp(prefix="me-measure-")
    share_bad = None

    def run_all(variants, label):
        todo = [(v, f) for v in variants for f in files if not os.path.exists(cache_path(cache, v, f))]
        if not todo:
            return
        t0 = time.time()
        done = 0
        with concurrent.futures.ThreadPoolExecutor(max_workers=jobs) as ex:
            futs = [ex.submit(build_one, a.fid, cache, v, f, tmp) for v, f in todo]
            for fu in concurrent.futures.as_completed(futs):
                fu.result()
                done += 1
                if done % 25 == 0 or done == len(todo):
                    print(f"  {label}: {done}/{len(todo)} builds, {int(time.time() - t0)} s", flush=True)

    if not a.report_only:
        run_all(["default"], "default")
        write_reports(a.out_dir, cache, passes, on_default, files, a.fid, jobs, None)
        for p in passes:
            von, voff = sides(p, on_default)
            run_all([v for v in (von, voff) if v != "default"], p)
            t = write_reports(a.out_dir, cache, passes, on_default, files, a.fid, jobs, None).get(p)
            if t:
                print(f"== {p}: instructions {t['ion']} vs {t['ioff']}, iropt {t['mon']} vs {t['moff']} ms, "
                      f"{t['diff']} DIFF", flush=True)
    if a.check_share > 0:
        share_bad = []
        first = files[:a.check_share]
        for p in passes:
            if not a.report_only:
                todo = [f for f in first if not os.path.exists(cache_path(cache, f"both:{p}", f))]
                with concurrent.futures.ThreadPoolExecutor(max_workers=jobs) as ex:
                    list(ex.map(lambda f: build_one(a.fid, cache, f"both:{p}", f, tmp), todo))
            for f in first:
                x, d = load(cache, f"both:{p}", f), load(cache, "default", f)
                if x is None or d is None:
                    continue
                if x["insts"] != d["insts"] or x["out_sha"] != d["out_sha"]:
                    share_bad.append(f"{p}:{os.path.basename(f)[:-3]} ({x['insts']} vs {d['insts']})")
        print("check-share: " + ("ok" if not share_bad else "DIFFERS: " + ", ".join(share_bad)), flush=True)
    totals = write_reports(a.out_dir, cache, passes, on_default, files, a.fid, jobs, share_bad)
    try:
        os.rmdir(tmp)
    except OSError:
        pass
    ndiff = sum(t["diff"] for t in totals.values())
    print(f"me-measure-suite: {len(totals)} pass files in {a.out_dir}, {ndiff} programs with different output; "
          f"table in {os.path.join(a.out_dir, 'optin-suite.md')}", flush=True)
    return 1 if (ndiff or share_bad) else 0


if __name__ == "__main__":
    sys.exit(main())
