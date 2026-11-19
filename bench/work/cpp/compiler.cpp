// compiler (BM4, real work): see bench/work/fides/compiler.fd. Nodes are
// owned by std::unique_ptr, one allocation each, as in the other programs.
#include <cstdint>
#include <cstdio>
#include <memory>
#include <vector>

struct Expr {
    int kind;  // 0 number, 1 variable, 2 binary
    int64_t val;
    char op;
    std::unique_ptr<Expr> l, r;
};
using P = std::unique_ptr<Expr>;

static int64_t rnd(int64_t &s) {
    s = (s * 1103515245 + 12345) % 2147483648LL;
    return s >> 8;
}

static void put_dec(std::vector<char> &out, int64_t v) {
    if (v >= 10) put_dec(out, v / 10);
    out.push_back((char)('0' + v % 10));
}

static void gen_expr(std::vector<char> &out, int64_t &s, int d, int64_t k) {
    int64_t r = rnd(s);
    if (d == 0 || r % 4 == 0) {
        int64_t r2 = rnd(s);
        if (k > 0 && r2 % 5 == 0) {
            out.push_back('v');
            put_dec(out, r2 % k);
        } else {
            put_dec(out, r2 % 1000);
        }
        return;
    }
    out.push_back('(');
    gen_expr(out, s, d - 1, k);
    char op = "+-*"[rnd(s) % 3];
    out.push_back(' ');
    out.push_back(op);
    out.push_back(' ');
    gen_expr(out, s, d - 1, k);
    out.push_back(')');
}

static std::vector<char> generate(int64_t lines, int depth) {
    std::vector<char> out;
    int64_t s = 1;
    for (int64_t k = 0; k < lines; k++) {
        for (const char *t = "let v"; *t; t++) out.push_back(*t);
        put_dec(out, k);
        for (const char *t = " = "; *t; t++) out.push_back(*t);
        gen_expr(out, s, depth, k);
        out.push_back(';');
        out.push_back('\n');
    }
    return out;
}

// token kinds: 1 number, 2 variable, 3 `let`, 4 punctuation (its byte)
struct Parser {
    std::vector<char> kinds;
    std::vector<int64_t> vals;
    size_t pos = 0;
    bool is_op(char op) const { return kinds[pos] == 4 && vals[pos] == op; }
    P atom() {
        size_t q = pos++;
        if (kinds[q] == 1) return P(new Expr{0, vals[q], 0, nullptr, nullptr});
        if (kinds[q] == 2) return P(new Expr{1, vals[q], 0, nullptr, nullptr});
        P e = expr();
        pos++;
        return e;
    }
    P term() {
        P a = atom();
        while (is_op('*')) {
            pos++;
            P b = atom();
            a = P(new Expr{2, 0, '*', std::move(a), std::move(b)});
        }
        return a;
    }
    P expr() {
        P a = term();
        while (is_op('+') || is_op('-')) {
            char op = (char)vals[pos++];
            P b = term();
            a = P(new Expr{2, 0, op, std::move(a), std::move(b)});
        }
        return a;
    }
};

static void lex(const std::vector<char> &src, Parser &p) {
    size_t n = src.size(), i = 0;
    while (i < n) {
        char c = src[i];
        if (c == ' ' || c == '\n') {
            i++;
        } else if ((c >= '0' && c <= '9') || c == 'v') {
            char kind = 1;
            if (c == 'v') {
                kind = 2;
                i++;
            }
            int64_t v = 0;
            while (i < n && src[i] >= '0' && src[i] <= '9') v = v * 10 + (src[i++] - '0');
            p.kinds.push_back(kind);
            p.vals.push_back(v);
        } else if (c == 'l') {
            i += 3;
            p.kinds.push_back(3);
            p.vals.push_back(0);
        } else {
            p.kinds.push_back(4);
            p.vals.push_back(c);
            i++;
        }
    }
    p.kinds.push_back(0);
    p.vals.push_back(0);
}

static int64_t eval(const Expr *e, const std::vector<int64_t> &env) {
    if (e->kind == 0) return e->val;
    if (e->kind == 1) return env[e->val];
    int64_t a = eval(e->l.get(), env), b = eval(e->r.get(), env);
    if (e->op == '+') return (a + b) % 1000003;
    if (e->op == '-') return (a - b + 1000003) % 1000003;
    return (a * b) % 1000003;
}

static int64_t compile_run(const std::vector<char> &src) {
    Parser p;
    lex(src, p);
    std::vector<int64_t> env;
    int64_t total = 0;
    while (p.kinds[p.pos] == 3) {
        p.pos += 3;
        P e = p.expr();
        p.pos++;
        int64_t x = eval(e.get(), env);
        env.push_back(x);
        total += x;
    }
    return total;
}

int main() {
    printf("%lld\n", (long long)compile_run(generate(50000, 8)));
    return 0;
}
