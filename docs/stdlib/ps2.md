[All modules](index.md)

# ps2 -- Fides Documentation

## Contents

- [PS2_DATA:](#PS2_DATA:) (const)
- [PS2_CMD:](#PS2_CMD:) (const)
- [PS2_ST_OBF:](#PS2_ST_OBF:) (const)
- [PS2_ST_IBF:](#PS2_ST_IBF:) (const)
- [PS2_ST_SYS:](#PS2_ST_SYS:) (const)
- [PS2_ST_CMD:](#PS2_ST_CMD:) (const)
- [PS2_ST_AUX:](#PS2_ST_AUX:) (const)
- [PS2_ST_TIMEOUT:](#PS2_ST_TIMEOUT:) (const)
- [PS2_ST_PARITY:](#PS2_ST_PARITY:) (const)
- [PS2C_READ_CONFIG:](#PS2C_READ_CONFIG:) (const)
- [PS2C_WRITE_CONFIG:](#PS2C_WRITE_CONFIG:) (const)
- [PS2C_DISABLE_P2:](#PS2C_DISABLE_P2:) (const)
- [PS2C_ENABLE_P2:](#PS2C_ENABLE_P2:) (const)
- [PS2C_TEST_P2:](#PS2C_TEST_P2:) (const)
- [PS2C_SELF_TEST:](#PS2C_SELF_TEST:) (const)
- [PS2C_TEST_P1:](#PS2C_TEST_P1:) (const)
- [PS2C_DISABLE_P1:](#PS2C_DISABLE_P1:) (const)
- [PS2C_ENABLE_P1:](#PS2C_ENABLE_P1:) (const)
- [PS2C_WRITE_P2:](#PS2C_WRITE_P2:) (const)
- [PS2C_RESET_CPU:](#PS2C_RESET_CPU:) (const)
- [PS2_CFG_IRQ1:](#PS2_CFG_IRQ1:) (const)
- [PS2_CFG_IRQ12:](#PS2_CFG_IRQ12:) (const)
- [PS2_CFG_SYS:](#PS2_CFG_SYS:) (const)
- [PS2_CFG_CLK1_OFF:](#PS2_CFG_CLK1_OFF:) (const)
- [PS2_CFG_CLK2_OFF:](#PS2_CFG_CLK2_OFF:) (const)
- [PS2_CFG_TRANSLATE:](#PS2_CFG_TRANSLATE:) (const)
- [KBD_SET_LEDS:](#KBD_SET_LEDS:) (const)
- [KBD_ECHO:](#KBD_ECHO:) (const)
- [KBD_SCANCODE_SET:](#KBD_SCANCODE_SET:) (const)
- [DEV_IDENTIFY:](#DEV_IDENTIFY:) (const)
- [KBD_TYPEMATIC:](#KBD_TYPEMATIC:) (const)
- [MOUSE_SAMPLE_RATE:](#MOUSE_SAMPLE_RATE:) (const)
- [DEV_ENABLE:](#DEV_ENABLE:) (const)
- [DEV_DISABLE:](#DEV_DISABLE:) (const)
- [DEV_DEFAULTS:](#DEV_DEFAULTS:) (const)
- [DEV_RESET:](#DEV_RESET:) (const)
- [MOUSE_RESOLUTION:](#MOUSE_RESOLUTION:) (const)
- [PS2_ACK:](#PS2_ACK:) (const)
- [PS2_RESEND:](#PS2_RESEND:) (const)
- [PS2_SELF_TEST_OK:](#PS2_SELF_TEST_OK:) (const)
- [PS2_CTRL_TEST_OK:](#PS2_CTRL_TEST_OK:) (const)
- [LED_SCROLL:](#LED_SCROLL:) (const)
- [LED_NUM:](#LED_NUM:) (const)
- [LED_CAPS:](#LED_CAPS:) (const)
- [Ps2Io](#Ps2Io) (struct)
- [ps2_status](#ps2_status) (function)
- [ps2_read](#ps2_read) (function)
- [ps2_flush](#ps2_flush) (function)
- [ps2_cmd](#ps2_cmd) (function)
- [ps2_write_data](#ps2_write_data) (function)
- [ps2_read_config](#ps2_read_config) (function)
- [ps2_write_config](#ps2_write_config) (function)
- [ps2_dev_send](#ps2_dev_send) (function)
- [ps2_dev_cmd_arg](#ps2_dev_cmd_arg) (function)
- [Ps2Info](#Ps2Info) (struct)
- [ps2_init](#ps2_init) (function)
- [ps2_kbd_set_leds](#ps2_kbd_set_leds) (function)
- [ps2_kbd_set_scancode_set](#ps2_kbd_set_scancode_set) (function)
- [ps2_kbd_get_scancode_set](#ps2_kbd_get_scancode_set) (function)
- [ps2_typematic_rate_x10](#ps2_typematic_rate_x10) (function)
- [ps2_typematic_byte](#ps2_typematic_byte) (function)
- [PS2_SET2_TO_SET1:](#PS2_SET2_TO_SET1:) (const)
- [ps2_set2_to_set1](#ps2_set2_to_set1) (function)
- [KeyEvent](#KeyEvent) (struct)
- [KbdDecoder](#KbdDecoder) (struct)
- [kbd_decoder_new](#kbd_decoder_new) (function)
- [kbd_feed](#kbd_feed) (function)
- [ps2_mouse_detect](#ps2_mouse_detect) (function)
- [ps2_mouse_enable](#ps2_mouse_enable) (function)
- [MousePacket](#MousePacket) (struct)
- [ps2_mouse_decode](#ps2_mouse_decode) (function)
- [MouseAssembler](#MouseAssembler) (struct)
- [mouse_assembler_new](#mouse_assembler_new) (function)
- [ps2_packet_size](#ps2_packet_size) (function)
- [mouse_feed](#mouse_feed) (function)
- [mouse_packet](#mouse_packet) (function)

## PS2_DATA:

```fides
pub const PS2_DATA: i64 = 0x60;
```

*Defined at line 25*

---

## PS2_CMD:

```fides
pub const PS2_CMD: i64 = 0x64;
```

*Defined at line 26*

---

## PS2_ST_OBF:

```fides
pub const PS2_ST_OBF: i64 = 0x01;
```

*Defined at line 29*

---

## PS2_ST_IBF:

```fides
pub const PS2_ST_IBF: i64 = 0x02;
```

*Defined at line 30*

---

## PS2_ST_SYS:

```fides
pub const PS2_ST_SYS: i64 = 0x04;
```

*Defined at line 31*

---

## PS2_ST_CMD:

```fides
pub const PS2_ST_CMD: i64 = 0x08;
```

*Defined at line 32*

---

## PS2_ST_AUX:

```fides
pub const PS2_ST_AUX: i64 = 0x20;
```

*Defined at line 33*

---

## PS2_ST_TIMEOUT:

```fides
pub const PS2_ST_TIMEOUT: i64 = 0x40;
```

*Defined at line 34*

---

## PS2_ST_PARITY:

```fides
pub const PS2_ST_PARITY: i64 = 0x80;
```

*Defined at line 35*

---

## PS2C_READ_CONFIG:

```fides
pub const PS2C_READ_CONFIG: i64 = 0x20;
```

*Defined at line 38*

---

## PS2C_WRITE_CONFIG:

```fides
pub const PS2C_WRITE_CONFIG: i64 = 0x60;
```

*Defined at line 39*

---

## PS2C_DISABLE_P2:

```fides
pub const PS2C_DISABLE_P2: i64 = 0xA7;
```

*Defined at line 40*

---

## PS2C_ENABLE_P2:

```fides
pub const PS2C_ENABLE_P2: i64 = 0xA8;
```

*Defined at line 41*

---

## PS2C_TEST_P2:

```fides
pub const PS2C_TEST_P2: i64 = 0xA9;
```

*Defined at line 42*

---

## PS2C_SELF_TEST:

```fides
pub const PS2C_SELF_TEST: i64 = 0xAA;
```

*Defined at line 43*

---

## PS2C_TEST_P1:

```fides
pub const PS2C_TEST_P1: i64 = 0xAB;
```

*Defined at line 44*

---

## PS2C_DISABLE_P1:

```fides
pub const PS2C_DISABLE_P1: i64 = 0xAD;
```

*Defined at line 45*

---

## PS2C_ENABLE_P1:

```fides
pub const PS2C_ENABLE_P1: i64 = 0xAE;
```

*Defined at line 46*

---

## PS2C_WRITE_P2:

```fides
pub const PS2C_WRITE_P2: i64 = 0xD4;
```

*Defined at line 47*

---

## PS2C_RESET_CPU:

```fides
pub const PS2C_RESET_CPU: i64 = 0xFE;
```

*Defined at line 48*

---

## PS2_CFG_IRQ1:

```fides
pub const PS2_CFG_IRQ1: i64 = 0x01;
```

*Defined at line 51*

---

## PS2_CFG_IRQ12:

```fides
pub const PS2_CFG_IRQ12: i64 = 0x02;
```

*Defined at line 52*

---

## PS2_CFG_SYS:

```fides
pub const PS2_CFG_SYS: i64 = 0x04;
```

*Defined at line 53*

---

## PS2_CFG_CLK1_OFF:

```fides
pub const PS2_CFG_CLK1_OFF: i64 = 0x10;
```

*Defined at line 54*

---

## PS2_CFG_CLK2_OFF:

```fides
pub const PS2_CFG_CLK2_OFF: i64 = 0x20;
```

*Defined at line 55*

---

## PS2_CFG_TRANSLATE:

```fides
pub const PS2_CFG_TRANSLATE: i64 = 0x40;
```

*Defined at line 56*

---

## KBD_SET_LEDS:

```fides
pub const KBD_SET_LEDS: i64 = 0xED;
```

*Defined at line 59*

---

## KBD_ECHO:

```fides
pub const KBD_ECHO: i64 = 0xEE;
```

*Defined at line 60*

---

## KBD_SCANCODE_SET:

```fides
pub const KBD_SCANCODE_SET: i64 = 0xF0;
```

*Defined at line 61*

---

## DEV_IDENTIFY:

```fides
pub const DEV_IDENTIFY: i64 = 0xF2;
```

*Defined at line 62*

---

## KBD_TYPEMATIC:

```fides
pub const KBD_TYPEMATIC: i64 = 0xF3;
```

*Defined at line 63*

---

## MOUSE_SAMPLE_RATE:

```fides
pub const MOUSE_SAMPLE_RATE: i64 = 0xF3;
```

*Defined at line 64*

---

## DEV_ENABLE:

```fides
pub const DEV_ENABLE: i64 = 0xF4;
```

*Defined at line 65*

---

## DEV_DISABLE:

```fides
pub const DEV_DISABLE: i64 = 0xF5;
```

*Defined at line 66*

---

## DEV_DEFAULTS:

```fides
pub const DEV_DEFAULTS: i64 = 0xF6;
```

*Defined at line 67*

---

## DEV_RESET:

```fides
pub const DEV_RESET: i64 = 0xFF;
```

*Defined at line 68*

---

## MOUSE_RESOLUTION:

```fides
pub const MOUSE_RESOLUTION: i64 = 0xE8;
```

*Defined at line 69*

---

## PS2_ACK:

```fides
pub const PS2_ACK: i64 = 0xFA;
```

*Defined at line 71*

---

## PS2_RESEND:

```fides
pub const PS2_RESEND: i64 = 0xFE;
```

*Defined at line 72*

---

## PS2_SELF_TEST_OK:

```fides
pub const PS2_SELF_TEST_OK: i64 = 0xAA;
```

*Defined at line 73*

---

## PS2_CTRL_TEST_OK:

```fides
pub const PS2_CTRL_TEST_OK: i64 = 0x55;
```

*Defined at line 74*

---

## LED_SCROLL:

```fides
pub const LED_SCROLL: i64 = 1;
```

*Defined at line 76*

---

## LED_NUM:

```fides
pub const LED_NUM: i64 = 2;
```

*Defined at line 77*

---

## LED_CAPS:

```fides
pub const LED_CAPS: i64 = 4;
```

*Defined at line 78*

---

## Ps2Io

```fides
pub struct Ps2Io {
```

**Fields:**

| Name | Type |
|------|------|
| `inb` | `fn(i64) -> i64` |
| `outb` | `fn(i64, i64) -> void` |
| `spins` | `i64,  // polls before a timeout` |

*Defined at line 82*

---

## ps2_status

```fides
pub fn ps2_status(io: &Ps2Io) -> i64 {
```

**Parameters:**

- `io`: `&Ps2Io`

**Returns:** `i64`

*Defined at line 88*

---

## ps2_read

Next byte from the output buffer, -1 on a timeout.

```fides
pub fn ps2_read(io: &Ps2Io) -> i64 {
```

**Parameters:**

- `io`: `&Ps2Io`

**Returns:** `i64`

*Defined at line 105*

---

## ps2_flush

Drop whatever is in the output buffer; returns how many bytes.

```fides
pub fn ps2_flush(io: &Ps2Io) -> i64 {
```

**Parameters:**

- `io`: `&Ps2Io`

**Returns:** `i64`

*Defined at line 118*

---

## ps2_cmd

```fides
pub fn ps2_cmd(io: &Ps2Io, c: i64) -> bool {
```

**Parameters:**

- `io`: `&Ps2Io`
- `c`: `i64`

**Returns:** `bool`

*Defined at line 128*

---

## ps2_write_data

```fides
pub fn ps2_write_data(io: &Ps2Io, b: i64) -> bool {
```

**Parameters:**

- `io`: `&Ps2Io`
- `b`: `i64`

**Returns:** `bool`

*Defined at line 137*

---

## ps2_read_config

```fides
pub fn ps2_read_config(io: &Ps2Io) -> i64 {
```

**Parameters:**

- `io`: `&Ps2Io`

**Returns:** `i64`

*Defined at line 146*

---

## ps2_write_config

```fides
pub fn ps2_write_config(io: &Ps2Io, v: i64) {
```

**Parameters:**

- `io`: `&Ps2Io`
- `v`: `i64`

*Defined at line 151*

---

## ps2_dev_send

Send a byte to a device (port 2 through 0xD4) and wait for its reply,
resending up to 3 times on 0xFE. Returns the reply or -1.

```fides
pub fn ps2_dev_send(io: &Ps2Io, port2: bool, b: i64) -> i64 {
```

**Parameters:**

- `io`: `&Ps2Io`
- `port2`: `bool`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 158*

---

## ps2_dev_cmd_arg

Command plus argument byte, both acknowledged.

```fides
pub fn ps2_dev_cmd_arg(io: &Ps2Io, port2: bool, c: i64, arg: i64) -> bool {
```

**Parameters:**

- `io`: `&Ps2Io`
- `port2`: `bool`
- `c`: `i64`
- `arg`: `i64`

**Returns:** `bool`

*Defined at line 175*

---

## Ps2Info

```fides
pub struct Ps2Info {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `dual` | `bool` |
| `port1` | `bool` |
| `port2` | `bool` |
| `kbd_present` | `bool` |
| `mouse_present` | `bool` |
| `config` | `i64` |
| `kbd_id` | `i64` |
| `mouse_id` | `i64` |

*Defined at line 184*

---

## ps2_init

The standard 8042 initialisation; leaves IRQs 1 and 12 enabled for
the ports that work, translation off (set 2 arrives raw) unless
`translate`.

```fides
pub fn ps2_init(io: &Ps2Io, translate: bool) -> Ps2Info {
```

**Parameters:**

- `io`: `&Ps2Io`
- `translate`: `bool`

**Returns:** `Ps2Info`

*Defined at line 200*

---

## ps2_kbd_set_leds

```fides
pub fn ps2_kbd_set_leds(io: &Ps2Io, leds: i64) -> bool {
```

**Parameters:**

- `io`: `&Ps2Io`
- `leds`: `i64`

**Returns:** `bool`

*Defined at line 265*

---

## ps2_kbd_set_scancode_set

Select scancode set 1, 2 or 3.

```fides
pub fn ps2_kbd_set_scancode_set(io: &Ps2Io, set: i64) -> bool {
```

**Parameters:**

- `io`: `&Ps2Io`
- `set`: `i64`

**Returns:** `bool`

*Defined at line 270*

---

## ps2_kbd_get_scancode_set

The current scancode set (as the keyboard reports it), -1 on error.

```fides
pub fn ps2_kbd_get_scancode_set(io: &Ps2Io) -> i64 {
```

**Parameters:**

- `io`: `&Ps2Io`

**Returns:** `i64`

*Defined at line 275*

---

## ps2_typematic_rate_x10

Typematic rate in tenths of characters per second for a 5-bit rate
code: 1 / ((8 + A) * 2^B * 4.17 ms), A = bits 2:0, B = bits 4:3.

```fides
pub fn ps2_typematic_rate_x10(code: i64) -> i64 {
```

**Parameters:**

- `code`: `i64`

**Returns:** `i64`

*Defined at line 295*

---

## ps2_typematic_byte

Typematic byte for a repeat rate (tenths of cps, nearest code) and a
delay in ms (250, 500, 750, 1000).

```fides
pub fn ps2_typematic_byte(rate_x10: i64, delay_ms: i64) -> i64 {
```

**Parameters:**

- `rate_x10`: `i64`
- `delay_ms`: `i64`

**Returns:** `i64`

*Defined at line 305*

---

## PS2_SET2_TO_SET1:

The 8042 translation table: scancode set 2 -> set 1, indexed by the
set-2 code (0x00..0x83); 0 = no key.

```fides
pub const PS2_SET2_TO_SET1: [i64; 132] = [0x00, 0x43, 0x00, 0x3F, 0x3D, 0x3B, 0x3C, 0x58, 0x00, 0x44, 0x42, 0x40, 0x3E, 0x0F, 0x29, 0x00, 0x00, 0x38, 0x2A, 0x00, 0x1D, 0x10, 0x02, 0x00, 0x00, 0x00, 0x2C, 0x1F, 0x1E, 0x11, 0x03, 0x00, 0x00, 0x2E, 0x2D, 0x20, 0x12, 0x05, 0x04, 0x00, 0x00, 0x39, 0x2F, 0x21, 0x14, 0x13, 0x06, 0x00, 0x00, 0x31, 0x30, 0x23, 0x22, 0x15, 0x07, 0x00, 0x00, 0x00, 0x32, 0x24, 0x16, 0x08, 0x09, 0x00, 0x00, 0x33, 0x25, 0x17, 0x18, 0x0B, 0x0A, 0x00, 0x00, 0x34, 0x35, 0x26, 0x27, 0x19, 0x0C, 0x00, 0x00, 0x00, 0x28, 0x00, 0x1A, 0x0D, 0x00, 0x00, 0x3A, 0x36, 0x1C, 0x1B, 0x00, 0x2B, 0x00, 0x00, 0x00, 0x56, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x4F, 0x00, 0x4B, 0x47, 0x00, 0x00, 0x00, 0x52, 0x53, 0x50, 0x4C, 0x4D, 0x48, 0x01, 0x45, 0x57, 0x4E, 0x51, 0x4A, 0x37, 0x49, 0x46, 0x00, 0x00, 0x00, 0x00, 0x41];
```

*Defined at line 332*

---

## ps2_set2_to_set1

Set-1 code of a set-2 code, 0 when none.

```fides
pub fn ps2_set2_to_set1(c: i64) -> i64 {
```

**Parameters:**

- `c`: `i64`

**Returns:** `i64`

*Defined at line 335*

---

## KeyEvent

```fides
pub struct KeyEvent {
```

**Fields:**

| Name | Type |
|------|------|
| `valid` | `bool` |
| `code` | `i64` |
| `extended` | `bool` |
| `pressed` | `bool` |
| `ascii` | `i64` |
| `pause` | `bool` |

*Defined at line 342*

---

## KbdDecoder

```fides
pub struct KbdDecoder {
```

**Fields:**

| Name | Type |
|------|------|
| `set` | `i64` |
| `ext` | `bool` |
| `brk` | `bool` |
| `skip` | `i64` |
| `shift` | `bool` |
| `ctrl` | `bool` |
| `alt` | `bool` |
| `leds` | `i64` |
| `leds_changed` | `bool` |

*Defined at line 355*

---

## kbd_decoder_new

```fides
pub fn kbd_decoder_new(set: i64) -> KbdDecoder {
```

**Parameters:**

- `set`: `i64`

**Returns:** `KbdDecoder`

*Defined at line 368*

---

## kbd_feed

Feed one byte from the keyboard; returns an event once a key's
sequence is complete.

```fides
pub fn kbd_feed(d: &mut KbdDecoder, b: i64) -> KeyEvent {
```

**Parameters:**

- `d`: `&mut KbdDecoder`
- `b`: `i64`

**Returns:** `KeyEvent`

*Defined at line 429*

---

## ps2_mouse_detect

Sample-rate knock and identify: 3 = IntelliMouse (wheel), 4 = 5-button.

```fides
pub fn ps2_mouse_detect(io: &Ps2Io) -> i64 {
```

**Parameters:**

- `io`: `&Ps2Io`

**Returns:** `i64`

*Defined at line 473*

---

## ps2_mouse_enable

```fides
pub fn ps2_mouse_enable(io: &Ps2Io) -> bool {
```

**Parameters:**

- `io`: `&Ps2Io`

**Returns:** `bool`

*Defined at line 492*

---

## MousePacket

```fides
pub struct MousePacket {
```

**Fields:**

| Name | Type |
|------|------|
| `left` | `bool` |
| `right` | `bool` |
| `middle` | `bool` |
| `button4` | `bool` |
| `button5` | `bool` |
| `dx` | `i64` |
| `dy` | `i64` |
| `dz` | `i64` |
| `overflow` | `bool` |

*Defined at line 496*

---

## ps2_mouse_decode

Decode a 3- or 4-byte packet starting at b[o] for device id `id`.

```fides
pub fn ps2_mouse_decode(b: Vec<i64>, o: i64, id: i64) -> MousePacket {
```

**Parameters:**

- `b`: `Vec<i64>`
- `o`: `i64`
- `id`: `i64`

**Returns:** `MousePacket`

*Defined at line 511*

---

## MouseAssembler

```fides
pub struct MouseAssembler {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `buf` | `Vec<i64>` |
| `dropped` | `i64` |

*Defined at line 535*

---

## mouse_assembler_new

```fides
pub fn mouse_assembler_new(id: i64) -> MouseAssembler {
```

**Parameters:**

- `id`: `i64`

**Returns:** `MouseAssembler`

*Defined at line 541*

---

## ps2_packet_size

```fides
pub fn ps2_packet_size(id: i64) -> i64 {
```

**Parameters:**

- `id`: `i64`

**Returns:** `i64`

*Defined at line 547*

---

## mouse_feed

Feed one byte (from IRQ 12); true when a whole packet is in a.buf.
A first byte without bit 3 set is out of sync and dropped.

```fides
pub fn mouse_feed(a: &mut MouseAssembler, byte: i64) -> bool {
```

**Parameters:**

- `a`: `&mut MouseAssembler`
- `byte`: `i64`

**Returns:** `bool`

*Defined at line 556*

---

## mouse_packet

```fides
pub fn mouse_packet(a: &MouseAssembler) -> MousePacket {
```

**Parameters:**

- `a`: `&MouseAssembler`

**Returns:** `MousePacket`

*Defined at line 568*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
