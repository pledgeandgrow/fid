# datetime — Fides Documentation

## Contents

- [dt_parse](#dt_parse) (function)
- [dt_parse_or_epoch](#dt_parse_or_epoch) (function)
- [dt_parse_time](#dt_parse_time) (function)
- [dt_is_valid](#dt_is_valid) (function)
- [dt_weekday_name](#dt_weekday_name) (function)
- [dt_weekday_short](#dt_weekday_short) (function)
- [dt_month_name](#dt_month_name) (function)
- [dt_month_short](#dt_month_short) (function)
- [dt_format](#dt_format) (function)
- [dt_to_iso](#dt_to_iso) (function)
- [dt_to_date](#dt_to_date) (function)
- [dt_to_rfc2822](#dt_to_rfc2822) (function)
- [dt_from_seconds](#dt_from_seconds) (function)
- [dt_to_seconds](#dt_to_seconds) (function)
- [dt_now](#dt_now) (function)
- [Duration](#duration) (struct)
- [dur_seconds](#dur_seconds) (function)
- [dur_minutes](#dur_minutes) (function)
- [dur_hours](#dur_hours) (function)
- [dur_days](#dur_days) (function)
- [dur_weeks](#dur_weeks) (function)
- [dur_millis](#dur_millis) (function)
- [dur_to_seconds](#dur_to_seconds) (function)
- [dur_to_millis](#dur_to_millis) (function)
- [dur_add](#dur_add) (function)
- [dur_sub](#dur_sub) (function)
- [dur_neg](#dur_neg) (function)
- [dur_abs](#dur_abs) (function)
- [dt_diff](#dt_diff) (function)
- [dt_add](#dt_add) (function)
- [dt_sub](#dt_sub) (function)
- [dur_parse](#dur_parse) (function)
- [dur_parse_or_zero](#dur_parse_or_zero) (function)
- [dur_format](#dur_format) (function)
- [dur_format_hms](#dur_format_hms) (function)

## dt_parse

Parse an ISO 8601 datetime to (ok, unix_seconds).

Accepted forms:
YYYY-MM-DD
YYYY-MM-DD HH:MM[:SS]            (space or 'T' separator)
YYYY-MM-DDTHH:MM[:SS][.frac]
any of the above + "Z" or ±HH / ±HHMM / ±HH:MM zone offset

The zone offset is applied (result is always UTC seconds).
Dates before 1970 produce negative seconds — fine.

```fides
pub fn dt_parse(s: str) -> (bool, i64) {
```

**Parameters:**

- `s`: `str`

**Returns:** `(bool, i64)`

*Defined at line 77*

---

## dt_parse_or_epoch

Parse an ISO 8601 datetime; returns the unix timestamp, or 0 on failure
(combine with dt_parse when the distinction matters).

```fides
pub fn dt_parse_or_epoch(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 179*

---

## dt_parse_time

Parse an ISO 8601 datetime to a LogicalTime; the epoch on failure.

```fides
pub fn dt_parse_time(s: str) -> LogicalTime {
```

**Parameters:**

- `s`: `str`

**Returns:** `LogicalTime`

*Defined at line 186*

---

## dt_is_valid

Is the string a valid ISO 8601 datetime in the forms dt_parse accepts?

```fides
pub fn dt_is_valid(s: str) -> bool {
```

**Parameters:**

- `s`: `str`

**Returns:** `bool`

*Defined at line 191*

---

## dt_weekday_name

Full weekday name ("Monday") for weekday index 0=Sunday..6=Saturday.

```fides
pub fn dt_weekday_name(wd: i64) -> str {
```

**Parameters:**

- `wd`: `i64`

**Returns:** `str`

*Defined at line 199*

---

## dt_weekday_short

Short weekday name ("Mon").

```fides
pub fn dt_weekday_short(wd: i64) -> str {
```

**Parameters:**

- `wd`: `i64`

**Returns:** `str`

*Defined at line 211*

---

## dt_month_name

Full month name ("January") for month 1..12.

```fides
pub fn dt_month_name(m: i64) -> str {
```

**Parameters:**

- `m`: `i64`

**Returns:** `str`

*Defined at line 216*

---

## dt_month_short

Short month name ("Jan").

```fides
pub fn dt_month_short(m: i64) -> str {
```

**Parameters:**

- `m`: `i64`

**Returns:** `str`

*Defined at line 233*

---

## dt_format

Format a LogicalTime with strftime-style codes.

Supported: %Y %y %m %d %e %H %I %M %S %p %A %a %B %b %j %F %T %R %D %z %Z %%
Unknown codes are emitted literally (the '%' is kept).

```fides
pub fn dt_format(t: LogicalTime, fmt: str) -> str {
```

**Parameters:**

- `t`: `LogicalTime`
- `fmt`: `str`

**Returns:** `str`

*Defined at line 257*

---

## dt_to_iso

ISO 8601 "YYYY-MM-DDTHH:MM:SSZ" (same as time_to_iso).

```fides
pub fn dt_to_iso(t: LogicalTime) -> str {
```

**Parameters:**

- `t`: `LogicalTime`

**Returns:** `str`

*Defined at line 333*

---

## dt_to_date

Date only "YYYY-MM-DD".

```fides
pub fn dt_to_date(t: LogicalTime) -> str {
```

**Parameters:**

- `t`: `LogicalTime`

**Returns:** `str`

*Defined at line 338*

---

## dt_to_rfc2822

RFC 2822 style: "Tue, 27 Oct 2026 14:30:00 +0000".

```fides
pub fn dt_to_rfc2822(t: LogicalTime) -> str {
```

**Parameters:**

- `t`: `LogicalTime`

**Returns:** `str`

*Defined at line 343*

---

## dt_from_seconds

Unix seconds -> LogicalTime.

```fides
pub fn dt_from_seconds(ts: i64) -> LogicalTime {
```

**Parameters:**

- `ts`: `i64`

**Returns:** `LogicalTime`

*Defined at line 354*

---

## dt_to_seconds

LogicalTime -> unix seconds.

```fides
pub fn dt_to_seconds(t: LogicalTime) -> i64 {
```

**Parameters:**

- `t`: `LogicalTime`

**Returns:** `i64`

*Defined at line 359*

---

## dt_now

The current wall-clock time as a LogicalTime (NOT deterministic —
reads CLOCK_REALTIME).

```fides
pub fn dt_now() -> LogicalTime {
```

**Returns:** `LogicalTime`

*Defined at line 365*

---

## Duration

A signed relative timespan in seconds.

```fides
pub struct Duration {
```

**Fields:**

| Name | Type |
|------|------|
| `seconds` | `i64` |

*Defined at line 372*

---

## dur_seconds

```fides
pub fn dur_seconds(n: i64) -> Duration { return Duration { seconds: n }; }
```

**Parameters:**

- `n`: `i64`

**Returns:** `Duration`

*Defined at line 376*

---

## dur_minutes

```fides
pub fn dur_minutes(n: i64) -> Duration { return Duration { seconds: n * 60 }; }
```

**Parameters:**

- `n`: `i64`

**Returns:** `Duration`

*Defined at line 377*

---

## dur_hours

```fides
pub fn dur_hours(n: i64) -> Duration { return Duration { seconds: n * 3600 }; }
```

**Parameters:**

- `n`: `i64`

**Returns:** `Duration`

*Defined at line 378*

---

## dur_days

```fides
pub fn dur_days(n: i64) -> Duration { return Duration { seconds: n * 86400 }; }
```

**Parameters:**

- `n`: `i64`

**Returns:** `Duration`

*Defined at line 379*

---

## dur_weeks

```fides
pub fn dur_weeks(n: i64) -> Duration { return Duration { seconds: n * 604800 }; }
```

**Parameters:**

- `n`: `i64`

**Returns:** `Duration`

*Defined at line 380*

---

## dur_millis

Milliseconds truncated to whole seconds.

```fides
pub fn dur_millis(n: i64) -> Duration { return Duration { seconds: n / 1000 }; }
```

**Parameters:**

- `n`: `i64`

**Returns:** `Duration`

*Defined at line 382*

---

## dur_to_seconds

```fides
pub fn dur_to_seconds(d: Duration) -> i64 { return d.seconds; }
```

**Parameters:**

- `d`: `Duration`

**Returns:** `i64`

*Defined at line 384*

---

## dur_to_millis

```fides
pub fn dur_to_millis(d: Duration) -> i64 { return d.seconds * 1000; }
```

**Parameters:**

- `d`: `Duration`

**Returns:** `i64`

*Defined at line 385*

---

## dur_add

```fides
pub fn dur_add(a: Duration, b: Duration) -> Duration {
```

**Parameters:**

- `a`: `Duration`
- `b`: `Duration`

**Returns:** `Duration`

*Defined at line 387*

---

## dur_sub

```fides
pub fn dur_sub(a: Duration, b: Duration) -> Duration {
```

**Parameters:**

- `a`: `Duration`
- `b`: `Duration`

**Returns:** `Duration`

*Defined at line 390*

---

## dur_neg

```fides
pub fn dur_neg(d: Duration) -> Duration {
```

**Parameters:**

- `d`: `Duration`

**Returns:** `Duration`

*Defined at line 393*

---

## dur_abs

```fides
pub fn dur_abs(d: Duration) -> Duration {
```

**Parameters:**

- `d`: `Duration`

**Returns:** `Duration`

*Defined at line 396*

---

## dt_diff

Difference between two times as a Duration (a - b).

```fides
pub fn dt_diff(a: LogicalTime, b: LogicalTime) -> Duration {
```

**Parameters:**

- `a`: `LogicalTime`
- `b`: `LogicalTime`

**Returns:** `Duration`

*Defined at line 402*

---

## dt_add

Shift a time by a Duration.

```fides
pub fn dt_add(t: LogicalTime, d: Duration) -> LogicalTime {
```

**Parameters:**

- `t`: `LogicalTime`
- `d`: `Duration`

**Returns:** `LogicalTime`

*Defined at line 407*

---

## dt_sub

```fides
pub fn dt_sub(t: LogicalTime, d: Duration) -> LogicalTime {
```

**Parameters:**

- `t`: `LogicalTime`
- `d`: `Duration`

**Returns:** `LogicalTime`

*Defined at line 410*

---

## dur_parse

Parse a duration like "90s", "1h30m", "2d", "1w2d3h4m5s", "-5m".
Units: s, m, h, d, w (also "ms" is NOT supported — ambiguity with 'm').
Returns (ok, seconds).

```fides
pub fn dur_parse(s: str) -> (bool, i64) {
```

**Parameters:**

- `s`: `str`

**Returns:** `(bool, i64)`

*Defined at line 417*

---

## dur_parse_or_zero

Parse a duration string, returning 0 seconds on failure.

```fides
pub fn dur_parse_or_zero(s: str) -> Duration {
```

**Parameters:**

- `s`: `str`

**Returns:** `Duration`

*Defined at line 456*

---

## dur_format

Format a Duration compactly: "2d3h4m5s", "-1h30m", "0s".
Zero value formats as "0s".

```fides
pub fn dur_format(d: Duration) -> str {
```

**Parameters:**

- `d`: `Duration`

**Returns:** `str`

*Defined at line 464*

---

## dur_format_hms

Format a Duration as "HH:MM:SS" (sign applied to the whole value;
hours can exceed 23 — this is a timespan, not a clock).

```fides
pub fn dur_format_hms(d: Duration) -> str {
```

**Parameters:**

- `d`: `Duration`

**Returns:** `str`

*Defined at line 492*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
