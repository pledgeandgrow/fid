/* alloc/small: bench/alloc/fides/small.fd in C (same blocks, same order). */
#include "alloc.h"

static int64_t round_(int64_t* slots, int64_t n, int64_t seed) {
    int64_t sum = 0;
    for (int64_t i = 0; i < n; i++) {
        seed = next(seed);
        int64_t* b = (int64_t*)b_alloc(16 + (seed >> 8) % 256);
        b[0] = i;
        slots[i] = (int64_t)b;
    }
    for (int64_t i = 0; i < n; i += 2) {
        int64_t* b = (int64_t*)slots[i];
        sum += b[0];
        B_FREE(b);
        seed = next(seed);
        int64_t* c = (int64_t*)b_alloc(16 + (seed >> 8) % 256);
        c[0] = i + 1;
        slots[i] = (int64_t)c;
    }
    for (int64_t i = n - 1; i >= 0; i--) {
        int64_t* b = (int64_t*)slots[i];
        sum += b[0];
        B_FREE(b);
    }
    return sum;
}

int main(void) {
    const int64_t n = 100000;
    int64_t* slots = (int64_t*)b_alloc(n * 8);
    int64_t total = 0;
    for (int64_t r = 0; r < 40; r++) total += round_(slots, n, r + 1);
    B_FREE(slots);
    printf("%lld\n", (long long)total);
    return 0;
}
