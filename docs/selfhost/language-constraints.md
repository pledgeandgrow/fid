# Fides Self-Hosted Compiler — Language Constraints

> What the Fides language must respect, and how the self-hosted compiler
> handles each feature.
>
> **Last updated:** Sep 5, 2026 (session 15 — **BOOTSTRAP FIXPOINT ACHIEVED**)

---

## 1. Types

### Primitive types

```fides
pub enum Type {
    I8, I16, I32, I64, I128,
    U8, U16, U32, U64, U128,
    F32, F64, Bool, Str, Void, Never, Infer,
    ...
}
```

- `I128` and `U128` are lowered to `IrType::I64` in the current Fides lowerer.
- `Infer` is used where the type has not been resolved yet.
- `Never` becomes `IrType::Void`.
- `Str` is a pointer to `u8` (`IrType::Ptr(Box::new(IrType::U8))`).

### Float constants (session 14 — B5 fix)

`ConstFloat` in IR stores the raw `u64`/`u32` bit pattern, not the decimal
value. The C backend emits union compound literals to reinterpret the bits:

```c
// f64
((union { uint64_t u; double f; }){ .u = 0x7FF8000000000000ULL }).f
// f32
((union { uint32_t u; float f; }){ .u = 0x7FC00000U }).f
```

This preserves NaN payloads, `+inf`/`-inf`, and `-0.0` bit patterns exactly.
The runtime functions `p_f64_to_bits` and `p_f32_to_bits` are registered as
externs in `Lowerer_register_runtime`.

### Named types

`Type::Named(str)` is resolved by the lowerer:
- If the name is a known alias, the alias is followed recursively.
- If the name is `Vec`, `HashMap`, `Tensor`, or `Allocator`, it becomes `IrType::Opaque(name)`.
- Otherwise it becomes `IrType::Struct(name)`.

**Critical rule:** Enums are not an `IrType`. They are lowered to `IrType::Struct(name)` and given an `StructLayout` with a tag field and payload fields.

### Composite types

| Fides syntax | AST representation | IR type | Notes |
|--------------|-------------------|---------|-------|
| `&T` / `&mut T` | `Type::Ref(Box<Type>, is_mut)` | `IrType::Ptr(Box<IrType>)` | References are always pointers in IR. |
| `*mut T` / `*const T` | `Type::RawPtr(Box<Type>, is_mut)` | `IrType::Ptr(Box<IrType>)` | `is_mut` only affects borrow-checking, not IR. |
| `[T; N]` | `Type::Array(Box<Type>, i64)` | `IrType::Array(Box<IrType>, i64)` | Array size is an `i64` literal. |
| `[T; NAME]` | `Type::ArrayConst(Box<Type>, str)` | `IrType::Array(Box<IrType>, 0)` | Size is looked up as a constant. |
| `Foo<T, U>` | `Type::Generic("Foo", Vec<Type>)` | depends | `Vec`, `HashMap`, `Tensor`, `Future` are special-cased. |
| `(T, U)` | `Type::Tuple(Vec<Type>)` | `IrType::Struct("Tuple_N")` | Lowered as a struct with generated name. |
| `fn(T, U) -> V` | `Type::Func(Vec<Type>, Box<Type>)` | `IrType::I64` | Function values are pointers. |

### `Box<T>`

`Box::new(x)` is lowered to a `p_malloc` of `ir_type_size(x)`, then the value is `Store`d into the returned pointer.

`Box<T>` in the self-hosted compiler's own source is used for recursive AST and IR fields (e.g. `Box<Expr>`, `Box<Type>`, `Box<IrType>`). Matching on `Box<T>` auto-unboxes in Fides, but the extracted field is still a `Box`, so explicit unbox helpers are needed:
- `box_type_unbox()`
- `box_expr_unbox()`
- `box_ir_type_unbox()`
- `box_ir_type_size()`, `box_ir_type_align()`, `box_ir_type_eq()`, `box_ir_type_to_str()`

---

## 2. Effects

### `Effect` enum

```fides
pub enum Effect {
    Alloc, IO, Net, Spawn, Diverge,
    Read, Write, Diff, Noise, Comm,
}
```

### `EffectSet`

Implemented as `Vec<Effect>` with `add`, `add_all`, `contains`, `is_pure`, `is_read_only`, `has_side_effects`.

### Function effect syntax

```fides
fn foo() !IO { ... }           // one effect
fn bar() !{IO, Net} { ... }    // multiple effects
fn pure() !{} { ... }          // no effects
```

### Runtime effects

The lowerer hard-codes `Effect` values for `p_*` builtins in `runtime_effects(name)`:
- `Effect::Write` for mutating `vec`/`map`/string/atomic/mutex/RC/KV-cache/DP operations.
- `Effect::Alloc` for `p_vec_new`, `p_malloc`, `p_alloc`, `p_free`, string ops, tensor ops.
- `Effect::IO` for `print`/`eprint`/`write`/`chan`/`file`/`socket`/`dl`/`all_reduce`/etc.

### Instruction effects

- `Inst::Alloca` → `Alloc`
- `Load` → `Read`; `Store` → `Write`
- `Call`/`CallPtr`/`DynCall` carry their `EffectSet`
- `Print` → `IO` + `Read`
- `Panic` / `Unreachable` → `Diverge`
- `RegionBegin/End` → `Alloc` (+ `Write` on `End`)
- `TxnBegin/End` → `Write`
- `GradBegin/End/Backward` → `Diff`

### Function-level aggregation

After lowering a function, every instruction's effects are unioned into `func_effects` and stored in `module.function_effects`. This is required for the optimizer to avoid deleting side-effecting calls.

---

## 3. Regions

```fides
pub enum Region {
    Stack, Heap, Static, Named(str),
}
```

### Syntax

```fides
region name { ... }
region gpu name { ... }
region kv_cache name window(W) { ... }
region name with budget(N) { ... }
```

### Lowering

- `Lowerer_current_region()` returns `Region::Named(top)` if the stack is non-empty, otherwise `Region::Stack`.
- `Inst::Alloca(ty, region)` uses the current region.
- Array literals and other local allocations default to stack unless inside a `region` block.
- `RegionBegin/End` instructions bracket the block and are emitted around the body.

---

## 4. Pattern Matching

### `Pattern` enum

```fides
pub enum Pattern {
    Wildcard,
    IntLit(i64),
    BoolLit(bool),
    Ident(str),
    EnumVariant(str, str, Vec<Binding>),
    StrLit(str),
    Range(Box<Pattern>, Box<Pattern>),
}
```

### Syntax

```fides
match expr {
    pattern => { ... }
    pattern if guard => { ... }
}
```

### Lowering `Lowerer_lower_match`

1. Lower the scrutinee to `match_val`.
2. Create a `merge_block`.
3. For each arm, create an `arm_block` and a `next_block`.
4. Emit pattern-specific tests:
   - `Wildcard` → unconditional `Jump(arm_block)`
   - `IntLit`/`BoolLit` → `BinOp(Eq)` + `Branch`
   - `Ident` → bind the whole value, `Jump`
   - `StrLit` → call `p_str_equal`
   - `Range` → lower/upper bound comparisons
   - `EnumVariant` → load `__tag` at field 0, compare to variant index, then `Branch`
5. In the `EnumVariant` arm block, each binding is bound with:
   ```fides
   var field_ptr: i64 = Lowerer_emit_inst(self, Inst::GetFieldPtr(match_val, bi + 1, field_ty));
   var field_val: i64 = Lowerer_emit_inst(self, Inst::Load(field_ptr, field_ty));
   Lowerer_bind_local(self, binding.name, field_val);
   ```
6. After each arm, bindings are truncated to saved lengths so arm-local variables do not leak to later arms.

### Current behavior (session 14 — pointer-based approach)

Under the pointer-based enum payload approach (COORDINATION.md [D1]),
`Inst::Load` reads the 8-byte pointer to the field's data. This is correct
for all field types — no `load_enum_field()` or `memcpy` is needed. See
`calling-conventions.md` §5 for details.

---

## 5. Closures

### AST

```fides
Expr::Closure(Vec<Param>, Vec<Stmt>, OptType)
```

### Lowering

1. Find free variables (captures).
2. Create a lifted function: `(env_ptr: *u8, ...own_params) -> ret`.
3. At creation site: `alloca` env struct, store captures, return struct ptr.
4. `CallPtr` passes `env_ptr` as the first argument.

### Environment struct

`Lowerer_make_fn_env`:
- Allocates `(1 + captures.len()) * 8` bytes.
- Field 0 stores the function-name pointer.
- Fields `1..N` store each captured value.
- The created `__env` struct is returned as a pointer.

### Calling convention

- Captured variables are loaded from the env struct inside the closure:
  `GetElemPtr(env_ptr, field_index, I64)` then `Load`.
- `CallPtr` is used for actual calls and the env pointer is passed as the first argument.

---

## 6. Impl Blocks and Traits

### AST

```fides
pub struct ImplDef {
    pub struct_name: str,
    pub type_params: Vec<str>,
    pub methods: Vec<Func>,
}

pub struct TraitImpl {
    pub trait_name: str,
    pub type_name: str,
    pub type_params: Vec<str>,
    pub methods: Vec<Func>,
    pub associated_types: Vec<AssocTypeBinding>,
}
```

### Syntax

```fides
impl Type {
    fn method(...) -> T { ... }
}

impl Trait for Type {
    type Assoc = T;
    fn method(...) -> T { ... }
}
```

### Lowering

1. **Register impl methods**: insert `(struct_name, method_name) -> Func` and `(type_name, method_name) -> Func` into `impl_methods`.
2. **Pre-register mangled `IrFunction`**: `StructName_method` / `TypeName_method` so calls can resolve return types.
3. **Register trait methods / impls** in `module.trait_methods` and `module.trait_impls`.
4. **Lower methods**: set `current_self_type` to the struct/type name so `Self` resolves correctly in params and return types.

### Method call resolution

- Method calls become `MethodCall(receiver, method, args)`.
- Lowerer resolves receiver type and constructs `mangled_name = type_name + "_" + method`.
- User functions get a dummy `__env` first argument; extern/runtime functions do not.

---

## 7. Extern / FFI

### AST

```fides
pub struct ExternDecl {
    pub abi: str,
    pub c_name: str,
    pub pledge_name: str,
    pub params: Vec<Param>,
    pub ret_ty: Type,
    pub include: OptStr,
    pub link_lib: OptStr,
}
```

### Syntax

```fides
extern "C" {
    include "header.h"
    link "ws2_32"
    fn c_name as pledge_name(params) -> ret;
}
```

### Lowering

- `pledge_name` is the name used inside Fides.
- `c_name` is the actual C symbol.
- Externs are added to `module.extern_functions` with their parameter and return types.
- Extern calls do **not** receive the `__env` dummy parameter; only user-defined functions do.

---

## 8. Defer

### Syntax

```fides
defer {
    ...
}
```

### Lowering

- `Stmt::Defer(body, _line)` pushes `StmtVec_from(body)` onto `defer_stack`.
- `Lowerer_emit_defers` emits the stored defer bodies in reverse order (LIFO) at:
  - Normal function end
  - `return` statement
  - `try` error propagation (when not inside a `try` block)

---

## 9. Try Operator and Try Blocks

### Syntax

```fides
let x = maybe_fails()?;       // Try operator
let y = try {                 // Try block
    ...
};
```

### Lowering `Expr::Try`

1. Lower the inner expression to `val`.
2. Load the tag (field 0) and `Branch(is_ok, ok_block, err_block)`.
3. **Ok path**: load the unwrapped value (field 1) and continue.
4. **Err path**:
   - If inside a `try` block: copy the result into the try's err slot and jump to `error_block`.
   - If not inside a `try` block: emit defers, then return the error via SRET.

### `TryContext`

```fides
pub struct TryContext {
    pub error_block: i64,
    pub error_result_slot: i64,
}
```

`try_stack: Vec<TryContext>` tracks active `try` scopes.

---

## 10. Return and the SRET Convention

### Non-struct return

```fides
return value;
```

Emits `Inst::Return(OptValueId::Some(value))`.

### Struct return

For a function returning a struct:
1. A hidden first parameter `sret: *Struct` is added to the IR function.
2. The last expression is lowered to a value.
3. `Lowerer_emit_sret_copy(src_ptr)` copies the value to the `sret` pointer.
4. `Inst::Return(OptValueId::None)` is emitted.

The C backend sees a `void` function and the first argument is the output buffer.

---

## 11. Enums

### Source syntax

```fides
pub enum ParseResultExpr {
    Ok(Expr),
    Err(str),
}
```

### IR layout (pointer-based approach — D1)

Enums are lowered to a `StructLayout`:
- Field 0: `__tag` at offset 0 (i64)
- Field 1..N: `__data0` at 8, `__data1` at 16, etc. (each is an 8-byte pointer)
- Size: `(1 + max_variant_arity) * 8`
- Alignment: 8

### Payload size

Each field in the largest variant gets exactly 8 bytes (a pointer/word),
regardless of the field's actual type. Struct/enum-typed fields are
heap-allocated and the pointer is stored in the payload slot. This is the
pointer-based approach (COORDINATION.md [D1]).

### Variant index

Variants are indexed in source order, starting at 0. `Ok` is 0, `Err` is 1 in the `ParseResultExpr` example.

---

## 12. Structs

### Source syntax

```fides
pub struct Point {
    pub x: i64,
    pub y: i64,
}
```

### IR layout

- Fields are laid out sequentially with 8-byte alignment.
- Each field is at least 8 bytes.
- Struct-typed fields use the full size from the module's `StructLayout`.
- Total size is rounded up to an 8-byte multiple.

---

## 13. Generics

### Source syntax

```fides
fn foo<T>(x: T) -> T { ... }
fn bar<T, U>(x: T, y: U) { ... }
```

### Lowering

- `mono.fd` clones generic functions for concrete type instantiations.
- `subst_*` helpers replace type parameters.
- Generic function bodies are not lowered until a concrete instantiation is found.
- `Lowerer_resolve_type` recursively resolves `Type::Generic(name, args)` by
  resolving each arg via `Lowerer_resolve_type` before returning (session 14).
  This fixes nested generic type alias expansion.

---

## 14. Tuples

### Source syntax

```fides
let t: (i64, str) = (1, "x");
```

### Lowering

- `Type::Tuple(Vec<Type>)` is lowered to `IrType::Struct("Tuple_N")` where `N` is the arity.
- Tuple fields are unnamed and accessed by index.
