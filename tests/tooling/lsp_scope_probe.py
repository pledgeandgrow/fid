#!/usr/bin/env python3
"""TL4 (docs/roadmap/competitive-roadmap.md): drive `fid lsp` over stdio and
check what compiler/lsp.fd added on Nov 16:
  - rename and references of a local stay in its scope (another function's
    local of the same name, a string and a comment are not touched);
  - references and rename of a top-level function reach the other file that
    declares it (helper.fd, not open in the editor, read from the
    document's directory);
  - inlay hints for a method call (`p.scale(3)` -> `factor:`), a function
    of an imported module (`greet("x", 2)` -> `who:`), and the type of a
    local declared without one (`var p = Pt { .. }` -> `: Pt`,
    `var t = a(1);` -> `: i64`).
Nov 17:
  - shadowing in one scope by declaration position: `var v = v + 10;`
    starts a new v whose right side is still the first one; a `var k` in
    an inner block hides the outer k there only;
  - references and rename of a top-level function skip a local of the
    same name (e()'s `var a`);
  - type hints of method-call initializers (`p.scale(3)`, `Pt::origin()`,
    a `-> Self` method) and generic returns (`first_of(nums)` with
    `nums: Vec<i64>`, `pick(p, d2)`, `pr.first()` of `impl<A, B> Pair<A, B>`).

Usage: python3 tests/tooling/lsp_scope_probe.py <fid>
Prints one `ok`/`FAIL` line per check; the exit status is 1 on any FAIL.
"""
import json
import os
import subprocess
import sys
import tempfile

MAIN = """use helper; use util::strs;

struct Pt {
    x: i64,
}

fn Pt_scale(self: Pt, factor: i64) -> i64 {
    return self.x * factor;
}

fn a(n: i64) -> i64 {
    var count: i64 = n;
    print("count");
    return count;
}

fn b() -> i64 {
    var count: i64 = 2;
    return count;
}

fn main() -> i64 {
    var p = Pt { x: 2 };
    var t = a(1);
    var r = p.scale(3);
    return t + r + b() + greet("x", 2);
}

fn c() -> i64 {
    var v: i64 = 1;
    var w: i64 = v;
    var v = v + 10;
    return v + w;
}

fn d(flag: bool) -> i64 {
    var k: i64 = 1;
    if flag {
        var k: i64 = 5;
        print(k);
    }
    return k;
}

fn e() -> i64 {
    var a: i64 = 1;
    return a + 2;
}

impl Pt {
    fn origin() -> Pt {
        return Pt { x: 0 };
    }
    fn twice(self: Pt) -> Self {
        return Pt { x: self.x * 2 };
    }
}

struct Pair<A, B> {
    fst: A,
    snd: B,
}

impl<A, B> Pair<A, B> {
    fn first(self: Pair<A, B>) -> A {
        return self.fst;
    }
}

fn first_of<T>(v: Vec<T>) -> T {
    return v.get(0);
}

fn pick<T>(x: T, y: T) -> T {
    return x;
}

fn more() -> i64 {
    var p = Pt { x: 2 };
    var r = p.scale(3);
    var q = Pt::origin();
    var d2 = q.twice();
    var nums: Vec<i64> = Vec::new();
    var f = first_of(nums);
    var g = pick(p, d2);
    var pr: Pair<str, i64> = Pair { fst: "x", snd: 1 };
    var h = pr.first();
    var s2 = shout("x");
    return r + f;
}
"""

MAIN_LINES = MAIN.split("\n")


def ln(text, nth=0):
    """The 0-based line of MAIN holding `text` (its nth such line)."""
    found = [i for i, l in enumerate(MAIN_LINES) if text in l]
    return found[nth]


def col(line, word, nth=0):
    """The column of the nth whole-word `word` on MAIN's line `line`."""
    import re
    return [m.start() for m in re.finditer(r"\b%s\b" % re.escape(word), MAIN_LINES[line])][nth]

HELPER = """pub fn greet(who: str, times: i64) -> i64 {
    print(who);
    return times;
}
"""

# a module imported from below the document's directory (util/strs.fd)
STRS = """pub fn shout(text: str) -> str {
    return text + "!";
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

    tmp = tempfile.mkdtemp(prefix="fid-lsp-scope-")
    main_path = os.path.join(tmp, "main.fd")
    helper_path = os.path.join(tmp, "helper.fd")
    os.makedirs(os.path.join(tmp, "util"), exist_ok=True)
    strs_path = os.path.join(tmp, "util", "strs.fd")
    for path, text in ((main_path, MAIN), (helper_path, HELPER), (strs_path, STRS)):
        with open(path, "w", encoding="utf-8", newline="\n") as f:
            f.write(text)
    uri = "file://" + main_path.replace("\\", "/")
    helper_uri = uri[: uri.rfind("/")] + "/helper.fd"
    strs_uri = uri[: uri.rfind("/")] + "/util/strs.fd"
    proc = subprocess.Popen([fid, "lsp"], stdin=subprocess.PIPE,
                            stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
    request(proc, 1, "initialize", {"capabilities": {}})
    send(proc, {"jsonrpc": "2.0", "method": "initialized", "params": {}})
    send(proc, {"jsonrpc": "2.0", "method": "textDocument/didOpen",
                "params": {"textDocument": {"uri": uri, "languageId": "fides",
                                            "version": 1, "text": MAIN}}})
    doc = {"uri": uri}

    # rename the local `count` of a (line 12, 0-based 11)
    ren = request(proc, 2, "textDocument/rename",
                  {"textDocument": doc, "position": {"line": 11, "character": 9},
                   "newName": "total_count"}) or {}
    edits = (ren.get("changes") or {}).get(uri, [])
    lines = sorted(e["range"]["start"]["line"] for e in edits)
    check("rename of a's local count: lines 12 and 14 only", lines == [11, 13])
    check("rename of a local: no other file", len(ren.get("changes") or {}) == 1)

    # references of b's local count (line 18, 0-based 17)
    refs = request(proc, 3, "textDocument/references",
                   {"textDocument": doc, "position": {"line": 17, "character": 9},
                    "context": {"includeDeclaration": True}}) or []
    rl = sorted(r["range"]["start"]["line"] for r in refs)
    check("references of b's local count: lines 18 and 19 only", rl == [17, 18])

    # references of greet (line 26, 0-based 25) reach helper.fd
    gcol = MAIN.split("\n")[25].index("greet")
    grefs = request(proc, 4, "textDocument/references",
                    {"textDocument": doc, "position": {"line": 25, "character": gcol + 1},
                     "context": {"includeDeclaration": True}}) or []
    guris = [r["uri"] for r in grefs]
    check("references of greet: in main.fd", uri in guris)
    check("references of greet: its declaration in helper.fd", helper_uri in guris)
    gren = request(proc, 5, "textDocument/rename",
                   {"textDocument": doc, "position": {"line": 25, "character": gcol + 1},
                    "newName": "welcome"}) or {}
    check("rename of greet: edits helper.fd too", helper_uri in (gren.get("changes") or {}))

    whole = {"start": {"line": 0, "character": 0}, "end": {"line": 40, "character": 0}}
    hints = request(proc, 6, "textDocument/inlayHint", {"textDocument": doc, "range": whole}) or []
    labels = [(h["position"]["line"], h["label"]) for h in hints]
    check("inlay hint factor: at p.scale(3)", (24, "factor:") in labels)
    check("inlay hint who: at greet(\"x\", 2) (imported)", (25, "who:") in labels)
    check("inlay hint times: at greet(\"x\", 2) (imported)", (25, "times:") in labels)
    check("type hint : Pt for var p", (22, ": Pt") in labels)
    check("type hint : i64 for var t = a(1)", (23, ": i64") in labels)

    # --- Nov 17: shadowing by declaration position, a top-level name
    # shadowed by a local, type hints of method calls and generic returns
    def occ_of(result):
        return sorted((r["range"]["start"]["line"], r["range"]["start"]["character"]) for r in result)

    def refs_at(rid, line, character):
        return occ_of(request(proc, rid, "textDocument/references",
                              {"textDocument": doc, "position": {"line": line, "character": character},
                               "context": {"includeDeclaration": True}}) or [])

    def rename_at(rid, line, character, new):
        r = request(proc, rid, "textDocument/rename",
                    {"textDocument": doc, "position": {"line": line, "character": character},
                     "newName": new}) or {}
        return occ_of((r.get("changes") or {}).get(uri, [])), r

    v1 = ln("var v: i64 = 1;")
    vw = ln("var w: i64 = v;")
    v2 = ln("var v = v + 10;")
    vr = ln("return v + w;")
    first_v = [(v1, col(v1, "v")), (vw, col(vw, "v")), (v2, col(v2, "v", 1))]
    second_v = [(v2, col(v2, "v")), (vr, col(vr, "v"))]
    got, _ = rename_at(20, v1, col(v1, "v"), "v0")
    check("rename of the first v: its uses, the right side of `var v = v + 10;`, not the second v",
          got == sorted(first_v))
    got, _ = rename_at(21, v2, col(v2, "v"), "v1")
    check("rename of the second v (same scope, declared later): its declaration and `return v` only",
          got == sorted(second_v))
    check("references from the right side of `var v = v + 10;`: the first v",
          refs_at(22, v2, col(v2, "v", 1)) == sorted(first_v))
    k1 = ln("var k: i64 = 1;")
    k2 = ln("var k: i64 = 5;")
    kp = ln("print(k);")
    kr = ln("return k;")
    check("references of the outer k: not the inner block's k",
          refs_at(23, k1, col(k1, "k")) == sorted([(k1, col(k1, "k")), (kr, col(kr, "k"))]))
    check("references of the inner k: its block only",
          refs_at(24, kp, col(kp, "k")) == sorted([(k2, col(k2, "k")), (kp, col(kp, "k"))]))
    ea = ln("var a: i64 = 1;")
    ear = ln("return a + 2;")
    fa = refs_at(25, 23, col(23, "a"))
    check("references of the function a: not the local a of e()",
          (23, col(23, "a")) in fa and all(l not in (ea, ear) for l, _ in fa))
    got, _ = rename_at(26, 23, col(23, "a"), "alpha")
    check("rename of the function a: e()'s local a left alone",
          (23, col(23, "a")) in got and all(l not in (ea, ear) for l, _ in got))
    got, _ = rename_at(27, ea, col(ea, "a"), "z")
    check("rename of e()'s local a: its two lines only", got == sorted([(ea, col(ea, "a")), (ear, col(ear, "a"))]))

    hints = request(proc, 28, "textDocument/inlayHint", {"textDocument": doc, "range": {
        "start": {"line": 0, "character": 0}, "end": {"line": len(MAIN_LINES), "character": 0}}}) or []
    labels = [(h["position"]["line"], h["label"]) for h in hints]
    check("type hint : i64 for var r = p.scale(3) (method call)", (24, ": i64") in labels)
    for text, want in (("var r = p.scale(3);", ": i64"),
                       ("var q = Pt::origin();", ": Pt"),
                       ("var d2 = q.twice();", ": Pt"),
                       ("var f = first_of(nums);", ": i64"),
                       ("var g = pick(p, d2);", ": Pt"),
                       ("var h = pr.first();", ": str")):
        line = ln(text, 1) if text == "var r = p.scale(3);" else ln(text)
        check("type hint %s for %s" % (want, text), (line, want) in labels)
    sl = ln('var s2 = shout("x");')
    check("type hint : str for var s2 = shout(\"x\") (function of util/strs.fd)", (sl, ": str") in labels)
    check("inlay hint text: at shout(\"x\") (imported from a subdirectory)", (sl, "text:") in labels)
    srefs = request(proc, 29, "textDocument/references",
                    {"textDocument": doc, "position": {"line": sl, "character": col(sl, "shout") + 1},
                     "context": {"includeDeclaration": True}}) or []
    check("references of shout: its declaration in util/strs.fd (an imported module below the directory)",
          strs_uri in [r["uri"] for r in srefs])
    sren = request(proc, 30, "textDocument/rename",
                   {"textDocument": doc, "position": {"line": sl, "character": col(sl, "shout") + 1},
                    "newName": "yell"}) or {}
    sch = sren.get("changes") or {}
    check("rename of shout: one WorkspaceEdit for main.fd and util/strs.fd", uri in sch and strs_uri in sch)
    check("no type hint for a declared type (var nums: Vec<i64>)",
          not any(l == ln("var nums: Vec<i64>") and lab.startswith(": ") for l, lab in labels))

    request(proc, 9, "shutdown", None)
    send(proc, {"jsonrpc": "2.0", "method": "exit", "params": None})
    try:
        proc.wait(timeout=10)
    except subprocess.TimeoutExpired:
        proc.kill()
    print("lsp-scope: " + ("PASS" if fails == 0 else "FAIL"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
