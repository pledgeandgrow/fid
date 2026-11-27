[All modules](index.md)

# mavlink -- Fides Documentation

## Contents

- [mav_crc_acc](#mav_crc_acc) (function)
- [mav_crc_bytes](#mav_crc_bytes) (function)
- [mav_crc_str](#mav_crc_str) (function)
- [MAV_U8:](#MAV_U8:) (const)
- [MAV_I8:](#MAV_I8:) (const)
- [MAV_U16:](#MAV_U16:) (const)
- [MAV_I16:](#MAV_I16:) (const)
- [MAV_U32:](#MAV_U32:) (const)
- [MAV_I32:](#MAV_I32:) (const)
- [MAV_U64:](#MAV_U64:) (const)
- [MAV_I64:](#MAV_I64:) (const)
- [MAV_F32:](#MAV_F32:) (const)
- [MAV_F64:](#MAV_F64:) (const)
- [mav_type_size](#mav_type_size) (function)
- [mav_type_name](#mav_type_name) (function)
- [MavDef](#MavDef) (struct)
- [mav_def_new](#mav_def_new) (function)
- [mav_def_field](#mav_def_field) (function)
- [mav_def_finish](#mav_def_finish) (function)
- [mav_common_defs](#mav_common_defs) (function)
- [mav_find_def](#mav_find_def) (function)
- [mav_find_def_name](#mav_find_def_name) (function)
- [MavMsg](#MavMsg) (struct)
- [mav_msg_new](#mav_msg_new) (function)
- [mav_set](#mav_set) (function)
- [mav_set_int](#mav_set_int) (function)
- [mav_get](#mav_get) (function)
- [mav_get_int](#mav_get_int) (function)
- [mav_f32_bits](#mav_f32_bits) (function)
- [mav_f32_from_bits](#mav_f32_from_bits) (function)
- [mav_encode_payload](#mav_encode_payload) (function)
- [mav_decode_payload](#mav_decode_payload) (function)
- [mav_frame_v1](#mav_frame_v1) (function)
- [mav_truncate](#mav_truncate) (function)
- [mav_frame_v2](#mav_frame_v2) (function)
- [mav_signature](#mav_signature) (function)
- [mav_frame_v2_signed](#mav_frame_v2_signed) (function)
- [mav_sign_timestamp](#mav_sign_timestamp) (function)
- [MavFrame](#MavFrame) (struct)
- [MavParser](#MavParser) (struct)
- [mav_parser_new](#mav_parser_new) (function)
- [mav_parser_set_key](#mav_parser_set_key) (function)
- [mav_parser_feed](#mav_parser_feed) (function)
- [mav_frame_msg](#mav_frame_msg) (function)
- [mav_hex](#mav_hex) (function)

## mav_crc_acc

Accumulate one byte into a CRC-16/MCRF4XX value.

```fides
pub fn mav_crc_acc(crc: i64, b: i64) -> i64 {
```

**Parameters:**

- `crc`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 46*

---

## mav_crc_bytes

The CRC of bytes, starting from `crc` (0xFFFF for a fresh one).

```fides
pub fn mav_crc_bytes(crc: i64, data: &Vec<i64>) -> i64 {
```

**Parameters:**

- `crc`: `i64`
- `data`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 53*

---

## mav_crc_str

The CRC of the bytes of a string.

```fides
pub fn mav_crc_str(crc: i64, s: str) -> i64 {
```

**Parameters:**

- `crc`: `i64`
- `s`: `str`

**Returns:** `i64`

*Defined at line 64*

---

## MAV_U8:

```fides
pub const MAV_U8: i64 = 1;
```

*Defined at line 76*

---

## MAV_I8:

```fides
pub const MAV_I8: i64 = 2;
```

*Defined at line 77*

---

## MAV_U16:

```fides
pub const MAV_U16: i64 = 3;
```

*Defined at line 78*

---

## MAV_I16:

```fides
pub const MAV_I16: i64 = 4;
```

*Defined at line 79*

---

## MAV_U32:

```fides
pub const MAV_U32: i64 = 5;
```

*Defined at line 80*

---

## MAV_I32:

```fides
pub const MAV_I32: i64 = 6;
```

*Defined at line 81*

---

## MAV_U64:

```fides
pub const MAV_U64: i64 = 7;
```

*Defined at line 82*

---

## MAV_I64:

```fides
pub const MAV_I64: i64 = 8;
```

*Defined at line 83*

---

## MAV_F32:

```fides
pub const MAV_F32: i64 = 9;
```

*Defined at line 84*

---

## MAV_F64:

```fides
pub const MAV_F64: i64 = 10;
```

*Defined at line 85*

---

## mav_type_size

```fides
pub fn mav_type_size(t: i64) -> i64 {
```

**Parameters:**

- `t`: `i64`

**Returns:** `i64`

*Defined at line 87*

---

## mav_type_name

The C type name mavgen uses for CRC_EXTRA.

```fides
pub fn mav_type_name(t: i64) -> str {
```

**Parameters:**

- `t`: `i64`

**Returns:** `str`

*Defined at line 101*

---

## MavDef

A message definition. The field vectors are in wire order after
`mav_def_finish`.

```fides
pub struct MavDef {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `name` | `str` |
| `names` | `Vec<str>` |
| `types` | `Vec<i64>` |
| `ext` | `Vec<bool>` |
| `offsets` | `Vec<i64>` |
| `min_len` | `i64` |
| `max_len` | `i64` |
| `crc_extra` | `i64` |

*Defined at line 134*

---

## mav_def_new

```fides
pub fn mav_def_new(id: i64, name: str) -> MavDef {
```

**Parameters:**

- `id`: `i64`
- `name`: `str`

**Returns:** `MavDef`

*Defined at line 147*

---

## mav_def_field

Add a field in XML order (`ext`: after <extensions/>).

```fides
pub fn mav_def_field(d: &mut MavDef, t: i64, name: str, ext: bool) {
```

**Parameters:**

- `d`: `&mut MavDef`
- `t`: `i64`
- `name`: `str`
- `ext`: `bool`

*Defined at line 154*

---

## mav_def_finish

Put the fields in wire order and compute offsets, lengths and CRC_EXTRA.

```fides
pub fn mav_def_finish(d: &mut MavDef) {
```

**Parameters:**

- `d`: `&mut MavDef`

*Defined at line 161*

---

## mav_common_defs

HEARTBEAT, SYS_STATUS, GPS_RAW_INT, ATTITUDE, GLOBAL_POSITION_INT,
COMMAND_LONG and COMMAND_ACK from common.xml (with their extensions).

```fides
pub fn mav_common_defs() -> Vec<MavDef> {
```

**Returns:** `Vec<MavDef>`

*Defined at line 222*

---

## mav_find_def

The definition with this message id (index into defs, -1 if none).

```fides
pub fn mav_find_def(defs: &Vec<MavDef>, id: i64) -> i64 {
```

**Parameters:**

- `defs`: `&Vec<MavDef>`
- `id`: `i64`

**Returns:** `i64`

*Defined at line 326*

---

## mav_find_def_name

The definition with this name.

```fides
pub fn mav_find_def_name(defs: &Vec<MavDef>, name: str) -> i64 {
```

**Parameters:**

- `defs`: `&Vec<MavDef>`
- `name`: `str`

**Returns:** `i64`

*Defined at line 338*

---

## MavMsg

A message: its definition and one value per field (wire order); integer
fields hold whole numbers, float fields any f64 (sent as float/double).

```fides
pub struct MavMsg {
```

**Fields:**

| Name | Type |
|------|------|
| `def` | `MavDef` |
| `vals` | `Vec<f64>` |
| `ivals` | `Vec<i64>` |

*Defined at line 353*

---

## mav_msg_new

```fides
pub fn mav_msg_new(d: MavDef) -> MavMsg {
```

**Parameters:**

- `d`: `MavDef`

**Returns:** `MavMsg`

*Defined at line 360*

---

## mav_set

Set a field (an integer field takes the value truncated); false when
the message has no such field.

```fides
pub fn mav_set(m: &mut MavMsg, name: str, v: f64) -> bool {
```

**Parameters:**

- `m`: `&mut MavMsg`
- `name`: `str`
- `v`: `f64`

**Returns:** `bool`

*Defined at line 391*

---

## mav_set_int

Set an integer field exactly.

```fides
pub fn mav_set_int(m: &mut MavMsg, name: str, v: i64) -> bool {
```

**Parameters:**

- `m`: `&mut MavMsg`
- `name`: `str`
- `v`: `i64`

**Returns:** `bool`

*Defined at line 402*

---

## mav_get

```fides
pub fn mav_get(m: &MavMsg, name: str) -> f64 {
```

**Parameters:**

- `m`: `&MavMsg`
- `name`: `str`

**Returns:** `f64`

*Defined at line 412*

---

## mav_get_int

```fides
pub fn mav_get_int(m: &MavMsg, name: str) -> i64 {
```

**Parameters:**

- `m`: `&MavMsg`
- `name`: `str`

**Returns:** `i64`

*Defined at line 420*

---

## mav_f32_bits

```fides
pub fn mav_f32_bits(x: f64) -> i64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `i64`

*Defined at line 432*

---

## mav_f32_from_bits

```fides
pub fn mav_f32_from_bits(bits: i64) -> f64 {
```

**Parameters:**

- `bits`: `i64`

**Returns:** `f64`

*Defined at line 478*

---

## mav_encode_payload

The full payload (max_len bytes, extensions included).

```fides
pub fn mav_encode_payload(m: &MavMsg) -> Vec<i64> {
```

**Parameters:**

- `m`: `&MavMsg`

**Returns:** `Vec<i64>`

*Defined at line 540*

---

## mav_decode_payload

A message from a payload (zero-extended when shorter than max_len).

```fides
pub fn mav_decode_payload(d: MavDef, payload: &Vec<i64>) -> MavMsg {
```

**Parameters:**

- `d`: `MavDef`
- `payload`: `&Vec<i64>`

**Returns:** `MavMsg`

*Defined at line 558*

---

## mav_frame_v1

A MAVLink 1 frame (the base fields only; msgid must be < 256).

```fides
pub fn mav_frame_v1(m: &MavMsg, sysid: i64, compid: i64, seq: i64) -> Vec<i64> {
```

**Parameters:**

- `m`: `&MavMsg`
- `sysid`: `i64`
- `compid`: `i64`
- `seq`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 580*

---

## mav_truncate

The v2 payload: full payload with trailing zero bytes removed (one
byte is always kept).

```fides
pub fn mav_truncate(p: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `p`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 608*

---

## mav_frame_v2

A MAVLink 2 frame (unsigned).

```fides
pub fn mav_frame_v2(m: &MavMsg, sysid: i64, compid: i64, seq: i64) -> Vec<i64> {
```

**Parameters:**

- `m`: `&MavMsg`
- `sysid`: `i64`
- `compid`: `i64`
- `seq`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 654*

---

## mav_signature

The 6-byte signature of a v2 frame (everything up to and including the
CRC) for link id and timestamp.

```fides
pub fn mav_signature(key: &Vec<i64>, frame: &Vec<i64>, link_id: i64, timestamp: i64) -> Vec<i64> {
```

**Parameters:**

- `key`: `&Vec<i64>`
- `frame`: `&Vec<i64>`
- `link_id`: `i64`
- `timestamp`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 660*

---

## mav_frame_v2_signed

A signed MAVLink 2 frame (incompat flag 0x01, 13 signature bytes).

```fides
pub fn mav_frame_v2_signed(m: &MavMsg, sysid: i64, compid: i64, seq: i64, key: &Vec<i64>, link_id: i64, timestamp: i64) -> Vec<i64> {
```

**Parameters:**

- `m`: `&MavMsg`
- `sysid`: `i64`
- `compid`: `i64`
- `seq`: `i64`
- `key`: `&Vec<i64>`
- `link_id`: `i64`
- `timestamp`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 685*

---

## mav_sign_timestamp

The signing timestamp (10 us units since 2015-01-01 00:00 UTC) for a
Unix time in microseconds.

```fides
pub fn mav_sign_timestamp(unix_us: i64) -> i64 {
```

**Parameters:**

- `unix_us`: `i64`

**Returns:** `i64`

*Defined at line 700*

---

## MavFrame

A received frame.

```fides
pub struct MavFrame {
```

**Fields:**

| Name | Type |
|------|------|
| `version` | `i64` |
| `len` | `i64` |
| `incompat` | `i64` |
| `compat` | `i64` |
| `seq` | `i64` |
| `sysid` | `i64` |
| `compid` | `i64` |
| `msgid` | `i64` |
| `payload` | `Vec<i64>` |
| `signed` | `bool` |
| `link_id` | `i64` |
| `timestamp` | `i64` |
| `def` | `i64` |

*Defined at line 707*

---

## MavParser

```fides
pub struct MavParser {
```

**Fields:**

| Name | Type |
|------|------|
| `defs` | `Vec<MavDef>` |
| `buf` | `Vec<i64>` |
| `frames` | `Vec<MavFrame>` |
| `crc_errors` | `i64` |
| `sig_errors` | `i64` |
| `unknown` | `i64` |
| `dropped_bytes` | `i64` |
| `lost` | `i64` |
| `key` | `Vec<i64>` |
| `check_sig` | `bool` |
| `seq_keys` | `Vec<i64>` |
| `seq_last` | `Vec<i64>` |

*Defined at line 725*

---

## mav_parser_new

```fides
pub fn mav_parser_new(defs: Vec<MavDef>) -> MavParser {
```

**Parameters:**

- `defs`: `Vec<MavDef>`

**Returns:** `MavParser`

*Defined at line 741*

---

## mav_parser_set_key

Check signatures of signed frames with this 32-byte key.

```fides
pub fn mav_parser_set_key(p: &mut MavParser, key: Vec<i64>) {
```

**Parameters:**

- `p`: `&mut MavParser`
- `key`: `Vec<i64>`

*Defined at line 748*

---

## mav_parser_feed

Feed received bytes; returns how many good frames were added to
p.frames. Bytes that cannot start a frame, and the start byte of a
frame with a bad CRC, are skipped (the parser resynchronises on the
next start byte).

```fides
pub fn mav_parser_feed(p: &mut MavParser, bytes: Vec<i64>) -> i64 {
```

**Parameters:**

- `p`: `&mut MavParser`
- `bytes`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 886*

---

## mav_frame_msg

The message of a received frame (requires a known definition).

```fides
pub fn mav_frame_msg(p: &MavParser, f: &MavFrame) -> MavMsg {
```

**Parameters:**

- `p`: `&MavParser`
- `f`: `&MavFrame`

**Returns:** `MavMsg`

*Defined at line 909*

---

## mav_hex

Hex dump "fe 09 00 ..." of bytes.

```fides
pub fn mav_hex(b: &Vec<i64>) -> str {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `str`

*Defined at line 914*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
