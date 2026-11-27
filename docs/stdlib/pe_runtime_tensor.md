[All modules](index.md)

# tensor -- Fides Documentation

## Contents

- [pert_tensor_zeros](#pert_tensor_zeros) (function)
- [pert_tensor_ones](#pert_tensor_ones) (function)
- [pert_tensor_filled](#pert_tensor_filled) (function)
- [pert_tensor_from_vec](#pert_tensor_from_vec) (function)
- [pert_tensor_clone](#pert_tensor_clone) (function)
- [pert_tensor_get](#pert_tensor_get) (function)
- [pert_tensor_set](#pert_tensor_set) (function)
- [pert_tensor_shape](#pert_tensor_shape) (function)
- [pert_tensor_ndim](#pert_tensor_ndim) (function)
- [pert_tensor_size](#pert_tensor_size) (function)
- [pert_tensor_len](#pert_tensor_len) (function)
- [pert_tensor_reshape](#pert_tensor_reshape) (function)
- [pert_tensor_transpose](#pert_tensor_transpose) (function)
- [pert_tensor_add](#pert_tensor_add) (function)
- [pert_tensor_sub](#pert_tensor_sub) (function)
- [pert_tensor_mul](#pert_tensor_mul) (function)
- [pert_tensor_scale](#pert_tensor_scale) (function)
- [pert_tensor_matmul](#pert_tensor_matmul) (function)
- [pert_tensor_dot](#pert_tensor_dot) (function)
- [pert_tensor_sum](#pert_tensor_sum) (function)
- [pert_tensor_mean](#pert_tensor_mean) (function)
- [pert_tensor_max](#pert_tensor_max) (function)
- [pert_tensor_min](#pert_tensor_min) (function)
- [pert_tensor_relu](#pert_tensor_relu) (function)
- [pert_tensor_sigmoid](#pert_tensor_sigmoid) (function)
- [pert_tensor_softmax](#pert_tensor_softmax) (function)
- [pert_tensor_to_str](#pert_tensor_to_str) (function)
- [pert_tensor_to_json](#pert_tensor_to_json) (function)
- [pert_autograd_begin](#pert_autograd_begin) (function)
- [pert_autograd_end](#pert_autograd_end) (function)
- [pert_autograd_backward](#pert_autograd_backward) (function)
- [pert_grad_get](#pert_grad_get) (function)
- [pert_grad_zero](#pert_grad_zero) (function)
- [pert_checkpoint_begin](#pert_checkpoint_begin) (function)
- [pert_checkpoint_end](#pert_checkpoint_end) (function)
- [pert_dp_budget_reset](#pert_dp_budget_reset) (function)
- [pert_dp_budget_check](#pert_dp_budget_check) (function)
- [pert_dp_noise](#pert_dp_noise) (function)
- [pert_gaussian_noise](#pert_gaussian_noise) (function)
- [pert_dropout](#pert_dropout) (function)
- [pert_tp_init](#pert_tp_init) (function)
- [pert_tp_rank](#pert_tp_rank) (function)
- [pert_tp_world_size](#pert_tp_world_size) (function)
- [pert_tp_exit](#pert_tp_exit) (function)
- [pert_all_reduce](#pert_all_reduce) (function)
- [pert_broadcast](#pert_broadcast) (function)
- [pert_all_gather](#pert_all_gather) (function)
- [pert_scatter](#pert_scatter) (function)
- [pert_kv_cache_new](#pert_kv_cache_new) (function)
- [pert_kv_cache_append](#pert_kv_cache_append) (function)
- [pert_kv_cache_get](#pert_kv_cache_get) (function)
- [pert_kv_cache_len](#pert_kv_cache_len) (function)
- [pert_kv_cache_clear](#pert_kv_cache_clear) (function)
- [pert_kv_cache_evict](#pert_kv_cache_evict) (function)
- [pert_quantize_q8](#pert_quantize_q8) (function)
- [pert_dequantize_q8](#pert_dequantize_q8) (function)
- [pert_qmatmul](#pert_qmatmul) (function)
- [pert_find_quant_params](#pert_find_quant_params) (function)
- [pert_send_tensor](#pert_send_tensor) (function)
- [pert_recv_tensor](#pert_recv_tensor) (function)

## pert_tensor_zeros

```fides
pub fn pert_tensor_zeros(shape: Vec<i64>) -> i64 {
```

**Parameters:**

- `shape`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 481*

---

## pert_tensor_ones

```fides
pub fn pert_tensor_ones(shape: Vec<i64>) -> i64 {
```

**Parameters:**

- `shape`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 485*

---

## pert_tensor_filled

```fides
pub fn pert_tensor_filled(shape: Vec<i64>, v: f64) -> i64 {
```

**Parameters:**

- `shape`: `Vec<i64>`
- `v`: `f64`

**Returns:** `i64`

*Defined at line 489*

---

## pert_tensor_from_vec

The data's integers as floats, with the given shape.

```fides
pub fn pert_tensor_from_vec(data: Vec<i64>, shape: Vec<i64>) -> i64 {
```

**Parameters:**

- `data`: `Vec<i64>`
- `shape`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 494*

---

## pert_tensor_clone

```fides
pub fn pert_tensor_clone(t: i64) -> i64 {
```

**Parameters:**

- `t`: `i64`

**Returns:** `i64`

*Defined at line 512*

---

## pert_tensor_get

```fides
pub fn pert_tensor_get(t: i64, idx: Vec<i64>) -> f64 {
```

**Parameters:**

- `t`: `i64`
- `idx`: `Vec<i64>`

**Returns:** `f64`

*Defined at line 516*

---

## pert_tensor_set

```fides
pub fn pert_tensor_set(t: i64, idx: Vec<i64>, v: f64) {
```

**Parameters:**

- `t`: `i64`
- `idx`: `Vec<i64>`
- `v`: `f64`

*Defined at line 524*

---

## pert_tensor_shape

```fides
pub fn pert_tensor_shape(t: i64) -> Vec<i64> {
```

**Parameters:**

- `t`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 531*

---

## pert_tensor_ndim

```fides
pub fn pert_tensor_ndim(t: i64) -> i64 {
```

**Parameters:**

- `t`: `i64`

**Returns:** `i64`

*Defined at line 541*

---

## pert_tensor_size

```fides
pub fn pert_tensor_size(t: i64) -> i64 {
```

**Parameters:**

- `t`: `i64`

**Returns:** `i64`

*Defined at line 545*

---

## pert_tensor_len

```fides
pub fn pert_tensor_len(t: i64) -> i64 {
```

**Parameters:**

- `t`: `i64`

**Returns:** `i64`

*Defined at line 549*

---

## pert_tensor_reshape

```fides
pub fn pert_tensor_reshape(t: i64, shape: Vec<i64>) -> i64 {
```

**Parameters:**

- `t`: `i64`
- `shape`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 553*

---

## pert_tensor_transpose

All axes reversed.

```fides
pub fn pert_tensor_transpose(t: i64) -> i64 {
```

**Parameters:**

- `t`: `i64`

**Returns:** `i64`

*Defined at line 578*

---

## pert_tensor_add

```fides
pub fn pert_tensor_add(a: i64, b: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 620*

---

## pert_tensor_sub

```fides
pub fn pert_tensor_sub(a: i64, b: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 624*

---

## pert_tensor_mul

```fides
pub fn pert_tensor_mul(a: i64, b: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 628*

---

## pert_tensor_scale

```fides
pub fn pert_tensor_scale(t: i64, s: f64) -> i64 {
```

**Parameters:**

- `t`: `i64`
- `s`: `f64`

**Returns:** `i64`

*Defined at line 632*

---

## pert_tensor_matmul

```fides
pub fn pert_tensor_matmul(a: i64, b: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 644*

---

## pert_tensor_dot

```fides
pub fn pert_tensor_dot(a: i64, b: i64) -> f64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `f64`

*Defined at line 678*

---

## pert_tensor_sum

```fides
pub fn pert_tensor_sum(t: i64) -> f64 {
```

**Parameters:**

- `t`: `i64`

**Returns:** `f64`

*Defined at line 712*

---

## pert_tensor_mean

```fides
pub fn pert_tensor_mean(t: i64) -> f64 {
```

**Parameters:**

- `t`: `i64`

**Returns:** `f64`

*Defined at line 718*

---

## pert_tensor_max

```fides
pub fn pert_tensor_max(t: i64) -> f64 {
```

**Parameters:**

- `t`: `i64`

**Returns:** `f64`

*Defined at line 728*

---

## pert_tensor_min

```fides
pub fn pert_tensor_min(t: i64) -> f64 {
```

**Parameters:**

- `t`: `i64`

**Returns:** `f64`

*Defined at line 742*

---

## pert_tensor_relu

```fides
pub fn pert_tensor_relu(t: i64) -> i64 {
```

**Parameters:**

- `t`: `i64`

**Returns:** `i64`

*Defined at line 806*

---

## pert_tensor_sigmoid

```fides
pub fn pert_tensor_sigmoid(t: i64) -> i64 {
```

**Parameters:**

- `t`: `i64`

**Returns:** `i64`

*Defined at line 810*

---

## pert_tensor_softmax

Softmax along the last axis.

```fides
pub fn pert_tensor_softmax(t: i64) -> i64 {
```

**Parameters:**

- `t`: `i64`

**Returns:** `i64`

*Defined at line 815*

---

## pert_tensor_to_str

```fides
pub fn pert_tensor_to_str(t: i64) -> str {
```

**Parameters:**

- `t`: `i64`

**Returns:** `str`

*Defined at line 881*

---

## pert_tensor_to_json

```fides
pub fn pert_tensor_to_json(t: i64) -> str {
```

**Parameters:**

- `t`: `i64`

**Returns:** `str`

*Defined at line 890*

---

## pert_autograd_begin

grad(x) { ... }: a new tape, no gradients.

```fides
pub fn pert_autograd_begin() {
```

*Defined at line 1062*

---

## pert_autograd_end

```fides
pub fn pert_autograd_end() {
```

*Defined at line 1068*

---

## pert_autograd_backward

Reverse-mode pass over the tape, seeded with 1.0 at the last sum/mean.

```fides
pub fn pert_autograd_backward(loss: i64) {
```

**Parameters:**

- `loss`: `i64`

*Defined at line 1073*

---

## pert_grad_get

The gradient of t after backward (zeros of t's shape when there is none).

```fides
pub fn pert_grad_get(t: i64) -> i64 {
```

**Parameters:**

- `t`: `i64`

**Returns:** `i64`

*Defined at line 1134*

---

## pert_grad_zero

```fides
pub fn pert_grad_zero() {
```

*Defined at line 1142*

---

## pert_checkpoint_begin

```fides
pub fn pert_checkpoint_begin() {
```

*Defined at line 1146*

---

## pert_checkpoint_end

```fides
pub fn pert_checkpoint_end() {
```

*Defined at line 1150*

---

## pert_dp_budget_reset

```fides
pub fn pert_dp_budget_reset(limit: f64) {
```

**Parameters:**

- `limit`: `f64`

*Defined at line 1156*

---

## pert_dp_budget_check

```fides
pub fn pert_dp_budget_check(epsilon: f64, limit: f64) {
```

**Parameters:**

- `epsilon`: `f64`
- `limit`: `f64`

*Defined at line 1161*

---

## pert_dp_noise

```fides
pub fn pert_dp_noise(t: i64, sigma: f64) -> i64 {
```

**Parameters:**

- `t`: `i64`
- `sigma`: `f64`

**Returns:** `i64`

*Defined at line 1208*

---

## pert_gaussian_noise

```fides
pub fn pert_gaussian_noise(shape: Vec<i64>, sigma: f64) -> i64 {
```

**Parameters:**

- `shape`: `Vec<i64>`
- `sigma`: `f64`

**Returns:** `i64`

*Defined at line 1218*

---

## pert_dropout

```fides
pub fn pert_dropout(t: i64, p: f64) -> i64 {
```

**Parameters:**

- `t`: `i64`
- `p`: `f64`

**Returns:** `i64`

*Defined at line 1228*

---

## pert_tp_init

Start the ranks: n of them (FID_TP_WORLD=n > 1 in the environment
overrides the program's n); this process's rank. Once per run: a later
call gives the rank again. A rank started by rank 0 returns its own.

```fides
pub fn pert_tp_init(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 1492*

---

## pert_tp_rank

```fides
pub fn pert_tp_rank() -> i64 {
```

**Returns:** `i64`

*Defined at line 1524*

---

## pert_tp_world_size

```fides
pub fn pert_tp_world_size() -> i64 {
```

**Returns:** `i64`

*Defined at line 1528*

---

## pert_tp_exit

main's end (and exit's): rank 0 waits for the other ranks; its exit
code becomes 1 when one of them failed and the program's was 0.

```fides
pub fn pert_tp_exit(code: i64) -> i64 {
```

**Parameters:**

- `code`: `i64`

**Returns:** `i64`

*Defined at line 1534*

---

## pert_all_reduce

The element-wise sum over the ranks, added in rank order on rank 0.

```fides
pub fn pert_all_reduce(t: i64) -> i64 {
```

**Parameters:**

- `t`: `i64`

**Returns:** `i64`

*Defined at line 1552*

---

## pert_broadcast

Rank 0's tensor, on every rank.

```fides
pub fn pert_broadcast(t: i64) -> i64 {
```

**Parameters:**

- `t`: `i64`

**Returns:** `i64`

*Defined at line 1585*

---

## pert_all_gather

Every rank's tensor joined along dimension 0, in rank order.

```fides
pub fn pert_all_gather(t: i64) -> i64 {
```

**Parameters:**

- `t`: `i64`

**Returns:** `i64`

*Defined at line 1602*

---

## pert_scatter

Rank 0's tensor cut along dimension 0: rank r gets the r-th share of
the rows (the first rows % n ranks one row more).

```fides
pub fn pert_scatter(t: i64) -> i64 {
```

**Parameters:**

- `t`: `i64`

**Returns:** `i64`

*Defined at line 1679*

---

## pert_kv_cache_new

```fides
pub fn pert_kv_cache_new(num_layers: i64, max_seq_len: i64, window_size: i64, head_dim: i64) -> i64 {
```

**Parameters:**

- `num_layers`: `i64`
- `max_seq_len`: `i64`
- `window_size`: `i64`
- `head_dim`: `i64`

**Returns:** `i64`

*Defined at line 1761*

---

## pert_kv_cache_append

```fides
pub fn pert_kv_cache_append(id: i64, layer: i64, k: i64, v: i64) {
```

**Parameters:**

- `id`: `i64`
- `layer`: `i64`
- `k`: `i64`
- `v`: `i64`

*Defined at line 1778*

---

## pert_kv_cache_get

[K; V] of the layer, shape [2, seq_len, head_dim].

```fides
pub fn pert_kv_cache_get(id: i64, layer: i64) -> i64 {
```

**Parameters:**

- `id`: `i64`
- `layer`: `i64`

**Returns:** `i64`

*Defined at line 1807*

---

## pert_kv_cache_len

```fides
pub fn pert_kv_cache_len(id: i64, layer: i64) -> i64 {
```

**Parameters:**

- `id`: `i64`
- `layer`: `i64`

**Returns:** `i64`

*Defined at line 1834*

---

## pert_kv_cache_clear

```fides
pub fn pert_kv_cache_clear(id: i64) {
```

**Parameters:**

- `id`: `i64`

*Defined at line 1846*

---

## pert_kv_cache_evict

Drops the oldest n tokens of every layer.

```fides
pub fn pert_kv_cache_evict(id: i64, n: i64) {
```

**Parameters:**

- `id`: `i64`
- `n`: `i64`

*Defined at line 1863*

---

## pert_quantize_q8

round(v / scale) as i32 + zero_point, clamped to [-128, 127].

```fides
pub fn pert_quantize_q8(t: i64, scale: f64, zero_point: i64) -> i64 {
```

**Parameters:**

- `t`: `i64`
- `scale`: `f64`
- `zero_point`: `i64`

**Returns:** `i64`

*Defined at line 1889*

---

## pert_dequantize_q8

```fides
pub fn pert_dequantize_q8(t: i64, scale: f64, zero_point: i64) -> i64 {
```

**Parameters:**

- `t`: `i64`
- `scale`: `f64`
- `zero_point`: `i64`

**Returns:** `i64`

*Defined at line 1916*

---

## pert_qmatmul

```fides
pub fn pert_qmatmul(a: i64, b: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 1949*

---

## pert_find_quant_params

"scale,zero_point": symmetric, scale = max(|min|, |max|) / 127.

```fides
pub fn pert_find_quant_params(t: i64) -> str {
```

**Parameters:**

- `t`: `i64`

**Returns:** `str`

*Defined at line 1985*

---

## pert_send_tensor

```fides
pub fn pert_send_tensor(t: i64) {
```

**Parameters:**

- `t`: `i64`

*Defined at line 2015*

---

## pert_recv_tensor

```fides
pub fn pert_recv_tensor(shape: Vec<i64>) -> i64 {
```

**Parameters:**

- `shape`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 2022*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
