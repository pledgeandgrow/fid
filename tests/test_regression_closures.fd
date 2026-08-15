// Regression test: closures with string-based dispatch
// Tests that CallPtr correctly dispatches to user functions
// via the closure environment struct.

pf double(x: i64) -> i64 {
    ret x * 2;
}

pf inc(x: i64) -> i64 {
    ret x + 1;
}

pf main() -> i64 {
    // Test closures through the standard library map/filter pattern
    pv d = double;
    pv i = inc;
    println(d(5));  // 10
    println(i(5));  // 6
    ret 0;
}
