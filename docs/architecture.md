# Pledge Compiler Architecture

**Version:** 0.4.0
**Targets:** x86_64 (Windows) native executables — direct PE emission (primary), MASM, C codegen + IR interpreter

---

## 1. Pipeline Overview

```
Source (.p)
    │
    ▼
┌──────────┐     ┌──────────┐     ┌──────────────┐     ┌──────────────┐     ┌──────────┐     ┌───────────────┐
│  Lexer   │────▶│  Parser  │────▶│  TypeChecker │────▶│  Monomorph   │────▶│  Lowerer │────▶│   SSA IR      │
│lexer.rs  │     │parser.rs │     │typechecker.rs│     │  mono.rs     │     │lower.rs  │     │   ir.rs       │
└──────────┘     └──────────┘     └──────────────┘     └──────────────┘     └──────────┘     └───────────────┘
                                                                    │
                                                    ┌───────────────┼───────────────┐
                                                    │               │               │
                                                    ▼               ▼               ▼
                                            ┌──────────────┐ ┌──────────────┐ ┌──────────────┐
                                            │  PE Emitter  │ │  x86_64 Code │ │  C Codegen   │
                                            │  pe.rs       │ │  x86.rs      │ │  codegen_c.rs│
                                            └──────────────┘ └──────────────┘ └──────────────┘
                                                    │               │               │
                                                    ▼               ▼               ▼
                                            Native .exe       MASM (.asm)    C source (.c)
                                                             │               │
                                                             ▼               ▼
                                                        ┌──────────┐    ┌──────────┐
                                                        │ ml64.exe │    │ cl.exe   │
                                                        └──────────┘    │ gcc      │
                                                             │          │ clang    │
                                                             ▼          └──────────┘
                                                        ┌──────────┐         │
                                                        │ link.exe │         ▼
                                                        └──────────┘    Native .exe
                                                             │
                                                             ▼
                                                        Native .exe
```

The **PE emitter** is the primary backend — it produces `.exe` files directly with zero external tool dependencies.

### Entry Points (`compiler.rs`)

| Function | Pipeline | CLI Command |
|----------|----------|-------------|
| `emit_source_with_path(source, path)` | lex → parse → typecheck → monomorphize → lower → PE emission | `emit` |
| `build_source_with_path(source, path)` | lex → parse → typecheck → monomorphize → lower → x86 codegen | `build` |
| `ccompile_source_with_path(source, path)` | lex → parse → typecheck → monomorphize → lower → C codegen | `ccompile` |
| `interpret_source_with_path(source, path, verbose)` | lex → parse → typecheck → monomorphize → lower → interpret | `run` |
| `lower_to_ir(source)` | lex → parse → typecheck → monomorphize → lower → format IR | `ir` |
| `lex(source)` | lex only | `tokens` |
| `parse_to_ast(source)` | lex → parse | `ast` |

### Module Resolution (`compiler.rs`)

Before type-checking, the compiler:
1. Parses `stdlib/core.p` (auto-included)
2. Parses user source with stdlib names pre-populated
3. Resolves `use` imports by searching for `<path>.p` or `<path>/mod.p` relative to source
4. Merges stdlib + imported modules + user code (user wins on conflicts)
5. Type-checks the merged program with module-level visibility enforcement

### Monomorphization (`mono.rs`)

After type-checking, generic functions are monomorphized:
- Scans all call sites for concrete type arguments
- Generates specialized copies (e.g., `identity__i64`, `identity__bool`)
- Replaces generic call sites with calls to specialized versions
- Handles generic methods on structs (mangled as `StructName__method__T1_T2`)

---

## 2. Stage Details

### 2.1 Lexer (`lexer.rs`, `token.rs`)

Reads source text character-by-character and produces a `Vec<Token>`.

**Key responsibilities:**
- Tokenize all Pledge keywords (`pf`, `pv`, `pc`, `ret`, `if`, `else`, `for`, `while`, `match`, etc.)
- Parse integer literals (decimal, hex `0xFF`, binary `0b1010`, octal `0o777`, underscores `1_000`)
- Parse float literals (`3.14`, `3.14f32`)
- Parse string literals (`"hello"`)
- Parse f-string literals (`f"hello {name}!"`) via `read_fstring()` with brace-depth tracking
- Tokenize operators (`+`, `-`, `*`, `/`, `%`, `==`, `!=`, `<=`, `>=`, `&&`, `||`, `&`, `|`, `^`, `~`, `<<`, `>>`, `..`, `..=`, `->`, `=>`, `::`, `?`, `#`)
- Skip comments (`//` line, `/* */` block)
- Track line/column for error messages

**Output:** `Vec<Token>` where `Token { kind: TokenKind, line: usize, col: usize }`

### 2.2 Parser (`parser.rs`, `ast.rs`)

Recursive descent parser. Consumes tokens and builds an AST (`Program`).

**AST structure:**
```
Program
├── functions: Vec<Func>
├── structs: Vec<StructDef>
├── enums: Vec<EnumDef>
├── impls: Vec<ImplDef>
├── type_aliases: Vec<TypeAlias>
├── consts: Vec<ConstDef>
├── imports: Vec<ImportDef>
└── attributes: Vec<Attribute>

Func
├── name, params, ret_type, body: Vec<Stmt>, type_params

Stmt
├── Let { name, ty, init, is_mut }
├── Assign { target, op, value }
├── Ret { expr }
├── If { cond, then_body, else_body }
├── While { cond, body }
├── For { var, iterator, body }
├── Match { expr, arms }
├── Expr(expr)
├── Defer { body }
├── Break, Continue
└── Use { path, items }

Expr
├── IntLit, FloatLit, StrLit, BoolLit
├── FString { parts: Vec<FStringPart> }
├── Ident, BinOp, UnaryOp
├── Call { func, args }
├── MethodCall { receiver, method, args }
├── StructLit { name, fields }
├── FieldAccess { expr, field }
├── ArrayLit, Index
├── Ref { expr, is_mut }, Deref { expr }
├── Closure { params, body, ret_ty }
├── TupleLit
├── Range { start, end, inclusive }
├── Block(Vec<Stmt>)
├── EnumLit { enum_name, variant, args }
└── Try { expr }
```

### 2.3 Type Checker (`typechecker.rs`)

Validates the AST before lowering. Collects all errors and reports them at once.

**Key responsibilities:**
- Type inference for `pv` bindings without explicit type
- Function call argument type matching
- Binary operation type compatibility (arithmetic, comparison, logical, bitwise)
- Struct literal field validation
- Field access on valid struct types
- Return statement type matching
- Pattern match arm type consistency
- Constant resolution (`pc` constants)
- Type alias resolution (`ty`)
- Generic type parameter registration (simplified)

**Output:** Either `Ok(())` or `Err(Vec<String>)` with all error messages.

### 2.4 IR Lowering (`lower.rs`)

Lowers the type-checked AST to typed SSA IR.

**SSA IR structure (`ir.rs`):**
```
IrModule
├── functions: Vec<IrFunction>
└── externs: Vec<ExternFunc>

IrFunction
├── name, params, ret_ty
├── blocks: Vec<BasicBlock>
├── values: HashMap<ValueId, ValueMeta>
└── allocas: Vec<AllocaInst>

BasicBlock
├── id: BlockId
├── insts: Vec<Inst>
└── terminator: Terminator

Inst
├── Const { value, ty }
├── ConstStr { value }
├── BinOp { op, lhs, rhs }
├── UnaryOp { op, operand }
├── Call { func, args, ret_ty }
├── Load { ptr, ty }
├── Store { ptr, value }
├── Alloca { ty }
├── GetElemPtr { array_ptr, index, elem_ty }
├── GetFieldPtr { struct_ptr, field_index, field_ty }
├── StructConstruct { fields }
├── StructField { struct_val, field_idx }
├── TryUnwrap { value, ok_block, err_block }
├── Jump { target }
├── Branch { cond, then, else_ }
└── ...

Terminator
├── Br { target }
├── CondBr { cond, then_block, else_block }
├── Ret { value }
└── Panic { msg }
```

**Key lowering decisions:**
- **Mutable variables** → `alloca` + `load`/`store` (not SSA renaming)
- **`alloca_types` tracking** → `HashMap<ValueId, IrType>` maps each alloca to its IR type, enabling correct `Load` types (not hardcoded `I64`)
- **For loops (range)** → `alloca` for loop variable, comparison block, body block, increment, back-edge
- **For loops (array iterator)** → index-based while loop: `alloca` for index counter, `GetElemPtr` + `Load` for element access, handles both direct array and pointer-to-array (from mutable variable loads)
- **While loops** → condition block, body block, back-edge
- **If/else** → `CondBr` to then/else blocks, merge block
- **Defer blocks** → `defer_stack: Vec<Vec<Stmt>>` tracks deferred statements; `emit_defers` drains and emits them before `Ret` instructions
- **F-strings** → `ConstStr` for literals, `__pledge_to_str` for non-string expressions, `__pledge_str_concat` to join all parts
- **Constants** → `Inst::Const` with value inlined
- **Array literals** → `Alloca` for array, `GetElemPtr` + `Store` per element, returns array pointer
- **Array indexing** → `GetElemPtr` with element type from array type, `Load` with correct element type
- **Array index assignment** → `GetElemPtr` + `Store` (with compound assignment via `Load` + `BinOp` + `Store`)
- **Method calls** → Name mangling (`__pledge_StructName_method`), receiver as first arg, SRET for struct returns
- **Match expressions** → `lower_match_expr` with result slot `alloca`, pattern matching branches, merge block
- **Match patterns** → `IntLit` (equality), `StrLit` (string comparison), `Range` (bounds check), `EnumVariant` (discriminant + bindings), `Wildcard`, `Ident` (binding)
- **Tuple literals** → `Alloca` for tuple struct, `GetFieldPtr` + `Store` per element
- **Enum literals** → `Alloca` for enum struct, stores discriminant + payload
- **Try operator (`?`)** → `TryUnwrap` with ok/err blocks, early return on error
- **Closures** → Lambda lifting: creates new `IrFunction` with mangled name (no environment capture yet)
- **Compound assignment** → Uses `get_value_type()` / `alloca_types` for correct `BinOp` type instead of hardcoded `I64`
- **Loop stack** → 3-tuple `(cond_block, exit_block, Option<ValueId>)` for loop-as-expression break values
- **Runtime functions** registered as externs: `__pledge_print_int`, `__pledge_print_str`, `__pledge_print_bool`, `__pledge_to_str`, `__pledge_str_concat`, `__pledge_panic`

### 2.5 IR Interpreter (`interp.rs`)

Executes SSA IR directly. Used by the `run` command for fast iteration.

**Value types:**
```
Value
├── Int(i64)
├── Float(f64)
├── Bool(bool)
├── Str(String)
├── Ptr(usize)
└── Unit
```

**Execution model:**
- Walks basic blocks following terminators
- Evaluates instructions and maintains a `HashMap<ValueId, Value>` per function
- Handles `Alloca` by allocating stack slots in a `HashMap<usize, Value>`
- Runtime functions are intercepted in `call_runtime()`:
  - `print` → dispatches based on value type to `__pledge_print_int/str/bool`
  - `__pledge_to_str` → converts `Int`/`Bool`/`Float` to `Str`; `Str` passes through
  - `__pledge_str_concat` → concatenates two string values
- Returns `RunResult::Return(Value)` or `RunResult::Panic(String)`

### 2.6 PE Emitter (`pe.rs`) — Direct Native Code Generation

The **PE emitter** is the primary native backend. It encodes x86_64 instructions directly to machine code bytes and constructs a valid PE file — no assembler, linker, or C compiler required.

**Pipeline:** `IrModule` → machine code bytes → PE file → `.exe`

**Code generation:**

1. **Instruction encoding**: Full x86_64 encoder with REX prefixes, ModR/M + SIB bytes, displacement (8-bit/32-bit), and immediates
2. **Register allocation**: Stack-based alloca model — every IR value gets a stack slot at a fixed `rbp` offset; registers used as scratch within instructions
3. **Windows x64 ABI**: RCX/RDX/R8/R9 for first 4 args, 32-byte shadow space, 16-byte stack alignment before calls, RAX for return values
4. **PE file construction**: DOS header, PE header, section headers (.text + .rdata), import directory, IAT for `kernel32.dll` functions
5. **Import resolution**: `GetStdHandle`, `WriteFile`, `ExitProcess` imported from `kernel32.dll` via IAT with RIP-relative addressing
6. **Runtime stubs** (generated inline as machine code):
   - `p_print_int`: Integer to string (div/mod loop, reverse fill), `WriteFile` to stdout, newline
   - `p_print_str`: String + newline to stdout via `WriteFile`
   - `p_print_bool`: "true"/"false" to stdout
   - `p_print_float`: (stub, prints truncated int)
   - `p_panic`: Prints message + `ExitProcess`
   - `p_malloc` / `p_free`: Heap allocation (stubs)
7. **Control flow**: Conditional jumps (`jcc`), unconditional jumps (`jmp`), label-based relocation with deferred resolution
8. **Arithmetic**: Integer add/sub/mul/div/mod, float add/sub/mul/div, comparisons, bitwise and/or/xor/shl/shr
9. **Stack frame management**: `push rbp` / `mov rbp, rsp` / `sub rsp, N` prologues, pre-allocated alloca slots
10. **Non-volatile registers**: `r12`/`r13` saved/restored in runtime stubs
11. **Entry point**: Calls `main`, passes exit code to `ExitProcess`
12. **Read-only data**: String literals in `.rdata` section, referenced via RIP-relative addressing

**Instruction encoders** (selected):
- `mov_rr`, `mov_mem_disp_reg`, `mov_mem_disp_reg32` (32-bit store), `mov_r_imm`, `mov_r8_imm`
- `add_rr`, `sub_rr`, `imul_rr`, `xor_rr`, `and_rr`, `or_rr`
- `cmp_rr`, `test_rr`, `jcc_label`, `call_label`, `call_reg`, `call_rip`
- `ret`, `push`, `pop`, `lea_rbp`, `sub_rsp_i32`, `add_rsp_i32`
- `mov_mem_reg8` (byte store for digit conversion)

### 2.7 x86_64 MASM Code Generator (`x86.rs`)

Generates MASM-compatible x86_64 assembly from SSA IR.

**Output format:** MASM (Microsoft Macro Assembler) syntax for `ml64.exe`

**Code generation:**

1. **Data section** (`.data`):
   - String literals as null-terminated byte arrays
   - `str_concat_buf` and `str_concat_tmp` — 4096-byte static buffers for string concatenation
   - `BYTE 4096 DUP(?)` for uninitialized data

2. **Code section** (`.code`):
   - User functions: prologue (`push rbp` / `mov rbp, rsp` / `sub rsp, N`), body, epilogue
   - Stack frame size: pre-computed from alloca count + shadow space + alignment
   - Alloca slots: pre-allocated at fixed offsets from `rbp`

3. **Calling convention** (Windows x64):
   - First 4 args: RCX, RDX, R8, R9
   - Additional args: stack
   - 32-byte shadow space allocated for each call
   - 16-byte stack alignment maintained before calls
   - Return value in RAX

4. **Runtime stubs** (generated inline):
   - `__pledge_print_int`: Converts integer to string in stack buffer, calls `WriteFile`
   - `__pledge_print_str`: Calls `WriteFile` with string pointer
   - `__pledge_print_bool`: Prints "true" or "false"
   - `__pledge_to_str`: Converts integer to null-terminated string in stack buffer (reverse fill)
   - `__pledge_str_concat`: Copies left string to temp buffer, then both to output buffer (avoids aliasing)
   - `__pledge_panic`: Prints message, calls `ExitProcess`

5. **Non-volatile registers**: `r12`, `r13` saved/restored in runtime stubs (Windows x64 ABI)

6. **External imports**: `GetStdHandle`, `WriteFile`, `ExitProcess` from `kernel32.lib`

7. **Entry point**: Both `main` and `_main` exported

**MASM-specific notes:**
- `mov reg, OFFSET label` for static data addressing (not `lea reg, [rip + label]` — MASM doesn't use `rip` keyword)
- `BYTE N DUP(?)` for uninitialized data (not `DUP(0)`)
- `EXTERN` declarations for kernel32 functions

### 2.8 C Code Generator (`codegen_c.rs`)

Translates SSA IR to portable C99 code that compiles with MSVC (`cl.exe`), GCC, or Clang.

**Output format:** C99 source code (`.c` file)

**Code generation:**

1. **Runtime header** (`#include` directives, `#undef` for `max`/`min`/`abs` to avoid MSVC macro conflicts, struct typedefs for `PVec`/`PMap`, runtime function declarations)
2. **String constants**: Emitted as `static const char str_N[] = "...";`
3. **Struct definitions**: Emitted as C `struct` typedefs with field layout
4. **Function prototypes**: All IR functions declared with renamed C names (`main` → `main_entry`, `abs` → `p_abs`, etc.)
5. **Function bodies**: SSA values declared as `int64_t vN;` at function start; instructions emitted as assignments; basic blocks as C labels with `goto` for control flow
6. **Polymorphic `print` dispatch**: `print` calls intercepted and dispatched to `p_print_str`/`p_print_int`/`p_print_bool`/`p_print_float` based on argument IR type, followed by `p_print_newline()`
7. **Runtime implementations**: `p_print*`, `p_panic`, `p_malloc`, `p_free`, `p_str_concat`, `p_to_str`, Vec/HashMap/String operations
8. **Main wrapper**: C `int main() { return (int)main_entry(); }`

**Function renaming:**
- `main` → `main_entry` (avoids conflict with C `main` wrapper)
- `abs` → `p_abs`, `max` → `p_max`, `min` → `p_min` (avoids MSVC intrinsic/macro conflicts)

**MSVC integration (`main.rs`):**
- `find_cl()`: Searches Visual Studio paths for `cl.exe`
- `find_vcvarsall()`: Searches for `vcvarsall.bat` to set up MSVC environment
- `ccompile` command: Generates `.c` file, invokes `cl.exe` via `vcvarsall.bat`, produces `.exe`

### 2.9 Build Pipeline (`main.rs`)

The `emit` command is the primary native backend — it requires zero external tools:

1. `compiler::emit_source_with_path(source, path)` → PE bytes (`Vec<u8>`)
2. Write bytes directly to `.exe` file
3. Print "Emitted native executable: <exe> (<size> bytes, no ml64.exe/link.exe)"

The `build` command orchestrates external tools (MASM pipeline):

1. `compiler::build_source(source)` → assembly string
2. Write `.asm` file
3. Find `ml64.exe` (searches Visual Studio paths)
4. Run `ml64.exe /nologo /c /Fo<obj> <asm>`
5. Find `link.exe` and `kernel32.lib`
6. Run `link.exe /SUBSYSTEM:CONSOLE /ENTRY:_main /LIBPATH:<path> <obj> kernel32.lib /OUT:<exe>`
7. Print "Built native executable: <exe>"

**Tool discovery:**
- `find_ml64()`: Searches `C:\Program Files\Microsoft Visual Studio\` and `C:\Program Files (x86)\Microsoft Visual Studio\`
- `find_link()`: Same search paths
- `find_kernel32_lib()`: Searches for `kernel32.lib` in Windows SDK directories

---

## 3. Source File Map

| File | Responsibility |
|------|---------------|
| `src/main.rs` | CLI entry point, command dispatch, external tool invocation |
| `src/token.rs` | `TokenKind` enum (all Pledge keywords and operators) |
| `src/lexer.rs` | Tokenizer: source text → `Vec<Token>` |
| `src/ast.rs` | AST node definitions: `Program`, `Func`, `Stmt`, `Expr`, `StructDef`, `EnumDef`, etc. |
| `src/parser.rs` | Recursive descent parser: `Vec<Token>` → `Program` |
| `src/typechecker.rs` | Type validation: `Program` → `Ok` or `Err(Vec<String>)` |
| `src/ir.rs` | SSA IR definitions: `IrModule`, `IrFunction`, `BasicBlock`, `Inst`, `IrType`, `ValueId` |
| `src/lower.rs` | AST → SSA IR lowering: `Program` → `IrModule` |
| `src/interp.rs` | IR interpreter: `IrModule` → execution result |
| `src/pe.rs` | **Direct PE emitter**: `IrModule` → x86_64 machine code → PE `.exe` (no external tools) |
| `src/x86.rs` | x86_64 MASM codegen: `IrModule` → assembly string (for `build` command) |
| `src/codegen_c.rs` | C codegen backend: `IrModule` → C99 source code (for `ccompile` command) |
| `src/codegen.rs` | Legacy ARM64 codegen: `Program` → assembly string (unused) |
| `src/mono.rs` | Generic monomorphization: specializes generic functions per call site |
| `src/compiler.rs` | Pipeline orchestration, module resolution, stdlib auto-include |
| `stdlib/core.p` | Core standard library (println, assert, assert_eq) |
| `tests/*.p` | Test programs (hello, struct, struct_return, features2-6, control_flow, functions, strings) |

---

## 4. Data Flow Example

For `tests/strings.p`:

```pledge
pf main() {
    print("Hello, World!");
    pv name = "Pledge";
    print(name);
    pv greeting = f"Hello, {name}!";
    print(greeting);
    pv x = 42;
    print(f"x = {x}");
}
```

1. **Lexer** produces tokens: `Keyword(Pf)`, `Ident("main")`, `LParen`, `RParen`, `LBrace`, `Ident("print")`, `LParen`, `StrLit("Hello, World!")`, `RParen`, `Semicolon`, `Keyword(Pv)`, `Ident("name")`, `Assign`, `StrLit("Pledge")`, `Semicolon`, ...

2. **Parser** builds AST with `Func { name: "main", body: [Expr(Call(print, "Hello, World!")), Let(name, "Pledge"), Expr(Call(print, Ident(name))), Let(greeting, FString([Literal("Hello, "), Expr(Ident(name)), Literal("!")])), ...] }`

3. **TypeChecker** validates: `print` takes `str`, `name` is `str`, f-string parts are valid, etc.

4. **Lowerer** produces IR:
   - `ConstStr("Hello, World!")` → `Call(__pledge_print_str, [str])`
   - `ConstStr("Pledge")` → `Store(alloca_name, str)`
   - `Load(alloca_name)` → `Call(__pledge_print_str, [str])`
   - `ConstStr("Hello, ")` + `Load(alloca_name)` (already Ptr, skip `__pledge_to_str`) + `Call(__pledge_str_concat, [prev, name])` + `ConstStr("!")` + `Call(__pledge_str_concat, [prev, "!"])` → `Call(__pledge_print_str, [result])`
   - `Const(42)` → `Call(__pledge_to_str, [42])` → concat with `ConstStr("x = ")` → `Call(__pledge_print_str, [result])`

5. **Interpreter** executes: prints "Hello, World!", "Pledge", "Hello, Pledge!", "x = 42"

6. **x86 Codegen** generates MASM assembly with:
   - String literals in `.data`
   - `main` function with prologue, calls to runtime stubs, epilogue
   - Runtime stubs for `__pledge_print_str`, `__pledge_to_str`, `__pledge_str_concat`
   - `EXTERN GetStdHandle`, `WriteFile`, `ExitProcess`

7. **ml64.exe** assembles `.asm` → `.obj`

8. **link.exe** links `.obj` + `kernel32.lib` → `.exe`

9. **.exe** runs and produces the same output as the interpreter

> **PE emitter path:** Steps 6-8 are replaced by `pe.rs` which encodes x86_64 instructions directly to machine code bytes and constructs a PE file — no `ml64.exe` or `link.exe` needed.
>
> **Alternative path (C codegen):** Step 6 can be replaced by `codegen_c.rs` which generates a `.c` file, then `cl.exe`/`gcc`/`clang` compiles it directly to a `.exe`. The output is identical.
