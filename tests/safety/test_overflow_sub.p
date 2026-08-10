// Safety test: Integer overflow on subtraction should panic (Phase 2D)
pf main() {
    // i64::MIN = -(i64::MAX) - 1 = -9223372036854775808
    pv max = 9223372036854775807;
    pv min = -max - 1;  // exactly i64::MIN
    pv y = min - 1;      // overflow — should panic
    println(y);
}
