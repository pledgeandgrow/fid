// Region inference test: basic region block.
// region r { ... } creates a scope; variables inside are region-allocated.
pf main() -> i64 !IO {
    region r {
        pv x = 42;
        print(x);
    }
    print("after region");
    ret 0;
}
