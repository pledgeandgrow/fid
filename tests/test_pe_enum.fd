// Test enum/Option/Result in PE emitter
pf main() -> i64 {
    pv o = Option::Some(42);
    match o {
        Some(x: i64) => { print(x); }
        None => { print(0); }
    }

    pv r = Result::Ok(100);
    match r {
        Ok(v: i64) => { print(v); }
        Err(e: i64) => { print(e); }
    }

    pv n = Option::None;
    match n {
        Some(x: i64) => { print(x); }
        None => { print(-1); }
    }

    ret 0;
}
