# Fides Adoption Strategy

> Build it and they will come — only if switching is easy.

> **Measured, Nov 6, 2026 — read this first.** Much of this document is the
> strategy as written in Aug–Sep 2026, and several of its claims were goals,
> not measurements. What is true today:
>
> - **Compile speed:** `fidesc emit` builds hello world in 0.39 s (Nov 3);
>   `fid` turns its own 133,688 lines into C in ~133 s (about 1,000
>   lines/s; type check 59 s, borrow check 16 s, lowering 20 s, C generation
>   12 s) and `fidesc` in ~21 s. Go and Zig compile on the order of 100,000
>   lines/s. "Sub-second" holds for small programs only.
> - **Runtime speed:** fib(40) 1.34 s native, 0.81 s through C, 0.53 s Rust
>   ([scoreboard](../roadmap/competitive-roadmap.md#the-scoreboard)).
> - **Binary size:** hello world from `fidesc emit` is 113 KB; an HTTP server
>   is 285 KB. The "< 4 KB" figures below are targets.
> - **Hot reload:** `fidesc watch` recompiles a Linux ELF program on change
>   and runs it again as a new process. It reports which functions changed;
>   it does not swap code inside a running process. `fid watch` is not
>   implemented. There is no `fidesc ai` command.
> - **Borrow checker:** Fides has one (`--borrowck=on` by default); it is
>   simpler than Rust's because references cannot be stored or returned.
> - **Platforms:** native output is Windows x64 (`fid emit`; native async
>   crashes); Linux through C; ARM64, RISC-V, WASM, macOS, native ELF and
>   bare metal are experimental / incomplete ([platforms](platforms.md)). The
>   embedded and robotics niches below depend on those targets.
> - **Generics** (proposal 0002) are in `fid`: inference or an error, bounds
>   checked, turbofish with arity check.
> - **Memory:** no GC; the compiler frees what it can prove is finished
>   (Vec/HashMap locals, owned elements, region collections, string
>   temporaries); everything else lives until exit. Copy-on-write value
>   semantics (proposal 0001) are written and switched off: `var b = a` on a
>   `Vec` aliases.
> - **Safety annotations** are 17 checked rules
>   ([safety-rules.md](safety-rules.md)); `fid qualify` writes evidence that
>   always says `NOT_QUALIFIED`. Fides is not suitable for medical,
>   automotive or avionics use: the medical, automotive and avionics sections
>   below describe a direction, not a product.
> - **Tests:** the full suite last ran on Nov 2 (interp 339/339, C 343/343,
>   PE 345/345, expected-fail 89/89); it has not been rerun with the Nov 4–6
>   compiler.
>
> Numbers: [CURRENT.md → Numbers](../../CURRENT.md#numbers).


## The Adoption Problem

Every new language faces the chicken-and-egg problem:
- Users want ecosystems before they adopt
- Ecosystems form around users
- Nobody switches languages for marginal improvements

**Fides's strategy: target niches where Fides uniquely wins, then expand.**

---

## Phase 1: Niche Domination (Year 1-2)

### Niche 1: Embedded AI
- **Why Fides wins:** SIMD types + comptime shape verification + bare metal + memory safety
- **Why not Rust:** No SIMD first-class, compile times too slow for iteration
- **Why not Python:** Too slow, no bare metal, GC pauses
- **Target users:** AI researchers deploying models to edge devices
- **Killer feature:** Write AI inference code that runs on a microcontroller with memory safety

### Niche 2: Robotics
- **Why Fides wins:** Real-time effects + capability security + hot reload on running robots
- **Why not Rust:** Async doesn't support real-time well, no hot reload
- **Why not C:** No memory safety, no real-time guarantees
- **Target users:** Robotics startups, autonomous vehicle teams
- **Killer feature (target):** Hot-reload control code on a running robot without safety violations — today `watch` restarts the program

### Niche 3: Safe Embedded Systems
- **Why Fides wins:** Memory safety + tiny binaries + deterministic memory + RISC-V support
- **Why not Rust:** Compile times too slow, borrow checker too rigid for embedded patterns
- **Why not C:** 70% of CVEs are memory safety issues
- **Target users:** IoT device manufacturers, medical device companies
- **Killer feature (target):** Memory-safe firmware with < 4KB binary size — not measured; ARM64/RISC-V runtimes are still stubs

> **Status (Nov 6, 2026):**
> - ❌ **Bare-metal mode** — `fid emit --bare` panics
> - ⚠️ **RISC-V target** — `codegen_riscv.rs` emits RISC-V assembly; the runtime is stubs
> - ✅ **FixedBufferAllocator** — `src/embedded.rs` (Rust compiler)
> - ⚠️ **Hot reload** — function-level change detection, then rebuild and restart
> - ✅ **C interop** — C header parser → Fides FFI bindings (`src/cparser.rs`); `parse-header` on SDL2/SQLite headers is open
> - ✅ **C codegen backend** — 343/343 tests match the interpreter (Nov 2)
>
> These niches remain aspirational until the embedded targets work.

---

## Phase 2: Expansion (Year 2-3)

### Bridge: Systems Programming
- **Why Fides wins:** Memory safety without borrow checker pain + fast compilation + hot reload
- **Migration path:** C interop + C-to-Fides transpiler
- **Target users:** C developers tired of CVEs, Rust developers tired of compile times
- **Killer feature:** Gradually migrate C codebase to memory safety, one file at a time

### Bridge: Web Backends
- **Why Fides wins:** Effect handlers for async I/O + hot reload + fast compilation
- **Migration path:** HTTP server stdlib + JSON serialization (comptime)
- **Target users:** Startups wanting fast iteration + safety
- **Killer feature:** Hot-reload a running web server, see changes instantly

### Bridge: CLI Tools
- **Why Fides wins:** Fast compilation + single binary output + no runtime
- **Migration path:** Simple syntax, familiar to Rust/Go developers
- **Target users:** Go/Rust CLI tool developers
- **Killer feature:** Sub-second compile for small programs + recompile-on-save (`watch`) during development

---

## Phase 3: Mainstream (Year 3-5)

### Bridge: Game Development
- **Why Fides wins:** Hot reload + SIMD + memory safety + fast compilation
- **Target users:** Game studios, indie developers
- **Killer feature:** Change game logic while the game is running

### Bridge: OS Development
- **Why Fides wins:** Bare metal + memory safety + inline assembly + capability security
- **Target users:** OS researchers, security-focused OS projects
- **Killer feature:** Write a memory-safe OS kernel with hot-reloadable drivers

### Bridge: Security-Critical Software
- **Why Fides wins:** Capability security + memory safety + effect system (auditable)
- **Target users:** Cryptography libraries, security tools, government software
- **Killer feature:** Prove no code can access the filesystem without explicit capability

---

## Migration Tools — Making Switching Easy

> **Note:** C-to-Fides and Rust-to-Fides transpilers are nice-to-have community
> goals, not on the critical path. See `docs/future.md` for honest
> feasibility assessment. The FFI generator below is the only migration
> tool that exists today.

### FFI Generator (Working now)
```
// Input: C header (openssl.h)
SSL_CTX* SSL_CTX_new(const SSL_METHOD* method);

// Output: Fides FFI binding
extern "C" fn SSL_CTX_new(method: *const SSL_METHOD) -> *mut SSL_CTX;
```
- Parse C headers automatically
- Generate Fides FFI bindings
- No hand-written bindings needed
- ✅ **Working now** — `fidesc parse-header <file.h>` parses C headers and emits Fides FFI bindings (`src/cparser.rs`)

### C-to-Fides Transpiler (Nice-to-have — see `docs/future.md`)
- Auto-translate C code to Fides code
- Feasible but moderate effort (real C has macros, goto, union punning)
- FFI already covers the interop case; transpiler would help porting logic

### Rust-to-Fides Transpiler (Aspirational — see `docs/future.md`)
- Convert Rust code to Fides code
- Very high effort — lifetimes, macros, async, trait system don't map cleanly
- Practical alternative: use FFI via C ABI, port files manually

---

## Developer Experience — The Secret Weapon

### Fast Compilation (small programs)
```
$ fidesc emit hello.fd
Emitted native executable (113152 bytes)   ← 0.39 s, Nov 3
```
Large programs are slower: the compiler itself (133,688 lines) takes about
133 s to turn into C with `fid` (CS1–CS7 in the competitive roadmap).

### Hot Reload (target)
`fidesc watch` rebuilds and reruns the program on save and reports which
functions changed; it does not swap code in a running process, and `fid
watch` runs `bench`. In-process reload is a target.

### Clear Error Messages
Every error carries an `Exxx` code ([diagnostics.md](diagnostics.md)) and
the source line. There is no region use-after-free check; the compile-time
memory errors are borrow and reference-escape errors, for example:
```
error: 'r' would outlive 'b'
```

### AI-Assisted Development
There is no `fidesc ai` or `fid ai` command. The language reference
([reference.md](reference.md)) cites the compiler function behind each rule,
which is what an AI assistant needs to write Fides that compiles.

---

## Community Building

### Open Source Strategy
- **MIT/Apache license** — permissive, no GPL contamination
- **GitHub** — where developers already are
- **Good first issues** — labeled for newcomers
- **Architecture docs** — help contributors understand the codebase

### Documentation Strategy
- **Fides Book** — free online book (like "The Rust Programming Language")
- **Interactive Tutorial** — learn by doing, in the browser
- **Examples Library** — real-world code for every use case
- **Playground** — try Fides without installing anything

### Conference Strategy
- **Talks at systems conferences** — OSDI, SOSP, PLDI
- **Talks at AI conferences** — NeurIPS, ICML (for the AI/ML angle)
- **Talks at embedded conferences** — Embedded World, IoT World
- **Blog posts** — technical deep dives, benchmarks, case studies

---

## Success Metrics

| Metric | Year 1 | Year 2 | Year 3 | Year 5 |
|---|---|---|---|---|
| GitHub stars | 1K | 10K | 50K | 100K |
| Active developers | 50 | 500 | 2K | 10K |
| Production users | 1 | 10 | 50 | 500 |
| Packages | 10 | 100 | 1K | 10K |
| Conference talks | 1 | 5 | 20 | 50 |
| University courses | 0 | 1 | 5 | 20 |

---

## The Adoption Funnel

```
Awareness (people hear about Fides)
    ↓ 10% convert
Interest (people try the playground)
    ↓ 20% convert
Trial (people install fidesc)
    ↓ 30% convert
Adoption (people use Fides for a project)
    ↓ 50% convert
Advocacy (people recommend Fides to others)
```

**Key conversion points:**
1. **Awareness → Interest:** Killer demos (hot reload on a robot, AI on a microcontroller)
2. **Interest → Trial:** Playground + one-command install
3. **Trial → Adoption:** Fast compilation + hot reload (instant gratification)
4. **Adoption → Advocacy:** Safety + performance (results speak for themselves)

---

# Fides vs The World — Competitive Analysis

> Honest assessment. No ego. No vanity. Just facts.

## The Competition

### C — The Legacy Champion
| Dimension | Rating | Notes |
|---|---|---|
| Performance | 10/10 | Optimal machine code, zero overhead |
| Safety | 1/10 | 70% of CVEs are memory safety issues |
| Flexibility | 10/10 | Can do anything, anywhere |
| Compilation | 8/10 | Fast compilation |
| Ecosystem | 10/10 | Everything talks to C |
| Metaprogramming | 2/10 | Preprocessor hacks |
| Concurrency | 3/10 | Manual, error-prone |
| Developer velocity | 4/10 | No safety net, slow debugging |

**C's fatal flaw:** Memory unsafety is structural, not fixable without changing the language.

### Rust — The Safety Champion
| Dimension | Rating | Notes |
|---|---|---|
| Performance | 9/10 | Near-optimal, zero-cost abstractions |
| Safety | 9/10 | Best in class, but borrow checker is rigid |
| Flexibility | 7/10 | Borrow checker rejects valid patterns |
| Compilation | 2/10 | Universally cited as #1 pain point |
| Ecosystem | 8/10 | Growing fast, crates.io |
| Metaprogramming | 6/10 | Two macro systems, complex |
| Concurrency | 6/10 | Async is "suffering" (their word) |
| Developer velocity | 3/10 | 25-30 min builds, no hot reload |

**Rust's fatal flaw:** Compile times + borrow checker rigidity + async complexity create a productivity tax that never fully goes away.

### Zig — The Elegance Champion
| Dimension | Rating | Notes |
|---|---|---|
| Performance | 9/10 | Excellent, comptime helps |
| Safety | 3/10 | No memory safety, relies on discipline |
| Flexibility | 9/10 | Explicit allocators, comptime |
| Compilation | 7/10 | Fast, but comptime can be slow |
| Ecosystem | 4/10 | Small but growing |
| Metaprogramming | 9/10 | comptime is brilliant, unified |
| Concurrency | 4/10 | Basic, manual |
| Developer velocity | 7/10 | Fast iteration, but no safety net |

**Zig's fatal flaw:** No memory safety. "Engineer discipline" doesn't scale to millions of lines of code.

### Mojo — The AI Champion
| Dimension | Rating | Notes |
|---|---|---|
| Performance | 9/10 | MLIR-based, AI-optimized |
| Safety | 5/10 | Some safety, not Rust-level |
| Flexibility | 8/10 | Python-like + systems-level |
| Compilation | 5/10 | Heavy compile-time, code explosion |
| Ecosystem | 3/10 | Very new, AI-focused |
| Metaprogramming | 8/10 | comptime-like, MLIR-powered |
| Concurrency | 6/10 | AI-parallelism focus |
| Developer velocity | 6/10 | Python syntax helps, but compile cost |

**Mojo's fatal flaw:** Niche focus (AI/ML), not a general systems language. Still depends on MLIR infrastructure.

---

## Fides — Current State

| Dimension | Current | Target | How |
|---|---|---|---|
| Performance | 7/10 | 9/10 | Custom codegen with 23 opt passes (enabled by default), LTO (`--lto`), loop vectorization + SLP (SSE2, constant trips). No PGO, no variable-trip vectorization. LLVM is 20+ years ahead but gap is narrowing. |
| Safety | 8/10 | 10/10 | Capabilities + effects + sandbox + borrow checker + reference escape check + 17 safety rules enforced at compile time. Data races: globals need a lock, spawned arguments must be `@send`; a `Vec` passed to a thread is shared unchecked. `var b = a` on a `Vec` aliases (copy-on-write off). Binary hardening (stack canaries, PIE, RELRO, NX stack). |
| Flexibility | 7/10 | 10/10 | Inline assembly (x86 only), comptime, FFI, dynamic loading, conditional compilation. Bare metal and ARM64/RISC-V are incomplete. No C++ interop. |
| Compilation | 5/10 | 10/10 | Custom IR, no LLVM, integrated linker on Windows. Sub-second for small programs; ~1,000 lines/s for `fid` on large ones (Nov 6) |
| Ecosystem | 2/10 | 8/10 | Package manager (ark) implemented, no crates registry yet, doc generator (scribe) implemented, 137 stdlib modules. Rust has 100k+ crates. C has everything. |
| Metaprogramming | 8/10 | 10/10 | comptime is powerful: `@derive(Debug/Clone/Serialize/Deserialize)`, `comptime gen { }`, reflection. No procedural macros. |
| Concurrency | 6/10 | 10/10 | OS threads (`spawn`, `@send` arguments), `parallel { }`, green tasks (`async fn`, `task` module), bounded MPSC channels, `select`, atomics (`use atomic`). Native async crashes. |
| Developer velocity | 9/10 | 10/10 | Hot reload + persist + watch --diff + LSP (completion/hover/goto-def) + formatter + linter + bench + stack traces. No debugger, no profiler. |
| Security | 9/10 | 10/10 | sandbox + cap-aware linking + @constant_time + @tainted + binary hardening — compile-time, zero runtime cost. Best-in-class. |
| Tooling | 6/10 | 8/10 | LSP (diagnostics + completion + hover + goto-def), formatter, linter, bench, stack traces. Debugger (probe/fidesc debug) implemented, profiler (pulse/fidesc bench) partial, doc generator (scribe) implemented, package manager (ark) implemented, coverage tool (veil) implemented. Rust has cargo + rust-analyzer + rustfmt + clippy + rustdoc. |
| Community | 1/10 | 7/10 | Independent, no corporate backing. Rust has Mozilla/AWS/Google. Zig has Zig Foundation. |

> **Current state (Aug 2026, corrected Nov 6):**
> - ⚠️ **Compilation is fast only for small programs** — sub-second for hello world; the 133,688-line compiler takes ~133 s with `fid` (Nov 6)
> - ✅ **Safety checks work** — effects + capabilities (per call) + borrow checker + escape check are enforced at compile time; regions are scopes with budgets
> - ⚠️ **Hot reload is recompile-and-restart** — `fidesc watch` rebuilds and reruns (Linux ELF), reports changed functions, keeps `persist` variables; no in-process swap
> - ✅ **Custom toolchain works** — self-hosted codegen, linker, ELF/PE/Mach-O/COFF emitters
> - ✅ **`sandbox { }` works** — compile-time capability revocation, zero runtime cost
> - ✅ **Memory budgets work** — `region r with budget(N)` enforced at compile time
> - ✅ **`watch --diff` works** — PIR L2 line diff + binary size delta on each reload
> - ✅ **Capability-aware linking works** — `--caps` flag strips ungranted functions from binary
> - ✅ **Optimizer enabled by default** — 23 passes including const-fold, CSE, GVN, DCE, DSE, inline, unroll, TCO, loop-vec, SLP. `--lto` for aggressive cross-module inlining.
> - ✅ **C codegen at 343/343 test parity** (Nov 2 full suite). Binary hardening: stack canaries, PIE/ASLR, RELRO, NX stack. Stack traces on panic.
> - ✅ **LSP server works** — `fidesc lsp` provides JSON-RPC over stdio with diagnostics, completion, hover, goto-definition. No external dependencies.
> - ✅ **Generics work** — `enum Option<T>`, `struct Foo<T>`, generic functions with inference, checked bounds and turbofish (`fid`)
> - ✅ **Concurrency works** — `spawn` (OS threads), bounded MPSC channels (`p_chan_*`), executor runtime (`p_executor_*`), async IO (`p_net_*`, `p_fs_read_async`), `select` statement
> - ✅ **Tooling works** — `fidesc fmt` (formatter), `fidesc lint` (linter), `fidesc bench` (benchmarking), stack traces on panic
> - ✅ **Type inference works** — Bidirectional (contextual literals, `-> _` return type inference)
> - ✅ **Auto-vectorization works** — Loop-vec (constant trips) + SLP (p_simd_load/store/add/sub/mul/div → SSE2)
> - ✅ **137 stdlib modules** (Nov 6) — core, vec, math, channel, sync, atomic, task, ecs, events, parallel, db, http, json, file, time, and more (uefi moved to fides-os)
> - ✅ **Binary hardening** — All C codegen output compiled with stack canaries, PIE/ASLR, full RELRO, NX stack
> - ✅ **Security annotations** — `@constant_time` (side-channel defense), `@tainted` (injection defense)
> - ⚠️ **Package manager (ark)** — path/git deps implemented, registry pending
> - ⚠️ **Debugger** — `fidesc debug` (interpreter); no DWARF debug info
> - ⚠️ **Cross-compilation** — `build --target arm64-linux`/`riscv64-linux` exist and are experimental; ARM64/RISC-V runtimes are stubs
> - ✅ **Green tasks** — `async fn` runs as a task on its own stack (interpreter and C; native async crashes)
> - ⚠️ **Data race checks** — `@send` arguments for `spawn`, locked writes to globals; data shared through a `Vec` handle or pointers is not checked

---

## Where Fides Wins Uniquely

### No other language has ALL of these:
1. **Region-based memory safety** (Rust has borrow checking, Zig has nothing, C has nothing)
2. **Effect handlers for concurrency** — ✅ Effect tracking works + OS threads + channels + executor + select. Rust has working async, Go has goroutines, C has pthreads.
3. **Capability-based security** (No systems language has this)
4. **`sandbox { }` capability revocation** — ✅ IMPLEMENTED (no other language can do this at compile time)
5. **Compile-time memory budgets** — ✅ IMPLEMENTED (Zig has runtime panic only, Rust/C have nothing)
6. **`persist` variables across hot reloads** — ✅ IMPLEMENTED (no language has scope-level state persistence)
7. **`watch --diff` optimizer visibility** — ✅ IMPLEMENTED (impossible with LLVM-based compilers)
8. **Capability-aware linking** — ✅ IMPLEMENTED (no linker strips code based on capabilities)
9. **comptime metaprogramming** (Zig has it but not type-safe at declaration, Rust uses macros)
10. **Recompile-on-save** (`fidesc watch`, Linux only; restarts the program — not in-process hot reload yet)
11. **Custom toolchain** (Every modern language uses LLVM — we don't. But this also means no LLVM optimizations)
12. **Few external dependencies** (No LLVM, no external linker or assembler on the native Windows path, which still imports kernel32 and msvcrt; other targets need a C compiler. But also: no LLVM ecosystem)
13. **Built-in LSP server** — ✅ IMPLEMENTED (`fidesc lsp` with completion, hover, goto-def. No external deps.)
14. **`@constant_time` annotation** — ✅ IMPLEMENTED (prevents timing side-channel attacks. No other language has this.)
15. **`@tainted` annotation** — ✅ IMPLEMENTED (prevents injection attacks. No other language has this as a language feature.)
16. **Binary hardening by default** — ✅ IMPLEMENTED (stack canaries, PIE, RELRO, NX stack — no opt-in required)
17. **AI/robotics/IoT ready** — ⚠️ PARTIAL (SIMD, bare metal, real-time effects, tensor types, autograd, GPU/SPIR-V exist but no real hardware drivers, no RTOS integration)

### The "Killer App" Argument
What can you do in Fides that you can't do in any other language?

- **Write a safe OS kernel with hot reload** — Rust can do safe kernels but no hot reload. Zig can do hot reload but no safety. C can do kernels but no safety. *(Note: Fides's kernel story is theoretical — no real kernel has been written in Fides yet.)*
- **Write a robot control system with real-time guarantees and memory safety** — Rust's async doesn't support real-time well. Zig has no safety. C has no safety. *(Note: Fides's real-time effects are parsed but the runtime doesn't enforce deadlines yet.)*
- **Deploy a web server with zero external dependencies** — Rust needs a linker. Zig needs a linker. C needs everything. *(Note: Fides's HTTP stdlib is basic — not production-grade.)*
- **Run untrusted plugins with compile-time proof they can't do IO** — `sandbox { }` makes this a one-liner. In C/Rust/Go you need separate processes or WASM sandboxes. ✅ This works today.
- **Prove an embedded system stays within 4KB of memory at compile time** — `region r with budget(4096)` is a compile error if exceeded. Zig's FixedBufferAllocator panics at runtime. ✅ This works today.
- **Strip all network code from a binary by not granting the Net capability** — `fidesc build --caps "IO"` removes Net-dependent functions. No other linker does this. ✅ This works today.
- **See what the optimizer did when you changed one line** — `fidesc watch --diff` shows PIR + binary size changes. LLVM compilers are black boxes. ✅ This works today.

> **Honest caveat:** The "killer apps" that work *today* are sandbox, memory
> budgets, cap-aware linking, and watch --diff. The kernel/robotics/web server
> scenarios are theoretically possible but haven't been demonstrated in real
> deployments. Fides's unique features are real, but their practical value is
> unproven at scale.

---

## Where Fides Could Lose

### Honest Risks

1. **Ecosystem cold start** — Rust has crates.io, C has everything. Fides starts at zero. This is the biggest practical barrier.

2. **Custom codegen quality** — LLVM has 20+ years of optimization. Our codegen will be worse for years. We mitigate by focusing on compilation speed first, optimization quality later.

3. **Region inference complexity** — If region inference requires explicit annotations everywhere, it becomes as annoying as Rust lifetimes. The key is making 90% of code annotation-free.

4. **Effect handler runtime overhead** — Delimited continuations have overhead. We mitigate by compiling effect handlers away when the handler is statically known.

5. **Adoption chicken-and-egg** — People use languages with ecosystems. Ecosystems form around languages with users. We break this by targeting niches where Fides uniquely wins (embedded AI, robotics, safe kernels).

6. **Team size** — Building a custom toolchain is massive work. LLVM has hundreds of contributors. We need to be strategic about what we build vs what we defer.

---

## What Fides Doesn't Do (That Others Do)

This is the honest list of gaps. No spin. These are things you can do in Rust,
Zig, Go, C, or Python today that you cannot do in Fides.

### Concurrency & Parallelism

| Feature | Rust | Zig | Go | C | Fides |
|---------|------|-----|-----|---|-------|
| OS threads | ✅ `std::thread` | ✅ `std.Thread` | ✅ goroutines (runtime) | ✅ pthreads | ✅ `spawn` + `p_thread_spawn` |
| Data race prevention | ✅ Send/Sync (compile-time) | ❌ None | ✅ Race detector (runtime) | ❌ None | ⚠️ `@send` for spawned arguments, locked global writes; shared `Vec` handles unchecked |
| Channels / message passing | ✅ `std::sync::mpsc` | ❌ None | ✅ Built-in channels | ⚠️ Manual | ✅ `p_chan_*` bounded MPSC channels |
| Mutex / RwLock / Condvar | ✅ `std::sync` | ⚠️ Basic | ✅ `sync` package | ⚠️ pthreads | ⚠️ pthread mutex+condvar (C codegen) |
| Async runtime | ✅ Tokio (mature) | ❌ None | ✅ Built-in (goroutines) | ❌ None | ✅ Executor runtime (`p_executor_*`) |
| Async file I/O | ✅ tokio::fs | ❌ None | ✅ Built-in | ❌ None | ✅ `p_fs_read_async`, `p_net_*` |
| Parallel iterators | ✅ rayon | ❌ None | ⚠️ errgroup | ⚠️ OpenMP | ⚠️ `stdlib/parallel.fd` (basic) |
| Atomic operations | ✅ `std::sync::atomic` | ✅ `@atomicLoad` | ✅ `sync/atomic` | ✅ `stdatomic.h` | ✅ `use atomic` (all orders run as SeqCst) |
| Thread-safe ref counting | ✅ `Arc<T>` | ❌ None | ✅ Built-in | ❌ None | ⚠️ `Arc<T>` with manual `clone`/`drop` |
| Select statement | ❌ None | ❌ None | ✅ `select` | ❌ None | ✅ `select { case ch => x => { } }` |

**Impact:** Fides now has working concurrency: OS threads (`spawn`), bounded
MPSC channels (`p_chan_*`), an executor runtime with non-blocking poll
(`p_executor_*`), async IO (`p_net_*`, `p_fs_read_async`), Go-style
`select` statements for multi-channel multiplexing, green tasks, atomics and
`@send` checks on spawned arguments. Rust's concurrency is more mature
(Send/Sync, Tokio ecosystem).

### Tooling & Developer Experience

| Feature | Rust | Zig | Go | Fides |
|---------|------|-----|-----|-------|
| Package manager | ✅ cargo (gold standard) | ⚠️ Zon (immature) | ✅ go modules | ❌ Not yet |
| Code formatter | ✅ rustfmt | ✅ zig fmt | ✅ gofmt | ✅ `fidesc fmt` (--check mode) |
| Linter | ✅ clippy | ❌ None | ✅ go vet | ✅ `fidesc lint` (unused funcs/imports) |
| Debugger | ✅ gdb/lldb (DWARF) | ✅ lldb (DWARF) | ✅ delve | ❌ No DWARF, no debugger |
| Profiler | ✅ perf/flamegraph | ⚠️ Basic | ✅ pprof | ❌ Not yet |
| Doc generator | ✅ rustdoc (docs.rs) | ✅ autodoc | ✅ godoc | ❌ Not yet |
| Test framework | ✅ `#[test]` (mature) | ⚠️ Basic | ✅ `testing` | ⚠️ `fidesc test` (basic, no `#[test]` attr) |
| Benchmarking | ✅ criterion | ⚠️ Basic | ✅ `testing.B` | ✅ `fidesc bench` (-n, --ccompile) |
| Fuzzing | ✅ cargo-fuzz | ❌ None | ✅ go-fuzz | ❌ Not yet |
| Property testing | ✅ proptest | ❌ None | ✅ gopter | ❌ Not yet |
| LSP: completion | ✅ rust-analyzer | ✅ zls | ✅ gopls | ✅ `fidesc lsp` (keywords + symbols) |
| LSP: goto-def | ✅ rust-analyzer | ✅ zls | ✅ gopls | ✅ `fidesc lsp` |
| LSP: hover | ✅ rust-analyzer | ✅ zls | ✅ gopls | ✅ `fidesc lsp` (type signatures) |
| LSP: rename | ✅ rust-analyzer | ✅ zls | ✅ gopls | ❌ Not yet |
| LSP: code actions | ✅ rust-analyzer | ⚠️ Partial | ✅ gopls | ❌ Not yet |
| Stack traces on panic | ✅ Backtrace | ✅ Stack trace | ✅ Stack trace | ✅ Call stack trace (interp + C codegen) |
| REPL history | ✅ evcxr (external) | ❌ None | ❌ None | ⚠️ Basic REPL, no history |

**Impact:** The developer experience gap has narrowed significantly. Fides now
has LSP (completion, hover, goto-def), a code formatter (`fidesc fmt`), a
linter (`fidesc lint`), benchmarking (`fidesc bench`), and stack traces on
panic. Still missing: debugger (no DWARF), profiler, doc generator, package
manager, rename/code actions in LSP. Rust's tooling is more mature, but
Fides is no longer bare — the core developer workflow works.

### Code Quality & Optimization

| Feature | Rust (LLVM) | Zig (LLVM) | Go | C (GCC/LLVM) | Fides |
|---------|------|-----|-----|---|-------|
| Loop unrolling | ✅ Automatic | ✅ Automatic | ⚠️ Limited | ✅ Automatic | ✅ Automatic (opt enabled by default) |
| Vectorization (SIMD auto) | ✅ LLVM auto-vec | ✅ LLVM auto-vec | ❌ None | ✅ GCC auto-vec | ✅ Loop-vec + SLP (SSE2, constant trips) |
| Inlining | ✅ Cross-crate | ✅ Cross-crate | ⚠️ Limited | ✅ LTO | ✅ Same-module + `--lto` cross-module |
| Link-time optimization | ✅ LTO | ✅ LTO | ❌ None | ✅ LTO | ✅ LTO (`--lto` flag) |
| Profile-guided opt | ✅ PGO | ✅ PGO | ❌ None | ✅ PGO | ❌ Not yet |
| Dead code elimination | ✅ LLVM | ✅ LLVM | ✅ Linker | ✅ Linker | ✅ IR-level DCE + cap-aware stripping |
| Constant propagation | ✅ LLVM | ✅ LLVM | ✅ Compiler | ✅ LLVM | ✅ Const-fold + partial-eval (enabled) |
| Tail call optimization | ✅ LLVM | ✅ LLVM | ❌ None | ✅ LLVM | ✅ TCO (enabled by default) |
| Escape analysis | ✅ LLVM | ✅ LLVM | ✅ Compiler | ⚠️ Limited | ❌ Not yet |
| Register allocation | ✅ Graph coloring | ✅ LLVM | ✅ Compiler | ✅ LLVM | ⚠️ Linear scan (known bug) |

**Impact:** Fides-generated binaries will be slower than Rust/Zig/C for
compute-heavy workloads. LLVM's auto-vectorization, LTO, and PGO are
decades of engineering we don't have. For I/O-bound or safety-critical
code, this doesn't matter. For HPC, game engines, or data processing,
it does.

### Standard Library Depth

| Feature | Rust | Go | Python | Fides |
|---------|------|-----|--------|-------|
| HTTP client | ✅ reqwest | ✅ net/http | ✅ requests | ❌ Not yet |
| HTTP server | ✅ actix/axum | ✅ net/http | ✅ flask/fastapi | ⚠️ Basic (stdlib/http.fd) |
| JSON serialization | ✅ serde_json | ✅ encoding/json | ✅ json | ⚠️ Basic (stdlib/json.fd) |
| Regex engine | ✅ regex crate | ✅ regexp | ✅ re module | ⚠️ Wildcard only (stdlib/regex.fd) |
| Date/time | ✅ chrono | ✅ time | ✅ datetime | ⚠️ LogicalTime only (no clock, no timezone) |
| Crypto/hashing | ✅ ring/rustls | ✅ crypto | ✅ hashlib | ❌ Not yet |
| TLS/SSL | ✅ rustls | ✅ crypto/tls | ✅ ssl | ❌ Not yet |
| Database drivers | ✅ diesel/sqlx | ✅ database/sql | ✅ psycopg2/etc | ❌ Not yet |
| Async I/O | ✅ tokio::fs | ✅ os package | ✅ asyncio | ❌ Not yet |
| Compression | ✅ flate2/zstd | ✅ compress | ✅ gzip/zlib | ❌ Not yet |
| Image processing | ✅ image crate | ✅ image | ✅ PIL | ❌ Not yet |
| Random numbers | ✅ rand crate | ✅ math/rand | ✅ random | ❌ Not yet |
| Logging | ✅ log/tracing | ✅ log | ✅ logging | ❌ Not yet |
| CLI parsing | ✅ clap | ✅ flag | ✅ argparse | ❌ Not yet |
| Temp files | ✅ tempfile | ✅ ioutil | ✅ tempfile | ❌ Not yet |
| Iterators | ✅ Iterator trait | ⚠️ Basic | ✅ Generators | ✅ `for x in vec`, adapters (take, skip, zip, etc.) |
| Collections (BTree, etc.) | ✅ std::collections | ✅ Built-in | ✅ collections | ⚠️ Vec + HashMap only |
| String processing | ✅ std::string | ✅ strings | ✅ str methods | ⚠️ Basic (len, find, slice, split) |
| Path manipulation | ✅ std::path | ✅ path/filepath | ✅ os.path | ❌ Not yet |
| Process management | ✅ std::process | ✅ os/exec | ✅ subprocess | ⚠️ Basic (stdlib/process.fd) |

**Impact:** Fides's stdlib has 137 modules (Nov 6). Rust's crates.io has 100k+.
Even Go's standard library is vast. For any real project, you'll need
to build infrastructure that other languages give you for free.

### Language Features Missing

| Feature | Rust | Zig | Go | C | Fides |
|---------|------|-----|-----|---|-------|
| Iterators / generators | ✅ Iterator trait | ⚠️ Basic | ⚠️ Basic | ❌ None | ✅ `for x in vec`, adapters |
| Trait `Display`/`Debug` | ✅ Derive macros | ❌ None | ✅ Stringer | ❌ None | ✅ `@derive(Debug)` |
| `From`/`Into` conversions | ✅ Traits | ❌ None | ❌ None | ❌ None | ❌ Not yet |
| `Drop` trait (custom cleanup) | ✅ Trait | ✅ `defer` | ✅ `defer` | ❌ None | ⚠️ `defer`; a `drop` method runs at function exit for locals with an explicit struct type |
| `Copy`/`Clone` traits | ✅ Derive | ⚠️ Implicit | ⚠️ Implicit | ⚠️ Implicit | ✅ `@derive(Clone)` |
| `Default` trait | ✅ Derive | ❌ None | ❌ None | ❌ None | ✅ `@derive(Default)` |
| `PartialEq`/`Eq`/`Hash`/`Ord` | ✅ Derive | ❌ None | ⚠️ Built-in | ❌ None | ⚠️ `@derive(PartialEq, Eq)`; struct `==` compares fields; no `Hash`/`Ord` |
| Serialization (serde) | ✅ serde (gold standard) | ❌ None | ✅ encoding/json | ❌ None | ✅ `@derive(Serialize/Deserialize)` |
| `#[derive]` macros | ✅ Powerful | ❌ None | ❌ None | ❌ None | ✅ `@derive(Debug/Clone/Serialize/Deserialize)` |
| Error trait / error chaining | ✅ `std::error::Error` | ❌ None | ✅ `error` type | ❌ None | ✅ `?` operator, `try {}`, Result<T,E> |
| Non-exhaustive enums/structs | ✅ `#[non_exhaustive]` | ❌ None | ❌ None | ❌ None | ❌ Not yet |
| Const generics (full) | ✅ Stable | ❌ None | ❌ None | ❌ None | ⚠️ Interpreter + PE only |
| Associated types (full) | ✅ Stable | ❌ None | ❌ None | ❌ None | ⚠️ Interpreter + PE only |
| GATs (full) | ✅ Stable | ❌ None | ❌ None | ❌ None | ⚠️ Interpreter + PE only |
| Conditional compilation | ✅ `#[cfg]` | ✅ `comptime` | ✅ Build tags | ✅ `#ifdef` | ✅ `@cfg(platform/debug)` |
| Feature flags | ✅ `#[feature]` | ❌ None | ❌ None | ❌ None | ❌ Not yet |
| Workspaces / multi-crate | ✅ cargo workspaces | ✅ build.zig | ✅ go.work | ⚠️ Makefiles | ❌ Not yet |
| Dynamic loading (.so/.dll) | ✅ libloading | ✅ `@cImport` | ✅ plugin | ✅ dlopen | ✅ `p_dl_open/sym/call` |
| C++ interop | ✅ `extern "C++"` | ⚠️ Limited | ❌ None | ✅ Native | ❌ C only |
| Inline functions | ✅ `#[inline]` | ✅ `inline` | ⚠️ Automatic | ✅ `static inline` | ⚠️ Inliner enabled by default |
| Stack traces | ✅ Backtrace | ✅ Stack trace | ✅ Stack trace | ⚠️ glibc backtrace | ✅ Call stack trace (interp + C) |
| Panic recovery | ✅ `catch_unwind` | ❌ None | ✅ `recover()` | ❌ None | ❌ Not yet |
| Type inference | ✅ Full HM | ❌ None | ⚠️ Partial | ❌ None | ✅ Bidirectional + `-> _` |
| Select statement | ❌ None | ❌ None | ✅ `select` | ❌ None | ✅ `select { case ... }` |

**Impact:** Fides now has many patterns that Rust/Go developers expect: iterators
with adapters, `@derive(Debug/Clone/Serialize/Deserialize)`, `?` error chaining,
`try {}` blocks, conditional compilation (`@cfg`), dynamic loading (`p_dl_*`),
type inference (bidirectional + `-> _`), and `select` for channels. Still
missing: `From`/`Into` conversions, `Hash`/`Ord` derive, non-exhaustive
enums, feature flags, workspaces, panic recovery, `where` clauses, nested and
or-patterns.

### Platform & Interop

| Feature | Rust | Zig | Go | C | Fides |
|---------|------|-----|-----|---|-------|
| Cross-compilation | ✅ `cargo build --target` | ✅ `zig build -Dtarget` | ✅ `GOOS/GOARCH` | ⚠️ Cross-toolchain | ✅ Zero-tool cross-compile |
| C translation | ❌ Manual | ✅ `zig translate-c` | ❌ Manual | N/A | ❌ Planned |
| C++ interop | ✅ `extern "C++"` | ⚠️ Limited | ❌ None | ✅ Native | ❌ C only |
| Dynamic loading | ✅ libloading | ✅ `@cImport` | ✅ plugin | ✅ dlopen | ✅ `p_dl_open/sym/call` |
| DWARF debug info | ✅ Full | ✅ Full | ✅ Full | ✅ Full | ❌ Not yet |
| Link with C++ | ✅ Yes | ⚠️ Limited | ❌ No | ✅ Yes | ❌ C only |
| FFI to Python | ✅ PyO3 | ❌ None | ✅ cgo | ✅ ctypes/cffi | ❌ Via C only |
| FFI to Node.js | ✅ napi-rs | ❌ None | ❌ None | ✅ node-addon | ❌ Via C only |
| WebAssembly (full) | ✅ wasm32 target | ✅ wasm32 target | ✅ wasm target | ⚠️ Emscripten | ⚠️ Binary output, no WASI |
| Mobile (iOS/Android) | ✅ cargo + targets | ⚠️ Limited | ✅ gomobile | ⚠️ NDK | ❌ Not yet |

**Impact:** Cross-compilation now works with zero external tools — `build --target arm64-linux`, `--target riscv64-linux`, and `--target x86-64-windows` all produce valid executables. This matches Rust/Zig/Go's cross-compilation story but without requiring any cross-toolchain installation.
No DWARF means you can't use gdb/lldb to debug Fides binaries.
No C++ interop limits integration with existing C++ codebases.

### Ecosystem & Community

| Feature | Rust | Zig | Go | C | Fides |
|---------|------|-----|-----|---|-------|
| Package registry | ✅ crates.io (100k+) | ❌ None (Zon planned) | ✅ proxy.golang.org | ❌ None | ❌ None |
| Documentation hosting | ✅ docs.rs | ⚠️ autodoc | ✅ pkg.go.dev | ⚠️ man pages | ❌ None |
| Community size | ✅ Large, active | ✅ Passionate | ✅ Large | ✅ Massive | ❌ Just starting |
| Corporate backing | ✅ Mozilla/AWS/Google | ⚠️ Zig Foundation | ✅ Google | ✅ Everyone | ❌ Independent |
| Conference talks | ✅ Many | ✅ Growing | ✅ Many | ✅ Many | ❌ None |
| Books published | ✅ Many | ⚠️ In progress | ✅ Many | ✅ Many | ❌ None |
| Online courses | ✅ Many | ⚠️ Growing | ✅ Many | ✅ Many | ❌ None |
| Stack Overflow | ✅ 50k+ questions | ⚠️ Growing | ✅ 100k+ | ✅ 500k+ | ❌ None |
| GitHub stars | ✅ 90k+ (rust) | ✅ 30k+ (zig) | ✅ 120k+ (go) | ❌ N/A | ❌ Minimal |
| Production users | ✅ Many (AWS, Cloudflare, Discord) | ⚠️ Growing (Uber, Bun) | ✅ Many (Google, Uber, Twitch) | ✅ Everyone | ❌ None known |

**Impact:** No community means no one to ask for help, no libraries to
reuse, no tutorials to learn from, no conference talks to watch. This is
the coldest of cold starts.

---

## The "100x" Claim — Honest Breakdown

| Claim | Reality | How |
|---|---|---|
| 100x faster compilation than Rust | **Plausible** | No LLVM, no proc macros, integrated linker. But only for small projects — Fides hasn't been tested on large codebases. |
| 100x faster iteration | **Plausible** | Hot reload vs full rebuild. But only if you don't need a debugger, formatter, or linter — which Fides doesn't have. |
| 10x faster runtime than Rust/Zig | **Not plausible** | Same machine code quality. Fides has auto-vec (loop-vec + SLP) but only for constant trip counts. LLVM's auto-vec is more general. LTO is available (`--lto`). |
| 2-3x faster runtime in specific cases | **Plausible for effect-aware opts** | Effect-based dead call elimination, pure call hoisting, and `@deterministic` CSE are unique to Fides. These can eliminate work LLVM can't. |
| 10x fewer bugs than C | **Plausible** | Memory safety eliminates 70% of CVEs. But Fides's own compiler may have bugs that produce wrong code. |
| 10x easier concurrency than Rust | **Plausible** | Effect handlers + channels + select are simpler than Rust's async/Send/Sync/Pin. No "async coloring" of functions. But Fides lacks data race prevention (Send/Sync). |
| 10x smaller binaries than C | **Plausible** | Dead code elimination at IR level, no libc, `--caps` stripping. But C with `-Os` and manual stripping can get close. |
| 10x safer than Rust | **Not plausible** | Rust's borrow checker is proven at scale (Firefox, Linux kernel). Fides's regions are unproven in production. Both are memory-safe. |
| 10x better DX than Rust | **Plausible for iteration** | Hot reload + fast compilation + LSP + formatter + linter. But Rust has mature registry — Fides has ark (package manager), scribe (doc generator), veil (coverage), probe (debugger) implemented, but no registry yet. |

**The "100x" is about compilation speed and iteration, not runtime performance
or overall developer experience.** Fides compiles fast and hot-reloads, but
the total developer experience (including debugging, tooling, library
availability) is currently worse than Rust, Go, and even C.

---

## Fides vs C — Detailed Head-to-Head

C is the most relevant comparison because Fides is designed as a C replacement.
Both target the same space: systems programming, bare metal, embedded, OS
kernels. Here's an honest feature-by-feature comparison.

### Memory Safety

| Feature | C | Fides |
|---------|---|---|
| Buffer overflow protection | ❌ None | ✅ Bounds checking (debug mode + compile-time for constants) |
| Use-after-free detection | ❌ None | ✅ Region-based, compile-time |
| Dangling pointer detection | ❌ None | ✅ Region escape detection, compile-time |
| Integer overflow checking | ❌ Silent wrap | ✅ Debug mode panics, release wraps |
| Division by zero | ❌ UB / crash | ✅ Runtime panic in debug mode |
| Null pointer dereference | ❌ UB / segfault | ⚠️ Runtime check (no compile-time null tracking yet) |
| Memory leaks | ❌ Manual | ✅ `defer` + region-based cleanup |

**Verdict:** Fides eliminates the entire class of memory safety bugs that cause
70% of C CVEs. Zero runtime cost in most cases (compile-time checks).

### Security

| Feature | C | Fides |
|---------|---|---|
| Capability system | ❌ None | ✅ Compile-time capability checking |
| `sandbox { }` revocation | ❌ Impossible | ✅ Compile-time, zero runtime cost |
| Capability-aware linking | ❌ None | ✅ `--caps` strips ungranted code |
| Effect system | ❌ None | ✅ `!IO`, `!Net`, `!Spawn`, `!{}` (pure) |
| Prove function is pure | ❌ Impossible | ✅ `!{}` annotation, compiler-enforced |
| Prove code can't do IO | ❌ Impossible | ✅ Wrap in `sandbox { }` |
| Prove code can't network | ❌ Impossible | ✅ `--caps "IO"` strips Net code |

**Verdict:** Fides has a security model that C fundamentally cannot have. C's
authority is ambient — any code can call `open()` or `socket()` at any time.
Fides's authority is explicit and revocable, verified at compile time.

### Performance

| Feature | C | Fides |
|---------|---|---|
| Machine code quality | ✅ 10/10 (40+ years of optimizer work) | ⚠️ 7/10 (custom codegen, 23 opt passes, loop-vec + SLP, LTO) |
| Auto-vectorization | ✅ GCC/LLVM auto-vec | ✅ Loop-vec + SLP (SSE2, constant trips) |
| Link-time optimization | ✅ LTO | ✅ LTO (`--lto` flag) |
| Profile-guided optimization | ✅ PGO | ❌ Not yet |
| Register allocation | ✅ Graph coloring (mature) | ⚠️ Linear scan (known bug) |
| Zero-cost abstractions | ✅ Yes | ✅ Yes (capabilities, effects, regions all zero-cost) |
| Inline assembly | ✅ Yes (all architectures) | ⚠️ Yes (`asm!`) — x86 only |
| Bare metal support | ✅ Yes | ✅ Yes (`--bare` mode, interrupt handlers) |
| SIMD | ✅ Yes (intrinsics, auto-vec) | ✅ Yes (stdlib `simd.fd` + auto-vec) |
| Compilation speed | ⚠️ Fast but needs external tools | ⚠️ Not measured against Zig; `fid` ~1,000 lines/s on large code (Nov 6) |
| Binary size | ⚠️ Can be small with effort | ✅ Smaller by default (DCE, no libc, `--caps` stripping) |

**Verdict:** C wins on raw code quality today (LLVM/GCC have 40+ years of
optimization). Fides wins on compilation speed and binary size control. Fides's
codegen will improve over time; C's compilation model won't.

### Developer Experience

| Feature | C | Fides |
|---------|---|---|
| Hot reload | ❌ None | ✅ `fidesc watch` — native JIT, function-level detection |
| Persist state across reloads | ❌ None | ✅ `persist val x = 0;` |
| See optimizer changes | ❌ None | ✅ `fidesc watch --diff` |
| REPL | ❌ None | ✅ `fidesc repl` |
| Error messages | ❌ Cryptic preprocessor errors | ⚠️ Structured but less detailed than Rust/Go |
| Memory-safe debugging | ❌ Valgrind (slow, external) | ✅ Built-in debug mode checks |
| Package manager | ❌ None (make/cmake) | ❌ Not yet (planned Phase 7) |
| LSP / IDE support | ⚠️ clangd (external, mature) | ✅ `fidesc lsp` (completion, hover, goto-def, diagnostics) |
| Debugger | ✅ gdb/lldb (DWARF) | ❌ No DWARF, no debugger |
| Formatter | ⚠️ clang-format (external) | ✅ `fidesc fmt` (--check mode for CI) |
| Linter | ⚠️ clang-tidy (external) | ✅ `fidesc lint` (unused funcs/imports) |
| Profiler | ✅ perf/gprof (external) | ❌ Not yet |
| Stack traces | ⚠️ glibc backtrace | ✅ Call stack trace (interp + C codegen) |
| Documentation generator | ⚠️ Doxygen (external) | ❌ Not yet |
| Test framework | ⚠️ Unity/CMocka (external) | ⚠️ `fidesc test` (basic, file-based) |
| Benchmarking | ⚠️ External (perf, hyperfine) | ✅ `fidesc bench` (-n, --ccompile) |

**Verdict:** Fides dramatically improves iteration speed. Hot reload + persist
+ `watch --diff` means you can tweak code and see results in milliseconds
without losing state. C requires a full recompile.

### Metaprogramming

| Feature | C | Fides |
|---------|---|---|
| Compile-time evaluation | ⚠️ `#define` macros (text substitution) | ✅ `comptime` — type-safe, reflection, code generation |
| Generic code | ⚠️ `_Generic` + macros (ugly) | ✅ Generics `<T>` + traits |
| Reflection | ❌ None | ✅ `comptime` reflection (field_names, field_types, etc.) |
| Code generation | ❌ Preprocessor hacks | ✅ `comptime gen { }` — parse string as Fides source, inject |
| Type-safe macros | ❌ None | ✅ `comptime fn` — compile-time functions with types |

**Verdict:** C's preprocessor is a text substitution engine that creates
bugs. Fides's comptime is a type-safe compile-time evaluation system with
reflection and code generation. Not even close.

### Concurrency

| Feature | C | Fides |
|---------|---|---|
| Threads | ✅ pthreads (manual but works) | ✅ `spawn` + `p_thread_spawn` (OS threads) |
| C11 threads | ✅ `thrd_create`, `mtx_t`, `cnd_t` | ⚠️ pthread mutex+condvar (C codegen) |
| Atomics | ✅ `stdatomic.h` | ❌ Not yet |
| Effect system | ❌ None | ✅ `!Spawn` effect tracking + enforcement |
| Async model | ❌ None (callbacks) | ✅ Executor runtime (`p_executor_*`), non-blocking poll |
| Data race detection | ❌ None | ❌ Not yet (planned) |
| Message passing | ❌ Manual | ✅ Bounded MPSC channels (`p_chan_*`) |
| Mutex / locks | ✅ pthread_mutex | ⚠️ pthread mutex (C codegen) |
| Condition variables | ✅ pthread_cond | ⚠️ pthread condvar (C codegen) |
| Thread-local storage | ✅ `__thread` | ❌ Not yet |
| Select statement | ❌ None | ✅ `select { case ch => x => { } default => { } }` |

**Verdict:** C still wins on concurrency maturity (atomics, thread-local
storage, decades of pthreads usage). But Fides now has working threads,
channels, executor runtime, async IO, and `select` — the core concurrency
toolkit. Fides's effect system gives compile-time tracking that C lacks.
The gap is now about maturity (atomics, TLS) not existence.

### Ecosystem

| Feature | C | Fides |
|---------|---|---|
| Libraries | ✅ Everything (40+ years) | ⚠️ Growing (stdlib + C FFI) |
| C interop | ✅ Native | ✅ `extern "C"` + `parse-header` |
| Package manager | ❌ None | ❌ Not yet |
| Community size | ✅ Massive | ❌ Just starting |
| Documentation | ✅ Vast | ⚠️ Growing (this file, roadmap, features) |
| Migration path | N/A | ✅ C FFI + `parse-header` + planned C-to-Fides transpiler |

**Verdict:** C wins on ecosystem — this is the biggest practical barrier
for Fides. But Fides's C interop + FFI generator means you can use existing C
libraries immediately. The migration path from C exists; the reverse
doesn't.

### Platform Support

| Feature | C | Fides |
|---------|---|---|
| Linux | ✅ Full | ✅ Full (ELF64, native build, hot reload) |
| Windows | ✅ Full (native) | ⚠️ PE emitter, COFF objects (not fully tested) |
| macOS | ✅ Full (native) | ⚠️ Mach-O objects (not fully tested) |
| WASM | ⚠️ Emscripten (external) | ⚠️ Native `fidesc wasm` (no WASI) |
| GPU | ❌ CUDA/OpenCL (external) | ✅ Native SPIR-V `fidesc gpu` |
| Bare metal | ✅ Yes (mature) | ✅ Yes (`--bare`, interrupt handlers) |
| ARM64 | ✅ Full (cross-compile) | ✅ Full (zero-tool cross-compile) |
| RISC-V | ✅ Full (cross-compile) | ✅ Full (zero-tool cross-compile) |
| Cross-compilation | ✅ Mature cross-toolchains | ✅ Zero-tool (`--target arm64-linux`) |
| DWARF debug info | ✅ Full | ❌ Not yet |
| Mobile (iOS/Android) | ✅ NDK / toolchains | ❌ Not yet |
| Embedded (RTOS) | ✅ FreeRTOS, Zephyr, etc. | ⚠️ `--bare` but no RTOS integration |
| Dynamic loading | ✅ dlopen/dlsym | ✅ `p_dl_open/sym/call` |

**Verdict:** Fides matches C on platform coverage and exceeds it on WASM and
GPU (native backends vs external tools). Fides generates object files for all
3 major formats (ELF, PE, Mach-O) with zero external dependencies.

### Summary: Where Fides Beats C

| Dimension | Winner | Why |
|-----------|--------|-----|
| Memory safety | **Fides** | Compile-time, zero cost. C has nothing. |
| Security | **Fides** | Capabilities + sandbox + cap-aware linking. C has nothing. |
| Compilation speed | **Fides** | No external tools, integrated linker. |
| Developer velocity | **Fides** | Hot reload + persist + watch --diff + REPL. |
| Metaprogramming | **Fides** | Type-safe comptime vs text-substitution macros. |
| Binary size control | **Fides** | DCE + `--caps` stripping. C needs manual effort. |
| WASM/GPU | **Fides** | Native backends vs external tools. |
| Ecosystem | **C** | 40+ years of libraries. Fides is just starting. |
| Code quality | **C** | GCC/LLVM have 40+ years of optimization. Auto-vectorization, LTO, PGO. |
| Community | **C** | Massive vs just starting. |
| Concurrency | **Tie** | C has pthreads + atomics + TLS. Fides has threads + channels + select + executor + effect tracking. Both work. C has atomics/TLS, Fides has channels/select/effects. |
| Cross-compilation | **A** | Zero-tool cross-compilation to ARM64, RISC-V, and Windows. No cross-toolchain installation needed. |
| Debugging | **C** | Full DWARF debug info, gdb/lldb. Fides has stack traces but no debugger. |
| C++ interop | **C** | Native. Fides is C-only. |
| Portability | **C** | Runs everywhere with a C compiler. Fides needs its own toolchain. |
| Inline assembly (multi-arch) | **C** | gcc/clang inline asm on all arches. Fides is x86 only. |

**Bottom line:** Fides is a safer, faster-to-compile, better DX C replacement
with compile-time security guarantees C cannot match. C's real advantages are
ecosystem (40+ years of libraries), optimizer maturity (40+ years of GCC/LLVM),
concurrency (pthreads/atomics work today), debugging (DWARF + gdb), and
portability (runs everywhere). Fides's advantages are structural (safety,
security, compilation speed); C's advantages are practical (maturity, breadth).

---

## Fides vs Rust — Detailed Head-to-Head

Rust is the most relevant modern comparison because both target safe
systems programming. Both have compile-time memory safety. The key
difference: *how* they achieve it and what it costs the developer.

### Memory Safety

| Feature | Rust | Fides |
|---------|------|---|
| Mechanism | Borrow checker (ownership + lifetimes) | Regions (lexical scopes) |
| Compile-time? | ✅ Yes | ✅ Yes |
| Runtime cost | ✅ Zero | ✅ Zero |
| Learning curve | ❌ Steep (lifetimes, Send/Sync, Pin) | ✅ Gentle (regions are just scopes) |
| False rejections | ❌ Common (borrow checker rejects valid patterns) | ✅ Rare (regions only constrain escape) |
| Self-referential structs | ❌ Requires `Pin` + `unsafe` | ✅ Allowed (region-scoped) |
| Graph structures | ❌ Requires `Rc<RefCell>` or `unsafe` | ✅ Region-allocated, natural |
| Iterator invalidation | ❌ Prevented via borrow rules | ✅ Prevented via region escape detection |
| Use-after-free | ✅ Prevented | ✅ Prevented (region freed at scope end) |
| Double free | ✅ Prevented | ✅ Prevented (no manual free) |
| Memory leaks | ⚠️ Possible (`Rc` cycles, `mem::forget`) | ✅ Region cleanup is deterministic |

**Verdict:** Both achieve compile-time memory safety. Rust's borrow checker
is more precise (tracks individual borrows) but rejects valid patterns and
has a steep learning curve. Fides's regions are coarser (scope-based) but never
reject valid code and are trivial to understand. Rust wins on precision;
Fides wins on ergonomics.

### Security

| Feature | Rust | Fides |
|---------|------|---|
| Capability system | ❌ None | ✅ Compile-time capabilities |
| `sandbox { }` revocation | ❌ Impossible | ✅ Compile-time, zero cost |
| Capability-aware linking | ❌ None | ✅ `--caps` strips ungranted code |
| Effect system | ❌ None (no tracked effects) | ✅ `!IO`, `!Net`, `!Spawn`, `!{}` |
| Prove function is pure | ❌ No effect tracking | ✅ `!{}` annotation |
| Prove code can't do IO | ❌ Impossible | ✅ `sandbox { }` |
| `unsafe` escape hatch | ⚠️ Yes (bypasses safety) | ✅ `asm!` is scoped, no blanket `unsafe` |
| Memory safety bypass | ⚠️ `unsafe` blocks | ✅ No bypass (regions always enforced) |

**Verdict:** Fides has a security model Rust fundamentally lacks. Rust's
`unsafe` is a blanket escape hatch — once you're in `unsafe`, anything goes.
Fides has no `unsafe` — capabilities are always enforced, and `sandbox { }`
can revoke them in any scope. Rust cannot prove a function doesn't do IO;
Fides can.

### Performance

| Feature | Rust | Fides |
|---------|------|---|
| Machine code quality | ✅ 9/10 (LLVM backend, 20+ years) | ⚠️ 7/10 (custom codegen, 23 opt passes, loop-vec + SLP, LTO) |
| Auto-vectorization | ✅ LLVM auto-vec (mature, variable trips) | ✅ Loop-vec + SLP (SSE2, constant trips only) |
| Link-time optimization | ✅ LTO | ✅ LTO (`--lto` flag) |
| Profile-guided optimization | ✅ PGO | ❌ Not yet |
| Register allocation | ✅ Graph coloring (LLVM) | ⚠️ Linear scan (known bug) |
| Zero-cost abstractions | ✅ Yes | ✅ Yes |
| Inline assembly | ✅ Yes (`asm!`, all arches) | ⚠️ Yes (`asm!`) — x86 only |
| Bare metal | ✅ Yes (`no_std`) | ✅ Yes (`--bare`) |
| SIMD | ✅ Yes (std::simd, auto-vec) | ✅ Yes (stdlib `simd.fd` + auto-vec) |
| Compilation speed | ❌ Slow on large projects | ⚠️ Sub-second for small programs; ~133 s for `fid`'s own 133,688 lines (Nov 6) |
| Binary size | ⚠️ Moderate (monomorphization bloat) | ✅ Small (DCE + `--caps` stripping) |
| Escape analysis | ✅ LLVM | ❌ Not yet |
| Tail call optimization | ✅ LLVM | ✅ TCO (enabled by default) |

**Verdict:** Rust wins on code quality today (LLVM is mature): fib(40) runs
in 0.53 s against 1.34 s for native Fides (Nov 2). Compile speed is meant to
be Fides's advantage, but it is not yet measured against Rust on the same
program (competitive roadmap BM6).

### Developer Experience

| Feature | Rust | Fides |
|---------|------|---|
| Hot reload | ❌ None (full recompile) | ⚠️ `fidesc watch` recompiles and restarts (Linux) |
| Persist state across reloads | ❌ None | ✅ `persist val x = 0;` |
| See optimizer changes | ❌ None (LLVM black box) | ✅ `fidesc watch --diff` |
| REPL | ❌ None (evcxr is external) | ✅ `fidesc repl` |
| Error messages | ✅ Excellent (detailed, helpful, with suggestions) | ⚠️ Basic (line/column, no suggestions) |
| Borrow checker fights | ❌ Daily reality | ✅ Simpler borrow checker: no lifetime annotations, references cannot be stored |
| Async complexity | ❌ "Suffering" (their word) | ✅ Effect handlers + executor (no coloring) |
| Compile times | ❌ Slow on large projects | ⚠️ Sub-second for small programs; not yet compared on large ones |
| Incremental compilation | ⚠️ Partial (still slow) | ⚠️ Function-level change detection; whole program rebuilt |
| IDE: diagnostics | ✅ rust-analyzer | ✅ `fidesc lsp` |
| IDE: completion | ✅ rust-analyzer | ✅ `fidesc lsp` (keywords + symbols) |
| IDE: goto-def | ✅ rust-analyzer | ✅ `fidesc lsp` |
| IDE: hover | ✅ rust-analyzer | ✅ `fidesc lsp` (type signatures) |
| IDE: rename | ✅ rust-analyzer | ❌ Not yet |
| IDE: code actions | ✅ rust-analyzer | ❌ Not yet |
| Package manager | ✅ cargo (gold standard) | ❌ Not yet |
| Formatter | ✅ rustfmt | ✅ `fidesc fmt` (--check mode) |
| Linter | ✅ clippy | ✅ `fidesc lint` (unused funcs/imports) |
| Debugger | ✅ gdb/lldb (DWARF) | ❌ No debugger (stack traces work) |
| Profiler | ✅ perf/flamegraph | ❌ Not yet |
| Doc generator | ✅ rustdoc (docs.rs) | ❌ Not yet |
| Test framework | ✅ `#[test]` (mature, fixtures, parametrize) | ⚠️ `fidesc test` (basic, file-based) |
| Fuzzing | ✅ cargo-fuzz | ❌ Not yet |
| Stack traces | ✅ Full backtrace | ✅ Call stack trace (interp + C codegen) |
| Benchmarking | ✅ criterion | ✅ `fidesc bench` (-n, --ccompile) |

**Verdict:** Fides dramatically outperforms Rust on developer velocity. Hot
reload + persist + fast compilation means Fides iteration is measured in
milliseconds, Rust in minutes. Rust wins on tooling maturity (cargo,
rust-analyzer) — Fides needs to build that infrastructure.

### Metaprogramming

| Feature | Rust | Fides |
|---------|------|---|
| Compile-time evaluation | ⚠️ `const fn` (limited but stable) | ✅ `comptime` (full evaluation) |
| Macros | ⚠️ Two systems (declarative + proc) | ✅ `comptime gen { }` (one system) |
| Reflection | ❌ None (no reflection in safe Rust) | ✅ `comptime` reflection |
| Code generation | ⚠️ Proc macros (external crate, complex) | ✅ `comptime gen { }` (built-in) |
| Type-safe macros | ⚠️ Proc macros are not type-checked | ✅ `comptime fn` is type-safe |
| Derive macros | ✅ `#[derive(Debug, Clone, Serialize)]` (powerful) | ✅ `@derive(Debug, Clone, Serialize, Deserialize)` |
| Build scripts | ⚠️ `build.rs` (separate file) | ✅ `comptime` (inline, same file) |
| Serde (serialization) | ✅ `#[derive(Serialize, Deserialize)]` | ✅ `@derive(Serialize, Deserialize)` → JSON |
| Proc macro ecosystem | ✅ Large (syn, quote, proc-macro2) | ❌ N/A (comptime is the only system) |

**Verdict:** Fides's comptime is simpler and more powerful than Rust's macro
system. One unified system vs two. Type-safe vs not. Built-in vs external
crate. Rust's proc macros are powerful but notoriously hard to write and
debug. Fides's comptime is just Fides code running at compile time.

### Concurrency

| Feature | Rust | Fides |
|---------|------|---|
| Threading | ✅ `std::thread` (safe via Send/Sync) | ✅ `spawn` + `p_thread_spawn` (OS threads) |
| Data race prevention | ✅ Compile-time (Send/Sync) — proven at scale | ❌ Not yet (planned) |
| Async model | ❌ async/await (function coloring, but works) | ✅ Executor runtime + non-blocking poll |
| Function coloring | ❌ `async fn` vs `fn` | ✅ No coloring (effect handlers) |
| `Send`/`Sync` complexity | ❌ Major pain point | ✅ Not needed (effect system) |
| `Pin` safety | ❌ Required for self-referential futures | ✅ Not needed |
| Tokio ecosystem | ✅ Mature (but complex) | ⚠️ Basic executor (`p_executor_*`) |
| Effect-based concurrency | ❌ No | ✅ `!Spawn` effect tracking + enforcement |
| Atomics | ✅ `std::sync::atomic` | ❌ Not yet |
| Mutex / RwLock | ✅ `std::sync` | ⚠️ pthread mutex (C codegen) |
| Channels | ✅ `std::sync::mpsc` | ✅ Bounded MPSC (`p_chan_*`) |
| Parallel iterators | ✅ rayon | ⚠️ `stdlib/parallel.fd` (basic) |
| Async I/O | ✅ tokio::fs, tokio::net | ✅ `p_net_*`, `p_fs_read_async` |
| Thread-safe ref counting | ✅ `Arc<T>` | ❌ Not yet |
| Select statement | ❌ None | ✅ `select { case ch => x => { } }` |

**Verdict:** Rust still wins on concurrency maturity — Send/Sync data race
prevention is proven at scale, Tokio is a mature ecosystem, atomics and
Arc<T> work. But Fides now has working concurrency: OS threads, bounded
MPSC channels, executor runtime, async IO, and `select`. The gap is now
about safety (Send/Sync) and ecosystem depth (Tokio vs basic executor),
not about existence. Fides's effect-based approach avoids function coloring
and Pin complexity — a genuine ergonomic advantage.

### Ecosystem

| Feature | Rust | Fides |
|---------|------|---|
| Libraries | ✅ crates.io (100k+ crates) | ⚠️ ~99 stdlib modules, no external packages |
| Package manager | ✅ cargo (gold standard) | ❌ Not yet |
| Documentation | ✅ docs.rs (auto-generated, hosted) | ⚠️ Markdown docs only |
| Community | ✅ Large, active, welcoming (90k+ stars) | ❌ Just starting |
| C interop | ✅ FFI (works but verbose) | ✅ `extern "C"` + `parse-header` |
| C++ interop | ✅ `extern "C++"` (cxx crate) | ❌ C only |
| Migration from C | ❌ Requires full rewrite | ✅ C FFI + planned transpiler |
| Corporate backing | ✅ Mozilla, AWS, Google, Microsoft | ❌ Independent |
| Production users | ✅ AWS, Cloudflare, Discord, Firefox | ❌ None known |
| Serialization | ✅ serde (gold standard) | ✅ `@derive(Serialize, Deserialize)` → JSON |
| Web framework | ✅ actix, axum, rocket | ⚠️ Basic (stdlib/http.fd) |
| Database ORM | ✅ diesel, sqlx | ⚠️ Basic (stdlib/db.fd) |
| HTTP client | ✅ reqwest | ⚠️ Basic (stdlib/http.fd) |
| TLS/SSL | ✅ rustls | ❌ Not yet |
| Crypto | ✅ ring, rustls | ❌ Not yet |
| Async runtime | ✅ Tokio (mature) | ✅ `p_executor_*` (basic) |
| GUI toolkit | ✅ egui, iced, tauri | ❌ Not yet |
| Game engine | ✅ bevy | ⚠️ Game stdlib (tilemap, sprite, physics, audio, input) |

**Verdict:** Rust wins decisively on ecosystem. This is Fides's biggest
barrier. cargo + crates.io + docs.rs is a mature, integrated experience.
Fides has the foundation (C interop, FFI generator) but needs a package
manager and community.

### Platform Support

| Feature | Rust | Fides |
|---------|------|---|
| Linux | ✅ Full | ✅ Full |
| Windows | ✅ Full (native) | ⚠️ PE emitter, COFF objects (not fully tested) |
| macOS | ✅ Full (native) | ⚠️ Mach-O objects (not fully tested) |
| WASM | ✅ Full (wasm32 target, WASI) | ⚠️ Binary output, no WASI support |
| GPU | ❌ No native GPU backend | ✅ Native SPIR-V `fidesc gpu` |
| Bare metal | ✅ `no_std` (mature) | ✅ `--bare` mode |
| ARM64 | ✅ Full (cross-compile) | ✅ Assembly codegen |
| RISC-V | ✅ Full (cross-compile) | ✅ Assembly codegen |
| Cross-compilation | ✅ Excellent (`cargo build --target`) | ✅ Zero-tool (`--target arm64-linux`) |
| Mobile (iOS/Android) | ✅ cargo + targets | ❌ Not yet |
| Embedded (RTOS) | ✅ no_std + embedded HAL | ⚠️ `--bare` but no RTOS integration |
| DWARF debug info | ✅ Full | ❌ Not yet |
| Dynamic loading | ✅ libloading crate | ✅ `p_dl_open/sym/call` |

**Verdict:** Rust wins on cross-compilation and platform maturity. Fides wins
on GPU (native SPIR-V vs nothing) and matches on everything else. Fides's
multi-target codegen works but lacks Rust's polished cross-compilation
tooling.

### Summary: Where Fides Beats Rust

| Dimension | Winner | Why |
|-----------|--------|-----|
| Compilation speed | Not measured | Goal, not result: no same-program comparison yet (BM6). |
| Developer velocity | **Fides** (claimed) | Recompile-on-save + persist + watch --diff + a simpler borrow checker. |
| Security model | **Fides** | Capabilities + sandbox + cap-aware linking. Rust has nothing. |
| Effect system | **Fides** | `!IO`, `!Net`, `!Spawn` tracking. Rust has no effects. |
| Metaprogramming | **Fides** | One unified comptime vs two macro systems. Simpler, type-safe. |
| Async model | **Fides** | Effect handlers + executor (no coloring). Rust async is "suffering". |
| Learning curve | **Fides** | Regions are scopes. Borrow checker takes months. |
| GPU support | **Fides** | Native SPIR-V backend. Rust has no GPU backend. |
| Binary size control | **Fides** | DCE + `--caps` stripping. Rust has monomorphization bloat. |
| Code quality | **Rust** | LLVM 20+ years. Auto-vectorization (variable trips), LTO, PGO, graph-coloring regalloc. Fides has auto-vec (constant trips only). |
| Data race prevention | **Rust** | Send/Sync is proven and compile-time. Fides has nothing. |
| Concurrency maturity | **Rust** | std::thread + Send/Sync + Tokio + atomics + Arc. Fides has threads + channels + select + executor but no data race prevention. |
| Ecosystem | **Rust** | cargo + crates.io (100k+) + docs.rs. Fides is just starting. |
| IDE support | **Rust** | rust-analyzer: completion, hover, goto-def, rename, refactor. Fides LSP has completion/hover/goto-def but no rename/code actions. |
| Tooling | **Rust** | cargo + rustfmt + clippy + rustdoc + cargo-fuzz + criterion. Fides has fmt + lint + bench but no debugger/profiler/doc-gen/package-manager. |
| Corporate backing | **Rust** | Mozilla, AWS, Google, Microsoft. Fides is independent. |
| Community | **Rust** | Large, active, 90k+ GitHub stars. Fides is just starting. |
| Standard library | **Rust** | Vast (collections, net, io, sync, fs, process, etc.). Fides has 137 modules. |
| Error messages | **Rust** | Gold standard — detailed, helpful, with suggestions. Fides has structured errors with codes + suggestions. |
| Debugging | **Rust** | Full DWARF, gdb/lldb, backtraces. Fides has stack traces but no debugger. |
| Cross-compilation | **Tie** | Rust has `cargo build --target`. Fides has zero-tool `--target arm64-linux|riscv64-linux|x86-64-windows`. Both work well. |
| Serialization | **Tie** | Rust has serde (gold standard). Fides has `@derive(Serialize/Deserialize)` → JSON. |

**Bottom line:** Fides beats Rust on compilation speed (10-100x), developer
velocity (hot reload), security (capabilities + sandbox), and metaprogramming
simplification. Rust beats Fides on practically everything else: ecosystem
maturity, code quality (LLVM), concurrency (Send/Sync + Tokio), tooling
(cargo + rust-analyzer + clippy + rustfmt), standard library depth, error
messages, debugging, and community. Fides's advantage is that Rust's pain
points (compile times, borrow checker, async complexity) are *structural* —
they won't get better. Fides's gaps (ecosystem, code quality, concurrency,
tooling) are *incremental* — they improve with time and contributors. But
"incremental" still means years of work.

---

## Fides vs Zig — Detailed Head-to-Head

Zig is the closest philosophical comparison: both are C replacements,
both have comptime, both target systems programming, both have explicit
allocation. The key difference: Fides has memory safety and capabilities;
Zig has neither.

### Memory Safety

| Feature | Zig | Fides |
|---------|-----|---|
| Mechanism | None (manual, like C) | Regions (compile-time) |
| Buffer overflow | ❌ Runtime panic (debug mode) | ✅ Compile-time + runtime (debug) |
| Use-after-free | ❌ None | ✅ Compile-time (region escape detection) |
| Dangling pointer | ❌ None | ✅ Compile-time (region freed at scope end) |
| Double free | ❌ Manual (can happen) | ✅ Impossible (no manual free) |
| Memory leaks | ❌ Manual (can happen) | ✅ Region cleanup is deterministic |
| Integer overflow | ⚠️ Runtime panic (debug) | ✅ Runtime panic (debug), wrap (release) |
| Bounds checking | ⚠️ Runtime (debug mode) | ✅ Runtime (debug) + compile-time (constants) |
| Allocator model | ✅ Explicit allocators (excellent) | ✅ Regions (similar, but compile-time checked) |
| `defer` cleanup | ✅ Yes | ✅ Yes |

**Verdict:** This is Fides's biggest win over Zig. Zig has *no memory safety* —
it relies on programmer discipline, exactly like C. Fides has compile-time
region-based safety with zero runtime cost. Zig's explicit allocators are
excellent design, but they don't prevent use-after-free or dangling
pointers. Fides's regions do.

### Security

| Feature | Zig | Fides |
|---------|-----|---|
| Capability system | ❌ None | ✅ Compile-time capabilities |
| `sandbox { }` revocation | ❌ Impossible | ✅ Compile-time, zero cost |
| Capability-aware linking | ❌ None | ✅ `--caps` strips ungranted code |
| Effect system | ❌ None | ✅ `!IO`, `!Net`, `!Spawn`, `!{}` |
| Prove function is pure | ❌ Impossible | ✅ `!{}` annotation |
| Prove code can't do IO | ❌ Impossible | ✅ `sandbox { }` |
| Memory safety | ❌ None | ✅ Compile-time, zero cost |
| Compile-time memory budget | ❌ None (runtime panic only) | ✅ `region r with budget(N)` |

**Verdict:** Fides has a complete security model. Zig has none. Zig is a
better C — fast, explicit, no safety. Fides is a safe C — fast, explicit,
*with* safety and security. If you want C without the bugs, Fides is the
answer, not Zig.

### Performance

| Feature | Zig | Fides |
|---------|-----|---|
| Machine code quality | ✅ 9/10 (LLVM backend) | ⚠️ 7/10 (custom codegen, 23 opt passes, loop-vec + SLP, LTO) |
| Auto-vectorization | ✅ LLVM auto-vec (mature) | ✅ Loop-vec + SLP (SSE2, constant trips only) |
| Link-time optimization | ✅ LTO | ✅ LTO (`--lto` flag) |
| Profile-guided optimization | ✅ PGO | ❌ Not yet |
| Register allocation | ✅ LLVM (graph coloring) | ⚠️ Linear scan (known bug) |
| Zero-cost abstractions | ✅ Yes | ✅ Yes |
| Inline assembly | ✅ Yes (all arches) | ⚠️ Yes — x86 only |
| Bare metal | ✅ Yes (excellent) | ✅ Yes |
| SIMD | ✅ Yes (@Vector, auto-vec) | ✅ Yes (stdlib `simd.fd` + auto-vec) |
| Compilation speed | ✅ 7/10 (fast, but LLVM link) | ✅ 10/10 (no LLVM at all) |
| Binary size | ✅ Small (good DCE) | ✅ Small (DCE + `--caps`) |
| Comptime specialization | ✅ Yes (powerful) | ✅ Yes (powerful) |
| Cross-compilation | ✅ Excellent (built-in) | ✅ Zero-tool cross-compile |

**Verdict:** Zig wins on code quality (LLVM) and cross-compilation
(built-in, excellent). Fides wins on compilation speed (no LLVM dependency).
Both are fast — this is close. Zig's LLVM backend gives it an edge on
optimization quality today.

### Developer Experience

| Feature | Zig | Fides |
|---------|-----|---|
| Hot reload | ⚠️ Partial (Zap, not native) | ✅ `fidesc watch` (native JIT) |
| Persist state across reloads | ❌ None | ✅ `persist val x = 0;` |
| See optimizer changes | ❌ None (LLVM black box) | ✅ `fidesc watch --diff` |
| REPL | ❌ None | ✅ `fidesc repl` |
| Error messages | ✅ Excellent (detailed, helpful) | ✅ Structured (error codes, source context, suggestions) |
| Memory safety debugging | ❌ Valgrind (external, slow) | ✅ Built-in debug mode checks |
| Build system | ✅ Built-in (`build.zig` — excellent) | ❌ No build system |
| Package manager | ⚠️ Zon (immature but exists) | ❌ Not yet |
| Cross-compilation | ✅ One command (`-Dtarget=`) | ✅ Zero-tool (`--target`) |
| Formatter | ✅ `zig fmt` | ✅ `fidesc fmt` (--check mode) |
| Debugger | ✅ lldb (DWARF) | ❌ No debugger (stack traces work) |
| Stack traces | ✅ Stack trace on panic | ✅ Call stack trace (interp + C codegen) |
| LSP | ✅ zls (community) | ✅ `fidesc lsp` (completion, hover, goto-def, diagnostics) |
| Linter | ❌ None | ✅ `fidesc lint` (unused funcs/imports) |
| Benchmarking | ⚠️ Basic | ✅ `fidesc bench` (-n, --ccompile) |
| Documentation | ✅ autodoc | ❌ No doc generator |

**Verdict:** Mixed. Zig wins on build system (Zig's build system is
excellent) and cross-compilation. Fides wins on hot reload, persist, and
watch --diff. Both are fast to compile. Fides's debug-mode safety checks
are a big DX win that Zig lacks entirely.

### Metaprogramming

| Feature | Zig | Fides |
|---------|-----|---|
| Compile-time evaluation | ✅ `comptime` (excellent) | ✅ `comptime` (excellent) |
| Type-safe at declaration | ⚠️ No (types inferred at use) | ✅ Yes (types checked at declaration) |
| Reflection | ⚠️ Limited (@typeInfo) | ✅ Full (field_names, field_types, etc.) |
| Code generation | ✅ Inline assembly + comptime | ✅ `comptime gen { }` (parse + inject) |
| Generic types | ✅ `comptime` parameters | ✅ Generics `<T>` + traits |
| Build scripts | ✅ `build.zig` (Zig code) | ✅ `comptime` (inline, same file) |
| Unified system | ✅ One system (comptime) | ✅ One system (comptime) |

**Verdict:** Close. Both have unified comptime systems — this is a shared
advantage over Rust and C. Zig's comptime is more mature (longer in
development). Fides's comptime has better reflection and type-safety at
declaration. Both are excellent; this is roughly a tie.

### Concurrency

| Feature | Zig | Fides |
|---------|-----|---|
| Threading | ✅ `std.Thread` (works) | ✅ `spawn` + `p_thread_spawn` (OS threads) |
| Atomics | ✅ `@atomicLoad`, `@atomicStore`, `@atomicRmw` | ❌ Not yet |
| Data race prevention | ❌ None | ❌ Not yet (planned) |
| Async model | ⚠️ Basic (no async/await) | ✅ Executor runtime + non-blocking poll |
| Effect system | ❌ None | ✅ `!Spawn` effect tracking + enforcement |
| Message passing | ❌ None | ✅ Bounded MPSC channels (`p_chan_*`) |
| Mutex | ✅ `std.Thread.Mutex` | ⚠️ pthread mutex (C codegen) |
| Condition variables | ✅ `std.Thread.Condition` | ⚠️ pthread condvar (C codegen) |
| Select statement | ❌ None | ✅ `select { case ch => x => { } }` |

**Verdict:** Mixed. Zig wins on atomics and build system. Fides wins on
channels, executor runtime, select, and effect tracking. Both have working
threads. Neither has data race prevention. Fides's channel + select model
is closer to Go's concurrency; Zig's is closer to C's pthreads. Fides's
effect system gives compile-time tracking that Zig lacks.

### Ecosystem

| Feature | Zig | Fides |
|---------|-----|---|
| Libraries | ⚠️ Small but growing | ⚠️ Small (~99 stdlib modules) |
| Package manager | ⚠️ Zon (immature but exists) | ❌ Not yet |
| Documentation | ⚠️ Growing (autodoc) | ⚠️ Markdown docs only |
| Community | ✅ Active, passionate (30k+ stars) | ❌ Just starting |
| C interop | ✅ Excellent (best in class) | ✅ Good (`extern "C"` + `parse-header`) |
| C translation | ✅ `zig translate-c` | ❌ Planned (transpiler) |
| Corporate backing | ⚠️ Zig Foundation | ❌ Independent |
| Production users | ⚠️ Bun, Uber (growing) | ❌ None known |
| Build system | ✅ `build.zig` (excellent) | ❌ No build system |
| Cross-compilation | ✅ Built-in, excellent | ✅ Zero-tool (`--target`) |

**Verdict:** Zig wins on ecosystem maturity — it's been around longer,
has a package manager, and has excellent C interop (translate-c is
best-in-class). Fides's C interop is good but not at Zig's level yet. Both
are small ecosystems; both need to grow.

### Platform Support

| Feature | Zig | Fides |
|---------|-----|---|
| Linux | ✅ Full | ✅ Full |
| Windows | ✅ Full (native) | ⚠️ PE emitter, COFF objects (not fully tested) |
| macOS | ✅ Full (native) | ⚠️ Mach-O objects (not fully tested) |
| WASM | ✅ Full (wasm32 target, WASI) | ⚠️ Binary output, no WASI support |
| GPU | ❌ No native GPU backend | ✅ Native SPIR-V `fidesc gpu` |
| Bare metal | ✅ Excellent (mature) | ✅ Yes (`--bare`) |
| ARM64 | ✅ Full (cross-compile) | ✅ Full (zero-tool cross-compile) |
| RISC-V | ✅ Full (cross-compile) | ✅ Full (zero-tool cross-compile) |
| Cross-compilation | ✅ Excellent (built-in, one command) | ✅ Zero-tool (`--target arm64-linux`) |
| DWARF debug info | ✅ Full | ❌ Not yet |
| C translation | ✅ `zig translate-c` | ❌ Planned |

**Verdict:** Zig wins on cross-compilation (best-in-class, built-in).
Fides wins on GPU (native SPIR-V). Both cover the same platforms. Zig's
cross-compilation is a real advantage — `zig build -Dtarget=aarch64-linux`
just works. Fides requires per-target codegen invocation.

### Summary: Where Fides Beats Zig

| Dimension | Winner | Why |
|-----------|--------|-----|
| Memory safety | **Fides** | Run-time bounds and overflow checks; compile-time borrow and reference-escape checks; no GC, values the compiler cannot prove finished leak. Zig has nothing comparable. |
| Security | **Fides** | Capabilities + sandbox + cap-aware linking. Zig has nothing. |
| Effect system | **Fides** | `!IO`, `!Net`, `!Spawn` tracking. Zig has no effects. |
| Compile-time memory budget | **Fides** | `region r with budget(N)`. Zig panics at runtime. |
| Hot reload | **Fides** | Native JIT + persist. Zig has partial (Zap). |
| Developer velocity | **Fides** | Hot reload + persist + watch --diff + safety checks. |
| GPU support | **Fides** | Native SPIR-V. Zig has no GPU backend. |
| Code quality | **Zig** | LLVM backend. Auto-vectorization (variable trips), LTO, PGO. Fides has auto-vec (constant trips only). |
| Cross-compilation | **Tie** | Zig has `zig build -Dtarget=`. Fides has zero-tool `--target arm64-linux|riscv64-linux|x86-64-windows`. Both work well. |
| Build system | **Zig** | `build.zig` is excellent. Fides has no build system. |
| C interop | **Zig** | `translate-c` is best-in-class. Fides is good but not there. |
| Ecosystem | **Zig** | Longer development, package manager (immature but exists), community. |
| Comptime maturity | **Zig** | Longer in development, more battle-tested. |
| Concurrency | **Tie** | Zig has atomics + TLS. Fides has channels + select + executor + effect tracking. Both have threads. Neither has data race prevention. |
| Debugging | **Zig** | DWARF debug info, lldb support. Fides has stack traces but no debugger. |
| Error messages | **Tie** | Both have excellent error messages. Fides has error codes + source context + suggestions. |
| Community | **Zig** | 30k+ GitHub stars, passionate community. Fides is just starting. |
| Corporate backing | **Zig** | Zig Foundation. Fides is independent. |

**Bottom line:** Fides's advantage over Zig is *safety and security*. Zig is a
better C — fast, explicit, comptime — but it's still C: no memory safety,
no security model, no effect tracking. Fides is a safe C with the same
performance and comptime story, plus compile-time security guarantees
Zig cannot match. Zig's advantages over Fides are practical: better
cross-compilation, better C interop, working concurrency, DWARF debugging,
more mature ecosystem, and LLVM code quality. Fides's advantages are
structural: safety and security that Zig fundamentally lacks.

---

## Fides vs Go — Detailed Head-to-Head

Go is the most popular modern systems language for servers and cloud
infrastructure. Both Fides and Go target systems programming, but Go has
a runtime and GC while Fides is bare-metal with manual region management.

### Where Go Beats Fides

| Feature | Go | Fides |
|---------|-----|-------|
| Concurrency | ✅ Goroutines + channels (excellent, battle-tested) | ✅ Threads + channels + select + executor |
| Standard library | ✅ Vast (net, http, json, crypto, sql, encoding, etc.) | ⚠️ 137 modules, basic |
| Tooling | ✅ go fmt + go vet + gopls + pprof + delve | ⚠️ fmt + lint + LSP + bench (no debugger/profiler) |
| Cross-compilation | ✅ `GOOS=x GOARCH=y go build` | ✅ Zero-tool (`--target`) |
| Build speed | ✅ Fast (compiled language) | ✅ Faster (no runtime, no LLVM) |
| Community | ✅ Huge (Google-backed, 120k+ GitHub stars) | ❌ Just starting |
| Production usage | ✅ Google, Uber, Twitch, Docker, Kubernetes | ❌ None known |
| Documentation | ✅ pkg.go.dev (auto-generated) | ❌ No doc generator |
| Testing | ✅ `testing` package (mature, builtin benchmarks) | ⚠️ `fidesc test` (basic, file-based) |
| Error handling | ✅ `error` interface (simple, explicit) | ✅ `Result<T, E>` + `?` + `try {}` (type-safe) |
| HTTP server | ✅ `net/http` (production-grade) | ⚠️ Basic (stdlib/http.fd) |
| JSON | ✅ `encoding/json` (reflection-based, mature) | ✅ `@derive(Serialize/Deserialize)` (stdlib/json.fd) |
| Database | ✅ `database/sql` (driver interface) | ⚠️ Basic (stdlib/db.fd) |
| Crypto/TLS | ✅ `crypto/tls` (production-grade) | ❌ Not yet |
| Goroutines | ✅ Lightweight, millions possible | ❌ OS threads only (green threads planned) |
| Channels | ✅ Built-in, type-safe | ✅ Bounded MPSC (`p_chan_*`) |
| Select | ✅ Built-in | ✅ `select { case ... }` |
| Race detector | ✅ Built-in (`go test -race`) | ❌ Not yet |
| Backwards compat | ✅ Go 1.x promise (no breaking changes) | ❌ No stability promise yet |
| Module system | ✅ Go modules (versioned, reproducible) | ⚠️ Package manager (ark) — path/git deps done, registry pending |

### Where Fides Beats Go

| Feature | Fides | Go |
|---------|-------|-----|
| Memory safety | ✅ Compile-time borrow and escape checks, no GC (leaks what it cannot prove finished) | ❌ GC (runtime, stop-the-world pauses) |
| No GC pauses | ✅ No runtime, no GC | ❌ GC pauses (even if short) |
| Deterministic cleanup | ✅ Region-based, `defer` | ❌ GC is non-deterministic |
| Zero runtime overhead | ✅ No runtime at all | ❌ Goroutine scheduler, GC, stack growth |
| Binary size | ✅ Tiny (no runtime, DCE, `--caps`) | ❌ Includes runtime (~2MB minimum) |
| Capability security | ✅ Compile-time capabilities + sandbox | ❌ None |
| Effect system | ✅ `!IO`, `!Net`, `!Spawn` | ❌ None |
| Metaprogramming | ✅ comptime (type-safe, reflection) | ❌ No metaprogramming (generics only) |
| Hot reload | ✅ Native JIT + persist | ❌ None (full rebuild) |
| Bare metal | ✅ `--bare` mode | ❌ Requires runtime |
| Inline assembly | ✅ `asm!` | ❌ No inline assembly |
| Custom toolchain | ✅ Self-hosted, no LLVM | ❌ Uses gc/gccgo toolchain |
| GPU | ✅ Native SPIR-V | ❌ No GPU backend |
| WASM (no runtime) | ✅ Pure WASM | ❌ WASM includes Go runtime (large) |
| Memory budget | ✅ `region r with budget(N)` compile-time | ❌ GC can't prove memory bounds |

**Bottom line:** Go wins on everything practical: concurrency (goroutines are
excellent), standard library (vast), tooling (fmt + vet + gopls + delve +
pprof), community (Google-backed), and production usage (Docker, Kubernetes,
Uber). Fides wins on everything structural: no GC, no runtime, compile-time
safety, capabilities, effects, hot reload, bare metal. Go is the pragmatic
choice today; Fides is the principled choice for safety-critical and
resource-constrained systems. Go's concurrency story alone makes it a better
choice for servers and cloud infrastructure — Fides can't compete there yet.

---

## Fides vs Python — Honest Comparison

Python is the most popular programming language in the world. It's not a
systems language, but it's the language Fides users will compare against
for developer experience, library availability, and ease of use.

### Where Python Beats Fides

| Feature | Python | Fides |
|---------|--------|-------|
| Ecosystem | ✅ PyPI (500k+ packages) | ❌ None |
| Data science / ML | ✅ NumPy, Pandas, PyTorch, TensorFlow | ❌ Basic tensor.fd |
| Web frameworks | ✅ Django, Flask, FastAPI | ❌ None |
| Scientific computing | ✅ SciPy, Matplotlib, Jupyter | ❌ None |
| Scripting | ✅ Glue code, automation, DevOps | ❌ Not a scripting language |
| Learning curve | ✅ Trivial (readable, beginner-friendly) | ⚠️ Moderate (regions, effects, capabilities) |
| REPL | ✅ Excellent (IPython, Jupyter) | ⚠️ Basic (no history, no multiline) |
| Dynamic typing | ✅ No type annotations needed | ❌ Static typing required |
| Prototyping speed | ✅ Fastest (no compile, dynamic) | ⚠️ Fast compile but static types |
| String processing | ✅ Excellent (regex, f-strings, methods) | ⚠️ Basic (len, find, slice, split) |
| File I/O | ✅ `open()`, pathlib, shutil | ⚠️ Basic (stdlib/file.fd) |
| Networking | ✅ socket, requests, asyncio | ❌ Basic (stdlib/http.fd) |
| Database | ✅ psycopg2, SQLAlchemy, etc. | ❌ None |
| Testing | ✅ pytest (excellent, fixtures, parametrize) | ⚠️ Basic (fidesc test) |
| Documentation | ✅ Sphinx, docstrings, readthedocs | ❌ None |
| Community | ✅ Massive (millions of developers) | ❌ Just starting |
| Corporate backing | ✅ PSF, Google, Microsoft, Meta | ❌ Independent |
| Education | ✅ Taught in every university | ❌ None |
| AI/ML libraries | ✅ Everything (HuggingFace, OpenAI, etc.) | ❌ Basic tensor.fd |
| Notebooks | ✅ Jupyter (interactive data exploration) | ❌ None |
| Package management | ✅ pip + venv + poetry | ❌ None |
| Cross-platform | ✅ Runs everywhere (interpreted) | ⚠️ Linux-focused |

### Where Fides Beats Python

| Feature | Fides | Python |
|---------|-------|--------|
| Performance | ✅ Compiled, native speed | ❌ Interpreted, 10-100x slower |
| Memory usage | ✅ Minimal (no runtime) | ❌ High (interpreter + objects) |
| Startup time | ✅ Instant (native binary) | ❌ Slow (interpreter startup) |
| Memory safety | ✅ Compile-time borrow and escape checks, no GC (leaks what it cannot prove finished) | ⚠️ GC (no use-after-free, but GC pauses) |
| Type safety | ✅ Compile-time static typing | ❌ Runtime type errors (mypy is optional) |
| Concurrency | ✅ Threads + channels + select | ⚠️ GIL limits threading (asyncio helps) |
| Bare metal | ✅ `--bare` mode | ❌ Requires interpreter |
| Embedded | ✅ Yes (no runtime, small binary) | ❌ Needs Python runtime |
| Real-time | ✅ No GC, deterministic | ❌ GC pauses, GIL |
| Binary distribution | ✅ Single executable | ❌ Needs Python installed |
| Capability security | ✅ Compile-time | ❌ None |
| Effect system | ✅ `!IO`, `!Net` tracking | ❌ None |
| Deterministic execution | ✅ `@deterministic` | ❌ GC non-determinism |
| Memory budget | ✅ Compile-time | ❌ GC can't bound memory |
| Hot reload | ✅ Native JIT + persist | ⚠️ `importlib.reload()` (no state persist) |

**Bottom line:** Python wins on everything that matters for most developers:
ecosystem (500k+ packages), data science (NumPy/PyTorch/Pandas), web
frameworks (Django/FastAPI), community (millions), and ease of use (dynamic
typing, trivial learning curve). Fides wins on performance (native vs
interpreted), safety (compile-time vs runtime), and resource constraints
(no runtime, no GC, bare metal). For data science, ML, web backends, or
scripting, Python is the obvious choice. For embedded, safety-critical, or
performance-sensitive systems, Fides is better — but you'll be building
infrastructure from scratch that Python gives you for free.

---

# Fides Language — Industry Conversion Guide

> How projects from different sectors can migrate to Fides, what they gain,
> what they lose, and whether it's worth it.
>
> The core thesis: **Fides's safety systems (capabilities, effects, regions) are
> domain-agnostic. Learning Fides once unlocks every domain annotation. The
> switching cost is one language. The benefit is every industry.**

---

## The Shared Foundation

Before looking at individual industries, understand why Fides can serve all of
them. Every domain annotation is a *lens* on the same three core systems:

```
┌─────────────────────────────────────────────────────────────┐
│                    Fides CORE SAFETY SYSTEMS                      │
│                                                              │
│  Capabilities    Effects           Regions                   │
│  (authority)     (side effects)    (memory lifetime)         │
│      │               │                  │                    │
│      ▼               ▼                  ▼                    │
│  sandbox { }     !IO !Alloc        region r { }              │
│  @serverless     @deterministic    @firmware                 │
│  @gas            @realtime         @pool                     │
│  @audit          @crashfree        memory budgets             │
│                  @no_grad          @lowpower                 │
│                                                              │
│  Comptime (compile-time codegen + reflection)                │
│      │                                                       │
│      ▼                                                       │
│  @shader  @simd  @differentiable  @interrupt                 │
└─────────────────────────────────────────────────────────────┘
```

**This is why the scope isn't explosion — it's reuse.** Building
`@deterministic` for medical makes `@gas` for blockchain cheaper because
both use the effect system. Building `@pool` for gaming makes `@firmware`
for IoT cheaper because both use the region system.

---

## 1. Gaming — Switching from C++ / C# / Unity / Unreal

### The Pain Today

| Problem | Current "Solution" | Why It Hurts |
|---------|-------------------|--------------|
| Crashes in game loop | Try/catch, hope for the best | One panic = lost player |
| Shader strings | GLSL/HLSL as string literals | No type checking, no IDE support, runtime crashes |
| Memory allocation in hot loop | Custom allocators, manual pools | Boilerplate, error-prone, GC stalls (C#) |
| ECS frameworks | Unity DOTS, Entitas, custom | Each is different, none are native, all are libraries |
| State machine bugs | Enum + switch, no compile-time check | Invalid transitions crash at runtime |
| SIMD optimization | Intrinsics, manual vectorization | Platform-specific, unreadable, unmaintainable |

### What Fides Gives Them

| Fides Feature | Benefit | Impossible In |
|-----------|---------|---------------|
| `@crashfree` | Compiler proves a function cannot panic — zero crashes in game loop | C++, C#, Rust (no proof) |
| `@shader` | Write GPU code in Fides, compiler emits SPIR-V — same language, same types | C++ (needs HLSL strings), C# (needs shader strings) |
| `@simd` | Annotate a loop, compiler auto-vectorizes to SSE/AVX/NEON | C++ (manual intrinsics), C# (limited) |
| `@pool` | Zero-allocation object pools, compiler manages pool memory | C++ (manual), C# (GC pressure) |
| `vec2/3/4` + `mat4` + swizzle | First-class math types with operator overloading + SIMD codegen | C# (no SIMD native), Python (no native) |
| ECS (`component`/`system`/`query`) | Native ECS with SoA memory layout, compiler-controlled | C++ (library only), C# (library only) |
| `state` machines | Compile-time verified state transitions — invalid transition = compile error | C++, C#, Rust (all use enum+switch) |
| `on` event handlers | Unified input/collision/tick/lifecycle events | C++ (each framework different), C# (Unity events are magic) |

### What They Lose

| Loss | Severity | Mitigation |
|------|----------|------------|
| Unity/Unreal editor | 🔴 Critical for AAA | Fides targets indie/2D/RTS first. Editor can be built later in Fides |
| Physics engine (PhysX, Havok) | 🔴 Critical | `stdlib/physics.fd` planned. Or FFI to existing engines |
| Asset pipeline | 🟡 Significant | Build tooling in Phase 11. Fides's comptime can process assets |
| Audio engine (Wwise, FMOD) | 🟡 Significant | `stdlib/audio.fd` via FFI |
| Community size | 🔴 Critical | This is the real barrier. Solved by shipping killer games |
| Existing codebase | 🔴 Varies | FFI allows calling C++ libraries. Migration is incremental |

### Migration Path

```
Step 1: Write game logic in Fides, call existing C++ engine via FFI
Step 2: Replace subsystems one at a time (physics, audio, rendering)
Step 3: Use @shader to move GPU code into Fides
Step 4: Use @crashfree to prove the game loop is panic-free
Step 5: Full Fides codebase, zero C++ dependency
```

### Verdict

🟢 **Worth it for indie/2D/RTS/web games.** The `@crashfree` guarantee alone
is a game-changer (pun intended). No other language can prove your game loop
won't crash. For AAA studios locked into Unreal/Unity, the migration cost is
too high — but they're not the target.

**Killer demo:** A 2D game in Fides with `@crashfree` game loop, `@shader` for
GPU effects, and ECS for entities. Under 5000 lines. Zero crashes. Prove it
runs at 60fps. That gets attention.

---

## 2. Blockchain — Switching from Solidity / Rust / Move / Vyper

### The Pain Today

| Problem | Current "Solution" | Why It Hurts |
|---------|-------------------|--------------|
| Gas limit exceeded at runtime | Runtime check, transaction reverts | User pays gas, tx fails, bad UX |
| Non-deterministic execution | Manual auditing | One non-deterministic op = chain fork |
| Reentrancy attacks | Checks-effects-interactions pattern, reentrancy guards | Easy to forget, $1.7B stolen to date |
| Unsafe math (overflow) | Solidity 0.8+ checked arithmetic, but still runtime | Runtime checks cost gas |
| No capability isolation | Anyone can call any public function | Hacks exploit unexpected call paths |
| Auditor bottleneck | Manual review by firms (Trail of Bits, OpenZeppelin) | $50K+ per audit, weeks of delay |

### What Fides Gives Them

| Fides Feature | Benefit | Impossible In |
|-----------|---------|---------------|
| `@gas` | Compiler counts IR ops, verifies gas < limit at compile time | Solidity (runtime only), Rust (no gas concept) |
| `@deterministic` | Compiler rejects FP, random, IO, time, mutable globals — provably deterministic | Solidity (runtime check), Rust (no enforcement) |
| `sandbox { }` | Capability revocation — untrusted contract code can't call IO/FS/Net | Solidity (no isolation), Rust (no compile-time caps) |
| `@emit` | Event emission with signature verification, compiler tracks all emitted events | Solidity (manual emit), Rust (no native events) |
| Effect system | Know at compile time what a function does (reads? writes? allocates?) | Solidity (no effects), Rust (no effect tracking) |
| Comptime | Compile-time code generation for serializers, ABI encoders | Solidity (limited), Rust (macros are complex) |
| Native performance | Fides compiles to native code — fast execution for L2/off-chain | Solidity (EVM interpreted), Rust (yes but complex) |

### What They Lose

| Loss | Severity | Mitigation |
|------|----------|------------|
| EVM compatibility | 🔴 Critical for L1s | Fides targets new L2s/sidechains/app-chains, not EVM |
| Solidity ecosystem (OpenZeppelin, etc.) | 🔴 Critical | Rebuild core libraries in Fides stdlib. Comptime helps |
| Existing DeFi composability | 🔴 Critical | New chain = new composability. This is a fresh start |
| Auditing firm familiarity | 🟡 Significant | Auditors will learn Fides if chains adopt it. `@deterministic` makes auditing easier |
| Wallet/tooling support | 🟡 Significant | Build tooling in Phase 11. Fides's comptime can generate ABIs |

### Migration Path

```
Step 1: Build a Fides VM (interpreter or JIT) as the chain's execution layer
Step 2: Write core contracts (token, vault, governance) in Fides with @gas + @deterministic
Step 3: Use sandbox { } to isolate untrusted contract calls
Step 4: Use @emit for event logging with signature verification
Step 5: Deploy chain with Fides as native contract language
```

### Verdict

🟢 **Worth it for new L1/L2 chains and app-chains.** Compile-time gas
verification is a genuine improvement over Solidity's runtime check.
`@deterministic` eliminates an entire class of consensus bugs. The barrier
is ecosystem (no OpenZeppelin), but new chains start fresh anyway.

**Killer demo:** A simple DEX contract in Fides with `@gas` (compile-time gas
proof), `@deterministic` (provably no IO/random), and `sandbox { }` (isolate
user-provided callbacks). Show the compiler rejecting a contract that
exceeds gas. That's something Solidity cannot do.

---

## 3. Robotics & IoT — Switching from C / C++ / Rust / MicroPython

### The Pain Today

| Problem | Current "Solution" | Why It Hurts |
|---------|-------------------|--------------|
| Missed deadlines (WCET) | Manual timing analysis, oscilloscope | One missed deadline = crashed robot |
| Memory leaks on long runs | Manual free, valgrind, hope | Robot runs for months, leak = crash |
| Heap allocation in control loop | Ban malloc (convention), code review | One malloc = unpredictable latency |
| Interrupt handler bugs | Manual ISR code, register save/restore | One wrong register = corrupted state |
| Watchdog timer management | Manual kicks, hope | Forgot to kick = robot resets mid-operation |
| Battery life | Manual sleep, clock gating | No compiler help for power optimization |
| Safety certification | Manual code review, DO-178C / IEC 61508 | Expensive, slow, error-prone |

### What Fides Gives Them

| Fides Feature | Benefit | Impossible In |
|-----------|---------|---------------|
| `@realtime` | Compiler analyzes WCET, rejects malloc, rejects unbounded loops | C (manual), Rust (no WCET), Zig (no WCET) |
| `@interrupt` | Compiler generates ISR code, vector table, register save/restore, RETI | C (manual asm), Rust (manual) |
| `@firmware` | Compiler strips OS deps, bare metal startup, rejects heap | C (manual linker scripts), Rust (no_std but manual) |
| `@watchdog` (not implemented) | Planned: watchdog kicks at safe points (loop boundaries) | C (manual), Rust (manual) |
| `@lowpower` (not implemented) | Planned: WFI/WFE sleep, clock gating | C (manual), Rust (no support) |
| Region system | Memory safety without GC, without borrow checker complexity | Rust (borrow checker is hard), C (unsafe) |
| `@deterministic` | Provably no FP/random/IO in control loop | C (manual review), Rust (no enforcement) |
| `@pool` (not implemented) | Planned: zero-allocation object pools for sensor data | C (manual), Rust (arena libraries) |

### What They Lose

| Loss | Severity | Mitigation |
|------|----------|------------|
| ROS/ROS2 ecosystem | 🔴 Critical for research robots | FFI to ROS. Or build Fides-native robotics stdlib |
| Vendor SDKs (STM32, NXP, ESP32) | 🔴 Critical | FFI to vendor C headers. `parse-header` already works |
| MicroPython ease of use | 🟡 For prototyping | Fides is more verbose but safer. Use MicroPython for prototypes, Fides for production |
| Existing C codebase | 🟡 Varies | FFI allows calling C. Migration is incremental |
| Arduino ecosystem | 🟡 For hobbyists | Fides targets professional robotics, not Arduino |

### Migration Path

```
Step 1: Write control loop in Fides with @realtime (compiler rejects malloc)
Step 2: Use @interrupt for ISR code (compiler generates vector table)
Step 3: Use @firmware for bare metal startup (no OS dependency)
Step 4: Use @watchdog for automatic watchdog kicks
Step 5: Call vendor SDK via FFI for hardware-specific operations
Step 6: Full Fides firmware, zero C dependency
```

### Verdict

🟢 **Worth it for professional robotics and IoT firmware.** `@realtime`
rejecting malloc at compile time is a genuine safety improvement. The region
system is simpler than Rust's borrow checker for embedded developers who
are used to C. The barrier is vendor SDKs, but FFI solves that.

**Killer demo:** A motor control loop in Fides with `@realtime` (prove no
malloc, bounded loop), `@interrupt` (auto-generated ISR for encoder input),
and `@watchdog` (auto-kick). Show the compiler rejecting a version that
accidentally calls malloc. That's something C and Rust cannot do.

---

## 4. Medical Devices — Switching from C++ / Ada / SPARK / Java

### The Pain Today

| Problem | Current "Solution" | Why It Hurts |
|---------|-------------------|--------------|
| Non-deterministic computation | Manual code review, ban FP | One FP rounding error = wrong dosage |
| Audit trails | Manual logging, hope devs remember | FDA requires audit trail, missing log = non-compliance |
| Untrusted code access | Process isolation, OS-level sandboxing | Heavy, slow, not always available on medical devices |
| Safety certification (IEC 62304, FDA 510(k)) | Expensive external audits | $1M+ per device, 12-18 months |
| Side effects in pure functions | Manual review, code conventions | One hidden side effect = wrong result |

### What Fides Gives Them

| Fides Feature | Benefit | Impossible In |
|-----------|---------|---------------|
| `@deterministic` | Compiler rejects FP, random, IO, time, mutable globals — provably deterministic | C++ (manual), Java (no enforcement), Ada (limited) |
| `@audit` | Compiler auto-instruments every function entry/exit — FDA-compliant audit trail for free | C++ (manual logging), Java (AOP but runtime) |
| `sandbox { }` | Compile-time capability revocation — untrusted code can't access patient data | C++ (OS-level only), Java (SecurityManager deprecated) |
| Effect system | Know at compile time that dosage calculation is pure (!{}) | C++ (no effects), Java (no effects), Ada (limited) |
| Region system | Memory safety without GC — critical for medical device reliability | Java (GC pauses), C++ (unsafe), Rust (borrow checker complexity) |
| `@crashfree` | Prove the device controller cannot panic | C++ (no proof), Java (exceptions everywhere) |

### What They Lose

| Loss | Severity | Mitigation |
|------|----------|------------|
| Ada/SPARK formal verification | 🔴 For safety-critical | Fides's `@deterministic` + `@crashfree` cover most needs. Full formal verification in Phase 12 |
| Existing certified codebase | 🔴 Critical | Recertification required. But Fides's compile-time guarantees may make certification *easier* |
| Java ecosystem (for hospital IT) | 🟡 For non-device code | Fides targets device firmware, not hospital backend |
| Auditing firm familiarity | 🟡 Significant | Auditors will learn Fides if medical companies adopt it. `@audit` makes their job easier |

### Migration Path

```
Step 1: Write dosage calculation in Fides with @deterministic (no FP, no IO)
Step 2: Use @audit for automatic FDA-compliant audit trail
Step 3: Use sandbox { } to isolate patient data access
Step 4: Use @crashfree to prove the device controller cannot panic
Step 5: Use effect system to prove pure functions are actually pure
Step 6: (future) Submit for FDA certification — not possible today
```

### Verdict

🔴 **Not usable for medical devices today.** Fides is not suitable for
medical, automotive or avionics use: the compiler is not qualified, `fid
qualify` produces evidence marked `NOT_QUALIFIED`, and the full test suite
has not been rerun on the current compiler. `@deterministic`, `@audit` and
`@crashfree` are checked rules that a future qualification effort could build
on.

**Killer demo:** A drug dosage calculator in Fides with `@deterministic` (no FP,
integer-only math), `@audit` (auto-logged every call), and `@crashfree`
(proven no panic). Show the compiler rejecting a version that accidentally
uses floating point. That's something C++ and Java cannot do.

---

## 5. Serverless & Edge — Switching from Go / TypeScript / Python / Rust

### The Pain Today

| Problem | Current "Solution" | Why It Hurts |
|---------|-------------------|--------------|
| Cold start latency | Provisioned concurrency, keep-alive | 100ms+ cold starts = bad UX |
| Binary size | Docker images, layers | 100MB+ images for a simple function |
| Unnecessary code in binary | Tree-shaking (JS), manual trimming | Ship entire runtime for one function |
| Security isolation | Containers, V8 isolates | Heavy, slow, attack surface |
| Paying for unused features | Runtime includes everything | Pay for GC, reflection, unused stdlib |
| WASM complexity | Manual WASM compilation, ABI issues | Hard to target, limited language support |

### What Fides Gives Them

| Fides Feature | Benefit | Impossible In |
|-----------|---------|---------------|
| `@serverless` | Compiler tree-shakes, pre-initializes, outputs WASM | Go (manual), TS (bundler-dependent), Python (impossible) |
| Capability-aware linking | `--caps IO` strips all non-IO functions from binary | Go (no caps), TS (no caps), Rust (no caps) |
| Tiny binaries | No runtime, no GC, no VM — KB not MB | Go (MB), Java (100MB+), Python (container) |
| `sandbox { }` | Compile-time capability isolation per function | Go (OS-level), TS (none), Python (none) |
| Native + WASM | Same code compiles to native (edge) or WASM (browser) | Go (WASM but large), Python (no native) |
| Effect system | Know exactly what a function does — bill for actual work | Go (no effects), TS (no effects) |
| Hot reload | `fidesc watch` — instant function-level reload | Go (rebuild), TS (HMR but complex) |

### What They Lose

| Loss | Severity | Mitigation |
|------|----------|------------|
| AWS Lambda/GCP Cloud Run native support | 🔴 Critical | Fides compiles to WASM — works with any WASM-compatible platform |
| npm/pip/crates ecosystem | � Significant | Fides stdlib + ark package manager (Phase 13.4). Start with core libraries |
| Node.js event loop | 🟡 For I/O-heavy code | Fides's effect system + native threads. Different model but capable |
| Existing DevOps tooling | 🟡 Significant | Fides's build system (Phase 11) handles compilation. CI/CD via CLI |

### Migration Path

```
Step 1: Write serverless function in Fides with @serverless (tree-shake + WASM output)
Step 2: Use capability-aware linking: --caps "IO,Net" strips everything else
Step 3: Deploy as WASM module to Cloudflare Workers / Fastly / AWS Lambda (WASM)
Step 4: Use sandbox { } to isolate untrusted request handlers
Step 5: Use effect system to verify functions only do what they declare
```

### Verdict

🟢 **Worth it for edge computing and WASM-based serverless.** Fides's tiny
binaries (KB not MB) and capability-aware linking are genuine advantages
for edge deployment where size and security matter. The barrier is platform
support, but WASM compatibility means Fides works anywhere WASM works.

**Killer demo:** A serverless API in Fides that compiles to a 50KB WASM module
with `@serverless` (tree-shaken), `--caps "IO,Net"` (only IO+Net functions
included), and `sandbox { }` for request isolation. Show it cold-starting
in <1ms. That's something Go and Node.js cannot match.

---

## 6. AI/ML — Switching from Python / C++ / CUDA / Mojo

### The Pain Today

| Problem | Current "Solution" | Why It Hurts |
|---------|-------------------|--------------|
| Two-language problem | Python for research, C++/CUDA for production | Rewrite everything, bugs in translation |
| CUDA strings | Write GPU kernels as string literals | No type checking, no IDE, runtime crashes |
| Shape errors | Runtime dimension mismatch | Crash in production, not at compile time |
| Gradient bugs | Manual autograd, hope it's right | Silent wrong gradients = wrong model |
| Deployment bloat | Ship Python runtime + PyTorch + CUDA | GB-sized containers for a small model |
| Inference latency | Python GIL, interpreter overhead | Slow inference, need ONNX/TensorRT workaround |

### What Fides Gives Them

| Fides Feature | Benefit | Impossible In |
|-----------|---------|---------------|
| `@differentiable` | ⚠️ Typechecker proves differentiability (no IO/Net/Spawn). Autograd transform NOT implemented — no backward pass generated. See Issue 7. | Python (PyTorch runtime), C++ (manual), Mojo (yes but new) |
| `@no_grad` | Disable gradient tracking for inference — zero overhead | Python (torch.no_grad() but runtime), C++ (manual) |
| Shape types | Compile-time dimension checking — shape mismatch = compile error | Python (runtime), C++ (no), Mojo (partial) |
| `@shader` / GPU | Write GPU kernels in Fides, compiler emits SPIR-V | Python (CUDA strings), C++ (CUDA strings) |
| `@simd` | Auto-vectorize inference loops to SSE/AVX/NEON | Python (NumPy runtime), C++ (manual intrinsics) |
| Native performance | No interpreter, no GIL — fast inference | Python (GIL + interpreter), Java (GC) |
| Tiny deployment | Compile model + inference to native binary (KB) | Python (GB container), PyTorch (GB) |
| Layer/model syntax | `layer Dense { ... }` — neural network syntax is native | Python (class boilerplate), C++ (class boilerplate) |
| Quantization | `w ~> q8` — native quantization syntax | Python (manual), C++ (manual) |

### What They Lose

| Loss | Severity | Mitigation |
|------|----------|------------|
| PyTorch/TensorFlow ecosystem | 🔴 Critical for training | Fides targets *inference deployment*, not training. Use PyTorch for training, Fides for deployment |
| Jupyter notebooks | 🔴 Critical for research | Fides REPL exists. Jupyter kernel possible in Phase 11 |
| Python ML libraries (sklearn, pandas, etc.) | 🔴 Critical | Rebuild core libraries in Fides. Start with tensor + autograd |
| Hugging Face model hub | 🟡 Significant | Fides can load exported models (ONNX format). FFI to existing runtimes |
| CUDA ecosystem | 🟡 For training | Fides's SPIR-V backend targets GPU. For training, PyTorch is still better |
| Mojo (direct competitor) | 🟡 Mojo has Python interop | Fides differentiates with safety systems (effects, capabilities, regions) that Mojo lacks |

### Migration Path

```
Step 1: Train model in PyTorch (keep existing workflow)
Step 2: Export model to Fides using comptime (generate Fides code from model graph)
Step 3: Write inference loop in Fides with @no_grad (zero tracing overhead)
Step 4: Use @simd for auto-vectorized inference
Step 5: Use @shader for GPU inference (SPIR-V output)
Step 6: Deploy as tiny native binary (KB not GB)
```

### Verdict

🟡 **Worth it for inference deployment and edge AI, not for training.**
Python's training ecosystem (PyTorch, Jupyter, Hugging Face) is too entrenched
to displace. But *deploying* models is a different story — Fides's tiny binaries,
native performance, `@no_grad`, `@simd`, and `@shader` make it compelling for
production inference. Mojo is the direct competitor, but Fides differentiates with
safety systems Mojo doesn't have.

**Killer demo:** A quantized transformer inference engine in Fides with `@no_grad`
(no tracing), `@simd` (auto-vectorized matmul), shape types (compile-time
dimension checking), and `~> q8` quantization. Deploy as a 100KB binary.
Show it running faster than PyTorch with 1000x smaller deployment. That's
the inference deployment pitch.

---

## 7. Systems Programming — Switching from C / Rust / Zig / Go

### The Pain Today

| Problem | Current "Solution" | Why It Hurts |
|---------|-------------------|--------------|
| Memory safety without GC | Rust (borrow checker) or C (unsafe) | Rust: learning curve. C: segfaults |
| Borrow checker complexity | Fight it, use Rc/RefCell, give up | #1 Rust complaint, 27.9% call it a "big problem" |
| No effect tracking | Manual reasoning, code review | Hidden side effects = bugs |
| No capability isolation | OS-level sandboxing, containers | Heavy, not compile-time |
| Slow compile times (Rust) | sccache, incremental, pray | 45% of Rust quitters cite compile times |
| C's undefined behavior | Sanitizers, valgrind, hope | One UAF = security vulnerability |
| Go's GC | Tune GC, accept pauses | Not suitable for realtime/low-latency |
| Zig's lack of safety | Manual discipline | No compile-time safety guarantees |

### What Fides Gives Them

| Fides Feature | Benefit | Impossible In |
|-----------|---------|---------------|
| Region-based safety | Memory safety without borrow checker complexity | Rust (borrow checker), C (unsafe), Zig (no guarantee) |
| Effect system | Know at compile time what code does (IO? Alloc? Pure?) | C (no), Rust (no), Zig (no), Go (no) |
| Capability system | Compile-time authority revocation — `sandbox { }` | C (no), Rust (no), Zig (no), Go (no) |
| Comptime | Compile-time codegen, reflection, no macros | C (macros), Rust (complex macros), Go (no generics until 1.18) |
| Native codegen | Zero external tools — PE, ELF64, Mach-O, COFF, WASM, SPIR-V | Rust (LLVM), Go (own but limited targets), Zig (LLVM) |
| No GC | Region-based memory management | Go (GC), Java (GC), Python (GC) |
| Memory budgets | Compile-time allocation limit | C (no), Rust (no), Zig (no), Go (no) |
| Fast compilation | No LLVM dependency, direct codegen | Rust (LLVM, slow), C++ (slow), Swift (slow) |

### What They Lose

| Loss | Severity | Mitigation |
|------|----------|------------|
| Rust crate ecosystem | 🔴 Critical | Build core libraries in Fides. FFI for existing C/Rust libraries |
| C ABI compatibility | 🟡 Fides has FFI | FFI works. Fides can call C libraries |
| Zig's C interop story | 🟡 Zig is better at this | Fides's FFI is functional. Improve over time |
| Go's goroutines | 🟡 For concurrent servers | Fides's `parallel { | }` fork-join (Phase 7). Effect system for concurrency safety |
| Rust's borrow checker guarantees | 🟡 Different safety model | Fides's region system provides different but comparable safety. Formal proof in Phase 12 |
| Maturity | 🔴 Critical | This is the real barrier. Solved by time + users + real projects |

### Migration Path

```
Step 1: Write new module in Fides, call existing C/Rust via FFI
Step 2: Use region system for memory safety (simpler than borrow checker)
Step 3: Use effect system to track side effects across the codebase
Step 4: Use sandbox { } to isolate untrusted/plugin code
Step 5: Use comptime for code generation (replace macros/templates)
Step 6: Gradually migrate modules, one at a time
```

### Verdict

🟢 **This is Fides's strongest sector.** The combination of region-based safety
(simpler than Rust), effect tracking (no one has this), capability revocation
(no one has this), and comptime (like Zig) is genuinely unique. The barrier
is ecosystem maturity, not technology.

**Killer demo:** A web server in Fides with sandbox blocks for plugin isolation,
effect-tracked request handlers (prove a handler is pure), region-based
memory management (no GC, no borrow checker), and comptime-generated
serialization. Show it handling 100K requests/sec with zero crashes. That's
the systems programming pitch.

---

## Cross-Industry Synergy: The Real Moat

The killer insight: **switching to Fides for one industry makes switching to
Fides for another industry free.**

```
Gaming company adopts Fides for @crashfree
    → Already has the effect system
    → @deterministic for medical is just another annotation on the same system
    → @gas for blockchain is just another annotation on the same system
    → @realtime for robotics is just another annotation on the same system

Robotics company adopts Fides for @realtime
    → Already has the region system
    → @pool for gaming is just another annotation on the same system
    → @firmware for IoT is just another annotation on the same system
    → memory budgets for safety are just another annotation on the same system
```

**No other language offers this.** If you learn Rust for systems programming,
you can't use it for blockchain gas verification (no effect system). If you
learn Solidity for blockchain, you can't use it for robotics (no realtime
guarantees). If you learn C++ for gaming, you can't use it for medical
devices (no deterministic enforcement).

Fides is the only language where **the safety systems are the product, and the
domain annotations are just configurations of those systems.**

---

## Summary: Migration Difficulty by Industry

| Industry | From | Migration Difficulty | Fides's Value | Verdict |
|----------|------|---------------------|-----------|---------|
| **Systems** | C/Rust/Zig | 🟡 Medium | 🟢 High — unique safety combo | **Primary target** |
| **Robotics/IoT** | C/C++ | 🟡 Medium | 🟢 High — @realtime + @interrupt + @firmware | **Strong target** |
| **Blockchain** | Solidity/Rust | 🟡 Medium | 🟢 High — @gas + @deterministic + sandbox | **Strong target** |
| **Medical** | C++/Ada | 🔴 Hard (regulatory) | 🟢 High — @deterministic + @audit + @crashfree | **Strong target, slow adoption** |
| **Gaming** | C++/C# | 🟡 Medium | 🟢 High — @crashfree + @shader + ECS + @pool | **Good target (indie first)** |
| **Serverless/Edge** | Go/TS | 🟡 Medium | 🟢 High — tiny binaries + WASM + capability linking | **Good target** |
| **AI/ML** | Python/C++ | 🔴 Hard (ecosystem) | 🟡 Medium — inference only, not training | **Niche target (inference)** |

## The Pitch

> **Learn Fides once. Use it for your game server (`@crashfree`), your smart
> contract (`@gas`), your robot firmware (`@realtime`), your medical device
> (`@deterministic`), your edge function (`@serverless`), and your inference
> engine (`@no_grad`). Same language, same safety systems, different
> annotations.**
>
> **No other language does this.**

---

*This document should be updated as domain annotations are implemented and
real migration experiences are gathered.*
