// Test generic enum: Option<T>

enum Option {
    Some(i64),
    None,
}

pf some_func() -> Option {
    ret Option::Some(42);
}

pf none_func() -> Option {
    ret Option::None;
}

pf main() -> i32 {
    pv a = some_func();
    match a {
        Option::Some(v: i64) => { print(v); }
        Option::None => { print(0); }
    }

    pv b = none_func();
    match b {
        Option::Some(v: i64) => { print(v); }
        Option::None => { print(-1); }
    }

    ret 0;
}
