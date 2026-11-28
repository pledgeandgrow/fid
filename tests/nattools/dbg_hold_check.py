#!/usr/bin/env python3
"""BG245 (docs/roadmap/bugs-roadmap.md): the machine layer's DWARF location
lists cover a value held in a register past its last use, until something
writes the register (compiler/nat_ir.fd ng_dbg_live_ranges,
ng_dbg_clobbers), not only where it is live (which gave most variables a
range of one byte, the first byte of their last use).

    python3 tests/nattools/dbg_hold_check.py --fid FID [--keep DIR]

For tests/bootstrap/test_machine_dbg_hold.fd (`// @name` markers, `// dbg`
lines: dbg_run_check.py's syntax), each build made twice, as is and with
FID_NAT_OFF=dbghold (liveness alone, the location lists of before):

  1. `fid emit --target T` for x86_64-linux, aarch64-linux and
     riscv64-linux (DWARF in every Linux image), read without running:
     at each marker's address (its line's first row) the variables its
     `dbg` lines name that have a location there. Acceptance: never fewer
     than with dbghold off on any target, more in all, and fewer one-byte
     register entries in the test file's functions; no location entry
     outside its function; llvm-dwarfdump --verify passes (when installed:
     PATH, LLVM_DWARFDUMP or the usual Windows install).
  2. On Windows, with gdb on PATH: `fid emit --nat` with FID_NAT_DWARF=1
     (DWARF in an optimized --nat image; -g would keep the variables in
     frame slots, CE8) run under gdb, stopped at each marker, `info args`
     and `info locals`. Acceptance: every value gdb shows is the `dbg`
     line's (a wrong value fails), and it shows more of them than the
     dbghold-off build.

Exit 0 when every check holds."""
import os
import re
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import dbg_vars_check as dv  # noqa: E402
import dbg_run_check as dr  # noqa: E402

SRC = os.path.join(ROOT, "tests", "bootstrap", "test_machine_dbg_hold.fd")
TARGETS = ["x86_64-linux", "aarch64-linux", "riscv64-linux"]


def emit(fid, args, out, env_extra):
    env = dict(os.environ)
    env.update(env_extra)
    r = subprocess.run([fid, "emit", SRC] + args + ["-o", out], cwd=ROOT, env=env, capture_output=True, text=True)
    if r.returncode != 0 or not os.path.exists(out):
        return "emit %s failed: %s" % (" ".join(args), (r.stdout + r.stderr)[-500:])
    return ""


def wanted():
    """marker -> the variables its `dbg` lines name; and (marker, hit) ->
    {variable: value}."""
    markers, expects, _f, _t = dr.parse_test(SRC)
    names = {}
    vals = {}
    for name, hit, vs in expects:
        for k, v, _must in vs:
            names.setdefault(name, set()).add(k)
            vals.setdefault((name, hit), {})[k] = v
    return markers, names, vals


def is_reg_expr(e):
    return len(e) == 1 and 0x50 <= e[0] <= 0x6F or (len(e) >= 2 and e[0] == 0x90)


def static_counts(image):
    """(variables with a location at the markers, one-byte register
    entries of the test file's functions, entries outside their function,
    detail lines)."""
    dbg = dr.Debug(image)
    markers, names, _vals = wanted()
    avail = 0
    detail = []
    for mname, line in sorted(markers.items()):
        a, sub = dbg.any_line_addr(line)
        if a is None:
            detail.append("@%s: line %d has no row" % (mname, line))
            continue
        scopes = dbg.scopes_at(sub, a)
        shown = []
        for vname in sorted(names.get(mname, ())):
            for sc in scopes:
                cands = [k for k in dbg.kids(sc) if k[1] in (dv.DW_TAG_formal_parameter, dv.DW_TAG_variable) and k[2].get(0x03) == vname]
                if cands:
                    if any(dbg.expr_at(c, a) is not None for c in cands):
                        shown.append(vname)
                    break
        avail += len(shown)
        detail.append("@%s: %s" % (mname, ",".join(shown) or "-"))
    # the location lists of the test file's functions
    decl = os.path.basename(SRC)
    one = 0
    outside = 0
    for lo, hi, d in dbg.subs:
        df = d[2].get(0x3A)
        if df is None:
            continue
        if not any(d[2].get(0x03) == f for f in ("spread", "main")):
            continue
        for k in dbg.kids(d):
            loc = k[2].get(0x02)
            if not isinstance(loc, int):
                continue
            for s, e, ex in dr.loc_list(dbg.loc, loc, dbg.cu_base):
                if s < lo or e > hi:
                    outside += 1
                if e - s == 1 and is_reg_expr(ex):
                    one += 1
    return avail, one, outside, detail


def dwarfdump():
    dd = os.environ.get("LLVM_DWARFDUMP") or shutil.which("llvm-dwarfdump")
    if not dd:
        user = os.environ.get("USERNAME", "")
        cand = os.path.join("C:\\Users", user, "AppData", "Local", "Programs", "llvm-21.1.0", "bin", "llvm-dwarfdump.exe")
        if os.path.exists(cand):
            dd = cand
    return dd


def gdb_values(exe):
    """The values gdb shows at each marker's stops: [(marker, hit,
    {name: shown})], or None when gdb cannot run it."""
    markers, _names, _vals = wanted()
    by_line = {}
    script = ["set pagination off", "set confirm off"]
    for mname, line in markers.items():
        by_line[line] = mname
        script += ["break %s:%d" % (os.path.basename(SRC), line), "commands", "silent",
                   'printf "@@STOP %d\\n"' % line, "info args", "info locals", "continue", "end"]
    script.append("run")
    gs = exe + ".gdb"
    with open(gs, "w") as fh:
        fh.write("\n".join(script) + "\n")
    try:
        r = subprocess.run(["gdb", "-batch", "-x", gs, exe], capture_output=True, text=True, timeout=180)
    except (OSError, subprocess.TimeoutExpired):
        return None
    stops = []
    hits = {}
    cur = None
    for ln in r.stdout.split("\n"):
        m = re.match(r"@@STOP (\d+)", ln)
        if m:
            mname = by_line.get(int(m.group(1)), "?")
            hits[mname] = hits.get(mname, 0) + 1
            cur = {}
            stops.append((mname, hits[mname], cur))
            continue
        m = re.match(r"^(\w+) = (.*)$", ln.strip())
        if m and cur is not None:
            cur[m.group(1)] = m.group(2).strip()
    return stops


def gdb_tally(stops):
    """(values shown, wrong values, detail)."""
    _m, _n, vals = wanted()
    shown = 0
    wrong = []
    for mname, hit, got in stops:
        for k, want in vals.get((mname, hit), {}).items():
            g = got.get(k)
            if g is None or "optimized out" in g:
                continue
            shown += 1
            if g != want:
                wrong.append("@%s#%d %s = %s, want %s" % (mname, hit, k, g, want))
    return shown, wrong


def main():
    argv = sys.argv[1:]
    fid = os.environ.get("FID", "fid")
    keep = None
    i = 0
    while i < len(argv):
        if argv[i] == "--fid":
            fid = argv[i + 1]
            i += 2
        elif argv[i] == "--keep":
            keep = argv[i + 1]
            i += 2
        else:
            print("unknown argument " + argv[i])
            return 2
    work = keep or tempfile.mkdtemp(prefix="bg245")
    os.makedirs(work, exist_ok=True)
    fails = []
    dd = dwarfdump()
    tot = {"on": 0, "off": 0}
    one = {"on": 0, "off": 0}
    for t in TARGETS:
        got = {}
        for mode, env in (("on", {}), ("off", {"FID_NAT_OFF": "dbghold"})):
            out = os.path.join(work, "hold-%s-%s" % (t, mode))
            err = emit(fid, ["--target", t], out, env)
            if err:
                fails.append("%s %s: %s" % (t, mode, err))
                continue
            av, ob, outside, detail = static_counts(out)
            got[mode] = av
            tot[mode] += av
            one[mode] += ob
            print("     %s %-3s: %d variable(s) at the markers (%s), %d one-byte register entries" % (t, mode, av, "; ".join(detail), ob))
            if outside:
                fails.append("%s %s: %d location entries outside their function" % (t, mode, outside))
            if mode == "on" and dd:
                r = subprocess.run([dd, "--verify", out], capture_output=True, text=True)
                if r.returncode != 0:
                    fails.append("%s: llvm-dwarfdump --verify: %s" % (t, (r.stdout + r.stderr)[-400:]))
                else:
                    print("ok   %s: llvm-dwarfdump --verify" % t)
        if "on" in got and "off" in got:
            if got["on"] < got["off"]:
                fails.append("%s: %d variable(s) at the markers, fewer than %d with FID_NAT_OFF=dbghold" % (t, got["on"], got["off"]))
            else:
                print("ok   %s: %d variable(s) at the markers (FID_NAT_OFF=dbghold: %d)" % (t, got["on"], got["off"]))
    if tot["on"] <= tot["off"]:
        fails.append("the location lists show no more variables at the markers than liveness alone (%d, dbghold off %d)" % (tot["on"], tot["off"]))
    else:
        print("ok   %d variable(s) at the markers over the targets, %d with liveness alone" % (tot["on"], tot["off"]))
    if one["off"] > 0 and one["on"] >= one["off"]:
        fails.append("one-byte register entries: %d, liveness alone %d" % (one["on"], one["off"]))
    else:
        print("ok   one-byte register entries of the test's functions: %d (liveness alone %d)" % (one["on"], one["off"]))
    if not dd:
        print("skip llvm-dwarfdump (not installed)")
    # 2. gdb on a --nat build (Windows)
    if os.name == "nt" and shutil.which("gdb"):
        res = {}
        for mode, env in (("on", {"FID_NAT_DWARF": "1"}), ("off", {"FID_NAT_DWARF": "1", "FID_NAT_OFF": "dbghold"})):
            exe = os.path.join(work, "hold-nat-%s.exe" % mode)
            err = emit(fid, ["--nat"], exe, env)
            if err:
                fails.append("--nat %s: %s" % (mode, err))
                continue
            stops = gdb_values(exe)
            if not stops:
                fails.append("--nat %s: gdb did not stop at the markers" % mode)
                continue
            shown, wrong = gdb_tally(stops)
            res[mode] = shown
            print("     --nat %-3s: gdb shows %d value(s) at the markers" % (mode, shown))
            for w in wrong:
                fails.append("--nat %s: gdb: %s" % (mode, w))
        if "on" in res and "off" in res:
            if res["on"] > res["off"]:
                print("ok   --nat: gdb shows %d value(s), %d with liveness alone, none wrong" % (res["on"], res["off"]))
            else:
                fails.append("--nat: gdb shows %d value(s), liveness alone %d" % (res["on"], res["off"]))
    else:
        print("skip gdb on a --nat build (not Windows, or no gdb)")
    for f in fails:
        print("FAIL " + f)
    print("dbg_hold_check: %s" % ("ok" if not fails else "%d failure(s)" % len(fails)))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
