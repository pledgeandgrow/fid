[All modules](index.md)

# terrain -- Fides Documentation

## Contents

- [Terrain](#Terrain) (struct)
- [terrain_create](#terrain_create) (function)
- [terrain_set_origin](#terrain_set_origin) (function)
- [terrain_set_height](#terrain_set_height) (function)
- [terrain_get_height_grid](#terrain_get_height_grid) (function)
- [terrain_height_at](#terrain_height_at) (function)
- [terrain_normal_at](#terrain_normal_at) (function)
- [terrain_generate_sine](#terrain_generate_sine) (function)
- [terrain_generate_flat](#terrain_generate_flat) (function)
- [terrain_generate_noise](#terrain_generate_noise) (function)
- [terrain_raise_circle](#terrain_raise_circle) (function)
- [terrain_to_mesh](#terrain_to_mesh) (function)
- [terrain_world_width](#terrain_world_width) (function)
- [terrain_world_depth](#terrain_world_depth) (function)
- [terrain_vertex_count](#terrain_vertex_count) (function)
- [terrain_triangle_count](#terrain_triangle_count) (function)
- [terrain_in_bounds](#terrain_in_bounds) (function)
- [terrain_smooth](#terrain_smooth) (function)

## Terrain

```fides
pub struct Terrain {
```

**Fields:**

| Name | Type |
|------|------|
| `width` | `i64` |
| `height` | `i64` |
| `spacing` | `f64` |
| `heights` | `Vec<f64>` |
| `origin_x` | `f64` |
| `origin_z` | `f64` |
| `max_height` | `f64` |
| `min_height` | `f64` |

*Defined at line 22*

---

## terrain_create

Create a terrain grid.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var t = terrain_create(3, 3, 1.0);
terrain_set_height(&mut t, 1, 1, 4.0);
assert(terrain_get_height_grid(&t, 1, 1) == 4.0);
assert(terrain_get_height_grid(&t, 0, 0) == 0.0);
assert(terrain_height_at(&t, 1.0, 1.0) == 4.0);
assert(terrain_height_at(&t, 0.5, 1.0) == 2.0);
```

```fides
pub fn terrain_create(width: i64, height: i64, spacing: f64) -> Terrain {
```

**Parameters:**

- `width`: `i64`
- `height`: `i64`
- `spacing`: `f64`

**Returns:** `Terrain`

*Defined at line 47*

---

## terrain_set_origin

Set the terrain origin (world-space offset).

```fides
pub fn terrain_set_origin(t: &mut Terrain, x: f64, z: f64) -> void {
```

**Parameters:**

- `t`: `&mut Terrain`
- `x`: `f64`
- `z`: `f64`

**Returns:** `void`

*Defined at line 60*

---

## terrain_set_height

Set the height at a grid point.

```fides
pub fn terrain_set_height(t: &mut Terrain, gx: i64, gy: i64, h: f64) -> void {
```

**Parameters:**

- `t`: `&mut Terrain`
- `gx`: `i64`
- `gy`: `i64`
- `h`: `f64`

**Returns:** `void`

*Defined at line 66*

---

## terrain_get_height_grid

Get the height at a grid point.

```fides
pub fn terrain_get_height_grid(t: &Terrain, gx: i64, gy: i64) -> f64 {
```

**Parameters:**

- `t`: `&Terrain`
- `gx`: `i64`
- `gy`: `i64`

**Returns:** `f64`

*Defined at line 74*

---

## terrain_height_at

Get the height at a world-space (x, z) position using bilinear interpolation.

```fides
pub fn terrain_height_at(t: &Terrain, wx: f64, wz: f64) -> f64 {
```

**Parameters:**

- `t`: `&Terrain`
- `wx`: `f64`
- `wz`: `f64`

**Returns:** `f64`

*Defined at line 82*

---

## terrain_normal_at

Compute the normal at a world-space (x, z) position using finite differences.

```fides
pub fn terrain_normal_at(t: &Terrain, wx: f64, wz: f64) -> vec3 {
```

**Parameters:**

- `t`: `&Terrain`
- `wx`: `f64`
- `wz`: `f64`

**Returns:** `vec3`

*Defined at line 122*

---

## terrain_generate_sine

Generate terrain using a sine wave pattern.

```fides
pub fn terrain_generate_sine(t: &mut Terrain) -> void {
```

**Parameters:**

- `t`: `&mut Terrain`

**Returns:** `void`

*Defined at line 141*

---

## terrain_generate_flat

Generate flat terrain (all zeros).

```fides
pub fn terrain_generate_flat(t: &mut Terrain) -> void {
```

**Parameters:**

- `t`: `&mut Terrain`

**Returns:** `void`

*Defined at line 168*

---

## terrain_generate_noise

Generate terrain using a pseudo-random noise function.

```fides
pub fn terrain_generate_noise(t: &mut Terrain, amplitude: f64) -> void {
```

**Parameters:**

- `t`: `&mut Terrain`
- `amplitude`: `f64`

**Returns:** `void`

*Defined at line 179*

---

## terrain_raise_circle

Raise a circular area of the terrain.

```fides
pub fn terrain_raise_circle(t: &mut Terrain, cx: f64, cz: f64, radius: f64, amount: f64) -> void {
```

**Parameters:**

- `t`: `&mut Terrain`
- `cx`: `f64`
- `cz`: `f64`
- `radius`: `f64`
- `amount`: `f64`

**Returns:** `void`

*Defined at line 206*

---

## terrain_to_mesh

Convert the terrain to a mesh for rendering.

```fides
pub fn terrain_to_mesh(t: &Terrain) -> Mesh {
```

**Parameters:**

- `t`: `&Terrain`

**Returns:** `Mesh`

*Defined at line 246*

---

## terrain_world_width

Get the total world-space width of the terrain.

```fides
pub fn terrain_world_width(t: &Terrain) -> f64 {
```

**Parameters:**

- `t`: `&Terrain`

**Returns:** `f64`

*Defined at line 295*

---

## terrain_world_depth

Get the total world-space depth of the terrain.

```fides
pub fn terrain_world_depth(t: &Terrain) -> f64 {
```

**Parameters:**

- `t`: `&Terrain`

**Returns:** `f64`

*Defined at line 300*

---

## terrain_vertex_count

Get the number of vertices.

```fides
pub fn terrain_vertex_count(t: &Terrain) -> i64 {
```

**Parameters:**

- `t`: `&Terrain`

**Returns:** `i64`

*Defined at line 305*

---

## terrain_triangle_count

Get the number of triangles.

```fides
pub fn terrain_triangle_count(t: &Terrain) -> i64 {
```

**Parameters:**

- `t`: `&Terrain`

**Returns:** `i64`

*Defined at line 310*

---

## terrain_in_bounds

Check if a world-space point is within the terrain bounds.

```fides
pub fn terrain_in_bounds(t: &Terrain, wx: f64, wz: f64) -> bool {
```

**Parameters:**

- `t`: `&Terrain`
- `wx`: `f64`
- `wz`: `f64`

**Returns:** `bool`

*Defined at line 315*

---

## terrain_smooth

Smooth the terrain using a simple box filter.

```fides
pub fn terrain_smooth(t: &mut Terrain) -> void {
```

**Parameters:**

- `t`: `&mut Terrain`

**Returns:** `void`

*Defined at line 324*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
