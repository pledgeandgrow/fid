// reverse-complement (Benchmarks Game, BM3): see bench/game/fides/revcomp.fd.
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

static const char *ALU = "GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGGGAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGACCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAATACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCAGCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGGAGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCCAGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA";

static std::vector<uint8_t> repeat_section(long m) {
    size_t len = strlen(ALU);
    std::vector<uint8_t> out(m);
    for (long k = 0; k < m; k++) out[k] = (uint8_t)ALU[k % len];
    return out;
}

static std::vector<uint8_t> random_section(int64_t *seed, const char *chars, const std::vector<double> &probs, long m) {
    std::vector<double> cum;
    double p = 0.0;
    for (double q : probs) {
        p += q;
        cum.push_back(p);
    }
    size_t last_i = cum.size() - 1;
    int64_t last = *seed;
    std::vector<uint8_t> out(m);
    for (long k = 0; k < m; k++) {
        last = (last * 3877 + 29573) % 139968;
        double r = (double)last / 139968.0;
        size_t i = 0;
        while (i < last_i && r >= cum[i]) i++;
        out[k] = (uint8_t)chars[i];
    }
    *seed = last;
    return out;
}

static void comp_table(uint8_t t[256]) {
    for (int c = 0; c < 256; c++) t[c] = (uint8_t)c;
    const char *from = "ACBDGHKMNSRUTWVYacbdghkmnsrutwvy";
    const char *to = "TGVHCDMKNSYAAWBRTGVHCDMKNSYAAWBR";
    for (size_t j = 0; from[j]; j++) t[(uint8_t)from[j]] = (uint8_t)to[j];
}

// in place: two indexes meet in the middle
static void reverse_complement(std::vector<uint8_t> &seq, const uint8_t t[256]) {
    if (seq.empty()) return;
    size_t i = 0, j = seq.size() - 1;
    while (i < j) {
        uint8_t a = seq[i], b = seq[j];
        seq[i] = t[b];
        seq[j] = t[a];
        i++;
        j--;
    }
    if (i == j) seq[i] = t[seq[i]];
}

static uint64_t hash_seq(uint64_t h, const std::vector<uint8_t> &seq) {
    for (uint8_t c : seq) h = (h * 31 + c) & 0xFFFFFFFFu;
    return h;
}

static uint64_t revcomp(long n) {
    uint8_t t[256];
    comp_table(t);
    int64_t seed = 42;
    std::vector<double> iub = {0.27, 0.12, 0.12, 0.27};
    for (int j = 0; j < 11; j++) iub.push_back(0.02);
    std::vector<double> homo = {0.3029549426680, 0.1979883004921, 0.1975473066391, 0.3015094502008};
    std::vector<uint8_t> one = repeat_section(2 * n);
    std::vector<uint8_t> two = random_section(&seed, "acgtBDHKMNRSVWY", iub, 3 * n);
    std::vector<uint8_t> three = random_section(&seed, "acgt", homo, 5 * n);
    reverse_complement(one, t);
    reverse_complement(two, t);
    reverse_complement(three, t);
    uint64_t h = hash_seq(0, one);
    h = hash_seq(h, two);
    return hash_seq(h, three);
}

int main() {
    printf("%llu\n", (unsigned long long)revcomp(2500000));
    return 0;
}
