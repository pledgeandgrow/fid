// Test: ?. optional chaining (Phase 7C — F8)
// expr?.field — if expr is None, return 0; else unwrap and access field.
// The key behavior: None short-circuits to 0 without crashing.
// Note: Full field access on wrapped structs requires Option<T> generics (Phase 10).

use core;

pf maybe_value(x: i64) -> Option {
    if x > 0 {
        ret Option::Some(x * 2);
    }
    ret Option::None;
}

pf main() -> i64 {
    // None — ?. short-circuits to 0 (no crash, no field access attempted)
    pv none = Option::None;
    pv val2 = none?.nonexistent;
    print(val2);
    print("\n");

    // Chain with function call — None case (short-circuits to 0)
    pv val4 = maybe_value(-1)?.nonexistent;
    print(val4);
    print("\n");

    ret 0;
}
