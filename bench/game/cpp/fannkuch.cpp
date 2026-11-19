// fannkuch-redux (Benchmarks Game, BM3): see bench/game/fides/fannkuch.fd.
#include <algorithm>
#include <cstdio>
#include <vector>

static long long fannkuch(int n) {
    std::vector<int> perm1(n), count(n), perm(n);
    for (int i = 0; i < n; i++) perm1[i] = i;
    long long maxflips = 0, checksum = 0, permcount = 0;
    int r = n;
    for (;;) {
        for (; r != 1; r--) count[r - 1] = r;
        perm = perm1;
        long long flips = 0;
        for (int k = perm[0]; k != 0; k = perm[0]) {
            std::reverse(perm.begin(), perm.begin() + k + 1);
            flips++;
        }
        maxflips = std::max(maxflips, flips);
        checksum += permcount % 2 == 0 ? flips : -flips;
        for (;;) {
            if (r == n) return checksum * 1000 + maxflips;
            std::rotate(perm1.begin(), perm1.begin() + 1, perm1.begin() + r + 1);
            if (--count[r] > 0) break;
            r++;
        }
        permcount++;
    }
}

int main() { std::printf("%lld\n", fannkuch(11)); }
