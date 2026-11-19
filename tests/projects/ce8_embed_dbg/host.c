/* CE8: a C host of a library built with `fid ccompile --lib -g`: panics
   caught by fides_call, then more calls (build.sh). */
#include <stdio.h>
#include "dbglib.h"

int main(void) {
    int64_t r = 0;
    int rc;
    fides_init();
    int64_t ok[2] = { 7, 2 };
    rc = fides_call("scaled_div", FIDES_CAP_NONE, 2, ok, &r);
    printf("scaled_div: %d %lld\n", rc, (long long)r);
    int64_t bad[2] = { 7, 0 };
    rc = fides_call("scaled_div", FIDES_CAP_NONE, 2, bad, &r);
    printf("panic: %d %s\n", rc, fides_last_error());
    rc = fides_call("scaled_div", FIDES_CAP_NONE, 2, bad, &r);
    printf("panic again: %d %s\n", rc, fides_last_error());
    int64_t one[1] = { 21 };
    rc = fides_call("twice", FIDES_CAP_NONE, 1, one, &r);
    printf("twice: %d %lld\n", rc, (long long)r);
    printf("direct: %lld\n", (long long)twice(5));
    printf("still running\n");
    return 0;
}
