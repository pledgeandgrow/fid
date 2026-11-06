#include <cstdio>
int main() { volatile long long n = 2000000000LL; long long s = 0; for (long long i = 0; i < n; i++) s += (i % 7) * 3 - 1; std::printf("%lld\n", s); }
