[All modules](index.md)

# wfc -- Fides Documentation

## Contents

- [WFC_RIGHT:](#WFC_RIGHT:) (const)
- [WFC_DOWN:](#WFC_DOWN:) (const)
- [WFC_LEFT:](#WFC_LEFT:) (const)
- [WFC_UP:](#WFC_UP:) (const)
- [WFC_RUNNING:](#WFC_RUNNING:) (const)
- [WFC_DONE:](#WFC_DONE:) (const)
- [WFC_FAILED:](#WFC_FAILED:) (const)
- [wfc_dx](#wfc_dx) (function)
- [wfc_dy](#wfc_dy) (function)
- [wfc_opposite](#wfc_opposite) (function)
- [wfc_f64s](#wfc_f64s) (function)
- [WfcModel](#WfcModel) (struct)
- [wfc_tiled_new](#wfc_tiled_new) (function)
- [wfc_tiled_weight](#wfc_tiled_weight) (function)
- [wfc_tiled_allow](#wfc_tiled_allow) (function)
- [wfc_tiled_allow_all](#wfc_tiled_allow_all) (function)
- [wfc_allowed](#wfc_allowed) (function)
- [WfcPatterns](#WfcPatterns) (struct)
- [wfc_overlapping](#wfc_overlapping) (function)
- [WfcSolver](#WfcSolver) (struct)
- [wfc_solver_new](#wfc_solver_new) (function)
- [wfc_possible](#wfc_possible) (function)
- [wfc_count](#wfc_count) (function)
- [wfc_set](#wfc_set) (function)
- [wfc_forbid](#wfc_forbid) (function)
- [wfc_entropy](#wfc_entropy) (function)
- [wfc_step](#wfc_step) (function)
- [wfc_run](#wfc_run) (function)
- [wfc_cell](#wfc_cell) (function)
- [wfc_valid](#wfc_valid) (function)
- [wfc_overlapping_image](#wfc_overlapping_image) (function)
- [wfc_image_windows_ok](#wfc_image_windows_ok) (function)

## WFC_RIGHT:

```fides
pub const WFC_RIGHT: i64 = 0;
```

*Defined at line 37*

---

## WFC_DOWN:

```fides
pub const WFC_DOWN: i64 = 1;
```

*Defined at line 38*

---

## WFC_LEFT:

```fides
pub const WFC_LEFT: i64 = 2;
```

*Defined at line 39*

---

## WFC_UP:

```fides
pub const WFC_UP: i64 = 3;
```

*Defined at line 40*

---

## WFC_RUNNING:

```fides
pub const WFC_RUNNING: i64 = 0;
```

*Defined at line 42*

---

## WFC_DONE:

```fides
pub const WFC_DONE: i64 = 1;
```

*Defined at line 43*

---

## WFC_FAILED:

```fides
pub const WFC_FAILED: i64 = 2;
```

*Defined at line 44*

---

## wfc_dx

```fides
pub fn wfc_dx(d: i64) -> i64 {
```

**Parameters:**

- `d`: `i64`

**Returns:** `i64`

*Defined at line 46*

---

## wfc_dy

```fides
pub fn wfc_dy(d: i64) -> i64 {
```

**Parameters:**

- `d`: `i64`

**Returns:** `i64`

*Defined at line 56*

---

## wfc_opposite

```fides
pub fn wfc_opposite(d: i64) -> i64 {
```

**Parameters:**

- `d`: `i64`

**Returns:** `i64`

*Defined at line 66*

---

## wfc_f64s

```fides
pub fn wfc_f64s(n: i64, x: f64) -> Vec<f64> {
```

**Parameters:**

- `n`: `i64`
- `x`: `f64`

**Returns:** `Vec<f64>`

*Defined at line 72*

---

## WfcModel

```fides
pub struct WfcModel {
```

**Fields:**

| Name | Type |
|------|------|
| `t` | `i64` |
| `weights` | `Vec<f64>` |
| `compat` | `Vec<bool>` |

*Defined at line 84*

---

## wfc_tiled_new

A simple tiled model of `t` tiles, weight 1 each, nothing allowed yet.

```fides
pub fn wfc_tiled_new(t: i64) -> WfcModel {
```

**Parameters:**

- `t`: `i64`

**Returns:** `WfcModel`

*Defined at line 92*

---

## wfc_tiled_weight

```fides
pub fn wfc_tiled_weight(m: &mut WfcModel, tile: i64, w: f64) {
```

**Parameters:**

- `m`: `&mut WfcModel`
- `tile`: `i64`
- `w`: `f64`

*Defined at line 98*

---

## wfc_tiled_allow

Allow tile b at direction d of tile a (and a at the opposite of b).

```fides
pub fn wfc_tiled_allow(m: &mut WfcModel, a: i64, d: i64, b: i64) {
```

**Parameters:**

- `m`: `&mut WfcModel`
- `a`: `i64`
- `d`: `i64`
- `b`: `i64`

*Defined at line 103*

---

## wfc_tiled_allow_all

Allow a and b side by side in all four directions.

```fides
pub fn wfc_tiled_allow_all(m: &mut WfcModel, a: i64, b: i64) {
```

**Parameters:**

- `m`: `&mut WfcModel`
- `a`: `i64`
- `b`: `i64`

*Defined at line 109*

---

## wfc_allowed

```fides
pub fn wfc_allowed(m: &WfcModel, a: i64, d: i64, b: i64) -> bool {
```

**Parameters:**

- `m`: `&WfcModel`
- `a`: `i64`
- `d`: `i64`
- `b`: `i64`

**Returns:** `bool`

*Defined at line 117*

---

## WfcPatterns

Patterns of an overlapping model and the model itself.

```fides
pub struct WfcPatterns {
```

**Fields:**

| Name | Type |
|------|------|
| `n` | `i64` |
| `pixels` | `Vec<i64>` |
| `model` | `WfcModel` |

*Defined at line 122*

---

## wfc_overlapping

Extract the N x N patterns of a sw x sh sample. `periodic_input` lets
windows wrap around the sample's edges; `symmetry` is 1 (as is), 2
(plus mirror), 4 (rotations) or 8 (rotations and mirrors).

```fides
pub fn wfc_overlapping(sample: &Vec<i64>, sw: i64, sh: i64, n: i64, periodic_input: bool, symmetry: i64) -> WfcPatterns {
```

**Parameters:**

- `sample`: `&Vec<i64>`
- `sw`: `i64`
- `sh`: `i64`
- `n`: `i64`
- `periodic_input`: `bool`
- `symmetry`: `i64`

**Returns:** `WfcPatterns`

*Defined at line 164*

---

## WfcSolver

```fides
pub struct WfcSolver {
```

**Fields:**

| Name | Type |
|------|------|
| `w` | `i64` |
| `h` | `i64` |
| `periodic` | `bool` |
| `t` | `i64` |
| `wave` | `Vec<bool>` |
| `count` | `Vec<i64>` |
| `sum_w` | `Vec<f64>` |
| `sum_wlogw` | `Vec<f64>` |
| `rng` | `i64` |
| `dec_cell` | `Vec<i64>` |
| `dec_pat` | `Vec<i64>` |
| `saved_wave` | `Vec<Vec<bool>>` |
| `saved_count` | `Vec<Vec<i64>>` |
| `saved_sw` | `Vec<Vec<f64>>` |
| `saved_swl` | `Vec<Vec<f64>>` |
| `status` | `i64` |
| `observations` | `i64` |
| `backtracks` | `i64` |
| `max_backtracks` | `i64` |

*Defined at line 260*

---

## wfc_solver_new

A solver for a w x h grid (periodic: the grid wraps around).

```fides
pub fn wfc_solver_new(m: &WfcModel, w: i64, h: i64, periodic: bool, seed: i64) -> WfcSolver {
```

**Parameters:**

- `m`: `&WfcModel`
- `w`: `i64`
- `h`: `i64`
- `periodic`: `bool`
- `seed`: `i64`

**Returns:** `WfcSolver`

*Defined at line 299*

---

## wfc_possible

```fides
pub fn wfc_possible(s: &WfcSolver, cell: i64, p: i64) -> bool {
```

**Parameters:**

- `s`: `&WfcSolver`
- `cell`: `i64`
- `p`: `i64`

**Returns:** `bool`

*Defined at line 331*

---

## wfc_count

Patterns still possible at a cell.

```fides
pub fn wfc_count(s: &WfcSolver, x: i64, y: i64) -> i64 {
```

**Parameters:**

- `s`: `&WfcSolver`
- `x`: `i64`
- `y`: `i64`

**Returns:** `i64`

*Defined at line 336*

---

## wfc_set

Fix cell (x, y) to pattern p before running (or ban everything else).
Returns false when that contradicts the rules.

```fides
pub fn wfc_set(s: &mut WfcSolver, m: &WfcModel, x: i64, y: i64, p: i64) -> bool {
```

**Parameters:**

- `s`: `&mut WfcSolver`
- `m`: `&WfcModel`
- `x`: `i64`
- `y`: `i64`
- `p`: `i64`

**Returns:** `bool`

*Defined at line 409*

---

## wfc_forbid

Ban pattern p at (x, y) before running.

```fides
pub fn wfc_forbid(s: &mut WfcSolver, m: &WfcModel, x: i64, y: i64, p: i64) -> bool {
```

**Parameters:**

- `s`: `&mut WfcSolver`
- `m`: `&WfcModel`
- `x`: `i64`
- `y`: `i64`
- `p`: `i64`

**Returns:** `bool`

*Defined at line 432*

---

## wfc_entropy

Shannon entropy of a cell's remaining patterns.

```fides
pub fn wfc_entropy(s: &WfcSolver, cell: i64) -> f64 {
```

**Parameters:**

- `s`: `&WfcSolver`
- `cell`: `i64`

**Returns:** `f64`

*Defined at line 445*

---

## wfc_step

One observation (and its propagation, and any backtracking). Returns
the status: WFC_RUNNING, WFC_DONE or WFC_FAILED.

```fides
pub fn wfc_step(s: &mut WfcSolver, m: &WfcModel) -> i64 {
```

**Parameters:**

- `s`: `&mut WfcSolver`
- `m`: `&WfcModel`

**Returns:** `i64`

*Defined at line 475*

---

## wfc_run

Run to the end; returns WFC_DONE or WFC_FAILED.

```fides
pub fn wfc_run(s: &mut WfcSolver, m: &WfcModel) -> i64 {
```

**Parameters:**

- `s`: `&mut WfcSolver`
- `m`: `&WfcModel`

**Returns:** `i64`

*Defined at line 542*

---

## wfc_cell

The pattern at (x, y) once decided, else -1.

```fides
pub fn wfc_cell(s: &WfcSolver, x: i64, y: i64) -> i64 {
```

**Parameters:**

- `s`: `&WfcSolver`
- `x`: `i64`
- `y`: `i64`

**Returns:** `i64`

*Defined at line 549*

---

## wfc_valid

Does every pair of neighbouring cells follow the rules?

```fides
pub fn wfc_valid(s: &WfcSolver, m: &WfcModel) -> bool {
```

**Parameters:**

- `s`: `&WfcSolver`
- `m`: `&WfcModel`

**Returns:** `bool`

*Defined at line 565*

---

## wfc_overlapping_image

Output image of an overlapping model solved on a (w-n+1) x (h-n+1)
non-periodic grid: w x h colours.

```fides
pub fn wfc_overlapping_image(s: &WfcSolver, p: &WfcPatterns) -> Vec<i64> {
```

**Parameters:**

- `s`: `&WfcSolver`
- `p`: `&WfcPatterns`

**Returns:** `Vec<i64>`

*Defined at line 590*

---

## wfc_image_windows_ok

Is every n x n window of a w x h image one of the patterns?

```fides
pub fn wfc_image_windows_ok(img: &Vec<i64>, w: i64, h: i64, p: &WfcPatterns) -> bool {
```

**Parameters:**

- `img`: `&Vec<i64>`
- `w`: `i64`
- `h`: `i64`
- `p`: `&WfcPatterns`

**Returns:** `bool`

*Defined at line 621*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
