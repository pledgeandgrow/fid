#!/usr/bin/env python3
"""The LSP bugs of docs/roadmap/bugs-roadmap.md fixed in compiler/lsp.fd,
driven over stdio like lsp_probe.py:

  BG325  rename and references see the names inside an f-string's `{...}`
         (locals and functions), its literal text and a format spec left out
  BG330  positions are UTF-16 code units: initialize announces
         positionEncoding "utf-16", a cursor after non-ASCII text is read
         right, and ranges after it (2-byte and 4-byte characters) land on
         the name
  BG461  hover answers in a file with a `use` (an imported function and a
         core one)
  BG488  the shutdown handshake: `exit` without `shutdown` ends with 1, a
         request after `shutdown` gets InvalidRequest (-32600), `exit` after
         it ends with 0

Usage: python3 scripts/tl/lsp_bug_fixes.py <fid>
Prints one `ok`/`FAIL` line per check; the exit status is 1 on any FAIL.
"""
import json
import os
import subprocess
import sys
import tempfile

# BG325: one statement per line
RENAME_SRC = """fn total(a: i64) -> i64 {
    return a + 1;
}

fn main() {
    val count = total(2);
    println(f"count is {count} and {total(5)}");
    println(f"{count:>6}|");
}
"""

# BG330: a 2-byte and a 4-byte character before a name (built with chr()
# here; the document holds the characters)
UTF16_SRC = ("fn main() {\n"
             "    val name: str = \"x\";\n"
             "    val s = \"h" + chr(0xE9) + chr(0xE9) + "lo\" + name;\n"
             "    val t = \"" + chr(0x1F600) + "\" + name;\n"
             "    println(s + t);\n"
             "}\n")

# BG461: an imported function and a core one
HOVER_SRC = """use string;

fn main() {
    println(str_pad_left("7", 3, 48));
    println(int_to_str(5));
}
"""


def send(proc, msg):
    # raw UTF-8, as editors send it (a 4-byte character as its 4 bytes, not
    # a JSON escape pair)
    body = json.dumps(msg, ensure_ascii=False).encode("utf-8")
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


def request_msg(proc, rid, method, params):
    send(proc, {"jsonrpc": "2.0", "id": rid, "method": method, "params": params})
    while True:
        msg = read_message(proc)
        if msg is None:
            return None
        if msg.get("id") == rid:
            return msg


def request(proc, rid, method, params):
    msg = request_msg(proc, rid, method, params)
    return None if msg is None else msg.get("result")


def u16_col(line, idx):
    """The UTF-16 column of code-point index `idx` of `line`."""
    return len(line[:idx].encode("utf-16-le")) // 2


def write(tmp, name, text):
    path = os.path.join(tmp, name)
    with open(path, "w", encoding="utf-8", newline="\n") as f:
        f.write(text)
    return path, "file://" + path.replace("\\", "/")


def start(fid):
    return subprocess.Popen([fid, "lsp"], stdin=subprocess.PIPE,
                            stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)


def open_doc(proc, uri, text):
    send(proc, {"jsonrpc": "2.0", "method": "textDocument/didOpen",
                "params": {"textDocument": {"uri": uri, "languageId": "fides",
                                            "version": 1, "text": text}}})


def edits_of(result):
    if not result:
        return []
    out = []
    for _, v in result.get("changes", {}).items():
        for e in v:
            r = e["range"]
            out.append((r["start"]["line"], r["start"]["character"], r["end"]["character"], e["newText"]))
    return sorted(out)


def locs_of(result):
    return sorted((l["range"]["start"]["line"], l["range"]["start"]["character"],
                   l["range"]["end"]["character"]) for l in (result or []))


def stop(proc):
    try:
        request(proc, 99, "shutdown", None)
        send(proc, {"jsonrpc": "2.0", "method": "exit", "params": None})
        proc.wait(timeout=30)
    except Exception:
        proc.kill()


def wait_code(proc):
    try:
        return proc.wait(timeout=30)
    except Exception:
        proc.kill()
        return None


def main():
    fid = sys.argv[1] if len(sys.argv) > 1 else "release/fid.exe"
    fails = 0
    try:
        sys.stdout.reconfigure(errors="backslashreplace")
    except Exception:
        pass

    def check(label, cond):
        nonlocal fails
        print(("  ok    " if cond else "  FAIL  ") + label)
        if not cond:
            fails += 1

    tmp = tempfile.mkdtemp(prefix="fid-lspbug-")

    # --- BG325 ---
    _, ruri = write(tmp, "r.fd", RENAME_SRC)
    rl = RENAME_SRC.split("\n")
    proc = start(fid)
    init = request(proc, 1, "initialize", {"capabilities": {}}) or {}
    send(proc, {"jsonrpc": "2.0", "method": "initialized", "params": {}})
    open_doc(proc, ruri, RENAME_SRC)
    doc = {"uri": ruri}
    c5 = rl[5].index("count")
    c6 = rl[6].index("{count}") + 1
    c7 = rl[7].index("{count") + 1
    t0 = rl[0].index("total")
    t5 = rl[5].index("total")
    t6 = rl[6].index("{total") + 1
    res = request(proc, 2, "textDocument/rename",
                  {"textDocument": doc, "position": {"line": 5, "character": c5}, "newName": "num"})
    got = edits_of(res)
    want = [(5, c5, c5 + 5, "num"), (6, c6, c6 + 5, "num"), (7, c7, c7 + 5, "num")]
    check("BG325 rename of a local edits its f-string uses, not the text or the format spec: %s" % got,
          got == want)
    res = request(proc, 3, "textDocument/references",
                  {"textDocument": doc, "position": {"line": 5, "character": c5},
                   "context": {"includeDeclaration": True}})
    got = locs_of(res)
    check("BG325 references of a local list its f-string uses: %s" % got,
          got == [(5, c5, c5 + 5), (6, c6, c6 + 5), (7, c7, c7 + 5)])
    res = request(proc, 4, "textDocument/rename",
                  {"textDocument": doc, "position": {"line": 0, "character": t0}, "newName": "sum1"})
    got = edits_of(res)
    want = [(0, t0, t0 + 5, "sum1"), (5, t5, t5 + 5, "sum1"), (6, t6, t6 + 5, "sum1")]
    check("BG325 rename of a function edits its call in an f-string: %s" % got, got == want)

    # --- BG330 ---
    caps = init.get("capabilities", {})
    check("BG330 initialize announces positionEncoding utf-16 (%r)" % caps.get("positionEncoding"),
          caps.get("positionEncoding") == "utf-16")
    _, uuri = write(tmp, "u.fd", UTF16_SRC)
    ul = UTF16_SRC.split("\n")
    open_doc(proc, uuri, UTF16_SRC)
    udoc = {"uri": uuri}
    n1 = u16_col(ul[1], ul[1].index("name"))
    n2 = u16_col(ul[2], ul[2].index("name"))
    n3 = u16_col(ul[3], ul[3].index("name"))
    res = request(proc, 5, "textDocument/references",
                  {"textDocument": udoc, "position": {"line": 1, "character": n1},
                   "context": {"includeDeclaration": True}})
    got = locs_of(res)
    check("BG330 references after a 2-byte and a 4-byte character, in UTF-16 units: %s (want columns %d %d %d)"
          % (got, n1, n2, n3),
          got == [(1, n1, n1 + 4), (2, n2, n2 + 4), (3, n3, n3 + 4)])
    # the cursor itself in UTF-16 units, after the 4-byte character
    res = request(proc, 6, "textDocument/rename",
                  {"textDocument": udoc, "position": {"line": 3, "character": n3 + 1}, "newName": "nm"})
    got = edits_of(res)
    check("BG330 rename at a UTF-16 cursor after an emoji edits `name` everywhere: %s" % got,
          got == [(1, n1, n1 + 4, "nm"), (2, n2, n2 + 4, "nm"), (3, n3, n3 + 4, "nm")])
    # the edits applied in UTF-16 units give the renamed text, no other
    # character touched
    lines16 = [l.encode("utf-16-le") for l in ul]
    for (ln, a, b, txt) in sorted(got, reverse=True):
        lines16[ln] = lines16[ln][:2 * a] + txt.encode("utf-16-le") + lines16[ln][2 * b:]
    applied = [l.decode("utf-16-le", errors="replace") for l in lines16]
    check("BG330 the edits applied give the renamed lines 2 and 3",
          applied[2] == ul[2].replace("name", "nm") and applied[3] == ul[3].replace("name", "nm"))
    stop(proc)

    # --- BG461 ---
    _, suri = write(tmp, "s.fd", HOVER_SRC)
    sl = HOVER_SRC.split("\n")
    proc = start(fid)
    request(proc, 1, "initialize", {"capabilities": {}})
    send(proc, {"jsonrpc": "2.0", "method": "initialized", "params": {}})
    open_doc(proc, suri, HOVER_SRC)
    sdoc = {"uri": suri}
    h1 = request(proc, 2, "textDocument/hover",
                 {"textDocument": sdoc, "position": {"line": 3, "character": sl[3].index("str_pad_left") + 2}})
    h2 = request(proc, 3, "textDocument/hover",
                 {"textDocument": sdoc, "position": {"line": 4, "character": sl[4].index("int_to_str") + 2}})
    check("BG461 hover of an imported function in a file with `use` (%s)" % json.dumps(h1)[:80],
          h1 is not None and "str_pad_left" in json.dumps(h1))
    check("BG461 hover of a core function in a file with `use` (%s)" % json.dumps(h2)[:80],
          h2 is not None and "int_to_str" in json.dumps(h2))
    stop(proc)

    # --- BG488 ---
    proc = start(fid)
    request(proc, 1, "initialize", {"capabilities": {}})
    send(proc, {"jsonrpc": "2.0", "method": "initialized", "params": {}})
    open_doc(proc, suri, HOVER_SRC)
    request(proc, 2, "textDocument/hover",
            {"textDocument": sdoc, "position": {"line": 4, "character": 14}})
    send(proc, {"jsonrpc": "2.0", "method": "exit", "params": None})
    code = wait_code(proc)
    check("BG488 exit without shutdown ends with 1 (got %r)" % code, code == 1)

    proc = start(fid)
    request(proc, 1, "initialize", {"capabilities": {}})
    send(proc, {"jsonrpc": "2.0", "method": "initialized", "params": {}})
    open_doc(proc, suri, HOVER_SRC)
    sd = request_msg(proc, 2, "shutdown", None)
    check("BG488 shutdown answers null", sd is not None and "result" in sd and sd["result"] is None)
    after = request_msg(proc, 3, "textDocument/hover",
                        {"textDocument": sdoc, "position": {"line": 4, "character": 14}})
    err = (after or {}).get("error") or {}
    check("BG488 a request after shutdown gets InvalidRequest (%s)" % json.dumps(after)[:100],
          err.get("code") == -32600 and "result" not in (after or {}))
    send(proc, {"jsonrpc": "2.0", "method": "exit", "params": None})
    code = wait_code(proc)
    check("BG488 exit after shutdown ends with 0 (got %r)" % code, code == 0)

    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
