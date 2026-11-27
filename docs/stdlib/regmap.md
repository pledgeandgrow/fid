[All modules](index.md)

# regmap -- Fides Documentation

## Contents

- [REG_RO:](#REG_RO:) (const)
- [REG_RW:](#REG_RW:) (const)
- [REG_WO:](#REG_WO:) (const)
- [REG_W1C:](#REG_W1C:) (const)
- [regmap_access_name](#regmap_access_name) (function)
- [regmap_access_parse](#regmap_access_parse) (function)
- [RegField](#RegField) (struct)
- [RegDesc](#RegDesc) (struct)
- [RegBlock](#RegBlock) (struct)
- [regfield_new](#regfield_new) (function)
- [regfield_add_enum](#regfield_add_enum) (function)
- [regdesc_new](#regdesc_new) (function)
- [regdesc_add_field](#regdesc_add_field) (function)
- [regblock_new](#regblock_new) (function)
- [regblock_add](#regblock_add) (function)
- [regblock_find](#regblock_find) (function)
- [regdesc_find_field](#regdesc_find_field) (function)
- [regblock_addr](#regblock_addr) (function)
- [regmap_ones](#regmap_ones) (function)
- [regfield_mask](#regfield_mask) (function)
- [regfield_get](#regfield_get) (function)
- [regfield_set](#regfield_set) (function)
- [regfield_enum_name](#regfield_enum_name) (function)
- [regfield_enum_value](#regfield_enum_value) (function)
- [regdesc_w1c_mask](#regdesc_w1c_mask) (function)
- [regdesc_writable_mask](#regdesc_writable_mask) (function)
- [regdesc_rmw_value](#regdesc_rmw_value) (function)
- [regblock_check](#regblock_check) (function)
- [regmap_hex](#regmap_hex) (function)
- [regmap_hex_digits](#regmap_hex_digits) (function)
- [RegBus](#RegBus) (struct)
- [regbus_fake](#regbus_fake) (function)
- [regbus_mmio](#regbus_mmio) (function)
- [regbus_attach](#regbus_attach) (function)
- [regbus_poke](#regbus_poke) (function)
- [regbus_peek](#regbus_peek) (function)
- [regbus_read](#regbus_read) (function)
- [regbus_write](#regbus_write) (function)
- [regmap_read](#regmap_read) (function)
- [regmap_write](#regmap_write) (function)
- [regmap_read_field](#regmap_read_field) (function)
- [regmap_write_field](#regmap_write_field) (function)
- [regmap_write_enum](#regmap_write_enum) (function)
- [regmap_write_fields](#regmap_write_fields) (function)
- [regmap_clear_flags](#regmap_clear_flags) (function)
- [regmap_wait_field](#regmap_wait_field) (function)
- [regdesc_decode](#regdesc_decode) (function)
- [regmap_dump](#regmap_dump) (function)

## REG_RO:

Read-only: writes are ignored by the hardware.

```fides
pub const REG_RO: i64 = 0;
```

*Defined at line 40*

---

## REG_RW:

Read-write.

```fides
pub const REG_RW: i64 = 1;
```

*Defined at line 42*

---

## REG_WO:

Write-only: reads give 0 (or garbage); the shadow keeps the last write.

```fides
pub const REG_WO: i64 = 2;
```

*Defined at line 44*

---

## REG_W1C:

Write-1-to-clear: reading gives the flags, writing 1 clears a flag,
writing 0 leaves it.

```fides
pub const REG_W1C: i64 = 3;
```

*Defined at line 47*

---

## regmap_access_name

```fides
pub fn regmap_access_name(a: i64) -> str {
```

**Parameters:**

- `a`: `i64`

**Returns:** `str`

*Defined at line 49*

---

## regmap_access_parse

Parse "RO", "RW", "WO", "W1C" (also the SVD spellings, see svd.fd);
-1 when unknown.

```fides
pub fn regmap_access_parse(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 67*

---

## RegField

A bit field: bits lsb .. lsb+width-1 of its register.

```fides
pub struct RegField {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `lsb` | `i64` |
| `width` | `i64` |
| `access` | `i64` |
| `desc` | `str` |
| `enum_names` | `Vec<str>` |
| `enum_vals` | `Vec<i64>` |

*Defined at line 86*

---

## RegDesc

A register at `offset` bytes from the block base.

```fides
pub struct RegDesc {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `offset` | `i64` |
| `width` | `i64` |
| `access` | `i64` |
| `reset` | `i64` |
| `desc` | `str` |
| `fields` | `Vec<RegField>` |

*Defined at line 97*

---

## RegBlock

A peripheral: a named base address and its registers.

```fides
pub struct RegBlock {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `base` | `i64` |
| `regs` | `Vec<RegDesc>` |

*Defined at line 108*

---

## regfield_new

```fides
pub fn regfield_new(name: str, lsb: i64, width: i64, access: i64) -> RegField {
```

**Parameters:**

- `name`: `str`
- `lsb`: `i64`
- `width`: `i64`
- `access`: `i64`

**Returns:** `RegField`

*Defined at line 114*

---

## regfield_add_enum

Add an enumerated value (name, value) to a field.

```fides
pub fn regfield_add_enum(f: &mut RegField, name: str, value: i64) {
```

**Parameters:**

- `f`: `&mut RegField`
- `name`: `str`
- `value`: `i64`

*Defined at line 121*

---

## regdesc_new

```fides
pub fn regdesc_new(name: str, offset: i64, width: i64, access: i64, reset: i64) -> RegDesc {
```

**Parameters:**

- `name`: `str`
- `offset`: `i64`
- `width`: `i64`
- `access`: `i64`
- `reset`: `i64`

**Returns:** `RegDesc`

*Defined at line 126*

---

## regdesc_add_field

```fides
pub fn regdesc_add_field(r: &mut RegDesc, f: RegField) {
```

**Parameters:**

- `r`: `&mut RegDesc`
- `f`: `RegField`

*Defined at line 132*

---

## regblock_new

```fides
pub fn regblock_new(name: str, base: i64) -> RegBlock {
```

**Parameters:**

- `name`: `str`
- `base`: `i64`

**Returns:** `RegBlock`

*Defined at line 136*

---

## regblock_add

```fides
pub fn regblock_add(b: &mut RegBlock, r: RegDesc) {
```

**Parameters:**

- `b`: `&mut RegBlock`
- `r`: `RegDesc`

*Defined at line 142*

---

## regblock_find

Index of the register named `name`, -1 when absent.

```fides
pub fn regblock_find(b: &RegBlock, name: str) -> i64 {
```

**Parameters:**

- `b`: `&RegBlock`
- `name`: `str`

**Returns:** `i64`

*Defined at line 147*

---

## regdesc_find_field

Index of the field named `name` in `r`, -1 when absent.

```fides
pub fn regdesc_find_field(r: &RegDesc, name: str) -> i64 {
```

**Parameters:**

- `r`: `&RegDesc`
- `name`: `str`

**Returns:** `i64`

*Defined at line 159*

---

## regblock_addr

Absolute address of register `name` (panics when absent).

```fides
pub fn regblock_addr(b: &RegBlock, name: str) -> i64 {
```

**Parameters:**

- `b`: `&RegBlock`
- `name`: `str`

**Returns:** `i64`

*Defined at line 171*

---

## regmap_ones

A mask of `width` ones (width 0..64).

```fides
pub fn regmap_ones(width: i64) -> i64 {
```

**Parameters:**

- `width`: `i64`

**Returns:** `i64`

*Defined at line 182*

---

## regfield_mask

The bits a field covers, in place.

```fides
pub fn regfield_mask(f: &RegField) -> i64 {
```

**Parameters:**

- `f`: `&RegField`

**Returns:** `i64`

*Defined at line 193*

---

## regfield_get

The field's value in a register value.

```fides
pub fn regfield_get(f: &RegField, regval: i64) -> i64 {
```

**Parameters:**

- `f`: `&RegField`
- `regval`: `i64`

**Returns:** `i64`

*Defined at line 198*

---

## regfield_set

`regval` with the field set to `x` (panics when x does not fit).

```fides
pub fn regfield_set(f: &RegField, regval: i64, x: i64) -> i64 {
```

**Parameters:**

- `f`: `&RegField`
- `regval`: `i64`
- `x`: `i64`

**Returns:** `i64`

*Defined at line 203*

---

## regfield_enum_name

Name of the enumerated value `x` of a field, "" when none.

```fides
pub fn regfield_enum_name(f: &RegField, x: i64) -> str {
```

**Parameters:**

- `f`: `&RegField`
- `x`: `i64`

**Returns:** `str`

*Defined at line 212*

---

## regfield_enum_value

Value of the enumerated name, -1 when none.

```fides
pub fn regfield_enum_value(f: &RegField, name: str) -> i64 {
```

**Parameters:**

- `f`: `&RegField`
- `name`: `str`

**Returns:** `i64`

*Defined at line 224*

---

## regdesc_w1c_mask

Bits of the register that are W1C (from the register's access when it
has no fields, else from its W1C fields).

```fides
pub fn regdesc_w1c_mask(r: &RegDesc) -> i64 {
```

**Parameters:**

- `r`: `&RegDesc`

**Returns:** `i64`

*Defined at line 237*

---

## regdesc_writable_mask

Bits a write can change (not RO). A register without fields follows
its own access.

```fides
pub fn regdesc_writable_mask(r: &RegDesc) -> i64 {
```

**Parameters:**

- `r`: `&RegDesc`

**Returns:** `i64`

*Defined at line 258*

---

## regdesc_rmw_value

The value to write to set field `fi` of `r` to `x`, given the value
`old` read from the register: the other RW bits are kept, the W1C
bits are written 0 (so no pending flag is cleared by accident) except
the target field when it is W1C itself (then `x` is the set of flags
to clear).

```fides
pub fn regdesc_rmw_value(r: &RegDesc, old: i64, fi: i64, x: i64) -> i64 {
```

**Parameters:**

- `r`: `&RegDesc`
- `old`: `i64`
- `fi`: `i64`
- `x`: `i64`

**Returns:** `i64`

*Defined at line 282*

---

## regblock_check

Problems of a block: fields overlapping each other, beyond the
register width, registers overlapping, offsets not aligned to their
width. Empty when consistent.

```fides
pub fn regblock_check(b: &RegBlock) -> Vec<str> {
```

**Parameters:**

- `b`: `&RegBlock`

**Returns:** `Vec<str>`

*Defined at line 292*

---

## regmap_hex

`v` as 0x followed by at least `digits` hex digits (the 64-bit
pattern for negative values).

```fides
pub fn regmap_hex(v: i64, digits: i64) -> str {
```

**Parameters:**

- `v`: `i64`
- `digits`: `i64`

**Returns:** `str`

*Defined at line 339*

---

## regmap_hex_digits

```fides
pub fn regmap_hex_digits(v: i64, digits: i64) -> str {
```

**Parameters:**

- `v`: `i64`
- `digits`: `i64`

**Returns:** `str`

*Defined at line 343*

---

## RegBus

Register access. mode 0: fake register file (`mem`, with the access
rules of attached registers in `rules`); mode 1: real MMIO.

```fides
pub struct RegBus {
```

**Fields:**

| Name | Type |
|------|------|
| `mode` | `i64` |
| `mem` | `HashMap<i64` |
| `ro_mask` | `HashMap<i64` |
| `w1c_mask` | `HashMap<i64` |
| `wo` | `HashMap<i64` |
| `log` | `Vec<str>` |
| `logging` | `bool` |
| `reads` | `i64` |
| `writes` | `i64` |

*Defined at line 364*

---

## regbus_fake

A fake register file: reads of unknown addresses give 0.

```fides
pub fn regbus_fake() -> RegBus {
```

**Returns:** `RegBus`

*Defined at line 381*

---

## regbus_mmio

A bus on real memory-mapped I/O (addresses are physical/bus addresses
the program can reach).

```fides
pub fn regbus_mmio() -> RegBus {
```

**Returns:** `RegBus`

*Defined at line 389*

---

## regbus_attach

Load a block into a fake bus: reset values, and which bits are RO, W1C
or write-only, so the fake behaves as the hardware would.

```fides
pub fn regbus_attach(bus: &mut RegBus, b: RegBlock) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `b`: `RegBlock`

*Defined at line 429*

---

## regbus_poke

Hardware side of the fake: set bits of a register directly (a status
flag rising, a received byte), ignoring the access rules.

```fides
pub fn regbus_poke(bus: &mut RegBus, addr: i64, v: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `addr`: `i64`
- `v`: `i64`

*Defined at line 446*

---

## regbus_peek

What the fake holds at `addr` (no access rules, no log).

```fides
pub fn regbus_peek(bus: &RegBus, addr: i64) -> i64 {
```

**Parameters:**

- `bus`: `&RegBus`
- `addr`: `i64`

**Returns:** `i64`

*Defined at line 451*

---

## regbus_read

Read `width` bits at `addr`.

```fides
pub fn regbus_read(bus: &mut RegBus, addr: i64, width: i64) -> i64 {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `addr`: `i64`
- `width`: `i64`

**Returns:** `i64`

*Defined at line 459*

---

## regbus_write

Write `width` bits at `addr`. On the fake: RO bits keep their value,
a 1 written to a W1C bit clears it, other bits take the written value.

```fides
pub fn regbus_write(bus: &mut RegBus, addr: i64, width: i64, v: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `addr`: `i64`
- `width`: `i64`
- `v`: `i64`

*Defined at line 479*

---

## regmap_read

Read register `name` of block `b`.

```fides
pub fn regmap_read(bus: &mut RegBus, b: RegBlock, name: str) -> i64 {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `b`: `RegBlock`
- `name`: `str`

**Returns:** `i64`

*Defined at line 522*

---

## regmap_write

Write register `name` (the whole value).

```fides
pub fn regmap_write(bus: &mut RegBus, b: RegBlock, name: str, v: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `b`: `RegBlock`
- `name`: `str`
- `v`: `i64`

*Defined at line 528*

---

## regmap_read_field

Read one field.

```fides
pub fn regmap_read_field(bus: &mut RegBus, b: RegBlock, reg: str, field: str) -> i64 {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `b`: `RegBlock`
- `reg`: `str`
- `field`: `str`

**Returns:** `i64`

*Defined at line 537*

---

## regmap_write_field

Read-modify-write one field. A write-only register cannot be read:
its reset value is the base (callers of WO registers usually write
whole values). W1C bits of other fields are written 0.

```fides
pub fn regmap_write_field(bus: &mut RegBus, b: RegBlock, reg: str, field: str, x: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `b`: `RegBlock`
- `reg`: `str`
- `field`: `str`
- `x`: `i64`

*Defined at line 550*

---

## regmap_write_enum

Set a field by enumerated name ("FAST").

```fides
pub fn regmap_write_enum(bus: &mut RegBus, b: RegBlock, reg: str, field: str, ename: str) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `b`: `RegBlock`
- `reg`: `str`
- `field`: `str`
- `ename`: `str`

*Defined at line 568*

---

## regmap_write_fields

Several fields of one register in one read-modify-write.

```fides
pub fn regmap_write_fields(bus: &mut RegBus, b: RegBlock, reg: str, fields: Vec<str>, vals: Vec<i64>) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `b`: `RegBlock`
- `reg`: `str`
- `fields`: `Vec<str>`
- `vals`: `Vec<i64>`

*Defined at line 582*

---

## regmap_clear_flags

Clear W1C flags: write the mask (only the given bits).

```fides
pub fn regmap_clear_flags(bus: &mut RegBus, b: RegBlock, reg: str, mask: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `b`: `RegBlock`
- `reg`: `str`
- `mask`: `i64`

*Defined at line 603*

---

## regmap_wait_field

Poll a field until it equals `want`, at most `tries` reads. True when
it did.

```fides
pub fn regmap_wait_field(bus: &mut RegBus, b: RegBlock, reg: str, field: str, want: i64, tries: i64) -> bool {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `b`: `RegBlock`
- `reg`: `str`
- `field`: `str`
- `want`: `i64`
- `tries`: `i64`

**Returns:** `bool`

*Defined at line 610*

---

## regdesc_decode

One register value decoded field by field: "EN=1 MODE=FAST(2)".

```fides
pub fn regdesc_decode(r: &RegDesc, v: i64) -> str {
```

**Parameters:**

- `r`: `&RegDesc`
- `v`: `i64`

**Returns:** `str`

*Defined at line 624*

---

## regmap_dump

The block, one register per line, read through the bus (write-only
registers are shown as "--" and not read).

```fides
pub fn regmap_dump(bus: &mut RegBus, b: RegBlock) -> str {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `b`: `RegBlock`

**Returns:** `str`

*Defined at line 656*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
