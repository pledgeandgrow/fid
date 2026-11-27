[All modules](index.md)

# hpet -- Fides Documentation

## Contents

- [HPET_GCAP_ID:](#HPET_GCAP_ID:) (const)
- [HPET_GEN_CONF:](#HPET_GEN_CONF:) (const)
- [HPET_GINTR_STA:](#HPET_GINTR_STA:) (const)
- [HPET_MAIN_CNT:](#HPET_MAIN_CNT:) (const)
- [HPET_ENABLE_CNF:](#HPET_ENABLE_CNF:) (const)
- [HPET_LEG_RT_CNF:](#HPET_LEG_RT_CNF:) (const)
- [HPET_TN_LEVEL:](#HPET_TN_LEVEL:) (const)
- [HPET_TN_INT_ENB:](#HPET_TN_INT_ENB:) (const)
- [HPET_TN_PERIODIC:](#HPET_TN_PERIODIC:) (const)
- [HPET_TN_PER_CAP:](#HPET_TN_PER_CAP:) (const)
- [HPET_TN_SIZE_CAP:](#HPET_TN_SIZE_CAP:) (const)
- [HPET_TN_VAL_SET:](#HPET_TN_VAL_SET:) (const)
- [HPET_TN_32MODE:](#HPET_TN_32MODE:) (const)
- [HPET_TN_FSB_EN:](#HPET_TN_FSB_EN:) (const)
- [HPET_TN_FSB_CAP:](#HPET_TN_FSB_CAP:) (const)
- [HPET_MAX_PERIOD_FS:](#HPET_MAX_PERIOD_FS:) (const)
- [hpet_tn_conf](#hpet_tn_conf) (function)
- [hpet_tn_cmp](#hpet_tn_cmp) (function)
- [hpet_tn_fsb](#hpet_tn_fsb) (function)
- [HpetCaps](#HpetCaps) (struct)
- [hpet_caps_decode](#hpet_caps_decode) (function)
- [hpet_freq_hz](#hpet_freq_hz) (function)
- [hpet_ns_to_ticks](#hpet_ns_to_ticks) (function)
- [hpet_ticks_to_ns](#hpet_ticks_to_ns) (function)
- [HpetTimerCaps](#HpetTimerCaps) (struct)
- [hpet_timer_caps](#hpet_timer_caps) (function)
- [hpet_first_route](#hpet_first_route) (function)
- [hpet_muldiv](#hpet_muldiv) (function)
- [hpet_mul_shift](#hpet_mul_shift) (function)
- [MultShift](#MultShift) (struct)
- [hpet_calc_mult_shift](#hpet_calc_mult_shift) (function)
- [hpet_cyc2ns](#hpet_cyc2ns) (function)
- [HpetBus](#HpetBus) (struct)
- [hpet_read](#hpet_read) (function)
- [hpet_write](#hpet_write) (function)
- [hpet_caps](#hpet_caps) (function)
- [hpet_restart](#hpet_restart) (function)
- [hpet_counter](#hpet_counter) (function)
- [hpet_tn_value](#hpet_tn_value) (function)
- [hpet_oneshot](#hpet_oneshot) (function)
- [hpet_periodic](#hpet_periodic) (function)
- [hpet_ack](#hpet_ack) (function)
- [HpetTable](#HpetTable) (struct)
- [hpet_table_parse](#hpet_table_parse) (function)
- [tsc_freq_from](#tsc_freq_from) (function)
- [tsc_counter_delta](#tsc_counter_delta) (function)
- [tsc_calibrate_once](#tsc_calibrate_once) (function)
- [tsc_calibrate](#tsc_calibrate) (function)
- [PIT_HZ:](#PIT_HZ:) (const)
- [PIT_PORT_CH0:](#PIT_PORT_CH0:) (const)
- [PIT_PORT_CH2:](#PIT_PORT_CH2:) (const)
- [PIT_PORT_CMD:](#PIT_PORT_CMD:) (const)
- [PIT_PORT_GATE:](#PIT_PORT_GATE:) (const)
- [pit_command](#pit_command) (function)
- [pit_divisor](#pit_divisor) (function)
- [pit_ch2_latch](#pit_ch2_latch) (function)
- [pit_ch2_start](#pit_ch2_start) (function)
- [pit_ch0_periodic](#pit_ch0_periodic) (function)
- [pit_ch2_wait](#pit_ch2_wait) (function)

## HPET_GCAP_ID:

```fides
pub const HPET_GCAP_ID: i64 = 0x000;
```

*Defined at line 27*

---

## HPET_GEN_CONF:

```fides
pub const HPET_GEN_CONF: i64 = 0x010;
```

*Defined at line 28*

---

## HPET_GINTR_STA:

```fides
pub const HPET_GINTR_STA: i64 = 0x020;
```

*Defined at line 29*

---

## HPET_MAIN_CNT:

```fides
pub const HPET_MAIN_CNT: i64 = 0x0F0;
```

*Defined at line 30*

---

## HPET_ENABLE_CNF:

```fides
pub const HPET_ENABLE_CNF: i64 = 1;
```

*Defined at line 32*

---

## HPET_LEG_RT_CNF:

```fides
pub const HPET_LEG_RT_CNF: i64 = 2;
```

*Defined at line 33*

---

## HPET_TN_LEVEL:

```fides
pub const HPET_TN_LEVEL: i64 = 0x2;
```

*Defined at line 36*

---

## HPET_TN_INT_ENB:

```fides
pub const HPET_TN_INT_ENB: i64 = 0x4;
```

*Defined at line 37*

---

## HPET_TN_PERIODIC:

```fides
pub const HPET_TN_PERIODIC: i64 = 0x8;
```

*Defined at line 38*

---

## HPET_TN_PER_CAP:

```fides
pub const HPET_TN_PER_CAP: i64 = 0x10;
```

*Defined at line 39*

---

## HPET_TN_SIZE_CAP:

```fides
pub const HPET_TN_SIZE_CAP: i64 = 0x20;
```

*Defined at line 40*

---

## HPET_TN_VAL_SET:

```fides
pub const HPET_TN_VAL_SET: i64 = 0x40;
```

*Defined at line 41*

---

## HPET_TN_32MODE:

```fides
pub const HPET_TN_32MODE: i64 = 0x100;
```

*Defined at line 42*

---

## HPET_TN_FSB_EN:

```fides
pub const HPET_TN_FSB_EN: i64 = 0x4000;
```

*Defined at line 43*

---

## HPET_TN_FSB_CAP:

```fides
pub const HPET_TN_FSB_CAP: i64 = 0x8000;
```

*Defined at line 44*

---

## HPET_MAX_PERIOD_FS:

Largest legal period: 100 ns = 10^8 fs (counter at least 10 MHz).

```fides
pub const HPET_MAX_PERIOD_FS: i64 = 100000000;
```

*Defined at line 47*

---

## hpet_tn_conf

```fides
pub fn hpet_tn_conf(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 49*

---

## hpet_tn_cmp

```fides
pub fn hpet_tn_cmp(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 53*

---

## hpet_tn_fsb

```fides
pub fn hpet_tn_fsb(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 57*

---

## HpetCaps

```fides
pub struct HpetCaps {
```

**Fields:**

| Name | Type |
|------|------|
| `rev` | `i64` |
| `timers` | `i64` |
| `counter64` | `bool` |
| `legacy_capable` | `bool` |
| `vendor` | `i64` |
| `period_fs` | `i64` |
| `valid` | `bool,  // period non-zero and at most 100 ns` |

*Defined at line 63*

---

## hpet_caps_decode

```fides
pub fn hpet_caps_decode(v: i64) -> HpetCaps {
```

**Parameters:**

- `v`: `i64`

**Returns:** `HpetCaps`

*Defined at line 73*

---

## hpet_freq_hz

Counter frequency in Hz: 10^15 / period.

```fides
pub fn hpet_freq_hz(period_fs: i64) -> i64 {
```

**Parameters:**

- `period_fs`: `i64`

**Returns:** `i64`

*Defined at line 81*

---

## hpet_ns_to_ticks

Counter ticks in ns nanoseconds (1 ns = 10^6 fs), split to avoid
overflowing the product.

```fides
pub fn hpet_ns_to_ticks(ns: i64, period_fs: i64) -> i64 {
```

**Parameters:**

- `ns`: `i64`
- `period_fs`: `i64`

**Returns:** `i64`

*Defined at line 87*

---

## hpet_ticks_to_ns

Nanoseconds of `ticks` counter ticks.

```fides
pub fn hpet_ticks_to_ns(ticks: i64, period_fs: i64) -> i64 {
```

**Parameters:**

- `ticks`: `i64`
- `period_fs`: `i64`

**Returns:** `i64`

*Defined at line 94*

---

## HpetTimerCaps

```fides
pub struct HpetTimerCaps {
```

**Fields:**

| Name | Type |
|------|------|
| `periodic` | `bool` |
| `wide` | `bool` |
| `fsb` | `bool` |
| `routes` | `i64,  // bitmap of IO APIC inputs the timer can drive` |

*Defined at line 98*

---

## hpet_timer_caps

```fides
pub fn hpet_timer_caps(conf: i64) -> HpetTimerCaps {
```

**Parameters:**

- `conf`: `i64`

**Returns:** `HpetTimerCaps`

*Defined at line 106*

---

## hpet_first_route

Lowest IO APIC input a timer may be routed to, -1 when none.

```fides
pub fn hpet_first_route(routes: i64) -> i64 {
```

**Parameters:**

- `routes`: `i64`

**Returns:** `i64`

*Defined at line 113*

---

## hpet_muldiv

```fides
pub fn hpet_muldiv(a: i64, b: i64, c: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`
- `c`: `i64`

**Returns:** `i64`

*Defined at line 167*

---

## hpet_mul_shift

```fides
pub fn hpet_mul_shift(a: i64, b: i64, s: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`
- `s`: `i64`

**Returns:** `i64`

*Defined at line 189*

---

## MultShift

```fides
pub struct MultShift {
```

**Fields:**

| Name | Type |
|------|------|
| `mult` | `i64` |
| `shift` | `i64` |

*Defined at line 202*

---

## hpet_calc_mult_shift

mult/shift converting a clock of `from` Hz to `to` Hz (to = 10^9 for
ns) with values up to maxsec seconds not overflowing 64 bits.

```fides
pub fn hpet_calc_mult_shift(from: i64, to: i64, maxsec: i64) -> MultShift {
```

**Parameters:**

- `from`: `i64`
- `to`: `i64`
- `maxsec`: `i64`

**Returns:** `MultShift`

*Defined at line 209*

---

## hpet_cyc2ns

ns for `cycles` with a mult/shift pair: (cycles * mult) >> shift.

```fides
pub fn hpet_cyc2ns(cycles: i64, ms: MultShift) -> i64 {
```

**Parameters:**

- `cycles`: `i64`
- `ms`: `MultShift`

**Returns:** `i64`

*Defined at line 233*

---

## HpetBus

```fides
pub struct HpetBus {
```

**Fields:**

| Name | Type |
|------|------|
| `rd` | `fn(i64) -> i64` |
| `wr` | `fn(i64, i64) -> void` |

*Defined at line 239*

---

## hpet_read

```fides
pub fn hpet_read(b: &HpetBus, off: i64) -> i64 {
```

**Parameters:**

- `b`: `&HpetBus`
- `off`: `i64`

**Returns:** `i64`

*Defined at line 244*

---

## hpet_write

```fides
pub fn hpet_write(b: &HpetBus, off: i64, v: i64) {
```

**Parameters:**

- `b`: `&HpetBus`
- `off`: `i64`
- `v`: `i64`

*Defined at line 249*

---

## hpet_caps

```fides
pub fn hpet_caps(b: &HpetBus) -> HpetCaps {
```

**Parameters:**

- `b`: `&HpetBus`

**Returns:** `HpetCaps`

*Defined at line 254*

---

## hpet_restart

Stop the main counter, zero it, then start it again (legacy
replacement routing on request).

```fides
pub fn hpet_restart(b: &HpetBus, legacy: bool) {
```

**Parameters:**

- `b`: `&HpetBus`
- `legacy`: `bool`

*Defined at line 260*

---

## hpet_counter

```fides
pub fn hpet_counter(b: &HpetBus) -> i64 {
```

**Parameters:**

- `b`: `&HpetBus`

**Returns:** `i64`

*Defined at line 270*

---

## hpet_tn_value

Configuration value for timer n: interrupt to IO APIC input `route`
(edge triggered), periodic or one-shot. Keeps the read-only bits out.

```fides
pub fn hpet_tn_value(route: i64, periodic: bool, level: bool) -> i64 {
```

**Parameters:**

- `route`: `i64`
- `periodic`: `bool`
- `level`: `bool`

**Returns:** `i64`

*Defined at line 276*

---

## hpet_oneshot

```fides
pub fn hpet_oneshot(b: &HpetBus, n: i64, route: i64, ticks: i64) -> i64 {
```

**Parameters:**

- `b`: `&HpetBus`
- `n`: `i64`
- `route`: `i64`
- `ticks`: `i64`

**Returns:** `i64`

*Defined at line 289*

---

## hpet_periodic

Periodic timer n every `period` ticks (timer must have PER_CAP):
counter stopped and zeroed, VAL_SET written with the first deadline,
a second comparator write sets the period accumulator, counter on.
Returns false when the timer cannot run periodically.

```fides
pub fn hpet_periodic(b: &HpetBus, n: i64, route: i64, period: i64) -> bool {
```

**Parameters:**

- `b`: `&HpetBus`
- `n`: `i64`
- `route`: `i64`
- `period`: `i64`

**Returns:** `bool`

*Defined at line 302*

---

## hpet_ack

Acknowledge a level-triggered interrupt of timer n (write 1 to clear).

```fides
pub fn hpet_ack(b: &HpetBus, n: i64) {
```

**Parameters:**

- `b`: `&HpetBus`
- `n`: `i64`

*Defined at line 319*

---

## HpetTable

```fides
pub struct HpetTable {
```

**Fields:**

| Name | Type |
|------|------|
| `valid` | `bool` |
| `block_id` | `i64` |
| `address` | `i64` |
| `space` | `i64` |
| `number` | `i64` |
| `min_tick` | `i64` |

*Defined at line 325*

---

## hpet_table_parse

```fides
pub fn hpet_table_parse(t: Vec<i64>) -> HpetTable {
```

**Parameters:**

- `t`: `Vec<i64>`

**Returns:** `HpetTable`

*Defined at line 335*

---

## tsc_freq_from

Frequency of a counter that advanced `delta` while the reference
advanced ref_delta at ref_hz.

```fides
pub fn tsc_freq_from(delta: i64, ref_delta: i64, ref_hz: i64) -> i64 {
```

**Parameters:**

- `delta`: `i64`
- `ref_delta`: `i64`
- `ref_hz`: `i64`

**Returns:** `i64`

*Defined at line 351*

---

## tsc_counter_delta

```fides
pub fn tsc_counter_delta(a: i64, b: i64, bits: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`
- `bits`: `i64`

**Returns:** `i64`

*Defined at line 357*

---

## tsc_calibrate_once

One calibration run: wait until the reference counter (ref_bits wide,
ref_hz) moved by ref_ticks, measuring the TSC across it. Returns Hz.

```fides
pub fn tsc_calibrate_once(read_tsc: fn() -> i64, read_ref: fn() -> i64, ref_hz: i64, ref_bits: i64, ref_ticks: i64) -> i64 {
```

**Parameters:**

- `read_tsc`: `fn() -> i64`
- `read_ref`: `fn() -> i64`
- `ref_hz`: `i64`
- `ref_bits`: `i64`
- `ref_ticks`: `i64`

**Returns:** `i64`

*Defined at line 366*

---

## tsc_calibrate

Several runs, keeping the lowest result: an interrupt or SMI inside a
run only ever stretches the TSC side.

```fides
pub fn tsc_calibrate(read_tsc: fn() -> i64, read_ref: fn() -> i64, ref_hz: i64, ref_bits: i64, ref_ticks: i64, runs: i64) -> i64 {
```

**Parameters:**

- `read_tsc`: `fn() -> i64`
- `read_ref`: `fn() -> i64`
- `ref_hz`: `i64`
- `ref_bits`: `i64`
- `ref_ticks`: `i64`
- `runs`: `i64`

**Returns:** `i64`

*Defined at line 381*

---

## PIT_HZ:

```fides
pub const PIT_HZ: i64 = 1193182;
```

*Defined at line 396*

---

## PIT_PORT_CH0:

```fides
pub const PIT_PORT_CH0: i64 = 0x40;
```

*Defined at line 397*

---

## PIT_PORT_CH2:

```fides
pub const PIT_PORT_CH2: i64 = 0x42;
```

*Defined at line 398*

---

## PIT_PORT_CMD:

```fides
pub const PIT_PORT_CMD: i64 = 0x43;
```

*Defined at line 399*

---

## PIT_PORT_GATE:

```fides
pub const PIT_PORT_GATE: i64 = 0x61;
```

*Defined at line 400*

---

## pit_command

Command byte: channel (0..2), access (3 = lobyte then hibyte), mode
(0..5), binary counting.

```fides
pub fn pit_command(channel: i64, access: i64, mode: i64) -> i64 {
```

**Parameters:**

- `channel`: `i64`
- `access`: `i64`
- `mode`: `i64`

**Returns:** `i64`

*Defined at line 404*

---

## pit_divisor

Divisor for a periodic rate (rounded), clamped to 1..65536 (0 = 65536).

```fides
pub fn pit_divisor(hz: i64) -> i64 {
```

**Parameters:**

- `hz`: `i64`

**Returns:** `i64`

*Defined at line 409*

---

## pit_ch2_latch

Reload value for a channel-2 calibration window of ms milliseconds.

```fides
pub fn pit_ch2_latch(ms: i64) -> i64 {
```

**Parameters:**

- `ms`: `i64`

**Returns:** `i64`

*Defined at line 421*

---

## pit_ch2_start

Port writes, packed port * 256 + value, that start a channel-2
one-shot count of `latch` with the speaker off, given the current
value of port 0x61.

```fides
pub fn pit_ch2_start(latch: i64, port61: i64) -> Vec<i64> {
```

**Parameters:**

- `latch`: `i64`
- `port61`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 428*

---

## pit_ch0_periodic

Port writes for channel 0 as the periodic system tick (mode 2 rate
generator) at hz.

```fides
pub fn pit_ch0_periodic(hz: i64) -> Vec<i64> {
```

**Parameters:**

- `hz`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 439*

---

## pit_ch2_wait

Poll port 0x61 until OUT2 (bit 5) rises; returns the number of polls,
-1 after max_polls.

```fides
pub fn pit_ch2_wait(inb61: fn() -> i64, max_polls: i64) -> i64 {
```

**Parameters:**

- `inb61`: `fn() -> i64`
- `max_polls`: `i64`

**Returns:** `i64`

*Defined at line 450*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
