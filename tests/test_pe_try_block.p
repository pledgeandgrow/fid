// Test try blocks — catches ? early returns, wraps result in Result
pf risky(x: i64) -> Result<i64, i64> {
    if x == 0 {
        ret Result::Err(42);
    }
    ret Result::Ok(x * 10);
}

pf main() -> i64 {
    // try block with Ok result
    pv r1 = try { risky(5)? };
    match r1 {
        Ok(v: i64) => { print(v); }
        Err(e: i64) => { print(e); }
    }

    // try block with Err result (caught, not propagated)
    pv r2 = try { risky(0)? };
    match r2 {
        Ok(v: i64) => { print(v); }
        Err(e: i64) => { print(e); }
    }

    // try block without ? — just wraps value in Ok
    pv r3 = try { 99 };
    match r3 {
        Ok(v: i64) => { print(v); }
        Err(e: i64) => { print(e); }
    }

    ret 0;
}
