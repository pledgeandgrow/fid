[All modules](index.md)

# datetime -- Fides Documentation

## Contents

- [dt_parse](#dt_parse) (function)
- [dt_parse_or_epoch](#dt_parse_or_epoch) (function)
- [dt_parse_time](#dt_parse_time) (function)
- [dt_is_valid](#dt_is_valid) (function)
- [dt_weekday_name](#dt_weekday_name) (function)
- [dt_weekday_short](#dt_weekday_short) (function)
- [dt_month_name](#dt_month_name) (function)
- [dt_month_short](#dt_month_short) (function)
- [dt_month_name_in](#dt_month_name_in) (function)
- [dt_month_short_in](#dt_month_short_in) (function)
- [dt_weekday_name_in](#dt_weekday_name_in) (function)
- [dt_weekday_short_in](#dt_weekday_short_in) (function)
- [dt_lang_supported](#dt_lang_supported) (function)
- [dt_format](#dt_format) (function)
- [dt_format_lang](#dt_format_lang) (function)
- [dt_parse_format](#dt_parse_format) (function)
- [dt_parse_format_lang](#dt_parse_format_lang) (function)
- [dt_to_iso](#dt_to_iso) (function)
- [dt_to_date](#dt_to_date) (function)
- [dt_to_rfc2822](#dt_to_rfc2822) (function)
- [dt_from_seconds](#dt_from_seconds) (function)
- [dt_to_seconds](#dt_to_seconds) (function)
- [dt_now](#dt_now) (function)
- [Duration](#Duration) (struct)
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
- [IsoDuration](#IsoDuration) (struct)
- [dur_iso_parse](#dur_iso_parse) (function)
- [dur_iso_format](#dur_iso_format) (function)
- [dur_to_iso](#dur_to_iso) (function)
- [dt_add_iso](#dt_add_iso) (function)

## dt_parse

Parse an ISO 8601 datetime to (ok, unix_seconds).

Accepted forms:
YYYY-MM-DD
YYYY-MM-DD HH:MM[:SS]            (space or 'T' separator)
YYYY-MM-DDTHH:MM[:SS][.frac]
any of the above + "Z" or ±HH / ±HHMM / ±HH:MM zone offset

The zone offset is applied (result is always UTC seconds).
Dates before 1970 produce negative seconds — fine.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var (ok, ts) = dt_parse("2026-10-27T14:30:00+02:00");
assert(ok);
assert_eq(dt_to_iso(dt_from_seconds(ts)), "2026-10-27T12:30:00Z");
var (bad, _z) = dt_parse("not a date");
assert(!bad);
```

```fides
pub fn dt_parse(s: str) -> (bool, i64) {
```

**Parameters:**

- `s`: `str`

**Returns:** `(bool, i64)`

*Defined at line 98*

---

## dt_parse_or_epoch

Parse an ISO 8601 datetime; returns the unix timestamp, or 0 on failure
(combine with dt_parse when the distinction matters).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dt_parse_or_epoch("1970-01-02"), 86400);
assert_eq(dt_parse_or_epoch("garbage"), 0);
```

```fides
pub fn dt_parse_or_epoch(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 246*

---

## dt_parse_time

Parse an ISO 8601 datetime to a LogicalTime; the epoch on failure.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var t = dt_parse_time("2024-03-05 08:09");
assert_eq(time_get_minute(t), 9);
```

```fides
pub fn dt_parse_time(s: str) -> LogicalTime {
```

**Parameters:**

- `s`: `str`

**Returns:** `LogicalTime`

*Defined at line 259*

---

## dt_is_valid

Is the string a valid ISO 8601 datetime in the forms dt_parse accepts?


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(dt_is_valid("2024-01-31T23:59:59Z"));
assert(!dt_is_valid("2024-13-01"));
```

```fides
pub fn dt_is_valid(s: str) -> bool {
```

**Parameters:**

- `s`: `str`

**Returns:** `bool`

*Defined at line 268*

---

## dt_weekday_name

Full weekday name ("Monday") for weekday index 0=Sunday..6=Saturday.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dt_weekday_name(1), "Monday");
```

```fides
pub fn dt_weekday_name(wd: i64) -> str {
```

**Parameters:**

- `wd`: `i64`

**Returns:** `str`

*Defined at line 279*

---

## dt_weekday_short

Short weekday name ("Mon").


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dt_weekday_short(0), "Sun");
```

```fides
pub fn dt_weekday_short(wd: i64) -> str {
```

**Parameters:**

- `wd`: `i64`

**Returns:** `str`

*Defined at line 308*

---

## dt_month_name

Full month name ("January") for month 1..12.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dt_month_name(12), "December");
```

```fides
pub fn dt_month_name(m: i64) -> str {
```

**Parameters:**

- `m`: `i64`

**Returns:** `str`

*Defined at line 316*

---

## dt_month_short

Short month name ("Jan").


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dt_month_short(3), "Mar");
```

```fides
pub fn dt_month_short(m: i64) -> str {
```

**Parameters:**

- `m`: `i64`

**Returns:** `str`

*Defined at line 360*

---

## dt_month_name_in

The name of month 1..12 in language `lang` ("?" for another number).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dt_month_name_in("fr", 8), "août");
assert_eq(dt_month_name_in("de-AT", 3), "März");
```

```fides
pub fn dt_month_name_in(lang: str, m: i64) -> str {
```

**Parameters:**

- `lang`: `str`
- `m`: `i64`

**Returns:** `str`

*Defined at line 484*

---

## dt_month_short_in

The abbreviated name of month 1..12 in language `lang`.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dt_month_short_in("fr", 2), "févr.");
```

```fides
pub fn dt_month_short_in(lang: str, m: i64) -> str {
```

**Parameters:**

- `lang`: `str`
- `m`: `i64`

**Returns:** `str`

*Defined at line 499*

---

## dt_weekday_name_in

The name of weekday 0 (Sunday) .. 6 in language `lang`.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dt_weekday_name_in("es", 3), "miércoles");
```

```fides
pub fn dt_weekday_name_in(lang: str, wd: i64) -> str {
```

**Parameters:**

- `lang`: `str`
- `wd`: `i64`

**Returns:** `str`

*Defined at line 514*

---

## dt_weekday_short_in

The abbreviated name of weekday 0 (Sunday) .. 6 in language `lang`.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dt_weekday_short_in("de", 1), "Mo.");
```

```fides
pub fn dt_weekday_short_in(lang: str, wd: i64) -> str {
```

**Parameters:**

- `lang`: `str`
- `wd`: `i64`

**Returns:** `str`

*Defined at line 529*

---

## dt_lang_supported

Whether `lang` has its own names here (English included); any other
language formats and parses with the English ones.

```fides
pub fn dt_lang_supported(lang: str) -> bool {
```

**Parameters:**

- `lang`: `str`

**Returns:** `bool`

*Defined at line 542*

---

## dt_format

Format a LogicalTime with strftime-style codes.

Supported: %Y %y %m %d %e %H %I %M %S %p %A %a %B %b %h %j %F %T %R %D %z %Z %%,
the ISO 8601 week date %G (ISO year), %g (its last two digits), %V (ISO
week 01..53), %u (weekday 1 Monday .. 7), and %w (weekday 0 Sunday .. 6),
%s (Unix seconds), %n (newline), %t (tab).
Unknown codes are emitted literally (the '%' is kept).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var t = time_new(2026, 10, 27, 14, 30, 5);
var s = dt_format(t, "%A %d %B %Y, %H:%M:%S");
println(s);
assert_eq(s, "Tuesday 27 October 2026, 14:30:05");
assert_eq(dt_format(t, "%F %I%p"), "2026-10-27 02PM");
```

```fides
pub fn dt_format(t: LogicalTime, fmt: str) -> str {
```

**Parameters:**

- `t`: `LogicalTime`
- `fmt`: `str`

**Returns:** `str`

*Defined at line 588*

---

## dt_format_lang

dt_format with the month and weekday names (%B %b %h %A %a) of
language `lang` (see "Localized names": en fr de es it pt ar). The
other codes are the same in every language (%p stays AM/PM).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var t = time_new(2026, 8, 15, 9, 0, 0);
assert_eq(dt_format_lang(t, "%A %e %B %Y", "fr"), "samedi 15 août 2026");
assert_eq(dt_format_lang(t, "%a, %d. %b %Y", "de"), "Sa., 15. Aug. 2026");
```

```fides
pub fn dt_format_lang(t: LogicalTime, fmt: str, lang: str) -> str {
```

**Parameters:**

- `t`: `LogicalTime`
- `fmt`: `str`
- `lang`: `str`

**Returns:** `str`

*Defined at line 600*

---

## dt_parse_format

Parse `s` by a strptime-style format to (ok, Unix seconds, UTC).

Codes: %Y (1..4 digits) %y (69..99 -> 19xx, 00..68 -> 20xx) %m %d %e
%H %I %M %S (0..60) %p (AM/PM, any case) %j (day of year) %b %h %B
(month names, full or short, any case) %a %A (weekday names: checked
against the date) %z (Z, +HH, +HHMM, +HH:MM) %Z (UTC, GMT or Z) %s
(Unix seconds: the rest of the date is not used) %G %V %u (ISO 8601
week date) %w (weekday 0 Sunday .. 6, checked) %F %T %R %D %n %t %%.
A space in the format matches any run of white space, none included;
anything else must match exactly. The whole of `s` must be used, and
the date must exist (no February 30, no week 53 in a 52-week year).
Fields the format leaves out default to 1970-01-01 00:00:00 UTC.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var (ok, ts) = dt_parse_format("Tue, 27 Oct 2026 14:30:00 +0200", "%a, %d %b %Y %T %z");
assert(ok);
assert_eq(dt_to_iso(dt_from_seconds(ts)), "2026-10-27T12:30:00Z");
var (ok2, ts2) = dt_parse_format("2026-W44-2", "%G-W%V-%u");
assert_eq(dt_to_date(dt_from_seconds(ts2)), "2026-10-27");
```

```fides
pub fn dt_parse_format(s: str, fmt_in: str) -> (bool, i64) {
```

**Parameters:**

- `s`: `str`
- `fmt_in`: `str`

**Returns:** `(bool, i64)`

*Defined at line 901*

---

## dt_parse_format_lang

dt_parse_format with the month and weekday names of language `lang`
(see "Localized names"): %b %h %B take the language's month names, full
or abbreviated, the final "." of an abbreviation optional; %a %A its
weekday names. ASCII letters match in any case; other letters as
written ("août", not "AOÛT").


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var (ok, ts) = dt_parse_format_lang("mercredi 4 févr. 2026", "%A %e %b %Y", "fr");
assert_eq(dt_to_date(dt_from_seconds(ts)), "2026-02-04");
```

```fides
pub fn dt_parse_format_lang(s: str, fmt_in: str, lang: str) -> (bool, i64) {
```

**Parameters:**

- `s`: `str`
- `fmt_in`: `str`
- `lang`: `str`

**Returns:** `(bool, i64)`

*Defined at line 914*

---

## dt_to_iso

ISO 8601 "YYYY-MM-DDTHH:MM:SSZ" (same as time_to_iso).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dt_to_iso(dt_from_seconds(0)), "1970-01-01T00:00:00Z");
```

```fides
pub fn dt_to_iso(t: LogicalTime) -> str {
```

**Parameters:**

- `t`: `LogicalTime`

**Returns:** `str`

*Defined at line 1202*

---

## dt_to_date

Date only "YYYY-MM-DD".


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dt_to_date(time_from_date(2026, 1, 9)), "2026-01-09");
```

```fides
pub fn dt_to_date(t: LogicalTime) -> str {
```

**Parameters:**

- `t`: `LogicalTime`

**Returns:** `str`

*Defined at line 1210*

---

## dt_to_rfc2822

RFC 2822 style: "Tue, 27 Oct 2026 14:30:00 +0000".


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dt_to_rfc2822(time_new(2026, 10, 27, 14, 30, 0)), "Tue, 27 Oct 2026 14:30:00 +0000");
```

```fides
pub fn dt_to_rfc2822(t: LogicalTime) -> str {
```

**Parameters:**

- `t`: `LogicalTime`

**Returns:** `str`

*Defined at line 1218*

---

## dt_from_seconds

Unix seconds -> LogicalTime.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(time_get_year(dt_from_seconds(946684800)), 2000);
```

```fides
pub fn dt_from_seconds(ts: i64) -> LogicalTime {
```

**Parameters:**

- `ts`: `i64`

**Returns:** `LogicalTime`

*Defined at line 1226*

---

## dt_to_seconds

LogicalTime -> unix seconds.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dt_to_seconds(time_from_date(1970, 1, 2)), 86400);
```

```fides
pub fn dt_to_seconds(t: LogicalTime) -> i64 {
```

**Parameters:**

- `t`: `LogicalTime`

**Returns:** `i64`

*Defined at line 1234*

---

## dt_now

The current wall-clock time as a LogicalTime (NOT deterministic —
reads CLOCK_REALTIME). With FIDES_TEST_NOW set to unix seconds (a test
run, complaint CT3), it is that time instead, so a test that reads the
date repeats exactly.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(time_get_year(dt_now()) >= 2024);
```

```fides
pub fn dt_now() -> LogicalTime {
```

**Returns:** `LogicalTime`

*Defined at line 1245*

---

## Duration

A signed relative timespan in seconds.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var d = dur_add(dur_hours(1), dur_minutes(30));
assert_eq(dur_to_seconds(d), 5400);
assert_eq(dur_format(d), "1h30m");
```

```fides
pub struct Duration {
```

**Fields:**

| Name | Type |
|------|------|
| `seconds` | `i64` |

*Defined at line 1268*

---

## dur_seconds

A Duration of `n` seconds.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dur_to_seconds(dur_seconds(42)), 42);
```

```fides
pub fn dur_seconds(n: i64) -> Duration {
```

**Parameters:**

- `n`: `i64`

**Returns:** `Duration`

*Defined at line 1276*

---

## dur_minutes

A Duration of `n` minutes.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dur_to_seconds(dur_minutes(2)), 120);
```

```fides
pub fn dur_minutes(n: i64) -> Duration {
```

**Parameters:**

- `n`: `i64`

**Returns:** `Duration`

*Defined at line 1285*

---

## dur_hours

A Duration of `n` hours.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dur_to_seconds(dur_hours(2)), 7200);
```

```fides
pub fn dur_hours(n: i64) -> Duration {
```

**Parameters:**

- `n`: `i64`

**Returns:** `Duration`

*Defined at line 1294*

---

## dur_days

A Duration of `n` days.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dur_to_seconds(dur_days(1)), 86400);
```

```fides
pub fn dur_days(n: i64) -> Duration {
```

**Parameters:**

- `n`: `i64`

**Returns:** `Duration`

*Defined at line 1303*

---

## dur_weeks

A Duration of `n` weeks.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dur_format(dur_weeks(1)), "1w");
```

```fides
pub fn dur_weeks(n: i64) -> Duration {
```

**Parameters:**

- `n`: `i64`

**Returns:** `Duration`

*Defined at line 1312*

---

## dur_millis

Milliseconds truncated to whole seconds.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dur_to_seconds(dur_millis(2500)), 2);
```

```fides
pub fn dur_millis(n: i64) -> Duration {
```

**Parameters:**

- `n`: `i64`

**Returns:** `Duration`

*Defined at line 1321*

---

## dur_to_seconds

The Duration's length in seconds.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dur_to_seconds(dur_minutes(1)), 60);
```

```fides
pub fn dur_to_seconds(d: Duration) -> i64 {
```

**Parameters:**

- `d`: `Duration`

**Returns:** `i64`

*Defined at line 1331*

---

## dur_to_millis

The Duration's length in milliseconds.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dur_to_millis(dur_seconds(3)), 3000);
```

```fides
pub fn dur_to_millis(d: Duration) -> i64 {
```

**Parameters:**

- `d`: `Duration`

**Returns:** `i64`

*Defined at line 1338*

---

## dur_add

Sum of two Durations.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dur_to_seconds(dur_add(dur_seconds(1), dur_seconds(2))), 3);
```

```fides
pub fn dur_add(a: Duration, b: Duration) -> Duration {
```

**Parameters:**

- `a`: `Duration`
- `b`: `Duration`

**Returns:** `Duration`

*Defined at line 1346*

---

## dur_sub

Difference of two Durations (a - b).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dur_to_seconds(dur_sub(dur_seconds(1), dur_seconds(5))), -4);
```

```fides
pub fn dur_sub(a: Duration, b: Duration) -> Duration {
```

**Parameters:**

- `a`: `Duration`
- `b`: `Duration`

**Returns:** `Duration`

*Defined at line 1355*

---

## dur_neg

The Duration with its sign flipped.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dur_to_seconds(dur_neg(dur_seconds(5))), -5);
```

```fides
pub fn dur_neg(d: Duration) -> Duration {
```

**Parameters:**

- `d`: `Duration`

**Returns:** `Duration`

*Defined at line 1364*

---

## dur_abs

The Duration without its sign.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dur_to_seconds(dur_abs(dur_seconds(-5))), 5);
```

```fides
pub fn dur_abs(d: Duration) -> Duration {
```

**Parameters:**

- `d`: `Duration`

**Returns:** `Duration`

*Defined at line 1373*

---

## dt_diff

Difference between two times as a Duration (a - b).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var a = time_from_date(2024, 1, 2);
var b = time_from_date(2024, 1, 1);
assert_eq(dur_format(dt_diff(a, b)), "1d");
```

```fides
pub fn dt_diff(a: LogicalTime, b: LogicalTime) -> Duration {
```

**Parameters:**

- `a`: `LogicalTime`
- `b`: `LogicalTime`

**Returns:** `Duration`

*Defined at line 1388*

---

## dt_add

Shift a time by a Duration.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var t = dt_add(time_from_date(2024, 1, 1), dur_hours(36));
assert_eq(dt_to_iso(t), "2024-01-02T12:00:00Z");
```

```fides
pub fn dt_add(t: LogicalTime, d: Duration) -> LogicalTime {
```

**Parameters:**

- `t`: `LogicalTime`
- `d`: `Duration`

**Returns:** `LogicalTime`

*Defined at line 1399*

---

## dt_sub

Shift a time back by a Duration.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var t = dt_sub(time_from_date(2024, 1, 1), dur_seconds(1));
assert_eq(dt_to_iso(t), "2023-12-31T23:59:59Z");
```

```fides
pub fn dt_sub(t: LogicalTime, d: Duration) -> LogicalTime {
```

**Parameters:**

- `t`: `LogicalTime`
- `d`: `Duration`

**Returns:** `LogicalTime`

*Defined at line 1407*

---

## dur_parse

Parse a duration like "90s", "1h30m", "2d", "1w2d3h4m5s", "-5m".
Units: s, m, h, d, w (also "ms" is NOT supported — ambiguity with 'm').
Returns (ok, seconds).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var (ok, secs) = dur_parse("1h30m");
assert(ok);
assert_eq(secs, 5400);
var (ok2, _s) = dur_parse("5x");
assert(!ok2);
```

```fides
pub fn dur_parse(s: str) -> (bool, i64) {
```

**Parameters:**

- `s`: `str`

**Returns:** `(bool, i64)`

*Defined at line 1421*

---

## dur_parse_or_zero

Parse a duration string, returning 0 seconds on failure.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dur_to_seconds(dur_parse_or_zero("2d")), 172800);
assert_eq(dur_to_seconds(dur_parse_or_zero("oops")), 0);
```

```fides
pub fn dur_parse_or_zero(s: str) -> Duration {
```

**Parameters:**

- `s`: `str`

**Returns:** `Duration`

*Defined at line 1484*

---

## dur_format

Format a Duration compactly: "2d3h4m5s", "-1h30m", "0s".
Zero value formats as "0s".


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dur_format(dur_seconds(93784)), "1d2h3m4s");
assert_eq(dur_format(dur_days(9)), "1w2d");
assert_eq(dur_format(dur_seconds(0)), "0s");
```

```fides
pub fn dur_format(d: Duration) -> str {
```

**Parameters:**

- `d`: `Duration`

**Returns:** `str`

*Defined at line 1503*

---

## dur_format_hms

Format a Duration as "HH:MM:SS" (sign applied to the whole value;
hours can exceed 23 — this is a timespan, not a clock).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dur_format_hms(dur_seconds(3725)), "01:02:05");
```

```fides
pub fn dur_format_hms(d: Duration) -> str {
```

**Parameters:**

- `d`: `Duration`

**Returns:** `str`

*Defined at line 1548*

---

## IsoDuration

A parsed ISO 8601 duration. `ok` is false when the text was not one.
Weeks are folded into `days`; hours, minutes and seconds into
`seconds`; `nanos` is the fraction of a second (0..999999999).

```fides
pub struct IsoDuration {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `neg` | `bool` |
| `years` | `i64` |
| `months` | `i64` |
| `days` | `i64` |
| `seconds` | `i64` |
| `nanos` | `i64` |

*Defined at line 1572*

---

## dur_iso_parse

Parse an ISO 8601 duration: an optional sign, `P`, then any of `nY`
`nM` `nW` `nD` in that order, then `T` and any of `nH` `nM` `nS` (at
least one component, at least one after `T`); only the seconds may have
a fraction (`.` or `,`, up to 9 digits).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var d = dur_iso_parse("P1Y2M10DT2H30M");
assert(d.ok);
assert_eq(d.months, 2);
assert_eq(d.seconds, 9000);
assert(!dur_iso_parse("P1H").ok);
```

```fides
pub fn dur_iso_parse(s: str) -> IsoDuration {
```

**Parameters:**

- `s`: `str`

**Returns:** `IsoDuration`

*Defined at line 1599*

---

## dur_iso_format

The canonical text of `d`: units that are zero left out, hours,
minutes and seconds from `seconds` (not carried into days), the
fraction without trailing zeros, "PT0S" for zero.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dur_iso_format(dur_iso_parse("P2W")), "P14D");
assert_eq(dur_iso_format(dur_iso_parse("PT0,250S")), "PT0.25S");
```

```fides
pub fn dur_iso_format(d: IsoDuration) -> str {
```

**Parameters:**

- `d`: `IsoDuration`

**Returns:** `str`

*Defined at line 1716*

---

## dur_to_iso

An exact Duration as ISO 8601: whole days (86400 s) and the rest as
hours, minutes and seconds.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dur_to_iso(dur_seconds(93784)), "P1DT2H3M4S");
assert_eq(dur_to_iso(dur_seconds(-90)), "-PT1M30S");
```

```fides
pub fn dur_to_iso(d: Duration) -> str {
```

**Parameters:**

- `d`: `Duration`

**Returns:** `str`

*Defined at line 1769*

---

## dt_add_iso

`t` moved by `d`: years and months on the calendar (a day past the
end of the new month becomes its last day), then the days, then the
hours, minutes and seconds; a negative duration moves back the same way.
The fraction of a second is dropped (LogicalTime counts whole seconds).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var t = time_new(2024, 1, 31, 10, 0, 0);
assert_eq(time_to_iso(dt_add_iso(t, dur_iso_parse("P1M"))), "2024-02-29T10:00:00Z");
assert_eq(time_to_iso(dt_add_iso(t, dur_iso_parse("-P1DT1H"))), "2024-01-30T09:00:00Z");
```

```fides
pub fn dt_add_iso(t: LogicalTime, d: IsoDuration) -> LogicalTime {
```

**Parameters:**

- `t`: `LogicalTime`
- `d`: `IsoDuration`

**Returns:** `LogicalTime`

*Defined at line 1790*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
