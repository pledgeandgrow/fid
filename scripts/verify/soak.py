#!/usr/bin/env python3
"""Soak test: a service under load for a long time, with flat memory (VF16).

    python scripts/verify/soak.py --fid release/fid.exe [--duration 3600]
        [--backend ccompile|emit] [--program tests/verify/soak_service.fd]
        [--sample 10] [--max-growth-mb-per-hour 1] [--out build/soak]

Builds the program once, runs it with no request limit, and every --sample
seconds records its resident memory (working set) and its last heartbeat
line. At the end:

- the process must still be running and still printing heartbeats;
- memory over the second half of the run must be flat: the slope of a
  least-squares line through those samples, in MB per hour, must stay under
  --max-growth-mb-per-hour (the first half is warm-up: caches fill, the
  allocator takes its arenas).

Release I6 asks for 24 hours and VF16 for a week; --duration sets it.
The samples are written to <out>/soak-<backend>.csv.
"""
import argparse
import os
import subprocess
import sys
import threading
import time

import psutil


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--fid", default=os.path.join("release", "fid.exe"))
    ap.add_argument("--duration", type=int, default=3600)
    ap.add_argument("--backend", default="ccompile")
    ap.add_argument("--program", default=os.path.join("tests", "verify", "soak_service.fd"))
    ap.add_argument("--sample", type=float, default=10)
    ap.add_argument("--max-growth-mb-per-hour", type=float, default=1.0)
    ap.add_argument("--out", default=os.path.join("build", "soak"))
    a = ap.parse_args()
    os.environ.setdefault("CC", "zig cc")
    os.makedirs(a.out, exist_ok=True)
    exe = os.path.abspath(os.path.join(a.out, f"soak-{a.backend}.exe"))
    p = subprocess.run([a.fid, a.backend, a.program, "-o", exe], capture_output=True, text=True, errors="replace")
    if p.returncode != 0 or not os.path.exists(exe):
        sys.exit("build failed:\n" + (p.stdout + p.stderr)[-2000:])
    proc = subprocess.Popen([exe, "0"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    last = {"line": "", "t": time.time(), "n": 0}

    def reader():
        for line in proc.stdout:
            last["line"] = line.strip()
            last["t"] = time.time()
            last["n"] += 1

    threading.Thread(target=reader, daemon=True).start()
    ps = psutil.Process(proc.pid)
    samples = []
    start = time.time()
    csv = os.path.join(a.out, f"soak-{a.backend}.csv")
    with open(csv, "w") as f:
        f.write("seconds,rss_mb,heartbeats,last\n")
        while time.time() - start < a.duration:
            time.sleep(a.sample)
            if proc.poll() is not None:
                break
            try:
                rss = ps.memory_info().rss / 1e6
            except psutil.Error:
                break
            t = time.time() - start
            samples.append((t, rss))
            f.write(f"{t:.1f},{rss:.2f},{last['n']},{last['line']}\n")
            f.flush()
    alive = proc.poll() is None
    stale = time.time() - last["t"]
    if alive:
        proc.kill()
    err = proc.stderr.read() if not alive else ""
    half = [s for s in samples if s[0] >= a.duration / 2] or samples
    slope = 0.0
    if len(half) >= 2:
        n = len(half)
        mt = sum(t for t, _ in half) / n
        mr = sum(r for _, r in half) / n
        num = sum((t - mt) * (r - mr) for t, r in half)
        den = sum((t - mt) ** 2 for t, _ in half) or 1
        slope = num / den * 3600
    peak = max((r for _, r in samples), default=0)
    first = samples[0][1] if samples else 0
    print(f"{a.backend}: {time.time() - start:.0f} s, {last['n']} heartbeats (last: {last['line']}), "
          f"memory {first:.1f} MB at first sample, peak {peak:.1f} MB, second-half slope {slope:+.2f} MB/hour")
    ok = True
    if not alive:
        print(f"FAIL: the process ended early (exit {proc.returncode}): {err[-500:]}")
        ok = False
    elif stale > max(60, 6 * a.sample):
        print(f"FAIL: no heartbeat for {stale:.0f} s")
        ok = False
    if slope > a.max_growth_mb_per_hour:
        print(f"FAIL: memory grows {slope:.2f} MB/hour (limit {a.max_growth_mb_per_hour})")
        ok = False
    print(("PASS" if ok else "FAIL") + f" (samples in {csv})")
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
