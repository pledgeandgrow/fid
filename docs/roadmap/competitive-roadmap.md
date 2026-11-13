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
[complaint-roadmap.md](complaint-roadmap.md) starts from the other side: what
developers complain about in the languages they use today.

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

Measured on Nov 2 (Windows x64, seconds, lower is better, best of 3, every
column in the same run). BM1 makes this a published, repeatable harness.

| Benchmark | Fides native | Fides via C | Rust | Zig | Go | C++ |
|---|---|---|---|---|---|---|
| fib(40) | 1.34 | 0.81 | 0.53 | not measured | not measured | not measured |
| Arithmetic loop, 2 billion | 7.59 | 3.53 | 5.36 | not measured | not measured | not measured |
| Sieve, 20M | 0.82 | 0.48 | 0.40 | not measured | not measured | not measured |
| HashMap, 1M strings | 1.04 | 0.80 | 1.12 | not measured | not measured | not measured |

What changed on Nov 2: through C, fib went from 3.0 s to 0.81 s (no
call-stack bookkeeping per call) and the sieve from 0.64 s to 0.48 s (`push`
inlines to a store); natively, from 1.63 s to 1.34 s on fib and 1.47 s to 1.04 s
on the map. The remaining fib gap is the overflow check: Rust's release mode
drops it, which lets LLVM turn one of the two recursive calls into a loop.

Compile speed: `fid` compiles itself (158,516 lines of Fides, 67 MB of C out) in
74 s on Nov 9 when built at -O2 (229 s at -O0), about 2,100 lines a second;
about 133 s on Nov 6; `fidesc` (Rust) does it in about 21 s. Go and Zig
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

- [x] BM1. A benchmark repo: the same programs in Fides, Rust, Go, Zig and C++, one
      command to run them all (M).
      Done (Nov 3): `bench/` with fib, loop, sieve, hashmap in all five
      languages (Zig has no hashmap yet) and `bench/run.sh`: builds each, checks
      every language prints the same number, best of N, markdown table.
- [~] BM2. Install Go and a real Clang and GCC beside `zig cc`; pin every version (S).
      Done: Go 1.27.3 (winget); Clang through `zig c++`; run.sh prints the
      versions. Left: GCC, and pinning the versions in a file.
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
- [~] BM9. CI fails on a slowdown over 5%, per benchmark (release D7, tighter) (M).
      Done (Nov 4): `bench/check.sh` fails on a slowdown over `LIMIT_PCT` against
      `bench/baseline.txt`. Left: running it in CI.
- [ ] BM10. A public page of results over time, losses included (M).

**Acceptance:** anyone can reproduce every number with one command.

## Track OP — Optimizer

Today the native path keeps most values in stack slots (seven callee-saved
registers hold the hottest values), runs a small IR optimizer and leaves floats
in memory. This track is where most of the speed is.

**Foundations**

- [x] OP1. Run the IR optimizer on the native path (release D6) (M).
      Done (Nov 4): `fid emit` runs `compiler/iropt.fd` before code generation.
- [ ] OP2. Real SSA: promote stack slots to values (mem2reg) before any other pass
      (L).
- [ ] OP3. Dominator tree, loop tree and use lists shared by every pass (M).
- [ ] OP4. One pass manager, with per-pass timing and a switch to turn each off (M).

**Classic passes**

- [~] OP5. Inlining with a cost model, across modules (release D5) (L).
      Done (Nov 4, native path): a call to a function of one block of plain
      instructions, up to 16 instructions, is inlined. Left: a cost model,
      functions of several blocks.
- [~] OP6. Constant propagation, folding and branch elimination (M).
      Done (Nov 4, native path): integer arithmetic and comparisons on two
      constants are folded when the result cannot overflow. Left: propagation,
      branch elimination.
- [ ] OP7. Global value numbering and common subexpressions (M).
- [~] OP8. Dead code and dead store elimination (M).
      Done (Nov 4, native path): constants and other pure values nothing reads
      are removed. Left: dead stores.
- [~] OP9. Loop-invariant code motion (M). Done Nov 7 (native path,
      compiler/iropt.fd): a read of a local the loop never stores, `v.len()` of a
      Vec the loop cannot resize (it calls nothing but get/set/len, so value
      semantics say no other name changes its length), and comparisons and bit
      operations on such values move to the block before the loop. Left: the
      Vec's data pointer (needs the header loads in the IR), arithmetic that
      may trap.
- [ ] OP10. Strength reduction and induction variables (M).
- [~] OP11. Loop unrolling, peeling and rotation (M). Rotation done Nov 7
      (native): a block that jumps back to a header made only of the loop's test
      (loads, constants, integer operations, `len`) runs that test itself, so an
      iteration takes one branch. Left: unrolling, peeling.
- [ ] OP12. Scalar replacement of structs, and returning small structs in registers
      (L).
- [ ] OP13. Tail calls, tail recursion into loops (M).
- [ ] OP14. Devirtualization of trait calls and closures (M).
- [~] OP15. Block layout for hot paths, using likely branches (M). Native: no jump to
      the block laid out next; a branch inverts its condition to fall through.

**Back end**

- [~] OP16. A full register allocator over all registers, caller-saved included,
      live ranges split at calls (release D4 goes further) (L). Native keeps
      values in 7 callee-saved registers (release D4, D13), and since Nov 7:
      copy coalescing (a local's loads, the value stored into it and the left
      operand chain of that value share its register, so `i = i + 1` is one
      `add`), operand hints, and r10/r11 for any value whose live range has no
      call (a value may end at a call that reads its arguments first). Left:
      rax-rdx and r8/r9 (the emitter's scratch), live ranges split at calls.
- [~] OP17. Floats in xmm registers, not stack slots (L). Done Nov 7: f64
      values and locals get xmm6/xmm7 (callee-saved, all 128 bits saved) and,
      with no call in their live range, xmm2-xmm5; every move to and from a
      value goes through the encoder, which turns a register slot into a
      register move and refuses (a compile-time panic) any memory operand on
      one. Left: f32, float operations reading a register operand directly
      (they still go through xmm0/xmm1).
- [~] OP18. Instruction selection: `lea`, addressing modes, `cmov`, `test`, fused
      compare and branch (L). Done: compare and branch without a stored bool,
      `imul` by an immediate, short immediate loads, shadow space set once.
      Nov 7: `cmp r, imm` / `cmp r, r` on a register value, `test r, r` for a
      branch on one, three-operand `imul`, `lea` for add, sub and * 2/3/5/9
      where no check is needed, 8-bit immediates, `x % c` and `x / c` without
      moves through rax. The loop benchmark's body went from 30 instructions
      to 20. Left: memory operands, `cmov`.
- [ ] OP19. A peephole pass on the native path (M).
- [ ] OP20. Modern targets: BMI, AVX2, a `--cpu` switch, safe defaults (M).

**Vectorization**

- [ ] OP21. SIMD types and intrinsics that compile to real vector code (L).
- [ ] OP22. Automatic loop vectorization: SSE2, AVX2 (XL).
- [ ] OP23. Vectorized `memcpy`, `memchr`, string compare and hashing in the runtime
      (M).

**Where Fides can beat them: what the language proves**

- [~] OP24. Bounds and overflow checks dropped on counted loops. Done for
      `while i < v.len()`, `for i in 0..v.len()` and a length kept in a local, on
      the native and the C backends (release D12). Left: range analysis for every
      loop and branch (L).
      Nov 7 (compiler/checks.fd, both backends): integer ranges from
      constants, `% c`, `/ c`, `& c`, `>> c`, comparisons, narrow types and
      lengths; an add, sub or mul of two ranged values that cannot leave i64
      is unchecked and ranged itself (`(i % 7) * 3 - 1`). Left: ranges of
      locals across iterations and from branch conditions (the sieve's
      `m = m + p`).
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

- [~] RT1. `Vec<T>` stores `T` inline: a `Vec<u8>` takes one byte per element, and a
      `Vec<Struct>` no pointer per element (L). Stage A done: one `Vec`
      implementation on every backend (release D14).
- [~] RT2. `str` carries its length: `str_len` is O(1), zero bytes are allowed, and
      HTTP bodies can be binary (release I8) (XL, touches every backend).
      Done: 16-byte `[magic][len]` header — `p_str_alloc`/`p_str_free` in
      `stdlib/core.fd`, `p_str_length` reads `*(s-8)` when the magic at
      `*(s-16)` matches and falls back to a NUL scan otherwise, so
      non-headered strings (literals, C returns) still work and degrade
      gracefully. `Str_drop` frees `s-16` for headered strings only.
      `p_str_concat`, `String_slice_len`, `p_to_str`, `p_char_to_str`,
      `p_str_literal_dup`, `p_file_read`, `p_argv` and `p_getenv` produce
      headered strings on the `.fd` runtime; interp + PE + C verified
      identical output. Left: PE asm `p_str_concat` headering, `.rdata`
      literal headers (needs `Str_drop` to know statics aren't freeable),
      embedded-NUL literals, then the fat `{ptr,len}` ABI if we go all
      the way.
- [ ] RT3. Small-string storage with no allocation (M).
- [~] RT4. A fast `HashMap`: open addressing with SIMD probing, a strong default hash
      (L).
      Already in `stdlib/core.fd`: open addressing with linear probing,
      tombstones (`-1` slots), 75%-load growth, FNV-1a string hash (PE
      overrides with `pert_str_hash`'s word-at-a-time loop). Left: SIMD
      control-byte probe groups and a stronger finalizer (FNV mixes weakly
      in the low bits).
- [ ] RT5. `HashMap<K, V>` with unboxed keys and values (L).
- [~] RT6. Generics fully monomorphized, so a `Vec<f64>` holds floats as floats (L).
      Done (Nov 3, verified): `val x: f64 = v.get(0)` round-trips 1.5 — the
      monomorphized layout already stores floats as floats when the element
      type is known. Left: `print(v.get(0))` on an untyped `Vec::new()`
      resolves the element to `i64` and prints float bits as an integer —
      element-type propagation into `print` dispatch.
- [~] RT7. Allocator: per-thread caches (release H8), freed memory returned to the
      system (L).
      Done (written Nov 4): per-thread arenas and free lists in the C runtime,
      per-thread free lists in the native runtime; every native test runs
      through them, no multi-thread stress test has been run. Left: freed
      memory returned to the system.
- [ ] RT8. Allocator speed on par with mimalloc on its own benchmarks (L).
- [~] RT9. Own context switch, not fibers: under 20 ns per task switch (M).
      Done Nov 8 on Windows, both paths. Native: `pe_ctx_swap`
      (compiler/pe_ir_stubs.fd, the same bytes in src/pe.rs, checked against
      the assembler) saves the callee-saved registers, xmm6-15 and the TEB's
      stack fields; `stdlib/pe_runtime/conc.fd` lays out task stacks as
      Windows does for a thread (1 MB reserved, guard page) and keeps 256
      finished ones. C path: the same switch for x64 Windows under clang/gcc
      (cl.exe and tcc keep fibers). A yield, two switches plus the scheduler:
      native 8.7 us -> 0.23 us, C 283 -> 58 ns; 100,000 sleepers 56 -> 19 s.
      Two races of the native scheduler fixed with it: a task woken on another
      worker could be resumed before it had saved its registers (a context's
      busy word, cleared by the switch once on the new stack), and a worker
      inferred "it yielded" from a state a wake had already changed (the task
      now tells the context it returns to). Left: the switch alone under
      20 ns is not measured apart from the scheduler.
- [~] RT10. Tasks across all cores: M:N scheduling with work stealing (XL).
      Done: per-worker local queues — `pe_wq_push` targets the running
      worker's queue via `pert_tls_wid` (spawn locality); `pe_wq_pop` drains
      own queue → global → steals a sibling's head. All run queues share
      `pert_wq`'s lock; `pert_lq_any` feeds the deadlock detector, which now
      requires two consecutive dead-looking cycles before panicking (steal
      handoffs look empty for an instant). Verified: all 8 task/async tests,
      3× `test_tasks` runs, 100k-sleeper bench — no false deadlocks. Left:
      true Chase-Lev deques (per-worker locks), steal-half batching, and a
      contention benchmark to prove the locality win. Nov 6: programs built by
      `fid emit` that use `async` or `spawn` + `await` crash (`test_async`,
      `test_async_float_struct`); the interpreter and C paths are fine
      (release H10).
- [~] RT11. IOCP on Windows, epoll on Linux, kqueue on macOS, replacing the O(n) poll
      loop (L).
      Done Nov 8. Native Windows: a task waiting for a socket submits one AFD
      poll (`IOCTL_AFD_POLL` on the socket's base handle, as wepoll and mio
      do) completed on an I/O completion port the poller waits on; a deadline
      cancels it. No wait set is rebuilt or scanned (WSAPoll stays the
      fallback when `\Device\Afd` cannot be opened). C path: epoll on Linux,
      kqueue on macOS and BSD, one registration per wait (a reader and a
      writer can share a socket; a second waiter the same way uses the poll
      set); its connect no longer blocks the thread. 200 clients and a task
      per connection: native 0.37 s (was 1.3-7 s). Left: native Linux
      (`stdlib/linux_runtime`), the C path on Windows (WSAPoll), 1M idle
      connections not measured.
- [~] RT12. A timer heap, so 100,000 sleeping tasks cost nothing (M).
      Done on the PE path: `pe_runtime/conc.fd` arms deadlines in a min-heap
      (indexed removal, so finishing tasks can't dangle) and keeps fd/join/
      pool waiters in a separate registry — the poller no longer scans live
      task blocks to find expirations. `tests/bootstrap/bench_sleepers.fd`
      arms 100,000 sleeping tasks and awaits them all: passes (~56 s wall,
      dominated by fiber spawn, not polling). Full suite green on all three
      backends. Left: interp/C-path schedulers still use their own waits.
- [x] RT13. Growable task stacks, or checked stack limits with a clear panic (L).
      Done: a guard page under every task stack on all backends (native
      Windows: the system's own stack growth, through the TEB fields the
      switch carries; C: POSIX guard pages, Windows the same as native), and
      an overflow in a task is `Panic: Stack overflow`, status 1, as on the
      main thread (Nov 8).
- [ ] RT14. Zero-copy file and socket I/O where the system allows it (M).
- [x] RT15. Panics unwind cleanly, or abort by a documented rule (L).
      Done Nov 8: the rule is reference.md 8.2. A panic, in main or in any
      task or thread, prints `Panic: <message>` and the trace, flushes stdout
      and ends the process with status 1; nothing unwinds. Stack overflow and
      memory faults are panics with the same words on `fid run`, C and native
      (the C runtime's handlers, a vectored handler on Windows; the
      interpreter no longer goes on after a task's panic).
      `test_task_panic_fail.fd`.

**Acceptance:** binary-trees and k-nucleotide ahead of Go; 1M idle connections on
one machine; 100,000 requests/s on one core.

## Track CS — Compile speed

This is the race we can win outright, since no rival's speed comes for free.

- [~] CS1. `emit` of hello world under 0.5 s (release D9), then under 0.1 s (L).
      Done: ~360 ms (Nov 2, fidesc); ~370 ms internal build time today
      (`fid emit hello.fd --target x86_64-linux`, ~780 ms wall incl. process
      start; type check ~85, borrow ~170, mid-end ~170).
      Nov (this session, -O2 fid-dev, hello `--nat` with the CS2 artifact
      and the CS6 binary fn-cache both warm): **631 ms total** — frontend
      138 ms (stdlib deser 45, tc register 19, typecheck 25, escape+secrecy
      16, lints 6, mono 17, borrowck 0), lower 185, mid-end 256 (iropt 68 +
      rules 48 + egraph 140), native code 43 (alloc+emit 8 on cache hit).
      The trusted-core skip was extended to escape/secrecy report passes and
      the lint body-walk (typecheck 43→25, escape+secrecy 27→16, lints 12→6
      ms vs before the extension); all escape/secrecy expected-fail tests
      still fire and lint output is unchanged. Summaries still flow through
      the fixpoints — only trusted bodies' reporting is skipped.
      Left (the 0.1 s stretch): lower 185 + mid-end 256 are now ~2/3 of the
      time — both per-decl work on the merged stdlib that nothing caches;
      stdlib deser itself is 45. Frontend is no longer the bottleneck.
      Nov 8-9 (`fid`): native code only for what main reaches (and what it
      names), iropt on those only: hello's executable 127 KB -> 33 KB. With
      the compiler itself built at -O2 (split into 8 units, `split-c`) hello
      takes 1.7 s instead of 6.4 s (type check 0.36 s, borrow check 0.19,
      lowering 0.45, native code 0.09; `[prof]` lines on `emit`).
- [x] CS2. Cache the checked stdlib in a binary form, not re-parsed each build (L).
      Done: trusted-module fast path — the baked-in core.fd and backend runtime
      skip per-function body checks and error reporting (~26 ms off hello's
      frontend; FIDES_CHECK_STDLIB restores full checking). Comptime pass
      skips its 8+4 bounded re-clone iterations entirely when the program
      defines no `comptime pf` functions (stdlib has none): 91 ms → 14 ms on
      hello, ~77 ms off the frontend (196 → ~98 ms); emit 387 → ~301 ms.
      Nov 4 (release D9): `fid` type-checks `core.fd` once per (compiler,
      `core.fd`) pair; a stamp in the temp directory skips its bodies on later
      builds (`FIDES_CHECK_CORE=1` forces the check).
      Binary AST artifact (this session, `compiler/ast_ser.fd` + the
      `astcache_*` glue in compiler.fd): the merged core+runtime `Program`
      persists as a versioned binary file (magic, format version 2, the
      lib_hash fold over every source file's hash plus the compiler's own
      hash — the same key as the D9 stamp). The file stores two Programs:
      core alone (what `get_cached_stdlib`/`reach_keep_set` sees) and the
      merged program the pipeline consumes. ~1,700 lines of codecs cover all
      53 AST types (Type 28 variants, Expr 40, Pattern 7, Stmt 33, all 17
      declaration lists) — complete by construction, not a public-decl
      subset. Artifacts live in the temp dir named by the key hash;
      `FID_ASTCACHE=<file>` overrides. A source edit, a rebuilt compiler or
      a corrupt/truncated file misses and falls back to the parse path
      (verified: byte-corrupting the stored hash and truncating to 2–5 KB
      both reparse and re-save). On the miss path the artifact is written
      once; `p_file_write_vec`/`p_file_read_bytes` carry raw bytes since
      counted strings can't hold NULs.
      Measured (fid-dev at -O0, hello emit): `stdlib` mark 415 ms → ~210 ms
      (file reads + hashes + deser of the 619 KB artifact) and `runtime`
      618 ms → 0 ms — stdlib frontend ~1,033 ms → ~210 ms (~5x, ~820 ms
      saved). Emitted executables byte-identical hit vs miss on every test
      (hello, struct, test_borrowed_str). On the -O2 release compiler the
      same skip applies to a ~226 ms parse+merge — expect deser there of
      ~40-80 ms. The 210 ms that remains on hits is hash Source + file IO +
      the deserializer itself; shrinking it further is format work, not
      correctness.
      Left: nothing structural — the artifact is complete and verified
      equivalent. The <100 ms hello stretch and CS4's <10 s still need the
      mid-end side (CS6's per-function native artifacts are the backend
      half; the text cache format there costs ~20 s to load at -O0 and
      wants the same byte treatment).
- [~] CS3. Borrow checking linear in program size (release D8) (M). On the
      compiler itself: 16 s (Nov 6).
      Nov 8: the reference-escape check iterates its fixpoint only over the
      functions whose result can hold a reference (scalars, strings and
      declared types cannot), not every function. With the -O2 compiler the
      borrow check of the compiler is 5.4 s.
      Nov (later): `release_dead_borrows` no longer scans every live borrow
      per statement — borrows are bucketed by the statement after their
      last_use (`dead_at`), released in one batched pass with per-call
      key->index maps.
      Later: the remaining keyed scans are gone — borrow entries are
      tombstones (borrowers.clear() / borrower=None instead of Vec.remove,
      so positions never move) behind eagerly maintained maps: sb_ix /
      mb_ix / bo_ix for exact queries and sb_pfx / mb_pfx prefix-count maps
      for the "inclusive" queries (prefix = range, still not hashable as an
      exact key). `declare_var`'s per-var release went O(live borrows) ->
      O(1). Borrow-dense synthetic (500 vars, 1500 borrows): ~2.3 s -> ~135
      ms (~17x); borrowck of the compiler is ~0.16-0.6 s of a ~5 s check.
      All expected-fail borrow diagnostics byte-identical.
- [~] CS4. Self-compile under 60 s (release), then under 10 s (L). Nov 9: 74 s
      with the compiler built at -O2 (8 split units), against 229 s at -O0;
      the -O2 compiler emits the same C (fixpoint). Of the 74 s: type check
      18 s, borrow check 5, mono 5, lowering 24, C generation 20.
      Later (this machine): `dump-c compiler/main.fd` on the -O2 release
      compiler ~25 s wall (71.9 MB C) — the 60 s target is met; <10 s
      remains. With the same release binary, `check compiler/main.fd
      --time` reports type check ~23-30 s on a loaded machine (was ~29 s
      before the CS8 tables on a comparable run — noisy machine, treat
      ±20%). `fid-dev.exe` is -O0: expect ~11x inflation there.
      Next: the <10 s stretch — measured (this session, -O2 fid-dev,
      `dump-c compiler/main.fd`, 22.9 s): the premise that frontend work
      dominates is stale at -O2. Real split: frontend to mono ~5.7 s
      (imports ~1.3, typecheck ~2.9, escape+secrecy ~1.5), **lower 16.4 s +
      C codegen 6.1 s = ~22.5 s of the 22.9**. `FID_C_REACH=1` (new, opt-in)
      applies reach_keep_set to the C path — verified the filtered C still
      compiles and runs — but on the self-compile reach keeps ~97% of fns
      (73.6→71.4 MB C; lower unchanged): it only pays off on small programs
      (a `use vec` hello: −7.5% C). The honest lever for <10 s is a CS6-style
      per-function *lowered-IR* artifact (cache `IrFunction`s, not just
      native bytes), applied before C codegen — the frontend side is done.
- [~] CS5. Parallel front end and code generation on all cores (L).
      Done (borrowck slice, opt-in): `FID_PAR=N` splits the borrow check's
      function set across N workers (own `BorrowChecker` each, chunks merged
      in order — diagnostics byte-identical to sequential, verified on an
      80-function borrow-error file). Units pass as chunk bounds over a
      shared read-only `g_bc_units` (spawn args must be `@send`; `Vec<Func>`
      is not). The spawn table `p_thread_entries` was raised 2048 -> 8192 in
      both codegens — the compiler's own ~4.9k functions overran it
      silently. Measured: borrowck of the compiler 7.9 s seq vs 10.1 s par
      (-O0, loaded machine) — a net loss: borrowck is now a small share of
      check post-CS3, and OS-thread spawn + join overhead exceeds it. Left
      (the real targets): imports and escape+secrecy are the same per-unit
      shape and are the larger phases (imports ~1.3 s, escape ~1.0 s,
      typecheck ~1.3 s vs borrowck ~0.16 s on a quiet check); imports has
      name-set ordering constraints.
- [~] CS6. Incremental native builds per function, under 100 ms for a one-line
      change (L).
      Done: whole-output incremental caching on `emit`, `ccompile`, and `build`
      — `.fides-stamp` records compiler identity, output hash, and every dep
      file hash; unchanged inputs print "is up to date" and do no work, and a
      touched dep correctly invalidates. An in-memory L2 IR cache keys modules
      by source hash within a process. Left: per-function native code reuse —
      the artifact machinery now exists (below) but its text format makes
      the load dominate.
      Artifact cache (opt-in, `FID_FNCACHE=<path>`): per-function record of
      pre-resolve bytes + symbolic fixups keyed by a content hash of the
      whole `IrFunction` (instructions, blocks, value metas, flags) plus a
      module/config salt (layouts, globals, externs, trait tables, function
      set, arch, pass toggles, libfacts). On a key+salt hit the function is
      a synthetic record (same MIR length, trap/call counts, and call edges —
      so postorder and the cold splitter see the same shape); at emit its
      bytes replay at the new offset and fixups re-register against the live
      label space (fn names, string/global data, singletons, and lazily
      re-created panic-line stubs). Verified: miss and uncached emits are
      byte-identical (zero cost when off); hit emits are deterministic
      (converged images equal across runs) and run correctly — layouts can
      legitimately differ from all-fresh images since replays relink.
      Binary format (this session): the cache now uses ast_ser.fd's byte
      codec (AwBuf/ArBuf, `p_file_write_vec`/`p_file_read_bytes` — no
      NUL-through-str), version 2 with a `GBD`-magic header. Measured on the
      -O2 fid-dev, hello `--nat`: 288 KB artifact (was ~550 KB text), and on
      a warm hit `native code` 138→35 ms with `nat alloc+emit` 86→9 ms;
      repeated hits converge deterministically and the emitted binary runs.
      Left: cold-split fidelity is approximate (order can shift), cached
      functions get no line-table rows, and a one-line change still
      recompiles the whole binary (~0.5 s on hello vs the 100 ms target —
      the frontend+mid-end around the cache is the gap, not the cache).
      Nov 13 blocker analysis (fid-dev -O2, hello --nat, all caches warm):
      frontend 125 ms (astcache deser ~40 + typecheck 33 + borrowck 11 +
      mono 18), mid-end 237 ms, native-with-fncache ~30 ms. The mid-end is
      the next cache boundary but it is NOT per-function: iro_inline,
      iro_conv_mark, iro_ipcp/spec/envtrim (iro_pass_on defaults on) read
      and rewrite across the module — a callee change alters callers'
      optimized IR, so a pre-opt per-function key is unsound. Phase C
      needs call-graph transitive-closure keys (or a per-fn/module pass
      split) before hits can skip the mid-end.
- [x] CS7. A fast debug mode with no optimizer, and a release mode (S).
      Done: `--debug` (C at -O0) and `--release` (-O2) on `ccompile` in both compilers.
- [x] CS8. No linear scans in the self-hosted compiler: tables, not `Vec` searches
      (M). Done: instructions, functions, strings, defaults and comptime functions
      by table. Method lookup in the type checker is now
      `methods_index` (type -> method Vec) plus `methods_hm` (per-type
      method-name -> entry, lazy, dropped on Vec rewrite) — call sites
      `x.m()`, `T::f()` and the `&mut self` probe all O(1). Parser:
      `alias_set`/`g_import_set`/`struct_name_set`/`enum_name_set` HashMaps
      replace the alias, import and struct-name Vec scans; lexer keywords
      already dispatch by first char. Typecheck of the compiler measured
      ~23 s vs ~29 s before on a loaded machine.
      Remaining by nature, not oversight: `shared_borrows`/`mut_borrow`/
      `borrow_of` field-prefix "inclusive" queries (a prefix is a range,
      not a hash key), tiny cold lists (reserved borrows, scope stacks),
      and the parser's sequential `token_kind_eq` chains (O(1)
      discriminant reads, not Vec scans).
- [x] CS9. `fid check`: type errors with no code generation, under 50 ms on an edit
      Done: `fidesc check <file.fd>` runs parse + register + type/borrow/effect
      checks only and exits — hello world reports in 43 ms.
## Track VF — Verification, in place of years

This track is what replaces production years. It runs for as long as the
language lives.

**Finding compiler bugs**

- [~] VF1. A random program generator in the spirit of Csmith: all three backends
      must agree (release M9 is the start) (L).
      Done (Nov 3): `scripts/fuzz/gen_programs.py` — seeded programs with
      arithmetic, branches, loops, calls and `Vec`s, run on `run`, `emit` and
      `ccompile`; disagreements kept with each output. 40 programs, 0
      disagreements. `scripts/fuzz/nightly.sh` and a `checked` backend
      (release M9) written Nov 4, not run; no CI workflow schedules it. Left:
      structs, enums, strings, floats, and long runs.
- [~] VF2. Random programs compared with an equivalent C program, built by Clang and
      GCC (L).
      Done (Nov 7): `scripts/fuzz/gen_vs_c.py` writes each program twice from
      one tree, in Fides and in C (the eight integer types with checked and
      `@wrapping` arithmetic, casts, shifts, the §4.1 precedence printed with
      the fewest parentheses, if/while/for/match, `if` values, structs,
      `Vec<i64>`, helper calls); the C is built with Clang (`zig cc` -O0/-O2)
      and GCC 15 (WSL, -O0/-O2), which must agree, then `run`, `ccompile` and
      `emit` must match. `triage.py` groups failures, `reduce.py` shrinks one.
      Run (seed 100, 60 programs): 47 agree, 12 differ, 1 C-oracle flake.
      Found M1 (same-width casts are the identity in run/emit), M7 (emit
      does not sign-extend a narrowing cast; seeds 103 and 137), M2 (nested
      `if` value typed void), M3 (println of a u64), E1, and the unreduced V1
      class (spurious and missed overflow panics in run/emit); see
      docs/verification/miscompile-log.md. Left: reduce V1, floats, strings,
      enums, long runs, CI.
- [~] VF3. Coverage-guided fuzzing of the lexer, parser, typechecker and every
      optimizer pass; no input may crash the compiler (L).
      Done (Nov 7): `scripts/fuzz/fuzz_frontend.py` mutates the suite (token
      edits, splices, cuts, deep nesting, odd bytes) through `fid check`; a
      crash, internal panic or hang is a finding (re-run to confirm); with
      `--cov-fid` (a compiler with VF8's counters) it keeps inputs that reach
      new IR blocks. Run: 1200 inputs, mutational (no coverage compiler built
      yet): one bug, F1 — no nesting limit in the parser, ~3000 nested
      brackets crash `fid` with a stack overflow. Left: the coverage-guided
      run, the optimizer passes (through `emit`), long runs.
- [~] VF4. Run every optimizer pass on and off over the whole suite: output must not
      change (M).
      Done (Nov 7–8): `scripts/verify/variants.py toggle` — the iropt passes
      off at run time (`FID_IROPT_OFF`), the `PEIR_*` constants off in
      compilers rebuilt by `scripts/verify/build-variant.sh`; each test is
      built by both compilers, run when the binaries differ, and compared.
      Run (emit): inline, fold, dce, tailcall, gvn, licm each off over 53
      programs (the VF5 tests, 24 sampled suite tests, 20 generated): no
      output changed (two "builds with one compiler only" were a build
      flake under load, identical when rebuilt alone). Not run: the six
      `PEIR_*`/`PEIR_RANGES` variants — `dump-c compiler/main.fd` with the
      Nov 7 compiler ran out of memory each time it was tried beside the
      other sessions' work. Left: those six, the whole suite, CI.
- [~] VF5. Each optimization has a test that fails if the pass is wrong (M).
      Done (Nov 7): `tests/bootstrap/test_opt_{fold,inline,dce,gvn,tailcall,
      licm,ranges,ranges_pop_fail,regalloc}.fd`, each a program whose output
      changes when its pass is wrong (unsigned folds, argument order, values
      reused across writes, zero-trip loops, swaps live across calls),
      checked against the interpreter, which runs no optimizer. All pass on
      run, ccompile and emit. Writing them found M4 (the C path drops the
      bounds check of `0..v.len() + 1`) and M5 (array literals). VF7 checks
      that the tests catch broken passes. Left: a test per `PEIR_*` switch.
- [ ] VF6. Translation validation: a checker proves each optimized function matches
      the original, on a chosen set of passes (XL).
      Scoped (Nov 7): docs/verification/translation-validation.md — witnesses
      from each iropt pass checked by a separate small checker, SMT for
      straight-line code. Nothing built: it needs a `--validate` mode in
      `compiler/iropt.fd`. Open.
- [ ] VF7. Mutation testing of the compiler: a broken pass must fail a test (L).
- [ ] VF8. Line and branch coverage of `compiler/*.fd` over 90%, reported in CI (L).

**Proving behaviour**

- [~] VF9. A written formal semantics of the core language (XL).
      First piece (Nov 7): docs/verification/semantics.md, a big-step
      semantics of the integer core (types, checked/wrapping arithmetic,
      casts, shifts, short circuit, `if` values, loops, match, calls,
      structs, `Vec<i64>`), each rule tied to its reference section; its
      arithmetic rules are the C prelude of `gen_vs_c.py`, so VF2 tests the
      compiler against it. Left: floats, strings, enums, generics, closures,
      references, effects, concurrency, memory; mechanisation.
- [~] VF10. A conformance suite of a thousand-plus tests, one per sentence of the
      reference, as Test262 is for JavaScript (L).
      Started (Nov 7): `tests/conformance/` — 104 tests, one sentence each
      (§1–8 and §13; the file name is the section), the expected result
      taken from the reference, run on all backends by
      `scripts/verify/expect.py`. 96 pass. Failing: K1 (keyword-named
      functions cannot be called in an expression, 2 tests), M2, M3, M5/M6
      (array literals; emit does not bounds-check arrays), D1 (§8.1's shared
      Vec example contradicts proposal 0004). Left: ~900 more sentences.
- [~] VF11. A proof of the memory-safety claim on a small core calculus (XL).
      First piece (Nov 7): docs/verification/memory-safety-core.md — the
      calculus of proposal 0004's ownership rule, its invariant and a
      pen-and-paper preservation/progress sketch, with the five compiler
      checks it rests on (escape check, borrow checker, the lowering's
      dup/drop insertion, thread copies, collection drops). Left:
      mechanising it, proving the lowering is that translation,
      copy-on-write, structs/enums/closures/regions/Rc.
- [~] VF12. Reproducible builds: the same input gives the same binary on any machine
      (M).
      Done (Nov 7): `scripts/verify/repro.sh` builds each program twice
      (another directory, temp folder and time) and compares bytes. Run (3
      programs): `dump-c` and `emit` byte-identical; `ccompile` executables
      differ in 20 bytes (R1: PE timestamp, PDB GUID/path from the linker;
      `SOURCE_DATE_EPOCH=0` fixes the timestamp with `zig cc`). Left: fix R1,
      a `COMPILER=1` run, a second machine.
- [~] VF13. A second bootstrap path checked regularly: fixpoint from a clean tree (S).
      Written (Nov 7): `scripts/verify/clean-bootstrap.sh` — `git archive
      HEAD` (or `--worktree`) into a new directory, cargo builds the Rust
      compiler there, `scripts/bootstrap.sh` 3 stages with the fixpoint check,
      stage 3 runs hello. Not run: the machine had no memory for three more
      stages beside the other sessions' builds. Left: run it, schedule it.

**Hardening the runtime**

- [~] VF14. AddressSanitizer, UBSan and ThreadSanitizer through the C path, in CI
      (release M8 is the start) (M).
      `scripts/sanitize.sh` (the tests through C under ASan and UBSan; needs
      clang with ASan, Linux or macOS) written Nov 4, not run. Left: running it,
      ThreadSanitizer, CI.
- [~] VF15. Fuzz every stdlib parser: JSON, regex, HTTP, TLS records, TOML, XML, CSV
      (L).
      Done (Nov 7): `scripts/fuzz/fuzz_parsers.py` with the driver
      `tests/fuzz/parsers.fd` (json, regex, http, csv, toml, xml, url, uuid).
      Runs: 8000 inputs (two seeds), 3 bugs — P1 `url_parse` overflow panic
      on a long port, P2 regex `a{39…9}` overflow panic, P3 `xml_parse` on a
      stray `<` runs out of memory or hangs. JSON, HTTP, CSV, TOML, UUID: no
      finding. Left: TLS records
      (the stdlib has no record layer yet), long runs.
- [~] VF16. Week-long soak tests of a service under load, with flat memory (release
      I6 is 24 hours) (M).
      Done (Nov 7): `scripts/verify/soak.py` runs `tests/verify/soak_service.fd`
      (HTTP head parse, JSON, a bounded cache) and fits the memory slope of
      the second half. Run: fails at once — L1, ~3.9 KB leaked per request on
      the C path, out of memory after ~1 M requests (15 s); `fid run --leaks`
      names the sites. Left: a 24-hour then week-long run once L1 is fixed,
      a real socket server under load.
- [~] VF17. Chaos tests: dropped connections, full disks, out of memory, clock jumps
      (L).
      Done (Nov 7): `scripts/verify/chaos.py` + `tests/verify/chaos.fd`.
      Run: dropped and reset connections, half responses, a closed stdout and
      missing files are handled on ccompile and emit; findings C1 (no read
      timeout: a silent server hangs the client), C2 (emit `read_file` of a
      missing file panics), C3 (emit names a stack overflow "invalid memory
      access"), C4 (Linux: `file_append` reports success when the disk is
      full), C5 (out of memory aborts with SIGABRT). Left: clock jumps
      (libfaketime or a VM), partitions between machines.
- [~] VF18. Sharded tests on thousands of task and thread interleavings (L).
      Done (Nov 7): `scripts/verify/interleave.py` runs
      `tests/verify/interleave.fd` (atomics, a mutex, a channel pipeline,
      parallel branches) on random CPU sets, with and without load, sharded
      over workers. Run: 300 runs each on ccompile and emit, one result, all
      totals right. Left: thousands of runs, async tasks, a controlled
      scheduler (it perturbs the OS scheduler; it does not choose switches).

**Learning from every bug**

- [x] VF19. A public miscompile log: cause, how it escaped, the test that now
      catches it (S).
      Done (Nov 7): docs/verification/miscompile-log.md — the bugs this track
      found (wrong code, wrong rejections, a compiler crash, parser, runtime
      and reproducibility bugs), each with a repro in `tests/verify/bugs/`,
      and the earlier miscompiles of AGENTS.md (none of which names its
      regression test: open under VF20). Tools: docs/verification/README.md.
- [x] VF20. Every bug fixed gets a regression test, always (S).
      Done: the rule is in CONTRIBUTING.md § Compiler changes.

**Acceptance:** a month of fuzzing on all cores finds no miscompile.

## Track LG — Language depth

Reach what Rust, Go, Zig and C++ programmers expect on day one.

- [~] LG1. Moves or counted handles decided and enforced (release M4, M5) (L).
      Decided (M4, proposal 0001): copy-on-write for `Vec`, `str` and
      `HashMap`; `==` by content is done. Left (M5): the copy-on-write lowering
      is behind `LOWER_COW`, off, because the compiler compiling itself runs out
      of memory with it on; today `var b = a` on a `Vec` still aliases.
- [x] LG2. Owned and borrowed strings (release M11) (L).
      Done (Nov 4–6): `str` is owned, `&str` borrowed; a borrowed string cannot
      be returned as `str`, stored, or passed to a user function's `str`
      parameter; `str_clone(s)` copies. A `@scoped` struct cannot be stored or
      sent. `http_serve_scoped` frees request memory after each response and is
      no longer `@unsafe`. Tests: `test_borrowed_str`,
      `test_borrowed_str_return_fail`, `test_borrowed_str_pass_fail`,
      `test_scoped_store_fail`.
- [ ] LG3. Lifetimes the borrow checker can prove across functions, without
      second-class limits where that matters (XL).
- [~] LG4. Traits: bounds checked, associated types, default methods, trait objects
      (release K9 is the start) (L).
      Done (release K9, in `fid`): every bound of `T: A + B` is checked at the
      call; a type parameter is inferred or an error; turbofish with an arity
      check (`test_generic_bound_fail`, `test_generic_infer_fail`,
      `test_turbofish_arity_fail`). Left: associated types, default methods
      and trait objects, not checked against this goal.
- [ ] LG5. Zero-cost iterators: `map`, `filter`, `fold` compile to a plain loop (L).
- [ ] LG6. Closures that capture by reference and by value, with no heap allocation
      when they do not escape (L).
- [ ] LG7. Const generics (M).
- [ ] LG8. Sum types and pattern matching with guards, nesting and exhaustiveness in
      every position (M).
- [ ] LG9. Hygienic macros, or a comptime that covers what they do (L).
- [x] LG10. Error handling with context and no allocation on the happy path (M).
      `Result.context(msg)`, `with_context(f)`, `Option.context(msg)` (complaint
      CE1).
- [ ] LG11. Structured concurrency with cancellation and timeouts (L).
- [ ] LG12. A defined memory model for atomics (L).
- [ ] LG13. Inline assembly on every native target (M).
- [~] LG14. `async` for every argument type, and async traits (release H9) (L).
      Done (H9): an `async fn` with `f64` or struct arguments or results runs as
      a task on the interpreter and C paths. Left: reference arguments, native
      async (`fid emit` programs using `async` crash), async traits.
- [ ] LG15. C interop without bindings: `@import("header.h")`, as Zig does (L).
- [ ] LG16. C++ interop for the common cases (XL).
- [x] LG17. Every annotation works or is removed (release K8) (M).
      Done: `@bounded-stack`, `@no-indirect` (rejects calls through function
      parameters) and `@tainted` (a tainted function or parameter reaches no IO
      or Net effect; no data flow in `fid`). The 17 safety rules each have a
      negative test: `docs/reference/safety-rules.md` (release L1, L2).
- [x] LG18. Effect-row polymorphism: functions with `fn`-typed parameters are
      effect-polymorphic, instantiated per call site (M).
      Done (Nov 3): `apply(pure_fn)` is pure, `apply(io_fn)` gets `IO`, closures
      are scanned directly, unknown function values fall back to all effects.
      (`fn_param_positions` + `instantiate_fn_arg_effects` in `src/typechecker.rs`;
      tests `effects/test_effect_ho_instantiation.fd` and
      `test_effect_ho_violation.fd`.)
- [x] LG19. Explicit `!E` effect rows in signatures (S).
      Done (Nov 3): a declared effect name that is not a concrete effect
      (`!E`, `!{IO, E}`) marks the function effect-polymorphic — the row is
      instantiated at call sites and under-declaration is not an error for
      row-annotated functions.

**Acceptance:** each feature in the comparison table has a conformance test.

## Track SL — Standard library depth

Go's lesson: a deep stdlib replaces a thousand packages. It suits our rule of
owning what we ship.

- [ ] SL1. HTTP/2 client and server (L).
      `stdlib/http.fd` + `stdlib/http_client.fd` are HTTP/1.1 only
      (Content-Length + chunked, keep-alive, task-per-connection); no
      HPACK, frames, or streams exist.
- [ ] SL2. HTTP/3 and QUIC (XL).
      Unstarted — also gated on real TLS 1.3 (SL4) since QUIC carries
      its own handshake.
- [x] SL3. WebSockets, committed and tested (M).
      Done: `stdlib/ws.fd` (RFC 6455: the Upgrade handshake with its SHA-1
      and base64 key, frames with masking, ping/pong, close), `test_ws.fd`
      checked against Python on C and native. Nov 9: wss:// (`stdlib/wss.fd`:
      `wss_connect`, `wss_accept`, `ws_connect_url`, over `tls`, which `ws`
      never links), fragmented messages reassembled with a ping in their
      middle answered, the handshake no longer reads past its headers, and
      binary frames sent byte-exact; `test_ws_local.fd` (a server and a
      client over IPv6 and IPv4) and a TLS round trip with a self-signed
      certificate on C and native. Left: permessage-deflate.
- [~] SL4. TLS 1.3 on every platform (release I7) (M).
      Verified live: `tls_connect13("github.com", 443)` over Schannel
      negotiates SP_PROT_TLS1_3_CLIENT (8192, checked via
      tls_session_protocol) — the SCH_CRED + TLS_PARAMETERS credentials
      path is in (`tls_client13`/`tls_connect13`/`ts_acquire13` pin
      grbitDisabledProtocols = ~SP_PROT_TLS1_3; the legacy
      grbitEnabledProtocols field can't express 1.3). Default
      tls_client keeps system negotiation (TLS 1.2 on some servers).
      Unix side goes through OpenSSL (1.3-capable). Left: TLS 1.3 on
      the server-credential path (paSecret vs paPublic) + a
      suite-visible handshake check (needs network).
- [~] SL5. DNS with caching, IPv6 everywhere (M).
      Done: `stdlib/dns.fd` parses every answer record with its TTL
      (bounds-checked names), A and AAAA, IPv6 text as RFC 5952, a cache by
      TTL (`dns_cache_resolve`); `test_dns.fd`. Nov 9: IPv6 sockets on the C
      path and native Windows: a listening socket takes IPv6 and IPv4 clients
      (dual stack), connect tries each address of a name, IPv6 ones included.
      Left: native Linux (its resolver reads only IPv4 and /etc/hosts).
- [~] SL6. JSON at simdjson-class speed (L).
      Measured (5.9 MB nested doc, PE emit): `json_is_valid` ~10 MB/s,
      `json_parse` (validate + canonicalize) ~1.6 MB/s — three orders of
      magnitude from simdjson-class. `j_canon` now writes into one
      StringBuilder and sorts member indices instead of copying value
      strings (was `out = out + ...` per member); canon is linear at
      ~6× the validate pass. Left: an indexed DOM (one scan, no
      canonicalize reserialize, zero-copy `str_slice` values) is the
      only route near the goal; the `str`-in/`str`-out API reserializes
      by design.
- [x] SL7. A linear-time regex engine (L).
      Done: `regex_match` in `stdlib/regex.fd` is a Pike VM (no
      backtracking): classes, escapes, counted quantifiers, groups,
      alternation, anchors; `test_regex_linear.fd` runs a pattern that is
      exponential for a backtracking engine in linear time.
- [~] SL8. Time zones, calendars, formatting and parsing (L).
      Done: `stdlib/tz.fd`: 57 named zones with their current DST rules (US,
      EU, southern hemisphere), fixed offsets ("+05:30"), local to UTC and
      back (a skipped hour moves forward, a repeated one takes the first),
      `tz_format` with %Z %z, ISO 8601; `test_tz.fd` checked against Python
      zoneinfo. Nov 9: the IANA database itself: `tz_from_tzif` reads TZif
      files (RFC 8536, versions 1 to 4) with every past transition and the
      file's POSIX rule after the last one (`tz_from_posix`, negative DST
      and 24:00 change times included); `tz_load` finds the system's files
      ($TZDIR, /usr/share/zoneinfo) and falls back to the table;
      `test_tzif.fd` (Kolkata, Phoenix, Lord Howe, New York since 1883)
      against zoneinfo on the same files. Left: calendars other than the
      Gregorian, the Julian forms of POSIX rule dates.
- [x] SL9. Unicode: normalization, segmentation, case folding (L).
      Done: `stdlib/unicode_data.fd` (generated from Unicode 16, packed
      fixed-width-decimal tables — const arrays re-evaluate per access on
      the interpreter and `[i64; N]` by-value params truncate on emit,
      known-bugs §2g.23): canonical + compatibility recursive
      decomposition, canonical combining classes, canonical composition
      (composition exclusions filtered via NFC oracle). `stdlib/unicode.fd`:
      `uni_nfd`/`uni_nfkd`/`uni_nfc`/`uni_nfkc` with Hangul algorithmic
      decomposition/composition, canonical ordering, and the UAX#15
      blocked rule; grapheme segmentation and case folding already there.
      `test_lib_unicode.fd` covers NFD/NFC/NFKD/NFKC incl. reordering,
      Hangul both ways, compat ligatures, the 18-cp ﷺ case and
      composition exclusion U+095B; a 400-string × 4-form differential
      run against Python unicodedata passed 1600/1600 on interp and emit.
- [x] SL10. Compression: gzip, zstd, brotli decode (L).
      Done: `stdlib/compress.fd` inflates DEFLATE, zlib and gzip (fixed and
      dynamic Huffman) and writes them (stored blocks); `test_compress.fd`
      decodes streams made by Python's zlib. Nov 9: `stdlib/zstd.fd`
      (RFC 8878: raw, RLE and compressed blocks, Huffman and FSE, repeat
      offsets, skippable frames, the XXH64 checksum; no dictionaries) and
      `stdlib/brotli.fd` (RFC 7932 complete: context modelling, the static
      dictionary in `brotli_dict.fd` with all 121 transforms), both in
      Fides, so native builds have them; `test_zstd.fd`, `test_brotli.fd`
      on all backends, against streams of Python's encoders (522 brotli
      streams and 1,800 corrupted ones decode as the reference decoder
      does; 39,000 corrupted zstd streams never panic). Left: compressing
      encoders for zstd and brotli.
- [~] SL11. Encoding: base64, hex, CSV, XML, YAML, TOML, protobuf (L).
      Done: CSV (RFC 4180, `test_csv.fd`) and the protobuf wire format
      (`stdlib/protobuf.fd`: varint, zigzag, fixed, length-delimited,
      packed, skipping unknown fields; `test_protobuf.fd` against bytes from
      Python). base64/hex (`encoding`), XML, YAML and TOML modules exist.
      Left: tests for XML, YAML and TOML against their specifications.
- [~] SL12. Database drivers: PostgreSQL, MySQL, SQLite, with parameter binding (L).
      Done: `stdlib/sqlite.fd` binds the system sqlite3 (`SQLite::open*`,
      exec/query → JSON rows) and implements `db.fd`'s `Database`
      capability; `SQLite::query_params` binds parameters through
      `sqlite3_bind_text` — verified on emit that `' OR 1=1 --` stays a
      literal (`test_pe_sqlite.fd`). `db.fd` also ships an in-memory
      `MemoryDB` for tests. Left: PostgreSQL and MySQL wire-protocol
      drivers behind the same capability.
- [~] SL13. Built-in testing: table tests, benchmarks, fuzzing, as in Go (M).
      Done: `fid test <file.fd>` runs each `fn test_*()` in its own process and times each `fn bench_*()`. Left: fuzzing.
- [x] SL14. Structured logging and metrics (M).
      Done: `metrics` (counters, gauges, histograms, Prometheus text format,
      `metrics_serve`); `log` with fields, JSON lines (`log_render_json`) and
      per-module levels (`logmf`, `log_set_module_level`), tests `test_log`,
      `test_log_module` (complaint CO3, CO4).
- [ ] SL15. A sampling profiler reachable from a running service (L).
      `stdlib/profiler.fd` is a scope profiler (caller-driven
      begin/end regions, frame/GPU timing) and `fidesc profile` is
      one-shot; nothing samples a running service or exposes stacks
      over a control channel. Investigated (Nov 11) whether the pe_runtime
      green-task scheduler can sample other tasks' PCs: a parked task's
      context keeps only [saved RSP, stack base] — its stack can be walked
      from there (pe_runtime/conc.fd §"Task contexts"), giving a
      WAIT-profile of where tasks yield or block. CPU-time sampling needs
      suspending an OS worker mid-run (SuspendThread + GetThreadContext on
      worker handles, which the scheduler does not currently export) —
      real L work in the runtime, not a stdlib add-on.
- [x] SL16. Big integers and decimals (M).
      Done: `stdlib/bigint.fd` (`test_bigint.fd`) and `stdlib/decimal.fd`:
      exact decimals on a BigInt mantissa, division and rounding to a scale
      with six rounding modes (half-even by default); `test_decimal.fd`.
- [~] SL17. Files, paths and processes complete on every platform (M).
      Done: `stdlib/path.fd` reads both separators, Windows drives and UNC
      roots in join, split, normalize, parent and components
      (`test_path.fd`). Left: files and processes checked the same way.
- [~] SL18. Crypto through reviewed libraries, the set of release J5 (L).
      Done: `test_crypto_tls_vectors.fd` proves the primitives against
      published vectors — ChaCha20 block/stream (RFC 8439 §2.3.2/§2.4.2),
      Poly1305 (§2.5.2), ChaCha20-Poly1305 AEAD encrypt+decrypt (§2.8.2),
      HKDF extract/expand (RFC 5869 case 1) — plus the existing NIST
      SHA-256 and RFC 4231 HMAC vectors; fixed `aead_mac` interleaving
      the length blocks (was unverifiable until the interp struct-assign
      bug was found). Left: signatures (no Ed25519/ECDSA/RSA anywhere),
      and "reviewed libraries" — today's set is pure-Fides; J5 wants
      bindings (CNG/OpenSSL) for the same primitives.
- [~] SL19. Every public function documented with an example that runs in CI (L).
      Done: `scripts/doctest.py` extracts `/// Example:` code blocks from
      .fd sources and runs each as a program through `fid run` — CI-gatable
      (exit 1 on any failure); `(native only)` marker skips FFI examples on
      the interpreter. Measured: 6210 pub items scanned, 6 examples, all
      pass (arena, db, the four uni_n* normalizers). Left: writing examples
      for the remaining ~6200 items — a content task, not a tooling task.
      examples (`scribe` renders docs only).
- [x] SL20. OS-development primitives: firmware tables, buses, memory (M).
      Done (Nov 10): the parse/layout halves a kernel needs, pure Fides so
      they run anywhere — `elf.fd` (ELF64 ehdr/phdr/shdr + names), `acpi.fd`
      (RSDP scan, checksums, SDT headers, MADT/MCFG), `pci.fd` (config
      decode, BAR classify, capability list, CF8), `virtio.fd` (split-queue
      layout, desc/avail/used rings, free chains), `multiboot.fd` (MB2 tags,
      e820 map, framebuffer), `fdt.fd` (Device Tree), `uart.fd` (16550),
      `scancode.fd` (PS/2 set-1), `pmm.fd` (bitmap PMM + x86-64 paging),
      `slab.fd`. `test_os_stdlib.fd` passes on interp and native. Port and
      MMIO access stays platform driver code by design.
- [x] SL21. AI weight-path and inference kernels (M).
      Done (Nov 11): `safetensors.fd` — the standard HF weight format:
      u64 header length, JSON map, per-tensor dtype/shape/data_offsets
      validation, aligned-buffer reads; f16/bf16/f32 decode in pure
      Fides bit math (the interps have no f32-from-bits primitive).
      `bpe.fd` — vocab.json + merges.txt tokenizer (encode/decode).
      `infer.fd` — the transformer kernels on flat Vec<f64>: matvec,
      RMSNorm, RoPE, stable softmax, argmax, top-k, top-p sampling,
      single-head attention. `noise.fd` — Perlin/fBm/Poisson-disk for
      procgen. `test_ai_stdlib.fd` passes interp + native. A full
      forward pass still needs the glue (embedding lookup, per-layer
      loop, kv_cache wiring) — the pieces are verified, the model loop
      is the next step.
- [x] SL22. Game content pipeline: glTF + Tiled + noise (M).
      Done (Nov 11): `gltf.fd` — .glb container framing (magic/version/
      chunks), accessor resolution to a struct (once per accessor, not
      per element — per-lookup JSON is unusable at real model sizes),
      every componentType and VEC/MAT arity, byteStride, normalized
      ints; feeds mesh.fd's Mesh (positions/normals/uv/indices).
      `tiled.fd` — TMJ maps: tile layers into tilemap.fd with an
      optional solid layer, object groups (point/rect/tile + custom
      properties), flip-flag gids. `test_gaming_stdlib.fd` passes
      native; interp-skipped (each json_* call revalidates the doc —
      O(n^2) per lookup on the interpreter). mesh.fd grew
      texcoord_create (cross-module struct literals don't parse);
      mesh/asset/data moved off the nonportable
      p_str_take(p_file_read()) onto core's file_read.

**Acceptance:** a real web service needs no package outside the stdlib.

## Track TL — Tooling

Go and Rust won developers with tools as much as with languages.

- [~] TL1. One binary, `fid`: build, run, test, fmt, doc, lint, bench, fuzz (L).
      Done: `fid` has build, run, test, fmt, doc, lint, bench, check, profile. Left: fuzz (VF).
- [~] TL2. Error messages as good as Rust's, each with a suggested fix, and a test
      per error (L).
      Done (Nov 3): every diagnostic in `typechecker.rs`/`borrowck.rs` now carries
      a `Fix:` or `Hint:` line with a concrete code suggestion — borrow
      (`holder_note`), capability (`use mod with { Cap }`), region escape,
      use-after-free, budget, sandbox, undefined variable, effect mismatch.
      Left: a `// expect-error:` conformance test per diagnostic, source-span
      rendering (caret under the token) for the remaining line-only errors,
      and a diagnostic-code index. `tests/bootstrap/diagnostics/` has 9
      conformance tests (capability, cap-propagation, effect, budget, escape,
      sandbox, undefined-var, borrow-mutation, linear) covering every family.
      Diagnostic codes are now assigned systematically: `classify_error` maps
      every message family to a stable `Exxx` (region E052–E053, budget E054,
      GPU/KV region E055–E056, annotation E057, escape E058, secrecy E059) and
      `enrich_errors` tags code-less errors too — E090/E091 now render on
      capability and sandbox failures.
      Left: caret spans for the remaining line-only messages (some sites carry
      no position at all) and self-hosted `compiler/*.fd` parity.
- [ ] TL3. `fid fix`: applies suggested fixes, and upgrades code between versions (L).
- [ ] TL4. LSP complete: references, rename, formatting, signature help, inlay
      hints, code actions (release N7 is the start) (L).
- [~] TL5. Debugger support with line tables on every platform (release N9) (L).
     Done: `-g`/`--debug` on `fidesc emit`/`build`/`ccompile` — PE gets
     `.debug_abbrev/.debug_info/.debug_line/.debug_str` (DWARF v4, per-instruction
     line tables); the C backend emits `#line` back to `.fd` source and passes
     `-g` to the C compiler. Left: port DWARF emission to `compiler/*.fd`
     (self-hosted `fid` doesn't emit it), variable/location DIEs, ELF/Mach-O.
- [ ] TL6. Cross-compilation out of the box: `fid build --target linux-arm64` from
      any host, with no extra download (L).
- [x] TL7. Profiler output readable by standard tools (M).
      Done: `fid profile` samples the native call stack every millisecond and writes collapsed stacks (flamegraph.pl, inferno, speedscope).
- [x] TL8. A web playground (M).
      Done: fides-lang.org/playground runs the real `fidesc` as a WASI WebAssembly
      build in a module worker — the UI stays responsive, executions time out at
      15 s and the worker restarts, and the modes cover run, tokens, AST, SSA IR
      and inferred effects. The bundled stdlib is virtualized into the WASI fs.
- [ ] TL9. Clear dependency handling: Git and path, a lock file, audits of what a
      build pulls in (M).
- [~] TL10. Editor support beyond VS Code: Neovim, JetBrains, Zed (M).
      Done: configs for Neovim, Helix, Emacs, Sublime and JetBrains (LSP4IJ) in `editors/`, untested. Left: testing them, Zed (needs a grammar).
- [ ] TL11. `fid doc` sites as good as docs.rs and pkg.go.dev (M).
- [x] TL12. Call-graph visualization: `fidesc diagram <file.fd>` emits a Mermaid
      sequence diagram of the file's calls, annotated with inferred effects
      (`-o out.mmd`) (S).
      Done (Nov 3): `src/seqdiag.rs`; participants, calls in source order, `do`
      ops, `spawn`, `spawn scope`, `resume`, effect notes per function.

**Acceptance:** a new user is productive within an hour, on any supported system.

## Track PL — Platforms

Every platform gets a tier, and a tier means CI, not a hope.

- [x] PL1. Publish tiers: Tier 1 is tested on each commit, Tier 2 builds, Tier 3 is
      best effort (S).
      Done: `docs/reference/platforms.md`.
- [ ] PL2. Tier 1: Windows x64, Linux x64, macOS ARM64, Linux ARM64 (release F4–F8)
      (XL).
- [ ] PL3. Tier 2: Windows ARM64, macOS x64, FreeBSD, RISC-V (L).
- [~] PL4. WebAssembly with WASI (release F9) (L).
      Done: `fidesc wasm` emits a runnable .wasm (80 KB on hello) via
      `src/wasm.rs`/`compiler/wasm.fd`. Left: it imports a custom Fides
      `env` host API (`p_print`, `p_str_concat`, `p_panic`, …), not
      `wasi_snapshot_preview1` — real WASI needs fd_write/proc_exit
      mapping and a `_start` export.
- [~] PL5. Freestanding and embedded: Cortex-M, RISC-V boards, with no OS (release G)
      (L).
      Done: `--no-std` skips `core.fd` and still emits static ELFs —
      verified on `riscv64-linux` (43 KB) and `arm64-linux` (40 KB) with
      zero external tools; `compiler/embedded.fd` holds the bare-metal
      helpers. Left: a true no-OS target (flat binary, no ELF runtime),
      Cortex-M (needs a Thumb-2 encoder — arm64 emits A64 only).
- [~] PL6. Android and iOS (XL).
      Done: the static arm64-linux ELF is Android-adjacent — direct
      syscalls, no libc dependency, so it should run under `adb shell`
      (unverified on a device). Left: device testing, Android NDK/ABI
      quirks, and iOS entirely (needs the Mach-O backend + Apple syscall
      numbers + code signing).
- [~] PL7. Native backends for ELF and Mach-O, sharing one encoder (L).
      Done: `build --target` emits valid static ELF64 executables for all
      three arches with zero external tools — x86-64-linux, arm64-linux, and
      riscv64-linux all produce correct ELF headers (machine 62/183/243,
      ET_EXEC), and the x86-64 ELF **runs natively under WSL** (prints
      `Hello, Fides!`). Fixed a VReg-shadowing bug that printed raw virtual
      registers (`str v6`, `sd v9`), non-canonical `fp`/`return` mnemonics,
      unsigned-only immediates on negative offsets (`add xN, x29, #-24` →
      `sub`; `[x29, #-N]` → LDUR/STUR), and >255-byte stack slots (via x9).
      Left: Mach-O backend, encoder sharing between ELF targets, and running
      the emitted binaries under qemu/native CI.

**Acceptance:** Tier 1 passes the whole suite on each commit.

## Track ST — Stability and process

Trust is built by what we do not break.

- [x] ST1. Freeze the Rust compiler as a bootstrap only: new features land in
      `compiler/*.fd` alone. Writing everything twice is the largest drag on pace (M).
      Done: `docs/reference/stability.md`; `fidesc test --compiler release/fid.exe` runs the suite on `fid`.
      Nov 3: the Rust compiler gets no new features (proposal 0002 is in `fid`
      only); `bootstrap.sh` builds from a pinned stage 0 (`NATIVE=1` stages by
      `fid emit` with a byte-identical check, `PIN=1` pins). No stage 0 is
      pinned yet: a natively built `fid` stops with "Invalid free" until RT2's
      native string headers are finished.
- [x] ST2. A written stability promise: code that compiles on 1.0 compiles on every
      1.x (release K12) (S).
      Done: `docs/reference/stability.md`.
- [~] ST3. Editions, so the language can change without breaking old code (L).
      Done: the plan. Left: editions in the compiler.
- [~] ST4. A release every six weeks, on a date, whatever is ready (M).
      Done: the policy. Left: the first dated release.
- [~] ST5. Long-term support releases with security fixes (M).
      Done: the policy. Left: the first LTS release.
- [~] ST6. Before each release, build every known public Fides project, as Rust's
      crater does (L).
      Done: the rule. Left: a list of public projects and the job that builds them.
- [x] ST7. A public process for language changes (S).
      Done: proposals in `docs/proposals/`, the process in `docs/reference/stability.md`.
- [x] ST8. A security policy, a contact, and published advisories (S).
      Done: `SECURITY.md` (private reporting, response times, advisories).
- [x] ST9. Release notes that name every breaking change (S).
      Done: the rule in `docs/reference/stability.md` and `CONTRIBUTING.md`.
- [~] ST10. Signed checksums for every download (S).
      Done: `SHA256SUMS` and a Sigstore build provenance attestation per archive in `release.yml`. Left: proven on a real release.

**Acceptance:** two years of releases with no unannounced break.

## Track AD — Real use

Only this produces true production years. It starts now, at small scale.

**Using it ourselves**

- [~] AD1. `ark`, `scribe` and `veil` written in Fides (release N8) (L). The
      implementations exist in `compiler/{ark,scribe,veil}.fd` and now have
      standalone `fn main` entries in `tools/`. `ark.exe` builds and runs
      end-to-end (`version`, `list` verified); `scribe.exe` builds and
      generates docs (module-name CRLF bug found and fixed); `veil` builds
      fully as a Linux ELF through the nat backend (2,570 functions — needed
      the `p_f32_to_bits`/`p_int_to_float`/`p_float_to_int` shims, added to
      `ng_gen_call_std`). Windows `veil.exe` via the PE backend needs more
      RAM than a small machine has (deterministic 2 MB alloc OOM at PE
      emission). Shipped binaries are still the Rust ones.
- [ ] AD2. The project website served by a Fides program (M).
- [ ] AD3. `fides-os` as a standing test of bare-metal work (release G4) (M).
- [ ] AD4. Every internal tool that can be in Fides is (M).
- [ ] AD5. A real production service of ours, on Fides, with its incidents written up
      (L).

**Being reachable**

- [~] AD6. The book: a full guide, from first program to writing a service (L).
      `docs/book/` draft exists — intro/install, first program, tour, ownership,
      and the service chapter written; effects/errors/toolchain/concurrency/
      shipping are stubs.
- [x] AD7. A cookbook of common tasks (M). `docs/cookbook.md` — args, files,
      collections, strings, JSON, errors, HTTP server/client, config, tasks,
      timing, crypto, testing; recipes use verified stdlib signatures.
- [x] AD8. Guides for people arriving from Rust, Go, Zig and C (M).
      `docs/guides/from-{rust,go,zig,c}.md` — syntax maps, ownership-model
      deltas, real ports, what doesn't carry over.
- [x] AD9. A channel for questions, answered (S). GitHub Discussions enabled
      on pledgeandgrow/fid; `SUPPORT.md` routes questions/bugs/contributing.
      "Answered" is ongoing by definition.
- [x] AD10. Good first issues, kept current (S). Issues #2–#6 filed from
      real known-bugs/limitations (debug-print gating, scribe readdir,
      non-ASCII CI check, ark stubs, GlobalAddr on other targets).
- [x] AD11. A compiler internals guide, so others can contribute (M).
      `docs/compiler-internals.md` — pipeline, IR shapes, pass list + kill
      switches, calling conventions, lib-facts format, contributor traps.

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
