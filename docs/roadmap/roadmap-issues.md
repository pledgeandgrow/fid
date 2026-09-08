# Fides Roadmap — Issues & Debates

> A living log of issues encountered, debates on the path taken, and open
> strategic questions that need resolution. Updated as decisions are made
> or new information surfaces.

---

## Critical Blockers (must fix before any production use)

> Added Sep 7, 2026 (Session 18). All four items verified by reading source
> and running tests. These are the highest-priority issues — they undermine
> the language's core value propositions if left unfixed.

### CB1: Self-hosted compiler doesn't enforce its own annotations

**Status:** Open — highest priority

The Rust bootstrap (`src/typechecker.rs`) has real enforcement of
`@medical`, `@conserve`, `@deterministic`, `@crashfree`, `@gas`, `@dp`,
`@shard`, `@constant_time`, `@realtime`, `@tainted`, `@differentiable`,
`@quantized`, `@checkpoint`, `@adapter`. But the self-hosted compiler
(`compiler/typechecker.fd`) has these as **stubs with `// stub (no
enforcement)` comments** — they are permissive and never reject code.

Verified in `compiler/typechecker.fd` lines 1494–1552:
```
// @conserve: energy conservation -- stub (no enforcement)
// @gas(N): gas cost analysis -- stub (no enforcement)
// @constant_time: constant-time execution -- stub (no enforcement)
// @realtime: real-time deadline -- stub (no enforcement)
// @differentiable: autodiff compatibility -- stub (no enforcement)
// @quantized: quantization annotation -- stub (no enforcement)
// @checkpoint: gradient checkpointing -- stub (no enforcement)
// @dp: differential privacy -- stub (no enforcement)
// @shard: tensor parallelism -- stub (no enforcement)
// @adapter: adapter pattern -- stub (no enforcement)
// @tainted: taint tracking -- stub (no enforcement)
```

Only `@deterministic` has partial enforcement (lines 1480–1487 — checks
for `!IO`/`!Alloc` effects and floating-point operations).

**Impact:** The self-hosted compiler is the primary compiler per
`AGENTS.md`. If you use it, the safety annotations that are the
language's main selling point don't actually work. You get false
confidence — code marked `@medical` or `@constant_time` is not checked.

**Fix:** Port the enforcement logic from `src/typechecker.rs` into
`compiler/typechecker.fd` for each stubbed annotation. Each annotation
needs its own verification pass that rejects non-conforming code.

---

### CB2: Result<T,E> and Option<T> hardcoded to i64

**Status:** Open — blocks real error handling

`stdlib/core.fd` defines `Result<T,E>` and `Option<T>` with `i64`
payloads only:

```fides
pub enum Option<T> {
    Some(i64),   // should be Some(T)
    None,
}

pub enum Result<T, E> {
    Ok(i64),     // should be Ok(T)
    Err(i64),    // should be Err(E)
}
```

Verified at `stdlib/core.fd` lines 48–49 and 126–129.

**Impact:** You can't return `Result<str, str>` or `Option<Vec>`
properly. This makes error handling unusable for real programs. A
self-hosted compiler needs `Option<Token>`, `Result<Node, ParseError>`,
etc. everywhere.

**Fix:** Implement proper generic enum instantiation in the
monomorphization pass (`src/mono.rs` / `compiler/mono.fd`). Same
pattern as the `Vec<T>` fix — declare payloads as type params
(`Some(T)` instead of `Some(i64)`), add type-param substitution to
`pattern_binding_ir_types()` in the lowerer.

**Note:** This is also tracked as Issue 1 below (kept there for
historical context).

---

### CB3: Backend test parity is not 100%

**Status:** Open — verified Sep 7, 2026

Current test results from `./target/release/fidesc test`:

| Backend | Pass rate | Detail |
|---------|----------|--------|
| Interpreter | 231/242 (95.5%) | 11 failures |
| C codegen | 214/242 (88.4%) | 28 failures |
| PE emitter | 234/242 (96.7%) | 8 failures |

Failing tests include: `real_crypto`, `real_database`, `real_threads`,
`test_banking`, `test_game_stdlib`, `test_os_features`, `test_parallel`,
`test_sandbox_alloc_fail`, `test_tier1_kernel`, `test_token_full`,
`probe_parser_direct`, `probe_parser_direct2`, `probe_parser_simple`,
`test_codegen_c`, `test_fmt_harness`, `test_lexer_harness`,
`test_parser_harness`, `test_sl_debug`, and others.

**Impact:** For production, you need 100% on at least one backend.
Currently no backend is at 100%.

**Fix:** Triage and fix the remaining failing tests. Run `./verify.sh`
and `./verify-selfhost.sh` to get the current list. Some failures are
shared across all backends (indicating a common root cause in lowering
or typechecking), others are backend-specific.

---

### CB4: No real error recovery in the parser

**Status:** Partially addressed — needs hardening

The Rust bootstrap parser (`src/parser.rs`) **does** have error recovery
with synchronization tokens (`recover_to_sync_token`, line 332). It
collects multiple errors per run and continues parsing.

The self-hosted parser (`compiler/parser.fd`) also has error recovery
(`recover_to_sync_token` at line 313, error collection at lines 442 and
2311). However, it has issues:

1. **Debug prints left in production code** — `print("DBG: parse loop
   done\n")` at line 451, `print("DBG: parse returning Err\n")` at line
   453, `print("DBG: parse returning Ok\n")` at line 456. These must be
   removed.
2. **Recovery may not be as robust** as the Rust version — the
   synchronization token set and recovery strategies need auditing to
   ensure they match.
3. **Statement-level recovery** exists but may not cover all cases
   (e.g., recovery inside expressions, nested blocks).

**Impact:** Parse errors are potentially fatal in edge cases — one
syntax error may stop compilation prematurely. Production compilers
report multiple errors per run.

**Fix:**
1. Remove debug prints from `compiler/parser.fd` (lines 451, 453, 456).
2. Audit `recover_to_sync_token` in `compiler/parser.fd` against
   `src/parser.rs` to ensure parity.
3. Add tests for multi-error recovery (e.g., two syntax errors in
   different functions should both be reported).

---

## Issue 1: Result<T,E> Hardcoded to i64

**Status:** Fix in progress (blocked on syntax migration)

`Result<T,E>` and `Option<T>` are hardcoded to `i64` in `stdlib/core.fd`.
Every match arm binds `v: i64` instead of the actual generic type. This is
the same class of bug that was fixed for `Vec<T>` in the Phase 10 self-host
blockers, but `Option<T>` and `Result<T,E>` were not addressed.

**Impact:** A self-hosted compiler needs `Option<Token>`, `Result<Node,
ParseError>`, etc. everywhere. Without generic payloads, Fides cannot
express real compiler data structures ergonomically.

**Fix plan:** Same pattern as the Vec<T> fix — declare payloads as type
params (`Some(T)` instead of `Some(i64)`), add type-param substitution
to `pattern_binding_ir_types()` in the lowerer.

**Debate:** Should we fix this before or after the syntax migration?
Decision: after — the syntax migration is in progress and touching
`stdlib/core.fd` now would conflict.

---

## Issue 2: Four Concurrency Models (Design Principle Violation)

**Status:** Open debate

Fides currently has **four** concurrency mechanisms:
1. `async`/`await` (Phase 7.6)
2. Effect handlers (Phase 2)
3. OS threads (`p_thread_spawn`)
4. Channels (`p_chan_*`)

This violates **Design Principle #5: "One Way to Do Things"**:
> One concurrency model (effect handlers, not async/await + threads + channels)

The industry consensus (2026) is that async/await creates "colored
functions" — one async call contaminates the entire call chain. Rich
Hickey's "Simple Made Easy" critique applies: async/await is *easy*
(familiar syntax) but not *simple* (structurally untangled).

**Options under consideration:**

- **A) Effect handlers as the sole mechanism.** Remove async/await syntax.
  All suspension is via effect handlers. This is the Koka/Effekt/OCaml 5
  approach. Cleanest design, but unfamiliar to most developers.

- **B) async/await as syntactic sugar over effect handlers.** Keep the
  familiar `async`/`await` syntax, but lower it to effect handlers
  internally. One mechanism, two surfaces. This is what OCaml 5's async
  libraries do. Best of both worlds — but requires careful lowering.

- **C) Keep both, document when to use each.** Pragmatic but violates
  the design principle and creates the "which one do I use?" confusion.

**Current lean:** Option B — async/await as sugar over effects. See the
"Concurrency Decision" section below for the full reasoning.

---

## Issue 3: "AI-Native" Claim Without AI Features

**Status:** Open — proposals exist, none implemented

Design Principle #9 says "AI-Native" but Fides has **no LLM-specific
features**. The effect system, capability system, and comptime *could*
be leveraged for AI agent programming but aren't.

The 2026 landscape shows new languages (Turn, ETAS, Noeta, Nexa, AINL)
built specifically for AI agents with features like:
- Cognitive type safety (LLM output validated against schema before binding)
- Confidence operators (control flow gated on model certainty)
- Capability-based credential isolation (raw credentials never enter
  agent memory)
- Compile-time schema absorption (typed API bindings from external specs)

**Fides's advantage:** Fides's existing architecture (effects +
capabilities + comptime + native codegen) is a better foundation than
any of these new languages have. They're building from scratch; Fides
already has the pieces.

**Proposal:** See "Cognitive Type Safety" in the new features analysis.
This would make Fides the only *systems* language with first-class AI
agent support.

---

## Issue 4: No SBOM / Supply-Chain Artifacts

**Status:** Open — low effort, high impact

Fides has capabilities + `sandbox {}` blocks but **does not emit SBOMs
or supply-chain artifacts**. The compiler knows exactly which functions
hold which capabilities, but this information is not exported.

Capa (2026) emits CycloneDX, SPDX, VEX, and SLSA provenance directly
from capability signatures — and that's their killer feature. Supply
chain security is a top 2026 concern (npm/PyPI/crates.io malware).

**Fix:** `fidesc sbom --format cyclonedx|spdx|vex` — serialize the
capability manifest the typechecker already builds. Low effort because
the data exists; it just needs serialization.

---

## Issue 5: @tainted Exists But No Full Information Flow Control

**Status:** Open — natural extension of existing system

Fides has `@tainted` (injection defense) but **not full Information Flow
Control (IFC)**. IFC tracks *where data can go* after it's read, not just
*who can read it*. This is the security research frontier in 2026
(Filament, Cocoon, Keleusma all target this).

The gap: `@tainted` prevents injection at sinks, but doesn't propagate
labels through the program. A function that reads a secret can still
write it to a log — `@tainted` doesn't catch that.

**Proposal:** `@secret` / `@public` / `@trusted` labels that propagate
through assignments, branches, and function calls. Compile-time
rejection of secret→public flows. This extends the existing `@tainted`
infrastructure — same typechecker pass, richer rules.

No systems language has built-in IFC. This would be a genuine first.

---

## Issue 6: ML9-ML14 Annotations Are Checked, Not Implemented

**Status:** ✅ Partially Resolved — ML3/ML11/ML12/ML13 now have runtime implementations

ML9-ML14 added 6 annotations (`@quantized`, `@checkpoint`, `@dp`, `@shard`,
`@adapter`, `@kv_cache`) with typechecker verification. ML3 (autodiff),
ML11 (checkpoint), ML12 (DP), and ML13 (TP) now have runtime implementations
in the interpreter. ML10 (quantization) already had runtime support. ML14
(@adapter) and ML9 (@kv_cache) remain typechecker-only checks.

- `@checkpoint` — ✅ Runtime checkpointing implemented: `checkpoint_mode` in
  interpreter, `p_checkpoint_begin`/`p_checkpoint_end` toggle activation
  saving/recompute. `AutogradOp::Relu`/`Sigmoid` store inputs (not results)
  and recompute during backward pass.
- `@dp(epsilon=1.0)` — ✅ Runtime epsilon budget tracking implemented:
  `dp_epsilon_budget`/`dp_epsilon_spent` in interpreter.
  `p_dp_budget_reset`/`p_dp_budget_check` enforce privacy budget at runtime.
- `@shard(dim=0)` — ✅ Simulated multi-rank runtime implemented:
  `tp_rank`/`tp_world_size` in interpreter. `p_tp_rank`/`p_tp_world_size`
  return rank info. `p_all_reduce` is identity in single-rank mode.
- `@adapter` — `src/typechecker.rs:1483` says "Only modify adapter weights
  (not base weights) — verified by checking that the function does not call
  base weight mutation ops" — but the implementation only checks effects
  (IO/Net/Spawn/Diverge). There is **no check** that base weights aren't
  modified. The comment is a lie.

**Remaining limitation:** C codegen does not implement the autograd tape
runtime. The interpreter-based implementations are sufficient for model
development and testing.

**Remaining fix plan (for annotations still without runtime):**
4. `@adapter` — needs base-weight immutability check (write to non-adapter
   persist vars inside `@adapter` = error)
5. `@quantized` — needs INT8 GEMM in codegen (currently float matmul)
6. `@kv_cache` — needs runtime cache in native backends (currently interp-only)

**Debate:** Is it acceptable to ship annotations as "compile-time proofs
about properties" without runtime enforcement? This is a valid incremental
strategy (get the type system right first), but it means the language
currently provides compile-time proofs about properties that don't hold at
runtime. The most dangerous case is `@dp` — a user might rely on the
privacy guarantee, but without epsilon tracking and a real PRNG, there is
no privacy.

---

## Issue 7: Autodiff Is a Framework, Not an Implementation

**Status:** ✅ Resolved — tape-based reverse-mode autodiff implemented in interpreter

The autograd engine is now implemented in the interpreter (`src/interp.rs`).
`GradBlock`/`GradEnd`/`Backward` IR instructions trigger the autograd mode.
Tensor builtins (`p_tensor_add`, `p_tensor_mul`, `relu`, `sigmoid`, etc.)
record operations on the `autograd_tape` when `autograd_mode` is enabled.
`backward()` traverses the tape in reverse, computing gradients via local
derivative functions (`add_backward`, `mul_backward`, `relu_backward`, etc.).
`p_tensor_grad`/`p_grad_get` retrieve computed gradients.

**What was done:**
1. Added `GradBlock`, `GradEnd`, `Backward` to AST and IR
2. Integrated autograd state into `Interpreter` (`autograd_tape`, `grad_vars`, `autograd_mode`)
3. Modified tensor builtins to record operations on the tape
4. Implemented backward pass with local derivative functions
5. Updated PIR lowering, C codegen, x86, PE, WASM backends, and verifier
6. Test: `test_autodiff_grad.fd` verifies correct gradient computation

**Remaining limitation:** C codegen does not implement the autograd tape
runtime (test skipped on C codegen). The interpreter-based autograd is
sufficient for model development and testing.

---

## Issue 8: PRNG Is Deterministic — Differential Privacy Is Broken

**Status:** Open — security-critical bug

The noise generation for differential privacy in `src/interp.rs:3479-3489`
uses `sin()` and `cos()` of deterministic indices as the PRNG:

```rust
let seed = (i as f64) * 0.1;
let u1 = 0.0001 + (seed.sin().abs()) * 0.9998;
let u2 = 0.0001 + (seed.cos().abs()) * 0.9998;
let z = (-2.0 * u1.ln()).sqrt() * (2.0 * std::f64::consts::PI * u2).cos();
```

Problems:
- **Not random at all** — `sin(0.1)`, `sin(0.2)`, `sin(0.3)` are
  deterministic. The "noise" is the same every run. This completely defeats
  differential privacy — the whole point of DP noise is that it's
  unpredictable.
- **Correlated samples** — adjacent indices produce correlated values
  because `sin(x)` and `sin(x+0.1)` are close. Box-Muller requires
  independent uniform samples.
- **`dropout` is worse** — `src/interp.rs:3505`:
  `let seed = ((i as f64) * 7.0).sin().abs();` — the dropout pattern is
  deterministic per index. Every forward pass drops the same neurons. This
  is not dropout, it's feature selection.

**Impact:** `@dp(epsilon=1.0)` with this PRNG provides **zero** differential
privacy. An attacker who knows the PRNG can subtract the noise. The Noise
effect proves noise was *added*, but not that it was *random*.

**Fix plan:**
1. Add a real PRNG to the interpreter (xorshift128+ or PCG — both are
   ~20 lines of code, no dependencies)
2. Seed it from `p_time()` or an external entropy source
3. Use the PRNG for `dp_noise`, `gaussian_noise`, and `dropout`
4. In native codegen, call the OS CSPRNG (`/dev/urandom` on Linux,
   `BCryptGenRandom` on Windows)

---

## Issue 9: KV Cache Region Budget Doesn't Track KV Cache Allocations

**Status:** Open — the compile-time OOM prevention doesn't work

The region budget system in `src/typechecker.rs:3573` tracks allocations via
`type_size(&final_ty)` on `Stmt::Let`. But the KV cache is created via
`p_kv_cache_new(num_layers, max_seq_len, ...)` — a runtime builtin call.
The typechecker sees this as a function call returning `i64` (a handle), not
as an allocation of `num_layers * max_seq_len * head_dim * 2` bytes.

**The budget check is bypassed:** You can write:
```
region kv_cache kc with budget(100) {
    val cache = kv_cache_new(100, 1000000, 0, 128);  // 100MB, budget says 100 bytes
}
```
This compiles. The budget check passes. The typechecker thinks `cache` is
an 8-byte `i64`. The actual KV cache allocates 100MB at runtime.

**Impact:** The compile-time OOM prevention — the entire point of ML9 —
doesn't work for the KV cache itself. It only works for local variables
allocated in the region, not for runtime-allocated caches.

**Fix plan:**
1. Add a `@alloc_size(args)` annotation to builtins that tells the
   typechecker how much memory a builtin allocates based on its arguments
2. For `p_kv_cache_new`, declare `@alloc_size(num_layers * max_seq_len *
   head_dim * 2 * 8)` (K and V, f64 each)
3. The typechecker's budget tracker should add this to the region budget
   when the call is inside a region
4. Alternatively, make `kv_cache_new` a first-class language construct
   (not a builtin) so the typechecker can reason about its allocation

---

## Issue 10: Transaction Rollback Doesn't Restore KV Cache State

**Status:** Open — memory leak on txn rollback

`src/interp.rs:1731-1740` — `TxnBegin` snapshots `memory` and
`opaque_values` but **not `kv_caches`**:

```rust
let mem_snapshot = self.memory.clone();
let opaque_snapshot = self.opaque_values.clone();
self.txn_snapshots.push((mem_snapshot, opaque_snapshot));
```

If you create a KV cache inside a `txn { }` block and then panic:
1. The KV cache is added to `self.kv_caches`
2. The panic triggers rollback — `memory` and `opaque_values` are restored
3. `kv_caches` is **not restored** — the cache leaks

**Impact:** Memory leak on transaction rollback. Also, if you retry the
transaction, you get a duplicate cache entry.

**Fix plan:** Add `kv_caches` to the transaction snapshot:
```rust
let kv_snapshot = self.kv_caches.clone();
self.txn_snapshots.push((mem_snapshot, opaque_snapshot, kv_snapshot));
```
And restore it on rollback. This is a 4-line fix.

---

## Issue 11: C Codegen Has No Declarations for New ML Builtins

**Status:** Open — linker errors on C backend

The C codegen (`src/codegen_c.rs`) has **zero** mentions of `kv_cache`,
`quantize`, `dp_noise`, `all_reduce`, or any of the 12 new builtins. They're
registered as externs in the lowerer (`src/lower.rs`), so the IR knows about
them, but the C backend has no C function declarations or stub
implementations.

**Impact:** Any test using the new ML features will fail with a linker error
on the C backend. The tests are in the skip list for `fidesc test`, which
hides this. But a user who tries to compile a quantized inference server
with `fidesc ccompile` will get undefined symbol errors.

**Fix plan:**
1. Add C function declarations for all 12 new builtins in the C codegen's
   preamble
2. For builtins that can't be implemented in C (e.g., `p_kv_cache_new`
   needs a runtime struct), emit stub implementations that call into a
   runtime library
3. Alternatively, emit `extern` declarations and link against a
   `libfides_ml.a` runtime library

---

## Issue 12: Quantization Is Fake — qmatmul Dequantizes

**Status:** Open — no INT8 performance benefit

`src/interp.rs:3464-3473` — the "quantized matmul" just calls regular
`matmul` on the inputs:

```rust
"p_qmatmul" => {
    if let (Some(Value::Tensor(a)), Some(Value::Tensor(b))) = (args.get(0), args.get(1)) {
        match a.borrow().matmul(&b.borrow()) {
```

The quantized values are stored as `f64` in the tensor (`src/tensor.rs:415`:
`clamped as f64`), not as actual `i8` values. So "quantization" is just
clamping + scaling, and "qmatmul" is just float matmul.

**Impact:** No performance benefit from quantization. The whole point of
INT8 is to use hardware INT8 GEMM (VNNI on x86, dotprod on ARM, INT8 matrix
multiply on GPU). This produces correct results but at float speed, defeating
the purpose.

**Fix plan:**
1. Add a `DType::I8` variant to `Tensor` and store quantized data as `Vec<i8>`
2. Implement `qmatmul_i8` using INT8 dot products
3. In x86 codegen, emit `VPDPBUSD` (VNNI) for INT8 GEMM
4. In ARM codegen, emit `SDOT`/`UDOT` for INT8 GEMM
5. In GPU codegen, emit `OpIntegerMatMul` (SPIR-V) or `mma.m8n8k4.s8`
   (PTX)

---

## Issue 13: PIR Lowering Drops KV Cache Metadata

**Status:** Open — native backends can't distinguish KV cache regions

`src/pir.rs:868-869`:

```rust
Inst::RegionBegin { name, .. } => L1Inst::NewRegion { name: name.clone() },
Inst::RegionEnd { .. } => L1Inst::FreeRegion { region: ValueId(0) },
```

The `..` pattern discards `is_gpu`, `is_kv_cache`, and `window_size`. PIR
(the portable IR used for native codegen) doesn't know about KV cache
regions. The L1/L2/L3 pipeline treats all regions identically.

**Impact:** Native backends (PE, ELF64, Mach-O) can't distinguish KV cache
regions from regular regions. No backend can implement KV cache-specific
memory management (e.g., using GPU memory pools, sliding window eviction in
compiled code).

**Fix plan:**
1. Add `is_kv_cache: bool` and `window_size: Option<u32>` to `L1Inst::NewRegion`
2. Propagate through L2/L3
3. In native codegen, use a separate memory pool for KV cache regions
4. In GPU codegen, use `cudaMallocAsync` with a memory pool for KV cache

---

## Issue 14: Effect System Has No Effect Polymorphism

**Status:** Open — makes higher-order functions impractical

`src/typechecker.rs:2818-2822` — effects are tracked as a flat
`HashSet<String>` per function. There's no row polymorphism, no effect
variables, no way to write a higher-order function that is effect-polymorphic.

A function like:
```
fn map(vec: Vec, f: fn(i64) -> i64) -> Vec { ... }
```
...always gets the effects of `f` baked in, or doesn't. There's no way to
say "this function has whatever effects `f` has."

**Impact:** Higher-order functions either over-report effects (rejected by
`@differentiable`) or under-report (soundness bug). This makes the effect
system impractical for functional-style ML code, which heavily uses `map`,
`fold`, `zip_with`, etc.

**Fix plan:**
1. Add effect variables to the effect system (like type variables for types)
2. Annotate higher-order function parameters with effect variables:
   `fn map(vec: Vec, f: fn(i64) -> i64 !e) -> Vec !e`
3. At call sites, instantiate `e` with the actual callee's effects
4. This is a significant typechecker change (~500 lines) but necessary for
   ML ergonomics

**Debate:** Koka and Effekt have row-polymorphic effects. OCaml 5 has
algebraic effects with handlers. Rust has no effect system at all (uses
`Send`/`Sync` traits instead). The question is whether Fides's flat set
approach can be extended to row polymorphism without a full rewrite.

---

## Issue 15: @quantized Scan Only Checks Direct Calls

**Status:** Open — bypass via helper functions

`src/typechecker.rs:1248-1254`:

```rust
let mut found_dequant = false;
scan_for_calls(&func.body, &mut found_dequant, "dequantize_q8");
```

This checks if `dequantize_q8` is called directly. But if function A
(`@quantized`) calls function B (not `@quantized`), and B calls
`dequantize_q8`, the scan doesn't catch it.

**Impact:** `@quantized` can be bypassed by hiding `dequantize_q8` behind a
helper function.

**Fix plan:** The effect system already propagates effects transitively
(`func_effects.get(callee)` at line 2948). The fix is to add a `Dequant`
effect (or reuse `Read` with a flag) and track it transitively, then reject
`@quantized` functions that have the `Dequant` effect. This is cleaner than
scanning for specific call names.

---

## Issue 16: Region Escape Analysis Is Not Flow-Sensitive

**Status:** Open — false negatives and false positives

`src/typechecker.rs:3600-3610` — the escape analysis snapshots `var_regions`
keys before the region block, then after the block checks which new keys
point to the region. This is a **set difference**, not flow analysis. It
doesn't handle:
- Re-assignment: `val x = region_var; x = outer_var;` — the region var is
  now hidden
- Conditional escape: `if cond { outer_var = region_var; }` — the escape
  only happens on one path
- Closure capture: `val f = || { return region_var; };` — the closure
  captures the region var

**Impact:** False negatives (region vars escape undetected) and false
positives (safe code rejected). The typechecker only catches the simplest
case: a `val` declared in the region that's still in scope after.

**Fix plan:** Implement a proper dataflow analysis:
1. Track region-origin of each variable through all paths
2. At region exit, check if any live variable has region-origin
3. Use the existing scope/liveness analysis infrastructure
4. Handle closures by checking captured variables

This is a medium-sized change (~300 lines) but important for the region
system's soundness guarantee.

---

## Issue 17: @dp Epsilon Is Not Composed Across Calls

**Status:** Open — privacy budget not tracked

Differential privacy requires **epsilon composition**: if you query the data
N times with epsilon each, the total privacy loss is N*epsilon. The
typechecker just checks that `@dp` functions have a Noise effect. There's no
tracking of cumulative epsilon across multiple calls, no privacy budget
accounting.

**Impact:** A user can call a `@dp(epsilon=0.1)` function 10000 times and
the total privacy loss is 1000, but the compiler says nothing.

**Fix plan:**
1. Add a `@privacy_budget(epsilon=N)` annotation at the module or program
   level
2. Track cumulative epsilon usage across all `@dp` call sites
3. At compile time, reject programs that exceed the privacy budget
4. This requires whole-program analysis (call graph + epsilon accounting)
5. Reference: Google's DP library and PyTorch's Opacus both do runtime
   epsilon tracking — Fides can do it at compile time

---

## Issue 18: No Tests for ML9-ML14 Edge Cases

**Status:** Open — only happy-path tests exist

The new tests are all "happy path" — they verify that the features work for
simple cases. There are no tests for:
- KV cache with 0 layers, 0 head_dim, or window > max_seq_len
- Quantization of an all-zeros tensor (scale = 1.0, but is that right?)
- `@dp` with epsilon=0 (should be rejected — no privacy)
- `@shard` with dim > tensor rank (runtime crash)
- `@checkpoint` on a function with no Diff effect (should this be an error?)
- KV cache eviction when `num_tokens > seq_len`
- Nested KV cache regions (should this be allowed?)
- `@adapter` function that modifies a non-adapter persist variable
- `@quantized` function that calls a non-`@quantized` helper which calls
  `dequantize_q8` (Issue 15)

**Fix plan:** Add edge-case tests for each of the above. Each test should
either pass (correct behavior) or fail (correct rejection). The test
harness already supports expected-fail tests.

---

## Issue 19: GPU Backend Doesn't Handle KV Cache Regions

**Status:** Open — `region kv_cache` on GPU is just a regular region

`src/gpu.rs` has no handling for `is_kv_cache` or `window_size`.
`RegionBegin` with `is_kv_cache=true` is treated identically to a standard
region. There's no GPU memory pool for KV cache, no sliding window eviction
on GPU.

**Impact:** `region kv_cache` on GPU is just a regular region with a budget
check. The sliding window is only enforced in the interpreter's `KvCache`
struct, not in compiled GPU code.

**Fix plan:**
1. In SPIR-V codegen, emit a separate storage class for KV cache allocations
2. In PTX codegen, use `cudaMallocAsync` with a memory pool
3. Emit runtime calls for sliding window eviction at the end of each
   attention layer
4. This requires the PIR fix (Issue 13) first

---

## Issue 20: Stack Alignment Bug in x86-64 Codegen

**Status:** Fixed — padding added for odd callee-saved count

`src/codegen_x64.rs:191-202` — the prologue pushes callee-saved registers
AFTER the `subq $aligned_frame, %rsp`:

```asm
pushq %rbp              ; RSP = 16n (aligned, was 16n+8 at entry)
movq %rsp, %rbp
subq $aligned_frame, %rsp  ; RSP = 16n - 16m (aligned, multiple of 16)
; Push N callee-saved registers:
pushq %rbx              ; RSP = 16n - 16m - 8
pushq %r12              ; RSP = 16n - 16m - 16
pushq %r13              ; RSP = 16n - 16m - 24  ← MISALIGNED if N is odd
```

After the `subq`, RSP is 16-byte aligned. Each `pushq` subtracts 8. If the
number of callee-saved registers is **odd**, RSP becomes 16m+8 — misaligned.

The System V ABI requires RSP to be 16-byte aligned **before** any `call`
instruction. With an odd number of callee-saved pushes, every `call` in this
function violates the ABI. The callee sees `(RSP+8) % 16 != 0` at entry,
which crashes `movaps`/`movdqa` instructions that require 16-byte alignment.

**Impact:** Functions that use an odd number of callee-saved registers
(RBX, R12-R15) and call functions that use SSE aligned instructions will
crash with SIGSEGV. This is intermittent — depends on regalloc decisions.

**Fix plan:** Add 8 bytes of padding when the number of callee-saved
registers is odd:
```rust
let pad = if a.used_callee_saved.len() % 2 == 1 { 8 } else { 0 };
let aligned_frame = (frame_size + 15) & !15) + pad;
```
And adjust the epilogue to restore RSP from RBP (which is already done via
`movq %rbp, %rsp; popq %rbp`), so the padding is automatically reclaimed.

---

## Issue 21: Borrow Checker Doesn't Handle Closures

**Status:** Open — closures bypass borrow checking entirely

`src/borrowck.rs` — the `check_expr` function (line 994) has no case for
`Expr::Closure`. A search for "Closure" or "capture" in `borrowck.rs` returns
zero matches. Closures are completely invisible to the borrow checker.

**Impact:** A closure can capture a `&mut` borrow, and the borrow checker
won't detect if the closure outlives the borrowed value or if two closures
capture conflicting borrows. For example:
```
var v = Vec::new();
val f = || { v.push(1); };  // captures &mut v
val g = || { v.push(2); };  // also captures &mut v — double mutable borrow!
f();
g();
```
This compiles without error. In Rust, this is rejected. In Fides, it's
silent — the borrow checker doesn't even visit the closure body.

**Fix plan:**
1. Add `Expr::Closure` to `check_expr` in `borrowck.rs`
2. Analyze which variables the closure captures (by scanning the body)
3. For each captured variable, record a borrow (shared or mutable based on
   usage)
4. The borrow lasts until the closure is dropped/goes out of scope
5. Reject if conflicting borrows are detected

---

## Issue 22: No Recursion Depth Limit in Interpreter

**Status:** Open — stack overflow on deep recursion

`src/interp.rs` — `run_function` calls itself recursively for function calls
(line 1368: `self.run_function(&callee, &arg_vals)`). There is no call depth
counter or limit. A user program with unbounded recursion (e.g., a recursive
factorial without a base case, or a recursive type that the typechecker
didn't catch) will cause the **Rust interpreter itself** to stack overflow,
crashing with SIGSEGV — not a graceful Fides panic.

**Impact:** A malformed Fides program crashes the interpreter with a Rust
stack overflow instead of a Fides runtime error. This is poor UX — the user
gets no error message, just a segfault.

**Fix plan:** Add a `call_depth: usize` field to `Interpreter`. Increment
on each `run_function` call, decrement on return. If depth exceeds a limit
(e.g., 10000), return `ExecOutcome::Panic("stack overflow: recursion too
deep")`. This is a 5-line fix.

---

## Issue 23: @no-recursion Only Checks Direct Self-Calls

**Status:** Open — safety-critical soundness bug

`src/typechecker.rs:2065`:
```rust
if prims.contains("no-recursion") && called == func_name {
```

This only checks if a function calls **itself directly**. Indirect recursion
(A → B → A) is not detected. The same issue applies to `@realtime` at
line 2506:
```rust
if called == name {
```

**Impact:** A `@no-recursion` function can call a helper that calls it back,
causing unbounded recursion. For `@medical` and `@safety-critical` (which
expand to `@no-recursion`), this means a safety-critical function can have
unbounded stack depth — the exact thing the annotation is supposed to
prevent.

**Fix plan:** Build a call graph and check for cycles involving the
annotated function. A function with `@no-recursion` must not be in any
cycle in the call graph (not just a self-loop). The call graph already
exists for gas computation (`compute_total_gas` uses it). Reuse it:
```rust
if prims.contains("no-recursion") {
    if is_in_cycle(func_name, &call_graph) {
        errs.push("...indirect recursion detected...");
    }
}
```

---

## Issue 24: Interpreter Never Frees Memory — No Use-After-Free Testing

**Status:** Open — interpreter hides bugs that native codegen would expose

`src/interp.rs:1701-1710` — `RegionEnd` is a no-op:
```rust
Inst::RegionEnd { name, is_gpu, is_kv_cache, .. } => {
    // End a named region — in the interpreter, this is a no-op.
    ExecOutcome::Continue
}
```

`p_free` (line 2169) and `p_region_free` (line 2173) are also no-ops:
```rust
"p_free" => {
    // Interpreter doesn't actually free — memory is arena-based
    ExecOutcome::Value(Value::Void)
}
```

The interpreter uses a bump allocator (`Memory::alloc` just increments
`next_alloc`). Nothing is ever freed.

**Impact:** A Fides program that has a use-after-free bug (accessing a
pointer after `RegionEnd`) will work fine in the interpreter but crash in
native code where `RegionEnd` actually frees memory. The interpreter gives
a false sense of correctness.

**Fix plan:**
1. Track which addresses belong to each region in the interpreter
2. On `RegionEnd`, mark those addresses as freed
3. On `Load`/`Store`, check if the address is freed and panic with
   "use-after-free: accessing freed region memory"
4. This makes the interpreter catch UAF bugs that native code would hit

---

## Issue 25: Integer Overflow in C Codegen String Operations

**Status:** Open — buffer overflow in generated C code

`src/codegen_c.rs:2527`:
```c
char* p_string_repeat(const char* s, int64_t n) {
    size_t slen = strlen(s);
    char* result = (char*)malloc(slen * (size_t)n + 1);
    result[0] = 0;
    for (int64_t i = 0; i < n; i++) strcat(result, s);
    return result;
}
```

If `slen * n` overflows `size_t` (e.g., `slen=2^32, n=2^32` on a 64-bit
system), `malloc` allocates a tiny buffer and the `strcat` loop overflows
it — heap buffer overflow.

Similarly, `p_str_concat` (line 2014):
```c
size_t len = strlen(a) + strlen(b);
```
If both strings are near `SIZE_MAX/2`, the addition overflows.

**Impact:** A Fides program that calls `string_repeat` with a large count
or concatenates very large strings can trigger a heap buffer overflow in
the generated C binary. This is a security vulnerability.

**Fix plan:** Add overflow checks before `malloc`:
```c
if (n < 0 || slen > SIZE_MAX / (size_t)n) return strdup("");
size_t total = slen * (size_t)n + 1;
char* result = (char*)malloc(total);
if (!result) return strdup("");
```

---

## Issue 26: No Monomorphization Depth Limit

**Status:** Open — stack overflow on recursive generic instantiation

`src/mono.rs:1012` — `instantiate` recursively processes the function body,
which can call `instantiate` again for nested generic calls. The `seen` set
(line 938) prevents re-instantiating the same mangled name, but each
instantiation with a different type parameter produces a different mangled
name. A generic function that calls itself with a different type parameter:
```
fn foo<T>() { foo<Vec<T>>() }
```
would produce `foo_i64`, `foo_Vec_i64`, `foo_Vec_Vec_i64`, ... infinitely.
The `seen` set doesn't help because each mangled name is unique.

**Impact:** A recursive generic function causes the Rust compiler process
to stack overflow during monomorphization. No error message, just a crash.

**Fix plan:** Add a depth counter to `MonoCtx` and limit to e.g. 32 levels.
If exceeded, return an error: "monomorphization depth limit exceeded —
possible infinite generic recursion in function 'foo'".

---

## Issue 27: Thread Interpreter Can't Share Complex Types

**Status:** Open — design limitation, not a bug

`src/interp.rs:4095-4176` — `p_thread_spawn` only passes `i64` arguments to
threads:
```rust
// Convert args to Send-safe primitives (i64 only — threads get raw values)
let thread_args: Vec<i64> = args.iter()
    .skip(2)
    .map(|a| match a {
        Value::Int(n) => *n,
        Value::Bool(b) => *b as i64,
        Value::Float(f) => f.to_bits() as i64,
        Value::Ptr(p) => *p as i64,
        _ => 0,  // ← Vec, Map, Struct → silently becomes 0
    }).collect();
```

The thread gets a new `Interpreter::new(module)` with its own private
`memory`. Only `shared_memory` (Arc<Mutex<Memory>>), `mutexes`, and
`channels` are shared. A `Vec`, `Map`, or `struct` passed to a thread
silently becomes `0`.

**Impact:** A user passes a `Vec` to a thread and the thread receives `0`.
No error, no panic — just wrong behavior. This is a silent data loss bug.

**Fix plan:** Either:
1. **Reject non-primitive args at typecheck time** — `p_thread_spawn` only
   accepts `i64`, `f64`, `bool`, `str` args. Vec/Map/struct args are a
   compile error.
2. **Share the heap** — use `Arc<Mutex<Memory>>` for the main heap too,
   so threads can access the same addresses. This has performance
   implications but enables sharing complex types.

Option 1 is safer and simpler.

---

## Issue 28: No Type Inference — All Types Must Be Explicitly Annotated

**Status:** Open — ergonomic limitation

`src/typechecker.rs` — there is no Hindley-Milner, no unification, no type
variables. A search for "Hindley", "Milner", "unification", "type_var"
returns zero matches. Every variable must have an explicit type annotation
or a literal initializer from which the type is obvious.

**Impact:** Fides is more verbose than Rust, OCaml, or Haskell. You can't
write `val x = compute()` without annotating the type of `x`. This makes
functional-style code (chains of `map`, `filter`, `fold`) very verbose:
```
val a: Vec<i64> = compute();
val b: Vec<i64> = vec_map(a, f);
val c: i64 = vec_sum(b);
```
instead of:
```
val c = a.map(f).sum();
```

**Fix plan:** Implement bidirectional type inference:
1. Literals and function returns propagate types downward
2. Function parameters and annotations propagate types upward
3. Use unification for generic type parameters
4. This is a large project (~2000 lines) but essential for ergonomics

**Debate:** Full HM type inference is complex. Bidirectional inference
(simpler, used by Swift and Rust) might be sufficient. The question is
whether to invest in this before or after self-hosting.

---

## Issue 29: No Hex/Unicode Escape Sequences in String Literals

**Status:** Open — limitation

`src/lexer.rs:268-275` — the lexer only supports: `\n`, `\t`, `\r`, `\\`,
`\"`, `\0`. There is no `\x41` (hex) or `\u0041` (unicode) escape sequence.
Unknown escapes produce a warning but are silently included as the literal
character.

**Impact:** You can't write string literals containing arbitrary bytes or
unicode characters. This matters for binary protocols, regex patterns, and
internationalization. A user wanting `\x00\x01\x02` must use `char(0) +
char(1) + char(2)`.

**Fix plan:** Add `\xHH` (hex byte) and `\u{HHHH}` (unicode codepoint)
escape sequences to the lexer. ~20 lines of code.

---

## Issue 30: Linker DCE Doesn't Handle Indirect Calls

**Status:** Open — function pointers can be incorrectly eliminated

`src/linker.rs:201-230` — `dead_code_elimination` builds a call graph from
relocations. A relocation `R_X86_64_PLT32` or `R_X86_64_PC32` at offset X
in function A targeting symbol B means "A calls B". But if A loads the
address of B into a register (e.g., `lea rax, [rip+B]`) and later calls
`rax`, there's no relocation targeting B from the call site — the
relocation is at the `lea`, and the call is indirect.

The DCE treats `lea` (loading a function address) as a reference, so it
won't eliminate B. But if the `lea` is optimized away by the peephole
optimizer and replaced with a different pattern, B could be eliminated.

More importantly, vtable entries (`.quad function_name`) are in `.data`/
`.rodata`, not `.text`. The DCE only scans `.text` relocations. If a
function is only reachable through a vtable, it will be eliminated.

**Impact:** Virtual method calls through vtables can crash if the method
is DCE'd. The function exists in the vtable but the function body is
removed from `.text`.

**Fix plan:**
1. Mark all functions that appear in vtables as roots (always reachable)
2. Mark all functions whose address is taken (via `lea` or `.quad`) as
   roots
3. Only DCE functions that are exclusively called via direct `call`
   instructions

---

## Issue 31: p_vec_get Clamps After Panic Check — Silent Wrong Value in unsafe

**Status:** Fixed — unsafe mode now does raw access without clamp

`src/interp.rs:2396-2409` — `p_vec_get` panics on out-of-bounds in safe mode,
but then **clamps the index** regardless:

```rust
if self.interp_unsafe_depth == 0 {
    if *idx < 0 || (*idx as usize) >= borrow.len() {
        return ExecOutcome::Panic(...);  // panics in safe mode
    }
}
// This runs in BOTH safe and unsafe mode:
let idx_clamped = if borrow.is_empty() { 0 } else { (*idx as usize).min(borrow.len() - 1) };
return ExecOutcome::Value(borrow[idx_clamped].clone());
```

In safe mode, the panic fires first, so the clamp is unreachable. But in
`unsafe {}` mode, the panic is skipped and the clamp silently returns the
**last element** instead of the out-of-bounds element. This is wrong —
`unsafe` should allow raw memory access (the actual OOB read), not silently
return a different value.

**Impact:** In `unsafe {}` blocks, `vec_get(v, 100)` on a 3-element vec
returns `v[2]` instead of either panicking or reading the actual memory at
offset 100. This hides bugs — the user thinks they got the element at
index 100, but they got the last element.

**Fix plan:** In unsafe mode, skip both the panic AND the clamp:
```rust
let idx_to_use = if self.interp_unsafe_depth > 0 {
    *idx as usize  // raw access, no clamp
} else if borrow.is_empty() {
    0
} else {
    (*idx as usize).min(borrow.len() - 1)
};
```

---

## Issue 32: p_malloc Accepts Negative Sizes — Huge Allocation or Crash

**Status:** Fixed — negative sizes now return NULL

`src/codegen_c.rs:2040`:
```c
void* p_malloc(int64_t size) { return malloc((size_t)size); }
```

`int64_t` is signed. If `size` is negative (e.g., from an integer overflow
in a computation), `(size_t)size` wraps to a huge value (e.g., 0xFFFFFFFFFFFFFFFF).
On 64-bit systems, `malloc` will fail and return NULL. But the caller may
not check for NULL, leading to a NULL pointer dereference.

Worse, on 32-bit systems or with certain `malloc` implementations, the
truncated value might be small enough to succeed, allocating a tiny buffer
when the caller expected a large one — classic heap buffer overflow.

**Impact:** A Fides program that computes a buffer size with integer
overflow can trigger a heap buffer overflow or NULL deref in the C backend.

**Fix plan:** Add a bounds check:
```c
void* p_malloc(int64_t size) {
    if (size < 0 || size > (int64_t)SIZE_MAX) return NULL;
    return malloc((size_t)size);
}
```
And ensure callers check for NULL (or have the runtime abort on NULL).

---

## Issue 33: p_file_read Doesn't Check ftell for Failure

**Status:** Fixed — ftell failure now checked, returns NULL

`src/codegen_c.rs:2046-2047`:
```c
fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
char* buf = (char*)malloc((size_t)sz + 1);
```

`ftell` returns `-1L` on failure (e.g., for pipes, sockets, or special
files like `/proc/cpuinfo`). The code doesn't check for this. When `sz = -1`:
- `(size_t)sz + 1` = `SIZE_MAX` on 64-bit → `malloc` fails → returns NULL
- Or on 32-bit: `(size_t)(-1) + 1` = `0` → `malloc(0)` → returns NULL or
  a tiny buffer → `fread` writes 0 bytes → `buf[0] = 0` → returns empty
  string (silent data loss)

**Impact:** Reading from `/proc/cpuinfo`, a pipe, or a socket crashes or
silently returns empty data.

**Fix plan:**
```c
long sz = ftell(f);
if (sz < 0) { fclose(f); return NULL; }
```

---

## Issue 34: HTTP Response Always Says "OK" Regardless of Status Code

**Status:** Open — protocol violation

`src/codegen_c.rs:3400`:
```c
snprintf(header, sizeof(header),
    "HTTP/1.1 %lld OK\r\nContent-Type: text/plain\r\nContent-Length: %zu\r\n\r\n",
    (long long)status, strlen(body));
```

The status text is hardcoded to "OK" regardless of the status code. A 404
response becomes `HTTP/1.1 404 OK`, a 500 becomes `HTTP/1.1 500 OK`. This
violates HTTP/1.1 (RFC 7230 §3.1.2 requires the reason phrase to match the
status code). Some HTTP clients parse the reason phrase and may behave
incorrectly.

**Impact:** HTTP responses from Fides servers have incorrect reason phrases.
Most clients ignore the reason phrase, but strict parsers (e.g., curl with
`--fail`) may behave unexpectedly.

**Fix plan:** Add a status code → reason phrase lookup table:
```c
const char* reason = (status == 200) ? "OK" :
                     (status == 404) ? "Not Found" :
                     (status == 500) ? "Internal Server Error" : "Unknown";
snprintf(header, ..., "HTTP/1.1 %lld %s\r\n...", (long long)status, reason, ...);
```

---

## Issue 35: p_json_get Uses strstr for Parsing — Injection via Key Names

**Status:** Open — security vulnerability

`src/codegen_c.rs:3301`:
```c
char pattern[256];
snprintf(pattern, sizeof(pattern), "\"%s\":", key);
const char* found = strstr(s, pattern);
```

This builds a search pattern by inserting `key` directly into a string
template. If `key` contains `"` or `\`, the pattern won't match the actual
JSON key encoding. For example, `key = 'a"b'` produces `"a"b":` which won't
match `"a\"b":` in the JSON.

Worse, a malicious JSON payload could craft a string that matches the
pattern at the wrong position. For example, if `key = "x"`, the pattern is
`"x":`. A JSON string value containing `"x":` (e.g., `{"a": "hello \"x\": world"}`)
would match inside the string value, not at the key.

**Impact:** `p_json_get` returns wrong data for keys containing special
characters, and can be tricked by crafted JSON payloads. This is not a
proper JSON parser — it's string matching.

**Fix plan:** Replace `p_json_get` with a real JSON parser (state machine
that tracks string boundaries, escapes, nesting). The existing `p_json_*`
functions are all string-based hacks, not a real parser. This is a
significant rewrite but necessary for correctness.

---

## Issue 36: p_socket_recv_str Doesn't Check n Against Buffer Size

**Status:** Open — potential buffer overflow

`src/codegen_c.rs:2755-2760`:
```c
char* p_socket_recv_str(int64_t fd) {
    char buf[4096];
    int64_t n = p_socket_read(fd, buf, sizeof(buf)-1);
    if (n <= 0) return strdup("");
    buf[n] = 0;
    return strdup(buf);
}
```

`p_socket_read` is declared as `int64_t p_socket_read(int64_t fd, char* buf,
int64_t max_len)`. If `p_socket_read` has a bug and returns a value larger
than `sizeof(buf)-1` (4095), then `buf[n] = 0` writes past the buffer —
stack buffer overflow.

The `max_len` parameter limits the read, but we're trusting the
implementation of `p_socket_read` to honor it. If there's a bug in
`p_socket_read` (e.g., on Windows where `recv` returns `int` but the
result is cast to `int64_t`), this could overflow.

**Impact:** Stack buffer overflow if `p_socket_read` returns more than
requested. Defense in depth: even if `p_socket_read` is correct, this
should be defensive.

**Fix plan:**
```c
if (n < 0) return strdup("");
if (n >= (int64_t)sizeof(buf)) n = sizeof(buf) - 1;
buf[n] = 0;
```

---

## Issue 37: LSP Go-to-Definition Uses Text Search, Not AST

**Status:** Open — incorrect results, false positives

`src/lsp.rs:908-934` — `find_definition` scans the source text line by line
looking for patterns like `fn name` or `struct name`. This is a text
search, not an AST lookup.

Problems:
- **Comments:** A comment containing `// we use fn foo here` would match
  for `foo`, returning the comment line as the definition.
- **String literals:** `val s = "fn bar"` would match for `bar`.
- **Shadowing:** If two modules define `fn helper`, the first one in the
  text is returned, not the one in scope.
- **Method names:** A method `foo` on struct `A` and a free function `foo`
  are indistinguishable — the first match wins.
- **Imported symbols:** `use math::sqrt` — `sqrt` is not defined in the
  current file, but `find_definition` returns null instead of jumping to
  the stdlib.

**Impact:** Go-to-definition frequently returns wrong locations or fails
to find definitions. This degrades the LSP experience significantly.

**Fix plan:** Use the typechecker's symbol table (`tc.get_funcs()`,
`tc.get_structs()`, etc.) to resolve symbols, then use the AST's source
line information to find the exact definition location. The typechecker
already knows which function/struct is in scope — use that information
instead of text search.

---

## Issue 38: LSP Re-parses Entire Stdlib on Every Request

**Status:** Open — performance issue

`src/lsp.rs:636` — `handle_completion` calls
`compiler::build_program_for_lsp(text)` on every request. This function
(`src/compiler.rs:1068`) parses `STDLIB_CORE` (an `include_str!` of
`stdlib/core.fd`, ~4000 lines) from scratch on every call.

Every keystroke that triggers completion (typically after `.` or identifier
prefix) re-parses the entire stdlib. On a slow machine, this can take
100-500ms per request, making the LSP feel sluggish.

**Impact:** LSP completion, hover, and go-to-definition are slow because
they re-parse the stdlib every time. The LSP is not usable for real-time
editing on slower machines.

**Fix plan:** Cache the parsed stdlib `Program` and `TypeChecker` globally
(lazy_static or OnceLock). On each LSP request, clone the cached state and
merge only the user's code. This reduces per-request work from "parse
stdlib + user code" to "parse user code only".

---

## Issue 39: Formatter Tokenizer Diverges from Main Lexer

**Status:** Open — formatter can produce invalid code

`src/fmt.rs:61-200` — the formatter has its own tokenizer (`tokenize`)
that is a separate implementation from the main lexer (`src/lexer.rs`).
The two lexers can diverge:

- The formatter tokenizer handles f-strings (line 121) but not nested
  f-string expressions with complex interpolation (`f"{a + b}"` — the
  `}` inside the expression terminates the string early).
- The formatter tokenizer handles `//` and `/* */` comments but not
  nested block comments (`/* outer /* inner */ still comment */`).
- The formatter tokenizer's number parsing (line 158) doesn't handle
  scientific notation (`1e10`) or negative exponents (`1e-10`).
- The formatter tokenizer's identifier parsing (line 187) uses
  `is_alphabetic()` which accepts Unicode letters, but the main lexer
  may restrict to ASCII.

**Impact:** Running `fidesc fmt` on a file with f-string interpolation,
nested comments, or scientific notation can produce invalid Fides code.
The formatter silently corrupts the source.

**Fix plan:** Either:
1. **Share the lexer** — use `src/lexer.rs` for tokenization and add
   comment preservation as a separate pass.
2. **Test against the main lexer** — run both tokenizers on every test
   file and flag divergences.

Option 1 is cleaner but requires modifying the main lexer to preserve
comments (currently dropped).

---

## Issue 40: Register Allocator Uses Whole-Function Live Intervals, Not Block-Local

**Status:** Open — excessive spilling, suboptimal code quality

`src/regalloc.rs:480-484`:
```rust
/// This is NOT a precise liveness analysis (no dataflow). It's the classic
/// linear-scan simplification: each VReg's interval spans from its first
/// definition to its last use across the whole function. This is
/// conservative (intervals may be longer than necessary) but correct.
```

The live interval of each VReg spans from its first definition to its last
use **across the entire function**, not just the blocks where it's live.
This means a variable defined in block 1 and used in block 10 has an
interval covering all blocks 1-10, even if it's dead in blocks 2-9. This
forces the register allocator to keep the variable in a register (or
spill it) across all those blocks.

**Impact:** Excessive register spilling. A function with 20 variables
that are each used in only one block will have 20 overlapping intervals,
causing the allocator to spill most of them — even though no more than
2-3 are live at any point. This produces significantly worse code than
LLVM or gcc.

**Fix plan:** Implement proper liveness analysis (backward dataflow):
1. Compute `live_in` and `live_out` for each block
2. A VReg's live interval is the union of blocks where it's live
3. Use this for register allocation

This is a medium-sized change (~400 lines) but would significantly improve
code quality.

---

## Issue 41: DSE Is Block-Local Only — Misses Cross-Block Dead Stores

**Status:** Open — missed optimization, not a correctness issue

`src/opt.rs:1520-1563` — the dead store elimination (Pattern 1) walks each
block in reverse, looking for stores overwritten by later stores in the
**same block**. If a store in block A is overwritten by a store in block B
(a later block that dominates A's successors), the first store is not
eliminated.

Example:
```
block A:
    store x = 1      // dead — overwritten in block B
    jump block B
block B:
    store x = 2      // overwrites the store in A
    return x
```

The store in block A is dead, but DSE doesn't detect it because it only
looks within each block.

**Impact:** Missed optimization — dead stores remain in the code, wasting
memory bandwidth. Not a correctness issue (the program still works
correctly, just slower).

**Fix plan:** Implement cross-block DSE using a backward dataflow analysis:
1. Compute which stores are "killed" (overwritten) along all paths to
   function exit
2. A store is dead if it's killed on all paths before any load of the
   same address
3. This requires the alias analysis already present in the optimizer

---

## Issue 42: Async/Await Is Thread-Based, Not State Machine

**Status:** Open — design limitation, not a bug

`src/interp.rs:4180-4263` — `p_executor_spawn` and `p_thread_spawn` both
use `std::thread::spawn` (real OS threads). The comment at line 4304 says:
```
// A2 will make this non-blocking inside async contexts (state machine).
```

But A2 (the state machine transform for stackless coroutines) was never
implemented. There is no IR-level transform that converts `async`/`await`
into a state machine. Every `async` function runs on its own OS thread.

**Impact:**
- **Performance:** Each async task creates an OS thread (1-8MB stack).
  10,000 concurrent tasks = 10,000 threads = 10-80GB of stack memory.
  A real async runtime (tokio, async-std) handles 10,000 tasks on a
  single thread with ~1MB total.
- **Scalability:** OS thread creation is ~50µs. A high-throughput HTTP
  server can't afford 50µs per request.
- **Semantics:** `await` blocks the thread (it calls `handle.join()`),
  so you can't have multiple concurrent awaits on the same thread.

**Fix plan:** Implement the state machine transform:
1. In the lowerer, transform `async fn` into a state machine struct
   with fields for locals and a state enum
2. `await` becomes a state transition (save locals, return Pending)
3. The executor polls the state machine without blocking
4. This is a large project (~1500 lines) but essential for real async

---

## Issue 43: No Error Recovery in Parser — One Error Stops Compilation

**Status:** Open — poor developer experience

`src/parser.rs:1412` — `parse_stmt` returns `Result<Stmt, String>`. When
a parse error occurs, it propagates up via `?` and stops parsing. The
`errors` field (line 22) exists for error recovery but is not used for
continuation after errors.

The parser collects errors in `self.errors` but the main `parse()` function
returns `Err(first_error)` — only the first error is reported.

**Impact:** A syntax error at line 10 prevents the parser from reaching
line 20, which might have another error. The developer fixes the first
error, recompiles, and only then sees the second. This is slow iteration.

**Fix plan:** Implement panic-mode error recovery:
1. On parse error, record it in `self.errors`
2. Skip tokens until a synchronization point (`;`, `}`, `fn`, `val`, etc.)
3. Continue parsing from there
4. Return all collected errors at the end
5. The typechecker should also handle partial ASTs (skip functions with
   parse errors)

---

## Issue 44: `break` Doesn't Work in Constant-Range `for` Loops

**Status:** ✅ Fixed — comptime unrolling skips loops with break/continue

`src/comptime.rs` lines 229-258 — the comptime loop unrolling pass
replaces `for i in 0..10 { ... break; }` with `if true { ...; break; ...; }`.
The `break` statement has no loop to break from inside an `if` block, so
it's silently ignored. The loop runs all iterations.

**Repro:**
```fides
for i in 0..10 {
    if i == 5 { break; }
    print(i);  // prints 0 1 2 3 4 5 6 7 8 9 instead of 0 1 2 3 4
}
```

With non-constant ranges (`for i in 0..n`), `break` works correctly
because the loop is not unrolled.

**Fix:** Option 1 was implemented — the comptime unrolling pass now
detects `break`/`continue` in the loop body and skips unrolling for
those loops, preserving correct break semantics.

---

## Issue 45: `&mut T` and `&T` Can't Call Methods on Inner Type

**Status:** ✅ Fixed — auto-deref &T/&mut T for method resolution

`src/typechecker.rs` line 5216+ — method call resolution only handles
`Type::Named` and `Type::Generic`, not `Type::Ref`. When the receiver is
`&mut Vec` or `&Vec`, calling `.len()`, `.get()`, `.push()` etc. fails
with "cannot call method on type &mut Vec".

**Repro:**
```fides
fn helper(v: &mut Vec) -> void {
    v.push(42);  // Type error: cannot call method 'push' on type &mut Vec
}
```

**Impact:** `test_game_stdlib.fd` fails (known-fail). Many stdlib
functions take `&mut` references to structs with Vec fields.

**Fix:** Auto-deref `Type::Ref { inner, .. }` to `inner` for method
lookup. The typechecker unwraps one layer of `Type::Ref` before looking
up the struct name in `self.methods`. The lowerer loads through
`Ptr(Ptr(...))` references before dispatching, and handles
`Ptr(Ptr(Struct(name)))` for struct name extraction. Test:
`test_refs_mut_methods.fd` — methods on `&mut Counter` work without
explicit deref.

---

## Issue 46: `val` Reassignment Is Silently Allowed

**Status:** ✅ Fixed — val reassignment now emits a type error

`val x = 10; x = 20;` compiles and runs without error, producing `20`.
`val` is supposed to be immutable (like Rust's `let` without `mut`).

**Repro:**
```fides
val x = 10;
x = 20;
print(x);  // prints 20 — should be a compile error
```

**Impact:** Bugs where a `val` is accidentally reassigned are silently
swallowed. This caused the empty SHA-256 hex string bug (accumulators
declared as `val` silently didn't accumulate).

**Fix:** The typechecker now tracks which variables are `val` (immutable)
and rejects `Assign` statements targeting them. A `mutable_vars:
HashSet<String>` was added to the typechecker, populated during
`Stmt::Let`/`Stmt::Var` processing, checked during `Stmt::Assign`.
247 `val`→`var` conversions were made in stdlib and test files where
variables were declared `val` but later reassigned (loop counters,
accumulators, etc.).

---

## Issue 47: `val` Struct Fields Can Be Mutated

**Status:** ✅ Fixed — FieldAssign now checks val binding mutability

`val c = Counter { count: 0 }; c.count = 10;` used to compile and run.
The struct binding is `val` (immutable) but its fields could be mutated
via `Stmt::FieldAssign`, which was a separate code path from
`Stmt::Assign` (Issue 46) and did not check `val_vars_per_scope`.

**Fix:** The `Stmt::FieldAssign` handler in the typechecker now extracts
the object variable name (when it's an `Expr::Ident`) and checks
`val_vars_per_scope`. If the variable was declared with `val`, a type
error is emitted: "cannot assign to field '{}' of '{}' — declared with
`val` (immutable)." This is a hard error (not a warning) because field
mutation through a `val` binding is always a bug, not a style issue.

---

## Issue 48: IR Verification Fails for `unsafe` Blocks with `return`

**Status:** ✅ Fixed — UnsafeEnd/RegionEnd/TxnEnd skipped when block already has terminator

`fidesc audit` reports "block 12 does not end with a terminator (ends
with UnsafeEnd)" for `p_str_equal` and `p_str_compare` in
`stdlib/core.fd`. The `unsafe { ... return ...; }` block generates an
`UnsafeEnd` instruction after the `return`, which the verifier rejects.

**Impact:** `fidesc audit` fails even for `hello.fd`. The audit command
is effectively broken for any program that uses `unsafe` blocks with
early returns (which core.fd does).

**Fix:** The lowerer now checks `self.current_block_has_terminator()`
before emitting `UnsafeEnd`, `RegionEnd`, and `TxnEnd`. The interpreter
also saves/restores `region_stack` depth on function entry/exit to
handle early returns inside region blocks without corrupting the stack
for subsequent calls.

---

## Issue 49: Non-Exhaustive `match` Not Caught

**Status:** ✅ Fixed — exhaustiveness checking emits warnings

```fides
enum Color { Red, Green, Blue }
match c {
    Color::Red => {},
    Color::Green => {},
    // Missing Color::Blue — no error!
}
```

The typechecker doesn't verify that all enum variants are covered (or
that a wildcard `_` arm exists). Missing variants silently fall through.

**Fix:** Exhaustiveness checking added to both `Stmt::Match` and
`Expr::Match` in the typechecker. For enums, all variants must be
covered (or a wildcard/ident catch-all). For bools, both `true` and
`false` must be covered. Emits a **warning** (not error) to allow
intentional non-exhaustive matches during development. Test:
`test_match_exhaustive.fd`.

---

## Issue 50: Stale Skip List — All 26 Tests Now Pass

**Status:** Resolved by verification — needs cleanup

All 26 tests in the `skip_tests` array in `src/main.rs` now pass on the
interpreter. Many also pass on C codegen. They should be removed from
the skip list and counted in the normal test suite.

**Tests that pass:** test_comptime_basic, test_comptime_func,
test_comptime_reflect, test_comptime_serialize, test_comptime_gen,
test_kernel_specialization, test_http, test_json, test_tensor,
test_autodiff, test_kv_cache, test_quantization, test_checkpoint,
test_dp, test_shard, test_lora, test_simd, test_parallel, test_shape,
test_wasm, test_allocator, test_effect_handler, test_asm, test_bare,
test_boot, test_interrupt, test_mmu, test_gat, test_ffi_header,
test_dlopen

**Fix plan:** Remove all entries from `skip_tests` array. Run
`fidesc test` to verify. If any fail on C codegen (but pass on
interpreter), add them to `known_fail` with a C-codegen-specific note.

---

## Issue 51: Stale Known-Fail List — 6 of 8 Now Pass

**Status:** Resolved by verification — needs cleanup

6 of 8 tests in the `known_fail` array now pass (bonus passes). Only
`test_game_stdlib` (Issue 45) and `test_vec_shape_fail` still fail.
`test_vec_shape_fail` is actually an expected-fail test (it tests
compile-time error detection), not a pre-existing failure.

**Fix plan:**
1. Remove passing tests from `known_fail`: test_iter, test_phase8b,
   test_vec_math, test_lang_features, test_const_generic, test_matrix_mul
2. Move `test_vec_shape_fail` to expected-fail category
3. Keep `test_game_stdlib` in `known_fail` until Issue 45 is fixed

---

## Issue 52: Duplicate Stdlib Modules — `result.fd` vs `core.fd` Result

**Status:** Open — consolidation needed (LOW PRIORITY)

`core.fd` has `enum Result<T, E> { Ok, Err }` with methods (map,
and_then, or_else, unwrap_or_else, expect).

`result.fd` has `struct ResultValue { tag, value, error }` with
functions (ResultValue_ok, resultvalue_is_ok, resultvalue_unwrap, etc.).

These are two different implementations of the same concept. The
`core.fd` version is more idiomatic (enum-based, generic). The
`result.fd` version was created because the enum-based Result had
limitations (Issue 1: hardcoded to i64).

**Fix plan:** After Issue 1 is fixed (generic Result payloads), remove
`result.fd` and use `core.fd`'s `Result<T, E>` everywhere. If
`ResultValue` is needed for non-generic use cases, keep it as a thin
wrapper.

---

## Issue 53: Duplicate Stdlib Modules — `fs.fd` vs `file.fd`

**Status:** Open — consolidation needed (LOW PRIORITY)

`file.fd`: file_read, file_write, file_append, file_exists,
file_write_bytes, file_read_bytes, file_delete, file_size (8 functions)

`fs.fd`: fs_exists, fs_file_size, fs_read_text, fs_read_bytes,
fs_write_text, fs_write_bytes, fs_append_text, fs_delete, fs_rename,
fs_list_dir, fs_is_dir, fs_is_file, fs_join, fs_path_in (15 functions)

`fs.fd` is a superset of `file.fd`. Both exist and are imported by
different tests.

**Fix plan:** Merge `file.fd` into `fs.fd`. Keep `file_*` functions as
aliases that call `fs_*` equivalents. Or remove `file.fd` and update
all imports to use `fs`.

---

## Issue 54: Duplicate Stdlib Modules — `os.fd` vs `env.fd` + `args.fd`

**Status:** Open — consolidation needed (LOW PRIORITY)

`env.fd`: env_get, env_exists (2 functions)
`args.fd`: args_len, args_get, args_program_name (3 functions)
`os.fd`: os_getenv, os_hasenv, os_argc, os_argv, os_args, os_exit,
os_system, os_name, os_home_dir, os_cwd, os_user, os_hostname,
os_shell, os_term, os_path, os_tmp_dir, os_path_sep,
os_path_list_sep (18+ functions)

`os.fd` is a superset of `env.fd` and `args.fd`.

**Fix plan:** Merge `env.fd` and `args.fd` into `os.fd`. Keep existing
function names as aliases, or update all imports.

---

## Issue 55: Debug Print Left in `lower.rs`

**Status:** Open — code hygiene (LOW PRIORITY)

`src/lower.rs` line 3472:
```rust
eprintln!("DEBUG METHOD: method={}, struct_name={:?}, mangled={}, impl_methods_has={}", ...);
```

This debug print fires on every method call that falls through to the
impl_methods lookup. It should be removed or gated behind `--verbose`.

**Fix plan:** Remove the `eprintln!` or wrap in `if self.verbose { ... }`.

---

## Issue 56: `random_seed_time()` Is a Stub

**Status:** Open — non-deterministic seed unavailable (LOW PRIORITY)

`stdlib/random.fd` line 42:
```fides
pub fn random_seed_time() -> void {
    RNG_DEFAULT.state = 12345;  // TODO: use p_time() when available
}
```

The `p_time()` function is not a registered runtime function in C
codegen. The function always seeds with 12345, making it deterministic
(the opposite of its intended purpose).

**Fix plan:** Either register `p_time` in the C codegen runtime, or
remove `random_seed_time()` and document that users should use
`random_seed(seed)` with their own seed.

---

## Issue 57: 26 Stdlib Modules Redundantly `use core;`

**Status:** Open — code hygiene (LOW PRIORITY)

`core.fd` is auto-imported by the compiler. The `use core;` statement
in 26 stdlib modules is redundant and clutters the import section.

**Fix plan:** Remove `use core;` from all stdlib modules. The compiler
already handles core.fd as a special case (always prepended).

---

## Issue 58: 89 Uses of `str_concat()` Could Use `+` Operator

**Status:** Open — code consistency (LOW PRIORITY)

After the string concatenation `+` fix, `str_concat(a, b)` can be
replaced with `a + b` throughout stdlib. 89 call sites use `str_concat`
or `p_str_concat` directly.

**Fix plan:** Replace `str_concat(a, b)` with `a + b` in stdlib for
consistency. This is a mechanical change — find and replace. Keep
`str_concat` as a function for compatibility but prefer `+` in new code.

---

## Issue 59: Drop Trait + Compiler-Inserted Drops

**Status:** ✅ Fixed — Drop trait implemented with automatic drop calls at exit

Fides had no RAII (Resource Acquisition Is Initialization) support.
Structs that hold resources (file handles, mutexes, GPU memory) had no
automatic cleanup — users had to manually call `close()`/`free()` or
use `defer` blocks.

**Fix:** When a struct has `impl T { fn drop(self: &mut T) }`, the
compiler automatically calls `T_drop` at every function exit point
(explicit `return`, implicit function end, `?` error propagation).
`IrFunction.drop_locals` tracks `(alloca_ptr, struct_name)` pairs for
Drop-eligible locals. `emit_drops()` in the lowerer emits calls in
reverse declaration order (LIFO — last declared is dropped first,
matching Rust's drop order). The drop call includes the dummy `env_ptr`
argument expected by user functions.

**Test:** `test_drop.fd` — `Logger` struct with `drop` method. "Logger
dropped" printed at function exit on interpreter + PE emitter. C codegen
has `test_drop` in known-fail list (auto-drop not yet implemented in C
backend — the drop call is emitted but the C codegen's function
signature handling differs).

**Limitation:** Only `Stmt::Let` with explicit type annotations are
tracked. Variables without type annotations, function parameters, and
struct fields are not yet drop-eligible. Scope-level drops (block exits)
are not implemented — drops only happen at function exit.

---

## Pre-Self-Host Refining Checklist

Based on the comprehensive code analysis, here is the priority-ordered
checklist of bugs to fix and consolidations to make before self-hosting:

### Must Fix (Correctness Bugs)

| # | Issue | File | Effort | Status |
|---|-------|------|--------|--------|
| 44 | `break` in constant `for` loops | comptime.rs | Small | ✅ Fixed |
| 45 | `&mut T` method calls | typechecker.rs | Medium | ✅ Fixed |
| 46 | `val` reassignment silently allowed | typechecker.rs | Small | ✅ Fixed |
| 47 | `val` struct field mutation | typechecker.rs | Small | ✅ Fixed |
| 48 | `unsafe` block IR verification | lower.rs | Small | ✅ Fixed |
| 50 | Stale skip list (26 tests pass) | main.rs | Trivial | ✅ Fixed |
| 51 | Stale known-fail list (6 pass) | main.rs | Trivial | ✅ Fixed |

### Should Fix (Safety/Ergonomics)

| # | Issue | File | Effort | Status |
|---|-------|------|--------|--------|
| 49 | Non-exhaustive match | typechecker.rs | Medium | ✅ Fixed |
| 56 | `random_seed_time` stub | random.fd / codegen_c.rs | Small | Open |
| 59 | Drop trait + compiler-inserted drops | lower.rs, ir.rs | Medium | ✅ Fixed |

### Consolidate (Code Duplication)

| # | Issue | Files | Effort |
|---|-------|------|--------|
| 52 | `result.fd` vs `core.fd` Result | result.fd, core.fd | Small |
| 53 | `fs.fd` vs `file.fd` | fs.fd, file.fd | Small |
| 54 | `os.fd` vs `env.fd` + `args.fd` | os.fd, env.fd, args.fd | Small |
| 57 | Redundant `use core;` | 26 stdlib files | Trivial |
| 58 | `str_concat` → `+` | 89 call sites | Trivial |

### Clean Up (Compiler Hygiene)

| # | Issue | File | Effort |
|---|-------|------|--------|
| 55 | Debug print in lower.rs | lower.rs:3472 | Trivial |
| — | 207 compiler warnings | all src/*.rs | Medium |
| — | 7 unreachable patterns | ir.rs, mono.rs | Trivial |
| — | Dead code (5+ functions/variants) | x86.rs, opt.rs, etc. | Small |
| — | Elf64 naming convention | elf64.rs | Trivial |
| — | Debug dump comments | compiler.rs:1435-1455 | Trivial |

### Existing Issues Now Partially Resolved

| # | Issue | Status Update |
|---|-------|---------------|
| 8 | PRNG deterministic | `stdlib/random.fd` now uses pure-Fides LCG (no FFI). The DP noise in `interp.rs` is still deterministic — that part is still open. |
| 28 | No type inference | Bidirectional type inference now exists (`Type::Infer`, `let_hint`, contextual hints). Not full HM but significantly better. Partially resolved. |
| 1 | Result hardcoded to i64 | `result.fd` provides `ResultValue` as a workaround (struct-based, tag field). The enum-based `Result<T,E>` in `core.fd` is still i64-only. Still open for the enum version. |

---

## Concurrency Decision: Effect Handlers vs async/await

**Question:** For developers who want async/await, what happens? Will
effect handlers be the default, or can both work together?

### The Short Answer

**Both can work together — but only if async/await is implemented *on
top of* effect handlers, not alongside them.** This is one mechanism
with two surfaces, not two mechanisms.

### Why Not "Both Side by Side"

If async/await and effect handlers are separate mechanisms (the current
state), you get:
- The "colored function" problem (async functions can't call sync
  functions that need to await)
- Two mental models developers must learn
- Interop friction (can an effect handler suspend an async function?)
- Violation of Design Principle #5

This is the trap Rust fell into: async/await + threads + channels +
`Send`/`Sync` = four concurrency models that don't compose cleanly.

### The Right Architecture: Sugar Over Effects

```
┌─────────────────────────────────────────────┐
│  Surface Syntax (developer writes this)     │
│                                             │
│  async fn fetch() -> str { await ... }      │  ← familiar
│  handle { ... } with { IO => ... }          │  ← powerful
│                                             │
├─────────────────────────────────────────────┤
│  Lowering (compiler does this)              │
│                                             │
│  async/await  ──lowers to──►  effect ops    │
│  handle/do    ──already──►    effect ops    │
│                                             │
│  ONE mechanism: effect handlers             │
├─────────────────────────────────────────────┤
│  Runtime                                     │
│                                             │
│  Single scheduler, single task model        │
│  Channels = effect (Chan)                   │
│  Threads = effect (Spawn) + OS thread pool  │
└─────────────────────────────────────────────┘
```

### How It Works in Practice

**For developers who want async/await (the 90%):**
```fides
// They write this — familiar, looks like Rust/JS/Python
async fn fetch_url(url: str) -> str {
    val resp = await http_get(url);
    return resp.body;
}
```

**The compiler lowers it to this (invisible to the developer):**
```fides
// Internally, async/await becomes effect operations
fn fetch_url(url: str) -> str !Async {
    val resp = do Async.Wait(http_get(url));
    return resp.body;
}
```

**For developers who want full control (the 10%):**
```fides
// They write effect handlers directly — more power, more control
fn with_retry<T>(f: fn() -> T !Net, max: i64) -> T {
    handle {
        return f();
    } with {
        Net.Timeout => resume => {
            if max > 0 { return with_retry(f, max - 1); }
            else { panic("max retries exceeded"); }
        }
    }
}
```

### What This Means Concretely

| Question | Answer |
|----------|--------|
| Do developers need to learn effect handlers? | **No.** Most developers use async/await and never see effects. |
| Can async functions use effect handlers? | **Yes.** They're the same thing under the hood. |
| Can effect handlers suspend async functions? | **Yes.** Same mechanism. |
| Is there a "colored function" problem? | **No.** Every function has an effect row. `async` is just `!Async` in the effect row. A "sync" function is one with an empty effect row. The type system tracks this — no keyword coloring. |
| What about channels? | **Channels become an effect.** `p_chan_send`/`p_chan_recv` are effect operations `do Chan.Send(...)` / `do Chan.Recv()`. The `select` statement is sugar over polling multiple effect operations. |
| What about threads? | **`spawn` becomes an effect operation** `do Spawn(...)`. The runtime maps it to OS threads or a thread pool. `@send`/`@sync` still enforce data-race safety. |
| Is this proven? | **Yes.** OCaml 5 does exactly this — effect handlers are the primitive, async libraries (Eio, Miou) are built on top. Koka and Effekt do the same. |

### The Migration Path

1. **Now (Phase 7.6 current state):** async/await and effects coexist
   as separate mechanisms. This works but violates the design principle.

2. **Step 1:** Lower async/await to effect operations in the lowerer
   (`src/lower.rs`). `async fn` becomes `fn ... !Async`, `await x`
   becomes `do Async.Await(x)`. No syntax change visible to users.

3. **Step 2:** Make channels and spawn into effect operations. `p_chan_send`
   becomes `do Chan.Send`. `spawn` becomes `do Spawn`.

4. **Step 3:** One scheduler handles all effect operations. The current
   executor runtime (`p_executor_*`) becomes the effect handler runtime.

5. **Result:** One mechanism (effects), two surfaces (async/await for
   the 90%, handle/with for the 10%). No colored functions. No design
   principle violation.

### Why This Is Better Than "Just Remove async/await"

- Developers coming from Rust/JS/Python already know async/await.
  Forcing them to learn effect handlers would be a adoption barrier.
- Effect handlers are more powerful but less familiar. Sugar gives
  familiarity without sacrificing power.
- OCaml 5 proved this works in production. It's not a research bet.

### Why This Is Better Than "Keep Both Separate"

- No "which concurrency model do I use?" confusion.
- No interop friction between async code and effect-handler code.
- One scheduler, one task model, one mental model for the runtime.
- Satisfies Design Principle #5.

### What Effect Handlers Enable That async/await Cannot

This is not about design principle purity. This is about 6 concrete
capabilities that are **impossible** in async/await languages and
**trivial** with effect handlers.

#### 1. Multi-shot continuations (backtracking, speculative execution)

```fides
// Backtracking search — resume the SAME suspension point multiple times
fn solve(puzzle: Puzzle) -> Option<Solution> {
    handle {
        return search(puzzle);
    } with {
        Choose(options: Vec<i64>) => resume => {
            for opt in options {
                val result = resume(opt);  // resume the SAME continuation
                if result.is_some() { return result; }
            }
            return Option::None;
        }
    }
}
```

`await` is single-shot — you await, get one result, move on. You cannot
"re-await" the same point with a different value. Effect handlers can
resume the same continuation multiple times. This enables backtracking,
probabilistic search, and speculative execution as plain library code.

**Languages that have this:** Koka, Eff, OCaml 5. **No mainstream
async/await language can do this.**

#### 2. One function, multiple execution contexts (no function coloring)

```fides
// Write this ONCE — it works sync, async, concurrent, mocked, whatever
fn process(data: Vec<i64>) -> i64 !IO {
    val raw = do IO.Read(data);
    val parsed = parse(raw);
    return transform(parsed);
}

// Caller A: synchronous (blocking IO)
handle { process(data) } with { IO.Read => resume => resume(blocking_read()) }

// Caller B: asynchronous (event loop)
handle { process(data) } with { IO.Read => resume => schedule_async(read_async(), resume) }

// Caller C: mocked (testing)
handle { process(data) } with { IO.Read => resume => resume(fake_data()) }

// Caller D: retry logic
handle { process(data) } with {
    IO.Read => resume => {
        for attempt in 0..3 {
            val result = resume(try_read());
            if result.is_ok() { return result; }
        }
        panic("read failed 3 times");
    }
}
```

In Rust, `process` must be declared `async fn` or `fn` — you pick one.
If it's async, sync callers need `block_on`. If it's sync, async callers
need a thread. The function is *colored* by its declaration. With effects,
the function declares what it *needs* (`!IO`) and the *caller* decides
*how* to provide it.

**Languages that have this:** Koka, Effekt, OCaml 5. **Rust, Go, Zig,
Python, JS — none can do this.**

#### 3. User-written schedulers (no runtime modification)

```fides
// A priority scheduler — plain Fides, no runtime hooks, no unsafe code
fn with_priority_scheduler<T>(tasks: Vec<Task>) -> Vec<T> {
    handle {
        return tasks.iter().map(|t| t.run()).collect();
    } with {
        Spawn(work: fn() -> T) => resume => {
            priority_queue.push(work, current_priority);
            resume(pending())
        }
        Yield => resume => {
            if let Some(task) = priority_queue.pop() { task(); }
            resume(())
        }
    }
}
```

In async/await languages, the scheduler is baked into the runtime
(Tokio, async-std, Node's libuv, Python's asyncio). You can't change
scheduling policy without forking the runtime or writing a custom
executor (hundreds of lines of unsafe code in Rust). With effects, the
scheduler is just a handler — user code.

#### 4. Composable retry/timeout/cancellation as libraries

```fides
// Timeout — 5 lines, no runtime support, no crate needed
fn with_timeout<T>(f: fn() -> T !Net, ms: i64) -> T {
    handle { return f(); } with {
        Net.Timeout => resume => {
            val timer = spawn(() => sleep(ms));
            val result = race(resume(continue()), timer);
            if result.is_timeout() { panic("timed out"); }
            return result;
        }
    }
}

// These compose — stack them freely
with_timeout(with_retry(with_cancel(my_func, cancel_chan), 3), 5000);
```

In Rust, timeout requires `tokio::time::timeout`, cancellation requires
`CancellationToken` from tokio-util, retry requires a crate. They're
runtime-specific, don't compose across runtimes, and you can't write
your own without deep runtime knowledge. With effects, they're 5-line
handlers that compose by nesting.

#### 5. Time-travel debugging for free

```fides
// Record every effect operation, replay from any point
fn debug_run<T>(f: fn() -> T) -> T {
    val trace = Vec::new();
    handle {
        return f();
    } with {
        IO.Read => resume => {
            val result = do IO.Read(request);
            trace.push(("read", request, result));
            resume(result)
        }
    }
    // Replay: re-run f() but feed back the recorded values
    // Step backward: resume from an earlier continuation
}
```

async/await doesn't expose continuations as values. You can't capture
"the rest of the computation" and replay it. Effect handlers make
continuations first-class — store them, resume them, serialize them.
This is why Fides's record/replay debugger works at the interpreter
level but couldn't work at the language level without effects.

#### 6. Effect-polymorphic generics (the biggest practical win)

```fides
// This generic function works for ANY effect combination — ONE map
fn map<T, U>(vec: Vec<T>, f: fn(T) -> U) -> Vec<U> {
    val result = Vec::new();
    for item in vec { result.push(f(item)); }
    return result;
}

// All of these work with the SAME map:
val a = map(numbers, |x| x * 2);                    // pure
val b = map(ids, |id| do IO.Read(fetch(id)));       // IO effect
val c = map(cells, |c| do Choose(candidates(c)));   // choice effect
```

In Rust, you need `map` for sync and `map_async` for async. Or `Stream`
traits, async closures (still unstable), and complex bounds. The
function is duplicated because async is a keyword, not an effect in the
type. With effects, `f`'s effect row is polymorphic — one `map` handles
everything.

**This is the single biggest practical advantage.** Every Rust developer
who's written `map` and `map_async` and `try_map` and `try_map_async`
feels this pain. Fides would have one `map`.

### Capabilities Summary

| Feature | async/await only | Effect handlers (with async sugar) |
|---------|-----------------|-----------------------------------|
| Familiar syntax | Yes | Yes (sugar) |
| Single-shot suspension | Yes | Yes |
| Multi-shot continuations | **No** | **Yes** — backtracking, speculative execution |
| Function coloring | **Yes (the problem)** | **No** — effects in type row, not keywords |
| User-written schedulers | **No** (runtime-baked) | **Yes** — handler is user code |
| Composable retry/timeout/cancel | **Needs runtime crates** | **5-line handlers** |
| Time-travel debugging | **No** | **Yes** — continuations are first-class |
| Effect-polymorphic generics | **No** (need map + map_async) | **Yes** — one map for all effects |
| Custom control flow (backtracking, probabilistic) | **No** | **Yes** |

**Bottom line:** 6 concrete capabilities that are impossible in
async/await languages and trivial with effect handlers. The multi-shot
continuation and effect-polymorphic generics alone eliminate entire
categories of code duplication that every Rust/Go developer deals with
daily. The sugar means developers who don't care about any of this just
write `async`/`await` and get the familiar experience. But when they
need retry, timeout, backtracking, or a custom scheduler — they don't
need a new crate or runtime fork. They write 5 lines.

---

## Guiding Philosophy: Most Powerful, Not Most Popular

**Fides is not trying to be the easiest language. It is trying to be the
most powerful.**

The goal is not "make it simple enough for beginners." The goal is:

> **Anything is possible with Fides — and better than any other language.**

Fides targets fields where power matters more than convenience:
- **Research** — experiments that need custom schedulers, backtracking,
  probabilistic execution, speculative parallelism
- **AI** — agent systems with typed LLM inference, confidence-gated
  control flow, capability-isolated credentials
- **IoT / Embedded** — bare-metal targets, deterministic memory,
  real-time guarantees, sub-4KB binaries
- **Electronics / Hardware** — inline assembly, interrupt handlers, MMU
  management, register access, device drivers
- **Robotics** — real-time control loops, hot-reload on running hardware,
  effect-based sensor/actuator abstraction
- **Systems** — OS kernels, compilers, databases, network stacks
- **Safety-critical** — medical (IEC 62304), automotive (ISO 26262),
  aerospace (DO-178C) — compile-time guarantees, not runtime hopes

**What this means for design decisions:**

1. **Ergonomics serve power, not the other way around.** Making
   something easier is good *only if it doesn't reduce capability*.
   async/await as sugar over effects is good — it's easier AND equally
   powerful. Removing effect handlers to "simplify" would be bad —
   easier but less powerful.

2. **We don't cut features because they're complex.** We cut features
   because they're *redundant*. Comptime replaces macros — that's a cut
   for non-redundancy, not for simplicity. Effect handlers replace
   async/await as the primitive — that's unification, not simplification.

3. **The benchmark is not "can a beginner learn this in a week."** The
   benchmark is "can a researcher build something in Fides that is
   impossible in Rust, Zig, C, or Python." If the answer is yes, Fides
   is succeeding. If the answer is "it's easier but the same things are
   possible" — that's nice but not the goal.

4. **Fides should be the language where you never hit a wall.** In Rust,
   you hit the borrow checker wall. In Go, you hit the "no generics"
   wall (less so now, but the generational trauma remains). In Python,
   you hit the performance wall. In C, you hit the safety wall. In
   Fides, there should be no wall — if you need to drop to bare metal,
   write inline assembly, build a custom scheduler, do compile-time
   code generation, or prove a safety property, the language lets you.

5. **"Easier for devs" is a secondary goal, not the primary one.** The
   primary goal is maximum power across maximum domains. Making it
   easier where possible is good (familiar syntax, good error messages,
   hot reload, LSP). But when there's a tradeoff between power and
   ease, power wins. Always.

**This is why Fides combines regions + effects + capabilities + comptime
+ native codegen.** No other language has all five. Each one unlocks a
domain that other languages can't serve:
- Regions → real-time, embedded, safety-critical (no GC pauses)
- Effects → research, AI agents, custom concurrency (no walls)
- Capabilities → security, supply chain, sandboxed plugins (no ambient
  authority)
- Comptime → metaprogramming, code generation, reflection (no macros)
- Native codegen → hardware, IoT, bare metal (no runtime, no LLVM)

**The north star: if a researcher, an AI engineer, an embedded
developer, a hardware engineer, and a systems programmer can all do
things in Fides that they cannot do in their current language of choice
— Fides has won. Not by being easier. By being more powerful.**

---

## Issue 60: Audit — p_vec_set Clamps in Unsafe Mode

**Status:** Fixed — unsafe mode does raw access without clamp

Same bug pattern as Issue 31. `p_vec_set` in the interpreter clamped the
index after the panic check. In unsafe mode (where the panic is skipped),
this silently wrote to the wrong element instead of the requested index.

**Fix:** `src/interp.rs` — unsafe mode now does raw access without clamp,
matching the Issue 31 fix for `p_vec_get`.

---

## Issue 61: Audit — p_shared_alloc / p_allocator_alloc Accept Negative Sizes

**Status:** Fixed — returns 0 for negative sizes

Same bug pattern as Issue 32. `p_shared_alloc(size)` and
`p_allocator_alloc(alloc, size)` in C codegen cast `int64_t` directly to
`size_t` without checking for negative values. A negative size would
become a huge allocation request.

**Fix:** `src/codegen_c.rs` — both functions now return 0 for negative sizes.

---

## Issue 62: Audit — p_file_size Doesn't Check ftell for Failure

**Status:** Fixed — returns -1 if ftell < 0

Same bug pattern as Issue 33. `p_file_size` used `ftell` without checking
for -1 return (failure on non-seekable files like pipes).

**Fix:** `src/codegen_c.rs` — returns -1 if `ftell` returns a negative value.

---

## Issue 63: Audit — TxnEnd / RegionEnd Emitted After Terminator

**Status:** Fixed — skipped when block already has terminator

Same bug pattern as Issue 48. `TxnEnd` and `RegionEnd` were unconditionally
emitted after the block body, even if the body ended with a `return`. The
IR verifier rejects instructions after a terminator.

**Fix:** `src/lower.rs` — both `TxnEnd` and `RegionEnd` are now skipped
when `current_block_has_terminator()` returns true.

---

## Issue 64: Audit — val_vars Not Scope-Aware

**Status:** Fixed — per-scope tracking with cleanup on pop

The `val_vars` set used by Issue 46 was a flat `HashSet<String>` that never
cleaned up on `pop_scope()`. A `val x` declared in an inner scope would
leak to outer scopes, causing false warnings when a `var x` was later
declared in an outer scope.

**Fix:** `src/typechecker.rs` — `val_vars` is now `Vec<HashSet<String>>`
(per-scope), pushed in `push_scope()` and popped in `pop_scope()`.

---

## Issue 65: Audit — RwLock TOCTOU Race Condition

**Status:** Fixed — uses atomic increment/decrement

`sync_rwlock_read_lock` and `sync_rwlock_read_unlock` in `stdlib/sync.fd`
used `p_atomic_load` + `p_atomic_store` for the reader count. This is a
non-atomic load-modify-store sequence — two concurrent readers could both
read `rc = 0`, both increment to 1, and both keep the lock (or both release).

**Fix:** `stdlib/sync.fd` — replaced load+store with `p_atomic_add(addr, 1)`
which returns the old value atomically. The lock is kept if old value was 0
(first reader), released otherwise.

---

## Issue 66: Audit — UAF Region Tracking Not Re-Entrant Safe

**Status:** Fixed — unique region keys (name#depth)

`region_addresses` in the interpreter used `entry().or_insert_with()` which
returned the same `HashSet` for re-entrant calls with the same region name
(e.g., recursive functions with region blocks). The second `RegionEnd`
would mark addresses from both invocations as freed.

**Fix:** `src/interp.rs` — region keys now include depth (`name#depth`),
ensuring each region invocation gets its own address set. `run_function`
saves/restores `region_stack` depth to handle early returns.

---

## Issue 67: PE Emitter — `val` Inside Loop Corrupts Loop Variable

**Status:** Open — critical PE emitter bug, self-hosting blocker

**The problem:** When a `val` declaration appears inside a `while` loop body,
the PE emitter allocates the `val`'s stack slot at the same offset as the
loop variable, corrupting it. The loop variable gets reset to its initial
value on every iteration, creating an infinite loop.

**Reproduction:**
```fides
var i: i64 = 0;
while i < len {
    val ch = str_byte_at(s, i);  // This val corrupts `i`
    print(i);  // Always prints 0
    i = i + 1;  // Never takes effect
}
```

**Workaround:** Use `var` instead of `val` inside loop bodies.

**Root cause:** Likely a stack slot allocation bug in the PE emitter's
lowering pass — `val` and `var` declarations in the same scope get
overlapping stack slots. The interpreter and C codegen handle this
correctly because they use separate allocation strategies.

**Impact:** Critical for self-hosting — a self-hosted compiler will have
many `val` declarations inside loops (token scanning, AST traversal, etc.).

---

## Issue 68: PE Emitter — `str_split` Returns Empty Vec

**Status:** Open — critical PE emitter bug, self-hosting blocker

**The problem:** `str_split(s, "\n")` returns an empty Vec in the PE
emitter, even when the input string contains newlines. The interpreter
and C codegen handle this correctly.

**Reproduction:**
```fides
val lines = str_split("a\nb\nc", "\n");
print(lines.len());  // PE: 0, Interp: 3, C: 3
```

**Impact:** Critical for self-hosting — a self-hosted compiler needs
`str_split` for parsing (line-by-line tokenization, splitting on
delimiters, etc.).

---

## Issue 69: PE Emitter — Recursive `Box<T>` Causes Segfault

**Status:** Open — critical PE emitter bug, self-hosting blocker

**The problem:** Programs using recursive enum types with `Box<T>` payloads
(e.g., `enum Expr { Add(Box<Expr>, Box<Expr>) }`) segfault in the PE
emitter. The interpreter and C codegen handle this correctly.

**Reproduction:**
```fides
enum Expr { Num(i64), Add(Box<Expr>, Box<Expr>) }
fn eval(e: Box<Expr>) -> i64 {
    match e {
        Expr::Add(a, b) => { return eval(a) + eval(b); }
        Expr::Num(n) => { return n; }
    }
}
```

**Note:** `test_recursive_box.fd` passes on PE emitter, but more complex
usage (with method calls, multiple recursion levels, or string operations)
segfaults. The issue may be in stack frame sizing or Box pointer handling.

**Impact:** Critical for self-hosting — a self-hosted compiler's AST is
inherently recursive (`Box<Expr>`, `Box<Stmt>`, etc.).

---

## Roadmap Audit: Useless, Redundant, and Valuable Items

> Analysis of all not-done items across `roadmap.md`, `features-roadmap.md`,
> `llvm-roadmap.md`, `roadmap-limitations.md`, and `security.md`.
> Each item is classified as USELESS (cut it), REDUNDANT (already covered),
> SHOULD BE STDLIB (not a compiler feature), or VALUABLE (do it).

---

### Category 1: COMPLETELY USELESS — Cut These

These items are either acknowledged as low-ROI in the roadmap itself, or
are so niche/complex that they waste time that could go to powerful features.

| Item | Source | Why It's Useless |
|------|--------|------------------|
| **Graph coloring register allocator** | roadmap.md Phase 4 | Linear scan works. Roadmap already says "good enough." Graph coloring is months of work for ~2% perf. The existing effect-aware regalloc (F-1D) is more innovative and already done. |
| **DWARF debug info emission** | roadmap.md Phase 5, 14 | Huge effort, zero power gain. Interpreter debugger + ptrace native debugger + stack traces already work. Roadmap already lists this under "What NOT to Focus On." External debugger (gdb) support is nice-to-have, not a differentiator. |
| **@lowpower (F31)** | features-roadmap.md | "Compiler inserts sleep, gates clocks" — this is hardware-specific OS/driver code. A compiler doesn't know about clock trees. This belongs in an IoT SDK, not the language. |
| **@watchdog (F35)** | features-roadmap.md | "Auto-inserts watchdog kicks" — this is a runtime pattern, not a compile-time guarantee. Use `defer watchdog_kick();` or a region with timeout. Not a language feature. |
| **@ota (F37)** | features-roadmap.md | "Compiler generates binary diff for OTA" — this is a build tool / linker feature, not a language annotation. Implement as `fidesc build --ota-diff <old> <new>`. |
| **@handler (F36)** | features-roadmap.md | "Auto-generates routing + OpenAPI docs" — this is a web framework concern. Build it in stdlib/http_server.fd. Not a compiler annotation. |
| **@serverless (F32)** | features-roadmap.md | "Tree-shake, pre-init, WASM output" — this is a build mode (`fidesc build --target wasm --strip`), not an annotation. |
| **@emit (F34)** | features-roadmap.md | "Blockchain event emission with signature verification" — this is a stdlib function (`emit_event(name, data)`), not a compiler annotation. |
| **@calibratable (F55)** | features-roadmap.md | "Marks tunable parameters, generates calibration data format" — this is a tooling/build-system concern. Use `@comptime` + a config file parser. Not a compiler annotation. |
| **@redundant (F45)** | features-roadmap.md | "Dual-channel redundant execution + comparison" — requires physically separate hardware. A compiler can't enforce this. This is a system architecture decision, not a language feature. |
| **@partitioned (F47)** | features-roadmap.md | "Spatial+temporal partitioning between components" — this is an OS/hypervisor concern (ARINC 653, separation kernels). A compiler can't enforce partitioning without an OS. |
| **S25: stdlib/ada.fd** | features-roadmap.md | "Ada-style safety patterns: ranged types, invariant checking" — Fides already has BETTER safety patterns than Ada. This is backwards-looking. Don't copy a weaker system. |

**Verdict: 12 items to cut.** These waste time on things that are either
impossible for a compiler to do, belong in other layers (OS, build tools,
hardware drivers), or are already acknowledged as not worth doing.

---

### Category 2: REDUNDANT — Already Covered by Existing Features

These items are already implemented under different names or are
compositions of existing features. Implementing them separately would
violate "One Way to Do Things" and bloat the annotation surface.

| Item | Source | What Already Covers It |
|------|--------|----------------------|
| **@no-alias (F61)** | features-roadmap.md | "Proves no two mutable refs to same data" — this is literally the borrow checker (Phase 7.55, COMPLETE). Redundant. |
| **@thread-safe (F85)** | features-roadmap.md | "Proves no data races, message passing only" — @send/@sync (Phase 7.55) already does this. Redundant. |
| **@race-free (F87)** | features-roadmap.md | "Verifies all shared access is synchronized" — same as @sync. Redundant. |
| **message passing only (F86)** | features-roadmap.md | "No shared mutable state between threads" — borrow checker + @send already enforce this. Redundant. |
| **@isolated (F49)** | features-roadmap.md | "No shared mutable state between components" — @send + @sync + borrow checker. Redundant. |
| **@bounded (F46)** | features-roadmap.md | "Proves all loops terminate" — @no-unbounded (done) + @gas (done). Redundant. |
| **@escaped (F69)** | features-roadmap.md | "Verifies output is escaped before rendering" — typed strings (F67) already do this. SQLString, HTMLString etc. are distinct types. Redundant. |
| **@schema-validated (F83)** | features-roadmap.md | "Verifies parser against schema" — comptime can do this. Not a compiler annotation. |
| **no auto-deserialize (F82)** | features-roadmap.md | "All parsing explicit + schema-validated" — Fides has no auto-deserialization by design. This is a non-feature. |
| **no runtime reflection (F84)** | features-roadmap.md | "Reflection is comptime only" — Fides already has no runtime reflection. This is a non-feature. |
| **F66: money overflow protection** | features-roadmap.md | "Money type overflow = transaction abort" — checked arithmetic is ALWAYS ON (F64). Money is i64. Overflow = panic. Already done. |
| **@capability-scope (F93)** | features-roadmap.md | "Capabilities can't be escalated at runtime" — already true by construction. Capabilities are compile-time. Non-feature. |
| **@reproducible (F51)** | features-roadmap.md | "Bit-identical output across runs/builds" — @deterministic (done) covers runs. Deterministic builds are a build-system flag, not an annotation. Partially redundant. |
| **@grid-safe (F59)** | features-roadmap.md | "IEC 61850: bounded response, no blocking, deterministic dispatch" — this is @realtime + @no-alloc + effects. Already composable. Redundant. |
| **Green threads (Phase 13.1)** | roadmap.md | Should be replaced by effect handler unification (see Concurrency Decision above). Green threads = another concurrency model = violates "one way to do things." |

**Verdict: 15 items redundant.** Each is already covered by an existing
feature. Adding them would create annotation bloat (the C++/Scala/Ada
trap) and confuse users about which annotation to use.

---

### Category 3: SHOULD BE STDLIB — Not a Compiler Feature

These are useful functionality but belong in `.fd` files, not the
compiler. The test: "Can a user build this in Fides itself?" → Yes.

| Item | Source | Why It's Stdlib |
|------|--------|----------------|
| **@can-bus (F53)** | features-roadmap.md | CAN frame encoding/decoding is a protocol library. No compiler magic needed. → stdlib/can.fd |
| **@diagnostic (F54)** | features-roadmap.md | UDS/OBD diagnostic service handlers are protocol code. → stdlib/can.fd |
| **@arinc (F57)** | features-roadmap.md | ARINC 653 partition scheduling is an OS API. → stdlib/arinc.fd (or a package) |
| **@scada (F60)** | features-roadmap.md | SCADA protocol stubs are protocol code. → stdlib/scada.fd |
| **@crypto-approved (F91)** | features-roadmap.md | "Only vetted algorithms" — this is a lint rule or stdlib choice, not a compiler annotation. Ship stdlib/crypto.fd with only approved algorithms. |
| **@csprng (F89)** | features-roadmap.md | "Only cryptographically secure random" — stdlib/crypto.fd provides `crypto_random()`. Don't add an annotation. |
| **@nonce-unique (F92)** | features-roadmap.md | "Verifies nonce never reused" — can't prove at compile time without tracking all nonce state across runs. This is a runtime check in stdlib/crypto.fd. |
| **@audited-dependency (F81)** | features-roadmap.md | "Dependency must have @audit trail" — this is a package manager (ark) feature, not a language annotation. |
| **@pinned (F79)** | features-roadmap.md | "Dependency pinned with content hash" — ark.lock already does this. Package manager feature. |
| **@sandbox-import (F80)** | features-roadmap.md | "Imported modules sandboxed by default" — capabilities already do this. Just don't pass capabilities to imported modules. Stdlib pattern. |
| **S10: stdlib/consensus.fd** | features-roadmap.md | PoW, PoS, PBFT, Raft — this is a huge library, not a stdlib module. Should be a ark package. |
| **S11: stdlib/storage.fd** | features-roadmap.md | Merkle trees, KV storage — package, not stdlib. |
| **S16: stdlib/ota.fd** | features-roadmap.md | OTA protocol — package, not stdlib. |
| **S18: stdlib/wasm_api.fd** | features-roadmap.md | WASM browser bindings — package, not stdlib. |
| **S20: stdlib/swift.fd** | features-roadmap.md | SWIFT message format — very niche banking protocol. Package. |
| **S22: stdlib/can.fd** | features-roadmap.md | CAN bus — niche automotive. Package. |
| **S23: stdlib/ecu.fd** | features-roadmap.md | ECU abstractions — niche automotive. Package. |
| **S26: stdlib/grid.fd** | features-roadmap.md | IEC 61850 — very niche energy. Package. |
| **S27: stdlib/scada.fd** | features-roadmap.md | SCADA — very niche. Package. |
| **S28: stdlib/medical.fd** | features-roadmap.md | HL7/FHIR — package. |
| **S29: stdlib/clinical.fd** | features-roadmap.md | Clinical calculations — package. |
| **F42: Account<State> types** | features-roadmap.md | State-encoded account types — this is a typestate pattern using existing enums + traits. Build in stdlib/banking.fd. |

**Verdict: 22 items should be stdlib or packages, not compiler features.**
The language stays small; the ecosystem grows. This is the Rust/Zig
philosophy that features-roadmap.md already endorses.

---

### Category 4: VALUABLE — Actually Need Compiler Support

These are items that genuinely need the compiler to do something a
library can't. These should be prioritized.

| Item | Source | Why It Needs the Compiler | Priority |
|------|--------|--------------------------|----------|
| **@differentiable (F14)** | roadmap.md Phase 7 | Compiler traces IR for autograd. ✅ Done — tape-based reverse-mode AD in interpreter. | **DONE** |
| **@realtime (F27)** | features-roadmap.md | Compiler analyzes WCET, rejects malloc/blocking IO. **JUST IMPLEMENTED** (typechecker.rs diff visible). | **DONE** |
| **@interrupt (F29)** | features-roadmap.md | Compiler generates ISR code + vector table. Needs codegen support. Critical for bare-metal/IoT. | **HIGH** |
| **@firmware (F30)** | features-roadmap.md | Strips OS deps, bare metal startup. Needs linker/codegen support. | **HIGH** |
| **@shader (F21)** | features-roadmap.md | Compiler emits SPIR-V for GPU. Fides already has SPIR-V codegen — just needs the annotation to trigger it. | **MEDIUM** |
| **@simd (F23)** | features-roadmap.md | Compiler vectorizes loops to SSE/AVX/NEON. Needs IR transformation. | **MEDIUM** |
| **@pool (F25)** | features-roadmap.md | Compiler manages pool memory layout, zero alloc. Needs IR/layout support. | **MEDIUM** |
| **@secret (F74)** | features-roadmap.md | Information flow control — secrets can't be printed/logged/sent. Extends @tainted. Needs typechecker data-flow tracking. | **HIGH** |
| **@cfi (F75)** | features-roadmap.md | Control flow integrity: shadow stack, CFI checks. Needs codegen support. | **MEDIUM** |
| **@zeroize (F88)** | features-roadmap.md | Secrets wiped from memory on scope exit. Needs compiler to insert memset after last use (like defer but automatic). | **MEDIUM** |
| **@bit-exact (F58)** | features-roadmap.md | Bit-identical FP results across hardware. Needs compiler to disable FMA, enforce strict FP operation order. | **MEDIUM** |
| **parallel { \| } (F13)** | features-roadmap.md | Fork-join parallelism. Compiler emits thread spawn/join. Can't be done in stdlib without spawning primitives. | **HIGH** |
| **@atomic-check (F70)** | features-roadmap.md | TOCTOU prevention — check and act are one atomic operation. Needs compiler to fuse check + act into atomic sequence. | **MEDIUM** |
| **@no-branch-on-secret (F73)** | features-roadmap.md | Reject branches based on secret-typed data. Side-channel defense. Needs typechecker + @secret. | **MEDIUM** |
| **@signed (F50)** | features-roadmap.md | Cryptographic signatures on state transitions. Needs compiler + runtime support for state transition tracking. | **LOW** |
| **Shadow stack (F78)** | features-roadmap.md | Hardware shadow stack where supported (ARM64, x86-64 CET). Needs codegen support. | **LOW** |
| **vtable protection (F76)** | features-roadmap.md | Vtables read-only, pointer-signed. Needs codegen + linker support. | **LOW** |
| **LSP rename** | roadmap.md Phase 11 | Needs compiler symbol table for cross-file rename. | **MEDIUM** |
| **Profiler** | roadmap.md Phase 11 | Needs compiler instrumentation for per-function timing. | **MEDIUM** |
| **State machine transform** | roadmap.md Phase 7.6 | Stackless coroutines — needs IR transformation. But: should be replaced by effect handler lowering (see Concurrency Decision). | **RECONSIDER** |

**Verdict: ~20 items are genuinely valuable.** Of these, the highest
priority for "most powerful language" are:

1. **@differentiable** — without it, the AI/ML claim is hollow
2. **@interrupt + @firmware** — without these, the bare-metal claim is incomplete
3. **@secret + @no-branch-on-secret** — without these, the security claim has a gap
4. **parallel { | }** — without it, no easy parallelism for researchers
5. **@shader + @simd** — without these, the GPU/SIMD claim is incomplete

---

### Category 5: CONTRADICTIONS IN THE ROADMAP

**All previously identified contradictions have been resolved** as of Aug 2026.
The roadmap files are now synchronized:
- `roadmap.md` is the canonical source for phase status and test baselines
- `features-roadmap.md` is the canonical source for F1-F65 feature specs
- `security.md` is the canonical source for security annotation details
- `roadmap-limitations.md` references `roadmap.md` instead of duplicating
- `llvm-roadmap.md` references `roadmap.md` for self-hosting status

If new contradictions are found, add them here.

---

### Category 6: STDLIB THAT'S ACTUALLY USEFUL (Keep)

These stdlib modules serve broad audiences. Status updated Aug 2026.

| Module | Who Needs It | Priority | Status |
|--------|-------------|----------|--------|
| **S9: stdlib/crypto.fd** | blockchain, medical, security, any encryption | **HIGH** | ✅ DONE — SHA-256, HMAC, nonce tracking |
| **S12: stdlib/hal.fd** | robotics, IoT, electronics (GPIO, I2C, SPI, UART) | **HIGH** | 🔲 PLANNED |
| **S13: stdlib/sensor.fd** | robotics, IoT | **HIGH** | 🔲 PLANNED |
| **S14: stdlib/motor.fd** | robotics | **HIGH** | 🔲 PLANNED |
| **S15: stdlib/protocol.fd** | IoT (MQTT, CoAP, BLE, LoRaWAN) | **MEDIUM** | 🔲 PLANNED |
| **S17: stdlib/http_server.fd** | serverless, web, any network service | **HIGH** | 🔲 PLANNED (http.fd has basic server) |
| **S19: stdlib/banking.fd** | finance | **MEDIUM** | ✅ DONE — Account, typestate pattern |

---

### Summary: The Cleanup

```
USELESS (cut):           12 items  → saves ~6 months of wasted work
REDUNDANT (already done): 15 items  → saves ~4 months of duplicate work
SHOULD BE STDLIB:        22 items  → saves ~8 months of compiler complexity
VALUABLE (do it):        ~20 items → ~10 months of high-impact work
STDLIB (keep):            7 items  → ~3 months of ecosystem work
CONTRADICTIONS:           7 items  → docs cleanup, ~1 day
```

**Net effect:** Cutting 49 items (12 useless + 15 redundant + 22 stdlib)
frees up ~18 months of developer time. That time should go to the ~20
valuable items + 7 stdlib modules.

**The principle:** Fides is the most powerful language, not the most
feature-heavy language. 50 annotations that overlap and confuse is
weaker than 15 annotations that are orthogonal and compose. Every
annotation should represent a guarantee the compiler uniquely provides
that no other language can. If it can be a stdlib function, it's a
stdlib function. If it's already covered by an existing feature, it's
not a new feature.

---

## How to Use This File

When you encounter an issue or have a design debate, add it here with:
1. **Status:** Open / Decided / Resolved
2. **The problem:** What's wrong or what's being debated
3. **Options:** What are the choices
4. **Decision:** What was decided (if decided)
5. **Rationale:** Why

This file is the record of *why* decisions were made, not *what* was
implemented (that's the changelog). Future-you will thank present-you
for writing down the reasoning.

---

## Issue 70: Stale Skip/Known-Fail Lists Hiding 41 Working Tests

**Status:** Resolved (Aug 27, 2026)

**The problem:** The skip list in `src/main.rs` had grown to 42 entries over
many phases, and the known-fail list had 10 entries. A systematic audit
revealed that **37 skip-list tests and 4 known-fail tests actually pass on
all 3 backends** (interpreter, C codegen, PE emitter). They were added when
features weren't done yet, the features got fixed, but nobody removed the
tests from the lists. This made the project look smaller and less healthy
than it actually was — the real test count was 147, not 119.

**Options:**
1. Leave the lists as-is (status quo, hides working tests)
2. Remove only the skip list, keep known-fail as-is
3. Audit every entry, promote passing tests, keep only real failures

**Decision:** Option 3 — full audit.

**Rationale:** The skip/known-fail lists are supposed to track *real* failures.
If they accumulate stale entries, they lose their value as a tracking tool.
A full audit is the only way to know which entries are real and which are
stale. The cost is low (run each test on each backend) and the benefit is
high (accurate picture of project health).

**Result:**
- Skip list: 42 → 5 → 4 (after Issue 71 runtime effects fix removed 3 more)
- Known-fail: 10 → 6 (3 parser bugs + 3 C codegen mismatches)
- Normal tests: 119 → 147 → 149 (41 promoted + 3 bonus passes from Issue 71)
- All docs updated with accurate counts

---

## Issue 71: Optimizer DCE Removing Side-Effecting Runtime Calls

**Status:** Resolved (Aug 27, 2026)

**The problem:** When `--opt` was enabled (the default for `build`), calls to
`Vec_push` and other side-effecting runtime functions (`p_vec_push`,
`p_vec_set`, `p_vec_clear`, `p_map_insert`, etc.) were silently removed by
the optimizer's dead-call elimination pass (`effect_optimize_function` in
`src/opt.rs`). This caused `Vec<i64>` push operations to appear to do nothing
— `v.len()` would return 0 after pushes, and `v.get(0)` would return 0.

**Root cause:** The lowerer (`src/lower.rs`) set `EffectSet::new()` (empty)
on calls to runtime builtin functions (`p_*` functions) because:
1. Only `p_print*` and `__pledge*` prefixed calls got `Effect::IO`
2. `p_vec_push` etc. are extern functions, not in `function_effects` (which
   only tracks user-defined functions)
3. With empty effects, `Vec_push` (which calls `p_vec_push`) was marked
   `is_pure = true` by the effects computation
4. The `effect_optimize_function` pass removed all calls to pure functions
   whose results are unused — `Vec_push` returns `void`, so all calls were
   removed as "dead pure calls"

**Fix:** Added `runtime_effects()` helper in `src/lower.rs` that returns
appropriate `EffectSet` for known runtime builtins:
- **Write** effect for mutating ops (`p_vec_push`, `p_vec_set`, `p_vec_clear`,
  `p_map_insert`, `p_atomic_*`, `p_mutex_*`, `p_rc_*`, etc.)
- **Alloc** effect for allocating ops (`p_vec_new`, `p_malloc`, `p_str_concat`,
  `p_tensor_*`, etc.)
- **IO** effect for I/O ops (`p_print*`, `p_panic`, `p_chan_*`, `p_file_*`,
  `p_socket_*`, `p_dl_*`, comm ops, etc.)

This helper is called in both SRET and non-SRET call lowering paths.

**Also fixed:**
- Added `p_vec_*` extern function declarations (23 functions) to the lowerer
  so the codegen uses the correct extern calling convention (no env_ptr)
- Added `GradBegin`/`GradEnd`/`Backward` match arms to 6 backend files
  (`codegen_c.rs`, `interp.rs`, `pe.rs`, `pir.rs`, `wasm.rs`, `x86.rs`)
- Removed debug prints from `stdlib/core.fd` `Vec_push`

**Impact:**
- `Vec<i64>` push/pop/get works correctly with `--opt` (the default)
- 3 bonus passes: `test_iter`, `test_vec_math`, `test_parallel_concurrent`
  now pass on interpreter and/or PE emitter (were known-fail)
- `test_shard`, `test_autodiff_grad`, `test_ml_full` removed from C codegen
  skip list (now pass or have minor formatting differences only)
- Skip list reduced from 7 to 4 tests
- Normal tests: 146 → 153 (5 bonus passes on interp/PE)
- 164/165 normal tests pass on interpreter + C codegen, 165/165 on PE, 265/265 cargo tests pass
