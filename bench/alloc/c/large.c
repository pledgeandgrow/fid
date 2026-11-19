/* alloc/large: bench/alloc/fides/large.fd in C (same blocks, same order). */
#include "alloc.h"

static int64_t* make(int64_t n, int64_t v) {
    char* b = (char*)b_alloc(n);
    for (int64_t o = 0; o < n; o += 4096) *(int64_t*)(b + o) = v;
    return (int64_t*)b;
}

int main(void) {
    const int64_t slots = 64;
    int64_t* tab[64];
    int64_t seed = 11, sum = 0;
    for (int64_t i = 0; i < slots; i++) tab[i] = make(65536, i);
    for (int64_t i = 0; i < 40000; i++) {
        seed = next(seed);
        int64_t s = (seed >> 4) % slots;
        sum += tab[s][0];
        B_FREE(tab[s]);
        seed = next(seed);
        int64_t e = (seed >> 8) % 6;
        seed = next(seed);
        int64_t base = (int64_t)65536 << e;
        int64_t n = (base + (seed >> 4) % base) & ~(int64_t)7;
        tab[s] = make(n, i & 4095);
    }
    for (int64_t i = 0; i < slots; i++) { sum += tab[i][0]; B_FREE(tab[i]); }
    printf("%lld\n", (long long)sum);
    return 0;
}
