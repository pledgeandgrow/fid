// Test ? operator in PE emitter
pf maybe_fail(x: i64) -> Result<i64, i64> {
    if x == 0 {
        ret Result::Err(99);
    }
    ret Result::Ok(x * 2);
}

pf main() -> i64 {
    pv r = maybe_fail(5)?;
    print(r);
    ret 0;
}
