#!/usr/bin/env python3
"""Chaos tests: the program must fail cleanly when the world does (VF17).

    python scripts/verify/chaos.py --fid release/fid.exe [--backends ccompile,emit]
                                   [--linux] [--out build/chaos]

Builds tests/verify/chaos.fd and runs each scenario against it. A scenario
passes when the program reports the failure (an error value, or a panic
message with a nonzero exit) and ends within its time limit; it fails on a
crash (access violation, SIGSEGV), a hang, or output that pretends nothing
went wrong.

On this host (Windows or Linux):
  dropped connection   a server that accepts and closes at once
  reset connection     a server that accepts and resets (SO_LINGER 0)
  half a response      a server that sends part of the headers and closes
  silent server        a server that accepts and never answers (the client
                       must give up, not wait forever)
  reader goes away     stdout closed after one line while it prints a million
  missing file         reading a file that does not exist; a directory
  stack exhaustion     recursion a million deep: "Stack overflow", not a
                       silent crash
With --linux (WSL on Windows, or natively on Linux), built for
x86_64-linux through C:
  full disk            appends under `ulimit -f` (SIGXFSZ ignored, so the
                       write fails with EFBIG as on a full disk)
  out of memory        allocation under `ulimit -v`

Not covered yet: clock jumps (needs libfaketime or a VM), a disk that fills
part-way through a write on Windows, network partitions between machines.
"""
import argparse
import os
import shutil
import socket
import struct
import subprocess
import sys
import threading
import time

CRASH = ("Access violation", "=== CRASH", "Segmentation fault")


def server(kind):
    s = socket.socket()
    s.bind(("127.0.0.1", 0))
    s.listen(8)
    port = s.getsockname()[1]
    held = []

    def loop():
        while True:
            try:
                c, _ = s.accept()
            except OSError:
                return
            if kind == "close":
                c.close()
            elif kind == "reset":
                c.setsockopt(socket.SOL_SOCKET, socket.SO_LINGER, struct.pack("ii", 1, 0))
                c.close()
            elif kind == "half":
                try:
                    c.recv(4096)
                    c.sendall(b"HTTP/1.1 200 OK\r\nContent-Len")
                except OSError:
                    pass
                c.close()
            elif kind == "silent":
                held.append(c)   # never answer, never close

    threading.Thread(target=loop, daemon=True).start()
    return s, port


def run(cmd, timeout, shell=False):
    t = time.time()
    try:
        p = subprocess.run(cmd, capture_output=True, timeout=timeout, shell=shell)
        return p.returncode, p.stdout.decode("utf-8", "replace"), p.stderr.decode("utf-8", "replace"), time.time() - t
    except subprocess.TimeoutExpired:
        return "timeout", "", "", time.time() - t


def judge(name, res, expect):
    rc, out, err, dt = res
    crashed = any(m in out + err for m in CRASH) or (isinstance(rc, int) and (rc in (139, 134) or rc > 255 or rc < 0)
                                                    and "Stack overflow" not in err and "OOM" not in err
                                                    and "out of memory" not in err.lower())
    ok, why = expect(rc, out, err)
    if rc == "timeout":
        ok, why = False, "hung (killed at the time limit)"
    elif crashed and not ok:
        why = f"crashed (exit {rc if isinstance(rc, str) else hex(rc & 0xFFFFFFFF)})"
    line = f"{'PASS' if ok else 'FAIL'}  {name:28} {dt:5.1f} s  {why}"
    print(line, flush=True)
    return ok, line


def host_scenarios(exe, work):
    results = []
    for kind, label in [("close", "dropped connection"), ("reset", "reset connection"),
                        ("half", "half a response"), ("silent", "silent server")]:
        s, port = server(kind)
        res = run([exe, "http", str(port), "/"], 60)
        s.close()
        results.append(judge(label, res, lambda rc, out, err: (
            rc == 0 and "status 0" in out, "status 0 (transport failure) reported"
            if "status 0" in out else f"exit {rc}, {out.strip()[:60]!r} {err.strip()[:80]!r}")))
    # reader goes away
    t = time.time()
    p = subprocess.Popen([exe, "print", "1000000"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    p.stdout.readline()
    p.stdout.close()
    try:
        p.wait(30)
        rc = p.returncode
        err = p.stderr.read().decode("utf-8", "replace")
    except subprocess.TimeoutExpired:
        p.kill()
        rc, err = "timeout", ""
    results.append(judge("reader goes away", (rc, "", err, time.time() - t), lambda rc, out, err: (
        rc != "timeout" and not any(m in err for m in CRASH), f"ended (exit {rc})")))
    missing = os.path.join(work, "no-such-file.txt")
    results.append(judge("missing file", run([exe, "read", missing], 30), lambda rc, out, err: (
        rc == 0 and out.strip() == "none", f"{out.strip()!r}")))
    results.append(judge("directory as a file", run([exe, "read", work], 30), lambda rc, out, err: (
        rc == 0 and out.strip() == "none", f"{out.strip()!r} exit {rc}")))
    results.append(judge("stack exhaustion", run([exe, "deep", "100000000"], 120), lambda rc, out, err: (
        rc != 0 and ("Stack overflow" in err or "stack overflow" in err.lower() or "Recursion" in err),
        f"exit {rc if isinstance(rc, str) else hex(rc & 0xFFFFFFFF)}: {err.strip().splitlines()[0][:70] if err.strip() else '(no message)'}")))
    return results


def wsl_prefix():
    if os.name != "nt":
        return []
    return ["wsl", "-d", os.environ.get("WSL_DISTRO", "kali-linux"), "-e"]


def to_wsl(p):
    if os.name != "nt":
        return p
    p = os.path.abspath(p).replace("\\", "/")
    return "/mnt/" + p[0].lower() + p[2:]


def linux_scenarios(lexe, work):
    pre = wsl_prefix()
    results = []
    lx = to_wsl(lexe)
    tmp = "/tmp/fides-chaos-full.bin"
    cmd = pre + ["sh", "-c", f"rm -f {tmp}; trap '' XFSZ; ulimit -f 2048; '{lx}' write {tmp} 200; s=$?; rm -f {tmp}; exit $s"]
    results.append(judge("full disk (ulimit -f)", run(cmd, 120), lambda rc, out, err: (
        rc == 0 and "write-failed" in out, f"{out.strip().splitlines()[-1][:60] if out.strip() else ''!r} exit {rc} {err.strip()[:80]!r}")))
    cmd = pre + ["sh", "-c", f"ulimit -v 400000; '{lx}' alloc"]
    results.append(judge("out of memory (ulimit -v)", run(cmd, 180), lambda rc, out, err: (
        isinstance(rc, int) and rc != 0 and rc not in (139, 134) and ("OOM" in err or "memory" in err.lower()),
        f"exit {rc}: {err.strip().splitlines()[0][:80] if err.strip() else '(no message)'}")))
    return results


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--fid", default=os.path.join("release", "fid.exe"))
    ap.add_argument("--backends", default="ccompile,emit")
    ap.add_argument("--linux", action="store_true")
    ap.add_argument("--out", default=os.path.join("build", "chaos"))
    a = ap.parse_args()
    os.environ.setdefault("CC", "zig cc")
    os.makedirs(a.out, exist_ok=True)
    work = os.path.abspath(a.out)
    src = os.path.join("tests", "verify", "chaos.fd")
    lines = []
    allok = True
    for b in a.backends.split(","):
        exe = os.path.join(work, f"chaos-{b}.exe")
        p = subprocess.run([a.fid, b, src, "-o", exe], capture_output=True, text=True, errors="replace")
        if p.returncode != 0 or not os.path.exists(exe):
            print(f"{b}: build failed: {(p.stdout + p.stderr)[-800:]}")
            allok = False
            continue
        print(f"== {b} (host)")
        for ok, line in host_scenarios(exe, work):
            allok &= ok
            lines.append(f"{b} {line}")
    if a.linux:
        lexe = os.path.join(work, "chaos-linux")
        p = subprocess.run([a.fid, "ccompile", src, "--target", "x86_64-linux", "-o", lexe],
                           capture_output=True, text=True, errors="replace")
        if p.returncode != 0 or not os.path.exists(lexe):
            print(f"linux build failed: {(p.stdout + p.stderr)[-800:]}")
            allok = False
        else:
            print("== ccompile --target x86_64-linux")
            for ok, line in linux_scenarios(lexe, work):
                allok &= ok
                lines.append(f"linux {line}")
    with open(os.path.join(work, "report.txt"), "w") as f:
        f.write("\n".join(lines) + "\n")
    sys.exit(0 if allok else 1)


if __name__ == "__main__":
    main()
