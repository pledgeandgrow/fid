// Safety test: Integer overflow on addition should panic (Phase 2D)
pf main() {
    pv x = 9223372036854775807;  // i64::MAX
    pv y = x + 1;                // overflow — should panic
    println(y);
}
