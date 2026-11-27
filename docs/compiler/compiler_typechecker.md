[All modules](index.md)

# compiler::typechecker -- Fides Documentation

## Contents

- [StringType](#StringType) (struct)
- [StringType_new](#StringType_new) (function)
- [StringVecType](#StringVecType) (struct)
- [StringVecType_new](#StringVecType_new) (function)
- [StringVecString](#StringVecString) (struct)
- [StringVecString_new](#StringVecString_new) (function)
- [StringBool](#StringBool) (struct)
- [StringBool_new](#StringBool_new) (function)
- [StringString](#StringString) (struct)
- [StringString_new](#StringString_new) (function)
- [FieldVisKey](#FieldVisKey) (struct)
- [FieldVisKey_new](#FieldVisKey_new) (function)
- [FieldVisEntry](#FieldVisEntry) (struct)
- [FieldVisEntry_new](#FieldVisEntry_new) (function)
- [TraitImplKey](#TraitImplKey) (struct)
- [TraitImplKey_new](#TraitImplKey_new) (function)
- [MethodInfo](#MethodInfo) (struct)
- [MethodInfo_new](#MethodInfo_new) (function)
- [MethodInfo_with_tps](#MethodInfo_with_tps) (function)
- [MethodInfo_with_params](#MethodInfo_with_params) (function)
- [TraitMethodInfo](#TraitMethodInfo) (struct)
- [TraitMethodInfo_new](#TraitMethodInfo_new) (function)
- [StructField](#StructField) (struct)
- [StructField_new](#StructField_new) (function)
- [EnumVariantEntry](#EnumVariantEntry) (struct)
- [EnumVariantEntry_new](#EnumVariantEntry_new) (function)
- [FuncSig](#FuncSig) (struct)
- [FuncSig_new](#FuncSig_new) (function)
- [FuncSig_new_with_min](#FuncSig_new_with_min) (function)
- [MethodMapEntry](#MethodMapEntry) (struct)
- [MethodMapEntry_new](#MethodMapEntry_new) (function)
- [TraitMethodMapEntry](#TraitMethodMapEntry) (struct)
- [TraitMethodMapEntry_new](#TraitMethodMapEntry_new) (function)
- [ScopeEntry](#ScopeEntry) (struct)
- [ScopeEntry_new](#ScopeEntry_new) (function)
- [LinearVar](#LinearVar) (struct)
- [LinearVar_new](#LinearVar_new) (function)
- [RegionBudget](#RegionBudget) (struct)
- [RegionBudget_new](#RegionBudget_new) (function)
- [CapturedVar](#CapturedVar) (struct)
- [CapturedVar_new](#CapturedVar_new) (function)
- [CallSite](#CallSite) (struct)
- [CallSite_new](#CallSite_new) (function)
- [DivSite](#DivSite) (struct)
- [DivSite_new](#DivSite_new) (function)
- [OptString](#OptString) (enum)
- [OptType](#OptType) (enum)
- [OptStmtList](#OptStmtList) (enum)
- [tc_str_vec_contains](#tc_str_vec_contains) (function)
- [tc_dedupe_strs](#tc_dedupe_strs) (function)
- [str_vec_contains_ref](#str_vec_contains_ref) (function)
- [OptVecType](#OptVecType) (enum)
- [OptVecStr](#OptVecStr) (enum)
- [TypeChecker_lookup_func_cache](#TypeChecker_lookup_func_cache) (function)
- [TypeChecker_lookup_const_cache](#TypeChecker_lookup_const_cache) (function)
- [TypeChecker_lookup_global_cache](#TypeChecker_lookup_global_cache) (function)
- [TypeChecker_lookup_alias_cache](#TypeChecker_lookup_alias_cache) (function)
- [TypeChecker_lookup_struct_cache](#TypeChecker_lookup_struct_cache) (function)
- [StringMethodMap](#StringMethodMap) (struct)
- [StringMethodMap_new](#StringMethodMap_new) (function)
- [OptMethodMap](#OptMethodMap) (enum)
- [OptFuncSig](#OptFuncSig) (enum)
- [OptVecStructField](#OptVecStructField) (enum)
- [OptMethodInfo](#OptMethodInfo) (enum)
- [StringTraitMethodMap](#StringTraitMethodMap) (struct)
- [StringTraitMethodMap_new](#StringTraitMethodMap_new) (function)
- [OptTraitMethodMap](#OptTraitMethodMap) (enum)
- [OptTraitMethodInfo](#OptTraitMethodInfo) (enum)
- [TypeChecker](#TypeChecker) (struct)
- [StringStructFields](#StringStructFields) (struct)
- [StringStructFields_new](#StringStructFields_new) (function)
- [StringEnumVariants](#StringEnumVariants) (struct)
- [StringEnumVariants_new](#StringEnumVariants_new) (function)
- [StringVecStrTc](#StringVecStrTc) (struct)
- [StringVecStrTc_new](#StringVecStrTc_new) (function)
- [StringI64](#StringI64) (struct)
- [StringI64_new](#StringI64_new) (function)
- [TraitImplAssocEntry](#TraitImplAssocEntry) (struct)
- [TraitImplAssocEntry_new](#TraitImplAssocEntry_new) (function)
- [StringFuncSig](#StringFuncSig) (struct)
- [StringFuncSig_new](#StringFuncSig_new) (function)
- [TypeChecker_free_maps](#TypeChecker_free_maps) (function)
- [TypeChecker](#TypeChecker) (impl)
- [tc_note_module_file](#tc_note_module_file) (function)
- [tc_note_program_file](#tc_note_program_file) (function)
- [tc_operator_method](#tc_operator_method) (function)
- [tc_struct_dim](#tc_struct_dim) (function)
- [TypeChecker_register_program](#TypeChecker_register_program) (function)
- [TypeChecker_register_trait_impls](#TypeChecker_register_trait_impls) (function)
- [TypeChecker_verify_trait_impls](#TypeChecker_verify_trait_impls) (function)
- [OptTraitDef](#OptTraitDef) (enum)
- [TypeChecker_register_generated_decls](#TypeChecker_register_generated_decls) (function)
- [TypeChecker_build_caches](#TypeChecker_build_caches) (function)
- [TypeChecker_check_annotation_validity](#TypeChecker_check_annotation_validity) (function)
- [TypeChecker_check_bodies](#TypeChecker_check_bodies) (function)
- [tc_skips](#tc_skips) (function)
- [tc_core_skip_set](#tc_core_skip_set) (function)
- [TypeChecker_effects_of](#TypeChecker_effects_of) (function)
- [TypeChecker_knows_fn](#TypeChecker_knows_fn) (function)
- [TypeChecker_compute_func_effects](#TypeChecker_compute_func_effects) (function)
- [tc_body_index](#tc_body_index) (function)
- [tc_find_body_indexed](#tc_find_body_indexed) (function)
- [tc_find_params_indexed](#tc_find_params_indexed) (function)
- [TypeChecker_find_body](#TypeChecker_find_body) (function)
- [TypeChecker_verify_declared_effects](#TypeChecker_verify_declared_effects) (function)
- [TypeChecker_verify_deterministic](#TypeChecker_verify_deterministic) (function)
- [TypeChecker_verify_conserve](#TypeChecker_verify_conserve) (function)
- [TypeChecker_verify_gas](#TypeChecker_verify_gas) (function)
- [TypeChecker_verify_safety_annotations](#TypeChecker_verify_safety_annotations) (function)
- [expand_safety_primitives](#expand_safety_primitives) (function)
- [has_primitive](#has_primitive) (function)
- [safety_domain_label](#safety_domain_label) (function)
- [is_blocking_func](#is_blocking_func) (function)
- [is_os_dependent_func](#is_os_dependent_func) (function)
- [safety_fn_params](#safety_fn_params) (function)
- [check_safety_body](#check_safety_body) (function)
- [tc_vec_contains_str](#tc_vec_contains_str) (function)
- [scan_stmts_for_calls](#scan_stmts_for_calls) (function)
- [scan_stmt_for_calls](#scan_stmt_for_calls) (function)
- [scan_expr_for_calls](#scan_expr_for_calls) (function)
- [scan_for_panic_risks](#scan_for_panic_risks) (function)
- [scan_for_division](#scan_for_division) (function)
- [scan_stmt_for_division](#scan_stmt_for_division) (function)
- [scan_stmts_for_division](#scan_stmts_for_division) (function)
- [scan_expr_for_division](#scan_expr_for_division) (function)
- [scan_for_while_loops](#scan_for_while_loops) (function)
- [scan_stmt_for_while](#scan_stmt_for_while) (function)
- [scan_stmts_for_while](#scan_stmts_for_while) (function)
- [scan_expr_for_while](#scan_expr_for_while) (function)
- [TypeChecker_verify_constant_time](#TypeChecker_verify_constant_time) (function)
- [TypeChecker_verify_realtime](#TypeChecker_verify_realtime) (function)
- [TypeChecker_verify_differentiable](#TypeChecker_verify_differentiable) (function)
- [TypeChecker_verify_quantized](#TypeChecker_verify_quantized) (function)
- [TypeChecker_verify_checkpoint](#TypeChecker_verify_checkpoint) (function)
- [TypeChecker_verify_dp](#TypeChecker_verify_dp) (function)
- [TypeChecker_verify_shard](#TypeChecker_verify_shard) (function)
- [TypeChecker_verify_adapter](#TypeChecker_verify_adapter) (function)
- [TypeChecker_mentions_scoped](#TypeChecker_mentions_scoped) (function)
- [TypeChecker_verify_scoped](#TypeChecker_verify_scoped) (function)
- [TypeChecker_verify_tainted](#TypeChecker_verify_tainted) (function)
- [TfResult](#TfResult) (struct)
- [TfGlobal](#TfGlobal) (struct)
- [TfCtx](#TfCtx) (struct)
- [LoSum](#LoSum) (struct)
- [LoGlobal](#LoGlobal) (struct)
- [LoCtx](#LoCtx) (struct)
- [TypeChecker_verify_lock_order](#TypeChecker_verify_lock_order) (function)
- [HmGraph](#HmGraph) (struct)
- [TypeChecker_verify_hermetic](#TypeChecker_verify_hermetic) (function)
- [PfFacts](#PfFacts) (struct)
- [PfReach](#PfReach) (struct)
- [TypeChecker_verify_redundant](#TypeChecker_verify_redundant) (function)
- [TypeChecker_verify_partitioned](#TypeChecker_verify_partitioned) (function)
- [TypeChecker_verify_serverless](#TypeChecker_verify_serverless) (function)
- [TypeChecker_verify_emit](#TypeChecker_verify_emit) (function)
- [TypeChecker_verify_contracts](#TypeChecker_verify_contracts) (function)
- [TypeChecker_verify_allocator_params](#TypeChecker_verify_allocator_params) (function)
- [TypeChecker_check_func](#TypeChecker_check_func) (function)
- [stmts_always_leave](#stmts_always_leave) (function)
- [stmts_have_break](#stmts_have_break) (function)
- [TypeChecker_push_scope](#TypeChecker_push_scope) (function)
- [TypeChecker_pop_scope](#TypeChecker_pop_scope) (function)
- [TypeChecker_declare](#TypeChecker_declare) (function)
- [TypeChecker_lookup](#TypeChecker_lookup) (function)
- [TypeChecker_resolve_type](#TypeChecker_resolve_type) (function)
- [TypeChecker_subst_type_params](#TypeChecker_subst_type_params) (function)
- [TypeChecker_resolve_self_type](#TypeChecker_resolve_self_type) (function)
- [TypeChecker_resolve_assoc_in_type](#TypeChecker_resolve_assoc_in_type) (function)
- [TypeChecker_resolve_struct_name](#TypeChecker_resolve_struct_name) (function)
- [TypeChecker_is_numeric](#TypeChecker_is_numeric) (function)
- [TypeChecker_is_integer](#TypeChecker_is_integer) (function)
- [TypeChecker_is_float](#TypeChecker_is_float) (function)
- [TypeChecker_is_typed_string](#TypeChecker_is_typed_string) (function)
- [tc_operand_type_str](#tc_operand_type_str) (function)
- [infer_int_type](#infer_int_type) (function)
- [int_rank](#int_rank) (function)
- [wider_type](#wider_type) (function)
- [can_implicitly_widen](#can_implicitly_widen) (function)
- [type_eq](#type_eq) (function)
- [TypeChecker_types_compatible](#TypeChecker_types_compatible) (function)
- [TypeChecker_type_contains_param](#TypeChecker_type_contains_param) (function)
- [TypeChecker_is_known_enum](#TypeChecker_is_known_enum) (function)
- [TypeChecker_type_size](#TypeChecker_type_size) (function)
- [tc_core_only_builtin](#tc_core_only_builtin) (function)
- [tc_rx_compile_ok](#tc_rx_compile_ok) (function)
- [tc_blocking_call_fix](#tc_blocking_call_fix) (function)
- [TypeChecker_lint_blocking_in_tasks](#TypeChecker_lint_blocking_in_tasks) (function)
- [TypeChecker_check_capability](#TypeChecker_check_capability) (function)
- [TypeChecker_check_visibility](#TypeChecker_check_visibility) (function)
- [TypeChecker_collect_capabilities](#TypeChecker_collect_capabilities) (function)
- [TypeChecker_is_linear_type](#TypeChecker_is_linear_type) (function)
- [tc_swizzle_result_type](#tc_swizzle_result_type) (function)
- [tc_is_vec_type](#tc_is_vec_type) (function)
- [TypeChecker_is_send_type](#TypeChecker_is_send_type) (function)
- [TypeChecker_is_sync_type](#TypeChecker_is_sync_type) (function)
- [TypeChecker_consume_var](#TypeChecker_consume_var) (function)
- [TypeChecker_consume_linear_in_expr](#TypeChecker_consume_linear_in_expr) (function)
- [TypeChecker_check_stmt](#TypeChecker_check_stmt) (function)
- [TypeChecker_collect_captured_vars](#TypeChecker_collect_captured_vars) (function)
- [TypeChecker_check_expr](#TypeChecker_check_expr) (function)
- [TypeChecker_check_expr_with_hint](#TypeChecker_check_expr_with_hint) (function)
- [TypeChecker_check_method_call](#TypeChecker_check_method_call) (function)
- [TypeChecker_bare_pattern_is_unit_variant](#TypeChecker_bare_pattern_is_unit_variant) (function)
- [TypeChecker_resolve_pattern_enum](#TypeChecker_resolve_pattern_enum) (function)
- [type_from_annotation](#type_from_annotation) (function)
- [is_runtime_layer](#is_runtime_layer) (function)
- [is_handle_int_pair](#is_handle_int_pair) (function)
- [TypeChecker_is_runtime_call](#TypeChecker_is_runtime_call) (function)
- [TypeChecker_arg_compatible](#TypeChecker_arg_compatible) (function)
- [TypeChecker_shares_repr](#TypeChecker_shares_repr) (function)
- [TypeChecker_check_return_type](#TypeChecker_check_return_type) (function)
- [TypeChecker_check_enum_constructor](#TypeChecker_check_enum_constructor) (function)
- [TypeChecker_check_block_value](#TypeChecker_check_block_value) (function)
- [TypeChecker_check_stmts_value](#TypeChecker_check_stmts_value) (function)
- [unbox_ty](#unbox_ty) (function)
- [is_ffi_pointer_pair](#is_ffi_pointer_pair) (function)
- [TypeChecker_enum_params_of](#TypeChecker_enum_params_of) (function)
- [type_mentions_any](#type_mentions_any) (function)
- [TypeChecker_is_enum_type_param](#TypeChecker_is_enum_type_param) (function)
- [TypeChecker_check_enum_pattern](#TypeChecker_check_enum_pattern) (function)
- [TypeChecker_pattern_binding_types](#TypeChecker_pattern_binding_types) (function)
- [TypeChecker_scan_stmts_for_effects](#TypeChecker_scan_stmts_for_effects) (function)
- [TypeChecker_scan_stmt_for_effects](#TypeChecker_scan_stmt_for_effects) (function)
- [TypeChecker_scan_expr_for_effects](#TypeChecker_scan_expr_for_effects) (function)
- [TypeChecker_lint_unused_functions](#TypeChecker_lint_unused_functions) (function)
- [TypeChecker_collect_called_funcs](#TypeChecker_collect_called_funcs) (function)
- [TypeChecker_collect_called_funcs_stmt](#TypeChecker_collect_called_funcs_stmt) (function)
- [TypeChecker_collect_called_funcs_expr](#TypeChecker_collect_called_funcs_expr) (function)
- [TypeChecker_lint_unused_imports](#TypeChecker_lint_unused_imports) (function)
- [scan_for_floats](#scan_for_floats) (function)
- [scan_stmt_for_floats](#scan_stmt_for_floats) (function)
- [scan_expr_for_floats](#scan_expr_for_floats) (function)

## StringType

```fides
pub struct StringType {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `val` | `Type` |

*Defined at line 42*

---

## StringType_new

```fides
pub fn StringType_new(key: str, val: Type) -> StringType {
```

**Parameters:**

- `key`: `str`
- `val`: `Type`

**Returns:** `StringType`

*Defined at line 47*

---

## StringVecType

```fides
pub struct StringVecType {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `val` | `Vec<Type>` |

*Defined at line 53*

---

## StringVecType_new

```fides
pub fn StringVecType_new(key: str, val: Vec<Type>) -> StringVecType {
```

**Parameters:**

- `key`: `str`
- `val`: `Vec<Type>`

**Returns:** `StringVecType`

*Defined at line 58*

---

## StringVecString

```fides
pub struct StringVecString {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `val` | `Vec<str>` |

*Defined at line 64*

---

## StringVecString_new

```fides
pub fn StringVecString_new(key: str, val: Vec<str>) -> StringVecString {
```

**Parameters:**

- `key`: `str`
- `val`: `Vec<str>`

**Returns:** `StringVecString`

*Defined at line 69*

---

## StringBool

```fides
pub struct StringBool {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `val` | `bool` |

*Defined at line 75*

---

## StringBool_new

```fides
pub fn StringBool_new(key: str, val: bool) -> StringBool {
```

**Parameters:**

- `key`: `str`
- `val`: `bool`

**Returns:** `StringBool`

*Defined at line 80*

---

## StringString

```fides
pub struct StringString {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `val` | `str` |

*Defined at line 86*

---

## StringString_new

```fides
pub fn StringString_new(key: str, val: str) -> StringString {
```

**Parameters:**

- `key`: `str`
- `val`: `str`

**Returns:** `StringString`

*Defined at line 91*

---

## FieldVisKey

```fides
pub struct FieldVisKey {
```

**Fields:**

| Name | Type |
|------|------|
| `struct_name` | `str` |
| `field_name` | `str` |

*Defined at line 98*

---

## FieldVisKey_new

```fides
pub fn FieldVisKey_new(struct_name: str, field_name: str) -> FieldVisKey {
```

**Parameters:**

- `struct_name`: `str`
- `field_name`: `str`

**Returns:** `FieldVisKey`

*Defined at line 103*

---

## FieldVisEntry

```fides
pub struct FieldVisEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `FieldVisKey` |
| `val` | `bool` |

*Defined at line 109*

---

## FieldVisEntry_new

```fides
pub fn FieldVisEntry_new(key: FieldVisKey, val: bool) -> FieldVisEntry {
```

**Parameters:**

- `key`: `FieldVisKey`
- `val`: `bool`

**Returns:** `FieldVisEntry`

*Defined at line 114*

---

## TraitImplKey

```fides
pub struct TraitImplKey {
```

**Fields:**

| Name | Type |
|------|------|
| `trait_name` | `str` |
| `type_name` | `str` |

*Defined at line 121*

---

## TraitImplKey_new

```fides
pub fn TraitImplKey_new(trait_name: str, type_name: str) -> TraitImplKey {
```

**Parameters:**

- `trait_name`: `str`
- `type_name`: `str`

**Returns:** `TraitImplKey`

*Defined at line 126*

---

## MethodInfo

```fides
pub struct MethodInfo {
```

**Fields:**

| Name | Type |
|------|------|
| `param_types` | `Vec<Type>` |
| `ret_ty` | `Type` |
| `has_self` | `bool` |
| `is_pub` | `bool` |
| `type_params` | `Vec<str>` |
| `type_param_bounds` | `Vec<StrPair>` |
| `min_args` | `i64` |
| `variadic` | `bool` |

*Defined at line 133*

---

## MethodInfo_new

```fides
pub fn MethodInfo_new(param_types: Vec<Type>, ret_ty: Type, has_self: bool, is_pub: bool) -> MethodInfo {
```

**Parameters:**

- `param_types`: `Vec<Type>`
- `ret_ty`: `Type`
- `has_self`: `bool`
- `is_pub`: `bool`

**Returns:** `MethodInfo`

*Defined at line 148*

---

## MethodInfo_with_tps

LG4: `info` with a method's own type parameters and bounds.

```fides
pub fn MethodInfo_with_tps(info: MethodInfo, tps: Vec<str>, bounds: Vec<StrPair>) -> MethodInfo {
```

**Parameters:**

- `info`: `MethodInfo`
- `tps`: `Vec<str>`
- `bounds`: `Vec<StrPair>`

**Returns:** `MethodInfo`

*Defined at line 155*

---

## MethodInfo_with_params

BG18: `info` with what a call needs of the method's parameters: how many
have no default (a call may leave the others out) and whether the last
one is variadic.

```fides
pub fn MethodInfo_with_params(info: MethodInfo, params: Vec<Param>) -> MethodInfo {
```

**Parameters:**

- `info`: `MethodInfo`
- `params`: `Vec<Param>`

**Returns:** `MethodInfo`

*Defined at line 165*

---

## TraitMethodInfo

```fides
pub struct TraitMethodInfo {
```

**Fields:**

| Name | Type |
|------|------|
| `param_types` | `Vec<Type>` |
| `ret_ty` | `Type` |
| `has_self` | `bool` |
| `type_params` | `Vec<str>` |
| `type_param_bounds` | `Vec<StrPair>` |

*Defined at line 185*

---

## TraitMethodInfo_new

```fides
pub fn TraitMethodInfo_new(param_types: Vec<Type>, ret_ty: Type, has_self: bool) -> TraitMethodInfo {
```

**Parameters:**

- `param_types`: `Vec<Type>`
- `ret_ty`: `Type`
- `has_self`: `bool`

**Returns:** `TraitMethodInfo`

*Defined at line 194*

---

## StructField

```fides
pub struct StructField {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `ty` | `Type` |

*Defined at line 201*

---

## StructField_new

```fides
pub fn StructField_new(name: str, ty: Type) -> StructField {
```

**Parameters:**

- `name`: `str`
- `ty`: `Type`

**Returns:** `StructField`

*Defined at line 206*

---

## EnumVariantEntry

```fides
pub struct EnumVariantEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `payload_types` | `Vec<Type>` |

*Defined at line 213*

---

## EnumVariantEntry_new

```fides
pub fn EnumVariantEntry_new(name: str, payload_types: Vec<Type>) -> EnumVariantEntry {
```

**Parameters:**

- `name`: `str`
- `payload_types`: `Vec<Type>`

**Returns:** `EnumVariantEntry`

*Defined at line 218*

---

## FuncSig

```fides
pub struct FuncSig {
```

**Fields:**

| Name | Type |
|------|------|
| `param_types` | `Vec<Type>` |
| `ret_ty` | `Type` |
| `min_args` | `i64` |

*Defined at line 225*

---

## FuncSig_new

```fides
pub fn FuncSig_new(param_types: Vec<Type>, ret_ty: Type) -> FuncSig {
```

**Parameters:**

- `param_types`: `Vec<Type>`
- `ret_ty`: `Type`

**Returns:** `FuncSig`

*Defined at line 231*

---

## FuncSig_new_with_min

```fides
pub fn FuncSig_new_with_min(param_types: Vec<Type>, ret_ty: Type, min_args: i64) -> FuncSig {
```

**Parameters:**

- `param_types`: `Vec<Type>`
- `ret_ty`: `Type`
- `min_args`: `i64`

**Returns:** `FuncSig`

*Defined at line 237*

---

## MethodMapEntry

```fides
pub struct MethodMapEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `val` | `MethodInfo` |

*Defined at line 244*

---

## MethodMapEntry_new

```fides
pub fn MethodMapEntry_new(key: str, val: MethodInfo) -> MethodMapEntry {
```

**Parameters:**

- `key`: `str`
- `val`: `MethodInfo`

**Returns:** `MethodMapEntry`

*Defined at line 249*

---

## TraitMethodMapEntry

```fides
pub struct TraitMethodMapEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `val` | `TraitMethodInfo` |

*Defined at line 256*

---

## TraitMethodMapEntry_new

```fides
pub fn TraitMethodMapEntry_new(key: str, val: TraitMethodInfo) -> TraitMethodMapEntry {
```

**Parameters:**

- `key`: `str`
- `val`: `TraitMethodInfo`

**Returns:** `TraitMethodMapEntry`

*Defined at line 261*

---

## ScopeEntry

```fides
pub struct ScopeEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `val` | `Type` |

*Defined at line 268*

---

## ScopeEntry_new

```fides
pub fn ScopeEntry_new(key: str, val: Type) -> ScopeEntry {
```

**Parameters:**

- `key`: `str`
- `val`: `Type`

**Returns:** `ScopeEntry`

*Defined at line 273*

---

## LinearVar

```fides
pub struct LinearVar {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `line` | `i64` |

*Defined at line 280*

---

## LinearVar_new

```fides
pub fn LinearVar_new(name: str, line: i64) -> LinearVar {
```

**Parameters:**

- `name`: `str`
- `line`: `i64`

**Returns:** `LinearVar`

*Defined at line 285*

---

## RegionBudget

```fides
pub struct RegionBudget {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `budget` | `OptI64` |
| `usage` | `i64` |
| `heap` | `str` |
| `kv` | `bool` |
| `loop0` | `i64` |

*Defined at line 292*

---

## RegionBudget_new

```fides
pub fn RegionBudget_new(name: str, budget: OptI64, usage: i64) -> RegionBudget {
```

**Parameters:**

- `name`: `str`
- `budget`: `OptI64`
- `usage`: `i64`

**Returns:** `RegionBudget`

*Defined at line 306*

---

## CapturedVar

```fides
pub struct CapturedVar {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `ty` | `Type` |

*Defined at line 313*

---

## CapturedVar_new

```fides
pub fn CapturedVar_new(name: str, ty: Type) -> CapturedVar {
```

**Parameters:**

- `name`: `str`
- `ty`: `Type`

**Returns:** `CapturedVar`

*Defined at line 318*

---

## CallSite

```fides
pub struct CallSite {
```

**Fields:**

| Name | Type |
|------|------|
| `callee` | `str` |
| `args` | `Vec<Expr>` |
| `line` | `i64` |

*Defined at line 325*

---

## CallSite_new

```fides
pub fn CallSite_new(callee: str, args: Vec<Expr>, line: i64) -> CallSite {
```

**Parameters:**

- `callee`: `str`
- `args`: `Vec<Expr>`
- `line`: `i64`

**Returns:** `CallSite`

*Defined at line 331*

---

## DivSite

```fides
pub struct DivSite {
```

**Fields:**

| Name | Type |
|------|------|
| `line` | `i64` |
| `context` | `str` |

*Defined at line 338*

---

## DivSite_new

```fides
pub fn DivSite_new(line: i64, context: str) -> DivSite {
```

**Parameters:**

- `line`: `i64`
- `context`: `str`

**Returns:** `DivSite`

*Defined at line 343*

---

## OptString

```fides
pub enum OptString {
```

**Variants:**

- `Some`
- `None`

*Defined at line 351*

---

## OptType

```fides
pub enum OptType {
```

**Variants:**

- `Some`
- `None`

*Defined at line 356*

---

## OptStmtList

```fides
pub enum OptStmtList {
```

**Variants:**

- `Some`
- `None`

*Defined at line 361*

---

## tc_str_vec_contains

```fides
pub fn tc_str_vec_contains(v: Vec<str>, s: str) -> bool {
```

**Parameters:**

- `v`: `Vec<str>`
- `s`: `str`

**Returns:** `bool`

*Defined at line 368*

---

## tc_dedupe_strs

Same as str_vec_contains but takes a &mut Vec<str> reference.
This is needed because the codegen doesn't auto-deref &mut T to T
for function call arguments.
`v` without repeats, in first-seen order: a list of called functions is
gathered with repeats and made a set once (D10: checking it before each
push was quadratic in the size of a function).

```fides
pub fn tc_dedupe_strs(v: Vec<str>) -> Vec<str> {
```

**Parameters:**

- `v`: `Vec<str>`

**Returns:** `Vec<str>`

*Defined at line 385*

---

## str_vec_contains_ref

```fides
pub fn str_vec_contains_ref(v: Vec<str>, s: str) -> bool {
```

**Parameters:**

- `v`: `Vec<str>`
- `s`: `str`

**Returns:** `bool`

*Defined at line 399*

---

## OptVecType

```fides
pub enum OptVecType {
```

**Variants:**

- `Some`
- `None`

*Defined at line 483*

---

## OptVecStr

```fides
pub enum OptVecStr {
```

**Variants:**

- `Some`
- `None`

*Defined at line 501*

---

## TypeChecker_lookup_func_cache

Cached function signature lookup. Returns the FuncSig for `name` or
OptFuncSig::None if not found. Uses func_cache for O(1) lookup.

```fides
pub fn TypeChecker_lookup_func_cache(self: &TypeChecker, name: str) -> OptFuncSig {
```

**Parameters:**

- `self`: `&TypeChecker`
- `name`: `str`

**Returns:** `OptFuncSig`

*Defined at line 583*

---

## TypeChecker_lookup_const_cache

Cached const lookup. Uses const_cache for O(1) lookup.

```fides
pub fn TypeChecker_lookup_const_cache(self: &TypeChecker, name: str) -> OptType {
```

**Parameters:**

- `self`: `&TypeChecker`
- `name`: `str`

**Returns:** `OptType`

*Defined at line 603*

---

## TypeChecker_lookup_global_cache

Cached global lookup. Uses global_cache for O(1) lookup.

```fides
pub fn TypeChecker_lookup_global_cache(self: &TypeChecker, name: str) -> OptType {
```

**Parameters:**

- `self`: `&TypeChecker`
- `name`: `str`

**Returns:** `OptType`

*Defined at line 615*

---

## TypeChecker_lookup_alias_cache

Cached type alias lookup. Uses alias_cache for O(1) lookup.

```fides
pub fn TypeChecker_lookup_alias_cache(self: &TypeChecker, name: str) -> OptType {
```

**Parameters:**

- `self`: `&TypeChecker`
- `name`: `str`

**Returns:** `OptType`

*Defined at line 627*

---

## TypeChecker_lookup_struct_cache

Cached struct field lookup. Uses struct_cache for O(1) index resolution.

```fides
pub fn TypeChecker_lookup_struct_cache(self: &TypeChecker, name: str) -> OptVecStructField {
```

**Parameters:**

- `self`: `&TypeChecker`
- `name`: `str`

**Returns:** `OptVecStructField`

*Defined at line 639*

---

## StringMethodMap

```fides
pub struct StringMethodMap {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `val` | `Vec<MethodMapEntry>` |

*Defined at line 676*

---

## StringMethodMap_new

```fides
pub fn StringMethodMap_new(key: str, val: Vec<MethodMapEntry>) -> StringMethodMap {
```

**Parameters:**

- `key`: `str`
- `val`: `Vec<MethodMapEntry>`

**Returns:** `StringMethodMap`

*Defined at line 681*

---

## OptMethodMap

```fides
pub enum OptMethodMap {
```

**Variants:**

- `Some`
- `None`

*Defined at line 687*

---

## OptFuncSig

```fides
pub enum OptFuncSig {
```

**Variants:**

- `Some`
- `None`

*Defined at line 692*

---

## OptVecStructField

```fides
pub enum OptVecStructField {
```

**Variants:**

- `Some`
- `None`

*Defined at line 697*

---

## OptMethodInfo

```fides
pub enum OptMethodInfo {
```

**Variants:**

- `Some`
- `None`

*Defined at line 713*

---

## StringTraitMethodMap

```fides
pub struct StringTraitMethodMap {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `val` | `Vec<TraitMethodMapEntry>` |

*Defined at line 734*

---

## StringTraitMethodMap_new

```fides
pub fn StringTraitMethodMap_new(key: str, val: Vec<TraitMethodMapEntry>) -> StringTraitMethodMap {
```

**Parameters:**

- `key`: `str`
- `val`: `Vec<TraitMethodMapEntry>`

**Returns:** `StringTraitMethodMap`

*Defined at line 739*

---

## OptTraitMethodMap

```fides
pub enum OptTraitMethodMap {
```

**Variants:**

- `Some`
- `None`

*Defined at line 756*

---

## OptTraitMethodInfo

```fides
pub enum OptTraitMethodInfo {
```

**Variants:**

- `Some`
- `None`

*Defined at line 772*

---

## TypeChecker

```fides
pub struct TypeChecker {
```

**Fields:**

| Name | Type |
|------|------|
| `scopes` | `Vec<ScopeStack>` |
| `funcs` | `Vec<StringFuncSig>` |
| `structs` | `Vec<StringStructFields>` |
| `enums` | `Vec<str>` |
| `enum_variants` | `Vec<StringEnumVariants>` |
| `enum_type_params` | `Vec<StringVecString>` |
| `enum_tp_set` | `i64` |
| `consts` | `Vec<StringType>` |
| `globals` | `Vec<StringType>` |
| `type_aliases` | `Vec<StringType>` |
| `methods` | `Vec<StringMethodMap>` |
| `current_type_params` | `Vec<str>` |
| `current_type_param_bounds` | `Vec<StringString>` |
| `skip_bodies` | `i64` |
| `scoped_structs` | `Vec<str>` |
| `methods_index` | `i64` |
| `methods_indexed` | `i64` |
| `methods_hm` | `Vec<i64>` |
| `sname_index` | `i64` |
| `sname_indexed` | `i64` |
| `ename_index` | `i64` |
| `ename_indexed` | `i64` |
| `func_generic_names` | `Vec<str>` |
| `func_generic_tps` | `Vec<Vec<str>>` |
| `func_generic_bounds` | `Vec<Vec<StrPair>>` |
| `impl_pairs` | `Vec<str>` |
| `impl_tp_names` | `Vec<str>` |
| `impl_tp_lists` | `Vec<Vec<str>>` |
| `value_param_names` | `Vec<str>` |
| `value_param_types` | `Vec<Type>` |
| `traits` | `Vec<StringTraitMethodMap>` |
| `trait_assoc_types` | `Vec<StringVecString>` |
| `trait_impl_assoc_bindings` | `Vec<TraitImplAssocEntry>` |
| `item_modules` | `Vec<StringString>` |
| `item_visibility` | `Vec<StringBool>` |
| `field_visibility` | `Vec<FieldVisEntry>` |
| `struct_modules` | `Vec<StringString>` |
| `current_module` | `str` |
| `errors` | `Vec<str>` |
| `warnings` | `Vec<str>` |
| `granted_caps` | `Vec<str>` |
| `strict_caps` | `bool` |
| `func_effects` | `Vec<StringVecStrTc>` |
| `ho_param_effects` | `Vec<StringVecStrTc>` |
| `region_stack` | `Vec<str>` |
| `var_regions` | `Vec<StringString>` |
| `freed_regions` | `Vec<str>` |
| `cap_verified` | `Vec<str>` |
| `current_func_name` | `OptString` |
| `reported_undefined` | `Vec<str>` |
| `ca6_scope_depth` | `i64` |
| `ca6_names` | `Vec<str>` |
| `ca6_lines` | `Vec<i64>` |
| `func_effects_index` | `i64` |
| `heap_alloc_index` | `i64` |
| `ffi_reach_index` | `i64` |
| `ho_param_effects_index` | `i64` |
| `cur_ho_params` | `Vec<str>` |
| `ho_fn_index` | `i64` |
| `ho_fn_idx` | `Vec<Vec<i64>>` |
| `ho_fn_pnames` | `Vec<Vec<str>>` |
| `item_modules_index` | `i64` |
| `item_visibility_index` | `i64` |
| `struct_modules_index` | `i64` |
| `method_call_callers` | `Vec<str>` |
| `method_call_owners` | `Vec<str>` |
| `method_call_names` | `Vec<str>` |
| `current_return_types` | `Vec<Type>` |
| `current_ret_ty` | `OptType` |
| `extern_fns` | `Vec<str>` |
| `current_self_type` | `OptString` |
| `sandbox_depth` | `i64` |
| `in_conserve` | `bool` |
| `in_no_alloc` | `bool` |
| `txn_depth` | `i64` |
| `struct_tp_names` | `Vec<str>` |
| `struct_tp_lists` | `Vec<Vec<str>>` |
| `gpu_region_depth` | `i64` |
| `region_budgets` | `Vec<RegionBudget>` |
| `region_lines` | `Vec<StringString>` |
| `var_alloc_lines` | `Vec<StringString>` |
| `linear_structs` | `Vec<str>` |
| `send_structs` | `Vec<str>` |
| `sync_structs` | `Vec<str>` |
| `consumed_vars` | `Vec<str>` |
| `scope_linear_vars` | `Vec<LinearVarStack>` |
| `loop_depth` | `i64` |
| `unsafe_depth` | `i64` |
| `checking_fn` | `str` |
| `thread_entries` | `Vec<str>` |
| `write_funcs` | `Vec<str>` |
| `write_globals` | `Vec<str>` |
| `write_lines` | `Vec<i64>` |
| `uninit_vars` | `Vec<str>` |
| `current_line` | `i64` |
| `val_vars_per_scope` | `Vec<StrStack>` |
| `check_depth` | `i64` |
| `func_cache` | `i64` |
| `struct_cache` | `i64` |
| `enum_cache` | `i64` |
| `const_cache` | `i64` |
| `global_cache` | `i64` |
| `alias_cache` | `i64` |
| `caches_built` | `bool` |
| `private_fields` | `i64` |
| `private_field_modules` | `Vec<str>` |
| `field_default_at` | `i64` |
| `field_default_exprs` | `Vec<Expr>` |
| `fn_pnames_at` | `i64` |
| `fn_pnames` | `Vec<Vec<str>>` |
| `fn_pdefaults` | `Vec<Vec<bool>>` |
| `deprecated_at` | `i64` |
| `deprecated_msgs` | `Vec<str>` |
| `last_fn_names` | `i64` |

*Defined at line 786*

---

## StringStructFields

```fides
pub struct StringStructFields {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `val` | `Vec<StructField>` |

*Defined at line 996*

---

## StringStructFields_new

```fides
pub fn StringStructFields_new(key: str, val: Vec<StructField>) -> StringStructFields {
```

**Parameters:**

- `key`: `str`
- `val`: `Vec<StructField>`

**Returns:** `StringStructFields`

*Defined at line 1001*

---

## StringEnumVariants

```fides
pub struct StringEnumVariants {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `val` | `Vec<EnumVariantEntry>` |

*Defined at line 1007*

---

## StringEnumVariants_new

```fides
pub fn StringEnumVariants_new(key: str, val: Vec<EnumVariantEntry>) -> StringEnumVariants {
```

**Parameters:**

- `key`: `str`
- `val`: `Vec<EnumVariantEntry>`

**Returns:** `StringEnumVariants`

*Defined at line 1012*

---

## StringVecStrTc

```fides
pub struct StringVecStrTc {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `val` | `Vec<str>` |

*Defined at line 1018*

---

## StringVecStrTc_new

```fides
pub fn StringVecStrTc_new(key: str, val: Vec<str>) -> StringVecStrTc {
```

**Parameters:**

- `key`: `str`
- `val`: `Vec<str>`

**Returns:** `StringVecStrTc`

*Defined at line 1023*

---

## StringI64

```fides
pub struct StringI64 {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `val` | `i64` |

*Defined at line 1029*

---

## StringI64_new

```fides
pub fn StringI64_new(key: str, val: i64) -> StringI64 {
```

**Parameters:**

- `key`: `str`
- `val`: `i64`

**Returns:** `StringI64`

*Defined at line 1034*

---

## TraitImplAssocEntry

```fides
pub struct TraitImplAssocEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `TraitImplKey` |
| `val` | `Vec<StringType>` |

*Defined at line 1040*

---

## TraitImplAssocEntry_new

```fides
pub fn TraitImplAssocEntry_new(key: TraitImplKey, val: Vec<StringType>) -> TraitImplAssocEntry {
```

**Parameters:**

- `key`: `TraitImplKey`
- `val`: `Vec<StringType>`

**Returns:** `TraitImplAssocEntry`

*Defined at line 1045*

---

## StringFuncSig

```fides
pub struct StringFuncSig {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `val` | `FuncSig` |

*Defined at line 1052*

---

## StringFuncSig_new

```fides
pub fn StringFuncSig_new(key: str, val: FuncSig) -> StringFuncSig {
```

**Parameters:**

- `key`: `str`
- `val`: `FuncSig`

**Returns:** `StringFuncSig`

*Defined at line 1057*

---

## TypeChecker_free_maps

Free the typechecker's index maps (raw handles, which ownership does
not drop) once checking is over; nothing reads them after.

```fides
pub fn TypeChecker_free_maps(self: &mut TypeChecker) {
```

**Parameters:**

- `self`: `&mut TypeChecker`

*Defined at line 1067*

---

## TypeChecker

```fides
impl TypeChecker {
```

**Methods:**

### new

```fides
pub fn new() -> TypeChecker {
```

### check

```fides
pub fn check(self: &mut TypeChecker, program: Program) -> bool {
```

### run_lints

```fides
pub fn run_lints(self: &mut TypeChecker, program: Program) {
```

*Defined at line 1124*

---

## tc_note_module_file

Record that `module` was loaded from `path`; `std_dir` is the standard
library directory. Every module but the compiler's own and the trusted
base (tc_path_is_trusted_base) has its raw memory, integer-to-pointer
casts and C calls checked for `unsafe`: user modules since release M2,
the stdlib's modules since M10 (Nov 22).

```fides
pub fn tc_note_module_file(module: str, path: str, std_dir: str) {
```

**Parameters:**

- `module`: `str`
- `path`: `str`
- `std_dir`: `str`

*Defined at line 1468*

---

## tc_note_program_file

Record the file of the program being compiled (release M2): a program
that is core or a runtime file keeps the trusted base's exemption from
the `unsafe` rule (M10: any other stdlib file does not).

```fides
pub fn tc_note_program_file(path: str, std_dir: str) {
```

**Parameters:**

- `path`: `str`
- `std_dir`: `str`

*Defined at line 1496*

---

## tc_operator_method

CL10: the method an arithmetic operator calls on a struct.

```fides
pub fn tc_operator_method(op: BinOp) -> str {
```

**Parameters:**

- `op`: `BinOp`

**Returns:** `str`

*Defined at line 1629*

---

## tc_struct_dim

The packed dimension of struct `name`, 0 when it is not a unit.

```fides
pub fn tc_struct_dim(name: str) -> i64 {
```

**Parameters:**

- `name`: `str`

**Returns:** `i64`

*Defined at line 1767*

---

## TypeChecker_register_program

```fides
pub fn TypeChecker_register_program(self: &mut TypeChecker, program: Program, module: str) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`
- `module`: `str`

*Defined at line 2177*

---

## TypeChecker_register_trait_impls

```fides
pub fn TypeChecker_register_trait_impls(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 2545*

---

## TypeChecker_verify_trait_impls

```fides
pub fn TypeChecker_verify_trait_impls(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 2601*

---

## OptTraitDef

```fides
pub enum OptTraitDef {
```

**Variants:**

- `Some`
- `None`

*Defined at line 2696*

---

## TypeChecker_register_generated_decls

Build O(1) index caches for the typechecker's symbol tables.
Called after all modules are registered, before checking bodies.
This converts O(n) linear scans into O(1) HashMap lookups for the
hottest paths: function signature lookup, struct field lookup,
enum variant lookup, const/global/type_alias resolution.
With 100K+ LOC, there are thousands of entries in each table.
Register the functions, structs and impl methods of the merged program
that the per-module registration did not see (src/typechecker.rs
register_generated_decls): comptime-generated or desugared items, and
the methods of a second `impl T` block for a type.

```fides
pub fn TypeChecker_register_generated_decls(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 3108*

---

## TypeChecker_build_caches

```fides
pub fn TypeChecker_build_caches(self: &mut TypeChecker) -> void {
```

**Parameters:**

- `self`: `&mut TypeChecker`

**Returns:** `void`

*Defined at line 3245*

---

## TypeChecker_check_annotation_validity

```fides
pub fn TypeChecker_check_annotation_validity(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 3543*

---

## TypeChecker_check_bodies

```fides
pub fn TypeChecker_check_bodies(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 3587*

---

## tc_skips

Whether `key` names a body this check leaves out (see skip_bodies).

```fides
pub fn tc_skips(self: &TypeChecker, key: str) -> bool {
```

**Parameters:**

- `self`: `&TypeChecker`
- `key`: `str`

**Returns:** `bool`

*Defined at line 3727*

---

## tc_core_skip_set

The keys of every body in `core` that `others` does not redefine: what a
build may skip once this core has been checked clean (release D9).

```fides
pub fn tc_core_skip_set(core: Program, others: Vec<Program>) -> i64 {
```

**Parameters:**

- `core`: `Program`
- `others`: `Vec<Program>`

**Returns:** `i64`

*Defined at line 3736*

---

## TypeChecker_effects_of

L7G (`:effects` in fid repl): the effects inferred for function `name`
in the language's terms (reference §9.1: IO, Net, Spawn, Diverge,
Alloc, ...), read after TypeChecker_check_bodies. Empty when it has none
(unlike the IR's set, a stack local is no Alloc/Read/Write effect).

```fides
pub fn TypeChecker_effects_of(self: &TypeChecker, name: str) -> Vec<str> {
```

**Parameters:**

- `self`: `&TypeChecker`
- `name`: `str`

**Returns:** `Vec<str>`

*Defined at line 4348*

---

## TypeChecker_knows_fn

L7G: whether the checker registered a function called `name`.

```fides
pub fn TypeChecker_knows_fn(self: &TypeChecker, name: str) -> bool {
```

**Parameters:**

- `self`: `&TypeChecker`
- `name`: `str`

**Returns:** `bool`

*Defined at line 4353*

---

## TypeChecker_compute_func_effects

```fides
pub fn TypeChecker_compute_func_effects(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 4522*

---

## tc_body_index

Find the StmtList body for a function by name, searching funcs then impls.
Matches the Rust compiler's `program.funcs.iter().find()` + `program.impls` lookup.
name -> 1 + function index, or -(1 + impl * 65536 + method) for an impl
method; the first of a name wins, as TypeChecker_find_body's scan.

```fides
pub fn tc_body_index(program: Program) -> i64 {
```

**Parameters:**

- `program`: `Program`

**Returns:** `i64`

*Defined at line 4682*

---

## tc_find_body_indexed

TypeChecker_find_body through an index from tc_body_index.

```fides
pub fn tc_find_body_indexed(program: Program, idx: i64, name: str) -> OptStmtList {
```

**Parameters:**

- `program`: `Program`
- `idx`: `i64`
- `name`: `str`

**Returns:** `OptStmtList`

*Defined at line 4707*

---

## tc_find_params_indexed

The parameters of the body tc_find_body_indexed finds (2g.65).

```fides
pub fn tc_find_params_indexed(program: Program, idx: i64, name: str) -> Vec<Param> {
```

**Parameters:**

- `program`: `Program`
- `idx`: `i64`
- `name`: `str`

**Returns:** `Vec<Param>`

*Defined at line 4722*

---

## TypeChecker_find_body

```fides
pub fn TypeChecker_find_body(self: &mut TypeChecker, program: Program, name: str) -> OptStmtList {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`
- `name`: `str`

**Returns:** `OptStmtList`

*Defined at line 4736*

---

## TypeChecker_verify_declared_effects

```fides
pub fn TypeChecker_verify_declared_effects(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 4761*

---

## TypeChecker_verify_deterministic

```fides
pub fn TypeChecker_verify_deterministic(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 4819*

---

## TypeChecker_verify_conserve

```fides
pub fn TypeChecker_verify_conserve(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 4894*

---

## TypeChecker_verify_gas

```fides
pub fn TypeChecker_verify_gas(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 5174*

---

## TypeChecker_verify_safety_annotations

```fides
pub fn TypeChecker_verify_safety_annotations(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 5542*

---

## expand_safety_primitives

```fides
pub fn expand_safety_primitives(annotations: Vec<Annotation>) -> Vec<str> {
```

**Parameters:**

- `annotations`: `Vec<Annotation>`

**Returns:** `Vec<str>`

*Defined at line 5956*

---

## has_primitive

```fides
pub fn has_primitive(prims: Vec<str>, name: str) -> bool {
```

**Parameters:**

- `prims`: `Vec<str>`
- `name`: `str`

**Returns:** `bool`

*Defined at line 6045*

---

## safety_domain_label

```fides
pub fn safety_domain_label(prims: Vec<str>) -> str {
```

**Parameters:**

- `prims`: `Vec<str>`

**Returns:** `str`

*Defined at line 6057*

---

## is_blocking_func

F59 / CA10: a builtin that can wait (forbidden in @no-blocking and so in
@grid-safe code): what blocks a worker thread (tc_blocking_call_fix),
the task waits that park a task (sleep, fd waits, join, yield), and
sockets, HTTP, DNS, TLS, files, directories and processes.

```fides
pub fn is_blocking_func(name: str) -> bool {
```

**Parameters:**

- `name`: `str`

**Returns:** `bool`

*Defined at line 6083*

---

## is_os_dependent_func

```fides
pub fn is_os_dependent_func(name: str) -> bool {
```

**Parameters:**

- `name`: `str`

**Returns:** `bool`

*Defined at line 6107*

---

## safety_fn_params

Parameters of function or `dyn` type: a call through one is indirect.

```fides
pub fn safety_fn_params(params: Vec<Param>) -> Vec<str> {
```

**Parameters:**

- `params`: `Vec<Param>`

**Returns:** `Vec<str>`

*Defined at line 6263*

---

## check_safety_body

```fides
pub fn check_safety_body(func_name: str, body: Vec<Stmt>, prims: Vec<str>, anns: Vec<Annotation>, func_pos: i64, all_func_prims: Vec<StrStack>, fn_params: Vec<str>, method_names: Vec<str>, msuf: i64) -> Vec<str> {
```

**Parameters:**

- `func_name`: `str`
- `body`: `Vec<Stmt>`
- `prims`: `Vec<str>`
- `anns`: `Vec<Annotation>`
- `func_pos`: `i64`
- `all_func_prims`: `Vec<StrStack>`
- `fn_params`: `Vec<str>`
- `method_names`: `Vec<str>`
- `msuf`: `i64`

**Returns:** `Vec<str>`

*Defined at line 6290*

---

## tc_vec_contains_str

```fides
pub fn tc_vec_contains_str(v: Vec<str>, s: str) -> bool {
```

**Parameters:**

- `v`: `Vec<str>`
- `s`: `str`

**Returns:** `bool`

*Defined at line 6697*

---

## scan_stmts_for_calls

```fides
pub fn scan_stmts_for_calls(stmts: Vec<Stmt>, calls: &mut Vec<str>) {
```

**Parameters:**

- `stmts`: `Vec<Stmt>`
- `calls`: `&mut Vec<str>`

*Defined at line 6710*

---

## scan_stmt_for_calls

```fides
pub fn scan_stmt_for_calls(stmt: Stmt, calls: &mut Vec<str>) {
```

**Parameters:**

- `stmt`: `Stmt`
- `calls`: `&mut Vec<str>`

*Defined at line 6718*

---

## scan_expr_for_calls

```fides
pub fn scan_expr_for_calls(expr: Expr, calls: &mut Vec<str>) {
```

**Parameters:**

- `expr`: `Expr`
- `calls`: `&mut Vec<str>`

*Defined at line 6845*

---

## scan_for_panic_risks

```fides
pub fn scan_for_panic_risks(stmts: Vec<Stmt>, wrapping: bool, lines: &mut Vec<i64>, msgs: &mut Vec<str>) {
```

**Parameters:**

- `stmts`: `Vec<Stmt>`
- `wrapping`: `bool`
- `lines`: `&mut Vec<i64>`
- `msgs`: `&mut Vec<str>`

*Defined at line 7068*

---

## scan_for_division

```fides
pub fn scan_for_division(stmts: Vec<Stmt>, sites: &mut Vec<str>) {
```

**Parameters:**

- `stmts`: `Vec<Stmt>`
- `sites`: `&mut Vec<str>`

*Defined at line 7637*

---

## scan_stmt_for_division

```fides
pub fn scan_stmt_for_division(stmt: Stmt, sites: &mut Vec<str>) {
```

**Parameters:**

- `stmt`: `Stmt`
- `sites`: `&mut Vec<str>`

*Defined at line 7674*

---

## scan_stmts_for_division

```fides
pub fn scan_stmts_for_division(stmts: Vec<Stmt>, sites: &mut Vec<str>) {
```

**Parameters:**

- `stmts`: `Vec<Stmt>`
- `sites`: `&mut Vec<str>`

*Defined at line 7780*

---

## scan_expr_for_division

```fides
pub fn scan_expr_for_division(expr: Expr, sites: &mut Vec<str>) {
```

**Parameters:**

- `expr`: `Expr`
- `sites`: `&mut Vec<str>`

*Defined at line 7788*

---

## scan_for_while_loops

```fides
pub fn scan_for_while_loops(stmts: Vec<Stmt>, sites: &mut Vec<i64>) {
```

**Parameters:**

- `stmts`: `Vec<Stmt>`
- `sites`: `&mut Vec<i64>`

*Defined at line 7985*

---

## scan_stmt_for_while

```fides
pub fn scan_stmt_for_while(stmt: Stmt, sites: &mut Vec<i64>) {
```

**Parameters:**

- `stmt`: `Stmt`
- `sites`: `&mut Vec<i64>`

*Defined at line 7993*

---

## scan_stmts_for_while

```fides
pub fn scan_stmts_for_while(stmts: Vec<Stmt>, sites: &mut Vec<i64>) {
```

**Parameters:**

- `stmts`: `Vec<Stmt>`
- `sites`: `&mut Vec<i64>`

*Defined at line 8060*

---

## scan_expr_for_while

```fides
pub fn scan_expr_for_while(expr: Expr, sites: &mut Vec<i64>) {
```

**Parameters:**

- `expr`: `Expr`
- `sites`: `&mut Vec<i64>`

*Defined at line 8068*

---

## TypeChecker_verify_constant_time

```fides
pub fn TypeChecker_verify_constant_time(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 8141*

---

## TypeChecker_verify_realtime

```fides
pub fn TypeChecker_verify_realtime(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 9016*

---

## TypeChecker_verify_differentiable

```fides
pub fn TypeChecker_verify_differentiable(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 9089*

---

## TypeChecker_verify_quantized

```fides
pub fn TypeChecker_verify_quantized(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 9168*

---

## TypeChecker_verify_checkpoint

```fides
pub fn TypeChecker_verify_checkpoint(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 9216*

---

## TypeChecker_verify_dp

```fides
pub fn TypeChecker_verify_dp(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 9246*

---

## TypeChecker_verify_shard

```fides
pub fn TypeChecker_verify_shard(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 9442*

---

## TypeChecker_verify_adapter

```fides
pub fn TypeChecker_verify_adapter(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 9474*

---

## TypeChecker_mentions_scoped

Whether a type is, or holds, a @scoped struct (release M11).

```fides
pub fn TypeChecker_mentions_scoped(self: &TypeChecker, t: Type) -> bool {
```

**Parameters:**

- `self`: `&TypeChecker`
- `t`: `Type`

**Returns:** `bool`

*Defined at line 9498*

---

## TypeChecker_verify_scoped

A @scoped struct lives only for its scope (release M11): it may be a
parameter or a local, never a field, a global, an element type or a
function result, so nothing keeps it (or the strings it borrows) longer.

```fides
pub fn TypeChecker_verify_scoped(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 9536*

---

## TypeChecker_verify_tainted

```fides
pub fn TypeChecker_verify_tainted(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 9582*

---

## TfResult

```fides
pub struct TfResult {
```

**Fields:**

| Name | Type |
|------|------|
| `ret` | `bool` |
| `hits` | `Vec<str>` |

*Defined at line 9663*

---

## TfGlobal

```fides
pub struct TfGlobal {
```

**Fields:**

| Name | Type |
|------|------|
| `funcs` | `Vec<Func>` |
| `index` | `i64` |
| `memo` | `i64` |
| `memo_ret` | `Vec<bool>` |
| `memo_hits` | `Vec<Vec<str>>` |

*Defined at line 9668*

---

## TfCtx

```fides
pub struct TfCtx {
```

**Fields:**

| Name | Type |
|------|------|
| `func` | `str` |
| `names` | `Vec<str>` |
| `taint` | `Vec<bool>` |
| `depth` | `i64` |
| `ret` | `bool` |
| `hits` | `Vec<str>` |

*Defined at line 9678*

---

## LoSum

```fides
pub struct LoSum {
```

**Fields:**

| Name | Type |
|------|------|
| `acq` | `Vec<str>` |
| `efrom` | `Vec<str>` |
| `eto` | `Vec<str>` |
| `ewhere` | `Vec<str>` |

*Defined at line 10380*

---

## LoGlobal

```fides
pub struct LoGlobal {
```

**Fields:**

| Name | Type |
|------|------|
| `funcs` | `Vec<Func>` |
| `index` | `i64` |
| `globals` | `i64` |
| `memo` | `i64` |
| `sums` | `Vec<LoSum>` |

*Defined at line 10387*

---

## LoCtx

```fides
pub struct LoCtx {
```

**Fields:**

| Name | Type |
|------|------|
| `fname` | `str` |
| `params` | `Vec<str>` |
| `held` | `Vec<str>` |
| `line` | `i64` |
| `acq` | `Vec<str>` |
| `efrom` | `Vec<str>` |
| `eto` | `Vec<str>` |
| `ewhere` | `Vec<str>` |
| `shadow` | `Vec<str>` |
| `clo_names` | `Vec<str>` |
| `clo_acq` | `Vec<Vec<str>>` |

*Defined at line 10399*

---

## TypeChecker_verify_lock_order

```fides
pub fn TypeChecker_verify_lock_order(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 11060*

---

## HmGraph

The call graph @hermetic searches. A node is a free function (its name)
or an impl method (`Type.method`); `index` maps a name, and a static
call's `Type_method` (scan_expr_for_calls with g_scan_dot_methods),
to its body code: fi + 1 for program.funcs[fi], -(1 + impl * 65536 +
method) for a method. Instance method calls come from what the type
checker recorded while checking bodies (TypeChecker_record_method_call:
caller, receiver type, method), chained by caller: `mc_head` maps a
caller to 1 + its last recorded call, `mc_next[i]` is 1 + the one
before it (0: none).

```fides
pub struct HmGraph {
```

**Fields:**

| Name | Type |
|------|------|
| `index` | `i64` |
| `mc_head` | `i64` |
| `mc_next` | `Vec<i64>` |

*Defined at line 11244*

---

## TypeChecker_verify_hermetic

```fides
pub fn TypeChecker_verify_hermetic(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 11383*

---

## PfFacts

```fides
pub struct PfFacts {
```

**Fields:**

| Name | Type |
|------|------|
| `calls` | `Vec<str>` |
| `writes` | `Vec<str>` |
| `lets` | `Vec<str>` |
| `spawns` | `bool` |
| `hand_callee` | `Vec<str>` |
| `hand_arg` | `Vec<i64>` |
| `hand_var` | `Vec<str>` |
| `hand_ref` | `Vec<bool>` |

*Defined at line 11413*

---

## PfReach

```fides
pub struct PfReach {
```

**Fields:**

| Name | Type |
|------|------|
| `fis` | `Vec<i64>` |
| `facts` | `Vec<PfFacts>` |

*Defined at line 11428*

---

## TypeChecker_verify_redundant

F45 @redundant — dual-channel execution. compiler/mono.fd turns the
function into two channels (`<f>_chan_a`, `<f>_chan_b`) and a voter `f`
that runs both on the same arguments, compares the results and stops
the program (fail-safe) when they differ. For that to be sound the
function must be one the voter can run twice: a scalar result, no
generics, no `async`, no `&mut`/`*mut` parameter (channel A's writes
would be channel B's input), and nothing it reaches may act outside
(IO, Net, Spawn, Write, Noise, Comm effects; a write to a global; a
task): the side effect would happen twice, and a voter cannot compare it.

```fides
pub fn TypeChecker_verify_redundant(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 11917*

---

## TypeChecker_verify_partitioned

F47 @partitioned(name) — spatial partitioning (ARINC 653) checked at
compile time. A function marked @partitioned(P) is an entry of
partition P (the name defaults to the function's). The code of a
partition is its entries and every function they reach that is not
itself an entry of a partition (shared library code). A mutable global
reached by the code of two partitions is shared memory between them: an
error. A partition calling an entry of another one directly is an error:
partitions communicate only through ports (stdlib/arinc.fd queuing and
sampling ports, passed in as values).

```fides
pub fn TypeChecker_verify_partitioned(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 11985*

---

## TypeChecker_verify_serverless

F32 @serverless — a request handler of a serverless function. Its
instance can be frozen after a response, recycled for the next request,
run many times in parallel, or started cold for one request. So the
handler and everything it reaches: writes no module global (state does
not reliably survive between requests), starts no task (it may never
finish once the instance is frozen), and reads no global whose
initializer runs code at start-up (every cold start pays for it before
the first request: make it a constant or a `comptime` value).

```fides
pub fn TypeChecker_verify_serverless(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 12246*

---

## TypeChecker_verify_emit

```fides
pub fn TypeChecker_verify_emit(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 12426*

---

## TypeChecker_verify_contracts

```fides
pub fn TypeChecker_verify_contracts(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 12611*

---

## TypeChecker_verify_allocator_params

```fides
pub fn TypeChecker_verify_allocator_params(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 12962*

---

## TypeChecker_check_func

```fides
pub fn TypeChecker_check_func(self: &mut TypeChecker, func: Func) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `func`: `Func`

*Defined at line 13184*

---

## stmts_always_leave

```fides
pub fn stmts_always_leave(stmts: Vec<Stmt>) -> bool {
```

**Parameters:**

- `stmts`: `Vec<Stmt>`

**Returns:** `bool`

*Defined at line 13580*

---

## stmts_have_break

```fides
pub fn stmts_have_break(stmts: Vec<Stmt>) -> bool {
```

**Parameters:**

- `stmts`: `Vec<Stmt>`

**Returns:** `bool`

*Defined at line 13668*

---

## TypeChecker_push_scope

```fides
pub fn TypeChecker_push_scope(self: &mut TypeChecker) {
```

**Parameters:**

- `self`: `&mut TypeChecker`

*Defined at line 13738*

---

## TypeChecker_pop_scope

```fides
pub fn TypeChecker_pop_scope(self: &mut TypeChecker) {
```

**Parameters:**

- `self`: `&mut TypeChecker`

*Defined at line 13744*

---

## TypeChecker_declare

```fides
pub fn TypeChecker_declare(self: &mut TypeChecker, name: str, ty: Type) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `name`: `str`
- `ty`: `Type`

*Defined at line 13780*

---

## TypeChecker_lookup

```fides
pub fn TypeChecker_lookup(self: &TypeChecker, name: str) -> OptType {
```

**Parameters:**

- `self`: `&TypeChecker`
- `name`: `str`

**Returns:** `OptType`

*Defined at line 13792*

---

## TypeChecker_resolve_type

```fides
pub fn TypeChecker_resolve_type(self: &TypeChecker, ty: Type) -> Type {
```

**Parameters:**

- `self`: `&TypeChecker`
- `ty`: `Type`

**Returns:** `Type`

*Defined at line 14199*

---

## TypeChecker_subst_type_params

```fides
pub fn TypeChecker_subst_type_params(ty: Type, subst: Vec<StringType>) -> Type {
```

**Parameters:**

- `ty`: `Type`
- `subst`: `Vec<StringType>`

**Returns:** `Type`

*Defined at line 14307*

---

## TypeChecker_resolve_self_type

```fides
pub fn TypeChecker_resolve_self_type(self: &TypeChecker, ty: Type) -> Type {
```

**Parameters:**

- `self`: `&TypeChecker`
- `ty`: `Type`

**Returns:** `Type`

*Defined at line 14356*

---

## TypeChecker_resolve_assoc_in_type

```fides
pub fn TypeChecker_resolve_assoc_in_type(self: &TypeChecker, ty: Type, type_param: str, trait_name: str) -> Type {
```

**Parameters:**

- `self`: `&TypeChecker`
- `ty`: `Type`
- `type_param`: `str`
- `trait_name`: `str`

**Returns:** `Type`

*Defined at line 14405*

---

## TypeChecker_resolve_struct_name

```fides
pub fn TypeChecker_resolve_struct_name(self: &TypeChecker, name: str) -> str {
```

**Parameters:**

- `self`: `&TypeChecker`
- `name`: `str`

**Returns:** `str`

*Defined at line 14430*

---

## TypeChecker_is_numeric

```fides
pub fn TypeChecker_is_numeric(self: &TypeChecker, ty: Type) -> bool {
```

**Parameters:**

- `self`: `&TypeChecker`
- `ty`: `Type`

**Returns:** `bool`

*Defined at line 14451*

---

## TypeChecker_is_integer

```fides
pub fn TypeChecker_is_integer(self: &TypeChecker, ty: Type) -> bool {
```

**Parameters:**

- `self`: `&TypeChecker`
- `ty`: `Type`

**Returns:** `bool`

*Defined at line 14499*

---

## TypeChecker_is_float

```fides
pub fn TypeChecker_is_float(self: &TypeChecker, ty: Type) -> bool {
```

**Parameters:**

- `self`: `&TypeChecker`
- `ty`: `Type`

**Returns:** `bool`

*Defined at line 14541*

---

## TypeChecker_is_typed_string

```fides
pub fn TypeChecker_is_typed_string(self: &TypeChecker, ty: Type) -> bool {
```

**Parameters:**

- `self`: `&TypeChecker`
- `ty`: `Type`

**Returns:** `bool`

*Defined at line 14556*

---

## tc_operand_type_str

2g.87: the type a message names for operand `e` of type `t`. An
unsuffixed integer literal is an i64 in the language; the checker types
one with no integer hint by the narrowest type that holds it
(infer_int_type: `2` is an i8 inside), which a message must not show
("got i8" for `2 * d`).

```fides
pub fn tc_operand_type_str(e: Expr, t: Type) -> str {
```

**Parameters:**

- `e`: `Expr`
- `t`: `Type`

**Returns:** `str`

*Defined at line 14588*

---

## infer_int_type

```fides
pub fn infer_int_type(value: i64) -> Type {
```

**Parameters:**

- `value`: `i64`

**Returns:** `Type`

*Defined at line 14618*

---

## int_rank

```fides
pub fn int_rank(ty: Type) -> i64 {
```

**Parameters:**

- `ty`: `Type`

**Returns:** `i64`

*Defined at line 14631*

---

## wider_type

```fides
pub fn wider_type(a: Type, b: Type) -> Type {
```

**Parameters:**

- `a`: `Type`
- `b`: `Type`

**Returns:** `Type`

*Defined at line 14670*

---

## can_implicitly_widen

```fides
pub fn can_implicitly_widen(from: Type, to: Type) -> bool {
```

**Parameters:**

- `from`: `Type`
- `to`: `Type`

**Returns:** `bool`

*Defined at line 14677*

---

## type_eq

```fides
pub fn type_eq(a: Type, b: Type) -> bool {
```

**Parameters:**

- `a`: `Type`
- `b`: `Type`

**Returns:** `bool`

*Defined at line 14737*

---

## TypeChecker_types_compatible

```fides
pub fn TypeChecker_types_compatible(self: &TypeChecker, declared: Type, actual: Type) -> bool {
```

**Parameters:**

- `self`: `&TypeChecker`
- `declared`: `Type`
- `actual`: `Type`

**Returns:** `bool`

*Defined at line 15659*

---

## TypeChecker_type_contains_param

```fides
pub fn TypeChecker_type_contains_param(self: &TypeChecker, ty: Type) -> bool {
```

**Parameters:**

- `self`: `&TypeChecker`
- `ty`: `Type`

**Returns:** `bool`

*Defined at line 17223*

---

## TypeChecker_is_known_enum

```fides
pub fn TypeChecker_is_known_enum(self: &TypeChecker, name: str) -> bool {
```

**Parameters:**

- `self`: `&TypeChecker`
- `name`: `str`

**Returns:** `bool`

*Defined at line 17296*

---

## TypeChecker_type_size

```fides
pub fn TypeChecker_type_size(self: &TypeChecker, ty: Type) -> i64 {
```

**Parameters:**

- `self`: `&TypeChecker`
- `ty`: `Type`

**Returns:** `i64`

*Defined at line 17305*

---

## tc_core_only_builtin

RT8: builtins only core (and the runtimes merged into it) may call:
p_malloc_uninit (a p_malloc block whose bytes are not zeroed) and core's
p_str_alloc_uninit (a counted string whose bytes are not zeroed).

```fides
pub fn tc_core_only_builtin(callee: str) -> bool {
```

**Parameters:**

- `callee`: `str`

**Returns:** `bool`

*Defined at line 17495*

---

## tc_rx_compile_ok

Whether regex_compile(re) gives a Regex with `ok` set.

```fides
pub fn tc_rx_compile_ok(re: str) -> bool {
```

**Parameters:**

- `re`: `str`

**Returns:** `bool`

*Defined at line 18610*

---

## tc_blocking_call_fix

CA10: a call that blocks the OS thread it runs on instead of parking
the task (the runtime routes file I/O, DNS and sockets through the
poller or the blocking pool; these it does not). Returns the fix to
suggest, or "" when the call does not block a worker.

```fides
pub fn tc_blocking_call_fix(c: str) -> str {
```

**Parameters:**

- `c`: `str`

**Returns:** `str`

*Defined at line 18681*

---

## TypeChecker_lint_blocking_in_tasks

CA10 (complaint-roadmap): warn when code a green task runs (its entry
function and every function that reaches) makes a call that blocks the
worker thread. A call run through task_pool_call names its function as
a string, so it is not followed: pool-wrapped calls do not warn.

```fides
pub fn TypeChecker_lint_blocking_in_tasks(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 18698*

---

## TypeChecker_check_capability

```fides
pub fn TypeChecker_check_capability(self: &mut TypeChecker, cap: str, context: str) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `cap`: `str`
- `context`: `str`

*Defined at line 18894*

---

## TypeChecker_check_visibility

```fides
pub fn TypeChecker_check_visibility(self: &mut TypeChecker, name: str) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `name`: `str`

*Defined at line 18937*

---

## TypeChecker_collect_capabilities

```fides
pub fn TypeChecker_collect_capabilities(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 19003*

---

## TypeChecker_is_linear_type

```fides
pub fn TypeChecker_is_linear_type(self: &TypeChecker, ty: Type) -> bool {
```

**Parameters:**

- `self`: `&TypeChecker`
- `ty`: `Type`

**Returns:** `bool`

*Defined at line 19022*

---

## tc_swizzle_result_type

```fides
pub fn tc_swizzle_result_type(field: str) -> OptType {
```

**Parameters:**

- `field`: `str`

**Returns:** `OptType`

*Defined at line 19069*

---

## tc_is_vec_type

```fides
pub fn tc_is_vec_type(name: str) -> bool {
```

**Parameters:**

- `name`: `str`

**Returns:** `bool`

*Defined at line 19104*

---

## TypeChecker_is_send_type

```fides
pub fn TypeChecker_is_send_type(self: &TypeChecker, ty: Type) -> bool {
```

**Parameters:**

- `self`: `&TypeChecker`
- `ty`: `Type`

**Returns:** `bool`

*Defined at line 19252*

---

## TypeChecker_is_sync_type

```fides
pub fn TypeChecker_is_sync_type(self: &TypeChecker, ty: Type) -> bool {
```

**Parameters:**

- `self`: `&TypeChecker`
- `ty`: `Type`

**Returns:** `bool`

*Defined at line 19474*

---

## TypeChecker_consume_var

```fides
pub fn TypeChecker_consume_var(self: &mut TypeChecker, name: str, line: i64) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `name`: `str`
- `line`: `i64`

*Defined at line 19562*

---

## TypeChecker_consume_linear_in_expr

```fides
pub fn TypeChecker_consume_linear_in_expr(self: &mut TypeChecker, expr: Expr, line: i64) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `expr`: `Expr`
- `line`: `i64`

*Defined at line 19648*

---

## TypeChecker_check_stmt

```fides
pub fn TypeChecker_check_stmt(self: &mut TypeChecker, stmt: Stmt) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `stmt`: `Stmt`

*Defined at line 19962*

---

## TypeChecker_collect_captured_vars

```fides
pub fn TypeChecker_collect_captured_vars(self: &TypeChecker, expr: Expr, cap_names: &mut Vec<str>, cap_types: &mut Vec<Type>) {
```

**Parameters:**

- `self`: `&TypeChecker`
- `expr`: `Expr`
- `cap_names`: `&mut Vec<str>`
- `cap_types`: `&mut Vec<Type>`

*Defined at line 21323*

---

## TypeChecker_check_expr

```fides
pub fn TypeChecker_check_expr(self: &mut TypeChecker, expr: Expr) -> Type {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `expr`: `Expr`

**Returns:** `Type`

*Defined at line 21453*

---

## TypeChecker_check_expr_with_hint

```fides
pub fn TypeChecker_check_expr_with_hint(self: &mut TypeChecker, expr: Expr, hint: Type) -> Type {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `expr`: `Expr`
- `hint`: `Type`

**Returns:** `Type`

*Defined at line 21457*

---

## TypeChecker_check_method_call

```fides
pub fn TypeChecker_check_method_call(self: &mut TypeChecker, receiver: Expr, method: str, args: Vec<Expr>, hint: Type) -> Type {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `receiver`: `Expr`
- `method`: `str`
- `args`: `Vec<Expr>`
- `hint`: `Type`

**Returns:** `Type`

*Defined at line 23642*

---

## TypeChecker_bare_pattern_is_unit_variant

```fides
pub fn TypeChecker_bare_pattern_is_unit_variant(self: &TypeChecker, match_ty: Type, name: str) -> bool {
```

**Parameters:**

- `self`: `&TypeChecker`
- `match_ty`: `Type`
- `name`: `str`

**Returns:** `bool`

*Defined at line 24687*

---

## TypeChecker_resolve_pattern_enum

```fides
pub fn TypeChecker_resolve_pattern_enum(self: &TypeChecker, match_ty: Type, enum_name: str, variant: str) -> OptString {
```

**Parameters:**

- `self`: `&TypeChecker`
- `match_ty`: `Type`
- `enum_name`: `str`
- `variant`: `str`

**Returns:** `OptString`

*Defined at line 24724*

---

## type_from_annotation

```fides
pub fn type_from_annotation(name: str) -> Type {
```

**Parameters:**

- `name`: `str`

**Returns:** `Type`

*Defined at line 24801*

---

## is_runtime_layer

```fides
pub fn is_runtime_layer(module: str) -> bool {
```

**Parameters:**

- `module`: `str`

**Returns:** `bool`

*Defined at line 24852*

---

## is_handle_int_pair

```fides
pub fn is_handle_int_pair(a: Type, b: Type) -> bool {
```

**Parameters:**

- `a`: `Type`
- `b`: `Type`

**Returns:** `bool`

*Defined at line 24885*

---

## TypeChecker_is_runtime_call

```fides
pub fn TypeChecker_is_runtime_call(self: &TypeChecker, callee: str) -> bool {
```

**Parameters:**

- `self`: `&TypeChecker`
- `callee`: `str`

**Returns:** `bool`

*Defined at line 24921*

---

## TypeChecker_arg_compatible

```fides
pub fn TypeChecker_arg_compatible(self: &TypeChecker, param: Type, arg: Type) -> bool {
```

**Parameters:**

- `self`: `&TypeChecker`
- `param`: `Type`
- `arg`: `Type`

**Returns:** `bool`

*Defined at line 24957*

---

## TypeChecker_shares_repr

```fides
pub fn TypeChecker_shares_repr(self: &TypeChecker, t: Type) -> bool {
```

**Parameters:**

- `self`: `&TypeChecker`
- `t`: `Type`

**Returns:** `bool`

*Defined at line 25094*

---

## TypeChecker_check_return_type

```fides
pub fn TypeChecker_check_return_type(self: &mut TypeChecker, actual: Type) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `actual`: `Type`

*Defined at line 25125*

---

## TypeChecker_check_enum_constructor

```fides
pub fn TypeChecker_check_enum_constructor(self: &mut TypeChecker, enum_name: str, variant: str, arg_tys: Vec<Type>, args: Vec<Expr>) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `enum_name`: `str`
- `variant`: `str`
- `arg_tys`: `Vec<Type>`
- `args`: `Vec<Expr>`

*Defined at line 25186*

---

## TypeChecker_check_block_value

```fides
pub fn TypeChecker_check_block_value(self: &mut TypeChecker, stmts: Vec<Stmt>) -> Type {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `stmts`: `Vec<Stmt>`

**Returns:** `Type`

*Defined at line 25711*

---

## TypeChecker_check_stmts_value

```fides
pub fn TypeChecker_check_stmts_value(self: &mut TypeChecker, stmts: Vec<Stmt>) -> Type {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `stmts`: `Vec<Stmt>`

**Returns:** `Type`

*Defined at line 25721*

---

## unbox_ty

```fides
pub fn unbox_ty(t: Type) -> Type {
```

**Parameters:**

- `t`: `Type`

**Returns:** `Type`

*Defined at line 25774*

---

## is_ffi_pointer_pair

```fides
pub fn is_ffi_pointer_pair(param: Type, arg: Type) -> bool {
```

**Parameters:**

- `param`: `Type`
- `arg`: `Type`

**Returns:** `bool`

*Defined at line 25930*

---

## TypeChecker_enum_params_of

```fides
pub fn TypeChecker_enum_params_of(self: &TypeChecker, name: str) -> Vec<str> {
```

**Parameters:**

- `self`: `&TypeChecker`
- `name`: `str`

**Returns:** `Vec<str>`

*Defined at line 25960*

---

## type_mentions_any

```fides
pub fn type_mentions_any(t: Type, params: Vec<str>) -> bool {
```

**Parameters:**

- `t`: `Type`
- `params`: `Vec<str>`

**Returns:** `bool`

*Defined at line 25972*

---

## TypeChecker_is_enum_type_param

```fides
pub fn TypeChecker_is_enum_type_param(self: &TypeChecker, t: Type) -> bool {
```

**Parameters:**

- `self`: `&TypeChecker`
- `t`: `Type`

**Returns:** `bool`

*Defined at line 26004*

---

## TypeChecker_check_enum_pattern

```fides
pub fn TypeChecker_check_enum_pattern(self: &mut TypeChecker, match_ty: Type, enum_name: str, variant: str, bindings: Vec<Binding>) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `match_ty`: `Type`
- `enum_name`: `str`
- `variant`: `str`
- `bindings`: `Vec<Binding>`

*Defined at line 26025*

---

## TypeChecker_pattern_binding_types

```fides
pub fn TypeChecker_pattern_binding_types(self: &TypeChecker, match_ty: Type, enum_name: str, variant: str, bindings: Vec<Binding>) -> Vec<Type> {
```

**Parameters:**

- `self`: `&TypeChecker`
- `match_ty`: `Type`
- `enum_name`: `str`
- `variant`: `str`
- `bindings`: `Vec<Binding>`

**Returns:** `Vec<Type>`

*Defined at line 26212*

---

## TypeChecker_scan_stmts_for_effects

```fides
pub fn TypeChecker_scan_stmts_for_effects(self: &TypeChecker, stmts: Vec<Stmt>, effects: &mut Vec<str>) {
```

**Parameters:**

- `self`: `&TypeChecker`
- `stmts`: `Vec<Stmt>`
- `effects`: `&mut Vec<str>`

*Defined at line 26544*

---

## TypeChecker_scan_stmt_for_effects

```fides
pub fn TypeChecker_scan_stmt_for_effects(self: &TypeChecker, stmt: Stmt, effects: &mut Vec<str>) {
```

**Parameters:**

- `self`: `&TypeChecker`
- `stmt`: `Stmt`
- `effects`: `&mut Vec<str>`

*Defined at line 26552*

---

## TypeChecker_scan_expr_for_effects

```fides
pub fn TypeChecker_scan_expr_for_effects(self: &TypeChecker, expr: Expr, effects: &mut Vec<str>) {
```

**Parameters:**

- `self`: `&TypeChecker`
- `expr`: `Expr`
- `effects`: `&mut Vec<str>`

*Defined at line 26720*

---

## TypeChecker_lint_unused_functions

```fides
pub fn TypeChecker_lint_unused_functions(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 27037*

---

## TypeChecker_collect_called_funcs

```fides
pub fn TypeChecker_collect_called_funcs(self: &TypeChecker, stmts: Vec<Stmt>, called: &mut Vec<str>) {
```

**Parameters:**

- `self`: `&TypeChecker`
- `stmts`: `Vec<Stmt>`
- `called`: `&mut Vec<str>`

*Defined at line 27102*

---

## TypeChecker_collect_called_funcs_stmt

```fides
pub fn TypeChecker_collect_called_funcs_stmt(self: &TypeChecker, stmt: Stmt, called: &mut Vec<str>) {
```

**Parameters:**

- `self`: `&TypeChecker`
- `stmt`: `Stmt`
- `called`: `&mut Vec<str>`

*Defined at line 27110*

---

## TypeChecker_collect_called_funcs_expr

```fides
pub fn TypeChecker_collect_called_funcs_expr(self: &TypeChecker, expr: Expr, called: &mut Vec<str>) {
```

**Parameters:**

- `self`: `&TypeChecker`
- `expr`: `Expr`
- `called`: `&mut Vec<str>`

*Defined at line 27251*

---

## TypeChecker_lint_unused_imports

```fides
pub fn TypeChecker_lint_unused_imports(self: &mut TypeChecker, program: Program) {
```

**Parameters:**

- `self`: `&mut TypeChecker`
- `program`: `Program`

*Defined at line 27451*

---

## scan_for_floats

```fides
pub fn scan_for_floats(stmts: Vec<Stmt>) -> Vec<str> {
```

**Parameters:**

- `stmts`: `Vec<Stmt>`

**Returns:** `Vec<str>`

*Defined at line 27538*

---

## scan_stmt_for_floats

```fides
pub fn scan_stmt_for_floats(stmt: Stmt, found: &mut Vec<str>) {
```

**Parameters:**

- `stmt`: `Stmt`
- `found`: `&mut Vec<str>`

*Defined at line 27548*

---

## scan_expr_for_floats

```fides
pub fn scan_expr_for_floats(expr: Expr, found: &mut Vec<str>) {
```

**Parameters:**

- `expr`: `Expr`
- `found`: `&mut Vec<str>`

*Defined at line 27765*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
