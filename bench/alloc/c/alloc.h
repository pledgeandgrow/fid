/* bench/alloc (competitive roadmap RT8): the allocator the C programs use.
   One program, built four ways by bench/alloc/run.sh (only the allocator
   differs, so the times compare allocators, not compilers):
     (default)        the C library's malloc/free
     -DUSE_MIMALLOC   mimalloc's mi_malloc/mi_free (linked from its static.c)
     -DUSE_FID        fid's p_malloc/p_free: the C runtime `fid dump-c`
                      writes for an empty program (bench/alloc/c/rt.fd),
                      compiled with -Dmain=__fid_rt_main and linked in
     -DZERO           calloc / mi_zalloc: zeroed blocks, as fid's p_malloc
                      gives (Fides memory is zeroed when it is handed out;
                      USE_FID is zeroed unless UNINIT)
     -DUNINIT         no zeroing in any variant (ZERO ignored): fid's
                      __p_malloc_uninit, the runtimes' own allocation for
                      memory they write whole (RT8), against malloc and
                      mi_malloc like for like (sizes_uninit.c)
   B_THREAD_* start and join a thread (pthreads, or Win32 threads). */
#ifndef BENCH_ALLOC_H
#define BENCH_ALLOC_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(UNINIT) && defined(ZERO)
#undef ZERO
#endif
#if defined(USE_FID)
void* p_malloc(int64_t size);
void* __p_malloc_uninit(int64_t size);
void p_free(void* ptr);
#if defined(UNINIT)
#define B_MALLOC(n) __p_malloc_uninit((int64_t)(n))
#else
#define B_MALLOC(n) p_malloc((int64_t)(n))
#endif
#define B_FREE(p) p_free(p)
#elif defined(USE_MIMALLOC)
#include <mimalloc.h>
#if defined(ZERO)
#define B_MALLOC(n) mi_zalloc((size_t)(n))
#else
#define B_MALLOC(n) mi_malloc((size_t)(n))
#endif
#define B_FREE(p) mi_free(p)
#else
#if defined(ZERO)
#define B_MALLOC(n) calloc(1, (size_t)(n))
#else
#define B_MALLOC(n) malloc((size_t)(n))
#endif
#define B_FREE(p) free(p)
#endif

/* the programs' 31-bit LCG, as in the Fides versions */
static inline int64_t next(int64_t x) { return (x * 1103515245 + 12345) % 2147483648LL; }

static inline void* b_alloc(int64_t n) {
    void* p = B_MALLOC(n);
    if (!p) { fprintf(stderr, "out of memory\n"); exit(1); }
    return p;
}

#ifdef _WIN32
#include <windows.h>
typedef HANDLE b_thread;
typedef struct { int64_t (*fn)(int64_t, int64_t, int64_t); int64_t a, b, c, ret; } b_targ;
static DWORD WINAPI b_tramp(LPVOID p) { b_targ* t = (b_targ*)p; t->ret = t->fn(t->a, t->b, t->c); return 0; }
static b_thread b_thread_start(b_targ* t) { return CreateThread(NULL, 0, b_tramp, t, 0, NULL); }
static void b_thread_join(b_thread h) { WaitForSingleObject(h, INFINITE); CloseHandle(h); }
#else
#include <pthread.h>
typedef pthread_t b_thread;
typedef struct { int64_t (*fn)(int64_t, int64_t, int64_t); int64_t a, b, c, ret; } b_targ;
static void* b_tramp(void* p) { b_targ* t = (b_targ*)p; t->ret = t->fn(t->a, t->b, t->c); return NULL; }
static b_thread b_thread_start(b_targ* t) { pthread_t h; pthread_create(&h, NULL, b_tramp, t); return h; }
static void b_thread_join(b_thread h) { pthread_join(h, NULL); }
#endif
#endif
