[All modules](index.md)

# compiler::ast -- Fides Documentation

## Contents

- [StrPair](#StrPair) (struct)
- [FieldInit](#FieldInit) (struct)
- [Binding](#Binding) (struct)
- [AssocTypeDecl](#AssocTypeDecl) (struct)
- [AssocTypeBinding](#AssocTypeBinding) (struct)
- [LayerInst](#LayerInst) (struct)
- [OptBoxExpr](#OptBoxExpr) (enum)
- [OptStmts](#OptStmts) (enum)
- [OptType](#OptType) (enum)
- [OptI64](#OptI64) (enum)
- [OptParams](#OptParams) (enum)
- [OptStr](#OptStr) (enum)
- [OptFunc](#OptFunc) (enum)
- [ShapeDim](#ShapeDim) (enum)
- [Type](#Type) (enum)
- [BinOp](#BinOp) (enum)
- [cold_fn_mark](#cold_fn_mark) (function)
- [is_cold_fn_hint](#is_cold_fn_hint) (function)
- [infer_ret_note](#infer_ret_note) (function)
- [infer_ret_of](#infer_ret_of) (function)
- [rt6_count](#rt6_count) (function)
- [rt6_reset](#rt6_reset) (function)
- [rt6_add](#rt6_add) (function)
- [rt6_add_map](#rt6_add_map) (function)
- [rt6_is_map](#rt6_is_map) (function)
- [rt6_state](#rt6_state) (function)
- [rt6_set_state](#rt6_set_state) (function)
- [rt6_set_typed](#rt6_set_typed) (function)
- [rt6_find](#rt6_find) (function)
- [rt6_close](#rt6_close) (function)
- [rt6_elem_at](#rt6_elem_at) (function)
- [rt6_map_at](#rt6_map_at) (function)
- [edition_known](#edition_known) (function)
- [set_default_edition](#set_default_edition) (function)
- [set_project_edition](#set_project_edition) (function)
- [default_edition](#default_edition) (function)
- [edition_set](#edition_set) (function)
- [edition_of](#edition_of) (function)
- [program_edition](#program_edition) (function)
- [binop_operator_method](#binop_operator_method) (function)
- [assign_op_binop](#assign_op_binop) (function)
- [assign_op_is_arith](#assign_op_is_arith) (function)
- [expr_is_place_path](#expr_is_place_path) (function)
- [dim_base_count](#dim_base_count) (function)
- [dim_base_name](#dim_base_name) (function)
- [dim_base_index](#dim_base_index) (function)
- [dim_dimensionless](#dim_dimensionless) (function)
- [dim_exp](#dim_exp) (function)
- [dim_combine](#dim_combine) (function)
- [dim_to_str](#dim_to_str) (function)
- [dim_annotation_text](#dim_annotation_text) (function)
- [DimRead](#DimRead) (struct)
- [dimension_annotation_error](#dimension_annotation_error) (function)
- [struct_dimension](#struct_dimension) (function)
- [binop_eq](#binop_eq) (function)
- [unaryop_eq](#unaryop_eq) (function)
- [UnaryOp](#UnaryOp) (enum)
- [AssignOp](#AssignOp) (enum)
- [FStringPart](#FStringPart) (enum)
- [MatchArm](#MatchArm) (struct)
- [ReflectOp](#ReflectOp) (enum)
- [AsmDir](#AsmDir) (enum)
- [AsmOperand](#AsmOperand) (struct)
- [Expr](#Expr) (enum)
- [Pattern](#Pattern) (enum)
- [SelectCase](#SelectCase) (struct)
- [EffectHandler](#EffectHandler) (struct)
- [Stmt](#Stmt) (enum)
- [Type_const_of](#Type_const_of) (function)
- [Type_const_value](#Type_const_value) (function)
- [Param](#Param) (struct)
- [Annotation](#Annotation) (struct)
- [Func](#Func) (struct)
- [StructDef](#StructDef) (struct)
- [EnumVariant](#EnumVariant) (struct)
- [EnumDef](#EnumDef) (struct)
- [ConstDef](#ConstDef) (struct)
- [GlobalVar](#GlobalVar) (struct)
- [TypeAlias](#TypeAlias) (struct)
- [ImplDef](#ImplDef) (struct)
- [TraitMethod](#TraitMethod) (struct)
- [TraitDef](#TraitDef) (struct)
- [TraitImpl](#TraitImpl) (struct)
- [ImportDef](#ImportDef) (struct)
- [Attribute](#Attribute) (struct)
- [ExternDecl](#ExternDecl) (struct)
- [LayerDef](#LayerDef) (struct)
- [ModelDef](#ModelDef) (struct)
- [ComponentDef](#ComponentDef) (struct)
- [SystemDef](#SystemDef) (struct)
- [StateTransition](#StateTransition) (struct)
- [StateDef](#StateDef) (struct)
- [StateMachineDef](#StateMachineDef) (struct)
- [EventHandlerDef](#EventHandlerDef) (struct)
- [Program](#Program) (struct)
- [program_new](#program_new) (function)
- [stmt_line](#stmt_line) (function)
- [shape_dim_to_str](#shape_dim_to_str) (function)
- [box_type_to_str](#box_type_to_str) (function)
- [type_to_str](#type_to_str) (function)
- [spec_base_name](#spec_base_name) (function)
- [spec_match](#spec_match) (function)
- [spec_takes](#spec_takes) (function)
- [spec_freshen](#spec_freshen) (function)
- [spec_param_types](#spec_param_types) (function)
- [spec_at_least](#spec_at_least) (function)
- [spec_choose](#spec_choose) (function)
- [FieldInit_new](#FieldInit_new) (function)
- [StrPair_new](#StrPair_new) (function)
- [Binding_new](#Binding_new) (function)
- [AssocTypeDecl_new](#AssocTypeDecl_new) (function)
- [AssocTypeBinding_new](#AssocTypeBinding_new) (function)
- [LayerInst_new](#LayerInst_new) (function)
- [Param_new](#Param_new) (function)
- [ast_spread_defaults](#ast_spread_defaults) (function)
- [ast_is_spread_defaults](#ast_is_spread_defaults) (function)
- [Annotation_new](#Annotation_new) (function)
- [MatchArm_new](#MatchArm_new) (function)
- [AsmOperand_new](#AsmOperand_new) (function)
- [SelectCase_new](#SelectCase_new) (function)
- [EffectHandler_new](#EffectHandler_new) (function)
- [EnumVariant_new](#EnumVariant_new) (function)
- [Func_new](#Func_new) (function)
- [Func_new_full](#Func_new_full) (function)
- [StructDef_new](#StructDef_new) (function)
- [StructDef_new_full](#StructDef_new_full) (function)
- [EnumDef_new](#EnumDef_new) (function)
- [ConstDef_new](#ConstDef_new) (function)
- [GlobalVar_new](#GlobalVar_new) (function)
- [TypeAlias_new](#TypeAlias_new) (function)
- [ImplDef_new](#ImplDef_new) (function)
- [ImplDef_new_full](#ImplDef_new_full) (function)
- [TraitMethod_new](#TraitMethod_new) (function)
- [TraitDef_new](#TraitDef_new) (function)
- [TraitDef_empty](#TraitDef_empty) (function)
- [box_type_unbox](#box_type_unbox) (function)
- [box_pattern_unbox](#box_pattern_unbox) (function)
- [box_expr_new](#box_expr_new) (function)
- [box_expr_unbox](#box_expr_unbox) (function)
- [TraitImpl_new](#TraitImpl_new) (function)
- [ImportDef_new](#ImportDef_new) (function)
- [ImportDef_new_full](#ImportDef_new_full) (function)
- [Attribute_new](#Attribute_new) (function)
- [ExternDecl_new](#ExternDecl_new) (function)
- [LayerDef_new](#LayerDef_new) (function)
- [ModelDef_new](#ModelDef_new) (function)
- [ComponentDef_new](#ComponentDef_new) (function)
- [SystemDef_new](#SystemDef_new) (function)
- [StateMachineDef_new](#StateMachineDef_new) (function)
- [StateDef_new](#StateDef_new) (function)
- [StateTransition_new](#StateTransition_new) (function)
- [EventHandlerDef_new](#EventHandlerDef_new) (function)
- [ann_lifetimes](#ann_lifetimes) (function)
- [func_has_lifetimes](#func_has_lifetimes) (function)
- [func_lifetime_ties](#func_lifetime_ties) (function)
- [trait_method_lifetime_ties](#trait_method_lifetime_ties) (function)
- [lifetime_ties](#lifetime_ties) (function)
- [str_vec_contains](#str_vec_contains) (function)
- [vec_contains_str](#vec_contains_str) (function)
- [escape_str](#escape_str) (function)
- [vec_contains_i64](#vec_contains_i64) (function)
- [vec_find_i64](#vec_find_i64) (function)
- [box_expr_deref](#box_expr_deref) (function)
- [ct_public_param_add](#ct_public_param_add) (function)
- [ct_public_param](#ct_public_param) (function)

## StrPair

```fides
pub struct StrPair {
```

**Fields:**

| Name | Type |
|------|------|
| `a` | `str` |
| `b` | `str` |

*Defined at line 24*

---

## FieldInit

```fides
pub struct FieldInit {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `value` | `Expr` |

*Defined at line 29*

---

## Binding

```fides
pub struct Binding {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `ty` | `str` |

*Defined at line 34*

---

## AssocTypeDecl

```fides
pub struct AssocTypeDecl {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `type_params` | `Vec<str>` |

*Defined at line 39*

---

## AssocTypeBinding

```fides
pub struct AssocTypeBinding {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `ty` | `Type` |

*Defined at line 44*

---

## LayerInst

```fides
pub struct LayerInst {
```

**Fields:**

| Name | Type |
|------|------|
| `field` | `str` |
| `layer_type` | `str` |
| `args` | `Vec<str>` |

*Defined at line 49*

---

## OptBoxExpr

```fides
pub enum OptBoxExpr {
```

**Variants:**

- `Some`
- `None`

*Defined at line 59*

---

## OptStmts

```fides
pub enum OptStmts {
```

**Variants:**

- `Some`
- `None`

*Defined at line 64*

---

## OptType

```fides
pub enum OptType {
```

**Variants:**

- `Some`
- `None`

*Defined at line 69*

---

## OptI64

```fides
pub enum OptI64 {
```

**Variants:**

- `Some`
- `None`

*Defined at line 74*

---

## OptParams

```fides
pub enum OptParams {
```

**Variants:**

- `Some`
- `None`

*Defined at line 79*

---

## OptStr

```fides
pub enum OptStr {
```

**Variants:**

- `Some`
- `None`

*Defined at line 84*

---

## OptFunc

```fides
pub enum OptFunc {
```

**Variants:**

- `Some`
- `None`

*Defined at line 89*

---

## ShapeDim

```fides
pub enum ShapeDim {
```

**Variants:**

- `Known`
- `Symbolic`

*Defined at line 96*

---

## Type

```fides
pub enum Type {
```

**Variants:**

- `I8`
- `I16`
- `I32`
- `I64`
- `I128`
- `U8`
- `U16`
- `U32`
- `U64`
- `U128`
- `F32`
- `F64`
- `Bool`
- `Str`
- `Void`
- `Never`
- `Infer`
- `Named`
- `Array`
- `ArrayConst`
- `Ref`
- `RawPtr`
- `Generic`
- `Tuple`
- `Func`
- `Assoc`
- `Dyn`
- `ShapedTensor`

*Defined at line 101*

---

## BinOp

```fides
pub enum BinOp {
```

**Variants:**

- `Add`
- `Sub`
- `Mul`
- `Div`
- `Mod`
- `Eq`
- `Neq`
- `Lt`
- `Gt`
- `LtEq`
- `GtEq`
- `And`
- `Or`
- `BitAnd`
- `BitOr`
- `BitXor`
- `Shl`
- `Shr`

*Defined at line 134*

---

## cold_fn_mark

```fides
pub fn cold_fn_mark(name: str) {
```

**Parameters:**

- `name`: `str`

*Defined at line 163*

---

## is_cold_fn_hint

```fides
pub fn is_cold_fn_hint(name: str) -> bool {
```

**Parameters:**

- `name`: `str`

**Returns:** `bool`

*Defined at line 170*

---

## infer_ret_note

```fides
pub fn infer_ret_note(name: str, t: Type) {
```

**Parameters:**

- `name`: `str`
- `t`: `Type`

*Defined at line 189*

---

## infer_ret_of

The return type the type checker inferred for `name`, if any.

```fides
pub fn infer_ret_of(name: str) -> OptType {
```

**Parameters:**

- `name`: `str`

**Returns:** `OptType`

*Defined at line 203*

---

## rt6_count

How many records there are (0: nothing to look up).

```fides
pub fn rt6_count() -> i64 {
```

**Returns:** `i64`

*Defined at line 238*

---

## rt6_reset

A new check starts (TypeChecker::new): an earlier check's records are
not its own.

```fides
pub fn rt6_reset() {
```

*Defined at line 244*

---

## rt6_add

```fides
pub fn rt6_add(key: str, name: str, line: i64, depth: i64, state: i64) {
```

**Parameters:**

- `key`: `str`
- `name`: `str`
- `line`: `i64`
- `depth`: `i64`
- `state`: `i64`

*Defined at line 255*

---

## rt6_add_map

rt6_add for a `let` whose value is a map (RT5).

```fides
pub fn rt6_add_map(key: str, name: str, line: i64, depth: i64, state: i64) {
```

**Parameters:**

- `key`: `str`
- `name`: `str`
- `line`: `i64`
- `depth`: `i64`
- `state`: `i64`

*Defined at line 267*

---

## rt6_is_map

Whether record `k` is a map's (rt6_add_map).

```fides
pub fn rt6_is_map(k: i64) -> bool {
```

**Parameters:**

- `k`: `i64`

**Returns:** `bool`

*Defined at line 273*

---

## rt6_state

```fides
pub fn rt6_state(k: i64) -> i64 {
```

**Parameters:**

- `k`: `i64`

**Returns:** `i64`

*Defined at line 277*

---

## rt6_set_state

```fides
pub fn rt6_set_state(k: i64, state: i64) {
```

**Parameters:**

- `k`: `i64`
- `state`: `i64`

*Defined at line 281*

---

## rt6_set_typed

Record `k` is typed: a Vec of `t`.

```fides
pub fn rt6_set_typed(k: i64, t: Type) {
```

**Parameters:**

- `k`: `i64`
- `t`: `Type`

*Defined at line 286*

---

## rt6_find

The record of the bare Vec `name` names in function `key` (the latest
one whose scope has not ended), or -1.

```fides
pub fn rt6_find(key: str, name: str) -> i64 {
```

**Parameters:**

- `key`: `str`
- `name`: `str`

**Returns:** `i64`

*Defined at line 293*

---

## rt6_close

A scope of function `key` ended, leaving `depth` scopes: the variables
it declared are out of scope. The function's records are the last ones.

```fides
pub fn rt6_close(key: str, depth: i64) {
```

**Parameters:**

- `key`: `str`
- `depth`: `i64`

*Defined at line 306*

---

## rt6_elem_at

The element type the first push gave the bare Vec of `var name =
Vec::new()` at `line` in function `fname` (as the lowering names it; a
generic function's copies are `fname__..`), if every check of that `let`
typed it, and the same way.

```fides
pub fn rt6_elem_at(fname: str, name: str, line: i64) -> OptType {
```

**Parameters:**

- `fname`: `str`
- `name`: `str`
- `line`: `i64`

**Returns:** `OptType`

*Defined at line 323*

---

## rt6_map_at

RT5: the map type (`HashMap<K, V>`) the first insert gave the map of an
untyped `var name = HashMap::new()` at `line` in function `fname` (as
the lowering names it), if every check of that `let` typed it, and the
same way.

```fides
pub fn rt6_map_at(fname: str, name: str, line: i64) -> OptType {
```

**Parameters:**

- `fname`: `str`
- `name`: `str`
- `line`: `i64`

**Returns:** `OptType`

*Defined at line 353*

---

## edition_known

```fides
pub fn edition_known(e: i64) -> bool {
```

**Parameters:**

- `e`: `i64`

**Returns:** `bool`

*Defined at line 388*

---

## set_default_edition

`--edition=N` (main.fd): wins over ark.toml's edition.

```fides
pub fn set_default_edition(e: i64) {
```

**Parameters:**

- `e`: `i64`

*Defined at line 395*

---

## set_project_edition

ark.toml's `edition` (compiler.fd), unless `--edition=N` was given.

```fides
pub fn set_project_edition(e: i64) {
```

**Parameters:**

- `e`: `i64`

*Defined at line 401*

---

## default_edition

```fides
pub fn default_edition() -> i64 {
```

**Returns:** `i64`

*Defined at line 407*

---

## edition_set

```fides
pub fn edition_set(module: str, e: i64) {
```

**Parameters:**

- `module`: `str`
- `e`: `i64`

*Defined at line 411*

---

## edition_of

The edition of `module`'s file (the default when it states none).

```fides
pub fn edition_of(module: str) -> i64 {
```

**Parameters:**

- `module`: `str`

**Returns:** `i64`

*Defined at line 419*

---

## program_edition

The edition a program states (`#[edition(N)]`), 0 when none, -1 when the
attribute has no number.

```fides
pub fn program_edition(p: Program) -> i64 {
```

**Parameters:**

- `p`: `Program`

**Returns:** `i64`

*Defined at line 430*

---

## binop_operator_method

CL10: the method `a op b` calls when `a` is a struct ("" for an
operator that is not overloadable): Rust's names, `%` is `rem`.

```fides
pub fn binop_operator_method(op: BinOp) -> str {
```

**Parameters:**

- `op`: `BinOp`

**Returns:** `str`

*Defined at line 461*

---

## assign_op_binop

CL10: the operator of an arithmetic compound assignment (`x += e` is
`x = x + e`), BinOp::BitAnd for any other kind (see assign_op_is_arith).

```fides
pub fn assign_op_binop(op: AssignOp) -> BinOp {
```

**Parameters:**

- `op`: `AssignOp`

**Returns:** `BinOp`

*Defined at line 486*

---

## assign_op_is_arith

CL10: whether `x op= e` is `+=`, `-=`, `*=`, `/=` or `%=` (the operators
a struct serves with a method).

```fides
pub fn assign_op_is_arith(op: AssignOp) -> bool {
```

**Parameters:**

- `op`: `AssignOp`

**Returns:** `bool`

*Defined at line 511*

---

## expr_is_place_path

Whether `e` is a variable or a field path of one (`p`, `self.pos`,
`a.b.c`): reading it twice reads the same place and runs nothing (CL10:
`obj.f += e` is rewritten to `obj.f = obj.f + e` only then).

```fides
pub fn expr_is_place_path(e: Expr) -> bool {
```

**Parameters:**

- `e`: `Expr`

**Returns:** `bool`

*Defined at line 518*

---

## dim_base_count

```fides
pub fn dim_base_count() -> i64 {
```

**Returns:** `i64`

*Defined at line 541*

---

## dim_base_name

The SI base `i` (packing order): m, kg, s, A, K, mol, cd.

```fides
pub fn dim_base_name(i: i64) -> str {
```

**Parameters:**

- `i`: `i64`

**Returns:** `str`

*Defined at line 546*

---

## dim_base_index

```fides
pub fn dim_base_index(name: str) -> i64 {
```

**Parameters:**

- `name`: `str`

**Returns:** `i64`

*Defined at line 571*

---

## dim_dimensionless

The packed dimension with every exponent 0.

```fides
pub fn dim_dimensionless() -> i64 {
```

**Returns:** `i64`

*Defined at line 593*

---

## dim_exp

The exponent of base `i` in a packed dimension.

```fides
pub fn dim_exp(packed: i64, i: i64) -> i64 {
```

**Parameters:**

- `packed`: `i64`
- `i`: `i64`

**Returns:** `i64`

*Defined at line 604*

---

## dim_combine

`a * b` (sign 1) or `a / b` (sign -1): the dimension of the result, or
-1 when an exponent leaves -8..7.

```fides
pub fn dim_combine(a: i64, b: i64, sign: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`
- `sign`: `i64`

**Returns:** `i64`

*Defined at line 614*

---

## dim_to_str

A dimension as text: `m^2 kg s^-2`, `1` when dimensionless.

```fides
pub fn dim_to_str(packed: i64) -> str {
```

**Parameters:**

- `packed`: `i64`

**Returns:** `str`

*Defined at line 629*

---

## dim_annotation_text

The annotation that declares a dimension: `@dimension(m = 2, s = -1)`.

```fides
pub fn dim_annotation_text(packed: i64) -> str {
```

**Parameters:**

- `packed`: `i64`

**Returns:** `str`

*Defined at line 652*

---

## DimRead

```fides
pub struct DimRead {
```

**Fields:**

| Name | Type |
|------|------|
| `packed` | `i64` |
| `error` | `str` |

*Defined at line 709*

---

## dimension_annotation_error

What is wrong with a struct's `@dimension(...)` ("" when it has none or
it is well formed).

```fides
pub fn dimension_annotation_error(anns: Vec<Annotation>) -> str {
```

**Parameters:**

- `anns`: `Vec<Annotation>`

**Returns:** `str`

*Defined at line 759*

---

## struct_dimension

The packed dimension a struct's `@dimension(...)` declares, 0 when it
has none (or a malformed one: the type checker reports it).

```fides
pub fn struct_dimension(anns: Vec<Annotation>) -> i64 {
```

**Parameters:**

- `anns`: `Vec<Annotation>`

**Returns:** `i64`

*Defined at line 772*

---

## binop_eq

```fides
pub fn binop_eq(a: BinOp, b: BinOp) -> bool {
```

**Parameters:**

- `a`: `BinOp`
- `b`: `BinOp`

**Returns:** `bool`

*Defined at line 787*

---

## unaryop_eq

```fides
pub fn unaryop_eq(a: UnaryOp, b: UnaryOp) -> bool {
```

**Parameters:**

- `a`: `UnaryOp`
- `b`: `UnaryOp`

**Returns:** `bool`

*Defined at line 955*

---

## UnaryOp

```fides
pub enum UnaryOp {
```

**Variants:**

- `Neg`
- `Not`
- `BitNot`

*Defined at line 988*

---

## AssignOp

```fides
pub enum AssignOp {
```

**Variants:**

- `Assign`
- `AddAssign`
- `SubAssign`
- `MulAssign`
- `DivAssign`
- `ModAssign`
- `BitAndAssign`
- `BitOrAssign`
- `BitXorAssign`
- `ShlAssign`
- `ShrAssign`

*Defined at line 994*

---

## FStringPart

```fides
pub enum FStringPart {
```

**Variants:**

- `Literal`
- `Expr`

*Defined at line 1010*

---

## MatchArm

```fides
pub struct MatchArm {
```

**Fields:**

| Name | Type |
|------|------|
| `pattern` | `Pattern` |
| `guard` | `OptBoxExpr` |
| `body` | `Vec<Stmt>` |

*Defined at line 1015*

---

## ReflectOp

```fides
pub enum ReflectOp {
```

**Variants:**

- `TypeName`
- `IsStruct`
- `IsEnum`
- `FieldCount`
- `FieldNames`
- `FieldTypes`
- `EnumVariants`
- `StructSize`
- `// The operand of FieldOffset / EnumDiscriminant is Type::Named`
- `// of FunctionEffects / FunctionCapabilities Type::Named`
- `FieldOffset`
- `EnumDiscriminant`
- `FunctionEffects`
- `FunctionCapabilities`

*Defined at line 1021*

---

## AsmDir

```fides
pub enum AsmDir {
```

**Variants:**

- `In`
- `Out`
- `Clobber`

*Defined at line 1038*

---

## AsmOperand

```fides
pub struct AsmOperand {
```

**Fields:**

| Name | Type |
|------|------|
| `direction` | `AsmDir` |
| `reg` | `str` |
| `expr` | `Expr` |

*Defined at line 1044*

---

## Expr

```fides
pub enum Expr {
```

**Variants:**

- `IntLit`
- `FloatLit`
- `StringLit`
- `BoolLit`
- `Ident`
- `Binary`
- `Unary`
- `Call`
- `FieldAccess`
- `OptionalFieldAccess`
- `StructLit`
- `If`
- `Range`
- `Block`
- `MethodCall`
- `ArrayLit`
- `Index`
- `FString`
- `Match`
- `EnumLit`
- `Ref`
- `Deref`
- `StrConcat`
- `Try`
- `Coalesce`
- `IsCheck`
- `TryBlock`
- `TupleLit`
- `Closure`
- `Cast`
- `Asm`
- `Comptime`
- `ComptimeBlock`
- `ComptimeReflect`
- `PipeForward`
- `QuantCast`
- `Await`
- `Spawn`
- `Yield`
- `NamedArg`

*Defined at line 1050*

---

## Pattern

```fides
pub enum Pattern {
```

**Variants:**

- `Wildcard`
- `IntLit`
- `BoolLit`
- `Ident`
- `EnumVariant`
- `StrLit`
- `Range`

*Defined at line 1095*

---

## SelectCase

```fides
pub struct SelectCase {
```

**Fields:**

| Name | Type |
|------|------|
| `channel` | `Expr` |
| `var_name` | `str` |
| `body` | `Vec<Stmt>` |

*Defined at line 1107*

---

## EffectHandler

```fides
pub struct EffectHandler {
```

**Fields:**

| Name | Type |
|------|------|
| `effect` | `str` |
| `op` | `str` |
| `params` | `Vec<str>` |
| `body` | `Vec<Stmt>` |

*Defined at line 1113*

---

## Stmt

```fides
pub enum Stmt {
```

**Variants:**

- `Let`
- `LetTuple`
- `LetUninit`
- `Assign`
- `IndexAssign`
- `DerefAssign`
- `FieldAssign`
- `Expr`
- `Return`
- `If`
- `While`
- `For`
- `Loop`
- `Break`
- `Continue`
- `Defer`
- `Match`
- `Use`
- `Handle`
- `Do`
- `RegionBlock`
- `Sandbox`
- `Unsafe`
- `Txn`
- `ComptimeGen`
- `GradBlock`
- `Backward`
- `Query`
- `Select`
- `Parallel`
- `// F13: parallel { | branch1 | branch2 | ... }`
- `Resume`
- `// resume`
- `SpawnScope`
- `// spawn scope { body } — structured concurrency`
- `CancelScope`

*Defined at line 1120*

---

## Type_const_of

The wire form of const-generic value `n` (n >= 0).

```fides
pub fn Type_const_of(n: i64) -> Type {
```

**Parameters:**

- `n`: `i64`

**Returns:** `Type`

*Defined at line 1170*

---

## Type_const_value

The value a const-generic wire type carries, or -1 when it is none.

```fides
pub fn Type_const_value(t: Type) -> i64 {
```

**Parameters:**

- `t`: `Type`

**Returns:** `i64`

*Defined at line 1175*

---

## Param

```fides
pub struct Param {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `ty` | `Type` |
| `is_pub` | `bool` |
| `annotations` | `Vec<Annotation>` |
| `has_default` | `bool` |
| `default_expr` | `Expr` |
| `is_variadic` | `bool` |

*Defined at line 1188*

---

## Annotation

```fides
pub struct Annotation {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `args` | `Vec<str>` |

*Defined at line 1198*

---

## Func

```fides
pub struct Func {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `type_params` | `Vec<str>` |
| `type_param_bounds` | `Vec<StrPair>` |
| `const_params` | `Vec<StrPair>` |
| `params` | `Vec<Param>` |
| `ret_ty` | `Type` |
| `body` | `Vec<Stmt>` |
| `is_pub` | `bool` |
| `is_comptime` | `bool` |
| `declared_effects` | `Vec<str>` |
| `has_effect_annotation` | `bool` |
| `is_interrupt` | `bool` |
| `annotations` | `Vec<Annotation>` |
| `preconditions` | `Vec<Expr>` |
| `postconditions` | `Vec<Expr>` |
| `is_async` | `bool` |
| `line` | `i64` |

*Defined at line 1203*

---

## StructDef

```fides
pub struct StructDef {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `fields` | `Vec<Param>` |
| `is_pub` | `bool` |
| `annotations` | `Vec<Annotation>` |
| `type_params` | `Vec<str>` |

*Defined at line 1225*

---

## EnumVariant

```fides
pub struct EnumVariant {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `data` | `OptParams` |

*Defined at line 1233*

---

## EnumDef

```fides
pub struct EnumDef {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `variants` | `Vec<EnumVariant>` |
| `is_pub` | `bool` |
| `type_params` | `Vec<str>` |

*Defined at line 1238*

---

## ConstDef

```fides
pub struct ConstDef {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `ty` | `Type` |
| `expr` | `Expr` |
| `is_pub` | `bool` |

*Defined at line 1245*

---

## GlobalVar

```fides
pub struct GlobalVar {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `ty` | `Type` |
| `init` | `Expr` |
| `is_pub` | `bool` |

*Defined at line 1253*

---

## TypeAlias

```fides
pub struct TypeAlias {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `alias_for` | `Type` |
| `is_pub` | `bool` |

*Defined at line 1260*

---

## ImplDef

```fides
pub struct ImplDef {
```

**Fields:**

| Name | Type |
|------|------|
| `struct_name` | `str` |
| `type_params` | `Vec<str>` |
| `methods` | `Vec<Func>` |

*Defined at line 1266*

---

## TraitMethod

```fides
pub struct TraitMethod {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `params` | `Vec<Param>` |
| `ret_ty` | `Type` |
| `has_self` | `bool` |
| `is_pub` | `bool` |
| `default_body` | `OptStmts` |
| `annotations` | `Vec<Annotation>` |
| `type_params` | `Vec<str>` |
| `type_param_bounds` | `Vec<StrPair>` |
| `is_async` | `bool` |

*Defined at line 1272*

---

## TraitDef

```fides
pub struct TraitDef {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `methods` | `Vec<TraitMethod>` |
| `associated_types` | `Vec<AssocTypeDecl>` |
| `is_pub` | `bool` |
| `supertraits` | `Vec<str>` |

*Defined at line 1291*

---

## TraitImpl

```fides
pub struct TraitImpl {
```

**Fields:**

| Name | Type |
|------|------|
| `trait_name` | `str` |
| `type_name` | `str` |
| `type_params` | `Vec<str>` |
| `methods` | `Vec<Func>` |
| `associated_types` | `Vec<AssocTypeBinding>` |

*Defined at line 1300*

---

## ImportDef

```fides
pub struct ImportDef {
```

**Fields:**

| Name | Type |
|------|------|
| `path` | `str` |
| `items` | `Vec<str>` |
| `capabilities` | `Vec<str>` |
| `version` | `OptStr` |
| `alias` | `OptStr` |
| `is_pub` | `bool` |
| `is_glob` | `bool` |

*Defined at line 1308*

---

## Attribute

```fides
pub struct Attribute {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `args` | `Vec<str>` |

*Defined at line 1318*

---

## ExternDecl

```fides
pub struct ExternDecl {
```

**Fields:**

| Name | Type |
|------|------|
| `abi` | `str` |
| `c_name` | `str` |
| `pledge_name` | `str` |
| `params` | `Vec<Param>` |
| `ret_ty` | `Type` |
| `include` | `OptStr` |
| `link_lib` | `OptStr` |

*Defined at line 1323*

---

## LayerDef

```fides
pub struct LayerDef {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `fields` | `Vec<Param>` |
| `constructor_args` | `Vec<str>` |
| `forward` | `Func` |
| `is_pub` | `bool` |

*Defined at line 1335*

---

## ModelDef

```fides
pub struct ModelDef {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `layers` | `Vec<LayerInst>` |
| `forward` | `Func` |
| `is_pub` | `bool` |

*Defined at line 1343*

---

## ComponentDef

```fides
pub struct ComponentDef {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `fields` | `Vec<Param>` |
| `is_pub` | `bool` |

*Defined at line 1350*

---

## SystemDef

```fides
pub struct SystemDef {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `params` | `Vec<Param>` |
| `body` | `Vec<Stmt>` |
| `is_pub` | `bool` |

*Defined at line 1356*

---

## StateTransition

```fides
pub struct StateTransition {
```

**Fields:**

| Name | Type |
|------|------|
| `event` | `str` |
| `target` | `str` |

*Defined at line 1363*

---

## StateDef

```fides
pub struct StateDef {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `transitions` | `Vec<StateTransition>` |

*Defined at line 1368*

---

## StateMachineDef

```fides
pub struct StateMachineDef {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `states` | `Vec<StateDef>` |
| `is_pub` | `bool` |

*Defined at line 1373*

---

## EventHandlerDef

```fides
pub struct EventHandlerDef {
```

**Fields:**

| Name | Type |
|------|------|
| `event_name` | `str` |
| `params` | `Vec<Param>` |
| `body` | `Vec<Stmt>` |
| `is_pub` | `bool` |

*Defined at line 1379*

---

## Program

```fides
pub struct Program {
```

**Fields:**

| Name | Type |
|------|------|
| `module` | `OptStr` |
| `funcs` | `Vec<Func>` |
| `structs` | `Vec<StructDef>` |
| `enums` | `Vec<EnumDef>` |
| `consts` | `Vec<ConstDef>` |
| `globals` | `Vec<GlobalVar>` |
| `type_aliases` | `Vec<TypeAlias>` |
| `impls` | `Vec<ImplDef>` |
| `imports` | `Vec<ImportDef>` |
| `attributes` | `Vec<Attribute>` |
| `traits` | `Vec<TraitDef>` |
| `trait_impls` | `Vec<TraitImpl>` |
| `extern_decls` | `Vec<ExternDecl>` |
| `layers` | `Vec<LayerDef>` |
| `models` | `Vec<ModelDef>` |
| `components` | `Vec<ComponentDef>` |
| `systems` | `Vec<SystemDef>` |
| `state_machines` | `Vec<StateMachineDef>` |
| `event_handlers` | `Vec<EventHandlerDef>` |

*Defined at line 1388*

---

## program_new

```fides
pub fn program_new() -> Program {
```

**Returns:** `Program`

*Defined at line 1412*

---

## stmt_line

```fides
pub fn stmt_line(s: Stmt) -> i64 {
```

**Parameters:**

- `s`: `Stmt`

**Returns:** `i64`

*Defined at line 1420*

---

## shape_dim_to_str

```fides
pub fn shape_dim_to_str(d: ShapeDim) -> str {
```

**Parameters:**

- `d`: `ShapeDim`

**Returns:** `str`

*Defined at line 1526*

---

## box_type_to_str

```fides
pub fn box_type_to_str(b: Box<Type>) -> str {
```

**Parameters:**

- `b`: `Box<Type>`

**Returns:** `str`

*Defined at line 1543*

---

## type_to_str

```fides
pub fn type_to_str(t: Type) -> str {
```

**Parameters:**

- `t`: `Type`

**Returns:** `str`

*Defined at line 1697*

---

## spec_base_name

The name a specialization `name__spec<k>` specializes, or "".

```fides
pub fn spec_base_name(name: str) -> str {
```

**Parameters:**

- `name`: `str`

**Returns:** `str`

*Defined at line 1712*

---

## spec_match

Whether pattern `pat` (its type parameters `tps`) takes type `t`, with
the bindings so far in `names`/`types` (a parameter bound twice must be
the same type both times).

```fides
pub fn spec_match(pat: Type, t: Type, tps: Vec<str>, names: &mut Vec<str>, types: &mut Vec<Type>) -> bool {
```

**Parameters:**

- `pat`: `Type`
- `t`: `Type`
- `tps`: `Vec<str>`
- `names`: `&mut Vec<str>`
- `types`: `&mut Vec<Type>`

**Returns:** `bool`

*Defined at line 1735*

---

## spec_takes

Whether the parameters `pats` (type parameters `tps`) take the argument
types `args`.

```fides
pub fn spec_takes(pats: Vec<Type>, tps: Vec<str>, args: Vec<Type>) -> bool {
```

**Parameters:**

- `pats`: `Vec<Type>`
- `tps`: `Vec<str>`
- `args`: `Vec<Type>`

**Returns:** `bool`

*Defined at line 1819*

---

## spec_freshen

`t` with the type parameters `tps` renamed apart (`'T`), so that a
definition's own parameters are read as fixed types when it is compared
with another definition.

```fides
pub fn spec_freshen(t: Type, tps: Vec<str>) -> Type {
```

**Parameters:**

- `t`: `Type`
- `tps`: `Vec<str>`

**Returns:** `Type`

*Defined at line 1838*

---

## spec_param_types

The parameter types of `f`.

```fides
pub fn spec_param_types(f: Func) -> Vec<Type> {
```

**Parameters:**

- `f`: `Func`

**Returns:** `Vec<Type>`

*Defined at line 1876*

---

## spec_at_least

Whether definition `a` is at least as specific as `b`: `b`'s parameters
take `a`'s (with `a`'s own type parameters held fixed).

```fides
pub fn spec_at_least(a: Func, b: Func) -> bool {
```

**Parameters:**

- `a`: `Func`
- `b`: `Func`

**Returns:** `bool`

*Defined at line 1888*

---

## spec_choose

Of the definitions `cands`, the one an instantiation with argument types
`args` takes its body from: the most specific that takes them. -1 when
none does; `tie` gets the index of a second, as specific, when two tie.

```fides
pub fn spec_choose(cands: Vec<Func>, args: Vec<Type>, tie: &mut Vec<i64>) -> i64 {
```

**Parameters:**

- `cands`: `Vec<Func>`
- `args`: `Vec<Type>`
- `tie`: `&mut Vec<i64>`

**Returns:** `i64`

*Defined at line 1901*

---

## FieldInit_new

```fides
pub fn FieldInit_new(name: str, value: Expr) -> FieldInit {
```

**Parameters:**

- `name`: `str`
- `value`: `Expr`

**Returns:** `FieldInit`

*Defined at line 1948*

---

## StrPair_new

```fides
pub fn StrPair_new(a: str, b: str) -> StrPair {
```

**Parameters:**

- `a`: `str`
- `b`: `str`

**Returns:** `StrPair`

*Defined at line 1954*

---

## Binding_new

```fides
pub fn Binding_new(name: str, ty: str) -> Binding {
```

**Parameters:**

- `name`: `str`
- `ty`: `str`

**Returns:** `Binding`

*Defined at line 1960*

---

## AssocTypeDecl_new

```fides
pub fn AssocTypeDecl_new(name: str, type_params: Vec<str>) -> AssocTypeDecl {
```

**Parameters:**

- `name`: `str`
- `type_params`: `Vec<str>`

**Returns:** `AssocTypeDecl`

*Defined at line 1966*

---

## AssocTypeBinding_new

```fides
pub fn AssocTypeBinding_new(name: str, ty: Type) -> AssocTypeBinding {
```

**Parameters:**

- `name`: `str`
- `ty`: `Type`

**Returns:** `AssocTypeBinding`

*Defined at line 1972*

---

## LayerInst_new

```fides
pub fn LayerInst_new(field: str, layer_type: str, args: Vec<str>) -> LayerInst {
```

**Parameters:**

- `field`: `str`
- `layer_type`: `str`
- `args`: `Vec<str>`

**Returns:** `LayerInst`

*Defined at line 1978*

---

## Param_new

```fides
pub fn Param_new(name: str, ty: Type, is_pub: bool) -> Param {
```

**Parameters:**

- `name`: `str`
- `ty`: `Type`
- `is_pub`: `bool`

**Returns:** `Param`

*Defined at line 1984*

---

## ast_spread_defaults

```fides
pub fn ast_spread_defaults() -> Expr {
```

**Returns:** `Expr`

*Defined at line 1997*

---

## ast_is_spread_defaults

```fides
pub fn ast_is_spread_defaults(e: Expr) -> bool {
```

**Parameters:**

- `e`: `Expr`

**Returns:** `bool`

*Defined at line 2001*

---

## Annotation_new

```fides
pub fn Annotation_new(name: str, args: Vec<str>) -> Annotation {
```

**Parameters:**

- `name`: `str`
- `args`: `Vec<str>`

**Returns:** `Annotation`

*Defined at line 2012*

---

## MatchArm_new

```fides
pub fn MatchArm_new(pattern: Pattern, guard: OptBoxExpr, body: Vec<Stmt>) -> MatchArm {
```

**Parameters:**

- `pattern`: `Pattern`
- `guard`: `OptBoxExpr`
- `body`: `Vec<Stmt>`

**Returns:** `MatchArm`

*Defined at line 2018*

---

## AsmOperand_new

```fides
pub fn AsmOperand_new(direction: AsmDir, reg: str, expr: Expr) -> AsmOperand {
```

**Parameters:**

- `direction`: `AsmDir`
- `reg`: `str`
- `expr`: `Expr`

**Returns:** `AsmOperand`

*Defined at line 2024*

---

## SelectCase_new

```fides
pub fn SelectCase_new(channel: Expr, var_name: str, body: Vec<Stmt>) -> SelectCase {
```

**Parameters:**

- `channel`: `Expr`
- `var_name`: `str`
- `body`: `Vec<Stmt>`

**Returns:** `SelectCase`

*Defined at line 2030*

---

## EffectHandler_new

```fides
pub fn EffectHandler_new(effect: str, op: str, params: Vec<str>, body: Vec<Stmt>) -> EffectHandler {
```

**Parameters:**

- `effect`: `str`
- `op`: `str`
- `params`: `Vec<str>`
- `body`: `Vec<Stmt>`

**Returns:** `EffectHandler`

*Defined at line 2036*

---

## EnumVariant_new

```fides
pub fn EnumVariant_new(name: str, data: OptParams) -> EnumVariant {
```

**Parameters:**

- `name`: `str`
- `data`: `OptParams`

**Returns:** `EnumVariant`

*Defined at line 2042*

---

## Func_new

```fides
pub fn Func_new(name: str, ret_ty: Type) -> Func {
```

**Parameters:**

- `name`: `str`
- `ret_ty`: `Type`

**Returns:** `Func`

*Defined at line 2048*

---

## Func_new_full

Full constructor for Func with all fields specified.

```fides
pub fn Func_new_full(name: str, params: Vec<Param>, ret_ty: Type, body: Vec<Stmt>, is_pub: bool) -> Func {
```

**Parameters:**

- `name`: `str`
- `params`: `Vec<Param>`
- `ret_ty`: `Type`
- `body`: `Vec<Stmt>`
- `is_pub`: `bool`

**Returns:** `Func`

*Defined at line 2055*

---

## StructDef_new

```fides
pub fn StructDef_new(name: str, is_pub: bool) -> StructDef {
```

**Parameters:**

- `name`: `str`
- `is_pub`: `bool`

**Returns:** `StructDef`

*Defined at line 2061*

---

## StructDef_new_full

Full constructor for StructDef with all fields.

```fides
pub fn StructDef_new_full(name: str, fields: Vec<Param>, is_pub: bool, annotations: Vec<Annotation>, type_params: Vec<str>) -> StructDef {
```

**Parameters:**

- `name`: `str`
- `fields`: `Vec<Param>`
- `is_pub`: `bool`
- `annotations`: `Vec<Annotation>`
- `type_params`: `Vec<str>`

**Returns:** `StructDef`

*Defined at line 2068*

---

## EnumDef_new

```fides
pub fn EnumDef_new(name: str, variants: Vec<EnumVariant>, is_pub: bool) -> EnumDef {
```

**Parameters:**

- `name`: `str`
- `variants`: `Vec<EnumVariant>`
- `is_pub`: `bool`

**Returns:** `EnumDef`

*Defined at line 2074*

---

## ConstDef_new

```fides
pub fn ConstDef_new(name: str, ty: Type, expr: Expr, is_pub: bool) -> ConstDef {
```

**Parameters:**

- `name`: `str`
- `ty`: `Type`
- `expr`: `Expr`
- `is_pub`: `bool`

**Returns:** `ConstDef`

*Defined at line 2080*

---

## GlobalVar_new

```fides
pub fn GlobalVar_new(name: str, ty: Type, init: Expr, is_pub: bool) -> GlobalVar {
```

**Parameters:**

- `name`: `str`
- `ty`: `Type`
- `init`: `Expr`
- `is_pub`: `bool`

**Returns:** `GlobalVar`

*Defined at line 2086*

---

## TypeAlias_new

```fides
pub fn TypeAlias_new(name: str, alias_for: Type, is_pub: bool) -> TypeAlias {
```

**Parameters:**

- `name`: `str`
- `alias_for`: `Type`
- `is_pub`: `bool`

**Returns:** `TypeAlias`

*Defined at line 2092*

---

## ImplDef_new

```fides
pub fn ImplDef_new(struct_name: str) -> ImplDef {
```

**Parameters:**

- `struct_name`: `str`

**Returns:** `ImplDef`

*Defined at line 2098*

---

## ImplDef_new_full

Full constructor for ImplDef with methods.

```fides
pub fn ImplDef_new_full(struct_name: str, type_params: Vec<str>, methods: Vec<Func>) -> ImplDef {
```

**Parameters:**

- `struct_name`: `str`
- `type_params`: `Vec<str>`
- `methods`: `Vec<Func>`

**Returns:** `ImplDef`

*Defined at line 2105*

---

## TraitMethod_new

```fides
pub fn TraitMethod_new(name: str, ret_ty: Type, has_self: bool) -> TraitMethod {
```

**Parameters:**

- `name`: `str`
- `ret_ty`: `Type`
- `has_self`: `bool`

**Returns:** `TraitMethod`

*Defined at line 2111*

---

## TraitDef_new

```fides
pub fn TraitDef_new(name: str, methods: Vec<TraitMethod>) -> TraitDef {
```

**Parameters:**

- `name`: `str`
- `methods`: `Vec<TraitMethod>`

**Returns:** `TraitDef`

*Defined at line 2117*

---

## TraitDef_empty

Default empty TraitDef for initialization.

```fides
pub fn TraitDef_empty() -> TraitDef {
```

**Returns:** `TraitDef`

*Defined at line 2124*

---

## box_type_unbox

```fides
pub fn box_type_unbox(b: Box<Type>) -> Type {
```

**Parameters:**

- `b`: `Box<Type>`

**Returns:** `Type`

*Defined at line 2135*

---

## box_pattern_unbox

```fides
pub fn box_pattern_unbox(b: Box<Pattern>) -> Pattern {
```

**Parameters:**

- `b`: `Box<Pattern>`

**Returns:** `Pattern`

*Defined at line 2225*

---

## box_expr_new

```fides
pub fn box_expr_new(e: Expr) -> Box<Expr> {
```

**Parameters:**

- `e`: `Expr`

**Returns:** `Box<Expr>`

*Defined at line 2252*

---

## box_expr_unbox

```fides
pub fn box_expr_unbox(b: Box<Expr>) -> Expr {
```

**Parameters:**

- `b`: `Box<Expr>`

**Returns:** `Expr`

*Defined at line 2257*

---

## TraitImpl_new

```fides
pub fn TraitImpl_new(trait_name: str, type_name: str) -> TraitImpl {
```

**Parameters:**

- `trait_name`: `str`
- `type_name`: `str`

**Returns:** `TraitImpl`

*Defined at line 2383*

---

## ImportDef_new

```fides
pub fn ImportDef_new(path: str, is_glob: bool) -> ImportDef {
```

**Parameters:**

- `path`: `str`
- `is_glob`: `bool`

**Returns:** `ImportDef`

*Defined at line 2389*

---

## ImportDef_new_full

Full constructor for ImportDef with all fields.

```fides
pub fn ImportDef_new_full(path: str, items: Vec<str>, capabilities: Vec<str>, version: OptStr, alias: OptStr, is_pub: bool, is_glob: bool) -> ImportDef {
```

**Parameters:**

- `path`: `str`
- `items`: `Vec<str>`
- `capabilities`: `Vec<str>`
- `version`: `OptStr`
- `alias`: `OptStr`
- `is_pub`: `bool`
- `is_glob`: `bool`

**Returns:** `ImportDef`

*Defined at line 2396*

---

## Attribute_new

```fides
pub fn Attribute_new(name: str, args: Vec<str>) -> Attribute {
```

**Parameters:**

- `name`: `str`
- `args`: `Vec<str>`

**Returns:** `Attribute`

*Defined at line 2402*

---

## ExternDecl_new

```fides
pub fn ExternDecl_new(abi: str, c_name: str, pledge_name: str, ret_ty: Type) -> ExternDecl {
```

**Parameters:**

- `abi`: `str`
- `c_name`: `str`
- `pledge_name`: `str`
- `ret_ty`: `Type`

**Returns:** `ExternDecl`

*Defined at line 2408*

---

## LayerDef_new

```fides
pub fn LayerDef_new(name: str, forward: Func, is_pub: bool) -> LayerDef {
```

**Parameters:**

- `name`: `str`
- `forward`: `Func`
- `is_pub`: `bool`

**Returns:** `LayerDef`

*Defined at line 2414*

---

## ModelDef_new

```fides
pub fn ModelDef_new(name: str, layers: Vec<LayerInst>, forward: Func, is_pub: bool) -> ModelDef {
```

**Parameters:**

- `name`: `str`
- `layers`: `Vec<LayerInst>`
- `forward`: `Func`
- `is_pub`: `bool`

**Returns:** `ModelDef`

*Defined at line 2420*

---

## ComponentDef_new

```fides
pub fn ComponentDef_new(name: str, is_pub: bool) -> ComponentDef {
```

**Parameters:**

- `name`: `str`
- `is_pub`: `bool`

**Returns:** `ComponentDef`

*Defined at line 2426*

---

## SystemDef_new

```fides
pub fn SystemDef_new(name: str) -> SystemDef {
```

**Parameters:**

- `name`: `str`

**Returns:** `SystemDef`

*Defined at line 2432*

---

## StateMachineDef_new

```fides
pub fn StateMachineDef_new(name: str, is_pub: bool) -> StateMachineDef {
```

**Parameters:**

- `name`: `str`
- `is_pub`: `bool`

**Returns:** `StateMachineDef`

*Defined at line 2438*

---

## StateDef_new

```fides
pub fn StateDef_new(name: str) -> StateDef {
```

**Parameters:**

- `name`: `str`

**Returns:** `StateDef`

*Defined at line 2444*

---

## StateTransition_new

```fides
pub fn StateTransition_new(event: str, target: str) -> StateTransition {
```

**Parameters:**

- `event`: `str`
- `target`: `str`

**Returns:** `StateTransition`

*Defined at line 2450*

---

## EventHandlerDef_new

```fides
pub fn EventHandlerDef_new(event_name: str) -> EventHandlerDef {
```

**Parameters:**

- `event_name`: `str`

**Returns:** `EventHandlerDef`

*Defined at line 2456*

---

## ann_lifetimes

The lifetimes an annotation list carries (none: empty).

```fides
pub fn ann_lifetimes(anns: Vec<Annotation>) -> Vec<str> {
```

**Parameters:**

- `anns`: `Vec<Annotation>`

**Returns:** `Vec<str>`

*Defined at line 2467*

---

## func_has_lifetimes

Whether `f`'s signature names its result's lifetimes (`-> &'a T`).

```fides
pub fn func_has_lifetimes(f: Func) -> bool {
```

**Parameters:**

- `f`: `Func`

**Returns:** `bool`

*Defined at line 2479*

---

## func_lifetime_ties

The parameters (by index) `f`'s result borrows from by its signature:
those whose type carries one of the result's lifetimes. `'static` ties
the result to no parameter.

```fides
pub fn func_lifetime_ties(f: Func) -> Vec<i64> {
```

**Parameters:**

- `f`: `Func`

**Returns:** `Vec<i64>`

*Defined at line 2486*

---

## trait_method_lifetime_ties

The same for a trait's method declaration (LG3).

```fides
pub fn trait_method_lifetime_ties(tm: TraitMethod) -> Vec<i64> {
```

**Parameters:**

- `tm`: `TraitMethod`

**Returns:** `Vec<i64>`

*Defined at line 2491*

---

## lifetime_ties

The parameters (by index) carrying one of `ret_lts`.

```fides
pub fn lifetime_ties(ret_lts: Vec<str>, params: Vec<Param>) -> Vec<i64> {
```

**Parameters:**

- `ret_lts`: `Vec<str>`
- `params`: `Vec<Param>`

**Returns:** `Vec<i64>`

*Defined at line 2496*

---

## str_vec_contains

```fides
pub fn str_vec_contains(v: Vec<str>, s: str) -> bool {
```

**Parameters:**

- `v`: `Vec<str>`
- `s`: `str`

**Returns:** `bool`

*Defined at line 2516*

---

## vec_contains_str

```fides
pub fn vec_contains_str(v: Vec<str>, s: str) -> bool {
```

**Parameters:**

- `v`: `Vec<str>`
- `s`: `str`

**Returns:** `bool`

*Defined at line 2528*

---

## escape_str

Escape a string for assembly output.

```fides
pub fn escape_str(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 2541*

---

## vec_contains_i64

```fides
pub fn vec_contains_i64(vec: Vec<i64>, value: i64) -> bool {
```

**Parameters:**

- `vec`: `Vec<i64>`
- `value`: `i64`

**Returns:** `bool`

*Defined at line 2571*

---

## vec_find_i64

```fides
pub fn vec_find_i64(vec: Vec<i64>, value: i64) -> i64 {
```

**Parameters:**

- `vec`: `Vec<i64>`
- `value`: `i64`

**Returns:** `i64`

*Defined at line 2583*

---

## box_expr_deref

```fides
pub fn box_expr_deref(b: Box<Expr>) -> Expr {
```

**Parameters:**

- `b`: `Box<Expr>`

**Returns:** `Expr`

*Defined at line 2595*

---

## ct_public_param_add

```fides
pub fn ct_public_param_add(name: str, pi: i64) -> void {
```

**Parameters:**

- `name`: `str`
- `pi`: `i64`

**Returns:** `void`

*Defined at line 2730*

---

## ct_public_param

Whether parameter `pi` of the IR function `name` is @public. A generic
function's instances are named `<name>__<types>`: they take the
parameters of their source.

```fides
pub fn ct_public_param(name: str, pi: i64) -> bool {
```

**Parameters:**

- `name`: `str`
- `pi`: `i64`

**Returns:** `bool`

*Defined at line 2740*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
