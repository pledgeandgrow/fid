// Test: Tail call optimization — self-recursive factorial should be
// rewritten as a loop (no stack growth). Phase 4B.5
pf fact(n: i64, acc: i64) -> i64 {
    if n <= 1 {
        ret acc;
    }
    ret fact(n - 1, n * acc);
}

pf main() -> i64 !IO {
    pv result = fact(5, 1);
    if result != 120 {
        print(999);
        ret 1;
    }
    print(42);
    ret 0;
}
