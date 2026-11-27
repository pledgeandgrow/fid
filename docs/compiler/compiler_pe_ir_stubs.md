[All modules](index.md)

# compiler::pe_ir_stubs -- Fides Documentation

## Contents

- [CThunk](#CThunk) (struct)
- [CAdapter](#CAdapter) (struct)
- [CAdapter_new](#CAdapter_new) (function)
- [pe_c_by_value](#pe_c_by_value) (function)
- [DynTypeId](#DynTypeId) (struct)
- [UnresolvedCall](#UnresolvedCall) (struct)
- [COverride](#COverride) (struct)
- [CThunk_new](#CThunk_new) (function)
- [COverride_new](#COverride_new) (function)
- [PeirGen](#PeirGen) (struct)
- [PeFcFix](#PeFcFix) (struct)
- [PeFcEntry](#PeFcEntry) (struct)
- [PEFC_LOCAL:](#PEFC_LOCAL:) (const)
- [PEFC_NAME:](#PEFC_NAME:) (const)
- [PEFC_STR:](#PEFC_STR:) (const)
- [PEFC_GLOBAL:](#PEFC_GLOBAL:) (const)
- [PEFC_IAT:](#PEFC_IAT:) (const)
- [PEFC_ALLOC:](#PEFC_ALLOC:) (const)
- [PEFC_PANIC:](#PEFC_PANIC:) (const)
- [PEFC_CRASH:](#PEFC_CRASH:) (const)
- [PeirGen_new](#PeirGen_new) (function)
- [pe_primitive_names](#pe_primitive_names) (function)
- [prim_interface_names](#prim_interface_names) (function)
- [pe_verify_on](#pe_verify_on) (function)
- [PeirGen_resolve_call](#PeirGen_resolve_call) (function)
- [PeirGen_call_builtin](#PeirGen_call_builtin) (function)
- [PeirGen_dyn_type_id](#PeirGen_dyn_type_id) (function)
- [PeirGen_call_runtime](#PeirGen_call_runtime) (function)
- [PeirGen_place_fn](#PeirGen_place_fn) (function)
- [PeirGen_str_off](#PeirGen_str_off) (function)
- [PeirGen_iat_off](#PeirGen_iat_off) (function)
- [PEIR_BARE_BOOT:](#PEIR_BARE_BOOT:) (const)
- [PEIR_BOOT_PREFIX:](#PEIR_BOOT_PREFIX:) (const)
- [pe_bare_boot_on](#pe_bare_boot_on) (function)
- [PE_EFI_STACK_PAGES:](#PE_EFI_STACK_PAGES:) (const)
- [PE_EFI_HEAP_PAGES:](#PE_EFI_HEAP_PAGES:) (const)
- [PE_EFI_ABORTED:](#PE_EFI_ABORTED:) (const)
- [PE_EFI_OUT_OF_RESOURCES:](#PE_EFI_OUT_OF_RESOURCES:) (const)
- [PeirGen_gen_uefi_entry](#PeirGen_gen_uefi_entry) (function)
- [PeirGen_gen_uefi_start](#PeirGen_gen_uefi_start) (function)
- [PeirGen_gen_uefi_io](#PeirGen_gen_uefi_io) (function)
- [pe_bare_boot_prefix](#pe_bare_boot_prefix) (function)
- [PeirGen_gen_bare_runtime](#PeirGen_gen_bare_runtime) (function)
- [PeirGen_trap_unplaced](#PeirGen_trap_unplaced) (function)
- [PeirGen_gen_runtime_stubs](#PeirGen_gen_runtime_stubs) (function)
- [PeirGen_gen_interrupt_stubs](#PeirGen_gen_interrupt_stubs) (function)
- [PeirGen_gen_mmu_stubs](#PeirGen_gen_mmu_stubs) (function)
- [PeirGen_gen_gdt_stubs](#PeirGen_gen_gdt_stubs) (function)
- [PeirGen_gen_enter_user_mode](#PeirGen_gen_enter_user_mode) (function)
- [PeirGen_gen_serial_stubs](#PeirGen_gen_serial_stubs) (function)
- [PeirGen_gen_print_int](#PeirGen_gen_print_int) (function)
- [PeirGen_gen_print_str](#PeirGen_gen_print_str) (function)
- [PeirGen_gen_print_bool](#PeirGen_gen_print_bool) (function)
- [PeirGen_gen_print_float](#PeirGen_gen_print_float) (function)
- [PeirGen_gen_panic](#PeirGen_gen_panic) (function)
- [PEIR_ALLOC_BYTES:](#PEIR_ALLOC_BYTES:) (const)
- [PEIR_ALLOC_CLS:](#PEIR_ALLOC_CLS:) (const)
- [PEIR_SMALL_MAX:](#PEIR_SMALL_MAX:) (const)
- [PEIR_ALLOC_STATE_SIZE:](#PEIR_ALLOC_STATE_SIZE:) (const)
- [PEIR_PANIC_DOUBLE_FREE:](#PEIR_PANIC_DOUBLE_FREE:) (const)
- [PEIR_PANIC_BAD_FREE:](#PEIR_PANIC_BAD_FREE:) (const)
- [peir_checked_uaf_fmt](#peir_checked_uaf_fmt) (function)
- [peir_checked_overrun_fmt](#peir_checked_overrun_fmt) (function)
- [PeirGen_gen_checked_fault](#PeirGen_gen_checked_fault) (function)
- [peir_class_bytes](#peir_class_bytes) (function)
- [peir_class_for](#peir_class_for) (function)
- [peir_alloc_state_byte](#peir_alloc_state_byte) (function)
- [PeirGen_gen_malloc_uninit](#PeirGen_gen_malloc_uninit) (function)
- [PeirGen_gen_malloc](#PeirGen_gen_malloc) (function)
- [PeirGen_gen_free](#PeirGen_gen_free) (function)
- [PeirGen_gen_memset](#PeirGen_gen_memset) (function)
- [PeirGen_gen_str_concat](#PeirGen_gen_str_concat) (function)
- [PeirGen_gen_to_str](#PeirGen_gen_to_str) (function)
- [PeirGen_gen_to_str_str](#PeirGen_gen_to_str_str) (function)
- [PeirGen_gen_to_str_bool](#PeirGen_gen_to_str_bool) (function)
- [PeirGen_gen_to_str_float](#PeirGen_gen_to_str_float) (function)
- [PeirGen_gen_to_str_vec](#PeirGen_gen_to_str_vec) (function)
- [PeirGen_gen_vec_new](#PeirGen_gen_vec_new) (function)
- [PeirGen_gen_vec_push](#PeirGen_gen_vec_push) (function)
- [PeirGen_gen_vec_len](#PeirGen_gen_vec_len) (function)
- [PeirGen_gen_vec_get](#PeirGen_gen_vec_get) (function)
- [PeirGen_gen_vec_pop](#PeirGen_gen_vec_pop) (function)
- [PeirGen_gen_print_vec](#PeirGen_gen_print_vec) (function)
- [PeirGen_gen_map_new](#PeirGen_gen_map_new) (function)
- [PeirGen_gen_map_insert](#PeirGen_gen_map_insert) (function)
- [PeirGen_gen_map_get](#PeirGen_gen_map_get) (function)
- [PeirGen_gen_map_contains](#PeirGen_gen_map_contains) (function)
- [PeirGen_gen_map_remove](#PeirGen_gen_map_remove) (function)
- [PeirGen_gen_map_len](#PeirGen_gen_map_len) (function)
- [PeirGen_gen_map_is_empty](#PeirGen_gen_map_is_empty) (function)
- [PeirGen_gen_print_map](#PeirGen_gen_print_map) (function)
- [PeirGen_gen_vec_set](#PeirGen_gen_vec_set) (function)
- [PeirGen_gen_vec_is_empty](#PeirGen_gen_vec_is_empty) (function)
- [PeirGen_gen_vec_clear](#PeirGen_gen_vec_clear) (function)
- [PeirGen_gen_vec_capacity](#PeirGen_gen_vec_capacity) (function)
- [PeirGen_gen_vec_first](#PeirGen_gen_vec_first) (function)
- [PeirGen_gen_vec_last](#PeirGen_gen_vec_last) (function)
- [PeirGen_gen_vec_contains](#PeirGen_gen_vec_contains) (function)
- [PeirGen_gen_vec_truncate](#PeirGen_gen_vec_truncate) (function)
- [PeirGen_gen_vec_clone](#PeirGen_gen_vec_clone) (function)
- [PeirGen_gen_map_clear](#PeirGen_gen_map_clear) (function)
- [PeirGen_gen_map_get_or](#PeirGen_gen_map_get_or) (function)
- [PeirGen_gen_string_len](#PeirGen_gen_string_len) (function)
- [PeirGen_gen_string_is_empty](#PeirGen_gen_string_is_empty) (function)
- [PeirGen_gen_string_starts_with](#PeirGen_gen_string_starts_with) (function)
- [PeirGen_gen_string_parse_int](#PeirGen_gen_string_parse_int) (function)
- [PeirGen_gen_string_bytes](#PeirGen_gen_string_bytes) (function)
- [PeirGen_gen_string_clone](#PeirGen_gen_string_clone) (function)
- [PeirGen_gen_float_to_str_alias](#PeirGen_gen_float_to_str_alias) (function)
- [PeirGen_gen_bool_to_str_alias](#PeirGen_gen_bool_to_str_alias) (function)
- [PeirGen_gen_char_to_str](#PeirGen_gen_char_to_str) (function)
- [PeirGen_gen_eprint_str](#PeirGen_gen_eprint_str) (function)
- [PeirGen_gen_print_newline](#PeirGen_gen_print_newline) (function)
- [PeirGen_gen_sb_new](#PeirGen_gen_sb_new) (function)
- [PeirGen_gen_sb_append](#PeirGen_gen_sb_append) (function)
- [PeirGen_gen_sb_to_str](#PeirGen_gen_sb_to_str) (function)
- [PeirGen_gen_file_read](#PeirGen_gen_file_read) (function)
- [PeirGen_gen_file_write](#PeirGen_gen_file_write) (function)
- [PeirGen_gen_argc](#PeirGen_gen_argc) (function)
- [PeirGen_gen_argv](#PeirGen_gen_argv) (function)
- [PeirGen_gen_system](#PeirGen_gen_system) (function)
- [PeirGen_gen_str_join_n](#PeirGen_gen_str_join_n) (function)
- [PeirGen_gen_entry_point](#PeirGen_gen_entry_point) (function)
- [PeirGen_gen_dll_loader](#PeirGen_gen_dll_loader) (function)
- [PeirGen_gen_c_adapter](#PeirGen_gen_c_adapter) (function)
- [PeirGen_gen_c_thunk](#PeirGen_gen_c_thunk) (function)
- [PeirGen_gen_pe_primitives](#PeirGen_gen_pe_primitives) (function)
- [PeirGen_nat_wanted](#PeirGen_nat_wanted) (function)
- [PeirGen_emit_copy_rcx_to_rax](#PeirGen_emit_copy_rcx_to_rax) (function)
- [PeirGen_gen_arena_stubs](#PeirGen_gen_arena_stubs) (function)
- [PeAsmOut](#PeAsmOut) (struct)
- [PeAsmResult](#PeAsmResult) (struct)
- [pe_asm_reg64](#pe_asm_reg64) (function)
- [pe_asm_encode](#pe_asm_encode) (function)
- [pe_asm_privileged](#pe_asm_privileged) (function)
- [peir_env_or_empty](#peir_env_or_empty) (function)
- [peir_on](#peir_on) (function)

## CThunk

An `extern "C"` function of msvcrt.dll or kernel32.dll: called through a
thunk that moves Fides arguments (all in integer registers, floats as
raw bits) into the Windows x64 C convention, and the result back.

```fides
pub struct CThunk {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `slot` | `i64` |
| `params` | `Vec<IrType>` |
| `ret` | `IrType` |
| `by_value` | `Vec<i64>` |
| `ret_size` | `i64` |

*Defined at line 43*

---

## CAdapter

What C calls for a Fides function: the C convention on one side, a null
`__env` and raw bits on the other.

```fides
pub struct CAdapter {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `target` | `str` |
| `params` | `Vec<IrType>` |
| `ret` | `IrType` |

*Defined at line 56*

---

## CAdapter_new

```fides
pub fn CAdapter_new(name: str, target: str, params: Vec<IrType>, ret: IrType) -> CAdapter {
```

**Parameters:**

- `name`: `str`
- `target`: `str`
- `params`: `Vec<IrType>`
- `ret`: `IrType`

**Returns:** `CAdapter`

*Defined at line 63*

---

## pe_c_by_value

The size of a struct Windows passes by value (in a register), else 0.

```fides
pub fn pe_c_by_value(module: &IrModule, ty: IrType) -> i64 {
```

**Parameters:**

- `module`: `&IrModule`
- `ty`: `IrType`

**Returns:** `i64`

*Defined at line 70*

---

## DynTypeId

Type id of dyn values (type name -> id, from 1).  Sorted Vec stand-in for
Rust `BTreeMap<String, i64>`.

```fides
pub struct DynTypeId {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `id` | `i64` |

*Defined at line 86*

---

## UnresolvedCall

A called function that neither the program nor this runtime defines,
with the (sorted) names of the functions that call it.  Sorted Vec
stand-in for Rust `BTreeMap<String, BTreeSet<String>>`.

```fides
pub struct UnresolvedCall {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `callers` | `Vec<str>` |

*Defined at line 94*

---

## COverride

An `extern "C"` function the Fides runtime replaces (see c_override):
pledge name -> `pert_c_<C name>`.  Sorted Vec for BTreeMap<String,String>.

```fides
pub struct COverride {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `over` | `str` |

*Defined at line 101*

---

## CThunk_new

```fides
pub fn CThunk_new(module: &IrModule, name: str, slot: i64, params: Vec<IrType>, ret: IrType) -> CThunk {
```

**Parameters:**

- `module`: `&IrModule`
- `name`: `str`
- `slot`: `i64`
- `params`: `Vec<IrType>`
- `ret`: `IrType`

**Returns:** `CThunk`

*Defined at line 109*

---

## COverride_new

```fides
pub fn COverride_new(name: str, over: str) -> COverride {
```

**Parameters:**

- `name`: `str`
- `over`: `str`

**Returns:** `COverride`

*Defined at line 138*

---

## PeirGen

```fides
pub struct PeirGen {
```

**Fields:**

| Name | Type |
|------|------|
| `enc` | `PeirEnc` |
| `func_labels` | `i64` |
| `string_rdata_offsets` | `i64` |
| `global_rdata_offsets` | `i64` |
| `iat_rdata_offsets` | `Vec<i64>` |
| `block_labels` | `i64` |
| `entry_offset` | `i64` |
| `bare_mode` | `bool` |
| `cur_func` | `str` |
| `cur_wrapping` | `bool` |
| `unchecked` | `Vec<i64>` |
| `cur_unchecked` | `bool` |
| `arith_panics` | `Vec<i64>` |
| `crash_filter` | `i64` |
| `saved_regs` | `Vec<PeOffEnt>` |
| `dyn_type_ids` | `Vec<DynTypeId>` |
| `unresolved` | `Vec<UnresolvedCall>` |
| `module_fns` | `Vec<str>` |
| `c_thunks` | `Vec<CThunk>` |
| `c_adapters` | `Vec<CAdapter>` |
| `stub_externs` | `Vec<str>` |
| `alloc_rdata_off` | `i64` |
| `c_overrides` | `Vec<COverride>` |
| `named_fns` | `Vec<str>` |
| `pending_cmp_cc` | `i64` |
| `pending_cmp_vid` | `i64` |
| `flags_only` | `bool` |
| `use_count` | `Vec<i64>` |
| `next_block` | `i64` |
| `calls_reserved` | `bool` |
| `tail_next` | `bool` |
| `tail_done` | `bool` |
| `tail_ok` | `bool` |
| `wipe_frame` | `bool` |
| `has_asm` | `bool` |
| `cold_fns` | `i64` |
| `line_fns` | `i64` |
| `lines_cur_fn` | `bool` |
| `cur_line` | `i64` |
| `lrows` | `Vec<i64>` |
| `dbg_names` | `Vec<str>` |
| `dbg_starts` | `Vec<i64>` |
| `dbg_ends` | `Vec<i64>` |
| `fc_on` | `bool` |
| `fc_path` | `str` |
| `fc_salt` | `i64` |
| `fc_key` | `i64` |
| `fc_old` | `Vec<PeFcEntry>` |
| `fc_ix` | `i64` |
| `fc_new` | `Vec<PeFcEntry>` |
| `fc_rev` | `Vec<str>` |
| `fc_rip` | `i64` |
| `fc_rip_kind` | `Vec<i64>` |
| `fc_rip_k` | `Vec<i64>` |
| `fc_rip_s` | `Vec<str>` |
| `fc_hits` | `i64` |
| `fc_misses` | `i64` |
| `unres_adds` | `i64` |
| `nat_own` | `i64` |
| `nat_wanted` | `i64` |
| `nat_names_tab` | `i64` |
| `nat_fntab_tab` | `i64` |
| `nat_abs_fix` | `Vec<i64>` |
| `nat_missing` | `str` |

*Defined at line 148*

---

## PeFcFix

B5: a relocation of a cached function. pos: offset of the 4 bytes in
the function's code; kind: PEIR_RELOC_* (a label) or -1 (an .rdata
reference); tkind: what it points at (PEFC_* below); toff: an offset in
the function (PEFC_LOCAL), an index (iat, alloc, arith panic); tstr: a
name (function, string content, global).

```fides
pub struct PeFcFix {
```

**Fields:**

| Name | Type |
|------|------|
| `pos` | `i64` |
| `kind` | `i64` |
| `tkind` | `i64` |
| `toff` | `i64` |
| `tstr` | `str` |

*Defined at line 277*

---

## PeFcEntry

B5: one function's code before its labels and .rdata references are
resolved, keyed by its IR and the build's salt.

```fides
pub struct PeFcEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `key` | `i64` |
| `bytes` | `Vec<i64>` |
| `fixes` | `Vec<PeFcFix>` |

*Defined at line 287*

---

## PEFC_LOCAL:

```fides
pub const PEFC_LOCAL: i64 = 0;
```

*Defined at line 294*

---

## PEFC_NAME:

```fides
pub const PEFC_NAME: i64 = 1;
```

*Defined at line 295*

---

## PEFC_STR:

```fides
pub const PEFC_STR: i64 = 2;
```

*Defined at line 296*

---

## PEFC_GLOBAL:

```fides
pub const PEFC_GLOBAL: i64 = 3;
```

*Defined at line 297*

---

## PEFC_IAT:

```fides
pub const PEFC_IAT: i64 = 4;
```

*Defined at line 298*

---

## PEFC_ALLOC:

```fides
pub const PEFC_ALLOC: i64 = 5;
```

*Defined at line 299*

---

## PEFC_PANIC:

```fides
pub const PEFC_PANIC: i64 = 6;
```

*Defined at line 300*

---

## PEFC_CRASH:

```fides
pub const PEFC_CRASH: i64 = 7;
```

*Defined at line 301*

---

## PeirGen_new

```fides
pub fn PeirGen_new() -> PeirGen {
```

**Returns:** `PeirGen`

*Defined at line 303*

---

## pe_primitive_names

Machine-code primitives of the Fides PE runtime (src/pe_runtime/), which
declares them `extern "C"`: called without `__env`, arguments from rcx.

```fides
pub fn pe_primitive_names() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 410*

---

## prim_interface_names

MI20: the interface of the runtime's machine-code primitives — the
`nat_*` names every native backend defines: the machine layer's targets
(Linux AArch64 / RISC-V / x86-64, `--nat` Windows, macOS; nat_ir.fd
checks them with ng_prim_check under FID_VERIFY=1) and pe_ir, the
default Windows backend (PeirGen_gen_nat_prims, checked by
PeirGen_nat_check under FID_VERIFY=1). Grouped: system calls and
threads; signals; atomics; tasks; traces; the rest. One list for both
(nat_ir imports this module). Target-only helpers (nat_win_call,
nat_mac_call, nat_c_*, pe_*) are not part of it.

```fides
pub fn prim_interface_names() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 447*

---

## pe_verify_on

```fides
pub fn pe_verify_on() -> bool {
```

**Returns:** `bool`

*Defined at line 477*

---

## PeirGen_resolve_call

The function a call to `name` runs: a `pe_runtime_alias`, the Fides
runtime's `pert_X` for builtin `p_X` (it replaces a machine-code stub
of the same builtin), else `name` (a stub, a thunk, a program function).

```fides
pub fn PeirGen_resolve_call(self: &PeirGen, name: str) -> str {
```

**Parameters:**

- `self`: `&PeirGen`
- `name`: `str`

**Returns:** `str`

*Defined at line 884*

---

## PeirGen_call_builtin

Call runtime builtin `name` with `nargs` (at most 3) arguments already
in rcx, rdx, r8.  A Fides stand-in takes `__env` first: the arguments
move up one register and rcx gets a null environment.

```fides
pub fn PeirGen_call_builtin(self: &mut PeirGen, name: str, nargs: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`
- `name`: `str`
- `nargs`: `i64`

**Returns:** `void`

*Defined at line 914*

---

## PeirGen_dyn_type_id

The id of dyn values of type `name` (stable within one binary).

```fides
pub fn PeirGen_dyn_type_id(self: &mut PeirGen, name: str) -> i64 {
```

**Parameters:**

- `self`: `&mut PeirGen`
- `name`: `str`

**Returns:** `i64`

*Defined at line 933*

---

## PeirGen_call_runtime

Call runtime or program function `name`; one nobody defines traps,
and makes generate_pe (or generate_flat) refuse the program.

```fides
pub fn PeirGen_call_runtime(self: &mut PeirGen, name: str) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`
- `name`: `str`

**Returns:** `void`

*Defined at line 1003*

---

## PeirGen_place_fn

Place the label of function `name` (16-byte aligned).

```fides
pub fn PeirGen_place_fn(self: &mut PeirGen, name: str) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`
- `name`: `str`

**Returns:** `void`

*Defined at line 1015*

---

## PeirGen_str_off

rdata offset of string constant `s` (0 when absent, as unwrap_or(&0)).

```fides
pub fn PeirGen_str_off(self: &PeirGen, s: str) -> i64 {
```

**Parameters:**

- `self`: `&PeirGen`
- `s`: `str`

**Returns:** `i64`

*Defined at line 1022*

---

## PeirGen_iat_off

Get IAT rdata offset for an import

```fides
pub fn PeirGen_iat_off(self: &PeirGen, idx: i64) -> i64 {
```

**Parameters:**

- `self`: `&PeirGen`
- `idx`: `i64`

**Returns:** `i64`

*Defined at line 1027*

---

## PEIR_BARE_BOOT:

Off: `--bare` writes the old flat binary at 0x10000 (no headers, no long
mode switch, no IDT, no debug exit), `--boot` the old pe.fd kernel.

```fides
pub const PEIR_BARE_BOOT: bool = true;
```

*Defined at line 1093*

---

## PEIR_BOOT_PREFIX:

Bytes in front of the flat binary of a bootable image

```fides
pub const PEIR_BOOT_PREFIX: i64 = 0x1000;
```

*Defined at line 1095*

---

## pe_bare_boot_on

The boot image is being built (a bare build, not --elf)

```fides
pub fn pe_bare_boot_on() -> bool {
```

**Returns:** `bool`

*Defined at line 1109*

---

## PE_EFI_STACK_PAGES:

```fides
pub const PE_EFI_STACK_PAGES: i64 = 256;
```

*Defined at line 1147*

---

## PE_EFI_HEAP_PAGES:

```fides
pub const PE_EFI_HEAP_PAGES: i64 = 0x8000;
```

*Defined at line 1148*

---

## PE_EFI_ABORTED:

```fides
pub const PE_EFI_ABORTED: i64 = 0x8000000000000015;
```

*Defined at line 1149*

---

## PE_EFI_OUT_OF_RESOURCES:

```fides
pub const PE_EFI_OUT_OF_RESOURCES: i64 = 0x8000000000000009;
```

*Defined at line 1150*

---

## PeirGen_gen_uefi_entry

The UEFI entry's first instructions (rcx = ImageHandle, rdx =
SystemTable): save what the caller expects back, remember the stack and
both arguments. 8 pushes and 168 bytes keep rsp 16-byte aligned.

```fides
pub fn PeirGen_gen_uefi_entry(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 1190*

---

## PeirGen_gen_uefi_start

After the bare entry's alignment: watchdog off, the heap and the stack
from AllocatePages, then rsp on the new stack (with shadow space).

```fides
pub fn PeirGen_gen_uefi_start(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 1205*

---

## PeirGen_gen_uefi_io

UEFI output: putc(cl), p_print_str / p_eprint_str(rcx = C string)
through ConOut->OutputString, and the exit routine. The string is
converted 125 UCS-2 characters at a time into a buffer on the stack.

```fides
pub fn PeirGen_gen_uefi_io(self: &mut PeirGen, putc: i64, pstr: i64, estr: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`
- `putc`: `i64`
- `pstr`: `i64`
- `estr`: `i64`

**Returns:** `void`

*Defined at line 1331*

---

## pe_bare_boot_prefix

The PEIR_BOOT_PREFIX bytes in front of the flat binary (see above).
`boot64` is the absolute address of boot_entry64 in the flat code.

```fides
pub fn pe_bare_boot_prefix(load: i64, boot64: i64) -> Vec {
```

**Parameters:**

- `load`: `i64`
- `boot64`: `i64`

**Returns:** `Vec`

*Defined at line 1539*

---

## PeirGen_gen_bare_runtime

```fides
pub fn PeirGen_gen_bare_runtime(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 2061*

---

## PeirGen_trap_unplaced

Place each of `names` not placed yet at a trap: the runtime functions a
bare build has no body for.

```fides
pub fn PeirGen_trap_unplaced(self: &mut PeirGen, names: Vec<str>) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`
- `names`: `Vec<str>`

**Returns:** `void`

*Defined at line 2326*

---

## PeirGen_gen_runtime_stubs

```fides
pub fn PeirGen_gen_runtime_stubs(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 2344*

---

## PeirGen_gen_interrupt_stubs

Generate interrupt control stubs (Phase 5A.3)

```fides
pub fn PeirGen_gen_interrupt_stubs(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 2450*

---

## PeirGen_gen_mmu_stubs

Generate MMU/page table management stubs (Phase 5A.4)

```fides
pub fn PeirGen_gen_mmu_stubs(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 2656*

---

## PeirGen_gen_gdt_stubs

Generate GDT stubs (Phase 5A.5)

```fides
pub fn PeirGen_gen_gdt_stubs(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 3249*

---

## PeirGen_gen_enter_user_mode

p_enter_user_mode(rcx = entry_addr, rdx = stack_addr) -> void
Transitions from ring 0 (kernel) to ring 3 (user) via iretq.

```fides
pub fn PeirGen_gen_enter_user_mode(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 3327*

---

## PeirGen_gen_serial_stubs

Generate serial port stubs (Phase 5A.6).  Only called in bare mode in
Rust; kept for parity (bare bodies not ported — they panic).

```fides
pub fn PeirGen_gen_serial_stubs(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 3371*

---

## PeirGen_gen_print_int

p_print_int(rcx = i64 value)

```fides
pub fn PeirGen_gen_print_int(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 3500*

---

## PeirGen_gen_print_str

p_print_str(rcx = *u8 pointer to null-terminated string)

```fides
pub fn PeirGen_gen_print_str(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 3602*

---

## PeirGen_gen_print_bool

p_print_bool(rcx = 0 or 1)

```fides
pub fn PeirGen_gen_print_bool(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 3651*

---

## PeirGen_gen_print_float

p_print_float(rcx = double bits as i64)
Converts double to string with up to 6 decimal places, strips trailing zeros.

```fides
pub fn PeirGen_gen_print_float(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 3722*

---

## PeirGen_gen_panic

p_panic(rcx = message pointer)

```fides
pub fn PeirGen_gen_panic(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 4066*

---

## PEIR_ALLOC_BYTES:

```fides
pub const PEIR_ALLOC_BYTES: i64 = 448;
```

*Defined at line 4325*

---

## PEIR_ALLOC_CLS:

```fides
pub const PEIR_ALLOC_CLS: i64 = 776;
```

*Defined at line 4326*

---

## PEIR_SMALL_MAX:

```fides
pub const PEIR_SMALL_MAX: i64 = 1016;
```

*Defined at line 4327*

---

## PEIR_ALLOC_STATE_SIZE:

```fides
pub const PEIR_ALLOC_STATE_SIZE: i64 = 960;
```

*Defined at line 4329*

---

## PEIR_PANIC_DOUBLE_FREE:

```fides
pub const PEIR_PANIC_DOUBLE_FREE: i64 = 7;
```

*Defined at line 4341*

---

## PEIR_PANIC_BAD_FREE:

```fides
pub const PEIR_PANIC_BAD_FREE: i64 = 8;
```

*Defined at line 4342*

---

## peir_checked_uaf_fmt

The --checked-alloc fault reports (format strings for msvcrt _snprintf).

```fides
pub fn peir_checked_uaf_fmt() -> str {
```

**Returns:** `str`

*Defined at line 4454*

---

## peir_checked_overrun_fmt

```fides
pub fn peir_checked_overrun_fmt() -> str {
```

**Returns:** `str`

*Defined at line 4457*

---

## PeirGen_gen_checked_fault

Part of the crash filter under --checked-alloc (release M7, complaint
CM9), entered with rax = the exception code and rcx = the
EXCEPTION_POINTERS. An access violation on a page of a checked block —
a freed block's pages, or the guard page after a live one — panics with
what the block was: its size, address, the offset accessed and the
image offset of the call that allocated it (from the block's record,
PeirGen_gen_checked_malloc). Anything else falls through with rax
unchanged, to the generic message.

```fides
pub fn PeirGen_gen_checked_fault(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 4469*

---

## peir_class_bytes

RT8: the block bytes of class c (as p_malloc makes them): 1 << (c / 2)
for an even c, three quarters of the next power of two for an odd one;
0 below the first class (stdlib/pe_runtime/conc.fd pe_al_bytes).

```fides
pub fn peir_class_bytes(c: i64) -> i64 {
```

**Parameters:**

- `c`: `i64`

**Returns:** `i64`

*Defined at line 4836*

---

## peir_class_for

RT8: the class of a block of n bytes with its header (n >= 8): the
power of two 2^b >= n (b at least 4) is class 2b, three quarters of it
class 2b - 1 when that holds n (not for b = 4), as the machine code of
PeirGen_gen_malloc_body computes for the sizes past the table.

```fides
pub fn peir_class_for(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 4850*

---

## peir_alloc_state_byte

RT8: the initial bytes of the allocator state in .rdata (pe_ir.fd
writes them): zero, but the class tables: at PEIR_ALLOC_BYTES each
class's block bytes (a word per class 0..40), at PEIR_ALLOC_CLS the
class of a request of s bytes at index (s + 15) >> 3, s up to
PEIR_SMALL_MAX (with the header: n = 8 * index).

```fides
pub fn peir_alloc_state_byte(i: i64) -> i64 {
```

**Parameters:**

- `i`: `i64`

**Returns:** `i64`

*Defined at line 4866*

---

## PeirGen_gen_malloc_uninit

RT8: __p_malloc_uninit(rcx = size) -> rax, what core's p_malloc_uninit
runs here (PeirGen_resolve_call): a block exactly as p_malloc's
(same kind, freed by p_free, the same FID_MEM_LIMIT count) whose bytes
are left as they are. Only core and the runtimes call it, for buffers
they write in full. Under --leaks, --checked-alloc and in a bare build
it is p_malloc (whose blocks read zero: the same, and every block
tracked or guarded).

```fides
pub fn PeirGen_gen_malloc_uninit(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 5330*

---

## PeirGen_gen_malloc

p_malloc(rcx = size) -> rax = ptr. Power-of-two blocks up to 1 MB (and
three quarters of one), as in the C runtime: an 8-byte header, reused
through the thread's free lists, refilled by a bump in the thread's own
4 MB chunk (RT7: no lock, no atomic; a chunk's blocks are in its
4 MB-aligned range, so stdlib/pe_runtime/conc.fd pert_alloc_trim can
find and release a chunk whose blocks are all free). Larger blocks are
HeapAlloc's. A full chunk: pert_alloc_chunk maps the next one. A reused
block whose inside pages a trim decommitted: pert_alloc_recommit. RT8:
the fast path is a leaf (PeirGen_gen_malloc_body), a reused block is
zeroed in the bytes asked for, and a thread takes back the blocks
other threads freed of its chunks before it bumps.

```fides
pub fn PeirGen_gen_malloc(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 5353*

---

## PeirGen_gen_free

p_free(rcx = ptr): back on its class's free list, or to the system heap
when large. Panics unless it is a live block. RT7: past the thread's
threshold of free bytes, pert_alloc_trim gives memory back to the system.
RT8: a block of another thread's chunk goes back to that thread (its
owner slot); the path to this thread's lists is a leaf with no frame.

```fides
pub fn PeirGen_gen_free(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 5414*

---

## PeirGen_gen_memset

p_memset(rcx=ptr, rdx=val, r8=size) — fill memory with val for size bytes

```fides
pub fn PeirGen_gen_memset(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 5631*

---

## PeirGen_gen_str_concat

```fides
pub fn PeirGen_gen_str_concat(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 5714*

---

## PeirGen_gen_to_str

p_to_str(rcx = i64 value) -> rax = *u8
Converts integer to null-terminated string, malloc'd

```fides
pub fn PeirGen_gen_to_str(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 5798*

---

## PeirGen_gen_to_str_str

p_to_str_str(rcx = *u8 str) -> rax = *u8
For strings, to_str just returns the input pointer (it's already a string)

```fides
pub fn PeirGen_gen_to_str_str(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 5984*

---

## PeirGen_gen_to_str_bool

p_to_str_bool(rcx = i64 bool) -> rax = *u8

```fides
pub fn PeirGen_gen_to_str_bool(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 5993*

---

## PeirGen_gen_to_str_float

p_to_str_float(rcx = f64 bits in i64 reg) -> rax = *u8
Simplified: convert to integer string (truncate decimal for now)

```fides
pub fn PeirGen_gen_to_str_float(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 6074*

---

## PeirGen_gen_to_str_vec

```fides
pub fn PeirGen_gen_to_str_vec(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 6435*

---

## PeirGen_gen_vec_new

p_vec_new() -> rax = *Vec (32-byte heap struct, all zeros)

```fides
pub fn PeirGen_gen_vec_new(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 6551*

---

## PeirGen_gen_vec_push

p_vec_push(rcx=*Vec, rdx=i64 value)

```fides
pub fn PeirGen_gen_vec_push(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 6576*

---

## PeirGen_gen_vec_len

p_vec_len(rcx=*Vec) -> rax = i64

```fides
pub fn PeirGen_gen_vec_len(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 6695*

---

## PeirGen_gen_vec_get

p_vec_get(rcx=*Vec, rdx=i64 index) -> rax = i64

The lowerer emits this builtin for `v[i]` (compiler/lower.fd), and the
interpreter's version panics on out-of-bounds — so unlike the Rust stub
(dead code there; its lowerer emits checked `Vec_get` calls), this one
bounds-checks: idx >= len or idx < 0 -> p_panic (exits 1).

```fides
pub fn PeirGen_gen_vec_get(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 6712*

---

## PeirGen_gen_vec_pop

p_vec_pop(rcx=*Vec) -> rax = i64 (last element, or 0 if empty)

```fides
pub fn PeirGen_gen_vec_pop(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 6746*

---

## PeirGen_gen_print_vec

p_print_vec(rcx=*Vec) — prints [1, 2, 3]

```fides
pub fn PeirGen_gen_print_vec(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 6792*

---

## PeirGen_gen_map_new

p_map_new() -> rax = *Map (24-byte heap struct, all zeros)

```fides
pub fn PeirGen_gen_map_new(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 6897*

---

## PeirGen_gen_map_insert

p_map_insert(rcx=*Map, rdx=*u8 key, r8=i64 value)

```fides
pub fn PeirGen_gen_map_insert(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 6919*

---

## PeirGen_gen_map_get

p_map_get(rcx=*Map, rdx=*u8 key) -> rax = i64

```fides
pub fn PeirGen_gen_map_get(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 7131*

---

## PeirGen_gen_map_contains

p_map_contains(rcx=*Map, rdx=*u8 key) -> rax = bool

```fides
pub fn PeirGen_gen_map_contains(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 7214*

---

## PeirGen_gen_map_remove

p_map_remove(rcx=*Map, rdx=*u8 key)

```fides
pub fn PeirGen_gen_map_remove(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 7283*

---

## PeirGen_gen_map_len

p_map_len(rcx=*Map) -> rax = i64

```fides
pub fn PeirGen_gen_map_len(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 7392*

---

## PeirGen_gen_map_is_empty

p_map_is_empty(rcx=*Map) -> rax = bool

```fides
pub fn PeirGen_gen_map_is_empty(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 7403*

---

## PeirGen_gen_print_map

p_print_map(rcx=*Map) — prints {key: value, ...}

```fides
pub fn PeirGen_gen_print_map(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 7426*

---

## PeirGen_gen_vec_set

p_vec_set(rcx=*Vec, rdx=i64 idx, r8=i64 value)
Bounds-checked like the interpreter's builtin: an out-of-range write
would silently corrupt the heap, so idx >= len or idx < 0 -> p_panic.

```fides
pub fn PeirGen_gen_vec_set(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 7578*

---

## PeirGen_gen_vec_is_empty

p_vec_is_empty(rcx=*Vec) -> rax = bool

```fides
pub fn PeirGen_gen_vec_is_empty(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 7620*

---

## PeirGen_gen_vec_clear

p_vec_clear(rcx=*Vec)

```fides
pub fn PeirGen_gen_vec_clear(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 7632*

---

## PeirGen_gen_vec_capacity

p_vec_capacity(rcx=*Vec) -> rax = i64

```fides
pub fn PeirGen_gen_vec_capacity(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 7642*

---

## PeirGen_gen_vec_first

p_vec_first(rcx=*Vec) -> rax = i64

```fides
pub fn PeirGen_gen_vec_first(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 7651*

---

## PeirGen_gen_vec_last

p_vec_last(rcx=*Vec) -> rax = i64

```fides
pub fn PeirGen_gen_vec_last(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 7670*

---

## PeirGen_gen_vec_contains

p_vec_contains(rcx=*Vec, rdx=i64 value) -> rax = bool

```fides
pub fn PeirGen_gen_vec_contains(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 7695*

---

## PeirGen_gen_vec_truncate

p_vec_truncate(rcx=*Vec, rdx=i64 new_len)

```fides
pub fn PeirGen_gen_vec_truncate(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 7745*

---

## PeirGen_gen_vec_clone

p_vec_clone(rcx=*Vec) -> rax = *Vec (new copy)

```fides
pub fn PeirGen_gen_vec_clone(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 7767*

---

## PeirGen_gen_map_clear

p_map_clear(rcx=*Map) — set count=0

```fides
pub fn PeirGen_gen_map_clear(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 7804*

---

## PeirGen_gen_map_get_or

p_map_get_or(rcx=*Map, rdx=*u8 key, r8=i64 default) -> rax = i64

```fides
pub fn PeirGen_gen_map_get_or(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 7814*

---

## PeirGen_gen_string_len

p_string_len(rcx=*u8) -> rax = i64 (strlen)

```fides
pub fn PeirGen_gen_string_len(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 7846*

---

## PeirGen_gen_string_is_empty

p_string_is_empty(rcx=*u8) -> rax = bool

```fides
pub fn PeirGen_gen_string_is_empty(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 7861*

---

## PeirGen_gen_string_starts_with

p_string_starts_with(rcx=*u8, rdx=*u8 prefix) -> rax = bool

```fides
pub fn PeirGen_gen_string_starts_with(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 7875*

---

## PeirGen_gen_string_parse_int

p_string_parse_int(rcx=*u8) -> rax = i64 (atoi)

```fides
pub fn PeirGen_gen_string_parse_int(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 7921*

---

## PeirGen_gen_string_bytes

p_string_bytes(rcx=*u8) -> rax = i64 (same as len)

```fides
pub fn PeirGen_gen_string_bytes(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 7960*

---

## PeirGen_gen_string_clone

p_string_clone(rcx=*u8) -> rax = *u8 (new copy)

```fides
pub fn PeirGen_gen_string_clone(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 7974*

---

## PeirGen_gen_float_to_str_alias

```fides
pub fn PeirGen_gen_float_to_str_alias(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 8012*

---

## PeirGen_gen_bool_to_str_alias

```fides
pub fn PeirGen_gen_bool_to_str_alias(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 8019*

---

## PeirGen_gen_char_to_str

p_char_to_str(rcx = char_code) -> rax = *u8

```fides
pub fn PeirGen_gen_char_to_str(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 8027*

---

## PeirGen_gen_eprint_str

p_eprint_str(rcx = *u8) — WriteFile to stderr

```fides
pub fn PeirGen_gen_eprint_str(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 8062*

---

## PeirGen_gen_print_newline

p_print_newline() — WriteFile "\n" to stdout

```fides
pub fn PeirGen_gen_print_newline(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 8112*

---

## PeirGen_gen_sb_new

p_sb_new() -> rax = *sb (struct: {buf, len, cap} = 24 bytes)

```fides
pub fn PeirGen_gen_sb_new(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 8150*

---

## PeirGen_gen_sb_append

p_sb_append(rcx = sb, rdx = s)
strlen once, memcpy for both the grow-copy and the append — no byte
loops (pe-roadmap items 73/75).

```fides
pub fn PeirGen_gen_sb_append(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 8196*

---

## PeirGen_gen_sb_to_str

p_sb_to_str(rcx = sb) -> rax = *u8

```fides
pub fn PeirGen_gen_sb_to_str(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 8297*

---

## PeirGen_gen_file_read

p_file_read(rcx = path) -> rax = *u8

```fides
pub fn PeirGen_gen_file_read(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 8349*

---

## PeirGen_gen_file_write

p_file_write(rcx = path, rdx = content) -> rax = i64 (0=success, -1=fail)

```fides
pub fn PeirGen_gen_file_write(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 8481*

---

## PeirGen_gen_argc

p_argc() -> rax = i64
Returns the number of command-line arguments.  On Windows,
GetCommandLineA returns a single string; we count tokens.

```fides
pub fn PeirGen_gen_argc(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 8627*

---

## PeirGen_gen_argv

p_argv(rcx = index) -> rax = *u8
Returns the command-line argument at the given index, or NULL.

```fides
pub fn PeirGen_gen_argv(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 8687*

---

## PeirGen_gen_system

p_system(rcx = cmd) -> rax = i64 (exit code)
msvcrt system(), as the C backend: runs through the shell and returns the
exit status.

```fides
pub fn PeirGen_gen_system(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 8836*

---

## PeirGen_gen_str_join_n

p_str_join_n(rcx = *Vec) -> rax = *u8
Vec struct: {data: *ptr, len: i64, cap: i64}; each element is a *u8 string.

```fides
pub fn PeirGen_gen_str_join_n(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 8855*

---

## PeirGen_gen_entry_point

_main — Windows entry point

```fides
pub fn PeirGen_gen_entry_point(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 8968*

---

## PeirGen_gen_dll_loader

Start-up loader of the `link` DLLs (release E2, E4), `__pe_load_dlls`:
LoadLibraryA each DLL (its candidates in order: sqlite3.dll, then the
system's winsqlite3.dll), GetProcAddress each function the program
reaches into its IAT slot. A DLL or function that is not there gets a
stub instead: the first call into it stops the program with a message
naming what is missing (a static import would make Windows refuse to
start the program at all, with no message on a console), and a program
that never calls it runs.

```fides
pub fn PeirGen_gen_dll_loader(self: &mut PeirGen, rdata: &mut PeirRdata) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`
- `rdata`: `&mut PeirRdata`

**Returns:** `void`

*Defined at line 9133*

---

## PeirGen_gen_c_adapter

```fides
pub fn PeirGen_gen_c_adapter(self: &mut PeirGen, a: &CAdapter) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`
- `a`: `&CAdapter`

**Returns:** `void`

*Defined at line 9276*

---

## PeirGen_gen_c_thunk

```fides
pub fn PeirGen_gen_c_thunk(self: &mut PeirGen, t: &CThunk) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`
- `t`: `&CThunk`

**Returns:** `void`

*Defined at line 9354*

---

## PeirGen_gen_pe_primitives

The machine-code primitives of the Fides PE runtime (PE_PRIMITIVES),
and the stubs that hand over to it (PE_STUB_DEPS).

```fides
pub fn PeirGen_gen_pe_primitives(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 9494*

---

## PeirGen_nat_wanted

MI20: nat primitive `name` is made: an extern declaration the program
reaches names it (pe_ir.fd fills nat_wanted), or FID_VERIFY=1.

```fides
pub fn PeirGen_nat_wanted(self: &PeirGen, name: str) -> bool {
```

**Parameters:**

- `self`: `&PeirGen`
- `name`: `str`

**Returns:** `bool`

*Defined at line 9758*

---

## PeirGen_emit_copy_rcx_to_rax

Copy `size` bytes from [rcx] to [rax] (clobbers rax, rcx, rdx):
8 bytes at a time, then single bytes, never past `size`.

```fides
pub fn PeirGen_emit_copy_rcx_to_rax(self: &mut PeirGen, size: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`
- `size`: `i64`

**Returns:** `void`

*Defined at line 10041*

---

## PeirGen_gen_arena_stubs

p_arena_save -> 0, p_arena_restore / p_region_free: no-ops.  Heap
memory here is never reused, so there is nothing to give back (as in
the interpreter).

```fides
pub fn PeirGen_gen_arena_stubs(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 10094*

---

## PeAsmOut

```fides
pub struct PeAsmOut {
```

**Fields:**

| Name | Type |
|------|------|
| `b` | `Vec<i64>` |

*Defined at line 10125*

---

## PeAsmResult

```fides
pub struct PeAsmResult {
```

**Fields:**

| Name | Type |
|------|------|
| `bytes` | `Vec<i64>` |
| `error` | `str` |

*Defined at line 10129*

---

## pe_asm_reg64

64-bit register number of `s` (rax=0 .. r15=15), else -1.

```fides
pub fn pe_asm_reg64(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 10135*

---

## pe_asm_encode

Encode an asm! template (instructions separated by `;` or newlines).

```fides
pub fn pe_asm_encode(template: str) -> PeAsmResult {
```

**Parameters:**

- `template`: `str`

**Returns:** `PeAsmResult`

*Defined at line 10750*

---

## pe_asm_privileged

LG13: does `template` hold an instruction a Windows process may not
run (ring 0 only: it faults with a privileged-instruction exception)?
hlt, cli, sti, port I/O, descriptor tables, MSRs, swapgs, iretq,
sysret/sysexit, cache invalidation and control or debug registers; and
the software interrupts (int, int3), which stop a process.

```fides
pub fn pe_asm_privileged(template: str) -> bool {
```

**Parameters:**

- `template`: `str`

**Returns:** `bool`

*Defined at line 10775*

---

## peir_env_or_empty

Environment variable `name`, "" when unset. Through core's env_var:
p_getenv's raw word (proposal 0004) must not be kept as a str.

```fides
pub fn peir_env_or_empty(name: str) -> str {
```

**Parameters:**

- `name`: `str`

**Returns:** `str`

*Defined at line 10859*

---

## peir_on

```fides
pub fn peir_on(pass: str) -> bool {
```

**Parameters:**

- `pass`: `str`

**Returns:** `bool`

*Defined at line 10878*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
