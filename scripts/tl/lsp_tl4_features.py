#!/usr/bin/env python3
"""TL4 (docs/roadmap/competitive-roadmap.md): the features a complete LSP
needs that lsp_probe.py, lsp_tl4_gaps.py and tests/tooling/lsp-scope-check.sh
do not exercise — completion (global and after `.`), hover, go-to-definition
(a function, a local, a method, a function of an imported module), document
symbols, and diagnostics on open, on change and pulled
(textDocument/diagnostic).

Usage: python3 scripts/tl/lsp_tl4_features.py <fid>
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

struct Pt {
    x: i64,
    y: i64,
}

impl Pt {
    fn norm1(self) -> i64 {
        return self.x + self.y;
    }
}

fn add(a: i64, b: i64) -> i64 {
    return a + b;
}

fn main() -> i64 {
    var total: i64 = add(1, 2);
    var p: Pt = Pt { x: 1, y: 2 };
    var q = make_pair(3, 4);
    total = total + p.norm1() + q.sum(2);
    return total;
}
"""

BROKEN = SOURCE.replace("var total: i64 = add(1, 2);", "var total: i64 = add(1, 2) + nope;")


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


class Client:
    def __init__(self, proc):
        self.proc = proc
        self.notes = []
        self.rid = 0

    def request(self, method, params):
        self.rid += 1
        send(self.proc, {"jsonrpc": "2.0", "id": self.rid, "method": method, "params": params})
        while True:
            msg = read_message(self.proc)
            if msg is None:
                return None
            if msg.get("id") == self.rid and "method" not in msg:
                return msg
            if "method" in msg:
                self.notes.append(msg)

    def notify(self, method, params):
        send(self.proc, {"jsonrpc": "2.0", "method": method, "params": params})

    def diagnostics(self, uri):
        """The next publishDiagnostics for `uri` (reading until it comes)."""
        while True:
            for i, n in enumerate(self.notes):
                if n.get("method") == "textDocument/publishDiagnostics" and n["params"]["uri"] == uri:
                    del self.notes[i]
                    return n["params"]["diagnostics"]
            msg = read_message(self.proc)
            if msg is None:
                return None
            self.notes.append(msg)


def pos(line, character):
    return {"line": line, "character": character}


def main():
    fid = sys.argv[1] if len(sys.argv) > 1 else "release/fid.exe"
    fails = 0

    def check(label, cond):
        nonlocal fails
        print(("  ok    " if cond else "  FAIL  ") + label)
        if not cond:
            fails += 1

    tmp = tempfile.mkdtemp(prefix="fid-tl4f-")
    path = os.path.join(tmp, "probe.fd")
    with open(path, "w", encoding="utf-8", newline="\n") as f:
        f.write(SOURCE)
    hpath = os.path.join(tmp, "helper.fd")
    with open(hpath, "w", encoding="utf-8", newline="\n") as f:
        f.write(HELPER)
    uri = "file://" + path.replace("\\", "/")
    huri = "file://" + hpath.replace("\\", "/")
    proc = subprocess.Popen([fid, "lsp"], stdin=subprocess.PIPE,
                            stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
    c = Client(proc)
    init = c.request("initialize", {"capabilities": {}}) or {}
    caps = (init.get("result") or {}).get("capabilities", {})
    c.notify("initialized", {})
    for cap in ("completionProvider", "hoverProvider", "definitionProvider",
                "documentSymbolProvider", "diagnosticProvider"):
        check("capability " + cap, bool(caps.get(cap)))
    dp = caps.get("diagnosticProvider") or {}
    check("diagnosticProvider has interFileDependencies and workspaceDiagnostics",
          "interFileDependencies" in dp and "workspaceDiagnostics" in dp)

    lines = SOURCE.split("\n")

    def at(line_no, word, nth=0):
        """(0-based line, column) of the nth `word` on 0-based line_no."""
        col = -1
        for _ in range(nth + 1):
            col = lines[line_no].index(word, col + 1)
        return pos(line_no, col)

    doc = {"uri": uri}
    c.notify("textDocument/didOpen", {"textDocument": {
        "uri": uri, "languageId": "fides", "version": 1, "text": SOURCE}})
    d0 = c.diagnostics(uri)
    check("diagnostics on open: none for a clean file (got %r)" % (d0,), d0 == [])

    # diagnostics on change: an error appears, then clears
    c.notify("textDocument/didChange", {"textDocument": {"uri": uri, "version": 2},
                                        "contentChanges": [{"text": BROKEN}]})
    d1 = c.diagnostics(uri) or []
    check("diagnostics on change: the unknown name is reported (%d)" % len(d1),
          len(d1) >= 1 and any("nope" in d.get("message", "") for d in d1))
    check("diagnostics on change: on line 19",
          any(d["range"]["start"]["line"] == 18 for d in d1))
    pulled = c.request("textDocument/diagnostic", {"textDocument": doc})
    pr = (pulled or {}).get("result") or {}
    check("pull diagnostics: a full report with the error",
          pr.get("kind") == "full" and len(pr.get("items", [])) >= 1)
    c.notify("textDocument/didChange", {"textDocument": {"uri": uri, "version": 3},
                                        "contentChanges": [{"text": SOURCE}]})
    d2 = c.diagnostics(uri)
    check("diagnostics on change: cleared once fixed (got %r)" % (d2,), d2 == [])

    # completion
    comp = (c.request("textDocument/completion",
                      {"textDocument": doc, "position": pos(21, 4)}) or {}).get("result") or []
    labels = [i.get("label") for i in comp] if isinstance(comp, list) else \
        [i.get("label") for i in comp.get("items", [])]
    check("completion: the file's function add", "add" in labels)
    check("completion: the struct Pt", "Pt" in labels)
    check("completion: a keyword (while)", "while" in labels)
    dot_line = "    total = total + p."
    c.notify("textDocument/didChange", {"textDocument": {"uri": uri, "version": 4},
                                        "contentChanges": [{"text": SOURCE.replace(
                                            "    total = total + p.norm1() + q.sum(2);", dot_line)}]})
    c.diagnostics(uri)
    dcomp = (c.request("textDocument/completion",
                       {"textDocument": doc, "position": pos(21, len(dot_line))}) or {}).get("result") or []
    dlabels = [i.get("label") for i in dcomp] if isinstance(dcomp, list) else \
        [i.get("label") for i in dcomp.get("items", [])]
    check("completion after `p.` (p: Pt): the method norm1 (%r)" % (dlabels,), "norm1" in dlabels)
    check("completion after `p.` (p: Pt): the field x", "x" in dlabels)
    qdot = "    total = total + q."
    c.notify("textDocument/didChange", {"textDocument": {"uri": uri, "version": 5},
                                        "contentChanges": [{"text": SOURCE.replace(
                                            "    total = total + p.norm1() + q.sum(2);", qdot)}]})
    c.diagnostics(uri)
    qcomp = (c.request("textDocument/completion",
                       {"textDocument": doc, "position": pos(21, len(qdot))}) or {}).get("result") or []
    qlabels = [i.get("label") for i in qcomp] if isinstance(qcomp, list) else []
    check("completion after `q.` (q = make_pair(..), imported Pair): sum and field a (%r)" % (qlabels,),
          "sum" in qlabels and "a" in qlabels)
    sdot = "        return self."
    c.notify("textDocument/didChange", {"textDocument": {"uri": uri, "version": 6},
                                        "contentChanges": [{"text": SOURCE.replace(
                                            "        return self.x + self.y;", sdot)}]})
    c.diagnostics(uri)
    scomp = (c.request("textDocument/completion",
                       {"textDocument": doc, "position": pos(9, len(sdot))}) or {}).get("result") or []
    slabels = [i.get("label") for i in scomp] if isinstance(scomp, list) else []
    check("completion after `self.` in impl Pt: x, y and norm1 (%r)" % (slabels,),
          "x" in slabels and "y" in slabels and "norm1" in slabels)
    c.notify("textDocument/didChange", {"textDocument": {"uri": uri, "version": 7},
                                        "contentChanges": [{"text": SOURCE}]})
    c.diagnostics(uri)

    # hover
    hv = (c.request("textDocument/hover", {"textDocument": doc, "position": at(18, "add")}) or {}).get("result") or {}
    hval = (hv.get("contents") or {}).get("value", "")
    check("hover on add: its signature (%r)" % hval, "fn add(a: i64, b: i64) -> i64" in hval)
    hv2 = (c.request("textDocument/hover", {"textDocument": doc, "position": at(19, "Pt")}) or {}).get("result") or {}
    check("hover on Pt: the struct", "struct Pt" in (hv2.get("contents") or {}).get("value", ""))

    # go-to-definition
    def definition(p):
        r = (c.request("textDocument/definition", {"textDocument": doc, "position": p}) or {}).get("result")
        if isinstance(r, list):
            r = r[0] if r else None
        return r

    d = definition(at(18, "add"))
    check("definition of add: line 14", d is not None and d["uri"] == uri and d["range"]["start"]["line"] == 13)
    d = definition(at(21, "total", 1))
    check("definition of the local total: its var on line 19",
          d is not None and d["range"]["start"]["line"] == 18 and d["range"]["start"]["character"] == 8)
    d = definition(at(21, "norm1"))
    check("definition of the method norm1: line 9", d is not None and d["range"]["start"]["line"] == 8)
    d = definition(at(20, "make_pair"))
    check("definition of make_pair: in helper.fd (another file) (%r)" % (d,),
          d is not None and d["uri"] == huri and d["range"]["start"]["line"] == 11)
    d = definition(at(21, "sum"))
    check("definition of the imported method sum: in helper.fd",
          d is not None and d["uri"] == huri and d["range"]["start"]["line"] == 6)

    # document symbols
    syms = (c.request("textDocument/documentSymbol", {"textDocument": doc}) or {}).get("result") or []
    names = [s.get("name") for s in syms]
    check("document symbols: Pt, add, main (%r)" % names,
          "Pt" in names and "add" in names and "main" in names)
    impl = [s for s in syms if s.get("name", "").startswith("impl")]
    check("document symbols: impl Pt with its method norm1",
          len(impl) == 1 and [ch.get("name") for ch in impl[0].get("children", [])] == ["norm1"])
    fn_add = [s for s in syms if s.get("name") == "add"]
    check("document symbols: add is a Function (12) spanning lines 14-16",
          len(fn_add) == 1 and fn_add[0]["kind"] == 12 and fn_add[0]["range"]["start"]["line"] == 13
          and fn_add[0]["range"]["end"]["line"] == 15)

    # an unknown request is answered with an error, the server keeps going
    unk = c.request("textDocument/fidesNoSuchThing", {"textDocument": doc}) or {}
    check("an unknown request gets MethodNotFound", (unk.get("error") or {}).get("code") == -32601)

    try:
        c.request("shutdown", None)
        c.notify("exit", None)
        proc.wait(timeout=10)
    except Exception:
        proc.kill()
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
