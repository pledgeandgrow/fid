#!/usr/bin/env python3
"""Add block coverage counters to the compiler's generated C (roadmap VF8).

    python scripts/verify/cov_instrument.py <in.c> <out.c>
    python scripts/verify/cov_instrument.py --self-test

The C that `fid dump-c compiler/main.fd` writes has one C function per Fides
function, and one label `bbN:` per basic block of the IR. Every function
entry and every block label gets `__fcov[ID] = 1;`, so each branch target
is counted: block coverage is branch coverage at the IR level.

Line coverage (VF8, Nov 18): the IR knows each instruction's source line
(ValueMeta.source_line, the line of the statement it was lowered from). The
C carries it in one of two forms, and each gets a counter of its own:
  `/*@L<n>*/`          FID_C_LINE_MARKS=1 (codegen_c.fd, RL12): a comment
                       before the C of each instruction on a new line, and
                       again at the start of every block;
  `#line <n> "x.fd"`   `-g` (TL5): the same directive a debugger reads. The
                       counter goes just before the directive, so the
                       directives still map every C line to its source line.
                       A `#line` naming a .c file (back to the C itself) is
                       not a source line. -g writes a directive only when the
                       line changes, also across a block label, so a block
                       that starts on the line in effect gets a counter for
                       that line too.
A source line is reached when any of its counters ran. The line is the
function's own .fd file's (the C path runs no inliner).

<out.c>.map gets one line per counter: `ID function kind`, kind being
`entry`, `bbN` (an IR block) or `L<n>` (source line n). The same format as
scripts/nat-cov-instrument.py (RL12); scripts/verify/coverage.py and
scripts/nat-coverage.py read either.

At exit the instrumented compiler writes the counters it hit as a bitmap:
to $FID_COV_OUT if set (one file, overwritten: the fuzzer of VF3 reads it
after each run), else into $FID_COV_DIR as <pid>-<n>.cov (the coverage
runner merges them). Neither set: nothing is written.
"""
import re
import sys

FUNC = re.compile(r"^[A-Za-z_][A-Za-z0-9_ \*]*?\b([A-Za-z_][A-Za-z0-9_]*)\(([^;{}]*)\) \{$")
LABEL = re.compile(r"^(\s*)(bb\d+):\s*$")
MARK = re.compile(r"^(\s*)/\*@L(\d+)\*/\s*$")
HASHLINE = re.compile(r'^#line (\d+) "(.*)"\s*$')


def is_line_start(line):
    """A mark or a `#line` to a .fd file: the next C is that line's."""
    if MARK.match(line):
        return True
    m = HASHLINE.match(line)
    return bool(m and m.group(2).endswith(".fd"))


def instrument(lines):
    """(instrumented lines, map lines, number of counters)."""
    out = []
    mp = []
    cur = None
    n = 0
    in_effect = None  # the source line the C being read belongs to
    for k, line in enumerate(lines):
        m = FUNC.match(line)
        if m and not line.startswith(("static inline", "typedef")):
            cur = m.group(1)
            in_effect = None
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
            # the block's first C is still the line in effect unless a new
            # line starts right here
            nxt = next((x for x in lines[k + 1:k + 4] if x.strip()), "")
            if in_effect is not None and not is_line_start(nxt):
                out.append(f"{m.group(1)}  __fcov[{n}] = 1;")
                mp.append(f"{n} {cur} L{in_effect}")
                n += 1
            continue
        m = MARK.match(line)
        if m and cur:
            ln = int(m.group(2))
            out.append(line)
            out.append(f"{m.group(1)}__fcov[{n}] = 1;")
            mp.append(f"{n} {cur} L{ln}")
            n += 1
            # marks start again at every block (codegen_c.fd), so a block
            # without one has no line: nothing carries over its label
            in_effect = None
            continue
        m = HASHLINE.match(line)
        if m and cur:
            if m.group(2).endswith(".fd"):
                ln = int(m.group(1))
                # before the directive: the C after it keeps its line
                out.append(f"    __fcov[{n}] = 1;")
                mp.append(f"{n} {cur} L{ln}")
                n += 1
                in_effect = ln
            else:
                in_effect = None
            out.append(line)
            continue
        if line.startswith("}"):
            cur = None
            in_effect = None
        out.append(line)
    return out, mp, n


def self_test():
    """Counters for blocks, marks and #line directives on a made-up C file."""
    src = [
        "#include <stdio.h>",
        "int64_t f(int64_t x) {",
        "    int64_t v1 = 0;",
        "    /*@L10*/",
        "    v1 = x + 1;",
        "    if (v1) goto bb1;",
        "    else goto bb2;",
        "  bb1:",
        "    /*@L11*/",
        "    return v1;",
        "  bb2:",
        "    return 0;",
        "}",
        "/*@L99*/",
        "int64_t g(int64_t x) {",
        '#line 20 "compiler/x.fd"',
        "    if (x) goto bb1;",
        "    else goto bb2;",
        "  bb1:",
        '#line 21 "compiler/x.fd"',
        "    x = x + 1;",
        "    goto bb2;",
        "  bb2:",
        "    return x;",
        '#line 900 "C:/build/out.c"',
        "    return 0;",
        "}",
        "int main(int argc, char** argv) {",
        "  bb1:",
        "    return (int)f(argc) + (int)g(argc);",
        "}",
    ]
    out, mp, n = instrument(src)
    # f (marks): bb2 has no mark, so no line; g (#line): bb2 has no
    # directive, so its C is still line 21's
    want = ["0 f entry", "1 f L10", "2 f bb1", "3 f L11", "4 f bb2",
            "5 g entry", "6 g L20", "7 g bb1", "8 g L21", "9 g bb2", "10 g L21",
            "11 main entry", "12 main bb1"]
    assert mp == want, mp
    assert n == len(want), n
    # a counter right after a mark, right before a #line to a .fd file, none
    # for the #line back to the C file or the mark outside a function
    i = out.index("    /*@L10*/")
    assert out[i + 1] == "    __fcov[1] = 1;", out[i + 1]
    i = out.index('#line 20 "compiler/x.fd"')
    assert out[i - 1] == "    __fcov[6] = 1;", out[i - 1]
    i = out.index('#line 900 "C:/build/out.c"')
    assert out[i - 1] == "    return x;", out[i - 1]
    assert out.count("/*@L99*/") == 1
    # bb1 of g starts a new line: one counter (the block's), then L21's
    i = out.index("  bb1:", out.index("int64_t g(int64_t x) {"))
    assert out[i + 1] == "    __fcov[7] = 1;" and out[i + 2] == "    __fcov[8] = 1;", out[i:i + 4]
    print("cov_instrument self-test: ok")


def main():
    if "--self-test" in sys.argv:
        self_test()
        return
    src, dst = sys.argv[1], sys.argv[2]
    with open(src, encoding="utf-8", errors="surrogateescape", newline="") as f:
        lines = f.read().split("\n")
    out, mp, n = instrument(lines)
    pre = f"""
/* === block and line coverage (scripts/verify/cov_instrument.py) === */
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
    /* static: the compiler has millions of counters, too many bits for
       the stack of whichever thread runs the atexit handlers */
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
    # declarations first: everything above the first function body may use
    # the counters only after they are declared, so put them at the very top
    text = pre + "\n".join(out)
    with open(dst, "w", encoding="utf-8", errors="surrogateescape", newline="") as f:
        f.write(text)
    with open(dst + ".map", "w") as f:
        f.write("\n".join(mp) + "\n")
    nf = sum(1 for x in mp if x.endswith(" entry"))
    nl = sum(1 for x in mp if x.split()[-1].startswith("L"))
    print(f"{n} counters ({nf} functions, {n - nf - nl} blocks, {nl} source lines)")
    if nl == 0:
        print("note: the C has no source lines (FID_C_LINE_MARKS=1 or `-g` writes them): block coverage only")


if __name__ == "__main__":
    main()
