// Test: SIMD stdlib — Phase 5C.2
// Tests SIMD vector operations (elementwise add/sub/mul/div, dot, sum, scale).

use simd;

pf main() {
    // ─── Create SIMD vectors ─────────────────────────────────────────
    pv a = Vec::new();
    a.push(1.0);
    a.push(2.0);
    a.push(3.0);
    a.push(4.0);

    pv b = Vec::new();
    b.push(10.0);
    b.push(20.0);
    b.push(30.0);
    b.push(40.0);

    // ─── Elementwise operations ──────────────────────────────────────
    pv c = add(a, b);
    print(to_str(c));
    print("\n");

    pv d = sub(b, a);
    print(to_str(d));
    print("\n");

    pv e = mul(a, b);
    print(to_str(e));
    print("\n");

    // ─── Dot product ─────────────────────────────────────────────────
    pv dp = dot(a, b);
    print(dp);
    print("\n");

    // ─── Sum ─────────────────────────────────────────────────────────
    pv s = sum(a);
    print(s);
    print("\n");

    // ─── Scale ───────────────────────────────────────────────────────
    pv sc = scale(a, 2.5);
    print(to_str(sc));
    print("\n");

    ret 0;
}
