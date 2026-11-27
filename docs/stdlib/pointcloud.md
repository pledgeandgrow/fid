[All modules](index.md)

# pointcloud -- Fides Documentation

## Contents

- [pcl_centroid](#pcl_centroid) (function)
- [pcl_bounds](#pcl_bounds) (function)
- [pcl_dist2](#pcl_dist2) (function)
- [pcl_voxel_downsample](#pcl_voxel_downsample) (function)
- [KdTree](#KdTree) (struct)
- [kdtree_build](#kdtree_build) (function)
- [kdtree_len](#kdtree_len) (function)
- [kdtree_knn](#kdtree_knn) (function)
- [kdtree_nearest](#kdtree_nearest) (function)
- [kdtree_radius](#kdtree_radius) (function)
- [EigenSym](#EigenSym) (struct)
- [pcl_eigen_sym](#pcl_eigen_sym) (function)
- [pcl_eigvec3](#pcl_eigvec3) (function)
- [pcl_covariance](#pcl_covariance) (function)
- [pcl_normals](#pcl_normals) (function)
- [pcl_fit_rigid](#pcl_fit_rigid) (function)
- [IcpResult](#IcpResult) (struct)
- [pcl_icp](#pcl_icp) (function)

## pcl_centroid

The centroid of a cloud ((0, 0, 0) for an empty one).

```fides
pub fn pcl_centroid(pts: &Vec<vec3>) -> vec3 {
```

**Parameters:**

- `pts`: `&Vec<vec3>`

**Returns:** `vec3`

*Defined at line 43*

---

## pcl_bounds

Componentwise minimum and maximum (two points).

```fides
pub fn pcl_bounds(pts: &Vec<vec3>) -> Vec<vec3> {
```

**Parameters:**

- `pts`: `&Vec<vec3>`

**Returns:** `Vec<vec3>`

*Defined at line 57*

---

## pcl_dist2

Squared distance.

```fides
pub fn pcl_dist2(a: vec3, b: vec3) -> f64 {
```

**Parameters:**

- `a`: `vec3`
- `b`: `vec3`

**Returns:** `f64`

*Defined at line 74*

---

## pcl_voxel_downsample

One point per voxel of edge `size`: the centroid of the points in it.
Voxel indices must stay within +-2^20 (size * 1e6 around the origin).

```fides
pub fn pcl_voxel_downsample(pts: &Vec<vec3>, size: f64) -> Vec<vec3> {
```

**Parameters:**

- `pts`: `&Vec<vec3>`
- `size`: `f64`

**Returns:** `Vec<vec3>`

*Defined at line 95*

---

## KdTree

A 3-d tree over a copy of the points; node n splits on axis[n] at
point idx[n].

```fides
pub struct KdTree {
```

**Fields:**

| Name | Type |
|------|------|
| `pts` | `Vec<vec3>` |
| `idx` | `Vec<i64>` |
| `axis` | `Vec<i64>` |
| `left` | `Vec<i64>` |
| `right` | `Vec<i64>` |
| `root` | `i64` |

*Defined at line 130*

---

## kdtree_build

A k-d tree over (a copy of) pts.

```fides
pub fn kdtree_build(pts: &Vec<vec3>) -> KdTree {
```

**Parameters:**

- `pts`: `&Vec<vec3>`

**Returns:** `KdTree`

*Defined at line 218*

---

## kdtree_len

The number of points in the tree.

```fides
pub fn kdtree_len(t: &KdTree) -> i64 {
```

**Parameters:**

- `t`: `&KdTree`

**Returns:** `i64`

*Defined at line 234*

---

## kdtree_knn

The indices (into the original points) of the k nearest neighbours of
q, nearest first (ties by index).

```fides
pub fn kdtree_knn(t: &KdTree, q: vec3, k: i64) -> Vec<i64> {
```

**Parameters:**

- `t`: `&KdTree`
- `q`: `vec3`
- `k`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 279*

---

## kdtree_nearest

The index of the point nearest q (-1 for an empty tree).

```fides
pub fn kdtree_nearest(t: &KdTree, q: vec3) -> i64 {
```

**Parameters:**

- `t`: `&KdTree`
- `q`: `vec3`

**Returns:** `i64`

*Defined at line 290*

---

## kdtree_radius

The indices of all points within distance r of q, in increasing index
order.

```fides
pub fn kdtree_radius(t: &KdTree, q: vec3, r: f64) -> Vec<i64> {
```

**Parameters:**

- `t`: `&KdTree`
- `q`: `vec3`
- `r`: `f64`

**Returns:** `Vec<i64>`

*Defined at line 319*

---

## EigenSym

Eigenvalues (ascending) and unit eigenvectors (vectors[i * n + c] is
component i of eigenvector c).

```fides
pub struct EigenSym {
```

**Fields:**

| Name | Type |
|------|------|
| `n` | `i64` |
| `values` | `Vec<f64>` |
| `vectors` | `Vec<f64>` |

*Defined at line 330*

---

## pcl_eigen_sym

The eigen decomposition of the symmetric n x n matrix `a` (row-major),
by cyclic Jacobi rotations until the off-diagonal part is negligible.

```fides
pub fn pcl_eigen_sym(a0: &Vec<f64>, n: i64) -> EigenSym {
```

**Parameters:**

- `a0`: `&Vec<f64>`
- `n`: `i64`

**Returns:** `EigenSym`

*Defined at line 338*

---

## pcl_eigvec3

Eigenvector c of e as a vec3 (n = 3).

```fides
pub fn pcl_eigvec3(e: &EigenSym, c: i64) -> vec3 {
```

**Parameters:**

- `e`: `&EigenSym`
- `c`: `i64`

**Returns:** `vec3`

*Defined at line 454*

---

## pcl_covariance

The 3x3 covariance (row-major) of the points pts[ids].

```fides
pub fn pcl_covariance(pts: &Vec<vec3>, ids: &Vec<i64>) -> Vec<f64> {
```

**Parameters:**

- `pts`: `&Vec<vec3>`
- `ids`: `&Vec<i64>`

**Returns:** `Vec<f64>`

*Defined at line 459*

---

## pcl_normals

A unit normal per point from its k nearest neighbours (PCA), pointing to
the side of `viewpoint`.

```fides
pub fn pcl_normals(pts: &Vec<vec3>, k: i64, viewpoint: vec3) -> Vec<vec3> {
```

**Parameters:**

- `pts`: `&Vec<vec3>`
- `k`: `i64`
- `viewpoint`: `vec3`

**Returns:** `Vec<vec3>`

*Defined at line 498*

---

## pcl_fit_rigid

The best rigid motion (least squares) taking src[k] onto dst[k], by
Horn's quaternion method: the eigenvector of the largest eigenvalue of
his 4x4 matrix built from the cross-covariance.

```fides
pub fn pcl_fit_rigid(src: &Vec<vec3>, dst: &Vec<vec3>) -> Transform3 {
```

**Parameters:**

- `src`: `&Vec<vec3>`
- `dst`: `&Vec<vec3>`

**Returns:** `Transform3`

*Defined at line 522*

---

## IcpResult

The result of `pcl_icp`.

```fides
pub struct IcpResult {
```

**Fields:**

| Name | Type |
|------|------|
| `transform` | `Transform3` |
| `rmse` | `f64` |
| `pairs` | `i64` |
| `iterations` | `i64` |
| `converged` | `bool` |

*Defined at line 575*

---

## pcl_icp

Point-to-point ICP aligning `source` to `target` from the guess
`initial`: pairs farther apart than max_dist are ignored; stops when an
iteration moves the source by less than `tol` (translation and rotation
angle) or after max_iter iterations.

```fides
pub fn pcl_icp(source: &Vec<vec3>, target: &Vec<vec3>, initial: Transform3, max_iter: i64, max_dist: f64, tol: f64) -> IcpResult {
```

**Parameters:**

- `source`: `&Vec<vec3>`
- `target`: `&Vec<vec3>`
- `initial`: `Transform3`
- `max_iter`: `i64`
- `max_dist`: `f64`
- `tol`: `f64`

**Returns:** `IcpResult`

*Defined at line 589*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
