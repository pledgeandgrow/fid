[All modules](index.md)

# tariff -- Fides Documentation

## Contents

- [TARIFF_FLAT:](#TARIFF_FLAT:) (const)
- [TARIFF_TOU:](#TARIFF_TOU:) (const)
- [TARIFF_TIERED:](#TARIFF_TIERED:) (const)
- [TARIFF_DYNAMIC:](#TARIFF_DYNAMIC:) (const)
- [TARIFF_EXPORT_NONE:](#TARIFF_EXPORT_NONE:) (const)
- [TARIFF_NET_METERING:](#TARIFF_NET_METERING:) (const)
- [TARIFF_NET_BILLING:](#TARIFF_NET_BILLING:) (const)
- [TARIFF_WEEKDAYS:](#TARIFF_WEEKDAYS:) (const)
- [TARIFF_WEEKENDS:](#TARIFF_WEEKENDS:) (const)
- [TARIFF_ALL_DAYS:](#TARIFF_ALL_DAYS:) (const)
- [TouPeriod](#TouPeriod) (struct)
- [Tariff](#Tariff) (struct)
- [LoadProfile](#LoadProfile) (struct)
- [BillLine](#BillLine) (struct)
- [Bill](#Bill) (struct)
- [tariff_new](#tariff_new) (function)
- [tariff_add_tou](#tariff_add_tou) (function)
- [tariff_add_tier](#tariff_add_tier) (function)
- [tariff_add_dynamic_price](#tariff_add_dynamic_price) (function)
- [tariff_add_dynamic_export_price](#tariff_add_dynamic_export_price) (function)
- [tariff_set_demand](#tariff_set_demand) (function)
- [tariff_set_export](#tariff_set_export) (function)
- [tariff_profile](#tariff_profile) (function)
- [LoadProfile_add](#LoadProfile_add) (function)
- [tariff_hour](#tariff_hour) (function)
- [tariff_dow](#tariff_dow) (function)
- [tariff_period_of](#tariff_period_of) (function)
- [tariff_bill](#tariff_bill) (function)
- [tariff_bill_to_str](#tariff_bill_to_str) (function)
- [tariff_average_rate](#tariff_average_rate) (function)

## TARIFF_FLAT:

```fides
pub const TARIFF_FLAT: i64 = 0;
```

*Defined at line 41*

---

## TARIFF_TOU:

```fides
pub const TARIFF_TOU: i64 = 1;
```

*Defined at line 42*

---

## TARIFF_TIERED:

```fides
pub const TARIFF_TIERED: i64 = 2;
```

*Defined at line 43*

---

## TARIFF_DYNAMIC:

```fides
pub const TARIFF_DYNAMIC: i64 = 3;
```

*Defined at line 44*

---

## TARIFF_EXPORT_NONE:

```fides
pub const TARIFF_EXPORT_NONE: i64 = 0;
```

*Defined at line 46*

---

## TARIFF_NET_METERING:

```fides
pub const TARIFF_NET_METERING: i64 = 1;
```

*Defined at line 47*

---

## TARIFF_NET_BILLING:

```fides
pub const TARIFF_NET_BILLING: i64 = 2;
```

*Defined at line 48*

---

## TARIFF_WEEKDAYS:

Day-of-week masks (bit d = day d, 0 = Monday).

```fides
pub const TARIFF_WEEKDAYS: i64 = 31;
```

*Defined at line 51*

---

## TARIFF_WEEKENDS:

```fides
pub const TARIFF_WEEKENDS: i64 = 96;
```

*Defined at line 52*

---

## TARIFF_ALL_DAYS:

```fides
pub const TARIFF_ALL_DAYS: i64 = 127;
```

*Defined at line 53*

---

## TouPeriod

```fides
pub struct TouPeriod {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `days` | `i64` |
| `start_hour` | `i64` |
| `end_hour` | `i64` |
| `rate` | `Decimal` |

*Defined at line 55*

---

## Tariff

```fides
pub struct Tariff {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `mode` | `i64` |
| `fixed` | `Decimal` |
| `base_rate` | `Decimal` |
| `tou` | `Vec<TouPeriod>` |
| `tier_limits_wh` | `Vec<i64>` |
| `tier_rates` | `Vec<Decimal>` |
| `dynamic_prices` | `Vec<Decimal>` |
| `demand_rate` | `Decimal` |
| `demand_period` | `str` |
| `export_mode` | `i64` |
| `export_rate` | `Decimal` |
| `excess_rate` | `Decimal` |
| `dynamic_export` | `Vec<Decimal>` |

*Defined at line 63*

---

## LoadProfile

```fides
pub struct LoadProfile {
```

**Fields:**

| Name | Type |
|------|------|
| `start_dow` | `i64` |
| `interval_minutes` | `i64` |
| `import_wh` | `Vec<i64>` |
| `export_wh` | `Vec<i64>` |

*Defined at line 80*

---

## BillLine

```fides
pub struct BillLine {
```

**Fields:**

| Name | Type |
|------|------|
| `label` | `str` |
| `quantity` | `Decimal` |
| `unit` | `str` |
| `rate` | `Decimal` |
| `amount` | `Decimal` |

*Defined at line 87*

---

## Bill

```fides
pub struct Bill {
```

**Fields:**

| Name | Type |
|------|------|
| `tariff` | `str` |
| `lines` | `Vec<BillLine>` |
| `total` | `Decimal` |
| `import_wh` | `i64` |
| `export_wh` | `i64` |
| `peak_kw` | `Decimal` |

*Defined at line 95*

---

## tariff_new

A tariff of `mode` with a fixed charge and a base rate (the flat rate,
the off-peak rate for TOU), amounts as decimal text.

```fides
pub fn tariff_new(name: str, mode: i64, fixed: str, base_rate: str) -> Tariff {
```

**Parameters:**

- `name`: `str`
- `mode`: `i64`
- `fixed`: `str`
- `base_rate`: `str`

**Returns:** `Tariff`

*Defined at line 108*

---

## tariff_add_tou

A time-of-use period: days mask, hours [start, end) (end <= start wraps
past midnight), rate per kWh.

```fides
pub fn tariff_add_tou(t: &mut Tariff, name: str, days: i64, start_hour: i64, end_hour: i64, rate: str) {
```

**Parameters:**

- `t`: `&mut Tariff`
- `name`: `str`
- `days`: `i64`
- `start_hour`: `i64`
- `end_hour`: `i64`
- `rate`: `str`

*Defined at line 116*

---

## tariff_add_tier

A block of the tiered schedule: up to `upto_kwh` (cumulative, -1 for
the last, unbounded block) at `rate`. Blocks are added in order.

```fides
pub fn tariff_add_tier(t: &mut Tariff, upto_kwh: i64, rate: str) {
```

**Parameters:**

- `t`: `&mut Tariff`
- `upto_kwh`: `i64`
- `rate`: `str`

*Defined at line 124*

---

## tariff_add_dynamic_price

```fides
pub fn tariff_add_dynamic_price(t: &mut Tariff, price: str) {
```

**Parameters:**

- `t`: `&mut Tariff`
- `price`: `str`

*Defined at line 133*

---

## tariff_add_dynamic_export_price

```fides
pub fn tariff_add_dynamic_export_price(t: &mut Tariff, price: str) {
```

**Parameters:**

- `t`: `&mut Tariff`
- `price`: `str`

*Defined at line 137*

---

## tariff_set_demand

Demand charge per kW of the highest interval demand; `period` names a
TOU period to restrict it to ("" for all hours).

```fides
pub fn tariff_set_demand(t: &mut Tariff, rate: str, period: str) {
```

**Parameters:**

- `t`: `&mut Tariff`
- `rate`: `str`
- `period`: `str`

*Defined at line 143*

---

## tariff_set_export

```fides
pub fn tariff_set_export(t: &mut Tariff, mode: i64, export_rate: str, excess_rate: str) {
```

**Parameters:**

- `t`: `&mut Tariff`
- `mode`: `i64`
- `export_rate`: `str`
- `excess_rate`: `str`

*Defined at line 148*

---

## tariff_profile

```fides
pub fn tariff_profile(start_dow: i64, interval_minutes: i64) -> LoadProfile {
```

**Parameters:**

- `start_dow`: `i64`
- `interval_minutes`: `i64`

**Returns:** `LoadProfile`

*Defined at line 154*

---

## LoadProfile_add

```fides
pub fn LoadProfile_add(p: &mut LoadProfile, import_wh: i64, export_wh: i64) {
```

**Parameters:**

- `p`: `&mut LoadProfile`
- `import_wh`: `i64`
- `export_wh`: `i64`

*Defined at line 160*

---

## tariff_hour

Hour of day (0..23) of interval i.

```fides
pub fn tariff_hour(p: &LoadProfile, i: i64) -> i64 {
```

**Parameters:**

- `p`: `&LoadProfile`
- `i`: `i64`

**Returns:** `i64`

*Defined at line 168*

---

## tariff_dow

Day of week (0 = Monday) of interval i.

```fides
pub fn tariff_dow(p: &LoadProfile, i: i64) -> i64 {
```

**Parameters:**

- `p`: `&LoadProfile`
- `i`: `i64`

**Returns:** `i64`

*Defined at line 173*

---

## tariff_period_of

Index of the TOU period of interval i, or -1 (off-peak / base rate).

```fides
pub fn tariff_period_of(t: &Tariff, p: &LoadProfile, i: i64) -> i64 {
```

**Parameters:**

- `t`: `&Tariff`
- `p`: `&LoadProfile`
- `i`: `i64`

**Returns:** `i64`

*Defined at line 188*

---

## tariff_bill

```fides
pub fn tariff_bill(t: &Tariff, p: &LoadProfile) -> Bill {
```

**Parameters:**

- `t`: `&Tariff`
- `p`: `&LoadProfile`

**Returns:** `Bill`

*Defined at line 238*

---

## tariff_bill_to_str

The bill as text, one line per charge and a total.

```fides
pub fn tariff_bill_to_str(b: &Bill) -> str {
```

**Parameters:**

- `b`: `&Bill`

**Returns:** `str`

*Defined at line 385*

---

## tariff_average_rate

Average cost per imported kWh, to 5 decimals (0 when nothing imported).

```fides
pub fn tariff_average_rate(b: &Bill) -> Decimal {
```

**Parameters:**

- `b`: `&Bill`

**Returns:** `Decimal`

*Defined at line 401*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
