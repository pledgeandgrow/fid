#!/usr/bin/env python3
"""Chaos tests: the program must fail cleanly when the world does (VF17).

    python scripts/verify/chaos.py --fid release/fid.exe [--backends ccompile,emit]
                                   [--linux [--linux-nat] | --clock | --partition] [--strict]
                                   [--faketime LIB] [--out build/chaos]

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
  clock jumps          (also --clock) the C of chaos.fd built by Linux's gcc
                       against glibc, run under scripts/verify/clockjump.c
                       (LD_PRELOAD; or libfaketime with --faketime LIB):
                       sleeps, task deadlines, timeout blocks, tokens and
                       socket waits with the wall clock stepped an hour
                       forward or back 250 ms into the wait, and with every
                       clock an hour forward (a paused VM)
  partitions           (also --partition) two network namespaces joined by
                       veth (`unshare -rn`, no root), a server in one, the
                       program in the other; nftables drops every packet in
                       both: cut mid-answer (with and without a read
                       timeout), cut and healed, connecting into a cut
These two run scripts/verify/chaos_linux.py inside WSL (or directly on
Linux); see its docstring.

A failure that is an open finding of docs/verification/miscompile-log.md
(KNOWN below) prints OPEN <id> and fails the run only with --strict; a pass
of one prints that the finding looks fixed.

Not covered yet: a disk that fills part-way through a write on Windows,
clock jumps on Windows and in statically linked programs (fid's musl
cross builds, the machine layer's images), partitions between real machines.
"""
import argparse
import json
import os
import shutil
import socket
import struct
import subprocess
import sys
import threading
import time

CRASH = ("Access violation", "=== CRASH", "Segmentation fault")

# Open findings (docs/verification/miscompile-log.md, "Runtime under failure")
# by scenario name
KNOWN = {
    "silent server": "C1",
    "partition mid-answer, no timeout": "C1",
    "linux full disk (ulimit -f)": "C4",          # the C path (ccompile)
    "linux out of memory (ulimit -v)": "C5",
    "connect into a partition, blocking": "C7",
    "emit argument past the last": "C8",      # "<backend> <name>": that backend only
}
for _case in ("timeout", "token", "cosleep"):
    for _jump in ("realtime +1h", "realtime -1h"):
        KNOWN[f"clock {_case} ({_jump})"] = "C6"
STRICT = False
BACKEND = ""   # the host backend being run, for KNOWN's "<backend> <name>" keys


def report(name, ok, why, dt):
    """Print a scenario's line; True when it does not fail the run."""
    known = KNOWN.get(f"{BACKEND} {name}") or KNOWN.get(name)
    tag = "PASS" if ok else (f"OPEN {known}" if known else "FAIL")
    if ok and known:
        why += f" ({known} looks fixed: update docs/verification/miscompile-log.md)"
    line = f"{tag:8}  {name:44} {dt:5.1f} s  {why}"
    print(line, flush=True)
    return ok or (known is not None and not STRICT), line


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
    return report(name, ok, why, dt)


def host_scenarios(exe, work):
    results = []
    for kind, label in [("close", "dropped connection"), ("reset", "reset connection"),
                        ("half", "half a response"), ("silent", "silent server")]:
        s, port = server(kind)
        res = run([exe, "http", str(port), "/"], 60)
        s.close()
        if kind == "half":
            # the driver does not parse a status: it must end and say what it got
            results.append(judge(label, res, lambda rc, out, err: (
                rc == 0 and "body" in out, f"ended: {out.strip()[:60]!r}")))
            continue
        results.append(judge(label, res, lambda rc, out, err: (
            rc == 0 and "status 0" in out, "status 0 (transport failure) reported"
            if "status 0" in out else f"exit {rc}, {out.strip()[:60]!r} {err.strip()[:80]!r}")))
    # reader goes away
    t = time.time()
    # every mode gets three arguments ("-" fills in): chaos.fd reads arg_at(3)
    # first, and with fewer arguments an emit build panics there (C8, its own
    # scenario below), before it does what the scenario names
    p = subprocess.Popen([exe, "print", "1000000", "-"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
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
    results.append(judge("missing file", run([exe, "read", missing, "-"], 30), lambda rc, out, err: (
        rc == 0 and out.strip() == "none", f"{out.strip()!r}")))
    results.append(judge("directory as a file", run([exe, "read", work, "-"], 30), lambda rc, out, err: (
        rc == 0 and out.strip() == "none", f"{out.strip()!r} exit {rc}")))
    results.append(judge("argument past the last", run([exe, "print", "2"], 30), lambda rc, out, err: (
        rc == 0 and out.replace("\r", "") == "line 0\nline 1\n",
        f"{out.strip()!r} exit {rc} {err.strip()[:70]!r}")))
    results.append(judge("stack exhaustion", run([exe, "deep", "100000000", "-"], 120), lambda rc, out, err: (
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
    # 2048 blocks of 512 bytes is 1 MiB; 200 chunks of 64 KiB do not fit
    cmd = pre + ["sh", "-c", f"rm -f {tmp}; trap '' XFSZ; ulimit -f 2048; '{lx}' write {tmp} 200; s=$?; rm -f {tmp}; exit $s"]
    results.append(judge("full disk (ulimit -f)", run(cmd, 120), lambda rc, out, err: (
        rc == 0 and "write-failed" in out, f"{out.strip().splitlines()[-1][:60] if out.strip() else ''!r} exit {rc} {err.strip()[:80]!r}")))
    cmd = pre + ["sh", "-c", f"ulimit -v 400000; '{lx}' alloc - -"]

    def oom_panic(err):
        # a panic that names the cause, not the SIGABRT of an abort() (C5;
        # the stage 0 of Nov 16 prints "Panic: signal 6 (SIGABRT)")
        return any(ln.startswith("Panic:") and ("memory" in ln.lower() or "OOM" in ln)
                   for ln in err.splitlines())

    results.append(judge("out of memory (ulimit -v)", run(cmd, 180), lambda rc, out, err: (
        rc == 1 and oom_panic(err),
        f"exit {rc}: {' / '.join(err.strip().splitlines()[:2])[:110] if err.strip() else '(no message)'}")))
    return results


def linux_helper(a, work, src, parts):
    """Clock jumps and partitions: scripts/verify/chaos_linux.py (in WSL on
    Windows) on the C of chaos.fd, built there by gcc against glibc."""
    results = []
    cfile = os.path.join(work, "chaos-linux.c")
    with open(cfile, "wb") as f:
        p = subprocess.run([a.fid, "dump-c", src], stdout=f, stderr=subprocess.PIPE)
    if p.returncode != 0 or os.path.getsize(cfile) == 0:
        print(f"dump-c failed: {p.stderr.decode('utf-8', 'replace')[-800:]}")
        return [(False, "linux dump-c failed")]
    pre = wsl_prefix()
    helper = to_wsl(os.path.join("scripts", "verify", "chaos_linux.py"))
    lout = a.linux_out
    exe = lout + "/chaos-glibc"
    p = subprocess.run(pre + ["python3", helper, "build", "--c", to_wsl(cfile), "--out", lout],
                       capture_output=True, text=True, errors="replace")
    if p.returncode != 0:
        print(f"linux build (gcc) failed: {(p.stdout + p.stderr)[-1200:]}")
        return [(False, "linux build failed")]
    for part in parts:
        cmd = pre + ["python3", helper, part, "--exe", exe]
        if part == "clock":
            cmd += ["--shim", lout + "/clockjump.so"]
            if a.faketime:
                cmd += ["--faketime", a.faketime]
        print(f"== {part} (gcc, glibc, {'WSL' if pre else 'Linux'})", flush=True)
        try:
            p = subprocess.run(cmd, capture_output=True, text=True, errors="replace", timeout=1800)
            out, err = p.stdout, p.stderr
        except subprocess.TimeoutExpired:
            out, err = "", "chaos_linux.py ran for more than 30 minutes"
        seen = 0
        for ln in out.splitlines():
            if ln.startswith("RESULT "):
                r = json.loads(ln[len("RESULT "):])
                results.append(report(r["name"], r["ok"], r["why"], r["secs"]))
                seen += 1
        for ln in err.splitlines():
            if ln.startswith("partition:"):
                print("   " + ln)
        if seen == 0:
            print(f"{part}: no results: {err[-1200:]}")
            results.append((False, f"{part}: no results"))
    return results


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--fid", default=os.path.join("release", "fid.exe"))
    ap.add_argument("--backends", default="ccompile,emit")
    ap.add_argument("--linux", action="store_true",
                    help="also the Linux scenarios: full disk, out of memory, clock jumps, partitions")
    ap.add_argument("--linux-nat", action="store_true",
                    help="with --linux: full disk and out of memory also on emit --target x86_64-linux")
    ap.add_argument("--clock", action="store_true", help="the clock jumps (Linux or WSL)")
    ap.add_argument("--partition", action="store_true", help="the network partitions (Linux or WSL)")
    ap.add_argument("--faketime", default="", help="libfaketime.so.1 (a Linux path) for the realtime jumps")
    ap.add_argument("--linux-out", default="/tmp/fides-chaos", help="Linux-side build folder")
    ap.add_argument("--strict", action="store_true", help="open findings (KNOWN) fail the run too")
    ap.add_argument("--out", default=os.path.join("build", "chaos"))
    ap.add_argument("--src", default=os.path.join("tests", "verify", "chaos.fd"),
                    help="the driver (a copy of chaos.fd, to try an older compiler)")
    a = ap.parse_args()
    global STRICT, BACKEND
    STRICT = a.strict
    if not a.linux_out.startswith("/"):
        # Git Bash turns /tmp/x into C:/.../Temp/x: WSL would make that path
        # inside the current folder
        ap.error(f"--linux-out must be a Linux path, not {a.linux_out!r} (Git Bash: MSYS_NO_PATHCONV=1)")
    only_linux = (a.clock or a.partition) and not a.linux
    if only_linux:
        a.backends = ""
    os.environ.setdefault("CC", "zig cc")
    os.makedirs(a.out, exist_ok=True)
    work = os.path.abspath(a.out)
    src = a.src
    lines = []
    allok = True
    for b in [x for x in a.backends.split(",") if x]:
        exe = os.path.join(work, f"chaos-{b}.exe")
        p = subprocess.run([a.fid, b, src, "-o", exe], capture_output=True, text=True, errors="replace")
        if p.returncode != 0 or not os.path.exists(exe):
            print(f"{b}: build failed: {(p.stdout + p.stderr)[-800:]}")
            allok = False
            continue
        print(f"== {b} (host)")
        BACKEND = b
        for ok, line in host_scenarios(exe, work):
            allok &= ok
            lines.append(f"{b} {line}")
    BACKEND = "linux"
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
        if a.linux_nat:
            # the machine layer's image and stdlib/linux_runtime (no libc)
            BACKEND = "linux-emit"
            nexe = os.path.join(work, "chaos-linux-nat")
            p = subprocess.run([a.fid, "emit", src, "--target", "x86_64-linux", "-o", nexe],
                               capture_output=True, text=True, errors="replace")
            if p.returncode != 0 or not os.path.exists(nexe):
                print(f"emit --target x86_64-linux failed: {(p.stdout + p.stderr)[-800:]}")
                allok = False
            else:
                print("== emit --target x86_64-linux")
                for ok, line in linux_scenarios(nexe, work):
                    allok &= ok
                    lines.append(f"linux-emit {line}")
            BACKEND = "linux"
    parts = [p for p, on in (("clock", a.clock or a.linux), ("partition", a.partition or a.linux)) if on]
    if parts:
        for ok, line in linux_helper(a, work, src, parts):
            allok &= ok
            lines.append(f"linux {line}")
    with open(os.path.join(work, "report.txt"), "w") as f:
        f.write("\n".join(lines) + "\n")
    sys.exit(0 if allok else 1)


if __name__ == "__main__":
    main()
