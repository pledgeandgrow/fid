// Phase 3A — comptime evaluation: basic arithmetic and substitution.
// `comptime <expr>` is evaluated at compile time and replaced by its value.

pf main() -> i64 {
    // comptime arithmetic — folded to 42 at compile time.
    pv x = comptime 40 + 2;
    print(x);

    // comptime block — value is the last expression.
    pv y = comptime {
        pv a = 10;
        pv b = 20;
        a + b;
    };
    print(y);

    // comptime expression with precedence.
    pv z = comptime 6 * 7;
    print(z);

    ret 0;
}
