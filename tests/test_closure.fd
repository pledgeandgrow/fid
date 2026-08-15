// Test closures
pf apply(f: pf(i64) -> i64, x: i64) -> i64 {
    ret f(x);
}

pf double(x: i64) -> i64 {
    ret x * 2;
}

pf main() -> i32 {
    pv result = apply(double, 5);
    print(result);

    ret 0;
}