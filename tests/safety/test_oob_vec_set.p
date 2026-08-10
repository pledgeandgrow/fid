// Safety test: Vec out-of-bounds set should panic (Phase 2D)
pf main() {
    pv v = Vec::new();
    v.push(1);
    v.push(2);
    v[10] = 99;  // index 10 but vec has 2 elements — should panic
}
