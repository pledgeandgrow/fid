// sort (BM4): see bench/work/fides/sort.fd.
#include <algorithm>
#include <cstdio>
#include <vector>

int main() {
    const long long n = 5000000;
    long long x = 12345;
    std::vector<long long> v;
    v.reserve(n);
    for (long long i = 0; i < n; i++) {
        x = (x * 1103515245 + 12345) % 2147483648LL;
        v.push_back(x);
    }
    std::sort(v.begin(), v.end());
    long long s = 0;
    for (long long i = 0; i < n; i++) s = (s + v[i] * (i % 1000)) % 1000000007;
    std::printf("%lld\n", s);
}
