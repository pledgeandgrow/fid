// Test: WASM target — exercises integer arithmetic, control flow,
// function calls, and string printing. This is the subset the WASM
// backend fully supports (Phase 5D.1).

pf fib(n: i64) -> i64 {
    if n < 2 {
        ret n;
    }
    ret fib(n - 1) + fib(n - 2);
}

pf gcd(a: i64, b: i64) -> i64 {
    if b == 0 {
        ret a;
    }
    ret gcd(b, a % b);
}

pf main() {
    pv r = fib(10);
    print(r);
    print("\n");
    pv g = gcd(48, 36);
    print(g);
    print("\n");
    pv sum = 0;
    pv i = 1;
    while i <= 100 {
        sum = sum + i;
        i = i + 1;
    }
    print(sum);
    print("\n");
    ret 0;
}
