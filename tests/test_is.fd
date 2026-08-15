// Test: is type/variant check (Phase 7B — F11)
// expr is Type — compile-time type check
// expr is Variant — runtime enum tag check

use core;

pf check_option(x: Option) -> i64 {
    if x is Some {
        ret 1;
    }
    if x is None {
        ret 0;
    }
    ret -1;
}

pf main() -> i64 {
    // Enum variant checks
    pv a = Option::Some(42);
    pv a_check = a is Some;
    print(a_check);
    print("\n");

    pv b = Option::None;
    pv b_check = b is None;
    print(b_check);
    print("\n");

    // Negated: not Some = None
    pv not_some = !(a is Some);
    print(not_some);
    print("\n");

    // Using is in if condition
    pv r1 = check_option(Option::Some(10));
    print(r1);
    print("\n");

    pv r2 = check_option(Option::None);
    print(r2);
    print("\n");

    // Type checks (compile-time, always constant)
    pv n = 42;
    pv n_is_i64 = n is i64;
    print(n_is_i64);
    print("\n");

    pv s = "hello";
    pv s_is_str = s is str;
    print(s_is_str);
    print("\n");

    pv flag = true;
    pv flag_is_bool = flag is bool;
    print(flag_is_bool);
    print("\n");

    ret 0;
}
