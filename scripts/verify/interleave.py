#!/usr/bin/env python3
"""Sharded interleaving tests: one program, many schedules (roadmap VF18).

    python scripts/verify/interleave.py --fid release/fid.exe [-n 2000] [--shards 4]
        [--backends ccompile,emit] [--programs tests/verify/interleave.fd,...]
        [--out build/interleave]

Each program is built once per backend, then run -n times, spread over
--shards worker processes. Runs differ in what the OS scheduler sees: each
run gets a random set of CPUs (one CPU forces the threads to interleave by
preemption alone; several let them run truly at once), and half of them run
while busy-loop threads load the machine. The program's output must be the
same in every run (its first run is the reference); a run that prints
something else, crashes, or hangs (--timeout) is kept in <out> with its
output.

This perturbs the OS scheduler; it does not choose the interleaving. A
controlled scheduler (every switch point enumerated, as CHESS or loom do)
would need hooks in the runtime: not done.
"""
import argparse
import os
import random
import subprocess
import sys
import threading
import time
from concurrent.futures import ThreadPoolExecutor

import psutil


def burner(stop):
    while not stop.is_set():
        x = 0
        for i in range(100000):
            x += i * i


def one(exe, seed, timeout):
    r = random.Random(seed)
    ncpu = psutil.cpu_count() or 1
    k = r.choice([1, 1, 2, max(1, ncpu // 2), ncpu])
    cpus = sorted(r.sample(range(ncpu), k))
    stop = threading.Event()
    burners = []
    if r.random() < 0.5:
        for _ in range(r.randint(1, 3)):
            t = threading.Thread(target=burner, args=(stop,), daemon=True)
            t.start()
            burners.append(t)
    try:
        p = subprocess.Popen([exe], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        try:
            psutil.Process(p.pid).cpu_affinity(cpus)
        except (psutil.Error, AttributeError):
            pass
        try:
            out, err = p.communicate(timeout=timeout)
            rc = p.returncode
        except subprocess.TimeoutExpired:
            p.kill()
            out, err = p.communicate()
            rc = "timeout"
    finally:
        stop.set()
    return rc, out.decode("utf-8", "replace").replace("\r", ""), err.decode("utf-8", "replace"), cpus, bool(burners)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--fid", default=os.path.join("release", "fid.exe"))
    ap.add_argument("-n", type=int, default=1000)
    ap.add_argument("--shards", type=int, default=4)
    ap.add_argument("--backends", default="ccompile,emit")
    ap.add_argument("--programs", default=os.path.join("tests", "verify", "interleave.fd"))
    ap.add_argument("--timeout", type=int, default=60)
    ap.add_argument("--out", default=os.path.join("build", "interleave"))
    a = ap.parse_args()
    os.environ.setdefault("CC", "zig cc")
    os.makedirs(a.out, exist_ok=True)
    allok = True
    for prog in a.programs.split(","):
        name = os.path.basename(prog)[:-3]
        for b in a.backends.split(","):
            exe = os.path.abspath(os.path.join(a.out, f"{name}-{b}.exe"))
            p = subprocess.run([a.fid, b, prog, "-o", exe], capture_output=True, text=True, errors="replace")
            if p.returncode != 0 or not os.path.exists(exe):
                print(f"{name} {b}: build failed: {(p.stdout + p.stderr)[-600:]}")
                allok = False
                continue
            ref = one(exe, 0, a.timeout)
            outs = {}
            bad = []
            t0 = time.time()
            with ThreadPoolExecutor(a.shards) as ex:
                for i, res in enumerate(ex.map(lambda s: one(exe, s, a.timeout), range(1, a.n + 1))):
                    rc, out, err, cpus, loaded = res
                    key = (rc, out)
                    outs[key] = outs.get(key, 0) + 1
                    if key != (ref[0], ref[1]):
                        bad.append((i + 1, res))
                        if len(bad) <= 5:
                            with open(os.path.join(a.out, f"{name}-{b}-run{i + 1}.out"), "w", encoding="utf-8") as f:
                                f.write(f"exit {rc}, cpus {cpus}, loaded {loaded}\n{out}\n--- stderr ---\n{err[-3000:]}")
            ok = not bad and ref[0] == 0
            allok &= ok
            print(f"{'PASS' if ok else 'FAIL'} {name} {b}: {a.n} runs in {time.time() - t0:.0f} s, "
                  f"{len(outs)} distinct result(s); reference exit {ref[0]}: {ref[1].strip().replace(chr(10), ' | ')}")
            for (rc, out), c in sorted(outs.items(), key=lambda kv: -kv[1])[:4]:
                if (rc, out) != (ref[0], ref[1]):
                    print(f"     {c} run(s): exit {rc}: {out.strip().replace(chr(10), ' | ')[:120]}")
    sys.exit(0 if allok else 1)


if __name__ == "__main__":
    main()
