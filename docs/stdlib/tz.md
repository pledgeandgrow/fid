[All modules](index.md)

# tz -- Fides Documentation

## Contents

- [TzWhen](#TzWhen) (struct)
- [TzHist](#TzHist) (struct)
- [Tz](#Tz) (struct)
- [tz_names](#tz_names) (function)
- [tz_find](#tz_find) (function)
- [tz_known](#tz_known) (function)
- [tz_is_dst](#tz_is_dst) (function)
- [tz_offset](#tz_offset) (function)
- [tz_abbr](#tz_abbr) (function)
- [tz_offset_str](#tz_offset_str) (function)
- [tz_to_local](#tz_to_local) (function)
- [tz_from_local](#tz_from_local) (function)
- [tz_tzif_ok](#tz_tzif_ok) (function)
- [tz_from_tzif](#tz_from_tzif) (function)
- [tz_load](#tz_load) (function)
- [tz_from_posix](#tz_from_posix) (function)
- [tz_format](#tz_format) (function)
- [tz_to_iso](#tz_to_iso) (function)
- [tz_convert](#tz_convert) (function)

## TzWhen

When a change happens: in `month`, the `week`-th `weekday` (0 Sunday
.. 6 Saturday; week 5 = the last one), at `secs` seconds after midnight
of the local clock (utc = false) or of UTC (utc = true).
`month` 0 is a day of the year instead (the POSIX Julian forms), held
in `weekday`: with `week` 1 it is "Jn", n from 1 to 365 with February 29
never counted (J60 is always March 1); with `week` 0 it is "n", n from
0 to 365 counting February 29 (59 is February 29 in a leap year).

```fides
pub struct TzWhen {
```

**Fields:**

| Name | Type |
|------|------|
| `month` | `i64` |
| `week` | `i64` |
| `weekday` | `i64` |
| `secs` | `i64` |
| `utc` | `bool` |

*Defined at line 44*

---

## TzHist

A zone's past (from a TZif file): the UTC moments the clock changed
(`at`, ascending) and the local time type from each one on (`kind`,
an index into `off`/`dst`/`abbr`); before the first, type `first`.
Empty for a zone of the built-in table.

```fides
pub struct TzHist {
```

**Fields:**

| Name | Type |
|------|------|
| `at` | `Vec<i64>` |
| `kind` | `Vec<i64>` |
| `off` | `Vec<i64>` |
| `dst` | `Vec<i64>` |
| `abbr` | `Vec<str>` |
| `first` | `i64` |
| `has_rule` | `bool` |

*Defined at line 56*

---

## Tz

A time zone: its name, standard offset (seconds east of UTC), and DST:
whether it has one, how far it moves the clock, when it starts and ends;
and, for a zone read from the database, its past (`hist`).

```fides
pub struct Tz {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `std_off` | `i64` |
| `std_abbr` | `str` |
| `has_dst` | `bool` |
| `dst_save` | `i64` |
| `dst_abbr` | `str` |
| `start` | `TzWhen` |
| `end` | `TzWhen` |
| `hist` | `TzHist` |

*Defined at line 75*

---

## tz_names

The zones this module knows, by IANA name. Unknown names give UTC.

```fides
pub fn tz_names() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 115*

---

## tz_find

The zone of that IANA name ("Europe/Paris"), "UTC", or a fixed offset
written "+05:30" / "-08:00"; anything else is UTC.

```fides
pub fn tz_find(name: str) -> Tz {
```

**Parameters:**

- `name`: `str`

**Returns:** `Tz`

*Defined at line 201*

---

## tz_known

Is `name` one of the zones this module knows?

```fides
pub fn tz_known(name: str) -> bool {
```

**Parameters:**

- `name`: `str`

**Returns:** `bool`

*Defined at line 224*

---

## tz_is_dst

Is daylight time in force at `t` (UTC seconds)?

```fides
pub fn tz_is_dst(z: Tz, t: LogicalTime) -> bool {
```

**Parameters:**

- `z`: `Tz`
- `t`: `LogicalTime`

**Returns:** `bool`

*Defined at line 308*

---

## tz_offset

The zone's offset from UTC at `t`, in seconds east.

```fides
pub fn tz_offset(z: Tz, t: LogicalTime) -> i64 {
```

**Parameters:**

- `z`: `Tz`
- `t`: `LogicalTime`

**Returns:** `i64`

*Defined at line 328*

---

## tz_abbr

The abbreviation in force at `t` (EST or EDT, CET or CEST ...).

```fides
pub fn tz_abbr(z: Tz, t: LogicalTime) -> str {
```

**Parameters:**

- `z`: `Tz`
- `t`: `LogicalTime`

**Returns:** `str`

*Defined at line 340*

---

## tz_offset_str

An offset as "+HH:MM" or "-HH:MM".


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(tz_offset_str(7200), "+02:00");
assert_eq(tz_offset_str(-16200), "-04:30");
assert(tz_known("Europe/Paris"));
assert(!tz_known("Mars/Olympus"));
```

```fides
pub fn tz_offset_str(off: i64) -> str {
```

**Parameters:**

- `off`: `i64`

**Returns:** `str`

*Defined at line 358*

---

## tz_to_local

The wall-clock time in zone `z` at the UTC moment `t`, as a LogicalTime
whose fields (time_get_hour ...) read as that local clock.

```fides
pub fn tz_to_local(t: LogicalTime, z: Tz) -> LogicalTime {
```

**Parameters:**

- `t`: `LogicalTime`
- `z`: `Tz`

**Returns:** `LogicalTime`

*Defined at line 380*

---

## tz_from_local

The UTC moment at which zone `z`'s clock shows `local` (built with
time_new). A local time skipped in spring is read after the gap; one
repeated in autumn is its first occurrence.

```fides
pub fn tz_from_local(local: LogicalTime, z: Tz) -> LogicalTime {
```

**Parameters:**

- `local`: `LogicalTime`
- `z`: `Tz`

**Returns:** `LogicalTime`

*Defined at line 387*

---

## tz_tzif_ok

Is `b` a TZif file this module reads (versions 1 to 4)?

```fides
pub fn tz_tzif_ok(b: Vec<i64>) -> bool {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 438*

---

## tz_from_tzif

The zone `name` from the bytes of its TZif file. Anything malformed
gives UTC (with that name); tz_tzif_ok says whether bytes are TZif.

```fides
pub fn tz_from_tzif(name: str, b: Vec<i64>) -> Tz {
```

**Parameters:**

- `name`: `str`
- `b`: `Vec<i64>`

**Returns:** `Tz`

*Defined at line 459*

---

## tz_load

The zone `name` ("America/New_York") with its whole past, from the
system's time zone database; the built-in table's zone where there is
no database (Windows) or no such file.

```fides
pub fn tz_load(name: str) -> Tz {
```

**Parameters:**

- `name`: `str`

**Returns:** `Tz`

*Defined at line 571*

---

## tz_from_posix

The zone a POSIX TZ string describes ("CET-1CEST,M3.5.0,M10.5.0/3",
"<+0530>-5:30", "<+0330>-3:30<+0430>,J79/24,J263/24"); a zone
whose name is "" when it cannot be read. POSIX
offsets count west, so "EST5" is five hours behind UTC.

```fides
pub fn tz_from_posix(name: str, s: str) -> Tz {
```

**Parameters:**

- `name`: `str`
- `s`: `str`

**Returns:** `Tz`

*Defined at line 729*

---

## tz_format

dt_format of the local time in `z`, plus %Z (abbreviation) and %z
(offset, "+HH:MM").

```fides
pub fn tz_format(t: LogicalTime, z: Tz, fmt: str) -> str {
```

**Parameters:**

- `t`: `LogicalTime`
- `z`: `Tz`
- `fmt`: `str`

**Returns:** `str`

*Defined at line 780*

---

## tz_to_iso

ISO 8601 with the zone's offset: "2026-07-04T12:00:00-04:00".

```fides
pub fn tz_to_iso(t: LogicalTime, z: Tz) -> str {
```

**Parameters:**

- `t`: `LogicalTime`
- `z`: `Tz`

**Returns:** `str`

*Defined at line 788*

---

## tz_convert

The same moment shown in another zone: wall clock of `to`.

```fides
pub fn tz_convert(local: LogicalTime, from: Tz, to: Tz) -> LogicalTime {
```

**Parameters:**

- `local`: `LogicalTime`
- `from`: `Tz`
- `to`: `Tz`

**Returns:** `LogicalTime`

*Defined at line 793*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
