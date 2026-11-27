[All modules](index.md)

# dnp3 -- Fides Documentation

## Contents

- [DNP3_LINK_RESET:](#DNP3_LINK_RESET:) (const)
- [DNP3_LINK_RESET_USER:](#DNP3_LINK_RESET_USER:) (const)
- [DNP3_LINK_TEST:](#DNP3_LINK_TEST:) (const)
- [DNP3_LINK_CONFIRMED:](#DNP3_LINK_CONFIRMED:) (const)
- [DNP3_LINK_UNCONFIRMED:](#DNP3_LINK_UNCONFIRMED:) (const)
- [DNP3_LINK_REQUEST_STATUS:](#DNP3_LINK_REQUEST_STATUS:) (const)
- [DNP3_FC_CONFIRM:](#DNP3_FC_CONFIRM:) (const)
- [DNP3_FC_READ:](#DNP3_FC_READ:) (const)
- [DNP3_FC_WRITE:](#DNP3_FC_WRITE:) (const)
- [DNP3_FC_SELECT:](#DNP3_FC_SELECT:) (const)
- [DNP3_FC_OPERATE:](#DNP3_FC_OPERATE:) (const)
- [DNP3_FC_DIRECT_OPERATE:](#DNP3_FC_DIRECT_OPERATE:) (const)
- [DNP3_FC_FREEZE:](#DNP3_FC_FREEZE:) (const)
- [DNP3_FC_COLD_RESTART:](#DNP3_FC_COLD_RESTART:) (const)
- [DNP3_FC_WARM_RESTART:](#DNP3_FC_WARM_RESTART:) (const)
- [DNP3_FC_ENABLE_UNSOL:](#DNP3_FC_ENABLE_UNSOL:) (const)
- [DNP3_FC_DISABLE_UNSOL:](#DNP3_FC_DISABLE_UNSOL:) (const)
- [DNP3_FC_RESPONSE:](#DNP3_FC_RESPONSE:) (const)
- [DNP3_FC_UNSOL_RESPONSE:](#DNP3_FC_UNSOL_RESPONSE:) (const)
- [DNP3_CROB_NUL:](#DNP3_CROB_NUL:) (const)
- [DNP3_CROB_PULSE_ON:](#DNP3_CROB_PULSE_ON:) (const)
- [DNP3_CROB_PULSE_OFF:](#DNP3_CROB_PULSE_OFF:) (const)
- [DNP3_CROB_LATCH_ON:](#DNP3_CROB_LATCH_ON:) (const)
- [DNP3_CROB_LATCH_OFF:](#DNP3_CROB_LATCH_OFF:) (const)
- [DNP3_CROB_TRIP:](#DNP3_CROB_TRIP:) (const)
- [DNP3_CROB_CLOSE:](#DNP3_CROB_CLOSE:) (const)
- [DNP3_STATUS_SUCCESS:](#DNP3_STATUS_SUCCESS:) (const)
- [DNP3_STATUS_TIMEOUT:](#DNP3_STATUS_TIMEOUT:) (const)
- [DNP3_STATUS_NO_SELECT:](#DNP3_STATUS_NO_SELECT:) (const)
- [DNP3_STATUS_FORMAT_ERROR:](#DNP3_STATUS_FORMAT_ERROR:) (const)
- [DNP3_STATUS_NOT_SUPPORTED:](#DNP3_STATUS_NOT_SUPPORTED:) (const)
- [DNP3_IIN_ALL_STATIONS:](#DNP3_IIN_ALL_STATIONS:) (const)
- [DNP3_IIN_CLASS1:](#DNP3_IIN_CLASS1:) (const)
- [DNP3_IIN_NEED_TIME:](#DNP3_IIN_NEED_TIME:) (const)
- [DNP3_IIN_DEVICE_RESTART:](#DNP3_IIN_DEVICE_RESTART:) (const)
- [DNP3_IIN_NO_FUNC_CODE:](#DNP3_IIN_NO_FUNC_CODE:) (const)
- [DNP3_IIN_OBJECT_UNKNOWN:](#DNP3_IIN_OBJECT_UNKNOWN:) (const)
- [DNP3_IIN_PARAM_ERROR:](#DNP3_IIN_PARAM_ERROR:) (const)
- [dnp3_crc](#dnp3_crc) (function)
- [dnp3_crc_all](#dnp3_crc_all) (function)
- [Dnp3LinkFrame](#Dnp3LinkFrame) (struct)
- [dnp3_link_frame](#dnp3_link_frame) (function)
- [dnp3_link_parse](#dnp3_link_parse) (function)
- [dnp3_transport_segment](#dnp3_transport_segment) (function)
- [Dnp3Assembly](#Dnp3Assembly) (struct)
- [dnp3_transport_assemble](#dnp3_transport_assemble) (function)
- [Dnp3ObjHeader](#Dnp3ObjHeader) (struct)
- [dnp3_objhdr_range](#dnp3_objhdr_range) (function)
- [dnp3_objhdr_all](#dnp3_objhdr_all) (function)
- [dnp3_objhdr_count](#dnp3_objhdr_count) (function)
- [dnp3_objhdr_count_prefix8](#dnp3_objhdr_count_prefix8) (function)
- [Dnp3ObjHeaderAt](#Dnp3ObjHeaderAt) (struct)
- [dnp3_objhdr_decode](#dnp3_objhdr_decode) (function)
- [dnp3_app_ctrl](#dnp3_app_ctrl) (function)
- [dnp3_build_read](#dnp3_build_read) (function)
- [dnp3_build_read_class0](#dnp3_build_read_class0) (function)
- [Dnp3AppRequest](#Dnp3AppRequest) (struct)
- [dnp3_app_request](#dnp3_app_request) (function)
- [Dnp3AppResponse](#Dnp3AppResponse) (struct)
- [dnp3_app_response_header](#dnp3_app_response_header) (function)
- [dnp3_crob_encode](#dnp3_crob_encode) (function)
- [Dnp3Crob](#Dnp3Crob) (struct)
- [dnp3_crob_decode](#dnp3_crob_decode) (function)
- [dnp3_binary_flags](#dnp3_binary_flags) (function)
- [dnp3_analog32_encode](#dnp3_analog32_encode) (function)
- [dnp3_analog32_value](#dnp3_analog32_value) (function)
- [dnp3_counter32_encode](#dnp3_counter32_encode) (function)
- [dnp3_counter32_value](#dnp3_counter32_value) (function)
- [Dnp3Outstation](#Dnp3Outstation) (struct)
- [dnp3_outstation_new](#dnp3_outstation_new) (function)
- [dnp3_outstation_select](#dnp3_outstation_select) (function)
- [dnp3_outstation_operate](#dnp3_outstation_operate) (function)
- [dnp3_outstation_class0](#dnp3_outstation_class0) (function)

## DNP3_LINK_RESET:

```fides
pub const DNP3_LINK_RESET: i64 = 0;  // reset link states
```

*Defined at line 40*

---

## DNP3_LINK_RESET_USER:

```fides
pub const DNP3_LINK_RESET_USER: i64 = 1;
```

*Defined at line 41*

---

## DNP3_LINK_TEST:

```fides
pub const DNP3_LINK_TEST: i64 = 2;
```

*Defined at line 42*

---

## DNP3_LINK_CONFIRMED:

```fides
pub const DNP3_LINK_CONFIRMED: i64 = 3;  // user data, confirmed
```

*Defined at line 43*

---

## DNP3_LINK_UNCONFIRMED:

```fides
pub const DNP3_LINK_UNCONFIRMED: i64 = 4;  // user data, unconfirmed
```

*Defined at line 44*

---

## DNP3_LINK_REQUEST_STATUS:

```fides
pub const DNP3_LINK_REQUEST_STATUS: i64 = 9;
```

*Defined at line 45*

---

## DNP3_FC_CONFIRM:

```fides
pub const DNP3_FC_CONFIRM: i64 = 0;
```

*Defined at line 48*

---

## DNP3_FC_READ:

```fides
pub const DNP3_FC_READ: i64 = 1;
```

*Defined at line 49*

---

## DNP3_FC_WRITE:

```fides
pub const DNP3_FC_WRITE: i64 = 2;
```

*Defined at line 50*

---

## DNP3_FC_SELECT:

```fides
pub const DNP3_FC_SELECT: i64 = 3;
```

*Defined at line 51*

---

## DNP3_FC_OPERATE:

```fides
pub const DNP3_FC_OPERATE: i64 = 4;
```

*Defined at line 52*

---

## DNP3_FC_DIRECT_OPERATE:

```fides
pub const DNP3_FC_DIRECT_OPERATE: i64 = 5;
```

*Defined at line 53*

---

## DNP3_FC_FREEZE:

```fides
pub const DNP3_FC_FREEZE: i64 = 7;
```

*Defined at line 54*

---

## DNP3_FC_COLD_RESTART:

```fides
pub const DNP3_FC_COLD_RESTART: i64 = 13;
```

*Defined at line 55*

---

## DNP3_FC_WARM_RESTART:

```fides
pub const DNP3_FC_WARM_RESTART: i64 = 14;
```

*Defined at line 56*

---

## DNP3_FC_ENABLE_UNSOL:

```fides
pub const DNP3_FC_ENABLE_UNSOL: i64 = 20;
```

*Defined at line 57*

---

## DNP3_FC_DISABLE_UNSOL:

```fides
pub const DNP3_FC_DISABLE_UNSOL: i64 = 21;
```

*Defined at line 58*

---

## DNP3_FC_RESPONSE:

```fides
pub const DNP3_FC_RESPONSE: i64 = 129;
```

*Defined at line 59*

---

## DNP3_FC_UNSOL_RESPONSE:

```fides
pub const DNP3_FC_UNSOL_RESPONSE: i64 = 130;
```

*Defined at line 60*

---

## DNP3_CROB_NUL:

```fides
pub const DNP3_CROB_NUL: i64 = 0;
```

*Defined at line 63*

---

## DNP3_CROB_PULSE_ON:

```fides
pub const DNP3_CROB_PULSE_ON: i64 = 1;
```

*Defined at line 64*

---

## DNP3_CROB_PULSE_OFF:

```fides
pub const DNP3_CROB_PULSE_OFF: i64 = 2;
```

*Defined at line 65*

---

## DNP3_CROB_LATCH_ON:

```fides
pub const DNP3_CROB_LATCH_ON: i64 = 3;
```

*Defined at line 66*

---

## DNP3_CROB_LATCH_OFF:

```fides
pub const DNP3_CROB_LATCH_OFF: i64 = 4;
```

*Defined at line 67*

---

## DNP3_CROB_TRIP:

```fides
pub const DNP3_CROB_TRIP: i64 = 0x81;  // trip | pulse-on
```

*Defined at line 68*

---

## DNP3_CROB_CLOSE:

```fides
pub const DNP3_CROB_CLOSE: i64 = 0x41;  // close | pulse-on
```

*Defined at line 69*

---

## DNP3_STATUS_SUCCESS:

```fides
pub const DNP3_STATUS_SUCCESS: i64 = 0;
```

*Defined at line 72*

---

## DNP3_STATUS_TIMEOUT:

```fides
pub const DNP3_STATUS_TIMEOUT: i64 = 1;
```

*Defined at line 73*

---

## DNP3_STATUS_NO_SELECT:

```fides
pub const DNP3_STATUS_NO_SELECT: i64 = 2;
```

*Defined at line 74*

---

## DNP3_STATUS_FORMAT_ERROR:

```fides
pub const DNP3_STATUS_FORMAT_ERROR: i64 = 3;
```

*Defined at line 75*

---

## DNP3_STATUS_NOT_SUPPORTED:

```fides
pub const DNP3_STATUS_NOT_SUPPORTED: i64 = 4;
```

*Defined at line 76*

---

## DNP3_IIN_ALL_STATIONS:

```fides
pub const DNP3_IIN_ALL_STATIONS: i64 = 0x0100;
```

*Defined at line 79*

---

## DNP3_IIN_CLASS1:

```fides
pub const DNP3_IIN_CLASS1: i64 = 0x0200;
```

*Defined at line 80*

---

## DNP3_IIN_NEED_TIME:

```fides
pub const DNP3_IIN_NEED_TIME: i64 = 0x0010;
```

*Defined at line 81*

---

## DNP3_IIN_DEVICE_RESTART:

```fides
pub const DNP3_IIN_DEVICE_RESTART: i64 = 0x0080;
```

*Defined at line 82*

---

## DNP3_IIN_NO_FUNC_CODE:

```fides
pub const DNP3_IIN_NO_FUNC_CODE: i64 = 0x0001;  // byte2 bit0
```

*Defined at line 83*

---

## DNP3_IIN_OBJECT_UNKNOWN:

```fides
pub const DNP3_IIN_OBJECT_UNKNOWN: i64 = 0x0002;
```

*Defined at line 84*

---

## DNP3_IIN_PARAM_ERROR:

```fides
pub const DNP3_IIN_PARAM_ERROR: i64 = 0x0004;
```

*Defined at line 85*

---

## dnp3_crc

CRC-16/DNP over `b` from `start` for `n` bytes. Reflected polynomial
0xA6BC (0x3D65), initial 0, final complement. "123456789" gives 0xEA82.

```fides
pub fn dnp3_crc(b: &Vec<i64>, start: i64, n: i64) -> i64 {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `start`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 91*

---

## dnp3_crc_all

CRC of a whole byte vector.

```fides
pub fn dnp3_crc_all(b: &Vec<i64>) -> i64 {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 111*

---

## Dnp3LinkFrame

A parsed link frame.

```fides
pub struct Dnp3LinkFrame {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `dir` | `i64,  // 0/1 master->outstation direction bit` |
| `prm` | `i64,  // 1 primary, 0 secondary` |
| `fcb` | `i64,  // frame count bit` |
| `fcv` | `i64,  // frame count valid` |
| `func` | `i64,  // link function code (low 4 bits of control)` |
| `dest` | `i64` |
| `src` | `i64` |
| `payload` | `Vec<i64>,  // transport+application bytes (CRCs stripped)` |

*Defined at line 123*

---

## dnp3_link_frame

Build a link frame carrying `payload` (<=250 bytes of transport data).
`ctrl_dir`/`prm`/`fcb`/`fcv` set the control octet bits; `func` is the
link function code.

```fides
pub fn dnp3_link_frame(dir: i64, prm: i64, fcb: i64, fcv: i64, func: i64, dest: i64, src: i64, payload: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `dir`: `i64`
- `prm`: `i64`
- `fcb`: `i64`
- `fcv`: `i64`
- `func`: `i64`
- `dest`: `i64`
- `src`: `i64`
- `payload`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 139*

---

## dnp3_link_parse

Parse a link frame, checking every CRC.

```fides
pub fn dnp3_link_parse(f: &Vec<i64>) -> Dnp3LinkFrame {
```

**Parameters:**

- `f`: `&Vec<i64>`

**Returns:** `Dnp3LinkFrame`

*Defined at line 193*

---

## dnp3_transport_segment

Segment an application fragment into transport segments: each segment is
one header byte (FIR | FIN | 6-bit sequence) followed by up to 249 bytes.
`seq0` is the sequence of the first segment.

```fides
pub fn dnp3_transport_segment(fragment: &Vec<i64>, seq0: i64) -> Vec<Vec<i64>> {
```

**Parameters:**

- `fragment`: `&Vec<i64>`
- `seq0`: `i64`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 245*

---

## Dnp3Assembly

Reassemble transport segments into the application fragment. Returns the
bytes; on a sequence or FIR/FIN error the Vec is empty and `ok` false.

```fides
pub struct Dnp3Assembly {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `fragment` | `Vec<i64>` |

*Defined at line 281*

---

## dnp3_transport_assemble

```fides
pub fn dnp3_transport_assemble(segs: &Vec<Vec<i64>>) -> Dnp3Assembly {
```

**Parameters:**

- `segs`: `&Vec<Vec<i64>>`

**Returns:** `Dnp3Assembly`

*Defined at line 286*

---

## Dnp3ObjHeader

An object header: group, variation, qualifier, and the range it covers.
For start-stop qualifiers `start`/`stop`; for count qualifiers `count`.

```fides
pub struct Dnp3ObjHeader {
```

**Fields:**

| Name | Type |
|------|------|
| `group` | `i64` |
| `variation` | `i64` |
| `qualifier` | `i64` |
| `start` | `i64` |
| `stop` | `i64` |
| `count` | `i64` |

*Defined at line 338*

---

## dnp3_objhdr_range

Encode an object header with an 8-bit or 16-bit start-stop range
(qualifier 0x00 / 0x01).

```fides
pub fn dnp3_objhdr_range(group: i64, variation: i64, start: i64, stop: i64) -> Vec<i64> {
```

**Parameters:**

- `group`: `i64`
- `variation`: `i64`
- `start`: `i64`
- `stop`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 349*

---

## dnp3_objhdr_all

Encode an object header for "all points" (qualifier 0x06).

```fides
pub fn dnp3_objhdr_all(group: i64, variation: i64) -> Vec<i64> {
```

**Parameters:**

- `group`: `i64`
- `variation`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 366*

---

## dnp3_objhdr_count

Encode an object header with an 8-bit or 16-bit count (qualifier 0x07 /
0x08).

```fides
pub fn dnp3_objhdr_count(group: i64, variation: i64, count: i64) -> Vec<i64> {
```

**Parameters:**

- `group`: `i64`
- `variation`: `i64`
- `count`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 376*

---

## dnp3_objhdr_count_prefix8

Encode an object header with an 8-bit count and 8-bit index prefixes
(qualifier 0x17), used by CROB (g12v1).

```fides
pub fn dnp3_objhdr_count_prefix8(group: i64, variation: i64, count: i64) -> Vec<i64> {
```

**Parameters:**

- `group`: `i64`
- `variation`: `i64`
- `count`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 392*

---

## Dnp3ObjHeaderAt

Decode one object header at `pos`; returns the header and the position
just past it in `next`.

```fides
pub struct Dnp3ObjHeaderAt {
```

**Fields:**

| Name | Type |
|------|------|
| `hdr` | `Dnp3ObjHeader` |
| `next` | `i64` |
| `ok` | `bool` |

*Defined at line 403*

---

## dnp3_objhdr_decode

```fides
pub fn dnp3_objhdr_decode(b: &Vec<i64>, pos: i64) -> Dnp3ObjHeaderAt {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `pos`: `i64`

**Returns:** `Dnp3ObjHeaderAt`

*Defined at line 409*

---

## dnp3_app_ctrl

Build an application control octet.

```fides
pub fn dnp3_app_ctrl(fir: bool, fin: bool, con: bool, uns: bool, seq: i64) -> i64 {
```

**Parameters:**

- `fir`: `bool`
- `fin`: `bool`
- `con`: `bool`
- `uns`: `bool`
- `seq`: `i64`

**Returns:** `i64`

*Defined at line 453*

---

## dnp3_build_read

Build a READ request application fragment reading one object group/var
over a range.

```fides
pub fn dnp3_build_read(seq: i64, group: i64, variation: i64, start: i64, stop: i64) -> Vec<i64> {
```

**Parameters:**

- `seq`: `i64`
- `group`: `i64`
- `variation`: `i64`
- `start`: `i64`
- `stop`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 472*

---

## dnp3_build_read_class0

Build a READ request for all points of a class group (Class 0 = g60v1).

```fides
pub fn dnp3_build_read_class0(seq: i64) -> Vec<i64> {
```

**Parameters:**

- `seq`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 486*

---

## Dnp3AppRequest

A decoded application request header (first two bytes of a fragment).

```fides
pub struct Dnp3AppRequest {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `fir` | `bool` |
| `fin` | `bool` |
| `con` | `bool` |
| `uns` | `bool` |
| `seq` | `i64` |
| `func` | `i64` |
| `body_start` | `i64` |

*Defined at line 500*

---

## dnp3_app_request

```fides
pub fn dnp3_app_request(b: &Vec<i64>) -> Dnp3AppRequest {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `Dnp3AppRequest`

*Defined at line 511*

---

## Dnp3AppResponse

A decoded application response header (control, function, 2 IIN bytes).

```fides
pub struct Dnp3AppResponse {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `seq` | `i64` |
| `func` | `i64` |
| `iin` | `i64` |
| `body_start` | `i64` |

*Defined at line 524*

---

## dnp3_app_response_header

```fides
pub fn dnp3_app_response_header(b: &Vec<i64>) -> Dnp3AppResponse {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `Dnp3AppResponse`

*Defined at line 532*

---

## dnp3_crob_encode

CROB (g12v1): 11 bytes. control code, count, on-time, off-time, status.

```fides
pub fn dnp3_crob_encode(index: i64, control_code: i64, count: i64, on_time: i64, off_time: i64, status: i64) -> Vec<i64> {
```

**Parameters:**

- `index`: `i64`
- `control_code`: `i64`
- `count`: `i64`
- `on_time`: `i64`
- `off_time`: `i64`
- `status`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 548*

---

## Dnp3Crob

A decoded CROB.

```fides
pub struct Dnp3Crob {
```

**Fields:**

| Name | Type |
|------|------|
| `index` | `i64` |
| `control_code` | `i64` |
| `count` | `i64` |
| `on_time` | `i64` |
| `off_time` | `i64` |
| `status` | `i64` |

*Defined at line 566*

---

## dnp3_crob_decode

Decode a CROB at `pos` (the index prefix then 11 CROB octets).

```fides
pub fn dnp3_crob_decode(b: &Vec<i64>, pos: i64) -> Dnp3Crob {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `pos`: `i64`

**Returns:** `Dnp3Crob`

*Defined at line 576*

---

## dnp3_binary_flags

g1v2 single-bit binary input with flags: one flag byte per point.
The low bit (bit 7, 0x80) is the state; bit 0 is ONLINE. We set ONLINE
plus the state.

```fides
pub fn dnp3_binary_flags(online: bool, state: bool) -> i64 {
```

**Parameters:**

- `online`: `bool`
- `state`: `bool`

**Returns:** `i64`

*Defined at line 587*

---

## dnp3_analog32_encode

g30v1 32-bit analog input with flags: flag byte + 4 little-endian bytes.

```fides
pub fn dnp3_analog32_encode(flags: i64, value: i64) -> Vec<i64> {
```

**Parameters:**

- `flags`: `i64`
- `value`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 599*

---

## dnp3_analog32_value

Decode a 32-bit signed analog value at `pos+1` (after its flag byte).

```fides
pub fn dnp3_analog32_value(b: &Vec<i64>, pos: i64) -> i64 {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `pos`: `i64`

**Returns:** `i64`

*Defined at line 610*

---

## dnp3_counter32_encode

g20v1 32-bit counter with flags: flag byte + 4 little-endian bytes.

```fides
pub fn dnp3_counter32_encode(flags: i64, value: i64) -> Vec<i64> {
```

**Parameters:**

- `flags`: `i64`
- `value`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 620*

---

## dnp3_counter32_value

```fides
pub fn dnp3_counter32_value(b: &Vec<i64>, pos: i64) -> i64 {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `pos`: `i64`

**Returns:** `i64`

*Defined at line 624*

---

## Dnp3Outstation

A simple outstation: binary inputs, binary (relay) outputs selected by
CROB, analog inputs and counters. `selected_*` tracks a pending SELECT.

```fides
pub struct Dnp3Outstation {
```

**Fields:**

| Name | Type |
|------|------|
| `binary_in` | `Vec<i64>,  // 0/1` |
| `relays` | `Vec<i64>,  // 0/1 output state` |
| `analog_in` | `Vec<i64>` |
| `counters` | `Vec<i64>` |
| `selected_index` | `i64,  // -1 if nothing selected` |
| `selected_code` | `i64` |
| `iin` | `i64` |

*Defined at line 632*

---

## dnp3_outstation_new

```fides
pub fn dnp3_outstation_new(nbin: i64, nrelay: i64, nanalog: i64, ncounter: i64) -> Dnp3Outstation {
```

**Parameters:**

- `nbin`: `i64`
- `nrelay`: `i64`
- `nanalog`: `i64`
- `ncounter`: `i64`

**Returns:** `Dnp3Outstation`

*Defined at line 642*

---

## dnp3_outstation_select

Apply a SELECT (g12v1): remember the point/code; returns the status.

```fides
pub fn dnp3_outstation_select(os: &mut Dnp3Outstation, crob: &Dnp3Crob) -> i64 {
```

**Parameters:**

- `os`: `&mut Dnp3Outstation`
- `crob`: `&Dnp3Crob`

**Returns:** `i64`

*Defined at line 649*

---

## dnp3_outstation_operate

Apply an OPERATE (g12v1): must match the prior SELECT; carries out the
latch and returns the status.

```fides
pub fn dnp3_outstation_operate(os: &mut Dnp3Outstation, crob: &Dnp3Crob) -> i64 {
```

**Parameters:**

- `os`: `&mut Dnp3Outstation`
- `crob`: `&Dnp3Crob`

**Returns:** `i64`

*Defined at line 660*

---

## dnp3_outstation_class0

Build a Class-0 response: application header (response, FIR+FIN) with the
given sequence and IIN, then g1v2 binary inputs and g30v1 analog inputs
over their full ranges.

```fides
pub fn dnp3_outstation_class0(os: &Dnp3Outstation, seq: i64, iin: i64) -> Vec<i64> {
```

**Parameters:**

- `os`: `&Dnp3Outstation`
- `seq`: `i64`
- `iin`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 678*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
