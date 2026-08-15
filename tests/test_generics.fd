// Test generic functions with monomorphization

pf identity<T>(x: T) -> T {
    ret x;
}

pf max_value<T>(a: T, b: T) -> T {
    if a > b {
        ret a;
    } else {
        ret b;
    }
}

pf main() -> i32 {
    // identity with i32
    pv a = identity(42);
    print(a);

    // identity with i64
    pv b: i64 = identity(100);
    print(b);

    // max_value with i32
    pv m = max_value(10, 20);
    print(m);

    // max_value with i64
    pv n: i64 = max_value(50, 30);
    print(n);

    ret 0;
}
