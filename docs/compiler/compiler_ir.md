[All modules](index.md)

# compiler::ir -- Fides Documentation

## Contents

- [ValueIdType](#ValueIdType) (struct)
- [ValueIdType_new](#ValueIdType_new) (function)
- [StringIrType](#StringIrType) (struct)
- [StringIrType_new](#StringIrType_new) (function)
- [StringValueId](#StringValueId) (struct)
- [StringValueId_new](#StringValueId_new) (function)
- [ValueIdString](#ValueIdString) (struct)
- [ValueIdString_new](#ValueIdString_new) (function)
- [ValueIdU64](#ValueIdU64) (struct)
- [ValueIdU64_new](#ValueIdU64_new) (function)
- [StringVecIrType](#StringVecIrType) (struct)
- [StringVecIrType_new](#StringVecIrType_new) (function)
- [StringVecStr](#StringVecStr) (struct)
- [StringVecStr_new](#StringVecStr_new) (function)
- [StringIrTypeOffset](#StringIrTypeOffset) (struct)
- [StringIrTypeOffset_new](#StringIrTypeOffset_new) (function)
- [IrStringValueIdEntry](#IrStringValueIdEntry) (struct)
- [IrStringValueIdEntry_new](#IrStringValueIdEntry_new) (function)
- [OptIrType](#OptIrType) (enum)
- [OptValueId](#OptValueId) (enum)
- [OptU64](#OptU64) (enum)
- [OptString](#OptString) (enum)
- [OptInst](#OptInst) (enum)
- [OptValueMeta](#OptValueMeta) (enum)
- [OptStructLayout](#OptStructLayout) (enum)
- [OptIrFunction](#OptIrFunction) (enum)
- [OptIrModule](#OptIrModule) (enum)
- [IrType](#IrType) (enum)
- [ir_type_size](#ir_type_size) (function)
- [box_ir_type_size](#box_ir_type_size) (function)
- [ir_type_align](#ir_type_align) (function)
- [box_ir_type_align](#box_ir_type_align) (function)
- [ir_type_is_int](#ir_type_is_int) (function)
- [ir_type_is_float](#ir_type_is_float) (function)
- [ir_type_is_ptr](#ir_type_is_ptr) (function)
- [ir_type_is_ptr_like](#ir_type_is_ptr_like) (function)
- [ir_type_is_opaque_named](#ir_type_is_opaque_named) (function)
- [ir_type_generic_elem](#ir_type_generic_elem) (function)
- [ir_type_generic_args](#ir_type_generic_args) (function)
- [ir_type_is_signed](#ir_type_is_signed) (function)
- [ir_type_is_unsigned](#ir_type_is_unsigned) (function)
- [ir_type_is_numeric](#ir_type_is_numeric) (function)
- [ir_type_is_scalar](#ir_type_is_scalar) (function)
- [ir_type_is_aggregate](#ir_type_is_aggregate) (function)
- [ir_type_is_collection](#ir_type_is_collection) (function)
- [ir_type_is_never](#ir_type_is_never) (function)
- [ir_type_is_void](#ir_type_is_void) (function)
- [ir_type_collection_elem](#ir_type_collection_elem) (function)
- [ir_type_map_key_type](#ir_type_map_key_type) (function)
- [ir_type_map_value_type](#ir_type_map_value_type) (function)
- [box_ir_type_eq](#box_ir_type_eq) (function)
- [box_ir_type_unbox](#box_ir_type_unbox) (function)
- [ir_type_eq](#ir_type_eq) (function)
- [ir_type_is_compatible_with](#ir_type_is_compatible_with) (function)
- [box_ir_type_is_compatible_with](#box_ir_type_is_compatible_with) (function)
- [ir_type_bit_width](#ir_type_bit_width) (function)
- [ir_type_to_str](#ir_type_to_str) (function)
- [box_ir_type_to_str](#box_ir_type_to_str) (function)
- [Effect](#Effect) (enum)
- [effect_to_str](#effect_to_str) (function)
- [effect_eq](#effect_eq) (function)
- [EffectSet](#EffectSet) (struct)
- [EffectSet_new](#EffectSet_new) (function)
- [EffectSet_none](#EffectSet_none) (function)
- [EffectSet_add](#EffectSet_add) (function)
- [EffectSet_add_all](#EffectSet_add_all) (function)
- [EffectSet_contains](#EffectSet_contains) (function)
- [EffectSet_is_pure](#EffectSet_is_pure) (function)
- [EffectSet_is_read_only](#EffectSet_is_read_only) (function)
- [EffectSet_has_side_effects](#EffectSet_has_side_effects) (function)
- [EffectSet_parse_effect](#EffectSet_parse_effect) (function)
- [EffectSet_to_str](#EffectSet_to_str) (function)
- [Region](#Region) (enum)
- [region_to_str](#region_to_str) (function)
- [region_eq](#region_eq) (function)
- [BorrowKind](#BorrowKind) (enum)
- [borrow_kind_eq](#borrow_kind_eq) (function)
- [value_id_to_str](#value_id_to_str) (function)
- [block_id_to_str](#block_id_to_str) (function)
- [ValueMeta](#ValueMeta) (struct)
- [ValueMeta_new](#ValueMeta_new) (function)
- [ValueMeta_with_type](#ValueMeta_with_type) (function)
- [Inst](#Inst) (enum)
- [inst_result_type](#inst_result_type) (function)
- [inst_effects](#inst_effects) (function)
- [inst_kind_name](#inst_kind_name) (function)
- [inst_is_terminator](#inst_is_terminator) (function)
- [inst_value_refs](#inst_value_refs) (function)
- [BasicBlock](#BasicBlock) (struct)
- [BasicBlock_new](#BasicBlock_new) (function)
- [IrFunction](#IrFunction) (struct)
- [ValueIdMeta](#ValueIdMeta) (struct)
- [ValueIdMeta_new](#ValueIdMeta_new) (function)
- [ValueIdInst](#ValueIdInst) (struct)
- [ValueIdInst_new](#ValueIdInst_new) (function)
- [IrFunction_new](#IrFunction_new) (function)
- [IrFunction_new_value](#IrFunction_new_value) (function)
- [IrFunction_new_block](#IrFunction_new_block) (function)
- [IrFunction_current_block](#IrFunction_current_block) (function)
- [IrFunction_emit](#IrFunction_emit) (function)
- [IrFunction_emit_line](#IrFunction_emit_line) (function)
- [IrFunction_add_block_param](#IrFunction_add_block_param) (function)
- [IrFunction_bind_local](#IrFunction_bind_local) (function)
- [IrFunction_lookup_local](#IrFunction_lookup_local) (function)
- [IrFunction_get_inst](#IrFunction_get_inst) (function)
- [IrFunction_get_meta](#IrFunction_get_meta) (function)
- [StructLayout](#StructLayout) (struct)
- [StructLayout_new](#StructLayout_new) (function)
- [SourceEntry](#SourceEntry) (struct)
- [SourceEntry_new](#SourceEntry_new) (function)
- [SourceMap](#SourceMap) (struct)
- [SourceMap_new](#SourceMap_new) (function)
- [SourceMap_lookup](#SourceMap_lookup) (function)
- [SourceMap_to_str](#SourceMap_to_str) (function)
- [IrModule](#IrModule) (struct)
- [IrFunctionEntry](#IrFunctionEntry) (struct)
- [IrFunctionEntry_new](#IrFunctionEntry_new) (function)
- [StructLayoutEntry](#StructLayoutEntry) (struct)
- [StructLayoutEntry_new](#StructLayoutEntry_new) (function)
- [StringStringEntry](#StringStringEntry) (struct)
- [StringStringEntry_new](#StringStringEntry_new) (function)
- [StringEffectSetEntry](#StringEffectSetEntry) (struct)
- [StringEffectSetEntry_new](#StringEffectSetEntry_new) (function)
- [GlobalEntry](#GlobalEntry) (struct)
- [GlobalEntry_new](#GlobalEntry_new) (function)
- [IrModule_get_global](#IrModule_get_global) (function)
- [IrModule_global_pos](#IrModule_global_pos) (function)
- [OptGlobalLookup](#OptGlobalLookup) (enum)
- [IrModule_new](#IrModule_new) (function)
- [c_fn_name](#c_fn_name) (function)
- [c_fn_target](#c_fn_target) (function)
- [IrModule_add_struct](#IrModule_add_struct) (function)
- [IrModule_get_struct](#IrModule_get_struct) (function)
- [IrModule_intern_string](#IrModule_intern_string) (function)
- [IrModule_add_extern](#IrModule_add_extern) (function)
- [IrModule_type_size](#IrModule_type_size) (function)
- [IrModule_type_align](#IrModule_type_align) (function)
- [IrModule_add_function](#IrModule_add_function) (function)
- [IrModule_replace_function](#IrModule_replace_function) (function)
- [IrModule_replace_function_named](#IrModule_replace_function_named) (function)
- [IrModule_get_function](#IrModule_get_function) (function)
- [IrModule_fn_pos](#IrModule_fn_pos) (function)
- [IrModule_effects_pos](#IrModule_effects_pos) (function)
- [lower_type](#lower_type) (function)
- [box_lower_type](#box_lower_type) (function)
- [IrModule_optimize](#IrModule_optimize) (function)
- [PeirStrPair](#PeirStrPair) (struct)
- [pe_strs_contains](#pe_strs_contains) (function)
- [pe_sort_strs](#pe_sort_strs) (function)
- [pe_stub_deps](#pe_stub_deps) (function)
- [pe_print_builtins](#pe_print_builtins) (function)
- [is_pe_runtime_fn](#is_pe_runtime_fn) (function)
- [pert_name](#pert_name) (function)
- [pe_runtime_alias](#pe_runtime_alias) (function)
- [pe_runtime_alias_target](#pe_runtime_alias_target) (function)
- [pe_module_has_fn](#pe_module_has_fn) (function)
- [pe_ir_c_override](#pe_ir_c_override) (function)
- [pe_ir_resolve_callee](#pe_ir_resolve_callee) (function)
- [pe_ir_print_formatter](#pe_ir_print_formatter) (function)
- [pe_ir_autograd_builtin](#pe_ir_autograd_builtin) (function)
- [pe_ir_reachable_from](#pe_ir_reachable_from) (function)
- [pe_ir_needed_functions](#pe_ir_needed_functions) (function)
- [pe_ir_reachable_functions](#pe_ir_reachable_functions) (function)
- [ir_dfe_root](#ir_dfe_root) (function)
- [ir_data_named_fns](#ir_data_named_fns) (function)
- [IrModule_dead_global_elimination](#IrModule_dead_global_elimination) (function)
- [IrModule_dead_function_elimination](#IrModule_dead_function_elimination) (function)
- [IrFunction_set_inst](#IrFunction_set_inst) (function)
- [IrFunction_set_meta](#IrFunction_set_meta) (function)
- [VerificationResult](#VerificationResult) (struct)
- [VerificationResult_new](#VerificationResult_new) (function)
- [VerificationResult_is_valid](#VerificationResult_is_valid) (function)
- [IrModule_verify](#IrModule_verify) (function)
- [IrModule_to_str](#IrModule_to_str) (function)
- [ir_binop_str](#ir_binop_str) (function)
- [ir_unop_str](#ir_unop_str) (function)
- [inst_detail_str](#inst_detail_str) (function)
- [IrFunction_detail_str](#IrFunction_detail_str) (function)
- [IrModule_function_detail_str](#IrModule_function_detail_str) (function)
- [IrModule_source_map](#IrModule_source_map) (function)
- [ptr_u8](#ptr_u8) (function)
- [c_vec1_i64](#c_vec1_i64) (function)
- [ResultBytes](#ResultBytes) (enum)
- [c_vec1](#c_vec1) (function)
- [c_file_exists](#c_file_exists) (function)
- [ir_note_fn_file](#ir_note_fn_file) (function)
- [ir_fn_file](#ir_fn_file) (function)
- [ir_note_program_files](#ir_note_program_files) (function)
- [ir_note_lib_program](#ir_note_lib_program) (function)
- [ir_lib_file_pairs](#ir_lib_file_pairs) (function)
- [ir_note_lib_pairs](#ir_note_lib_pairs) (function)
- [ir_lib_fn_file](#ir_lib_fn_file) (function)
- [ir_note_fn_clone](#ir_note_fn_clone) (function)
- [ir_fn_src_file](#ir_fn_src_file) (function)
- [ir_value_file_rec](#ir_value_file_rec) (function)
- [ir_fn_has_value_files](#ir_fn_has_value_files) (function)
- [ir_value_file](#ir_value_file) (function)
- [ir_copy_value_file](#ir_copy_value_file) (function)
- [ir_value_files_reset](#ir_value_files_reset) (function)
- [ir_switch_on](#ir_switch_on) (function)
- [switch_succ_blocks](#switch_succ_blocks) (function)
- [inst_succ_blocks](#inst_succ_blocks) (function)
- [IrFunction_form_switches](#IrFunction_form_switches) (function)
- [IrModule_form_switches](#IrModule_form_switches) (function)
- [IrFunction_has_switch](#IrFunction_has_switch) (function)
- [IrFunction_expand_switches](#IrFunction_expand_switches) (function)
- [IrFunction_without_switches](#IrFunction_without_switches) (function)
- [SWP_EQ:](#SWP_EQ:) (const)
- [SWP_LT:](#SWP_LT:) (const)
- [SWP_BITS:](#SWP_BITS:) (const)
- [SWP_LABEL:](#SWP_LABEL:) (const)
- [SWP_JMP:](#SWP_JMP:) (const)
- [ir_switch_plan](#ir_switch_plan) (function)

## ValueIdType

```fides
pub struct ValueIdType {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `ty` | `IrType` |

*Defined at line 43*

---

## ValueIdType_new

```fides
pub fn ValueIdType_new(id: i64, ty: IrType) -> ValueIdType {
```

**Parameters:**

- `id`: `i64`
- `ty`: `IrType`

**Returns:** `ValueIdType`

*Defined at line 48*

---

## StringIrType

```fides
pub struct StringIrType {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `ty` | `IrType` |

*Defined at line 54*

---

## StringIrType_new

```fides
pub fn StringIrType_new(name: str, ty: IrType) -> StringIrType {
```

**Parameters:**

- `name`: `str`
- `ty`: `IrType`

**Returns:** `StringIrType`

*Defined at line 59*

---

## StringValueId

```fides
pub struct StringValueId {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `id` | `i64` |

*Defined at line 65*

---

## StringValueId_new

```fides
pub fn StringValueId_new(name: str, id: i64) -> StringValueId {
```

**Parameters:**

- `name`: `str`
- `id`: `i64`

**Returns:** `StringValueId`

*Defined at line 70*

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

*Defined at line 76*

---

## ValueIdString_new

```fides
pub fn ValueIdString_new(id: i64, name: str) -> ValueIdString {
```

**Parameters:**

- `id`: `i64`
- `name`: `str`

**Returns:** `ValueIdString`

*Defined at line 81*

---

## ValueIdU64

```fides
pub struct ValueIdU64 {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `val` | `i64` |

*Defined at line 87*

---

## ValueIdU64_new

```fides
pub fn ValueIdU64_new(id: i64, val: i64) -> ValueIdU64 {
```

**Parameters:**

- `id`: `i64`
- `val`: `i64`

**Returns:** `ValueIdU64`

*Defined at line 92*

---

## StringVecIrType

```fides
pub struct StringVecIrType {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `params` | `Vec<IrType>` |
| `ret` | `IrType` |

*Defined at line 98*

---

## StringVecIrType_new

```fides
pub fn StringVecIrType_new(name: str, params: Vec<IrType>, ret: IrType) -> StringVecIrType {
```

**Parameters:**

- `name`: `str`
- `params`: `Vec<IrType>`
- `ret`: `IrType`

**Returns:** `StringVecIrType`

*Defined at line 104*

---

## StringVecStr

```fides
pub struct StringVecStr {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `items` | `Vec<str>` |

*Defined at line 110*

---

## StringVecStr_new

```fides
pub fn StringVecStr_new(name: str, items: Vec<str>) -> StringVecStr {
```

**Parameters:**

- `name`: `str`
- `items`: `Vec<str>`

**Returns:** `StringVecStr`

*Defined at line 115*

---

## StringIrTypeOffset

```fides
pub struct StringIrTypeOffset {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `ty` | `IrType` |
| `offset` | `i64` |

*Defined at line 121*

---

## StringIrTypeOffset_new

```fides
pub fn StringIrTypeOffset_new(name: str, ty: IrType, offset: i64) -> StringIrTypeOffset {
```

**Parameters:**

- `name`: `str`
- `ty`: `IrType`
- `offset`: `i64`

**Returns:** `StringIrTypeOffset`

*Defined at line 127*

---

## IrStringValueIdEntry

```fides
pub struct IrStringValueIdEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `id` | `i64` |

*Defined at line 133*

---

## IrStringValueIdEntry_new

```fides
pub fn IrStringValueIdEntry_new(name: str, id: i64) -> IrStringValueIdEntry {
```

**Parameters:**

- `name`: `str`
- `id`: `i64`

**Returns:** `IrStringValueIdEntry`

*Defined at line 138*

---

## OptIrType

```fides
pub enum OptIrType {
```

**Variants:**

- `Some`
- `None`

*Defined at line 146*

---

## OptValueId

```fides
pub enum OptValueId {
```

**Variants:**

- `Some`
- `None`

*Defined at line 151*

---

## OptU64

```fides
pub enum OptU64 {
```

**Variants:**

- `Some`
- `None`

*Defined at line 156*

---

## OptString

```fides
pub enum OptString {
```

**Variants:**

- `Some`
- `None`

*Defined at line 161*

---

## OptInst

```fides
pub enum OptInst {
```

**Variants:**

- `Some`
- `None`

*Defined at line 168*

---

## OptValueMeta

```fides
pub enum OptValueMeta {
```

**Variants:**

- `Some`
- `None`

*Defined at line 173*

---

## OptStructLayout

```fides
pub enum OptStructLayout {
```

**Variants:**

- `Some`
- `None`

*Defined at line 178*

---

## OptIrFunction

```fides
pub enum OptIrFunction {
```

**Variants:**

- `Some`
- `None`

*Defined at line 183*

---

## OptIrModule

```fides
pub enum OptIrModule {
```

**Variants:**

- `Some`
- `None`

*Defined at line 188*

---

## IrType

```fides
pub enum IrType {
```

**Variants:**

- `I8`
- `I16`
- `I32`
- `I64`
- `U8`
- `U16`
- `U32`
- `U64`
- `F32`
- `F64`
- `Bool`
- `Ptr`
- `Struct`
- `Array`
- `Void`
- `Never`
- `Opaque`
- `Generic`

*Defined at line 195*

---

## ir_type_size

```fides
pub fn ir_type_size(t: IrType) -> i64 {
```

**Parameters:**

- `t`: `IrType`

**Returns:** `i64`

*Defined at line 218*

---

## box_ir_type_size

```fides
pub fn box_ir_type_size(b: Box<IrType>) -> i64 {
```

**Parameters:**

- `b`: `Box<IrType>`

**Returns:** `i64`

*Defined at line 280*

---

## ir_type_align

```fides
pub fn ir_type_align(t: IrType) -> i64 {
```

**Parameters:**

- `t`: `IrType`

**Returns:** `i64`

*Defined at line 341*

---

## box_ir_type_align

```fides
pub fn box_ir_type_align(b: Box<IrType>) -> i64 {
```

**Parameters:**

- `b`: `Box<IrType>`

**Returns:** `i64`

*Defined at line 401*

---

## ir_type_is_int

```fides
pub fn ir_type_is_int(t: IrType) -> bool {
```

**Parameters:**

- `t`: `IrType`

**Returns:** `bool`

*Defined at line 461*

---

## ir_type_is_float

```fides
pub fn ir_type_is_float(t: IrType) -> bool {
```

**Parameters:**

- `t`: `IrType`

**Returns:** `bool`

*Defined at line 497*

---

## ir_type_is_ptr

```fides
pub fn ir_type_is_ptr(t: IrType) -> bool {
```

**Parameters:**

- `t`: `IrType`

**Returns:** `bool`

*Defined at line 512*

---

## ir_type_is_ptr_like

```fides
pub fn ir_type_is_ptr_like(t: IrType) -> bool {
```

**Parameters:**

- `t`: `IrType`

**Returns:** `bool`

*Defined at line 524*

---

## ir_type_is_opaque_named

```fides
pub fn ir_type_is_opaque_named(t: IrType, name: str) -> bool {
```

**Parameters:**

- `t`: `IrType`
- `name`: `str`

**Returns:** `bool`

*Defined at line 542*

---

## ir_type_generic_elem

```fides
pub fn ir_type_generic_elem(t: IrType) -> OptIrType {
```

**Parameters:**

- `t`: `IrType`

**Returns:** `OptIrType`

*Defined at line 557*

---

## ir_type_generic_args

```fides
pub fn ir_type_generic_args(t: IrType) -> Vec<IrType> {
```

**Parameters:**

- `t`: `IrType`

**Returns:** `Vec<IrType>`

*Defined at line 572*

---

## ir_type_is_signed

```fides
pub fn ir_type_is_signed(t: IrType) -> bool {
```

**Parameters:**

- `t`: `IrType`

**Returns:** `bool`

*Defined at line 584*

---

## ir_type_is_unsigned

```fides
pub fn ir_type_is_unsigned(t: IrType) -> bool {
```

**Parameters:**

- `t`: `IrType`

**Returns:** `bool`

*Defined at line 605*

---

## ir_type_is_numeric

```fides
pub fn ir_type_is_numeric(t: IrType) -> bool {
```

**Parameters:**

- `t`: `IrType`

**Returns:** `bool`

*Defined at line 626*

---

## ir_type_is_scalar

```fides
pub fn ir_type_is_scalar(t: IrType) -> bool {
```

**Parameters:**

- `t`: `IrType`

**Returns:** `bool`

*Defined at line 630*

---

## ir_type_is_aggregate

```fides
pub fn ir_type_is_aggregate(t: IrType) -> bool {
```

**Parameters:**

- `t`: `IrType`

**Returns:** `bool`

*Defined at line 634*

---

## ir_type_is_collection

```fides
pub fn ir_type_is_collection(t: IrType) -> bool {
```

**Parameters:**

- `t`: `IrType`

**Returns:** `bool`

*Defined at line 655*

---

## ir_type_is_never

```fides
pub fn ir_type_is_never(t: IrType) -> bool {
```

**Parameters:**

- `t`: `IrType`

**Returns:** `bool`

*Defined at line 659*

---

## ir_type_is_void

```fides
pub fn ir_type_is_void(t: IrType) -> bool {
```

**Parameters:**

- `t`: `IrType`

**Returns:** `bool`

*Defined at line 663*

---

## ir_type_collection_elem

```fides
pub fn ir_type_collection_elem(t: IrType) -> OptIrType {
```

**Parameters:**

- `t`: `IrType`

**Returns:** `OptIrType`

*Defined at line 667*

---

## ir_type_map_key_type

```fides
pub fn ir_type_map_key_type(t: IrType) -> OptIrType {
```

**Parameters:**

- `t`: `IrType`

**Returns:** `OptIrType`

*Defined at line 691*

---

## ir_type_map_value_type

```fides
pub fn ir_type_map_value_type(t: IrType) -> OptIrType {
```

**Parameters:**

- `t`: `IrType`

**Returns:** `OptIrType`

*Defined at line 708*

---

## box_ir_type_eq

```fides
pub fn box_ir_type_eq(a: Box<IrType>, b: Box<IrType>) -> bool {
```

**Parameters:**

- `a`: `Box<IrType>`
- `b`: `Box<IrType>`

**Returns:** `bool`

*Defined at line 725*

---

## box_ir_type_unbox

```fides
pub fn box_ir_type_unbox(b: Box<IrType>) -> IrType {
```

**Parameters:**

- `b`: `Box<IrType>`

**Returns:** `IrType`

*Defined at line 729*

---

## ir_type_eq

```fides
pub fn ir_type_eq(a: IrType, b: IrType) -> bool {
```

**Parameters:**

- `a`: `IrType`
- `b`: `IrType`

**Returns:** `bool`

*Defined at line 789*

---

## ir_type_is_compatible_with

```fides
pub fn ir_type_is_compatible_with(a: IrType, b: IrType) -> bool {
```

**Parameters:**

- `a`: `IrType`
- `b`: `IrType`

**Returns:** `bool`

*Defined at line 953*

---

## box_ir_type_is_compatible_with

```fides
pub fn box_ir_type_is_compatible_with(a: Box<IrType>, b: Box<IrType>) -> bool {
```

**Parameters:**

- `a`: `Box<IrType>`
- `b`: `Box<IrType>`

**Returns:** `bool`

*Defined at line 1004*

---

## ir_type_bit_width

```fides
pub fn ir_type_bit_width(t: IrType) -> OptU64 {
```

**Parameters:**

- `t`: `IrType`

**Returns:** `OptU64`

*Defined at line 1052*

---

## ir_type_to_str

```fides
pub fn ir_type_to_str(t: IrType) -> str {
```

**Parameters:**

- `t`: `IrType`

**Returns:** `str`

*Defined at line 1099*

---

## box_ir_type_to_str

```fides
pub fn box_ir_type_to_str(b: Box<IrType>) -> str {
```

**Parameters:**

- `b`: `Box<IrType>`

**Returns:** `str`

*Defined at line 1171*

---

## Effect

```fides
pub enum Effect {
```

**Variants:**

- `Alloc`
- `IO`
- `Net`
- `Spawn`
- `Diverge`
- `Read`
- `Write`
- `Diff`
- `Noise`
- `Comm`

*Defined at line 1245*

---

## effect_to_str

```fides
pub fn effect_to_str(e: Effect) -> str {
```

**Parameters:**

- `e`: `Effect`

**Returns:** `str`

*Defined at line 1258*

---

## effect_eq

```fides
pub fn effect_eq(a: Effect, b: Effect) -> bool {
```

**Parameters:**

- `a`: `Effect`
- `b`: `Effect`

**Returns:** `bool`

*Defined at line 1294*

---

## EffectSet

```fides
pub struct EffectSet {
```

**Fields:**

| Name | Type |
|------|------|
| `effects` | `Vec<Effect>` |

*Defined at line 1392*

---

## EffectSet_new

```fides
pub fn EffectSet_new() -> EffectSet {
```

**Returns:** `EffectSet`

*Defined at line 1396*

---

## EffectSet_none

```fides
pub fn EffectSet_none() -> EffectSet {
```

**Returns:** `EffectSet`

*Defined at line 1402*

---

## EffectSet_add

```fides
pub fn EffectSet_add(self: &mut EffectSet, effect: Effect) {
```

**Parameters:**

- `self`: `&mut EffectSet`
- `effect`: `Effect`

*Defined at line 1408*

---

## EffectSet_add_all

```fides
pub fn EffectSet_add_all(self: &mut EffectSet, other: &EffectSet) {
```

**Parameters:**

- `self`: `&mut EffectSet`
- `other`: `&EffectSet`

*Defined at line 1414*

---

## EffectSet_contains

```fides
pub fn EffectSet_contains(self: &EffectSet, effect: Effect) -> bool {
```

**Parameters:**

- `self`: `&EffectSet`
- `effect`: `Effect`

**Returns:** `bool`

*Defined at line 1422*

---

## EffectSet_is_pure

```fides
pub fn EffectSet_is_pure(self: &EffectSet) -> bool {
```

**Parameters:**

- `self`: `&EffectSet`

**Returns:** `bool`

*Defined at line 1433*

---

## EffectSet_is_read_only

```fides
pub fn EffectSet_is_read_only(self: &EffectSet) -> bool {
```

**Parameters:**

- `self`: `&EffectSet`

**Returns:** `bool`

*Defined at line 1437*

---

## EffectSet_has_side_effects

```fides
pub fn EffectSet_has_side_effects(self: &EffectSet) -> bool {
```

**Parameters:**

- `self`: `&EffectSet`

**Returns:** `bool`

*Defined at line 1452*

---

## EffectSet_parse_effect

```fides
pub fn EffectSet_parse_effect(s: str) -> OptU64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `OptU64`

*Defined at line 1467*

---

## EffectSet_to_str

```fides
pub fn EffectSet_to_str(self: &EffectSet) -> str {
```

**Parameters:**

- `self`: `&EffectSet`

**Returns:** `str`

*Defined at line 1493*

---

## Region

```fides
pub enum Region {
```

**Variants:**

- `Stack`
- `Heap`
- `Static`
- `Named`

*Defined at line 1511*

---

## region_to_str

```fides
pub fn region_to_str(r: Region) -> str {
```

**Parameters:**

- `r`: `Region`

**Returns:** `str`

*Defined at line 1518*

---

## region_eq

```fides
pub fn region_eq(a: Region, b: Region) -> bool {
```

**Parameters:**

- `a`: `Region`
- `b`: `Region`

**Returns:** `bool`

*Defined at line 1536*

---

## BorrowKind

```fides
pub enum BorrowKind {
```

**Variants:**

- `None`
- `Shared`
- `Mut`

*Defined at line 1582*

---

## borrow_kind_eq

```fides
pub fn borrow_kind_eq(a: BorrowKind, b: BorrowKind) -> bool {
```

**Parameters:**

- `a`: `BorrowKind`
- `b`: `BorrowKind`

**Returns:** `bool`

*Defined at line 1588*

---

## value_id_to_str

```fides
pub fn value_id_to_str(id: i64) -> str {
```

**Parameters:**

- `id`: `i64`

**Returns:** `str`

*Defined at line 1623*

---

## block_id_to_str

```fides
pub fn block_id_to_str(id: i64) -> str {
```

**Parameters:**

- `id`: `i64`

**Returns:** `str`

*Defined at line 1627*

---

## ValueMeta

```fides
pub struct ValueMeta {
```

**Fields:**

| Name | Type |
|------|------|
| `ty` | `IrType` |
| `region` | `Region` |
| `effects` | `EffectSet` |
| `source_line` | `OptU64` |
| `in_sandbox` | `bool` |
| `is_linear` | `bool` |
| `borrow_kind` | `BorrowKind` |

*Defined at line 1633*

---

## ValueMeta_new

```fides
pub fn ValueMeta_new(ty: IrType) -> ValueMeta {
```

**Parameters:**

- `ty`: `IrType`

**Returns:** `ValueMeta`

*Defined at line 1643*

---

## ValueMeta_with_type

```fides
pub fn ValueMeta_with_type(old: ValueMeta, new_ty: IrType) -> ValueMeta {
```

**Parameters:**

- `old`: `ValueMeta`
- `new_ty`: `IrType`

**Returns:** `ValueMeta`

*Defined at line 1649*

---

## Inst

```fides
pub enum Inst {
```

**Variants:**

- `// --- Constants ---`
- `ConstInt`
- `ConstFloat`
- `ConstBool`
- `ConstStr`
- `ConstFnRef`
- `ConstNull`
- `// --- Globals ---`
- `// Get the address of a module-level mutable global variable.`
- `// Returns a pointer that can be used with Load/Store.`
- `GlobalAddr`
- `// --- Arithmetic ---`
- `BinOp`
- `UnaryOp`
- `// --- Memory ---`
- `Alloca`
- `Load`
- `Store`
- `GetFieldPtr`
- `GetElemPtr`
- `// --- Control Flow ---`
- `Jump`
- `Branch`
- `// ME28/MI27: multi-way branch on an integer:`
- `// default); keys distinct, targets[i] is the block for keys[i]. Formed`
- `// from compare chains by IrFunction_form_switches, after the mid-end.`
- `Switch`
- `Return`
- `Unreachable`
- `// --- Function Calls ---`
- `Call`
- `CallPtr`
- `DynCall`
- `MakeDyn`
- `// --- Conversions ---`
- `BitCast`
- `IntExtend`
- `IntTrunc`
- `IntToFloat`
- `FloatToInt`
- `// --- Fides-Specific ---`
- `TryUnwrap`
- `Print`
- `Panic`
- `Asm`
- `// --- Region Management ---`
- `RegionBegin`
- `RegionEnd`
- `// --- Persist Variables ---`
- `PersistLoad`
- `PersistSave`
- `// --- Transaction Blocks ---`
- `TxnBegin`
- `TxnEnd`
- `// --- Unsafe Blocks ---`
- `UnsafeBegin`
- `UnsafeEnd`
- `// --- Autograd ---`
- `GradBegin`
- `GradEnd`
- `Backward`
- `// --- Struct Copy ---`
- `StructCopy`

*Defined at line 1658*

---

## inst_result_type

```fides
pub fn inst_result_type(inst: Inst) -> IrType {
```

**Parameters:**

- `inst`: `Inst`

**Returns:** `IrType`

*Defined at line 1739*

---

## inst_effects

```fides
pub fn inst_effects(inst: &Inst) -> EffectSet {
```

**Parameters:**

- `inst`: `&Inst`

**Returns:** `EffectSet`

*Defined at line 1877*

---

## inst_kind_name

```fides
pub fn inst_kind_name(inst: &Inst) -> str {
```

**Parameters:**

- `inst`: `&Inst`

**Returns:** `str`

*Defined at line 1958*

---

## inst_is_terminator

```fides
pub fn inst_is_terminator(inst: Inst) -> bool {
```

**Parameters:**

- `inst`: `Inst`

**Returns:** `bool`

*Defined at line 2096*

---

## inst_value_refs

```fides
pub fn inst_value_refs(inst: &Inst) -> Vec<i64> {
```

**Parameters:**

- `inst`: `&Inst`

**Returns:** `Vec<i64>`

*Defined at line 2123*

---

## BasicBlock

```fides
pub struct BasicBlock {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `instructions` | `Vec<i64>` |
| `predecessors` | `Vec<i64>` |
| `params` | `Vec<ValueIdType>` |

*Defined at line 2280*

---

## BasicBlock_new

```fides
pub fn BasicBlock_new(id: i64) -> BasicBlock {
```

**Parameters:**

- `id`: `i64`

**Returns:** `BasicBlock`

*Defined at line 2287*

---

## IrFunction

```fides
pub struct IrFunction {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `params` | `Vec<ValueIdType>` |
| `ret_ty` | `IrType` |
| `blocks` | `Vec<BasicBlock>` |
| `value_metas` | `Vec<ValueIdMeta>` |
| `insts` | `Vec<ValueIdInst>` |
| `inst_at` | `Vec<i64>` |
| `effects` | `EffectSet` |
| `next_value` | `i64` |
| `next_block` | `i64` |
| `current_block_idx` | `i64` |
| `entry` | `i64` |
| `locals` | `Vec<StringValueId>` |
| `dbg_locals` | `Vec<StringValueId>` |
| `is_interrupt` | `bool` |
| `interrupt_vector` | `i64` |
| `persist_vars` | `Vec<StringValueId>` |
| `drop_locals` | `Vec<ValueIdString>` |
| `is_wrapping` | `bool` |
| `is_deterministic` | `bool` |
| `is_crashfree` | `bool` |
| `gas_limit` | `OptU64` |
| `is_no_alloc` | `bool` |
| `is_medical` | `bool` |
| `is_kernel` | `bool` |
| `workgroup_size` | `Vec<i64>` |
| `is_simd` | `bool` |
| `zeroize_locals` | `Vec<ValueIdU64>` |
| `internal_conv` | `bool` |
| `is_constant_time` | `bool,  // J7: @constant_time — native backends check the emitted code` |
| `has_secret` | `bool` |
| `secret_values` | `Vec<i64>` |

*Defined at line 2296*

---

## ValueIdMeta

```fides
pub struct ValueIdMeta {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `meta` | `ValueMeta` |

*Defined at line 2343*

---

## ValueIdMeta_new

```fides
pub fn ValueIdMeta_new(id: i64, meta: ValueMeta) -> ValueIdMeta {
```

**Parameters:**

- `id`: `i64`
- `meta`: `ValueMeta`

**Returns:** `ValueIdMeta`

*Defined at line 2348*

---

## ValueIdInst

```fides
pub struct ValueIdInst {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `inst` | `Inst` |

*Defined at line 2354*

---

## ValueIdInst_new

```fides
pub fn ValueIdInst_new(id: i64, inst: Inst) -> ValueIdInst {
```

**Parameters:**

- `id`: `i64`
- `inst`: `Inst`

**Returns:** `ValueIdInst`

*Defined at line 2359*

---

## IrFunction_new

```fides
pub fn IrFunction_new(name: str, ret_ty: IrType) -> IrFunction {
```

**Parameters:**

- `name`: `str`
- `ret_ty`: `IrType`

**Returns:** `IrFunction`

*Defined at line 2365*

---

## IrFunction_new_value

```fides
pub fn IrFunction_new_value(self: &mut IrFunction, meta: ValueMeta) -> i64 {
```

**Parameters:**

- `self`: `&mut IrFunction`
- `meta`: `ValueMeta`

**Returns:** `i64`

*Defined at line 2376*

---

## IrFunction_new_block

```fides
pub fn IrFunction_new_block(self: &mut IrFunction) -> i64 {
```

**Parameters:**

- `self`: `&mut IrFunction`

**Returns:** `i64`

*Defined at line 2383*

---

## IrFunction_current_block

```fides
pub fn IrFunction_current_block(self: &IrFunction) -> i64 {
```

**Parameters:**

- `self`: `&IrFunction`

**Returns:** `i64`

*Defined at line 2390*

---

## IrFunction_emit

```fides
pub fn IrFunction_emit(self: &mut IrFunction, inst: Inst) -> i64 {
```

**Parameters:**

- `self`: `&mut IrFunction`
- `inst`: `Inst`

**Returns:** `i64`

*Defined at line 2394*

---

## IrFunction_emit_line

IrFunction_emit with the value's source line set to `line` (when the
value's meta lands at index `id` of value_metas, as it does when
value_metas holds one meta per value so far).

```fides
pub fn IrFunction_emit_line(self: &mut IrFunction, inst: Inst, line: OptU64) -> i64 {
```

**Parameters:**

- `self`: `&mut IrFunction`
- `inst`: `Inst`
- `line`: `OptU64`

**Returns:** `i64`

*Defined at line 2401*

---

## IrFunction_add_block_param

```fides
pub fn IrFunction_add_block_param(self: &mut IrFunction, block: i64, ty: IrType) -> i64 {
```

**Parameters:**

- `self`: `&mut IrFunction`
- `block`: `i64`
- `ty`: `IrType`

**Returns:** `i64`

*Defined at line 2428*

---

## IrFunction_bind_local

```fides
pub fn IrFunction_bind_local(self: &mut IrFunction, name: str, value: i64) {
```

**Parameters:**

- `self`: `&mut IrFunction`
- `name`: `str`
- `value`: `i64`

*Defined at line 2440*

---

## IrFunction_lookup_local

```fides
pub fn IrFunction_lookup_local(self: &IrFunction, name: str) -> OptValueId {
```

**Parameters:**

- `self`: `&IrFunction`
- `name`: `str`

**Returns:** `OptValueId`

*Defined at line 2446*

---

## IrFunction_get_inst

```fides
pub fn IrFunction_get_inst(self: &IrFunction, id: i64) -> OptInst {
```

**Parameters:**

- `self`: `&IrFunction`
- `id`: `i64`

**Returns:** `OptInst`

*Defined at line 2459*

---

## IrFunction_get_meta

```fides
pub fn IrFunction_get_meta(self: &IrFunction, id: i64) -> OptValueMeta {
```

**Parameters:**

- `self`: `&IrFunction`
- `id`: `i64`

**Returns:** `OptValueMeta`

*Defined at line 2469*

---

## StructLayout

```fides
pub struct StructLayout {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `fields` | `Vec<StringIrTypeOffset>` |
| `size` | `i64` |
| `align` | `i64` |
| `repr_c` | `bool` |
| `packed_layout` | `bool` |

*Defined at line 2490*

---

## StructLayout_new

```fides
pub fn StructLayout_new(name: str) -> StructLayout {
```

**Parameters:**

- `name`: `str`

**Returns:** `StructLayout`

*Defined at line 2499*

---

## SourceEntry

```fides
pub struct SourceEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `function` | `str` |
| `value_id` | `i64` |
| `line` | `i64` |
| `inst` | `str` |

*Defined at line 2507*

---

## SourceEntry_new

```fides
pub fn SourceEntry_new(function: str, value_id: i64, line: i64, inst: str) -> SourceEntry {
```

**Parameters:**

- `function`: `str`
- `value_id`: `i64`
- `line`: `i64`
- `inst`: `str`

**Returns:** `SourceEntry`

*Defined at line 2514*

---

## SourceMap

```fides
pub struct SourceMap {
```

**Fields:**

| Name | Type |
|------|------|
| `entries` | `Vec<SourceEntry>` |

*Defined at line 2520*

---

## SourceMap_new

```fides
pub fn SourceMap_new() -> SourceMap {
```

**Returns:** `SourceMap`

*Defined at line 2524*

---

## SourceMap_lookup

```fides
pub fn SourceMap_lookup(self: &SourceMap, function: str, value_id: i64) -> OptU64 {
```

**Parameters:**

- `self`: `&SourceMap`
- `function`: `str`
- `value_id`: `i64`

**Returns:** `OptU64`

*Defined at line 2530*

---

## SourceMap_to_str

```fides
pub fn SourceMap_to_str(self: &SourceMap) -> str {
```

**Parameters:**

- `self`: `&SourceMap`

**Returns:** `str`

*Defined at line 2542*

---

## IrModule

```fides
pub struct IrModule {
```

**Fields:**

| Name | Type |
|------|------|
| `functions` | `Vec<IrFunctionEntry>` |
| `fn_index` | `i64` |
| `struct_layouts` | `Vec<StructLayoutEntry>` |
| `struct_index` | `i64` |
| `string_constants` | `Vec<StringStringEntry>` |
| `str_index` | `i64` |
| `extern_functions` | `Vec<StringVecIrType>` |
| `function_effects` | `Vec<StringEffectSetEntry>` |
| `effects_index` | `i64` |
| `extern_decls` | `Vec<ExternDecl>` |
| `c_callbacks` | `Vec<str>` |
| `trait_methods` | `Vec<StringVecStr>` |
| `trait_impls` | `Vec<StringVecStr>` |
| `linear_structs` | `Vec<str>` |
| `globals` | `Vec<GlobalEntry>` |
| `global_index` | `i64` |

*Defined at line 2558*

---

## IrFunctionEntry

```fides
pub struct IrFunctionEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `func` | `IrFunction` |

*Defined at line 2586*

---

## IrFunctionEntry_new

```fides
pub fn IrFunctionEntry_new(name: str, func: IrFunction) -> IrFunctionEntry {
```

**Parameters:**

- `name`: `str`
- `func`: `IrFunction`

**Returns:** `IrFunctionEntry`

*Defined at line 2591*

---

## StructLayoutEntry

```fides
pub struct StructLayoutEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `layout` | `StructLayout` |

*Defined at line 2597*

---

## StructLayoutEntry_new

```fides
pub fn StructLayoutEntry_new(name: str, layout: StructLayout) -> StructLayoutEntry {
```

**Parameters:**

- `name`: `str`
- `layout`: `StructLayout`

**Returns:** `StructLayoutEntry`

*Defined at line 2602*

---

## StringStringEntry

```fides
pub struct StringStringEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `val` | `str` |

*Defined at line 2608*

---

## StringStringEntry_new

```fides
pub fn StringStringEntry_new(key: str, val: str) -> StringStringEntry {
```

**Parameters:**

- `key`: `str`
- `val`: `str`

**Returns:** `StringStringEntry`

*Defined at line 2613*

---

## StringEffectSetEntry

```fides
pub struct StringEffectSetEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `effects` | `EffectSet` |

*Defined at line 2619*

---

## StringEffectSetEntry_new

```fides
pub fn StringEffectSetEntry_new(name: str, effects: EffectSet) -> StringEffectSetEntry {
```

**Parameters:**

- `name`: `str`
- `effects`: `EffectSet`

**Returns:** `StringEffectSetEntry`

*Defined at line 2624*

---

## GlobalEntry

```fides
pub struct GlobalEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `ty` | `IrType` |
| `init` | `Expr` |

*Defined at line 2631*

---

## GlobalEntry_new

```fides
pub fn GlobalEntry_new(name: str, ty: IrType, init: Expr) -> GlobalEntry {
```

**Parameters:**

- `name`: `str`
- `ty`: `IrType`
- `init`: `Expr`

**Returns:** `GlobalEntry`

*Defined at line 2637*

---

## IrModule_get_global

```fides
pub fn IrModule_get_global(self: &IrModule, name: str) -> OptGlobalLookup {
```

**Parameters:**

- `self`: `&IrModule`
- `name`: `str`

**Returns:** `OptGlobalLookup`

*Defined at line 2646*

---

## IrModule_global_pos

SP10: the position of global `name` in `self.globals`, or -1: the first
of that name, as a scan from the front finds it. The index is lazy:
entries pushed since the last lookup are indexed first ("#n" counts the
positions indexed). It is not trusted when the list shrank, when the
last entry it indexed is no longer where it says (the list was replaced,
or a copy of the module pushed to the shared index), or when a hit's
position holds another name: then it is rebuilt from the front.

```fides
pub fn IrModule_global_pos(self: &IrModule, name: str) -> i64 {
```

**Parameters:**

- `self`: `&IrModule`
- `name`: `str`

**Returns:** `i64`

*Defined at line 2662*

---

## OptGlobalLookup

```fides
pub enum OptGlobalLookup {
```

**Variants:**

- `Some`
- `None`

*Defined at line 2709*

---

## IrModule_new

```fides
pub fn IrModule_new() -> IrModule {
```

**Returns:** `IrModule`

*Defined at line 2714*

---

## c_fn_name

A `ConstFnRef` of this prefix and a function name is the address of the
function's adapter, which C can call: it supplies the `__env` Fides takes.

```fides
pub fn c_fn_name(name: str) -> str {
```

**Parameters:**

- `name`: `str`

**Returns:** `str`

*Defined at line 2726*

---

## c_fn_target

The function a `ConstFnRef` name refers to.

```fides
pub fn c_fn_target(name: str) -> str {
```

**Parameters:**

- `name`: `str`

**Returns:** `str`

*Defined at line 2731*

---

## IrModule_add_struct

```fides
pub fn IrModule_add_struct(self: &mut IrModule, layout: StructLayout) {
```

**Parameters:**

- `self`: `&mut IrModule`
- `layout`: `StructLayout`

*Defined at line 2738*

---

## IrModule_get_struct

```fides
pub fn IrModule_get_struct(self: &IrModule, name: str) -> OptStructLayout {
```

**Parameters:**

- `self`: `&IrModule`
- `name`: `str`

**Returns:** `OptStructLayout`

*Defined at line 2757*

---

## IrModule_intern_string

```fides
pub fn IrModule_intern_string(self: &mut IrModule, s: str) -> str {
```

**Parameters:**

- `self`: `&mut IrModule`
- `s`: `str`

**Returns:** `str`

*Defined at line 2772*

---

## IrModule_add_extern

```fides
pub fn IrModule_add_extern(self: &mut IrModule, name: str, params: Vec<IrType>, ret: IrType) {
```

**Parameters:**

- `self`: `&mut IrModule`
- `name`: `str`
- `params`: `Vec<IrType>`
- `ret`: `IrType`

*Defined at line 2783*

---

## IrModule_type_size

```fides
pub fn IrModule_type_size(self: &IrModule, ty: IrType) -> i64 {
```

**Parameters:**

- `self`: `&IrModule`
- `ty`: `IrType`

**Returns:** `i64`

*Defined at line 2787*

---

## IrModule_type_align

```fides
pub fn IrModule_type_align(self: &IrModule, ty: IrType) -> i64 {
```

**Parameters:**

- `self`: `&IrModule`
- `ty`: `IrType`

**Returns:** `i64`

*Defined at line 2807*

---

## IrModule_add_function

```fides
pub fn IrModule_add_function(self: &mut IrModule, func: IrFunction) {
```

**Parameters:**

- `self`: `&mut IrModule`
- `func`: `IrFunction`

*Defined at line 2827*

---

## IrModule_replace_function

```fides
pub fn IrModule_replace_function(self: &mut IrModule, func: IrFunction) {
```

**Parameters:**

- `self`: `&mut IrModule`
- `func`: `IrFunction`

*Defined at line 2841*

---

## IrModule_replace_function_named

```fides
pub fn IrModule_replace_function_named(self: &mut IrModule, func: IrFunction, name: str) {
```

**Parameters:**

- `self`: `&mut IrModule`
- `func`: `IrFunction`
- `name`: `str`

*Defined at line 2853*

---

## IrModule_get_function

```fides
pub fn IrModule_get_function(self: &IrModule, name: str) -> OptIrFunction {
```

**Parameters:**

- `self`: `&IrModule`
- `name`: `str`

**Returns:** `OptIrFunction`

*Defined at line 2863*

---

## IrModule_fn_pos

SP10 (own-backend roadmap): the position of function `name` in
`self.functions`, or -1, through `fn_index` (the lazy index that
IrModule_get_function keeps: entries pushed since the last lookup are
indexed first, the first function of a name wins, as a scan from the
front would find it). An entry whose position no longer holds that name
(the list was changed without HashMap_clear, AGENTS.md item 14) is not
trusted: the index is rebuilt and the name looked up again.

```fides
pub fn IrModule_fn_pos(self: &IrModule, name: str) -> i64 {
```

**Parameters:**

- `self`: `&IrModule`
- `name`: `str`

**Returns:** `i64`

*Defined at line 2887*

---

## IrModule_effects_pos

SP10: the position of `name` in `self.function_effects`, or -1, through
`effects_index` — the same lazy index IrModule_fn_pos keeps for
`functions` (entries pushed since the last lookup are indexed first, the
first entry of a name wins). A stale entry is rebuilt as there.

```fides
pub fn IrModule_effects_pos(self: &IrModule, name: str) -> i64 {
```

**Parameters:**

- `self`: `&IrModule`
- `name`: `str`

**Returns:** `i64`

*Defined at line 2921*

---

## lower_type

```fides
pub fn lower_type(ast_ty: Type) -> IrType {
```

**Parameters:**

- `ast_ty`: `Type`

**Returns:** `IrType`

*Defined at line 2952*

---

## box_lower_type

```fides
pub fn box_lower_type(b: Box<Type>) -> IrType {
```

**Parameters:**

- `b`: `Box<Type>`

**Returns:** `IrType`

*Defined at line 3054*

---

## IrModule_optimize

```fides
pub fn IrModule_optimize(self: &mut IrModule) -> i64 {
```

**Parameters:**

- `self`: `&mut IrModule`

**Returns:** `i64`

*Defined at line 3158*

---

## PeirStrPair

A (stub, Fides runtime function) dependency pair.

```fides
pub struct PeirStrPair {
```

**Fields:**

| Name | Type |
|------|------|
| `a` | `str` |
| `b` | `str` |

*Defined at line 3178*

---

## pe_strs_contains

Linear-scan membership for Vec<str> (BTreeSet<String>::contains).

```fides
pub fn pe_strs_contains(v: Vec<str>, s: str) -> bool {
```

**Parameters:**

- `v`: `Vec<str>`
- `s`: `str`

**Returns:** `bool`

*Defined at line 3184*

---

## pe_sort_strs

Sort a Vec<str> with str_compare (deterministic BTree order). A
bottom-up merge sort since Nov 17 (WP15: the reachable set of a whole
program is thousands of names, and insertion sort was quadratic); the
order is the same as before.

```fides
pub fn pe_sort_strs(v: Vec<str>) -> Vec<str> {
```

**Parameters:**

- `v`: `Vec<str>`

**Returns:** `Vec<str>`

*Defined at line 3199*

---

## pe_stub_deps

Machine code that only exists with the Fides function it hands over to:
(stub, Fides runtime function).

```fides
pub fn pe_stub_deps() -> Vec<PeirStrPair> {
```

**Returns:** `Vec<PeirStrPair>`

*Defined at line 3241*

---

## pe_print_builtins

The builtins `print`, `p_to_str` and the Print instruction dispatch to.

```fides
pub fn pe_print_builtins() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 3268*

---

## is_pe_runtime_fn

A function of the Fides PE runtime (src/pe_runtime/): builtins `pert_*`
and the helpers of each file (`pe_*`, `pet_*` tensors, `ped_*` data).

```fides
pub fn is_pe_runtime_fn(name: str) -> bool {
```

**Parameters:**

- `name`: `str`

**Returns:** `bool`

*Defined at line 3283*

---

## pert_name

The Fides runtime's stand-in for runtime builtin `p_X` / `__X`: `pert_X`.
Returns "" when `name` has no `p_`/`__` prefix (Option::None).

```fides
pub fn pert_name(name: str) -> str {
```

**Parameters:**

- `name`: `str`

**Returns:** `str`

*Defined at line 3289*

---

## pe_runtime_alias

Runtime builtins this backend implements with core's Fides code.

```fides
pub fn pe_runtime_alias(name: str) -> str {
```

**Parameters:**

- `name`: `str`

**Returns:** `str`

*Defined at line 3300*

---

## pe_runtime_alias_target

Is `name` a Fides stand-in `pe_runtime_alias` maps a builtin to?  Such a
function keeps the `__env`-first convention: the native backends call it
through the p_* builtin name with a null env inserted, whatever its body
uses (mirrors the outputs of `pe_runtime_alias`).

```fides
pub fn pe_runtime_alias_target(name: str) -> bool {
```

**Parameters:**

- `name`: `str`

**Returns:** `bool`

*Defined at line 3367*

---

## pe_module_has_fn

Does the module define a function `name`?

```fides
pub fn pe_module_has_fn(module: &IrModule, name: str) -> bool {
```

**Parameters:**

- `module`: `&IrModule`
- `name`: `str`

**Returns:** `bool`

*Defined at line 3372*

---

## pe_ir_c_override

A C library function the Fides runtime implements itself, for `extern
"C"` function `name`: `pert_c_<C name>` (putchar, puts: stdout goes
through this backend's own output, unbuffered and in order with print,
as all of it goes through C stdio in the C backend).  "" = no override.

```fides
pub fn pe_ir_c_override(module: &IrModule, name: str) -> str {
```

**Parameters:**

- `module`: `&IrModule`
- `name`: `str`

**Returns:** `str`

*Defined at line 3381*

---

## pe_ir_resolve_callee

`PeirGen::resolve_call` for the call graph: an alias, or the Fides
runtime's `pert_X` for builtin `p_X` when the program has it.

```fides
pub fn pe_ir_resolve_callee(module: &IrModule, name: str) -> str {
```

**Parameters:**

- `module`: `&IrModule`
- `name`: `str`

**Returns:** `str`

*Defined at line 3400*

---

## pe_ir_print_formatter

The Fides function that formats a printed collection, if any. "" = none.

```fides
pub fn pe_ir_print_formatter(ty: IrType) -> str {
```

**Parameters:**

- `ty`: `IrType`

**Returns:** `str`

*Defined at line 3417*

---

## pe_ir_autograd_builtin

The runtime builtin an autograd instruction calls (as in the C backend).

```fides
pub fn pe_ir_autograd_builtin(inst: Inst) -> str {
```

**Parameters:**

- `inst`: `Inst`

**Returns:** `str`

*Defined at line 3465*

---

## pe_ir_reachable_from

Functions reachable from `roots`: through calls, function references
(closures, thread entries) and `dyn` method dispatch — the exact set of
names the native backends may emit a call for.

```fides
pub fn pe_ir_reachable_from(module: &IrModule, roots: Vec<str>) -> Vec<str> {
```

**Parameters:**

- `module`: `&IrModule`
- `roots`: `Vec<str>`

**Returns:** `Vec<str>`

*Defined at line 3483*

---

## pe_ir_needed_functions

The Fides runtime functions an executable needs: those reachable from
the functions it is made of (every program and stdlib function is
compiled, used or not).

```fides
pub fn pe_ir_needed_functions(module: &IrModule) -> Vec<str> {
```

**Parameters:**

- `module`: `&IrModule`

**Returns:** `Vec<str>`

*Defined at line 3611*

---

## pe_ir_reachable_functions

Functions reachable from `main`: through calls, function references
(closures, thread entries) and `dyn` method dispatch.  Everything, when
there is no `main`.

```fides
pub fn pe_ir_reachable_functions(module: &IrModule) -> Vec<str> {
```

**Parameters:**

- `module`: `&IrModule`

**Returns:** `Vec<str>`

*Defined at line 3672*

---

## ir_dfe_root

WP15: a function dead function elimination keeps whatever calls it:
`main`, `main_entry`, the p_* builtins and `__` helpers (the C runtime
and the native stubs call them), the Fides runtime's pe_/pert_/pet_/ped_
functions (machine code calls pe_rt_init, pe_win_start, pe_on_signal and
others by name: is_pe_runtime_fn), nat_* (nat_ir's TLS helpers), core's
Vec_* and vec_raw_* stand-ins that p_vec_* builtins become
(pe_runtime_alias_target), the collection printers, and the bare
image's syscall entry points. Interrupt handlers and GPU kernels are
installed by name too (the caller adds them). WP7 (Nov 17): also core's
`vec_buf_alloc`, which nat_ir's vector loops (ng_simd_call, OP21) call
by name with a null env first: envtrim must not drop that env, WP6 must
keep its platform convention.

```fides
pub fn ir_dfe_root(name: str) -> bool {
```

**Parameters:**

- `name`: `str`

**Returns:** `bool`

*Defined at line 3699*

---

## ir_data_named_fns

WP7/WP15: the functions the module's data names (a string constant or a
global's string initializer equal to a function's name): a task or
thread entry looked up by name at run time. Their signature must stay
(envtrim, WP6) and they stay alive (deadf), as pe88 roots them.

```fides
pub fn ir_data_named_fns(module: &IrModule) -> Vec<str> {
```

**Parameters:**

- `module`: `&IrModule`

**Returns:** `Vec<str>`

*Defined at line 3713*

---

## IrModule_dead_global_elimination

WP15: globals nothing reads. A global whose address is only ever stored
through (never loaded, never passed on, in no function) and whose
initializer is a literal is removed, with those stores and its
GlobalAddr instructions: nothing can see its value. (A str/Vec/map
global is read by every assignment, which drops the old value, so only
scalars qualify.) Returns how many were removed.

```fides
pub fn IrModule_dead_global_elimination(self: &mut IrModule) -> i64 {
```

**Parameters:**

- `self`: `&mut IrModule`

**Returns:** `i64`

*Defined at line 3749*

---

## IrModule_dead_function_elimination

Remove functions that are never called (except `main` and `p_*` runtime stubs).
This reduces the size of the generated C code significantly.

Reachability is `pe_ir_reachable_from` — the same set of names the pe_ir
and nat_ir backends may emit a call for (resolved aliases / pert_*
stand-ins, function references, dyn method candidates, print dispatch,
thread entry names, autograd builtins).  Keeping anything less removed
functions a backend still calls (M10).

```fides
pub fn IrModule_dead_function_elimination(self: &mut IrModule) -> i64 {
```

**Parameters:**

- `self`: `&mut IrModule`

**Returns:** `i64`

*Defined at line 3943*

---

## IrFunction_set_inst

```fides
pub fn IrFunction_set_inst(self: &mut IrFunction, id: i64, inst: Inst) {
```

**Parameters:**

- `self`: `&mut IrFunction`
- `id`: `i64`
- `inst`: `Inst`

*Defined at line 4081*

---

## IrFunction_set_meta

```fides
pub fn IrFunction_set_meta(self: &mut IrFunction, id: i64, meta: ValueMeta) {
```

**Parameters:**

- `self`: `&mut IrFunction`
- `id`: `i64`
- `meta`: `ValueMeta`

*Defined at line 4091*

---

## VerificationResult

```fides
pub struct VerificationResult {
```

**Fields:**

| Name | Type |
|------|------|
| `errors` | `Vec<str>` |
| `warnings` | `Vec<str>` |

*Defined at line 4237*

---

## VerificationResult_new

```fides
pub fn VerificationResult_new() -> VerificationResult {
```

**Returns:** `VerificationResult`

*Defined at line 4242*

---

## VerificationResult_is_valid

```fides
pub fn VerificationResult_is_valid(self: &VerificationResult) -> bool {
```

**Parameters:**

- `self`: `&VerificationResult`

**Returns:** `bool`

*Defined at line 4248*

---

## IrModule_verify

```fides
pub fn IrModule_verify(self: &IrModule) -> VerificationResult {
```

**Parameters:**

- `self`: `&IrModule`

**Returns:** `VerificationResult`

*Defined at line 4252*

---

## IrModule_to_str

```fides
pub fn IrModule_to_str(self: &IrModule) -> str {
```

**Parameters:**

- `self`: `&IrModule`

**Returns:** `str`

*Defined at line 4431*

---

## ir_binop_str

A binary operator as the IR printer shows it.

```fides
pub fn ir_binop_str(op: BinOp) -> str {
```

**Parameters:**

- `op`: `BinOp`

**Returns:** `str`

*Defined at line 4576*

---

## ir_unop_str

A unary operator as the IR printer shows it.

```fides
pub fn ir_unop_str(op: UnaryOp) -> str {
```

**Parameters:**

- `op`: `UnaryOp`

**Returns:** `str`

*Defined at line 4637*

---

## inst_detail_str

One instruction with its operands, e.g. `add %3, %4: i64`.

```fides
pub fn inst_detail_str(inst: Inst) -> str {
```

**Parameters:**

- `inst`: `Inst`

**Returns:** `str`

*Defined at line 4672*

---

## IrFunction_detail_str

One function with every instruction's operands and source line.

```fides
pub fn IrFunction_detail_str(func: &IrFunction, name: str) -> str {
```

**Parameters:**

- `func`: `&IrFunction`
- `name`: `str`

**Returns:** `str`

*Defined at line 4830*

---

## IrModule_function_detail_str

Function `name` of the module as IrFunction_detail_str prints it, or ""
when the module has no such function (inlined away, or never reached).

```fides
pub fn IrModule_function_detail_str(self: &IrModule, name: str) -> str {
```

**Parameters:**

- `self`: `&IrModule`
- `name`: `str`

**Returns:** `str`

*Defined at line 4901*

---

## IrModule_source_map

```fides
pub fn IrModule_source_map(self: &IrModule) -> SourceMap {
```

**Parameters:**

- `self`: `&IrModule`

**Returns:** `SourceMap`

*Defined at line 4912*

---

## ptr_u8

```fides
pub fn ptr_u8() -> IrType {
```

**Returns:** `IrType`

*Defined at line 4944*

---

## c_vec1_i64

```fides
pub fn c_vec1_i64(x: i64) -> Vec<i64> {
```

**Parameters:**

- `x`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 4949*

---

## ResultBytes

Bytes-or-errors result used by the native backends (PE/ELF/WASM).

```fides
pub enum ResultBytes {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 4964*

---

## c_vec1

Create a Vec<str> with one element.

```fides
pub fn c_vec1(s: str) -> Vec<str> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<str>`

*Defined at line 4970*

---

## c_file_exists

Check if a file exists.

```fides
pub fn c_file_exists(path: str) -> bool {
```

**Parameters:**

- `path`: `str`

**Returns:** `bool`

*Defined at line 4977*

---

## ir_note_fn_file

```fides
pub fn ir_note_fn_file(name: str, path: str) {
```

**Parameters:**

- `name`: `str`
- `path`: `str`

*Defined at line 4997*

---

## ir_fn_file

The file `name` was recorded from, or "": the program's own files
first, then the library's (a function the program defines again is the
program's, as in the merge).

```fides
pub fn ir_fn_file(name: str) -> str {
```

**Parameters:**

- `name`: `str`

**Returns:** `str`

*Defined at line 5007*

---

## ir_note_program_files

Record every function and method of `prog` as defined in `path`.

```fides
pub fn ir_note_program_files(prog: &Program, path: str) {
```

**Parameters:**

- `prog`: `&Program`
- `path`: `str`

*Defined at line 5024*

---

## ir_note_lib_program

Record every function and method of the library file `prog` (core, a
runtime file) as defined in `path`.

```fides
pub fn ir_note_lib_program(prog: &Program, path: str) {
```

**Parameters:**

- `prog`: `&Program`
- `path`: `str`

*Defined at line 5077*

---

## ir_lib_file_pairs

The library's records as pairs: name, path, name, path, ... (what the
binary artifact stores).

```fides
pub fn ir_lib_file_pairs() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 5110*

---

## ir_note_lib_pairs

Restore library records from pairs name, path (an artifact hit).

```fides
pub fn ir_note_lib_pairs(pairs: Vec<str>) {
```

**Parameters:**

- `pairs`: `Vec<str>`

*Defined at line 5115*

---

## ir_lib_fn_file

The library file `name` was recorded from, or "" (tests, tools).

```fides
pub fn ir_lib_fn_file(name: str) -> str {
```

**Parameters:**

- `name`: `str`

**Returns:** `str`

*Defined at line 5124*

---

## ir_note_fn_clone

`copy_name` is a copy of function `orig` under a new name (iropt.fd
iro_clone_function: a specialization): same instructions, same ids, so
its rows take the files `orig`'s take.

```fides
pub fn ir_note_fn_clone(orig: str, copy_name: str) {
```

**Parameters:**

- `orig`: `str`
- `copy_name`: `str`

*Defined at line 5140*

---

## ir_fn_src_file

The file of function `name` for its line rows: ir_fn_file; for a clone
(ir_note_fn_clone) the file of the function cloned; for a
monomorphized instance (mono.fd mangle: `f__T1_T2`, mangle_method:
`Type__m__T1`) or a function the optimizer made from another
(`f__cdev_g`, `f__dyn0_T`) the file of the function it was made from.
"" when none is known (a compiler-made `__name`).

```fides
pub fn ir_fn_src_file(name: str) -> str {
```

**Parameters:**

- `name`: `str`

**Returns:** `str`

*Defined at line 5168*

---

## ir_value_file_rec

The file recorded for instruction `id` of function `fname` by
ir_copy_value_file (or for the same instruction of the function `fname`
was cloned from), or "": the function's own file.

```fides
pub fn ir_value_file_rec(fname: str, id: i64) -> str {
```

**Parameters:**

- `fname`: `str`
- `id`: `i64`

**Returns:** `str`

*Defined at line 5206*

---

## ir_fn_has_value_files

Whether any instruction of `fname` (or of the function it was cloned
from) has a file of its own (nat_ir.fd asks once per instruction).

```fides
pub fn ir_fn_has_value_files(fname: str) -> bool {
```

**Parameters:**

- `fname`: `str`

**Returns:** `bool`

*Defined at line 5230*

---

## ir_value_file

The source file of instruction `id` of function `fname`: the file of
the code it was inlined from, else the function's own.

```fides
pub fn ir_value_file(fname: str, id: i64) -> str {
```

**Parameters:**

- `fname`: `str`
- `id`: `i64`

**Returns:** `str`

*Defined at line 5252*

---

## ir_copy_value_file

Instruction `to` of function `dst` is a copy of instruction `from` of
function `src` (inlined: iropt.fd iro_copy_line, iropt_me.fd
ime_copy_line; or copied within one function: ime_copy_line_self). The
copy keeps the source line (those helpers) and, here, the file of the
code it came from, recorded only when that is not `dst`'s own file: a
function with no code from elsewhere has no entries. A copy from a
function of no known file stays on `dst`'s.

```fides
pub fn ir_copy_value_file(src: str, from: i64, dst: str, to: i64) {
```

**Parameters:**

- `src`: `str`
- `from`: `i64`
- `dst`: `str`
- `to`: `i64`

*Defined at line 5267*

---

## ir_value_files_reset

Forget the records of inlined code and clones (a new module is about to
be optimized: in a long-lived process, `fid serve` or the LSP, an
earlier build's ids would otherwise name instructions of this one).

```fides
pub fn ir_value_files_reset() {
```

*Defined at line 5292*

---

## ir_switch_on

Whether switches are formed (FID_SWITCH=off or 0: no).

```fides
pub fn ir_switch_on() -> bool {
```

**Returns:** `bool`

*Defined at line 5316*

---

## switch_succ_blocks

The blocks a Switch may go to: each target once, then the default.

```fides
pub fn switch_succ_blocks(targets: Vec<i64>, dflt: i64) -> Vec<i64> {
```

**Parameters:**

- `targets`: `Vec<i64>`
- `dflt`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 5328*

---

## inst_succ_blocks

The blocks a terminator may go to (each once): Jump, Branch, Switch;
empty for anything else.

```fides
pub fn inst_succ_blocks(inst: Inst) -> Vec<i64> {
```

**Parameters:**

- `inst`: `Inst`

**Returns:** `Vec<i64>`

*Defined at line 5345*

---

## IrFunction_form_switches

Replace compare chains of `f` on one value by Switch terminators. A
link (every test after the first) must be a block entered only from the
previous test, holding nothing but its test (ir_sw_link_inst), whose
values nothing outside it uses, and testing the same value: the same
value id, or a load of the same word when the first test's value is a
load with nothing that writes after it in its block. Links become
unreachable blocks (one Unreachable). Returns how many were formed.
Never in @constant_time code (LF18).

```fides
pub fn IrFunction_form_switches(f: &mut IrFunction) -> i64 {
```

**Parameters:**

- `f`: `&mut IrFunction`

**Returns:** `i64`

*Defined at line 5570*

---

## IrModule_form_switches

IrFunction_form_switches on every function; returns how many formed.

```fides
pub fn IrModule_form_switches(self: &mut IrModule) -> i64 {
```

**Parameters:**

- `self`: `&mut IrModule`

**Returns:** `i64`

*Defined at line 5830*

---

## IrFunction_has_switch

Does `f` hold a Switch?

```fides
pub fn IrFunction_has_switch(f: &IrFunction) -> bool {
```

**Parameters:**

- `f`: `&IrFunction`

**Returns:** `bool`

*Defined at line 5863*

---

## IrFunction_expand_switches

Turn every Switch of `f` back into a chain of `x == k` tests (new
blocks), for code that knows only Jump and Branch. Returns how many.

```fides
pub fn IrFunction_expand_switches(f: &mut IrFunction) -> i64 {
```

**Parameters:**

- `f`: `&mut IrFunction`

**Returns:** `i64`

*Defined at line 5883*

---

## IrFunction_without_switches

`f` as code that knows only Jump and Branch sees it: itself when it has
no Switch, else a copy with every Switch expanded (pe_ir).

```fides
pub fn IrFunction_without_switches(f: IrFunction) -> IrFunction {
```

**Parameters:**

- `f`: `IrFunction`

**Returns:** `IrFunction`

*Defined at line 5936*

---

## SWP_EQ:

```fides
pub const SWP_EQ: i64 = 1;
```

*Defined at line 5960*

---

## SWP_LT:

```fides
pub const SWP_LT: i64 = 2;
```

*Defined at line 5961*

---

## SWP_BITS:

```fides
pub const SWP_BITS: i64 = 3;
```

*Defined at line 5962*

---

## SWP_LABEL:

```fides
pub const SWP_LABEL: i64 = 4;
```

*Defined at line 5963*

---

## SWP_JMP:

```fides
pub const SWP_JMP: i64 = 5;
```

*Defined at line 5964*

---

## ir_switch_plan

The plan for Switch(_, keys, targets, dflt) (see above).

```fides
pub fn ir_switch_plan(keys: Vec<i64>, targets: Vec<i64>, dflt: i64) -> Vec<i64> {
```

**Parameters:**

- `keys`: `Vec<i64>`
- `targets`: `Vec<i64>`
- `dflt`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 6027*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
