[All modules](index.md)

# compiler::interp -- Fides Documentation

## Contents

- [Value](#Value) (enum)
- [Value_int](#Value_int) (function)
- [interp_reinterpret_word](#interp_reinterpret_word) (function)
- [Value_float](#Value_float) (function)
- [Value_bool](#Value_bool) (function)
- [Value_str](#Value_str) (function)
- [Value_ptr](#Value_ptr) (function)
- [Value_void](#Value_void) (function)
- [value_eq](#value_eq) (function)
- [value_is_void](#value_is_void) (function)
- [Memory](#Memory) (struct)
- [MemTxMark](#MemTxMark) (struct)
- [Memory_new](#Memory_new) (function)
- [Memory_put](#Memory_put) (function)
- [Memory_set_block](#Memory_set_block) (function)
- [Memory_tx_depth](#Memory_tx_depth) (function)
- [Memory_tx_begin](#Memory_tx_begin) (function)
- [Memory_tx_commit](#Memory_tx_commit) (function)
- [Memory_tx_rollback](#Memory_tx_rollback) (function)
- [Memory_alloc](#Memory_alloc) (function)
- [Memory_alloc_frame](#Memory_alloc_frame) (function)
- [Memory_free](#Memory_free) (function)
- [Memory_free_frame](#Memory_free_frame) (function)
- [Memory_block_size](#Memory_block_size) (function)
- [Memory_allocated](#Memory_allocated) (function)
- [Memory_capacity](#Memory_capacity) (function)
- [Memory_is_valid_addr](#Memory_is_valid_addr) (function)
- [Memory_store_byte](#Memory_store_byte) (function)
- [Memory_load_byte](#Memory_load_byte) (function)
- [Memory_store_i64](#Memory_store_i64) (function)
- [Memory_load_i64](#Memory_load_i64) (function)
- [Memory_store_bool](#Memory_store_bool) (function)
- [Memory_load_bool](#Memory_load_bool) (function)
- [Memory_store_i32](#Memory_store_i32) (function)
- [Memory_store_i16](#Memory_store_i16) (function)
- [Memory_load_i32](#Memory_load_i32) (function)
- [Memory_store_f64](#Memory_store_f64) (function)
- [f64_from_bits_i](#f64_from_bits_i) (function)
- [f32_from_bits_i](#f32_from_bits_i) (function)
- [interp_round_f32](#interp_round_f32) (function)
- [interp_narrow_int](#interp_narrow_int) (function)
- [interp_scalar_ty](#interp_scalar_ty) (function)
- [Memory_load_f64](#Memory_load_f64) (function)
- [Memory_store_f32](#Memory_store_f32) (function)
- [Memory_load_f32](#Memory_load_f32) (function)
- [Memory_copy](#Memory_copy) (function)
- [Memory_store_cstr](#Memory_store_cstr) (function)
- [Memory_load_cstr](#Memory_load_cstr) (function)
- [ExecOutcome](#ExecOutcome) (enum)
- [RunResult](#RunResult) (enum)
- [StringAddrEntry](#StringAddrEntry) (struct)
- [StringAddrEntry_new](#StringAddrEntry_new) (function)
- [GlobalAddrEntry](#GlobalAddrEntry) (struct)
- [GlobalAddrEntry_new](#GlobalAddrEntry_new) (function)
- [OpaqueEntry](#OpaqueEntry) (struct)
- [OpaqueEntry_new](#OpaqueEntry_new) (function)
- [I64Table](#I64Table) (struct)
- [I64Table_new](#I64Table_new) (function)
- [I64Table_find](#I64Table_find) (function)
- [I64Table_get](#I64Table_get) (function)
- [I64Table_set](#I64Table_set) (function)
- [I64Table_remove](#I64Table_remove) (function)
- [I64Table_clone](#I64Table_clone) (function)
- [StrTable](#StrTable) (struct)
- [StrTable_new](#StrTable_new) (function)
- [StrTable_find](#StrTable_find) (function)
- [StrTable_get](#StrTable_get) (function)
- [StrTable_set](#StrTable_set) (function)
- [ArrayBoundEntry](#ArrayBoundEntry) (struct)
- [ArrayBoundEntry_new](#ArrayBoundEntry_new) (function)
- [VecSideEntry](#VecSideEntry) (struct)
- [VecSideEntry_new](#VecSideEntry_new) (function)
- [VecValueEntry](#VecValueEntry) (struct)
- [VecValueEntry_new](#VecValueEntry_new) (function)
- [MapValueEntry](#MapValueEntry) (struct)
- [TensorSideEntry](#TensorSideEntry) (struct)
- [TensorSideEntry_new](#TensorSideEntry_new) (function)
- [MapValueEntry_new](#MapValueEntry_new) (function)
- [Interpreter](#Interpreter) (struct)
- [Interpreter_new](#Interpreter_new) (function)
- [Interpreter_get_function](#Interpreter_get_function) (function)
- [Interpreter_verbose](#Interpreter_verbose) (function)
- [Interpreter_set_max_recursion](#Interpreter_set_max_recursion) (function)
- [Interpreter_set_max_steps](#Interpreter_set_max_steps) (function)
- [Interpreter_set_max_memory](#Interpreter_set_max_memory) (function)
- [Interpreter_lookup_string_addr](#Interpreter_lookup_string_addr) (function)
- [Interpreter_lookup_global_addr](#Interpreter_lookup_global_addr) (function)
- [Interpreter_lookup_opaque](#Interpreter_lookup_opaque) (function)
- [Interpreter_set_opaque](#Interpreter_set_opaque) (function)
- [Interpreter_remove_opaque](#Interpreter_remove_opaque) (function)
- [Interpreter_txn_begin](#Interpreter_txn_begin) (function)
- [Interpreter_txn_commit](#Interpreter_txn_commit) (function)
- [Interpreter_txn_rollback](#Interpreter_txn_rollback) (function)
- [Interpreter_txn_unwind](#Interpreter_txn_unwind) (function)
- [Interpreter_lookup_array_bound](#Interpreter_lookup_array_bound) (function)
- [Interpreter_set_array_bound](#Interpreter_set_array_bound) (function)
- [Interpreter_free_block](#Interpreter_free_block) (function)
- [Interpreter_free_frame_block](#Interpreter_free_frame_block) (function)
- [OptValue](#OptValue) (enum)
- [OptStr](#OptStr) (enum)
- [OptVecI64](#OptVecI64) (enum)
- [OptVecF64](#OptVecF64) (enum)
- [Interpreter_init_strings](#Interpreter_init_strings) (function)
- [Interpreter_init_globals](#Interpreter_init_globals) (function)
- [eval_global_init](#eval_global_init) (function)
- [Interpreter_value_to_string](#Interpreter_value_to_string) (function)
- [Interpreter_value_bytes](#Interpreter_value_bytes) (function)
- [Interpreter_display_value](#Interpreter_display_value) (function)
- [sorted_key_order](#sorted_key_order) (function)
- [str_lt](#str_lt) (function)
- [Interpreter_print_via](#Interpreter_print_via) (function)
- [Interpreter_format_call_stack](#Interpreter_format_call_stack) (function)
- [Interpreter_run](#Interpreter_run) (function)
- [Interpreter_run_function](#Interpreter_run_function) (function)
- [Interpreter_value_word](#Interpreter_value_word) (function)
- [get_value](#get_value) (function)
- [Interpreter_execute_inst](#Interpreter_execute_inst) (function)
- [Interpreter_eval_binop](#Interpreter_eval_binop) (function)
- [Interpreter_eval_unaryop](#Interpreter_eval_unaryop) (function)
- [Interpreter_cast_value](#Interpreter_cast_value) (function)
- [interp_builtin_cap](#interp_builtin_cap) (function)
- [Interpreter_cap_is_revoked](#Interpreter_cap_is_revoked) (function)
- [Interpreter_cap_denied](#Interpreter_cap_denied) (function)
- [interp_sandbox_panic](#interp_sandbox_panic) (function)
- [interp_is_cap_builtin](#interp_is_cap_builtin) (function)
- [Interpreter_cap_builtin](#Interpreter_cap_builtin) (function)
- [Interpreter_is_runtime_func](#Interpreter_is_runtime_func) (function)
- [Interpreter_is_runtime_func_uncached](#Interpreter_is_runtime_func_uncached) (function)
- [Interpreter_call_runtime](#Interpreter_call_runtime) (function)
- [interp_rr_effect](#interp_rr_effect) (function)
- [rr_hex](#rr_hex) (function)
- [rr_unhex](#rr_unhex) (function)
- [rr_hex_bytes](#rr_hex_bytes) (function)
- [rr_unhex_bytes](#rr_unhex_bytes) (function)
- [interp_rr_record_start](#interp_rr_record_start) (function)
- [interp_rr_flush](#interp_rr_flush) (function)
- [interp_rr_record_finish](#interp_rr_record_finish) (function)
- [interp_rr_replay_start](#interp_rr_replay_start) (function)
- [interp_rr_program](#interp_rr_program) (function)
- [interp_rr_source](#interp_rr_source) (function)
- [interp_rr_replay_finish](#interp_rr_replay_finish) (function)
- [interp_rr_exiting](#interp_rr_exiting) (function)
- [Interpreter_rr_call](#Interpreter_rr_call) (function)
- [interp_set_env](#interp_set_env) (function)
- [interp_set_program_args](#interp_set_program_args) (function)
- [interp_crash_report_setup](#interp_crash_report_setup) (function)
- [interp_crash_report_text](#interp_crash_report_text) (function)
- [interp_write_crash_report](#interp_write_crash_report) (function)
- [Interpreter_run_and_print](#Interpreter_run_and_print) (function)
- [Interpreter_coerce_str_to_int](#Interpreter_coerce_str_to_int) (function)
- [Interpreter_is_wrapping_func](#Interpreter_is_wrapping_func) (function)
- [Interpreter_stringify_value](#Interpreter_stringify_value) (function)
- [Interpreter_parse_persisted_value](#Interpreter_parse_persisted_value) (function)
- [Interpreter_arg_string](#Interpreter_arg_string) (function)
- [Interpreter_lookup_vec_value](#Interpreter_lookup_vec_value) (function)
- [Interpreter_read_raw_vec_i64](#Interpreter_read_raw_vec_i64) (function)
- [Interpreter_arg_to_vec_i64](#Interpreter_arg_to_vec_i64) (function)
- [Interpreter_vec_handle_i64s](#Interpreter_vec_handle_i64s) (function)
- [Interpreter_vec_handle_f64s](#Interpreter_vec_handle_f64s) (function)
- [Interpreter_new_vec_value](#Interpreter_new_vec_value) (function)
- [Interpreter_new_float_vec](#Interpreter_new_float_vec) (function)
- [Interpreter_new_core_f64_vec](#Interpreter_new_core_f64_vec) (function)
- [Interpreter_alloc_tensor](#Interpreter_alloc_tensor) (function)
- [Interpreter_tensor_handle](#Interpreter_tensor_handle) (function)
- [Interpreter_args_to_f64_vec](#Interpreter_args_to_f64_vec) (function)
- [Interpreter_fides_vec_to_value_vec](#Interpreter_fides_vec_to_value_vec) (function)
- [Interpreter_find_vec_addr](#Interpreter_find_vec_addr) (function)
- [Interpreter_format_panic_trace](#Interpreter_format_panic_trace) (function)
- [Interpreter_take_panic_stack_trace](#Interpreter_take_panic_stack_trace) (function)
- [Interpreter_get_call_stack](#Interpreter_get_call_stack) (function)
- [Interpreter_get_current_func_name](#Interpreter_get_current_func_name) (function)
- [Interpreter_get_memory_usage](#Interpreter_get_memory_usage) (function)
- [Interpreter_get_module](#Interpreter_get_module) (function)
- [Interpreter_get_thread_count](#Interpreter_get_thread_count) (function)
- [Interpreter_enable_profiling](#Interpreter_enable_profiling) (function)
- [Interpreter_enable_recording](#Interpreter_enable_recording) (function)
- [Interpreter_get_profile](#Interpreter_get_profile) (function)
- [Interpreter_get_trace](#Interpreter_get_trace) (function)
- [Interpreter_set_debug_hook](#Interpreter_set_debug_hook) (function)
- [Interpreter_cov_count](#Interpreter_cov_count) (function)
- [Interpreter_cov_fn](#Interpreter_cov_fn) (function)
- [Interpreter_cov_id](#Interpreter_cov_id) (function)
- [Interpreter_branch_report](#Interpreter_branch_report) (function)
- [Interpreter_save_persist_state](#Interpreter_save_persist_state) (function)
- [Interpreter_with_persist_file](#Interpreter_with_persist_file) (function)
- [wildcard_match](#wildcard_match) (function)
- [arg_vec_usize](#arg_vec_usize) (function)
- [arg_vec_i64](#arg_vec_i64) (function)
- [arg_vec_f64](#arg_vec_f64) (function)
- [simd_binary](#simd_binary) (function)
- [interpret](#interpret) (function)
- [interpret_with_options](#interpret_with_options) (function)
- [interpret_with_options_full](#interpret_with_options_full) (function)
- [interpret_with_options_full2](#interpret_with_options_full2) (function)
- [interp_limit_defaults](#interp_limit_defaults) (function)
- [interp_lim_setup](#interp_lim_setup) (function)
- [Interpreter_lim_cpu_check](#Interpreter_lim_cpu_check) (function)
- [Interpreter_lim_mem_over](#Interpreter_lim_mem_over) (function)
- [Interpreter_lim_str](#Interpreter_lim_str) (function)
- [interpret_ir_module](#interpret_ir_module) (function)
- [interpret_ir_module_with_options](#interpret_ir_module_with_options) (function)
- [ChanState](#ChanState) (struct)
- [DbRow](#DbRow) (struct)
- [EcsComp](#EcsComp) (struct)
- [Interpreter_arg_str_ok](#Interpreter_arg_str_ok) (function)
- [Interpreter_arg_str](#Interpreter_arg_str) (function)
- [json_escape_body](#json_escape_body) (function)
- [Interpreter_run_eager](#Interpreter_run_eager) (function)
- [Interpreter_ecs_comp](#Interpreter_ecs_comp) (function)
- [ITask](#ITask) (struct)
- [InterpCtx](#InterpCtx) (struct)
- [Interpreter_itask_stack_check](#Interpreter_itask_stack_check) (function)
- [Interpreter_ctx_take](#Interpreter_ctx_take) (function)
- [Interpreter_ctx_put](#Interpreter_ctx_put) (function)
- [Interpreter_host_switch](#Interpreter_host_switch) (function)
- [Interpreter_itask_wait_turn](#Interpreter_itask_wait_turn) (function)
- [Interpreter_itask_others](#Interpreter_itask_others) (function)
- [Interpreter_mutex_held](#Interpreter_mutex_held) (function)
- [Interpreter_itask_new](#Interpreter_itask_new) (function)
- [Interpreter_itask_start](#Interpreter_itask_start) (function)
- [Interpreter_itask_body](#Interpreter_itask_body) (function)
- [Interpreter_itask_of](#Interpreter_itask_of) (function)
- [Interpreter_itask_await](#Interpreter_itask_await) (function)
- [Interpreter_tok_get](#Interpreter_tok_get) (function)
- [Interpreter_tok_set](#Interpreter_tok_set) (function)
- [Interpreter_tok_cancelled](#Interpreter_tok_cancelled) (function)
- [Interpreter_tok_slice](#Interpreter_tok_slice) (function)
- [Interpreter_chan_must_wait](#Interpreter_chan_must_wait) (function)
- [Interpreter_chan_send_must_wait](#Interpreter_chan_send_must_wait) (function)
- [Interpreter_itask_builtin](#Interpreter_itask_builtin) (function)
- [Interpreter_call_runtime2](#Interpreter_call_runtime2) (function)
- [Interpreter_shared_get](#Interpreter_shared_get) (function)
- [Tensor_clone_data](#Tensor_clone_data) (function)
- [agop_add](#agop_add) (function)
- [agop_sub](#agop_sub) (function)
- [agop_mul](#agop_mul) (function)
- [agop_matmul](#agop_matmul) (function)
- [agop_scale](#agop_scale) (function)
- [agop_relu](#agop_relu) (function)
- [agop_sigmoid](#agop_sigmoid) (function)
- [agop_sum](#agop_sum) (function)
- [agop_mean](#agop_mean) (function)
- [AgEntry](#AgEntry) (struct)
- [Interpreter_record_ag](#Interpreter_record_ag) (function)
- [Interpreter_accum_grad](#Interpreter_accum_grad) (function)
- [Interpreter_run_backward](#Interpreter_run_backward) (function)
- [Interpreter_prng_f64](#Interpreter_prng_f64) (function)
- [Interpreter_prng_gaussian](#Interpreter_prng_gaussian) (function)
- [Interpreter_call_tensor](#Interpreter_call_tensor) (function)
- [interp_tensor_sigmoid](#interp_tensor_sigmoid) (function)
- [interp_tensor_softmax](#interp_tensor_softmax) (function)
- [interp_tensor_to_json](#interp_tensor_to_json) (function)
- [fmt_fixed4](#fmt_fixed4) (function)
- [interp_tensor_to_string](#interp_tensor_to_string) (function)
- [interp_quantize_q8](#interp_quantize_q8) (function)
- [interp_dequantize_q8](#interp_dequantize_q8) (function)
- [Interpreter_display_vec_quoted](#Interpreter_display_vec_quoted) (function)
- [Interpreter_libc_call](#Interpreter_libc_call) (function)
- [Interpreter_call_fn_value](#Interpreter_call_fn_value) (function)
- [Interpreter_libc_qsort](#Interpreter_libc_qsort) (function)
- [Interpreter_libc_bsearch](#Interpreter_libc_bsearch) (function)

## Value

```fides
pub enum Value {
```

**Variants:**

- `Int`
- `Float`
- `Bool`
- `Str`
- `Ptr`
- `Void`
- `Vec`
- `// index into interp.vec_side_table`
- `Map`

*Defined at line 39*

---

## Value_int

```fides
pub fn Value_int(v: i64) -> Value {
```

**Parameters:**

- `v`: `i64`

**Returns:** `Value`

*Defined at line 51*

---

## interp_reinterpret_word

```fides
pub fn interp_reinterpret_word(v: Value, ty: IrType) -> Value {
```

**Parameters:**

- `v`: `Value`
- `ty`: `IrType`

**Returns:** `Value`

*Defined at line 90*

---

## Value_float

```fides
pub fn Value_float(v: f64) -> Value {
```

**Parameters:**

- `v`: `f64`

**Returns:** `Value`

*Defined at line 134*

---

## Value_bool

```fides
pub fn Value_bool(v: bool) -> Value {
```

**Parameters:**

- `v`: `bool`

**Returns:** `Value`

*Defined at line 137*

---

## Value_str

```fides
pub fn Value_str(v: str) -> Value {
```

**Parameters:**

- `v`: `str`

**Returns:** `Value`

*Defined at line 140*

---

## Value_ptr

```fides
pub fn Value_ptr(v: i64) -> Value {
```

**Parameters:**

- `v`: `i64`

**Returns:** `Value`

*Defined at line 143*

---

## Value_void

```fides
pub fn Value_void() -> Value {
```

**Returns:** `Value`

*Defined at line 146*

---

## value_eq

Check if two values are equal (for PartialEq equivalent)

```fides
pub fn value_eq(a: Value, b: Value) -> bool {
```

**Parameters:**

- `a`: `Value`
- `b`: `Value`

**Returns:** `bool`

*Defined at line 151*

---

## value_is_void

Check if a value is Void (used for overflow/error detection)

```fides
pub fn value_is_void(v: Value) -> bool {
```

**Parameters:**

- `v`: `Value`

**Returns:** `bool`

*Defined at line 238*

---

## Memory

```fides
pub struct Memory {
```

**Fields:**

| Name | Type |
|------|------|
| `data` | `Vec<i64>` |
| `next_alloc` | `i64` |
| `blocks` | `I64Table` |
| `free_heads` | `Vec<i64>` |
| `cstr_cache` | `I64Table` |
| `lim_max` | `i64` |
| `lim_used` | `i64` |
| `lim_sizes` | `I64Table` |
| `lim_hit_used` | `i64` |
| `lim_hit_want` | `i64` |
| `oom_hit` | `i64` |
| `tx_marks` | `Vec<MemTxMark>` |
| `tx_serial` | `i64` |
| `tx_next` | `i64` |
| `tx_addr` | `Vec<i64>` |
| `tx_old` | `Vec<i64>` |
| `tx_seen` | `I64Table` |
| `tx_blk_addr` | `Vec<i64>` |
| `tx_blk_old` | `Vec<Value>` |
| `tx_blk_had` | `Vec<bool>` |
| `tx_head_cls` | `Vec<i64>` |
| `tx_head_old` | `Vec<i64>` |

*Defined at line 258*

---

## MemTxMark

BG229: where an open txn's part of Memory's write log starts, the
enclosing txn's id, and the allocator's words when it began.

```fides
pub struct MemTxMark {
```

**Fields:**

| Name | Type |
|------|------|
| `serial` | `i64` |
| `log_len` | `i64` |
| `blk_len` | `i64` |
| `head_len` | `i64` |
| `next_alloc` | `i64` |
| `oom_hit` | `i64` |

*Defined at line 314*

---

## Memory_new

```fides
pub fn Memory_new() -> Memory {
```

**Returns:** `Memory`

*Defined at line 509*

---

## Memory_put

BG229: byte `addr` (inside Memory.data) becomes `b`. Inside a txn the
byte it held is noted first, once per txn (Memory_tx_rollback puts it
back). Every write of Memory.data goes through here.

```fides
pub fn Memory_put(self: &mut Memory, addr: i64, b: i64) {
```

**Parameters:**

- `self`: `&mut Memory`
- `addr`: `i64`
- `b`: `i64`

*Defined at line 531*

---

## Memory_set_block

BG229: the block table's entry for `addr` becomes `v` (noted inside a
txn).

```fides
pub fn Memory_set_block(self: &mut Memory, addr: i64, v: Value) {
```

**Parameters:**

- `self`: `&mut Memory`
- `addr`: `i64`
- `v`: `Value`

*Defined at line 562*

---

## Memory_tx_depth

BG229: how many txn blocks are open.

```fides
pub fn Memory_tx_depth(self: &Memory) -> i64 {
```

**Parameters:**

- `self`: `&Memory`

**Returns:** `i64`

*Defined at line 591*

---

## Memory_tx_begin

BG229: a txn begins: from here every write is noted.

```fides
pub fn Memory_tx_begin(self: &mut Memory) {
```

**Parameters:**

- `self`: `&mut Memory`

*Defined at line 596*

---

## Memory_tx_commit

BG229: the innermost txn ends and keeps its writes (an enclosing txn
still holds their old values).

```fides
pub fn Memory_tx_commit(self: &mut Memory) {
```

**Parameters:**

- `self`: `&mut Memory`

*Defined at line 622*

---

## Memory_tx_rollback

BG229: the innermost txn ends and its writes are put back, newest
first: the bytes, the block table, the free lists and the heap's end.

```fides
pub fn Memory_tx_rollback(self: &mut Memory) {
```

**Parameters:**

- `self`: `&mut Memory`

*Defined at line 634*

---

## Memory_alloc

A zeroed block of at least `size` bytes, 8-byte aligned: a freed block
of its size class when there is one, else new memory. 0 when the
interpreter's memory (INTERP_MEM_LIMIT) is used up. The block is the
program's heap (FID_MEM_LIMIT counts it while it is live).

```fides
pub fn Memory_alloc(self: &mut Memory, size: i64) -> i64 {
```

**Parameters:**

- `self`: `&mut Memory`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 682*

---

## Memory_alloc_frame

A block of a call's frame (an alloca): as Memory_alloc, but the stack,
not the heap, so FID_MEM_LIMIT does not count it (C's frame is not
counted either). Freed with Memory_free_frame.

```fides
pub fn Memory_alloc_frame(self: &mut Memory, size: i64) -> i64 {
```

**Parameters:**

- `self`: `&mut Memory`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 689*

---

## Memory_free

Free a block: it goes on its size class's free list (a block marked 1
is never reused). A bad free fails as it does natively. Returns the
panic message, "" when the free is fine. Interpreter_free_block also
drops the side tables' entries for the block: free through it. A block
FID_MEM_LIMIT counted is taken off (also one marked 1: the program
freed it, though the interpreter does not reuse it).

```fides
pub fn Memory_free(self: &mut Memory, addr: i64) -> str {
```

**Parameters:**

- `self`: `&mut Memory`
- `addr`: `i64`

**Returns:** `str`

*Defined at line 801*

---

## Memory_free_frame

Free a block of a call's frame (Memory_alloc_frame): not counted.

```fides
pub fn Memory_free_frame(self: &mut Memory, addr: i64) -> str {
```

**Parameters:**

- `self`: `&mut Memory`
- `addr`: `i64`

**Returns:** `str`

*Defined at line 806*

---

## Memory_block_size

The size of the live block at `addr` (its size class's bytes), 0 when
`addr` is not one or is never reused.

```fides
pub fn Memory_block_size(self: &Memory, addr: i64) -> i64 {
```

**Parameters:**

- `self`: `&Memory`
- `addr`: `i64`

**Returns:** `i64`

*Defined at line 856*

---

## Memory_allocated

```fides
pub fn Memory_allocated(self: &Memory) -> i64 {
```

**Parameters:**

- `self`: `&Memory`

**Returns:** `i64`

*Defined at line 880*

---

## Memory_capacity

```fides
pub fn Memory_capacity(self: &Memory) -> i64 {
```

**Parameters:**

- `self`: `&Memory`

**Returns:** `i64`

*Defined at line 884*

---

## Memory_is_valid_addr

```fides
pub fn Memory_is_valid_addr(self: &Memory, addr: i64) -> bool {
```

**Parameters:**

- `self`: `&Memory`
- `addr`: `i64`

**Returns:** `bool`

*Defined at line 888*

---

## Memory_store_byte

```fides
pub fn Memory_store_byte(self: &mut Memory, addr: i64, b: i64) {
```

**Parameters:**

- `self`: `&mut Memory`
- `addr`: `i64`
- `b`: `i64`

*Defined at line 893*

---

## Memory_load_byte

```fides
pub fn Memory_load_byte(self: &Memory, addr: i64) -> i64 {
```

**Parameters:**

- `self`: `&Memory`
- `addr`: `i64`

**Returns:** `i64`

*Defined at line 913*

---

## Memory_store_i64

```fides
pub fn Memory_store_i64(self: &mut Memory, addr: i64, val: i64) {
```

**Parameters:**

- `self`: `&mut Memory`
- `addr`: `i64`
- `val`: `i64`

*Defined at line 921*

---

## Memory_load_i64

```fides
pub fn Memory_load_i64(self: &Memory, addr: i64) -> i64 {
```

**Parameters:**

- `self`: `&Memory`
- `addr`: `i64`

**Returns:** `i64`

*Defined at line 932*

---

## Memory_store_bool

```fides
pub fn Memory_store_bool(self: &mut Memory, addr: i64, val: bool) {
```

**Parameters:**

- `self`: `&mut Memory`
- `addr`: `i64`
- `val`: `bool`

*Defined at line 944*

---

## Memory_load_bool

```fides
pub fn Memory_load_bool(self: &Memory, addr: i64) -> bool {
```

**Parameters:**

- `self`: `&Memory`
- `addr`: `i64`

**Returns:** `bool`

*Defined at line 953*

---

## Memory_store_i32

```fides
pub fn Memory_store_i32(self: &mut Memory, addr: i64, val: i64) {
```

**Parameters:**

- `self`: `&mut Memory`
- `addr`: `i64`
- `val`: `i64`

*Defined at line 958*

---

## Memory_store_i16

```fides
pub fn Memory_store_i16(self: &mut Memory, addr: i64, val: i64) {
```

**Parameters:**

- `self`: `&mut Memory`
- `addr`: `i64`
- `val`: `i64`

*Defined at line 969*

---

## Memory_load_i32

```fides
pub fn Memory_load_i32(self: &Memory, addr: i64) -> i64 {
```

**Parameters:**

- `self`: `&Memory`
- `addr`: `i64`

**Returns:** `i64`

*Defined at line 975*

---

## Memory_store_f64

```fides
pub fn Memory_store_f64(self: &mut Memory, addr: i64, val: f64) {
```

**Parameters:**

- `self`: `&mut Memory`
- `addr`: `i64`
- `val`: `f64`

*Defined at line 991*

---

## f64_from_bits_i

```fides
pub fn f64_from_bits_i(bits: i64) -> f64 {
```

**Parameters:**

- `bits`: `i64`

**Returns:** `f64`

*Defined at line 998*

---

## f32_from_bits_i

```fides
pub fn f32_from_bits_i(bits: i64) -> f64 {
```

**Parameters:**

- `bits`: `i64`

**Returns:** `f64`

*Defined at line 1013*

---

## interp_round_f32

BG66: an f64 rounded to single precision (the nearest f32), the value an
f32 register holds after an operation or a conversion

```fides
pub fn interp_round_f32(f: f64) -> f64 {
```

**Parameters:**

- `f`: `f64`

**Returns:** `f64`

*Defined at line 1036*

---

## interp_narrow_int

BG38: the low `bits` bits of a word as a value of that width, signed
(sign-extended) or unsigned (zero-extended)

```fides
pub fn interp_narrow_int(i: i64, bits: i64, want_signed: bool) -> i64 {
```

**Parameters:**

- `i`: `i64`
- `bits`: `i64`
- `want_signed`: `bool`

**Returns:** `i64`

*Defined at line 1045*

---

## interp_scalar_ty

A number slot type (a local's, a parameter's, a field's): an integer, a
float or a bool.

```fides
pub fn interp_scalar_ty(t: IrType) -> bool {
```

**Parameters:**

- `t`: `IrType`

**Returns:** `bool`

*Defined at line 1064*

---

## Memory_load_f64

```fides
pub fn Memory_load_f64(self: &Memory, addr: i64) -> f64 {
```

**Parameters:**

- `self`: `&Memory`
- `addr`: `i64`

**Returns:** `f64`

*Defined at line 1091*

---

## Memory_store_f32

```fides
pub fn Memory_store_f32(self: &mut Memory, addr: i64, val: f64) {
```

**Parameters:**

- `self`: `&mut Memory`
- `addr`: `i64`
- `val`: `f64`

*Defined at line 1097*

---

## Memory_load_f32

```fides
pub fn Memory_load_f32(self: &Memory, addr: i64) -> f64 {
```

**Parameters:**

- `self`: `&Memory`
- `addr`: `i64`

**Returns:** `f64`

*Defined at line 1103*

---

## Memory_copy

```fides
pub fn Memory_copy(self: &mut Memory, dst: i64, src: i64, size: i64) {
```

**Parameters:**

- `self`: `&mut Memory`
- `dst`: `i64`
- `src`: `i64`
- `size`: `i64`

*Defined at line 1109*

---

## Memory_store_cstr

```fides
pub fn Memory_store_cstr(self: &mut Memory, s: str) -> i64 {
```

**Parameters:**

- `self`: `&mut Memory`
- `s`: `str`

**Returns:** `i64`

*Defined at line 1144*

---

## Memory_load_cstr

```fides
pub fn Memory_load_cstr(self: &Memory, addr: i64) -> str {
```

**Parameters:**

- `self`: `&Memory`
- `addr`: `i64`

**Returns:** `str`

*Defined at line 1222*

---

## ExecOutcome

```fides
pub enum ExecOutcome {
```

**Variants:**

- `Continue`
- `Value`
- `Jump`
- `Return`
- `Panic`

*Defined at line 1277*

---

## RunResult

```fides
pub enum RunResult {
```

**Variants:**

- `Return`
- `Panic`
- `Paused`

*Defined at line 1285*

---

## StringAddrEntry

```fides
pub struct StringAddrEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `content` | `str` |
| `addr` | `i64` |

*Defined at line 1297*

---

## StringAddrEntry_new

```fides
pub fn StringAddrEntry_new(content: str, addr: i64) -> StringAddrEntry {
```

**Parameters:**

- `content`: `str`
- `addr`: `i64`

**Returns:** `StringAddrEntry`

*Defined at line 1302*

---

## GlobalAddrEntry

```fides
pub struct GlobalAddrEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `addr` | `i64` |

*Defined at line 1308*

---

## GlobalAddrEntry_new

```fides
pub fn GlobalAddrEntry_new(name: str, addr: i64) -> GlobalAddrEntry {
```

**Parameters:**

- `name`: `str`
- `addr`: `i64`

**Returns:** `GlobalAddrEntry`

*Defined at line 1313*

---

## OpaqueEntry

```fides
pub struct OpaqueEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `addr` | `i64` |
| `value` | `Value` |

*Defined at line 1319*

---

## OpaqueEntry_new

```fides
pub fn OpaqueEntry_new(addr: i64, value: Value) -> OpaqueEntry {
```

**Parameters:**

- `addr`: `i64`
- `value`: `Value`

**Returns:** `OpaqueEntry`

*Defined at line 1324*

---

## I64Table

```fides
pub struct I64Table {
```

**Fields:**

| Name | Type |
|------|------|
| `keys` | `Vec<i64>` |
| `vals` | `Vec<Value>` |
| `state` | `Vec<i64>` |
| `live` | `i64` |
| `filled` | `i64` |

*Defined at line 1335*

---

## I64Table_new

```fides
pub fn I64Table_new() -> I64Table {
```

**Returns:** `I64Table`

*Defined at line 1343*

---

## I64Table_find

```fides
pub fn I64Table_find(t: &I64Table, key: i64) -> i64 {
```

**Parameters:**

- `t`: `&I64Table`
- `key`: `i64`

**Returns:** `i64`

*Defined at line 1375*

---

## I64Table_get

```fides
pub fn I64Table_get(t: &I64Table, key: i64) -> OptValue {
```

**Parameters:**

- `t`: `&I64Table`
- `key`: `i64`

**Returns:** `OptValue`

*Defined at line 1394*

---

## I64Table_set

```fides
pub fn I64Table_set(t: &mut I64Table, key: i64, val: Value) {
```

**Parameters:**

- `t`: `&mut I64Table`
- `key`: `i64`
- `val`: `Value`

*Defined at line 1420*

---

## I64Table_remove

```fides
pub fn I64Table_remove(t: &mut I64Table, key: i64) {
```

**Parameters:**

- `t`: `&mut I64Table`
- `key`: `i64`

*Defined at line 1443*

---

## I64Table_clone

```fides
pub fn I64Table_clone(t: &I64Table) -> I64Table {
```

**Parameters:**

- `t`: `&I64Table`

**Returns:** `I64Table`

*Defined at line 1452*

---

## StrTable

```fides
pub struct StrTable {
```

**Fields:**

| Name | Type |
|------|------|
| `keys` | `Vec<str>` |
| `vals` | `Vec<i64>` |
| `used` | `Vec<bool>` |
| `count` | `i64` |

*Defined at line 1463*

---

## StrTable_new

```fides
pub fn StrTable_new(cap: i64) -> StrTable {
```

**Parameters:**

- `cap`: `i64`

**Returns:** `StrTable`

*Defined at line 1470*

---

## StrTable_find

Slot of `key`, or -1.

```fides
pub fn StrTable_find(t: &StrTable, key: str) -> i64 {
```

**Parameters:**

- `t`: `&StrTable`
- `key`: `str`

**Returns:** `i64`

*Defined at line 1505*

---

## StrTable_get

```fides
pub fn StrTable_get(t: &StrTable, key: str, dflt: i64) -> i64 {
```

**Parameters:**

- `t`: `&StrTable`
- `key`: `str`
- `dflt`: `i64`

**Returns:** `i64`

*Defined at line 1517*

---

## StrTable_set

```fides
pub fn StrTable_set(t: &mut StrTable, key: str, val: i64) {
```

**Parameters:**

- `t`: `&mut StrTable`
- `key`: `str`
- `val`: `i64`

*Defined at line 1525*

---

## ArrayBoundEntry

```fides
pub struct ArrayBoundEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `addr` | `i64` |
| `size` | `i64` |

*Defined at line 1559*

---

## ArrayBoundEntry_new

```fides
pub fn ArrayBoundEntry_new(addr: i64, size: i64) -> ArrayBoundEntry {
```

**Parameters:**

- `addr`: `i64`
- `size`: `i64`

**Returns:** `ArrayBoundEntry`

*Defined at line 1564*

---

## VecSideEntry

```fides
pub struct VecSideEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `vec_addr` | `i64` |
| `side_idx` | `i64` |

*Defined at line 1570*

---

## VecSideEntry_new

```fides
pub fn VecSideEntry_new(vec_addr: i64, side_idx: i64) -> VecSideEntry {
```

**Parameters:**

- `vec_addr`: `i64`
- `side_idx`: `i64`

**Returns:** `VecSideEntry`

*Defined at line 1575*

---

## VecValueEntry

```fides
pub struct VecValueEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `handle` | `i64` |
| `elements` | `Vec<Value>` |

*Defined at line 1582*

---

## VecValueEntry_new

```fides
pub fn VecValueEntry_new(handle: i64) -> VecValueEntry {
```

**Parameters:**

- `handle`: `i64`

**Returns:** `VecValueEntry`

*Defined at line 1587*

---

## MapValueEntry

```fides
pub struct MapValueEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `handle` | `i64` |
| `keys` | `Vec<str>` |
| `vals` | `Vec<Value>` |

*Defined at line 1594*

---

## TensorSideEntry

```fides
pub struct TensorSideEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `addr` | `i64` |
| `handle` | `i64` |

*Defined at line 1602*

---

## TensorSideEntry_new

```fides
pub fn TensorSideEntry_new(addr: i64, handle: i64) -> TensorSideEntry {
```

**Parameters:**

- `addr`: `i64`
- `handle`: `i64`

**Returns:** `TensorSideEntry`

*Defined at line 1607*

---

## MapValueEntry_new

```fides
pub fn MapValueEntry_new(handle: i64) -> MapValueEntry {
```

**Parameters:**

- `handle`: `i64`

**Returns:** `MapValueEntry`

*Defined at line 1613*

---

## Interpreter

```fides
pub struct Interpreter {
```

**Fields:**

| Name | Type |
|------|------|
| `module` | `IrModule` |
| `memory` | `Memory` |
| `string_addrs` | `Vec<StringAddrEntry>` |
| `global_addrs` | `Vec<GlobalAddrEntry>` |
| `opaque_values` | `I64Table` |
| `dyn_types` | `I64Table` |
| `fn_cache` | `StrTable` |
| `rt_cache` | `StrTable` |
| `cur_wrapping` | `bool` |
| `vec_side_table` | `Vec<VecSideEntry>` |
| `vec_values` | `Vec<VecValueEntry>` |
| `map_values` | `Vec<MapValueEntry>` |
| `tensors` | `Vec<TensorData>` |
| `tensor_side_table` | `Vec<TensorSideEntry>` |
| `tensor_index` | `I64Table` |
| `kv_caches` | `Vec<KvCache>` |
| `verbose` | `bool` |
| `debug_mode` | `bool` |
| `cov_on` | `bool` |
| `cov_seen` | `i64` |
| `cov_fns` | `Vec<str>` |
| `cov_ids` | `Vec<i64>` |
| `cov_br_seen` | `i64` |
| `array_bounds` | `I64Table` |
| `frame_allocas` | `Vec<i64>` |
| `current_func_name` | `str` |
| `call_stack` | `Vec<str>` |
| `call_site_lines` | `Vec<i64>` |
| `pending_call_site` | `OptU64` |
| `panic_stack_trace` | `OptStr` |
| `interp_unsafe_depth` | `i64` |
| `txn_op_addr` | `Vec<i64>` |
| `txn_op_old` | `Vec<Value>` |
| `txn_op_had` | `Vec<bool>` |
| `txn_op_marks` | `Vec<i64>` |
| `max_recursion` | `i64` |
| `max_steps` | `i64` |
| `step_count` | `i64` |
| `max_memory` | `i64` |
| `region_stack` | `Vec<str>` |
| `prng_state_a` | `u64` |
| `prng_state_b` | `u64` |
| `dp_epsilon_budget` | `f64` |
| `dp_epsilon_limit` | `f64` |
| `thread_handle_count` | `i64` |
| `future_values` | `Vec<Value>` |
| `future_exec` | `Vec<i64>` |
| `executor_count` | `i64` |
| `shared_mem` | `I64Table` |
| `next_shared_addr` | `i64` |
| `next_mutex_handle` | `i64` |
| `tls` | `I64Table` |
| `next_tls_key` | `i64` |
| `chans` | `Vec<ChanState>` |
| `dl_handles` | `I64Table` |
| `next_dl_handle` | `i64` |
| `ecs_entity_count` | `i64` |
| `ecs_comps` | `Vec<EcsComp>` |
| `sb_strs` | `Vec<str>` |
| `db_rows` | `Vec<DbRow>` |
| `next_db_handle` | `i64` |
| `closed_dbs` | `Vec<i64>` |
| `alloc_ptr` | `i64` |
| `persist_keys` | `Vec<str>` |
| `persist_vals` | `Vec<str>` |
| `ag_enabled` | `bool` |
| `ag_checkpoint` | `bool` |
| `ag_tape` | `Vec<AgEntry>` |
| `ag_grad_keys` | `Vec<i64>` |
| `ag_grad_vals` | `Vec<TensorData>` |
| `gt_scheduler` | `GreenThreadScheduler` |
| `revoked_caps` | `Vec<str>` |
| `cap_marks` | `Vec<i64>` |
| `itasks` | `Vec<ITask>` |
| `itask_cur` | `i64` |
| `itask_pending` | `Vec<i64>` |
| `itask_pending_at` | `i64` |
| `itask_live` | `i64` |
| `itask_open` | `i64` |

*Defined at line 1627*

---

## Interpreter_new

```fides
pub fn Interpreter_new(module: IrModule) -> Interpreter {
```

**Parameters:**

- `module`: `IrModule`

**Returns:** `Interpreter`

*Defined at line 1737*

---

## Interpreter_get_function

IrModule_get_function through the name index.

```fides
pub fn Interpreter_get_function(self: &Interpreter, name: str) -> OptIrFunction {
```

**Parameters:**

- `self`: `&Interpreter`
- `name`: `str`

**Returns:** `OptIrFunction`

*Defined at line 1756*

---

## Interpreter_verbose

```fides
pub fn Interpreter_verbose(self: &mut Interpreter) {
```

**Parameters:**

- `self`: `&mut Interpreter`

*Defined at line 1764*

---

## Interpreter_set_max_recursion

```fides
pub fn Interpreter_set_max_recursion(self: &mut Interpreter, depth: i64) {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `depth`: `i64`

*Defined at line 1768*

---

## Interpreter_set_max_steps

```fides
pub fn Interpreter_set_max_steps(self: &mut Interpreter, steps: i64) {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `steps`: `i64`

*Defined at line 1772*

---

## Interpreter_set_max_memory

```fides
pub fn Interpreter_set_max_memory(self: &mut Interpreter, bytes: i64) {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `bytes`: `i64`

*Defined at line 1776*

---

## Interpreter_lookup_string_addr

```fides
pub fn Interpreter_lookup_string_addr(self: &Interpreter, content: str) -> i64 {
```

**Parameters:**

- `self`: `&Interpreter`
- `content`: `str`

**Returns:** `i64`

*Defined at line 1784*

---

## Interpreter_lookup_global_addr

```fides
pub fn Interpreter_lookup_global_addr(self: &Interpreter, name: str) -> i64 {
```

**Parameters:**

- `self`: `&Interpreter`
- `name`: `str`

**Returns:** `i64`

*Defined at line 1795*

---

## Interpreter_lookup_opaque

```fides
pub fn Interpreter_lookup_opaque(self: &Interpreter, addr: i64) -> OptValue {
```

**Parameters:**

- `self`: `&Interpreter`
- `addr`: `i64`

**Returns:** `OptValue`

*Defined at line 1806*

---

## Interpreter_set_opaque

```fides
pub fn Interpreter_set_opaque(self: &mut Interpreter, addr: i64, value: Value) {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `addr`: `i64`
- `value`: `Value`

*Defined at line 1810*

---

## Interpreter_remove_opaque

```fides
pub fn Interpreter_remove_opaque(self: &mut Interpreter, addr: i64) {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `addr`: `i64`

*Defined at line 1815*

---

## Interpreter_txn_begin

BG229: a txn block begins (Inst::TxnBegin). Nothing is copied: from
here the writes are logged (Memory_put, Interpreter_opaque_note).

```fides
pub fn Interpreter_txn_begin(self: &mut Interpreter) {
```

**Parameters:**

- `self`: `&mut Interpreter`

*Defined at line 1841*

---

## Interpreter_txn_commit

BG229: the innermost txn ends and keeps its changes (Inst::TxnEnd).

```fides
pub fn Interpreter_txn_commit(self: &mut Interpreter) {
```

**Parameters:**

- `self`: `&mut Interpreter`

*Defined at line 1847*

---

## Interpreter_txn_rollback

BG229: the innermost txn ends and what it changed is put back (a panic
inside it).

```fides
pub fn Interpreter_txn_rollback(self: &mut Interpreter) {
```

**Parameters:**

- `self`: `&mut Interpreter`

*Defined at line 1862*

---

## Interpreter_txn_unwind

BG229: close the txn blocks opened since `depth` were open: a call
returning from inside one (a `return`, `break` or `continue` out of
a txn skips its TxnEnd) keeps their changes; a panic puts them back.

```fides
pub fn Interpreter_txn_unwind(self: &mut Interpreter, depth: i64, back: bool) {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `depth`: `i64`
- `back`: `bool`

*Defined at line 1888*

---

## Interpreter_lookup_array_bound

```fides
pub fn Interpreter_lookup_array_bound(self: &Interpreter, addr: i64) -> i64 {
```

**Parameters:**

- `self`: `&Interpreter`
- `addr`: `i64`

**Returns:** `i64`

*Defined at line 1898*

---

## Interpreter_set_array_bound

```fides
pub fn Interpreter_set_array_bound(self: &mut Interpreter, addr: i64, size: i64) {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `addr`: `i64`
- `size`: `i64`

*Defined at line 1915*

---

## Interpreter_free_block

Free a block of interpreted memory (p_free, a returning call's allocas):
its side-table entries go, and Memory_free puts it on a free list.
Returns Memory_free's panic message ("" when the free is fine).

```fides
pub fn Interpreter_free_block(self: &mut Interpreter, addr: i64) -> str {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `addr`: `i64`

**Returns:** `str`

*Defined at line 1941*

---

## Interpreter_free_frame_block

Free a dead alloca of a returning call (Memory_alloc_frame): as
Interpreter_free_block, but its bytes were never the heap's.

```fides
pub fn Interpreter_free_frame_block(self: &mut Interpreter, addr: i64) -> str {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `addr`: `i64`

**Returns:** `str`

*Defined at line 1951*

---

## OptValue

```fides
pub enum OptValue {
```

**Variants:**

- `Some`
- `None`

*Defined at line 1960*

---

## OptStr

```fides
pub enum OptStr {
```

**Variants:**

- `Some`
- `None`

*Defined at line 1966*

---

## OptVecI64

```fides
pub enum OptVecI64 {
```

**Variants:**

- `Some`
- `None`

*Defined at line 1972*

---

## OptVecF64

```fides
pub enum OptVecF64 {
```

**Variants:**

- `Some`
- `None`

*Defined at line 1978*

---

## Interpreter_init_strings

```fides
pub fn Interpreter_init_strings(self: &mut Interpreter) {
```

**Parameters:**

- `self`: `&mut Interpreter`

*Defined at line 1987*

---

## Interpreter_init_globals

```fides
pub fn Interpreter_init_globals(self: &mut Interpreter) {
```

**Parameters:**

- `self`: `&mut Interpreter`

*Defined at line 1998*

---

## eval_global_init

```fides
pub fn eval_global_init(expr: Expr, ty: IrType) -> Value {
```

**Parameters:**

- `expr`: `Expr`
- `ty`: `IrType`

**Returns:** `Value`

*Defined at line 2040*

---

## Interpreter_value_to_string

```fides
pub fn Interpreter_value_to_string(self: &Interpreter, v: Value) -> str {
```

**Parameters:**

- `self`: `&Interpreter`
- `v`: `Value`

**Returns:** `str`

*Defined at line 2072*

---

## Interpreter_value_bytes

```fides
pub fn Interpreter_value_bytes(self: &Interpreter, v: Value, n: i64) -> Vec<i64> {
```

**Parameters:**

- `self`: `&Interpreter`
- `v`: `Value`
- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 2169*

---

## Interpreter_display_value

Rust's `Display for Value`.

```fides
pub fn Interpreter_display_value(self: &Interpreter, v: Value) -> str {
```

**Parameters:**

- `self`: `&Interpreter`
- `v`: `Value`

**Returns:** `str`

*Defined at line 2229*

---

## sorted_key_order

Indices of `keys` in ascending string order (insertion sort).

```fides
pub fn sorted_key_order(keys: Vec<str>) -> Vec<i64> {
```

**Parameters:**

- `keys`: `Vec<str>`

**Returns:** `Vec<i64>`

*Defined at line 2293*

---

## str_lt

Byte-wise string ordering (Rust's str Ord).

```fides
pub fn str_lt(a: str, b: str) -> bool {
```

**Parameters:**

- `a`: `str`
- `b`: `str`

**Returns:** `bool`

*Defined at line 2310*

---

## Interpreter_print_via

Print `v` with the Fides formatter `fname` (Vec_to_str, HashMap_to_str).
Continue / Panic when handled; Value(Void) when the caller should fall
back to the default printing.

```fides
pub fn Interpreter_print_via(self: &mut Interpreter, fname: str, v: Value) -> ExecOutcome {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `fname`: `str`
- `v`: `Value`

**Returns:** `ExecOutcome`

*Defined at line 2328*

---

## Interpreter_format_call_stack

```fides
pub fn Interpreter_format_call_stack(self: &Interpreter) -> str {
```

**Parameters:**

- `self`: `&Interpreter`

**Returns:** `str`

*Defined at line 2365*

---

## Interpreter_run

```fides
pub fn Interpreter_run(self: &mut Interpreter) -> RunResult {
```

**Parameters:**

- `self`: `&mut Interpreter`

**Returns:** `RunResult`

*Defined at line 2396*

---

## Interpreter_run_function

```fides
pub fn Interpreter_run_function(self: &mut Interpreter, func: IrFunction, args: Vec<Value>) -> RunResult {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `func`: `IrFunction`
- `args`: `Vec<Value>`

**Returns:** `RunResult`

*Defined at line 2412*

---

## Interpreter_value_word

The 8-byte word a raw (core) Vec stores for `v`.

```fides
pub fn Interpreter_value_word(self: &mut Interpreter, v: Value) -> i64 {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `v`: `Value`

**Returns:** `i64`

*Defined at line 2942*

---

## get_value

```fides
pub fn get_value(values: Vec<Value>, id: i64) -> Value {
```

**Parameters:**

- `values`: `Vec<Value>`
- `id`: `i64`

**Returns:** `Value`

*Defined at line 2970*

---

## Interpreter_execute_inst

```fides
pub fn Interpreter_execute_inst(self: &mut Interpreter, inst: Inst, inst_id: i64, values: Vec<Value>, func: &IrFunction) -> ExecOutcome {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `inst`: `Inst`
- `inst_id`: `i64`
- `values`: `Vec<Value>`
- `func`: `&IrFunction`

**Returns:** `ExecOutcome`

*Defined at line 2981*

---

## Interpreter_eval_binop

```fides
pub fn Interpreter_eval_binop(self: &Interpreter, op: BinOp, l: Value, r: Value) -> Value {
```

**Parameters:**

- `self`: `&Interpreter`
- `op`: `BinOp`
- `l`: `Value`
- `r`: `Value`

**Returns:** `Value`

*Defined at line 4792*

---

## Interpreter_eval_unaryop

```fides
pub fn Interpreter_eval_unaryop(self: &Interpreter, op: UnaryOp, v: Value) -> Value {
```

**Parameters:**

- `self`: `&Interpreter`
- `op`: `UnaryOp`
- `v`: `Value`

**Returns:** `Value`

*Defined at line 5206*

---

## Interpreter_cast_value

```fides
pub fn Interpreter_cast_value(self: &Interpreter, v: Value, to: IrType) -> Value {
```

**Parameters:**

- `self`: `&Interpreter`
- `v`: `Value`
- `to`: `IrType`

**Returns:** `Value`

*Defined at line 5252*

---

## interp_builtin_cap

The capability runtime builtin `name` needs ("" for none); the same
families as the type checker's (compiler.fd repl_call_cap). Allocation is
not one of them: core allocates for every string and Vec.

```fides
pub fn interp_builtin_cap(name: str) -> str {
```

**Parameters:**

- `name`: `str`

**Returns:** `str`

*Defined at line 5402*

---

## Interpreter_cap_is_revoked

Whether capability `cap` is revoked.

```fides
pub fn Interpreter_cap_is_revoked(self: &Interpreter, cap: str) -> bool {
```

**Parameters:**

- `self`: `&Interpreter`
- `cap`: `str`

**Returns:** `bool`

*Defined at line 5422*

---

## Interpreter_cap_denied

The capability call `name` needs when it is revoked, else "": a
builtin's (interp_builtin_cap), or "FFI" for a C function the module
declares extern.

```fides
pub fn Interpreter_cap_denied(self: &Interpreter, name: str) -> str {
```

**Parameters:**

- `self`: `&Interpreter`
- `name`: `str`

**Returns:** `str`

*Defined at line 5436*

---

## interp_sandbox_panic

The panic of a call to `name`, which needs revoked capability `cap`.

```fides
pub fn interp_sandbox_panic(name: str, cap: str) -> ExecOutcome {
```

**Parameters:**

- `name`: `str`
- `cap`: `str`

**Returns:** `ExecOutcome`

*Defined at line 5458*

---

## interp_is_cap_builtin

Whether `name` is one of the revocation builtins.

```fides
pub fn interp_is_cap_builtin(name: str) -> bool {
```

**Parameters:**

- `name`: `str`

**Returns:** `bool`

*Defined at line 5463*

---

## Interpreter_cap_builtin

Run revocation builtin `name` (interp_is_cap_builtin) on `args`.

```fides
pub fn Interpreter_cap_builtin(self: &mut Interpreter, name: str, args: Vec<Value>) -> ExecOutcome {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `name`: `str`
- `args`: `Vec<Value>`

**Returns:** `ExecOutcome`

*Defined at line 5468*

---

## Interpreter_is_runtime_func

```fides
pub fn Interpreter_is_runtime_func(self: &mut Interpreter, name: str) -> bool {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `name`: `str`

**Returns:** `bool`

*Defined at line 5531*

---

## Interpreter_is_runtime_func_uncached

```fides
pub fn Interpreter_is_runtime_func_uncached(self: &Interpreter, name: str) -> bool {
```

**Parameters:**

- `self`: `&Interpreter`
- `name`: `str`

**Returns:** `bool`

*Defined at line 5545*

---

## Interpreter_call_runtime

```fides
pub fn Interpreter_call_runtime(self: &mut Interpreter, name: str, args: &mut Vec<Value>) -> ExecOutcome {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `name`: `str`
- `args`: `&mut Vec<Value>`

**Returns:** `ExecOutcome`

*Defined at line 5937*

---

## interp_rr_effect

```fides
pub fn interp_rr_effect(name: str) -> bool {
```

**Parameters:**

- `name`: `str`

**Returns:** `bool`

*Defined at line 8927*

---

## rr_hex

CT7: `s` as hex ("-" for "").

```fides
pub fn rr_hex(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 8970*

---

## rr_unhex

CT7: the text of hex `h` (rr_hex).

```fides
pub fn rr_unhex(h: str) -> str {
```

**Parameters:**

- `h`: `str`

**Returns:** `str`

*Defined at line 8987*

---

## rr_hex_bytes

CT7: bytes `b` (each 0..255) as hex ("-" for none); a byte 0 is kept,
which a text cannot hold.

```fides
pub fn rr_hex_bytes(b: Vec<i64>) -> str {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `str`

*Defined at line 9002*

---

## rr_unhex_bytes

CT7: the bytes of hex `h` (rr_hex_bytes).

```fides
pub fn rr_unhex_bytes(h: str) -> Vec<i64> {
```

**Parameters:**

- `h`: `str`

**Returns:** `Vec<i64>`

*Defined at line 9018*

---

## interp_rr_record_start

CT7: start recording into `path` the run of `program` (its text
`source`). False when the file cannot be written.

```fides
pub fn interp_rr_record_start(path: str, program: str, source: str) -> bool {
```

**Parameters:**

- `path`: `str`
- `program`: `str`
- `source`: `str`

**Returns:** `bool`

*Defined at line 9113*

---

## interp_rr_flush

CT7: write the recorded calls not written yet.

```fides
pub fn interp_rr_flush() {
```

*Defined at line 9125*

---

## interp_rr_record_finish

CT7: the recorded run ended with exit code `code`: the rest of the
trace and its `exit` line are written, and recording stops.

```fides
pub fn interp_rr_record_finish(code: i64) {
```

**Parameters:**

- `code`: `i64`

*Defined at line 9141*

---

## interp_rr_replay_start

CT7: read trace `text` and answer the effect calls of the next run from
it ("" when it is a trace; else why not). `steps`: print each answered
call on stderr.

```fides
pub fn interp_rr_replay_start(text: str, steps: bool) -> str {
```

**Parameters:**

- `text`: `str`
- `steps`: `bool`

**Returns:** `str`

*Defined at line 9153*

---

## interp_rr_program

```fides
pub fn interp_rr_program() -> str {
```

**Returns:** `str`

*Defined at line 9190*

---

## interp_rr_source

```fides
pub fn interp_rr_source() -> str {
```

**Returns:** `str`

*Defined at line 9194*

---

## interp_rr_replay_finish

CT7: the replayed run ended with exit code `code`: "" when it used every
recorded call and ended as the recorded run did; else what differs.

```fides
pub fn interp_rr_replay_finish(code: i64) -> str {
```

**Parameters:**

- `code`: `i64`

**Returns:** `str`

*Defined at line 9200*

---

## interp_rr_exiting

CT7: the program ends the process now with `code` (p_exit, a panic in a
task): a recording gets its rest and its `exit` line; a replay is
checked against the recording here, since the driver never sees its end
(a difference is said on stderr, and a run that would exit 0 exits 1).

```fides
pub fn interp_rr_exiting(code: i64) {
```

**Parameters:**

- `code`: `i64`

*Defined at line 9218*

---

## Interpreter_rr_call

CT7: the real call (recording) or the trace's answer (replaying) of
effect builtin `name`.

```fides
pub fn Interpreter_rr_call(self: &mut Interpreter, name: str, args: &mut Vec<Value>) -> ExecOutcome {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `name`: `str`
- `args`: `&mut Vec<Value>`

**Returns:** `ExecOutcome`

*Defined at line 9236*

---

## interp_set_env

```fides
pub fn interp_set_env(name: str, value: str) {
```

**Parameters:**

- `name`: `str`
- `value`: `str`

*Defined at line 9430*

---

## interp_set_program_args

2g.60: the program's argument 0 is the process's argument `arg0`, its
argument 1 the process's argument `first` (-1, 0: the process's own).

```fides
pub fn interp_set_program_args(arg0: i64, first: i64) {
```

**Parameters:**

- `arg0`: `i64`
- `first`: `i64`

*Defined at line 9456*

---

## interp_crash_report_setup

CO9: where a panic's report goes ("" for FID_CRASH_REPORT or none),
which build runs (`version`) and the program's file.

```fides
pub fn interp_crash_report_setup(path: str, version: str, program: str) {
```

**Parameters:**

- `path`: `str`
- `version`: `str`
- `program`: `str`

*Defined at line 9533*

---

## interp_crash_report_text

CO9: the text of a crash report for panic `msg` with stack trace
`trace` (Interpreter_format_call_stack's text, "" for none).

```fides
pub fn interp_crash_report_text(msg: str, trace: str, version: str, program: str) -> str {
```

**Parameters:**

- `msg`: `str`
- `trace`: `str`
- `version`: `str`
- `program`: `str`

**Returns:** `str`

*Defined at line 9541*

---

## interp_write_crash_report

CO9: write the crash report of panic `msg` (trace `trace`) when one was
asked for; says where on stderr.

```fides
pub fn interp_write_crash_report(msg: str, trace: str) {
```

**Parameters:**

- `msg`: `str`
- `trace`: `str`

*Defined at line 9568*

---

## Interpreter_run_and_print

Run main, as `fidesc run` does: the exit code is main's integer result
(0 otherwise); a panic prints "Panic: <message>" and the stack trace.

```fides
pub fn Interpreter_run_and_print(self: &mut Interpreter) -> i64 {
```

**Parameters:**

- `self`: `&mut Interpreter`

**Returns:** `i64`

*Defined at line 9595*

---

## Interpreter_coerce_str_to_int

coerce_str_to_int — materialize a Str value into memory and return Int(addr).
Used when a string value is passed to an I64/U64 parameter.

```fides
pub fn Interpreter_coerce_str_to_int(self: &mut Interpreter, v: Value) -> Value {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `v`: `Value`

**Returns:** `Value`

*Defined at line 9638*

---

## Interpreter_is_wrapping_func

is_wrapping_func — check if the currently executing function is @wrapping.

```fides
pub fn Interpreter_is_wrapping_func(self: &Interpreter) -> bool {
```

**Parameters:**

- `self`: `&Interpreter`

**Returns:** `bool`

*Defined at line 9651*

---

## Interpreter_stringify_value

stringify_value — serialize a Value to a type-prefixed string for persistence.
Format: "i:42", "b:true", "f:3.14", "s:hello", "p:0x1234", "pv:elem,elem"

```fides
pub fn Interpreter_stringify_value(self: &Interpreter, val: Value) -> str {
```

**Parameters:**

- `self`: `&Interpreter`
- `val`: `Value`

**Returns:** `str`

*Defined at line 9657*

---

## Interpreter_parse_persisted_value

parse_persisted_value — parse a type-prefixed string back into a Value.

```fides
pub fn Interpreter_parse_persisted_value(self: &Interpreter, saved: str, default: Value) -> Value {
```

**Parameters:**

- `self`: `&Interpreter`
- `saved`: `str`
- `default`: `Value`

**Returns:** `Value`

*Defined at line 9700*

---

## Interpreter_arg_string

arg_string — extract a string from a Value argument.
Handles Value::Str, Value::Ptr (load C string from memory), and
Value::Int (check opaque table or load from memory).

```fides
pub fn Interpreter_arg_string(self: &Interpreter, v: Value) -> OptStr {
```

**Parameters:**

- `self`: `&Interpreter`
- `v`: `Value`

**Returns:** `OptStr`

*Defined at line 9730*

---

## Interpreter_lookup_vec_value

lookup_vec_value — look up a Vec Value from an Int/Ptr address.
Checks the vec_side_table first, then the opaque_values table.

```fides
pub fn Interpreter_lookup_vec_value(self: &Interpreter, arg: Value) -> OptValue {
```

**Parameters:**

- `self`: `&Interpreter`
- `arg`: `Value`

**Returns:** `OptValue`

*Defined at line 9777*

---

## Interpreter_read_raw_vec_i64

read_raw_vec_i64 — read a raw Fides Vec struct from memory.
Vec is 32 bytes: { data: i64, len: i64, cap: i64, code word }.
Elements are at their size (interp_vec_ld; D14).

```fides
pub fn Interpreter_read_raw_vec_i64(self: &Interpreter, addr: i64) -> OptVecI64 {
```

**Parameters:**

- `self`: `&Interpreter`
- `addr`: `i64`

**Returns:** `OptVecI64`

*Defined at line 9819*

---

## Interpreter_arg_to_vec_i64

/ arg_to_vec_i64 — extract a Vec<i64> from an argument.
Handles Value::Vec (interpreter builtin) and Value::Int/Ptr (raw Fides Vec).

```fides
pub fn Interpreter_arg_to_vec_i64(self: &Interpreter, arg: Value) -> OptVecI64 {
```

**Parameters:**

- `self`: `&Interpreter`
- `arg`: `Value`

**Returns:** `OptVecI64`

*Defined at line 9844*

---

## Interpreter_vec_handle_i64s

The elements of side-table Vec `h` as i64 (non-Int elements read as 0).

```fides
pub fn Interpreter_vec_handle_i64s(self: &Interpreter, h: i64) -> Vec<i64> {
```

**Parameters:**

- `self`: `&Interpreter`
- `h`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 9895*

---

## Interpreter_vec_handle_f64s

The elements of side-table Vec `h` as f64: an Int is a stored f64 word.

```fides
pub fn Interpreter_vec_handle_f64s(self: &Interpreter, h: i64, int_is_bits: bool) -> Vec<f64> {
```

**Parameters:**

- `self`: `&Interpreter`
- `h`: `i64`
- `int_is_bits`: `bool`

**Returns:** `Vec<f64>`

*Defined at line 9917*

---

## Interpreter_new_vec_value

A new side-table Vec holding `elems`.

```fides
pub fn Interpreter_new_vec_value(self: &mut Interpreter, elems: Vec<Value>) -> Value {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `elems`: `Vec<Value>`

**Returns:** `Value`

*Defined at line 9946*

---

## Interpreter_new_float_vec

A side-table Vec of floats.

```fides
pub fn Interpreter_new_float_vec(self: &mut Interpreter, fs: Vec<f64>) -> Value {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `fs`: `Vec<f64>`

**Returns:** `Value`

*Defined at line 9955*

---

## Interpreter_new_core_f64_vec

A core Vec<f64> in the interpreter's memory holding `fs` (as the
program's own Vecs are: the 40-byte header {data, len, cap, code, tid},
8-byte elements, each the f64's bits), for a builtin whose result the
program keeps: the lowering hands it to core's Vec_share / Vec_get /
Vec_push, which read the header, so a side-table Value::Vec failed there
("Arithmetic error (vec, int)" in Vec_share). The buffer is a plain
block, as p_string_split's: core gives it a share box of its own.

```fides
pub fn Interpreter_new_core_f64_vec(self: &mut Interpreter, fs: Vec<f64>) -> Value {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `fs`: `Vec<f64>`

**Returns:** `Value`

*Defined at line 9972*

---

## Interpreter_alloc_tensor

alloc_tensor — store a Tensor in the side table and return an opaque Ptr.
Value::Ptr(addr) stands in for Rust's Value::Tensor (see file header).

```fides
pub fn Interpreter_alloc_tensor(self: &mut Interpreter, t: TensorData) -> Value {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `t`: `TensorData`

**Returns:** `Value`

*Defined at line 10009*

---

## Interpreter_tensor_handle

tensor_handle — resolve a Value::Ptr/Int addr to a tensors index, or -1.

```fides
pub fn Interpreter_tensor_handle(self: &Interpreter, arg: Value) -> i64 {
```

**Parameters:**

- `self`: `&Interpreter`
- `arg`: `Value`

**Returns:** `i64`

*Defined at line 10022*

---

## Interpreter_args_to_f64_vec

args_to_f64_vec — extract a Vec<f64> from args[idx].
Handles Value::Vec and Value::Int/Ptr (Fides Vec via fides_vec_to_value_vec).

```fides
pub fn Interpreter_args_to_f64_vec(self: &mut Interpreter, args: Vec<Value>, idx: i64) -> OptVecF64 {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `args`: `Vec<Value>`
- `idx`: `i64`

**Returns:** `OptVecF64`

*Defined at line 10055*

---

## Interpreter_fides_vec_to_value_vec

fides_vec_to_value_vec — convert a Fides Vec (i64 pointer) to a Value::Vec
by running the program's Vec_len / Vec_get.

```fides
pub fn Interpreter_fides_vec_to_value_vec(self: &mut Interpreter, vec_ptr: i64) -> Value {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `vec_ptr`: `i64`

**Returns:** `Value`

*Defined at line 10102*

---

## Interpreter_find_vec_addr

find_vec_addr — find the Vec struct address for a Value::Vec by scanning vec_side_table.

```fides
pub fn Interpreter_find_vec_addr(self: &Interpreter, v: Value) -> i64 {
```

**Parameters:**

- `self`: `&Interpreter`
- `v`: `Value`

**Returns:** `i64`

*Defined at line 10152*

---

## Interpreter_format_panic_trace

```fides
pub fn Interpreter_format_panic_trace(self: &Interpreter) -> OptStr {
```

**Parameters:**

- `self`: `&Interpreter`

**Returns:** `OptStr`

*Defined at line 10170*

---

## Interpreter_take_panic_stack_trace

take_panic_stack_trace — consume the panic stack trace.

```fides
pub fn Interpreter_take_panic_stack_trace(self: &mut Interpreter) -> OptStr {
```

**Parameters:**

- `self`: `&mut Interpreter`

**Returns:** `OptStr`

*Defined at line 10182*

---

## Interpreter_get_call_stack

get_call_stack — return a copy of the call stack.

```fides
pub fn Interpreter_get_call_stack(self: &Interpreter) -> Vec<str> {
```

**Parameters:**

- `self`: `&Interpreter`

**Returns:** `Vec<str>`

*Defined at line 10189*

---

## Interpreter_get_current_func_name

get_current_func_name — return the current function name.

```fides
pub fn Interpreter_get_current_func_name(self: &Interpreter) -> str {
```

**Parameters:**

- `self`: `&Interpreter`

**Returns:** `str`

*Defined at line 10200*

---

## Interpreter_get_memory_usage

get_memory_usage — return (allocated, capacity) as packed i64.

```fides
pub fn Interpreter_get_memory_usage(self: &Interpreter) -> i64 {
```

**Parameters:**

- `self`: `&Interpreter`

**Returns:** `i64`

*Defined at line 10205*

---

## Interpreter_get_module

get_module — return a reference to the IR module.
In Fides, we return a copy since references are limited.

```fides
pub fn Interpreter_get_module(self: &Interpreter) -> IrModule {
```

**Parameters:**

- `self`: `&Interpreter`

**Returns:** `IrModule`

*Defined at line 10213*

---

## Interpreter_get_thread_count

get_thread_count — return the number of threads (always 1 for interpreter).

```fides
pub fn Interpreter_get_thread_count(self: &Interpreter) -> i64 {
```

**Parameters:**

- `self`: `&Interpreter`

**Returns:** `i64`

*Defined at line 10218*

---

## Interpreter_enable_profiling

enable_profiling — enable profiling mode.

```fides
pub fn Interpreter_enable_profiling(self: &mut Interpreter) {
```

**Parameters:**

- `self`: `&mut Interpreter`

*Defined at line 10223*

---

## Interpreter_enable_recording

enable_recording — enable trace recording.

```fides
pub fn Interpreter_enable_recording(self: &mut Interpreter) {
```

**Parameters:**

- `self`: `&mut Interpreter`

*Defined at line 10228*

---

## Interpreter_get_profile

get_profile — return profiling data (simplified: empty).

```fides
pub fn Interpreter_get_profile(self: &Interpreter) -> Vec<str> {
```

**Parameters:**

- `self`: `&Interpreter`

**Returns:** `Vec<str>`

*Defined at line 10233*

---

## Interpreter_get_trace

get_trace — return trace data (simplified: empty).

```fides
pub fn Interpreter_get_trace(self: &Interpreter) -> Vec<str> {
```

**Parameters:**

- `self`: `&Interpreter`

**Returns:** `Vec<str>`

*Defined at line 10238*

---

## Interpreter_set_debug_hook

set_debug_hook — record coverage: every instruction run, once
(Interpreter_cov_count / _cov_fn / _cov_id read it back; fid veil).

```fides
pub fn Interpreter_set_debug_hook(self: &mut Interpreter) {
```

**Parameters:**

- `self`: `&mut Interpreter`

*Defined at line 10244*

---

## Interpreter_cov_count

```fides
pub fn Interpreter_cov_count(self: &Interpreter) -> i64 {
```

**Parameters:**

- `self`: `&Interpreter`

**Returns:** `i64`

*Defined at line 10249*

---

## Interpreter_cov_fn

```fides
pub fn Interpreter_cov_fn(self: &Interpreter, i: i64) -> str {
```

**Parameters:**

- `self`: `&Interpreter`
- `i`: `i64`

**Returns:** `str`

*Defined at line 10252*

---

## Interpreter_cov_id

```fides
pub fn Interpreter_cov_id(self: &Interpreter, i: i64) -> i64 {
```

**Parameters:**

- `self`: `&Interpreter`
- `i`: `i64`

**Returns:** `i64`

*Defined at line 10255*

---

## Interpreter_branch_report

CT8 branch coverage, after a run with Interpreter_set_debug_hook: every
conditional branch of the functions named in `user_fns` (all functions
when it is empty) that has a source line counts two ways, taken and not
taken. The text is a "Branches:" summary line, then one line per way
never taken ("  f line 12: true never taken").

```fides
pub fn Interpreter_branch_report(self: &Interpreter, user_fns: Vec<str>) -> str {
```

**Parameters:**

- `self`: `&Interpreter`
- `user_fns`: `Vec<str>`

**Returns:** `str`

*Defined at line 10264*

---

## Interpreter_save_persist_state

save_persist_state — save persistent state (simplified: no-op).

```fides
pub fn Interpreter_save_persist_state(self: &Interpreter) {  // No-op in self-hosted interpreter
```

**Parameters:**

- `self`: `&Interpreter`

*Defined at line 10335*

---

## Interpreter_with_persist_file

with_persist_file — set persist file path (simplified: no-op).

```fides
pub fn Interpreter_with_persist_file(self: &mut Interpreter, path: str) {  // No-op in self-hosted interpreter
```

**Parameters:**

- `self`: `&mut Interpreter`
- `path`: `str`

*Defined at line 10339*

---

## wildcard_match

wildcard_match — simple wildcard pattern matching.
? matches any single char, * matches zero or more chars.

```fides
pub fn wildcard_match(s: str, pattern: str) -> bool {
```

**Parameters:**

- `s`: `str`
- `pattern`: `str`

**Returns:** `bool`

*Defined at line 10344*

---

## arg_vec_usize

arg_vec_usize — extract a Vec<usize> from args[idx] (simplified).

```fides
pub fn arg_vec_usize(args: Vec<Value>, idx: i64) -> OptVecI64 {
```

**Parameters:**

- `args`: `Vec<Value>`
- `idx`: `i64`

**Returns:** `OptVecI64`

*Defined at line 10384*

---

## arg_vec_i64

arg_vec_i64 — extract a Vec<i64> from args[idx] (simplified).

```fides
pub fn arg_vec_i64(args: Vec<Value>, idx: i64) -> OptVecI64 {
```

**Parameters:**

- `args`: `Vec<Value>`
- `idx`: `i64`

**Returns:** `OptVecI64`

*Defined at line 10399*

---

## arg_vec_f64

arg_vec_f64 — extract a Vec<f64> from args[idx] (simplified).

```fides
pub fn arg_vec_f64(args: Vec<Value>, idx: i64) -> OptVecF64 {
```

**Parameters:**

- `args`: `Vec<Value>`
- `idx`: `i64`

**Returns:** `OptVecF64`

*Defined at line 10414*

---

## simd_binary

simd_binary — SIMD binary operation helper.

```fides
pub fn simd_binary(args: Vec<Value>, f: fn(f64, f64) -> f64) -> ExecOutcome {
```

**Parameters:**

- `args`: `Vec<Value>`
- `f`: `fn(f64`

**Returns:** `ExecOutcome`

*Defined at line 10429*

---

## interpret

interpret — convenience function: lower and interpret a program.

```fides
pub fn interpret(module: IrModule, verbose: bool, release: bool) -> i64 {
```

**Parameters:**

- `module`: `IrModule`
- `verbose`: `bool`
- `release`: `bool`

**Returns:** `i64`

*Defined at line 10435*

---

## interpret_with_options

interpret_with_options — interpret with configurable max_recursion.

```fides
pub fn interpret_with_options(module: IrModule, verbose: bool, release: bool, max_recursion: i64) -> i64 {
```

**Parameters:**

- `module`: `IrModule`
- `verbose`: `bool`
- `release`: `bool`
- `max_recursion`: `i64`

**Returns:** `i64`

*Defined at line 10440*

---

## interpret_with_options_full

interpret_with_options_full — full version with max_steps.

```fides
pub fn interpret_with_options_full(module: IrModule, verbose: bool, release: bool, max_recursion: i64, max_steps: i64) -> i64 {
```

**Parameters:**

- `module`: `IrModule`
- `verbose`: `bool`
- `release`: `bool`
- `max_recursion`: `i64`
- `max_steps`: `i64`

**Returns:** `i64`

*Defined at line 10447*

---

## interpret_with_options_full2

interpret_with_options_full2 — full version with max_steps and max_memory.

```fides
pub fn interpret_with_options_full2(module: IrModule, verbose: bool, release: bool, max_recursion: i64, max_steps: i64, max_memory: i64) -> i64 {
```

**Parameters:**

- `module`: `IrModule`
- `verbose`: `bool`
- `release`: `bool`
- `max_recursion`: `i64`
- `max_steps`: `i64`
- `max_memory`: `i64`

**Returns:** `i64`

*Defined at line 10453*

---

## interp_limit_defaults

The limits `fid run --max-mem` / `--cpu-limit` give (bytes, ms; 0:
none): used when FID_MEM_LIMIT / FID_CPU_LIMIT is not set.

```fides
pub fn interp_limit_defaults(mem_bytes: i64, cpu_ms: i64) {
```

**Parameters:**

- `mem_bytes`: `i64`
- `cpu_ms`: `i64`

*Defined at line 10524*

---

## interp_lim_setup

roadmap P12: read FID_MEM_LIMIT and FID_CPU_LIMIT (or the --max-mem /
--cpu-limit defaults) for the program `self` is about to run. A value
that is not a number stops fid with exit code 2, as a compiled program
stops at start-up.

```fides
pub fn interp_lim_setup(self: &mut Interpreter) {
```

**Parameters:**

- `self`: `&mut Interpreter`

*Defined at line 10717*

---

## Interpreter_lim_cpu_check

FID_CPU_LIMIT: the program's CPU time so far against the limit.

```fides
pub fn Interpreter_lim_cpu_check(self: &mut Interpreter) {
```

**Parameters:**

- `self`: `&mut Interpreter`

*Defined at line 10769*

---

## Interpreter_lim_mem_over

FID_MEM_LIMIT: an allocation took the program past it (Memory_lim_take
noted it): the C runtime's message, the interpreted stack, exit 125.

```fides
pub fn Interpreter_lim_mem_over(self: &mut Interpreter) {
```

**Parameters:**

- `self`: `&mut Interpreter`

*Defined at line 10778*

---

## Interpreter_lim_str

FID_MEM_LIMIT and a string the interpreter builds as its own value
(not in its memory yet): one that cannot fit beside what is in use (as
its block, `len` + 17 bytes, would not in a compiled program) stops the
program as an allocation past the limit does. Returns `s`.

```fides
pub fn Interpreter_lim_str(self: &mut Interpreter, s: str) -> str {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `s`: `str`

**Returns:** `str`

*Defined at line 10791*

---

## interpret_ir_module

```fides
pub fn interpret_ir_module(ir_mod: IrModule) -> i64 {
```

**Parameters:**

- `ir_mod`: `IrModule`

**Returns:** `i64`

*Defined at line 10806*

---

## interpret_ir_module_with_options

```fides
pub fn interpret_ir_module_with_options(ir_mod: IrModule, max_recursion: i64, max_steps: i64, max_memory: i64, verbose: bool) -> i64 {
```

**Parameters:**

- `ir_mod`: `IrModule`
- `max_recursion`: `i64`
- `max_steps`: `i64`
- `max_memory`: `i64`
- `verbose`: `bool`

**Returns:** `i64`

*Defined at line 10812*

---

## ChanState

```fides
pub struct ChanState {
```

**Fields:**

| Name | Type |
|------|------|
| `buf` | `Vec<i64>` |
| `head` | `i64` |
| `cap` | `i64` |
| `closed` | `bool` |

*Defined at line 10837*

---

## DbRow

```fides
pub struct DbRow {
```

**Fields:**

| Name | Type |
|------|------|
| `db` | `i64` |
| `key` | `str` |
| `val` | `Value` |

*Defined at line 10844*

---

## EcsComp

```fides
pub struct EcsComp {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `table` | `I64Table` |

*Defined at line 10850*

---

## Interpreter_arg_str_ok

Rust's arg_string: Some(text) for a Str / Ptr / string-pointer Int.

```fides
pub fn Interpreter_arg_str_ok(self: &Interpreter, v: Value) -> bool {
```

**Parameters:**

- `self`: `&Interpreter`
- `v`: `Value`

**Returns:** `bool`

*Defined at line 10978*

---

## Interpreter_arg_str

```fides
pub fn Interpreter_arg_str(self: &Interpreter, v: Value) -> str {
```

**Parameters:**

- `self`: `&Interpreter`
- `v`: `Value`

**Returns:** `str`

*Defined at line 10989*

---

## json_escape_body

JSON string-literal escaping (Rust's p_json_string / http esc).

```fides
pub fn json_escape_body(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 11025*

---

## Interpreter_run_eager

Run `func_name` eagerly with `targs` (a spawned thread / task); the
thread's return value. A panic in it ends the program (status 1).

```fides
pub fn Interpreter_run_eager(self: &mut Interpreter, func_name: str, targs: Vec<Value>, what: str) -> Value {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `func_name`: `str`
- `targs`: `Vec<Value>`
- `what`: `str`

**Returns:** `Value`

*Defined at line 11080*

---

## Interpreter_ecs_comp

```fides
pub fn Interpreter_ecs_comp(self: &mut Interpreter, name: str) -> i64 {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `name`: `str`

**Returns:** `i64`

*Defined at line 11125*

---

## ITask

An interpreted task: its function and arguments, its result once done.

```fides
pub struct ITask {
```

**Fields:**

| Name | Type |
|------|------|
| `func` | `str` |
| `args` | `Vec<Value>` |
| `what` | `str` |
| `started` | `bool` |
| `done` | `bool` |
| `result` | `Value` |
| `token` | `i64` |

*Defined at line 11247*

---

## InterpCtx

What belongs to one call chain: set aside while another task runs.

```fides
pub struct InterpCtx {
```

**Fields:**

| Name | Type |
|------|------|
| `func_name` | `str` |
| `wrapping` | `bool` |
| `call_stack` | `Vec<str>` |
| `call_site_lines` | `Vec<i64>` |
| `pending_call_site` | `OptU64` |
| `frame_allocas` | `Vec<i64>` |
| `region_stack` | `Vec<str>` |
| `unsafe_depth` | `i64` |
| `cur` | `i64` |
| `top` | `i64` |

*Defined at line 11259*

---

## Interpreter_itask_stack_check

In a task with a stack budget: a panic message once its calls have
taken more of the stack than that, else "".

```fides
pub fn Interpreter_itask_stack_check(self: &Interpreter, fname: str) -> str {
```

**Parameters:**

- `self`: `&Interpreter`
- `fname`: `str`

**Returns:** `str`

*Defined at line 11335*

---

## Interpreter_ctx_take

The running call chain's state, taken out (the interpreter is left with
a fresh one).

```fides
pub fn Interpreter_ctx_take(self: &mut Interpreter) -> InterpCtx {
```

**Parameters:**

- `self`: `&mut Interpreter`

**Returns:** `InterpCtx`

*Defined at line 11353*

---

## Interpreter_ctx_put

Put back what Interpreter_ctx_take took.

```fides
pub fn Interpreter_ctx_put(self: &mut Interpreter, c: InterpCtx) {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `c`: `InterpCtx`

*Defined at line 11371*

---

## Interpreter_host_switch

A call into the C runtime's scheduler, which runs other tasks
meanwhile: `op` 0 yields, 1 sleeps `a` ms, 2 runs every task to its
end, 3 waits for socket `a` (`b` 0 read, 1 write) at most `c` ms.

```fides
pub fn Interpreter_host_switch(self: &mut Interpreter, op: i64, a: i64, b: i64, c: i64) -> i64 {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `op`: `i64`
- `a`: `i64`
- `b`: `i64`
- `c`: `i64`

**Returns:** `i64`

*Defined at line 11387*

---

## Interpreter_itask_wait_turn

One turn of a wait that has to wait: the others run. The first turns
hand over at once (main, which is no task, runs one round of them);
later ones sleep a millisecond, so a long wait does not spin.

```fides
pub fn Interpreter_itask_wait_turn(self: &mut Interpreter, spins: i64) {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `spins`: `i64`

*Defined at line 11437*

---

## Interpreter_itask_others

Whether a task other than the running one may still run (so waiting
for it is not waiting forever).

```fides
pub fn Interpreter_itask_others(self: &Interpreter) -> bool {
```

**Parameters:**

- `self`: `&Interpreter`

**Returns:** `bool`

*Defined at line 11451*

---

## Interpreter_mutex_held

BG225: whether mutex `mk` (a p_mutex_init handle) is locked.

```fides
pub fn Interpreter_mutex_held(self: &Interpreter, mk: i64) -> bool {
```

**Parameters:**

- `self`: `&Interpreter`
- `mk`: `i64`

**Returns:** `bool`

*Defined at line 11463*

---

## Interpreter_itask_new

A new task of `func` with `targs`; not started.

```fides
pub fn Interpreter_itask_new(self: &mut Interpreter, func: str, targs: Vec<Value>, what: str) -> i64 {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `func`: `str`
- `targs`: `Vec<Value>`
- `what`: `str`

**Returns:** `i64`

*Defined at line 11483*

---

## Interpreter_itask_start

Make task `idx` run: when tasks interleave on a stack of its own
(`now`: entered at once until it first waits, else at the next switch),
else to its end here and now.

```fides
pub fn Interpreter_itask_start(self: &mut Interpreter, idx: i64, now: bool) {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `idx`: `i64`
- `now`: `bool`

*Defined at line 11493*

---

## Interpreter_itask_body

Task `idx` from its start to its end, then the next one waiting for a
stack starts.

```fides
pub fn Interpreter_itask_body(self: &mut Interpreter, idx: i64) {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `idx`: `i64`

*Defined at line 11576*

---

## Interpreter_itask_of

The task a handle names, or -1: a tagged future (p_co_async,
p_thread_spawn) or, unless `tagged_only`, a p_co_go handle.

```fides
pub fn Interpreter_itask_of(self: &Interpreter, v: Value, tagged_only: bool) -> i64 {
```

**Parameters:**

- `self`: `&Interpreter`
- `v`: `Value`
- `tagged_only`: `bool`

**Returns:** `i64`

*Defined at line 11606*

---

## Interpreter_itask_await

Wait until task `idx` has ended (the others run meanwhile); its result.

```fides
pub fn Interpreter_itask_await(self: &mut Interpreter, idx: i64) -> Value {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `idx`: `i64`

**Returns:** `Value`

*Defined at line 11668*

---

## Interpreter_tok_get

The token in the running task's slot (p_task_token), 0 if none.

```fides
pub fn Interpreter_tok_get(self: &Interpreter) -> i64 {
```

**Parameters:**

- `self`: `&Interpreter`

**Returns:** `i64`

*Defined at line 11708*

---

## Interpreter_tok_set

Put `tok` in the running task's slot (p_task_set_token); the one it held.

```fides
pub fn Interpreter_tok_set(self: &mut Interpreter, tok: i64) -> i64 {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `tok`: `i64`

**Returns:** `i64`

*Defined at line 11716*

---

## Interpreter_tok_cancelled

Whether `tok`, or a token it replaced, is cancelled or past its
deadline (a token past its deadline is marked cancelled).

```fides
pub fn Interpreter_tok_cancelled(self: &mut Interpreter, tok: i64) -> bool {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `tok`: `i64`

**Returns:** `bool`

*Defined at line 11731*

---

## Interpreter_tok_slice

The next slice of a wait of `left` ms (-1: no limit) under `tok`: 10 ms
at most, and no later than the first deadline of its chain.

```fides
pub fn Interpreter_tok_slice(self: &Interpreter, tok: i64, left: i64) -> i64 {
```

**Parameters:**

- `self`: `&Interpreter`
- `tok`: `i64`
- `left`: `i64`

**Returns:** `i64`

*Defined at line 11751*

---

## Interpreter_chan_must_wait

Whether a receive on channel `ch` has to wait: it is empty and open,
and someone may still send -- another task, or main when a task waits
(main runs again once the tasks let it).

```fides
pub fn Interpreter_chan_must_wait(self: &Interpreter, ch: i64) -> bool {
```

**Parameters:**

- `self`: `&Interpreter`
- `ch`: `i64`

**Returns:** `bool`

*Defined at line 11779*

---

## Interpreter_chan_send_must_wait

BG74: whether a send on channel `ch` has to wait: it holds `cap` values
and is open, and someone may still receive (as Interpreter_chan_must_wait).

```fides
pub fn Interpreter_chan_send_must_wait(self: &Interpreter, ch: i64) -> bool {
```

**Parameters:**

- `self`: `&Interpreter`
- `ch`: `i64`

**Returns:** `bool`

*Defined at line 11795*

---

## Interpreter_itask_builtin

The task builtins (LG11); Jump(-99) when `name` is none of them.

```fides
pub fn Interpreter_itask_builtin(self: &mut Interpreter, name: str, args: &mut Vec<Value>) -> ExecOutcome {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `name`: `str`
- `args`: `&mut Vec<Value>`

**Returns:** `ExecOutcome`

*Defined at line 11810*

---

## Interpreter_call_runtime2

```fides
pub fn Interpreter_call_runtime2(self: &mut Interpreter, name: str, args: &mut Vec<Value>) -> ExecOutcome {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `name`: `str`
- `args`: `&mut Vec<Value>`

**Returns:** `ExecOutcome`

*Defined at line 11996*

---

## Interpreter_shared_get

```fides
pub fn Interpreter_shared_get(self: &Interpreter, addr: i64) -> i64 {
```

**Parameters:**

- `self`: `&Interpreter`
- `addr`: `i64`

**Returns:** `i64`

*Defined at line 13500*

---

## Tensor_clone_data

```fides
pub fn Tensor_clone_data(t: &TensorData) -> TensorData {
```

**Parameters:**

- `t`: `&TensorData`

**Returns:** `TensorData`

*Defined at line 13517*

---

## agop_add

```fides
pub fn agop_add() -> i64 {
```

**Returns:** `i64`

*Defined at line 13526*

---

## agop_sub

```fides
pub fn agop_sub() -> i64 {
```

**Returns:** `i64`

*Defined at line 13529*

---

## agop_mul

```fides
pub fn agop_mul() -> i64 {
```

**Returns:** `i64`

*Defined at line 13532*

---

## agop_matmul

```fides
pub fn agop_matmul() -> i64 {
```

**Returns:** `i64`

*Defined at line 13535*

---

## agop_scale

```fides
pub fn agop_scale() -> i64 {
```

**Returns:** `i64`

*Defined at line 13538*

---

## agop_relu

```fides
pub fn agop_relu() -> i64 {
```

**Returns:** `i64`

*Defined at line 13541*

---

## agop_sigmoid

```fides
pub fn agop_sigmoid() -> i64 {
```

**Returns:** `i64`

*Defined at line 13544*

---

## agop_sum

```fides
pub fn agop_sum() -> i64 {
```

**Returns:** `i64`

*Defined at line 13547*

---

## agop_mean

```fides
pub fn agop_mean() -> i64 {
```

**Returns:** `i64`

*Defined at line 13550*

---

## AgEntry

```fides
pub struct AgEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `result` | `i64` |
| `op` | `i64` |
| `a` | `i64` |
| `b` | `i64` |
| `s` | `f64` |
| `a_shape` | `Vec<i64>` |
| `b_shape` | `Vec<i64>` |
| `a_size` | `i64` |
| `data` | `Vec<f64>` |
| `has_input` | `bool` |
| `input` | `Vec<f64>` |

*Defined at line 13554*

---

## Interpreter_record_ag

```fides
pub fn Interpreter_record_ag(self: &mut Interpreter, e: AgEntry) {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `e`: `AgEntry`

*Defined at line 13588*

---

## Interpreter_accum_grad

accum_grad: add `g` into the gradient for `addr`.

```fides
pub fn Interpreter_accum_grad(self: &mut Interpreter, keys: &mut Vec<i64>, vals: &mut Vec<TensorData>, addr: i64, g: TensorData) {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `keys`: `&mut Vec<i64>`
- `vals`: `&mut Vec<TensorData>`
- `addr`: `i64`
- `g`: `TensorData`

*Defined at line 13599*

---

## Interpreter_run_backward

```fides
pub fn Interpreter_run_backward(self: &mut Interpreter) {
```

**Parameters:**

- `self`: `&mut Interpreter`

*Defined at line 13632*

---

## Interpreter_prng_f64

```fides
pub fn Interpreter_prng_f64(self: &mut Interpreter) -> f64 {
```

**Parameters:**

- `self`: `&mut Interpreter`

**Returns:** `f64`

*Defined at line 13753*

---

## Interpreter_prng_gaussian

```fides
pub fn Interpreter_prng_gaussian(self: &mut Interpreter) -> f64 {
```

**Parameters:**

- `self`: `&mut Interpreter`

**Returns:** `f64`

*Defined at line 13767*

---

## Interpreter_call_tensor

```fides
pub fn Interpreter_call_tensor(self: &mut Interpreter, name: str, args: Vec<Value>) -> ExecOutcome {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `name`: `str`
- `args`: `Vec<Value>`

**Returns:** `ExecOutcome`

*Defined at line 13773*

---

## interp_tensor_sigmoid

```fides
pub fn interp_tensor_sigmoid(t: &TensorData) -> TensorData {
```

**Parameters:**

- `t`: `&TensorData`

**Returns:** `TensorData`

*Defined at line 14017*

---

## interp_tensor_softmax

```fides
pub fn interp_tensor_softmax(t: &TensorData) -> TensorData {
```

**Parameters:**

- `t`: `&TensorData`

**Returns:** `TensorData`

*Defined at line 14027*

---

## interp_tensor_to_json

```fides
pub fn interp_tensor_to_json(t: &TensorData) -> str {
```

**Parameters:**

- `t`: `&TensorData`

**Returns:** `str`

*Defined at line 14066*

---

## fmt_fixed4

Rust's `{:.4}` (round half away from zero on the binary value).

```fides
pub fn fmt_fixed4(v: f64) -> str {
```

**Parameters:**

- `v`: `f64`

**Returns:** `str`

*Defined at line 14105*

---

## interp_tensor_to_string

src/tensor.rs Tensor::to_string

```fides
pub fn interp_tensor_to_string(t: &TensorData) -> str {
```

**Parameters:**

- `t`: `&TensorData`

**Returns:** `str`

*Defined at line 14170*

---

## interp_quantize_q8

```fides
pub fn interp_quantize_q8(t: &TensorData, scale: f64, zp: i64) -> TensorData {
```

**Parameters:**

- `t`: `&TensorData`
- `scale`: `f64`
- `zp`: `i64`

**Returns:** `TensorData`

*Defined at line 14176*

---

## interp_dequantize_q8

```fides
pub fn interp_dequantize_q8(t: &TensorData, scale: f64, zp: i64) -> TensorData {
```

**Parameters:**

- `t`: `&TensorData`
- `scale`: `f64`
- `zp`: `i64`

**Returns:** `TensorData`

*Defined at line 14200*

---

## Interpreter_display_vec_quoted

A side-table Vec as `[a, "s", ...]` (string elements quoted).

```fides
pub fn Interpreter_display_vec_quoted(self: &Interpreter, h: i64) -> str {
```

**Parameters:**

- `self`: `&Interpreter`
- `h`: `i64`

**Returns:** `str`

*Defined at line 14238*

---

## Interpreter_libc_call

ExecOutcome::Jump(-99): a builtin dispatcher did not know the name.
An `extern "C"` call to the C library that the interpreter can make
through its own runtime (getenv, printf of a plain string, system, exit,
getpid); Jump(-99) for any other.

```fides
pub fn Interpreter_libc_call(self: &mut Interpreter, c_name: str, args: Vec<Value>) -> ExecOutcome {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `c_name`: `str`
- `args`: `Vec<Value>`

**Returns:** `ExecOutcome`

*Defined at line 15027*

---

## Interpreter_call_fn_value

Call a function value (a closure env, or a function's name) with
`cargs` as CallPtr does; ExecOutcome::Value holds the result. For the C
library calls that take a callback (qsort, bsearch).

```fides
pub fn Interpreter_call_fn_value(self: &mut Interpreter, fv: Value, cargs: Vec<Value>) -> ExecOutcome {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `fv`: `Value`
- `cargs`: `Vec<Value>`

**Returns:** `ExecOutcome`

*Defined at line 15170*

---

## Interpreter_libc_qsort

qsort(base, n, size, cmp): an insertion sort in place, `cmp` called on
the addresses of the array's own elements as C's qsort calls it.

```fides
pub fn Interpreter_libc_qsort(self: &mut Interpreter, args: Vec<Value>) -> ExecOutcome {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `args`: `Vec<Value>`

**Returns:** `ExecOutcome`

*Defined at line 15296*

---

## Interpreter_libc_bsearch

bsearch(key, base, n, size, cmp): the element `cmp(key, element)` finds
equal (glibc's halving order), or a null pointer.

```fides
pub fn Interpreter_libc_bsearch(self: &mut Interpreter, args: Vec<Value>) -> ExecOutcome {
```

**Parameters:**

- `self`: `&mut Interpreter`
- `args`: `Vec<Value>`

**Returns:** `ExecOutcome`

*Defined at line 15339*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
