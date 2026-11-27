# AGENTS.md — Fides Compiler Working Guide

> This file gives AI assistants (and humans) the context needed to work on the
> Fides compiler safely. Read this before making changes.
>
> **Fides is self-hosted and independent.** The primary compiler is in
> `compiler/*.fd` (65 modules, ~357K lines). The Rust source in `src/*.rs`
> (57 files, ~114K lines) is the bootstrap reference, frozen: it builds
> stage 1 when `compiler/*.fd` changes and runs the test harness. The
> language is what `fid` accepts: docs/reference/reference.md.
>
> **The self-hosted compiler is the release compiler:** `RELEASE=1
> ./scripts/bootstrap.sh` builds it in 3 stages (fixpoint checked) into
> `release/fid.exe`, which the installer ships as `fid.exe`. It checks
> more than the Rust compiler (generics, borrows, safety annotations,
> borrowed strings): see docs/reference/language-guide.md § 10.
>
> **On this dev machine the commit limit, not free RAM, is the build wall**
> (Nov 27: 44 of the 48 GB taken with 8 GB of RAM free; `zig cc` died on
> the giant units at once). Before a build compare `CommitLimit -
> CommittedBytes`, not only the free memory; `SPLIT_OPT=-O1 JOBS=1
> ./scripts/build-selfhost.sh` builds every unit where -O2 does not fit.

## Build & Test Commands

### Self-hosted compiler

```bash
# Release build: 3 stages, fixpoint checked, installed as
# release/fid.exe (the installer ships it as fid.exe)
RELEASE=1 CC="zig cc" ./scripts/bootstrap.sh
#   Stage 0: release/stage0/fid(.exe) pinned by scripts/stage0.sha256, else
#   fidesc; STAGE0=<exe> overrides. Neither pin file exists (Nov 21), and
#   from fidesc stage 2 runs out of memory (known-bugs 2g.79, roadmap CB8):
#   pass STAGE0=<a self-hosted fid>. Nov 21 evening: statement
#   temporaries are dropped in src/lower.rs too (Vec/HashMap/str
#   fresh-call temps); the wall is not re-measured yet.
.\release\fid.exe run      tests/bootstrap/hello.fd
.\release\fid.exe ccompile tests/bootstrap/hello.fd -o hello.exe  # needs a C compiler

# Single stage (development; not fixpoint-checked):
# temporary C in build/selfhost/ + release/fid-dev(.exe)
./scripts/build-selfhost.sh          # stage-0 dump-c + C compile → release/fid-dev.exe
#   (8 units at -O2 when STAGE0 has `fid split-c`, else one unit at -O0;
#    on a memory-tight machine SPLIT=16 JOBS=2: one 123MB unit at -O0
#    needs >16GB and LLVM dies, so the split must succeed; JOBS=4 x -O2
#    units does not fit under ~5GB free. Nov 21 gate log: build/release-archive/gate-nov21*.log)
#    SPLIT_OPT=-O1 (or -O0) JOBS=1 when the machine cannot commit what the
#    giant units take at -O2. A unit that fails is built again alone, then
#    at -O1 and -O0 with the same flags, and the last line says what was
#    built ("(16 units at -O2)", or "(16 units: 10 at -O2, 6 below it:
#    unit 1-O0 ...)"): read it before you time a build.
#    STAGE0=release/fid.exe for the fast path: fid at -O2 runs ~17x faster;
#    STAGE0 defaults to release/stage0/fid.exe — pinned Nov 21 to a
#    self-hosted 1.0.0+dev, hash in scripts/stage0.sha256 — else fidesc,
#    which has no split-c. A failed split build falls back to one unit
#    at -O0 and still exits 0, 2g.86: look for "8 units at -O2" in its
#    output. SPLIT=1 CC=tcc is the fastest way to *a* fid for a one-shot
#    smoke test; the -O2 split is worth it when the new fid itself does
#    heavy compiling. `fid emit compiler/main.fd` is NOT a dev shortcut
#    yet: Nov 21 it finished in 5m18s (iropt 97s + pe_ir emission 188s)
#    but the produced binary panicked on `run hello` — unresolved-label
#    warnings, a live pe_ir miscompile of the tree.)

.\release\fid.exe dump-c compiler/main.fd > c2.c   # self-compile (fixpoint)
.\release\fid.exe run tests/bootstrap/hello.fd    # interpreter (as fidesc run)
```

### Rust bootstrap compiler (builds stage 1; runs the test harness)

```bash
# Build (release — what verify.sh uses)
cargo build --release

# Build (debug — faster, used by cargo test)
cargo build

# Run all tests across all backends
./target/release/fidesc test

# Full verification (build + test + compare to baseline)
./verify.sh

# Save current test results as new baseline (only after intentional changes)
./verify.sh --save

# Quick build check (no tests)
./verify.sh --quick

# Self-host verification (tests/selfhost/*.fd on 3 backends)
# Separate from verify.sh: verify.sh → tests/bootstrap/ (Rust compiler in src/)
# verify-selfhost.sh → tests/selfhost/ (self-hosted Fides components in compiler/)
./verify-selfhost.sh
./verify-selfhost.sh --save
./verify-selfhost.sh --list

# Build the self-hosted compiler into release/ (manual two-step)
#   1. fidesc ccompile compiler\main.fd -o ... generates C via the Rust bootstrap
#   2. link the generated C with a C compiler (zig cc / cl / tcc)
#   See docs/reference/selfhost.md § Build Pipeline for details.
# (build-selfhost.ps1 no longer exists — do not reference it.)

# NOTE: scripts/gen_golden.sh and scripts/bisect.sh do NOT exist —
# verify.sh --golden currently fails; bisect via dump-* commands instead.

# Run a single test through the interpreter
./target/release/fidesc run tests/bootstrap/<name>.fd

# Show IR for a test
./target/release/fidesc ir tests/bootstrap/<name>.fd

# Show PIR (L1/L2/L3) for a test
./target/release/fidesc pir tests/bootstrap/<name>.fd 3

# Dump intermediate representations (dump-* commands)
./target/release/fidesc dump-tokens tests/bootstrap/<name>.fd
./target/release/fidesc dump-ast tests/bootstrap/<name>.fd
./target/release/fidesc dump-ir tests/bootstrap/<name>.fd
./target/release/fidesc dump-pir tests/bootstrap/<name>.fd 3
./target/release/fidesc dump-c tests/bootstrap/<name>.fd

# Differential testing (compare against golden files)
./target/release/fidesc diff-test tests/bootstrap/<name>.fd --stage ir

# Run with per-stage timing
./target/release/fidesc run tests/bootstrap/<name>.fd --trace

# Run with recursion depth limit (0 = unlimited)
./target/release/fidesc run tests/bootstrap/<name>.fd --max-recursion 1000

# Show assembly (x86-64)
./target/release/fidesc codegen tests/bootstrap/<name>.fd --target x86-64

# Show assembly with peephole (what `build` actually uses)
./target/release/fidesc codegen tests/bootstrap/<name>.fd --target x86-64 --peephole

# Run the LSP server (for editor integration)
./target/release/fidesc lsp

# Cross targets through C (zig cc from any host): Linux ARM64/RISC-V/x86-64
# (static musl), macOS, WebAssembly (WASI). cross-test.sh builds the suite
# for a target, runs it (qemu-user in WSL/Linux, Node for WASI) and compares
# output and exit code with `fid run`. Builds at 10 jobs need a lot of RAM.
.\release\fid.exe ccompile <file.fd> --target aarch64-linux -o out
.\release\fid.exe ccompile <file.fd> --target wasm32-wasi -o out.wasm
node scripts/wasm-run.mjs out.wasm
# Native Linux AArch64 / RISC-V 64 / x86-64, no C compiler (compiler/nat_ir.fd:
# machine IR, one allocator; nat_enc.fd encoders; runtime stdlib/linux_runtime/);
# NATIVE=1 for the suite. `--target x86_64-linux` uses this backend, and so
# do `--elf` / `--target linux` (Release F4); FID_ELF_PEIR=1 keeps pe_ir's
# old no-stdlib ELF path.
.\release\fid.exe emit <file.fd> --target aarch64-linux -o out
.\release\fid.exe emit <file.fd> --target x86_64-linux -o out
# Windows x64 on the same machine layer (opt-in; pe_ir stays the default):
# a PE, the Linux runtime over kernel32/ws2_32 (stdlib/linux_runtime/win.fd);
# floats in xmm2..xmm11 (MI14), vector registers xmm12..xmm15, the
# exception handler keeps xmm6..xmm15
.\release\fid.exe emit <file.fd> --nat -o out.exe
NATIVE=1 FID=release/fid.exe JOBS=4 bash scripts/cross-test.sh x86_64-windows
# Checks of the own backend (docs/roadmap/llvm-roadmap.md): FID_VERIFY=1 runs
# the IR verifier and the register-allocation checker; FID_NAT_OFF=<passes>
# turns native passes off (nat_ir: mir, promote, ipra, fold, hint, coalesce,
# remat (a spilled rematerialized value has no slot), rematmov (moves
# between coalesced vregs do not count as definitions), weights,
# segscan (RA2: the scan over hulls), uselive (RA2: liveness to a block
# fixed point), holes (RA2: hulls throughout), split, splitprune (RA4
# live-range splitting; splitfloat: spilled floats not split, MI14),
# arghint (RA7 argument-register hints, on by default), argfree, argtrade
# (RA3: argument registers for spilled ranges no call crosses, and the
# trade of callee-saved registers; docs/reference/argument-registers.md),
# fcallee (RA18: floats across calls in callee-saved float registers),
# fregs (floats in
# float registers, on by default on every target, --nat Windows included),
# winfregs (MI14: no float registers on --nat Windows only), coldhint
# (blocks calling @cold, cold_path() or never-returning functions laid out
# last), pgolayout (blocks a --profile-use profile never or rarely entered
# laid out last), vecinline, unchecked, panicline, xmmcache, debug, probe,
# coldpanic, brfuse, addshl, asm, cmpslot, rmwslot (TG7: compares and
# read-modify-writes on a spilled value's slot), blsmsk, blsi (TG7 BMI1
# under --cpu x86-64-v3|v4), wbidx (TG8: AArch64 pre/post-indexed loads
# and stores in loops), vzsimd, vzunroll (K10: @simd's checked-op
# vectorization and 2x unroll; docs/reference/simd-annotation.md),
# dbgscopes (DB4: lexical scopes in the debug info for a name bound more
# than once), dbgbraces (DB4: a scope runs to its block's `}`; off: only
# the lines that define or read it); rules.fd:
# rules, purecse; egraph.fd: egraph, forward); FID_NAT_ON=greedy uses the
# greedy allocator in place of the linear scan, FID_NAT_ON=greedycmp (with
# --stats) prints an [ra9] line per function comparing the two (RA9,
# docs/reference/greedy-allocator.md); pe_ir (default Windows `emit`):
# FID_PE_OFF=<asm,wipe,tail,layout,ra,...> (ra: no pe_ir register
# allocator, RA1, every value in its frame slot); the full list of switches is
# docs/verification/kill-switches.md. FID_NAT_DUMP=<function> prints
# its machine IR. IR optimizer (compiler/iropt.fd): FID_IROPT_OFF=<passes>,
# FID_IROPT_ON=<opt-in passes>|all, FID_IROPT_STATS=1; FID_NAT_OFF=readcse.
# The C path (ccompile, dump-c) runs no IR optimizer.
# Compile time (output unchanged; each has a switch to compare against):
# the checked-library stamp (D9) skips the bodies of core and the merged
# runtime once checked clean (FIDES_CHECK_CORE=1: check them); comptime
# substitution skips functions it cannot change (FID_CT_FULL=1: all);
# `emit` lowers only what compiler/reach.fd finds reachable (FID_LOWER_ALL=1:
# everything; the C path and `run` always lower everything).
python3 scripts/enc-oracle.py --fid release/fid.exe   # encoders vs clang's assembler
.\release\fid.exe test-rules                          # rewrite rules on values
bash scripts/rules-smt.sh release/fid.exe              # rules proved by z3 (pip install z3-solver)
NATIVE=1 bash scripts/pass-matrix.sh aarch64-linux     # each pass off: same results
bash tests/nattools/machine_split_check.sh             # RA4 splitting: checker, mutation
bash tests/nattools/sw_unwind_check.sh                 # RA12/DB6: unwinding under qemu's gdb stub
bash tests/nattools/ra14_fast_alloc_time.sh            # RA14: allocation time of the fast allocator
bash tests/nattools/win_callback_regs_check.sh         # --nat Windows: the exception handler keeps rdi/rsi/xmm6-15 (RA7/RA3)
NATIVE=1 FID=release/fid.exe JOBS=4 bash scripts/cross-test.sh riscv64-linux
FID=release/fid.exe JOBS=4 bash scripts/cross-test.sh aarch64-linux

# Run lint checks (unused functions, unused imports; fid also unwrap() outside tests)
./target/release/fidesc lint <file.fd>
.\release\fid.exe lint <file.fd> --perf      # + quadratic patterns in loops (CP9)

# Explain what the optimizer did (CP3) and where a function allocates (CP2)
.\release\fid.exe explain <file.fd> [<name-substring>]   # checks removed vs kept, and why
.\release\fid.exe explain <file.fd> --alloc [<name>]     # allocation sites + why heap

# Self-hosted only: unsafe code a program reaches, leaks, use after free
.\release\fid.exe audit <file.fd> [--stdlib] [--strict]
.\release\fid.exe run <file.fd> --leaks
.\release\fid.exe run <file.fd> --checked-alloc

# Benchmark a file (interpreter or C native)
./target/release/fidesc bench <file.fd> [-n <count>] [--ccompile]

# Profile a file (per-function timing)
./target/release/fidesc profile <file.fd>

# Format a Fides source file
./target/release/fidesc fmt <file.fd>

# Parse a C header and generate Fides FFI bindings
./target/release/fidesc parse-header <header.h>

# Debug a Fides program (interactive debugger). `fid debug <file.fd>` is
# wired (main.fd cmd_debug -> debugger.fd Debugger_run); `fid replay
# <trace>` replays its own traces, written by `fid run <file.fd> --record`.
# `autopsy` remains fidesc-only: fid has no autopsy command and
# compiler/native_debugger.fd is imported by nothing.
./target/release/fidesc debug <file.fd>
.\release\fid.exe debug <file.fd>

# Replay a recorded execution trace
./target/release/fidesc replay <trace.fdtrace>

# Debug a native binary (ptrace-based, Linux only)
./target/release/fidesc autopsy <file.fd>

# Run with post-mortem trace saving (auto-saves trace on panic)
./target/release/fidesc run <file.fd> --post-mortem

# Package manager (dependencies, install, lock files)
./target/release/ark init [name]
./target/release/ark add <dep> --path <path> | --git <url>
./target/release/ark install
./target/release/ark list

# Documentation generator (HTML or Markdown from .fd source)
./target/release/scribe <file.fd> [--format html|markdown] [--output <dir>]
./target/release/scribe <dir>   [--format html|markdown] [--output <dir>]

# Code coverage (function, line, and instruction coverage)
./target/release/veil <file.fd> [--format text|html] [--output <dir>]

# Build the Windows installer (requires NSIS)
# On Windows: cd installer && makensis fides.nsi
# On CI: triggered automatically by GitHub Actions on release tags
# (release.yml, on a v* tag; it also builds the Linux archive)
# Manual trigger: gh workflow run release.yml -f version=1.0.0
```

## Current Baseline (Nov 27 2026 — v0.8.5-unstable, one-stage builds from a self-hosted stage 0; whole suite on `run` Nov 25)

> Project-wide numbers are kept in docs/reference/selfhost.md → "Numbers"
> (CURRENT.md was removed on Nov 8); keep the two in step.
>
> Nov 27, afternoon (the LLVM roadmap): SP6, LF20 and SP7 are marked done
> in docs/roadmap/llvm-roadmap.md (163 done, 33 partly), each with its gate
> green on a one-stage -O2 build of the tree (16:19, all 16 units at -O2,
> `check compiler/main.fd` clean in 29 s). CF32 and CF37 are closed (four
> conformance gaps left: CF33 to CF36). The gates of MI14, SP16, TG11 and
> WP6 are green after their defects were fixed; those goals stay partly
> done for what their entries name. Each of the 19 partly-done goals whose
> code sits in files another session edits says in its entry what is
> exactly left (docs/roadmap/goal-workflow.md, section A, has the queue).
>
> Nov 27: v0.8.5-unstable is being prepared from be36317 ("0.8.5
> unstable"). Its fid.exe is a ONE-STAGE build from a self-hosted stage 0,
> not a 3-stage fixpoint (the last fixpoint is Nov 21, below). The newest
> checked builds: Nov 25, a fid built from the 905d756b tree in one stage
> from a self-hosted stage 0 (8 split units, unit 3 at -O0, the rest -O2);
> Nov 27, the Nov 25 and Nov 27 trees each built fully with gcc -O2 for a
> before/after comparison (docs/perf/compile-time-2026-11-27.md). With the
> Nov 25 fid: `check compiler/main.fd` no errors (78.8 s, unit 3 at -O0);
> whole suite on `run` 1,897 of 1,983 pass (82 of the 86 failures fail the
> same with the earlier compiler, the other 4 are 300 s timeouts with
> correct output); leak gate 11/11. With the Nov 27 fid: self `dump-c
> compiler/main.fd` 43.5 s. Not finished when the Nov 27 gate stopped: SP9
> checks, the leak gate and the whole suite on 3 backends.
>
> Nov 20: fidesc compiles the Nov 19 tree again (impl-method @annotations,
> the timeout(ms){} desugar, token-level pre-scan imports, Struct_m module
> attribution, raw-i64 builtins, 7 ported builtins, counted p_getenv/
> p_argv — the Nov 20 commits). Every CI workflow passes stage 0; the
> Bare-metal qemu lane and Editors are green, macOS reaches its
> 1,155-test suite. NEW BLOCKER (roadmap CB8 / known-bugs 2g.79): a
> fidesc-BUILT fid self-compiling the 361K tree needs >16 GB (the frozen
> lowering emits no 0004 temporary drops) — the 3-stage bootstrap from
> fidesc cannot pass stage 2 until the rc-ownership lowering is ported to
> src/lower.rs. Nov 21: part of it is ported (owners for Vec/HashMap
> elements, the C runtime's 0004 string layer); statement temporaries and
> parameters are not yet.
>
> Nov 21: the wall is in the fidesc-built stage 1 only. A self-hosted fid
> of Nov 18 used as stage 0 compiled the unmodified Nov 20 tree (86cc919)
> in one step: 121 MB of C in ~5 min, 8 split units at -O2, a 15 MB fid.
> That fid passes run, emit (pe_ir) and ccompile of hello and the leak gate
> (10/10), with none of the fidesc-built fid's crashes. So the 3-stage
> bootstrap can start from a released fid instead of fidesc. The Nov 21
> check of 224 roadmap goals with it: 87 validated, 121 partly, 15 failed,
> 1 not checkable here; the bugs it found are known-bugs 2g.80-2g.113.

```
SELF-HOSTED COMPILER (primary source):
  compiler/*.fd:  65 modules, 361,317 lines (Nov 27 evening) — THE COMPILER. 58 are reached
                  from main.fd and all are called (export prefixes differ
                  from file names: irverify→ir_verify, iropt_me→ime_*,
                  csplit→cs_*, nat_ir→nat_*/ng_*, scribe→process_directory,
                  build_info→fid_*, cowcheck→cow_*, debugger→Debugger_*).
                  The retired PIR files were deleted outright since Nov 21.
                  7 are imported by nothing: autograd (intentional no-op
                  shim; real autodiff is the interp.fd tape), runtime
                  (superseded by stdlib/linux_runtime, merged at emit),
                  flat_ast (deliberately unwired — compiler.fd:6140; its
                  int_to_hex would hide string's), diag (used by
                  tests/selfhost/test_diag.fd + test_sl_debug.fd),
                  embedded, hotreload, native_debugger (last three: Rust
                  features whose fid commands are not wired — no autopsy
                  command exists; main.fd:9938 keeps a dangling
                  cmd_native_debug comment). `fid watch <file> [cmd]` IS
                  wired (cmd_watch: polls the file's contents, re-runs
                  `fid <cmd>` on change). debugger.fd IS wired:
                  `fid debug` -> Debugger_new/Debugger_run.
  New modules     since the Nov 9 audit (each has a file-analysis page now):
                  ssa (mid-end SSA: mem2reg, dead phis, loop exit values),
                  mir (the machine IR of the machine layer), tv
                  (translation validation, --validate), disasm (fid disasm,
                  also on .exe files), pdb (PDB debug info for `emit -g`,
                  pe_ir and --nat), pgo (`run --profile` → emit
                  --profile-use), image_writer (one writer for ELF/PE/
                  Mach-O), nat_thumb (Cortex-M Thumb-2 codegen), ir_ser +
                  ast_ser (binary IR/AST caches, .fidcache CS2/CS4),
                  build_info (version stamp; fid version prints 1.0.0+dev)
  Fixpoint:       VERIFIED Nov 21 on the c082ee6+changes tree (incl. the
                  558d0bc temps port): a 3-stage bootstrap from the pinned
                  release/stage0/fid.exe reached stage 3 — `FIXPOINT OK:
                  stage 2 and stage 3 emit identical C` (123,338,942 bytes
                  both), RELEASE OK with smoke test; build/release-archive/gate-nov21-c.log.
                  All three stages fell back to one -O2-OOM'd unit at -O0
                  (~15 min each): the split -O2 build does not fit in ~4GB
                  free even at SPLIT=16 JOBS=2. Before: Nov 8, and NOT
                  re-verified on the Nov 19 tree (the compiler grew ~2.3x).
  Bootstrap:      5 generations verified (Rust → c1 → c2 → c3 → c4 → c5)
  Binary:         release/fid.exe, shipped as fid.exe. release/ is local
                  and gitignored, and other sessions rebuild it (fid.exe
                  and fid-dev.exe were both rebuilt Nov 21): check which
                  stage 0 built a binary (release/fid-dev.log, .cc.log)
                  before trusting or timing it. A fidesc-built binary self-compiling the whole
                  tree hits the 2g.79 memory wall — see the blocker note
                  above. Nov 21: a fid built from a self-hosted stage 0
                  (the Nov 18 fid) builds the current tree in one step and
                  passes run/emit/ccompile and the leak gate.
                  scripts/build-selfhost.sh's split -O2 step can fail
                  silently and fall back to one unit at -O0 with exit 0
                  (2g.86): check for "8 units at -O2" in its output. The
                  whole suite was run with fid as the compiler on Nov 25,
                  on `run` only (above). Nov 25: fid built from the
                  905d756b tree, one stage from a self-hosted stage 0 (8
                  split units, unit 3 at -O0). Nov 27: the Nov 25 and Nov 27
                  trees built fully with gcc -O2 (timing comparison).
                  v0.8.5-unstable (Nov 27, be36317): fid.exe is a one-stage
                  build from a self-hosted stage 0, not a 3-stage fixpoint.
  Self-compile:   `dump-c compiler/main.fd`, Nov 27: 43.5 s total (44 s
                  wall) with the Nov 27 tree's fid built fully with gcc -O2
                  (typecheck 4.9 s, mono 0.2, lower 11.8, C generation 14.9);
                  the C is byte-identical with FID_MONO_FULL=1 and with
                  FID_PAR=4 (llvm-roadmap SP12).
                  Nov 21 on the 361K tree: 57 s
                  with the compiler built at -O2 (8 split units; type check
                  4.8 s, borrow check 0.7, mono 0.6, front end 16.6 in all,
                  lowering 32.9, C 19.3; competitive-roadmap CS4).
                  Nov 9 (158K lines): 74 s at -O2 (type check 18 s, borrow
                  check 5, mono 5, lowering 24, C 20), 229 s at -O0.
                  `check compiler/main.fd --time`: 35 s at -O2 (Nov 21);
                  137 s with fid31 at -O0 (earlier). Several fid processes at
                  once (other sessions' builds) can make a stage 10-200x
                  slower: time on a quiet machine. scripts/bootstrap.sh splits
                  each stage's C into 8 units at -O2 when the previous stage
                  has `fid split-c` (SPLIT=n units, JOBS=n at a time;
                  SPLIT=1 or tcc: one unit at -O0).

RUST BOOTSTRAP (reference + test harness):
  src/*.rs:       57 files, 114,899 lines (Nov 27) — bootstrap reference (frozen;
                  Nov 21: the CB8 ownership port in lower.rs/codegen_c.rs)
  Used when:      compiler/*.fd changes (builds stage 1); runs `fidesc test`
  Suite (Nov 2):  interp 339/339, C 343/343, PE 345/345, expected-fail 89/89
                  (saved baseline: an earlier run, 334/338/341). The suite
                  is ~3.3x larger now — a fresh full run is part of the
                  1.0 release prep.
  Suite (Nov 25, fid as the compiler, `run` only): 1,897 of 1,983 pass;
                  82 of the 86 failures fail the same with the earlier
                  compiler, the other 4 are timeouts (300 s limit) with
                  correct output (noise_proto 262 s new vs 241 s old alone).
                  C and native lanes of the whole suite: not finished.

WINDOWS INSTALLER (fides.nsi's default VERSION is 0.8.0):
  installer/:     NSIS script + build helper (5 tracked files; the built
                  Fid-Setup-*.exe beside them are not tracked)
  CI:            .github/workflows/: 14 workflows — ci.yml, tier1.yml and
                 tier2.yml (scripts/tier1.sh/tier2.sh lanes on Windows,
                 Linux, Linux-ARM, macOS), suite-all.yml,
                 macos-native.yml, release.yml, bench.yml, bare-metal.yml,
                 editors.yml, pages.yml, verify-nightly.yml,
                 verify-weekly.yml, cross.yml (Nov 25), stage0-native.yml
                 (Nov 27: the native stage 0 seed)
  Output:        Fid-Setup-<version>.exe (named Fides-Setup up to
                 0.8.0; Fides-Setup-1.0.0.exe was 2.5 MB, tested on Windows)
  Ships:         fid.exe = self-hosted (release/fid.exe), fidesc.exe =
                 Rust compiler, ark/scribe/veil, stdlib/ and examples/
                 beside them
  Version:       build_info.fd base "1.0.0" → `fid version` prints
                 1.0.0+dev on dev builds; tags: v0.7 (Sep 9, the commit
                 "Fides v1.0.0"), v0.8.0-unstable (Nov 10),
                 v0.8.1-unstable and v0.8.2-unstable (Nov 21),
                 v0.8.3-unstable (Nov 22), v0.8.4-unstable (Nov 23);
                 v0.8.5-unstable being prepared Nov 27 (be36317)

TESTS:
  tests/bootstrap/:  2,131 .fd (Nov 27 evening) — 2,056 at top level (586
                     with a `// expect-error` line; 631 named *_fail.fd),
                     75 in subdirs: at_import, baseline, capability,
                     decl_lib, diag_dup, diagnostics, effects, golden,
                     nattools, race_lib, regions, safety, same_name_lib,
                     spec_lib, trait_conflict
  tests/conformance/: 1,617 .fd, one per rule of reference.md §1–§13
                     (`//! error:` = rejected); _modules/ + known-gaps.txt
  tests/selfhost/:  47 .fd files (verify-selfhost.sh)
  tests/leaks/:     11 programs + .expected (scripts/leak-check.sh)
  tests/nattools/:  75 .fd + 98 .sh machine-layer check scripts
  tests/projects/:  52 .fd — whole-program fixtures (cli, service, game,
                    embed, library, workspace, ...)
  tests/verify/:    49 .fd — bug reproductions, soak/interleave, tv pairs
  tests/tooling/:   14 .fd + 143 .sh (135 *-check.sh) — per-feature tool checks
                    (crash reports, LSP probes, caches, repl, docs, serve)
  tests/ also:      ark, book, c2fd, cancel, cascade, checked_alloc,
                    course, ct, ctx_switch, db, determinism, fmt, fuzz,
                    limits, memprof, mono_share, net, par, perf, prof,
                    rc_cancel, sched, tls, tools, tp, uefi, wasm
  Baseline:         tests/bootstrap/baseline/test_results.txt

STDLIB:
  stdlib/*.fd:    403 top-level modules, 283,248 lines; 414 .fd with the
                  runtimes, 312,667 lines (Nov 27, be36317; 259,299 before
                  the formatter repair, a1bede6) — core/collections, text and
                  data formats, the HTTP/2/3+QUIC and TLS 1.3 stacks,
                  databases (postgres/mysql/mongodb/redis/sqlite),
                  crypto incl. post-quantum (pqc, hpke, jose), wireguard,
                  ssh, noise_proto; industrial and energy (iec104,
                  iec61850, dnp3, modbus, opcua, mqtt, lwm2m, dlms,
                  sunspec, ocpp, iso15118, synchrophasor, microgrid,
                  power_flow, scada, plc); automotive/robotics (can, ecu,
                  bms, mavlink, ros2_msg, dds, arinc, kalman, planner,
                  navmesh, occupancy_grid, lidar, pointcloud); kernel and
                  embedded (e1000, xhci, nvme, ahci, ext4, fat32, vfs,
                  acpi, apic, pci, uart, i2c, spi, gpio, usb_*); games and
                  graphics (ecs, physics3d, tilemap, sdl, d3d12, vulkan,
                  metal, webgpu, gltf, voxel_mesh); media (mp3, vorbis,
                  wav, dsp, spatial_audio); ML (tensor, kv_cache, infer,
                  bpe, safetensors); mobile/desktop interop (jni, objc,
                  swift, android_ndk, webview); finance (money, banking,
                  quant, decimal)
  Runtimes:       stdlib/pe_runtime/ (4 files, 12,258 lines: native
                  Windows, pe_ir) and stdlib/linux_runtime/ (7 files,
                  17,161 lines: nat_ir's Linux targets and --nat Windows;
                  darwin.fd for native macOS, wasi.fd for `fid wasm`).
                  kernel/uefi moved to the fides-os project

TOOLS (tools/*.fd, 42 .fd files, 40,406 lines, Nov 27; 12 on Nov 22):
                  ark, c2fd (C → Fid translator), cas (.fidcache remote
                  server), check_stdlib (import every stdlib module),
                  doctest (run /// Example: blocks), fuzz_triage,
                  macho_check, release_checksums, release_notes, scribe,
                  site (project website served by a Fides program), veil.
                  AD4 ports (Nov 25; docs/reference/internal-tools.md),
                  each with tests/tools/<name>/check.sh: cov_instrument;
                  15 gate/verify tools (expect, leak_check,
                  stale_cache_test, rules_smt, compile_gate, me_measure,
                  nat_gate, pass_matrix, variants, coverage, chaos, soak,
                  interleave, enc_oracle, gen_brotli_dict); 6 fuzz tools
                  (fuzz_gen, fuzz_vs_c, fuzz_frontend, fuzz_parsers,
                  reduce, fuzz_nightly); 4 target runners (cross_test,
                  wasm_test, bare_qemu_test, sanitize); helper libraries
                  py_random, fuzz_lib, fuzz_programs, target_test_lib

RECENT IMPROVEMENTS:
  Nov 27, afternoon (the LLVM roadmap, on two one-stage -O2 builds of the
                  tree, 15:52 and 16:19): SP6 done (its cache gate and the
                  tail-summary gate pass). SP7 done: relink_check.sh 21 ok
                  on x86-64, AArch64 and `--nat` Windows; a function a
                  build inlines away keeps its slot in the kept layout
                  (nat_ir.fd nl_loop_end). LF20 done: the "3 of 8 secret
                  values in registers" was the [lf20] line counting IR
                  values with no machine life; ng_secret_mir follows what
                  selection makes of a secret, 48 of 48 in registers on
                  the four targets, both gates 44 ok and 22 ok. CF32 and
                  CF37 closed (checks.fd ct_source_param: @public
                  parameters by their index in the source; the C path
                  keeps its null pointer checks). Gates fixed and green:
                  machine_mi14_check 33 ok, machine_fregs_check 25 ok,
                  machine_sp16_check 9 ok, wp6_args_check 23 ok (AArch64
                  fwd3 stays: x0-x7 are no pool registers there). Build
                  script: the retry of a failed unit keeps the C flags,
                  SPLIT_OPT picks the first level, the last line is true.
                  Started at 17:04 and recorded in the goal-workflow run
                  log when they end: the whole-suite legs for WP14, LF11,
                  LF16 and LF14 (build/llvm19/suite-*.raw); the gates RA1,
                  LF11 and SP9 wait for them.
  Nov 24–27       (0dcfe61a … be36317; v0.8.5-unstable prepared Nov 27,
                  its fid.exe a one-stage build from a self-hosted stage 0).
                  Checked Nov 25 with a fid built from the 905d756b tree:
                  the six bugs fixed (12/12 bug tests on run, ccompile,
                  emit; 9/9 f32 tests on emit with
                  FID_IROPT_OFF=inline,inline2,inlineclo; 3/3 on --nat).
                  TL2 done: scripts/diag-coverage.py, 467 messages, 100%
                  with a test, a Fix line and a code
                  (docs/reference/error-codes.md, `fid explain-error`);
                  602/602 expected-fail tests print what they expect,
                  365/365 conformance `//! error:` files rejected. TL4 done:
                  lsp_tl4_features 29 ok, lsp_tl4_gaps 5 ok, lsp_probe 17
                  ok, lsp-scope PASS. AD1 done: tools-selfbuild-check PASS
                  (veil vs Rust veil: only line totals differ). TL5 PDB
                  locals (compiler/pdb.fd): `emit --debug -g` shows 25/25
                  values through the DWARF (pe_dbg_run_check), 22/22
                  through the PDB with dbghelp (pe_pdb_locals_check);
                  fid-debug-session-check ok. AD4: 25 internal tools ported
                  to Fid on Nov 25 evening (15 gate/verify, 6 fuzz, 4
                  target runners) plus cov_instrument and 4 helper
                  libraries (TOOLS above). Bugs roadmap: 71 of BG128-BG241
                  marked [x] Nov 25 after running their tests; at be36317
                  80 [x], 3 [~] (BG183, BG204, BG227), 31 open; BG242-BG246
                  found Nov 25, open. Nov 26: constant global initializers
                  folded at lowering (2g.119), extern call arguments cast to
                  the declared prototypes (2g.121), the process runtime in
                  the bootstrap C backend. Nov 27: stage0-native.yml (the
                  native stage 0 seed), the linux-arm64 cross-build seeded
                  from the pinned self-hosted fid, the fid-windows-arm64
                  stage 0 asset pinned; 2g.122 recorded (with a fix design).
                  Nov 27 batch (21/21 bug tests on run, ccompile, emit;
                  4/4 + 2/2 under FID_VERIFY=1 on --nat): the inliners
                  narrow an f64 argument/result to an f32 parameter/return
                  (iropt.fd iro_inline_function, iropt_me.fd
                  ime_inline_site; BG165); a spawn, async fn or parallel
                  branch with more than 8 arguments is rejected with a Fix
                  line (typechecker.fd; BG159); `dyn` of a generic impl
                  with no direct call (mono.fd; BG183, still partly) and an
                  inline `(q as dyn T).m()` (BG184), chained generic calls
                  (BG185),
                  `a?.f` on f64/f32 fields (BG182); a p_json_parse/get/at
                  result passed to a JSON reader is dropped (BG204:
                  test_bg204_json_parse_drop and test_json_runtime_values,
                  every block freed); machine_c_callback_check 0
                  failures. Compile time: the function cache reuses
                  273/273/274 functions after an edit and compiles 2 (was 0
                  reused, 275 compiled; SP6); mono skips bodies it cannot
                  change (mono 33-37 -> 10-17 ms on hello, image identical
                  with FID_MONO_FULL=1); lower.fd's alloca_type index
                  (sp12_scaling 400: ratio 2.97, limit 3.5, was 3.8-4.6);
                  build/emit exit right after the image is written
                  (g_main_fast_exit, FID_NO_FAST_EXIT=1 off); the import
                  closure parsed ahead in parallel under FID_PAR>=2 (opt-in;
                  `dump-c compiler/main.fd` byte-identical with FID_PAR=4;
                  SP9). Self `dump-c` 43.5 s (57 s Nov 21); emit --debug
                  hello wall 1,250 -> 967 ms. In the gate run 3
                  machine_fncache_check checks failed ("mk not given up";
                  be36317 turned that stale line into a note) and 4
                  relink_check (SP7); SP8's 100 ms not met
                  (docs/perf/compile-time-2026-11-27.md).
  Nov 10–19       ("1.0 release preparation", commit 49fb2b9): the tree
                  roughly doubled — compiler 68→80 modules / 159K→361K
                  lines, stdlib 146→403 modules / 51K→259K lines,
                  bootstrap tests 502→1,681 (expected-fail 105→456),
                  conformance tests 104→1,596 (one per reference.md rule,
                  §1–§13). New fid commands: fix (apply suggested fixes),
                  fuzz (fn fuzz_*(Vec<u8>)), reduce + --crash-report
                  --reduce (shrink a failing program), ir-fuzz, serve
                  (long-lived checker), disasm (on .fd and on exes),
                  dump-mir, emit-ir (.fir → machine layer), doc (=scribe),
                  test -run <test>/<row>, parse-header --link. New
                  targets/paths: native WASM (compiler/wasm.fd: 316/344
                  suite tests, task stack switching), native macOS
                  (nat_enc nat_macho + darwin runtime: builds, not run),
                  macOS universal binaries, UEFI images (--uefi),
                  multiboot kernels (--boot), Cortex-M Thumb-2
                  (nat_thumb), aarch64-windows and x86_64-freebsd flags.
                  PGO (run --profile / emit --profile-use), translation
                  validation (--validate), PDB debug info (emit -g),
                  .fidcache AST/IR binary caches (CS2/CS4), `fid test`
                  runs the bootstrap suite itself.
  Nov 7–9:        machine layer (nat_ir/nat_enc): one MIR and allocator for
                  Linux AArch64/RISC-V/x86-64 and opt-in `--nat` Windows;
                  mid-end: iropt passes, iropt_me (opt-in), rules (z3-proved),
                  e-graph, irverify; ownership 0004 layouts (counted strings
                  and Boxes, inline structs in Vec, 40-byte Vec header,
                  HM_HASHED); LF12/13/15/17/19/25; `Vec::filled`; split C at
                  -O2 (csplit.fd, self-compile 74 s); pe_ir task switching
                  and IOCP sockets; tests/conformance and docs/verification.
                  Nov 9: every compiler module re-audited
                  (docs/file-analysis/, open bugs: known-bugs.md §2g).
  Nov 4–6:        fid: generics (0002), if/match values, @cfg, freeing of
                  provably finished locals (C5–C8), K4/K5 borrow checks, K7
                  effects, @send on spawn args (H4), parallel branches (H7),
                  async f64/struct (H9), native per-thread free lists (H8),
                  safety annotations enforced + `fid qualify` + safety-rules.md
                  (L1–L4), owned/borrowed strings (M11), integer-key HashMap,
                  per-call-site higher-order effects, iropt on the native path,
                  core checked once (D9). Copy-on-write written, LOWER_COW off.
  Session 38:     C path: no per-call call-stack push/pop; panic traces from
                  the machine stack (__p_frames / __p_fn_name over the
                  __p_fns table), stack overflow from the OS guard page
                  (SetThreadStackGuarantee, sigaltstack); fib through C 3.0 s
                  -> 0.81 s. Counted-loop check removal shared with C
                  (compiler/checks.fd, pe::unchecked_ops). Vec_push inlines.
                  Native: shadow space once, flags-only compares, fall-through
                  blocks, params in registers, short immediates. One Vec
                  implementation: every backend's p_vec_* runs core's Vec_*.
                  Crypto vectors (NIST, RFC 4231), p_os_random_u64.
  Session 37:     Self-compile 4.4x faster (601 s -> 136 s, fixpoint kept),
                  found with a stack sampler on the stage-3 C. C runtime prints
                  with one fwrite per print (printf on unbuffered stdout wrote a
                  byte at a time on Windows: every printing program was slow);
                  IrFunction.inst_at (id -> instruction, was a linear scan for
                  any function with parameters); IrModule.fn_index/str_index,
                  Lowerer.func_info_at and the comptime pass's name set replace
                  scans by name. Linux CI green: suite, bootstrap, clean install.
                  Tasks wake by deadline; StringBuilder usable after to_str on C.
  Session 30:     Post-codegen constant-time verifier on the x86-64 ELF path:
                  after peephole, `verify_constant_time_asm` scans final asm for
                  conditional/indirect jumps, memory cmov, indexed memory ops,
                  variable-latency ops, and calls to non-CT functions; violations
                  become build errors (test_ct_asm.fd / test_ct_asm_fail.fd).
                  cast_arg in both C codegens now dereferences Ptr(inner)→Generic
                  when inner == the generic type (heals &mut Vec → Vec forwards).
                  cl.exe invocation fixed (cmd /C quote-stripping).
  Session 29:     Reference escape check (src/escape.rs, compiler/escape.fd):
                  references are second-class — cannot be stored, returned from
                  locals, sent to tasks, or captured by escaping closures.
                  Vec<f64> element reads + f64 return conversion fixed on all
                  backends. Fixpoint re-verified (stage2==stage3).
  Session 26-28:  Branch consolidation (release-infra → main), fixpoint
                  re-verified (c4==c5 byte-for-byte), file-analysis/ audit of
                  all compiler/*.fd modules, B/C/D remediation batches.
  Session 23:     6 high-priority language features implemented:
                  @zeroize (F88) — compiler-inserted memset(0) at scope exit
                  @secret (F74) — info-flow control: secrets can't reach output sinks
                  @interrupt (F29) — @interrupt(N) annotation + automatic IDT setup
                  @firmware (F30) — rejects OS-dependent calls in bare-metal code
                  parallel { | } (F13) — fork-join syntax (contextual keyword)
                  @simd (F23) — aggressive loop vectorization (wider vectors, relaxed cost)
                  Test baseline improved: +2 interp, +1 C codegen, 0 regressions
  Session 21:     26 game/media stdlib modules added (sdl, gl, image, font, mesh,
                  gameloop, physics3d, skel, ui, net, camera, particles, animation2d,
                  audio_mixer, asset, serialize, collision_broad, raycast, navmesh,
                  terrain, profiler, replay, shader, editor, chunk, touch)
                  — 9,140 LOC added, 99 stdlib modules total
                  4 runtime bugs documented (global Vec crash, Vec::set overflow,
                  struct return reassignment overflow, Vec<f64> bit patterns)
  Session 20:     12 Rust modules ported (coff, macho, cparser, embedded, tensor,
                  wasm, scribe, veil, hotreload, debugger, native_debugger, gpu)
                  + 7 cross-comp backends (codegen_arm64/riscv, runtime_arm64/riscv,
                  arm64_enc, riscv_enc) — 14,577 LOC added, 50 modules total
  PE stubs:       p_map_keys/values/get_or/clone + p_string_trim/split implemented
  ELF dump:       dump_symbols implemented (symbol table parser)
  @packed:        Packed struct layout (field_align=1, no padding)
  Opaque fields:  GetFieldPtr resolves IrType::Opaque when struct exists
  ELF64 encoder:  Full x86-64 instruction encoder ported (compiler/elf64.fd, 2,423 LOC)
  Interp port:    75 missing interpreter functions ported (compiler/interp.fd, 2,839 LOC)
  Linux runtime:  Native Linux syscall runtime (compiler/runtime_linux.fd, 4,362 LOC)
  Linker:         ELF relocation/linker ported (compiler/linker.fd, 1,051 LOC)
  Sandbox:        Alloc capability enforced (p_malloc/p_free blocked in sandbox)
  Rc/Arc:         stdlib/rc.fd — reference counting (rc_new, rc_clone, rc_drop, etc.)
  @repr(C):       C-compatible struct layout for hardware/FFI/kernel
  Kernel:         stdlib/kernel.fd — MOVED to the fides-os project (Oct 24)
  @cpu_local:     Per-CPU variables via GS segment (compiler/pe.fd; nothing
                  generates it any more: pe.fd's tables are never filled)
  Inline asm:     Clobber lists, 50+ x86 instructions encoded (now
                  pe_ir_stubs `pe_asm_encode`, bare builds only; a Windows
                  build gives 0 for `asm!`)
  --no-std:       Bare-metal mode skips stdlib/core.fd (compiler/compiler.fd)
  UEFI:           stdlib/uefi.fd — MOVED to the fides-os project (Oct 24)
  Cross-comp:     --target arm64/riscv64 validation with C codegen fallback

Binaries:     4 (fidesc, ark, scribe, veil) + the self-hosted fid
```

**Build directories:**
- `target/` — Rust bootstrap compiler output (`fidesc.exe`) — gitignored
- Never commit a build directory. `target3/` (an agent's build folder) went
  into commits f9c55d8/087609a by mistake; it is now in `.gitignore`. Agents
  build in their scratch folder (`CARGO_TARGET_DIR`), not in the repository.
- `release/` — Self-hosted compiler output: `fid.exe` (from the release
  bootstrap) and `fid-dev.exe` (a single stage from
  build-selfhost.sh) — gitignored
- `compiler/` — Self-hosted compiler source (65 `.fd` modules) — THE PRIMARY COMPILER

**Changelog:** See `docs/reference/changelog.md` for the full history of
features and fixes. (Previously duplicated here — removed to avoid maintaining
the same information in two places.)

**Before making any change:** run `./verify.sh` to confirm you're starting from a known-good state.

## HIGH-RISK FILES — Always run `./verify.sh` after editing these

These files are where silent errors are most dangerous. Edits here can break
everything with no warning. Always run the full verification suite after
touching them.

| File | Risk | Why |
|------|------|-----|
| `src/regalloc.rs` | **CRITICAL** | Linear scan allocator (~1,500 lines). F-1A: constant rematerialization (`Location::Remat`). F-1D: effect-aware register allocation — calls to pure functions (in `L3Module.pure_functions`) only clobber RAX, not all caller-saved registers. I-2D: calls to `@no-alloc` functions (in `L3Module.no_alloc_functions`) also only clobber RAX. `@medical` functions included in no-alloc set. S-17: deterministic interval sorting (secondary key: vreg ID) to prevent non-deterministic codegen from HashMap iteration order. Any change here affects EVERY function. |
| `src/peephole.rs` | **CRITICAL** | 9 passes, always on in `build`. A wrong transform silently corrupts all output. |
| `src/elf64.rs` | **HIGH** | ELF64 header bytes, section layout, relocations (R_X86_64_PC32/PLT32/64). Off-by-one = corrupt binary. Self-hosted port in `compiler/elf64.fd` (2,423 LOC and 85% parity when ported; 3,629 lines on Nov 24) includes full x86-64 instruction encoder, assembly parser, and ELF builder. |
| `src/pe.rs` | **HIGH** | PE header bytes, import table (kernel32.dll + msvcrt.dll: every name in `IMPORTS`/`MSVCRT_IMPORTS` must be a real export, or no program starts), section layout. Off-by-one = corrupt .exe. Machine-code stubs must not keep a value in a volatile register (rcx, rdx, r8–r11) across a call (HeapFree clobbers them). |
| `stdlib/linux_runtime/*.fd` | **MEDIUM** | The runtime of the machine layer (compiler/nat_ir.fd): Linux system calls (base, conc, math, sys) and `win.fd` for `--nat` Windows over kernel32/ws2_32. Merged into every nat_ir build. |
| `compiler/nat_ir.fd`, `compiler/nat_enc.fd` | **HIGH** | Machine IR, register allocator and encoders of the Linux targets and `--nat`. Checks: `FID_VERIFY=1`, `scripts/enc-oracle.py`, `scripts/pass-matrix.sh`. |
| `compiler/pe_ir*.fd` | **HIGH** | Default Windows native backend: machine code, imports, PE writer. Same byte-level risk as `src/pe.rs`. |
| `stdlib/pe_runtime/*.fd` | **MEDIUM** | The Fides runtime of native Windows executables (pe_ir): builtin `p_X` and `__X` run `pert_X` (base, conc, tensor, data); `pert_c_<C name>` overrides a C function. Merged into the Windows `emit`/`build` (also `build --native --target windows`), not into `--bare`, `--boot`, pe_ir's ELF path (`FID_ELF_PEIR=1 --elf`) or C; `data.fd` and `tensor.fd` are also merged into every nat_ir build (Linux targets, `--elf`, `--nat`). A type error here breaks those builds. `p_vec_*` never reaches `pert_*`: `pe_runtime_alias` sends it to core first. Helpers prefixed `pe_`/`pet_`/`ped_` (unused ones are left out of the .exe). |
| `src/coff.rs` | **HIGH** | COFF object file format. Same byte-level risk as PE/ELF. |
| `src/macho.rs` | **HIGH** | Mach-O object file format. Same byte-level risk. |
| `src/codegen_x64.rs` | **HIGH** | PIR L3 → x86-64 instruction encoding. ModRM/SIB bytes, REX prefixes, ABI rules. |
| `src/codegen_c.rs` | **HIGH** | IR → C99. ABI casts, struct offsets, function renaming. 343/343 test parity (Nov 2). Phase 10B: Print instruction calls Fides Vec_to_str/HashMap_to_str for formatting. Channels: p_chan_* with pthread mutex+condvar. Binary hardening: stack canaries, PIE/ASLR, RELRO, NX stack. Stack traces on panic. Rc/Arc runtime via p_rc_incr/p_rc_decr. ML3: Autograd tape (ag_tape, ag_record, p_autograd_backward) with reverse-mode AD for add/sub/mul/matmul/scale/relu/sigmoid/sum/mean. ML11: Checkpoint mode stores input data for relu/sigmoid backward. ML12: DP noise/budget builtins. ML13: TP builtins (identity in single-rank). ML9: KV cache with sliding window eviction. ML10: Quantization (quantize/dequantize/qmatmul/find_quant_params). Tensor printing via p_tensor_to_str with %.4f for non-integers. Dlopen simulated to match interpreter (sequential handles). |
| `src/pir.rs` | **HIGH** | PIR L1/L2/L3 transforms. A bad lowering propagates to all backends. |
| `src/lower.rs` | **MEDIUM** | AST → SSA IR. Changes here affect interpreter + all backends. `runtime_effects()` helper sets Write/Alloc/IO effects on `p_*` builtin calls — without this, the optimizer DCE's side-effecting calls. `p_vec_*` extern declarations (23 functions) ensure correct calling convention. |
| `src/typechecker.rs` | **MEDIUM** | Types + regions + effects + capabilities. Wrong rejection = broken code; wrong acceptance = silent miscompilation. |
| `src/borrowck.rs` | **LOW** | Borrow checker (Phases 1-6 complete). Separate pass after typecheck. `--borrowck=on` by default; `off`/`warn` modes available. Field-wise borrows, two-phase borrows, NLL (non-lexical lifetimes via last-use analysis). |
| `src/opt.rs` | **MEDIUM** | PIR optimizer. 23 passes: const-fold, CSE, GVN, SROA, DSE, DCE, inline, unroll, TCO, alias-analysis, effect-reorder, partial-eval, vectorize, strength-reduce, devirt, jump-thread, loop-rotate, loop-vec, effect-opt, region-pool, reassociate, memoize. 22 in default pass list (region-pool via `--region-pool`). `--opt` is enabled by default in `build` (use `--no-opt` to disable). `--lto` enables aggressive cross-module inlining (larger inline limits). PGO has been removed. S-1A pass skipping: `pass_relevant_for_module()` skips passes that can't help (no loops → skip unroll, no calls → skip inline, etc.). S-17: all instruction traversal functions now handle `L2Inst::Copy` (was missing, causing SROA+CSE to produce dangling references). Inline cost model (F-3F): cost = blocks×5 + insts, with `is_pure` (50% bonus), `is_crashfree` (25%), `is_deterministic` (10%), capped at 75%. Sandbox-aware inlining (I-3C): sandbox call sites use 6 blocks / 40 insts vs normal 3/20. Sandbox-aware devirt (I-3C): devirtualize DynCalls in sandbox blocks to pure impls. Region bulk free (I-1C): `pass_region_pool` emits `p_region_free` before returns. @crashfree code hoisting (I-4B): `pass_effect_optimize` hoists @crashfree + effectively-pure calls out of loops. @gas-guided unrolling (I-4C): `pass_unroll` uses max_unroll=64 when `gas_limit.is_some()` (vs normal 8). Annotations propagated: `is_wrapping`, `is_deterministic`, `is_crashfree`, `is_no_alloc`, `is_medical`, `gas_limit`. I-6B linear type alias analysis: `linear_structs` propagated from AST → IrModule → L1Module → L2Module; `alias_analysis_function` tracks `linear_ptrs` and uses them to prove non-aliasing (stores through linear pointers can't invalidate loads from other pointers). |
| `src/comptime.rs` | **LOW** | Comptime evaluation engine. Evaluates `comptime <expr>`, `comptime { block }`, `comptime reflect_op(T)`, `comptime gen { block }` at compile time. I-5A: Aggressive comptime evaluation — auto-evaluates calls to `comptime fn` functions with all-literal args (no explicit `comptime` wrapper needed). I-5D: Comptime loop unrolling — unrolls `for` loops with literal bounds (≤16 iterations, ≤16 body stmts) at AST level. Also eliminates dead `if` branches with literal conditions. |
| `src/runtime_linux.rs` | **HIGH** | Hand-encoded x86-64 machine code (raw bytes). No assembler — every byte is manual. |
| `src/linker.rs` | **MEDIUM** | Relocation resolution. Wrong offset = crash or silent corruption. |
| `src/interp.rs` | **MEDIUM** | The reference oracle (~6,950 lines). If this is wrong, all backends "match" but produce wrong results. `p_vec_new` uses `vec_side_table` to separate raw struct fields from `Value::Vec`. `run_function` auto-detects missing `__env` param and inserts dummy. Self-hosted port in `compiler/interp.fd` (7,456 lines on Nov 9; 14,992 on Nov 24) ran every test program `fidesc run` did (Oct 27); it cannot run FFI other than a few libc calls or `sysconf`; tasks interleave (LG11, docs/reference/interp-tasks.md). |

## LOWER-RISK FILES

| File | Risk | Why |
|------|------|-----|
| `src/lexer.rs` | LOW | Tokenizer — errors are usually loud (parse failures). |
| `src/parser.rs` | LOW | Recursive descent — errors are loud. |
| `src/ast.rs` | LOW | Data definitions — changes propagate visibly. |
| `src/token.rs` | LOW | Token enum — adding tokens is safe. |
| `src/mono.rs` | LOW | Monomorphization — errors surface as missing functions. |
| `src/comptime.rs` | LOW | Compile-time eval — errors are loud. |
| `src/main.rs` | LOW | CLI dispatch — adding commands is safe. |
| `src/compiler.rs` | LOW | Pipeline orchestration — changes are visible. Phase 12: `enrich_errors()` post-processes errors with source context + error codes. |
| `src/diag.rs` | LOW | Diagnostic system — structured error/warning/note/help with error codes, source context, color. Zero external deps. |
| `src/lsp.rs` | LOW | LSP server — JSON-RPC over stdio, no external deps. |
| `stdlib/*.fd` | LOW | Fides source — errors surface as test failures. 414 modules (~313K lines): core/collections, text/data formats, HTTP/2/3+QUIC, TLS 1.3, databases, post-quantum crypto, industrial and energy protocols, automotive/robotics, kernel drivers and embedded peripherals, games/graphics, media, ML, mobile/desktop interop, finance — grouped list in `AGENTS.md` → Current Baseline → STDLIB and in `INDEX.md`. Runtimes in `stdlib/pe_runtime/` and `stdlib/linux_runtime/` (incl. `darwin.fd`, `wasi.fd`). (kernel.fd + uefi.fd moved to the fides-os project.) |

## Known Bugs (Do Not Re-Introduce)

> **Critical Blockers:** See `docs/roadmap/roadmap.md` § Issues & Debates → "Critical
> Blockers" section for the highest-priority issues that must be fixed
> before production use:
> 1. CB1: Safety annotations — ✅ in `fid` (Nov 4): every rule enforced, each with a negative test (docs/reference/safety-rules.md)
> 2. CB2: Generics — ✅ in `fid` (Nov 3–4, proposal 0002): inference or an error, bounds checked, turbofish; `fidesc` is frozen without it
> 3. CB3: Backend test parity — ✅ resolved (Nov 2: 339/339 interp, 343/343 C, 345/345 PE)
> 4. CB4: Parser debug prints — ✅ resolved (Session 19)
> 5. CB5: Self-hosted compiler build fails — ✅ resolved (Session 20, all 50 modules type-check)
> 6. CB6: PE emitter stubs — ✅ resolved in the old PIR emitter (pe.fd), which is no longer the native path
> 7. CB7: ELF64 symbol table dump — ✅ resolved (Session 20)
> 8. CB8: 3-stage bootstrap from fidesc dies at stage 2 (out of memory, known-bugs 2g.79) — open (Nov 20); a self-hosted stage 0 avoids it

1. **Self-hosted compiler: what still differs from `fidesc`** (Oct 27).
   `run` matches on all 302 test programs and all 10 examples; `ccompile`
   on 242 of 243 test programs and 9 of 10 examples; all 59 expected-fail
   tests are rejected. The rest:
   - `test_sqlite` needs libsqlite3 and `examples/minimal_game` needs SDL2
     (the Rust compiler fails the same way without them).
   - `build --native` went to the old PIR path (compiler.fd
     `build_executable`: opt, codegen_x64, elf64, linker), which crashed in
     opt.fd `pass_jump_thread`. `build_executable` was removed on Nov 17
     (MI18): `--native` is now the machine layer (nat_ir) on every target,
     the default `x86-64` being `x86_64-linux`; a target it has no code for
     is an error. `fid emit` (pe_ir on Windows, nat_ir for Linux targets) is
     the working native path. compiler/pe.fd was deleted with the retired
     PIR path; `emit --boot` builds the bootable image through pe_ir, as `--bare`
     does (`PEIR_BARE_BOOT`).
   Fixed Oct 25–27 (docs/reference/known-bugs.md §2e): block jumps ended
   functions, byte loads read side-table strings, builtins shadowed core
   functions, the comptime loop unroller, String/HashMap result types,
   stdlib lookup from an installed location, `ccompile` without `CC`, a
   second `impl` block in an imported module was dropped (`merge_programs`
   aliasing), @conserve checked only effects, `extern "C" link` libraries
   not linked.

2. **Nondeterministic self-hosted typecheck/borrowck**: regenerating
   `build/selfhost/fid-dev.c` from identical `compiler/*.fd` input can fail with
   *different* errors per run (e.g. `Vec<i64>` vs `Vec<Vec<EscVar>>`
   assignment, or a reference-escape error) or succeed. HashMap iteration
   order in the self-hosted typechecker/inference is the suspected source.
   Retry the build before assuming a regression. (Not reproduced on
   Oct 25: five 3-stage bootstraps all type-checked and reached the
   fixpoint.)

3. **`cmd /C` quote stripping** (`src/main.rs`): the `vcvarsall.bat && cl.exe`
   link path failed on Windows because `cmd /C` strips the first quote when
   the command string begins with `"` and contains `&&`/`>` — `cl.exe` then
   ran as `'C:\Program'` and produced empty stderr. **Fixed:** `cmd_str` is
   wrapped in an outer quote pair.

4. **`Vec` argument ABI at `cast_arg`** (`src/codegen_c.rs`,
   `compiler/codegen_c.fd`): when a `Ptr(inner)` value (e.g. an
   `&mut Vec<T>` parameter slot) is passed to a `Generic`/`Opaque` parameter
   of exactly that inner type, the caller must emit `*(void**)` to load the
   handle. Without the deref, `Vec_len`/`Vec_get` read the slot pointer as a
   header → garbage lengths and crashes (seen in `record_def` →
   `ra_vec_find_i64`). The deref fires **only** when the pointee type equals
   the destination type — a broader version dereferenced `str` (which is
   `*u8`) passed to generic params and corrupted `Parser_new`.

5. **Post-codegen constant-time verification** (`src/codegen_x64.rs`):
   `verify_constant_time_asm` runs on the x86-64 ELF path after peephole. It
   rejects `jcc`/indirect jumps, `cmov` with memory operands, indexed
   memory, variable-latency ops, and calls to non-`@constant_time` functions
   in annotated code. `@wrapping` exempts generated overflow branches.
   The recursion-guard and stack-canary sequences are exempt. **x86-64 ELF
   only** — the PE path emits machine code directly and is not yet covered;
   ARM64/RISC-V are not covered. (That is fidesc. In fid, `@constant_time`
   code is checked on pe_ir's and nat_ir's final machine code (J7) and, in
   the C generator, on the IR it is handed; FID_PE_CT_OFF / FID_C_CT_OFF
   turn the pe_ir and C checks off.)

6. **Session-16 regression — bare runtime stubs**: the `p_hlt`/`p_cli`/
   `p_sti`/`p_inb`/`p_outb`/`p_serial_*` stubs lost in the
   `git checkout -- compiler/pe.fd` incident (known-bugs.md §1.37) were
   re-added. `emit --bare` now builds through pe_ir_stubs
   (`PeirGen_gen_bare_runtime`), not pe.fd: check the bare stubs there.

7. **Register allocator clobbering** (`regalloc.rs`): Previously, constants not
   loaded into registers before use could be clobbered by intervening
   instructions. **Fixed by F-1A (Constant Rematerialization):** constants are
   now rematerialized at each use site via `Location::Remat(i64)` instead of
   being spilled and reloaded. The L2 optimizer's `effect_reorder` pass was also
   fixed to respect memory dependencies and keep terminators last. Test with
   `--no-opt` when changing regalloc to isolate issues.

8. **Debug `eprintln!` statements**: Temporary debug prints (DBG ASM, DBG ALU,
   DEBUG gen_store, DEBUG struct_layouts) were added during debugging sessions
   and have been removed. Do not re-add them.

9. **PIR L2 GetFieldPtr offset** (`pir.rs`): Previously used `field_index * 8`
   as the byte offset, which was correct when all struct fields were 8 bytes.
   **Fixed (SH-6.1):** Now looks up real offsets from `struct_layouts` using a
   `value_types` map built during L2→L3 lowering. This was necessary because
   enum-typed struct fields need `(1 + max_payload) * 8` bytes, not 8.
   `compute_struct_layout()` in `lower.rs` now uses enum sizes for enum-typed
   fields, and the PIR L2→L3 lowering looks up the real offset.

10. **Native codegen bugs** (not optimizer-related): All previously failing
   native ELF64 tests (`test_collections`, `test_db`, `test_match_range`,
   `test_matrix_mul`) are now FIXED. The root cause was a single wrong REX
   prefix byte in `p_json_at` (`0x49` instead of `0x4C` — see changelog for
   details). The `--opt` flag produces identical output to non-opt for all
   tests except 3 closure tests that are broken in both paths.

11. **Non-deterministic register allocation** (`src/regalloc.rs`): The linear
   scan allocator sorted live intervals by start point only. When multiple
   intervals shared the same start, HashMap iteration order (non-deterministic
   due to random seeding) determined their relative order, producing different
   register/stack-slot assignments on each run. **Fixed:** Added `i.vreg.0` as
   a secondary sort key: `sorted.sort_by_key(|i| (i.start, i.vreg.0))`. The
   self-hosted compiler (`compiler/regalloc.fd`) was already deterministic
   (uses Vec, not HashMap).

12. **SROA + CSE dangling reference** (`src/opt.rs`): SROA replaces struct
   field loads with `Copy` instructions. CSE then eliminates duplicate
   constants and calls `replace_all_refs` to update references — but
   `replace_refs_in_inst` did not handle `L2Inst::Copy`, leaving dangling
   references after DCE removed the original constant. This caused
   zero-valued struct fields to print garbage in optimized native builds.
   **Fixed:** Added `L2Inst::Copy { value, .. }` to all instruction traversal
   functions in `src/opt.rs` (`replace_refs_in_inst`, `apply_replacements`,
   `remap_inst`, `inst_references`, `is_pure`, `is_pure_inst`, `cse_key`,
   `gvn_key`). The self-hosted compiler (`compiler/opt.fd`) already handled
   `Copy` correctly — the bug was only in the Rust bootstrap.

13. **Stack canary / float scratch overlap** (`compiler/pir.fd`,
   `compiler/codegen_x64.fd`): PIR frame allocation started at offset 0,
   causing the first alloca slot to overlap with the canary at `-8(%rbp)`.
   Float conversion also used `-8(%rbp)`, overlapping the canary.
   **Fixed:** Frame size starts at 16 bytes (8 canary + 8 float scratch) in
   `compiler/pir.fd`. Float scratch moved to `-16(%rbp)` in
   `compiler/codegen_x64.fd`.

14. **Slow paths that look harmless** (`compiler/ir.fd`, both C runtimes):
   do not print through `printf` in the C runtime (stdout is unbuffered: one
   write per byte on Windows; use `fwrite`), and do not look up instructions,
   functions or strings by scanning a list: use `inst_at`, `fn_index`,
   `str_index`. A function's parameters take value ids without instructions,
   so `id == index` does not hold. Any code that replaces `module.functions`
   wholesale must `HashMap_clear(module.fn_index)`.

15. **Per-call bookkeeping in generated C** (both C generators): do not
   bring back `p_call_stack_push`/`pop` or a recursion counter. Traces come
   from the machine stack (`__p_print_trace_ctx`); overflow from the guard
   page. `zig cc` off Windows needs `-lunwind` for `_Unwind_Backtrace`
   (main.rs, main.fd, bootstrap.sh, build-selfhost.sh add it).

16. **One `Vec` implementation** (`pe_runtime_alias`, in `compiler/ir.fd`
   since Nov 9; `interp.rs core_vec_fn`): every native
   `p_vec_*` builtin (pe_ir and nat_ir) runs core's `Vec_*` function.
   compiler/interp.fd keeps its own `p_vec_*` over simulated memory, and
   pe_ir still emits its old machine `p_vec_*` stubs (unused). Do not add a backend stub with its own semantics; the old ones
   disagreed (`clone` shared data, `first` returned garbage). The C runtime's
   `PVec` functions share core's layout and must keep doing so.
   `test_vec_builtins.fd` checks every backend agrees.

17. **The counted-loop analysis lives in `compiler/checks.fd`**, not in
   `pe_ir.fd`: `codegen_c.fd` importing `pe_ir` doubled the C of any program
   that uses the C generator (`test_codegen_c` 5 MB -> 11 MB).

18. **Testing a runtime `str` for null**: write `(s as i64) == 0`. `s == 0`
   on a `str` is a text comparison in `fid`, so it never detects the null
   that `p_file_read`, `p_getenv` and `p_argv` return on failure.

19. **fidesc bakes `stdlib/core.fd` in** (`include_str!` in
   `src/compiler.rs`): after editing core, rebuild `fidesc` before testing
   with it, or it runs the old core. `fid` reads core at run time.

20. **The debug allocator** (`P_LEAKS`, `P_CHECKED_ALLOC` in
   `compiler/codegen_c.fd`) renames the block allocator to
   `__p_block_malloc`/`__p_block_free` with `#define` and puts recording
   `p_malloc`/`p_free` after it. Keep code that must call the real block
   allocator between those defines, and nothing else.

21. **A plain string is not a headered one** (`stdlib/core.fd`
   `p_str_free`, `p_str_length`): a length-headered string has
   `P_STR_MAGIC` at `s - 16` and its length at `s - 8`. A plain `p_malloc`
   string can sit right after a block whose last word happens to be the
   magic (the compiler's own IR of core.fd holds it as a constant), so the
   magic alone is not enough: `p_str_len_word` also requires the word at
   `s - 8` to be a plausible length (a block header is negative). Without
   it, natively built `fid` freed `s - 16` ("Invalid free").

22. **The C runtime frees what it allocated with the same allocator**:
   strings from `p_malloc` (`p_to_str_float`, `p_file_read`, `p_dir_list`,
   `p_string_split`, `p_to_str_vec`, `p_to_str_map`) go back through
   `p_free`, never `free()` (`p_print_float` did: a crash after the second
   float printed).

23. **Ownership is one rule now** (proposal 0004, `LOWER_RC` in
   `compiler/lower.fd`, section "Ownership"): a variable owns its value and
   is dropped at its scope's end; reading borrows; keeping a value (let,
   assignment, field, element, return, payload) takes an owner
   (`Lowerer_rc_own_value`: a temporary is handed over, anything else gets
   `Str_dup`/`Vec_share`/`HashMap_share`/`__own_dup_N`); temporaries are
   dropped at the end of their statement (`Lowerer_lower_stmt` wrapper);
   parameters are borrowed (one the body changes is copied at entry).
   Copy-on-write (`LOWER_COW`) is retired (CM3) and the auto-drop passes
   (`lw_auto_drop_vecs/strs`) run only with `LOWER_RC` off: do not bring
   either back with it, they would free values twice. Do not
   store a str/Vec/map anywhere without going through these helpers; a
   value cast to `i64` (`f() as i64`) is the raw word's to free by hand.

24. **Field addresses come from a pointer typed as the struct**
   (`Lowerer_emit_struct_share`): a pointer typed as what holds the struct
   (an enum's payload) resolves field indexes against the holder's layout.
   Bit-cast to `Ptr(Struct)` first. Likewise word offsets of fields come
   from the layout's `offset`, not from adding up field sizes: an enum
   field is `(1 + payload)` words.

25. **Layouts (proposal 0004)**: a heap string is `[P_STR_RC + count][len]`
   bytes NUL (core `p_str_alloc`, C `p_str_alloc_c`, native concat stub),
   a literal `[P_STR_STATIC][len]`; every runtime string producer returns a
   counted string (C: `p_str_own_c` wrappers; native: `p_str_adopt`). A Vec
   header is 40 bytes `{data, len, cap, code, tid}`, a HashMap 56
   `{.., types}`; every creator (core, C `PVec`, interpreter `p_vec_new`,
   C fast bodies) must make the full header. A shared buffer's box is
   `{owners, cap}`; a buffer core allocates (`vec_buf_alloc`) is
   `[P_VBUF_KEY + data][owners][cap]` data..., so its first share uses that
   room: free a Vec buffer with `vec_buf_free` (C `__p_vbuf_free`) and a box
   with `vec_box_free`, never `p_free`. Element codes 16 and up are structs
   held in the buffer, `code - 15` words each, reached by address
   (`vec_ld`); structs enter a Vec through `Vec_push_mem`/`Vec_set_mem`/
   `Vec_insert_mem` (`LOWER_INLINE_STRUCTS`). A Box is `[P_BOX_RC +
   owners]` then what it holds, the Box the address after the count
   (`LOWER_BOX_RC`; `Box_dup`, `Box_drop_t`); a Box's element type is
   `P_TID_BOX + ` what it holds. HashMap types word bit `HM_HASHED`: the
   values block holds each slot's hash after the `cap` values.
   `p_file_read`, `p_getenv`, `p_argv`, `p_read_line`,
   `p_read_stdin` are raw `i64` (0 on failure): use `read_file`, `env_var`,
   `arg_at`, `read_line`, `read_stdin` (Option) or `p_str_take`. Gate:
   `scripts/leak-check.sh` (tests/leaks: output and "every block was freed").

26. **Another agent may be editing `core.fd`/`lower.fd`**: build from a
   snapshot of `compiler/`, `stdlib/` and `scripts/` when files change
   during a build, or the stage-3 C differs only because a file changed
   between the stages.

27. **The Nov 9 module audit** (docs/file-analysis/<module>.md for all 68
   modules, docs/reference/known-bugs.md §2g). Fixed Nov 15: the split C's
   `static` locals (2g.1), SCCP on narrow/unsigned types (2g.11), NaN in
   branch facts (2g.12), `fid veil`/`fid lsp`/`fid fmt` (2g.2, 2g.5, 2g.6),
   and 2g.3, 2g.7–2g.10, 2g.14, 2g.18, 2f.11. Still open in §2g: see the
   table. Two traps the fixes rely on: a struct literal's type for
   lowering stays `i64` unless the struct is generic (other lowering
   decisions depend on it: changing it broke the compiler's own parser), and
   a function parameter has no instruction, so its type is in
   `func.params`, not `IrFunction_get_meta` (`peir_operand_ty`,
   `ng_meta_ty`).

28. **Gate scripts and `wsl.exe`** (tests/nattools, Nov 27): wsl.exe is on
   PATH on every Windows, with no distribution installed too, and a gate
   that takes it for a working WSL reports every Linux image as "output
   differs". Test it first: `wsl.exe -e sh -c 'echo wsl-ok'`. A path
   written to `fid serve` on its stdin is converted by no shell: give it
   `cygpath -m` paths (relink_check.sh).

29. **`scripts/build-selfhost.sh` under `set -u`** (Nov 27): a variable a
   new line reads must exist on every path (a summary line's list stopped
   every successful build before the link), and the retry of a failed unit
   must compile with the same flags as the first build (the retry had
   neither `-w` nor the `-Wno-*`: int-to-pointer conversions in the
   generated C were errors). Run the script to its end, the success path
   included, before committing a change to it.

30. **A CF32 mark is read by the parameter's index in the source**
   (compiler/checks.fd `ct_source_param`, Nov 27): the IR the C generator
   checks is as lowered, with the result pointer of a struct-returning
   function and the environment in front of the source's parameters; the
   native paths drop the environment (iro_env_trim). An IR check that takes
   the IR's index for the source's looks one parameter too far on the C
   path and on the native paths whenever the environment stays.

## NEVER DO THESE (Hard Rules)

1. **NEVER run `git stash`, `git checkout -- .`, `git reset --hard`, or any
   command that discards/hides uncommitted work.** This destroys the user's
   in-progress changes. To compare against a baseline, use `git diff` or a
   separate worktree (`git worktree add`). If you need to test "before" state,
   ask the user — do not touch the working tree.

2. **NEVER force-push, rewrite history, or delete branches** without explicit
   user confirmation for that specific action.

3. **NEVER run destructive shell commands** (`rm -rf`, `truncate`, bulk
   deletes) without describing exactly what will happen and waiting for
   approval.

These rules apply to ALL sessions and agents. No exceptions.

## Coding Conventions

1. **Codegen output**: use the `wln!` macro for formatted output in
   `codegen_c.rs` (defined at top of that file).

2. **Error handling**: return `Result<_, Vec<String>>` for compiler errors
   (multiple errors collected). Don't use `panic!` for user-facing errors.

3. **Test naming**: `test_<feature>.fd` for feature tests, `test_pe_<feature>.fd`
   for PE-emitter-specific tests. Keep the skip list in `tests/harness.rs`
   updated when adding tests for unimplemented features.

4. **Adding a new test**: create `tests/bootstrap/test_<name>.fd`, run `./verify.sh --save`
   to update the baseline. If the test exercises a not-yet-implemented feature,
   add it to `is_skip_test()` in `tests/harness.rs`.

5. **Adding a new backend stage**: `scripts/gen_golden.sh` and
   `scripts/bisect.sh` do not exist yet — when they are written, add a
   golden-file capture and a bisect check there. Until then use the
   `dump-*`/`codegen`/`pir` commands to inspect stages manually.

## Debugging Workflow (When Tests Break)

1. **Don't panic.** Run `./verify.sh` to see exactly which tests regressed.

2. **Bisect the layer.** There is no `scripts/bisect.sh` — bisect manually:
   ```bash
   ./target/release/fidesc dump-ir  tests/bootstrap/<name>.fd   # AST→IR
   ./target/release/fidesc dump-pir tests/bootstrap/<name>.fd 3 # L1→L3
   ./target/release/fidesc codegen  tests/bootstrap/<name>.fd --target x86-64 --peephole
   ```
   This tells you whether the regression is in lowering, PIR, codegen, or
   peephole.

3. **Check if it's a real regression or a "was passing for the wrong reason".**
   Compare the failing test's golden output (`tests/bootstrap/golden/`) to the
   new output. If the new output is *correct* and the golden was *wrong*, the
   test was passing by accident — update the golden file.

4. **Make the smallest possible fix.** Don't refactor while fixing. One change,
   one commit, one `./verify.sh` run.

5. **If you must make a large change**, gate it behind a flag:
   ```rust
   const NEW_BEHAVIOR: bool = true;  // flip to false to test old path
   ```
   Then test both ways before removing the flag.

## What To Do Before Every Session

```bash
# 1. Confirm baseline
./verify.sh

# 2. If baseline doesn't match, someone changed something — investigate before
#    building on top of a broken state.

# 3. If you're starting fresh and there's no baseline:
./verify.sh --save
```

## What To Do After Every Change

```bash
# 1. Run verification
./verify.sh

# 2. If regressions: bisect the layer manually with dump-ir / dump-pir /
#    codegen --peephole (scripts/bisect.sh does not exist)

# 3. If no regressions and tests improved: update baseline
./verify.sh --save

# 4. If you changed codegen/regalloc/peephole/elf64/pe: regenerate golden
#    files (scripts/gen_golden.sh does not exist yet — capture dumps by hand
#    into tests/bootstrap/golden/)
```

## CRITICAL RULES — NEVER DO THESE

1. **NEVER run `git stash`.** This can lose the user's work. Ever. No exceptions.
   If you need to compare against original code, use `git diff` or `git show`
   on specific files — never stash the working tree.

2. **NEVER run `git checkout` on files the user has modified.** This overwrites
   their changes. Use `git diff` to see what changed instead.

3. **NEVER run `git reset --hard` or `git clean`.** These are destructive and
   will wipe uncommitted work.

4. **NEVER force-push or rewrite git history.** No `git push --force`,
   no `git rebase` that rewrites shared commits.

5. **NEVER delete files you did not create.** If a file is in the way, ask the
   user — don't `rm` it.

6. **NEVER use `rm -rf` on project directories.** Ever.

If you need to see the original state of a file, use `git show HEAD:<path>`
or `git diff <path>` — these are read-only and safe.

## File Map (Quick Reference)

The self-hosted compiler's modules, grouped by stage with line counts and
state, are in INDEX.md ("Self-Hosted Compiler") and
docs/reference/architecture.md ("Self-hosted pipeline"). The table below is
the frozen Rust compiler.

| File | Role |
|------|------|
| `src/lexer.rs` | Tokenizer |
| `src/parser.rs` | Recursive descent parser |
| `src/typechecker.rs` | Types + regions + effects + capabilities |
| `src/borrowck.rs` | Borrow checker (alias analysis, `--borrowck=off\|warn\|on`) |
| `src/ir.rs` | Single-level SSA IR |
| `src/lower.rs` | AST → SSA IR |
| `src/pir.rs` | Multi-level PIR (L1/L2/L3) |
| `src/opt.rs` | PIR optimizer (enabled by default in `build`, `--lto` for aggressive inlining) |
| `src/peephole.rs` | 9-pass peephole (always on in `build`) |
| `src/regalloc.rs` | Linear scan register allocator (Known Bugs 7 and 11: both fixed) |
| `src/mono.rs` | Generic monomorphization |
| `src/comptime.rs` | Compile-time eval + reflection |
| `src/pe.rs` | PE emitter (Windows) |
| `stdlib/pe_runtime/*.fd` | Fides runtime of the PE backend |
| `src/elf64.rs` | ELF64 emitter (Linux) |
| `src/macho.rs` | Mach-O emitter (macOS) |
| `src/coff.rs` | COFF emitter (Windows) |
| `src/linker.rs` | Integrated linker |
| `src/runtime_linux.rs` | Hand-encoded Linux runtime (raw syscalls) |
| `src/codegen_x64.rs` | PIR L3 → x86-64 assembly |
| `src/codegen_arm64.rs` | PIR L3 → ARM64 assembly |
| `src/codegen_riscv.rs` | PIR L3 → RISC-V assembly |
| `src/codegen_c.rs` | IR → C99 source |
| `src/wasm.rs` | PIR → WebAssembly |
| `src/gpu.rs` | PIR → SPIR-V |
| `src/embedded.rs` | FixedBufferAllocator, RealTime effect, HAL |
| `src/hotreload.rs` | Native JIT + function-level change detection |
| `src/cparser.rs` | C header parser → Fides FFI bindings |
| `src/compiler.rs` | Pipeline orchestration |
| `src/lsp.rs` | LSP server (JSON-RPC over stdio) |
| `src/main.rs` | CLI entry point |
