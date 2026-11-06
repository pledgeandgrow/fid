# Fides Language — Feature Status

*Last updated: Nov 6, 2026. Most tables below were written against the Rust
compiler `fidesc` (`src/*.rs`), which is now frozen; the language is what the
self-hosted `fid` (`compiler/*.fd`) accepts. Where a row disagrees with
[reference.md](reference.md) (written Nov 4 from the compiler source),
reference.md is right. Counts: [CURRENT.md → Numbers](../../CURRENT.md#numbers).*
*See `docs/reference/syntax.md` for the language syntax and
[safety-rules.md](safety-rules.md) for the safety annotations.*

## Current State (Nov 6, 2026)

- Compiler: `compiler/*.fd`, 61 modules, 133,688 lines; `src/*.rs` (frozen
  bootstrap reference and test harness), 57 files, 112,511 lines; stdlib 137
  modules, 43,576 lines (+ 4 in `stdlib/pe_runtime/`).
- Tests: 493 `.fd` under `tests/bootstrap/` (452 top level, 91 of them
  expected-fail `*_fail.fd`), 51 in `tests/selfhost/`, 10 examples.
- Fixpoint: stage 2 C == stage 3 C, 49 MB (Nov 6). Self-compile ~133 s.
- Last full suite run: Nov 2 (Rust harness, `fidesc` as compiler): interp
  339/339, C 343/343, PE 345/345, expected-fail 89/89. Not rerun with the
  Nov 4–6 compiler; the features below were checked with `fid run`,
  `fid ccompile` and `fid emit` on their tests.
- Backends: interpreter (`fid run`), C (`fid ccompile`, needs zig cc, clang,
  gcc or cl), native Windows x64 (`fid emit`, no C compiler). Linux x64 goes
  through C. ARM64, RISC-V, WASM, macOS, native ELF and bare metal are
  experimental / incomplete.

### New in `fid` since Nov 3

| Feature | Status | Notes |
|---------|--------|-------|
| Generics (proposal 0002) | ✅ | Inference or an error, bounds `T: A + B` checked, turbofish `f::<T>()` with arity check (`test_generics_full`, `test_generic_infer_fail`, `test_generic_bound_fail`, `test_turbofish_arity_fail`) |
| `if`/`match` values | ✅ | The value of the branch taken, also as a function's last statement (`test_if_expr_value`) |
| `@cfg` | ✅ | `platform`/`os = "…"`, `linux`/`windows`/`macos`, arch, debug/release filter functions and structs; the host OS decides; `release` always on, `debug` off (`test_cfg`, all 3 backends) |
| Freeing memory | ✅ | Vec/HashMap locals, a `Vec<str>`/`Vec<Vec>` that owns its elements, collections declared in a `region` at its end, string temporaries (`test_mem_owned_drop`). No GC; everything else lives until exit (leaks, never dangles) |
| Borrow checks K4/K5 | ✅ | Changing a Vec a `for` loop walks; `s.x = 1` while `s` is borrowed; `&v[i]`; trait impl bodies |
| Effects of runtime calls (K7) | ✅ | file/socket/process/clock runtime calls are `IO`/`Net` |
| Higher-order effects | ✅ | A function taking a function value gets the effects of the function passed at each call site (`effects/test_effect_ho_instantiation`) |
| `@send` | ✅ | Every argument of a spawned call must be `@send`; a struct of `@send` fields is `@send`; raw pointers and references are not (`test_race_ptr_fail`) |
| `parallel { \| a \| b }` | ✅ | `x = f(..)` branches and multi-statement branches that only read word-sized locals run on threads (`test_parallel_branches`, all 3 backends) |
| `async fn` with f64/struct | ⚠️ | Runs as a task in the interpreter and C; native async crashes |
| Per-thread allocation | ⚠️ | C runtime: per-thread arenas and free lists; native runtime: per-thread free lists. Written Nov 4, exercised by every native test; no multi-thread stress test run |
| Safety annotations | ✅ | `@no-alloc` sees `.push()`, `Vec::new()`, string `+`; `@no-indirect` rejects calls through function parameters; `@firmware` rejects OS runtime calls; `@gas` counts literal loop trip counts; `@tainted` on parameters; `@no-recursion`, `@bounded-stack`. 17 rules, each with a negative test |
| `fid qualify <file.fd> [-o out.json]` | ✅ | JSON evidence (compiler and source hashes, check result, annotated functions, each rule with its requirement and tests). Always `NOT_QUALIFIED`: evidence, not a certificate |
| Owned and borrowed strings, `@scoped` | ✅ | `str` owned, `&str` borrowed; `@scoped` struct `str` fields read as `&str`; `str_clone`; `http_serve_scoped` frees request memory after each response and is no longer `@unsafe` (`test_borrowed_str`, `test_borrowed_str_return_fail`, `test_borrowed_str_pass_fail`, `test_scoped_store_fail`) |
| HashMap integer keys | ✅ | Was a crash; `test_hashmap_int_keys` on all 3 backends. 0 and -1 are ordinary keys; `0x5DEECE66D1234567` and its complement are reserved and panic |
| Native IR optimizer | ✅ | `fid emit`: inlining of small single-block functions, constant folding, dead code removal (`compiler/iropt.fd`) |
| core.fd check stamp | ✅ | core.fd is type-checked once per (compiler, core.fd) pair; `FIDES_CHECK_CORE=1` forces it. `.fidcache/` evicts least recently used artifacts past 64 files or 1 GB |
| `fid emit --checked-alloc` | written, not run | Every block on pages of its own, ending at a guard page; freed pages inaccessible, never reused |
| `scripts/sanitize.sh`, `scripts/fuzz/nightly.sh` | written, not run | ASan/UBSan through C (needs clang with ASan); fuzzing with a `checked` backend; no CI workflow schedules it |
| Value semantics (proposal 0001) | ❌ | Copy-on-write written behind `LOWER_COW` (off), lazy and counted; with it on, value semantics hold on the tests and the compiler reaches its fixpoint (4.7 GB peak vs 1.8 GB); not on until the full suite runs with it. `var b = a` on a Vec still aliases |

## Legend

- ✅ **Working** — Fully functional and tested
- ⚠️ **Partially Working** — Functions but has known bugs or limitations
- ❌ **Not Working** — Implemented but broken, or not yet implemented

---

## Compiler Pipeline

| Feature | Status | Notes |
|---------|--------|-------|
| Lexer (token.rs) | ✅ | All Fides keywords (fn, val, const, return, etc.) |
| Parser (parser.rs) | ✅ | Produces AST from tokens |
| Type Checker (typechecker.rs) | ✅ | Validates types, capabilities, effects, regions, traits |
| IR Lowering (lower.rs) | ✅ | AST → typed SSA IR with effects/regions |
| IR Interpreter (interp.rs) | ✅ | Runs IR directly via `fidesc run` |
| PE Emitter (pe.rs) | ✅ | Generates native x64 PE executables. In `fid`, `fid emit` (`compiler/pe_ir*.fd`) is the native path; bare metal (`emit --bare`) panics |
| ELF64 Emitter (elf64.rs) | ⚠️ | Experimental / incomplete; Linux x64 goes through C |
| C Codegen (codegen_c.rs) | ✅ | Generates C code, 198/211 tests match interpreter output. Phase 10B: Print calls Fides Vec_to_str/HashMap_to_str for formatting. Binary hardening: stack canaries, PIE/ASLR, RELRO, NX stack. Stack traces on panic via p_call_stack. Rc/Arc runtime via p_rc_incr/p_rc_decr. Drop trait: compiler-inserted drop calls. Negative size checks on p_malloc/p_shared_alloc/p_allocator_alloc. ftell failure check on p_file_read/p_file_size. |
| PIR (pir.rs) | ✅ | PIR L1/L2/L3, `fidesc pir` command |
| Optimizer (opt.rs) | ✅ | 23 passes: const-fold, CSE, GVN, SROA, DSE, DCE, inline, unroll, TCO, alias-analysis, effect-reorder, partial-eval, vectorize, strength-reduce, devirt, jump-thread, loop-rotate, loop-vec, effect-opt, region-pool, reassociate, memoize. 22 in default pass list. Enabled by default in `build` (use `--no-opt` to disable). `--lto` for aggressive inlining. |
| Peephole (peephole.rs) | ✅ | 9 passes, always on in `build` command: redundant moves, zero idiom, nop arithmetic, branch threading, dead store, redundant test, store-to-load forwarding, LEA coalescing, cmp+setcc+jcc instruction selection |
| Register Allocator (regalloc.rs) | ✅ | Linear scan allocator with constant rematerialization (F-1A). Effect-aware allocation (F-1D): pure/@no-alloc calls only clobber RAX. |
| Mach-O Emitter (macho.rs) | ✅ | Generates Mach-O object files for macOS |
| COFF Emitter (coff.rs) | ✅ | Generates COFF object files for Windows |
| WASM Backend (wasm.rs) | ✅ | PIR → WASM binary, `fidesc wasm` command |
| GPU/SPIR-V Backend (gpu.rs) | ✅ | PIR → SPIR-V binary, `fidesc gpu` command |
| Comptime (comptime.rs) | ✅ | Compile-time evaluation, reflection, f-string interpolation |
| Multi-arch Codegen | ⚠️ | x86-64, ARM64, RISC-V assembly via `fidesc codegen --target`; ARM64/RISC-V experimental / incomplete |
| ARM64 Instruction Encoder (arm64_enc.rs) | ✅ | Converts ARM64 assembly text to 4-byte machine code. 17 unit tests. |
| ARM64 Linux Runtime (runtime_arm64.rs) | ✅ | ARM64 runtime using raw SVC syscalls. 9 unit tests. |
| RISC-V Instruction Encoder (riscv_enc.rs) | ✅ | Converts RV64 assembly text to 4-byte machine code. 14 unit tests. |
| RISC-V Linux Runtime (runtime_riscv.rs) | ✅ | RISC-V runtime using raw ECALL syscalls. 7 unit tests. |
| Cross-compilation (ARM64 Linux ELF) | ⚠️ | `build --target arm64-linux` → AArch64 ELF64 executable; experimental |
| Cross-compilation (RISC-V Linux ELF) | ⚠️ | `build --target riscv64-linux` → RISC-V ELF64 executable; experimental |
| Cross-compilation (Windows PE) | ✅ | `build --target x86-64-windows` → x86-64 PE executable |
| Target Triple Parsing | ✅ | `parse_target_triple()` normalizes arch-os format strings |
| @cpu_local variables | ❌ | In `fid` no pass reads it: it is valid only on globals, which cannot carry annotations (the Rust PE emitter had GS-relative addressing) |
| Inline asm clobbers | ✅ | `clobber("reg")` syntax, clobber save/restore in PE backend, 50+ x86 instructions encoded |
| No-stdlib mode | ✅ | `--bare` / `--no-std` / `--no-stdlib` skips stdlib/core.fd, zero hidden runtime deps |
| Cross-compilation dispatch | ✅ | `--target arm64`/`riscv64` validates and emits diagnostic, falls back to C codegen |
| UEFI boot services | — | `uefi.fd` moved to the fides-os project (Oct 24) |

## CLI Commands

| Command | Status | Notes |
|---------|--------|-------|
| `fidesc version` | ✅ | Shows version info |
| `fidesc tokens <file>` | ✅ | Dumps lexer tokens |
| `fidesc ast <file>` | ✅ | Dumps parsed AST |
| `fidesc ir <file>` | ✅ | Dumps lowered IR |
| `fidesc run <file>` | ✅ | Interprets and runs program |
| `fidesc emit <file> -o <exe>` | ✅ | Emits native PE executable (Windows) |
| `fidesc emit <file> --bare -o <bin>` | ✅ | Bare-metal flat binary (no stdlib, no PE headers) |
| `fidesc build <file> -o <out> [--target] [--opt]` | ✅ | Native build: PIR → codegen → peephole → ELF64/PE executable (zero external tools). Cross-compilation: `--target arm64-linux`, `--target riscv64-linux`, `--target x86-64-windows`. |
| `fidesc ccompile <file> -o <exe>` | ✅ | C codegen → gcc/clang → executable |
| `fidesc test` | ✅ | Runs the test programs across interpreter, PE emitter, and C codegen (`--compiler fid` to test the self-hosted compiler). Last full run Nov 2 |
| `fidesc test --fail` | ✅ | Runs only expected-fail tests (failing = correct) |
| `fidesc repl` | ✅ | Interactive REPL |
| `fidesc audit <file>` | ✅ | IR verification pass (also `verify`) |
| `fidesc sourcemap <file>` | ✅ | Source map output |
| `fidesc pir <file> [1\|2\|3\|all] [--opt]` | ✅ | PIR output (L1/L2/L3) with optional optimization |
| `fidesc opt <file> [passes]` | ✅ | Run PIR optimizer, print statistics |
| `fidesc codegen <file> --target <arch>` | ✅ | x86-64, ARM64, or RISC-V assembly output |
| `fidesc codegen <file> --elf\|--macho\|--coff <o>` | ✅ | Emit ELF64, Mach-O, or COFF object files |
| `fidesc wasm <file> -o <out.wasm>` | ✅ | WebAssembly backend |
| `fidesc gpu <file>` | ✅ | SPIR-V GPU backend |
| `fidesc parse-header <file.h>` | ✅ | Parse C header → generate Fides FFI bindings |
| `fidesc watch <file>` | ⚠️ | Rebuilds and reruns on save, reports changed functions; no in-process swap. `fid watch` runs `bench` |
| `fidesc link <files...>` | ✅ | Link multiple object files |
| `fidesc lsp` | ✅ | Language Server Protocol server (JSON-RPC over stdio, diagnostics, no external deps) |
| `fidesc lint <file>` | ✅ | Lint checks (unused functions, unused imports) |
| `fidesc bench <file> [-n <count>] [--ccompile]` | ✅ | Benchmark a file (interpreter or C native) |
| `fidesc fmt <file>` | ✅ | Format a Fides source file |
| `fidesc profile <file>` | ✅ | Per-function timing profiler (interpreter mode) |
| `fidesc debug <file>` | ✅ | Interactive debugger (breakpoints, step, locals) |
| `fidesc replay <trace>` | ✅ | Replay a recorded execution trace |
| `fidesc autopsy <file>` | ✅ | Native binary debug (ptrace-based, Linux only) |
| `fidesc dump-tokens <file>` | ✅ | Dump lexer token stream |
| `fidesc dump-ast <file>` | ✅ | Dump AST as S-expressions |
| `fidesc dump-ir <file>` | ✅ | Dump lowered IR as text |
| `fidesc dump-pir <file> [1\|2\|3]` | ✅ | Dump PIR L1/L2/L3 instructions |
| `fidesc dump-c <file>` | ✅ | Dump generated C source (before gcc) |
| `fidesc diff-test <file> --stage <s>` | ✅ | Differential testing: tokens, ast, ir, pir1/2/3, c, output |
| `fidesc run <file> --trace` | ✅ | Per-stage compiler timing (Frontend/Lower/Backend/Total) |
| `fidesc run <file> --max-recursion N` | ✅ | Set recursion depth limit (0 = unlimited, default 1000) |
| `fid qualify <file> [-o out.json]` | ✅ | Safety evidence as JSON; always `NOT_QUALIFIED` |
| `fid audit <file> [--stdlib] [--strict]` | ✅ | Unsafe code a program reaches |
| `fid run <file> --leaks` / `--checked-alloc` | ✅ | Leak report / use-after-free detection (C runtime debug allocator) |

## Language Features — Interpreter (`fidesc run`)

### Core Syntax

| Feature | Status | Notes |
|---------|--------|-------|
| `fn` function declarations | ✅ | Named functions with params and return types |
| `val` variable declarations | ✅ | Immutable bindings |
| `var` mutable variables | ✅ | Mutable bindings |
| `const` constants | ✅ | Compile-time constants |
| `return` return statement | ✅ | Return with optional value |
| `if` / `else` conditionals | ✅ | Branching |
| `while` loops | ✅ | Conditional loops |
| `for x in iter` loops | ✅ | Iterator-based loops (Vec, HashMap, range, nested Vec) |
| `break` / `continue` | ✅ | Loop control flow |
| Block expressions `{ }` | ✅ | Scoped blocks with values |
| Comments `//` and `/* */` | ✅ | Line and block comments |
| Semicolons `;` | ✅ | Optional statement terminators |

### Types

| Feature | Status | Notes |
|---------|--------|-------|
| `i64` / `i32` / `i8` integers | ✅ | Signed integers |
| `u64` / `u32` / `u8` unsigned | ✅ | Unsigned integers |
| `f64` / `f32` floats | ✅ | Floating point |
| `bool` | ✅ | Boolean type |
| `str` / `&str` | ✅ | Owned / borrowed strings (`str` has no methods; use `str_*` functions) |
| `Vec<T>` | ✅ | Dynamic arrays |
| `HashMap<K,V>` | ✅ | Hash maps, `str` or integer keys |
| `Option<T>` | ✅ | Optional values (Some/None) — declared as `enum Option<T>` in stdlib/core.fd |
| `Result<T,E>` | ✅ | Error handling (Ok/Err) — declared as `enum Result<T, E>` in stdlib/core.fd |
| `struct` | ✅ | Struct definitions and instances |
| `enum` (algebraic) | ✅ | Sum types with data |
| Generic enums `enum Foo<T>` | ✅ | Type parameters on enum declarations |
| Generic structs `struct Foo<T>` | ✅ | Type parameters on struct declarations |
| Generics `<T>` | ✅ | Generic functions and types; bounds checked, inference, turbofish (`fid`) |
| Type aliases `type` | ✅ | Type alias declarations |

### Expressions

| Feature | Status | Notes |
|---------|--------|-------|
| Arithmetic (`+`, `-`, `*`, `/`, `%`) | ✅ | Integer and float |
| Comparison (`<`, `>`, `==`, `!=`, `<=`, `>=`) | ✅ | All comparison operators |
| Logical (`&&`, `\|\|`, `!`) | ✅ | Boolean logic |
| Bitwise (`&`, `\|`, `^`, `<<`, `>>`) | ✅ | Bit operations |
| f-string interpolation `f"{expr}"` | ✅ | String formatting |
| Ranges `..` and `..=` | ✅ | Range expressions |
| Closures `fn(x) { }` / `fn(x) => expr` | ✅ | Anonymous functions |
| `match` expressions | ✅ | Int, string, enum, wildcard patterns; expression and statement forms (statement arms need blocks) |
| `if` / `match` as values | ✅ | Value of the branch taken, also as a function's last statement |
| `?` operator | ✅ | Error propagation |
| Casts `as` | ✅ | Type casts |

### Control Flow

| Feature | Status | Notes |
|---------|--------|-------|
| `if` / `else if` / `else` | ✅ | Multi-branch conditionals |
| `while` | ✅ | While loops |
| `for` over Vec | ✅ | Iterate vector elements |
| `for` over range | ✅ | Iterate numeric ranges |
| `loop` | ✅ | Infinite loops (a statement; `break value` discards the value) |
| `break` / `continue` | ✅ | Loop control |
| `match` | ✅ | Int, string, enum, wildcard patterns; expression and statement forms |

### Functions & Closures

| Feature | Status | Notes |
|---------|--------|-------|
| Named functions `fn name() { }` | ✅ | |
| Function parameters | ✅ | Up to 4 register args, more on stack |
| Return values | ✅ | Single return value |
| Closures | ✅ | Captures environment |
| Generic functions `fn name<T>()` | ✅ | Type parameters |
| Default arguments | ✅ | `fn add(a: i64, b: i64 = 10)`; trailing defaulted parameters may be left out |
| Named arguments | ✅ | `add(b: 2, a: 1)`; put in parameter order by lowering. A named argument's type is not checked |
| Variadic arguments | ❌ | `...name` is parsed and ignored |

### Data Structures

| Feature | Status | Notes |
|---------|--------|-------|
| `struct` definition | ✅ | Named fields |
| `struct` instantiation | ✅ | Field initialization |
| `impl` blocks | ✅ | Methods on types |
| Field access `obj.field` | ✅ | Read struct fields |
| Method calls `obj.method()` | ✅ | Call impl methods |
| `enum` with data | ✅ | Algebraic sum types |
| Enum variant construction | ✅ | `Option::Some(42)` |
| Enum pattern matching | ✅ | `match` on enums |

### Memory & Ownership

| Feature | Status | Notes |
|---------|--------|-------|
| `&` references | ✅ | Immutable borrows |
| `&mut` references | ✅ | Mutable borrows, auto-deref for method calls (Issue 45) |
| `defer` | ✅ | Go-style deferred cleanup |
| Drop trait | ✅ | A `drop` method is called at function exit, last declared first, for locals declared with an explicit struct type. Works on all 3 backends. |
| Region blocks `region r { }` | ✅ | A scope with an optional size budget; collections declared in it are freed at its end. No region use-after-free check |
| Reference escape detection | ✅ | References cannot be stored, returned from locals, outlive their block or be sent (`compiler/escape.fd`) |
| Automatic freeing | ✅ | Vec/HashMap locals, owned `Vec<str>`/`Vec<Vec>` elements, region collections, string temporaries (`test_mem_owned_drop`) |
| Use-after-free detection | ⚠️ | `fid run --checked-alloc` (debug allocator); `fid emit --checked-alloc` is written, not run |
| `val` immutability | ✅ | Type error on reassignment + field mutation (Issues 46-47). 247 stdlib vars migrated to `var`. |
| Non-exhaustive match | ✅ | An error for missing enum variants or bool arms, unless a catch-all arm exists. |

### Modules & Imports

| Feature | Status | Notes |
|---------|--------|-------|
| `use` imports | ✅ | Module imports |
| `pub` visibility | ✅ | Public/private visibility |
| `use ... as` aliases | ✅ | Import aliases |
| `use ... { }` selective imports | ⚠️ | Parsed; the list does not select: the whole module is imported |
| `use ... *` glob imports | ⚠️ | Parsed; same as a plain `use` |
| Re-exports | ✅ | |
| Multi-file modules | ✅ | |
| Standard library (stdlib/core.fd) | ✅ | core is auto-imported. 137 modules, 43,576 lines (Nov 6), among them: core, vec, math, string, fmt, bits, sort, hash, iter, stack, queue, buffer, collections, result, random, path, units, uuid, crypto, storage, clinical, medical, ecu, scada, swift, grid, ota, banking, kv_cache, quant, dp, shard, fs, io, os, sys, ecs, events, parallel, sync, channel, db, http, json, shape, simd, tensor, money, alloc, file, time, typed_strings, memo, privacy, env, args, process, regex, tilemap, sprite, flowfield, streaming, scene, physics, audio, input, arinc, can, task. (uefi and kernel moved to fides-os.) |

### Capabilities (Security)

| Feature | Status | Notes |
|---------|--------|-------|
| `cap` (capability/trait) declarations | ✅ | |
| Capability grants | ✅ | `use ... with { IO, FS, Net, Spawn, Alloc }` |
| Capability denial | ✅ | Compile-time error if not granted (strict mode: any import has `with`) |
| Transitive capabilities | ❌ | Not done: each call is checked by name; capabilities do not propagate through the call graph (effects do) |
| `sandbox { }` | ✅ | Every capability-needing call inside is an error, granted or not |
| Capability elision | ✅ | Zero runtime overhead — all checks compile-time |

### Error Handling

| Feature | Status | Notes |
|---------|--------|-------|
| `Result<T,E>` type | ✅ | |
| `?` operator | ✅ | Early return on Err |
| `match` on Result | ✅ | |
| `try` blocks | ✅ | `test_pe_try_block.fd` |

### Concurrency

| Feature | Status | Notes |
|---------|--------|-------|
| `async` / `await` | ⚠️ | An `async fn` runs as a green task on its own stack (f64/struct arguments and results through an adapter); prefix `await` gives the result. Interpreter and C work; native (`fid emit`) async crashes |
| `spawn` | ⚠️ | `spawn f(args)` runs `f` on an OS thread and gives a `Future<T>`; every argument must be `@send`. Native spawn+await crashes |
| `parallel { \| … }` | ✅ | Branches on threads (see "New in `fid`") |
| Mutex | ✅ | `sync_mutex_create/lock/unlock` via pthread mutex |
| RwLock | ✅ | `sync_rwlock_create/read_lock/read_unlock/write_lock/write_unlock` — atomic reader count, no TOCTOU race |
| AtomicU64 | ✅ | `atomic_u64_new/load/store/fetch_add/cas` wrappers |
| Channels | ✅ | Bounded, thread-safe message passing (`sync_channel_create/send/recv/try_recv/close`) |
| Effect handlers `handle { do } with` | ✅ | `do E.op(args)` runs the handler; `resume(cont, v)` runs the rest of the `handle` block (`test_effect_handler`, `test_effect_resume`, `test_effect_noresume`) |
| Effect annotations `!IO`, `!{}` | ✅ | Compile-time effect inference and checking |
| Effect propagation | ✅ | Fixpoint iteration over call graph |
| Effect-row polymorphism | ✅ | Higher-order functions (`fn` params) are instantiated per call site in both `fid` and `fidesc`: `apply(pure_fn)` is pure, `apply(io_fn)` gets `IO`; closures are scanned directly (`effects/test_effect_ho_instantiation`) |
| `fidesc diagram <file>` | ✅ | Mermaid sequence diagram of calls annotated with inferred effects (`src/seqdiag.rs`) |
| `!E` effect rows | ✅ | Declaring a non-concrete effect name marks a function effect-polymorphic; callers instantiate the row |
| Pure-call CSE | ✅ | `pass_cse` folds duplicate calls to any `is_pure` function (empty effect set), not just `@deterministic` |

### Safety (Phase 2)

| Feature | Status | Notes |
|---------|--------|-------|
| Bounds checking (Vec/array/string) | ✅ | Always on (F62); skippable in `unsafe {}` blocks |
| Integer overflow checking | ✅ | Always on (F64: overflow = panic); `@wrapping` annotation for wrap-around (F65) |
| Division by zero detection | ✅ | Runtime panic |
| `Option::unwrap` panics on None | ✅ | Calls `p_panic` |
| Effect system (annotations + inference) | ✅ | `!IO`, `!Net`, `!Spawn`, `!Alloc`, `!{}` for pure |
| Effect checking (under-declaration) | ✅ | Compile-time error if inferred > declared |
| Region use-after-free detection | ❌ | None: the type checker writes `freed_regions` and never reads it. A region is a scope |
| Reference escape detection | ✅ | `compiler/escape.fd`: a reference cannot outlive its block |
| Capability elision | ✅ | Zero runtime overhead, all checks compile-time |

### Advanced

| Feature | Status | Notes |
|---------|--------|-------|
| `comptime` blocks | ✅ | `comptime <expr>`, `comptime { block }`, reflection |
| `comptime` reflection | ✅ | type_name, is_struct, is_enum, field_names, field_types, enum_variants |
| `comptime` functions | ✅ | `comptime fn func()` — evaluated at compile time |
| Match guards | ✅ | `pattern if cond => body` |
| Match range patterns | ✅ | `1..=5 => body` |
| Trait associated types | ✅ | `type Item;` in trait, `type Item = T;` in impl |
| Trait default methods | ✅ | Default body in trait, inherited by impls |
| Trait objects (`dyn Trait`) | ✅ | Dynamic dispatch via vtable |
| Trait impl verification | ✅ | Check all required methods implemented |
| Type aliases (`type`) | ✅ | `type X = T` — parsed, typechecked, lowered |
| Inline assembly (`asm!`) | ✅ | Hardware-specific instructions |
| Bare-metal mode (`--bare`) | ✅ | No stdlib, flat binary output |
| SIMD types | ✅ | `stdlib/simd.fd` — elementwise ops, dot, sum, scale |
| Tensor type | ✅ | `stdlib/tensor.fd` — n-d arrays, matmul, relu, softmax |
| JSON parsing | ✅ | `stdlib/json.fd` — parse, stringify, get, at |
| HTTP server | ✅ | `stdlib/http.fd` — socket + HTTP parsing |
| Database framework | ✅ | `stdlib/db.fd` — cap Database trait, in-memory KV |
| Parallel operations | ✅ | `stdlib/parallel.fd` — par_map, par_sum, par_dot |
| Const generics | ✅ | `ArrayConst` type variant, monomorphization handles it — `test_const_generic.fd` passes in interpreter |
| GATs | ✅ | `test_gat.fd` passes in interpreter, outputs correct results |
| C FFI (`extern "C"`) | ✅ | Parsed, lowered, C codegen, interpreter stubs — `test_ffi.fd`, `test_ffi_full.fd`, `test_ffi_header.fd` pass |
| Hot reload | ⚠️ | `fidesc watch` rebuilds and reruns on save, reports function-level changes; no in-process swap |
| `#[test]` attribute | ❌ | Parsed and ignored. `fid test file.fd` runs functions named `test_*` and `bench_*` |
| `@derive` | ✅ | `@derive(Debug/Clone/Serialize/Deserialize)` generates methods (Phase 7.7) |
| Borrow checker | ✅ | Phases 1-6 complete: `--borrowck=off\|warn\|on`, field-wise borrows, two-phase borrows, NLL, on by default |
| Raw pointers `*mut T` / `*const T` | ✅ | With `unsafe {}` blocks for dereference |
| `var` (uninitialized mutable) | ✅ | F63: definite assignment analysis |
| `@wrapping` arithmetic | ✅ | F65: escape hatch from checked arithmetic |
| `@gas(N)` budget | ✅ | Compile-time gas budget annotation |
| `@crashfree` | ✅ | No panic, no unbounded loops |
| `@medical` | ✅ | No alloc, no recursion, no while, no panic, must be @audit. A set of checks, not IEC 62304 compliance |
| `@safety-critical` | ✅ | No alloc, no recursion, no unbounded, no panic, no indirect. A set of checks, not ISO 26262 compliance |
| `@autosar` | ✅ | No alloc, no recursion, no unbounded, no panic |
| `@linear` types | ✅ | Used exactly once; the only types that move |
| `@conserve` | ✅ | Money conservation: no IO/Net/Spawn/Diverge, no floats, only `@conserve` callees, never `Money::new`/`from_cents` |
| `@audit` | ✅ | Logs entry (with parameters) and exit at run time |
| `@deterministic` | ✅ | No IO/Net/Spawn, no floating-point |
| `given`/`ensure` contracts | ⚠️ | `given` is checked at calls with constant arguments; no run-time check. `ensure` is parsed and not checked |
| `txn {}` blocks | ⚠️ | `fid run` snapshots memory at entry; C and native run the body with no rollback |
| `sandbox {}` blocks | ✅ | Capability revocation |
| ECS (component/system/query) | ✅ | Phase 8B: F18 — entity-component-system |
| State machines | ✅ | Phase 8B: F19 — `state_machine` with states/transitions |
| Event handlers `on` | ✅ | Phase 8B: F20 — desugars to `__on_<event>` function |
| Typed strings | ✅ | F67: SQLString/ShellString/HTMLString/JSONString/PathString |
| `@cfg` conditional compilation | ✅ | `@cfg(platform = "linux")`, `@cfg(windows)`, arch, `debug`/`release`; host OS decides (`test_cfg`, all 3 backends) |
| Dynamic loading | ✅ | `p_dl_open`/`p_dl_sym`/`p_dl_close`/`p_dl_call` — real dlopen in C codegen |
| Vec stdlib (Phase 10B) | ✅ | `Vec` implemented in `stdlib/core.fd` using raw pointers |
| HashMap stdlib (Phase 10B) | ✅ | `HashMap` implemented in `stdlib/core.fd` using raw pointers |
| String stdlib (Phase 10B) | ✅ | `str_*` functions and `StringBuilder` in `stdlib/core.fd` |
| `Rc<T>` smart pointer | ✅ | Reference-counted pointer with manual refcount (new, clone, drop, get, count) |
| `Arc<T>` smart pointer | ✅ | Atomic reference-counted pointer with manual refcount (new, clone, drop, get, count) |
| `Box<T>` recursive types | ✅ | Heap indirection enabling recursive types (e.g. `enum Expr { Add(Box<Expr>, Box<Expr>) }`) |
| Vec/Mat math types | ✅ | vec2/vec3/vec4, mat4, quat in `stdlib/math.fd` with operator overloading + swizzle |
| String concatenation with `+` | ✅ | `str + str` lowers to `p_str_concat` call. Works with literals, variables, and function call results inline. `str + i64` remains pointer arithmetic. |
| Transitive `use` imports | ✅ | `use` imports in stdlib modules are automatically resolved (e.g. `use clinical` auto-loads `math`). |
| C codegen CallPtr void dispatch | ✅ | CallPtr dispatch checks each candidate's actual return type; aggregate returns handled as void to prevent C compile errors. |
| `select` statement | ✅ | CSP-style channel select (`select { case ch => x => { } default => { } }`) |
| Option/Result combinators | ✅ | `map`, `and_then`, `or_else`, `unwrap_or_else`, `expect` in `stdlib/core.fd` |
| Vec iterator adapters | ✅ | `take`, `skip`, `zip`, `enumerate`, `flat_map`, `chunk`, `window`, `dedup`, `sort_by` |
| Stack traces on panic | ✅ | Printed on panic. Compiled code walks the machine stack (unwind tables): nothing is recorded per call, and at -O2 inlined frames fold into their caller. The interpreter keeps its own call stack. |
| Assertion builtins | ✅ | `assert(cond)`, `assert_eq(a, b)`, `assert_msg(cond, msg)`, `assert_eq_msg(a, b, msg)` in stdlib/core.fd. `assert_msg`/`assert_eq_msg` take f-string formatted messages. |
| Recursion depth limit | ✅ | Interpreter: `--max-recursion N` (default 1000, 0 = unlimited), with a clear panic. Compiled code has no fixed depth: running out of stack hits the OS guard page and prints "Stack overflow: the recursion is too deep" with a trace. |
| Iterative parser fallback | ✅ | Shunting-yard parser activates for deeply nested expressions (>256 levels) to prevent stack overflow. |
| `@constant_time` | ✅ | Security annotation: no early returns, no data-dependent branches, no IO/Net |
| `@tainted` | ✅ | On a parameter: the function may have no `IO` or `Net` effect |
| `@secret` / `@sensitive` | ✅ | Information-flow control: secrets never reach output sinks; sensitive (personal) data only via `privacy::release`'s runtime consent check |
| `@derive` | ✅ | `@derive(Debug/Clone/Serialize/Deserialize)` generates methods |
| Profiler (`fidesc profile`) | ✅ | Per-function timing profiler (interpreter mode) |
| Incremental compilation | ✅ | `fid`: build cache in `.fidcache/` (LRU eviction past 64 files or 1 GB), core.fd check stamp. `fidesc`: in-memory cache (`src/inc.rs`) |
| Function-level parallelism | ✅ | 19/22 optimization passes parallelized via `std::thread::scope` |
| LEA instruction selection | ✅ | `leaq` for struct/array offset calculations instead of shlq+addq |
| Borrow-based alias analysis | ✅ | `BorrowKind` (Shared/Mutable/None) enables more store-load forwarding |
| x86-64 ABI compliance | ✅ | System V AMD64 ABI: RDI/RSI/RDX/RCX/R8/R9 + XMM0-XMM7, 16-byte alignment |
| **ML1: Region-scoped GPU memory** | ✅ | `region gpu r { }` — GPU allocations freed at block exit, escape check |
| **ML2: Comptime shape verification** | ✅ | Compile-time shape checking for matmul, conv2d, concat, flatten, etc. |
| **ML3: Tape-based reverse-mode autodiff** | ✅ | `@differentiable` annotation verified by typechecker. Runtime tape-based reverse-mode AD in interpreter: `grad(var) { ... backward(loss) }` computes gradients. `p_grad_get`/`p_grad_zero` manage gradient state. |
| **ML4: Hot-swap model weights** | ✅ | Extended `persist` with file-based persistence and pointer/Vec serialization |
| **ML8: comptime kernel specialization** | ✅ | `@kernel(x,y,z)` annotation — SPIR-V/PTX entry point with workgroup size |
| **ML9: KV cache with region memory** | ✅ | `region kv_cache r with budget(N) window(W) { }` — bounded cache, sliding window, compile-time OOM prevention |
| **ML10: Real quantization (INT8)** | ✅ | `@quantized` annotation, `quantize_q8`/`dequantize_q8`/`qmatmul`, symmetric quantization |
| **ML11: Gradient checkpointing** | ✅ | `@checkpoint` annotation verified by typechecker (requires `@differentiable` + `@deterministic`). Runtime checkpointing in interpreter: `p_checkpoint_begin`/`p_checkpoint_end` toggle activation saving/recompute mode. |
| **ML12: Differential privacy** | ✅ | `@dp(epsilon=N)` annotation verified by typechecker (must have Noise effect, no IO/Net). Runtime epsilon budget tracking: `p_dp_budget_reset`/`p_dp_budget_check` enforce privacy budget at runtime. |
| **ML13: Tensor parallelism** | ✅ | `@shard` annotation verified by typechecker (must have Comm effect, no IO/Net). Simulated multi-rank runtime: `p_tp_rank`/`p_tp_world_size` return rank info, `p_all_reduce` etc. work in single-rank mode. |
| **ML14: LoRA/Adapter hot-swap** | ✅ | `@adapter` annotation — pure weight transformations, works with `persist` for hot-swap |
| **`@realtime` annotation** | ✅ | Compile-time enforcement: no alloc, no blocking IO, no while loops, no recursion |
| **Safety rules (17)** | ✅ | Each with a negative test: [safety-rules.md](safety-rules.md). `fid qualify` writes the evidence |

---

## PE Emitter (`fidesc emit`) — Native Code Generation

The PE emitter generates standalone Windows x64 PE executables with no external linker.

### PE Infrastructure

| Feature | Status | Notes |
|---------|--------|-------|
| PE file structure (headers, sections) | ✅ | Valid PE32+ executable |
| Import table (kernel32.dll) | ✅ | ExitProcess, GetStdHandle, WriteFile, HeapAlloc, HeapFree, GetProcessHeap |
| IAT patching by loader | ✅ | .rdata section writable for IAT |
| RIP-relative addressing | ✅ | String constants and IAT calls |
| Label resolution (jumps/calls) | ✅ | Rel32 relocations |
| Entry point (`_main`) | ✅ | Calls main(), then ExitProcess(rax) |

### PE — Instruction Encoding

| Feature | Status | Notes |
|---------|--------|-------|
| `mov reg, imm64` / `mov reg, imm32` | ✅ | |
| `mov reg, [rbp+disp]` / `mov [rbp+disp], reg` | ✅ | Stack frame access |
| `mov reg, [reg]` / `mov [reg], reg` | ✅ | Pointer dereference |
| `mov [reg+disp], reg` | ✅ | Struct field access |
| `add` / `sub` / `imul` | ✅ | Arithmetic |
| `cmp` / `test` | ✅ | Comparison |
| `jcc` (conditional jumps) | ✅ | All common conditions |
| `call` (label / RIP-relative) | ✅ | Local and IAT calls |
| `push` / `pop` / `return` | ✅ | Stack operations |
| `xor` / `shl` / `inc` | ✅ | Bit and increment ops |
| `lea` (RIP-relative / RBP-relative) | ✅ | Address computation |
| `ud2` (trap) | ✅ | Safety trap |

### PE — Code Generation

| Feature | Status | Notes |
|---------|--------|-------|
| Function prologue/epilogue | ✅ | push rbp, mov rbp rsp, sub rsp, return |
| Stack frame allocation | ✅ | 16-byte aligned + 32 shadow space |
| Parameter passing (Win64 ABI) | ✅ | RCX, RDX, R8, R9 + stack |
| `ConstInt` | ✅ | Load immediate to stack slot |
| `ConstBool` / `ConstStr` / `ConstFloat` | ✅ | |
| `ConstNull` | ✅ | |
| Arithmetic ops (add, sub, mul, div) | ✅ | |
| Comparison ops | ✅ | |
| `Load` / `Store` | ✅ | Memory access |
| `Alloca` | ✅ | Stack allocation |
| `GetFieldPtr` | ✅ | Struct field addressing |
| `GetElemPtr` | ✅ | Array element addressing |
| `Call` (to runtime stubs) | ✅ | |
| `CallPtr` (closures) | ✅ | Tested with captured environment |
| `Jump` / `Branch` | ✅ | Control flow |
| `Return` | ✅ | Return value in RAX |
| `main()` return → exit code | ✅ | RAX moved to RCX for ExitProcess |

### PE — Language Feature Coverage

| Feature | Status | Test |
|---------|--------|------|
| `defer` | ✅ | `test_pe_defer.fd` |
| `?` operator | ✅ | `test_pe_try_op.fd` |
| References `&` / `&mut` | ✅ | `test_pe_refs.fd` |
| `enum` / `Option` / `Result` | ✅ | `test_pe_enum.fd` |
| `for` over Vec | ✅ | `test_pe_for_vec.fd` |
| `struct` + `impl` methods | ✅ | `test_pe_struct.fd` |
| Generics `<T>` | ✅ | `test_pe_generics.fd` |
| Closures with captures | ✅ | `test_closure_capture.fd` |
| `match` (int/string/enum/wildcard) | ✅ | `test_match_edge.fd` |
| HashMap (insert/get/contains/remove/len) | ✅ | `test_hashmap.fd` |
| `p_print_map` | ✅ | `test_print_map.fd` |
| f-string with float | ✅ | `test_fstring_float.fd` |
| `try { }` blocks | ✅ | `test_pe_try_block.fd` |

### PE — Runtime Stubs

| Stub | Status | Notes |
|------|--------|-------|
| `p_malloc` | ✅ | HeapAlloc wrapper |
| `p_free` | ✅ | HeapFree wrapper |
| `p_print_str` | ✅ | WriteFile to stdout |
| `p_print_int` | ✅ | Integer to string + WriteFile |
| `p_print_float` | ✅ | Float to string + WriteFile |
| `p_print_bool` | ✅ | "true"/"false" + WriteFile |
| `p_print_vec` | ✅ | Formats vector with brackets and commas |
| `p_vec_new` | ✅ | Allocates 24-byte zeroed Vec struct |
| `p_vec_push` | ✅ | Full grow logic: alloc, copy, free, update len/cap |
| `p_vec_len` | ✅ | Reads len from Vec struct |
| `p_vec_get` | ✅ | Reads element at index |
| `p_vec_pop` | ✅ | Removes and returns last element |
| `p_str_concat` | ✅ | Concatenates two null-terminated strings |
| `p_to_str` | ✅ | Integer to string (i64 → *u8) |
| `p_to_str_vec` | ✅ | Vec to string `[1, 2, ...]` |
| `p_to_str_str` | ✅ | String passthrough (returns input) |
| `p_to_str_bool` | ✅ | Bool to `"true"`/`"false"` string |
| `p_to_str_float` | ✅ | Float to string with 6 decimal digits, trailing zero strip, rounding |
| `p_map_new` | ✅ | Allocates 24-byte zeroed HashMap struct |
| `p_map_insert` | ✅ | Insert key-value pair with grow logic and string key compare |
| `p_map_get` | ✅ | Lookup by string key, returns value or 0 |
| `p_map_contains` | ✅ | Check if key exists, returns bool |
| `p_map_remove` | ✅ | Remove entry by key, shifts remaining entries |
| `p_map_len` | ✅ | Reads len from HashMap struct |
| `p_map_is_empty` | ✅ | Returns true if len == 0 |
| `p_print_map` | ✅ | Formats map as `{key: value, ...}` |

### PE — Known Issues

1. **Exit code capture** — `cmd /c %errorlevel%` does not reliably capture exit codes. Use `Start-Process -PassThru -Wait` in PowerShell for reliable exit code testing. The `main()` return value IS correctly propagated to `ExitProcess`.

---

## AI/ML Features

Six AI/ML-oriented features have been added to the Fides language, enabling neural network programming with compile-time safety, automatic differentiation, GPU kernel support, and quantization.

### Pipeline Operator (`|>`)

| Feature | Status | Notes |
|---------|--------|-------|
| `|>` operator | ✅ | Chains function calls: `x \|> f \|> g` desugars to `g(f(x))` |
| Bare identifier chaining | ✅ | `x \|> f` calls `f(x)` |
| Method call chaining | ✅ | `x \|> obj.method()` calls `obj.method(x)` |
| Field access chaining | ✅ | `x \|> obj.field` accesses `obj.field` |

**Syntax example:**
```p
val result = x |> square |> add_one |> print;
// equivalent to: print(add_one(square(x)));
```

### Layer/Model Syntax

| Feature | Status | Notes |
|---------|--------|-------|
| `layer` keyword | ✅ | Neural network layer declarations |
| `model` keyword | ✅ | Model declarations containing layers |
| Layer desugaring | ✅ | Layers desugar to structs + impls + constructor functions |
| Model desugaring | ✅ | Models desugar to structs containing layers + forward method |
| `forward` method | ✅ | Required method on layers and models |

**Syntax example:**
```p
layer Dense {
    w: Tensor,
    b: Tensor,
    forward(self, x: Tensor) -> Tensor {
        return matmul(x, self.w) + self.b;
    }
}

model MLP {
    l1: Dense(784, 128),
    l2: Dense(128, 10),
    forward(self, x: Tensor) -> Tensor {
        return x |> self.l1.forward |> relu |> self.l2.forward;
    }
}
```

### Shape-Level Type Safety

| Feature | Status | Notes |
|---------|--------|-------|
| `Tensor<f32, [784, 128]>` type syntax | ✅ | Compile-time shape checking |
| Concrete shape dimensions | ✅ | e.g. `784`, `128` |
| Symbolic shape dimensions | ✅ | e.g. `batch` |
| Shape verification for `matmul` | ✅ | Compile-time check: inner dimensions must match |
| Shape verification for `add`/`sub`/`mul` | ✅ | Compile-time check: shapes must be compatible |
| Shape verification for `transpose` | ✅ | Compile-time check: dimensions swap correctly |
| Shape mismatch as compile error | ✅ | Shape mismatches are compile errors, not runtime crashes |

**Syntax example:**
```p
val w: Tensor<f32, [784, 128]> = ...;
val x: Tensor<f32, [batch, 784]> = ...;
val y: Tensor<f32, [batch, 128]> = matmul(x, w);  // shape-checked at compile time
```

### Autograd Block

| Feature | Status | Notes |
|---------|--------|-------|
| `grad(var) { ... }` syntax | ✅ | Parsed, lowered to `GradBlock`/`GradEnd` IR instructions, executed by interpreter |
| `backward(loss)` statement | ✅ | Triggers reverse-mode AD: traverses tape in reverse, computes gradients via local derivative functions |
| Tape-based gradient computation | ✅ | Tape records tensor ops (`add`, `mul`, `relu`, `sigmoid`, etc.) with inputs. Backward pass computes gradients. |
| `@differentiable` annotation | ✅ | Typechecker verifies: no IO/Net/Spawn/Diverge, transitive. Compile-time proof of differentiability. |
| Compilation pipeline integration | ✅ | `GradBlock`/`GradEnd`/`Backward` instructions handled in interpreter, PIR lowering, C codegen, x86, PE, WASM backends |

**Syntax example:**
```p
grad(w) {
    val loss = compute_loss(w);
    backward(loss);
}
// After the grad block, w has been updated with computed gradients
```

### GPU Kernel Annotation

| Feature | Status | Notes |
|---------|--------|-------|
| `@kernel` annotation | ✅ | Marks functions as GPU kernels |
| `thread_id(dim)` builtin | ✅ | On GPU: maps to SPIR-V `GlobalInvocationId` / PTX `threadIdx`; on CPU: returns 0 |
| `workgroup_size(dim)` builtin | ✅ | Workgroup size query |
| `workgroup_id(dim)` builtin | ✅ | Workgroup index query |
| `num_workgroups()` builtin | ✅ | Total workgroup count |
| Configurable workgroup sizes | ✅ | Enhanced SPIR-V codegen |
| CPU fallback | ✅ | Single-threaded fallback (`thread_id()` returns 0) |

**Syntax example:**
```p
@kernel
fn vector_add(a: &Tensor, b: &Tensor, c: &mut Tensor, n: i64) {
    val i = thread_id(0);
    if i < n {
        c[i] = a[i] + b[i];
    }
}
```

### Quantized Types

| Feature | Status | Notes |
|---------|--------|-------|
| `q8` type alias | ✅ | Maps to `i8` storage |
| `q4` type alias | ✅ | Maps to `i8` storage |
| `~>` quantization cast operator | ✅ | `expr ~> q8` quantizes an expression |
| `quantize_q8` stdlib function | ✅ | `stdlib/quant.fd` |
| `dequantize_q8` stdlib function | ✅ | `stdlib/quant.fd` |
| `qmatmul` stdlib function | ✅ | Quantized matrix multiplication |
| `find_quant_params` stdlib function | ✅ | Find quantization scale and zero-point |

**Syntax example:**
```p
use quant;

val w: Tensor = ...;
val q_w = w ~> q8;                    // quantize weights to 8-bit
val q_x = x ~> q8;                    // quantize activations
val q_out = quant::qmatmul(q_x, q_w); // quantized matmul
val out = quant::dequantize_q8(q_out); // dequantize result
```

---

## Safety Features (Phase 7.5 — Tier 1 Safety)

### Compile-Time Safety Annotations

| Annotation | Status | What It Does |
|-----------|--------|-------------|
| `@deterministic` | ✅ DONE | Rejects FP, IO, Net, Spawn, Diverge effects — same inputs always produce same outputs |
| `@conserve` | ✅ DONE | Proves money-in = money-out at compile time (no mint, no IO, no float, only call @conserve funcs) |
| `@linear` | ✅ DONE | Move semantics on structs — no duplication, must consume, loop-aware |
| `@gas(N)` | ✅ DONE | Compile-time gas budget — counts AST nodes, propagates through call graph, rejects if > N |
| `@crashfree` | ✅ DONE | Compile-time panic prevention — rejects unwrap, p_panic, non-const div/mod, while loops, non-crashfree calls |
| `@audit` | ✅ DONE | Entry/exit logging with parameter values via IR injection. Type-aware printing. |
| `txn {}` | ⚠️ | Parsing + typechecking + interpreter rollback (memory snapshot on TxnBegin, restore on panic, commit on TxnEnd); no rollback in C or native |

### Financial Safety Stdlib

| Module | Status | Description |
|--------|--------|-------------|
| `stdlib/money.fd` | ✅ DONE | Fixed-point i64 cents, @linear struct, arithmetic, formatting |
| `stdlib/banking.fd` | ✅ DONE | Account, deposit, withdraw, transfer, interest, double-credit/lost-money detection, Account<State> typestate pattern |
| `stdlib/file.fd` | ✅ DONE | File I/O: read, write, append, exists |
| `stdlib/time.fd` | ✅ DONE | LogicalTime: timezone-free, clock-free, @deterministic date math |
| `stdlib/alloc.fd` | ✅ DONE | Heap allocator builtins (malloc, realloc, free) |
| `stdlib/privacy.fd` | ✅ DONE | Field-level privacy test module |

### Industry & Protocol Stdlib (New)

| Module | Status | Description |
|--------|--------|-------------|
| `stdlib/clinical.fd` | ✅ DONE | Clinical calculations: BMI, eGFR, CHA₂DS₂-VASc, MELD, NEWS2, creatinine clearance, pediatric dosing |
| `stdlib/crypto.fd` | ✅ DONE | SHA-256, HMAC, nonce tracking (NonceTracker struct), constant-time comparison |
| `stdlib/storage.fd` | ✅ DONE | Merkle tree (build, verify, root hash), KV store (set, get, state hash) |
| `stdlib/ecu.fd` | ✅ DONE | ECU abstractions: SensorReading, ActuatorCommand, PID controller, watchdog, calibration params |
| `stdlib/scada.fd` | ✅ DONE | Modbus protocol: read/write holding registers, frame serialization/deserialization |
| `stdlib/swift.fd` | ✅ DONE | SWIFT MT103 message format: construct, serialize, parse fields |
| `stdlib/grid.fd` | ✅ DONE | IEC 61850 data models: XCBR (breaker), MMXU (measurement), logical node references |
| `stdlib/medical.fd` | ✅ DONE | HL7 message parsing: MSH/PID/PV1/OBX segments, ADT^A01/A03, serialize/parse |
| `stdlib/ota.fd` | ✅ DONE | OTA update protocol: manifest, session, download/verify/apply/commit, binary diff |
| `stdlib/arinc.fd` | ✅ DONE | ARINC 429 protocol: label encoding/decoding, data types (BNR/BCD/discrete) |
| `stdlib/can.fd` | ✅ DONE | CAN bus: frame encoding/decoding, ID arbitration, standard/extended frames |

### Data Structures & Utilities Stdlib (New)

| Module | Status | Description |
|--------|--------|-------------|
| `stdlib/string.fd` | ✅ DONE | String utilities: reverse, capitalize, pad, count, split, join, hex/bin conversion |
| `stdlib/fmt.fd` | ✅ DONE | Formatting: int/hex/bin/bool, commas, padding, table rows, separators |
| `stdlib/bits.fd` | ✅ DONE | Bit manipulation: popcount, get/set/clear, power-of-two, masks, leading/trailing zeros |
| `stdlib/sort.fd` | ✅ DONE | Sorting: insertion, bubble, selection, quicksort, binary search, reverse |
| `stdlib/hash.fd` | ✅ DONE | Hashing: FNV-1a (32-bit), DJB2, CRC-32, checksum, combine |
| `stdlib/iter.fd` | ✅ DONE | Iterator adapters: map, filter, reduce, sum, min/max, chain, chunk, partition, flatten |
| `stdlib/stack.fd` | ✅ DONE | Stack: push, pop, peek, contains, drain (Vec-backed) |
| `stdlib/queue.fd` | ✅ DONE | Queue: enqueue, dequeue, peek, compact (Vec-backed) |
| `stdlib/buffer.fd` | ✅ DONE | ByteBuffer: read/write u16/u32/u64 (LE/BE), string, bytes, cursor |
| `stdlib/collections.fd` | ✅ DONE | Deque, LinkedList (parallel arrays), Set — union, intersection, difference |
| `stdlib/result.fd` | ✅ DONE | ResultValue type: Ok/Err, is_ok/err, unwrap, map, and_then, or_else |
| `stdlib/random.fd` | ✅ DONE | Pure-Fides LCG RNG: seed, int, float, bool, shuffle, hex (no FFI) |
| `stdlib/path.fd` | ✅ DONE | Path utilities: join, basename, dirname, extension, stem, is_absolute |
| `stdlib/units.fd` | ✅ DONE | Units of measurement: meters, seconds, kg, newtons, joules, watts, constants |
| `stdlib/uuid.fd` | ✅ DONE | UUID v4 generation, nil, parse, to_string, version, equality |
| `stdlib/fs.fd` | ✅ DONE | Filesystem operations |
| `stdlib/io.fd` | ✅ DONE | I/O utilities |
| `stdlib/os.fd` | ✅ DONE | OS interface |
| `stdlib/sys.fd` | ✅ DONE | System-level functions |

### ML Inference Stdlib (New)

| Module | Status | Description |
|--------|--------|-------------|
| `stdlib/kv_cache.fd` | ✅ DONE | KV cache for LLM serving: bounded memory, sliding window eviction, per-layer K/V tensors |
| `stdlib/quant.fd` | ✅ DONE | Quantization: q8 quantize/dequantize, qmatmul, find_quant_params, @quantized annotation |
| `stdlib/dp.fd` | ✅ DONE | Differential privacy: @dp annotation, noise injection, epsilon tracking, Noise effect |
| `stdlib/shard.fd` | ✅ DONE | Tensor parallelism: @shard annotation, all-reduce, broadcast, all-gather, scatter, Comm effect |

### Language Features Added in Phase 7.5

| Feature | Syntax | Status |
|---------|--------|--------|
| Tuple destructuring | `val (a, b) = expr` | ✅ DONE |
| Loop-aware linear types | Linear vars can't be consumed in loops | ✅ DONE |
| `txn {}` blocks | `txn { ... }` | ✅ DONE (with interpreter rollback) |
| Raw pointer types | `*mut T`, `*const T` | ✅ DONE (with `unsafe {}` enforcement) |
| Memoization | `stdlib/memo.fd` | ✅ DONE (Memo::new, get_or_compute) |
| Checked arithmetic (F64) | overflow = panic | ✅ DONE (always on, not just debug mode) |
| `@wrapping` (F65) | `@wrapping fn foo() { ... }` | ✅ DONE (escape hatch for wrap-around) |
| Bounds checking lock (F62) | bounds checks always on | ✅ DONE (skippable in `unsafe {}`) |
| Parameterized queries (F68) | no concat on typed strings | ✅ DONE (compile error on SQLString + str) |
| `@medical` (F48) | Checks named after IEC 62304 (not compliance) | ✅ DONE (no alloc, no recursion, no while, no panic, must be @audit) |
| `given`/`ensure` contracts (F10) | Design by Contract | ✅ DONE (preconditions verified at call sites with constant args) |
| `async`/`await`/`spawn` (Phase 7.6) | Cooperative concurrency | ⚠️ (green tasks on their own stacks; interpreter and C work; native async crashes) |
| Borrow checker (`--borrowck`) | Compile-time aliasing analysis | ✅ Phases 1-6 (Rules 1-3, field-wise, two-phase, NLL; `on` by default; tested) |
| Generic enums (`enum Option<T>`) | Type-parameterized enums | ✅ DONE (methods instantiated per `T` in `fid`; a bare `Option` has `T = i64`) |
| Generic structs (`struct Foo<T>`) | Type-parameterized structs | ✅ DONE (AST, parser; type_params field) |
| LSP server (`fidesc lsp`) | Language Server Protocol | ✅ DONE (JSON-RPC over stdio, initialize/didOpen/didChange/diagnostics) |
| Iterators (Phase 7.7) | `for x in vec/map/range` | ✅ DONE (Vec, HashMap, Range iteration; nested Vec support) |
| Error chaining (Phase 7.7) | `?` on Option/Result | ✅ DONE (propagates errors, try blocks, Option/Result methods) |
| Conditional compilation (Phase 7.7) | `@cfg(platform = "linux")` | ✅ DONE (filters functions/structs by platform/debug/release) |
| Derive macros (Phase 7.7) | `@derive(Debug/Clone)` | ✅ DONE (generates debug_str/clone methods for structs) |
| Serde (Phase 7.7) | `@derive(Serialize/Deserialize)` | ✅ DONE (generates to_json/from_json methods for structs) |
| Dynamic loading (Phase 7.7) | `p_dl_open/sym/close/call` | ✅ DONE (real dlopen/dlsym in C codegen, simulated in interpreter) |
| Select statement (Phase 3.2) | `select { case ch => x => { } default => { } }` | ✅ DONE (multi-channel multiplexing, AST + parser + typechecker + lowerer) |
| Type inference (Phase 3.3) | Bidirectional + `-> _` | ✅ DONE (contextual literal inference, return type inference via `Type::Infer`) |
| Auto-vectorization (Phase 3.4) | Loop-vec + SLP | ✅ DONE (loop-vec for constant trips, SLP vectorizer with p_simd_load/store/add/sub/mul/div → SSE2) |
| Binary hardening (Phase 12) | Stack canaries, PIE, RELRO, NX stack | ✅ DONE (all C codegen output hardened by default, no opt-in) |
| @constant_time (Phase 12) | Side-channel defense | ✅ DONE (no early returns, no data-dependent branches, no IO/Net/Spawn) |
| @tainted (Phase 12) | Injection defense | ✅ DONE (prevents tainted values in SQL/shell/HTML/path sinks) |
| Stack traces (Phase 12) | Panic call stack | ✅ DONE (interpreter; C codegen from the machine stack since Nov 2, 2026, replacing per-call p_call_stack_push/pop) |

---

## New Stdlib Modules (Phase 11)

| Module | Status | Description |
|--------|--------|-------------|
| `stdlib/math.fd` | ✅ DONE | Math functions: sqrt, pow, floor, ceil, sin, cos, fabs, constants (pi, e) |
| `stdlib/env.fd` | ✅ DONE | Environment variables: env_get, env_exists |
| `stdlib/args.fd` | ✅ DONE | Command-line args: args_len, args_get, args_program_name |
| `stdlib/process.fd` | ✅ DONE | Process control: process_exit, process_system |
| `stdlib/regex.fd` | ✅ DONE | Wildcard pattern matching: regex_matches (?/*), regex_find, regex_contains, regex_starts_with, regex_ends_with |

---

## LSP Server (Phase 11)

The `fidesc lsp` command runs a Language Server Protocol server over stdio,
enabling editor integration (VS Code, Neovim, Emacs, etc.) with live diagnostics.

| Feature | Status | Notes |
|---------|--------|-------|
| `initialize` / `initialized` | ✅ | Returns capabilities (textDocumentSync, diagnosticProvider) |
| `shutdown` / `exit` | ✅ | Clean shutdown |
| `textDocument/didOpen` | ✅ | Publishes diagnostics on file open |
| `textDocument/didChange` | ✅ | Full sync — re-publishes diagnostics on every change |
| `textDocument/didClose` | ✅ | Clears document state |
| `textDocument/publishDiagnostics` | ✅ | Sends error diagnostics with line/column positions |
| JSON-RPC 2.0 | ✅ | Built-in JSON parser/serializer — no external dependencies |

---

## Summary

| Component | Working | Partial | Broken |
|-----------|---------|---------|--------|
| Lexer | 100% | — | — |
| Parser | 100% | — | — |
| Type Checker | 100% | — | — |
| IR Lowering | 100% | — | — |
| Interpreter | 339/339 | — | Nov 2 full suite (Rust harness) |
| PE Emitter | 345/345 | — | Nov 2 full suite (Rust harness) |
| ELF64 Emitter | — | ⚠️ | Experimental / incomplete |
| C Codegen | 343/343 | — | Nov 2 full suite (Rust harness). Binary hardening enabled. |
| Multi-arch Codegen | — | ⚠️ | ARM64, RISC-V experimental / incomplete |
| Cross-compilation (ARM64) | — | ⚠️ | `build --target arm64-linux` → AArch64 ELF64; experimental |
| Cross-compilation (RISC-V) | — | ⚠️ | `build --target riscv64-linux` → RISC-V ELF64; experimental |
| Cross-compilation (Windows) | ✅ | — | `build --target x86-64-windows` → PE executable (98KB) |
| ARM64 Encoder | ✅ (17 tests) | — | `arm64_enc.rs` — ARM64 assembly → machine code |
| ARM64 Runtime | ✅ (9 tests) | — | `runtime_arm64.rs` — SVC syscalls |
| RISC-V Encoder | ✅ (14 tests) | — | `riscv_enc.rs` — RV64 assembly → machine code |
| RISC-V Runtime | ✅ (7 tests) | — | `runtime_riscv.rs` — ECALL syscalls |
| Peephole Optimizer | ✅ (9 passes) | — | Always on in `build` |
| L2 Optimizer | ✅ (23 passes) | — | Enabled by default in `build` (use `--no-opt` to disable). `--lto` for aggressive inlining. |
| Register Allocator | ✅ | — | Linear scan with constant rematerialization (F-1A). Effect-aware allocation (F-1D). |
| Source Maps | ✅ | — | `sourcemap` command works |
| IR Verification | ✅ | — | `audit` command works (also `verify`) |
| WASM Backend | ✅ | — | `wasm` command works |
| GPU/SPIR-V Backend | ✅ | — | `gpu` command works |
| LSP Server | ✅ | — | `fidesc lsp` — JSON-RPC over stdio, diagnostics, no external deps |
| Generic Enums/Structs | ✅ | — | `enum Option<T>`, `struct Foo<T>` — type params in AST/parser/typechecker/lowerer |
| Linter | ✅ | — | `fidesc lint` — unused functions, unused imports |
| Benchmark | ✅ | — | `fidesc bench` — interpreter and C native timing |
| Format | ✅ | — | `fidesc fmt` — source formatting |
| Stack Traces | ✅ | — | Panic prints call stack (interpreter + C codegen) |

### Test Coverage

Counts are in [CURRENT.md → Numbers](../../CURRENT.md#numbers). The last full
suite run is Nov 2 (interp 339/339, C 343/343, PE 345/345, expected-fail
89/89); it was not rerun with the Nov 4–6 compiler. The saved baselines are
`tests/bootstrap/baseline/test_results.txt` and
`tests/selfhost/baseline/test_results.txt`.

### Self-Hosting (current state in [selfhost.md](selfhost.md))

| Feature | Status | Notes |
|---------|--------|-------|
| Self-compilation | ✅ | `fides-c.exe` compiles `compiler/main.fd` → up to `fides-c5.exe` (5 generations; session-24 regression fixed in session 26) |
| Bootstrap fixpoint | ✅ | stage 2 == stage 3 byte-identical (49 MB of C, Nov 6), checked by `RELEASE=1 scripts/bootstrap.sh` on every release build; stage 1 comes from a pinned stage 0 when one exists (`PIN=1` pins one; none yet), otherwise from `fidesc` |
| Self-host modules | ✅ | 61 `.fd` modules, 133,688 lines (Nov 6; was 25/61K in session 15) |
| Self-host tests | ✅ | 51 programs in `tests/selfhost/` (`verify-selfhost.sh`; saved baseline 36/47×3) |
| Self-compile time | ✅ | ~133 s (type check 59 s, borrow check 16 s, lowering 20 s, C generation 12 s; Nov 6); `fidesc` ~21 s |
| PE emitter (self-hosted) | ✅ | `fid emit` (`compiler/pe_ir*.fd`) makes native Windows executables with no C compiler and reaches a byte-identical fixpoint. The older `build --native` pipeline (`compiler/pe.fd`: PIR → x86-64 → PE) still crashes in the optimizer and register allocator |
| LSP server (self-hosted) | ✅ | `compiler/lsp.fd` (1,082 lines) — JSON-RPC over stdio |
| Linux native codegen (self-hosted) | ⚠️ | `peephole.fd` + `regalloc.fd` + `codegen_x64.fd` — wired into the L3 → x86-64 → asm pipeline (session 28-29); experimental, Linux x64 goes through C |
| Module-level `var` in self-hosted | ✅ | Supported on all backends since session 11 |
| Standalone binary | ✅ | `release/fid.exe` (`RELEASE=1 scripts/bootstrap.sh`, fixpoint checked), shipped as `fid.exe`; `run` and `ccompile` match `fidesc` on the test programs |
