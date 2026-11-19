#!/usr/bin/env python3
"""The checksums of bench/mixed/mixed.fd, computed independently.

  python3 bench/mixed/reference.py                 the full sizes (what
                                                   mixed.fd prints)
  python3 bench/mixed/reference.py SB VEC MAP      other sizes (the test
                                                   tests/bootstrap/
                                                   test_verify_bm_mixed.fd
                                                   uses 1000 10000 500)
  --rounds=R                                       rounds (default 10,
                                                   MIXED_ROUNDS)
Prints the sb, vec and map checksums of one round and the folded total, one
a line, then what mixed.fd prints after R rounds for each MIXED_PHASE.
"""
import sys

P = 1000000007


def mixed_sb(n):
    s = "".join("%d," % i for i in range(n)).encode()
    h = 0
    for b in s:
        h = (h * 31 + b) % P
    return (h * 1000003 + len(s)) % P


def mixed_vec(n):
    v = [i * 3 + 1 for i in range(n)]
    total = 0
    at = 0
    for k in range(n):
        total = (total + v[at] * (k % 5 + 1)) % P
        at += 7919
        if at >= n:
            at -= n
    return total


def mixed_map(n):
    m = {"k%d" % i: i * 7 for i in range(n)}
    total = 0
    for i in range(n):
        total = (total + m["k%d" % i]) % P
    missing = sum(1 for i in range(n // 4) if ("x%d" % i) not in m)
    return (total * 31 + missing + len(m)) % P


def mixed(a_n, b_n, c_n):
    a = mixed_sb(a_n)
    b = mixed_vec(b_n)
    c = mixed_map(c_n)
    return a, b, c, ((a * 1000003 + b) % P * 1000003 + c) % P


def rounds(x, r):
    """mixed_rounds: every round computes the same x; fold it with r."""
    acc = 0
    for k in range(r):
        acc = (acc * 1000003 + x + k) % P
    return acc


if __name__ == "__main__":
    args = [a for a in sys.argv[1:] if not a.startswith("--rounds=")]
    r = 10
    for a in sys.argv[1:]:
        if a.startswith("--rounds="):
            r = int(a.split("=", 1)[1])
    sizes = [int(x) for x in args[0:3]] if len(args) >= 3 else [200000, 2000000, 100000]
    a, b, c, t = mixed(*sizes)
    for x in (a, b, c, t):
        print(x)
    # what mixed.fd prints: MIXED_PHASE=sb, vec, map, then all (r rounds)
    print("rounds=%d: sb %d vec %d map %d all %d" % (r, rounds(a, r), rounds(b, r), rounds(c, r), rounds(t, r)))
