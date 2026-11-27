[All modules](index.md)

# compiler::codegen_c -- Fides Documentation

## Contents

- [ValueNameEntry](#ValueNameEntry) (struct)
- [ValueNameEntry_new](#ValueNameEntry_new) (function)
- [StringConstEntry](#StringConstEntry) (struct)
- [StringConstEntry_new](#StringConstEntry_new) (function)
- [FnParamTypesEntry](#FnParamTypesEntry) (struct)
- [FnParamTypesEntry_new](#FnParamTypesEntry_new) (function)
- [FnRetTypeEntry](#FnRetTypeEntry) (struct)
- [FnRetTypeEntry_new](#FnRetTypeEntry_new) (function)
- [CCodeGen](#CCodeGen) (struct)
- [CCodeGen_new](#CCodeGen_new) (function)
- [set_c_debug_info](#set_c_debug_info) (function)
- [clear_c_debug_info](#clear_c_debug_info) (function)
- [c_debug_cflags](#c_debug_cflags) (function)
- [c_debug_clflags](#c_debug_clflags) (function)
- [CGen_c_file_for](#CGen_c_file_for) (function)
- [set_c_register_all](#set_c_register_all) (function)
- [CGen_lib_set](#CGen_lib_set) (function)
- [CGen_lib_reset_exports](#CGen_lib_reset_exports) (function)
- [CGen_lib_export](#CGen_lib_export) (function)
- [CGen_lib_export_names](#CGen_lib_export_names) (function)
- [CGen_lib_header](#CGen_lib_header) (function)
- [CGen_lib_skipped](#CGen_lib_skipped) (function)
- [CGen_lib_cc_flags](#CGen_lib_cc_flags) (function)
- [CGen_lib_file_name](#CGen_lib_file_name) (function)
- [c_type](#c_type) (function)
- [c_type_global](#c_type_global) (function)
- [cc_box_ir_type_unbox](#cc_box_ir_type_unbox) (function)
- [OptVecIrType](#OptVecIrType) (enum)
- [OptIrType](#OptIrType) (enum)
- [cc_box_type_unbox](#cc_box_type_unbox) (function)
- [set_c_unit_marks](#set_c_unit_marks) (function)
- [set_c_profile](#set_c_profile) (function)
- [cg_prof_attach](#cg_prof_attach) (function)
- [cg_prof_attach_c](#cg_prof_attach_c) (function)
- [set_c_leaks](#set_c_leaks) (function)
- [set_c_checked_alloc](#set_c_checked_alloc) (function)
- [set_c_mem_profile](#set_c_mem_profile) (function)
- [set_c_mem_source](#set_c_mem_source) (function)
- [CGen_ct_error_text](#CGen_ct_error_text) (function)
- [cg_ct_check](#cg_ct_check) (function)
- [CCodeGen_generate](#CCodeGen_generate) (function)

## ValueNameEntry

```fides
pub struct ValueNameEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `name` | `str` |

*Defined at line 31*

---

## ValueNameEntry_new

```fides
pub fn ValueNameEntry_new(id: i64, name: str) -> ValueNameEntry {
```

**Parameters:**

- `id`: `i64`
- `name`: `str`

**Returns:** `ValueNameEntry`

*Defined at line 36*

---

## StringConstEntry

```fides
pub struct StringConstEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `val` | `str` |

*Defined at line 43*

---

## StringConstEntry_new

```fides
pub fn StringConstEntry_new(key: str, val: str) -> StringConstEntry {
```

**Parameters:**

- `key`: `str`
- `val`: `str`

**Returns:** `StringConstEntry`

*Defined at line 48*

---

## FnParamTypesEntry

```fides
pub struct FnParamTypesEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `types` | `Vec<IrType>` |

*Defined at line 55*

---

## FnParamTypesEntry_new

```fides
pub fn FnParamTypesEntry_new(key: str, types: Vec<IrType>) -> FnParamTypesEntry {
```

**Parameters:**

- `key`: `str`
- `types`: `Vec<IrType>`

**Returns:** `FnParamTypesEntry`

*Defined at line 60*

---

## FnRetTypeEntry

```fides
pub struct FnRetTypeEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `ty` | `IrType` |

*Defined at line 69*

---

## FnRetTypeEntry_new

```fides
pub fn FnRetTypeEntry_new(key: str, ty: IrType) -> FnRetTypeEntry {
```

**Parameters:**

- `key`: `str`
- `ty`: `IrType`

**Returns:** `FnRetTypeEntry`

*Defined at line 74*

---

## CCodeGen

```fides
pub struct CCodeGen {
```

**Fields:**

| Name | Type |
|------|------|
| `unchecked` | `Vec<i64>` |
| `c_wrapped` | `Vec<str>` |
| `sb` | `i64` |
| `out` | `str` |
| `value_names` | `Vec<str>` |
| `string_consts` | `Vec<StringConstEntry>` |
| `string_const_index` | `i64` |
| `fn_param_index` | `i64` |
| `fn_ret_index` | `i64` |
| `label_counter` | `i64` |
| `temp_counter` | `i64` |
| `cur_fn_c_ret_is_void` | `bool` |
| `cur_fn_c_ret_type` | `str` |
| `fn_param_types` | `Vec<FnParamTypesEntry>` |
| `user_functions` | `Vec<str>` |
| `user_fn_param_types` | `Vec<FnParamTypesEntry>` |
| `fn_ret_types` | `Vec<FnRetTypeEntry>` |
| `extern_func_names` | `Vec<str>` |
| `struct_layouts` | `Vec<StructLayoutEntry>` |
| `layout_index` | `i64` |

*Defined at line 82*

---

## CCodeGen_new

```fides
pub fn CCodeGen_new() -> CCodeGen {
```

**Returns:** `CCodeGen`

*Defined at line 125*

---

## set_c_debug_info

-g (TL5, CE8): `fd_file` is the input file and `source` its text, whose
functions get `#line` directives; `c_file` is the C file the driver
writes (where the directives hand back after each function). Every
function also gets the frame record that a panic prints.

```fides
pub fn set_c_debug_info(fd_file: str, source: str, c_file: str) {
```

**Parameters:**

- `fd_file`: `str`
- `source`: `str`
- `c_file`: `str`

*Defined at line 179*

---

## clear_c_debug_info

BG50: -g off again (`fid serve` resets it between requests).

```fides
pub fn clear_c_debug_info() {
```

*Defined at line 188*

---

## c_debug_cflags

The C compiler flag -g needs (" -g", or "" without -g).

```fides
pub fn c_debug_cflags() -> str {
```

**Returns:** `str`

*Defined at line 197*

---

## c_debug_clflags

TL5: the same for cl.exe (" /Zi": a PDB for the program, which cl's
link step writes with /DEBUG implied; "" without -g).

```fides
pub fn c_debug_clflags() -> str {
```

**Returns:** `str`

*Defined at line 206*

---

## CGen_c_file_for

The C file the driver writes next to `output` (main.fd's rule).

```fides
pub fn CGen_c_file_for(output: str) -> str {
```

**Parameters:**

- `output`: `str`

**Returns:** `str`

*Defined at line 214*

---

## set_c_register_all

A9': register every function by name and list each in __p_fns.

```fides
pub fn set_c_register_all(v: bool) {
```

**Parameters:**

- `v`: `bool`

*Defined at line 225*

---

## CGen_lib_set

CI7: generate a library (no C `main`; exported wrappers, fides_init and
fides_call). The driver then links with CGen_lib_cc_flags and writes
CGen_lib_header next to the library.

```fides
pub fn CGen_lib_set(on: bool) {
```

**Parameters:**

- `on`: `bool`

*Defined at line 232*

---

## CGen_lib_reset_exports

BG50: forget the `--export` names (`fid serve` between requests).

```fides
pub fn CGen_lib_reset_exports() {
```

*Defined at line 237*

---

## CGen_lib_export

CI7: export `name` as well as the `@export` functions.

```fides
pub fn CGen_lib_export(name: str) {
```

**Parameters:**

- `name`: `str`

*Defined at line 242*

---

## CGen_lib_export_names

2g.85: the names CGen_lib_export added (`--export`), in a library
build: roots of the C path's reachability (compiler.fd
c_reach_keep_set), as `@export` functions are (they are annotated).

```fides
pub fn CGen_lib_export_names() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 249*

---

## CGen_lib_header

CI7: the C header of the last library generated (`lib_name` names its
include guard).

```fides
pub fn CGen_lib_header(lib_name: str) -> str {
```

**Parameters:**

- `lib_name`: `str`

**Returns:** `str`

*Defined at line 267*

---

## CGen_lib_skipped

CI7: the functions marked for export that have no C form, one per line
with the reason ("" when none).

```fides
pub fn CGen_lib_skipped() -> str {
```

**Returns:** `str`

*Defined at line 274*

---

## CGen_lib_cc_flags

CI7: the C compiler flags of a shared library for target OS `os`
("windows", "macos", else ELF).

```fides
pub fn CGen_lib_cc_flags(os: str) -> str {
```

**Parameters:**

- `os`: `str`

**Returns:** `str`

*Defined at line 280*

---

## CGen_lib_file_name

CI7: the file name of library `name` for target OS `os`.

```fides
pub fn CGen_lib_file_name(name: str, os: str) -> str {
```

**Parameters:**

- `name`: `str`
- `os`: `str`

**Returns:** `str`

*Defined at line 291*

---

## c_type

```fides
pub fn c_type(ty: IrType) -> str {
```

**Parameters:**

- `ty`: `IrType`

**Returns:** `str`

*Defined at line 1070*

---

## c_type_global

```fides
pub fn c_type_global(ty: IrType) -> str {
```

**Parameters:**

- `ty`: `IrType`

**Returns:** `str`

*Defined at line 1144*

---

## cc_box_ir_type_unbox

```fides
pub fn cc_box_ir_type_unbox(b: Box<IrType>) -> IrType {
```

**Parameters:**

- `b`: `Box<IrType>`

**Returns:** `IrType`

*Defined at line 1217*

---

## OptVecIrType

```fides
pub enum OptVecIrType {
```

**Variants:**

- `Some`
- `None`

*Defined at line 2225*

---

## OptIrType

```fides
pub enum OptIrType {
```

**Variants:**

- `Some`
- `None`

*Defined at line 2230*

---

## cc_box_type_unbox

```fides
pub fn cc_box_type_unbox(b: Box<Type>) -> Type {
```

**Parameters:**

- `b`: `Box<Type>`

**Returns:** `Type`

*Defined at line 3223*

---

## set_c_unit_marks

```fides
pub fn set_c_unit_marks(v: bool) {
```

**Parameters:**

- `v`: `bool`

*Defined at line 8752*

---

## set_c_profile

```fides
pub fn set_c_profile(v: bool) {
```

**Parameters:**

- `v`: `bool`

*Defined at line 8757*

---

## cg_prof_attach

CO5: build the attaching profiler (cg_prof_attach_c) with a C compiler
(`cc_env`, else zig cc, gcc, clang or cc from the PATH) and run it on
process `pid` for `seconds`, writing collapsed stacks to `out`, one
line per sample as the in-process profiler does. 0 when it wrote
samples. The program must be built through C by a fid with
__p_prof_anchor (any `fid ccompile` build; no --profile needed).

```fides
pub fn cg_prof_attach(pid: str, seconds: str, out: str, cc_env: str, posix: bool) -> i64 {
```

**Parameters:**

- `pid`: `str`
- `seconds`: `str`
- `out`: `str`
- `cc_env`: `str`
- `posix`: `bool`

**Returns:** `i64`

*Defined at line 8768*

---

## cg_prof_attach_c

CO5: the C source of the attaching profiler: `<it> <pid> <seconds>
<out.folded>`. It finds the program's function table through
__p_prof_anchor (the magic words, then the anchor's own address), and
samples every thread about every millisecond: on Windows with
SuspendThread/GetThreadContext and dbghelp's StackWalk64 over the
program's unwind tables; on Linux with ptrace (PTRACE_SEIZE and
PTRACE_INTERRUPT), scanning the stack for return addresses whose call
instruction calls the frame found below (ranges from the executable's
.symtab). tests/tooling/co5-profile-pid-check.sh runs it.

```fides
pub fn cg_prof_attach_c() -> str {
```

**Returns:** `str`

*Defined at line 8836*

---

## set_c_leaks

```fides
pub fn set_c_leaks(v: bool) {
```

**Parameters:**

- `v`: `bool`

*Defined at line 9469*

---

## set_c_checked_alloc

```fides
pub fn set_c_checked_alloc(v: bool) {
```

**Parameters:**

- `v`: `bool`

*Defined at line 9474*

---

## set_c_mem_profile

```fides
pub fn set_c_mem_profile(v: bool) {
```

**Parameters:**

- `v`: `bool`

*Defined at line 9481*

---

## set_c_mem_source

```fides
pub fn set_c_mem_source(fd_file: str, source: str) {
```

**Parameters:**

- `fd_file`: `str`
- `source`: `str`

*Defined at line 9489*

---

## CGen_ct_error_text

```fides
pub fn CGen_ct_error_text() -> str {
```

**Returns:** `str`

*Defined at line 9552*

---

## cg_ct_check

The violations of @constant_time function `func` (named `name`), one
message each; empty when it is clean.

```fides
pub fn cg_ct_check(func: &IrFunction, name: str) -> Vec<str> {
```

**Parameters:**

- `func`: `&IrFunction`
- `name`: `str`

**Returns:** `Vec<str>`

*Defined at line 9685*

---

## CCodeGen_generate

```fides
pub fn CCodeGen_generate(self: &mut CCodeGen, module: IrModule) -> str {
```

**Parameters:**

- `self`: `&mut CCodeGen`
- `module`: `IrModule`

**Returns:** `str`

*Defined at line 9888*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
