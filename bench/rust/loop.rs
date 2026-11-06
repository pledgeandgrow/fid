fn main() {
    let n: i64 = std::hint::black_box(2_000_000_000);
    let mut s: i64 = 0;
    for i in 0..n { s += (i % 7) * 3 - 1; }
    println!("{}", s);
}
