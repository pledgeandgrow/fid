# units — Fides Documentation

## Contents

- [Meters](#meters) (struct)
- [Kilograms](#kilograms) (struct)
- [Seconds](#seconds) (struct)
- [Kelvin](#kelvin) (struct)
- [Amperes](#amperes) (struct)
- [Moles](#moles) (struct)
- [Candelas](#candelas) (struct)
- [MetersPerSecond](#meterspersecond) (struct)
- [MetersPerSecond2](#meterspersecond2) (struct)
- [Newtons](#newtons) (struct)
- [Joules](#joules) (struct)
- [Watts](#watts) (struct)
- [Pascals](#pascals) (struct)
- [Hertz](#hertz) (struct)
- [SquareMeters](#squaremeters) (struct)
- [CubicMeters](#cubicmeters) (struct)
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
- [SPEED_OF_LIGHT:](#speed_of_light:) (const)
- [G_GRAVITATIONAL:](#g_gravitational:) (const)
- [G_EARTH:](#g_earth:) (const)
- [AVOGADRO:](#avogadro:) (const)
- [BOLTZMANN:](#boltzmann:) (const)
- [PLANCK:](#planck:) (const)
- [ELEMENTARY_CHARGE:](#elementary_charge:) (const)
- [GAS_CONSTANT:](#gas_constant:) (const)
- [ATM_PRESSURE:](#atm_pressure:) (const)
- [ABSOLUTE_ZERO_C:](#absolute_zero_c:) (const)

## Meters

Length in meters.

```fides
pub struct Meters { pub value: f64 }
```

*Defined at line 21*

---

## Kilograms

Mass in kilograms.

```fides
pub struct Kilograms { pub value: f64 }
```

*Defined at line 23*

---

## Seconds

Time in seconds.

```fides
pub struct Seconds { pub value: f64 }
```

*Defined at line 25*

---

## Kelvin

Temperature in Kelvin.

```fides
pub struct Kelvin { pub value: f64 }
```

*Defined at line 27*

---

## Amperes

Electric current in amperes.

```fides
pub struct Amperes { pub value: f64 }
```

*Defined at line 29*

---

## Moles

Amount of substance in moles.

```fides
pub struct Moles { pub value: f64 }
```

*Defined at line 31*

---

## Candelas

Luminous intensity in candelas.

```fides
pub struct Candelas { pub value: f64 }
```

*Defined at line 33*

---

## MetersPerSecond

Velocity in meters per second.

```fides
pub struct MetersPerSecond { pub value: f64 }
```

*Defined at line 38*

---

## MetersPerSecond2

Acceleration in meters per second squared.

```fides
pub struct MetersPerSecond2 { pub value: f64 }
```

*Defined at line 40*

---

## Newtons

Force in newtons (kg·m/s²).

```fides
pub struct Newtons { pub value: f64 }
```

*Defined at line 42*

---

## Joules

Energy in joules (N·m).

```fides
pub struct Joules { pub value: f64 }
```

*Defined at line 44*

---

## Watts

Power in watts (J/s).

```fides
pub struct Watts { pub value: f64 }
```

*Defined at line 46*

---

## Pascals

Pressure in pascals (N/m²).

```fides
pub struct Pascals { pub value: f64 }
```

*Defined at line 48*

---

## Hertz

Frequency in hertz (1/s).

```fides
pub struct Hertz { pub value: f64 }
```

*Defined at line 50*

---

## SquareMeters

Area in square meters.

```fides
pub struct SquareMeters { pub value: f64 }
```

*Defined at line 52*

---

## CubicMeters

Volume in cubic meters.

```fides
pub struct CubicMeters { pub value: f64 }
```

*Defined at line 54*

---

## meters

```fides
pub fn meters(v: f64) -> Meters { return Meters { value: v }; }
```

**Parameters:**

- `v`: `f64`

**Returns:** `Meters`

*Defined at line 58*

---

## kilograms

```fides
pub fn kilograms(v: f64) -> Kilograms { return Kilograms { value: v }; }
```

**Parameters:**

- `v`: `f64`

**Returns:** `Kilograms`

*Defined at line 59*

---

## seconds

```fides
pub fn seconds(v: f64) -> Seconds { return Seconds { value: v }; }
```

**Parameters:**

- `v`: `f64`

**Returns:** `Seconds`

*Defined at line 60*

---

## kelvin

```fides
pub fn kelvin(v: f64) -> Kelvin { return Kelvin { value: v }; }
```

**Parameters:**

- `v`: `f64`

**Returns:** `Kelvin`

*Defined at line 61*

---

## amperes

```fides
pub fn amperes(v: f64) -> Amperes { return Amperes { value: v }; }
```

**Parameters:**

- `v`: `f64`

**Returns:** `Amperes`

*Defined at line 62*

---

## moles

```fides
pub fn moles(v: f64) -> Moles { return Moles { value: v }; }
```

**Parameters:**

- `v`: `f64`

**Returns:** `Moles`

*Defined at line 63*

---

## candelas

```fides
pub fn candelas(v: f64) -> Candelas { return Candelas { value: v }; }
```

**Parameters:**

- `v`: `f64`

**Returns:** `Candelas`

*Defined at line 64*

---

## meters_per_second

```fides
pub fn meters_per_second(v: f64) -> MetersPerSecond { return MetersPerSecond { value: v }; }
```

**Parameters:**

- `v`: `f64`

**Returns:** `MetersPerSecond`

*Defined at line 66*

---

## meters_per_second2

```fides
pub fn meters_per_second2(v: f64) -> MetersPerSecond2 { return MetersPerSecond2 { value: v }; }
```

**Parameters:**

- `v`: `f64`

**Returns:** `MetersPerSecond2`

*Defined at line 67*

---

## newtons

```fides
pub fn newtons(v: f64) -> Newtons { return Newtons { value: v }; }
```

**Parameters:**

- `v`: `f64`

**Returns:** `Newtons`

*Defined at line 68*

---

## joules

```fides
pub fn joules(v: f64) -> Joules { return Joules { value: v }; }
```

**Parameters:**

- `v`: `f64`

**Returns:** `Joules`

*Defined at line 69*

---

## watts

```fides
pub fn watts(v: f64) -> Watts { return Watts { value: v }; }
```

**Parameters:**

- `v`: `f64`

**Returns:** `Watts`

*Defined at line 70*

---

## pascals

```fides
pub fn pascals(v: f64) -> Pascals { return Pascals { value: v }; }
```

**Parameters:**

- `v`: `f64`

**Returns:** `Pascals`

*Defined at line 71*

---

## hertz

```fides
pub fn hertz(v: f64) -> Hertz { return Hertz { value: v }; }
```

**Parameters:**

- `v`: `f64`

**Returns:** `Hertz`

*Defined at line 72*

---

## square_meters

```fides
pub fn square_meters(v: f64) -> SquareMeters { return SquareMeters { value: v }; }
```

**Parameters:**

- `v`: `f64`

**Returns:** `SquareMeters`

*Defined at line 73*

---

## cubic_meters

```fides
pub fn cubic_meters(v: f64) -> CubicMeters { return CubicMeters { value: v }; }
```

**Parameters:**

- `v`: `f64`

**Returns:** `CubicMeters`

*Defined at line 74*

---

## km_to_m

Convert kilometers to meters.

```fides
pub fn km_to_m(km: f64) -> Meters { return meters(km * 1000.0); }
```

**Parameters:**

- `km`: `f64`

**Returns:** `Meters`

*Defined at line 79*

---

## m_to_km

Convert meters to kilometers.

```fides
pub fn m_to_km(m: Meters) -> f64 { return m.value / 1000.0; }
```

**Parameters:**

- `m`: `Meters`

**Returns:** `f64`

*Defined at line 82*

---

## cm_to_m

Convert centimeters to meters.

```fides
pub fn cm_to_m(cm: f64) -> Meters { return meters(cm / 100.0); }
```

**Parameters:**

- `cm`: `f64`

**Returns:** `Meters`

*Defined at line 85*

---

## m_to_cm

Convert meters to centimeters.

```fides
pub fn m_to_cm(m: Meters) -> f64 { return m.value * 100.0; }
```

**Parameters:**

- `m`: `Meters`

**Returns:** `f64`

*Defined at line 88*

---

## mm_to_m

Convert millimeters to meters.

```fides
pub fn mm_to_m(mm: f64) -> Meters { return meters(mm / 1000.0); }
```

**Parameters:**

- `mm`: `f64`

**Returns:** `Meters`

*Defined at line 91*

---

## m_to_mm

Convert meters to millimeters.

```fides
pub fn m_to_mm(m: Meters) -> f64 { return m.value * 1000.0; }
```

**Parameters:**

- `m`: `Meters`

**Returns:** `f64`

*Defined at line 94*

---

## miles_to_m

Convert miles to meters.

```fides
pub fn miles_to_m(miles: f64) -> Meters { return meters(miles * 1609.344); }
```

**Parameters:**

- `miles`: `f64`

**Returns:** `Meters`

*Defined at line 97*

---

## m_to_miles

Convert meters to miles.

```fides
pub fn m_to_miles(m: Meters) -> f64 { return m.value / 1609.344; }
```

**Parameters:**

- `m`: `Meters`

**Returns:** `f64`

*Defined at line 100*

---

## feet_to_m

Convert feet to meters.

```fides
pub fn feet_to_m(ft: f64) -> Meters { return meters(ft * 0.3048); }
```

**Parameters:**

- `ft`: `f64`

**Returns:** `Meters`

*Defined at line 103*

---

## m_to_feet

Convert meters to feet.

```fides
pub fn m_to_feet(m: Meters) -> f64 { return m.value / 0.3048; }
```

**Parameters:**

- `m`: `Meters`

**Returns:** `f64`

*Defined at line 106*

---

## inches_to_m

Convert inches to meters.

```fides
pub fn inches_to_m(inch: f64) -> Meters { return meters(inch * 0.0254); }
```

**Parameters:**

- `inch`: `f64`

**Returns:** `Meters`

*Defined at line 109*

---

## m_to_inches

Convert meters to inches.

```fides
pub fn m_to_inches(m: Meters) -> f64 { return m.value / 0.0254; }
```

**Parameters:**

- `m`: `Meters`

**Returns:** `f64`

*Defined at line 112*

---

## g_to_kg

Convert grams to kilograms.

```fides
pub fn g_to_kg(g: f64) -> Kilograms { return kilograms(g / 1000.0); }
```

**Parameters:**

- `g`: `f64`

**Returns:** `Kilograms`

*Defined at line 115*

---

## kg_to_g

Convert kilograms to grams.

```fides
pub fn kg_to_g(kg: Kilograms) -> f64 { return kg.value * 1000.0; }
```

**Parameters:**

- `kg`: `Kilograms`

**Returns:** `f64`

*Defined at line 118*

---

## lb_to_kg

Convert pounds to kilograms.

```fides
pub fn lb_to_kg(lb: f64) -> Kilograms { return kilograms(lb * 0.453592); }
```

**Parameters:**

- `lb`: `f64`

**Returns:** `Kilograms`

*Defined at line 121*

---

## kg_to_lb

Convert kilograms to pounds.

```fides
pub fn kg_to_lb(kg: Kilograms) -> f64 { return kg.value / 0.453592; }
```

**Parameters:**

- `kg`: `Kilograms`

**Returns:** `f64`

*Defined at line 124*

---

## celsius_to_kelvin

Convert Celsius to Kelvin.

```fides
pub fn celsius_to_kelvin(c: f64) -> Kelvin { return kelvin(c + 273.15); }
```

**Parameters:**

- `c`: `f64`

**Returns:** `Kelvin`

*Defined at line 127*

---

## kelvin_to_celsius

Convert Kelvin to Celsius.

```fides
pub fn kelvin_to_celsius(k: Kelvin) -> f64 { return k.value - 273.15; }
```

**Parameters:**

- `k`: `Kelvin`

**Returns:** `f64`

*Defined at line 130*

---

## fahrenheit_to_kelvin

Convert Fahrenheit to Kelvin.

```fides
pub fn fahrenheit_to_kelvin(f: f64) -> Kelvin { return kelvin((f - 32.0) * 5.0 / 9.0 + 273.15); }
```

**Parameters:**

- `f`: `f64`

**Returns:** `Kelvin`

*Defined at line 133*

---

## kelvin_to_fahrenheit

Convert Kelvin to Fahrenheit.

```fides
pub fn kelvin_to_fahrenheit(k: Kelvin) -> f64 { return (k.value - 273.15) * 9.0 / 5.0 + 32.0; }
```

**Parameters:**

- `k`: `Kelvin`

**Returns:** `f64`

*Defined at line 136*

---

## minutes_to_s

Convert minutes to seconds.

```fides
pub fn minutes_to_s(min: f64) -> Seconds { return seconds(min * 60.0); }
```

**Parameters:**

- `min`: `f64`

**Returns:** `Seconds`

*Defined at line 139*

---

## hours_to_s

Convert hours to seconds.

```fides
pub fn hours_to_s(h: f64) -> Seconds { return seconds(h * 3600.0); }
```

**Parameters:**

- `h`: `f64`

**Returns:** `Seconds`

*Defined at line 142*

---

## days_to_s

Convert days to seconds.

```fides
pub fn days_to_s(d: f64) -> Seconds { return seconds(d * 86400.0); }
```

**Parameters:**

- `d`: `f64`

**Returns:** `Seconds`

*Defined at line 145*

---

## s_to_minutes

Convert seconds to minutes.

```fides
pub fn s_to_minutes(s: Seconds) -> f64 { return s.value / 60.0; }
```

**Parameters:**

- `s`: `Seconds`

**Returns:** `f64`

*Defined at line 148*

---

## s_to_hours

Convert seconds to hours.

```fides
pub fn s_to_hours(s: Seconds) -> f64 { return s.value / 3600.0; }
```

**Parameters:**

- `s`: `Seconds`

**Returns:** `f64`

*Defined at line 151*

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

*Defined at line 156*

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

*Defined at line 161*

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

*Defined at line 166*

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

*Defined at line 171*

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

*Defined at line 176*

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

*Defined at line 181*

---

## frequency

Frequency = 1 / period.

```fides
pub fn frequency(period: Seconds) -> Hertz {
```

**Parameters:**

- `period`: `Seconds`

**Returns:** `Hertz`

*Defined at line 186*

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

*Defined at line 191*

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

*Defined at line 196*

---

## meters_add

```fides
pub fn meters_add(a: Meters, b: Meters) -> Meters { return meters(a.value + b.value); }
```

**Parameters:**

- `a`: `Meters`
- `b`: `Meters`

**Returns:** `Meters`

*Defined at line 202*

---

## meters_sub

```fides
pub fn meters_sub(a: Meters, b: Meters) -> Meters { return meters(a.value - b.value); }
```

**Parameters:**

- `a`: `Meters`
- `b`: `Meters`

**Returns:** `Meters`

*Defined at line 203*

---

## seconds_add

```fides
pub fn seconds_add(a: Seconds, b: Seconds) -> Seconds { return seconds(a.value + b.value); }
```

**Parameters:**

- `a`: `Seconds`
- `b`: `Seconds`

**Returns:** `Seconds`

*Defined at line 204*

---

## seconds_sub

```fides
pub fn seconds_sub(a: Seconds, b: Seconds) -> Seconds { return seconds(a.value - b.value); }
```

**Parameters:**

- `a`: `Seconds`
- `b`: `Seconds`

**Returns:** `Seconds`

*Defined at line 205*

---

## kg_add

```fides
pub fn kg_add(a: Kilograms, b: Kilograms) -> Kilograms { return kilograms(a.value + b.value); }
```

**Parameters:**

- `a`: `Kilograms`
- `b`: `Kilograms`

**Returns:** `Kilograms`

*Defined at line 206*

---

## kg_sub

```fides
pub fn kg_sub(a: Kilograms, b: Kilograms) -> Kilograms { return kilograms(a.value - b.value); }
```

**Parameters:**

- `a`: `Kilograms`
- `b`: `Kilograms`

**Returns:** `Kilograms`

*Defined at line 207*

---

## newtons_add

```fides
pub fn newtons_add(a: Newtons, b: Newtons) -> Newtons { return newtons(a.value + b.value); }
```

**Parameters:**

- `a`: `Newtons`
- `b`: `Newtons`

**Returns:** `Newtons`

*Defined at line 208*

---

## newtons_sub

```fides
pub fn newtons_sub(a: Newtons, b: Newtons) -> Newtons { return newtons(a.value - b.value); }
```

**Parameters:**

- `a`: `Newtons`
- `b`: `Newtons`

**Returns:** `Newtons`

*Defined at line 209*

---

## joules_add

```fides
pub fn joules_add(a: Joules, b: Joules) -> Joules { return joules(a.value + b.value); }
```

**Parameters:**

- `a`: `Joules`
- `b`: `Joules`

**Returns:** `Joules`

*Defined at line 210*

---

## joules_sub

```fides
pub fn joules_sub(a: Joules, b: Joules) -> Joules { return joules(a.value - b.value); }
```

**Parameters:**

- `a`: `Joules`
- `b`: `Joules`

**Returns:** `Joules`

*Defined at line 211*

---

## watts_add

```fides
pub fn watts_add(a: Watts, b: Watts) -> Watts { return watts(a.value + b.value); }
```

**Parameters:**

- `a`: `Watts`
- `b`: `Watts`

**Returns:** `Watts`

*Defined at line 212*

---

## watts_sub

```fides
pub fn watts_sub(a: Watts, b: Watts) -> Watts { return watts(a.value - b.value); }
```

**Parameters:**

- `a`: `Watts`
- `b`: `Watts`

**Returns:** `Watts`

*Defined at line 213*

---

## SPEED_OF_LIGHT:

Speed of light in vacuum (m/s).

```fides
pub const SPEED_OF_LIGHT: f64 = 299792458.0;
```

*Defined at line 218*

---

## G_GRAVITATIONAL:

Gravitational constant (m³ kg⁻¹ s⁻²).

```fides
pub const G_GRAVITATIONAL: f64 = 0.00000000006674;
```

*Defined at line 221*

---

## G_EARTH:

Standard gravity on Earth (m/s²).

```fides
pub const G_EARTH: f64 = 9.80665;
```

*Defined at line 224*

---

## AVOGADRO:

Avogadro's number (mol⁻¹).

```fides
pub const AVOGADRO: f64 = 602214076000000000000000.0;
```

*Defined at line 227*

---

## BOLTZMANN:

Boltzmann constant (J/K).

```fides
pub const BOLTZMANN: f64 = 0.00000000000000000000001380649;
```

*Defined at line 230*

---

## PLANCK:

Planck constant (J·s).

```fides
pub const PLANCK: f64 = 0.000000000000000000000000000000000662607015;
```

*Defined at line 233*

---

## ELEMENTARY_CHARGE:

Elementary charge (C).

```fides
pub const ELEMENTARY_CHARGE: f64 = 0.0000000000000000001602176634;
```

*Defined at line 236*

---

## GAS_CONSTANT:

Gas constant (J mol⁻¹ K⁻¹).

```fides
pub const GAS_CONSTANT: f64 = 8.314462618;
```

*Defined at line 239*

---

## ATM_PRESSURE:

Atmospheric pressure at sea level (Pa).

```fides
pub const ATM_PRESSURE: f64 = 101325.0;
```

*Defined at line 242*

---

## ABSOLUTE_ZERO_C:

Absolute zero in Celsius.

```fides
pub const ABSOLUTE_ZERO_C: f64 = -273.15;
```

*Defined at line 245*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
