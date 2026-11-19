#!/usr/bin/env python3
"""TL4 (docs/roadmap/competitive-roadmap.md): drive `fid lsp` over stdio and
check the requests the roadmap names: inlay hints (parameter names) and code
actions (quick fixes), beside references, rename, formatting and signature
help, which were there before (release-roadmap N7: references, signature
help and formatting are checked here too).

Usage: python3 scripts/tl/lsp_probe.py <fid>
Prints one `ok`/`FAIL` line per check; the exit status is 1 on any FAIL.
"""
import json
import os
import subprocess
import sys
import tempfile

SOURCE = """fn add(a: i64, b: i64) -> i64 {
    return a + b;
}

fn main() -> i64 {
    val x: i64 = add(1, 2);
    x = add(x, 3);
    print(x);
    return 0;
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

    tmp = tempfile.mkdtemp(prefix="fid-lsp-")
    path = os.path.join(tmp, "probe.fd")
    with open(path, "w", encoding="utf-8", newline="\n") as f:
        f.write(SOURCE)
    uri = "file://" + path.replace("\\", "/")
    proc = subprocess.Popen([fid, "lsp"], stdin=subprocess.PIPE,
                            stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
    init = request(proc, 1, "initialize", {"capabilities": {}}) or {}
    caps = init.get("capabilities", {})
    for cap in ("referencesProvider", "renameProvider", "documentFormattingProvider",
                "signatureHelpProvider", "inlayHintProvider", "codeActionProvider"):
        check("capability " + cap, bool(caps.get(cap)))
    send(proc, {"jsonrpc": "2.0", "method": "initialized", "params": {}})
    send(proc, {"jsonrpc": "2.0", "method": "textDocument/didOpen",
                "params": {"textDocument": {"uri": uri, "languageId": "fides",
                                            "version": 1, "text": SOURCE}}})
    doc = {"uri": uri}
    whole = {"start": {"line": 0, "character": 0}, "end": {"line": 20, "character": 0}}

    hints = request(proc, 2, "textDocument/inlayHint", {"textDocument": doc, "range": whole}) or []
    labels = [(h["position"]["line"], h["position"]["character"], h["label"]) for h in hints]
    check("inlay hint a: at add(1, ...) on line 6", (5, 21, "a:") in labels)
    check("inlay hint b: at add(..., 2) on line 6", (5, 24, "b:") in labels)
    check("no inlay hint on the declaration of add", all(l != 0 for (l, _, _) in labels))

    line7 = {"start": {"line": 6, "character": 0}, "end": {"line": 6, "character": 20}}
    actions = request(proc, 3, "textDocument/codeAction",
                      {"textDocument": doc, "range": line7, "context": {"diagnostics": []}}) or []
    titles = [a.get("title") for a in actions]
    check("quick fix: declare 'x' with var", "declare 'x' with var" in titles)
    edit_ok = False
    for a in actions:
        if a.get("title") == "declare 'x' with var":
            edits = a["edit"]["changes"][uri]
            e = edits[0]
            edit_ok = (e["newText"] == "var" and e["range"]["start"] == {"line": 5, "character": 4}
                       and e["range"]["end"] == {"line": 5, "character": 7})
    check("quick fix edit replaces `val` on line 6", edit_ok)

    refs = request(proc, 4, "textDocument/references",
                   {"textDocument": doc, "position": {"line": 0, "character": 4},
                    "context": {"includeDeclaration": True}}) or []
    check("references of add: 3", len(refs) == 3)

    # N7 (release-roadmap): signature help inside `add(x, |3)` on line 7.
    sig = request(proc, 6, "textDocument/signatureHelp",
                  {"textDocument": doc, "position": {"line": 6, "character": 15}}) or {}
    sig_labels = [s.get("label") for s in sig.get("signatures", [])]
    check("signature help names add(a: i64, b: i64) -> i64",
          "fn add(a: i64, b: i64) -> i64" in sig_labels)
    check("signature help: active parameter 1", sig.get("activeParameter") == 1)

    # N7: formatting of a badly indented document gives one whole-document
    # edit with the body indented; a formatted document gives no edit.
    ugly_uri = "file://" + os.path.join(tmp, "ugly.fd").replace("\\", "/")
    ugly = "fn main() -> i64 {\nreturn 0;\n}\n"
    send(proc, {"jsonrpc": "2.0", "method": "textDocument/didOpen",
                "params": {"textDocument": {"uri": ugly_uri, "languageId": "fides",
                                            "version": 1, "text": ugly}}})
    fmt_edits = request(proc, 7, "textDocument/formatting",
                        {"textDocument": {"uri": ugly_uri},
                         "options": {"tabSize": 4, "insertSpaces": True}}) or []
    check("formatting: one edit for an unformatted document", len(fmt_edits) == 1)
    check("formatting: body indented by four spaces",
          len(fmt_edits) == 1 and "\n    return 0;\n" in fmt_edits[0].get("newText", ""))
    if len(fmt_edits) == 1:
        clean = fmt_edits[0]["newText"]
        send(proc, {"jsonrpc": "2.0", "method": "textDocument/didChange",
                    "params": {"textDocument": {"uri": ugly_uri, "version": 2},
                               "contentChanges": [{"text": clean}]}})
        again = request(proc, 8, "textDocument/formatting",
                        {"textDocument": {"uri": ugly_uri},
                         "options": {"tabSize": 4, "insertSpaces": True}})
        check("formatting: no edit once formatted", again == [])

    request(proc, 5, "shutdown", None)
    send(proc, {"jsonrpc": "2.0", "method": "exit"})
    try:
        proc.wait(timeout=10)
    except subprocess.TimeoutExpired:
        proc.kill()
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
