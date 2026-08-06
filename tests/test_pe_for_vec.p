// Test for-over-Vec in PE emitter
pf main() -> i64 {
    pv v = Vec::new();
    v.push(10);
    v.push(20);
    v.push(30);
    pv sum = 0;
    for x in v {
        sum = sum + x;
    }
    print(sum);
    ret 0;
}
