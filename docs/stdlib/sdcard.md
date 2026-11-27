[All modules](index.md)

# sdcard -- Fides Documentation

## Contents

- [sd_crc7](#sd_crc7) (function)
- [sd_crc16](#sd_crc16) (function)
- [sd_cmd_frame](#sd_cmd_frame) (function)
- [R1_IDLE:](#R1_IDLE:) (const)
- [R1_ERASE_RESET:](#R1_ERASE_RESET:) (const)
- [R1_ILLEGAL:](#R1_ILLEGAL:) (const)
- [R1_CRC:](#R1_CRC:) (const)
- [R1_ERASE_SEQ:](#R1_ERASE_SEQ:) (const)
- [R1_ADDRESS:](#R1_ADDRESS:) (const)
- [R1_PARAMETER:](#R1_PARAMETER:) (const)
- [sd_r1_decode](#sd_r1_decode) (function)
- [SD_V1:](#SD_V1:) (const)
- [SD_V2_SC:](#SD_V2_SC:) (const)
- [SD_V2_HC:](#SD_V2_HC:) (const)
- [SdCardSim](#SdCardSim) (struct)
- [sdsim_new](#sdsim_new) (function)
- [sdsim_xfer](#sdsim_xfer) (function)
- [SdBus](#SdBus) (struct)
- [sdbus_new](#sdbus_new) (function)
- [sd_xfer](#sd_xfer) (function)
- [SdDriver](#SdDriver) (struct)
- [sd_command](#sd_command) (function)
- [sd_init](#sd_init) (function)
- [SdBlock](#SdBlock) (struct)
- [sd_read_block](#sd_read_block) (function)
- [sd_write_block](#sd_write_block) (function)

## sd_crc7

CRC7 of bytes[0..n) (polynomial 0x09), 7 bits.

```fides
pub fn sd_crc7(b: Vec<i64>, n: i64) -> i64 {
```

**Parameters:**

- `b`: `Vec<i64>`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 35*

---

## sd_crc16

CRC16-CCITT (XMODEM: polynomial 0x1021, initial 0) of bytes[from..from+n).

```fides
pub fn sd_crc16(b: Vec<i64>, from: i64, n: i64) -> i64 {
```

**Parameters:**

- `b`: `Vec<i64>`
- `from`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 55*

---

## sd_cmd_frame

The 6-byte frame of command `index` with argument `arg`.

```fides
pub fn sd_cmd_frame(index: i64, arg: i64) -> Vec<i64> {
```

**Parameters:**

- `index`: `i64`
- `arg`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 75*

---

## R1_IDLE:

```fides
pub const R1_IDLE: i64 = 0x01;
```

*Defined at line 86*

---

## R1_ERASE_RESET:

```fides
pub const R1_ERASE_RESET: i64 = 0x02;
```

*Defined at line 87*

---

## R1_ILLEGAL:

```fides
pub const R1_ILLEGAL: i64 = 0x04;
```

*Defined at line 88*

---

## R1_CRC:

```fides
pub const R1_CRC: i64 = 0x08;
```

*Defined at line 89*

---

## R1_ERASE_SEQ:

```fides
pub const R1_ERASE_SEQ: i64 = 0x10;
```

*Defined at line 90*

---

## R1_ADDRESS:

```fides
pub const R1_ADDRESS: i64 = 0x20;
```

*Defined at line 91*

---

## R1_PARAMETER:

```fides
pub const R1_PARAMETER: i64 = 0x40;
```

*Defined at line 92*

---

## sd_r1_decode

```fides
pub fn sd_r1_decode(r: i64) -> str {
```

**Parameters:**

- `r`: `i64`

**Returns:** `str`

*Defined at line 94*

---

## SD_V1:

```fides
pub const SD_V1: i64 = 1;
```

*Defined at line 128*

---

## SD_V2_SC:

```fides
pub const SD_V2_SC: i64 = 2;
```

*Defined at line 129*

---

## SD_V2_HC:

```fides
pub const SD_V2_HC: i64 = 3;
```

*Defined at line 130*

---

## SdCardSim

```fides
pub struct SdCardSim {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `blocks` | `i64` |
| `data` | `Vec<i64>` |
| `idle` | `bool` |
| `acmd` | `bool` |
| `warmup` | `i64` |
| `cmd` | `Vec<i64>` |
| `out` | `Vec<i64>` |
| `rx_mode` | `i64` |
| `rx` | `Vec<i64>` |
| `rx_addr` | `i64` |
| `blocklen` | `i64` |
| `busy` | `i64` |
| `corrupt_next_read` | `bool` |
| `log` | `Vec<str>` |

*Defined at line 132*

---

## sdsim_new

A card of `blocks` 512-byte blocks; `warmup` ACMD41 calls before ready.

```fides
pub fn sdsim_new(kind: i64, blocks: i64, warmup: i64) -> SdCardSim {
```

**Parameters:**

- `kind`: `i64`
- `blocks`: `i64`
- `warmup`: `i64`

**Returns:** `SdCardSim`

*Defined at line 151*

---

## sdsim_xfer

One SPI byte exchange from the card's side.

```fides
pub fn sdsim_xfer(c: &mut SdCardSim, b: i64) -> i64 {
```

**Parameters:**

- `c`: `&mut SdCardSim`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 305*

---

## SdBus

```fides
pub struct SdBus {
```

**Fields:**

| Name | Type |
|------|------|
| `card` | `SdCardSim` |
| `cs` | `bool` |
| `bytes` | `i64` |

*Defined at line 352*

---

## sdbus_new

```fides
pub fn sdbus_new(card: SdCardSim) -> SdBus {
```

**Parameters:**

- `card`: `SdCardSim`

**Returns:** `SdBus`

*Defined at line 358*

---

## sd_xfer

The seam: one full-duplex SPI byte. With CS high the card ignores the
bus (the line reads 0xFF).

```fides
pub fn sd_xfer(bus: &mut SdBus, b: i64) -> i64 {
```

**Parameters:**

- `bus`: `&mut SdBus`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 366*

---

## SdDriver

```fides
pub struct SdDriver {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `version` | `i64` |
| `block_addr` | `bool` |
| `error` | `str` |
| `steps` | `Vec<str>` |
| `acmd41_tries` | `i64` |

*Defined at line 374*

---

## sd_command

Send a command and wait (up to 8 bytes, NCR) for R1. Returns R1 (0xFF:
no response). Extra response bytes are read by the caller.

```fides
pub fn sd_command(bus: &mut SdBus, index: i64, arg: i64) -> i64 {
```

**Parameters:**

- `bus`: `&mut SdBus`
- `index`: `i64`
- `arg`: `i64`

**Returns:** `i64`

*Defined at line 385*

---

## sd_init

The SPI-mode initialisation flow.

```fides
pub fn sd_init(bus: &mut SdBus, max_tries: i64) -> SdDriver {
```

**Parameters:**

- `bus`: `&mut SdBus`
- `max_tries`: `i64`

**Returns:** `SdDriver`

*Defined at line 422*

---

## SdBlock

```fides
pub struct SdBlock {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `data` | `Vec<i64>` |
| `error` | `str` |

*Defined at line 486*

---

## sd_read_block

CMD17: read one block, checking its CRC16.

```fides
pub fn sd_read_block(bus: &mut SdBus, d: &SdDriver, lba: i64) -> SdBlock {
```

**Parameters:**

- `bus`: `&mut SdBus`
- `d`: `&SdDriver`
- `lba`: `i64`

**Returns:** `SdBlock`

*Defined at line 500*

---

## sd_write_block

CMD24: write one block; "" on success, else why.

```fides
pub fn sd_write_block(bus: &mut SdBus, d: &SdDriver, lba: i64, data: Vec<i64>) -> str {
```

**Parameters:**

- `bus`: `&mut SdBus`
- `d`: `&SdDriver`
- `lba`: `i64`
- `data`: `Vec<i64>`

**Returns:** `str`

*Defined at line 534*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
