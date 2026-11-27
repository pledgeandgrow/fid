[All modules](index.md)

# units -- Fides Documentation

## Contents

- [Meters](#Meters) (struct)
- [Kilograms](#Kilograms) (struct)
- [Seconds](#Seconds) (struct)
- [Kelvin](#Kelvin) (struct)
- [Amperes](#Amperes) (struct)
- [Moles](#Moles) (struct)
- [Candelas](#Candelas) (struct)
- [MetersPerSecond](#MetersPerSecond) (struct)
- [MetersPerSecond2](#MetersPerSecond2) (struct)
- [Newtons](#Newtons) (struct)
- [Joules](#Joules) (struct)
- [Watts](#Watts) (struct)
- [Pascals](#Pascals) (struct)
- [Hertz](#Hertz) (struct)
- [SquareMeters](#SquareMeters) (struct)
- [CubicMeters](#CubicMeters) (struct)
- [meters](#meters) (function)
- [kilograms](#kilograms) (function)
- [seconds](#seconds) (function)
- [kelvin](#kelvin) (function)
- [amperes](#amperes) (function)
- [moles](#moles) (function)
- [candelas](#candelas) (function)
- [meters_per_second](#meters_per_second) (function)
- [meters_per_second2](#meters_per_second2) (function)
- [newtons](#newtons) (function)
- [joules](#joules) (function)
- [watts](#watts) (function)
- [pascals](#pascals) (function)
- [hertz](#hertz) (function)
- [square_meters](#square_meters) (function)
- [cubic_meters](#cubic_meters) (function)
- [km_to_m](#km_to_m) (function)
- [m_to_km](#m_to_km) (function)
- [cm_to_m](#cm_to_m) (function)
- [m_to_cm](#m_to_cm) (function)
- [mm_to_m](#mm_to_m) (function)
- [m_to_mm](#m_to_mm) (function)
- [miles_to_m](#miles_to_m) (function)
- [m_to_miles](#m_to_miles) (function)
- [feet_to_m](#feet_to_m) (function)
- [m_to_feet](#m_to_feet) (function)
- [inches_to_m](#inches_to_m) (function)
- [m_to_inches](#m_to_inches) (function)
- [g_to_kg](#g_to_kg) (function)
- [kg_to_g](#kg_to_g) (function)
- [lb_to_kg](#lb_to_kg) (function)
- [kg_to_lb](#kg_to_lb) (function)
- [celsius_to_kelvin](#celsius_to_kelvin) (function)
- [kelvin_to_celsius](#kelvin_to_celsius) (function)
- [fahrenheit_to_kelvin](#fahrenheit_to_kelvin) (function)
- [kelvin_to_fahrenheit](#kelvin_to_fahrenheit) (function)
- [minutes_to_s](#minutes_to_s) (function)
- [hours_to_s](#hours_to_s) (function)
- [days_to_s](#days_to_s) (function)
- [s_to_minutes](#s_to_minutes) (function)
- [s_to_hours](#s_to_hours) (function)
- [velocity](#velocity) (function)
- [acceleration](#acceleration) (function)
- [force](#force) (function)
- [energy](#energy) (function)
- [power](#power) (function)
- [pressure](#pressure) (function)
- [frequency](#frequency) (function)
- [area](#area) (function)
- [volume](#volume) (function)
- [meters_add](#meters_add) (function)
- [meters_sub](#meters_sub) (function)
- [seconds_add](#seconds_add) (function)
- [seconds_sub](#seconds_sub) (function)
- [kg_add](#kg_add) (function)
- [kg_sub](#kg_sub) (function)
- [newtons_add](#newtons_add) (function)
- [newtons_sub](#newtons_sub) (function)
- [joules_add](#joules_add) (function)
- [joules_sub](#joules_sub) (function)
- [watts_add](#watts_add) (function)
- [watts_sub](#watts_sub) (function)
- [kelvin_add](#kelvin_add) (function)
- [kelvin_sub](#kelvin_sub) (function)
- [amperes_add](#amperes_add) (function)
- [amperes_sub](#amperes_sub) (function)
- [moles_add](#moles_add) (function)
- [moles_sub](#moles_sub) (function)
- [candelas_add](#candelas_add) (function)
- [candelas_sub](#candelas_sub) (function)
- [mps_add](#mps_add) (function)
- [mps_sub](#mps_sub) (function)
- [mps2_add](#mps2_add) (function)
- [mps2_sub](#mps2_sub) (function)
- [pascals_add](#pascals_add) (function)
- [pascals_sub](#pascals_sub) (function)
- [hertz_add](#hertz_add) (function)
- [hertz_sub](#hertz_sub) (function)
- [m2_add](#m2_add) (function)
- [m2_sub](#m2_sub) (function)
- [m3_add](#m3_add) (function)
- [m3_sub](#m3_sub) (function)
- [meters_scale](#meters_scale) (function)
- [meters_lt](#meters_lt) (function)
- [meters_ratio](#meters_ratio) (function)
- [seconds_scale](#seconds_scale) (function)
- [seconds_lt](#seconds_lt) (function)
- [seconds_ratio](#seconds_ratio) (function)
- [kg_scale](#kg_scale) (function)
- [kg_lt](#kg_lt) (function)
- [kg_ratio](#kg_ratio) (function)
- [newtons_scale](#newtons_scale) (function)
- [newtons_lt](#newtons_lt) (function)
- [newtons_ratio](#newtons_ratio) (function)
- [joules_scale](#joules_scale) (function)
- [joules_lt](#joules_lt) (function)
- [joules_ratio](#joules_ratio) (function)
- [watts_scale](#watts_scale) (function)
- [watts_lt](#watts_lt) (function)
- [watts_ratio](#watts_ratio) (function)
- [travel](#travel) (function)
- [travel_time](#travel_time) (function)
- [mass_from_force](#mass_from_force) (function)
- [SPEED_OF_LIGHT:](#SPEED_OF_LIGHT:) (const)
- [G_GRAVITATIONAL:](#G_GRAVITATIONAL:) (const)
- [G_EARTH:](#G_EARTH:) (const)
- [AVOGADRO:](#AVOGADRO:) (const)
- [BOLTZMANN:](#BOLTZMANN:) (const)
- [PLANCK:](#PLANCK:) (const)
- [ELEMENTARY_CHARGE:](#ELEMENTARY_CHARGE:) (const)
- [GAS_CONSTANT:](#GAS_CONSTANT:) (const)
- [ATM_PRESSURE:](#ATM_PRESSURE:) (const)
- [ABSOLUTE_ZERO_C:](#ABSOLUTE_ZERO_C:) (const)
- [Meters](#Meters) (impl)
- [Seconds](#Seconds) (impl)
- [Kilograms](#Kilograms) (impl)
- [Newtons](#Newtons) (impl)
- [Joules](#Joules) (impl)
- [Watts](#Watts) (impl)
- [Kelvin](#Kelvin) (impl)
- [Amperes](#Amperes) (impl)
- [Moles](#Moles) (impl)
- [Candelas](#Candelas) (impl)
- [MetersPerSecond](#MetersPerSecond) (impl)
- [Pascals](#Pascals) (impl)
- [Hertz](#Hertz) (impl)

## Meters

```fides
pub struct Meters {
```

**Fields:**

| Name | Type |
|------|------|
| `value` | `f64` |

*Defined at line 26*

---

## Kilograms

```fides
pub struct Kilograms {
```

**Fields:**

| Name | Type |
|------|------|
| `value` | `f64` |

*Defined at line 31*

---

## Seconds

```fides
pub struct Seconds {
```

**Fields:**

| Name | Type |
|------|------|
| `value` | `f64` |

*Defined at line 36*

---

## Kelvin

```fides
pub struct Kelvin {
```

**Fields:**

| Name | Type |
|------|------|
| `value` | `f64` |

*Defined at line 41*

---

## Amperes

```fides
pub struct Amperes {
```

**Fields:**

| Name | Type |
|------|------|
| `value` | `f64` |

*Defined at line 46*

---

## Moles

```fides
pub struct Moles {
```

**Fields:**

| Name | Type |
|------|------|
| `value` | `f64` |

*Defined at line 51*

---

## Candelas

```fides
pub struct Candelas {
```

**Fields:**

| Name | Type |
|------|------|
| `value` | `f64` |

*Defined at line 56*

---

## MetersPerSecond

```fides
pub struct MetersPerSecond {
```

**Fields:**

| Name | Type |
|------|------|
| `value` | `f64` |

*Defined at line 64*

---

## MetersPerSecond2

```fides
pub struct MetersPerSecond2 {
```

**Fields:**

| Name | Type |
|------|------|
| `value` | `f64` |

*Defined at line 69*

---

## Newtons

```fides
pub struct Newtons {
```

**Fields:**

| Name | Type |
|------|------|
| `value` | `f64` |

*Defined at line 74*

---

## Joules

```fides
pub struct Joules {
```

**Fields:**

| Name | Type |
|------|------|
| `value` | `f64` |

*Defined at line 79*

---

## Watts

```fides
pub struct Watts {
```

**Fields:**

| Name | Type |
|------|------|
| `value` | `f64` |

*Defined at line 84*

---

## Pascals

```fides
pub struct Pascals {
```

**Fields:**

| Name | Type |
|------|------|
| `value` | `f64` |

*Defined at line 89*

---

## Hertz

```fides
pub struct Hertz {
```

**Fields:**

| Name | Type |
|------|------|
| `value` | `f64` |

*Defined at line 94*

---

## SquareMeters

```fides
pub struct SquareMeters {
```

**Fields:**

| Name | Type |
|------|------|
| `value` | `f64` |

*Defined at line 99*

---

## CubicMeters

```fides
pub struct CubicMeters {
```

**Fields:**

| Name | Type |
|------|------|
| `value` | `f64` |

*Defined at line 104*

---

## meters

A length in meters.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
val v: MetersPerSecond = velocity(meters(100.0), seconds(10.0));
assert(v.value == 10.0);
assert(travel(v, seconds(25.0)).value == 250.0);
assert(travel_time(meters(250.0), v).value == 25.0);
assert(meters_add(meters(1.0), meters(2.0)).value == 3.0);
assert(kelvin_add(kelvin(273.0), kelvin(27.0)).value == 300.0);
```

```fides
pub fn meters(v: f64) -> Meters {
```

**Parameters:**

- `v`: `f64`

**Returns:** `Meters`

*Defined at line 119*

---

## kilograms

```fides
pub fn kilograms(v: f64) -> Kilograms {
```

**Parameters:**

- `v`: `f64`

**Returns:** `Kilograms`

*Defined at line 124*

---

## seconds

```fides
pub fn seconds(v: f64) -> Seconds {
```

**Parameters:**

- `v`: `f64`

**Returns:** `Seconds`

*Defined at line 129*

---

## kelvin

```fides
pub fn kelvin(v: f64) -> Kelvin {
```

**Parameters:**

- `v`: `f64`

**Returns:** `Kelvin`

*Defined at line 134*

---

## amperes

```fides
pub fn amperes(v: f64) -> Amperes {
```

**Parameters:**

- `v`: `f64`

**Returns:** `Amperes`

*Defined at line 139*

---

## moles

```fides
pub fn moles(v: f64) -> Moles {
```

**Parameters:**

- `v`: `f64`

**Returns:** `Moles`

*Defined at line 144*

---

## candelas

```fides
pub fn candelas(v: f64) -> Candelas {
```

**Parameters:**

- `v`: `f64`

**Returns:** `Candelas`

*Defined at line 149*

---

## meters_per_second

```fides
pub fn meters_per_second(v: f64) -> MetersPerSecond {
```

**Parameters:**

- `v`: `f64`

**Returns:** `MetersPerSecond`

*Defined at line 155*

---

## meters_per_second2

```fides
pub fn meters_per_second2(v: f64) -> MetersPerSecond2 {
```

**Parameters:**

- `v`: `f64`

**Returns:** `MetersPerSecond2`

*Defined at line 160*

---

## newtons

```fides
pub fn newtons(v: f64) -> Newtons {
```

**Parameters:**

- `v`: `f64`

**Returns:** `Newtons`

*Defined at line 165*

---

## joules

```fides
pub fn joules(v: f64) -> Joules {
```

**Parameters:**

- `v`: `f64`

**Returns:** `Joules`

*Defined at line 170*

---

## watts

```fides
pub fn watts(v: f64) -> Watts {
```

**Parameters:**

- `v`: `f64`

**Returns:** `Watts`

*Defined at line 175*

---

## pascals

```fides
pub fn pascals(v: f64) -> Pascals {
```

**Parameters:**

- `v`: `f64`

**Returns:** `Pascals`

*Defined at line 180*

---

## hertz

```fides
pub fn hertz(v: f64) -> Hertz {
```

**Parameters:**

- `v`: `f64`

**Returns:** `Hertz`

*Defined at line 185*

---

## square_meters

```fides
pub fn square_meters(v: f64) -> SquareMeters {
```

**Parameters:**

- `v`: `f64`

**Returns:** `SquareMeters`

*Defined at line 190*

---

## cubic_meters

```fides
pub fn cubic_meters(v: f64) -> CubicMeters {
```

**Parameters:**

- `v`: `f64`

**Returns:** `CubicMeters`

*Defined at line 195*

---

## km_to_m

Convert kilometers to meters.

```fides
pub fn km_to_m(km: f64) -> Meters {
```

**Parameters:**

- `km`: `f64`

**Returns:** `Meters`

*Defined at line 204*

---

## m_to_km

Convert meters to kilometers.

```fides
pub fn m_to_km(m: Meters) -> f64 {
```

**Parameters:**

- `m`: `Meters`

**Returns:** `f64`

*Defined at line 209*

---

## cm_to_m

Convert centimeters to meters.

```fides
pub fn cm_to_m(cm: f64) -> Meters {
```

**Parameters:**

- `cm`: `f64`

**Returns:** `Meters`

*Defined at line 214*

---

## m_to_cm

Convert meters to centimeters.

```fides
pub fn m_to_cm(m: Meters) -> f64 {
```

**Parameters:**

- `m`: `Meters`

**Returns:** `f64`

*Defined at line 219*

---

## mm_to_m

Convert millimeters to meters.

```fides
pub fn mm_to_m(mm: f64) -> Meters {
```

**Parameters:**

- `mm`: `f64`

**Returns:** `Meters`

*Defined at line 224*

---

## m_to_mm

Convert meters to millimeters.

```fides
pub fn m_to_mm(m: Meters) -> f64 {
```

**Parameters:**

- `m`: `Meters`

**Returns:** `f64`

*Defined at line 229*

---

## miles_to_m

Convert miles to meters.

```fides
pub fn miles_to_m(miles: f64) -> Meters {
```

**Parameters:**

- `miles`: `f64`

**Returns:** `Meters`

*Defined at line 234*

---

## m_to_miles

Convert meters to miles.

```fides
pub fn m_to_miles(m: Meters) -> f64 {
```

**Parameters:**

- `m`: `Meters`

**Returns:** `f64`

*Defined at line 239*

---

## feet_to_m

Convert feet to meters.

```fides
pub fn feet_to_m(ft: f64) -> Meters {
```

**Parameters:**

- `ft`: `f64`

**Returns:** `Meters`

*Defined at line 244*

---

## m_to_feet

Convert meters to feet.

```fides
pub fn m_to_feet(m: Meters) -> f64 {
```

**Parameters:**

- `m`: `Meters`

**Returns:** `f64`

*Defined at line 249*

---

## inches_to_m

Convert inches to meters.

```fides
pub fn inches_to_m(inch: f64) -> Meters {
```

**Parameters:**

- `inch`: `f64`

**Returns:** `Meters`

*Defined at line 254*

---

## m_to_inches

Convert meters to inches.

```fides
pub fn m_to_inches(m: Meters) -> f64 {
```

**Parameters:**

- `m`: `Meters`

**Returns:** `f64`

*Defined at line 259*

---

## g_to_kg

Convert grams to kilograms.

```fides
pub fn g_to_kg(g: f64) -> Kilograms {
```

**Parameters:**

- `g`: `f64`

**Returns:** `Kilograms`

*Defined at line 264*

---

## kg_to_g

Convert kilograms to grams.

```fides
pub fn kg_to_g(kg: Kilograms) -> f64 {
```

**Parameters:**

- `kg`: `Kilograms`

**Returns:** `f64`

*Defined at line 269*

---

## lb_to_kg

Convert pounds to kilograms.

```fides
pub fn lb_to_kg(lb: f64) -> Kilograms {
```

**Parameters:**

- `lb`: `f64`

**Returns:** `Kilograms`

*Defined at line 274*

---

## kg_to_lb

Convert kilograms to pounds.

```fides
pub fn kg_to_lb(kg: Kilograms) -> f64 {
```

**Parameters:**

- `kg`: `Kilograms`

**Returns:** `f64`

*Defined at line 279*

---

## celsius_to_kelvin

Convert Celsius to Kelvin.

```fides
pub fn celsius_to_kelvin(c: f64) -> Kelvin {
```

**Parameters:**

- `c`: `f64`

**Returns:** `Kelvin`

*Defined at line 284*

---

## kelvin_to_celsius

Convert Kelvin to Celsius.

```fides
pub fn kelvin_to_celsius(k: Kelvin) -> f64 {
```

**Parameters:**

- `k`: `Kelvin`

**Returns:** `f64`

*Defined at line 289*

---

## fahrenheit_to_kelvin

Convert Fahrenheit to Kelvin.

```fides
pub fn fahrenheit_to_kelvin(f: f64) -> Kelvin {
```

**Parameters:**

- `f`: `f64`

**Returns:** `Kelvin`

*Defined at line 294*

---

## kelvin_to_fahrenheit

Convert Kelvin to Fahrenheit.

```fides
pub fn kelvin_to_fahrenheit(k: Kelvin) -> f64 {
```

**Parameters:**

- `k`: `Kelvin`

**Returns:** `f64`

*Defined at line 299*

---

## minutes_to_s

Convert minutes to seconds.

```fides
pub fn minutes_to_s(min: f64) -> Seconds {
```

**Parameters:**

- `min`: `f64`

**Returns:** `Seconds`

*Defined at line 304*

---

## hours_to_s

Convert hours to seconds.

```fides
pub fn hours_to_s(h: f64) -> Seconds {
```

**Parameters:**

- `h`: `f64`

**Returns:** `Seconds`

*Defined at line 309*

---

## days_to_s

Convert days to seconds.

```fides
pub fn days_to_s(d: f64) -> Seconds {
```

**Parameters:**

- `d`: `f64`

**Returns:** `Seconds`

*Defined at line 314*

---

## s_to_minutes

Convert seconds to minutes.

```fides
pub fn s_to_minutes(s: Seconds) -> f64 {
```

**Parameters:**

- `s`: `Seconds`

**Returns:** `f64`

*Defined at line 319*

---

## s_to_hours

Convert seconds to hours.

```fides
pub fn s_to_hours(s: Seconds) -> f64 {
```

**Parameters:**

- `s`: `Seconds`

**Returns:** `f64`

*Defined at line 324*

---

## velocity

Velocity = distance / time.

```fides
pub fn velocity(d: Meters, t: Seconds) -> MetersPerSecond {
```

**Parameters:**

- `d`: `Meters`
- `t`: `Seconds`

**Returns:** `MetersPerSecond`

*Defined at line 331*

---

## acceleration

Acceleration = velocity / time.

```fides
pub fn acceleration(v: MetersPerSecond, t: Seconds) -> MetersPerSecond2 {
```

**Parameters:**

- `v`: `MetersPerSecond`
- `t`: `Seconds`

**Returns:** `MetersPerSecond2`

*Defined at line 336*

---

## force

Force = mass × acceleration (F = ma).

```fides
pub fn force(m: Kilograms, a: MetersPerSecond2) -> Newtons {
```

**Parameters:**

- `m`: `Kilograms`
- `a`: `MetersPerSecond2`

**Returns:** `Newtons`

*Defined at line 343*

---

## energy

Energy = force × distance (E = Fd).

```fides
pub fn energy(f: Newtons, d: Meters) -> Joules {
```

**Parameters:**

- `f`: `Newtons`
- `d`: `Meters`

**Returns:** `Joules`

*Defined at line 348*

---

## power

Power = energy / time (P = E/t).

```fides
pub fn power(e: Joules, t: Seconds) -> Watts {
```

**Parameters:**

- `e`: `Joules`
- `t`: `Seconds`

**Returns:** `Watts`

*Defined at line 353*

---

## pressure

Pressure = force / area.

```fides
pub fn pressure(f: Newtons, a: SquareMeters) -> Pascals {
```

**Parameters:**

- `f`: `Newtons`
- `a`: `SquareMeters`

**Returns:** `Pascals`

*Defined at line 358*

---

## frequency

Frequency = 1 / period.

```fides
pub fn frequency(period: Seconds) -> Hertz {
```

**Parameters:**

- `period`: `Seconds`

**Returns:** `Hertz`

*Defined at line 363*

---

## area

Area = length × width.

```fides
pub fn area(length: Meters, width: Meters) -> SquareMeters {
```

**Parameters:**

- `length`: `Meters`
- `width`: `Meters`

**Returns:** `SquareMeters`

*Defined at line 368*

---

## volume

Volume = length × width × height.

```fides
pub fn volume(length: Meters, width: Meters, height: Meters) -> CubicMeters {
```

**Parameters:**

- `length`: `Meters`
- `width`: `Meters`
- `height`: `Meters`

**Returns:** `CubicMeters`

*Defined at line 373*

---

## meters_add

```fides
pub fn meters_add(a: Meters, b: Meters) -> Meters {
```

**Parameters:**

- `a`: `Meters`
- `b`: `Meters`

**Returns:** `Meters`

*Defined at line 379*

---

## meters_sub

```fides
pub fn meters_sub(a: Meters, b: Meters) -> Meters {
```

**Parameters:**

- `a`: `Meters`
- `b`: `Meters`

**Returns:** `Meters`

*Defined at line 382*

---

## seconds_add

```fides
pub fn seconds_add(a: Seconds, b: Seconds) -> Seconds {
```

**Parameters:**

- `a`: `Seconds`
- `b`: `Seconds`

**Returns:** `Seconds`

*Defined at line 385*

---

## seconds_sub

```fides
pub fn seconds_sub(a: Seconds, b: Seconds) -> Seconds {
```

**Parameters:**

- `a`: `Seconds`
- `b`: `Seconds`

**Returns:** `Seconds`

*Defined at line 388*

---

## kg_add

```fides
pub fn kg_add(a: Kilograms, b: Kilograms) -> Kilograms {
```

**Parameters:**

- `a`: `Kilograms`
- `b`: `Kilograms`

**Returns:** `Kilograms`

*Defined at line 391*

---

## kg_sub

```fides
pub fn kg_sub(a: Kilograms, b: Kilograms) -> Kilograms {
```

**Parameters:**

- `a`: `Kilograms`
- `b`: `Kilograms`

**Returns:** `Kilograms`

*Defined at line 394*

---

## newtons_add

```fides
pub fn newtons_add(a: Newtons, b: Newtons) -> Newtons {
```

**Parameters:**

- `a`: `Newtons`
- `b`: `Newtons`

**Returns:** `Newtons`

*Defined at line 397*

---

## newtons_sub

```fides
pub fn newtons_sub(a: Newtons, b: Newtons) -> Newtons {
```

**Parameters:**

- `a`: `Newtons`
- `b`: `Newtons`

**Returns:** `Newtons`

*Defined at line 400*

---

## joules_add

```fides
pub fn joules_add(a: Joules, b: Joules) -> Joules {
```

**Parameters:**

- `a`: `Joules`
- `b`: `Joules`

**Returns:** `Joules`

*Defined at line 403*

---

## joules_sub

```fides
pub fn joules_sub(a: Joules, b: Joules) -> Joules {
```

**Parameters:**

- `a`: `Joules`
- `b`: `Joules`

**Returns:** `Joules`

*Defined at line 406*

---

## watts_add

```fides
pub fn watts_add(a: Watts, b: Watts) -> Watts {
```

**Parameters:**

- `a`: `Watts`
- `b`: `Watts`

**Returns:** `Watts`

*Defined at line 409*

---

## watts_sub

```fides
pub fn watts_sub(a: Watts, b: Watts) -> Watts {
```

**Parameters:**

- `a`: `Watts`
- `b`: `Watts`

**Returns:** `Watts`

*Defined at line 412*

---

## kelvin_add

```fides
pub fn kelvin_add(a: Kelvin, b: Kelvin) -> Kelvin {
```

**Parameters:**

- `a`: `Kelvin`
- `b`: `Kelvin`

**Returns:** `Kelvin`

*Defined at line 417*

---

## kelvin_sub

```fides
pub fn kelvin_sub(a: Kelvin, b: Kelvin) -> Kelvin {
```

**Parameters:**

- `a`: `Kelvin`
- `b`: `Kelvin`

**Returns:** `Kelvin`

*Defined at line 420*

---

## amperes_add

```fides
pub fn amperes_add(a: Amperes, b: Amperes) -> Amperes {
```

**Parameters:**

- `a`: `Amperes`
- `b`: `Amperes`

**Returns:** `Amperes`

*Defined at line 423*

---

## amperes_sub

```fides
pub fn amperes_sub(a: Amperes, b: Amperes) -> Amperes {
```

**Parameters:**

- `a`: `Amperes`
- `b`: `Amperes`

**Returns:** `Amperes`

*Defined at line 426*

---

## moles_add

```fides
pub fn moles_add(a: Moles, b: Moles) -> Moles {
```

**Parameters:**

- `a`: `Moles`
- `b`: `Moles`

**Returns:** `Moles`

*Defined at line 429*

---

## moles_sub

```fides
pub fn moles_sub(a: Moles, b: Moles) -> Moles {
```

**Parameters:**

- `a`: `Moles`
- `b`: `Moles`

**Returns:** `Moles`

*Defined at line 432*

---

## candelas_add

```fides
pub fn candelas_add(a: Candelas, b: Candelas) -> Candelas {
```

**Parameters:**

- `a`: `Candelas`
- `b`: `Candelas`

**Returns:** `Candelas`

*Defined at line 435*

---

## candelas_sub

```fides
pub fn candelas_sub(a: Candelas, b: Candelas) -> Candelas {
```

**Parameters:**

- `a`: `Candelas`
- `b`: `Candelas`

**Returns:** `Candelas`

*Defined at line 438*

---

## mps_add

```fides
pub fn mps_add(a: MetersPerSecond, b: MetersPerSecond) -> MetersPerSecond {
```

**Parameters:**

- `a`: `MetersPerSecond`
- `b`: `MetersPerSecond`

**Returns:** `MetersPerSecond`

*Defined at line 441*

---

## mps_sub

```fides
pub fn mps_sub(a: MetersPerSecond, b: MetersPerSecond) -> MetersPerSecond {
```

**Parameters:**

- `a`: `MetersPerSecond`
- `b`: `MetersPerSecond`

**Returns:** `MetersPerSecond`

*Defined at line 444*

---

## mps2_add

```fides
pub fn mps2_add(a: MetersPerSecond2, b: MetersPerSecond2) -> MetersPerSecond2 {
```

**Parameters:**

- `a`: `MetersPerSecond2`
- `b`: `MetersPerSecond2`

**Returns:** `MetersPerSecond2`

*Defined at line 447*

---

## mps2_sub

```fides
pub fn mps2_sub(a: MetersPerSecond2, b: MetersPerSecond2) -> MetersPerSecond2 {
```

**Parameters:**

- `a`: `MetersPerSecond2`
- `b`: `MetersPerSecond2`

**Returns:** `MetersPerSecond2`

*Defined at line 450*

---

## pascals_add

```fides
pub fn pascals_add(a: Pascals, b: Pascals) -> Pascals {
```

**Parameters:**

- `a`: `Pascals`
- `b`: `Pascals`

**Returns:** `Pascals`

*Defined at line 453*

---

## pascals_sub

```fides
pub fn pascals_sub(a: Pascals, b: Pascals) -> Pascals {
```

**Parameters:**

- `a`: `Pascals`
- `b`: `Pascals`

**Returns:** `Pascals`

*Defined at line 456*

---

## hertz_add

```fides
pub fn hertz_add(a: Hertz, b: Hertz) -> Hertz {
```

**Parameters:**

- `a`: `Hertz`
- `b`: `Hertz`

**Returns:** `Hertz`

*Defined at line 459*

---

## hertz_sub

```fides
pub fn hertz_sub(a: Hertz, b: Hertz) -> Hertz {
```

**Parameters:**

- `a`: `Hertz`
- `b`: `Hertz`

**Returns:** `Hertz`

*Defined at line 462*

---

## m2_add

```fides
pub fn m2_add(a: SquareMeters, b: SquareMeters) -> SquareMeters {
```

**Parameters:**

- `a`: `SquareMeters`
- `b`: `SquareMeters`

**Returns:** `SquareMeters`

*Defined at line 465*

---

## m2_sub

```fides
pub fn m2_sub(a: SquareMeters, b: SquareMeters) -> SquareMeters {
```

**Parameters:**

- `a`: `SquareMeters`
- `b`: `SquareMeters`

**Returns:** `SquareMeters`

*Defined at line 468*

---

## m3_add

```fides
pub fn m3_add(a: CubicMeters, b: CubicMeters) -> CubicMeters {
```

**Parameters:**

- `a`: `CubicMeters`
- `b`: `CubicMeters`

**Returns:** `CubicMeters`

*Defined at line 471*

---

## m3_sub

```fides
pub fn m3_sub(a: CubicMeters, b: CubicMeters) -> CubicMeters {
```

**Parameters:**

- `a`: `CubicMeters`
- `b`: `CubicMeters`

**Returns:** `CubicMeters`

*Defined at line 474*

---

## meters_scale

```fides
pub fn meters_scale(a: Meters, k: f64) -> Meters {
```

**Parameters:**

- `a`: `Meters`
- `k`: `f64`

**Returns:** `Meters`

*Defined at line 482*

---

## meters_lt

```fides
pub fn meters_lt(a: Meters, b: Meters) -> bool {
```

**Parameters:**

- `a`: `Meters`
- `b`: `Meters`

**Returns:** `bool`

*Defined at line 485*

---

## meters_ratio

```fides
pub fn meters_ratio(a: Meters, b: Meters) -> f64 {
```

**Parameters:**

- `a`: `Meters`
- `b`: `Meters`

**Returns:** `f64`

*Defined at line 488*

---

## seconds_scale

```fides
pub fn seconds_scale(a: Seconds, k: f64) -> Seconds {
```

**Parameters:**

- `a`: `Seconds`
- `k`: `f64`

**Returns:** `Seconds`

*Defined at line 491*

---

## seconds_lt

```fides
pub fn seconds_lt(a: Seconds, b: Seconds) -> bool {
```

**Parameters:**

- `a`: `Seconds`
- `b`: `Seconds`

**Returns:** `bool`

*Defined at line 494*

---

## seconds_ratio

```fides
pub fn seconds_ratio(a: Seconds, b: Seconds) -> f64 {
```

**Parameters:**

- `a`: `Seconds`
- `b`: `Seconds`

**Returns:** `f64`

*Defined at line 497*

---

## kg_scale

```fides
pub fn kg_scale(a: Kilograms, k: f64) -> Kilograms {
```

**Parameters:**

- `a`: `Kilograms`
- `k`: `f64`

**Returns:** `Kilograms`

*Defined at line 500*

---

## kg_lt

```fides
pub fn kg_lt(a: Kilograms, b: Kilograms) -> bool {
```

**Parameters:**

- `a`: `Kilograms`
- `b`: `Kilograms`

**Returns:** `bool`

*Defined at line 503*

---

## kg_ratio

```fides
pub fn kg_ratio(a: Kilograms, b: Kilograms) -> f64 {
```

**Parameters:**

- `a`: `Kilograms`
- `b`: `Kilograms`

**Returns:** `f64`

*Defined at line 506*

---

## newtons_scale

```fides
pub fn newtons_scale(a: Newtons, k: f64) -> Newtons {
```

**Parameters:**

- `a`: `Newtons`
- `k`: `f64`

**Returns:** `Newtons`

*Defined at line 509*

---

## newtons_lt

```fides
pub fn newtons_lt(a: Newtons, b: Newtons) -> bool {
```

**Parameters:**

- `a`: `Newtons`
- `b`: `Newtons`

**Returns:** `bool`

*Defined at line 512*

---

## newtons_ratio

```fides
pub fn newtons_ratio(a: Newtons, b: Newtons) -> f64 {
```

**Parameters:**

- `a`: `Newtons`
- `b`: `Newtons`

**Returns:** `f64`

*Defined at line 515*

---

## joules_scale

```fides
pub fn joules_scale(a: Joules, k: f64) -> Joules {
```

**Parameters:**

- `a`: `Joules`
- `k`: `f64`

**Returns:** `Joules`

*Defined at line 518*

---

## joules_lt

```fides
pub fn joules_lt(a: Joules, b: Joules) -> bool {
```

**Parameters:**

- `a`: `Joules`
- `b`: `Joules`

**Returns:** `bool`

*Defined at line 521*

---

## joules_ratio

```fides
pub fn joules_ratio(a: Joules, b: Joules) -> f64 {
```

**Parameters:**

- `a`: `Joules`
- `b`: `Joules`

**Returns:** `f64`

*Defined at line 524*

---

## watts_scale

```fides
pub fn watts_scale(a: Watts, k: f64) -> Watts {
```

**Parameters:**

- `a`: `Watts`
- `k`: `f64`

**Returns:** `Watts`

*Defined at line 527*

---

## watts_lt

```fides
pub fn watts_lt(a: Watts, b: Watts) -> bool {
```

**Parameters:**

- `a`: `Watts`
- `b`: `Watts`

**Returns:** `bool`

*Defined at line 530*

---

## watts_ratio

```fides
pub fn watts_ratio(a: Watts, b: Watts) -> f64 {
```

**Parameters:**

- `a`: `Watts`
- `b`: `Watts`

**Returns:** `f64`

*Defined at line 533*

---

## travel

Distance covered at speed `v` for time `t` (d = v t).

```fides
pub fn travel(v: MetersPerSecond, t: Seconds) -> Meters {
```

**Parameters:**

- `v`: `MetersPerSecond`
- `t`: `Seconds`

**Returns:** `Meters`

*Defined at line 540*

---

## travel_time

Time to cover distance `d` at speed `v` (t = d / v).

```fides
pub fn travel_time(d: Meters, v: MetersPerSecond) -> Seconds {
```

**Parameters:**

- `d`: `Meters`
- `v`: `MetersPerSecond`

**Returns:** `Seconds`

*Defined at line 545*

---

## mass_from_force

Mass that force `f` gives acceleration `a` (m = F / a).

```fides
pub fn mass_from_force(f: Newtons, a: MetersPerSecond2) -> Kilograms {
```

**Parameters:**

- `f`: `Newtons`
- `a`: `MetersPerSecond2`

**Returns:** `Kilograms`

*Defined at line 550*

---

## SPEED_OF_LIGHT:

Speed of light in vacuum (m/s).

```fides
pub const SPEED_OF_LIGHT: f64 = 299792458.0;
```

*Defined at line 557*

---

## G_GRAVITATIONAL:

Gravitational constant (m³ kg⁻¹ s⁻²).

```fides
pub const G_GRAVITATIONAL: f64 = 0.00000000006674;
```

*Defined at line 559*

---

## G_EARTH:

Standard gravity on Earth (m/s²).

```fides
pub const G_EARTH: f64 = 9.80665;
```

*Defined at line 561*

---

## AVOGADRO:

Avogadro's number (mol⁻¹).

```fides
pub const AVOGADRO: f64 = 602214076000000000000000.0;
```

*Defined at line 563*

---

## BOLTZMANN:

Boltzmann constant (J/K).

```fides
pub const BOLTZMANN: f64 = 0.00000000000000000000001380649;
```

*Defined at line 565*

---

## PLANCK:

Planck constant (J·s).

```fides
pub const PLANCK: f64 = 0.000000000000000000000000000000000662607015;
```

*Defined at line 567*

---

## ELEMENTARY_CHARGE:

Elementary charge (C).

```fides
pub const ELEMENTARY_CHARGE: f64 = 0.0000000000000000001602176634;
```

*Defined at line 569*

---

## GAS_CONSTANT:

Gas constant (J mol⁻¹ K⁻¹).

```fides
pub const GAS_CONSTANT: f64 = 8.314462618;
```

*Defined at line 571*

---

## ATM_PRESSURE:

Atmospheric pressure at sea level (Pa).

```fides
pub const ATM_PRESSURE: f64 = 101325.0;
```

*Defined at line 573*

---

## ABSOLUTE_ZERO_C:

Absolute zero in Celsius.

```fides
pub const ABSOLUTE_ZERO_C: f64 = -273.15;
```

*Defined at line 575*

---

## Meters

```fides
impl Meters {
```

**Methods:**

### add

```fides
pub fn add(self: Meters, other: Meters) -> Meters {
```

### sub

```fides
pub fn sub(self: Meters, other: Meters) -> Meters {
```

### mul

```fides
pub fn mul(self: Meters, k: f64) -> Meters {
```

### div

```fides
pub fn div(self: Meters, k: f64) -> Meters {
```

*Defined at line 581*

---

## Seconds

```fides
impl Seconds {
```

**Methods:**

### add

```fides
pub fn add(self: Seconds, other: Seconds) -> Seconds {
```

### sub

```fides
pub fn sub(self: Seconds, other: Seconds) -> Seconds {
```

### mul

```fides
pub fn mul(self: Seconds, k: f64) -> Seconds {
```

### div

```fides
pub fn div(self: Seconds, k: f64) -> Seconds {
```

*Defined at line 596*

---

## Kilograms

```fides
impl Kilograms {
```

**Methods:**

### add

```fides
pub fn add(self: Kilograms, other: Kilograms) -> Kilograms {
```

### sub

```fides
pub fn sub(self: Kilograms, other: Kilograms) -> Kilograms {
```

### mul

```fides
pub fn mul(self: Kilograms, k: f64) -> Kilograms {
```

### div

```fides
pub fn div(self: Kilograms, k: f64) -> Kilograms {
```

*Defined at line 611*

---

## Newtons

```fides
impl Newtons {
```

**Methods:**

### add

```fides
pub fn add(self: Newtons, other: Newtons) -> Newtons {
```

### sub

```fides
pub fn sub(self: Newtons, other: Newtons) -> Newtons {
```

### mul

```fides
pub fn mul(self: Newtons, k: f64) -> Newtons {
```

### div

```fides
pub fn div(self: Newtons, k: f64) -> Newtons {
```

*Defined at line 626*

---

## Joules

```fides
impl Joules {
```

**Methods:**

### add

```fides
pub fn add(self: Joules, other: Joules) -> Joules {
```

### sub

```fides
pub fn sub(self: Joules, other: Joules) -> Joules {
```

### mul

```fides
pub fn mul(self: Joules, k: f64) -> Joules {
```

### div

```fides
pub fn div(self: Joules, k: f64) -> Joules {
```

*Defined at line 641*

---

## Watts

```fides
impl Watts {
```

**Methods:**

### add

```fides
pub fn add(self: Watts, other: Watts) -> Watts {
```

### sub

```fides
pub fn sub(self: Watts, other: Watts) -> Watts {
```

### mul

```fides
pub fn mul(self: Watts, k: f64) -> Watts {
```

### div

```fides
pub fn div(self: Watts, k: f64) -> Watts {
```

*Defined at line 656*

---

## Kelvin

```fides
impl Kelvin {
```

**Methods:**

### add

```fides
pub fn add(self: Kelvin, other: Kelvin) -> Kelvin {
```

### sub

```fides
pub fn sub(self: Kelvin, other: Kelvin) -> Kelvin {
```

*Defined at line 671*

---

## Amperes

```fides
impl Amperes {
```

**Methods:**

### add

```fides
pub fn add(self: Amperes, other: Amperes) -> Amperes {
```

### sub

```fides
pub fn sub(self: Amperes, other: Amperes) -> Amperes {
```

*Defined at line 680*

---

## Moles

```fides
impl Moles {
```

**Methods:**

### add

```fides
pub fn add(self: Moles, other: Moles) -> Moles {
```

### sub

```fides
pub fn sub(self: Moles, other: Moles) -> Moles {
```

*Defined at line 689*

---

## Candelas

```fides
impl Candelas {
```

**Methods:**

### add

```fides
pub fn add(self: Candelas, other: Candelas) -> Candelas {
```

### sub

```fides
pub fn sub(self: Candelas, other: Candelas) -> Candelas {
```

*Defined at line 698*

---

## MetersPerSecond

```fides
impl MetersPerSecond {
```

**Methods:**

### add

```fides
pub fn add(self: MetersPerSecond, other: MetersPerSecond) -> MetersPerSecond {
```

### sub

```fides
pub fn sub(self: MetersPerSecond, other: MetersPerSecond) -> MetersPerSecond {
```

*Defined at line 707*

---

## Pascals

```fides
impl Pascals {
```

**Methods:**

### add

```fides
pub fn add(self: Pascals, other: Pascals) -> Pascals {
```

### sub

```fides
pub fn sub(self: Pascals, other: Pascals) -> Pascals {
```

*Defined at line 716*

---

## Hertz

```fides
impl Hertz {
```

**Methods:**

### add

```fides
pub fn add(self: Hertz, other: Hertz) -> Hertz {
```

### sub

```fides
pub fn sub(self: Hertz, other: Hertz) -> Hertz {
```

*Defined at line 725*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
