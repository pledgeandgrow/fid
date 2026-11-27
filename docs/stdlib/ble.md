[All modules](index.md)

# ble -- Fides Documentation

## Contents

- [BleUuid](#BleUuid) (struct)
- [ble_base_uuid](#ble_base_uuid) (function)
- [ble_uuid16](#ble_uuid16) (function)
- [ble_uuid_parse](#ble_uuid_parse) (function)
- [ble_uuid_str](#ble_uuid_str) (function)
- [ble_uuid_short](#ble_uuid_short) (function)
- [ble_uuid_wire](#ble_uuid_wire) (function)
- [ble_uuid_from_wire](#ble_uuid_from_wire) (function)
- [ble_uuid_eq](#ble_uuid_eq) (function)
- [AD_FLAGS:](#AD_FLAGS:) (const)
- [AD_UUID16_INCOMPLETE:](#AD_UUID16_INCOMPLETE:) (const)
- [AD_UUID16_COMPLETE:](#AD_UUID16_COMPLETE:) (const)
- [AD_UUID128_INCOMPLETE:](#AD_UUID128_INCOMPLETE:) (const)
- [AD_UUID128_COMPLETE:](#AD_UUID128_COMPLETE:) (const)
- [AD_NAME_SHORT:](#AD_NAME_SHORT:) (const)
- [AD_NAME_COMPLETE:](#AD_NAME_COMPLETE:) (const)
- [AD_TX_POWER:](#AD_TX_POWER:) (const)
- [AD_APPEARANCE:](#AD_APPEARANCE:) (const)
- [AD_SERVICE_DATA16:](#AD_SERVICE_DATA16:) (const)
- [AD_MANUFACTURER:](#AD_MANUFACTURER:) (const)
- [AD_FLAG_LE_LIMITED:](#AD_FLAG_LE_LIMITED:) (const)
- [AD_FLAG_LE_GENERAL:](#AD_FLAG_LE_GENERAL:) (const)
- [AD_FLAG_NO_BREDR:](#AD_FLAG_NO_BREDR:) (const)
- [BLE_ADV_MAX:](#BLE_ADV_MAX:) (const)
- [AdStruct](#AdStruct) (struct)
- [ad_struct](#ad_struct) (function)
- [ad_flags](#ad_flags) (function)
- [ad_name](#ad_name) (function)
- [ad_uuid16_list](#ad_uuid16_list) (function)
- [ad_tx_power](#ad_tx_power) (function)
- [ad_manufacturer](#ad_manufacturer) (function)
- [ad_service_data16](#ad_service_data16) (function)
- [ad_encode](#ad_encode) (function)
- [ad_parse](#ad_parse) (function)
- [ad_find](#ad_find) (function)
- [ad_find_name](#ad_find_name) (function)
- [ad_ibeacon](#ad_ibeacon) (function)
- [ATT_ERROR_RSP:](#ATT_ERROR_RSP:) (const)
- [ATT_MTU_REQ:](#ATT_MTU_REQ:) (const)
- [ATT_MTU_RSP:](#ATT_MTU_RSP:) (const)
- [ATT_FIND_INFO_REQ:](#ATT_FIND_INFO_REQ:) (const)
- [ATT_FIND_INFO_RSP:](#ATT_FIND_INFO_RSP:) (const)
- [ATT_READ_BY_TYPE_REQ:](#ATT_READ_BY_TYPE_REQ:) (const)
- [ATT_READ_BY_TYPE_RSP:](#ATT_READ_BY_TYPE_RSP:) (const)
- [ATT_READ_REQ:](#ATT_READ_REQ:) (const)
- [ATT_READ_RSP:](#ATT_READ_RSP:) (const)
- [ATT_READ_BLOB_REQ:](#ATT_READ_BLOB_REQ:) (const)
- [ATT_READ_BLOB_RSP:](#ATT_READ_BLOB_RSP:) (const)
- [ATT_READ_BY_GROUP_REQ:](#ATT_READ_BY_GROUP_REQ:) (const)
- [ATT_READ_BY_GROUP_RSP:](#ATT_READ_BY_GROUP_RSP:) (const)
- [ATT_WRITE_REQ:](#ATT_WRITE_REQ:) (const)
- [ATT_WRITE_RSP:](#ATT_WRITE_RSP:) (const)
- [ATT_WRITE_CMD:](#ATT_WRITE_CMD:) (const)
- [ATT_NOTIFY:](#ATT_NOTIFY:) (const)
- [ATT_INDICATE:](#ATT_INDICATE:) (const)
- [ATT_CONFIRM:](#ATT_CONFIRM:) (const)
- [ATT_ERR_INVALID_HANDLE:](#ATT_ERR_INVALID_HANDLE:) (const)
- [ATT_ERR_READ_NOT_PERMITTED:](#ATT_ERR_READ_NOT_PERMITTED:) (const)
- [ATT_ERR_WRITE_NOT_PERMITTED:](#ATT_ERR_WRITE_NOT_PERMITTED:) (const)
- [ATT_ERR_INVALID_PDU:](#ATT_ERR_INVALID_PDU:) (const)
- [ATT_ERR_REQUEST_NOT_SUPPORTED:](#ATT_ERR_REQUEST_NOT_SUPPORTED:) (const)
- [ATT_ERR_INVALID_OFFSET:](#ATT_ERR_INVALID_OFFSET:) (const)
- [ATT_ERR_ATTRIBUTE_NOT_FOUND:](#ATT_ERR_ATTRIBUTE_NOT_FOUND:) (const)
- [ATT_ERR_INVALID_VALUE_LENGTH:](#ATT_ERR_INVALID_VALUE_LENGTH:) (const)
- [ATT_ERR_UNSUPPORTED_GROUP_TYPE:](#ATT_ERR_UNSUPPORTED_GROUP_TYPE:) (const)
- [ATT_DEFAULT_MTU:](#ATT_DEFAULT_MTU:) (const)
- [att_error_rsp](#att_error_rsp) (function)
- [att_mtu_req](#att_mtu_req) (function)
- [att_find_info_req](#att_find_info_req) (function)
- [att_read_by_type_req](#att_read_by_type_req) (function)
- [att_read_by_group_req](#att_read_by_group_req) (function)
- [att_read_req](#att_read_req) (function)
- [att_read_blob_req](#att_read_blob_req) (function)
- [att_write_req](#att_write_req) (function)
- [att_write_cmd](#att_write_cmd) (function)
- [att_notify](#att_notify) (function)
- [att_indicate](#att_indicate) (function)
- [AttEntry](#AttEntry) (struct)
- [att_decode_list](#att_decode_list) (function)
- [GATT_PRIMARY_SERVICE:](#GATT_PRIMARY_SERVICE:) (const)
- [GATT_SECONDARY_SERVICE:](#GATT_SECONDARY_SERVICE:) (const)
- [GATT_INCLUDE:](#GATT_INCLUDE:) (const)
- [GATT_CHARACTERISTIC:](#GATT_CHARACTERISTIC:) (const)
- [GATT_CCCD:](#GATT_CCCD:) (const)
- [GATT_PROP_BROADCAST:](#GATT_PROP_BROADCAST:) (const)
- [GATT_PROP_READ:](#GATT_PROP_READ:) (const)
- [GATT_PROP_WRITE_NR:](#GATT_PROP_WRITE_NR:) (const)
- [GATT_PROP_WRITE:](#GATT_PROP_WRITE:) (const)
- [GATT_PROP_NOTIFY:](#GATT_PROP_NOTIFY:) (const)
- [GATT_PROP_INDICATE:](#GATT_PROP_INDICATE:) (const)
- [GATT_PERM_READ:](#GATT_PERM_READ:) (const)
- [GATT_PERM_WRITE:](#GATT_PERM_WRITE:) (const)
- [GattAttr](#GattAttr) (struct)
- [GattDb](#GattDb) (struct)
- [gatt_db_new](#gatt_db_new) (function)
- [gatt_add_service](#gatt_add_service) (function)
- [gatt_add_characteristic](#gatt_add_characteristic) (function)
- [gatt_get](#gatt_get) (function)
- [gatt_set](#gatt_set) (function)
- [gatt_subscription](#gatt_subscription) (function)
- [gatt_update](#gatt_update) (function)
- [gatt_handle](#gatt_handle) (function)
- [L2CAP_CID_ATT:](#L2CAP_CID_ATT:) (const)
- [L2CAP_CID_SIGNALING:](#L2CAP_CID_SIGNALING:) (const)
- [L2CAP_CID_SMP:](#L2CAP_CID_SMP:) (const)
- [l2cap_frame](#l2cap_frame) (function)
- [l2cap_unframe](#l2cap_unframe) (function)
- [HCI_COMMAND_PKT:](#HCI_COMMAND_PKT:) (const)
- [HCI_ACL_PKT:](#HCI_ACL_PKT:) (const)
- [HCI_EVENT_PKT:](#HCI_EVENT_PKT:) (const)
- [HCI_EV_DISCONN_COMPLETE:](#HCI_EV_DISCONN_COMPLETE:) (const)
- [HCI_EV_CMD_COMPLETE:](#HCI_EV_CMD_COMPLETE:) (const)
- [HCI_EV_CMD_STATUS:](#HCI_EV_CMD_STATUS:) (const)
- [HCI_EV_LE_META:](#HCI_EV_LE_META:) (const)
- [HCI_LE_ADV_REPORT:](#HCI_LE_ADV_REPORT:) (const)
- [HCI_LE_CONN_COMPLETE:](#HCI_LE_CONN_COMPLETE:) (const)
- [hci_opcode](#hci_opcode) (function)
- [hci_op_reset](#hci_op_reset) (function)
- [hci_op_le_set_adv_params](#hci_op_le_set_adv_params) (function)
- [hci_op_le_set_adv_data](#hci_op_le_set_adv_data) (function)
- [hci_op_le_set_scan_rsp_data](#hci_op_le_set_scan_rsp_data) (function)
- [hci_op_le_set_adv_enable](#hci_op_le_set_adv_enable) (function)
- [hci_op_le_set_scan_params](#hci_op_le_set_scan_params) (function)
- [hci_op_le_set_scan_enable](#hci_op_le_set_scan_enable) (function)
- [hci_command](#hci_command) (function)
- [hci_reset](#hci_reset) (function)
- [hci_le_set_adv_params](#hci_le_set_adv_params) (function)
- [hci_le_set_adv_data](#hci_le_set_adv_data) (function)
- [hci_le_set_adv_enable](#hci_le_set_adv_enable) (function)
- [hci_le_set_scan_params](#hci_le_set_scan_params) (function)
- [hci_le_set_scan_enable](#hci_le_set_scan_enable) (function)
- [HciEvent](#HciEvent) (struct)
- [hci_decode_event](#hci_decode_event) (function)
- [BleAdvReport](#BleAdvReport) (struct)
- [hci_decode_adv_reports](#hci_decode_adv_reports) (function)
- [ble_addr_str](#ble_addr_str) (function)
- [hci_acl](#hci_acl) (function)
- [hci_acl_decode](#hci_acl_decode) (function)
- [HciTransport](#HciTransport) (struct)
- [hci_send_command](#hci_send_command) (function)

## BleUuid

A UUID as its 16 bytes in wire order (little-endian).

```fides
pub struct BleUuid {
```

**Fields:**

| Name | Type |
|------|------|
| `b` | `Vec<i64>` |

*Defined at line 33*

---

## ble_base_uuid

00000000-0000-1000-8000-00805F9B34FB in wire order.

```fides
pub fn ble_base_uuid() -> BleUuid {
```

**Returns:** `BleUuid`

*Defined at line 38*

---

## ble_uuid16

```fides
pub fn ble_uuid16(u: i64) -> BleUuid {
```

**Parameters:**

- `u`: `i64`

**Returns:** `BleUuid`

*Defined at line 51*

---

## ble_uuid_parse

Parse "0000180d-0000-1000-8000-00805f9b34fb" (or 4 hex digits).

```fides
pub fn ble_uuid_parse(s: str) -> BleUuid {
```

**Parameters:**

- `s`: `str`

**Returns:** `BleUuid`

*Defined at line 91*

---

## ble_uuid_str

```fides
pub fn ble_uuid_str(u: &BleUuid) -> str {
```

**Parameters:**

- `u`: `&BleUuid`

**Returns:** `str`

*Defined at line 116*

---

## ble_uuid_short

The 16-bit alias when the UUID is on the base, else -1.

```fides
pub fn ble_uuid_short(u: &BleUuid) -> i64 {
```

**Parameters:**

- `u`: `&BleUuid`

**Returns:** `i64`

*Defined at line 130*

---

## ble_uuid_wire

Wire form: 2 bytes for a base UUID, else 16.

```fides
pub fn ble_uuid_wire(u: &BleUuid) -> Vec<i64> {
```

**Parameters:**

- `u`: `&BleUuid`

**Returns:** `Vec<i64>`

*Defined at line 146*

---

## ble_uuid_from_wire

```fides
pub fn ble_uuid_from_wire(b: &Vec<i64>, off: i64, n: i64) -> BleUuid {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `off`: `i64`
- `n`: `i64`

**Returns:** `BleUuid`

*Defined at line 157*

---

## ble_uuid_eq

```fides
pub fn ble_uuid_eq(a: &BleUuid, b: &BleUuid) -> bool {
```

**Parameters:**

- `a`: `&BleUuid`
- `b`: `&BleUuid`

**Returns:** `bool`

*Defined at line 172*

---

## AD_FLAGS:

```fides
pub const AD_FLAGS: i64 = 0x01;
```

*Defined at line 185*

---

## AD_UUID16_INCOMPLETE:

```fides
pub const AD_UUID16_INCOMPLETE: i64 = 0x02;
```

*Defined at line 186*

---

## AD_UUID16_COMPLETE:

```fides
pub const AD_UUID16_COMPLETE: i64 = 0x03;
```

*Defined at line 187*

---

## AD_UUID128_INCOMPLETE:

```fides
pub const AD_UUID128_INCOMPLETE: i64 = 0x06;
```

*Defined at line 188*

---

## AD_UUID128_COMPLETE:

```fides
pub const AD_UUID128_COMPLETE: i64 = 0x07;
```

*Defined at line 189*

---

## AD_NAME_SHORT:

```fides
pub const AD_NAME_SHORT: i64 = 0x08;
```

*Defined at line 190*

---

## AD_NAME_COMPLETE:

```fides
pub const AD_NAME_COMPLETE: i64 = 0x09;
```

*Defined at line 191*

---

## AD_TX_POWER:

```fides
pub const AD_TX_POWER: i64 = 0x0A;
```

*Defined at line 192*

---

## AD_APPEARANCE:

```fides
pub const AD_APPEARANCE: i64 = 0x19;
```

*Defined at line 193*

---

## AD_SERVICE_DATA16:

```fides
pub const AD_SERVICE_DATA16: i64 = 0x16;
```

*Defined at line 194*

---

## AD_MANUFACTURER:

```fides
pub const AD_MANUFACTURER: i64 = 0xFF;
```

*Defined at line 195*

---

## AD_FLAG_LE_LIMITED:

```fides
pub const AD_FLAG_LE_LIMITED: i64 = 0x01;
```

*Defined at line 196*

---

## AD_FLAG_LE_GENERAL:

```fides
pub const AD_FLAG_LE_GENERAL: i64 = 0x02;
```

*Defined at line 197*

---

## AD_FLAG_NO_BREDR:

```fides
pub const AD_FLAG_NO_BREDR: i64 = 0x04;
```

*Defined at line 198*

---

## BLE_ADV_MAX:

```fides
pub const BLE_ADV_MAX: i64 = 31;
```

*Defined at line 199*

---

## AdStruct

```fides
pub struct AdStruct {
```

**Fields:**

| Name | Type |
|------|------|
| `ad_type` | `i64` |
| `data` | `Vec<i64>` |

*Defined at line 201*

---

## ad_struct

```fides
pub fn ad_struct(t: i64, data: Vec<i64>) -> AdStruct {
```

**Parameters:**

- `t`: `i64`
- `data`: `Vec<i64>`

**Returns:** `AdStruct`

*Defined at line 206*

---

## ad_flags

```fides
pub fn ad_flags(f: i64) -> AdStruct {
```

**Parameters:**

- `f`: `i64`

**Returns:** `AdStruct`

*Defined at line 212*

---

## ad_name

```fides
pub fn ad_name(name: str) -> AdStruct {
```

**Parameters:**

- `name`: `str`

**Returns:** `AdStruct`

*Defined at line 218*

---

## ad_uuid16_list

```fides
pub fn ad_uuid16_list(uuids: &Vec<i64>) -> AdStruct {
```

**Parameters:**

- `uuids`: `&Vec<i64>`

**Returns:** `AdStruct`

*Defined at line 222*

---

## ad_tx_power

```fides
pub fn ad_tx_power(dbm: i64) -> AdStruct {
```

**Parameters:**

- `dbm`: `i64`

**Returns:** `AdStruct`

*Defined at line 233*

---

## ad_manufacturer

```fides
pub fn ad_manufacturer(company: i64, payload: &Vec<i64>) -> AdStruct {
```

**Parameters:**

- `company`: `i64`
- `payload`: `&Vec<i64>`

**Returns:** `AdStruct`

*Defined at line 239*

---

## ad_service_data16

```fides
pub fn ad_service_data16(uuid: i64, payload: &Vec<i64>) -> AdStruct {
```

**Parameters:**

- `uuid`: `i64`
- `payload`: `&Vec<i64>`

**Returns:** `AdStruct`

*Defined at line 251*

---

## ad_encode

Encode AD structures; an empty result when they exceed max_len.

```fides
pub fn ad_encode(list: &Vec<AdStruct>, max_len: i64) -> Vec<i64> {
```

**Parameters:**

- `list`: `&Vec<AdStruct>`
- `max_len`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 264*

---

## ad_parse

Parse advertising data; stops at a zero length (padding) or at a
structure that runs past the end.

```fides
pub fn ad_parse(bytes: &Vec<i64>) -> Vec<AdStruct> {
```

**Parameters:**

- `bytes`: `&Vec<i64>`

**Returns:** `Vec<AdStruct>`

*Defined at line 286*

---

## ad_find

The data of the first structure of a type (empty if absent).

```fides
pub fn ad_find(list: &Vec<AdStruct>, t: i64) -> Vec<i64> {
```

**Parameters:**

- `list`: `&Vec<AdStruct>`
- `t`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 309*

---

## ad_find_name

```fides
pub fn ad_find_name(list: &Vec<AdStruct>) -> str {
```

**Parameters:**

- `list`: `&Vec<AdStruct>`

**Returns:** `str`

*Defined at line 320*

---

## ad_ibeacon

Apple iBeacon manufacturer data (company 0x004C, type 0x02, length
0x15, proximity UUID big-endian, major, minor, measured power).

```fides
pub fn ad_ibeacon(uuid_be: &Vec<i64>, major: i64, minor: i64, power: i64) -> AdStruct {
```

**Parameters:**

- `uuid_be`: `&Vec<i64>`
- `major`: `i64`
- `minor`: `i64`
- `power`: `i64`

**Returns:** `AdStruct`

*Defined at line 330*

---

## ATT_ERROR_RSP:

```fides
pub const ATT_ERROR_RSP: i64 = 0x01;
```

*Defined at line 349*

---

## ATT_MTU_REQ:

```fides
pub const ATT_MTU_REQ: i64 = 0x02;
```

*Defined at line 350*

---

## ATT_MTU_RSP:

```fides
pub const ATT_MTU_RSP: i64 = 0x03;
```

*Defined at line 351*

---

## ATT_FIND_INFO_REQ:

```fides
pub const ATT_FIND_INFO_REQ: i64 = 0x04;
```

*Defined at line 352*

---

## ATT_FIND_INFO_RSP:

```fides
pub const ATT_FIND_INFO_RSP: i64 = 0x05;
```

*Defined at line 353*

---

## ATT_READ_BY_TYPE_REQ:

```fides
pub const ATT_READ_BY_TYPE_REQ: i64 = 0x08;
```

*Defined at line 354*

---

## ATT_READ_BY_TYPE_RSP:

```fides
pub const ATT_READ_BY_TYPE_RSP: i64 = 0x09;
```

*Defined at line 355*

---

## ATT_READ_REQ:

```fides
pub const ATT_READ_REQ: i64 = 0x0A;
```

*Defined at line 356*

---

## ATT_READ_RSP:

```fides
pub const ATT_READ_RSP: i64 = 0x0B;
```

*Defined at line 357*

---

## ATT_READ_BLOB_REQ:

```fides
pub const ATT_READ_BLOB_REQ: i64 = 0x0C;
```

*Defined at line 358*

---

## ATT_READ_BLOB_RSP:

```fides
pub const ATT_READ_BLOB_RSP: i64 = 0x0D;
```

*Defined at line 359*

---

## ATT_READ_BY_GROUP_REQ:

```fides
pub const ATT_READ_BY_GROUP_REQ: i64 = 0x10;
```

*Defined at line 360*

---

## ATT_READ_BY_GROUP_RSP:

```fides
pub const ATT_READ_BY_GROUP_RSP: i64 = 0x11;
```

*Defined at line 361*

---

## ATT_WRITE_REQ:

```fides
pub const ATT_WRITE_REQ: i64 = 0x12;
```

*Defined at line 362*

---

## ATT_WRITE_RSP:

```fides
pub const ATT_WRITE_RSP: i64 = 0x13;
```

*Defined at line 363*

---

## ATT_WRITE_CMD:

```fides
pub const ATT_WRITE_CMD: i64 = 0x52;
```

*Defined at line 364*

---

## ATT_NOTIFY:

```fides
pub const ATT_NOTIFY: i64 = 0x1B;
```

*Defined at line 365*

---

## ATT_INDICATE:

```fides
pub const ATT_INDICATE: i64 = 0x1D;
```

*Defined at line 366*

---

## ATT_CONFIRM:

```fides
pub const ATT_CONFIRM: i64 = 0x1E;
```

*Defined at line 367*

---

## ATT_ERR_INVALID_HANDLE:

```fides
pub const ATT_ERR_INVALID_HANDLE: i64 = 0x01;
```

*Defined at line 369*

---

## ATT_ERR_READ_NOT_PERMITTED:

```fides
pub const ATT_ERR_READ_NOT_PERMITTED: i64 = 0x02;
```

*Defined at line 370*

---

## ATT_ERR_WRITE_NOT_PERMITTED:

```fides
pub const ATT_ERR_WRITE_NOT_PERMITTED: i64 = 0x03;
```

*Defined at line 371*

---

## ATT_ERR_INVALID_PDU:

```fides
pub const ATT_ERR_INVALID_PDU: i64 = 0x04;
```

*Defined at line 372*

---

## ATT_ERR_REQUEST_NOT_SUPPORTED:

```fides
pub const ATT_ERR_REQUEST_NOT_SUPPORTED: i64 = 0x06;
```

*Defined at line 373*

---

## ATT_ERR_INVALID_OFFSET:

```fides
pub const ATT_ERR_INVALID_OFFSET: i64 = 0x07;
```

*Defined at line 374*

---

## ATT_ERR_ATTRIBUTE_NOT_FOUND:

```fides
pub const ATT_ERR_ATTRIBUTE_NOT_FOUND: i64 = 0x0A;
```

*Defined at line 375*

---

## ATT_ERR_INVALID_VALUE_LENGTH:

```fides
pub const ATT_ERR_INVALID_VALUE_LENGTH: i64 = 0x0D;
```

*Defined at line 376*

---

## ATT_ERR_UNSUPPORTED_GROUP_TYPE:

```fides
pub const ATT_ERR_UNSUPPORTED_GROUP_TYPE: i64 = 0x10;
```

*Defined at line 377*

---

## ATT_DEFAULT_MTU:

```fides
pub const ATT_DEFAULT_MTU: i64 = 23;
```

*Defined at line 379*

---

## att_error_rsp

```fides
pub fn att_error_rsp(req_opcode: i64, handle: i64, code: i64) -> Vec<i64> {
```

**Parameters:**

- `req_opcode`: `i64`
- `handle`: `i64`
- `code`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 398*

---

## att_mtu_req

```fides
pub fn att_mtu_req(mtu: i64) -> Vec<i64> {
```

**Parameters:**

- `mtu`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 407*

---

## att_find_info_req

```fides
pub fn att_find_info_req(start: i64, end: i64) -> Vec<i64> {
```

**Parameters:**

- `start`: `i64`
- `end`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 414*

---

## att_read_by_type_req

```fides
pub fn att_read_by_type_req(start: i64, end: i64, t: &BleUuid) -> Vec<i64> {
```

**Parameters:**

- `start`: `i64`
- `end`: `i64`
- `t`: `&BleUuid`

**Returns:** `Vec<i64>`

*Defined at line 422*

---

## att_read_by_group_req

```fides
pub fn att_read_by_group_req(start: i64, end: i64, t: &BleUuid) -> Vec<i64> {
```

**Parameters:**

- `start`: `i64`
- `end`: `i64`
- `t`: `&BleUuid`

**Returns:** `Vec<i64>`

*Defined at line 431*

---

## att_read_req

```fides
pub fn att_read_req(handle: i64) -> Vec<i64> {
```

**Parameters:**

- `handle`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 437*

---

## att_read_blob_req

```fides
pub fn att_read_blob_req(handle: i64, offset: i64) -> Vec<i64> {
```

**Parameters:**

- `handle`: `i64`
- `offset`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 444*

---

## att_write_req

```fides
pub fn att_write_req(handle: i64, value: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `handle`: `i64`
- `value`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 452*

---

## att_write_cmd

```fides
pub fn att_write_cmd(handle: i64, value: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `handle`: `i64`
- `value`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 460*

---

## att_notify

```fides
pub fn att_notify(handle: i64, value: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `handle`: `i64`
- `value`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 466*

---

## att_indicate

```fides
pub fn att_indicate(handle: i64, value: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `handle`: `i64`
- `value`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 474*

---

## AttEntry

One attribute of a Read By Type / Read By Group Type response.

```fides
pub struct AttEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `handle` | `i64` |
| `end_handle` | `i64` |
| `value` | `Vec<i64>` |

*Defined at line 481*

---

## att_decode_list

Decode a Read By Type (0x09) or Read By Group Type (0x11) response.

```fides
pub fn att_decode_list(pdu: &Vec<i64>) -> Vec<AttEntry> {
```

**Parameters:**

- `pdu`: `&Vec<i64>`

**Returns:** `Vec<AttEntry>`

*Defined at line 488*

---

## GATT_PRIMARY_SERVICE:

```fides
pub const GATT_PRIMARY_SERVICE: i64 = 0x2800;
```

*Defined at line 524*

---

## GATT_SECONDARY_SERVICE:

```fides
pub const GATT_SECONDARY_SERVICE: i64 = 0x2801;
```

*Defined at line 525*

---

## GATT_INCLUDE:

```fides
pub const GATT_INCLUDE: i64 = 0x2802;
```

*Defined at line 526*

---

## GATT_CHARACTERISTIC:

```fides
pub const GATT_CHARACTERISTIC: i64 = 0x2803;
```

*Defined at line 527*

---

## GATT_CCCD:

```fides
pub const GATT_CCCD: i64 = 0x2902;
```

*Defined at line 528*

---

## GATT_PROP_BROADCAST:

```fides
pub const GATT_PROP_BROADCAST: i64 = 0x01;
```

*Defined at line 529*

---

## GATT_PROP_READ:

```fides
pub const GATT_PROP_READ: i64 = 0x02;
```

*Defined at line 530*

---

## GATT_PROP_WRITE_NR:

```fides
pub const GATT_PROP_WRITE_NR: i64 = 0x04;
```

*Defined at line 531*

---

## GATT_PROP_WRITE:

```fides
pub const GATT_PROP_WRITE: i64 = 0x08;
```

*Defined at line 532*

---

## GATT_PROP_NOTIFY:

```fides
pub const GATT_PROP_NOTIFY: i64 = 0x10;
```

*Defined at line 533*

---

## GATT_PROP_INDICATE:

```fides
pub const GATT_PROP_INDICATE: i64 = 0x20;
```

*Defined at line 534*

---

## GATT_PERM_READ:

```fides
pub const GATT_PERM_READ: i64 = 1;
```

*Defined at line 535*

---

## GATT_PERM_WRITE:

```fides
pub const GATT_PERM_WRITE: i64 = 2;
```

*Defined at line 536*

---

## GattAttr

```fides
pub struct GattAttr {
```

**Fields:**

| Name | Type |
|------|------|
| `handle` | `i64` |
| `atype` | `BleUuid` |
| `value` | `Vec<i64>` |
| `perms` | `i64` |
| `group_end` | `i64` |

*Defined at line 538*

---

## GattDb

```fides
pub struct GattDb {
```

**Fields:**

| Name | Type |
|------|------|
| `attrs` | `Vec<GattAttr>` |
| `mtu` | `i64` |
| `server_mtu` | `i64` |

*Defined at line 546*

---

## gatt_db_new

```fides
pub fn gatt_db_new(server_mtu: i64) -> GattDb {
```

**Parameters:**

- `server_mtu`: `i64`

**Returns:** `GattDb`

*Defined at line 552*

---

## gatt_add_service

A primary service; its handle.

```fides
pub fn gatt_add_service(db: &mut GattDb, uuid: &BleUuid) -> i64 {
```

**Parameters:**

- `db`: `&mut GattDb`
- `uuid`: `&BleUuid`

**Returns:** `i64`

*Defined at line 580*

---

## gatt_add_characteristic

A characteristic: its declaration, the value (permissions from the
properties) and a CCCD when it notifies or indicates. The value handle.

```fides
pub fn gatt_add_characteristic(db: &mut GattDb, uuid: &BleUuid, props: i64, value: Vec<i64>) -> i64 {
```

**Parameters:**

- `db`: `&mut GattDb`
- `uuid`: `&BleUuid`
- `props`: `i64`
- `value`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 586*

---

## gatt_get

```fides
pub fn gatt_get(db: &GattDb, handle: i64) -> Vec<i64> {
```

**Parameters:**

- `db`: `&GattDb`
- `handle`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 612*

---

## gatt_set

Set a value from the application side (no permission check).

```fides
pub fn gatt_set(db: &mut GattDb, handle: i64, value: Vec<i64>) {
```

**Parameters:**

- `db`: `&mut GattDb`
- `handle`: `i64`
- `value`: `Vec<i64>`

*Defined at line 620*

---

## gatt_subscription

The CCCD bits (1 notify, 2 indicate) of the characteristic whose value
is at `handle`.

```fides
pub fn gatt_subscription(db: &GattDb, value_handle: i64) -> i64 {
```

**Parameters:**

- `db`: `&GattDb`
- `value_handle`: `i64`

**Returns:** `i64`

*Defined at line 630*

---

## gatt_update

Update a value and give the PDU to send: a notification or an
indication when the client subscribed, else empty.

```fides
pub fn gatt_update(db: &mut GattDb, value_handle: i64, value: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `db`: `&mut GattDb`
- `value_handle`: `i64`
- `value`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 643*

---

## gatt_handle

Answer one ATT PDU from the client. Write commands, notifications'
confirmations and unknown commands give an empty answer.

```fides
pub fn gatt_handle(db: &mut GattDb, pdu: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `db`: `&mut GattDb`
- `pdu`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 666*

---

## L2CAP_CID_ATT:

```fides
pub const L2CAP_CID_ATT: i64 = 0x0004;
```

*Defined at line 881*

---

## L2CAP_CID_SIGNALING:

```fides
pub const L2CAP_CID_SIGNALING: i64 = 0x0005;
```

*Defined at line 882*

---

## L2CAP_CID_SMP:

```fides
pub const L2CAP_CID_SMP: i64 = 0x0006;
```

*Defined at line 883*

---

## l2cap_frame

```fides
pub fn l2cap_frame(cid: i64, payload: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `cid`: `i64`
- `payload`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 885*

---

## l2cap_unframe

[cid, payload...] of a complete basic frame; empty if the length
field does not match.

```fides
pub fn l2cap_unframe(frame: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `frame`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 895*

---

## HCI_COMMAND_PKT:

```fides
pub const HCI_COMMAND_PKT: i64 = 0x01;
```

*Defined at line 915*

---

## HCI_ACL_PKT:

```fides
pub const HCI_ACL_PKT: i64 = 0x02;
```

*Defined at line 916*

---

## HCI_EVENT_PKT:

```fides
pub const HCI_EVENT_PKT: i64 = 0x04;
```

*Defined at line 917*

---

## HCI_EV_DISCONN_COMPLETE:

```fides
pub const HCI_EV_DISCONN_COMPLETE: i64 = 0x05;
```

*Defined at line 918*

---

## HCI_EV_CMD_COMPLETE:

```fides
pub const HCI_EV_CMD_COMPLETE: i64 = 0x0E;
```

*Defined at line 919*

---

## HCI_EV_CMD_STATUS:

```fides
pub const HCI_EV_CMD_STATUS: i64 = 0x0F;
```

*Defined at line 920*

---

## HCI_EV_LE_META:

```fides
pub const HCI_EV_LE_META: i64 = 0x3E;
```

*Defined at line 921*

---

## HCI_LE_ADV_REPORT:

```fides
pub const HCI_LE_ADV_REPORT: i64 = 0x02;
```

*Defined at line 922*

---

## HCI_LE_CONN_COMPLETE:

```fides
pub const HCI_LE_CONN_COMPLETE: i64 = 0x01;
```

*Defined at line 923*

---

## hci_opcode

```fides
pub fn hci_opcode(ogf: i64, ocf: i64) -> i64 {
```

**Parameters:**

- `ogf`: `i64`
- `ocf`: `i64`

**Returns:** `i64`

*Defined at line 925*

---

## hci_op_reset

```fides
pub fn hci_op_reset() -> i64 {
```

**Returns:** `i64`

*Defined at line 929*

---

## hci_op_le_set_adv_params

```fides
pub fn hci_op_le_set_adv_params() -> i64 {
```

**Returns:** `i64`

*Defined at line 933*

---

## hci_op_le_set_adv_data

```fides
pub fn hci_op_le_set_adv_data() -> i64 {
```

**Returns:** `i64`

*Defined at line 937*

---

## hci_op_le_set_scan_rsp_data

```fides
pub fn hci_op_le_set_scan_rsp_data() -> i64 {
```

**Returns:** `i64`

*Defined at line 941*

---

## hci_op_le_set_adv_enable

```fides
pub fn hci_op_le_set_adv_enable() -> i64 {
```

**Returns:** `i64`

*Defined at line 945*

---

## hci_op_le_set_scan_params

```fides
pub fn hci_op_le_set_scan_params() -> i64 {
```

**Returns:** `i64`

*Defined at line 949*

---

## hci_op_le_set_scan_enable

```fides
pub fn hci_op_le_set_scan_enable() -> i64 {
```

**Returns:** `i64`

*Defined at line 953*

---

## hci_command

An H4 command packet: 0x01, opcode (LE), parameter length, parameters.

```fides
pub fn hci_command(opcode: i64, params: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `opcode`: `i64`
- `params`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 958*

---

## hci_reset

```fides
pub fn hci_reset() -> Vec<i64> {
```

**Returns:** `Vec<i64>`

*Defined at line 967*

---

## hci_le_set_adv_params

LE Set Advertising Parameters: intervals in 0.625 ms units, type
(0 ADV_IND, 2 ADV_SCAN_IND, 3 ADV_NONCONN_IND), own address type,
channel map 0x07, no filter.

```fides
pub fn hci_le_set_adv_params(min_int: i64, max_int: i64, adv_type: i64, own_addr_type: i64) -> Vec<i64> {
```

**Parameters:**

- `min_int`: `i64`
- `max_int`: `i64`
- `adv_type`: `i64`
- `own_addr_type`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 974*

---

## hci_le_set_adv_data

LE Set Advertising Data: length byte, then 31 bytes (zero padded).

```fides
pub fn hci_le_set_adv_data(ad: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `ad`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 992*

---

## hci_le_set_adv_enable

```fides
pub fn hci_le_set_adv_enable(on: bool) -> Vec<i64> {
```

**Parameters:**

- `on`: `bool`

**Returns:** `Vec<i64>`

*Defined at line 1007*

---

## hci_le_set_scan_params

LE Set Scan Parameters: active (true) or passive, interval and window
in 0.625 ms units.

```fides
pub fn hci_le_set_scan_params(active: bool, interval: i64, window: i64) -> Vec<i64> {
```

**Parameters:**

- `active`: `bool`
- `interval`: `i64`
- `window`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1019*

---

## hci_le_set_scan_enable

```fides
pub fn hci_le_set_scan_enable(on: bool, filter_dup: bool) -> Vec<i64> {
```

**Parameters:**

- `on`: `bool`
- `filter_dup`: `bool`

**Returns:** `Vec<i64>`

*Defined at line 1033*

---

## HciEvent

A decoded event. kind: the event code; for Command Complete / Status
opcode and status (Complete: the first return parameter); for LE Meta
the subevent; params are the bytes after the event header.

```fides
pub struct HciEvent {
```

**Fields:**

| Name | Type |
|------|------|
| `code` | `i64` |
| `opcode` | `i64` |
| `status` | `i64` |
| `subevent` | `i64` |
| `params` | `Vec<i64>` |
| `ok` | `bool` |

*Defined at line 1051*

---

## hci_decode_event

Decode an H4 event packet (0x04, code, length, parameters).

```fides
pub fn hci_decode_event(pkt: &Vec<i64>) -> HciEvent {
```

**Parameters:**

- `pkt`: `&Vec<i64>`

**Returns:** `HciEvent`

*Defined at line 1061*

---

## BleAdvReport

One device seen in an LE Advertising Report.

```fides
pub struct BleAdvReport {
```

**Fields:**

| Name | Type |
|------|------|
| `event_type` | `i64` |
| `addr_type` | `i64` |
| `addr` | `Vec<i64>` |
| `data` | `Vec<i64>` |
| `rssi` | `i64` |

*Defined at line 1096*

---

## hci_decode_adv_reports

The reports of an LE Meta / Advertising Report event's parameters
(subevent, num_reports, then the reports one after another).

```fides
pub fn hci_decode_adv_reports(params: &Vec<i64>) -> Vec<BleAdvReport> {
```

**Parameters:**

- `params`: `&Vec<i64>`

**Returns:** `Vec<BleAdvReport>`

*Defined at line 1106*

---

## ble_addr_str

A device address (6 bytes, wire order) as "AA:BB:CC:DD:EE:FF".

```fides
pub fn ble_addr_str(addr: &Vec<i64>) -> str {
```

**Parameters:**

- `addr`: `&Vec<i64>`

**Returns:** `str`

*Defined at line 1147*

---

## hci_acl

An H4 ACL packet: handle (12 bits) | PB flag << 12 | BC flag << 14.

```fides
pub fn hci_acl(handle: i64, pb: i64, data: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `handle`: `i64`
- `pb`: `i64`
- `data`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 1163*

---

## hci_acl_decode

[handle, pb, data...] of an H4 ACL packet; empty when malformed.

```fides
pub fn hci_acl_decode(pkt: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `pkt`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 1173*

---

## HciTransport

The controller link: send one H4 packet (0 = sent), receive one (an
empty Vec when nothing arrived).

```fides
pub struct HciTransport {
```

**Fields:**

| Name | Type |
|------|------|
| `handle` | `i64` |
| `write_pkt` | `fn(i64, Vec<i64>) -> i64` |
| `read_pkt` | `fn(i64) -> Vec<i64>` |

*Defined at line 1195*

---

## hci_send_command

Send a command and wait (up to max_packets received) for its Command
Complete or Command Status; the event, ok=false on no answer.

```fides
pub fn hci_send_command(t: &HciTransport, cmd: Vec<i64>, max_packets: i64) -> HciEvent {
```

**Parameters:**

- `t`: `&HciTransport`
- `cmd`: `Vec<i64>`
- `max_packets`: `i64`

**Returns:** `HciEvent`

*Defined at line 1203*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
