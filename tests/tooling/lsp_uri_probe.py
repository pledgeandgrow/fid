#!/usr/bin/env python3
"""BG53 (docs/roadmap/bugs-roadmap.md): `fid lsp` decodes a document's URI
(`file:///c%3A/my%20dir/main.fd`, as VS Code sends it) before using it as
a path (compiler/lsp.fd lsp_uri_path). Before, the diagnostics took the
raw URI text as the file's path, so every `use` of a sibling module was
unresolved. Checked on a main.fd in a directory whose name has a space and
a `+` (sent as %20 and %2B), with its drive letter's colon as %3A on
Windows, that uses helper.fd beside it:
  - no diagnostics (the sibling module is found);
  - a real error in the file is still reported.

Usage: python3 tests/tooling/lsp_uri_probe.py <fid>
Prints one `ok`/`FAIL` line per check; the exit status is 1 on any FAIL.
"""
import json
import os
import shutil
import subprocess
import sys
import tempfile
import urllib.parse

MAIN_OK = """use helper;

fn main() -> i64 {
    return greet(2);
}
"""

MAIN_BAD = """use helper;

fn main() -> i64 {
    var x: i64 = "text";
    return greet(x);
}
"""

HELPER = """pub fn greet(n: i64) -> i64 {
    return n + 1;
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


def diagnostics(fid, uri, text):
    proc = subprocess.Popen([fid, "lsp"], stdin=subprocess.PIPE,
                            stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
    send(proc, {"jsonrpc": "2.0", "id": 1, "method": "initialize", "params": {"capabilities": {}}})
    while True:
        msg = read_message(proc)
        if msg is None or msg.get("id") == 1:
            break
    send(proc, {"jsonrpc": "2.0", "method": "initialized", "params": {}})
    send(proc, {"jsonrpc": "2.0", "method": "textDocument/didOpen",
                "params": {"textDocument": {"uri": uri, "languageId": "fides",
                                            "version": 1, "text": text}}})
    diags = None
    while diags is None:
        msg = read_message(proc)
        if msg is None:
            break
        if msg.get("method") == "textDocument/publishDiagnostics":
            diags = msg["params"]["diagnostics"]
    send(proc, {"jsonrpc": "2.0", "id": 9, "method": "shutdown", "params": None})
    send(proc, {"jsonrpc": "2.0", "method": "exit", "params": None})
    try:
        proc.wait(timeout=30)
    except subprocess.TimeoutExpired:
        proc.kill()
    return diags


def editor_uri(path):
    """The URI an editor sends: every reserved byte escaped, `:` too."""
    p = os.path.abspath(path).replace("\\", "/")
    if not p.startswith("/"):
        p = "/" + p
    return "file://" + urllib.parse.quote(p, safe="/")


def main():
    fid = sys.argv[1] if len(sys.argv) > 1 else "release/fid.exe"
    fails = 0

    def check(label, cond):
        nonlocal fails
        print(("  ok    " if cond else "  FAIL  ") + label)
        if not cond:
            fails += 1

    base = tempfile.mkdtemp(prefix="fid-lsp-uri-")
    try:
        d = os.path.join(base, "my dir+x")
        os.makedirs(d)
        main_path = os.path.join(d, "main.fd")
        with open(os.path.join(d, "helper.fd"), "w", encoding="utf-8", newline="\n") as f:
            f.write(HELPER)
        with open(main_path, "w", encoding="utf-8", newline="\n") as f:
            f.write(MAIN_OK)
        uri = editor_uri(main_path)
        check("the URI is escaped (%20, %2B)", "%20" in uri and "%2B" in uri)
        ok_diags = diagnostics(fid, uri, MAIN_OK)
        ok_errs = [x for x in (ok_diags or []) if x.get("severity", 1) == 1]
        check("a sibling `use` is resolved: no errors (got %r)" % (ok_errs,),
              ok_diags is not None and len(ok_errs) == 0)
        bad_diags = diagnostics(fid, uri, MAIN_BAD)
        bad_errs = [x for x in (bad_diags or []) if x.get("severity", 1) == 1]
        check("a type error is still reported", len(bad_errs) > 0)
    finally:
        shutil.rmtree(base, ignore_errors=True)
    print("lsp-uri-check: " + ("PASS" if fails == 0 else "FAIL"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
