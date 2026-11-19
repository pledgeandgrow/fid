// regex-redux (Benchmarks Game, BM3): see bench/game/fides/regexredux.fd.
// Rust's standard library has no regular expressions (and run.sh fetches no
// crates), so this is the same small backtracking matcher every language of
// bench/game/ runs.
const ALU: &[u8] = b"GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGGGAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGACCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAATACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCAGCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGGAGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCCAGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA";

struct Item {
    set: [bool; 256],
    star: bool,
}

struct Pat {
    alts: Vec<Vec<Item>>,
    first: [bool; 256],
}

fn compile(pattern: &str) -> Pat {
    let b = pattern.as_bytes();
    let mut alts = vec![Vec::new()];
    let mut i = 0;
    while i < b.len() {
        let c = b[i];
        if c == b'|' {
            alts.push(Vec::new());
            i += 1;
            continue;
        }
        let mut set = [false; 256];
        if c == b'[' {
            i += 1;
            let neg = b[i] == b'^';
            if neg {
                i += 1;
            }
            while b[i] != b']' {
                set[b[i] as usize] = true;
                i += 1;
            }
            i += 1;
            if neg {
                for e in set.iter_mut() {
                    *e = !*e;
                }
            }
        } else if c == b'\\' {
            set[b[i + 1] as usize] = true;
            i += 2;
        } else if c == b'.' {
            for (z, e) in set.iter_mut().enumerate() {
                *e = z != b'\n' as usize;
            }
            i += 1;
        } else {
            set[c as usize] = true;
            i += 1;
        }
        let star = i < b.len() && b[i] == b'*';
        if star {
            i += 1;
        }
        alts.last_mut().unwrap().push(Item { set, star });
    }
    let mut first = [false; 256];
    for alt in &alts {
        for z in 0..256 {
            if alt[0].star || alt[0].set[z] {
                first[z] = true;
            }
        }
    }
    Pat { alts, first }
}

// where a match of items that starts at pos ends
fn match_at(items: &[Item], s: &[u8], pos: usize) -> Option<usize> {
    let Some(it) = items.first() else {
        return Some(pos);
    };
    if it.star {
        let mut j = pos;
        while j < s.len() && it.set[s[j] as usize] {
            j += 1;
        }
        loop {
            if let Some(e) = match_at(&items[1..], s, j) {
                return Some(e);
            }
            if j == pos {
                return None;
            }
            j -= 1;
        }
    }
    if pos < s.len() && it.set[s[pos] as usize] {
        return match_at(&items[1..], s, pos + 1);
    }
    None
}

// where the match that starts at pos ends (the first alternative that matches)
fn find_at(p: &Pat, s: &[u8], pos: usize) -> Option<usize> {
    if !p.first[s[pos] as usize] {
        return None;
    }
    for alt in &p.alts {
        if let Some(e) = match_at(alt, s, pos) {
            if e > pos {
                return Some(e);
            }
        }
    }
    None
}

fn count_matches(pattern: &str, s: &[u8]) -> u64 {
    let p = compile(pattern);
    let (mut n, mut pos) = (0, 0);
    while pos < s.len() {
        match find_at(&p, s, pos) {
            Some(e) => {
                n += 1;
                pos = e;
            }
            None => pos += 1,
        }
    }
    n
}

fn replace_all(pattern: &str, s: &[u8], rep: &str) -> Vec<u8> {
    let p = compile(pattern);
    let mut out = Vec::with_capacity(s.len());
    let mut pos = 0;
    while pos < s.len() {
        match find_at(&p, s, pos) {
            Some(e) => {
                out.extend_from_slice(rep.as_bytes());
                pos = e;
            }
            None => {
                out.push(s[pos]);
                pos += 1;
            }
        }
    }
    out
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

// a header line, then the section in lines of 60 characters
fn put_section(out: &mut Vec<u8>, title: &str, sec: &[u8]) {
    out.extend_from_slice(title.as_bytes());
    out.push(b'\n');
    for line in sec.chunks(60) {
        out.extend_from_slice(line);
        out.push(b'\n');
    }
}

fn fasta_text(n: usize) -> Vec<u8> {
    let mut seed = 42i64;
    let mut iub = vec![0.27, 0.12, 0.12, 0.27];
    iub.extend(std::iter::repeat(0.02).take(11));
    let homo = [0.3029549426680, 0.1979883004921, 0.1975473066391, 0.3015094502008];
    let one: Vec<u8> = (0..2 * n).map(|k| ALU[k % ALU.len()]).collect();
    let two = random_section(&mut seed, b"acgtBDHKMNRSVWY", &iub, 3 * n);
    let three = random_section(&mut seed, b"acgt", &homo, 5 * n);
    let mut out = Vec::new();
    put_section(&mut out, ">ONE Homo sapiens alu", &one);
    put_section(&mut out, ">TWO IUB ambiguity codes", &two);
    put_section(&mut out, ">THREE Homo sapiens frequency", &three);
    out
}

fn fold(h: u64, v: u64) -> u64 {
    (h * 31 + v) & 0xFFFF_FFFF
}

fn regexredux(n: usize) -> u64 {
    let text = fasta_text(n);
    let seq = replace_all(">.*\n|\n", &text, "");
    let mut h = 0;
    for v in [
        "agggtaaa|tttaccct",
        "[cgt]gggtaaa|tttaccc[acg]",
        "a[act]ggtaaa|tttacc[agt]t",
        "ag[act]gtaaa|tttac[agt]ct",
        "agg[act]taaa|ttta[agt]cct",
        "aggg[acg]aaa|ttt[cgt]ccct",
        "agggt[cgt]aa|tt[acg]accct",
        "agggta[cgt]a|t[acg]taccct",
        "agggtaa[cgt]|[acg]ttaccct",
    ] {
        h = fold(h, count_matches(v, &seq));
    }
    let mut s = seq.clone();
    for (pat, rep) in [
        ("tHa[Nt]", "<4>"),
        ("aND|caN|Ha[DS]|WaS", "<3>"),
        ("a[NSt]|BY", "<2>"),
        ("<[^>]*>", "|"),
        ("\\|[^|][^|]*\\|", "-"),
    ] {
        s = replace_all(pat, &s, rep);
    }
    h = fold(h, text.len() as u64);
    h = fold(h, seq.len() as u64);
    fold(h, s.len() as u64)
}

fn main() {
    println!("{}", regexredux(500000));
}
