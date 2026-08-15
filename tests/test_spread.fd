// Test: .. spread operator (Phase 7B — F7)
// Point { x: 1, ..p } copies all fields from p, then overrides x

struct Point {
    x: i64,
    y: i64,
    label: str,
}

pf main() -> i64 {
    // Create a base point
    pv base = Point { x: 10, y: 20, label: "base" };
    print(base.x);
    print("\n");
    print(base.y);
    print("\n");
    print(base.label);
    print("\n");

    // Spread with override: copy base, change x
    pv moved = Point { x: 99, ..base };
    print(moved.x);
    print("\n");
    print(moved.y);
    print("\n");
    print(moved.label);
    print("\n");

    // Spread with override of different field
    pv relabeled = Point { label: "moved", ..base };
    print(relabeled.x);
    print("\n");
    print(relabeled.y);
    print("\n");
    print(relabeled.label);
    print("\n");

    // Spread with multiple overrides
    pv both = Point { x: 1, label: "both", ..base };
    print(both.x);
    print("\n");
    print(both.y);
    print("\n");
    print(both.label);
    print("\n");

    // Spread only (no overrides = copy)
    pv copy = Point { ..base };
    print(copy.x);
    print("\n");
    print(copy.y);
    print("\n");
    print(copy.label);
    print("\n");

    ret 0;
}
