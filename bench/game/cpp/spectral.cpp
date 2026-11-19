// spectral-norm (Benchmarks Game, BM3): see bench/game/fides/spectral.fd.
#include <cmath>
#include <cstdio>
#include <vector>

static double a(size_t i, size_t j) { return 1.0 / (double)((i + j) * (i + j + 1) / 2 + i + 1); }

static void mul_av(const std::vector<double>& v, std::vector<double>& out) {
    size_t n = v.size();
    for (size_t i = 0; i < n; i++) {
        double s = 0;
        for (size_t j = 0; j < n; j++) s += a(i, j) * v[j];
        out[i] = s;
    }
}

static void mul_atv(const std::vector<double>& v, std::vector<double>& out) {
    size_t n = v.size();
    for (size_t i = 0; i < n; i++) {
        double s = 0;
        for (size_t j = 0; j < n; j++) s += a(j, i) * v[j];
        out[i] = s;
    }
}

static void mul_atav(const std::vector<double>& v, std::vector<double>& out, std::vector<double>& tmp) {
    mul_av(v, tmp);
    mul_atv(tmp, out);
}

int main() {
    const size_t n = 2000;
    std::vector<double> u(n, 1.0), v(n, 0.0), tmp(n, 0.0);
    for (int k = 0; k < 10; k++) {
        mul_atav(u, v, tmp);
        mul_atav(v, u, tmp);
    }
    double vbv = 0, vv = 0;
    for (size_t i = 0; i < n; i++) {
        vbv += u[i] * v[i];
        vv += v[i] * v[i];
    }
    std::printf("%lld\n", (long long)(std::sqrt(vbv / vv) * 1e9));
}
