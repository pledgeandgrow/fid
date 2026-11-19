// fileread (BM4): see bench/work/fides/fileread.fd. Reads a 50 MB text
// file three times and sums the numbers in it; writes the file first when
// it is not there.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

static const char* PATH = "out/fileread.dat";
static const int64_t LINES = 2150000;
static const long SIZE = 49994158;

static long file_size() {
    FILE* f = fopen(PATH, "rb");
    if (!f) return -1;
    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    fclose(f);
    return n;
}

static void generate() {
    std::vector<char> o;
    o.reserve(SIZE);
    int64_t s = 1;
    char line[64];
    for (int64_t k = 0; k < LINES; k++) {
        s = (s * 1103515245 + 12345) % 2147483648LL;
        int64_t r1 = s >> 8;
        s = (s * 1103515245 + 12345) % 2147483648LL;
        int64_t r2 = s >> 8;
        int n = snprintf(line, sizeof line, "rec%lld %lld x%lld\n", (long long)k, (long long)(r1 % 1000000), (long long)(r2 % 10000));
        o.insert(o.end(), line, line + n);
    }
    FILE* f = fopen(PATH, "wb");
    if (!f || fwrite(o.data(), 1, o.size(), f) != o.size()) {
        printf("cannot write %s\n", PATH);
        exit(1);
    }
    fclose(f);
}

static int64_t scan(const unsigned char* d, size_t n) {
    int64_t tot = 0, cur = 0, lines = 0;
    bool in = false;
    for (size_t i = 0; i < n; i++) {
        unsigned char b = d[i];
        if (b >= '0' && b <= '9') {
            cur = cur * 10 + (b - 48);
            in = true;
        } else {
            if (in) {
                tot += cur;
                cur = 0;
                in = false;
            }
            if (b == '\n') lines++;
        }
    }
    if (in) tot += cur;
    return tot + lines;
}

int main() {
    if (file_size() != SIZE) generate();
    int64_t total = 0;
    for (int r = 0; r < 3; r++) {
        FILE* f = fopen(PATH, "rb");
        if (!f) {
            printf("cannot read %s\n", PATH);
            return 1;
        }
        fseek(f, 0, SEEK_END);
        long n = ftell(f);
        fseek(f, 0, SEEK_SET);
        std::vector<unsigned char> d((size_t)n);
        size_t got = fread(d.data(), 1, (size_t)n, f);
        fclose(f);
        total += scan(d.data(), got);
    }
    printf("%lld\n", (long long)total);
    return 0;
}
