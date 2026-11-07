#!/usr/bin/env python3
"""Add block coverage counters to the compiler's generated C (roadmap VF8).

    python scripts/verify/cov_instrument.py <in.c> <out.c>

The C that `fid dump-c compiler/main.fd` writes has one C function per Fides
function, and one label `bbN:` per basic block of the IR. Every function
entry and every block label gets `__fcov[ID] = 1;`, so each branch target
is counted: block coverage is branch coverage at the IR level. (There is no
source-line information in the generated C, so this is not line coverage.)

<out.c>.map gets one line per counter: `ID function block`.

At exit the instrumented compiler writes the counters it hit as a bitmap:
to $FID_COV_OUT if set (one file, overwritten: the fuzzer of VF3 reads it
after each run), else into $FID_COV_DIR as <pid>-<n>.cov (the coverage
runner merges them). Neither set: nothing is written.
"""
import re
import sys

FUNC = re.compile(r"^[A-Za-z_][A-Za-z0-9_ \*]*?\b([A-Za-z_][A-Za-z0-9_]*)\(([^;{}]*)\) \{$")
LABEL = re.compile(r"^(\s*)(bb\d+):\s*$")


def main():
    src, dst = sys.argv[1], sys.argv[2]
    out = []
    mp = []
    cur = None
    n = 0
    with open(src, encoding="utf-8", errors="surrogateescape", newline="") as f:
        lines = f.read().split("\n")
    for line in lines:
        m = FUNC.match(line)
        if m and not line.startswith(("static inline", "typedef")):
            cur = m.group(1)
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
            continue
        if line.startswith("}"):
            cur = None
        out.append(line)
    pre = f"""
/* === block coverage (scripts/verify/cov_instrument.py) === */
#include <stdio.h>
#include <stdlib.h>
static unsigned char __fcov[{n}];
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
    unsigned char bits[{(n + 7) // 8}] = {{0}};
    for (int i = 0; i < {n}; i++) if (__fcov[i]) bits[i >> 3] |= (unsigned char)(1u << (i & 7));
    fwrite(bits, 1, sizeof bits, f);
    fclose(f);
}}
#include <time.h>
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
    print(f"{n} counters ({sum(1 for x in mp if x.endswith(' entry'))} functions)")


if __name__ == "__main__":
    main()
