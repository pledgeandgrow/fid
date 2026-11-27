[All modules](index.md)

# compiler::x86 -- Fides Documentation

## Contents

- [StackOffsetEntry](#StackOffsetEntry) (struct)
- [StackOffsetEntry_new](#StackOffsetEntry_new) (function)
- [AllocaSlotEntry](#AllocaSlotEntry) (struct)
- [AllocaSlotEntry_new](#AllocaSlotEntry_new) (function)
- [X86StringLabelEntry](#X86StringLabelEntry) (struct)
- [X86StringLabelEntry_new](#X86StringLabelEntry_new) (function)
- [StackFrame](#StackFrame) (struct)
- [StackFrame_new](#StackFrame_new) (function)
- [StackFrame_alloc](#StackFrame_alloc) (function)
- [StackFrame_alloc_raw](#StackFrame_alloc_raw) (function)
- [StackFrame_offset](#StackFrame_offset) (function)
- [StackFrame_alloca_slot](#StackFrame_alloca_slot) (function)
- [X86CodeGen](#X86CodeGen) (struct)
- [X86CodeGen_new](#X86CodeGen_new) (function)
- [X86CodeGen_generate](#X86CodeGen_generate) (function)
- [X86CodeGen_emit_data_section](#X86CodeGen_emit_data_section) (function)
- [X86CodeGen_intern_string](#X86CodeGen_intern_string) (function)
- [X86CodeGen_gen_function](#X86CodeGen_gen_function) (function)
- [X86CodeGen_gen_inst](#X86CodeGen_gen_inst) (function)
- [X86CodeGen_gen_int_binop](#X86CodeGen_gen_int_binop) (function)
- [X86CodeGen_gen_float_binop](#X86CodeGen_gen_float_binop) (function)
- [X86CodeGen_gen_runtime_stubs](#X86CodeGen_gen_runtime_stubs) (function)
- [fmt_offset](#fmt_offset) (function)
- [escape_string](#escape_string) (function)
- [mangle_name](#mangle_name) (function)
- [generate_asm_x86](#generate_asm_x86) (function)

## StackOffsetEntry

```fides
pub struct StackOffsetEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `offset` | `i64` |

*Defined at line 38*

---

## StackOffsetEntry_new

```fides
pub fn StackOffsetEntry_new(id: i64, offset: i64) -> StackOffsetEntry {
```

**Parameters:**

- `id`: `i64`
- `offset`: `i64`

**Returns:** `StackOffsetEntry`

*Defined at line 43*

---

## AllocaSlotEntry

```fides
pub struct AllocaSlotEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `offset` | `i64` |

*Defined at line 50*

---

## AllocaSlotEntry_new

```fides
pub fn AllocaSlotEntry_new(id: i64, offset: i64) -> AllocaSlotEntry {
```

**Parameters:**

- `id`: `i64`
- `offset`: `i64`

**Returns:** `AllocaSlotEntry`

*Defined at line 55*

---

## X86StringLabelEntry

```fides
pub struct X86StringLabelEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `label` | `str` |

*Defined at line 62*

---

## X86StringLabelEntry_new

```fides
pub fn X86StringLabelEntry_new(key: str, label: str) -> X86StringLabelEntry {
```

**Parameters:**

- `key`: `str`
- `label`: `str`

**Returns:** `X86StringLabelEntry`

*Defined at line 67*

---

## StackFrame

```fides
pub struct StackFrame {
```

**Fields:**

| Name | Type |
|------|------|
| `offsets` | `Vec<StackOffsetEntry>` |
| `size` | `i64` |
| `alloca_slots` | `Vec<AllocaSlotEntry>` |

*Defined at line 77*

---

## StackFrame_new

```fides
pub fn StackFrame_new() -> StackFrame {
```

**Returns:** `StackFrame`

*Defined at line 86*

---

## StackFrame_alloc

```fides
pub fn StackFrame_alloc(self: &mut StackFrame, id: i64, size: i64) -> i64 {
```

**Parameters:**

- `self`: `&mut StackFrame`
- `id`: `i64`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 93*

---

## StackFrame_alloc_raw

```fides
pub fn StackFrame_alloc_raw(self: &mut StackFrame, size: i64) -> i64 {
```

**Parameters:**

- `self`: `&mut StackFrame`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 113*

---

## StackFrame_offset

```fides
pub fn StackFrame_offset(self: &StackFrame, id: i64) -> i64 {
```

**Parameters:**

- `self`: `&StackFrame`
- `id`: `i64`

**Returns:** `i64`

*Defined at line 130*

---

## StackFrame_alloca_slot

```fides
pub fn StackFrame_alloca_slot(self: &StackFrame, id: i64) -> i64 {
```

**Parameters:**

- `self`: `&StackFrame`
- `id`: `i64`

**Returns:** `i64`

*Defined at line 142*

---

## X86CodeGen

```fides
pub struct X86CodeGen {
```

**Fields:**

| Name | Type |
|------|------|
| `out` | `str` |
| `data` | `str` |
| `string_counter` | `i64` |
| `string_labels` | `Vec<X86StringLabelEntry>` |

*Defined at line 157*

---

## X86CodeGen_new

```fides
pub fn X86CodeGen_new() -> X86CodeGen {
```

**Returns:** `X86CodeGen`

*Defined at line 168*

---

## X86CodeGen_generate

```fides
pub fn X86CodeGen_generate(self: &mut X86CodeGen, module: &IrModule) -> str {
```

**Parameters:**

- `self`: `&mut X86CodeGen`
- `module`: `&IrModule`

**Returns:** `str`

*Defined at line 192*

---

## X86CodeGen_emit_data_section

```fides
pub fn X86CodeGen_emit_data_section(self: &mut X86CodeGen, module: &IrModule) -> void {
```

**Parameters:**

- `self`: `&mut X86CodeGen`
- `module`: `&IrModule`

**Returns:** `void`

*Defined at line 216*

---

## X86CodeGen_intern_string

```fides
pub fn X86CodeGen_intern_string(self: &mut X86CodeGen, s: str) -> str {
```

**Parameters:**

- `self`: `&mut X86CodeGen`
- `s`: `str`

**Returns:** `str`

*Defined at line 255*

---

## X86CodeGen_gen_function

```fides
pub fn X86CodeGen_gen_function(self: &mut X86CodeGen, name: str, func: &IrFunction, module: &IrModule) -> void {
```

**Parameters:**

- `self`: `&mut X86CodeGen`
- `name`: `str`
- `func`: `&IrFunction`
- `module`: `&IrModule`

**Returns:** `void`

*Defined at line 270*

---

## X86CodeGen_gen_inst

```fides
pub fn X86CodeGen_gen_inst(self: &mut X86CodeGen, id: i64, inst: Inst, frame: &StackFrame, func: &IrFunction, module: &IrModule, func_name: str,) -> void {
```

**Parameters:**

- `self`: `&mut X86CodeGen`
- `id`: `i64`
- `inst`: `Inst`
- `frame`: `&StackFrame`
- `func`: `&IrFunction`
- `module`: `&IrModule`
- `func_name`: `str`

**Returns:** `void`

*Defined at line 376*

---

## X86CodeGen_gen_int_binop

```fides
pub fn X86CodeGen_gen_int_binop(self: &mut X86CodeGen, op: BinOp, loff: i64, roff: i64, dst: i64, _ty: IrType,) -> void {
```

**Parameters:**

- `self`: `&mut X86CodeGen`
- `op`: `BinOp`
- `loff`: `i64`
- `roff`: `i64`
- `dst`: `i64`
- `_ty`: `IrType`

**Returns:** `void`

*Defined at line 968*

---

## X86CodeGen_gen_float_binop

```fides
pub fn X86CodeGen_gen_float_binop(self: &mut X86CodeGen, op: BinOp, loff: i64, roff: i64, dst: i64, ty: IrType,) -> void {
```

**Parameters:**

- `self`: `&mut X86CodeGen`
- `op`: `BinOp`
- `loff`: `i64`
- `roff`: `i64`
- `dst`: `i64`
- `ty`: `IrType`

**Returns:** `void`

*Defined at line 1059*

---

## X86CodeGen_gen_runtime_stubs

```fides
pub fn X86CodeGen_gen_runtime_stubs(self: &mut X86CodeGen) -> void {
```

**Parameters:**

- `self`: `&mut X86CodeGen`

**Returns:** `void`

*Defined at line 1202*

---

## fmt_offset

```fides
pub fn fmt_offset(offset: i64) -> str {
```

**Parameters:**

- `offset`: `i64`

**Returns:** `str`

*Defined at line 1665*

---

## escape_string

```fides
pub fn escape_string(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 1783*

---

## mangle_name

```fides
pub fn mangle_name(name: str) -> str {
```

**Parameters:**

- `name`: `str`

**Returns:** `str`

*Defined at line 1821*

---

## generate_asm_x86

```fides
pub fn generate_asm_x86(module: IrModule) -> str {
```

**Parameters:**

- `module`: `IrModule`

**Returns:** `str`

*Defined at line 1889*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
