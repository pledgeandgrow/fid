#!/usr/bin/env python3
"""Do the VF5 tests of the machine layer reach their passes? (roadmap VF5)

    python scripts/verify/nat-pass-reach.py [--fid release/fid.exe]
           [--target nat|x86_64-linux|aarch64-linux|riscv64-linux] [--mutants]

Each tests/bootstrap/test_verify_nat_*.fd is meant for some switches of
docs/verification/kill-switches.md (the table below). For each, the test is
built with the machine layer as it is and with the switch off
(FID_NAT_OFF=<pass>); a test whose image does not change does not reach the
pass and checks nothing there ("same image": a finding, not a failure of
the compiler). When the image changes, both are run and must print the
same thing (a pass that is off must not change the output: VF4).

--mutants also builds each test under the run-time mutants of the machine
layer it is meant to kill (FID_NAT_MUTATE=<name>, compiler/nat_ir.fd
ng_mutant and compiler/egraph.fd; scripts/mutation-check.sh runs them over
the whole suite): the output must change ("killed"); "inert" when the image
does not change, "SURVIVED" when it changes and the output does not.

A test's own `//@ env:` lines are set for every build. Exit status 1 when a
switch changed an output or a mutant survived, else 0.
"""
import argparse
import hashlib
import os
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
sys.path.insert(0, os.path.join(ROOT, "scripts", "verify"))
import expect  # noqa: E402  (backend_cmd, linux_path, spec_of)

# test -> (FID_NAT_OFF switches it is meant for, FID_NAT_MUTATE mutants)
PLAN = {
    "test_verify_nat_promote.fd": (["promote"], []),
    "test_verify_nat_ipra.fd": (["ipra"], ["arghint", "argfree", "fcallee"]),
    "test_verify_nat_coalesce.fd": (["coalesce", "livecoal", "hint"], ["split"]),
    "test_verify_nat_remat.fd": (["remat", "rematmov", "weights"], []),
    "test_verify_nat_brfuse.fd": (["brfuse", "brgap", "brnot", "brext", "brandor", "brxblock", "tbz"],
                                  ["brgap", "brnot", "brandor", "brxblock", "tbz"]),
    "test_verify_nat_addshl.fd": (["addshl", "vecinline"], ["memop", "ldop", "ldext", "rmw", "memcmp"]),
    "test_verify_nat_egraph.fd": (["egraph", "forward", "rules", "purecse"],
                                  ["egfold", "egident", "egsame", "egflip", "egcomm", "egzero", "egdouble"]),
    "test_verify_nat_misc.fd": (["lea", "leaf", "tailcall", "retregs", "xmmcache", "mirfold", "relax", "peep", "fold"],
                                ["isel", "iselimm", "iselcc", "addi", "shi"]),
    "test_verify_nat_unchecked_fail.fd": (["unchecked", "coldpanic", "vecinline", "panicline"], []),
}


def sha(p):
    with open(p, "rb") as f:
        return hashlib.sha256(f.read()).hexdigest()


def build_run(fid, target, test, exe, env):
    cmd, runner = expect.backend_cmd(fid, target, test, exe)
    if os.path.exists(exe):
        os.remove(exe)
    rc, out, err = expect.run(cmd, env=env)
    if rc != 0 or not os.path.exists(exe):
        return None, None
    h = sha(exe)
    res = expect.run(runner + [expect.linux_path(exe) if runner else exe], 120, env=env)
    return h, (res[0], res[1])


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--fid", default=os.path.join("release", "fid.exe"))
    ap.add_argument("--target", default="nat" if os.name == "nt" else "x86_64-linux")
    ap.add_argument("--mutants", action="store_true")
    ap.add_argument("--only", default="", help="comma list of test file names")
    a = ap.parse_args()
    os.environ.setdefault("CC", "zig cc")
    os.environ["FIDES_NO_CACHE"] = "1"
    fid = os.path.abspath(a.fid)
    work = tempfile.mkdtemp(prefix="natreach")
    only = set(x for x in a.only.split(",") if x)
    bad = 0
    for name, (offs, muts) in PLAN.items():
        if only and name not in only:
            continue
        test = os.path.join(ROOT, "tests", "bootstrap", name)
        if not os.path.exists(test):
            print(f"{name}: missing")
            bad = 1
            continue
        tenv = expect.spec_of(test)["env"]
        base_env = dict(tenv)
        h0, r0 = build_run(fid, a.target, test, os.path.join(work, "base.exe"), base_env)
        if h0 is None:
            print(f"{name}: does not build on {a.target}")
            bad = 1
            continue
        rows = [("off", s) for s in offs] + ([("mutant", m) for m in muts] if a.mutants else [])
        for kind, s in rows:
            env = dict(base_env)
            if kind == "off":
                env["FID_NAT_OFF"] = s
            else:
                env["FID_NAT_MUTATE"] = s
            h1, r1 = build_run(fid, a.target, test, os.path.join(work, "var.exe"), env)
            if h1 is None:
                verdict = "build failed" if kind == "off" else "killed (build failed)"
                if kind == "off":
                    bad = 1
            elif h1 == h0:
                verdict = "same image (not reached)" if kind == "off" else "inert"
            elif r1 == r0:
                verdict = "reached, same output" if kind == "off" else "SURVIVED"
                if kind == "mutant":
                    bad = 1
            else:
                verdict = "OUTPUT CHANGED" if kind == "off" else "killed"
                if kind == "off":
                    bad = 1
            label = ("FID_NAT_OFF=" if kind == "off" else "FID_NAT_MUTATE=") + s
            print(f"{name:36} {label:28} {verdict}")
            sys.stdout.flush()
    sys.exit(bad)


if __name__ == "__main__":
    main()
