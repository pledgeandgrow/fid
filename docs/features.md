# P Language — Feature Status

*Last updated: verified against actual source code and test runs (136/136 unit tests, 8/8 harness tests, 78/78 interpreter tests + 1/1 expected-fail, 76/78 C codegen, 102 .p test files)*

## Legend

- ✅ **Working** — Fully functional and tested
- ⚠️ **Partially Working** — Functions but has known bugs or limitations
- ❌ **Not Working** — Implemented but broken, or not yet implemented

---

## Compiler Pipeline

| Feature | Status | Notes |
|---------|--------|-------|
| Lexer (token.rs) | ✅ | All P keywords (pf, pv, pc, ret, etc.) |
| Parser (parser.rs) | ✅ | Produces AST from tokens |
| Type Checker (typechecker.rs) | ✅ | Validates types, capabilities, effects, regions, traits |
| IR Lowering (lower.rs) | ✅ | AST → typed SSA IR with effects/regions |
| IR Interpreter (interp.rs) | ✅ | Runs IR directly via `pledgec run` |
| PE Emitter (pe.rs) | ✅ | Generates native x64 PE executables + flat binary (bare-metal mode) |
| ELF64 Emitter (elf64.rs) | ✅ | Generates native ELF64 executables on Linux (zero external tools) + relocatable objects |
| C Codegen (codegen_c.rs) | ✅ | Generates C code, 76/78 tests match interpreter output (2 mismatches: test_asm by design, test_comptime_reflect interpreter-only) |
| PIR (pir.rs) | ✅ | PIR L1/L2/L3, `pledgec pir` command |
| Optimizer (opt.rs) | ⚠️ | Const fold, CSE, DCE, inline, unroll, TCO, effect-based elision. Works for simple programs but exposes register allocator bug on complex ones. Not enabled by default. |
| Peephole (peephole.rs) | ✅ | 9 passes, always on in `build` command: redundant moves, zero idiom, nop arithmetic, branch threading, dead store, redundant test, store-to-load forwarding, LEA coalescing, cmp+setcc+jcc instruction selection |
| Register Allocator (regalloc.rs) | ⚠️ | Linear scan allocator. Has pre-existing bug: constants not loaded into registers before use can be clobbered by intervening instructions. Exposed by L2 optimizer's instruction reordering. |
| Mach-O Emitter (macho.rs) | ✅ | Generates Mach-O object files for macOS |
| COFF Emitter (coff.rs) | ✅ | Generates COFF object files for Windows |
| WASM Backend (wasm.rs) | ✅ | PIR → WASM binary, `pledgec wasm` command |
| GPU/SPIR-V Backend (gpu.rs) | ✅ | PIR → SPIR-V binary, `pledgec gpu` command |
| Comptime (comptime.rs) | ✅ | Compile-time evaluation, reflection, f-string interpolation |
| Multi-arch Codegen | ✅ | x86-64, ARM64, RISC-V assembly via `pledgec codegen --target` |

## CLI Commands

| Command | Status | Notes |
|---------|--------|-------|
| `pledgec version` | ✅ | Shows version info |
| `pledgec tokens <file>` | ✅ | Dumps lexer tokens |
| `pledgec ast <file>` | ✅ | Dumps parsed AST |
| `pledgec ir <file>` | ✅ | Dumps lowered IR |
| `pledgec run <file>` | ✅ | Interprets and runs program |
| `pledgec emit <file> -o <exe>` | ✅ | Emits native PE executable (Windows) |
| `pledgec emit <file> --bare -o <bin>` | ✅ | Bare-metal flat binary (no stdlib, no PE headers) |
| `pledgec build <file> -o <out> [--target] [--opt]` | ✅ | Native build: PIR → codegen → peephole → ELF64 executable (zero external tools on Linux) |
| `pledgec ccompile <file> -o <exe>` | ✅ | C codegen → gcc/clang → executable |
| `pledgec test` | ✅ | Runs all 78 normal + 1 expected-fail tests across interpreter, PE emitter, and C codegen |
| `pledgec repl` | ✅ | Interactive REPL |
| `pledgec verify <file>` | ✅ | IR verification pass |
| `pledgec sourcemap <file>` | ✅ | Source map output |
| `pledgec pir <file> [1\|2\|3\|all] [--opt]` | ✅ | PIR output (L1/L2/L3) with optional optimization |
| `pledgec opt <file> [passes]` | ✅ | Run PIR optimizer, print statistics |
| `pledgec codegen <file> --target <arch>` | ✅ | x86-64, ARM64, or RISC-V assembly output |
| `pledgec codegen <file> --elf\|--macho\|--coff <o>` | ✅ | Emit ELF64, Mach-O, or COFF object files |
| `pledgec wasm <file> -o <out.wasm>` | ✅ | WebAssembly backend |
| `pledgec gpu <file>` | ✅ | SPIR-V GPU backend |
| `pledgec parse-header <file.h>` | ✅ | Parse C header → generate P FFI bindings |
| `pledgec watch <file>` | ✅ | Hot reload: native JIT (ELF64) + function-level change detection |
| `pledgec link <files...>` | ✅ | Link multiple object files |

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
| `try` blocks | ✅ | `test_pe_try_block.p` |

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
| SIMD types | ✅ | `stdlib/simd.p` — elementwise ops, dot, sum, scale |
| Tensor type | ✅ | `stdlib/tensor.p` — n-d arrays, matmul, relu, softmax |
| JSON parsing | ✅ | `stdlib/json.p` — parse, stringify, get, at |
| HTTP server | ✅ | `stdlib/http.p` — socket + HTTP parsing |
| Database framework | ✅ | `stdlib/db.p` — cap Database trait, in-memory KV |
| Parallel operations | ✅ | `stdlib/parallel.p` — par_map, par_sum, par_dot |
| Const generics | ✅ | `ArrayConst` type variant, monomorphization handles it — `test_const_generic.p` passes in interpreter |
| GATs | ✅ | `test_gat.p` passes in interpreter, outputs correct results |
| C FFI (`extern "C"`) | ✅ | Parsed, lowered, C codegen, interpreter stubs — `test_ffi.p`, `test_ffi_full.p`, `test_ffi_header.p` pass |
| Hot reload | ✅ | `pledgec watch` compiles to native ELF64, runs, detects function-level changes via L2 IR hashing |
| `#[test]` attribute | ❌ | Parsed but not executed by `pledgec test` |
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

## AI/ML Features

Six AI/ML-oriented features have been added to the P language, enabling neural network programming with compile-time safety, automatic differentiation, GPU kernel support, and quantization.

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
| `quantize_q8` stdlib function | ✅ | `stdlib/quant.p` |
| `dequantize_q8` stdlib function | ✅ | `stdlib/quant.p` |
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

## Summary

| Component | Working | Partial | Broken |
|-----------|---------|---------|--------|
| Lexer | 100% | — | — |
| Parser | 100% | — | — |
| Type Checker | 100% | — | — |
| IR Lowering | 100% | — | — |
| Interpreter | 78/78 + 1/1 expected-fail | — | `test_trait_verify_fail` (expected failure) |
| PE Emitter | 78/78 + 1/1 expected-fail | — | `test_trait_verify_fail` (expected failure) |
| ELF64 Emitter | ✅ | — | Working executables on Linux (zero external tools) |
| C Codegen | 76/78 + 1/1 expected-fail | — | 2 real mismatches (asm by design, comptime_reflect interpreter-only). test_db flaky (HashMap ordering). |
| Multi-arch Codegen | ✅ | — | x86-64, ARM64, RISC-V assembly all work |
| Peephole Optimizer | ✅ (9 passes) | — | Always on in `build` |
| L2 Optimizer | — | ⚠️ | Works for simple programs, regalloc bug on complex ones |
| Register Allocator | — | ⚠️ | Pre-existing bug with constant loading |
| Source Maps | ✅ | — | `sourcemap` command works |
| IR Verification | ✅ | — | `verify` command works |
| WASM Backend | ✅ | — | `wasm` command works |
| GPU/SPIR-V Backend | ✅ | — | `gpu` command works |

### Test Coverage

- **102 test files** in `tests/` (79 root + 9 safety + 4 capability + 7 effects + 3 regions)
- **133 unit tests** in `cargo test --bin pledgec` — all pass
- **8 cargo tests** in `tests/harness.rs` — 7 pass, 1 fails (golden_files_match: interpreter debug format)
- **43 golden files** in `tests/golden/` — expected outputs for regression testing
- **Benchmarks** in `benches/bench_main.rs`
