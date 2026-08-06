// Test references in PE emitter
pf add_one(x: &i64) -> i64 {
    ret *x + 1;
}

pf main() -> i64 {
    pv v = 41;
    pv r = add_one(&v);
    print(r);
    ret 0;
}
