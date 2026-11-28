#!/usr/bin/env python3
"""BG207: run a Windows program in a console of its own, type lines into
that console, and print the text the console then shows.

    python tests/nattools/console_feed.py <exe> [line ...]

Each argument after the program is typed as one line (its characters, then
Enter); `^Z` is Ctrl+Z (end of input on a console). The program's standard
input and output are that console (nothing redirected: a redirected stdin
is a file or a pipe, not what BG207 is about). The screen's lines are
printed without their trailing blanks, then "rc <exit status>" (or
"TIMEOUT" when the program has not ended 5 s after the last line).
Windows only (kernel32: AttachConsole, WriteConsoleInputW,
ReadConsoleOutputCharacterW).
"""
import ctypes
import ctypes.wintypes as W
import subprocess
import sys
import time

CREATE_NEW_CONSOLE = 0x10


class KEY_EVENT(ctypes.Structure):
    _fields_ = [("bKeyDown", W.BOOL), ("wRepeatCount", W.WORD), ("wVirtualKeyCode", W.WORD),
                ("wVirtualScanCode", W.WORD), ("uChar", W.WCHAR), ("dwControlKeyState", W.DWORD)]


class INPUT_RECORD(ctypes.Structure):
    _fields_ = [("EventType", W.WORD), ("_pad", W.WORD), ("Event", KEY_EVENT)]


def main():
    if sys.platform != "win32":
        print("skip: a Windows console is needed")
        return 0
    k32 = ctypes.WinDLL("kernel32", use_last_error=True)
    k32.CreateFileW.restype = W.HANDLE
    exe = sys.argv[1]
    lines = [("\x1a" if a == "^Z" else a) + "\r" for a in sys.argv[2:]]
    p = subprocess.Popen([exe], creationflags=CREATE_NEW_CONSOLE, close_fds=True)
    time.sleep(1.0)
    k32.FreeConsole()
    if not k32.AttachConsole(p.pid):
        print("AttachConsole failed: %d" % ctypes.get_last_error())
        p.kill()
        return 2
    hin = k32.CreateFileW("CONIN$", 0xC0000000, 3, None, 3, 0, None)

    def type_text(text):
        recs = []
        for ch in text:
            vk = 0x0D if ch == "\r" else (0x5A if ch == "\x1a" else 0)
            ctrl = 0x0008 if ch == "\x1a" else 0  # LEFT_CTRL_PRESSED
            for down in (1, 0):
                r = INPUT_RECORD()
                r.EventType = 1  # KEY_EVENT
                r.Event = KEY_EVENT(down, 1, vk, 0, ch, ctrl)
                recs.append(r)
        arr = (INPUT_RECORD * len(recs))(*recs)
        n = W.DWORD()
        k32.WriteConsoleInputW(hin, arr, len(recs), ctypes.byref(n))

    for line in lines:
        type_text(line)
        time.sleep(0.3)
    timed_out = False
    try:
        p.wait(timeout=5)
    except subprocess.TimeoutExpired:
        timed_out = True
    hout = k32.CreateFileW("CONOUT$", 0xC0000000, 3, None, 3, 0, None)

    class COORD(ctypes.Structure):
        _fields_ = [("X", W.SHORT), ("Y", W.SHORT)]

    class CSBI(ctypes.Structure):
        _fields_ = [("dwSize", COORD), ("dwCursorPosition", COORD), ("wAttributes", W.WORD),
                    ("srWindow", W.SMALL_RECT), ("dwMaximumWindowSize", COORD)]

    info = CSBI()
    k32.GetConsoleScreenBufferInfo(hout, ctypes.byref(info))
    width = max(info.dwSize.X, 1)
    rows = max(info.dwCursorPosition.Y + 1, 1)
    buf = ctypes.create_unicode_buffer(width * rows + 1)
    n = W.DWORD()
    k32.ReadConsoleOutputCharacterW(hout, buf, width * rows, 0, ctypes.byref(n))
    text = buf[:n.value]
    if p.poll() is None:
        p.kill()
    k32.FreeConsole()
    for i in range(0, len(text), width):
        row = text[i:i + width].rstrip()
        if row:
            print(row)
    print("TIMEOUT" if timed_out else "rc %d" % p.returncode)
    return 0


if __name__ == "__main__":
    sys.exit(main())
