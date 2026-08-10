// Safety test: Vec::first() on empty vec should panic (Phase 2D)
pf main() {
    pv v = Vec::new();
    pv x = v.first();  // should panic — empty vec
    println(x);
}
