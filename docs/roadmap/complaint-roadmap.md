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
      Nov 17: on a Linux or macOS host `fid build <file>` with no flags
      builds for the machine through the machine layer (no C compiler, no
      linker) and goes on through C by itself when the machine layer
      rejects a program (compiler/main.fd cmd_build, `host_auto`). The
      suite run: tests/tooling/build-host-suite.sh (PASS-NATIVE /
      PASS-FALLBACK per test against `fid run`). Left: run it on Linux and
      macOS and record the counts here.
- [~] CB5. **"Cross-compiling needs a toolchain per target"** (C, C++, Rust).
      `fid build --target linux-arm64` from any host, nothing to install
      (competitive TL6) (L).
      Checked Nov 15: started under competitive TL6 — `--target` takes the OS
      first (compiler/main.fd normalize_target: linux-arm64, linux-x64,
      linux-riscv64, macos-arm64, windows-x64) and the Linux and macOS
      targets go through the machine layer (nat_ir: no C compiler, no
      sysroot). Left (TL6): a Windows target from a Linux or macOS host not
      run yet; `ccompile --target` still needs zig cc.
      Nov 17 (TL6's resolution): scripts/cross-from-linux.sh runs a
      Linux-built fid in WSL to build --target windows-x64 and runs the PE
      on Windows; ci.yml selfhost runs build-no-toolchain-check.sh with
      wine (REQUIRE_WINE=1); ccompile --target needs zig by design
      (docs/reference/cross-compilation.md). Left: cross-from-linux.sh
      passing in a validation run.
- [~] CB6. **"The build is not reproducible"** (C, C++, npm). The same source and
      compiler give the same bytes. Done for the compiler itself (the bootstrap
      fixpoint). Left: guaranteed and tested for user programs (M).
      Nov 15: `scripts/verify/repro.sh` run on user programs — `dump-c` and the
      native `emit` exe are byte-identical across directories, temp dirs and
      time. `ccompile` differs only in the PE timestamp the linker writes
      (20 bytes in the COFF header): left, pass `/Brepro` when the linker is lld.
- [x] CB7. **"Debug or release: which flags?"** (C, C++). `--debug` and `--release`,
      nothing else to know.
- [x] CB8. **"The build cache serves stale results"** (Bazel, Gradle, ccache).
      Content hashes of everything an artifact depends on, tested against changes in
      each input. (A stale hit was found and fixed on Nov 2.) (M)
      Done Nov 15: `scripts/stale-cache-test.sh` proves a change to an imported
      module or to the flags re-runs the build, `--no-cache` forces it, and an
      unchanged rebuild still hits the cache.
- [~] CB9. **"Linker errors are unreadable"** (C, C++, Rust). The linker is part of
      the compiler: a missing symbol is reported as a Fides error with its source
      line and the library that should have provided it (M).
      Done (Nov 15, `fid ccompile`): the C compiler's messages are kept and
      each symbol the linker reports missing (GNU ld, lld/zig cc, tcc, MSVC
      link, the macOS linker) becomes "error: link: the C function 'f' is
      not in any library the program links" with `--> file.fd:LINE` of its
      `extern "C"` declaration, the `link "lib"` that should provide it and
      a Fix line, before the linker's own text (compiler/main.fd
      link_error_report). Checked by scripts/tooling-goals-check.sh over
      scripts/tooling/cb9_link.fd after the next build. Left: declarations
      in imported modules (only the built file is searched); the native
      backends (pe_ir's missing DLL export is already a Fides error,
      test_pe_dll_nofn_fail.fd).

## Track CM — Memory and safety

- [x] CM1. **"Lifetime annotations are unreadable"** (Rust). References cannot be
      stored, returned or captured by escaping closures, so a lifetime is never
      written.
- [x] CM2. **"I fight the borrow checker for code that is obviously fine"**
      (Rust). Each rejection explains which use conflicts and suggests the change
      that fixes it; patterns people hit most (two-phase borrows, disjoint fields)
      accepted (release K5) (L).
      Done: every borrow error names the borrow that holds the value (who, `&` or
      `&mut`, which path, the line it was made on) and a `Fix:` line, in both
      compilers. Release K5's soundness gaps are closed in `fid` (Nov 4–6):
      `s.x = 1` while `s` is borrowed, `&v[i]` and trait impl bodies are
      borrow-checked. Verified Nov 15: `fid` already accepts both patterns —
      borrows are kept per field path (`extract_field_path`, conflicts only on
      the same path, a prefix or an extension), and call and method arguments
      are checked in a reservation phase that activates `&mut` after the
      shared reads (`in_reservation_phase`, `activate_reserved_borrows` in
      compiler/borrowck.fd). Tests: `test_borrowck.fd` (`&p.x` with `&p.y`,
      `f(&mut t, &t)`) and `test_frontend_disjoint_borrows.fd` (`swap(&mut
      p.a, &mut p.b)`, `v.push(v.len())`).
- [~] CM3. **"Is this a move or a copy?"** (Rust, C++). One rule for `Vec`, `str`
      and `HashMap`, checked by the compiler (release M4, M5) (L).
      Decided (M4, proposal 0001): copy-on-write. Left (M5): the lowering is
      behind `LOWER_COW`, off (the compiler compiling itself runs out of
      memory with it on). Since ownership (proposal 0004) a `let` of an
      existing Vec takes its own owner, so `var b = a; b.push(2)` leaves `a`
      as it was (checked on the interpreter, competitive LG1); what is left
      is the M5 decision and its compiler check.
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
- [x] CM7. **"Garbage collection pauses"** (Go, Java, C#). No collector; the
      compiler frees what does not escape (L).
      Done (Nov 4–6): `Vec` and `HashMap` locals, a `Vec<str>` or `Vec<Vec>`
      that owns its elements, collections declared in a `region` at its end,
      and string temporaries are freed (`test_mem_owned_drop`). What the
      compiler cannot prove finished lives until exit: a leak, never a
      dangling pointer.
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
      with the trace. `fid emit --checked-alloc` (native) written Nov 4, not
      run. Left: running the native mode; naming the block in the report.
- [~] CM10. **"`unsafe` hides everywhere in my dependencies"** (Rust, Go, C#).
      `fid audit` lists every `unsafe` block reachable from a program, with its
      written justification (release M10) (M).
      Done: `fid audit <file.fd>` lists each `unsafe` block, `@unsafe` function
      and `extern "C"` in the program and every module it uses, with its
      `// SAFETY:` comment; the stdlib is summed per module (`--stdlib` lists
      it); `--strict` fails when one outside the stdlib has none.
      Done Nov 15: every unsafe site of stdlib/*.fd has its `// SAFETY:`
      comment (core 248, collections 29, the FFI modules' extern blocks: 341
      in all, 0 bare); core states its one shared assumption (handles are
      values core made) before Vec_new. Found on the way and fixed:
      `v.extend(v)` read the buffer its own pushes had freed (core
      vec_raw_extend re-reads it each round); HashSet's `table` pointer was a
      `pub` field any program could forge (now private). Nov 16: the
      runtimes' blocks have their comment too (stdlib/pe_runtime,
      stdlib/linux_runtime: 0 bare); the word/byte helpers say NOT checked,
      their callers' blocks being the runtime's own. `v.extend(v)` has its
      test (`test_runtime_vec_extend_self.fd`; the C runtime reads the count
      once). Left: the sites the
      comments mark NOT checked or NOT sound — raw-address APIs that should
      be `@unsafe` (volatile ptr_read/ptr_write, arena mem_copy, net_recv,
      ByteBuffer.write_bytes, core's `pub` i64 entry points, raw tls/sync
      handles) and image_load_png/_jpg passing values as out-pointers
      (release M10).
      Nov 16: the C runtime's own `p_vec_extend` read `other->len` each
      round, so `p_vec_extend(v, v)` never ended on the C path; it reads the
      count once now (codegen_c.fd), and test_cback_vec_extend_self.fd
      checks self-extension on every backend. Left as above: the runtimes'
      49 blocks, the NOT checked / NOT sound sites.

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
- [x] CE3. **"Exceptions I did not know a function could throw"** (Java, C#,
      Python, JS). A function's failure modes are in its type; there are no hidden
      exceptions (M).
      Verified Nov 15: the language has no exception mechanism — recoverable
      failure is `Result`/`Option` in the type (reference §8.2), panic is a
      documented abort with a stack trace, `@no-panic`/`@crashfree` prove a
      function cannot panic. Conformance tests cover the panic behaviours.
- [~] CE4. **"Compiler errors I cannot read"** (C++ templates, Rust generics, Scala
      implicits). Every error has a code, the source line, and a `Fix:` line.
      Left: a test for each error message (competitive TL2) (L).
- [~] CE5. **"The compiler knows the fix but makes me type it"** (most).
      `fid fix` applies suggested fixes (competitive TL3) (M).
      Checked Nov 15: `fid fix <file> [--check]` exists (compiler/main.fd
      cmd_fix over lsp.fd fix_suggestions, also the editor's quick fixes):
      val -> var, `as T` for a narrowing, a misspelled name, an unused import,
      and now a call of a `@deprecated(use = ...)` function (CU3); a fix is
      kept only when it leaves fewer errors. Left: the other `Fix:` lines
      (most diagnostics print one that no fix applies yet).
- [~] CE6. **"Cascades: one mistake, two hundred errors"** (C++, Java). After a
      first error, dependent errors are suppressed (M).
      Done in `fid`: an undefined name is reported once per function, and an
      operand whose error is reported adds no arithmetic error. Nov 15: an
      unknown struct literal is reported once per function, with its line
      (`test_frontend_cascade_struct_fail.fd`). Left: the same in `fidesc`
      (frozen), more dependent errors.
- [x] CE7. **"No stack trace in release builds"** (C, C++, Rust without
      `RUST_BACKTRACE`). Panic traces in every build, at no cost per call.
- [~] CE8. **"A crash report without the values that caused it"** (most). A panic
      prints the arguments of the failing call and the locals of its frame, when
      they are words (M).
      Done Nov 16, C path, `fid ccompile -g`: every function keeps a frame
      record (names, kinds and addresses of its word-sized parameters and
      locals: integers, bools, floats, strings), made current at entry and
      after each call; p_panic and the invalid-access handlers print it as
      `values in <fn>:` before the trace (codegen_c.fd cg_dbg_prologue,
      `__p_dbg_print_frame`). Debug builds only: it is per-call work, so
      plain builds and `--debug` are unchanged. test_cback_debug_info.fd.
      Left: the native backends (pe_ir, nat_ir: from the frame layout and
      DB4's variable locations); a panic caught by a longjmp leaves the
      callee's record current until the next call.
- [~] CE9. **"Error messages that blame the library, not my call"** (C++, Rust
      generics). An error inside a generic function is reported at the call site
      that chose the types (M).
      Nov 16: built and tested (test_generic_call_site_fail.fd, the two
      test_frontend_generic_callsite_*_fail.fd, diagnostics/err_generic_call_site.fd
      prints the library file as a note): a generic body is checked once with T
      abstract; what it does to a T that holds only for some types (reading
      a field `v.x`, directly or by passing T on to another generic) is
      checked at each call with the type the call chose
      (compiler/typechecker.fd TypeChecker_ce9_*). The error's place is the
      call, the body's lines are notes: `Type error (line 31, in 'main'):
      'total_x' is called with T = Label, but Label has no field 'x', which
      the body of 'total_x' needs.` then `note: at <file>:20, in 'total_x':
      its body reads the field 'x' of a T` (one note per generic in the
      chain; compiler.fd enrich_note_locations turns "line N" / "module 'm',
      line N" into file:line). Tests: test_generic_call_site_fail.fd,
      test_frontend_generic_callsite{,_fail,_nested_fail}.fd,
      diagnostics/err_generic_call_site.fd (library in another module).
      Left: field writes on a T (`v.x = 1` is still an error in the body),
      and other operations an unbounded T allows only for some types.

## Track CA — Async and concurrency

- [~] CA1. **"Function colouring: async infects everything"** (Rust, JS, Python,
      C#). Any function runs inside a task and suspends where it waits; callers
      do not change (L). Done (release H9): an `async fn` with `f64` or struct
      arguments or results runs as a task on the interpreter and C paths.
      Nov 17: native async matches `fid run` on `emit` and `x86_64-linux`
      (morning check); reference arguments work
      (test_lowering_async_ref); `fid run` runs an async fn until its first
      wait (LG11, compiler/interp.fd "Interleaved tasks"; tests
      test_async_suspend, test_async_first_wait); async methods in impls
      and traits (LG14, test_lowering_async_method).
- [x] CA2. **"Which async runtime: tokio or async-std?"** (Rust). One runtime,
      in the standard library, used by every library (M).
      Done: `stdlib/pe_runtime/conc.fd` is the only task runtime — workers,
      fibers, run queue, poller and blocking pool; every spawn/task/channel
      goes through it on all backends.
- [x] CA3. **"`Send` and `Sync` errors I do not understand"** (Rust). Spawning
      checks what crosses threads and says in one sentence what cannot and why
      (release H4) (M).
      Done: `spawn` captures, `p_thread_spawn` args and `p_chan_send` values
      are checked for @send; errors say what cannot cross, name the type and
      suggest the fix — `E082` + `file:line:col` caret via `enrich_errors`
      (test `diagnostics/err_send_spawn.fd`).
- [ ] CA4. **"Deadlocks in production"** (Go, C++, Java). Lock order checked across
      the call graph at compile time; a cycle is an error (L).
- [~] CA5. **"Data races"** (Go, C++, Java). Code reached from `spawn` cannot share
      mutable state unchecked (release H3) (L).
      Done: every argument of a spawned call must be `@send`; raw pointers and
      references are not (`test_race_ptr_fail`); a global is not written from
      spawned code without a lock. Nov 17: spawn arguments are copied
      whole (core `p_send_copy`) on every backend, and the spawn wrapper
      frees the copies for any one-word result, str/Vec/map included
      (tests test_spawn_mutate_both, test_await_spawn_collections,
      tests/leaks/spawn_args.fd, await_results.fd).
- [~] CA6. **"Goroutine and task leaks"** (Go, JS). Structured concurrency: a task
      cannot outlive the scope that started it unless detached by name
      (competitive LG11) (L).
      Checked Nov 15: `spawn scope { ... }` exists in the language (parser
      `Stmt::SpawnScope`; lower.fd makes an executor and joins its tasks at
      the end, `p_executor_run_all`; `test_spawn_scope.fd`). Left: a bare
      `spawn` outside a scope is accepted and can outlive its caller (no
      check, no named detach).
- [~] CA7. **"Cancellation is an afterthought"** (Go contexts, JS AbortController).
      Every task can be cancelled, and every wait respects it (L).
      Checked Nov 15: `cancel scope;` cancels the pending tasks of the
      innermost scope (lower.fd `Stmt::CancelScope`, `p_executor_cancel`;
      `test_cancellation.fd`); stdlib/task.fd has a cooperative scope flag
      (`task_scope_cancel`, `task_scope_cancelled`). Nov 16: waits that
      respect cancellation, on every backend (stdlib/task.fd "Cancellation
      tokens"): a token any task or thread can cancel (`task_token_new`,
      `task_token_cancel`), and `task_sleep_cancellable`,
      `task_wait_read/_write_cancellable`, `task_recv_cancellable`,
      `task_join_cancellable`, `task_scope_sleep` return within 10 ms of a
      cancel (they wait in slices that short) (`test_runtime_cancel.fd`).
      Decision: cancellation is a checkpoint, not a kill — a task stopped
      mid-way would skip the drops of what it owns. Left: the waits are
      library functions, not every builtin wait (sync_channel_recv,
      p_co_sleep keep ignoring tokens).
- [~] CA8. **"Timeouts are bolted on"** (most). `timeout(ms) { ... }` around any
      block that waits (M).
      Checked Nov 15: library timeouts only — `task_join_timeout(t, ms)`
      (stdlib/task.fd), `net_set_recv_timeout`. Nov 16: the cancellable
      waits (CA7) and `task_token_with_timeout(ms)`, a token that cancels
      itself: the library form of the block — every cancellable wait given
      that token gives up at the deadline (`test_runtime_cancel.fd`). Left:
      the `timeout(ms) { }` syntax; for waits inside it to find their token
      without an argument, each runtime needs a per-task slot for it (a
      word of the task block natively, a field of the C `__p_co`, the
      interpreter's task), then parser and lowering hunks.
      Nov 20: the syntax exists in `fid` (stdlib/http_client.fd uses it)
      and the frozen bootstrap parses and lowers it again — the Nov 20
      stage-0 fixes backport the parser desugar (`if __timeout_block(ms)`
      / `__timeout_expired` markers) and the token lowering with the
      p_task_token/p_task_set_token/p_clock_mono_ms builtins; test_
      timeout_block.fd documents that a stage-1-built fid still gives
      tasks 1 MB stacks (a messaged panic in the recursive drop).
- [ ] CA9. **"Async stack traces are useless"** (Rust, JS, C#). A panic in a task
      shows the chain of tasks that awaited it (M).
- [~] CA10. **"Blocking call in async code freezes everything"** (Rust, JS,
      Python). The effect system knows which calls block; a blocking call inside a
      task is an error or runs on a worker thread (M).
      Done: blocking syscalls run on the blocking pool in `conc.fd` — a task
      in a syscall never stalls its worker. Left: effect-system metadata
      marking calls blocking so the compiler can warn when one bypasses.

## Track CT — Testing and debugging

- [ ] CT1. **"Mocking time, network and files is painful and flaky"** (all). A test
      replaces the clock, network or file system for the code under test, because
      effects say exactly what that code touches. No mocking framework (L).
- [x] CT2. **"Tests need a framework to write"** (C, C++, Java). `fid test file.fd`
      runs every `fn test_*()` in its own process and times every `fn bench_*()`.
      Verified Nov 15: table tests with a name per row are in stdlib/test.fd
      (`table_new`/`table_int`/`table_str`/`table_bool`/`table_done`: every
      row runs, failures listed by row; FIDES_ROW=<text> runs only matching
      rows; `test_stdlib_table.fd`). Left: a filter on test functions and
      parallel runs in `fid test` (compiler/main.fd `cmd_test_file`) (M).
      Done Nov 15 (compiler/main.fd): `fid test <file> --filter <text>` (or
      `--run`) keeps the tests, benches and fuzz targets whose name holds the
      text; `--jobs <n>` (`-j`) runs the tests n at a time, each in its own
      process as before, printing each one's output and then its verdict in
      order (test_run_parallel: background jobs and `wait` on POSIX,
      PowerShell Start-Process/Wait-Process on Windows). Checked by
      scripts/tooling-goals-check.sh over scripts/tooling/ct2_tests.fd after
      the next build.
- [x] CT3. **"Flaky tests from time and randomness"** (all). Under `fid test`,
      time and random numbers are fixed per test unless the test asks otherwise (M).
      Done Nov 15 (stdlib): `dt_now()` returns FIDES_TEST_NOW (unix seconds)
      and `random_seed_time()` seeds with FIDES_TEST_SEED when they are set
      (stdlib random is already seeded 1 by default; `test_stdlib_test_clock.fd`,
      `scripts/verify/ct3_test_clock.sh` runs it with both set). Left: `fid
      test` setting both for each test (compiler/main.fd `cmd_test_file`, the
      way it sets FIDES_UPDATE), and an opt-out for a test that wants the
      real clock.
      Done Nov 15 (compiler/main.fd test_env_prefix, test_fixed_clock): `fid
      test` runs each test with FIDES_TEST_NOW=1700000000 and
      FIDES_TEST_SEED=1 (also under --jobs); opt-outs: `--real-clock` for the
      run, a test named `*_real_clock`, or the variable already set (its
      value is passed on). Checked by scripts/tooling-goals-check.sh over
      scripts/tooling/ct3_clock_tests.fd after the next build. Not covered:
      the raw builtins (p_clock_now_ms called directly is not fixed).
- [x] CT4. **"Fuzzing needs another tool"** (C, C++, Rust). `fn fuzz_*(data: Vec<u8>)`
      runs under `fid test --fuzz`, as in Go (competitive SL13) (M).
      Done Nov 15: `fid test <file> --fuzz` runs each `fn fuzz_*(data: Vec<u8>)`
      on 100 deterministic seeded inputs (`_fuzz_input`'s bounded LCG); a
      panic in a target fails the run. Verified: a mutating target caught an
      overflow; a fold-only target passes `(100 seeds)`.
- [x] CT5. **"Snapshot tests are hand-rolled"** (most). `expect_snapshot(value)`
      writes and checks a stored file, updated with `--update` (S).
      Done Nov 15: `expect_snapshot(name, value)` in stdlib/test.fd compares
      with `<name>.snap` beside the test; `fid test --update` writes it. A
      missing snapshot fails with "run fid test --update to write it";
      verified missing→fail, --update→write, re-run→pass.
- [ ] CT6. **"I can't debug a release build"** (C++, Rust). Line tables and
      variable locations in optimized builds, readable by standard debuggers
      (release N9) (L).
- [ ] CT7. **"The bug only happens in production"** (all). Record the inputs every
      effect brings in; replay the run exactly, in the interpreter, step by step (L).
- [~] CT8. **"Coverage needs another tool"** (C, C++, Go before 1.20). `fid test
      --coverage` with lines and branches, from `veil` (M).
      Done Nov 15: `fid test <file> --coverage` runs the suite's tests through
      `veil` and prints its line/function/instruction report. Fixed on the
      way: veil resolved `use` imports (`lower_to_ir_module_with_path`) and
      three by-value copies that double-freed (`fid veil` crashed on any
      program). Left: `fid test --coverage` still double-frees when the
      covered program panics — one more shared-owner site (known-bugs 2g.30).
- [ ] CT9. **"Debugger shows mangled names and optimized-out values"** (C++, Rust).
      Debug info names Fides functions and types as written (M).
- [x] CT10. **"printf debugging needs a recompile"** (all compiled languages).
      `fid run --trace fn_name` prints each call to a function with its arguments
      and result, no code change (M).
      Done Nov 15: `fid run <file> --trace f,g` (or `--trace=f,g`; a method
      as `T::m`) prints `[trace] f(1, "a")` at each call and `[trace] f = 7`
      (or `returned`, or `panicked: ...`) at its end on stderr, indented by
      call depth; strings are quoted, the closure environment is left out
      (compiler/interp.fd Interpreter_run_function, Interpreter_trace_value;
      the list in pgo.fd g_trace_fns; main.fd cmd_run). Before, `--trace` was
      parsed and did nothing. tests/bootstrap/test_tooling_trace.fd, checked
      by scripts/tooling-goals-check.sh after the next build. The trace is
      the interpreter's: a native build is not traced.

## Track CD — Dependencies and supply chain

- [~] CD1. **"Any dependency can steal my keys"** (npm, PyPI, Cargo). Each
      dependency is granted capabilities in `ark.toml`; a build where it uses more
      fails. Upgrades show a capability diff (L).
      Nov 17: direct `fid build`/`run`/`check`/`ccompile` enforce
      ark.toml's `[capabilities]` from the capabilities the compiler infers
      each imported pact to use (compiler.fd `cd1_check`, on every build
      through `build_program_with_path_borrowck`), with the call path that
      needs each one (docs/pact-capabilities.md); `ark install`/`build`/
      `test`/`audit`/`update` check them by a scan of each pact's text
      (ark.fd `ark_cap_check`; `ark update` prints the diff). Left: a run
      of its checks, then [x].
- [~] CD2. **"I need fifty packages for basic things"** (Rust, JS). A standard
      library that covers HTTP, JSON, TLS, regex, time, CSV, compression and more,
      each tested on every backend. Left: depth (competitive SL track) (L).
- [ ] CD3. **"`node_modules` is bigger than my program"** (JS). Dependencies are
      fetched once per machine, shared, and only the functions a program uses end
      up in the binary (M).
- [ ] CD4. **"Diamond dependencies: two versions of the same library"** (Rust,
      Java). One version per library in a build; a conflict is reported with both
      paths and the newest version that satisfies both (M).
- [x] CD5. **"Left-pad: a deleted package breaks the world"** (npm). Dependencies
      come from Git or paths with content hashes in the lock file, and are
      vendored with one command (S).
      Done: ark.lock records each git pact's commit and an FNV-1a hash of
      each path pact's files (`fid ark`, competitive TL9; `ark install`
      checks out the locked commit, `ark audit` fails on a change). Nov 15:
      `ark vendor` (CD9) is the one command. The path-pact hash is 64-bit
      FNV, not a cryptographic hash.
- [x] CD6. **"Typosquatted packages"** (npm, PyPI). `ark add` warns about a name
      one edit away from a known library (S).
      Done Nov 15: `ark add` warns when the name is one edit (Levenshtein) from a
      dependency already in ark.toml or from a package in the local index copy.
      Checked Nov 15: that was the Rust ark (src/ark.rs) only; `fid ark` now
      warns the same way (compiler/ark.fd ark_edit_distance in ark_cmd_add),
      checked by scripts/tooling-goals-check.sh after the next build.
- [x] CD7. **"I don't know what my dependencies do"** (all). `ark audit` prints each
      dependency's capabilities, `unsafe` blocks and C libraries (M).
      Done: `fid ark audit [--files]` (compiler/ark.fd, competitive TL9): per
      pact its revision against ark.lock, files, lines, `unsafe` blocks,
      `extern` declarations and process/file/network calls. Nov 15: also the
      C libraries it links (`extern "C" link "x"`) and the capabilities its
      imports ask for (`use m with { IO, ... }`), and a vendored pact is
      audited from vendor/. Checked by scripts/tooling-goals-check.sh after
      the next build. It reads the source text (a pattern in a comment
      counts too).
- [ ] CD8. **"Build scripts run arbitrary code at install time"** (npm postinstall,
      Cargo build.rs, pip setup.py). No install-time code; `comptime` is sandboxed
      with no I/O (M).
- [x] CD9. **"Vendoring is a pain"** (Go before modules, C++). `ark vendor` copies
      every dependency into the project, and builds work offline (S).
      Done Nov 15: `fid ark vendor` (compiler/ark.fd ark_cmd_vendor) installs
      what is missing, copies each pact into vendor/ (its .fd, .c, .h, .toml
      and .md files, not .git) and writes vendor/vendor.lock with each copy's
      hash. `use` looks in vendor/ before ark_modules/ (compiler.fd
      ark_push_candidates), so a checkout with vendor/ builds with no
      network and no `ark install`; `ark install` takes a vendored pact as it
      is after checking it against vendor.lock (a changed copy fails), and
      `ark audit` reports it. Checked by scripts/tooling-goals-check.sh after
      the next build.

## Track CU — Upgrades and stability

- [ ] CU1. **"Upgrading the language breaks my code"** (Python 2→3, Scala 3,
      Swift). `fid fix --edition` migrates code between editions (competitive
      TL3, ST3) (L).
- [x] CU2. **"No promise about what will keep working"** (young languages). A
      written stability promise and semantic versioning
      (docs/reference/stability.md).
- [x] CU3. **"Deprecations with no replacement path"** (all). `@deprecated(use =
      "new_fn")` warns with the replacement, and `fid fix` applies it (S).
      Done (Nov 15, `fid`): `@deprecated`, `@deprecated("text")` and
      `@deprecated(use = "new_fn")` are accepted on a function (the name check
      rejected it, and `use` inside an annotation was a parse error); every
      call of it outside its own body is a warning naming the replacement and
      a `Fix:` line (`TypeChecker_warn_deprecated`; `test_frontend_deprecated`).
      Left: the warning is printed by `fid lint` only (a build does not print
      type-checker warnings: compiler.fd); `fid fix` applying the rename;
      methods marked `@deprecated` (calls through `.m()` are not checked).
      Nov 15: every build, run and check prints the deprecation warnings on
      stderr (compiler.fd, after the type check; the other checker warnings
      stay with `fid lint`), and `fid fix` replaces each call of the old
      function with the one `use =` names (lsp.fd fix_for; the editor does
      not offer it as a quick fix yet: code actions read errors only).
      tests/bootstrap/test_tooling_fix_deprecated.fd, checked by
      scripts/tooling-goals-check.sh after the next build. Nov 16 (not yet
      built): methods too: the parser takes annotations on a method of an
      `impl`, and a call through `.m()` or `T::m()` of a `@deprecated` one
      warns as `'T.m' is deprecated` (typechecker
      `TypeChecker_warn_deprecated_method`; test_frontend_deprecated_method);
      `fid fix` renames plain calls only.
- [x] CU4. **"Features stuck on nightly for years"** (Rust). No nightly-only
      features: a feature ships behind a flag in a release, or not at all (S).
      Done Nov 15: docs/reference/stability.md — there is no nightly track; a
      flagged feature graduates into the language or is removed; nothing stays
      flag-only across a stable release.
- [ ] CU5. **"A minor release broke my build"** (npm semver in practice). `ark`
      checks a library's public API against its last release and refuses a minor
      version that removes or changes anything (M).
- [x] CU6. **"I can't tell what changed"** (all). Release notes list every change
      that can affect a working program under **Breaking** (docs/reference/
      stability.md). Left: generated from the changelog and checked (S).
      Done Nov 15: `scripts/release-notes.py` renders the newest changelog
      section as release notes, pulling breaking-change lines into a
      `## Breaking changes` section on top.
- [~] CU7. **"Old tutorials no longer compile"** (fast-moving languages). Examples in
      the docs are compiled and run in CI, so docs and compiler never drift
      (competitive SL19) (M).
      Done Nov 15: `scripts/doctest.py` runs every `/// Example:` block in
      stdlib against the built `fid` in CI (.github/workflows/ci.yml);
      `/// Example (native only):` marks examples that need FFI or hit the
      interpreter UTF-8 bug (known-bugs 2g.29). Left: run the native-only
      examples through `emit` in CI.
      Nov 16: `doctest.py --emit` built nothing (it ran `fid run` either
      way); now it builds a native-only example with `fid emit --target
      <host>-linux` (`emit` on Windows, `ccompile` elsewhere) and runs the
      binary, and ci.yml's selfhost job runs `--emit --native-only`.
      Left: a green CI run of that step (the 9 examples, db.fd's included,
      have not been built natively yet).
- [~] CU8. **"The compiler upgrade changed my program's speed"** (all). Benchmarks in
      CI fail on a slowdown over 10% (release D7) (M).
      Done (Nov 4): `bench/check.sh` fails on a slowdown over `LIMIT_PCT`
      against `bench/baseline.txt`. Left: running it in CI.

## Track CL — Language ergonomics

- [x] CL1. **"Serialization boilerplate"** (Go, Java, C++). `@derive(Deserialize)`
      beside the existing `@derive(Serialize)` (M).
      Checked Nov 15: `process_derive` (compiler.fd) accepts
      `@derive(Deserialize)` but the `<S>_from_json(json)` it generates ignores
      its argument and returns every field zero or empty: nothing is parsed.
      Nov 16 (not yet built): `S_from_json` reads each field from its key
      (integers, floats, bools, strings with escapes, a nested struct that
      derives Deserialize; only the object's own keys), and
      `S_from_json_error(json)` names a missing key or a value of the wrong
      type ("" when complete). Decision: `S_from_json` keeps its signature (a
      missing key leaves the default, so existing programs still build); the
      error is a separate function, not a `Result`. The readers are
      stdlib/derive_json.fd, which compiler.fd `process_derive` adds to a
      program that derives Deserialize (test_frontend_derive_deser). Fields of
      other types (Vec, enums) keep their default.
- [x] CL2. **"No sum types"** (Go, Java before 17). Enums with payloads and `match`.
      Left: exhaustiveness and guards everywhere (competitive LG8) (M).
      Verified Nov 15: done with competitive LG8 — guards, literals,
      wildcards, nested constructor patterns and exhaustiveness naming the
      missing variants (`TypeChecker_check_exhaustive`; `test_match_nested`,
      `test_match_guard`, `test_match_exhaustive`, and the `_nonexhaustive_fail`
      tests, generic and guarded included). LG8's one restriction stays: in
      expression position a nested pattern's scrutinee must be a variable.
- [x] CL3. **"`String`, `&str`, `OsString`, `CString`... which one?"** (Rust). One
      owned and one borrowed string type, UTF-8, and nothing else
      (release M11) (L).
      Done (Nov 4–6): `str` owned, `&str` borrowed; a borrowed string can be
      read, compared, printed and joined with `+`, but not returned as `str`,
      stored, or passed to a user function's `str` parameter; `str_clone(s)`
      copies (`test_borrowed_str` and three `_fail` tests).
- [x] CL4. **"No default or named arguments"** (Rust, Go, Java). Default and named
      arguments, checked at compile time (M).
      Default values (`fn f(a: i64, b: i64 = 10)`) and named arguments
      (`f(b: 2, a: 1)`) already parsed and lowered (lower.fd
      `Lowerer_resolve_call_args`); a named argument was not checked at all
      (an unknown name was dropped and its parameter passed 0). Done Nov 15
      (`fid`, `TypeChecker_check_named_args`): an unknown name, a parameter
      given twice, a positional argument after a named one, a parameter
      without a default left out and a named argument of the wrong type are
      errors. Tests: `test_frontend_named_args`, `_named_args_fail`,
      `_named_args_type_fail`, `_named_args_missing_fail`. Not covered: named
      arguments to methods (`v.m(x: 1)`), and `fidesc`.
- [x] CL5. **"Builder-pattern boilerplate"** (Rust, Java). Struct literals with
      defaults: `Config { port: 80, .. }` (M).
      Done Nov 15 (`fid`): a struct field may declare a default (`port: i64 =
      8080`, parser `parse_struct`); a literal ending in a bare `..` gives
      every field it leaves out its default, and one without a default is an
      error (`TypeChecker_check_struct_defaults`, which also type-checks the
      default); monomorphization writes the defaults into the literal
      (mono.fd `mn_fill_struct_defaults`), so no back end changes. Works for
      a struct of another module too (`Name { ..` starts a literal). Tests:
      `test_frontend_struct_defaults` (strings, an expression, a nested
      default literal), `_struct_defaults_fail`. Not covered: a literal in a
      global's or constant's initializer, a default calling a generic
      function, `fidesc`.
- [~] CL6. **"Macros nobody can read"** (C, Rust). `comptime` functions in plain
      Fides cover what macros are used for (competitive LG9) (L).
      Checked Nov 15 (state of competitive LG9): `comptime` functions and
      blocks run at compile time, `comptime gen` generates functions,
      structs, enums, constants and impls from a source string, `gensym`
      gives names nothing else declares, and a generated name never replaces
      a declared one. Left: expression-position expansion (what a macro call
      in an expression does), `gensym` inside a `comptime fn`.
- [x] CL7. **"Orphan rule: I can't implement a trait for a type"** (Rust). A
      trait can be implemented for any type inside one's own program; libraries
      cannot conflict, since the program's choice wins (M).
      Checked Nov 15: there is no orphan rule — the type checker accepts an
      impl of any trait for any type, and `merge_programs` (compiler.fd)
      drops a library's impl of a (trait, type) the program also implements
      ("user wins on (trait, type) conflicts"). Nov 16 (not yet built): two
      imported libraries that implement the same trait for the same type are
      an error naming both modules, unless the program implements it itself
      (compiler.fd `cc_find_trait_impl_conflicts`;
      test_frontend_trait_conflict_fail, and test_frontend_trait_override for
      a program replacing both, helpers in tests/bootstrap/trait_conflict/).
      Core's own impls are not compared with a library's.
- [x] CL8. **"String formatting is a mini-language"** (C, Rust, Python).
      Interpolation `"{name} is {age}"` checked at compile time (M).
      Format strings `f"{name} is {age}"` existed (parser
      `parse_fstring_parts`, each `{...}` parsed as an expression and
      type-checked: an unknown name or a bad call is a compile error). Nov
      15: a value with no text form is an error too — a struct (or `&`
      struct) interpolated was read as the bytes of a string
      (`TypeChecker_check_fstring_part`; `test_frontend_fstring`,
      `test_frontend_fstring_struct_fail`). There are no format specifiers
      (`{x:#x}` is `{x}`, reference §1.8).
- [x] CL9. **"Equality compares addresses by surprise"** (Java `==`, JS objects,
      Fides structs until now). `==` on structs and enums compares contents;
      ordering needs an explicit `cmp` (M).
      Done (proposal 0001): fields one by one, nested structs and enums through
      their own `==`, strings by text (`test_struct_eq`).
- [~] CL10. **"Units: metres added to seconds"** (all but F#). Units of measure
      checked at compile time, free at run time (stdlib `units` as types) (L).
      Checked Nov 15: stdlib/units.fd already gives each SI unit its own
      struct (Meters, Seconds, Newtons, ...), so metres passed where seconds
      are expected is a type error (`test_stdlib_units_mismatch_fail.fd`).
      Nov 15: same-unit add/sub for every unit, scaling, comparison, ratio
      and the inverse derived units (`travel`, `travel_time`,
      `mass_from_force`; `test_stdlib_units.fd`). Nov 16 (not yet built):
      `+ - * / %` on any struct call its `add`, `sub`, `mul`, `div`, `rem`
      method (typechecker `TypeChecker_check_struct_operator`, lower.fd
      `Lowerer_struct_operator`; a missing method is an error naming it), and
      units.fd gives each unit `add`/`sub` and, where it scales, `mul`/`div`
      by an f64 (test_frontend_struct_ops, test_frontend_units_ops, and the
      `_fail` tests). Left: dimensions computed by the type checker
      (`Newtons * Meters` is Joules) instead of one function per unit pair;
      a left operand that is not a struct (`2.0 * m`).

## Track CO — Observability and production

- [x] CO1. **"API keys ended up in the logs"** (all; real incidents). `@secret`
      values cannot reach print, logs or the network; the compiler rejects it.
      Adoption, Nov 15: the stdlib functions that take a key mark it
      `@secret` and release their result with `@declassify`
      (crypto_hmac_sha256_bytes, crypto_pbkdf2_sha256, chacha20_encrypt,
      poly1305_tag, chacha20poly_encrypt/_decrypt, x25519_public_key,
      ed25519_public_key/_sign, crypto_bind's cb_gcm_encrypt/_decrypt,
      cb_pbkdf2_derive, cb_ecdsa_sign on both platforms), and credentials
      read at run time come out secret: `env_secret(name)` (env) and
      `config_secret(c, key)` (config) — printing one is a compile error
      (`test_stdlib_secret_keys.fd`, `_secret_env_fail`, `_secret_config_fail`).
      Nov 16: results that are key material are `@secret` — hkdf_extract,
      hkdf_expand, hkdf_expand_label, chacha20_block, x25519_shared_secret,
      ed25519_keypair, and the new tls_record_seal/_open take their key
      `@secret`; the RFC-vector tests print through a `@declassify fn
      reveal` of their own (test_x25519, test_crypto_tls_vectors,
      test_ed25519). Decision: cb_ecdsa_generate stays as it is — it hands
      the key out through a reference parameter and returns a status, and
      a `@secret` function's label is its result's: callers keep the
      private blob in a `@secret` local. Left: postgres and TLS passwords
      (they are sent and stored, which needs `@declassify` boundaries).
- [~] CO2. **"Tracing needs code changes everywhere"** (all). Spans for every
      network, file and database effect, switched on with a flag, no code change (M).
      Done Nov 15: stdlib/trace.fd; with FIDES_TRACE=1 the stdlib prints one
      JSON line per span on stderr (`{"span","detail","us","ok"}`) for
      http_client's http_request (every helper goes through it), postgres
      pg_query / pg_query_params, file_write / file_append; programs add
      their own with trace_span_begin/_end (`test_stdlib_trace.fd`,
      `scripts/verify/co2_trace.sh`). Left: file reads, sockets (net), the
      HTTP server side, sqlite; a `fid run --trace-spans` flag; parent/child
      span ids.
- [x] CO3. **"Metrics need a library and boilerplate"** (most). `metrics` in the
      stdlib, Prometheus text format, served by `metrics_serve(m, port)` —
      one call (S).
- [x] CO4. **"Logs are unstructured"** (most). `log` with fields, JSON lines
      checked against the spec (`log_render_json` is public; a test parses it
      back), per-module levels (`log_set_module_level`, `logmf`) (S).
- [~] CO5. **"Profiling production needs another tool"** (most). `fid profile`
      samples the native call stack and writes flame graph data. Left: attach to a
      running service (competitive SL15) (L).
- [x] CO6. **"Config parsing and validation boilerplate"** (all). `config`:
      declared keys load from file (INI), environment and `--flags` —
      precedence flag > env > file > default, errors naming the key (M).
- [x] CO7. **"Personal data leaks into analytics"** (all; GDPR fines).
      `@sensitive` data cannot reach logs or the network without a consent check
      (stdlib `privacy`: `consent_grant`/`consent_revoke`/`consent_given` and
      the `@declassify`d `privacy::release`) (L).
- [x] CO8. **"Health checks and graceful shutdown are hand-written"** (servers in
      all languages). `http_serve` serves `/health`, stops on SIGTERM/SIGINT or
      `http_stop()`, and drains live connections before closing (M).
- [ ] CO9. **"Crash dumps are unreadable"** (C, C++). A crash writes a report with
      the trace, the panic message and the build's version, readable by
      `fid inspect` (M).
      Nov 17: `fid run --crash-file <file>` / FID_CRASH_REPORT (interp.fd
      interp_write_crash_report), programs built through C (codegen_c.fd
      __p_crash_report: a panic, a crash, a stack overflow), `fid inspect`
      (main.fd cmd_inspect); native programs too: linux_runtime/base.fd
      pe_crash_report (pert_panic, pe_on_signal; nat_ir Linux targets and
      --nat Windows), pe_runtime/base.fd pert_crash_report (pe_ir: the
      p_panic stub calls it, pe_ir_stubs.fd PeirGen_gen_panic); a release
      build's report names its version (compiler.fd astcache_load);
      tests/tooling/crash-report-check.sh native section. Pending a run of
      that script on Windows and Linux.

## Track CI — Interop and platforms

- [~] CI1. **"FFI needs a binding generator"** (Rust bindgen, Go cgo). `extern "C"
      link` declares and links a C function in one block. Left: `@import("x.h")`
      (competitive LG15) (L).
- [ ] CI2. **"cgo makes builds slow and cross-compiling hard"** (Go). C calls cost
      nothing at build time on the native backend (no C compiler) (M).
- [~] CI3. **"Callbacks from C are unsafe and awkward"** (Rust, Go). A named Fides
      function passes as a C callback; a closure that captures is rejected with
      the fix (done for named functions). Left: user data pointers (S).
      Nov 16: user data pointers written: `&mut st as *u8` is the struct's
      address (lower.fd, Expr::Ref of a Cast), the callback takes it as a
      `*u8` and reads and changes the struct through `ctx as *State`
      (test_c_callback_user_data.fd: qsort_s on Windows, qsort_r on Linux
      and macOS, each order of context and comparison). A closure given to C
      is refused with its captures named and the fix, pass the state through
      the user data pointer (typechecker.fd TypeChecker_c_callback_fix;
      test_c_callback_capture_fail.fd). The C output defines _GNU_SOURCE on
      Linux when C gets a callback through a system header (glibc declares
      qsort_r only then; codegen_c.fd cg_emit_ffi_feature_macros). Built
      and run on Windows: test_c_callback_user_data.fd prints its expected
      output through `fid ccompile`, test_c_callback_capture_fail.fd is
      refused with the fix (the closure written `fn(..) -> i32 { }`). Left:
      a run on Linux and macOS, the native backends (pe_ir, nat_ir).
- [~] CI4. **"Windows is a second-class platform"** (many Unix-first languages).
      Windows is Tier 1: native executables, no MinGW or MSVC needed. Left: the
      same for Linux and macOS (competitive PL2) (L).
      Nov 17: native Linux suite gated in ci.yml natgate on
      x86_64/aarch64/riscv64-linux with the targets' C libraries for qemu
      and REQUIRE_LOADER=1 (no dynamic test skipped); macOS through
      macos-native.yml and tier1.yml macos-15. Sample run
      (docs/verification/ci4-native-suite-2026-11-17.md): 35/39
      x86_64-linux, 11/12 aarch64 and riscv64, 6/6 Mach-O well-formed;
      found ME26 (ime_sra) dropping struct payload words
      (test_native_enum_payload_param). Left: the ime_sra and nat_ir ldyn
      fixes checked in a built compiler, the full suite green on all three
      Linux targets, and a run on a Mac.
- [~] CI5. **"WebAssembly support is partial"** (Go, Python). WASI programs from
      the same source, the same tests (competitive PL4) (L).
      Done (Nov 15): `fid wasm` (compiler/wasm.fd, no C compiler) and `fid
      ccompile --target wasm32-wasi` build WASI programs from unchanged
      source; scripts/wasm-test.sh runs the suite's tests under Node's
      WASI and compares with `fid run`: 316 of 344 match. Left: the rest
      (processes, Node's fd_readdir on Windows, shared native bugs), stack
      switching for tasks, `fid wasm` in CI (scripts/ci/cross.yml runs the
      C path's wasm32-wasi before a release).
      Nov 18 (not yet run with a built compiler): `fid wasm` checks an
      index into a fixed-size array as the interpreter does (`Panic: Array
      index out of bounds`, not in `unsafe`); a typed fixed-size array
      binding (`var a: [i64; 3] = ...`) holds the array's pointer (lower.fd
      Lowerer_lower_let_stmt), so the interpreter reads it right and the C
      path compiles it (it emitted `int64_t[3] v = 0`):
      tests/bootstrap/test_array_typed_let.fd, test_wasm_array_index.fd.
      Tasks switch stacks under `fid wasm`; processes panic `not available
      in WebAssembly: processes (...)` on both paths; CI's wasm32 lane runs
      `fid wasm` (scripts/wasm-test.sh, tests/wasm/task-switch-check.sh,
      tests/wasm/stackify-check.sh). See competitive PL4.
- [~] CI6. **"Embedding the language in another app is hard"** (Python, JS). A
      C API to run Fides code with a capability-limited sandbox (L).
      Done Nov 16 (codegen_c.fd, on CI7's library build): `fides_call(name,
      caps, nargs, args, &result)` runs an export by name with only the
      capabilities in `caps` (FIDES_CAP_FS, _NET, _PROC, _ENV; a nested call
      can only narrow them); the C runtime's file, directory, socket,
      process and environment entry points test the mask (`__p_caps`,
      `P_CAP_DENIED`) and fail as on a missing file; a panic returns
      FIDES_E_PANIC with `fides_last_error()` instead of ending the host
      (setjmp/longjmp in p_panic); `fides_str_free` drops a returned string.
      tests/projects/embed (mathlib.fd, host.c, build.sh, expected.txt).
      Decision: the sandbox is the runtime mask, not the interpreter: the
      library is compiled code, and the compile-time `sandbox {}` checks
      stay as they are. Left: the driver's `--lib` (CI7); a thread or task
      spawned inside a call starts with every capability; memory a
      panicking call allocated is not freed.
      Nov 17: threads, I/O workers and tasks started inside fides_call keep
      its capability mask (p_thread_arg_t / p_io_arg_t / __p_co .caps); the
      blocks a panicking call allocated are freed (codegen_c.fd
      emit_embed_log_runtime, P_EL_* in fides_call); tests/projects/embed
      covers both. Pending a run of tests/projects/embed/build.sh.
- [x] CI7. **"Calling the language from others needs glue"** (Rust, Go). `fid build
      --lib` writes a C header and a shared library from `pub extern` functions (M).
      Done Nov 16: the marker is `@export` on a function (typechecker.fd
      whitelist; lower.fd collects the names in ir.fd `g_ir_exports`).
      codegen_c.fd's library mode (`CGen_lib_set(true)`): no C `main`;
      `fides_init` does main's setup (registrations, globals); each export
      gets a wrapper with its own C name and C types (integers, bool,
      double/float, `const char*` for str, `void*` for handles and structs
      by pointer; the Fides function becomes `p_<name>_fx`); a struct or
      array by value, or a name C already uses, is left out and listed
      (`CGen_lib_skipped`). `CGen_lib_header(name)` is the header,
      `CGen_lib_cc_flags(os)` the flags (-shared, -fPIC, -dynamiclib),
      `CGen_lib_file_name(name, os)` the file. test_cback_lib_export.fd,
      tests/projects/embed. Left: main.fd's `ccompile_lib` (a stub) must
      call these and link; `fid build --lib` on the native backends.
      Done Nov 16 (main.fd ccompile_lib): `fid ccompile <file> --lib [-o
      <lib>] [--export a,b] [--target <t>]` (and `fid build --lib`, which
      takes the C path) generates in library mode, writes `<stem>.h` beside
      the library, links with CGen_lib_cc_flags through gcc, clang or zig cc
      (cl.exe and tcc are refused), lists the exports left out, and reports
      a missing C symbol as CB9 does. Checked by
      scripts/driver-goals-check.sh (tests/projects/embed/build.sh) after
      the next build. Not done: a shared library from the native backends.
      Checked Nov 15: not started (no `--lib`, no export marker in the AST
      or IR, no header writer). Needs: the parser and IR to mark exported
      functions, codegen_c.fd to skip `main` and write exported wrappers
      (C types, no `__env`) plus an init for globals, main.fd's `--lib` to
      link with `-shared` and write the header.

## Track CP — Performance surprises

- [x] CP1. **"Debug builds are unusably slow"** (Rust, C++). `--debug` skips the
      mid-end but the emitted binary is still fast: measured 0.93x of release on
      a fib+loop benchmark (Nov 10), well inside 3x (M).
- [x] CP2. **"Hidden allocations"** (C++, Rust `clone`, Go escapes). `fid explain
      --alloc` lists every allocation site in each function with a why-on-the-heap
      reason (Vec buffer, string copy, COW share, dyn box, ...) (M, Nov 10).
- [x] CP3. **"I can't tell why the optimizer did or didn't"** (all). `fid explain`
      lists the runtime checks removed and kept per function, and which proof
      removed each (counted loop vs range analysis) (S, Nov 10).
- [~] CP4. **"Cloning everything to satisfy the compiler"** (Rust). `fid explain
      --alloc` lists each `Vec copy for COW sharing` site per function, so a
      clone the compiler inserts is at least visible; there is still no
      dedicated "clone required here because …" diagnostic (M).
- [ ] CP5. **"`async` costs more than threads would"** (Rust, C#). Tasks switch in
      under 20 ns and allocate nothing per await (competitive RT9) (L).
- [x] CP6. **"Bounds checks I can't get rid of safely"** (Rust, Go). The counted-loop
      proof, range analysis, AND `if i < v.len()` guard-fact propagation now
      remove provably-safe checks — `fid explain` shows `removed bounds check`
      on both shapes (verified Nov 11: 3/3 removed in the guarded block;
      kept when the index can be negative, when the vec is shrunk inside the
      guard, and when the index is an unproven param — `&&` guards stay
      conservative). peir_guard_ops in compiler/checks.fd; the may-shorten
      whitelist now covers destructors/to-string helpers (pe.rs + checks.fd).
- [x] CP7. **"Startup time is slow"** (Java, Python, JS). PE binaries have no
      runtime: hello measures at the raw C-binary floor (8.9 ms wall vs 9.9 ms
      for a trivial C exe — Windows process creation dominates; in-process
      startup is sub-ms) (S, measured Nov 10).
- [x] CP8. **"Binaries are huge"** (Go, Rust). `emit` hello = 32.5 KB (Nov 10);
      dead-function elimination keeps only reachable code (M).
- [x] CP9. **"Performance cliffs nobody warned me about"** (all). `fid lint --perf`
      warns on quadratic patterns: `str + str` in loops, `Vec::remove(0)`, linear
      lookups in loops — implemented in `lint_perf` (compiler/compiler.fd), with
      fix hints and per-statement line numbers (M, Nov 10).

## Summary

100 goals: 39 done, 31 partly done, 30 not started (Nov 15; stdlib: CO1 keys
`@secret` across the crypto modules plus env_secret/config_secret, CM10
SAFETY comments on every stdlib block, CT3/CL10/CO2 partly; frontend: CL4 named
arguments checked, CL5 struct field defaults, CL8 f-string values checked, CU3
`@deprecated` partly, CM2/CL2 verified done, CA6–CA8/CL6/CL7 verified partly;
CE3 verified, CT4/CT5
fuzz and snapshots in `fid test`, CB8 stale-cache script, CD6 typosquat warnings,
CU4 nightly policy, CU6 release-notes script, CU7 doctests in CI; Nov 11: CP1/CP7/CP8 verified
by measurement, CP9 `fid lint --perf`, CP2/CP3 `fid explain` implemented, CP6
done — counted-loop, range, and `if i < v.len()` guard facts all remove checks;
CP4 partly — `--alloc` shows COW copies). The first done ones were
where the language's design already answers the complaint: no lifetime
annotations, defined integers, no undefined behaviour in safe code, traces in
every build, `@secret`, debug and release in one flag, and a written stability
promise. CO7 added `@sensitive` on the same machinery: personal data reaches
outputs only through `privacy::release`'s runtime consent check. Tracks CM and
CE added: no null `str` from the stdlib, error context, the `unwrap` lint, and
partly borrow-error explanations, the leak report, the checked allocator,
`fid audit` and cascade suppression. Nov 4–6 added owned and borrowed strings
(CL3), `==` by content (CL9) and the compiler-inserted frees of CM7.

Complaints about learning and day-to-day tooling (out-of-date guides, editor
support, a playground) are goals of the other roadmaps: release N2, competitive
TL8 and TL10.
