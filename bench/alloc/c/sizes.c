/* alloc/sizes: bench/alloc/fides/sizes.fd in C (same blocks, same order). */
#include "alloc.h"

static int64_t* make(int64_t n, int64_t v) {
    int64_t* b = (int64_t*)b_alloc(n);
    b[0] = v;
    b[n / 8 - 1] = v;
    return b;
}

int main(void) {
    const int64_t slots = 20000;
    int64_t** blk = (int64_t**)b_alloc(slots * 8);
    int64_t* len = (int64_t*)b_alloc(slots * 8);
    int64_t seed = 3, sum = 0;
    for (int64_t i = 0; i < slots; i++) { blk[i] = make(8, i & 4095); len[i] = 8; }
    for (int64_t i = 0; i < 4000000; i++) {
        seed = next(seed);
        int64_t s = (seed >> 4) % slots;
        int64_t* old = blk[s];
        sum += old[0] + old[len[s] / 8 - 1];
        B_FREE(old);
        seed = next(seed);
        int64_t e = (seed >> 8) % 13;
        seed = next(seed);
        int64_t base = (int64_t)8 << e;
        int64_t n = (base + (seed >> 6) % base) & ~(int64_t)7;
        blk[s] = make(n, i & 4095);
        len[s] = n;
    }
    for (int64_t i = 0; i < slots; i++) { sum += blk[i][0]; B_FREE(blk[i]); }
    B_FREE(blk);
    B_FREE(len);
    printf("%lld\n", (long long)sum);
    return 0;
}
