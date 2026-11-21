#!/usr/bin/env python3
"""Run the layout benchmark ports (bench/contig/*.fd) and time them.

    python bench/contig/run.py [--fid release/fid-dev.exe] [--n small|full|N]
                              [--bench sum,words,sexp] [--reps 3] [--native]

The ports are faithful: the source suite's xorshift64 RNG and algorithms, so
the checksums must match the source suite's rows for the same BENCH_N (see
github.com/aardappel/goose bench/results.md). Times are whole-process
medians, which for the interpreter includes the front end; --native builds
each benchmark once with `fid ccompile` (CC must name a C compiler) and
times only the binary. On a loaded machine every number is an upper bound:
run on a quiet machine before quoting any of it (the repo's bench gates
follow the same rule).

Sizes: small = N/20 (interpreter-friendly), full = the source suite's BENCH_N.
"""
import argparse
import os
import statistics
import subprocess
import sys
import tempfile
import time

BENCHES = {
    "sum": 4_000_000,
    "words": 2_000_000,
    "sexp": 200_000,
}
# small-n divisors: the interpreter's ~127MB memory limit caps sexp around
# ~250k nodes (n=2500); words and sum are comfortable at N/20
SMALL_DIV = {"sum": 20, "words": 20, "sexp": 100}


def run_interp(fid, path, n, reps, cc=None):
    times, out = [], None
    for _ in range(reps):
        t0 = time.perf_counter()
        r = subprocess.run([fid, "run", path, str(n)], capture_output=True,
                           text=True, timeout=1800)
        times.append(time.perf_counter() - t0)
        if r.returncode != 0:
            print("    FAILED:", (r.stderr or "").strip().splitlines()[-1][:120])
            return None, None
        out = r.stdout.strip()
    return statistics.median(times), out


def run_native(fid, path, n, reps, cc):
    exe = os.path.join(tempfile.gettempdir(), os.path.basename(path) + ".exe")
    env = dict(os.environ, CC=cc)
    r = subprocess.run([fid, "ccompile", path, "-o", exe], env=env,
                       capture_output=True, text=True, timeout=1800)
    if r.returncode != 0:
        print("    build FAILED:", (r.stderr or "").strip().splitlines()[-1][:120])
        return None, None
    times, out = [], None
    for _ in range(reps):
        t0 = time.perf_counter()
        r = subprocess.run([exe, str(n)], capture_output=True, text=True, timeout=1800)
        times.append(time.perf_counter() - t0)
        if r.returncode != 0:
            print("    FAILED:", (r.stderr or "").strip()[:120])
            return None, None
        out = r.stdout.strip()
    return statistics.median(times), out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--fid", default="release/fid-dev.exe")
    ap.add_argument("--n", default="small", help="small | full | an integer")
    ap.add_argument("--bench", default=",".join(BENCHES))
    ap.add_argument("--reps", type=int, default=3)
    ap.add_argument("--native", action="store_true")
    ap.add_argument("--cc", default="zig cc")
    args = ap.parse_args()

    root = os.path.dirname(os.path.abspath(__file__))
    print(f"{'bench':8} {'n':>9} {'median s':>9}  checksum")
    for name in args.bench.split(","):
        path = os.path.join(root, name + ".fd")
        full = BENCHES[name]
        n = full if args.n == "full" else full // SMALL_DIV[name] if args.n == "small" else int(args.n)
        for mode in (["interp", "native"] if args.native else ["interp"]):
            t, out = (run_native if mode == "native" else run_interp)(
                args.fid, path, n, args.reps, args.cc)
            if t is None:
                print(f"{name:8} {n:9} {'FAILED':>9}  ({mode})")
            else:
                first = out.splitlines()[0] if out else ""
                last = out.splitlines()[-1] if out else ""
                print(f"{name:8} {n:9} {t:9.3f}  {mode} {first}..{last}")


if __name__ == "__main__":
    main()
