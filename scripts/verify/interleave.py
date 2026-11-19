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

This perturbs the OS scheduler; it does not choose the interleaving.
--sched-random (ccompile, and native builds: emit on Windows and Linux) also sets
FIDES_SCHED_RANDOM=<run's seed>: the runtime's scheduler then picks the
next task at random, yields at loop safepoints at random and sleeps at
random at thread starts, mutexes, atomics and channels (roadmap RL17,
pe_runtime/conc.fd and linux_runtime/conc.fd "Random-choice scheduling",
compiler/codegen_c.fd "Roadmap RL17").
A failing run's seed reproduces the scheduler's choices. Every switch point
enumerated, as CHESS or loom do, is not done.

Replay and record (Nov 18):
    --seeds 17,42     run only these seeds (the reference run, seed 0, too)
                      and print each one's output: a failing run's line
                      names its seed and the command that replays it
    --record FILE     append one Markdown table row per program and backend
                      (date, compiler, runs, shards, mode, result, time) to
                      FILE, e.g. docs/verification/vf18-interleave-runs.md
A program with an '// Expected output' comment block (one '// line' per
printed line) is held to it: the reference run and every run must print it.
A seed fixes the CPU set, the load and (--sched-random) the runtime
scheduler's choices. A program whose tasks all run on one thread replays its
schedule exactly on the C path; with OS threads the OS still decides part of
the interleaving, so a seed makes a failure likely to come back, not certain.
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


SCHED_RANDOM = False


def one(exe, seed, timeout):
    r = random.Random(seed)
    env = dict(os.environ)
    if SCHED_RANDOM and seed > 0:
        env["FIDES_SCHED_RANDOM"] = str(seed)
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
        p = subprocess.Popen([exe], stdout=subprocess.PIPE, stderr=subprocess.PIPE, env=env)
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
    ap.add_argument("--sched-random", action="store_true",
                    help="runs 1..n set FIDES_SCHED_RANDOM to their seed (native runtimes)")
    ap.add_argument("--seeds", default="",
                    help="comma-separated seeds to run instead of 1..n (replay)")
    ap.add_argument("--record", default="",
                    help="append a Markdown row per program and backend to this file")
    a = ap.parse_args()
    try:
        sys.stdout.reconfigure(line_buffering=True)  # each result as it comes, into a log too
    except AttributeError:
        pass
    global SCHED_RANDOM
    SCHED_RANDOM = a.sched_random
    os.environ.setdefault("CC", "zig cc")
    os.makedirs(a.out, exist_ok=True)
    seeds = [int(s) for s in a.seeds.split(",") if s.strip()] if a.seeds else list(range(1, a.n + 1))
    allok = True
    for prog in a.programs.split(","):
        name = os.path.basename(prog)[:-3]
        want = expected_output(prog)
        for b in a.backends.split(","):
            exe = os.path.abspath(os.path.join(a.out, f"{name}-{b}.exe"))
            p = subprocess.run([a.fid, b, prog, "-o", exe], capture_output=True, text=True, errors="replace")
            if p.returncode != 0 or not os.path.exists(exe):
                print(f"{name} {b}: build failed: {(p.stdout + p.stderr)[-600:]}")
                allok = False
                record(a, name, b, len(seeds), "build failed", 0, 0)
                continue
            ref = one(exe, 0, a.timeout)
            ref_ok = ref[0] == 0 and (want is None or ref[1].strip() == want)
            if want is not None and ref[1].strip() != want:
                print(f"     {name} {b}: the reference run (seed 0) does not print the "
                      f"program's 'Expected output' block")
            outs = {}
            bad = []
            t0 = time.time()
            with ThreadPoolExecutor(a.shards) as ex:
                for seed, res in zip(seeds, ex.map(lambda s: one(exe, s, a.timeout), seeds)):
                    rc, out, err, cpus, loaded = res
                    key = (rc, out)
                    outs[key] = outs.get(key, 0) + 1
                    if a.seeds:
                        print(f"     seed {seed}: exit {rc}, cpus {cpus}, loaded {loaded}: "
                              f"{out.strip().replace(chr(10), ' | ')[:200]}")
                    # with an expected block, a run is right when it prints
                    # it; without one, when it matches the reference run
                    wrong = (rc != 0 or out.strip() != want) if want is not None else key != (ref[0], ref[1])
                    if wrong:
                        bad.append((seed, res))
                        if len(bad) <= 5:
                            with open(os.path.join(a.out, f"{name}-{b}-seed{seed}.out"), "w", encoding="utf-8") as f:
                                f.write(f"seed {seed}, exit {rc}, cpus {cpus}, loaded {loaded}\n{out}\n"
                                        f"--- stderr ---\n{err[-3000:]}")
            secs = time.time() - t0
            ok = not bad and ref_ok
            allok &= ok
            print(f"{'PASS' if ok else 'FAIL'} {name} {b}: {len(seeds)} runs in {secs:.0f} s, "
                  f"{len(outs)} distinct result(s); reference exit {ref[0]}: {ref[1].strip().replace(chr(10), ' | ')}")
            for (rc, out), c in sorted(outs.items(), key=lambda kv: -kv[1])[:4]:
                if (rc, out) != (ref[0], ref[1]) or not ref_ok:
                    print(f"     {c} run(s): exit {rc}: {out.strip().replace(chr(10), ' | ')[:120]}")
            if bad:
                first = ",".join(str(s) for s, _ in bad[:10])
                print(f"     failing seeds: {first}{' ...' if len(bad) > 10 else ''}; replay with: "
                      f"python scripts/verify/interleave.py --fid {a.fid} --backends {b} --programs {prog}"
                      f"{' --sched-random' if a.sched_random else ''} --seeds {first}")
            record(a, name, b, len(seeds), "PASS" if ok else f"FAIL ({len(bad)} wrong)", secs, len(outs))
    sys.exit(0 if allok else 1)


def expected_output(prog):
    """The program's '// Expected output' comment block, or None."""
    try:
        with open(prog, encoding="utf-8") as f:
            lines = f.read().replace("\r", "").split("\n")
    except OSError:
        return None
    for i, l in enumerate(lines):
        if l.startswith("//") and "Expected output" in l:
            got = []
            for m in lines[i + 1:]:
                if not m.startswith("// ") or not m[3:].strip():
                    break
                got.append(m[3:].rstrip())
            return "\n".join(got) if got else None
    return None


def record(a, name, backend, runs, result, secs, distinct):
    if not a.record:
        return
    new = not os.path.exists(a.record)
    with open(a.record, "a", encoding="utf-8") as f:
        if new:
            f.write("| date | compiler | program | backend | runs | shards | mode | result | distinct | time |\n"
                    "|---|---|---|---|---|---|---|---|---|---|\n")
        mode = "sched-random" if a.sched_random else "OS scheduler"
        f.write(f"| {time.strftime('%Y-%m-%d %H:%M')} | {os.path.basename(a.fid)} | {name} | {backend} | "
                f"{runs} | {a.shards} | {mode} | {result} | {distinct} | {secs:.0f} s |\n")


if __name__ == "__main__":
    main()
