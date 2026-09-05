# Fides Language Guide

> **A comprehensive guide to how the Fides language works — from source code
> to executable binary, covering the type system, memory model, effect system,
> and compilation pipeline.**

*Last updated: Sep 5, 2026 (session 15 — bootstrap fixpoint achieved)*

## Table of Contents

1. [Introduction](#1-introduction)
2. [Compilation Pipeline](#2-compilation-pipeline)
3. [Type System](#3-type-system)
4. [Memory & Ownership](#4-memory--ownership)
5. [Effects & Capabilities](#5-effects--capabilities)
6. [Error Handling & Control Flow](#6-error-handling--control-flow)
7. [Intermediate Representations](#7-intermediate-representations)
8. [Backends & Code Generation](#8-backends--code-generation)
9. [Standard Library](#9-standard-library)
10. [Self-Hosting](#10-self-hosting)

---

## 1. Introduction

Fides is a systems programming language designed around three principles:
**safety by construction, verifiable correctness, and zero external
dependencies.** It compiles to native code (x86-64, ARM64, RISC-V), C99,
WebAssembly, and includes a built-in interpreter.

### What Makes Fides Different

| Feature | Fides | Rust | C |
|---------|-------|------|---|
| Memory safety | Compile-time (ownership + regions) | Compile-time (ownership + lifetimes) | Manual |
| Effect tracking | First-class (effects in type system) | None | None |
| Capability security | Compile-time capability grants | None | None |
| Dependency model | Self-contained (no libc, no LLVM) | External (crates, LLVM) | External (libc) |
| Codegen | Direct machine code emission | Via LLVM | Via gcc/clang |
| Self-hosting | ✅ Achieved (25 modules, 61K LOC, bootstrap fixpoint verified) | Yes | Yes |

### Design Philosophy

1. **Safety by construction, not by convention** — The compiler enforces
   memory safety, capability access, and effect annotations at compile time.
   No runtime checks needed for the core guarantees.

2. **If we depend on it, we build it** — No LLVM, no libc, no external
   linker. Every byte of machine code is emitted by Fides itself.

3. **Explicit over implicit** — Effects, capabilities, and ownership
   transfers are visible in the source. The compiler infers types and
   regions, but never silently changes semantics.

4. **Compile-time computation** — `comptime` blocks and `comptime gen`
   enable metaprogramming without a separate macro language.

---

## 2. Compilation Pipeline

Fides source code (`.fd` files) passes through these stages to become an
executable:

```
Source (.fd)
    │
    ▼
┌─────────┐     ┌──────────┐     ┌──────────────┐     ┌───────────┐
│  Lexer   │────▶│  Parser  │────▶│ Import Merge │────▶│ Type Check │
│ (tokens) │     │  (AST)   │     │ + Comptime   │     │ + Borrowck│
└─────────┘     └──────────┘     └──────────────┘     └───────────┘
                                                              │
                                                              ▼
┌──────────┐     ┌──────────┐     ┌───────────┐     ┌───────────────┐
│ Codegen  │◀────│ Optimize │◀────│ PIR Lower │◀────│ IR Lower      │
│ (x86/PE/ │     │ (L2 opt) │     │ (L1→L2→L3)│     │ (AST→SSA IR)  │
│  C/WASM) │     └──────────┘     └───────────┘     └───────────────┘
└──────────┘
    │
    ▼
Executable / C source / WASM
```

### Stage Details

#### 2.1 Lexing (`lexer.rs` / `lexer.fd`)

The lexer is a hand-written character scanner that produces tokens:
- **Keywords:** `fn`, `val`, `var`, `const`, `if`, `else`, `match`, `struct`,
  `enum`, `impl`, `pub`, `use`, `module`, `defer`, `comptime`, `region`,
  `sandbox`, `unsafe`, `txn`, `spawn`, `async`, `await`, etc.
- **Type keywords:** `i8`–`i128`, `u8`–`u128`, `f32`, `f64`, `bool`, `str`, `void`
- **Literals:** integers (dec/hex/bin/oct with `_` separators), floats,
  strings, f-strings (`f"..."`), raw strings (`r"..."`, `r#"..."#`)
- **Operators:** longest-match greedy (e.g. `<<=` before `<<` before `<`)

#### 2.2 Parsing (`parser.rs` / `parser.fd`)

Recursive-descent parser with operator-precedence climbing. Produces an AST
(`Program` containing `Func`, `StructDef`, `EnumDef`, `ImplDef`, `TraitDef`,
etc.).

**Expression precedence** (lowest to highest):

| Level | Operators | Associativity |
|-------|-----------|---------------|
| 1 | `\|>` | left |
| 2 | `??` | left |
| 3 | `..` `..=` | — |
| 4 | `\|\|` | left |
| 5 | `\|` | left |
| 6 | `^` | left |
| 7 | `&` | left |
| 8 | `<<` `>>` | left |
| 9 | `&&` | left |
| 10 | `==` `!=` | left |
| 11 | `<` `>` `<=` `>=` `is` | left |
| 12 | `+` `-` | left |
| 13 | `*` `/` `%` | left |
| 14 | unary `-` `!` `~` `&` `*` | prefix |
| 15 | `.` `?.` `[]` `?` `as` `~>` | postfix |
| 16 | literals, idents, closures | — |

#### 2.3 Import Resolution & Merging

1. Parse stdlib (`core.fd` embedded in compiler)
2. Parse user source
3. Resolve `use` imports from disk (searches `<base>/<path>.fd`,
   `<base>/<path>/mod.fd`, `stdlib/`, project root)
4. Pre-scan imports for transitive dependencies
5. Re-parse with full struct/enum name set (for `EnumName::Variant` parsing)
6. Merge: stdlib → imported modules → user program (user wins on conflicts)

#### 2.4 Type Checking (`typechecker.rs` / `typechecker.fd`)

- Bidirectional type inference with integer literal narrowing
- Region inference (lifetimes deduced automatically)
- Effect checking (function body effects vs. declared effects)
- Capability checking (sandbox restrictions, strict mode)
- Trait impl verification
- Linear type checking (`@linear` structs)
- Borrow checking (separate pass, NLL-based)

#### 2.5 IR Lowering (`lower.rs` / `lower.fd`)

AST → typed SSA IR. Key transformations:
- Mutable variables become `alloca` + `Load`/`Store`
- Closures are lambda-lifted to top-level functions with `__env` struct
- `match` becomes compare-and-branch chains
- `?` operator becomes tag-check + conditional error propagation
- `defer` blocks are pushed onto a stack and emitted at function exit (LIFO)
- Enums become structs with a tag field (field 0) + payload fields

#### 2.6 PIR Lowering (`pir.rs` / `pir.fd`)

SSA IR → three-level PIR (Portable IR):
- **L1:** Adds named regions, capability sets, effect sets to IR instructions
- **L2:** Drops region handles, converts to memory regions, keeps SSA form
- **L3:** Lowers to virtual registers (`VReg`) and stack slots — the final
  representation consumed by native code generators

#### 2.7 Optimization (`opt.rs` / `opt.fd`)

23 optimization passes over L2 PIR:
- **Constant folding** — evaluate constant expressions at compile time
- **CSE/GVN** — common subexpression elimination / global value numbering
- **DCE/DSE** — dead code/store elimination
- **Inlining** — with cost model (blocks×5 + insts, purity bonus)
- **Loop unrolling** — with `@gas`-guided limits
- **TCO** — tail-call optimization
- **Alias analysis** — linear type-based non-aliasing proofs
- **Devirtualization** — DynCall → direct Call
- **Partial evaluation** — evaluate calls with literal args
- **Vectorization, strength reduction, reassociation, memoization**

#### 2.8 Code Generation

| Backend | Output | Use Case |
|---------|--------|----------|
| x86-64 (ELF64) | Native Linux binary | Default `build` |
| x86-64 (PE) | Native Windows binary | Cross-compilation |
| ARM64 (ELF64) | Native ARM binary | Cross-compilation |
| RISC-V (ELF64) | Native RISC-V binary | Cross-compilation |
| C codegen | C99 source | Portability, bootstrap |
| Interpreter | Direct execution | Testing, debugging |
| WebAssembly | `.wasm` | Web deployment |

---

## 3. Type System

### 3.1 Primitive Types

| Type | Size | Description |
|------|------|-------------|
| `i8`–`i128` | 1–16 bytes | Signed integers |
| `u8`–`u128` | 1–16 bytes | Unsigned integers |
| `f32` | 4 bytes | Single-precision float |
| `f64` | 8 bytes | Double-precision float |
| `bool` | 1 byte | Boolean (`true`/`false`) |
| `str` | 8 bytes | String (pointer to runtime string) |
| `void` | 0 bytes | Unit type |
| `Never` | — | Bottom type (unreachable) |

**Note:** In the IR, all values are stored as 8 bytes (i64/pointer-sized).
Smaller types are widened on load and truncated on store. `i128`/`u128` are
lowered to `i64` in the interpreter but supported as `__int128` in C codegen.

### 3.2 Compound Types

```fides
// References
&str           // shared reference (immutable borrow)
&mut str       // mutable reference (exclusive borrow)
*mut T         // raw pointer (unsafe)

// Arrays
[i64; 10]      // fixed-size array
[T; N]         // generic-size array (ArrayConst)

// Tuples
(i64, str, bool)

// Function types
fn(i64, i64) -> i64

// Generic types
Vec<T>
HashMap<K, V>
Option<T>      // generic enum
```

### 3.3 Type Inference

Fides uses **bidirectional inference**:
- **Forward:** literal expressions infer the smallest fitting type
- **Backward:** context (let type, function param, return type) propagates
  hints downward, narrowing integer/float literals

```fides
val x = 42;          // infers i32 (smallest fitting)
val y: i64 = 42;     // hint forces i64
val z = 3.14;        // infers f64
val w = if cond { 1 } else { 2 };  // infers from both branches
```

### 3.4 Generics

```fides
fn identity<T>(x: T) -> T { x }

fn max<T: Ord>(a: T, b: T) -> T {
    if a >= b { a } else { b }
}

struct Vec<T> {
    data: *T,
    len: i64,
    cap: i64,
}
```

Generic functions are **monomorphized** — each concrete instantiation gets
its own compiled version. Type parameters can have trait bounds (`T: Ord`).

### 3.5 Traits (caps)

Fides calls traits "caps" (capabilities, not to be confused with security
capabilities):

```fides
cap Display {
    fn fmt(&self) -> str;
}

cap Iterator {
    type Item;  // associated type
    fn next(&mut self) -> Option<Self::Item>;
}

impl Display for Point {
    fn fmt(&self) -> str {
        f"({self.x}, {self.y})"
    }
}
```

- Associated types are supported (`type Item;`)
- Default method bodies are supported
- Trait objects via `dyn Trait` (dispatch by type name string at runtime)

### 3.6 Type Compatibility Rules

- Exact match (after alias resolution)
- `Never` is compatible with anything (bottom type)
- `Infer` (`_`) is compatible with anything
- Integer widening (i32 → i64) is implicit
- Int → Float conversion is implicit in comparisons
- `str` ↔ `i64` is allowed (pointer representation)
- Type parameters are compatible with anything in their scope
- `dyn Trait` is compatible with any concrete impl
- Tuples are checked element-wise

---

## 4. Memory & Ownership

### 4.1 Ownership Model

Fides uses Rust-like ownership with these rules:
1. Each value has exactly one **owner**
2. When the owner goes out of scope, the value is dropped (RAII)
3. You can borrow immutably (`&`) with unlimited readers
4. You can borrow mutably (`&mut`) with exactly one writer
5. Borrows must not outlive the owner (enforced by region inference)

### 4.2 Move vs Copy

- **Copy types** (`i32`, `bool`, `f64`, etc.) — copied by default, original
  still valid
- **Move types** (`String`, `Vec`, structs without `Copy`) — moved by
  default, original is invalidated

```fides
val a = 5;
val b = a;       // i32 is Copy — a still valid

val s1 = String::from("hello");
val s2 = s1;     // String is Move — s1 is moved
// println(s1);  // ERROR: s1 was moved
```

### 4.3 Region-Based Memory

Fides supports region-based memory management via `region` blocks:

```fides
fn process(data: &str) -> &str {
    region r {
        val cached = data.to_string();
        &cached    // borrow valid within region r
    }
}
```

- Allocations inside a region get `Region::Named(name)`
- All region allocations are bulk-freed at region exit (`p_region_free`)
- Region budgets can limit memory usage
- GPU regions (`region gpu { ... }`) for GPU-scoped allocation
- KV-cache regions with sliding window eviction

### 4.4 Borrow Checking

The borrow checker is a separate pass after type checking:
- **NLL (Non-Lexical Lifetimes):** borrows end at last use, not scope end
- **Field-wise borrows:** `&s.x` and `&s.y` can coexist
- **Two-phase borrows:** `&mut` and `&` in same call expression
- Modes: `on` (default), `warn`, `off`

### 4.5 Linear Types

Structs annotated `@linear` are tracked as linear types:
- Must be consumed (moved or explicitly dropped) — cannot be forgotten
- Used for resource handles, database connections, financial transactions
- Alias analysis uses linear pointers to prove non-aliasing

---

## 5. Effects & Capabilities

### 5.1 Effect System

Every function has an inferred **effect set** — the set of side effects it
may perform. Effects are tracked through the IR and checked against declared
annotations.

| Effect | Description |
|--------|-------------|
| `Alloc` | Allocates memory (heap or stack) |
| `IO` | Console/file I/O |
| `Net` | Network I/O |
| `Spawn` | Spawns threads or subprocesses |
| `Diverge` | May not return (infinite loop, panic) |
| `Read` | Reads from memory |
| `Write` | Writes to memory |
| `Diff` | Differentiable operation (autodiff) |
| `Noise` | Adds noise (differential privacy) |
| `Comm` | Cross-device communication |

Functions can declare their effects:

```fides
fn read_config(path: &str) -> Config !FileIO {
    val data = read_file(path)?;
    parse_config(data)
}
```

The compiler verifies that the function body only performs declared effects.

### 5.2 Capability Security

Capabilities control what resources a module can access:

```fides
use fs::file with { Read, Write };
use net::http with { Net };
```

- **Strict mode:** When any `with` clause is present, only listed
  capabilities are granted
- **Sandbox:** `sandbox { ... }` blocks deny all capabilities not explicitly
  granted
- **Transitive:** Imported modules inherit the importer's capabilities
- **Compile-time:** All checks happen at compile time — no runtime overhead

### 5.3 Safety Annotations

| Annotation | Meaning |
|------------|---------|
| `@deterministic` | Output is purely a function of inputs |
| `@crashfree` | Function cannot panic or abort |
| `@no-alloc` | Function performs no heap allocation |
| `@gas(n)` | Function has a gas limit of n operations |
| `@conserve` | Function conserves a resource (no leaks) |
| `@audit` | Function is audited for compliance |
| `@medical` | Function meets medical device safety standards |
| `@linear` | Struct type is linear (must be consumed) |
| `@send` | Struct can be sent between threads |
| `@sync` | Struct can be shared between threads |

---

## 6. Error Handling & Control Flow

### 6.1 Result Type

```fides
// Result is modeled as a struct with a tag field
// tag 0 = Ok, tag 1 = Err
fn parse_int(s: &str) -> Result<i64, ParseError> {
    // ...
    Ok(42)
}
```

### 6.2 The `?` Operator

The `?` operator propagates errors:

```fides
fn read_file(path: &str) -> Result<str, FileError> {
    val file = open(path)?;        // if Err, return Err immediately
    val data = read(file)?;
    Ok(data)
}
```

**How it works (IR level):**
1. Lower the inner expression → Result struct value
2. Load the tag field (field 0)
3. If tag == 0 (Ok): continue with the value field (field 1)
4. If tag == 1 (Err):
   - If inside a `try` block: copy error to the try context's error slot,
     jump to the error handler block
   - Otherwise: emit deferred blocks, return the error from the current
     function

### 6.3 Try Blocks

```fides
val result = try {
    val a = parse_int(s1)?;
    val b = parse_int(s2)?;
    a + b
};
```

A `try` block pushes an error target onto the `try_stack`. Any `?` inside
jumps to the error handler on failure. On success, the last expression is
wrapped as `Ok`.

### 6.4 Defer

```fides
fn process_file(path: &str) -> Result<(), FileError> {
    val file = open(path)?;
    defer close(file);          // runs when function exits (even on error)
    val data = read(file)?;
    Ok(())
    // close(file) runs here automatically
}
```

`defer` blocks are pushed onto a stack and emitted in LIFO order at:
- Function return (explicit or implicit)
- Error propagation from `?`
- Function epilogue

### 6.5 Match

```fides
match option {
    Some(value) => println(f"Got {value}"),
    None => println("Nothing"),
    _ => println("Other"),
}
```

**Match lowering:**
- Scrutinee is lowered once to a value
- Each arm generates a compare block and a body block
- `IntLit`/`BoolLit` → `BinOp::Eq` compare
- `StrLit` → `p_str_equal` call
- `Range` → `<=` and `<` compares
- `EnumVariant` → load tag field 0, compare to variant index
- Bindings → `GetFieldPtr`/`Load` from payload fields
- Guards → extra `Branch` after pattern match
- All arms merge to a single merge block

### 6.6 Loops

```fides
// For-in (iterator based)
for x in nums { ... }
for i in 0..10 { ... }
for i in 0..=10 { ... }

// While
while condition { ... }

// Infinite loop (with break-value)
val result = loop {
    if done { break value; }
};
```

`break` can carry a value (the loop expression evaluates to the break value).

---

## 7. Intermediate Representations

Fides uses three levels of intermediate representation between the AST and
native code:

### 7.1 AST (Abstract Syntax Tree)

The parser produces a `Program` containing:
- `funcs: Vec<Func>` — function definitions
- `structs: Vec<StructDef>` — struct types
- `enums: Vec<EnumDef>` — enum types
- `impls: Vec<ImplDef>` — impl blocks
- `traits: Vec<TraitDef>` — trait definitions
- `trait_impls: Vec<TraitImpl>` — trait implementations
- `consts: Vec<ConstDef>` — constants
- `type_aliases: Vec<TypeAlias>` — type aliases
- `imports: Vec<ImportDef>` — imports
- `extern_decls: Vec<ExternDecl>` — FFI declarations

**Key AST types:**
- `Type` — all type expressions (primitives, generics, refs, arrays, etc.)
- `Expr` — all expression forms (literals, calls, match, closures, etc.)
- `Stmt` — all statement forms (let, assign, if, while, for, defer, etc.)
- `Pattern` — match patterns (literals, bindings, enum variants, ranges)

### 7.2 IR (SSA Intermediate Representation)

The lowerer converts AST to typed SSA IR:

**IrType:**
```
I8/I16/I32/I64, U8/U16/U32/U64, F32/F64, Bool
Ptr(Box<IrType>), Struct(str), Array(Box<IrType>, i64)
Void, Never, Opaque(str), Generic(str, Vec<IrType>)
```

**Key instructions:**
- Constants: `ConstInt`, `ConstFloat(f64)`, `ConstBool`, `ConstStr`, `ConstNull`
- Arithmetic: `BinOp`, `UnaryOp`
- Memory: `Alloca(IrType, Region)`, `Load`, `Store`, `GetFieldPtr`, `GetElemPtr`
- Control flow: `Jump`, `Branch`, `Return`, `Unreachable`
- Calls: `Call`, `CallPtr` (closures/fn pointers), `DynCall` (trait objects), `MakeDyn`
- Conversions: `BitCast`, `IntExtend`, `IntTrunc`, `IntToFloat`, `FloatToInt`
- Fides-specific: `TryUnwrap`, `Print`, `Panic`, `Asm`
- Regions: `RegionBegin`, `RegionEnd`
- Persistence: `PersistLoad`, `PersistSave`
- Transactions: `TxnBegin`, `TxnEnd`
- Unsafe: `UnsafeBegin`, `UnsafeEnd`
- Autograd: `GradBegin`, `GradEnd`, `Backward`

**IR structure:**
- `IrModule` — collection of functions, struct layouts, extern declarations
- `IrFunction` — SSA function with basic blocks, value table, instruction map
- `BasicBlock` — list of instructions + predecessors + phi params
- `ValueId` / `BlockId` — typed handles for values and blocks
- `ValueMeta` — type, region, effects, source line, borrow kind per value

### 7.3 PIR (Portable IR — L1/L2/L3)

PIR is a three-level lowering pipeline:

**L1 (IR + effects/caps/regions):**
- Adds `EffectSet` and `CapSet` to each instruction
- Adds `RegionRef` for named region tracking
- `NewRegion`/`FreeRegion` for region lifecycle

**L2 (SSA + memory regions):**
- Drops region handles, uses `MemRegion` (Stack/Heap/Static/Named)
- Keeps SSA form with virtual value IDs
- This is the level the optimizer works on

**L3 (Virtual registers + stack slots):**
- Lowers SSA values to `VReg` (virtual registers) and `Slot` (stack slots)
- `Mov`, `Bin`, `Unary`, `Load`, `Store`, `FieldAddr`, `ElemAddr`
- `Jmp`, `Br`, `Ret`, `Call`, `CallPtr`, `DynCall`
- `Cast` with `CastKind` (BitCast/IntExtend/IntTrunc/IntToFloat/FloatToInt)
- This is the final representation consumed by native code generators

### 7.4 Closures

Closures use **lambda lifting** with environment capture:

1. **Find captures:** Free variables in the closure body (excluding params)
2. **Lift:** Create a top-level function `__closure_N` with `__env` as first
   parameter
3. **Environment:** At creation site, `p_malloc` an env struct:
   - Field 0: function name string (for dispatch)
   - Fields 1..N: captured values
4. **Call:** `CallPtr` with `has_env=true` — the closure pointer is passed
   as the first argument

In C codegen, `CallPtr` generates an inline `if/else` chain with `strcmp`
over candidate function names to dispatch to the right closure function.

### 7.5 Enum Representation

Enums are compiled to structs with a tag + payload:

```
Enum Foo { Bar(i64), Baz(str), Qux }

→ struct Foo_s {
    i64 tag;        // 0=Bar, 1=Baz, 2=Qux
    // payload fields (union of all variant data)
    i64 field_1;    // i64 for Bar, unused for Baz/Qux
    // str field_2;  // str for Baz (same offset, 8 bytes)
}
```

- Tag is always field 0
- Payload fields start at field 1
- Enum size = (1 + max_payload_fields) * 8 bytes
- `match` loads tag and compares to variant index

---

## 8. Backends & Code Generation

### 8.1 Interpreter

The interpreter is the reference oracle (~4400 LOC). It directly executes
the SSA IR:
- Walks basic blocks, evaluates each instruction
- Maintains a value table per function
- `Call`/`CallPtr` dispatch to `p_*` builtins or user functions
- Supports recursion depth limiting
- Auto-detects missing `__env` param and inserts dummy

**Value representation:**
```
Value::Int(i64), Float(f64), Bool, Str, Ptr(u64), Void
Vec(Rc<RefCell<SmallVec<[Value; 8]>>>)
Map(Rc<RefCell<HashMap<String, Value>>>)
Dyn(String, u64)     // trait object (type_name, data_ptr)
Tensor, Json, Future, ThreadHandle
```

### 8.2 C Codegen

Generates C99 source code from the IR:
- Each IR function → C function
- Each SSA value → `vN` temporary variable
- Basic blocks → labeled sections with `goto`
- Structs → `struct Name_s { ... }`
- Strings → static `char[]` constants
- Runtime functions (`p_*`) linked to bundled C runtime
- Binary hardening: stack canaries, PIE/ASLR, RELRO, NX stack
- Stack traces on panic

**C type mapping:**
```
IrType::I8  → int8_t      IrType::U8  → uint8_t
IrType::I64 → int64_t     IrType::U64 → uint64_t
IrType::F32 → float       IrType::F64 → double
IrType::Bool → bool       IrType::Ptr(_) → void*
IrType::Struct(n) → struct n_s*
IrType::Array(e, n) → {ctype}[{n}]
```

### 8.3 Native Codegen (x86-64/ARM64/RISC-V)

Direct machine code emission — no assembler, no LLVM:
- PIR L3 → machine code (manual byte encoding)
- ModRM/SIB bytes, REX prefixes, ABI rules
- Linear scan register allocation with constant rematerialization
- Peephole optimizer (9 passes, always on)
- ELF64 (Linux), PE (Windows), Mach-O (macOS) object file formats
- Custom linker for raw linking with runtime

### 8.4 Runtime Library

The runtime (`runtime_linux.rs`, etc.) is hand-encoded x86-64 machine code:
- Memory: `p_malloc`, `p_free`, `p_region_free`
- Strings: `p_str_concat`, `p_str_equal`, `p_to_str`, `p_float_to_str`
- Collections: `p_vec_new/push/pop/len/get/set`, `p_map_*`
- I/O: `p_print_int/float/bool/str/newline`, `p_panic`
- Threads: `p_thread_create/join`, `p_mutex_*`, `p_atomic_*`
- Networking: `p_socket_*`, `p_http_*`
- ML: `p_tensor_*`, `p_autograd_backward`, `p_quantize`
- Privacy: `p_dp_budget_*`, `p_tp_*`

All builtins are prefixed with `p_` to avoid C standard library collisions.

---

## 9. Standard Library

Fides has 69 stdlib modules (~18,200 LOC). Key modules:

### Core
| Module | Description |
|--------|-------------|
| `core` | Built-in types, Option, Result, range, Box |
| `vec` | Dynamic array (Vec<T>) |
| `string` | String operations |
| `hash` | Hashing (fnv1a, siphash) |
| `collections` | HashMap, HashSet, BTreeMap, BTreeSet, Deque |
| `sort` | Sorting algorithms |
| `iter` | Iterator adapters (eager Vec-based) |
| `stack` / `queue` | Stack and queue data structures |
| `buffer` | Byte buffer |
| `result` | Result type helpers |
| `fmt` | String formatting |
| `math` | Math functions |
| `bits` | Bit manipulation |
| `random` | Random number generation |

### I/O & System
| Module | Description |
|--------|-------------|
| `fs` / `file` / `io` | File system, file I/O, stream I/O |
| `os` / `sys` / `env` / `args` / `process` | OS interface, environment, CLI args |
| `path` | Path manipulation |
| `time` | Time and timers |
| `channel` | Channel-based messaging |
| `sync` | Mutex, atomics, thread-local storage |
| `parallel` | Parallel computation |

### Networking & Data
| Module | Description |
|--------|-------------|
| `http` | HTTP client/server |
| `db` | Database interface |
| `json` | JSON parsing/serialization |
| `regex` | Regular expressions |
| `uuid` | UUID generation |
| `crypto` | Cryptographic primitives |

### Domain-Specific
| Module | Description |
|--------|-------------|
| `money` / `banking` | Financial types and operations |
| `clinical` / `medical` | Healthcare data standards |
| `ecu` / `scada` / `swift` / `arinc` / `can` | Industrial/automotive/aviation protocols |
| `grid` / `ota` | Grid computing, over-the-air updates |

### ML & AI
| Module | Description |
|--------|-------------|
| `tensor` | Tensor operations |
| `simd` | SIMD intrinsics |
| `shape` | Shape-level type safety |
| `kv_cache` | KV cache for LLM inference |
| `quant` | Quantization |
| `dp` | Differential privacy |
| `shard` | Tensor sharding/parallelism |
| `privacy` | Privacy primitives |

### Game & Graphics
| Module | Description |
|--------|-------------|
| `ecs` / `events` | Entity-component-system |
| `tilemap` / `sprite` | 2D game primitives |
| `flowfield` / `scene` / `physics` | Game physics and scenes |
| `audio` / `input` | Audio and input handling |
| `streaming` | Streaming content |

### Utilities
| Module | Description |
|--------|-------------|
| `memo` | Memoization |
| `units` | Unit conversions |
| `typed_strings` | Type-safe string newtypes |
| `alloc` | Custom allocators |
| `test` | Testing framework |

---

## 10. Self-Hosting

Fides is being self-hosted — the compiler is being rewritten from Rust
(`src/*.rs`, ~88K LOC) into Fides (`compiler/*.fd`, ~42K LOC).

### Current State

- **17 active modules** in `compiler/`:
  `token`, `diag`, `json`, `fmt`, `ast`, `lexer`, `parser`, `ir`,
  `typechecker`, `lower`, `mono`, `borrowck`, `pir`, `opt`, `codegen_c`,
  `compiler`, `comptime_eval`
- **23/23 self-host tests pass** on all 3 backends (interpreter, C codegen,
  PE emitter)
- **SH-10 critical fixes complete:** All 7 blocking bugs fixed (Inst::Cast,
  ConstFloat, CallPtr, comptime stubs, Expr::Try, emit_defers, import errors)

### Bootstrap Strategy

The bootstrap uses the C codegen path:
1. Rust compiler (`fidesc`) compiles `compiler/*.fd` to C source
2. C source is compiled with `gcc` to produce a native `fides-bootstrap`
3. `fides-bootstrap` is used to compile Fides programs
4. Eventually `fides-bootstrap` compiles itself (full bootstrap)

### Key Adaptations (Rust → Fides)

| Rust Feature | Fides Adaptation |
|--------------|------------------|
| `enum` with struct variants | Tuple-style variants (`Type::Array(Box<Type>, i64)`) |
| `Option<T>` | Custom `Opt*` enums (`OptBoxExpr`, `OptStmts`, etc.) |
| `Result<T, E>` | Custom `ParseResult*` enums |
| `String` / `usize` / `u64` | `str` / `i64` |
| `format!` macro | String concatenation with `+` and `int_to_str` |
| `HashMap<K,V>` | `HashMap` via `collections.fd` |
| `HashSet<T>` | `HashSet` in `collections.fd` |
| Lazy iterators | Eager Vec-based adapters |
| `mem::replace`/`swap` | Manual temp variables |
| `derive(Debug, Clone)` | `@derive(Debug, Clone)` |
| `?` operator | Explicit `match` propagation |

### Remaining Work

- **SH-9 bootstrap:** 3 remaining bugs (unsafe block init, Vec method
  dispatch, undefined function lookups)
- **SH-10 medium items:** DynCall codegen, TryBlock, trait impl verification,
  borrowck enablement, OptionalFieldAccess, FString dispatch
- **Differential testing:** Compare self-hosted compiler output against Rust
  compiler output for all test programs

---

*For syntax details, see [syntax.md](syntax.md). For compiler architecture,
see [architecture.md](architecture.md). For self-hosting details, see
[selfhost.md](selfhost.md). For feature status, see
[features.md](features.md).*
