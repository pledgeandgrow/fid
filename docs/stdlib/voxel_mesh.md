[All modules](index.md)

# voxel_mesh -- Fides Documentation

## Contents

- [vox_edge_corner](#vox_edge_corner) (function)
- [vox_mc_edge_mask](#vox_mc_edge_mask) (function)
- [vox_mc_triangles](#vox_mc_triangles) (function)
- [VoxChunk](#VoxChunk) (struct)
- [vox_chunk_new](#vox_chunk_new) (function)
- [vox_get](#vox_get) (function)
- [vox_set](#vox_set) (function)
- [vox_fill_box](#vox_fill_box) (function)
- [vox_solid_count](#vox_solid_count) (function)
- [VoxMesh](#VoxMesh) (struct)
- [vox_mesh_new](#vox_mesh_new) (function)
- [vox_mesh_vertex_count](#vox_mesh_vertex_count) (function)
- [vox_mesh_triangle_count](#vox_mesh_triangle_count) (function)
- [vox_ao_value](#vox_ao_value) (function)
- [vox_face_ao_xyz](#vox_face_ao_xyz) (function)
- [vox_mesh_naive](#vox_mesh_naive) (function)
- [vox_mesh_greedy](#vox_mesh_greedy) (function)
- [vox_mesh_area](#vox_mesh_area) (function)
- [vox_mesh_volume](#vox_mesh_volume) (function)
- [vox_field_sample](#vox_field_sample) (function)
- [vox_marching_cubes](#vox_marching_cubes) (function)
- [vox_mesh_is_closed](#vox_mesh_is_closed) (function)
- [vox_mesh_euler](#vox_mesh_euler) (function)
- [vox_mesh_to_obj](#vox_mesh_to_obj) (function)

## vox_edge_corner

The two corners (which = 0 or 1) of cube edge e (0..11).

```fides
pub fn vox_edge_corner(e: i64, which: i64) -> i64 {
```

**Parameters:**

- `e`: `i64`
- `which`: `i64`

**Returns:** `i64`

*Defined at line 73*

---

## vox_mc_edge_mask

Bourke's edgeTable entry for a cube configuration (12-bit mask).

```fides
pub fn vox_mc_edge_mask(config: i64) -> i64 {
```

**Parameters:**

- `config`: `i64`

**Returns:** `i64`

*Defined at line 129*

---

## vox_mc_triangles

The triangle list of a configuration: edge numbers, three per triangle.

```fides
pub fn vox_mc_triangles(config: i64) -> Vec<i64> {
```

**Parameters:**

- `config`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 135*

---

## VoxChunk

```fides
pub struct VoxChunk {
```

**Fields:**

| Name | Type |
|------|------|
| `sx` | `i64` |
| `sy` | `i64` |
| `sz` | `i64` |
| `cells` | `Vec<i64>` |

*Defined at line 148*

---

## vox_chunk_new

```fides
pub fn vox_chunk_new(sx: i64, sy: i64, sz: i64) -> VoxChunk {
```

**Parameters:**

- `sx`: `i64`
- `sy`: `i64`
- `sz`: `i64`

**Returns:** `VoxChunk`

*Defined at line 156*

---

## vox_get

The voxel at (x, y, z); 0 outside the chunk.

```fides
pub fn vox_get(c: &VoxChunk, x: i64, y: i64, z: i64) -> i64 {
```

**Parameters:**

- `c`: `&VoxChunk`
- `x`: `i64`
- `y`: `i64`
- `z`: `i64`

**Returns:** `i64`

*Defined at line 163*

---

## vox_set

```fides
pub fn vox_set(c: &mut VoxChunk, x: i64, y: i64, z: i64, m: i64) {
```

**Parameters:**

- `c`: `&mut VoxChunk`
- `x`: `i64`
- `y`: `i64`
- `z`: `i64`
- `m`: `i64`

*Defined at line 170*

---

## vox_fill_box

```fides
pub fn vox_fill_box(c: &mut VoxChunk, x0: i64, y0: i64, z0: i64, x1: i64, y1: i64, z1: i64, m: i64) {
```

**Parameters:**

- `c`: `&mut VoxChunk`
- `x0`: `i64`
- `y0`: `i64`
- `z0`: `i64`
- `x1`: `i64`
- `y1`: `i64`
- `z1`: `i64`
- `m`: `i64`

*Defined at line 177*

---

## vox_solid_count

```fides
pub fn vox_solid_count(c: &VoxChunk) -> i64 {
```

**Parameters:**

- `c`: `&VoxChunk`

**Returns:** `i64`

*Defined at line 193*

---

## VoxMesh

```fides
pub struct VoxMesh {
```

**Fields:**

| Name | Type |
|------|------|
| `positions` | `Vec<f64>` |
| `normals` | `Vec<f64>` |
| `uvs` | `Vec<f64>` |
| `ao` | `Vec<i64>` |
| `material` | `Vec<i64>` |
| `indices` | `Vec<i64>` |

*Defined at line 207*

---

## vox_mesh_new

```fides
pub fn vox_mesh_new() -> VoxMesh {
```

**Returns:** `VoxMesh`

*Defined at line 216*

---

## vox_mesh_vertex_count

```fides
pub fn vox_mesh_vertex_count(m: &VoxMesh) -> i64 {
```

**Parameters:**

- `m`: `&VoxMesh`

**Returns:** `i64`

*Defined at line 222*

---

## vox_mesh_triangle_count

```fides
pub fn vox_mesh_triangle_count(m: &VoxMesh) -> i64 {
```

**Parameters:**

- `m`: `&VoxMesh`

**Returns:** `i64`

*Defined at line 226*

---

## vox_ao_value

AO of one face corner: s1, s2 the side voxels, k the diagonal one
(each 0 or 1). 3 = open, 0 = in a corner.

```fides
pub fn vox_ao_value(s1: i64, s2: i64, k: i64) -> i64 {
```

**Parameters:**

- `s1`: `i64`
- `s2`: `i64`
- `k`: `i64`

**Returns:** `i64`

*Defined at line 286*

---

## vox_face_ao_xyz

The 4 AO values of the face of voxel (x, y, z) toward d, packed 2 bits
each, corners (u,v) = (0,0) (1,0) (1,1) (0,1).

```fides
pub fn vox_face_ao_xyz(c: &VoxChunk, d: i64, x: i64, y: i64, z: i64) -> i64 {
```

**Parameters:**

- `c`: `&VoxChunk`
- `d`: `i64`
- `x`: `i64`
- `y`: `i64`
- `z`: `i64`

**Returns:** `i64`

*Defined at line 313*

---

## vox_mesh_naive

One quad per exposed face.

```fides
pub fn vox_mesh_naive(c: &VoxChunk) -> VoxMesh {
```

**Parameters:**

- `c`: `&VoxChunk`

**Returns:** `VoxMesh`

*Defined at line 392*

---

## vox_mesh_greedy

Exposed faces merged into maximal rectangles (same material and AO).

```fides
pub fn vox_mesh_greedy(c: &VoxChunk) -> VoxMesh {
```

**Parameters:**

- `c`: `&VoxChunk`

**Returns:** `VoxMesh`

*Defined at line 425*

---

## vox_mesh_area

Total area of the mesh's triangles.

```fides
pub fn vox_mesh_area(m: &VoxMesh) -> f64 {
```

**Parameters:**

- `m`: `&VoxMesh`

**Returns:** `f64`

*Defined at line 502*

---

## vox_mesh_volume

Signed volume enclosed (divergence theorem); positive when triangles
wind counter-clockwise seen from outside.

```fides
pub fn vox_mesh_volume(m: &VoxMesh) -> f64 {
```

**Parameters:**

- `m`: `&VoxMesh`

**Returns:** `f64`

*Defined at line 526*

---

## vox_field_sample

Sample a field into the (nx+1)(ny+1)(nz+1) grid marching cubes reads:
the value at grid point (x, y, z) is f(x * scale, y * scale, z * scale).

```fides
pub fn vox_field_sample(nx: i64, ny: i64, nz: i64, scale: f64, f: fn(f64, f64, f64) -> f64) -> Vec<f64> {
```

**Parameters:**

- `nx`: `i64`
- `ny`: `i64`
- `nz`: `i64`
- `scale`: `f64`
- `f`: `fn(f64`

**Returns:** `Vec<f64>`

*Defined at line 591*

---

## vox_marching_cubes

Iso-surface of `field` (samples on an (nx+1) x (ny+1) x (nz+1) grid, x
fastest) at `iso`; inside is below iso. Positions are in grid units.
Triangles wind counter-clockwise seen from outside (the side above
iso), and normals (the field's gradient) point outward.

```fides
pub fn vox_marching_cubes(field: &Vec<f64>, nx: i64, ny: i64, nz: i64, iso: f64) -> VoxMesh {
```

**Parameters:**

- `field`: `&Vec<f64>`
- `nx`: `i64`
- `ny`: `i64`
- `nz`: `i64`
- `iso`: `f64`

**Returns:** `VoxMesh`

*Defined at line 613*

---

## vox_mesh_is_closed

Every edge used by exactly two triangles, once in each direction: the
mesh is closed and consistently wound.

```fides
pub fn vox_mesh_is_closed(m: &VoxMesh) -> bool {
```

**Parameters:**

- `m`: `&VoxMesh`

**Returns:** `bool`

*Defined at line 718*

---

## vox_mesh_euler

V - E + F of an indexed triangle mesh (2 for a sphere, 0 for a torus).

```fides
pub fn vox_mesh_euler(m: &VoxMesh) -> i64 {
```

**Parameters:**

- `m`: `&VoxMesh`

**Returns:** `i64`

*Defined at line 749*

---

## vox_mesh_to_obj

Wavefront OBJ text (v, vt, vn and f lines; indices 1-based).

```fides
pub fn vox_mesh_to_obj(m: &VoxMesh) -> str {
```

**Parameters:**

- `m`: `&VoxMesh`

**Returns:** `str`

*Defined at line 798*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
