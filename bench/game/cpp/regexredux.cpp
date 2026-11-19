// regex-redux (Benchmarks Game, BM3): see bench/game/fides/regexredux.fd.
// C++ has <regex>, but Rust and Zig have no regular expressions in their
// standard libraries, so every language of bench/game/ runs this same small
// backtracking matcher.
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

static const char *ALU = "GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGGGAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGACCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAATACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCAGCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGGAGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCCAGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA";

struct Item {
    bool set[256];
    bool star;
};

struct Pat {
    std::vector<std::vector<Item>> alts;
    bool first[256];
};

typedef std::vector<uint8_t> Bytes;

static Pat compile(const char *pattern) {
    const uint8_t *b = (const uint8_t *)pattern;
    size_t n = strlen(pattern), i = 0;
    Pat p;
    p.alts.emplace_back();
    while (i < n) {
        uint8_t c = b[i];
        if (c == '|') {
            p.alts.emplace_back();
            i++;
            continue;
        }
        Item it;
        memset(it.set, 0, sizeof it.set);
        if (c == '[') {
            i++;
            bool neg = b[i] == '^';
            if (neg) i++;
            while (b[i] != ']') it.set[b[i++]] = true;
            i++;
            if (neg)
                for (int z = 0; z < 256; z++) it.set[z] = !it.set[z];
        } else if (c == '\\') {
            it.set[b[i + 1]] = true;
            i += 2;
        } else if (c == '.') {
            for (int z = 0; z < 256; z++) it.set[z] = z != '\n';
            i++;
        } else {
            it.set[c] = true;
            i++;
        }
        it.star = i < n && b[i] == '*';
        if (it.star) i++;
        p.alts.back().push_back(it);
    }
    memset(p.first, 0, sizeof p.first);
    for (auto &alt : p.alts)
        for (int z = 0; z < 256; z++)
            if (alt[0].star || alt[0].set[z]) p.first[z] = true;
    return p;
}

// where a match of items it .. end - 1 that starts at pos ends, or -1
static long match_at(const Item *it, const Item *end, const Bytes &s, long pos) {
    if (it == end) return pos;
    long len = (long)s.size();
    if (it->star) {
        long j = pos;
        while (j < len && it->set[s[j]]) j++;
        for (; j >= pos; j--) {
            long e = match_at(it + 1, end, s, j);
            if (e >= 0) return e;
        }
        return -1;
    }
    if (pos < len && it->set[s[pos]]) return match_at(it + 1, end, s, pos + 1);
    return -1;
}

// where the match that starts at pos ends (the first alternative that
// matches), or -1
static long find_at(const Pat &p, const Bytes &s, long pos) {
    if (!p.first[s[pos]]) return -1;
    for (auto &alt : p.alts) {
        long e = match_at(alt.data(), alt.data() + alt.size(), s, pos);
        if (e > pos) return e;
    }
    return -1;
}

static uint64_t count_matches(const char *pattern, const Bytes &s) {
    Pat p = compile(pattern);
    uint64_t n = 0;
    long pos = 0, len = (long)s.size();
    while (pos < len) {
        long e = find_at(p, s, pos);
        if (e > pos) {
            n++;
            pos = e;
        } else {
            pos++;
        }
    }
    return n;
}

static Bytes replace_all(const char *pattern, const Bytes &s, const char *rep) {
    Pat p = compile(pattern);
    size_t rlen = strlen(rep);
    Bytes out;
    out.reserve(s.size());
    long pos = 0, len = (long)s.size();
    while (pos < len) {
        long e = find_at(p, s, pos);
        if (e > pos) {
            out.insert(out.end(), rep, rep + rlen);
            pos = e;
        } else {
            out.push_back(s[pos]);
            pos++;
        }
    }
    return out;
}

static Bytes random_section(int64_t *seed, const char *chars, const std::vector<double> &probs, long m) {
    std::vector<double> cum;
    double p = 0.0;
    for (double q : probs) {
        p += q;
        cum.push_back(p);
    }
    size_t last_i = cum.size() - 1;
    int64_t last = *seed;
    Bytes out(m);
    for (long k = 0; k < m; k++) {
        last = (last * 3877 + 29573) % 139968;
        double r = (double)last / 139968.0;
        size_t i = 0;
        while (i < last_i && r >= cum[i]) i++;
        out[k] = (uint8_t)chars[i];
    }
    *seed = last;
    return out;
}

// a header line, then the section in lines of 60 characters
static void put_section(Bytes &out, const char *title, const Bytes &sec) {
    out.insert(out.end(), title, title + strlen(title));
    out.push_back('\n');
    for (size_t i = 0; i < sec.size(); i += 60) {
        size_t e = i + 60 < sec.size() ? i + 60 : sec.size();
        out.insert(out.end(), sec.begin() + i, sec.begin() + e);
        out.push_back('\n');
    }
}

static Bytes fasta_text(long n) {
    int64_t seed = 42;
    std::vector<double> iub = {0.27, 0.12, 0.12, 0.27};
    for (int j = 0; j < 11; j++) iub.push_back(0.02);
    std::vector<double> homo = {0.3029549426680, 0.1979883004921, 0.1975473066391, 0.3015094502008};
    size_t alen = strlen(ALU);
    Bytes one(2 * n);
    for (long k = 0; k < 2 * n; k++) one[k] = (uint8_t)ALU[k % alen];
    Bytes two = random_section(&seed, "acgtBDHKMNRSVWY", iub, 3 * n);
    Bytes three = random_section(&seed, "acgt", homo, 5 * n);
    Bytes out;
    put_section(out, ">ONE Homo sapiens alu", one);
    put_section(out, ">TWO IUB ambiguity codes", two);
    put_section(out, ">THREE Homo sapiens frequency", three);
    return out;
}

static uint64_t fold(uint64_t h, uint64_t v) { return (h * 31 + v) & 0xFFFFFFFFu; }

static uint64_t regexredux(long n) {
    Bytes text = fasta_text(n);
    Bytes seq = replace_all(">.*\n|\n", text, "");
    uint64_t h = 0;
    static const char *variants[] = {
        "agggtaaa|tttaccct",         "[cgt]gggtaaa|tttaccc[acg]", "a[act]ggtaaa|tttacc[agt]t",
        "ag[act]gtaaa|tttac[agt]ct", "agg[act]taaa|ttta[agt]cct", "aggg[acg]aaa|ttt[cgt]ccct",
        "agggt[cgt]aa|tt[acg]accct", "agggta[cgt]a|t[acg]taccct", "agggtaa[cgt]|[acg]ttaccct",
    };
    for (const char *v : variants) h = fold(h, count_matches(v, seq));
    static const char *subst[][2] = {
        {"tHa[Nt]", "<4>"}, {"aND|caN|Ha[DS]|WaS", "<3>"}, {"a[NSt]|BY", "<2>"}, {"<[^>]*>", "|"}, {"\\|[^|][^|]*\\|", "-"},
    };
    Bytes s = seq;
    for (auto &sub : subst) s = replace_all(sub[0], s, sub[1]);
    h = fold(h, text.size());
    h = fold(h, seq.size());
    return fold(h, s.size());
}

int main() {
    printf("%llu\n", (unsigned long long)regexredux(500000));
    return 0;
}
