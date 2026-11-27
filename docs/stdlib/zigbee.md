[All modules](index.md)

# zigbee -- Fides Documentation

## Contents

- [ZCL_FRAME_GLOBAL:](#ZCL_FRAME_GLOBAL:) (const)
- [ZCL_FRAME_CLUSTER:](#ZCL_FRAME_CLUSTER:) (const)
- [ZCL_DIR_TO_SERVER:](#ZCL_DIR_TO_SERVER:) (const)
- [ZCL_DIR_TO_CLIENT:](#ZCL_DIR_TO_CLIENT:) (const)
- [ZCL_READ_ATTRIBUTES:](#ZCL_READ_ATTRIBUTES:) (const)
- [ZCL_READ_ATTRIBUTES_RSP:](#ZCL_READ_ATTRIBUTES_RSP:) (const)
- [ZCL_WRITE_ATTRIBUTES:](#ZCL_WRITE_ATTRIBUTES:) (const)
- [ZCL_WRITE_ATTRIBUTES_UNDIVIDED:](#ZCL_WRITE_ATTRIBUTES_UNDIVIDED:) (const)
- [ZCL_WRITE_ATTRIBUTES_RSP:](#ZCL_WRITE_ATTRIBUTES_RSP:) (const)
- [ZCL_WRITE_ATTRIBUTES_NO_RSP:](#ZCL_WRITE_ATTRIBUTES_NO_RSP:) (const)
- [ZCL_CONFIGURE_REPORTING:](#ZCL_CONFIGURE_REPORTING:) (const)
- [ZCL_CONFIGURE_REPORTING_RSP:](#ZCL_CONFIGURE_REPORTING_RSP:) (const)
- [ZCL_REPORT_ATTRIBUTES:](#ZCL_REPORT_ATTRIBUTES:) (const)
- [ZCL_DEFAULT_RSP:](#ZCL_DEFAULT_RSP:) (const)
- [ZCL_SUCCESS:](#ZCL_SUCCESS:) (const)
- [ZCL_FAILURE:](#ZCL_FAILURE:) (const)
- [ZCL_MALFORMED_COMMAND:](#ZCL_MALFORMED_COMMAND:) (const)
- [ZCL_UNSUP_COMMAND:](#ZCL_UNSUP_COMMAND:) (const)
- [ZCL_UNSUP_GENERAL_COMMAND:](#ZCL_UNSUP_GENERAL_COMMAND:) (const)
- [ZCL_UNSUPPORTED_ATTRIBUTE:](#ZCL_UNSUPPORTED_ATTRIBUTE:) (const)
- [ZCL_INVALID_VALUE:](#ZCL_INVALID_VALUE:) (const)
- [ZCL_READ_ONLY:](#ZCL_READ_ONLY:) (const)
- [ZCL_INVALID_DATA_TYPE:](#ZCL_INVALID_DATA_TYPE:) (const)
- [ZCL_NODATA:](#ZCL_NODATA:) (const)
- [ZCL_DATA8:](#ZCL_DATA8:) (const)
- [ZCL_BOOL:](#ZCL_BOOL:) (const)
- [ZCL_MAP8:](#ZCL_MAP8:) (const)
- [ZCL_MAP16:](#ZCL_MAP16:) (const)
- [ZCL_UINT8:](#ZCL_UINT8:) (const)
- [ZCL_UINT16:](#ZCL_UINT16:) (const)
- [ZCL_UINT24:](#ZCL_UINT24:) (const)
- [ZCL_UINT32:](#ZCL_UINT32:) (const)
- [ZCL_UINT48:](#ZCL_UINT48:) (const)
- [ZCL_UINT64:](#ZCL_UINT64:) (const)
- [ZCL_INT8:](#ZCL_INT8:) (const)
- [ZCL_INT16:](#ZCL_INT16:) (const)
- [ZCL_INT24:](#ZCL_INT24:) (const)
- [ZCL_INT32:](#ZCL_INT32:) (const)
- [ZCL_INT64:](#ZCL_INT64:) (const)
- [ZCL_ENUM8:](#ZCL_ENUM8:) (const)
- [ZCL_ENUM16:](#ZCL_ENUM16:) (const)
- [ZCL_SINGLE:](#ZCL_SINGLE:) (const)
- [ZCL_OCTSTR:](#ZCL_OCTSTR:) (const)
- [ZCL_STRING:](#ZCL_STRING:) (const)
- [ZCL_UTC:](#ZCL_UTC:) (const)
- [ZCL_CLUSTER_ID:](#ZCL_CLUSTER_ID:) (const)
- [ZCL_ATTRIB_ID:](#ZCL_ATTRIB_ID:) (const)
- [ZCL_IEEE_ADDR:](#ZCL_IEEE_ADDR:) (const)
- [ZCL_CLUSTER_BASIC:](#ZCL_CLUSTER_BASIC:) (const)
- [ZCL_CLUSTER_ON_OFF:](#ZCL_CLUSTER_ON_OFF:) (const)
- [ZCL_CLUSTER_LEVEL:](#ZCL_CLUSTER_LEVEL:) (const)
- [ZCL_CLUSTER_TEMPERATURE:](#ZCL_CLUSTER_TEMPERATURE:) (const)
- [ZCL_ATTR_ON_OFF:](#ZCL_ATTR_ON_OFF:) (const)
- [ZCL_ATTR_CURRENT_LEVEL:](#ZCL_ATTR_CURRENT_LEVEL:) (const)
- [ZCL_ATTR_MEASURED_VALUE:](#ZCL_ATTR_MEASURED_VALUE:) (const)
- [ZCL_ATTR_MIN_MEASURED_VALUE:](#ZCL_ATTR_MIN_MEASURED_VALUE:) (const)
- [ZCL_ATTR_MAX_MEASURED_VALUE:](#ZCL_ATTR_MAX_MEASURED_VALUE:) (const)
- [ZCL_CMD_OFF:](#ZCL_CMD_OFF:) (const)
- [ZCL_CMD_ON:](#ZCL_CMD_ON:) (const)
- [ZCL_CMD_TOGGLE:](#ZCL_CMD_TOGGLE:) (const)
- [ZCL_CMD_MOVE_TO_LEVEL:](#ZCL_CMD_MOVE_TO_LEVEL:) (const)
- [ZCL_CMD_MOVE_TO_LEVEL_ON_OFF:](#ZCL_CMD_MOVE_TO_LEVEL_ON_OFF:) (const)
- [ZCL_TEMPERATURE_INVALID:](#ZCL_TEMPERATURE_INVALID:) (const)
- [ZIGBEE_PROFILE_HA:](#ZIGBEE_PROFILE_HA:) (const)
- [ZclFrame](#ZclFrame) (struct)
- [zcl_frame](#zcl_frame) (function)
- [zcl_encode](#zcl_encode) (function)
- [zcl_decode](#zcl_decode) (function)
- [ZclValue](#ZclValue) (struct)
- [zcl_num](#zcl_num) (function)
- [zcl_bool](#zcl_bool) (function)
- [zcl_string](#zcl_string) (function)
- [zcl_type_size](#zcl_type_size) (function)
- [zcl_type_signed](#zcl_type_signed) (function)
- [zcl_invalid_value](#zcl_invalid_value) (function)
- [zcl_put_value](#zcl_put_value) (function)
- [zcl_get_value](#zcl_get_value) (function)
- [zcl_f32_bits](#zcl_f32_bits) (function)
- [zcl_f32_from_bits](#zcl_f32_from_bits) (function)
- [zcl_single](#zcl_single) (function)
- [zcl_read_attributes](#zcl_read_attributes) (function)
- [ZclAttrRecord](#ZclAttrRecord) (struct)
- [zcl_write_attributes](#zcl_write_attributes) (function)
- [zcl_report_attributes](#zcl_report_attributes) (function)
- [zcl_decode_attr_values](#zcl_decode_attr_values) (function)
- [zcl_decode_read_rsp](#zcl_decode_read_rsp) (function)
- [zcl_default_response](#zcl_default_response) (function)
- [zcl_configure_reporting](#zcl_configure_reporting) (function)
- [zcl_on_off_command](#zcl_on_off_command) (function)
- [zcl_move_to_level](#zcl_move_to_level) (function)
- [zcl_temperature_value](#zcl_temperature_value) (function)
- [ZclAttr](#ZclAttr) (struct)
- [ZclEndpoint](#ZclEndpoint) (struct)
- [zcl_endpoint_light_sensor](#zcl_endpoint_light_sensor) (function)
- [zcl_get](#zcl_get) (function)
- [zcl_set](#zcl_set) (function)
- [zcl_report_of](#zcl_report_of) (function)
- [zcl_endpoint_handle](#zcl_endpoint_handle) (function)
- [APS_FRAME_DATA:](#APS_FRAME_DATA:) (const)
- [APS_FRAME_COMMAND:](#APS_FRAME_COMMAND:) (const)
- [APS_FRAME_ACK:](#APS_FRAME_ACK:) (const)
- [APS_DELIVERY_UNICAST:](#APS_DELIVERY_UNICAST:) (const)
- [APS_DELIVERY_BROADCAST:](#APS_DELIVERY_BROADCAST:) (const)
- [APS_DELIVERY_GROUP:](#APS_DELIVERY_GROUP:) (const)
- [ApsHeader](#ApsHeader) (struct)
- [aps_unicast](#aps_unicast) (function)
- [aps_group](#aps_group) (function)
- [aps_encode](#aps_encode) (function)
- [aps_decode](#aps_decode) (function)
- [aps_payload](#aps_payload) (function)
- [ZigbeeRadio](#ZigbeeRadio) (struct)
- [zigbee_send_zcl](#zigbee_send_zcl) (function)

## ZCL_FRAME_GLOBAL:

```fides
pub const ZCL_FRAME_GLOBAL: i64 = 0;
```

*Defined at line 29*

---

## ZCL_FRAME_CLUSTER:

```fides
pub const ZCL_FRAME_CLUSTER: i64 = 1;
```

*Defined at line 30*

---

## ZCL_DIR_TO_SERVER:

```fides
pub const ZCL_DIR_TO_SERVER: i64 = 0;
```

*Defined at line 31*

---

## ZCL_DIR_TO_CLIENT:

```fides
pub const ZCL_DIR_TO_CLIENT: i64 = 1;
```

*Defined at line 32*

---

## ZCL_READ_ATTRIBUTES:

```fides
pub const ZCL_READ_ATTRIBUTES: i64 = 0x00;
```

*Defined at line 35*

---

## ZCL_READ_ATTRIBUTES_RSP:

```fides
pub const ZCL_READ_ATTRIBUTES_RSP: i64 = 0x01;
```

*Defined at line 36*

---

## ZCL_WRITE_ATTRIBUTES:

```fides
pub const ZCL_WRITE_ATTRIBUTES: i64 = 0x02;
```

*Defined at line 37*

---

## ZCL_WRITE_ATTRIBUTES_UNDIVIDED:

```fides
pub const ZCL_WRITE_ATTRIBUTES_UNDIVIDED: i64 = 0x03;
```

*Defined at line 38*

---

## ZCL_WRITE_ATTRIBUTES_RSP:

```fides
pub const ZCL_WRITE_ATTRIBUTES_RSP: i64 = 0x04;
```

*Defined at line 39*

---

## ZCL_WRITE_ATTRIBUTES_NO_RSP:

```fides
pub const ZCL_WRITE_ATTRIBUTES_NO_RSP: i64 = 0x05;
```

*Defined at line 40*

---

## ZCL_CONFIGURE_REPORTING:

```fides
pub const ZCL_CONFIGURE_REPORTING: i64 = 0x06;
```

*Defined at line 41*

---

## ZCL_CONFIGURE_REPORTING_RSP:

```fides
pub const ZCL_CONFIGURE_REPORTING_RSP: i64 = 0x07;
```

*Defined at line 42*

---

## ZCL_REPORT_ATTRIBUTES:

```fides
pub const ZCL_REPORT_ATTRIBUTES: i64 = 0x0A;
```

*Defined at line 43*

---

## ZCL_DEFAULT_RSP:

```fides
pub const ZCL_DEFAULT_RSP: i64 = 0x0B;
```

*Defined at line 44*

---

## ZCL_SUCCESS:

```fides
pub const ZCL_SUCCESS: i64 = 0x00;
```

*Defined at line 47*

---

## ZCL_FAILURE:

```fides
pub const ZCL_FAILURE: i64 = 0x01;
```

*Defined at line 48*

---

## ZCL_MALFORMED_COMMAND:

```fides
pub const ZCL_MALFORMED_COMMAND: i64 = 0x80;
```

*Defined at line 49*

---

## ZCL_UNSUP_COMMAND:

```fides
pub const ZCL_UNSUP_COMMAND: i64 = 0x81;
```

*Defined at line 50*

---

## ZCL_UNSUP_GENERAL_COMMAND:

```fides
pub const ZCL_UNSUP_GENERAL_COMMAND: i64 = 0x82;
```

*Defined at line 51*

---

## ZCL_UNSUPPORTED_ATTRIBUTE:

```fides
pub const ZCL_UNSUPPORTED_ATTRIBUTE: i64 = 0x86;
```

*Defined at line 52*

---

## ZCL_INVALID_VALUE:

```fides
pub const ZCL_INVALID_VALUE: i64 = 0x87;
```

*Defined at line 53*

---

## ZCL_READ_ONLY:

```fides
pub const ZCL_READ_ONLY: i64 = 0x88;
```

*Defined at line 54*

---

## ZCL_INVALID_DATA_TYPE:

```fides
pub const ZCL_INVALID_DATA_TYPE: i64 = 0x8D;
```

*Defined at line 55*

---

## ZCL_NODATA:

```fides
pub const ZCL_NODATA: i64 = 0x00;
```

*Defined at line 58*

---

## ZCL_DATA8:

```fides
pub const ZCL_DATA8: i64 = 0x08;
```

*Defined at line 59*

---

## ZCL_BOOL:

```fides
pub const ZCL_BOOL: i64 = 0x10;
```

*Defined at line 60*

---

## ZCL_MAP8:

```fides
pub const ZCL_MAP8: i64 = 0x18;
```

*Defined at line 61*

---

## ZCL_MAP16:

```fides
pub const ZCL_MAP16: i64 = 0x19;
```

*Defined at line 62*

---

## ZCL_UINT8:

```fides
pub const ZCL_UINT8: i64 = 0x20;
```

*Defined at line 63*

---

## ZCL_UINT16:

```fides
pub const ZCL_UINT16: i64 = 0x21;
```

*Defined at line 64*

---

## ZCL_UINT24:

```fides
pub const ZCL_UINT24: i64 = 0x22;
```

*Defined at line 65*

---

## ZCL_UINT32:

```fides
pub const ZCL_UINT32: i64 = 0x23;
```

*Defined at line 66*

---

## ZCL_UINT48:

```fides
pub const ZCL_UINT48: i64 = 0x25;
```

*Defined at line 67*

---

## ZCL_UINT64:

```fides
pub const ZCL_UINT64: i64 = 0x27;
```

*Defined at line 68*

---

## ZCL_INT8:

```fides
pub const ZCL_INT8: i64 = 0x28;
```

*Defined at line 69*

---

## ZCL_INT16:

```fides
pub const ZCL_INT16: i64 = 0x29;
```

*Defined at line 70*

---

## ZCL_INT24:

```fides
pub const ZCL_INT24: i64 = 0x2A;
```

*Defined at line 71*

---

## ZCL_INT32:

```fides
pub const ZCL_INT32: i64 = 0x2B;
```

*Defined at line 72*

---

## ZCL_INT64:

```fides
pub const ZCL_INT64: i64 = 0x2F;
```

*Defined at line 73*

---

## ZCL_ENUM8:

```fides
pub const ZCL_ENUM8: i64 = 0x30;
```

*Defined at line 74*

---

## ZCL_ENUM16:

```fides
pub const ZCL_ENUM16: i64 = 0x31;
```

*Defined at line 75*

---

## ZCL_SINGLE:

```fides
pub const ZCL_SINGLE: i64 = 0x39;
```

*Defined at line 76*

---

## ZCL_OCTSTR:

```fides
pub const ZCL_OCTSTR: i64 = 0x41;
```

*Defined at line 77*

---

## ZCL_STRING:

```fides
pub const ZCL_STRING: i64 = 0x42;
```

*Defined at line 78*

---

## ZCL_UTC:

```fides
pub const ZCL_UTC: i64 = 0xE2;
```

*Defined at line 79*

---

## ZCL_CLUSTER_ID:

```fides
pub const ZCL_CLUSTER_ID: i64 = 0xE8;
```

*Defined at line 80*

---

## ZCL_ATTRIB_ID:

```fides
pub const ZCL_ATTRIB_ID: i64 = 0xE9;
```

*Defined at line 81*

---

## ZCL_IEEE_ADDR:

```fides
pub const ZCL_IEEE_ADDR: i64 = 0xF0;
```

*Defined at line 82*

---

## ZCL_CLUSTER_BASIC:

```fides
pub const ZCL_CLUSTER_BASIC: i64 = 0x0000;
```

*Defined at line 85*

---

## ZCL_CLUSTER_ON_OFF:

```fides
pub const ZCL_CLUSTER_ON_OFF: i64 = 0x0006;
```

*Defined at line 86*

---

## ZCL_CLUSTER_LEVEL:

```fides
pub const ZCL_CLUSTER_LEVEL: i64 = 0x0008;
```

*Defined at line 87*

---

## ZCL_CLUSTER_TEMPERATURE:

```fides
pub const ZCL_CLUSTER_TEMPERATURE: i64 = 0x0402;
```

*Defined at line 88*

---

## ZCL_ATTR_ON_OFF:

```fides
pub const ZCL_ATTR_ON_OFF: i64 = 0x0000;
```

*Defined at line 89*

---

## ZCL_ATTR_CURRENT_LEVEL:

```fides
pub const ZCL_ATTR_CURRENT_LEVEL: i64 = 0x0000;
```

*Defined at line 90*

---

## ZCL_ATTR_MEASURED_VALUE:

```fides
pub const ZCL_ATTR_MEASURED_VALUE: i64 = 0x0000;
```

*Defined at line 91*

---

## ZCL_ATTR_MIN_MEASURED_VALUE:

```fides
pub const ZCL_ATTR_MIN_MEASURED_VALUE: i64 = 0x0001;
```

*Defined at line 92*

---

## ZCL_ATTR_MAX_MEASURED_VALUE:

```fides
pub const ZCL_ATTR_MAX_MEASURED_VALUE: i64 = 0x0002;
```

*Defined at line 93*

---

## ZCL_CMD_OFF:

```fides
pub const ZCL_CMD_OFF: i64 = 0x00;
```

*Defined at line 94*

---

## ZCL_CMD_ON:

```fides
pub const ZCL_CMD_ON: i64 = 0x01;
```

*Defined at line 95*

---

## ZCL_CMD_TOGGLE:

```fides
pub const ZCL_CMD_TOGGLE: i64 = 0x02;
```

*Defined at line 96*

---

## ZCL_CMD_MOVE_TO_LEVEL:

```fides
pub const ZCL_CMD_MOVE_TO_LEVEL: i64 = 0x00;
```

*Defined at line 97*

---

## ZCL_CMD_MOVE_TO_LEVEL_ON_OFF:

```fides
pub const ZCL_CMD_MOVE_TO_LEVEL_ON_OFF: i64 = 0x04;
```

*Defined at line 98*

---

## ZCL_TEMPERATURE_INVALID:

```fides
pub const ZCL_TEMPERATURE_INVALID: i64 = 0x8000;
```

*Defined at line 99*

---

## ZIGBEE_PROFILE_HA:

```fides
pub const ZIGBEE_PROFILE_HA: i64 = 0x0104;
```

*Defined at line 100*

---

## ZclFrame

```fides
pub struct ZclFrame {
```

**Fields:**

| Name | Type |
|------|------|
| `frame_type` | `i64` |
| `manufacturer` | `i64` |
| `direction` | `i64` |
| `disable_default_rsp` | `bool` |
| `seq` | `i64` |
| `command` | `i64` |
| `payload` | `Vec<i64>` |
| `ok` | `bool` |

*Defined at line 104*

---

## zcl_frame

A frame; manufacturer -1 when not manufacturer specific.

```fides
pub fn zcl_frame(frame_type: i64, direction: i64, seq: i64, command: i64, payload: Vec<i64>) -> ZclFrame {
```

**Parameters:**

- `frame_type`: `i64`
- `direction`: `i64`
- `seq`: `i64`
- `command`: `i64`
- `payload`: `Vec<i64>`

**Returns:** `ZclFrame`

*Defined at line 116*

---

## zcl_encode

```fides
pub fn zcl_encode(f: &ZclFrame) -> Vec<i64> {
```

**Parameters:**

- `f`: `&ZclFrame`

**Returns:** `Vec<i64>`

*Defined at line 122*

---

## zcl_decode

```fides
pub fn zcl_decode(b: &Vec<i64>) -> ZclFrame {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `ZclFrame`

*Defined at line 149*

---

## ZclValue

A typed value: integers, booleans, bitmaps, enums and the single float
(as its IEEE-754 bits) are in num; strings and addresses in bytes.

```fides
pub struct ZclValue {
```

**Fields:**

| Name | Type |
|------|------|
| `dtype` | `i64` |
| `num` | `i64` |
| `bytes` | `Vec<i64>` |

*Defined at line 182*

---

## zcl_num

```fides
pub fn zcl_num(dtype: i64, v: i64) -> ZclValue {
```

**Parameters:**

- `dtype`: `i64`
- `v`: `i64`

**Returns:** `ZclValue`

*Defined at line 188*

---

## zcl_bool

```fides
pub fn zcl_bool(b: bool) -> ZclValue {
```

**Parameters:**

- `b`: `bool`

**Returns:** `ZclValue`

*Defined at line 194*

---

## zcl_string

```fides
pub fn zcl_string(s: str) -> ZclValue {
```

**Parameters:**

- `s`: `str`

**Returns:** `ZclValue`

*Defined at line 201*

---

## zcl_type_size

Fixed size in bytes of a type; -1 for strings (length-prefixed), -2
unknown.

```fides
pub fn zcl_type_size(t: i64) -> i64 {
```

**Parameters:**

- `t`: `i64`

**Returns:** `i64`

*Defined at line 209*

---

## zcl_type_signed

```fides
pub fn zcl_type_signed(t: i64) -> bool {
```

**Parameters:**

- `t`: `i64`

**Returns:** `bool`

*Defined at line 261*

---

## zcl_invalid_value

The "invalid value" a sensor reports when it has no reading: the
most negative value of a signed type, all ones for unsigned.

```fides
pub fn zcl_invalid_value(t: i64) -> i64 {
```

**Parameters:**

- `t`: `i64`

**Returns:** `i64`

*Defined at line 267*

---

## zcl_put_value

Append a value's bytes (without the type byte).

```fides
pub fn zcl_put_value(out: &mut Vec<i64>, v: &ZclValue) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `v`: `&ZclValue`

*Defined at line 279*

---

## zcl_get_value

Read a value of type t at off; [the value, the next offset] via
next (next.num = -1 when the bytes run out or the type is unknown).

```fides
pub fn zcl_get_value(b: &Vec<i64>, off: i64, t: i64, next: &mut i64) -> ZclValue {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `off`: `i64`
- `t`: `i64`
- `next`: `&mut i64`

**Returns:** `ZclValue`

*Defined at line 311*

---

## zcl_f32_bits

IEEE-754 single bits of an f64 (round to nearest even; overflow to
infinity, tiny values to subnormals or zero).

```fides
pub fn zcl_f32_bits(x: f64) -> i64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `i64`

*Defined at line 370*

---

## zcl_f32_from_bits

```fides
pub fn zcl_f32_from_bits(b: i64) -> f64 {
```

**Parameters:**

- `b`: `i64`

**Returns:** `f64`

*Defined at line 414*

---

## zcl_single

```fides
pub fn zcl_single(x: f64) -> ZclValue {
```

**Parameters:**

- `x`: `f64`

**Returns:** `ZclValue`

*Defined at line 439*

---

## zcl_read_attributes

```fides
pub fn zcl_read_attributes(seq: i64, attrs: &Vec<i64>) -> ZclFrame {
```

**Parameters:**

- `seq`: `i64`
- `attrs`: `&Vec<i64>`

**Returns:** `ZclFrame`

*Defined at line 454*

---

## ZclAttrRecord

One attribute record: id, status, value (value used when status is 0).

```fides
pub struct ZclAttrRecord {
```

**Fields:**

| Name | Type |
|------|------|
| `attr` | `i64` |
| `status` | `i64` |
| `value` | `ZclValue` |

*Defined at line 465*

---

## zcl_write_attributes

```fides
pub fn zcl_write_attributes(seq: i64, recs: &Vec<ZclAttrRecord>) -> ZclFrame {
```

**Parameters:**

- `seq`: `i64`
- `recs`: `&Vec<ZclAttrRecord>`

**Returns:** `ZclFrame`

*Defined at line 471*

---

## zcl_report_attributes

Report Attributes (server to client, default response disabled).

```fides
pub fn zcl_report_attributes(seq: i64, recs: &Vec<ZclAttrRecord>) -> ZclFrame {
```

**Parameters:**

- `seq`: `i64`
- `recs`: `&Vec<ZclAttrRecord>`

**Returns:** `ZclFrame`

*Defined at line 485*

---

## zcl_decode_attr_values

Decode the records of a Report Attributes or Write Attributes payload
(attr id, type, value).

```fides
pub fn zcl_decode_attr_values(p: &Vec<i64>) -> Vec<ZclAttrRecord> {
```

**Parameters:**

- `p`: `&Vec<i64>`

**Returns:** `Vec<ZclAttrRecord>`

*Defined at line 502*

---

## zcl_decode_read_rsp

Decode a Read Attributes Response payload (attr id, status, [type,
value] when the status is success).

```fides
pub fn zcl_decode_read_rsp(p: &Vec<i64>) -> Vec<ZclAttrRecord> {
```

**Parameters:**

- `p`: `&Vec<i64>`

**Returns:** `Vec<ZclAttrRecord>`

*Defined at line 523*

---

## zcl_default_response

Default Response to a received frame.

```fides
pub fn zcl_default_response(req: &ZclFrame, status: i64) -> ZclFrame {
```

**Parameters:**

- `req`: `&ZclFrame`
- `status`: `i64`

**Returns:** `ZclFrame`

*Defined at line 553*

---

## zcl_configure_reporting

Configure Reporting of one attribute (direction 0: we report it):
min/max interval in seconds, reportable change for analog types.

```fides
pub fn zcl_configure_reporting(seq: i64, attr: i64, dtype: i64, min_s: i64, max_s: i64, change: i64) -> ZclFrame {
```

**Parameters:**

- `seq`: `i64`
- `attr`: `i64`
- `dtype`: `i64`
- `min_s`: `i64`
- `max_s`: `i64`
- `change`: `i64`

**Returns:** `ZclFrame`

*Defined at line 565*

---

## zcl_on_off_command

```fides
pub fn zcl_on_off_command(seq: i64, cmd: i64) -> ZclFrame {
```

**Parameters:**

- `seq`: `i64`
- `cmd`: `i64`

**Returns:** `ZclFrame`

*Defined at line 581*

---

## zcl_move_to_level

Move to Level (with On/Off when with_on_off): level 0..254, transition
time in tenths of a second.

```fides
pub fn zcl_move_to_level(seq: i64, level: i64, transition_ds: i64, with_on_off: bool) -> ZclFrame {
```

**Parameters:**

- `seq`: `i64`
- `level`: `i64`
- `transition_ds`: `i64`
- `with_on_off`: `bool`

**Returns:** `ZclFrame`

*Defined at line 587*

---

## zcl_temperature_value

Temperature Measurement's MeasuredValue: 100 x Celsius, rounded;
0x8000 (invalid) for NaN.

```fides
pub fn zcl_temperature_value(celsius: f64) -> i64 {
```

**Parameters:**

- `celsius`: `f64`

**Returns:** `i64`

*Defined at line 600*

---

## ZclAttr

```fides
pub struct ZclAttr {
```

**Fields:**

| Name | Type |
|------|------|
| `cluster` | `i64` |
| `attr` | `i64` |
| `value` | `ZclValue` |
| `writable` | `bool` |

*Defined at line 613*

---

## ZclEndpoint

```fides
pub struct ZclEndpoint {
```

**Fields:**

| Name | Type |
|------|------|
| `endpoint` | `i64` |
| `attrs` | `Vec<ZclAttr>` |
| `seq` | `i64` |

*Defined at line 620*

---

## zcl_endpoint_light_sensor

An endpoint with On/Off, Level Control and Temperature Measurement
servers (temperature range -40.00 .. 125.00 C).

```fides
pub fn zcl_endpoint_light_sensor(ep: i64) -> ZclEndpoint {
```

**Parameters:**

- `ep`: `i64`

**Returns:** `ZclEndpoint`

*Defined at line 628*

---

## zcl_get

```fides
pub fn zcl_get(e: &ZclEndpoint, cluster: i64, attr: i64) -> i64 {
```

**Parameters:**

- `e`: `&ZclEndpoint`
- `cluster`: `i64`
- `attr`: `i64`

**Returns:** `i64`

*Defined at line 665*

---

## zcl_set

Set an attribute from the application (a sensor reading).

```fides
pub fn zcl_set(e: &mut ZclEndpoint, cluster: i64, attr: i64, v: i64) {
```

**Parameters:**

- `e`: `&mut ZclEndpoint`
- `cluster`: `i64`
- `attr`: `i64`
- `v`: `i64`

*Defined at line 674*

---

## zcl_report_of

A report of one attribute's current value.

```fides
pub fn zcl_report_of(e: &mut ZclEndpoint, cluster: i64, attr: i64) -> Vec<i64> {
```

**Parameters:**

- `e`: `&mut ZclEndpoint`
- `cluster`: `i64`
- `attr`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 684*

---

## zcl_endpoint_handle

Handle a ZCL frame received for `cluster`; the response frame bytes
(empty when none is due).

```fides
pub fn zcl_endpoint_handle(e: &mut ZclEndpoint, cluster: i64, frame: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `e`: `&mut ZclEndpoint`
- `cluster`: `i64`
- `frame`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 737*

---

## APS_FRAME_DATA:

```fides
pub const APS_FRAME_DATA: i64 = 0;
```

*Defined at line 817*

---

## APS_FRAME_COMMAND:

```fides
pub const APS_FRAME_COMMAND: i64 = 1;
```

*Defined at line 818*

---

## APS_FRAME_ACK:

```fides
pub const APS_FRAME_ACK: i64 = 2;
```

*Defined at line 819*

---

## APS_DELIVERY_UNICAST:

```fides
pub const APS_DELIVERY_UNICAST: i64 = 0;
```

*Defined at line 820*

---

## APS_DELIVERY_BROADCAST:

```fides
pub const APS_DELIVERY_BROADCAST: i64 = 2;
```

*Defined at line 821*

---

## APS_DELIVERY_GROUP:

```fides
pub const APS_DELIVERY_GROUP: i64 = 3;
```

*Defined at line 822*

---

## ApsHeader

```fides
pub struct ApsHeader {
```

**Fields:**

| Name | Type |
|------|------|
| `frame_type` | `i64` |
| `delivery` | `i64` |
| `security` | `bool` |
| `ack_request` | `bool` |
| `dst_endpoint` | `i64` |
| `group` | `i64` |
| `cluster` | `i64` |
| `profile` | `i64` |
| `src_endpoint` | `i64` |
| `counter` | `i64` |
| `header_len` | `i64` |

*Defined at line 824*

---

## aps_unicast

```fides
pub fn aps_unicast(dst_ep: i64, cluster: i64, profile: i64, src_ep: i64, counter: i64) -> ApsHeader {
```

**Parameters:**

- `dst_ep`: `i64`
- `cluster`: `i64`
- `profile`: `i64`
- `src_ep`: `i64`
- `counter`: `i64`

**Returns:** `ApsHeader`

*Defined at line 838*

---

## aps_group

```fides
pub fn aps_group(group: i64, cluster: i64, profile: i64, src_ep: i64, counter: i64) -> ApsHeader {
```

**Parameters:**

- `group`: `i64`
- `cluster`: `i64`
- `profile`: `i64`
- `src_ep`: `i64`
- `counter`: `i64`

**Returns:** `ApsHeader`

*Defined at line 844*

---

## aps_encode

The APS data frame: header then payload (a ZCL frame).

```fides
pub fn aps_encode(h: &ApsHeader, payload: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `h`: `&ApsHeader`
- `payload`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 851*

---

## aps_decode

Decode an APS data frame header (header_len = where the payload
starts; 0 when the frame is too short or not a data frame).

```fides
pub fn aps_decode(b: &Vec<i64>) -> ApsHeader {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `ApsHeader`

*Defined at line 880*

---

## aps_payload

```fides
pub fn aps_payload(b: &Vec<i64>, h: &ApsHeader) -> Vec<i64> {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `h`: `&ApsHeader`

**Returns:** `Vec<i64>`

*Defined at line 922*

---

## ZigbeeRadio

The radio seam: tx(handle, aps frame) -> 0 when queued, plus the
network addressing a coordinator API needs (short address / group).

```fides
pub struct ZigbeeRadio {
```

**Fields:**

| Name | Type |
|------|------|
| `tx` | `fn(i64, Vec<i64>) -> i64` |
| `radio` | `i64` |
| `counter` | `i64` |

*Defined at line 934*

---

## zigbee_send_zcl

Send a ZCL frame to an endpoint of the HA profile; the APS counter used.

```fides
pub fn zigbee_send_zcl(r: &mut ZigbeeRadio, dst_ep: i64, cluster: i64, zcl: &ZclFrame) -> i64 {
```

**Parameters:**

- `r`: `&mut ZigbeeRadio`
- `dst_ep`: `i64`
- `cluster`: `i64`
- `zcl`: `&ZclFrame`

**Returns:** `i64`

*Defined at line 941*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
