[All modules](index.md)

# geolocation -- Fides Documentation

## Contents

- [GEO_EARTH_RADIUS_M:](#GEO_EARTH_RADIUS_M:) (const)
- [GEO_WGS84_A:](#GEO_WGS84_A:) (const)
- [GEO_WGS84_F:](#GEO_WGS84_F:) (const)
- [GeoPoint](#GeoPoint) (struct)
- [geo_point](#geo_point) (function)
- [geo_pi](#geo_pi) (function)
- [geo_rad](#geo_rad) (function)
- [geo_deg](#geo_deg) (function)
- [geo_atan](#geo_atan) (function)
- [geo_atan2](#geo_atan2) (function)
- [geo_asin](#geo_asin) (function)
- [geo_norm360](#geo_norm360) (function)
- [geo_norm180](#geo_norm180) (function)
- [geo_haversine](#geo_haversine) (function)
- [geo_bearing](#geo_bearing) (function)
- [geo_final_bearing](#geo_final_bearing) (function)
- [geo_destination](#geo_destination) (function)
- [geo_midpoint](#geo_midpoint) (function)
- [GeoInverse](#GeoInverse) (struct)
- [geo_vincenty](#geo_vincenty) (function)
- [geo_dms](#geo_dms) (function)
- [geohash_encode](#geohash_encode) (function)
- [GeohashBox](#GeohashBox) (struct)
- [geohash_decode](#geohash_decode) (function)
- [geohash_neighbor](#geohash_neighbor) (function)
- [GEOFENCE_CIRCLE:](#GEOFENCE_CIRCLE:) (const)
- [GEOFENCE_POLYGON:](#GEOFENCE_POLYGON:) (const)
- [GEOFENCE_ENTER:](#GEOFENCE_ENTER:) (const)
- [GEOFENCE_EXIT:](#GEOFENCE_EXIT:) (const)
- [Geofence](#Geofence) (struct)
- [geofence_circle](#geofence_circle) (function)
- [geofence_polygon](#geofence_polygon) (function)
- [geo_polygon_contains](#geo_polygon_contains) (function)
- [geofence_depth](#geofence_depth) (function)
- [geofence_update](#geofence_update) (function)
- [GeoFix](#GeoFix) (struct)
- [GeoFilter](#GeoFilter) (struct)
- [geo_filter_new](#geo_filter_new) (function)
- [geo_filter_update](#geo_filter_update) (function)
- [geo_filter_accuracy](#geo_filter_accuracy) (function)

## GEO_EARTH_RADIUS_M:

```fides
pub const GEO_EARTH_RADIUS_M: f64 = 6371008.8;  // IUGG mean radius
```

*Defined at line 30*

---

## GEO_WGS84_A:

```fides
pub const GEO_WGS84_A: f64 = 6378137.0;
```

*Defined at line 31*

---

## GEO_WGS84_F:

```fides
pub const GEO_WGS84_F: f64 = 0.0033528106647474805;  // 1 / 298.257223563
```

*Defined at line 32*

---

## GeoPoint

```fides
pub struct GeoPoint {
```

**Fields:**

| Name | Type |
|------|------|
| `lat` | `f64` |
| `lon` | `f64` |

*Defined at line 34*

---

## geo_point

```fides
pub fn geo_point(lat: f64, lon: f64) -> GeoPoint {
```

**Parameters:**

- `lat`: `f64`
- `lon`: `f64`

**Returns:** `GeoPoint`

*Defined at line 39*

---

## geo_pi

```fides
pub fn geo_pi() -> f64 {
```

**Returns:** `f64`

*Defined at line 47*

---

## geo_rad

```fides
pub fn geo_rad(d: f64) -> f64 {
```

**Parameters:**

- `d`: `f64`

**Returns:** `f64`

*Defined at line 50*

---

## geo_deg

```fides
pub fn geo_deg(r: f64) -> f64 {
```

**Parameters:**

- `r`: `f64`

**Returns:** `f64`

*Defined at line 53*

---

## geo_atan

atan(x), accurate to about 1e-15.

```fides
pub fn geo_atan(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 58*

---

## geo_atan2

atan2(y, x) in (-pi, pi]; 0 for (0, 0).

```fides
pub fn geo_atan2(y: f64, x: f64) -> f64 {
```

**Parameters:**

- `y`: `f64`
- `x`: `f64`

**Returns:** `f64`

*Defined at line 86*

---

## geo_asin

asin(x), x clamped to [-1, 1].

```fides
pub fn geo_asin(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 106*

---

## geo_norm360

Normalize degrees to [0, 360).

```fides
pub fn geo_norm360(d: f64) -> f64 {
```

**Parameters:**

- `d`: `f64`

**Returns:** `f64`

*Defined at line 118*

---

## geo_norm180

Normalize a longitude to [-180, 180).

```fides
pub fn geo_norm180(d: f64) -> f64 {
```

**Parameters:**

- `d`: `f64`

**Returns:** `f64`

*Defined at line 127*

---

## geo_haversine

Great-circle distance in meters (haversine).

```fides
pub fn geo_haversine(a: GeoPoint, b: GeoPoint) -> f64 {
```

**Parameters:**

- `a`: `GeoPoint`
- `b`: `GeoPoint`

**Returns:** `f64`

*Defined at line 134*

---

## geo_bearing

Initial great-circle bearing from a to b, degrees in [0, 360).

```fides
pub fn geo_bearing(a: GeoPoint, b: GeoPoint) -> f64 {
```

**Parameters:**

- `a`: `GeoPoint`
- `b`: `GeoPoint`

**Returns:** `f64`

*Defined at line 146*

---

## geo_final_bearing

Final bearing on arrival at b, degrees in [0, 360).

```fides
pub fn geo_final_bearing(a: GeoPoint, b: GeoPoint) -> f64 {
```

**Parameters:**

- `a`: `GeoPoint`
- `b`: `GeoPoint`

**Returns:** `f64`

*Defined at line 156*

---

## geo_destination

The point `dist` meters from `a` along initial bearing `brg` degrees.

```fides
pub fn geo_destination(a: GeoPoint, brg: f64, dist: f64) -> GeoPoint {
```

**Parameters:**

- `a`: `GeoPoint`
- `brg`: `f64`
- `dist`: `f64`

**Returns:** `GeoPoint`

*Defined at line 161*

---

## geo_midpoint

The great-circle midpoint of a and b.

```fides
pub fn geo_midpoint(a: GeoPoint, b: GeoPoint) -> GeoPoint {
```

**Parameters:**

- `a`: `GeoPoint`
- `b`: `GeoPoint`

**Returns:** `GeoPoint`

*Defined at line 175*

---

## GeoInverse

```fides
pub struct GeoInverse {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool,  // false: did not converge (nearly antipodal)` |
| `distance` | `f64,  // meters` |
| `azimuth1` | `f64,  // initial bearing at a, degrees` |
| `azimuth2` | `f64,  // forward bearing at b, degrees` |
| `iterations` | `i64` |

*Defined at line 191*

---

## geo_vincenty

Vincenty's inverse problem on the WGS-84 ellipsoid.

```fides
pub fn geo_vincenty(a: GeoPoint, b: GeoPoint) -> GeoInverse {
```

**Parameters:**

- `a`: `GeoPoint`
- `b`: `GeoPoint`

**Returns:** `GeoInverse`

*Defined at line 200*

---

## geo_dms

Degrees from degrees, minutes, seconds (negative degrees: south/west).

```fides
pub fn geo_dms(d: i64, m: i64, s: f64) -> f64 {
```

**Parameters:**

- `d`: `i64`
- `m`: `i64`
- `s`: `f64`

**Returns:** `f64`

*Defined at line 268*

---

## geohash_encode

Geohash of a point with `precision` characters (1..12).

```fides
pub fn geohash_encode(lat: f64, lon: f64, precision: i64) -> str {
```

**Parameters:**

- `lat`: `f64`
- `lon`: `f64`
- `precision`: `i64`

**Returns:** `str`

*Defined at line 287*

---

## GeohashBox

A decoded geohash: its cell and the cell's center.

```fides
pub struct GeohashBox {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `lat_lo` | `f64` |
| `lat_hi` | `f64` |
| `lon_lo` | `f64` |
| `lon_hi` | `f64` |
| `lat` | `f64` |
| `lon` | `f64` |

*Defined at line 329*

---

## geohash_decode

```fides
pub fn geohash_decode(h: str) -> GeohashBox {
```

**Parameters:**

- `h`: `str`

**Returns:** `GeohashBox`

*Defined at line 339*

---

## geohash_neighbor

The neighbouring cell of the same precision: dlat, dlon in -1..1
(north is dlat 1). Longitude wraps; "" past a pole.

```fides
pub fn geohash_neighbor(h: str, dlat: i64, dlon: i64) -> str {
```

**Parameters:**

- `h`: `str`
- `dlat`: `i64`
- `dlon`: `i64`

**Returns:** `str`

*Defined at line 385*

---

## GEOFENCE_CIRCLE:

```fides
pub const GEOFENCE_CIRCLE: i64 = 0;
```

*Defined at line 400*

---

## GEOFENCE_POLYGON:

```fides
pub const GEOFENCE_POLYGON: i64 = 1;
```

*Defined at line 401*

---

## GEOFENCE_ENTER:

```fides
pub const GEOFENCE_ENTER: i64 = 1;
```

*Defined at line 402*

---

## GEOFENCE_EXIT:

```fides
pub const GEOFENCE_EXIT: i64 = 2;
```

*Defined at line 403*

---

## Geofence

```fides
pub struct Geofence {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `str` |
| `kind` | `i64` |
| `center` | `GeoPoint` |
| `radius` | `f64` |
| `lats` | `Vec<f64>,  // polygon vertices` |
| `lons` | `Vec<f64>` |
| `hysteresis` | `f64,  // meters on each side of the boundary` |
| `inside` | `bool` |
| `known` | `bool,  // has a fix decided the state yet` |

*Defined at line 405*

---

## geofence_circle

```fides
pub fn geofence_circle(id: str, center: GeoPoint, radius: f64, hysteresis: f64) -> Geofence {
```

**Parameters:**

- `id`: `str`
- `center`: `GeoPoint`
- `radius`: `f64`
- `hysteresis`: `f64`

**Returns:** `Geofence`

*Defined at line 417*

---

## geofence_polygon

```fides
pub fn geofence_polygon(id: str, lats: Vec<f64>, lons: Vec<f64>, hysteresis: f64) -> Geofence {
```

**Parameters:**

- `id`: `str`
- `lats`: `Vec<f64>`
- `lons`: `Vec<f64>`
- `hysteresis`: `f64`

**Returns:** `Geofence`

*Defined at line 423*

---

## geo_polygon_contains

Is p inside the polygon (ray casting in latitude / longitude)?

```fides
pub fn geo_polygon_contains(lats: &Vec<f64>, lons: &Vec<f64>, p: GeoPoint) -> bool {
```

**Parameters:**

- `lats`: `&Vec<f64>`
- `lons`: `&Vec<f64>`
- `p`: `GeoPoint`

**Returns:** `bool`

*Defined at line 432*

---

## geofence_depth

How far inside the fence p is, in meters (negative: outside).

```fides
pub fn geofence_depth(g: &Geofence, p: GeoPoint) -> f64 {
```

**Parameters:**

- `g`: `&Geofence`
- `p`: `GeoPoint`

**Returns:** `f64`

*Defined at line 492*

---

## geofence_update

Feed a fix: GEOFENCE_ENTER, GEOFENCE_EXIT or 0. A fence changes state
only once the point is more than `hysteresis` meters past the
boundary; the first fix decides the initial state (an ENTER when it is
inside, nothing when outside).

```fides
pub fn geofence_update(g: &mut Geofence, p: GeoPoint) -> i64 {
```

**Parameters:**

- `g`: `&mut Geofence`
- `p`: `GeoPoint`

**Returns:** `i64`

*Defined at line 507*

---

## GeoFix

A fix as the platform reports it.

```fides
pub struct GeoFix {
```

**Fields:**

| Name | Type |
|------|------|
| `lat` | `f64` |
| `lon` | `f64` |
| `accuracy` | `f64,  // meters (1 sigma)` |
| `time_ms` | `i64` |

*Defined at line 534*

---

## GeoFilter

Kalman filter on latitude / longitude with a variance in m^2.

```fides
pub struct GeoFilter {
```

**Fields:**

| Name | Type |
|------|------|
| `lat` | `f64` |
| `lon` | `f64` |
| `variance` | `f64,  // < 0: no fix yet` |
| `time_ms` | `i64` |
| `q` | `f64,  // process noise, meters per second` |
| `min_accuracy` | `f64` |

*Defined at line 542*

---

## geo_filter_new

```fides
pub fn geo_filter_new(q_meters_per_sec: f64) -> GeoFilter {
```

**Parameters:**

- `q_meters_per_sec`: `f64`

**Returns:** `GeoFilter`

*Defined at line 551*

---

## geo_filter_update

Fold a fix in; returns the filtered position.

```fides
pub fn geo_filter_update(f: &mut GeoFilter, fix: GeoFix) -> GeoPoint {
```

**Parameters:**

- `f`: `&mut GeoFilter`
- `fix`: `GeoFix`

**Returns:** `GeoPoint`

*Defined at line 558*

---

## geo_filter_accuracy

The filter's current 1-sigma accuracy in meters.

```fides
pub fn geo_filter_accuracy(f: &GeoFilter) -> f64 {
```

**Parameters:**

- `f`: `&GeoFilter`

**Returns:** `f64`

*Defined at line 587*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
