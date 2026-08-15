// Integer overflow checking — valid arithmetic works (Phase 2D)

pf main() {
    // Normal arithmetic
    pv a = 100;
    pv b = 200;
    println(a + b);  // 300

    // Near boundary — still valid
    pv c = 9223372036854775806;  // i64::MAX - 1
    pv d = c + 1;                 // exactly i64::MAX — no overflow
    println(d);                   // 9223372036854775807

    // Subtraction near boundary
    pv e = -9223372036854775807;  // i64::MIN + 1
    pv f = e - 1;                 // exactly i64::MIN — no overflow
    println(f);                   // -9223372036854775808

    // Multiplication
    pv g = 1000000;
    pv h = 1000000;
    println(g * h);  // 1000000000000

    // Division
    pv i = 100;
    pv j = 7;
    println(i / j);  // 14

    // Modulo
    println(i % j);  // 2

    // Negative numbers
    pv k = -10;
    pv l = 3;
    println(k + l);  // -7
    println(k * l);  // -30
}
