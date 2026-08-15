// Test FFI — comprehensive C library calls
// Phase 3D success metric: Call C library functions from Pledge

// Declare extern C functions from math.h and string.h
extern "C" include "<math.h>" link "m" {
    pf sqrt as c_sqrt(x: f64) -> f64;
    pf pow as c_pow(base: f64, exp: f64) -> f64;
    pf floor as c_floor(x: f64) -> f64;
    pf ceil as c_ceil(x: f64) -> f64;
    pf sin as c_sin(x: f64) -> f64;
    pf cos as c_cos(x: f64) -> f64;
    pf fabs as c_fabs(x: f64) -> f64;
}

extern "C" include "<string.h>" {
    pf strlen as c_strlen(s: str) -> u64;
    pf strcmp as c_strcmp(a: str, b: str) -> i32;
}

pf main() -> i64 {
    // Math functions from libm
    pv s = c_sqrt(144.0);
    print(s);  // 12

    pv p = c_pow(2.0, 8.0);
    print(p);  // 256

    pv f = c_floor(3.9);
    print(f);  // 3

    pv c = c_ceil(3.1);
    print(c);  // 4

    pv si = c_sin(0.0);
    print(si);  // 0

    pv co = c_cos(0.0);
    print(co);  // 1

    pv ab = c_fabs(-5.5);
    print(ab);  // 5.5

    // String functions from libc
    pv len = c_strlen("hello");
    print(len);  // 5

    pv cmp = c_strcmp("abc", "abc");
    print(cmp);  // 0

    ret 0;
}
