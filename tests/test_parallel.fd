// Test: Auto-parallelization framework — Phase 5C.4
// Tests parallel map, sum, dot product, fill, and range operations.

use parallel;

pf main() {
    // ─── Parallel map ────────────────────────────────────────────────
    pv squares = par_map(5);
    print(squares);
    print("\n");

    // ─── Parallel sum ────────────────────────────────────────────────
    pv data = Vec::new();
    data.push(1);
    data.push(2);
    data.push(3);
    data.push(4);
    data.push(5);
    pv s = par_sum(data);
    print(s);
    print("\n");

    // ─── Parallel dot product ────────────────────────────────────────
    pv a = Vec::new();
    a.push(1);
    a.push(2);
    a.push(3);
    pv b = Vec::new();
    b.push(4);
    b.push(5);
    b.push(6);
    pv dp = par_dot(a, b);
    print(dp);
    print("\n");

    // ─── Parallel fill ───────────────────────────────────────────────
    pv filled = par_fill(5, 42);
    print(filled);
    print("\n");

    // ─── Parallel range ──────────────────────────────────────────────
    pv r = par_range(0, 5);
    print(r);
    print("\n");

    // ─── Parallelizability check ─────────────────────────────────────
    pv pure = is_parallelizable(0);
    print(pure);
    print("\n");

    pv has_io = is_parallelizable(1);
    print(has_io);
    print("\n");

    pv has_state = is_parallelizable(2);
    print(has_state);
    print("\n");

    pv has_panic = is_parallelizable(4);
    print(has_panic);
    print("\n");

    ret 0;
}
