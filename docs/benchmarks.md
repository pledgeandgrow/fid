# Benchmarks

What we measure, how to reproduce it, and every result the repository has,
losses included (roadmap BM10, AD13). If a number here is wrong, or a rival
program is not the way a good programmer of that language would write it,
[open an issue](#corrections-welcome).

## What is measured

Four programs, each written in Fides, Rust, Zig, C++ and Go, in
[`bench/`](../bench/README.md):

| Program | What it measures | Prints |
|---|---|---|
| `fib` | Recursive calls: `fib(40)` | 102334155 |
| `loop` | Integer arithmetic in a counted loop, 2 billion iterations | one sum |
| `sieve` | A 20-million-entry array, written and read in loops | the number of primes below 20M |
| `hashmap` | One million string keys inserted, then looked up | a sum |

Each program prints one number; a language that prints a different number is
shown as `WRONG`, not timed. Fides is measured twice: native (`fid emit`, the
compiler's own Windows backend, no C compiler) and through C (`fid ccompile`,
`zig cc`). The rivals are built as their users ship them: `rustc -C
opt-level=3`, `zig build-exe -O ReleaseFast`, `zig c++ -O2` (Clang), `go
build`. Each rival is written in its own idiom, not translated word for word
(Rust's `vec![true; n]`, C++'s `std::vector<bool>`, Go's `[]bool`, Fides's
`Vec<u8>`). Zig has no `hashmap` program yet.

Times are wall clock, best of N runs, of the whole process (start-up
included), in seconds.

Not measured yet (roadmap BM3–BM8): the Benchmarks Game set, real work
(JSON, regex, sorting, a small compiler), service benchmarks with the client
on another machine, compile speed at 10k–1M lines against rivals, binary size
and memory, Linux runs, several runs with the spread reported.

## How to reproduce

```bash
./bench/run.sh                    # every program, every language, best of 3
RUNS=5 ./bench/run.sh fib sieve   # some programs, best of 5
FID=release/fid.exe ./bench/run.sh
```

`run.sh` builds every program (`bench/out/`), checks that all languages print
the same number, and prints a Markdown table with the toolchain versions. A
missing toolchain shows as `-`; a run over `LIMIT` seconds (default 120) as
"over 120 s". It needs Bash (Git Bash on Windows), `zig` 0.16 (for `zig cc`,
Zig and C++), and `rustc` and `go` for those columns.

`./bench/check.sh` measures the Fides programs only and fails when one is more
than `LIMIT_PCT` (default 10) percent slower than `bench/baseline.txt`
(`--save` records it; the file is not in the repository: a baseline is only
meaningful on the machine that recorded it). `bench/compile-check.sh` and
`scripts/compile-gate.sh` do the same for the compiler's per-stage times
(`fid emit --time`). The CI workflow `.github/workflows/bench.yml` builds the
base commit's and the head's compiler on one runner and runs both checks;
its results are not kept in the repository.

## Results

### Nov 3, 2026 (bench/README.md)

Windows 11 x64, one laptop, not a quiet machine; best of 3. `fid` built from
the working tree of that day (generics in, copy-on-write off); rustc 1.97.0,
zig 0.16.0 (also C++ through `zig c++`), Go 1.27.3.

| Benchmark | Fides native | Fides via C | Rust | Zig | C++ | Go |
|---|---|---|---|---|---|---|
| fib | 1.37 | 0.92 | 0.59 | 0.64 | 0.61 | 0.96 |
| hashmap | over 60 s | 0.85 | 1.34 | - | 1.29 | 0.94 |
| loop | 8.58 | 3.78 | 3.21 | 3.76 | 3.99 | 3.12 |
| sieve | 0.80 | 0.55 | 0.26 | 0.29 | 0.26 | 0.30 |

What it says: through C, Fides is within 20% of Rust on `loop` and faster than
Rust, C++ and Go on `hashmap`, 1.5x Rust on `fib` and 2x on `sieve`. Native
output was 2–3x slower than through C (no optimizer in the native backend
then), and native `hashmap` was quadratic (20,000 keys: 4 s natively, 0.3 s
through C).

### Nov 2, 2026 (competitive-roadmap.md, "The scoreboard")

Windows x64, best of 3, Fides and Rust only (the other columns were not
measured). The earlier run, kept because it differs from Nov 3's on the same
machine: Rust's `loop` took 5.36 s here and 3.21 s a day later, which says
how noisy one laptop is.

| Benchmark | Fides native | Fides via C | Rust |
|---|---|---|---|
| fib(40) | 1.34 | 0.81 | 0.53 |
| loop, 2 billion | 7.59 | 3.53 | 5.36 |
| sieve, 20M | 0.82 | 0.48 | 0.40 |
| hashmap, 1M strings | 1.04 | 0.80 | 1.12 |

### Nov 15, 2026 (this page's own run)

Run by the agent that wrote this page, 18:00–18:18 on Nov 15, on a **busy
shared machine**: AMD Ryzen 7 250 (8 cores, 16 threads), 31 GB, Windows 11
Pro; at the start about 15 other compiler processes (fid, zig, clang) of
other sessions were running and 7.6 GB of memory was free. Read these as
noisy: the same Rust `hashmap` took 1.34 s on Nov 3 and 1.82 s here.

How: the build flags and the best-of-3 timing of `bench/run.sh`, by a copy of
it (not `run.sh` itself, which builds Fides from the repository root: that
day's working-tree `stdlib/core.fd` needed a newer compiler than the one at
hand). Fides was built with a `fid` compiled from the Nov 15 working tree,
with the standard library of that tree except `core.fd`, taken from commit
a9a8522; `CC="zig cc" fid ccompile`. Toolchains: rustc 1.97.0, zig 0.16.0
(Zig, and C++ through `zig c++ -O2`), Go 1.27.3. Every language printed the
same number for every program. **Fides native was not measured**: `fid emit`
of that compiler and library failed to build even hello world (type errors in
the merged native runtime), so the column is left out rather than filled from
another day.

| Benchmark | Fides via C | Rust | Zig | C++ | Go |
|---|---|---|---|---|---|
| fib | 1.07 | 0.66 | 0.68 | 0.66 | 0.96 |
| hashmap | 0.87 | 1.82 | - | 1.48 | 2.01 |
| loop | 4.95 | 3.94 | 3.24 | 5.05 | 4.05 |
| sieve | 0.37 | 0.28 | 0.28 | 0.29 | 0.39 |

What it says, as far as a noisy run can: through C, Fides is fastest on
`hashmap`, 1.6x the fastest on `fib`, 1.3x on `sieve` (2x on Nov 3, before
`Vec<u8>` held one byte per element) and 1.5x on `loop`, where C++ and Go
were about as slow on this run.

### Other measurements in the repository

These have no rival measured beside them:

- **Self-compile** (`fid dump-c compiler/main.fd`, 158,516 lines of Fides to
  67 MB of C): 74 s on Nov 9 with the compiler built at -O2 in 8 split
  units, 229 s at -O0; ~133 s on Nov 6. The Rust bootstrap compiler `fidesc`
  does it in ~21 s. About 2,100 lines a second: the widest gap to Go and Zig
  (roadmap CS1–CS7). Source: [selfhost.md, Numbers](reference/selfhost.md#numbers).
- **Compile-time baselines** per stage, for `tests/bootstrap/hello.fd` and a
  mid-size test, in `bench/compile-baseline.txt` (Nov 9; e.g. hello.fd, native
  emit: 6.1 s total, of which front end 2.7 s, lowering 1.6 s, native code
  1.4 s) and, for the Linux machine layer, `scripts/compile-gate.baseline`
  (hello: 677 ms total). These are gate baselines for the machine that
  recorded them, not comparisons.
- **HTTP server** (`stdlib/http.fd`): a 285 KB executable with no runtime
  dependency, 28,000 requests/s on one thread with the client on the same
  machine, 5.2 MB of memory after 160,000 requests with `http_serve_scoped`
  (release-roadmap.md I4, I6). The client and server shared a machine, so
  this is not a TechEmpower-style number.

## Known weak spots

Losses are part of the results:

- `fib` is 1.5–1.6x Rust through C and over 2x natively: Fides checks every
  addition for overflow; Rust's release mode does not, which also lets LLVM
  turn one of the two recursive calls into a loop.
- `sieve` was about 2x the rivals on Nov 2–3, 1.3x on Nov 15.
- Native output (`fid emit`) is slower than through C on every program.
- `HashMap` keys had to be strings when these programs were written
  (integer keys came on Nov 4–6); `hashmap.fd` still uses string keys, as the
  rivals do.

## Corrections welcome

Benchmarks are easy to get wrong. If you find a rival program that is slower
than its language allows, a flag a real user would set, a Fides program that
cheats, a number we cannot reproduce, or a machine where the picture is
different, please open an issue at
<https://github.com/pledgeandgrow/fid/issues> with:

- the program and the change (or a pull request against `bench/`);
- the output of `./bench/run.sh` before and after, with the versions it
  prints, your OS and CPU, and whether the machine was otherwise idle.

A corrected rival replaces ours, and the next results table here says so. A
result that makes Fides look worse is as welcome as one that makes it look
better.
