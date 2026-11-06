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
`zig -O ReleaseFast`, `zig c++ -O2` (Clang), `go build`.

| Program | What it measures |
|---|---|
| `fib` | Recursive calls: fib(40) |
| `loop` | Integer arithmetic in a counted loop, 2 billion iterations |
| `sieve` | A 20-million-entry array, written and read in loops |
| `hashmap` | One million string keys inserted and looked up |

Each program is written the way a good programmer of that language would
write it, not translated word for word: Rust's `vec![true; n]`, C++'s
`std::vector<bool>`, Go's `[]bool`, Fides's `Vec<i64>` (a `Vec<u8>` does not
store bytes yet, roadmap RT1). Zig has no `hashmap` program yet.

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

Still to come (roadmap BM3–BM10): the Benchmarks Game set, real-work
programs, service, compile-speed and size benchmarks, Linux runs, a CI
check on slowdowns, and a public results page.
