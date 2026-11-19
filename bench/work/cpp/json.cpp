// json (BM4): see bench/work/fides/json.fd. A hand-written parser into an
// arena of nodes and a compact printer, standard library only.
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

typedef std::vector<uint8_t> Buf;

static int64_t seed = 1;
static int64_t rnd() {
    seed = (seed * 1103515245 + 12345) % 2147483648LL;
    return seed >> 8;
}

static void put_int(Buf& o, int64_t v) {
    char tmp[24];
    int n = snprintf(tmp, sizeof tmp, "%lld", (long long)v);
    o.insert(o.end(), tmp, tmp + n);
}

static void put(Buf& o, const char* s) {
    while (*s) o.push_back((uint8_t)*s++);
}

static void gen_string(Buf& o) {
    o.push_back('"');
    int64_t n = rnd() % 12;
    for (int64_t i = 0; i < n; i++) {
        int64_t c = rnd() % 40;
        if (c < 26) o.push_back((uint8_t)(97 + c));
        else if (c == 26) put(o, "\\n");
        else if (c == 27) put(o, "\\\"");
        else if (c == 28) put(o, "\\\\");
        else if (c == 29) put(o, "\\u00e9");
        else if (c == 30) put(o, "\\t");
        else if (c == 31) put(o, "\\u0041");
        else if (c < 36) o.push_back(' ');
        else o.push_back((uint8_t)(48 + c - 36));
    }
    o.push_back('"');
}

static void gen_value(Buf& o, int64_t d) {
    int64_t r = rnd() % 8;
    if (d == 0) r %= 5;
    if (r == 0) {
        int64_t v = rnd() % 2000000 - 1000000;
        put_int(o, v);
    } else if (r == 1 || r == 4) {
        gen_string(o);
    } else if (r == 2) {
        int64_t t = rnd() % 3;
        put(o, t == 0 ? "true" : t == 1 ? "false" : "null");
    } else if (r == 3) {
        int64_t v = rnd() % 100;
        put_int(o, v);
    } else if (r == 5 || r == 6) {
        int64_t n = rnd() % 5;
        o.push_back('[');
        for (int64_t i = 0; i < n; i++) {
            if (i > 0) put(o, ", ");
            gen_value(o, d - 1);
        }
        o.push_back(']');
    } else {
        int64_t n = rnd() % 5;
        o.push_back('{');
        for (int64_t i = 0; i < n; i++) {
            if (i > 0) put(o, ",\n  ");
            gen_string(o);
            put(o, ": ");
            gen_value(o, d - 1);
        }
        o.push_back('}');
    }
}

static Buf generate(int64_t items, int64_t depth) {
    Buf o;
    o.push_back('[');
    for (int64_t i = 0; i < items; i++) {
        if (i > 0) put(o, ",\n");
        gen_value(o, depth);
    }
    o.push_back(']');
    o.push_back('\n');
    return o;
}

// node kinds: 0 null, 1 false, 2 true, 3 number, 4 string, 5 array, 6 object
struct Parser {
    const Buf& t;
    size_t pos = 0;
    std::vector<int64_t> kind, num, first, next, key, ps, pl;
    Buf pool;

    explicit Parser(const Buf& text) : t(text) {}

    void ws() {
        for (;;) {
            uint8_t c = t[pos];
            if (c != ' ' && c != '\n' && c != '\r' && c != '\t') return;
            pos++;
        }
    }

    int64_t node(int64_t k, int64_t v) {
        kind.push_back(k);
        num.push_back(v);
        first.push_back(-1);
        next.push_back(-1);
        key.push_back(-1);
        return (int64_t)kind.size() - 1;
    }

    int64_t str() {
        pos++;
        size_t st = pool.size();
        for (;;) {
            uint8_t c = t[pos];
            if (c == '"') {
                pos++;
                break;
            }
            if (c == '\\') {
                uint8_t e = t[pos + 1];
                pos += 2;
                if (e == 'n') pool.push_back(10);
                else if (e == 't') pool.push_back(9);
                else if (e == 'r') pool.push_back(13);
                else if (e == 'b') pool.push_back(8);
                else if (e == 'f') pool.push_back(12);
                else if (e == 'u') {
                    int64_t cp = 0;
                    for (int j = 0; j < 4; j++) {
                        int64_t h = t[pos++];
                        int64_t v = (h >= 48 && h <= 57) ? h - 48 : (h >= 97 && h <= 102) ? h - 87 : h - 55;
                        cp = cp * 16 + v;
                    }
                    if (cp < 128) {
                        pool.push_back((uint8_t)cp);
                    } else if (cp < 2048) {
                        pool.push_back((uint8_t)(192 | (cp >> 6)));
                        pool.push_back((uint8_t)(128 | (cp & 63)));
                    } else {
                        pool.push_back((uint8_t)(224 | (cp >> 12)));
                        pool.push_back((uint8_t)(128 | ((cp >> 6) & 63)));
                        pool.push_back((uint8_t)(128 | (cp & 63)));
                    }
                } else {
                    pool.push_back(e);
                }
            } else {
                pool.push_back(c);
                pos++;
            }
        }
        ps.push_back((int64_t)st);
        pl.push_back((int64_t)(pool.size() - st));
        return (int64_t)ps.size() - 1;
    }

    int64_t value() {
        ws();
        uint8_t c = t[pos];
        if (c == '{' || c == '[') {
            uint8_t close = c == '{' ? '}' : ']';
            int64_t n = node(c == '{' ? 6 : 5, 0);
            pos++;
            int64_t prev = -1;
            ws();
            if (t[pos] == close) {
                pos++;
                return n;
            }
            for (;;) {
                int64_t k = -1;
                if (c == '{') {
                    ws();
                    k = str();
                    ws();
                    pos++;
                }
                int64_t ch = value();
                key[ch] = k;
                if (prev < 0) first[n] = ch;
                else next[prev] = ch;
                prev = ch;
                ws();
                if (t[pos] == ',') {
                    pos++;
                } else {
                    pos++;
                    return n;
                }
            }
        }
        if (c == '"') {
            int64_t s = str();
            return node(4, s);
        }
        if (c == 't') { pos += 4; return node(2, 0); }
        if (c == 'f') { pos += 5; return node(1, 0); }
        if (c == 'n') { pos += 4; return node(0, 0); }
        bool neg = false;
        if (c == '-') {
            neg = true;
            pos++;
        }
        int64_t v = 0;
        while (t[pos] >= '0' && t[pos] <= '9') {
            v = v * 10 + (t[pos] - 48);
            pos++;
        }
        return node(3, neg ? -v : v);
    }
};

static const char HEX[] = "0123456789abcdef";

static void print_str(Buf& o, const Parser& p, int64_t sid) {
    o.push_back('"');
    int64_t st = p.ps[sid], end = st + p.pl[sid];
    for (int64_t i = st; i < end; i++) {
        uint8_t b = p.pool[i];
        if (b == '"') put(o, "\\\"");
        else if (b == '\\') put(o, "\\\\");
        else if (b == '\n') put(o, "\\n");
        else if (b == '\t') put(o, "\\t");
        else if (b < 32) {
            put(o, "\\u00");
            o.push_back((uint8_t)HEX[b >> 4]);
            o.push_back((uint8_t)HEX[b & 15]);
        } else o.push_back(b);
    }
    o.push_back('"');
}

static void print_node(Buf& o, const Parser& p, int64_t n) {
    int64_t k = p.kind[n];
    if (k == 0) { put(o, "null"); return; }
    if (k == 1) { put(o, "false"); return; }
    if (k == 2) { put(o, "true"); return; }
    if (k == 3) { put_int(o, p.num[n]); return; }
    if (k == 4) { print_str(o, p, p.num[n]); return; }
    o.push_back(k == 5 ? '[' : '{');
    int64_t c = p.first[n];
    bool f = true;
    while (c >= 0) {
        if (!f) o.push_back(',');
        f = false;
        if (k == 6) {
            print_str(o, p, p.key[c]);
            o.push_back(':');
        }
        print_node(o, p, c);
        c = p.next[c];
    }
    o.push_back(k == 5 ? ']' : '}');
}

static int64_t run(int64_t items, int64_t depth, int64_t rounds) {
    Buf text = generate(items, depth);
    int64_t total = 0;
    for (int64_t r = 0; r < rounds; r++) {
        Buf o;
        int64_t nodes;
        {
            Parser p(text);
            int64_t root = p.value();
            print_node(o, p, root);
            nodes = (int64_t)p.kind.size();
        }
        int64_t h = 0;
        for (uint8_t b : o) h = (h * 31 + b) % 1000000007;
        total += h + nodes;
        text.swap(o);
    }
    return total;
}

int main() {
    printf("%lld\n", (long long)run(500000, 6, 3));
    return 0;
}
