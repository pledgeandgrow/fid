// strings (BM4): see bench/work/fides/strings.fd.
#include <cstdio>
#include <string>

int main() {
    const int n = 2000000;
    std::string s;
    for (int i = 0; i < n; i++) {
        s += std::to_string(i);
        s += ',';
    }
    long long h = 0;
    for (unsigned char b : s) h = (h * 31 + b) % 1000000007;
    std::printf("%lld\n", h * 100000000 + (long long)s.size());
}
