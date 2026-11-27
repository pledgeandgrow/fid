[All modules](index.md)

# matter -- Fides Documentation

## Contents

- [TLV_TAG_ANON:](#TLV_TAG_ANON:) (const)
- [TLV_TAG_CONTEXT:](#TLV_TAG_CONTEXT:) (const)
- [TLV_TAG_COMMON2:](#TLV_TAG_COMMON2:) (const)
- [TLV_TAG_COMMON4:](#TLV_TAG_COMMON4:) (const)
- [TLV_TAG_IMPLICIT2:](#TLV_TAG_IMPLICIT2:) (const)
- [TLV_TAG_IMPLICIT4:](#TLV_TAG_IMPLICIT4:) (const)
- [TLV_TAG_FULL6:](#TLV_TAG_FULL6:) (const)
- [TLV_TAG_FULL8:](#TLV_TAG_FULL8:) (const)
- [TLV_INT:](#TLV_INT:) (const)
- [TLV_UINT:](#TLV_UINT:) (const)
- [TLV_BOOL:](#TLV_BOOL:) (const)
- [TLV_FLOAT:](#TLV_FLOAT:) (const)
- [TLV_DOUBLE:](#TLV_DOUBLE:) (const)
- [TLV_UTF8:](#TLV_UTF8:) (const)
- [TLV_BYTES:](#TLV_BYTES:) (const)
- [TLV_NULL:](#TLV_NULL:) (const)
- [TLV_STRUCT:](#TLV_STRUCT:) (const)
- [TLV_ARRAY:](#TLV_ARRAY:) (const)
- [TLV_LIST:](#TLV_LIST:) (const)
- [TLV_END:](#TLV_END:) (const)
- [TlvTag](#TlvTag) (struct)
- [tlv_anon](#tlv_anon) (function)
- [tlv_ctx](#tlv_ctx) (function)
- [tlv_common](#tlv_common) (function)
- [tlv_implicit](#tlv_implicit) (function)
- [tlv_full](#tlv_full) (function)
- [TlvWriter](#TlvWriter) (struct)
- [tlv_writer](#tlv_writer) (function)
- [tlv_put_int](#tlv_put_int) (function)
- [tlv_put_uint](#tlv_put_uint) (function)
- [tlv_put_bool](#tlv_put_bool) (function)
- [tlv_put_null](#tlv_put_null) (function)
- [tlv_f32_bits](#tlv_f32_bits) (function)
- [tlv_f32_value](#tlv_f32_value) (function)
- [tlv_put_float](#tlv_put_float) (function)
- [tlv_put_double](#tlv_put_double) (function)
- [tlv_put_utf8](#tlv_put_utf8) (function)
- [tlv_put_bytes](#tlv_put_bytes) (function)
- [tlv_start_struct](#tlv_start_struct) (function)
- [tlv_start_array](#tlv_start_array) (function)
- [tlv_start_list](#tlv_start_list) (function)
- [tlv_end](#tlv_end) (function)
- [tlv_put_raw_tagged](#tlv_put_raw_tagged) (function)
- [TlvElem](#TlvElem) (struct)
- [tlv_decode](#tlv_decode) (function)
- [tlv_container_end](#tlv_container_end) (function)
- [tlv_member](#tlv_member) (function)
- [tlv_members](#tlv_members) (function)
- [tlv_text](#tlv_text) (function)
- [MATTER_CLUSTER_ON_OFF:](#MATTER_CLUSTER_ON_OFF:) (const)
- [MATTER_CLUSTER_LEVEL:](#MATTER_CLUSTER_LEVEL:) (const)
- [MATTER_CLUSTER_DESCRIPTOR:](#MATTER_CLUSTER_DESCRIPTOR:) (const)
- [MATTER_CLUSTER_BASIC_INFORMATION:](#MATTER_CLUSTER_BASIC_INFORMATION:) (const)
- [MATTER_CLUSTER_TEMPERATURE:](#MATTER_CLUSTER_TEMPERATURE:) (const)
- [MATTER_ATTR_CLUSTER_REVISION:](#MATTER_ATTR_CLUSTER_REVISION:) (const)
- [MATTER_ATTR_FEATURE_MAP:](#MATTER_ATTR_FEATURE_MAP:) (const)
- [MATTER_ATTR_ATTRIBUTE_LIST:](#MATTER_ATTR_ATTRIBUTE_LIST:) (const)
- [MATTER_ATTR_ACCEPTED_COMMANDS:](#MATTER_ATTR_ACCEPTED_COMMANDS:) (const)
- [MATTER_ATTR_GENERATED_COMMANDS:](#MATTER_ATTR_GENERATED_COMMANDS:) (const)
- [MATTER_STATUS_SUCCESS:](#MATTER_STATUS_SUCCESS:) (const)
- [MATTER_STATUS_UNSUPPORTED_ATTRIBUTE:](#MATTER_STATUS_UNSUPPORTED_ATTRIBUTE:) (const)
- [MATTER_STATUS_UNSUPPORTED_ENDPOINT:](#MATTER_STATUS_UNSUPPORTED_ENDPOINT:) (const)
- [MATTER_STATUS_UNSUPPORTED_CLUSTER:](#MATTER_STATUS_UNSUPPORTED_CLUSTER:) (const)
- [MATTER_STATUS_INVALID_ACTION:](#MATTER_STATUS_INVALID_ACTION:) (const)
- [MATTER_IM_REVISION:](#MATTER_IM_REVISION:) (const)
- [MATTER_PROTOCOL_IM:](#MATTER_PROTOCOL_IM:) (const)
- [MATTER_OP_STATUS_RESPONSE:](#MATTER_OP_STATUS_RESPONSE:) (const)
- [MATTER_OP_READ_REQUEST:](#MATTER_OP_READ_REQUEST:) (const)
- [MATTER_OP_REPORT_DATA:](#MATTER_OP_REPORT_DATA:) (const)
- [MatterAttr](#MatterAttr) (struct)
- [MatterClusterVersion](#MatterClusterVersion) (struct)
- [MatterNode](#MatterNode) (struct)
- [matter_node](#matter_node) (function)
- [matter_add_cluster](#matter_add_cluster) (function)
- [matter_set_attr](#matter_set_attr) (function)
- [matter_data_version](#matter_data_version) (function)
- [matter_value_bool](#matter_value_bool) (function)
- [matter_value_uint](#matter_value_uint) (function)
- [matter_value_int](#matter_value_int) (function)
- [matter_value_utf8](#matter_value_utf8) (function)
- [matter_value_null](#matter_value_null) (function)
- [MatterPath](#MatterPath) (struct)
- [matter_path](#matter_path) (function)
- [matter_read_request](#matter_read_request) (function)
- [matter_parse_read_request](#matter_parse_read_request) (function)
- [matter_node_read](#matter_node_read) (function)
- [MatterReport](#MatterReport) (struct)
- [matter_parse_report](#matter_parse_report) (function)
- [matter_protocol_header](#matter_protocol_header) (function)
- [verhoeff_check_digit](#verhoeff_check_digit) (function)
- [verhoeff_valid](#verhoeff_valid) (function)
- [MatterSetup](#MatterSetup) (struct)
- [MATTER_DISCOVERY_SOFTAP:](#MATTER_DISCOVERY_SOFTAP:) (const)
- [MATTER_DISCOVERY_BLE:](#MATTER_DISCOVERY_BLE:) (const)
- [MATTER_DISCOVERY_ON_NETWORK:](#MATTER_DISCOVERY_ON_NETWORK:) (const)
- [matter_passcode_valid](#matter_passcode_valid) (function)
- [matter_manual_code](#matter_manual_code) (function)
- [matter_parse_manual_code](#matter_parse_manual_code) (function)
- [matter_base38_encode](#matter_base38_encode) (function)
- [matter_base38_decode](#matter_base38_decode) (function)
- [matter_qr_payload](#matter_qr_payload) (function)
- [matter_parse_qr](#matter_parse_qr) (function)

## TLV_TAG_ANON:

```fides
pub const TLV_TAG_ANON: i64 = 0;
```

*Defined at line 34*

---

## TLV_TAG_CONTEXT:

```fides
pub const TLV_TAG_CONTEXT: i64 = 1;
```

*Defined at line 35*

---

## TLV_TAG_COMMON2:

```fides
pub const TLV_TAG_COMMON2: i64 = 2;
```

*Defined at line 36*

---

## TLV_TAG_COMMON4:

```fides
pub const TLV_TAG_COMMON4: i64 = 3;
```

*Defined at line 37*

---

## TLV_TAG_IMPLICIT2:

```fides
pub const TLV_TAG_IMPLICIT2: i64 = 4;
```

*Defined at line 38*

---

## TLV_TAG_IMPLICIT4:

```fides
pub const TLV_TAG_IMPLICIT4: i64 = 5;
```

*Defined at line 39*

---

## TLV_TAG_FULL6:

```fides
pub const TLV_TAG_FULL6: i64 = 6;
```

*Defined at line 40*

---

## TLV_TAG_FULL8:

```fides
pub const TLV_TAG_FULL8: i64 = 7;
```

*Defined at line 41*

---

## TLV_INT:

```fides
pub const TLV_INT: i64 = 0;
```

*Defined at line 43*

---

## TLV_UINT:

```fides
pub const TLV_UINT: i64 = 1;
```

*Defined at line 44*

---

## TLV_BOOL:

```fides
pub const TLV_BOOL: i64 = 2;
```

*Defined at line 45*

---

## TLV_FLOAT:

```fides
pub const TLV_FLOAT: i64 = 3;
```

*Defined at line 46*

---

## TLV_DOUBLE:

```fides
pub const TLV_DOUBLE: i64 = 4;
```

*Defined at line 47*

---

## TLV_UTF8:

```fides
pub const TLV_UTF8: i64 = 5;
```

*Defined at line 48*

---

## TLV_BYTES:

```fides
pub const TLV_BYTES: i64 = 6;
```

*Defined at line 49*

---

## TLV_NULL:

```fides
pub const TLV_NULL: i64 = 7;
```

*Defined at line 50*

---

## TLV_STRUCT:

```fides
pub const TLV_STRUCT: i64 = 8;
```

*Defined at line 51*

---

## TLV_ARRAY:

```fides
pub const TLV_ARRAY: i64 = 9;
```

*Defined at line 52*

---

## TLV_LIST:

```fides
pub const TLV_LIST: i64 = 10;
```

*Defined at line 53*

---

## TLV_END:

```fides
pub const TLV_END: i64 = 11;
```

*Defined at line 54*

---

## TlvTag

```fides
pub struct TlvTag {
```

**Fields:**

| Name | Type |
|------|------|
| `control` | `i64` |
| `vendor` | `i64` |
| `profile` | `i64` |
| `num` | `i64` |

*Defined at line 56*

---

## tlv_anon

```fides
pub fn tlv_anon() -> TlvTag {
```

**Returns:** `TlvTag`

*Defined at line 63*

---

## tlv_ctx

```fides
pub fn tlv_ctx(n: i64) -> TlvTag {
```

**Parameters:**

- `n`: `i64`

**Returns:** `TlvTag`

*Defined at line 69*

---

## tlv_common

A tag in the Matter common profile (2 or 4 octets as needed).

```fides
pub fn tlv_common(n: i64) -> TlvTag {
```

**Parameters:**

- `n`: `i64`

**Returns:** `TlvTag`

*Defined at line 76*

---

## tlv_implicit

```fides
pub fn tlv_implicit(n: i64) -> TlvTag {
```

**Parameters:**

- `n`: `i64`

**Returns:** `TlvTag`

*Defined at line 87*

---

## tlv_full

```fides
pub fn tlv_full(vendor: i64, profile: i64, n: i64) -> TlvTag {
```

**Parameters:**

- `vendor`: `i64`
- `profile`: `i64`
- `n`: `i64`

**Returns:** `TlvTag`

*Defined at line 98*

---

## TlvWriter

```fides
pub struct TlvWriter {
```

**Fields:**

| Name | Type |
|------|------|
| `out` | `Vec<i64>` |
| `depth` | `i64` |

*Defined at line 111*

---

## tlv_writer

```fides
pub fn tlv_writer() -> TlvWriter {
```

**Returns:** `TlvWriter`

*Defined at line 116*

---

## tlv_put_int

A signed integer in the fewest octets (1, 2, 4 or 8).

```fides
pub fn tlv_put_int(w: &mut TlvWriter, tag: TlvTag, v: i64) {
```

**Parameters:**

- `w`: `&mut TlvWriter`
- `tag`: `TlvTag`
- `v`: `i64`

*Defined at line 151*

---

## tlv_put_uint

An unsigned integer in the fewest octets (a negative i64 is taken as
its 64-bit pattern).

```fides
pub fn tlv_put_uint(w: &mut TlvWriter, tag: TlvTag, v: i64) {
```

**Parameters:**

- `w`: `&mut TlvWriter`
- `tag`: `TlvTag`
- `v`: `i64`

*Defined at line 169*

---

## tlv_put_bool

```fides
pub fn tlv_put_bool(w: &mut TlvWriter, tag: TlvTag, b: bool) {
```

**Parameters:**

- `w`: `&mut TlvWriter`
- `tag`: `TlvTag`
- `b`: `bool`

*Defined at line 185*

---

## tlv_put_null

```fides
pub fn tlv_put_null(w: &mut TlvWriter, tag: TlvTag) {
```

**Parameters:**

- `w`: `&mut TlvWriter`
- `tag`: `TlvTag`

*Defined at line 193*

---

## tlv_f32_bits

IEEE-754 single bits of an f64, round to nearest even.

```fides
pub fn tlv_f32_bits(x: f64) -> i64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `i64`

*Defined at line 198*

---

## tlv_f32_value

```fides
pub fn tlv_f32_value(b: i64) -> f64 {
```

**Parameters:**

- `b`: `i64`

**Returns:** `f64`

*Defined at line 241*

---

## tlv_put_float

```fides
pub fn tlv_put_float(w: &mut TlvWriter, tag: TlvTag, x: f64) {
```

**Parameters:**

- `w`: `&mut TlvWriter`
- `tag`: `TlvTag`
- `x`: `f64`

*Defined at line 266*

---

## tlv_put_double

```fides
pub fn tlv_put_double(w: &mut TlvWriter, tag: TlvTag, x: f64) {
```

**Parameters:**

- `w`: `&mut TlvWriter`
- `tag`: `TlvTag`
- `x`: `f64`

*Defined at line 271*

---

## tlv_put_utf8

```fides
pub fn tlv_put_utf8(w: &mut TlvWriter, tag: TlvTag, s: str) {
```

**Parameters:**

- `w`: `&mut TlvWriter`
- `tag`: `TlvTag`
- `s`: `str`

*Defined at line 295*

---

## tlv_put_bytes

```fides
pub fn tlv_put_bytes(w: &mut TlvWriter, tag: TlvTag, b: &Vec<i64>) {
```

**Parameters:**

- `w`: `&mut TlvWriter`
- `tag`: `TlvTag`
- `b`: `&Vec<i64>`

*Defined at line 299*

---

## tlv_start_struct

```fides
pub fn tlv_start_struct(w: &mut TlvWriter, tag: TlvTag) {
```

**Parameters:**

- `w`: `&mut TlvWriter`
- `tag`: `TlvTag`

*Defined at line 303*

---

## tlv_start_array

```fides
pub fn tlv_start_array(w: &mut TlvWriter, tag: TlvTag) {
```

**Parameters:**

- `w`: `&mut TlvWriter`
- `tag`: `TlvTag`

*Defined at line 308*

---

## tlv_start_list

```fides
pub fn tlv_start_list(w: &mut TlvWriter, tag: TlvTag) {
```

**Parameters:**

- `w`: `&mut TlvWriter`
- `tag`: `TlvTag`

*Defined at line 313*

---

## tlv_end

```fides
pub fn tlv_end(w: &mut TlvWriter) {
```

**Parameters:**

- `w`: `&mut TlvWriter`

*Defined at line 318*

---

## tlv_put_raw_tagged

Copy an already encoded element, giving it a new tag (the element
must start with an anonymous tag: its control byte's low 5 bits keep
the type).

```fides
pub fn tlv_put_raw_tagged(w: &mut TlvWriter, tag: TlvTag, anon_elem: &Vec<i64>) {
```

**Parameters:**

- `w`: `&mut TlvWriter`
- `tag`: `TlvTag`
- `anon_elem`: `&Vec<i64>`

*Defined at line 326*

---

## TlvElem

One decoded element. Containers are followed by their members (depth
+ 1) and an END element at the container's depth.

```fides
pub struct TlvElem {
```

**Fields:**

| Name | Type |
|------|------|
| `tag` | `TlvTag` |
| `kind` | `i64` |
| `ival` | `i64` |
| `fval` | `f64` |
| `bytes` | `Vec<i64>` |
| `depth` | `i64` |
| `width` | `i64` |

*Defined at line 342*

---

## tlv_decode

Decode a TLV byte stream; Err on truncation, a reserved type or an
unbalanced end of container.

```fides
pub fn tlv_decode(b: &Vec<i64>) -> Result<Vec<TlvElem>, str> {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `Result<Vec<TlvElem>, str>`

*Defined at line 364*

---

## tlv_container_end

The index of the END that closes the container at i (i itself for a
non-container).

```fides
pub fn tlv_container_end(es: &Vec<TlvElem>, i: i64) -> i64 {
```

**Parameters:**

- `es`: `&Vec<TlvElem>`
- `i`: `i64`

**Returns:** `i64`

*Defined at line 483*

---

## tlv_member

The direct member of the container at i with context tag n, or -1.

```fides
pub fn tlv_member(es: &Vec<TlvElem>, i: i64, n: i64) -> i64 {
```

**Parameters:**

- `es`: `&Vec<TlvElem>`
- `i`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 500*

---

## tlv_members

The direct members of the container at i.

```fides
pub fn tlv_members(es: &Vec<TlvElem>, i: i64) -> Vec<i64> {
```

**Parameters:**

- `es`: `&Vec<TlvElem>`
- `i`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 514*

---

## tlv_text

The text form used by the spec's examples: {0 = 42, 1 = -17},
[1, 2], [[...]] for lists, 42U for unsigned, "text", hex:0102, null.

```fides
pub fn tlv_text(es: &Vec<TlvElem>) -> str {
```

**Parameters:**

- `es`: `&Vec<TlvElem>`

**Returns:** `str`

*Defined at line 556*

---

## MATTER_CLUSTER_ON_OFF:

```fides
pub const MATTER_CLUSTER_ON_OFF: i64 = 0x0006;
```

*Defined at line 627*

---

## MATTER_CLUSTER_LEVEL:

```fides
pub const MATTER_CLUSTER_LEVEL: i64 = 0x0008;
```

*Defined at line 628*

---

## MATTER_CLUSTER_DESCRIPTOR:

```fides
pub const MATTER_CLUSTER_DESCRIPTOR: i64 = 0x001D;
```

*Defined at line 629*

---

## MATTER_CLUSTER_BASIC_INFORMATION:

```fides
pub const MATTER_CLUSTER_BASIC_INFORMATION: i64 = 0x0028;
```

*Defined at line 630*

---

## MATTER_CLUSTER_TEMPERATURE:

```fides
pub const MATTER_CLUSTER_TEMPERATURE: i64 = 0x0402;
```

*Defined at line 631*

---

## MATTER_ATTR_CLUSTER_REVISION:

```fides
pub const MATTER_ATTR_CLUSTER_REVISION: i64 = 0xFFFD;
```

*Defined at line 632*

---

## MATTER_ATTR_FEATURE_MAP:

```fides
pub const MATTER_ATTR_FEATURE_MAP: i64 = 0xFFFC;
```

*Defined at line 633*

---

## MATTER_ATTR_ATTRIBUTE_LIST:

```fides
pub const MATTER_ATTR_ATTRIBUTE_LIST: i64 = 0xFFFB;
```

*Defined at line 634*

---

## MATTER_ATTR_ACCEPTED_COMMANDS:

```fides
pub const MATTER_ATTR_ACCEPTED_COMMANDS: i64 = 0xFFF9;
```

*Defined at line 635*

---

## MATTER_ATTR_GENERATED_COMMANDS:

```fides
pub const MATTER_ATTR_GENERATED_COMMANDS: i64 = 0xFFF8;
```

*Defined at line 636*

---

## MATTER_STATUS_SUCCESS:

```fides
pub const MATTER_STATUS_SUCCESS: i64 = 0x00;
```

*Defined at line 637*

---

## MATTER_STATUS_UNSUPPORTED_ATTRIBUTE:

```fides
pub const MATTER_STATUS_UNSUPPORTED_ATTRIBUTE: i64 = 0x86;
```

*Defined at line 638*

---

## MATTER_STATUS_UNSUPPORTED_ENDPOINT:

```fides
pub const MATTER_STATUS_UNSUPPORTED_ENDPOINT: i64 = 0x7F;
```

*Defined at line 639*

---

## MATTER_STATUS_UNSUPPORTED_CLUSTER:

```fides
pub const MATTER_STATUS_UNSUPPORTED_CLUSTER: i64 = 0xC3;
```

*Defined at line 640*

---

## MATTER_STATUS_INVALID_ACTION:

```fides
pub const MATTER_STATUS_INVALID_ACTION: i64 = 0x80;
```

*Defined at line 641*

---

## MATTER_IM_REVISION:

```fides
pub const MATTER_IM_REVISION: i64 = 11;
```

*Defined at line 642*

---

## MATTER_PROTOCOL_IM:

```fides
pub const MATTER_PROTOCOL_IM: i64 = 0x0001;
```

*Defined at line 643*

---

## MATTER_OP_STATUS_RESPONSE:

```fides
pub const MATTER_OP_STATUS_RESPONSE: i64 = 0x01;
```

*Defined at line 644*

---

## MATTER_OP_READ_REQUEST:

```fides
pub const MATTER_OP_READ_REQUEST: i64 = 0x02;
```

*Defined at line 645*

---

## MATTER_OP_REPORT_DATA:

```fides
pub const MATTER_OP_REPORT_DATA: i64 = 0x05;
```

*Defined at line 646*

---

## MatterAttr

A stored attribute: its value as an anonymous TLV element.

```fides
pub struct MatterAttr {
```

**Fields:**

| Name | Type |
|------|------|
| `endpoint` | `i64` |
| `cluster` | `i64` |
| `attribute` | `i64` |
| `value` | `Vec<i64>` |

*Defined at line 649*

---

## MatterClusterVersion

```fides
pub struct MatterClusterVersion {
```

**Fields:**

| Name | Type |
|------|------|
| `endpoint` | `i64` |
| `cluster` | `i64` |
| `version` | `i64` |

*Defined at line 656*

---

## MatterNode

```fides
pub struct MatterNode {
```

**Fields:**

| Name | Type |
|------|------|
| `node_id` | `i64` |
| `attrs` | `Vec<MatterAttr>` |
| `versions` | `Vec<MatterClusterVersion>` |

*Defined at line 662*

---

## matter_node

```fides
pub fn matter_node(node_id: i64) -> MatterNode {
```

**Parameters:**

- `node_id`: `i64`

**Returns:** `MatterNode`

*Defined at line 668*

---

## matter_add_cluster

Add a cluster to an endpoint with its starting data version.

```fides
pub fn matter_add_cluster(n: &mut MatterNode, ep: i64, cl: i64, version: i64) {
```

**Parameters:**

- `n`: `&mut MatterNode`
- `ep`: `i64`
- `cl`: `i64`
- `version`: `i64`

*Defined at line 687*

---

## matter_set_attr

Set (or add) an attribute value (one anonymous TLV element); the
cluster's data version goes up by one when the value changes.

```fides
pub fn matter_set_attr(n: &mut MatterNode, ep: i64, cl: i64, attr: i64, value: Vec<i64>) {
```

**Parameters:**

- `n`: `&mut MatterNode`
- `ep`: `i64`
- `cl`: `i64`
- `attr`: `i64`
- `value`: `Vec<i64>`

*Defined at line 697*

---

## matter_data_version

```fides
pub fn matter_data_version(n: &MatterNode, ep: i64, cl: i64) -> i64 {
```

**Parameters:**

- `n`: `&MatterNode`
- `ep`: `i64`
- `cl`: `i64`

**Returns:** `i64`

*Defined at line 721*

---

## matter_value_bool

Anonymous TLV encodings of plain values, for matter_set_attr.

```fides
pub fn matter_value_bool(b: bool) -> Vec<i64> {
```

**Parameters:**

- `b`: `bool`

**Returns:** `Vec<i64>`

*Defined at line 730*

---

## matter_value_uint

```fides
pub fn matter_value_uint(v: i64) -> Vec<i64> {
```

**Parameters:**

- `v`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 736*

---

## matter_value_int

```fides
pub fn matter_value_int(v: i64) -> Vec<i64> {
```

**Parameters:**

- `v`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 742*

---

## matter_value_utf8

```fides
pub fn matter_value_utf8(s: str) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<i64>`

*Defined at line 748*

---

## matter_value_null

```fides
pub fn matter_value_null() -> Vec<i64> {
```

**Returns:** `Vec<i64>`

*Defined at line 754*

---

## MatterPath

An attribute path; -1 in a field is a wildcard.

```fides
pub struct MatterPath {
```

**Fields:**

| Name | Type |
|------|------|
| `endpoint` | `i64` |
| `cluster` | `i64` |
| `attribute` | `i64` |

*Defined at line 763*

---

## matter_path

```fides
pub fn matter_path(ep: i64, cl: i64, attr: i64) -> MatterPath {
```

**Parameters:**

- `ep`: `i64`
- `cl`: `i64`
- `attr`: `i64`

**Returns:** `MatterPath`

*Defined at line 769*

---

## matter_read_request

ReadRequestMessage: {0 = [paths...], 3 = fabric_filtered, 255 = rev}.

```fides
pub fn matter_read_request(paths: &Vec<MatterPath>, fabric_filtered: bool) -> Vec<i64> {
```

**Parameters:**

- `paths`: `&Vec<MatterPath>`
- `fabric_filtered`: `bool`

**Returns:** `Vec<i64>`

*Defined at line 808*

---

## matter_parse_read_request

The attribute paths of a ReadRequestMessage.

```fides
pub fn matter_parse_read_request(b: &Vec<i64>) -> Result<Vec<MatterPath>, str> {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `Result<Vec<MatterPath>, str>`

*Defined at line 826*

---

## matter_node_read

Answer a ReadRequestMessage with a ReportDataMessage: {1 = [reports],
4 = true (SuppressResponse), 255 = rev}. Wildcards expand over what the
node has; a concrete path that does not exist gives an AttributeStatusIB.

```fides
pub fn matter_node_read(n: &MatterNode, request: &Vec<i64>) -> Result<Vec<i64>, str> {
```

**Parameters:**

- `n`: `&MatterNode`
- `request`: `&Vec<i64>`

**Returns:** `Result<Vec<i64>, str>`

*Defined at line 885*

---

## MatterReport

One entry of a ReportDataMessage: data (status -1, version, value in
text form) or a status.

```fides
pub struct MatterReport {
```

**Fields:**

| Name | Type |
|------|------|
| `path` | `MatterPath` |
| `version` | `i64` |
| `status` | `i64` |
| `value_text` | `str` |

*Defined at line 931*

---

## matter_parse_report

```fides
pub fn matter_parse_report(b: &Vec<i64>) -> Result<Vec<MatterReport>, str> {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `Result<Vec<MatterReport>, str>`

*Defined at line 938*

---

## matter_protocol_header

The protocol header in front of an IM payload: exchange flags
(0x01 initiator, 0x02 ack present, 0x04 reliability wanted), opcode,
exchange id, protocol id (IM = 0x0001), then the acknowledged message
counter when ack_counter >= 0.

```fides
pub fn matter_protocol_header(opcode: i64, exchange_id: i64, initiator: bool, reliable: bool, ack_counter: i64) -> Vec<i64> {
```

**Parameters:**

- `opcode`: `i64`
- `exchange_id`: `i64`
- `initiator`: `bool`
- `reliable`: `bool`
- `ack_counter`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1019*

---

## verhoeff_check_digit

The Verhoeff check digit of a decimal string.

```fides
pub fn verhoeff_check_digit(digits: str) -> i64 {
```

**Parameters:**

- `digits`: `str`

**Returns:** `i64`

*Defined at line 1060*

---

## verhoeff_valid

Whether a decimal string ends with its correct Verhoeff check digit.

```fides
pub fn verhoeff_valid(digits: str) -> bool {
```

**Parameters:**

- `digits`: `str`

**Returns:** `bool`

*Defined at line 1073*

---

## MatterSetup

```fides
pub struct MatterSetup {
```

**Fields:**

| Name | Type |
|------|------|
| `version` | `i64` |
| `vendor_id` | `i64` |
| `product_id` | `i64` |
| `flow` | `i64` |
| `discovery` | `i64` |
| `discriminator` | `i64` |
| `passcode` | `i64` |
| `ok` | `bool` |

*Defined at line 1096*

---

## MATTER_DISCOVERY_SOFTAP:

```fides
pub const MATTER_DISCOVERY_SOFTAP: i64 = 0x01;
```

*Defined at line 1107*

---

## MATTER_DISCOVERY_BLE:

```fides
pub const MATTER_DISCOVERY_BLE: i64 = 0x02;
```

*Defined at line 1108*

---

## MATTER_DISCOVERY_ON_NETWORK:

```fides
pub const MATTER_DISCOVERY_ON_NETWORK: i64 = 0x04;
```

*Defined at line 1109*

---

## matter_passcode_valid

Passcodes the spec forbids (trivial ones and out of range).

```fides
pub fn matter_passcode_valid(pc: i64) -> bool {
```

**Parameters:**

- `pc`: `i64`

**Returns:** `bool`

*Defined at line 1112*

---

## matter_manual_code

The manual pairing code: 11 digits, or 21 with vendor and product id
(custom flow); the short discriminator is the top 4 of the 12 bits.

```fides
pub fn matter_manual_code(s: &MatterSetup, with_vid_pid: bool) -> str {
```

**Parameters:**

- `s`: `&MatterSetup`
- `with_vid_pid`: `bool`

**Returns:** `str`

*Defined at line 1124*

---

## matter_parse_manual_code

Decode a manual code (dashes and spaces allowed): passcode, the short
discriminator (as discriminator << 8) and vendor / product ids.

```fides
pub fn matter_parse_manual_code(text: str) -> MatterSetup {
```

**Parameters:**

- `text`: `str`

**Returns:** `MatterSetup`

*Defined at line 1142*

---

## matter_base38_encode

Base38 (Matter 5.1.3.1): 3 bytes -> 5 characters, 2 -> 4, 1 -> 2,
least significant first.

```fides
pub fn matter_base38_encode(b: &Vec<i64>) -> str {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `str`

*Defined at line 1184*

---

## matter_base38_decode

```fides
pub fn matter_base38_decode(s: str) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<i64>`

*Defined at line 1216*

---

## matter_qr_payload

The QR code payload: "MT:" + Base38 of the 88-bit packing (LSB first)
version(3) vendor(16) product(16) flow(2) discovery(8)
discriminator(12) passcode(27) padding(4).

```fides
pub fn matter_qr_payload(s: &MatterSetup) -> str {
```

**Parameters:**

- `s`: `&MatterSetup`

**Returns:** `str`

*Defined at line 1265*

---

## matter_parse_qr

Decode "MT:..." back into its fields (ok=false when malformed).

```fides
pub fn matter_parse_qr(text: str) -> MatterSetup {
```

**Parameters:**

- `text`: `str`

**Returns:** `MatterSetup`

*Defined at line 1305*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
