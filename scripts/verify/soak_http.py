#!/usr/bin/env python3
"""Socket soak test (release-roadmap I6): an HTTP service on a real socket
under an external load generator, for a long time, with flat memory and no
failed request.

    python scripts/verify/soak_http.py --fid release/fid.exe [--duration 86400]
        [--backend ccompile|emit] [--program tests/verify/soak_http_service.fd]
        [--port 8099] [--clients 8] [--loadgen builtin|wrk|hey]
        [--sample 10] [--max-growth-mb-per-hour 1] [--out build/soak-http]

The program (default tests/verify/soak_http_service.fd, served with
`http_serve_scoped`) is built once and started with the port as its
argument. The load comes from another process:

- builtin: this script's own client threads (--clients of them). Each
  thread alternates keep-alive connections (http.client, several requests
  per connection) with a new connection per request, over GET /,
  GET /item?id=N and POST /echo, and checks every status and body;
- wrk / hey: that tool, rerun in --chunk second runs for the whole
  duration (no body check; its error and non-2xx counts must be 0). The
  builtin client also runs, with one thread, so bodies are still checked.

Every --sample seconds the service's resident memory (working set) is
recorded with the request and failure counts. At the end:

- the service must still be running and still answering;
- no request may have failed (wrong status, wrong body, a refused or
  reset connection, a timeout);
- memory over the second half of the run must be flat: the slope of a
  least-squares line through those samples, in MB per hour, must stay
  under --max-growth-mb-per-hour (the first half is warm-up).

Release I6 asks for 24 hours: --duration 86400. The samples are written to
<out>/soak-http-<backend>.csv; the failures (at most 50) to
<out>/failures.txt. Needs psutil (pip install psutil).
"""
import argparse
import http.client
import os
import random
import shutil
import socket
import subprocess
import sys
import threading
import time

try:
    import psutil
except ImportError:  # pragma: no cover
    sys.exit("soak_http: needs psutil (pip install psutil)")


class Stats:
    def __init__(self):
        self.lock = threading.Lock()
        self.ok = 0
        self.failed = 0
        self.first_failures = []
        self.ext_requests = 0
        self.ext_errors = 0
        self.connections = 0
        self.client_waits = 0
        self.client_restarts = 0

    def good(self):
        with self.lock:
            self.ok += 1

    def bad(self, what):
        with self.lock:
            self.failed += 1
            if len(self.first_failures) < 50:
                self.first_failures.append(f"{time.strftime('%H:%M:%S')} {what}")


def expected(kind, n):
    """(method, path, body, the reply body the service must give)."""
    if kind == 0:
        return "GET", "/", None, "ok\n"
    if kind == 1:
        return "GET", f"/item?id={n}", None, f'{{"id": {n}, "double": {2 * n}, "name": "item{n % 997}"}}'
    body = f"echo {n} " + "x" * (n % 200)
    return "POST", "/echo", body, body


KINDS = [0, 1, 2]


def one_request(conn, stats, rng):
    kind = rng.choice(KINDS)
    n = rng.randrange(1_000_000)
    method, path, body, want = expected(kind, n)
    headers = {"Content-Type": "text/plain"} if body is not None else {}
    conn.request(method, path, body=body, headers=headers)
    resp = conn.getresponse()
    got = resp.read().decode("utf-8", "replace")
    if resp.status != 200:
        stats.bad(f"{method} {path}: status {resp.status}")
        return False
    if got != want:
        stats.bad(f"{method} {path}: body {got[:80]!r}, expected {want[:80]!r}")
        return False
    stats.good()
    return True


# connect() failing for want of a local port (the client's ephemeral
# ports all in TIME_WAIT) is the load generator's limit, not the service's
CLIENT_PORTS = {98, 99, 10048, 10049, 10055}


def client(port, stats, stop, seed, new_share, per_conn):
    """One load thread. An unexpected exception (a MemoryError under the
    machine's commit limit was seen in a 30-minute run) used to end the
    thread silently, which only lowered the load: it is counted as a failure
    and the loop restarts."""
    restarts = 0
    while not stop.is_set():
        try:
            client_loop(port, stats, stop, seed + 7919 * restarts, new_share, per_conn)
            return
        except Exception as e:  # noqa: BLE001 - every escape is reported
            restarts += 1
            with stats.lock:
                stats.client_restarts += 1
            if not stop.is_set():
                stats.bad(f"client thread {seed}: {type(e).__name__}: {e} (restarted)")
                time.sleep(1)


def client_loop(port, stats, stop, seed, new_share, per_conn):
    rng = random.Random(seed)
    while not stop.is_set():
        keep_alive = rng.random() >= new_share
        conn = http.client.HTTPConnection("127.0.0.1", port, timeout=30)
        try:
            conn.connect()
        except OSError as e:
            if getattr(e, "winerror", None) in CLIENT_PORTS or e.errno in CLIENT_PORTS:
                with stats.lock:
                    stats.client_waits += 1
                time.sleep(1)
            elif not stop.is_set():
                stats.bad(f"connect: {type(e).__name__}: {e}")
                time.sleep(0.1)
            conn.close()
            continue
        with stats.lock:
            stats.connections += 1
        try:
            for _ in range(rng.randrange(1, per_conn + 1) if keep_alive else 1):
                if stop.is_set() or not one_request(conn, stats, rng):
                    break
        except (OSError, http.client.HTTPException) as e:
            if not stop.is_set():
                stats.bad(f"connection: {type(e).__name__}: {e}")
                time.sleep(0.1)
        finally:
            conn.close()


def external(tool, port, stats, stop, chunk, out):
    """wrk or hey, rerun in chunks; their error counts go into stats."""
    url = f"http://127.0.0.1:{port}/item?id=42"
    log = open(os.path.join(out, f"{tool}.log"), "a")
    while not stop.is_set():
        if tool == "wrk":
            cmd = ["wrk", "-t2", "-c16", f"-d{chunk}s", "--latency", url]
        else:
            cmd = ["hey", "-z", f"{chunk}s", "-c", "16", url]
        p = subprocess.run(cmd, capture_output=True, text=True, errors="replace")
        log.write(p.stdout + p.stderr)
        log.flush()
        reqs, errs = parse_external(tool, p.stdout)
        with stats.lock:
            stats.ext_requests += reqs
            stats.ext_errors += errs
        if p.returncode != 0 or errs:
            stats.bad(f"{tool}: exit {p.returncode}, {errs} errors in a {chunk} s run")


def parse_external(tool, text):
    reqs = errs = 0
    for line in text.splitlines():
        s = line.strip()
        if tool == "wrk":
            # "123456 requests in 30.00s, 12.34MB read"
            if " requests in " in s:
                reqs = int(s.split()[0])
            # "Socket errors: connect 0, read 0, write 0, timeout 0"
            if s.startswith("Socket errors:"):
                errs += sum(int(x.strip().split()[-1]) for x in s.split(":", 1)[1].split(","))
            # "Non-2xx or 3xx responses: 12"
            if s.startswith("Non-2xx"):
                errs += int(s.split(":")[1])
        else:
            # "[200]	123456 responses"; any other status is an error
            if s.startswith("[") and "responses" in s:
                code = s[1:s.index("]")]
                count = int(s.split()[1])
                reqs += count
                if code != "200":
                    errs += count
            if s.startswith("Error distribution:"):
                errs += 1
    return reqs, errs


def wait_port(port, proc, seconds=30):
    end = time.time() + seconds
    while time.time() < end:
        if proc.poll() is not None:
            return False
        try:
            socket.create_connection(("127.0.0.1", port), timeout=1).close()
            return True
        except OSError:
            time.sleep(0.2)
    return False


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--fid", default=os.path.join("release", "fid.exe"))
    ap.add_argument("--duration", type=int, default=3600)
    ap.add_argument("--backend", default="ccompile")
    ap.add_argument("--program", default=os.path.join("tests", "verify", "soak_http_service.fd"))
    ap.add_argument("--port", type=int, default=8099)
    ap.add_argument("--clients", type=int, default=8)
    ap.add_argument("--loadgen", default="builtin", choices=["builtin", "wrk", "hey"])
    ap.add_argument("--new-conn-share", type=float, default=0.1,
                    help="share of connections that carry one request (the rest keep alive)")
    ap.add_argument("--per-conn", type=int, default=100,
                    help="at most this many requests on a keep-alive connection")
    ap.add_argument("--kinds", default="0,1,2",
                    help="requests to send: 0 GET /, 1 GET /item, 2 POST /echo")
    ap.add_argument("--chunk", type=int, default=60, help="seconds per wrk/hey run")
    ap.add_argument("--sample", type=float, default=10)
    ap.add_argument("--max-growth-mb-per-hour", type=float, default=1.0)
    ap.add_argument("--out", default=os.path.join("build", "soak-http"))
    a = ap.parse_args()
    KINDS[:] = [int(k) for k in a.kinds.split(",")]
    if a.loadgen != "builtin" and not shutil.which(a.loadgen):
        sys.exit(f"soak_http: {a.loadgen} is not on PATH")
    os.environ.setdefault("CC", "zig cc")
    os.makedirs(a.out, exist_ok=True)
    exe = os.path.abspath(os.path.join(a.out, f"soak-http-{a.backend}.exe"))
    p = subprocess.run([a.fid, a.backend, a.program, "-o", exe], capture_output=True, text=True, errors="replace")
    if p.returncode != 0 or not os.path.exists(exe):
        sys.exit("build failed:\n" + (p.stdout + p.stderr)[-2000:])
    proc = subprocess.Popen([exe, str(a.port)], stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
    if not wait_port(a.port, proc):
        err = proc.stderr.read().decode("utf-8", "replace") if proc.poll() is not None else ""
        if proc.poll() is None:
            proc.kill()
        sys.exit(f"soak_http: the service does not accept connections on {a.port} {err[-500:]}")

    stats = Stats()
    stop = threading.Event()
    threads = []
    n_builtin = a.clients if a.loadgen == "builtin" else 1
    for i in range(n_builtin):
        threads.append(threading.Thread(target=client, args=(a.port, stats, stop, 1000 + i, a.new_conn_share, a.per_conn), daemon=True))
    if a.loadgen != "builtin":
        threads.append(threading.Thread(target=external, args=(a.loadgen, a.port, stats, stop, a.chunk, a.out), daemon=True))
    for t in threads:
        t.start()

    ps = psutil.Process(proc.pid)
    samples = []
    # (seconds, requests answered so far): the load must keep going
    progress = []
    start = time.time()
    csv = os.path.join(a.out, f"soak-http-{a.backend}.csv")
    with open(csv, "w") as f:
        f.write("seconds,rss_mb,ok,failed,external_requests\n")
        while time.time() - start < a.duration:
            time.sleep(min(a.sample, max(0.1, a.duration - (time.time() - start))))
            if proc.poll() is not None:
                break
            try:
                rss = ps.memory_info().rss / 1e6
            except psutil.Error:
                break
            t = time.time() - start
            samples.append((t, rss))
            progress.append((t, stats.ok + stats.ext_requests))
            f.write(f"{t:.1f},{rss:.2f},{stats.ok},{stats.failed},{stats.ext_requests}\n")
            f.flush()

    # still answering at the end?
    # (the clients stop first: they may hold every local port for a while)
    stop.set()
    answering = False
    end = time.time() + 120
    while proc.poll() is None and time.time() < end and not answering:
        try:
            c = http.client.HTTPConnection("127.0.0.1", a.port, timeout=10)
            c.request("GET", "/")
            answering = c.getresponse().read() == b"ok\n"
            c.close()
        except OSError as e:
            if getattr(e, "winerror", None) not in CLIENT_PORTS and e.errno not in CLIENT_PORTS:
                break
            time.sleep(2)
        except http.client.HTTPException:
            break
    for t in threads:
        t.join(timeout=a.chunk + 30 if a.loadgen != "builtin" else 35)
    alive = proc.poll() is None
    if alive:
        proc.kill()
    err = proc.stderr.read().decode("utf-8", "replace") if not alive else ""

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
    last = samples[-1][1] if samples else 0
    elapsed = time.time() - start
    total = stats.ok + stats.ext_requests
    with open(os.path.join(a.out, "failures.txt"), "w") as f:
        f.write("\n".join(stats.first_failures) + ("\n" if stats.first_failures else ""))
    print(f"{a.backend} over a socket ({a.loadgen}): {elapsed:.0f} s, {total} requests "
          f"({stats.ok} checked by body, {stats.ext_requests} by {a.loadgen if a.loadgen != 'builtin' else 'nothing else'}), "
          f"{stats.failed} failed, {total / max(elapsed, 1):.0f}/s, {stats.connections} connections"
          f"{f' ({stats.client_waits} waits for a client port)' if stats.client_waits else ''}; memory {first:.1f} MB at first sample, "
          f"{last:.1f} at the last, peak {peak:.1f}, second-half slope {slope:+.2f} MB/hour")
    ok = True
    if not alive or not answering:
        print(f"FAIL: the service {'ended early (exit ' + str(proc.returncode) + ')' if not alive else 'stopped answering'} {err[-500:]}")
        ok = False
    if stats.failed:
        print(f"FAIL: {stats.failed} requests failed; the first:")
        for x in stats.first_failures[:10]:
            print("  " + x)
        ok = False
    if stats.ok == 0:
        print("FAIL: no request was answered")
        ok = False
    # the answered count must still grow at the end: over the last three
    # samples (wrk/hey report once per chunk: over a chunk and two samples)
    window = 3 * a.sample if a.loadgen == "builtin" else a.chunk + 2 * a.sample
    if progress and progress[-1][0] - progress[0][0] >= window:
        tail = [p for p in progress if p[0] >= progress[-1][0] - window]
        if len(tail) >= 2 and tail[-1][1] <= tail[0][1]:
            print(f"FAIL: no request answered in the last {window:.0f} s of the run "
                  f"(stuck at {tail[-1][1]}; {stats.client_restarts} client restarts)")
            ok = False
    if stats.client_restarts:
        print(f"note: {stats.client_restarts} client threads were restarted after an unexpected error")
    if slope > a.max_growth_mb_per_hour:
        print(f"FAIL: memory grows {slope:.2f} MB/hour (limit {a.max_growth_mb_per_hour})")
        ok = False
    print(("PASS" if ok else "FAIL") + f" (samples in {csv})")
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
