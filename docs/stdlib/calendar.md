[All modules](index.md)

# calendar -- Fides Documentation

## Contents

- [CalDate](#CalDate) (struct)
- [CalIsoWeek](#CalIsoWeek) (struct)
- [CAL_JDN_UNIX_EPOCH:](#CAL_JDN_UNIX_EPOCH:) (const)
- [cal_days_from_civil](#cal_days_from_civil) (function)
- [cal_civil_from_days](#cal_civil_from_days) (function)
- [cal_is_valid](#cal_is_valid) (function)
- [cal_date_str](#cal_date_str) (function)
- [cal_iso_weekday](#cal_iso_weekday) (function)
- [cal_jdn_from_days](#cal_jdn_from_days) (function)
- [cal_days_from_jdn](#cal_days_from_jdn) (function)
- [cal_jdn](#cal_jdn) (function)
- [cal_julian_is_leap](#cal_julian_is_leap) (function)
- [cal_julian_days_in_month](#cal_julian_days_in_month) (function)
- [cal_julian_to_days](#cal_julian_to_days) (function)
- [cal_days_to_julian](#cal_days_to_julian) (function)
- [cal_gregorian_to_julian](#cal_gregorian_to_julian) (function)
- [cal_julian_to_gregorian](#cal_julian_to_gregorian) (function)
- [cal_iso_week_of_days](#cal_iso_week_of_days) (function)
- [cal_iso_week](#cal_iso_week) (function)
- [cal_iso_weeks_in_year](#cal_iso_weeks_in_year) (function)
- [cal_days_from_iso_week](#cal_days_from_iso_week) (function)
- [cal_iso_week_is_valid](#cal_iso_week_is_valid) (function)
- [cal_iso_week_str](#cal_iso_week_str) (function)
- [cal_day_of_year](#cal_day_of_year) (function)
- [cal_days_from_ordinal](#cal_days_from_ordinal) (function)
- [cal_easter](#cal_easter) (function)
- [cal_julian_easter](#cal_julian_easter) (function)
- [CAL_ISLAMIC_I:](#CAL_ISLAMIC_I:) (const)
- [CAL_ISLAMIC_II:](#CAL_ISLAMIC_II:) (const)
- [CAL_ISLAMIC_III:](#CAL_ISLAMIC_III:) (const)
- [CAL_ISLAMIC_IV:](#CAL_ISLAMIC_IV:) (const)
- [CAL_ISLAMIC_EPOCH_CIVIL:](#CAL_ISLAMIC_EPOCH_CIVIL:) (const)
- [CAL_ISLAMIC_EPOCH_ASTRONOMICAL:](#CAL_ISLAMIC_EPOCH_ASTRONOMICAL:) (const)
- [cal_islamic_is_leap_rule](#cal_islamic_is_leap_rule) (function)
- [cal_islamic_is_leap](#cal_islamic_is_leap) (function)
- [cal_islamic_days_in_month_rule](#cal_islamic_days_in_month_rule) (function)
- [cal_islamic_days_in_month](#cal_islamic_days_in_month) (function)
- [cal_islamic_is_valid_rule](#cal_islamic_is_valid_rule) (function)
- [cal_islamic_is_valid](#cal_islamic_is_valid) (function)
- [cal_islamic_to_days_rule](#cal_islamic_to_days_rule) (function)
- [cal_days_to_islamic_rule](#cal_days_to_islamic_rule) (function)
- [cal_islamic_to_days](#cal_islamic_to_days) (function)
- [cal_days_to_islamic](#cal_days_to_islamic) (function)
- [cal_gregorian_to_islamic](#cal_gregorian_to_islamic) (function)
- [cal_islamic_to_gregorian](#cal_islamic_to_gregorian) (function)
- [cal_islamic_month_name](#cal_islamic_month_name) (function)
- [CAL_HEBREW_EPOCH_JDN:](#CAL_HEBREW_EPOCH_JDN:) (const)
- [CAL_HEBREW_DAY_PARTS:](#CAL_HEBREW_DAY_PARTS:) (const)
- [CAL_HEBREW_MONTH_PARTS:](#CAL_HEBREW_MONTH_PARTS:) (const)
- [cal_hebrew_is_leap](#cal_hebrew_is_leap) (function)
- [cal_hebrew_months_in_year](#cal_hebrew_months_in_year) (function)
- [cal_hebrew_molad](#cal_hebrew_molad) (function)
- [cal_hebrew_new_year](#cal_hebrew_new_year) (function)
- [cal_hebrew_year_length](#cal_hebrew_year_length) (function)
- [cal_hebrew_days_in_month](#cal_hebrew_days_in_month) (function)
- [cal_hebrew_is_valid](#cal_hebrew_is_valid) (function)
- [cal_hebrew_to_days](#cal_hebrew_to_days) (function)
- [cal_days_to_hebrew](#cal_days_to_hebrew) (function)
- [cal_gregorian_to_hebrew](#cal_gregorian_to_hebrew) (function)
- [cal_hebrew_to_gregorian](#cal_hebrew_to_gregorian) (function)
- [cal_hebrew_month_name](#cal_hebrew_month_name) (function)

## CalDate

A calendar date (which calendar depends on the function that made it).

```fides
pub struct CalDate {
```

**Fields:**

| Name | Type |
|------|------|
| `year` | `i64` |
| `month` | `i64` |
| `day` | `i64` |

*Defined at line 38*

---

## CalIsoWeek

An ISO 8601 week date: ISO year, week 1..53, weekday 1 (Monday) .. 7.

```fides
pub struct CalIsoWeek {
```

**Fields:**

| Name | Type |
|------|------|
| `year` | `i64` |
| `week` | `i64` |
| `weekday` | `i64` |

*Defined at line 45*

---

## CAL_JDN_UNIX_EPOCH:

The Julian Day Number of 1970-01-01.

```fides
pub const CAL_JDN_UNIX_EPOCH: i64 = 2440588;
```

*Defined at line 52*

---

## cal_days_from_civil

Days since 1970-01-01 of a proleptic Gregorian date.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(cal_days_from_civil(1970, 1, 2), 1);
assert_eq(cal_date_str(cal_civil_from_days(19723)), "2024-01-01");
assert(cal_is_valid(2024, 2, 29));
assert(!cal_is_valid(2023, 2, 29));
assert_eq(cal_iso_weekday(cal_days_from_civil(2024, 1, 1)), 1);
val w = cal_iso_week(2021, 1, 3);
assert_eq(w.year, 2020);
assert_eq(w.week, 53);
assert_eq(cal_iso_weeks_in_year(2020), 53);
assert(cal_julian_is_leap(1900));
```

```fides
pub fn cal_days_from_civil(year: i64, month: i64, day: i64) -> i64 {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`
- `day`: `i64`

**Returns:** `i64`

*Defined at line 89*

---

## cal_civil_from_days

The proleptic Gregorian date of an epoch day.

```fides
pub fn cal_civil_from_days(days: i64) -> CalDate {
```

**Parameters:**

- `days`: `i64`

**Returns:** `CalDate`

*Defined at line 95*

---

## cal_is_valid

Whether year/month/day is a Gregorian date (month 1..12, the day within
the month, February 29 only in a leap year).

```fides
pub fn cal_is_valid(year: i64, month: i64, day: i64) -> bool {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`
- `day`: `i64`

**Returns:** `bool`

*Defined at line 102*

---

## cal_date_str

"YYYY-MM-DD" (a year below 0 or above 9999 is written as it is).

```fides
pub fn cal_date_str(d: CalDate) -> str {
```

**Parameters:**

- `d`: `CalDate`

**Returns:** `str`

*Defined at line 110*

---

## cal_iso_weekday

The ISO weekday of an epoch day: 1 Monday .. 7 Sunday (1970-01-01 was
a Thursday, 4).

```fides
pub fn cal_iso_weekday(days: i64) -> i64 {
```

**Parameters:**

- `days`: `i64`

**Returns:** `i64`

*Defined at line 128*

---

## cal_jdn_from_days

The Julian Day Number of an epoch day (the JDN of the noon of that day).

```fides
pub fn cal_jdn_from_days(days: i64) -> i64 {
```

**Parameters:**

- `days`: `i64`

**Returns:** `i64`

*Defined at line 135*

---

## cal_days_from_jdn

The epoch day of a Julian Day Number.

```fides
pub fn cal_days_from_jdn(jdn: i64) -> i64 {
```

**Parameters:**

- `jdn`: `i64`

**Returns:** `i64`

*Defined at line 140*

---

## cal_jdn

The Julian Day Number of a Gregorian date (2000-01-01 is 2451545).

```fides
pub fn cal_jdn(year: i64, month: i64, day: i64) -> i64 {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`
- `day`: `i64`

**Returns:** `i64`

*Defined at line 145*

---

## cal_julian_is_leap

Whether a Julian-calendar year is a leap year (every fourth one; year 0
is 1 BC, a leap year).

```fides
pub fn cal_julian_is_leap(year: i64) -> bool {
```

**Parameters:**

- `year`: `i64`

**Returns:** `bool`

*Defined at line 153*

---

## cal_julian_days_in_month

Days in a month of the Julian calendar.

```fides
pub fn cal_julian_days_in_month(year: i64, month: i64) -> i64 {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`

**Returns:** `i64`

*Defined at line 158*

---

## cal_julian_to_days

The epoch day of a Julian-calendar date (the JDN formula of the Julian
calendar, years counted from March so that February ends the year).

```fides
pub fn cal_julian_to_days(year: i64, month: i64, day: i64) -> i64 {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`
- `day`: `i64`

**Returns:** `i64`

*Defined at line 173*

---

## cal_days_to_julian

The Julian-calendar date of an epoch day.

```fides
pub fn cal_days_to_julian(days: i64) -> CalDate {
```

**Parameters:**

- `days`: `i64`

**Returns:** `CalDate`

*Defined at line 182*

---

## cal_gregorian_to_julian

The Julian-calendar date of a Gregorian date (1582-10-15 is 1582-10-05
Julian; 1917-11-07 is 1917-10-25).

```fides
pub fn cal_gregorian_to_julian(year: i64, month: i64, day: i64) -> CalDate {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`
- `day`: `i64`

**Returns:** `CalDate`

*Defined at line 195*

---

## cal_julian_to_gregorian

The Gregorian date of a Julian-calendar date.

```fides
pub fn cal_julian_to_gregorian(year: i64, month: i64, day: i64) -> CalDate {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`
- `day`: `i64`

**Returns:** `CalDate`

*Defined at line 200*

---

## cal_iso_week_of_days

The ISO week date of an epoch day: the week's Thursday decides the ISO
year, and the week number counts from the week of that year's first
Thursday.

```fides
pub fn cal_iso_week_of_days(days: i64) -> CalIsoWeek {
```

**Parameters:**

- `days`: `i64`

**Returns:** `CalIsoWeek`

*Defined at line 209*

---

## cal_iso_week

The ISO week date of a Gregorian date.

```fides
pub fn cal_iso_week(year: i64, month: i64, day: i64) -> CalIsoWeek {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`
- `day`: `i64`

**Returns:** `CalIsoWeek`

*Defined at line 220*

---

## cal_iso_weeks_in_year

How many ISO weeks a year has: 53 when it starts on a Thursday, or on
a Wednesday in a leap year; 52 otherwise.

```fides
pub fn cal_iso_weeks_in_year(year: i64) -> i64 {
```

**Parameters:**

- `year`: `i64`

**Returns:** `i64`

*Defined at line 226*

---

## cal_days_from_iso_week

The epoch day of an ISO week date (week 1's Monday is the Monday on or
before January 4). The week and weekday are not checked: week 0 or
weekday 8 count on from there.

```fides
pub fn cal_days_from_iso_week(year: i64, week: i64, weekday: i64) -> i64 {
```

**Parameters:**

- `year`: `i64`
- `week`: `i64`
- `weekday`: `i64`

**Returns:** `i64`

*Defined at line 237*

---

## cal_iso_week_is_valid

Whether an ISO week date exists (week 1..weeks of the year, weekday
1..7).

```fides
pub fn cal_iso_week_is_valid(year: i64, week: i64, weekday: i64) -> bool {
```

**Parameters:**

- `year`: `i64`
- `week`: `i64`
- `weekday`: `i64`

**Returns:** `bool`

*Defined at line 245*

---

## cal_iso_week_str

"YYYY-Www-D", the ISO 8601 extended form.

```fides
pub fn cal_iso_week_str(w: CalIsoWeek) -> str {
```

**Parameters:**

- `w`: `CalIsoWeek`

**Returns:** `str`

*Defined at line 250*

---

## cal_day_of_year

The day of the year, 1..366, of a Gregorian date.

```fides
pub fn cal_day_of_year(year: i64, month: i64, day: i64) -> i64 {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`
- `day`: `i64`

**Returns:** `i64`

*Defined at line 257*

---

## cal_days_from_ordinal

The epoch day of day `doy` (1..366) of `year`.

```fides
pub fn cal_days_from_ordinal(year: i64, doy: i64) -> i64 {
```

**Parameters:**

- `year`: `i64`
- `doy`: `i64`

**Returns:** `i64`

*Defined at line 262*

---

## cal_easter

Western Easter Sunday of a year (Gregorian; the Anonymous Gregorian
algorithm, years 1583 and later).

```fides
pub fn cal_easter(year: i64) -> CalDate {
```

**Parameters:**

- `year`: `i64`

**Returns:** `CalDate`

*Defined at line 270*

---

## cal_julian_easter

Orthodox Easter Sunday as a Julian-calendar date (Meeus' Julian
algorithm); cal_julian_to_gregorian gives the civil date.

```fides
pub fn cal_julian_easter(year: i64) -> CalDate {
```

**Parameters:**

- `year`: `i64`

**Returns:** `CalDate`

*Defined at line 289*

---

## CAL_ISLAMIC_I:

Leap-year rules of the tabular Islamic calendar (see above).

```fides
pub const CAL_ISLAMIC_I: i64 = 15;
```

*Defined at line 340*

---

## CAL_ISLAMIC_II:

```fides
pub const CAL_ISLAMIC_II: i64 = 14;
```

*Defined at line 341*

---

## CAL_ISLAMIC_III:

```fides
pub const CAL_ISLAMIC_III: i64 = 11;
```

*Defined at line 342*

---

## CAL_ISLAMIC_IV:

```fides
pub const CAL_ISLAMIC_IV: i64 = 9;
```

*Defined at line 343*

---

## CAL_ISLAMIC_EPOCH_CIVIL:

1 Muharram AH 1 as a Julian Day Number: the civil epoch (Friday 16 July
622, Julian) and the astronomical one (Thursday 15 July).

```fides
pub const CAL_ISLAMIC_EPOCH_CIVIL: i64 = 1948440;
```

*Defined at line 347*

---

## CAL_ISLAMIC_EPOCH_ASTRONOMICAL:

```fides
pub const CAL_ISLAMIC_EPOCH_ASTRONOMICAL: i64 = 1948439;
```

*Defined at line 348*

---

## cal_islamic_is_leap_rule

Whether an Islamic year is leap (Dhu al-Hijjah has 30 days) by `rule`
(one of CAL_ISLAMIC_I .. CAL_ISLAMIC_IV).

```fides
pub fn cal_islamic_is_leap_rule(year: i64, rule: i64) -> bool {
```

**Parameters:**

- `year`: `i64`
- `rule`: `i64`

**Returns:** `bool`

*Defined at line 352*

---

## cal_islamic_is_leap

Whether an Islamic year is leap by the usual rule (CAL_ISLAMIC_II).

```fides
pub fn cal_islamic_is_leap(year: i64) -> bool {
```

**Parameters:**

- `year`: `i64`

**Returns:** `bool`

*Defined at line 357*

---

## cal_islamic_days_in_month_rule

Days in a month of the tabular Islamic calendar by `rule`.

```fides
pub fn cal_islamic_days_in_month_rule(year: i64, month: i64, rule: i64) -> i64 {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`
- `rule`: `i64`

**Returns:** `i64`

*Defined at line 362*

---

## cal_islamic_days_in_month

Days in a month of the tabular Islamic calendar (CAL_ISLAMIC_II).

```fides
pub fn cal_islamic_days_in_month(year: i64, month: i64) -> i64 {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`

**Returns:** `i64`

*Defined at line 373*

---

## cal_islamic_is_valid_rule

Whether year/month/day is a date of the tabular Islamic calendar by
`rule` (month 1..12, Dhu al-Hijjah 30 only in a leap year).

```fides
pub fn cal_islamic_is_valid_rule(year: i64, month: i64, day: i64, rule: i64) -> bool {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`
- `day`: `i64`
- `rule`: `i64`

**Returns:** `bool`

*Defined at line 379*

---

## cal_islamic_is_valid

Whether year/month/day is a date of the tabular Islamic calendar
(CAL_ISLAMIC_II).

```fides
pub fn cal_islamic_is_valid(year: i64, month: i64, day: i64) -> bool {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`
- `day`: `i64`

**Returns:** `bool`

*Defined at line 388*

---

## cal_islamic_to_days_rule

The epoch day of a tabular Islamic date by `rule` and `epoch` (a JDN:
CAL_ISLAMIC_EPOCH_CIVIL or CAL_ISLAMIC_EPOCH_ASTRONOMICAL). The date is
not checked: day 31 counts on into the next month.

```fides
pub fn cal_islamic_to_days_rule(year: i64, month: i64, day: i64, rule: i64, epoch: i64) -> i64 {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`
- `day`: `i64`
- `rule`: `i64`
- `epoch`: `i64`

**Returns:** `i64`

*Defined at line 407*

---

## cal_days_to_islamic_rule

The tabular Islamic date of an epoch day by `rule` and `epoch`. Years
before AH 1 count down through 0 (1 BH) and below.

```fides
pub fn cal_days_to_islamic_rule(days: i64, rule: i64, epoch: i64) -> CalDate {
```

**Parameters:**

- `days`: `i64`
- `rule`: `i64`
- `epoch`: `i64`

**Returns:** `CalDate`

*Defined at line 413*

---

## cal_islamic_to_days

The epoch day of a civil tabular Islamic date (CAL_ISLAMIC_II, the
Friday epoch).

```fides
pub fn cal_islamic_to_days(year: i64, month: i64, day: i64) -> i64 {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`
- `day`: `i64`

**Returns:** `i64`

*Defined at line 431*

---

## cal_days_to_islamic

The civil tabular Islamic date of an epoch day (CAL_ISLAMIC_II, the
Friday epoch).

```fides
pub fn cal_days_to_islamic(days: i64) -> CalDate {
```

**Parameters:**

- `days`: `i64`

**Returns:** `CalDate`

*Defined at line 437*

---

## cal_gregorian_to_islamic

The civil tabular Islamic date of a Gregorian date (2026-02-18 is
1447-09-01, 1 Ramadan 1447).

```fides
pub fn cal_gregorian_to_islamic(year: i64, month: i64, day: i64) -> CalDate {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`
- `day`: `i64`

**Returns:** `CalDate`

*Defined at line 443*

---

## cal_islamic_to_gregorian

The Gregorian date of a civil tabular Islamic date.

```fides
pub fn cal_islamic_to_gregorian(year: i64, month: i64, day: i64) -> CalDate {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`
- `day`: `i64`

**Returns:** `CalDate`

*Defined at line 448*

---

## cal_islamic_month_name

The English name of an Islamic month 1..12, as CLDR spells it
("Muharram", "Rabiʻ I", "Dhuʻl-Hijjah"); "" for any other number.

```fides
pub fn cal_islamic_month_name(month: i64) -> str {
```

**Parameters:**

- `month`: `i64`

**Returns:** `str`

*Defined at line 454*

---

## CAL_HEBREW_EPOCH_JDN:

1 Tishri AM 1 as a Julian Day Number (Monday 7 October 3761 BC, Julian).

```fides
pub const CAL_HEBREW_EPOCH_JDN: i64 = 347998;
```

*Defined at line 486*

---

## CAL_HEBREW_DAY_PARTS:

Parts (halakim) in a day, and in a mean month (29 d 12 h 793 p).

```fides
pub const CAL_HEBREW_DAY_PARTS: i64 = 25920;
```

*Defined at line 489*

---

## CAL_HEBREW_MONTH_PARTS:

```fides
pub const CAL_HEBREW_MONTH_PARTS: i64 = 765433;
```

*Defined at line 490*

---

## cal_hebrew_is_leap

Whether a Hebrew year has 13 months (years 3 6 8 11 14 17 19 of the
19-year cycle).

```fides
pub fn cal_hebrew_is_leap(year: i64) -> bool {
```

**Parameters:**

- `year`: `i64`

**Returns:** `bool`

*Defined at line 494*

---

## cal_hebrew_months_in_year

Months in a Hebrew year: 12, or 13 in a leap year.

```fides
pub fn cal_hebrew_months_in_year(year: i64) -> i64 {
```

**Parameters:**

- `year`: `i64`

**Returns:** `i64`

*Defined at line 499*

---

## cal_hebrew_molad

The molad of Tishri of a Hebrew year as (day, parts): `day` is the
epoch day of the Hebrew day the molad falls in, a day that began at
6 p.m. of the civil day before; `parts` is the time since that 6 p.m.
in parts (1080 an hour, 25920 a day). Parts below 6480 (6 hours) are the
evening of the civil day `day - 1`; from 6480 on, `parts - 6480` after
midnight of `day`. (Jerusalem mean time, as the calendar counts it.)

```fides
pub fn cal_hebrew_molad(year: i64) -> (i64, i64) {
```

**Parameters:**

- `year`: `i64`

**Returns:** `(i64, i64)`

*Defined at line 517*

---

## cal_hebrew_new_year

The epoch day of 1 Tishri (Rosh Hashanah) of a Hebrew year: the day of
its molad, moved by the four postponements.

```fides
pub fn cal_hebrew_new_year(year: i64) -> i64 {
```

**Parameters:**

- `year`: `i64`

**Returns:** `i64`

*Defined at line 525*

---

## cal_hebrew_year_length

Days in a Hebrew year: 353, 354 or 355 (deficient, regular, complete),
or 383, 384 or 385 in a leap year.

```fides
pub fn cal_hebrew_year_length(year: i64) -> i64 {
```

**Parameters:**

- `year`: `i64`

**Returns:** `i64`

*Defined at line 546*

---

## cal_hebrew_days_in_month

Days in a month of a Hebrew year (0 for a month the year does not
have).

```fides
pub fn cal_hebrew_days_in_month(year: i64, month: i64) -> i64 {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`

**Returns:** `i64`

*Defined at line 569*

---

## cal_hebrew_is_valid

Whether year/month/day is a Hebrew date (month 13 only in a leap year,
the day within the month).

```fides
pub fn cal_hebrew_is_valid(year: i64, month: i64, day: i64) -> bool {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`
- `day`: `i64`

**Returns:** `bool`

*Defined at line 578*

---

## cal_hebrew_to_days

The epoch day of a Hebrew date. The day is not checked (day 31 counts
on); a month the year does not have gives the day after its last month.

```fides
pub fn cal_hebrew_to_days(year: i64, month: i64, day: i64) -> i64 {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`
- `day`: `i64`

**Returns:** `i64`

*Defined at line 584*

---

## cal_days_to_hebrew

The Hebrew date of an epoch day.

```fides
pub fn cal_days_to_hebrew(days: i64) -> CalDate {
```

**Parameters:**

- `days`: `i64`

**Returns:** `CalDate`

*Defined at line 606*

---

## cal_gregorian_to_hebrew

The Hebrew date of a Gregorian date (2026-09-12 is 5787-07-01, Rosh
Hashanah 5787).

```fides
pub fn cal_gregorian_to_hebrew(year: i64, month: i64, day: i64) -> CalDate {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`
- `day`: `i64`

**Returns:** `CalDate`

*Defined at line 637*

---

## cal_hebrew_to_gregorian

The Gregorian date of a Hebrew date.

```fides
pub fn cal_hebrew_to_gregorian(year: i64, month: i64, day: i64) -> CalDate {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`
- `day`: `i64`

**Returns:** `CalDate`

*Defined at line 642*

---

## cal_hebrew_month_name

The English name of a month of a Hebrew year, as CLDR spells it:
"Nisan" .. "Elul", "Tishri" .. "Shevat", then "Adar" in a common year
and "Adar I", "Adar II" in a leap year; "" for a month the year does not
have.

```fides
pub fn cal_hebrew_month_name(year: i64, month: i64) -> str {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`

**Returns:** `str`

*Defined at line 650*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
