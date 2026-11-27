[All modules](index.md)

# occupancy_grid -- Fides Documentation

## Contents

- [OccGrid](#OccGrid) (struct)
- [GridCell](#GridCell) (struct)
- [ogrid_new](#ogrid_new) (function)
- [ogrid_logit](#ogrid_logit) (function)
- [ogrid_inv_logit](#ogrid_inv_logit) (function)
- [ogrid_set_model](#ogrid_set_model) (function)
- [ogrid_in_bounds](#ogrid_in_bounds) (function)
- [ogrid_world_to_cell](#ogrid_world_to_cell) (function)
- [ogrid_cell_center](#ogrid_cell_center) (function)
- [ogrid_get](#ogrid_get) (function)
- [ogrid_update_cell](#ogrid_update_cell) (function)
- [ogrid_prob](#ogrid_prob) (function)
- [ogrid_prob_at](#ogrid_prob_at) (function)
- [ogrid_state](#ogrid_state) (function)
- [ogrid_count](#ogrid_count) (function)
- [ogrid_bresenham](#ogrid_bresenham) (function)
- [ogrid_insert_ray](#ogrid_insert_ray) (function)
- [ogrid_insert_scan](#ogrid_insert_scan) (function)
- [ogrid_raycast](#ogrid_raycast) (function)
- [ogrid_merge](#ogrid_merge) (function)
- [ogrid_merge_at](#ogrid_merge_at) (function)
- [ogrid_to_ascii](#ogrid_to_ascii) (function)

## OccGrid

A log-odds occupancy grid.

```fides
pub struct OccGrid {
```

**Fields:**

| Name | Type |
|------|------|
| `width` | `i64` |
| `height` | `i64` |
| `resolution` | `f64` |
| `origin_x` | `f64` |
| `origin_y` | `f64` |
| `logodds` | `Vec<f64>` |
| `l_occ` | `f64` |
| `l_free` | `f64` |
| `l_min` | `f64` |
| `l_max` | `f64` |

*Defined at line 38*

---

## GridCell

A cell index pair.

```fides
pub struct GridCell {
```

**Fields:**

| Name | Type |
|------|------|
| `x` | `i64` |
| `y` | `i64` |

*Defined at line 55*

---

## ogrid_new

A width x height grid of unknown cells; hit 0.7 / miss 0.4, clamped to
p in [0.12, 0.97] (the usual defaults of mapping packages).

```fides
pub fn ogrid_new(width: i64, height: i64, resolution: f64, origin_x: f64, origin_y: f64) -> OccGrid {
```

**Parameters:**

- `width`: `i64`
- `height`: `i64`
- `resolution`: `f64`
- `origin_x`: `f64`
- `origin_y`: `f64`

**Returns:** `OccGrid`

*Defined at line 62*

---

## ogrid_logit

log(p / (1 - p)).

```fides
pub fn ogrid_logit(p: f64) -> f64 {
```

**Parameters:**

- `p`: `f64`

**Returns:** `f64`

*Defined at line 75*

---

## ogrid_inv_logit

1 / (1 + exp(-l)).

```fides
pub fn ogrid_inv_logit(l: f64) -> f64 {
```

**Parameters:**

- `l`: `f64`

**Returns:** `f64`

*Defined at line 80*

---

## ogrid_set_model

The sensor model: p(occupied) of a hit cell and of a crossed cell, and
the clamp range of p.

```fides
pub fn ogrid_set_model(g: &mut OccGrid, p_hit: f64, p_miss: f64, p_min: f64, p_max: f64) {
```

**Parameters:**

- `g`: `&mut OccGrid`
- `p_hit`: `f64`
- `p_miss`: `f64`
- `p_min`: `f64`
- `p_max`: `f64`

*Defined at line 86*

---

## ogrid_in_bounds

True when (cx, cy) is inside the grid.

```fides
pub fn ogrid_in_bounds(g: &OccGrid, cx: i64, cy: i64) -> bool {
```

**Parameters:**

- `g`: `&OccGrid`
- `cx`: `i64`
- `cy`: `i64`

**Returns:** `bool`

*Defined at line 94*

---

## ogrid_world_to_cell

The cell containing world point (x, y) (it may be out of bounds).

```fides
pub fn ogrid_world_to_cell(g: &OccGrid, x: f64, y: f64) -> GridCell {
```

**Parameters:**

- `g`: `&OccGrid`
- `x`: `f64`
- `y`: `f64`

**Returns:** `GridCell`

*Defined at line 99*

---

## ogrid_cell_center

The world coordinates of the centre of cell (cx, cy).

```fides
pub fn ogrid_cell_center(g: &OccGrid, cx: i64, cy: i64) -> vec2 {
```

**Parameters:**

- `g`: `&OccGrid`
- `cx`: `i64`
- `cy`: `i64`

**Returns:** `vec2`

*Defined at line 106*

---

## ogrid_get

The log-odds of a cell (0 outside the grid).

```fides
pub fn ogrid_get(g: &OccGrid, cx: i64, cy: i64) -> f64 {
```

**Parameters:**

- `g`: `&OccGrid`
- `cx`: `i64`
- `cy`: `i64`

**Returns:** `f64`

*Defined at line 111*

---

## ogrid_update_cell

Add `delta` to a cell's log-odds, clamped; cells outside are ignored.

```fides
pub fn ogrid_update_cell(g: &mut OccGrid, cx: i64, cy: i64, delta: f64) {
```

**Parameters:**

- `g`: `&mut OccGrid`
- `cx`: `i64`
- `cy`: `i64`
- `delta`: `f64`

*Defined at line 119*

---

## ogrid_prob

p(occupied) of a cell; 0.5 outside the grid.

```fides
pub fn ogrid_prob(g: &OccGrid, cx: i64, cy: i64) -> f64 {
```

**Parameters:**

- `g`: `&OccGrid`
- `cx`: `i64`
- `cy`: `i64`

**Returns:** `f64`

*Defined at line 135*

---

## ogrid_prob_at

p(occupied) at world point (x, y).

```fides
pub fn ogrid_prob_at(g: &OccGrid, x: f64, y: f64) -> f64 {
```

**Parameters:**

- `g`: `&OccGrid`
- `x`: `f64`
- `y`: `f64`

**Returns:** `f64`

*Defined at line 140*

---

## ogrid_state

1 occupied (p > 0.65), 0 free (p < 0.35), -1 unknown.

```fides
pub fn ogrid_state(g: &OccGrid, cx: i64, cy: i64) -> i64 {
```

**Parameters:**

- `g`: `&OccGrid`
- `cx`: `i64`
- `cy`: `i64`

**Returns:** `i64`

*Defined at line 146*

---

## ogrid_count

How many cells are in `state` (1, 0 or -1).

```fides
pub fn ogrid_count(g: &OccGrid, state: i64) -> i64 {
```

**Parameters:**

- `g`: `&OccGrid`
- `state`: `i64`

**Returns:** `i64`

*Defined at line 158*

---

## ogrid_bresenham

The cells of the line from (x0, y0) to (x1, y1), both included, as a
flat Vec [x, y, x, y, ...] (Bresenham, every octant).

```fides
pub fn ogrid_bresenham(x0: i64, y0: i64, x1: i64, y1: i64) -> Vec<i64> {
```

**Parameters:**

- `x0`: `i64`
- `y0`: `i64`
- `x1`: `i64`
- `y1`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 183*

---

## ogrid_insert_ray

One beam from world (sx, sy) to (ex, ey): the cells before the end are
free; the end cell is occupied when `hit`, else free too.

```fides
pub fn ogrid_insert_ray(g: &mut OccGrid, sx: f64, sy: f64, ex: f64, ey: f64, hit: bool) {
```

**Parameters:**

- `g`: `&mut OccGrid`
- `sx`: `f64`
- `sy`: `f64`
- `ex`: `f64`
- `ey`: `f64`
- `hit`: `bool`

*Defined at line 219*

---

## ogrid_insert_scan

A scan from pose (px, py, theta): beam i at angle theta + angle_min +
i * angle_inc with range ranges[i]. Readings <= 0 or NaN are skipped;
readings >= range_max clear space up to range_max without a hit.

```fides
pub fn ogrid_insert_scan(g: &mut OccGrid, px: f64, py: f64, theta: f64, ranges: &Vec<f64>, angle_min: f64, angle_inc: f64, range_max: f64) {
```

**Parameters:**

- `g`: `&mut OccGrid`
- `px`: `f64`
- `py`: `f64`
- `theta`: `f64`
- `ranges`: `&Vec<f64>`
- `angle_min`: `f64`
- `angle_inc`: `f64`
- `range_max`: `f64`

*Defined at line 239*

---

## ogrid_raycast

The distance from world (x, y) along `angle` to the first occupied cell
(state 1), stepping a quarter cell at a time; max_range if none.

```fides
pub fn ogrid_raycast(g: &OccGrid, x: f64, y: f64, angle: f64, max_range: f64) -> f64 {
```

**Parameters:**

- `g`: `&OccGrid`
- `x`: `f64`
- `y`: `f64`
- `angle`: `f64`
- `max_range`: `f64`

**Returns:** `f64`

*Defined at line 259*

---

## ogrid_merge

The sum of the log-odds of a and b (clamped with a's limits), for two
grids of the same size, resolution and origin.

```fides
pub fn ogrid_merge(a: &OccGrid, b: &OccGrid) -> Result<OccGrid, str> {
```

**Parameters:**

- `a`: `&OccGrid`
- `b`: `&OccGrid`

**Returns:** `Result<OccGrid, str>`

*Defined at line 279*

---

## ogrid_merge_at

Add b's log-odds into a, b's cell (0, 0) on a's cell (dx, dy); cells
falling outside a are dropped.

```fides
pub fn ogrid_merge_at(a: &mut OccGrid, b: &OccGrid, dx: i64, dy: i64) {
```

**Parameters:**

- `a`: `&mut OccGrid`
- `b`: `&OccGrid`
- `dx`: `i64`
- `dy`: `i64`

*Defined at line 305*

---

## ogrid_to_ascii

The map as text: one line per row from the top (largest y), '#'
occupied, '.' free, '?' unknown.

```fides
pub fn ogrid_to_ascii(g: &OccGrid) -> str {
```

**Parameters:**

- `g`: `&OccGrid`

**Returns:** `str`

*Defined at line 322*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
