# tensor — Fides Documentation

## Contents

- [zeros](#zeros) (function)
- [ones](#ones) (function)
- [filled](#filled) (function)
- [from_vec](#from_vec) (function)
- [get](#get) (function)
- [set](#set) (function)
- [shape](#shape) (function)
- [ndim](#ndim) (function)
- [size](#size) (function)
- [reshape](#reshape) (function)
- [transpose](#transpose) (function)
- [add](#add) (function)
- [sub](#sub) (function)
- [mul](#mul) (function)
- [scale](#scale) (function)
- [matmul](#matmul) (function)
- [dot](#dot) (function)
- [sum](#sum) (function)
- [mean](#mean) (function)
- [max](#max) (function)
- [min](#min) (function)
- [relu](#relu) (function)
- [sigmoid](#sigmoid) (function)
- [softmax](#softmax) (function)
- [to_str](#to_str) (function)
- [to_json](#to_json) (function)
- [clone](#clone) (function)

## zeros

Create a tensor of zeros with the given shape.
shape is a Vec<i64> of dimension sizes.

```fides
pub fn zeros(shape: Vec<i64>) -> Tensor {
```

**Parameters:**

- `shape`: `Vec<i64>`

**Returns:** `Tensor`

*Defined at line 14*

---

## ones

Create a tensor of ones with the given shape.

```fides
pub fn ones(shape: Vec<i64>) -> Tensor {
```

**Parameters:**

- `shape`: `Vec<i64>`

**Returns:** `Tensor`

*Defined at line 19*

---

## filled

Create a tensor filled with a constant value.

```fides
pub fn filled(shape: Vec<i64>, value: f64) -> Tensor {
```

**Parameters:**

- `shape`: `Vec<i64>`
- `value`: `f64`

**Returns:** `Tensor`

*Defined at line 24*

---

## from_vec

Create a tensor from a flat Vec with a given shape.

```fides
pub fn from_vec(data: Vec<i64>, shape: Vec<i64>) -> Tensor {
```

**Parameters:**

- `data`: `Vec<i64>`
- `shape`: `Vec<i64>`

**Returns:** `Tensor`

*Defined at line 29*

---

## get

Get an element by multi-index. indices is a Vec<i64>.

```fides
pub fn get(t: Tensor, indices: Vec<i64>) -> f64 {
```

**Parameters:**

- `t`: `Tensor`
- `indices`: `Vec<i64>`

**Returns:** `f64`

*Defined at line 34*

---

## set

Set an element by multi-index.

```fides
pub fn set(t: Tensor, indices: Vec<i64>, value: f64) -> void {
```

**Parameters:**

- `t`: `Tensor`
- `indices`: `Vec<i64>`
- `value`: `f64`

**Returns:** `void`

*Defined at line 39*

---

## shape

Get the shape as a Vec<i64>.

```fides
pub fn shape(t: Tensor) -> Vec<i64> {
```

**Parameters:**

- `t`: `Tensor`

**Returns:** `Vec<i64>`

*Defined at line 44*

---

## ndim

Get the number of dimensions.

```fides
pub fn ndim(t: Tensor) -> i64 {
```

**Parameters:**

- `t`: `Tensor`

**Returns:** `i64`

*Defined at line 49*

---

## size

Get the total number of elements.

```fides
pub fn size(t: Tensor) -> i64 {
```

**Parameters:**

- `t`: `Tensor`

**Returns:** `i64`

*Defined at line 54*

---

## reshape

Reshape the tensor (total size must be preserved).

```fides
pub fn reshape(t: Tensor, new_shape: Vec<i64>) -> Tensor {
```

**Parameters:**

- `t`: `Tensor`
- `new_shape`: `Vec<i64>`

**Returns:** `Tensor`

*Defined at line 59*

---

## transpose

Transpose the tensor (reverse all axes).

```fides
pub fn transpose(t: Tensor) -> Tensor {
```

**Parameters:**

- `t`: `Tensor`

**Returns:** `Tensor`

*Defined at line 64*

---

## add

Elementwise addition.

```fides
pub fn add(a: Tensor, b: Tensor) -> Tensor {
```

**Parameters:**

- `a`: `Tensor`
- `b`: `Tensor`

**Returns:** `Tensor`

*Defined at line 69*

---

## sub

Elementwise subtraction.

```fides
pub fn sub(a: Tensor, b: Tensor) -> Tensor {
```

**Parameters:**

- `a`: `Tensor`
- `b`: `Tensor`

**Returns:** `Tensor`

*Defined at line 74*

---

## mul

Elementwise multiplication (Hadamard product).

```fides
pub fn mul(a: Tensor, b: Tensor) -> Tensor {
```

**Parameters:**

- `a`: `Tensor`
- `b`: `Tensor`

**Returns:** `Tensor`

*Defined at line 79*

---

## scale

Scale by a scalar.

```fides
pub fn scale(t: Tensor, s: f64) -> Tensor {
```

**Parameters:**

- `t`: `Tensor`
- `s`: `f64`

**Returns:** `Tensor`

*Defined at line 84*

---

## matmul

Matrix multiplication: (M, K) @ (K, N) -> (M, N).

```fides
pub fn matmul(a: Tensor, b: Tensor) -> Tensor {
```

**Parameters:**

- `a`: `Tensor`
- `b`: `Tensor`

**Returns:** `Tensor`

*Defined at line 89*

---

## dot

Dot product of 1D vectors.

```fides
pub fn dot(a: Tensor, b: Tensor) -> f64 {
```

**Parameters:**

- `a`: `Tensor`
- `b`: `Tensor`

**Returns:** `f64`

*Defined at line 94*

---

## sum

Sum of all elements.

```fides
pub fn sum(t: Tensor) -> f64 {
```

**Parameters:**

- `t`: `Tensor`

**Returns:** `f64`

*Defined at line 99*

---

## mean

Mean of all elements.

```fides
pub fn mean(t: Tensor) -> f64 {
```

**Parameters:**

- `t`: `Tensor`

**Returns:** `f64`

*Defined at line 104*

---

## max

Maximum element.

```fides
pub fn max(t: Tensor) -> f64 {
```

**Parameters:**

- `t`: `Tensor`

**Returns:** `f64`

*Defined at line 109*

---

## min

Minimum element.

```fides
pub fn min(t: Tensor) -> f64 {
```

**Parameters:**

- `t`: `Tensor`

**Returns:** `f64`

*Defined at line 114*

---

## relu

ReLU activation: max(0, x).

```fides
pub fn relu(t: Tensor) -> Tensor {
```

**Parameters:**

- `t`: `Tensor`

**Returns:** `Tensor`

*Defined at line 119*

---

## sigmoid

Sigmoid activation: 1 / (1 + e^-x).

```fides
pub fn sigmoid(t: Tensor) -> Tensor {
```

**Parameters:**

- `t`: `Tensor`

**Returns:** `Tensor`

*Defined at line 124*

---

## softmax

Softmax along the last axis.

```fides
pub fn softmax(t: Tensor) -> Tensor {
```

**Parameters:**

- `t`: `Tensor`

**Returns:** `Tensor`

*Defined at line 129*

---

## to_str

Convert tensor to a readable string representation.

```fides
pub fn to_str(t: Tensor) -> str {
```

**Parameters:**

- `t`: `Tensor`

**Returns:** `str`

*Defined at line 134*

---

## to_json

Convert tensor to JSON (shape + flat data).

```fides
pub fn to_json(t: Tensor) -> str {
```

**Parameters:**

- `t`: `Tensor`

**Returns:** `str`

*Defined at line 139*

---

## clone

Clone a tensor.

```fides
pub fn clone(t: Tensor) -> Tensor {
```

**Parameters:**

- `t`: `Tensor`

**Returns:** `Tensor`

*Defined at line 144*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
