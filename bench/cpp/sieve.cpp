#include <cstdio>
#include <vector>
int main() {
    const size_t n = 20000000; std::vector<bool> is(n, true); is[0] = is[1] = false;
    for (size_t p = 2; p * p < n; p++) if (is[p]) for (size_t m = p * p; m < n; m += p) is[m] = false;
    size_t c = 0; for (size_t i = 0; i < n; i++) c += is[i];
    std::printf("%zu\n", c);
}
