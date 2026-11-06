#include <cstdio>
#include <string>
#include <unordered_map>
int main() {
    const long long n = 1000000; std::unordered_map<std::string, long long> m;
    for (long long i = 0; i < n; i++) m["key" + std::to_string(i)] = i;
    long long sum = 0; for (long long i = 0; i < n; i++) sum += m["key" + std::to_string(i)];
    std::printf("%lld\n", sum);
}
