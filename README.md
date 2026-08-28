<h1 align="center">Fides Language — Bootstrap Compiler</h1>

A bootstrap compiler for the Fides programming language, written in Rust. Generates **native ELF64 executables on Linux** (x86-64, ARM64, RISC-V) and **PE executables on Windows** via zero-external-tool cross-compilation, plus portable C codegen. Also includes an IR interpreter for rapid testing, WebAssembly and SPIR-V GPU backends, multi-arch assembly codegen (x86-64, ARM64, RISC-V), ELF64/Mach-O/COFF object file emission, and a bare-metal flat binary mode.

## Status

**Phase 1: Foundation — Complete.** **Phase 2: Safety — Complete.** **Phase 3: Power — Complete (interpreter).** **Phase 4: Performance — Mostly complete.** **Phase 5: Platform — Mostly complete.** **Cross-Compilation — Complete (x86-64, ARM64, RISC-V, Windows).** The compiler lexes, parses, type-checks, lowers to SSA IR, and generates native executables. The **ELF64 emitter** (`build --target x86-64` on Linux) produces working executables directly — no assembler, linker, or C compiler required. **Cross-compilation** to ARM64 (`--target arm64-linux`), RISC-V (`--target riscv64-linux`), and Windows (`--target x86-64-windows`) also works with zero external tools. Phase 2 adds region inference, effect system, capability security (with compile-time elision), and bounds safety. The optimizer is enabled by default in `build` (use `--no-opt` to disable) with 23 passes; `--lto` enables aggressive cross-module inlining.

**Test results:** 153/153 normal tests pass on all 3 backends (interpreter, C codegen, PE emitter). 265/265 unit tests. 0 skip, 0 known-fail. See `AGENTS.md` → "Current Baseline" for authoritative counts.

**Phase status:** See `docs/roadmap/roadmap.md` for full phase status and what's left. See `docs/roadmap/selfhost-roadmap.md` for the self-hosting plan.

### Financial Safety Features (New)

| Feature | Syntax | Description |
|---------|--------|-------------|
| Linear types | `@linear` | Move semantics — no duplication of linear values |
| Conservation proof | `@conserve` | Compile-time financial conservation proof (money in = money out) |
| Money stdlib | `stdlib/money.fd` | Fixed-point money type, arithmetic, formatting |
| Banking stdlib | `stdlib/banking.fd` | Accounts, transfers, double-credit/lost-money detection |
| Tuple destructuring | `val (a, b) = expr` | Decompose tuple values into bindings |
| File I/O | `stdlib/file.fd` | File descriptors, read/write via `stdlib/file.fd` |
| Atomic transactions | `txn { ... }` | Atomic transaction blocks with rollback on failure |
| Gas budgets | `@gas(N)` | Compile-time gas budget annotation |
| Crash-free proof | `@crashfree` | Compile-time proof that a function cannot panic |
| Audit logging | `@audit` | Automatic function audit logging with parameter values |
| Loop-aware linear types | — | Linear variables cannot be consumed inside loops (compile error) |
| LogicalTime | `stdlib/time.fd` | Timezone-free, clock-free deterministic date math |
| Typed strings | `stdlib/typed_strings.fd` | SQLString, ShellString, HTMLString, JSONString, PathString — injection prevention via type system |
| Init guarantee | `var` keyword (without initializer) | Uninitialized mutable variables + definite assignment analysis (compile error on read before assignment) |
| Raw pointer types | `*mut T`, `*const T` | Raw pointers with `unsafe {}` enforcement (no borrow checking) |
| Memoization | `stdlib/memo.fd` | Simple cache helper: `Memo::new()`, `get_or_compute()` |
| Checked arithmetic | F64 | Integer overflow = panic (always on, not just debug mode) |
| `@wrapping` | `@wrapping fn foo() { ... }` | Escape hatch for wrap-around arithmetic (hashing, crypto) |
| Bounds checking lock | F62 | Bounds checks always on, skippable in `unsafe {}` blocks |
| Parameterized queries | F68 | No string concat on typed strings (SQLString + str = compile error) |
| `@medical` | `@medical @audit fn foo() { ... }` | IEC 62304: no dynamic alloc, no recursion, no while, no unwrap, must be @audit |
| `given`/`ensure` contracts | `fn foo(x: i64) -> i64 given x >= 0 { ... }` | Design by Contract — preconditions verified at call sites |
| `async`/`await`/`spawn` | `async fn foo() -> Future<T> { ... }` | Cooperative concurrency — async functions, await, spawn (Phase 7.6) |
| Borrow checker | `--borrowck=on` / `--borrowck=warn` | Compile-time aliasing analysis — `&mut` exclusivity, no mutation while borrowed |

### What Works (Verified End-to-End)

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
| Bare-metal mode (`emit --bare`) | N/A | Yes (flat binary) | N/A |
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
| `@conserve` conservation proof | Compile-time | Compile-time | Compile-time |
| Tuple destructuring (`val (a, b) = expr`) | Yes | Yes | Yes |
| `txn {}` atomic transactions | Yes | Yes | N/A |
| `@gas` gas budgets | Compile-time | Compile-time | Compile-time |
| `@crashfree` panic prevention | Compile-time | Compile-time | Compile-time |
| `@audit` audit logging | Yes | Yes | Yes |
| Loop-aware linear types | Compile-time | Compile-time | Compile-time |
| LogicalTime (`stdlib/time.fd`) | Yes | Yes | Yes |
| Typed strings (`stdlib/typed_strings.fd`) | Yes | Yes | Yes |
| Init guarantee (`var` keyword (without initializer)) | Yes | Yes | Yes |
| WebAssembly backend (`fidesc wasm`) | N/A | N/A | N/A |
| SPIR-V GPU backend (`fidesc gpu`) | N/A | N/A | N/A |
| Hot reload (`fidesc watch`) | N/A | N/A | N/A |
| Multi-arch codegen (x86-64/ARM64/RISC-V) | N/A | N/A | N/A |
| Cross-compilation (ARM64 Linux ELF) | N/A | `build --target arm64-linux` | N/A |
| Cross-compilation (RISC-V Linux ELF) | N/A | `build --target riscv64-linux` | N/A |
| Cross-compilation (Windows PE) | N/A | `build --target x86-64-windows` | N/A |
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
| Stack traces on panic | Yes | Yes | Yes |
| `@constant_time` security annotation | Compile-time | Compile-time | Compile-time |
| `@tainted` taint tracking | Compile-time | Compile-time | Compile-time |
| Profiler (`fidesc profile`) | Yes | N/A | N/A |
| Linter (`fidesc lint`) | Yes | Yes | Yes |
| Benchmark (`fidesc bench`) | Yes | N/A | Yes |
| Formatter (`fidesc fmt`) | Yes | Yes | Yes |
| Package manager (`pacte`) | N/A | N/A | N/A |
| Doc generator (`scribe`) | N/A | N/A | N/A |
| Coverage tool (`veil`) | N/A | N/A | N/A |
| Incremental compilation | N/A | Yes | N/A |
| Function-level parallelism (opt) | N/A | Yes | N/A |

### What Is Parsed But Not Fully Working

| Feature | Status | Notes |
|---------|--------|-------|
| `async` / `await` | ✅ Works | Phase 7.6 — `async fn` declares async functions, `await <expr>` waits for Futures, eager evaluation; state machine transform pending |
| `spawn` | ✅ Works | Phase 7.6/8A — `spawn <expr>` creates OS threads via `p_thread_spawn` |
| `#[test]` attribute | Parsed | Not yet executed by `fidesc test` |
| `@derive` | ✅ Works | Phase 7.7 — `@derive(Debug/Clone/Serialize/Deserialize)` generates methods |
| Effect handler continuations | Syntax works | `handle { do } with` parses and runs; full continuation semantics pending |
| `--opt` (L2 optimizer) | ✅ Works | 23 passes, enabled by default in `build` (use `--no-opt` to disable). `--lto` for aggressive inlining. |
| Default arguments | Not parsed | Not yet implemented |
| Variadic arguments | Not parsed | Not yet implemented |

### Known Limitations

- **ELF64 executable emission works on Linux** — `fidesc build <file> --target x86-64` produces working ELF64 executables directly (zero external tools). `fidesc codegen --elf <out.o>` emits valid relocatable objects (readelf compatible). `fidesc codegen --macho` and `--coff` also produce valid object files.
- **Cross-compilation to ARM64, RISC-V, and Windows** — `fidesc build --target arm64-linux`, `--target riscv64-linux`, and `--target x86-64-windows` all produce valid executables with zero external tools. ARM64 and RISC-V use built-in instruction encoders (`arm64_enc.rs`, `riscv_enc.rs`) and runtime libraries (`runtime_arm64.rs`, `runtime_riscv.rs`) with raw Linux syscalls. Windows uses the PE emitter (`pe.rs`) with kernel32.dll imports.
- **PE emitter targets Windows x64 only** — `fidesc emit` or `fidesc build --target x86-64-windows` produces PE32+ executables; tested via `fidesc test` (153/153 valid)
- **Register allocator** — Linear scan allocator with constant rematerialization (F-1A). Effect-aware allocation (F-1D): pure/@no-alloc calls only clobber RAX. Works correctly for all passing tests.
- **Peephole optimizer always on** — 9 passes run automatically in the `build` command: redundant moves, zero idiom, nop arithmetic, branch threading, dead store, redundant test, store-to-load forwarding, LEA coalescing, cmp+setcc+jcc instruction selection
- **L2 optimizer enabled by default** — PIR optimizer runs automatically in `build` (use `--no-opt` to disable) with 23 passes: const-fold, CSE, GVN, SROA, DSE, DCE, inline, unroll, TCO, alias-analysis, effect-reorder, partial-eval, vectorize, strength-reduce, devirt, jump-thread, loop-rotate, loop-vec, effect-opt, region-pool, reassociate, memoize. `--lto` enables aggressive cross-module inlining.
- **`#[test]` attribute** — parsed but not executed by `fidesc test` (tests are file-based)
- **C codegen coverage** — 153/153 normal tests match interpreter output. Phase 10B: Vec/HashMap/String now implemented in Fides stdlib using raw pointers. Binary hardening: stack canaries, PIE/ASLR, RELRO, NX stack.
- **214 compiler warnings** — dead code, unused variables (cleanup needed)

---

## Quick Start

```bash
# Build the compiler
cargo build

# Build a native ELF64 executable on Linux (zero external tools needed)
cargo run -- build tests/hello.fd -o hello
./hello  # Output: Hello, Fides!50y is big (exit code 8)

# Run a Fides file in the IR interpreter (fast, no external tools)
cargo run -- run tests/hello.fd

# Build with optimization (enabled by default, use --no-opt to disable)
cargo run -- build tests/hello.fd -o hello_opt

# Build via C codegen (requires gcc or clang)
cargo run -- ccompile tests/hello.fd -o hello_c

# Emit a Windows PE executable (cross-compilation, no external tools)
cargo run -- emit tests/hello.fd -o hello.exe
# Or via the unified build command:
cargo run -- build tests/hello.fd --target x86-64-windows -o hello.exe

# Cross-compile to ARM64 Linux (zero external tools)
cargo run -- build tests/hello.fd --target arm64-linux -o hello_arm64
# Verify: readelf -h hello_arm64 → Machine: AArch64

# Cross-compile to RISC-V Linux (zero external tools)
cargo run -- build tests/hello.fd --target riscv64-linux -o hello_riscv
# Verify: readelf -h hello_riscv → Machine: RISC-V

# Generate assembly for multiple architectures
cargo run -- codegen tests/hello.fd --target x86-64
cargo run -- codegen tests/hello.fd --target arm64
cargo run -- codegen tests/hello.fd --target riscv

# Emit object files in multiple formats
cargo run -- codegen tests/hello.fd --elf hello.o
cargo run -- codegen tests/hello.fd --macho hello.macho
cargo run -- codegen tests/hello.fd --coff hello.coff

# View tokens, AST, or IR
cargo run -- tokens tests/hello.fd
cargo run -- ast tests/hello.fd
cargo run -- ir tests/hello.fd

# Run the test suite
cargo run -- test

# Show version
cargo run -- version
```

### CLI Commands

| Command | Description |
|---------|-------------|
| `build <file.fd> -o <out> [--target x86-64\|arm64\|riscv\|arm64-linux\|riscv64-linux\|x86-64-windows] [--opt]` | **Native build**: lex → parse → typecheck → lower → PIR → codegen → peephole → ELF64/PE executable (zero external tools). Cross-compilation to ARM64, RISC-V, and Windows. `--opt` runs L2 optimizer. |
| `emit <file.fd> -o <out.exe>` | **Direct PE emission**: lex → parse → typecheck → lower → x86 machine code → PE file → `.exe` (no external tools, Windows) |
| `emit <file.fd> -o <out.bin> --bare` | **Bare-metal mode**: no stdlib, flat binary output (Phase 5A) |
| `ccompile <file.fd> -o <out>` | C codegen pipeline: lex → parse → typecheck → lower → C source → `gcc`/`clang` → executable |
| `run <file.fd> [-v]` | Lex → parse → typecheck → lower → interpret IR (no external tools needed) |
| `ir <file.fd>` | Lex → parse → typecheck → lower → print SSA IR |
| `pir <file.fd> [1\|2\|3\|all] [--opt]` | Lower to PIR (L1/L2/L3) and print; `--opt` runs optimization passes |
| `opt <file.fd> [pass1,pass2,...]` | Run PIR optimizer and print statistics (const-fold, cse, dce, inline, unroll, tco) |
| `codegen <file.fd> --target <arch> [--peephole] [--elf\|--macho\|--coff <o>]` | Generate assembly for x86-64, ARM64, or RISC-V; optionally emit object files |
| `wasm <file.fd> -o <out.wasm> [--wat]` | WebAssembly backend: PIR → WASM binary |
| `gpu <file.fd>` | SPIR-V GPU backend: PIR → SPIR-V binary |
| `tokens <file.fd>` | Lex only — print token stream |
| `ast <file.fd>` | Lex + parse — print AST tree |
| `test` | Run all 159 normal tests across interpreter, PE emitter, and C codegen |
| `repl` | Interactive REPL — evaluate Fides expressions |
| `verify <file.fd>` | Verify IR correctness (terminators, block refs, value refs) |
| `sourcemap <file.fd>` | Show source map (IR value → source line mapping) |
| `parse-header <file.h> -o <out.fd>` | Parse C header file → generate Fides FFI bindings |
| `watch <file.fd>` | Hot reload: native JIT (ELF64) + function-level change detection |
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

# Bisect which compiler layer broke a test
./scripts/bisect.sh <test_name>
# Example: ./scripts/bisect.sh strings

# Generate golden files for intermediate compiler stages
./scripts/gen_golden.sh
```

**Before making any change:** run `./verify.sh` to confirm you're starting from a known-good state. After changes, run `./verify.sh` again to check for regressions. If a test regresses, use `./scripts/bisect.sh <test_name>` to identify which compiler layer (lowering, PIR, codegen, or peephole) is responsible.

Current baseline:
```
Unit tests:   265/265 passed  (All passing — incl. 47 cross-compilation encoder/runtime tests + 7 incremental compilation)
Harness:      8/8 passed
Interpreter:  153/153 normal
C Codegen:    153/153 matched
PE Emitter:   153/153 valid
Cross-compilation: x86-64, ARM64, RISC-V, Windows — all producing valid executables
```

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

See `docs/syntax-spec.md` for the complete syntax specification.

---

## Project Structure

```
p-lang/
├── Cargo.toml              # Rust project config (4 binary targets: fidesc, pacte, scribe, veil)
├── src/                    # 54 source files, 86,011 lines of Rust
│   ├── main.rs             # CLI entry point (20+ commands: build, emit, run, codegen, ccompile, lsp, profile, fmt, lint, bench, debug, etc.)
│   ├── lib.rs              # Library crate root (shared by all 4 binaries)
│   ├── token.rs            # Token kind definitions (54+ keywords)
│   ├── lexer.rs            # Tokenizer (source → tokens)
│   ├── ast.rs              # AST node types (Expr, Stmt, Func, StructDef, EnumDef, TraitDef, etc.)
│   ├── parser.rs           # Recursive descent parser (tokens → AST)
│   ├── typechecker.rs      # Type checking + capability + effect + region enforcement
│   ├── borrowck.rs         # Borrow checker (aliasing analysis, &mut exclusivity)
│   ├── mono.rs             # Generic monomorphization (specialize generic functions per call site)
│   ├── ir.rs               # SSA IR definitions (types, instructions, functions, modules, effects, regions)
│   ├── lower.rs            # AST → SSA IR lowering
│   ├── interp.rs           # IR interpreter (for `run` command)
│   ├── pe.rs               # Direct PE emitter — x86_64 machine code → .exe + flat binary (no external tools)
│   ├── elf64.rs            # ELF64 executable + object file emitter for Linux (zero external tools)
│   ├── macho.rs            # Mach-O object file emitter for macOS
│   ├── coff.rs             # COFF object file emitter for Windows
│   ├── codegen.rs          # Codegen dispatch + shared utilities
│   ├── codegen_x64.rs      # x86_64 assembly codegen (PIR L3 → AT&T syntax .s)
│   ├── codegen_arm64.rs    # ARM64 assembly codegen (PIR L3 → .s)
│   ├── codegen_riscv.rs    # RISC-V assembly codegen (PIR L3 → .s)
│   ├── codegen_c.rs        # C codegen backend (IR → .c, for `ccompile` command)
│   ├── x86.rs              # Legacy x86_64 MASM assembly generation (IR → .asm)
│   ├── pir.rs              # PIR (Fides Intermediate Representation) — L1/L2/L3 levels
│   ├── opt.rs              # PIR optimizer (23 passes, function-level parallelism)
│   ├── peephole.rs         # Peephole optimizer (9 passes, always on in `build`)
│   ├── regalloc.rs         # Linear scan register allocator
│   ├── comptime.rs         # Comptime evaluation engine (Phase 3A)
│   ├── cparser.rs          # C header parser for FFI bindings
│   ├── tensor.rs           # Tensor type for AI/ML (Phase 5C)
│   ├── autograd.rs         # Autograd / reverse-mode AD (Phase 5C)
│   ├── json.rs             # JSON parsing/serialization (Phase 5D)
│   ├── wasm.rs             # WebAssembly backend (Phase 5D)
│   ├── gpu.rs              # SPIR-V GPU backend (Phase 5D)
│   ├── embedded.rs         # Embedded/HAL framework (Phase 5B)
│   ├── hotreload.rs        # Hot reload support (mmap-based code loading)
│   ├── inc.rs              # Incremental compilation cache (in-memory, hash-keyed)
│   ├── linker.rs           # Incremental linker (ELF64)
│   ├── runtime_linux.rs    # Linux runtime (syscalls for bare-metal/standalone)
│   ├── runtime_arm64.rs    # ARM64 Linux runtime (raw SVC syscalls)
│   ├── runtime_riscv.rs    # RISC-V Linux runtime (raw ECALL syscalls)
│   ├── arm64_enc.rs        # ARM64 instruction encoder
│   ├── riscv_enc.rs        # RISC-V instruction encoder
│   ├── debugger.rs         # Interactive IR-level debugger
│   ├── native_debugger.rs  # Native ptrace-based debugger (Linux)
│   ├── diag.rs             # Diagnostic messages with source context
│   ├── fmt.rs              # Source code formatter
│   ├── lsp.rs              # Language Server Protocol implementation
│   ├── pacte.rs            # Package manager library
│   ├── scribe.rs           # Documentation generator library
│   ├── veil.rs             # Code coverage tool library
│   ├── bin/pacte.rs        # Package manager binary entry point
│   ├── bin/scribe.rs       # Documentation generator binary entry point
│   ├── bin/veil.rs         # Coverage tool binary entry point
│   └── compiler.rs         # Pipeline orchestration, module resolution, stdlib auto-include
├── stdlib/                 # 69 standard library modules, 17,541 LOC
│   ├── core.fd              # Option, Result, println, assert, max, min, abs, clamp, combinators
│   ├── vec.fd               # Vec<T> with iterator adapters (take, skip, zip, enumerate, etc.)
│   ├── math.fd              # vec2/3/4, mat4, quat types, operator overloading, swizzle
│   ├── channel.fd           # Channel-based concurrency
│   ├── sync.fd              # Sync primitives (sync_channel wrappers)
│   ├── parallel.fd          # Parallel operations (par_map, par_sum, par_dot)
│   ├── ecs.fd               # Entity Component System
│   ├── events.fd            # Event system
│   ├── db.fd                # Database driver framework
│   ├── http.fd              # HTTP server stdlib
│   ├── json.fd              # JSON parsing and serialization
│   ├── simd.fd              # SIMD intrinsics
│   ├── shape.fd             # Shape operations
│   ├── tensor.fd            # Tensor type for AI/ML (zeros, ones, matmul, relu, softmax, etc.)
│   ├── quant.fd             # Quantization (q8, find_quant_params, qmatmul)
│   ├── money.fd             # Fixed-point money type, arithmetic, formatting
│   ├── banking.fd           # Accounts, transfers, double-credit/lost-money detection
│   ├── file.fd              # File I/O (file descriptors, read/write)
│   ├── time.fd              # LogicalTime: timezone-free, clock-free deterministic date math
│   ├── typed_strings.fd     # SQLString, ShellString, HTMLString, JSONString, PathString
│   ├── memo.fd              # Memoization helper (Memo::new, get_or_compute)
│   ├── alloc.fd             # Heap allocation builtins (p_alloc_new, p_alloc_free)
│   ├── privacy.fd           # Field privacy test module
│   ├── env.fd               # Environment variable access
│   ├── args.fd              # Command-line argument parsing
│   ├── process.fd           # Process management
│   ├── regex.fd             # Regular expressions
│   ├── tilemap.fd           # Tilemap utilities
│   ├── sprite.fd            # Sprite utilities
│   ├── flowfield.fd         # Flow field utilities
│   ├── streaming.fd         # Streaming data processing
│   ├── scene.fd             # Scene graph
│   ├── physics.fd           # Physics simulation
│   ├── audio.fd             # Audio processing
│   └── input.fd             # Input handling
├── tests/                  # 218 .fd test files + harness
│   ├── harness.rs          # Automated test harness (8 cargo tests)
│   ├── *.fd                 # 193 test files in root
│   ├── safety/             # 9 bounds safety tests (OOB, overflow, div-by-zero, unwrap)
│   ├── capability/         # 4 capability security tests (grant, deny, propagation)
│   ├── effects/            # 7 effect system tests (annotations, inference, handlers, pure)
│   ├── regions/            # 3 region inference tests (blocks, nesting, escape detection)
│   └── golden/             # 148 golden file outputs for regression testing
├── examples/               # 4 example programs
│   ├── http_server.fd       # HTTP server example
│   ├── neural_network.fd    # Neural network example
│   ├── riscv_blink.fd       # RISC-V LED blink (embedded)
│   └── rpi_led_blink.fd     # Raspberry Pi LED blink (embedded)
├── benches/
│   └── bench_main.rs       # Performance benchmarks
└── docs/                   # 4 subdirectories: reference, guides, meta, roadmap
    ├── reference/          # 7 core reference docs
    │   ├── architecture.md # Compiler architecture and pipeline
    │   ├── changelog.md    # Version history and changes
    │   ├── features.md     # Feature list and status
    │   ├── syntax.md       # Complete language syntax specification
    │   ├── testing.md      # Test results + test batch specifications
    │   ├── tool.md         # Tooling guide
    │   └── vision.md       # Language vision and 8 pillars
    ├── guides/             # How things work (3 files)
    │   ├── best-practices.md   # Engineering lessons
    │   ├── architecture-overview.md # Custom toolchain + full-stack
    │   └── borrowchecker.md    # Borrow checker design
    ├── meta/               # Big-picture thinking (4 files)
    │   ├── strategy.md     # Design principles + direction analysis
    │   ├── positioning.md  # Competitive analysis + adoption + industry
    │   ├── future.md       # Innovation + community goals
    │   └── lessons.md      # PL history + IBM lessons
    └── roadmap/            # Roadmaps (7 files)
        ├── roadmap.md      # Master milestone roadmap
        ├── features-roadmap.md # Feature-by-feature roadmap
        ├── llvm-roadmap.md # LLVM parity roadmap
        ├── roadmap-limitations.md # Limitations & pain points
        ├── roadmap-issues.md # Issues & debates
        ├── opportunities.md # AI/ML + industry opportunities
        └── security.md     # Security hardening guide
```

---

## Test Programs

188 test files across `tests/` and subdirectories. Key tests:

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
| `tests/capability/test_cap_*.fd` | Capability security: grant/deny/propagation (Phase 2C) |
| `tests/safety/test_*.fd` | Bounds safety: OOB, overflow, div-by-zero, unwrap None (Phase 2D) |
| `tests/effects/test_effect_*.fd` | Effect system: annotations, inference, checking, handlers (Phase 2B) |
| `tests/regions/test_region_*.fd` | Region inference: region blocks, escape detection, safety (Phase 2A) |
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

Run all tests: `cargo test --test harness`

---

## Design Documents

- `docs/reference/syntax.md` — Complete Fides syntax specification (current `fn`/`val`/`const` syntax + proposed `fn`/`val`/`var` migration)
- `docs/reference/architecture.md` — Compiler architecture, pipeline stages, and internal design
- `docs/roadmap/roadmap.md` — Full 8-phase roadmap (currently at Phase 8 in progress)
- `docs/roadmap/roadmap-limitations.md` — Verified gap analysis (claimed vs actual)
- `docs/reference/vision.md` — Language vision and 8 pillars (Originality, Security, Performance, etc.)
- `docs/meta/strategy.md` — Design principles + direction analysis
- `docs/reference/changelog.md` — Version history and detailed change log
- `docs/reference/features.md` — Feature list and status
- `docs/meta/positioning.md` — Competitive analysis (vs Rust/Zig/Go/Swift) + adoption strategy + industry conversion guide
- `docs/meta/lessons.md` — PL history lessons + IBM AIX/z/OS lessons
- `docs/guides/architecture-overview.md` — Custom toolchain (no LLVM) + full-stack architecture
- `docs/meta/future.md` — Innovation (AI/robotics) + community goals
- `docs/guides/best-practices.md` — Best practices guide
- `docs/guides/borrowchecker.md` — Borrow checker documentation
- `docs/roadmap/opportunities.md` — AI/ML + industry opportunities
- `docs/reference/testing.md` — Test results across all backends + test batch specifications
- `docs/reference/tool.md` — Tooling guide

---

## Roadmap

See `docs/roadmap/roadmap.md` for the full phase status and milestones.
See `docs/roadmap/selfhost-roadmap.md` for the 10-phase self-hosting plan.
