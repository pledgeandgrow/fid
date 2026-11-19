#!/usr/bin/env python3
"""HTTP/1.1 load client of the service benchmarks (competitive roadmap BM5).

    python bench/service/load.py --url http://HOST:PORT --test plaintext
        [--connections 64] [--duration 10] [--warmup 2] [--check-only]

Keeps --connections keep-alive connections busy for --duration seconds
(after --warmup seconds that are not counted), one request in flight per
connection, and checks every reply's status and body against the test:

    plaintext   GET /plaintext          body "Hello, World!"
    json        GET /json               {"message": "Hello, World!"}
    db          GET /db                 {"id": N, "randomNumber": M}, with M
                                        the World row of N (every server of
                                        bench/service holds the same table)
    queries     GET /queries?queries=20 20 such rows

Prints one JSON line: test, requests, rps, errors, p50_ms, p99_ms. Exits 1
when a reply was wrong or a connection failed. --check-only sends one
request per test and checks it (what run.sh does before wrk, which does
not look at bodies).

Python's asyncio client tops out at some tens of thousands of requests per
second: on a fast server the client is the limit, so run.sh uses wrk when
it is installed (LOADGEN=wrk) and says which client measured each table.
"""
import argparse
import asyncio
import json
import sys
import time
from urllib.parse import urlparse

WORLD_ROWS = 10000
TESTS = {
    "plaintext": "/plaintext",
    "json": "/json",
    "db": "/db",
    "queries": "/queries?queries=20",
}


def world(i):
    return (i * 7919 + 13) % WORLD_ROWS + 1


def good_row(r):
    return (isinstance(r, dict) and set(r) == {"id", "randomNumber"}
            and isinstance(r["id"], int) and 1 <= r["id"] <= WORLD_ROWS
            and r["randomNumber"] == world(r["id"]))


def check(test, status, body):
    """None when the reply is right, else what is wrong."""
    if status != 200:
        return f"status {status}"
    try:
        if test == "plaintext":
            return None if body == b"Hello, World!" else f"body {body[:60]!r}"
        doc = json.loads(body)
    except ValueError:
        return f"not JSON: {body[:60]!r}"
    if test == "json":
        return None if doc == {"message": "Hello, World!"} else f"body {doc!r}"[:80]
    if test == "db":
        return None if good_row(doc) else f"row {doc!r}"[:80]
    if test == "queries":
        if isinstance(doc, list) and len(doc) == 20 and all(good_row(r) for r in doc):
            return None
        return f"rows {str(doc)[:60]}"
    return f"unknown test {test}"


async def read_reply(reader):
    head = await reader.readuntil(b"\r\n\r\n")
    lines = head.decode("latin-1").split("\r\n")
    status = int(lines[0].split()[1])
    headers = {}
    for line in lines[1:]:
        if ":" in line:
            k, v = line.split(":", 1)
            headers[k.strip().lower()] = v.strip()
    if "content-length" in headers:
        body = await reader.readexactly(int(headers["content-length"]))
    elif headers.get("transfer-encoding", "").lower() == "chunked":
        body = b""
        while True:
            size = int((await reader.readuntil(b"\r\n")).strip().split(b";")[0], 16)
            if size == 0:
                await reader.readuntil(b"\r\n")
                break
            body += await reader.readexactly(size)
            await reader.readexactly(2)
    else:
        body = await reader.read()
    return status, body, headers.get("connection", "").lower() == "close"


class Run:
    def __init__(self):
        self.count = 0
        self.errors = 0
        self.first_errors = []
        self.lat = []
        self.counting = False


async def worker(run, host, port, test, stop_at):
    request = (f"GET {TESTS[test]} HTTP/1.1\r\nHost: {host}:{port}\r\n"
               "Connection: keep-alive\r\n\r\n").encode()
    reader = writer = None
    while time.monotonic() < stop_at:
        try:
            if writer is None:
                reader, writer = await asyncio.open_connection(host, port)
            t0 = time.perf_counter()
            writer.write(request)
            status, body, closing = await asyncio.wait_for(read_reply(reader), 10)
            dt = time.perf_counter() - t0
            bad = check(test, status, body)
            if run.counting:
                if bad:
                    run.errors += 1
                    if len(run.first_errors) < 5:
                        run.first_errors.append(bad)
                else:
                    run.count += 1
                    run.lat.append(dt)
            if closing:
                writer.close()
                writer = None
        except (OSError, asyncio.IncompleteReadError, asyncio.TimeoutError, ValueError, IndexError) as e:
            if run.counting:
                run.errors += 1
                if len(run.first_errors) < 5:
                    run.first_errors.append(f"{type(e).__name__}: {e}")
            if writer is not None:
                writer.close()
            writer = None
            await asyncio.sleep(0.01)
    if writer is not None:
        writer.close()


async def measure(host, port, test, connections, duration, warmup):
    run = Run()
    start = time.monotonic()
    stop_at = start + warmup + duration
    tasks = [asyncio.create_task(worker(run, host, port, test, stop_at)) for _ in range(connections)]
    await asyncio.sleep(warmup)
    run.counting = True
    t0 = time.monotonic()
    await asyncio.gather(*tasks)
    elapsed = time.monotonic() - t0
    lat = sorted(run.lat)

    def pct(p):
        return round(lat[min(len(lat) - 1, int(len(lat) * p))] * 1000, 2) if lat else None
    return {
        "test": test, "requests": run.count, "rps": round(run.count / elapsed, 1),
        "errors": run.errors, "p50_ms": pct(0.50), "p99_ms": pct(0.99),
        "connections": connections, "seconds": round(elapsed, 2), "client": "load.py",
        "first_errors": run.first_errors,
    }


async def check_once(host, port, test):
    reader, writer = await asyncio.open_connection(host, port)
    writer.write(f"GET {TESTS[test]} HTTP/1.1\r\nHost: {host}\r\nConnection: close\r\n\r\n".encode())
    status, body, _ = await asyncio.wait_for(read_reply(reader), 10)
    writer.close()
    return check(test, status, body)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--url", required=True)
    ap.add_argument("--test", choices=sorted(TESTS), required=True)
    ap.add_argument("--connections", type=int, default=64)
    ap.add_argument("--duration", type=float, default=10)
    ap.add_argument("--warmup", type=float, default=2)
    ap.add_argument("--check-only", action="store_true")
    a = ap.parse_args()
    u = urlparse(a.url)
    host, port = u.hostname, u.port or 80
    if a.check_only:
        bad = asyncio.run(check_once(host, port, a.test))
        print(json.dumps({"test": a.test, "check": bad or "ok"}))
        return 1 if bad else 0
    res = asyncio.run(measure(host, port, a.test, a.connections, a.duration, a.warmup))
    print(json.dumps(res))
    return 1 if res["errors"] or res["requests"] == 0 else 0


if __name__ == "__main__":
    sys.exit(main())
