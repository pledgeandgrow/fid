// Test match with enum and string patterns
pf main() -> i64 {
    // Test 1: match on integer
    pv n = 2;
    pv r1 = match n {
        1 => 10,
        2 => 20,
        3 => 30,
        _ => 99,
    };
    print(r1);

    // Test 2: match on string
    pv s = "hello";
    pv r2 = match s {
        "hi" => 1,
        "hello" => 2,
        "bye" => 3,
        _ => 99,
    };
    print(r2);

    // Test 3: match with enum
    pv opt = Option::Some(42);
    pv r3 = match opt {
        Some(x: i64) => x,
        None => 0,
    };
    print(r3);

    // Test 4: match with default only
    pv r4 = match n {
        _ => 77,
    };
    print(r4);

    ret 0;
}
