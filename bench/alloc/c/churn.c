/* alloc/churn: bench/alloc/fides/churn.fd in C (same blocks, same order). */
#include "alloc.h"

int main(void) {
    int64_t* ring[64];
    int64_t seed = 7, sum = 0;
    for (int i = 0; i < 64; i++) { ring[i] = (int64_t*)b_alloc(8); ring[i][0] = 0; }
    for (int64_t i = 0; i < 30000000; i++) {
        int64_t* old = ring[i & 63];
        sum += old[0];
        B_FREE(old);
        seed = next(seed);
        int64_t* b = (int64_t*)b_alloc(8 + (seed >> 8) % 128);
        b[0] = i & 1023;
        ring[i & 63] = b;
    }
    for (int i = 0; i < 64; i++) { sum += ring[i][0]; B_FREE(ring[i]); }
    printf("%lld\n", (long long)sum);
    return 0;
}
