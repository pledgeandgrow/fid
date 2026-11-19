// pidigits (Benchmarks Game, BM3): see bench/game/fides/pidigits.fd.
// Rust's standard library has no big integers (and run.sh fetches no
// crates): a small signed one here, 32-bit limbs, with what the spigot needs
// (add, multiply by a word, compare, a quotient below 10 by subtraction).
use std::cmp::Ordering;

#[derive(Clone, Default)]
struct Big {
    neg: bool,
    mag: Vec<u32>, // little-endian, no high zero limbs; zero is empty
}

fn cmp_mag(a: &[u32], b: &[u32]) -> Ordering {
    if a.len() != b.len() {
        return a.len().cmp(&b.len());
    }
    for i in (0..a.len()).rev() {
        if a[i] != b[i] {
            return a[i].cmp(&b[i]);
        }
    }
    Ordering::Equal
}

fn trim(a: &mut Vec<u32>) {
    while a.last() == Some(&0) {
        a.pop();
    }
}

// a += b
fn add_mag(a: &mut Vec<u32>, b: &[u32]) {
    if a.len() < b.len() {
        a.resize(b.len(), 0);
    }
    let mut carry = 0u64;
    for i in 0..a.len() {
        if i >= b.len() && carry == 0 {
            break;
        }
        let s = a[i] as u64 + if i < b.len() { b[i] as u64 } else { 0 } + carry;
        a[i] = s as u32;
        carry = s >> 32;
    }
    if carry != 0 {
        a.push(carry as u32);
    }
}

// a -= b, a >= b
fn sub_mag(a: &mut Vec<u32>, b: &[u32]) {
    let mut borrow = 0i64;
    for i in 0..a.len() {
        if i >= b.len() && borrow == 0 {
            break;
        }
        let d = a[i] as i64 - if i < b.len() { b[i] as i64 } else { 0 } - borrow;
        if d < 0 {
            a[i] = (d + (1i64 << 32)) as u32;
            borrow = 1;
        } else {
            a[i] = d as u32;
            borrow = 0;
        }
    }
    trim(a);
}

// a = b - a, b > a
fn rsub_mag(a: &mut Vec<u32>, b: &[u32]) {
    a.resize(b.len(), 0);
    let mut borrow = 0i64;
    for i in 0..b.len() {
        let d = b[i] as i64 - a[i] as i64 - borrow;
        if d < 0 {
            a[i] = (d + (1i64 << 32)) as u32;
            borrow = 1;
        } else {
            a[i] = d as u32;
            borrow = 0;
        }
    }
    trim(a);
}

impl Big {
    fn from(v: u32) -> Big {
        let mut b = Big::default();
        if v != 0 {
            b.mag.push(v);
        }
        b
    }

    // self += b, or -= b when b_neg
    fn add(&mut self, b_neg: bool, b: &[u32]) {
        if self.mag.is_empty() {
            self.neg = b_neg;
        }
        if self.neg == b_neg {
            add_mag(&mut self.mag, b);
        } else if cmp_mag(&self.mag, b) != Ordering::Less {
            sub_mag(&mut self.mag, b);
        } else {
            rsub_mag(&mut self.mag, b);
            self.neg = b_neg;
        }
        if self.mag.is_empty() {
            self.neg = false;
        }
    }

    fn mul_small(&mut self, m: u32) {
        if m == 0 {
            self.mag.clear();
            self.neg = false;
            return;
        }
        let mut carry = 0u64;
        for x in self.mag.iter_mut() {
            let p = *x as u64 * m as u64 + carry;
            *x = p as u32;
            carry = p >> 32;
        }
        if carry != 0 {
            self.mag.push(carry as u32);
        }
    }

    fn gt(&self, b: &Big) -> bool {
        match (self.neg, b.neg) {
            (false, true) => true,
            (true, false) => false,
            (false, false) => cmp_mag(&self.mag, &b.mag) == Ordering::Greater,
            (true, true) => cmp_mag(&self.mag, &b.mag) == Ordering::Less,
        }
    }
}

// (num * nth + acc) / den, the quotient being a digit (num * nth + acc >= 0)
fn extract(num: &Big, acc: &Big, den: &Big, nth: u32, t: &mut Big) -> u32 {
    t.clone_from(num);
    t.mul_small(nth);
    t.add(acc.neg, &acc.mag);
    let mut q = 0;
    while cmp_mag(&t.mag, &den.mag) != Ordering::Less {
        sub_mag(&mut t.mag, &den.mag);
        q += 1;
    }
    q
}

fn pidigits(count: usize) -> u64 {
    let (mut num, mut acc, mut den) = (Big::from(1), Big::from(0), Big::from(1));
    let mut t = Big::default();
    let (mut k, mut i, mut h) = (0u32, 0usize, 0u64);
    while i < count {
        k += 1;
        let k2 = 2 * k + 1;
        t.clone_from(&num);
        t.mul_small(2);
        acc.add(false, &t.mag);
        acc.mul_small(k2);
        den.mul_small(k2);
        num.mul_small(k);
        if num.gt(&acc) {
            continue;
        }
        let d = extract(&num, &acc, &den, 3, &mut t);
        if d != extract(&num, &acc, &den, 4, &mut t) {
            continue;
        }
        h = (h * 31 + d as u64) & 0xFFFF_FFFF;
        i += 1;
        t.clone_from(&den);
        t.mul_small(d);
        acc.add(true, &t.mag);
        acc.mul_small(10);
        num.mul_small(10);
    }
    h
}

fn main() {
    println!("{}", pidigits(10000));
}
