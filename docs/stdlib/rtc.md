[All modules](index.md)

# rtc -- Fides Documentation

## Contents

- [RtcTime](#RtcTime) (struct)
- [rtc_time](#rtc_time) (function)
- [rtc_bcd_to_bin](#rtc_bcd_to_bin) (function)
- [rtc_bin_to_bcd](#rtc_bin_to_bcd) (function)
- [rtc_is_leap](#rtc_is_leap) (function)
- [rtc_days_in_month](#rtc_days_in_month) (function)
- [rtc_weekday](#rtc_weekday) (function)
- [rtc_weekday_name](#rtc_weekday_name) (function)
- [rtc_day_of_year](#rtc_day_of_year) (function)
- [rtc_days_from_civil](#rtc_days_from_civil) (function)
- [rtc_civil_from_days](#rtc_civil_from_days) (function)
- [rtc_to_unix](#rtc_to_unix) (function)
- [rtc_from_unix](#rtc_from_unix) (function)
- [rtc_add_seconds](#rtc_add_seconds) (function)
- [rtc_valid](#rtc_valid) (function)
- [rtc_iso](#rtc_iso) (function)
- [rtc_diff](#rtc_diff) (function)
- [CMOS_SECONDS:](#CMOS_SECONDS:) (const)
- [CMOS_ALARM_SECONDS:](#CMOS_ALARM_SECONDS:) (const)
- [CMOS_MINUTES:](#CMOS_MINUTES:) (const)
- [CMOS_ALARM_MINUTES:](#CMOS_ALARM_MINUTES:) (const)
- [CMOS_HOURS:](#CMOS_HOURS:) (const)
- [CMOS_ALARM_HOURS:](#CMOS_ALARM_HOURS:) (const)
- [CMOS_WEEKDAY:](#CMOS_WEEKDAY:) (const)
- [CMOS_DAY:](#CMOS_DAY:) (const)
- [CMOS_MONTH:](#CMOS_MONTH:) (const)
- [CMOS_YEAR:](#CMOS_YEAR:) (const)
- [CMOS_STATUS_A:](#CMOS_STATUS_A:) (const)
- [CMOS_STATUS_B:](#CMOS_STATUS_B:) (const)
- [CMOS_STATUS_C:](#CMOS_STATUS_C:) (const)
- [CMOS_STATUS_D:](#CMOS_STATUS_D:) (const)
- [CMOS_CENTURY:](#CMOS_CENTURY:) (const)
- [CMOS_A_UIP:](#CMOS_A_UIP:) (const)
- [CMOS_B_SET:](#CMOS_B_SET:) (const)
- [CMOS_B_PIE:](#CMOS_B_PIE:) (const)
- [CMOS_B_AIE:](#CMOS_B_AIE:) (const)
- [CMOS_B_UIE:](#CMOS_B_UIE:) (const)
- [CMOS_B_BINARY:](#CMOS_B_BINARY:) (const)
- [CMOS_B_24H:](#CMOS_B_24H:) (const)
- [CMOS_DONT_CARE:](#CMOS_DONT_CARE:) (const)
- [cmos_update_in_progress](#cmos_update_in_progress) (function)
- [cmos_snapshots_agree](#cmos_snapshots_agree) (function)
- [cmos_decode](#cmos_decode) (function)
- [cmos_encode](#cmos_encode) (function)
- [cmos_apply](#cmos_apply) (function)
- [cmos_irq_flags](#cmos_irq_flags) (function)
- [cmos_periodic_hz](#cmos_periodic_hz) (function)
- [RtcAlarm](#RtcAlarm) (struct)
- [rtc_alarm](#rtc_alarm) (function)
- [rtc_alarm_matches](#rtc_alarm_matches) (function)
- [rtc_alarm_next](#rtc_alarm_next) (function)
- [cmos_alarm_regs](#cmos_alarm_regs) (function)
- [rtc_prescalers](#rtc_prescalers) (function)

## RtcTime

```fides
pub struct RtcTime {
```

**Fields:**

| Name | Type |
|------|------|
| `year` | `i64` |
| `month` | `i64` |
| `day` | `i64` |
| `hour` | `i64` |
| `minute` | `i64` |
| `second` | `i64` |
| `weekday` | `i64` |

*Defined at line 30*

---

## rtc_time

A time; the weekday is computed (0 = Sunday).

```fides
pub fn rtc_time(year: i64, month: i64, day: i64, hour: i64, minute: i64, second: i64) -> RtcTime {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`
- `day`: `i64`
- `hour`: `i64`
- `minute`: `i64`
- `second`: `i64`

**Returns:** `RtcTime`

*Defined at line 41*

---

## rtc_bcd_to_bin

Packed BCD byte to binary; -1 when a nibble is not a decimal digit.

```fides
pub fn rtc_bcd_to_bin(b: i64) -> i64 {
```

**Parameters:**

- `b`: `i64`

**Returns:** `i64`

*Defined at line 50*

---

## rtc_bin_to_bcd

0..99 to packed BCD.

```fides
pub fn rtc_bin_to_bcd(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 60*

---

## rtc_is_leap

```fides
pub fn rtc_is_leap(y: i64) -> bool {
```

**Parameters:**

- `y`: `i64`

**Returns:** `bool`

*Defined at line 69*

---

## rtc_days_in_month

```fides
pub fn rtc_days_in_month(y: i64, m: i64) -> i64 {
```

**Parameters:**

- `y`: `i64`
- `m`: `i64`

**Returns:** `i64`

*Defined at line 73*

---

## rtc_weekday

0 = Sunday .. 6 = Saturday (Sakamoto's method).

```fides
pub fn rtc_weekday(y0: i64, m: i64, d: i64) -> i64 {
```

**Parameters:**

- `y0`: `i64`
- `m`: `i64`
- `d`: `i64`

**Returns:** `i64`

*Defined at line 87*

---

## rtc_weekday_name

```fides
pub fn rtc_weekday_name(w: i64) -> str {
```

**Parameters:**

- `w`: `i64`

**Returns:** `str`

*Defined at line 112*

---

## rtc_day_of_year

1-based day of the year.

```fides
pub fn rtc_day_of_year(y: i64, m: i64, d: i64) -> i64 {
```

**Parameters:**

- `y`: `i64`
- `m`: `i64`
- `d`: `i64`

**Returns:** `i64`

*Defined at line 118*

---

## rtc_days_from_civil

Days since 1970-01-01 (negative before).

```fides
pub fn rtc_days_from_civil(y0: i64, m: i64, d: i64) -> i64 {
```

**Parameters:**

- `y0`: `i64`
- `m`: `i64`
- `d`: `i64`

**Returns:** `i64`

*Defined at line 137*

---

## rtc_civil_from_days

The date of a day count since 1970-01-01, packed y * 10000 + m * 100 + d.

```fides
pub fn rtc_civil_from_days(z0: i64) -> i64 {
```

**Parameters:**

- `z0`: `i64`

**Returns:** `i64`

*Defined at line 154*

---

## rtc_to_unix

Unix time (seconds since 1970-01-01T00:00:00Z, no leap seconds).

```fides
pub fn rtc_to_unix(t: RtcTime) -> i64 {
```

**Parameters:**

- `t`: `RtcTime`

**Returns:** `i64`

*Defined at line 174*

---

## rtc_from_unix

```fides
pub fn rtc_from_unix(u: i64) -> RtcTime {
```

**Parameters:**

- `u`: `i64`

**Returns:** `RtcTime`

*Defined at line 178*

---

## rtc_add_seconds

```fides
pub fn rtc_add_seconds(t: RtcTime, s: i64) -> RtcTime {
```

**Parameters:**

- `t`: `RtcTime`
- `s`: `i64`

**Returns:** `RtcTime`

*Defined at line 185*

---

## rtc_valid

True when every field is in range for its month and year.

```fides
pub fn rtc_valid(t: RtcTime) -> bool {
```

**Parameters:**

- `t`: `RtcTime`

**Returns:** `bool`

*Defined at line 190*

---

## rtc_iso

"2024-02-29T13:45:00".

```fides
pub fn rtc_iso(t: RtcTime) -> str {
```

**Parameters:**

- `t`: `RtcTime`

**Returns:** `str`

*Defined at line 211*

---

## rtc_diff

Seconds from a to b.

```fides
pub fn rtc_diff(a: RtcTime, b: RtcTime) -> i64 {
```

**Parameters:**

- `a`: `RtcTime`
- `b`: `RtcTime`

**Returns:** `i64`

*Defined at line 220*

---

## CMOS_SECONDS:

```fides
pub const CMOS_SECONDS: i64 = 0x00;
```

*Defined at line 226*

---

## CMOS_ALARM_SECONDS:

```fides
pub const CMOS_ALARM_SECONDS: i64 = 0x01;
```

*Defined at line 227*

---

## CMOS_MINUTES:

```fides
pub const CMOS_MINUTES: i64 = 0x02;
```

*Defined at line 228*

---

## CMOS_ALARM_MINUTES:

```fides
pub const CMOS_ALARM_MINUTES: i64 = 0x03;
```

*Defined at line 229*

---

## CMOS_HOURS:

```fides
pub const CMOS_HOURS: i64 = 0x04;
```

*Defined at line 230*

---

## CMOS_ALARM_HOURS:

```fides
pub const CMOS_ALARM_HOURS: i64 = 0x05;
```

*Defined at line 231*

---

## CMOS_WEEKDAY:

```fides
pub const CMOS_WEEKDAY: i64 = 0x06;
```

*Defined at line 232*

---

## CMOS_DAY:

```fides
pub const CMOS_DAY: i64 = 0x07;
```

*Defined at line 233*

---

## CMOS_MONTH:

```fides
pub const CMOS_MONTH: i64 = 0x08;
```

*Defined at line 234*

---

## CMOS_YEAR:

```fides
pub const CMOS_YEAR: i64 = 0x09;
```

*Defined at line 235*

---

## CMOS_STATUS_A:

```fides
pub const CMOS_STATUS_A: i64 = 0x0A;
```

*Defined at line 236*

---

## CMOS_STATUS_B:

```fides
pub const CMOS_STATUS_B: i64 = 0x0B;
```

*Defined at line 237*

---

## CMOS_STATUS_C:

```fides
pub const CMOS_STATUS_C: i64 = 0x0C;
```

*Defined at line 238*

---

## CMOS_STATUS_D:

```fides
pub const CMOS_STATUS_D: i64 = 0x0D;
```

*Defined at line 239*

---

## CMOS_CENTURY:

```fides
pub const CMOS_CENTURY: i64 = 0x32;
```

*Defined at line 240*

---

## CMOS_A_UIP:

```fides
pub const CMOS_A_UIP: i64 = 0x80;
```

*Defined at line 242*

---

## CMOS_B_SET:

```fides
pub const CMOS_B_SET: i64 = 0x80;
```

*Defined at line 243*

---

## CMOS_B_PIE:

```fides
pub const CMOS_B_PIE: i64 = 0x40;
```

*Defined at line 244*

---

## CMOS_B_AIE:

```fides
pub const CMOS_B_AIE: i64 = 0x20;
```

*Defined at line 245*

---

## CMOS_B_UIE:

```fides
pub const CMOS_B_UIE: i64 = 0x10;
```

*Defined at line 246*

---

## CMOS_B_BINARY:

```fides
pub const CMOS_B_BINARY: i64 = 0x04;
```

*Defined at line 247*

---

## CMOS_B_24H:

```fides
pub const CMOS_B_24H: i64 = 0x02;
```

*Defined at line 248*

---

## CMOS_DONT_CARE:

Alarm "don't care" values are 0xC0..0xFF.

```fides
pub const CMOS_DONT_CARE: i64 = 0xC0;
```

*Defined at line 250*

---

## cmos_update_in_progress

A snapshot may be used only when it was read with UIP clear.

```fides
pub fn cmos_update_in_progress(regs: Vec<i64>) -> bool {
```

**Parameters:**

- `regs`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 253*

---

## cmos_snapshots_agree

The read discipline: two snapshots taken back to back are trusted when
neither saw UIP and the time registers agree.

```fides
pub fn cmos_snapshots_agree(a: Vec<i64>, b: Vec<i64>) -> bool {
```

**Parameters:**

- `a`: `Vec<i64>`
- `b`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 259*

---

## cmos_decode

Decode a snapshot (index = register number; the century register is
used when the snapshot reaches 0x32 and it is non-zero, else years
< 70 are 20xx). Hours in 12-hour mode carry PM in bit 7. Returns a
time with year 0 when a field is not valid BCD or out of range.

```fides
pub fn cmos_decode(regs: Vec<i64>) -> RtcTime {
```

**Parameters:**

- `regs`: `Vec<i64>`

**Returns:** `RtcTime`

*Defined at line 294*

---

## cmos_encode

Register writes that set the clock, as (register, value) pairs packed
reg * 256 + value, in the order a driver issues them: SET in status B
first (the clock stops updating), the time, the century, status B with
SET clear last.

```fides
pub fn cmos_encode(t: RtcTime, status_b: i64) -> Vec<i64> {
```

**Parameters:**

- `t`: `RtcTime`
- `status_b`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 353*

---

## cmos_apply

Apply packed register writes to a snapshot (what the chip would hold).

```fides
pub fn cmos_apply(regs: &mut Vec<i64>, writes: Vec<i64>) {
```

**Parameters:**

- `regs`: `&mut Vec<i64>`
- `writes`: `Vec<i64>`

*Defined at line 384*

---

## cmos_irq_flags

Status C bits: interrupt flags (reading clears them on the chip).

```fides
pub fn cmos_irq_flags(c: i64) -> str {
```

**Parameters:**

- `c`: `i64`

**Returns:** `str`

*Defined at line 394*

---

## cmos_periodic_hz

Periodic interrupt rate from status A's RS bits (3..15): 32768 >> (rs - 1) Hz.

```fides
pub fn cmos_periodic_hz(rs: i64) -> i64 {
```

**Parameters:**

- `rs`: `i64`

**Returns:** `i64`

*Defined at line 409*

---

## RtcAlarm

An alarm: each field -1 means "any". weekday 0 = Sunday.

```fides
pub struct RtcAlarm {
```

**Fields:**

| Name | Type |
|------|------|
| `day` | `i64` |
| `weekday` | `i64` |
| `hour` | `i64` |
| `minute` | `i64` |
| `second` | `i64` |

*Defined at line 419*

---

## rtc_alarm

```fides
pub fn rtc_alarm(day: i64, weekday: i64, hour: i64, minute: i64, second: i64) -> RtcAlarm {
```

**Parameters:**

- `day`: `i64`
- `weekday`: `i64`
- `hour`: `i64`
- `minute`: `i64`
- `second`: `i64`

**Returns:** `RtcAlarm`

*Defined at line 427*

---

## rtc_alarm_matches

```fides
pub fn rtc_alarm_matches(a: RtcAlarm, t: RtcTime) -> bool {
```

**Parameters:**

- `a`: `RtcAlarm`
- `t`: `RtcTime`

**Returns:** `bool`

*Defined at line 437*

---

## rtc_alarm_next

The first time strictly after `from` at which the alarm matches,
searched over 4 years (a 31st, or a 29th of February, comes round
within that). Year 0 when it does not fire within the window (a
field out of range, such as day 32 or hour 24).

```fides
pub fn rtc_alarm_next(a: RtcAlarm, from: RtcTime) -> RtcTime {
```

**Parameters:**

- `a`: `RtcAlarm`
- `from`: `RtcTime`

**Returns:** `RtcTime`

*Defined at line 477*

---

## cmos_alarm_regs

CMOS alarm registers (seconds, minutes, hours) for an alarm in BCD
24-hour mode: a "don't care" field is 0xC0.

```fides
pub fn cmos_alarm_regs(a: RtcAlarm) -> Vec<i64> {
```

**Parameters:**

- `a`: `RtcAlarm`

**Returns:** `Vec<i64>`

*Defined at line 506*

---

## rtc_prescalers

Split a crystal frequency into async (7-bit) and sync (15-bit)
prescalers giving exactly 1 Hz, async as large as possible (lowest
power): packed async * 65536 + sync, -1 when impossible.

```fides
pub fn rtc_prescalers(clk_hz: i64) -> i64 {
```

**Parameters:**

- `clk_hz`: `i64`

**Returns:** `i64`

*Defined at line 531*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
