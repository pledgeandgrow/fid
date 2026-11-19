#!/usr/bin/env python3
"""Reference values of the Benchmarks Game programs of bench/game/ that take
their input from the fasta generator (roadmap BM3): reverse-complement,
k-nucleotide, regex-redux, and pidigits.

Each program prints one integer; this script computes the same integer the
plain way (Python's own `re` for regex-redux, Python integers for pidigits),
so the five languages' programs and tests/bootstrap/test_verify_bm_*.fd are
checked against an independent implementation.

    python3 bench/game/reference.py                 # the benchmark sizes
    python3 bench/game/reference.py --small         # the test sizes
    python3 bench/game/reference.py revcomp 1000    # one program, one size

Every hash is h = (h * 31 + v) & 0xFFFFFFFF over the values in order.
"""
import re
import sys

ALU = (b"GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGGGAGGCCGAGGCGGGCGGATCACCTGAGG"
       b"TCAGGAGTTCGAGACCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAATACAAAAATTAGCCGG"
       b"GCGTGGTGGCGCGCGCCTGTAATCCCAGCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGGAG"
       b"GCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCCAGCCTGGGCGACAGAGCGAGACTCCGTCTCA"
       b"AAAA")
IUB_CHARS = b"acgtBDHKMNRSVWY"
IUB_PROBS = [0.27, 0.12, 0.12, 0.27] + [0.02] * 11
HOMO_CHARS = b"acgt"
HOMO_PROBS = [0.3029549426680, 0.1979883004921, 0.1975473066391, 0.3015094502008]
MASK = 0xFFFFFFFF


def fold(h, v):
    return (h * 31 + v) & MASK


def repeat_section(m):
    k = len(ALU)
    return bytes(ALU[i % k] for i in range(m))


def random_section(seed, chars, probs, m):
    """m characters from the benchmark's generator; seed is a one-element
    list holding the generator's state, carried from one section to the next."""
    cum = []
    p = 0.0
    for q in probs:
        p = p + q
        cum.append(p)
    last_i = len(cum) - 1
    last = seed[0]
    out = bytearray(m)
    for k in range(m):
        last = (last * 3877 + 29573) % 139968
        r = last / 139968.0
        i = 0
        while i < last_i and r >= cum[i]:
            i += 1
        out[k] = chars[i]
    seed[0] = last
    return bytes(out)


def sections(n):
    seed = [42]
    one = repeat_section(2 * n)
    two = random_section(seed, IUB_CHARS, IUB_PROBS, 3 * n)
    three = random_section(seed, HOMO_CHARS, HOMO_PROBS, 5 * n)
    return one, two, three


# --- reverse-complement ------------------------------------------------------

COMP = bytes.maketrans(b"ACBDGHKMNSRUTWVYacbdghkmnsrutwvy",
                       b"TGVHCDMKNSYAAWBRTGVHCDMKNSYAAWBR")


def revcomp(n):
    h = 0
    for sec in sections(n):
        for c in sec[::-1].translate(COMP):
            h = fold(h, c)
    return h


# --- k-nucleotide ------------------------------------------------------------

def knucleotide(n):
    # THREE of fasta: the generator's state after TWO's 3n draws, 5n draws
    seed = [42]
    last = 42
    for _ in range(3 * n):
        last = (last * 3877 + 29573) % 139968
    seed[0] = last
    three = random_section(seed, HOMO_CHARS, HOMO_PROBS, 5 * n).upper()
    code = {ord("A"): 0, ord("C"): 1, ord("G"): 2, ord("T"): 3}
    codes = [code[c] for c in three]
    h = 0
    queries = {3: "GGT", 4: "GGTA", 6: "GGTATT", 12: "GGTATTTTAATT",
               18: "GGTATTTTAATTTATAGT"}
    for k in (1, 2, 3, 4, 6, 12, 18):
        mask = (1 << (2 * k)) - 1
        counts = {}
        key = 0
        for i, c in enumerate(codes):
            key = ((key << 2) | c) & mask
            if i >= k - 1:
                counts[key] = counts.get(key, 0) + 1
        h = fold(h, len(counts))
        if k <= 2:
            for key in range(1 << (2 * k)):
                h = fold(h, counts.get(key, 0))
        else:
            q = 0
            for ch in queries[k]:
                q = (q << 2) | code[ord(ch)]
            h = fold(h, counts.get(q, 0))
    return h


# --- regex-redux -------------------------------------------------------------

VARIANTS = [
    "agggtaaa|tttaccct",
    "[cgt]gggtaaa|tttaccc[acg]",
    "a[act]ggtaaa|tttacc[agt]t",
    "ag[act]gtaaa|tttac[agt]ct",
    "agg[act]taaa|ttta[agt]cct",
    "aggg[acg]aaa|ttt[cgt]ccct",
    "agggt[cgt]aa|tt[acg]accct",
    "agggta[cgt]a|t[acg]taccct",
    "agggtaa[cgt]|[acg]ttaccct",
]
SUBST = [
    ("tHa[Nt]", "<4>"),
    ("aND|caN|Ha[DS]|WaS", "<3>"),
    ("a[NSt]|BY", "<2>"),
    ("<[^>]*>", "|"),
    ("\\|[^|][^|]*\\|", "-"),
]


def fasta_text(n):
    """The fasta program's output: three headers, 60-character lines."""
    one, two, three = sections(n)
    out = []
    for title, sec in ((">ONE Homo sapiens alu", one),
                       (">TWO IUB ambiguity codes", two),
                       (">THREE Homo sapiens frequency", three)):
        out.append(title + "\n")
        s = sec.decode()
        for i in range(0, len(s), 60):
            out.append(s[i:i + 60] + "\n")
    return "".join(out)


def regexredux(n):
    seq = fasta_text(n)
    ilen = len(seq)
    seq = re.sub(">.*\n|\n", "", seq)
    clen = len(seq)
    h = 0
    for v in VARIANTS:
        h = fold(h, len(re.findall(v, seq)))
    for pat, rep in SUBST:
        seq = re.sub(pat, rep, seq)
    h = fold(h, ilen)
    h = fold(h, clen)
    h = fold(h, len(seq))
    return h


# --- pidigits ----------------------------------------------------------------

def pidigits(count):
    num, acc, den = 1, 0, 1
    k = 0
    i = 0
    h = 0
    while i < count:
        k += 1
        k2 = 2 * k + 1
        acc = (acc + num * 2) * k2
        den = den * k2
        num = num * k
        if num > acc:
            continue
        t3 = num * 3 + acc
        t4 = num * 4 + acc
        assert t3 >= 0 and den > 0
        d = t3 // den
        if d != t4 // den:
            continue
        assert 0 <= d <= 9
        h = fold(h, d)
        i += 1
        acc = (acc - den * d) * 10
        num = num * 10
    return h


PROGRAMS = {"revcomp": revcomp, "knucleotide": knucleotide,
            "regexredux": regexredux, "pidigits": pidigits}
BENCH = {"revcomp": 2500000, "knucleotide": 1000000,
         "regexredux": 500000, "pidigits": 10000}
# the sizes of tests/bootstrap/test_verify_bm_*.fd: small enough for the
# interpreter, which gives the suite's reference output
SMALL = {"revcomp": 1000, "knucleotide": 200,
         "regexredux": 200, "pidigits": 50}


def main(argv):
    if len(argv) >= 2 and argv[0] in PROGRAMS:
        print(PROGRAMS[argv[0]](int(argv[1])))
        return
    sizes = SMALL if argv[:1] == ["--small"] else BENCH
    for name, f in PROGRAMS.items():
        print(name, sizes[name], f(sizes[name]))


if __name__ == "__main__":
    main(sys.argv[1:])
