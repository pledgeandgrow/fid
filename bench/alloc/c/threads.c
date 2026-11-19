/* alloc/threads: bench/alloc/fides/threads.fd in C (same blocks, same
   order in each thread). */
#include "alloc.h"

#define SLOTS 10000

static int64_t churn(int64_t tab_, int64_t seed, int64_t steps) {
    int64_t** tab = (int64_t**)(intptr_t)tab_;
    int64_t sum = 0;
    for (int64_t i = 0; i < steps; i++) {
        seed = next(seed);
        int64_t s = (seed >> 4) % SLOTS;
        if (tab[s]) { sum += tab[s][0]; B_FREE(tab[s]); }
        seed = next(seed);
        int64_t* b = (int64_t*)b_alloc(16 + (seed >> 8) % 512);
        b[0] = i & 255;
        tab[s] = b;
    }
    return sum;
}

static int64_t swap_(int64_t tab_, int64_t seed, int64_t tag) {
    int64_t** tab = (int64_t**)(intptr_t)tab_;
    int64_t sum = 0;
    for (int64_t i = 0; i < SLOTS; i++) {
        if (tab[i]) { sum += tab[i][0]; B_FREE(tab[i]); }
        seed = next(seed);
        int64_t* b = (int64_t*)b_alloc(16 + (seed >> 8) % 512);
        b[0] = (i + tag) & 255;
        tab[i] = b;
    }
    return sum;
}

int main(void) {
    int64_t** tabs = (int64_t**)calloc(4 * SLOTS, sizeof(int64_t*));
    int64_t total = 0;
    for (int64_t r = 0; r < 6; r++) {
        b_targ a[4];
        b_thread h[4];
        for (int t = 0; t < 4; t++) {
            a[t].fn = churn; a[t].a = (int64_t)(intptr_t)(tabs + t * SLOTS); a[t].b = r * 8 + 1 + t; a[t].c = 500000;
            h[t] = b_thread_start(&a[t]);
        }
        for (int t = 0; t < 4; t++) { b_thread_join(h[t]); total += a[t].ret; }
        for (int t = 0; t < 4; t++) {
            a[t].fn = swap_; a[t].a = (int64_t)(intptr_t)(tabs + ((t + 1) % 4) * SLOTS); a[t].b = r * 8 + 5 + t; a[t].c = t;
            h[t] = b_thread_start(&a[t]);
        }
        for (int t = 0; t < 4; t++) { b_thread_join(h[t]); total += a[t].ret; }
    }
    for (int64_t i = 0; i < 4 * SLOTS; i++) { total += tabs[i][0]; B_FREE(tabs[i]); }
    free(tabs);
    printf("%lld\n", (long long)total);
    return 0;
}
