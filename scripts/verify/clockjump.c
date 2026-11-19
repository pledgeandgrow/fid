/* scripts/verify/clockjump.c: the clocks jump once, part-way through a run
 * (roadmap VF17, scripts/verify/chaos_linux.py).
 *
 * An LD_PRELOAD library for a dynamically linked Linux program (glibc), in
 * the manner of libfaketime but with one scheduled jump and nothing else:
 *
 *   gcc -shared -fPIC -O2 scripts/verify/clockjump.c -o clockjump.so
 *   CLOCKJUMP_AT_MS=250 CLOCKJUMP_BY_MS=-3600000 CLOCKJUMP_CLOCK=realtime \
 *       LD_PRELOAD=./clockjump.so ./program
 *
 * CLOCKJUMP_AT_MS   when, in real milliseconds after the program started
 *                   (unset: never; 0: from the start)
 * CLOCKJUMP_BY_MS   how far, in milliseconds (negative: back)
 * CLOCKJUMP_CLOCK   realtime (default: what an NTP step or a user setting
 *                   the date does), monotonic, or both (a paused or
 *                   suspended VM: every clock moves forward together)
 *
 * clock_gettime, gettimeofday and time are replaced; the jumped clocks are
 * CLOCK_REALTIME(_COARSE) and CLOCK_TAI for realtime, CLOCK_MONOTONIC(
 * _COARSE, _RAW) and CLOCK_BOOTTIME for monotonic. The real time comes from
 * the system call, not the vDSO, so the library never calls itself. What
 * the kernel times itself is not changed: relative nanosleep, poll and
 * epoll timeouts run on the real clock, as they do when a real clock jumps
 * (an absolute clock_nanosleep on CLOCK_REALTIME would wake at the jump on
 * a real system and does not here). A statically linked program (fid's
 * musl cross builds, the machine layer's ELF images) does not load it.
 */
#define _GNU_SOURCE
/* glibc declares gettimeofday with `void *restrict tz` or `struct timezone *`
 * depending on its version: hide its declaration and give ours */
#define gettimeofday __clockjump_hidden_gettimeofday
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#undef gettimeofday

static int64_t cj_at_ns = -1;
static int64_t cj_by_ns = 0;
static int cj_which = 1;            /* 1 realtime, 2 monotonic, 3 both */
static int64_t cj_start_ns = 0;

static int64_t cj_ns(const struct timespec* t) {
    return (int64_t)t->tv_sec * 1000000000LL + (int64_t)t->tv_nsec;
}

static void cj_set(struct timespec* t, int64_t ns) {
    int64_t s = ns / 1000000000LL, r = ns % 1000000000LL;
    if (r < 0) { r += 1000000000LL; s -= 1; }
    t->tv_sec = (time_t)s;
    t->tv_nsec = (long)r;
}

static int64_t cj_real_mono(void) {
    struct timespec t;
    if (syscall(SYS_clock_gettime, CLOCK_MONOTONIC, &t) != 0) return 0;
    return cj_ns(&t);
}

__attribute__((constructor)) static void cj_init(void) {
    const char* at = getenv("CLOCKJUMP_AT_MS");
    const char* by = getenv("CLOCKJUMP_BY_MS");
    const char* w = getenv("CLOCKJUMP_CLOCK");
    cj_start_ns = cj_real_mono();
    if (at && *at) cj_at_ns = strtoll(at, NULL, 10) * 1000000LL;
    if (by && *by) cj_by_ns = strtoll(by, NULL, 10) * 1000000LL;
    if (w && strcmp(w, "monotonic") == 0) cj_which = 2;
    else if (w && strcmp(w, "both") == 0) cj_which = 3;
    /* the program's children (fid_proc_*, p_system) keep the real clocks */
    unsetenv("LD_PRELOAD");
}

static int64_t cj_offset(clockid_t id) {
    if (cj_at_ns < 0 || cj_by_ns == 0) return 0;
    int real = id == CLOCK_REALTIME || id == CLOCK_REALTIME_COARSE || id == CLOCK_TAI;
    int mono = id == CLOCK_MONOTONIC || id == CLOCK_MONOTONIC_COARSE || id == CLOCK_MONOTONIC_RAW
            || id == CLOCK_BOOTTIME;
    if (!((real && (cj_which & 1)) || (mono && (cj_which & 2)))) return 0;
    return cj_real_mono() - cj_start_ns >= cj_at_ns ? cj_by_ns : 0;
}

int clock_gettime(clockid_t id, struct timespec* ts) {
    struct timespec t;
    if (syscall(SYS_clock_gettime, id, &t) != 0) return -1;   /* errno set */
    int64_t off = cj_offset(id);
    if (off) cj_set(&t, cj_ns(&t) + off);
    if (ts) *ts = t;
    return 0;
}

int gettimeofday(struct timeval* tv, void* tz) {
    struct timespec t;
    (void)tz;
    if (clock_gettime(CLOCK_REALTIME, &t) != 0) return -1;
    if (tv) {
        tv->tv_sec = t.tv_sec;
        tv->tv_usec = (suseconds_t)(t.tv_nsec / 1000);
    }
    return 0;
}

time_t time(time_t* out) {
    struct timespec t;
    if (clock_gettime(CLOCK_REALTIME, &t) != 0) return (time_t)-1;
    if (out) *out = t.tv_sec;
    return t.tv_sec;
}
