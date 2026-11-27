[All modules](index.md)

# acpi -- Fides Documentation

## Contents

- [acpi_u8](#acpi_u8) (function)
- [acpi_u16](#acpi_u16) (function)
- [acpi_u32](#acpi_u32) (function)
- [acpi_u64](#acpi_u64) (function)
- [acpi_checksum](#acpi_checksum) (function)
- [Rsdp](#Rsdp) (struct)
- [rsdp_at](#rsdp_at) (function)
- [rsdp_find](#rsdp_find) (function)
- [SdtHeader](#SdtHeader) (struct)
- [sdt_sig_str](#sdt_sig_str) (function)
- [sdt_at](#sdt_at) (function)
- [sdt_entry_count](#sdt_entry_count) (function)
- [sdt_entry](#sdt_entry) (function)
- [madt_entry_count](#madt_entry_count) (function)
- [madt_entry](#madt_entry) (function)
- [madt_entries_of](#madt_entries_of) (function)
- [McfgEntry](#McfgEntry) (struct)
- [mcfg_entry](#mcfg_entry) (function)
- [mcfg_entry_count](#mcfg_entry_count) (function)

## acpi_u8

```fides
pub fn acpi_u8(b: Vec<i64>, o: i64) -> i64 {
```

**Parameters:**

- `b`: `Vec<i64>`
- `o`: `i64`

**Returns:** `i64`

*Defined at line 7*

---

## acpi_u16

```fides
pub fn acpi_u16(b: Vec<i64>, o: i64) -> i64 {
```

**Parameters:**

- `b`: `Vec<i64>`
- `o`: `i64`

**Returns:** `i64`

*Defined at line 10*

---

## acpi_u32

Little-endian 32-bit field at byte offset `o`.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var b: Vec<i64> = Vec::new();
b.push(0x78); b.push(0x56); b.push(0x34); b.push(0x12);
assert_eq(acpi_u8(b, 1), 0x56);
assert_eq(acpi_u16(b, 0), 0x5678);
assert_eq(acpi_u32(b, 0), 0x12345678);
```

```fides
pub fn acpi_u32(b: Vec<i64>, o: i64) -> i64 {
```

**Parameters:**

- `b`: `Vec<i64>`
- `o`: `i64`

**Returns:** `i64`

*Defined at line 21*

---

## acpi_u64

```fides
pub fn acpi_u64(b: Vec<i64>, o: i64) -> i64 {
```

**Parameters:**

- `b`: `Vec<i64>`
- `o`: `i64`

**Returns:** `i64`

*Defined at line 25*

---

## acpi_checksum

Checksum over b[o .. o+n): valid when the byte sum mod 256 is 0.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var b: Vec<i64> = Vec::new();
b.push(200); b.push(50); b.push(6);
assert(acpi_checksum(b, 0, 3));
assert(!acpi_checksum(b, 0, 2));
```

```fides
pub fn acpi_checksum(b: Vec<i64>, o: i64, n: i64) -> bool {
```

**Parameters:**

- `b`: `Vec<i64>`
- `o`: `i64`
- `n`: `i64`

**Returns:** `bool`

*Defined at line 36*

---

## Rsdp

```fides
pub struct Rsdp {
```

**Fields:**

| Name | Type |
|------|------|
| `valid` | `bool` |
| `revision` | `i64` |
| `rsdt` | `i64` |
| `xsdt` | `i64` |

*Defined at line 48*

---

## rsdp_at

Validate an RSDP at offset o ("RSD PTR " signature + checksum).

```fides
pub fn rsdp_at(b: Vec<i64>, o: i64) -> Rsdp {
```

**Parameters:**

- `b`: `Vec<i64>`
- `o`: `i64`

**Returns:** `Rsdp`

*Defined at line 56*

---

## rsdp_find

Scan the BIOS search areas — 0x40E EBDA pointer then the 0xE0000 and
legacy 0xF0000 ranges — for an RSDP. mem is the low 1 MiB of physical
memory; returns the offset or -1.

```fides
pub fn rsdp_find(mem: Vec<i64>) -> i64 {
```

**Parameters:**

- `mem`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 82*

---

## SdtHeader

```fides
pub struct SdtHeader {
```

**Fields:**

| Name | Type |
|------|------|
| `valid` | `bool` |
| `length` | `i64` |
| `revision` | `i64` |
| `oem` | `str` |
| `oem_table` | `str` |

*Defined at line 111*

---

## sdt_sig_str

The 4-character signature of the SDT at offset o ("APIC", "FACP", ...).

```fides
pub fn sdt_sig_str(b: Vec<i64>, o: i64) -> str {
```

**Parameters:**

- `b`: `Vec<i64>`
- `o`: `i64`

**Returns:** `str`

*Defined at line 120*

---

## sdt_at

SDT header at offset o; expected is the 4-char signature ("APIC",
"MCFG", ...). Returns valid=false on mismatch, bad checksum, or
overrun of the byte buffer.

```fides
pub fn sdt_at(b: Vec<i64>, o: i64, expected: str) -> SdtHeader {
```

**Parameters:**

- `b`: `Vec<i64>`
- `o`: `i64`
- `expected`: `str`

**Returns:** `SdtHeader`

*Defined at line 133*

---

## sdt_entry_count

Number of 4-byte (rsdt) or 8-byte (xsdt) table pointers in the SDT
whose header is at o. Entries sit after the 36-byte header.

```fides
pub fn sdt_entry_count(b: Vec<i64>, o: i64, xsdt: bool) -> i64 {
```

**Parameters:**

- `b`: `Vec<i64>`
- `o`: `i64`
- `xsdt`: `bool`

**Returns:** `i64`

*Defined at line 178*

---

## sdt_entry

```fides
pub fn sdt_entry(b: Vec<i64>, o: i64, i: i64, xsdt: bool) -> i64 {
```

**Parameters:**

- `b`: `Vec<i64>`
- `o`: `i64`
- `i`: `i64`
- `xsdt`: `bool`

**Returns:** `i64`

*Defined at line 189*

---

## madt_entry_count

Number of interrupt-controller structure entries in a MADT at o.

```fides
pub fn madt_entry_count(madt: Vec<i64>) -> i64 {
```

**Parameters:**

- `madt`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 201*

---

## madt_entry

Offset of MADT entry i (type at +0, length at +1), or -1.

```fides
pub fn madt_entry(madt: Vec<i64>, i: i64) -> i64 {
```

**Parameters:**

- `madt`: `Vec<i64>`
- `i`: `i64`

**Returns:** `i64`

*Defined at line 213*

---

## madt_entries_of

Offsets of all MADT entries of a given type (0 = LAPIC, 1 = IOAPIC,
2 = ISO, 9 = x2APIC).

```fides
pub fn madt_entries_of(madt: Vec<i64>, want_type: i64) -> Vec<i64> {
```

**Parameters:**

- `madt`: `Vec<i64>`
- `want_type`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 229*

---

## McfgEntry

```fides
pub struct McfgEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `base` | `i64` |
| `segment` | `i64` |
| `bus_start` | `i64` |
| `bus_end` | `i64` |

*Defined at line 245*

---

## mcfg_entry

ECAM window i of the MCFG table at o.

```fides
pub fn mcfg_entry(mcfg: Vec<i64>, i: i64) -> McfgEntry {
```

**Parameters:**

- `mcfg`: `Vec<i64>`
- `i`: `i64`

**Returns:** `McfgEntry`

*Defined at line 253*

---

## mcfg_entry_count

Number of ECAM windows in a MCFG at o.

```fides
pub fn mcfg_entry_count(mcfg: Vec<i64>) -> i64 {
```

**Parameters:**

- `mcfg`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 261*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
