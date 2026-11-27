[All modules](index.md)

# time -- Fides Documentation

## Contents

- [LogicalTime](#LogicalTime) (struct)
- [time_from_timestamp](#time_from_timestamp) (function)
- [time_new](#time_new) (function)
- [time_from_date](#time_from_date) (function)
- [time_get_year](#time_get_year) (function)
- [time_get_month](#time_get_month) (function)
- [time_get_day](#time_get_day) (function)
- [time_get_hour](#time_get_hour) (function)
- [time_get_minute](#time_get_minute) (function)
- [time_get_second](#time_get_second) (function)
- [time_get_weekday](#time_get_weekday) (function)
- [time_add_seconds](#time_add_seconds) (function)
- [time_sub_seconds](#time_sub_seconds) (function)
- [time_add_minutes](#time_add_minutes) (function)
- [time_add_hours](#time_add_hours) (function)
- [time_add_days](#time_add_days) (function)
- [time_diff_seconds](#time_diff_seconds) (function)
- [time_diff_days](#time_diff_days) (function)
- [time_before](#time_before) (function)
- [time_after](#time_after) (function)
- [time_eq](#time_eq) (function)
- [time_before_eq](#time_before_eq) (function)
- [time_to_iso](#time_to_iso) (function)
- [time_to_date_string](#time_to_date_string) (function)
- [is_leap_year](#is_leap_year) (function)
- [time_get_day_of_year](#time_get_day_of_year) (function)
- [days_in_month](#days_in_month) (function)
- [clock_now_ns](#clock_now_ns) (function)
- [clock_now_us](#clock_now_us) (function)
- [clock_now_ms](#clock_now_ms) (function)
- [clock_elapsed_ns](#clock_elapsed_ns) (function)
- [clock_elapsed_us](#clock_elapsed_us) (function)
- [clock_elapsed_ms](#clock_elapsed_ms) (function)

## LogicalTime

A logical point in time. Internally stored as Unix seconds (i64).
All operations are deterministic — no system clock, no timezones.

```fides
pub struct LogicalTime {
```

**Fields:**

| Name | Type |
|------|------|
| `seconds` | `i64` |

*Defined at line 20*

---

## time_from_timestamp

```fides
pub fn time_from_timestamp(ts: i64) -> LogicalTime {
```

**Parameters:**

- `ts`: `i64`

**Returns:** `LogicalTime`

*Defined at line 32*

---

## time_new

```fides
pub fn time_new(year: i64, month: i64, day: i64, hour: i64, minute: i64, second: i64) -> LogicalTime {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`
- `day`: `i64`
- `hour`: `i64`
- `minute`: `i64`
- `second`: `i64`

**Returns:** `LogicalTime`

*Defined at line 48*

---

## time_from_date

```fides
pub fn time_from_date(year: i64, month: i64, day: i64) -> LogicalTime {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`
- `day`: `i64`

**Returns:** `LogicalTime`

*Defined at line 85*

---

## time_get_year

```fides
pub fn time_get_year(t: LogicalTime) -> i64 {
```

**Parameters:**

- `t`: `LogicalTime`

**Returns:** `i64`

*Defined at line 137*

---

## time_get_month

```fides
pub fn time_get_month(t: LogicalTime) -> i64 {
```

**Parameters:**

- `t`: `LogicalTime`

**Returns:** `i64`

*Defined at line 148*

---

## time_get_day

```fides
pub fn time_get_day(t: LogicalTime) -> i64 {
```

**Parameters:**

- `t`: `LogicalTime`

**Returns:** `i64`

*Defined at line 159*

---

## time_get_hour

```fides
pub fn time_get_hour(t: LogicalTime) -> i64 {
```

**Parameters:**

- `t`: `LogicalTime`

**Returns:** `i64`

*Defined at line 170*

---

## time_get_minute

```fides
pub fn time_get_minute(t: LogicalTime) -> i64 {
```

**Parameters:**

- `t`: `LogicalTime`

**Returns:** `i64`

*Defined at line 184*

---

## time_get_second

```fides
pub fn time_get_second(t: LogicalTime) -> i64 {
```

**Parameters:**

- `t`: `LogicalTime`

**Returns:** `i64`

*Defined at line 198*

---

## time_get_weekday

```fides
pub fn time_get_weekday(t: LogicalTime) -> i64 {
```

**Parameters:**

- `t`: `LogicalTime`

**Returns:** `i64`

*Defined at line 213*

---

## time_add_seconds

```fides
pub fn time_add_seconds(t: LogicalTime, seconds: i64) -> LogicalTime {
```

**Parameters:**

- `t`: `LogicalTime`
- `seconds`: `i64`

**Returns:** `LogicalTime`

*Defined at line 239*

---

## time_sub_seconds

```fides
pub fn time_sub_seconds(t: LogicalTime, seconds: i64) -> LogicalTime {
```

**Parameters:**

- `t`: `LogicalTime`
- `seconds`: `i64`

**Returns:** `LogicalTime`

*Defined at line 251*

---

## time_add_minutes

```fides
pub fn time_add_minutes(t: LogicalTime, minutes: i64) -> LogicalTime {
```

**Parameters:**

- `t`: `LogicalTime`
- `minutes`: `i64`

**Returns:** `LogicalTime`

*Defined at line 262*

---

## time_add_hours

```fides
pub fn time_add_hours(t: LogicalTime, hours: i64) -> LogicalTime {
```

**Parameters:**

- `t`: `LogicalTime`
- `hours`: `i64`

**Returns:** `LogicalTime`

*Defined at line 273*

---

## time_add_days

```fides
pub fn time_add_days(t: LogicalTime, days: i64) -> LogicalTime {
```

**Parameters:**

- `t`: `LogicalTime`
- `days`: `i64`

**Returns:** `LogicalTime`

*Defined at line 285*

---

## time_diff_seconds

```fides
pub fn time_diff_seconds(a: LogicalTime, b: LogicalTime) -> i64 {
```

**Parameters:**

- `a`: `LogicalTime`
- `b`: `LogicalTime`

**Returns:** `i64`

*Defined at line 298*

---

## time_diff_days

```fides
pub fn time_diff_days(a: LogicalTime, b: LogicalTime) -> i64 {
```

**Parameters:**

- `a`: `LogicalTime`
- `b`: `LogicalTime`

**Returns:** `i64`

*Defined at line 309*

---

## time_before

```fides
pub fn time_before(a: LogicalTime, b: LogicalTime) -> bool {
```

**Parameters:**

- `a`: `LogicalTime`
- `b`: `LogicalTime`

**Returns:** `bool`

*Defined at line 320*

---

## time_after

```fides
pub fn time_after(a: LogicalTime, b: LogicalTime) -> bool {
```

**Parameters:**

- `a`: `LogicalTime`
- `b`: `LogicalTime`

**Returns:** `bool`

*Defined at line 329*

---

## time_eq

```fides
pub fn time_eq(a: LogicalTime, b: LogicalTime) -> bool {
```

**Parameters:**

- `a`: `LogicalTime`
- `b`: `LogicalTime`

**Returns:** `bool`

*Defined at line 338*

---

## time_before_eq

```fides
pub fn time_before_eq(a: LogicalTime, b: LogicalTime) -> bool {
```

**Parameters:**

- `a`: `LogicalTime`
- `b`: `LogicalTime`

**Returns:** `bool`

*Defined at line 348*

---

## time_to_iso

```fides
pub fn time_to_iso(t: LogicalTime) -> str {
```

**Parameters:**

- `t`: `LogicalTime`

**Returns:** `str`

*Defined at line 387*

---

## time_to_date_string

```fides
pub fn time_to_date_string(t: LogicalTime) -> str {
```

**Parameters:**

- `t`: `LogicalTime`

**Returns:** `str`

*Defined at line 415*

---

## is_leap_year

```fides
pub fn is_leap_year(year: i64) -> bool {
```

**Parameters:**

- `year`: `i64`

**Returns:** `bool`

*Defined at line 437*

---

## time_get_day_of_year

```fides
pub fn time_get_day_of_year(t: LogicalTime) -> i64 {
```

**Parameters:**

- `t`: `LogicalTime`

**Returns:** `i64`

*Defined at line 456*

---

## days_in_month

```fides
pub fn days_in_month(year: i64, month: i64) -> i64 {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`

**Returns:** `i64`

*Defined at line 468*

---

## clock_now_ns

Get the current wall-clock time in nanoseconds since Unix epoch.
NOT deterministic — use for profiling only.

```fides
pub fn clock_now_ns() -> i64 {
```

**Returns:** `i64`

*Defined at line 521*

---

## clock_now_us

Get the current wall-clock time in microseconds since Unix epoch.
NOT deterministic — use for profiling only.

```fides
pub fn clock_now_us() -> i64 {
```

**Returns:** `i64`

*Defined at line 530*

---

## clock_now_ms

Get the current wall-clock time in milliseconds since Unix epoch.
NOT deterministic — use for profiling only.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var a = clock_now_ms();
var b = clock_now_ms();
assert(a > 0);
assert(b >= a);
```

```fides
pub fn clock_now_ms() -> i64 {
```

**Returns:** `i64`

*Defined at line 545*

---

## clock_elapsed_ns

Measure elapsed time in nanoseconds between two clock readings.
Usage: value start = clock_now_ns(); ...work...; value elapsed = clock_elapsed_ns(start);


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var start = clock_now_ns();
var e = clock_elapsed_ns(start);
assert(e >= 0);
```

```fides
pub fn clock_elapsed_ns(start: i64) -> i64 {
```

**Parameters:**

- `start`: `i64`

**Returns:** `i64`

*Defined at line 559*

---

## clock_elapsed_us

Measure elapsed time in microseconds between two clock readings.

```fides
pub fn clock_elapsed_us(start: i64) -> i64 {
```

**Parameters:**

- `start`: `i64`

**Returns:** `i64`

*Defined at line 564*

---

## clock_elapsed_ms

Measure elapsed time in milliseconds between two clock readings.

```fides
pub fn clock_elapsed_ms(start: i64) -> i64 {
```

**Parameters:**

- `start`: `i64`

**Returns:** `i64`

*Defined at line 569*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
