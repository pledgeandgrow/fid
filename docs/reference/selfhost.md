# Self-Hosted Fides Compiler

> **Status (Nov 9, 2026):** the self-hosted compiler `fid` (`compiler/*.fd`,
> 68 modules, 158,516 lines; 60 reached from `main.fd`) is the release
> compiler and defines the language ([reference.md](reference.md)); the Rust
> compiler `fidesc` (`src/*.rs`, 57 files, 112,598 lines) is frozen and builds
> stage 1 and runs the test harness. `RELEASE=1 CC="zig cc"
> ./scripts/bootstrap.sh` builds `fid` in three stages, each stage's C split
> into 8 units compiled at -O2 when the previous stage has `fid split-c`,
> checks that stages 2 and 3 emit identical C, installs `release/fid.exe` and
> runs a program from an installed layout; the installer ships it as
> `fid.exe`. The fixpoint was last reached on Nov 8 (67 MB of C, with the
> ownership layouts of proposal 0004); later changes were checked on
> one-stage builds. `fid` compiles itself to C in 74 s when built at -O2
> (229 s at -O0); `fidesc` takes ~21 s.
>
> The last full suite run is Nov 2 (Rust harness, `fidesc` as compiler:
> interp 339/339, C 343/343, PE 345/345, expected-fail 89/89); it has not
> been rerun with `fid` as the compiler. Native suites on Nov 9: Linux
> AArch64 273, RISC-V 271, x86-64 272 of 317 (`NATIVE=1
> scripts/cross-test.sh`), Windows `--nat` 301 of 338.
>
> Native executables, no C compiler: Windows x64 from `fid emit`/`fid build`
> (compiler/pe_ir*.fd, with the compiler/iropt.fd optimizer), Linux
> AArch64/RISC-V/x86-64 and opt-in Windows `--nat` from the machine layer
> (compiler/nat_ir.fd, nat_enc.fd, runtime in stdlib/linux_runtime/). The
> old PIR path (`build --native` with the default target: opt.fd,
> codegen_x64, elf64, linker) panics in opt.fd `pass_jump_thread`;
> compiler/pe.fd is reached only by `emit --boot`, which panics. Native async
> under `emit` broke in builds of Nov 9 afternoon (iropt passes; fixed again in the evening's fid31, known-bugs 2f.2). Open items:
> [known-bugs.md §2g](known-bugs.md#2g-nov-9-2026--module-audit-open-bugs)
> (Nov 9 audit) and §2f. Each module is described in
> `docs/file-analysis/<module>.md` (all rewritten Nov 9).
> Current counts are in [§ Numbers](#numbers).
> Most of this file is the history of getting here; commands below that
> mention `release.fid.exe` or `C:\tcc` are historical.

> Consolidated 2026-10-24 from selfhost/ README, agent-guide, COORDINATION, architecture, build-pipeline, language-constraints, path-to-fixpoint, capability-comparison, vision-analysis. Living ledgers now live alongside it in `docs/reference/`: `known-bugs.md` (includes the port-gap-analysis appendix), `calling-conventions.md`.


## Overview

*From `README.md`.*

> **Read `selfhost.md` § Coordination FIRST, before any other file in this folder.**
> It tracks active sessions, architectural decisions, and file ownership.
> Without it, parallel agents will conflict and undo each other's work.

### Files in this folder

| File | Purpose | Read order |
|------|---------|------------|
| `selfhost.md` § Coordination | **READ FIRST.** Active sessions, architectural decisions, file ownership, pre-flight checklist | 1 |
| `selfhost.md` § Vision | Are we on the right path? What the language can do now, what's missing | 2 |
| `selfhost.md` § Capability Comparison | What Fides can do today + honest comparison vs C/Rust/Zig/Go/Python | 3 |
| `selfhost.md` § Path to Fixpoint | **The steps from current state to self-reproduction** | 4 |
| `AGENTS.md` → Current Baseline (repo root) | What is true right now: counts, fixpoint, open items | 5 |
| `selfhost.md` § Architecture | File-by-file breakdown of every `compiler/*.fd` module (including `pe.fd`) | 4 |
| `selfhost.md` § Language Constraints | What the Fides language must respect: types, effects, regions, patterns, closures, FFI, defer, try | 5 |
| `calling-conventions.md` | SRET, enum layout, struct copy, Box semantics, Vec handling — the low-level machinery | 6 |
| `selfhost.md` § Build Pipeline | How to build, test, bootstrap, and verify the self-hosted compiler (including PE emitter `build` command) | 7 |
| `known-bugs.md` | Known bugs, gotchas, and things that have been fixed (do not re-introduce) | 8 |
| `known-bugs.md` appendix | Rust vs Fides module comparison (historical) | 9 |
| `selfhost.md` § Agent Guide | Best practices and rules for agents working on the compiler | 10 |

> **Note:** `calling-conventions.md` and `known-bugs.md` contain some outdated
> content about the inline + memcpy approach for enum payloads. The current
> direction is the **pointer-based approach** — see `selfhost.md` § Coordination [D1].
> Those docs will be updated when the approach is finalized.

### Quick Start

```bash
# 1. Build the Rust host compiler
cargo build --release

# 2. Build the self-hosted compiler → release/fid-dev.c + release/fid-dev(.exe)
./scripts/build-selfhost.sh
# (the script does dump-c, adds the 256MB-stack wrapper, and compiles —
#  naming convention: release/fid-dev.c for generated C, release/fid-dev(.exe)
#  for the binary. The shipped binary is release/fid(.exe), built by
#  RELEASE=1 ./scripts/bootstrap.sh.)

# 3. Multi-stage bootstrap + fixpoint check (optional, slow)
./scripts/bootstrap.sh            # builds c1…cN in build/bootstrap/, compares c2.c vs c3.c

# 4. Run the self-host test suite
./verify-selfhost.sh

# 5. Verify the Rust compiler's test suite still passes
./verify.sh
```

### Numbers

The one place project counts and measurements are kept (other docs round
them and link here). Counted with `wc -l` on Nov 9, 2026.

| Component | Files | Lines |
|-----------|-------|-------|
| Self-hosted compiler (`compiler/*.fd`) | 68 (60 reached from `main.fd`) | 158,516 |
| Rust compiler (`src/*.rs`, frozen) | 57 | 112,598 |
| Stdlib (`stdlib/*.fd`) | 146 | 51,496 |
| Native runtimes (`stdlib/pe_runtime/`, `stdlib/linux_runtime/`) | 4 + 5 | 5,688 + 5,627 |
| Tests (`tests/bootstrap/`) | 543 .fd (502 top level, 105 expected-fail) | — |
| Tests (`tests/selfhost/`) | 51 .fd | — |
| Tests (`tests/conformance/`) | 104 .fd (each checks a rule of reference.md) | — |
| Tests (`tests/leaks/`) | 3 .fd + .expected (`scripts/leak-check.sh`) | — |
| Examples (`examples/`) | 11 .fd | — |
| Installer (`installer/`) | 5 | — |

| Measurement | Value |
|-------------|-------|
| Last full suite (Rust harness, `fidesc` as the compiler) | Nov 2: interp 339/339, C 343/343, PE 345/345, expected-fail 89/89 |
| Full suite with `fid` as the compiler | not run |
| Native Linux suite (`NATIVE=1 scripts/cross-test.sh`, Nov 9) | AArch64 273, RISC-V 271, x86-64 272 of 317 match the interpreter |
| Windows x64 on the machine layer (`fid emit --nat`, Nov 9) | 301 of 338 |
| Bootstrap fixpoint | stage 2 C == stage 3 C: Nov 6 (49 MB), Nov 8 (67 MB, ownership 0004) |
| Self-compile (`dump-c compiler/main.fd`) | Nov 9: 74 s with the compiler built at -O2 in 8 split units (type check 18 s, borrow check 5, mono 5, lowering 24, C 20), 229 s at -O0; Nov 6: ~133 s; `fidesc`: ~21 s |

Session 30 (Oct 2026) for comparison: compiler 54 files / 104,439 lines, Rust
55 / 100,935, stdlib 104 / 30,446, 302 bootstrap tests.

### Status at v1.0.0 (Sep 9, 2026; history)

- **🎉 v1.0.0 RELEASED!** — Windows installer (`Fides-Setup-1.0.0.exe`, 2.5 MB)
  tested and working on Windows. Users can download, install, and compile
  `.fd` files immediately. Available at
  [GitHub Releases](https://github.com/pledgeandgrow/fid/releases/tag/v1.0.0).
- **🎉 GitHub Actions CI** — automated installer builds on tagged releases
  (`.github/workflows/build-installer.yml`).
- **Bootstrap fixpoint (VERIFIED, re-verified session 29)** — The fixpoint was
  first achieved in Session 15 (`fides-c2.c` == `fides-c3.c`, 701,100 lines),
  then re-established on the larger codebase: `fides-c4.c` == `fides-c5.c`
  byte-for-byte (~29.7 MB), via `scripts/bootstrap.sh` +
  `verify-selfhost.sh --fixpoint` (session 26, re-verified session 29).
- **Standalone binary (sessions 33–34)** — the session-30 stdlib-parse crash
  no longer reproduces. `RELEASE=1 scripts/bootstrap.sh` builds
  `release/fid.exe` (fixpoint checked), the installer ships it as
  `fid.exe`, and its `run`/`ccompile` match `fidesc` on the test programs.
  See AGENTS.md Known Bugs #1 for the exceptions.
- **5 bootstrap generations verified** — Rust → c1 → c2 → c3 → c4 → c5.
- **🎉 Category A crash blockers RESOLVED!** (session 15):
  - A1 (TypeChecker_lookup crash) — **ROOT CAUSE FOUND**: TCC produces executables with a 4KB stack reserve, causing stack overflows in deeply recursive compiler code (parser, type checker, codegen). Fixed with a Windows thread wrapper providing 256MB stack.
  - Type equality bug — **FIXED**: `type_eq` in `compiler/typechecker.fd` used pointer `==` for string comparisons instead of `str_equal`. Changed `n1 == n2` to `str_equal(n1, n2)` for Named, Generic, Dyn, and Assoc type variants. This fixed the `cannot assign Vec<X> to field of type Vec<X>` errors.
  - Large match expression codegen — **FIXED**: Split `subst_expr`/`subst_stmt` in `mono.fd` and `substitute_expr` comptime cases in `comptime_eval.fd` into helper functions to avoid codegen issues with oversized match arms.
- **Self-compilation (WORKS)** — `fides-c.exe` compiles `compiler/main.fd` → `fides-c2.exe` … up to c5 (fixed in session 26). The failure previously reported here (SIGSEGV in `Lowerer_lower_stmt`, session 24) is resolved; the *current* standalone-binary issue is the stdlib-parse crash documented above.
- **🎉 PE emitter ported to Fides!** — `compiler/pe.fd` (4,845 lines) generates native Windows PE executables without tcc
- **🎉 comptime_eval.fd fully ported!** — process_gen, substitute, I-5A aggressive comptime, I-5D loop unrolling, reflection
- **🎉 Linux native codegen ported!** — `peephole.fd` + `regalloc.fd` + `codegen_x64.fd` (session 8), builds OK, not yet integrated
- **🎉 LSP server ported to Fides!** — `compiler/lsp.fd` (1,082 lines) — JSON-RPC over stdio, diagnostics, completion, hover, definition, rename (session 9)
- **🎉 flat_ast.fd ported!** — `compiler/flat_ast.fd` (1,259 lines) — flat AST serialization for `dump-ast-flat` command (session 9)
- **🎉 Module-level `var`/`pub var` implemented!** — Rust compiler now supports mutable globals across all backends (interp, C, PE, WASM, ARM64, RISC-V). `test_globals.fd` passes on interp, C, and PE. (session 11)
- **🎉 Category B codegen parity gaps resolved!** (session 14):
  - B5 (float bit-pattern) — **FIXED**: `p_f64_to_bits`/`p_f32_to_bits` runtime + union compound literals in ConstFloat codegen
  - B6 (print/println dispatch) — **AT PARITY**: all type cases (Vec/HashMap/Tensor/F64/Bool/Ptr) match Rust
  - B7 (runtime sigs) — **AT PARITY**: `p_malloc_persistent`/`p_arena_save`/`p_arena_restore` registered
  - B8 (Vec word_repr) — **AT PARITY**: `lower_method_call` uses `word_repr(elem_ty)`; `infer_expr_type` matches Rust by design
- **Box<T> unbox helpers centralized** — `box_expr_unbox`, `box_type_unbox`, `box_pattern_unbox` now in `ast.fd` (session 9)
- **`.exe` extension auto-appended** — `ccompile` and `build` now produce `.exe` output without manual rename (session 8)
- **`build` command** — `fides-c.exe build tests/bootstrap/hello_pe.fd -o hello_pe.exe` → working .exe
- **`lsp` command** — `fides-c.exe lsp` starts the LSP server (session 9)
- **PE runtime stubs** — 25+ stubs (print, string, vec, map, float, bool) for native PE executables
- **All pipeline stages pass** during self-compilation (lex, parse, typecheck, borrowck, mono, lower, C codegen, tcc link)
- **SRET fixed** — struct returns via out-parameter convention now work
- **Enum `==` fixed** — all enum comparisons use `*_eq()` helper functions
- **String builder** — `p_sb_*` eliminates O(n²) memory in C codegen
- **23/23 self-host tests pass** on interp + PE (tests/compiler/)
- **Strict fixpoint (VERIFIED)** — `fides-c4.c` == `fides-c5.c` (byte-identical, ~29.7 MB) re-verified in session 29. The earlier `fides-c2.c` == `fides-c3.c` proof (session 15) was superseded, not lost.

#### Test results (v1.0.0, Session 24)
| Backend | Result |
|---------|--------|
| Self-host tests (Interpreter) | 38/38 passed |
| Self-host tests (C Codegen) | 38/38 passed |
| Self-host tests (PE Emitter) | 38/38 passed |
| Bootstrap: Interpreter | 236/237 passed |
| Bootstrap: C Codegen | 226/237 passed |
| Bootstrap: PE Emitter | 237/237 passed |
| Bootstrap chain | 3 generations verified (Rust→c1→c2→c3, all work) |
| Self-compilation | **FAILS** — SIGSEGV in Lowerer_lower_stmt |
| Fixpoint | **NOT VERIFIABLE** — fides-c2.c missing |
| Windows installer | Tested on Windows — working |

Pre-existing failures (documented in `known-bugs.md` §2b.8): test_codegen_c, test_lower, test_banking, test_deep_nesting, test_new_features, test_parallel, test_parallel_concurrent, test_phase8b, test_rc_arc, test_regression_collections, test_vec_math, test_token_full, test_fmt_real — all related to the `var` feature implementation (session 11).

#### Remaining steps to strict fixpoint:
1. ~~Fix `.exe` extension in self-hosted compiler output~~ — DONE (session 8)
2. ~~Port LSP server to Fides~~ — DONE (session 9)
3. ~~Category B codegen parity gaps~~ — DONE (session 14: B5 fixed, B6/B7/B8 at parity)
4. ~~Fix Category A crash blockers~~ — DONE (session 15: stack wrapper + type_eq str_equal fix)
5. ~~Achieve strict fixpoint~~ — **DONE (session 15), but NO LONGER VERIFIABLE.** Subsequent sessions (17-24) added significant code. `fides-c2.c` is missing. Self-compilation currently FAILS with SIGSEGV.
6. **Fix self-compilation SIGSEGV** — Crash in `Lowerer_lower_stmt` during lowering of full 75K LOC compiler. This is the critical blocker.
7. **Regenerate fides-c2.c and verify fixpoint** — Once self-compilation works, re-establish the fixpoint.
8. **Fix remaining Category B gaps** (B1-B4, B9-B10 — low priority, not used by compiler source):
   - B1: `generate_c` entry point
   - B2: DynCall/TryUnwrap stubs
   - B3: Allocator builtins stubs
   - B4: Bare-metal builtins stubs
   - B9: Async/await lowering
   - B10: Closure lowering
9. **Mirror `var` feature to `compiler/*.fd` files** — Rust compiler has it, Fides self-hosted compiler does not yet
10. **Fix `inc.fd` global_cache_* using `var` feature** — now possible since Rust compiler supports `var`
11. ~~Fix Category E fixpoint stability risks~~ — **RESOLVED (session 15): c2==c3 proved no non-determinism at that time.** Debug prints (287 `p_eprint_str` calls) go to stderr and don't affect C output — gating is a cleanup task, not a fixpoint requirement.
12. Extend PE emitter to support full compiler source (struct/enum, Vec/HashMap, floats)
13. Integrate Linux native codegen (port `elf64.rs`, `linker.rs`, `runtime_linux.rs`, wire into pipeline)
14. ~~Assess debugger.rs porting feasibility~~ — **ASSESSED (session 9): NOT feasible without porting interp.rs (5,834 LOC, 251 runtime handlers, closures for debug hooks). Skip — debugger is a dev tool, not needed for self-hosting.**

See `docs/roadmap/roadmap.md` for the comprehensive gap analysis (Categories A–F) and `docs/reference/known-bugs.md` for the full bug history.

### For Parallel Sessions

If you run multiple agent sessions simultaneously:

1. **Every session reads `selfhost.md` § Coordination first.**
2. **Every session adds itself to "Active Sessions"** with the files it's editing.
3. **No two sessions edit the same file.**
4. **Architectural decisions are recorded immediately** in `selfhost.md` § Coordination.
5. **`src/lower.rs` and `compiler/lower.fd` must stay in sync** — if one changes, the other needs the same change.
6. **`src/codegen_c.rs` and `compiler/codegen_c.fd` must stay in sync.**


## Agent Guide

*From `selfhost.md` § Agent Guide.*

> Rules and workflows for AI agents (and humans) working on the self-hosted
> compiler. Read this before touching any file in `compiler/`.

---

### 1. Golden Rule: Verify Before and After

```bash
# ALWAYS run this before starting work
./verify.sh

# AND this if you are touching the self-hosted compiler
./verify-selfhost.sh

# Run this after your changes
./verify.sh
./verify-selfhost.sh
```

If a test fails, understand why before fixing. Do not guess. Use the tools:

```bash
./scripts/bisect.sh <test_name>
.\verify-stage-diff.ps1 -File <test_name>
.\verify-stdout-diff.ps1 -File <test_name> -Verbose
```

---

### 2. Read These Files First

Every session, in this order:

1. `AGENTS.md` → Current Baseline and Known Bugs — the current state and open items.
2. `AGENTS.md` — build/test commands and high-risk files.
3. `docs/reference/selfhost.md` — this file.
4. The relevant section of `docs/reference/selfhost.md` § Architecture or § Language Constraints or `calling-conventions.md`.
5. `docs/roadmap/roadmap.md` — the phase you are in.

---

### 3. High-Risk Files — Do Not Touch Blindly

These files can break the entire compiler or silently corrupt output:

| File | Why it is dangerous |
|------|---------------------|
| `compiler/lower.fd` | SRET, enum layout, match, try, defer, effects, calling conventions |
| `compiler/ir.fd` | Core IR types; changes ripple to all backends |
| `compiler/codegen_c.fd` | ABI, struct copy, SRET, C type mapping |
| `compiler/pe.fd` | PE emitter — x86-64 instruction encoding, ModRM/SIB bytes, REX prefixes, PE file format. Off-by-one = corrupt .exe. Must mirror `src/pe.rs`. |
| `compiler/peephole.fd` | 9 peephole passes — a wrong transform silently corrupts all output. Must mirror `src/peephole.rs`. (session 8, not yet integrated) |
| `compiler/regalloc.fd` | Linear scan register allocator — any change affects EVERY function. Must mirror `src/regalloc.rs`. (session 8, not yet integrated) |
| `compiler/codegen_x64.fd` | PIR L3 → x86-64 instruction encoding, ModRM/SIB bytes, REX prefixes, ABI rules. Must mirror `src/codegen_x64.rs`. (session 8, not yet integrated) |
| `compiler/parser.fd` | Recursive descent; subtle parse changes |
| `compiler/ast.fd` | Canonical AST; if you add/remove variants, update `lower.fd` `box_*_unbox` and `codegen_c.fd` |
| `compiler/compiler.fd` | Pipeline driver; import resolution is fragile |

For the Rust reference compiler, also treat `src/lower.rs`, `src/codegen_c.rs`, `src/pir.rs`, `src/regalloc.rs`, `src/peephole.rs`, `src/codegen_x64.rs`, and `src/pe.rs` as high-risk.

---

### 4. How to Make a Safe Change

#### Step 1: Reproduce the problem

If you are fixing a bug, write a failing test or run the existing one:

```bash
./target/release/fidesc run tests/<name>.fd
./target/release/fidesc run tests/<name>.fd --backend c
```

#### Step 2: Make the smallest possible change

- Edit in place.
- Do not refactor surrounding code.
- Do not rename things unless absolutely necessary.
- Do not add abstractions.
- Do not add tests/docs unless asked.

#### Step 3: Verify the fix

```bash
# For the Rust compiler
./target/release/fidesc run tests/<name>.fd
./target/release/fidesc run tests/<name>.fd --backend c
./target/release/fidesc run tests/<name>.fd --backend pe

# For the self-hosted compiler, rebuild and run
.\build-selfhost.ps1
.\release.fid.exe run tests/<name>.fd

# For the PE emitter (native codegen, no tcc)
.\release.fid.exe build tests/bootstrap/hello_pe.fd -o release/hello_pe.exe
.\release\hello_pe.exe
```

#### Step 4: Run the full suites

```bash
./verify.sh
./verify-selfhost.sh
```

#### Step 5: Commit

See `AGENTS.md` for the commit format. Use the `git commit` helper pattern:

```powershell
git commit -m "$(cat <<'EOF'
Fix X by doing Y.

Generated with [Devin](https://devin.ai)

Co-Authored-By: Devin <...>
EOF
)"
```

Do not commit if no changes exist. Do not push unless asked.

---

### 5. What to Do When the Self-Hosted Compiler Crashes

#### If the crash is `STATUS_ACCESS_VIOLATION` / segfault

1. Check if the PE stack was patched to 256 MB.
2. Check if the crash handler printed a call stack.
3. Look at the last progressive stage message to see which stage crashed.
4. Add targeted `p_eprint_str` prints in `compiler.fd` or the relevant module.
5. Rebuild and rerun.
6. Check for:
   - Null pointer from `p_file_read` / `p_malloc`
   - Unbounded recursion
   - SRET mismatch (first arg not a buffer)
   - Undersized enum/struct layout
   - `Inst::Load` on a struct-typed field

#### If the crash is in `Lexer_tokenize`

Check:
- `src_len` is cached and used everywhere (not `str_len(self.source)` in hot loops).
- No O(n²) string concatenation in the lexer.
- String literals are not overflowing buffers.

#### If the crash is in `Parser_...`

Check:
- `box_expr_unbox` matches the canonical `ast.fd` `Expr` enum.
- SRET is used for `ParseResult*` returns.
- Enum variant extraction is not truncating payloads.

#### If the crash is in the PE emitter (`pe.fd`)

Check:
- **ModRM byte encoding** — `mod=3` for register-register ops, `mod=0` for
  memory. A wrong `mod` field generates a memory store instead of a register
  load (or vice versa). Compare with `src/pe.rs`.
- **Call targets** — verify rip-relative call offsets resolve to the correct
  function labels. Use PowerShell to read the PE bytes and decode `E8` (CALL)
  instructions.
- **Missing statements** — the Fides compiler silently drops statements in
  long functions. If a function call is missing from the generated C, split
  the source function into smaller helpers.
- **Print type dispatch** — `Inst::Print` must dispatch by IR type (Ptr→str,
  F64→float, Bool→bool, default→int). Calling `p_print_int` with a string
  pointer crashes.
- **Binary output** — `p_file_write` truncates at null bytes. PE files must
  be hex-encoded and decoded via PowerShell.

---

### 6. How to Debug the SRET / Enum Machinery

#### Check the generated C

```bash
# Compile a small test to C
.\target\release\fidesc.exe ccompile test.fd -o test_out

# Or the self-hosted compiler
.\release.fid.exe ccompile test.fd -o test_out

# Read test_out.c
```

##### What enum variant extraction looks like in C (session 14)

For `ParseResultExpr::Ok(e) => { ... }`, under the **pointer-based enum
payload approach** (Coordination section [D1]), the generated C looks like:

```c
v12 = alloca(16);                  // ParseResultExpr buffer (tag + 1 pointer)
memset(v12, 0, 16);
Parser_parse_unary(v12, ...);      // SRET call (correct)
v15 = (char*)v12 + 0;
v16 = *(int64_t*)v15;              // tag
v18 = v16 == 0;                    // Ok variant?
if (v18) goto bb2;
else goto bb3;
bb2:
v20 = (char*)v12 + 8;              // pointer to Expr payload slot
v21 = *(void**)v20;                // reads the 8-byte pointer — CORRECT
```

With the pointer-based approach, `Load` reads the 8-byte pointer to the
payload data. No `load_enum_field()` or `memcpy` is needed. This is the
correct behavior under D1.

##### What to look for in generated C

- `alloca(N)` sizes match the struct/enum layouts (pointer-based: `(1 + max_arity) * 8`).
- Calls to struct-returning functions have the SRET pointer first: `func(&buf, ...)`.
- `memcpy` is used for `Inst::Store` when the store type is `IrType::Struct(_)`.
- `Load` on `IrType::Struct(_)` reads 8 bytes (the pointer) — this is correct
  for both regular struct values and enum payload extraction under D1.
- No `*(int64_t*)` stores for `IrType::Struct(_)`.
- Float constants use union compound literals: `((union { uint64_t u; double f; }){ .u = ... }).f`

#### Compare to Rust output

```bash
# Rust compiler
.\target\release\fidesc.exe dump-ir test.fd
.\target\release\fidesc.exe dump-c test.fd

# Self-hosted (when it works)
.\release.fid.exe dump-ir test.fd
.\release.fid.exe dump-c test.fd
```

#### Use stage differential

```bash
.\verify-stage-diff.ps1 -File <test_name> -Stage ir
.\verify-stage-diff.ps1 -File <test_name> -Stage c
```

Tells you whether the divergence is in lexer, parser, IR, or C codegen.

---

### 7. Do Not Do These Things

- **Do not `git stash`, `git checkout` over user files, `git reset --hard`, `git clean`, force-push, or delete branches.** These are destructive.
- **Do not run `rm -rf`.**
- **Do not add dependencies published less than 7 days ago.**
- **Do not commit secrets.**
- **Do not modify `minimumReleaseAge` or other security controls to work around CI.**
- **Do not use `-i` (interactive) git flags.**
- **Do not push unless explicitly asked.**
- **Do not write new files unless explicitly required** (docs are an exception when updating docs).

---

### 8. Documentation and Roadmap Updates

If you discover new information about the self-hosted compiler, update these
files:

- `AGENTS.md` → Current Baseline and `docs/reference/selfhost.md` → Numbers — counts and state
- `docs/roadmap/roadmap.md` — phase status and known tasks (SH-13 for PE emitter)
- `docs/reference/known-bugs.md` — new bugs or gotchas
- `docs/reference/calling-conventions.md` — new low-level conventions
- `docs/reference/selfhost.md` § Architecture and `docs/file-analysis/<module>.md` — new modules or pipeline changes

Do **not** create new files for every small note. Prefer appending to the
existing files in `docs/reference/`.

---

### 9. When to Ask for Help

Ask the user when:
- You hit an authentication/permission issue.
- A project configuration change is needed.
- You need to run a destructive operation.
- The fix requires changing many files and you are unsure of the approach.
- The Rust compiler and Fides compiler disagree and you cannot determine which is right.

Otherwise, keep trying reasonable approaches, run the tools, and verify.

---

### 10. Quick Checklist for Every Session

- [ ] Read `AGENTS.md` → Current Baseline.
- [ ] Read the relevant `docs/reference/*.md` and `docs/file-analysis/*.md` files.
- [ ] Run `./verify.sh` (or at least check the current state).
- [ ] Make the smallest change that fixes the issue.
- [ ] Rebuild the self-hosted compiler if `compiler/*.fd` changed.
- [ ] Run the relevant tests.
- [ ] Run `./verify.sh` and `./verify-selfhost.sh` before declaring done.
- [ ] Update `AGENTS.md` and `docs/reference/` if you learned something new.


## Coordination

*From `selfhost.md` § Coordination.*

> **READ THIS FIRST, BEFORE ANY OTHER FILE.**
> This is the single source of truth for what is being worked on, by whom,
> and what architectural decisions have been made. Without this, parallel
> agents will conflict and undo each other's work.
>
> **Last updated:** Sep 6, 2026 (session 16 — Tier 3 OS features added: @cpu_local, inline asm clobbers, --no-std, UEFI bindings. Bootstrap fixpoint preserved.)

---

### How to Use This File

#### Before starting work
1. Read this file completely.
2. Check "Active Sessions" — are any sessions working on files you need?
3. Check "Architectural Decisions" — has a direction changed since you last read the docs?
4. Add your session to "Active Sessions" with the files you plan to touch.
5. Read `AGENTS.md` → Current Baseline for the current state.

#### During work
1. If you make an architectural decision, add it to "Architectural Decisions" immediately.
2. If you change which files you're editing, update your session entry.
3. If you discover something important, add it to "Key Findings".

#### After finishing
1. Run `./verify.sh` and `./verify-selfhost.sh`.
2. Update `AGENTS.md` → Current Baseline and `docs/reference/changelog.md` with what you did.
3. Remove your session from "Active Sessions".
4. Update relevant docs in `docs/reference/` if you learned something new.

---

### Active Sessions

> Format: `[Session ID] — [Task] — [Files being edited] — [Status]`
> Add your session when you start, remove when you finish.

| Session | Task | Files | Status |
|---------|------|-------|--------|
| (none) | — | — | — |

> **If a file is listed here, DO NOT edit it in another session.**
> Wait for the session to finish, or coordinate with the user.

---

### Architectural Decisions

> **Every architectural decision must be recorded here with a date.**
> If a decision is reversed, mark it as `[REVERSED]` and add the new decision.
> This is what prevents agents from following outdated docs.

#### [D1] Enum payload storage — Sep 3, 2026

**Decision:** Enum payload fields that are structs/enums are stored as **pointers** (8 bytes each), not inline data.

**Rationale:**
- The C backend treats `IrType::Struct(_)` as a pointer type (`struct Foo_s*`).
- `Inst::Load` with a struct type reads only 8 bytes (the pointer).
- Storing inline struct data requires `memcpy` via `Inst::Store` with explicit `store_ty`, which is complex and error-prone.
- Storing pointers is simpler: every payload slot is 8 bytes, `Load` reads the pointer, field access works through the pointer.

**What this means for the code:**
1. `compute_enum_layout` / `Lowerer_make_enum_layout`: every field gets 8 bytes, regardless of type. `payload_slots = max_variant_arity` (number of fields), not `max_payload_size / 8`.
2. `EnumLit`: struct-typed args are heap-allocated (or already pointers from SRET), and the pointer is stored in the payload slot.
3. `pattern_binding_ir_types`: returns `word_repr(lower_type(t))` for all fields — `Struct(_)` becomes `Ptr(Struct(_))`. **[DONE in `src/lower.rs` by user]**
4. Match extraction: `Inst::Load` reads the 8-byte pointer. No `load_enum_field` needed.
5. `load_enum_field` in `src/lower.rs`: can be simplified to just `Inst::Load` for all types, or removed.

**Status:** `src/lower.rs` `pattern_binding_ir_types` changed to use `word_repr` for all types. `compute_enum_layout` and `EnumLit` in `src/lower.rs` still use inline storage — **INCONSISTENT — needs to be updated**.

**What needs to change:**
- `src/lower.rs` `compute_enum_layout`: change to 8 bytes per field (not struct size)
- `src/lower.rs` `EnumLit`: store struct args as pointers (single slot), not multi-slot inline copy
- `src/lower.rs` `load_enum_field`: remove or simplify (just `Load`)
- `compiler/lower.fd` `Lowerer_make_enum_layout`: change to 8 bytes per field
- `compiler/lower.fd` `EnumLit`: store struct args as pointers
- `compiler/lower.fd` match extraction: use `Load` (already does — no change needed)

**Previous direction [REVERSED]:** Store struct data inline in enum payloads, use `load_enum_field()` + `memcpy` to extract. This was documented in `docs/selfhost/calling-conventions.md` and `docs/selfhost/known-bugs.md` but is now outdated.

#### [D2] SRET calling convention — Feb 2026

**Decision:** Functions returning structs use the SRET (struct return via hidden out-parameter) convention.

**Status:** Implemented in both `src/lower.rs` and `compiler/lower.fd`. Working correctly.

#### [D3] `box_expr_unbox` in `compiler/lower.fd` is out of sync — Feb 2026

**Decision:** Must be reconciled with `compiler/ast.fd` canonical `Expr` enum.

**Status:** Not yet done. `box_expr_unbox` lists variants that don't exist in `ast.fd` (`Lambda`, `Throw`, `TryUnwrap`, `Assign`, `CompoundAssign`, `ConstExpr`, `ComptimeGen`, `QuantCast`, `Await`, `Spawn`).

#### [D4] PE emitter ported to Fides — Sep 10, 2026 (session 5)

**Decision:** The PE emitter (`src/pe.rs`) has been ported to Fides as
`compiler/pe.fd` (~2600 LOC). The `build` command in `main.fd` uses it to
generate native Windows PE executables without tcc.

**Rationale:**
- Eliminates the tcc dependency for Windows executable generation.
- The PE emitter is self-contained: own x86-64 encoder, own PE file writer,
  own import table, own runtime stubs (p_print_str, p_print_int, etc.).
- Brings Fides closer to the vision of a fully custom toolchain with zero
  external tools.

**What this means for the code:**
1. `compiler/pe.fd` contains `X86Encoder` (instruction encoder), `PeCodeGen`
   (IR → x86-64 code generation), and PE file writing logic.
2. `compiler/main.fd` has a `build` command that uses `PeCodeGen` instead of
   `ccompile` (which generates C and invokes tcc).
3. Binary output uses hex encoding + PowerShell decode (workaround for
   `p_file_write` truncating at null bytes).
4. `src/pe.rs` and `compiler/pe.fd` must stay in sync — if one changes, the
   other needs the same change.

**Status:** Working for simple programs (print, arithmetic, strings). Full
compiler source support is pending (struct/enum, Vec/HashMap, floats).

#### [D5] comptime_eval.fd fully ported — Sep 4, 2026 (session 7)

**Decision:** The comptime evaluation engine (`src/comptime.rs`, 1345 LOC) has
been fully ported to Fides as `compiler/comptime_eval.fd` (~1500 LOC).

**Rationale:**
- The previous `comptime_eval.fd` was a 39-line stub that only registered
  the module — no actual comptime evaluation was performed.
- The full port enables `comptime gen` blocks, `comptime <expr>` substitution,
  aggressive comptime evaluation (I-5A), and loop unrolling (I-5D) in the
  self-hosted compiler.

**What this means for the code:**
1. `compiler/comptime_eval.fd` contains `process_gen`, `substitute`,
   `evaluate`, `evaluate_with_env`, `evaluate_block`, and reflection helpers.
2. `compiler/compiler.fd` calls `process_gen` and `substitute` during the
   compilation pipeline (after typecheck, before monomorphization).
3. `ComptimeEnv` uses parallel Vecs (no HashMap at compile time).
4. `box_expr_unbox` and `box_expr_new` are local to `comptime_eval.fd` since
   they were local to other modules in the Rust source.

**Status:** Builds successfully. Self-host tests pass.

#### [D6] pe.fd runtime stubs expanded — Sep 4, 2026 (session 7)

**Decision:** The PE emitter's runtime stubs were expanded from 5 stubs
(print_int, print_str, panic, malloc, free) to 25+ stubs covering the
full runtime surface needed by the compiler source.

**Rationale:**
- The 5-stub minimum was sufficient for simple test programs but not for
  the full compiler source, which uses Vec, HashMap, string concatenation,
  bool/float printing, and int-to-string conversion.
- Without these stubs, the PE emitter would crash on any program using
  these runtime functions.

**Stubs added:**
- Print: `p_print_bool`, `p_print_float`
- String: `p_str_concat`, `p_to_str`, `p_to_str_str`, `p_to_str_bool`,
  `p_to_str_float`, `p_to_str_vec`
- Vec: `p_vec_new`, `p_vec_push`, `p_vec_len`, `p_vec_get`, `p_vec_pop`,
  `p_vec_set`, `p_vec_is_empty`, `p_vec_clear`, `p_vec_capacity`,
  `p_vec_first`, `p_vec_last`
- HashMap: `p_map_new`, `p_map_len`, `p_map_is_empty`, `p_map_contains`,
  `p_map_get`, `p_map_insert`, `p_map_remove`
- Encoder helper: `X86Encoder_div_r` (unsigned division)

**Status:** Builds successfully. `p_to_str_float` and `p_to_str_vec` are
simplified implementations (float uses int part + 6 fractional digits;
vec returns "[]" stub). Full implementations can be added later.

#### [D7] Linux native codegen ported to Fides — Sep 4, 2026 (session 8)

**Decision:** Three Rust source files forming the Linux ELF64 native
codegen pipeline were ported to Fides:
- `compiler/peephole.fd` (985 lines, from `src/peephole.rs` 903 lines)
- `compiler/regalloc.fd` (1086 lines, from `src/regalloc.rs` 1096 lines)
- `compiler/codegen_x64.fd` (1780 lines, from `src/codegen_x64.rs` 1923 lines)

**Rationale:**
- These three files form the Linux native codegen pipeline (PIR L3 →
  register allocation → peephole optimization → x86-64 AT&T assembly).
- Porting them is a prerequisite for eliminating tcc on Linux and for
  achieving a fully self-hosted Linux backend.
- `x86.rs` (1377 lines) was identified as dead code (not called from any
  CLI command) and was NOT ported. See known-bugs.md §1.29.

**What this means for the code:**
1. All three files build successfully with the bootstrap compiler.
2. They are NOT yet integrated into `compiler.fd`/`main.fd`.
3. Integration requires porting `elf64.rs`, `linker.rs`, and
   `runtime_linux.rs` to Fides, then wiring the pipeline.
4. The Windows PE backend (`pe.rs`/`pe.fd`) is separate and self-contained
   — it does NOT use these three files.

**Adaptations made during porting:**
- `val` is a reserved keyword in Fides → renamed to `value` everywhere
  (see known-bugs.md §1.26)
- `str_to_int` → `str_parse_int` (Fides stdlib naming, see §1.27)
- `Vec::new()` field reassignment requires intermediate typed variable
  (see §1.28)
- `PhysReg` enum → `i64` constants with `physreg_name()` helper
- `Option<T>` → `Opt*` enums (`OptStr`, `OptI64`, `OptLocation`, etc.)
- Char literals → i64 byte values
- `HashMap`/`HashSet` → Vec-based maps with helper structs

**Status:** All three files build OK. Not yet wired into the pipeline.

#### [D8] `.exe` extension auto-appended on Windows — Sep 4, 2026 (session 8)

**Decision:** The `ccompile` and `build` commands in `compiler/main.fd`
now automatically append `.exe` to output filenames on Windows if not
already present.

**Rationale:**
- Previously, `fides-c ccompile file.fd -o hello` produced `hello` (no
  extension), requiring a manual `Copy-Item hello hello.exe` to run it.
- The bootstrap compiler (`src/main.rs`) already defaults to `{stem}.exe`
  and preserves `.exe` in user-supplied output names.
- The self-hosted compiler currently targets Windows only (PE emitter +
  tcc with `-lws2_32`), so always appending `.exe` is correct.

**What this means for the code:**
1. New helper `ensure_exe_extension(s)` in `compiler/main.fd` — returns
   `s` unchanged if it ends with `.exe`, otherwise appends `.exe`.
2. `cmd_ccompile`: default changed from `a.out` → `a.exe`, output
   normalized via `ensure_exe_extension`. C source path derived by
   replacing `.exe` with `.c` (matching `src/main.rs` behavior).
3. `cmd_build`: output normalized via `ensure_exe_extension`.
4. `run`/`test`/`bench` commands already used hardcoded `.exe` temp
   names — no change needed.

**Status:** Done. Bug #2.9 in known-bugs.md marked as RESOLVED.

**Future note:** If Linux support is added to the self-hosted compiler,
this should be gated on a platform check rather than always appending
`.exe`.

#### [D9] LSP server ported to Fides — Sep 4, 2026 (session 9)

**Decision:** The LSP server (`src/lsp.rs`, 996 LOC) has been ported to
Fides as `compiler/lsp.fd` (1084 lines). The `lsp` command has been
added to `compiler/main.fd`, dispatching to `lsp_run()`.

**What was ported:**
- JSON-RPC 2.0 over stdio with Content-Length header framing
- `initialize` / `shutdown` / `exit` lifecycle
- `textDocument/didOpen` / `didChange` / `didClose` — document tracking
- `textDocument/publishDiagnostics` — via `get_diagnostics()` from `compiler.fd`
- `textDocument/completion` — keywords + functions + structs + enums +
  consts + type aliases + traits + dot-access methods/fields
- `textDocument/hover` — function/struct/enum/const/type/trait/method signatures
- `textDocument/definition` — source text search for definition location
- `textDocument/rename` — whole-word occurrence search + WorkspaceEdit

**Runtime I/O functions added to `compiler/codegen_c.fd`:**
- `p_read_line` — read a line from stdin (NULL on EOF)
- `p_read_stdin(n)` — read exactly N bytes from stdin (for LSP body)
- `p_stdin_eof` — feof check (detect client disconnect)
- `p_flush_stdout` — fflush(stdout) for immediate message delivery

**Status:** Module parses correctly. Full self-hosted build works
(session 15: bootstrap fixpoint achieved). The `lsp` command and
`lsp_run()` entry point are wired into `main.fd`.

---

#### [D10] flat_ast.fd ported + Box<T> unbox helpers centralized — Sep 4, 2026 (session 9)

**Decision:** `src/flat_ast.rs` (755 LOC) has been ported to Fides as
`compiler/flat_ast.fd` (1260 lines). This module provides flat AST
serialization for the `dump-ast-flat` command and self-host parser
differential testing.

**Box<T> unbox helpers centralized in ast.fd:**
- Added `pub fn box_expr_unbox`, `box_type_unbox`, `box_pattern_unbox` to
  `compiler/ast.fd`
- These were previously duplicated across 6+ modules (typechecker.fd,
  comptime_eval.fd, parser.fd, lower.fd, ir.fd, codegen_c.fd)
- Now available as shared public functions from `ast.fd`

**Key finding: Box<T> deref gap**
- `Box<T>` is `Generic("Box", [T])` in Fides — not a pointer type
- `*` (Deref) only works on `Type::Ref` and `Type::RawPtr`
- Workaround: `box_*_unbox()` helpers (match on all variants, return unboxed)
- Proper fix: `Deref` trait system (future work)

**interp.rs NOT ported:**
- The self-hosted compiler doesn't need an interpreter
- `run` command uses C compilation + tcc
- Comptime evaluation handled by `comptime_eval.fd` (AST-level)
- `interp.rs` (5782 lines) has features not needed for self-hosting

**Status (session 9; Nov 9: `flat_ast.fd` no longer passes `fid check` and nothing imports it):** `flat_ast.fd` builds successfully. `ast.fd` changes verified —
all 22/23 self-host tests still pass (1 pre-existing failure in
`test_codegen_c.fd` due to missing `p_sb_new` runtime function).

---

#### [D11] Category B codegen parity gaps resolved — Sep 5, 2026 (session 14)

**Decision:** The Category B code generation parity gaps between the Rust
compiler and the self-hosted compiler have been resolved:

- **B5 (Float constant bit-pattern) — FIXED:** Added `p_f64_to_bits`/
  `p_f32_to_bits` C runtime functions to both `src/codegen_c.rs` and
  `compiler/codegen_c.fd`. Registered as externs in both `src/lower.rs`
  and `compiler/lower.fd`. Updated `ConstFloat` handler in
  `compiler/codegen_c.fd` to emit union compound literals
  (`((union { uint64_t u; double f; }){ .u = <bits>ULL }).f`) matching
  the Rust codegen. Implemented `f64_to_bits`/`f32_to_bits` in
  `compiler/lower.fd` (were stubs returning 0). NaN/Inf/-0.0 bit patterns
  are now preserved.

- **B6 (Polymorphic print/println dispatch) — AT PARITY:** Verified via
  side-by-side comparison. All type cases (Vec, HashMap, Tensor, F64, Bool,
  Ptr/Opaque, Generic, any, default) match between Rust and Fides codegen.
  No changes needed.

- **B7 (Missing runtime sigs) — AT PARITY:** `p_malloc_persistent`,
  `p_arena_save`, `p_arena_restore` are registered in `compiler/codegen_c.fd`
  with matching parameter types. No changes needed.

- **B8 (Vec method word_repr) — AT PARITY:** `Lowerer_lower_method_call`
  in `compiler/lower.fd` uses `word_repr(vec_elem_ty)` for
  get/pop/first/last/remove, matching Rust. The `infer_expr_type` path
  returns raw `elem_ty` in both compilers — this is intentional parity
  by design. No changes needed.

**Files changed:**
- `src/codegen_c.rs` — added `p_f64_to_bits`/`p_f32_to_bits` (sigs, decls, impls)
- `src/lower.rs` — registered `p_f64_to_bits`/`p_f32_to_bits` externs
- `compiler/codegen_c.fd` — added runtime functions + updated ConstFloat handler
- `compiler/lower.fd` — registered externs + implemented `f64_to_bits`/`f32_to_bits`

**Remaining Category B gaps (B1-B4, B9-B10):** Low priority — not used by
the compiler source. See `docs/roadmap/roadmap.md` Category B table.

---

### Key Findings

> Discoveries that affect the direction. Add new findings at the top.

#### [F11] BOOTSTRAP FIXPOINT ACHIEVED — Sep 5, 2026 (session 15)

**fides-c2.c == fides-c3.c** (SHA256-identical, 701,100 lines each). The
self-hosted compiler compiles itself to byte-for-byte identical output
across generations. This is the ultimate goal of self-hosting.

**Bootstrap chain verified:**
```
Rust compiler → fides-c.c (706,318 lines) → fides-c.exe
fides-c.exe   → fides-c2.c (701,100 lines) → fides-c2.exe
fides-c2.exe  → fides-c3.c (701,100 lines) → fides-c3.exe  ← IDENTICAL to c2
fides-c3.exe  → compiles test_simple.fd correctly (EXIT 0)
```

**Four bugs were fixed in session 15 to achieve this:**
1. **TCC 4KB stack overflow** (bug 1.33) — TCC's default stack was 4KB,
   not 1MB as previously believed. Fixed by adding `-Wl,-stack,268435456`
   (256MB) to the TCC compile command.
2. **type_eq pointer == on strings** (bug 1.34) — `type_eq` used `==` for
   string comparison in `Type::Named`, `Type::Generic`, `Type::Dyn`, and
   `Type::Assoc`. Replaced with `str_equal`.
3. **Large match expression codegen** (bug 1.35) — `subst_expr` and
   `subst_stmt` in `mono.fd` had large match expressions that caused
   codegen issues. Split into helper functions.
4. **Non-ASCII characters in compiler source** (bug 1.36) — Unicode
   characters (em-dash, arrows, box-drawing) in comments and string
   literals caused fixpoint divergence. Replaced with ASCII equivalents.

**What's NOT yet done:**
- Rust vs self-hosted parity gap (c1=706K lines vs c2=701K lines, ~5K
  line difference). This does NOT block the self-hosted fixpoint but
  means the Rust and self-hosted compilers produce slightly different C.
- 287 `p_eprint_str` debug prints still in compiler source (go to stderr,
  don't affect C output or fixpoint).
- Category C/D pipeline features (interpretation, native build, WASM,
  safety verification) — not needed for fixpoint.

#### [F10] Large-function body truncation analysis — RESOLVED Sep 5, 2026 (session 15)

**Initial finding:** Second-generation C output (`stage2.c`) was truncated
at 361,955 lines (vs 701,100 expected). 1278 of 1881 function bodies were
missing. Truncation occurred at `void* v3` in `subst_expr` variable
declarations.

**Resolution:** The truncation was caused by the three bugs above (TCC
stack overflow, type_eq pointer comparison, large match codegen). After
fixing all three, the second-generation compiler produces full 701,100-line
output with 1795 function bodies (105 missing = expected external functions).

The earlier hypothesis of "value_metas Vec corruption" was incorrect —
the actual cause was the TCC 4KB stack overflow causing silent corruption
during code generation.

#### [F9] debugger.rs porting assessment — NOT feasible without interp.rs — Sep 4, 2026 (session 9)

**Assessment:** Porting `src/debugger.rs` (914 LOC) to Fides is **not
feasible** as a standalone task because it has a hard dependency on the
interpreter (`src/interp.rs`, 5782 LOC).

**Dependency chain:**
```
debugger.rs (914 LOC)
  └─ imports from interp.rs:
     ├─ Interpreter (struct with 50+ fields)
     ├─ RunResult (enum: Return/Pause/Panic)
     ├─ DebugAction (enum: Continue/Pause/InjectReturn/InjectPanic)
     ├─ Value (enum: Int/Float/Bool/Str/Ptr/Void/Vec/Map/Dyn/Tensor/Json/...)
     └─ TraceEntry (struct: func_name, block_id, inst_id, source_line, value_repr)
  └─ uses:
     ├─ Interpreter::new(module) — constructs interpreter from IrModule
     ├─ interp.set_debug_hook(closure) — installs callback before each instruction
     ├─ interp.run() — executes main function
     └─ closure receives (func_name, block, source_line, inst_id, values, func)
```

**What interp.rs contains (5782 LOC):**
- `Value` enum (14 variants, 130 LOC) — runtime values
- `Memory` struct (110 LOC) — simulated heap with byte addressing
- `Interpreter` struct (155 LOC, 50+ fields) — execution state
- `Interpreter::new()` (65 LOC) — initialization
- `Interpreter::run()` (10 LOC) — entry point
- `run_function()` (225 LOC) — function call dispatch + block iteration
- `execute_inst()` (888 LOC) — IR instruction evaluator (all Inst variants)
- `call_runtime()` (3247 LOC) — 251 runtime function handlers (p_print_*, p_vec_*,
  p_map_*, p_malloc, p_tensor_*, p_json_*, p_db_*, p_chan_*, p_mutex_*, p_tls_*,
  p_future_*, p_spawn, p_autograd_*, p_dp_*, p_quant_*, p_kv_cache_*, etc.)
- `eval_binop()` (186 LOC), `eval_unaryop()` (26 LOC), `cast_value()` (13 LOC)
- Autograd tape, tensor ops, futures, threads, channels, ECS, databases,
  DP noise, TP simulation, KV cache, quantization

**Why it's not needed for self-hosting:**
1. The self-hosted compiler's `run` command uses C compilation + tcc, not
   the interpreter
2. Comptime evaluation is handled by `comptime_eval.fd` (AST-level
   interpreter, already ported)
3. The `debug` and `replay` commands are developer tools, not required
   for bootstrap fixpoint
4. The self-hosted compiler's `main.fd` has no `debug`/`replay` commands

**What would be needed to port it:**
1. Port `interp.rs` (5782 LOC) → `compiler/interp.fd` — massive effort,
   ~8000+ LOC Fides. Would need:
   - `Value` enum with all 14 variants
   - `Memory` struct with byte-level load/store
   - `Interpreter` struct with 50+ fields
   - `execute_inst()` — all IR instruction handling
   - `call_runtime()` — all 251 runtime function handlers
   - Closures for debug hooks (Fides has no closures — would need a
     different mechanism, e.g. function pointers or a callback struct)
2. Port `debugger.rs` (914 LOC) → `compiler/debugger.fd`
3. Add `debug`/`replay` commands to `main.fd`

**Estimated effort:** 8000+ LOC Fides for interp.fd + 1200 LOC for
debugger.fd = ~9200 LOC total. Multiple sessions.

**Recommendation:** **Skip for now.** The debugger is a developer
convenience tool, not a self-hosting requirement. Port it only if:
- The self-hosted compiler needs interactive debugging capability
- Or if a future `run` command redesign switches from C compilation to
  a native interpreter (unlikely — C compilation is faster)

#### [F8] Pre-existing parse error blocks self-hosted rebuild — RESOLVED Sep 5, 2026 (session 15)

The original parse error at `codegen_c.fd:973` ("expected identifier but got
'var'") was resolved in earlier sessions. The self-hosted build pipeline
builds `fides-c.exe` successfully.

**RESOLVED (session 15):** All Category A crash blockers resolved. The
"TypeChecker_lookup null deref" was actually a TCC 4KB stack overflow
(bug 1.33), not a null pointer issue. Fixed by adding
`-Wl,-stack,268435456` to the TCC compile command. The bootstrap fixpoint
is now achieved (fides-c2.c == fides-c3.c).

#### [F7] Statement-dropping bug and fides-c2 crash — RESOLVED Sep 5, 2026 (session 15)

The `STATUS_ACCESS_VIOLATION` crash in `fides-c2_test.exe` while lexing
`stdlib/core.fd` was initially attributed to the **statement-dropping bug**
(bug 1.19 in known-bugs.md). Session 12 investigation found the primary
cause was **stack overflow** — the TCC-linked binary had a 4KB default stack
(not 1MB as previously believed). Fix: increased TCC stack reserve to 256MB
(`-Wl,-stack,268435456`) and split `keyword_or_ident_core` (51 if-statements,
933 C vars) into 5 smaller functions.

The statement-dropping bug (1.19) was also addressed in session 15 by
splitting `subst_expr` and `subst_stmt` in `mono.fd` into helper functions.
After all fixes, the second-generation compiler produces full 701,100-line
output with 1795 function bodies (105 missing = expected external functions).

**RESOLVED (session 15):** Bootstrap fixpoint achieved. No more crashes.
fides-c2.c == fides-c3.c (SHA256-identical).

#### [F6] Linux native codegen ported to Fides — Sep 4, 2026 (session 8)

Three files ported from Rust to Fides, all compile successfully:
- `compiler/peephole.fd` (985 lines) — 9 peephole optimization passes
- `compiler/regalloc.fd` (1086 lines) — linear scan register allocator
- `compiler/codegen_x64.fd` (1780 lines) — PIR L3 → x86-64 AT&T assembly
  (System V AMD64 ABI)

These form the Linux ELF64 native codegen pipeline. NOT yet integrated into
`compiler.fd`/`main.fd` — requires porting `elf64.rs`, `linker.rs`, and
`runtime_linux.rs` first.

**Key finding:** `x86.rs` (1377 lines) is dead code — not called from any
CLI command in `main.rs`. The actual Windows backend is `pe.rs` (5704 lines),
which is self-contained and does NOT use `codegen_x64.rs`/`regalloc.rs`/
`peephole.rs`. See known-bugs.md §1.29.

#### [F5] PE emitter ported to Fides — Sep 10, 2026 (session 5)

`compiler/pe.fd` (~2600 LOC) is a self-contained x86-64 code generator + PE
file writer, ported from `src/pe.rs` (5704 lines of Rust). The `build`
command in `main.fd` uses it to generate native Windows PE executables
directly from `.fd` source — no tcc, no gcc, no external linker.

**Working:** `fides-c.exe build tests\hello_pe.fd -o release\hello_pe.exe`
→ prints "Hello from PE!", exit code 0.

**Three bugs were fixed during the port:**
1. `X86Encoder_mov_imm32` ModRM bug — `mod=0` (memory) instead of `mod=3`
   (register), generating `mov [rcx], imm32` instead of `mov rcx, imm32`.
2. `Inst::Print` type dispatch — always called `p_print_int` regardless of
   argument type. Fixed to dispatch by IR type (Ptr→str, F64→float, Bool→bool).
3. Fides compiler statement-dropping bug — long functions silently lose
   statements. Worked around by splitting `gen_print_str` into smaller helpers.

**Binary file output:** `p_file_write` truncates at null bytes. Solved via
hex encoding the PE bytes and using PowerShell to decode to binary.

**Remaining for full compiler source:** struct/enum support, Vec/HashMap
runtime, float arithmetic, many-arg function calls. See SH-13 sub-tasks in
`docs/roadmap/roadmap.md`.

#### [F4] Self-compilation works — Sep 9, 2026 (session 4)

`fides-c.exe` successfully compiles `compiler/main.fd` → `fides-c2.exe`.
Six bugs were fixed to achieve this (see `docs/reference/known-bugs.md`
sections 1.11-1.16). The generated `fides-c2.exe` correctly compiles
`test_strconcat.fd` → "hello world".

**Remaining for fixpoint:** fides-c2.exe crashes on stdlib (statement-dropping
bug — root cause confirmed session 8), debug prints need gating.
~~`.exe` extension missing~~ — FIXED (session 8, see [D8]).

#### [F3] Fides lowerer `EnumLit` does not do multi-slot copy — Sep 3, 2026

`compiler/lower.fd` line 3337 stores each enum arg at field index `ai + 1` as a single 8-byte value:
```fides
var data_ptr: i64 = Lowerer_emit_inst(self, Inst::GetFieldPtr(alloca, ai + 1, IrType::I64));
var _s2: i64 = Lowerer_emit_inst(self, Inst::Store(data_ptr, arg_val, OptIrType::None));
```

The Rust lowerer (`src/lower.rs` lines 4650-4668) does multi-slot copy for struct-typed args. The Fides lowerer does not. This means the Fides lowerer already loses struct data on enum construction, not just extraction.

**With the pointer-based approach [D1], this bug goes away** — every arg is a single 8-byte pointer, so single-slot store is correct.

#### [F2] `c_type(IrType::Struct(name))` returns a pointer type — Feb 2026

`compiler/codegen_c.fd` line 146:
```fides
IrType::Struct(name) => { return "struct " + name + "_s*"; }
```

This means `Inst::Load` with `IrType::Struct(_)` reads 8 bytes (a pointer), not the full struct. This is the root cause of the enum variant extraction bug.

**With the pointer-based approach [D1], this is correct** — the payload slot stores a pointer, and `Load` reads it.

#### [F1] `Inst::Store` in Fides codegen does not emit `memcpy` for structs — Feb 2026

`compiler/codegen_c.fd` line 1961 ignores the `store_ty` parameter and treats all stores as scalar. The Rust codegen (`src/codegen_c.rs` lines 1498-1556) handles `IrType::Struct(name)` with `memcpy`.

**With the pointer-based approach [D1], this doesn't matter** — struct stores are always 8-byte pointer stores.

---

### Pre-Flight Checklist (Every Session)

Before touching any file:

- [ ] Read this file (`docs/reference/selfhost.md` § Coordination)
- [ ] Read `AGENTS.md` → Current Baseline
- [ ] Check "Active Sessions" — no conflicts with your planned files
- [ ] Check "Architectural Decisions" — no direction changes since last time
- [ ] Run `./verify.sh` to confirm baseline
- [ ] Add your session to "Active Sessions"

---

### File Ownership Map

> Which files are "owned" by which concern. If you're working on one of these
> files, you need to understand the full context.

| File | Concern | Coordinated by |
|------|---------|----------------|
| `src/lower.rs` | Enum layout, SRET, match extraction | This file |
| `compiler/lower.fd` | Fides lowerer — must mirror `src/lower.rs` | This file |
| `src/codegen_c.rs` | C codegen — struct store, SRET | This file |
| `compiler/codegen_c.fd` | Fides C codegen — must mirror `src/codegen_c.rs` | This file |
| `src/pe.rs` | Rust PE emitter — x86-64 codegen + PE file format | This file |
| `compiler/pe.fd` | Fides PE emitter — must mirror `src/pe.rs` | This file |
| `src/peephole.rs` | Rust peephole optimizer — 9 passes | This file |
| `compiler/peephole.fd` | Fides peephole — must mirror `src/peephole.rs` | This file |
| `src/regalloc.rs` | Rust register allocator — linear scan | This file |
| `compiler/regalloc.fd` | Fides register allocator — must mirror `src/regalloc.rs` | This file |
| `src/codegen_x64.rs` | Rust x86-64 codegen (Linux, AT&T syntax) | This file |
| `compiler/codegen_x64.fd` | Fides x86-64 codegen — must mirror `src/codegen_x64.rs` | This file |
| `src/lsp.rs` | Rust LSP server — JSON-RPC over stdio | This file |
| `compiler/lsp.fd` | Fides LSP server — must mirror `src/lsp.rs` | This file |
| `compiler/main.fd` | CLI entry point — `.exe` extension, command dispatch, `lsp` command | This file |
| `compiler/ast.fd` | Canonical AST — changes require updating `box_*_unbox` everywhere | This file |
| `AGENTS.md` → Current Baseline | Current status — update after every milestone | Session that completes a milestone |

---

### Session Protocol

#### Starting a session
1. Read this file + `AGENTS.md`.
2. Pick a task from the roadmaps (`docs/roadmap/`).
3. Add your session to "Active Sessions" with the files you'll touch.
4. Run `./verify.sh`.

#### Making architectural decisions
1. Add the decision to "Architectural Decisions" with a date and rationale.
2. If it reverses a previous decision, mark the old one as `[REVERSED]`.
3. Update any docs that reference the old direction.
4. Commit the decision before proceeding with implementation.

#### Finishing a session
1. Run `./verify.sh` and `./verify-selfhost.sh`.
2. Update `AGENTS.md` → Current Baseline and `docs/reference/changelog.md` with what you did and the new state.
3. Update relevant docs in `docs/reference/`.
4. Remove your session from "Active Sessions".
5. Commit your changes.

---

### Common Mistakes to Avoid

1. **Following outdated docs.** Always check "Architectural Decisions" first. The docs in `docs/reference/` may be stale.

2. **Editing files another session is working on.** Check "Active Sessions" first.

3. **Changing direction without recording it.** If you reverse an architectural decision, record it here immediately so other agents don't follow the old direction.

4. **Not running `./verify.sh` before and after.** You might be starting from a broken state, or you might break something without knowing.

5. **Updating `src/lower.rs` without updating `compiler/lower.fd`.** These must stay in sync. If you change one, the other needs the same change.

6. **Updating `src/codegen_c.rs` without updating `compiler/codegen_c.fd`.** Same — these must stay in sync.

7. **Updating `src/pe.rs` without updating `compiler/pe.fd`.** Same — these must stay in sync. The PE emitter is now ported to Fides (session 5).


## Architecture

*From `selfhost.md` § Architecture.*

> File-by-file breakdown of the self-hosted compiler modules in `compiler/`.
>
> **Historical (sessions 11–20).** The current description of every module
> (role, callers, the commands that reach it, open problems with line
> numbers) is `docs/file-analysis/<module>.md`, rewritten for all 68 modules
> on Nov 9, 2026; the one-line map is in INDEX.md. Known issues listed below
> were mostly fixed since; known-bugs.md §2g has the open ones.
>
> **Last updated:** Sep 6, 2026 (session 16 — Tier 3 OS features added: @cpu_local, inline asm clobbers, --no-std, UEFI bindings. Bootstrap fixpoint preserved.)
>
> **Note on line numbers:** Source line numbers in "Known issues (session 11
> analysis)" sections were accurate as of session 11 but may have shifted in
> sessions 12–14. Treat them as approximate guides, not exact references.
> Re-verify with `grep` before editing based on these numbers.
>
> **Module count:** 68 `.fd` files in `compiler/` (158,516 lines, Nov 9);
> the breakdown below was written for the 25 modules of session 16 (57,021
> lines) and does not cover the later ones (among them `interp.fd`,
> `escape.fd`, `secrecy.fd`, `checks.fd`, `iropt.fd`, `pe_ir*.fd`,
> `green_thread.fd`, `cowcheck.fd`). The "18 modules" count in older session
> notes is obsolete.

### The Pipeline (Simplified)

```
main.fd
  ├─ compiler.fd
  │    ├─ lexer.fd  ──► parser.fd  ──► ast.fd
  │    ├─ typechecker.fd ──► ast.fd
  │    ├─ borrowck.fd ──► ast.fd
  │    ├─ comptime_eval.fd ──► ast.fd
  │    ├─ flat_ast.fd ──► ast.fd (dump-ast-flat serialization)
  │    ├─ lower.fd ──► ir.fd
  │    ├─ mono.fd ──► ast.fd
  │    ├─ pir.fd / opt.fd ──► ir.fd
  │    ├─ inc.fd ──► ir.fd (incremental compilation cache)
  │    ├─ codegen_c.fd ──► ir.fd  ──► C source ──► tcc ──► .exe
  │    ├─ pe.fd ──► ir.fd  ──► x86-64 machine code + PE file ──► .exe (no tcc!)
  │    └─ [Linux backend — ported but NOT yet integrated]
  │         ├─ peephole.fd ──► L3Module (9 optimization passes)
  │         ├─ regalloc.fd ──► L3Module (linear scan register allocator)
  │         └─ codegen_x64.fd ──► L3Module ──► x86-64 AT&T asm ──► ELF64 (future)
  ├─ lsp.fd ──► compiler.fd (get_diagnostics) + json.fd ──► JSON-RPC over stdio
  ├─ fmt.fd ──► source formatter
  └─ diag.fd ──► structured diagnostics
```

`compiler.fd` is the central hub. `ast.fd`, `ir.fd`, and `token.fd` are the
shared data-definition leaves that almost everything else consumes.

The `ccompile` command uses `codegen_c.fd` (C codegen + tcc). The `build`
command uses `pe.fd` (native PE emitter, no tcc). The `lsp` command uses
`lsp.fd` (JSON-RPC over stdio, diagnostics via `compiler.fd`). The Linux
native backend (`peephole.fd` + `regalloc.fd` + `codegen_x64.fd`) is ported
but not yet wired into the pipeline — integration requires porting
`elf64.rs`, `linker.rs`, and `runtime_linux.rs` first.

---

### `compiler/main.fd` — CLI entry point

- **Module:** `compiler::main`
- **Purpose:** Minimal command-line front-end. Supports `ccompile` (C codegen + tcc) and `build` (native PE emitter, no tcc).
- **Key functions:** `main()` dispatches commands; `cmd_ccompile()` parses CLI args and invokes the C codegen pipeline; `cmd_build()` invokes the PE emitter pipeline.
- **Imports:** `core`, `vec`, `string`, `compiler::compiler`, `compiler::pe`
- **Notes:** Top-level executable; no other compiler module imports it. The `build` command was added in session 5.

#### Known issues (session 11 analysis)

- **`--borrowck` off-by-one** — line 71: `str_substring_from(garg, 12)` for
  `--borrowck=` (11-char prefix) drops the first value character. **P1.**
- **`g_borrowck` parsed but not passed through** — read from CLI but only
  printed, not forwarded to the compiler pipeline. **P1.**
- **`cmd_lint` is a stub** — line 1009: always prints "No lint warnings."
- **`cmd_run` has no interpreter backend** — line 547: compiles to C and
  executes the binary; `--max-recursion`, `--max-steps`, `--max-memory`,
  `--trace` are parsed but not wired.
- **`cmd_test` missing backends** — line 666: only C codegen + PE compile;
  no interpreter backend, no PE run check.
- **`cmd_build` unimplemented flags** — `--no-opt`, `--lto`, `--caps`,
  non-x86-64 `--target` are parsed but not implemented.
- **`read_source_or_error` silent failure** — returns `""` for unreadable
  files, indistinguishable from empty source.

### `compiler/compiler.fd` — Pipeline driver

- **Module:** `compiler::compiler`
- **Purpose:** Ties the whole compiler together: lex, parse, typecheck, lower, C codegen, plus path helpers, error enrichment, capability stripping.
- **Key types:** `ResultStr`, `ResultProgram`, `ResultIrModule`, `CompileResult`, `ErrCode`, `I64Pair`, `StrTriple`, `Option`
- **Key functions:** `build_program()`, `build_program_with_path()`, `ccompile_source_with_path()`, `lower_to_ir_module()`, `enrich_errors()`, `errcode_to_str()`
- **Imports:** `compiler::ast`, `token`, `lexer`, `parser`, `ir`, `typechecker`, `lower`, `mono`, `borrowck`, `codegen_c`, `comptime_eval`
- **Imported by:** `main.fd`

#### Import resolution (`build_program_with_path`)

1. Parse stdlib (`core` and dependencies).
2. Parse the user source.
3. Resolve imports LIFO from a queue.
4. `core` is skipped (already loaded).
5. Each import path `foo::bar` is converted to `foo/bar` and tried in several candidate locations:
   - `base_dir/foo/bar.fd` and `foo/bar/mod.fd`
   - `stdlib/foo/bar.fd` and `stdlib/foo/bar/mod.fd`
   - project-root variants
6. A pre-scan parse extracts transitive imports and struct/enum names.
7. Modules with unloaded dependencies are deferred; dependencies are loaded first.
8. Once all dependencies are ready, the module is fully parsed and added to `imported_modules`.
9. All loaded modules are merged (`merge_programs`) with user code last.
10. Pipeline continues: cfg → derive → desugar → comptime → typecheck → borrowck → mono.

**Critical rule:** there is no standalone `parse_import` helper. Import loading and parsing are all inline inside `build_program_with_path`. Refactor with extreme care.

#### Known issues (session 11 analysis)

- **`--borrowck` not wired** — `BorrowChecker_new(BorrowckMode::Off)` is
  hardcoded at line 1006 (TODO at 1003). The CLI flag is parsed in `main.fd`
  but never reaches the pipeline.
- **`apply_cfg_filter` is a no-op** — line 1295: `// No-op — include all items.`
- **`fill_default_trait_methods` incomplete** — line 1597: creates default
  method functions but never pushes them into `program.trait_impls`.
- **`c_file_read` suspicious comparison** — line 1960: compares `str` result
  to integer `0` (`if content == 0`), which likely never matches.
- **`c_os_cwd` hardcoded** — line 1985: always returns `"."`.
- **`inc.fd` global cache is a no-op** — `global_cache_get`/`global_cache_store`
  are stubs (see `inc.fd` section). Now fixable with the `var` feature.

---

### `compiler/token.fd` — Token types

- **Module:** `compiler::token`
- **Purpose:** Defines every token kind and the `Token` struct.
- **Key types:** `TokenKind` (133 variants), `Token`
- **Key functions:** `token_new()`, `token_kind_to_str()`, `is_word_keyword()`, `token_kind_eq()`
- **Imports:** none
- **Imported by:** `lexer.fd`, `parser.fd`, `compiler.fd`

### `compiler/lexer.fd` — Tokenizer

- **Module:** `compiler::lexer`
- **Purpose:** Byte-based tokenizer. Produces `Vec<Token>` with line/column info and collects lex errors.
- **Key types:** `Lexer`
- **Key functions:** `Lexer::new()`, `Lexer::tokenize()`, `keyword_or_ident()`, `read_identifier_or_keyword()`, `read_number()`, `read_string()`, `read_operator()`
- **Imports:** `core`, `vec`, `compiler::token`
- **Imported by:** `parser.fd`, `compiler.fd`

#### Important lexer implementation details

- `src_len` is cached on `Lexer` to avoid calling `str_len()` on every `peek()`.
- `peek()` returns `0` (NUL) at EOF; `peek_at(offset)` returns `-1` at EOF.
- Identifiers and numbers are extracted with `str_slice(self.source, start, end)` to avoid O(n²) `str_concat`.
- UTF-8 multi-byte sequences are counted as one column; continuation bytes are skipped by `advance()`.
- `read_utf8()` extracts 2/3/4 byte sequences and returns a `Str` token.

#### Known issues (session 11 analysis)

- **Duplicate `str_filter_underscores`** — empty stub at line 291 shadows the
  real implementation at lines 572-583. Numeric literals with underscores
  (e.g. `1_000_000`) will not have underscores stripped. **P0 — must fix.**

---

### `compiler/ast.fd` — Abstract Syntax Tree

- **Module:** `compiler::ast`
- **Purpose:** All AST node definitions used by parser, typechecker, lowerer, and codegen.
- **Key types:** `Program`, `Func`, `Stmt`, `Expr`, `Type`, `Pattern`, `BinOp`, `UnaryOp`, `ImplDef`, `TraitImpl`, `ExternDecl`, and many small helpers.
- **Key functions:** `program_new()`, `type_to_str()`, `binop_eq()`, `unaryop_eq()`, many `*_new` constructors.
- **Imports:** `core`, `vec`, `string`
- **Imported by:** `parser`, `lower`, `ir`, `typechecker`, `codegen_c`, `pir`, `opt`, `mono`, `borrowck`, `comptime_eval`, `compiler`

#### Canonical AST enums

**`Expr` (lines 244-283):**

```fides
pub enum Expr {
    IntLit(i64), FloatLit(f64), StringLit(str), BoolLit(bool),
    Ident(str),
    Binary(BinOp, Box<Expr>, Box<Expr>),
    Unary(UnaryOp, Box<Expr>),
    Call(str, Vec<Expr>),
    FieldAccess(Box<Expr>, str), OptionalFieldAccess(Box<Expr>, str),
    StructLit(str, Vec<FieldInit>, OptBoxExpr),
    If(Box<Expr>, Vec<Stmt>, OptStmts),
    Range(Box<Expr>, Box<Expr>, bool),
    Block(Vec<Stmt>),
    MethodCall(Box<Expr>, str, Vec<Expr>),
    ArrayLit(Box<Type>, Vec<Expr>),
    Index(Box<Expr>, Box<Expr>),
    FString(Vec<FStringPart>),
    Match(Box<Expr>, Vec<MatchArm>),
    EnumLit(str, str, Vec<Expr>),
    Ref(Box<Expr>, bool),
    Deref(Box<Expr>),
    StrConcat(Box<Expr>, Box<Expr>),
    Try(Box<Expr>), Coalesce(Box<Expr>, Box<Expr>),
    IsCheck(Box<Expr>, str),
    TryBlock(Vec<Stmt>),
    TupleLit(Vec<Expr>),
    Closure(Vec<Param>, Vec<Stmt>, OptType),
    Cast(Box<Expr>, Type),
    Asm(str, Vec<AsmOperand>),
    Comptime(Box<Expr>),
    ComptimeBlock(Vec<Stmt>),
    ComptimeReflect(ReflectOp, Type),
    PipeForward(Box<Expr>, Box<Expr>),
    QuantCast(Box<Expr>, Type),
    Await(Box<Expr>),
    Spawn(Box<Expr>),
}
```

**`Stmt` (lines 312-342):** Let, LetTuple, LetUninit, Assign, IndexAssign, DerefAssign, FieldAssign, Expr, Return, If, While, For, Loop, Break, Continue, Defer, Match, Use, Handle, Do, RegionBlock, Sandbox, Unsafe, Txn, ComptimeGen, GradBlock, Backward, Query, Select.

**`Type` (lines 103-132):** I8/16/32/64/128, U8/16/32/64/128, F32, F64, Bool, Str, Void, Never, Infer, Named, Array, ArrayConst, Ref, RawPtr, Generic, Tuple, Func, Assoc, Dyn, ShapedTensor.

**`BinOp` (lines 136-155):** Add, Sub, Mul, Div, Mod, Eq, Neq, Lt, Gt, LtEq, GtEq, And, Or, BitAnd, BitOr, BitXor, Shl, Shr.

**`Pattern` (lines 287-295):** Wildcard, IntLit, BoolLit, Ident, EnumVariant, StrLit, Range.

#### Important warning

`compiler/lower.fd` contains a `box_expr_unbox` helper that enumerates an
*out-of-sync* set of `Expr` variants (e.g. `Lambda`, `Throw`, `TryUnwrap`,
`Assign`, `CompoundAssign`, `ConstExpr`, `ComptimeGen`, `QuantCast`, `Await`,
`Spawn`). These **do not exist** in `compiler/ast.fd`. The self-hosted compiler
must use the `ast.fd` canonical enums; `lower.fd` needs to be reconciled to them.

---

### `compiler/parser.fd` — Recursive-descent parser

- **Module:** `compiler::parser`
- **Purpose:** Turns `Vec<Token>` into an AST `Program`. Returns custom `ParseResult*` enums because Fides has no `?` operator.
- **Key types:** `Parser`, `ParseResultExpr`, `ParseResultStmt`, `ParseResultStmts`, `ParseResultType`, `ParseResultProgram`, `OpEntry`, `BinOpInfo`, `IfExpr`
- **Key functions:** `Parser_new()`, `Parser_with_names()`, `parse()` → `ParseResultProgram`, `parse_expr()`, `parse_stmt()`, `parse_pattern()`, `parse_match()`
- **Imports:** `core`, `vec`, `compiler::token`, `compiler::ast`, `compiler::lexer`
- **Imported by:** `compiler.fd`

#### Parser conventions

- `ParseResultX::Ok(value)` / `ParseResultX::Err(str)` because Fides lacks `?`.
- Calls like `self.parse_expr()` must be matched explicitly every time.
- `peek_token()` and `peek()` inspect the current token.
- `advance()` consumes and advances.
- `match_tok(k)` consumes `k` if it is the current token and returns `true`.
- `expect(k, msg)` reports a parse error if the current token is not `k`.

#### Known issues (session 11 analysis)

- **`box_expr_deref` out-of-sync with `ast.fd`** — references 8 non-existent
  `Expr` variants (`CharLit`, `Assign`, `Lambda`, `SizeOf`, `AlignOf`,
  `OffsetOf`, `Path`, `UnitLit`) and uses wrong arities for `ArrayLit` (1 arg
  vs 2 in ast.fd) and `Range` (2 args vs 3 in ast.fd). **P0 — will fail to
  compile.**
- **Missing `return` statements** — `parse_guard_stmt` (line 2521),
  `parse_use_stmt` (line 3024), `parse_match_expr` (line 4002), and
  `parse_comptime_expr` (line 3848) fall off the end without returning a
  `ParseResult*`. **P0.**
- **Duplicate `var has_forward`** — declared twice in `parse_layer`
  (lines 1637-1639) and `parse_model` (lines 1704-1706). **P0.**
- **`parse_use` version-pin always errors** — lines 4684-4695: even when a
  valid version string is found, the function returns `Err`. **P1.**
- **`parse_fstring_parts` is a stub** (fixed since; Nov 9) — line 4847: f-string interpolation is
  extracted as raw text, not parsed. **P2.**
- **Non-exhaustive `match` patterns** — several helper functions
  (`expect_ident`, `read_type_name`, `parse_type` extraction, `parse_pattern`
  extraction) may need `_ => {}` default arms. **P1.**

---

### `compiler/typechecker.fd` — Type, effect, region, capability, and safety check

- **Module:** `compiler::typechecker`
- **Purpose:** The largest semantic pass — inference, region/effect/capability checks, traits, generics, linear types, annotations (`@deterministic`, `@gas`, etc.).
- **Key types:** `TypeChecker`, `StringType`, `MethodInfo`, `TraitMethodInfo`, `FuncSig`, `ScopeEntry`, `LinearVar`, `RegionBudget`
- **Key functions:** `TypeChecker_register_program()`, `TypeChecker_check_bodies()`, `TypeChecker_compute_func_effects()`, `TypeChecker_verify_*` family
- **Imports:** `core`, `vec`, `string`, `compiler::ast`
- **Imported by:** `compiler.fd`

#### Known issues (session 11 analysis)

- **13 empty safety verification stubs** (implemented since, each rule with a negative test; Nov 9) — lines 1451-1505: `verify_conserve`,
  `verify_gas`, `verify_safety_annotations`, `verify_constant_time`,
  `verify_realtime`, `verify_differentiable`, `verify_quantized`,
  `verify_checkpoint`, `verify_dp`, `verify_shard`, `verify_adapter`,
  `verify_tainted`, `verify_contracts`, `verify_allocator_params` are all
  no-ops. Safety annotations are accepted but not enforced. **P2** (not needed
  for bootstrap, but dangerous for production use).
- **`type_size` returns 8 for most composites** — line 2317: coarse sizing
  for ref, raw-pointer, and many composite types regardless of actual layout.
- **`verify_deterministic` is partial** — line 1407: rejects IO/Net/Spawn/
  Diverge and floating-point ops but does not reject randomness or time.

---

### `compiler/borrowck.fd` — Borrow checker

- **Module:** `compiler::borrowck`
- **Purpose:** Separate pass after typechecking that catches `&mut` vs shared borrow conflicts and illegal mutations while borrows are active.
- **Key types:** `SharedBorrowEntry`, `MutBorrowEntry`, `BorrowOfEntry`, `ReservedMutBorrow`, `BorrowChecker`, `BorrowckMode`
- **Key functions:** `BorrowChecker_new()`, `BorrowChecker.check()`, `BorrowChecker.check_func()`, `pattern_bindings()`
- **Imports:** `core`, `vec`, `string`, `compiler::ast`
- **Imported by:** `compiler.fd`

---

### `compiler/ir.fd` — Typed SSA intermediate representation

- **Module:** `compiler::ir`
- **Purpose:** Core IR types: types, effects, regions, instructions, basic blocks, functions, and modules.
- **Key types:** `IrType`, `Effect`, `EffectSet`, `Region`, `Inst`, `BasicBlock`, `IrFunction`, `IrModule`, `ValueId`, `ValueIdType`, `StringIrType`
- **Key functions:** `ir_type_size()`, `ir_type_align()`, `ir_type_is_int()`, `ir_type_is_float()`, `ir_type_is_ptr()`
- **Imports:** `core`, `vec`, `string`, `compiler::ast`
- **Imported by:** `lower`, `codegen_c`, `pir`, `opt`, `compiler`

#### `Opt*` pattern

```fides
pub enum OptIrType { Some(IrType), None }
pub enum OptValueId { Some(i64), None }
pub enum OptU64     { Some(i64), None }
pub enum OptString  { Some(str),  None }
pub enum OptInst    { Some(Inst), None }
pub enum OptValueMeta { Some(ValueMeta), None }
pub enum OptStructLayout { Some(StructLayout), None }
pub enum OptIrFunction { Some(IrFunction), None }
```

These are consumed with explicit `match X { Some(v) => ..., None => ... }`.
Fides has no `Option` combinators.

---

### `compiler/lower.fd` — AST → SSA IR lowering

- **Module:** `compiler::lower`
- **Purpose:** Lowers typed AST into Fides SSA IR (`IrModule`). Handles control flow, expressions, structs, enums, effects, defer, try, closures, regions.
- **Key types:** `Lowerer`, `StringStructDefEntry`, `StringEnumDefEntry`, `I64IrTypeEntry`, `OptFunc`, `OptStructDef`, `OptValueId`
- **Key functions:** `Lowerer_new()`, `Lowerer_lower_program()`, `lower()`, `Lowerer_compute_struct_layout()`, `Lowerer_compute_enum_layout()`, `Lowerer_lower_type()`, `Lowerer_lower_expr()`, `Lowerer_lower_match()`
- **Imports:** `core`, `vec`, `string`, `compiler::ast`, `compiler::ir`
- **Imported by:** `compiler.fd`

#### Key responsibilities

- Pre-register all function signatures so calls can look up return types before bodies are lowered.
- Pre-register impl methods and trait impl methods under mangled names (`StructName_method`, `TypeName_method`).
- Compute struct and enum layouts with iterative fixpoint for nested types.
- Implement the SRET (struct return via hidden out-parameter) convention for both callees and callers.
- Lower match expressions with tag checks and payload extraction.
- Track `current_sret`, `defer_stack`, `try_stack`, `region_stack`, `loop_stack`.

#### Currently known issue

Enum variant extraction for struct-typed payloads uses a plain `Inst::Load`,
which only copies 8 bytes. Under the pointer-based enum payload approach
(Coordination section [D1]), this is correct — each payload slot is an 8-byte
pointer. The old `load_enum_field()` workaround is no longer needed.

#### Session 14 changes (Category B)

- **`f64_to_bits` / `f32_to_bits` implemented** — were stubs returning 0;
  now call `p_f64_to_bits` / `p_f32_to_bits` runtime functions. Used by
  `ConstFloat` codegen to preserve NaN/Inf/-0.0 bit patterns.
- **`Lowerer_resolve_type` recursively resolves generic type arguments** —
  `Type::Generic(name, args)` now resolves each arg via `Lowerer_resolve_type`
  before returning. This fixes nested generic type alias expansion.
- **`p_f64_to_bits` / `p_f32_to_bits` externs registered** in
  `Lowerer_register_runtime`.

#### Known issues (session 11 analysis — line numbers approximate)

- **~~`f64_to_bits` always returns 0~~** — **FIXED (session 14)**.
- **`lower_type` narrows I128/U128 to I64/U64** — 128-bit integers silently
  truncated to 64-bit. **P1.**
- **`runtime_effects` is a hardcoded whitelist** — any `p_*` function not in
  the list returns `None` and is treated as pure, risking the optimizer
  deleting side-effecting calls. **P1.**
- **`Lowerer_infer_expr_ast_type` falls back to I64** — simplified inference
  returns `Type::I64` for most expression forms, making closure return type
  inference unreliable. **P2.**
- **`box_expr_unbox` enumerates out-of-sync variants** — references
  `Lambda`, `Throw`, `TryUnwrap`, `Assign`, `CompoundAssign`, `ConstExpr`,
  `ComptimeGen`, `QuantCast`, `Await`, `Spawn` which do not exist in
  `ast.fd`. (Already noted in the warning above.)

---

### `compiler/codegen_c.fd` — C99 code generator

- **Module:** `compiler::codegen_c`
- **Purpose:** Translates `IrModule` into C99 source, relying on a C runtime for `p_vec_*`, `p_map_*`, `p_string_*`.
- **Key types:** `ValueNameEntry`, `StringConstEntry`, `FnParamTypesEntry`, `FnRetTypeEntry`, `CCodeGen`
- **Key functions:** `CCodeGen_new()`, `c_type()`, `CCodeGen_generate()`
- **Imports:** `core`, `vec`, `string`, `compiler::ast`, `compiler::ir`
- **Imported by:** `compiler.fd`

#### Key responsibilities

- Map `IrType` to C types. `IrType::Struct(_)` becomes `void*` in C.
- Detect aggregate returns (`Struct` / `Array`) and emit `void` C return types.
- At call sites, emit `fname(&sret, ...)` for aggregate returns and set the result pointer.
- Emit `Inst::Store` as `memcpy` for struct-typed values (this is essential for SRET copy correctness).
- Cast pointer args to `void*` / `int64_t` as needed.
- Sanitize Fides function names that collide with C keywords.

#### Session 14 changes (Category B)

- **`ConstFloat` now preserves exact bit patterns** — emits union compound
  literals (`((union { uint64_t u; double f; }){ .u = <bits>ULL }).f`) instead
  of decimal casts. NaN/Inf/-0.0 are now bit-exact. Added `p_f64_to_bits` and
  `p_f32_to_bits` C runtime functions (decls + implementations).
- **Polymorphic print/println dispatch verified at parity** with Rust codegen
  (Vec/HashMap/Tensor/F64/Bool/Ptr/Generic/any/default cases all match).

#### Known issues (session 11 analysis — line numbers approximate)

- **`DynCall` not implemented** — emits only a comment
  `// DynCall not implemented in C backend`, leaving the destination variable
  uninitialized. **P1.** (Category B2)
- **`TryUnwrap` not implemented** — emits only a comment, same
  issue. **P1.** (Category B2)
- **~~`ConstFloat` TODO~~** — **FIXED (session 14)**. Now uses union compound
  literals with `p_f64_to_bits`/`p_f32_to_bits` for exact bit preservation.

---

### `compiler/pe.fd` — PE emitter (native x86-64 + PE file writer)

- **Module:** `compiler::pe`
- **Purpose:** Self-contained x86-64 code generator + Windows PE file writer.
  Translates `IrModule` directly into machine code and emits a valid PE
  executable — no tcc, no gcc, no external linker needed.
- **Key types:** `X86Encoder` (instruction encoder), `PeCodeGen` (IR → codegen),
  `PeStackFrame` (stack frame management), `PeFuncEntry`, `PeImportEntry`
- **Key functions:** `X86Encoder_*` (instruction encoding), `PeCodeGen_new()`,
  `PeCodeGen_generate()` (main entry), `PeCodeGen_gen_print_str()`,
  `PeCodeGen_gen_print_int()`, `PeCodeGen_gen_panic()`,
  `PeCodeGen_emit_entry_point()`, `PeCodeGen_write_pe_file()`
- **Imports:** `core`, `vec`, `string`, `compiler::ast`, `compiler::ir`
- **Imported by:** `main.fd` (via `build` command)
- **Ported from:** `src/pe.rs` (6,345 lines Rust → 4,845 lines Fides, session 5)

#### Key responsibilities

- Encode x86-64 instructions: REX prefixes, ModRM/SIB bytes, immediates,
  rip-relative addressing for calls and string literals.
- Generate PE file format: DOS header, PE header, section table, .text
  section (code), .rdata section (string literals + import table), IAT.
- Import Windows API functions: `GetStdHandle`, `WriteFile`, `ExitProcess`.
- Generate runtime stubs: `p_print_str` (strlen + WriteFile to stdout),
  `p_print_int` (itoa + WriteFile), `p_print_float`, `p_print_bool`,
  `p_panic` (ExitProcess with error code).
- Manage stack frames: prologue (`push rbp; mov rbp, rsp; sub rsp, N`),
  epilogue (`mov rsp, rbp; pop rbp; ret`), local variable access via
  `[rbp - offset]`.
- Label resolution: function labels, jump labels (for control flow),
  call targets resolved via rip-relative addressing.

#### Current capabilities (session 14)

| Feature | Status |
|---------|--------|
| PE header + section layout | ✅ |
| Import table (IAT) | ✅ |
| Entry point → main | ✅ |
| Function prologues/epilogues | ✅ |
| Stack frame management | ✅ |
| Print stubs (str/int/float/bool) | ✅ |
| Panic stub | ✅ |
| String literals (.rdata) | ✅ |
| Basic arithmetic | ✅ |
| Local variables | ✅ |
| Control flow (if/while/loop) | ✅ |
| String concat (p_str_concat) | ✅ |
| Int→string (p_to_str) | ✅ |
| Bool→string (p_to_str_bool) | ✅ |
| Float→string (p_to_str_float) | ✅ (simplified) |
| Vec runtime (new/push/len/get/pop/set/...) | ✅ |
| HashMap runtime (new/insert/get/contains/...) | ✅ |
| Full struct/enum support | ⏳ TODO |
| Float arithmetic (xmm ops) | ⏳ TODO |
| Many-arg calls (>4 args) | ⏳ TODO |
| Full compiler source | ⏳ TODO |

#### Known issues

- **`p_file_write` truncates at null bytes** — binary PE data is hex-encoded
  and decoded via PowerShell.
- **Fides compiler statement-dropping bug** — long functions in `pe.fd` may
  lose statements. Worked around by splitting into smaller helpers.
- **`src/pe.rs` and `compiler/pe.fd` must stay in sync** — if one changes,
  the other needs the same change.

#### Known issues (session 11 analysis)

- **20+ runtime stubs return wrong values** — `p_vec_insert`, `p_vec_remove`,
  `p_vec_reverse`, `p_vec_extend`, `p_vec_sort` all return 0 (`xor rax; ret`).
  `p_string_contains`, `p_string_starts_with`, `p_string_ends_with` return
  `true`. `p_string_split` returns empty Vec. `p_string_find`/`rfind` return
  0. `p_string_replace`/`to_uppercase`/`to_lowercase`/`trim`/`repeat` return
  input unchanged. `p_map_keys`/`values`/`clone` return empty. **P1** — any
  test using these via the PE backend will see wrong behavior.
- **Missing REX prefixes** — `X86Encoder` does not emit REX for all 8-bit/
  16-bit partial register operations. **P2.**

---

### `compiler/peephole.fd` — Peephole optimizer (Linux backend, NOT yet integrated)

- **Module:** `compiler::peephole`
- **Purpose:** 9 peephole optimization passes that run on L3 (post-regalloc)
  x86-64 instructions. Always enabled in the Rust `build` command.
- **Key types:** `PeepholeOptimizer`, `PeepInst`, `PeepOperand`
- **Key functions:** `PeepholeOptimizer_new()`, `PeepholeOptimizer_optimize()`,
  9 passes: redundant move elimination, zero idioms, dead store elimination,
  constant folding, branch folding, lea-to-add, strength reduction, address
  mode folding, jump threading
- **Imports:** `core`, `vec`, `compiler::pir` (for L3 types)
- **Ported from:** `src/peephole.rs` (903 lines Rust → 1,064 lines Fides, session 8)
- **Status:** Builds OK with bootstrap compiler. NOT yet wired into
  `compiler.fd`/`main.fd`. Integration requires the full Linux backend
  (`elf64.fd`, `linker.fd`, `runtime_linux.fd`).

---

### `compiler/regalloc.fd` — Linear scan register allocator (Linux backend, NOT yet integrated)

- **Module:** `compiler::regalloc`
- **Purpose:** Converts virtual registers in L3 IR to physical x86-64
  registers. Handles spill slots and constant rematerialization
  (`Location::Remat`).
- **Key types:** `ModuleAllocation`, `FuncAllocation`, `LiveInterval`,
  `Location` (Reg/Spill/Remat), `PhysReg` constants
- **Key functions:** `allocate_module()`, `allocate_function()`,
  `linear_scan()`, `spill()`, `rematerialize()`
- **Imports:** `core`, `vec`, `compiler::pir` (for L3 types)
- **Ported from:** `src/regalloc.rs` (1,088 lines Rust → 1,181 lines Fides, session 8)
- **Status:** Builds OK with bootstrap compiler. NOT yet wired into
  `compiler.fd`/`main.fd`.
- **Note:** `val` is a reserved keyword in Fides — all `val` variables in the
  Rust source were renamed to `value` during porting (see known-bugs.md §1.26).

---

### `compiler/codegen_x64.fd` — PIR L3 → x86-64 assembly (Linux backend, NOT yet integrated)

- **Module:** `compiler::codegen_x64`
- **Purpose:** Generates AT&T-syntax x86-64 assembly from PIR L3 modules.
  Uses the System V AMD64 ABI (RDI, RSI, RDX, RCX, R8, R9 for arguments).
  Output is suitable for `gcc -c` or `as`.
- **Key types:** `X64CodeGen`, `Operand` (Slot/Reg/Imm/Label/Const),
  `BlockLabelEntry`, `PanicStub`, `StringConstEntry`
- **Key functions:** `generate_asm()` (main entry), `X64CodeGen_gen_module()`,
  `X64CodeGen_gen_function()`, `X64CodeGen_gen_inst()`,
  `X64CodeGen_gen_call_abi()`, `X64CodeGen_gen_call_ptr()`,
  `X64CodeGen_gen_load()`, `X64CodeGen_gen_store()`
- **Imports:** `core`, `vec`, `string`, `compiler::pir`, `compiler::regalloc`
- **Ported from:** `src/codegen_x64.rs` (1,940 lines Rust → 1,942 lines Fides, session 8)
- **Status:** Builds OK with bootstrap compiler. NOT yet wired into
  `compiler.fd`/`main.fd`. Integration requires `elf64.fd` (object file
  emitter) + `linker.fd` + `runtime_linux.fd`.
- **Note:** `x86.rs` (1377 lines) is dead code and was NOT ported — see
  known-bugs.md §1.29. The actual Windows backend is `pe.rs`/`pe.fd`, which
  is self-contained and does not use this file.

---

### `compiler/lsp.fd` — LSP server (JSON-RPC over stdio)

- **Module:** `compiler::lsp`
- **Purpose:** Language Server Protocol implementation for editor integration.
  Reads JSON-RPC 2.0 messages from stdin (Content-Length framing), dispatches
  to handlers, writes responses/notifications to stdout.
- **Key types:** `DocEntry` (uri + text), `OptI64Pair` (definition search
  result), `I64Triple` (occurrence line/col/length)
- **Key functions:** `lsp_run()` (main message loop), `handle_initialize()`,
  `handle_completion()`, `handle_hover()`, `handle_definition()`,
  `handle_rename()`, `publish_diagnostics()`, `send_notification()`,
  `send_response()`, `send_error()`
- **Imports:** `core`, `vec`, `string`, `compiler::ast`, `compiler::json`,
  `compiler::compiler`
- **Ported from:** `src/lsp.rs` (916 lines Rust → 1,082 lines Fides, session 9)
- **Runtime dependencies (added to `codegen_c.fd`):** `p_read_line`,
  `p_read_stdin(n)`, `p_stdin_eof`, `p_flush_stdout`
- **Status:** Module parses correctly. `lsp` command wired into
  `main.fd`. Full self-hosted build works (session 15: bootstrap fixpoint achieved).
- **Features:**
  - `initialize` / `shutdown` / `exit` lifecycle
  - `textDocument/didOpen` / `didChange` / `didClose` — document tracking
  - `textDocument/publishDiagnostics` — via `get_diagnostics()` from `compiler.fd`
  - `textDocument/completion` — keywords + functions + structs + enums +
    consts + type aliases + traits + dot-access methods/fields
  - `textDocument/hover` — function/struct/enum/const/type/trait/method signatures
  - `textDocument/definition` — source text search for definition location
  - `textDocument/rename` — whole-word occurrence search + WorkspaceEdit

---

### `compiler/pir.fd` — Multi-level PIR (L1→L2→L3)

- **Module:** `compiler::pir`
- **Purpose:** Lowers IR through PIR levels: L1 (named regions/effects/caps), L2 (SSA mid-level), L3 (virtual regs / stack slots).
- **Key types:** `CapSet`, `RegionRef`, `Region`, `L1Block`/`L1Function`/`L1Module`, `L2*` and `L3*` levels
- **Key functions:** `CapSet_new()`, `L1Function_new()`, `L2Function_new()`, `region_ref_eq()`, `region_eq()`, `l1inst_is_terminator()`
- **Imports:** `core`, `vec`, `string`, `compiler::ast`, `compiler::ir`
- **Imported by:** `opt.fd`

#### Known issues (session 11 analysis)

- **`str_substring` is a stub** — line 2681: returns the original string `s`,
  ignoring `start` and `len` parameters. Semantically wrong if any caller
  uses it. **P1.**

---

### `compiler/opt.fd` — PIR L2 optimizer

- **Module:** `compiler::opt`
- **Purpose:** Core optimizer: pass enum, `optimize_l2` driver, and shared helpers; detailed passes live in `opt_*.fd` siblings.
- **Key types:** `Pass`, `OptStats`
- **Key functions:** `Pass_name()`, `Pass_all()`, `optimize_l2()`, `pass_const_fold()`, `pass_cse()`, `pass_dce()`, `pass_gvn()`, `is_pure()`
- **Imports:** `core`, `vec`, `string`, `compiler::ast`, `compiler::ir`, `compiler::pir`
- **Imported by:** part of the broader build pipeline

#### Known issues (session 11 analysis)

- **`is_lto_mode()` hardcoded false** — line 221: always returns `false`,
  LTO detection is broken (LTO is inferred from `max_iters` in the driver
  instead).
- **`cse_key` unreachable duplicate arms** — lines 1649-1660: duplicate
  `ConstInt`/`ConstFloat`/`ConstStr`/`ConstBool` arms after a catch-all `_`
  are dead code.
- **`effect_reorder_block` changed detection bug** — line 5926: compares
  `insts.get(pi2)` vs `pure_insts.get(pi2)` but `pi2` iterates over
  `pure_insts`, not `insts`, making the position check meaningless.

---

### `compiler/mono.fd` — Monomorphization

- **Module:** `compiler::mono`
- **Purpose:** Clones generic functions for concrete type instantiations, resolves const generics, handles trait dispatch.
- **Key types:** `OptType`, `OptStr`, `SubstEntry`, `AssocBindingEntry`, `TraitAssocEntry`, `GenericFuncEntry`, `GenericMethodEntry`
- **Key functions:** `subst_get()`, `subst_set()`, `generic_funcs_get()`, `trait_assoc_get()`
- **Imports:** `core`, `vec`, `string`, `compiler::ast`
- **Imported by:** `compiler.fd`

#### Known issues (session 11 analysis)

- **`subst_gat_type_args` always uses args.get(0)** — line 649: for
  single-character uppercase generic parameters, always uses the first type
  argument regardless of parameter index. Mis-substitutes any generic with
  more than one type parameter. **P1.**
- **`resolve_assoc_types` falls back to I64** — lines 711, 751, 789:
  unresolved associated types silently become `Type::I64` instead of
  producing an error. **P2.**
- **`infer_expr_type` is crude** — line 2565: returns `Type::I64` for many
  unhandled expression forms. **P2.**

---

### `compiler/fmt.fd` — Source formatter

- **Module:** `compiler::fmt`
- **Purpose:** Token-based pretty-printer for `.fd` files; preserves comments and normalizes spacing/indentation.
- **Key types:** `FTok`
- **Key functions:** `fmt_tokenize()`, `ftok_text()`, `format_source()`, `is_formatted()`
- **Imports:** `core`, `vec`, `string`
- **Notes:** Used via `fidesc fmt` CLI, not imported by the compiler pipeline.

---

### `compiler/diag.fd` — Diagnostic / error reporting

- **Module:** `compiler::diag`
- **Purpose:** Structured errors/warnings with severity, source context, color, and error codes.
- **Key types:** `Severity`, `ErrCode`, `Diag`
- **Key functions:** `Severity.label()`, `color_code()`, `ErrCode_*()` constructors
- **Imports:** `vec`
- **Notes:** The active compiler pipeline mostly uses `StrTriple` / string errors, but `diag` is the eventual replacement.

---

### `compiler/json.fd` — JSON parser/serializer

- **Module:** `compiler::json`
- **Purpose:** Self-contained JSON handling; used for LSP/config/dumps.
- **Key types:** `Json`, `JsonResult`, `JsonObject`
- **Key functions:** `json_parse()`, `json_dump()`, `json_escape_string()`
- **Imports:** `core`, `vec`, `string`
- **Notes:** Leaf utility module.

---

### `compiler/inc.fd` — Incremental compilation cache

- **Module:** `compiler::inc`
- **Purpose:** Source-hash-based incremental compilation cache. Stores
  `IrModule` results keyed by source path + hash to avoid recompiling
  unchanged modules.
- **Key types:** `IncCacheEntry`, `IncrementalCache`, `OptIrModule`
- **Key functions:** `hash_source()`, `hash_str()` (FNV-1a 64-bit),
  `IncrementalCache_new()`, `IncrementalCache_get()`,
  `IncrementalCache_store()`, `global_cache_store()`, `global_cache_get()`,
  `global_cache_clear()`
- **Imports:** `core`, `vec`, `string`, `compiler::ir`
- **Imported by:** `compiler.fd`

#### Known issues (session 11 analysis)

- **`global_cache_*` are all no-ops** — lines 134-145:
  `global_cache_store` does nothing, `global_cache_get` always returns
  `OptIrModule::None`, `global_cache_clear` does nothing. The file comments
  say these will be replaced "when Fides supports module-level mutable
  state." **Now fixable** — the `var` feature was implemented in the Rust
  compiler in session 11. The Fides `inc.fd` can now use `var` to implement
  a real global cache. **P1.**

---

### `compiler/comptime_eval.fd` — Compile-time evaluation engine

- **Module:** `compiler::comptime_eval`
- **Purpose:** Full comptime evaluation engine — evaluates `comptime <expr>`,
  `comptime gen { }` blocks, aggressive comptime (I-5A), and loop unrolling
  (I-5D). Injects generated functions/structs into the program and substitutes
  comptime expressions with their evaluated values.
- **Key functions:**
  - `process_gen()` — evaluates `comptime gen` blocks, parses result as Fides
    source, injects generated functions/structs
  - `substitute()` — recursively substitutes `comptime` expressions with
    evaluated values (8 iterations for fixpoint)
  - `evaluate()` / `evaluate_with_env()` / `evaluate_block()` — full AST
    interpreter (binary ops, unary ops, calls, if/else, blocks, for loops,
    string concat, f-strings, identifiers, comptime functions)
  - `evaluate_reflect()` — reflection: `TypeName`, `IsStruct`, `IsEnum`,
    `FieldCount`, `FieldNames`, `FieldTypes`, `EnumVariants`
  - `box_expr_unbox()` / `box_expr_new()` — boxing helpers (`box_expr_unbox`
    is centralized in `ast.fd` per [D10]; `box_expr_new` is local)
- **ComptimeEnv:** Uses parallel Vecs (no HashMap at compile time)
- **I-5A:** Auto-evaluates calls to `comptime fn` functions when all
  arguments are literals (no explicit `comptime` wrapper needed)
- **I-5D:** Unrolls `for` loops with literal bounds (≤16 iterations, ≤16
  body stmts) at AST level; eliminates dead `if` branches with literal
  conditions
- **Imports:** `core`, `vec`, `compiler::ast`
- **Imported by:** `compiler.fd`

#### Known issues (session 11 analysis)

- **Return re-wrap omissions** — `substitute_expr` and `replace_ident_in_*`
  skip `Stmt::Return` and `OptBoxExpr` re-wrapping (comments say "Can't
  re-wrap into Box easily; skip for now"). Comptime expressions inside
  `return` statements are not substituted or evaluated. **P1.**
- **`aggressive_comptime_expr` limited recursion** — only recurses into
  `Binary`, `Unary`, `Call`, `MethodCall`; other expressions (If, Match,
  ArrayLit, StructLit) are not traversed for nested comptime calls. **P2.**
- **`evaluate` falls through to `IntLit(0)`** — line 1077: unsupported
  expression forms silently become 0. **P2.**
- **`box_expr_unbox` defaults to `IntLit(0)`** — line 64: unhandled `Box<Expr>`
  variants silently become zero. **P2.**

---

### `compiler/flat_ast.fd` — Flat AST serialization

- **Module:** `compiler::flat_ast`
- **Purpose:** Produces a line-based serialization of the AST for
  `dump-ast-flat` command and self-host parser differential testing.
  Format: 2-space indent per depth, node type first, then fields
  (strings double-quoted, numbers bare, bools as true/false).
- **Key functions:**
  - `program_flat(p: Program) -> str` — main entry point, serializes entire program
  - `func_flat(f: Func, depth) -> str` — serializes a function with params, body
  - `expr_flat(e: Expr, depth) -> str` — serializes all 39 Expr variants
  - `stmt_flat(s: Stmt, depth) -> str` — serializes all 28 Stmt variants
  - `type_flat(t: Type, depth) -> str` — serializes all 29 Type variants
  - `pattern_flat(p: Pattern, depth) -> str` — serializes all 7 Pattern variants
  - `esc(s: str) -> str` — escapes strings for output (\\n, \\r, \\t, \\", \\\\, \\u{XX})
- **Box<T> handling:** Uses `box_expr_unbox()`, `box_type_unbox()`,
  `box_pattern_unbox()` from `ast.fd` to unbox `Box<Expr>`/`Box<Type>`/
  `Box<Pattern>` fields for recursive calls (Fides has no `*` deref for `Box<T>`)
- **Imports:** `core`, `vec`, `string`, `compiler::ast`
- **Imported by:** Not yet integrated into `compiler.fd` (standalone module)
- **Ported from:** `src/flat_ast.rs` (741 LOC Rust → 1,259 LOC Fides)

---

### `compiler/borrowck` (no `.fd` extension)

This is a compiled artifact / placeholder in `compiler/`. The actual borrow
checker is `compiler/borrowck.fd`.

### `compiler/codegen_c` (no `.fd` extension)

Also a compiled artifact. The source is `compiler/codegen_c.fd`.

### `compiler/lower` (no `.fd` extension)

Also a compiled artifact. The source is `compiler/lower.fd`.


## Build Pipeline

*From `selfhost.md` § Build Pipeline.*

> How to build, test, and verify the self-hosted compiler.
>
> **Last updated:** Sep 9, 2026 (v1.0.0 — Windows installer added, GitHub Actions CI)

---

### 0. Windows Installer (v1.0.0+)

#### Download (end users)

Download `Fides-Setup-1.0.0.exe` from
[GitHub Releases](https://github.com/pledgeandgrow/fid/releases/tag/v1.0.0)
and run it. This installs:
- the compilers to `C:\Program Files\Fides\` (`installer/fides.nsi` today ships `fid.exe`, the self-hosted compiler, plus `fidesc.exe`, `ark.exe`, `scribe.exe`, `veil.exe`)
- Standard library to `C:\Program Files\Fides\stdlib\` (99 modules in v1.0.0; 146 in the tree on Nov 9)
- C runtime files to `C:\Program Files\Fides\runtime\`
- Documentation to `C:\Program Files\Fides\docs\`
- PATH integration (system-wide)
- `.fd` file association
- Start Menu shortcuts
- Uninstaller via Add/Remove Programs

#### Build the installer (developers)

The installer is built automatically by GitHub Actions on tagged releases.
To build manually:

```bash
# On Windows with NSIS installed:
cd installer
makensis fides.nsi
# Output: installer/Fides-Setup-1.0.0.exe
```

Or trigger the workflow manually:
```bash
gh workflow run build-installer.yml -f version=1.0.0
```

The workflow (`.github/workflows/build-installer.yml`):
1. Builds the Rust bootstrap compiler (`cargo build --release`)
2. Copies `fidesc.exe` as `fides.exe` (the shipped compiler)
3. Installs NSIS via Chocolatey
4. Runs `makensis` to create the installer
5. Uploads the `.exe` to the GitHub release

#### Installer files

```
installer/
├── fides.nsi              Main NSIS installer script
├── license.txt            License shown during install
├── build-installer.sh     Linux-side build helper
├── EnvVarUpdate.nsh       (legacy, not used in v1.0.0)
└── FileAssociation.nsh    (legacy, not used in v1.0.0)
```

The v1.0.0 NSIS script inlines PATH and file association logic directly
(no custom includes needed).

---

### 1. Build Pipeline

#### Step 1: Build the Rust host compiler

```bash
cargo build --release
```

Produces `target/release/fidesc.exe` and `target/release/ark.exe`, etc.

For faster iteration (but slower execution):

```bash
cargo build
```

#### Step 2: Generate the self-hosted compiler C source

```bash
.\target\release\fidesc.exe dump-c compiler\main.fd > release\fides-c.c
```

Produces `release/fides-c.c`. The output is UTF-16 encoded on Windows and
may need conversion to UTF-8 before TCC can compile it:

```powershell
$content = [System.IO.File]::ReadAllText("release\fides-c.c", [System.Text.Encoding]::Unicode)
[System.IO.File]::WriteAllText("release\fides-c.c", $content, [System.Text.UTF8Encoding]::new($false))
```

#### Step 3: Apply the stack wrapper

TCC produces executables with a 4KB default PE stack reserve. The Fides
compiler needs 256MB for its deeply recursive parser, type checker, and
codegen. The stack wrapper renames `main` → `fides_main` and adds a Windows
thread with a 256MB stack:

```powershell
$content = [System.IO.File]::ReadAllText("release\fides-c.c")
$content = $content -replace "int main\(int argc, char\*\* argv\)", "int fides_main(int argc, char** argv)"
$wrapper = @"

// === Stack wrapper ===
#include <windows.h>
typedef struct { int argc; char** argv; int ret; } MainArgs;
DWORD WINAPI thread_main(LPVOID param) {
    MainArgs* args = (MainArgs*)param;
    args->ret = fides_main(args->argc, args->argv);
    return 0;
}
int main(int argc, char** argv) {
    MainArgs args = { argc, argv, 0 };
    HANDLE h = CreateThread(NULL, 256*1024*1024, thread_main, &args, 0, NULL);
    if (!h) return 1;
    WaitForSingleObject(h, INFINITE);
    CloseHandle(h);
    return args.ret;
}
"@
$content = $content + $wrapper
[System.IO.File]::WriteAllText("release\fides-c_wrapped.c", $content, [System.Text.Encoding]::ASCII)
```

**Why not PE header patching?** PE header patching (modifying
`SizeOfStackReserve` in the PE optional header) was attempted but was
fragile — some patched binaries were corrupted and crashed with
`0xC0000005`. The thread-based wrapper is more reliable and doesn't require
byte-level PE manipulation.

#### Step 4: Compile the C to an executable with tcc

```bash
C:\tcc\tcc.exe release\fides-c_wrapped.c -o release.fid.exe -lws2_32
```

**Why tcc?** It is fast, produces small binaries, and is what the bootstrap
pipeline currently targets. The Fides compiler can also target gcc/clang
when using the Rust host (`fidesc build`), but the self-hosted `main.fd`
currently hard-codes tcc.

**Why `-lws2_32`?** The runtime uses Windows sockets for some networking
primitives. Linking `ws2_32` satisfies those imports.

#### Full build sequence (session 15)

```powershell
# 1. Build Rust compiler
cargo build --release

# 2. Generate first-gen C
.\target\release\fidesc.exe dump-c compiler\main.fd > release\fides-c.c

# 3. Convert UTF-16 → UTF-8
$content = [System.IO.File]::ReadAllText("release\fides-c.c", [System.Text.Encoding]::Unicode)
[System.IO.File]::WriteAllText("release\fides-c.c", $content, [System.Text.UTF8Encoding]::new($false))

# 4. Apply stack wrapper (see Step 3 above)
# → produces release\fides-c_wrapped.c

# 5. Compile with TCC
C:\tcc\tcc.exe release\fides-c_wrapped.c -o release.fid.exe -lws2_32

# 6. Test
.\release.fid.exe dump-c release\test_simple.fd   # should EXIT 0

# 7. Self-host: gen 1 → gen 2
.\release.fid.exe dump-c compiler\main.fd > release\fides-c2.c
# (repeat steps 3-5 for fides-c2.c → fides-c2_new.exe)

# 8. Self-host: gen 2 → gen 3
.\release\fides-c2_new.exe dump-c compiler\main.fd > release\fides-c3.c
# (repeat steps 3-5 for fides-c3.c → fides-c3.exe)

# 9. Verify gen 3 works
.\release\fides-c3.exe dump-c release\test_simple.fd   # should EXIT 0
```

---

### 1b. PE Emitter Pipeline (session 5+)

The self-hosted compiler now has a `build` command that generates native
Windows PE executables **directly from `.fd` source** — no tcc, no gcc, no
external linker. This uses the PE emitter ported in session 5
(`compiler/pe.fd`).

#### Build a PE executable

```bash
.\release.fid.exe build tests\hello_pe.fd -o release\hello_pe.exe
.\release\hello_pe.exe
# → Hello from PE!
```

#### How it works

1. The `build` command in `main.fd` compiles the `.fd` source through the
   normal pipeline (lex, parse, typecheck, lower, mono).
2. Instead of generating C and invoking tcc, it calls `PeCodeGen` from
   `compiler/pe.fd` to generate x86-64 machine code + PE file format.
3. The PE bytes are hex-encoded (because `p_file_write` truncates at null
   bytes) and written to a `.hex` file.
4. PowerShell decodes the hex file to binary and writes the `.exe`.

#### Current limitations

The PE emitter currently supports:
- ✅ PE header generation, import table (GetStdHandle, WriteFile, ExitProcess)
- ✅ Entry point → main, function prologues/epilogues, stack frames
- ✅ `p_print_str`, `p_print_int`, `p_print_float`, `p_print_bool`, `p_panic`
- ✅ String literals (.rdata section)
- ✅ Basic arithmetic, local variables, control flow
- ✅ String concat (`p_str_concat`), int/bool/float→string
- ✅ Vec runtime (new/push/len/get/pop/set/first/last/...)
- ✅ HashMap runtime (new/insert/get/contains/...)

The PE emitter does NOT yet support:
- ⏳ Full struct/enum support (memcpy, SRET)
- ⏳ Float arithmetic operations (xmm ops)
- ⏳ Many-arg function calls (>4 args)
- ⏳ Full compiler source compilation
- ⏳ 20+ runtime stubs return placeholder values (see § Architecture, pe.fd section)

See SH-13 sub-tasks in `docs/roadmap/roadmap.md` for the full list.

---

### 2. Bootstrap Pipeline

#### Goal: prove the self-hosted compiler is self-reproducing

```
stage0: target/release/fidesc.exe (Rust compiler)
   │
   ├─► ccompile compiler/main.fd → stage1.c (release/fides-c.c)
   │
stage1: release.fid.exe (C compiled from stage1.c)
   │
   ├─► ccompile compiler/main.fd → stage2.c
   │
stage2: release/stage2.exe
   │
   ├─► ccompile compiler/main.fd → stage3.c
   │
compare: diff stage2.c stage3.c  → must be identical
```

If `stage2.c` and `stage3.c` are identical, the compiler can reproduce
itself — this is the bootstrap fixpoint.

#### Why the fixpoint matters

It catches "compiles but wrong output" bugs that assertion tests miss. If
the self-hosted compiler generates C that is functionally different from
the Rust compiler's C output, the fixpoint will diverge.

#### Current status (session 15)

**BOOTSTRAP FIXPOINT ACHIEVED.** `fides-c2.c` is byte-identical to
`fides-c3.c` (SHA256-identical, 701,100 lines, 19.8MB). The self-hosted
compiler reproduces itself exactly across generations.

**Self-compilation works (session 4).** `fides-c.exe` successfully compiles
`compiler/main.fd` → `fides-c2.exe`. **PE emitter works (session 5).**
`fides-c.exe build tests\hello_pe.fd` → working `.exe` without tcc.
**Linux native codegen ported (session 8)** — `peephole.fd`, `regalloc.fd`,
`codegen_x64.fd` build OK but are not yet integrated. **`.exe` extension
auto-appended (session 8)** — no more manual `Copy-Item` needed.
**Category B codegen parity resolved (session 14)** — B5 (float bits) fixed,
B6/B7/B8 verified at parity.
**Category A crash blockers resolved (session 15)** — TCC 4KB stack overflow
fixed with 256MB stack wrapper; type_eq str_equal fix; large match split;
non-ASCII → ASCII.
**Category E fixpoint stability resolved (session 15)** — c2==c3 proves no
non-determinism.

The fixpoint verification command:

```bash
# fides-c2 (self-compiled) compiles the compiler → fides-c3
.\release\fides-c2_new.exe dump-c compiler\main.fd > release\fides-c3.c
# Compare
diff release\fides-c2.c release\fides-c3.c
# Result: 0 differences out of 701,100 lines — BYTE IDENTICAL
```

---

### 3. Test Commands

#### Full Rust compiler verification

```bash
./verify.sh
```

Runs `cargo test`, the interpreter, C codegen, and PE emitter over the
full `tests/` suite. **Run this before and after any session.**

Save a new baseline:

```bash
./verify.sh --save
```

Quick build check (no tests):

```bash
./verify.sh --quick
```

#### Self-host verification

```bash
./verify-selfhost.sh
```

Runs every `tests/selfhost/*.fd` on three backends:
- Interpreter (`fidesc run`)
- C Codegen (`fidesc run --backend c`) — in-process only
- PE Emitter (`fidesc run --backend pe`) — in-process only

Compares results against `tests/selfhost/baseline/test_results.txt`.

Options:

```bash
./verify-selfhost.sh --save    # update baseline
./verify-selfhost.sh --quick   # build only
./verify-selfhost.sh --verbose # full output
./verify-selfhost.sh --list    # list tests
```

#### Single test

```bash
./target/release/fidesc run tests/<name>.fd
./target/release/fidesc run tests/<name>.fd --backend c
./target/release/fidesc run tests/<name>.fd --backend pe
```

#### Differential tests

```bash
.\verify-stdout-diff.ps1        # compare stdout across backends
.\verify-stdout-diff.ps1 -Quick # skip interpreter
.\verify-stdout-diff.ps1 -File test_borrowck
.\verify-stdout-diff.ps1 -Verbose

.\verify-stage-diff.ps1         # compare per-stage output
.\verify-stage-diff.ps1 -File test_borrowck
.\verify-stage-diff.ps1 -Stage c    # C output only
.\verify-stage-diff.ps1 -Stage ir   # IR output only
.\verify-stage-diff.ps1 -Stage tokens # tokens only
```

#### Debug a broken layer

```bash
./scripts/bisect.sh <test_name>
```

Tells you which layer (lexer, IR, C codegen, etc.) first diverges.

---

### 4. What Each Test Suite Does

#### `tests/`

The main language test suite, run against the Rust compiler's three backends:
- **Interpreter** (`run`): reference oracle
- **C Codegen** (`run --backend c`): C99 backend
- **PE Emitter** (`run --backend pe`): native x86-64 backend

Subdirectories:
- `tests/bootstrap/capability/`: capability tests
- `tests/bootstrap/effects/`: effect system tests
- `tests/bootstrap/regions/`: region tests
- `tests/bootstrap/safety/`: expected-fail safety tests (panic on OOB, div by zero, etc.)

#### `tests/selfhost/`

23 self-host tests for the Fides compiler modules:
- `test_ast.fd`
- `test_borrowck.fd`
- `test_codegen_c.fd`
- `test_compiler.fd`
- `test_diag.fd`
- `test_fmt.fd`
- `test_fmt_debug.fd`
- `test_fmt_diff.fd`
- `test_fmt_real.fd`
- `test_fmt_tok.fd`
- `test_ir.fd`
- `test_json.fd`
- `test_json_min2.fd`
- `test_json_ref.fd`
- `test_json_ref2.fd`
- `test_lower.fd`
- `test_mono.fd`
- `test_opt.fd`
- `test_pir.fd`
- `test_struct_lit.fd`
- `test_struct_simple.fd`
- `test_struct_str.fd`
- `test_token.fd`

#### `tests/selfhost/`

Probe tests for self-hosting behavior:
- `probe2.fd`
- `probe_argc.fd`
- `probe_argc2.fd`
- `probe_import.fd`
- `probe_import2.fd`
- `probe_import3.fd`
- `probe_import4.fd`
- `probe_parser_direct.fd`
- `probe_parser_direct2.fd`
- `probe_parser_simple.fd`
- `test_fmt_harness.fd`
- `test_lexer_harness.fd`
- `test_mut.fd`
- `test_parser_harness.fd`
- `test_sl_debug.fd`
- `vec_expr_test.fd`
- `vec_fieldinit_test.fd`

---

### 5. Directory Layout

```
fid/
├── compiler/             # Self-hosted Fides compiler modules (68 .fd files, 158,516 lines, Nov 9; list below is from session 16)
│   ├── main.fd           # CLI entry point (ccompile + build + lsp + test + fmt + ...)
│   ├── compiler.fd       # Pipeline driver
│   ├── token.fd          # Token kinds (133 variants)
│   ├── lexer.fd          # Tokenizer
│   ├── ast.fd            # AST types + Box<T> unbox helpers
│   ├── parser.fd         # Recursive-descent parser
│   ├── typechecker.fd    # Type/effect/region/capability/safety checker
│   ├── lower.fd          # AST → SSA IR lowering
│   ├── ir.fd             # IR types, instructions, modules
│   ├── pir.fd            # Multi-level PIR (L1→L2→L3)
│   ├── opt.fd            # PIR L2 optimizer (20+ passes)
│   ├── mono.fd           # Monomorphization
│   ├── borrowck.fd       # Borrow checker
│   ├── codegen_c.fd      # C99 code generator + runtime
│   ├── pe.fd             # PE emitter — native x86-64 + PE file writer (session 5)
│   ├── peephole.fd       # Peephole optimizer — 9 passes (session 8, not yet integrated)
│   ├── regalloc.fd       # Linear scan register allocator (session 8, not yet integrated)
│   ├── codegen_x64.fd    # PIR L3 → x86-64 AT&T asm, Linux (session 8, not yet integrated)
│   ├── comptime_eval.fd  # Comptime eval — full port (session 7)
│   ├── flat_ast.fd       # Flat AST serialization (session 9)
│   ├── lsp.fd            # LSP server — JSON-RPC over stdio (session 9)
│   ├── fmt.fd            # Source formatter
│   ├── diag.fd           # Structured diagnostics
│   ├── json.fd           # JSON parser/serializer
│   └── inc.fd            # Incremental compilation cache
├── docs/reference/       # This documentation (selfhost.md, known-bugs.md, ...)
├── release/              # Self-hosted build artifacts (gitignored)
├── src/                  # Rust reference compiler, frozen (57 .rs files, 112,511 lines)
├── stdlib/               # Fides standard library (146 .fd modules, 51,496 lines)
├── tests/bootstrap/      # Language tests (543 .fd files)
├── tests/selfhost/       # Self-host module tests (51 .fd files)
├── tests/selfhost/       # Bootstrap probe tests
├── target/               # Rust build artifacts
├── verify.sh             # Full Rust compiler verification
├── verify-selfhost.sh    # Self-host verification
├── verify-fixpoint.ps1   # Bootstrap fixpoint (Windows)
├── verify-stdout-diff.ps1
├── verify-stage-diff.ps1
└── build-selfhost.ps1    # Build the self-hosted compiler (Windows)
```

---

### 6. Windows-Specific Notes

#### tcc location

TCC is installed at `C:\tcc\`:
- `C:\tcc\tcc.exe` — main compiler
- `C:\tcc\lib\ws2_32.def` — Windows sockets import library
- `C:\tcc\lib\libtcc1-64.a` — 64-bit TCC runtime
- `C:\tcc\include\` — C headers

#### Why standalone PE/C binaries can fail

`verify-selfhost.sh` runs the C and PE backends in-process because standalone
binaries can segfault:
- C binaries can segfault on complex string concatenation when stdout is
  redirected.
- PE binaries can segfault when stdout is redirected.

In-process `run --backend c` / `run --backend pe` avoids these issues.

#### Stack size

The self-hosted compiler needs a large stack. Two mechanisms:
1. **PE stack patching** after `tcc` links the executable (256 MB reserve).
2. **Windows 16 MB stack thread wrapper** in `src/codegen_c.rs` (`real_main` in a `CreateThread` with 16 MB) for Rust-compiled binaries.


## Language Constraints

*From `selfhost.md` § Language Constraints.*

> What the Fides language must respect, and how the self-hosted compiler
> handles each feature.
>
> **Last updated:** Sep 6, 2026 (session 16 — Tier 3 OS features added: @cpu_local, inline asm clobbers, --no-std, UEFI bindings)

---

### 1. Types

#### Primitive types

```fides
pub enum Type {
    I8, I16, I32, I64, I128,
    U8, U16, U32, U64, U128,
    F32, F64, Bool, Str, Void, Never, Infer,
    ...
}
```

- `I128` and `U128` are lowered to `IrType::I64` in the current Fides lowerer.
- `Infer` is used where the type has not been resolved yet.
- `Never` becomes `IrType::Void`.
- `Str` is a pointer to `u8` (`IrType::Ptr(Box::new(IrType::U8))`).

#### Float constants (session 14 — B5 fix)

`ConstFloat` in IR stores the raw `u64`/`u32` bit pattern, not the decimal
value. The C backend emits union compound literals to reinterpret the bits:

```c
// f64
((union { uint64_t u; double f; }){ .u = 0x7FF8000000000000ULL }).f
// f32
((union { uint32_t u; float f; }){ .u = 0x7FC00000U }).f
```

This preserves NaN payloads, `+inf`/`-inf`, and `-0.0` bit patterns exactly.
The runtime functions `p_f64_to_bits` and `p_f32_to_bits` are registered as
externs in `Lowerer_register_runtime`.

#### Named types

`Type::Named(str)` is resolved by the lowerer:
- If the name is a known alias, the alias is followed recursively.
- If the name is `Vec`, `HashMap`, `Tensor`, or `Allocator`, it becomes `IrType::Opaque(name)`.
- Otherwise it becomes `IrType::Struct(name)`.

**Critical rule:** Enums are not an `IrType`. They are lowered to `IrType::Struct(name)` and given an `StructLayout` with a tag field and payload fields.

#### Composite types

| Fides syntax | AST representation | IR type | Notes |
|--------------|-------------------|---------|-------|
| `&T` / `&mut T` | `Type::Ref(Box<Type>, is_mut)` | `IrType::Ptr(Box<IrType>)` | References are always pointers in IR. |
| `*mut T` / `*const T` | `Type::RawPtr(Box<Type>, is_mut)` | `IrType::Ptr(Box<IrType>)` | `is_mut` only affects borrow-checking, not IR. |
| `[T; N]` | `Type::Array(Box<Type>, i64)` | `IrType::Array(Box<IrType>, i64)` | Array size is an `i64` literal. |
| `[T; NAME]` | `Type::ArrayConst(Box<Type>, str)` | `IrType::Array(Box<IrType>, 0)` | Size is looked up as a constant. |
| `Foo<T, U>` | `Type::Generic("Foo", Vec<Type>)` | depends | `Vec`, `HashMap`, `Tensor`, `Future` are special-cased. |
| `(T, U)` | `Type::Tuple(Vec<Type>)` | `IrType::Struct("Tuple_N")` | Lowered as a struct with generated name. |
| `fn(T, U) -> V` | `Type::Func(Vec<Type>, Box<Type>)` | `IrType::I64` | Function values are pointers. |

#### `Box<T>`

`Box::new(x)` is lowered to a `p_malloc` of `ir_type_size(x)`, then the value is `Store`d into the returned pointer.

`Box<T>` in the self-hosted compiler's own source is used for recursive AST and IR fields (e.g. `Box<Expr>`, `Box<Type>`, `Box<IrType>`). Matching on `Box<T>` auto-unboxes in Fides, but the extracted field is still a `Box`, so explicit unbox helpers are needed:
- `box_type_unbox()`
- `box_expr_unbox()`
- `box_ir_type_unbox()`
- `box_ir_type_size()`, `box_ir_type_align()`, `box_ir_type_eq()`, `box_ir_type_to_str()`

---

### 2. Effects

#### `Effect` enum

```fides
pub enum Effect {
    Alloc, IO, Net, Spawn, Diverge,
    Read, Write, Diff, Noise, Comm,
}
```

#### `EffectSet`

Implemented as `Vec<Effect>` with `add`, `add_all`, `contains`, `is_pure`, `is_read_only`, `has_side_effects`.

#### Function effect syntax

```fides
fn foo() !IO { ... }           // one effect
fn bar() !{IO, Net} { ... }    // multiple effects
fn pure() !{} { ... }          // no effects
```

#### Runtime effects

The lowerer hard-codes `Effect` values for `p_*` builtins in `runtime_effects(name)`:
- `Effect::Write` for mutating `vec`/`map`/string/atomic/mutex/RC/KV-cache/DP operations.
- `Effect::Alloc` for `p_vec_new`, `p_malloc`, `p_alloc`, `p_free`, string ops, tensor ops.
- `Effect::IO` for `print`/`eprint`/`write`/`chan`/`file`/`socket`/`dl`/`all_reduce`/etc.

#### Instruction effects

- `Inst::Alloca` → `Alloc`
- `Load` → `Read`; `Store` → `Write`
- `Call`/`CallPtr`/`DynCall` carry their `EffectSet`
- `Print` → `IO` + `Read`
- `Panic` / `Unreachable` → `Diverge`
- `RegionBegin/End` → `Alloc` (+ `Write` on `End`)
- `TxnBegin/End` → `Write`
- `GradBegin/End/Backward` → `Diff`

#### Function-level aggregation

After lowering a function, every instruction's effects are unioned into `func_effects` and stored in `module.function_effects`. This is required for the optimizer to avoid deleting side-effecting calls.

---

### 3. Regions

```fides
pub enum Region {
    Stack, Heap, Static, Named(str),
}
```

#### Syntax

```fides
region name { ... }
region gpu name { ... }
region kv_cache name window(W) { ... }
region name with budget(N) { ... }
```

#### Lowering

- `Lowerer_current_region()` returns `Region::Named(top)` if the stack is non-empty, otherwise `Region::Stack`.
- `Inst::Alloca(ty, region)` uses the current region.
- Array literals and other local allocations default to stack unless inside a `region` block.
- `RegionBegin/End` instructions bracket the block and are emitted around the body.

---

### 4. Pattern Matching

#### `Pattern` enum

```fides
pub enum Pattern {
    Wildcard,
    IntLit(i64),
    BoolLit(bool),
    Ident(str),
    EnumVariant(str, str, Vec<Binding>),
    StrLit(str),
    Range(Box<Pattern>, Box<Pattern>),
}
```

#### Syntax

```fides
match expr {
    pattern => { ... }
    pattern if guard => { ... }
}
```

#### Lowering `Lowerer_lower_match`

1. Lower the scrutinee to `match_val`.
2. Create a `merge_block`.
3. For each arm, create an `arm_block` and a `next_block`.
4. Emit pattern-specific tests:
   - `Wildcard` → unconditional `Jump(arm_block)`
   - `IntLit`/`BoolLit` → `BinOp(Eq)` + `Branch`
   - `Ident` → bind the whole value, `Jump`
   - `StrLit` → call `p_str_equal`
   - `Range` → lower/upper bound comparisons
   - `EnumVariant` → load `__tag` at field 0, compare to variant index, then `Branch`
5. In the `EnumVariant` arm block, each binding is bound with:
   ```fides
   var field_ptr: i64 = Lowerer_emit_inst(self, Inst::GetFieldPtr(match_val, bi + 1, field_ty));
   var field_val: i64 = Lowerer_emit_inst(self, Inst::Load(field_ptr, field_ty));
   Lowerer_bind_local(self, binding.name, field_val);
   ```
6. After each arm, bindings are truncated to saved lengths so arm-local variables do not leak to later arms.

#### Current behavior (session 14 — pointer-based approach)

Under the pointer-based enum payload approach (Coordination section [D1]),
`Inst::Load` reads the 8-byte pointer to the field's data. This is correct
for all field types — no `load_enum_field()` or `memcpy` is needed. See
`calling-conventions.md` §5 for details.

---

### 5. Closures

#### AST

```fides
Expr::Closure(Vec<Param>, Vec<Stmt>, OptType)
```

#### Lowering

1. Find free variables (captures).
2. Create a lifted function: `(env_ptr: *u8, ...own_params) -> ret`.
3. At creation site: `alloca` env struct, store captures, return struct ptr.
4. `CallPtr` passes `env_ptr` as the first argument.

#### Environment struct

`Lowerer_make_fn_env`:
- Allocates `(1 + captures.len()) * 8` bytes.
- Field 0 stores the function-name pointer.
- Fields `1..N` store each captured value.
- The created `__env` struct is returned as a pointer.

#### Calling convention

- Captured variables are loaded from the env struct inside the closure:
  `GetElemPtr(env_ptr, field_index, I64)` then `Load`.
- `CallPtr` is used for actual calls and the env pointer is passed as the first argument.

---

### 6. Impl Blocks and Traits

#### AST

```fides
pub struct ImplDef {
    pub struct_name: str,
    pub type_params: Vec<str>,
    pub methods: Vec<Func>,
}

pub struct TraitImpl {
    pub trait_name: str,
    pub type_name: str,
    pub type_params: Vec<str>,
    pub methods: Vec<Func>,
    pub associated_types: Vec<AssocTypeBinding>,
}
```

#### Syntax

```fides
impl Type {
    fn method(...) -> T { ... }
}

impl Trait for Type {
    type Assoc = T;
    fn method(...) -> T { ... }
}
```

#### Lowering

1. **Register impl methods**: insert `(struct_name, method_name) -> Func` and `(type_name, method_name) -> Func` into `impl_methods`.
2. **Pre-register mangled `IrFunction`**: `StructName_method` / `TypeName_method` so calls can resolve return types.
3. **Register trait methods / impls** in `module.trait_methods` and `module.trait_impls`.
4. **Lower methods**: set `current_self_type` to the struct/type name so `Self` resolves correctly in params and return types.

#### Method call resolution

- Method calls become `MethodCall(receiver, method, args)`.
- Lowerer resolves receiver type and constructs `mangled_name = type_name + "_" + method`.
- User functions get a dummy `__env` first argument; extern/runtime functions do not.

---

### 7. Extern / FFI

#### AST

```fides
pub struct ExternDecl {
    pub abi: str,
    pub c_name: str,
    pub pledge_name: str,
    pub params: Vec<Param>,
    pub ret_ty: Type,
    pub include: OptStr,
    pub link_lib: OptStr,
}
```

#### Syntax

```fides
extern "C" {
    include "header.h"
    link "ws2_32"
    fn c_name as pledge_name(params) -> ret;
}
```

#### Lowering

- `pledge_name` is the name used inside Fides.
- `c_name` is the actual C symbol.
- Externs are added to `module.extern_functions` with their parameter and return types.
- Extern calls do **not** receive the `__env` dummy parameter; only user-defined functions do.

---

### 8. Defer

#### Syntax

```fides
defer {
    ...
}
```

#### Lowering

- `Stmt::Defer(body, _line)` pushes `StmtVec_from(body)` onto `defer_stack`.
- `Lowerer_emit_defers` emits the stored defer bodies in reverse order (LIFO) at:
  - Normal function end
  - `return` statement
  - `try` error propagation (when not inside a `try` block)

---

### 9. Try Operator and Try Blocks

#### Syntax

```fides
let x = maybe_fails()?;       // Try operator
let y = try {                 // Try block
    ...
};
```

#### Lowering `Expr::Try`

1. Lower the inner expression to `val`.
2. Load the tag (field 0) and `Branch(is_ok, ok_block, err_block)`.
3. **Ok path**: load the unwrapped value (field 1) and continue.
4. **Err path**:
   - If inside a `try` block: copy the result into the try's err slot and jump to `error_block`.
   - If not inside a `try` block: emit defers, then return the error via SRET.

#### `TryContext`

```fides
pub struct TryContext {
    pub error_block: i64,
    pub error_result_slot: i64,
}
```

`try_stack: Vec<TryContext>` tracks active `try` scopes.

---

### 10. Return and the SRET Convention

#### Non-struct return

```fides
return value;
```

Emits `Inst::Return(OptValueId::Some(value))`.

#### Struct return

For a function returning a struct:
1. A hidden first parameter `sret: *Struct` is added to the IR function.
2. The last expression is lowered to a value.
3. `Lowerer_emit_sret_copy(src_ptr)` copies the value to the `sret` pointer.
4. `Inst::Return(OptValueId::None)` is emitted.

The C backend sees a `void` function and the first argument is the output buffer.

---

### 11. Enums

#### Source syntax

```fides
pub enum ParseResultExpr {
    Ok(Expr),
    Err(str),
}
```

#### IR layout (pointer-based approach — D1)

Enums are lowered to a `StructLayout`:
- Field 0: `__tag` at offset 0 (i64)
- Field 1..N: `__data0` at 8, `__data1` at 16, etc. (each is an 8-byte pointer)
- Size: `(1 + max_variant_arity) * 8`
- Alignment: 8

#### Payload size

Each field in the largest variant gets exactly 8 bytes (a pointer/word),
regardless of the field's actual type. Struct/enum-typed fields are
heap-allocated and the pointer is stored in the payload slot. This is the
pointer-based approach (Coordination section [D1]).

#### Variant index

Variants are indexed in source order, starting at 0. `Ok` is 0, `Err` is 1 in the `ParseResultExpr` example.

---

### 12. Structs

#### Source syntax

```fides
pub struct Point {
    pub x: i64,
    pub y: i64,
}
```

#### IR layout

- Fields are laid out sequentially with 8-byte alignment.
- Each field is at least 8 bytes.
- Struct-typed fields use the full size from the module's `StructLayout`.
- Total size is rounded up to an 8-byte multiple.

---

### 13. Generics

#### Source syntax

```fides
fn foo<T>(x: T) -> T { ... }
fn bar<T, U>(x: T, y: U) { ... }
```

#### Lowering

- `mono.fd` clones generic functions for concrete type instantiations.
- `subst_*` helpers replace type parameters.
- Generic function bodies are not lowered until a concrete instantiation is found.
- `Lowerer_resolve_type` recursively resolves `Type::Generic(name, args)` by
  resolving each arg via `Lowerer_resolve_type` before returning (session 14).
  This fixes nested generic type alias expansion.

---

### 14. Tuples

#### Source syntax

```fides
let t: (i64, str) = (1, "x");
```

#### Lowering

- `Type::Tuple(Vec<Type>)` is lowered to `IrType::Struct("Tuple_N")` where `N` is the arity.
- Tuple fields are unnamed and accessed by index.


## Path to Fixpoint

*From `selfhost.md` § Path to Fixpoint.*

> Companion to `docs/roadmap/roadmap.md`.
> The full plan with debugging workflows is there. This is the short version.
>
> Last updated: Sep 6, 2026 (session 16 — Tier 3 OS features added: @cpu_local, inline asm clobbers, --no-std, UEFI bindings. Bootstrap fixpoint preserved.).

---

### Where We Are

```
✅ Lexer — works (stdlib/core.fd tokenized)
✅ Parser — works (stdlib + user code parsed)
✅ TypeChecker — works (hello.fd type checks ok)
✅ Lowerer — works (hello.fd lowered to IR)
✅ CCodeGen — works (hello.fd → C code → tcc → correct output)
✅ Self-compilation — WORKS (fides-c.exe → fides-c2.c, EXIT 0)
✅ PE emitter — WORKS (fides-c.exe build → native .exe without tcc)
✅ HashMap import resolution — O(1) lookups (session 6)
✅ Cross-module enum constructors — MethodCall→EnumLit fallback (session 6)
✅ comptime_eval.fd — FULL PORT (process_gen, substitute, I-5A, I-5D, reflection)
✅ pe.fd runtime stubs — 25+ stubs (print, string, vec, map, float, bool)
✅ peephole.fd — PORTED (1,064 lines, from src/peephole.rs 903 lines)
✅ regalloc.fd — PORTED (1,181 lines, from src/regalloc.rs 1,088 lines)
✅ codegen_x64.fd — PORTED (1,942 lines, from src/codegen_x64.rs 1,940 lines)
✅ lsp.fd — PORTED (1,082 lines, from src/lsp.rs 916 lines) — JSON-RPC over stdio
✅ flat_ast.fd — PORTED (1,259 lines, from src/flat_ast.rs 741 lines)
✅ Module-level `var`/`pub var` — IMPLEMENTED in Rust compiler (session 11)
✅ Full static analysis of all 25 compiler/*.fd files — COMPLETED (session 11)
✅ DCE removed from self-hosted ccompile — 1814 functions (matches Rust's 1813)
✅ TypeChecker_compute_func_effects rewritten — no extra boxing
✅ C codegen return type casts — (int64_t)/(void*)/(double) added
✅ Type alias resolution in Lowerer_mangle_method — StrStack→Vec resolved
✅ Category B codegen parity — B5 FIXED, B6/B7/B8 AT PARITY (session 14)
✅ Float bit-pattern preservation — p_f64_to_bits/p_f32_to_bits + union literals
✅ Lowerer_resolve_type recursive generic arg resolution (session 14)
✅ Category A crash blockers RESOLVED (session 15):
  - A1 root cause: TCC 4KB stack → stack overflow → fixed with 256MB stack wrapper
  - type_eq string comparison: pointer == → str_equal (Named/Generic/Dyn/Assoc)
  - Large match expression codegen: split into helper functions (mono.fd, comptime_eval.fd)
✅ Self-hosting across 3 generations VERIFIED (session 15):
  - Gen 1 (fides-c.exe) → builds gen 2 C → runs test_simple.fd ✓
  - Gen 2 (fides-c2_new.exe) → builds gen 3 C → runs test_simple.fd ✓
  - Gen 3 (fides-c3.exe) → runs test_simple.fd ✓
✅ **BOOTSTRAP FIXPOINT ACHIEVED** (session 15): fides-c2.c == fides-c3.c (0 diffs / 701,100 lines)
⬜ Native codegen integration — ported but not wired into compiler pipeline
⬜ Mirror var feature to compiler/*.fd — NOT YET DONE (Rust-only so far)
⬜ Fix P0 bugs in parser.fd and lexer.fd — IDENTIFIED but not yet fixed
⬜ Fix inc.fd global_cache_* using var feature — IDENTIFIED but not yet done
```

**Session 15 summary:** **BOOTSTRAP FIXPOINT ACHIEVED.** Self-hosting works
across 3 generations with byte-identical output (fides-c2.c == fides-c3.c,
0 diffs out of 701,100 lines). Four bugs were fixed:
1. TCC 4KB stack overflow → 256MB stack thread wrapper
2. `type_eq` pointer `==` for strings → `str_equal` (Named/Generic/Dyn/Assoc)
3. Large match expressions in `mono.fd`/`comptime_eval.fd` → split into helpers
4. Non-ASCII characters in compiler source → replaced with ASCII equivalents
   (corrupted Unicode `─`/`═`/`→`/`—` in comments/strings caused different
   output between generations; replacing with `-`/`=`/`->`/`--` fixed it)

**MILESTONE (session 15):** Self-hosting achieved across 3 generations:
```
Gen 1: Rust → fides-c.c → TCC → fides-c.exe     → runs test_simple.fd ✓
Gen 2: Gen 1 → fides-c2.c → TCC → fides-c2.exe  → runs test_simple.fd ✓
Gen 3: Gen 2 → fides-c3.c → TCC → fides-c3.exe  → runs test_simple.fd ✓
```
Three bugs fixed:
1. TCC 4KB stack overflow → 256MB stack thread wrapper
2. type_eq pointer == for strings → str_equal (Named/Generic/Dyn/Assoc)
3. Large match expressions in mono.fd/comptime_eval.fd → split into helpers

**MILESTONE (session 14):** Category B codegen parity resolved:
```
B5 (float bits)    — FIXED: p_f64_to_bits/p_f32_to_bits + union compound literals
B6 (print dispatch) — AT PARITY: all type cases match Rust codegen
B7 (runtime sigs)  — AT PARITY: p_malloc_persistent/p_arena_save/restore registered
B8 (Vec word_repr) — AT PARITY: lower_method_call uses word_repr(elem_ty)
Files changed: src/codegen_c.rs, src/lower.rs, compiler/codegen_c.fd, compiler/lower.fd
```

**MILESTONE (session 9):** LSP server ported to Fides:
```
compiler/lsp.fd (1,082 lines) — full LSP server ported from src/lsp.rs (916 lines)
  - JSON-RPC 2.0 over stdio (Content-Length framing)
  - initialize / shutdown / exit
  - textDocument/didOpen / didChange / didClose
  - textDocument/publishDiagnostics (via get_diagnostics)
  - textDocument/completion (keywords + symbols + dot-access methods/fields)
  - textDocument/hover (function/struct/enum/const/type/trait/method signatures)
  - textDocument/definition (source search for fn/struct/enum/const/type/cap)
  - textDocument/rename (whole-word occurrence search + WorkspaceEdit)
Runtime support added to codegen_c.fd:
  - p_read_line (stdin line read, NULL on EOF)
  - p_read_stdin (exact N-byte read for LSP body)
  - p_stdin_eof (feof check for client disconnect)
  - p_flush_stdout (fflush for immediate message delivery)
'lsp' command added to compiler/main.fd → lsp_run()
```

**MILESTONE (session 8):** Linux native codegen backend ported:
```
.\target\release\fidesc.exe build compiler\peephole.fd      → Built (OK)
.\target\release\fidesc.exe build compiler\regalloc.fd       → Built (OK)
.\target\release\fidesc.exe build compiler\codegen_x64.fd    → Built (OK)
```
Three files ported from Rust to Fides, all compile successfully:
- `peephole.fd` (1,064 lines) — 9 peephole optimization passes
- `regalloc.fd` (1,181 lines) — linear scan register allocator
- `codegen_x64.fd` (1,942 lines) — PIR L3 → x86-64 assembly (System V AMD64 ABI)
These form the Linux ELF64 native codegen pipeline. NOT yet integrated into
`compiler.fd`/`main.fd`. The Windows PE backend (`pe.rs`, 5704 lines) is a
separate self-contained codegen that does NOT use these files.

**MILESTONE (session 7):** comptime_eval.fd fully ported + pe.fd runtime stubs expanded:
```
.\target\release\fidesc.exe build compiler\comptime_eval.fd  → Built (OK)
.\target\release\fidesc.exe build compiler\pe.fd             → Built (OK)
.\target\release\fidesc.exe build compiler\compiler.fd       → Built (OK)
.\target\release\fidesc.exe build compiler\main.fd           → Built (OK)
.\target\release\fidesc.exe run tests\compiler\test_compiler.fd → All tests passed!
```

**MILESTONE (session 6):** Self-compile succeeds with all fixes:
```
.\release.fid.exe ccompile compiler\main.fd -o release\fides-c2_test.exe
→ [prof] typecheck: 4000ms
→ [prof] total build: 262000ms
→ [prof] codegen: 38000ms
→ Built native executable: release\fides-c2_test.exe
→ EXIT: 0  ✅
```

**MILESTONE (session 5):** The self-hosted compiler generates native PE executables:
```
.\release.fid.exe build tests\hello_pe.fd -o release\hello_pe.exe
→ Built native executable: release\hello_pe.exe
.\release\hello_pe.exe
→ Hello from PE!    ✅ CORRECT OUTPUT, EXIT CODE 0
```

**MILESTONE (session 4):** The self-hosted compiler compiles itself:
```
.\release.fid.exe ccompile compiler\main.fd -o release\fides-c2.exe
→ Built native executable: release\fides-c2.exe
```

The generated `fides-c2.exe` correctly compiles `test_strconcat.fd` → "hello world".

### The Steps to Fixpoint

#### Step 1: Fix Vec-of-struct truncation ✅ DONE

Fixed `map_str_str_get` crash. All Vec paths now handle struct elements.

#### Step 2: Port Fides lowerer gaps 1-5 ✅ DONE (session 3)

All 5 gaps ported to the Fides lowerer. Plus Gaps 6-11 ported in sessions 2-3.

#### Step 3: hello.fd compiles without crash ✅ DONE

#### Step 4: hello.fd output is correct ✅ DONE

Output: `Hello, Fides!50y is big`

#### Step 5: Self-compilation (stage1) ✅ DONE (session 4)

```bash
.\release.fid.exe ccompile compiler\main.fd -o release\fides-c2.exe
→ Built native executable: release\fides-c2.exe
```

**Six bugs were fixed in session 4 to achieve this:**
1. Enum `==` comparison bug — replaced with `*_eq()` helpers
2. O(n²) string concat in C codegen — added `p_sb_*` string builder
3. Duplicate function definitions — `IrModule_add_function` deduplicates
4. `p_str_equal` env_ptr mismatch — added `dummy_env` to call sites
5. C stdlib `remove` collision — added to `sanitize_c_name`
6. Vec method mangling for I64 receivers — AST type lookup + prefix fallback

#### Step 6: stage1 produces correct output ✅ DONE (session 4)

```bash
.\release.fid.exe ccompile test_strconcat.fd -o test_strconcat2
Copy-Item test_strconcat2 test_strconcat2.exe -Force
.\test_strconcat2.exe
→ hello world    ✅ CORRECT
```

#### Step 6b: PE emitter generates native executables ✅ DONE (session 5)

```bash
.\release.fid.exe build tests\hello_pe.fd -o release\hello_pe.exe
.\release\hello_pe.exe
→ Hello from PE!    ✅ CORRECT
```

**Three bugs were fixed in session 5 to achieve this:**
1. `X86Encoder_mov_imm32` ModRM bug — `mod=0` (memory) → `mod=3` (register)
2. `Inst::Print` type dispatch — always called `p_print_int` → now dispatches by IR type
3. Statement-dropping bug workaround — split long functions into smaller helpers

#### Step 6c: OptStr type resolution + HashMap imports ✅ DONE (session 6)

Three bugs were fixed to make self-compile succeed (EXIT 0):

1. **OptStr type resolution** — `OptStr::Some(input)` was parsed as `MethodCall`
   instead of `EnumLit` because the parser didn't know `OptStr` was a cross-module
   enum. Fixed in typechecker (`check_method_call` checks enum names), lowerer
   (`MethodCall` on enum name → delegate to `EnumLit`), and `infer_expr_type`.
2. **Duplicate `p_file_write_vec`** — `emit_runtime_impl` emitted a function body
   already in `emit_file_write_bin_impl`. Removed the duplicate.
3. **HashMap for import resolution** — replaced O(n) `c_str_vec_contains` scans for
   `loaded_module_names`, `struct_names`, `enum_names` with O(1) `HashMap_get`
   lookups. Import resolution dropped from ~238s to negligible.

#### Step 7: Self-hosting across generations ✅ DONE (session 15)

```bash
# Gen 1: Rust → C → TCC → fides-c.exe
.\target\release\fidesc.exe dump-c compiler\main.fd > release\fides-c.c
# (convert UTF-16→UTF-8, apply stack wrapper, compile with TCC)
.\release.fid.exe dump-c release\test_simple.fd   # ✓ EXIT 0

# Gen 2: Gen 1 → C → TCC → fides-c2_new.exe
.\release.fid.exe dump-c compiler\main.fd > release\fides-c2.c
# (convert, wrap, compile)
.\release\fides-c2_new.exe dump-c release\test_simple.fd   # ✓ EXIT 0

# Gen 3: Gen 2 → C → TCC → fides-c3.exe
.\release\fides-c2_new.exe dump-c compiler\main.fd > release\fides-c3.c
# (convert, wrap, compile)
.\release\fides-c3.exe dump-c release\test_simple.fd   # ✓ EXIT 0
```

**Three bugs were fixed in session 15 to achieve this:**
1. **TCC 4KB stack overflow** — TCC produces executables with a 4KB default
   PE stack reserve. The compiler's deeply recursive parser, type checker,
   and C codegen need far more stack. Fixed by wrapping `main` in a Windows
   thread with a 256MB stack (`CreateThread(NULL, 256*1024*1024, ...)`).
2. **`type_eq` string comparison** — `type_eq` in `compiler/typechecker.fd`
   used pointer `==` for string comparisons (`n1 == n2`), which compares
   addresses instead of content. This caused identical generic types
   (`Vec<Param>` vs `Vec<Param>`) to be rejected as incompatible. Fixed by
   replacing with `str_equal(n1, n2)` for Named, Generic, Dyn, and Assoc
   type variants.
3. **Large match expression codegen** — `subst_expr`/`subst_stmt` in
   `mono.fd` and comptime cases in `comptime_eval.fd` were split into helper
   functions (`subst_expr_part2`, `subst_stmt_part2`,
   `substitute_expr_comptime`, etc.) to avoid codegen issues with oversized
   match arms.

#### Step 7b: Strict fixpoint ✅ DONE (session 15)

```bash
# Compare gen 2 vs gen 3
diff release\fides-c2.c release\fides-c3.c
# Result: 0 differences out of 701,100 lines — BYTE IDENTICAL
```

The fixpoint was achieved by replacing all non-ASCII characters in the 25
`compiler/*.fd` source files with ASCII equivalents. The corrupted Unicode
characters (box-drawing `─`/`═`, arrows `→`, em-dashes `—`, and their
mojibake variants) in comments and string literals were causing different
output between generations because the Rust compiler and self-hosted compiler
handled multi-byte UTF-8 sequences differently in string literals.

**Replacements made:**
- `─` (U+2500) → `-`
- `═` (U+2550) → `=`
- `—` (U+2014) → `--`
- `→` (U+2192) → `->`
- `↔` (U+2194) → `<->`
- `≤` (U+2264) → `<=`
- `×` (U+00D7) → `x`
- `²` (U+00B2) → `^2`
- `│` (U+2502) → `|`
- `▶` (U+25B6) → `>`
- Mojibake sequences (`ΓöÇ` etc.) → `-`

**Remaining work (not blocking fixpoint):**
1. **P0 bugs in parser.fd and lexer.fd** (session 11 static analysis):
   - `lexer.fd` — duplicate empty `str_filter_underscores` stub
   - `parser.fd` — `box_expr_deref` references 8 non-existent `Expr` variants
   - `parser.fd` — missing `return` in 4 parse functions
   - `parser.fd` — duplicate `var has_forward` in `parse_layer`/`parse_model`
   - `parser.fd` — `parse_use` version-pin always returns Err
2. **`var` feature not yet mirrored to `compiler/*.fd`** — the Rust compiler
   now supports module-level `var`/`pub var`, but the Fides self-hosted
   compiler files have not yet been updated to parse, type-check, lower, and
   emit globals.

**Resolved blockers:**
- ~~Category A crash blockers~~ — DONE (session 15: stack wrapper + type_eq fix)
- ~~`.exe` extension~~ — FIXED (session 8).
- ~~LSP port~~ — DONE (session 9).
- ~~Pre-existing parse error at codegen_c.fd:973~~ — RESOLVED in earlier sessions.
- ~~Category B codegen parity gaps~~ — DONE (session 14: B5 fixed, B6/B7/B8 at parity).

#### Step 8: Native codegen integration (future)

Three Linux native codegen files are ported but not yet integrated:
- `compiler/peephole.fd` (1,064 lines) — peephole optimizer
- `compiler/regalloc.fd` (1,181 lines) — register allocator
- `compiler/codegen_x64.fd` (1,942 lines) — PIR L3 → x86-64 AT&T asm

**To integrate the Linux backend:**
1. Port `src/elf64.rs` → `compiler/elf64.fd` (ELF64 object file emitter)
2. Port `src/linker.rs` → `compiler/linker.fd` (linker with runtime)
3. Port `src/runtime_linux.rs` → `compiler/runtime_linux.fd` (Linux runtime)
4. Wire `codegen_x64::generate_asm()` + `peephole::optimize()` +
   `elf64::emit_object_file()` + `linker::link_with_runtime()` into
   `compiler.fd` as the `"x86-64"` target path
5. Add `--target x86-64` support to `main.fd`

**For Windows native codegen:**
- `pe.rs` (6,345 lines) is the actual Windows backend — self-contained, does
  NOT use `codegen_x64.rs`/`regalloc.rs`/`peephole.rs`
- `pe.fd` is already partially ported (4,845 lines) with 25+ runtime stubs
- To complete: port remaining `pe.rs` functionality to `pe.fd`
- `x86.rs` (1,381 lines) is DEAD CODE — skip it (see session 8 analysis)

### Rebuild Cycle

Every fix requires a full rebuild:

```bash
cargo build --release
# Generate first-gen C
.\target\release\fidesc.exe dump-c compiler\main.fd > release\fides-c.c
# Convert UTF-16 → UTF-8, strip BOM
# Apply stack wrapper (256MB thread — see § Build Pipeline)
C:\tcc\tcc.exe release\fides-c_wrapped.c -o release.fid.exe -lws2_32
# Self-host: gen 1 → gen 2
.\release.fid.exe dump-c compiler\main.fd > release\fides-c2.c
# Convert, wrap, compile → fides-c2_new.exe
# Self-host: gen 2 → gen 3
.\release\fides-c2_new.exe dump-c compiler\main.fd > release\fides-c3.c
# Compare fides-c2.c vs fides-c3.c for fixpoint check
```

### What Success Looks Like

No crash ≠ working. The three failure modes after crashes stop:

1. **Wrong output** — program runs but prints wrong thing → compare C output against bootstrap compiler
2. **Fixpoint divergence** — stage2.c ≠ stage3.c → non-determinism (HashMap order, uninit memory)
3. **Correct + stable** — stage2.c == stage3.c → **fixpoint achieved**

**We are at stage 3** (session 15): **FIXPOINT ACHIEVED.** Self-hosting
works across 3 generations, all produce correct output, and stage2.c ==
stage3.c (byte-identical, 0 diffs out of 701,100 lines). The bootstrap
fixpoint is complete.


## Capability Comparison

*From `selfhost.md` § Capability Comparison.*

> Evidence-based, verified against the actual codebase and test runs.
> Last updated: Sep 6, 2026 (session 16 — Tier 3 OS features added: @cpu_local, inline asm clobbers, --no-std, UEFI bindings. Bootstrap fixpoint preserved.). Based on test runs, source inspection, and docs.

---

### Verified Test Results (Sep 5, 2026 — session 15)

```
Interpreter:  209/211 passed
C Codegen:    198/211 matched
PE Emitter:   209/211 valid
Self-host:    23/23 pass (interp + PE)
Stdlib:       99 modules (28,370 LOC)
Source:       55 .rs files (91,490 LOC)
Self-host:    50 .fd modules (91,924 LOC)
Test files:   225+ in tests/ + 23 in tests/selfhost/
```

The interpreter and PE emitter are near-complete (209/211). The C codegen
backend is at 198/211 — most failures are related to the `var` feature
implementation (session 11) and not core codegen bugs.

---

### What You Can Build Today (Verified Working)

#### 1. Systems Programs
- CLI tools with sub-second compilation
- File I/O (read, write, stdin/stdout)
- Process management (basic)
- Inline assembly (x86-64)
- Raw pointers with `unsafe {}` enforcement
- Bare-metal flat binaries (PE emitter)

#### 2. Safe Systems Programs
- Memory-safe code via regions (no lifetime annotations)
- Effect-annotated functions (`!IO`, `!{IO,Net}`, `!{}`)
- Capability-restricted code (`sandbox { }`)
- Compile-time memory budgets (`region r with budget(4096)`)
- Borrow-checked code (field-wise, two-phase, NLL)
- `@medical` IEC 62304 compliance (no alloc, no recursion, no while)

#### 3. Data Processing
- Vec, HashMap, arrays, tuples
- Pattern matching (enum variants, ranges, guards)
- Closures with capture analysis
- Generic functions + monomorphization
- Traits + impls + trait objects (`dyn Trait`)
- `@derive(Debug/Clone/Serialize/Deserialize)`

#### 4. Concurrency
- OS threads (`spawn`)
- Bounded MPSC channels (`p_chan_*`)
- Async/await (stackless coroutines via state machine transform)
- `select` statement (CSP-style multi-channel)
- Executor runtime with non-blocking poll

#### 5. AI/ML
- Tensor type with shape checking
- Autograd (reverse-mode AD, gradient checkpointing)
- Quantization, KV cache, differential privacy
- Multi-rank sharding (simulated)
- SIMD stdlib

#### 6. Cross-Compilation
- x86-64 Linux (ELF64)
- ARM64 Linux (ELF64)
- RISC-V Linux (ELF64)
- x86-64 Windows (PE)
- All with zero external tools (no LLVM, no gcc, no external linker)

#### 7. Tooling
- LSP server (completion, hover, goto-definition)
- Code formatter (`fidesc fmt`)
- Linter (`fidesc lint`)
- Benchmarking (`fidesc bench`)
- Stack traces on panic
- C header parser → Fides FFI bindings
- Package manager (ark -- path/git deps, no registry yet)
- Doc generator (scribe)
- Coverage tool (veil)

#### 8. Security
- `@constant_time` (timing side-channel defense)
- `@tainted` (injection defense)
- Binary hardening by default (stack canaries, PIE, RELRO, NX stack)
- Capability-aware linking (`--caps` strips ungranted code)
- `sandbox { }` compile-time capability revocation

#### 9. Self-Hosting (self-compilation + native codegen achieved!)
- 25 modules rewritten in Fides (57K LOC, including PE emitter + Linux native codegen + LSP)
- All 25 modules compile + pass tests
- `release.fid.exe` builds via tcc
- **Self-compilation works** — `fides-c.exe` compiles `compiler/main.fd` → `fides-c2.exe` (Sep 9, 2026)
- `fides-c2.exe` correctly compiles `test_strconcat.fd` → "hello world"
- **PE emitter ported to Fides** — `compiler/pe.fd` generates native Windows PE executables without tcc (Sep 10, 2026)
- `fides-c.exe build tests\hello_pe.fd -o hello_pe.exe` → working .exe that prints "Hello from PE!"
- **Linux native codegen ported** — `peephole.fd` + `regalloc.fd` + `codegen_x64.fd` (session 8), builds OK but not yet integrated
- **LSP server ported to Fides** — `compiler/lsp.fd` (1084 lines) — JSON-RPC over stdio, diagnostics, completion, hover, definition, rename (session 9)
- **`.exe` extension auto-appended** — `ccompile` and `build` now produce `.exe` output without manual rename (session 8)
- **🎉 BOOTSTRAP FIXPOINT ACHIEVED** — fides-c2.c == fides-c3.c (SHA256-identical, 701,100 lines, session 15)
- **Three generations verified** — Rust → c1 → c2 → c3, all run test_simple.fd correctly (session 15)
- **Package manager ported to Fides** — `compiler/ark.fd` (740 lines) — self-hosted ark, compiled by fides-c3 (session 15)
- PE emitter currently supports simple programs; full compiler source support is in progress

---

### How Fides Compares — Honest Scorecard

#### vs C

| Dimension | C | Fides | Winner |
|-----------|---|-------|--------|
| Performance | 10/10 | 7/10 | C (LLVM/GCC has 20+ years of optimization) |
| Memory safety | 1/10 | 9/10 | **Fides** (regions + bounds + overflow checks) |
| Compilation speed | 8/10 | 10/10 | **Fides** (no LLVM, integrated linker) |
| Ecosystem | 10/10 | 2/10 | C (everything talks to C) |
| Cross-compilation | 6/10 (needs toolchain) | 9/10 (built-in, zero tools) | **Fides** |
| Security | 1/10 | 9/10 | **Fides** (capabilities, sandbox, hardening) |
| Hot reload | ❌ | ✅ | **Fides** |
| Metaprogramming | 2/10 (preprocessor) | 8/10 (comptime) | **Fides** |

**Where Fides wins:** Safety, compilation speed, security, hot reload, metaprogramming.
**Where C wins:** Performance, ecosystem, ubiquity, 50 years of code.

#### vs Rust

| Dimension | Rust | Fides | Winner |
|-----------|------|-------|--------|
| Performance | 9/10 | 7/10 | Rust (LLVM) |
| Memory safety | 9/10 | 9/10 | Tie (different approaches) |
| Safety UX | 6/10 (borrow checker fights) | 8/10 (regions, no fights) | **Fides** |
| Compilation speed | 2/10 (25-30 min builds) | 10/10 (sub-second) | **Fides** |
| Ecosystem | 8/10 (100k+ crates) | 4/10 (99 stdlib modules) | Rust |
| Concurrency safety | 9/10 (Send/Sync) | 4/10 (no data race prevention) | Rust |
| Async | 6/10 ("suffering") | 6/10 (effect handlers) | Tie |
| Hot reload | ❌ | ✅ | **Fides** |
| Tooling | 9/10 (rust-analyzer, cargo) | 6/10 (LSP, fmt, lint, no debugger) | Rust |
| Custom toolchain | ❌ (LLVM) | ✅ (everything custom) | **Fides** (unique) |
| Security | 6/10 | 9/10 (capabilities, sandbox) | **Fides** |

**Where Fides wins:** Compilation speed, safety UX, hot reload, security, custom toolchain.
**Where Rust wins:** Performance, ecosystem, concurrency safety, tooling maturity.

#### vs Zig

| Dimension | Zig | Fides | Winner |
|-----------|-----|-------|--------|
| Performance | 9/10 | 7/10 | Zig (LLVM) |
| Memory safety | 3/10 | 9/10 | **Fides** |
| Compilation | 7/10 | 10/10 | **Fides** |
| Ecosystem | 4/10 | 2/10 | Zig (growing) |
| Metaprogramming | 9/10 (comptime) | 8/10 (comptime) | Tie |
| Explicit allocators | ✅ | ✅ (regions) | Tie |
| Safety UX | 3/10 (discipline) | 8/10 (compile-time) | **Fides** |
| Hot reload | ❌ | ✅ | **Fides** |

**Where Fides wins:** Memory safety, compilation speed, hot reload, security.
**Where Zig wins:** Performance, ecosystem, LLVM optimizations.

#### vs Go

| Dimension | Go | Fides | Winner |
|-----------|-----|-------|--------|
| Performance | 7/10 (GC pauses) | 7/10 (no GC) | Tie |
| Memory safety | 6/10 (GC, no compile-time) | 9/10 (regions, compile-time) | **Fides** |
| Compilation | 8/10 | 10/10 | **Fides** |
| Ecosystem | 8/10 | 2/10 | Go |
| Concurrency | 9/10 (goroutines) | 6/10 (OS threads + channels) | Go |
| Simplicity | 9/10 | 7/10 | Go |
| Binary size | 8/10 | 7/10 | Go |
| Zero runtime | ❌ (GC, scheduler) | ✅ | **Fides** |

**Where Fides wins:** Memory safety, compilation speed, zero runtime, security.
**Where Go wins:** Ecosystem, concurrency, simplicity, adoption.

#### vs Python

| Dimension | Python | Fides | Winner |
|-----------|--------|-------|--------|
| Performance | 2/10 | 7/10 | **Fides** |
| Safety | 4/10 (runtime) | 9/10 (compile-time) | **Fides** |
| Compilation | N/A (interpreted) | 10/10 | N/A |
| Ecosystem | 10/10 | 2/10 | Python |
| AI/ML | 10/10 (PyTorch, etc.) | 6/10 (tensor, autograd) | Python |
| Ease of use | 10/10 | 7/10 | Python |
| Hot reload | ❌ | ✅ | **Fides** |

**Where Fides wins:** Performance, safety, compilation, hot reload.
**Where Python wins:** Ecosystem, AI/ML, ease of use, adoption.

---

### What No Other Language Has (Fides-Unique Features)

These are verified working today, not aspirational:

1. **`sandbox { }`** — compile-time capability revocation. Run untrusted plugins with proof they can't do IO. No other language does this at compile time.

2. **Compile-time memory budgets** — `region r with budget(4096)` is a compile error if exceeded. Zig's FixedBufferAllocator panics at runtime.

3. **Capability-aware linking** — `--caps "IO"` strips all Net-dependent functions from the binary. No other linker does this.

4. **`watch --diff`** — see what the optimizer changed when you edit one line. LLVM compilers are black boxes.

5. **`@constant_time`** — prevents timing side-channel attacks at the language level. No other language has this.

6. **`@tainted`** — prevents injection attacks (SQL, shell, HTML) at the language level.

7. **`@medical`** — IEC 62304 compliance (no alloc, no recursion, no while, must be @audit). No other language has medical device compliance built in.

8. **Region-based memory safety without borrow checker fights** — Rust rejects valid patterns; Fides doesn't.

9. **Custom toolchain with cross-compilation** — no LLVM, no gcc, no external linker. Zero external tools for x86-64/ARM64/RISC-V/Windows.

10. **Hot reload for a systems language** — change code while it runs, with `persist` variables that survive reloads.

---

### What Fides Cannot Do Yet (Honest Gaps)

#### Missing vs Rust
- No data race prevention (Send/Sync equivalent)
- No atomics
- No DWARF debug info / debugger
- No profiler
- No PGO (profile-guided optimization)
- No escape analysis
- Weaker auto-vectorization (constant trips only, no variable-trip)
- Smaller ecosystem (99 modules vs 100k+ crates)
- No registry for packages

#### Missing vs Go
- No goroutine-equivalent (green threads planned)
- No mature HTTP server
- No mature database drivers
- No mature regex engine
- Smaller stdlib

#### Missing vs Python
- No AI/ML ecosystem (PyTorch, TensorFlow, etc.)
- No Jupyter notebooks
- No web framework ecosystem
- No scientific computing libraries

#### Missing vs C
- No 50-year ecosystem
- No LLVM/GCC optimization quality
- No universal FFI (everything talks to C, not Fides)

#### Missing vs Zig
- No LLVM optimization quality
- Smaller community
- Less mature comptime (Zig's comptime is more polished)

---

### The Self-Hosting Milestone

**Self-compilation was achieved on Sep 9, 2026 (session 4).** The self-hosted
compiler (`fides-c.exe`) successfully compiles the entire 57K LOC compiler
source (`compiler/main.fd`) into a working second-generation binary
(`fides-c2.exe`). The generated binary correctly compiles test files.

**Native PE code generation was achieved on Sep 10, 2026 (session 5).** The
PE emitter was ported from Rust (`src/pe.rs`) to Fides (`compiler/pe.fd`,
4,845 LOC). The `build` command generates native Windows PE executables
directly from `.fd` source — no tcc, no gcc, no external linker.

**Category B codegen parity resolved (session 14).** Float bit-pattern
preservation (B5), print/println dispatch (B6), runtime signatures (B7),
and Vec word_repr (B8) are all at parity between Rust and self-hosted
compilers.

The next steps are:

1. ~~Fix `.exe` extension in self-hosted compiler output~~ — DONE (session 8)
2. ~~Category B codegen parity gaps~~ — DONE (session 14)
3. ~~Fix Category A crash blockers~~ — DONE (session 15: stack wrapper + type_eq fix + large match split + non-ASCII removal)
4. ~~Run the bootstrap fixpoint~~ — DONE (session 15: fides-c2.c == fides-c3.c, SHA256-identical, 701,100 lines)
5. **Mirror `var` feature to `compiler/*.fd` files**
6. **Gate debug prints** (287 `p_eprint_str` calls — cleanup, doesn't affect fixpoint)
7. **Close Rust vs self-hosted parity gap** (c1=706K vs c2=701K lines)
8. Extend PE emitter to support full compiler source (struct/enum, Vec/HashMap, floats)
9. Integrate Linux native codegen (port `elf64.rs`, `linker.rs`, `runtime_linux.rs`, wire into pipeline)

Once the fixpoint is achieved and the PE emitter supports the full compiler
source, Fides becomes a **self-reproducing compiler with a fully custom
toolchain** — the foundation for everything else. No more dependency on Rust
or tcc for building the compiler. The compiler can be built by itself, on
any platform it targets.

---

### Bottom Line

**Fides is a real, working systems programming language** with unique features
no other language has. It's not ready to replace Rust or C for production use
(the ecosystem is too small), but it has a clear niche:

- **Embedded AI** — memory-safe AI inference on microcontrollers
- **Robotics** — real-time effects + hot reload + safety
- **Safe embedded systems** — memory-safe firmware with tiny binaries
- **Security-critical software** — capabilities + sandbox + audit

The self-hosting effort has achieved self-compilation (session 4), native PE
code generation (session 5), and Linux native codegen porting (session 8).
Once the bootstrap fixpoint is verified and the PE emitter supports the full
compiler source, Fides becomes self-reproducing with a fully custom toolchain
— a major milestone that most languages never achieve (Rust still uses a C
bootstrap, Zig still uses LLVM).

The honest assessment: **Fides is where Rust was in 2012** — the architecture
is sound, the unique features work, but the ecosystem and optimization quality
need years of work. The difference is that Fides has a clearer niche strategy
and doesn't have to fight LLVM's complexity.


## Vision

*From `selfhost.md` § Vision.*

> An honest assessment of where Fides is, where it's going, and what the
> self-hosting effort needs to succeed. Updated Sep 6, 2026 (session 16 — Tier 3 OS features added, bootstrap fixpoint preserved).

---

### The Vision (from `docs/reference/vision.md`)

> Build the last programming language the world needs.

Eight pillars:
1. **Originality** — custom toolchain, no LLVM
2. **Security** — capabilities, no ambient authority
3. **Performance** — 10x compilation, 2-3x runtime
4. **Memory Safety** — regions, no borrow checker fights
5. **Power** — comptime, raw pointers, FFI
6. **Adoption** — C interop, hot reload, fast builds
7. **Custom** — everything built by us
8. **Innovation** — AI/ML, robotics, IoT

The North Star: one language for OS kernels, web servers, robots, AI, microcontrollers — all memory-safe, all fast, all without fighting the compiler.

---

### Where We Are Now

#### What works (Rust compiler — `src/`)

| Area | Status | Evidence |
|------|--------|----------|
| Language features | Phases 1-8 complete | 209/211 on interp, 198/211 on C, 209/211 on PE |
| Cross-compilation | Done | x86-64, ARM64, RISC-V, Windows |
| Optimizer | 23 passes, mostly complete | SLP, loop-vec, LTO, inline, DCE, etc. |
| AI/ML | Complete | Tensor ops, autograd, KV cache, quantization, DP |
| Safety | Complete | Regions, effects, capabilities, borrow checker, @medical |
| Industry stdlibs | Partial | banking.fd, medical.fd, clinical.fd, ecu.fd, can.fd, arinc.fd |
| Tooling | In progress | LSP, formatter, linter, bench, profile, debugger, ark, scribe, veil |

#### What works (self-hosted compiler — `compiler/`)

| Area | Status | Evidence |
|------|--------|----------|
| All 25 modules | Compile + pass tests | 23/23 on interp + PE |
| Build pipeline | Works | `fides-c.exe` builds via tcc |
| SRET | Works | Struct returns via out-parameter |
| Enum layout | Works | Pointer-based payload approach (D1) |
| Self-compilation | **✅ WORKS** | `fides-c.exe` → `fides-c2.exe` (session 4) |
| PE emitter | **✅ WORKS** | `fides-c.exe build` → native .exe without tcc (session 5) |
| Comptime eval | **✅ FULL PORT** | process_gen, substitute, I-5A, I-5D, reflection (session 7) |
| PE runtime stubs | **✅ 25+ stubs** | print, string, vec, map, float, bool (session 7) |
| Linux native codegen | **✅ PORTED** | peephole.fd + regalloc.fd + codegen_x64.fd (session 8, not yet integrated) |
| `.exe` extension | **✅ FIXED** | Auto-appended in ccompile + build commands (session 8) |
| Category B parity | **✅ RESOLVED** | B5 fixed, B6/B7/B8 at parity (session 14) |
| Float bit-patterns | **✅ PRESERVED** | p_f64_to_bits/p_f32_to_bits + union literals (session 14) |
| Bootstrap fixpoint | **✅ ACHIEVED** | fides-c2.c == fides-c3.c (SHA256-identical, 701,100 lines, session 15) |

#### What doesn't work yet

1. **Rust vs self-hosted parity gap** — c1=706K lines vs c2=701K lines (~5K line difference). Does NOT block the self-hosted fixpoint but means the Rust and self-hosted compilers produce slightly different C.
2. **Debug prints** — 287 `p_eprint_str` calls go to stderr (don't affect C output or fixpoint). Gating behind a flag is a cleanup task.
3. **PE emitter limited to simple programs** — needs struct/enum, float arithmetic, many-arg calls for full compiler source
4. **Linux native codegen not integrated** — peephole.fd, regalloc.fd, codegen_x64.fd are ported but not wired into the pipeline (requires elf64.rs, linker.rs, runtime_linux.rs porting)
5. **`var` feature not mirrored** — Rust compiler supports module-level `var`/`pub var` but self-hosted compiler files don't yet parse/typecheck/lower/emit globals

---

### Are We on the Right Path?

#### Yes — the architecture is sound

The vision calls for a custom toolchain with no LLVM dependency. We have that:
- Custom lexer, parser, typechecker, IR, optimizer, codegen, linker
- Cross-compilation to 4 targets with zero external tools
- 84K LOC Rust → 57K LOC Fides (the self-host is more compact)

The self-hosting approach is correct:
- Rewrite leaf modules first, climb the dependency graph
- Test at every step (23/23 self-host tests pass)
- Bootstrap via C codegen (simplest path to a working binary)

#### Yes — the bootstrap fixpoint is achieved

The self-hosted compiler compiles itself (session 4) and reproduces
byte-identical output across generations (session 15):
1. ✅ Fixpoint verified: fides-c2.c == fides-c3.c (SHA256-identical, 701,100 lines)
2. ✅ Self-reproducing compiler — Rust is only needed for the initial bootstrap
3. ✅ Three generations verified (Rust → c1 → c2 → c3, all work)

#### The direction change is correct

The user's change to `pattern_binding_ir_types` (using `word_repr` for all types) moves toward a **pointer-based enum payload** approach. This is simpler and more consistent than the inline + memcpy approach:

| Approach | Complexity | Consistency | Risk |
|----------|------------|-------------|------|
| Inline + memcpy (old) | High — needs `load_enum_field`, `memcpy` in codegen, multi-slot copy in `EnumLit` | Must keep 3 things in sync | High — easy to get wrong |
| Pointer-based (new) | Low — every slot is 8 bytes, `Load` reads pointer | Only 1 thing to get right | Low — matches how C already works |

**The pointer-based approach is the right direction.** It aligns with:
- The C backend's treatment of `IrType::Struct(_)` as a pointer type
- The `word_repr` function's existing behavior
- The principle of simplicity (one way to do things, from `docs/reference/vision.md` § Design Principles)

---

### What Needs to Happen

#### Immediate (post-fixpoint cleanup)

1. ~~**Fix `.exe` extension** in self-hosted compiler output (Windows)~~ — DONE (session 8)
2. ~~**Category B codegen parity gaps**~~ — DONE (session 14: B5 fixed, B6/B7/B8 at parity)
3. ~~**Fix Category A crash blockers**~~ — DONE (session 15: stack wrapper + type_eq fix + large match split + non-ASCII removal)
4. ~~**Run the fixpoint test**~~ — DONE (session 15: fides-c2.c == fides-c3.c, 0 diffs / 701,100 lines)
5. **Mirror `var` feature to `compiler/*.fd`** — needed for inc.fd global cache
6. **Gate debug prints** — 287 `p_eprint_str` calls (cleanup, doesn't affect fixpoint)
7. **Close Rust vs self-hosted parity gap** — c1=706K vs c2=701K lines

#### Near-term (after bootstrap)

5. **Run the full test suite** through the self-hosted compiler
6. **Verify the fixpoint** (stage2.c == stage3.c)
7. **Fix any remaining codegen differences** found during fixpoint testing
8. **Extend PE emitter to full compiler source** — add struct/enum, Vec/HashMap, float arithmetic support

#### Long-term (post-bootstrap)

9. **Port codegen_x64.fd + peephole.fd + elf64.fd** for Linux native codegen
10. **Industry domain stdlibs** (Phase 9)
11. **Playground website** (post-self-host)
12. **Package registry** (post-self-host)

---

### What the Language Can Do Now

#### Systems programming
- Raw pointers (`*mut T`, `*const T`) with `unsafe {}` enforcement
- Inline assembly
- Bare-metal targets (flat binary mode)
- Cross-compilation to ARM64, RISC-V, Windows
- Region-based memory (stack, heap, named regions)
- Arena allocators

#### Safety
- Region inference (no lifetime annotations)
- Effect system (IO, Net, Alloc, etc. — all explicit)
- Capability security (no ambient authority)
- Borrow checker (field-wise, two-phase, NLL)
- Bounds checking (always on, skippable in `unsafe`)
- `@medical` (IEC 62304 compliance), `@crashfree`, `@constant_time`, `@tainted`
- Linear types (`@linear`), conservation proofs (`@conserve`)

#### Performance
- 23 optimization passes (inline, CSE, GVN, DCE, SROA, DSE, loop-vec, SLP, etc.)
- LTO (cross-module inlining)
- Comptime evaluation (auto-eval, loop unrolling, dead branch elimination)
- SIMD stdlib
- GPU codegen (SPIR-V)

#### AI/ML
- Tensor operations with shape checking
- Autograd (reverse-mode AD, gradient checkpointing)
- Quantization, KV cache, differential privacy
- Multi-rank sharding (simulated)

#### Concurrency
- Async/await (stackless coroutines, state machine transform)
- Channels (pthread mutex + condvar)
- Select, spawn
- Effect handlers

#### Ergonomics
- F-strings, `?.`, `??`, `is`, spread, guard
- Contracts (`given`/`ensure`)
- `@memo`, `@gas`, `@audit`, `@wrapping`, `@deterministic`
- Pattern matching (enum variants, ranges, guards)
- Closures with capture analysis
- Traits and impls

#### Tooling
- LSP (completion, hover, definition)
- Formatter, linter
- Debugger (interactive, record/replay, post-mortem, native)
- Package manager (ark)
- Doc generator (scribe)
- Coverage tool (veil)

---

### What's Missing for the Vision

| Vision pillar | What's missing | Priority |
|---------------|----------------|----------|
| Self-hosting | Fixpoint verification + PE emitter for full compiler | **CRITICAL** — self-compilation + PE emitter work, need fixpoint + full native codegen |
| Performance | AVX2/SSE2 codegen, graph-coloring regalloc | Medium |
| Platform | DWARF debug info, Linux native codegen (elf64.fd port) | Low |
| Innovation | Industry domain stdlibs (medical, blockchain, robotics) | Low (post-bootstrap) |
| Adoption | Playground website, package registry | Low (post-bootstrap) |
| Ergonomics | Hot reload, green threads | Medium |

---

### Lessons for the Self-Host Effort

1. **Keep `src/` and `compiler/` in sync.** Every change to the Rust lowerer must be mirrored in the Fides lowerer. The enum payload direction change is a perfect example — if one side changes and the other doesn't, things break silently.

2. **The C backend treats structs as pointers.** This is a fundamental design choice that affects everything: SRET, enum payloads, struct copies, field access. Don't fight it — work with it.

3. **Simpler is better.** The inline + memcpy approach for enum payloads was complex and required 3 things to stay in sync. The pointer-based approach requires 1 thing. Always choose the simpler approach.

4. **Test the fixpoint early and often.** Once the self-hosted compiler can compile anything, run the fixpoint. Divergences catch bugs that unit tests miss.

5. **Document decisions immediately.** When you change direction, record it in `selfhost.md` § Coordination before doing anything else. Otherwise parallel agents will follow the old direction.

---

### Conclusion

**Yes, we are on the right path.** The vision is ambitious but the architecture
is sound. The self-hosting effort has achieved self-compilation (session 4),
native PE code generation (session 5), and Linux native codegen porting
(session 8) — `fides-c.exe` compiles the entire ~45K LOC compiler to a working
second-generation binary, the PE emitter generates native Windows executables
without tcc, and the Linux native codegen backend is ported (pending
integration). The remaining work is verifying the fixpoint (stage2.c ==
stage3.c), which requires fixing the statement-dropping bug (root cause of the
fides-c2.exe stdlib crash), and extending the PE emitter to support the full
compiler source.

Once the bootstrap fixpoint is achieved and the PE emitter supports the full
compiler source, Fides becomes a **self-reproducing compiler with a fully
custom toolchain** — no LLVM, no gcc, no tcc, no external linker. The
foundation for everything else in the vision.


## Porting Guide: Rust → Fides

*From `selfhost.md` (the practical porting guide — bootstrap strategy, language differences, adaptation checklist).*

> **What this is:** A practical reference for anyone working on the self-hosting
> effort. Explains the process, how Fides differs from Rust/C, what limitations
> to expect, and how to avoid common pitfalls during bootstrap.
>
> **Status (Oct 25, 2026 - post-session 30):** Bootstrap fixpoint verified
> byte-for-byte and re-verified (`fides-c4.c == fides-c5.c`, ~29.7 MB).
> 54 compiler modules, 104,439 LOC — all type-check. Since session 20:
> `escape.fd` (reference escape analysis) and `runtime.fd` (Fides-authored
> runtime scaffold) were added. (The session-30 stdlib-parse crash of the
> generated binary is fixed; see the status note at the top of this file.)
> This guide's process description remains accurate; specifics superseded by
> `docs/selfhost/selfhost.md` (consolidated 2026-10-24).

### Table of Contents

1. [What Self-Hosting Means](#1-what-self-hosting-means)
2. [The Strategy: C Codegen Bootstrap](#2-the-strategy-c-codegen-bootstrap)
3. [How Fides Differs from Rust](#3-how-fides-differs-from-rust)
4. [How Fides Differs from C](#4-how-fides-differs-from-c)
5. [Limitations and Encountered Issues](#5-limitations-and-encountered-issues)
6. [What to Keep in Mind During Bootstrap](#6-what-to-keep-in-mind-during-bootstrap)
7. [Recommended Process](#7-recommended-process)
8. [Failure Modes and Debugging](#8-failure-modes-and-debugging)
9. [Files: Rewritten and Remaining](#9-files-rewritten-and-remaining)

---

### 1. What Self-Hosting Means

Self-hosting means the Fides compiler is written in Fides. The bootstrap
process proves this works:

```
Step 1: Rust compiler (fidesc) compiles Fides compiler -> fides-c
Step 2: fides-c compiles the Fides compiler -> fides-c2
Step 3: fides-c2 compiles the Fides compiler -> fides-c3
Step 4: diff fides-c2.c fides-c3.c  (must be identical -- deterministic)
Step 5: fides-c3 passes the full test suite
```

If step 4 produces identical output, the compiler is **round-trip stable** --
it can compile itself forever without drift. **This was achieved in session 15:
fides-c2.c == fides-c3.c (SHA256-identical, 701,100 lines).**

### 2. The Strategy: C Codegen Bootstrap

The Fides compiler has three backends:
- **Interpreter** -- runs IR directly (slow, reference implementation)
- **C codegen** -- translates IR -> C source, hands off to gcc/clang
- **Native (PE/ELF)** -- translates IR -> raw x86-64 machine code (no external tools)

We bootstrap via **C codegen** because:
- It's the most stable (167/167 tests pass)
- gcc handles optimization (we don't need to match gcc -O2)
- The C codegen path is simpler than native (no register allocation, no ELF headers)
- It's portable (works anywhere gcc exists)

The native backends (codegen_x64, regalloc, peephole, elf64, linker) are
**not needed** for the bootstrap. They can be rewritten later to eliminate
the gcc dependency entirely.

#### The Compilation Pipeline (C Codegen Path)

```
source (.fd)
  -> lexer        (tokenize)
  -> parser       (build AST)
  -> typechecker  (type inference, region/effect/capability checks)
  -> lower        (AST -> SSA IR)
  -> mono         (monomorphize generics)
  -> borrowck     (borrow checking)
  -> pir          (IR -> PIR L1/L2/L3 transforms)
  -> opt          (23 optimization passes on PIR)
  -> codegen_c    (PIR -> C source code)
  -> gcc/clang    (C -> native binary)
```

All 25 files in this pipeline have been rewritten in Fides (see section 9).
The bootstrap fixpoint is achieved: `fides-c2.c == fides-c3.c`.

### 3. How Fides Differs from Rust

These differences shaped every rewrite. Understanding them is essential.

#### 3.1 Structs Passed by Value

**Rust:** `fn process(&module: &IrModule)` -- passes a pointer, O(1).
**Fides:** `fn process(self: &mut CCodeGen, module: IrModule)` -- copies the entire struct.

Every function that takes a large struct (IrModule, IrFunction, AstModule)
copies all its Vecs, strings, and nested data. This is the biggest performance
difference.

**Impact:** The self-hosted compiler will be significantly slower than the
Rust compiler. For bootstrap, this is acceptable -- correctness matters more
than speed. The workaround is "get-modify-set": read the struct, modify a
local copy, write it back.

**Future fix:** Add reference types (`&T`) to Fides for large struct passing.

#### 3.2 No Real HashMap -- Vec-Based Lookup

**Rust:** `HashMap<String, V>` -- O(1) lookup via hash table.
**Fides:** `HashMap` is i64->i64 only. String-keyed maps use `Vec<Entry>` with
linear search -- O(n).

Every type lookup, function signature lookup, and variable resolution in the
typechecker is a linear scan. For a compiler processing thousands of items,
this makes the typechecker quadratically slow on large programs.

**Impact:** The self-hosted compiler compiling its own 4800-line typechecker
will be slow. Acceptable for bootstrap.

**Future fix:** Add generic HashMap to Fides stdlib (string-keyed, hash-based).

#### 3.3 No Lazy Iterators -- Eager Intermediate Vecs

**Rust:** `vec.iter().map(|x| ...).filter(|x| ...).collect()` -- lazy, zero
intermediate allocations.
**Fides:** Explicit while loops, each step produces a new Vec.

Every transformation creates intermediate Vecs. More allocations, more memory.

#### 3.4 No `continue` in Loops

**Rust:** `for x in items { if cond { continue; } ... }`
**Fides:** No `continue` keyword. Must use `if/else` or flag-based skip patterns.

**Workaround:** Wrap the rest of the loop body in `if !skip { ... }`.

#### 3.5 No `if/else` Inside Match Arms

**Rust:**
```rust
match x {
    Foo(a, b) => if a > 0 { ... } else { ... },
    _ => {},
}
```
**Fides:** Match arms must be simple. No `if/else` directly inside an arm.

**Workaround:** Extract to helper functions, or use separate match statements.

#### 3.6 No Variable Assignment from Match Arms

**Rust:**
```rust
let x = match y {
    Foo(a) => a,
    Bar(b) => b,
    _ => default,
};
```
**Fides:** Can't assign a variable from a match arm directly in all contexts.

**Workaround:** Declare `var x: T` before the match, assign inside each arm.
Or use helper functions that return values.

#### 3.7 No Nested Enum Pattern Matching

**Rust:**
```rust
match x {
    Opt::Some(Inst::Call(name, args, ret)) => { ... }
    _ => {}
}
```
**Fides:** Can't nest enum patterns in a single match arm.

**Workaround:** Flatten into two matches:
```fides
match x {
    Opt::Some(inst) => {
        match inst {
            Inst::Call(name, args, ret) => { ... }
            _ => {}
        }
    }
    Opt::None => {}
}
```

#### 3.8 `val` is a Keyword

**Rust:** `let val = 42;` -- fine.
**Fides:** `val` is a keyword (used for value declarations). Must rename to
`v`, `ret_val`, `value`, etc.

#### 3.9 `region` is a Keyword

**Rust:** `let region = ...` -- fine.
**Fides:** `region` is a keyword (region-based memory management). Must rename
to `mem_region` or similar.

#### 3.10 `>>` Parsing Issues

**Rust:** `Vec<Vec<i64>>` -- fine.
**Fides:** `>>` can be misinterpreted. Must write `Vec<Vec<i64> >` with a
space. Also use `shr_i64()` helper instead of `>>` operator in expressions.

#### 3.11 No `derive(PartialEq)`

**Rust:** `#[derive(PartialEq)]` -- auto-generates equality comparison.
**Fides:** `@derive(Debug, Clone)` exists but no `PartialEq`. Must write
manual `equals()` functions.

#### 3.12 Clone is Shallow

**Rust:** `.clone()` does a deep copy.
**Fides:** Clone returns `self` (shallow). But since structs are passed by
value anyway, this is usually fine -- the copy semantics handle it.

#### 3.13 String Concatenation Allocates

**Rust:** `String` is a growable buffer. `format!` is efficient.
**Fides:** `str` is an i64 pointer to a runtime-managed string. `a + b`
allocates a new string every time. The codegen modules do massive string
concatenation (building C source code), which is slow.

#### 3.14 No `as` Casts for Enums

**Rust:** `x as u64` -- numeric cast.
**Fides:** Must use explicit cast functions or match patterns.

#### 3.15 `Option<T>` -> Custom Opt Enums

**Rust:** `Option<T>` is built-in and generic.
**Fides:** `Option<T>` exists but for each concrete type used in the compiler,
we define a custom enum:
```
OptStr = Some(str) | None
OptIrType = Some(IrType) | None
OptVecIrType = Some(Vec<IrType>) | None
OptInst = Some(Inst) | None
OptStructLayout = Some(StructLayout) | None
```

#### 3.16 No `mem::replace` / `mem::swap` / `mem::transmute`

**Rust:** Standard library memory operations.
**Fides:** Must use manual temp variables:
```fides
var tmp = a;
a = b;
b = tmp;
```

### 4. How Fides Differs from C

The C codegen translates Fides IR -> C99. Understanding the mapping helps
debug codegen issues.

#### 4.1 All Values Are 8 Bytes

Fides IR uses I64 for everything (integers, pointers, booleans, enums).
In C, these become `int64_t`. Pointers are cast to `void*` or `int64_t`
interchangeably. This works on x86-64 (64-bit pointers) but would break
on 32-bit.

#### 4.2 Closures via String Dispatch

Fides closures are structs where field 0 is a `char*` function name. The
C codegen generates an if/else chain comparing the name and calling the
matching C function. This is O(n) per call but works.

#### 4.3 Trait Objects via Type Name Dispatch

Dyn values are `{ const char* type_name; void* ptr; }`. Method calls
compare `type_name` and dispatch to `Type_method` C functions.

#### 4.4 Runtime Library

The C codegen emits a ~3000-line C runtime implementing:
- Vec operations (p_vec_new, p_vec_push, p_vec_get, ...)
- Map operations (p_map_new, p_map_insert, p_map_get, ...)
- String operations (p_string_*, p_to_str, p_print_*)
- Threads + mutex (pthread-based)
- Channels (bounded MPSC with mutex + condvar)
- Sockets (native HTTP server)
- SIMD builtins
- Tensor + autograd runtime
- KV cache, quantization
- JSON parser
- HTTP client
- DB (memory-backed)
- ECS (entity-component-system)

#### 4.5 Binary Hardening

Generated C includes:
- Stack canaries (`-fstack-protector-strong`)
- PIE/ASLR (`-fPIE -pie`)
- RELRO (`-Wl,-z,relro,-z,now`)
- NX stack (default)
- Stack traces on panic (p_call_stack)

### 5. Limitations and Encountered Issues

#### 5.1 Performance

The self-hosted compiler will be **10-100x slower** than the Rust compiler
due to:
- Struct copying (no references)
- O(n) map lookups (no hash table)
- Eager intermediate Vecs (no lazy iterators)
- String allocation per concatenation

**Acceptable for bootstrap.** Optimize later.

#### 5.2 Recursion Depth

The typechecker and parser are deeply recursive. Fides has recursion limits
(the parser has an iterative fallback at depth 512). When the self-hosted
compiler processes its own 4800-line typechecker, it may hit limits.

**Mitigation:** Increase `--max-recursion` flag. The parser already has
an iterative fallback.

#### 5.3 Memory Usage

Struct-by-value passing + intermediate Vecs = high memory usage. The
self-hosted compiler compiling itself may use 2-4x more memory than the
Rust compiler.

**Mitigation:** Ensure sufficient RAM (4GB+). Accept for bootstrap.

#### 5.4 Determinism

Bootstrap step 4 requires identical binaries across rounds. Sources of
non-determinism:
- HashMap iteration order (mitigated: Fides HashMap is deterministic)
- Pointer addresses in output (should not appear in C codegen)
- Allocation order (Vec iteration is insertion-order, which is deterministic)

**Risk:** Low. But must verify with the actual bootstrap.

#### 5.5 Visibility and Module Resolution

Fides has module-private visibility. Multiple modules can define functions
with the same name (e.g., `str_starts_with` exists in both `core` and `opt`).
The typechecker must allow each module to access its own private definitions
even when another module registered the same name first.

**Current approach:** Track which functions each module defines. When
checking visibility, if the current module has its own definition, allow
access.

#### 5.6 FFI Handle Types

Fides FFI uses `i64` for all handle types (FILE*, DIR*, etc.). On x86-64
this is safe (pointers are 8 bytes). gcc 14+ warns about int-to-pointer
casts. The `-Wno-int-conversion` flag suppresses this.

#### 5.7 Missing Language Features (Worked Around)

| Feature | Status | Workaround |
|---------|--------|------------|
| Reference passing (`&T`) | Not implemented | Copy structs by value |
| Generic HashMap | Not implemented | Vec-based maps with linear search |
| `continue` in loops | Not supported | if/else wrapping |
| if/else in match arms | Not supported | Helper functions |
| Nested enum patterns | Not supported | Flattened matches |
| `derive(PartialEq)` | Not implemented | Manual equals() functions |
| Lazy iterators | Not implemented | Eager while loops + intermediate Vecs |

### 6. What to Keep in Mind During Bootstrap

#### 6.1 The Rust Compiler is the Oracle

Never break `src/*.rs` until bootstrap is fully working. The Rust compiler
is the reference implementation. Compare Fides output against it.

#### 6.2 Differential Testing Before Bootstrap

Before attempting bootstrap, verify that each Fides module produces identical
output to its Rust counterpart:
```bash
fidesc diff-test tests/bootstrap/hello.fd --stage c     # C output
fidesc diff-test tests/bootstrap/hello.fd --stage ir    # IR output
fidesc diff-test tests/bootstrap/hello.fd --stage pir3  # PIR output
```
Run across all test files. Fix differences before bootstrap.

#### 6.3 Start Small

Don't jump to compiling the full compiler. Build up:
1. Compile `hello.fd` with the Fides compiler
2. Compile `token.fd` (small, no deps)
3. Compile `json.fd` (small, no deps)
4. Compile `ast.fd` (medium, has deps)
5. Compile `ir.fd` (large)
6. Compile `typechecker.fd` (huge -- 4800 lines)
7. Compile `codegen_c.fd` (huge -- 4700 lines)
8. Compile `compiler.fd` (the driver)
9. Compile all together (full bootstrap)

#### 6.4 Each Step is Independently Cached

Write the bootstrap script so each step is cached. If step 6 fails, you
don't redo steps 1-5.

#### 6.5 The 36 Unrewritten Files Are Not Needed

The bootstrap only needs the 16 pipeline files. The 36 remaining files
(native backends, binary formats, tooling) are not on the C codegen path.

### 7. Recommended Process

#### Phase 1: Differential Testing (Before Bootstrap)

```bash
# Compare Fides compiler output to Rust compiler output
for test in tests/*.fd; do
    fidesc diff-test "$test" --stage c
    fidesc diff-test "$test" --stage ir
    fidesc diff-test "$test" --stage pir3
done
```

Fix all differences. This is the most important step.

#### Phase 2: Create Missing Tests

- `tests/selfhost/test_typechecker.fd` -- the only rewritten module without a test

#### Phase 3: Incremental Bootstrap

```
hello.fd -> token.fd -> json.fd -> ast.fd -> ir.fd -> 
typechecker.fd -> codegen_c.fd -> compiler.fd -> FULL BOOTSTRAP
```

#### Phase 4: Round-Trip Stability

```bash
fidesc ccompile compiler/compiler.fd -o fides-c
./fides-c compiler/compiler.fd -o fides-self
./fides-self compiler/compiler.fd -o fides-self-2
diff fides-self fides-self-2  # must be identical
```

#### Phase 5: Full Test Suite

```bash
./fides-self test
# Must pass: all tests on interpreter + C codegen + PE emitter
```

### 8. Failure Modes and Debugging

| Failure | Symptom | Where to Look |
|---------|---------|---------------|
| Visibility error | "private to module X" | typechecker -- module resolution |
| Missing function | "unknown function foo" | lower or mono -- function registration |
| Wrong type | "type mismatch" | typechecker -- type inference |
| Infinite loop | Hangs | parser or typechecker -- recursion |
| Crash/abort | Segfault | codegen_c -- pointer casting |
| Non-determinism | Step 3 differs from step 2 | Any module using Vec-based maps |
| Out of memory | OOM | Struct-by-value copying -- reduce struct size |
| Recursion limit | "max recursion depth" | parser/typechecker -- increase --max-recursion |
| C compilation error | gcc error | codegen_c -- invalid C output |
| Wrong output | Test fails | Trace: which stage produces wrong output? |

#### Debugging Strategy

1. **Bisect the layer:** `./scripts/bisect.sh <test_name>`
2. **Compare stages:** `fidesc diff-test <test> --stage ir` (or pir, c)
3. **Check if golden was wrong:** Compare to known-good output
4. **Make the smallest possible fix:** One change, one test, one verify

### 9. Files: Rewritten and Remaining

#### Rewritten (25 core pipeline files — the full compiler, session 15)

| File | LOC | Status | Test |
|------|-----|--------|------|
| `token.fd` | ~400 | Done | test_token.fd |
| `diag.fd` | ~300 | Done | (via other tests) |
| `json.fd` | ~800 | Done | test_json.fd |
| `fmt.fd` | ~500 | Done | test_fmt_*.fd |
| `ast.fd` | ~1200 | Done | (via parser/lower tests) |
| `lexer.fd` | ~600 | Done | (via parser tests) |
| `parser.fd` | ~5145 | Done | (via lower tests) |
| `ir.fd` | ~1600 | Done | test_ir.fd |
| `typechecker.fd` | ~5521 | Done (builds) | **no test file yet** |
| `lower.fd` | ~6420 | Done | test_lower.fd |
| `mono.fd` | ~2500 | Done | test_mono.fd |
| `borrowck.fd` | ~1800 | Done | test_borrowck.fd |
| `pir.fd` | ~2700 | Done | test_pir.fd |
| `opt.fd` | ~7695 | Done | test_opt.fd |
| `codegen_c.fd` | ~5202 | Done | test_codegen_c.fd |
| `compiler.fd` | ~565 | Done | test_compiler.fd |
| `comptime_eval.fd` | ~2000 | Done | (via compiler tests) |
| `flat_ast.fd` | ~1259 | Done | (via dump-ast-flat) |
| `lsp.fd` | ~1082 | Done | (manual LSP test) |
| `pe.fd` | ~1500 | Done | (via PE emitter tests) |
| `peephole.fd` | ~1064 | Done (not integrated) | -- |
| `regalloc.fd` | ~1181 | Done (not integrated) | -- |
| `codegen_x64.fd` | ~1942 | Done (not integrated) | -- |
| `main.fd` | ~800 | Done | (bootstrap) |
| `inc.fd` | ~146 | Done | (incremental cache) |

**Total: 25 files, 61,145 LOC. Bootstrap fixpoint achieved (session 15).**

#### Session 20 Additions (19 new files, 14,577 LOC; historical — current status per module in docs/file-analysis/)

**Ported from Rust reference (12 modules):**

| File | LOC | Source | Status |
|------|-----|--------|--------|
| `coff.fd` | 497 | `src/coff.rs` | Type check OK, C codegen OK |
| `macho.fd` | 483 | `src/macho.rs` | Type check OK, C codegen OK |
| `cparser.fd` | 523 | `src/cparser.rs` | Type check OK, C codegen OK |
| `embedded.fd` | 511 | `src/embedded.rs` | Type check OK, C codegen OK |
| `tensor.fd` | 755 | `src/tensor.rs` | Type check OK, C codegen OK |
| `wasm.fd` | 1655 | `src/wasm.rs` | Type check OK, C codegen segfault |
| `scribe.fd` | 999 | `src/scribe.rs` | Type check OK, C codegen OK |
| `veil.fd` | 527 | `src/veil.rs` | Type check OK, C codegen segfault |
| `hotreload.fd` | 635 | `src/hotreload.rs` | Type check OK, C codegen segfault |
| `debugger.fd` | 977 | `src/debugger.rs` | Type check OK, C codegen segfault |
| `native_debugger.fd` | 544 | `src/native_debugger.rs` | Type check OK, C codegen segfault |
| `gpu.fd` | 826 | `src/gpu.rs` | Type check OK, C codegen segfault |

**Cross-compilation backends (7 modules, user-created):**

| File | LOC | Status |
|------|-----|--------|
| `codegen_arm64.fd` | 557 | Type check OK, C codegen OK |
| `codegen_riscv.fd` | 554 | Type check OK, C codegen OK |
| `runtime_arm64.fd` | 794 | Type check OK, C codegen OK |
| `runtime_riscv.fd` | 825 | Type check OK, C codegen OK |
| `arm64_enc.fd` | 1664 | Type check OK, C codegen OK |
| `riscv_enc.fd` | 1210 | Type check OK, C codegen OK |
| `autograd.fd` | 41 | Type check OK |

**Total: 50 modules, 91,924 LOC (session 20).**

#### Previously "Not Rewritten" — Now Ported (Session 20)

The following files were listed as "not rewritten" in session 15.
They are now ported (session 20):

- ✅ `coff` — COFF object file format
- ✅ `macho` — Mach-O object file format
- ✅ `wasm` — WASM backend
- ✅ `gpu` — SPIR-V/PTX GPU codegen
- ✅ `codegen` — dispatch (was already present)
- ✅ `tensor` — tensor runtime library
- ✅ `interp` — interpreter (~4400 LOC, was already ported)
- ✅ `codegen_arm64`, `codegen_riscv` — cross-compilation backends
- ✅ `runtime_arm64`, `runtime_riscv` — cross-compilation runtimes
- ✅ `arm64_enc`, `riscv_enc` — assemblers

**Still not ported (not needed for C codegen bootstrap):**
- `x86` — x86 assembler (not needed; `codegen_x64.fd` handles x86-64)
- `autograd` — autograd tape (41-line stub)

#### Post-Fixpoint Priority

1. **Fix C codegen segfault on complex modules** — 6 modules (wasm, veil,
   hotreload, debugger, native_debugger, gpu) type-check but crash during
   C code generation. This is a self-hosted compiler bug, not a source error.
2. **Port remaining PE emitter stubs** — 9 `p_string_*` functions still
   return wrong values (see CB6 in docs/roadmap/roadmap.md § Issues & Debates)
3. **Wire interpreter debug hook** — `Interpreter_set_debug_hook` is a no-op;
   needed for veil coverage and debugger stepping
4. **Close Rust vs self-hosted parity gap** — c1=706K vs c2=701K lines (~5K difference)
5. **Gate debug prints** — 287 `p_eprint_str` calls (cleanup, doesn't affect fixpoint)
6. **PE emitter full compiler support** — struct/enum, floats, many-arg calls

---

### Quick Reference: Fides -> Rust Adaptation Checklist

When rewriting a Rust file to Fides, check:

- [ ] `HashMap<K,V>` -> `Vec<Entry>` with linear search helper
- [ ] `HashSet<T>` -> `Vec<T>` with `vec_contains_*` helper
- [ ] `&T` references -> pass by value (copy the struct)
- [ ] `&mut T` -> `self: &mut Struct` or get-modify-set pattern
- [ ] `Option<T>` -> custom `Opt*` enum per type
- [ ] `Box<T>` -> `box_*_unbox()` helper for recursive types
- [ ] `format!` / `println!` -> string concatenation + `int_to_str`
- [ ] `.iter().map().filter().collect()` -> explicit while loops
- [ ] `continue` -> `if/else` wrapping
- [ ] `if/else` in match arms -> helper functions
- [ ] Nested enum patterns -> flattened matches
- [ ] `val` variable -> rename to `v` or `value`
- [ ] `region` variable -> rename to `mem_region`
- [ ] `Vec<Vec<T>>` -> `Vec<Vec<T> >` (space between brackets)
- [ ] `>>` operator -> `shr_i64()` helper
- [ ] `derive(PartialEq)` -> manual `equals()` function
- [ ] `mem::replace` / `mem::swap` -> manual temp variables
- [ ] `usize` / `u32` / `u64` -> `i64`
- [ ] `char` -> `i64` (codepoint)
- [ ] `String::from_utf8` / `as_bytes` -> manual byte->str conversion
