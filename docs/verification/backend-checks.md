# Checks of the own backend: images, mutants, coverage

Own-backend roadmap (docs/roadmap/llvm-roadmap.md) RL9, RL11 and RL12.
Each section says what the check is, how to run it, and what it has
shown so far. Nothing here was run with the compiler of Nov 16 (the tree
had not been built when this was written); the runs listed were made
with the stage-0 compiler, which has none of the switches below.

## RL9: every image through the system's readers and our own

`scripts/image-check.sh [test.fd ...]` builds each program with every
writer and runs every reader that is installed on each image; a reader
that exits non-zero or reports a warning or an error fails the image.

| format | built by | readers |
|---|---|---|
| ELF executables (x86-64, AArch64, RISC-V) | `emit --target <arch>-linux` | readelf, llvm-readobj, llvm-objdump, llvm-dwarfdump --verify, image-lint |
| ELF objects (x86-64, AArch64) | `emit --obj --target <arch>-linux` | readelf, llvm-readobj, llvm-objdump -dr, image-lint |
| PE32+ (pe_ir, the machine layer) | `emit`, `emit --nat` | llvm-readobj, llvm-objdump, dumpbin, objdump -x, image-lint |
| UEFI application | `emit test_uefi_hello.fd --uefi` | the PE readers and tests/uefi/check_efi.py |
| COFF object | `codegen --coff` | llvm-readobj, llvm-objdump, dumpbin, objdump -x, image-lint |
| Mach-O (both macOS targets) | `emit --target <arch>-macos` | llvm-readobj, llvm-objdump, otool -hlv (macOS), macho-check.py |

`scripts/image-lint.py` is our own reader, run on every ELF, PE and COFF
image whatever is installed (its header lists what it checks);
`--self-test` builds a small ELF and PE in memory and finds 16
corruptions of them. `READERS_ONLY=1 scripts/image-check.sh` lists the
readers it would use. Running the images is cross-test.sh's part.

Test: `tests/tooling/image-lint-check.sh` (self-test, our images clean,
corrupted copies found, an image-check run).

Findings with the stage-0 compiler (Nov 16, `hello.fd`; readers: readelf
in WSL, dumpbin, objdump with PE support, the three own checkers):

- every ELF executable: readelf "Error: no .dynamic section in the
  dynamic segment"; image-lint reports the same (PT_DYNAMIC without an
  SHT_DYNAMIC section). The writer (compiler/nat_enc.fd
  `nat_elf_debug_sections`) writes no section header for the dynamic
  segment.
- the COFF object of `codegen --coff`: every symbol with a long name has
  string-table offset 0 (no name). compiler/coff.fd
  `symbol_name_map_lookup` finds none of the names it recorded.
- PE (both writers), ELF objects and Mach-O: clean.

Both bugs were fixed by TG13 (Nov 17): `nat_elf_debug_sections`
(compiler/nat_enc.fd) now writes `.interp`, `.dynstr` and a SHT_DYNAMIC
`.dynamic` section at PT_DYNAMIC's address and offset, and compiler/coff.fd
writes its symbols through image_writer.fd `img_coff_symbols`, which gives
each long name its string-table offset. `tests/nattools/rl9_writer_fixes_check.sh`
checks both on real programs built by the compiler under test (dynamically
linked ELF executables of the three Linux targets; a COFF object of
`codegen --coff` with long names); docs/verification/rl9-rl25-checks-2026-11-17.md.

Nov 21 check (the tree built from a self-hosted stage 0, llvm-roadmap
RL9): the ELF images of the three Linux targets carry `.dynamic` at
PT_DYNAMIC and pass image-lint and readelf; `image-lint-check.sh` 0
failed; `image-check.sh` on hello 22 passed, 1 failed: the COFF object of
`codegen --coff` does not build (LoadLibraryA, GetProcAddress, FreeLibrary
undefined). Not run with llvm-readobj, llvm-objdump or llvm-dwarfdump
installed.

## RL11: a broken rule or pass must fail the suite

Mutants are run-time switches of the compiler under test,
`FID_NAT_MUTATE=<mutant>` (part of nat_ir's cache key; never set in a
real build), so no compiler is rebuilt per mutant:

- `rule<N>`: rule N of compiler/rules.fd (in `fid test-rules` order)
  gives a wrong result where it applies (`x + 0 = 0`, `x * 0 = x`,
  `x == x` false); `rulec<N>`: an `x op c` rule matches the wrong constant
  (`x + 1 = x`; the next constant no earlier rule takes). `fid test-rules`
  reports the broken rule as failed.
- `egcomm`, `egflip`, `egfold`, `egident`, `egzero`, `egdouble`, `egsame`:
  a rule family of compiler/egraph.fd broken (eg_mutate_mask).
- every `ng_mutant("<name>")` of compiler/nat_ir.fd (selection rules,
  encodings, machine passes; TG2).

`scripts/mutation-check.sh` builds the suite once without a mutant, then
with each mutant (the killer tests first: tests/bootstrap/
test_mutation_kill.fd has an operation for every rule, on values only
known at run time); an image identical to the base one is not run. A
mutant is killed (a test's output or exit status changed, or its build
failed), SURVIVED (images changed, no test noticed) or inert (no image
changed on this target). Exit 1 when one survived (STRICT=1: or was
inert). TESTS=quick runs the killers alone; SAMPLE=N adds N other tests.
The whole suite costs one build per test per surviving mutant: hours.
Source-edit mutants (each a compiler build) stay in
`scripts/verify/variants.py mutate`.

Test: `tests/tooling/mutation-switch-check.sh`.

Run with the stage-0 compiler (no rules/e-graph switches yet), target
`nat`, test_mutation_kill.fd only: `isel` killed (exit 127), `ldop`
killed (exit 139), `tbz` survived (an AArch64 rule: the x86-64 image
changed, the output did not).

## RL12: line and branch coverage of the machine layer

`scripts/nat-coverage.py --base-fid release/fid.exe [--gen N] [-j 4]`:

1. builds a counter compiler from the tree (scripts/verify/
   build-variant.sh with scripts/nat-cov-instrument.py over its C, and
   `FID_C_LINE_MARKS=1` so the C generator writes `/*@L<n>*/` before the
   C of each instruction on a new source line); each function entry,
   IR block and marked line gets a counter;
2. compiles every test with it for x86_64-linux, aarch64-linux,
   riscv64-linux and `--nat` (the compiler runs, the images do not);
   then the variants (`--variants all`, the default; `none` leaves them
   out): samples of the tests (`--variant-tests 40` for the main
   switches, `--off-tests 6` for the others; the machine layer's own
   tests, `test_verify_nat_*`, `test_natabi_*`, `test_native_*`,
   `test_verify_rl12_*` ..., are always in a sample) built with the
   switches a plain build never sets: `FID_VERIFY=1` (the IR verifier
   and the allocation checker), `FID_NAT_PAR=4`, `FID_NAT_FAST=1`,
   `--debug`, `-g`, `FID_NAT_FRAMES=1`, `--stats --remarks`,
   `FID_NAT_DUMP=*`, `--obj`, the function cache (`FID_FNCACHE`, built
   twice: filled, then read), `FID_RULES_ON=readcse` (since Nov 17 a
   no-op: readcse is on by default and no rule reads FID_RULES_ON),
   `FID_IROPT_ON=all`,
   the rule and e-graph statistics, each `FID_NAT_ON` opt-in pass, the
   CPU levels (`--cpu x86-64-v3/v4`, `armv8.1-a`, `armv9-a`,
   `rv64gc_zba_zbb`, `rv64gcv`, `rv64g`, `rva23u64`), the macOS targets
   and `--target universal` (the Mach-O writer), `dump-mir`/`disasm`, and
   each `FID_NAT_OFF` pass of kill-switches.md (the fallbacks); `fid
   test-rules` and `fid test-rules --smt <dir>` run once (rules.fd's
   evaluator and SMT writer). About 3,900 variant builds besides the
   plain ones;
3. reports per module (default the four modules of the goal:
   compiler/nat_ir.fd, nat_enc.fd, rules.fd, egraph.fd):
   lines run / lines with a counter (plus the lines of functions that
   have no C, counted as not run), IR blocks entered (branch targets:
   branch coverage at the IR level), functions entered. `--goal 90`
   fails below 90%; `--record <file.md>` appends a row;
   `uncovered-<module>.txt` lists the lines never run and
   `gaps-<module>.txt` the functions with lines or blocks not reached,
   the largest gap first (where the next test or fuzzer program aims).

Not countable from the counter compiler: the `*_unit` functions of
nat_ir.fd/nat_enc.fd (`nat_tg78_unit_check`, `nat_split_unit`, ...,
about 750 lines) run inside the tests/nattools/*_unit.fd programs, not
inside the compiler, so they count as lines with no C (not run).

Without the line marks (a compiler whose C generator lacks
FID_C_LINE_MARKS) the line figure is scripts/verify/coverage.py's
block-weighted estimate and is labelled "est.". (Nov 18: the
FID_C_LINE_MARKS hunk of compiler/codegen_c.fd was not in the tree yet,
and `dump-c` takes no `-g`, so `#line` directives were no way round it;
on Nov 21 the hunk is in, `cg_line_marks_on`.)

Test: `tests/tooling/nat-coverage-check.sh` (on a small program: marks,
counters, a line known uncovered and one known covered; BUILD=1 runs the
whole thing on a few tests).

First number: not measured yet (it needs the tree's compiler, built
once at the end of this run). Record it with
`python3 scripts/nat-coverage.py --base-fid release/fid.exe -j 4 --record docs/verification/backend-checks.md`.
