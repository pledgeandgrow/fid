# Fides Self-Hosting Roadmap

> **Goal:** The Fides compiler compiles itself, producing identical output
> across generations (bootstrap fixpoint).
>
> **Last updated:** Sep 3, 2026

---

## Current State (Sep 3, 2026)

### What works

- All 18 self-host modules rewritten in Fides (39.5K LOC)
- All 23 self-host module tests pass on interpreter + PE emitter
- `release/fides-c.exe` builds via tcc (8.5MB)
- The self-hosted compiler **lexes** stdlib/core.fd successfully
- The self-hosted compiler **parses** stdlib/core.fd successfully
- The self-hosted compiler **parses** user code (tests/hello.fd) successfully
- Type checking starts but crashes

### Current crash

```
[build] Stdlib parsed ok. Extracting names...
[build] Lexing user code...
[build] User code lexed. Checking errors...
[build] Parsing user code...
[build] Type checking...
=== CRASH: 0xC0000005 ===
Access violation at address 0xE
Call stack depth: 8
  [0] main
  [1] cmd_ccompile
  [2] ccompile_source_with_path
  [3] build_program_with_path
  [4] TypeChecker_check_bodies
  [5] TypeChecker_check_func
  [6] map_str_str_get
  [7] p_str_equal
```

**Pipeline progress:** Lexer OK → Parser OK → TypeChecker CRASHES

### Build commands

```bash
cargo build --release                                         # Rust compiler
.\target\release\fidesc.exe ccompile compiler\main.fd -o release\fides-c   # Generate C
C:\tcc\tcc.exe -O2 -w -o release\fides-c.exe release\fides-c.c C:\tcc\lib\ws2_32.def  # Link
.\release\fides-c.exe ccompile tests\hello.fd -o release\hello_test        # Test
```

---

## The Root Cause: Struct-by-Value in the C Backend

Almost every crash in the self-hosted compiler traces back to one fundamental
issue: **the C backend treats all structs and enums as pointers (`struct Foo_s*`), but the Fides language passes and returns them by value.**

This means:

1. **Struct returns (SRET):** When a function returns a struct, the C backend
   uses a hidden first parameter (SRET pointer) to copy the struct data out.
   If the caller doesn't know the function returns a struct (missing
   pre-registration), no SRET buffer is allocated → the callee writes to
   garbage → crash.

2. **Vec-of-struct:** When `Vec::get(i)` returns a struct, the C backend
   copies only 8 bytes (one pointer-sized slot) instead of the full struct.
   The remaining fields are garbage → accessing `.key` or `.val` crashes.

3. **Enum variant extraction:** When matching an enum with a struct payload
   (e.g. `ParseResultExpr::Ok(Expr)`), `Inst::Load` with `IrType::Struct`
   only reads 8 bytes. The full struct data (which can be 80+ bytes) is
   truncated → the match arm sees a garbage tag → wrong arm or crash.

4. **Enum literal storage:** `Expr::EnumLit` must store struct-typed fields
   **inline** (via `memcpy`) in the parent enum's data slots. If it stores a
   pointer to a stack alloca instead, the pointer dangles when the creating
   function returns.

### Fixes already applied

| Fix | Description | Rust side | Fides side |
|-----|-------------|-----------|------------|
| SRET calling convention | Struct-returning functions get hidden out-param | DONE | DONE |
| Pre-registration of impl methods | Mangled method signatures registered before call sites | DONE | DONE |
| Pre-registration of free functions | All function signatures registered before lowering | DONE | DONE |
| Enum layout from field sizes | Payload size computed from actual field sizes, not field count | DONE | DONE |
| `load_enum_field()` helper | Alloca + Store(memcpy) for struct-typed enum fields | DONE | DONE |
| `Inst::Store` honors `store_ty` | C codegen emits `memcpy` when `store_ty` is `Struct(_)` | DONE | DONE |
| `EnumLit` inline struct storage | Struct fields stored inline via `memcpy`, not as pointers | DONE | DONE |
| Match field index accumulation | `field_idx` accumulates `enum_field_slots()` per field | DONE | DONE |
| `maybe_deref_box` simplified | No-op (Box pointer already points to struct data) | DONE | DONE |
| `apply_cfg_filter` no-op | Skips CFG filtering (not needed for self-host) | N/A | DONE |
| `compute_func_effects` no-op | Skips effect inference (not needed for bootstrap) | N/A | DONE |
| Stmt::Let alloca type | Uses `Struct(name)` for alloca when declared type matches | DONE | **TODO (Gap 1)** |
| Stmt::Let store_ty | Uses `Some(Struct(_))` for struct-typed alloca store | DONE | **TODO (Gap 2)** |
| Expr::Ident struct alloca | Returns alloca pointer directly for `Struct(_)` typed alloca | DONE | **TODO (Gap 3)** |
| StructLit field store | Uses `emit_struct_copy` for `Struct(_)` fields | DONE | **TODO (Gap 4)** |
| Vec element type from `&mut`/`&` | Unwraps reference types to get Vec element type | DONE | DONE |
| Vec-of-struct push/get | Heap-allocates structs before pushing to Vec | **TODO** | **TODO** |

### What's still broken

The current crash (`map_str_str_get` → `p_str_equal`) is the **Vec-of-struct**
problem. `Vec<StringString>::get(i)` returns a `StringString` struct (16 bytes:
two `str` pointers), but the C backend only copies 8 bytes. The `.key` field
is garbage, and `p_str_equal(garbage, key)` crashes at address 0xE.

**After fixing Vec-of-struct**, the Fides lowerer gaps (1-4) will cause
crashes when `stage1.exe` tries to compile itself. These must be ported
before the fixpoint test.

---

## The Path to Fixpoint

### Phase 1: Fix remaining struct-by-value crashes [CURRENT]

The same root cause (struct-by-value only copies 8 bytes) will cause crashes
in every pipeline stage that handles structs. The crash will move forward
through the pipeline as each one is fixed.

**Approach:** Iterative crash-and-fix loop.

1. Run the self-hosted compiler on `tests/hello.fd`
2. Read the crash stack trace
3. Identify the function and the struct being truncated
4. Fix in `compiler/*.fd` (the self-hosted source)
5. Rebuild: `cargo build --release && fidesc ccompile compiler\main.fd -o release\fides-c && tcc ...`
6. Run again, find the next crash
7. Repeat

**Expected crashes by pipeline stage:**

| Stage | Status | Expected crashes | Pattern |
|-------|--------|-----------------|---------|
| Lexer | DONE | 0 (fixed) | — |
| Parser | DONE | 0 (fixed) | — |
| TypeChecker | CRASHES | 2-5 | Vec-of-struct, HashMap ops, Type enum handling |
| Lowerer | Not reached | 1-3 | IrType struct returns, enum layout |
| CCodeGen | Not reached | 1-2 | String building, struct layout |
| **Total remaining** | | **4-10** | |

**Current crash to fix:** `map_str_str_get` in `compiler/typechecker.fd` —
`Vec<StringString>::get(i)` truncates the 16-byte struct to 8 bytes.

**Fix options:**
- (A) Change `map_str_str_get` to iterate by index and access fields through
  the Vec's internal pointer (avoiding the by-value copy)
- (B) Fix `Vec::get` for struct element types to emit `memcpy` (the general
  fix — benefits all Vec-of-struct uses)
- (C) Change `StringString` to be heap-allocated (push pointers, not structs)

**Recommendation:** Heap-allocate structs before pushing (see "Vec runtime
architecture" in the Known Bugs section). Both lowerers need this fix.
This is SH-11.11.

### Phase 1b: Port Fides lowerer gaps (after Vec-of-struct fix)

Once the Vec-of-struct crash is fixed, the typechecker should work. But
when `stage1.exe` tries to compile itself, the Fides lowerer's missing
fixes (Gaps 1-4) will cause crashes. These must be ported before the
fixpoint test.

| Gap | Description | SH-# | Priority |
|-----|-------------|------|----------|
| 1 | Stmt::Let alloca type — use `Struct(name)` not `Ptr(Struct)` | SH-11.12 | HIGH |
| 2 | Stmt::Let store_ty — use `Some(Struct(_))` for struct alloca | SH-11.13 | HIGH |
| 3 | Expr::Ident struct alloca — return alloca pointer directly | SH-11.14 | HIGH |
| 4 | StructLit field store — use `emit_struct_copy` for struct fields | SH-11.15 | HIGH |

These don't block `fides-c.exe` (built by Rust compiler with all fixes), but
they block `stage1.exe` (built by `fides-c.exe` using the Fides lowerer).

### Phase 2: Self-hosted compiler compiles hello.fd

Once all crashes are fixed:

```bash
.\release\fides-c.exe ccompile tests\hello.fd -o release\hello_test
# Should produce release/hello_test.c without crashing
```

**This is not yet success.** No crash means the compiler ran to completion,
but the output could still be:
- Empty file
- Syntactically invalid C (tcc won't compile it)
- Valid C but wrong output (prints garbage instead of "Hello, Fides!")

### Phase 3: Verify hello.fd output is correct

```bash
C:\tcc\tcc.exe -O2 -w -o release\hello_test.exe release\hello_test.c
.\release\hello_test.exe
# Expected: Hello, Fides!50y is big
```

**If tcc fails:** The generated C is syntactically invalid. Debug the codegen
path that produced the bad C.

**If output is wrong:** The generated C compiles but produces wrong results.
Compare `release/hello_test.c` against the Rust compiler's output:
```bash
.\target\release\fidesc.exe ccompile tests\hello.fd -o release\hello_rust
diff release\hello_test.c release\hello_rust.c
```
The diff shows exactly which codegen path is wrong.

**If output is correct:** First real milestone. The self-hosted compiler
correctly compiles a simple program.

### Phase 4: Self-hosted compiler compiles itself (stage1)

```bash
.\release\fides-c.exe ccompile compiler\main.fd -o release\stage1
# Should produce release/stage1.c (~13MB)
C:\tcc\tcc.exe -O2 -w -o release\stage1.exe release\stage1.c C:\tcc\lib\ws2_32.def
```

This is the hardest phase. The compiler source is 39.5K LOC across 18 modules.
Expect 10-30 more crashes of the same struct-by-value pattern, plus:

- **HashMap operations** — iteration order, bucket management with struct values
- **Deep recursion** — parser and typechecker on large inputs
- **Large Vec operations** — growing, resizing with struct elements
- **String building** — concatenation, formatting with many intermediate values

**Debugging approach for this phase:**
1. The crash stack trace shows which function crashed
2. Find the function in `release/fides-c.c` (the generated C)
3. Add `fprintf(stderr, ...)` debug prints to trace parameter values
4. Rebuild with tcc, run, identify the bad parameter
5. Fix in `compiler/*.fd`, rebuild from Rust, repeat

### Phase 5: Verify stage1 produces correct output

```bash
# stage1 compiles hello.fd
.\release\stage1.exe ccompile tests\hello.fd -o release\hello_s1
C:\tcc\tcc.exe -O2 -w -o release\hello_s1.exe release\hello_s1.c
.\release\hello_s1.exe
# Expected: Hello, Fides!50y is big

# Compare stage1's C output against the Rust compiler's C output
diff release\hello_s1.c release\hello_rust.c
# Should be identical (or equivalent)
```

**If they differ:** The self-hosted compiler generates slightly different C
than the Rust compiler. This is not necessarily wrong (different variable
names, different ordering) but must be verified as semantically equivalent.

### Phase 6: Verify the fixpoint

The fixpoint test: the compiler compiles itself, and the output is stable
across generations.

```bash
# stage1 (Rust-compiled) compiles the compiler → stage2
.\release\fides-c.exe ccompile compiler\main.fd -o release\stage2
C:\tcc\tcc.exe -O2 -w -o release\stage2.exe release\stage2.c C:\tcc\lib\ws2_32.def

# stage2 (self-compiled) compiles the compiler → stage3
.\release\stage2.exe ccompile compiler\main.fd -o release\stage3
C:\tcc\tcc.exe -O2 -w -o release\stage3.exe release\stage3.c C:\tcc\lib\ws2_32.def

# THE FIXPOINT CHECK
diff release\stage2.c release\stage3.c
```

**If identical:** Fides is self-reproducing. The compiler can build itself,
and the output is stable. This is the bootstrap fixpoint.

**If not identical:** The compiler works but is not stable. Common causes:
- HashMap iteration order (non-deterministic) → sort keys before output
- Uninitialized memory (random values) → zero-initialize all allocas
- Pointer addresses in output (different each run) → don't print pointers
- Float formatting differences → use deterministic formatting

---

## What "Success" Actually Means

No crash does NOT mean it works. There are three classes of bugs:

| Class | Symptom | Detection | Difficulty |
|-------|---------|-----------|------------|
| **Crash** | Segfault, access violation | Stack trace | Easy — trace points to the bug |
| **Wrong output** | Program runs but prints wrong thing | Compare against expected | Medium — find which codegen path is wrong |
| **Fixpoint divergence** | stage2.c ≠ stage3.c | `diff stage2.c stage3.c` | Hard — non-determinism, uninit memory |

The crash-fix loop (Phase 1) is the easy part. Each crash has a stack trace
that points to the problem. The hard part comes after — silent wrong output
and fixpoint divergence.

### Milestones and what they prove

| Milestone | What it proves | Probability after crashes stop |
|-----------|---------------|-------------------------------|
| No crash on hello.fd | Compiler pipeline runs end-to-end | ~60% |
| tcc compiles the output | Generated C is syntactically valid | ~50% |
| hello.exe prints correct output | Codegen is correct for simple programs | ~40% |
| No crash on compiler/main.fd | Compiler handles 39.5K LOC | ~20% |
| stage1 output matches Rust compiler | Codegen is correct for complex programs | ~10% |
| stage2.c == stage3.c | Compiler is stable across generations | ~5% |

These probabilities are for "works on first try after crashes stop." Each
failure mode is debuggable — the probabilities just reflect that there will
be more work after the crashes are fixed.

---

## Debugging Workflow

### When a crash occurs

1. **Read the stack trace** — it shows which function crashed and the call chain
2. **Find the function in the generated C** — `Select-String release\fides-c.c -Pattern "function_name"`
3. **Check the function signature** — does it have an SRET parameter (`void* v0`)? Does the caller pass one?
4. **Check for Vec-of-struct** — is a struct being returned from `Vec::get(i)` or pushed via `Vec::push`?
5. **Check for enum extraction** — is a struct-typed enum field being loaded with `Inst::Load` instead of `load_enum_field`?
6. **Add debug prints** — `fprintf(stderr, "[DBG] %s: v0=%p v1=%p\n", __func__, v0, v1);` at the start of the function
7. **Rebuild with tcc, run, inspect** — which parameter is null or garbage?
8. **Fix in `compiler/*.fd`** — the self-hosted source, not the generated C
9. **Rebuild from Rust** — `cargo build --release && fidesc ccompile compiler\main.fd -o release\fides-c && tcc ...`

### When output is wrong (no crash)

1. **Compare against Rust compiler output:**
   ```bash
   .\target\release\fidesc.exe ccompile tests\hello.fd -o release\hello_rust
   .\release\fides-c.exe ccompile tests\hello.fd -o release\hello_self
   diff release\hello_rust.c release\hello_self.c
   ```
2. **The diff shows which codegen path diverges**
3. **Trace the divergence to a specific function in `compiler/*.fd`**
4. **Fix and rebuild**

### When the fixpoint doesn't converge

1. **Diff stage2.c and stage3.c:**
   ```bash
   diff release\stage2.c release\stage3.c > fixpoint-diff.txt
   ```
2. **Categorize the differences:**
   - Variable naming differences → cosmetic, can be normalized
   - Ordering differences → HashMap iteration, need sorted output
   - Value differences → uninitialized memory or a codegen bug
3. **Fix the root cause of each category**
4. **Re-run the fixpoint test**

---

## Known Bugs and Gotchas

### The Vec runtime architecture (root cause of Vec-of-struct)

The C backend's Vec stores elements as `void**` — an array of 8-byte pointers.
Each element occupies exactly one slot:

```c
void p_vec_push(PVec* v, void* val) {
    // ... resize ...
    v->data[v->len++] = val;  // stores ONE pointer (8 bytes)
}

void* p_vec_get(PVec* v, int64_t idx) {
    return v->data[idx];  // returns ONE pointer (8 bytes)
}
```

For scalar types (i64, bool, f64, str, pointers), this is correct — each
value fits in 8 bytes. For struct types larger than 8 bytes (e.g.
`StringString` = 16 bytes, `Func` = 40+ bytes), this **truncates** — only
the first 8 bytes are stored, the rest is lost.

The `word_repr` function in both lowerers converts `Struct(_)` to
`Ptr(Struct(_))` for Vec_get's return type, so `Vec_get` returns a pointer.
But `p_vec_push` still only stores 8 bytes, so the pointer points to
incomplete data.

**Fix:** Heap-allocate structs before pushing to Vec. The lowerer must:
- Before `Vec::push(struct_val)`: if element type is `Struct(_)`,
  `p_malloc(struct_size)` + `memcpy(malloc_ptr, struct_val, struct_size)` +
  `p_vec_push(vec, malloc_ptr)`
- After `Vec::get(i)`: if element type is `Struct(_)`, the return value is
  already a pointer to the heap-allocated struct data. Use it directly.

This is consistent with how `Box<T>` already works.

### The Vec-of-struct problem (gotcha 3.8) — CURRENT CRASH

**This is the current crash.** `map_str_str_get` calls `Vec::get(i)` to get a
`StringString` struct (16 bytes: two `str` pointers). But `p_vec_get` returns
8 bytes. The `.key` field is garbage, and `p_str_equal(garbage, key)` crashes.

**Vec::set / Vec::get struct handling status:**

| Path | Rust lowerer | Fides lowerer |
|------|-------------|---------------|
| `vec.set(i, struct)` method | ✓ HANDLED (boxes value) | ✓ HANDLED (boxes value) |
| `vec[i] = struct` indexing | ✗ NOT HANDLED (calls Vec_set directly) | ✗ NOT HANDLED (calls Vec_set directly) |
| `vec.get(i)` method (struct elem) | ✓ HANDLED (returns `word_repr(elem_ty)`) | ✓ HANDLED (returns `elem_ty` = `Struct(_)`) |
| `vec[i]` indexing (struct elem) | ✓ HANDLED (returns `word_repr(elem_ty)`) | ✗ NOT HANDLED (returns hardcoded `I64`) |
| `for x in vec` (struct elem) | ✓ HANDLED (returns `word_repr(elem_ty)`) | ✗ NOT HANDLED (returns hardcoded `I64`) |
| `vec[i] += x` compound (struct elem) | ✗ NOT HANDLED (returns `I64`) | ✗ NOT HANDLED (returns `I64`) |

**Fixes needed:**

In BOTH lowerers:
1. `vec[i] = struct` indexing — box the struct before `Vec_set` (same as push/insert/set method)
2. `vec[i] += x` compound — use `word_repr(elem_ty)` for `Vec_get` return type

In Fides lowerer only:
3. `vec[i]` indexing — use `word_repr(elem_ty)` instead of `IrType::I64` for `p_vec_get` return (line 3229)
4. `for x in vec` — use `word_repr(elem_ty)` instead of `IrType::I64` for `p_vec_get` return (line 4518)

**Affected sites in the compiler source:**
- `map_str_str_get` in `compiler/typechecker.fd` — `Vec<StringString>::get(i)` (method call — HANDLED)
- `map_str_str_set` in `compiler/typechecker.fd` — `Vec<StringString>::get(i)` (method call — HANDLED)
- `map_fieldvis_get` in `compiler/typechecker.fd` — `Vec<FieldVisEntry>::get(i)` (method call — HANDLED)
- Any `for x in vec` loop where `x` is a struct type (NOT HANDLED in Fides)
- Any `vec[i]` indexing where the element is a struct type (NOT HANDLED in Fides)

**Fix:** See "Vec runtime architecture" above. Heap-allocate structs before
pushing. Both lowerers need this fix.

### The enum variant extraction problem (gotcha 3.4)

`Inst::Load` with `IrType::Struct(_)` only reads 8 bytes because the C backend
treats `IrType::Struct` as a pointer type (`struct Foo_s*`).

**Fix:** Use `load_enum_field()` helper — allocates a buffer and uses
`Inst::Store` with `store_ty: Some(Struct(_))` to trigger `memcpy`.
**Status: FIXED in both lowerers.**

### The `__env` hidden parameter (gotcha 3.6)

User functions receive a hidden `__env: *u8` first argument (after SRET for
struct returns). It is always `0`. Do not pass `__env` to extern or `p_*`
functions.

### Debug prints in the parser

The self-hosted parser currently has `DEBUG parse_unary` and `DEBUG assign`
prints (visible in stderr output). These should be removed once parsing is
confirmed stable.

---

## Fides Lowerer Gaps (compiler/lower.fd)

The Rust lowerer (`src/lower.rs`) has 4 fixes that the Fides lowerer
(`compiler/lower.fd`) is missing. These don't cause crashes yet (the
self-hosted compiler crashes before reaching the codegen stage), but they
WILL cause crashes once the Vec-of-struct problem is fixed and the
typechecker starts working.

The pattern: `fides-c.exe` is built by the Rust compiler, so it has the
Rust lowerer's fixes. It can compile `compiler/*.fd` correctly. But the
resulting `stage1.exe` uses the Fides lowerer, which is missing these fixes.
So `stage1.exe` will generate bad C code for itself.

### Gap 1: Stmt::Let alloca type (Fides line ~2128)

When a `let` binding has a declared struct type and the value is a pointer to
that struct, the Fides lowerer uses `Ptr(Struct)` (8 bytes) for the alloca.
The Rust lowerer uses `Struct(name)` (full size) when the pointer points to
the same struct type.

**Fides (broken):**
```fides
if ir_type_is_aggregate(declared_ir) && ir_type_is_ptr(val_ty) {
    ir_ty = val_ty;  // Ptr(Struct) — only 8 bytes!
}
```

**Rust (fixed, line 1534-1543):**
```rust
if d.is_aggregate() && v.is_ptr() {
    match v {
        IrType::Ptr(inner) if inner.as_ref() == &d => d,  // Struct (full size)
        _ => v.clone(),
    }
}
```

### Gap 2: Stmt::Let store_ty (Fides line ~2145)

The Fides lowerer stores the let value with `OptIrType::None`, which only
copies 8 bytes. The Rust lowerer uses `Some(Struct(name))` for struct-typed
allocas, which triggers `memcpy`.

**Fides (broken):**
```fides
var _s = Lowerer_emit_inst(self, Inst::Store(alloca, val, OptIrType::None));
```

**Rust (fixed, line 1567-1577):**
```rust
let store_ty = match &ir_ty {
    IrType::Struct(_) => Some(ir_ty.clone()),
    _ => None,
};
self.emit_inst(Inst::Store { ptr: alloca, value: val, store_ty });
```

### Gap 3: Expr::Ident struct alloca (Fides line ~2686)

When loading a struct-typed alloca, the Fides lowerer does `Inst::Load(alloca,
Struct)` which reads 8 bytes (the tag/pointer). The Rust lowerer returns the
alloca pointer directly — the alloca IS the struct data.

**Fides (broken):**
```fides
return Lowerer_emit_inst(self, Inst::Load(alloca, load_ty));
// For Struct(_) alloca, reads 8 bytes, not full data
```

**Rust (fixed, line 3244-3249):**
```rust
if matches!(&load_ty, IrType::Struct(_)) {
    return *alloca;  // alloca pointer IS the struct value
}
return self.emit_inst(Inst::Load { ptr: *alloca, ty: load_ty });
```

### Gap 4: StructLit field store (Fides line ~3245)

The Fides lowerer stores struct fields with `OptIrType::None`, truncating
struct-typed fields to 8 bytes. The Rust lowerer uses `emit_struct_copy` for
struct-typed fields.

**Fides (broken):**
```fides
var _s = Lowerer_emit_inst(self, Inst::Store(field_ptr, field_val, OptIrType::None));
```

**Rust (fixed, line 4377-4378):**
```rust
if let IrType::Struct(ref field_struct_name) = field_ty {
    self.emit_struct_copy(field_val, field_ptr, field_struct_name);
} else {
    self.emit_inst(Inst::Store { ptr: field_ptr, value: field_val, store_ty: None });
}
```

### Additional Store(..., None) sites that MAY need fixes

Beyond the 4 gaps above, there are additional `Inst::Store(..., None)` calls
in the Fides lowerer that could truncate structs. These are lower priority
because they may not be hit with struct values in the current compiler source:

| Line | Code Path | Risk | Notes |
|------|-----------|------|-------|
| 2207 | Stmt::LetTuple | LOW | Tuples are arrays of i64 |
| 2352 | Stmt::Break | MEDIUM | Break value could be a struct |
| 2508 | Stmt::Assign field | MEDIUM | Field assignment of a struct |
| 3349 | TupleLit element | LOW | Tuples are arrays of i64 |
| 3881 | ArrayLit element | MEDIUM | Array of structs |
| 4432 | for loop array element | MEDIUM | Array of structs |
| 4488 | for loop Vec element | **HIGH** | Vec-of-struct (current crash) |

The Rust lowerer has the same `store_ty: None` calls at corresponding sites.
They haven't been fixed because the current compiler source doesn't hit them
with struct values. But the self-hosted compiler source might.

### Fix Order

1. **Vec-of-struct (both lowerers)** — fix `p_vec_push`/`p_vec_get` to handle
   struct elements via heap allocation. This unblocks typechecking.
2. **Port Gaps 1-4 to Fides lowerer** — needed for `stage1.exe` to generate
   correct C code for itself (fixpoint).
3. **Check additional Store sites** — if crashes occur after Gaps 1-4 are
   ported, check the "Additional" table above.

---

## Self-Host Module Status

All 18 modules rewritten in Fides. 23/23 self-host tests pass on
interpreter + PE emitter.

| Module | Fides LOC | Status |
|--------|-----------|--------|
| `token.fd` | 517 | DONE |
| `diag.fd` | 594 | DONE |
| `json.fd` | 628 | DONE |
| `fmt.fd` | 1007 | DONE |
| `ast.fd` | 920 | DONE |
| `lexer.fd` | 673 | DONE |
| `parser.fd` | 4800 | DONE |
| `ir.fd` | 2139 | DONE |
| `typechecker.fd` | 4848 | DONE (compute_func_effects is no-op) |
| `lower.fd` | 4444 | DONE |
| `mono.fd` | 2422 | DONE |
| `borrowck.fd` | 1731 | DONE |
| `pir.fd` | 2432 | DONE |
| `opt.fd` | 6022 | DONE |
| `codegen_c.fd` | 4529 | DONE |
| `compiler.fd` | 1641 | DONE (apply_cfg_filter is no-op) |
| `comptime_eval.fd` | 35 | DONE |
| `main.fd` | 120 | DONE (entry point) |
| **Total** | **39,502** | **18/18 modules** |

---

## Phase Summary

| Phase | Name | Status |
|-------|------|--------|
| SH-0 | Pre-Flight Audit | DONE |
| SH-1 | Debugging Infrastructure | DONE |
| SH-2 | Test Harness per Module | PARTIAL |
| SH-3 | Fides Language Gaps | DONE |
| SH-4 | Fides Stdlib Gaps | DONE |
| SH-5 | C Codegen Hardening | DONE |
| SH-6 | Rewrite Leaf Modules | DONE |
| SH-7 | Rewrite Frontend | DONE |
| SH-8 | Rewrite Middle-End | DONE |
| SH-9 | Rewrite Back-End | DONE |
| SH-10 | Self-Host Breaking Bugs | IN PROGRESS |
| SH-11 | Bootstrap Crash Fix | IN PROGRESS |
| SH-12 | Safety & Crash Prevention | DONE |

### SH-11 sub-tasks

| # | Task | Status |
|---|------|--------|
| SH-11.1 | NULL check after p_file_read | DONE |
| SH-11.2 | Progressive stage reporting | DONE |
| SH-11.3 | Recursion limit in C codegen | DONE |
| SH-11.4 | OOM detection + crash handlers | DONE |
| SH-11.5 | SRET calling convention | DONE |
| SH-11.6 | Enum layout from field sizes | DONE |
| SH-11.7 | Enum inline storage + extraction | DONE (both Rust + Fides) |
| SH-11.8 | Build infrastructure | DONE |
| SH-11.9 | Fixpoint verification | BLOCKED (crashes not yet fixed) |
| SH-11.10 | Typechecker crash (box_expr_unbox) | DONE (compute_func_effects is no-op) |
| SH-11.11 | Vec-of-struct truncation (both lowerers) | **CURRENT** — map_str_str_get crash |
| SH-11.12 | Port Gap 1: Stmt::Let alloca type to Fides | TODO — needed for fixpoint |
| SH-11.13 | Port Gap 2: Stmt::Let store_ty to Fides | TODO — needed for fixpoint |
| SH-11.14 | Port Gap 3: Expr::Ident struct alloca to Fides | TODO — needed for fixpoint |
| SH-11.15 | Port Gap 4: StructLit field store to Fides | TODO — needed for fixpoint |
| SH-11.16 | Check additional Store(..., None) sites | TODO — after Gaps 1-4 are ported |

---

## After the Fixpoint

Once Fides is self-reproducing:

1. **Remove debug prints** from `compiler/parser.fd` and other modules
2. **Re-enable `apply_cfg_filter`** (currently no-op) if cross-compilation is needed
3. **Re-enable `compute_func_effects`** (currently no-op) for effect checking
4. **The Rust compiler becomes the test oracle** — run both, compare output
5. **Start working on the PE emitter rewrite** (Phase 10B)
6. **Start building the ecosystem** (packages, docs, playground)

---

## Build Directory Layout

```
target/              ← Rust compiler output (fidesc.exe) — gitignored
release/             ← Self-hosted compiler output — gitignored
  fides-c.c          ← Generated C (13MB)
  fides-c.exe        ← Self-hosted compiler (linked by tcc)
  stage1.c / .exe    ← First self-compiled generation
  stage2.c / .exe    ← Second generation (for fixpoint test)
  stage3.c / .exe    ← Third generation (must equal stage2)
compiler/            ← Self-hosted compiler source (18 .fd modules)
tests/compiler/      ← Self-host module tests (23 .fd files)
tests/selfhost/      ← Harness/probe files for differential testing
stdlib/              ← Fides standard library (69 .fd modules)
```
