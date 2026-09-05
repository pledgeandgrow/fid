# Fides Self-Hosted Compiler — Calling Conventions and Low-Level Machinery

> This is the most important file for agents working on codegen. The
> SRET convention, enum layout, struct copy, and value semantics must stay
> in sync across `lower.fd`, `codegen_c.fd`, and the runtime.
>
> **Last updated:** Sep 5, 2026 (session 14)
>
> **Important:** Sections 3 and 5 were updated in session 14 to reflect the
> **pointer-based enum payload approach** (COORDINATION.md [D1]). The old
> inline-struct + `memcpy` approach has been superseded.

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

> **Updated session 14:** The pointer-based approach (COORDINATION.md [D1])
> is now the canonical direction. Each payload field gets exactly 8 bytes
> (a pointer), regardless of the field's actual type.

### `Lowerer_make_enum_layout` (pointer-based approach)

1. Compute the maximum number of fields across all variants (`max_variant_arity`).
2. Every field gets exactly 8 bytes (a pointer/word).
3. Layout is:
   - `__tag` at offset 0 (i64)
   - `__data0` at offset 8 (i64 — pointer to field 0's data)
   - `__data1` at offset 16 (i64 — pointer to field 1's data)
   - ...

```fides
pub fn Lowerer_make_enum_layout(self: &Lowerer, def: EnumDef) -> StructLayout {
    var max_arity: i64 = 0;
    ... for each variant, count fields, track max ...
    var payload_slots: i64 = max_arity;
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

### Why pointer-based?

The C backend treats `IrType::Struct(_)` as `void*` (a pointer type). A plain
`Inst::Load` with a struct type reads only 8 bytes — the pointer. Storing
inline struct data would require `memcpy` via `Inst::Store` with explicit
`store_ty`, which is complex and error-prone. With the pointer-based approach:

- Every payload slot is 8 bytes.
- `Inst::Load` reads the 8-byte pointer.
- Field access works through the pointer.
- No `load_enum_field` workaround needed.

### Iterative fixpoint

Because enums can contain structs and structs can contain enums, layouts are
computed in a loop (up to 20 iterations) until no size changes. With the
pointer-based approach, this is simpler since all payload slots are 8 bytes.

### Example: `ParseResultExpr`

```fides
pub enum ParseResultExpr {
    Ok(Expr),
    Err(str),
}
```

- `Ok(Expr)` has 1 field → 1 payload slot.
- `Err(str)` has 1 field → 1 payload slot.
- `max_arity = 1`, `payload_slots = 1`.
- Total size = `(1 + 1) * 8 = 16` bytes.
- `__tag` at offset 0, `__data0` at offset 8 (pointer to `Expr` or `str`).

### Previous approach [REVERSED]

The old inline-struct approach computed `max_payload_size` by summing field
sizes and allocated `payload_slots = (max_payload_size + 7) / 8` slots. This
required `load_enum_field()` + `memcpy` for extraction and caused truncation
bugs. It has been reversed in favor of the pointer-based approach.

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

## 5. Enum Variant Extraction — Pointer-Based (session 14)

> **Updated session 14:** Under the pointer-based approach, this is no
> longer a blocker. The old `load_enum_field()` workaround is not needed.

### What `ParseResultExpr::Ok(e) => { ... }` means

When the lowerer matches an enum variant, it does:

1. Load tag (field 0), compare to variant index.
2. If matched, compute `field_ptr = match_val + (bi + 1) * 8`.
3. Load the field: `field_val = Load(field_ptr, field_ty)`.
4. Bind `field_val` to the pattern name.

### Why it works now

With the pointer-based enum layout, every payload slot stores an 8-byte
pointer. `Inst::Load` reads exactly 8 bytes — the pointer to the field's
data. This is correct for all field types:

- **Primitive fields** (i64, str, bool, f64): the slot stores the value
  directly (8 bytes is sufficient).
- **Struct/enum fields**: the slot stores a pointer to the heap-allocated
  struct/enum data. `Load` reads the pointer, and subsequent field access
  works through the pointer.

No `load_enum_field()` or `memcpy` is needed. The extraction is a simple
`Inst::Load` in all cases.

### Previous approach [REVERSED]

The old inline-struct approach stored struct data inline in enum payload
slots. `Inst::Load` with `IrType::Struct(_)` only read 8 bytes, truncating
the struct. The Rust lowerer had `load_enum_field()` (alloca + `Store` with
`store_ty: Some(field_ty)` → `memcpy`) to work around this. This is no
longer needed under the pointer-based approach.

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

---

## 12. PE Emitter Calling Conventions (session 5)

The PE emitter (`compiler/pe.fd`) generates x86-64 machine code directly,
bypassing C entirely. It uses the Windows x64 calling convention:

### Register usage

| Register | Role |
|----------|------|
| RCX | 1st argument |
| RDX | 2nd argument |
| R8 | 3rd argument |
| R9 | 4th argument |
| RAX | Return value |
| RSP | Stack pointer (16-byte aligned before calls) |
| RBP | Frame pointer (base for local variables) |

### Stack frame layout

```
[higher addresses]
  saved RBP          ← RBP points here
  local var (-8)     ← [rbp - 8]
  local var (-16)    ← [rbp - 16]
  local var (-24)    ← [rbp - 24]
  ...
  shadow space (32)  ← for WinAPI calls
[lower addresses]    ← RSP points here
```

### Function prologue/epilogue

```asm
; prologue
push rbp
mov rbp, rsp
sub rsp, <frame_size>

; epilogue
mov rsp, rbp
pop rbp
ret
```

### Windows API calls

The PE emitter imports `GetStdHandle`, `WriteFile`, and `ExitProcess` via
the Import Address Table (IAT). Calls use `call [rip + offset]` (rip-relative
addressing). Shadow space (32 bytes) is allocated on the stack before each
call and freed after.

### Runtime stubs

The PE emitter generates these runtime stubs inline:
- `p_print_str` — strlen loop + GetStdHandle(-11) + WriteFile to stdout
- `p_print_int` — itoa + WriteFile
- `p_print_float` — dtoa + WriteFile
- `p_print_bool` — "true"/"false" + WriteFile
- `p_panic` — ExitProcess(1)

### String literals

String literals are stored in the `.rdata` section. They are accessed via
rip-relative `lea` instructions. The `PeCodeGen` struct maintains a list of
string constants and their offsets.

### Binary output

PE files are binary data containing null bytes. Since `p_file_write` truncates
at null bytes, the PE emitter hex-encodes the binary data and writes it as a
hex string. PowerShell then decodes the hex file to binary.

---

## 13. Float Constant Bit-Pattern Preservation (session 14 — B5 fix)

### The problem

IEEE-754 floating-point values have special bit patterns that cannot be
preserved by decimal literal emission:
- **NaN** — multiple payload variants (quiet vs signaling)
- **Infinity** — `+inf` / `-inf`
- **Negative zero** — `-0.0` has a distinct bit pattern from `0.0`

The old `ConstFloat` handler in `compiler/codegen_c.fd` emitted decimal
literals (`3.14`, `nan`, `inf`), which lost exact bit patterns.

### The fix

`ConstFloat` now stores the raw `u64`/`u32` bit pattern and emits a C
union compound literal to reinterpret the bits as a float:

```c
// f64
((union { uint64_t u; double f; }){ .u = 0x7FF8000000000000ULL }).f
// f32
((union { uint32_t u; float f; }){ .u = 0x7FC00000U }).f
```

### Runtime functions

Two C runtime functions were added to both `src/codegen_c.rs` and
`compiler/codegen_c.fd`:

```c
uint64_t p_f64_to_bits(double f);
uint32_t p_f32_to_bits(float f);
```

These are registered as externs in both `src/lower.rs` and
`compiler/lower.fd` (`Lowerer_register_runtime`). The Fides `lower.fd`
implements `f64_to_bits` / `f32_to_bits` by calling these externs.

### What this preserves

- NaN payloads (quiet NaN `0x7FF8...`, signaling NaN `0x7FF4...`)
- `+inf` (`0x7FF0000000000000`), `-inf` (`0xFFF0000000000000`)
- `-0.0` (`0x8000000000000000`)
- All normal/subnormal values

This is critical for bootstrap fixpoint: if the Rust compiler and the
self-hosted compiler emit different bit patterns for the same float
constant, the fixpoint will diverge.
