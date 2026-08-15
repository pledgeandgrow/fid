// Test: ?? coalescing operator (Phase 7A — F9)
// expr ?? default — if expr is None, return default; else unwrap and return value

use core;

pf maybe_value(x: i64) -> Option {
    if x > 0 {
        ret Option::Some(x);
    }
    ret Option::None;
}

pf main() -> i64 {
    // Some value ?? default → unwrapped value
    pv a = maybe_value(42) ?? 0;
    print(a);
    print("\n");

    // None ?? default → default
    pv b = maybe_value(-1) ?? 99;
    print(b);
    print("\n");

    // Chain with function call
    pv c = maybe_value(10) ?? -1;
    print(c);
    print("\n");

    // None with negative default
    pv d = maybe_value(0) ?? -100;
    print(d);
    print("\n");

    // Direct Option literal
    pv e = Option::Some(7) ?? 0;
    print(e);
    print("\n");

    pv f = Option::None ?? 55;
    print(f);
    print("\n");

    ret 0;
}
