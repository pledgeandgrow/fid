[All modules](index.md)

# compiler::gpu -- Fides Documentation

## Contents

- [GpuStrI64](#GpuStrI64) (struct)
- [GpuStrI64_new](#GpuStrI64_new) (function)
- [GpuI64I64](#GpuI64I64) (struct)
- [GpuI64I64_new](#GpuI64I64_new) (function)
- [SPIRV_MAGIC:](#SPIRV_MAGIC:) (const)
- [SPIRV_VERSION_1_0:](#SPIRV_VERSION_1_0:) (const)
- [SPIRV_GENERATOR:](#SPIRV_GENERATOR:) (const)
- [SpirVCodeGen](#SpirVCodeGen) (struct)
- [SpirVCodeGen_new](#SpirVCodeGen_new) (function)
- [SpirVCodeGen_alloc_id](#SpirVCodeGen_alloc_id) (function)
- [SpirVCodeGen_get_int_type](#SpirVCodeGen_get_int_type) (function)
- [SpirVCodeGen_get_void_type](#SpirVCodeGen_get_void_type) (function)
- [SpirVCodeGen_get_bool_type](#SpirVCodeGen_get_bool_type) (function)
- [SpirVCodeGen_get_constant](#SpirVCodeGen_get_constant) (function)
- [SpirVCodeGen_get_vreg_id](#SpirVCodeGen_get_vreg_id) (function)
- [SpirVCodeGen_operand_to_id](#SpirVCodeGen_operand_to_id) (function)
- [binop_to_spirv_opcode](#binop_to_spirv_opcode) (function)
- [binop_is_spirv_comparison](#binop_is_spirv_comparison) (function)
- [SpirVCodeGen_gen_module](#SpirVCodeGen_gen_module) (function)
- [SpirVCodeGen_gen_function](#SpirVCodeGen_gen_function) (function)
- [SpirVCodeGen_gen_block](#SpirVCodeGen_gen_block) (function)
- [SpirVCodeGen_gen_inst](#SpirVCodeGen_gen_inst) (function)
- [disassemble](#disassemble) (function)
- [opcode_name](#opcode_name) (function)
- [PtxCodeGen](#PtxCodeGen) (struct)
- [PtxCodeGen_new](#PtxCodeGen_new) (function)
- [PtxCodeGen_emit_header](#PtxCodeGen_emit_header) (function)
- [PtxCodeGen_gen_module](#PtxCodeGen_gen_module) (function)
- [PtxCodeGen_gen_function](#PtxCodeGen_gen_function) (function)
- [PtxCodeGen_gen_block](#PtxCodeGen_gen_block) (function)
- [PtxCodeGen_gen_inst](#PtxCodeGen_gen_inst) (function)
- [generate_spirv](#generate_spirv) (function)
- [generate_spirv_text](#generate_spirv_text) (function)
- [generate_ptx](#generate_ptx) (function)

## GpuStrI64

```fides
pub struct GpuStrI64 {
```

**Fields:**

| Name | Type |
|------|------|
| `a` | `str` |
| `b` | `i64` |

*Defined at line 33*

---

## GpuStrI64_new

```fides
pub fn GpuStrI64_new(a: str, b: i64) -> GpuStrI64 {
```

**Parameters:**

- `a`: `str`
- `b`: `i64`

**Returns:** `GpuStrI64`

*Defined at line 38*

---

## GpuI64I64

```fides
pub struct GpuI64I64 {
```

**Fields:**

| Name | Type |
|------|------|
| `a` | `i64` |
| `b` | `i64` |

*Defined at line 44*

---

## GpuI64I64_new

```fides
pub fn GpuI64I64_new(a: i64, b: i64) -> GpuI64I64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `GpuI64I64`

*Defined at line 49*

---

## SPIRV_MAGIC:

```fides
pub const SPIRV_MAGIC: i64 = 0x07230203;
```

*Defined at line 65*

---

## SPIRV_VERSION_1_0:

```fides
pub const SPIRV_VERSION_1_0: i64 = 0x00010000;
```

*Defined at line 66*

---

## SPIRV_GENERATOR:

```fides
pub const SPIRV_GENERATOR: i64 = 0;
```

*Defined at line 67*

---

## SpirVCodeGen

```fides
pub struct SpirVCodeGen {
```

**Fields:**

| Name | Type |
|------|------|
| `words` | `Vec<i64>` |
| `types` | `Vec<i64>` |
| `vars` | `Vec<i64>` |
| `pre` | `Vec<i64>` |
| `code` | `Vec<i64>` |
| `next_id` | `i64` |
| `type_ids` | `Vec<GpuStrI64>` |
| `const_ids` | `i64` |
| `vreg_ids` | `Vec<i64>` |
| `slot_ids` | `Vec<i64>` |
| `label_ids` | `Vec<i64>` |
| `skipped` | `i64` |

*Defined at line 301*

---

## SpirVCodeGen_new

```fides
pub fn SpirVCodeGen_new() -> SpirVCodeGen {
```

**Returns:** `SpirVCodeGen`

*Defined at line 327*

---

## SpirVCodeGen_alloc_id

```fides
pub fn SpirVCodeGen_alloc_id(self: &mut SpirVCodeGen) -> i64 {
```

**Parameters:**

- `self`: `&mut SpirVCodeGen`

**Returns:** `i64`

*Defined at line 333*

---

## SpirVCodeGen_get_int_type

The signed int type of `bits` bits.

```fides
pub fn SpirVCodeGen_get_int_type(self: &mut SpirVCodeGen, bits: i64) -> i64 {
```

**Parameters:**

- `self`: `&mut SpirVCodeGen`
- `bits`: `i64`

**Returns:** `i64`

*Defined at line 420*

---

## SpirVCodeGen_get_void_type

```fides
pub fn SpirVCodeGen_get_void_type(self: &mut SpirVCodeGen) -> i64 {
```

**Parameters:**

- `self`: `&mut SpirVCodeGen`

**Returns:** `i64`

*Defined at line 424*

---

## SpirVCodeGen_get_bool_type

```fides
pub fn SpirVCodeGen_get_bool_type(self: &mut SpirVCodeGen) -> i64 {
```

**Parameters:**

- `self`: `&mut SpirVCodeGen`

**Returns:** `i64`

*Defined at line 428*

---

## SpirVCodeGen_get_constant

```fides
pub fn SpirVCodeGen_get_constant(self: &mut SpirVCodeGen, value: i64) -> i64 {
```

**Parameters:**

- `self`: `&mut SpirVCodeGen`
- `value`: `i64`

**Returns:** `i64`

*Defined at line 446*

---

## SpirVCodeGen_get_vreg_id

The variable of IR value `vreg`.

```fides
pub fn SpirVCodeGen_get_vreg_id(self: &mut SpirVCodeGen, vreg: i64) -> i64 {
```

**Parameters:**

- `self`: `&mut SpirVCodeGen`
- `vreg`: `i64`

**Returns:** `i64`

*Defined at line 482*

---

## SpirVCodeGen_operand_to_id

The id of IR value `v`'s i64 (loaded from its variable).

```fides
pub fn SpirVCodeGen_operand_to_id(self: &mut SpirVCodeGen, v: i64) -> i64 {
```

**Parameters:**

- `self`: `&mut SpirVCodeGen`
- `v`: `i64`

**Returns:** `i64`

*Defined at line 515*

---

## binop_to_spirv_opcode

The arithmetic opcode of a BinOp, 0 if it is not arithmetic. `&&` and
`||` work on 0/1 values as `&` and `|`. `%` is C's remainder (OpSRem).

```fides
pub fn binop_to_spirv_opcode(op: BinOp) -> i64 {
```

**Parameters:**

- `op`: `BinOp`

**Returns:** `i64`

*Defined at line 556*

---

## binop_is_spirv_comparison

Check if a BinOp is a comparison (result type is bool).

```fides
pub fn binop_is_spirv_comparison(op: BinOp) -> bool {
```

**Parameters:**

- `op`: `BinOp`

**Returns:** `bool`

*Defined at line 614*

---

## SpirVCodeGen_gen_module

Generate the module (into self.words): header, capabilities, memory
model, entry point, execution mode, types and constants, the function.

```fides
pub fn SpirVCodeGen_gen_module(self: &mut SpirVCodeGen, module: &IrModule) -> void {
```

**Parameters:**

- `self`: `&mut SpirVCodeGen`
- `module`: `&IrModule`

**Returns:** `void`

*Defined at line 620*

---

## SpirVCodeGen_gen_function

The function's blocks into self.code (each with its OpLabel, the IR
entry block first), its variables into self.vars, the parameters'
zero stores into self.pre.

```fides
pub fn SpirVCodeGen_gen_function(self: &mut SpirVCodeGen, func: &IrFunction) -> void {
```

**Parameters:**

- `self`: `&mut SpirVCodeGen`
- `func`: `&IrFunction`

**Returns:** `void`

*Defined at line 683*

---

## SpirVCodeGen_gen_block

One block: its label, its instructions up to the first terminator, and
OpReturn if it has none.

```fides
pub fn SpirVCodeGen_gen_block(self: &mut SpirVCodeGen, func: &IrFunction, block: BasicBlock, slots: &Vec<bool>) -> void {
```

**Parameters:**

- `self`: `&mut SpirVCodeGen`
- `func`: `&IrFunction`
- `block`: `BasicBlock`
- `slots`: `&Vec<bool>`

**Returns:** `void`

*Defined at line 704*

---

## SpirVCodeGen_gen_inst

One IR instruction (value `id`); true when it ends the block.

```fides
pub fn SpirVCodeGen_gen_inst(self: &mut SpirVCodeGen, func: &IrFunction, id: i64, inst: Inst, slots: &Vec<bool>) -> bool {
```

**Parameters:**

- `self`: `&mut SpirVCodeGen`
- `func`: `&IrFunction`
- `id`: `i64`
- `inst`: `Inst`
- `slots`: `&Vec<bool>`

**Returns:** `bool`

*Defined at line 726*

---

## disassemble

Disassemble SPIR-V words to text (one line per instruction).

```fides
pub fn disassemble(words: Vec<i64>) -> str {
```

**Parameters:**

- `words`: `Vec<i64>`

**Returns:** `str`

*Defined at line 872*

---

## opcode_name

The name of a SPIR-V opcode. (An `if` chain: a `match` on const names
binds the first arm to every value.)

```fides
pub fn opcode_name(opcode: i64) -> str {
```

**Parameters:**

- `opcode`: `i64`

**Returns:** `str`

*Defined at line 904*

---

## PtxCodeGen

```fides
pub struct PtxCodeGen {
```

**Fields:**

| Name | Type |
|------|------|
| `output` | `str` |
| `fname` | `str` |
| `skipped` | `i64` |

*Defined at line 1039*

---

## PtxCodeGen_new

```fides
pub fn PtxCodeGen_new() -> PtxCodeGen {
```

**Returns:** `PtxCodeGen`

*Defined at line 1047*

---

## PtxCodeGen_emit_header

Emit the PTX header.

```fides
pub fn PtxCodeGen_emit_header(self: &mut PtxCodeGen) -> void {
```

**Parameters:**

- `self`: `&mut PtxCodeGen`

**Returns:** `void`

*Defined at line 1054*

---

## PtxCodeGen_gen_module

Generate PTX for an IR module: its kernels (else `main`).

```fides
pub fn PtxCodeGen_gen_module(self: &mut PtxCodeGen, module: &IrModule) -> void {
```

**Parameters:**

- `self`: `&mut PtxCodeGen`
- `module`: `&IrModule`

**Returns:** `void`

*Defined at line 1082*

---

## PtxCodeGen_gen_function

One function as a `.entry` kernel.

```fides
pub fn PtxCodeGen_gen_function(self: &mut PtxCodeGen, name: str, func: &IrFunction) -> void {
```

**Parameters:**

- `self`: `&mut PtxCodeGen`
- `name`: `str`
- `func`: `&IrFunction`

**Returns:** `void`

*Defined at line 1103*

---

## PtxCodeGen_gen_block

One block: its label, its instructions up to the first terminator, and
`ret` if it has none.

```fides
pub fn PtxCodeGen_gen_block(self: &mut PtxCodeGen, func: &IrFunction, block: BasicBlock, slots: &Vec<bool>) -> void {
```

**Parameters:**

- `self`: `&mut PtxCodeGen`
- `func`: `&IrFunction`
- `block`: `BasicBlock`
- `slots`: `&Vec<bool>`

**Returns:** `void`

*Defined at line 1149*

---

## PtxCodeGen_gen_inst

One IR instruction (value `id`); true when it ends the block.

```fides
pub fn PtxCodeGen_gen_inst(self: &mut PtxCodeGen, func: &IrFunction, id: i64, inst: Inst, slots: &Vec<bool>) -> bool {
```

**Parameters:**

- `self`: `&mut PtxCodeGen`
- `func`: `&IrFunction`
- `id`: `i64`
- `inst`: `Inst`
- `slots`: `&Vec<bool>`

**Returns:** `bool`

*Defined at line 1223*

---

## generate_spirv

SPIR-V words for an IR module (a Vulkan compute shader; see
SpirVCodeGen for what is translated).

```fides
pub fn generate_spirv(module: &IrModule) -> Vec<i64> {
```

**Parameters:**

- `module`: `&IrModule`

**Returns:** `Vec<i64>`

*Defined at line 1354*

---

## generate_spirv_text

Generate SPIR-V as a text disassembly (for debugging).

```fides
pub fn generate_spirv_text(module: &IrModule) -> str {
```

**Parameters:**

- `module`: `&IrModule`

**Returns:** `str`

*Defined at line 1364*

---

## generate_ptx

Generate PTX (NVIDIA GPU assembly) for an IR module.

```fides
pub fn generate_ptx(module: &IrModule) -> str {
```

**Parameters:**

- `module`: `&IrModule`

**Returns:** `str`

*Defined at line 1370*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
