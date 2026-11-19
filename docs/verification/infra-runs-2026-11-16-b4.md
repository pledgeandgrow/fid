# Infra runs, Nov 16 (batch 4: B8, A4, F5, F6, G4)

Compiler used: a one-stage `fid` built Nov 15 23:01 (before the Nov 15
`--inc` fixes in compiler/main.fd and compiler/compiler.fd were built).

## Every CI build stops at stage 1 (A4, F5, G4)

The last pushed runs (Nov 15, commit "optimisation + security fixes") of
ci.yml, macos.yml, macos-native.yml, bench.yml and bare-metal.yml all fail in
the first `fidesc dump-c compiler/main.fd` with

    error: Parse error at 2183:31: unexpected token 'Eq'

which is `guard = guard + 1;` in compiler/comptime_eval.fd. `guard` is a
keyword of the frozen Rust compiler (src/lexer.rs); fidesc takes such a word
as a parameter, a `var` name or an operand, but not at the start of a
statement. The working tree has six such statements:

    compiler/comptime_eval.fd:2851  compiler/egraph.fd:1709
    compiler/ssa.fd:195             compiler/ssa.fd:532
    stdlib/linux_runtime/sys.fd:756 stdlib/yaml.fd:877

`scripts/stage0-names-check.sh` finds them without building (ci.yml `gates`
job, wired in scripts/ci-wiring-check.sh; `PROBE=1` rederives the word lists
from fidesc). Renaming the variables (e.g. `guard` -> `iters`) is needed
before any CI job, the weekly `suite-fid` (A4), macos.yml (F5) or fides-os's
build.yml (G4), can get past stage 1. Per-file `fidesc dump-ast` shows other
errors (nat_ir.fd, typechecker.fd, pe_ir.fd, ir_ser.fd, mono.fd), but those
are struct literals used before their struct is declared, which a
whole-program parse (struct names collected first) accepts; whether stage 1
then passes is checked by the next CI run.

## F6: overflow and division semantics through C on every cross target

`scripts/cross-test.sh <target>` on `test_int_semantics`,
`test_panic_overflow_fail`, `test_panic_div_zero_fail`,
`test_panic_min_div_fail`:

| target | result |
|---|---|
| aarch64-linux (qemu-aarch64, WSL) | 4/4 pass |
| riscv64-linux (qemu-riscv64, WSL) | 4/4 pass |
| x86_64-linux (WSL) | 4/4 pass |
| wasm32-wasi (Node) | does not build: `SO_ERROR` undeclared (p_co_connect in the C runtime); with `#define SO_ERROR 4` in the `__wasi__` block: 4/4 pass |
| x86_64-macos, aarch64-macos | build (Mach-O); not run, no Mac |

Two script bugs fixed on the way: cross-test.sh wrote `run-all.sh` with
paths relative to the root even for an absolute `OUT` (and gave WSL
`/mnt/c/<root>//c/...`), and its panic comparison kept the interpreter's
" at line N" (`Panic: Division by zero at line 6` against compiled
`Panic: Division by zero`), so the two division tests failed on every
target. wasm-test.sh had the same comparison.

## B8: `--inc` across the suite

`SUITE=1 scripts/inc-check.sh` could not run outside the tree: the fixture
builds in a temporary directory where fid finds no stdlib (now
`FID_HOME=<root>` is exported). With the Nov 15 23:01 fid every `--inc`
build of a test fails at C compilation: the prelude's `P_STR_DUP` /
`P_STR_DROP` macros call `Str_dup` / `Str_drop`, whose prototypes a unit
gets only when its function bodies name them (compiler.fd's per-unit proto
selection reads the bodies, not the shared prelude). Adding the two
prototypes to the unit makes it compile. The fixture's duplicate symbol
(`one` in lib1.o and lib2.o) is the canonical-hash object reuse that the
source already fixes (`inc_unit_hash`, "o2" key); that fid predates it. The
suite comparison is left for the end-of-run build (853 top-level tests,
about 14 s each both ways).

## G4: fides-os

With the same fid, fides-os's kernels build (`emit --boot --load
0x1000000`) and boot under QEMU (WSL): minimal.fd prints "Hello from Fides
OS!", shell.fd reaches its prompt. fides-os's `.github/` and
`scripts/qemu-boot-test.sh` are untracked in that repository, so its CI has
never run (`gh run list` is empty); once pushed it stops at stage 1 like the
others until the `guard` statements are renamed.

## A4

`scripts/suite-fid-compare.sh` compares the weekly `suite-fid` job's pass
counts with tests/bootstrap/baseline/test_results.txt per backend (normal
and expected-fail) and fails when fid fails more tests than the baseline;
verify-weekly.yml runs it and puts the table in the job summary. The job
has not run yet (the workflow is not on the default branch: `gh` reports
verify-weekly.yml not found).
