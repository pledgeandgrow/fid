[All modules](index.md)

# compiler::mono -- Fides Documentation

## Contents

- [OptType](#OptType) (enum)
- [OptStr](#OptStr) (enum)
- [SubstEntry](#SubstEntry) (struct)
- [SubstEntry_new](#SubstEntry_new) (function)
- [subst_get](#subst_get) (function)
- [subst_set](#subst_set) (function)
- [subst_contains](#subst_contains) (function)
- [subst_new](#subst_new) (function)
- [AssocBindingEntry](#AssocBindingEntry) (struct)
- [AssocBindingEntry_new](#AssocBindingEntry_new) (function)
- [TraitAssocEntry](#TraitAssocEntry) (struct)
- [TraitAssocEntry_new](#TraitAssocEntry_new) (function)
- [OptVecAssocBinding](#OptVecAssocBinding) (enum)
- [trait_assoc_get](#trait_assoc_get) (function)
- [trait_assoc_get_by_type](#trait_assoc_get_by_type) (function)
- [assoc_bindings_get](#assoc_bindings_get) (function)
- [GenericFuncEntry](#GenericFuncEntry) (struct)
- [GenericFuncEntry_new](#GenericFuncEntry_new) (function)
- [OptGenericFunc](#OptGenericFunc) (enum)
- [GenericFuncMap](#GenericFuncMap) (struct)
- [GenericFuncMap_new](#GenericFuncMap_new) (function)
- [GenericFuncMap_push](#GenericFuncMap_push) (function)
- [generic_funcs_get](#generic_funcs_get) (function)
- [generic_funcs_contains](#generic_funcs_contains) (function)
- [GenericMethodEntry](#GenericMethodEntry) (struct)
- [GenericMethodEntry_new](#GenericMethodEntry_new) (function)
- [OptGenericMethod](#OptGenericMethod) (enum)
- [GenericMethodMap](#GenericMethodMap) (struct)
- [GenericMethodMap_new](#GenericMethodMap_new) (function)
- [GenericMethodMap_push](#GenericMethodMap_push) (function)
- [generic_methods_get](#generic_methods_get) (function)
- [generic_methods_contains](#generic_methods_contains) (function)
- [RenameEntry](#RenameEntry) (struct)
- [RenameEntry_new](#RenameEntry_new) (function)
- [rename_get](#rename_get) (function)
- [rename_set](#rename_set) (function)
- [rename_new](#rename_new) (function)
- [GenericCallEntry](#GenericCallEntry) (struct)
- [GenericCallEntry_new](#GenericCallEntry_new) (function)
- [GenericMethodCallEntry](#GenericMethodCallEntry) (struct)
- [GenericMethodCallEntry_new](#GenericMethodCallEntry_new) (function)
- [mangle](#mangle) (function)
- [mangle_method](#mangle_method) (function)
- [mangle_type](#mangle_type) (function)
- [subst_type](#subst_type) (function)
- [mn_box_expr_new](#mn_box_expr_new) (function)
- [mn_box_expr_deref](#mn_box_expr_deref) (function)
- [subst_expr](#subst_expr) (function)
- [subst_stmt](#subst_stmt) (function)
- [infer_subst](#infer_subst) (function)
- [infer_from_pair](#infer_from_pair) (function)
- [MonoCtx](#MonoCtx) (struct)
- [MonoCtx_new](#MonoCtx_new) (function)
- [MonoCtx_seen_insert](#MonoCtx_seen_insert) (function)
- [mono_turbofish_errors](#mono_turbofish_errors) (function)
- [mono_resolve_turbofish](#mono_resolve_turbofish) (function)
- [mono_method_inst](#mono_method_inst) (function)
- [mono_desugar_variadics](#mono_desugar_variadics) (function)
- [mn_is_event](#mn_is_event) (function)
- [mn_event_type_name](#mn_event_type_name) (function)
- [mn_event_signature](#mn_event_signature) (function)
- [mn_fnv32](#mn_fnv32) (function)
- [mn_hex8](#mn_hex8) (function)
- [monomorphize](#monomorphize) (function)
- [fixup_generic_calls](#fixup_generic_calls) (function)
- [fixup_generic_calls_stmt](#fixup_generic_calls_stmt) (function)
- [method_subst](#method_subst) (function)
- [subst_with_result](#subst_with_result) (function)
- [fixup_hinted_call](#fixup_hinted_call) (function)
- [fixup_generic_calls_expr](#fixup_generic_calls_expr) (function)
- [process_func](#process_func) (function)
- [instantiate](#instantiate) (function)
- [collect_generic_calls_stmt](#collect_generic_calls_stmt) (function)
- [collect_generic_calls_stmt_with_hint](#collect_generic_calls_stmt_with_hint) (function)
- [collect_generic_calls_expr_with_hint](#collect_generic_calls_expr_with_hint) (function)
- [infer_receiver_struct_name](#infer_receiver_struct_name) (function)
- [infer_expr_type_with_hint](#infer_expr_type_with_hint) (function)
- [infer_expr_type](#infer_expr_type) (function)

## OptType

```fides
pub enum OptType {
```

**Variants:**

- `Some`
- `None`

*Defined at line 22*

---

## OptStr

```fides
pub enum OptStr {
```

**Variants:**

- `Some`
- `None`

*Defined at line 27*

---

## SubstEntry

```fides
pub struct SubstEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `val` | `Type` |

*Defined at line 36*

---

## SubstEntry_new

```fides
pub fn SubstEntry_new(key: str, val: Type) -> SubstEntry {
```

**Parameters:**

- `key`: `str`
- `val`: `Type`

**Returns:** `SubstEntry`

*Defined at line 41*

---

## subst_get

```fides
pub fn subst_get(map: Vec<SubstEntry>, key: str) -> OptType {
```

**Parameters:**

- `map`: `Vec<SubstEntry>`
- `key`: `str`

**Returns:** `OptType`

*Defined at line 47*

---

## subst_set

```fides
pub fn subst_set(map: &mut Vec<SubstEntry>, key: str, val: Type) {
```

**Parameters:**

- `map`: `&mut Vec<SubstEntry>`
- `key`: `str`
- `val`: `Type`

*Defined at line 60*

---

## subst_contains

```fides
pub fn subst_contains(map: Vec<SubstEntry>, key: str) -> bool {
```

**Parameters:**

- `map`: `Vec<SubstEntry>`
- `key`: `str`

**Returns:** `bool`

*Defined at line 66*

---

## subst_new

```fides
pub fn subst_new() -> Vec<SubstEntry> {
```

**Returns:** `Vec<SubstEntry>`

*Defined at line 77*

---

## AssocBindingEntry

```fides
pub struct AssocBindingEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `assoc_name` | `str` |
| `ty` | `Type` |

*Defined at line 87*

---

## AssocBindingEntry_new

```fides
pub fn AssocBindingEntry_new(assoc_name: str, ty: Type) -> AssocBindingEntry {
```

**Parameters:**

- `assoc_name`: `str`
- `ty`: `Type`

**Returns:** `AssocBindingEntry`

*Defined at line 92*

---

## TraitAssocEntry

```fides
pub struct TraitAssocEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `trait_name` | `str` |
| `type_name` | `str` |
| `bindings` | `Vec<AssocBindingEntry>` |

*Defined at line 98*

---

## TraitAssocEntry_new

```fides
pub fn TraitAssocEntry_new(trait_name: str, type_name: str) -> TraitAssocEntry {
```

**Parameters:**

- `trait_name`: `str`
- `type_name`: `str`

**Returns:** `TraitAssocEntry`

*Defined at line 104*

---

## OptVecAssocBinding

```fides
pub enum OptVecAssocBinding {
```

**Variants:**

- `Some`
- `None`

*Defined at line 110*

---

## trait_assoc_get

```fides
pub fn trait_assoc_get(map: Vec<TraitAssocEntry>, trait_name: str, type_name: str) -> OptVecAssocBinding {
```

**Parameters:**

- `map`: `Vec<TraitAssocEntry>`
- `trait_name`: `str`
- `type_name`: `str`

**Returns:** `OptVecAssocBinding`

*Defined at line 115*

---

## trait_assoc_get_by_type

```fides
pub fn trait_assoc_get_by_type(map: Vec<TraitAssocEntry>, type_name: str) -> OptVecAssocBinding {
```

**Parameters:**

- `map`: `Vec<TraitAssocEntry>`
- `type_name`: `str`

**Returns:** `OptVecAssocBinding`

*Defined at line 127*

---

## assoc_bindings_get

```fides
pub fn assoc_bindings_get(bindings: Vec<AssocBindingEntry>, name: str) -> OptType {
```

**Parameters:**

- `bindings`: `Vec<AssocBindingEntry>`
- `name`: `str`

**Returns:** `OptType`

*Defined at line 139*

---

## GenericFuncEntry

```fides
pub struct GenericFuncEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `func` | `Func` |
| `type_params` | `Vec<str>` |

*Defined at line 154*

---

## GenericFuncEntry_new

```fides
pub fn GenericFuncEntry_new(name: str, func: Func, type_params: Vec<str>) -> GenericFuncEntry {
```

**Parameters:**

- `name`: `str`
- `func`: `Func`
- `type_params`: `Vec<str>`

**Returns:** `GenericFuncEntry`

*Defined at line 160*

---

## OptGenericFunc

```fides
pub enum OptGenericFunc {
```

**Variants:**

- `Some`
- `None`

*Defined at line 166*

---

## GenericFuncMap

```fides
pub struct GenericFuncMap {
```

**Fields:**

| Name | Type |
|------|------|
| `entries` | `Vec<GenericFuncEntry>` |
| `index` | `i64` |

*Defined at line 174*

---

## GenericFuncMap_new

```fides
pub fn GenericFuncMap_new() -> GenericFuncMap {
```

**Returns:** `GenericFuncMap`

*Defined at line 179*

---

## GenericFuncMap_push

```fides
pub fn GenericFuncMap_push(map: &mut GenericFuncMap, e: GenericFuncEntry) {
```

**Parameters:**

- `map`: `&mut GenericFuncMap`
- `e`: `GenericFuncEntry`

*Defined at line 185*

---

## generic_funcs_get

```fides
pub fn generic_funcs_get(map: GenericFuncMap, name: str) -> OptGenericFunc {
```

**Parameters:**

- `map`: `GenericFuncMap`
- `name`: `str`

**Returns:** `OptGenericFunc`

*Defined at line 192*

---

## generic_funcs_contains

```fides
pub fn generic_funcs_contains(map: GenericFuncMap, name: str) -> bool {
```

**Parameters:**

- `map`: `GenericFuncMap`
- `name`: `str`

**Returns:** `bool`

*Defined at line 200*

---

## GenericMethodEntry

```fides
pub struct GenericMethodEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `struct_name` | `str` |
| `method_name` | `str` |
| `func` | `Func` |
| `type_params` | `Vec<str>` |

*Defined at line 208*

---

## GenericMethodEntry_new

```fides
pub fn GenericMethodEntry_new(struct_name: str, method_name: str, func: Func, type_params: Vec<str>) -> GenericMethodEntry {
```

**Parameters:**

- `struct_name`: `str`
- `method_name`: `str`
- `func`: `Func`
- `type_params`: `Vec<str>`

**Returns:** `GenericMethodEntry`

*Defined at line 215*

---

## OptGenericMethod

```fides
pub enum OptGenericMethod {
```

**Variants:**

- `Some`
- `None`

*Defined at line 221*

---

## GenericMethodMap

```fides
pub struct GenericMethodMap {
```

**Fields:**

| Name | Type |
|------|------|
| `entries` | `Vec<GenericMethodEntry>` |
| `index` | `i64` |

*Defined at line 227*

---

## GenericMethodMap_new

```fides
pub fn GenericMethodMap_new() -> GenericMethodMap {
```

**Returns:** `GenericMethodMap`

*Defined at line 232*

---

## GenericMethodMap_push

```fides
pub fn GenericMethodMap_push(map: &mut GenericMethodMap, e: GenericMethodEntry) {
```

**Parameters:**

- `map`: `&mut GenericMethodMap`
- `e`: `GenericMethodEntry`

*Defined at line 238*

---

## generic_methods_get

```fides
pub fn generic_methods_get(map: GenericMethodMap, struct_name: str, method_name: str) -> OptGenericMethod {
```

**Parameters:**

- `map`: `GenericMethodMap`
- `struct_name`: `str`
- `method_name`: `str`

**Returns:** `OptGenericMethod`

*Defined at line 252*

---

## generic_methods_contains

```fides
pub fn generic_methods_contains(map: GenericMethodMap, struct_name: str, method_name: str) -> bool {
```

**Parameters:**

- `map`: `GenericMethodMap`
- `struct_name`: `str`
- `method_name`: `str`

**Returns:** `bool`

*Defined at line 260*

---

## RenameEntry

```fides
pub struct RenameEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `from` | `str` |
| `to` | `str` |

*Defined at line 268*

---

## RenameEntry_new

```fides
pub fn RenameEntry_new(from: str, to: str) -> RenameEntry {
```

**Parameters:**

- `from`: `str`
- `to`: `str`

**Returns:** `RenameEntry`

*Defined at line 273*

---

## rename_get

```fides
pub fn rename_get(map: Vec<RenameEntry>, key: str) -> OptStr {
```

**Parameters:**

- `map`: `Vec<RenameEntry>`
- `key`: `str`

**Returns:** `OptStr`

*Defined at line 279*

---

## rename_set

```fides
pub fn rename_set(map: &mut Vec<RenameEntry>, from: str, to: str) {
```

**Parameters:**

- `map`: `&mut Vec<RenameEntry>`
- `from`: `str`
- `to`: `str`

*Defined at line 292*

---

## rename_new

```fides
pub fn rename_new() -> Vec<RenameEntry> {
```

**Returns:** `Vec<RenameEntry>`

*Defined at line 298*

---

## GenericCallEntry

```fides
pub struct GenericCallEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `callee` | `str` |
| `arg_types` | `Vec<Type>` |

*Defined at line 306*

---

## GenericCallEntry_new

```fides
pub fn GenericCallEntry_new(callee: str, arg_types: Vec<Type>) -> GenericCallEntry {
```

**Parameters:**

- `callee`: `str`
- `arg_types`: `Vec<Type>`

**Returns:** `GenericCallEntry`

*Defined at line 311*

---

## GenericMethodCallEntry

```fides
pub struct GenericMethodCallEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `struct_name` | `str` |
| `method_name` | `str` |
| `arg_types` | `Vec<Type>` |
| `recv_ty` | `Type` |

*Defined at line 320*

---

## GenericMethodCallEntry_new

```fides
pub fn GenericMethodCallEntry_new(struct_name: str, method_name: str, arg_types: Vec<Type>, recv_ty: Type) -> GenericMethodCallEntry {
```

**Parameters:**

- `struct_name`: `str`
- `method_name`: `str`
- `arg_types`: `Vec<Type>`
- `recv_ty`: `Type`

**Returns:** `GenericMethodCallEntry`

*Defined at line 327*

---

## mangle

```fides
pub fn mangle(name: str, subst: Vec<SubstEntry>, type_params: Vec<str>) -> str {
```

**Parameters:**

- `name`: `str`
- `subst`: `Vec<SubstEntry>`
- `type_params`: `Vec<str>`

**Returns:** `str`

*Defined at line 335*

---

## mangle_method

Mangled name for a monomorphized generic method: StructName__methodName__T1_T2

```fides
pub fn mangle_method(struct_name: str, method_name: str, subst: Vec<SubstEntry>, type_params: Vec<str>) -> str {
```

**Parameters:**

- `struct_name`: `str`
- `method_name`: `str`
- `subst`: `Vec<SubstEntry>`
- `type_params`: `Vec<str>`

**Returns:** `str`

*Defined at line 356*

---

## mangle_type

```fides
pub fn mangle_type(ty: Type) -> str {
```

**Parameters:**

- `ty`: `Type`

**Returns:** `str`

*Defined at line 379*

---

## subst_type

```fides
pub fn subst_type(ty: Type, subst: Vec<SubstEntry>) -> Type {
```

**Parameters:**

- `ty`: `Type`
- `subst`: `Vec<SubstEntry>`

**Returns:** `Type`

*Defined at line 612*

---

## mn_box_expr_new

```fides
pub fn mn_box_expr_new(e: Expr) -> Box<Expr> {
```

**Parameters:**

- `e`: `Expr`

**Returns:** `Box<Expr>`

*Defined at line 691*

---

## mn_box_expr_deref

```fides
pub fn mn_box_expr_deref(b: Box<Expr>) -> Expr {
```

**Parameters:**

- `b`: `Box<Expr>`

**Returns:** `Expr`

*Defined at line 695*

---

## subst_expr

```fides
pub fn subst_expr(expr: Expr, subst: Vec<SubstEntry>, renames: Vec<RenameEntry>, method_renames: Vec<RenameEntry>,) -> Expr {
```

**Parameters:**

- `expr`: `Expr`
- `subst`: `Vec<SubstEntry>`
- `renames`: `Vec<RenameEntry>`
- `method_renames`: `Vec<RenameEntry>`

**Returns:** `Expr`

*Defined at line 1004*

---

## subst_stmt

```fides
pub fn subst_stmt(stmt: Stmt, subst: Vec<SubstEntry>, renames: Vec<RenameEntry>, method_renames: Vec<RenameEntry>,) -> Stmt {
```

**Parameters:**

- `stmt`: `Stmt`
- `subst`: `Vec<SubstEntry>`
- `renames`: `Vec<RenameEntry>`
- `method_renames`: `Vec<RenameEntry>`

**Returns:** `Stmt`

*Defined at line 1399*

---

## infer_subst

```fides
pub fn infer_subst(param_types: Vec<Type>, arg_types: Vec<Type>, type_params: Vec<str>,) -> Vec<SubstEntry> {
```

**Parameters:**

- `param_types`: `Vec<Type>`
- `arg_types`: `Vec<Type>`
- `type_params`: `Vec<str>`

**Returns:** `Vec<SubstEntry>`

*Defined at line 1720*

---

## infer_from_pair

```fides
pub fn infer_from_pair(param: Type, arg: Type, type_params: Vec<str>, subst: &mut Vec<SubstEntry>) {
```

**Parameters:**

- `param`: `Type`
- `arg`: `Type`
- `type_params`: `Vec<str>`
- `subst`: `&mut Vec<SubstEntry>`

*Defined at line 1736*

---

## MonoCtx

```fides
pub struct MonoCtx {
```

**Fields:**

| Name | Type |
|------|------|
| `generics` | `GenericFuncMap` |
| `generic_methods` | `GenericMethodMap` |
| `instantiated` | `Vec<Func>` |
| `seen` | `Vec<str>` |
| `seen_index` | `i64` |
| `trait_assoc_bindings` | `Vec<TraitAssocEntry>` |

*Defined at line 1845*

---

## MonoCtx_new

```fides
pub fn MonoCtx_new() -> MonoCtx {
```

**Returns:** `MonoCtx`

*Defined at line 1855*

---

## MonoCtx_seen_insert

```fides
pub fn MonoCtx_seen_insert(ctx: &mut MonoCtx, name: str) -> bool {
```

**Parameters:**

- `ctx`: `&mut MonoCtx`
- `name`: `str`

**Returns:** `bool`

*Defined at line 1861*

---

## mono_turbofish_errors

The errors of the last mono_resolve_turbofish.

```fides
pub fn mono_turbofish_errors() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 1885*

---

## mono_resolve_turbofish

Resolve the turbofish calls of `program` (table: the parser's
turbofish_types). Errors are read with mono_turbofish_errors.

```fides
pub fn mono_resolve_turbofish(program0: Program, table: Vec<Type>) -> Program {
```

**Parameters:**

- `program0`: `Program`
- `table`: `Vec<Type>`

**Returns:** `Program`

*Defined at line 1945*

---

## mono_method_inst

L8H: the instantiation `mangled` of method `gen_func` of the generic
impl of `struct_name` (impl parameters `impl_tps`, bound in `subst`):
with the body of the most specific narrower impl's method of that name
whose type takes the receiver's (`impl Wrap<i64>` for a Wrap<i64>,
`impl<U> Wrap<Vec<U>>` for a Wrap<Vec<str>>), else its own. The type
checker has reported two that tie (TypeChecker_check_mspec_call).

```fides
pub fn mono_method_inst(struct_name: str, gen_func: Func, impl_tps: Vec<str>, subst: Vec<SubstEntry>, mangled: str, ctx: &mut MonoCtx) -> Func {
```

**Parameters:**

- `struct_name`: `str`
- `gen_func`: `Func`
- `impl_tps`: `Vec<str>`
- `subst`: `Vec<SubstEntry>`
- `mangled`: `str`
- `ctx`: `&mut MonoCtx`

**Returns:** `Func`

*Defined at line 2258*

---

## mono_desugar_variadics

L8E: the program with its variadic functions' parameters made Vecs and
every call of one packed. The program itself when it has none.

```fides
pub fn mono_desugar_variadics(program: Program) -> Program {
```

**Parameters:**

- `program`: `Program`

**Returns:** `Program`

*Defined at line 2951*

---

## mn_is_event

```fides
pub fn mn_is_event(func: Func) -> bool {
```

**Parameters:**

- `func`: `Func`

**Returns:** `bool`

*Defined at line 3269*

---

## mn_event_type_name

The canonical name of an event parameter's type in its signature.

```fides
pub fn mn_event_type_name(t: Type) -> str {
```

**Parameters:**

- `t`: `Type`

**Returns:** `str`

*Defined at line 3281*

---

## mn_event_signature

`Name(t1,t2,...)`: the event's canonical signature.

```fides
pub fn mn_event_signature(func: Func) -> str {
```

**Parameters:**

- `func`: `Func`

**Returns:** `str`

*Defined at line 3320*

---

## mn_fnv32

FNV-1a, 32 bits, of `s`.

```fides
pub fn mn_fnv32(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 3334*

---

## mn_hex8

`v` (0 <= v < 2^32) as 8 lower-case hex digits.

```fides
pub fn mn_hex8(v: i64) -> str {
```

**Parameters:**

- `v`: `i64`

**Returns:** `str`

*Defined at line 3346*

---

## monomorphize

```fides
pub fn monomorphize(program: Program) -> Program {
```

**Parameters:**

- `program`: `Program`

**Returns:** `Program`

*Defined at line 3695*

---

## fixup_generic_calls

```fides
pub fn fixup_generic_calls(stmts: Vec<Stmt>, generics: GenericFuncMap, gm: GenericMethodMap, current_func: Func) -> Vec<Stmt> {
```

**Parameters:**

- `stmts`: `Vec<Stmt>`
- `generics`: `GenericFuncMap`
- `gm`: `GenericMethodMap`
- `current_func`: `Func`

**Returns:** `Vec<Stmt>`

*Defined at line 3934*

---

## fixup_generic_calls_stmt

```fides
pub fn fixup_generic_calls_stmt(stmt: Stmt, generics: GenericFuncMap, gm: GenericMethodMap, current_func: Func) -> Stmt {
```

**Parameters:**

- `stmt`: `Stmt`
- `generics`: `GenericFuncMap`
- `gm`: `GenericMethodMap`
- `current_func`: `Func`

**Returns:** `Stmt`

*Defined at line 3944*

---

## method_subst

The substitution for a call of a generic impl method, as mono.rs: from the
arguments (after `self`), then from the receiver's type arguments
(`impl<T> Option` on an Option<f64> gives T=f64), then i64 for the rest.

```fides
pub fn method_subst(gen_func: Func, type_params: Vec<str>, arg_types: Vec<Type>, recv_ty: Type) -> Vec<SubstEntry> {
```

**Parameters:**

- `gen_func`: `Func`
- `type_params`: `Vec<str>`
- `arg_types`: `Vec<Type>`
- `recv_ty`: `Type`

**Returns:** `Vec<SubstEntry>`

*Defined at line 4104*

---

## subst_with_result

The substitution for a call: from the arguments, then, for a type
parameter they leave unbound, from the expected type (the element after
the arguments in `arg_types`, when there is one) against the result type.

```fides
pub fn subst_with_result(gen_func: Func, arg_types: Vec<Type>, type_params: Vec<str>) -> Vec<SubstEntry> {
```

**Parameters:**

- `gen_func`: `Func`
- `arg_types`: `Vec<Type>`
- `type_params`: `Vec<str>`

**Returns:** `Vec<SubstEntry>`

*Defined at line 4160*

---

## fixup_hinted_call

A direct call to a generic function whose result has a declared type:
renamed with that type for what the arguments leave unbound. One
expression, or none.

```fides
pub fn fixup_hinted_call(expr: Expr, hint: OptType, generics: GenericFuncMap, gm: GenericMethodMap, current_func: Func) -> Vec<Expr> {
```

**Parameters:**

- `expr`: `Expr`
- `hint`: `OptType`
- `generics`: `GenericFuncMap`
- `gm`: `GenericMethodMap`
- `current_func`: `Func`

**Returns:** `Vec<Expr>`

*Defined at line 4196*

---

## fixup_generic_calls_expr

```fides
pub fn fixup_generic_calls_expr(expr: Expr, generics: GenericFuncMap, gm: GenericMethodMap, current_func: Func) -> Expr {
```

**Parameters:**

- `expr`: `Expr`
- `generics`: `GenericFuncMap`
- `gm`: `GenericMethodMap`
- `current_func`: `Func`

**Returns:** `Expr`

*Defined at line 4266*

---

## process_func

```fides
pub fn process_func(func: Func, ctx: &mut MonoCtx) -> Func {
```

**Parameters:**

- `func`: `Func`
- `ctx`: `&mut MonoCtx`

**Returns:** `Func`

*Defined at line 4603*

---

## instantiate

```fides
pub fn instantiate(gen_func: Func, subst: Vec<SubstEntry>, mangled_name: str, ctx: &mut MonoCtx) -> Func {
```

**Parameters:**

- `gen_func`: `Func`
- `subst`: `Vec<SubstEntry>`
- `mangled_name`: `str`
- `ctx`: `&mut MonoCtx`

**Returns:** `Func`

*Defined at line 4770*

---

## collect_generic_calls_stmt

```fides
pub fn collect_generic_calls_stmt(stmt: Stmt, generics: GenericFuncMap, generic_methods: GenericMethodMap, calls: &mut Vec<GenericCallEntry>, method_calls: &mut Vec<GenericMethodCallEntry>, current_func: Func,) {
```

**Parameters:**

- `stmt`: `Stmt`
- `generics`: `GenericFuncMap`
- `generic_methods`: `GenericMethodMap`
- `calls`: `&mut Vec<GenericCallEntry>`
- `method_calls`: `&mut Vec<GenericMethodCallEntry>`
- `current_func`: `Func`

*Defined at line 4872*

---

## collect_generic_calls_stmt_with_hint

```fides
pub fn collect_generic_calls_stmt_with_hint(stmt: Stmt, generics: GenericFuncMap, generic_methods: GenericMethodMap, calls: &mut Vec<GenericCallEntry>, method_calls: &mut Vec<GenericMethodCallEntry>, current_func: Func, expected_ty: OptType,) {
```

**Parameters:**

- `stmt`: `Stmt`
- `generics`: `GenericFuncMap`
- `generic_methods`: `GenericMethodMap`
- `calls`: `&mut Vec<GenericCallEntry>`
- `method_calls`: `&mut Vec<GenericMethodCallEntry>`
- `current_func`: `Func`
- `expected_ty`: `OptType`

*Defined at line 4876*

---

## collect_generic_calls_expr_with_hint

```fides
pub fn collect_generic_calls_expr_with_hint(expr: Expr, generics: GenericFuncMap, generic_methods: GenericMethodMap, calls: &mut Vec<GenericCallEntry>, method_calls: &mut Vec<GenericMethodCallEntry>, current_func: Func, expected_ty: OptType,) {
```

**Parameters:**

- `expr`: `Expr`
- `generics`: `GenericFuncMap`
- `generic_methods`: `GenericMethodMap`
- `calls`: `&mut Vec<GenericCallEntry>`
- `method_calls`: `&mut Vec<GenericMethodCallEntry>`
- `current_func`: `Func`
- `expected_ty`: `OptType`

*Defined at line 5111*

---

## infer_receiver_struct_name

```fides
pub fn infer_receiver_struct_name(receiver: Expr, func: Func) -> OptStr {
```

**Parameters:**

- `receiver`: `Expr`
- `func`: `Func`

**Returns:** `OptStr`

*Defined at line 5434*

---

## infer_expr_type_with_hint

```fides
pub fn infer_expr_type_with_hint(expr: Expr, func: Func, hint: OptType) -> Type {
```

**Parameters:**

- `expr`: `Expr`
- `func`: `Func`
- `hint`: `OptType`

**Returns:** `Type`

*Defined at line 5465*

---

## infer_expr_type

```fides
pub fn infer_expr_type(expr: Expr, func: Func) -> Type {
```

**Parameters:**

- `expr`: `Expr`
- `func`: `Func`

**Returns:** `Type`

*Defined at line 5613*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
