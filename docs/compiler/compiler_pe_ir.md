[All modules](index.md)

# compiler::pe_ir -- Fides Documentation

## Contents

- [PeOffEnt](#PeOffEnt) (struct)
- [PeOffEnt_new](#PeOffEnt_new) (function)
- [PeStackFrame](#PeStackFrame) (struct)
- [PeStackFrame_new](#PeStackFrame_new) (function)
- [PeStackFrame_alloc](#PeStackFrame_alloc) (function)
- [PeStackFrame_alloc_raw](#PeStackFrame_alloc_raw) (function)
- [PeStackFrame_offset](#PeStackFrame_offset) (function)
- [PeDynCandidate](#PeDynCandidate) (struct)
- [pe_set_cpu](#pe_set_cpu) (function)
- [pe_set_output_path](#pe_set_output_path) (function)
- [PeirGen_generate](#PeirGen_generate) (function)
- [PeirGen_gen_function](#PeirGen_gen_function) (function)
- [PeirRaFacts](#PeirRaFacts) (struct)
- [PeirGen_gen_inst](#PeirGen_gen_inst) (function)
- [peir_recyclable](#peir_recyclable) (function)
- [peir_operand_ty](#peir_operand_ty) (function)
- [peir_int_cmp_cc](#peir_int_cmp_cc) (function)
- [PeirGen_gen_int_binop](#PeirGen_gen_int_binop) (function)
- [PeirGen_gen_arith_panics](#PeirGen_gen_arith_panics) (function)
- [PeirGen_gen_float_binop](#PeirGen_gen_float_binop) (function)
- [pe_lines_setup](#pe_lines_setup) (function)
- [peir_dbg_type_descs](#peir_dbg_type_descs) (function)
- [pe_ir_generate_flat](#pe_ir_generate_flat) (function)
- [pe_ir_generate_elf](#pe_ir_generate_elf) (function)
- [pe_ir_generate_uefi](#pe_ir_generate_uefi) (function)
- [pe_ir_generate_pe](#pe_ir_generate_pe) (function)

## PeOffEnt

Key-value pair for the frame maps (Fides HashMap is string-keyed, so
integer-keyed maps use a linear-search entry list, as x86.fd does).

```fides
pub struct PeOffEnt {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `offset` | `i64` |

*Defined at line 47*

---

## PeOffEnt_new

```fides
pub fn PeOffEnt_new(id: i64, offset: i64) -> PeOffEnt {
```

**Parameters:**

- `id`: `i64`
- `offset`: `i64`

**Returns:** `PeOffEnt`

*Defined at line 52*

---

## PeStackFrame

Per-function stack layout: every IR value gets a slot at a negative
rbp offset, alloca data slots follow the value slots.

```fides
pub struct PeStackFrame {
```

**Fields:**

| Name | Type |
|------|------|
| `offsets` | `Vec<PeOffEnt>` |
| `size` | `i64` |
| `alloca_slots` | `Vec<PeOffEnt>` |
| `const_vals` | `Vec<PeOffEnt>` |
| `reg_vars` | `Vec<PeOffEnt>` |
| `imm_only` | `Vec<i64>` |
| `cold` | `Vec<i64>` |

*Defined at line 60*

---

## PeStackFrame_new

```fides
pub fn PeStackFrame_new() -> PeStackFrame {
```

**Returns:** `PeStackFrame`

*Defined at line 79*

---

## PeStackFrame_alloc

Allocate space for a value and return its (negative) rbp offset.

```fides
pub fn PeStackFrame_alloc(self: &mut PeStackFrame, id: i64, size: i64) -> i64 {
```

**Parameters:**

- `self`: `&mut PeStackFrame`
- `id`: `i64`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 125*

---

## PeStackFrame_alloc_raw

Allocate raw space (not tied to a ValueId) and return its offset.

```fides
pub fn PeStackFrame_alloc_raw(self: &mut PeStackFrame, size: i64) -> i64 {
```

**Parameters:**

- `self`: `&mut PeStackFrame`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 144*

---

## PeStackFrame_offset

The stack offset of a value (0 when absent, as unwrap_or(&0)).

```fides
pub fn PeStackFrame_offset(self: &PeStackFrame, id: i64) -> i64 {
```

**Parameters:**

- `self`: `&PeStackFrame`
- `id`: `i64`

**Returns:** `i64`

*Defined at line 161*

---

## PeDynCandidate

(type id, function label) for one dyn-call candidate
(Rust Vec<(i64, u32)>).

```fides
pub struct PeDynCandidate {
```

**Fields:**

| Name | Type |
|------|------|
| `tid` | `i64` |
| `label` | `i64` |

*Defined at line 686*

---

## pe_set_cpu

```fides
pub fn pe_set_cpu(cpu: str) {
```

**Parameters:**

- `cpu`: `str`

*Defined at line 833*

---

## pe_set_output_path

Record the output file of the next PE build (its folder: g_pe_out_dir).

```fides
pub fn pe_set_output_path(path: str) {
```

**Parameters:**

- `path`: `str`

*Defined at line 843*

---

## PeirGen_generate

Generate the complete PE file from an IR module.

```fides
pub fn PeirGen_generate(self: &mut PeirGen, module: &IrModule) -> Vec {
```

**Parameters:**

- `self`: `&mut PeirGen`
- `module`: `&IrModule`

**Returns:** `Vec`

*Defined at line 864*

---

## PeirGen_gen_function

Generate code for a single function.

```fides
pub fn PeirGen_gen_function(self: &mut PeirGen, name: str, func: &IrFunction, module: &IrModule) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`
- `name`: `str`
- `func`: `&IrFunction`
- `module`: `&IrModule`

**Returns:** `void`

*Defined at line 1920*

---

## PeirRaFacts

What pick_reg_vars works out, for the allocator: per alloca its weight
+ 1 when it is a candidate and whether it must stay in memory; per block
its loop depth; block id -> index.

```fides
pub struct PeirRaFacts {
```

**Fields:**

| Name | Type |
|------|------|
| `weight` | `Vec<i64>` |
| `bad` | `Vec<i64>` |
| `depth` | `Vec<i64>` |
| `index` | `Vec<i64>` |

*Defined at line 3286*

---

## PeirGen_gen_inst

Generate code for a single instruction.

```fides
pub fn PeirGen_gen_inst(self: &mut PeirGen, id: i64, inst: Inst, next_cond: i64, frame: &mut PeStackFrame, func: &IrFunction, module: &IrModule, func_name: str,) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`
- `id`: `i64`
- `inst`: `Inst`
- `next_cond`: `i64`
- `frame`: `&mut PeStackFrame`
- `func`: `&IrFunction`
- `module`: `&IrModule`
- `func_name`: `str`

**Returns:** `void`

*Defined at line 4557*

---

## peir_recyclable

Slot-reuse rule: a value is a recyclable temporary iff it is defined by
an instruction inside one block and every use sits in that same block.
Params and cross-block/loop-carried values keep permanent slots.

```fides
pub fn peir_recyclable(v: i64, def_block: Vec<i64>, n_use_blocks: Vec<i64>, a_use_block: Vec<i64>, nvals: i64) -> bool {
```

**Parameters:**

- `v`: `i64`
- `def_block`: `Vec<i64>`
- `n_use_blocks`: `Vec<i64>`
- `a_use_block`: `Vec<i64>`
- `nvals`: `i64`

**Returns:** `bool`

*Defined at line 6000*

---

## peir_operand_ty

The jcc code jumping when an integer comparison `a OP b` holds (the
setcc-style numbering gen_int_binop uses: 4=E, 5=NE, C=L, D=GE, E=LE,
The IR type of value `id` of `func`: its instruction's, or a
parameter's declared type; i64 when neither is recorded.

```fides
pub fn peir_operand_ty(func: &IrFunction, id: i64) -> IrType {
```

**Parameters:**

- `func`: `&IrFunction`
- `id`: `i64`

**Returns:** `IrType`

*Defined at line 6039*

---

## peir_int_cmp_cc

F=G; unsigned 2=B, 3=AE, 6=BE, 7=A), or -1 for a non-comparison.

```fides
pub fn peir_int_cmp_cc(op: BinOp, unsigned: bool) -> i64 {
```

**Parameters:**

- `op`: `BinOp`
- `unsigned`: `bool`

**Returns:** `i64`

*Defined at line 6127*

---

## PeirGen_gen_int_binop

Integer arithmetic in type `ty`: the one meaning the interpreter and the
C backend implement. A checked operation costs one jcc to a shared panic
stub. Bare-metal code checks too (release G3): its panics go to the
program's @panic_handler, else to COM1 and a halt (gen_bare_runtime).

```fides
pub fn PeirGen_gen_int_binop(self: &mut PeirGen, op: BinOp, loff: i64, roff: i64, dst: i64, lconst: OptU64, rconst: OptU64, ty: IrType, unsigned_cmp: bool) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`
- `op`: `BinOp`
- `loff`: `i64`
- `roff`: `i64`
- `dst`: `i64`
- `lconst`: `OptU64`
- `rconst`: `OptU64`
- `ty`: `IrType`
- `unsigned_cmp`: `bool`

**Returns:** `void`

*Defined at line 6210*

---

## PeirGen_gen_arith_panics

The shared stubs behind the arithmetic panics, and the handler turning a
CPU fault into a panic with a message.

```fides
pub fn PeirGen_gen_arith_panics(self: &mut PeirGen) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`

**Returns:** `void`

*Defined at line 7335*

---

## PeirGen_gen_float_binop

Generate float binary operation.

```fides
pub fn PeirGen_gen_float_binop(self: &mut PeirGen, op: BinOp, loff: i64, roff: i64, dst: i64, ty: IrType, lty: IrType, rty: IrType) -> void {
```

**Parameters:**

- `self`: `&mut PeirGen`
- `op`: `BinOp`
- `loff`: `i64`
- `roff`: `i64`
- `dst`: `i64`
- `ty`: `IrType`
- `lty`: `IrType`
- `rty`: `IrType`

**Returns:** `void`

*Defined at line 7376*

---

## pe_lines_setup

DB3: a debug build (`fid emit --debug` / `-g`, or FID_PE_DEBUG=1)
writes line tables for the functions of the input file. main.fd calls
this with the input's source before pe_ir_generate_pe.

```fides
pub fn pe_lines_setup(source: str, input: str) {
```

**Parameters:**

- `source`: `str`
- `input`: `str`

*Defined at line 8119*

---

## peir_dbg_type_descs

N9: the module's struct types in g_nat_dbg_types' format (name, size,
then per field its name, offset, type name, size and encoding): a
field's size is the distance to the next field; a field that size of
a struct of the module holds it inline ("S").

```fides
pub fn peir_dbg_type_descs(module: &IrModule) -> Vec<str> {
```

**Parameters:**

- `module`: `&IrModule`

**Returns:** `Vec<str>`

*Defined at line 8516*

---

## pe_ir_generate_flat

Public entry point: generate a PE file from an IR module. Fails when the
program calls runtime functions this native backend does not provide.
`fid emit --bare` (release G2): a flat x86-64 binary, no OS and no C
library. `panic_handler` is the program's @panic_handler function, or "".

```fides
pub fn pe_ir_generate_flat(module: IrModule, panic_handler: str) -> ResultBytes {
```

**Parameters:**

- `module`: `IrModule`
- `panic_handler`: `str`

**Returns:** `ResultBytes`

*Defined at line 9409*

---

## pe_ir_generate_elf

`fid emit --elf` (release F4, first step): a static Linux x86-64
executable from the same machine code as the Windows and bare builds.
For now it is the bare build's runtime (print, heap, panic, exit through
Linux system calls), so a program built without the stdlib runs; the
stdlib needs the runtime of stdlib/pe_runtime on Linux (what is left).
Layout: ELF header and one program header, then at 0x401000 the flat
binary ([code][data], entered at its first byte), one RWX segment.

```fides
pub fn pe_ir_generate_elf(module: IrModule) -> ResultBytes {
```

**Parameters:**

- `module`: `IrModule`

**Returns:** `ResultBytes`

*Defined at line 9468*

---

## pe_ir_generate_uefi

`fid emit --uefi` (roadmap Phase 16): a UEFI application, a PE32+ image
of subsystem EFI_APPLICATION from the bare build's code and a runtime
over the firmware's boot services (pe_ir_stubs.fd, "UEFI application").
Like `--bare`, no stdlib. `panic_handler` is the @panic_handler, or "".
Layout (pe_build_efi): headers, .text at RVA 0x1000 (the flat binary's
code, entered at its first byte), .data after it, .reloc.

```fides
pub fn pe_ir_generate_uefi(module: IrModule, panic_handler: str) -> ResultBytes {
```

**Parameters:**

- `module`: `IrModule`
- `panic_handler`: `str`

**Returns:** `ResultBytes`

*Defined at line 9565*

---

## pe_ir_generate_pe

```fides
pub fn pe_ir_generate_pe(module: IrModule) -> ResultBytes {
```

**Parameters:**

- `module`: `IrModule`

**Returns:** `ResultBytes`

*Defined at line 9589*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
