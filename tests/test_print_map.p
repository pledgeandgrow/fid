// Test printing a HashMap directly
pf main() -> i64 {
    pv m = HashMap::new();
    m.insert("alice", 30);
    m.insert("bob", 25);
    print(m);
    ret 0;
}
