[All modules](index.md)

# tensor -- Fides Documentation

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

*Defined at line 13*

---

## ones

Create a tensor of ones with the given shape.

```fides
pub fn ones(shape: Vec<i64>) -> Tensor {
```

**Parameters:**

- `shape`: `Vec<i64>`

**Returns:** `Tensor`

*Defined at line 18*

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

*Defined at line 23*

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

*Defined at line 28*

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

*Defined at line 33*

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

*Defined at line 38*

---

## shape

Get the shape as a Vec<i64>.

```fides
pub fn shape(t: Tensor) -> Vec<i64> {
```

**Parameters:**

- `t`: `Tensor`

**Returns:** `Vec<i64>`

*Defined at line 43*

---

## ndim

Get the number of dimensions.

```fides
pub fn ndim(t: Tensor) -> i64 {
```

**Parameters:**

- `t`: `Tensor`

**Returns:** `i64`

*Defined at line 48*

---

## size

Get the total number of elements.

```fides
pub fn size(t: Tensor) -> i64 {
```

**Parameters:**

- `t`: `Tensor`

**Returns:** `i64`

*Defined at line 53*

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

*Defined at line 58*

---

## transpose

Transpose the tensor (reverse all axes).

```fides
pub fn transpose(t: Tensor) -> Tensor {
```

**Parameters:**

- `t`: `Tensor`

**Returns:** `Tensor`

*Defined at line 63*

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

*Defined at line 68*

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

*Defined at line 73*

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

*Defined at line 78*

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

*Defined at line 83*

---

## matmul

Matrix multiplication: (M, K) @ (K, N) -> (M, N).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var s: Vec<i64> = Vec::new();
s.push(2); s.push(2);
val a = ones(s);
val b = filled(s, 3.0);
val m = matmul(a, b);
var ij: Vec<i64> = Vec::new();
ij.push(1); ij.push(0);
assert(get(m, ij) == 6.0);
assert(sum(add(a, b)) == 16.0);
assert(mean(scale(a, 2.0)) == 2.0);
assert_eq(size(m), 4);
assert_eq(ndim(m), 2);
assert(max(sub(b, a)) == 2.0);
```

```fides
pub fn matmul(a: Tensor, b: Tensor) -> Tensor {
```

**Parameters:**

- `a`: `Tensor`
- `b`: `Tensor`

**Returns:** `Tensor`

*Defined at line 103*

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

*Defined at line 108*

---

## sum

Sum of all elements.

```fides
pub fn sum(t: Tensor) -> f64 {
```

**Parameters:**

- `t`: `Tensor`

**Returns:** `f64`

*Defined at line 113*

---

## mean

Mean of all elements.

```fides
pub fn mean(t: Tensor) -> f64 {
```

**Parameters:**

- `t`: `Tensor`

**Returns:** `f64`

*Defined at line 118*

---

## max

Maximum element.

```fides
pub fn max(t: Tensor) -> f64 {
```

**Parameters:**

- `t`: `Tensor`

**Returns:** `f64`

*Defined at line 123*

---

## min

Minimum element.

```fides
pub fn min(t: Tensor) -> f64 {
```

**Parameters:**

- `t`: `Tensor`

**Returns:** `f64`

*Defined at line 128*

---

## relu

ReLU activation: max(0, x).

```fides
pub fn relu(t: Tensor) -> Tensor {
```

**Parameters:**

- `t`: `Tensor`

**Returns:** `Tensor`

*Defined at line 133*

---

## sigmoid

Sigmoid activation: 1 / (1 + e^-x).

```fides
pub fn sigmoid(t: Tensor) -> Tensor {
```

**Parameters:**

- `t`: `Tensor`

**Returns:** `Tensor`

*Defined at line 138*

---

## softmax

Softmax along the last axis.

```fides
pub fn softmax(t: Tensor) -> Tensor {
```

**Parameters:**

- `t`: `Tensor`

**Returns:** `Tensor`

*Defined at line 143*

---

## to_str

Convert tensor to a readable string representation.

```fides
pub fn to_str(t: Tensor) -> str {
```

**Parameters:**

- `t`: `Tensor`

**Returns:** `str`

*Defined at line 148*

---

## to_json

Convert tensor to JSON (shape + flat data).

```fides
pub fn to_json(t: Tensor) -> str {
```

**Parameters:**

- `t`: `Tensor`

**Returns:** `str`

*Defined at line 153*

---

## clone

Clone a tensor.

```fides
pub fn clone(t: Tensor) -> Tensor {
```

**Parameters:**

- `t`: `Tensor`

**Returns:** `Tensor`

*Defined at line 158*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
