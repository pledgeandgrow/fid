# Release Roadmap

> Goal: a public beta people can build real programs on, then a stable 1.0.0
> whose promises are all true and all tested. Every track below is part of 1.0.0,
> and all of it is work we can do ourselves. What needs a certifier or money is
> set aside, at the end.
>
> Created: Nov 1, 2026. Update it in the same commit as the work.
> `[x]` done and verified, `[~]` partly done (the note says what is left),
> `[ ]` not started. Sizes are effort, not dates: S = hours, M = days,
> L = weeks, XL = months.

## Where it stands (Nov 2, 2026)

**Works today (tested):** native Windows `.exe` files with no C compiler; the
self-hosted compiler reproduces itself byte for byte; one integer meaning on
the interpreter, C and native paths; sound incremental builds; a real
allocator; local `Vec`s and strings freed by the compiler, a `Vec` passed to
functions that keep nothing included; `@secret`; native threads; `async` that
really suspends; sockets on the native path; HTTP/1.1 and HTTPS, client and
server, 28,000 requests/s on one thread; TLS through Schannel and OpenSSL; C
callbacks and small structs by value; a stdlib with typed `Vec`s and a test for
every module; language support running inside VS Code.
Suite: interpreter 339/339, C 343/343, native 345/345, expected-fail 89/89.
The self-hosted compiler reproduces itself from its second generation on.

**Measured starting point** (Windows x64, seconds):

| Benchmark | fid native | fid via C -O2 | Rust -O | Target for 1.0 |
|---|---|---|---|---|
| fib(40) | 1.77 | 2.95 | 0.46 | ≤ 1.0 |
| Arithmetic loop, 2 billion | 14.0 | 3.51 | 3.89 | ≤ 5.0 |
| Sieve, 20M | 2.63 | 0.42 | 0.31 | ≤ 0.6 |
| HashMap, 1M strings | 1.65 | 0.78 | 1.09 | ≤ 1.0 |
| Peak memory, 30M temporary strings | 2.3 GB | 3.4 GB | 5 MB on 3M | ≤ 50 MB |

**Now** (Nov 2, same machine, best of 3, Rust re-measured in the same run):

| Benchmark | fid native | fid via C -O2 | Rust -O | Target for 1.0 |
|---|---|---|---|---|
| fib(40) | 1.34 | 0.81 | 0.53 | ≤ 1.0 |
| Arithmetic loop, 2 billion | 7.59 | 3.53 | 5.36 | ≤ 5.0 |
| Sieve, 20M | 0.82 | 0.48 | 0.40 | ≤ 0.6 |
| HashMap, 1M strings | 1.04 | 0.80 | 1.12 | ≤ 1.0 |
| Peak memory, 30M temporary strings | not re-measured | not re-measured | 5 MB on 3M | ≤ 50 MB |

Through C, three of four targets are met and two benchmarks beat Rust; fib is
left, where Rust's release mode drops the overflow check Fides keeps. Native
meets the sieve target and is behind on the others: release D6, D13.

Self-compile (`fid dump-c compiler/main.fd`, the compiler built through C):
112–137 s on Nov 2, down from 601 s; 21 s with `fidesc`. See D10, D11.

**Which gap is handled where:**

| Gap | Track |
|---|---|
| `use sqlite` and `use sdl` are untested through `emit` | E |
| A server's request strings stay allocated unless its handler promises to keep none | M, C |
| Native builds are about twice as slow as C on tight loops | D |
| Bare-metal tests only run hosted, where they fault | G |
| Linux has not had a green CI run yet; the last failure was in the test harness | F |
| The 24-hour load test has not been run | I |
| Only SHA-256 and HMAC, unreviewed | J |
| macOS, ARM64, RISC-V and WASM are partial | F |
| Safety annotations are not certified or fully enforced | K, L |
| Safe code can still corrupt memory | M, K |
| The package index is not public; the extension is not on the Marketplace | N |

## Release gates

**Public beta** needs: all of tracks A and B; C1–C4; D1–D3; E1–E3; F1–F3;
H1–H2; K1–K3; M1–M3, M6; N1–N5. A beta may have gaps, but every gap must be
documented.

**Stable 1.0.0** needs: every item of every track, A to N, plus every guide and
feature table matching what the compiler does. That includes bare metal (G),
native macOS, ARM64, RISC-V and WASM (F7–F9), crypto verification (J7–J8) and the
safety-critical evidence (L1–L4).

What needs an outside party or money is set aside (see the end) and does not
block 1.0.0. Until those items are done, crypto stays marked "not for production"
and Fides is not described as suitable for medical, automotive or avionics use.

## Decisions needed

Each changes the work below. Recommendations first; none is taken yet.

1. **Native speed:** write a register allocator for the PE backend (recommended
   for 1.0: no dependencies, keeps "no C, no LLVM"), or lean on the C path or
   `llvm-roadmap.md` for release builds.
2. **TLS and crypto:** bind reviewed libraries (done: Schannel on Windows, the
   system's OpenSSL elsewhere). Writing our own TLS is
   not an option for a first release.
3. **Memory model:** compiler-inserted frees with escape analysis (recommended:
   matches "no GC"), or real ownership with moves checked everywhere. A garbage
   collector contradicts `docs/reference/positioning.md`.
4. **`async`:** stackful green threads (recommended: simplest, no function
   colouring, matches the existing `test_no_coloring`), or compiled state
   machines.
5. **Platform order:** Linux first (recommended), then macOS, then ARM64.
6. **Package registry:** an index in a Git repository (recommended: cheap to run
   and easy to mirror), or a hosted service.

## Track A — Release foundations

Everything else relies on these.

- [x] A1. `fidesc test` exits non-zero on a failure (S).
- [x] A2. Dev probes (`probe_*`, `test_pe_ir_*`) are left out of the suite (S).
- [x] A3. CI runs the unit tests, suite, expected-fail tests, the stdlib check
      and the native fixpoint on every push (`.github/workflows/ci.yml`); first
      green run Nov 1, Windows and Linux. Linux only builds, runs unit tests and
      one C program so far (F1).
- [~] A4. The suite runs on the self-hosted compiler too, not only `fidesc` (M).
      Done: `fidesc test --compiler release/fid.exe`. Left: in CI.
      `tests/` parity is a local script today.
- [x] A5. `_fail` tests name the error they expect. Today any failure passes,
      so a test can reject a program for the wrong reason (M).
- [x] A6. The effect, capability, region and safety tests in subdirectories run
      in `fidesc test`, not only `cargo test` (S).
- [x] A7. Refresh `tests/bootstrap/baseline/test_results.txt`; it is stale (S).
- [x] A8. Fix `fid run` tensor `mean` (wrong value; `test_tensor`) (S).
- [x] A9. Fix `handle` expression values: `test_no_coloring` reads an unset
      value on every backend (M).
- [ ] A10. `release/fid.exe` from the release bootstrap replaces stale builds;
      the installer ships it and `fid emit` is tested from an installed layout
      in CI (S).
- [~] A11. `verify-selfhost.sh` baseline restored (S). Saved at 36/47 on all three
      backends. Left: the 11 others are old debugging probes (`probe_*`, removed names,
      the old `str` rule) or run the compiler inside the interpreter, which is slow.
- [ ] A12. Find why `test_spawn_gt` failed natively once on CI: it passed on the
      next run and 320 of 320 here under load. The harness now prints the
      reason if it comes back (S).
- [x] A13. `run_with_timeout` reads a child's output only after it exits, so a
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
- [x] B12. Every module has a test and a `docs/stdlib/` page (L). `tests/bootstrap/
      test_lib_*.fd`; those of `sdl`, `ui`, `audio_mixer`, `image`, `font`, `gl`,
      `shader` and `sys` need their library and run with `FIDES_TEST_EXTERNAL=1`.
- [x] B13. Replace bare `Vec` with `Vec<T>` across the stdlib, then make the
      `str`/`i64` rule strict in imported modules too (L). Only `core`, `vec` and
      the self-hosted compiler keep the old rule. `Deque`, `Queue` and `Stack`
      still hold `Vec<i64>` inside.
- [x] B14. `StringBuilder` is quadratic (`buf + s`): back it with `p_sb_*` (S).
- [~] B15. Remove the false claims in stubs (M). Done: `@derive(Serialize)` writes
      the real values, and `ota_verify_payload` checks size and SHA-256. Left:
      `sqlite`'s `query_params` binds values, but has not been run: SQLite is not
      installed here.

- [x] B16. Two imported modules may define the same function: `module::name()`
      picks one, and `use json as j` gives `j::name()`.
- [x] B17. A private function in one module must not shadow a core function for
      other modules: `sqlite`'s local `int_to_str` broke `json` and `string` (S).
- [x] B18. A struct defined in two modules must be an error, as a function is:
      `core` and `buffer` both had a public `ByteBuffer`, and each compiler
      silently picked a different one (S).
- [~] B19. Bugs the module tests found and left open (M). Fixed: direct `p_vec_*`
      calls did nothing in the interpreter (D14). Left: an untyped `Vec::new()` of floats is misread as
      `i64` in C; `T { a: 1 }` of an imported struct does not parse; a user
      function named `yn` or `j0` clashes with libm in C; the interpreter prints a
      `bool` from a `Vec<bool>` as 0 or 1; `__ecs_*` in the interpreter miss a
      `str` passed through a wrapper; `LN_CSWI2` has no name in `grid`;
      `font_render_text` starts from opaque black; a closure that captures a `Vec` is
      refused by the borrow checker ("cannot mutate v").

**Acceptance:** every stdlib module compiles and has a passing test.

## Track C — Memory

- [x] C0. Real allocator on both native paths; `Vec` growth and `HashMap`
      rehashing reuse memory; local `Vec`s freed by the compiler.
- [x] C1. Free string temporaries: the intermediate results of `a + b + c`, and
      a fresh call result used once as a concat operand (M).
- [x] C2. Free local strings that never escape, by extending the
      `auto_drop_vecs` pass (M).
- [x] C3. Free `Vec`s passed only to functions that do not keep them: per-function
      summaries of which arguments are only borrowed (L).
- [x] C4. Half-step size classes, so a 33-byte string does not take 64 bytes (S).
- [x] C5. Free elements of a dropped `Vec<str>` and `Vec<Vec<T>>` when they are
      owned (L).
      Done Nov 4: a local `Vec<str>`/`Vec<Vec<T>>` whose elements are fresh and
      never escape frees them (`Vec_drop_strs`, `Vec_drop_vecs`);
      test_mem_owned_drop.
- [x] C6. Regions really free: `region { }` releases its allocations, with a
      compile-time rule that nothing outside can hold them. Today an outer
      `Vec` growing inside a region would dangle, which is why it stays a no-op (L).
      Done Nov 4: collections declared in a `region` and kept local are freed at
      its end (test_mem_owned_drop).
- [x] C7. `HashMap` locals freed like `Vec`s (M).
      Done Nov 4: `HashMap` locals freed like `Vec`s (`HashMap_drop`).
- [x] C8. `.fidcache` eviction: it grows without limit today (S).
      Done Nov 4: `.fidcache` evicts least recently used artifacts past 64 files
      or 1 GB.

**Acceptance:** a 24-hour text-processing soak test keeps a flat memory profile;
30M temporary strings peak under 50 MB.

## Track D — Native performance

"Faster than everything" cannot be measured, so the aim is two things that can:
the fastest build-to-native among compiled languages, and native code within
1.5× of C on the benchmarks above with no C compiler installed.

- [x] D0. `Vec_get`/`Vec_len` inlined; allocator state cache-aligned;
      type checking about 14× faster.
- [x] D1. Inline `Vec_set` and `Vec_push`'s fast path (S).
- [x] D2. Division by a constant as a multiplication (`i % 7` costs an `idiv`) (S).
- [x] D3. Bounds-check and overflow-check elimination on simple counted loops,
      on the native path (M).
- [~] D4. Registers for the PE backend. Done: the five hottest locals of a
      function and short-lived temporaries live in callee-saved registers,
      weighted by loop depth; parameters read more than once keep a register.
      Left: other values that cross blocks, caller-saved registers, floats (L).
      Nov 6: a whole-function allocator (pe_ir.fd peir_global_ra,
      PEIR_GLOBAL_RA): liveness by dataflow over the blocks, one interval
      per word-sized SSA value, parameter and local whose accesses are all
      whole-word loads and stores, linear scan over the seven callee-saved
      registers, the coldest interval (uses weighted by loop depth) left in
      memory. Values that cross blocks, all parameters and any number of
      locals now get registers. The suite (364 programs, `fid emit`) gives the
      same output as before. Left: caller-saved registers (the emitter uses
      rax, rcx, rdx and r8-r11 as scratch) and floats in xmm registers.
      Nov 7: fixpoint kept with it on (stage 2 == stage 3, 52 MB of C).
      Then (competitive OP9, OP11, OP16-OP18, OP24): copy coalescing, r10/r11
      and xmm registers, loop rotation, instruction selection, integer
      ranges, loop-invariant code motion. 101 native programs (every test
      with a loop or a float) give the same output as before, and
      test_native_opt passes on run, C and native; fixpoint kept (52.8 MB).
      bench/fides, `fid emit`, best of 3: loop 5.2 -> 3.8 s, sieve 0.70 ->
      0.59 s, fib unchanged (1.3 s: its checks and the save of two registers
      on the early return).
      `fid emit`, best of 3, against the compiler before D4/D13/D14:
      bench/fides fib 1358 -> 1274 ms, loop 8261 -> 5190 ms, sieve
      793 -> 723 ms.
- [x] D5. Inlining of small functions at IR level (M).
      Done Nov 4: compiler/iropt.fd inlines single-block functions of up to 16
      instructions.
- [x] D6. Run the IR optimizer on the native path (M).
      Done Nov 4: `fid emit` runs iropt (inlining, constant folding, dead code)
      before machine code.
- [~] D7. Benchmarks in CI, failing on a slowdown over 10% (M).
      Written Nov 4: bench/check.sh fails on a slowdown over LIMIT_PCT against
      bench/baseline.txt; not in CI.
- [~] D8. Borrow checking is now the slowest compiler phase (about 30 s on the
      compiler itself): profile and fix it, as for the type checker (M).
      Nov 6: profiled (gprof on a Linux -pg build of the compiler): the phase
      printed as "borrowck" is mostly lints (15 s of 31 under gprof: the
      called-functions lists were deduplicated by scanning before each push),
      the escape check (5.6 s) and substitution (4.2 s); borrow checking
      itself is 1.8 s. Lists now gathered and made sets once: the phase is
      7 s of a self-compile (31 s under gprof before, which runs about twice
      as slow). Left: the escape check.
- [~] D9. `emit` of hello world under 0.5 s: stop re-checking the stdlib on each
      build, by caching its checked form (L).
      Done Nov 4: core.fd is checked once per (compiler, core.fd) pair, a stamp
      in the temp directory skips its bodies after. Hello-world timing not
      re-measured.
      Measured Nov 6: 3.2–3.7 s (front end 2.2 s, then lowering and codegen of
      the whole stdlib). Left: the checked stdlib cached in a binary form
      (competitive CS2) and lowering only what is reached.
- [x] D11. Per-call cost on the C path (M). Functions no longer push and pop a
      call-stack array: panic traces walk the machine stack (unwind tables on
      Windows, `_Unwind_Backtrace` elsewhere; `zig cc` off Windows links
      `-lunwind`), and a stack overflow hits the OS guard page, with a handler on
      reserved stack (`SetThreadStackGuarantee`, `sigaltstack`). The fixed limit of
      10,000 nested calls is gone. Task stacks on POSIX have a guard page.
      fib(40) through C: 3.0 s to 0.9 s.
- [x] D12. The counted-loop check removal (pe.rs `unchecked_ops`) is shared with
      the C backend, and proves more: a length kept in a local, `p_vec_*` calls,
      print calls inside the loop; no `Vec` shortened between the length and
      the loop. `Vec_push` inlines to a store (growth out of line).
- [~] D13. The native emitters (`pe.rs`, `pe_ir.fd`) waste less per call and
      branch (M): shadow space reserved once in the prologue, a comparison used
      only by its branch sets flags without a stored bool, no jump to the block
      laid out next, parameters in registers, short immediates, `imul` by an
      immediate, no register for a call's null environment. Left: the optimizer
      on this path (D6) and a full register allocator (competitive OP16).
      Nov 6: with values in registers (D4), an i64 add, sub, mul, and, or or
      xor whose result is in a register is computed there (`mov rd, l; op rd,
      r`) instead of through rax and rcx, and loads and stores of a local in
      a register are one move. Left: comparisons and branches on registers,
      and copy propagation between the moves that remain.
- [~] D14. `Vec` elements at their real size (L). Stage A done: one `Vec`
      implementation, core's. Every backend's `p_vec_*` builtins run core's
      `Vec_*` functions; on native and the interpreter they disagreed with it
      (`clone` shared the data, `first` returned garbage, builtins did nothing
      on a core `Vec`): `test_vec_builtins.fd` now agrees everywhere. Left: an
      element size in the header, typed access in lowering, structs inline.
      About 400 places know today's layout.
      Counted Nov 6: 39 Vec functions in core (30 word-size multiplications),
      80 `data[i]` accesses in the C runtime (PVec is `void** data`), about
      57 in the native emitter and stubs, about 60 in stdlib/pe_runtime, 73
      Vec calls the lowering emits, copy-on-write's element copies
      (Vec_share_deep), and the interpreter's own Vec. Copy-on-write (M5),
      its prerequisite, is on since Nov 6.
      Stage B done Nov 7: small integers and bools at their size. The Vec
      header is 32 bytes; the word at +24 is the element code (0: one word per
      element; else a tag plus 1/2/4 for u8/bool, u16, u32, negative for
      i8/i16/i32, so a 24-byte header from an older runtime reads as a word
      Vec). `var v: Vec<u8> = Vec::new()` (and the other small types)
      lowers to core's Vec_new_sized; every core Vec function dispatches on
      the code (vec_sz_*), as do the C runtime's fast bodies and helpers,
      native inline get/set/push (sized ones take the core call), print and
      to_str, zeroize, file writes, copy-on-write (shares keep the code,
      vec_own copies len × size bytes) and the interpreter.
      test_vec_sized (same output on every backend, fidesc included) and
      test_vec_sized_layout (fid only: codes, sharing, growth, a Vec<u8>
      written as 4 bytes) pass on run, C and native; fixpoint kept. Left: a
      Vec built any other way (a struct field, a function's result, `Vec<u8>`
      from p_vec_new) still has word elements; floats and structs stay one
      word (stage C: structs inline).
- [ ] D15. References into containers: `&v[i]` and large structs passed by
      pointer, not copied (needs D14) (L).
- [~] D16. Strings carry their length: O(1) length, zero bytes allowed,
      slices without copying (competitive RT2) (L).
      Nov 6: every string a program makes has [magic][length] before its
      bytes: core's (p_str_alloc), the C runtime's concatenation and
      p_char_to_str, the native ones, and now literals too (a static magic:
      never freed), in the C generator's string table and in native .rdata.
      str_len is O(1) for all of them; p_str_equal returns at once on
      different lengths and compares a word at a time; concatenation,
      printing, byte access and comparison keep zero bytes (test_str_length).
      Strings from C (FFI) have no header and are measured as before. Left:
      slices without copying, which need a (pointer, length) string; the C
      runtime's other string functions (split, replace, ...) still stop at a
      zero byte.
      Nov 7: also on `fid run` (the interpreter stores strings with the
      header and reads them back by length) and native `==` (it measured
      with strlen): test_str_length passes on run, C and native.
- [~] D10. Self-compile under 60 s (M). Profiled with a stack sampler: 601 s to
      136 s, 4.4 times faster, fixpoint kept. Fixed: on Windows `printf` wrote
      stdout a byte at a time (31% of the time, and every printing program on the C
      path); instruction lookup by id scanned the whole function whenever it had
      parameters (20%); comptime, function, string and default-argument lookups
      scanned lists by name. Left: string compares in method lookup and in the
      parser's token checks; and the compiler is built at -O0, since optimizing
      one 44 MB C file needs more than 16 GB.
      Nov 6: 133 s to 93 s (stage 2: 103 s), fixpoint kept. The gprof
      profile: 48% of the time in string compares, from name scans: every
      method call on a variable scanned all structs and enums for an
      associated function, effect inference looked function bodies up by
      name, enum lookups in lowering and field offsets in the C generator
      scanned lists. Now hash indexes (TypeChecker struct/enum positions,
      tc_body_index, Lowerer.enum_index, IrModule.struct_index,
      CCodeGen.layout_index); type checking 59 s to 24 s. Left: parsing,
      where token_kind_tag (a 150-arm match) runs 28 million times and its C
      function zeroes about 1,900 locals per call (an enum match as a C
      `switch`, or no zeroing of SSA temporaries, would fix it); lowering
      (19 s) and C generation (10 s).

**Acceptance:** the Target column above; self-compile under 60 s.

## Track E — C interop

- [x] E1. `extern "C"` from any DLL in native builds: `link "x"` adds to the PE
      import table (M). `ccompile` is needed today beyond msvcrt and kernel32.
- [x] E2. `use sqlite` and `use sdl` work through `emit` (depends on E1, B4) (S).
      Done Nov 6: a `link` DLL is no longer a static import; the program
      binds it at start-up (LoadLibraryA, GetProcAddress into the same IAT
      slots). `link "sqlite3"` tries sqlite3.dll, then winsqlite3.dll, which
      every Windows 10 and 11 has, so `use sqlite` runs with nothing
      installed (test_pe_sqlite, test_sqlite through `emit`). stdlib/sqlite.fd
      passed `&s` of a `str` as SQLite's out-parameters, which is the string
      itself: SQLite wrote its pointers over string literals; now word slots.
      `use sdl` builds and binds SDL2*.dll the same way; not run here (no
      SDL2 on this machine), and its `pledge_sdl_*` helpers are still stubs
      returning 0 natively, as on the C path.
- [x] E3. Struct-by-value and callback arguments in C calls: a struct of 1, 2, 4
      or 8 bytes by value and as a result, a named Fides function as a callback,
      native and C (M).
- [x] E4. `fid` finds and reports a missing library clearly (S).
      Done Nov 6 for `emit`: it warns at build time when a `link` DLL is not
      in the current folder, the system folder or PATH; the program runs,
      and the first call into a missing DLL or function stops it with
      `error: this program needs X.dll, which Windows could not load` (or
      `X.dll has no function F`) and exit code 1 (test_pe_dll_missing_fail,
      test_pe_dll_nofn_fail). Left: `ccompile` still reports the linker's
      own message.
- [x] E5. `parse-header` output compiles for a real header set (SDL2, SQLite) (M).
      Done Nov 6: compiler/cparser.fd rewritten: quoted `#include`s followed,
      object- and function-like macros, attribute macros dropped, typedefs,
      structs, enums, function pointers, constant expressions (`#define` and
      enum values, casts and macro calls included). sqlite3.h 3.54.1: 356
      functions and 530 constants (stdlib/sqlite3_h.fd), the 11 left out are
      varargs or va_list. SDL.h 2.32.10 and its 60 headers: 852 functions and
      1413 constants (stdlib/sdl2_h.fd), 26 left out (varargs, va_list, a
      struct by value). test_parse_header_sqlite runs with `fid emit` and no
      C compiler; test_parse_header_sdl and examples/sdl_window.fd build (the
      window needs SDL2.dll, not on this machine).

**Acceptance:** one SDL2 and one SQLite example build with `fid emit` and no C
compiler.

## Track F — Platforms

- [x] F1. Run the whole suite on Linux x64 through C, in CI (M). Green on Nov 2
      (6f94314). Locally, the generated C of every test ran under gcc on
      WSL, which found and fixed `p_system`'s status and a use after free in
      `process_capture`.
- [x] F10. The C path works with MinGW `gcc`: linking fails because `-Wl,-z,...`
      is ELF-only (`src/main.rs`, `compiler/main.fd`). `zig cc` accepts it. CI's
      diagnostic step proves the fix (S).
- [x] F2. `fid` builds and bootstraps itself on Linux, in CI (M). Green on Nov 2.
- [x] F3. Install archives tested on a clean Linux machine: a clean Ubuntu
      container in CI (S). Green on Nov 2.
- [~] F4. Native ELF path for `fid`: an IR→ELF backend sharing the x86-64
      encoder with `pe_ir` (L).
      First step Nov 6: `fid emit --elf` (or `--target linux`) writes a
      static Linux x86-64 executable from pe_ir's code generator, the same
      machine code as the Windows build, with no C compiler, linker or libc:
      ELF header, one RWX segment at 0x400000, the bare build's runtime with
      Linux system calls for output, the heap (mmap) and exit. It runs a
      program without the stdlib (test_elf_native, run under WSL). Left: the
      stdlib, i.e. stdlib/pe_runtime on Linux (its msvcrt and kernel32 calls
      as system calls or a libc), argv, files, threads; and the PIR path in
      compiler/elf64.fd and linker.fd is not used by it.
      Cross targets through C (Nov 6–7, F5/F8/F9): `fid ccompile --target
      <arch>-<os>` cross-compiles with `zig cc -target` from any host:
      `aarch64-linux`, `riscv64-linux`, `x86_64-linux` (static musl),
      `x86_64-macos`, `aarch64-macos`, `wasm32-wasi`, `x86_64-windows`.
      `@cfg` follows the target (`g_cfg_os`/`g_cfg_arch` in compiler.fd).
      `scripts/cross-test.sh <target>` builds the suite for a target, runs it
      (qemu-user in WSL or on Linux, Node for WASI) and compares output and
      exit code with `fid run`. The C runtime's green tasks switch stacks
      with their own assembly on x86-64, AArch64 and RISC-V (musl has no
      ucontext, macOS deprecates it); `p_co_pool_call` exists in the C
      runtime (it did not: every program using `task_pool_call` failed to
      build through C, on Windows too). `fid` on a Mac is no longer taken for
      Windows (`host_os()` in main.fd).
- [~] F5. macOS x64 and ARM64 through C, in CI (M).
      Nov 7: `fid ccompile --target x86_64-macos|aarch64-macos` builds Mach-O
      executables (checked on a sample program; the suite was not built for
      macOS), and the compiler's own stage-1 C builds for aarch64-macos with
      the flags scripts/bootstrap.sh now gives Darwin. Not run: there is no
      Mac here. `scripts/ci/cross.yml` has the macOS job (bootstrap with
      clang, then the suite through C, compared with the interpreter); this
      repository keeps no `.github/`, so it is not active. Left: run that job.
- [~] F6. Native ARM64/RISC-V/WASM backends: overflow and divide-by-zero
      semantics match the other backends (M).
      Nov 7: through C every target shares the C runtime's checks.
      `test_int_semantics` (wrapping, signed `/` and `%`, shifts, casts, the
      edges of i64) and the run-time panic tests `test_panic_overflow_fail`,
      `test_panic_div_zero_fail`, `test_panic_min_div_fail` give the same
      output, exit code and panic message on the interpreter, C and native
      Windows; cross-test.sh compares them on every cross target (not yet run
      there: the runs were stopped). Native AArch64 and RISC-V (F8, Nov 7):
      the four tests pass under QEMU (output, exit code, panic message).
      Left: a native WASM backend (F9).
- [ ] F7. Native macOS: Mach-O executables, not only objects (L).
      macOS executables come from the C path (F5) for now.
- [~] F8. ARM64 and RISC-V: replace the runtime stubs (19 and 18 zero stubs, 46
      capability stubs), and test under QEMU (XL).
      Nov 7, through C: AArch64 Linux ran 270 of 286 suite tests the same as
      `fid run` under qemu-aarch64 (WSL) before the fixes above; of the 16,
      6 failed to build for want of `p_co_pool_call` (now in the C runtime),
      2 need SDL2, 1 (`test_args`) trapped in zig's -O0 UB sanitizer on an
      unaligned string-header read (zig builds now pass
      `-fno-sanitize=undefined`), and 6 fail the same way through C on the
      Windows host (in-progress work in the tree, not cross). RISC-V Linux
      and x86-64 Linux run the sample programs correctly; their full suites
      have not completed (stopped with the machine out of memory).
      Native, Nov 7: a new backend, `fid emit <file> --target
      aarch64-linux|riscv64-linux -o out` (also `--native --target
      arm64|riscv64`): IR → machine code with pe_ir's semantics, written
      once over a small architecture layer (compiler/nat_ir.fd), its own
      A64 and RV64IMFD encoders and a static ELF writer
      (compiler/nat_enc.fd), no assembler, linker or C library. The runtime
      is Fides over Linux system calls (stdlib/linux_runtime/base.fd,
      merged as stdlib/pe_runtime is on Windows): pe_ir's allocator blocks
      and panics, printing, strings, the float text of the other backends,
      files, clocks, args, environment; Vec and HashMap are core's. Only
      what `main` reaches is compiled. Under qemu-aarch64 and qemu-riscv64
      alike: 225 of the 231 suite tests that build give the interpreter's
      output and exit code (5 of the 6 failures fail through C too); 76 do
      not build yet,
      each refused with the builtins it lacks: threads, tasks, channels,
      atomics, the C math functions (sqrt, floor, pow...), tensors, sockets,
      some file builtins. `test_int_semantics` and the three run-time panic
      tests pass (F6). The old PIR path (codegen_arm64/riscv.fd,
      runtime_arm64/riscv.fd and their stubs) is no longer used by `fid`.
      Nov 7–9: the missing builtins are in: threads through clone (a guard
      region under each stack, freed at the join), M:N green tasks on worker
      threads with an own context switch, futex waits for mutexes and
      channels, sockets, processes, qsort callbacks, musl's exp/log/pow and
      fdlibm's sin/cos, JSON/HTTP/DB/tensors shared with the Windows runtime
      (stdlib/linux_runtime). Faults and stack overflow are panics with the
      C path's trace (RT15). The backend records a machine IR and allocates
      registers once for AArch64, RISC-V and now x86-64 Linux (own-backend
      roadmap, docs/roadmap/llvm-roadmap.md), with an IR verifier and an
      allocation checker (FID_VERIFY=1), iropt plus rules and an e-graph on
      this path. Suite, Nov 9, with every check on: AArch64 273 passed,
      RISC-V 271, x86-64 272 (of 317; 7–9 do not build: two lowering bugs
      found by the verifier, three front-end errors, x86-64-only `@cfg`,
      bare-metal and SDL tests). Left: the native mismatches older than this
      work (`test_args`, `test_free_escape`, `test_sensitive` / integer-key
      HashMap through an i64 handle, `test_str_return3`), and a rerun
      against a current interpreter (many references failed in the 19:03
      snapshot's).
      Nov 9: Windows x64 on the same layer, opt-in (`fid emit --nat`): a PE
      written by nat_enc.fd, the Linux runtime over kernel32 / ws2_32
      (stdlib/linux_runtime/win.fd maps each system call); 301 of 338 suite
      tests match. Processes wait for a `nat_os()` hook in `pe_sh`. Line
      tables (`.debug_line`) in every native Linux executable.
- [~] F9. WASM: globals, struct copies, collections (L).
      Nov 7, through C: `fid ccompile --target wasm32-wasi -o x.wasm` (zig cc,
      WASI preview 1) and `node scripts/wasm-run.mjs x.wasm` run the sample
      programs with globals, structs, `Vec`, strings and floats. Under WASI,
      tasks run to their end when entered (no stack switching), and
      processes, signals and socket creation fail. The suite through WASI has
      not completed. The native backend (`fid wasm`, compiler/wasm.fd) is
      unchanged: it still writes a module that fails validation (an i64
      data-segment offset in 32-bit memory) and has no collections.

**Acceptance:** Windows, Linux and macOS are fully supported at 1.0.0, on x64 and
ARM64 where the system runs there; RISC-V and WASM pass the suite.

## Track G — Bare metal

- [ ] G1. Bare-metal tests run under QEMU in CI, not hosted, where they are
      expected to fault (M).
- [x] G2. `fid emit --bare` works: `pe_ir` panics with "bare mode not ported" (M).
      Done Nov 6: `--bare` goes through pe_ir (the PIR path crashed): a flat
      binary loaded at 0x10000, entered at its first byte. The hardware
      stubs are ported from src/pe.rs (three of its bugs fixed: the IDT gate
      flags, the GDT fifth argument, a stray byte in p_page_map); p_malloc is
      a bump allocator from 0x300000; print goes to COM1; `asm!` templates
      are encoded (Intel syntax: system, port, control-register, ALU, shift,
      push/pop, jmp/call through a register), an unknown one is a build error.
      Run on Linux with scripts/bare-run.c, which plays COM1 and `hlt`
      (test_bare_print, test_kernel_serial, test_serial_hello); paging and
      descriptor tables need QEMU (G1), not available here.
- [x] G3. Bare builds define their integer semantics: panics need a handler, so
      overflow and division are unchecked today (M).
      Done Nov 6: bare builds check overflow and division like every other
      backend; a failed check, like any panic, calls the program's
      `@panic_handler` with the message, else prints `panic: <message>` to
      COM1, then halts. `@wrapping` wraps (test_bare_panic_handler).
- [ ] G4. `fides-os` builds with the released compiler, in its own CI (M).

**Acceptance:** a kernel boots and prints under QEMU, from a clean checkout.

## Track H — Concurrency

- [x] H1. `spawn` really runs concurrently on the Windows C path: it runs
      synchronously there today, and differs from native (M).
- [x] H2. Mutexes on the Windows C path are real (S).
- [x] H3. Data races rejected. Done: code reached from `spawn` or
      `p_thread_spawn` may not write a global of the program without a lock. Left:
      shared data reached through pointers and captures (L).
      Nov 4–6: raw pointers, references and structs holding them are not `@send`,
      so they cannot reach another task (test_race_ptr_fail). Left: a `Vec`
      passed to a task is still the same handle until copy-on-write is on (M5).
      Done Nov 6: with copy-on-write on (M5) a Vec passed to a task is the
      task's own copy: two tasks changing theirs and the caller's stay apart
      (test_spawn_vec_copy). Shared data between tasks goes through locks,
      channels or atomics, which are @send/@sync.
- [x] H4. Enforce `@send`/`@sync` for every spawn argument (M).
      Done Nov 4: every argument of a spawned call must be `@send`; a struct of
      `@send` fields is `@send` (test_spawn_send_fail).
- [x] H5. Real `async`: an `async fn` is a task that runs until it first waits;
      `await`, `task_sleep` and socket waits suspend it (L, see decision 4).
      Float and struct arguments and results run as tasks too since Nov 4
      (H9); an `async fn` with a reference argument still runs to its end.
- [x] H6. Green threads outside the interpreter: fibers on Windows, `ucontext` on
      Linux, 10,000 tasks on one thread. The interpreter runs a task to its end.
- [x] H7. `parallel { }` runs multi-statement branches concurrently (M).
      Done Nov 4: `x = f(..)` branches are spawned and joined; a multi-statement
      branch that reads only word-sized locals runs as its own function on a
      thread (test_parallel_branches, all backends). Other branches run in place.
- [x] H8. Allocator: per-thread caches instead of one global lock (M).
      Done Nov 4: the C runtime has per-thread arenas and free lists; the native
      runtime now keeps a free list per thread in a TLS slot (only chunk growth
      takes the lock). No multi-thread stress test run.
- [~] H9. An `async fn` with float, struct or reference arguments or results is a
      task too: today it runs to its end at the call (M).
      Done Nov 4 for the interpreter and C: an `async fn` with f64 or struct
      arguments or results runs as a task through a word adapter
      (test_async_float_struct). Natively built programs crash, as all native
      async does (H10). References are still not task arguments.
      Nov 6: natively built async programs no longer crash (a fix of the
      native-async session in pe_ir's named_fns): test_async_float_struct and
      the other async tests agree with the interpreter under `fid emit`.
      Left: reference arguments. A reference cannot reach another task
      (second-class references, Session 29), so such an `async fn` still runs
      to its end at the call; making it a task needs a rule that the future
      is awaited before the referent goes away.
- [x] H10. Tasks on several threads natively: the C runtime keeps them per thread,
      the native one has a single set (M).
      Done: the native runtime is M:N with work stealing, one worker per core
      (stdlib/pe_runtime/conc.fd); the Nov 6 crash is fixed. test_tasks_10k:
      10,000 sleeping tasks at once run on 21 threads (16 workers on a
      16-core machine) in 0.45 s.

**Acceptance:** a race test is rejected; 10,000 concurrent tasks run on a few
threads.

## Track I — Networking

Depends on B, C, H5 and J.

- [x] I1. TCP in the native runtime: `p_socket_*` over Winsock (M).
- [x] I2. Non-blocking I/O tied to tasks: `p_co_recv`, `p_co_send`, `p_co_accept`
      (L).
- [x] I3. TLS client and server: `use tls`, Schannel on Windows, OpenSSL on Linux
      (L, see decision 2). TLS 1.3 needs the newer Schannel credentials: today
      Windows negotiates 1.2.
- [x] I4. HTTP/1.1 client and server: `http` and `https`, tested with curl, keep-
      alive, chunked bodies, pipelining, 28,000 requests/s on one thread (L).
      Bodies are text: a zero byte ends them. `http_serve_scoped` keeps memory
      flat, on the promise that a handler keeps no request string.
- [x] I5. `examples/http_server` runs on a native build (S).
- [ ] I6. A sample service survives a 24-hour load test (M). A short run:
      160,000 requests stay at 5.2 MB with `http_serve_scoped`.
- [ ] I7. TLS 1.3 on Windows: Schannel needs its newer credentials structure
      (SCH_CRED + TLS_PARAMETERS); the SP_PROT_TLS1_3 bit in SCHANNEL_CRED
      fails the handshake (verified — `tls_session_protocol` reads the
      negotiated version, 1.2 today) (S).
- [ ] I8. Binary HTTP bodies: a body is a `str`, so a zero byte ends it (M).
- [ ] I9. `net`: `net_reliable_recv` never recognises an ACK, and the ACK it builds
      is never sent (S).

**Acceptance:** an HTTPS service runs for 24 hours under load, with flat memory.

## Track J — Cryptography

Nothing here is ready to protect real data yet. Do not write new primitives.

- [x] J1. `crypto` tests fail on a mismatch: `real_crypto.fd` prints `MISMATCH`
      and still passes (S). It panics now.
- [x] J2. SHA-256 and HMAC checked against the NIST test vectors (S):
      `test_crypto_vectors.fd`, FIPS 180-4 and RFC 4231; a mismatch panics.
- [x] J3. `crypto.fd`'s header claims SHA-512 and a CSPRNG that do not exist:
      remove the claims or add them (S). The header now lists what exists.
- [x] J4. A CSPRNG from the OS (`BCryptGenRandom`, `getrandom`) (S):
      `crypto_random_bytes`, `crypto_random_u64` over the builtin
      `p_os_random_u64` (rand_s / BCryptGenRandom on Windows, /dev/urandom
      elsewhere), on every backend.
- [ ] J5. AEAD, signatures and a KDF through bindings (L, see decision 2).
- [ ] J6. Apply `@constant_time`, `@secret` and `@zeroize` in `stdlib/crypto.fd`
      itself: it uses none of them today (M).
- [ ] J7. `@constant_time` verified on the PE path: its asm verifier covers x86-64
      ELF only (L).
- [ ] J8. `@zeroize` reaches old `Vec` buffers and cannot be optimised away (M).

**Acceptance:** vectors pass in CI; the documented claims match the tests.

## Track K — Language soundness

- [x] K0. `val`/`const` reassignment, non-exhaustive `match`, wrong `Vec` element
      types and `str`/`i64` mixing in a program are rejected; safety rules follow
      method calls.
- [x] K1. Raw pointer dereference outside `unsafe` is an error: a null `*p`
      compiles and only panics at runtime (M).
- [x] K2. Literal range checks: `var x: u8 = 300` is rejected (S).
- [x] K3. Silent narrowing rejected: an `i64` assigned to an `i32` truncates (M).
- [x] K4. Iterator invalidation: `for x in v { v.push(1); }` is rejected (M).
      Done Nov 4: changing a `Vec` while a `for` loop walks it is rejected in
      `fid` (test_iter_invalidation_fail).
- [x] K5. Borrow-checker gaps: `s.x = 1`, `&v[i]`, trait impl bodies (M).
      Done Nov 4 in `fid`: field assignments checked against borrows, `&v[i]`
      borrows `v`, trait impl bodies checked (test_field_borrow_fail).
- [x] K6. `&(rvalue)` no longer points at a dead temporary (M).
      Done Nov 4: the native backend never recycles an alloca's slot, so
      `&(rvalue)` keeps pointing at live memory.
- [x] K7. `@deterministic`, `@crashfree` and `sandbox` cover effects hidden in
      method bodies: extend the effect table with `p_file_*`, `p_socket_*`,
      `p_system`, `p_clock_now` (M).
      Done Nov 4: file, directory, process, clock, environment and random runtime
      calls are IO; sockets, TLS, DNS and HTTP are Net.
- [x] K8. `@no-indirect`, `@bounded-stack`, `@tainted` either work or are removed
      (M). They parse but do nothing useful.
      Done: `@bounded-stack` is checked in both compilers (no recursion, no call
      through a function value in anything it reaches). Left: `@tainted`.
      Done Nov 4: `@no-indirect` rejects calls through function parameters;
      `@tainted` on a parameter taints the function (test_safety_indirect_fail,
      test_tainted_param_fail).
- [x] K9. Generic bounds are checked; an unbound `T` is an error, not `i32` (M).
      Done Nov 3–4 in `fid` (proposal 0002): bounds checked, an uninferable type
      parameter is an error (test_generic_bound_fail, test_generic_infer_fail).
- [~] K10. `@cfg` and `@simd` work on `fid` as on `fidesc` (M).
      Done Nov 4 for `@cfg` in `fid` (platform/os, arch, debug/release; test_cfg
      on all backends). `@simd` not compared.
- [x] K11. A language reference that matches both compilers, with the differences
      closed (M).
      Done Nov 4: docs/reference/reference.md, written from the compiler source,
      each rule citing the function that enforces it; differences from `fidesc`
      in language-guide.md § 10.
- [x] K12. Semver and a stability promise for 1.0 (S): `docs/reference/stability.md`.

**Acceptance:** every line in `docs/reference/features.md` has a test proving it,
and a test proving what it rejects.

## Track L — Safety-critical

Fides must not be described as suitable for medical, automotive or avionics use
until this track and L5 (set aside) are complete.

- [x] L1. Every safety annotation enforced, with a negative test per rule (L).
      Done Nov 4: @no-alloc sees method calls and string `+`, @firmware sees
      runtime calls, @gas counts literal loop trips; a negative test per rule
      (docs/reference/safety-rules.md, 17 rules).
- [x] L2. Remaining stubs in the safety-annotation checks closed (M).
      Done Nov 4: the stubs found (methods unchecked, function-parameter calls,
      runtime calls skipped before @firmware, @tainted on parameters) are closed.
- [x] L3. `qualify` reports complete evidence (L).
      Done Nov 4: `fid qualify <file.fd>` prints JSON evidence (hashes, check
      result, annotated functions, each rule with its tests and whether they
      exist). Always "NOT_QUALIFIED".
- [x] L4. Requirements traceability from each rule to its tests (L).
      Done Nov 4: docs/reference/safety-rules.md traces each rule to its
      requirement and its negative and positive tests.

**Acceptance:** every rule has its negative test and traces to its requirement.
This is our evidence, not a certificate: see "Set aside".

## Track M — Memory safety

The goal: **a program with no `unsafe` block cannot corrupt memory.** A memory
bug is rejected at compile time or stops with a panic. It never reads freed
memory or silently damages other data.

Track C makes this more urgent: every compiler-inserted free is a new way to
free too early. M3 and M6 must land before C1 and C2.

Tested on Nov 1, native build:

| Error | What happens today | Fixed by |
|---|---|---|
| `Vec` index out of bounds | Panics | Done |
| Wild or null pointer read | Rejected outside `unsafe`; inside, panics only if unmapped | K1, M2 |
| Double free | Rejected outside `unsafe`; inside, panics, unless the block has been reused | M2, M5, M7 |
| Free of a pointer that is not a block | Panics, on the interpreter, C and native paths | Done (M3) |
| Use after free | Needs `unsafe` to read, but nothing detects it: it read 0 | M7, M8 |
| Two names for one `Vec` | `var b = a` shares it, with no move check | M4, M5 |
| `push` during `for` over the same `Vec` | Rejected in `fid` (Nov 4) | Done (K4) |

- [x] M1. Write down the guarantee in the language guide: what safe code
      promises, and exactly what `unsafe` gives up (S).
- [~] M2. `p_malloc`, `p_free`, integer-to-pointer casts and `extern "C"` calls
      need `unsafe`, or `@unsafe` on the function. Done in the program itself;
      imported modules wait for M10 (M).
- [x] M3. The allocator marks freed blocks in their header and panics on a double
      free or a pointer it never handed out. A second free after the block was
      reused cannot be told from a valid one, and frees the new owner's block:
      M5 and M7 close that. Covers the interpreter, C and native Windows paths;
      the native ELF, ARM64 and RISC-V runtimes have their own p_free, unchanged.
- [x] M4. Decide what `var b = a` means for `Vec`, `str` and `HashMap`: a move, or
      a shared counted handle (S, see decision 3).
      Decided Nov 2: copy-on-write for `Vec`, `str` and `HashMap`; `==` by
      content.
- [x] M5. Enforce it: use after move is rejected, or shared handles are counted
      (L).
      Done Nov 6: copy-on-write is on (LOWER_COW, proposal 0001): a Vec or
      HashMap copied, passed or stored is a lazy, counted share; the first
      write copies, the last owner frees. The suite gives the same output on
      and off (364 programs through `fid emit`); the compiler reaches its
      fixpoint with it on (93 s, 4.7 GB peak, 1.8 GB off). Fixed on the way:
      the last use of a by-value parameter was handed over as a move while
      callers still pass their own handle (LOWER_ARG_SHARE off), so a callee's
      copy changed the caller's Vec (test_spawn_vec_copy).
- [x] M6. Every compiler-inserted free has tests for each way a value can escape:
      assignment, field, return, closure, task, container, `unsafe` (M).
- [~] M7. A checked allocator mode that keeps and poisons freed blocks, so use
      after free panics instead of reading stale data (M).
      Done: `fid run --checked-alloc` (C path): freed blocks' pages become
      inaccessible and are never reused, blocks end at an inaccessible page;
      the access faults with a trace. `--leaks` reports blocks never freed.
      Left: the PE path.
      Nov 4: `fid emit --checked-alloc` gives every native block pages of its
      own, ending at a guard page, and makes freed pages inaccessible. Written
      and built, not yet run on the suite.
- [~] M8. The suite runs under AddressSanitizer through the C path, in CI (M).
      Written Nov 4: scripts/sanitize.sh builds each test through C with
      ASan/UBSan (`FIDES_CFLAGS`) and compares with `fid run`; under ASan the C
      runtime takes every block from calloc. Needs clang with ASan (Linux/macOS);
      not run yet, not in CI.
- [~] M9. Fuzzing: random programs must agree on the interpreter, C and native
      paths and stay clean under M7 and M8, run nightly (L).
      Written Nov 4: scripts/fuzz/nightly.sh runs the generator on the
      interpreter, C, native and the checked allocator, and a sample under the
      sanitizers on Linux. Not scheduled: no CI workflow in the repo.
- [~] M10. Audit every `unsafe` block in the stdlib: each states why it is sound,
      and no safe stdlib function can be misused into a memory error (L).
      Done: `fid audit <file.fd>` lists the `unsafe` code a program reaches with
      its `// SAFETY:` comment. Left: those comments in the stdlib (core alone
      has 174 blocks without one).
- [x] M11. An owned and a borrowed string type, so a server frees its request
      strings without a promise: `http_serve_scoped` is `@unsafe` until then (L,
      after M4).
      Done Nov 4: `&str` is the borrowed string, `str` the owned one; a `str`
      field of a `@scoped` struct reads as `&str`; a borrowed string cannot be
      stored, returned as `str` or passed to a user function's `str` parameter
      (`str_clone` copies). `http_serve_scoped` hands the handler a
      `ScopedRequest` and is no longer `@unsafe` (test_borrowed_str and three
      expected-fail tests).

K1, K4, K5, K6, C5, C6 and H3 count towards this goal.

M3 found a real leak on its first run: shared slots on the C path came from plain
`malloc`, so `p_free` had ignored every one of them. Any other pointer the
runtime hands out from `malloc` will now panic if freed, rather than leak.

**Acceptance:** every row of the table is rejected or panics, with a test;
sanitizer runs and a week of fuzzing find no memory error in programs without
`unsafe`. Until then the docs must not call Fides memory-safe.

## Track N — Tooling and packages

Some tools exist, but a new user cannot yet get from install to a working
project with packages and editor help. Checked Nov 1:

| Tool | Works today | Missing |
|---|---|---|
| `ark` | `init`, `add`, `install`, `list`, `lock` for path and git packages | No registry: `search` and `publish` only print "not yet implemented" |
| LSP | Completion, go to definition, hover, rename, diagnostics | References, formatting, signature help; not tested in CI |
| VS Code extension | Syntax highlighting | Does not use the LSP; not on the Marketplace |
| `fmt`, `lint`, `test`, `bench`, `profile` | On both compilers | Not run in CI |
| `ark`, `scribe`, `veil` | Rust binaries | No self-hosted version |
| Debugging | `debug` and `replay`; `autopsy` on Linux; `emit -g` adds DWARF line tables to the PE (Rust bootstrap only) | DWARF emission in the self-hosted `fid`; variable DIEs |

- [ ] N1. CI runs `ark init`, `add`, `install` and a build from a clean machine,
      on Windows and Linux (S).
- [ ] N2. A getting-started guide, followed step by step in CI (M).
- [~] N3. The VS Code extension starts the LSP, and is on the Marketplace (M).
      Done: it runs inside VS Code, with diagnostics on their real line, hover, go
      to definition, completion and rename. Left: the Marketplace.
- [ ] N4. `fmt --check` passes on the stdlib and the compiler in CI; formatting
      keeps every comment and never changes behaviour (M).
- [ ] N5. `lint` runs in CI and is clean on the stdlib (S).
- [~] N6. Package registry: names, versions, checksums and yanking, with
      `ark search` and `ark publish` working (L, see decision 6). Done: a Git index,
      `search`, `publish` and `install`, which checks the commit, tested end to end
      on a local index. Left: the public index, yanking, committing it.
- [ ] N7. The LSP gains references, formatting and signature help, with tests (M).
- [ ] N8. `ark`, `scribe` and `veil` are ported to Fides or shipped by the same
      release process as `fid` (L).
- [~] N9. Source-level debugging of native Windows builds: line tables a standard
      debugger can read (L).
      Done: `fidesc emit -g` writes DWARF v4 sections (`.debug_line` etc.) into
      the PE with per-instruction line tables; `ccompile -g` annotates generated
      C with `#line` to `.fd` sources. Left: same emission in the self-hosted
      `fid` (compiler/*.fd), variable DIEs, verify with gdb/lldb/VS debugger.
- [ ] N10. `docs/stdlib/` is generated by `scribe` and published with each release
      (M).
- [ ] N11. Five example projects built in CI: command-line tool, library, service,
      game, C binding (M).
- [ ] N12. Install and update on a clean machine, tested in CI; `fid --version`
      names the exact build (M).
- [ ] N13. Contributor basics: contribution guide, issue templates, release notes,
      a security contact (S).

**Acceptance:** on a clean Windows or Linux machine, a new user follows only the
guide to install Fides, create a project, add a registry package, get completion
and go to definition in VS Code, then format, test and build it.

## Order of work

1. **A, B:** quick, and every other track relies on them.
2. **M1–M3, M6, then C1–C4, D1–D3, E1–E3:** the cheap wins with the most visible
   effect, with the safety checks first so new frees cannot go unnoticed.
3. **F1–F3, H1–H2, K1–K3, N1–N5:** the rest of the beta gate. **→ Public beta**
4. **D4, H3, J, M4–M5, N6:** the large projects, after decisions 1–4 and 6.
5. **K, remaining F4–F6, M and N, docs.**
6. **G, F7–F9, J7–J8, L1–L4:** bare metal, the other platforms, and the safety
   evidence. **→ Stable 1.0.0**

## Set aside

These need an outside party or money, so they wait, and they do not block 1.0.0.

- [ ] J9. Independent security review before anything is called production
      ready (external).
- [ ] L5. A qualified toolchain process per IEC 62304, ISO 26262 or DO-178C,
      through a certification body (XL, external).

## Notes

- Library policy (decided Nov 1): the standard library is pure Fides wherever it
  can be. System libraries serve what only the OS provides (sockets, secure
  random numbers, Schannel). TLS and cryptography bind reviewed libraries. SQLite
  and SDL2 stay optional bindings.
- Keep `src/` and `compiler/` in step: a change to one is not done until the
  other matches and both pass.
- Do not claim a feature in `README.md` or `positioning.md` until its track says
  `[x]`. Several current claims are ahead of the code.
- Related roadmaps: `inc-roadmap.md`, `pe-roadmap.md`, `llvm-roadmap.md`,
  `security-roadmap.md`, `safety-critical.md`.
