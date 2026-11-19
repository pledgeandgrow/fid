#!/usr/bin/env python3
"""SP19 (docs/roadmap/llvm-roadmap.md): the LSP with FID_SERVE=<port> takes
its diagnostics from a running `fid serve --port <port>` (compiler.fd
check_text_errors), and they are the ones the LSP alone publishes.

Usage: python3 tests/tooling/serve_lsp_probe.py <fid> <port> <server log>
Run by tests/tooling/serve-socket-check.sh, which starts the server.
Exit status 1 on any failed check.
"""
import json
import os
import subprocess
import sys
import tempfile

SOURCE = """fn main() -> i64 {
    var x: i64 = "text";
    return x;
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


def diagnostics(fid, env, uri):
    proc = subprocess.Popen([fid, "lsp"], stdin=subprocess.PIPE,
                            stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, env=env)
    send(proc, {"jsonrpc": "2.0", "id": 1, "method": "initialize", "params": {"capabilities": {}}})
    while True:
        msg = read_message(proc)
        if msg is None or msg.get("id") == 1:
            break
    send(proc, {"jsonrpc": "2.0", "method": "initialized", "params": {}})
    send(proc, {"jsonrpc": "2.0", "method": "textDocument/didOpen",
                "params": {"textDocument": {"uri": uri, "languageId": "fides",
                                            "version": 1, "text": SOURCE}}})
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


def main():
    fid, port, log = sys.argv[1], sys.argv[2], sys.argv[3]
    fails = 0

    def check(label, cond):
        nonlocal fails
        print(("  ok    " if cond else "  FAIL  ") + label)
        if not cond:
            fails += 1

    tmp = tempfile.mkdtemp(prefix="fid-serve-lsp-")
    path = os.path.join(tmp, "doc.fd")
    with open(path, "w", encoding="utf-8", newline="\n") as f:
        f.write(SOURCE)
    uri = "file://" + path.replace("\\", "/")
    alone_env = dict(os.environ)
    alone_env.pop("FID_SERVE", None)
    served_env = dict(os.environ)
    served_env["FID_SERVE"] = port
    with open(log, encoding="utf-8", errors="replace") as f:
        before = f.read().count("fid serve: check-text")
    alone = diagnostics(fid, alone_env, uri)
    served = diagnostics(fid, served_env, uri)
    with open(log, encoding="utf-8", errors="replace") as f:
        after = f.read().count("fid serve: check-text")
    check("the LSP alone publishes the error", bool(alone))
    check("the LSP through the server publishes the same diagnostics", served == alone)
    check("the server answered the LSP's check-text request", after > before)
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
