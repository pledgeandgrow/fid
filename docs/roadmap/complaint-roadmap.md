# Complaint Roadmap

> Goal: fix what developers complain about in the languages they use today.
> Each goal starts from a real, recurring complaint, names the languages it is
> made about, and says how Fides answers it.
>
> Created: Nov 2, 2026. Update it in the same commit as the work.
> `[x]` done and verified, `[~]` partly done, `[ ]` not started. Sizes are effort,
> not dates: S = hours, M = days, L = weeks, XL = months.

This sits beside [release-roadmap.md](release-roadmap.md) (what 1.0.0 promises)
and [competitive-roadmap.md](competitive-roadmap.md) (catching up with mature
languages). Those roadmaps ask what Fides lacks; this one asks what hurts
people elsewhere, and whether Fides can make it stop hurting. Goals that already
exist in the other two are named here, not repeated.

A complaint counts when it is common and lasting: it shows up year after year in
surveys (the Rust, Go, Stack Overflow and JetBrains developer surveys), issue
trackers and post-mortems, not as one person's taste.

## Priorities

The ten that matter most, in order:

1. CB1 compile speed: otherwise Fides inherits Rust's and C++'s worst complaint.
2. CT1 tests without mocks: no mainstream language can do it; effects make it possible.
3. CA1 no coloured functions, finished.
4. CD1 per-dependency capabilities against supply-chain attacks.
5. CO1 secrets that cannot reach logs, adopted across the stdlib.
6. CE1 error context without boilerplate.
7. CL1 `@derive(Deserialize)`.
8. CA4 deadlocks found at compile time.
9. CP1 debug builds that are not unusably slow.
10. CU1 `fid fix`: upgrades that migrate the code.

## Track CB — Building and compiling

- [ ] CB1. **"Compiling takes forever"** (Rust, C++, Scala, Swift). Seconds for a
      large project, not minutes: fast type checking, a cached stdlib, parallel
      and incremental builds (competitive CS1–CS9) (XL).
- [~] CB2. **"I wait for the whole build to see one type error"** (Rust, C++).
      `fid check` type-checks with no code generation. Left: under 50 ms after an
      edit in a large program (M).
- [ ] CB3. **"Incremental builds are not incremental"** (C++, Rust). A one-line
      change rebuilds one function, not the crate (competitive CS6) (L).
- [~] CB4. **"CMake, Makefiles, build.rs: a second language just to build"** (C,
      C++, Rust). One command, `fid build`, no build scripts; native Windows
      executables need no linker. Left: the same on Linux and macOS (L).
- [ ] CB5. **"Cross-compiling needs a toolchain per target"** (C, C++, Rust).
      `fid build --target linux-arm64` from any host, nothing to install
      (competitive TL6) (L).
- [~] CB6. **"The build is not reproducible"** (C, C++, npm). The same source and
      compiler give the same bytes. Done for the compiler itself (the bootstrap
      fixpoint). Left: guaranteed and tested for user programs (M).
- [x] CB7. **"Debug or release: which flags?"** (C, C++). `--debug` and `--release`,
      nothing else to know.
- [ ] CB8. **"The build cache serves stale results"** (Bazel, Gradle, ccache).
      Content hashes of everything an artifact depends on, tested against changes in
      each input. (A stale hit was found and fixed on Nov 2.) (M)
- [ ] CB9. **"Linker errors are unreadable"** (C, C++, Rust). The linker is part of
      the compiler: a missing symbol is reported as a Fides error with its source
      line and the library that should have provided it (M).

## Track CM — Memory and safety

- [x] CM1. **"Lifetime annotations are unreadable"** (Rust). References cannot be
      stored, returned or captured by escaping closures, so a lifetime is never
      written.
- [~] CM2. **"I fight the borrow checker for code that is obviously fine"**
      (Rust). Each rejection explains which use conflicts and suggests the change
      that fixes it; patterns people hit most (two-phase borrows, disjoint fields)
      accepted (release K5) (L).
      Done: every borrow error names the borrow that holds the value (who, `&` or
      `&mut`, which path, the line it was made on) and a `Fix:` line, in both
      compilers. Left: the gaps of release K5.
- [ ] CM3. **"Is this a move or a copy?"** (Rust, C++). One rule for `Vec`, `str`
      and `HashMap`, checked by the compiler (release M4, M5) (L).
- [x] CM4. **"Integer overflow is undefined, or silently wraps"** (C, C++, Go,
      Rust in release). Every integer operation means the same on every backend,
      and overflow panics in every build unless `@wrapping` says otherwise.
- [x] CM5. **"Undefined behaviour in code that looks fine"** (C, C++, Zig). Safe
      code has no undefined behaviour; the guarantee is written down (release M1).
- [x] CM6. **"Null pointer crashes"** (Go, Java, C, C#). `Option` instead of null.
      No stdlib function returns a null `str`: the wrappers of the runtime calls
      that can give one (`p_file_read`, `p_getenv`, `p_argv`) return `""`, `[]`
      or a `Result` (`file_read_result`). Their null tests were wrong in `fid`
      (`s == 0` on a `str` compared text) and are now `(s as i64) == 0` (M).
- [~] CM7. **"Garbage collection pauses"** (Go, Java, C#). No collector; the
      compiler frees what does not escape. Left: `Vec<str>` elements, real regions
      (release C5, C6) (L).
- [~] CM8. **"Memory leaks I cannot find"** (C, C++, Go). `fid run --leaks` reports
      every block never freed, with the line that allocated it (M).
      Done: `fid run --leaks` (and `fid ccompile --leaks`) reports at exit the
      blocks never freed, grouped by the functions that allocated them
      (`make_node <- build <- main`). Left: line numbers; `fidesc`.
- [~] CM9. **"Use after free only shows up in production"** (C, C++). A checked
      allocator mode poisons and keeps freed blocks (release M7) (M).
      Done: `fid run --checked-alloc`: each block on its own pages ending at an
      inaccessible page; a freed block's pages become inaccessible and are never
      reused, so a use after free or a write past the end faults at that access,
      with the trace. Left: the native (PE) path; naming the block in the
      report.
- [~] CM10. **"`unsafe` hides everywhere in my dependencies"** (Rust, Go, C#).
      `fid audit` lists every `unsafe` block reachable from a program, with its
      written justification (release M10) (M).
      Done: `fid audit <file.fd>` lists each `unsafe` block, `@unsafe` function
      and `extern "C"` in the program and every module it uses, with its
      `// SAFETY:` comment; the stdlib is summed per module (`--stdlib` lists
      it); `--strict` fails when one outside the stdlib has none. Left: SAFETY
      comments on the stdlib's own blocks (release M10).

## Track CE — Errors and diagnostics

- [x] CE1. **"`if err != nil` on every other line"** (Go). `?` propagates; adding
      context (`.context("reading config")`) costs nothing when nothing fails
      (competitive LG10) (M).
      `Result.context(msg)`, `Result.with_context(f)` and `Option.context(msg)`:
      an `Ok` goes through as it is, the message is built only on `Err`. `?` in a
      function returning a `Result` by struct return lost the error in `fid`
      (fixed).
- [x] CE2. **"`unwrap()` panics in production"** (Rust). `fid lint` flags every
      `unwrap` outside tests; `@crashfree` forbids it (S).
      `fid lint` warns at each function that calls `unwrap()` or `expect()`,
      except `fn test_*`, `fn bench_*` and `*_test.fd` files.
- [ ] CE3. **"Exceptions I did not know a function could throw"** (Java, C#,
      Python, JS). A function's failure modes are in its type; there are no hidden
      exceptions (M).
- [~] CE4. **"Compiler errors I cannot read"** (C++ templates, Rust generics, Scala
      implicits). Every error has a code, the source line, and a `Fix:` line.
      Left: a test for each error message (competitive TL2) (L).
- [ ] CE5. **"The compiler knows the fix but makes me type it"** (most).
      `fid fix` applies suggested fixes (competitive TL3) (M).
- [~] CE6. **"Cascades: one mistake, two hundred errors"** (C++, Java). After a
      first error, dependent errors are suppressed (M).
      Done in `fid`: an undefined name is reported once per function, and an
      operand whose error is reported adds no arithmetic error. Left: the same
      in `fidesc`, more dependent errors.
- [x] CE7. **"No stack trace in release builds"** (C, C++, Rust without
      `RUST_BACKTRACE`). Panic traces in every build, at no cost per call.
- [ ] CE8. **"A crash report without the values that caused it"** (most). A panic
      prints the arguments of the failing call and the locals of its frame, when
      they are words (M).
- [ ] CE9. **"Error messages that blame the library, not my call"** (C++, Rust
      generics). An error inside a generic function is reported at the call site
      that chose the types (M).

## Track CA — Async and concurrency

- [~] CA1. **"Function colouring: async infects everything"** (Rust, JS, Python,
      C#). Any function runs inside a task and suspends where it waits; callers
      do not change. Left: async with float, struct or reference arguments
      (release H9) (L).
- [ ] CA2. **"Which async runtime: tokio or async-std?"** (Rust). One runtime,
      in the standard library, used by every library (M).
- [ ] CA3. **"`Send` and `Sync` errors I do not understand"** (Rust). Spawning
      checks what crosses threads and says in one sentence what cannot and why
      (release H4) (M).
- [ ] CA4. **"Deadlocks in production"** (Go, C++, Java). Lock order checked across
      the call graph at compile time; a cycle is an error (L).
- [~] CA5. **"Data races"** (Go, C++, Java). Code reached from `spawn` cannot share
      mutable state unchecked (release H3) (L).
- [ ] CA6. **"Goroutine and task leaks"** (Go, JS). Structured concurrency: a task
      cannot outlive the scope that started it unless detached by name
      (competitive LG11) (L).
- [ ] CA7. **"Cancellation is an afterthought"** (Go contexts, JS AbortController).
      Every task can be cancelled, and every wait respects it (L).
- [ ] CA8. **"Timeouts are bolted on"** (most). `timeout(ms) { ... }` around any
      block that waits (M).
- [ ] CA9. **"Async stack traces are useless"** (Rust, JS, C#). A panic in a task
      shows the chain of tasks that awaited it (M).
- [ ] CA10. **"Blocking call in async code freezes everything"** (Rust, JS,
      Python). The effect system knows which calls block; a blocking call inside a
      task is an error or runs on a worker thread (M).

## Track CT — Testing and debugging

- [ ] CT1. **"Mocking time, network and files is painful and flaky"** (all). A test
      replaces the clock, network or file system for the code under test, because
      effects say exactly what that code touches. No mocking framework (L).
- [~] CT2. **"Tests need a framework to write"** (C, C++, Java). `fid test file.fd`
      runs every `fn test_*()` in its own process and times every `fn bench_*()`.
      Left: table tests with a name per row, filters, parallel runs (M).
- [ ] CT3. **"Flaky tests from time and randomness"** (all). Under `fid test`,
      time and random numbers are fixed per test unless the test asks otherwise (M).
- [ ] CT4. **"Fuzzing needs another tool"** (C, C++, Rust). `fn fuzz_*(data: Vec<u8>)`
      runs under `fid test --fuzz`, as in Go (competitive SL13) (M).
- [ ] CT5. **"Snapshot tests are hand-rolled"** (most). `expect_snapshot(value)`
      writes and checks a stored file, updated with `--update` (S).
- [ ] CT6. **"I can't debug a release build"** (C++, Rust). Line tables and
      variable locations in optimized builds, readable by standard debuggers
      (release N9) (L).
- [ ] CT7. **"The bug only happens in production"** (all). Record the inputs every
      effect brings in; replay the run exactly, in the interpreter, step by step (L).
- [ ] CT8. **"Coverage needs another tool"** (C, C++, Go before 1.20). `fid test
      --coverage` with lines and branches, from `veil` (M).
- [ ] CT9. **"Debugger shows mangled names and optimized-out values"** (C++, Rust).
      Debug info names Fides functions and types as written (M).
- [ ] CT10. **"printf debugging needs a recompile"** (all compiled languages).
      `fid run --trace fn_name` prints each call to a function with its arguments
      and result, no code change (M).

## Track CD — Dependencies and supply chain

- [ ] CD1. **"Any dependency can steal my keys"** (npm, PyPI, Cargo). Each
      dependency is granted capabilities in `ark.toml`; a build where it uses more
      fails. Upgrades show a capability diff (L).
- [~] CD2. **"I need fifty packages for basic things"** (Rust, JS). A standard
      library that covers HTTP, JSON, TLS, regex, time, CSV, compression and more,
      each tested on every backend. Left: depth (competitive SL track) (L).
- [ ] CD3. **"`node_modules` is bigger than my program"** (JS). Dependencies are
      fetched once per machine, shared, and only the functions a program uses end
      up in the binary (M).
- [ ] CD4. **"Diamond dependencies: two versions of the same library"** (Rust,
      Java). One version per library in a build; a conflict is reported with both
      paths and the newest version that satisfies both (M).
- [ ] CD5. **"Left-pad: a deleted package breaks the world"** (npm). Dependencies
      come from Git or paths with content hashes in the lock file, and are
      vendored with one command (S).
- [ ] CD6. **"Typosquatted packages"** (npm, PyPI). `ark add` warns about a name
      one edit away from a known library (S).
- [ ] CD7. **"I don't know what my dependencies do"** (all). `ark audit` prints each
      dependency's capabilities, `unsafe` blocks and C libraries (M).
- [ ] CD8. **"Build scripts run arbitrary code at install time"** (npm postinstall,
      Cargo build.rs, pip setup.py). No install-time code; `comptime` is sandboxed
      with no I/O (M).
- [ ] CD9. **"Vendoring is a pain"** (Go before modules, C++). `ark vendor` copies
      every dependency into the project, and builds work offline (S).

## Track CU — Upgrades and stability

- [ ] CU1. **"Upgrading the language breaks my code"** (Python 2→3, Scala 3,
      Swift). `fid fix --edition` migrates code between editions (competitive
      TL3, ST3) (L).
- [x] CU2. **"No promise about what will keep working"** (young languages). A
      written stability promise and semantic versioning
      (docs/reference/stability.md).
- [ ] CU3. **"Deprecations with no replacement path"** (all). `@deprecated(use =
      "new_fn")` warns with the replacement, and `fid fix` applies it (S).
- [ ] CU4. **"Features stuck on nightly for years"** (Rust). No nightly-only
      features: a feature ships behind a flag in a release, or not at all (S).
- [ ] CU5. **"A minor release broke my build"** (npm semver in practice). `ark`
      checks a library's public API against its last release and refuses a minor
      version that removes or changes anything (M).
- [~] CU6. **"I can't tell what changed"** (all). Release notes list every change
      that can affect a working program under **Breaking** (docs/reference/
      stability.md). Left: generated from the changelog and checked (S).
- [ ] CU7. **"Old tutorials no longer compile"** (fast-moving languages). Examples in
      the docs are compiled and run in CI, so docs and compiler never drift
      (competitive SL19) (M).
- [ ] CU8. **"The compiler upgrade changed my program's speed"** (all). Benchmarks in
      CI fail on a slowdown over 10% (release D7) (M).

## Track CL — Language ergonomics

- [ ] CL1. **"Serialization boilerplate"** (Go, Java, C++). `@derive(Deserialize)`
      beside the existing `@derive(Serialize)` (M).
- [~] CL2. **"No sum types"** (Go, Java before 17). Enums with payloads and `match`.
      Left: exhaustiveness and guards everywhere (competitive LG8) (M).
- [ ] CL3. **"`String`, `&str`, `OsString`, `CString`... which one?"** (Rust). One
      owned and one borrowed string type, UTF-8, and nothing else
      (release M11) (L).
- [ ] CL4. **"No default or named arguments"** (Rust, Go, Java). Default and named
      arguments, checked at compile time (M).
- [ ] CL5. **"Builder-pattern boilerplate"** (Rust, Java). Struct literals with
      defaults: `Config { port: 80, .. }` (M).
- [ ] CL6. **"Macros nobody can read"** (C, Rust). `comptime` functions in plain
      Fides cover what macros are used for (competitive LG9) (L).
- [ ] CL7. **"Orphan rule: I can't implement a trait for a type"** (Rust). A
      trait can be implemented for any type inside one's own program; libraries
      cannot conflict, since the program's choice wins (M).
- [ ] CL8. **"String formatting is a mini-language"** (C, Rust, Python).
      Interpolation `"{name} is {age}"` checked at compile time (M).
- [ ] CL9. **"Equality compares addresses by surprise"** (Java `==`, JS objects,
      Fides structs until now). `==` on structs and enums compares contents;
      ordering needs an explicit `cmp`. (Work in progress, Nov 2.) (M)
- [ ] CL10. **"Units: metres added to seconds"** (all but F#). Units of measure
      checked at compile time, free at run time (stdlib `units` as types) (L).

## Track CO — Observability and production

- [x] CO1. **"API keys ended up in the logs"** (all; real incidents). `@secret`
      values cannot reach print, logs or the network; the compiler rejects it.
      Left for adoption: stdlib APIs that take keys mark them `@secret`.
- [ ] CO2. **"Tracing needs code changes everywhere"** (all). Spans for every
      network, file and database effect, switched on with a flag, no code change (M).
- [~] CO3. **"Metrics need a library and boilerplate"** (most). `metrics` in the
      stdlib, Prometheus text format. Left: served by `http` with one call (S).
- [~] CO4. **"Logs are unstructured"** (most). `log` with fields. Left: JSON output
      checked against the spec, levels per module (S).
- [~] CO5. **"Profiling production needs another tool"** (most). `fid profile`
      samples the native call stack and writes flame graph data. Left: attach to a
      running service (competitive SL15) (L).
- [ ] CO6. **"Config parsing and validation boilerplate"** (all). A config struct
      loads from file, environment and flags, with errors that name the key (M).
- [ ] CO7. **"Personal data leaks into analytics"** (all; GDPR fines).
      `@personal` data cannot reach logs or the network without a consent check
      (stdlib `privacy`) (L).
- [ ] CO8. **"Health checks and graceful shutdown are hand-written"** (servers in
      all languages). `http_serve` handles SIGTERM, drains connections and serves
      `/health` (M).
- [ ] CO9. **"Crash dumps are unreadable"** (C, C++). A crash writes a report with
      the trace, the panic message and the build's version, readable by
      `fid inspect` (M).

## Track CI — Interop and platforms

- [~] CI1. **"FFI needs a binding generator"** (Rust bindgen, Go cgo). `extern "C"
      link` declares and links a C function in one block. Left: `@import("x.h")`
      (competitive LG15) (L).
- [ ] CI2. **"cgo makes builds slow and cross-compiling hard"** (Go). C calls cost
      nothing at build time on the native backend (no C compiler) (M).
- [ ] CI3. **"Callbacks from C are unsafe and awkward"** (Rust, Go). A named Fides
      function passes as a C callback; a closure that captures is rejected with
      the fix (done for named functions). Left: user data pointers (S).
- [~] CI4. **"Windows is a second-class platform"** (many Unix-first languages).
      Windows is Tier 1: native executables, no MinGW or MSVC needed. Left: the
      same for Linux and macOS (competitive PL2) (L).
- [ ] CI5. **"WebAssembly support is partial"** (Go, Python). WASI programs from
      the same source, the same tests (competitive PL4) (L).
- [ ] CI6. **"Embedding the language in another app is hard"** (Python, JS). A
      C API to run Fides code with a capability-limited sandbox (L).
- [ ] CI7. **"Calling the language from others needs glue"** (Rust, Go). `fid build
      --lib` writes a C header and a shared library from `pub extern` functions (M).

## Track CP — Performance surprises

- [ ] CP1. **"Debug builds are unusably slow"** (Rust, C++). A debug build keeps
      check removal and inlining that do not hurt debugging, so it runs within 3x of
      release (M).
- [ ] CP2. **"Hidden allocations"** (C++, Rust `clone`, Go escapes). `fid explain
      --alloc` lists every allocation in a function and why it is on the heap (M).
- [ ] CP3. **"I can't tell why the optimizer did or didn't"** (all). `fid explain`
      lists the runtime checks removed and kept, with the reason for each (S).
- [ ] CP4. **"Cloning everything to satisfy the compiler"** (Rust). Moves where
      possible; the compiler says when a clone is needed and when it is not (M).
- [ ] CP5. **"`async` costs more than threads would"** (Rust, C#). Tasks switch in
      under 20 ns and allocate nothing per await (competitive RT9) (L).
- [ ] CP6. **"Bounds checks I can't get rid of safely"** (Rust, Go). Range analysis
      removes them whenever the index is provably in range (competitive OP24) (L).
- [ ] CP7. **"Startup time is slow"** (Java, Python, JS). Native binaries start in
      under a millisecond, with no runtime to load (S, measure and keep).
- [ ] CP8. **"Binaries are huge"** (Go, Rust). A hello world under 50 KB; only the
      functions used end up in the binary (M).
- [ ] CP9. **"Performance cliffs nobody warned me about"** (all). `fid lint --perf`
      warns on quadratic patterns: `str + str` in loops, `Vec::remove(0)`, linear
      lookups in loops (M).

## Summary

100 goals: 10 done, 21 partly done, 69 not started. The first done ones were
where the language's design already answers the complaint: no lifetime
annotations, defined integers, no undefined behaviour in safe code, traces in
every build, `@secret`, debug and release in one flag, and a written stability
promise. Tracks CM and CE added: no null `str` from the stdlib, error context,
the `unwrap` lint, and partly borrow-error explanations, the leak report, the
checked allocator, `fid audit` and cascade suppression.

Complaints about learning and day-to-day tooling (out-of-date guides, editor
support, a playground) are goals of the other roadmaps: release N2, competitive
TL8 and TL10.
