#!/usr/bin/env python3
"""determinism-scan.py: no hash-map order reaches code generation
(docs/roadmap/llvm-roadmap.md RL13). Used by scripts/determinism-check.sh.

A HashMap's slots are in hash order. The compiler may look keys up (get,
insert, contains, remove, len, clear, clone, share: no order), but anything
that walks the slots hands that order on: HashMap_keys / HashMap_values
(core sorts the keys, but a map whose keys it does not own sorts them as
raw words: string keys by address), their _i forms, HashMap_to_str, the C
runtime's p_map_keys / p_map_values, core's slot helpers (hm_*), and a
`for k in map` loop (lowered to HashMap_keys). Random numbers
(p_os_random_u64) are another source of different output per run.

Two scans, each reporting every site with its enclosing function:

  source   compiler/*.fd: calls of the functions above, outside comments
           and string literals. `for x in <map>` needs types: the C scan
           finds it.
  c        a generated C file of the compiler (`fid dump-c compiler/main.fd`,
           or build/selfhost/fid-dev.c of a build): every call of those functions,
           with the function it is in. Higher-order dispatch by name
           (`strcmp(_fn_name, "HashMap_keys")` then the call) is a function
           value of that name, not a walk the compiler makes, and is skipped.

A site is allowed only when scripts/determinism-allow.txt names it:
  source <file> <function>   reason...
  c <function>               reason...
Anything else fails (exit status 1), so a new walk of a map, or a new
random number, in the compiler has to be looked at and either made
order-independent or listed with its reason.

Usage:
  python3 scripts/determinism-scan.py source [--allow FILE] [DIR]
  python3 scripts/determinism-scan.py c FILE.c [--allow FILE]
"""
import os
import re
import sys

ORDER_FNS = ["HashMap_keys", "HashMap_values", "HashMap_keys_i", "HashMap_values_i",
             "HashMap_to_str", "p_map_keys", "p_map_values", "p_os_random_u64"]
SRC_CALL = re.compile(r"\b(HashMap_keys_i|HashMap_values_i|HashMap_keys|HashMap_values|HashMap_to_str|"
                      r"p_map_keys|p_map_values|p_os_random_u64|hm_[a-z0-9_]+)\s*\(")
SRC_METHOD = re.compile(r"\.(keys|values)\s*\(\s*\)")
SRC_FN = re.compile(r"^\s*(?:@[A-Za-z_]+(?:\([^)]*\))?\s+)*(?:pub\s+)?fn\s+([A-Za-z_][A-Za-z0-9_]*)")
STRING = re.compile(r'"(?:[^"\\]|\\.)*"')
C_DEF = re.compile(r"^[A-Za-z_][A-Za-z0-9_ \*]*?[ \*]([A-Za-z_][A-Za-z0-9_]*)\(.*\)\s*\{\s*$")
C_CALL = re.compile(r"(?<![A-Za-z0-9_\"])(" + "|".join(sorted(ORDER_FNS, key=len, reverse=True)) + r")\s*\(")


def load_allow(path):
    src, c = set(), set()
    if not path or not os.path.exists(path):
        return src, c
    with open(path, encoding="utf-8") as fh:
        for raw in fh:
            line = raw.split("#", 1)[0].strip()
            if not line:
                continue
            parts = line.split()
            if parts[0] == "source" and len(parts) >= 3:
                src.add((parts[1].replace("\\", "/"), parts[2]))
            elif parts[0] == "c" and len(parts) >= 2:
                c.add(parts[1])
    return src, c


def strip_code(line):
    """The line without string literals and a trailing // comment."""
    t = STRING.sub('""', line)
    i = t.find("//")
    if i >= 0:
        t = t[:i]
    return t


def scan_source(root, allow_src):
    sites, bad = 0, 0
    files = sorted(f for f in os.listdir(root) if f.endswith(".fd"))
    for name in files:
        path = os.path.join(root, name)
        rel = (os.path.basename(root.rstrip("/\\")) + "/" + name).replace("\\", "/")
        fn = "<top>"
        with open(path, encoding="utf-8", errors="replace") as fh:
            for no, raw in enumerate(fh, 1):
                line = raw.rstrip("\r\n")
                m = SRC_FN.match(line)
                if m:
                    fn = m.group(1)
                if line.lstrip().startswith("//"):
                    continue
                t = strip_code(line)
                hits = [h.group(1) for h in SRC_CALL.finditer(t)]
                hits += ["." + h.group(1) + "()" for h in SRC_METHOD.finditer(t)]
                # a definition of a function of that name is not a call
                if m and m.group(1) in hits:
                    hits.remove(m.group(1))
                if not hits:
                    continue
                sites += 1
                ok = (rel, fn) in allow_src
                if not ok:
                    bad += 1
                print("  %s %s:%d %s: %s" % ("allowed" if ok else "NEW    ", rel, no, fn, ", ".join(hits)))
    print("source scan: %d sites in %s, %d not allowed" % (sites, root, bad))
    return bad


def scan_c(path, allow_c):
    sites, bad = 0, 0
    fn = "<top>"
    prev = ""
    seen = {}
    with open(path, encoding="utf-8", errors="replace") as fh:
        for raw in fh:
            line = raw.rstrip("\r\n")
            m = C_DEF.match(line)
            if m:
                fn = m.group(1)
            # declarations, string constants and name dispatch are not walks
            if line.endswith(");") and C_DEF.match(line[:-1] + " {"):
                prev = line
                continue
            if 'strcmp(_fn_name, "' in prev or line.lstrip().startswith("static const struct"):
                prev = line
                continue
            for h in C_CALL.finditer(STRING.sub('""', line)):
                if h.group(1) == fn and m:
                    continue
                key = (fn, h.group(1))
                seen[key] = seen.get(key, 0) + 1
            prev = line
    for (f, callee), n in sorted(seen.items()):
        sites += n
        ok = f in allow_c
        if not ok:
            bad += 1
        print("  %s %s -> %s (%d)" % ("allowed" if ok else "NEW    ", f, callee, n))
    print("C scan: %d call sites in %d functions of %s, %d functions not allowed"
          % (sites, len(set(k[0] for k in seen)), path, bad))
    return bad


def main(argv):
    if len(argv) < 2 or argv[1] not in ("source", "c"):
        print(__doc__)
        return 2
    allow = os.path.join(os.path.dirname(os.path.abspath(__file__)), "determinism-allow.txt")
    rest = []
    i = 2
    while i < len(argv):
        if argv[i] == "--allow" and i + 1 < len(argv):
            allow = argv[i + 1]
            i += 2
            continue
        rest.append(argv[i])
        i += 1
    allow_src, allow_c = load_allow(allow)
    if argv[1] == "source":
        root = rest[0] if rest else "compiler"
        return 1 if scan_source(root, allow_src) else 0
    if not rest:
        print("usage: determinism-scan.py c FILE.c")
        return 2
    return 1 if scan_c(rest[0], allow_c) else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
