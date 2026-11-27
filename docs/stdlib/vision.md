[All modules](index.md)

# vision -- Fides Documentation

## Contents

- [GrayImg](#GrayImg) (struct)
- [FImg](#FImg) (struct)
- [vis_gray_new](#vis_gray_new) (function)
- [vis_fimg_new](#vis_fimg_new) (function)
- [vis_gray_from_bytes](#vis_gray_from_bytes) (function)
- [vis_get](#vis_get) (function)
- [vis_set](#vis_set) (function)
- [vis_fget](#vis_fget) (function)
- [vis_from_rgb](#vis_from_rgb) (function)
- [vis_to_float](#vis_to_float) (function)
- [vis_to_gray](#vis_to_gray) (function)
- [vis_bilinear](#vis_bilinear) (function)
- [vis_convolve](#vis_convolve) (function)
- [vis_convolve_sep](#vis_convolve_sep) (function)
- [vis_gaussian_kernel](#vis_gaussian_kernel) (function)
- [vis_gaussian_blur](#vis_gaussian_blur) (function)
- [Gradient](#Gradient) (struct)
- [vis_sobel](#vis_sobel) (function)
- [vis_grad_mag](#vis_grad_mag) (function)
- [vis_canny](#vis_canny) (function)
- [vis_histogram](#vis_histogram) (function)
- [vis_otsu](#vis_otsu) (function)
- [vis_threshold](#vis_threshold) (function)
- [vis_count_nonzero](#vis_count_nonzero) (function)
- [vis_erode](#vis_erode) (function)
- [vis_dilate](#vis_dilate) (function)
- [vis_open](#vis_open) (function)
- [vis_close](#vis_close) (function)
- [Components](#Components) (struct)
- [vis_components](#vis_components) (function)
- [Keypoint](#Keypoint) (struct)
- [vis_fast_score](#vis_fast_score) (function)
- [vis_fast](#vis_fast) (function)
- [vis_orientation](#vis_orientation) (function)
- [vis_orb](#vis_orb) (function)
- [vis_hamming](#vis_hamming) (function)
- [Match](#Match) (struct)
- [vis_match](#vis_match) (function)
- [Camera](#Camera) (struct)
- [cam_pinhole](#cam_pinhole) (function)
- [cam_set_distortion](#cam_set_distortion) (function)
- [cam_distort](#cam_distort) (function)
- [cam_undistort](#cam_undistort) (function)
- [cam_project](#cam_project) (function)
- [cam_pixel_to_norm](#cam_pixel_to_norm) (function)
- [vis_undistort](#vis_undistort) (function)
- [Homography](#Homography) (struct)
- [vis_solve](#vis_solve) (function)
- [vis_homography](#vis_homography) (function)
- [vis_hom_apply](#vis_hom_apply) (function)
- [vis_mat3_inverse](#vis_mat3_inverse) (function)

## GrayImg

```fides
pub struct GrayImg {
```

**Fields:**

| Name | Type |
|------|------|
| `w` | `i64` |
| `h` | `i64` |
| `data` | `Vec<i64>` |

*Defined at line 51*

---

## FImg

```fides
pub struct FImg {
```

**Fields:**

| Name | Type |
|------|------|
| `w` | `i64` |
| `h` | `i64` |
| `data` | `Vec<f64>` |

*Defined at line 57*

---

## vis_gray_new

```fides
pub fn vis_gray_new(w: i64, h: i64, v: i64) -> GrayImg {
```

**Parameters:**

- `w`: `i64`
- `h`: `i64`
- `v`: `i64`

**Returns:** `GrayImg`

*Defined at line 63*

---

## vis_fimg_new

```fides
pub fn vis_fimg_new(w: i64, h: i64) -> FImg {
```

**Parameters:**

- `w`: `i64`
- `h`: `i64`

**Returns:** `FImg`

*Defined at line 75*

---

## vis_gray_from_bytes

A gray image over existing bytes (w * h of them).

```fides
pub fn vis_gray_from_bytes(w: i64, h: i64, bytes: Vec<i64>) -> GrayImg {
```

**Parameters:**

- `w`: `i64`
- `h`: `i64`
- `bytes`: `Vec<i64>`

**Returns:** `GrayImg`

*Defined at line 88*

---

## vis_get

Pixel (x, y), coordinates clamped to the image (replicated border).

```fides
pub fn vis_get(g: &GrayImg, x: i64, y: i64) -> i64 {
```

**Parameters:**

- `g`: `&GrayImg`
- `x`: `i64`
- `y`: `i64`

**Returns:** `i64`

*Defined at line 105*

---

## vis_set

```fides
pub fn vis_set(g: &mut GrayImg, x: i64, y: i64, v: i64) {
```

**Parameters:**

- `g`: `&mut GrayImg`
- `x`: `i64`
- `y`: `i64`
- `v`: `i64`

*Defined at line 109*

---

## vis_fget

```fides
pub fn vis_fget(f: &FImg, x: i64, y: i64) -> f64 {
```

**Parameters:**

- `f`: `&FImg`
- `x`: `i64`
- `y`: `i64`

**Returns:** `f64`

*Defined at line 115*

---

## vis_from_rgb

Gray from interleaved RGB (channels 3) or RGBA (4) bytes: BT.601 luma,
rounded.

```fides
pub fn vis_from_rgb(w: i64, h: i64, channels: i64, data: &Vec<i64>) -> GrayImg {
```

**Parameters:**

- `w`: `i64`
- `h`: `i64`
- `channels`: `i64`
- `data`: `&Vec<i64>`

**Returns:** `GrayImg`

*Defined at line 121*

---

## vis_to_float

```fides
pub fn vis_to_float(g: &GrayImg) -> FImg {
```

**Parameters:**

- `g`: `&GrayImg`

**Returns:** `FImg`

*Defined at line 136*

---

## vis_to_gray

Round and clamp to 0..255.

```fides
pub fn vis_to_gray(f: &FImg) -> GrayImg {
```

**Parameters:**

- `f`: `&FImg`

**Returns:** `GrayImg`

*Defined at line 149*

---

## vis_bilinear

Bilinear sample at (x, y) in pixel coordinates (pixel centres at
integers), border replicated.

```fides
pub fn vis_bilinear(f: &FImg, x: f64, y: f64) -> f64 {
```

**Parameters:**

- `f`: `&FImg`
- `x`: `f64`
- `y`: `f64`

**Returns:** `f64`

*Defined at line 163*

---

## vis_convolve

Convolve with a kw x kh kernel (row-major, centred), replicated border.
(Correlation; for the symmetric kernels used here it is the same.)

```fides
pub fn vis_convolve(f: &FImg, k: &Vec<f64>, kw: i64, kh: i64) -> FImg {
```

**Parameters:**

- `f`: `&FImg`
- `k`: `&Vec<f64>`
- `kw`: `i64`
- `kh`: `i64`

**Returns:** `FImg`

*Defined at line 177*

---

## vis_convolve_sep

Convolve rows with kx, then columns with ky (odd lengths).

```fides
pub fn vis_convolve_sep(f: &FImg, kx: &Vec<f64>, ky: &Vec<f64>) -> FImg {
```

**Parameters:**

- `f`: `&FImg`
- `kx`: `&Vec<f64>`
- `ky`: `&Vec<f64>`

**Returns:** `FImg`

*Defined at line 204*

---

## vis_gaussian_kernel

A normalized 1-D Gaussian of radius ceil(3 sigma).

```fides
pub fn vis_gaussian_kernel(sigma: f64) -> Vec<f64> {
```

**Parameters:**

- `sigma`: `f64`

**Returns:** `Vec<f64>`

*Defined at line 210*

---

## vis_gaussian_blur

```fides
pub fn vis_gaussian_blur(f: &FImg, sigma: f64) -> FImg {
```

**Parameters:**

- `f`: `&FImg`
- `sigma`: `f64`

**Returns:** `FImg`

*Defined at line 233*

---

## Gradient

```fides
pub struct Gradient {
```

**Fields:**

| Name | Type |
|------|------|
| `gx` | `FImg` |
| `gy` | `FImg` |

*Defined at line 240*

---

## vis_sobel

Sobel derivatives (x to the right, y down).

```fides
pub fn vis_sobel(f: &FImg) -> Gradient {
```

**Parameters:**

- `f`: `&FImg`

**Returns:** `Gradient`

*Defined at line 246*

---

## vis_grad_mag

Gradient magnitude sqrt(gx^2 + gy^2).

```fides
pub fn vis_grad_mag(gr: &Gradient) -> FImg {
```

**Parameters:**

- `gr`: `&Gradient`

**Returns:** `FImg`

*Defined at line 273*

---

## vis_canny

Canny edges (255 on edges, 0 elsewhere) with thresholds on the Sobel
magnitude.

```fides
pub fn vis_canny(g: &GrayImg, sigma: f64, low: f64, high: f64) -> GrayImg {
```

**Parameters:**

- `g`: `&GrayImg`
- `sigma`: `f64`
- `low`: `f64`
- `high`: `f64`

**Returns:** `GrayImg`

*Defined at line 287*

---

## vis_histogram

```fides
pub fn vis_histogram(g: &GrayImg) -> Vec<i64> {
```

**Parameters:**

- `g`: `&GrayImg`

**Returns:** `Vec<i64>`

*Defined at line 381*

---

## vis_otsu

Otsu's threshold: the t maximizing the between-class variance of
{<= t} and {> t}.

```fides
pub fn vis_otsu(g: &GrayImg) -> i64 {
```

**Parameters:**

- `g`: `&GrayImg`

**Returns:** `i64`

*Defined at line 399*

---

## vis_threshold

255 where the pixel is > t, 0 elsewhere.

```fides
pub fn vis_threshold(g: &GrayImg, t: i64) -> GrayImg {
```

**Parameters:**

- `g`: `&GrayImg`
- `t`: `i64`

**Returns:** `GrayImg`

*Defined at line 432*

---

## vis_count_nonzero

The number of pixels > 0.

```fides
pub fn vis_count_nonzero(g: &GrayImg) -> i64 {
```

**Parameters:**

- `g`: `&GrayImg`

**Returns:** `i64`

*Defined at line 445*

---

## vis_erode

Minimum over a (2r+1)^2 square (shrinks bright regions).

```fides
pub fn vis_erode(g: &GrayImg, r: i64) -> GrayImg {
```

**Parameters:**

- `g`: `&GrayImg`
- `r`: `i64`

**Returns:** `GrayImg`

*Defined at line 493*

---

## vis_dilate

Maximum over a (2r+1)^2 square (grows bright regions).

```fides
pub fn vis_dilate(g: &GrayImg, r: i64) -> GrayImg {
```

**Parameters:**

- `g`: `&GrayImg`
- `r`: `i64`

**Returns:** `GrayImg`

*Defined at line 498*

---

## vis_open

```fides
pub fn vis_open(g: &GrayImg, r: i64) -> GrayImg {
```

**Parameters:**

- `g`: `&GrayImg`
- `r`: `i64`

**Returns:** `GrayImg`

*Defined at line 502*

---

## vis_close

```fides
pub fn vis_close(g: &GrayImg, r: i64) -> GrayImg {
```

**Parameters:**

- `g`: `&GrayImg`
- `r`: `i64`

**Returns:** `GrayImg`

*Defined at line 507*

---

## Components

```fides
pub struct Components {
```

**Fields:**

| Name | Type |
|------|------|
| `labels` | `Vec<i64>` |
| `count` | `i64` |
| `area` | `Vec<i64>` |
| `min_x` | `Vec<i64>` |
| `min_y` | `Vec<i64>` |
| `max_x` | `Vec<i64>` |
| `max_y` | `Vec<i64>` |
| `cx` | `Vec<f64>` |
| `cy` | `Vec<f64>` |

*Defined at line 514*

---

## vis_components

Label the pixels > 0 (labels in raster order of each component's first
pixel).

```fides
pub fn vis_components(g: &GrayImg, eight: bool) -> Components {
```

**Parameters:**

- `g`: `&GrayImg`
- `eight`: `bool`

**Returns:** `Components`

*Defined at line 530*

---

## Keypoint

```fides
pub struct Keypoint {
```

**Fields:**

| Name | Type |
|------|------|
| `x` | `i64` |
| `y` | `i64` |
| `score` | `f64` |
| `angle` | `f64` |

*Defined at line 613*

---

## vis_fast_score

The FAST-9 score of pixel (x, y) at threshold t, or 0 when it is not a
corner.

```fides
pub fn vis_fast_score(g: &GrayImg, x: i64, y: i64, t: i64) -> f64 {
```

**Parameters:**

- `g`: `&GrayImg`
- `x`: `i64`
- `y`: `i64`
- `t`: `i64`

**Returns:** `f64`

*Defined at line 664*

---

## vis_fast

FAST-9 corners (3 pixels from the border or more), with 3x3
non-maximum suppression when `nms`.

```fides
pub fn vis_fast(g: &GrayImg, t: i64, nms: bool) -> Vec<Keypoint> {
```

**Parameters:**

- `g`: `&GrayImg`
- `t`: `i64`
- `nms`: `bool`

**Returns:** `Vec<Keypoint>`

*Defined at line 716*

---

## vis_orientation

The intensity-centroid orientation of a radius-r disc around (x, y).

```fides
pub fn vis_orientation(f: &FImg, x: i64, y: i64, r: i64) -> f64 {
```

**Parameters:**

- `f`: `&FImg`
- `x`: `i64`
- `y`: `i64`
- `r`: `i64`

**Returns:** `f64`

*Defined at line 773*

---

## vis_orb

Orientation of each keypoint (in place) and its 256-bit descriptor as
8 words of 32 bits. Keypoints closer than 20 pixels to the border get
an empty descriptor.

```fides
pub fn vis_orb(g: &GrayImg, kps: &mut Vec<Keypoint>) -> Vec<Vec<i64>> {
```

**Parameters:**

- `g`: `&GrayImg`
- `kps`: `&mut Vec<Keypoint>`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 823*

---

## vis_hamming

Hamming distance of two descriptors (256 when either is empty).

```fides
pub fn vis_hamming(a: &Vec<i64>, b: &Vec<i64>) -> i64 {
```

**Parameters:**

- `a`: `&Vec<i64>`
- `b`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 883*

---

## Match

```fides
pub struct Match {
```

**Fields:**

| Name | Type |
|------|------|
| `i` | `i64` |
| `j` | `i64` |
| `dist` | `i64` |

*Defined at line 896*

---

## vis_match

Brute-force matches a -> b with a cross check (each is the other's
nearest) and a maximum distance.

```fides
pub fn vis_match(a: &Vec<Vec<i64>>, b: &Vec<Vec<i64>>, max_dist: i64) -> Vec<Match> {
```

**Parameters:**

- `a`: `&Vec<Vec<i64>>`
- `b`: `&Vec<Vec<i64>>`
- `max_dist`: `i64`

**Returns:** `Vec<Match>`

*Defined at line 919*

---

## Camera

```fides
pub struct Camera {
```

**Fields:**

| Name | Type |
|------|------|
| `fx` | `f64` |
| `fy` | `f64` |
| `cx` | `f64` |
| `cy` | `f64` |
| `k1` | `f64` |
| `k2` | `f64` |
| `p1` | `f64` |
| `p2` | `f64` |
| `k3` | `f64` |

*Defined at line 943*

---

## cam_pinhole

```fides
pub fn cam_pinhole(fx: f64, fy: f64, cx: f64, cy: f64) -> Camera {
```

**Parameters:**

- `fx`: `f64`
- `fy`: `f64`
- `cx`: `f64`
- `cy`: `f64`

**Returns:** `Camera`

*Defined at line 955*

---

## cam_set_distortion

```fides
pub fn cam_set_distortion(c: &mut Camera, k1: f64, k2: f64, p1: f64, p2: f64, k3: f64) {
```

**Parameters:**

- `c`: `&mut Camera`
- `k1`: `f64`
- `k2`: `f64`
- `p1`: `f64`
- `p2`: `f64`
- `k3`: `f64`

*Defined at line 961*

---

## cam_distort

Distort normalized image coordinates.

```fides
pub fn cam_distort(c: &Camera, x: f64, y: f64) -> vec2 {
```

**Parameters:**

- `c`: `&Camera`
- `x`: `f64`
- `y`: `f64`

**Returns:** `vec2`

*Defined at line 970*

---

## cam_undistort

Undistort normalized coordinates (fixed-point iteration).

```fides
pub fn cam_undistort(c: &Camera, xd: f64, yd: f64) -> vec2 {
```

**Parameters:**

- `c`: `&Camera`
- `xd`: `f64`
- `yd`: `f64`

**Returns:** `vec2`

*Defined at line 977*

---

## cam_project

The pixel of a camera-frame point (z > 0), distortion applied.

```fides
pub fn cam_project(c: &Camera, p: vec3) -> vec2 {
```

**Parameters:**

- `c`: `&Camera`
- `p`: `vec3`

**Returns:** `vec2`

*Defined at line 1000*

---

## cam_pixel_to_norm

The normalized (undistorted) coordinates of a pixel.

```fides
pub fn cam_pixel_to_norm(c: &Camera, u: f64, v: f64) -> vec2 {
```

**Parameters:**

- `c`: `&Camera`
- `u`: `f64`
- `v`: `f64`

**Returns:** `vec2`

*Defined at line 1006*

---

## vis_undistort

The image the same camera would see without distortion: each output
pixel is sampled (bilinear) where the distorted camera saw it.

```fides
pub fn vis_undistort(g: &GrayImg, c: &Camera) -> GrayImg {
```

**Parameters:**

- `g`: `&GrayImg`
- `c`: `&Camera`

**Returns:** `GrayImg`

*Defined at line 1012*

---

## Homography

```fides
pub struct Homography {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `h` | `Vec<f64>` |

*Defined at line 1030*

---

## vis_solve

```fides
pub fn vis_solve(a: &Vec<f64>, b: &Vec<f64>, n: i64) -> Vec<f64> {
```

**Parameters:**

- `a`: `&Vec<f64>`
- `b`: `&Vec<f64>`
- `n`: `i64`

**Returns:** `Vec<f64>`

*Defined at line 1038*

---

## vis_homography

The homography mapping (sx, sy) to (dx, dy), from 4 or more points.

```fides
pub fn vis_homography(sx: &Vec<f64>, sy: &Vec<f64>, dx: &Vec<f64>, dy: &Vec<f64>) -> Homography {
```

**Parameters:**

- `sx`: `&Vec<f64>`
- `sy`: `&Vec<f64>`
- `dx`: `&Vec<f64>`
- `dy`: `&Vec<f64>`

**Returns:** `Homography`

*Defined at line 1130*

---

## vis_hom_apply

H applied to (x, y).

```fides
pub fn vis_hom_apply(h: &Vec<f64>, x: f64, y: f64) -> vec2 {
```

**Parameters:**

- `h`: `&Vec<f64>`
- `x`: `f64`
- `y`: `f64`

**Returns:** `vec2`

*Defined at line 1259*

---

## vis_mat3_inverse

The inverse of a 3x3 matrix (empty when singular), scaled so [8] = 1
when possible.

```fides
pub fn vis_mat3_inverse(m: &Vec<f64>) -> Vec<f64> {
```

**Parameters:**

- `m`: `&Vec<f64>`

**Returns:** `Vec<f64>`

*Defined at line 1266*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
