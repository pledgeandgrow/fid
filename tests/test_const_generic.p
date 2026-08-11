// Test const generics — [T; N] where N is a comptime value
pf sum<N: i64>(arr: [i64; N]) -> i64 {
    pv s = 0;
    for i in 0..N {
        s = s + arr[i];
    }
    ret s;
}

pf dot<N: i64>(a: [i64; N], b: [i64; N]) -> i64 {
    pv s = 0;
    for i in 0..N {
        s = s + a[i] * b[i];
    }
    ret s;
}

pf main() -> i64 {
    // Test 1: sum of 3-element array
    pv arr1 = [1, 2, 3];
    pv r1 = sum(arr1);
    print(r1);  // 6

    // Test 2: sum of 5-element array
    pv arr2 = [10, 20, 30, 40, 50];
    pv r2 = sum(arr2);
    print(r2);  // 150

    // Test 3: dot product of 3-element arrays
    pv a = [1, 2, 3];
    pv b = [4, 5, 6];
    pv r3 = dot(a, b);
    print(r3);  // 1*4 + 2*5 + 3*6 = 4 + 10 + 18 = 32

    // Test 4: dot product of 4-element arrays
    pv c = [1, 0, 2, 1];
    pv d = [3, 1, 4, 1];
    pv r4 = dot(c, d);
    print(r4);  // 1*3 + 0*1 + 2*4 + 1*1 = 3 + 0 + 8 + 1 = 12

    ret 0;
}
