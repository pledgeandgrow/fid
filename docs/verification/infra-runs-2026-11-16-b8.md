# Infra runs, Nov 16 (batch 8): TL10, PL7, AD3, RL1, RL2

Compiler for every local run: the self-hosted `fid` built from the tree of
Nov 15 ~23:00 (`fidh1.exe`, one stage), Windows x64; Linux binaries run in
WSL (qemu-user for AArch64 and RISC-V, `qemu-system-x86_64` for fides-os).

## RL1 — `scripts/fuzz/gen_vs_c.py --backends all`

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

## RL2 — `scripts/suite-all.sh` on every lane (smoke)

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

## AD3 — fides-os booted with the current fid

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

## PL7 — macOS native: what the recorded runs show

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

## TL10 — editor configs

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
