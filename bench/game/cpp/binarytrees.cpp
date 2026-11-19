// binary-trees (Benchmarks Game, BM3): see bench/game/fides/binarytrees.fd.
// Each node is its own allocation (new/delete), as in the Fides, Rust and
// Zig programs; a Leaf is nullptr.
#include <cstdint>
#include <cstdio>

struct Node {
    Node *l, *r;
};

static Node *make(int d) {
    if (d == 0) return new Node{nullptr, nullptr};
    Node *l = make(d - 1);
    Node *r = make(d - 1);
    return new Node{l, r};
}

static int64_t check(const Node *t) {
    if (!t) return 0;
    return 1 + check(t->l) + check(t->r);
}

static void drop(Node *t) {
    if (!t) return;
    drop(t->l);
    drop(t->r);
    delete t;
}

static int64_t trees(int min_depth, int max_depth) {
    Node *s = make(max_depth + 1);
    int64_t total = check(s);
    drop(s);
    Node *long_lived = make(max_depth);
    for (int d = min_depth; d <= max_depth; d += 2) {
        int64_t iters = (int64_t)1 << (max_depth - d + min_depth);
        for (int64_t i = 0; i < iters; i++) {
            Node *t = make(d);
            total += check(t);
            drop(t);
        }
    }
    total += check(long_lived);
    drop(long_lived);
    return total;
}

int main() {
    printf("%lld\n", (long long)trees(4, 16));
    return 0;
}
