// Safety test: Option::unwrap on None should panic (Phase 2D)
pf main() {
    pv x = Option::None;
    pv y = x.unwrap();  // should panic
    println(y);
}
