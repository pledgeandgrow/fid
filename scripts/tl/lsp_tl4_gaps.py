#!/usr/bin/env python3
"""TL4 (docs/roadmap/competitive-roadmap.md): the three inlay-hint gaps —
a type hint on a turbofish call `f::<T>(..)`, the methods of a type from an
imported module, and a local that shadows a function name taking its calls.

Usage: python3 scripts/tl/lsp_tl4_gaps.py <fid>
Prints one `ok`/`FAIL` line per check; the exit status is 1 on any FAIL.
"""
import json
import os
import subprocess
import sys
import tempfile

HELPER = """pub struct Pair {
    pub a: i64,
    pub b: i64,
}

impl Pair {
    pub fn sum(self, scale: i64) -> i64 {
        return (self.a + self.b) * scale;
    }
}

pub fn make_pair(a: i64, b: i64) -> Pair {
    return Pair { a: a, b: b };
}
"""

SOURCE = """use helper;

fn add(a: i64, b: i64) -> i64 {
    return a + b;
}

fn identity<T>(x: T) -> T {
    return x;
}

fn main() -> i64 {
    var p = make_pair(1, 2);
    var s = p.sum(10);
    var n = identity::<i64>(5);
    var add = 0;
    add(1, 2);
    return s + n;
}
"""


def send(proc, msg):
    body = json.dumps(msg).encode("utf-8")
    proc.stdin.write(b"Content-Length: %d\r\n\r\n" % len(body) + body)
    proc.stdin.flush()


def read_message(proc):
    length = 0
    while True:
        line = proc.stdout.readline()
        if not line:
            return None
        line = line.strip()
        if not line:
            break
        if line.lower().startswith(b"content-length:"):
            length = int(line.split(b":", 1)[1].strip())
    return json.loads(proc.stdout.read(length).decode("utf-8"))


def request(proc, rid, method, params):
    send(proc, {"jsonrpc": "2.0", "id": rid, "method": method, "params": params})
    while True:
        msg = read_message(proc)
        if msg is None:
            return None
        if msg.get("id") == rid:
            return msg.get("result")


def main():
    fid = sys.argv[1] if len(sys.argv) > 1 else "release/fid.exe"
    fails = 0

    def check(label, cond):
        nonlocal fails
        print(("  ok    " if cond else "  FAIL  ") + label)
        if not cond:
            fails += 1

    tmp = tempfile.mkdtemp(prefix="fid-tl4-")
    path = os.path.join(tmp, "probe.fd")
    with open(path, "w", encoding="utf-8", newline="\n") as f:
        f.write(SOURCE)
    with open(os.path.join(tmp, "helper.fd"), "w", encoding="utf-8", newline="\n") as f:
        f.write(HELPER)
    uri = "file://" + path.replace("\\", "/")
    proc = subprocess.Popen([fid, "lsp"], stdin=subprocess.PIPE,
                            stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
    request(proc, 1, "initialize", {"capabilities": {}})
    send(proc, {"jsonrpc": "2.0", "method": "initialized", "params": {}})
    send(proc, {"jsonrpc": "2.0", "method": "textDocument/didOpen",
                "params": {"textDocument": {"uri": uri, "languageId": "fides",
                                            "version": 1, "text": SOURCE}}})
    doc = {"uri": uri}
    whole = {"start": {"line": 0, "character": 0}, "end": {"line": 16, "character": 0}}
    hints = request(proc, 2, "textDocument/inlayHint",
                    {"textDocument": doc, "range": whole}) or []
    labels = [(h["position"]["line"], h["position"]["character"], h["label"])
              for h in hints]
    for l in labels:
        print("        hint", l)

    # TL4: the type of `s` is the imported impl method's return type.
    check("type hint : i64 on `var s = p.sum(10)` (line 13)",
          (12, 9, ": i64") in labels)
    # TL4: the type of `n` is the turbofish's type argument.
    check("type hint : i64 on `var n = identity::<i64>(5)` (line 14)",
          (13, 9, ": i64") in labels)
    # TL4: the imported impl method's parameter gets a name hint.
    check("param hint scale: at p.sum(10) (line 13)",
          (12, 18, "scale:") in labels)
    # TL4: a local `add` shadows `fn add`: `add(1, 2)` takes no hints of it.
    check("no a:/b: param hints at add(1, 2) under a shadowing local (line 16)",
          all(l[0] != 15 or l[2] not in ("a:", "b:") for l in labels))
    # ...but the unshadowed call still gets them (there is none here; make_pair
    # is an imported function and its params must still hint).
    check("param hints of the imported fn make_pair(a, b) (line 12)",
          (11, 22, "a:") in labels and (11, 25, "b:") in labels)

    try:
        proc.terminate()
    except Exception:
        pass
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
