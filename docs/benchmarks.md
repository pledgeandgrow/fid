# Benchmarks

What we measure, how to reproduce it, and every result the repository has,
losses included (roadmap BM10, AD13). If a number here is wrong, or a rival
program is not the way a good programmer of that language would write it,
[open an issue](#corrections-welcome).

## What is measured

Four programs, each written in Fid, Rust, Zig, C++ and Go, in
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
opt-level=3`, `zig build-exe -O ReleaseFast`, `clang++ -O2` (a real Clang
when one is on the PATH, `zig c++` otherwise), `g++ -O2` when GCC is on the
PATH, `go build`. Each rival is written in its own idiom, not translated word for word
(Rust's `vec![true; n]`, C++'s `std::vector<bool>`, Go's `[]bool`, Fides's
`Vec<u8>`). Zig has no `hashmap` program yet.

Times are wall clock, best of N runs, of the whole process (start-up
included), in seconds.

Real work (BM4: sorting, string building, hashing, a small compiler, JSON,
regex, file reading; `SUITE=work ./bench/run.sh`) is in the Nov 17 entry of
[Results](#results), and so is the Benchmarks Game set (BM3, `SUITE=game
./bench/run.sh`, ten programs). Compile speed at 10k–1M lines against
the rivals (BM6) and service benchmarks with the client on the same machine
(BM5) are in the Nov 16–17 entries; spreads are reported since Nov 17. Not
here yet: a service client on another machine, and Linux runs (BM8:
verify-weekly.yml runs `bench/run.sh` on Linux and pages.yml merges its
runs into the published page; none is in this file). Binary size, startup and peak
memory (BM7, `bench/size.sh`) are in the Nov 17 entry of
[Results](#results).

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
Zig and the C++ fallback), and `rustc`, `clang++`, `g++` and `go` for those
columns. `bench/versions.txt` pins the versions.

`./bench/check.sh` measures the Fides programs only and fails when one is more
than `LIMIT_PCT` (default 10) percent slower than `bench/baseline.txt`
(`--save` records it; the file is not in the repository: a baseline is only
meaningful on the machine that recorded it). `bench/compile-check.sh` and
`scripts/compile-gate.sh` do the same for the compiler's per-stage times
(`fid emit --time`). The CI workflow `.github/workflows/bench.yml` builds the
base commit's and the head's compiler on one runner and runs both checks;
its results are not kept in the repository.

## Results

### Nov 17, 2026 — the Benchmarks Game set (BM3, busy Windows machine)

`SUITE=game RUNS=3 ./bench/run.sh` (`bench/game/`): n-body, spectral-norm,
mandelbrot, fannkuch-redux, binary-trees, fasta, reverse-complement,
k-nucleotide, regex-redux and pidigits, each in Fides, Rust, Zig, C++ and Go.
Every program prints one integer (a 32-bit hash of the output for the last
four) and every language printed the same one: no `WRONG`. The values of
reverse-complement (1054774738), k-nucleotide (3642494098), regex-redux
(2268963707) and pidigits (1413423979) were also computed independently by
`python3 bench/game/reference.py` (Python's `re` and integers). The input of
the last four is fasta's output, made in memory instead of read from stdin.
Rust and Zig have no regular expressions in their standard libraries and
`run.sh` fetches no crates, so regex-redux is one small backtracking matcher,
written the same way in all five languages (Go's `regexp` and C++'s `<regex>`
are not used). pidigits uses `stdlib/bigint.fd` in Fides, `math/big` in Go,
and a small big integer of their own (32-bit limbs, what the spigot needs) in
Rust, C++ and Zig.

Windows 11 x64, 16 cores, other sessions' compiler builds running (load
average 8 to 44 during the runs; the disk filled up during the first run, so
the binary-trees, n-body, reverse-complement and spectral-norm rows were run
again a few minutes later, and those are the rows here). `FID` was the
stage-0 `fid` of Nov 16 (a one-stage build, not a release bootstrap), run on
the stdlib of HEAD. rustc 1.97.0, Zig 0.16.0 (also C++ through `zig c++`),
Go 1.27.3, g++ 15.2.0 (the g++ of WSL, built and timed inside WSL: Linux on
the same machine; the other columns run on Windows).

| Benchmark | Fides native | Fides via C | Rust | Zig | C++ (Clang) | C++ (GCC) | Go |
|---|---|---|---|---|---|---|---|
| binarytrees | 2.38 ±28% | 1.71 ±16% | 2.99 ±43% | 1.01 ±16% | 2.09 ±32% | 1.17 ±136% | 3.99 ±15% |
| fannkuch | 11.40 ±8% | 6.36 ±13% | 4.85 ±12% | 6.76 ±10% | 5.20 ±10% | 4.06 ±14% | 4.82 ±27% |
| fasta | 0.97 ±22% | 0.95 ±5% | 0.75 ±48% | 0.71 ±44% | 0.97 ±52% | 0.48 ±10% | 0.90 ±87% |
| knucleotide | 10.97 ±12% | 4.57 ±11% | 1.84 ±53% | 1.52 ±12% | 2.52 ±26% | 0.74 ±92% | 1.61 ±22% |
| mandelbrot | 3.37 ±43% | 2.16 ±8% | 1.85 ±12% | 1.82 ±24% | 1.75 ±20% | 1.92 ±10% | 2.31 ±16% |
| nbody | 4.17 ±52% | 2.53 ±23% | 0.55 ±26% | 0.49 ±37% | 0.66 ±8% | 0.56 ±5% | 0.81 ±29% |
| pidigits | 64.57 ±4% | 67.78 ±9% | 5.98 ±8% | 5.93 ±6% | 15.90 ±2% | 5.42 ±10% | 3.67 ±0% |
| regexredux | 3.52 ±15% | 2.20 ±3% | 1.46 ±64% | 2.25 ±463% | 1.51 ±5% | 1.12 ±19% | 1.60 ±17% |
| revcomp | 1.09 ±44% | 0.86 ±16% | 0.75 ±109% | 0.79 ±9% | 0.76 ±49% | 0.56 ±58% | 0.85 ±36% |
| spectral | 2.30 ±18% | 0.79 ±4% | 0.61 ±45% | 0.73 ±29% | 0.68 ±37% | 0.35 ±27% | 0.78 ±101% |

Seconds, best of 3, wall clock, ±spread of the runs. Most spreads are over
10%: the machine was not quiet, and these numbers say more about which
programs are slow than about how slow.

What the table shows, losses first:

- **pidigits is 11-18x slower than Rust, Zig, GCC and Go** (65 s against
  3.7-6 s) and 4x slower than Clang's C++ (15.9 s). `stdlib/bigint.fd` keeps base-10^9 limbs in a `Vec` and makes a
  new `BigInt` for every operation; the spigot's two quotients each go
  through `bigint_div`'s general long division, where the rivals divide by
  repeated subtraction (the quotient is a digit) or, in Go, use `math/big`.
- **k-nucleotide is 4-7x slower natively** (11.0 s; 4.6 s through C):
  `HashMap<i64, i64>` inserts and lookups, 35 million of them.
- **n-body and spectral-norm natively are 3-9x slower than the rivals**, and
  fannkuch-redux 1.7-2.8x: floating-point and `Vec` index loops on the
  native backend. Through C, n-body is 3-5x slower, spectral-norm 1-2.3x and
  fannkuch-redux 0.9-1.6x.
- reverse-complement, fasta and mandelbrot natively are 1-2x the rivals'
  times, regex-redux 1.6-3.1x (1-2x through C); binary-trees through C
  (1.71 s) is faster than Rust and Go here and slower than Zig.

### Nov 17, 2026 — real work (BM4, busy Windows machine)

`SUITE=work ./bench/run.sh` (`bench/work/`), Windows 11 Pro, AMD Ryzen 7
250, other sessions' compilers running throughout (load average 13.8
before, 16.0 after: the spreads are wide, read the best times, not the
spreads). `fid` is the Nov 16 one-stage build the day's agents used
(`stage0.exe`), with the working tree's `stdlib/`; rustc 1.97.0, zig
0.16.0, Go 1.27.3, g++ 15.2.0 in WSL. Every language printed the same
number for every program (no `WRONG`). The programs: `compiler` (generate
50000 lines of `let vK = <expr>;`, lex, parse into boxed trees, evaluate),
`fileread` (read a 50 MB text file three times and sum its numbers),
`hash` (FNV-1a over 50 MB, 4 rounds), `json` (parse 15.9 MB of generated
JSON into an arena of nodes and print it back compact, 3 rounds), `regex`
(a Thompson NFA, 8 patterns over 200000 lines), `sort` (5M integers, each
language's own sort), `strings` (a 2M-number string with the string
builder, then hashed). JSON and regex are written by hand, the same
algorithm in every language: Rust's standard library has neither and
`run.sh` fetches no crates.

| Benchmark | Fides native | Fides via C | Rust | Zig | C++ (Clang) | C++ (GCC) | Go |
|---|---|---|---|---|---|---|---|
| compiler | 0.97 ±37% | 0.63 ±19% | 0.67 ±36% | 0.60 ±11% | 0.82 ±12% | 0.65 ±44% | 0.82 ±42% |
| fileread | 5.07 ±84% | 1.43 ±8% | 0.52 ±158% | 0.44 ±18% | 0.52 ±46% | 1.62 ±5% | 0.47 ±58% |
| hash | 1.25 ±16% | 0.68 ±36% | 0.57 ±158% | 0.56 ±38% | 0.60 ±21% | 0.54 ±5% | 0.66 ±27% |
| json | 2.42 ±36% | 1.62 ±5% | 1.59 ±8% | 1.82 ±5% | 1.66 ±28% | 1.90 ±7% | 1.43 ±17% |
| regex | 4.42 ±9% | 2.61 ±9% | 1.72 ±19% | 1.83 ±10% | 2.23 ±14% | 1.72 ±11% | 1.76 ±30% |
| sort | 7.07 ±11% | 2.56 ±8% | 0.46 ±548% | 1.35 ±6% | 0.48 ±29% | 0.79 ±10% | 1.08 ±22% |
| strings | 1.38 ±18% | 0.96 ±11% | 0.48 ±77% | - | 0.47 ±35% | 0.22 ±3% | 0.46 ±49% |

Seconds, best of 3, wall clock. What it says: through C, Fides is level
with the rivals on the small compiler and JSON (within 15% of the
fastest), 1.2x behind on hashing, 1.5x on regex, 2–4x on string building,
3x on file reading and 5x on sorting. Fides native (`pe_ir`) is slower
than Fides through C on every program, most on file reading (3.5x) and
sorting (2.8x). C++ (GCC) runs inside WSL and reads the
50 MB file across the Windows file system, which is why its `fileread` is
the slowest rival. Zig has no `strings` program.

### Nov 17, 2026 — binary size, startup and peak memory (BM7, busy Windows machine)

`bench/size.sh` (all programs: `bench/size/hello` and every `bench/fides/`
program, every language), Windows 11 x64, other sessions' builds running.
`FID` was the stage-0 `fid` of Nov 16 (a one-stage build, not a release
bootstrap). Rivals built as `bench/run.sh` builds them: `rustc -C
opt-level=3` (rustc 1.97.0), `zig build-exe -O ReleaseFast` (Zig 0.16.0),
`zig c++ -O2`, `go build` (Go 1.27.3); no GCC on this machine. Zig has no
`hashmap` program. Every program printed the same as the Fides one (no
`WRONG`). The first recorded run (Nov 16, a Nov 15 `fid`) is
[docs/perf/size-2026-11-16.md](perf/size-2026-11-16.md).

Binary size (KB, as built, not stripped)

| Program | Fides native | Fides via C | Rust | Zig | C++ (Clang) | C++ (GCC) | Go |
|---|---|---|---|---|---|---|---|
| hello | 40 | 317 | 123 | 790 | 187 | - | 2436 |
| fib | 41 | 318 | 123 | 791 | 187 | - | 2436 |
| hashmap | 79 | 318 | 129 | - | 432 | - | 2437 |
| loop | 40 | 317 | 124 | 791 | 187 | - | 2436 |
| sieve | 67 | 317 | 124 | 798 | 356 | - | 2436 |

Startup: hello, best of 20 (ms; wall time of the child process seen from
`bench/peakmem.py`, process creation included; differences under ~3 ms are
noise on this machine)

| Program | Fides native | Fides via C | Rust | Zig | C++ (Clang) | C++ (GCC) | Go |
|---|---|---|---|---|---|---|---|
| hello | 12.5 | 16.8 | 16.8 | 13.0 | 14.9 | - | 20.3 |

Peak memory (MB, peak working set)

| Program | Fides native | Fides via C | Rust | Zig | C++ (Clang) | C++ (GCC) | Go |
|---|---|---|---|---|---|---|---|
| hello | 3.5 | 4.3 | 4.1 | 3.2 | 4.0 | - | 5.8 |
| fib | 3.5 | 4.3 | 4.1 | 3.2 | 4.0 | - | 5.9 |
| hashmap | 114.3 | 114.6 | 131.0 | - | 77.5 | - | 93.0 |
| loop | 3.5 | 4.3 | 4.2 | 3.2 | 4.0 | - | 5.9 |
| sieve | 22.5 | 22.9 | 22.7 | 22.2 | 5.9 | - | 26.2 |

Reading it: Fides native (`fid emit`, pe_ir) makes the smallest executables
of the table (40 KB for hello, 33 KB on Nov 16: the runtime grew), and
starts as fast as Zig. Through C the runtime is linked whole (317 KB, down
from 410 KB on Nov 16). Peak memory is at the rivals' level except
`hashmap` (114 MB against 78 MB for C++ and 93 MB for Go; Rust 131 MB) and
`sieve`, where C++ stores bits (`std::vector<bool>`) and the others a byte
per entry. One run on one machine (BM8: several runs, the spread, Linux).

### Nov 17, 2026 — JSON scanning (SL6, busy Windows machine)

`bench/json/json_bench.fd` builds a 5,895,337-byte document in memory
(12,700 records nested 4 deep: strings with `\"`, `\\` and `\u` escapes,
integers, floats with exponents, booleans, nulls, arrays and objects;
711,213 values) and times `json_is_valid`, `json_index` (the indexed DOM's
one scan), `json_parse` (validate + canonicalize) and lookups. MB/s is
10^6 bytes a second, best of 3 (parse: one run). Same machine as the Nov 16
entry, load average about 14; `fid` was an agent's one-stage build of the
Nov 16 tree (`stage0.exe`), `stdlib/core.fd` as committed. "Before" is
`stdlib/json.fd` with the indexed DOM and the old scanners (`str_byte_at`
and `str_len` at every byte); "after" reads bytes in place with the
length taken once, picks a value's scanner with one load from a 256-byte
class table (JCLASS) and runs a string's plain bytes through a second
table (JSTRC) that stops only at a quote, a backslash or a control byte.

| Scan | Native before | Native after | Through C before | Through C after |
|---|---|---|---|---|
| `json_is_valid` | 10.6–14.7 MB/s | 82.8–88.9 MB/s | 91.0–91.4 MB/s | 410.8–465.6 MB/s |
| `json_index` | 12.2–13.2 MB/s | 42.3–49.0 MB/s | 60.6–66.3 MB/s | 142.9–178.7 MB/s |
| `json_parse` | 1.2–1.4 MB/s | 3.3–3.8 MB/s | 8.1–9.3 MB/s | 14.7–21.0 MB/s |
| one `json_path` (validates the text) | 3.3–4.2 s | 0.40–0.60 s | 0.43–0.63 s | 77–92 ms |
| one `jd_get`/`jd_str` on the index | 1.8–2.0 µs | 1.0–2.0 µs | 0.5–0.9 µs | 0.5–0.7 µs |

Ranges are two runs each, alternating before and after. What it says: the
validating scan is 6–8x faster natively and 5x through C; the index scan
3.5x and 2.5x, where it now spends most of its time adding nodes (five
`Vec` pushes a value); a lookup on the index costs a microsecond against
seconds for the string API on this document. simdjson reads 2–3 GB/s on
one core: through C, Fides's validating scan is about 5x short of that,
natively about 30x. `stdlib/simd.fd` has no byte lanes (its native
vector loops are f64 and i64 only), so the vector step is not there.

Later the same day: strings a word at a time. After 16 plain bytes of a
string, while 16 more bytes of text remain, the scan goes 8 bytes a step
(`j_plain_words` in `stdlib/json.fd`: one aligned word load and three
SWAR tests, for a quote, a backslash and a byte below 0x20), and a leaf
of the index pushes its five words once (`jd_new_leaf`, no later
`d.end.set`). The interpreter cannot read a string a word at a time (a
word load at a string's address gives the string back); `j_words_ok`
finds that out at run time and it keeps the byte loop
(`tests/bootstrap/test_json_swar.fd` checks the word loop against the
grammar at every position and alignment). Same machine, load average
8–15, `stage0.exe`, `stdlib/core.fd` as committed; "before" is the same
`json.fd` with the word loop switched off. Three runs alternating, best
of 3 (5) within a run:

| Scan | Native before | Native after | Through C before | Through C after |
|---|---|---|---|---|
| `json_is_valid`, the 5.9 MB document | 72.6–126.9 MB/s | 67.2–120.0 MB/s | 303.3–442.7 MB/s | 368.9–577.4 MB/s |
| `json_index`, the 5.9 MB document | 38.8–61.5 MB/s | 38.1–61.9 MB/s | 107.6–160.4 MB/s | 113.5–170.4 MB/s |
| `json_is_valid`, 5,000 strings of 1,120 plain bytes (5.6 MB) | 294–330 MB/s | 935–1,549 MB/s | 768–804 MB/s | 3,254–5,087 MB/s |

On long strings the word loop is 3–5x faster natively and 4–6x through C,
where it passes simdjson's 2–3 GB/s for that part of a document. On the
5.9 MB document the strings are short (keys, tags, one 130-byte note a
record) and the native numbers are within the machine's noise; through C
`json_is_valid` gains about 25%. What is left before simdjson: the
structural bytes, numbers and white space still go a byte at a time, and
`json_index` is bound by its `Vec` pushes.

The glTF and Tiled loaders (`stdlib/gltf.fd`, `stdlib/tiled.fd`) now index
their JSON once and walk the index; they rescanned the document (or a
layer's data array) at every lookup before. Loading a 128x128 two-layer
map into a `Tilemap` with the old loaders did not finish in 180 s
natively; with the index the whole test program took 0.45 s
(`tests/bootstrap/test_stdlib_jsondoc_loaders.fd` runs the same code at
32x32 so that the interpreter finishes it).

### Nov 17, 2026 — idle connections (RT11, WSL2 kernel 6.6, C path)

One task per connection, parked on epoll; `bench/idle/run.sh`
(`CBUILD=dumpc`, gcc 15 -O2). 4.64 KB resident per connection, flat from
1k to 500k (1000: 6.8 MB, 100000: 455.6 MB, 500000: 2267.9 MB). An idle
ping takes 0.10 ms (p50) at 1k and at 500k (it was 14 ms at 500k before
parked tasks left the scheduler's list); about 20k connections opened per
second. The run stops at 589,808 connections with `vm.max_map_count`
1048576 (two mappings per task stack before Linux 6.13). Details:
docs/perf/rt11-idle-connections.md.

### Nov 16, 2026 — compile speed and service benchmarks (busy Windows machine)

Windows 11 Pro, AMD Ryzen 7 250 (8 cores, 16 threads), a **busy shared
machine**: other sessions' compilers were running throughout (Git Bash's
load average 8–13). `fid` is a one-stage build of the Nov 16 working tree
(an agent's `fidh1.exe`, not the release build at -O2); rustc 1.97.0, zig
0.16.0, Go 1.27.3, Node 24.10.

**Compile speed** (BM6), `bench/compile-speed.sh`, `SIZES="hello 10000
100000"`, best of 1, wall clock in seconds. Rivals in debug mode; `change`
is the build after a one-line change, every cache warm. The first attempt
failed at 10k lines in Fides and Zig: `gen_lines.py` named its functions
`f0, f1, ...`, and `f32`, `f64`, `f80`, `f128` are type names in both
languages (the functions are `w0, w1, ...` since).

| Program | Build | fid check | Fides native | Fides via C | Rust | Go | Zig | C++ (Clang) |
|---|---|---|---|---|---|---|---|---|
| hello (5 lines) | clean | 1.66 | 10.53 | 32.82 | 1.44 | 1.56 | 6.05 | 10.02 |
| 10000 (10002 lines) | clean | 9.91 | 30.52 | 39.72 | 2.05 | 1.64 | 5.05 | 1.87 |
| 10000 (10002 lines) | change | - | 27.54 | 40.43 | 1.45 | 1.66 | 4.71 | 1.72 |
| 100000 (100062 lines) | clean | 70.44 | 327.10 | 223.90 | 14.54 | 3.43 | 11.43 | 9.85 |
| 100000 (100062 lines) | change | - | 319.31 | 251.78 | 8.92 | 3.50 | 11.60 | 9.60 |
| 1000000 (1000662 lines), Nov 17 | clean | 3421.86 | over 20000 (stopped) | not run | 184.52 | 81.87 | 101.69 | 107.52 |

What it says: at 100k lines Fides takes 15–95x the rivals' debug builds, and
a one-line change costs as much as a clean build (the build cache keys the
whole program; Rust's incremental build saves 40%). `fid check` alone, the
editor's wait, is 70 s at 100k lines, about 1,400 lines a second.

The 1M row (Nov 17, `SIZES=1000000 bash bench/compile-speed.sh`, `fid` the
Nov 16 one-stage build `stage0.exe`) is worse, and on a busier machine: load
average 9–17 during `fid check`, then 13–125 (mostly 20–90) during the
native build, while other sessions built compilers; the rivals were timed
by hand afterwards with the script's commands at load 16–90, and all of
them print the same number (12323). `fid check` took 3422 s, 49x its
100k time for 10x the lines: lexing 87 s, parsing 126 s, the step between
"Resolving imports" and type checking (the program has no imports) about
1470 s, type checking 196 s, borrow checking 1477 s, monomorphization 59 s.
The step before type checking and the borrow checker grow much faster
than the program. The native build (`fid emit`) ran the same front end
(borrow checking 1769 s; 3827 s in all), lowering (497 s) and the IR
optimizer (1619 s), then was still in the `pe_ir` backend after 3 h 55
min there; it was stopped 5 h 34 min after it started, having used 13460
s of CPU (the machine ran almost nothing of it for about an hour of that
time: the load sampler, every 5 minutes, has no sample from 17:00 to
18:11), so the cell is wall time that is not a measurement of the
compiler alone, only a sign of how far it is from the rivals. The build through C and the one-line-change row
were not run (each would take hours more at this rate).

**Service benchmarks** (BM5), `bench/service/run.sh` as run by
`scripts/service-bench-check.sh`: 2 s per test after 1 s warm-up, 8
keep-alive connections, client `bench/service/load.py` (every reply's body
checked) **on the same machine**. Requests per second:

| Test | Fides via C | Go | Node |
|---|---|---|---|
| plaintext | 16041 | 13188 | 14686 |
| json | 14402 | 11800 | 13008 |
| db (in-memory table) | 13595 | 11604 | 13110 |
| queries (20 rows) | 9628 | 8640 | 9143 |

The Python client is the limit at these rates, so this says every server
answered every request right under load, not which is fastest; wrk on Linux
(verify-weekly) and a client on another machine (`run.sh serve` / `load`) are
the measurements to read. Fides native was not measured on Windows: the
`pe_ir` backend has no `p_signal_check` / `p_signal_register`, which
`stdlib/http.fd`'s accept loop calls, so `fid emit` of an HTTP server fails
there.

**The 5% gate on an unchanged compiler** (BM9),
`scripts/bench-noise-check.sh`, `ROUNDS=2 RUNS=5 LIMIT_PCT=5`, the core set:
both rounds passed, no false failure. The largest head/base difference over
the two rounds (the same executable as base and head): fib +1.1% native /
-0.6% through C, hashmap +3.5% / +4.1%, loop +3.1% / +1.4%, sieve +5.1% /
+5.0%. sieve (about 0.3 s) passed only through the gate's 5 ms slack: on a
machine this busy, 5% is at the edge of the noise for the short programs.

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

- **Self-compile** (`fid dump-c compiler/main.fd`): 57 s on Nov 21 for
  the 361K-line compiler, built at -O2 in 8 split units (about 6,300 lines
  a second); `fidesc` (Rust) takes ~45 s on the same tree (Nov 20). Earlier,
  on 158,516 lines (67 MB of C): 74 s on Nov 9 at -O2, 229 s at -O0; ~133 s
  on Nov 6; `fidesc` ~21 s. Still the widest gap to Go and Zig
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
