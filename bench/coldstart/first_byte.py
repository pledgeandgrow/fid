#!/usr/bin/env python3
"""Cold start of a serverless handler (roadmap Phase 9, F32): the time from
starting the process to the first byte of its response on stdout.

    python3 first_byte.py <runs> <request> <exe> [<exe> ...]

Each executable is started <runs> times with <request> as argv[1]; the clock
starts before the process is created and stops when the first byte of
stdout arrives (the process may still be exiting). One line per executable:

    <exe> min_ms median_ms p90_ms <first line of the response>

Exit 1 when an executable fails to start, exits non-zero or answers
differently from one run to the next.
"""
import os
import subprocess
import sys
import time


def one_run(exe, request):
    t0 = time.perf_counter()
    p = subprocess.Popen([exe, request], stdout=subprocess.PIPE,
                         stderr=subprocess.DEVNULL, stdin=subprocess.DEVNULL)
    first = p.stdout.read(1)
    t1 = time.perf_counter()
    rest = p.stdout.read()
    rc = p.wait()
    return (t1 - t0) * 1000.0, rc, (first + rest).decode('utf-8', 'replace')


def main():
    if len(sys.argv) < 4:
        print(__doc__.strip())
        return 2
    runs = int(sys.argv[1])
    request = sys.argv[2]
    ok = True
    for exe in sys.argv[3:]:
        path = os.path.abspath(exe)
        if not os.path.exists(path):
            print('%s missing' % exe)
            ok = False
            continue
        # one warm-up run: the file in the page cache, as on a warm host
        _, rc, want = one_run(path, request)
        if rc != 0:
            print('%s exit %d' % (exe, rc))
            ok = False
            continue
        times = []
        same = True
        for _ in range(runs):
            ms, rc, got = one_run(path, request)
            if rc != 0 or got != want:
                same = False
            times.append(ms)
        times.sort()
        med = times[len(times) // 2]
        p90 = times[min(len(times) - 1, (len(times) * 9) // 10)]
        line = want.splitlines()[0] if want else ''
        print('%s %.2f %.2f %.2f %s' % (exe, times[0], med, p90, line))
        if not same:
            print('%s: the response or exit code changed between runs' % exe)
            ok = False
    return 0 if ok else 1


if __name__ == '__main__':
    sys.exit(main())
