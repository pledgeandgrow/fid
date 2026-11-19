// regex (BM4): see bench/work/fides/regex.fd. A hand-written Thompson NFA:
// a recursive-descent parser builds the states, a simulation over state
// lists counts the lines with a match. No crates.
struct Gen {
    s: i64,
}

impl Gen {
    fn rnd(&mut self) -> i64 {
        self.s = (self.s * 1103515245 + 12345) % 2147483648;
        self.s >> 8
    }
}

const DICT: [&[u8]; 8] = [b"cat", b"dogs", b"fox", b"bird", b"king", b"queen", b"xyz", b"banana"];

fn gen_text(lines: i64) -> Vec<u8> {
    let mut g = Gen { s: 1 };
    let mut o = Vec::new();
    for _ in 0..lines {
        let w = 3 + g.rnd() % 8;
        for j in 0..w {
            if j > 0 {
                o.push(b' ');
            }
            let k = g.rnd() % 10;
            if k < 6 {
                let n = 2 + g.rnd() % 7;
                for _ in 0..n {
                    o.push(97 + (g.rnd() % 26) as u8);
                }
            } else if k < 8 {
                let n = 1 + g.rnd() % 5;
                for _ in 0..n {
                    o.push(48 + (g.rnd() % 10) as u8);
                }
            } else if k == 8 {
                o.extend_from_slice(DICT[(g.rnd() % 8) as usize]);
            } else {
                let n = 1 + g.rnd() % 3;
                for _ in 0..n {
                    o.push(48 + (g.rnd() % 10) as u8);
                }
                o.push(b'-');
                let n = 1 + g.rnd() % 4;
                for _ in 0..n {
                    o.push(48 + (g.rnd() % 10) as u8);
                }
            }
        }
        o.push(b'\n');
    }
    o
}

const PATTERNS: [&[u8]; 8] = [
    b"ab+c",
    b"(cat|dogs?|fox) [a-z]+",
    b"[0-9][0-9]-[0-9]+",
    b"x.*y.*z",
    b"q[^u]",
    b"[aeiou][aeiou][aeiou]",
    b"k(e|i)?ng",
    b"\\d+-\\d\\d\\d\\d",
];

// state kinds: 0 byte (arg), 1 any but newline, 2 class (arg), 3 split,
// 4 empty, 5 match
const CHAR: i64 = 0;
const ANY: i64 = 1;
const CLASS: i64 = 2;
const SPLIT: i64 = 3;
const EPS: i64 = 4;
const MATCH: i64 = 5;

struct Nfa {
    kind: Vec<i64>,
    arg: Vec<i64>,
    out1: Vec<i64>,
    out2: Vec<i64>,
    cls: Vec<u8>,
}

impl Nfa {
    fn st(&mut self, k: i64, a: i64, o1: i64, o2: i64) -> i64 {
        self.kind.push(k);
        self.arg.push(a);
        self.out1.push(o1);
        self.out2.push(o2);
        self.kind.len() as i64 - 1
    }

    fn new_class(&mut self) -> i64 {
        let id = (self.cls.len() / 256) as i64;
        self.cls.resize(self.cls.len() + 256, 0);
        id
    }

    fn class_add_esc(&mut self, base: usize, e: u8) {
        match e {
            b'd' => {
                for c in 48..58 {
                    self.cls[base + c] = 1;
                }
            }
            b'w' => {
                for c in 0..256usize {
                    let b = c as u8;
                    if b.is_ascii_alphanumeric() || b == b'_' {
                        self.cls[base + c] = 1;
                    }
                }
            }
            b's' => {
                for c in [32usize, 9, 10, 13] {
                    self.cls[base + c] = 1;
                }
            }
            _ => self.cls[base + e as usize] = 1,
        }
    }
}

struct Compiler<'a> {
    p: &'a [u8],
    pos: usize,
    n: Nfa,
}

// a fragment: (start state, end state); the end is an empty state whose
// out1 is patched when the fragment is joined to what follows
impl<'a> Compiler<'a> {
    fn atom(&mut self) -> (i64, i64) {
        let c = self.p[self.pos];
        if c == b'(' {
            self.pos += 1;
            let f = self.alt();
            self.pos += 1;
            return f;
        }
        let e = self.n.st(EPS, 0, -1, -1);
        if c == b'.' {
            self.pos += 1;
            return (self.n.st(ANY, 0, e, -1), e);
        }
        if c == b'[' {
            self.pos += 1;
            let id = self.n.new_class();
            let base = id as usize * 256;
            let mut neg = false;
            if self.p[self.pos] == b'^' {
                neg = true;
                self.pos += 1;
            }
            while self.p[self.pos] != b']' {
                let a = self.p[self.pos];
                if a == b'\\' {
                    let e2 = self.p[self.pos + 1];
                    self.n.class_add_esc(base, e2);
                    self.pos += 2;
                } else if self.pos + 2 < self.p.len() && self.p[self.pos + 1] == b'-' && self.p[self.pos + 2] != b']' {
                    let b = self.p[self.pos + 2];
                    for x in a..=b {
                        self.n.cls[base + x as usize] = 1;
                    }
                    self.pos += 3;
                } else {
                    self.n.cls[base + a as usize] = 1;
                    self.pos += 1;
                }
            }
            self.pos += 1;
            if neg {
                for x in 0..256 {
                    self.n.cls[base + x] = 1 - self.n.cls[base + x];
                }
                self.n.cls[base + 10] = 0;
            }
            return (self.n.st(CLASS, id, e, -1), e);
        }
        if c == b'\\' {
            let esc = self.p[self.pos + 1];
            self.pos += 2;
            if esc == b'd' || esc == b'w' || esc == b's' {
                let id = self.n.new_class();
                self.n.class_add_esc(id as usize * 256, esc);
                return (self.n.st(CLASS, id, e, -1), e);
            }
            return (self.n.st(CHAR, esc as i64, e, -1), e);
        }
        self.pos += 1;
        (self.n.st(CHAR, c as i64, e, -1), e)
    }

    fn repeat(&mut self) -> (i64, i64) {
        let mut f = self.atom();
        while self.pos < self.p.len() && matches!(self.p[self.pos], b'*' | b'+' | b'?') {
            let op = self.p[self.pos];
            self.pos += 1;
            let e = self.n.st(EPS, 0, -1, -1);
            let s = self.n.st(SPLIT, 0, f.0, e);
            if op == b'*' {
                self.n.out1[f.1 as usize] = s;
                f = (s, e);
            } else if op == b'+' {
                self.n.out1[f.1 as usize] = s;
                f = (f.0, e);
            } else {
                self.n.out1[f.1 as usize] = e;
                f = (s, e);
            }
        }
        f
    }

    fn concat(&mut self) -> (i64, i64) {
        let mut f: Option<(i64, i64)> = None;
        while self.pos < self.p.len() && self.p[self.pos] != b'|' && self.p[self.pos] != b')' {
            let g = self.repeat();
            f = Some(match f {
                None => g,
                Some(f) => {
                    self.n.out1[f.1 as usize] = g.0;
                    (f.0, g.1)
                }
            });
        }
        match f {
            Some(f) => f,
            None => {
                let e = self.n.st(EPS, 0, -1, -1);
                (e, e)
            }
        }
    }

    fn alt(&mut self) -> (i64, i64) {
        let mut f = self.concat();
        while self.pos < self.p.len() && self.p[self.pos] == b'|' {
            self.pos += 1;
            let g = self.concat();
            let e = self.n.st(EPS, 0, -1, -1);
            let s = self.n.st(SPLIT, 0, f.0, g.0);
            self.n.out1[f.1 as usize] = e;
            self.n.out1[g.1 as usize] = e;
            f = (s, e);
        }
        f
    }
}

fn compile(p: &[u8]) -> (Nfa, i64) {
    let mut c = Compiler {
        p,
        pos: 0,
        n: Nfa { kind: Vec::new(), arg: Vec::new(), out1: Vec::new(), out2: Vec::new(), cls: Vec::new() },
    };
    let f = c.alt();
    let m = c.n.st(MATCH, 0, -1, -1);
    c.n.out1[f.1 as usize] = m;
    (c.n, f.0)
}

struct Sim {
    mark: Vec<i64>,
    gen: i64,
    matched: bool,
}

fn addstate(n: &Nfa, sim: &mut Sim, list: &mut Vec<i64>, s: i64) {
    if s < 0 || sim.mark[s as usize] == sim.gen {
        return;
    }
    sim.mark[s as usize] = sim.gen;
    let k = n.kind[s as usize];
    if k == SPLIT {
        addstate(n, sim, list, n.out1[s as usize]);
        addstate(n, sim, list, n.out2[s as usize]);
    } else if k == EPS {
        addstate(n, sim, list, n.out1[s as usize]);
    } else if k == MATCH {
        sim.matched = true;
    } else {
        list.push(s);
    }
}

fn match_line(n: &Nfa, start: i64, sim: &mut Sim, cl: &mut Vec<i64>, nl: &mut Vec<i64>, t: &[u8], a: usize, b: usize) -> bool {
    sim.gen += 1;
    sim.matched = false;
    cl.clear();
    let mut i = a;
    loop {
        addstate(n, sim, cl, start);
        if sim.matched {
            return true;
        }
        if i == b {
            return false;
        }
        let c = t[i];
        sim.gen += 1;
        nl.clear();
        for &s in cl.iter() {
            let k = n.kind[s as usize];
            let arg = n.arg[s as usize];
            if (k == CHAR && arg == c as i64) || (k == ANY && c != b'\n') || (k == CLASS && n.cls[arg as usize * 256 + c as usize] == 1) {
                addstate(n, sim, nl, n.out1[s as usize]);
            }
        }
        if sim.matched {
            return true;
        }
        std::mem::swap(cl, nl);
        i += 1;
    }
}

fn count(p: &[u8], t: &[u8]) -> i64 {
    let (n, start) = compile(p);
    let mut sim = Sim { mark: vec![0; n.kind.len()], gen: 0, matched: false };
    let mut cl = Vec::new();
    let mut nl = Vec::new();
    let mut cnt = 0;
    let mut a = 0;
    while a < t.len() {
        let mut b = a;
        while t[b] != b'\n' {
            b += 1;
        }
        if match_line(&n, start, &mut sim, &mut cl, &mut nl, t, a, b) {
            cnt += 1;
        }
        a = b + 1;
    }
    cnt
}

fn main() {
    let lines = std::hint::black_box(200000);
    let t = gen_text(lines);
    let mut total: i64 = 0;
    for (k, p) in PATTERNS.iter().enumerate() {
        total += (k as i64 + 1) * count(p, &t);
    }
    println!("{}", total);
}
