// Regression test: f-string interpolation with different types
// Tests that p_to_str dispatches correctly for float, vec, map, bool, int

pf main() -> i64 {
    pv x = 3.14;
    print(f"float: {x}");       // float: 3.14

    pv v = Vec::new();
    v.push(1);
    v.push(2);
    print(f"vec: {v}");         // vec: [1, 2]

    pv b = true;
    print(f"bool: {b}");        // bool: true

    pv i = 42;
    print(f"int: {i}");         // int: 42

    ret 0;
}
