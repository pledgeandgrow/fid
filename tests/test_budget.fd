// Test: region with budget (F3)
// Allocations under the budget should compile fine.

pf main() {
    // 4 i64 variables = 32 bytes, budget = 64 → OK
    region r with budget(64) {
        pv a: i64 = 1;
        pv b: i64 = 2;
        pv c: i64 = 3;
        pv d: i64 = 4;
        print(f"sum={a + b + c + d}");
        print("\n");
    }
    ret 0;
}
