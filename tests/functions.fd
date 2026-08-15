// Test user-defined functions
pf add(a: i32, b: i32) -> i32 {
    ret a + b;
}

pf factorial(n: i32) -> i32 {
    if n <= 1 {
        ret 1;
    }
    ret n * factorial(n - 1);
}

pf main() {
    pv result = add(3, 4);
    print(result);

    pv fact5 = factorial(5);
    print(fact5);
}
