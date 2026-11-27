[All modules](index.md)

# compiler::tensor -- Fides Documentation

## Contents

- [TensorData](#TensorData) (struct)
- [Tensor_new](#Tensor_new) (function)
- [row_major_strides](#row_major_strides) (function)
- [Tensor_size](#Tensor_size) (function)
- [Tensor_ndim](#Tensor_ndim) (function)
- [Tensor_filled](#Tensor_filled) (function)
- [Tensor_zeros](#Tensor_zeros) (function)
- [Tensor_ones](#Tensor_ones) (function)
- [Tensor_from_vec](#Tensor_from_vec) (function)
- [Tensor_flat_index](#Tensor_flat_index) (function)
- [Tensor_get](#Tensor_get) (function)
- [Tensor_set](#Tensor_set) (function)
- [ResultTensor](#ResultTensor) (enum)
- [ResultF64](#ResultF64) (enum)
- [Tensor_reshape](#Tensor_reshape) (function)
- [Tensor_transpose](#Tensor_transpose) (function)
- [Tensor_add](#Tensor_add) (function)
- [Tensor_sub](#Tensor_sub) (function)
- [Tensor_mul](#Tensor_mul) (function)
- [Tensor_scale](#Tensor_scale) (function)
- [Tensor_matmul](#Tensor_matmul) (function)
- [Tensor_dot](#Tensor_dot) (function)
- [Tensor_sum](#Tensor_sum) (function)
- [Tensor_mean](#Tensor_mean) (function)
- [Tensor_max](#Tensor_max) (function)
- [Tensor_min](#Tensor_min) (function)
- [Tensor_relu](#Tensor_relu) (function)
- [Tensor_sigmoid](#Tensor_sigmoid) (function)
- [Tensor_softmax](#Tensor_softmax) (function)
- [Tensor_quantize_q8](#Tensor_quantize_q8) (function)
- [Tensor_dequantize_q8](#Tensor_dequantize_q8) (function)
- [QuantParams](#QuantParams) (struct)
- [Tensor_find_quant_params](#Tensor_find_quant_params) (function)
- [Tensor_to_json](#Tensor_to_json) (function)
- [Tensor_to_string](#Tensor_to_string) (function)
- [I64Vec](#I64Vec) (struct)
- [I64Vec_new](#I64Vec_new) (function)
- [KvCache](#KvCache) (struct)
- [KvCache_new](#KvCache_new) (function)
- [KvCache_append](#KvCache_append) (function)
- [KvCache_get](#KvCache_get) (function)
- [KvCache_len](#KvCache_len) (function)
- [KvCache_clear](#KvCache_clear) (function)
- [KvCache_evict_oldest](#KvCache_evict_oldest) (function)

## TensorData

```fides
pub struct TensorData {
```

**Fields:**

| Name | Type |
|------|------|
| `shape` | `Vec<i64>` |
| `strides` | `Vec<i64>` |
| `data` | `Vec<f64>,  // flat data storage (f64 for now)` |

*Defined at line 28*

---

## Tensor_new

```fides
pub fn Tensor_new(shape: Vec<i64>, strides: Vec<i64>, data: Vec<f64>) -> TensorData {
```

**Parameters:**

- `shape`: `Vec<i64>`
- `strides`: `Vec<i64>`
- `data`: `Vec<f64>`

**Returns:** `TensorData`

*Defined at line 36*

---

## row_major_strides

Compute row-major strides for a given shape.

```fides
pub fn row_major_strides(shape: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `shape`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 43*

---

## Tensor_size

Total number of elements (product of shape).

```fides
pub fn Tensor_size(self: &TensorData) -> i64 {
```

**Parameters:**

- `self`: `&TensorData`

**Returns:** `i64`

*Defined at line 64*

---

## Tensor_ndim

Number of dimensions.

```fides
pub fn Tensor_ndim(self: &TensorData) -> i64 {
```

**Parameters:**

- `self`: `&TensorData`

**Returns:** `i64`

*Defined at line 75*

---

## Tensor_filled

Create a tensor filled with a constant value.

```fides
pub fn Tensor_filled(shape: Vec<i64>, value: f64) -> TensorData {
```

**Parameters:**

- `shape`: `Vec<i64>`
- `value`: `f64`

**Returns:** `TensorData`

*Defined at line 80*

---

## Tensor_zeros

Create a tensor of zeros.

```fides
pub fn Tensor_zeros(shape: Vec<i64>) -> TensorData {
```

**Parameters:**

- `shape`: `Vec<i64>`

**Returns:** `TensorData`

*Defined at line 98*

---

## Tensor_ones

Create a tensor of ones.

```fides
pub fn Tensor_ones(shape: Vec<i64>) -> TensorData {
```

**Parameters:**

- `shape`: `Vec<i64>`

**Returns:** `TensorData`

*Defined at line 103*

---

## Tensor_from_vec

Create a tensor from a flat Vec with a given shape.

```fides
pub fn Tensor_from_vec(shape: Vec<i64>, data: Vec<f64>) -> TensorData {
```

**Parameters:**

- `shape`: `Vec<i64>`
- `data`: `Vec<f64>`

**Returns:** `TensorData`

*Defined at line 108*

---

## Tensor_flat_index

Convert a multi-index to a flat offset.

```fides
pub fn Tensor_flat_index(self: &TensorData, indices: Vec<i64>) -> i64 {
```

**Parameters:**

- `self`: `&TensorData`
- `indices`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 114*

---

## Tensor_get

Get an element by multi-index.

```fides
pub fn Tensor_get(self: &TensorData, indices: Vec<i64>) -> f64 {
```

**Parameters:**

- `self`: `&TensorData`
- `indices`: `Vec<i64>`

**Returns:** `f64`

*Defined at line 125*

---

## Tensor_set

Set an element by multi-index.

```fides
pub fn Tensor_set(self: &mut TensorData, indices: Vec<i64>, value: f64) -> void {
```

**Parameters:**

- `self`: `&mut TensorData`
- `indices`: `Vec<i64>`
- `value`: `f64`

**Returns:** `void`

*Defined at line 134*

---

## ResultTensor

```fides
pub enum ResultTensor {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 143*

---

## ResultF64

```fides
pub enum ResultF64 {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 147*

---

## Tensor_reshape

Reshape the tensor (total size must be preserved).

```fides
pub fn Tensor_reshape(self: &TensorData, new_shape: Vec<i64>) -> ResultTensor {
```

**Parameters:**

- `self`: `&TensorData`
- `new_shape`: `Vec<i64>`

**Returns:** `ResultTensor`

*Defined at line 153*

---

## Tensor_transpose

Transpose: reverse the shape and rearrange data.

```fides
pub fn Tensor_transpose(self: &TensorData) -> TensorData {
```

**Parameters:**

- `self`: `&TensorData`

**Returns:** `TensorData`

*Defined at line 168*

---

## Tensor_add

Elementwise addition (shapes must match).

```fides
pub fn Tensor_add(self: &TensorData, other: &TensorData) -> ResultTensor {
```

**Parameters:**

- `self`: `&TensorData`
- `other`: `&TensorData`

**Returns:** `ResultTensor`

*Defined at line 220*

---

## Tensor_sub

Elementwise subtraction.

```fides
pub fn Tensor_sub(self: &TensorData, other: &TensorData) -> ResultTensor {
```

**Parameters:**

- `self`: `&TensorData`
- `other`: `&TensorData`

**Returns:** `ResultTensor`

*Defined at line 234*

---

## Tensor_mul

Elementwise multiplication (Hadamard product).

```fides
pub fn Tensor_mul(self: &TensorData, other: &TensorData) -> ResultTensor {
```

**Parameters:**

- `self`: `&TensorData`
- `other`: `&TensorData`

**Returns:** `ResultTensor`

*Defined at line 248*

---

## Tensor_scale

Scale by a scalar.

```fides
pub fn Tensor_scale(self: &TensorData, scalar: f64) -> TensorData {
```

**Parameters:**

- `self`: `&TensorData`
- `scalar`: `f64`

**Returns:** `TensorData`

*Defined at line 262*

---

## Tensor_matmul

Matrix multiplication: (M, K) @ (K, N) -> (M, N). Both tensors must be 2D.

```fides
pub fn Tensor_matmul(self: &TensorData, other: &TensorData) -> ResultTensor {
```

**Parameters:**

- `self`: `&TensorData`
- `other`: `&TensorData`

**Returns:** `ResultTensor`

*Defined at line 273*

---

## Tensor_dot

Dot product (1D vectors).

```fides
pub fn Tensor_dot(self: &TensorData, other: &TensorData) -> ResultF64 {
```

**Parameters:**

- `self`: `&TensorData`
- `other`: `&TensorData`

**Returns:** `ResultF64`

*Defined at line 317*

---

## Tensor_sum

Sum of all elements.

```fides
pub fn Tensor_sum(self: &TensorData) -> f64 {
```

**Parameters:**

- `self`: `&TensorData`

**Returns:** `f64`

*Defined at line 334*

---

## Tensor_mean

Mean of all elements.

```fides
pub fn Tensor_mean(self: &TensorData) -> f64 {
```

**Parameters:**

- `self`: `&TensorData`

**Returns:** `f64`

*Defined at line 345*

---

## Tensor_max

Maximum element.

```fides
pub fn Tensor_max(self: &TensorData) -> f64 {
```

**Parameters:**

- `self`: `&TensorData`

**Returns:** `f64`

*Defined at line 353*

---

## Tensor_min

Minimum element.

```fides
pub fn Tensor_min(self: &TensorData) -> f64 {
```

**Parameters:**

- `self`: `&TensorData`

**Returns:** `f64`

*Defined at line 366*

---

## Tensor_relu

ReLU activation: max(0, x)

```fides
pub fn Tensor_relu(self: &TensorData) -> TensorData {
```

**Parameters:**

- `self`: `&TensorData`

**Returns:** `TensorData`

*Defined at line 385*

---

## Tensor_sigmoid

Sigmoid activation: 1 / (1 + e^-x)

```fides
pub fn Tensor_sigmoid(self: &TensorData) -> TensorData {
```

**Parameters:**

- `self`: `&TensorData`

**Returns:** `TensorData`

*Defined at line 400*

---

## Tensor_softmax

Softmax along the last axis.

```fides
pub fn Tensor_softmax(self: &TensorData) -> TensorData {
```

**Parameters:**

- `self`: `&TensorData`

**Returns:** `TensorData`

*Defined at line 412*

---

## Tensor_quantize_q8

Quantize a float tensor to int8 (symmetric quantization).

```fides
pub fn Tensor_quantize_q8(self: &TensorData, scale: f64, zero_point: i64) -> TensorData {
```

**Parameters:**

- `self`: `&TensorData`
- `scale`: `f64`
- `zero_point`: `i64`

**Returns:** `TensorData`

*Defined at line 459*

---

## Tensor_dequantize_q8

Dequantize an int8 tensor back to float.

```fides
pub fn Tensor_dequantize_q8(self: &TensorData, scale: f64, zero_point: i64) -> TensorData {
```

**Parameters:**

- `self`: `&TensorData`
- `scale`: `f64`
- `zero_point`: `i64`

**Returns:** `TensorData`

*Defined at line 478*

---

## QuantParams

```fides
pub struct QuantParams {
```

**Fields:**

| Name | Type |
|------|------|
| `scale` | `f64` |
| `zero_point` | `i64` |

*Defined at line 489*

---

## Tensor_find_quant_params

Find optimal quantization parameters (scale and zero_point) for a tensor.
Uses symmetric quantization: scale = max(|min|, |max|) / 127, zero_point = 0.

```fides
pub fn Tensor_find_quant_params(self: &TensorData) -> QuantParams {
```

**Parameters:**

- `self`: `&TensorData`

**Returns:** `QuantParams`

*Defined at line 496*

---

## Tensor_to_json

Serialize to JSON string.

```fides
pub fn Tensor_to_json(self: &TensorData) -> str {
```

**Parameters:**

- `self`: `&TensorData`

**Returns:** `str`

*Defined at line 535*

---

## Tensor_to_string

Pretty-print as a nested array string.

```fides
pub fn Tensor_to_string(self: &TensorData) -> str {
```

**Parameters:**

- `self`: `&TensorData`

**Returns:** `str`

*Defined at line 559*

---

## I64Vec

A KV cache for LLM attention -- stores K and V tensors per layer.
Bounded by max_seq_len with optional sliding window eviction.

```fides
pub struct I64Vec {
```

**Fields:**

| Name | Type |
|------|------|
| `items` | `Vec<i64>` |

*Defined at line 638*

---

## I64Vec_new

```fides
pub fn I64Vec_new() -> I64Vec {
```

**Returns:** `I64Vec`

*Defined at line 642*

---

## KvCache

```fides
pub struct KvCache {
```

**Fields:**

| Name | Type |
|------|------|
| `k_cache` | `Vec<I64Vec>` |
| `v_cache` | `Vec<I64Vec>` |
| `seq_len` | `Vec<i64>` |
| `max_seq_len` | `i64` |
| `window_size` | `i64` |
| `head_dim` | `i64,  // Head dimension` |

*Defined at line 648*

---

## KvCache_new

```fides
pub fn KvCache_new(num_layers: i64, max_seq_len: i64, window_size: i64, head_dim: i64) -> KvCache {
```

**Parameters:**

- `num_layers`: `i64`
- `max_seq_len`: `i64`
- `window_size`: `i64`
- `head_dim`: `i64`

**Returns:** `KvCache`

*Defined at line 662*

---

## KvCache_append

Append K and V for a given layer. The tensor should be 2D [seq, head_dim].
If the cache exceeds max_seq_len, oldest tokens are evicted (sliding window).

```fides
pub fn KvCache_append(self: &mut KvCache, layer: i64, k: &TensorData, v: &TensorData) -> void {
```

**Parameters:**

- `self`: `&mut KvCache`
- `layer`: `i64`
- `k`: `&TensorData`
- `v`: `&TensorData`

**Returns:** `void`

*Defined at line 680*

---

## KvCache_get

Get K and V for a layer as a concatenated tensor [2, seq, head_dim].

```fides
pub fn KvCache_get(self: &KvCache, layer: i64) -> TensorData {
```

**Parameters:**

- `self`: `&KvCache`
- `layer`: `i64`

**Returns:** `TensorData`

*Defined at line 725*

---

## KvCache_len

Current sequence length for a layer.

```fides
pub fn KvCache_len(self: &KvCache, layer: i64) -> i64 {
```

**Parameters:**

- `self`: `&KvCache`
- `layer`: `i64`

**Returns:** `i64`

*Defined at line 752*

---

## KvCache_clear

Clear all layers.

```fides
pub fn KvCache_clear(self: &mut KvCache) -> void {
```

**Parameters:**

- `self`: `&mut KvCache`

**Returns:** `void`

*Defined at line 760*

---

## KvCache_evict_oldest

Evict the oldest `n` tokens from all layers (sliding window).

```fides
pub fn KvCache_evict_oldest(self: &mut KvCache, n: i64) -> void {
```

**Parameters:**

- `self`: `&mut KvCache`
- `n`: `i64`

**Returns:** `void`

*Defined at line 771*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
