[All modules](index.md)

# compiler::lower -- Fides Documentation

## Contents

- [lw_box_type_unbox](#lw_box_type_unbox) (function)
- [lw_box_expr_unbox](#lw_box_expr_unbox) (function)
- [StringStructDefEntry](#StringStructDefEntry) (struct)
- [StringStructDefEntry_new](#StringStructDefEntry_new) (function)
- [StringEnumDefEntry](#StringEnumDefEntry) (struct)
- [StringEnumDefEntry_new](#StringEnumDefEntry_new) (function)
- [StringValueIdEntry](#StringValueIdEntry) (struct)
- [StringValueIdEntry_new](#StringValueIdEntry_new) (function)
- [I64IrTypeEntry](#I64IrTypeEntry) (struct)
- [I64IrTypeEntry_new](#I64IrTypeEntry_new) (function)
- [StringTypeEntry](#StringTypeEntry) (struct)
- [StringTypeEntry_new](#StringTypeEntry_new) (function)
- [StringVecStrEntry](#StringVecStrEntry) (struct)
- [StringVecStrEntry_new](#StringVecStrEntry_new) (function)
- [StringVecExprEntry](#StringVecExprEntry) (struct)
- [StringVecExprEntry_new](#StringVecExprEntry_new) (function)
- [StringExprEntry](#StringExprEntry) (struct)
- [StringExprEntry_new](#StringExprEntry_new) (function)
- [StringStringFuncEntry](#StringStringFuncEntry) (struct)
- [StringStringFuncEntry_new](#StringStringFuncEntry_new) (function)
- [LwTxn](#LwTxn) (struct)
- [LoopContext](#LoopContext) (struct)
- [LoopContext_new](#LoopContext_new) (function)
- [TryContext](#TryContext) (struct)
- [TryContext_new](#TryContext_new) (function)
- [DeferEntry](#DeferEntry) (struct)
- [DeferEntry_new](#DeferEntry_new) (function)
- [ClosureCapture](#ClosureCapture) (struct)
- [ClosureCapture_new](#ClosureCapture_new) (function)
- [OptFunc](#OptFunc) (enum)
- [OptStructDef](#OptStructDef) (enum)
- [OptEnumDef](#OptEnumDef) (enum)
- [OptExpr](#OptExpr) (enum)
- [OptType](#OptType) (enum)
- [OptStringLower](#OptStringLower) (enum)
- [lw_c_str_vec_contains](#lw_c_str_vec_contains) (function)
- [is_vec_type_name](#is_vec_type_name) (function)
- [get_vec_type_name](#get_vec_type_name) (function)
- [is_valid_swizzle](#is_valid_swizzle) (function)
- [swizzle_index](#swizzle_index) (function)
- [runtime_effects](#runtime_effects) (function)
- [OptEffectSet](#OptEffectSet) (enum)
- [OwnSlot](#OwnSlot) (struct)
- [Lowerer](#Lowerer) (struct)
- [lw_c_vec1_i64](#lw_c_vec1_i64) (function)
- [c_vec2_i64](#c_vec2_i64) (function)
- [Lowerer_new](#Lowerer_new) (function)
- [Lowerer_current_region](#Lowerer_current_region) (function)
- [Lowerer_load_enum_field](#Lowerer_load_enum_field) (function)
- [Lowerer_maybe_deref_box](#Lowerer_maybe_deref_box) (function)
- [Lowerer_enum_field_slots](#Lowerer_enum_field_slots) (function)
- [Lowerer_set_linear_structs](#Lowerer_set_linear_structs) (function)
- [Lowerer_is_linear_ir_type](#Lowerer_is_linear_ir_type) (function)
- [Lowerer_lookup_struct](#Lowerer_lookup_struct) (function)
- [Lowerer_lookup_enum](#Lowerer_lookup_enum) (function)
- [Lowerer_lookup_pattern_enum](#Lowerer_lookup_pattern_enum) (function)
- [Lowerer_bare_pattern_is_unit_variant](#Lowerer_bare_pattern_is_unit_variant) (function)
- [Lowerer_pattern_binding_ir_types](#Lowerer_pattern_binding_ir_types) (function)
- [Lowerer_pattern_binding_params](#Lowerer_pattern_binding_params) (function)
- [Lowerer_load_param_payload](#Lowerer_load_param_payload) (function)
- [Lowerer_pattern_binding_ast_types](#Lowerer_pattern_binding_ast_types) (function)
- [Lowerer_lookup_alloca](#Lowerer_lookup_alloca) (function)
- [Lowerer_lookup_alloca_type](#Lowerer_lookup_alloca_type) (function)
- [Lowerer_lookup_var_ast_type](#Lowerer_lookup_var_ast_type) (function)
- [Lowerer_lookup_constant](#Lowerer_lookup_constant) (function)
- [Lowerer_lookup_impl_method](#Lowerer_lookup_impl_method) (function)
- [Lowerer_mut_self_impl](#Lowerer_mut_self_impl) (function)
- [Lowerer_lookup_func_ret_type](#Lowerer_lookup_func_ret_type) (function)
- [Lowerer_lookup_type_alias](#Lowerer_lookup_type_alias) (function)
- [Lowerer_is_async_func](#Lowerer_is_async_func) (function)
- [Lowerer_is_extern_func](#Lowerer_is_extern_func) (function)
- [Lowerer_lookup_param_type](#Lowerer_lookup_param_type) (function)
- [Lowerer_is_vec_alloca](#Lowerer_is_vec_alloca) (function)
- [Lowerer_insert_struct](#Lowerer_insert_struct) (function)
- [Lowerer_insert_enum](#Lowerer_insert_enum) (function)
- [Lowerer_insert_alloca](#Lowerer_insert_alloca) (function)
- [Lowerer_insert_alloca_type](#Lowerer_insert_alloca_type) (function)
- [Lowerer_insert_var_ast_type](#Lowerer_insert_var_ast_type) (function)
- [Lowerer_insert_constant](#Lowerer_insert_constant) (function)
- [Lowerer_insert_impl_method](#Lowerer_insert_impl_method) (function)
- [Lowerer_insert_func_ret_type](#Lowerer_insert_func_ret_type) (function)
- [Lowerer_insert_type_alias](#Lowerer_insert_type_alias) (function)
- [Lowerer_insert_param_type](#Lowerer_insert_param_type) (function)
- [Lowerer_insert_async_func](#Lowerer_insert_async_func) (function)
- [Lowerer_insert_linear_struct](#Lowerer_insert_linear_struct) (function)
- [Lowerer_insert_vec_alloca](#Lowerer_insert_vec_alloca) (function)
- [Lowerer_is_user_func](#Lowerer_is_user_func) (function)
- [Lowerer_has_function](#Lowerer_has_function) (function)
- [Lowerer_is_struct](#Lowerer_is_struct) (function)
- [Lowerer_emit_inst](#Lowerer_emit_inst) (function)
- [Lowerer_new_value](#Lowerer_new_value) (function)
- [Lowerer_new_block](#Lowerer_new_block) (function)
- [Lowerer_set_current_block](#Lowerer_set_current_block) (function)
- [Lowerer_current_block_has_terminator](#Lowerer_current_block_has_terminator) (function)
- [Lowerer_set_value_type](#Lowerer_set_value_type) (function)
- [Lowerer_get_value_type](#Lowerer_get_value_type) (function)
- [Lowerer_emit_terminator](#Lowerer_emit_terminator) (function)
- [Lowerer_bind_local](#Lowerer_bind_local) (function)
- [Lowerer_lookup_local](#Lowerer_lookup_local) (function)
- [Lowerer_add_block_param](#Lowerer_add_block_param) (function)
- [Lowerer_add_extern_0](#Lowerer_add_extern_0) (function)
- [Lowerer_add_extern_1](#Lowerer_add_extern_1) (function)
- [Lowerer_add_extern_2](#Lowerer_add_extern_2) (function)
- [Lowerer_add_extern_3](#Lowerer_add_extern_3) (function)
- [Lowerer_add_extern_4](#Lowerer_add_extern_4) (function)
- [Lowerer_add_extern_5](#Lowerer_add_extern_5) (function)
- [lw_ptr_u8](#lw_ptr_u8) (function)
- [opaque_ty](#opaque_ty) (function)
- [Lowerer_resolve_type](#Lowerer_resolve_type) (function)
- [Lowerer_lower_type](#Lowerer_lower_type) (function)
- [lw_tuple_field_ty](#lw_tuple_field_ty) (function)
- [lw_tuple_layout_name](#lw_tuple_layout_name) (function)
- [lw_tuple_name_fields](#lw_tuple_name_fields) (function)
- [Lowerer_register_tuple_layout](#Lowerer_register_tuple_layout) (function)
- [Lowerer_register_ret_tuple](#Lowerer_register_ret_tuple) (function)
- [Lowerer_lower_type_sret](#Lowerer_lower_type_sret) (function)
- [Lowerer_resolve_struct_name](#Lowerer_resolve_struct_name) (function)
- [Lowerer_is_opaque_type_param](#Lowerer_is_opaque_type_param) (function)
- [Lowerer_auto_deref_recv](#Lowerer_auto_deref_recv) (function)
- [word_repr](#word_repr) (function)
- [ir_types_repr_compatible](#ir_types_repr_compatible) (function)
- [Lowerer_ast_type_from_name](#Lowerer_ast_type_from_name) (function)
- [Lowerer_resolve_self_in_type](#Lowerer_resolve_self_in_type) (function)
- [Lowerer_compute_struct_layout](#Lowerer_compute_struct_layout) (function)
- [Lowerer_make_struct_layout](#Lowerer_make_struct_layout) (function)
- [Lowerer_compute_enum_layout](#Lowerer_compute_enum_layout) (function)
- [Lowerer_make_enum_layout](#Lowerer_make_enum_layout) (function)
- [Lowerer_register_runtime](#Lowerer_register_runtime) (function)
- [Lowerer_lower_program](#Lowerer_lower_program) (function)
- [lower](#lower) (function)
- [LwBorrows](#LwBorrows) (struct)
- [has_annotation](#has_annotation) (function)
- [find_annotation](#find_annotation) (function)
- [OptAnnotation](#OptAnnotation) (enum)
- [Lowerer_lower_func](#Lowerer_lower_func) (function)
- [Lowerer_lower_stmts](#Lowerer_lower_stmts) (function)
- [Lowerer_lower_stmt](#Lowerer_lower_stmt) (function)
- [Lowerer_vecv_method](#Lowerer_vecv_method) (function)
- [Lowerer_lower_method_call](#Lowerer_lower_method_call) (function)
- [Lowerer_lower_box_new](#Lowerer_lower_box_new) (function)
- [Lowerer_lower_assoc_call](#Lowerer_lower_assoc_call) (function)
- [Lowerer_resolve_type_alias_name](#Lowerer_resolve_type_alias_name) (function)
- [Lowerer_mangle_method](#Lowerer_mangle_method) (function)
- [Lowerer_box_vec_element](#Lowerer_box_vec_element) (function)
- [Lowerer_emit_array_bound_check](#Lowerer_emit_array_bound_check) (function)
- [Lowerer_box_struct_arg](#Lowerer_box_struct_arg) (function)
- [Lowerer_lower_expr](#Lowerer_lower_expr) (function)
- [Lowerer_find_captures](#Lowerer_find_captures) (function)
- [Lowerer_collect_free_vars_arm](#Lowerer_collect_free_vars_arm) (function)
- [lw_pattern_names](#lw_pattern_names) (function)
- [Lowerer_collect_free_vars](#Lowerer_collect_free_vars) (function)
- [Lowerer_collect_free_vars_stmt](#Lowerer_collect_free_vars_stmt) (function)
- [Lowerer_collect_free_vars_expr](#Lowerer_collect_free_vars_expr) (function)
- [Lowerer_make_fn_env](#Lowerer_make_fn_env) (function)
- [Lowerer_lower_while](#Lowerer_lower_while) (function)
- [Lowerer_lower_for](#Lowerer_lower_for) (function)
- [Lowerer_lower_for_string](#Lowerer_lower_for_string) (function)
- [Lowerer_lower_for_vec](#Lowerer_lower_for_vec) (function)
- [Lowerer_lower_loop](#Lowerer_lower_loop) (function)
- [Lowerer_lower_match](#Lowerer_lower_match) (function)
- [Lowerer_lower_compound_assign](#Lowerer_lower_compound_assign) (function)
- [Lowerer_type_struct_name](#Lowerer_type_struct_name) (function)
- [Lowerer_has_drop_impl](#Lowerer_has_drop_impl) (function)
- [Lowerer_emit_drops](#Lowerer_emit_drops) (function)
- [Lowerer_mark_sensitive](#Lowerer_mark_sensitive) (function)
- [Lowerer_emit_zeroize](#Lowerer_emit_zeroize) (function)
- [Lowerer_infer_binop_type](#Lowerer_infer_binop_type) (function)
- [Lowerer_infer_unary_type](#Lowerer_infer_unary_type) (function)
- [Lowerer_infer_expr_type](#Lowerer_infer_expr_type) (function)
- [Lowerer_coalesce_ir_type](#Lowerer_coalesce_ir_type) (function)
- [Lowerer_lookup_func_ret_ty_ir](#Lowerer_lookup_func_ret_ty_ir) (function)
- [Lowerer_coerce_return](#Lowerer_coerce_return) (function)
- [Lowerer_int_to_float_value](#Lowerer_int_to_float_value) (function)
- [Lowerer_lower_cast](#Lowerer_lower_cast) (function)
- [Lowerer_infer_match_type](#Lowerer_infer_match_type) (function)
- [Lowerer_emit_sret_copy](#Lowerer_emit_sret_copy) (function)
- [Lowerer_emit_struct_copy](#Lowerer_emit_struct_copy) (function)
- [f64_to_bits](#f64_to_bits) (function)
- [f32_to_bits](#f32_to_bits) (function)
- [Lowerer_emit_defers](#Lowerer_emit_defers) (function)
- [Lowerer_emit_persist_saves](#Lowerer_emit_persist_saves) (function)
- [ValueIdString](#ValueIdString) (struct)
- [lw_ValueIdString_new](#lw_ValueIdString_new) (function)
- [str_parse_u64](#str_parse_u64) (function)
- [Lowerer_infer_vec_elem_type](#Lowerer_infer_vec_elem_type) (function)
- [Lowerer_infer_map_types](#Lowerer_infer_map_types) (function)
- [MapTypes](#MapTypes) (struct)
- [MapTypes_new](#MapTypes_new) (function)
- [Lowerer_infer_closure_ret_ty](#Lowerer_infer_closure_ret_ty) (function)
- [Lowerer_infer_expr_ast_type](#Lowerer_infer_expr_ast_type) (function)
- [vec_eq_fn_for](#vec_eq_fn_for) (function)
- [ir_type_to_ast_type](#ir_type_to_ast_type) (function)
- [Lowerer_lower_swizzle](#Lowerer_lower_swizzle) (function)
- [Lowerer_struct_operator](#Lowerer_struct_operator) (function)
- [Lowerer_struct_operator_ty](#Lowerer_struct_operator_ty) (function)
- [Lowerer_struct_dim](#Lowerer_struct_dim) (function)
- [Lowerer_binop_plan](#Lowerer_binop_plan) (function)
- [Lowerer_binop_plan_exprs](#Lowerer_binop_plan_exprs) (function)
- [Lowerer_dim_result_ty](#Lowerer_dim_result_ty) (function)
- [Lowerer_lower_dim_operator](#Lowerer_lower_dim_operator) (function)
- [Lowerer_lower_vec_method_call](#Lowerer_lower_vec_method_call) (function)
- [Lowerer_lower_vec_sret_call](#Lowerer_lower_vec_sret_call) (function)
- [OwnTemp](#OwnTemp) (struct)
- [OwnType](#OwnType) (struct)
- [Lowerer_rc_kind](#Lowerer_rc_kind) (function)
- [Lowerer_rc_type_index](#Lowerer_rc_type_index) (function)
- [Lowerer_rc_tid](#Lowerer_rc_tid) (function)
- [Lowerer_rc_dup](#Lowerer_rc_dup) (function)
- [Lowerer_rc_drop](#Lowerer_rc_drop) (function)
- [Lowerer_rc_temp](#Lowerer_rc_temp) (function)
- [Lowerer_rc_claim](#Lowerer_rc_claim) (function)
- [Lowerer_rc_flush](#Lowerer_rc_flush) (function)
- [Lowerer_rc_own_value](#Lowerer_rc_own_value) (function)
- [Lowerer_rc_owned_any](#Lowerer_rc_owned_any) (function)
- [Lowerer_lower_owned](#Lowerer_lower_owned) (function)
- [Lowerer_rc_new_slot](#Lowerer_rc_new_slot) (function)
- [Lowerer_rc_add_slot](#Lowerer_rc_add_slot) (function)
- [Lowerer_rc_owns_slot](#Lowerer_rc_owns_slot) (function)
- [Lowerer_rc_scope_open](#Lowerer_rc_scope_open) (function)
- [Lowerer_rc_scope_close](#Lowerer_rc_scope_close) (function)
- [Lowerer_rc_drop_all](#Lowerer_rc_drop_all) (function)
- [Lowerer_rc_loop_open](#Lowerer_rc_loop_open) (function)
- [Lowerer_rc_loop_close](#Lowerer_rc_loop_close) (function)
- [Lowerer_rc_loop_exit](#Lowerer_rc_loop_exit) (function)
- [Lowerer_rc_finish_func](#Lowerer_rc_finish_func) (function)
- [Lowerer_rc_note_sret](#Lowerer_rc_note_sret) (function)
- [Lowerer_rc_gen_types](#Lowerer_rc_gen_types) (function)
- [lw_own_type_keys](#lw_own_type_keys) (function)
- [Lowerer_rc_new_coll](#Lowerer_rc_new_coll) (function)
- [lw_rc_stack_vecs_fn](#lw_rc_stack_vecs_fn) (function)
- [lw_rc_stack_strs_fn](#lw_rc_stack_strs_fn) (function)
- [IrUnit](#IrUnit) (struct)
- [IrLinkTable](#IrLinkTable) (struct)
- [ir_units_split](#ir_units_split) (function)
- [ir_units_check](#ir_units_check) (function)
- [ir_units_link](#ir_units_link) (function)
- [ir_units_table_text](#ir_units_table_text) (function)
- [ir_units_roundtrip](#ir_units_roundtrip) (function)

## lw_box_type_unbox

```fides
pub fn lw_box_type_unbox(b: Box<Type>) -> Type {
```

**Parameters:**

- `b`: `Box<Type>`

**Returns:** `Type`

*Defined at line 29*

---

## lw_box_expr_unbox

```fides
pub fn lw_box_expr_unbox(b: Box<Expr>) -> Expr {
```

**Parameters:**

- `b`: `Box<Expr>`

**Returns:** `Expr`

*Defined at line 119*

---

## StringStructDefEntry

```fides
pub struct StringStructDefEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `val` | `StructDef` |

*Defined at line 248*

---

## StringStructDefEntry_new

```fides
pub fn StringStructDefEntry_new(key: str, val: StructDef) -> StringStructDefEntry {
```

**Parameters:**

- `key`: `str`
- `val`: `StructDef`

**Returns:** `StringStructDefEntry`

*Defined at line 253*

---

## StringEnumDefEntry

```fides
pub struct StringEnumDefEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `val` | `EnumDef` |

*Defined at line 259*

---

## StringEnumDefEntry_new

```fides
pub fn StringEnumDefEntry_new(key: str, val: EnumDef) -> StringEnumDefEntry {
```

**Parameters:**

- `key`: `str`
- `val`: `EnumDef`

**Returns:** `StringEnumDefEntry`

*Defined at line 264*

---

## StringValueIdEntry

```fides
pub struct StringValueIdEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `val` | `i64` |

*Defined at line 270*

---

## StringValueIdEntry_new

```fides
pub fn StringValueIdEntry_new(key: str, val: i64) -> StringValueIdEntry {
```

**Parameters:**

- `key`: `str`
- `val`: `i64`

**Returns:** `StringValueIdEntry`

*Defined at line 275*

---

## I64IrTypeEntry

```fides
pub struct I64IrTypeEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `i64` |
| `val` | `IrType` |

*Defined at line 281*

---

## I64IrTypeEntry_new

```fides
pub fn I64IrTypeEntry_new(key: i64, val: IrType) -> I64IrTypeEntry {
```

**Parameters:**

- `key`: `i64`
- `val`: `IrType`

**Returns:** `I64IrTypeEntry`

*Defined at line 286*

---

## StringTypeEntry

```fides
pub struct StringTypeEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `val` | `Type` |

*Defined at line 292*

---

## StringTypeEntry_new

```fides
pub fn StringTypeEntry_new(key: str, val: Type) -> StringTypeEntry {
```

**Parameters:**

- `key`: `str`
- `val`: `Type`

**Returns:** `StringTypeEntry`

*Defined at line 297*

---

## StringVecStrEntry

```fides
pub struct StringVecStrEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `val` | `Vec<str>` |

*Defined at line 303*

---

## StringVecStrEntry_new

```fides
pub fn StringVecStrEntry_new(key: str, val: Vec<str>) -> StringVecStrEntry {
```

**Parameters:**

- `key`: `str`
- `val`: `Vec<str>`

**Returns:** `StringVecStrEntry`

*Defined at line 308*

---

## StringVecExprEntry

```fides
pub struct StringVecExprEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `val` | `Vec<Expr>` |

*Defined at line 314*

---

## StringVecExprEntry_new

```fides
pub fn StringVecExprEntry_new(key: str, val: Vec<Expr>) -> StringVecExprEntry {
```

**Parameters:**

- `key`: `str`
- `val`: `Vec<Expr>`

**Returns:** `StringVecExprEntry`

*Defined at line 319*

---

## StringExprEntry

```fides
pub struct StringExprEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `val` | `Expr` |

*Defined at line 325*

---

## StringExprEntry_new

```fides
pub fn StringExprEntry_new(key: str, val: Expr) -> StringExprEntry {
```

**Parameters:**

- `key`: `str`
- `val`: `Expr`

**Returns:** `StringExprEntry`

*Defined at line 330*

---

## StringStringFuncEntry

```fides
pub struct StringStringFuncEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `key1` | `str` |
| `key2` | `str` |
| `val` | `Func` |

*Defined at line 336*

---

## StringStringFuncEntry_new

```fides
pub fn StringStringFuncEntry_new(key1: str, key2: str, val: Func) -> StringStringFuncEntry {
```

**Parameters:**

- `key1`: `str`
- `key2`: `str`
- `val`: `Func`

**Returns:** `StringStringFuncEntry`

*Defined at line 342*

---

## LwTxn

A txn { } being lowered: a snapshot of each live local, restored by
`abort;`, which then jumps to `end_block`. `owned[i]`: the local owns
its value (the snapshot took an owner and is dropped on commit);
otherwise both copies are raw words or bytes. `mark`: own_slots at
entry, so abort drops what the body declared.

```fides
pub struct LwTxn {
```

**Fields:**

| Name | Type |
|------|------|
| `slots` | `Vec<i64>` |
| `snaps` | `Vec<i64>` |
| `tys` | `Vec<Type>` |
| `irtys` | `Vec<IrType>` |
| `aggs` | `Vec<bool>` |
| `owned` | `Vec<bool>` |
| `end_block` | `i64` |
| `mark` | `i64` |

*Defined at line 353*

---

## LoopContext

```fides
pub struct LoopContext {
```

**Fields:**

| Name | Type |
|------|------|
| `continue_target` | `i64` |
| `break_target` | `i64` |
| `result_slot` | `OptValueId` |

*Defined at line 365*

---

## LoopContext_new

```fides
pub fn LoopContext_new(continue_target: i64, break_target: i64, result_slot: OptValueId) -> LoopContext {
```

**Parameters:**

- `continue_target`: `i64`
- `break_target`: `i64`
- `result_slot`: `OptValueId`

**Returns:** `LoopContext`

*Defined at line 371*

---

## TryContext

```fides
pub struct TryContext {
```

**Fields:**

| Name | Type |
|------|------|
| `error_block` | `i64` |
| `error_result_slot` | `i64` |

*Defined at line 378*

---

## TryContext_new

```fides
pub fn TryContext_new(error_block: i64, error_result_slot: i64) -> TryContext {
```

**Parameters:**

- `error_block`: `i64`
- `error_result_slot`: `i64`

**Returns:** `TryContext`

*Defined at line 383*

---

## DeferEntry

```fides
pub struct DeferEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `body` | `Vec<Stmt>` |
| `flag` | `i64` |
| `line` | `i64` |
| `names` | `Vec<str>` |
| `slots` | `Vec<i64>` |
| `irtys` | `Vec<IrType>` |
| `asts` | `Vec<Type>` |
| `params` | `Vec<i64>` |
| `keeps` | `Vec<i64>` |
| `otys` | `Vec<Type>` |

*Defined at line 399*

---

## DeferEntry_new

```fides
pub fn DeferEntry_new(body: Vec<Stmt>, flag: i64, line: i64) -> DeferEntry {
```

**Parameters:**

- `body`: `Vec<Stmt>`
- `flag`: `i64`
- `line`: `i64`

**Returns:** `DeferEntry`

*Defined at line 412*

---

## ClosureCapture

```fides
pub struct ClosureCapture {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `field_index` | `i64` |
| `ty` | `Type` |

*Defined at line 419*

---

## ClosureCapture_new

```fides
pub fn ClosureCapture_new(name: str, field_index: i64, ty: Type) -> ClosureCapture {
```

**Parameters:**

- `name`: `str`
- `field_index`: `i64`
- `ty`: `Type`

**Returns:** `ClosureCapture`

*Defined at line 425*

---

## OptFunc

```fides
pub enum OptFunc {
```

**Variants:**

- `Some`
- `None`

*Defined at line 433*

---

## OptStructDef

```fides
pub enum OptStructDef {
```

**Variants:**

- `Some`
- `None`

*Defined at line 438*

---

## OptEnumDef

```fides
pub enum OptEnumDef {
```

**Variants:**

- `Some`
- `None`

*Defined at line 443*

---

## OptExpr

```fides
pub enum OptExpr {
```

**Variants:**

- `Some`
- `None`

*Defined at line 448*

---

## OptType

```fides
pub enum OptType {
```

**Variants:**

- `Some`
- `None`

*Defined at line 453*

---

## OptStringLower

```fides
pub enum OptStringLower {
```

**Variants:**

- `Some`
- `None`

*Defined at line 458*

---

## lw_c_str_vec_contains

Check if a string is contained in a Vec<str>.

```fides
pub fn lw_c_str_vec_contains(vec: Vec<str>, s: str) -> bool {
```

**Parameters:**

- `vec`: `Vec<str>`
- `s`: `str`

**Returns:** `bool`

*Defined at line 466*

---

## is_vec_type_name

```fides
pub fn is_vec_type_name(name: str) -> bool {
```

**Parameters:**

- `name`: `str`

**Returns:** `bool`

*Defined at line 477*

---

## get_vec_type_name

```fides
pub fn get_vec_type_name(ty: IrType) -> OptStringLower {
```

**Parameters:**

- `ty`: `IrType`

**Returns:** `OptStringLower`

*Defined at line 490*

---

## is_valid_swizzle

```fides
pub fn is_valid_swizzle(field: str) -> bool {
```

**Parameters:**

- `field`: `str`

**Returns:** `bool`

*Defined at line 508*

---

## swizzle_index

```fides
pub fn swizzle_index(c: i64) -> i64 {
```

**Parameters:**

- `c`: `i64`

**Returns:** `i64`

*Defined at line 561*

---

## runtime_effects

```fides
pub fn runtime_effects(name: str) -> OptEffectSet {
```

**Parameters:**

- `name`: `str`

**Returns:** `OptEffectSet`

*Defined at line 580*

---

## OptEffectSet

```fides
pub enum OptEffectSet {
```

**Variants:**

- `Some`
- `None`

*Defined at line 697*

---

## OwnSlot

A variable slot that owns its value.

```fides
pub struct OwnSlot {
```

**Fields:**

| Name | Type |
|------|------|
| `slot` | `i64` |
| `ty` | `Type` |

*Defined at line 703*

---

## Lowerer

```fides
pub struct Lowerer {
```

**Fields:**

| Name | Type |
|------|------|
| `module` | `IrModule` |
| `current_func` | `IrFunction` |
| `has_current_func` | `bool` |
| `current_block` | `i64` |
| `loop_stack` | `Vec<LoopContext>` |
| `structs` | `Vec<StringStructDefEntry>` |
| `allocas` | `Vec<StringValueIdEntry>` |
| `alloca_types` | `Vec<I64IrTypeEntry>` |
| `var_ast_types` | `Vec<StringTypeEntry>` |
| `alloca_ix` | `i64` |
| `var_ast_ix` | `i64` |
| `alloca_type_ix` | `i64` |
| `match_scrutinee_ty` | `Type` |
| `constants` | `Vec<StringExprEntry>` |
| `current_sret` | `OptValueId` |
| `enums` | `Vec<StringEnumDefEntry>` |
| `enum_index` | `i64` |
| `struct_index` | `i64` |
| `impl_index` | `i64` |
| `extern_index` | `i64` |
| `impl_methods` | `Vec<StringStringFuncEntry>` |
| `defer_stack` | `Vec<DeferEntry>` |
| `current_captures` | `Vec<ClosureCapture>` |
| `try_stack` | `Vec<TryContext>` |
| `effect_handler_stack` | `Vec<Vec<EffectHandler>>` |
| `global_inits` | `Vec<GlobalVar>` |
| `handle_slot_fn` | `str` |
| `handle_slot` | `i64` |
| `scope_executor_stack` | `Vec<i64>` |
| `current_line` | `OptU64` |
| `type_aliases` | `Vec<StringTypeEntry>` |
| `current_self_type` | `OptStringLower` |
| `region_stack` | `Vec<str>` |
| `gpu_region_stack` | `Vec<str>` |
| `sandbox_depth` | `i64` |
| `func_ret_types` | `Vec<StringTypeEntry>` |
| `async_funcs` | `Vec<str>` |
| `async_tasks` | `Vec<str>` |
| `async_adapted` | `Vec<str>` |
| `cur_body` | `Vec<Stmt>` |
| `cur_has_defer` | `bool` |
| `no_struct_share` | `bool` |
| `rest_of` | `Vec<Stmt>` |
| `rest_from` | `i64` |
| `fr_lists` | `Vec<Vec<Stmt>>` |
| `fr_idx` | `Vec<i64>` |
| `fr_lu` | `Vec<i64>` |
| `rest_lv` | `i64` |
| `fn_externs` | `Vec<str>` |
| `current_func_param_types` | `Vec<StringTypeEntry>` |
| `vec_allocas` | `Vec<i64>` |
| `linear_structs` | `Vec<str>` |
| `let_hint` | `OptType` |
| `func_param_names` | `Vec<StringVecStrEntry>` |
| `func_defaults` | `Vec<StringVecExprEntry>` |
| `func_info_at` | `i64` |
| `func_param_asts` | `Vec<Vec<Type>>` |
| `continuation_counter` | `i64` |
| `to_toks` | `Vec<i64>` |
| `to_loops` | `Vec<i64>` |
| `to_trys` | `Vec<i64>` |
| `to_base` | `i64` |
| `to_hit` | `i64` |
| `own_slots` | `Vec<OwnSlot>` |
| `own_marks` | `Vec<i64>` |
| `own_temps` | `Vec<OwnTemp>` |
| `own_loop_slots` | `Vec<i64>` |
| `own_loop_temps` | `Vec<i64>` |
| `own_prologue` | `Vec<i64>` |
| `txn_stack` | `Vec<LwTxn>` |
| `own_on` | `bool` |
| `own_ret_ty` | `Type` |
| `own_raw_ret` | `bool` |
| `cur_line` | `i64` |
| `own_str_externs` | `Vec<str>` |
| `own_nomove` | `Vec<str>` |
| `own_types` | `Vec<OwnType>` |
| `own_type_index` | `i64` |
| `own_kind_cache` | `i64` |
| `own_ret_index` | `i64` |
| `rg_on` | `bool` |
| `rg_fns` | `Vec<str>` |
| `rg_ids` | `Vec<i64>` |
| `rg_loop_marks` | `Vec<i64>` |
| `rg_keep_out` | `bool` |

*Defined at line 710*

---

## lw_c_vec1_i64

```fides
pub fn lw_c_vec1_i64(x: i64) -> Vec<i64> {
```

**Parameters:**

- `x`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 869*

---

## c_vec2_i64

```fides
pub fn c_vec2_i64(x: i64, y: i64) -> Vec<i64> {
```

**Parameters:**

- `x`: `i64`
- `y`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 875*

---

## Lowerer_new

```fides
pub fn Lowerer_new() -> Lowerer {
```

**Returns:** `Lowerer`

*Defined at line 882*

---

## Lowerer_current_region

```fides
pub fn Lowerer_current_region(self: & Lowerer) -> Region {
```

**Parameters:**

- `self`: `& Lowerer`

**Returns:** `Region`

*Defined at line 888*

---

## Lowerer_load_enum_field

```fides
pub fn Lowerer_load_enum_field(self: & mut Lowerer, field_ptr: i64, field_ty: IrType) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `field_ptr`: `i64`
- `field_ty`: `IrType`

**Returns:** `i64`

*Defined at line 901*

---

## Lowerer_maybe_deref_box

If `val` is a Box<T> (Ptr(Ptr(Struct(..))) or Generic("Box", [Struct(..)])),
load through it once so enum/struct pattern matching sees the inner struct
pointer directly. Otherwise return `val` unchanged.

NOTE: Box::new stores the struct data directly in the heap allocation (via
memcpy), so the Box pointer already points to the struct data. This function
is a no-op - the match code can read the tag/fields directly from the Box
pointer.

```fides
pub fn Lowerer_maybe_deref_box(self: & mut Lowerer, val: i64) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `val`: `i64`

**Returns:** `i64`

*Defined at line 922*

---

## Lowerer_enum_field_slots

Compute the number of 8-byte slots a field occupies in an enum layout.
Struct/enum types use their full layout size (rounded up to 8);
all other types use 1 slot.

```fides
pub fn Lowerer_enum_field_slots(self: & Lowerer, ty: IrType) -> i64 {
```

**Parameters:**

- `self`: `& Lowerer`
- `ty`: `IrType`

**Returns:** `i64`

*Defined at line 932*

---

## Lowerer_set_linear_structs

```fides
pub fn Lowerer_set_linear_structs(self: & mut Lowerer, structs: Vec<str>) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `structs`: `Vec<str>`

*Defined at line 951*

---

## Lowerer_is_linear_ir_type

```fides
pub fn Lowerer_is_linear_ir_type(self: & Lowerer, ty: IrType) -> bool {
```

**Parameters:**

- `self`: `& Lowerer`
- `ty`: `IrType`

**Returns:** `bool`

*Defined at line 959*

---

## Lowerer_lookup_struct

```fides
pub fn Lowerer_lookup_struct(self: & Lowerer, name: str) -> OptStructDef {
```

**Parameters:**

- `self`: `& Lowerer`
- `name`: `str`

**Returns:** `OptStructDef`

*Defined at line 980*

---

## Lowerer_lookup_enum

```fides
pub fn Lowerer_lookup_enum(self: & Lowerer, name: str) -> OptEnumDef {
```

**Parameters:**

- `self`: `& Lowerer`
- `name`: `str`

**Returns:** `OptEnumDef`

*Defined at line 1000*

---

## Lowerer_lookup_pattern_enum

Look up the EnumDef a variant pattern refers to. `enum_name` may be
empty for shorthand patterns like `Some(x)`, in which case we search
all enums for a variant with that name.

```fides
pub fn Lowerer_lookup_pattern_enum(self: & Lowerer, enum_name: str, variant: str) -> OptEnumDef {
```

**Parameters:**

- `self`: `& Lowerer`
- `enum_name`: `str`
- `variant`: `str`

**Returns:** `OptEnumDef`

*Defined at line 1018*

---

## Lowerer_bare_pattern_is_unit_variant

A bare name in a match arm that is a unit variant of the scrutinee's enum
(`None =>` on an `Option`) is that variant, as `Some(x)` is (reference
§6): the type checker binds no variable for it
(TypeChecker_bare_pattern_is_unit_variant). Before, the lowering bound the
name to the scrutinee, so the arm matched every value and the arms after
it could not run.

```fides
pub fn Lowerer_bare_pattern_is_unit_variant(self: & Lowerer, scrut_expr: Expr, name: str) -> bool {
```

**Parameters:**

- `self`: `& Lowerer`
- `scrut_expr`: `Expr`
- `name`: `str`

**Returns:** `bool`

*Defined at line 1095*

---

## Lowerer_pattern_binding_ir_types

IR types for the values bound by an enum-variant pattern.
Enum-typed fields (structs/enums declared directly, not Box/Ptr) are
stored INLINE in the parent enum (per Lowerer_make_enum_layout). Returning
`Struct(name)` for these lets `load_enum_field` copy the full inline data
via alloca+memcpy.
Box/Ptr/Vec/str fields are one machine word wide (a pointer) and pass
through `word_repr` unchanged.

```fides
pub fn Lowerer_pattern_binding_ir_types(self: & Lowerer, enum_name: str, variant: str, bindings: Vec<Binding>) -> Vec<IrType> {
```

**Parameters:**

- `self`: `& Lowerer`
- `enum_name`: `str`
- `variant`: `str`
- `bindings`: `Vec<Binding>`

**Returns:** `Vec<IrType>`

*Defined at line 1130*

---

## Lowerer_pattern_binding_params

For each payload of `En::Variant`: whether it is a type parameter of the
enum (one word, holding a struct or enum by pointer to a heap copy).

```fides
pub fn Lowerer_pattern_binding_params(self: & Lowerer, enum_name: str, variant: str) -> Vec<bool> {
```

**Parameters:**

- `self`: `& Lowerer`
- `enum_name`: `str`
- `variant`: `str`

**Returns:** `Vec<bool>`

*Defined at line 1197*

---

## Lowerer_load_param_payload

Read a type-parameter payload: a struct or enum is copied out of its heap
box into a local of its own; anything else is the word itself.

```fides
pub fn Lowerer_load_param_payload(self: & mut Lowerer, match_val: i64, field_idx: i64, field_ty: IrType) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `match_val`: `i64`
- `field_idx`: `i64`
- `field_ty`: `IrType`

**Returns:** `i64`

*Defined at line 1228*

---

## Lowerer_pattern_binding_ast_types

Like `pattern_binding_ir_types` but returns the AST types, which
we need to register in `var_ast_types` so that `infer_expr_type`
can recover generic element types (e.g. `Vec<FieldInit>`).

```fides
pub fn Lowerer_pattern_binding_ast_types(self: & Lowerer, enum_name: str, variant: str, bindings: Vec<Binding>) -> Vec<Type> {
```

**Parameters:**

- `self`: `& Lowerer`
- `enum_name`: `str`
- `variant`: `str`
- `bindings`: `Vec<Binding>`

**Returns:** `Vec<Type>`

*Defined at line 1247*

---

## Lowerer_lookup_alloca

```fides
pub fn Lowerer_lookup_alloca(self: & Lowerer, name: str) -> OptValueId {
```

**Parameters:**

- `self`: `& Lowerer`
- `name`: `str`

**Returns:** `OptValueId`

*Defined at line 1344*

---

## Lowerer_lookup_alloca_type

```fides
pub fn Lowerer_lookup_alloca_type(self: & Lowerer, id: i64) -> OptIrType {
```

**Parameters:**

- `self`: `& Lowerer`
- `id`: `i64`

**Returns:** `OptIrType`

*Defined at line 1370*

---

## Lowerer_lookup_var_ast_type

```fides
pub fn Lowerer_lookup_var_ast_type(self: & Lowerer, name: str) -> OptType {
```

**Parameters:**

- `self`: `& Lowerer`
- `name`: `str`

**Returns:** `OptType`

*Defined at line 1391*

---

## Lowerer_lookup_constant

```fides
pub fn Lowerer_lookup_constant(self: & Lowerer, name: str) -> OptExpr {
```

**Parameters:**

- `self`: `& Lowerer`
- `name`: `str`

**Returns:** `OptExpr`

*Defined at line 1411*

---

## Lowerer_lookup_impl_method

```fides
pub fn Lowerer_lookup_impl_method(self: & Lowerer, struct_name: str, method_name: str) -> OptFunc {
```

**Parameters:**

- `self`: `& Lowerer`
- `struct_name`: `str`
- `method_name`: `str`

**Returns:** `OptFunc`

*Defined at line 1422*

---

## Lowerer_mut_self_impl

Whether `mangled_name` (Vec_x/HashMap_x/String_x) resolves to an impl
method whose self parameter is `& mut` — those need the receiver's
slot address at the call site, not the collection word.

```fides
pub fn Lowerer_mut_self_impl(self: & Lowerer, mangled_name: str) -> bool {
```

**Parameters:**

- `self`: `& Lowerer`
- `mangled_name`: `str`

**Returns:** `bool`

*Defined at line 1451*

---

## Lowerer_lookup_func_ret_type

```fides
pub fn Lowerer_lookup_func_ret_type(self: & Lowerer, name: str) -> OptType {
```

**Parameters:**

- `self`: `& Lowerer`
- `name`: `str`

**Returns:** `OptType`

*Defined at line 1484*

---

## Lowerer_lookup_type_alias

```fides
pub fn Lowerer_lookup_type_alias(self: & Lowerer, name: str) -> OptType {
```

**Parameters:**

- `self`: `& Lowerer`
- `name`: `str`

**Returns:** `OptType`

*Defined at line 1510*

---

## Lowerer_is_async_func

```fides
pub fn Lowerer_is_async_func(self: & Lowerer, name: str) -> bool {
```

**Parameters:**

- `self`: `& Lowerer`
- `name`: `str`

**Returns:** `bool`

*Defined at line 1521*

---

## Lowerer_is_extern_func

```fides
pub fn Lowerer_is_extern_func(self: & Lowerer, name: str) -> bool {
```

**Parameters:**

- `self`: `& Lowerer`
- `name`: `str`

**Returns:** `bool`

*Defined at line 1533*

---

## Lowerer_lookup_param_type

```fides
pub fn Lowerer_lookup_param_type(self: & Lowerer, name: str) -> OptType {
```

**Parameters:**

- `self`: `& Lowerer`
- `name`: `str`

**Returns:** `OptType`

*Defined at line 1576*

---

## Lowerer_is_vec_alloca

```fides
pub fn Lowerer_is_vec_alloca(self: & Lowerer, id: i64) -> bool {
```

**Parameters:**

- `self`: `& Lowerer`
- `id`: `i64`

**Returns:** `bool`

*Defined at line 1587*

---

## Lowerer_insert_struct

```fides
pub fn Lowerer_insert_struct(self: & mut Lowerer, name: str, val: StructDef) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `name`: `str`
- `val`: `StructDef`

*Defined at line 1600*

---

## Lowerer_insert_enum

```fides
pub fn Lowerer_insert_enum(self: & mut Lowerer, name: str, val: EnumDef) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `name`: `str`
- `val`: `EnumDef`

*Defined at line 1608*

---

## Lowerer_insert_alloca

```fides
pub fn Lowerer_insert_alloca(self: & mut Lowerer, name: str, val: i64) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `name`: `str`
- `val`: `i64`

*Defined at line 1615*

---

## Lowerer_insert_alloca_type

```fides
pub fn Lowerer_insert_alloca_type(self: & mut Lowerer, id: i64, ty: IrType) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `id`: `i64`
- `ty`: `IrType`

*Defined at line 1738*

---

## Lowerer_insert_var_ast_type

```fides
pub fn Lowerer_insert_var_ast_type(self: & mut Lowerer, name: str, ty: Type) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `name`: `str`
- `ty`: `Type`

*Defined at line 1968*

---

## Lowerer_insert_constant

```fides
pub fn Lowerer_insert_constant(self: & mut Lowerer, name: str, expr: Expr) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `name`: `str`
- `expr`: `Expr`

*Defined at line 1977*

---

## Lowerer_insert_impl_method

```fides
pub fn Lowerer_insert_impl_method(self: & mut Lowerer, struct_name: str, method_name: str, val: Func) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `struct_name`: `str`
- `method_name`: `str`
- `val`: `Func`

*Defined at line 1981*

---

## Lowerer_insert_func_ret_type

```fides
pub fn Lowerer_insert_func_ret_type(self: & mut Lowerer, name: str, ty: Type) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `name`: `str`
- `ty`: `Type`

*Defined at line 1990*

---

## Lowerer_insert_type_alias

```fides
pub fn Lowerer_insert_type_alias(self: & mut Lowerer, name: str, ty: Type) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `name`: `str`
- `ty`: `Type`

*Defined at line 1998*

---

## Lowerer_insert_param_type

```fides
pub fn Lowerer_insert_param_type(self: & mut Lowerer, name: str, ty: Type) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `name`: `str`
- `ty`: `Type`

*Defined at line 2002*

---

## Lowerer_insert_async_func

```fides
pub fn Lowerer_insert_async_func(self: & mut Lowerer, name: str) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `name`: `str`

*Defined at line 2006*

---

## Lowerer_insert_linear_struct

```fides
pub fn Lowerer_insert_linear_struct(self: & mut Lowerer, name: str) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `name`: `str`

*Defined at line 2104*

---

## Lowerer_insert_vec_alloca

```fides
pub fn Lowerer_insert_vec_alloca(self: & mut Lowerer, id: i64) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `id`: `i64`

*Defined at line 2108*

---

## Lowerer_is_user_func

Check if a function name is a user-defined function (not a runtime builtin).
User functions get __env as first argument; runtime functions (p_*, __ecs_*) don't.

```fides
pub fn Lowerer_is_user_func(self: & Lowerer, name: str) -> bool {
```

**Parameters:**

- `self`: `& Lowerer`
- `name`: `str`

**Returns:** `bool`

*Defined at line 2367*

---

## Lowerer_has_function

Check if a function exists in the module (user functions only).

```fides
pub fn Lowerer_has_function(self: & Lowerer, name: str) -> bool {
```

**Parameters:**

- `self`: `& Lowerer`
- `name`: `str`

**Returns:** `bool`

*Defined at line 2389*

---

## Lowerer_is_struct

```fides
pub fn Lowerer_is_struct(self: & Lowerer, name: str) -> bool {
```

**Parameters:**

- `self`: `& Lowerer`
- `name`: `str`

**Returns:** `bool`

*Defined at line 2393*

---

## Lowerer_emit_inst

```fides
pub fn Lowerer_emit_inst(self: & mut Lowerer, inst: Inst) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `inst`: `Inst`

**Returns:** `i64`

*Defined at line 2412*

---

## Lowerer_new_value

```fides
pub fn Lowerer_new_value(self: & mut Lowerer, meta: ValueMeta) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `meta`: `ValueMeta`

**Returns:** `i64`

*Defined at line 2529*

---

## Lowerer_new_block

```fides
pub fn Lowerer_new_block(self: & mut Lowerer) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`

**Returns:** `i64`

*Defined at line 2533*

---

## Lowerer_set_current_block

```fides
pub fn Lowerer_set_current_block(self: & mut Lowerer, block: i64) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `block`: `i64`

*Defined at line 2538*

---

## Lowerer_current_block_has_terminator

```fides
pub fn Lowerer_current_block_has_terminator(self: & Lowerer) -> bool {
```

**Parameters:**

- `self`: `& Lowerer`

**Returns:** `bool`

*Defined at line 2543*

---

## Lowerer_set_value_type

```fides
pub fn Lowerer_set_value_type(self: & mut Lowerer, id: i64, ty: IrType) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `id`: `i64`
- `ty`: `IrType`

*Defined at line 2568*

---

## Lowerer_get_value_type

```fides
pub fn Lowerer_get_value_type(self: & Lowerer, val: i64) -> IrType {
```

**Parameters:**

- `self`: `& Lowerer`
- `val`: `i64`

**Returns:** `IrType`

*Defined at line 2578*

---

## Lowerer_emit_terminator

```fides
pub fn Lowerer_emit_terminator(self: & mut Lowerer, inst: Inst) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `inst`: `Inst`

*Defined at line 2594*

---

## Lowerer_bind_local

```fides
pub fn Lowerer_bind_local(self: & mut Lowerer, name: str, val: i64) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `name`: `str`
- `val`: `i64`

*Defined at line 2598*

---

## Lowerer_lookup_local

```fides
pub fn Lowerer_lookup_local(self: & Lowerer, name: str) -> OptValueId {
```

**Parameters:**

- `self`: `& Lowerer`
- `name`: `str`

**Returns:** `OptValueId`

*Defined at line 2602*

---

## Lowerer_add_block_param

```fides
pub fn Lowerer_add_block_param(self: & mut Lowerer, block: i64, ty: IrType) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `block`: `i64`
- `ty`: `IrType`

**Returns:** `i64`

*Defined at line 2606*

---

## Lowerer_add_extern_0

```fides
pub fn Lowerer_add_extern_0(self: & mut Lowerer, name: str, ret: IrType) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `name`: `str`
- `ret`: `IrType`

*Defined at line 2613*

---

## Lowerer_add_extern_1

```fides
pub fn Lowerer_add_extern_1(self: & mut Lowerer, name: str, p1: IrType, ret: IrType) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `name`: `str`
- `p1`: `IrType`
- `ret`: `IrType`

*Defined at line 2618*

---

## Lowerer_add_extern_2

```fides
pub fn Lowerer_add_extern_2(self: & mut Lowerer, name: str, p1: IrType, p2: IrType, ret: IrType) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `name`: `str`
- `p1`: `IrType`
- `p2`: `IrType`
- `ret`: `IrType`

*Defined at line 2624*

---

## Lowerer_add_extern_3

```fides
pub fn Lowerer_add_extern_3(self: & mut Lowerer, name: str, p1: IrType, p2: IrType, p3: IrType, ret: IrType) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `name`: `str`
- `p1`: `IrType`
- `p2`: `IrType`
- `p3`: `IrType`
- `ret`: `IrType`

*Defined at line 2631*

---

## Lowerer_add_extern_4

```fides
pub fn Lowerer_add_extern_4(self: & mut Lowerer, name: str, p1: IrType, p2: IrType, p3: IrType, p4: IrType, ret: IrType) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `name`: `str`
- `p1`: `IrType`
- `p2`: `IrType`
- `p3`: `IrType`
- `p4`: `IrType`
- `ret`: `IrType`

*Defined at line 2639*

---

## Lowerer_add_extern_5

```fides
pub fn Lowerer_add_extern_5(self: & mut Lowerer, name: str, p1: IrType, p2: IrType, p3: IrType, p4: IrType, p5: IrType, ret: IrType) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `name`: `str`
- `p1`: `IrType`
- `p2`: `IrType`
- `p3`: `IrType`
- `p4`: `IrType`
- `p5`: `IrType`
- `ret`: `IrType`

*Defined at line 2648*

---

## lw_ptr_u8

```fides
pub fn lw_ptr_u8() -> IrType {
```

**Returns:** `IrType`

*Defined at line 2659*

---

## opaque_ty

```fides
pub fn opaque_ty(name: str) -> IrType {
```

**Parameters:**

- `name`: `str`

**Returns:** `IrType`

*Defined at line 2662*

---

## Lowerer_resolve_type

```fides
pub fn Lowerer_resolve_type(self: & Lowerer, ty: Type) -> Type {
```

**Parameters:**

- `self`: `& Lowerer`
- `ty`: `Type`

**Returns:** `Type`

*Defined at line 2668*

---

## Lowerer_lower_type

```fides
pub fn Lowerer_lower_type(self: & Lowerer, ty: Type) -> IrType {
```

**Parameters:**

- `self`: `& Lowerer`
- `ty`: `Type`

**Returns:** `IrType`

*Defined at line 2709*

---

## lw_tuple_field_ty

Like lower_type, but converts enum types (e.g. Result<T,E>, Option<T>)
from Generic("Result", ...) to Struct("Result") so that SRET calling
convention treats them as aggregate types.
Used only at function signature boundaries (return type + call sites).
Field type of a tuple element: a struct pointer is stored inline as the
struct (src/lower.rs TupleLit; Lowerer_register_tuple_layout).

```fides
pub fn lw_tuple_field_ty(ty: IrType) -> IrType {
```

**Parameters:**

- `ty`: `IrType`

**Returns:** `IrType`

*Defined at line 2751*

---

## lw_tuple_layout_name

The name of the layout of a tuple whose fields (lw_tuple_field_ty of
the element types) are `ftys`: `__tuple_<n>_<codes>`, one code per field
(lw_tuple_field_code), `__tuple_0` for the empty tuple. Named by its
shape, so two tuples of one length, `(str, i64)` and `((str, i64),
i64)`, have a layout each (a name made of the length alone gave both
one layout, the last registered, and the other's fields were read at
its offsets). The name holds the field types: a cached function's
layouts are made again from it (Lowerer_irc_tuple_layouts).

```fides
pub fn lw_tuple_layout_name(ftys: Vec<IrType>) -> str {
```

**Parameters:**

- `ftys`: `Vec<IrType>`

**Returns:** `str`

*Defined at line 2861*

---

## lw_tuple_name_fields

The field types a tuple layout name (lw_tuple_layout_name) holds, as
types that lay it out the same (a pointer is a `*u8`, a word an opaque
word, other room a byte array); an empty list when `name` is no such
name (or is `__tuple_0`).

```fides
pub fn lw_tuple_name_fields(name: str) -> Vec<IrType> {
```

**Parameters:**

- `name`: `str`

**Returns:** `Vec<IrType>`

*Defined at line 2894*

---

## Lowerer_register_tuple_layout

Register the layout of a tuple of IR element types (a field each,
lw_tuple_field_ty of its type, every field at a multiple of 8) under
its shape's name (lw_tuple_layout_name), and return that name.

```fides
pub fn Lowerer_register_tuple_layout(self: & mut Lowerer, elem_tys: Vec<IrType>) -> str {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `elem_tys`: `Vec<IrType>`

**Returns:** `str`

*Defined at line 2966*

---

## Lowerer_register_ret_tuple

A function returning a tuple registers its layout, as a tuple literal
of its element types lays it out (Lowerer_key_tuple_ir: a nested tuple
and a struct inline).

```fides
pub fn Lowerer_register_ret_tuple(self: & mut Lowerer, ret: Type) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `ret`: `Type`

*Defined at line 3068*

---

## Lowerer_lower_type_sret

```fides
pub fn Lowerer_lower_type_sret(self: & mut Lowerer, ty: Type) -> IrType {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `ty`: `Type`

**Returns:** `IrType`

*Defined at line 3078*

---

## Lowerer_resolve_struct_name

```fides
pub fn Lowerer_resolve_struct_name(self: & Lowerer, name: str) -> str {
```

**Parameters:**

- `self`: `& Lowerer`
- `name`: `str`

**Returns:** `str`

*Defined at line 3119*

---

## Lowerer_is_opaque_type_param

```fides
pub fn Lowerer_is_opaque_type_param(self: & Lowerer, ty: Type) -> bool {
```

**Parameters:**

- `self`: `& Lowerer`
- `ty`: `Type`

**Returns:** `bool`

*Defined at line 3139*

---

## Lowerer_auto_deref_recv

```fides
pub fn Lowerer_auto_deref_recv(self: & mut Lowerer, recv_val: i64, recv_ty: IrType) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `recv_val`: `i64`
- `recv_ty`: `IrType`

**Returns:** `i64`

*Defined at line 3189*

---

## word_repr

```fides
pub fn word_repr(ty: IrType) -> IrType {
```

**Parameters:**

- `ty`: `IrType`

**Returns:** `IrType`

*Defined at line 3218*

---

## ir_types_repr_compatible

```fides
pub fn ir_types_repr_compatible(a: IrType, b: IrType) -> bool {
```

**Parameters:**

- `a`: `IrType`
- `b`: `IrType`

**Returns:** `bool`

*Defined at line 3233*

---

## Lowerer_ast_type_from_name

```fides
pub fn Lowerer_ast_type_from_name(self: & Lowerer, name: str) -> OptType {
```

**Parameters:**

- `self`: `& Lowerer`
- `name`: `str`

**Returns:** `OptType`

*Defined at line 3246*

---

## Lowerer_resolve_self_in_type

```fides
pub fn Lowerer_resolve_self_in_type(self: & Lowerer, ty: Type) -> Type {
```

**Parameters:**

- `self`: `& Lowerer`
- `ty`: `Type`

**Returns:** `Type`

*Defined at line 3300*

---

## Lowerer_compute_struct_layout

```fides
pub fn Lowerer_compute_struct_layout(self: & mut Lowerer, def: StructDef) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `def`: `StructDef`

*Defined at line 3356*

---

## Lowerer_make_struct_layout

```fides
pub fn Lowerer_make_struct_layout(self: & Lowerer, def: StructDef) -> StructLayout {
```

**Parameters:**

- `self`: `& Lowerer`
- `def`: `StructDef`

**Returns:** `StructLayout`

*Defined at line 3375*

---

## Lowerer_compute_enum_layout

```fides
pub fn Lowerer_compute_enum_layout(self: & mut Lowerer, def: EnumDef) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `def`: `EnumDef`

*Defined at line 3567*

---

## Lowerer_make_enum_layout

```fides
pub fn Lowerer_make_enum_layout(self: & Lowerer, def: EnumDef) -> StructLayout {
```

**Parameters:**

- `self`: `& Lowerer`
- `def`: `EnumDef`

**Returns:** `StructLayout`

*Defined at line 3591*

---

## Lowerer_register_runtime

```fides
pub fn Lowerer_register_runtime(self: & mut Lowerer) {
```

**Parameters:**

- `self`: `& mut Lowerer`

*Defined at line 3660*

---

## Lowerer_lower_program

```fides
pub fn Lowerer_lower_program(self: & mut Lowerer, program_in: Program) -> IrModule {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `program_in`: `Program`

**Returns:** `IrModule`

*Defined at line 4122*

---

## lower

```fides
pub fn lower(program0: Program) -> IrModule {
```

**Parameters:**

- `program0`: `Program`

**Returns:** `IrModule`

*Defined at line 4933*

---

## LwBorrows

Per function, per argument: 1 for a Vec it only uses, keeping no reference
to it and leaving it to the caller to free.

```fides
pub struct LwBorrows {
```

**Fields:**

| Name | Type |
|------|------|
| `index` | `i64` |
| `rows` | `Vec<Vec<i64>>` |
| `funcs` | `Vec<i64>,  // row -> function` |

*Defined at line 5328*

---

## has_annotation

```fides
pub fn has_annotation(annotations: Vec<Annotation>, name: str) -> bool {
```

**Parameters:**

- `annotations`: `Vec<Annotation>`
- `name`: `str`

**Returns:** `bool`

*Defined at line 6791*

---

## find_annotation

```fides
pub fn find_annotation(annotations: Vec<Annotation>, name: str) -> OptAnnotation {
```

**Parameters:**

- `annotations`: `Vec<Annotation>`
- `name`: `str`

**Returns:** `OptAnnotation`

*Defined at line 6802*

---

## OptAnnotation

```fides
pub enum OptAnnotation {
```

**Variants:**

- `Some`
- `None`

*Defined at line 6813*

---

## Lowerer_lower_func

```fides
pub fn Lowerer_lower_func(self: & mut Lowerer, func: Func) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `func`: `Func`

*Defined at line 7006*

---

## Lowerer_lower_stmts

```fides
pub fn Lowerer_lower_stmts(self: & mut Lowerer, stmts: Vec<Stmt>) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `stmts`: `Vec<Stmt>`

*Defined at line 7848*

---

## Lowerer_lower_stmt

```fides
pub fn Lowerer_lower_stmt(self: & mut Lowerer, stmt: Stmt) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `stmt`: `Stmt`

*Defined at line 12120*

---

## Lowerer_vecv_method

A Vec<E..> method (0008): len/is_empty/get/last/push over core's
VecV_* functions. push's argument is the enum literal; its variant's
payload count and index come from the enum table.

```fides
pub fn Lowerer_vecv_method(self: & mut Lowerer, receiver: Box<Expr>, recv_val: i64, method: str, args: Vec<Expr>) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `receiver`: `Box<Expr>`
- `recv_val`: `i64`
- `method`: `str`
- `args`: `Vec<Expr>`

**Returns:** `i64`

*Defined at line 13678*

---

## Lowerer_lower_method_call

```fides
pub fn Lowerer_lower_method_call(self: & mut Lowerer, receiver: Box<Expr>, method: str, args: Vec<Expr>) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `receiver`: `Box<Expr>`
- `method`: `str`
- `args`: `Vec<Expr>`

**Returns:** `i64`

*Defined at line 13902*

---

## Lowerer_lower_box_new

```fides
pub fn Lowerer_lower_box_new(self: & mut Lowerer, arg_expr: Expr) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `arg_expr`: `Expr`

**Returns:** `i64`

*Defined at line 14473*

---

## Lowerer_lower_assoc_call

```fides
pub fn Lowerer_lower_assoc_call(self: & mut Lowerer, struct_name: str, method: str, args: Vec<Expr>) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `struct_name`: `str`
- `method`: `str`
- `args`: `Vec<Expr>`

**Returns:** `i64`

*Defined at line 14566*

---

## Lowerer_resolve_type_alias_name

```fides
pub fn Lowerer_resolve_type_alias_name(self: & Lowerer, name: str) -> str {
```

**Parameters:**

- `self`: `& Lowerer`
- `name`: `str`

**Returns:** `str`

*Defined at line 14632*

---

## Lowerer_mangle_method

```fides
pub fn Lowerer_mangle_method(self: & mut Lowerer, method: str, recv_ty: IrType, receiver: Box<Expr>) -> str {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `method`: `str`
- `recv_ty`: `IrType`
- `receiver`: `Box<Expr>`

**Returns:** `str`

*Defined at line 14656*

---

## Lowerer_box_vec_element

```fides
pub fn Lowerer_box_vec_element(self: & mut Lowerer, call_args: & mut Vec<i64>, recv_ty: IrType, receiver: Box<Expr>, mangled: str) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `call_args`: `& mut Vec<i64>`
- `recv_ty`: `IrType`
- `receiver`: `Box<Expr>`
- `mangled`: `str`

**Returns:** `i64`

*Defined at line 14737*

---

## Lowerer_emit_array_bound_check

M6: `if idx < 0 || idx >= N { p_panic("Array index out of bounds") }`
before a GetElemPtr into an array of `arr_ty` (lw_fixed_array_len).

```fides
pub fn Lowerer_emit_array_bound_check(self: & mut Lowerer, arr_ty: IrType, idx_val: i64) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `arr_ty`: `IrType`
- `idx_val`: `i64`

*Defined at line 15453*

---

## Lowerer_box_struct_arg

```fides
pub fn Lowerer_box_struct_arg(self: & mut Lowerer, call_args: & mut Vec<i64>, struct_ty: IrType) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `call_args`: `& mut Vec<i64>`
- `struct_ty`: `IrType`

*Defined at line 15480*

---

## Lowerer_lower_expr

```fides
pub fn Lowerer_lower_expr(self: & mut Lowerer, expr: Expr) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `expr`: `Expr`

**Returns:** `i64`

*Defined at line 19038*

---

## Lowerer_find_captures

Find free variables (captures) in a closure body.
Returns list of (name, type) tuples for variables that are used but not
declared locally or in the closure parameters. The type is looked up from
the enclosing scope's alloca types or falls back to I64.

```fides
pub fn Lowerer_find_captures(self: & Lowerer, body: Vec<Stmt>, param_names: Vec<str>) -> Vec<StringTypeEntry> {
```

**Parameters:**

- `self`: `& Lowerer`
- `body`: `Vec<Stmt>`
- `param_names`: `Vec<str>`

**Returns:** `Vec<StringTypeEntry>`

*Defined at line 20991*

---

## Lowerer_collect_free_vars_arm

Collect free variables from a list of statements.
A match arm's free variables: the names its pattern binds are the arm's
own while its guard and body are read (stdlib-agent-bugs 28: `n` of
`Option::Some(n)` in a closure was taken as a capture, stored as 0 in
the env, and the arm read that 0 instead of the payload).

```fides
pub fn Lowerer_collect_free_vars_arm(self: & Lowerer, arm: MatchArm, free_vars: & mut Vec<str>, local_names: & mut Vec<str>) {
```

**Parameters:**

- `self`: `& Lowerer`
- `arm`: `MatchArm`
- `free_vars`: `& mut Vec<str>`
- `local_names`: `& mut Vec<str>`

*Defined at line 21219*

---

## lw_pattern_names

The names a pattern binds.

```fides
pub fn lw_pattern_names(pat: Pattern, out: & mut Vec<str>) {
```

**Parameters:**

- `pat`: `Pattern`
- `out`: `& mut Vec<str>`

*Defined at line 21234*

---

## Lowerer_collect_free_vars

```fides
pub fn Lowerer_collect_free_vars(self: & Lowerer, stmts: Vec<Stmt>, free_vars: & mut Vec<str>, local_names: & mut Vec<str>) {
```

**Parameters:**

- `self`: `& Lowerer`
- `stmts`: `Vec<Stmt>`
- `free_vars`: `& mut Vec<str>`
- `local_names`: `& mut Vec<str>`

*Defined at line 21251*

---

## Lowerer_collect_free_vars_stmt

Collect free variables from a single statement.

```fides
pub fn Lowerer_collect_free_vars_stmt(self: & Lowerer, stmt: Stmt, free_vars: & mut Vec<str>, local_names: & mut Vec<str>) {
```

**Parameters:**

- `self`: `& Lowerer`
- `stmt`: `Stmt`
- `free_vars`: `& mut Vec<str>`
- `local_names`: `& mut Vec<str>`

*Defined at line 21260*

---

## Lowerer_collect_free_vars_expr

Collect free variables from an expression.

```fides
pub fn Lowerer_collect_free_vars_expr(self: & Lowerer, expr: Expr, free_vars: & mut Vec<str>, local_names: & mut Vec<str>) {
```

**Parameters:**

- `self`: `& Lowerer`
- `expr`: `Expr`
- `free_vars`: `& mut Vec<str>`
- `local_names`: `& mut Vec<str>`

*Defined at line 21362*

---

## Lowerer_make_fn_env

```fides
pub fn Lowerer_make_fn_env(self: & mut Lowerer, fn_name: str, captures: Vec<StringTypeEntry>) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `fn_name`: `str`
- `captures`: `Vec<StringTypeEntry>`

**Returns:** `i64`

*Defined at line 21583*

---

## Lowerer_lower_while

```fides
pub fn Lowerer_lower_while(self: & mut Lowerer, cond: Expr, body: Vec<Stmt>) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `cond`: `Expr`
- `body`: `Vec<Stmt>`

*Defined at line 21917*

---

## Lowerer_lower_for

```fides
pub fn Lowerer_lower_for(self: & mut Lowerer, loop_var: str, iterator: Expr, body: Vec<Stmt>) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `loop_var`: `str`
- `iterator`: `Expr`
- `body`: `Vec<Stmt>`

*Defined at line 21972*

---

## Lowerer_lower_for_string

Lower `for c in str_expr { body }` - iterate over string bytes.
Uses p_str_length for the loop bound and String_byte_at for element access.

```fides
pub fn Lowerer_lower_for_string(self: & mut Lowerer, loop_var: str, str_val: i64, body: Vec<Stmt>) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `loop_var`: `str`
- `str_val`: `i64`
- `body`: `Vec<Stmt>`

*Defined at line 22202*

---

## Lowerer_lower_for_vec

```fides
pub fn Lowerer_lower_for_vec(self: & mut Lowerer, loop_var: str, vec_val: i64, body: Vec<Stmt>) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `loop_var`: `str`
- `vec_val`: `i64`
- `body`: `Vec<Stmt>`

*Defined at line 22267*

---

## Lowerer_lower_loop

```fides
pub fn Lowerer_lower_loop(self: & mut Lowerer, body: Vec<Stmt>) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `body`: `Vec<Stmt>`

*Defined at line 22341*

---

## Lowerer_lower_match

```fides
pub fn Lowerer_lower_match(self: & mut Lowerer, expr: Expr, arms: Vec<MatchArm>) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `expr`: `Expr`
- `arms`: `Vec<MatchArm>`

*Defined at line 22363*

---

## Lowerer_lower_compound_assign

```fides
pub fn Lowerer_lower_compound_assign(self: & mut Lowerer, op: AssignOp, current: i64, rhs: i64) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `op`: `AssignOp`
- `current`: `i64`
- `rhs`: `i64`

**Returns:** `i64`

*Defined at line 22626*

---

## Lowerer_type_struct_name

```fides
pub fn Lowerer_type_struct_name(self: & Lowerer, ty: Type) -> OptStringLower {
```

**Parameters:**

- `self`: `& Lowerer`
- `ty`: `Type`

**Returns:** `OptStringLower`

*Defined at line 22685*

---

## Lowerer_has_drop_impl

```fides
pub fn Lowerer_has_drop_impl(self: & Lowerer, struct_name: str) -> bool {
```

**Parameters:**

- `self`: `& Lowerer`
- `struct_name`: `str`

**Returns:** `bool`

*Defined at line 22723*

---

## Lowerer_emit_drops

```fides
pub fn Lowerer_emit_drops(self: & mut Lowerer) {
```

**Parameters:**

- `self`: `& mut Lowerer`

*Defined at line 22743*

---

## Lowerer_mark_sensitive

F88: Emit compiler-inserted memset(0) calls for all @zeroize locals.
Called before drops and returns. Wipes secrets from memory at scope exit.
Emits in reverse declaration order (LIFO — last declared wiped first).
J8: a @zeroize Vec keeps its contents in a heap buffer that growth
replaces — register the local with the runtime's sensitive-Vec table so
vec_sz_reserve wipes the old buffer before freeing it. The table key is
the alloca (the Vec header's address), which is what the Vec primitives
receive.

```fides
pub fn Lowerer_mark_sensitive(self: & mut Lowerer, alloca: i64, ir_ty: IrType) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `alloca`: `i64`
- `ir_ty`: `IrType`

*Defined at line 22769*

---

## Lowerer_emit_zeroize

```fides
pub fn Lowerer_emit_zeroize(self: & mut Lowerer) {
```

**Parameters:**

- `self`: `& mut Lowerer`

*Defined at line 22781*

---

## Lowerer_infer_binop_type

```fides
pub fn Lowerer_infer_binop_type(self: & Lowerer, op: BinOp, left: Expr, right: Expr) -> IrType {
```

**Parameters:**

- `self`: `& Lowerer`
- `op`: `BinOp`
- `left`: `Expr`
- `right`: `Expr`

**Returns:** `IrType`

*Defined at line 22833*

---

## Lowerer_infer_unary_type

```fides
pub fn Lowerer_infer_unary_type(self: & Lowerer, op: UnaryOp, expr: Expr) -> IrType {
```

**Parameters:**

- `self`: `& Lowerer`
- `op`: `UnaryOp`
- `expr`: `Expr`

**Returns:** `IrType`

*Defined at line 23347*

---

## Lowerer_infer_expr_type

```fides
pub fn Lowerer_infer_expr_type(self: & Lowerer, expr: Expr) -> IrType {
```

**Parameters:**

- `self`: `& Lowerer`
- `expr`: `Expr`

**Returns:** `IrType`

*Defined at line 23378*

---

## Lowerer_coalesce_ir_type

The type of `opt ?? default` (known-bugs 2g.54): the Option's payload
type when it is a string or a float, else I64 (the payload word, as
before). An Option whose type is unknown here takes the type of a
string or float default.

```fides
pub fn Lowerer_coalesce_ir_type(self: & Lowerer, left: Expr, right: Expr) -> IrType {
```

**Parameters:**

- `self`: `& Lowerer`
- `left`: `Expr`
- `right`: `Expr`

**Returns:** `IrType`

*Defined at line 23804*

---

## Lowerer_lookup_func_ret_ty_ir

```fides
pub fn Lowerer_lookup_func_ret_ty_ir(self: & Lowerer, name: str) -> IrType {
```

**Parameters:**

- `self`: `& Lowerer`
- `name`: `str`

**Returns:** `IrType`

*Defined at line 23938*

---

## Lowerer_coerce_return

A function declared `-> f64`/`-> f32` may return an integer expression
(`return 1;`): convert it numerically, as `val x: f64 = 1;` does.

```fides
pub fn Lowerer_coerce_return(self: & mut Lowerer, val: i64) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `val`: `i64`

**Returns:** `i64`

*Defined at line 23960*

---

## Lowerer_int_to_float_value

`val` as a value of the float type `want` when it is an integer: the
conversion written out (reference §3.3: an integer becomes a float). It is
made where a value enters a float slot or parameter: a declaration, an
assignment, a call. The C backend converted it by its own assignment and
the interpreter did not; the native backend read the integer's bits as the
float (`val f: f64 = n` printed 0.000..., `half(5)` got 0.0).

```fides
pub fn Lowerer_int_to_float_value(self: & mut Lowerer, val: i64, want: IrType) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `val`: `i64`
- `want`: `IrType`

**Returns:** `i64`

*Defined at line 23975*

---

## Lowerer_lower_cast

```fides
pub fn Lowerer_lower_cast(self: & mut Lowerer, val: i64, src_ty: IrType, dst_ty: IrType) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `val`: `i64`
- `src_ty`: `IrType`
- `dst_ty`: `IrType`

**Returns:** `i64`

*Defined at line 24093*

---

## Lowerer_infer_match_type

The type of a value-form `match`: the join of its arms' values (their
last expressions, lw_join_value_ty). It took the first expression
statement of any arm, so `change(&mut b); b.len()` gave the match the
void call's type and the value was lost (0), and an arm of a narrower
integer type truncated the others.

```fides
pub fn Lowerer_infer_match_type(self: & Lowerer, arms: Vec<MatchArm>) -> IrType {
```

**Parameters:**

- `self`: `& Lowerer`
- `arms`: `Vec<MatchArm>`

**Returns:** `IrType`

*Defined at line 24169*

---

## Lowerer_emit_sret_copy

```fides
pub fn Lowerer_emit_sret_copy(self: & mut Lowerer, src_ptr: i64) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `src_ptr`: `i64`

*Defined at line 24246*

---

## Lowerer_emit_struct_copy

```fides
pub fn Lowerer_emit_struct_copy(self: & mut Lowerer, src_ptr: i64, dst_ptr: i64, struct_name: str) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `src_ptr`: `i64`
- `dst_ptr`: `i64`
- `struct_name`: `str`

*Defined at line 24282*

---

## f64_to_bits

```fides
pub fn f64_to_bits(f: f64) -> i64 {
```

**Parameters:**

- `f`: `f64`

**Returns:** `i64`

*Defined at line 24337*

---

## f32_to_bits

```fides
pub fn f32_to_bits(f: f64) -> i64 {
```

**Parameters:**

- `f`: `f64`

**Returns:** `i64`

*Defined at line 24341*

---

## Lowerer_emit_defers

```fides
pub fn Lowerer_emit_defers(self: & mut Lowerer) {
```

**Parameters:**

- `self`: `& mut Lowerer`

*Defined at line 24345*

---

## Lowerer_emit_persist_saves

Emit PersistSave instructions for all @persist variables.
Called before every Return statement and at implicit function end.

```fides
pub fn Lowerer_emit_persist_saves(self: & mut Lowerer) {
```

**Parameters:**

- `self`: `& mut Lowerer`

*Defined at line 24633*

---

## ValueIdString

```fides
pub struct ValueIdString {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `name` | `str` |

*Defined at line 24655*

---

## lw_ValueIdString_new

```fides
pub fn lw_ValueIdString_new(id: i64, name: str) -> ValueIdString {
```

**Parameters:**

- `id`: `i64`
- `name`: `str`

**Returns:** `ValueIdString`

*Defined at line 24660*

---

## str_parse_u64

```fides
pub fn str_parse_u64(s: str) -> OptU64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `OptU64`

*Defined at line 24668*

---

## Lowerer_infer_vec_elem_type

Infer Vec<T> element type from the let hint.
Returns IrType::I64 if no hint is available.

```fides
pub fn Lowerer_infer_vec_elem_type(self: & Lowerer) -> IrType {
```

**Parameters:**

- `self`: `& Lowerer`

**Returns:** `IrType`

*Defined at line 24693*

---

## Lowerer_infer_map_types

Infer HashMap<K,V> key and value types from the let hint.
Returns (Opaque("String"), I64) if no hint is available.

```fides
pub fn Lowerer_infer_map_types(self: & Lowerer) -> MapTypes {
```

**Parameters:**

- `self`: `& Lowerer`

**Returns:** `MapTypes`

*Defined at line 24719*

---

## MapTypes

```fides
pub struct MapTypes {
```

**Fields:**

| Name | Type |
|------|------|
| `key_ty` | `IrType` |
| `val_ty` | `IrType` |

*Defined at line 24749*

---

## MapTypes_new

```fides
pub fn MapTypes_new(key_ty: IrType, val_ty: IrType) -> MapTypes {
```

**Parameters:**

- `key_ty`: `IrType`
- `val_ty`: `IrType`

**Returns:** `MapTypes`

*Defined at line 24754*

---

## Lowerer_infer_closure_ret_ty

Infer a closure's return type from its body (last expression or return stmt).
Returns Type::I64 if no return expression is found.

```fides
pub fn Lowerer_infer_closure_ret_ty(self: & Lowerer, body: Vec<Stmt>) -> Type {
```

**Parameters:**

- `self`: `& Lowerer`
- `body`: `Vec<Stmt>`

**Returns:** `Type`

*Defined at line 24762*

---

## Lowerer_infer_expr_ast_type

Infer the AST type of an expression (for closure return type inference).
This is a simplified version - returns Type::I64 for most expressions.

```fides
pub fn Lowerer_infer_expr_ast_type(self: & Lowerer, expr: Expr) -> Type {
```

**Parameters:**

- `self`: `& Lowerer`
- `expr`: `Expr`

**Returns:** `Type`

*Defined at line 24790*

---

## vec_eq_fn_for

The core function that compares two values of the Vec type `t` by
content: Vec_eq_words for numbers and bools, Vec_eq_strs for strings;
"" for anything else (compared as before).

```fides
pub fn vec_eq_fn_for(t: Type) -> str {
```

**Parameters:**

- `t`: `Type`

**Returns:** `str`

*Defined at line 25720*

---

## ir_type_to_ast_type

Convert an IrType back to an AST Type (for capture type recovery).

```fides
pub fn ir_type_to_ast_type(ty: IrType) -> Type {
```

**Parameters:**

- `ty`: `IrType`

**Returns:** `Type`

*Defined at line 25920*

---

## Lowerer_lower_swizzle

Lower a swizzle expression (e.g. v.xyx) by loading components and building a result vec.

```fides
pub fn Lowerer_lower_swizzle(self: & mut Lowerer, vec_name: str, struct_ptr: i64, swizzle: str) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `vec_name`: `str`
- `struct_ptr`: `i64`
- `swizzle`: `str`

**Returns:** `i64`

*Defined at line 25990*

---

## Lowerer_struct_operator

CL10: the method `a op b` calls when `a` is a struct other than vecN that
has it (`Point_add`), or "".

```fides
pub fn Lowerer_struct_operator(self: & Lowerer, op: BinOp, left: Expr) -> str {
```

**Parameters:**

- `self`: `& Lowerer`
- `op`: `BinOp`
- `left`: `Expr`

**Returns:** `str`

*Defined at line 26036*

---

## Lowerer_struct_operator_ty

Lowerer_struct_operator for a left operand whose type is known. A unit
(@dimension) is left to Lowerer_binop_plan: its method serves only an
operand it takes.

```fides
pub fn Lowerer_struct_operator_ty(self: & Lowerer, op: BinOp, left_ty: IrType) -> str {
```

**Parameters:**

- `self`: `& Lowerer`
- `op`: `BinOp`
- `left_ty`: `IrType`

**Returns:** `str`

*Defined at line 26046*

---

## Lowerer_struct_dim

The packed dimension of struct `name`, 0 when it is not a unit.

```fides
pub fn Lowerer_struct_dim(self: & Lowerer, name: str) -> i64 {
```

**Parameters:**

- `self`: `& Lowerer`
- `name`: `str`

**Returns:** `i64`

*Defined at line 26069*

---

## Lowerer_binop_plan

How `a op b` lowers: 0 as a plain operator (or vecN's own path), 1 the
left struct's method (`a.add(b)`), 2 the right struct's r-method
(`b.rmul(a)`), 3 the dimension rule on units (Lowerer_lower_dim_operator).

```fides
pub fn Lowerer_binop_plan(self: & Lowerer, op: BinOp, left_ty: IrType, right_ty: IrType) -> i64 {
```

**Parameters:**

- `self`: `& Lowerer`
- `op`: `BinOp`
- `left_ty`: `IrType`
- `right_ty`: `IrType`

**Returns:** `i64`

*Defined at line 26144*

---

## Lowerer_binop_plan_exprs

Lowerer_binop_plan with the right operand's type inferred only when it
can matter (a literal on the right of a non-struct is never a struct).

```fides
pub fn Lowerer_binop_plan_exprs(self: & Lowerer, op: BinOp, left_ty: IrType, right: Expr) -> i64 {
```

**Parameters:**

- `self`: `& Lowerer`
- `op`: `BinOp`
- `left_ty`: `IrType`
- `right`: `Expr`

**Returns:** `i64`

*Defined at line 26185*

---

## Lowerer_dim_result_ty

The IR type of a dimension-rule result.

```fides
pub fn Lowerer_dim_result_ty(self: & Lowerer, op: BinOp, left_ty: IrType, right_ty: IrType) -> IrType {
```

**Parameters:**

- `self`: `& Lowerer`
- `op`: `BinOp`
- `left_ty`: `IrType`
- `right_ty`: `IrType`

**Returns:** `IrType`

*Defined at line 26256*

---

## Lowerer_lower_dim_operator

`a op b` by the dimension rule: the f64 of each unit operand combined,
in a struct literal of the result unit (`Joules { value: f.value *
d.value }`), or the bare f64 when the dimensions cancel. Operands are
evaluated left to right, once each.

```fides
pub fn Lowerer_lower_dim_operator(self: & mut Lowerer, op: BinOp, left: Expr, right: Expr, left_ty: IrType, right_ty: IrType) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `op`: `BinOp`
- `left`: `Expr`
- `right`: `Expr`
- `left_ty`: `IrType`
- `right_ty`: `IrType`

**Returns:** `i64`

*Defined at line 26296*

---

## Lowerer_lower_vec_method_call

Operator overloading: `a op b` on vecN lowers to vecN_<method>(a, b).

```fides
pub fn Lowerer_lower_vec_method_call(self: & mut Lowerer, vec_name: str, method: str, receiver: Expr, arg: Expr) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `vec_name`: `str`
- `method`: `str`
- `receiver`: `Expr`
- `arg`: `Expr`

**Returns:** `i64`

*Defined at line 26483*

---

## Lowerer_lower_vec_sret_call

Emit an SRET-style call for vec-returning methods.
Allocates a struct slot, calls the function with sret_ptr as first arg, returns sret_ptr.

```fides
pub fn Lowerer_lower_vec_sret_call(self: & mut Lowerer, func_name: str, ret_ty: IrType, recv_val: i64, arg_val: i64) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `func_name`: `str`
- `ret_ty`: `IrType`
- `recv_val`: `i64`
- `arg_val`: `i64`

**Returns:** `i64`

*Defined at line 26491*

---

## OwnTemp

A temporary the current statement owns; `claimed` once it was kept.

```fides
pub struct OwnTemp {
```

**Fields:**

| Name | Type |
|------|------|
| `val` | `i64` |
| `ty` | `Type` |
| `claimed` | `bool` |

*Defined at line 26547*

---

## OwnType

A struct or enum type with generated drop/dup functions
(__own_drop_<index>, __own_dup_<index>); its element type id is
index + 16.

```fides
pub struct OwnType {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `ty` | `Type` |

*Defined at line 26556*

---

## Lowerer_rc_kind

How values of type `t` are owned: RC_STR, RC_VEC, RC_MAP (a handle),
RC_MEM (a struct, enum or generic instance holding one), RC_NONE.

```fides
pub fn Lowerer_rc_kind(self: & mut Lowerer, t0: Type) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `t0`: `Type`

**Returns:** `i64`

*Defined at line 26891*

---

## Lowerer_rc_type_index

The index of struct-like type `t` among those with generated functions.

```fides
pub fn Lowerer_rc_type_index(self: & mut Lowerer, t0: Type) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `t0`: `Type`

**Returns:** `i64`

*Defined at line 26962*

---

## Lowerer_rc_tid

The element type id of values of type `t` kept in a Vec or a map
(core's P_TID_*): 1 str, 2 Vec, 3 HashMap, 16 + index for a struct or
enum (held by pointer to a heap copy), 0 for plain values.

```fides
pub fn Lowerer_rc_tid(self: & mut Lowerer, t: Type) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `t`: `Type`

**Returns:** `i64`

*Defined at line 26987*

---

## Lowerer_rc_dup

One more owner of `v`: the value to keep (a new handle for a Vec or a
map; for a struct, a copy whose fields have one more owner each).

```fides
pub fn Lowerer_rc_dup(self: & mut Lowerer, v: i64, t: Type) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `v`: `i64`
- `t`: `Type`

**Returns:** `i64`

*Defined at line 27078*

---

## Lowerer_rc_drop

Drop one owner of `v` (for a struct, of each of its fields).

```fides
pub fn Lowerer_rc_drop(self: & mut Lowerer, v: i64, t: Type) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `v`: `i64`
- `t`: `Type`

*Defined at line 27145*

---

## Lowerer_rc_temp

`v` is a temporary of type `t` that the current statement owns.

```fides
pub fn Lowerer_rc_temp(self: & mut Lowerer, v: i64, t: Type) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `v`: `i64`
- `t`: `Type`

*Defined at line 27181*

---

## Lowerer_rc_claim

Hand temporary `v` over to whoever keeps it: true when it was one.

```fides
pub fn Lowerer_rc_claim(self: & mut Lowerer, v: i64) -> bool {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `v`: `i64`

**Returns:** `bool`

*Defined at line 27194*

---

## Lowerer_rc_flush

The end of the statement (or branch) whose temporaries start at `mark`.

```fides
pub fn Lowerer_rc_flush(self: & mut Lowerer, mark: i64) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `mark`: `i64`

*Defined at line 27235*

---

## Lowerer_rc_own_value

`v`, kept somewhere that will drop it as a `t`: a temporary is handed
over, anything else gets one more owner.

```fides
pub fn Lowerer_rc_own_value(self: & mut Lowerer, v: i64, t: Type) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `v`: `i64`
- `t`: `Type`

**Returns:** `i64`

*Defined at line 27248*

---

## Lowerer_rc_owned_any

Lower `expr` for a place that keeps it, as whatever type it has.

```fides
pub fn Lowerer_rc_owned_any(self: & mut Lowerer, expr: Expr) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `expr`: `Expr`

**Returns:** `i64`

*Defined at line 27271*

---

## Lowerer_lower_owned

Lower `expr` for a place that keeps it as a `t` (proposal 0004).

```fides
pub fn Lowerer_lower_owned(self: & mut Lowerer, expr: Expr, t: Type) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `expr`: `Expr`
- `t`: `Type`

**Returns:** `i64`

*Defined at line 27280*

---

## Lowerer_rc_new_slot

A slot for a variable that owns a `t`: allocated and zeroed in the entry
block, so a drop on a path where the variable was never set does nothing.

```fides
pub fn Lowerer_rc_new_slot(self: & mut Lowerer, ir_ty: IrType, t: Type) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `ir_ty`: `IrType`
- `t`: `Type`

**Returns:** `i64`

*Defined at line 27321*

---

## Lowerer_rc_add_slot

`slot` owns its value until the current scope ends.

```fides
pub fn Lowerer_rc_add_slot(self: & mut Lowerer, slot: i64, t: Type) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `slot`: `i64`
- `t`: `Type`

*Defined at line 27329*

---

## Lowerer_rc_owns_slot

Whether `slot` owns its value (an owned variable of an open scope).

```fides
pub fn Lowerer_rc_owns_slot(self: & Lowerer, slot: i64) -> bool {
```

**Parameters:**

- `self`: `& Lowerer`
- `slot`: `i64`

**Returns:** `bool`

*Defined at line 27596*

---

## Lowerer_rc_scope_open

```fides
pub fn Lowerer_rc_scope_open(self: & mut Lowerer) {
```

**Parameters:**

- `self`: `& mut Lowerer`

*Defined at line 27637*

---

## Lowerer_rc_scope_close

The end of a block: its variables are dropped.

```fides
pub fn Lowerer_rc_scope_close(self: & mut Lowerer) {
```

**Parameters:**

- `self`: `& mut Lowerer`

*Defined at line 27642*

---

## Lowerer_rc_drop_all

Leaving the function (a return, the `?` error path): every temporary
and owned variable is dropped.

```fides
pub fn Lowerer_rc_drop_all(self: & mut Lowerer) {
```

**Parameters:**

- `self`: `& mut Lowerer`

*Defined at line 27657*

---

## Lowerer_rc_loop_open

A loop starts: `break` and `continue` drop what its body made.

```fides
pub fn Lowerer_rc_loop_open(self: & mut Lowerer) {
```

**Parameters:**

- `self`: `& mut Lowerer`

*Defined at line 27666*

---

## Lowerer_rc_loop_close

```fides
pub fn Lowerer_rc_loop_close(self: & mut Lowerer) {
```

**Parameters:**

- `self`: `& mut Lowerer`

*Defined at line 27672*

---

## Lowerer_rc_loop_exit

Before `break`/`continue`: drop the innermost loop body's temporaries
and variables.

```fides
pub fn Lowerer_rc_loop_exit(self: & mut Lowerer) {
```

**Parameters:**

- `self`: `& mut Lowerer`

*Defined at line 27686*

---

## Lowerer_rc_finish_func

Put the owned slots' allocations and zeroing at the start of the entry
block of the function just lowered.

```fides
pub fn Lowerer_rc_finish_func(self: & mut Lowerer) {
```

**Parameters:**

- `self`: `& mut Lowerer`

*Defined at line 27696*

---

## Lowerer_rc_note_sret

After a call that returns a struct through `sret`: the caller owns the
struct written there.

```fides
pub fn Lowerer_rc_note_sret(self: & mut Lowerer, sret: i64, name: str) {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `sret`: `i64`
- `name`: `str`

*Defined at line 27804*

---

## Lowerer_rc_gen_types

After every function is lowered: the drop/dup function of each struct
and enum type used (the list grows while it is built: a type's fields
register theirs), then the element dispatchers.

```fides
pub fn Lowerer_rc_gen_types(self: & mut Lowerer) {
```

**Parameters:**

- `self`: `& mut Lowerer`

*Defined at line 28244*

---

## lw_own_type_keys

The own types' keys of the last lowering, by index (B6).

```fides
pub fn lw_own_type_keys() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 28280*

---

## Lowerer_rc_new_coll

An empty collection of AST type `t` that owns its elements by their
type: Vec_new_typed(code, tid) or HashMap_new_typed(keys, vtid); -1 when
`t` says nothing about them (the caller makes it the old way).

```fides
pub fn Lowerer_rc_new_coll(self: & mut Lowerer, t: Type) -> i64 {
```

**Parameters:**

- `self`: `& mut Lowerer`
- `t`: `Type`

**Returns:** `i64`

*Defined at line 28504*

---

## lw_rc_stack_vecs_fn

LF13 in one function: true when it changed. A `h = Vec_new_typed(...)`
(or Vec_new, Vec_new_sized, and Vec_filled_typed when `filled_ok`: core
has Vec_init_filled) qualifies when h is only stored into one slot S,
every other store to S is a zero, S is used only by those loads and
stores, and every value loaded from S is only argument 1 of core's Vec
functions. `vbuf_ok`: core has Vec_init_filled_at / Vec_drop_stackbuf, so
a small Vec_filled_typed whose buffer never grows or is shared puts the
buffer on the stack too (lw_rc_vbuf_fits).

```fides
pub fn lw_rc_stack_vecs_fn(f: & mut IrFunction, safe: i64, filled_ok: bool, vbuf_ok: bool) -> bool {
```

**Parameters:**

- `f`: `& mut IrFunction`
- `safe`: `i64`
- `filled_ok`: `bool`
- `vbuf_ok`: `bool`

**Returns:** `bool`

*Defined at line 28959*

---

## lw_rc_stack_strs_fn

LF13 strings in one function: true when it changed. A
`h = p_str_concat(a, b)` qualifies when h is only stored into one slot
S, every other store to S is a literal (or null), S is only loaded and
stored, and every value loaded from S is only printed, an argument of
lw_rc_str_reader's calls, or cast and dropped (Str_drop).

```fides
pub fn lw_rc_stack_strs_fn(f: & mut IrFunction) -> bool {
```

**Parameters:**

- `f`: `& mut IrFunction`

**Returns:** `bool`

*Defined at line 29524*

---

## IrUnit

One source module's part of the lowered program.

```fides
pub struct IrUnit {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `module` | `IrModule` |
| `positions` | `Vec<i64>` |
| `exports` | `Vec<str>` |
| `imports` | `Vec<str>` |
| `key` | `i64` |
| `layout_key` | `i64` |

*Defined at line 33217*

---

## IrLinkTable

The units of a lowered program and what links them.

```fides
pub struct IrLinkTable {
```

**Fields:**

| Name | Type |
|------|------|
| `units` | `Vec<IrUnit>` |
| `count` | `i64` |
| `layout_hash` | `i64` |
| `owner` | `i64` |
| `errors` | `Vec<str>` |

*Defined at line 33236*

---

## ir_units_split

Cut `module` into one unit per source module; `unit_of` names the module
of each function, in order ("main" past its end).

```fides
pub fn ir_units_split(module: & IrModule, unit_of: Vec<str>) -> IrLinkTable {
```

**Parameters:**

- `module`: `& IrModule`
- `unit_of`: `Vec<str>`

**Returns:** `IrLinkTable`

*Defined at line 33466*

---

## ir_units_check

What is wrong with `t`: the split's errors, an import no unit defines,
a unit whose layout table differs from the others', a function lost.

```fides
pub fn ir_units_check(t: & IrLinkTable) -> Vec<str> {
```

**Parameters:**

- `t`: `& IrLinkTable`

**Returns:** `Vec<str>`

*Defined at line 33545*

---

## ir_units_link

The whole module again: every unit's functions at their positions, with
the first unit's tables.

```fides
pub fn ir_units_link(t: & IrLinkTable) -> IrModule {
```

**Parameters:**

- `t`: `& IrLinkTable`

**Returns:** `IrModule`

*Defined at line 33577*

---

## ir_units_table_text

The link table as text: one line per unit (name, key, functions,
imports), then the layout hash.

```fides
pub fn ir_units_table_text(t: & IrLinkTable) -> str {
```

**Parameters:**

- `t`: `& IrLinkTable`

**Returns:** `str`

*Defined at line 33620*

---

## ir_units_roundtrip

FID_IR_UNITS: split `module` (`unit_of` as for ir_units_split), report
what ir_units_check finds, write the units and the table to `dir` when
it is not "1", and give the linked module back.

```fides
pub fn ir_units_roundtrip(module: IrModule, unit_of: Vec<str>, dir: str) -> IrModule {
```

**Parameters:**

- `module`: `IrModule`
- `unit_of`: `Vec<str>`
- `dir`: `str`

**Returns:** `IrModule`

*Defined at line 33649*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
