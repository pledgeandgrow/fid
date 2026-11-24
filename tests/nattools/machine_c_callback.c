/* Bugs roadmap BG166: C calls program functions handed to it (a Fides
 * function's name as a `fn(...)` argument of an `extern "C"` function).
 * The library tests/nattools/machine_c_callback.fd links ("cbk"):
 *
 *   zig cc -target x86_64-linux-gnu -shared -fPIC -O1 \
 *       -o libcbk.so tests/nattools/machine_c_callback.c
 *
 * (aarch64-linux-gnu, riscv64-linux-gnu likewise); see
 * tests/nattools/machine_c_callback_check.sh.
 */
#include <stdint.h>

/* f64 argument and result, an integer after it */
double cbk_d(double (*f)(double, int64_t), double x) {
    return f(x, 3) + f(x, 4);
}

/* eight integers: System V passes 7 and 8 on the stack */
int64_t cbk_i8(int64_t (*f)(int64_t, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t, int64_t)) {
    return f(1, 2, 3, 4, 5, 6, 7, 8);
}

/* seven integers and ten doubles interleaved: both classes run out of
 * registers, so the stack holds integers and doubles in their order */
typedef double (*mixfn)(int64_t, double, int64_t, double, int64_t, double, int64_t, double,
                        int64_t, double, int64_t, double, int64_t, double, double, double, double);
double cbk_mix(mixfn f) {
    return f(1, 0.5, 2, 1.5, 3, 2.5, 4, 3.5, 5, 4.5, 6, 5.5, 7, 6.5, 8.5, 9.5, 10.5);
}

/* an f32 argument and result, narrow integers (C leaves the bits above
 * them undefined) */
float cbk_n(float (*f)(float, int8_t, uint16_t, int32_t, uint8_t, double), float a) {
    return f(a, -3, 60000, -100000, 200, 0.25);
}

/* an i32 result */
int32_t cbk_w(int32_t (*f)(int32_t), int32_t x) {
    return f(x) * 2;
}

/* the function pointer is the sixth argument of this call */
int64_t cbk_late(int64_t a, int64_t b, int64_t c, int64_t d, int64_t e,
                 int64_t (*f)(int64_t, int64_t)) {
    return f(a + b + c, d + e);
}
