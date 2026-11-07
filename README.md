<h1 align="center">Fides Language — Self-Hosted Compiler</h1>

**Fides is a self-hosted systems programming language.** The Fides compiler is written in Fides, compiles itself, and produces byte-identical output across generations (bootstrap fixpoint). It is the release compiler: the Windows installer ships it as `fid.exe`. The Rust compiler (`src/`) is the bootstrap reference implementation, used to build the self-hosted compiler and shipped beside it as `fidesc.exe`.

The self-hosted compiler generates **native PE executables on Windows** and **C code** for portability. It includes a full IR interpreter and a PE emitter for direct native code generation. A bare-metal flat binary mode (`fid emit --bare`) works since Nov 6 on the hosted test runner, not yet under QEMU. ARM64/RISC-V assembly and WebAssembly are started but do not work yet (Nov 6: `fid wasm` output fails validation, the ARM64/RISC-V assembly is not assembled or run).

## Status

**Self-Hosting — Bootstrap fixpoint achieved and verified.** The Fides compiler compiles itself to byte-identical C across generations (stage 2 == stage 3, 49 MB of C, verified Nov 6, 2026). `RELEASE=1 ./scripts/bootstrap.sh` checks this on every release build.

**63 self-hosted modules** (~142K lines) in `compiler/*.fd` — lexer, parser, type checker, borrow checker, escape analyzer, monomorphizer, IR lowering, IR and PIR optimizers, C codegen, native PE emitter, x86-64/ARM64/RISC-V codegen, register allocator, peephole optimizer, comptime evaluator, LSP server, formatter, diagnostics, JSON, incremental compilation, ELF/COFF/Mach-O writers, per-arch runtimes, and the Ark package manager.

**The language is what `fid` accepts.** The rules, each with the compiler function that enforces it, are in the [language reference](docs/reference/reference.md). The Rust compiler (`fidesc`) is frozen and checks less: [the differences](docs/reference/language-guide.md).

**Tests.** The last full suite run is from Nov 2 (interpreter 339/339, C 343/343, native Windows 345/345, expected-fail 89/89), **with `fidesc`, the Rust compiler, as the compiler under test**: the shipped `fid` has not run the full suite. The compiler of Nov 4–6 was checked on the new and the key test programs on all three backends, and every new expected-fail test is rejected; the full suite has not been rerun with it.

**New in `fid` (Nov 4–6):** generics with inference, bounds and turbofish for functions, enums and `impl<T>` ([0002](docs/proposals/0002-real-generics.md); a literal of a user-defined generic struct is still rejected); `if`/`match` as values; `@cfg`; freeing of locals the compiler proves finished; iterator-invalidation and field-borrow checks; `@send` on spawned arguments; `parallel {}` branches on threads; `async fn` with float and struct arguments; every safety annotation enforced with a negative test per rule ([safety-rules.md](docs/reference/safety-rules.md)) and `fid qualify` evidence; owned and borrowed strings (`&str`, `@scoped`, `str_clone`); integer-key `HashMap`s; per-call-site effects of higher-order functions.

**Limits:**
- The self-hosted `ccompile` and `build` go through C, so they need a C compiler (`$CC`, gcc, clang, `zig cc` or tcc). For native Windows executables without a C compiler, use `fid emit`; the older `build --native` path does not work.
- Native output without a C compiler: Windows x64 (`fid emit`), and since Nov 7 Linux AArch64 and RISC-V 64 (`fid emit --target aarch64-linux|riscv64-linux`: 225 of the 231 suite tests that build match the interpreter under QEMU; threads, tasks, channels, atomics and the C math functions are not there yet). Other systems go through C: `fid ccompile --target aarch64-linux` (also `riscv64-linux`, `x86_64-linux`, `x86_64-macos`, `aarch64-macos`, `wasm32-wasi`) cross-compiles with `zig cc` from any host. Linux ARM64 matched the interpreter on 270 of 286 tests under QEMU; RISC-V and WASI run sample programs; macOS builds but has not been run. See [platforms](docs/reference/platforms.md).
- Native `async`/`spawn` + `await` work since Nov 6 (the worker threads were started through a name lookup that returned 0). The interpreters (`fid run`, `fidesc run`) run an `async fn` to the end when it is called instead of until it first waits, so a program that interleaves tasks prints in a different order there (`test_async_suspend`).
- Value semantics ([0001](docs/proposals/0001-value-semantics.md)): copy-on-write was switched on in the working tree on Nov 6 (`LOWER_COW`, fixpoint reached with it); the full suite has not run with it. The compiler of Nov 2 (`release/fid.exe`) still gives a second name for the same data on `var b = a` for a `Vec`.
- Some compile-time checks are weaker than their names (checked by running, Nov 6): `sandbox {}` does not see IO done by the functions it calls, `region ... with budget(N)` counts declared locals but not heap growth, `@conserve` only forbids `Money_new`, `@crashfree` does not bound recursion, and `txn {}` rolls nothing back on C or native. The list, with what does hold: [positioning.md](docs/reference/positioning.md) (top).
- Speed: fib(40) takes 1.34 s native and 0.81 s through C, against 0.53 s for Rust (Nov 2, [scoreboard](docs/roadmap/competitive-roadmap.md#the-scoreboard)). `fid` compiles its own ~134K lines to C in about 133 s; the native backend has no full register allocator.
- Fides is not suitable for safety-critical use: `fid qualify` gives evidence, not a certification.

**Numbers:** [CURRENT.md → Numbers](CURRENT.md#numbers) is the one place counts and measurements are kept.

**Phase status:** See `docs/roadmap/roadmap.md` for the consolidated roadmap (features, release, self-host, limitations, opportunities).

### Financial Safety Features (New)

| Feature | Syntax | Description |
|---------|--------|-------------|
| Linear types | `@linear` | Move semantics — no duplication of linear values |
| Conservation check | `@conserve` | Forbids `Money_new` and requires linear use of `Money`; not a proof: `Money_from_cents` still mints (Nov 6) |
| Money stdlib | `stdlib/money.fd` | Fixed-point money type, arithmetic, formatting |
| Banking stdlib | `stdlib/banking.fd` | Accounts, transfers, double-credit/lost-money detection |
| Tuple destructuring | `val (a, b) = expr` | Decompose tuple values into bindings |
| File I/O | `stdlib/file.fd` | File descriptors, read/write via `stdlib/file.fd` |
| Transaction blocks | `txn { ... }` | The interpreter snapshots memory at the start; C and native treat it as a plain block (no rollback) |
| Gas budgets | `@gas(N)` | Compile-time count of the operations of the body and its callees; recursion is not bounded |
| Crash-free check | `@crashfree` | Rejects unwrap, unproven `/`, unchecked arithmetic, indexing and calls to unannotated functions; recursion depth is not bounded |
| Audit logging | `@audit` | Automatic function audit logging with parameter values |
| Loop-aware linear types | — | Linear variables cannot be consumed inside loops (compile error) |
| LogicalTime | `stdlib/time.fd` | Timezone-free, clock-free deterministic date math |
| Typed strings | `stdlib/typed_strings.fd` | SQLString, ShellString, HTMLString, JSONString, PathString — `SQLString + str` is a type error; the constructors (`sql_query`) still accept any string |
| Init guarantee | `var` keyword (without initializer) | Uninitialized mutable variables + definite assignment analysis (compile error on read before assignment) |
| Raw pointer types | `*mut T`, `*const T` | Raw pointers with `unsafe {}` enforcement (no borrow checking) |
| Memoization | `stdlib/memo.fd` | Simple cache helper: `Memo::new()`, `get_or_compute()` |
| Checked arithmetic | F64 | Integer overflow = panic (always on, not just debug mode) |
| `@wrapping` | `@wrapping fn foo() { ... }` | Escape hatch for wrap-around arithmetic (hashing, crypto) |
| Bounds checking lock | F62 | Bounds checks always on, skippable in `unsafe {}` blocks |
| Parameterized queries | F68 | No string concat on typed strings (SQLString + str = compile error) |
| `@medical` | `@medical @audit fn foo() { ... }` | IEC 62304: no dynamic alloc, no recursion, no while, no unwrap, must be @audit |
| `given` contracts | `fn foo(x: i64) -> i64 given x >= 0 { ... }` | Preconditions: a call whose constant arguments break one is rejected at compile time (`ensure` parses but is not checked) |
| `async`/`await`/`spawn` | `async fn foo() -> Future<T> { ... }` | Green tasks on worker threads — async functions, await, spawn (interpreters run an async body to the end at the call) |
| Borrow checker | `--borrowck=on` / `--borrowck=warn` | Compile-time aliasing analysis — `&mut` exclusivity, no mutation while borrowed |

### Tier 3 OS Features (New — Session 16)

| Feature | Syntax | Description |
|---------|--------|-------------|
| `@cpu_local` variables | `@cpu_local var x: i64;` | Per-CPU data via the GS segment, in the native kernel path (parsed; not checked by `fid`) |
| Inline asm clobbers | `asm!("...", clobber("rax"))` | Clobber lists for inline assembly — compiler saves/restores clobbered registers |
| No-stdlib mode | `--bare` / `--no-std` / `--no-stdlib` | Skips stdlib/core.fd entirely — zero hidden runtime dependencies for kernel/UEFI |
| Cross-compilation dispatch | `--target arm64` / `--target riscv64` | Target validation with honest diagnostics, C codegen fallback |
| UEFI boot services | `use uefi;` | Moved to the `fides-os` project — AllocatePool, FreePool, GetMemoryMap, ExitBootServices, console output |

### What Works (Verified End-to-End)

Most rows were verified with `fidesc` and the Nov 2 suite; rows corrected on Nov 6 describe `fid`, checked by running small programs.

| Feature | Interpreter (`run`) | PE Emitter (`emit`) | C Codegen (`ccompile`) |
|---------|:--------------------:|:-------------------:|:----------------------:|
| String literals + `print` / `println` | Yes | Yes | Yes |
| Integer arithmetic + `print` | Yes | Yes | Yes |
| Float arithmetic + printing | Yes | Yes | Yes |
| `if` / `else if` / `else` | Yes | Yes | Yes |
| `for` loops with `..` and `..=` ranges | Yes | Yes | Yes |
| `while` / `loop` loops | Yes | Yes | Yes |
| `break` / `continue` | Yes | Yes | Yes |
| Mutable variables (`var`) | Yes | Yes | Yes |
| Constants (`const`) | Yes | Yes | Yes |
| User-defined functions | Yes | Yes | Yes |
| Recursive function calls | Yes | Yes | Yes |
| F-string interpolation (`f"{expr}"`) | Yes | Yes | Yes |
| F-string with int, float, vec | Yes | Yes | Yes |
| String concatenation | Yes | Yes | Yes |
| Bitwise operators (`& | ^ ~ << >>`) | Yes | Yes | Yes |
| Compound assignment (`+= -= *=` etc.) | Yes | Yes | Yes |
| Hex / binary / octal literals | Yes | Yes | Yes |
| Structs (definition, literals, field access) | Yes | Yes | Yes |
| Struct methods (`impl` blocks) | Yes | Yes | Yes |
| Pattern matching (`match`) | Yes | Yes | Yes |
| Enums with data | Yes | Yes | Yes |
| References (`&` / `&mut` / `*`) | Yes | Yes | Yes |
| Closures (`fn(x) => expr`) | Yes | Yes | Yes |
| Closure environment capture | Yes | Yes | Yes |
| Generic functions (`fn<T>`) | Yes | Yes | Yes |
| Monomorphization (generic specialization) | Yes | Yes | Yes |
| `?` operator | Yes | Yes | Yes |
| `try { }` blocks | Yes | Yes | Yes |
| Tuples | Yes | Yes | Yes |
| Arrays (`[1, 2, 3]`, indexing, assignment) | Yes | Yes | Yes |
| `Vec<T>` (push, pop, len, get, etc.) | Yes | Yes | Yes |
| `HashMap<K,V>` (insert, get, contains, etc.) | Yes | Yes | Yes |
| `for` loop over Vec | Yes | Yes | Yes |
| `defer` blocks | Yes | Yes | Yes |
| `Option<T>` (Some/None, unwrap, etc.) | Yes | Yes | Yes |
| `Result<T,E>` (Ok/Err, unwrap, etc.) | Yes | Yes | Yes |
| Traits (`cap` keyword, `impl Cap for Type`) | Yes | Yes | Yes |
| Module imports (`use foo::bar`) | Yes | Yes | Yes |
| `use` aliases, selective, glob imports | Yes | Yes | Yes |
| Capabilities (grants, denial, transitive) | Yes | Yes | Yes |
| Type casts (`as`) | Yes | Yes | Yes |
| `print` for Vec/Map (deterministic output) | Yes | Yes | Yes |
| Type aliases (`type X = T`) | Yes | Yes | Yes |
| Match guards (`pattern if cond =>`) | Yes | Yes | Yes |
| Match range patterns (`1..=5 =>`) | Yes | Yes | Yes |
| Trait associated types (`type Item;`) | Yes | Yes | Yes |
| Trait default methods | Yes | Yes | Yes |
| Trait objects (`dyn Trait`) | Yes | Yes | Yes |
| Trait impl verification | Yes | Yes | Yes |
| Inline assembly (`asm!("...")`) | Yes | Yes | Yes |
| Effect annotations (`!IO`, `!{}`, `!{IO,Net}`) | Yes | Yes | Yes |
| Effect inference (fixpoint over call graph) | Yes | Yes | Yes |
| Effect checking (under-declaration errors) | Yes | Yes | Yes |
| Effect handlers (`handle { do } with`) | Works (basic dispatch) | Works (basic dispatch) | Works (basic dispatch) |
| Region blocks (`region r { ... }`) | Yes | Yes | Yes |
| Region use-after-free detection | Compile-time | Compile-time | Compile-time |
| Region escape detection | Compile-time | Compile-time | Compile-time |
| Capability elision (zero runtime overhead) | Compile-time | Compile-time | Compile-time |
| Bounds checking (Vec/array/string) | Always on (F62) | Always on (F62) | Always on (F62) |
| Integer overflow checking | Always on (F64) | Always on (F64) | Always on (F64) |
| `Option::unwrap` panics on None | Yes | Yes | Yes |
| Bare-metal mode (`emit --bare`) | N/A | Yes since Nov 6 (flat binary; not yet under QEMU) | N/A |
| `comptime` blocks + reflection | Yes | Yes | Partial |
| C FFI (`extern "C"`) | Yes (stubs) | Yes | Yes |
| Const generics (`[T; N]`) | Yes | Yes | Yes |
| GATs (Generic Associated Types) | Yes | Yes | Yes |
| SIMD types (`stdlib/simd.fd`) | Yes | Yes | Partial |
| Tensor type (`stdlib/tensor.fd`) | Yes | Yes | Partial |
| JSON parsing (`stdlib/json.fd`) | Yes | Yes | Partial |
| HTTP server (`stdlib/http.fd`) | Yes | Yes | N/A |
| Database framework (`stdlib/db.fd`) | Yes | Yes | N/A |
| Parallel operations (`stdlib/parallel.fd`) | Yes | Yes | N/A |
| Money stdlib (`stdlib/money.fd`) | Yes | Yes | Yes |
| Banking stdlib (`stdlib/banking.fd`) | Yes | Yes | N/A |
| File I/O (`stdlib/file.fd`) | Yes | Yes | Yes |
| Linear types (`@linear`) | Yes | Yes | Yes |
| `@conserve` (no `Money_new`, linear `Money`) | Compile-time | Compile-time | Compile-time |
| Tuple destructuring (`val (a, b) = expr`) | Yes | Yes | Yes |
| `txn {}` rollback | Snapshot only | No (plain block) | No (plain block) |
| `@gas` gas budgets (no recursion bound) | Compile-time | Compile-time | Compile-time |
| `@crashfree` (no recursion bound) | Compile-time | Compile-time | Compile-time |
| `@audit` audit logging | Yes | Yes | Yes |
| Loop-aware linear types | Compile-time | Compile-time | Compile-time |
| LogicalTime (`stdlib/time.fd`) | Yes | Yes | Yes |
| Typed strings (`stdlib/typed_strings.fd`) | Yes | Yes | Yes |
| Init guarantee (`var` keyword (without initializer)) | Yes | Yes | Yes |
| WebAssembly backend (`fidesc wasm`) | N/A | N/A | N/A |
| SPIR-V GPU backend (`fidesc gpu`) | N/A | N/A | N/A |
| Hot reload (`fidesc watch`) | N/A | N/A | N/A |
| Multi-arch codegen (x86-64/ARM64/RISC-V) | N/A | N/A | N/A |
| Cross-compilation (ARM64 Linux ELF) | N/A | `fidesc build --target arm64-linux` (runtime stubs) | N/A |
| Cross-compilation (RISC-V Linux ELF) | N/A | `fidesc build --target riscv64-linux` (runtime stubs) | N/A |
| Cross-compilation (Windows PE) | N/A | `fidesc build --target x86-64-windows` | N/A |
| ELF64/Mach-O/COFF object files | N/A | N/A | N/A |
| Integrated linker (`fidesc link`) | N/A | N/A | N/A |
| Embedded/IoT support (`embedded.rs`) | N/A | N/A | N/A |
| `Rc<T>` / `Arc<T>` smart pointers | Yes | Yes | Yes |
| `Box<T>` recursive types | Yes | Yes | Yes |
| Vec/Mat math types (`vec2/3/4`, `mat4`, `quat`) | Yes | Yes | Yes |
| Swizzle access (`.xyz`, `.rgb`) | Yes | Yes | Yes |
| Operator overloading (vec + vec, etc.) | Yes | Yes | Yes |
| `@derive(Debug/Clone/Serialize/Deserialize)` | Yes | Yes | Yes |
| `select` statement (CSP channels) | Yes | Yes | Yes |
| Option/Result combinators (`map`, `and_then`, etc.) | Yes | Yes | Yes |
| Vec iterator adapters (`take`, `skip`, `zip`, etc.) | Yes | Yes | Yes |
| Stack traces on panic | Yes | No (message only) | Yes |
| `@constant_time` security annotation | Compile-time | Compile-time | Compile-time |
| `@tainted` taint tracking | Compile-time | Compile-time | Compile-time |
| Profiler (`fidesc profile`) | Yes | N/A | N/A |
| Linter (`fidesc lint`) | Yes | Yes | Yes |
| Benchmark (`fidesc bench`) | Yes | N/A | Yes |
| Formatter (`fidesc fmt`) | Yes | Yes | Yes |
| Package manager (`ark`) | N/A | N/A | N/A |
| Doc generator (`scribe`) | N/A | N/A | N/A |
| Coverage tool (`veil`) | N/A | N/A | N/A |
| Incremental compilation | N/A | Yes | N/A |
| Function-level parallelism (opt) | N/A | Yes | N/A |

### What Is Parsed But Not Fully Working

| Feature | Status | Notes |
|---------|--------|-------|
| `async` / `await` | ✅ Works (C, native since Nov 6) | An `async fn` is a task that runs until it first waits; `await` suspends it. Float and struct arguments and results work (Nov 4). The interpreters run an async body to the end at the call. |
| `spawn` | ✅ Works | `spawn f(args)` runs `f` on an OS thread; every argument must be `@send` (references and raw pointers are not). `spawn { block }` runs the block in place. |
| `#[test]` attribute | Parsed | Not yet executed by `fidesc test` |
| `@derive` | ✅ Works | Phase 7.7 — `@derive(Debug/Clone/Serialize/Deserialize)` generates methods |
| Effect handler continuations | Syntax works | `handle { do } with` parses and runs; full continuation semantics pending |
| `--opt` (L2 optimizer) | ✅ Works | 23 passes, enabled by default in `build` (use `--no-opt` to disable). `--lto` for aggressive inlining. |
| Default and named arguments | ✅ Works | `fn f(x: i64, y: i64 = 2)`, `f(1, y: 3)` |
| Variadic arguments | Parsed | `...` parses and does nothing |

### Known Limitations

- **ELF64 executables on Linux: `fidesc` only, tier 2** — `fidesc build <file> --target x86-64` produces ELF64 executables directly and works for the programs tried, but is not in CI; `fid` has no ELF backend yet. `fidesc codegen --elf <out.o>` emits relocatable objects; `--macho` and `--coff` emit object files (no macOS executables yet).
- **ARM64 and RISC-V native: tier 3** — `fidesc build --target arm64-linux` / `--target riscv64-linux` run the built-in encoders (`arm64_enc.rs`, `riscv_enc.rs`), but the runtimes (`runtime_arm64.rs`, `runtime_riscv.rs`) still have stubs that do nothing. Use the C backend with a cross-compiler for these targets. See [platforms](docs/reference/platforms.md).
- **PE emitter targets Windows x64 only** — `fid emit`, `fidesc emit` or `fidesc build --target x86-64-windows` produce PE32+ executables; `fidesc`'s emitter (src/pe.rs) matched the interpreter on 345/345 (Nov 2); `fid emit` (compiler/pe_ir*.fd) has not run the full suite
- **Register allocator** — Linear scan allocator with constant rematerialization (F-1A). Effect-aware allocation (F-1D): pure/@no-alloc calls only clobber RAX. Works correctly for all passing tests.
- **Peephole optimizer always on** — 9 passes run automatically in the `build` command: redundant moves, zero idiom, nop arithmetic, branch threading, dead store, redundant test, store-to-load forwarding, LEA coalescing, cmp+setcc+jcc instruction selection
- **L2 optimizer enabled by default** — PIR optimizer runs automatically in `build` (use `--no-opt` to disable) with 23 passes: const-fold, CSE, GVN, SROA, DSE, DCE, inline, unroll, TCO, alias-analysis, effect-reorder, partial-eval, vectorize, strength-reduce, devirt, jump-thread, loop-rotate, loop-vec, effect-opt, region-pool, reassociate, memoize. `--lto` enables aggressive cross-module inlining.
- **`#[test]` attribute** — parsed but not executed by `fidesc test` (tests are file-based)
- **C codegen coverage** — 343/343 normal tests match interpreter output (Nov 2). Phase 10B: Vec/HashMap/String now implemented in Fides stdlib using raw pointers. Binary hardening: stack canaries, PIE/ASLR, RELRO, NX stack.
- **214 compiler warnings** — dead code, unused variables (cleanup needed)

---

## Quick Start

### Using the self-hosted compiler

```bash
# Installed: the installer puts fid.exe (self-hosted) and fidesc.exe
# (Rust) on PATH, with the stdlib beside them.
fid run      examples/json_parser.fd
fid ccompile examples/json_parser.fd -o json_parser.exe   # needs a C compiler

# From the repository: build it in 3 stages (checks the fixpoint) into
# release/fid.exe
cargo build --release
RELEASE=1 CC="zig cc" ./scripts/bootstrap.sh
.\release\fid.exe run tests/bootstrap/hello.fd
```

The self-hosted compiler finds the stdlib through `$FID_HOME/stdlib`,
`<exe dir>/stdlib` or `<exe dir>/../stdlib`, so it runs from any folder.

### Using the Rust bootstrap compiler

```bash
# Build the Rust bootstrap compiler
cargo build --release

# Run / compile / inspect a Fides program
.\target\release\fidesc.exe run      tests/bootstrap/hello.fd
.\target\release\fidesc.exe ccompile tests/bootstrap/hello.fd -o hello.exe
.\target\release\fidesc.exe dump-c   tests/bootstrap/hello.fd > hello.c
.\target\release\fidesc.exe ir       tests/bootstrap/hello.fd
.\target\release\fidesc.exe fmt      tests/bootstrap/hello.fd
.\target\release\fidesc.exe lsp

# Rebuild the self-hosted compiler: 3 stages + fixpoint → release/fid.exe
RELEASE=1 CC="zig cc" ./scripts/bootstrap.sh
# or a single stage for development (dump-c + compile → release/fid.exe,
# same name, no fixpoint check)
./scripts/build-selfhost.sh

# Build a native ELF64 executable on Linux (zero external tools needed)
cargo run -- build tests/bootstrap/hello.fd -o hello

# Cross-compile to ARM64, RISC-V, or Windows
cargo run -- build tests/bootstrap/hello.fd --target arm64-linux -o hello_arm64
cargo run -- build tests/bootstrap/hello.fd --target riscv64-linux -o hello_riscv
cargo run -- build tests/bootstrap/hello.fd --target x86-64-windows -o hello.exe

# Run the test suite (Rust compiler tests)
cargo run -- test
```

### When to use which compiler

| Task | Use | Why |
|------|-----|-----|
| Writing and running Fides programs | `fid.exe` (self-hosted) or `fidesc.exe` | Both give the same output |
| Native Windows .exe with no C compiler | `fidesc.exe emit` | The self-hosted native backend is not working yet |
| Adding stdlib modules, tests | either; run `fidesc test` | The test harness is in the Rust compiler |
| Changing `compiler/*.fd` | `fidesc.exe` (Rust) to build stage 1 | Then `RELEASE=1 scripts/bootstrap.sh` to check the fixpoint |
| Adding new language features | `fidesc.exe` (Rust) first, then port to `compiler/*.fd` | New features need both compilers |

### CLI Commands

| Command | Description |
|---------|-------------|
| `build <file.fd> -o <out>` | `fid`: through C, needs a C compiler (`build --native`, the PIR path, crashes). `fidesc build --target x86-64\|arm64\|riscv64-linux\|x86-64-windows`: its own ELF64/PE path, tier 2–3. |
| `emit <file.fd> -o <out.exe> [--checked-alloc]` | **Native Windows executable**, no C compiler: IR → x86-64 machine code → PE. `--checked-alloc` puts every block on pages of its own (use after free and overruns fault; written Nov 4, not yet run on the suite). |
| `emit <file.fd> -o <out.bin> --bare` | Bare-metal flat binary (`fid` since Nov 6: loaded at 0x10000, print to COM1; not yet run under QEMU) |
| `ccompile <file.fd> -o <out>` | C codegen pipeline: lex → parse → typecheck → lower → C source → `gcc`/`clang` → executable |
| `run <file.fd> [--leaks] [--checked-alloc]` | Interpret the IR (no external tools). `--leaks` / `--checked-alloc` run the program through C with the debug allocator. |
| `check <file.fd>` | Type, borrow and escape checks only |
| `qualify <file.fd> [-o out.json]` | Safety evidence as JSON: compiler and source hashes, check result, annotated functions, each rule with its tests ([safety-rules.md](docs/reference/safety-rules.md)). Always "NOT_QUALIFIED". |
| `audit <file.fd> [--stdlib] [--strict]` | List the `unsafe` code a program reaches, with its `// SAFETY:` comments |
| `ir <file.fd>` | Lex → parse → typecheck → lower → print SSA IR |
| `pir <file.fd> [1\|2\|3\|all] [--opt]` | Lower to PIR (L1/L2/L3) and print; `--opt` runs optimization passes |
| `opt <file.fd> [pass1,pass2,...]` | Run PIR optimizer and print statistics (const-fold, cse, dce, inline, unroll, tco) |
| `codegen <file.fd> --target <arch> [--peephole] [--elf\|--macho\|--coff <o>]` | Generate assembly for x86-64, ARM64, or RISC-V; optionally emit object files |
| `wasm <file.fd> -o <out.wasm> [--wat]` | WebAssembly backend: PIR → WASM binary |
| `gpu <file.fd>` | SPIR-V GPU backend: PIR → SPIR-V binary |
| `tokens <file.fd>` | Lex only — print token stream |
| `ast <file.fd>` | Lex + parse — print AST tree |
| `test` | Run every test in `tests/bootstrap/` across interpreter, PE emitter, and C codegen (`--fail` for the expected-fail tests) |
| `repl` | Interactive REPL — evaluate Fides expressions |
| `verify <file.fd>` | Verify IR correctness (terminators, block refs, value refs) |
| `sourcemap <file.fd>` | Show source map (IR value → source line mapping) |
| `parse-header <file.h> -o <out.fd>` | Parse C header file → generate Fides FFI bindings |
| `watch <file.fd>` | `fidesc`: rebuild and rerun on change (native JIT on ELF64) |
| `link <files...>` | Link multiple object files (integrated linker) |
| `profile <file.fd>` | Per-function timing profiler (interpreter mode) |
| `fmt <file.fd>` | Format a Fides source file |
| `lint <file.fd>` | Lint checks (unused functions, unused imports) |
| `bench <file.fd> [-n <count>] [--ccompile]` | Benchmark a file (interpreter or C native) |
| `debug <file.fd>` | Interactive debugger (breakpoints, step, locals) |
| `replay <trace.fdtrace>` | Replay a recorded execution trace |
| `autopsy <file.fd>` | Native binary debug (ptrace-based, Linux only) |
| `version` | Print compiler version |

---

## Verification

The project includes automated verification scripts for ensuring correctness across all backends:

```bash
# Full verification: build + test + compare to baseline
./verify.sh

# Save current test results as new baseline (only after intentional changes)
./verify.sh --save

# Quick build check (no tests)
./verify.sh --quick

# Bisect which compiler layer broke a test — no script exists; do it manually:
./target/release/fidesc dump-ir  tests/bootstrap/<name>.fd
./target/release/fidesc dump-pir tests/bootstrap/<name>.fd 3
./target/release/fidesc codegen  tests/bootstrap/<name>.fd --target x86-64 --peephole
```

**Before making any change:** run `./verify.sh` to confirm you're starting from a known-good state. After changes, run `./verify.sh` again to check for regressions. If a test regresses, bisect manually with the `dump-*`/`codegen` commands above to identify which compiler layer (lowering, PIR, codegen, or peephole) is responsible.

Current baseline is stored in `tests/bootstrap/baseline/test_results.txt` —
`./verify.sh` prints the diff against it.

---

## How It Works

The compiler is a multi-stage pipeline:

```
Source (.fd) → Lexer → Parser → TypeChecker → Monomorphizer → Lowerer → SSA IR
                                                                         │
                                                    ┌────────────────────┼────────────────────┐
                                                    │                    │                    │
                                                    ▼                    ▼                    ▼
                                            ┌──────────────┐  ┌──────────────┐  ┌──────────────┐
                                            │  ELF64/PE    │  │  PIR L1/L2/L3│  │  C Codegen   │
                                            │  Emitter     │  │  → Codegen   │  │  codegen_c.rs│
                                            │  elf64.rs/   │  │  x64/arm64/  │  └──────────────┘
                                            │  pe.rs       │  │  riscv       │          │
                                            └──────────────┘  │  + peephole  │          ▼
                                                    │        └──────────────┘    C source (.c)
                                                    ▼                │                │
                                            Native ELF64/.exe       ▼                ▼
                                            (zero tools)    Assembly / Object    gcc/clang
                                                            (.s / .o)          │
                                                                                ▼
                                                                          Native executable
```

The **ELF64 emitter** (`build` command on Linux) is the primary native backend — it encodes x86_64 instructions directly to machine code bytes and constructs a valid ELF64 executable with zero external tools. **Cross-compilation** to ARM64 and RISC-V uses built-in instruction encoders (`arm64_enc.rs`, `riscv_enc.rs`) and runtime libraries (`runtime_arm64.rs`, `runtime_riscv.rs`) with raw Linux syscalls. The **PE emitter** (`emit` command or `--target x86-64-windows`) does the same for Windows.

### 0. Module Resolution & Monomorphization (`compiler.rs`, `mono.rs`)

Before lowering, the compiler:
- Auto-includes `stdlib/core.fd` (user definitions override stdlib)
- Resolves `use` imports by searching for `<path>.fd` or `<path>/mod.fd` files relative to the source
- Merges stdlib + imported modules + user code (user wins on conflicts)
- **Monomorphizes** generic functions: analyzes all call sites with concrete type arguments, generates specialized non-generic copies (e.g., `identity__i64`), and replaces calls

### 1. Lexer (`lexer.rs`)

Reads Fides source text and produces a stream of tokens. Handles:

- **Keywords**: `fn`, `val`, `const`, `mut`, `cap`, `type`, `return`, `if`, `else`, `match`, `struct`, `enum`, `impl`, `pub`, `use`, `async`, `await`, `comptime`, `defer`, `spawn`, `for`, `while`, `loop`, `break`, `continue`, `in`, `as`, `where`, `region`, `move`, `static`, `true`, `false`
- **Primitive types**: `i8`–`i128`, `u8`–`u128`, `f32`, `f64`, `bool`, `str`, `void`
- **Operators**: `+ - * / % == != < > <= >= && || ! & | ^ ~ << >> = += -= *= /= %= &= |= ^= <<= >>= -> => :: ? .. ..=`
- **Literals**: integers (hex `0xFF`, binary `0b1010`, octal `0o777`, underscores `1_000`), floats, strings, f-strings, identifiers
- **Comments**: `//` line comments, `/* */` block comments

### 2. Parser (`parser.rs`)

Recursive descent parser. Consumes the token stream and builds an AST. Handles:

- **Top-level declarations**: `fn` functions, `struct` definitions, `enum` definitions, `impl` blocks, `type` aliases, `const` constants, `use` imports, `#[attributes]`
- **Statements**: `val`/`var` bindings, `return`, `if`/`else`, `for`/`while`/`loop`, `break`/`continue`, `match`, `defer`, assignment + compound assignment, expression statements
- **Expressions**: literals, identifiers, binary/unary ops, function calls, method calls, struct literals, field access, array literals + indexing, references/derefs, closures, tuples, f-strings, ranges, block expressions
- **Types**: all primitives, named types, generic types (`Option<T>`), references (`&T`, `&mut T`), arrays (`[T; N]`), tuples, function types

### 3. Type Checker (`typechecker.rs`)

Validates the AST — ensures types are correct before lowering. Checks:

- Variable bindings have valid types (with inference)
- Function calls match declared parameter types
- Binary operations are between compatible types
- Struct literals have correct field names and types
- Field access is on valid struct types
- Return statements match function return type
- Pattern match arms are type-consistent
- Constants, type aliases, and generics are resolved

Reports all errors at once (does not stop on first error).

### 4. IR Lowering (`lower.rs`)

Lowers the type-checked AST to a typed **SSA (Static Single Assignment) IR** defined in `ir.rs`. Key features:

- **SSA values**: every value has a unique ID and single definition
- **Basic blocks**: control flow lowered to blocks with terminators (jump, branch, return)
- **Alloca + load/store**: mutable variables lowered via stack allocations
- **Function calls**: lowered to `Inst::Call` with argument passing
- **Runtime functions**: `print`, `__fides_to_str`, `__fides_str_concat`, `__fides_print_int`, `__fides_print_str`, `__fides_print_bool`, `__fides_panic`
- **F-string lowering**: literal parts become `ConstStr`, expression parts are converted via `__fides_to_str` (if not already a string), then all parts are concatenated via `__fides_str_concat`
- **Loop lowering**: `for` loops over ranges become `while`-style blocks with a loop variable alloca; `for` loops over arrays become index-based while loops with `GetElemPtr` + `Load`; `while` loops become condition + body + back-edge blocks
- **Alloca type tracking**: `HashMap<ValueId, IrType>` maps each alloca to its IR type, enabling correct `Load` types (not hardcoded `I64`)
- **Defer emission**: `defer_stack` tracks deferred statements; `emit_defers` drains and emits them before `Ret`
- **Match expression lowering**: `lower_match_expr` with result slot alloca, pattern matching branches (IntLit, StrLit, Range, EnumVariant, Wildcard, Ident), merge block
- **Method call lowering**: Name mangling (`__fides_StructName_method`), SRET for struct returns, receiver as first arg
- **Compound assignment**: Uses `get_value_type()` / `alloca_types` for correct `BinOp` type instead of hardcoded `I64`

### 5. IR Interpreter (`interp.rs`)

Executes the SSA IR directly — no external assembler or linker needed. Used by the `run` command. Supports:

- All IR instructions (arithmetic, comparisons, control flow, calls)
- Runtime function calls (`print`, `__fides_to_str`, `__fides_str_concat`, etc.)
- Value types: `Int`, `Float`, `Bool`, `Str`, `Ptr`, `Unit`
- Verbose mode (`-v`) for debugging

### 6. PE Emitter (`pe.rs`) — Direct Native Code Generation

The **PE emitter** is the primary native backend. It encodes x86_64 instructions directly to machine code bytes and constructs a valid PE (Portable Executable) file — no assembler, linker, or C compiler required.

**Pipeline:** `IrModule` → machine code bytes → PE file → `.exe`

**Capabilities:**
- **Full x86_64 instruction encoding**: REX prefixes, ModR/M + SIB bytes, displacement encoding (8-bit/32-bit), immediate encoding
- **Register allocation**: Stack-based alloca model — every IR value gets a stack slot at a fixed `rbp` offset; registers used as scratch within instructions
- **Windows x64 ABI**: RCX/RDX/R8/R9 for first 4 args, 32-byte shadow space, 16-byte stack alignment before calls, RAX for return values
- **PE file construction**: DOS header, PE header, section headers (.text + .rdata), import directory, IAT (Import Address Table) for `kernel32.dll` functions
- **Import resolution**: `GetStdHandle`, `WriteFile`, `ExitProcess` imported from `kernel32.dll` via IAT with RIP-relative addressing
- **Runtime stubs** (generated inline as machine code):
  - `p_print_int` — integer to string conversion (div/mod loop, reverse fill), `WriteFile` to stdout, newline
  - `p_print_str` — string + newline to stdout via `WriteFile`
  - `p_print_bool` — "true"/"false" to stdout
  - `p_print_float` — (stub, prints truncated int)
  - `p_panic` — prints message + `ExitProcess`
  - `p_malloc` / `p_free` — heap allocation (stubs)
- **Control flow**: Conditional jumps (`jcc`), unconditional jumps (`jmp`), label-based relocation with deferred resolution
- **Arithmetic**: Integer add/sub/mul/div/mod, float add/sub/mul/div, comparisons, bitwise and/or/xor/shl/shr
- **Stack frame management**: `push rbp` / `mov rbp, rsp` / `sub rsp, N` prologues, pre-allocated alloca slots at fixed offsets
- **Non-volatile registers**: `r12`/`r13` saved/restored in runtime stubs
- **Entry point**: Calls `main`, passes exit code to `ExitProcess`
- **Read-only data**: String literals stored in `.rdata` section, referenced via RIP-relative addressing

**Instruction encoders** (selected):
- `mov_rr`, `mov_mem_disp_reg`, `mov_mem_disp_reg32` (32-bit store), `mov_r_imm`, `mov_r8_imm`
- `add_rr`, `sub_rr`, `imul_rr`, `xor_rr`, `and_rr`, `or_rr`
- `cmp_rr`, `test_rr`, `jcc_label`, `call_label`, `call_reg`, `call_rip`
- `return`, `push`, `pop`, `lea_rbp`, `sub_rsp_i32`, `add_rsp_i32`
- `mov_mem_reg8` (byte store for digit conversion)

### 7. Cross-Compilation Backends (`arm64_enc.rs`, `runtime_arm64.rs`, `riscv_enc.rs`, `runtime_riscv.rs`)

The compiler supports cross-compilation to ARM64 and RISC-V targets with zero external tools. Each target has its own instruction encoder and runtime library:

**ARM64 (AArch64) — `--target arm64-linux`:**
- **Encoder** (`arm64_enc.rs`): Converts ARM64 assembly text to 4-byte machine code. Supports arithmetic (ADD, SUB, MUL, SDIV, UDIV), load/store (LDR, STR, LDRB, STRB), branches (B, BL, B.cond, RET), ADRP+ADD:lo12 for PC-relative addressing, SVC for syscalls. 17 unit tests.
- **Runtime** (`runtime_arm64.rs`): ARM64 Linux runtime using raw SVC syscalls (write=64, exit=93, mmap=222). Provides p_print_str, p_print_int, p_print_bool, p_panic, p_malloc, p_to_str, p_str_concat, and Vec operations. 9 unit tests.
- **ELF machine type**: EM_AARCH64 (183)

**RISC-V (RV64) — `--target riscv64-linux`:**
- **Encoder** (`riscv_enc.rs`): Converts RV64 assembly text to 4-byte machine code. Supports R-type, I-type, S-type, B-type, J-type, U-type instructions. Includes pseudo-instructions (li, mv, neg, not, seqz, snez, j, call, return, bgez, beqz, bnez, sext.w). 14 unit tests.
- **Runtime** (`runtime_riscv.rs`): RISC-V Linux runtime using raw ECALL syscalls (write=64, exit=93, mmap=222). Same function set as ARM64 runtime. 7 unit tests.
- **ELF machine type**: EM_RISCV (243)

**Windows (x86-64) — `--target x86-64-windows`:**
- Uses the existing PE emitter (`pe.rs`) which generates x86-64 Windows .exe files directly from IR with kernel32.dll imports. No additional runtime needed — PE emitter has its own inline runtime stubs.

**Target triple parsing** (`parse_target_triple()` in `main.rs`): Normalizes target strings to canonical names. Accepted formats: `x86-64`, `arm64`, `aarch64`, `riscv`, `riscv64`, `arm64-linux`, `aarch64-linux`, `riscv64-linux`, `x86-64-windows`, `x64-windows`, `windows`.

**Linker** (`linker.rs`): Target-aware ELF linker. `link_raw_with_runtime_for_target()` links user code + runtime for any target. Handles ARM64 relocations (BL, B, B.cond, ADRP, ADD:lo12) and RISC-V relocations (JAL, branch).

### 8. x86_64 MASM Code Generator (`x86.rs`)

Generates MASM-compatible x86_64 assembly from the SSA IR. Features:

- **Windows x64 calling convention**: RCX, RDX, R8, R9 for first 4 args; 32-byte shadow space; 16-byte stack alignment
- **Stack frame management**: `push rbp` / `mov rbp, rsp` / `sub rsp, N` prologues; pre-allocated alloca slots
- **Non-volatile register preservation**: `r12`/`r13` saved/restored in runtime stubs
- **Runtime stubs** (generated inline, no C runtime):
  - `__fides_print_int` — integer to string + `WriteFile` to stdout
  - `__fides_print_str` — string + newline to stdout
  - `__fides_print_bool` — "true"/"false" to stdout
  - `__fides_print_float` — (stub, prints truncated int)
  - `__fides_to_str` — integer to null-terminated string in stack buffer
  - `__fides_str_concat` — concatenates two strings into static buffer (with temp buffer to avoid aliasing)
  - `__fides_panic` — prints message + `ExitProcess`
- **Static data**: string literals, concatenation buffers (`str_concat_buf`, `str_concat_tmp`) in `.data` section
- **External imports**: `GetStdHandle`, `WriteFile`, `ExitProcess` from `kernel32.lib`
- **Entry point**: `main` / `_main` (both exported)

### 9. C Code Generator (`codegen_c.rs`)

Translates SSA IR to portable C99 code. Used by the `ccompile` command. Features:

- **Portable C output**: Generates `.c` files compilable with MSVC (`cl.exe`), GCC, or Clang
- **Function renaming**: `main` → `main_entry`, `abs`/`max`/`min` → `p_abs`/`p_max`/`p_min` (avoids C/MSVC conflicts)
- **`#undef` directives**: Removes MSVC `max`/`min`/`abs` macros that conflict with Fides functions
- **SSA variable declarations**: All values declared once at function start; instructions are assignments
- **Polymorphic `print`**: Dispatches to `p_print_str`/`p_print_int`/`p_print_bool`/`p_print_float` + `p_print_newline()` based on argument type
- **Runtime functions**: `p_print*`, `p_panic`, `p_malloc`, `p_free`, `p_str_concat`, `p_to_str`, Vec/HashMap/String operations
- **Control flow**: Basic blocks as C labels with `goto` statements
- **MSVC integration**: `find_cl()` and `find_vcvarsall()` locate and set up the MSVC compiler environment

### 10. Compiler Pipeline (`compiler.rs`)

Orchestrates the stages. Entry points:

- `emit_source_with_path(source, path)` — Direct PE emission: lex → parse → typecheck → monomorphize → lower → PE bytes
- `build_executable(source, path)` — Native build pipeline: lex → parse → typecheck → monomorphize → lower → PIR (L1→L2→L3) → codegen → peephole → ELF64/PE executable. Supports cross-compilation via `--target arm64`, `--target riscv`, `--target x86-64-windows`.
- `ccompile_source_with_path(source, path)` — C codegen pipeline: lex → parse → typecheck → monomorphize → lower → C source string
- `interpret_source_with_path(source, path, verbose)` — Lex → parse → typecheck → monomorphize → lower → interpret IR
- `lower_to_ir(source)` — Lex → parse → typecheck → monomorphize → lower → formatted IR string
- `lex(source)` — Lex only, returns formatted token list
- `parse_to_ast(source)` — Lex + parse, returns pretty-printed AST
- `compile(source)` — Legacy ARM64 pipeline (unused for x86/PE/C path)

---

## Language Features

### Currently Supported (End-to-End: parse → lower → codegen → run)

| Feature | Syntax | Example |
|---------|--------|---------|
| Functions | `fn` | `fn add(a: i32, b: i32) -> i32 { return a + b; }` |
| Variables | `val` / `var` | `val x = 42;` / `var y = 0;` |
| Constants | `const` | `const MAX: i32 = 100;` |
| If/Else | `if` / `else` | `if x > 0 { ... } else { ... }` |
| For loops | `for x in start..end` | `for i in 0..5 { sum += i; }` |
| While loops | `while cond` | `while counter < 5 { ... }` |
| Return | `return` | `return 42;` |
| Arithmetic | `+ - * / %` | `a + b * c` |
| Comparison | `== != < > <= >=` | `if x >= 10 { ... }` |
| Logical | `&& \|\| !` | `if a && !b { ... }` |
| Bitwise | `& \| ^ ~ << >>` | `0xFF & 0x0F` |
| Compound assign | `+= -= *= /= %=` | `x += 10;` |
| Function calls | `name(args)` | `add(10, 20)` |
| Recursive calls | self-referential `fn` | `fn fact(n: i32) -> i32 { ... fact(n-1) ... }` |
| Print | `print(...)` | `print("Hello, Fides!");` |
| String literals | `"..."` | `print("Hello");` |
| F-strings | `f"{expr}"` | `f"Hello, {name}!"` |
| F-string with int | `f"x = {x}"` | `print(f"x = {x}");` |
| Hex/binary/octal | `0xFF` `0b1010` `0o777` | `val y = 0xFF & 0x0F;` |
| Type annotations | `: type` | `val x: i32 = 42;` |
| Semicolons | `;` | Statement terminator |
| Comments | `//` `/* */` | `// line comment` |
| Structs | `struct Point { x: i32, y: i32 }` | `struct Point { x: i64, y: i64 }` |
| Struct literals | `Point { x: 1, y: 2 }` | `val p = Point { x: 1, y: 2 };` |
| Field access | `point.x` | `print(p.x);` |
| Struct return (SRET) | `fn make_point() -> Point { ... }` | `val p = make_point(); print(p.x);` |

### Lowered (AST → SSA IR → Interpreter + PE Emitter)

| Feature | Syntax |
|---------|--------|
| Struct methods | `impl Point { fn get_x(self: Point) -> i32 { ... } }` |
| Pattern matching | `match val { 1 => { ... } _ => { ... } }` |
| Match patterns (int, str, range, enum) | `match s { "foo" => { ... } 1..=10 => { ... } Shape::Circle(r) => { ... } }` |
| Enums with data | `enum Shape { Circle(f64), Rectangle(f64, f64), Unit }` |
| Enum variant construction | `Shape::Circle(3.14)` |
| References | `&x`, `&mut x`, `*x` |
| Closures (lambda lifting) | `fn(x: i32) => x * 2` |
| `?` operator | `expr?` |
| Tuples | `(i32, bool)`, `(42, true)` |
| Array literals | `[1, 2, 3, 4, 5]` |
| Array indexing + assignment | `arr[0]`, `arr[0] = 10`, `arr[2] += 100` |
| `for` loop over arrays | `for x in arr { sum += x; }` |
| `defer` blocks | `defer close(file);` |
| Block expressions | `{ val a = 10; val b = 20; a + b }` |
| Linear types (`@linear`) | `@linear fn transfer(...) { ... }` — move semantics, no duplication |
| `@conserve` | `@conserve fn settle(...) { ... }` — compile-time financial conservation proof |
| Tuple destructuring | `val (a, b) = expr` |
| `txn {}` blocks | `txn { transfer(a, b, 100); }` — atomic transactions with rollback |
| `@gas` | `@gas(1000) fn foo() { ... }` — compile-time gas budgets |
| `@crashfree` | `@crashfree fn foo() { ... }` — compile-time proof of no panics |
| `@audit` | `@audit fn foo() { ... }` — automatic function audit logging |
| Loop-aware linear types | Linear vars consumed in loops → compile error |

### Implemented (Lexer + Parser + Typechecker + Lowering + Codegen)

| Feature | Syntax | Notes |
|---------|--------|-------|
| `async` / `await` | `async fn` / `await <expr>` | ✅ Works (Phase 7.6) — Future<T> type, eager evaluation, `p_future_ready`/`p_future_poll` runtime |
| `spawn` | `spawn <expr>` | ✅ Works (Phase 7.6/8A) — creates OS threads via `p_thread_spawn` |
| `@derive` | `@derive(Debug/Clone/Serialize/Deserialize)` | ✅ Works (Phase 7.7) — generates debug_str/clone/to_json/from_json methods |
| `#[test]` attribute | `#[test]` | Parsed but not executed by `fidesc test` (tests are file-based) |

### Planned (Not Yet Implemented)

| Feature | Syntax | Phase |
|---------|--------|-------|
| Default arguments | `fn f(x: i32 = 0)` | Future |
| Variadic arguments | `fn f(...args)` | Future |
| Self-hosting | (rewrite compiler in Fides) | Phase 6 |

---

## Fides Syntax Overview

Fides is a systems language with unique keywords designed for clarity and AI-friendliness:

- **Compile-time financial conservation** — Bugs that cost the real world billions (Knight Capital: $440M in 45 minutes from double-processing) are compile errors in Fides. The `@conserve` annotation proves money-in equals money-out at compile time, and linear types (`@linear`) prevent double-processing of financial values.

```p
// Function declaration
fn main() {
    val x = 42;
    var y = 0;
    y = x + 8;
    print(y);
}

// Struct with methods
struct Point {
    x: f64,
    y: f64,
}

impl Point {
    fn new(x: f64, y: f64) -> Point {
        Point { x: x, y: y }
    }

    fn distance(&self, other: &Point) -> f64 {
        val dx = self.x - other.x;
        val dy = self.y - other.y;
        (dx * dx + dy * dy).sqrt()
    }
}

// Algebraic enum
enum Shape {
    Circle { radius: f64 },
    Rectangle { width: f64, height: f64 },
    Triangle(f64, f64, f64),
}

// Constant
const MAX_PLAYERS: i32 = 64;
```

### Fides Keyword Reference

| Keyword | Meaning | Replaces (Rust) |
|---------|---------|-----------------|
| `fn` | Fides Function (all functions) | `fn` |
| `val` / `var` | Variable (immutable / mutable) | `let` / `let mut` |
| `const` | Constant | `const` |
| `cap` | Capability / Trait | `trait` |
| `return` | Return | `return` |
| `type` | Type alias | `type` |
| `comptime` | Compile-time code (no macros) | N/A (replaces macros) |
| `defer` | Deferred cleanup | N/A (Go-inspired) |
| `spawn` | Task spawning | N/A |
| `region` | Explicit memory region | `'a` lifetimes |

See `docs/reference/syntax.md` for the complete syntax specification.

---

## Project Structure

```
p-lang/
├── compiler/               # 63 self-hosted Fides modules (~142K lines) — THE COMPILER
│   ├── main.fd             # CLI entry point (command dispatch)
│   ├── lexer.fd / parser.fd / ast.fd / token.fd   # Front end
│   ├── typechecker.fd      # Type checking + capability + effect + region + sandbox
│   ├── borrowck.fd         # Borrow checker (NLL, field-wise, two-phase)
│   ├── escape.fd           # Reference escape analysis (session 29)
│   ├── mono.fd             # Generic monomorphization
│   ├── ir.fd / lower.fd    # SSA IR + AST → IR lowering
│   ├── pir.fd / opt.fd / peephole.fd / regalloc.fd  # Middle end
│   ├── codegen_c.fd        # C codegen backend (IR -> .c)
│   ├── codegen_x64.fd / codegen_arm64.fd / codegen_riscv.fd / x86.fd  # asm backends
│   ├── elf64.fd / pe.fd / coff.fd / macho.fd / linker.fd  # object/executable writers
│   ├── runtime.fd / runtime_linux.fd / runtime_arm64.fd / runtime_riscv.fd
│   ├── arm64_enc.fd / riscv_enc.fd  # instruction encoders
│   ├── comptime_eval.fd / flat_ast.fd / inc.fd   # compile-time + caching
│   ├── diag.fd / fmt.fd / json.fd / lsp.fd       # tooling
│   ├── scribe.fd / veil.fd / ark.fd              # doc gen / coverage / pkg manager
│   ├── wasm.fd / gpu.fd / tensor.fd / autograd.fd # wasm / GPU / ML
│   ├── embedded.fd / green_thread.fd / debugger.fd / native_debugger.fd / hotreload.fd
│   └── cparser.fd          # C header parser → FFI bindings
├── src/                    # Rust bootstrap reference (57 files, ~112K lines)
│   ├── main.rs             # CLI entry point (used to generate first-generation C)
│   ├── escape.rs           # Reference escape analysis (Rust twin of escape.fd)
│   ├── dwarf.rs            # DWARF debug info (started)
│   ├── ...                 # Mirrors compiler/*.fd in Rust (reference implementation)
│   └── bin/                # Binary entry points (fidesc, ark, scribe, veil)
├── stdlib/                 # 136 standard library modules (~43K lines)
│   ├── core.fd             # Option, Result, println, assert, max, min, abs, clamp
│   ├── vec.fd              # Vec<T> with iterator adapters
│   ├── rc.fd               # Rc<T> / Arc<T> reference counting
│   └── ...                 # 101 more modules (math, banking, medical, tensor, game/*)
│                           #   kernel.fd + uefi.fd moved to the fides-os project
├── tests/                  # 348 .fd test files (302 bootstrap + 46 selfhost) + harness
├── examples/               # 10 example programs
├── release/                # Self-hosted compiler binaries + generated C (gitignored)
│   └── fid.exe             # Self-hosted compiler (RELEASE=1 scripts/bootstrap.sh: 3 stages,
│                           #   fixpoint; scripts/build-selfhost.sh: 1 stage)
└── docs/                   # Documentation (reference, guides, meta, roadmap, selfhost)
```

---

## Test Programs

348 test files across `tests/bootstrap/` and `tests/selfhost/`. Key tests:

| Test File | What It Tests |
|-----------|---------------|
| `hello.fd` | Hello world — `fn`, `val`, `if/else`, `print` |
| `arithmetic.fd` | Arithmetic, nested function calls |
| `struct.fd` | Struct definition, literals, field access |
| `control_flow.fd` | `if`/`else if`/`else`, `for` ranges, compound assignment |
| `functions.fd` | User-defined functions, recursive factorial |
| `strings.fd` | String literals, f-string interpolation |
| `test_float.fd` | Float arithmetic and printing |
| `test_fstring_*.fd` | F-string with int, float, vec |
| `test_vec_*.fd` | Vec creation, push, loop, printing |
| `test_hashmap.fd` | HashMap insert, get, contains |
| `test_match.fd` | Pattern matching (int, string, enum, wildcard) |
| `test_match_guard.fd` | Match guards (`pattern if cond =>`) |
| `test_match_range.fd` | Match range patterns (`1..=5 =>`) |
| `test_option.fd` | Option type (Some/None, unwrap) |
| `test_try.fd` | `?` operator and `try { }` blocks |
| `test_defer.fd` | `defer` blocks (LIFO cleanup order) |
| `test_closure.fd` | Closures with environment capture |
| `test_impl.fd` | `impl` blocks, struct methods |
| `test_generics.fd` | Generic functions, monomorphization |
| `test_traits.fd` | `cap` traits, `impl Cap for Type` |
| `test_trait_assoc.fd` | Trait associated types |
| `test_trait_default.fd` | Trait default methods |
| `test_trait_object.fd` | Trait objects (`dyn Trait`) |
| `test_type_alias.fd` | Type aliases (`type X = T`) |
| `test_asm.fd` | Inline assembly (`asm!`) |
| `test_bare.fd` | Bare-metal mode (no stdlib, `asm!("hlt")`) |
| `test_capabilities.fd` | Capability grants, denial, transitive |
| `tests/bootstrap/capability/test_cap_*.fd` | Capability security: grant/deny/propagation (Phase 2C) |
| `tests/bootstrap/safety/test_*.fd` | Bounds safety: OOB, overflow, div-by-zero, unwrap None (Phase 2D) |
| `tests/bootstrap/effects/test_effect_*.fd` | Effect system: annotations, inference, checking, handlers (Phase 2B) |
| `tests/bootstrap/regions/test_region_*.fd` | Region inference: region blocks, escape detection, safety (Phase 2A) |
| `test_refs_mut.fd` | `&mut` references, dereference |
| `test_pe_*.fd` | PE emitter specific tests (struct, enum, refs, generics, defer, try) |
| `test_regression_*.fd` | Regression tests (closures, fstrings, collections) |
| `test_money_*.fd` | Money stdlib: fixed-point arithmetic, linear type enforcement (`test_money_linear_fail`) |
| `test_money_conserve*.fd` | `@conserve` conservation proof (`test_money_conserve_fail`) |
| `test_banking_*.fd` | Banking stdlib: accounts, transfers, double-credit/lost-money detection |
| `test_loop_linear*.fd` | Loop-aware linear types (`test_loop_linear_fail`) |
| `test_gas*.fd` | `@gas` compile-time gas budgets (`test_gas_fail`) |
| `test_crashfree*.fd` | `@crashfree` compile-time panic prevention (`test_crashfree_fail`) |
| `test_tuple_destructure.fd` | Tuple destructuring (`val (a, b) = expr`) |
| `test_file.fd` | File I/O (`stdlib/file.fd`) |
| `test_txn.fd` | `txn {}` atomic transaction blocks |
| `test_time*.fd` | LogicalTime stdlib: construction, extraction, arithmetic, formatting, leap years |
| `test_typed_strings*.fd` | F67: SQLString, ShellString, HTMLString, JSONString, PathString — escape, bind, is_safe |
| `test_init_guarantee*.fd` | F63: `var` keyword (without initializer), definite assignment analysis, init guarantee |
| `test_rc_arc.fd` | Rc/Arc smart pointers (new, clone, drop, get, count) |
| `test_recursive_box.fd` | Recursive types via `Box<T>` (enum Expr with Box children) |
| `test_enum_payload_types.fd` | Typed enum payloads (variant types preserved in match) |
| `test_generic_vec_typed.fd` | Generic Vec<T> with typed elements |
| `test_vec_math.fd` | Vec2/3/4 + mat4 + quat math types |
| `test_constant_time*.fd` | `@constant_time` security annotation (pass + fail tests) |

Run all tests: `./target/release/fidesc test` (or `cargo test --test harness`)

---

## Design Documents

- `docs/reference/syntax.md` — Complete Fides syntax specification (current `fn`/`val`/`const` syntax + proposed `fn`/`val`/`var` migration)
- `docs/reference/architecture.md` — Compiler architecture, pipeline stages, and internal design
- `docs/roadmap/roadmap.md` — Full 8-phase roadmap (currently at Phase 8 in progress)
- `docs/roadmap/roadmap.md` § Limitations — Verified gap analysis (claimed vs actual)
- `docs/reference/vision.md` — Language vision and 8 pillars (Originality, Security, Performance, etc.)
- `docs/reference/vision.md` — Vision, design principles, decisions, future direction
- `docs/reference/changelog.md` — Version history and detailed change log
- `docs/reference/features.md` — Feature list and status
- `docs/reference/positioning.md` — Competitive analysis (vs Rust/Zig/Go/Swift) + adoption strategy
- `docs/reference/lessons.md` — PL history lessons + IBM AIX/z/OS lessons
- `docs/reference/architecture.md` — Custom toolchain rationale (§0) + full pipeline reference
- `docs/reference/vision.md` § Future — Innovation (AI/robotics) + community goals
- `docs/reference/best-practices.md` — Best practices guide
- `docs/reference/borrowchecker.md` — Borrow checker documentation
- `docs/roadmap/roadmap.md` § Opportunities — AI/ML + industry opportunities
- `docs/reference/testing.md` — Test results across all backends + test batch specifications
- `docs/reference/tool.md` — Tooling guide

---

## Roadmap

See `docs/roadmap/roadmap.md` for the full phase status and milestones.
See `docs/roadmap/roadmap.md` § Self-Host Details for the self-hosting plan.

**Self-hosting status:** Bootstrap fixpoint verified (stage 2 == stage 3 byte-for-byte, checked on every release build; last verified Nov 6, 2026). 61 self-hosted modules (~137K lines) in `compiler/*.fd`. The self-hosted compiler is the release compiler (`fid.exe`) and defines the language; the Rust compiler (`fidesc.exe`) is frozen.
