[All modules](index.md)

# usb_hid -- Fides Documentation

## Contents

- [HID_INPUT:](#HID_INPUT:) (const)
- [HID_OUTPUT:](#HID_OUTPUT:) (const)
- [HID_FEATURE:](#HID_FEATURE:) (const)
- [HID_CONSTANT:](#HID_CONSTANT:) (const)
- [HID_VARIABLE:](#HID_VARIABLE:) (const)
- [HID_RELATIVE:](#HID_RELATIVE:) (const)
- [HID_PAGE_DESKTOP:](#HID_PAGE_DESKTOP:) (const)
- [HID_PAGE_KEYBOARD:](#HID_PAGE_KEYBOARD:) (const)
- [HID_PAGE_LED:](#HID_PAGE_LED:) (const)
- [HID_PAGE_BUTTON:](#HID_PAGE_BUTTON:) (const)
- [HID_PAGE_CONSUMER:](#HID_PAGE_CONSUMER:) (const)
- [HID_USAGE_POINTER:](#HID_USAGE_POINTER:) (const)
- [HID_USAGE_MOUSE:](#HID_USAGE_MOUSE:) (const)
- [HID_USAGE_KEYBOARD:](#HID_USAGE_KEYBOARD:) (const)
- [HID_USAGE_X:](#HID_USAGE_X:) (const)
- [HID_USAGE_Y:](#HID_USAGE_Y:) (const)
- [HID_USAGE_WHEEL:](#HID_USAGE_WHEEL:) (const)
- [HidField](#HidField) (struct)
- [HidDescriptor](#HidDescriptor) (struct)
- [hid_parse](#hid_parse) (function)
- [hid_report_bits](#hid_report_bits) (function)
- [hid_report_bytes](#hid_report_bytes) (function)
- [hid_field_usage](#hid_field_usage) (function)
- [hid_bits](#hid_bits) (function)
- [hid_field_value](#hid_field_value) (function)
- [hid_find](#hid_find) (function)
- [hid_usage_value](#hid_usage_value) (function)
- [HID_MOD_LCTRL:](#HID_MOD_LCTRL:) (const)
- [HID_MOD_LSHIFT:](#HID_MOD_LSHIFT:) (const)
- [HID_MOD_LALT:](#HID_MOD_LALT:) (const)
- [HID_MOD_LGUI:](#HID_MOD_LGUI:) (const)
- [HID_MOD_RCTRL:](#HID_MOD_RCTRL:) (const)
- [HID_MOD_RSHIFT:](#HID_MOD_RSHIFT:) (const)
- [HID_MOD_RALT:](#HID_MOD_RALT:) (const)
- [HID_MOD_RGUI:](#HID_MOD_RGUI:) (const)
- [HID_KEY_ROLLOVER:](#HID_KEY_ROLLOVER:) (const)
- [HID_KEY_ENTER:](#HID_KEY_ENTER:) (const)
- [HID_KEY_CAPS:](#HID_KEY_CAPS:) (const)
- [HID_LED_NUM:](#HID_LED_NUM:) (const)
- [HID_LED_CAPS:](#HID_LED_CAPS:) (const)
- [HID_LED_SCROLL:](#HID_LED_SCROLL:) (const)
- [HID_KEYS_PLAIN:](#HID_KEYS_PLAIN:) (const)
- [HID_KEYS_SHIFT:](#HID_KEYS_SHIFT:) (const)
- [hid_key_ascii](#hid_key_ascii) (function)
- [KeyboardReport](#KeyboardReport) (struct)
- [hid_kbd_decode](#hid_kbd_decode) (function)
- [KeyboardState](#KeyboardState) (struct)
- [hid_kbd_state_new](#hid_kbd_state_new) (function)
- [hid_kbd_update](#hid_kbd_update) (function)
- [hid_led_report](#hid_led_report) (function)
- [MouseReport](#MouseReport) (struct)
- [hid_mouse_decode](#hid_mouse_decode) (function)
- [HID_GET_REPORT:](#HID_GET_REPORT:) (const)
- [HID_GET_IDLE:](#HID_GET_IDLE:) (const)
- [HID_GET_PROTOCOL:](#HID_GET_PROTOCOL:) (const)
- [HID_SET_REPORT:](#HID_SET_REPORT:) (const)
- [HID_SET_IDLE:](#HID_SET_IDLE:) (const)
- [HID_SET_PROTOCOL:](#HID_SET_PROTOCOL:) (const)
- [hid_req_set_protocol](#hid_req_set_protocol) (function)
- [hid_req_set_idle](#hid_req_set_idle) (function)
- [hid_req_set_report](#hid_req_set_report) (function)
- [hid_req_get_report](#hid_req_get_report) (function)
- [hid_req_report_descriptor](#hid_req_report_descriptor) (function)
- [HidInfo](#HidInfo) (struct)
- [hid_find_in_config](#hid_find_in_config) (function)

## HID_INPUT:

```fides
pub const HID_INPUT: i64 = 0;
```

*Defined at line 24*

---

## HID_OUTPUT:

```fides
pub const HID_OUTPUT: i64 = 1;
```

*Defined at line 25*

---

## HID_FEATURE:

```fides
pub const HID_FEATURE: i64 = 2;
```

*Defined at line 26*

---

## HID_CONSTANT:

```fides
pub const HID_CONSTANT: i64 = 0x01;
```

*Defined at line 29*

---

## HID_VARIABLE:

```fides
pub const HID_VARIABLE: i64 = 0x02;
```

*Defined at line 30*

---

## HID_RELATIVE:

```fides
pub const HID_RELATIVE: i64 = 0x04;
```

*Defined at line 31*

---

## HID_PAGE_DESKTOP:

```fides
pub const HID_PAGE_DESKTOP: i64 = 0x01;
```

*Defined at line 34*

---

## HID_PAGE_KEYBOARD:

```fides
pub const HID_PAGE_KEYBOARD: i64 = 0x07;
```

*Defined at line 35*

---

## HID_PAGE_LED:

```fides
pub const HID_PAGE_LED: i64 = 0x08;
```

*Defined at line 36*

---

## HID_PAGE_BUTTON:

```fides
pub const HID_PAGE_BUTTON: i64 = 0x09;
```

*Defined at line 37*

---

## HID_PAGE_CONSUMER:

```fides
pub const HID_PAGE_CONSUMER: i64 = 0x0C;
```

*Defined at line 38*

---

## HID_USAGE_POINTER:

```fides
pub const HID_USAGE_POINTER: i64 = 0x01;
```

*Defined at line 41*

---

## HID_USAGE_MOUSE:

```fides
pub const HID_USAGE_MOUSE: i64 = 0x02;
```

*Defined at line 42*

---

## HID_USAGE_KEYBOARD:

```fides
pub const HID_USAGE_KEYBOARD: i64 = 0x06;
```

*Defined at line 43*

---

## HID_USAGE_X:

```fides
pub const HID_USAGE_X: i64 = 0x30;
```

*Defined at line 44*

---

## HID_USAGE_Y:

```fides
pub const HID_USAGE_Y: i64 = 0x31;
```

*Defined at line 45*

---

## HID_USAGE_WHEEL:

```fides
pub const HID_USAGE_WHEEL: i64 = 0x38;
```

*Defined at line 46*

---

## HidField

```fides
pub struct HidField {
```

**Fields:**

| Name | Type |
|------|------|
| `report_id` | `i64` |
| `kind` | `i64` |
| `flags` | `i64` |
| `usage_page` | `i64` |
| `usages` | `Vec<i64>` |
| `usage_min` | `i64` |
| `usage_max` | `i64` |
| `bit_offset` | `i64` |
| `size` | `i64` |
| `count` | `i64` |
| `logical_min` | `i64` |
| `logical_max` | `i64` |
| `collection` | `i64,  // usage of the innermost application/physical collection` |

*Defined at line 48*

---

## HidDescriptor

```fides
pub struct HidDescriptor {
```

**Fields:**

| Name | Type |
|------|------|
| `fields` | `Vec<HidField>` |
| `report_ids` | `Vec<i64>` |
| `uses_ids` | `bool` |
| `collections` | `Vec<i64>` |
| `error` | `str` |

*Defined at line 67*

---

## hid_parse

Parse a report descriptor.

```fides
pub fn hid_parse(d: Vec<i64>) -> HidDescriptor {
```

**Parameters:**

- `d`: `Vec<i64>`

**Returns:** `HidDescriptor`

*Defined at line 102*

---

## hid_report_bits

Bits of report `id` of a kind (without the report ID byte).

```fides
pub fn hid_report_bits(h: &HidDescriptor, id: i64, kind: i64) -> i64 {
```

**Parameters:**

- `h`: `&HidDescriptor`
- `id`: `i64`
- `kind`: `i64`

**Returns:** `i64`

*Defined at line 265*

---

## hid_report_bytes

Bytes on the wire, report ID byte included when IDs are used.

```fides
pub fn hid_report_bytes(h: &HidDescriptor, id: i64, kind: i64) -> i64 {
```

**Parameters:**

- `h`: `&HidDescriptor`
- `id`: `i64`
- `kind`: `i64`

**Returns:** `i64`

*Defined at line 279*

---

## hid_field_usage

Usage of element `idx` of a variable field.

```fides
pub fn hid_field_usage(f: &HidField, idx: i64) -> i64 {
```

**Parameters:**

- `f`: `&HidField`
- `idx`: `i64`

**Returns:** `i64`

*Defined at line 288*

---

## hid_bits

Raw bits at bit offset `off` (little-endian bit order), n <= 32.

```fides
pub fn hid_bits(r: Vec<i64>, off: i64, n: i64) -> i64 {
```

**Parameters:**

- `r`: `Vec<i64>`
- `off`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 302*

---

## hid_field_value

Element idx of field f in report r (r starts with the report ID byte
when the descriptor uses IDs); sign-extended when logical_min < 0.

```fides
pub fn hid_field_value(h: &HidDescriptor, f: &HidField, r: Vec<i64>, idx: i64) -> i64 {
```

**Parameters:**

- `h`: `&HidDescriptor`
- `f`: `&HidField`
- `r`: `Vec<i64>`
- `idx`: `i64`

**Returns:** `i64`

*Defined at line 318*

---

## hid_find

Index of the field of `kind` carrying usage (page << 16 | id), -1.
Array fields match when the usage is inside their range.

```fides
pub fn hid_find(h: &HidDescriptor, kind: i64, usage: i64) -> i64 {
```

**Parameters:**

- `h`: `&HidDescriptor`
- `kind`: `i64`
- `usage`: `i64`

**Returns:** `i64`

*Defined at line 332*

---

## hid_usage_value

Value of a variable usage in an input report (e.g. desktop X), or
`missing` when the descriptor has no such field.

```fides
pub fn hid_usage_value(h: &HidDescriptor, r: Vec<i64>, usage: i64, missing: i64) -> i64 {
```

**Parameters:**

- `h`: `&HidDescriptor`
- `r`: `Vec<i64>`
- `usage`: `i64`
- `missing`: `i64`

**Returns:** `i64`

*Defined at line 351*

---

## HID_MOD_LCTRL:

```fides
pub const HID_MOD_LCTRL: i64 = 0x01;
```

*Defined at line 373*

---

## HID_MOD_LSHIFT:

```fides
pub const HID_MOD_LSHIFT: i64 = 0x02;
```

*Defined at line 374*

---

## HID_MOD_LALT:

```fides
pub const HID_MOD_LALT: i64 = 0x04;
```

*Defined at line 375*

---

## HID_MOD_LGUI:

```fides
pub const HID_MOD_LGUI: i64 = 0x08;
```

*Defined at line 376*

---

## HID_MOD_RCTRL:

```fides
pub const HID_MOD_RCTRL: i64 = 0x10;
```

*Defined at line 377*

---

## HID_MOD_RSHIFT:

```fides
pub const HID_MOD_RSHIFT: i64 = 0x20;
```

*Defined at line 378*

---

## HID_MOD_RALT:

```fides
pub const HID_MOD_RALT: i64 = 0x40;
```

*Defined at line 379*

---

## HID_MOD_RGUI:

```fides
pub const HID_MOD_RGUI: i64 = 0x80;
```

*Defined at line 380*

---

## HID_KEY_ROLLOVER:

```fides
pub const HID_KEY_ROLLOVER: i64 = 0x01;
```

*Defined at line 382*

---

## HID_KEY_ENTER:

```fides
pub const HID_KEY_ENTER: i64 = 0x28;
```

*Defined at line 383*

---

## HID_KEY_CAPS:

```fides
pub const HID_KEY_CAPS: i64 = 0x39;
```

*Defined at line 384*

---

## HID_LED_NUM:

```fides
pub const HID_LED_NUM: i64 = 1;
```

*Defined at line 386*

---

## HID_LED_CAPS:

```fides
pub const HID_LED_CAPS: i64 = 2;
```

*Defined at line 387*

---

## HID_LED_SCROLL:

```fides
pub const HID_LED_SCROLL: i64 = 4;
```

*Defined at line 388*

---

## HID_KEYS_PLAIN:

US ASCII for keyboard usages 0x1E..0x38 (digits, Enter, Esc,
Backspace, Tab, Space, punctuation), unshifted and shifted.

```fides
pub const HID_KEYS_PLAIN: [i64; 27] = [49, 50, 51, 52, 53, 54, 55, 56, 57, 48, 10, 27, 8, 9, 32, 45, 61, 91, 93, 92, 35, 59, 39, 96, 44, 46, 47];
```

*Defined at line 392*

---

## HID_KEYS_SHIFT:

```fides
pub const HID_KEYS_SHIFT: [i64; 27] = [33, 64, 35, 36, 37, 94, 38, 42, 40, 41, 10, 27, 8, 9, 32, 95, 43, 123, 125, 124, 126, 58, 34, 126, 60, 62, 63];
```

*Defined at line 393*

---

## hid_key_ascii

US ASCII of a keyboard usage, shifted or not; 0 when not printable.

```fides
pub fn hid_key_ascii(usage: i64, shift: bool) -> i64 {
```

**Parameters:**

- `usage`: `i64`
- `shift`: `bool`

**Returns:** `i64`

*Defined at line 396*

---

## KeyboardReport

```fides
pub struct KeyboardReport {
```

**Fields:**

| Name | Type |
|------|------|
| `mods` | `i64` |
| `keys` | `Vec<i64>` |
| `rollover` | `bool` |

*Defined at line 412*

---

## hid_kbd_decode

```fides
pub fn hid_kbd_decode(r: Vec<i64>) -> KeyboardReport {
```

**Parameters:**

- `r`: `Vec<i64>`

**Returns:** `KeyboardReport`

*Defined at line 418*

---

## KeyboardState

```fides
pub struct KeyboardState {
```

**Fields:**

| Name | Type |
|------|------|
| `prev` | `KeyboardReport` |
| `leds` | `i64` |
| `pressed` | `Vec<i64>` |
| `released` | `Vec<i64>` |
| `text` | `Vec<i64>,  // ASCII typed by the last report` |

*Defined at line 436*

---

## hid_kbd_state_new

```fides
pub fn hid_kbd_state_new() -> KeyboardState {
```

**Returns:** `KeyboardState`

*Defined at line 445*

---

## hid_kbd_update

Apply a boot keyboard report: compute presses / releases against the
previous one (a rollover report changes nothing), toggle Caps Lock,
produce ASCII for new keys. Returns true when the LEDs changed (send
hid_led_report).

```fides
pub fn hid_kbd_update(s: &mut KeyboardState, r: Vec<i64>) -> bool {
```

**Parameters:**

- `s`: `&mut KeyboardState`
- `r`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 457*

---

## hid_led_report

The one-byte LED output report.

```fides
pub fn hid_led_report(leds: i64) -> Vec<i64> {
```

**Parameters:**

- `leds`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 504*

---

## MouseReport

```fides
pub struct MouseReport {
```

**Fields:**

| Name | Type |
|------|------|
| `buttons` | `i64` |
| `dx` | `i64` |
| `dy` | `i64` |
| `wheel` | `i64` |

*Defined at line 512*

---

## hid_mouse_decode

```fides
pub fn hid_mouse_decode(r: Vec<i64>) -> MouseReport {
```

**Parameters:**

- `r`: `Vec<i64>`

**Returns:** `MouseReport`

*Defined at line 519*

---

## HID_GET_REPORT:

```fides
pub const HID_GET_REPORT: i64 = 0x01;
```

*Defined at line 531*

---

## HID_GET_IDLE:

```fides
pub const HID_GET_IDLE: i64 = 0x02;
```

*Defined at line 532*

---

## HID_GET_PROTOCOL:

```fides
pub const HID_GET_PROTOCOL: i64 = 0x03;
```

*Defined at line 533*

---

## HID_SET_REPORT:

```fides
pub const HID_SET_REPORT: i64 = 0x09;
```

*Defined at line 534*

---

## HID_SET_IDLE:

```fides
pub const HID_SET_IDLE: i64 = 0x0A;
```

*Defined at line 535*

---

## HID_SET_PROTOCOL:

```fides
pub const HID_SET_PROTOCOL: i64 = 0x0B;
```

*Defined at line 536*

---

## hid_req_set_protocol

SET_PROTOCOL: 0 = boot, 1 = report.

```fides
pub fn hid_req_set_protocol(iface: i64, protocol: i64) -> Vec<i64> {
```

**Parameters:**

- `iface`: `i64`
- `protocol`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 552*

---

## hid_req_set_idle

SET_IDLE: duration in 4 ms units (0 = report only on change).

```fides
pub fn hid_req_set_idle(iface: i64, report_id: i64, duration_4ms: i64) -> Vec<i64> {
```

**Parameters:**

- `iface`: `i64`
- `report_id`: `i64`
- `duration_4ms`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 557*

---

## hid_req_set_report

SET_REPORT; type 1 input, 2 output, 3 feature.

```fides
pub fn hid_req_set_report(iface: i64, rtype: i64, report_id: i64, length: i64) -> Vec<i64> {
```

**Parameters:**

- `iface`: `i64`
- `rtype`: `i64`
- `report_id`: `i64`
- `length`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 562*

---

## hid_req_get_report

```fides
pub fn hid_req_get_report(iface: i64, rtype: i64, report_id: i64, length: i64) -> Vec<i64> {
```

**Parameters:**

- `iface`: `i64`
- `rtype`: `i64`
- `report_id`: `i64`
- `length`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 566*

---

## hid_req_report_descriptor

Standard GET_DESCRIPTOR for the report descriptor (type 0x22).

```fides
pub fn hid_req_report_descriptor(iface: i64, length: i64) -> Vec<i64> {
```

**Parameters:**

- `iface`: `i64`
- `length`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 571*

---

## HidInfo

```fides
pub struct HidInfo {
```

**Fields:**

| Name | Type |
|------|------|
| `found` | `bool` |
| `bcd_hid` | `i64` |
| `country` | `i64` |
| `report_len` | `i64` |
| `iface` | `i64` |
| `subclass` | `i64` |
| `protocol` | `i64,  // 1 = keyboard, 2 = mouse` |

*Defined at line 575*

---

## hid_find_in_config

Find the first HID interface (class 3) in a configuration descriptor
and its HID descriptor (type 0x21) with the report descriptor length.

```fides
pub fn hid_find_in_config(c: Vec<i64>) -> HidInfo {
```

**Parameters:**

- `c`: `Vec<i64>`

**Returns:** `HidInfo`

*Defined at line 588*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
