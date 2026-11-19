#!/usr/bin/env python3
"""Shrink a failing Fides program while it still fails the same way
(fuzz triage; own-backend roadmap RL21).

    # two backends disagree (a miscompile)
    python scripts/fuzz/reduce.py <in.fd> <out.fd> --bad emit --good ccompile
                                  [--fid release/fid.exe] [--match 'overflow']
    # one backend fails: the compiler crashes or rejects the program, or the
    # program panics or exits non-zero; the failure must stay the same
    python scripts/fuzz/reduce.py <in.fd> <out.fd> --fails emit [--match 'Invalid memory']
    # any command: {} is the candidate file; it must keep its exit status
    # (non-zero) and, with --match, print the text
    python scripts/fuzz/reduce.py <in.fd> <out.fd> --cmd "release/fid.exe check {}" --match "panicked"

Removes statements (a line, or a whole `if`/`while`/`for`/`match`/`fn`
with its block) and whole helper functions, one chunk at a time, keeping a
removal when the program is still interesting:

  --bad/--good  it compiles on both backends and the two still disagree, each
                with the exit status it had (with --match, the bad backend's
                output must also contain the text);
  --fails B     it fails on backend B (run, check, ccompile, emit, nat, or
                emit:<target> built only) at the same stage (build or run),
                with the same exit status and the same first error or panic
                line, numbers aside (line numbers move as lines go);
  --cmd C       the command exits with the same non-zero status.

The result is a short program that shows the bug.
"""
import argparse
import os
import re
import shlex
import subprocess
import tempfile


def run(fid, b, path, work):
    try:
        if b == "run":
            p = subprocess.run([fid, "run", path], capture_output=True, timeout=120)
            return p.returncode, p.stdout.decode("utf-8", "replace").replace("\r", ""), p.stderr.decode("utf-8", "replace")
        if b == "checked":
            # the C path under the checked allocator (gen_programs.py's
            # backend): it builds _fides_memcheck_tmp.exe in the current
            # directory and names it on stdout
            p = subprocess.run([fid, "run", os.path.abspath(path), "--checked-alloc"],
                               capture_output=True, timeout=300, cwd=work)
            out = p.stdout.decode("utf-8", "replace").replace("\r", "")
            out = "".join(l for l in out.splitlines(True) if not l.startswith("Built native executable:"))
            return p.returncode, out, p.stderr.decode("utf-8", "replace")
        exe = os.path.join(work, f"r-{b}.exe")
        if os.path.exists(exe):
            os.remove(exe)
        # gen_vs_c.py's backend names: nat is `emit --nat`, ccompile-O2 the
        # C path at -O2 (VF2: a V1 seed differs on one of them)
        cmd = {"nat": [fid, "emit", path, "--nat", "-o", exe],
               "ccompile-O2": [fid, "ccompile", path, "--cc-opt", "2", "-o", exe]}.get(
                   b, [fid, b, path, "-o", exe])
        p = subprocess.run(cmd, capture_output=True, timeout=300)
        if p.returncode != 0 or not os.path.exists(exe):
            return None
        q = subprocess.run([exe], capture_output=True, timeout=60)
        return q.returncode, q.stdout.decode("utf-8", "replace").replace("\r", ""), q.stderr.decode("utf-8", "replace")
    except subprocess.TimeoutExpired:
        return None


def build_and_run(fid, b, path, work):
    """(stage, exit status, output) of backend `b` on `path`: stage is
    "build" when the compiler fails, "run" otherwise; None on a timeout."""
    kind, _, target = b.partition(":")
    try:
        if b in ("run", "check"):
            p = subprocess.run([fid, b, path], capture_output=True, timeout=120)
            return "run", p.returncode, (p.stdout + p.stderr).decode("utf-8", "replace").replace("\r", "")
        exe = os.path.join(work, f"r-{b.replace(':', '-')}.exe")
        if os.path.exists(exe):
            os.remove(exe)
        cmd = [fid, kind, path] + (["--target", target] if target else []) + \
            (["--nat"] if kind == "nat" else []) + ["-o", exe]
        if kind == "nat":
            cmd[1] = "emit"
        p = subprocess.run(cmd, capture_output=True, timeout=300)
        text = (p.stdout + p.stderr).decode("utf-8", "replace").replace("\r", "")
        if p.returncode != 0 or not os.path.exists(exe):
            return "build", p.returncode, text
        if target:
            return "run", 0, text  # another target: built, not run here
        q = subprocess.run([exe], capture_output=True, timeout=60)
        return "run", q.returncode, (q.stdout + q.stderr).decode("utf-8", "replace").replace("\r", "")
    except subprocess.TimeoutExpired:
        return None


def failure_key(text):
    """The first error or panic line, with numbers and paths blurred."""
    for l in text.split("\n"):
        if re.search(r"Panic|panic|rror|Assertion|Invalid|overflow|Stack", l):
            l = re.sub(r"[A-Za-z]:[\\/][^\s:]*|/[^\s:]+", "<path>", l)
            return re.sub(r"\d+", "N", l.strip())[:200]
    return ""


def signature(res):
    """What makes two failures 'the same': stage, status, first message."""
    if res is None:
        return ("timeout",)
    stage, rc, text = res
    return (stage, rc, failure_key(text))


def units(lines):
    """Index ranges [i, j) that can be removed as one statement."""
    out = []
    for i, l in enumerate(lines):
        s = l.strip()
        if not s or s.startswith("}") or s.startswith("//") or s.startswith("fn main"):
            continue
        if s.startswith("return") and l.startswith("    ") and not l.startswith("     "):
            continue  # a function's own return: removing it changes the exit status
        if s.endswith("{"):
            depth = 0
            j = i
            while j < len(lines):
                t = lines[j].strip()
                depth += t.count("{") - t.count("}")
                j += 1
                if depth <= 0 and not lines[j - 1].strip().endswith("{"):
                    break
            out.append((i, j))
        else:
            out.append((i, i + 1))
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("src")
    ap.add_argument("dst")
    ap.add_argument("--bad")
    ap.add_argument("--good")
    ap.add_argument("--fails", help="one backend fails; keep the same failure")
    ap.add_argument("--cmd", help="a command with {} for the file; keep its exit status")
    ap.add_argument("--fid", default=os.path.join("release", "fid.exe"))
    ap.add_argument("--match", default="")
    a = ap.parse_args()
    if not (a.fails or a.cmd or (a.bad and a.good)):
        ap.error("give --bad and --good, or --fails <backend>, or --cmd <command>")
    os.environ.setdefault("CC", "zig cc")
    os.environ.setdefault("FIDES_NO_CACHE", "1")
    work = tempfile.mkdtemp(prefix="reduce")
    path = os.path.join(work, "cand.fd")
    with open(a.src, encoding="utf-8") as f:
        lines = f.read().split("\n")

    def outcome():
        """The current candidate's failure, for --fails and --cmd."""
        if a.cmd:
            try:
                p = subprocess.run(a.cmd.replace("{}", shlex.quote(path) if os.name != "nt" else path),
                                   shell=True, capture_output=True, timeout=300)
            except subprocess.TimeoutExpired:
                return ("timeout",), ""
            text = (p.stdout + p.stderr).decode("utf-8", "replace")
            return ("cmd", p.returncode), text
        res = build_and_run(a.fid, a.fails, path, work)
        return signature(res), (res[2] if res else "")

    def failing(sig):
        if sig[0] == "timeout":
            return False
        return sig[1] != 0

    def interesting(ls):
        with open(path, "w", encoding="utf-8", newline="\n") as f:
            f.write("\n".join(ls))
        if a.fails or a.cmd:
            sig, text = outcome()
            return failing(sig) and sig == orig["sig"] and (not a.match or a.match in text)
        b = run(a.fid, a.bad, path, work)
        if b is None or (a.bad == "run" and b[0] != 0 and "rror" in b[2] and not b[1] and "Panic" not in b[2]):
            return False
        g = run(a.fid, a.good, path, work)
        if g is None or (a.good == "run" and g[0] != 0 and "rror" in g[2] and not g[1] and "Panic" not in g[2]):
            return False
        if a.match and a.match not in b[2] + b[1]:
            return False
        # the same failure: each backend keeps its exit status
        if orig and (b[0] != orig["bad"][0] or g[0] != orig["good"][0]):
            return False
        return (b[0], b[1]) != (g[0], g[1])

    orig = {}

    def interesting_first(ls):
        with open(path, "w", encoding="utf-8", newline="\n") as f:
            f.write("\n".join(ls))
        if a.fails or a.cmd:
            orig["sig"], text = outcome()
            print(f"the failure to keep: {orig['sig']}")
            return failing(orig["sig"]) and (not a.match or a.match in text)
        orig["bad"] = run(a.fid, a.bad, path, work)
        orig["good"] = run(a.fid, a.good, path, work)
        return orig["bad"] is not None and orig["good"] is not None and \
            (orig["bad"][0], orig["bad"][1]) != (orig["good"][0], orig["good"][1])

    if not interesting_first(lines):
        print("the input does not show the failure (or the disagreement)")
        return
    tries = 0
    changed = True
    while changed:
        changed = False
        us = units(lines)
        # big chunks first (helpers, loops), then single lines
        us.sort(key=lambda r: -(r[1] - r[0]))
        for i, j in us:
            if j > len(lines):
                continue
            cand = lines[:i] + lines[j:]
            tries += 1
            if interesting(cand):
                lines = cand
                changed = True
                print(f"  {len(lines)} lines (try {tries})", flush=True)
                break
    with open(a.dst, "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(lines))
    print(f"reduced to {len([l for l in lines if l.strip()])} lines in {tries} tries -> {a.dst}")


if __name__ == "__main__":
    main()
