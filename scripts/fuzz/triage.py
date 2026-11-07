#!/usr/bin/env python3
"""Summarise the failures a fuzz run kept (gen_vs_c.py, gen_programs.py).

    python scripts/fuzz/triage.py <keep dir>

For each kept seed: which backends differ from the expected output (the C
oracle's, <seed>.want; without it, the majority), the first line that
differs, and the line of stderr that names the problem (a panic message, a
compile error). Seeds are grouped by that signature, so one bug that many
programs hit shows once with its count.
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
            return re.sub(r"\d+", "N", s)[:110]
    return ""


def main():
    d = sys.argv[1]
    groups = collections.defaultdict(list)
    for want_p in sorted(glob.glob(os.path.join(d, "*.want")), key=lambda p: int(os.path.basename(p).split(".")[0])):
        seed = os.path.basename(want_p).split(".")[0]
        w = read(want_p)
        wrc, _, wbody = w.partition("\n")
        wrc = wrc.replace("exit: ", "").strip()
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
                kind = f"exit {rc}, C exits {wrc}"
            else:
                kind = "other output"
            sig = f"{b}: {kind}: {key_line(err)}"
            detail = (f"line {k + 1}: C {wl[k] if k < len(wl) else '<end>'!r}, "
                      f"{b} {gl[k] if k < len(gl) else '<end>'!r}")
            groups[sig].append(f"{seed} ({detail})")
    for sig, seeds in sorted(groups.items(), key=lambda kv: -len(kv[1])):
        print(f"{len(seeds):3}  {sig}")
        for s in seeds[:4]:
            print(f"       {s}")


if __name__ == "__main__":
    sys.stdout.reconfigure(encoding="utf-8")
    main()
