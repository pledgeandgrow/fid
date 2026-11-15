# Fides status (detailed)

> Moved from the README on Nov 15, 2026; the README keeps a short summary.
> Counts: [selfhost.md → Numbers](selfhost.md#numbers). Open bugs:
> [known-bugs.md](known-bugs.md) §2f, §2g.

**Self-Hosting — Bootstrap fixpoint achieved and verified.** The Fides compiler compiles itself to byte-identical C across generations (stage 2 == stage 3, 49 MB of C, verified Nov 6, 2026). `RELEASE=1 ./scripts/bootstrap.sh` checks this on every release build.

**68 self-hosted modules** (~159K lines) in `compiler/*.fd` — lexer, parser, type checker, borrow checker, escape analyzer, monomorphizer, IR lowering, IR and PIR optimizers, C codegen, native PE emitter, x86-64/ARM64/RISC-V codegen, register allocator, peephole optimizer, comptime evaluator, LSP server, formatter, diagnostics, JSON, incremental compilation, ELF/COFF/Mach-O writers, per-arch runtimes, and the Ark package manager.

**The language is what `fid` accepts.** The rules, each with the compiler function that enforces it, are in the [language reference](reference.md). The Rust compiler (`fidesc`) is frozen and checks less: [the differences](language-guide.md).

**Tests.** The last full suite run is from Nov 2 (interpreter 339/339, C 343/343, native Windows 345/345, expected-fail 89/89), **with `fidesc`, the Rust compiler, as the compiler under test**: the shipped `fid` has not run the full suite. The compiler of Nov 4–6 was checked on the new and the key test programs on all three backends, and every new expected-fail test is rejected; the full suite has not been rerun with it.

**New in `fid` (Nov 7–9):** the machine layer (`compiler/nat_ir.fd`: one machine IR and register allocator for Linux AArch64/RISC-V/x86-64 and opt-in Windows x64), a rule- and e-graph-based mid-end with an IR verifier and z3-proved rules, ownership layouts of proposal 0004 (counted strings and Boxes, structs inline in a `Vec`), `Vec::filled`, the compiler built from split C at -O2 (self-compile 74 s), and language facts used by the optimizer ([optimizer-facts.md](optimizer-facts.md)). [Changelog](changelog.md).

**New in `fid` (Nov 4–6):** generics with inference, bounds and turbofish for functions, enums and `impl<T>` ([0002](../proposals/0002-real-generics.md); a literal of a user-defined generic struct is still rejected); `if`/`match` as values; `@cfg`; freeing of locals the compiler proves finished; iterator-invalidation and field-borrow checks; `@send` on spawned arguments; `parallel {}` branches on threads; `async fn` with float and struct arguments; every safety annotation enforced with a negative test per rule ([safety-rules.md](safety-rules.md)) and `fid qualify` evidence; owned and borrowed strings (`&str`, `@scoped`, `str_clone`); integer-key `HashMap`s; per-call-site effects of higher-order functions.

**Limits:**
- `fid ccompile` goes through C, so it needs a C compiler (`$CC`, gcc, clang, `zig cc` or tcc). `fid build` is the same command as `fid emit`: on a Windows host it writes a native PE with no C compiler; on other hosts, without `--target`, it goes through C. `build --native` with the default target goes to the old PIR path and crashes (opt.fd `pass_jump_thread`).
- Native output without a C compiler: Windows x64 (`fid emit`; opt-in on the new machine layer with `--nat`: 301 of 338 suite tests, Nov 9), and Linux AArch64, RISC-V 64 and x86-64 (`fid emit --target aarch64-linux|riscv64-linux|x86_64-linux`: 273, 271 and 272 of 317 suite tests match the interpreter under QEMU, Nov 9; the runtime has threads, M:N tasks, futexes and signals on system calls). Other systems go through C: `fid ccompile --target aarch64-linux` (also `riscv64-linux`, `x86_64-linux`, `x86_64-macos`, `aarch64-macos`, `wasm32-wasi`) cross-compiles with `zig cc` from any host. Linux ARM64 matched the interpreter on 270 of 286 tests under QEMU; RISC-V and WASI run sample programs; macOS builds but has not been run. See [platforms](platforms.md).
- Native `async`/`spawn` + `await` under `fid emit` were fixed on Nov 6 (the worker threads were started through a name lookup that returned 0) and broke again in builds of Nov 9 afternoon (three iropt passes on by default; fixed again in the evening's fid31, which matches the interpreter on pe_ir and `--nat`; known-bugs 2f.2). The interpreters (`fid run`, `fidesc run`) run an `async fn` to the end when it is called instead of until it first waits, so a program that interleaves tasks prints in a different order there (`test_async_suspend`).
- Value semantics and ownership: since Nov 7 one ownership rule ([0004](../proposals/0004-ownership-and-layout.md), `LOWER_RC` on): a variable owns its value and is dropped at the end of its scope, keeping a value takes an owner, parameters are borrowed. Strings, `Vec` buffers and Boxes are counted; copy-on-write (`LOWER_COW`, [0001](../proposals/0001-value-semantics.md)) is off. The full suite has not run with it; the leak gate (`scripts/leak-check.sh`) passes.
- Some compile-time checks are weaker than their names (checked by running, Nov 6): `sandbox {}` does not see IO done by the functions it calls, `region ... with budget(N)` counts declared locals but not heap growth, `@conserve` only forbids `Money_new`, `@crashfree` does not bound recursion, and `txn {}` rolls nothing back on C or native. The list, with what does hold: [positioning.md](positioning.md) (top).
- Speed: fib(40) takes 1.34 s native and 0.81 s through C, against 0.53 s for Rust (Nov 2, [scoreboard](../roadmap/competitive-roadmap.md#the-scoreboard)). `fid` compiles its own ~159K lines to C in 74 s when built at -O2 (Nov 9; 229 s at -O0); `fidesc` takes ~21 s.
- Fides is not suitable for safety-critical use: `fid qualify` gives evidence, not a certification.

**Numbers:** [selfhost.md → Numbers](selfhost.md#numbers) is the one place counts and measurements are kept.

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
| Borrow checker | `--borrowck=on` (default) / `off` | Compile-time aliasing analysis — `&mut` exclusivity, no mutation while borrowed. `--borrowck=warn` prints nothing in `fid` (Nov 9: the warnings are never read) |

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
| `async` / `await` | ⚠️ Works through C; native (`fid emit`) worked Nov 6, broke in Nov 9 afternoon builds, fixed again that evening | An `async fn` is a task that runs until it first waits; `await` suspends it. Float and struct arguments and results work (Nov 4). The interpreters run an async body to the end at the call. |
| `spawn` | ✅ Works | `spawn f(args)` runs `f` on an OS thread; every argument must be `@send` (references and raw pointers are not). `spawn { block }` runs the block in place. |
| `#[test]` attribute | Parsed | Not yet executed by `fidesc test` |
| `@derive` | ✅ Works | Phase 7.7 — `@derive(Debug/Clone/Serialize/Deserialize)` generates methods |
| Effect handler continuations | Syntax works | `handle { do } with` parses and runs; full continuation semantics pending |
| `--opt` (L2 optimizer) | ✅ Works | 23 passes, enabled by default in `build` (use `--no-opt` to disable). `--lto` for aggressive inlining. |
| Default and named arguments | ✅ Works | `fn f(x: i64, y: i64 = 2)`, `f(1, y: 3)` |
| Variadic arguments | Parsed | `...` parses and does nothing |

### Known Limitations

- **ELF64 executables on Linux: `fidesc` only, tier 2** — `fidesc build <file> --target x86-64` produces ELF64 executables directly and works for the programs tried, but is not in CI; `fid` has no ELF backend yet. `fidesc codegen --elf <out.o>` emits relocatable objects; `--macho` and `--coff` emit object files (no macOS executables yet).
- **ARM64 and RISC-V native: tier 3** — `fidesc build --target arm64-linux` / `--target riscv64-linux` run the built-in encoders (`arm64_enc.rs`, `riscv_enc.rs`), but the runtimes (`runtime_arm64.rs`, `runtime_riscv.rs`) still have stubs that do nothing. Use the C backend with a cross-compiler for these targets. See [platforms](platforms.md).
- **PE emitter targets Windows x64 only** — `fid emit`, `fidesc emit` or `fidesc build --target x86-64-windows` produce PE32+ executables; `fidesc`'s emitter (src/pe.rs) matched the interpreter on 345/345 (Nov 2); `fid emit` (compiler/pe_ir*.fd) has not run the full suite
- **Register allocator** — Linear scan allocator with constant rematerialization (F-1A). Effect-aware allocation (F-1D): pure/@no-alloc calls only clobber RAX. Works correctly for all passing tests.
- **Peephole optimizer (`fidesc build`; in `fid` only on the PIR commands `codegen --peephole`, `emit --boot`, `build --native`)** — 9 passes: redundant moves, zero idiom, nop arithmetic, branch threading, dead store, redundant test, store-to-load forwarding, LEA coalescing, cmp+setcc+jcc instruction selection
- **L2 optimizer** — `fidesc build` runs the PIR optimizer by default (use `--no-opt` to disable). In `fid` it is reached only by `build --native` (default target), `opt` and `pir --opt`, and it panics there (Nov 9); `fid`'s native paths use the IR optimizer compiler/iropt.fd instead. Passes: const-fold, CSE, GVN, SROA, DSE, DCE, inline, unroll, TCO, alias-analysis, effect-reorder, partial-eval, vectorize, strength-reduce, devirt, jump-thread, loop-rotate, loop-vec, effect-opt, region-pool, reassociate, memoize. `--lto` enables aggressive cross-module inlining.
- **`#[test]` attribute** — parsed but not executed by `fidesc test` (tests are file-based)
- **C codegen coverage** — 343/343 normal tests match interpreter output (Nov 2). Phase 10B: Vec/HashMap/String now implemented in Fides stdlib using raw pointers. Binary hardening: stack canaries, PIE/ASLR, RELRO, NX stack.
- **214 compiler warnings** — dead code, unused variables (cleanup needed)

---
