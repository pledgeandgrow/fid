// fasta (Benchmarks Game, BM3): see bench/game/fides/fasta.fd.
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

static const char *ALU = "GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGGGAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGACCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAATACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCAGCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGGAGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCCAGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA";

static uint64_t random_hash(uint64_t h, int64_t *seed, const char *chars, const std::vector<double> &probs, long m) {
    std::vector<double> cum;
    double p = 0.0;
    for (double q : probs) {
        p += q;
        cum.push_back(p);
    }
    size_t last_i = cum.size() - 1;
    int64_t last = *seed;
    for (long k = 0; k < m; k++) {
        last = (last * 3877 + 29573) % 139968;
        double r = (double)last / 139968.0;
        size_t i = 0;
        while (i < last_i && r >= cum[i]) i++;
        h = (h * 31 + (unsigned char)chars[i]) & 0xFFFFFFFFu;
    }
    *seed = last;
    return h;
}

static uint64_t fasta(long n) {
    uint64_t h = 0;
    size_t len = strlen(ALU);
    for (long k = 0; k < 2 * n; k++) h = (h * 31 + (unsigned char)ALU[k % len]) & 0xFFFFFFFFu;
    int64_t seed = 42;
    std::vector<double> iub = {0.27, 0.12, 0.12, 0.27};
    for (int j = 0; j < 11; j++) iub.push_back(0.02);
    h = random_hash(h, &seed, "acgtBDHKMNRSVWY", iub, 3 * n);
    std::vector<double> homo = {0.3029549426680, 0.1979883004921, 0.1975473066391, 0.3015094502008};
    return random_hash(h, &seed, "acgt", homo, 5 * n);
}

int main() {
    printf("%llu\n", (unsigned long long)fasta(2500000));
    return 0;
}
