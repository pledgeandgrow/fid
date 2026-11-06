#include <cstdio>
long fib(long n) { return n < 2 ? n : fib(n - 1) + fib(n - 2); }
int main() { volatile long n = 40; std::printf("%ld\n", fib(n)); }
