# Fides Language — Feature Status

*Last updated: verified against actual source code and test runs (137/137 unit tests, 8/8 harness tests, 99/99 interpreter tests + 12/12 expected-fail, 99/99 C codegen — FULL PARITY, 111 .fd test files)*

## Legend

- ✅ **Working** — Fully functional and tested
- ⚠️ **Partially Working** — Functions but has known bugs or limitations
- ❌ **Not Working** — Implemented but broken, or not yet implemented

---

## Compiler Pipeline

| Feature | Status | Notes |
|---------|--------|-------|
| Lexer (token.rs) | ✅ | All Fides keywords (pf, pv, pc, ret, etc.) |
| Parser (parser.rs) | ✅ | Produces AST from tokens |
| Type Checker (typechecker.rs) | ✅ | Validates types, capabilities, effects, regions, traits |
| IR Lowering (lower.rs) | ✅ | AST → typed SSA IR with effects/regions |
| IR Interpreter (interp.rs) | ✅ | Runs IR directly via `fidesc run` |
| PE Emitter (pe.rs) | ✅ | Generates native x64 PE executables + flat binary (bare-metal mode) |
| ELF64 Emitter (elf64.rs) | ✅ | Generates native ELF64 executables on Linux (zero external tools) + relocatable objects |
| C Codegen (codegen_c.rs) | ✅ | Generates C code, 99/99 tests match interpreter output (FULL PARITY — no gaps) |
| PIR (pir.rs) | ✅ | PIR L1/L2/L3, `fidesc pir` command |
| Optimizer (opt.rs) | ⚠️ | Const fold, CSE, DCE, inline, unroll, TCO, effect-based elision. Works for simple programs but exposes register allocator bug on complex ones. Not enabled by default. |
| Peephole (peephole.rs) | ✅ | 9 passes, always on in `build` command: redundant moves, zero idiom, nop arithmetic, branch threading, dead store, redundant test, store-to-load forwarding, LEA coalescing, cmp+setcc+jcc instruction selection |
| Register Allocator (regalloc.rs) | ⚠️ | Linear scan allocator. Has pre-existing bug: constants not loaded into registers before use can be clobbered by intervening instructions. Exposed by L2 optimizer's instruction reordering. |
| Mach-O Emitter (macho.rs) | ✅ | Generates Mach-O object files for macOS |
| COFF Emitter (coff.rs) | ✅ | Generates COFF object files for Windows |
| WASM Backend (wasm.rs) | ✅ | PIR → WASM binary, `fidesc wasm` command |
| GPU/SPIR-V Backend (gpu.rs) | ✅ | PIR → SPIR-V binary, `fidesc gpu` command |
| Comptime (comptime.rs) | ✅ | Compile-time evaluation, reflection, f-string interpolation |
| Multi-arch Codegen | ✅ | x86-64, ARM64, RISC-V assembly via `fidesc codegen --target` |

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
| `fidesc build <file> -o <out> [--target] [--opt]` | ✅ | Native build: PIR → codegen → peephole → ELF64 executable (zero external tools on Linux) |
| `fidesc ccompile <file> -o <exe>` | ✅ | C codegen → gcc/clang → executable |
| `fidesc test` | ✅ | Runs all 99 normal + 12 expected-fail tests across interpreter, PE emitter, and C codegen |
| `fidesc repl` | ✅ | Interactive REPL |
| `fidesc verify <file>` | ✅ | IR verification pass |
| `fidesc sourcemap <file>` | ✅ | Source map output |
| `fidesc pir <file> [1\|2\|3\|all] [--opt]` | ✅ | PIR output (L1/L2/L3) with optional optimization |
| `fidesc opt <file> [passes]` | ✅ | Run PIR optimizer, print statistics |
| `fidesc codegen <file> --target <arch>` | ✅ | x86-64, ARM64, or RISC-V assembly output |
| `fidesc codegen <file> --elf\|--macho\|--coff <o>` | ✅ | Emit ELF64, Mach-O, or COFF object files |
| `fidesc wasm <file> -o <out.wasm>` | ✅ | WebAssembly backend |
| `fidesc gpu <file>` | ✅ | SPIR-V GPU backend |
| `fidesc parse-header <file.h>` | ✅ | Parse C header → generate Fides FFI bindings |
| `fidesc watch <file>` | ✅ | Hot reload: native JIT (ELF64) + function-level change detection |
| `fidesc link <files...>` | ✅ | Link multiple object files |

## Language Features — Interpreter (`fidesc run`)

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
| Region blocks `region r { }` | ✅ | Compile-time region tracking, use-after-free detection |
| Region escape detection | ✅ | Compile-time dangling pointer detection |
| Region inference | ✅ | Auto-detect regions via region_stack tracking in typechecker |

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
| Capability grants | ✅ | `use ... with { IO, FS, Net, Spawn }` |
| Capability denial | ✅ | Compile-time error if not granted |
| Transitive capabilities | ✅ | Propagation through call graph |
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
| `async` / `await` | ❌ | Parsed, not yet implemented (Phase 4+) |
| `spawn` | ❌ | Parsed, not yet implemented |
| Effect handlers `handle { do } with` | ⚠️ | Syntax parses and runs; full continuation semantics pending |
| Effect annotations `!IO`, `!{}` | ✅ | Compile-time effect inference and checking |
| Effect propagation | ✅ | Fixpoint iteration over call graph |

### Safety (Phase 2)

| Feature | Status | Notes |
|---------|--------|-------|
| Bounds checking (Vec/array/string) | ✅ | Runtime in debug mode, compile-time for constant indices |
| Integer overflow checking | ✅ | Debug mode panics, release mode wraps (`--release`) |
| Division by zero detection | ✅ | Runtime panic in debug mode |
| `Option::unwrap` panics on None | ✅ | Calls `p_panic` |
| Effect system (annotations + inference) | ✅ | `!IO`, `!Net`, `!Spawn`, `!Alloc`, `!{}` for pure |
| Effect checking (under-declaration) | ✅ | Compile-time error if inferred > declared |
| Region use-after-free detection | ✅ | Compile-time via `freed_regions` tracking |
| Region escape detection | ✅ | Compile-time via `region_stack` tracking |
| Capability elision | ✅ | Zero runtime overhead, all checks compile-time |

### Advanced

| Feature | Status | Notes |
|---------|--------|-------|
| `comptime` blocks | ✅ | `comptime <expr>`, `comptime { block }`, reflection |
| `comptime` reflection | ✅ | type_name, is_struct, is_enum, field_names, field_types, enum_variants |
| `comptime` functions | ✅ | `comptime pf func()` — evaluated at compile time |
| Match guards | ✅ | `pattern if cond => body` |
| Match range patterns | ✅ | `1..=5 => body` |
| Trait associated types | ✅ | `ty Item;` in trait, `ty Item = T;` in impl |
| Trait default methods | ✅ | Default body in trait, inherited by impls |
| Trait objects (`dyn Trait`) | ✅ | Dynamic dispatch via vtable |
| Trait impl verification | ✅ | Check all required methods implemented |
| Type aliases (`ty`) | ✅ | `ty X = T` — parsed, typechecked, lowered |
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
| Hot reload | ✅ | `fidesc watch` compiles to native ELF64, runs, detects function-level changes via L2 IR hashing |
| `#[test]` attribute | ❌ | Parsed but not executed by `fidesc test` |
| `#[derive]` | ❌ | Not yet implemented |

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
pv result = x |> square |> add_one |> print;
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
        ret matmul(x, self.w) + self.b;
    }
}

model MLP {
    l1: Dense(784, 128),
    l2: Dense(128, 10),
    forward(self, x: Tensor) -> Tensor {
        ret x |> self.l1.forward |> relu |> self.l2.forward;
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
pv w: Tensor<f32, [784, 128]> = ...;
pv x: Tensor<f32, [batch, 784]> = ...;
pv y: Tensor<f32, [batch, 128]> = matmul(x, w);  // shape-checked at compile time
```

### Autograd Block

| Feature | Status | Notes |
|---------|--------|-------|
| `grad(var) { ... }` syntax | ✅ | Compile-time reverse-mode automatic differentiation |
| `backward(loss)` statement | ✅ | Triggers reverse-mode AD within a grad block |
| Tape-based gradient computation | ✅ | `src/autograd.rs` — traces operations and builds backward computation graph |
| Compilation pipeline integration | ✅ | Autograd pass hooked into compilation pipeline |

**Syntax example:**
```p
grad(w) {
    pv loss = compute_loss(w);
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
pf vector_add(a: &Tensor, b: &Tensor, c: &mut Tensor, n: i64) {
    pv i = thread_id(0);
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

pv w: Tensor = ...;
pv q_w = w ~> q8;                    // quantize weights to 8-bit
pv q_x = x ~> q8;                    // quantize activations
pv q_out = quant::qmatmul(q_x, q_w); // quantized matmul
pv out = quant::dequantize_q8(q_out); // dequantize result
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
| `@audit` | ✅ Partial | Entry/exit logging via IR injection. Full trail (timestamps, params, crypto) pending |
| `txn {}` | ✅ Partial | Parsing + typechecking done. Interpreter rollback (memory snapshot) pending |

### Financial Safety Stdlib

| Module | Status | Description |
|--------|--------|-------------|
| `stdlib/money.fd` | ✅ DONE | Fixed-point i64 cents, @linear struct, arithmetic, formatting |
| `stdlib/banking.fd` | ✅ DONE | Account, deposit, withdraw, transfer, interest, double-credit/lost-money detection |
| `stdlib/file.fd` | ✅ DONE | File I/O: read, write, append, exists |
| `stdlib/time.fd` | ✅ DONE | LogicalTime: timezone-free, clock-free, @deterministic date math |
| `stdlib/alloc.fd` | ✅ DONE | Heap allocator builtins (malloc, realloc, free) |
| `stdlib/privacy.fd` | ✅ DONE | Field-level privacy test module |

### Language Features Added in Phase 7.5

| Feature | Syntax | Status |
|---------|--------|--------|
| Tuple destructuring | `pv (a, b) = expr` | ✅ DONE |
| Loop-aware linear types | Linear vars can't be consumed in loops | ✅ DONE |
| `txn {}` blocks | `txn { ... }` | ✅ Partial (parsing+typechecking) |

---

## Summary

| Component | Working | Partial | Broken |
|-----------|---------|---------|--------|
| Lexer | 100% | — | — |
| Parser | 100% | — | — |
| Type Checker | 100% | — | — |
| IR Lowering | 100% | — | — |
| Interpreter | 99/99 + 12/12 expected-fail | — | `test_trait_verify_fail`, `test_budget_fail`, `test_sandbox_fail` (expected failures) |
| PE Emitter | 99/99 + 12/12 expected-fail | — | `test_trait_verify_fail`, `test_budget_fail`, `test_sandbox_fail` (expected failures) |
| ELF64 Emitter | ✅ | — | Working executables on Linux (zero external tools) |
| C Codegen | 99/99 + 12/12 expected-fail | — | FULL PARITY — no gaps. test_db no longer flaky (HashMap keys sorted). |
| Multi-arch Codegen | ✅ | — | x86-64, ARM64, RISC-V assembly all work |
| Peephole Optimizer | ✅ (9 passes) | — | Always on in `build` |
| L2 Optimizer | — | ⚠️ | Works for simple programs, regalloc bug on complex ones |
| Register Allocator | — | ⚠️ | Pre-existing bug with constant loading |
| Source Maps | ✅ | — | `sourcemap` command works |
| IR Verification | ✅ | — | `verify` command works |
| WASM Backend | ✅ | — | `wasm` command works |
| GPU/SPIR-V Backend | ✅ | — | `gpu` command works |

### Test Coverage

- **111 test files** in `tests/` (99 normal + 12 expected-fail)
- **137 unit tests** in `cargo test --bin fidesc` — all pass
- **8 cargo tests** in `tests/harness.rs` — all pass (including golden_files_match)
- **43 golden files** in `tests/golden/` — expected outputs for regression testing
- **Benchmarks** in `benches/bench_main.rs`
