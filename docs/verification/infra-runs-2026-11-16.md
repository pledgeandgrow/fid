# Recorded check runs, 2026-11-16 (RL4, RL9, RL22, SP20)

Each run used the agents' one-stage `fid` of Nov 15 23:01 (`fidh1.exe`, not
a release bootstrap) on Windows 11 x64, with WSL (kali-linux) for the Linux
readers and executables and `zig cc` 0.16 as the C compiler. None of these is
the release run its roadmap entry asks for; each says what is left.
SP14's run is in docs/perf/sp14-egraph-bound.md.

## RL4: miscompile gate

`scripts/miscompile-gate.sh` now:

- checks the rows of "Earlier miscompiles" that name a test (the test must
  exist) and runs them with `--run`; `--before` skips them (no recent
  compiler has those bugs);
- `--new-since <rev>` limits `--run`/`--before` to the entries fixed now and
  not fixed at `<rev>`; with none, there is nothing to run;
- `MISCOMPILE_LOG=<file>` reads a copy of the log (to test the gate).

CI: .github/workflows/bench.yml, which already builds the base commit's fid
and head's on one runner, runs `miscompile-gate.sh --fid <head> --run
--before <base> --new-since <base sha>`, so the before-step runs on its own
for every change that marks an entry fixed. scripts/ci-wiring-check.sh keeps
that step wired.

Ten tests for the ten entries fixed before the log, each passing on `run`,
`ccompile` and `emit`:

| Entry | Test |
|---|---|
| AGENTS 4 | `tests/bootstrap/test_verify_legacy_vec_mut_arg.fd` |
| AGENTS 9 / known-bugs 1.6 | `tests/bootstrap/test_verify_legacy_enum_field_offset.fd` |
| AGENTS 10 / 1.7 | `tests/bootstrap/test_verify_legacy_native_values.fd` |
| AGENTS 12 | `tests/bootstrap/test_verify_legacy_zero_fields.fd` |
| AGENTS 13 | `tests/bootstrap/test_verify_legacy_first_slot_float.fd` |
| AGENTS 21 | `tests/bootstrap/test_verify_legacy_plain_str_free.fd` |
| known-bugs 1.11 | `tests/bootstrap/test_verify_legacy_enum_eq.fd` |
| known-bugs 1.18 | `tests/bootstrap/test_verify_legacy_print_types.fd` |
| known-bugs 1.19 | `tests/bootstrap/test_verify_legacy_long_function.fd` |
| known-bugs 1.30 | `tests/bootstrap/test_verify_legacy_float_consts.fd` |

Run, with a copy of the log that names these tests in the rows of
"Earlier miscompiles" (`MISCOMPILE_LOG=<copy> miscompile-gate.sh --fid
fidh1.exe --run`): "5 fixed entries checked, 0 without a test; 10 earlier
entries (before the log), 10 of them name a test", the 14 tests "14 of 14
files pass on run,ccompile,emit", the open repros 3 of 16 (informational),
"miscompile gate: ok". Left: the log itself names the tests (it is not this
group's file), and the CI step has not run yet on a change that fixes an
entry.

## RL9: image-check.sh

`scripts/image-check.sh` on Windows now finds dumpbin in Visual Studio's
MSVC tools without a developer prompt (and passes it `-nologo -headers
-imports`: Git Bash rewrote `/nologo` into a path, which made every dumpbin
check fail), and runs readelf, objdump and llvm-* in WSL when they are not
on PATH. `READERS_ONLY=1` lists the readers it would use.

Readers this host has: readelf (WSL), objdump with pei-x86-64 (WSL), dumpbin
(Visual Studio 18, MSVC 14.51), scripts/macho-check.py. Not installed:
llvm-readobj, llvm-objdump, llvm-dwarfdump.

Run on the default programs (hello, test_closure, test_hashmap, test_float,
test_tasks), every writer (ELF for x86_64/aarch64/riscv64-linux, PE from
pe_ir and `--nat`, Mach-O for both macOS targets): 45 checks passed, 0
failed (readelf 15 ELF images, dumpbin and objdump 10 PE images each,
macho-check 10 Mach-O images). Left: the same run with the LLVM readers
installed.

## RL22: AGREE=1 suite-all.sh

`scripts/suite-all.sh` takes `TESTS="a.fd b.fd"` (a smoke run on every
lane; the release check is still the whole suite).
`scripts/cross-test.sh` no longer exports `OS` (now `TOS`): on Windows it
overwrote `OS=Windows_NT`, which fid reads, so every pe_ir build of the
`native:x86_64-windows-pe` lane failed ("C compilation failed").

Smoke run, `AGREE=1 JOBS=1`, on hello, test_hashmap,
test_verify_legacy_enum_eq and test_verify_legacy_float_consts: all ten
lanes this host runs. c and native x86_64/aarch64/riscv64-linux (WSL with
qemu-user), c:x86_64-windows, native:x86_64-windows-pe and
native:x86_64-windows: 4 of 4 each. c:wasm32-wasi: no test builds — the C
runtime calls `getsockopt(..., SO_ERROR, ...)`, which WASI's headers do not
define (compiler/codegen_c.fd, the non-blocking connect). Left: that fix,
and the whole suite with `AGREE=1` on the release compiler.

## SP20: repro-check.sh

`scripts/repro-check.sh` now runs the x86-64 Linux executables in WSL on
Windows (so a fn-cache hit is checked to behave the same), says when no
artifact was written (the compare is then between two cache-off builds),
and `STRICT=1` fails on that and on a fn-cache hit that differs.

Run on hello, test_hashmap, test_tooling_explain (`emit --target
x86_64-linux`): the executables are identical with the artifact off, on a
miss and on a hit, and with FID_FNCACHE on a miss. On a fn-cache hit the
executable differs (hello: 47,896 bytes against 54,712) and runs the same
(output and exit 0, hello and test_hashmap). This compiler writes no
artifact for `dump-c` (the SP4 branch that keeps core alone for `dump-c` is
newer than it), so the C compares were vacuous. Left: the run with the
end-of-run build, and fn-cache hits that produce the same image
(compiler/nat_ir.fd, CS6).

## Batch logs

The eight batch reports of Nov 16 (b4–b13; some batch numbers had no run):

## Infra runs, Nov 16 (batch 4: B8, A4, F5, F6, G4)

Compiler used: a one-stage `fid` built Nov 15 23:01 (before the Nov 15
`--inc` fixes in compiler/main.fd and compiler/compiler.fd were built).

### Every CI build stops at stage 1 (A4, F5, G4)

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

### F6: overflow and division semantics through C on every cross target

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

### B8: `--inc` across the suite

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

### G4: fides-os

With the same fid, fides-os's kernels build (`emit --boot --load
0x1000000`) and boot under QEMU (WSL): minimal.fd prints "Hello from Fides
OS!", shell.fd reaches its prompt. fides-os's `.github/` and
`scripts/qemu-boot-test.sh` are untracked in that repository, so its CI has
never run (`gh run list` is empty); once pushed it stops at stage 1 like the
others until the `guard` statements are renamed.

### A4

`scripts/suite-fid-compare.sh` compares the weekly `suite-fid` job's pass
counts with tests/bootstrap/baseline/test_results.txt per backend (normal
and expected-fail) and fails when fid fails more tests than the baseline;
verify-weekly.yml runs it and puts the table in the job summary. The job
has not run yet (the workflow is not on the default branch: `gh` reports
verify-weekly.yml not found).

## Infra runs, Nov 16 (batch 5): N2, N4, N11, M8, I6

The CI scripts of these goals, run on Windows 11 with a fid built on
Nov 15 at 23:01 (older than the tree: it has the ark.fd of HEAD), CC=zig cc.

### N2 `scripts/quickstart-check.sh`: PASS

All 6 steps of docs/reference/quickstart.md ran and printed what the guide
says (ccompile, the built hello, emit, run, the Vec example), 27 s. The
compiler must find stdlib/ next to it or above it (`release/fid-dev` in CI
does), or through FID_HOME.

### N11 `scripts/projects-check.sh`: 4 of 5 pass

- cli, game (run and ccompile), cbind: PASS.
- service: failed to type-check. Its request function was named `handle`,
  which is the keyword of an effect handler: `handle(line, ...)` parsed as a
  handler expression, typed as the function itself ("cannot assign
  fn(str, &mut Vec<str>, ...) -> str to variable 'reply' of type str").
  Renamed `reply_to` in tests/projects/service/main.fd: PASS (lint clean,
  exit status 1, output = expected.txt). The parser accepting `fn handle`
  as a name is a compiler bug (needs a "keyword" error).
- library (scripts/ark-check.sh): `ark install` of a path pact that is a
  directory fails with that compiler ("Path pact 'textkit' does not exist"):
  HEAD's ark.fd copies files only; the tree's ark.fd has
  `ark_install_path_in` (xcopy/cp -R of a directory). Not checkable without
  a build of the tree.

### N4 `scripts/fmt-check.sh`: FAIL, cause found

Step 1: 229 of 258 files of compiler/ and stdlib/ "would reformat"; steps
2-4: 7 of 20 formatted test copies are not stable. One bug causes the
instability and most of the tree's damage: compiler/fmt.fd ends a line at
`;` only inside braces (`if brace_depth > 0 && paren_depth == 0`), so at the
top level `module m; use core; use vec;` stay on one line, the comment on the
next line becomes a trailing comment, and after a blank line the next
declaration gets a leading space (the space is decided before the blank line
is written). A second run joins that line too. The tree shows an earlier
whole-tree run of that formatter: ` use vec;` with a leading space in 28
stdlib files, `module compiler::fmt; use core; use vec; use string;  // ---`
in fmt.fd itself, all of sdl2_h.fd's constants on one line.

With the condition changed to `if paren_depth == 0` (checked with a copy of
fmt.fd compiled as a program over the whole tree): unstable files 50 -> 0,
token changes 0, and the files the bug had not touched (arena, token) pass
`--check`. 229 files still need one `fid fmt` pass (repairs of the old run,
and new code written in another style: aligned trailing comments, one-line
`if ... { return x; }`).

New: step 0 of fmt-check.sh formats tests/fmt/toplevel_in.fd and compares
it with tests/fmt/toplevel.fd, which must pass `--check`; it fails with the
current fmt and passes with the fix.

### M8 `scripts/sanitize.sh`

Already run nightly (verify-nightly.yml `sanitize`, clang on Linux); now
also a sample of 6 programs on every push (ci.yml `selfhost`). The script
now stops each run after LIMIT seconds (default 300) with no input, tells a
program that prints something different on each run (judged on sanitizer
reports alone) from a C-path difference ("also built without sanitizers")
and from one only under the sanitizers, and writes $OUT/summary.san. Here:
UBSan through zig cc on hello, test_vec_filled, test_closure: 3 clean
(zig cc has no ASan runtime; ASan needs the Linux job).

### I6 socket soak

scripts/verify/soak_http.py + tests/verify/soak_http_service.fd, nightly
job `soak` (30 minutes under wrk). First runs: every request answered
correctly, but memory grows about 95-265 bytes per request on a kept-alive
connection (docs/verification/soak-http.md).

## Infra runs, Nov 16 2026 (batch 6: L10D, BM5, BM6, BM8, BM9)

Runs on a busy shared Windows 11 machine (other sessions' compilers
running, Git Bash load average 8–13), `fid` = an agent's one-stage build of
the Nov 16 tree. Tables: docs/benchmarks.md, Nov 16 section.

| Goal | Check | Result |
|---|---|---|
| L10D | `node scripts/playground-embed-check.mjs` | 15 ok: share links in the playground's form, dedent, modes, init, example.html's 3 presets (hello, 5050, E044 rejected), tokens/ast/ir/effects, unknown mode |
| L10D | docs/playground/example.html in a browser, assets from localhost | autorun preset printed, IR mode, diagnostic shown (ANSI stripped), iframe of embed.html ran its `?code=` |
| BM5 | `scripts/service-bench-check.sh` | Fides (C), Go and Node selftests give the same bodies; queries clamp; run.sh: every cell a number, every reply checked |
| BM5 | `fid emit bench/service/fides/server.fd` (Windows) | fails: pe_ir has no p_signal_check / p_signal_register (stdlib/http.fd) |
| BM6 | `bench/compile-speed.sh`, hello / 10k / 100k | first attempt: Fides and Zig failed at 10k (functions named f32, f64, f80, f128); gen_lines.py now names them w0, w1, ...; second run complete |
| BM6 | 1M lines | not run (about an hour per Fides build at these rates); verify-weekly runs it in a step of its own |
| BM9 | game and work sets with fidh1 | all 7 build with emit and ccompile and print the same number; added to bench.yml's --ab gate (`SUITES: "game work"`) |
| BM9 | `scripts/bench-noise-check.sh`, 2 rounds, 5% | no false failure; largest difference sieve +5.1% (passed on the 5 ms slack) |
| BM8 | quiet-machine Windows run | not done: the machine was never quiet |
| all | `scripts/ci-wiring-check.sh` | every gate wired (BM5, BM6, BM9 lines added) |

## Infra runs, Nov 16 2026 (batch 7: BM10, RT11, VF4, VF5, VF14)

Runs on a busy shared Windows 11 machine (other sessions' compilers
running), `fid` = an agent's one-stage build of the Nov 16 tree (fidh1).
Linux runs are in WSL2 (Kali, gcc 15.2, 16 cores, 15 GB).

| Goal | Check | Result |
|---|---|---|
| VF14 | `scripts/sanitize-wsl.sh` (new): the Windows `fid` writes each test's C and its `fid run` output; gcc in WSL builds it with ASan+UBSan and runs it | first run of the sanitizers on the C path. Unpatched: every program that turns a number or tensor into a string hits finding S1 (test_vec_builtins at its first `int_to_str`). With S1 patched in the C (`PATCH=`), a 45-program sample: 37 clean, 3 findings (S2, S3), 5 C-path output differences that are not sanitizer findings, 3 that `dump-c` cannot lower |
| VF14 | `SAN=thread scripts/sanitize-wsl.sh` (the 22 threaded programs of sanitize.sh's TSan set) | 21 clean, no data race reported; test_async_suspend prints differently through C also without TSan |
| VF14 | S2 and S3 patched in the C as well | test_autodiff_grad and test_lora clean, same output as `fid run`; test_free_escape then shows S4 |
| VF5 | `tests/bootstrap/test_verify_opt_ipcp_gconst.fd`, `test_verify_opt_promote_facts.fd`, `test_verify_opt_spec_devirt_icf.fd` (new) | each prints its expected output on `fid run`, `ccompile`, `emit` (pe_ir), `emit --nat`, and `emit` with `FID_IROPT_ON=ipcp,spec,icf` |
| VF5 | `FID_IROPT_STATS=1` on them | promote 48-49, hofspec 1-3, sccp 2 changed functions by default; ipcp 8, spec 49 with `FID_IROPT_ON=ipcp,spec`; **gconst 0 everywhere**, also on a 7-line program whose only global is read once (finding O1); branchfacts 0 (every case in the test is one where a fact must not be used, so only a wrong pass changes anything) |
| VF5/VF7 | five mutants added to `scripts/verify/variants.py` (ipcp-first-constant-wins, gconst-stored-folded, promote-join-first-store, facts-join-assumed, facts-float-negated); each edit matches its source exactly once | not built here (no compiler builds in this batch); the weekly `mutants` job builds them. gconst-stored-folded will survive while O1 stands |
| VF4 | `toggles-peir` job in verify-weekly.yml (new): Windows, one matrix job per `PEIR_*` variant, the whole suite and 20 generated programs | wired, not run (it builds a compiler per variant) |
| BM10 | `scripts/bench-history-check.sh` (new) | 15 checks pass: history.sh order and untouched page, weekly history file, `bench/page-merge.sh` order and idempotence, the weekly job and pages.yml wiring |
| BM10 | `.github/workflows/pages.yml` (new) | not run: needs GitHub Pages turned on (Settings > Pages > Source: GitHub Actions) |
| RT11 | `scripts/idle-bench-check.sh` (new) | server builds through C, selftest, 200 and 1000 idle connections held and counted, every ping answered |
| RT11 | `bench/idle/run.sh` on Windows; `bench/idle/idle.py` against the C build in WSL | docs/perf/rt11-idle-connections.md |
| all | `scripts/ci-wiring-check.sh` | every gate wired (BM10, RT11, VF4 lines added) |

### Sanitizer findings (VF14)

The C fixes of S1-S3 as checked here are `scripts/verify/sanitize-patches.sed`
(`PATCH=scripts/verify/sanitize-patches.sed bash scripts/sanitize-wsl.sh`
looks past them until codegen_c.fd has them).

S1. **Header probes read in front of a large or sanitizer allocation.**
`p_str_hdr_c`, `P_STR_DUP`/`P_STR_DROP` and `__p_vbuf_free` read the words
at `s - 16` and `s - 24` to tell a counted string or a tagged Vec buffer
from a plain one (known-bugs 21, 25). A block of the size classes has room
there (the block before it), but `p_malloc`'s large path (over 1 MB, and
every block under ASan: `P_SAN_ALLOC`) is `calloc(total)` with one header
word, so the probe reads 8-16 bytes before the allocation. Under ASan:
heap-buffer-overflow in the first `int_to_str`; without it, a read of
another allocation's memory, or of an unmapped page for a fresh mmap. Fix
(compiler/codegen_c.fd, checked by patching the C): give the large path
two more words in front, `calloc(1, total + 16)` and `big += 2` before the
header is written, `free(b - 3)` in `p_free`'s `P_CLASS_LARGE` branch, and
the same in `p_shared_alloc` (`+ 24`, `b += 2`).

S2. **`p_tensor_to_str` probes a libc buffer.** `p_tensor_to_str__raw`
builds its text with `malloc`/`realloc`, and `p_str_own_c` reads 16 bytes
before it (heap-buffer-overflow in test_autodiff_grad, test_lora); the
buffer is also never freed. Fix: `char* r = p_tensor_to_str__raw(t); char*
o = p_str_from_bytes_c(r, strlen(r)); free(r); return o;`.

S3. **A global `str` with a literal initializer is a plain C literal.**
`var kept_str: str = "";` becomes `static void* _g_kept_str = "";`, without
the `[P_STR_STATIC][len]` header every other literal has; the first
assignment's `P_STR_DROP` reads 16 bytes before the literal
(global-buffer-overflow in test_free_escape), and would decrement or free
whatever is there if it looked like a count. Fix (codegen_c.fd
`gen_global_init_expr`, `Expr::StringLit`): emit the headered static
(`static const struct { int64_t m; int64_t l; char s[N]; } _g_x_h = {
P_STR_STATIC_C, len, "..." };` and initialize the global with
`(void*)_g_x_h.s`).

S4. **A string captured by a returned closure is freed with its scope.**
test_free_escape `by_closure`: `val s: str = "1234567" + int_to_str(8);
return fn(x: i64) { str_len(s) + x - 1 };` — `by_closure` drops `s` when it
returns (`Str_drop`), and the closure's call reads it
(heap-use-after-free in `str_len`). Without ASan the freed block still holds
the bytes, so the test passes. The capture must take an owner (proposal
0004: keeping a value takes an owner; compiler/lower.fd closure env
build).

C-path differences seen (not sanitizer findings, output differs from
`fid run` also without sanitizers, under gcc on Linux): test_c_callback,
test_metrics_serve, test_references, test_midend_egraph_unary,
test_async_suspend. Not followed up here.

### Optimizer finding (VF5)

O1. **gconst folds nothing.** `iro_const_globals` (compiler/iropt.fd,
WP18) reports 0 folded loads on every program tried, including

```fd
var base: i64 = 100;
fn rb(d: i64) -> i64 { return base + d; }
fn main() -> i64 { println(rb(5)); return 0; }
```

whose IR is `GlobalAddr` then `Load` in `rb` and nothing else. Some use of
the global's address outside these functions (the module's own
initialisation, or the merged runtime) must mark it as escaping in phase 0.
The pass is on by default, so this is lost optimisation, not a
miscompilation.

## Infra runs, Nov 16 (batch 8): TL10, PL7, AD3, RL1, RL2

Compiler for every local run: the self-hosted `fid` built from the tree of
Nov 15 ~23:00 (`fidh1.exe`, one stage), Windows x64; Linux binaries run in
WSL (qemu-user for AArch64 and RISC-V, `qemu-system-x86_64` for fides-os).

### RL1 — `scripts/fuzz/gen_vs_c.py --backends all`

    python scripts/fuzz/gen_vs_c.py -n 20 --seed 1 --fid <fid> --backends all

Backends: run, ccompile, ccompile-O2, emit (pe_ir), nat (`emit --nat`),
emit and ccompile for aarch64-linux, riscv64-linux and x86_64-linux (11).
The C oracle (zig cc -O0/-O2 and WSL's gcc -O0/-O2) agreed with itself on
every seed. About 2.5 to 3.5 minutes per seed with everything built one at a
time, so the run was stopped after seed 10.

| Seed | Result | Backends that differ | Triage |
|------|--------|----------------------|--------|
| 1 | differs | run, emit, nat, emit:{aarch64,riscv64,x86_64}-linux | **M12 (new)**: `~x` on u8/u16/u32 is not masked in an expression. `v11 \|= ~(~v10 as u8 - v11 as u8)` with v10 = v11 = 3 panics "Integer overflow on subtraction: -4 - 3"; ccompile right. Repro `tests/verify/bugs/m12_bitnot_narrow_unsigned.fd`: `fid run` prints `0` for `~a > 200` and `-4` for `(~a) as i64` (want 1 and 252); a store into a typed `var` masks, so `var n: u8 = ~a` is right |
| 2, 3, 4, 6, 8, 9 | ok | — | — |
| 5 | differs | all 11 | **M13 (new)**: every backend loops forever (timeout); C prints 5. `continue` in `for k6 in 0..3` |
| 7 | differs | emit, nat, emit:{aarch64,riscv64,x86_64}-linux | `v1 -= -(~v1 as u64) as i16` panics on subtraction in the native backends only, run and ccompile right: a narrowing cast of a u64 to i16 not sign-extended, the known **M7** (not reduced further) |
| 10 | differs | all 11 | **M13**: timeout on every backend; `continue` in `for k18 in 0..3` |

M13 reduced: `for k in 0..3 { if k == 1 { continue; } n = n + 1; }` never
ends with `fid run` (the Nov 10 `release/fid.exe` and the Nov 15
`release/fid-dev.exe` too). Cause: `compiler/lower.fd` lowers every `for`
form (range ~13587, array ~13728, Vec ~13860, str ~13800) with
`LoopContext_new(cond_block, exit_block, ..)`: `continue` jumps to the
condition, and the index increment is emitted after the body, so it is
skipped. `while` loops carry their own increment in the body and are right,
which is why the suite (its `continue` tests are all in `while` loops) never
saw it. Repro `tests/verify/bugs/m13_for_continue_skips_increment.fd` (range,
Vec and str loops; `fid run` times out).

`gen_vs_c.py` steps around both with the default `--avoid all` (new keys
`bitnot-narrow`, `for-continue`; the programs of every seed stay the same
apart from those spots: the `~` is masked, `& 255`, and a `for` whose body
has a `continue` is written as the equal `while`), so later runs find new
bugs instead of these; `--avoid none` keeps finding them.

Verdict: RL1 not green. 10 of 20 planned programs run on all 11 backends;
4 disagreements, each triaged (M12 and M13 new, seed 7 the known M7). The
log rows for M12 and M13 are to be added to
docs/verification/miscompile-log.md (scripts/miscompile-gate.sh requires
every repro in tests/verify/bugs/ to be named there).

### RL2 — `scripts/suite-all.sh` on every lane (smoke)

    FID=<fid> JOBS=1 AGREE=1 TESTS="tests/bootstrap/hello.fd \
      tests/bootstrap/test_checked_arith.fd tests/bootstrap/test_closure.fd \
      tests/bootstrap/test_hashmap.fd" bash scripts/suite-all.sh     # 513 s

| Lane | Passed | Failed | Did not build |
|------|--------|--------|---------------|
| c:x86_64-linux | 4 | 0 | 0 |
| c:aarch64-linux | 4 | 0 | 0 |
| c:riscv64-linux | 3 | 0 | 1 (test_checked_arith: zig's compiler_rt sub-compilation, "failed to check cache: ... valgrind.zig file_open Unexpected": the shared zig cache under load from other builds, not the program) |
| c:wasm32-wasi | 0 | 0 | 4 (`SO_ERROR` undeclared in the C runtime's `p_co_connect`, compiler/codegen_c.fd ~9703: the known WASI gap of batches 3 and 4) |
| native:x86_64-linux | 4 | 0 | 0 |
| native:aarch64-linux | 4 | 0 | 0 |
| native:riscv64-linux | 4 | 0 | 0 |
| c:x86_64-windows | 4 | 0 | 0 |
| native:x86_64-windows-pe | 4 | 0 | 0 |
| native:x86_64-windows | 4 | 0 | 0 |

Exit 3 (2 lanes failing, and with AGREE=1 the four tests disagree across
lanes because of the wasm lane). No test produced wrong output on any lane
that built it. This is a smoke run (4 tests), not the whole suite;
scripts/ci/cross.yml stays where it is (activating it is the user's call).

### AD3 — fides-os booted with the current fid

New `scripts/fides-os-boot.sh` (and the `fides-os` job of
.github/workflows/bare-metal.yml): builds fides-os's `kernel/minimal.fd` and
`kernel/shell.fd` with `fid emit --boot --load 0x1000000`, boots each with
`qemu-system-x86_64 -kernel` (WSL on Windows) and checks COM1: minimal must
print exactly `Hello from Fides OS!`, shell its `Fides OS v0.1` banner.

    FID=<fid> OUT=<dir> TIMEOUT=40 bash scripts/fides-os-boot.sh
    fides-os: ../fides-os (f14544d 2026-09-13)
    built    minimal: 13480 bytes in 0 s
    ok       minimal: COM1 exact "Hello from Fides OS!"
    built    shell: 435560 bytes in 13 s
    ok       shell: COM1 has "Fides OS v0.1"
    fides-os-boot: 2/2 kernels ok

Negative check: a truncated minimal.bin (first 4000 bytes) booted with
`BUILD=0` fails ("0/1 kernels ok", exit 1). `SELFTEST=1` checks the matcher
(CRLF, extra output after an exact line, a wrong banner, a missing log).
The CI job has not run yet: every CI build of fid stops at stage 1 on the
names the frozen Rust compiler reserves (`scripts/stage0-names-check.sh`:
6 statements, `guard = ...` in compiler/comptime_eval.fd, egraph.fd,
ssa.fd, stdlib/linux_runtime/sys.fd, stdlib/yaml.fd); the job runs that check
first so it fails there with the file and line.

### PL7 — macOS native: what the recorded runs show

`gh run list --workflow macos-native.yml`: five runs on Nov 15, all failed,
on both macos-15 and macos-15-intel, at the same step: `bootstrap.sh`
stage 1, "Parse error at 2183:31: unexpected token 'Eq'" (compiler/
comptime_eval.fd `guard = guard + 1;`, a word fidesc reserves). So no Mach-O
binary has run on a Mac yet; the failure is not in the Mach-O path. CI,
macOS, Bare metal and Benchmarks failed the same way on that commit.

Changes to the workflow: the stage-0 names check first; zig 0.16.0 through
`mlugg/setup-zig@v2.2.1` as in the other workflows (it had 0.13.0);
GNU coreutils for `timeout` (macOS has none: every `cross-test.sh` build and
run calls it, so the suite step of macos.yml would have reported every test
as failing; macos.yml gets the same step); after the six hand-written
programs, the whole suite through the machine layer on the Mac
(`NATIVE=1 scripts/cross-test.sh <arch>-macos`, each test against `fid run`)
with results.txt kept as an artifact; timeout 180 minutes.

### TL10 — editor configs

New `scripts/tl/editor-check.sh` (+ `nvim_check.lua`, `emacs_check.el`,
workflow `.github/workflows/editors.yml`). Neovim, Emacs and Helix are not
installed on this machine, so only the static part ran here:

    STATIC=1 bash scripts/tl/editor-check.sh
    ok   helix: (6 checks)
    ok   sublime: client fid runs `fid lsp`; has a selector
    FAIL sublime: a .sublime-syntax in editors/sublime/ gives *.fd the scope
         source.fides (none: .fd files open as Plain Text, so the selector
         never matches and the server never starts)
    FAIL emacs: auto-mode-alist pattern matches x.fd only (the string reads
         as the regexp ".fd'": x.fd does not match; write "\\.fd\\'")
    ok   neovim: (2 checks)

Both FAILs are real bugs of the configs: Emacs reads `"\.fd\'"` as `.fd'`
(an unknown escape drops the backslash), so fides-mode never turns on and
eglot never starts; Sublime's LSP package selects a client by scope, and
nothing gives `.fd` the scope `source.fides`. With the two fixes applied to
a copy of editors/ (`"\\.fd\\'"`; an `editors/sublime/Fides.sublime-syntax`
with `file_extensions: [fd]`, `scope: source.fides`) the static part passes.
The editors themselves (headless Neovim, batch Emacs with eglot,
`hx --health fides`) run in editors.yml, not run yet. Zed still needs a
Tree-sitter grammar; JetBrains is set up by hand (reported, not checked).

## Infra runs, Nov 16 (batch 9): RL3, RL12, RL13, RL15, RL25

The compiler used is the agents' build `fidh1.exe` (Nov 15 23:01), built
before the current working tree's driver and backend changes, on Windows
(zig cc 0.16, WSL, Node). No compiler was built for these runs.

### RL25: ABI against C, floats mixed with integers

New test `tests/bootstrap/test_verify_abi_c_mixed.fd`: `ldexp(f64, i32)`,
`frexp(f64, *i32)`, `modf(f64, *f64)`, `strtod(*u8, *i64)`, `fmod(f64, f64)`,
and a C result fed into the next call with an integer in between. Every
value is exact in binary and printed scaled to an integer. On Win64 the
i32 of `ldexp` goes in edx (by position), on System V in edi: a backend
that mixes the two conventions prints wrong numbers.

| Lane | Result |
|------|--------|
| C path, Windows (`ccompile`, zig cc) | pass: all 7 lines as expected |
| pe_ir (`emit`) | does not build: `ldexp`, `frexp`, `modf` are not in pe_ir's msvcrt import list (`pe_msvcrt_imports`, compiler/pe_ir_enc.fd); `fmod` and `strtod` are |
| `emit --nat` | does not build: nat_ir has no C import on Windows for names without a runtime counterpart |
| `emit --target x86_64-linux` | does not build with fidh1, and neither does `test_machine_cimport.fd` (same "not available in native Linux executables yet"): fidh1 predates or lacks the CI2/TG15 dynamic imports of the current source, so this is to be rerun with the final build |
| WASM (`ccompile --target wasm32-wasi`) | does not build with fidh1, and neither does `hello.fd` (the C includes mman.h/signal.h and uses SO_ERROR without the WASI emulation defines): a driver/C-runtime problem of that build, not of the test |
| macOS | no host |

The first version used `extern "C" link "m"` without `include "<math.h>"`:
the C path then declared `frexp(double, void*)` and zig cc rejected the
conflict with math.h. The test includes `<math.h>` as the other libm tests
do, and uses the C names directly (no `as` alias), the form
`test_machine_cimport.fd` uses on the machine layer.

### RL13: deterministic output, R1

`fid ccompile tests/bootstrap/hello.fd` twice (different directories and
TEMP, two seconds apart, `FIDES_NO_CACHE=1`) with fidh1: the executables
differ at file offset 0x80 (the PE time stamp) and around 0x61E00 (the
debug directory's PDB GUID), and a `.pdb` is written. fidh1 does not
contain the working tree's R1 fix (compiler/main.fd `repro_win_flags`:
`-Wl,-Brepro` and `-s` with zig cc, `--no-insert-timestamp` with gcc,
`SOURCE_DATE_EPOCH=0`; not in HEAD either). To close R1 with the final
build:

    FID=release/fid.exe NAT_TARGETS="" scripts/verify/repro.sh tests/bootstrap/hello.fd

The two-host part needs a second host:

    MANIFEST=build/m-win.txt scripts/verify/repro.sh      # Windows
    MANIFEST=build/m-linux.txt scripts/verify/repro.sh    # Linux
    scripts/verify/repro.sh --compare build/m-win.txt build/m-linux.txt

### RL3, RL12, RL15: not run

- RL3 (`NATIVE=1 scripts/pass-matrix.sh <target>`): the whole suite once
  per switch (about 55 switches) on each target is hours per target and
  needs the final compiler (the switch list comes from the current source,
  which fidh1 is not built from). Not run.
- RL12 (`scripts/verify/coverage.py --goal 90`): needs an instrumented
  compiler build, which this batch may not make. Not run.
- RL15 (`COMPILER=1 scripts/sanitize.sh`): builds the compiler's C with
  ASan/UBSan; needs that build and Linux or macOS clang (none on this host
  outside WSL). Not run.

## Infra runs, Nov 16 (batch 10: B9, M9, CE4, BM3, BM4)

Compiler for every run: a one-stage `fid` of this tree (the workflow's
`fidh1.exe`), on Windows x64 (MINGW64), with rustc 1.97.0, zig 0.16.0 (also
C++ through `zig c++`), go 1.27.3; no g++. The machine was loaded (load
average 12-14: other sessions' builds), and each cell is one run, so the
times below are rough. What they do show: every language prints the same
number on every program.

### BM3: Benchmarks Game set (`SUITE=game RUNS=1 bash bench/run.sh`)

New: `bench/game/{fides,rust,go,cpp,zig}/binarytrees.*` and `fasta.*`;
`tests/bootstrap/test_verify_bm_binarytrees.fd` (4398) and
`test_verify_bm_fasta.fd` (2235702122). Their expected values come from a
Python reference of the same algorithm. Full-size outputs: binary-trees
14985902, fasta 887466333, the same in all five languages and on both Fides
paths.

| Benchmark | Fides native | Fides via C | Rust | Zig | C++ (Clang) | Go |
|---|---|---|---|---|---|---|
| binarytrees | 1.91 | 1.21 | 2.66 | 0.56 | 1.27 | 0.85 |
| fannkuch | 7.37 | 5.35 | 4.27 | 5.30 | 4.70 | 4.21 |
| fasta | 1.09 | 0.88 | 1.01 | 0.76 | 0.78 | 0.91 |
| mandelbrot | 2.76 | 1.94 | 1.84 | 1.80 | 1.77 | 2.02 |
| nbody | 4.19 | 2.22 | 0.70 | 0.61 | 0.80 | 1.07 |
| spectral | 2.47 | 0.74 | 0.72 | 0.66 | 0.67 | 0.79 |

This is the first run of the four earlier programs too. Not done: the
reverse-complement, k-nucleotide, regex-redux and pidigits benchmarks
(the first two read a FASTA file, regex-redux needs each language's regex
library, pidigits big integers: Rust's standard library has neither, and
run.sh does not fetch crates).

### BM4: real work (`SUITE=work RUNS=1 bash bench/run.sh`)

New: `bench/work/{fides,rust,go,cpp,zig}/compiler.*`, a small compiler.
A generator writes 50000 lines of `let vK = <expr>;` (11 MB). A lexer turns
them into tokens, a recursive-descent parser builds a boxed tree for each
line, and an evaluator runs the tree modulo 1000003. Output: 13744772436
everywhere. `tests/bootstrap/test_verify_bm_compiler.fd`: 20 lines, depth
6 (5210287).

| Benchmark | Fides native | Fides via C | Rust | Zig | C++ (Clang) | Go |
|---|---|---|---|---|---|---|
| compiler | 1.12 | 0.73 | 0.82 | 0.61 | 0.75 | 0.75 |
| hash | 1.21 | 0.81 | 1.43 | 0.67 | 0.81 | 0.81 |
| sort | 4.16 | 1.76 | 2.12 | 1.16 | 0.52 | 1.10 |
| strings | 1.21 | 0.78 | 0.74 | - | 0.51 | 0.61 |

This is the first run of sort, strings and hash. Not done: JSON, regex and
file-reading benchmarks. Rust needs serde and regex crates for them, which
run.sh does not fetch. Zig still has no `strings` program.

### CE4: every error message has a test (`python3 scripts/diag-coverage.py`)

New script `scripts/diag-coverage.py`. It finds the error messages of
compiler/*.fd: the literals of `errors.push(...)` lines, plus literals that
start like an error ("Type error", "Parse error", ...). The literals that
classify_error itself tests are skipped. For each message it reports:

- whether it has a `Fix:`/`Hint:` line;
- the ErrCode classify_error gives it;
- whether a test expects it: a `// expect-error:` or `//! error:` line in
  tests/ whose text holds a literal piece of the message.

It is a ratchet against `scripts/diag-coverage-baseline.txt`: CI's `gates`
job fails when more messages are untested, lack a Fix line or lack a code
than the baseline allows.

Results: 304 messages, of which 183 are tested after this batch (172
before), 92 have a Fix line and 72 get a code other than `Custom`. New
expected-fail tests, each checked with `fid check` to print its text:

- `test_diag_lex_empty_char_fail.fd`
- `test_diag_lex_unterminated_string_fail.fd`
- `test_diag_lex_unknown_escape_fail.fd`
- `test_diag_lex_x_escape_fail.fd`
- `test_diag_lex_raw_string_fail.fd`
- `test_diag_parse_range_int_fail.fd`
- `test_diag_parse_annotation_name_fail.fd`
- `test_diag_tc_impl_undefined_trait_fail.fd`

A ninth test, a positional argument after a named one, was dropped: this
`fid` accepted `area(w: 2, 3)` with no error, although the message is in
typechecker.fd. That is worth a look by the front-end group.

Left, in two parts:

- Tests for the other 121 messages: `--list untested`.
- Codes for the 232 messages classify_error maps to `Custom`: `--list
  custom`. This is a compiler.fd change.

### M9: nightly differential fuzzing

`scripts/fuzz/nightly.sh` was already scheduled (verify-nightly.yml, job
`fuzz`, 02:23 UTC). On Linux it now also runs the native path:
`gen_programs.py` has a new backend, `emit:<target>`, which builds with the
machine layer and runs the result (under qemu-user for a foreign target).
nightly.sh adds `emit:x86_64-linux`, or `emit:aarch64-linux` on an AArch64
host. `NO_NATIVE=1` or `FUZZ_BACKENDS=...` overrides the list. The sanitizer
sample now runs on any host that is not Windows, where before it depended on
the exact backend list.

A local check, 5 programs with `--backends run,ccompile`, failed on C only
because this shell has no C compiler on PATH (`C compilation failed`). That
is the environment, not a finding. Left: a week of clean nightly runs.

### B9: --inc self-compile fixpoint

This batch may not build the compiler, so `FIXPOINT=1 scripts/inc-check.sh`
was not run. I read through what it relies on:

- The second build can reuse every unit only if the cache is shared and
  keyed by content. It is: unit objects go to `<output dir>/.fidcache`, and
  `inc_artifact_path` keys them by the unit's C hash, the C compiler and the
  flags, not by the compiler's own hash.
- Both builds write to the same `$X` directory, so the cache is shared.
- `--cc-opt` exists (main.fd).

The check is now wired into verify-weekly.yml (job `repro`, after VF12), so
it runs every week with the one-stage fid-dev. Left: its first run, and fixes
for anything it finds.

## Infra runs, Nov 16 2026, batch 13 (RL14, VF9, VF11, PL2)

Compiler used for the runs: a one-stage fid built before this batch
(`fidh1.exe`), Windows x64, at most one fid process at a time. Nothing in
`compiler/` was built (no bootstrap, no `dump-c` of compiler/main.fd).

### RL14, fixpoint through our own backend

- `scripts/native-fixpoint.sh` itself was **not run**: building
  compiler/main.fd with `fid emit` is a compiler build, which this batch
  may not do. Whether `emit` builds the compiler is still unknown.
- New `scripts/native-fixpoint-check.sh`: runs native-fixpoint.sh against
  five stand-in compilers (shell scripts) and checks its verdict: fixpoint
  → 0, stage 2 ≠ stage 3 → 2, emit fails / stage 2 cannot run / stage 3's
  hello differs → 1. Run: 5 of 5 right (17 s).
- It found that native-fixpoint.sh reported "differ (0 bytes)" when the
  stages differ in length (`cmp -l` stops at the shorter one): the message
  now gives both sizes and the differing bytes of the common part.
- CI: verify-weekly.yml job `native-fixpoint` (BACKEND linux on Ubuntu, pe
  and nat on Windows, FID_VERIFY=1) runs the real check every week;
  ci.yml's gates job runs native-fixpoint-check.sh on every push.

### VF9, formal semantics

- `docs/verification/semantics-2.md`: rules for floats, strings, enums /
  Option / Result / match, generics (monomorphisation), closures,
  references, effects (static only), memory and spawn/await, each tied to
  its reference section; unspecified points listed.
- `tests/bootstrap/test_verify_semantics_ext.fd`: one printed line per
  rule, the value the rule gives. With fidh1 on `run`, `ccompile` (zig cc)
  and `emit` (pe_ir) all 30 lines but S1 match; S1 (`str_len("ab\0cd")`)
  prints 2 where the rule (reference §1.8, RT2, `test_frontend_str_nul.fd`)
  says 5. fidh1 predates RT2; the test should pass on the rebuilt tree.
- Not mechanised in a proof assistant (none installed).

### VF11, memory safety

- `scripts/verify/own_model.py`: exhaustive bounded check of λ-own with
  copy-on-write, boxes in boxes and `&mut` calls; size 3: 1,351 accepted
  programs, size 4: 46,089, size 5: 1,859,415 — 0 failures; all 7 mutants
  caught. Details: `docs/verification/memory-safety-model.md`.
- CI: size 4 + mutants in ci.yml gates; size 5 in verify-weekly.yml.

### PL2, tier 1

- `scripts/tier1.sh`: picks the host's tier-1 platform (windows-x64,
  linux-x64, linux-arm64, macos-arm64), runs the whole suite on its C and
  native lanes with `suite-all.sh AGREE=1` and checks every expected-fail
  test is refused. Smoke run on Windows (lane native:x86_64-windows-pe,
  two tests and two expected-fail tests): PASS in 42 s.
- `.github/workflows/tier1.yml`: one job per platform (windows-latest,
  ubuntu-latest, ubuntu-24.04-arm, macos-15) on every push and pull
  request.
- `scripts/cross-test.sh`: an ARM64 (or RISC-V) Linux host now runs its own
  architecture's binaries directly instead of through qemu (the ARM64
  runner has no qemu).
- Not done: a green run of tier1.yml. Until the four jobs pass, no
  platform is tier 1 by this definition.
