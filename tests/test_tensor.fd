// Test: Tensor stdlib — Phase 5C.1
// Tests tensor creation, indexing, arithmetic, matmul, activations, and shape ops.

use tensor;

pf main() {
    // ─── Create tensors ──────────────────────────────────────────────
    pv shape2x3 = Vec::new();
    shape2x3.push(2);
    shape2x3.push(3);

    pv data_a = Vec::new();
    data_a.push(1);
    data_a.push(2);
    data_a.push(3);
    data_a.push(4);
    data_a.push(5);
    data_a.push(6);

    pv a = from_vec(data_a, shape2x3);
    print(to_str(a));
    print("\n");

    // ─── Indexing ────────────────────────────────────────────────────
    pv idx = Vec::new();
    idx.push(1);
    idx.push(2);
    print(get(a, idx));
    print("\n");

    // ─── Shape queries ───────────────────────────────────────────────
    print(ndim(a));
    print("\n");
    print(size(a));
    print("\n");

    // ─── Zeros and ones ──────────────────────────────────────────────
    pv z = zeros(shape2x3);
    print(sum(z));
    print("\n");

    pv o = ones(shape2x3);
    print(sum(o));
    print("\n");

    // ─── Matrix multiplication ───────────────────────────────────────
    pv shape3x2 = Vec::new();
    shape3x2.push(3);
    shape3x2.push(2);

    pv data_b = Vec::new();
    data_b.push(7);
    data_b.push(8);
    data_b.push(9);
    data_b.push(10);
    data_b.push(11);
    data_b.push(12);

    pv b = from_vec(data_b, shape3x2);
    pv c = matmul(a, b);
    print(to_str(c));
    print("\n");

    // ─── Elementwise arithmetic ──────────────────────────────────────
    pv d = add(a, a);
    print(to_str(d));
    print("\n");

    pv e = scale(a, 2.0);
    print(to_str(e));
    print("\n");

    // ─── Reductions ──────────────────────────────────────────────────
    print(sum(a));
    print("\n");
    print(mean(a));
    print("\n");
    print(max(a));
    print("\n");
    print(min(a));
    print("\n");

    // ─── Activations ─────────────────────────────────────────────────
    pv neg_data = Vec::new();
    neg_data.push(-1);
    neg_data.push(0);
    neg_data.push(2);
    neg_data.push(-3);
    neg_data.push(4);
    neg_data.push(-5);
    pv neg = from_vec(neg_data, shape2x3);
    pv r = relu(neg);
    print(to_str(r));
    print("\n");

    // ─── Transpose ───────────────────────────────────────────────────
    pv t = transpose(a);
    print(to_str(t));
    print("\n");

    // ─── Reshape ─────────────────────────────────────────────────────
    pv shape1x6 = Vec::new();
    shape1x6.push(1);
    shape1x6.push(6);
    pv reshaped = reshape(a, shape1x6);
    print(to_str(reshaped));
    print("\n");

    // ─── JSON serialization ──────────────────────────────────────────
    print(to_json(a));
    print("\n");

    ret 0;
}
