# Self-Hosting Guide: Fides Compiler in Fides

> **What this is:** A practical reference for anyone working on the self-hosting
> effort. Explains the process, how Fides differs from Rust/C, what limitations
> to expect, and how to avoid common pitfalls during bootstrap.

## Table of Contents

1. [What Self-Hosting Means](#1-what-self-hosting-means)
2. [The Strategy: C Codegen Bootstrap](#2-the-strategy-c-codegen-bootstrap)
3. [How Fides Differs from Rust](#3-how-fides-differs-from-rust)
4. [How Fides Differs from C](#4-how-fides-differs-from-c)
5. [Limitations and Encountered Issues](#5-limitations-and-encountered-issues)
6. [What to Keep in Mind During Bootstrap](#6-what-to-keep-in-mind-during-bootstrap)
7. [Recommended Process](#7-recommended-process)
8. [Failure Modes and Debugging](#8-failure-modes-and-debugging)
9. [Files: Rewritten and Remaining](#9-files-rewritten-and-remaining)

---

## 1. What Self-Hosting Means

Self-hosting means the Fides compiler is written in Fides. The bootstrap
process proves this works:

```
Step 1: Rust compiler (fidesc) compiles Fides compiler → fides-c
Step 2: fides-c compiles the Fides compiler → fides-self
Step 3: fides-self compiles the Fides compiler → fides-self-2
Step 4: diff fides-self fides-self-2  (must be identical — deterministic)
Step 5: fides-self passes the full test suite
```

If step 4 produces identical binaries, the compiler is **round-trip stable** —
it can compile itself forever without drift.

## 2. The Strategy: C Codegen Bootstrap

The Fides compiler has three backends:
- **Interpreter** — runs IR directly (slow, reference implementation)
- **C codegen** — translates IR → C source, hands off to gcc/clang
- **Native (PE/ELF)** — translates IR → raw x86-64 machine code (no external tools)

We bootstrap via **C codegen** because:
- It's the most stable (167/167 tests pass)
- gcc handles optimization (we don't need to match gcc -O2)
- The C codegen path is simpler than native (no register allocation, no ELF headers)
- It's portable (works anywhere gcc exists)

The native backends (codegen_x64, regalloc, peephole, elf64, linker) are
**not needed** for the bootstrap. They can be rewritten later to eliminate
the gcc dependency entirely.

### The Compilation Pipeline (C Codegen Path)

```
source (.fd)
  → lexer        (tokenize)
  → parser       (build AST)
  → typechecker  (type inference, region/effect/capability checks)
  → lower        (AST → SSA IR)
  → mono         (monomorphize generics)
  → borrowck     (borrow checking)
  → pir          (IR → PIR L1/L2/L3 transforms)
  → opt          (23 optimization passes on PIR)
  → codegen_c    (PIR → C source code)
  → gcc/clang    (C → native binary)
```

All 16 files in this pipeline have been rewritten in Fides (see section 9).

## 3. How Fides Differs from Rust

These differences shaped every rewrite. Understanding them is essential.

### 3.1 Structs Passed by Value

**Rust:** `fn process(&module: &IrModule)` — passes a pointer, O(1).
**Fides:** `fn process(self: &mut CCodeGen, module: IrModule)` — copies the entire struct.

Every function that takes a large struct (IrModule, IrFunction, AstModule)
copies all its Vecs, strings, and nested data. This is the biggest performance
difference.

**Impact:** The self-hosted compiler will be significantly slower than the
Rust compiler. For bootstrap, this is acceptable — correctness matters more
than speed. The workaround is "get-modify-set": read the struct, modify a
local copy, write it back.

**Future fix:** Add reference types (`&T`) to Fides for large struct passing.

### 3.2 No Real HashMap — Vec-Based Lookup

**Rust:** `HashMap<String, V>` — O(1) lookup via hash table.
**Fides:** `HashMap` is i64→i64 only. String-keyed maps use `Vec<Entry>` with
linear search — O(n).

Every type lookup, function signature lookup, and variable resolution in the
typechecker is a linear scan. For a compiler processing thousands of items,
this makes the typechecker quadratically slow on large programs.

**Impact:** The self-hosted compiler compiling its own 4800-line typechecker
will be slow. Acceptable for bootstrap.

**Future fix:** Add generic HashMap to Fides stdlib (string-keyed, hash-based).

### 3.3 No Lazy Iterators — Eager Intermediate Vecs

**Rust:** `vec.iter().map(|x| ...).filter(|x| ...).collect()` — lazy, zero
intermediate allocations.
**Fides:** Explicit while loops, each step produces a new Vec.

Every transformation creates intermediate Vecs. More allocations, more memory.

### 3.4 No `continue` in Loops

**Rust:** `for x in items { if cond { continue; } ... }`
**Fides:** No `continue` keyword. Must use `if/else` or flag-based skip patterns.

**Workaround:** Wrap the rest of the loop body in `if !skip { ... }`.

### 3.5 No `if/else` Inside Match Arms

**Rust:**
```rust
match x {
    Foo(a, b) => if a > 0 { ... } else { ... },
    _ => {},
}
```
**Fides:** Match arms must be simple. No `if/else` directly inside an arm.

**Workaround:** Extract to helper functions, or use separate match statements.

### 3.6 No Variable Assignment from Match Arms

**Rust:**
```rust
let x = match y {
    Foo(a) => a,
    Bar(b) => b,
    _ => default,
};
```
**Fides:** Can't assign a variable from a match arm directly in all contexts.

**Workaround:** Declare `var x: T` before the match, assign inside each arm.
Or use helper functions that return values.

### 3.7 No Nested Enum Pattern Matching

**Rust:**
```rust
match x {
    Opt::Some(Inst::Call(name, args, ret)) => { ... }
    _ => {}
}
```
**Fides:** Can't nest enum patterns in a single match arm.

**Workaround:** Flatten into two matches:
```fides
match x {
    Opt::Some(inst) => {
        match inst {
            Inst::Call(name, args, ret) => { ... }
            _ => {}
        }
    }
    Opt::None => {}
}
```

### 3.8 `val` is a Keyword

**Rust:** `let val = 42;` — fine.
**Fides:** `val` is a keyword (used for value declarations). Must rename to
`v`, `ret_val`, `value`, etc.

### 3.9 `region` is a Keyword

**Rust:** `let region = ...` — fine.
**Fides:** `region` is a keyword (region-based memory management). Must rename
to `mem_region` or similar.

### 3.10 `>>` Parsing Issues

**Rust:** `Vec<Vec<i64>>` — fine.
**Fides:** `>>` can be misinterpreted. Must write `Vec<Vec<i64> >` with a
space. Also use `shr_i64()` helper instead of `>>` operator in expressions.

### 3.11 No `derive(PartialEq)`

**Rust:** `#[derive(PartialEq)]` — auto-generates equality comparison.
**Fides:** `@derive(Debug, Clone)` exists but no `PartialEq`. Must write
manual `equals()` functions.

### 3.12 Clone is Shallow

**Rust:** `.clone()` does a deep copy.
**Fides:** Clone returns `self` (shallow). But since structs are passed by
value anyway, this is usually fine — the copy semantics handle it.

### 3.13 String Concatenation Allocates

**Rust:** `String` is a growable buffer. `format!` is efficient.
**Fides:** `str` is an i64 pointer to a runtime-managed string. `a + b`
allocates a new string every time. The codegen modules do massive string
concatenation (building C source code), which is slow.

### 3.14 No `as` Casts for Enums

**Rust:** `x as u64` — numeric cast.
**Fides:** Must use explicit cast functions or match patterns.

### 3.15 `Option<T>` → Custom Opt Enums

**Rust:** `Option<T>` is built-in and generic.
**Fides:** `Option<T>` exists but for each concrete type used in the compiler,
we define a custom enum:
```
OptStr = Some(str) | None
OptIrType = Some(IrType) | None
OptVecIrType = Some(Vec<IrType>) | None
OptInst = Some(Inst) | None
OptStructLayout = Some(StructLayout) | None
```

### 3.16 No `mem::replace` / `mem::swap` / `mem::transmute`

**Rust:** Standard library memory operations.
**Fides:** Must use manual temp variables:
```fides
var tmp = a;
a = b;
b = tmp;
```

## 4. How Fides Differs from C

The C codegen translates Fides IR → C99. Understanding the mapping helps
debug codegen issues.

### 4.1 All Values Are 8 Bytes

Fides IR uses I64 for everything (integers, pointers, booleans, enums).
In C, these become `int64_t`. Pointers are cast to `void*` or `int64_t`
interchangeably. This works on x86-64 (64-bit pointers) but would break
on 32-bit.

### 4.2 Closures via String Dispatch

Fides closures are structs where field 0 is a `char*` function name. The
C codegen generates an if/else chain comparing the name and calling the
matching C function. This is O(n) per call but works.

### 4.3 Trait Objects via Type Name Dispatch

Dyn values are `{ const char* type_name; void* ptr; }`. Method calls
compare `type_name` and dispatch to `Type_method` C functions.

### 4.4 Runtime Library

The C codegen emits a ~3000-line C runtime implementing:
- Vec operations (p_vec_new, p_vec_push, p_vec_get, ...)
- Map operations (p_map_new, p_map_insert, p_map_get, ...)
- String operations (p_string_*, p_to_str, p_print_*)
- Threads + mutex (pthread-based)
- Channels (bounded MPSC with mutex + condvar)
- Sockets (native HTTP server)
- SIMD builtins
- Tensor + autograd runtime
- KV cache, quantization
- JSON parser
- HTTP client
- DB (memory-backed)
- ECS (entity-component-system)

### 4.5 Binary Hardening

Generated C includes:
- Stack canaries (`-fstack-protector-strong`)
- PIE/ASLR (`-fPIE -pie`)
- RELRO (`-Wl,-z,relro,-z,now`)
- NX stack (default)
- Stack traces on panic (p_call_stack)

## 5. Limitations and Encountered Issues

### 5.1 Performance

The self-hosted compiler will be **10-100x slower** than the Rust compiler
due to:
- Struct copying (no references)
- O(n) map lookups (no hash table)
- Eager intermediate Vecs (no lazy iterators)
- String allocation per concatenation

**Acceptable for bootstrap.** Optimize later.

### 5.2 Recursion Depth

The typechecker and parser are deeply recursive. Fides has recursion limits
(the parser has an iterative fallback at depth 512). When the self-hosted
compiler processes its own 4800-line typechecker, it may hit limits.

**Mitigation:** Increase `--max-recursion` flag. The parser already has
an iterative fallback.

### 5.3 Memory Usage

Struct-by-value passing + intermediate Vecs = high memory usage. The
self-hosted compiler compiling itself may use 2-4x more memory than the
Rust compiler.

**Mitigation:** Ensure sufficient RAM (4GB+). Accept for bootstrap.

### 5.4 Determinism

Bootstrap step 4 requires identical binaries across rounds. Sources of
non-determinism:
- HashMap iteration order (mitigated: Fides HashMap is deterministic)
- Pointer addresses in output (should not appear in C codegen)
- Allocation order (Vec iteration is insertion-order, which is deterministic)

**Risk:** Low. But must verify with the actual bootstrap.

### 5.5 Visibility and Module Resolution

Fides has module-private visibility. Multiple modules can define functions
with the same name (e.g., `str_starts_with` exists in both `core` and `opt`).
The typechecker must allow each module to access its own private definitions
even when another module registered the same name first.

**Current approach:** Track which functions each module defines. When
checking visibility, if the current module has its own definition, allow
access.

### 5.6 FFI Handle Types

Fides FFI uses `i64` for all handle types (FILE*, DIR*, etc.). On x86-64
this is safe (pointers are 8 bytes). gcc 14+ warns about int-to-pointer
casts. The `-Wno-int-conversion` flag suppresses this.

### 5.7 Missing Language Features (Worked Around)

| Feature | Status | Workaround |
|---------|--------|------------|
| Reference passing (`&T`) | Not implemented | Copy structs by value |
| Generic HashMap | Not implemented | Vec-based maps with linear search |
| `continue` in loops | Not supported | if/else wrapping |
| if/else in match arms | Not supported | Helper functions |
| Nested enum patterns | Not supported | Flattened matches |
| `derive(PartialEq)` | Not implemented | Manual equals() functions |
| Lazy iterators | Not implemented | Eager while loops + intermediate Vecs |

## 6. What to Keep in Mind During Bootstrap

### 6.1 The Rust Compiler is the Oracle

Never break `src/*.rs` until bootstrap is fully working. The Rust compiler
is the reference implementation. Compare Fides output against it.

### 6.2 Differential Testing Before Bootstrap

Before attempting bootstrap, verify that each Fides module produces identical
output to its Rust counterpart:
```bash
fidesc diff-test tests/hello.fd --stage c     # C output
fidesc diff-test tests/hello.fd --stage ir    # IR output
fidesc diff-test tests/hello.fd --stage pir3  # PIR output
```
Run across all test files. Fix differences before bootstrap.

### 6.3 Start Small

Don't jump to compiling the full compiler. Build up:
1. Compile `hello.fd` with the Fides compiler
2. Compile `token.fd` (small, no deps)
3. Compile `json.fd` (small, no deps)
4. Compile `ast.fd` (medium, has deps)
5. Compile `ir.fd` (large)
6. Compile `typechecker.fd` (huge — 4800 lines)
7. Compile `codegen_c.fd` (huge — 4700 lines)
8. Compile `compiler.fd` (the driver)
9. Compile all together (full bootstrap)

### 6.4 Each Step is Independently Cached

Write the bootstrap script so each step is cached. If step 6 fails, you
don't redo steps 1-5.

### 6.5 The 36 Unrewritten Files Are Not Needed

The bootstrap only needs the 16 pipeline files. The 36 remaining files
(native backends, binary formats, tooling) are not on the C codegen path.

## 7. Recommended Process

### Phase 1: Differential Testing (Before Bootstrap)

```bash
# Compare Fides compiler output to Rust compiler output
for test in tests/*.fd; do
    fidesc diff-test "$test" --stage c
    fidesc diff-test "$test" --stage ir
    fidesc diff-test "$test" --stage pir3
done
```

Fix all differences. This is the most important step.

### Phase 2: Create Missing Tests

- `compiler/tests/test_typechecker.fd` — the only rewritten module without a test

### Phase 3: Incremental Bootstrap

```
hello.fd → token.fd → json.fd → ast.fd → ir.fd → 
typechecker.fd → codegen_c.fd → compiler.fd → FULL BOOTSTRAP
```

### Phase 4: Round-Trip Stability

```bash
fidesc ccompile compiler/compiler.fd -o fides-c
./fides-c compiler/compiler.fd -o fides-self
./fides-self compiler/compiler.fd -o fides-self-2
diff fides-self fides-self-2  # must be identical
```

### Phase 5: Full Test Suite

```bash
./fides-self test
# Must pass: all tests on interpreter + C codegen + PE emitter
```

## 8. Failure Modes and Debugging

| Failure | Symptom | Where to Look |
|---------|---------|---------------|
| Visibility error | "private to module X" | typechecker — module resolution |
| Missing function | "unknown function foo" | lower or mono — function registration |
| Wrong type | "type mismatch" | typechecker — type inference |
| Infinite loop | Hangs | parser or typechecker — recursion |
| Crash/abort | Segfault | codegen_c — pointer casting |
| Non-determinism | Step 3 differs from step 2 | Any module using Vec-based maps |
| Out of memory | OOM | Struct-by-value copying — reduce struct size |
| Recursion limit | "max recursion depth" | parser/typechecker — increase --max-recursion |
| C compilation error | gcc error | codegen_c — invalid C output |
| Wrong output | Test fails | Trace: which stage produces wrong output? |

### Debugging Strategy

1. **Bisect the layer:** `./scripts/bisect.sh <test_name>`
2. **Compare stages:** `fidesc diff-test <test> --stage ir` (or pir, c)
3. **Check if golden was wrong:** Compare to known-good output
4. **Make the smallest possible fix:** One change, one test, one verify

## 9. Files: Rewritten and Remaining

### Rewritten (16 files — the core pipeline)

| File | LOC | Status | Test |
|------|-----|--------|------|
| `token.fd` | ~400 | Done | test_token.fd |
| `diag.fd` | ~300 | Done | (via other tests) |
| `json.fd` | ~800 | Done | test_json.fd |
| `fmt.fd` | ~500 | Done | test_fmt_*.fd |
| `ast.fd` | ~1200 | Done | (via parser/lower tests) |
| `lexer.fd` | ~600 | Done | (via parser tests) |
| `parser.fd` | ~3000 | Done | (via lower tests) |
| `ir.fd` | ~1600 | Done | test_ir.fd |
| `typechecker.fd` | ~4800 | Done (builds) | **no test file yet** |
| `lower.fd` | ~3500 | Done | test_lower.fd |
| `mono.fd` | ~2500 | Done | test_mono.fd |
| `borrowck.fd` | ~1800 | Done | test_borrowck.fd |
| `pir.fd` | ~2700 | Done | test_pir.fd |
| `opt.fd` | ~3500 | Done | test_opt.fd |
| `codegen_c.fd` | ~4700 | Done | test_codegen_c.fd |
| `compiler.fd` | ~565 | Done | test_compiler.fd |

### Not Rewritten (36 files — not needed for C codegen bootstrap)

**Native backends (would eliminate gcc dependency):**
- `codegen_x64`, `regalloc`, `peephole`, `linker`, `runtime_linux`

**Binary format emitters:**
- `elf64`, `pe`, `coff`, `macho`

**Other architecture backends:**
- `codegen_arm64`, `codegen_riscv`, `runtime_arm64`, `runtime_riscv`,
  `arm64_enc`, `riscv_enc`, `x86`

**Interpreter:**
- `interp` (~4400 LOC — the reference oracle)

**Tooling:**
- `main` (CLI dispatch), `comptime`, `debugger`, `native_debugger`, `lsp`,
  `hotreload`, `cparser`, `pacte`, `scribe`, `veil`, `inc`, `embedded`,
  `flat_ast`, `lib`

**Other backends:**
- `wasm`, `gpu`, `codegen` (dispatch)

**Runtime libraries:**
- `tensor`, `autograd`

### Rewriting Priority (Post-Bootstrap)

1. `main` — CLI entry point (needed for self-hosted compiler to be usable)
2. `codegen_x64` + `regalloc` + `peephole` + `elf64` + `linker` + `runtime_linux` — eliminate gcc dependency
3. `interp` — self-hosted interpreter
4. Tooling (`lsp`, `debugger`, `pacte`, etc.) — as needed

---

## Quick Reference: Fides → Rust Adaptation Checklist

When rewriting a Rust file to Fides, check:

- [ ] `HashMap<K,V>` → `Vec<Entry>` with linear search helper
- [ ] `HashSet<T>` → `Vec<T>` with `vec_contains_*` helper
- [ ] `&T` references → pass by value (copy the struct)
- [ ] `&mut T` → `self: &mut Struct` or get-modify-set pattern
- [ ] `Option<T>` → custom `Opt*` enum per type
- [ ] `Box<T>` → `box_*_unbox()` helper for recursive types
- [ ] `format!` / `println!` → string concatenation + `int_to_str`
- [ ] `.iter().map().filter().collect()` → explicit while loops
- [ ] `continue` → `if/else` wrapping
- [ ] `if/else` in match arms → helper functions
- [ ] Nested enum patterns → flattened matches
- [ ] `val` variable → rename to `v` or `value`
- [ ] `region` variable → rename to `mem_region`
- [ ] `Vec<Vec<T>>` → `Vec<Vec<T> >` (space between brackets)
- [ ] `>>` operator → `shr_i64()` helper
- [ ] `derive(PartialEq)` → manual `equals()` function
- [ ] `mem::replace` / `mem::swap` → manual temp variables
- [ ] `usize` / `u32` / `u64` → `i64`
- [ ] `char` → `i64` (codepoint)
- [ ] `String::from_utf8` / `as_bytes` → manual byte↔str conversion
