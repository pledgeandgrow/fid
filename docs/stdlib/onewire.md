[All modules](index.md)

# onewire -- Fides Documentation

## Contents

- [OW_READ_ROM:](#OW_READ_ROM:) (const)
- [OW_MATCH_ROM:](#OW_MATCH_ROM:) (const)
- [OW_SKIP_ROM:](#OW_SKIP_ROM:) (const)
- [OW_SEARCH_ROM:](#OW_SEARCH_ROM:) (const)
- [OW_ALARM_SEARCH:](#OW_ALARM_SEARCH:) (const)
- [DS18B20_FAMILY:](#DS18B20_FAMILY:) (const)
- [DS18B20_CONVERT_T:](#DS18B20_CONVERT_T:) (const)
- [DS18B20_WRITE_SCRATCHPAD:](#DS18B20_WRITE_SCRATCHPAD:) (const)
- [DS18B20_READ_SCRATCHPAD:](#DS18B20_READ_SCRATCHPAD:) (const)
- [DS18B20_COPY_SCRATCHPAD:](#DS18B20_COPY_SCRATCHPAD:) (const)
- [DS18B20_RECALL_E2:](#DS18B20_RECALL_E2:) (const)
- [DS18B20_READ_POWER:](#DS18B20_READ_POWER:) (const)
- [OW_STEP_LOW:](#OW_STEP_LOW:) (const)
- [OW_STEP_RELEASE:](#OW_STEP_RELEASE:) (const)
- [OW_STEP_SAMPLE:](#OW_STEP_SAMPLE:) (const)
- [ow_crc8](#ow_crc8) (function)
- [ow_rom_valid](#ow_rom_valid) (function)
- [ow_rom_make](#ow_rom_make) (function)
- [ow_rom_hex](#ow_rom_hex) (function)
- [OwTiming](#OwTiming) (struct)
- [ow_timing_standard](#ow_timing_standard) (function)
- [ow_timing_overdrive](#ow_timing_overdrive) (function)
- [OwStep](#OwStep) (struct)
- [ow_steps_reset](#ow_steps_reset) (function)
- [ow_steps_write](#ow_steps_write) (function)
- [ow_steps_read](#ow_steps_read) (function)
- [ow_steps_ns](#ow_steps_ns) (function)
- [OwPins](#OwPins) (struct)
- [ow_run_steps](#ow_run_steps) (function)
- [OWF_ROM:](#OWF_ROM:) (const)
- [OWF_SEARCH:](#OWF_SEARCH:) (const)
- [OWF_MATCH:](#OWF_MATCH:) (const)
- [OWF_SEND:](#OWF_SEND:) (const)
- [OWF_FUNC:](#OWF_FUNC:) (const)
- [OWF_RECV:](#OWF_RECV:) (const)
- [OWF_DONE:](#OWF_DONE:) (const)
- [OWF_IDLE:](#OWF_IDLE:) (const)
- [OwFakeDev](#OwFakeDev) (struct)
- [ow_fake_rom](#ow_fake_rom) (function)
- [ow_fake_ds18b20](#ow_fake_ds18b20) (function)
- [OW_BUS_FAKE:](#OW_BUS_FAKE:) (const)
- [OW_BUS_PINS:](#OW_BUS_PINS:) (const)
- [OneWire](#OneWire) (struct)
- [ow_bus_fake](#ow_bus_fake) (function)
- [ow_bus_pins](#ow_bus_pins) (function)
- [ow_fake_add](#ow_fake_add) (function)
- [ow_reset](#ow_reset) (function)
- [ow_touch_bit](#ow_touch_bit) (function)
- [ow_write_bit](#ow_write_bit) (function)
- [ow_read_bit](#ow_read_bit) (function)
- [ow_write_byte](#ow_write_byte) (function)
- [ow_read_byte](#ow_read_byte) (function)
- [ow_write_bytes](#ow_write_bytes) (function)
- [ow_read_bytes](#ow_read_bytes) (function)
- [ow_read_rom](#ow_read_rom) (function)
- [ow_select](#ow_select) (function)
- [OwSearch](#OwSearch) (struct)
- [ow_search_new](#ow_search_new) (function)
- [ow_search_family](#ow_search_family) (function)
- [ow_search_next](#ow_search_next) (function)
- [ow_search_all](#ow_search_all) (function)
- [ds18b20_raw_to_c10000](#ds18b20_raw_to_c10000) (function)
- [ds18b20_raw_to_celsius](#ds18b20_raw_to_celsius) (function)
- [ds18b20_celsius_to_raw](#ds18b20_celsius_to_raw) (function)
- [ds18b20_config](#ds18b20_config) (function)
- [ds18b20_resolution](#ds18b20_resolution) (function)
- [ds18b20_conversion_us](#ds18b20_conversion_us) (function)
- [ds18b20_convert](#ds18b20_convert) (function)
- [ds18b20_read_scratchpad](#ds18b20_read_scratchpad) (function)
- [ds18b20_read_temp](#ds18b20_read_temp) (function)
- [ds18b20_write_config](#ds18b20_write_config) (function)

## OW_READ_ROM:

```fides
pub const OW_READ_ROM: i64 = 0x33;
```

*Defined at line 25*

---

## OW_MATCH_ROM:

```fides
pub const OW_MATCH_ROM: i64 = 0x55;
```

*Defined at line 26*

---

## OW_SKIP_ROM:

```fides
pub const OW_SKIP_ROM: i64 = 0xCC;
```

*Defined at line 27*

---

## OW_SEARCH_ROM:

```fides
pub const OW_SEARCH_ROM: i64 = 0xF0;
```

*Defined at line 28*

---

## OW_ALARM_SEARCH:

```fides
pub const OW_ALARM_SEARCH: i64 = 0xEC;
```

*Defined at line 29*

---

## DS18B20_FAMILY:

```fides
pub const DS18B20_FAMILY: i64 = 0x28;
```

*Defined at line 31*

---

## DS18B20_CONVERT_T:

```fides
pub const DS18B20_CONVERT_T: i64 = 0x44;
```

*Defined at line 32*

---

## DS18B20_WRITE_SCRATCHPAD:

```fides
pub const DS18B20_WRITE_SCRATCHPAD: i64 = 0x4E;
```

*Defined at line 33*

---

## DS18B20_READ_SCRATCHPAD:

```fides
pub const DS18B20_READ_SCRATCHPAD: i64 = 0xBE;
```

*Defined at line 34*

---

## DS18B20_COPY_SCRATCHPAD:

```fides
pub const DS18B20_COPY_SCRATCHPAD: i64 = 0x48;
```

*Defined at line 35*

---

## DS18B20_RECALL_E2:

```fides
pub const DS18B20_RECALL_E2: i64 = 0xB8;
```

*Defined at line 36*

---

## DS18B20_READ_POWER:

```fides
pub const DS18B20_READ_POWER: i64 = 0xB4;
```

*Defined at line 37*

---

## OW_STEP_LOW:

```fides
pub const OW_STEP_LOW: i64 = 0;
```

*Defined at line 39*

---

## OW_STEP_RELEASE:

```fides
pub const OW_STEP_RELEASE: i64 = 1;
```

*Defined at line 40*

---

## OW_STEP_SAMPLE:

```fides
pub const OW_STEP_SAMPLE: i64 = 2;
```

*Defined at line 41*

---

## ow_crc8

Dallas/Maxim CRC-8 (polynomial 0x31 reflected = 0x8C, initial 0). A
ROM code or scratchpad with its CRC appended gives 0.

```fides
pub fn ow_crc8(bytes: &Vec<i64>) -> i64 {
```

**Parameters:**

- `bytes`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 47*

---

## ow_rom_valid

```fides
pub fn ow_rom_valid(rom: &Vec<i64>) -> bool {
```

**Parameters:**

- `rom`: `&Vec<i64>`

**Returns:** `bool`

*Defined at line 67*

---

## ow_rom_make

A ROM code from family and 48-bit serial, with its CRC.

```fides
pub fn ow_rom_make(family: i64, serial: i64) -> Vec<i64> {
```

**Parameters:**

- `family`: `i64`
- `serial`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 83*

---

## ow_rom_hex

```fides
pub fn ow_rom_hex(rom: &Vec<i64>) -> str {
```

**Parameters:**

- `rom`: `&Vec<i64>`

**Returns:** `str`

*Defined at line 95*

---

## OwTiming

Recommended times in ns (AN126): A write-1 low, B write-1 release,
C write-0 low, D write-0 recovery, E read sample delay, F read
recovery, G reset delay, H reset low, I presence sample, J reset end.

```fides
pub struct OwTiming {
```

**Fields:**

| Name | Type |
|------|------|
| `a` | `i64` |
| `b` | `i64` |
| `c` | `i64` |
| `d` | `i64` |
| `e` | `i64` |
| `f` | `i64` |
| `g` | `i64` |
| `h` | `i64` |
| `i` | `i64` |
| `j` | `i64` |

*Defined at line 112*

---

## ow_timing_standard

```fides
pub fn ow_timing_standard() -> OwTiming {
```

**Returns:** `OwTiming`

*Defined at line 125*

---

## ow_timing_overdrive

```fides
pub fn ow_timing_overdrive() -> OwTiming {
```

**Returns:** `OwTiming`

*Defined at line 131*

---

## OwStep

One step: do `action` on the line, then wait wait_ns.

```fides
pub struct OwStep {
```

**Fields:**

| Name | Type |
|------|------|
| `action` | `i64` |
| `wait_ns` | `i64` |

*Defined at line 138*

---

## ow_steps_reset

```fides
pub fn ow_steps_reset(t: &OwTiming) -> Vec<OwStep> {
```

**Parameters:**

- `t`: `&OwTiming`

**Returns:** `Vec<OwStep>`

*Defined at line 143*

---

## ow_steps_write

```fides
pub fn ow_steps_write(t: &OwTiming, bit: i64) -> Vec<OwStep> {
```

**Parameters:**

- `t`: `&OwTiming`
- `bit`: `i64`

**Returns:** `Vec<OwStep>`

*Defined at line 160*

---

## ow_steps_read

```fides
pub fn ow_steps_read(t: &OwTiming) -> Vec<OwStep> {
```

**Parameters:**

- `t`: `&OwTiming`

**Returns:** `Vec<OwStep>`

*Defined at line 180*

---

## ow_steps_ns

```fides
pub fn ow_steps_ns(steps: &Vec<OwStep>) -> i64 {
```

**Parameters:**

- `steps`: `&Vec<OwStep>`

**Returns:** `i64`

*Defined at line 194*

---

## OwPins

One GPIO, open drain with a pull-up.

```fides
pub struct OwPins {
```

**Fields:**

| Name | Type |
|------|------|
| `drive_low` | `fn(i64) -> i64` |
| `release` | `fn(i64) -> i64` |
| `read` | `fn(i64) -> i64` |
| `delay_ns` | `fn(i64) -> i64` |

*Defined at line 205*

---

## ow_run_steps

Run steps on the pins; the last sampled level (1 if none).

```fides
pub fn ow_run_steps(p: &OwPins, steps: &Vec<OwStep>) -> i64 {
```

**Parameters:**

- `p`: `&OwPins`
- `steps`: `&Vec<OwStep>`

**Returns:** `i64`

*Defined at line 213*

---

## OWF_ROM:

```fides
pub const OWF_ROM: i64 = 0;
```

*Defined at line 239*

---

## OWF_SEARCH:

```fides
pub const OWF_SEARCH: i64 = 1;
```

*Defined at line 240*

---

## OWF_MATCH:

```fides
pub const OWF_MATCH: i64 = 2;
```

*Defined at line 241*

---

## OWF_SEND:

```fides
pub const OWF_SEND: i64 = 3;
```

*Defined at line 242*

---

## OWF_FUNC:

```fides
pub const OWF_FUNC: i64 = 4;
```

*Defined at line 243*

---

## OWF_RECV:

```fides
pub const OWF_RECV: i64 = 5;
```

*Defined at line 244*

---

## OWF_DONE:

```fides
pub const OWF_DONE: i64 = 6;
```

*Defined at line 245*

---

## OWF_IDLE:

```fides
pub const OWF_IDLE: i64 = 7;
```

*Defined at line 246*

---

## OwFakeDev

```fides
pub struct OwFakeDev {
```

**Fields:**

| Name | Type |
|------|------|
| `rom` | `Vec<i64>` |
| `state` | `i64` |
| `after` | `i64` |
| `shift` | `i64` |
| `nbits` | `i64` |
| `pos` | `i64` |
| `phase` | `i64` |
| `out` | `Vec<i64>` |
| `recv` | `Vec<i64>` |
| `need` | `i64` |
| `is_ds18b20` | `bool` |
| `temp_raw` | `i64` |
| `scratch` | `Vec<i64>` |
| `eeprom` | `Vec<i64>` |

*Defined at line 248*

---

## ow_fake_rom

A device that only has a ROM code (DS2401 serial number style).

```fides
pub fn ow_fake_rom(rom: Vec<i64>) -> OwFakeDev {
```

**Parameters:**

- `rom`: `Vec<i64>`

**Returns:** `OwFakeDev`

*Defined at line 280*

---

## ow_fake_ds18b20

A DS18B20 that will measure temp_raw (1/16 C units); power-up
scratchpad 85 C, alarms TH=75 TL=70, 12-bit.

```fides
pub fn ow_fake_ds18b20(serial: i64, temp_raw: i64) -> OwFakeDev {
```

**Parameters:**

- `serial`: `i64`
- `temp_raw`: `i64`

**Returns:** `OwFakeDev`

*Defined at line 288*

---

## OW_BUS_FAKE:

```fides
pub const OW_BUS_FAKE: i64 = 0;
```

*Defined at line 482*

---

## OW_BUS_PINS:

```fides
pub const OW_BUS_PINS: i64 = 1;
```

*Defined at line 483*

---

## OneWire

```fides
pub struct OneWire {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `timing` | `OwTiming` |
| `pins` | `OwPins` |
| `devs` | `Vec<OwFakeDev>` |
| `slots` | `i64` |
| `bus_ns` | `i64` |

*Defined at line 485*

---

## ow_bus_fake

```fides
pub fn ow_bus_fake() -> OneWire {
```

**Returns:** `OneWire`

*Defined at line 498*

---

## ow_bus_pins

```fides
pub fn ow_bus_pins(p: OwPins, t: OwTiming) -> OneWire {
```

**Parameters:**

- `p`: `OwPins`
- `t`: `OwTiming`

**Returns:** `OneWire`

*Defined at line 507*

---

## ow_fake_add

```fides
pub fn ow_fake_add(bus: &mut OneWire, d: OwFakeDev) {
```

**Parameters:**

- `bus`: `&mut OneWire`
- `d`: `OwFakeDev`

*Defined at line 513*

---

## ow_reset

Reset pulse; true when at least one device answers with presence.

```fides
pub fn ow_reset(bus: &mut OneWire) -> bool {
```

**Parameters:**

- `bus`: `&mut OneWire`

**Returns:** `bool`

*Defined at line 518*

---

## ow_touch_bit

One time slot: the master sends bit (1 = a read slot); the level of
the line after the devices had their say (wired-AND).

```fides
pub fn ow_touch_bit(bus: &mut OneWire, bit: i64) -> i64 {
```

**Parameters:**

- `bus`: `&mut OneWire`
- `bit`: `i64`

**Returns:** `i64`

*Defined at line 538*

---

## ow_write_bit

```fides
pub fn ow_write_bit(bus: &mut OneWire, bit: i64) {
```

**Parameters:**

- `bus`: `&mut OneWire`
- `bit`: `i64`

*Defined at line 569*

---

## ow_read_bit

```fides
pub fn ow_read_bit(bus: &mut OneWire) -> i64 {
```

**Parameters:**

- `bus`: `&mut OneWire`

**Returns:** `i64`

*Defined at line 573*

---

## ow_write_byte

```fides
pub fn ow_write_byte(bus: &mut OneWire, b: i64) {
```

**Parameters:**

- `bus`: `&mut OneWire`
- `b`: `i64`

*Defined at line 577*

---

## ow_read_byte

```fides
pub fn ow_read_byte(bus: &mut OneWire) -> i64 {
```

**Parameters:**

- `bus`: `&mut OneWire`

**Returns:** `i64`

*Defined at line 585*

---

## ow_write_bytes

```fides
pub fn ow_write_bytes(bus: &mut OneWire, bytes: &Vec<i64>) {
```

**Parameters:**

- `bus`: `&mut OneWire`
- `bytes`: `&Vec<i64>`

*Defined at line 595*

---

## ow_read_bytes

```fides
pub fn ow_read_bytes(bus: &mut OneWire, n: i64) -> Vec<i64> {
```

**Parameters:**

- `bus`: `&mut OneWire`
- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 603*

---

## ow_read_rom

READ ROM (only valid with one device on the bus): the code, or empty
when there is no presence or the CRC fails (several devices answered).

```fides
pub fn ow_read_rom(bus: &mut OneWire) -> Vec<i64> {
```

**Parameters:**

- `bus`: `&mut OneWire`

**Returns:** `Vec<i64>`

*Defined at line 617*

---

## ow_select

Reset and address one device (MATCH ROM), or all (SKIP ROM) when rom
is empty. False without presence.

```fides
pub fn ow_select(bus: &mut OneWire, rom: &Vec<i64>) -> bool {
```

**Parameters:**

- `bus`: `&mut OneWire`
- `rom`: `&Vec<i64>`

**Returns:** `bool`

*Defined at line 631*

---

## OwSearch

Search state (AN187).

```fides
pub struct OwSearch {
```

**Fields:**

| Name | Type |
|------|------|
| `rom` | `Vec<i64>` |
| `last_discrepancy` | `i64` |
| `last_family_discrepancy` | `i64` |
| `last_device` | `bool` |
| `command` | `i64` |

*Defined at line 645*

---

## ow_search_new

```fides
pub fn ow_search_new(command: i64) -> OwSearch {
```

**Parameters:**

- `command`: `i64`

**Returns:** `OwSearch`

*Defined at line 653*

---

## ow_search_family

Start the search at devices of one family (the first one found is of
that family if any is present).

```fides
pub fn ow_search_family(family: i64) -> OwSearch {
```

**Parameters:**

- `family`: `i64`

**Returns:** `OwSearch`

*Defined at line 661*

---

## ow_search_next

The next device; empty when the search is over.

```fides
pub fn ow_search_next(bus: &mut OneWire, s: &mut OwSearch) -> Vec<i64> {
```

**Parameters:**

- `bus`: `&mut OneWire`
- `s`: `&mut OwSearch`

**Returns:** `Vec<i64>`

*Defined at line 669*

---

## ow_search_all

Every device that answers `command` (SEARCH ROM or ALARM SEARCH).

```fides
pub fn ow_search_all(bus: &mut OneWire, command: i64) -> Vec<Vec<i64>> {
```

**Parameters:**

- `bus`: `&mut OneWire`
- `command`: `i64`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 731*

---

## ds18b20_raw_to_c10000

Temperature in 1/10000 C from the raw register (two's complement,
1/16 C per unit).

```fides
pub fn ds18b20_raw_to_c10000(raw: i64) -> i64 {
```

**Parameters:**

- `raw`: `i64`

**Returns:** `i64`

*Defined at line 753*

---

## ds18b20_raw_to_celsius

```fides
pub fn ds18b20_raw_to_celsius(raw: i64) -> f64 {
```

**Parameters:**

- `raw`: `i64`

**Returns:** `f64`

*Defined at line 761*

---

## ds18b20_celsius_to_raw

The register value for a temperature (nearest 1/16 C).

```fides
pub fn ds18b20_celsius_to_raw(c: f64) -> i64 {
```

**Parameters:**

- `c`: `f64`

**Returns:** `i64`

*Defined at line 766*

---

## ds18b20_config

Configuration register for 9..12 bits resolution.

```fides
pub fn ds18b20_config(bits: i64) -> i64 {
```

**Parameters:**

- `bits`: `i64`

**Returns:** `i64`

*Defined at line 778*

---

## ds18b20_resolution

```fides
pub fn ds18b20_resolution(config: i64) -> i64 {
```

**Parameters:**

- `config`: `i64`

**Returns:** `i64`

*Defined at line 789*

---

## ds18b20_conversion_us

Maximum conversion time in microseconds (93.75 ms at 9 bits, doubling).

```fides
pub fn ds18b20_conversion_us(bits: i64) -> i64 {
```

**Parameters:**

- `bits`: `i64`

**Returns:** `i64`

*Defined at line 794*

---

## ds18b20_convert

Start a conversion on one device (or all with an empty rom).

```fides
pub fn ds18b20_convert(bus: &mut OneWire, rom: &Vec<i64>) -> bool {
```

**Parameters:**

- `bus`: `&mut OneWire`
- `rom`: `&Vec<i64>`

**Returns:** `bool`

*Defined at line 799*

---

## ds18b20_read_scratchpad

Read the 9-byte scratchpad; empty when absent or the CRC is wrong.

```fides
pub fn ds18b20_read_scratchpad(bus: &mut OneWire, rom: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `bus`: `&mut OneWire`
- `rom`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 808*

---

## ds18b20_read_temp

Convert, then read: temperature in 1/10000 C, or an error.

```fides
pub fn ds18b20_read_temp(bus: &mut OneWire, rom: &Vec<i64>) -> Result<i64, str> {
```

**Parameters:**

- `bus`: `&mut OneWire`
- `rom`: `&Vec<i64>`

**Returns:** `Result<i64, str>`

*Defined at line 832*

---

## ds18b20_write_config

Write TH, TL and the resolution into the scratchpad.

```fides
pub fn ds18b20_write_config(bus: &mut OneWire, rom: &Vec<i64>, th: i64, tl: i64, bits: i64) -> bool {
```

**Parameters:**

- `bus`: `&mut OneWire`
- `rom`: `&Vec<i64>`
- `th`: `i64`
- `tl`: `i64`
- `bits`: `i64`

**Returns:** `bool`

*Defined at line 844*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
