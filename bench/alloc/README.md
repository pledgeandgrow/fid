# bench/alloc: allocator speed (competitive roadmap RT8)

Five programs in the shapes of mimalloc-bench's tests, each in C
(`c/*.c`, the allocator chosen by `c/alloc.h`) and in Fides (`fides/*.fd`,
`p_malloc`/`p_free`), the same blocks in the same order:

| program | shape (mimalloc-bench) | what it does |
|---------|------------------------|--------------|
| small   | alloc-test, cfrac      | 40 rounds of 100,000 blocks of 16..271 bytes, every other one freed and refilled, then all freed newest first |
| churn   | glibc-simple, rptest   | a ring of 64 live blocks, 30 million times the oldest freed and a new one of 8..135 bytes made |
| sizes   | sh6bench, sh8bench     | 20,000 slots, 4 million random replacements by blocks of 8 bytes to 64 KB (log-uniform) |
| threads | larson, xmalloc-test   | 4 threads at once, blocks freed by a thread other than the one that made them |
| large   | malloc-large           | 64 slots, 40,000 replacements by blocks of 64 KB to 4 MB, one word written per page |

`bash bench/alloc/run.sh` builds every C program five ways (the C
library's `malloc` and `calloc`, mimalloc's `mi_malloc` and `mi_zalloc`,
fid's `p_malloc` from the C runtime `fid dump-c c/rt.fd` writes) and the
Fides programs with `fid ccompile` and `fid emit`, runs each `RUNS` times
and prints the best time of each and fid's time over mimalloc's. Every
variant of a program must print the same number. mimalloc's source is
`MIMALLOC_DIR`, the copy cargo keeps for the mimalloc crate, or a clone
(`MIMALLOC_FETCH=1`); see the script's header.

fid hands out zeroed memory (a Fides allocation reads as zero), so its
fair comparison is `mi_zalloc` (`mi-z`); `mi_malloc` (`mi`) does not zero.

## Results (Nov 17 2026, Windows x64, zig cc 0.16 -O2, mimalloc 2.3.2)

Best of 3, ms, on a machine shared with other builds (a 20-30% spread
between runs). "fid before" is the C runtime as of Nov 16; "fid" is that
runtime with the RT8 changes below applied.

| program | mi       | mi-z     | fid before | fid       | fid / mi-z | fid / mi |
|---------|----------|----------|------------|-----------|------------|----------|
| small   | 512      | 1068     | 1473       | 856-1006  | 0.8-0.9    | 1.7-2.0  |
| churn   | 600      | 1815     | 2027       | 912-1187  | 0.5-0.65   | 1.5-1.9  |
| sizes   | 618-1248 | 13770    | 14821      | 3666-4331 | 0.27-0.31  | 3-7      |
| threads | 394-674  | 199-1102 | 1179       | 645-749   | about 0.7  | 1.0-1.9  |
| large   | 794-817  | 15330    | 10486      | 6784-8327 | 0.44-0.54  | 8-10     |

fid is now faster than mimalloc's zeroing allocation on all five, and
within 2x of mimalloc's plain `mi_malloc` on the small-block programs
(small, churn, threads). On sizes and large the time is the zeroing
itself (about 40 GB written on large), which `mi_zalloc` pays too.

## What changed (compiler/codegen_c.fd, the C runtime's p_malloc)

- A reused block is zeroed in the bytes asked for (rounded up to 16), not
  the whole size class (`-DP_ZERO_FULL` brings the old behaviour back).
- `__p_zero`: up to 256 bytes, two overlapping stores of a constant size
  (inline vector stores); beyond, `rep stosb` on x86-64. The C library's
  `memset` call cost more than the rest of `p_malloc` (MinGW's on
  Windows): this alone halved small, churn and threads.
- The size class from `__builtin_clzll`, not a loop over the powers of two.

Tried and left out: a per-thread cache of the mapped spans of blocks over
1 MB (4 spans, 8 MB): about 20% on large, but it keeps freed memory
resident, against RT7 (tests/bootstrap/test_mem_return.fd).

tests/bootstrap/test_alloc_reuse_zero.fd checks that a reused block is
zero in every byte asked for (every size 0..300, large ones, a smaller
request in the same class) on every backend.
