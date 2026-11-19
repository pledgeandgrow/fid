// pidigits (Benchmarks Game, BM3): see bench/game/fides/pidigits.fd.
// No GMP (run.sh builds with zig c++ and links nothing else): a small signed
// big integer here, 32-bit limbs, the same as the Rust and Zig programs'
// (add, multiply by a word, compare, a quotient below 10 by subtraction).
#include <cstdint>
#include <cstdio>
#include <vector>

typedef std::vector<uint32_t> Mag;  // little-endian, no high zero limbs; zero is empty

struct Big {
    bool neg = false;
    Mag mag;
};

static int cmp_mag(const Mag &a, const Mag &b) {
    if (a.size() != b.size()) return a.size() < b.size() ? -1 : 1;
    for (size_t i = a.size(); i-- > 0;)
        if (a[i] != b[i]) return a[i] < b[i] ? -1 : 1;
    return 0;
}

static void trim(Mag &a) {
    while (!a.empty() && a.back() == 0) a.pop_back();
}

// a += b
static void add_mag(Mag &a, const Mag &b) {
    if (a.size() < b.size()) a.resize(b.size(), 0);
    uint64_t carry = 0;
    for (size_t i = 0; i < a.size(); i++) {
        if (i >= b.size() && carry == 0) break;
        uint64_t s = (uint64_t)a[i] + (i < b.size() ? b[i] : 0) + carry;
        a[i] = (uint32_t)s;
        carry = s >> 32;
    }
    if (carry) a.push_back((uint32_t)carry);
}

// a -= b, a >= b
static void sub_mag(Mag &a, const Mag &b) {
    int64_t borrow = 0;
    for (size_t i = 0; i < a.size(); i++) {
        if (i >= b.size() && borrow == 0) break;
        int64_t d = (int64_t)a[i] - (i < b.size() ? (int64_t)b[i] : 0) - borrow;
        if (d < 0) {
            a[i] = (uint32_t)(d + (1ll << 32));
            borrow = 1;
        } else {
            a[i] = (uint32_t)d;
            borrow = 0;
        }
    }
    trim(a);
}

// a = b - a, b > a
static void rsub_mag(Mag &a, const Mag &b) {
    a.resize(b.size(), 0);
    int64_t borrow = 0;
    for (size_t i = 0; i < b.size(); i++) {
        int64_t d = (int64_t)b[i] - (int64_t)a[i] - borrow;
        if (d < 0) {
            a[i] = (uint32_t)(d + (1ll << 32));
            borrow = 1;
        } else {
            a[i] = (uint32_t)d;
            borrow = 0;
        }
    }
    trim(a);
}

// x += b, or -= b when b_neg
static void add(Big &x, bool b_neg, const Mag &b) {
    if (x.mag.empty()) x.neg = b_neg;
    if (x.neg == b_neg) {
        add_mag(x.mag, b);
    } else if (cmp_mag(x.mag, b) >= 0) {
        sub_mag(x.mag, b);
    } else {
        rsub_mag(x.mag, b);
        x.neg = b_neg;
    }
    if (x.mag.empty()) x.neg = false;
}

static void mul_small(Big &x, uint32_t m) {
    if (m == 0) {
        x.mag.clear();
        x.neg = false;
        return;
    }
    uint64_t carry = 0;
    for (auto &l : x.mag) {
        uint64_t p = (uint64_t)l * m + carry;
        l = (uint32_t)p;
        carry = p >> 32;
    }
    if (carry) x.mag.push_back((uint32_t)carry);
}

static bool gt(const Big &a, const Big &b) {
    if (a.neg != b.neg) return !a.neg;
    int c = cmp_mag(a.mag, b.mag);
    return a.neg ? c < 0 : c > 0;
}

// (num * nth + acc) / den, the quotient being a digit (num * nth + acc >= 0)
static uint32_t extract(const Big &num, const Big &acc, const Big &den, uint32_t nth, Big &t) {
    t = num;
    mul_small(t, nth);
    add(t, acc.neg, acc.mag);
    uint32_t q = 0;
    while (cmp_mag(t.mag, den.mag) >= 0) {
        sub_mag(t.mag, den.mag);
        q++;
    }
    return q;
}

static uint64_t pidigits(long count) {
    Big num, acc, den, t;
    num.mag.push_back(1);
    den.mag.push_back(1);
    uint32_t k = 0;
    uint64_t h = 0;
    for (long i = 0; i < count;) {
        k++;
        uint32_t k2 = 2 * k + 1;
        t = num;
        mul_small(t, 2);
        add(acc, false, t.mag);
        mul_small(acc, k2);
        mul_small(den, k2);
        mul_small(num, k);
        if (gt(num, acc)) continue;
        uint32_t d = extract(num, acc, den, 3, t);
        if (d != extract(num, acc, den, 4, t)) continue;
        h = (h * 31 + d) & 0xFFFFFFFFu;
        i++;
        t = den;
        mul_small(t, d);
        add(acc, true, t.mag);
        mul_small(acc, 10);
        mul_small(num, 10);
    }
    return h;
}

int main() {
    printf("%llu\n", (unsigned long long)pidigits(10000));
    return 0;
}
