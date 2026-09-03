# Fides Self-Hosted Compiler — Known Bugs, Gotchas, and History

> Things that have broken before and how to avoid re-introducing them.

---

## 1. Fixed Bugs (Do Not Re-Introduce)

### 1.1 `STATUS_ACCESS_VIOLATION` in `Lexer_tokenize`

**Symptom:** `release/fides-c.exe` crashed with 0xC0000005 when compiling any `.fd` file.

**Causes (plural):**
- `p_file_read("stdlib/core.fd")` could return `NULL` without a check.
- Deep recursion in the C codegen had no limit.
- `p_malloc` failures were not detected / did not abort.
- The Windows stack was too small.

**Fixes applied:**
- NULL check after `p_file_read` in `compiler.fd`.
- Recursion limit (`p_max_recursion = 10000`) + abort in `compiler/codegen_c.fd`.
- OOM abort on all `p_malloc` failures in `compiler/codegen_c.fd`.
- Vectored exception handler (Windows) / signal handler (Linux) in both `compiler/codegen_c.fd` and `src/codegen_c.rs`.
- Windows 16 MB stack thread wrapper in `src/codegen_c.rs`.
- PE stack patching to 256 MB after tcc links the bootstrap binary.

### 1.10 Enum variant extraction truncates struct payloads

**Symptom:** `Parse error: assignment target must be an identifier or array index at line 298 col 15` for `*p0 = 0;` in `stdlib/core.fd`.

**Root cause:** When matching `ParseResultExpr::Ok(e) => { ... }`, the lowerer used `Inst::Load` for `e` (which is `Expr`). Because the C backend treats `IrType::Struct(_)` as a pointer, `Load` only read 8 bytes (the `Expr` tag), not the full 80-byte `Expr`.

**Fix applied (Sep 3, 2026):**
- `compiler/lower.fd`: Added `Lowerer_load_enum_field()` helper — allocates a buffer and uses `Inst::Store` with `store_ty: Some(field_ty)` to copy the full inline struct data via `memcpy`.
- `compiler/lower.fd`: Fixed `Lowerer_pattern_binding_ir_types()` to return `IrType::Struct(_)` as-is (not `word_repr`) for struct-typed fields, so `load_enum_field` knows to copy the full data.
- `compiler/lower.fd`: Fixed `Lowerer_emit_struct_copy()` to use `load_enum_field` for struct-typed fields (was using `Load` which only reads 8 bytes).
- `compiler/codegen_c.fd`: Fixed `Inst::Store` handler to honor the explicit `store_ty` parameter and emit `memcpy` for `IrType::Struct(_)`. Also checks the pointer's pointee type (matching the Rust codegen behavior).

**Result:** The self-hosted compiler now parses `stdlib/core.fd` successfully. The `*p0 = 0;` deref assignment parse error is resolved.

### 1.3 Undersized enum layouts

**Symptom:** Struct-typed enum variants were being truncated during SRET copies.

**Root cause:** `Lowerer_make_enum_layout` computed `max_fields` (count of fields) and allocated 8 bytes per field. It did not account for the actual size of struct/enum-typed fields.

**Fix:** Compute `max_payload_size` from actual field sizes, looking up nested struct layouts. Use iterative fixpoint computation for nested enum/struct types.

### 1.4 Missing SRET for caller-side function calls

**Symptom:** Functions returning structs were called without passing the hidden SRET pointer, so the result was discarded / corrupted.

**Root cause:** `Expr::Call` and `Expr::MethodCall` lowering emitted `Inst::Call(callee, args, ret_ty, eff)` with `ret_ty = Struct` directly, instead of allocating a buffer, prepending it, and using `IrType::Void`.

**Fix:** Added SRET handling in three call lowering paths:
- `Expr::Call` in `compiler/lower.fd`
- `Expr::MethodCall` receiver path in `compiler/lower.fd`
- Associated function calls like `Vec::new()` in `compiler/lower.fd`

### 1.5 Pre-registration of impl method signatures

**Symptom:** Calls to `self.parse_unary()` returned the wrong / unknown type, so SRET handling was not triggered.

**Root cause:** `Lowerer_lookup_func_ret_ty_ir` only found functions pre-registered in the IR module. Free functions were pre-registered, but impl and trait impl methods were not.

**Fix:** Pre-register mangled impl and trait impl method signatures (`StructName_method` / `TypeName_method`) in the IR module before lowering function bodies.

### 1.6 `PIR L2 GetFieldPtr` offset bug

**Symptom:** Native codegen produced wrong struct field offsets.

**Root cause:** `pir.rs` previously used `field_index * 8` for all offsets, which was wrong once struct fields could be larger than 8 bytes (e.g. enum-typed fields).

**Fix:** `pir.rs` now looks up real offsets from `struct_layouts`.

### 1.7 Native codegen REX-prefix bug

**Symptom:** `p_json_at` produced a wrong instruction encoding (`0x49` instead of `0x4C`).

**Fix:** Corrected the REX prefix in the hand-coded x86-64 backend.

### 1.8 `opaque_values` stale string bug in interpreter

**Symptom:** Strings were lost during SRET operations.

**Root cause:** `Value::Str` entries in `opaque_values` were not cleared when a non-opaque value was stored to the same address.

**Fix:** Clear `self.opaque_values.remove(&addr)` before storing a non-opaque value.

### 1.9 Register allocator constant clobbering

**Symptom:** Constants were clobbered by intervening instructions.

**Fix:** Constant rematerialization (`Location::Remat`) instead of spilling/reloading.

---

## 2. Current Open Blocker

### 2.1 ~~Enum variant extraction truncates struct payloads~~ RESOLVED (Sep 3, 2026)

**Status:** Fixed. See section 1.10 above for details.

### 2.2 Crash in `check_cfg` / `apply_cfg_filter`

**Status:** Open.

**Symptom:** Self-hosted compiler parses `stdlib/core.fd` successfully but crashes:
```
=== CRASH: 0xC0000005 ===
Access violation at address 0x11
Call stack depth: 6
  [0] main
  [1] cmd_ccompile
  [2] ccompile_source_with_path
  [3] build_program_with_path
  [4] apply_cfg_filter
  [5] check_cfg
```

**Likely cause:** `apply_cfg_filter` builds new Vecs by pushing `Func`/`StructDef`
structs from `program.funcs.get(i)`. These are large structs containing pointers
(`Vec<Param>`, `Vec<Stmt>`, etc.) that may dangle when stack frames are reused
(gotcha 3.8: "Vec of structs requires heap allocation"). The address 0x11
suggests a null pointer + small offset dereference, possibly from a corrupted
struct field.

**Next step:** Investigate whether `apply_cfg_filter` needs the same struct-boxing
fix applied to other Vec push sites, or if `check_cfg` is dereferencing a null
pointer in `annotations.get(i).name` or `.args`.

**Diagnosis confirmed by code inspection:**

1. **Match binding sites use `Inst::Load` for struct-typed payloads** (`compiler/lower.fd`):
   - `Lowerer_lower_match` statement form at **lines 3429-3430**:
     ```fides
     var field_ptr: i64 = Lowerer_emit_inst(self, Inst::GetFieldPtr(match_val, bi + 1, field_ty));
     var field_val: i64 = Lowerer_emit_inst(self, Inst::Load(field_ptr, field_ty));
     ```
   - Match expression form at **lines 4463-4464**:
     ```fides
     var field_ptr: i64 = Lowerer_emit_inst(self, Inst::GetFieldPtr(match_val, bi + 1, field_ty));
     var field_val: i64 = Lowerer_emit_inst(self, Inst::Load(field_ptr, field_ty));
     ```
   When `field_ty` is `IrType::Struct(_)` (e.g. `Expr` inside `ParseResultExpr::Ok(e)`), this only reads 8 bytes because the C backend treats `IrType::Struct` as a pointer.

2. **`compiler/codegen_c.fd` `Inst::Store` ignores the explicit `store_ty` and does NOT emit `memcpy` for `IrType::Struct`** (`compiler/codegen_c.fd` **line 1961**):
   ```fides
   Inst::Store(ptr, value, _) => {  // <-- third OptIrType ignored!
       ...
       if is_ptr_type(val_ty) { ... }
       else if val_ty == IrType::F64 { ... }
       ...
       else { *(int64_t*)p = (int64_t)v; }  // <-- struct stored as 8 bytes
   }
   ```
   The Rust reference (`src/codegen_c.rs` **lines 1498-1556**) handles `IrType::Struct(name)` with `memcpy`.

3. **`c_type(IrType::Struct(name))` returns a pointer type** (`compiler/codegen_c.fd` **line 146**):
   ```fides
   IrType::Struct(name) => { return "struct " + name + "_s*"; }
   ```
   This makes `Inst::Load` with a struct type read only the first 8 bytes of the inline data, not the full struct.

**Fix required (two parts):**

A. **In `compiler/lower.fd`:** add a `load_enum_field()` helper and use it at the two match binding sites. For struct-typed fields, allocate a buffer and use `Inst::Store(ptr=buf, value=field_ptr, store_ty=Some(field_ty))` to trigger a full struct copy.

B. **In `compiler/codegen_c.fd`:** modify `Inst::Store` to honor the explicit `store_ty` and emit `memcpy` when the store type is `IrType::Struct(name)`.

**Reference implementation:** `src/lower.rs` `load_enum_field()` (lines 246-271) and `src/codegen_c.rs` `Inst::Store` (lines 1498-1556).

**Files to modify:**
- `compiler/lower.fd` — add `load_enum_field()` and use it at lines 3429-3430 and 4463-4464
- `compiler/codegen_c.fd` — honor `store_ty` in `Inst::Store` and emit `memcpy` for `IrType::Struct(name)`
- `compiler/ir.fd` — already supports `Inst::Store(i64, i64, OptIrType);` no changes needed

**Why this is the right direction:**
- The SRET convention is already correct (the `parse_unary` call site allocates an 80-byte buffer and passes it).
- The enum layout is already correct (80-byte `Expr` payload fits in the `ParseResultExpr` layout).
- The only remaining break is *extracting* the payload from the enum. The data is written correctly; the read is wrong.

### Fix recipe for the current blocker

#### Step A — Add `load_enum_field()` to `compiler/lower.fd`

Insert a helper near the top of the file (after the existing `box_*_unbox` helpers):

```fides
// Load a field from an enum variant's inline storage.
// When the field type is a struct (enum), the data is stored inline within
// the parent enum. A plain Inst::Load with a struct type only reads 8 bytes
// (a pointer) because the C codegen treats IrType::Struct as a pointer type.
// To correctly extract the full struct, we allocate a new buffer and use
// Inst::Store with the explicit store_ty to emit a memcpy in the C backend.
pub fn Lowerer_load_enum_field(self: &mut Lowerer, field_ptr: i64, field_ty: IrType) -> i64 {
    match field_ty {
        IrType::Struct(_) => {
            var buf: i64 = Lowerer_emit_inst(self, Inst::Alloca(field_ty, Lowerer_current_region(self)));
            Lowerer_emit_inst(self, Inst::Store(buf, field_ptr, OptIrType::Some(field_ty)));
            return buf;
        }
        _ => {
            return Lowerer_emit_inst(self, Inst::Load(field_ptr, field_ty));
        }
    }
}
```

Then at the two match binding sites, replace:
```fides
var field_val: i64 = Lowerer_emit_inst(self, Inst::Load(field_ptr, field_ty));
```
with:
```fides
var field_val: i64 = Lowerer_load_enum_field(self, field_ptr, field_ty);
```

#### Step B — Update `Inst::Store` in `compiler/codegen_c.fd`

At **line 1961**, change the match to honor the explicit `store_ty`:

```fides
        Inst::Store(ptr, value, store_ty) => {
            var p: str = val_name(self, ptr);
            var v: str = val_name(self, value);

            // Determine the effective store type. Prefer the explicit store_ty
            // parameter; fall back to the value's type.
            var effective_ty: IrType = value_ty_for_id(func, value);
            match store_ty {
                OptIrType::Some(ty) => { effective_ty = ty; }
                OptIrType::None => {}
            }

            match effective_ty {
                IrType::Struct(sname) => {
                    // Struct store: copy the full struct content, not just 8 bytes.
                    // The value is a pointer to the source struct data, and the
                    // destination is a pointer to the target buffer.
                    var size: i64 = 8;
                    var i: i64 = 0;
                    while i < self.struct_layouts.len() {
                        if str_equal(self.struct_layouts.get(i).name, sname) {
                            size = self.struct_layouts.get(i).layout.size;
                            break;
                        }
                        i = i + 1;
                    }
                    wln(self, "    memcpy(" + p + ", " + v + ", " + int_to_str(size) + ");");
                }
                IrType::Ptr(_) | IrType::Opaque(_) | IrType::Generic(_, _) => {
                    wln(self, "    *(void**)" + p + " = (void*)" + v + ";");
                }
                ...existing scalar cases...
            }
        }
```

#### Step C — Verify

```bash
cargo build --release
.\build-selfhost.ps1
.\release\fides-c.exe ccompile tests\hello.fd -o release\hello_test
```

If the parse error at `stdlib/core.fd` line 298 is gone, the fix worked. Then run:

```bash
./verify.sh
./verify-selfhost.sh
.\verify-fixpoint.ps1
```

---

## 3. Gotchas

### 3.1 `Box<T>` in compiler source is not the same as language `Box`

- Language `Box::new(x)` is `p_malloc` + `Store`.
- Compiler-source `Box<T>` is a recursive pointer wrapper for AST/IR fields.
- Match patterns on `Box<T>` auto-unbox, but explicit `box_*_unbox()` helpers are needed for `Box<Expr>`, `Box<Type>`, `Box<IrType>`.

### 3.2 `Expr` variants in `lower.fd` are out of sync with `ast.fd`

`box_expr_unbox` in `compiler/lower.fd` (lines 71-111) lists variants that do not exist in `compiler/ast.fd`: `Lambda`, `Throw`, `TryUnwrap`, `Assign`, `CompoundAssign`, `ConstExpr`, `ComptimeGen`, `QuantCast`, `Await`, `Spawn`.

**Action needed:** Reconcile `box_expr_unbox` with `ast.fd:244-283`.

### 3.3 `Str` is a pointer in IR

`Type::Str` lowers to `IrType::Ptr(Box::new(IrType::U8))`. Do not try to pass `str` as a struct.

### 3.4 `Inst::Load` on `IrType::Struct(_)` only reads 8 bytes

This is the source of the current bug. Any code that extracts a struct-typed field from an enum or struct with `GetFieldPtr` + `Load` is almost certainly wrong.

### 3.5 `Inst::Store` with `store_ty: Some(IrType::Struct(_))` must emit `memcpy`

If the C backend does not do this, `load_enum_field()` and `Lowerer_emit_sret_copy` will not actually copy full struct data.

### 3.6 `__env` dummy pointer

User functions receive a hidden `__env: *u8` first argument (after SRET for struct returns). It is always `0` and is used to make `CallPtr` uniform with direct calls.

**Do not pass `__env` to extern or `p_*` functions.**

### 3.7 Effects must be tracked or the optimizer will delete side-effecting calls

`runtime_effects()` in `compiler/lower.fd` sets `Write`, `Alloc`, `IO`, etc. on `p_*` builtins. Without this, the optimizer's DCE pass will remove calls it thinks are pure.

### 3.8 Vec of structs requires heap allocation

When pushing or inserting a struct into a `Vec`, the lowerer heap-allocates the struct before pushing the pointer. Skipping this produces a dangling SRET pointer.

### 3.9 `region` blocks affect allocation region

`Inst::Alloca` uses the current region from `Lowerer_current_region()`. Inside a `region` block, allocations go to the named region; otherwise they go to `Region::Stack`.

### 3.10 Match arm bindings do not leak

`Lowerer_lower_match` saves and truncates `var_ast_types` and other binding lists after each arm to prevent arm-local variables from leaking to later arms.

### 3.11 Progressive stage reporting uses `p_eprint_str`

`compiler/compiler.fd` and `compiler/main.fd` now call `p_eprint_str` to print each stage to stderr. When adding new stages, follow this pattern so we can still tell where a crash occurred.

### 3.12 Do not `git stash`, `git reset --hard`, `git clean`, or force-push

This can wipe user work and generated artifacts. Always commit or ask before destructive git operations.

---

## 4. Files That Require Extra Care

From `AGENTS.md`, these are high/medium risk:

| File | Risk | Why |
|------|------|-----|
| `compiler/lower.fd` | **CRITICAL** | AST → IR; SRET, enum layout, match, try, defer, effects all live here |
| `compiler/ir.fd` | **HIGH** | Core IR; changes ripple to all backends |
| `compiler/codegen_c.fd` | **HIGH** | IR → C; ABI, casting, SRET, struct copy all here |
| `compiler/parser.fd` | **MEDIUM** | Recursive descent; changes can subtly shift parse behavior |
| `compiler/ast.fd` | **MEDIUM** | Canonical AST; `lower.fd` unbox helper is already out of sync |
| `compiler/compiler.fd` | **MEDIUM** | Pipeline driver; import resolution is fragile |

Also high-risk in Rust reference compiler:
- `src/regalloc.rs`, `src/peephole.rs`, `src/elf64.rs`, `src/pe.rs`, `src/pir.rs`, `src/lower.rs`, `src/codegen_c.rs`, `src/codegen_x64.rs`

---

## 5. Common Failure Modes

| Symptom | Likely cause | Where to look |
|---------|--------------|---------------|
| `STATUS_ACCESS_VIOLATION` / segfault | Stack too small, null pointer, OOM, SRET mismatch | `codegen_c.fd` crash handler, PE stack, `lower.fd` SRET |
| Parse errors on valid code | Parser unbox helper out of sync, enum variant truncation | `ast.fd` vs `lower.fd`, `load_enum_field` |
| Wrong output / fixpoint diverges | Enum layout, SRET copy, struct copy, C backend `memcpy` | `lower.fd` layout, `codegen_c.fd` `Store` |
| `p_*` calls deleted by optimizer | Missing `EffectSet` on call | `lower.fd` `runtime_effects()` |
| Vec of structs corrupt | Dangling SRET pointer on `push`/`insert` | `lower.fd` `Vec::push`/`insert` lowering |
| Import not found | Import path resolution order | `compiler.fd` `build_program_with_path()` |
