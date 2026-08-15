// Test type aliases — `ty X = T;`
ty Int = i64;
ty Float = f64;
ty Str = str;

struct Pair { first: i64, second: i64, }
ty PairAlias = Pair;

pf main() -> i64 {
    // Basic alias to primitive
    pv x: Int = 42;
    print(x);

    // Alias to float
    pv y: Float = 3.14;
    print(y);

    // Alias to string
    pv s: Str = "hello";
    print(s);

    // Alias to struct type
    pv p = PairAlias { first: 10, second: 20, };
    print(p.first + p.second);

    // Alias in function return type
    pv result = add(5, 3);
    print(result);

    ret 0;
}

pf add(a: Int, b: Int) -> Int {
    ret a + b;
}
