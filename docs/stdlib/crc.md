[All modules](index.md)

# crc -- Fides Documentation

## Contents

- [CrcSpec](#CrcSpec) (struct)
- [crc_reflect](#crc_reflect) (function)
- [crc_spec](#crc_spec) (function)
- [crc_spec_crc8](#crc_spec_crc8) (function)
- [crc_spec_crc8_maxim](#crc_spec_crc8_maxim) (function)
- [crc_spec_crc16_ccitt_false](#crc_spec_crc16_ccitt_false) (function)
- [crc_spec_crc16_kermit](#crc_spec_crc16_kermit) (function)
- [crc_spec_crc16_xmodem](#crc_spec_crc16_xmodem) (function)
- [crc_spec_crc16_modbus](#crc_spec_crc16_modbus) (function)
- [crc_spec_crc16_arc](#crc_spec_crc16_arc) (function)
- [crc_spec_crc32](#crc_spec_crc32) (function)
- [crc_spec_crc32c](#crc_spec_crc32c) (function)
- [crc_spec_crc32_bzip2](#crc_spec_crc32_bzip2) (function)
- [crc_begin](#crc_begin) (function)
- [crc_update](#crc_update) (function)
- [crc_finish](#crc_finish) (function)
- [crc_compute](#crc_compute) (function)
- [crc_self_test](#crc_self_test) (function)
- [crc_compute_bitwise](#crc_compute_bitwise) (function)
- [crc32_bytes](#crc32_bytes) (function)
- [crc32c_bytes](#crc32c_bytes) (function)
- [crc16_modbus_bytes](#crc16_modbus_bytes) (function)
- [crc16_modbus_append](#crc16_modbus_append) (function)
- [crc16_modbus_check](#crc16_modbus_check) (function)
- [crc16_xmodem_bytes](#crc16_xmodem_bytes) (function)
- [crc16_ccitt_bytes](#crc16_ccitt_bytes) (function)
- [crc8_bytes](#crc8_bytes) (function)
- [crc_hex](#crc_hex) (function)

## CrcSpec

A CRC algorithm in the Rocksoft model, with its lookup table.

```fides
pub struct CrcSpec {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `width` | `i64` |
| `poly` | `i64` |
| `init` | `i64` |
| `refin` | `bool` |
| `refout` | `bool` |
| `xorout` | `i64` |
| `check` | `i64` |
| `mask` | `i64` |
| `table` | `Vec<i64>` |

*Defined at line 27*

---

## crc_reflect

The low `width` bits of x in reverse order.

```fides
pub fn crc_reflect(x: i64, width: i64) -> i64 {
```

**Parameters:**

- `x`: `i64`
- `width`: `i64`

**Returns:** `i64`

*Defined at line 48*

---

## crc_spec

A spec from its parameters (width 8..32). `check` is the expected CRC
of "123456789" (0 when unknown); crc_self_test compares against it.

```fides
pub fn crc_spec(name: str, width: i64, poly: i64, init: i64, refin: bool, refout: bool, xorout: i64, check: i64) -> CrcSpec {
```

**Parameters:**

- `name`: `str`
- `width`: `i64`
- `poly`: `i64`
- `init`: `i64`
- `refin`: `bool`
- `refout`: `bool`
- `xorout`: `i64`
- `check`: `i64`

**Returns:** `CrcSpec`

*Defined at line 62*

---

## crc_spec_crc8

CRC-8/SMBUS: poly 0x07, check 0xf4.

```fides
pub fn crc_spec_crc8() -> CrcSpec {
```

**Returns:** `CrcSpec`

*Defined at line 107*

---

## crc_spec_crc8_maxim

CRC-8/MAXIM-DOW (1-Wire): reflected poly 0x31, check 0xa1.

```fides
pub fn crc_spec_crc8_maxim() -> CrcSpec {
```

**Returns:** `CrcSpec`

*Defined at line 111*

---

## crc_spec_crc16_ccitt_false

CRC-16/IBM-3740, often called CCITT-FALSE: poly 0x1021, init 0xffff, check 0x29b1.

```fides
pub fn crc_spec_crc16_ccitt_false() -> CrcSpec {
```

**Returns:** `CrcSpec`

*Defined at line 115*

---

## crc_spec_crc16_kermit

CRC-16/KERMIT, the reflected "true CCITT": check 0x2189.

```fides
pub fn crc_spec_crc16_kermit() -> CrcSpec {
```

**Returns:** `CrcSpec`

*Defined at line 119*

---

## crc_spec_crc16_xmodem

CRC-16/XMODEM: poly 0x1021, init 0, check 0x31c3.

```fides
pub fn crc_spec_crc16_xmodem() -> CrcSpec {
```

**Returns:** `CrcSpec`

*Defined at line 123*

---

## crc_spec_crc16_modbus

CRC-16/MODBUS: reflected poly 0x8005, init 0xffff, check 0x4b37.

```fides
pub fn crc_spec_crc16_modbus() -> CrcSpec {
```

**Returns:** `CrcSpec`

*Defined at line 127*

---

## crc_spec_crc16_arc

CRC-16/ARC: reflected poly 0x8005, init 0, check 0xbb3d.

```fides
pub fn crc_spec_crc16_arc() -> CrcSpec {
```

**Returns:** `CrcSpec`

*Defined at line 131*

---

## crc_spec_crc32

CRC-32/ISO-HDLC (zip, PNG, Ethernet): check 0xcbf43926.

```fides
pub fn crc_spec_crc32() -> CrcSpec {
```

**Returns:** `CrcSpec`

*Defined at line 135*

---

## crc_spec_crc32c

CRC-32/ISCSI, Castagnoli (iSCSI, SCTP, ext4): check 0xe3069283.

```fides
pub fn crc_spec_crc32c() -> CrcSpec {
```

**Returns:** `CrcSpec`

*Defined at line 139*

---

## crc_spec_crc32_bzip2

CRC-32/BZIP2 (non-reflected CRC-32): check 0xfc891918.

```fides
pub fn crc_spec_crc32_bzip2() -> CrcSpec {
```

**Returns:** `CrcSpec`

*Defined at line 143*

---

## crc_begin

The register before any data. A reflected algorithm keeps its register
reflected, so the initial value is reflected too.

```fides
pub fn crc_begin(spec: &CrcSpec) -> i64 {
```

**Parameters:**

- `spec`: `&CrcSpec`

**Returns:** `i64`

*Defined at line 151*

---

## crc_update

Feed bytes (0..255 each) into the register `state`.

```fides
pub fn crc_update(spec: &CrcSpec, state: &mut i64, data: Vec<i64>) {
```

**Parameters:**

- `spec`: `&CrcSpec`
- `state`: `&mut i64`
- `data`: `Vec<i64>`

*Defined at line 159*

---

## crc_finish

The CRC value from a register: output reflection and the final XOR.

```fides
pub fn crc_finish(spec: &CrcSpec, state: i64) -> i64 {
```

**Parameters:**

- `spec`: `&CrcSpec`
- `state`: `i64`

**Returns:** `i64`

*Defined at line 180*

---

## crc_compute

The CRC of a whole byte vector.

```fides
pub fn crc_compute(spec: &CrcSpec, data: Vec<i64>) -> i64 {
```

**Parameters:**

- `spec`: `&CrcSpec`
- `data`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 189*

---

## crc_self_test

The CRC of "123456789" equals the spec's catalogue check value.

```fides
pub fn crc_self_test(spec: &CrcSpec) -> bool {
```

**Parameters:**

- `spec`: `&CrcSpec`

**Returns:** `bool`

*Defined at line 196*

---

## crc_compute_bitwise

Bit-at-a-time CRC without the table: the definition, for cross-checks.

```fides
pub fn crc_compute_bitwise(spec: &CrcSpec, data: Vec<i64>) -> i64 {
```

**Parameters:**

- `spec`: `&CrcSpec`
- `data`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 201*

---

## crc32_bytes

CRC-32 (zip/PNG) of bytes.

```fides
pub fn crc32_bytes(data: Vec<i64>) -> i64 {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 231*

---

## crc32c_bytes

CRC-32C (Castagnoli) of bytes.

```fides
pub fn crc32c_bytes(data: Vec<i64>) -> i64 {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 237*

---

## crc16_modbus_bytes

CRC-16/MODBUS of bytes. The frame carries it low byte first.

```fides
pub fn crc16_modbus_bytes(data: Vec<i64>) -> i64 {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 243*

---

## crc16_modbus_append

Append the Modbus RTU CRC to a frame (low byte, then high byte).

```fides
pub fn crc16_modbus_append(frame: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `frame`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 249*

---

## crc16_modbus_check

Whether a Modbus RTU frame ends with a correct CRC.

```fides
pub fn crc16_modbus_check(frame: Vec<i64>) -> bool {
```

**Parameters:**

- `frame`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 263*

---

## crc16_xmodem_bytes

CRC-16/XMODEM of bytes.

```fides
pub fn crc16_xmodem_bytes(data: Vec<i64>) -> i64 {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 278*

---

## crc16_ccitt_bytes

CRC-16/CCITT-FALSE (IBM-3740) of bytes.

```fides
pub fn crc16_ccitt_bytes(data: Vec<i64>) -> i64 {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 284*

---

## crc8_bytes

CRC-8/SMBUS of bytes.

```fides
pub fn crc8_bytes(data: Vec<i64>) -> i64 {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 290*

---

## crc_hex

A CRC as lowercase hex padded to the width (2, 4 or 8 digits).

```fides
pub fn crc_hex(value: i64, width: i64) -> str {
```

**Parameters:**

- `value`: `i64`
- `width`: `i64`

**Returns:** `str`

*Defined at line 296*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
