fn main() {
    let n: usize = std::hint::black_box(20_000_000);
    let mut is = vec![true; n];
    is[0] = false; is[1] = false;
    let mut p = 2;
    while p * p < n {
        if is[p] { let mut m = p * p; while m < n { is[m] = false; m += p; } }
        p += 1;
    }
    println!("{}", is.iter().filter(|&&b| b).count());
}
