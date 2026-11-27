[All modules](index.md)

# usb_device -- Fides Documentation

## Contents

- [USB_DESC_DEVICE:](#USB_DESC_DEVICE:) (const)
- [USB_DESC_CONFIG:](#USB_DESC_CONFIG:) (const)
- [USB_DESC_STRING:](#USB_DESC_STRING:) (const)
- [USB_DESC_INTERFACE:](#USB_DESC_INTERFACE:) (const)
- [USB_DESC_ENDPOINT:](#USB_DESC_ENDPOINT:) (const)
- [USB_DESC_QUALIFIER:](#USB_DESC_QUALIFIER:) (const)
- [USB_DESC_IAD:](#USB_DESC_IAD:) (const)
- [USB_DESC_CS_INTERFACE:](#USB_DESC_CS_INTERFACE:) (const)
- [USB_REQ_GET_STATUS:](#USB_REQ_GET_STATUS:) (const)
- [USB_REQ_CLEAR_FEATURE:](#USB_REQ_CLEAR_FEATURE:) (const)
- [USB_REQ_SET_FEATURE:](#USB_REQ_SET_FEATURE:) (const)
- [USB_REQ_SET_ADDRESS:](#USB_REQ_SET_ADDRESS:) (const)
- [USB_REQ_GET_DESCRIPTOR:](#USB_REQ_GET_DESCRIPTOR:) (const)
- [USB_REQ_SET_DESCRIPTOR:](#USB_REQ_SET_DESCRIPTOR:) (const)
- [USB_REQ_GET_CONFIGURATION:](#USB_REQ_GET_CONFIGURATION:) (const)
- [USB_REQ_SET_CONFIGURATION:](#USB_REQ_SET_CONFIGURATION:) (const)
- [USB_REQ_GET_INTERFACE:](#USB_REQ_GET_INTERFACE:) (const)
- [USB_REQ_SET_INTERFACE:](#USB_REQ_SET_INTERFACE:) (const)
- [USB_FEAT_ENDPOINT_HALT:](#USB_FEAT_ENDPOINT_HALT:) (const)
- [USB_FEAT_REMOTE_WAKEUP:](#USB_FEAT_REMOTE_WAKEUP:) (const)
- [CDC_SET_LINE_CODING:](#CDC_SET_LINE_CODING:) (const)
- [CDC_GET_LINE_CODING:](#CDC_GET_LINE_CODING:) (const)
- [CDC_SET_CONTROL_LINE_STATE:](#CDC_SET_CONTROL_LINE_STATE:) (const)
- [CDC_SEND_BREAK:](#CDC_SEND_BREAK:) (const)
- [CDC_SERIAL_STATE:](#CDC_SERIAL_STATE:) (const)
- [USB_STATE_DEFAULT:](#USB_STATE_DEFAULT:) (const)
- [USB_STATE_ADDRESS:](#USB_STATE_ADDRESS:) (const)
- [USB_STATE_CONFIGURED:](#USB_STATE_CONFIGURED:) (const)
- [USB_EP_CONTROL:](#USB_EP_CONTROL:) (const)
- [USB_EP_ISO:](#USB_EP_ISO:) (const)
- [USB_EP_BULK:](#USB_EP_BULK:) (const)
- [USB_EP_INTERRUPT:](#USB_EP_INTERRUPT:) (const)
- [usb_state_name](#usb_state_name) (function)
- [usb_device_desc](#usb_device_desc) (function)
- [usb_config_desc](#usb_config_desc) (function)
- [usb_interface_desc](#usb_interface_desc) (function)
- [usb_endpoint_desc](#usb_endpoint_desc) (function)
- [usb_iad_desc](#usb_iad_desc) (function)
- [usb_config_finish](#usb_config_finish) (function)
- [usb_string_desc](#usb_string_desc) (function)
- [usb_langid_desc](#usb_langid_desc) (function)
- [usb_cdc_acm_config](#usb_cdc_acm_config) (function)
- [UsbSetup](#UsbSetup) (struct)
- [usb_setup_parse](#usb_setup_parse) (function)
- [usb_setup_bytes](#usb_setup_bytes) (function)
- [usb_setup_kind](#usb_setup_kind) (function)
- [usb_setup_recipient](#usb_setup_recipient) (function)
- [usb_setup_in](#usb_setup_in) (function)
- [UsbResp](#UsbResp) (struct)
- [UsbDevice](#UsbDevice) (struct)
- [usbdev_new](#usbdev_new) (function)
- [usbdev_bus_reset](#usbdev_bus_reset) (function)
- [usbdev_setup](#usbdev_setup) (function)
- [usbdev_status_done](#usbdev_status_done) (function)
- [usb_line_coding](#usb_line_coding) (function)
- [usb_line_coding_str](#usb_line_coding_str) (function)
- [usb_cdc_serial_state](#usb_cdc_serial_state) (function)
- [usb_ep0_packets](#usb_ep0_packets) (function)

## USB_DESC_DEVICE:

```fides
pub const USB_DESC_DEVICE: i64 = 1;
```

*Defined at line 36*

---

## USB_DESC_CONFIG:

```fides
pub const USB_DESC_CONFIG: i64 = 2;
```

*Defined at line 37*

---

## USB_DESC_STRING:

```fides
pub const USB_DESC_STRING: i64 = 3;
```

*Defined at line 38*

---

## USB_DESC_INTERFACE:

```fides
pub const USB_DESC_INTERFACE: i64 = 4;
```

*Defined at line 39*

---

## USB_DESC_ENDPOINT:

```fides
pub const USB_DESC_ENDPOINT: i64 = 5;
```

*Defined at line 40*

---

## USB_DESC_QUALIFIER:

```fides
pub const USB_DESC_QUALIFIER: i64 = 6;
```

*Defined at line 41*

---

## USB_DESC_IAD:

```fides
pub const USB_DESC_IAD: i64 = 11;
```

*Defined at line 42*

---

## USB_DESC_CS_INTERFACE:

```fides
pub const USB_DESC_CS_INTERFACE: i64 = 0x24;
```

*Defined at line 43*

---

## USB_REQ_GET_STATUS:

```fides
pub const USB_REQ_GET_STATUS: i64 = 0;
```

*Defined at line 45*

---

## USB_REQ_CLEAR_FEATURE:

```fides
pub const USB_REQ_CLEAR_FEATURE: i64 = 1;
```

*Defined at line 46*

---

## USB_REQ_SET_FEATURE:

```fides
pub const USB_REQ_SET_FEATURE: i64 = 3;
```

*Defined at line 47*

---

## USB_REQ_SET_ADDRESS:

```fides
pub const USB_REQ_SET_ADDRESS: i64 = 5;
```

*Defined at line 48*

---

## USB_REQ_GET_DESCRIPTOR:

```fides
pub const USB_REQ_GET_DESCRIPTOR: i64 = 6;
```

*Defined at line 49*

---

## USB_REQ_SET_DESCRIPTOR:

```fides
pub const USB_REQ_SET_DESCRIPTOR: i64 = 7;
```

*Defined at line 50*

---

## USB_REQ_GET_CONFIGURATION:

```fides
pub const USB_REQ_GET_CONFIGURATION: i64 = 8;
```

*Defined at line 51*

---

## USB_REQ_SET_CONFIGURATION:

```fides
pub const USB_REQ_SET_CONFIGURATION: i64 = 9;
```

*Defined at line 52*

---

## USB_REQ_GET_INTERFACE:

```fides
pub const USB_REQ_GET_INTERFACE: i64 = 10;
```

*Defined at line 53*

---

## USB_REQ_SET_INTERFACE:

```fides
pub const USB_REQ_SET_INTERFACE: i64 = 11;
```

*Defined at line 54*

---

## USB_FEAT_ENDPOINT_HALT:

```fides
pub const USB_FEAT_ENDPOINT_HALT: i64 = 0;
```

*Defined at line 56*

---

## USB_FEAT_REMOTE_WAKEUP:

```fides
pub const USB_FEAT_REMOTE_WAKEUP: i64 = 1;
```

*Defined at line 57*

---

## CDC_SET_LINE_CODING:

```fides
pub const CDC_SET_LINE_CODING: i64 = 0x20;
```

*Defined at line 59*

---

## CDC_GET_LINE_CODING:

```fides
pub const CDC_GET_LINE_CODING: i64 = 0x21;
```

*Defined at line 60*

---

## CDC_SET_CONTROL_LINE_STATE:

```fides
pub const CDC_SET_CONTROL_LINE_STATE: i64 = 0x22;
```

*Defined at line 61*

---

## CDC_SEND_BREAK:

```fides
pub const CDC_SEND_BREAK: i64 = 0x23;
```

*Defined at line 62*

---

## CDC_SERIAL_STATE:

```fides
pub const CDC_SERIAL_STATE: i64 = 0x20;
```

*Defined at line 63*

---

## USB_STATE_DEFAULT:

```fides
pub const USB_STATE_DEFAULT: i64 = 0;
```

*Defined at line 65*

---

## USB_STATE_ADDRESS:

```fides
pub const USB_STATE_ADDRESS: i64 = 1;
```

*Defined at line 66*

---

## USB_STATE_CONFIGURED:

```fides
pub const USB_STATE_CONFIGURED: i64 = 2;
```

*Defined at line 67*

---

## USB_EP_CONTROL:

```fides
pub const USB_EP_CONTROL: i64 = 0;
```

*Defined at line 69*

---

## USB_EP_ISO:

```fides
pub const USB_EP_ISO: i64 = 1;
```

*Defined at line 70*

---

## USB_EP_BULK:

```fides
pub const USB_EP_BULK: i64 = 2;
```

*Defined at line 71*

---

## USB_EP_INTERRUPT:

```fides
pub const USB_EP_INTERRUPT: i64 = 3;
```

*Defined at line 72*

---

## usb_state_name

```fides
pub fn usb_state_name(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 74*

---

## usb_device_desc

```fides
pub fn usb_device_desc(bcd_usb: i64, cls: i64, sub: i64, proto: i64, mps0: i64, vid: i64, pid: i64, bcd_dev: i64, i_manuf: i64, i_product: i64, i_serial: i64, n_configs: i64) -> Vec<i64> {
```

**Parameters:**

- `bcd_usb`: `i64`
- `cls`: `i64`
- `sub`: `i64`
- `proto`: `i64`
- `mps0`: `i64`
- `vid`: `i64`
- `pid`: `i64`
- `bcd_dev`: `i64`
- `i_manuf`: `i64`
- `i_product`: `i64`
- `i_serial`: `i64`
- `n_configs`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 91*

---

## usb_config_desc

The 9-byte configuration header; usb_config_finish sets wTotalLength
and bNumInterfaces.

```fides
pub fn usb_config_desc(value: i64, i_config: i64, attributes: i64, max_power_ma: i64) -> Vec<i64> {
```

**Parameters:**

- `value`: `i64`
- `i_config`: `i64`
- `attributes`: `i64`
- `max_power_ma`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 112*

---

## usb_interface_desc

```fides
pub fn usb_interface_desc(num: i64, alt: i64, n_eps: i64, cls: i64, sub: i64, proto: i64, i_str: i64) -> Vec<i64> {
```

**Parameters:**

- `num`: `i64`
- `alt`: `i64`
- `n_eps`: `i64`
- `cls`: `i64`
- `sub`: `i64`
- `proto`: `i64`
- `i_str`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 125*

---

## usb_endpoint_desc

```fides
pub fn usb_endpoint_desc(addr: i64, ep_type: i64, mps: i64, interval: i64) -> Vec<i64> {
```

**Parameters:**

- `addr`: `i64`
- `ep_type`: `i64`
- `mps`: `i64`
- `interval`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 139*

---

## usb_iad_desc

```fides
pub fn usb_iad_desc(first: i64, count: i64, cls: i64, sub: i64, proto: i64, i_str: i64) -> Vec<i64> {
```

**Parameters:**

- `first`: `i64`
- `count`: `i64`
- `cls`: `i64`
- `sub`: `i64`
- `proto`: `i64`
- `i_str`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 150*

---

## usb_config_finish

Append descriptors to a configuration and fill in its header.

```fides
pub fn usb_config_finish(cfg: Vec<i64>, parts: Vec<Vec<i64>>) -> Vec<i64> {
```

**Parameters:**

- `cfg`: `Vec<i64>`
- `parts`: `Vec<Vec<i64>>`

**Returns:** `Vec<i64>`

*Defined at line 164*

---

## usb_string_desc

String descriptor of `s` (ASCII/Latin-1 as UTF-16LE).

```fides
pub fn usb_string_desc(s: str) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<i64>`

*Defined at line 183*

---

## usb_langid_desc

String descriptor 0: the supported language IDs.

```fides
pub fn usb_langid_desc(langs: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `langs`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 197*

---

## usb_cdc_acm_config

The CDC-ACM virtual COM port: IAD, communication interface 0 with
header/call-management/ACM/union functional descriptors and an
interrupt IN notification endpoint, data interface 1 with bulk OUT and
bulk IN endpoints.

```fides
pub fn usb_cdc_acm_config(notif_ep: i64, out_ep: i64, in_ep: i64, bulk_mps: i64) -> Vec<i64> {
```

**Parameters:**

- `notif_ep`: `i64`
- `out_ep`: `i64`
- `in_ep`: `i64`
- `bulk_mps`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 213*

---

## UsbSetup

```fides
pub struct UsbSetup {
```

**Fields:**

| Name | Type |
|------|------|
| `request_type` | `i64` |
| `request` | `i64` |
| `value` | `i64` |
| `index` | `i64` |
| `length` | `i64` |

*Defined at line 252*

---

## usb_setup_parse

```fides
pub fn usb_setup_parse(b: Vec<i64>) -> UsbSetup {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `UsbSetup`

*Defined at line 260*

---

## usb_setup_bytes

```fides
pub fn usb_setup_bytes(rt: i64, req: i64, value: i64, index: i64, length: i64) -> Vec<i64> {
```

**Parameters:**

- `rt`: `i64`
- `req`: `i64`
- `value`: `i64`
- `index`: `i64`
- `length`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 266*

---

## usb_setup_kind

0 standard, 1 class, 2 vendor.

```fides
pub fn usb_setup_kind(s: UsbSetup) -> i64 {
```

**Parameters:**

- `s`: `UsbSetup`

**Returns:** `i64`

*Defined at line 277*

---

## usb_setup_recipient

0 device, 1 interface, 2 endpoint, 3 other.

```fides
pub fn usb_setup_recipient(s: UsbSetup) -> i64 {
```

**Parameters:**

- `s`: `UsbSetup`

**Returns:** `i64`

*Defined at line 281*

---

## usb_setup_in

```fides
pub fn usb_setup_in(s: UsbSetup) -> bool {
```

**Parameters:**

- `s`: `UsbSetup`

**Returns:** `bool`

*Defined at line 284*

---

## UsbResp

```fides
pub struct UsbResp {
```

**Fields:**

| Name | Type |
|------|------|
| `stall` | `bool` |
| `data` | `Vec<i64>` |
| `why` | `str` |

*Defined at line 290*

---

## UsbDevice

```fides
pub struct UsbDevice {
```

**Fields:**

| Name | Type |
|------|------|
| `device_desc` | `Vec<i64>` |
| `config_desc` | `Vec<i64>` |
| `strings` | `Vec<Vec<i64>>` |
| `state` | `i64` |
| `address` | `i64` |
| `pending_address` | `i64` |
| `configuration` | `i64` |
| `self_powered` | `bool` |
| `remote_wakeup` | `bool` |
| `halted` | `HashMap<i64` |
| `endpoints` | `Vec<i64>` |
| `baud` | `i64` |
| `stop_bits` | `i64` |
| `parity` | `i64` |
| `data_bits` | `i64` |
| `dtr` | `bool` |
| `rts` | `bool` |
| `break_ms` | `i64` |

*Defined at line 296*

---

## usbdev_new

A device; `strings` are descriptors for indexes 0, 1, 2 ...

```fides
pub fn usbdev_new(device_desc: Vec<i64>, config_desc: Vec<i64>, strings: Vec<Vec<i64>>) -> UsbDevice {
```

**Parameters:**

- `device_desc`: `Vec<i64>`
- `config_desc`: `Vec<i64>`
- `strings`: `Vec<Vec<i64>>`

**Returns:** `UsbDevice`

*Defined at line 319*

---

## usbdev_bus_reset

```fides
pub fn usbdev_bus_reset(d: &mut UsbDevice) {
```

**Parameters:**

- `d`: `&mut UsbDevice`

*Defined at line 338*

---

## usbdev_setup

Handle a SETUP packet; `out_data` is the OUT data stage (empty for IN
or no-data requests). The reply's data is already cut to wLength.

```fides
pub fn usbdev_setup(d: &mut UsbDevice, setup: Vec<i64>, out_data: Vec<i64>) -> UsbResp {
```

**Parameters:**

- `d`: `&mut UsbDevice`
- `setup`: `Vec<i64>`
- `out_data`: `Vec<i64>`

**Returns:** `UsbResp`

*Defined at line 390*

---

## usbdev_status_done

The status stage completed: a pending SET_ADDRESS takes effect now.

```fides
pub fn usbdev_status_done(d: &mut UsbDevice) {
```

**Parameters:**

- `d`: `&mut UsbDevice`

*Defined at line 531*

---

## usb_line_coding

The 7-byte line coding structure.

```fides
pub fn usb_line_coding(baud: i64, stop_bits: i64, parity: i64, data_bits: i64) -> Vec<i64> {
```

**Parameters:**

- `baud`: `i64`
- `stop_bits`: `i64`
- `parity`: `i64`
- `data_bits`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 582*

---

## usb_line_coding_str

"115200 8N1" from the device's line coding.

```fides
pub fn usb_line_coding_str(d: &UsbDevice) -> str {
```

**Parameters:**

- `d`: `&UsbDevice`

**Returns:** `str`

*Defined at line 595*

---

## usb_cdc_serial_state

SERIAL_STATE notification (PSTN 6.5.4): 8-byte header + UART state bits
(DCD bit 0, DSR bit 1, break 2, ring 3, framing 4, parity 5, overrun 6).

```fides
pub fn usb_cdc_serial_state(interface: i64, state: i64) -> Vec<i64> {
```

**Parameters:**

- `interface`: `i64`
- `state`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 603*

---

## usb_ep0_packets

Split an IN data stage into EP0 packet sizes; a zero-length packet ends
it when the data is shorter than wLength and a multiple of mps.

```fides
pub fn usb_ep0_packets(len: i64, wlength: i64, mps: i64) -> Vec<i64> {
```

**Parameters:**

- `len`: `i64`
- `wlength`: `i64`
- `mps`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 616*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
