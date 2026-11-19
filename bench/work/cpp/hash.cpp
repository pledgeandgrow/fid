// hash (BM4): see bench/work/fides/hash.fd.
#include <cstdint>
#include <cstdio>
#include <vector>

static uint64_t fnv(const std::vector<uint8_t>& data, int rounds) {
    uint64_t h = 0xcbf29ce484222325ULL;
    for (int r = 0; r < rounds; r++)
        for (uint8_t b : data) h = (h ^ b) * 0x100000001b3ULL;
    return h;
}

int main() {
    const size_t n = 50000000;
    std::vector<uint8_t> data(n);
    for (size_t i = 0; i < n; i++) data[i] = (uint8_t)((i * 7 + i / 256) % 256);
    std::printf("%lld\n", (long long)fnv(data, 4));
}
