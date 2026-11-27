[All modules](index.md)

# lidar -- Fides Documentation

## Contents

- [LaserScan](#LaserScan) (struct)
- [lidar_scan_new](#lidar_scan_new) (function)
- [lidar_len](#lidar_len) (function)
- [lidar_angle](#lidar_angle) (function)
- [lidar_is_valid](#lidar_is_valid) (function)
- [lidar_valid_count](#lidar_valid_count) (function)
- [lidar_to_points](#lidar_to_points) (function)
- [lidar_to_points_pose](#lidar_to_points_pose) (function)
- [lidar_to_points3](#lidar_to_points3) (function)
- [lidar_filter_range](#lidar_filter_range) (function)
- [lidar_median_filter](#lidar_median_filter) (function)
- [LineSeg](#LineSeg) (struct)
- [lidar_fit_line](#lidar_fit_line) (function)
- [lidar_extract_lines_pts](#lidar_extract_lines_pts) (function)
- [lidar_extract_lines](#lidar_extract_lines) (function)
- [lidar_seg_length](#lidar_seg_length) (function)
- [ScanMatch](#ScanMatch) (struct)
- [lidar_match](#lidar_match) (function)

## LaserScan

A planar laser scan.

```fides
pub struct LaserScan {
```

**Fields:**

| Name | Type |
|------|------|
| `angle_min` | `f64` |
| `angle_inc` | `f64` |
| `range_min` | `f64` |
| `range_max` | `f64` |
| `ranges` | `Vec<f64>` |

*Defined at line 44*

---

## lidar_scan_new

A scan from its fields.

```fides
pub fn lidar_scan_new(angle_min: f64, angle_inc: f64, range_min: f64, range_max: f64, ranges: Vec<f64>) -> LaserScan {
```

**Parameters:**

- `angle_min`: `f64`
- `angle_inc`: `f64`
- `range_min`: `f64`
- `range_max`: `f64`
- `ranges`: `Vec<f64>`

**Returns:** `LaserScan`

*Defined at line 53*

---

## lidar_len

The number of beams.

```fides
pub fn lidar_len(s: &LaserScan) -> i64 {
```

**Parameters:**

- `s`: `&LaserScan`

**Returns:** `i64`

*Defined at line 60*

---

## lidar_angle

The angle of beam i.

```fides
pub fn lidar_angle(s: &LaserScan, i: i64) -> f64 {
```

**Parameters:**

- `s`: `&LaserScan`
- `i`: `i64`

**Returns:** `f64`

*Defined at line 65*

---

## lidar_is_valid

True when reading i is a number in [range_min, range_max] and > 0.

```fides
pub fn lidar_is_valid(s: &LaserScan, i: i64) -> bool {
```

**Parameters:**

- `s`: `&LaserScan`
- `i`: `i64`

**Returns:** `bool`

*Defined at line 70*

---

## lidar_valid_count

How many readings are valid.

```fides
pub fn lidar_valid_count(s: &LaserScan) -> i64 {
```

**Parameters:**

- `s`: `&LaserScan`

**Returns:** `i64`

*Defined at line 76*

---

## lidar_to_points

The valid readings as points in the sensor frame, in beam order.

```fides
pub fn lidar_to_points(s: &LaserScan) -> Vec<vec2> {
```

**Parameters:**

- `s`: `&LaserScan`

**Returns:** `Vec<vec2>`

*Defined at line 89*

---

## lidar_to_points_pose

The valid readings as points in the frame where the sensor is at
(x, y) facing theta.

```fides
pub fn lidar_to_points_pose(s: &LaserScan, x: f64, y: f64, theta: f64) -> Vec<vec2> {
```

**Parameters:**

- `s`: `&LaserScan`
- `x`: `f64`
- `y`: `f64`
- `theta`: `f64`

**Returns:** `Vec<vec2>`

*Defined at line 95*

---

## lidar_to_points3

The valid readings as 3D points (z = 0) for pointcloud.

```fides
pub fn lidar_to_points3(s: &LaserScan) -> Vec<vec3> {
```

**Parameters:**

- `s`: `&LaserScan`

**Returns:** `Vec<vec3>`

*Defined at line 110*

---

## lidar_filter_range

A copy of s whose readings outside [lo, hi] (and invalid ones) are 0.

```fides
pub fn lidar_filter_range(s: &LaserScan, lo: f64, hi: f64) -> LaserScan {
```

**Parameters:**

- `s`: `&LaserScan`
- `lo`: `f64`
- `hi`: `f64`

**Returns:** `LaserScan`

*Defined at line 122*

---

## lidar_median_filter

A copy of s where each valid reading is replaced by the median of the
valid readings among beams i - half .. i + half (invalid ones stay).

```fides
pub fn lidar_median_filter(s: &LaserScan, half: i64) -> LaserScan {
```

**Parameters:**

- `s`: `&LaserScan`
- `half`: `i64`

**Returns:** `LaserScan`

*Defined at line 139*

---

## LineSeg

A fitted line segment: x cos(alpha) + y sin(alpha) = r (r >= 0), its
end points projected on the line, and the points it was fitted to.

```fides
pub struct LineSeg {
```

**Fields:**

| Name | Type |
|------|------|
| `r` | `f64` |
| `alpha` | `f64` |
| `start` | `vec2` |
| `end` | `vec2` |
| `first` | `i64` |
| `last` | `i64` |
| `count` | `i64` |

*Defined at line 177*

---

## lidar_fit_line

The total-least-squares line through pts[first..=last].

```fides
pub fn lidar_fit_line(pts: &Vec<vec2>, first: i64, last: i64) -> LineSeg {
```

**Parameters:**

- `pts`: `&Vec<vec2>`
- `first`: `i64`
- `last`: `i64`

**Returns:** `LineSeg`

*Defined at line 198*

---

## lidar_extract_lines_pts

Line segments of an ordered point list: runs are cut where consecutive
points are more than `gap` apart, split while a point is more than
`thresh` from the chord, merged back while one fitted line stays within
`thresh` of both neighbours' points; segments with fewer than
`min_points` points are dropped.

```fides
pub fn lidar_extract_lines_pts(pts: &Vec<vec2>, gap: f64, thresh: f64, min_points: i64) -> Vec<LineSeg> {
```

**Parameters:**

- `pts`: `&Vec<vec2>`
- `gap`: `f64`
- `thresh`: `f64`
- `min_points`: `i64`

**Returns:** `Vec<LineSeg>`

*Defined at line 289*

---

## lidar_extract_lines

Line segments of a scan (sensor frame); see lidar_extract_lines_pts.

```fides
pub fn lidar_extract_lines(s: &LaserScan, gap: f64, thresh: f64, min_points: i64) -> Vec<LineSeg> {
```

**Parameters:**

- `s`: `&LaserScan`
- `gap`: `f64`
- `thresh`: `f64`
- `min_points`: `i64`

**Returns:** `Vec<LineSeg>`

*Defined at line 346*

---

## lidar_seg_length

The length of a segment.

```fides
pub fn lidar_seg_length(seg: &LineSeg) -> f64 {
```

**Parameters:**

- `seg`: `&LineSeg`

**Returns:** `f64`

*Defined at line 352*

---

## ScanMatch

The pose of a scan's sensor in a reference scan's frame.

```fides
pub struct ScanMatch {
```

**Fields:**

| Name | Type |
|------|------|
| `x` | `f64` |
| `y` | `f64` |
| `theta` | `f64` |
| `rmse` | `f64` |
| `converged` | `bool` |
| `iterations` | `i64` |

*Defined at line 359*

---

## lidar_match

Where the sensor of `cur` is in the frame of `reference`, by ICP of the
two scans' points from the guess (x, y, theta); pairs farther apart than
max_dist are ignored.

```fides
pub fn lidar_match(reference: &LaserScan, cur: &LaserScan, x: f64, y: f64, theta: f64, max_dist: f64) -> ScanMatch {
```

**Parameters:**

- `reference`: `&LaserScan`
- `cur`: `&LaserScan`
- `x`: `f64`
- `y`: `f64`
- `theta`: `f64`
- `max_dist`: `f64`

**Returns:** `ScanMatch`

*Defined at line 371*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
