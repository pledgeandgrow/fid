# Release Roadmap

> Goal: a public beta people can build real programs on, then a stable 1.0
> whose promises are all true and all tested.
>
> Created: Nov 1, 2026. Update it in the same commit as the work.
> `[x]` done and verified, `[~]` partly done (the note says what is left),
> `[ ]` not started. Sizes are effort, not dates: S = hours, M = days,
> L = weeks, XL = months.

## Where it stands (Nov 1, 2026)

**Works today (tested):** native Windows `.exe` files with no C compiler; the
self-hosted compiler reproduces itself byte for byte; one integer meaning on
the interpreter, C and native paths; sound incremental builds; a real
allocator; local `Vec`s freed by the compiler; `@secret`; native threads.
Suite: interpreter 287/287, C 286/286, native 288/288, expected-fail 66/66.
The self-hosted compiler reproduces itself from its second generation on.

**Measured starting point** (Windows x64, seconds):

| Benchmark | fid native | fid via C -O2 | Rust -O | Target for 1.0 |
|---|---|---|---|---|
| fib(40) | 1.77 | 2.95 | 0.46 | ≤ 1.0 |
| Arithmetic loop, 2 billion | 14.0 | 3.51 | 3.89 | ≤ 5.0 |
| Sieve, 20M | 2.63 | 0.42 | 0.31 | ≤ 0.6 |
| HashMap, 1M strings | 1.65 | 0.78 | 1.09 | ≤ 1.0 |
| Peak memory, 30M temporary strings | 2.3 GB | 3.4 GB | 5 MB on 3M | ≤ 50 MB |

Self-compile: about 3 minutes natively, 22 s with `fidesc`.

**Which gap is handled where:**

| Gap | Track |
|---|---|
| C libraries need `ccompile`; `use sqlite` fails | B, E |
| Strings are not reclaimed in long-running programs | C |
| Native builds are several times slower on tight loops | D |
| Bare-metal tests only run hosted, where they fault | G |
| Linux has not been run | F |
| No TLS; `http`, `net` and `sqlite` do not compile | B, I |
| Only SHA-256 and HMAC, unreviewed | J |
| `async` runs eagerly and does not suspend | H |
| macOS, ARM64, RISC-V and WASM are partial | F |
| Safety annotations are not certified or fully enforced | K, L |

## Release gates

**Public beta** needs: all of tracks A and B; C1–C4; D1–D3; E1–E3; F1–F3;
H1–H2; K1–K3. A beta may have gaps, but every gap must be documented.

**Stable 1.0** needs: all of tracks A–E, F1–F6, H, I, J1–J6 and K, plus every
guide and feature table matching what the compiler does.

**After 1.0:** track L (safety-critical), native macOS, ARM64, RISC-V and WASM
(F7–F9), J7–J9 and track G beyond what `fides-os` needs. None of them block
1.0, as long as the docs say they are not supported yet. Crypto stays marked
"not for production" until J9 is done.

## Decisions needed

Each changes the work below. Recommendations first; none is taken yet.

1. **Native speed:** write a register allocator for the PE backend (recommended
   for 1.0: no dependencies, keeps "no C, no LLVM"), or lean on the C path or
   `llvm-roadmap.md` for release builds.
2. **TLS and crypto:** bind reviewed libraries (recommended: Schannel on
   Windows, a vendored mbedTLS or BearSSL elsewhere). Writing our own TLS is
   not an option for a first release.
3. **Memory model:** compiler-inserted frees with escape analysis (recommended:
   matches "no GC"), or real ownership with moves checked everywhere. A garbage
   collector contradicts `docs/reference/positioning.md`.
4. **`async`:** stackful green threads (recommended: simplest, no function
   colouring, matches the existing `test_no_coloring`), or compiled state
   machines.
5. **Platform order:** Linux first (recommended), then macOS, then ARM64.

## Track A — Release foundations

Everything else relies on these.

- [x] A1. `fidesc test` exits non-zero on a failure (S).
- [x] A2. Dev probes (`probe_*`, `test_pe_ir_*`) are left out of the suite (S).
- [x] A3. CI runs the unit tests, suite, expected-fail tests, the stdlib check
      and the native fixpoint on every push (`.github/workflows/ci.yml`); first
      green run Nov 1, Windows and Linux. Linux only builds, runs unit tests and
      one C program so far (F1).
- [ ] A4. The suite runs on the self-hosted compiler too, not only `fidesc` (M).
      `tests/` parity is a local script today.
- [ ] A5. `_fail` tests name the error they expect. Today any failure passes,
      so a test can reject a program for the wrong reason (M).
- [ ] A6. The effect, capability, region and safety tests in subdirectories run
      in `fidesc test`, not only `cargo test` (S).
- [ ] A7. Refresh `tests/bootstrap/baseline/test_results.txt`; it is stale (S).
- [ ] A8. Fix `fid run` tensor `mean` (wrong value; `test_tensor`) (S).
- [ ] A9. Fix `handle` expression values: `test_no_coloring` reads an unset
      value on every backend (M).
- [ ] A10. `release/fid.exe` from the release bootstrap replaces stale builds;
      the installer ships it and `fid emit` is tested from an installed layout
      in CI (S).
- [ ] A11. `verify-selfhost.sh` baseline restored (S).
- [ ] A12. Find why `test_spawn_gt` failed natively once on CI: it passed on the
      next run and 320 of 320 here under load. The harness now prints the
      reason if it comes back (S).
- [ ] A13. `run_with_timeout` reads a child's output only after it exits, so a
      test printing more than the pipe holds would hang until the limit (S).

**Acceptance:** a red test blocks a merge, on both compilers.

## Track B — Standard library

On Nov 1, 13 of 108 modules did not compile when imported on their own. All do
now, and `test_stdlib_repaired` runs them. Running them found what compiling
could not: `template` printed JSON quotes, `csv_get` returned them, `replay`
could not read its own files, and `skel` stored the wrong type in a track.
Most modules still have no test, so expect more of this (B12).

- [x] B1. `data`: `json_escape_string` undefined (S).
- [x] B2. `http`: `json_get` undefined (S).
- [x] B3. `ecs`, `serialize`: JSON names and return types (S).
- [x] B4. `sqlite`: visibility error on `int_to_str` (S).
- [x] B5. `game`: `float_from_i64` undefined (S).
- [x] B6. `mesh`, `terrain`: `str_split` called with a char, not a str (S).
- [x] B7. `net`, `replay`: `&mut Vec` passed where `Vec` is declared (S).
- [x] B8. `shape`: tensor methods called as free functions (S).
- [x] B9. `skel`: a `Vec<Keyframe>` stored in a bare `Vec` field (S).
- [x] B10. `template`: `str_char_at` undefined (S).
- [x] B11. A CI step imports every stdlib module on its own, so this cannot
      regress (S).
- [ ] B12. Every module has a test and a `docs/stdlib/` page; 36 of 108 have no
      page (L).
- [ ] B13. Replace bare `Vec` with `Vec<T>` across the stdlib, then make the
      `str`/`i64` rule strict in imported modules too (L).
- [ ] B14. `StringBuilder` is quadratic (`buf + s`): back it with `p_sb_*` (S).
- [ ] B15. Remove the false claims in stubs: `@derive(Serialize)` emits constant
      data; `ota_verify_manifest` only checks a prefix; `sqlite` has no
      parameter binding, so it is open to injection (M).

- [ ] B16. Modules share one namespace, so 44 function names clash and those
      pairs cannot be imported together: `file`+`fs` (11), `simd`+`tensor` (8),
      `args`+`os`, `encoding`+`url`, `env`+`os`, `arena`+`http`, `csv`+`data`.
      Add qualified names (`json::get`) or rename (M).
- [ ] B17. A private function in one module must not shadow a core function for
      other modules: `sqlite`'s local `int_to_str` broke `json` and `string` (S).
- [ ] B18. A struct defined in two modules must be an error, as a function is:
      `core` and `buffer` both had a public `ByteBuffer`, and each compiler
      silently picked a different one (S).

**Acceptance:** every stdlib module compiles and has a passing test.

## Track C — Memory

- [x] C0. Real allocator on both native paths; `Vec` growth and `HashMap`
      rehashing reuse memory; local `Vec`s freed by the compiler.
- [ ] C1. Free string temporaries: the intermediate results of `a + b + c`, and
      a fresh call result used once as a concat operand (M).
- [ ] C2. Free local strings that never escape, by extending the
      `auto_drop_vecs` pass (M).
- [ ] C3. Free `Vec`s passed only to functions that do not keep them: per-function
      escape summaries, as `compiler/escape.fd` has for references (L).
- [ ] C4. Half-step size classes, so a 33-byte string does not take 64 bytes (S).
- [ ] C5. Free elements of a dropped `Vec<str>` and `Vec<Vec<T>>` when they are
      owned (L).
- [ ] C6. Regions really free: `region { }` releases its allocations, with a
      compile-time rule that nothing outside can hold them. Today an outer
      `Vec` growing inside a region would dangle, which is why it stays a no-op (L).
- [ ] C7. `HashMap` locals freed like `Vec`s (M).
- [ ] C8. `.fidcache` eviction: it grows without limit today (S).

**Acceptance:** a 24-hour text-processing soak test keeps a flat memory profile;
30M temporary strings peak under 50 MB.

## Track D — Native performance

"Faster than everything" cannot be measured, so the aim is two things that can:
the fastest build-to-native among compiled languages, and native code within
1.5× of C on the benchmarks above with no C compiler installed.

- [x] D0. `Vec_get`/`Vec_len` inlined; allocator state cache-aligned;
      type checking about 14× faster.
- [ ] D1. Inline `Vec_set` and `Vec_push`'s fast path (S).
- [ ] D2. Division by a constant as a multiplication (`i % 7` costs an `idiv`) (S).
- [ ] D3. Bounds-check and overflow-check elimination on simple counted loops (M).
- [ ] D4. Register allocation for the PE backend: values live in stack slots
      today, the main cause of the 3–7× gap to C (L, see decision 1).
- [ ] D5. Inlining of small functions at IR level (M).
- [ ] D6. Run the IR optimizer on the native path (M).
- [ ] D7. Benchmarks in CI, failing on a slowdown over 10% (M).
- [ ] D8. Borrow checking is now the slowest compiler phase (about 30 s on the
      compiler itself): profile and fix it, as for the type checker (M).
- [ ] D9. `emit` of hello world under 0.5 s: stop re-checking the stdlib on each
      build, by caching its checked form (L).

**Acceptance:** the Target column above; self-compile under 60 s.

## Track E — C interop

- [ ] E1. `extern "C"` from any DLL in native builds: `link "x"` adds to the PE
      import table (M). `ccompile` is needed today beyond msvcrt and kernel32.
- [ ] E2. `use sqlite` and `use sdl` work through `emit` (depends on E1, B4) (S).
- [ ] E3. Struct-by-value and callback arguments in the native thunks (M).
- [ ] E4. `fid` finds and reports a missing library clearly (S).
- [ ] E5. `parse-header` output compiles for a real header set (SDL2, SQLite) (M).

**Acceptance:** one SDL2 and one SQLite example build with `fid emit` and no C
compiler.

## Track F — Platforms

- [ ] F1. Run the whole suite on Linux x64 through C, in CI (M). Not run so far.
      `fidesc` did not even build on Linux until Nov 1: a missing import.
- [ ] F10. The C path works with MinGW `gcc`: linking fails because `-Wl,-z,...`
      is ELF-only (`src/main.rs`, `compiler/main.fd`). `zig cc` accepts it. CI's
      diagnostic step proves the fix (S).
- [ ] F2. `fid` builds and bootstraps itself on Linux (M).
- [ ] F3. Install archives tested on a clean Linux machine (S).
- [ ] F4. Native ELF path for `fid`: an IR→ELF backend sharing the x86-64
      encoder with `pe_ir` (L).
- [ ] F5. macOS x64 and ARM64 through C, in CI (M).
- [ ] F6. Native ARM64/RISC-V/WASM backends: overflow and divide-by-zero
      semantics match the other backends (M).
- [ ] F7. Native macOS: Mach-O executables, not only objects (L).
- [ ] F8. ARM64 and RISC-V: replace the runtime stubs (19 and 18 zero stubs, 46
      capability stubs), and test under QEMU (XL).
- [ ] F9. WASM: globals, struct copies, collections (L).

**Acceptance:** Windows and Linux are fully supported at 1.0; everything else is
listed as experimental in the docs, with what does not work.

## Track G — Bare metal

- [ ] G1. Bare-metal tests run under QEMU in CI, not hosted, where they are
      expected to fault (M).
- [ ] G2. `fid emit --bare` works: `pe_ir` panics with "bare mode not ported" (M).
- [ ] G3. Bare builds define their integer semantics: panics need a handler, so
      overflow and division are unchecked today (M).
- [ ] G4. `fides-os` builds with the released compiler, in its own CI (M).

**Acceptance:** a kernel boots and prints under QEMU, from a clean checkout.

## Track H — Concurrency

- [ ] H1. `spawn` really runs concurrently on the Windows C path: it runs
      synchronously there today, and differs from native (M).
- [ ] H2. Mutexes on the Windows C path are real (S).
- [ ] H3. Data races rejected: a global or a captured `var` used from a spawned
      function needs a lock or an atomic (L). Today one compiles silently and
      10,000,000 increments give about 5.7M.
- [ ] H4. Enforce `@send`/`@sync` for every spawn argument (M).
- [ ] H5. Real `async`: tasks suspend and resume (L, see decision 4). Today
      `async` runs eagerly and `yield` does nothing.
- [ ] H6. Green threads outside the interpreter (L).
- [ ] H7. `parallel { }` runs multi-statement branches concurrently (M).
- [ ] H8. Allocator: per-thread caches instead of one global lock (M).

**Acceptance:** a race test is rejected; 10,000 concurrent tasks run on a few
threads.

## Track I — Networking

Depends on B, C, H5 and J.

- [ ] I1. TCP in the native runtime: `net` goes through a C header
      (`pledge_net.h`) and has no `listen` or `accept` (M).
- [ ] I2. Non-blocking I/O tied to the `async` runtime (L).
- [ ] I3. TLS client and server through a reviewed library (L, see decision 2).
- [ ] I4. HTTP/1.1 client and server that handle real traffic, with tests
      against a real client (L). `http` is a thin blocking wrapper today.
- [ ] I5. `examples/http_server` runs on a native build (S).
- [ ] I6. A sample service survives a 24-hour load test (M).

**Acceptance:** an HTTPS service runs for 24 hours under load, with flat memory.

## Track J — Cryptography

Nothing here is ready to protect real data yet. Do not write new primitives.

- [ ] J1. `crypto` tests fail on a mismatch: `real_crypto.fd` prints `MISMATCH`
      and still passes (S).
- [ ] J2. SHA-256 and HMAC checked against the NIST test vectors (S).
- [ ] J3. `crypto.fd`'s header claims SHA-512 and a CSPRNG that do not exist:
      remove the claims or add them (S).
- [ ] J4. A CSPRNG from the OS (`BCryptGenRandom`, `getrandom`) (S).
- [ ] J5. AEAD, signatures and a KDF through bindings (L, see decision 2).
- [ ] J6. Apply `@constant_time`, `@secret` and `@zeroize` in `stdlib/crypto.fd`
      itself: it uses none of them today (M).
- [ ] J7. `@constant_time` verified on the PE path: its asm verifier covers x86-64
      ELF only (L).
- [ ] J8. `@zeroize` reaches old `Vec` buffers and cannot be optimised away (M).
- [ ] J9. Independent security review before anything is called production
      ready (external).

**Acceptance:** vectors pass in CI; a third party signs off.

## Track K — Language soundness

- [x] K0. `val`/`const` reassignment, non-exhaustive `match`, wrong `Vec` element
      types and `str`/`i64` mixing in a program are rejected; safety rules follow
      method calls.
- [ ] K1. Raw pointer dereference outside `unsafe` is an error: a null `*p`
      compiles and only panics at runtime (M).
- [ ] K2. Literal range checks: `var x: u8 = 300` is rejected (S).
- [ ] K3. Silent narrowing rejected: an `i64` assigned to an `i32` truncates (M).
- [ ] K4. Iterator invalidation: `for x in v { v.push(1); }` is rejected (M).
- [ ] K5. Borrow-checker gaps: `s.x = 1`, `&v[i]`, trait impl bodies (M).
- [ ] K6. `&(rvalue)` no longer points at a dead temporary (M).
- [ ] K7. `@deterministic`, `@crashfree` and `sandbox` cover effects hidden in
      method bodies: extend the effect table with `p_file_*`, `p_socket_*`,
      `p_system`, `p_clock_now` (M).
- [ ] K8. `@no-indirect`, `@bounded-stack`, `@tainted` either work or are removed
      (M). They parse but do nothing useful.
- [ ] K9. Generic bounds are checked; an unbound `T` is an error, not `i32` (M).
- [ ] K10. `@cfg` and `@simd` work on `fid` as on `fidesc` (M).
- [ ] K11. A language reference that matches both compilers, with the differences
      closed (M).
- [ ] K12. Semver and a stability promise for 1.0 (S).

**Acceptance:** every line in `docs/reference/features.md` has a test proving it,
and a test proving what it rejects.

## Track L — Safety-critical (after 1.0)

Fides must not be described as suitable for medical, automotive or avionics use
until this track is complete.

- [ ] L1. Every safety annotation enforced, with a negative test per rule (L).
- [ ] L2. Remaining stubs in the safety-annotation checks closed (M).
- [ ] L3. `qualify` reports complete evidence (L).
- [ ] L4. Requirements traceability from each rule to its tests (L).
- [ ] L5. A qualified toolchain process per IEC 62304, ISO 26262 or DO-178C,
      through a certification body (XL, external).

**Acceptance:** an external assessor's report, not our own claim.

## Order of work

1. **A, B:** quick, and every other track relies on them.
2. **C1–C4, D1–D3, E1–E3:** the cheap wins with the most visible effect.
3. **F1–F3, H1–H2, K1–K3:** the rest of the beta gate. **→ Public beta**
4. **D4, H3, H5, C3, I, J:** the large projects, after decisions 1–4.
5. **K, remaining F, docs.** **→ Stable 1.0**
6. **G, F7–F9, L:** after 1.0.

## Notes

- Keep `src/` and `compiler/` in step: a change to one is not done until the
  other matches and both pass.
- Do not claim a feature in `README.md` or `positioning.md` until its track says
  `[x]`. Several current claims are ahead of the code.
- Related roadmaps: `inc-roadmap.md`, `pe-roadmap.md`, `llvm-roadmap.md`,
  `security-roadmap.md`, `safety-critical.md`.
