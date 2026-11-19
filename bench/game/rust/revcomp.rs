// reverse-complement (Benchmarks Game, BM3): see bench/game/fides/revcomp.fd.
const ALU: &[u8] = b"GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGGGAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGACCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAATACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCAGCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGGAGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCCAGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA";

fn repeat_section(m: usize) -> Vec<u8> {
    (0..m).map(|k| ALU[k % ALU.len()]).collect()
}

fn random_section(seed: &mut i64, chars: &[u8], probs: &[f64], m: usize) -> Vec<u8> {
    let mut cum = Vec::with_capacity(probs.len());
    let mut p = 0.0f64;
    for &q in probs {
        p += q;
        cum.push(p);
    }
    let last_i = cum.len() - 1;
    let mut last = *seed;
    let mut out = Vec::with_capacity(m);
    for _ in 0..m {
        last = (last * 3877 + 29573) % 139968;
        let r = last as f64 / 139968.0;
        let mut i = 0;
        while i < last_i && r >= cum[i] {
            i += 1;
        }
        out.push(chars[i]);
    }
    *seed = last;
    out
}

fn comp_table() -> [u8; 256] {
    let mut t = [0u8; 256];
    for (c, e) in t.iter_mut().enumerate() {
        *e = c as u8;
    }
    let from = b"ACBDGHKMNSRUTWVYacbdghkmnsrutwvy";
    let to = b"TGVHCDMKNSYAAWBRTGVHCDMKNSYAAWBR";
    for (f, c) in from.iter().zip(to.iter()) {
        t[*f as usize] = *c;
    }
    t
}

// in place: two indexes meet in the middle
fn reverse_complement(seq: &mut [u8], t: &[u8; 256]) {
    if seq.is_empty() {
        return;
    }
    let (mut i, mut j) = (0usize, seq.len() - 1);
    while i < j {
        let (a, b) = (seq[i], seq[j]);
        seq[i] = t[b as usize];
        seq[j] = t[a as usize];
        i += 1;
        j -= 1;
    }
    if i == j {
        seq[i] = t[seq[i] as usize];
    }
}

fn hash_seq(mut h: u64, seq: &[u8]) -> u64 {
    for &c in seq {
        h = (h * 31 + c as u64) & 0xFFFF_FFFF;
    }
    h
}

fn revcomp(n: usize) -> u64 {
    let t = comp_table();
    let mut seed = 42i64;
    let mut iub = vec![0.27, 0.12, 0.12, 0.27];
    iub.extend(std::iter::repeat(0.02).take(11));
    let homo = [0.3029549426680, 0.1979883004921, 0.1975473066391, 0.3015094502008];
    let mut one = repeat_section(2 * n);
    let mut two = random_section(&mut seed, b"acgtBDHKMNRSVWY", &iub, 3 * n);
    let mut three = random_section(&mut seed, b"acgt", &homo, 5 * n);
    reverse_complement(&mut one, &t);
    reverse_complement(&mut two, &t);
    reverse_complement(&mut three, &t);
    let h = hash_seq(0, &one);
    let h = hash_seq(h, &two);
    hash_seq(h, &three)
}

fn main() {
    println!("{}", revcomp(2500000));
}
