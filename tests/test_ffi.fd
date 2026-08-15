// Test FFI — extern "C" declarations and calling C library functions
// Phase 3D

// Declare extern C functions from the C standard library
// Using `as` to map C names to Pledge names (avoids conflicts with stdlib)
extern "C" include "<math.h>" link "m" {
    pf sqrt as pledge_sqrt(x: f64) -> f64;
    pf pow as pledge_pow(base: f64, exp: f64) -> f64;
    pf floor as pledge_floor(x: f64) -> f64;
    pf ceil as pledge_ceil(x: f64) -> f64;
}

extern "C" include "<string.h>" {
    pf strlen as pledge_strlen(s: str) -> u64;
}

pf main() -> i64 {
    // Call C's sqrt function (from libm)
    pv s = pledge_sqrt(16.0);
    print(s);  // 4

    // Call C's pow function
    pv p = pledge_pow(2.0, 10.0);
    print(p);  // 1024

    // Call C's floor function
    pv f = pledge_floor(3.7);
    print(f);  // 3

    // Call C's strlen function
    pv len = pledge_strlen("hello world");
    print(len);  // 11

    ret 0;
}
