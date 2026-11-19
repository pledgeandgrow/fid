// hash (BM4): see bench/work/fides/hash.fd.
fn fnv(data: &[u8], rounds: usize) -> u64 {
    let mut h: u64 = 0xcbf29ce484222325;
    for _ in 0..rounds {
        for &b in data {
            h = (h ^ b as u64).wrapping_mul(0x100000001b3);
        }
    }
    h
}

fn main() {
    let n: usize = std::hint::black_box(50_000_000);
    let data: Vec<u8> = (0..n).map(|i| ((i * 7 + i / 256) % 256) as u8).collect();
    println!("{}", fnv(&data, 4) as i64);
}
