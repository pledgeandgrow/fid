// Effect system test: pure function (no effects).
// Declares !{} and performs no effects — should compile and run.
pf add(a: i64, b: i64) -> i64 !{} {
    ret a + b;
}

pf main() -> i64 !IO {
    pv result = add(3, 4);
    print(result);
    ret 0;
}
