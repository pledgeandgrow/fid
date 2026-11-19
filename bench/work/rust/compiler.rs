// compiler (BM4, real work): see bench/work/fides/compiler.fd.
enum Expr {
    Num(i64),
    Var(usize),
    Bin(u8, Box<Expr>, Box<Expr>),
}

fn rnd(s: &mut i64) -> i64 {
    *s = (*s * 1103515245 + 12345) % 2147483648;
    *s >> 8
}

fn put_dec(out: &mut Vec<u8>, v: i64) {
    if v >= 10 {
        put_dec(out, v / 10);
    }
    out.push(b'0' + (v % 10) as u8);
}

fn gen_expr(out: &mut Vec<u8>, s: &mut i64, d: u32, k: i64) {
    let r = rnd(s);
    if d == 0 || r % 4 == 0 {
        let r2 = rnd(s);
        if k > 0 && r2 % 5 == 0 {
            out.push(b'v');
            put_dec(out, r2 % k);
        } else {
            put_dec(out, r2 % 1000);
        }
        return;
    }
    out.push(b'(');
    gen_expr(out, s, d - 1, k);
    let op = [b'+', b'-', b'*'][(rnd(s) % 3) as usize];
    out.extend_from_slice(&[b' ', op, b' ']);
    gen_expr(out, s, d - 1, k);
    out.push(b')');
}

fn generate(lines: i64, depth: u32) -> Vec<u8> {
    let mut out = Vec::new();
    let mut s = 1i64;
    for k in 0..lines {
        out.extend_from_slice(b"let v");
        put_dec(&mut out, k);
        out.extend_from_slice(b" = ");
        gen_expr(&mut out, &mut s, depth, k);
        out.extend_from_slice(b";\n");
    }
    out
}

// token kinds: 1 number, 2 variable, 3 `let`, 4 punctuation (its byte)
fn lex(src: &[u8]) -> (Vec<u8>, Vec<i64>) {
    let (mut kinds, mut vals) = (Vec::new(), Vec::new());
    let n = src.len();
    let mut i = 0;
    while i < n {
        let c = src[i];
        if c == b' ' || c == b'\n' {
            i += 1;
        } else if c.is_ascii_digit() || c == b'v' {
            let mut kind = 1;
            if c == b'v' {
                kind = 2;
                i += 1;
            }
            let mut v = 0i64;
            while i < n && src[i].is_ascii_digit() {
                v = v * 10 + (src[i] - b'0') as i64;
                i += 1;
            }
            kinds.push(kind);
            vals.push(v);
        } else if c == b'l' {
            i += 3;
            kinds.push(3);
            vals.push(0);
        } else {
            kinds.push(4);
            vals.push(c as i64);
            i += 1;
        }
    }
    kinds.push(0);
    vals.push(0);
    (kinds, vals)
}

struct Parser<'a> {
    kinds: &'a [u8],
    vals: &'a [i64],
    pos: usize,
}

impl<'a> Parser<'a> {
    fn is_op(&self, op: u8) -> bool {
        self.kinds[self.pos] == 4 && self.vals[self.pos] == op as i64
    }
    fn atom(&mut self) -> Box<Expr> {
        let p = self.pos;
        self.pos += 1;
        match self.kinds[p] {
            1 => Box::new(Expr::Num(self.vals[p])),
            2 => Box::new(Expr::Var(self.vals[p] as usize)),
            _ => {
                let e = self.expr();
                self.pos += 1;
                e
            }
        }
    }
    fn term(&mut self) -> Box<Expr> {
        let mut a = self.atom();
        while self.is_op(b'*') {
            self.pos += 1;
            let b = self.atom();
            a = Box::new(Expr::Bin(b'*', a, b));
        }
        a
    }
    fn expr(&mut self) -> Box<Expr> {
        let mut a = self.term();
        while self.is_op(b'+') || self.is_op(b'-') {
            let op = self.vals[self.pos] as u8;
            self.pos += 1;
            let b = self.term();
            a = Box::new(Expr::Bin(op, a, b));
        }
        a
    }
}

fn eval(e: &Expr, env: &[i64]) -> i64 {
    match e {
        Expr::Num(n) => *n,
        Expr::Var(i) => env[*i],
        Expr::Bin(op, l, r) => {
            let (a, b) = (eval(l, env), eval(r, env));
            match op {
                b'+' => (a + b) % 1000003,
                b'-' => (a - b + 1000003) % 1000003,
                _ => (a * b) % 1000003,
            }
        }
    }
}

fn compile_run(src: &[u8]) -> i64 {
    let (kinds, vals) = lex(src);
    let mut p = Parser { kinds: &kinds, vals: &vals, pos: 0 };
    let mut env = Vec::new();
    let mut total = 0i64;
    while p.kinds[p.pos] == 3 {
        p.pos += 3;
        let e = p.expr();
        p.pos += 1;
        let x = eval(&e, &env);
        env.push(x);
        total += x;
    }
    total
}

fn main() {
    println!("{}", compile_run(&generate(50000, 8)));
}
