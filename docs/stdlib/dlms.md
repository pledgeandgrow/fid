[All modules](index.md)

# dlms -- Fides Documentation

## Contents

- [dlms_crc](#dlms_crc) (function)
- [ObisCode](#ObisCode) (struct)
- [dlms_obis_parse](#dlms_obis_parse) (function)
- [dlms_obis_bytes](#dlms_obis_bytes) (function)
- [dlms_obis_format](#dlms_obis_format) (function)
- [dlms_obis_name](#dlms_obis_name) (function)
- [AXDR_NULL:](#AXDR_NULL:) (const)
- [AXDR_ARRAY:](#AXDR_ARRAY:) (const)
- [AXDR_STRUCTURE:](#AXDR_STRUCTURE:) (const)
- [AXDR_BOOLEAN:](#AXDR_BOOLEAN:) (const)
- [AXDR_BIT_STRING:](#AXDR_BIT_STRING:) (const)
- [AXDR_INT32:](#AXDR_INT32:) (const)
- [AXDR_UINT32:](#AXDR_UINT32:) (const)
- [AXDR_OCTET_STRING:](#AXDR_OCTET_STRING:) (const)
- [AXDR_VISIBLE_STRING:](#AXDR_VISIBLE_STRING:) (const)
- [AXDR_INT8:](#AXDR_INT8:) (const)
- [AXDR_INT16:](#AXDR_INT16:) (const)
- [AXDR_UINT8:](#AXDR_UINT8:) (const)
- [AXDR_UINT16:](#AXDR_UINT16:) (const)
- [AXDR_ENUM:](#AXDR_ENUM:) (const)
- [AXDR_DATE_TIME:](#AXDR_DATE_TIME:) (const)
- [dlms_axdr_len](#dlms_axdr_len) (function)
- [dlms_axdr_uint8](#dlms_axdr_uint8) (function)
- [dlms_axdr_uint16](#dlms_axdr_uint16) (function)
- [dlms_axdr_uint32](#dlms_axdr_uint32) (function)
- [dlms_axdr_int32](#dlms_axdr_int32) (function)
- [dlms_axdr_enum](#dlms_axdr_enum) (function)
- [dlms_axdr_boolean](#dlms_axdr_boolean) (function)
- [dlms_axdr_octet_string](#dlms_axdr_octet_string) (function)
- [dlms_axdr_visible_string](#dlms_axdr_visible_string) (function)
- [dlms_axdr_structure](#dlms_axdr_structure) (function)
- [dlms_axdr_array](#dlms_axdr_array) (function)
- [dlms_axdr_datetime](#dlms_axdr_datetime) (function)
- [AxdrValue](#AxdrValue) (struct)
- [dlms_decode](#dlms_decode) (function)
- [dlms_hdlc_addr](#dlms_hdlc_addr) (function)
- [dlms_hdlc_frame](#dlms_hdlc_frame) (function)
- [HdlcFrame](#HdlcFrame) (struct)
- [dlms_hdlc_parse](#dlms_hdlc_parse) (function)
- [dlms_aarq](#dlms_aarq) (function)
- [dlms_aare](#dlms_aare) (function)
- [Aare](#Aare) (struct)
- [dlms_aare_parse](#dlms_aare_parse) (function)
- [dlms_get_request](#dlms_get_request) (function)
- [GetRequest](#GetRequest) (struct)
- [dlms_get_request_parse](#dlms_get_request_parse) (function)
- [dlms_get_response](#dlms_get_response) (function)
- [dlms_get_response_error](#dlms_get_response_error) (function)
- [GetResponse](#GetResponse) (struct)
- [dlms_get_response_parse](#dlms_get_response_parse) (function)

## dlms_crc

CRC-16/X-25: reflected polynomial 0x8408, initial 0xFFFF, final XOR
0xFFFF. "123456789" gives 0x906E.

```fides
pub fn dlms_crc(b: &Vec<i64>, start: i64, n: i64) -> i64 {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `start`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 38*

---

## ObisCode

A parsed OBIS code: the six value groups A..F.

```fides
pub struct ObisCode {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `a` | `i64` |
| `b` | `i64` |
| `c` | `i64` |
| `d` | `i64` |
| `e` | `i64` |
| `f` | `i64` |

*Defined at line 60*

---

## dlms_obis_parse

Parse "A-B:C.D.E.F" into its groups.

```fides
pub fn dlms_obis_parse(s: str) -> ObisCode {
```

**Parameters:**

- `s`: `str`

**Returns:** `ObisCode`

*Defined at line 71*

---

## dlms_obis_bytes

The 6 OBIS bytes.

```fides
pub fn dlms_obis_bytes(o: &ObisCode) -> Vec<i64> {
```

**Parameters:**

- `o`: `&ObisCode`

**Returns:** `Vec<i64>`

*Defined at line 94*

---

## dlms_obis_format

Format an OBIS code as "A-B:C.D.E.F".

```fides
pub fn dlms_obis_format(o: &ObisCode) -> str {
```

**Parameters:**

- `o`: `&ObisCode`

**Returns:** `str`

*Defined at line 106*

---

## dlms_obis_name

A human name for a common OBIS code, or "" when unknown.

```fides
pub fn dlms_obis_name(o: &ObisCode) -> str {
```

**Parameters:**

- `o`: `&ObisCode`

**Returns:** `str`

*Defined at line 111*

---

## AXDR_NULL:

```fides
pub const AXDR_NULL: i64 = 0;
```

*Defined at line 140*

---

## AXDR_ARRAY:

```fides
pub const AXDR_ARRAY: i64 = 1;
```

*Defined at line 141*

---

## AXDR_STRUCTURE:

```fides
pub const AXDR_STRUCTURE: i64 = 2;
```

*Defined at line 142*

---

## AXDR_BOOLEAN:

```fides
pub const AXDR_BOOLEAN: i64 = 3;
```

*Defined at line 143*

---

## AXDR_BIT_STRING:

```fides
pub const AXDR_BIT_STRING: i64 = 4;
```

*Defined at line 144*

---

## AXDR_INT32:

```fides
pub const AXDR_INT32: i64 = 5;  // double-long
```

*Defined at line 145*

---

## AXDR_UINT32:

```fides
pub const AXDR_UINT32: i64 = 6;  // double-long-unsigned
```

*Defined at line 146*

---

## AXDR_OCTET_STRING:

```fides
pub const AXDR_OCTET_STRING: i64 = 9;
```

*Defined at line 147*

---

## AXDR_VISIBLE_STRING:

```fides
pub const AXDR_VISIBLE_STRING: i64 = 10;
```

*Defined at line 148*

---

## AXDR_INT8:

```fides
pub const AXDR_INT8: i64 = 15;  // integer
```

*Defined at line 149*

---

## AXDR_INT16:

```fides
pub const AXDR_INT16: i64 = 16;  // long
```

*Defined at line 150*

---

## AXDR_UINT8:

```fides
pub const AXDR_UINT8: i64 = 17;  // unsigned
```

*Defined at line 151*

---

## AXDR_UINT16:

```fides
pub const AXDR_UINT16: i64 = 18;  // long-unsigned
```

*Defined at line 152*

---

## AXDR_ENUM:

```fides
pub const AXDR_ENUM: i64 = 22;
```

*Defined at line 153*

---

## AXDR_DATE_TIME:

```fides
pub const AXDR_DATE_TIME: i64 = 25;
```

*Defined at line 154*

---

## dlms_axdr_len

Encode an A-XDR length (short form < 128, else long form).

```fides
pub fn dlms_axdr_len(out: &mut Vec<i64>, n: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `n`: `i64`

*Defined at line 157*

---

## dlms_axdr_uint8

```fides
pub fn dlms_axdr_uint8(v: i64) -> Vec<i64> {
```

**Parameters:**

- `v`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 176*

---

## dlms_axdr_uint16

```fides
pub fn dlms_axdr_uint16(v: i64) -> Vec<i64> {
```

**Parameters:**

- `v`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 183*

---

## dlms_axdr_uint32

```fides
pub fn dlms_axdr_uint32(v: i64) -> Vec<i64> {
```

**Parameters:**

- `v`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 191*

---

## dlms_axdr_int32

```fides
pub fn dlms_axdr_int32(v: i64) -> Vec<i64> {
```

**Parameters:**

- `v`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 201*

---

## dlms_axdr_enum

```fides
pub fn dlms_axdr_enum(v: i64) -> Vec<i64> {
```

**Parameters:**

- `v`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 211*

---

## dlms_axdr_boolean

```fides
pub fn dlms_axdr_boolean(b: bool) -> Vec<i64> {
```

**Parameters:**

- `b`: `bool`

**Returns:** `Vec<i64>`

*Defined at line 218*

---

## dlms_axdr_octet_string

```fides
pub fn dlms_axdr_octet_string(bytes: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `bytes`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 229*

---

## dlms_axdr_visible_string

```fides
pub fn dlms_axdr_visible_string(s: str) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<i64>`

*Defined at line 241*

---

## dlms_axdr_structure

Encode a structure from already-encoded element byte vectors.

```fides
pub fn dlms_axdr_structure(elements: &Vec<Vec<i64>>) -> Vec<i64> {
```

**Parameters:**

- `elements`: `&Vec<Vec<i64>>`

**Returns:** `Vec<i64>`

*Defined at line 254*

---

## dlms_axdr_array

Encode an array from already-encoded element byte vectors.

```fides
pub fn dlms_axdr_array(elements: &Vec<Vec<i64>>) -> Vec<i64> {
```

**Parameters:**

- `elements`: `&Vec<Vec<i64>>`

**Returns:** `Vec<i64>`

*Defined at line 272*

---

## dlms_axdr_datetime

Encode a COSEM date-time (12 bytes): year(2), month, day, day-of-week,
hour, minute, second, hundredths, deviation(2), clock status.

```fides
pub fn dlms_axdr_datetime(year: i64, month: i64, day: i64, dow: i64, hour: i64, minute: i64, second: i64) -> Vec<i64> {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`
- `day`: `i64`
- `dow`: `i64`
- `hour`: `i64`
- `minute`: `i64`
- `second`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 291*

---

## AxdrValue

A decoded A-XDR value: its type tag, a scalar `ival` (integers/enum/
bool), bytes (octet/visible string), and the position just past it. For
structures/arrays, `count` is the element count and `content` the offset
of the first element.

```fides
pub struct AxdrValue {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `tag` | `i64` |
| `ival` | `i64` |
| `bytes` | `Vec<i64>` |
| `text` | `str` |
| `count` | `i64` |
| `content` | `i64` |
| `next` | `i64` |

*Defined at line 315*

---

## dlms_decode

Decode one A-XDR value at `pos`.

```fides
pub fn dlms_decode(b: &Vec<i64>, pos: i64) -> AxdrValue {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `pos`: `i64`

**Returns:** `AxdrValue`

*Defined at line 348*

---

## dlms_hdlc_addr

Encode an HDLC address (1, 2 or 4 bytes): each byte carries 7 bits, the
last has its low bit set. `addr` is the full address value.

```fides
pub fn dlms_hdlc_addr(out: &mut Vec<i64>, addr: i64, nbytes: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `addr`: `i64`
- `nbytes`: `i64`

*Defined at line 426*

---

## dlms_hdlc_frame

Build an HDLC type-3 information frame carrying `info`. `dest_bytes` and
`src_bytes` choose the address widths; `control` is the HDLC control
byte. The format field encodes the full frame length; HCS covers the
header, FCS the whole frame. Flags (0x7E) bracket it.

```fides
pub fn dlms_hdlc_frame(dest: i64, dest_bytes: i64, src: i64, src_bytes: i64, control: i64, info: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `dest`: `i64`
- `dest_bytes`: `i64`
- `src`: `i64`
- `src_bytes`: `i64`
- `control`: `i64`
- `info`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 444*

---

## HdlcFrame

A parsed HDLC frame.

```fides
pub struct HdlcFrame {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `frame_len` | `i64` |
| `segmentation` | `bool` |
| `control` | `i64` |
| `info` | `Vec<i64>` |

*Defined at line 493*

---

## dlms_hdlc_parse

Parse an HDLC frame, checking HCS and FCS.

```fides
pub fn dlms_hdlc_parse(b: &Vec<i64>) -> HdlcFrame {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `HdlcFrame`

*Defined at line 523*

---

## dlms_aarq

Build a minimal AARQ APDU (tag 0x60) carrying the application context
name (LN referencing, no ciphering: 2.16.756.5.8.1.1). `ctx_last` picks
the context (1 = LN, 3 = SN).

```fides
pub fn dlms_aarq(ctx_last: i64) -> Vec<i64> {
```

**Parameters:**

- `ctx_last`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 572*

---

## dlms_aare

Build a minimal AARE APDU (tag 0x61) carrying the association result
(0 accepted) and a diagnostic.

```fides
pub fn dlms_aare(result: i64, diagnostic: i64) -> Vec<i64> {
```

**Parameters:**

- `result`: `i64`
- `diagnostic`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 605*

---

## Aare

A parsed AARE result.

```fides
pub struct Aare {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `result` | `i64` |
| `diagnostic` | `i64` |

*Defined at line 633*

---

## dlms_aare_parse

```fides
pub fn dlms_aare_parse(b: &Vec<i64>) -> Aare {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `Aare`

*Defined at line 639*

---

## dlms_get_request

Build a GET.request (normal): tag 0xC0, type 0x01, invoke-id-and-priority,
then the COSEM attribute descriptor (class-id:2, instance OBIS:6,
attribute-id:1) and a null access-selection (0x00).

```fides
pub fn dlms_get_request(invoke_id: i64, class_id: i64, obis: &ObisCode, attribute_id: i64) -> Vec<i64> {
```

**Parameters:**

- `invoke_id`: `i64`
- `class_id`: `i64`
- `obis`: `&ObisCode`
- `attribute_id`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 669*

---

## GetRequest

```fides
pub struct GetRequest {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `invoke_id` | `i64` |
| `class_id` | `i64` |
| `obis` | `ObisCode` |
| `attribute_id` | `i64` |

*Defined at line 687*

---

## dlms_get_request_parse

```fides
pub fn dlms_get_request_parse(b: &Vec<i64>) -> GetRequest {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `GetRequest`

*Defined at line 695*

---

## dlms_get_response

Build a GET.response (normal) carrying data: tag 0xC4, type 0x01,
invoke-id, result choice 0 (data) then the A-XDR encoded value.

```fides
pub fn dlms_get_response(invoke_id: i64, data: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `invoke_id`: `i64`
- `data`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 713*

---

## dlms_get_response_error

Build a GET.response (normal) carrying a data-access-result error: tag
0xC4, type 0x01, invoke-id, result choice 1 (data-access-result) then the
error code.

```fides
pub fn dlms_get_response_error(invoke_id: i64, error_code: i64) -> Vec<i64> {
```

**Parameters:**

- `invoke_id`: `i64`
- `error_code`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 730*

---

## GetResponse

```fides
pub struct GetResponse {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `invoke_id` | `i64` |
| `is_data` | `bool` |
| `error_code` | `i64` |
| `data_start` | `i64,  // offset of the A-XDR value when is_data` |

*Defined at line 740*

---

## dlms_get_response_parse

```fides
pub fn dlms_get_response_parse(b: &Vec<i64>) -> GetResponse {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `GetResponse`

*Defined at line 748*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
