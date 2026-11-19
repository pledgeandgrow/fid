// k-nucleotide (Benchmarks Game, BM3): see bench/game/fides/knucleotide.fd.
use std::collections::HashMap;

// the THREE section as codes: the index of each drawn character in "acgt"
fn three_codes(n: usize) -> Vec<u8> {
    let mut last = 42i64;
    for _ in 0..3 * n {
        last = (last * 3877 + 29573) % 139968;
    }
    let probs = [0.3029549426680, 0.1979883004921, 0.1975473066391, 0.3015094502008];
    let mut cum = Vec::with_capacity(probs.len());
    let mut p = 0.0f64;
    for &q in &probs {
        p += q;
        cum.push(p);
    }
    let last_i = cum.len() - 1;
    let mut out = Vec::with_capacity(5 * n);
    for _ in 0..5 * n {
        last = (last * 3877 + 29573) % 139968;
        let r = last as f64 / 139968.0;
        let mut i = 0;
        while i < last_i && r >= cum[i] {
            i += 1;
        }
        out.push(i as u8);
    }
    out
}

fn count_kmers(codes: &[u8], k: usize) -> HashMap<u64, u64> {
    let mask: u64 = (1u64 << (2 * k)) - 1;
    let mut m = HashMap::new();
    let mut key = 0u64;
    for (i, &c) in codes.iter().enumerate() {
        key = ((key << 2) | c as u64) & mask;
        if i + 1 >= k {
            *m.entry(key).or_insert(0) += 1;
        }
    }
    m
}

fn encode(s: &str) -> u64 {
    s.bytes().fold(0u64, |key, c| {
        let v = match c {
            b'C' => 1,
            b'G' => 2,
            b'T' => 3,
            _ => 0,
        };
        (key << 2) | v
    })
}

fn fold(h: u64, v: u64) -> u64 {
    (h * 31 + v) & 0xFFFF_FFFF
}

fn frequencies(h: u64, codes: &[u8], k: usize) -> u64 {
    let m = count_kmers(codes, k);
    let mut h = fold(h, m.len() as u64);
    for key in 0..(1u64 << (2 * k)) {
        h = fold(h, *m.get(&key).unwrap_or(&0));
    }
    h
}

fn count_of(h: u64, codes: &[u8], s: &str) -> u64 {
    let m = count_kmers(codes, s.len());
    let h = fold(h, m.len() as u64);
    fold(h, *m.get(&encode(s)).unwrap_or(&0))
}

fn knucleotide(n: usize) -> u64 {
    let codes = three_codes(n);
    let mut h = frequencies(0, &codes, 1);
    h = frequencies(h, &codes, 2);
    for s in ["GGT", "GGTA", "GGTATT", "GGTATTTTAATT", "GGTATTTTAATTTATAGT"] {
        h = count_of(h, &codes, s);
    }
    h
}

fn main() {
    println!("{}", knucleotide(1000000));
}
