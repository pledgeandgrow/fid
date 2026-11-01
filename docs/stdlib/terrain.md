# terrain — Fides Documentation

## Contents

- [Terrain](#terrain) (struct)
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
| `width` | `i64,           // grid width (vertices)` |
| `height` | `i64,          // grid height (vertices)` |
| `spacing` | `f64,         // distance between grid points` |
| `heights` | `Vec<f64>,     // height values (width * height)` |
| `origin_x` | `f64,         // world offset` |
| `origin_z` | `f64` |
| `max_height` | `f64` |
| `min_height` | `f64` |

*Defined at line 24*

---

## terrain_create

Create a terrain grid.

```fides
pub fn terrain_create(width: i64, height: i64, spacing: f64) -> Terrain {
```

**Parameters:**

- `width`: `i64`
- `height`: `i64`
- `spacing`: `f64`

**Returns:** `Terrain`

*Defined at line 36*

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

*Defined at line 56*

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

*Defined at line 62*

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

*Defined at line 68*

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

*Defined at line 74*

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

*Defined at line 102*

---

## terrain_generate_sine

Generate terrain using a sine wave pattern.

```fides
pub fn terrain_generate_sine(t: &mut Terrain) -> void {
```

**Parameters:**

- `t`: `&mut Terrain`

**Returns:** `void`

*Defined at line 121*

---

## terrain_generate_flat

Generate flat terrain (all zeros).

```fides
pub fn terrain_generate_flat(t: &mut Terrain) -> void {
```

**Parameters:**

- `t`: `&mut Terrain`

**Returns:** `void`

*Defined at line 144*

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

*Defined at line 155*

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

*Defined at line 178*

---

## terrain_to_mesh

Convert the terrain to a mesh for rendering.

```fides
pub fn terrain_to_mesh(t: &Terrain) -> Mesh {
```

**Parameters:**

- `t`: `&Terrain`

**Returns:** `Mesh`

*Defined at line 210*

---

## terrain_world_width

Get the total world-space width of the terrain.

```fides
pub fn terrain_world_width(t: &Terrain) -> f64 {
```

**Parameters:**

- `t`: `&Terrain`

**Returns:** `f64`

*Defined at line 259*

---

## terrain_world_depth

Get the total world-space depth of the terrain.

```fides
pub fn terrain_world_depth(t: &Terrain) -> f64 {
```

**Parameters:**

- `t`: `&Terrain`

**Returns:** `f64`

*Defined at line 264*

---

## terrain_vertex_count

Get the number of vertices.

```fides
pub fn terrain_vertex_count(t: &Terrain) -> i64 {
```

**Parameters:**

- `t`: `&Terrain`

**Returns:** `i64`

*Defined at line 269*

---

## terrain_triangle_count

Get the number of triangles.

```fides
pub fn terrain_triangle_count(t: &Terrain) -> i64 {
```

**Parameters:**

- `t`: `&Terrain`

**Returns:** `i64`

*Defined at line 274*

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

*Defined at line 279*

---

## terrain_smooth

Smooth the terrain using a simple box filter.

```fides
pub fn terrain_smooth(t: &mut Terrain) -> void {
```

**Parameters:**

- `t`: `&mut Terrain`

**Returns:** `void`

*Defined at line 288*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
