# Benchmarks

The same programs in Fides, Rust, Zig, C++ and Go, built and timed with one
command (competitive roadmap BM1):

```bash
./bench/run.sh              # every program, best of 3
RUNS=5 ./bench/run.sh fib   # one program, best of 5
FID=release/fid.exe ./bench/run.sh
```

Each program prints one number. A language whose output differs from the
others is shown as `WRONG`, not timed; a missing toolchain or program as `-`.
Fides is measured twice: native (`fid emit`) and through C (`fid ccompile`).
Rivals are built as their users ship them: `rustc -C opt-level=3`,
`zig -O ReleaseFast`, `clang++ -O2` (a real Clang when one is on the PATH,
`zig c++` otherwise), `g++ -O2` when GCC is on the PATH, `go build`.

| Program | What it measures |
|---|---|
| `fib` | Recursive calls: fib(40) |
| `loop` | Integer arithmetic in a counted loop, 2 billion iterations |
| `sieve` | A 20-million-entry array, written and read in loops |
| `hashmap` | One million string keys inserted and looked up |

Each program is written the way a good programmer of that language would
write it, not translated word for word: Rust's `vec![true; n]`, C++'s
`std::vector<bool>`, Go's `[]bool`, Fides's `Vec<u8>` (one byte per element
since release D14; it was a `Vec<i64>` before). Zig has no `hashmap` program yet.

## Results

Nov 3, 2026, Windows 11 x64, one laptop, not a quiet machine; seconds, best
of 3, wall clock. `fid` built from the working tree (proposal 0002 in, copy-
on-write off); rustc 1.97.0, zig 0.16.0 (also C++ through `zig c++`), Go
1.27.3.

| Benchmark | Fides native | Fides via C | Rust | Zig | C++ | Go |
|---|---|---|---|---|---|---|
| fib | 1.37 | 0.92 | 0.59 | 0.64 | 0.61 | 0.96 |
| hashmap | over 60 s | 0.85 | 1.34 | - | 1.29 | 0.94 |
| loop | 8.58 | 3.78 | 3.21 | 3.76 | 3.99 | 3.12 |
| sieve | 0.80 | 0.55 | 0.26 | 0.29 | 0.26 | 0.30 |

- Through C, Fides is within 20% of Rust on `loop`, and faster than Rust, C++
  and Go on `hashmap`; it is 1.5x Rust on `fib` and 2x on `sieve` (each
  element is a word, not a byte: RT1).
- Native output is 2–3x slower than through C: the native backend has no
  optimizer yet (OP1–OP19).
- Native `hashmap` is quadratic: 20,000 keys take 4 s natively and 0.3 s
  through C, in the last commit too (it took 1.04 s for a million keys on
  Nov 2). The native string hash (`pert_str_hash`, RT4) is the first suspect.
- `HashMap` keys must be strings for now: core hashes every key as a `str`,
  so `HashMap<i64, i64>` crashes on every backend.

## Slowdown check

`./bench/check.sh --save` records the Fides times (native and through C) on
this machine in `bench/baseline.txt`; `./bench/check.sh` then exits 1 when a
program got more than 10% slower (`LIMIT_PCT` to change it). Run it in CI or
before a change to code generation lands (release D7).

In CI (`.github/workflows/bench.yml`, BM9) the base commit's compiler and the
head's run in turn on one runner, `BASE_FID=... ./bench/check.sh --ab` with
`LIMIT_PCT=5`: best of `RUNS` each, and a program over the limit is measured
again before it fails (`SLACK_MS`, default 5, absorbs timer noise on short
programs).

## More sets and measures (Nov 15)

| Command | Roadmap | What |
|---|---|---|
| `SUITE=game ./bench/run.sh` | BM3 | `bench/game/`: n-body (5M steps), spectral-norm (n = 2000), mandelbrot (4000 x 4000), fannkuch-redux (11), binary-trees (depths 4..16, one allocation per node), fasta (n = 2.5M, its 25M characters folded into a 32-bit hash instead of written), reverse-complement (fasta's three sections for n = 2.5M, made in memory, each reversed and complemented in place), k-nucleotide (fasta's THREE section for n = 1M: hash tables of every 1-, 2-, 3-, 4-, 6-, 12- and 18-long substring), regex-redux (fasta's output for n = 500000: nine variant counts and five substitutions; Rust and Zig have no regular expressions in their standard libraries, so all five run the same small backtracking matcher) and pidigits (10000 digits, Gibbons' spigot; big integers from `stdlib/bigint.fd`, Go's `math/big`, and a small one of their own in Rust, C++ and Zig), in all five languages; each prints one integer (the energy or norm times 1e9, the points in the set, checksum * 1000 + flips, the nodes of every tree, a 32-bit hash of the output), the arithmetic in the same order everywhere so every language must print the same number. `tests/bootstrap/test_verify_bm_*.fd` are the Fides programs on small inputs, checked by the suite against the reference algorithm's values; `python3 bench/game/reference.py` computes the values of the last four independently (Python's `re` and integers). |
| `SUITE=work ./bench/run.sh` | BM4 | `bench/work/`: real work, not loops: sorting 5M integers with each language's own sort, building a 2M-number string with its string builder and hashing it, FNV-1a over 50 MB, a small compiler (generate 50000 lines of `let vK = <expr>;`, 11 MB, then lex, parse into boxed trees and evaluate; Zig has no `strings` program yet), JSON parse and print (15.9 MB of generated JSON parsed into an arena of nodes and printed back compact, 3 rounds), regex (a Thompson NFA, 8 patterns over 200000 generated lines) and file reading (a 50 MB text file, `bench/out/fileread.dat`, written by the first program that needs it, read 3 times and its numbers summed). JSON and regex are written by hand, the same algorithm in every language: Rust has neither in its standard library and `run.sh` fetches no crates, so Go and C++ do not use theirs either. `tests/bootstrap/test_verify_bm_{sort,strings,hash,compiler,json,regex,fileread}.fd` check the Fides ones. |
| `./bench/size.sh` | BM7 | Binary size, startup time (hello, best of 20) and peak memory (`bench/peakmem.py`: ru_maxrss, or the peak working set on Windows) of `bench/size/hello` and every `bench/fides/` program, every language. First recorded run: [docs/perf/size-2026-11-16.md](../docs/perf/size-2026-11-16.md). |
| `./bench/compile-speed.sh` | BM6 | `bench/gen_lines.py` writes the same program of N lines in each language; each is built clean and after a one-line change, plus `fid check` alone. `SIZES="hello 10000 100000 1000000"`. First recorded run (hello, 10k, 100k): [docs/benchmarks.md](../docs/benchmarks.md), Nov 16; 1M on Nov 17 (`fid check` 3422 s, the rivals' clean builds 82–185 s; the native build stopped after 5.5 h, through C and the change row not run). The footer gives the load average before and after. |
| `bash bench/service/run.sh` | BM5 | TechEmpower-style service tests (plaintext, JSON, one row, 20 rows) for Fides, Go and Node, under wrk or `bench/service/load.py`; `serve` / `load` put the client on another machine. See [service/README.md](service/README.md). |
| `bash bench/mixed.sh` | pe 89 | The mixed workload of pe-roadmap item 89 (`bench/mixed/mixed.fd`: 200k StringBuilder appends, 2M Vec pushes and gets, 100k HashMap inserts and lookups, ten rounds), `fid emit` against `fid ccompile --cc-opt 0`, each part alone too (`MIXED_PHASE`); checksums from `bench/mixed/reference.py`; exits 1 over 2x. First run: [docs/verification/a11-m7-lint-mixed-2026-11-18.md](../docs/verification/a11-m7-lint-mixed-2026-11-18.md) (native 0.27x of C -O0). |
| `bash bench/alloc/run.sh` | RT8 | `bench/alloc/`: five allocator programs (small, churn, sizes, threads, large: mimalloc-bench's shapes) in C, built with the C library's malloc/calloc, mimalloc's mi_malloc/mi_zalloc and fid's p_malloc (the C runtime of `fid dump-c bench/alloc/c/rt.fd`), and in Fides through ccompile and emit; prints the best time of each and fid over mimalloc. See bench/alloc/README.md. |
| `bench/versions.txt` | BM2 | The pinned toolchain versions (rustc, zig, clang, go and gcc); `run.sh` names any installed version that differs. `run.sh` prefers a real `clang++` for the C++ (Clang) column and also has a C++ (GCC) column (`g++ -O2`, when on the PATH; on Windows without one, the g++ of WSL, built and timed inside WSL and marked so under the table; `GCC_WSL=0` turns that off). |
| `./bench/history.sh <tables.md> "<machine>"` | BM10 | Adds a run's tables, unchanged, to `docs/benchmarks.md` (newest first). |

`run.sh` reports the spread of the runs beside each best time (BM8) and the
load average before and after; on Linux "Fides native" is the machine layer
(`emit --target x86_64-linux`). `.github/workflows/verify-weekly.yml` runs
`run.sh` and `size.sh` on Linux every week and keeps the tables as an
artifact.

Still to come (roadmap BM5–BM10): Benchmarks Game numbers from a quiet
machine (the Nov 17 table in docs/benchmarks.md is from a busy one); service
benchmarks with the client on another machine (BM5); published Linux runs
and results kept over time on the website.
