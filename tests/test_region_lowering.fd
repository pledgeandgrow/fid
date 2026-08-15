// Test: Region-aware lowering — allocations inside region blocks use
// Region::Named instead of Region::Stack. Verified via PIR output.
// Phase 2A.4

pf main() -> i64 !IO {
    region r {
        pv x = 42;
        print(x);
    }
    print(99);
    ret 0;
}
