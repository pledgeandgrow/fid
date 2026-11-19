// sort (BM4): see bench/work/fides/sort.fd.
fn main() {
    let n: usize = std::hint::black_box(5_000_000);
    let mut x: i64 = 12345;
    let mut v: Vec<i64> = Vec::with_capacity(n);
    for _ in 0..n {
        x = (x * 1103515245 + 12345) % 2147483648;
        v.push(x);
    }
    v.sort_unstable();
    let mut s: i64 = 0;
    for (i, &e) in v.iter().enumerate() {
        s = (s + e * (i as i64 % 1000)) % 1000000007;
    }
    println!("{}", s);
}
