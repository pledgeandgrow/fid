// k-nucleotide (Benchmarks Game, BM3): see bench/game/fides/knucleotide.fd.
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <unordered_map>
#include <vector>

// the THREE section as codes: the index of each drawn character in "acgt"
static std::vector<uint8_t> three_codes(long n) {
    int64_t last = 42;
    for (long k = 0; k < 3 * n; k++) last = (last * 3877 + 29573) % 139968;
    const double probs[4] = {0.3029549426680, 0.1979883004921, 0.1975473066391, 0.3015094502008};
    double cum[4];
    double p = 0.0;
    for (int j = 0; j < 4; j++) {
        p += probs[j];
        cum[j] = p;
    }
    const int last_i = 3;
    std::vector<uint8_t> out(5 * n);
    for (long k = 0; k < 5 * n; k++) {
        last = (last * 3877 + 29573) % 139968;
        double r = (double)last / 139968.0;
        int i = 0;
        while (i < last_i && r >= cum[i]) i++;
        out[k] = (uint8_t)i;
    }
    return out;
}

static std::unordered_map<uint64_t, uint64_t> count_kmers(const std::vector<uint8_t> &codes, size_t k) {
    uint64_t mask = (1ull << (2 * k)) - 1;
    std::unordered_map<uint64_t, uint64_t> m;
    uint64_t key = 0;
    for (size_t i = 0; i < codes.size(); i++) {
        key = ((key << 2) | codes[i]) & mask;
        if (i + 1 >= k) m[key]++;
    }
    return m;
}

static uint64_t encode(const char *s) {
    uint64_t key = 0;
    for (; *s; s++) {
        uint64_t v = *s == 'C' ? 1 : *s == 'G' ? 2 : *s == 'T' ? 3 : 0;
        key = (key << 2) | v;
    }
    return key;
}

static uint64_t fold(uint64_t h, uint64_t v) { return (h * 31 + v) & 0xFFFFFFFFu; }

static uint64_t get_or0(const std::unordered_map<uint64_t, uint64_t> &m, uint64_t key) {
    auto it = m.find(key);
    return it == m.end() ? 0 : it->second;
}

static uint64_t frequencies(uint64_t h, const std::vector<uint8_t> &codes, size_t k) {
    auto m = count_kmers(codes, k);
    h = fold(h, m.size());
    for (uint64_t key = 0; key < (1ull << (2 * k)); key++) h = fold(h, get_or0(m, key));
    return h;
}

static uint64_t count_of(uint64_t h, const std::vector<uint8_t> &codes, const char *s) {
    auto m = count_kmers(codes, strlen(s));
    h = fold(h, m.size());
    return fold(h, get_or0(m, encode(s)));
}

static uint64_t knucleotide(long n) {
    std::vector<uint8_t> codes = three_codes(n);
    uint64_t h = frequencies(0, codes, 1);
    h = frequencies(h, codes, 2);
    for (const char *s : {"GGT", "GGTA", "GGTATT", "GGTATTTTAATT", "GGTATTTTAATTTATAGT"}) h = count_of(h, codes, s);
    return h;
}

int main() {
    printf("%llu\n", (unsigned long long)knucleotide(1000000));
    return 0;
}
