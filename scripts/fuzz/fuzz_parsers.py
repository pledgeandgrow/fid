#!/usr/bin/env python3
"""Fuzz the stdlib parsers (roadmap VF15).

    python scripts/fuzz/fuzz_parsers.py --fid release/fid.exe [-n 2000] [--seed 1]
        [--parsers json,regex,http,csv,toml,xml,url,uuid] [--out build/fuzz-parsers]
        [--backend ccompile|emit]

tests/fuzz/parsers.fd is built once (through C by default, the fastest
backend to run); then each input, a mutant of a seed of that format, is
written to a file and the driver runs the parser on it. None of these entry
points is documented to panic (json_parse_safe, not json_parse), so a
finding is any panic, crash or hang (--timeout). Findings are kept as
<out>/<parser>-<n>.txt with the output beside it; one is kept per distinct
first line of the error.

The TLS record layer is not here: the stdlib has none yet (tls_crypto.fd:
"What remains for full TLS 1.3: the record layer").
"""
import argparse
import os
import random
import re
import subprocess
import sys

SEEDS = {
    "json": ['{"a": [1, 2.5, -3e2, true, false, null, {"b": "x\\n\\u00e9"}]}', '[]', '{}', '"s"',
             '[[[[1]]]]', '{"k": {"k": {"k": []}}}', '123', '-0.0', '"\\ud83d\\ude00"'],
    "regex": ["a*b\naaab", "^(ab|cd)+$\nabcdab", "[a-z0-9_]+@[a-z]+\\.com\nme@x.com",
              "x?y{2,3}\nyyy", "(a|b)*c\nababc", ".\\d\\w\\s\nx1_ ", "[^abc]\nd", "a{3}\naaa"],
    "http": ["GET /index.html?x=1&y=2 HTTP/1.1\r\nHost: example.com\r\nContent-Length: 0\r\n\r\n",
             "POST /api HTTP/1.1\r\nHost: a\r\nContent-Type: application/json\r\nContent-Length: 7\r\n\r\n{\"a\":1}",
             "GET / HTTP/1.0\r\n\r\n", "PUT /x HTTP/1.1\r\nTransfer-Encoding: chunked\r\n\r\n5\r\nhello\r\n0\r\n\r\n"],
    "csv": ['a,b,c\n1,2,3\n', 'name,"quoted, field","with ""quotes"""\nx,y,z\n', '\n\n,,\n', 'a;b\r\n1;2\r\n',
            'h1,h2\n"multi\nline",2\n'],
    "toml": ['title = "x"\n[owner]\nname = "Tom"\ndob = 1979-05-27T07:32:00Z\n',
             '[a.b]\nc = [1, 2, 3]\nd = { e = 1, f = "g" }\n', '[[arr]]\nx = 1\n[[arr]]\nx = 2\n',
             'k = """multi\nline"""\nb = true\nf = 3.14\n', "s = 'literal'\nn = -12\n"],
    "xml": ['<?xml version="1.0"?><root a="1" b=\'2\'><c>text &amp; more</c><d/><!-- c --></root>',
            '<a><b><c><d>deep</d></c></b></a>', '<x><![CDATA[raw <stuff>]]></x>', '<e attr="&lt;&gt;"/>'],
    "url": ["https://user:pw@example.com:8080/p/a/t/h?query=1&x=%20y#frag", "http://[::1]:80/",
            "mailto:me@x.com", "file:///c:/x/y", "/relative/path?a=b", "http://h/%zz%4"],
    "uuid": ["123e4567-e89b-12d3-a456-426614174000", "00000000-0000-0000-0000-000000000000",
             "{123E4567-E89B-12D3-A456-426614174000}", "123e4567e89b12d3a456426614174000"],
}

INTERESTING = ["\x00", "\\", '"', "'", "<", ">", "&", "%", "{", "}", "[", "]", "\r", "\n", ",", ";", "=",
               "-", "+", ".", "e", "9" * 30, "\\u", "\\ud800", "\xff", "\xe9", "(", ")", "*", "?", "|", "^", "$",
               "[[", "]]", "<!--", "]]>", "%%", "%0", "::", "//", "#", "@"]


def mutate(r, s):
    for _ in range(r.randint(1, 5)):
        n = len(s)
        k = r.randint(0, 6)
        i = r.randrange(n + 1)
        if k == 0 and n:
            s = s[:i] + s[i + r.randint(1, 8):]
        elif k == 1:
            s = s[:i] + r.choice(INTERESTING) + s[i:]
        elif k == 2 and n:
            j = min(n, i + r.randint(1, 16))
            s = s[:j] + s[i:j] * r.randint(1, 4) + s[j:]
        elif k == 3:
            s = s[:i]
        elif k == 4:
            d = r.choice([10, 100, 2000])
            o, c = r.choice([("[", "]"), ("{", "}"), ("<a>", "</a>"), ("(", ")"), ('{"a":', "}")])
            s = s[:i] + o * d + c * d + s[i:]
        elif k == 5 and n:
            s = s[:i] + chr(r.randrange(1, 256)) + s[i + 1:]
        else:
            s = s[:i] + str(r.choice([-1, 0, 2 ** 31, 2 ** 63, -2 ** 63, 10 ** 30])) + s[i:]
    return s


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--fid", default=os.path.join("release", "fid.exe"))
    ap.add_argument("-n", type=int, default=1000)
    ap.add_argument("--seed", type=int, default=1)
    ap.add_argument("--parsers", default=",".join(SEEDS))
    ap.add_argument("--out", default=os.path.join("build", "fuzz-parsers"))
    ap.add_argument("--backend", default="ccompile")
    ap.add_argument("--timeout", type=int, default=20)
    a = ap.parse_args()
    os.environ.setdefault("CC", "zig cc")
    os.makedirs(a.out, exist_ok=True)
    exe = os.path.abspath(os.path.join(a.out, "parsers.exe"))
    p = subprocess.run([a.fid, a.backend, os.path.join("tests", "fuzz", "parsers.fd"), "-o", exe],
                       capture_output=True, text=True, errors="replace")
    if p.returncode != 0 or not os.path.exists(exe):
        print("could not build tests/fuzz/parsers.fd:\n" + (p.stdout + p.stderr)[-3000:])
        sys.exit(2)
    r = random.Random(a.seed)
    parsers = a.parsers.split(",")
    inp = os.path.join(a.out, "input.txt")
    finds = {}
    per = {x: 0 for x in parsers}
    for i in range(a.n):
        which = parsers[i % len(parsers)]
        s = mutate(r, r.choice(SEEDS[which]))
        with open(inp, "wb") as f:
            f.write(s.encode("utf-8", "surrogatepass"))
        try:
            q = subprocess.run([exe, which, inp], capture_output=True, timeout=a.timeout)
            rc = q.returncode
            out = (q.stdout + q.stderr).decode("utf-8", "replace")
        except subprocess.TimeoutExpired:
            rc, out = "timeout", ""
        per[which] += 1
        if rc == 0:
            continue
        # a finding must happen again (an OOM can be the machine's)
        try:
            q2 = subprocess.run([exe, which, inp], capture_output=True, timeout=a.timeout)
            if q2.returncode == 0:
                print(f"[{i}] {which}: not reproducible (exit {rc}), dropped", flush=True)
                continue
        except subprocess.TimeoutExpired:
            pass
        first = next((l for l in out.split("\n") if "Panic" in l or "CRASH" in l or "overflow" in l), out[:80])
        first = re.sub(r"\d+", "N", first.strip())[:100]
        key = f"{which}: {rc if isinstance(rc, str) else 'exit ' + hex(rc & 0xFFFFFFFF)}: {first}"
        if key in finds:
            continue
        n = len(finds)
        finds[key] = n
        with open(os.path.join(a.out, f"{which}-{n}.txt"), "wb") as f:
            f.write(s.encode("utf-8", "surrogatepass"))
        with open(os.path.join(a.out, f"{which}-{n}.out"), "w", encoding="utf-8") as f:
            f.write(f"exit: {rc}\n{out[-4000:]}")
        print(f"[{i}] {key}", flush=True)
    print(f"{a.n} inputs ({', '.join(f'{k} {v}' for k, v in per.items())}): {len(finds)} distinct findings")
    for k in finds:
        print("  " + k)
    sys.exit(1 if finds else 0)


if __name__ == "__main__":
    main()
