// Test: guard => early exit syntax (Phase 7A — F6)
// guard cond => action desugars to if !cond { action }

pf check_positive(x: i64) -> i64 {
    guard x > 0 => ret -1;
    ret x;
}

pf check_positive_block(x: i64) -> i64 {
    guard x > 0 => {
        ret -1;
    }
    ret x;
}

pf classify(n: i64) -> str {
    guard n >= 0 => ret "negative rejected";
    guard n <= 100 => ret "too big";
    ret "in range";
}

pf main() -> i64 {
    // Basic guard with single statement
    pv r1 = check_positive(42);
    print(r1);
    print("\n");

    pv r2 = check_positive(-5);
    print(r2);
    print("\n");

    // Guard with block
    pv r3 = check_positive_block(10);
    print(r3);
    print("\n");

    pv r4 = check_positive_block(-3);
    print(r4);
    print("\n");

    // Multiple guards in sequence
    pv c1 = classify(50);
    print(c1);
    print("\n");

    pv c2 = classify(-1);
    print(c2);
    print("\n");

    pv c3 = classify(150);
    print(c3);
    print("\n");

    ret 0;
}
