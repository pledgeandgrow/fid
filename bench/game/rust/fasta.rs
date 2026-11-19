// fasta (Benchmarks Game, BM3): see bench/game/fides/fasta.fd.
const ALU: &[u8] = b"GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGGGAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGACCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAATACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCAGCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGGAGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCCAGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA";

fn random_hash(mut h: u64, seed: &mut i64, chars: &[u8], probs: &[f64], m: usize) -> u64 {
    let mut cum = Vec::with_capacity(probs.len());
    let mut p = 0.0f64;
    for &q in probs {
        p += q;
        cum.push(p);
    }
    let last_i = cum.len() - 1;
    let mut last = *seed;
    for _ in 0..m {
        last = (last * 3877 + 29573) % 139968;
        let r = last as f64 / 139968.0;
        let mut i = 0;
        while i < last_i && r >= cum[i] {
            i += 1;
        }
        h = (h * 31 + chars[i] as u64) & 0xFFFF_FFFF;
    }
    *seed = last;
    h
}

fn fasta(n: usize) -> u64 {
    let mut h = 0u64;
    for k in 0..2 * n {
        h = (h * 31 + ALU[k % ALU.len()] as u64) & 0xFFFF_FFFF;
    }
    let mut seed = 42i64;
    let mut iub = vec![0.27, 0.12, 0.12, 0.27];
    iub.extend(std::iter::repeat(0.02).take(11));
    h = random_hash(h, &mut seed, b"acgtBDHKMNRSVWY", &iub, 3 * n);
    let homo = [0.3029549426680, 0.1979883004921, 0.1975473066391, 0.3015094502008];
    random_hash(h, &mut seed, b"acgt", &homo, 5 * n)
}

fn main() {
    println!("{}", fasta(2500000));
}
