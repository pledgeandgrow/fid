// strings (BM4): see bench/work/fides/strings.fd.
use std::fmt::Write;

fn main() {
    let n: i64 = std::hint::black_box(2_000_000);
    let mut s = String::new();
    for i in 0..n {
        write!(s, "{},", i).unwrap();
    }
    let mut h: i64 = 0;
    for &b in s.as_bytes() {
        h = (h * 31 + b as i64) % 1000000007;
    }
    println!("{}", h * 100000000 + s.len() as i64);
}
