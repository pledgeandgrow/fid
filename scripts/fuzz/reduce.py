#!/usr/bin/env python3
"""Shrink a Fides program on which two backends disagree (fuzz triage).

    python scripts/fuzz/reduce.py <in.fd> <out.fd> --bad emit --good ccompile
                                  [--fid release/fid.exe] [--match 'overflow']

Removes statements (a line, or a whole `if`/`while`/`for`/`match`/`fn`
with its block) and whole helper functions, one chunk at a time, keeping a
removal when the program still compiles on both backends and the two still
disagree (with --match, the bad backend's stderr must also contain the
text). The result is a short program that shows the bug.
"""
import argparse
import os
import subprocess
import tempfile


def run(fid, b, path, work):
    try:
        if b == "run":
            p = subprocess.run([fid, "run", path], capture_output=True, timeout=120)
            return p.returncode, p.stdout.decode("utf-8", "replace").replace("\r", ""), p.stderr.decode("utf-8", "replace")
        exe = os.path.join(work, f"r-{b}.exe")
        if os.path.exists(exe):
            os.remove(exe)
        p = subprocess.run([fid, b, path, "-o", exe], capture_output=True, timeout=300)
        if p.returncode != 0 or not os.path.exists(exe):
            return None
        q = subprocess.run([exe], capture_output=True, timeout=60)
        return q.returncode, q.stdout.decode("utf-8", "replace").replace("\r", ""), q.stderr.decode("utf-8", "replace")
    except subprocess.TimeoutExpired:
        return None


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
    ap.add_argument("--bad", required=True)
    ap.add_argument("--good", required=True)
    ap.add_argument("--fid", default=os.path.join("release", "fid.exe"))
    ap.add_argument("--match", default="")
    a = ap.parse_args()
    os.environ.setdefault("CC", "zig cc")
    os.environ.setdefault("FIDES_NO_CACHE", "1")
    work = tempfile.mkdtemp(prefix="reduce")
    path = os.path.join(work, "cand.fd")
    with open(a.src, encoding="utf-8") as f:
        lines = f.read().split("\n")

    def interesting(ls):
        with open(path, "w", encoding="utf-8", newline="\n") as f:
            f.write("\n".join(ls))
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
        orig["bad"] = run(a.fid, a.bad, path, work)
        orig["good"] = run(a.fid, a.good, path, work)
        return orig["bad"] is not None and orig["good"] is not None and \
            (orig["bad"][0], orig["bad"][1]) != (orig["good"][0], orig["good"][1])

    if not interesting_first(lines):
        print("the input does not show the disagreement")
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
