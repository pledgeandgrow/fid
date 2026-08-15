// Test: Loop unrolling — a counted loop with constant trip count 3
// should be fully unrolled by the optimizer. Phase 4B.4
pf sum3() -> i64 {
    pv total = 0;
    pv i = 0;
    while i < 3 {
        total = total + i;
        i = i + 1;
    }
    ret total;
}

pf main() -> i64 !IO {
    pv result = sum3();
    if result != 3 {
        print(999);
        ret 1;
    }
    print(42);
    ret 0;
}
