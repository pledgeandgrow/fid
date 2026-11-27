[All modules](index.md)

# planner -- Fides Documentation

## Contents

- [PlanRng](#PlanRng) (struct)
- [plan_rng_new](#plan_rng_new) (function)
- [plan_rng_next](#plan_rng_next) (function)
- [plan_rng_uniform](#plan_rng_uniform) (function)
- [PlanGrid](#PlanGrid) (struct)
- [plan_grid_new](#plan_grid_new) (function)
- [plan_grid_from_rows](#plan_grid_from_rows) (function)
- [plan_grid_in](#plan_grid_in) (function)
- [plan_grid_free](#plan_grid_free) (function)
- [plan_grid_set](#plan_grid_set) (function)
- [plan_grid_step_cost](#plan_grid_step_cost) (function)
- [plan_grid_heuristic](#plan_grid_heuristic) (function)
- [PlanHeap](#PlanHeap) (struct)
- [plan_heap_new](#plan_heap_new) (function)
- [plan_heap_push](#plan_heap_push) (function)
- [plan_heap_len](#plan_heap_len) (function)
- [plan_heap_pop](#plan_heap_pop) (function)
- [GridPath](#GridPath) (struct)
- [plan_astar](#plan_astar) (function)
- [plan_grid_path_cost](#plan_grid_path_cost) (function)
- [plan_grid_line_free](#plan_grid_line_free) (function)
- [plan_grid_shortcut](#plan_grid_shortcut) (function)
- [plan_grid_polyline_length](#plan_grid_polyline_length) (function)
- [DStarLite](#DStarLite) (struct)
- [dstar_new](#dstar_new) (function)
- [dstar_compute](#dstar_compute) (function)
- [dstar_cost](#dstar_cost) (function)
- [dstar_path](#dstar_path) (function)
- [dstar_move_start](#dstar_move_start) (function)
- [dstar_set_blocked](#dstar_set_blocked) (function)
- [PlanWorld](#PlanWorld) (struct)
- [plan_world_new](#plan_world_new) (function)
- [plan_world_add_circle](#plan_world_add_circle) (function)
- [plan_world_add_box](#plan_world_add_box) (function)
- [plan_world_segment_free](#plan_world_segment_free) (function)
- [plan_world_point_free](#plan_world_point_free) (function)
- [plan_path_free](#plan_path_free) (function)
- [plan_path_length](#plan_path_length) (function)
- [PlanParams](#PlanParams) (struct)
- [plan_params](#plan_params) (function)
- [PlanResult](#PlanResult) (struct)
- [plan_rrt](#plan_rrt) (function)
- [plan_rrt_star](#plan_rrt_star) (function)
- [plan_shortcut](#plan_shortcut) (function)
- [plan_shortcut_random](#plan_shortcut_random) (function)

## PlanRng

SplitMix64 state.

```fides
pub struct PlanRng {
```

**Fields:**

| Name | Type |
|------|------|
| `state` | `i64` |

*Defined at line 39*

---

## plan_rng_new

A generator seeded with `seed`.

```fides
pub fn plan_rng_new(seed: i64) -> PlanRng {
```

**Parameters:**

- `seed`: `i64`

**Returns:** `PlanRng`

*Defined at line 44*

---

## plan_rng_next

```fides
pub fn plan_rng_next(r: &mut PlanRng) -> i64 {
```

**Parameters:**

- `r`: `&mut PlanRng`

**Returns:** `i64`

*Defined at line 52*

---

## plan_rng_uniform

```fides
pub fn plan_rng_uniform(r: &mut PlanRng) -> f64 {
```

**Parameters:**

- `r`: `&mut PlanRng`

**Returns:** `f64`

*Defined at line 63*

---

## PlanGrid

A grid of free / blocked cells; cell (x, y) has id y * width + x.

```fides
pub struct PlanGrid {
```

**Fields:**

| Name | Type |
|------|------|
| `width` | `i64` |
| `height` | `i64` |
| `blocked` | `Vec<bool>` |

*Defined at line 71*

---

## plan_grid_new

A width x height grid with every cell free.

```fides
pub fn plan_grid_new(width: i64, height: i64) -> PlanGrid {
```

**Parameters:**

- `width`: `i64`
- `height`: `i64`

**Returns:** `PlanGrid`

*Defined at line 78*

---

## plan_grid_from_rows

A grid from text rows ('#' blocked, anything else free); row k is y = k.

```fides
pub fn plan_grid_from_rows(rows: &Vec<str>) -> PlanGrid {
```

**Parameters:**

- `rows`: `&Vec<str>`

**Returns:** `PlanGrid`

*Defined at line 91*

---

## plan_grid_in

True when (x, y) is inside the grid.

```fides
pub fn plan_grid_in(g: &PlanGrid, x: i64, y: i64) -> bool {
```

**Parameters:**

- `g`: `&PlanGrid`
- `x`: `i64`
- `y`: `i64`

**Returns:** `bool`

*Defined at line 118*

---

## plan_grid_free

True when (x, y) is inside and free.

```fides
pub fn plan_grid_free(g: &PlanGrid, x: i64, y: i64) -> bool {
```

**Parameters:**

- `g`: `&PlanGrid`
- `x`: `i64`
- `y`: `i64`

**Returns:** `bool`

*Defined at line 123*

---

## plan_grid_set

Block or free cell (x, y).

```fides
pub fn plan_grid_set(g: &mut PlanGrid, x: i64, y: i64, blocked: bool) {
```

**Parameters:**

- `g`: `&mut PlanGrid`
- `x`: `i64`
- `y`: `i64`
- `blocked`: `bool`

*Defined at line 128*

---

## plan_grid_step_cost

The cost of the move from cell a to its neighbour b (1 or sqrt 2), or a
negative number when it is not allowed (blocked, or a diagonal past a
blocked corner).

```fides
pub fn plan_grid_step_cost(g: &PlanGrid, ax: i64, ay: i64, bx: i64, by: i64) -> f64 {
```

**Parameters:**

- `g`: `&PlanGrid`
- `ax`: `i64`
- `ay`: `i64`
- `bx`: `i64`
- `by`: `i64`

**Returns:** `f64`

*Defined at line 137*

---

## plan_grid_heuristic

The heuristic between cells: Manhattan (4-connected) or octile (8).

```fides
pub fn plan_grid_heuristic(ax: i64, ay: i64, bx: i64, by: i64, diag: bool) -> f64 {
```

**Parameters:**

- `ax`: `i64`
- `ay`: `i64`
- `bx`: `i64`
- `by`: `i64`
- `diag`: `bool`

**Returns:** `f64`

*Defined at line 179*

---

## PlanHeap

A min-heap ordered by (k1, k2).

```fides
pub struct PlanHeap {
```

**Fields:**

| Name | Type |
|------|------|
| `k1` | `Vec<f64>` |
| `k2` | `Vec<f64>` |
| `node` | `Vec<i64>` |

*Defined at line 197*

---

## plan_heap_new

An empty heap.

```fides
pub fn plan_heap_new() -> PlanHeap {
```

**Returns:** `PlanHeap`

*Defined at line 204*

---

## plan_heap_push

Add node with key (k1, k2).

```fides
pub fn plan_heap_push(h: &mut PlanHeap, k1: f64, k2: f64, node: i64) {
```

**Parameters:**

- `h`: `&mut PlanHeap`
- `k1`: `f64`
- `k2`: `f64`
- `node`: `i64`

*Defined at line 233*

---

## plan_heap_len

The number of entries.

```fides
pub fn plan_heap_len(h: &PlanHeap) -> i64 {
```

**Parameters:**

- `h`: `&PlanHeap`

**Returns:** `i64`

*Defined at line 250*

---

## plan_heap_pop

Remove the smallest entry (the heap must not be empty).

```fides
pub fn plan_heap_pop(h: &mut PlanHeap) {
```

**Parameters:**

- `h`: `&mut PlanHeap`

*Defined at line 255*

---

## GridPath

A grid path: cell ids from start to goal (empty when none), its cost and
the number of expanded cells.

```fides
pub struct GridPath {
```

**Fields:**

| Name | Type |
|------|------|
| `found` | `bool` |
| `cells` | `Vec<i64>` |
| `cost` | `f64` |
| `expanded` | `i64` |

*Defined at line 285*

---

## plan_astar

The cheapest path from (sx, sy) to (gx, gy); diag allows the 8 moves.

```fides
pub fn plan_astar(g: &PlanGrid, sx: i64, sy: i64, gx: i64, gy: i64, diag: bool) -> GridPath {
```

**Parameters:**

- `g`: `&PlanGrid`
- `sx`: `i64`
- `sy`: `i64`
- `gx`: `i64`
- `gy`: `i64`
- `diag`: `bool`

**Returns:** `GridPath`

*Defined at line 293*

---

## plan_grid_path_cost

The sum of step costs along a cell path (negative if a step is illegal).

```fides
pub fn plan_grid_path_cost(g: &PlanGrid, cells: &Vec<i64>) -> f64 {
```

**Parameters:**

- `g`: `&PlanGrid`
- `cells`: `&Vec<i64>`

**Returns:** `f64`

*Defined at line 375*

---

## plan_grid_line_free

True when the straight segment between the centres of cells a and b
touches only free cells (a supercover walk: both cells are visited
where the segment passes exactly through a corner).

```fides
pub fn plan_grid_line_free(g: &PlanGrid, ax: i64, ay: i64, bx: i64, by: i64) -> bool {
```

**Parameters:**

- `g`: `&PlanGrid`
- `ax`: `i64`
- `ay`: `i64`
- `bx`: `i64`
- `by`: `i64`

**Returns:** `bool`

*Defined at line 394*

---

## plan_grid_shortcut

The waypoints of a cell path after dropping every cell a straight free
line can skip (greedy, from the start).

```fides
pub fn plan_grid_shortcut(g: &PlanGrid, cells: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `g`: `&PlanGrid`
- `cells`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 441*

---

## plan_grid_polyline_length

The Euclidean length of a waypoint list of cell ids.

```fides
pub fn plan_grid_polyline_length(g: &PlanGrid, cells: &Vec<i64>) -> f64 {
```

**Parameters:**

- `g`: `&PlanGrid`
- `cells`: `&Vec<i64>`

**Returns:** `f64`

*Defined at line 465*

---

## DStarLite

D* Lite state over an 8-connected grid.

```fides
pub struct DStarLite {
```

**Fields:**

| Name | Type |
|------|------|
| `grid` | `PlanGrid` |
| `start` | `i64` |
| `goal` | `i64` |
| `last` | `i64` |
| `km` | `f64` |
| `g` | `Vec<f64>` |
| `rhs` | `Vec<f64>` |
| `qk1` | `Vec<f64>` |
| `qk2` | `Vec<f64>` |
| `inq` | `Vec<bool>` |
| `open` | `PlanHeap` |
| `expanded` | `i64` |

*Defined at line 480*

---

## dstar_new

D* Lite from (sx, sy) to (gx, gy) on a copy of the grid (8-connected).
Call dstar_compute before reading the path.

```fides
pub fn dstar_new(grid: &PlanGrid, sx: i64, sy: i64, gx: i64, gy: i64) -> DStarLite {
```

**Parameters:**

- `grid`: `&PlanGrid`
- `sx`: `i64`
- `sy`: `i64`
- `gx`: `i64`
- `gy`: `i64`

**Returns:** `DStarLite`

*Defined at line 589*

---

## dstar_compute

Bring g up to date for the current start (ComputeShortestPath).

```fides
pub fn dstar_compute(d: &mut DStarLite) {
```

**Parameters:**

- `d`: `&mut DStarLite`

*Defined at line 625*

---

## dstar_cost

The cost of the current best path from the start (>= 1e300: none).

```fides
pub fn dstar_cost(d: &DStarLite) -> f64 {
```

**Parameters:**

- `d`: `&DStarLite`

**Returns:** `f64`

*Defined at line 674*

---

## dstar_path

The path from the current start to the goal following the cheapest
successor (empty when there is none).

```fides
pub fn dstar_path(d: &DStarLite) -> Vec<i64> {
```

**Parameters:**

- `d`: `&DStarLite`

**Returns:** `Vec<i64>`

*Defined at line 680*

---

## dstar_move_start

The robot is now at (x, y): the start moves and km grows by the
heuristic distance moved. Call dstar_compute afterwards.

```fides
pub fn dstar_move_start(d: &mut DStarLite, x: i64, y: i64) {
```

**Parameters:**

- `d`: `&mut DStarLite`
- `x`: `i64`
- `y`: `i64`

*Defined at line 716*

---

## dstar_set_blocked

Cell (x, y) became blocked (or free): every vertex whose edges changed
(the cell and its 8 neighbours) is updated. Call dstar_compute afterwards.

```fides
pub fn dstar_set_blocked(d: &mut DStarLite, x: i64, y: i64, blocked: bool) {
```

**Parameters:**

- `d`: `&mut DStarLite`
- `x`: `i64`
- `y`: `i64`
- `blocked`: `bool`

*Defined at line 725*

---

## PlanWorld

A rectangle [xmin, xmax] x [ymin, ymax] with circle and box obstacles;
`margin` is the clearance every point must keep from them.

```fides
pub struct PlanWorld {
```

**Fields:**

| Name | Type |
|------|------|
| `xmin` | `f64` |
| `ymin` | `f64` |
| `xmax` | `f64` |
| `ymax` | `f64` |
| `margin` | `f64` |
| `cx` | `Vec<f64>` |
| `cy` | `Vec<f64>` |
| `cr` | `Vec<f64>` |
| `bx0` | `Vec<f64>` |
| `by0` | `Vec<f64>` |
| `bx1` | `Vec<f64>` |
| `by1` | `Vec<f64>` |

*Defined at line 751*

---

## plan_world_new

An empty world.

```fides
pub fn plan_world_new(xmin: f64, ymin: f64, xmax: f64, ymax: f64, margin: f64) -> PlanWorld {
```

**Parameters:**

- `xmin`: `f64`
- `ymin`: `f64`
- `xmax`: `f64`
- `ymax`: `f64`
- `margin`: `f64`

**Returns:** `PlanWorld`

*Defined at line 767*

---

## plan_world_add_circle

Add a disc obstacle.

```fides
pub fn plan_world_add_circle(w: &mut PlanWorld, x: f64, y: f64, r: f64) {
```

**Parameters:**

- `w`: `&mut PlanWorld`
- `x`: `f64`
- `y`: `f64`
- `r`: `f64`

*Defined at line 774*

---

## plan_world_add_box

Add an axis-aligned box obstacle [x0, x1] x [y0, y1].

```fides
pub fn plan_world_add_box(w: &mut PlanWorld, x0: f64, y0: f64, x1: f64, y1: f64) {
```

**Parameters:**

- `w`: `&mut PlanWorld`
- `x0`: `f64`
- `y0`: `f64`
- `x1`: `f64`
- `y1`: `f64`

*Defined at line 781*

---

## plan_world_segment_free

True when the segment ab stays inside the world and at least `margin`
away from every obstacle.

```fides
pub fn plan_world_segment_free(w: &PlanWorld, a: vec2, b: vec2) -> bool {
```

**Parameters:**

- `w`: `&PlanWorld`
- `a`: `vec2`
- `b`: `vec2`

**Returns:** `bool`

*Defined at line 827*

---

## plan_world_point_free

True when point p is free.

```fides
pub fn plan_world_point_free(w: &PlanWorld, p: vec2) -> bool {
```

**Parameters:**

- `w`: `&PlanWorld`
- `p`: `vec2`

**Returns:** `bool`

*Defined at line 868*

---

## plan_path_free

True when every segment of the path is free.

```fides
pub fn plan_path_free(w: &PlanWorld, path: &Vec<vec2>) -> bool {
```

**Parameters:**

- `w`: `&PlanWorld`
- `path`: `&Vec<vec2>`

**Returns:** `bool`

*Defined at line 873*

---

## plan_path_length

The length of a polyline.

```fides
pub fn plan_path_length(path: &Vec<vec2>) -> f64 {
```

**Parameters:**

- `path`: `&Vec<vec2>`

**Returns:** `f64`

*Defined at line 885*

---

## PlanParams

Settings of the sampling planners.

```fides
pub struct PlanParams {
```

**Fields:**

| Name | Type |
|------|------|
| `step` | `f64` |
| `goal_bias` | `f64` |
| `max_iter` | `i64` |
| `goal_tol` | `f64` |
| `seed` | `i64` |
| `radius` | `f64` |

*Defined at line 898*

---

## plan_params

Settings with radius 2 * step.

```fides
pub fn plan_params(step: f64, goal_bias: f64, max_iter: i64, goal_tol: f64, seed: i64) -> PlanParams {
```

**Parameters:**

- `step`: `f64`
- `goal_bias`: `f64`
- `max_iter`: `i64`
- `goal_tol`: `f64`
- `seed`: `i64`

**Returns:** `PlanParams`

*Defined at line 912*

---

## PlanResult

A sampling planner's result.

```fides
pub struct PlanResult {
```

**Fields:**

| Name | Type |
|------|------|
| `found` | `bool` |
| `path` | `Vec<vec2>` |
| `cost` | `f64` |
| `nodes` | `i64` |
| `iterations` | `i64` |

*Defined at line 919*

---

## plan_rrt

RRT from start to goal: stops at the first connection to the goal.

```fides
pub fn plan_rrt(w: &PlanWorld, start: vec2, goal: vec2, p: PlanParams) -> PlanResult {
```

**Parameters:**

- `w`: `&PlanWorld`
- `start`: `vec2`
- `goal`: `vec2`
- `p`: `PlanParams`

**Returns:** `PlanResult`

*Defined at line 1129*

---

## plan_rrt_star

RRT*: runs all max_iter iterations, choosing parents and rewiring within
the radius, and returns the cheapest path to the goal found.

```fides
pub fn plan_rrt_star(w: &PlanWorld, start: vec2, goal: vec2, p: PlanParams) -> PlanResult {
```

**Parameters:**

- `w`: `&PlanWorld`
- `start`: `vec2`
- `goal`: `vec2`
- `p`: `PlanParams`

**Returns:** `PlanResult`

*Defined at line 1135*

---

## plan_shortcut

Greedy shortcutting: from each waypoint jump to the farthest later one
a free segment reaches.

```fides
pub fn plan_shortcut(w: &PlanWorld, path: &Vec<vec2>) -> Vec<vec2> {
```

**Parameters:**

- `w`: `&PlanWorld`
- `path`: `&Vec<vec2>`

**Returns:** `Vec<vec2>`

*Defined at line 1143*

---

## plan_shortcut_random

Random shortcutting: `iters` times pick two points on the path (on any
segments) and splice in the straight segment between them when it is
free.

```fides
pub fn plan_shortcut_random(w: &PlanWorld, path: &Vec<vec2>, iters: i64, seed: i64) -> Vec<vec2> {
```

**Parameters:**

- `w`: `&PlanWorld`
- `path`: `&Vec<vec2>`
- `iters`: `i64`
- `seed`: `i64`

**Returns:** `Vec<vec2>`

*Defined at line 1167*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
