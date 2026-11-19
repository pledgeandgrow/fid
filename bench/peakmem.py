#!/usr/bin/env python3
"""Run a program once; print its peak memory and wall time (roadmap BM7).

    python bench/peakmem.py <exe> [args ...]
    -> "<peak resident KB> <wall ms> <last line of output>"

Peak memory is the most the process had resident at once:
  Linux    ru_maxrss of the child (os.wait4), in KB
  macOS    ru_maxrss of the child, in bytes (converted to KB)
  Windows  PeakWorkingSetSize (GetProcessMemoryInfo on the exited process,
           whose handle Popen keeps open), in bytes (converted to KB)
Standard output is read and its last line printed, so bench/size.sh can
check every language printed the same thing.
"""
import os
import subprocess
import sys
import time


def peak_kb_windows(handle):
    import ctypes
    from ctypes import wintypes

    class PMC(ctypes.Structure):
        _fields_ = [("cb", wintypes.DWORD), ("PageFaultCount", wintypes.DWORD),
                    ("PeakWorkingSetSize", ctypes.c_size_t), ("WorkingSetSize", ctypes.c_size_t),
                    ("QuotaPeakPagedPoolUsage", ctypes.c_size_t), ("QuotaPagedPoolUsage", ctypes.c_size_t),
                    ("QuotaPeakNonPagedPoolUsage", ctypes.c_size_t), ("QuotaNonPagedPoolUsage", ctypes.c_size_t),
                    ("PagefileUsage", ctypes.c_size_t), ("PeakPagefileUsage", ctypes.c_size_t)]
    pmc = PMC()
    pmc.cb = ctypes.sizeof(PMC)
    psapi = ctypes.WinDLL("psapi")
    psapi.GetProcessMemoryInfo.argtypes = [wintypes.HANDLE, ctypes.POINTER(PMC), wintypes.DWORD]
    if not psapi.GetProcessMemoryInfo(int(handle), ctypes.byref(pmc), pmc.cb):
        return -1
    return pmc.PeakWorkingSetSize // 1024


def main():
    if len(sys.argv) < 2:
        sys.exit("usage: peakmem.py <exe> [args ...]")
    cmd = sys.argv[1:]
    # Windows' CreateProcess does not find a relative path with a directory
    # in it ("out/size-hello-rust.exe", as bench/size.sh passes it): give it
    # the absolute path
    if os.path.isfile(cmd[0]):
        cmd[0] = os.path.abspath(cmd[0])
    t0 = time.perf_counter()
    if os.name == "nt":
        p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        out, _ = p.communicate()
        ms = (time.perf_counter() - t0) * 1000
        kb = peak_kb_windows(p._handle)
    else:
        p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        out = p.stdout.read()
        _, _, ru = os.wait4(p.pid, 0)
        ms = (time.perf_counter() - t0) * 1000
        p.returncode = 0  # reaped by wait4: keep Popen from waiting again
        kb = ru.ru_maxrss // 1024 if sys.platform == "darwin" else ru.ru_maxrss
    lines = out.decode("utf-8", "replace").replace("\r", "").strip().split("\n")
    print(f"{kb} {ms:.1f} {lines[-1] if lines else ''}")


if __name__ == "__main__":
    main()
