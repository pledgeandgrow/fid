# Fides Self-Hosted Compiler — Calling Conventions and Low-Level Machinery

> This is the most important file for agents working on codegen. The
> SRET convention, enum layout, struct copy, and value semantics must stay
> in sync across `lower.fd`, `codegen_c.fd`, and the runtime.

---

## 1. The Big Idea: Everything Is 8 Bytes or a Pointer

In the C backend:
- `IrType::I8/16/32/64`, `U8/16/32/64`, `F64`, `F32`, `Bool`, `Ptr`, `Str`, `Opaque` → a single 8-byte (or smaller) C value.
- `IrType::Struct(_)`, `IrType::Array(_, _)` → a buffer in memory; the IR value is a `void*` pointer to that buffer.
- `IrType::Generic(_, _)` → typically `I64` (for `Future`) or `Opaque` (for `Vec`, `HashMap`, `Tensor`).

This means:
- A plain `Inst::Load` with `IrType::Struct(_)` only reads 8 bytes — just the pointer.
- A plain `Inst::Store` with `IrType::Struct(_)` only stores the pointer, not the data it points to.
- To copy the **full data** of a struct, you must use `Inst::Store` with `store_ty: Some(IrType::Struct(name))` (which the C backend lowers to `memcpy`), or manually copy each field.

This distinction is the root cause of the current enum variant extraction bug.

---

## 2. SRET — Struct Return via Hidden Out-Parameter

### Callee side

When a Fides function returns a `Struct` or `Array`:

1. `Lowerer_lower_func` detects `ret_ty` is `IrType::Struct(_)`:
   ```fides
   var is_struct_ret: bool = false;
   match ret_ty {
       IrType::Struct(_) => { is_struct_ret = true; }
       _ => { is_struct_ret = false; }
   }
   ```
2. A hidden first parameter is added:
   ```fides
   var sret_ty: IrType = IrType::Ptr(Box::new(ret_ty));
   var sret_meta: ValueMeta = ValueMeta_new(sret_ty);
   var sret_val: i64 = IrFunction_new_value(&mut ir_func, sret_meta);
   ir_func.params.push(ValueIdType_new(sret_val, sret_ty));
   self.current_sret = OptValueId::Some(sret_val);
   ```
3. The C backend emits `void fname(void* sret, ...)` because `c_ret_type` returns `"void"` for aggregate types:
   ```fides
   fn c_ret_type(ty: IrType) -> str {
       if is_aggregate_ret(ty) { return "void"; }
       return c_type(ty);
   }
   ```
4. On return, `Lowerer_emit_sret_copy(src_ptr)` copies the result into `sret`:
   ```fides
   pub fn Lowerer_emit_sret_copy(self: &mut Lowerer, src_ptr: i64) {
       match self.current_sret {
           OptValueId::Some(sret) => {
               var sret_ty: IrType = Lowerer_get_value_type(self, sret);
               ... determine struct_name ...
               Lowerer_emit_struct_copy(self, src_ptr, sret, struct_name);
           }
           ...
       }
   }
   ```
5. The function emits `Inst::Return(OptValueId::None)` (void in C).

### Caller side

For any call that returns a `Struct` or `Array`:

1. Allocate a buffer for the result:
   ```fides
   var sret_ptr: i64 = Lowerer_emit_inst(self, Inst::Alloca(IrType::Struct(sname), Lowerer_current_region(self)));
   ```
2. Prepend the buffer pointer to the arguments:
   ```fides
   var sret_args: Vec<i64> = Vec::new();
   sret_args.push(sret_ptr);
   ... push remaining args ...
   ```
3. Call the function with `IrType::Void` return:
   ```fides
   Lowerer_emit_inst(self, Inst::Call(callee, sret_args, IrType::Void, eff));
   ```
4. Return the `sret_ptr` value as the call result:
   ```fides
   return sret_ptr;
   ```

This applies to:
- `Expr::Call` (free function calls)
- `Expr::MethodCall` (receiver.method() calls)
- Associated function calls like `Vec::new()`

### What can go wrong

If the caller does NOT prepend the SRET pointer, the function will still be
called, but the hidden first argument is interpreted as a real argument.
This causes the function to write its result into a random location or
read the wrong argument. The C backend will emit `fname(arg0, ...)` instead
of `fname(&result, ...)`.

---

## 3. Enum Layout

### `Lowerer_make_enum_layout`

1. Compute the maximum payload size across all variants.
2. Each field is at least 8 bytes.
3. Struct/enum-typed fields use the full layout size.
4. Round `max_payload_size` up to an 8-byte multiple and count slots.
5. Layout is:
   - `__tag` at offset 0
   - `__data0` at offset 8
   - `__data1` at offset 16
   - ...

```fides
pub fn Lowerer_make_enum_layout(self: &Lowerer, def: EnumDef) -> StructLayout {
    var max_payload_size: i64 = 0;
    ... for each variant, sum field sizes ...
    var payload_slots: i64 = (max_payload_size + 7) / 8;
    var fields: Vec<StringIrTypeOffset> = Vec::new();
    fields.push(StringIrTypeOffset_new("__tag", IrType::I64, 0));
    var j: i64 = 0;
    while j < payload_slots {
        fields.push(StringIrTypeOffset_new("__data" + int_to_str(j), IrType::I64, (j + 1) * 8));
        j = j + 1;
    }
    ...
}
```

### Iterative fixpoint

Because enums can contain structs and structs can contain enums, layouts are
computed in a loop (up to 20 iterations) until no size changes.

### Example: `ParseResultExpr`

```fides
pub enum ParseResultExpr {
    Ok(Expr),
    Err(str),
}
```

- `Ok(Expr)` needs `sizeof(Expr)` bytes for the payload.
- `Err(str)` needs 8 bytes (the `str` pointer).
- `max_payload_size = max(sizeof(Expr), 8)`.
- If `Expr` is 80 bytes, `payload_slots = 10` and total size = `11 * 8 = 88` bytes.

If the layout only allocated 1 slot (the old bug), `sizeof(ParseResultExpr) = 16`,
so SRET copy would only copy 16 bytes, truncating the `Expr` data.

---

## 4. Struct Copy

### `Lowerer_emit_struct_copy`

```fides
pub fn Lowerer_emit_struct_copy(self: &mut Lowerer, src_ptr: i64, dst_ptr: i64, struct_name: str) {
    match IrModule_get_struct(&self.module, struct_name) {
        OptStructLayout::Some(layout) => {
            while fi < layout.fields.len() {
                var field_ty: IrType = layout.fields.get(fi).ty;
                var src_field: i64 = Lowerer_emit_inst(self, Inst::GetFieldPtr(src_ptr, fi, field_ty));
                var dst_field: i64 = Lowerer_emit_inst(self, Inst::GetFieldPtr(dst_ptr, fi, field_ty));
                var val: i64 = Lowerer_emit_inst(self, Inst::Load(src_field, field_ty));
                var _s: i64 = Lowerer_emit_inst(self, Inst::Store(dst_field, val, OptIrType::None));
                fi = fi + 1;
            }
        }
        ...
    }
}
```

This is field-by-field copy. It relies on `Inst::Store` with a struct-typed
field copying 8 bytes only (the pointer), which is fine because the source
data has the same layout.

### C backend `Inst::Store` for structs

For the full-data copy to work, `Inst::Store` in `codegen_c.fd` must emit a
`memcpy` when the stored type is `IrType::Struct(_)` or the `store_ty` is
`Some(IrType::Struct(_))`.

Current Rust `codegen_c.rs` does this. The Fides `codegen_c.fd` must too.

---

## 5. Enum Variant Extraction — The Current Blocker

### What `ParseResultExpr::Ok(e) => { ... }` means

When the lowerer matches an enum variant, it does:

1. Load tag (field 0), compare to variant index.
2. If matched, compute `field_ptr = match_val + (bi + 1) * 8`.
3. Load the field: `field_val = Load(field_ptr, field_ty)`.
4. Bind `field_val` to the pattern name.

### The bug

When `field_ty` is `IrType::Struct(_)`:
- `Load(field_ptr, field_ty)` in the C backend only reads 8 bytes.
- But the actual data at `field_ptr` is a full struct (e.g. `Expr` is 80 bytes).
- Result: the binding gets only the first 8 bytes (the struct's tag), not the full struct.

### The fix in Rust

```rust
fn load_enum_field(&mut self, field_ptr: ValueId, field_ty: &IrType) -> ValueId {
    if let IrType::Struct(_) = field_ty {
        let buf = self.emit_inst(Inst::Alloca {
            ty: field_ty.clone(),
            region: self.current_region(),
        });
        self.emit_inst(Inst::Store {
            ptr: buf,
            value: field_ptr,
            store_ty: Some(field_ty.clone()),
        });
        buf
    } else {
        self.emit_inst(Inst::Load { ptr: field_ptr, ty: field_ty.clone() })
    }
}
```

What it does:
1. If the field is a struct, allocate a buffer of the full struct size.
2. Use `Inst::Store` with `store_ty: Some(field_ty)` to copy the inline data from `field_ptr` into `buf`.
3. The C backend, seeing `store_ty: Some(Struct(_))`, emits `memcpy`.
4. Return `buf` (a pointer to the full data).
5. If the field is not a struct, just do a normal `Load`.

### Where to apply it in `compiler/lower.fd`

Two match binding sites need `load_enum_field` instead of `Load`:
1. `Lowerer_lower_match` around `GetFieldPtr(match_val, bi + 1, field_ty)` → `Load(field_ptr, field_ty)`.
2. The other match lowering path that binds enum payload fields.

### Why `Inst::Store` must support `memcpy`

If `Inst::Store` in `compiler/codegen_c.fd` does not emit `memcpy` for
`IrType::Struct(_)` (or `store_ty: Some(IrType::Struct(_))`), then the
`load_enum_field` fix will still only copy 8 bytes.

The C backend must check:
```fides
match store_ty {
    OptIrType::Some(IrType::Struct(_)) => {
        // emit memcpy(dst, src, sizeof(struct))
    }
    _ => {
        // normal scalar store
    }
}
```

---

## 6. Box Semantics

### Language-level `Box::new(x)`

```fides
var val: i64 = Lowerer_lower_expr(self, args.get(0));
var size: i64 = ir_type_size(val_ty);
var size_val: i64 = Lowerer_emit_inst(self, Inst::ConstInt(size, IrType::I64));
var ptr_ty: IrType = IrType::Ptr(Box::new(val_ty));
var ptr: i64 = Lowerer_emit_inst(self, Inst::Call("p_malloc", c_vec1_i64(size_val), ptr_ty, EffectSet_new()));
// then Store the value into ptr
```

### Compiler-internal `Box<T>`

The self-hosted compiler's AST and IR use `Box<T>` for recursive types. In
Fides, matching on `Box<T>` auto-unboxes, but the extracted value is still a
`Box`. Explicit unbox helpers are required:
- `box_type_unbox()`
- `box_expr_unbox()`
- `box_ir_type_unbox()`
- `box_ir_type_size()`, `box_ir_type_align()`, `box_ir_type_eq()`, `box_ir_type_to_str()`

### `box_expr_unbox` must stay in sync with `ast.fd`

`compiler/lower.fd` `box_expr_unbox` currently enumerates variants that do
not exist in `compiler/ast.fd` (e.g. `Lambda`, `Throw`, `TryUnwrap`, `Assign`,
`CompoundAssign`, `ConstExpr`, `ComptimeGen`, `QuantCast`, `Await`, `Spawn`).
These must be reconciled with the canonical `ast.fd` `Expr` enum.

---

## 7. Vec Handling

### Creation

```fides
Vec::new()
```

Lowered to `Vec_new(dummy_env)` returning `IrType::Opaque("Vec")` (an `i64`/pointer).

### Runtime externs

```fides
p_vec_new, p_vec_push, p_vec_push_str, p_vec_pop, p_vec_len, p_vec_get
```

### Indexing

`arr[i]` on an opaque `Vec` becomes `p_vec_get(arr, idx)` returning `IrType::I64`.
`arr[i] = x` becomes `p_vec_set(arr, idx, x)`.

### Struct elements

When pushing/inserting a struct into a `Vec`, the lowerer heap-allocates the
struct first, then pushes the heap pointer. This prevents a stack-allocated
SRET buffer from becoming a dangling pointer once the caller returns.

```fides
var size: i64 = IrModule_type_size(&self.module, vec_elem_ty);
var size_val: i64 = Lowerer_emit_inst(self, Inst::ConstInt(size, IrType::I64));
var heap_ptr: i64 = Lowerer_emit_inst(self, Inst::Call("p_malloc", c_vec1_i64(size_val), IrType::Ptr(Box::new(vec_elem_ty)), EffectSet_new()));
Lowerer_emit_inst(self, Inst::Store(heap_ptr, val, OptIrType::None));
// Replace the value in call_args with the heap pointer
```

### For loops

`for v in vec` uses `p_vec_len` and `p_vec_get`.

---

## 8. String Handling

### Lexer

- `src_len` is cached on `Lexer` to avoid `O(n)` `str_len()` on every `peek()`.
- `str_byte_at(source, pos)` inspects the current byte.
- `str_slice(source, start, end)` extracts a contiguous substring.
- `str_concat` is used for building f-strings and escape sequences.

### `str` in IR

`IrType::Str` is `IrType::Ptr(Box::new(IrType::U8))`. A `str` value is a
pointer to a heap-allocated, refcounted, length-prefixed string.

### String operations

Most string operations call into the runtime:
- `p_str_concat` / `p_str_append`
- `p_str_equal`
- `p_to_str` (polymorphic to-string)

---

## 9. The `Opt` Pattern

Because Fides has no `?` operator and no `Option` combinators, the compiler
uses explicit `Opt*` enums:

```fides
pub enum OptIrType { Some(IrType), None }
pub enum OptValueId { Some(i64), None }
pub enum OptU64     { Some(i64), None }
pub enum OptString  { Some(str),  None }
pub enum OptInst    { Some(Inst), None }
pub enum OptValueMeta { Some(ValueMeta), None }
pub enum OptStructLayout { Some(StructLayout), None }
pub enum OptIrFunction { Some(IrFunction), None }
pub enum OptExpr { Some(Expr), None }
pub enum OptType { Some(Type), None }
```

Usage pattern:
```fides
match Lowerer_lookup_local(self, name) {
    OptValueId::Some(v) => { ... }
    OptValueId::None => { ... }
}
```

Custom parse result enums (e.g. `ParseResultExpr::Ok(e) | Err(msg)`) are
also part of this pattern.

---

## 10. Function Calling Conventions Summary

| Call type | First arg | Env pointer | Return convention |
|-----------|-----------|-------------|-------------------|
| Free function | None | `__env` (if user func) | SRET if struct, else `return value` |
| Method call | `__env` (if user func) | `__env` (if user func) | SRET if struct, else `return value` |
| Extern | No `__env` | No `__env` | SRET if struct, else `return value` |
| Runtime (`p_*`) | No `__env` | No `__env` | Non-SRET (return by value) |
| Closure (`CallPtr`) | `__env` pointer | `__env` pointer | SRET if struct, else `return value` |

Notes:
- The `__env` dummy pointer is `0` (a `ConstInt(0, I64)` cast to `void*`) for user functions.
- For closures, the real `__env` pointer is passed first, then the hidden `__env` pointer is also first. The `__env` pointer is the same as the closure's environment in this design.
- Struct returns always use a hidden SRET pointer as the first *real* parameter, before the `__env` dummy for user functions.

### Argument order for a struct-returning user function

```fides
fn foo(x: i64) -> ParseResultExpr { ... }
```

C signature:
```c
void foo(void* sret, void* __env, int64_t x);
```

Call:
```c
void* result = alloca(sizeof(ParseResultExpr));
foo(result, (void*)0, 42);
```

---

## 11. Casting and Conversions

`cast_arg` in `codegen_c.fd` handles:
- Array params decay to `ElementType*`.
- `float → pointer` and `float → int` via union punning.
- `int → pointer` cast to `(void*)`.
- `pointer → int` cast to `(int64_t)`.
- Non-pointer → struct pointer cast to `(void*)` (for SRET calls).

Always verify the cast is safe. A missing cast causes C compiler warnings or incorrect code.
