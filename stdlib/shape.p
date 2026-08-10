// Pledge comptime tensor shape verification — Phase 5C.3
//
// Provides runtime shape assertions that double as documentation.
// When comptime evaluation is available (Phase 3A), these assertions
// will be checked at compile time. For now, they're runtime checks.
//
// Usage:
//   pv a = from_vec(data, shape_2x3);
//   assert_shape(a, "2x3");           // runtime check
//   pv b = matmul(a, weight);          // shape verified by matmul itself
//   assert_compatible(a, b, "add");    // verify shapes match for elementwise

module shape;

/// Assert that a tensor has the expected number of dimensions.
pub pf assert_ndim(t: Tensor, expected: i64) -> void {
    pv actual = ndim(t);
    if actual != expected {
        print("Shape error: expected ndim=");
        print(expected);
        print(" but got ndim=");
        print(actual);
        print("\n");
        p_panic("tensor shape mismatch");
    }
}

/// Assert that a tensor has a specific size (total elements).
pub pf assert_size(t: Tensor, expected: i64) -> void {
    pv actual = size(t);
    if actual != expected {
        print("Shape error: expected size=");
        print(expected);
        print(" but got size=");
        print(actual);
        print("\n");
        p_panic("tensor shape mismatch");
    }
}

/// Assert that two tensors have the same shape (for elementwise ops).
pub pf assert_same_shape(a: Tensor, b: Tensor, op: str) -> void {
    pv sa = shape(a);
    pv sb = shape(b);
    pv na = ndim(a);
    pv nb = ndim(b);
    if na != nb {
        print("Shape error: ");
        print(op);
        print(" requires same ndim (");
        print(na);
        print(" vs ");
        print(nb);
        print(")\n");
        p_panic("tensor shape mismatch");
    }
    pv i = 0;
    while i < na {
        pv da = sa.get(i);
        pv db = sb.get(i);
        if da != db {
            print("Shape error: ");
            print(op);
            print(" dimension mismatch at axis ");
            print(i);
            print(" (");
            print(da);
            print(" vs ");
            print(db);
            print(")\n");
            p_panic("tensor shape mismatch");
        }
        i = i + 1;
    }
}

/// Assert that two tensors are compatible for matmul:
/// a is (M, K) and b is (K, N).
pub pf assert_matmul_compatible(a: Tensor, b: Tensor) -> void {
    pv na = ndim(a);
    pv nb = ndim(b);
    if na != 2 || nb != 2 {
        print("Shape error: matmul requires 2D tensors (got ");
        print(na);
        print("D and ");
        print(nb);
        print("D)\n");
        p_panic("tensor shape mismatch");
    }
    pv sa = shape(a);
    pv sb = shape(b);
    pv k_a = sa.get(1);
    pv k_b = sb.get(0);
    if k_a != k_b {
        print("Shape error: matmul inner dimension mismatch (");
        print(k_a);
        print(" vs ");
        print(k_b);
        print(")\n");
        p_panic("tensor shape mismatch");
    }
}

/// Assert that a reshape is valid: total size must be preserved.
pub pf assert_reshape(t: Tensor, new_size: i64) -> void {
    pv old_size = size(t);
    if old_size != new_size {
        print("Shape error: reshape size mismatch (");
        print(old_size);
        print(" vs ");
        print(new_size);
        print(")\n");
        p_panic("tensor shape mismatch");
    }
}

// ─── Comptime shape constants ────────────────────────────────────────
// These are compile-time known shape values that can be used with
// assert_shape to enable future comptime verification.

pub pc shape_1x1: Vec<i64> = vec_lit(1, 1);
pub pc shape_2x2: Vec<i64> = vec_lit(2, 2);
pub pc shape_2x3: Vec<i64> = vec_lit(2, 3);
pub pc shape_3x2: Vec<i64> = vec_lit(3, 2);
pub pc shape_3x4: Vec<i64> = vec_lit(3, 4);
pub pc shape_4x2: Vec<i64> = vec_lit(4, 2);
pub pc shape_4x4: Vec<i64> = vec_lit(4, 4);
pub pc shape_1x3: Vec<i64> = vec_lit(1, 3);
pub pc shape_1x4: Vec<i64> = vec_lit(1, 4);
pub pc shape_4x1: Vec<i64> = vec_lit(4, 1);

// Helper: create a Vec<i64> from two values (comptime)
pf vec_lit(a: i64, b: i64) -> Vec<i64> {
    pv v = Vec::new();
    v.push(a);
    v.push(b);
    ret v;
}
