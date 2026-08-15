// Test: region budget exceeded (F3)
// 4 i64 variables = 32 bytes, budget = 16 → compile error

pf main() {
    region r with budget(16) {
        pv a: i64 = 1;
        pv b: i64 = 2;
        pv c: i64 = 3;
        pv d: i64 = 4;
    }
    ret 0;
}
