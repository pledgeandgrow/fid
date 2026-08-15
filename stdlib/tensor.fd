// Pledge Tensor standard library — Phase 5C.1
//
// N-dimensional arrays with stride info for AI/ML workloads.
// Tensors are opaque values backed by native f64 storage.
//
// Design: Tensor operations are implemented as native builtins (p_tensor_*).
// This stdlib provides a clean Pledge-level API. When comptime reflection
// (Phase 3A) lands, tensor shape types will be verified at compile time.

module tensor;

/// Create a tensor of zeros with the given shape.
/// shape is a Vec<i64> of dimension sizes.
pub pf zeros(shape: Vec<i64>) -> Tensor {
    p_tensor_zeros(shape)
}

/// Create a tensor of ones with the given shape.
pub pf ones(shape: Vec<i64>) -> Tensor {
    p_tensor_ones(shape)
}

/// Create a tensor filled with a constant value.
pub pf filled(shape: Vec<i64>, value: f64) -> Tensor {
    p_tensor_filled(shape, value)
}

/// Create a tensor from a flat Vec with a given shape.
pub pf from_vec(data: Vec<i64>, shape: Vec<i64>) -> Tensor {
    p_tensor_from_vec(data, shape)
}

/// Get an element by multi-index. indices is a Vec<i64>.
pub pf get(t: Tensor, indices: Vec<i64>) -> f64 {
    p_tensor_get(t, indices)
}

/// Set an element by multi-index.
pub pf set(t: Tensor, indices: Vec<i64>, value: f64) -> void {
    p_tensor_set(t, indices, value)
}

/// Get the shape as a Vec<i64>.
pub pf shape(t: Tensor) -> Vec<i64> {
    p_tensor_shape(t)
}

/// Get the number of dimensions.
pub pf ndim(t: Tensor) -> i64 {
    p_tensor_ndim(t)
}

/// Get the total number of elements.
pub pf size(t: Tensor) -> i64 {
    p_tensor_size(t)
}

/// Reshape the tensor (total size must be preserved).
pub pf reshape(t: Tensor, new_shape: Vec<i64>) -> Tensor {
    p_tensor_reshape(t, new_shape)
}

/// Transpose the tensor (reverse all axes).
pub pf transpose(t: Tensor) -> Tensor {
    p_tensor_transpose(t)
}

/// Elementwise addition.
pub pf add(a: Tensor, b: Tensor) -> Tensor {
    p_tensor_add(a, b)
}

/// Elementwise subtraction.
pub pf sub(a: Tensor, b: Tensor) -> Tensor {
    p_tensor_sub(a, b)
}

/// Elementwise multiplication (Hadamard product).
pub pf mul(a: Tensor, b: Tensor) -> Tensor {
    p_tensor_mul(a, b)
}

/// Scale by a scalar.
pub pf scale(t: Tensor, s: f64) -> Tensor {
    p_tensor_scale(t, s)
}

/// Matrix multiplication: (M, K) @ (K, N) -> (M, N).
pub pf matmul(a: Tensor, b: Tensor) -> Tensor {
    p_tensor_matmul(a, b)
}

/// Dot product of 1D vectors.
pub pf dot(a: Tensor, b: Tensor) -> f64 {
    p_tensor_dot(a, b)
}

/// Sum of all elements.
pub pf sum(t: Tensor) -> f64 {
    p_tensor_sum(t)
}

/// Mean of all elements.
pub pf mean(t: Tensor) -> f64 {
    p_tensor_mean(t)
}

/// Maximum element.
pub pf max(t: Tensor) -> f64 {
    p_tensor_max(t)
}

/// Minimum element.
pub pf min(t: Tensor) -> f64 {
    p_tensor_min(t)
}

/// ReLU activation: max(0, x).
pub pf relu(t: Tensor) -> Tensor {
    p_tensor_relu(t)
}

/// Sigmoid activation: 1 / (1 + e^-x).
pub pf sigmoid(t: Tensor) -> Tensor {
    p_tensor_sigmoid(t)
}

/// Softmax along the last axis.
pub pf softmax(t: Tensor) -> Tensor {
    p_tensor_softmax(t)
}

/// Convert tensor to a readable string representation.
pub pf to_str(t: Tensor) -> str {
    p_tensor_to_str(t)
}

/// Convert tensor to JSON (shape + flat data).
pub pf to_json(t: Tensor) -> str {
    p_tensor_to_json(t)
}

/// Clone a tensor.
pub pf clone(t: Tensor) -> Tensor {
    p_tensor_clone(t)
}
