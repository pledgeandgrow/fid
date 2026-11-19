// fileread (BM4): see bench/work/fides/fileread.fd. Reads a 50 MB text
// file three times and sums the numbers in it; writes the file first when
// it is not there.
use std::fs;

const PATH: &str = "out/fileread.dat";
const LINES: i64 = 2150000;
const SIZE: u64 = 49994158;

fn generate() {
    let mut s: i64 = 1;
    let mut o: Vec<u8> = Vec::with_capacity(SIZE as usize);
    for k in 0..LINES {
        s = (s * 1103515245 + 12345) % 2147483648;
        let r1 = s >> 8;
        s = (s * 1103515245 + 12345) % 2147483648;
        let r2 = s >> 8;
        o.extend_from_slice(format!("rec{} {} x{}\n", k, r1 % 1000000, r2 % 10000).as_bytes());
    }
    fs::write(PATH, &o).expect("cannot write out/fileread.dat");
}

fn scan(d: &[u8]) -> i64 {
    let mut tot: i64 = 0;
    let mut cur: i64 = 0;
    let mut ind = false;
    let mut lines: i64 = 0;
    for &b in d {
        if b.is_ascii_digit() {
            cur = cur * 10 + (b - 48) as i64;
            ind = true;
        } else {
            if ind {
                tot += cur;
                cur = 0;
                ind = false;
            }
            if b == b'\n' {
                lines += 1;
            }
        }
    }
    if ind {
        tot += cur;
    }
    tot + lines
}

fn main() {
    if fs::metadata(PATH).map(|m| m.len()).unwrap_or(0) != SIZE {
        generate();
    }
    let mut total: i64 = 0;
    for _ in 0..3 {
        let d = fs::read(PATH).expect("cannot read out/fileread.dat");
        total += scan(&d);
    }
    println!("{}", total);
}
