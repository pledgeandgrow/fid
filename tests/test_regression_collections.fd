// Regression test: collection printing (vec, map)
// Tests that print/println correctly format Vec and HashMap

pf main() -> i64 {
    pv v = Vec::new();
    v.push(42);
    print(v);                   // [42]

    pv m = HashMap::new();
    m.insert("alice", 30);
    m.insert("bob", 25);
    print(m);                   // {alice: 30, bob: 25}

    ret 0;
}
