[All modules](index.md)

# bvh -- Fides Documentation

## Contents

- [BVH_BINS:](#BVH_BINS:) (const)
- [BVH_FAR:](#BVH_FAR:) (const)
- [bvh_f64s](#bvh_f64s) (function)
- [Bvh](#Bvh) (struct)
- [bvh_node_count](#bvh_node_count) (function)
- [bvh_is_leaf](#bvh_is_leaf) (function)
- [bvh_build](#bvh_build) (function)
- [bvh_build_median](#bvh_build_median) (function)
- [bvh_refit](#bvh_refit) (function)
- [bvh_sah_cost](#bvh_sah_cost) (function)
- [bvh_depth](#bvh_depth) (function)
- [bvh_check](#bvh_check) (function)
- [bvh_query_aabb](#bvh_query_aabb) (function)
- [BvhRay](#BvhRay) (struct)
- [bvh_ray](#bvh_ray) (function)
- [bvh_ray_box](#bvh_ray_box) (function)
- [BvhHit](#BvhHit) (struct)
- [bvh_no_hit](#bvh_no_hit) (function)
- [bvh_ray_triangle](#bvh_ray_triangle) (function)
- [BvhMesh](#BvhMesh) (struct)
- [bvh_mesh_new](#bvh_mesh_new) (function)
- [bvh_mesh_refit](#bvh_mesh_refit) (function)
- [bvh_mesh_closest](#bvh_mesh_closest) (function)
- [bvh_mesh_any](#bvh_mesh_any) (function)
- [bvh_mesh_closest_brute](#bvh_mesh_closest_brute) (function)

## BVH_BINS:

```fides
pub const BVH_BINS: i64 = 16;
```

*Defined at line 33*

---

## BVH_FAR:

```fides
pub const BVH_FAR: f64 = 1.0e30;
```

*Defined at line 34*

---

## bvh_f64s

```fides
pub fn bvh_f64s(n: i64, x: f64) -> Vec<f64> {
```

**Parameters:**

- `n`: `i64`
- `x`: `f64`

**Returns:** `Vec<f64>`

*Defined at line 38*

---

## Bvh

```fides
pub struct Bvh {
```

**Fields:**

| Name | Type |
|------|------|
| `bmin` | `Vec<f64>` |
| `bmax` | `Vec<f64>` |
| `left` | `Vec<i64>` |
| `first` | `Vec<i64>` |
| `count` | `Vec<i64>` |
| `order` | `Vec<i64>` |
| `pmin` | `Vec<f64>` |
| `pmax` | `Vec<f64>` |

*Defined at line 48*

---

## bvh_node_count

```fides
pub fn bvh_node_count(b: &Bvh) -> i64 {
```

**Parameters:**

- `b`: `&Bvh`

**Returns:** `i64`

*Defined at line 60*

---

## bvh_is_leaf

```fides
pub fn bvh_is_leaf(b: &Bvh, node: i64) -> bool {
```

**Parameters:**

- `b`: `&Bvh`
- `node`: `i64`

**Returns:** `bool`

*Defined at line 64*

---

## bvh_build

Build over primitive boxes (pmin/pmax: 3 values per primitive) with the
surface area heuristic. max_leaf >= 1.

```fides
pub fn bvh_build(pmin: Vec<f64>, pmax: Vec<f64>, max_leaf: i64) -> Bvh {
```

**Parameters:**

- `pmin`: `Vec<f64>`
- `pmax`: `Vec<f64>`
- `max_leaf`: `i64`

**Returns:** `Bvh`

*Defined at line 267*

---

## bvh_build_median

Build by splitting at the median centroid of the longest axis (for
comparison with the SAH build).

```fides
pub fn bvh_build_median(pmin: Vec<f64>, pmax: Vec<f64>, max_leaf: i64) -> Bvh {
```

**Parameters:**

- `pmin`: `Vec<f64>`
- `pmax`: `Vec<f64>`
- `max_leaf`: `i64`

**Returns:** `Bvh`

*Defined at line 305*

---

## bvh_refit

Recompute every box from new primitive boxes (same primitives).

```fides
pub fn bvh_refit(b: &mut Bvh, pmin: Vec<f64>, pmax: Vec<f64>) {
```

**Parameters:**

- `b`: `&mut Bvh`
- `pmin`: `Vec<f64>`
- `pmax`: `Vec<f64>`

*Defined at line 356*

---

## bvh_sah_cost

SAH cost of the tree: sum over nodes of area / root area, times 1 per
inner node and the primitive count per leaf.

```fides
pub fn bvh_sah_cost(b: &Bvh) -> f64 {
```

**Parameters:**

- `b`: `&Bvh`

**Returns:** `f64`

*Defined at line 378*

---

## bvh_depth

Depth of the deepest leaf (root = 1).

```fides
pub fn bvh_depth(b: &Bvh) -> i64 {
```

**Parameters:**

- `b`: `&Bvh`

**Returns:** `i64`

*Defined at line 401*

---

## bvh_check

Is every primitive inside its leaf's box and every child inside its
parent's box?

```fides
pub fn bvh_check(b: &Bvh) -> bool {
```

**Parameters:**

- `b`: `&Bvh`

**Returns:** `bool`

*Defined at line 424*

---

## bvh_query_aabb

Primitives whose box overlaps [qmin, qmax] (touching counts).

```fides
pub fn bvh_query_aabb(b: &Bvh, qminx: f64, qminy: f64, qminz: f64, qmaxx: f64, qmaxy: f64, qmaxz: f64) -> Vec<i64> {
```

**Parameters:**

- `b`: `&Bvh`
- `qminx`: `f64`
- `qminy`: `f64`
- `qminz`: `f64`
- `qmaxx`: `f64`
- `qmaxy`: `f64`
- `qmaxz`: `f64`

**Returns:** `Vec<i64>`

*Defined at line 457*

---

## BvhRay

```fides
pub struct BvhRay {
```

**Fields:**

| Name | Type |
|------|------|
| `ox` | `f64` |
| `oy` | `f64` |
| `oz` | `f64` |
| `dx` | `f64` |
| `dy` | `f64` |
| `dz` | `f64` |
| `ix` | `f64` |
| `iy` | `f64` |
| `iz` | `f64` |

*Defined at line 489*

---

## bvh_ray

```fides
pub fn bvh_ray(ox: f64, oy: f64, oz: f64, dx: f64, dy: f64, dz: f64) -> BvhRay {
```

**Parameters:**

- `ox`: `f64`
- `oy`: `f64`
- `oz`: `f64`
- `dx`: `f64`
- `dy`: `f64`
- `dz`: `f64`

**Returns:** `BvhRay`

*Defined at line 508*

---

## bvh_ray_box

Entry distance of the ray into node n's box within [0, tmax], or -1.

```fides
pub fn bvh_ray_box(b: &Bvh, n: i64, r: &BvhRay, tmax: f64) -> f64 {
```

**Parameters:**

- `b`: `&Bvh`
- `n`: `i64`
- `r`: `&BvhRay`
- `tmax`: `f64`

**Returns:** `f64`

*Defined at line 515*

---

## BvhHit

```fides
pub struct BvhHit {
```

**Fields:**

| Name | Type |
|------|------|
| `hit` | `bool` |
| `t` | `f64` |
| `prim` | `i64` |
| `u` | `f64` |
| `v` | `f64` |

*Defined at line 561*

---

## bvh_no_hit

```fides
pub fn bvh_no_hit() -> BvhHit {
```

**Returns:** `BvhHit`

*Defined at line 569*

---

## bvh_ray_triangle

Moller-Trumbore: the hit of ray o + t d with triangle (a, b, c) for t in
(tmin, tmax), two-sided; u, v barycentric (point = a + u (b-a) + v (c-a)).

```fides
pub fn bvh_ray_triangle(r: &BvhRay, ax: f64, ay: f64, az: f64, bx: f64, by: f64, bz: f64, cx: f64, cy: f64, cz: f64, tmin: f64, tmax: f64) -> BvhHit {
```

**Parameters:**

- `r`: `&BvhRay`
- `ax`: `f64`
- `ay`: `f64`
- `az`: `f64`
- `bx`: `f64`
- `by`: `f64`
- `bz`: `f64`
- `cx`: `f64`
- `cy`: `f64`
- `cz`: `f64`
- `tmin`: `f64`
- `tmax`: `f64`

**Returns:** `BvhHit`

*Defined at line 577*

---

## BvhMesh

```fides
pub struct BvhMesh {
```

**Fields:**

| Name | Type |
|------|------|
| `positions` | `Vec<f64>` |
| `indices` | `Vec<i64>` |
| `tree` | `Bvh` |

*Defined at line 617*

---

## bvh_mesh_new

A BVH over the triangles of an indexed mesh (positions xyz, 3 indices
per triangle).

```fides
pub fn bvh_mesh_new(positions: Vec<f64>, indices: Vec<i64>, max_leaf: i64) -> BvhMesh {
```

**Parameters:**

- `positions`: `Vec<f64>`
- `indices`: `Vec<i64>`
- `max_leaf`: `i64`

**Returns:** `BvhMesh`

*Defined at line 648*

---

## bvh_mesh_refit

New vertex positions, same triangles: refit instead of rebuilding.

```fides
pub fn bvh_mesh_refit(m: &mut BvhMesh, positions: Vec<f64>) {
```

**Parameters:**

- `m`: `&mut BvhMesh`
- `positions`: `Vec<f64>`

*Defined at line 656*

---

## bvh_mesh_closest

The nearest triangle hit along the ray with 0 < t < tmax.

```fides
pub fn bvh_mesh_closest(m: &BvhMesh, ox: f64, oy: f64, oz: f64, dx: f64, dy: f64, dz: f64, tmax: f64) -> BvhHit {
```

**Parameters:**

- `m`: `&BvhMesh`
- `ox`: `f64`
- `oy`: `f64`
- `oz`: `f64`
- `dx`: `f64`
- `dy`: `f64`
- `dz`: `f64`
- `tmax`: `f64`

**Returns:** `BvhHit`

*Defined at line 671*

---

## bvh_mesh_any

Is any triangle hit with 0 < t < tmax? (Stops at the first one.)

```fides
pub fn bvh_mesh_any(m: &BvhMesh, ox: f64, oy: f64, oz: f64, dx: f64, dy: f64, dz: f64, tmax: f64) -> bool {
```

**Parameters:**

- `m`: `&BvhMesh`
- `ox`: `f64`
- `oy`: `f64`
- `oz`: `f64`
- `dx`: `f64`
- `dy`: `f64`
- `dz`: `f64`
- `tmax`: `f64`

**Returns:** `bool`

*Defined at line 735*

---

## bvh_mesh_closest_brute

Brute force closest hit over every triangle (reference for tests).

```fides
pub fn bvh_mesh_closest_brute(m: &BvhMesh, ox: f64, oy: f64, oz: f64, dx: f64, dy: f64, dz: f64, tmax: f64) -> BvhHit {
```

**Parameters:**

- `m`: `&BvhMesh`
- `ox`: `f64`
- `oy`: `f64`
- `oz`: `f64`
- `dx`: `f64`
- `dy`: `f64`
- `dz`: `f64`
- `tmax`: `f64`

**Returns:** `BvhHit`

*Defined at line 766*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
