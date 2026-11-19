/* A C program embedding the Fides library mathlib (CI7, CI6). */
#include <stdio.h>
#include "mathlib.h"

int main(void) {
    int64_t r = 0;
    int rc;
    fides_init();
    printf("add %lld\n", (long long)add(2, 3));
    const char* g = greet("C");
    printf("%s\n", g);
    fides_str_free(g);
    int64_t args[2] = { 40, 2 };
    rc = fides_call("add", FIDES_CAP_NONE, 2, args, &r);
    printf("call add: %d %lld\n", rc, (long long)r);
    rc = fides_call("can_read", FIDES_CAP_NONE, 0, NULL, &r);
    printf("without FS: %d %lld\n", rc, (long long)r);
    rc = fides_call("can_read", FIDES_CAP_FS, 0, NULL, &r);
    printf("with FS: %d %lld\n", rc, (long long)r);
    int64_t zero[1] = { 0 };
    rc = fides_call("boom", FIDES_CAP_ALL, 1, zero, &r);
    printf("panic: %d %s\n", rc, fides_last_error());
    rc = fides_call("add", FIDES_CAP_ALL, 1, args, &r);
    printf("wrong count: %d\n", rc);
    rc = fides_call("nope", FIDES_CAP_ALL, 0, NULL, &r);
    printf("missing: %d\n", rc);

    /* CI6: a thread or task started inside a call has the call's mask */
    rc = fides_call("thread_can_read", FIDES_CAP_NONE, 0, NULL, &r);
    printf("thread without FS: %d %lld\n", rc, (long long)r);
    rc = fides_call("thread_can_read", FIDES_CAP_FS, 0, NULL, &r);
    printf("thread with FS: %d %lld\n", rc, (long long)r);
    rc = fides_call("task_can_read", FIDES_CAP_NONE, 0, NULL, &r);
    printf("task without FS: %d %lld\n", rc, (long long)r);
    rc = fides_call("task_can_read", FIDES_CAP_FS, 0, NULL, &r);
    printf("task with FS: %d %lld\n", rc, (long long)r);
    /* started in a call without FS, run after it by a direct call (every
       capability): still without FS */
    int64_t t = 0;
    rc = fides_call("task_start", FIDES_CAP_NONE, 0, NULL, &t);
    printf("task run later without FS: %d %lld\n", rc, (long long)task_finish(t));
    rc = fides_call("task_start", FIDES_CAP_FS, 0, NULL, &t);
    printf("task run later with FS: %d %lld\n", rc, (long long)task_finish(t));

    /* CI6: what a panicking call allocated is freed (the next block of its
       size is the same block); what a call that returns allocated is kept */
    int64_t n[1] = { 4000 };
    int64_t a = probe(4000);
    int64_t kept = 0;
    rc = fides_call("alloc_keep", FIDES_CAP_ALL, 1, n, &kept);
    printf("kept after a return: %s\n", (rc == 0 && kept == a && probe(4000) != a) ? "yes" : "no");
    release(kept);
    a = probe(4000);
    rc = fides_call("fail_alloc", FIDES_CAP_ALL, 1, n, &r);
    printf("panic: %d %s\n", rc, fides_last_error());
    printf("freed after a panic: %s\n", probe(4000) == a ? "yes" : "no");
    /* many panicking calls: the same block every time */
    int same = 1;
    for (int i = 0; i < 1000; i++) {
        rc = fides_call("fail_alloc", FIDES_CAP_ALL, 1, n, &r);
        if (rc != 3 || probe(4000) != a) same = 0;
    }
    printf("1000 panics, no growth: %s\n", same ? "yes" : "no");
    printf("still running\n");
    return 0;
}
