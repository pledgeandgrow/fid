// Test mutable references
pf increment(x: &mut i64) {
    *x = *x + 1;
}

pf add_to(x: &mut i64, n: i64) {
    *x += n;
}

pf main() -> i64 {
    pv mut v = 10;
    increment(&mut v);
    print(v);

    add_to(&mut v, 5);
    print(v);

    pv mut w = 100;
    pv r = &mut w;
    *r = 42;
    print(w);

    ret 0;
}
