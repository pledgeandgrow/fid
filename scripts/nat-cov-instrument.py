#!/usr/bin/env python3
"""Block and line counters in the compiler's generated C (own-backend
roadmap RL12; the block half is scripts/verify/cov_instrument.py's).

    python3 scripts/nat-cov-instrument.py <in.c> <out.c>
    python3 scripts/nat-cov-instrument.py --self-test

The C of `fid dump-c compiler/main.fd` has one C function per Fides
function and a label `bbN:` per basic block of the IR. Every function entry
and every block label gets a counter (block coverage = branch coverage at
the IR level). Line coverage needs to know which source line each piece of
C came from: with FID_C_LINE_MARKS=1 the C generator writes `/*@L<n>*/`
before the C of an instruction on a new source line n (compiler/
codegen_c.fd; `-g`'s `#line <n> "<file>"` directives are read the same
way), and each of those gets a counter too. Without marks only blocks are
counted.

<out.c>.map gets one line per counter: `ID function kind`, kind being
`entry`, `bbN` or `L<n>` (n the line in the function's own .fd file;
scripts/nat-coverage.py finds that file from the function's name).

At exit the instrumented compiler writes the counters it hit as a bitmap:
to $FID_COV_OUT if set, else into $FID_COV_DIR as one new file per process
(<random>-<clock>-<n>.cov); neither set: nothing is written. (The same
format as cov_instrument.py: scripts/verify/coverage.py merges either.)
"""
import re
import sys

FUNC = re.compile(r"^[A-Za-z_][A-Za-z0-9_ \*]*?\b([A-Za-z_][A-Za-z0-9_]*)\(([^;{}]*)\) \{$")
LABEL = re.compile(r"^(\s*)(bb\d+):\s*$")
MARK = re.compile(r"^(\s*)/\*@L(\d+)\*/\s*$")
HASHLINE = re.compile(r'^#line (\d+) "')


def instrument(lines):
    """(instrumented lines, map lines, counters)."""
    out = []
    mp = []
    cur = None
    n = 0
    last_line = None
    for line in lines:
        m = FUNC.match(line)
        if m and not line.startswith(("static inline", "typedef")):
            cur = m.group(1)
            last_line = None
            out.append(line)
            out.append(f"    __fcov[{n}] = 1;")
            mp.append(f"{n} {cur} entry")
            n += 1
            continue
        m = LABEL.match(line)
        if m and cur:
            out.append(line)
            out.append(f"{m.group(1)}  __fcov[{n}] = 1;")
            mp.append(f"{n} {cur} {m.group(2)}")
            n += 1
            last_line = None
            continue
        m = MARK.match(line)
        if m and cur:
            out.append(line)
            ln = int(m.group(2))
            out.append(f"{m.group(1)}__fcov[{n}] = 1;")
            mp.append(f"{n} {cur} L{ln}")
            n += 1
            last_line = ln
            continue
        m = HASHLINE.match(line)
        if m and cur:
            out.append(line)
            ln = int(m.group(1))
            # a `#line` back to the C file itself (after a function) is not
            # a source line: those name a .c file
            if not line.rstrip().endswith('.c"'):
                out.append(f"    __fcov[{n}] = 1;")
                mp.append(f"{n} {cur} L{ln}")
                n += 1
                last_line = ln
            continue
        if line.startswith("}"):
            cur = None
        out.append(line)
    return out, mp, n


def prelude(n):
    return f"""
/* === block and line coverage (scripts/nat-cov-instrument.py) === */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
static unsigned char __fcov[{max(n, 1)}];
static void __fcov_dump(void) {{
    static int done = 0;
    if (done) return;
    done = 1;
    const char* path = getenv("FID_COV_OUT");
    char buf[1024];
    if (!path) {{
        const char* dir = getenv("FID_COV_DIR");
        if (!dir) return;
        static int seq = 0;
        snprintf(buf, sizeof buf, "%s/%ld-%d-%d.cov", dir, (long)rand(), (int)clock(), seq++);
        path = buf;
    }}
    FILE* f = fopen(path, "wb");
    if (!f) return;
    static unsigned char bits[{(max(n, 1) + 7) // 8}];
    for (int i = 0; i < {n}; i++) if (__fcov[i]) bits[i >> 3] |= (unsigned char)(1u << (i & 7));
    fwrite(bits, 1, sizeof bits, f);
    fclose(f);
}}
__attribute__((constructor)) static void __fcov_init(void) {{
    srand((unsigned)time(0) ^ (unsigned)clock() ^ (unsigned)(size_t)&__fcov_init);
    atexit(__fcov_dump);
}}
"""


def self_test():
    src = [
        "#include <stdio.h>",
        "int64_t f(int64_t x) {",
        "  bb0:",
        "    /*@L10*/",
        "    int64_t v1 = x + 1;",
        "    /*@L11*/",
        "    if (v1) goto bb1; else goto bb2;",
        "  bb1:",
        '#line 12 "compiler/x.fd"',
        "    return v1;",
        '#line 40 "out.c"',
        "  bb2:",
        "    return 0;",
        "}",
        "/*@L99*/",
        "int main(int argc, char** argv) {",
        "    return (int)f(argc);",
        "}",
    ]
    out, mp, n = instrument(src)
    want = ["0 f entry", "1 f bb0", "2 f L10", "3 f L11", "4 f bb1", "5 f L12", "6 f bb2", "7 main entry"]
    assert mp == want, mp
    assert n == 8, n
    # a counter right after each label and mark, none for the mark outside
    # a function or the #line back to the C file
    i = out.index("    /*@L10*/")
    assert out[i + 1] == "    __fcov[2] = 1;", out[i + 1]
    i = out.index("  bb1:")
    assert out[i + 1] == "    __fcov[4] = 1;", out[i + 1]
    i = out.index('#line 40 "out.c"')
    assert out[i + 1] == "  bb2:", out[i + 1]
    assert out.count("/*@L99*/") == 1 and "__fcov[8]" not in "\n".join(out)
    print("nat-cov-instrument self-test: ok")


def main():
    if "--self-test" in sys.argv:
        self_test()
        return
    src, dst = sys.argv[1], sys.argv[2]
    with open(src, encoding="utf-8", errors="surrogateescape", newline="") as f:
        lines = f.read().split("\n")
    out, mp, n = instrument(lines)
    text = prelude(n) + "\n".join(out)
    with open(dst, "w", encoding="utf-8", errors="surrogateescape", newline="") as f:
        f.write(text)
    with open(dst + ".map", "w") as f:
        f.write("\n".join(mp) + "\n")
    nl = sum(1 for x in mp if x.split()[-1].startswith("L"))
    nf = sum(1 for x in mp if x.endswith(" entry"))
    print(f"{n} counters ({nf} functions, {n - nf - nl} blocks, {nl} line marks)")
    if nl == 0:
        print("note: no line marks in the C (FID_C_LINE_MARKS=1 makes the C generator write them): blocks only")


if __name__ == "__main__":
    main()
