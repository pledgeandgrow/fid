#!/usr/bin/env python3
"""The Linux half of the chaos tests (roadmap VF17): clock jumps and network
partitions, for scripts/verify/chaos.py (which runs this inside WSL on
Windows) or by hand on Linux.

    python3 scripts/verify/chaos_linux.py build --c chaos.c --out DIR
    python3 scripts/verify/chaos_linux.py clock --exe DIR/chaos-glibc --shim DIR/clockjump.so
    python3 scripts/verify/chaos_linux.py partition --exe DIR/chaos-glibc

`chaos.c` is `fid dump-c tests/verify/chaos.fd` (the same C on every
target: its runtime picks the OS with #ifdef). `build` compiles it with the
system's gcc or clang as a dynamic glibc executable, so that LD_PRELOAD
reaches it, and builds scripts/verify/clockjump.c beside it.

clock      Each case of `chaos clock <case>` (tests/verify/chaos.fd) runs once
           with the real clocks and once per jump, the jump coming 250 ms in,
           during the wait:
             realtime +1 h, realtime -1 h   an NTP step, a user setting the date
             both +1 h                      a VM paused or suspended for an hour
           A case passes when it prints its line, and on a realtime jump takes
           the time it names on the real clock (neither cut short nor
           stretched by the jump); on a forward jump of every clock, ending at
           the jump is right (the hour passed). --faketime LIB uses
           libfaketime (FAKETIME_TIMESTAMP_FILE, rewritten at the jump) for
           the realtime jumps instead of clockjump.so.
partition  Two network namespaces joined by a veth pair (an unprivileged user
           namespace: `unshare -rn`, no root needed), a server in one, the
           chaos program in the other. A partition drops every packet that
           arrives on either side (nftables, input hook: the sender is told
           nothing, as when a link between machines fails); `heal` removes
           the rules. Falls back to one namespace and the loopback device when
           a veth pair cannot be made, and to `tc netem loss 100%` when
           nftables is missing.

Each result is printed as `RESULT {json}` with name, ok, why and secs;
chaos.py reads those lines (and knows which open finding a failure is).
Exit 0 when every case passed.
"""
import argparse
import json
import os
import queue
import shutil
import signal
import socket
import subprocess
import sys
import threading
import time

HERE = os.path.dirname(os.path.abspath(__file__))
JUMP_AT_MS = 250
HOUR_MS = 3600 * 1000

# case, the time it takes on the real clock (s), the line it prints
CLOCK_CASES = [
    ("sleep", 1.0, "slept"),
    ("tasks", 0.9, "woke 300\nwoke 600\nwoke 900\ntasks 3 true"),
    ("timeout", 0.7, "timeout whole false late true"),
    ("token", 0.7, "token whole false"),
    ("cosleep", 0.7, "cosleep late true"),
    ("waitfd", 0.7, "waitfd timed out"),
]
# label, clock, by (ms)
JUMPS = [
    ("no jump", "realtime", 0),
    ("realtime +1h", "realtime", HOUR_MS),
    ("realtime -1h", "realtime", -HOUR_MS),
    ("all clocks +1h", "both", HOUR_MS),
]

SERVER_PORT = 18080
BODY = 200000
CHUNK = 8000


def result(name, ok, why, secs):
    print("RESULT " + json.dumps({"name": name, "ok": bool(ok), "why": why, "secs": round(secs, 2)}), flush=True)
    print(f"{'PASS' if ok else 'FAIL'}  {name:44} {secs:5.1f} s  {why}", file=sys.stderr, flush=True)
    return ok


def run_timed(cmd, env=None, limit=30.0):
    t = time.monotonic()
    try:
        p = subprocess.run(cmd, env=env, capture_output=True, timeout=limit)
        return (p.returncode, p.stdout.decode("utf-8", "replace").replace("\r", ""),
                p.stderr.decode("utf-8", "replace"), time.monotonic() - t)
    except subprocess.TimeoutExpired:
        return "timeout", "", "", time.monotonic() - t


# --- build -------------------------------------------------------------------

def cmd_build(a):
    cc = a.cc or shutil.which("gcc") or shutil.which("clang") or shutil.which("cc")
    if not cc:
        print("chaos_linux: no C compiler (gcc, clang or cc)", file=sys.stderr)
        return 2
    os.makedirs(a.out, exist_ok=True)
    exe = os.path.join(a.out, "chaos-glibc")
    shim = os.path.join(a.out, "clockjump.so")
    base = ["-O1", "-w", "-Wno-int-conversion", "-Wno-incompatible-pointer-types",
            "-fno-strict-overflow", "-fno-strict-aliasing", "-fPIE", "-pie"]
    p = subprocess.run([cc] + base + [a.c, "-o", exe, "-lm", "-lpthread"], capture_output=True, text=True)
    if p.returncode != 0:
        print("chaos_linux: the chaos program did not build:\n" + (p.stdout + p.stderr)[-1500:], file=sys.stderr)
        return 1
    p = subprocess.run([cc, "-shared", "-fPIC", "-O2", os.path.join(HERE, "clockjump.c"), "-o", shim],
                       capture_output=True, text=True)
    if p.returncode != 0:
        print("chaos_linux: clockjump.so did not build:\n" + (p.stdout + p.stderr)[-1500:], file=sys.stderr)
        return 1
    print(exe)
    print(shim)
    return 0


# --- clock jumps -------------------------------------------------------------

def silent_server():
    s = socket.socket()
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(("127.0.0.1", 0))
    s.listen(16)
    held = []

    def loop():
        while True:
            try:
                c, _ = s.accept()
            except OSError:
                return
            held.append(c)   # never answers, never closes

    threading.Thread(target=loop, daemon=True).start()
    return s, s.getsockname()[1]


def shim_works(shim):
    """The library moves `date`'s clock (a dynamic glibc program)."""
    date = shutil.which("date")
    if not date:
        return True
    env = dict(os.environ, LD_PRELOAD=shim, CLOCKJUMP_AT_MS="0", CLOCKJUMP_BY_MS=str(HOUR_MS))
    try:
        moved = int(subprocess.run([date, "+%s"], env=env, capture_output=True, text=True).stdout.strip())
        now = int(subprocess.run([date, "+%s"], capture_output=True, text=True).stdout.strip())
    except ValueError:
        return False
    return 3500 <= moved - now <= 3700


def clock_env(a, clock, by, stamp):
    env = dict(os.environ)
    if by == 0:
        return env
    if a.faketime and clock == "realtime":
        with open(stamp, "w") as f:
            f.write("+0\n")
        env.update(LD_PRELOAD=a.faketime, FAKETIME_TIMESTAMP_FILE=stamp, FAKETIME_NO_CACHE="1",
                   DONT_FAKE_MONOTONIC="1")
        return env
    env.update(LD_PRELOAD=a.shim, CLOCKJUMP_AT_MS=str(JUMP_AT_MS), CLOCKJUMP_BY_MS=str(by),
               CLOCKJUMP_CLOCK=clock)
    return env


def cmd_clock(a):
    if not a.faketime and not shim_works(a.shim):
        result("clock jump library works", False, f"{a.shim} does not move date's clock", 0.0)
        return 1
    srv, port = silent_server()
    allok = True
    stamp = os.path.join(os.path.dirname(os.path.abspath(a.exe)), "faketime.stamp")
    cases = [c for c in CLOCK_CASES if not a.cases or c[0] in a.cases.split(",")]
    for label, clock, by in JUMPS:
        for case, nominal, want in cases:
            env = clock_env(a, clock, by, stamp)
            if a.faketime and clock == "realtime" and by != 0:
                offset = f"{'+' if by > 0 else '-'}{abs(by) // 1000}\n"

                def jump(path=stamp, text=offset):
                    time.sleep(JUMP_AT_MS / 1000.0)
                    with open(path, "w") as f:
                        f.write(text)

                threading.Thread(target=jump, daemon=True).start()
            elif a.faketime and by != 0:
                continue   # libfaketime here: realtime jumps only
            limit = nominal + 8.0
            rc, out, err, secs = run_timed([a.exe, "clock", case, str(port)], env, limit)
            got = out.strip()
            name = f"clock {case} ({label})"
            if rc == "timeout":
                allok &= result(name, False, f"hung (killed at {limit:.0f} s)", secs)
                continue
            if rc != 0 or got != want:
                allok &= result(name, False, f"exit {rc}, printed {got!r} (want {want!r}) {err.strip()[:80]!r}", secs)
                continue
            if secs > nominal + 1.5:
                allok &= result(name, False, f"stretched: {secs:.2f} s for a {nominal} s wait", secs)
                continue
            if clock == "realtime" and secs < nominal * 0.85:
                allok &= result(name, False, f"cut short: {secs:.2f} s for a {nominal} s wait", secs)
                continue
            allok &= result(name, True, f"{got.splitlines()[-1]!r} in {secs:.2f} s", secs)
    srv.close()
    return 0 if allok else 1


# --- network partitions ------------------------------------------------------

def sh(cmd, check=True):
    p = subprocess.run(cmd, capture_output=True, text=True)
    if check and p.returncode != 0:
        raise RuntimeError(f"{' '.join(cmd)}: {(p.stdout + p.stderr).strip()[:300]}")
    return p.returncode == 0


class Net:
    """Two namespaces joined by veth (or one, over loopback); partitions."""

    def __init__(self):
        self.peer = None
        self.how = "nft"
        self.cut = False
        sh(["ip", "link", "set", "lo", "up"])
        try:
            self.peer = subprocess.Popen(["unshare", "-n", "sleep", "100000"])
            mine = os.readlink("/proc/self/ns/net")
            for _ in range(100):
                try:
                    if os.readlink(f"/proc/{self.peer.pid}/ns/net") != mine:
                        break
                except OSError:
                    pass
                time.sleep(0.02)
            sh(["ip", "link", "add", "fc0", "type", "veth", "peer", "name", "fc1", "netns", str(self.peer.pid)])
            sh(["ip", "addr", "add", "10.213.0.1/24", "dev", "fc0"])
            sh(["ip", "link", "set", "fc0", "up"])
            self.ns(["ip", "link", "set", "lo", "up"])
            self.ns(["ip", "addr", "add", "10.213.0.2/24", "dev", "fc1"])
            self.ns(["ip", "link", "set", "fc1", "up"])
            self.host = "10.213.0.2"
            self.topology = "two namespaces, veth"
        except (RuntimeError, OSError) as e:
            if self.peer:
                self.peer.kill()
            self.peer = None
            self.host = "127.0.0.1"
            self.topology = f"one namespace, loopback ({e})"
        if not shutil.which("nft") or not self._nft_ok():
            self.how = "netem"

    def ns(self, cmd, check=True):
        if self.peer is None:
            return sh(cmd, check)
        return sh(["nsenter", "-t", str(self.peer.pid), "-n"] + cmd, check)

    def _nft_ok(self):
        ok = sh(["nft", "add", "table", "inet", "fidchaos"], False)
        if ok:
            sh(["nft", "delete", "table", "inet", "fidchaos"], False)
        return ok

    def popen_peer(self, cmd, **kw):
        if self.peer is None:
            return subprocess.Popen(cmd, **kw)
        return subprocess.Popen(["nsenter", "-t", str(self.peer.pid), "-n"] + cmd, **kw)

    def _nft(self, run, rules):
        run(["nft", "add", "table", "inet", "fidchaos"])
        run(["nft", "add", "chain", "inet", "fidchaos", "input",
             "{ type filter hook input priority 0 ; policy accept ; }"])
        for r in rules:
            run(["nft", "add", "rule", "inet", "fidchaos", "input"] + r + ["drop"])

    def partition(self):
        if self.cut:
            return
        self.cut = True
        if self.how == "nft":
            if self.peer:
                self._nft(sh, [["iifname", "fc0"]])
                self._nft(self.ns, [["iifname", "fc1"]])
            else:
                self._nft(sh, [["tcp", "dport", str(SERVER_PORT)], ["tcp", "sport", str(SERVER_PORT)]])
        elif self.peer:
            sh(["tc", "qdisc", "add", "dev", "fc0", "root", "netem", "loss", "100%"])
            self.ns(["tc", "qdisc", "add", "dev", "fc1", "root", "netem", "loss", "100%"])
        else:
            sh(["tc", "qdisc", "add", "dev", "lo", "root", "netem", "loss", "100%"])

    def heal(self):
        if not self.cut:
            return
        self.cut = False
        if self.how == "nft":
            sh(["nft", "delete", "table", "inet", "fidchaos"], False)
            if self.peer:
                self.ns(["nft", "delete", "table", "inet", "fidchaos"], False)
        else:
            sh(["tc", "qdisc", "del", "dev", "fc0" if self.peer else "lo", "root"], False)
            if self.peer:
                self.ns(["tc", "qdisc", "del", "dev", "fc1", "root"], False)

    def close(self):
        self.heal()
        if self.peer:
            self.peer.kill()
            self.peer.wait()


def response_head():
    return f"HTTP/1.1 200 OK\r\nContent-Length: {BODY}\r\nConnection: close\r\n\r\n".encode()


def cmd_serve(a):
    """The server (in the peer namespace): the head and one chunk, then
    `sent-part` on stdout, then a chunk every 50 ms; every connection on a
    thread of its own."""
    s = socket.socket()
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(("0.0.0.0", a.port))
    s.listen(16)
    print("listening", flush=True)
    lock = threading.Lock()

    def say(t):
        with lock:
            print(t, flush=True)

    def one(c):
        try:
            c.settimeout(5)
            got = b""
            while b"\r\n\r\n" not in got and len(got) < 65536:
                d = c.recv(4096)
                if not d:
                    break
                got += d
            c.settimeout(None)
            c.sendall(response_head() + b"x" * CHUNK)
            say("sent-part")
            sent = CHUNK
            while sent < BODY:
                time.sleep(0.05)
                n = min(CHUNK, BODY - sent)
                c.sendall(b"x" * n)
                sent += n
            say("sent-all")
        except OSError as e:
            say(f"error {e}")
        finally:
            try:
                c.close()
            except OSError:
                pass

    while True:
        try:
            c, _ = s.accept()
        except OSError:
            return 0
        threading.Thread(target=one, args=(c,), daemon=True).start()


class Lines:
    """The server's stdout, one reader thread; wait() for a line."""

    def __init__(self, stream):
        self.q = queue.Queue()

        def rd():
            for line in stream:
                self.q.put(line.strip())

        threading.Thread(target=rd, daemon=True).start()

    def drain(self):
        while True:
            try:
                self.q.get_nowait()
            except queue.Empty:
                return

    def wait(self, want, limit):
        end = time.monotonic() + limit
        while True:
            left = end - time.monotonic()
            if left <= 0:
                return False
            try:
                if self.q.get(timeout=left) == want:
                    return True
            except queue.Empty:
                return False


def cmd_partition(a):
    if os.geteuid() != 0 and not os.environ.get("FID_CHAOS_INNER"):
        env = dict(os.environ, FID_CHAOS_INNER="1")
        args = ["unshare", "-rn", sys.executable, os.path.abspath(__file__)] + sys.argv[1:]
        return subprocess.run(args, env=env).returncode
    net = Net()
    print(f"partition: {net.topology}; partitions by {net.how}", file=sys.stderr, flush=True)
    srv = net.popen_peer([sys.executable, os.path.abspath(__file__), "serve", "--port", str(SERVER_PORT)],
                         stdout=subprocess.PIPE, text=True)
    lines = Lines(srv.stdout)
    allok = True
    full = len(response_head()) + BODY

    def client(ms):
        lines.drain()
        return subprocess.Popen([a.exe, "fetch", net.host, str(SERVER_PORT), "/", ms],
                                stdout=subprocess.PIPE, stderr=subprocess.PIPE), time.monotonic()

    def finish(started, limit):
        p, t0 = started
        try:
            out, err = p.communicate(timeout=max(0.1, limit - (time.monotonic() - t0)))
            return (p.returncode, out.decode("utf-8", "replace").replace(chr(13), "").strip(),
                    err.decode("utf-8", "replace"), time.monotonic() - t0)
        except subprocess.TimeoutExpired:
            p.kill()
            p.communicate()
            return "timeout", "", "", time.monotonic() - t0

    def judge(name, res, test, limit):
        rc, out, err, secs = res
        if rc == "timeout":
            return result(name, False, f"hung (killed at {limit:.0f} s)", secs)
        ok, why = test(rc, out, secs)
        return result(name, ok, why + ("" if ok else f" (exit {rc}) {err.strip()[:80]!r}"), secs)

    def timed_out_part(rc, out, secs):
        ok = rc == 0 and out.startswith("fetch timed out body ") and int(out.split()[-1]) < full
        return ok, repr(out)

    try:
        if not lines.wait("listening", 10):
            result("partition: the server starts", False, "it did not start", 0.0)
            return 1

        # 1. no partition: the whole answer
        res = finish(client("3000"), 15)
        allok &= judge("partition: none (whole answer)", res, lambda rc, out, secs: (
            rc == 0 and out == f"fetch closed body {full}", repr(out)), 15)

        # 2. cut mid-answer; each read waits at most 1.5 s: it gives up
        c = client("1500")
        if lines.wait("sent-part", 5):
            net.partition()
        res = finish(c, 12)
        net.heal()
        allok &= judge("partition mid-answer, read timeout", res, timed_out_part, 12)
        time.sleep(0.5)

        # 3. cut mid-answer, no limit (stdlib/http_client.fd's transport)
        c = client("none")
        if lines.wait("sent-part", 5):
            net.partition()
        res = finish(c, 12)
        net.heal()
        allok &= judge("partition mid-answer, no timeout", res, lambda rc, out, secs: (
            rc == 0 and out.startswith("fetch "), repr(out)), 12)
        time.sleep(0.5)

        # 4. cut for 2.5 s mid-answer, then healed: TCP resends, all of it
        #    arrives
        c = client("8000")
        if lines.wait("sent-part", 5):
            net.partition()
            time.sleep(2.5)
            net.heal()
        res = finish(c, 30)
        allok &= judge("partition 2.5 s, then healed", res, lambda rc, out, secs: (
            rc == 0 and out == f"fetch closed body {full}", repr(out)), 30)

        # 5. connect into a partition inside a 1.5 s timeout block
        net.partition()
        res = finish(client("1500"), 12)
        allok &= judge("connect into a partition, timeout", res, lambda rc, out, secs: (
            rc == 0 and out == "connect timed out" and secs < 6, repr(out)), 12)

        # 6. connect into a partition, blocking (p_net_connect: no limit)
        res = finish(client("none"), 12)
        allok &= judge("connect into a partition, blocking", res, lambda rc, out, secs: (
            rc == 0 and out in ("connect failed", "connect timed out"), repr(out)), 12)
        net.heal()
    finally:
        srv.kill()
        net.close()
    return 0 if allok else 1


def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    b = sub.add_parser("build")
    b.add_argument("--c", required=True, help="fid dump-c tests/verify/chaos.fd")
    b.add_argument("--out", default="/tmp/fides-chaos")
    b.add_argument("--cc", default="")
    c = sub.add_parser("clock")
    c.add_argument("--exe", required=True)
    c.add_argument("--shim", default="")
    c.add_argument("--faketime", default="", help="libfaketime.so.1, for the realtime jumps")
    c.add_argument("--cases", default="", help="comma-separated subset of the cases")
    p = sub.add_parser("partition")
    p.add_argument("--exe", required=True)
    s = sub.add_parser("serve")
    s.add_argument("--port", type=int, default=SERVER_PORT)
    a = ap.parse_args()
    if a.cmd == "build":
        return cmd_build(a)
    if a.cmd == "clock":
        if not a.shim and not a.faketime:
            ap.error("clock needs --shim or --faketime")
        return cmd_clock(a)
    if a.cmd == "partition":
        return cmd_partition(a)
    signal.signal(signal.SIGTERM, lambda *_: sys.exit(0))
    return cmd_serve(a)


if __name__ == "__main__":
    sys.exit(main())
