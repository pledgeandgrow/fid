// fannkuch-redux (Benchmarks Game, BM3): see bench/game/fides/fannkuch.fd.
fn fannkuch(n: usize) -> i64 {
    let mut perm1: Vec<usize> = (0..n).collect();
    let mut count = vec![0usize; n];
    let mut perm = vec![0usize; n];
    let (mut maxflips, mut checksum, mut permcount) = (0i64, 0i64, 0i64);
    let mut r = n;
    loop {
        while r != 1 {
            count[r - 1] = r;
            r -= 1;
        }
        perm.copy_from_slice(&perm1);
        let mut flips = 0i64;
        let mut k = perm[0];
        while k != 0 {
            perm[..=k].reverse();
            flips += 1;
            k = perm[0];
        }
        maxflips = maxflips.max(flips);
        checksum += if permcount % 2 == 0 { flips } else { -flips };
        loop {
            if r == n {
                return checksum * 1000 + maxflips;
            }
            perm1[..=r].rotate_left(1);
            count[r] -= 1;
            if count[r] > 0 {
                break;
            }
            r += 1;
        }
        permcount += 1;
    }
}

fn main() {
    let n: usize = std::hint::black_box(11);
    println!("{}", fannkuch(n));
}
