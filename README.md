<h1 align="center">Fides Language — Bootstrap Compiler</h1>

A bootstrap compiler for the Fides programming language, written in Rust. Generates **native ELF64 executables on Linux** and **PE executables on Windows** via zero-external-tool emission, plus portable C codegen. Also includes an IR interpreter for rapid testing, WebAssembly and SPIR-V GPU backends, multi-arch assembly codegen (x86-64, ARM64, RISC-V), ELF64/Mach-O/COFF object file emission, and a bare-metal flat binary mode.

## Status

**Phase 1: Foundation — Complete.** **Phase 2: Safety — Complete.** **Phase 3: Power — Complete (interpreter).** **Phase 4: Performance — Mostly complete.** **Phase 5: Platform — Mostly complete.** The compiler lexes, parses, type-checks, lowers to SSA IR, and generates native executables. The **ELF64 emitter** (`build --target x86-64` on Linux) produces working executables directly — no assembler, linker, or C compiler required. The **PE emitter** (`emit` command) produces Windows `.exe` files directly. Phase 2 adds region inference, effect system, capability security (with compile-time elision), and bounds safety. Phase 4 has a known register allocator bug that prevents `--opt` from being enabled by default. See `docs/roadmap.md` for full phase status and what's left.

### Test Results (Verified Aug 2026)

| Test Group | Result | Notes |
|------------|:------:|-------|
| Unit tests (`cargo test --bin fidesc`) | **136/136** | elf64, pir, peephole (26 tests), opt, regalloc, tensor, codegen_x64, hotreload (8), embedded, gpu, wasm, coff, macho, linker |
| Harness tests (`cargo test --test harness`) | **8/8** | All pass including golden_files_match |
| Interpreter (`fidesc test`) | **113/113** normal + **20/20** expected-fail | Expected-fail: trait verify, budget, sandbox, deterministic, field privacy, money linear, money conserve, banking double-credit, banking lost-money, loop linear, gas, crashfree, txn rollback, typed strings, init guarantee, param queries |
| PE Emitter (`fidesc test`) | **113/113** normal + **20/20** expected-fail | All PE structures valid |
| C Codegen (`fidesc test`) | **113/113** normal + **20/20** expected-fail | FULL PARITY with interpreter — no gaps |
| Total `.fd` test files | **123** | 113 normal + 20 expected-fail |
| Source files | **36** `.rs` | 52,845 LOC |
| Golden files | **43** | Expected outputs in `tests/golden/` |

### Financial Safety Features (New)

| Feature | Syntax | Description |
|---------|--------|-------------|
| Linear types | `@linear` | Move semantics — no duplication of linear values |
| Conservation proof | `@conserve` | Compile-time financial conservation proof (money in = money out) |
| Money stdlib | `stdlib/money.fd` | Fixed-point money type, arithmetic, formatting |
| Banking stdlib | `stdlib/banking.fd` | Accounts, transfers, double-credit/lost-money detection |
| Tuple destructuring | `pv (a, b) = expr` | Decompose tuple values into bindings |
| File I/O | `stdlib/file.fd` | File descriptors, read/write via `stdlib/file.fd` |
| Atomic transactions | `txn { ... }` | Atomic transaction blocks with rollback on failure |
| Gas budgets | `@gas(N)` | Compile-time gas budget annotation |
| Crash-free proof | `@crashfree` | Compile-time proof that a function cannot panic |
| Audit logging | `@audit` | Automatic function audit logging with parameter values |
| Loop-aware linear types | — | Linear variables cannot be consumed inside loops (compile error) |
| LogicalTime | `stdlib/time.fd` | Timezone-free, clock-free deterministic date math |
| Typed strings | `stdlib/typed_strings.fd` | SQLString, ShellString, HTMLString, JSONString, PathString — injection prevention via type system |
| Init guarantee | `pm` keyword | Uninitialized mutable variables + definite assignment analysis (compile error on read before assignment) |
| Raw pointer types | `*mut T`, `*const T` | Raw pointers with `unsafe {}` enforcement (no borrow checking) |
| Memoization | `stdlib/memo.fd` | Simple cache helper: `Memo::new()`, `get_or_compute()` |
| Checked arithmetic | F64 | Integer overflow = panic (always on, not just debug mode) |
| `@wrapping` | `@wrapping pf foo() { ... }` | Escape hatch for wrap-around arithmetic (hashing, crypto) |
| Bounds checking lock | F62 | Bounds checks always on, skippable in `unsafe {}` blocks |
| Parameterized queries | F68 | No string concat on typed strings (SQLString + str = compile error) |
| `@medical` | `@medical @audit pf foo() { ... }` | IEC 62304: no dynamic alloc, no recursion, no while, no unwrap, must be @audit |
| `given`/`ensure` contracts | `pf foo(x: i64) -> i64 given x >= 0 { ... }` | Design by Contract — preconditions verified at call sites |
| `async`/`await`/`spawn` | `async pf foo() -> Future<T> { ... }` | Cooperative concurrency — async functions, await, spawn (Phase 7.6) |
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
| Mutable variables (`pv mut`) | Yes | Yes | Yes |
| Constants (`pc`) | Yes | Yes | Yes |
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
| Closures (`pf(x) => expr`) | Yes | Yes | Yes |
| Closure environment capture | Yes | Yes | Yes |
| Generic functions (`pf<T>`) | Yes | Yes | Yes |
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
| Type aliases (`ty X = T`) | Yes | Yes | Yes |
| Match guards (`pattern if cond =>`) | Yes | Yes | Yes |
| Match range patterns (`1..=5 =>`) | Yes | Yes | Yes |
| Trait associated types (`ty Item;`) | Yes | Yes | Yes |
| Trait default methods | Yes | Yes | Yes |
| Trait objects (`dyn Trait`) | Yes | Yes | Yes |
| Trait impl verification | Yes | Yes | Yes |
| Inline assembly (`asm!("...")`) | Yes | Yes | Yes |
| Effect annotations (`!IO`, `!{}`, `!{IO,Net}`) | Yes | Yes | Yes |
| Effect inference (fixpoint over call graph) | Yes | Yes | Yes |
| Effect checking (under-declaration errors) | Yes | Yes | Yes |
| Effect handlers (`handle { do } with`) | Parsed | Parsed | Parsed |
| Region blocks (`region r { ... }`) | Yes | Yes | Yes |
| Region use-after-free detection | Compile-time | Compile-time | Compile-time |
| Region escape detection | Compile-time | Compile-time | Compile-time |
| Capability elision (zero runtime overhead) | Compile-time | Compile-time | Compile-time |
| Bounds checking (Vec/array/string) | Runtime (debug) | Runtime (debug) | Runtime (debug) |
| Integer overflow checking | Runtime (debug) | Runtime (debug) | Runtime (debug) |
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
| Tuple destructuring (`pv (a, b) = expr`) | Yes | Yes | Yes |
| `txn {}` atomic transactions | Yes | Yes | N/A |
| `@gas` gas budgets | Compile-time | Compile-time | Compile-time |
| `@crashfree` panic prevention | Compile-time | Compile-time | Compile-time |
| `@audit` audit logging | Yes | Yes | Yes |
| Loop-aware linear types | Compile-time | Compile-time | Compile-time |
| LogicalTime (`stdlib/time.fd`) | Yes | Yes | Yes |
| Typed strings (`stdlib/typed_strings.fd`) | Yes | Yes | Yes |
| Init guarantee (`pm` keyword) | Yes | Yes | Yes |
| WebAssembly backend (`fidesc wasm`) | N/A | N/A | N/A |
| SPIR-V GPU backend (`fidesc gpu`) | N/A | N/A | N/A |
| Hot reload (`fidesc watch`) | N/A | N/A | N/A |
| Multi-arch codegen (x86-64/ARM64/RISC-V) | N/A | N/A | N/A |
| ELF64/Mach-O/COFF object files | N/A | N/A | N/A |
| Integrated linker (`fidesc link`) | N/A | N/A | N/A |
| Embedded/IoT support (`embedded.rs`) | N/A | N/A | N/A |

### What Is Parsed But Not Fully Working

| Feature | Status | Notes |
|---------|--------|-------|
| `async` / `await` | Parsed | Phase 4+ — not yet implemented |
| `spawn` | Parsed | Phase 4+ — not yet implemented |
| `#[test]` attribute | Parsed | Not yet executed by `fidesc test` |
| `#[derive]` | Parsed | Not yet implemented |
| Effect handler continuations | Syntax works | `handle { do } with` parses and runs; full continuation semantics pending |
| `--opt` (L2 optimizer) | Partial | Inliner fixed, but exposes pre-existing register allocator bug for complex programs. Simple programs work correctly. Not enabled by default. |
| Default arguments | Not parsed | Not yet implemented |
| Variadic arguments | Not parsed | Not yet implemented |

### Known Limitations

- **ELF64 executable emission works on Linux** — `fidesc build <file> --target x86-64` produces working ELF64 executables directly (zero external tools). `fidesc codegen --elf <out.o>` emits valid relocatable objects (readelf compatible). `fidesc codegen --macho` and `--coff` also produce valid object files.
- **PE emitter targets Windows x64 only** — `fidesc emit` produces PE32+ executables; tested via `fidesc test` (113/113 valid + 20/20 expected-fail)
- **Register allocator has a pre-existing bug** — linear scan allocator in `regalloc.rs` maps VRegs to physical registers, but constants not loaded into registers before use can be clobbered by intervening instructions. Exposed by the L2 optimizer's instruction reordering. Blocks enabling `--opt` by default.
- **Peephole optimizer always on** — 9 passes run automatically in the `build` command: redundant moves, zero idiom, nop arithmetic, branch threading, dead store, redundant test, store-to-load forwarding, LEA coalescing, cmp+setcc+jcc instruction selection
- **L2 optimizer (`--opt`) not enabled by default** — PIR optimizer exists (`fidesc opt`, `fidesc pir --opt`, `fidesc build --opt`) with constant folding, DCE, CSE, inlining, TCO, loop unrolling, and effect-based pure function elision. Works for simple programs but exposes the register allocator bug on complex ones.
- **`#[test]` attribute** — parsed but not executed by `fidesc test` (tests are file-based)
- **C codegen coverage** — 113/113 tests match interpreter output (+ 20/20 expected-fail). FULL PARITY — no gaps. `test_db` is no longer flaky (HashMap keys sorted for deterministic output).
- **215 compiler warnings** — dead code, unused variables (cleanup needed)

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

# Build with optimization (smaller binary, but may be incorrect on complex programs)
cargo run -- build tests/hello.fd -o hello_opt --opt

# Build via C codegen (requires gcc or clang)
cargo run -- ccompile tests/hello.fd -o hello_c

# Emit a Windows PE executable (cross-compilation, no external tools)
cargo run -- emit tests/hello.fd -o hello.exe

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
| `build <file.fd> -o <out> [--target x86-64\|arm64\|riscv] [--opt]` | **Native build**: lex → parse → typecheck → lower → PIR → codegen → peephole → ELF64 executable (zero external tools on Linux). `--opt` runs L2 optimizer. |
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
| `test` | Run all 130 tests across interpreter, PE emitter, and C codegen |
| `repl` | Interactive REPL — evaluate Fides expressions |
| `verify <file.fd>` | Verify IR correctness (terminators, block refs, value refs) |
| `sourcemap <file.fd>` | Show source map (IR value → source line mapping) |
| `parse-header <file.h> -o <out.fd>` | Parse C header file → generate Fides FFI bindings |
| `watch <file.fd>` | Hot reload: native JIT (ELF64) + function-level change detection |
| `link <files...>` | Link multiple object files (integrated linker) |
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
Unit tests:   136/136 passed
Harness:      8/8 passed
Interpreter:  113/113 passed   (+ 20/20 expected fail: trait verify, budget, sandbox, deterministic, field privacy, money linear, money conserve, banking double-credit, banking lost-money, loop linear, gas, crashfree, txn rollback, typed strings, init guarantee, param queries)
C Codegen:    113/113 matched   (FULL PARITY — no gaps)
PE Emitter:   113/113 valid     (+ 20/20 expected fail: same as interpreter)
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

The **ELF64 emitter** (`build` command on Linux) is the primary native backend — it encodes x86_64 instructions directly to machine code bytes and constructs a valid ELF64 executable with zero external tools. The **PE emitter** (`emit` command) does the same for Windows.

### 0. Module Resolution & Monomorphization (`compiler.rs`, `mono.rs`)

Before lowering, the compiler:
- Auto-includes `stdlib/core.fd` (user definitions override stdlib)
- Resolves `use` imports by searching for `<path>.fd` or `<path>/mod.fd` files relative to the source
- Merges stdlib + imported modules + user code (user wins on conflicts)
- **Monomorphizes** generic functions: analyzes all call sites with concrete type arguments, generates specialized non-generic copies (e.g., `identity__i64`), and replaces calls

### 1. Lexer (`lexer.rs`)

Reads Fides source text and produces a stream of tokens. Handles:

- **Keywords**: `pf`, `pv`, `pc`, `mut`, `cap`, `ty`, `ret`, `if`, `else`, `match`, `struct`, `enum`, `impl`, `pub`, `use`, `async`, `await`, `comptime`, `defer`, `spawn`, `for`, `while`, `loop`, `break`, `continue`, `in`, `as`, `where`, `region`, `move`, `static`, `true`, `false`
- **Primitive types**: `i8`–`i128`, `u8`–`u128`, `f32`, `f64`, `bool`, `str`, `void`
- **Operators**: `+ - * / % == != < > <= >= && || ! & | ^ ~ << >> = += -= *= /= %= &= |= ^= <<= >>= -> => :: ? .. ..=`
- **Literals**: integers (hex `0xFF`, binary `0b1010`, octal `0o777`, underscores `1_000`), floats, strings, f-strings, identifiers
- **Comments**: `//` line comments, `/* */` block comments

### 2. Parser (`parser.rs`)

Recursive descent parser. Consumes the token stream and builds an AST. Handles:

- **Top-level declarations**: `pf` functions, `struct` definitions, `enum` definitions, `impl` blocks, `ty` aliases, `pc` constants, `use` imports, `#[attributes]`
- **Statements**: `pv`/`pv mut` bindings, `ret`, `if`/`else`, `for`/`while`/`loop`, `break`/`continue`, `match`, `defer`, assignment + compound assignment, expression statements
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
- `ret`, `push`, `pop`, `lea_rbp`, `sub_rsp_i32`, `add_rsp_i32`
- `mov_mem_reg8` (byte store for digit conversion)

### 7. x86_64 MASM Code Generator (`x86.rs`)

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

### 8. C Code Generator (`codegen_c.rs`)

Translates SSA IR to portable C99 code. Used by the `ccompile` command. Features:

- **Portable C output**: Generates `.c` files compilable with MSVC (`cl.exe`), GCC, or Clang
- **Function renaming**: `main` → `main_entry`, `abs`/`max`/`min` → `p_abs`/`p_max`/`p_min` (avoids C/MSVC conflicts)
- **`#undef` directives**: Removes MSVC `max`/`min`/`abs` macros that conflict with Fides functions
- **SSA variable declarations**: All values declared once at function start; instructions are assignments
- **Polymorphic `print`**: Dispatches to `p_print_str`/`p_print_int`/`p_print_bool`/`p_print_float` + `p_print_newline()` based on argument type
- **Runtime functions**: `p_print*`, `p_panic`, `p_malloc`, `p_free`, `p_str_concat`, `p_to_str`, Vec/HashMap/String operations
- **Control flow**: Basic blocks as C labels with `goto` statements
- **MSVC integration**: `find_cl()` and `find_vcvarsall()` locate and set up the MSVC compiler environment

### 9. Compiler Pipeline (`compiler.rs`)

Orchestrates the stages. Entry points:

- `emit_source_with_path(source, path)` — Direct PE emission: lex → parse → typecheck → monomorphize → lower → PE bytes
- `build_source_with_path(source, path)` — MASM pipeline: lex → parse → typecheck → monomorphize → lower → x86 assembly string
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
| Functions | `pf` | `pf add(a: i32, b: i32) -> i32 { ret a + b; }` |
| Variables | `pv` / `pv mut` | `pv x = 42;` / `pv mut y = 0;` |
| Constants | `pc` | `pc MAX: i32 = 100;` |
| If/Else | `if` / `else` | `if x > 0 { ... } else { ... }` |
| For loops | `for x in start..end` | `for i in 0..5 { sum += i; }` |
| While loops | `while cond` | `while counter < 5 { ... }` |
| Return | `ret` | `ret 42;` |
| Arithmetic | `+ - * / %` | `a + b * c` |
| Comparison | `== != < > <= >=` | `if x >= 10 { ... }` |
| Logical | `&& \|\| !` | `if a && !b { ... }` |
| Bitwise | `& \| ^ ~ << >>` | `0xFF & 0x0F` |
| Compound assign | `+= -= *= /= %=` | `x += 10;` |
| Function calls | `name(args)` | `add(10, 20)` |
| Recursive calls | self-referential `pf` | `pf fact(n: i32) -> i32 { ... fact(n-1) ... }` |
| Print | `print(...)` | `print("Hello, Fides!");` |
| String literals | `"..."` | `print("Hello");` |
| F-strings | `f"{expr}"` | `f"Hello, {name}!"` |
| F-string with int | `f"x = {x}"` | `print(f"x = {x}");` |
| Hex/binary/octal | `0xFF` `0b1010` `0o777` | `pv y = 0xFF & 0x0F;` |
| Type annotations | `: type` | `pv x: i32 = 42;` |
| Semicolons | `;` | Statement terminator |
| Comments | `//` `/* */` | `// line comment` |
| Structs | `struct Point { x: i32, y: i32 }` | `struct Point { x: i64, y: i64 }` |
| Struct literals | `Point { x: 1, y: 2 }` | `pv p = Point { x: 1, y: 2 };` |
| Field access | `point.x` | `print(p.x);` |
| Struct return (SRET) | `pf make_point() -> Point { ... }` | `pv p = make_point(); print(p.x);` |

### Lowered (AST → SSA IR → Interpreter + PE Emitter)

| Feature | Syntax |
|---------|--------|
| Struct methods | `impl Point { pf get_x(self: Point) -> i32 { ... } }` |
| Pattern matching | `match val { 1 => { ... } _ => { ... } }` |
| Match patterns (int, str, range, enum) | `match s { "foo" => { ... } 1..=10 => { ... } Shape::Circle(r) => { ... } }` |
| Enums with data | `enum Shape { Circle(f64), Rectangle(f64, f64), Unit }` |
| Enum variant construction | `Shape::Circle(3.14)` |
| References | `&x`, `&mut x`, `*x` |
| Closures (lambda lifting) | `pf(x: i32) => x * 2` |
| `?` operator | `expr?` |
| Tuples | `(i32, bool)`, `(42, true)` |
| Array literals | `[1, 2, 3, 4, 5]` |
| Array indexing + assignment | `arr[0]`, `arr[0] = 10`, `arr[2] += 100` |
| `for` loop over arrays | `for x in arr { sum += x; }` |
| `defer` blocks | `defer close(file);` |
| Block expressions | `{ pv a = 10; pv b = 20; a + b }` |
| Linear types (`@linear`) | `@linear pf transfer(...) { ... }` — move semantics, no duplication |
| `@conserve` | `@conserve pf settle(...) { ... }` — compile-time financial conservation proof |
| Tuple destructuring | `pv (a, b) = expr` |
| `txn {}` blocks | `txn { transfer(a, b, 100); }` — atomic transactions with rollback |
| `@gas` | `@gas(1000) pf foo() { ... }` — compile-time gas budgets |
| `@crashfree` | `@crashfree pf foo() { ... }` — compile-time proof of no panics |
| `@audit` | `@audit pf foo() { ... }` — automatic function audit logging |
| Loop-aware linear types | Linear vars consumed in loops → compile error |

### Parsed (Lexer + Parser + Typechecker — not yet lowered to IR)

| Feature | Syntax | Notes |
|---------|--------|-------|
| `async` / `await` | `async pf` / `.await` | Parsed, not yet implemented (Phase 4+) |
| `spawn` | `spawn` | Parsed, not yet implemented |
| `#[test]` attribute | `#[test]` | Parsed but not executed by `fidesc test` |
| `#[derive]` | `#[derive]` | Parsed, not yet implemented |

### Planned (Not Yet Implemented)

| Feature | Syntax | Phase |
|---------|--------|-------|
| Default arguments | `pf f(x: i32 = 0)` | Future |
| Variadic arguments | `pf f(...args)` | Future |
| Self-hosting | (rewrite compiler in Fides) | Phase 6 |

---

## Fides Syntax Overview

Fides is a systems language with unique keywords designed for clarity and AI-friendliness:

- **Compile-time financial conservation** — Bugs that cost the real world billions (Knight Capital: $440M in 45 minutes from double-processing) are compile errors in Fides. The `@conserve` annotation proves money-in equals money-out at compile time, and linear types (`@linear`) prevent double-processing of financial values.

```p
// Function declaration
pf main() {
    pv x = 42;
    pv mut y = 0;
    y = x + 8;
    print(y);
}

// Struct with methods
struct Point {
    x: f64,
    y: f64,
}

impl Point {
    pf new(x: f64, y: f64) -> Point {
        Point { x: x, y: y }
    }

    pf distance(&self, other: &Point) -> f64 {
        pv dx = self.x - other.x;
        pv dy = self.y - other.y;
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
pc MAX_PLAYERS: i32 = 64;
```

### Fides Keyword Reference

| Keyword | Meaning | Replaces (Rust) |
|---------|---------|-----------------|
| `pf` | Fides Function (all functions) | `fn` |
| `pv` / `pv mut` | Variable (immutable / mutable) | `let` / `let mut` |
| `pc` | Constant | `const` |
| `cap` | Capability / Trait | `trait` |
| `ret` | Return | `return` |
| `ty` | Type alias | `type` |
| `comptime` | Compile-time code (no macros) | N/A (replaces macros) |
| `defer` | Deferred cleanup | N/A (Go-inspired) |
| `spawn` | Task spawning | N/A |
| `region` | Explicit memory region | `'a` lifetimes |

See `docs/syntax-spec.md` for the complete syntax specification.

---

## Project Structure

```
p-lang/
├── Cargo.toml              # Rust project config
├── src/                    # 35 source files, ~45,000 lines of Rust
│   ├── main.rs             # CLI entry point (21 commands: build, emit, run, codegen, ccompile, etc.)
│   ├── token.rs            # Token kind definitions (45+ keywords)
│   ├── lexer.rs            # Tokenizer (source → tokens)
│   ├── ast.rs              # AST node types (Expr, Stmt, Func, StructDef, EnumDef, TraitDef, etc.)
│   ├── parser.rs           # Recursive descent parser (tokens → AST)
│   ├── typechecker.rs      # Type checking + capability + effect + region enforcement
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
│   ├── opt.rs              # PIR optimizer (const fold, CSE, DCE, inline, unroll, TCO, effect-based elision)
│   ├── peephole.rs         # Peephole optimizer (9 passes, always on in `build`)
│   ├── regalloc.rs         # Linear scan register allocator
│   ├── comptime.rs         # Comptime evaluation engine (Phase 3A)
│   ├── cparser.rs          # C header parser for FFI bindings
│   ├── tensor.rs           # Tensor type for AI/ML (Phase 5C)
│   ├── json.rs             # JSON parsing/serialization (Phase 5D)
│   ├── wasm.rs             # WebAssembly backend (Phase 5D)
│   ├── gpu.rs              # SPIR-V GPU backend (Phase 5D)
│   ├── embedded.rs         # Embedded/HAL framework (Phase 5B)
│   ├── hotreload.rs        # Hot reload support (mmap-based code loading)
│   ├── linker.rs           # Incremental linker (ELF64)
│   ├── runtime_linux.rs    # Linux runtime (syscalls for bare-metal/standalone)
│   └── compiler.rs         # Pipeline orchestration, module resolution, stdlib auto-include
├── stdlib/                 # 17 standard library modules
│   ├── core.fd              # Option, Result, println, assert, max, min, abs, clamp
│   ├── json.fd              # JSON parsing and serialization
│   ├── tensor.fd            # Tensor type for AI/ML (zeros, ones, matmul, relu, softmax, etc.)
│   ├── parallel.fd          # Parallel operations (par_map, par_sum, par_dot)
│   ├── db.fd                # Database driver framework
│   ├── http.fd              # HTTP server stdlib
│   ├── simd.fd              # SIMD intrinsics
│   ├── shape.fd             # Shape operations
│   ├── quant.fd             # Quantization (q8, find_quant_params, qmatmul)
│   ├── money.fd             # Fixed-point money type, arithmetic, formatting
│   ├── banking.fd           # Accounts, transfers, double-credit/lost-money detection
│   ├── file.fd              # File I/O (file descriptors, read/write)
│   ├── time.fd              # LogicalTime: timezone-free, clock-free deterministic date math
│   ├── typed_strings.fd     # F67: SQLString, ShellString, HTMLString, JSONString, PathString
│   ├── memo.fd              # F12: Memoization helper (Memo::new, get_or_compute)
│   ├── alloc.fd             # Heap allocation builtins (p_alloc_new, p_alloc_free)
│   └── privacy.fd           # Field privacy test module
├── tests/                  # 127 .fd test files + harness
│   ├── harness.rs          # Automated test harness (8 cargo tests)
│   ├── *.fd                 # 107 feature tests + 16 expected-fail tests
│   ├── safety/             # 9 bounds safety tests (OOB, overflow, div-by-zero, unwrap)
│   ├── capability/         # 4 capability security tests (grant, deny, propagation)
│   ├── effects/            # 7 effect system tests (annotations, inference, handlers, pure)
│   ├── regions/            # 3 region inference tests (blocks, nesting, escape detection)
│   └── golden/             # 43 golden file outputs for regression testing
├── examples/               # 4 example programs
│   ├── http_server.fd       # HTTP server example
│   ├── neural_network.fd    # Neural network example
│   ├── riscv_blink.fd       # RISC-V LED blink (embedded)
│   └── rpi_led_blink.fd     # Raspberry Pi LED blink (embedded)
├── benches/
│   └── bench_main.rs       # Performance benchmarks
└── docs/                   # 12 documentation files
    ├── roadmap.md          # Full 8-phase roadmap
    ├── roadmap-limitations.md # Verified gap analysis (claimed vs actual)
    ├── syntax-spec.md      # Complete language syntax specification
    ├── architecture.md     # Compiler architecture and pipeline
    ├── changelog.md        # Version history and changes
    ├── vision.md           # Language vision and 8 pillars
    ├── design-principles.md # Core design principles
    ├── competitive-analysis.md # Comparison with Rust, Zig, Go, Swift
    ├── adoption-strategy.md # Language adoption strategy
    ├── full-stack-architecture.md # Full-stack Fides ecosystem vision
    ├── innovation.md       # Innovative language features
    └── custom-toolchain.md # Custom toolchain plans
```

---

## Test Programs

127 test files across `tests/` and subdirectories. Key tests:

| Test File | What It Tests |
|-----------|---------------|
| `hello.fd` | Hello world — `pf`, `pv`, `if/else`, `print` |
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
| `test_type_alias.fd` | Type aliases (`ty X = T`) |
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
| `test_tuple_destructure.fd` | Tuple destructuring (`pv (a, b) = expr`) |
| `test_file.fd` | File I/O (`stdlib/file.fd`) |
| `test_txn.fd` | `txn {}` atomic transaction blocks |
| `test_time*.fd` | LogicalTime stdlib: construction, extraction, arithmetic, formatting, leap years |
| `test_typed_strings*.fd` | F67: SQLString, ShellString, HTMLString, JSONString, PathString — escape, bind, is_safe |
| `test_init_guarantee*.fd` | F63: `pm` keyword, definite assignment analysis, init guarantee |

Run all tests: `cargo test --test harness`

---

## Design Documents

- `docs/syntax-spec.md` — Complete Fides syntax specification (all keywords, types, features)
- `docs/architecture.md` — Compiler architecture, pipeline stages, and internal design
- `docs/roadmap.md` — Full 8-phase roadmap (currently at Phase 1 complete)
- `docs/vision.md` — Language vision and 8 pillars (Originality, Security, Performance, etc.)
- `docs/design-principles.md` — Core design principles
- `docs/changelog.md` — Version history and detailed change log
- `docs/competitive-analysis.md` — Comparison with Rust, Zig, Go, Swift
- `docs/adoption-strategy.md` — Language adoption strategy
- `docs/full-stack-architecture.md` — Full-stack Fides ecosystem vision
- `docs/innovation.md` — Innovative language features
- `docs/custom-toolchain.md` — Custom toolchain plans
- `features.md` — Detailed feature status matrix
- `TEST.md` — Complete test results across all backends

---

## Roadmap

| Phase | Goal | Status |
|-------|------|--------|
| **Phase 1** | Bootstrap compiler in Rust — direct PE/ELF64 emission, C backend, IR interpreter | **Complete** (136 unit tests, 113/113 + 20/20 expected-fail integration tests) |
| **Phase 2** | Region inference, effect system, capability security, bounds safety | **Complete** (2A ✓, 2B ✓, 2C ✓ with elision, 2D ✓; 8/8 harness tests pass) |
| **Phase 3** | comptime, advanced traits, FFI & interop | **Complete** (3A comptime ✓, 3B traits ✓, 3C const generics + GATs ✓; 3D C FFI ✓ via `extern "C"` + `parse-header`) |
| **Phase 4** | PIR optimizer, multi-arch codegen, integrated linker, hot reload | **Mostly complete** (4A PIR ✓, 4B optimizer ✓ but disabled by default due to regalloc bug, 4C x86-64/arm64/riscv codegen + regalloc + 9 peephole passes + ELF64 executables ✓; 4D linker ✓, 4E hot reload ✓ via `fidesc watch`) |
| **Phase 5** | Bare metal, IoT, AI/ML, web & cloud | **Mostly complete** (5A bare-metal mode ✓, 5B IoT/embedded framework ✓ `embedded.rs`, 5C AI/ML tensor ✓, 5D WASM + GPU/SPIR-V + HTTP + JSON ✓) |
| **Phase 6** | Self-hosting | Not started |
| **Phase 7** | Ecosystem (package manager, LSP, docs) | Not started |
| **Phase 8** | World class | Not started |

See `docs/roadmap.md` for the full 8-phase roadmap and `docs/roadmap-limitations.md` for the verified gap analysis.
