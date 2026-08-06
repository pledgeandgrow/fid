<h1 align="center">Pledge Language — Bootstrap Compiler</h1>

A bootstrap compiler for the Pledge programming language, written in Rust. Generates **native x86_64 Windows executables** via three backends: direct PE emission (no external tools), MASM assembly, or portable C codegen. Also includes an IR interpreter for rapid testing.

## Status

**Phase 1: Bootstrap** — The compiler lexes, parses, type-checks, lowers to SSA IR, and generates native x86_64 Windows executables. The **PE emitter** (`emit` command) produces `.exe` files directly — no assembler, linker, or C compiler required. Additional backends (MASM, C codegen) and an IR interpreter are also available.

### What Works (Verified)

| Feature | Interpreter (`run`) | PE Emitter (`emit`) | MASM (`build`) | C Codegen (`ccompile`) |
|---------|:--------------------:|:-------------------:|:---------------:|:----------------------:|
| String literals + `print` | Yes | Yes | Yes | Yes |
| Integer arithmetic + `print` | Yes | Yes | Yes | Yes |
| `if` / `else if` / `else` | Yes | Yes | Yes | Yes |
| `for` loops with `..` and `..=` ranges | Yes | Yes | Yes | Lowered |
| `while` loops | Yes | Yes | Yes | Lowered |
| Mutable variables (`pv mut`) | Yes | Yes | Yes | Yes |
| Constants (`pc`) | Yes | Yes | Yes | Yes |
| User-defined functions | Yes | Yes | Yes | Yes |
| Recursive function calls | Yes | Yes | Yes | Yes |
| F-string interpolation (`f"{expr}"`) | Yes | Yes | Yes | Lowered |
| String concatenation | Yes | Yes | Yes | Lowered |
| Int-to-string conversion | Yes | Yes | Yes | Lowered |
| Bitwise operators (`& | ^ ~ << >>`) | Yes | Yes | Yes | Lowered |
| Compound assignment (`+= -= *=` etc.) | Yes | Yes | Yes | Lowered |
| Hex / binary / octal literals | Yes | Yes | Yes | Lowered |
| Structs (definition, literals, field access) | Yes | Yes | Yes | Lowered |
| Struct methods (`impl` blocks) | Lowered | — | Parsed | Lowered |
| Pattern matching (`match`) | Lowered | — | Parsed | Lowered |
| Enums with data | Lowered | — | Parsed | Lowered |
| References (`&` / `&mut` / `*`) | Lowered | — | Parsed | Lowered |
| Closures (`pf(x) => expr`) | Lowered | — | Parsed | Lowered |
| Generic functions (`pf<T>`) | Parsed | — | Parsed | Parsed |
| `?` operator | Lowered | — | Parsed | Lowered |
| Tuples | Lowered | — | Parsed | Lowered |
| Arrays (`[1, 2, 3]`, indexing, assignment) | Lowered | — | Lowered | Lowered |
| `for` loop over arrays | Lowered | — | Parsed | Lowered |
| `defer` blocks | Lowered | — | Parsed | Lowered |
| `use` imports (file resolution) | Parsed | Parsed | Parsed | Parsed |
| Monomorphization (generic specialization) | Yes | Yes | Yes | Yes |
| Module imports (`use foo::bar`) | Yes | Yes | Yes | Yes |
| Attributes (`#[test]`) | Parsed | Parsed | Parsed | Parsed |
| Type aliases (`ty`) | Parsed | Parsed | Parsed | Parsed |
| Block expressions | Parsed | Parsed | Parsed | Parsed |

> **Parsed** = syntax accepted by lexer/parser/typechecker but not yet fully lowered to IR or codegen.
> **Lowered** = AST is lowered to SSA IR and executable in the interpreter, but not yet in the PE/native backend.
> **Yes** = works end-to-end (parse → lower → codegen → execute).

---

## Quick Start

```bash
# Build the compiler
cargo build

# Emit a native Windows .exe directly (no assembler, linker, or C compiler needed)
cargo run -- emit tests/hello.p -o hello.exe

# Run a Pledge file in the IR interpreter (fast, no external tools)
cargo run -- run tests/hello.p

# Build via MASM (requires ml64.exe + link.exe)
cargo run -- build tests/functions.p -o fn.exe

# Build via C codegen (requires cl.exe, gcc, or clang)
cargo run -- ccompile tests/hello.p -o hello_c.exe

# View tokens (lexer output)
cargo run -- tokens tests/hello.p

# View AST (parser output)
cargo run -- ast tests/hello.p

# View lowered IR
cargo run -- ir tests/control_flow.p

# Show version
cargo run -- version
```

### CLI Commands

| Command | Description |
|---------|-------------|
| `emit <file.p> -o <out.exe>` | **Direct PE emission**: lex → parse → typecheck → lower → x86 machine code → PE file → `.exe` (no external tools) |
| `build <file.p> -o <out.exe>` | MASM pipeline: lex → parse → typecheck → lower → x86 assembly → `ml64.exe` → `link.exe` → `.exe` |
| `ccompile <file.p> -o <out.exe>` | C codegen pipeline: lex → parse → typecheck → lower → C source → `cl.exe`/`gcc`/`clang` → `.exe` |
| `run <file.p> [-v]` | Lex → parse → typecheck → lower → interpret IR (no external tools needed) |
| `ir <file.p>` | Lex → parse → typecheck → lower → print SSA IR |
| `tokens <file.p>` | Lex only — print token stream |
| `ast <file.p>` | Lex + parse — print AST tree |
| `version` | Print compiler version |

---

## How It Works

The compiler is a multi-stage pipeline:

```
Source (.p) → Lexer → Parser → TypeChecker → Monomorphizer → Lowerer → SSA IR
                                                                         │
                                                    ┌────────────────────┼────────────────────┐
                                                    │                    │                    │
                                                    ▼                    ▼                    ▼
                                            ┌──────────────┐  ┌──────────────┐  ┌──────────────┐
                                            │  PE Emitter  │  │  x86 CodeGen │  │  C Codegen   │
                                            │  pe.rs       │  │  x86.rs      │  │  codegen_c.rs│
                                            └──────────────┘  └──────────────┘  └──────────────┘
                                                    │                    │                    │
                                                    ▼                    ▼                    ▼
                                            Native .exe          MASM (.asm)        C source (.c)
                                                                 │                    │
                                                                 ▼                    ▼
                                                            ┌──────────┐       ┌──────────┐
                                                            │ ml64.exe │       │ cl.exe   │
                                                            └──────────┘       │ gcc      │
                                                                 │              │ clang    │
                                                                 ▼              └──────────┘
                                                            ┌──────────┐            │
                                                            │ link.exe │            ▼
                                                            └──────────┘       Native .exe
                                                                 │
                                                                 ▼
                                                            Native .exe
```

The **PE emitter** (`emit` command) is the primary backend — it encodes x86_64 instructions directly to machine code bytes and constructs a valid PE file with import tables, requiring zero external tools.

### 0. Module Resolution & Monomorphization (`compiler.rs`, `mono.rs`)

Before lowering, the compiler:
- Auto-includes `stdlib/core.p` (user definitions override stdlib)
- Resolves `use` imports by searching for `<path>.p` or `<path>/mod.p` files relative to the source
- Merges stdlib + imported modules + user code (user wins on conflicts)
- **Monomorphizes** generic functions: analyzes all call sites with concrete type arguments, generates specialized non-generic copies (e.g., `identity__i64`), and replaces calls

### 1. Lexer (`lexer.rs`)

Reads Pledge source text and produces a stream of tokens. Handles:

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
- **Runtime functions**: `print`, `__pledge_to_str`, `__pledge_str_concat`, `__pledge_print_int`, `__pledge_print_str`, `__pledge_print_bool`, `__pledge_panic`
- **F-string lowering**: literal parts become `ConstStr`, expression parts are converted via `__pledge_to_str` (if not already a string), then all parts are concatenated via `__pledge_str_concat`
- **Loop lowering**: `for` loops over ranges become `while`-style blocks with a loop variable alloca; `for` loops over arrays become index-based while loops with `GetElemPtr` + `Load`; `while` loops become condition + body + back-edge blocks
- **Alloca type tracking**: `HashMap<ValueId, IrType>` maps each alloca to its IR type, enabling correct `Load` types (not hardcoded `I64`)
- **Defer emission**: `defer_stack` tracks deferred statements; `emit_defers` drains and emits them before `Ret`
- **Match expression lowering**: `lower_match_expr` with result slot alloca, pattern matching branches (IntLit, StrLit, Range, EnumVariant, Wildcard, Ident), merge block
- **Method call lowering**: Name mangling (`__pledge_StructName_method`), SRET for struct returns, receiver as first arg
- **Compound assignment**: Uses `get_value_type()` / `alloca_types` for correct `BinOp` type instead of hardcoded `I64`

### 5. IR Interpreter (`interp.rs`)

Executes the SSA IR directly — no external assembler or linker needed. Used by the `run` command. Supports:

- All IR instructions (arithmetic, comparisons, control flow, calls)
- Runtime function calls (`print`, `__pledge_to_str`, `__pledge_str_concat`, etc.)
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
  - `__pledge_print_int` — integer to string + `WriteFile` to stdout
  - `__pledge_print_str` — string + newline to stdout
  - `__pledge_print_bool` — "true"/"false" to stdout
  - `__pledge_print_float` — (stub, prints truncated int)
  - `__pledge_to_str` — integer to null-terminated string in stack buffer
  - `__pledge_str_concat` — concatenates two strings into static buffer (with temp buffer to avoid aliasing)
  - `__pledge_panic` — prints message + `ExitProcess`
- **Static data**: string literals, concatenation buffers (`str_concat_buf`, `str_concat_tmp`) in `.data` section
- **External imports**: `GetStdHandle`, `WriteFile`, `ExitProcess` from `kernel32.lib`
- **Entry point**: `main` / `_main` (both exported)

### 8. C Code Generator (`codegen_c.rs`)

Translates SSA IR to portable C99 code. Used by the `ccompile` command. Features:

- **Portable C output**: Generates `.c` files compilable with MSVC (`cl.exe`), GCC, or Clang
- **Function renaming**: `main` → `main_entry`, `abs`/`max`/`min` → `p_abs`/`p_max`/`p_min` (avoids C/MSVC conflicts)
- **`#undef` directives**: Removes MSVC `max`/`min`/`abs` macros that conflict with Pledge functions
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
| Print | `print(...)` | `print("Hello, Pledge!");` |
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

### Lowered (AST → SSA IR → Interpreter — not yet in x86 native backend)

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

### Parsed (Lexer + Parser + Typechecker — not yet lowered to IR)

| Feature | Syntax |
|---------|--------|
| Generic functions | `pf identity<T>(x: T) -> T { ret x; }` |
| Type aliases | `ty Count = i32;` |
| `use` imports | `use std::io;` |
| Attributes | `#[test]` |
| `loop` / `break` / `continue` | `loop { ... break; }` |

### Planned (Not Yet Implemented)

| Feature | Syntax | Phase |
|---------|--------|-------|
| Traits (capabilities) | `cap` + `impl` | Phase 2 |
| Ownership & borrowing | `&` / `&mut` (full semantics) | Phase 2 |
| Error handling | `Result<T,E>` + `?` (full semantics) | Phase 2 |
| Modules & imports | `use` (full resolution) | Phase 2 |
| Async / await | `async pf` / `.await` | Phase 3 |
| Effect handlers | `!EffectType` | Phase 4 |
| Compile-time code | `comptime { }` | Phase 4 |
| Region inference | `region` | Phase 4 |
| Hot reload | `#[hot_reloadable]` | Phase 5 |

---

## Pledge Syntax Overview

Pledge is a systems language with unique keywords designed for clarity and AI-friendliness:

```pledge
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

### Pledge Keyword Reference

| Keyword | Meaning | Replaces (Rust) |
|---------|---------|-----------------|
| `pf` | Pledge Function (all functions) | `fn` |
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
pledge-lang/
├── Cargo.toml              # Rust project config
├── src/
│   ├── main.rs             # CLI entry point (emit, build, ccompile, run, ir, tokens, ast, version)
│   ├── token.rs            # Token kind definitions (all Pledge keywords)
│   ├── lexer.rs            # Tokenizer (source → tokens)
│   ├── ast.rs              # AST node types (Expr, Stmt, Func, StructDef, EnumDef, etc.)
│   ├── parser.rs           # Recursive descent parser (tokens → AST)
│   ├── typechecker.rs      # Type validation (AST → checked AST)
│   ├── mono.rs             # Generic monomorphization (specialize generic functions per call site)
│   ├── ir.rs               # SSA IR definitions (types, instructions, functions, modules)
│   ├── lower.rs            # AST → SSA IR lowering
│   ├── interp.rs           # IR interpreter (for `run` command)
│   ├── pe.rs               # Direct PE emitter — x86_64 machine code → .exe (no external tools)
│   ├── x86.rs              # x86_64 MASM assembly generation (IR → .asm, for `build` command)
│   ├── codegen_c.rs        # C codegen backend (IR → .c, for `ccompile` command)
│   ├── codegen.rs          # Legacy ARM64 assembly generation (AST → .s, unused)
│   └── compiler.rs         # Pipeline orchestration, module resolution, stdlib auto-include
├── stdlib/
│   └── core.p              # Core standard library (println, assert, assert_eq, Option, Result, max, min, abs, clamp)
├── tests/
│   ├── hello.p             # Hello world — print, if/else, variables, arithmetic
│   ├── arithmetic.p        # Arithmetic and nested function calls
│   ├── struct.p            # Struct definition, literals, field access
│   ├── control_flow.p      # if/else if/else, for loops with ranges
│   ├── functions.p         # User-defined functions, recursive factorial
│   ├── strings.p           # String literals, f-string interpolation
│   ├── print_int.p         # Integer printing test
│   ├── minimal.p           # Minimal program (ret 42)
│   └── ...                 # Feature tests (features2-6, test_*, etc.)
└── docs/
    ├── syntax-spec.md      # Complete language syntax specification
    ├── architecture.md     # Compiler architecture and pipeline
    └── changelog.md        # Version history and changes
```

---

## Test Programs

### `tests/hello.p`
Basic hello world — demonstrates `pf`, `pv`, `if/else`, `print`.

### `tests/arithmetic.p`
Arithmetic operations — demonstrates function calls, nested calls, variable bindings.

### `tests/struct.p`
Struct operations — demonstrates struct definition, struct literals, field access, functions returning structs.

### `tests/control_flow.p`
Control flow — `if`/`else if`/`else`, `for` loops with exclusive (`..`) and inclusive (`..=`) ranges, mutable variables, compound assignment.

### `tests/functions.p`
User-defined functions — `add(a, b)` and recursive `factorial(n)`. Tests argument passing, return values, and recursion in both interpreter and native x86_64.

### `tests/strings.p`
String handling — string literals, string variables, f-string interpolation with string and integer expressions.

### `tests/features.p`
Feature showcase — compound assignment (`+=`, `-=`, `*=`), bitwise operators (`&`, `|`, `^`, `~`, `<<`, `>>`), `while` loops, mutable variables, constants (`pc`), hex literals.

### `tests/features2.p`–`features5.p`
Advanced features — type aliases, block expressions, struct methods, arrays, match, f-strings, floats, enums, references, closures, generics, tuples, `?` operator, imports, attributes. These exercise the parser and typechecker.

### `tests/features6.p`
Array features — array literals (`[1, 2, 3, 4, 5]`), array indexing (read/write), compound assignment on array elements (`arr[2] += 100`), `for` loop over array with mutable sum accumulation. Verified in both interpreter and native x86_64 (arrays work natively; for-over-array has a known bug in native).

---

## Design Documents

- `docs/syntax-spec.md` — Complete Pledge syntax specification (all keywords, types, features)
- `docs/architecture.md` — Compiler architecture, pipeline stages, and internal design
- `docs/changelog.md` — Version history and detailed change log
- `../docs/foundation-lang.md` — Full language design blueprint and philosophy

---

## Roadmap

| Phase | Goal |
|-------|------|
| **Phase 1** (current) | Bootstrap compiler in Rust — direct PE emission, MASM + C backends, IR interpreter |
| **Phase 2** | Full struct/enum codegen in PE emitter, pattern matching, traits (cap), ownership, error handling, modules |
| **Phase 3** | Async/await, defer, full closures, float formatting |
| **Phase 4** | Effect handlers, comptime blocks, region inference |
| **Phase 5** | Self-hosting — compiler rewritten in Pledge, hot reload |
