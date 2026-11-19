// mandelbrot (Benchmarks Game, BM3): see bench/game/fides/mandelbrot.fd.
#include <cstdio>

int main() {
    const int n = 4000;
    const double nf = n;
    long long count = 0;
    for (int y = 0; y < n; y++) {
        double ci = 2.0 * (double)y / nf - 1.0;
        for (int x = 0; x < n; x++) {
            double cr = 2.0 * (double)x / nf - 1.5;
            double zr = 0, zi = 0, tr = 0, ti = 0;
            for (int i = 0; i < 50 && tr + ti <= 4.0; i++) {
                zi = 2.0 * zr * zi + ci;
                zr = tr - ti + cr;
                tr = zr * zr;
                ti = zi * zi;
            }
            if (tr + ti <= 4.0) count++;
        }
    }
    std::printf("%lld\n", count);
}
