// Region inference test: variable escape from region (should fail).
// pv x is allocated in region r, but we try to use it after the block.
// This should produce a region error.
pf main() -> i64 !IO {
    region r {
        pv x = 42;
    }
    // x is no longer valid here — use-after-free
    print(x);
    ret 0;
}
