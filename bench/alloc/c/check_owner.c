/* alloc/check_owner (RT8 check, not a mimalloc-bench shape): fid's C runtime
   allocator linked in (-DUSE_FID, as run.sh's fid variant: the runtime of
   `fid dump-c c/rt.fd` compiled with -Dmain=__fid_rt_main), with threads
   the C library starts (the runtime's thread-end hooks, not its wrapper):
     - __p_malloc_uninit: a block like p_malloc's (the same header, freed by
       p_free); p_malloc's block reads zero after an uninit one was filled
       and freed, at every size to 2100 bytes and some large ones;
     - owner return: blocks thread A made and thread B freed are not handed
       out to B, and A gets them back;
     - thread ends: 30 generations of 4 threads, each freeing the blocks of
       a thread of the generation before (ended: its heap went to the pool)
       and making its own: every new block reads zero, every old one holds
       its number, and the memory is reused (the resident size where it is
       known: less than 48 MB more after generation 30 than after 5).
   Prints "ok" lines; exit 1 on any failure.
     zig cc -O2 -DUSE_FID -Ibench/alloc/c bench/alloc/c/check_owner.c rt.o -o chk.exe -lws2_32 */
#define USE_FID 1
#include "alloc.h"

int64_t p_mem_resident(void);

#define MAGIC 0xF1DE5A0000000000ULL
static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { fails++; fprintf(stderr, "FAIL: " __VA_ARGS__); fprintf(stderr, "\n"); } } while (0)

#if defined(_WIN32)
#define B_ALOAD(p) InterlockedCompareExchange64((volatile LONG64*)(p), 0, 0)
#define B_ASTORE(p, v) InterlockedExchange64((volatile LONG64*)(p), (v))
#define B_YIELD() SwitchToThread()
#else
#include <sched.h>
#define B_ALOAD(p) __atomic_load_n((p), __ATOMIC_SEQ_CST)
#define B_ASTORE(p, v) __atomic_store_n((p), (v), __ATOMIC_SEQ_CST)
#define B_YIELD() sched_yield()
#endif

static void check_uninit(void) {
    int64_t sizes[] = { 16384, 100000, 262144, 1048568, 1048577, 3000000 };
    int bad = 0;
    for (int64_t n = 0; n <= 2100 + 6; n++) {
        int64_t sz = n <= 2100 ? n : sizes[n - 2101];
        unsigned char* p = (unsigned char*)__p_malloc_uninit(sz);
        CHECK(p != NULL, "__p_malloc_uninit(%lld) gave NULL", (long long)sz);
        uint64_t h = ((uint64_t*)p)[-1];
        CHECK((h & ~(uint64_t)0xFF) == MAGIC && (int64_t)h < 0, "header of an uninit block of %lld: %llx", (long long)sz, (unsigned long long)h);
        memset(p, 0xA5, (size_t)sz);
        p_free(p);
        unsigned char* q = (unsigned char*)p_malloc(sz);
        for (int64_t i = 0; i < sz; i++) if (q[i]) { bad++; break; }
        memset(q, 0x5A, (size_t)sz);
        p_free(q);
    }
    CHECK(bad == 0, "%d p_malloc blocks not zeroed after an uninit one", bad);
    if (!bad) printf("ok: __p_malloc_uninit blocks, p_malloc zeroed after them (2107 sizes)\n");
}

/* owner return between two threads that both stay alive */
#define OR_N 1000
static void* or_blocks[OR_N];
static int64_t or_step = 0;
static int64_t or_overlap_b = -1, or_overlap_a = -1;
static int in_set(void* p) { for (int i = 0; i < OR_N; i++) if (or_blocks[i] == p) return 1; return 0; }
static int64_t or_a(int64_t x, int64_t y, int64_t z) {
    (void)x; (void)y; (void)z;
    for (int i = 0; i < OR_N; i++) { or_blocks[i] = p_malloc(40); ((int64_t*)or_blocks[i])[0] = i; }
    B_ASTORE(&or_step, 1);
    while (B_ALOAD(&or_step) != 2) B_YIELD();
    int64_t ov = 0;
    void* mine[OR_N];
    for (int i = 0; i < OR_N; i++) { mine[i] = p_malloc(40); ov += in_set(mine[i]); }
    or_overlap_a = ov;
    for (int i = 0; i < OR_N; i++) p_free(mine[i]);
    return 0;
}
static int64_t or_b(int64_t x, int64_t y, int64_t z) {
    (void)x; (void)y; (void)z;
    while (B_ALOAD(&or_step) != 1) B_YIELD();
    for (int i = 0; i < OR_N; i++) p_free(or_blocks[i]);
    int64_t ov = 0;
    void* mine[OR_N];
    for (int i = 0; i < OR_N; i++) { mine[i] = p_malloc(40); ov += in_set(mine[i]); }
    or_overlap_b = ov;
    for (int i = 0; i < OR_N; i++) p_free(mine[i]);
    B_ASTORE(&or_step, 2);
    return 0;
}
static void check_owner_return(void) {
    b_targ a = { or_a, 0, 0, 0, 0 }, b = { or_b, 0, 0, 0, 0 };
    b_thread ha = b_thread_start(&a), hb = b_thread_start(&b);
    b_thread_join(ha);
    b_thread_join(hb);
    CHECK(or_overlap_b == 0, "thread B was handed %lld blocks thread A made and B freed", (long long)or_overlap_b);
    CHECK(or_overlap_a == OR_N, "thread A got %lld of its %d blocks back", (long long)or_overlap_a, OR_N);
    if (or_overlap_b == 0 && or_overlap_a == OR_N) printf("ok: owner return (B got none of A's blocks, A got all %d back)\n", OR_N);
}

/* generations of threads that end */
#define GEN_T 4
#define GEN_N 5000
static int64_t* gen_tab[GEN_T][GEN_N];
static int64_t gen_size[GEN_T][GEN_N];
static int64_t gen_bad = 0, gen_dirty = 0;
static int64_t gen_run(int64_t t, int64_t g, int64_t z) {
    (void)z;
    int64_t from = (t + 1) % GEN_T;
    int64_t bad = 0;
    for (int i = 0; i < GEN_N; i++) {
        int64_t* old = gen_tab[from][i];
        if (old) {
            if (old[0] != (g - 1) * 100000 + i || old[gen_size[from][i] / 8 - 1] != i) bad++;
        }
    }
    /* free the other thread's blocks (its thread ended), then make ours */
    for (int i = 0; i < GEN_N; i++) { if (gen_tab[from][i]) p_free(gen_tab[from][i]); }
    return bad * 1000000;
}
static int64_t gen_make(int64_t t, int64_t g, int64_t z) {
    (void)z;
    int64_t seed = g * 977 + t, dirty = 0;
    for (int i = 0; i < GEN_N; i++) {
        seed = next(seed);
        int64_t n = 16 + ((seed >> 8) % 125) * 8;
        if (i % 97 == 0) n = 20000 + (seed >> 8) % 4096 * 8;
        int64_t* b = (int64_t*)p_malloc(n);
        if (b[0] || b[n / 8 - 1]) dirty++;
        b[0] = g * 100000 + i;
        b[n / 8 - 1] = i;
        gen_tab[t][i] = b;
        gen_size[t][i] = n;
    }
    return dirty;
}
static void check_generations(void) {
    int64_t r5 = -1, r30 = -1;
    for (int64_t g = 0; g < 30; g++) {
        b_targ a[GEN_T];
        b_thread h[GEN_T];
        /* each thread of this generation frees the blocks of a thread of the
           one before (ended), then the tables are refilled by new threads */
        if (g > 0) {
            for (int t = 0; t < GEN_T; t++) { a[t].fn = gen_run; a[t].a = t; a[t].b = g; a[t].c = 0; h[t] = b_thread_start(&a[t]); }
            for (int t = 0; t < GEN_T; t++) { b_thread_join(h[t]); gen_bad += a[t].ret / 1000000; }
            for (int t = 0; t < GEN_T; t++) for (int i = 0; i < GEN_N; i++) gen_tab[t][i] = NULL;
        }
        for (int t = 0; t < GEN_T; t++) { a[t].fn = gen_make; a[t].a = t; a[t].b = g; a[t].c = 0; h[t] = b_thread_start(&a[t]); }
        for (int t = 0; t < GEN_T; t++) { b_thread_join(h[t]); gen_dirty += a[t].ret; }
        if (g == 4) r5 = p_mem_resident();
    }
    r30 = p_mem_resident();
    for (int t = 0; t < GEN_T; t++) for (int i = 0; i < GEN_N; i++) if (gen_tab[t][i]) p_free(gen_tab[t][i]);
    CHECK(gen_bad == 0, "%lld blocks lost their numbers across thread generations", (long long)gen_bad);
    CHECK(gen_dirty == 0, "%lld new blocks not zeroed across thread generations", (long long)gen_dirty);
    int reused = r5 < 0 || r30 - r5 < ((int64_t)48 << 20);
    CHECK(reused, "memory added up across thread generations: %lld MB after 5, %lld MB after 30", (long long)(r5 >> 20), (long long)(r30 >> 20));
    if (gen_bad == 0 && gen_dirty == 0 && reused) printf("ok: 30 generations of %d threads: blocks kept, new ones zeroed, memory reused (%lld -> %lld MB)\n", GEN_T, (long long)(r5 >> 20), (long long)(r30 >> 20));
}

int main(void) {
    check_uninit();
    check_owner_return();
    check_generations();
    if (fails) { printf("%d failure(s)\n", fails); return 1; }
    printf("all ok\n");
    return 0;
}
