// Test match with range patterns — `start..=end =>` and `start..end =>`
pf main() -> i64 {
    // Test 1: inclusive range (1..=3)
    pv n = 2;
    pv r1 = match n {
        1..=3 => 10,
        4..=6 => 20,
        _ => 99,
    };
    print(r1);

    // Test 2: exclusive range (1..5 means 1,2,3,4)
    pv m = 4;
    pv r2 = match m {
        1..5 => 20,
        5..=10 => 30,
        _ => 99,
    };
    print(r2);

    // Test 3: boundary — inclusive end
    pv a = 3;
    pv r3 = match a {
        1..=3 => 100,
        _ => 999,
    };
    print(r3);

    // Test 4: boundary — exclusive end (5 not included in 1..5)
    pv b = 5;
    pv r4 = match b {
        1..5 => 100,
        5..=10 => 200,
        _ => 999,
    };
    print(r4);

    // Test 5: range with guard
    pv c = 7;
    pv r5 = match c {
        1..=10 if c > 5 => 300,
        1..=10 => 400,
        _ => 999,
    };
    print(r5);

    ret 0;
}
