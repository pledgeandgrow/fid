// regex (BM4): see bench/work/fides/regex.fd. A hand-written Thompson NFA,
// not std::regex.
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <utility>
#include <vector>

typedef std::vector<uint8_t> Buf;

static int64_t seed = 1;
static int64_t rnd() {
    seed = (seed * 1103515245 + 12345) % 2147483648LL;
    return seed >> 8;
}

static const char* DICT[8] = {"cat", "dogs", "fox", "bird", "king", "queen", "xyz", "banana"};

static Buf gen_text(int64_t lines) {
    Buf o;
    for (int64_t l = 0; l < lines; l++) {
        int64_t w = 3 + rnd() % 8;
        for (int64_t j = 0; j < w; j++) {
            if (j > 0) o.push_back(' ');
            int64_t k = rnd() % 10;
            if (k < 6) {
                int64_t n = 2 + rnd() % 7;
                for (int64_t i = 0; i < n; i++) o.push_back((uint8_t)(97 + rnd() % 26));
            } else if (k < 8) {
                int64_t n = 1 + rnd() % 5;
                for (int64_t i = 0; i < n; i++) o.push_back((uint8_t)(48 + rnd() % 10));
            } else if (k == 8) {
                const char* d = DICT[rnd() % 8];
                o.insert(o.end(), d, d + strlen(d));
            } else {
                int64_t n = 1 + rnd() % 3;
                for (int64_t i = 0; i < n; i++) o.push_back((uint8_t)(48 + rnd() % 10));
                o.push_back('-');
                n = 1 + rnd() % 4;
                for (int64_t i = 0; i < n; i++) o.push_back((uint8_t)(48 + rnd() % 10));
            }
        }
        o.push_back('\n');
    }
    return o;
}

static const char* PATTERNS[8] = {
    "ab+c",
    "(cat|dogs?|fox) [a-z]+",
    "[0-9][0-9]-[0-9]+",
    "x.*y.*z",
    "q[^u]",
    "[aeiou][aeiou][aeiou]",
    "k(e|i)?ng",
    "\\d+-\\d\\d\\d\\d",
};

// state kinds: 0 byte (arg), 1 any but newline, 2 class (arg), 3 split,
// 4 empty, 5 match
enum { S_CHAR, S_ANY, S_CLASS, S_SPLIT, S_EPS, S_MATCH };

struct Nfa {
    std::vector<int64_t> kind, arg, out1, out2;
    Buf cls;

    int64_t st(int64_t k, int64_t a, int64_t o1, int64_t o2) {
        kind.push_back(k);
        arg.push_back(a);
        out1.push_back(o1);
        out2.push_back(o2);
        return (int64_t)kind.size() - 1;
    }
    int64_t new_class() {
        int64_t id = (int64_t)(cls.size() / 256);
        cls.resize(cls.size() + 256, 0);
        return id;
    }
    void class_add_esc(size_t base, uint8_t e) {
        if (e == 'd') {
            for (int c = 48; c < 58; c++) cls[base + c] = 1;
        } else if (e == 'w') {
            for (int c = 0; c < 256; c++)
                if ((c >= 48 && c <= 57) || (c >= 65 && c <= 90) || (c >= 97 && c <= 122) || c == 95) cls[base + c] = 1;
        } else if (e == 's') {
            cls[base + 32] = 1;
            cls[base + 9] = 1;
            cls[base + 10] = 1;
            cls[base + 13] = 1;
        } else {
            cls[base + e] = 1;
        }
    }
};

typedef std::pair<int64_t, int64_t> Frag;  // start, end

struct Compiler {
    const char* p;
    size_t len;
    size_t pos = 0;
    Nfa n;

    Frag atom() {
        uint8_t c = (uint8_t)p[pos];
        if (c == '(') {
            pos++;
            Frag f = alt();
            pos++;
            return f;
        }
        int64_t e = n.st(S_EPS, 0, -1, -1);
        if (c == '.') {
            pos++;
            return Frag(n.st(S_ANY, 0, e, -1), e);
        }
        if (c == '[') {
            pos++;
            int64_t id = n.new_class();
            size_t base = (size_t)id * 256;
            bool neg = false;
            if (p[pos] == '^') {
                neg = true;
                pos++;
            }
            while (p[pos] != ']') {
                uint8_t a = (uint8_t)p[pos];
                if (a == '\\') {
                    n.class_add_esc(base, (uint8_t)p[pos + 1]);
                    pos += 2;
                } else if (pos + 2 < len && p[pos + 1] == '-' && p[pos + 2] != ']') {
                    uint8_t b = (uint8_t)p[pos + 2];
                    for (int x = a; x <= b; x++) n.cls[base + x] = 1;
                    pos += 3;
                } else {
                    n.cls[base + a] = 1;
                    pos++;
                }
            }
            pos++;
            if (neg) {
                for (int x = 0; x < 256; x++) n.cls[base + x] = 1 - n.cls[base + x];
                n.cls[base + 10] = 0;
            }
            return Frag(n.st(S_CLASS, id, e, -1), e);
        }
        if (c == '\\') {
            uint8_t esc = (uint8_t)p[pos + 1];
            pos += 2;
            if (esc == 'd' || esc == 'w' || esc == 's') {
                int64_t id = n.new_class();
                n.class_add_esc((size_t)id * 256, esc);
                return Frag(n.st(S_CLASS, id, e, -1), e);
            }
            return Frag(n.st(S_CHAR, esc, e, -1), e);
        }
        pos++;
        return Frag(n.st(S_CHAR, c, e, -1), e);
    }

    Frag repeat() {
        Frag f = atom();
        while (pos < len && (p[pos] == '*' || p[pos] == '+' || p[pos] == '?')) {
            char op = p[pos];
            pos++;
            int64_t e = n.st(S_EPS, 0, -1, -1);
            int64_t s = n.st(S_SPLIT, 0, f.first, e);
            if (op == '*') {
                n.out1[f.second] = s;
                f = Frag(s, e);
            } else if (op == '+') {
                n.out1[f.second] = s;
                f = Frag(f.first, e);
            } else {
                n.out1[f.second] = e;
                f = Frag(s, e);
            }
        }
        return f;
    }

    Frag concat() {
        bool have = false;
        Frag f(0, 0);
        while (pos < len && p[pos] != '|' && p[pos] != ')') {
            Frag g = repeat();
            if (!have) {
                f = g;
                have = true;
            } else {
                n.out1[f.second] = g.first;
                f = Frag(f.first, g.second);
            }
        }
        if (!have) {
            int64_t e = n.st(S_EPS, 0, -1, -1);
            f = Frag(e, e);
        }
        return f;
    }

    Frag alt() {
        Frag f = concat();
        while (pos < len && p[pos] == '|') {
            pos++;
            Frag g = concat();
            int64_t e = n.st(S_EPS, 0, -1, -1);
            int64_t s = n.st(S_SPLIT, 0, f.first, g.first);
            n.out1[f.second] = e;
            n.out1[g.second] = e;
            f = Frag(s, e);
        }
        return f;
    }
};

struct Sim {
    const Nfa& n;
    std::vector<int64_t> mark;
    int64_t gen = 0;
    bool matched = false;

    explicit Sim(const Nfa& nfa) : n(nfa), mark(nfa.kind.size(), 0) {}

    void add(std::vector<int64_t>& list, int64_t s) {
        if (s < 0 || mark[s] == gen) return;
        mark[s] = gen;
        int64_t k = n.kind[s];
        if (k == S_SPLIT) {
            add(list, n.out1[s]);
            add(list, n.out2[s]);
        } else if (k == S_EPS) {
            add(list, n.out1[s]);
        } else if (k == S_MATCH) {
            matched = true;
        } else {
            list.push_back(s);
        }
    }
};

static int64_t count(const char* pat, const Buf& t) {
    Compiler c;
    c.p = pat;
    c.len = strlen(pat);
    Frag f = c.alt();
    int64_t m = c.n.st(S_MATCH, 0, -1, -1);
    c.n.out1[f.second] = m;
    const Nfa& n = c.n;
    int64_t start = f.first;
    Sim sim(n);
    std::vector<int64_t> cl, nl;
    int64_t cnt = 0;
    size_t a = 0;
    while (a < t.size()) {
        size_t b = a;
        while (t[b] != '\n') b++;
        sim.gen++;
        sim.matched = false;
        cl.clear();
        size_t i = a;
        for (;;) {
            sim.add(cl, start);
            if (sim.matched || i == b) break;
            uint8_t ch = t[i];
            sim.gen++;
            nl.clear();
            for (int64_t s : cl) {
                int64_t k = n.kind[s], arg = n.arg[s];
                if ((k == S_CHAR && arg == ch) || (k == S_ANY && ch != '\n') || (k == S_CLASS && n.cls[arg * 256 + ch] == 1))
                    sim.add(nl, n.out1[s]);
            }
            if (sim.matched) break;
            std::swap(cl, nl);
            i++;
        }
        if (sim.matched) cnt++;
        a = b + 1;
    }
    return cnt;
}

int main() {
    Buf t = gen_text(200000);
    int64_t total = 0;
    for (int k = 0; k < 8; k++) total += (int64_t)(k + 1) * count(PATTERNS[k], t);
    printf("%lld\n", (long long)total);
    return 0;
}
