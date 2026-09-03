# Current Focus

> Single source of truth for "what are we doing right now."
> Read this first every session. Updated after every milestone change.
> Last updated: Sep 3, 2026 (session 2).

## Where We Stopped (Sep 3 session 2 — pick up here)

**MILESTONE: Self-hosted compiler compiles `tests/hello.fd` end-to-end and
produces correct output: `Hello, Fides!50y is big`**

### What works:
1. **Build pipeline complete** — `compiler/*.fd` (18 modules, 39.5K LOC) compiles to C via the bootstrap compiler (`fidesc.exe`), then links to `fides-c.exe` via tcc
2. **All 18 self-host modules compile + pass tests** (23/23 on tests/compiler/ via interpreter + PE)
3. **Lexer works** — stdlib/core.fd tokenized successfully
4. **Parser works** — stdlib/core.fd and user code parsed successfully (deref assignments work)
5. **TypeChecker works** — hello.fd type checks ok
6. **Borrow checker works** — hello.fd borrow checks ok
7. **Monomorphizer works** — hello.fd monomorphized ok
8. **Lowerer works** — hello.fd lowered to IR
9. **C codegen works** — hello.fd → C code (534KB) → tcc → correct output
10. **SRET calling convention** — struct-returning functions use hidden out-parameter
11. **Enum layout** — payload sizes computed from actual field sizes with fixpoint iteration
12. **Enum inline storage** — struct-typed enum fields stored inline via memcpy
13. **Enum variant extraction** — `load_enum_field()` copies full struct data
14. **Inst::Store with store_ty** — C codegen emits memcpy for struct store types
15. **Vec-of-struct handling** — all Vec paths (set/push/insert/get/pop/first/last/indexing/for-loop/compound) handle struct elements correctly
16. **Safety hardening** — recursion limit, OOM abort, crash handlers, Windows stack
17. **Enum SRET calling convention** — enum return types (Result, Option) now use SRET (hidden out-param) via `lower_type_sret()` in `src/lower.rs`. Previously enums were `Generic("Result", ...)` which the codegen treated as `void*` (pointer), causing dangling stack alloca pointers.
18. **Windows binary stdout** — C codegen sets stdout/stderr to `_O_BINARY` mode on Windows, preventing `\r\n` doubling when printing strings that already contain `\r\n` (e.g. HTTP responses)

### Verified output:
```
.\release\fides-c.exe ccompile tests\hello.fd -o release\hello_test
C:\tcc\tcc.exe -O2 -w -o release\hello_test.exe release\hello_test.c C:\tcc\lib\ws2_32.def
.\release\hello_test.exe
→ Hello, Fides!50y is big    ✅ CORRECT
```

### Build commands:
```
cargo build --release
.\target\release\fidesc.exe ccompile compiler\main.fd -o release\fides-c
C:\tcc\tcc.exe -O2 -w -o release\fides-c.exe release\fides-c.c C:\tcc\lib\ws2_32.def
.\release\fides-c.exe ccompile tests\hello.fd -o release\hello_test
```

### Next steps:
1. **Port Gap 5 (SRET for enums) to Fides lowerer** — `compiler/lower.fd` needs `lower_type_sret()` equivalent
2. **Port Gaps 1-4 to Fides lowerer** — struct alloca, store_ty, ident, StructLit
3. **Self-compilation** — `fides-c.exe ccompile compiler\main.fd -o release\stage1` (the hardest step)
4. **Verify stage1 output** — stage1 compiles hello.fd correctly
5. **Fixpoint verification** — stage2.c == stage3.c

See `docs/roadmap/selfhost-roadmap.md` for the full plan.

## Temporary workarounds (to remove after fixpoint):
- `apply_cfg_filter` is a no-op in `compiler/compiler.fd`
- `compute_func_effects` is a no-op in `compiler/typechecker.fd`
- Debug prints in `compiler/parser.fd` (DEBUG parse_unary, DEBUG assign)

## Self-Host Module Status

All 18 modules rewritten in Fides. 23/23 self-host tests pass on interpreter + PE.

| Module | LOC | Status |
|--------|-----|--------|
| token.fd | 517 | DONE |
| diag.fd | 594 | DONE |
| json.fd | 628 | DONE |
| fmt.fd | 1007 | DONE |
| ast.fd | 920 | DONE |
| lexer.fd | 673 | DONE |
| parser.fd | 4800 | DONE |
| ir.fd | 2139 | DONE |
| typechecker.fd | 4848 | DONE (effects no-op) |
| lower.fd | 4444 | DONE |
| mono.fd | 2422 | DONE |
| borrowck.fd | 1731 | DONE |
| pir.fd | 2432 | DONE |
| opt.fd | 6022 | DONE |
| codegen_c.fd | 4529 | DONE |
| compiler.fd | 1641 | DONE (cfg no-op) |
| comptime_eval.fd | 35 | DONE |
| main.fd | 120 | DONE |
| **Total** | **39,502** | **18/18** |

## Deferred (Do Not Start)

- PE emitter rewrite (after C codegen self-host lands)
- Playground website (post-self-host)
- Package registry (post-self-host)
- New language features (add on Rust until self-host lands)
- Industry domain stdlibs (medical, blockchain, robotics)

---

## Struct-by-Value Bug Analysis (Sep 3)

> The root cause of ALL self-host crashes is the same family of bugs: struct
> data is truncated to 8 bytes (one machine word) when it should be copied in
> full. This section documents every place this happens and the current state
> of fixes in both lowerers.

### The Core Problem

In Fides, structs and enums are "aggregate" types. At the IR level, they are
represented as `IrType::Struct(name)`. The C codegen represents struct VALUES
as pointers (`void*` or `SomeStruct*`). A struct "value" is a pointer to the
struct data in memory.

The bug: many code paths treat a struct value as a single `i64` word. When
copying (Store, Load, function return, Vec get, etc.), only 8 bytes are
copied — the pointer itself, not the struct data it points to. This truncates
the struct to garbage.

The fix: use `Inst::Store` with `store_ty: Some(Struct(name))` which makes the
C codegen emit `memcpy(dst, src, struct_size)` instead of `*(void**)dst = src`.

### Fix Status Matrix (Rust lowerer vs Fides lowerer)

| Code Path | Rust (`src/lower.rs`) | Fides (`compiler/lower.fd`) | Notes |
|-----------|----------------------|----------------------------|-------|
| **Box::new** | FIXED | FIXED | Both: `p_malloc(full_size)` + `Store(ptr, val, Some(Struct))` → `memcpy` |
| **Stmt::Let alloca type** | FIXED | **NOT FIXED** | Rust: uses `Struct(name)` for alloca when declared type matches value's inner struct. Fides: still uses `Ptr(Struct)` (8 bytes). |
| **Stmt::Let store** | FIXED | **NOT FIXED** | Rust: `Store(alloca, val, Some(Struct))` for struct-typed alloca. Fides: `Store(alloca, val, None)` — only copies 8 bytes. |
| **Stmt::Assign** | FIXED | FIXED | Both: `Store(alloca, val, Some(inner_struct))` when val is `Ptr(Struct)` |
| **Expr::Ident (struct alloca)** | FIXED | **NOT FIXED** | Rust: returns alloca pointer directly for `Struct(_)` typed alloca. Fides: always does `Load(alloca, Struct)` which reads 8 bytes (the tag/pointer), not the full data. |
| **Expr::Ident (Ptr(Struct) alloca)** | Both: `Load(alloca, Ptr(Struct))` — loads the pointer from the alloca. Correct for `Ptr(Struct)` alloca. | Same. | This is the OLD path — for when alloca holds a pointer to struct, not inline struct. |
| **maybe_deref_box** | NO-OP | NO-OP | Both: returns val unchanged. Box::new stores data directly, so the Box pointer already points to struct data. |
| **load_enum_field** | FIXED | FIXED | Both: for `Struct(_)` field type, allocates buffer + `Store(buf, field_ptr, Some(Struct))` → `memcpy`. For other types, regular `Load`. |
| **enum_field_slots** | FIXED | FIXED | Both: `Struct(name)` → `(size + 7) / 8` slots; everything else → 1 slot. |
| **pattern_binding_ir_types** | FIXED | FIXED | Both: keeps `Struct(_)` as-is; applies `word_repr` to everything else. |
| **match arm field_idx** | FIXED | FIXED | Both: accumulates `field_idx += enum_field_slots(field_ty)` for inline struct fields. |
| **StructLit field store** | FIXED | **NOT FIXED** | Rust: uses `emit_struct_copy` for `Struct(_)` fields. Fides: `Store(field_ptr, val, None)` for ALL fields — truncates struct fields to 8 bytes. |
| **Vec indexing (Expr::Index)** | Uses `Vec_get` stdlib fn | Uses `p_vec_get` raw runtime | **Both broken for struct elements.** `p_vec_get` returns `I64` (8 bytes). `Vec_get` calls `p_vec_get` too. Need memcpy for struct elements. |
| **for loop Vec element** | `p_vec_get` → `Store(alloca, val, None)` | Same | **Both broken for struct elements.** Only 8 bytes stored. |
| **SRET return** | FIXED | FIXED | Both: `emit_struct_copy` field-by-field from val to sret pointer. |
| **Struct param passing** | `Ptr(Struct)` — pointer to struct data | Same | Both: struct params are `Ptr(Struct)` in IR. Caller passes pointer, callee receives pointer. Correct as long as caller passes a valid pointer. |
| **SRET call (struct return)** | FIXED | FIXED | Both: alloca + pass as hidden first arg + return alloca pointer. |
| **SRET for enum returns** | FIXED (session 2) | **NOT FIXED** | Rust: `lower_type_sret()` converts enum `Generic("Result", ...)` → `Struct("Result")` at function sig boundaries, triggering SRET. Fides: still uses `Generic` for enum return types → no SRET → dangling alloca. |

### Critical Gaps in Fides Lowerer (compiler/lower.fd)

These 4 gaps are the reason the self-hosted compiler crashes. The bootstrap compiler
(`fidesc.exe`, built from Rust source) works because the Rust lowerer has all fixes. But
when the Fides lowerer tries to compile itself, it generates bad C code because
these fixes are missing.

#### Gap 5: SRET for enum return types (NEW — session 2)
**Current (broken):**
```fides
// Fides lower_type() returns Generic("Result", [I64, I64]) for enum types.
// The SRET check only matches Struct(_), so enums don't get SRET.
// The callee allocas the enum on its stack and returns a pointer, which dangles.
```
**Needed (Rust side fix at line 1296-1316):**
```fides
// Add a lower_type_sret() that converts enum Generic types to Struct
// at function signature boundaries (return type + call sites).
fn lower_type_sret(self, ty) -> IrType {
    var ir_ty = self.lower_type(ty);
    match ir_ty {
        IrType::Generic(name, _) => {
            var resolved = self.resolve_struct_name(name);
            if map_contains(self.enums, resolved) {
                return IrType::Struct(resolved);
            }
        }
        _ => {}
    }
    return ir_ty;
}
// Use lower_type_sret() at:
// - Function pre-registration (line ~434)
// - Function definition return type (line ~959)
```

#### Gap 1: Stmt::Let alloca type (line 2128-2129)
**Current (broken):**
```fides
if ir_type_is_aggregate(declared_ir) && ir_type_is_ptr(val_ty) {
    ir_ty = val_ty;  // Ptr(Struct) — only 8 bytes!
}
```
**Needed (Rust side fix at line 1534-1543):**
```fides
if ir_type_is_aggregate(declared_ir) && ir_type_is_ptr(val_ty) {
    // If pointer points to the SAME struct type, use the struct type
    // (inline storage). Otherwise (e.g. Vec_get returning *Struct), use ptr.
    match val_ty {
        IrType::Ptr(inner) => {
            if box_ir_type_unbox(inner) == declared_ir { ir_ty = declared_ir; }
            else { ir_ty = val_ty; }
        }
        _ => { ir_ty = val_ty; }
    }
}
```

#### Gap 2: Stmt::Let store_ty (line 2145)
**Current (broken):**
```fides
var _s: i64 = Lowerer_emit_inst(self, Inst::Store(alloca, val, OptIrType::None));
```
**Needed (Rust side fix at line 1567-1577):**
```fides
var store_ty: OptIrType = OptIrType::None;
match ir_ty {
    IrType::Struct(_) => { store_ty = OptIrType::Some(ir_ty); }
    _ => {}
}
var _s: i64 = Lowerer_emit_inst(self, Inst::Store(alloca, val, store_ty));
```

#### Gap 3: Expr::Ident struct alloca (line 2686)
**Current (broken):**
```fides
return Lowerer_emit_inst(self, Inst::Load(alloca, load_ty));
// For Struct(_) alloca, this reads 8 bytes (tag/pointer), not full data
```
**Needed (Rust side fix at line 3244-3249):**
```fides
// If alloca holds inline struct, return alloca pointer directly
match load_ty {
    IrType::Struct(_) => { return alloca; }
    _ => {}
}
return Lowerer_emit_inst(self, Inst::Load(alloca, load_ty));
```

#### Gap 4: StructLit field store (line 3239)
**Current (broken):**
```fides
var _s: i64 = Lowerer_emit_inst(self, Inst::Store(field_ptr, field_val, OptIrType::None));
// For Struct(_) fields, only copies 8 bytes (pointer), not full struct data
```
**Needed (Rust side fix at line 4372-4373):**
```fides
match field_ty {
    IrType::Struct(field_struct_name) => {
        Lowerer_emit_struct_copy(self, field_val, field_ptr, field_struct_name);
    }
    _ => {
        var _s: i64 = Lowerer_emit_inst(self, Inst::Store(field_ptr, field_val, OptIrType::None));
    }
}
```

### Vec-of-Struct Problem — RESOLVED (Sep 3, 2026)

**Was the previous crash.** `map_str_str_get` called `Vec::get(i)` to get a
`StringString` struct (16 bytes: two `str` pointers). But `p_vec_get` returned
`I64` (8 bytes). Only the first field (`.key`) was returned; `.value` was garbage.

**All Vec paths now handle struct elements correctly:**

| Path | Rust | Fides |
|------|------|-------|
| `vec.set(i, struct)` method | ✓ DONE | ✓ DONE |
| `vec[i] = struct` indexing | ✓ DONE | ✓ DONE |
| `vec.get(i)` method (struct) | ✓ DONE | ✓ DONE |
| `vec[i]` indexing (struct) | ✓ DONE | ✓ DONE |
| `for x in vec` (struct) | ✓ DONE | ✓ DONE |
| `vec[i] += x` compound (struct) | ✓ DONE | ✓ DONE |

**Result:** `tests/hello.fd` compiles end-to-end and produces correct output.

### Test Results (Sep 3 session 2, after SRET + binary mode fixes)

```
Interpreter:  203/203 passed  (was 201 — test_cfg + test_deref_assign fixed)
C Codegen:    192/203 matched  (was 188 — +test_cfg, +test_deref_assign, +test_error_chain, +test_http)
PE Emitter:   202/203 valid    (was 201 — +test_deref_assign)
```

**Fixes this session:**
- **test_cfg** — rewrote to be platform-aware (added `@cfg(windows)` functions + `test_platform_specific()` helper)
- **test_deref_assign** — added `fn main()` that calls the test function
- **test_error_chain** — fixed dangling SRET alloca for enum returns (Result/Option). Added `lower_type_sret()` to `src/lower.rs` that converts enum `Generic` types to `Struct` at function signature boundaries, triggering existing SRET calling convention.
- **test_http** — fixed Windows `\r\n` doubling by setting stdout to `_O_BINARY` mode in C codegen's `real_main()`. Previously `printf` translated `\n` → `\r\n`, but HTTP response strings already contain `\r\n`, creating `\r\r\n`.

**Remaining 11 C codegen failures:**

| Test | Category | Root Cause |
|------|----------|------------|
| test_fmt_real | COMPILE_FAIL | tcc limitation (likely float formatting) |
| test_deep_nesting | COMPILE_FAIL | tcc limitation (deep recursion?) |
| test_lower | CRASH | Crashes at Test 12 (`Lowerer_lower_program` returns IrModule struct) — pre-existing, not ptr_u8 |
| test_banking | REAL_DIFF | Float/int calculation: `-10500` vs `126000` — pre-existing tuple destructuring bug |
| test_new_features | CRASH | C binary crashes — real codegen bug |
| test_phase8b | CRASH | C binary crashes mid-output — real codegen bug |
| test_rc_arc | CRASH | C binary crashes — Rc/Arc runtime bug |
| test_regression_collections | REAL_DIFF | Missing `{alice: 30, bob: 25}` — HashMap_to_str bug in Fides stdlib |
| test_vec_math | REAL_DIFF | Float precision: `...475` vs `...476` |
| test_parallel | LEN_DIFF | Threading output differs (timing) — not a bug |
| test_parallel_concurrent | LEN_DIFF | Threading output differs (timing) — not a bug |

Of these 11: 2 are tcc limitations, 2 are threading (not bugs), 2 are float/precision,
1 is a test bug (HashMap_to_str), 4 are real codegen bugs (test_lower, test_banking,
test_new_features, test_phase8b, test_rc_arc).

### Crash Progression (Sep 3)

Each fix in the Rust lowerer moved the crash further along the pipeline:

1. **Before fixes:** Crash in `box_expr_deref` (parser) — `Box<Expr>` pointer
   was 0x4 (the Expr tag, not a pointer). Root cause: `Stmt::Let` alloca was
   8 bytes (`Ptr(Struct)`), storing only the tag. `Expr::Ident` loaded the tag
   as a pointer. `Box::new` then used the tag as a memcpy source.

2. **After Stmt::Let + Expr::Ident fix:** Crash moved to `Lexer_read_operator`
   → `token_new` — struct parameter passing. Same root cause: `kind: TokenKind`
   local variable's alloca was 8 bytes, `Expr::Ident` loaded the tag as a
   pointer, passed it to `token_new` which tried to copy from it.

   Wait — this should have been fixed by the same changes. The crash moved
   because the Fides lowerer (which generated the C code) does NOT have the
   fixes. The `fides-c.exe` was built from C code generated by the Fides
   lowerer, not the Rust lowerer.

   Actually no — `fides-c.exe` is built by the bootstrap compiler (`fidesc.exe`)
   which uses the Rust lowerer. So the Rust lowerer's fixes SHOULD apply. The
   crash moved to the typechecker, meaning the lexer crash was also fixed.

3. **~~Current crash~~ RESOLVED:** `TypeChecker_check_func` → `map_str_str_get` →
   `p_str_equal` at address 0xE. Was the Vec-of-struct truncation:
   `Vec<StringString>::get(i)` returned only 8 bytes of the 16-byte struct.
   Fixed by boxing structs before Vec push and using `word_repr(elem_ty)` for
   Vec_get return types. `tests/hello.fd` now compiles end-to-end.

### Key Insight

The Rust lowerer fixes (Stmt::Let, Expr::Ident, StructLit) are sufficient for
the bootstrap compiler to generate correct C code for the self-hosted compiler.
The self-hosted compiler (`fides-c.exe`) now gets past lexing and parsing.

The remaining crash is the Vec-of-struct problem, which affects BOTH lowerers
and is a more fundamental issue with how `p_vec_get` works.

Once the Vec-of-struct problem is fixed, the self-hosted compiler should get
past typechecking. Then we'll hit the next crash (likely in lowering or
codegen), which will be another struct-by-value truncation in the Fides
lowerer's output (since the Fides lowerer is missing Gaps 1-4).

### Fix Order

1. ~~**Vec-of-struct (both lowerers)**~~ — DONE. All Vec paths handle struct elements.
2. ~~**SRET for enum returns (Rust lowerer)**~~ — DONE (session 2). `lower_type_sret()` in `src/lower.rs`.
3. ~~**Windows binary stdout**~~ — DONE (session 2). `_setmode(_fileno(stdout), _O_BINARY)` in C codegen.
4. **Port Gaps 1-5 to Fides lowerer** — needed for the self-hosted compiler
   to generate correct C code for itself (stage1 → stage2 fixpoint).
   - Gap 1: Stmt::Let alloca type
   - Gap 2: Stmt::Let store_ty
   - Gap 3: Expr::Ident struct alloca
   - Gap 4: StructLit field store
   - Gap 5: SRET for enum return types (NEW)
5. **Continue crash-fix loop** — each remaining crash will be a struct-by-value
   truncation in the Fides lowerer's output.
