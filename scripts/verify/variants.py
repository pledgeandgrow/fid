#!/usr/bin/env python3
"""Optimizer passes switched off one at a time (VF4) and broken on purpose (VF7).

    # VF4: every pass off in turn; the output of every test must not change
    python scripts/verify/variants.py toggle --fid <base fid> --tree <snapshot> \
        [--tests 'tests/bootstrap/*.fd'] [--gen 40] [-j 4] [--out build/variants] [--only name,...]

    # VF7: each mutant is a compiler with one deliberate bug in a pass; some
    # test must notice (its output or exit status changes)
    python scripts/verify/variants.py mutate --fid <base fid> --tree <snapshot> [...]

    python scripts/verify/variants.py list

<tree> is a copy of the source tree (compiler/, stdlib/) — the base compiler
must have been built from it, so the only difference is the switch or the
mutation. Switches of compiler/iropt.fd are read at run time
(FID_IROPT_OFF=<pass>), so they need no build; the others are constants,
and each variant is built with scripts/verify/build-variant.sh (several GB
of memory and a few minutes each; built once, kept in <out>).

Backends: emit (the host's default native backend: pe_ir on Windows),
ccompile (the C path), nat (the machine layer, compiler/nat_ir.fd:
`emit --nat` on Windows, `emit --target x86_64-linux` on x86-64 Linux).

`toggle` also tries every run-time switch the tree's compiler reads that the
table below does not name (iro_pass_on -> FID_IROPT_OFF on emit and nat;
ng_on/ru_on/eg_on -> FID_NAT_OFF on nat), so a new pass is covered at once.
The mutants include the backend (own-backend roadmap RL11): a rewrite rule,
an x86-64 encoding and the allocator's clobber sets, each on nat.

For each variant and each test, the test is compiled (emit, or ccompile
for a switch the C path reads too) by the base and the variant compiler.
When the two executables are byte-identical the pass did nothing there;
otherwise both run and their exit status and output are compared.
--gen N adds N programs of scripts/fuzz/gen_vs_c.py and gen_programs.py.
A test's own `//@ env: NAME=value` lines (scripts/verify/expect.py's form)
are set for both compilers; a comma-list switch the variant sets too
(FID_IROPT_OFF, FID_NAT_OFF, ...) gets both lists.

A whole-suite run (Nov 17, VF4; scripts/verify/vf4-run.sh drives it):
--only takes shell patterns ('iropt-*', 'peir-*'); --backends emit keeps one
backend; --no-fail-tests leaves out the expected-fail tests; --prune-stats
skips a FID_IROPT_OFF build where the base build's FID_IROPT_STATS counter
says the pass changed no function (a share, --prune-check, is built anyway
to check the counter: PRUNE WRONG when the binary changed); --resume keeps
every result in <out>/<cmd>-results.jsonl and skips those already there, so
a run of many hours can be stopped and started again. A changed output is
run again on both binaries: when two runs of one binary disagree the test
is reported as flaky, not as a change. Every run has stdin closed.

Exit status (CI: .github/workflows/verify-weekly.yml): 0 when every toggle
left every output alone and every mutant was killed; 1 when a toggle changed
an output or a mutant survived; 2 when a variant did not build. On Linux
`emit` means `emit --target x86_64-linux` (a plain `emit` there goes through
C, which runs no IR optimizer).
"""
import argparse
import fnmatch
import glob
import hashlib
import json
import os
import random
import re
import subprocess
import sys
import threading
from concurrent.futures import ThreadPoolExecutor

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

# On Windows `bash` on the PATH can be WSL's (System32\bash.exe), which
# cannot run the scripts on Windows paths: prefer Git's
BASH = os.environ.get("BASH_EXE") or next(
    (p for p in (r"C:\Program Files\Git\bin\bash.exe", r"C:\Program Files\Git\usr\bin\bash.exe")
     if os.name == "nt" and os.path.exists(p)), "bash")

# (name, kind, how, backends)
#   kind env:   FID_IROPT_OFF value (compiler/iropt.fd iro_pass_on), or
#               "VAR=value" for another variable (FID_NAT_OFF=rules)
#   kind const: (file, from, to) edit of the tree
TOGGLES = [
    ("iropt-inline", "env", "inline", ["emit"]),
    ("iropt-fold", "env", "fold", ["emit"]),
    ("iropt-dce", "env", "dce", ["emit"]),
    ("iropt-tailcall", "env", "tailcall", ["emit"]),
    ("iropt-gvn", "env", "gvn", ["emit"]),
    ("iropt-licm", "env", "licm", ["emit"]),
    ("peir-global-ra", "const", ("compiler/pe_ir.fd", "const PEIR_GLOBAL_RA: bool = true;",
                                  "const PEIR_GLOBAL_RA: bool = false;"), ["emit"]),
    ("peir-coalesce", "const", ("compiler/pe_ir.fd", "const PEIR_COALESCE: bool = true;",
                                 "const PEIR_COALESCE: bool = false;"), ["emit"]),
    ("peir-volatile-ra", "const", ("compiler/pe_ir.fd", "const PEIR_VOLATILE_RA: bool = true;",
                                    "const PEIR_VOLATILE_RA: bool = false;"), ["emit"]),
    ("peir-xmm-ra", "const", ("compiler/pe_ir.fd", "const PEIR_XMM_RA: bool = true;",
                               "const PEIR_XMM_RA: bool = false;"), ["emit"]),
    ("peir-rotate", "const", ("compiler/pe_ir.fd", "const PEIR_ROTATE: bool = true;",
                               "const PEIR_ROTATE: bool = false;"), ["emit"]),
    ("ranges", "const", ("compiler/checks.fd", "const PEIR_RANGES: bool = true;",
                          "const PEIR_RANGES: bool = false;"), ["emit", "ccompile"]),
]

# Deliberate bugs, one per pass (VF7). Each edit must match exactly once.
MUTANTS = [
    ("fold-lt-as-le", ("compiler/iropt.fd",
                       "return OptInst::Some(Inst::ConstBool(a < c));",
                       "return OptInst::Some(Inst::ConstBool(a <= c));"), ["emit"]),
    ("fold-sub-swapped", ("compiler/iropt.fd",
                          "return OptInst::Some(Inst::ConstInt(a - c, t));",
                          "return OptInst::Some(Inst::ConstInt(c - a, t));"), ["emit"]),
    ("gvn-ignores-rhs", ("compiler/iropt.fd",
                         'return "B" + iro_binop_code(op) + "|" + int_to_str(a) + "|" + int_to_str(b) + "|" + ir_type_to_str(t);',
                         'return "B" + iro_binop_code(op) + "|" + int_to_str(a) + "|" + ir_type_to_str(t);'), ["emit"]),
    ("dce-all-pure", ("compiler/iropt.fd",  # iro_pure: every call is pure
                      "        Inst::BitCast(_, _, _) => {\n            return true;\n        }\n"
                      "        _ => {\n            return false;\n        }",
                      "        Inst::BitCast(_, _, _) => {\n            return true;\n        }\n"
                      "        Inst::Call(_, _, _, _) => {\n            return true;\n        }\n"
                      "        _ => {\n            return false;\n        }"),
     ["emit"]),
    ("inline-max-huge", ("compiler/iropt.fd", "const IRO_INLINE_MAX: i64 = 16;",
                         "const IRO_INLINE_MAX: i64 = 0;"), ["emit"]),
    ("ranges-len-bigger", ("compiler/checks.fd",
                           "peir_rng_put(r, id, 0, 281474976710656);",
                           "peir_rng_put(r, id, 1, 281474976710656);"), ["emit", "ccompile"]),
    # VF5: the passes of tests/bootstrap/test_verify_opt_ipcp_gconst.fd and
    # test_verify_opt_promote_facts.fd. ipcp: two different constants for
    # one parameter no longer make it vary (the first one wins)
    ("ipcp-first-constant-wins", ("compiler/iropt.fd",
                                  "} else if old != cval + 2 {",
                                  "} else if old != cval + 2 && false {"), ["emit"]),
    # gconst: a global stored to keeps its loads folded to the initializer
    ("gconst-stored-folded", ("compiler/iropt.fd",
                              "bad.set(o, 1);\n                                cval.set(cand.get(o) - 1, 0 - 1);",
                              "bad.set(o, 0);"), ["emit"]),
    # promote: at a join whose predecessors stored different values, the
    # first predecessor's value is used
    ("promote-join-first-store", ("compiler/iropt.fd",
                                  "} else if m != s {",
                                  "} else if m != s && false {"), ["emit"]),
    # branchfacts: an edge's fact is applied to a target that other edges
    # reach too
    ("facts-join-assumed", ("compiler/iropt.fd",
                            "var sole: bool = ps.len() == 1 && ps.get(0) == b;",
                            "var sole: bool = ps.len() >= 1;"), ["emit"]),
    # branchfacts: a false float comparison is read as its negation (wrong
    # for NaN)
    ("facts-float-negated", ("compiler/iropt.fd",
                             "if cmask != 0 && iop_is_cmp(iop) && (truth || !fcmp) {",
                             "if cmask != 0 && iop_is_cmp(iop) && (truth || true) {"), ["emit"]),
    # The backend (RL11): the machine layer's rules, encoders and allocator.
    # `fid test-rules` and scripts/enc-oracle.py catch the first ones on
    # their own; these ask whether the suite would too.
    ("rules-sub-zero-left", ("compiler/rules.fd",
                             'v.push(ru("x - 0 = x", RU_SUB, RS_RIGHT, 0, RR_OTHER, 0));',
                             'v.push(ru("x - 0 = x", RU_SUB, RS_LEFT, 0, RR_OTHER, 0));'), ["nat"]),
    ("rules-lt-self-true", ("compiler/rules.fd",
                            'v.push(ru("x < x", RU_LT, RS_SAME, 0, RR_FALSE, 0));',
                            'v.push(ru("x < x", RU_LT, RS_SAME, 0, RR_TRUE, 0));'), ["nat"]),
    ("enc-x64-disp8-128", ("compiler/nat_enc.fd",
                           "return v >= 0 - 128 && v <= 127;",
                           "return v >= 0 - 128 && v <= 128;"), ["nat"]),
    ("enc-x64-cqo-nop", ("compiler/nat_enc.fd",
                         "NatEnc_byte(e, 0x99);",
                         "NatEnc_byte(e, 0x90);"), ["nat"]),
    ("ra-calls-clobber-nothing", ("compiler/nat_ir.fd",
                                  "return ng_label_mask(g, m.i0);",
                                  "return 0;"), ["nat"]),
    # Nov 16 (VF7 "mutants of the other passes"): iv, sccp, dse, licm,
    # tailcall, the lowering of `for` and the C generator. Each is the
    # smallest edit that makes its pass wrong on some program; the test
    # meant to kill it is named beside it.
    # iv: `L = L - d` is read as `L = L + d` (test_verify_opt_dse_sccp_iv)
    ("iv-sub-as-add", ("compiler/iropt.fd",
                       "                    cid = b;\n                    sgn = - 1;",
                       "                    cid = b;\n                    sgn = 1;"), ["emit"]),
    # sccp: `a + 0` folds to 0 (test_verify_opt_dse_sccp_iv, test_opt_fold)
    ("sccp-add-zero-is-zero", ("compiler/iropt.fd",
                               "    if binop_eq(op, BinOp::Add) {\n        if b == 0 {\n            r = a;",
                               "    if binop_eq(op, BinOp::Add) {\n        if b == 0 {\n            r = b;"),
     ["emit"]),
    # dse: a load does not end a store's run, so `x = 1; read x; x = 2`
    # drops the first store (test_verify_opt_dse_sccp_iv)
    ("dse-load-not-a-read", ("compiler/iropt.fd",
                             "HashMap_remove(pending, int_to_str(p2));",
                             "HashMap_remove(pending, int_to_str(p2 + 1000000007));"), ["emit"]),
    # licm: the locals a loop stores are not seen, so their loads are
    # hoisted out of it (test_opt_licm)
    ("licm-stores-unseen", ("compiler/iropt.fd",
                            "stored.set(p, 1);",
                            "stored.set(p, 0);"), ["emit"]),
    # tailcall: the arguments go into the parameter slots in reverse order
    # (test_opt_tailcall)
    ("tailcall-args-reversed", ("compiler/iropt.fd",
                                "var st: i64 = iro_detached(f, Inst::Store(slots.get(ai), cargs2.get(ai), OptIrType::None));",
                                "var st: i64 = iro_detached(f, Inst::Store(slots.get(cargs2.len() - 1 - ai), cargs2.get(ai), OptIrType::None));"),
     ["emit"]),
    # lowering: `for i in a..b` runs to b inclusive (every backend; any loop test)
    ("lower-for-range-inclusive", ("compiler/lower.fd",
                                   "            var cmp_op: BinOp = BinOp::Lt;",
                                   "            var cmp_op: BinOp = BinOp::LtEq;"), ["emit", "ccompile"]),
    # the C generator: `@wrapping` a - b is emitted as b - a (test_wrapping*)
    ("c-wrapping-sub-swapped", ("compiler/codegen_c.fd",
                                'wln(self, "    " + dst + " = (" + t + ")(" + u64l + " - " + u64r + ");");',
                                'wln(self, "    " + dst + " = (" + t + ")(" + u64r + " - " + u64l + ");");'),
     ["ccompile"]),
    # the C generator: `>=` is emitted as `>` (any comparison at its bound)
    ("c-ge-as-gt", ("compiler/codegen_c.fd",
                    'BinOp::GtEq => {\n            return ">=";',
                    'BinOp::GtEq => {\n            return ">";'), ["ccompile"]),
    # Nov 17 (VF7: the machine layer's passes and the e-graph had no
    # source mutant; FID_NAT_MUTATE run-time ones are scripts/mutation-check.sh).
    # The tests meant to kill them are tests/bootstrap/test_verify_nat_*.fd.
    # promote: a local whose address goes to a call is promoted anyway
    # (test_verify_nat_promote: `bump(&mut c)`)
    ("promote-addr-escapes", ("compiler/nat_ir.fd",
                              "                if u >= 0 && u < n {\n                    cand.set(u, 0);",
                              "                if u >= 0 && u < n && false {\n                    cand.set(u, 0);"),
     ["nat"]),
    # promote: a promoted narrow local is not cut to its width
    # (test_verify_nat_promote: wrapping u8, i8, u16, i32)
    ("promote-no-narrow", ("compiler/nat_ir.fd",
                           "            cand.set(i, 1 + kind);",
                           "            cand.set(i, 1);"), ["nat"]),
    # coalesce: d <- s merged although s is written while d lives
    # (test_verify_nat_coalesce, test_verify_peir_coalesce's cases)
    ("coalesce-source-redefined", ("compiler/nat_ir.fd",
                                   "                    if ddefs.len() == 1 && ddefs.get(0) == k && !ng_any_in(sdefs, lo.get(cd), hi.get(cd), 0 - 1) {",
                                   "                    if ddefs.len() == 1 && ddefs.get(0) == k {"), ["nat"]),
    # remat: a spilled constant comes back one more (test_verify_nat_remat)
    ("remat-value-off-by-one", ("compiler/nat_ir.fd",
                                "                    remat_val.set(rr, rm.i0);",
                                "                    remat_val.set(rr, rm.i0 + 1);"), ["nat"]),
    # remat: a value defined by a constant and then changed is rematerialized
    # as its last constant (test_verify_nat_remat `changed_later`, `stepped`)
    ("remat-multi-def", ("compiler/nat_ir.fd",
                         "            var ok2: bool = rdefs.get(rr2) == 1 && ng_remat_is(remat.get(rr2));",
                         "            var ok2: bool = rdefs.get(rr2) >= 1 && ng_remat_is(remat.get(rr2));"), ["nat"]),
    # brfuse: a branch on a false compare (BEQZ) keeps the compare's sense
    # (test_verify_nat_brfuse)
    ("brfuse-beqz-not-inverted", ("compiler/nat_ir.fd",
                                  "            if b0.op == MO_BEQZ {\n                cc = ng_cc_inverse(cc);\n            }\n            if cand_gap.get(c) > 0 && ng_mutant(\"brgap\") {",
                                  "            if b0.op == MO_BEQZ && false {\n                cc = ng_cc_inverse(cc);\n            }\n            if cand_gap.get(c) > 0 && ng_mutant(\"brgap\") {"),
     ["nat"]),
    # brfuse: the compare is fused although its value is read after the
    # branch (test_verify_nat_brfuse `kept`)
    ("brfuse-live-ignored", ("compiler/nat_ir.fd",
                             "        if !live {\n            var setcc: MInst = mir.get(cand_pos.get(cand_at.get(c + 1) - 1));",
                             "        if true {\n            var setcc: MInst = mir.get(cand_pos.get(cand_at.get(c + 1) - 1));"),
     ["nat"]),
    # addshl: the inlined Vec element address scales the index by 4, not 8
    # (test_verify_nat_addshl, any Vec<i64> read at an index above 0)
    ("addshl-shift-off-by-one", ("compiler/nat_ir.fd",
                                 "        ng_addshl(g, g.t2, g.t2, g.t1, 3);",
                                 "        ng_addshl(g, g.t2, g.t2, g.t1, 2);"), ["nat"]),
    # egraph: a constant `a >= b` folds as `a > b` (test_verify_nat_egraph
    # `const_cmp`, built with the IR optimizer's folds off)
    ("egraph-ge-as-gt", ("compiler/egraph.fd",
                         "    } else if op == EB_GE {\n        if a >= b {\n            r = 1;",
                         "    } else if op == EB_GE {\n        if a > b {\n            r = 1;"), ["nat"]),
]


def switches_of(tree):
    """Run-time switches the tree's compiler reads: [(name, how, backends)]."""
    src = ""
    for f in sorted(glob.glob(os.path.join(tree, "compiler", "*.fd"))):
        with open(f, encoding="utf-8", errors="replace") as fh:
            src += fh.read()
    names = lambda fn: sorted(set(re.findall(fn + r'\("([a-z0-9_]+)"\)', src)))
    # Nov 17: the mid-end passes of IME_DEFAULT_ON (compiler/iropt_me.fd)
    # are on by default and FID_IROPT_OFF turns them off, so they are
    # toggles too. A pass_on switch is opt-in (left out: off already) only
    # when every line that reads it also asks ime_on for passes none of which
    # is on by default (`iro_pass_on("rotate") && ime_on("rotate")`); a name
    # merely also read by ime_on elsewhere (promote) stays a toggle.
    m = re.search(r'const IME_DEFAULT_ON: str = "([a-z0-9_,]*)"', src)
    default_on = set(x for x in (m.group(1) if m else "").split(",") if x)
    active = set()
    for line in src.split("\n"):
        gates = re.findall(r'ime_on\("([a-z0-9_]+)"\)', line)
        if gates and not any(g in default_on for g in gates):
            continue
        active.update(re.findall(r'iro_pass_on\("([a-z0-9_]+)"\)', line))
    pass_names = sorted(set(names("iro_pass_on")) & active | default_on)
    out = [(f"iropt-{n}", n, ["emit", "nat"]) for n in pass_names]
    nat = sorted(set(names("ng_on") + names("ru_on") + names("eg_on")))
    out += [(f"nat-{n}", f"FID_NAT_OFF={n}", ["nat"]) for n in nat]
    return out


def sha(p):
    with open(p, "rb") as f:
        return hashlib.sha256(f.read()).hexdigest()


def run(cmd, env=None, timeout=300):
    try:
        # stdin closed: a test that reads it gets end of input on both sides
        # (an inherited console would block the run)
        p = subprocess.run(cmd, capture_output=True, timeout=timeout, env=env,
                           stdin=subprocess.DEVNULL)
        return p.returncode, p.stdout.decode("utf-8", "replace").replace("\r\n", "\n"), p.stderr.decode("utf-8", "replace")
    except subprocess.TimeoutExpired:
        return "timeout", "", ""


def build(tree, name, edit, fid, out):
    exe = os.path.join(out, f"fid-{name}.exe")
    if os.path.exists(exe):
        return exe
    arg = f"{edit[0]}:{edit[1]}=>{edit[2]}" if edit else None
    fw = lambda p: p.replace("\\", "/")  # bash on Windows reads backslashes as escapes
    cmd = [BASH, fw(os.path.join(ROOT, "scripts", "verify", "build-variant.sh")), fw(tree), fw(exe)] + ([arg] if arg else [])
    env = dict(os.environ, FID=fw(fid))
    print(f"building {name} ...", flush=True)
    rc, out_, err = run(cmd, env, timeout=3600)
    if rc != 0 or not os.path.exists(exe):
        print(f"  build of {name} failed: {(out_ + err)[-600:]}")
        return None
    return exe


def build_cmd(fid, backend, test, exe_path):
    if backend == "nat":
        # the machine layer: --nat on Windows, the Linux x86-64 target here
        extra = ["--nat"] if os.name == "nt" else ["--target", "x86_64-linux"]
        return [fid, "emit", test] + extra + ["-o", exe_path]
    if backend == "emit" and sys.platform.startswith("linux"):
        # a plain `emit` on Linux goes through C, which runs no IR optimizer:
        # the iropt variants would change nothing there. The host's native
        # backend on Linux is the machine layer (`--target x86_64-linux`).
        return [fid, "emit", test, "--target", "x86_64-linux", "-o", exe_path]
    return [fid, backend, test, "-o", exe_path]


def compile_and_run(fid, env, backend, test, exe_path, cwd):
    if os.path.exists(exe_path):
        os.remove(exe_path)
    rc, out, err = run(build_cmd(fid, backend, test, exe_path), env)
    if rc != 0 or not os.path.exists(exe_path):
        # once more on its own: under load the C compiler's shared cache, or
        # memory, fails now and then
        with RETRY_LOCK:
            rc, out, err = run(build_cmd(fid, backend, test, exe_path), env)
    if rc != 0 or not os.path.exists(exe_path):
        return ("no-build", "", sha_text(out + err), out + err)
    # the compiler's own output: FID_IROPT_STATS lines for --prune-stats
    return (sha(exe_path), exe_path, "", out + err)


# --prune-stats (Nov 17): FID_IROPT_STATS=1 makes compiler/iropt.fd print, per
# module, how many functions each of these passes changed. A pass that
# changed nothing in a test's build leaves the same IR when it is turned
# off (the other passes see the same input), so its variant build can be
# skipped: the binary would be the base's. --prune-check builds a share of
# the skipped ones anyway; a binary that differs there means the counter is
# wrong, and is reported (PRUNE WRONG, exit 1). Switch name -> counter name.
STAT_OF = {"fold": "fold", "branchfacts": "branchfacts", "sccp": "sccp", "promote": "promote",
           "devirt": "devirt", "gconst": "gconst", "dse": "dse", "dce": "dce", "tailcall": "tailcall",
           "gvn": "gvn", "licm": "licm", "iv": "iv", "spec": "spec", "closuredevirt": "cdevirt",
           "hofspec": "hofspec", "version": "versioned loops"}
STAT_RE = re.compile(r"iropt: (fold \d+, .*?) functions changed")


def iropt_stats(text):
    """{counter: total} over every `iropt: fold N, ... functions changed` line,
    or None when the build printed none (the stats switch not read: no
    pruning on that test)."""
    tot = {}
    seen_any = False
    for m in STAT_RE.finditer(text):
        seen_any = True
        for part in m.group(1).split(", "):
            name, _, num = part.rpartition(" ")
            if num.isdigit():
                tot[name] = tot.get(name, 0) + int(num)
    return tot if seen_any else None


def sha_text(t):
    return hashlib.sha256(t.encode("utf-8", "replace")).hexdigest()[:12]


def gen_tests(n, out):
    sys.path.insert(0, os.path.join(ROOT, "scripts", "fuzz"))
    import gen_vs_c
    import gen_programs
    d = os.path.join(out, "gen")
    os.makedirs(d, exist_ok=True)
    files = []
    for s in range(n):
        p = os.path.join(d, f"gvc_{s}.fd")
        with open(p, "w", newline="\n") as f:
            f.write(gen_vs_c.pair(7000 + s)[0])
        files.append(p)
        p = os.path.join(d, f"gp_{s}.fd")
        with open(p, "w", newline="\n") as f:
            f.write(gen_programs.Gen(random.Random(9000 + s)).program())
        files.append(p)
    return files


RETRY_LOCK = threading.Lock()
BASE = {}  # (test, backend) -> (hash, exe kept for running, (rc, out))

# switches holding comma lists: a test's value is added to the variant's
LIST_VARS = ("FID_IROPT_OFF", "FID_IROPT_ON", "FID_NAT_OFF", "FID_NAT_ON", "FID_NAT_MUTATE")


def test_env(path):
    """The `//@ env: NAME=value` lines of a test file, as a dict."""
    env = {}
    try:
        with open(path, encoding="utf-8", errors="replace") as fh:
            for line in fh:
                s = line.strip()
                if s.startswith("//@ env:"):
                    k, _, v = s[len("//@ env:"):].strip().partition("=")
                    if k.strip():
                        env[k.strip()] = v.strip()
    except OSError:
        pass
    return env


def merge_env(base, extra):
    """base with extra's variables set; a comma-list switch both name gets
    both lists (the variant's FID_IROPT_OFF=licm and a test's own
    FID_IROPT_OFF=fold make FID_IROPT_OFF=licm,fold)."""
    if not extra:
        return base
    env = dict(base)
    for k, v in extra.items():
        if k in LIST_VARS and env.get(k):
            env[k] = env[k] + "," + v
        else:
            env[k] = v
    return env


class Opts:
    """Run-wide switches of compare (set from the command line in main)."""
    prune = False         # --prune-stats
    prune_check = 0.05    # --prune-check: share of pruned builds made anyway
    results = None        # --resume: the results file (JSON lines), or None
    done = {}             # (variant, test, backend) -> earlier result
    lock = threading.Lock()
    stats = {"pruned": 0, "prune_checked": 0, "resumed": 0}


def run_twice_differs(exe):
    """True when two runs of one binary disagree (a test whose output depends
    on time, addresses or scheduling): such a test cannot show a pass's
    effect, and is reported as flaky, not as a change."""
    a = run([exe], timeout=60)
    b = run([exe], timeout=60)
    return (a[0], a[1]) != (b[0], b[1])


def compare(base_fid, var_fid, env_var, backends, tests, out, jobs, name=""):
    """Returns (changed binaries, differing outputs, total, flaky, prune errors)."""
    work = os.path.join(out, "work")
    os.makedirs(work, exist_ok=True)
    benv = dict(os.environ)
    venv = dict(os.environ)
    if env_var:
        if "=" in env_var:
            k, _, v = env_var.partition("=")
            venv[k] = v
        else:
            venv["FID_IROPT_OFF"] = env_var
    # prune only a FID_IROPT_OFF switch with a counter of its own
    stat = STAT_OF.get(env_var) if (Opts.prune and env_var and "=" not in env_var) else None
    if Opts.prune:
        # every base build prints the counters (whichever variant builds it
        # first), so each later variant can be pruned on them
        benv["FID_IROPT_STATS"] = "1"
    jobs_list = [(t, b) for t in tests for b in backends]

    def one(tb):
        t, b = tb
        rk = (name, os.path.relpath(t, ROOT).replace("\\", "/"), b)
        if rk in Opts.done:
            d = Opts.done[rk]
            with Opts.lock:
                Opts.stats["resumed"] += 1
                if d.get("kind") == "pruned":
                    Opts.stats["pruned"] += 1
            return d["changed"], ((t, b, d["why"]) if d["why"] else None), d["kind"]
        tag = hashlib.md5((t + b + (env_var or var_fid)).encode()).hexdigest()[:10]
        bt = hashlib.md5((t + b).encode()).hexdigest()[:10]
        e1 = os.path.join(work, f"{bt}-base.exe")
        e2 = os.path.join(work, f"{tag}-var.exe")
        # a test's own //@ env: lines (a pass switch it is built with, as
        # scripts/verify/expect.py reads them) go to both compilers
        tenv = test_env(t)
        benv_t = merge_env(benv, tenv)
        venv_t = merge_env(venv, tenv)
        # the base build of a test is the same for every variant: once. Its
        # binary is kept (work/<hash>-base.exe) for the rerun of a mismatch.
        with Opts.lock:
            have = (t, b) in BASE
        if not have or not (BASE[(t, b)][0] == "no-build" or os.path.exists(e1)):
            h = compile_and_run(base_fid, benv_t, b, t, e1, out)
            r = run([e1], timeout=60) if h[0] != "no-build" else None
            with Opts.lock:
                BASE[(t, b)] = (h[0], r, iropt_stats(h[3]) if len(h) > 3 else None)
        h1 = BASE[(t, b)][0]
        kind = "built"
        pstats = BASE[(t, b)][2] if len(BASE[(t, b)]) > 2 else None
        pruned = (stat is not None and h1 != "no-build" and pstats is not None
                  and pstats.get(stat, -1) == 0)
        if pruned:
            # the pass changed no function here: the variant's binary is the
            # base's; a share is built anyway to check that claim
            pick = int(hashlib.md5((rk[1] + b + name).encode()).hexdigest()[:8], 16) / 0xFFFFFFFF
            if pick >= Opts.prune_check:
                with Opts.lock:
                    Opts.stats["pruned"] += 1
                rec = {"changed": False, "why": None, "kind": "pruned"}
                record(rk, rec)
                return False, None, "pruned"
            with Opts.lock:
                Opts.stats["prune_checked"] += 1
            kind = "prune-checked"
        h2 = compile_and_run(var_fid, venv_t, b, t, e2, out)[0]
        why = None
        if h1 == "no-build" or h2 == "no-build":
            if h1 != h2:
                why = "builds with one compiler only"
        elif h1 != h2:
            if kind == "prune-checked":
                kind = "prune-wrong"
            r1 = BASE[(t, b)][1]
            r2 = run([e2], timeout=60)
            if (r1[0], r1[1]) != (r2[0], r2[1]):
                if run_twice_differs(e2) or run_twice_differs(e1):
                    kind = "flaky"
                else:
                    # the base's recorded run against a fresh one of each
                    r1b = run([e1], timeout=60)
                    if (r1b[0], r1b[1]) != (r1[0], r1[1]):
                        kind = "flaky"
                    else:
                        why = f"exit {r1[0]} vs {r2[0]}; output {len(r1[1])} vs {len(r2[1])} bytes"
        for e in (e2, e2[:-4] + ".pdb", e1[:-4] + ".pdb"):
            if os.path.exists(e):
                try:
                    os.remove(e)
                except OSError:
                    pass
        # prune-wrong: the pass's counter was 0 but the binary changed (an
        # output change, if any, is in `why` as for any other build)
        record(rk, {"changed": h1 != h2, "why": why, "kind": kind})
        return h1 != h2, ((t, b, why) if why else None), kind

    changed = 0
    diffs = []
    flaky = []
    prune_wrong = 0
    with ThreadPoolExecutor(jobs) as ex:
        for (ch, res, kind), tb in zip(ex.map(one, jobs_list), jobs_list):
            changed += 1 if ch else 0
            if res and res[2]:
                diffs.append(res)
            if kind == "flaky":
                flaky.append(tb)
            if kind == "prune-wrong":
                prune_wrong += 1
    return changed, diffs, len(jobs_list), flaky, prune_wrong


def record(rk, rec):
    """Append one result to the --resume file."""
    if not Opts.results:
        return
    line = json.dumps({"variant": rk[0], "test": rk[1], "backend": rk[2], **rec})
    with Opts.lock:
        with open(Opts.results, "a", encoding="utf-8") as f:
            f.write(line + "\n")


def load_results(path, fid_sha):
    """The --resume file's results made with this same base compiler."""
    done = {}
    if not os.path.exists(path):
        return done
    with open(path, encoding="utf-8") as f:
        first = f.readline()
        try:
            head = json.loads(first)
        except ValueError:
            head = {}
        if head.get("fid_sha") != fid_sha:
            print(f"resume: {path} was made with another compiler; starting again", flush=True)
            return None
        for line in f:
            try:
                d = json.loads(line)
            except ValueError:
                continue  # a line cut short by an interrupted run
            done[(d["variant"], d["test"], d["backend"])] = d
    return done


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("cmd", choices=["toggle", "mutate", "list", "check"])
    ap.add_argument("--fid", default=os.path.join("release", "fid.exe"))
    ap.add_argument("--tree", default=".")
    ap.add_argument("--tests", default="tests/bootstrap/*.fd")
    ap.add_argument("--gen", type=int, default=0)
    ap.add_argument("--sample", type=int, default=0, help="a random sample of this many --tests files")
    ap.add_argument("--out", default=os.path.join("build", "variants"))
    ap.add_argument("--only", default="",
                    help="variant names, comma-separated; shell patterns too ('iropt-*', 'peir-*')")
    ap.add_argument("--backends", default="",
                    help="only these backends of each variant (emit, nat, ccompile)")
    ap.add_argument("--prune-stats", action="store_true",
                    help="skip a FID_IROPT_OFF variant's build where the base's FID_IROPT_STATS says "
                         "the pass changed no function (checked on --prune-check of them)")
    ap.add_argument("--prune-check", type=float, default=0.05)
    ap.add_argument("--resume", action="store_true",
                    help="keep each result in <out>/<cmd>-results.jsonl and skip the ones already "
                         "there (same base compiler): a long run in several sittings")
    ap.add_argument("--no-fail-tests", action="store_true",
                    help="leave out *_fail.fd (rejected by both compilers: nothing to compare)")
    ap.add_argument("--build-only", action="store_true")
    ap.add_argument("-j", type=int, default=4)
    a = ap.parse_args()
    if a.cmd == "check":
        # No build: every mutant's and constant toggle's edit must match its
        # file exactly once in --tree (build-variant.sh refuses it otherwise,
        # and the weekly run would report BUILD FAILED). Exit 1 if one does not.
        edits = [(n, e) for n, e, _ in MUTANTS] + [(n, how) for n, k, how, _ in TOGGLES if k == "const"]
        bad = 0
        for n, (path, frm, _to) in edits:
            p = os.path.join(a.tree, path)
            try:
                with open(p, encoding="utf-8", newline="") as fh:
                    cnt = fh.read().count(frm)
            except OSError:
                cnt = -1
            ok = cnt == 1
            bad += 0 if ok else 1
            print(f"{'ok' if ok else 'STALE':6} {n:28} {path}" + ("" if ok else f" (matches {cnt} times)"))
        print(f"{len(edits)} edits, {bad} stale")
        sys.exit(1 if bad else 0)
    if a.cmd == "toggle" or a.cmd == "list":
        known = {how for _, k, how, _ in TOGGLES if k == "env"}
        for n, how, b in switches_of(os.path.abspath(a.tree)):
            if how not in known:
                TOGGLES.append((n, "env", how, b))
    if a.cmd == "list":
        for n, k, how, b in TOGGLES:
            print(f"toggle {n:18} {k:5} {b}")
        for n, e, b in MUTANTS:
            print(f"mutant {n:18} {e[0]} {b}")
        return
    os.environ.setdefault("CC", "zig cc")
    os.environ["FIDES_NO_CACHE"] = "1"
    out = os.path.abspath(a.out)
    os.makedirs(out, exist_ok=True)
    fid = os.path.abspath(a.fid)
    tree = os.path.abspath(a.tree)
    tests = []
    for g in a.tests.split(","):
        tests += sorted(glob.glob(g))
    if a.no_fail_tests:
        tests = [t for t in tests if not t.endswith("_fail.fd")]
    if a.sample and len(tests) > a.sample:
        tests = sorted(random.Random(1).sample(tests, a.sample))
    if a.gen:
        tests += gen_tests(a.gen, out)
    only = [x for x in a.only.split(",") if x]
    picked = lambda n: not only or any(fnmatch.fnmatchcase(n, p) for p in only)
    rows = []
    if a.cmd == "toggle":
        items = [(n, k, how, b) for n, k, how, b in TOGGLES if picked(n)]
    else:
        items = [(n, "const", e, b) for n, e, b in MUTANTS if picked(n)]
    if a.backends:
        want = set(a.backends.split(","))
        items = [(n, k, how, [x for x in b if x in want]) for n, k, how, b in items]
        items = [it for it in items if it[3]]
    if not items:
        sys.exit(f"no variant matches --only {a.only!r}" + (f" on {a.backends}" if a.backends else ""))
    # build every variant that needs it first (one at a time: memory). The
    # base is the unedited tree built the same way, so a const variant
    # differs from it by the edit alone; an env switch runs on the base.
    exes = {}
    base = fid
    if any(k == "const" for _, k, _, _ in items):
        base = build(tree, "base", None, fid, out)
        if not base:
            sys.exit("could not build the base compiler from the tree")
    for n, k, how, b in items:
        exes[n] = base if k == "env" else build(tree, n, how, fid, out)
    if a.build_only:
        return
    fid = base
    Opts.prune = a.prune_stats and a.cmd == "toggle"
    Opts.prune_check = a.prune_check
    if a.resume:
        Opts.results = os.path.join(out, f"{a.cmd}-results.jsonl")
        fsha = sha(fid)
        done = load_results(Opts.results, fsha)
        if done is None or not os.path.exists(Opts.results):
            with open(Opts.results, "w", encoding="utf-8") as f:
                f.write(json.dumps({"fid": fid, "fid_sha": fsha}) + "\n")
            done = {}
        Opts.done = done
        print(f"resume: {len(done)} results already in {Opts.results}", flush=True)
    report = os.path.join(out, f"{a.cmd}-report.txt")
    # exit status (for CI, VF4/VF7): 1 when a toggle changed an output or a
    # mutant survived (no test noticed it), 2 when a variant did not build
    bad = 0
    with open(report, "w") as rep:
        rep.write(f"# {a.cmd}: {len(tests)} tests, base {fid}"
                  + (f", pruned on FID_IROPT_STATS (checked on {a.prune_check:.0%})" if Opts.prune else "")
                  + "\n")
        for n, k, how, b in items:
            if not exes[n]:
                rows.append((n, "build failed", ""))
                line = f"{n:18} {'BUILD FAILED':12} ({','.join(b)})"
                print(line, flush=True)
                rep.write(line + "\n")
                bad = max(bad, 2)
                continue
            p0 = Opts.stats["pruned"]
            changed, diffs, total, flaky, prune_wrong = compare(
                fid, exes[n], how if k == "env" else None, b, tests, out, a.j, name=n)
            if a.cmd == "toggle":
                verdict = "ok" if not diffs else f"{len(diffs)} DIFFER"
            else:
                verdict = "killed" if diffs else "SURVIVED"
            if verdict != "ok" and verdict != "killed" and bad == 0:
                bad = 1
            if prune_wrong and bad == 0:
                bad = 1
            extra = ""
            if Opts.stats["pruned"] > p0:
                extra += f", {Opts.stats['pruned'] - p0} pruned"
            if flaky:
                extra += f", {len(flaky)} flaky"
            if prune_wrong:
                extra += f", PRUNE WRONG {prune_wrong} (counter 0, binary changed)"
            line = f"{n:18} {verdict:12} binaries changed in {changed}/{total} ({','.join(b)}{extra})"
            print(line, flush=True)
            rep.write(line + "\n")
            for t, bk, why in diffs[:30]:
                rep.write(f"    {bk} {os.path.relpath(t)}: {why}\n")
                print(f"    {bk} {os.path.relpath(t)}: {why}")
            for t, bk in flaky[:10]:
                rep.write(f"    flaky {bk} {os.path.relpath(t)}: two runs of one binary differ\n")
            rep.flush()
        rep.write(f"# pruned {Opts.stats['pruned']}, prune-checked {Opts.stats['prune_checked']}, "
                  f"resumed {Opts.stats['resumed']}\n")
    print(f"report: {report}")
    sys.exit(bad)


if __name__ == "__main__":
    main()
