# navmesh — Fides Documentation

## Contents

- [NavGrid](#navgrid) (struct)
- [nav_grid_create](#nav_grid_create) (function)
- [nav_grid_set_blocked](#nav_grid_set_blocked) (function)
- [nav_grid_is_blocked](#nav_grid_is_blocked) (function)
- [nav_grid_index](#nav_grid_index) (function)
- [nav_grid_unpack](#nav_grid_unpack) (function)
- [AStarNode](#astarnode) (struct)
- [nav_grid_find_path](#nav_grid_find_path) (function)
- [nav_grid_path_length](#nav_grid_path_length) (function)
- [nav_grid_path_waypoint](#nav_grid_path_waypoint) (function)
- [NavPoly](#navpoly) (struct)
- [NavMesh](#navmesh) (struct)
- [navmesh_create](#navmesh_create) (function)
- [navmesh_add_polygon](#navmesh_add_polygon) (function)
- [navmesh_connect](#navmesh_connect) (function)
- [navmesh_find_path](#navmesh_find_path) (function)
- [navmesh_poly_center](#navmesh_poly_center) (function)
- [navmesh_poly_count](#navmesh_poly_count) (function)

## NavGrid

```fides
pub struct NavGrid {
```

**Fields:**

| Name | Type |
|------|------|
| `width` | `i64` |
| `height` | `i64` |
| `cells` | `Vec<bool>,     // true = blocked` |

*Defined at line 24*

---

## nav_grid_create

```fides
pub fn nav_grid_create(width: i64, height: i64) -> NavGrid {
```

**Parameters:**

- `width`: `i64`
- `height`: `i64`

**Returns:** `NavGrid`

*Defined at line 30*

---

## nav_grid_set_blocked

```fides
pub fn nav_grid_set_blocked(grid: &mut NavGrid, x: i64, y: i64, blocked: bool) -> void {
```

**Parameters:**

- `grid`: `&mut NavGrid`
- `x`: `i64`
- `y`: `i64`
- `blocked`: `bool`

**Returns:** `void`

*Defined at line 44*

---

## nav_grid_is_blocked

```fides
pub fn nav_grid_is_blocked(grid: &NavGrid, x: i64, y: i64) -> bool {
```

**Parameters:**

- `grid`: `&NavGrid`
- `x`: `i64`
- `y`: `i64`

**Returns:** `bool`

*Defined at line 49*

---

## nav_grid_index

```fides
pub fn nav_grid_index(x: i64, y: i64) -> i64 {
```

**Parameters:**

- `x`: `i64`
- `y`: `i64`

**Returns:** `i64`

*Defined at line 54*

---

## nav_grid_unpack

```fides
pub fn nav_grid_unpack(idx: i64) -> vec2 {
```

**Parameters:**

- `idx`: `i64`

**Returns:** `vec2`

*Defined at line 58*

---

## AStarNode

```fides
pub struct AStarNode {
```

**Fields:**

| Name | Type |
|------|------|
| `x` | `i64` |
| `y` | `i64` |
| `g` | `f64,    // cost from start` |
| `h` | `f64,    // heuristic to goal` |
| `f` | `f64,    // g + h` |
| `parent_x` | `i64` |
| `parent_y` | `i64` |
| `open` | `bool` |
| `closed` | `bool` |

*Defined at line 63*

---

## nav_grid_find_path

Find a path on the grid using A*. Returns a Vec<i64> of packed (x,y) coordinates.

```fides
pub fn nav_grid_find_path(grid: &NavGrid, start_x: i64, start_y: i64, goal_x: i64, goal_y: i64) -> Vec<i64> {
```

**Parameters:**

- `grid`: `&NavGrid`
- `start_x`: `i64`
- `start_y`: `i64`
- `goal_x`: `i64`
- `goal_y`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 76*

---

## nav_grid_path_length

Get the path length (number of waypoints).

```fides
pub fn nav_grid_path_length(path: Vec<i64>) -> i64 {
```

**Parameters:**

- `path`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 223*

---

## nav_grid_path_waypoint

Get a waypoint from the path.

```fides
pub fn nav_grid_path_waypoint(path: Vec<i64>, idx: i64) -> vec2 {
```

**Parameters:**

- `path`: `Vec<i64>`
- `idx`: `i64`

**Returns:** `vec2`

*Defined at line 228*

---

## NavPoly

```fides
pub struct NavPoly {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `center_x` | `f64` |
| `center_y` | `f64` |
| `neighbors` | `Vec<i64>,    // adjacent polygon IDs` |

*Defined at line 235*

---

## NavMesh

```fides
pub struct NavMesh {
```

**Fields:**

| Name | Type |
|------|------|
| `polygons` | `Vec<NavPoly>` |

*Defined at line 242*

---

## navmesh_create

```fides
pub fn navmesh_create() -> NavMesh {
```

**Returns:** `NavMesh`

*Defined at line 246*

---

## navmesh_add_polygon

Add a polygon to the navmesh. Returns the polygon ID.

```fides
pub fn navmesh_add_polygon(mesh: &mut NavMesh, center_x: f64, center_y: f64) -> i64 {
```

**Parameters:**

- `mesh`: `&mut NavMesh`
- `center_x`: `f64`
- `center_y`: `f64`

**Returns:** `i64`

*Defined at line 251*

---

## navmesh_connect

Connect two polygons (bidirectional).

```fides
pub fn navmesh_connect(mesh: &mut NavMesh, a: i64, b: i64) -> void {
```

**Parameters:**

- `mesh`: `&mut NavMesh`
- `a`: `i64`
- `b`: `i64`

**Returns:** `void`

*Defined at line 263*

---

## navmesh_find_path

Find a path between two polygons using A*.

```fides
pub fn navmesh_find_path(mesh: &NavMesh, start: i64, goal: i64) -> Vec<i64> {
```

**Parameters:**

- `mesh`: `&NavMesh`
- `start`: `i64`
- `goal`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 275*

---

## navmesh_poly_center

Get the center of a polygon.

```fides
pub fn navmesh_poly_center(mesh: &NavMesh, id: i64) -> vec2 {
```

**Parameters:**

- `mesh`: `&NavMesh`
- `id`: `i64`

**Returns:** `vec2`

*Defined at line 382*

---

## navmesh_poly_count

Get the number of polygons.

```fides
pub fn navmesh_poly_count(mesh: &NavMesh) -> i64 {
```

**Parameters:**

- `mesh`: `&NavMesh`

**Returns:** `i64`

*Defined at line 389*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
