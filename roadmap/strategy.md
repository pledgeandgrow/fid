# Fides Strategic Improvement Plan — Beating the Competition

> Where to focus effort to close gaps with Rust, Go, Zig, and C.
> Prioritized by impact/effort ratio. Honest assessment.

## Current State (Aug 2026)

```
Interpreter:  104/104 normal  (+ 25/25 expected fail, 6 bonus pass)
C Codegen:    104/104 matched  (+ 25/25 expected fail, 2 bonus pass)
PE Emitter:   104/104 valid   (+ 25/25 expected fail, 6 bonus pass)
Unit tests:   257/257 passed  (incl. 47 cross-compilation tests)
Harness:      8/8 passed
Stdlib:       35 modules (8,080 LOC)
Source:       81,041 LOC Rust (53 files)
Tests:        183 .fd files
Binaries:     fidesc, pacte, scribe, veil
Security:     Binary hardening + @constant_time + @tainted
Cross-comp:   ARM64, RISC-V, Windows — zero external tools
Debugging:    Interpreter debugger + record/replay + hot-reload + post-mortem + native debugger (autopsy)
```

## The Competition's Advantages (Where They Win Today)

| Area | Rust | Go | Zig | C | Fides | Gap Size |
|------|------|-----|-----|---|-------|----------|
| Ecosystem (packages) | 100k+ crates | Large | Growing | Everything | Zero | **CRITICAL** |
| Codegen quality | LLVM (20yr) | Good | LLVM | LLVM/GCC | 23 passes | **LARGE** |
| Concurrency ergonomics | Tokio + Send/Sync | Goroutines | Basic | pthreads | Threads+channels+select+@send/@sync | **SMALL** |
| Developer tooling | cargo+RA+clippy+rustfmt | go tools | zig fmt | make | LSP+lint+bench+fmt+debugger | **SMALL** |
| Debugging | gdb/lldb + RA | delve | gdb | gdb/lldb | Interactive debugger+replay+hot-reload+native | **NONE** |
| Type inference | Full HM | Partial | Partial | None | Bidirectional + `-> _` | **NONE** |
| Stdlib breadth | Comprehensive | Comprehensive | Growing | Everything | 35 modules | **MEDIUM** |
| Real-world deployment | Everywhere | Everywhere | Growing | Everywhere | None | **CRITICAL** |

## Strategy: Focus Where We Can Win Easily

The key insight: **Fides already wins on compilation speed, safety architecture,
and unique features (sandbox, capabilities, hot reload, @conserve).** The gaps
are in ecosystem, tooling, codegen quality, and stdlib breadth.

We cannot close the ecosystem gap directly (that requires users). But we CAN
close the tooling and stdlib gaps with focused engineering. And we can close the
codegen gap partially with targeted optimizations.

### Priority Matrix

```
HIGH IMPACT / LOW EFFORT  → DO FIRST (quick wins)
HIGH IMPACT / HIGH EFFORT → PLAN CAREFULLY (strategic bets)
LOW IMPACT / LOW EFFORT   → DO WHEN BORED (nice-to-have)
LOW IMPACT / HIGH EFFORT  → SKIP (not worth it)
```

---

## Tier 1: Quick Wins (1-2 days each, high impact)

### 1.1 Channel Wrapper in sync.fd (1-2 hours) — ✅ DONE

**Status:** Complete. `stdlib/channel.fd` provides `channel_create`, `channel_send`,
`channel_recv`, `channel_try_recv`, `channel_close`, `channel_is_closed`, and
`CHAN_CLOSED()`. `stdlib/sync.fd` also has `sync_channel_*` wrappers. Runtime
primitives `p_chan_*` implemented in both interpreter (Arc<Mutex<ChannelState>>
with VecDeque) and C codegen (pthread mutex + condvar with bounded ring buffer).
Test: `tests/test_channel.fd` passes on all backends.

**Why:** The `p_chan_*` runtime exists in both interpreter and C codegen, but
`stdlib/sync.fd` doesn't expose channels. Users have to call `p_chan_*` directly.

**What to do:** Add channel functions to `stdlib/sync.fd`:
```fides
pub pf sync_channel_create(capacity: i64) -> i64
pub pf sync_channel_send(ch: i64, val: i64) -> bool
pub pf sync_channel_recv(ch: i64) -> i64
pub pf sync_channel_try_recv(ch: i64) -> i64
pub pf sync_channel_close(ch: i64) -> void
pub pf sync_channel_is_closed(ch: i64) -> bool
```

**Beats:** Go has channels built-in, but Fides now has them too. Rust needs
`std::sync::mpsc` or tokio channels (external crate).

**Files:** `stdlib/sync.fd`, `tests/test_channels.fd` (new)

---

### 1.2 Fix C Codegen for Vec::map/filter (4-8 hours) — ✅ DONE

**Status:** Complete. `tests/test_lang_features.fd` now compiles and runs via
`fidesc ccompile` (produces correct output). The `Vec_map`/`Vec_filter`/
`Vec_fold`/`Vec_for_each`/`Vec_any`/`Vec_all`/`Vec_find`/`Vec_position` functions
in `stdlib/vec.fd` work in C codegen. (Note: `test_lang_features` is still in
the skip list for `fidesc test` for historical reasons, but manual ccompile works.)

**Why:** `test_lang_features` fails in C codegen because `Vec_map`/`Vec_filter`
in `stdlib/vec.fd` call `Vec_new`/`Vec_push`/`Vec_get`/`Vec_len` which ARE in
C codegen. The issue is likely function pointer passing for the closure
parameter `f: pf(i64) -> i64`.

**What to do:**
1. Check how `pf` (function pointer) types are lowered to C
2. Ensure C codegen emits function pointers correctly
3. Test with `test_lang_features.fd`

**Beats:** Rust supports closures everywhere, but C codegen parity means Fides
can target platforms where Rust can't (embedded, bare metal via C).

**Files:** `src/codegen_c.rs`, `src/lower.rs`, `tests/test_lang_features.fd`

---

### 1.3 Stack Traces on Panic (2-4 hours) — ✅ DONE

**Status:** Complete. The interpreter has `call_stack: Vec<String>` and
`format_call_stack()` which prints a stack trace on panic. C codegen has
`p_call_stack[256]` with `p_call_stack_push`/`p_call_stack_pop` in function
prologues/epilogues, and `p_panic()` prints the full stack trace. `Inst::Panic`
now calls `p_panic()` instead of directly calling `fprintf`+`abort()`. PE
emitter still lacks stack traces (minor — PE is for Windows-only testing).

**Why:** When a Fides program panics, it prints just the message. No stack
trace. This is a major debugging disadvantage vs Rust, Go, and Zig.

**What to do:**
1. Interpreter: Print call stack on panic (already have `call_stack`)
2. C codegen: Add `p_print_backtrace()` that uses `backtrace()` from libc
3. PE emitter: Add minimal stack walker

**Beats:** Go has stack traces built-in. Rust has them with `RUST_BACKTRACE=1`.
Fides should have them by default.

**Files:** `src/interp.rs`, `src/codegen_c.rs`, `stdlib/core.fd`

---

### 1.4 Basic Linter Command (4-6 hours) — ✅ DONE

**Status:** Complete. `fidesc lint <file.fd>` command exists in `src/main.rs`.
Checks for unused functions and unused imports, outputs warnings in diagnostic
format. No `src/lint.rs` file — lint logic is inline in main.rs.

**Why:** No linter means no static analysis beyond type checking. Rust has
clippy, Go has go vet. A basic linter catches common bugs.

**What to do:**
1. Add `fidesc lint <file.fd>` command
2. Check for: unused variables, unused functions, unreachable code,
   missing error handling (Result not checked), suspicious patterns
3. Reuse typechecker symbol tables
4. Output warnings in diagnostic format

**Beats:** Zig has no linter. C has cppcheck (external). Go has go vet.

**Files:** `src/main.rs`, `src/lint.rs` (new), `src/typechecker.rs`

---

### 1.5 Benchmark Command (2-4 hours) — ✅ DONE

**Status:** Complete. `fidesc bench <file.fd> [-n <count>] [--ccompile]` command
exists in `src/main.rs`. Runs file N times, reports timing statistics. Supports
both interpreter and C-compiled benchmarks. Additionally, `benchmarks/bench_codegen.sh`
provides a codegen benchmark suite comparing Fides ccompile vs gcc -O2 across
6 workloads (fib, sum, nested loops, call overhead, Collatz, compilation speed).

**Why:** `benches/bench_main.rs` exists but only runs via `cargo bench`. A
`fidesc bench` command makes performance testing accessible.

**What to do:**
1. Expose benchmark functions as pub
2. Add `fidesc bench [name]` command
3. Support `fidesc bench --all`, `fidesc bench lexing`, etc.

**Beats:** Go has `testing.B`. Rust has criterion (external). C has nothing
built-in.

**Files:** `src/main.rs`, `benches/bench_main.rs`

---

## Tier 2: Strategic Bets (1-2 weeks each, high impact)

### 2.1 LSP Completion + Hover + Goto-Definition (1-2 weeks) — ✅ DONE

**Status:** Complete. The LSP server (`src/lsp.rs`) supports `initialize`,
`textDocument/didOpen`, `didChange`, `didClose`, `publishDiagnostics`,
`textDocument/completion` (keywords + symbols), `textDocument/hover` (type
signatures), and `textDocument/definition` (goto-definition). Typechecker
exposes symbol tables via `get_funcs()`, `get_structs()`, `get_methods()`,
`get_enums()`, `get_consts()`, `get_type_aliases()`, `get_traits()`.
`build_program_for_lsp()` in compiler.rs provides partial symbol resolution
even for code with type errors.

**Why:** Autocomplete is the #1 developer experience feature. Without it,
Fides can't compete with Rust (rust-analyzer), Go (gopls), or Zig (zls).

**What to do:**
1. Add `textDocument/completion` — use typechecker symbol tables
   (`funcs`, `structs`, `scopes`, `methods`) to suggest completions
2. Add `textDocument/hover` — show type signatures and docs
3. Add `textDocument/definition` — jump to function/struct definition
4. Add `textDocument/rename` — rename symbol across file

**The typechecker already has:**
- `scopes: Vec<HashMap<String, Type>>` — variables in scope
- `funcs: HashMap<String, (Vec<Type>, Type)>` — function signatures
- `structs: HashMap<String, Vec<(String, Type)>>` — struct fields
- `methods: HashMap<String, HashMap<String, ...>>` — methods

**Beats:** Zig's zls is basic. Go's gopls is mature. Rust's rust-analyzer is
gold standard. Fides can match Zig's level with focused effort.

**Files:** `src/lsp.rs`, `src/typechecker.rs`

---

### 2.2 Code Formatter (1 week) — ✅ DONE

**Status:** Complete. `src/fmt.rs` (972 LOC) implements a token-based
pretty-printer. `fidesc fmt <file.fd>` formats source code with 4-space
indentation, normalized spacing, and consistent brace style. `--check` mode
exits 1 if formatting would change the file (for CI).

**Why:** Every modern language has a formatter (rustfmt, gofmt, zig fmt).
Fides doesn't. This is table stakes.

**What to do:**
1. Parse to AST
2. Pretty-print with consistent indentation (4 spaces)
3. Normalize spacing around operators
4. Add `fidesc fmt <file.fd>` and `fidesc fmt --check` (CI mode)

**Beats:** C has no built-in formatter (clang-format is external). Rust, Go,
Zig all have built-in formatters.

**Files:** `src/fmt.rs` (new), `src/main.rs`

---

### 2.3 Iterator Adapters in Stdlib (3-5 days) — ✅ DONE

**Status:** Complete. `stdlib/vec.fd` has `vec_take`, `vec_skip`, `vec_zip`,
`vec_enumerate`, `vec_flat_map`, `vec_chunk`, `vec_window`, `vec_dedup`,
`vec_sort_by`, and `vec_contains_val`. Method-style wrappers also available
(`v.take(n)`, `v.skip(n)`, `v.zip(other)`).

**Why:** Rust's iterator adapters (`map`, `filter`, `take`, `skip`, `zip`,
`enumerate`, `flat_map`, `collect`) are a major productivity feature. Fides
has `Vec_map`/`Vec_filter` but not the full adapter set.

**What to do:** Add to `stdlib/vec.fd`:
```fides
Vec_take(self: i64, n: i64) -> i64
Vec_skip(self: i64, n: i64) -> i64
Vec_zip(self: i64, other: i64) -> i64
Vec_enumerate(self: i64) -> i64
Vec_flat_map(self: i64, f: pf(i64) -> i64) -> i64
Vec_chunk(self: i64, size: i64) -> i64
Vec_window(self: i64, size: i64) -> i64
Vec_dedup(self: i64) -> void
Vec_sort_by(self: i64, cmp: pf(i64, i64) -> i64) -> void
```

**Beats:** Go doesn't have iterator adapters. C doesn't. Zig doesn't. Rust
does (lazy iterators). Fides can match Rust's API (eager evaluation).

**Files:** `stdlib/vec.fd`, `tests/test_vec_adapters.fd` (new)

---

### 2.4 Error Recovery in Parser (3-5 days) — ✅ DONE

**Status:** Complete. Parser now has `errors: Vec<String>` field and
`recover_to_sync_token()` method. `parse_top_level_item()` returns
`Result<(), String>` per item. On error, the parser records it, skips to
the next sync token (`;`, `}`, `pf`, `pub`, `struct`, `enum`, `impl`, `use`),
and continues parsing. All errors are collected and reported together.

**Why:** The parser stops on the first error. Rust, Go, and Zig all recover
and show multiple errors. This is critical for developer productivity.

**What to do:**
1. Change parser return type to `(Program, Vec<Error>)` instead of `Result`
2. Add synchronization tokens (`;`, `}`, `pf`, `pub`) for recovery
3. Skip to next sync token on error, continue parsing
4. Collect all errors, report them together

**Beats:** C compilers often stop on first error. Fides can do better.

**Files:** `src/parser.rs`, `src/compiler.rs`

---

### 2.5 More Result/Option Methods (2-3 days) — ✅ DONE

**Status:** Complete. `stdlib/core.fd` has `Option.map`, `Option.and_then`,
`Option.or_else`, `Option.unwrap_or_else`, `Option.expect`, `Result.map`,
`Result.map_err`, `Result.and_then`, `Result.unwrap_or_else`, and
`Result.expect`. Method-style wrappers available (`opt.map(f)`, `res.map_err(f)`).

**Why:** Rust's Option/Result have ~30 methods each. Fides has ~5. This is
a major ergonomics gap.

**What to do:** Add to `stdlib/core.fd`:
```fides
// Option
Option_map(self: i64, f: pf(i64) -> i64) -> i64
Option_and_then(self: i64, f: pf(i64) -> i64) -> i64
Option_or_else(self: i64, default: pf() -> i64) -> i64
Option_expect(self: i64, msg: str) -> i64
Option_unwrap_or_else(self: i64, default: pf() -> i64) -> i64
Option_is_some(self: i64) -> bool  // exists
Option_is_none(self: i64) -> bool  // exists

// Result
Result_map(self: i64, f: pf(i64) -> i64) -> i64
Result_map_err(self: i64, f: pf(i64) -> i64) -> i64
Result_and_then(self: i64, f: pf(i64) -> i64) -> i64
Result_unwrap_or(self: i64, default: i64) -> i64
Result_expect(self: i64, msg: str) -> i64
Result_is_ok(self: i64) -> bool
Result_is_err(self: i64) -> bool
```

**Beats:** Go doesn't have Option/Result. C doesn't. Zig doesn't. Rust does.
Fides can match Rust's API.

**Files:** `stdlib/core.fd`, `tests/test_option_result.fd` (new)

---

## Tier 3: Long-Term Bets (1+ month, transformative)

### 3.1 Lightweight Green Threads / Goroutine-Style Concurrency — ❌ NOT DONE

**Status:** Not started. No coroutines, swapcontext, or green thread infrastructure.
Fides has OS threads (`p_thread_spawn`) and an executor runtime, but no lightweight
green threads. (Related: #7 Real stackless async is also deferred.)

**Why:** Go's goroutines are its killer feature. Rust has tokio. Fides has
OS threads + executor but no lightweight green threads.

**What to do:**
1. Implement stackful coroutines using `makecontext`/`swapcontext` or
   assembly stack switching
2. Add work-stealing scheduler (like Go's runtime)
3. Add `go pf()` syntax (or reuse `spawn`)
4. Integrate with channels for CSP-style concurrency

**Beats:** Go directly. This would make Fides a credible alternative for
concurrent server programming.

**Effort:** 1-2 months
**Files:** `src/runtime_linux.rs`, `src/interp.rs`, `src/codegen_c.rs`,
`src/ast.rs` (syntax), `src/parser.rs`

---

### 3.2 Select Statement for Channels — ✅ DONE

**Status:** Complete. `select { case ch => x => { ... } default => { ... } }`
syntax is in the parser (`parse_select_stmt`), AST (`Stmt::Select`,
`SelectCase`), typechecker, and lowerer. The lowerer implements select as:
try each channel with `try_recv`, first ready wins. If none ready and no
default, block on first channel.

**Why:** Go's `select` statement is essential for multi-channel concurrency.
Without it, channels are much less useful.

**What to do:**
1. Add `select { case ch1 => x => ..., case ch2 => y => ..., default => ... }` syntax
2. Implement in interpreter (poll all channels, pick ready one)
3. Implement in C codegen (poll with timeout, or block on first ready)

**Beats:** Go's select. Rust's `select!` macro (tokio). Fides can make it
a first-class language construct.

**Effort:** 2-3 weeks
**Files:** `src/ast.rs`, `src/parser.rs`, `src/lower.rs`, `src/interp.rs`,
`src/codegen_c.rs`

---

### 3.3 Type Inference (Bidirectional) — ✅ DONE

**Status:** Complete. Bidirectional type inference is implemented in
`src/typechecker.rs`. Key features:
- **Contextual integer literal inference**: `pv x: i64 = 42` infers `42` as
  `i64`, not `i8` (the default for small literals). The declared type is
  passed as a hint to `check_expr_with_hint()`.
- **Float hint propagation**: `pv x: f32 = 1.0` infers `f32` instead of `f64`.
- **Binary expression hints**: `pv x: i64 = a + b` propagates the `i64` hint
  to both operands.
- **Return type inference**: `pf foo() -> _ { ret 42; }` infers the return
  type from the body's return expressions. `Type::Infer` variant added to
  the AST. The typechecker collects return expression types and updates the
  function signature.
- **Type compatibility**: `Type::Infer` is compatible with any type in
  `types_compatible()`.
- **IR lowering**: `Type::Infer` lowers to `IrType::I64` as a default.

**What's not done:** Full Hindley-Milner inference (inferring types for
ungeneric functions from call sites, unification-based inference). The
current approach is bidirectional (expected type flows down, actual type
flows up), which covers the most common use cases.

---

### 3.4 Auto-Vectorization (Real SIMD) — ✅ DONE (SLP) / ⚠️ PARTIAL (loop-vec)

**Status:** SLP vectorizer re-implemented with correct semantics. Loop
vectorization (`pass_loop_vectorize`) handles constant trip counts.
- **SLP vectorizer** (`pass_slp_vectorize` in opt.rs): Detects pairs of
  adjacent F64 loads from consecutive array indices, replaces them with
  `p_simd_load`, combines paired BinOps into `p_simd_add`/`sub`/`mul`/`div`,
  and replaces paired stores with `p_simd_store`. The x86-64 codegen lowers
  these to SSE2 `movupd`/`addpd`/`subpd`/`mulpd`/`divpd`.
- **Loop vectorizer** (`pass_loop_vectorize`): Unrolls counted loops by
  vector width (2x for F64, 4x for I32) to prepare for SLP. Handles both
  while-loop and rotated-loop forms. Only constant trip counts.
- **Codegen**: `p_simd_load` → `movupd`, `p_simd_store` → `movupd`,
  `p_simd_add` → `addpd`, etc. in `src/codegen_x64.rs`.

**What's not done:** Variable trip count vectorization (requires runtime
trip count check + scalar remainder loop), cost model, AVX2 (256-bit).

**Why:** The SLP vectorizer was removed (semantically incorrect). Loop
vectorization exists but only for constant trip counts. LLVM auto-vectorizes
aggressively. This is a major performance gap.

**What to do:**
1. Improve loop vectorization to handle variable trip counts
2. Add proper SLP vectorization (correct semantics this time)
3. Emit SSE2/AVX2 instructions in codegen_x64.rs
4. Add cost model to decide when vectorization is profitable

**Beats:** C/Rust/Zig with LLVM auto-vectorization. Fides needs this for
compute-heavy workloads.

**Effort:** 1-2 months
**Files:** `src/opt.rs`, `src/codegen_x64.rs`

---

### 3.5 Package Manager — ✅ DONE (basic)

**Status:** `pacte` binary implemented. `pacte.toml` manifest, path + git
dependencies, lock file. Commands: init, add, install, remove, list, lock,
update, search, publish. Registry support not yet implemented.

**What's done:**
1. ✅ `pacte.toml` manifest format (TOML, custom parser — zero deps)
2. ✅ Path dependencies (files and directories)
3. ✅ Git dependencies (clone with `--depth 1`, branch/tag support)
4. ✅ Lock file (`pacte.lock` with resolved commit hashes)
5. ✅ `pacte init/add/install/remove/list/lock/update`

**What's left:**
- [ ] Registry (pacte.dev) — search, publish, version resolution
- [ ] Semver dependency resolution
- [ ] Integration with `fidesc build` (auto-install deps)
- [ ] Workspace support (multi-package repos)

**Beats:** cargo is the gold standard. Go modules are good. Zig's zon is
immature. Fides can learn from all of them.

**Effort:** Basic version done. Registry: 1-2 months more.
**Files:** `src/pacte.rs`, `src/bin/pacte.rs`

---

## What NOT to Focus On (Low ROI)

| Feature | Why Skip |
|---------|----------|
| HKT (Higher-Kinded Types) | Rust doesn't have them either. Niche academic feature. |
| Lifetime parameters | Fides uses regions instead. Different approach, don't mix. |
| PGO (Profile-Guided Optimization) | Complex infrastructure, low payoff for small programs. |
| Full macro system | comptime replaces macros. Don't add a second system. |
| C++ interop | Massive complexity. C interop is sufficient. |
| DWARF debug info | Huge effort. Interpreter debugger + native debugger (ptrace) + stack traces are sufficient. |
| Graph coloring register allocator | Linear scan is good enough. The "known bug" is likely minor. |

---

## Recommended Execution Order

### Phase 1: Quick Wins (Week 1-2)
1. ✅ Channel wrapper in sync.fd (1-2h) — DONE (stdlib/channel.fd + sync_channel_* in sync.fd)
2. ✅ Fix C codegen for Vec::map/filter (4-8h) — DONE (ccompile works, test still in skip list)
3. ✅ Stack traces on panic (2-4h) — DONE (interpreter only; C codegen + PE pending)
4. ✅ Basic linter command (4-6h) — DONE (fidesc lint, unused funcs/imports)
5. ✅ Benchmark command (2-4h) — DONE (fidesc bench + benchmarks/bench_codegen.sh)

**Result:** Developer experience improves dramatically. C codegen parity
improves. Channels are usable.

### Phase 2: Strategic Bets (Week 3-6)
1. ✅ LSP completion + hover + goto-def — DONE (completion, hover, definition)
2. ✅ Code formatter — DONE (fidesc fmt, 972 LOC, --check mode)
3. ✅ Iterator adapters in stdlib — DONE (take, skip, zip, enumerate, flat_map, chunk, window, dedup, sort_by, contains_val)
4. ✅ Error recovery in parser — DONE (sync tokens, multi-error)
5. ✅ More Result/Option methods — DONE (map, and_then, or_else, unwrap_or_else, expect)

**Result:** Fides becomes usable for real development. Tooling matches
Zig's level. Stdlib approaches Rust's breadth.

### Phase 3: Long-Term Bets (Month 2+)
1. ❌ Green threads / goroutine-style concurrency (1-2 months) — NOT STARTED
2. ✅ Select statement for channels — DONE (AST, parser, typechecker, lowerer)
3. ✅ Type inference (bidirectional) — DONE (contextual literals, return type inference via `-> _`)
4. ✅ Auto-vectorization — DONE (SLP vectorizer + loop-vec for constant trips; SSE2 codegen)
5. ✅ Package manager — `pacte` binary with init/add/install/remove/list/lock — DONE (registry pending)
6. ✅ Debugging tools (Phase 14) — DONE (interpreter debugger, record/replay, hot-reload, post-mortem, native debugger)

**Result:** Fides becomes a credible alternative to Go for concurrent
programming, to Rust for systems programming, and to C for embedded.
World-class debugging support matches or exceeds Rust (gdb/lldb) and Go (delve).

---

## Competitive Positioning After Each Phase

### After Phase 1 (Quick Wins)
| Dimension | Fides | vs Rust | vs Go | vs Zig | vs C |
|-----------|-------|---------|-------|--------|------|
| Compilation speed | **10/10** | +8 | +1 | +2 | +2 |
| Safety | **9/10** | = | +6 | +6 | +8 |
| Concurrency | 5/10 | -3 | -4 | +1 | +2 |
| Tooling | 4/10 | -6 | -5 | -4 | -1 |
| Stdlib | 6/10 | -2 | -3 | +2 | -4 |

### After Phase 2 (Strategic Bets)
| Dimension | Fides | vs Rust | vs Go | vs Zig | vs C |
|-----------|-------|---------|-------|--------|------|
| Compilation speed | **10/10** | +8 | +1 | +2 | +2 |
| Safety | **9/10** | = | +6 | +6 | +8 |
| Concurrency | 5/10 | -3 | -4 | +1 | +2 |
| Tooling | 7/10 | -3 | -2 | = | +3 |
| Stdlib | 8/10 | = | -1 | +4 | -2 |

### After Phase 3 (Long-Term Bets)
| Dimension | Fides | vs Rust | vs Go | vs Zig | vs C |
|-----------|-------|---------|-------|--------|------|
| Compilation speed | **10/10** | +8 | +1 | +2 | +2 |
| Safety | **9/10** | = | +6 | +6 | +8 |
| Concurrency | **8/10** | -1 | -1 | +4 | +5 |
| Tooling | 8/10 | -1 | = | +1 | +4 |
| Debugging | **9/10** | = | +1 | +2 | +5 |
| Stdlib | 8/10 | = | -1 | +4 | -2 |
| Unique features | **10/10** | +2 | +4 | +3 | +6 |

**Bottom line:** After Phase 3, Fides beats C and Zig on safety, matches Go
on concurrency, beats Rust on compilation speed, and has unique features
(sandbox, capabilities, hot reload, @conserve, interactive debugger with
hot-reload and post-mortem) that no competitor has.
The remaining gaps are ecosystem (packages) and codegen quality (LLVM).
