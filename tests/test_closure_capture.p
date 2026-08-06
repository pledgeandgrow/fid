// Test closure with captured environment
pf make_adder(n: i64) -> pf(i64) -> i64 {
    ret pf(x: i64) { x + n };
}

pf main() -> i64 {
    pv add5 = make_adder(5);
    pv result = add5(10);
    print(result);
    ret 0;
}
