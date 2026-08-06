// Test vec with one push
pf main() -> i64 {
    pv v = Vec::new();
    v.push(42);
    print(v);
    ret 0;
}
