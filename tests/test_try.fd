// Test ? operator with Result-like enum
enum Result {
    Ok(i64),
    Err(i64),
}

pf ok_value() -> Result {
    ret Result::Ok(42);
}

pf main() -> i32 {
    pv r = ok_value();
    pv v = r?;
    print(v);
    ret 0;
}
