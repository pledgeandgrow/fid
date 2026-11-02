# Competitive Roadmap

> Goal: reach the maturity of Rust, Go, Zig and C++, and beat them on measured
> benchmarks where the language gives us a real advantage.
>
> Created: Nov 2, 2026. Update it in the same commit as the work.
> `[x]` done and verified, `[~]` partly done, `[ ]` not started. Sizes are effort,
> not dates: S = hours, M = days, L = weeks, XL = months.

This starts where [release-roadmap.md](release-roadmap.md) stops. That roadmap
makes every promise of 1.0.0 true. This one asks a harder question: why would
someone choose Fides over a language with fifteen years of production use?
Goals already in the release roadmap are named here, not repeated.

## What maturity is

Maturity is not a feature. It is what remains after years of real programs have
failed in every way they can. We cannot compress the years, but we can make each
one count more, and we can replace part of the evidence they give.

| What rivals have | What it gives them | What replaces it here | Track |
|---|---|---|---|
| Billions of lines compiled | Compiler bugs found | Fuzzing, differential testing, proofs | VF |
| Years of optimizer work | Fast code | A real optimizer, aimed at what we can prove | OP |
| Production incidents | Runtime hardened | Soak tests, chaos tests, our own services | VF, AD |
| Thousands of libraries | Nothing to write | A deep stdlib, painless C interop | SL, LG |
| Users on every platform | Portability | Tiers we test, not tiers we claim | PL |
| Stability record | Trust | A promise, then keeping it | ST |
| Community | Answers, tools, people | Docs, a book, being easy to contribute to | AD |

**What cannot be shortcut:** being trusted takes years of not breaking people.
Only real users find some bugs. Nothing here makes that untrue.

## The scoreboard

Measured so far (Windows x64, seconds, lower is better). These predate the
counted-loop and allocator work of Nov 1–2 and are re-measured by BM1.

| Benchmark | Fides native | Fides via C | Rust | Zig | Go | C++ |
|---|---|---|---|---|---|---|
| fib(40) | 1.26 | 2.95 | 0.46 | not measured | not measured | not measured |
| Arithmetic loop, 2 billion | 7.73 | 3.51 | 3.89 | not measured | not measured | not measured |
| Sieve, 20M | 0.91 | 0.42 | 0.31 | not measured | not measured | not measured |
| HashMap, 1M strings | 1.0–1.5 | 0.78 | 1.09 | not measured | not measured | not measured |

Compile speed: `fid` compiles itself (104,000 lines of Fides, 44 MB of C out) in
136 s, about 770 lines a second; `fidesc` (Rust) does it in 21 s. Go and Zig
compile on the order of 100,000 lines a second or more, so this is the widest gap
on the board: CS1–CS7.

Also measured: an HTTP server of 285 KB with no runtime dependency, 28,000
requests/s on one thread with the client on the same machine, 5.2 MB after 160,000
requests in scoped mode. No rival has been measured on these yet.

**Where we aim to win, tie, and hold our ground:**

| Area | Aim | Why it is credible |
|---|---|---|
| Compile speed | Beat Rust and C++ clearly, match Go | No LLVM, whole design is ours |
| Binary size | Beat Go and Rust | No runtime, unused code left out |
| Build setup | Beat all four | One file, no linker, no C toolchain |
| Startup and memory | Beat Go, match Rust | No GC, no runtime to start |
| Runtime speed | Beat Go, within 5% of Rust, Zig and C++ | Needs track OP in full |
| Safety-checked speed | Beat Rust in safe code | Effects and linearity prove more; see OP |
| Peak optimization on hand-tuned C++ | Do not expect to win | LLVM and GCC hold decades of work |

A benchmark only counts if it is published with its code, compiler versions and
flags, and if each rival is written the way a good programmer of that language
would write it.

## Track BM — Measure honestly

Nothing below means anything until this exists.

- [ ] BM1. A benchmark repo: the same programs in Fides, Rust, Go, Zig and C++, one
      command to run them all (M).
- [ ] BM2. Install Go and a real Clang and GCC beside `zig cc`; pin every version (S).
- [ ] BM3. The Benchmarks Game set: n-body, spectral-norm, mandelbrot,
      fannkuch-redux, binary-trees, fasta, reverse-complement, k-nucleotide,
      regex-redux, pidigits (L).
- [ ] BM4. Real work, not only loops: JSON parse and print, regex, sorting, hashing,
      string building, file reading, a small compiler (L).
- [ ] BM5. Service benchmarks in the TechEmpower style: plaintext, JSON, database,
      with the client on a separate machine (M).
- [ ] BM6. Compile-speed benchmarks: hello world, 10k, 100k and 1M lines, clean and
      one-line change (M).
- [ ] BM7. Size and memory benchmarks: binary size, peak memory, startup time (S).
- [ ] BM8. Runs on Windows and Linux, on a quiet machine, several runs, with spread
      reported (M).
- [ ] BM9. CI fails on a slowdown over 5%, per benchmark (release D7, tighter) (M).
- [ ] BM10. A public page of results over time, losses included (M).

**Acceptance:** anyone can reproduce every number with one command.

## Track OP — Optimizer

Today the native path keeps most values in stack slots, inlines nothing at IR
level and leaves floats in memory. This track is where most of the speed is.

**Foundations**

- [ ] OP1. Run the IR optimizer on the native path (release D6) (M).
- [ ] OP2. Real SSA: promote stack slots to values (mem2reg) before any other pass
      (L).
- [ ] OP3. Dominator tree, loop tree and use lists shared by every pass (M).
- [ ] OP4. One pass manager, with per-pass timing and a switch to turn each off (M).

**Classic passes**

- [ ] OP5. Inlining with a cost model, across modules (release D5) (L).
- [ ] OP6. Constant propagation, folding and branch elimination (M).
- [ ] OP7. Global value numbering and common subexpressions (M).
- [ ] OP8. Dead code and dead store elimination (M).
- [ ] OP9. Loop-invariant code motion (M).
- [ ] OP10. Strength reduction and induction variables (M).
- [ ] OP11. Loop unrolling, peeling and rotation (M).
- [ ] OP12. Scalar replacement of structs, and returning small structs in registers
      (L).
- [ ] OP13. Tail calls, tail recursion into loops (M).
- [ ] OP14. Devirtualization of trait calls and closures (M).
- [ ] OP15. Block layout for hot paths, using likely branches (M).

**Back end**

- [ ] OP16. A full register allocator over all registers, caller-saved included,
      live ranges split at calls (release D4 goes further) (L).
- [ ] OP17. Floats in xmm registers, not stack slots (L).
- [ ] OP18. Instruction selection: `lea`, addressing modes, `cmov`, `test`, fused
      compare and branch (L).
- [ ] OP19. A peephole pass on the native path (M).
- [ ] OP20. Modern targets: BMI, AVX2, a `--cpu` switch, safe defaults (M).

**Vectorization**

- [ ] OP21. SIMD types and intrinsics that compile to real vector code (L).
- [ ] OP22. Automatic loop vectorization: SSE2, AVX2 (XL).
- [ ] OP23. Vectorized `memcpy`, `memchr`, string compare and hashing in the runtime
      (M).

**Where Fides can beat them: what the language proves**

- [~] OP24. Bounds and overflow checks dropped on counted loops. Done for
      `while i < v.len()` and `for i in 0..v.len()`. Left: range analysis for every
      loop and branch (L).
- [ ] OP25. Effects drive optimization: `pure` calls merged, moved out of loops and
      removed; no-write calls keep memory facts alive (L).
- [ ] OP26. Linear types and second-class references as alias facts: no-alias
      everywhere safe code allows it, stronger than C's default (L).
- [ ] OP27. `@no-alloc` and known allocation sites: escape analysis moves heap
      objects to the stack (L).
- [ ] OP28. `comptime` evaluation of whole tables and parsers, so work disappears
      from the binary (M).
- [ ] OP29. Regions freed in one step, not object by object (needs release C6) (M).
- [ ] OP30. Profile-guided optimization from a real run (L).
- [ ] OP31. Whole-program mode by default: every build sees all the code (M).

**Acceptance:** native within 5% of Rust on BM3 and BM4; ahead of Go; the
overhead of safety checks under 2% where Rust's is measured higher.

## Track RT — Runtime and data layout

Some of the largest costs are in how data is laid out, not in the optimizer.

- [ ] RT1. `Vec<T>` stores `T` inline: a `Vec<u8>` takes one byte per element, and a
      `Vec<Struct>` no pointer per element (L).
- [ ] RT2. `str` carries its length: `str_len` is O(1), zero bytes are allowed, and
      HTTP bodies can be binary (release I8) (XL, touches every backend).
- [ ] RT3. Small-string storage with no allocation (M).
- [ ] RT4. A fast `HashMap`: open addressing with SIMD probing, a strong default hash
      (L).
- [ ] RT5. `HashMap<K, V>` with unboxed keys and values (L).
- [ ] RT6. Generics fully monomorphized, so a `Vec<f64>` holds floats as floats (L).
- [ ] RT7. Allocator: per-thread caches (release H8), freed memory returned to the
      system (L).
- [ ] RT8. Allocator speed on par with mimalloc on its own benchmarks (L).
- [ ] RT9. Own context switch, not fibers: under 20 ns per task switch (M).
- [ ] RT10. Tasks across all cores: M:N scheduling with work stealing (XL).
- [ ] RT11. IOCP on Windows, epoll on Linux, kqueue on macOS, replacing the O(n) poll
      loop (L).
- [ ] RT12. A timer heap, so 100,000 sleeping tasks cost nothing (M).
- [ ] RT13. Growable task stacks, or checked stack limits with a clear panic (L).
- [ ] RT14. Zero-copy file and socket I/O where the system allows it (M).
- [ ] RT15. Panics unwind cleanly, or abort by a documented rule (L).

**Acceptance:** binary-trees and k-nucleotide ahead of Go; 1M idle connections on
one machine; 100,000 requests/s on one core.

## Track CS — Compile speed

This is the race we can win outright, since no rival's speed comes for free.

- [ ] CS1. `emit` of hello world under 0.5 s (release D9), then under 0.1 s (L).
- [ ] CS2. Cache the checked stdlib in a binary form, not re-parsed each build (L).
- [ ] CS3. Borrow checking linear in program size (release D8) (M).
- [~] CS4. Self-compile under 60 s (release), then under 10 s (L). 136 s on Nov 2,
      from 601 s (release D10). Next: string compares in method lookup and the
      parser, then building the compiler with optimization.
- [ ] CS5. Parallel front end and code generation on all cores (L).
- [ ] CS6. Incremental native builds per function, under 100 ms for a one-line
      change (L).
- [ ] CS7. A fast debug mode with no optimizer, and a release mode (S).
- [ ] CS8. No linear scans in the self-hosted compiler: tables, not `Vec` searches
      (M).
- [ ] CS9. `fid check`: type errors with no code generation, under 50 ms on an edit
      (M).

**Acceptance:** 1M lines per second on the front end, ahead of Go on clean builds
and far ahead of Rust and C++.

## Track VF — Verification, in place of years

This track is what replaces production years. It runs for as long as the
language lives.

**Finding compiler bugs**

- [ ] VF1. A random program generator in the spirit of Csmith: all three backends
      must agree (release M9 is the start) (L).
- [ ] VF2. Random programs compared with an equivalent C program, built by Clang and
      GCC (L).
- [ ] VF3. Coverage-guided fuzzing of the lexer, parser, typechecker and every
      optimizer pass; no input may crash the compiler (L).
- [ ] VF4. Run every optimizer pass on and off over the whole suite: output must not
      change (M).
- [ ] VF5. Each optimization has a test that fails if the pass is wrong (M).
- [ ] VF6. Translation validation: a checker proves each optimized function matches
      the original, on a chosen set of passes (XL).
- [ ] VF7. Mutation testing of the compiler: a broken pass must fail a test (L).
- [ ] VF8. Line and branch coverage of `compiler/*.fd` over 90%, reported in CI (L).

**Proving behaviour**

- [ ] VF9. A written formal semantics of the core language (XL).
- [ ] VF10. A conformance suite of a thousand-plus tests, one per sentence of the
      reference, as Test262 is for JavaScript (L).
- [ ] VF11. A proof of the memory-safety claim on a small core calculus (XL).
- [ ] VF12. Reproducible builds: the same input gives the same binary on any machine
      (M).
- [ ] VF13. A second bootstrap path checked regularly: fixpoint from a clean tree (S).

**Hardening the runtime**

- [ ] VF14. AddressSanitizer, UBSan and ThreadSanitizer through the C path, in CI
      (release M8 is the start) (M).
- [ ] VF15. Fuzz every stdlib parser: JSON, regex, HTTP, TLS records, TOML, XML, CSV
      (L).
- [ ] VF16. Week-long soak tests of a service under load, with flat memory (release
      I6 is 24 hours) (M).
- [ ] VF17. Chaos tests: dropped connections, full disks, out of memory, clock jumps
      (L).
- [ ] VF18. Sharded tests on thousands of task and thread interleavings (L).

**Learning from every bug**

- [ ] VF19. A public miscompile log: cause, how it escaped, the test that now
      catches it (S).
- [ ] VF20. Every bug fixed gets a regression test, always (S).

**Acceptance:** a month of fuzzing on all cores finds no miscompile.

## Track LG — Language depth

Reach what Rust, Go, Zig and C++ programmers expect on day one.

- [ ] LG1. Moves or counted handles decided and enforced (release M4, M5) (L).
- [ ] LG2. Owned and borrowed strings (release M11) (L).
- [ ] LG3. Lifetimes the borrow checker can prove across functions, without
      second-class limits where that matters (XL).
- [ ] LG4. Traits: bounds checked, associated types, default methods, trait objects
      (release K9 is the start) (L).
- [ ] LG5. Zero-cost iterators: `map`, `filter`, `fold` compile to a plain loop (L).
- [ ] LG6. Closures that capture by reference and by value, with no heap allocation
      when they do not escape (L).
- [ ] LG7. Const generics (M).
- [ ] LG8. Sum types and pattern matching with guards, nesting and exhaustiveness in
      every position (M).
- [ ] LG9. Hygienic macros, or a comptime that covers what they do (L).
- [ ] LG10. Error handling with context and no allocation on the happy path (M).
- [ ] LG11. Structured concurrency with cancellation and timeouts (L).
- [ ] LG12. A defined memory model for atomics (L).
- [ ] LG13. Inline assembly on every native target (M).
- [ ] LG14. `async` for every argument type, and async traits (release H9) (L).
- [ ] LG15. C interop without bindings: `@import("header.h")`, as Zig does (L).
- [ ] LG16. C++ interop for the common cases (XL).
- [ ] LG17. Every annotation works or is removed (release K8) (M).

**Acceptance:** each feature in the comparison table has a conformance test.

## Track SL — Standard library depth

Go's lesson: a deep stdlib replaces a thousand packages. It suits our rule of
owning what we ship.

- [ ] SL1. HTTP/2 client and server (L).
- [ ] SL2. HTTP/3 and QUIC (XL).
- [ ] SL3. WebSockets, committed and tested (M).
- [ ] SL4. TLS 1.3 on every platform (release I7) (M).
- [ ] SL5. DNS with caching, IPv6 everywhere (M).
- [ ] SL6. JSON at simdjson-class speed (L).
- [ ] SL7. A linear-time regex engine (L).
- [ ] SL8. Time zones, calendars, formatting and parsing (L).
- [ ] SL9. Unicode: normalization, segmentation, case folding (L).
- [ ] SL10. Compression: gzip, zstd, brotli decode (L).
- [ ] SL11. Encoding: base64, hex, CSV, XML, YAML, TOML, protobuf (L).
- [ ] SL12. Database drivers: PostgreSQL, MySQL, SQLite, with parameter binding (L).
- [ ] SL13. Built-in testing: table tests, benchmarks, fuzzing, as in Go (M).
- [ ] SL14. Structured logging and metrics (M).
- [ ] SL15. A sampling profiler reachable from a running service (L).
- [ ] SL16. Big integers and decimals (M).
- [ ] SL17. Files, paths and processes complete on every platform (M).
- [ ] SL18. Crypto through reviewed libraries, the set of release J5 (L).
- [ ] SL19. Every public function documented with an example that runs in CI (L).

**Acceptance:** a real web service needs no package outside the stdlib.

## Track TL — Tooling

Go and Rust won developers with tools as much as with languages.

- [ ] TL1. One binary, `fid`: build, run, test, fmt, doc, lint, bench, fuzz (L).
- [ ] TL2. Error messages as good as Rust's, each with a suggested fix, and a test
      per error (L).
- [ ] TL3. `fid fix`: applies suggested fixes, and upgrades code between versions (L).
- [ ] TL4. LSP complete: references, rename, formatting, signature help, inlay
      hints, code actions (release N7 is the start) (L).
- [ ] TL5. Debugger support with line tables on every platform (release N9) (L).
- [ ] TL6. Cross-compilation out of the box: `fid build --target linux-arm64` from
      any host, with no extra download (L).
- [ ] TL7. Profiler output readable by standard tools (M).
- [ ] TL8. A web playground (M).
- [ ] TL9. Clear dependency handling: Git and path, a lock file, audits of what a
      build pulls in (M).
- [ ] TL10. Editor support beyond VS Code: Neovim, JetBrains, Zed (M).
- [ ] TL11. `fid doc` sites as good as docs.rs and pkg.go.dev (M).

**Acceptance:** a new user is productive within an hour, on any supported system.

## Track PL — Platforms

Every platform gets a tier, and a tier means CI, not a hope.

- [ ] PL1. Publish tiers: Tier 1 is tested on each commit, Tier 2 builds, Tier 3 is
      best effort (S).
- [ ] PL2. Tier 1: Windows x64, Linux x64, macOS ARM64, Linux ARM64 (release F4–F8)
      (XL).
- [ ] PL3. Tier 2: Windows ARM64, macOS x64, FreeBSD, RISC-V (L).
- [ ] PL4. WebAssembly with WASI (release F9) (L).
- [ ] PL5. Freestanding and embedded: Cortex-M, RISC-V boards, with no OS (release G)
      (L).
- [ ] PL6. Android and iOS (XL).
- [ ] PL7. Native backends for ELF and Mach-O, sharing one encoder (L).

**Acceptance:** Tier 1 passes the whole suite on each commit.

## Track ST — Stability and process

Trust is built by what we do not break.

- [ ] ST1. Freeze the Rust compiler as a bootstrap only: new features land in
      `compiler/*.fd` alone. Writing everything twice is the largest drag on pace (M).
- [ ] ST2. A written stability promise: code that compiles on 1.0 compiles on every
      1.x (release K12) (S).
- [ ] ST3. Editions, so the language can change without breaking old code (L).
- [ ] ST4. A release every six weeks, on a date, whatever is ready (M).
- [ ] ST5. Long-term support releases with security fixes (M).
- [ ] ST6. Before each release, build every known public Fides project, as Rust's
      crater does (L).
- [ ] ST7. A public process for language changes (S).
- [ ] ST8. A security policy, a contact, and published advisories (S).
- [ ] ST9. Release notes that name every breaking change (S).
- [ ] ST10. Signed checksums for every download (S).

**Acceptance:** two years of releases with no unannounced break.

## Track AD — Real use

Only this produces true production years. It starts now, at small scale.

**Using it ourselves**

- [ ] AD1. `ark`, `scribe` and `veil` written in Fides (release N8) (L).
- [ ] AD2. The project website served by a Fides program (M).
- [ ] AD3. `fides-os` as a standing test of bare-metal work (release G4) (M).
- [ ] AD4. Every internal tool that can be in Fides is (M).
- [ ] AD5. A real production service of ours, on Fides, with its incidents written up
      (L).

**Being reachable**

- [ ] AD6. The book: a full guide, from first program to writing a service (L).
- [ ] AD7. A cookbook of common tasks (M).
- [ ] AD8. Guides for people arriving from Rust, Go, Zig and C (M).
- [ ] AD9. A channel for questions, answered (S).
- [ ] AD10. Good first issues, kept current (S).
- [ ] AD11. A compiler internals guide, so others can contribute (M).

**Earning users**

- [ ] AD12. A public launch once the beta gate is met, with honest limits (M).
- [ ] AD13. Publish the benchmarks (BM10), and invite corrections (S).
- [ ] AD14. Ten external projects built on Fides (XL, not only effort).
- [ ] AD15. Ten contributors outside the core (XL, not only effort).
- [ ] AD16. A first company running Fides in production (XL, not only effort).

**Acceptance:** real incidents from real users, handled in public.

## Order of work

1. **Release beta gate first.** No one can use what they cannot rely on.
2. **ST1, BM1–BM8.** Stop writing everything twice, and start measuring.
3. **VF1, VF3, VF14, VF20.** Before the optimizer grows, so new passes are checked
   from their first day.
4. **RT1, RT2, RT6, OP2, OP16, OP17.** Data layout and SSA: the largest gains, and
   the hardest to change later.
5. **OP5–OP19, CS1–CS4.** The speed most programs will feel.
6. **OP24–OP29, the proofs only we have.** Where we can beat them, not only match.
7. **AD1–AD5, ST2–ST10.** Run on it ourselves, and start the release rhythm.
8. **LG, SL, TL, PL in parallel,** led by what real use asks for.
9. **AD12 onward.** Launch, listen, repeat. This never ends.

## Set aside

These cost money and do not block anything above.

- [ ] Paid security audits of the compiler and the runtime (release J9).
- [ ] Toolchain certification (release L5).
- [ ] A bug bounty.
- [ ] Dedicated benchmark and fuzzing machines; a laptop and free CI come first.
- [ ] A foundation, full-time staff, conferences.
