// Phase 3A — comptime functions.
// `comptime pf` marks a function intended for compile-time evaluation.
// Calls to it from within `comptime { }` blocks are evaluated by the
// comptime engine; the result is substituted as a constant.

comptime pf factorial(n: i64) -> i64 {
    if n <= 1 {
        ret 1;
    }
    ret n * factorial(n - 1);
}

comptime pf fib(n: i64) -> i64 {
    if n < 2 {
        ret n;
    }
    ret fib(n - 1) + fib(n - 2);
}

pf main() -> i64 {
    // 5! = 120, computed entirely at compile time.
    pv f = comptime factorial(5);
    print(f);

    // fib(10) = 55, computed at compile time.
    pv g = comptime fib(10);
    print(g);

    // A comptime block that calls a comptime function.
    pv h = comptime {
        pv a = factorial(3);
        pv b = factorial(4);
        a + b;
    };
    print(h);

    ret 0;
}
