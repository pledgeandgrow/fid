// Test generics in PE emitter
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

pf main() -> i64 {
    pv a = identity(42);
    print(a);
    pv m = max_value(10, 20);
    print(m);
    pv n: i64 = max_value(50, 30);
    print(n);
    ret 0;
}
