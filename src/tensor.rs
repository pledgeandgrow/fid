// Tensor type — n-dimensional arrays with stride info.
//
// Phase 5C.1: AI/ML foundation. A Tensor stores:
//   - shape: Vec<usize> — size of each dimension
//   - strides: Vec<usize> — element offset per dimension (row-major by default)
//   - data: Vec<f64> — flat storage (doubles for now; f32 specialization later)
//
// Supported operations:
//   - create (zeros, ones, from_vec, filled)
//   - indexing (get/set by multi-index)
//   - shape queries (shape, ndim, size, reshape, transpose)
//   - elementwise arithmetic (add, sub, mul, scale)
//   - linear algebra (matmul, dot)
//   - reductions (sum, mean, max, min)
//   - utility (to_json, from_json, print)

use std::cell::RefCell;
use std::rc::Rc;

/// A tensor — n-dimensional array with stride info.
#[derive(Debug, Clone)]
pub struct Tensor {
    /// Shape: size of each dimension, e.g. [2, 3] for a 2x3 matrix
    pub shape: Vec<usize>,
    /// Strides: element offset to advance one step in each dimension.
    /// Row-major (C order) by default.
    pub strides: Vec<usize>,
    /// Flat data storage (f64 for now)
    pub data: Vec<f64>,
}

impl Tensor {
    /// Compute row-major strides for a given shape.
    pub fn row_major_strides(shape: &[usize]) -> Vec<usize> {
        let n = shape.len();
        if n == 0 {
            return vec![];
        }
        let mut strides = vec![0usize; n];
        strides[n - 1] = 1;
        for i in (0..n - 1).rev() {
            strides[i] = strides[i + 1] * shape[i + 1];
        }
        strides
    }

    /// Total number of elements (product of shape).
    pub fn size(&self) -> usize {
        self.shape.iter().product()
    }

    /// Number of dimensions.
    pub fn ndim(&self) -> usize {
        self.shape.len()
    }

    /// Create a tensor filled with a constant value.
    pub fn filled(shape: Vec<usize>, value: f64) -> Self {
        let strides = Self::row_major_strides(&shape);
        let size = shape.iter().product();
        let data = vec![value; size];
        Tensor { shape, strides, data }
    }

    /// Create a tensor of zeros.
    pub fn zeros(shape: Vec<usize>) -> Self {
        Self::filled(shape, 0.0)
    }

    /// Create a tensor of ones.
    pub fn ones(shape: Vec<usize>) -> Self {
        Self::filled(shape, 1.0)
    }

    /// Create a tensor from a flat Vec with a given shape.
    pub fn from_vec(shape: Vec<usize>, data: Vec<f64>) -> Self {
        let strides = Self::row_major_strides(&shape);
        Tensor { shape, strides, data }
    }

    /// Convert a multi-index to a flat offset.
    pub fn flat_index(&self, indices: &[i64]) -> usize {
        let mut offset = 0usize;
        for (i, &idx) in indices.iter().enumerate() {
            offset += (idx as usize) * self.strides[i];
        }
        offset
    }

    /// Get an element by multi-index.
    pub fn get(&self, indices: &[i64]) -> f64 {
        let fi = self.flat_index(indices);
        if fi < self.data.len() { self.data[fi] } else { 0.0 }
    }

    /// Set an element by multi-index.
    pub fn set(&mut self, indices: &[i64], value: f64) {
        let fi = self.flat_index(indices);
        if fi < self.data.len() {
            self.data[fi] = value;
        };
    }

    /// Reshape the tensor (total size must be preserved).
    pub fn reshape(&self, new_shape: Vec<usize>) -> Result<Tensor, String> {
        let new_size: usize = new_shape.iter().product();
        if new_size != self.size() {
            return Err(format!(
                "reshape: size mismatch ({} -> {})",
                self.size(), new_size
            ));
        }
        let strides = Self::row_major_strides(&new_shape);
        Ok(Tensor {
            shape: new_shape,
            strides,
            data: self.data.clone(),
        })
    }

    /// Transpose: swap two axes (for 2D, swap rows and columns).
    /// For general transpose, reverse the shape and recompute strides.
    pub fn transpose(&self) -> Tensor {
        let n = self.shape.len();
        let new_shape: Vec<usize> = self.shape.iter().rev().copied().collect();
        // Need to actually rearrange data for row-major output
        let mut result = Tensor::zeros(new_shape.clone());
        let old_indices = vec![0i64; n];
        let mut idx = old_indices;
        loop {
            let val = self.get(&idx);
            // Reverse indices for the transposed tensor
            let rev_idx: Vec<i64> = idx.iter().rev().copied().collect();
            result.set(&rev_idx, val);
            // Increment indices
            let mut carry = true;
            for i in (0..n).rev() {
                if !carry {
                    break;
                }
                idx[i] += 1;
                if idx[i] as usize >= self.shape[i] {
                    idx[i] = 0;
                } else {
                    carry = false;
                }
            }
            if carry {
                break;
            }
        }
        result
    }

    /// Elementwise addition (shapes must match).
    pub fn add(&self, other: &Tensor) -> Result<Tensor, String> {
        if self.shape != other.shape {
            return Err(format!(
                "add: shape mismatch {:?} vs {:?}",
                self.shape, other.shape
            ));
        }
        let data: Vec<f64> = self.data.iter()
            .zip(other.data.iter())
            .map(|(a, b)| a + b)
            .collect();
        Ok(Tensor {
            shape: self.shape.clone(),
            strides: self.strides.clone(),
            data,
        })
    }

    /// Elementwise subtraction.
    pub fn sub(&self, other: &Tensor) -> Result<Tensor, String> {
        if self.shape != other.shape {
            return Err(format!(
                "sub: shape mismatch {:?} vs {:?}",
                self.shape, other.shape
            ));
        }
        let data: Vec<f64> = self.data.iter()
            .zip(other.data.iter())
            .map(|(a, b)| a - b)
            .collect();
        Ok(Tensor {
            shape: self.shape.clone(),
            strides: self.strides.clone(),
            data,
        })
    }

    /// Elementwise multiplication (Hadamard product).
    pub fn mul(&self, other: &Tensor) -> Result<Tensor, String> {
        if self.shape != other.shape {
            return Err(format!(
                "mul: shape mismatch {:?} vs {:?}",
                self.shape, other.shape
            ));
        }
        let data: Vec<f64> = self.data.iter()
            .zip(other.data.iter())
            .map(|(a, b)| a * b)
            .collect();
        Ok(Tensor {
            shape: self.shape.clone(),
            strides: self.strides.clone(),
            data,
        })
    }

    /// Scale by a scalar.
    pub fn scale(&self, scalar: f64) -> Tensor {
        let data: Vec<f64> = self.data.iter().map(|v| v * scalar).collect();
        Tensor {
            shape: self.shape.clone(),
            strides: self.strides.clone(),
            data,
        }
    }

    /// Matrix multiplication: (M, K) @ (K, N) -> (M, N).
    /// Both tensors must be 2D.
    pub fn matmul(&self, other: &Tensor) -> Result<Tensor, String> {
        if self.ndim() != 2 || other.ndim() != 2 {
            return Err(format!(
                "matmul: requires 2D tensors (got {}D and {}D)",
                self.ndim(), other.ndim()
            ));
        }
        let m = self.shape[0];
        let k = self.shape[1];
        let k2 = other.shape[0];
        let n = other.shape[1];
        if k != k2 {
            return Err(format!(
                "matmul: inner dimension mismatch ({} vs {})",
                k, k2
            ));
        }
        let mut result = Tensor::zeros(vec![m, n]);
        for i in 0..m {
            for j in 0..n {
                let mut sum = 0.0;
                for kk in 0..k {
                    sum += self.get(&[i as i64, kk as i64]) * other.get(&[kk as i64, j as i64]);
                }
                result.set(&[i as i64, j as i64], sum);
            }
        }
        Ok(result)
    }

    /// Dot product (1D vectors).
    pub fn dot(&self, other: &Tensor) -> Result<f64, String> {
        if self.ndim() != 1 || other.ndim() != 1 {
            return Err(format!(
                "dot: requires 1D tensors (got {}D and {}D)",
                self.ndim(), other.ndim()
            ));
        }
        if self.shape[0] != other.shape[0] {
            return Err(format!(
                "dot: size mismatch ({} vs {})",
                self.shape[0], other.shape[0]
            ));
        }
        Ok(self.data.iter()
            .zip(other.data.iter())
            .map(|(a, b)| a * b)
            .sum())
    }

    /// Sum of all elements.
    pub fn sum(&self) -> f64 {
        self.data.iter().sum()
    }

    /// Mean of all elements.
    pub fn mean(&self) -> f64 {
        if self.data.is_empty() {
            return 0.0;
        }
        self.sum() / self.data.len() as f64
    }

    /// Maximum element.
    pub fn max(&self) -> f64 {
        self.data.iter().cloned().fold(f64::NEG_INFINITY, f64::max)
    }

    /// Minimum element.
    pub fn min(&self) -> f64 {
        self.data.iter().cloned().fold(f64::INFINITY, f64::min)
    }

    /// Apply a function elementwise (for ReLU, sigmoid, etc.)
    pub fn apply<F: Fn(f64) -> f64>(&self, f: F) -> Tensor {
        let data: Vec<f64> = self.data.iter().map(|v| f(*v)).collect();
        Tensor {
            shape: self.shape.clone(),
            strides: self.strides.clone(),
            data,
        }
    }

    /// ReLU activation: max(0, x)
    pub fn relu(&self) -> Tensor {
        self.apply(|x| x.max(0.0))
    }

    /// Sigmoid activation: 1 / (1 + e^-x)
    pub fn sigmoid(&self) -> Tensor {
        self.apply(|x| 1.0 / (1.0 + (-x).exp()))
    }

    /// Softmax along the last axis.
    pub fn softmax(&self) -> Tensor {
        if self.ndim() == 0 {
            return self.clone();
        }
        let mut result = self.clone();
        let last = self.ndim() - 1;
        let row_size = self.shape[last];
        let num_rows = self.size() / row_size;
        for r in 0..num_rows {
            let start = r * row_size;
            let end = start + row_size;
            let max_val = self.data[start..end].iter().cloned().fold(f64::NEG_INFINITY, f64::max);
            let mut exp_vals: Vec<f64> = self.data[start..end].iter()
                .map(|v| (v - max_val).exp())
                .collect();
            let sum: f64 = exp_vals.iter().sum();
            for v in &mut exp_vals {
                *v /= sum;
            }
            result.data[start..end].copy_from_slice(&exp_vals);
        }
        result
    }

    /// Serialize to JSON string.
    pub fn to_json(&self) -> String {
        let shape_json: Vec<String> = self.shape.iter()
            .map(|s| s.to_string())
            .collect();
        let data_json: Vec<String> = self.data.iter()
            .map(|d| {
                if *d == d.trunc() && d.is_finite() && d.abs() < 1e15 {
                    format!("{}", *d as i64)
                } else {
                    format!("{}", d)
                }
            })
            .collect();
        format!(
            "{{\"shape\":[{}],\"data\":[{}]}}",
            shape_json.join(","),
            data_json.join(",")
        )
    }

    /// Pretty-print as a nested array string.
    pub fn to_string(&self) -> String {
        fn format_recursive(data: &[f64], shape: &[usize], strides: &[usize], offset: usize) -> String {
            if shape.is_empty() {
                let v = if offset < data.len() { data[offset] } else { 0.0 };
                return format!("{}", v);
            }
            if shape.len() == 1 {
                let items: Vec<String> = (0..shape[0])
                    .map(|i| {
                        let idx = offset + i * strides[0];
                        let v = if idx < data.len() { data[idx] } else { 0.0 };
                        if v == v.trunc() && v.is_finite() && v.abs() < 1e15 {
                            format!("{}", v as i64)
                        } else {
                            format!("{:.4}", v)
                        }
                    })
                    .collect();
                return format!("[{}]", items.join(", "));
            }
            let items: Vec<String> = (0..shape[0])
                .map(|i| format_recursive(data, &shape[1..], &strides[1..], offset + i * strides[0]))
                .collect();
            format!("[{}]", items.join(", "))
        }
        format_recursive(&self.data, &self.shape, &self.strides, 0)
    }

    // ─── ML10: Quantization ──────────────────────────────────────────

    /// Quantize a float tensor to int8 (symmetric quantization).
    /// Each value is clamped to [-128, 127] after scaling.
    /// The quantized value = round(value / scale) + zero_point, clamped to [-128, 127].
    pub fn quantize_q8(&self, scale: f64, zero_point: i32) -> Tensor {
        let data: Vec<f64> = self.data.iter().map(|&v| {
            let q = (v / scale).round() as i32 + zero_point;
            let clamped = q.clamp(-128, 127);
            clamped as f64
        }).collect();
        Tensor {
            shape: self.shape.clone(),
            strides: self.strides.clone(),
            data,
        }
    }

    /// Dequantize an int8 tensor back to float.
    /// value = (quantized - zero_point) * scale
    pub fn dequantize_q8(&self, scale: f64, zero_point: i32) -> Tensor {
        let data: Vec<f64> = self.data.iter().map(|&v| {
            (v - zero_point as f64) * scale
        }).collect();
        Tensor {
            shape: self.shape.clone(),
            strides: self.strides.clone(),
            data,
        }
    }

    /// Find optimal quantization parameters (scale and zero_point) for a tensor.
    /// Uses symmetric quantization: scale = max(|min|, |max|) / 127, zero_point = 0.
    pub fn find_quant_params(&self) -> (f64, i32) {
        let min = self.data.iter().fold(f64::INFINITY, |a, &b| a.min(b));
        let max = self.data.iter().fold(f64::NEG_INFINITY, |a, &b| a.max(b));
        let abs_max = min.abs().max(max.abs());
        let scale = if abs_max > 0.0 { abs_max / 127.0 } else { 1.0 };
        (scale, 0)
    }
}

// ─── ML9: KV Cache ──────────────────────────────────────────────────

/// A KV cache for LLM attention — stores K and V tensors per layer.
/// Bounded by max_seq_len with optional sliding window eviction.
#[derive(Clone)]
pub struct KvCache {
    /// K tensors per layer: shape [seq_len, head_dim]
    pub k_cache: Vec<Vec<f64>>,
    /// V tensors per layer: shape [seq_len, head_dim]
    pub v_cache: Vec<Vec<f64>>,
    /// Current sequence length per layer
    pub seq_len: Vec<usize>,
    /// Maximum sequence length (budget in tokens)
    pub max_seq_len: usize,
    /// Sliding window size (0 = no window, keep all until max)
    pub window_size: usize,
    /// Head dimension
    pub head_dim: usize,
}

impl KvCache {
    /// Create a new KV cache for `num_layers` layers.
    pub fn new(num_layers: usize, max_seq_len: usize, window_size: usize, head_dim: usize) -> Self {
        KvCache {
            k_cache: vec![Vec::new(); num_layers],
            v_cache: vec![Vec::new(); num_layers],
            seq_len: vec![0; num_layers],
            max_seq_len,
            window_size,
            head_dim,
        }
    }

    /// Append K and V for a given layer. The tensor should be 2D [seq, head_dim].
    /// If the cache exceeds max_seq_len, oldest tokens are evicted (sliding window).
    pub fn append(&mut self, layer: usize, k: &Tensor, v: &Tensor) {
        if layer >= self.k_cache.len() {
            return;
        }
        let k_flat = &k.data;
        let v_flat = &v.data;
        // Append new tokens
        self.k_cache[layer].extend_from_slice(k_flat);
        self.v_cache[layer].extend_from_slice(v_flat);
        self.seq_len[layer] = self.k_cache[layer].len() / self.head_dim.max(1);
        // Enforce sliding window
        let max_tokens = if self.window_size > 0 {
            self.window_size.min(self.max_seq_len)
        } else {
            self.max_seq_len
        };
        let max_bytes = max_tokens * self.head_dim;
        if self.k_cache[layer].len() > max_bytes {
            let excess = self.k_cache[layer].len() - max_bytes;
            self.k_cache[layer].drain(0..excess);
            self.v_cache[layer].drain(0..excess);
            self.seq_len[layer] = self.k_cache[layer].len() / self.head_dim.max(1);
        }
    }

    /// Get K and V for a layer as a concatenated tensor [K; V].
    pub fn get(&self, layer: usize) -> Tensor {
        if layer >= self.k_cache.len() {
            return Tensor::zeros(vec![0, self.head_dim]);
        }
        let seq = self.seq_len[layer];
        let mut data = self.k_cache[layer].clone();
        data.extend_from_slice(&self.v_cache[layer]);
        Tensor::from_vec(vec![2, seq, self.head_dim], data)
    }

    /// Current sequence length for a layer.
    pub fn len(&self, layer: usize) -> usize {
        if layer < self.seq_len.len() {
            self.seq_len[layer]
        } else {
            0
        }
    }

    /// Clear all layers.
    pub fn clear(&mut self) {
        for layer in 0..self.k_cache.len() {
            self.k_cache[layer].clear();
            self.v_cache[layer].clear();
            self.seq_len[layer] = 0;
        }
    }

    /// Evict the oldest `n` tokens from all layers (sliding window).
    pub fn evict_oldest(&mut self, n: usize) {
        let bytes_to_evict = n * self.head_dim;
        for layer in 0..self.k_cache.len() {
            if self.k_cache[layer].len() >= bytes_to_evict {
                self.k_cache[layer].drain(0..bytes_to_evict);
                self.v_cache[layer].drain(0..bytes_to_evict);
                self.seq_len[layer] = self.k_cache[layer].len() / self.head_dim.max(1);
            } else {
                self.k_cache[layer].clear();
                self.v_cache[layer].clear();
                self.seq_len[layer] = 0;
            }
        }
    }
}

/// Convenience type for Rc<RefCell<Tensor>> (matches interpreter's opaque value pattern).
pub type TensorRef = Rc<RefCell<Tensor>>;

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_create_and_index() {
        let t = Tensor::from_vec(vec![2, 3], vec![1.0, 2.0, 3.0, 4.0, 5.0, 6.0]);
        assert_eq!(t.get(&[0, 0]), 1.0);
        assert_eq!(t.get(&[0, 2]), 3.0);
        assert_eq!(t.get(&[1, 1]), 5.0);
        assert_eq!(t.size(), 6);
        assert_eq!(t.ndim(), 2);
    }

    #[test]
    fn test_matmul() {
        let a = Tensor::from_vec(vec![2, 3], vec![1.0, 2.0, 3.0, 4.0, 5.0, 6.0]);
        let b = Tensor::from_vec(vec![3, 2], vec![7.0, 8.0, 9.0, 10.0, 11.0, 12.0]);
        let c = a.matmul(&b).unwrap();
        assert_eq!(c.shape, vec![2, 2]);
        // [1*7+2*9+3*11, 1*8+2*10+3*12] = [58, 64]
        // [4*7+5*9+6*11, 4*8+5*10+6*12] = [139, 154]
        assert_eq!(c.get(&[0, 0]), 58.0);
        assert_eq!(c.get(&[0, 1]), 64.0);
        assert_eq!(c.get(&[1, 0]), 139.0);
        assert_eq!(c.get(&[1, 1]), 154.0);
    }

    #[test]
    fn test_elementwise() {
        let a = Tensor::from_vec(vec![2, 2], vec![1.0, 2.0, 3.0, 4.0]);
        let b = Tensor::from_vec(vec![2, 2], vec![5.0, 6.0, 7.0, 8.0]);
        let c = a.add(&b).unwrap();
        assert_eq!(c.data, vec![6.0, 8.0, 10.0, 12.0]);
        let d = a.mul(&b).unwrap();
        assert_eq!(d.data, vec![5.0, 12.0, 21.0, 32.0]);
    }

    #[test]
    fn test_relu() {
        let a = Tensor::from_vec(vec![3], vec![-1.0, 0.0, 2.0]);
        let r = a.relu();
        assert_eq!(r.data, vec![0.0, 0.0, 2.0]);
    }

    #[test]
    fn test_softmax() {
        let a = Tensor::from_vec(vec![3], vec![1.0, 2.0, 3.0]);
        let s = a.softmax();
        let total: f64 = s.data.iter().sum();
        assert!((total - 1.0).abs() < 1e-10);
        // Largest input should have largest probability
        assert!(s.data[2] > s.data[1]);
        assert!(s.data[1] > s.data[0]);
    }

    #[test]
    fn test_transpose() {
        let a = Tensor::from_vec(vec![2, 3], vec![1.0, 2.0, 3.0, 4.0, 5.0, 6.0]);
        let t = a.transpose();
        assert_eq!(t.shape, vec![3, 2]);
        assert_eq!(t.get(&[0, 0]), 1.0);
        assert_eq!(t.get(&[0, 1]), 4.0);
        assert_eq!(t.get(&[1, 0]), 2.0);
        assert_eq!(t.get(&[2, 1]), 6.0);
    }
}
