# Pledge Language — Feature Status

*Last updated: Aug 10, 2026 (41/41 tests pass across all backends)*

## Legend

- ✅ **Working** — Fully functional and tested
- ⚠️ **Partially Working** — Functions but has known bugs or limitations
- ❌ **Not Working** — Implemented but broken, or not yet implemented

---

## Compiler Pipeline

| Feature | Status | Notes |
|---------|--------|-------|
| Lexer (token.rs) | ✅ | All Pledge keywords (pf, pv, pc, ret, etc.) |
| Parser (parser.rs) | ✅ | Produces AST from tokens |
| Type Checker (typechecker.rs) | ✅ | Validates types, capabilities |
| IR Lowering (lower.rs) | ✅ | AST → typed SSA IR with effects/regions |
| IR Interpreter (interp.rs) | ✅ | Runs IR directly via `pledgec run` |
| PE Emitter (pe.rs) | ✅ | Generates native x64 PE executables, all core features working |
| C Codegen (codegen_c.rs) | ✅ | Generates C code, 41/41 tests match interpreter output |

## CLI Commands

| Command | Status | Notes |
|---------|--------|-------|
| `pledgec version` | ✅ | Shows version info |
| `pledgec tokens <file>` | ✅ | Dumps lexer tokens |
| `pledgec ast <file>` | ✅ | Dumps parsed AST |
| `pledgec ir <file>` | ✅ | Dumps lowered IR |
| `pledgec run <file>` | ✅ | Interprets and runs program |
| `pledgec emit <file> -o <exe>` | ✅ | Emits native PE executable |
| `pledgec ccompile <file> -o <exe>` | ✅ | C codegen → gcc/clang → executable |
| `pledgec build <file> -o <exe>` | ✅ | MASM pipeline (requires ml64.exe) |
| `pledgec test` | ✅ | Runs all 41 tests across all backends |
| `pledgec repl` | ✅ | Interactive REPL |
| `pledgec verify <file>` | ✅ | IR verification pass |
| `pledgec sourcemap <file>` | ✅ | Source map output |

## Language Features — Interpreter (`pledgec run`)

### Core Syntax

| Feature | Status | Notes |
|---------|--------|-------|
| `pf` function declarations | ✅ | Named functions with params and return types |
| `pv` variable declarations | ✅ | Immutable bindings |
| `pv mut` mutable variables | ✅ | Mutable bindings |
| `pc` constants | ✅ | Compile-time constants |
| `ret` return statement | ✅ | Return with optional value |
| `if` / `else` conditionals | ✅ | Branching |
| `while` loops | ✅ | Conditional loops |
| `for x in iter` loops | ✅ | Iterator-based loops (Vec, range) |
| `break` / `continue` | ✅ | Loop control flow |
| Block expressions `{ }` | ✅ | Scoped blocks with values |
| Comments `//` and `/* */` | ✅ | Line and block comments |
| Semicolons `;` | ✅ | Statement terminators |

### Types

| Feature | Status | Notes |
|---------|--------|-------|
| `i64` / `i32` / `i8` integers | ✅ | Signed integers |
| `u64` / `u32` / `u8` unsigned | ✅ | Unsigned integers |
| `f64` / `f32` floats | ✅ | Floating point |
| `bool` | ✅ | Boolean type |
| `&str` / `String` | ✅ | String types |
| `Vec<T>` | ✅ | Dynamic arrays |
| `HashMap<K,V>` | ✅ | Hash maps |
| `Option<T>` | ✅ | Optional values (Some/None) |
| `Result<T,E>` | ✅ | Error handling (Ok/Err) |
| `struct` | ✅ | Struct definitions and instances |
| `enum` (algebraic) | ✅ | Sum types with data |
| Generics `<T>` | ✅ | Generic functions and types |
| Type aliases `ty` | ✅ | Type alias declarations |

### Expressions

| Feature | Status | Notes |
|---------|--------|-------|
| Arithmetic (`+`, `-`, `*`, `/`, `%`) | ✅ | Integer and float |
| Comparison (`<`, `>`, `==`, `!=`, `<=`, `>=`) | ✅ | All comparison operators |
| Logical (`&&`, `\|\|`, `!`) | ✅ | Boolean logic |
| Bitwise (`&`, `\|`, `^`, `<<`, `>>`) | ✅ | Bit operations |
| f-string interpolation `f"{expr}"` | ✅ | String formatting |
| Ranges `..` and `..=` | ✅ | Range expressions |
| Closures `pf(x) { }` / `pf(x) => expr` | ✅ | Anonymous functions |
| `match` expressions | ✅ | Int, string, enum, wildcard patterns; expression and statement forms |
| `?` operator | ✅ | Error propagation |
| Casts `as` | ✅ | Type casts |

### Control Flow

| Feature | Status | Notes |
|---------|--------|-------|
| `if` / `else if` / `else` | ✅ | Multi-branch conditionals |
| `while` | ✅ | While loops |
| `for` over Vec | ✅ | Iterate vector elements |
| `for` over range | ✅ | Iterate numeric ranges |
| `loop` | ✅ | Infinite loops |
| `break` / `continue` | ✅ | Loop control |
| `match` | ✅ | Int, string, enum, wildcard patterns; expression and statement forms |

### Functions & Closures

| Feature | Status | Notes |
|---------|--------|-------|
| Named functions `pf name() { }` | ✅ | |
| Function parameters | ✅ | Up to 4 register args, more on stack |
| Return values | ✅ | Single return value |
| Closures | ✅ | Captures environment |
| Generic functions `pf name<T>()` | ✅ | Type parameters |
| Default arguments | ❌ | Not implemented |
| Variadic arguments | ❌ | Not implemented |

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
| `&mut` references | ✅ | Mutable borrows |
| `defer` | ✅ | Go-style deferred cleanup |
| Region inference | ⚠️ | IR tracks regions, not fully enforced |

### Modules & Imports

| Feature | Status | Notes |
|---------|--------|-------|
| `use` imports | ✅ | Module imports |
| `pub` visibility | ✅ | Public/private visibility |
| `use ... as` aliases | ✅ | Import aliases |
| `use ... { }` selective imports | ✅ | Selective imports |
| `use ... *` glob imports | ✅ | Glob imports |
| Re-exports | ✅ | |
| Multi-file modules | ✅ | |
| Standard library (stdlib/core.pl) | ✅ | Auto-imported |

### Capabilities (Security)

| Feature | Status | Notes |
|---------|--------|-------|
| `cap` (capability/trait) declarations | ✅ | |
| Capability grants | ✅ | |
| Capability denial | ✅ | |
| Transitive capabilities | ✅ | |

### Error Handling

| Feature | Status | Notes |
|---------|--------|-------|
| `Result<T,E>` type | ✅ | |
| `?` operator | ✅ | Early return on Err |
| `match` on Result | ✅ | |
| `try` blocks | ✅ | `test_pe_try_block.p` |

### Concurrency

| Feature | Status | Notes |
|---------|--------|-------|
| `async` / `await` | ❌ | Not yet implemented (Phase 4+) |
| `spawn` | ❌ | Not yet implemented |
| Effect handlers | ❌ | Not yet implemented (Phase 4+) |

### Advanced

| Feature | Status | Notes |
|---------|--------|-------|
| `comptime` blocks | ❌ | Not yet implemented |
| Hot reload | ❌ | Not yet implemented |
| `#[test]` attribute | ❌ | Parsed but not executed |
| `#[derive]` | ❌ | Not yet implemented |

---

## PE Emitter (`pledgec emit`) — Native Code Generation

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
| `push` / `pop` / `ret` | ✅ | Stack operations |
| `xor` / `shl` / `inc` | ✅ | Bit and increment ops |
| `lea` (RIP-relative / RBP-relative) | ✅ | Address computation |
| `ud2` (trap) | ✅ | Safety trap |

### PE — Code Generation

| Feature | Status | Notes |
|---------|--------|-------|
| Function prologue/epilogue | ✅ | push rbp, mov rbp rsp, sub rsp, ret |
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
| `defer` | ✅ | `test_pe_defer.p` |
| `?` operator | ✅ | `test_pe_try_op.p` |
| References `&` / `&mut` | ✅ | `test_pe_refs.p` |
| `enum` / `Option` / `Result` | ✅ | `test_pe_enum.p` |
| `for` over Vec | ✅ | `test_pe_for_vec.p` |
| `struct` + `impl` methods | ✅ | `test_pe_struct.p` |
| Generics `<T>` | ✅ | `test_pe_generics.p` |
| Closures with captures | ✅ | `test_closure_capture.p` |
| `match` (int/string/enum/wildcard) | ✅ | `test_match_edge.p` |
| HashMap (insert/get/contains/remove/len) | ✅ | `test_hashmap.p` |
| `p_print_map` | ✅ | `test_print_map.p` |
| f-string with float | ✅ | `test_fstring_float.p` |
| `try { }` blocks | ✅ | `test_pe_try_block.p` |

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

## Summary

| Component | Working | Partial | Broken |
|-----------|---------|---------|--------|
| Lexer | 100% | — | — |
| Parser | 100% | — | — |
| Type Checker | 100% | — | — |
| IR Lowering | 100% | — | — |
| Interpreter | 100% (41/41) | — | async/spawn/comptime (not parsed) |
| PE Emitter | 100% (41/41) | — | — |
| C Codegen | 100% (41/41) | — | — |
| MASM Backend | ~90% | — | Not actively tested (requires Windows + MASM) |
| IR Optimizer | — | Constant folding exists | Not wired into pipeline |
| Source Maps | ✅ | Line tracking in AST | `sourcemap` command works |
| IR Verification | ✅ | — | `verify` command works |

### Test Coverage

- **41 test files** in `tests/` — all pass across interpreter, PE emitter, and C codegen
- **4 cargo tests** in `tests/harness.rs` — interpreter, PE, C codegen, golden files
- **Golden files** in `tests/golden/` — 41 expected outputs
- **3 regression tests** — closures, fstrings, collections
- **Benchmarks** in `benches/bench_main.rs`
