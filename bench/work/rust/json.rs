// json (BM4): see bench/work/fides/json.fd. A hand-written parser into an
// arena of nodes and a compact printer, no crates.
struct Gen {
    s: i64,
}

impl Gen {
    fn rnd(&mut self) -> i64 {
        self.s = (self.s * 1103515245 + 12345) % 2147483648;
        self.s >> 8
    }
}

fn put_int(o: &mut Vec<u8>, v: i64) {
    o.extend_from_slice(v.to_string().as_bytes());
}

fn gen_string(o: &mut Vec<u8>, g: &mut Gen) {
    o.push(b'"');
    let n = g.rnd() % 12;
    for _ in 0..n {
        let c = g.rnd() % 40;
        if c < 26 {
            o.push(97 + c as u8);
        } else if c == 26 {
            o.extend_from_slice(b"\\n");
        } else if c == 27 {
            o.extend_from_slice(b"\\\"");
        } else if c == 28 {
            o.extend_from_slice(b"\\\\");
        } else if c == 29 {
            o.extend_from_slice(b"\\u00e9");
        } else if c == 30 {
            o.extend_from_slice(b"\\t");
        } else if c == 31 {
            o.extend_from_slice(b"\\u0041");
        } else if c < 36 {
            o.push(b' ');
        } else {
            o.push(48 + (c - 36) as u8);
        }
    }
    o.push(b'"');
}

fn gen_value(o: &mut Vec<u8>, g: &mut Gen, d: i64) {
    let mut r = g.rnd() % 8;
    if d == 0 {
        r %= 5;
    }
    if r == 0 {
        let v = g.rnd() % 2000000 - 1000000;
        put_int(o, v);
    } else if r == 1 || r == 4 {
        gen_string(o, g);
    } else if r == 2 {
        let t = g.rnd() % 3;
        o.extend_from_slice(match t {
            0 => b"true".as_slice(),
            1 => b"false".as_slice(),
            _ => b"null".as_slice(),
        });
    } else if r == 3 {
        let v = g.rnd() % 100;
        put_int(o, v);
    } else if r == 5 || r == 6 {
        let n = g.rnd() % 5;
        o.push(b'[');
        for i in 0..n {
            if i > 0 {
                o.extend_from_slice(b", ");
            }
            gen_value(o, g, d - 1);
        }
        o.push(b']');
    } else {
        let n = g.rnd() % 5;
        o.push(b'{');
        for i in 0..n {
            if i > 0 {
                o.extend_from_slice(b",\n  ");
            }
            gen_string(o, g);
            o.extend_from_slice(b": ");
            gen_value(o, g, d - 1);
        }
        o.push(b'}');
    }
}

fn generate(items: i64, depth: i64) -> Vec<u8> {
    let mut g = Gen { s: 1 };
    let mut o = Vec::new();
    o.push(b'[');
    for i in 0..items {
        if i > 0 {
            o.extend_from_slice(b",\n");
        }
        gen_value(&mut o, &mut g, depth);
    }
    o.push(b']');
    o.push(b'\n');
    o
}

// node kinds: 0 null, 1 false, 2 true, 3 number, 4 string, 5 array, 6 object
struct Doc {
    kind: Vec<i64>,
    num: Vec<i64>,
    first: Vec<i64>,
    next: Vec<i64>,
    key: Vec<i64>,
    pool: Vec<u8>,
    ps: Vec<i64>,
    pl: Vec<i64>,
}

struct Parser<'a> {
    t: &'a [u8],
    pos: usize,
    d: Doc,
}

impl<'a> Parser<'a> {
    fn ws(&mut self) {
        while matches!(self.t[self.pos], b' ' | b'\n' | b'\r' | b'\t') {
            self.pos += 1;
        }
    }

    fn node(&mut self, k: i64, v: i64) -> i64 {
        self.d.kind.push(k);
        self.d.num.push(v);
        self.d.first.push(-1);
        self.d.next.push(-1);
        self.d.key.push(-1);
        self.d.kind.len() as i64 - 1
    }

    fn string(&mut self) -> i64 {
        self.pos += 1;
        let st = self.d.pool.len();
        loop {
            let c = self.t[self.pos];
            if c == b'"' {
                self.pos += 1;
                break;
            }
            if c == b'\\' {
                let e = self.t[self.pos + 1];
                self.pos += 2;
                match e {
                    b'n' => self.d.pool.push(10),
                    b't' => self.d.pool.push(9),
                    b'r' => self.d.pool.push(13),
                    b'b' => self.d.pool.push(8),
                    b'f' => self.d.pool.push(12),
                    b'u' => {
                        let mut cp: i64 = 0;
                        for _ in 0..4 {
                            let h = self.t[self.pos] as i64;
                            self.pos += 1;
                            let v = if (48..=57).contains(&h) {
                                h - 48
                            } else if (97..=102).contains(&h) {
                                h - 87
                            } else {
                                h - 55
                            };
                            cp = cp * 16 + v;
                        }
                        if cp < 128 {
                            self.d.pool.push(cp as u8);
                        } else if cp < 2048 {
                            self.d.pool.push((192 | (cp >> 6)) as u8);
                            self.d.pool.push((128 | (cp & 63)) as u8);
                        } else {
                            self.d.pool.push((224 | (cp >> 12)) as u8);
                            self.d.pool.push((128 | ((cp >> 6) & 63)) as u8);
                            self.d.pool.push((128 | (cp & 63)) as u8);
                        }
                    }
                    _ => self.d.pool.push(e),
                }
            } else {
                self.d.pool.push(c);
                self.pos += 1;
            }
        }
        self.d.ps.push(st as i64);
        self.d.pl.push((self.d.pool.len() - st) as i64);
        self.d.ps.len() as i64 - 1
    }

    fn value(&mut self) -> i64 {
        self.ws();
        let c = self.t[self.pos];
        if c == b'{' || c == b'[' {
            let close = if c == b'{' { b'}' } else { b']' };
            let n = self.node(if c == b'{' { 6 } else { 5 }, 0);
            self.pos += 1;
            let mut prev: i64 = -1;
            self.ws();
            if self.t[self.pos] == close {
                self.pos += 1;
                return n;
            }
            loop {
                let mut k: i64 = -1;
                if c == b'{' {
                    self.ws();
                    k = self.string();
                    self.ws();
                    self.pos += 1;
                }
                let ch = self.value();
                self.d.key[ch as usize] = k;
                if prev < 0 {
                    self.d.first[n as usize] = ch;
                } else {
                    self.d.next[prev as usize] = ch;
                }
                prev = ch;
                self.ws();
                if self.t[self.pos] == b',' {
                    self.pos += 1;
                } else {
                    self.pos += 1;
                    return n;
                }
            }
        }
        if c == b'"' {
            let s = self.string();
            return self.node(4, s);
        }
        if c == b't' {
            self.pos += 4;
            return self.node(2, 0);
        }
        if c == b'f' {
            self.pos += 5;
            return self.node(1, 0);
        }
        if c == b'n' {
            self.pos += 4;
            return self.node(0, 0);
        }
        let mut neg = false;
        if c == b'-' {
            neg = true;
            self.pos += 1;
        }
        let mut v: i64 = 0;
        while self.t[self.pos].is_ascii_digit() {
            v = v * 10 + (self.t[self.pos] - 48) as i64;
            self.pos += 1;
        }
        self.node(3, if neg { -v } else { v })
    }
}

const HEX: &[u8] = b"0123456789abcdef";

fn print_str(o: &mut Vec<u8>, d: &Doc, sid: i64) {
    o.push(b'"');
    let st = d.ps[sid as usize] as usize;
    let len = d.pl[sid as usize] as usize;
    for &b in &d.pool[st..st + len] {
        match b {
            b'"' => o.extend_from_slice(b"\\\""),
            b'\\' => o.extend_from_slice(b"\\\\"),
            b'\n' => o.extend_from_slice(b"\\n"),
            b'\t' => o.extend_from_slice(b"\\t"),
            0..=31 => {
                o.extend_from_slice(b"\\u00");
                o.push(HEX[(b >> 4) as usize]);
                o.push(HEX[(b & 15) as usize]);
            }
            _ => o.push(b),
        }
    }
    o.push(b'"');
}

fn print(o: &mut Vec<u8>, d: &Doc, n: i64) {
    let k = d.kind[n as usize];
    match k {
        0 => o.extend_from_slice(b"null"),
        1 => o.extend_from_slice(b"false"),
        2 => o.extend_from_slice(b"true"),
        3 => put_int(o, d.num[n as usize]),
        4 => print_str(o, d, d.num[n as usize]),
        _ => {
            o.push(if k == 5 { b'[' } else { b'{' });
            let mut c = d.first[n as usize];
            let mut f = true;
            while c >= 0 {
                if !f {
                    o.push(b',');
                }
                f = false;
                if k == 6 {
                    print_str(o, d, d.key[c as usize]);
                    o.push(b':');
                }
                print(o, d, c);
                c = d.next[c as usize];
            }
            o.push(if k == 5 { b']' } else { b'}' });
        }
    }
}

fn run(items: i64, depth: i64, rounds: i64) -> i64 {
    let mut text = generate(items, depth);
    let mut total: i64 = 0;
    for _ in 0..rounds {
        let mut p = Parser {
            t: &text,
            pos: 0,
            d: Doc {
                kind: Vec::new(),
                num: Vec::new(),
                first: Vec::new(),
                next: Vec::new(),
                key: Vec::new(),
                pool: Vec::new(),
                ps: Vec::new(),
                pl: Vec::new(),
            },
        };
        let root = p.value();
        let d = p.d;
        let mut o = Vec::new();
        print(&mut o, &d, root);
        let mut h: i64 = 0;
        for &b in &o {
            h = (h * 31 + b as i64) % 1000000007;
        }
        total += h + d.kind.len() as i64;
        text = o;
    }
    total
}

fn main() {
    let items = std::hint::black_box(500000);
    println!("{}", run(items, 6, 3));
}
