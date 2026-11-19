#!/usr/bin/env python3
"""Summarise the failures a fuzz run kept (gen_vs_c.py, gen_programs.py).

    python scripts/fuzz/triage.py <keep dir>

For each kept seed: which backends differ from the expected output (the C
oracle's, <seed>.want; without it, the majority), the first line that
differs, and the line of stderr that names the problem (a panic message, a
compile error). Seeds are grouped by that signature, so one bug that many
programs hit shows once with its count.

Since Nov 17 (VF1): a failure that points at the machine (out of memory, a
C compiler that could not start, a timeout) is grouped under
"environment?" and listed at the end as a seed to run again; a signature
that looks like a logged bug gets a hint ([likely M25], [maybe M24]).
"""
import collections
import glob
import os
import re
import sys


def read(p):
    with open(p, encoding="utf-8", errors="replace") as f:
        return f.read()


def split(o):
    out, _, err = o.partition("\n--- stderr ---\n")
    head, _, body = out.partition("\n")
    rc = head.replace("exit: ", "").strip()
    return rc, body, err


def key_line(err):
    for l in err.split("\n"):
        s = l.strip()
        if not s or s.startswith("[") or s.startswith("-->") or s.startswith("|"):
            continue
        if any(w in s for w in ("Panic", "error", "Error", "OOM", "CRASH", "violation", "overflow")):
            # addresses differ from run to run: 0x1C8EE1DFFE8 -> 0xN
            return re.sub(r"\d+", "N", re.sub(r"0x[0-9A-Fa-f]+", "0x", s))[:110]
    return ""


ENV_WORDS = ("out of memory", "Allocation failed", "failed to spawn", "OOM",
             "not enough memory", "paging file is too small")


def env_reason(rc, err):
    """Why a backend's failure may come from the machine, not the compiler
    (VF1, Nov 17: on a machine running several fid builds at once, `zig cc`
    ran out of memory, could not be started, or a step ran out of time).
    Such a failure is grouped apart; run its seed again on a quiet machine
    before counting it. Empty when nothing points at the machine."""
    if rc == "timeout":
        return "timeout"
    for l in err.split("\n"):
        for w in ENV_WORDS:
            if w in l:
                return re.sub(r"\d+", "N", l.strip())[:80]
    return ""


def known_hint(d, seed, b, err, rc=""):
    """A bug of docs/verification/miscompile-log.md the failure looks like
    (a hint for the reader, not a proof: reduce the program to be sure).
    `err` is what the backend printed (stdout and stderr)."""
    if re.search(r"expected identifier but got 'I8'", err):
        return "q4/q8 as a variable name: a compiler from before the fix of stdlib-agent-bugs 13 (test_fix_type_word_idents.fd)"
    if b == "checked" and re.search(r"Invalid memory access[^\n]* at 0x[0-9A-Fa-f]*FFE8\b", err):
        return "likely M25: a Vec buffer's tag probe under --checked-alloc"
    if b.startswith("emit") and rc != "build-failed":
        p = os.path.join(d, f"{seed}.fd")
        if os.path.exists(p) and re.search(r"^\s*(\w+) = \1;", read(p), re.M):
            return "maybe M24: self-assignment and promote; try FID_IROPT_OFF=promote"
    return ""


def wants(d):
    """(seed, expected exit status, expected output) of each kept seed: the
    C oracle's <seed>.want (gen_vs_c.py), or else (gen_programs.py) what
    most backends printed, the interpreter's when there is no majority."""
    res = []
    for want_p in glob.glob(os.path.join(d, "*.want")):
        seed = os.path.basename(want_p).split(".")[0]
        wrc, _, wbody = read(want_p).partition("\n")
        res.append((seed, wrc.replace("exit: ", "").strip(), wbody))
    have = {s for s, _, _ in res}
    by_seed = collections.defaultdict(list)
    for p in glob.glob(os.path.join(d, "*.out")):
        seed = os.path.basename(p).split(".")[0]
        if seed not in have and seed.isdigit():
            rc, body, _ = split(read(p))
            by_seed[seed].append((p.split(".")[-2], rc, body.rstrip("\n")))
    for seed, outs in by_seed.items():
        votes = collections.Counter((rc, body) for _, rc, body in outs)
        (rc, body), n = votes.most_common(1)[0]
        if n * 2 <= len(outs):
            # no majority: the most common result of the backends that
            # finished (a timeout of `run` and a crash of `checked` left two
            # agreeing backends a minority, VF1 seed 1000504), else run's
            fin = collections.Counter((r, o) for _, r, o in outs
                                      if r not in ("timeout", "build-failed")).most_common()
            if fin and (len(fin) == 1 or fin[0][1] > fin[1][1]):
                rc, body = fin[0][0]
            else:
                rc, body = next(((r, o) for b, r, o in outs if b == "run"), (rc, body))
        res.append((seed, rc, body))
    return sorted(res, key=lambda t: int(t[0]))


def main():
    d = sys.argv[1]
    groups = collections.defaultdict(list)
    for seed, wrc, wbody in wants(d):
        for p in sorted(glob.glob(os.path.join(d, f"{seed}.*.out"))):
            b = p.split(".")[-2]
            rc, body, err = split(read(p))
            if rc == wrc and body.rstrip("\n") == wbody.rstrip("\n"):
                continue
            wl, gl = wbody.split("\n"), body.split("\n")
            k = next((i for i in range(max(len(wl), len(gl)))
                      if i >= len(wl) or i >= len(gl) or wl[i] != gl[i]), 0)
            if rc == "build-failed":
                kind = "does not compile"
            elif rc != wrc:
                kind = f"exit {rc}, expected {wrc}"
            else:
                kind = "other output"
            env = env_reason(rc, err)
            sig = (f"environment? {b}: {kind}: {env}" if env
                   else f"{b}: {kind}: {key_line(err)}")
            known = known_hint(d, seed, b, body + "\n" + err, rc)
            if known and not env:
                sig += f" [{known}]"
            detail = (f"line {k + 1}: want {wl[k] if k < len(wl) else '<end>'!r}, "
                      f"{b} {gl[k] if k < len(gl) else '<end>'!r}")
            groups[sig].append(f"{seed} ({detail})")
    for sig, seeds in sorted(groups.items(), key=lambda kv: -len(kv[1])):
        print(f"{len(seeds):3}  {sig}")
        for s in seeds[:4]:
            print(f"       {s}")
    # the seeds with at least one failure that is not the machine's
    real = sorted({s.split(" ")[0] for sig, ss in groups.items()
                   if not sig.startswith("environment?") for s in ss}, key=int)
    envonly = sorted({s.split(" ")[0] for sig, ss in groups.items()
                      if sig.startswith("environment?") for s in ss} - set(real), key=int)
    print(f"seeds with a compiler-side signature: {len(real)} {' '.join(real)}")
    print(f"seeds with only machine-side signatures (rerun them): {len(envonly)} {' '.join(envonly)}")


if __name__ == "__main__":
    sys.stdout.reconfigure(encoding="utf-8")
    main()
