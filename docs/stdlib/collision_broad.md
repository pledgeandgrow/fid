[All modules](index.md)

# collision_broad -- Fides Documentation

## Contents

- [BroadAabb](#BroadAabb) (struct)
- [SpatialHashCell](#SpatialHashCell) (struct)
- [SpatialHashGrid](#SpatialHashGrid) (struct)
- [spatial_hash_create](#spatial_hash_create) (function)
- [spatial_hash_insert](#spatial_hash_insert) (function)
- [spatial_hash_query](#spatial_hash_query) (function)
- [spatial_hash_query_pairs](#spatial_hash_query_pairs) (function)
- [spatial_hash_clear](#spatial_hash_clear) (function)
- [spatial_hash_count](#spatial_hash_count) (function)
- [SapEndpoint](#SapEndpoint) (struct)
- [SweepAndPrune](#SweepAndPrune) (struct)
- [sap_create](#sap_create) (function)
- [sap_insert](#sap_insert) (function)
- [sap_query_pairs](#sap_query_pairs) (function)
- [sap_clear](#sap_clear) (function)
- [sap_count](#sap_count) (function)
- [sap_update](#sap_update) (function)

## BroadAabb

```fides
pub struct BroadAabb {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `min_x` | `f64` |
| `min_y` | `f64` |
| `max_x` | `f64` |
| `max_y` | `f64` |

*Defined at line 27*

---

## SpatialHashCell

```fides
pub struct SpatialHashCell {
```

**Fields:**

| Name | Type |
|------|------|
| `ids` | `Vec<i64>` |

*Defined at line 37*

---

## SpatialHashGrid

```fides
pub struct SpatialHashGrid {
```

**Fields:**

| Name | Type |
|------|------|
| `cell_size` | `f64` |
| `cells` | `Vec<SpatialHashCell>` |
| `cell_count` | `i64` |
| `aabbs` | `Vec<BroadAabb>` |

*Defined at line 41*

---

## spatial_hash_create

Create a spatial hash grid with the given cell size.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var g = spatial_hash_create(64.0);
spatial_hash_insert(&mut g, 0, 0.0, 0.0, 32.0, 32.0);
spatial_hash_insert(&mut g, 1, 10.0, 10.0, 42.0, 42.0);
spatial_hash_insert(&mut g, 2, 200.0, 200.0, 232.0, 232.0);
assert_eq(spatial_hash_count(&g), 3);
assert_eq(spatial_hash_query(&g, 0.0, 0.0, 5.0, 5.0).len(), 2);
assert_eq(spatial_hash_query(&g, 100.0, 100.0, 110.0, 110.0).len(), 0);
spatial_hash_clear(&mut g);
assert_eq(spatial_hash_count(&g), 0);
```

```fides
pub fn spatial_hash_create(cell_size: f64) -> SpatialHashGrid {
```

**Parameters:**

- `cell_size`: `f64`

**Returns:** `SpatialHashGrid`

*Defined at line 60*

---

## spatial_hash_insert

Insert an AABB into the grid.

```fides
pub fn spatial_hash_insert(grid: &mut SpatialHashGrid, id: i64, min_x: f64, min_y: f64, max_x: f64, max_y: f64) -> void {
```

**Parameters:**

- `grid`: `&mut SpatialHashGrid`
- `id`: `i64`
- `min_x`: `f64`
- `min_y`: `f64`
- `max_x`: `f64`
- `max_y`: `f64`

**Returns:** `void`

*Defined at line 77*

---

## spatial_hash_query

Query for all IDs that could potentially collide with the given AABB.

```fides
pub fn spatial_hash_query(grid: &SpatialHashGrid, min_x: f64, min_y: f64, max_x: f64, max_y: f64) -> Vec<i64> {
```

**Parameters:**

- `grid`: `&SpatialHashGrid`
- `min_x`: `f64`
- `min_y`: `f64`
- `max_x`: `f64`
- `max_y`: `f64`

**Returns:** `Vec<i64>`

*Defined at line 110*

---

## spatial_hash_query_pairs

Find all pairs of potentially colliding AABBs.

```fides
pub fn spatial_hash_query_pairs(grid: &SpatialHashGrid) -> Vec<i64> {
```

**Parameters:**

- `grid`: `&SpatialHashGrid`

**Returns:** `Vec<i64>`

*Defined at line 151*

---

## spatial_hash_clear

Clear all entries from the grid.

```fides
pub fn spatial_hash_clear(grid: &mut SpatialHashGrid) -> void {
```

**Parameters:**

- `grid`: `&mut SpatialHashGrid`

**Returns:** `void`

*Defined at line 188*

---

## spatial_hash_count

Get the number of AABBs in the grid.

```fides
pub fn spatial_hash_count(grid: &SpatialHashGrid) -> i64 {
```

**Parameters:**

- `grid`: `&SpatialHashGrid`

**Returns:** `i64`

*Defined at line 200*

---

## SapEndpoint

```fides
pub struct SapEndpoint {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `value` | `f64` |
| `is_min` | `bool,  // true = min endpoint, false = max` |

*Defined at line 206*

---

## SweepAndPrune

```fides
pub struct SweepAndPrune {
```

**Fields:**

| Name | Type |
|------|------|
| `endpoints` | `Vec<SapEndpoint>` |
| `aabbs` | `Vec<BroadAabb>` |

*Defined at line 212*

---

## sap_create

```fides
pub fn sap_create() -> SweepAndPrune {
```

**Returns:** `SweepAndPrune`

*Defined at line 217*

---

## sap_insert

Insert an AABB into the sweep-and-prune structure.

```fides
pub fn sap_insert(sap: &mut SweepAndPrune, id: i64, min_x: f64, max_x: f64) -> void {
```

**Parameters:**

- `sap`: `&mut SweepAndPrune`
- `id`: `i64`
- `min_x`: `f64`
- `max_x`: `f64`

**Returns:** `void`

*Defined at line 224*

---

## sap_query_pairs

Query for all overlapping pairs along the X axis.

```fides
pub fn sap_query_pairs(sap: &mut SweepAndPrune) -> Vec<i64> {
```

**Parameters:**

- `sap`: `&mut SweepAndPrune`

**Returns:** `Vec<i64>`

*Defined at line 259*

---

## sap_clear

Clear all entries.

```fides
pub fn sap_clear(sap: &mut SweepAndPrune) -> void {
```

**Parameters:**

- `sap`: `&mut SweepAndPrune`

**Returns:** `void`

*Defined at line 295*

---

## sap_count

Get the number of AABBs.

```fides
pub fn sap_count(sap: &SweepAndPrune) -> i64 {
```

**Parameters:**

- `sap`: `&SweepAndPrune`

**Returns:** `i64`

*Defined at line 301*

---

## sap_update

Update an existing AABB's position (removes and re-inserts).

```fides
pub fn sap_update(sap: &mut SweepAndPrune, id: i64, min_x: f64, max_x: f64) -> void {
```

**Parameters:**

- `sap`: `&mut SweepAndPrune`
- `id`: `i64`
- `min_x`: `f64`
- `max_x`: `f64`

**Returns:** `void`

*Defined at line 306*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
