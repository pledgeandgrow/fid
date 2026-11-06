use std::collections::HashMap;
fn main() {
    let n: i64 = std::hint::black_box(1_000_000);
    let mut m: HashMap<String, i64> = HashMap::new();
    for i in 0..n { m.insert(format!("key{}", i), i); }
    let mut sum: i64 = 0;
    for i in 0..n { sum += m[&format!("key{}", i)]; }
    println!("{}", sum);
}
