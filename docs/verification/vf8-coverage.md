# VF8: line and branch coverage of compiler/*.fd

Roadmap: docs/roadmap/competitive-roadmap.md, VF8 (line and branch
coverage of `compiler/*.fd` over 90%, reported in CI).

## How it is measured

`scripts/verify/vf8-run.sh` does the whole run:

1. **The counter compiler.** `scripts/verify/build-variant.sh` builds the
   tree's compiler through C with `FID_C_LINE_MARKS=1`, so the C generator
   writes `/*@L<n>*/` before the C of each instruction on a new source line
   and at the start of each block (the line is the IR's
   `ValueMeta.source_line`: the line of the statement the instruction was
   lowered from). `scripts/verify/cov_instrument.py` puts a counter at every
   function entry, every IR block label (`bbN:`) and every line mark. It
   also reads `-g`'s `#line <n> "<file>.fd"` directives (the counter goes
   before the directive, so the C keeps its line mapping), for a compiler
   whose `dump-c` takes `-g` (`DUMP_C_FLAGS=-g`).
2. **The runs.** `scripts/verify/coverage.py` compiles each test of the
   suite (tests/bootstrap, its subfolders, tests/conformance) and the
   fuzzers' programs in each mode: `check`, `run` (the interpreter),
   `ccompile`, `emit` (pe_ir on Windows), `emit --nat` and `emit --target`
   for x86_64, aarch64 and riscv64 Linux (only the compiler runs; no target
   binary is executed). The counter compiler writes the counters it reached
   at exit; the bitmaps are merged.
3. **The figures**, per module (`compiler/*.fd` that main.fd reaches, and
   `stdlib/core.fd`):
   - *blocks*: IR basic blocks entered / blocks. Every branch target is a
     block, so this is branch coverage at the IR level.
   - *functions*: functions entered / functions in the C.
   - *lines*: statement lines that ran / statement lines with a counter,
     plus the source lines of the module's functions that have no C at all
     (never lowered because nothing reaches them), counted as not run. A
     line is reached when any of its counters ran (a line spanning blocks,
     or a generic function's copies, has several).
   `uncovered-lines.txt` lists every line no test reached as
   `module:line`, `never-called.txt` every function never entered, and
   `low-modules.txt` the modules below the goal, worst first.
4. **The goal**: `coverage.py --goal 90 --goal-lines 90 --goal-modules all`
   exits 1 when a module of the map is below 90% of its blocks or lines.

Without line marks in the C (a compiler whose C generator lacks
`FID_C_LINE_MARKS`), the line table falls back to the Nov 16 estimate
(lines of functions entered, and a block-weighted figure) and says so.

`tests/tooling/vf8-coverage-check.sh` checks the measuring itself on
`tests/bootstrap/test_mutation_kill.fd` without a compiler build: the
instrumented program prints its expected output, the line of `if x != x`
(never true) is uncovered and the one of `if x == x` is covered, and the
goals exit as they should. Nov 18 with the Nov 16 stage 0: checks 1 and
3–5 pass (the `#line` form, through `ccompile -g`); check 2 fails because
neither `FID_C_LINE_MARKS` (compiler/codegen_c.fd) nor `dump-c -g`
(compiler/main.fd) is in that compiler.

## Runs

None recorded yet: the first needs the tree's compiler, built once at the
end of the Nov 18 run, and the codegen_c.fd line-mark hunk. Record one with

    FID=release/fid.exe J=4 bash scripts/verify/vf8-run.sh

(`LIMIT=50 GEN=0` for a short one; `SKIP_BUILD=1` reuses the counter
compiler and adds to its bitmap). Each run appends its report below.
