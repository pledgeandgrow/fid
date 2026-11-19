#!/usr/bin/env python3
"""Idle connections against a Fides server (competitive roadmap RT11).

    python bench/idle/idle.py --server <exe> [--steps 1000,10000,100000,1000000]
                              [--port 9300] [--ports 16] [--addrs 16]
                              [--samples 200] [--label "C path"]

Starts the server (bench/idle/server.fd built by bench/idle/run.sh) on
`ports` consecutive ports, then opens connections that send nothing, in
steps. After each step it records:

  open      connections held open (the server's own count, from a ping)
  rss MB    the server's resident memory (Linux /proc/<pid>/status VmRSS,
            Windows WorkingSetSize, macOS `ps -o rss`)
  KB/conn   the growth of rss since the server started, per connection
  open/s    how fast the step's connections were opened
  new p50/p99 ms   a fresh connection: connect, ping, pong, close
  idle p50/p99 ms  a ping on idle connections picked at random (the server
            must find the one ready socket among all the parked ones: an
            O(n) poll loop shows here as latency growing with `open`)

and prints one Markdown table row per step. A connection is to
127.0.0.<k>:<port> for k = 1..addrs (Linux answers on all of 127/8; macOS
and Windows only on the addresses that exist, so use --addrs 1 there): each
(address, port) pair has its own ~28k-64k ephemeral ports, so 1M
connections need about 16 x 16 pairs. Limits to raise for 1M (Linux):

  ulimit -n 1100000 (here and for the server: run.sh does it)
  sysctl -w fs.nr_open=2200000 fs.file-max=4400000
  sysctl -w vm.max_map_count=4194304   (before Linux 6.13 a task's stack is
            two mappings, its guard page mprotect'ed: the C path stopped
            at 589,808 tasks with 1048576, "task: out of memory" at 2.7 GB
            resident; a stock 65530 allows ~32k. 6.13+ installs the guard
            with MADV_GUARD_INSTALL: one mapping per stack. run.sh raises
            it when it can)
  sysctl -w net.ipv4.ip_local_port_range="1024 65535"
  sysctl -w net.core.somaxconn=65535 net.ipv4.tcp_max_syn_backlog=65535

A step that cannot be completed (out of descriptors, ports or memory) is
reported with how many it reached and the run stops there.
"""
import argparse
import os
import random
import socket
import struct
import subprocess
import sys
import time


# close with a reset: a million sockets in TIME_WAIT would hold the ports
LINGER0 = struct.pack("HH" if sys.platform == "win32" else "ii", 1, 0)


def raise_nofile():
    try:
        import resource
        soft, hard = resource.getrlimit(resource.RLIMIT_NOFILE)
        resource.setrlimit(resource.RLIMIT_NOFILE, (hard, hard))
        return hard
    except Exception:
        return -1


def rss_kb(pid):
    if sys.platform.startswith("linux"):
        try:
            with open(f"/proc/{pid}/status") as f:
                for line in f:
                    if line.startswith("VmRSS:"):
                        return int(line.split()[1])
        except OSError:
            return -1
    if sys.platform == "win32":
        import ctypes
        from ctypes import wintypes

        class PMC(ctypes.Structure):
            _fields_ = [("cb", wintypes.DWORD), ("PageFaultCount", wintypes.DWORD),
                        ("PeakWorkingSetSize", ctypes.c_size_t), ("WorkingSetSize", ctypes.c_size_t),
                        ("QuotaPeakPagedPoolUsage", ctypes.c_size_t), ("QuotaPagedPoolUsage", ctypes.c_size_t),
                        ("QuotaPeakNonPagedPoolUsage", ctypes.c_size_t), ("QuotaNonPagedPoolUsage", ctypes.c_size_t),
                        ("PagefileUsage", ctypes.c_size_t), ("PeakPagefileUsage", ctypes.c_size_t)]
        k32 = ctypes.WinDLL("kernel32")
        psapi = ctypes.WinDLL("psapi")
        k32.OpenProcess.restype = wintypes.HANDLE
        h = k32.OpenProcess(0x0400 | 0x0010, False, pid)  # QUERY_INFORMATION | VM_READ
        if not h:
            return -1
        pmc = PMC()
        pmc.cb = ctypes.sizeof(PMC)
        psapi.GetProcessMemoryInfo.argtypes = [wintypes.HANDLE, ctypes.POINTER(PMC), wintypes.DWORD]
        ok = psapi.GetProcessMemoryInfo(h, ctypes.byref(pmc), pmc.cb)
        k32.CloseHandle(h)
        return pmc.WorkingSetSize // 1024 if ok else -1
    try:
        out = subprocess.run(["ps", "-o", "rss=", "-p", str(pid)], capture_output=True, text=True).stdout
        return int(out.strip() or -1)
    except Exception:
        return -1


def ping(s, timeout=10.0):
    """Send a ping on socket s; return (ms, open count from the pong)."""
    s.settimeout(timeout)
    t0 = time.perf_counter()
    s.sendall(b"ping\n")
    data = b""
    while not data.endswith(b"\n"):
        chunk = s.recv(64)
        if not chunk:
            raise ConnectionError("closed")
        data += chunk
    ms = (time.perf_counter() - t0) * 1000.0
    parts = data.decode().split()
    if len(parts) != 2 or parts[0] != "pong":
        raise ValueError(f"bad reply {data!r}")
    return ms, int(parts[1])


def pct(xs, p):
    if not xs:
        return float("nan")
    xs = sorted(xs)
    return xs[min(len(xs) - 1, int(round(p / 100.0 * (len(xs) - 1))))]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--server", required=True)
    ap.add_argument("--steps", default="1000,10000,100000,1000000")
    ap.add_argument("--port", type=int, default=9300)
    ap.add_argument("--ports", type=int, default=16)
    ap.add_argument("--addrs", type=int, default=16 if sys.platform.startswith("linux") else 1)
    # source addresses 127.0.0.<k>: each has its own ephemeral ports (Windows
    # shares one pool of ~16k among all destinations of an unbound socket)
    ap.add_argument("--srcs", type=int, default=1)
    ap.add_argument("--samples", type=int, default=200)
    ap.add_argument("--label", default="")
    ap.add_argument("--out", default="")
    a = ap.parse_args()
    steps = [int(x) for x in a.steps.split(",") if x]
    nofile = raise_nofile()

    # the server's output goes to a file (a pipe nobody reads would stop
    # it once full); its last lines are reported if it exits
    log_path = (a.out or "idle") + ".server.log"
    log = open(log_path, "w")
    srv = subprocess.Popen([a.server, str(a.port), str(a.ports)], stdout=log, stderr=subprocess.STDOUT)
    t_start = time.time()
    while True:
        with open(log_path) as f:
            line = f.read()
        if "listening" in line:
            break
        if srv.poll() is not None or time.time() - t_start > 30:
            print(f"idle.py: the server did not start: {line!r}", file=sys.stderr)
            srv.kill()
            return 2
        time.sleep(0.05)
    targets = [(f"127.0.0.{k + 1}", a.port + p) for k in range(a.addrs) for p in range(a.ports)]
    time.sleep(0.3)
    base = rss_kb(srv.pid)
    conns = []
    rows = []
    rnd = random.Random(7)
    stopped = ""
    for want in steps:
        t0 = time.perf_counter()
        start_n = len(conns)
        try:
            while len(conns) < want:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.setsockopt(socket.SOL_SOCKET, socket.SO_LINGER, LINGER0)
                s.settimeout(30)
                if a.srcs > 1:
                    s.bind((f"127.0.0.{(len(conns) // len(targets)) % a.srcs + 1}", 0))
                s.connect(targets[len(conns) % len(targets)])
                conns.append(s)
        except OSError as e:
            stopped = f"stopped at {len(conns)} connections: {e}"
        opened = len(conns) - start_n
        open_s = opened / max(time.perf_counter() - t0, 1e-9)
        time.sleep(1.0)   # let the server accept the backlog
        new_ms, idle_ms = [], []
        server_open = -1
        for i in range(a.samples):
            try:
                c = socket.create_connection(targets[i % len(targets)], timeout=10)
                ms, server_open = ping(c)
                new_ms.append(ms)
                c.close()
            except OSError:
                pass
            if conns:
                try:
                    ms, server_open = ping(conns[rnd.randrange(len(conns))])
                    idle_ms.append(ms)
                except (OSError, ValueError):
                    pass
        rss = rss_kb(srv.pid)
        per = (rss - base) / len(conns) if conns and rss >= 0 and base >= 0 else float("nan")
        rows.append((len(conns), server_open, rss / 1024.0, per, open_s,
                     pct(new_ms, 50), pct(new_ms, 99), pct(idle_ms, 50), pct(idle_ms, 99),
                     len(new_ms), len(idle_ms)))
        if stopped or srv.poll() is not None:
            if srv.poll() is not None:
                with open(log_path) as f:
                    tail = " / ".join(f.read().strip().splitlines()[-3:])
                stopped = (stopped + "; " if stopped else "") + f"the server exited ({srv.returncode}): {tail}"
            break
    for s in conns:
        try:
            s.close()
        except OSError:
            pass
    srv.kill()
    srv.wait()

    out = []
    label = f" — {a.label}" if a.label else ""
    out.append(f"#### Idle connections{label}")
    out.append("")
    out.append("| held | server count | rss MB | KB/conn | open/s | new p50 ms | new p99 ms | idle p50 ms | idle p99 ms |")
    out.append("|---:|---:|---:|---:|---:|---:|---:|---:|---:|")
    for r in rows:
        out.append(f"| {r[0]} | {r[1]} | {r[2]:.1f} | {r[3]:.2f} | {r[4]:.0f} | {r[5]:.2f} | {r[6]:.2f} | {r[7]:.2f} | {r[8]:.2f} |")
    out.append("")
    out.append(f"{a.samples} samples each (new: {rows[-1][9] if rows else 0} answered, idle: {rows[-1][10] if rows else 0} answered "
               f"at the last step); {len(targets)} address:port pairs; server rss at start {base / 1024.0:.1f} MB; "
               f"{a.srcs} source address(es); descriptor limit {nofile}; {sys.platform}.")
    if stopped:
        out.append(f"The run {stopped}.")
    text = "\n".join(out)
    print(text)
    if a.out:
        with open(a.out, "w") as f:
            f.write(text + "\n")
    # every step answered: responsive. A step whose pings were not answered
    # (more than 5% lost) is a failure
    bad = [r for r in rows if r[9] < a.samples * 0.95 or (r[0] > 0 and r[10] < a.samples * 0.95)]
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
