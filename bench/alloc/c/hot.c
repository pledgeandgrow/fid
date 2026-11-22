/* alloc/hot (RT8 check, not a mimalloc-bench shape): a temporary buffer of
   16 KB .. 1 MB asked for, filled and freed again and again (it stays in the
   cache: its zeroing must not go to memory). */
#include "alloc.h"

int main(void) {
    int64_t sum = 0;
    for (int64_t n = 16384; n <= 1048576 - 64; n *= 2) {
        int64_t iters = ((int64_t)2 << 30) / n;
        for (int64_t i = 0; i < iters; i++) {
            int64_t* b = (int64_t*)b_alloc(n - 64);
            for (int64_t o = 0; o < (n - 64) / 8; o++) b[o] += o + i;
            sum += b[(n - 64) / 8 - 1];
            B_FREE(b);
        }
    }
    printf("%lld\n", (long long)sum);
    return 0;
}
