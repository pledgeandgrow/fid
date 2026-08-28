# AGENTS.md — Fides Compiler Working Guide

> This file gives AI assistants (and humans) the context needed to work on the
> Fides compiler safely. Read this before making changes.

## Build & Test Commands

```bash
# Build (release — what verify.sh uses)
cargo build --release

# Build (debug — faster, used by cargo test)
cargo build

# Run all tests across all backends
./target/release/fidesc test

# Full verification (build + test + compare to baseline)
./verify.sh

# Save current test results as new baseline (only after intentional changes)
./verify.sh --save

# Quick build check (no tests)
./verify.sh --quick

# Generate golden files for intermediate compiler stages
./scripts/gen_golden.sh

# Bisect which layer broke a test
./scripts/bisect.sh <test_name>
# Example: ./scripts/bisect.sh strings

# Run a single test through the interpreter
./target/release/fidesc run tests/<name>.fd

# Show IR for a test
./target/release/fidesc ir tests/<name>.fd

# Show PIR (L1/L2/L3) for a test
./target/release/fidesc pir tests/<name>.fd 3

# Show assembly (x86-64)
./target/release/fidesc codegen tests/<name>.fd --target x86-64

# Show assembly with peephole (what `build` actually uses)
./target/release/fidesc codegen tests/<name>.fd --target x86-64 --peephole

# Run the LSP server (for editor integration)
./target/release/fidesc lsp

# Run lint checks (unused functions, unused imports)
./target/release/fidesc lint <file.fd>

# Benchmark a file (interpreter or C native)
./target/release/fidesc bench <file.fd> [-n <count>] [--ccompile]

# Profile a file (per-function timing)
./target/release/fidesc profile <file.fd>

# Format a Fides source file
./target/release/fidesc fmt <file.fd>

# Parse a C header and generate Fides FFI bindings
./target/release/fidesc parse-header <header.h>

# Debug a Fides program (interactive debugger)
./target/release/fidesc debug <file.fd>

# Replay a recorded execution trace
./target/release/fidesc replay <trace.fdtrace>

# Debug a native binary (ptrace-based, Linux only)
./target/release/fidesc autopsy <file.fd>

# Run with post-mortem trace saving (auto-saves trace on panic)
./target/release/fidesc run <file.fd> --post-mortem

# Package manager (dependencies, install, lock files)
./target/release/pacte init [name]
./target/release/pacte add <dep> --path <path> | --git <url>
./target/release/pacte install
./target/release/pacte list

# Documentation generator (HTML or Markdown from .fd source)
./target/release/scribe <file.fd> [--format html|markdown] [--output <dir>]
./target/release/scribe <dir>   [--format html|markdown] [--output <dir>]

# Code coverage (function, line, and instruction coverage)
./target/release/veil <file.fd> [--format text|html] [--output <dir>]
```

## Current Baseline (as of Aug 2026)

```
Unit tests:   265/265 passed  (All passing — cargo test --lib)
Harness:      8/8 passed      (All passing — all 3 self-host blockers pass on all backends)
Interpreter:  153/153 normal  (+ 35 expected-fail, + 0 skip, + 6 known-fail; 2 bonus pass)
C Codegen:    153/153 matched (0 skip — all tests pass on C codegen)
PE Emitter:   153/153 valid   (3 bonus pass)
Total .fd:    218 test files  (195 in root; 23 in subdirs: capability/effects/regions/safety)
Skip list:    0 tests         (all previously skipped tests now pass)
Expected-fail: 35 tests       (tests that verify compiler error detection — failing is correct)
Known-fail:   6 tests         (3 parser bugs: game_stdlib, time, time_edge + 3 C codegen mismatches: iter, vec_math, parallel_concurrent. Interp bonus-passes: iter, vec_math. PE bonus-passes: iter, vec_math, parallel_concurrent.)
Source:       54 .rs files, ~85,493 LOC
Stdlib:       69 .fd modules, 17,567 LOC
Docs:         22 docs/ + 8 roadmap/ + 3 root = 33 .md files
Binaries:     4 (fidesc, pacte, scribe, veil)
Warnings:     225 (dead code, unused variables — cleanup needed)
```

### New Features (Runtime Effects Fix — Optimizer DCE Bug)

- **Runtime effects for `p_*` builtins** (`src/lower.rs`): Added
  `runtime_effects()` helper that returns proper `EffectSet` for known
  runtime builtin functions. Mutating ops (`p_vec_push`, `p_vec_set`,
  `p_map_insert`, `p_atomic_*`, etc.) get `Effect::Write`; allocating ops
  (`p_vec_new`, `p_malloc`, `p_str_concat`, `p_tensor_*`, etc.) get
  `Effect::Alloc`; I/O ops (`p_print*`, `p_chan_*`, `p_file_*`, etc.) get
  `Effect::IO`. Without this, the optimizer's `effect_optimize_function`
  pass treated `Vec_push` (which calls `p_vec_push`) as pure and DCE'd all
  its call sites — causing `Vec<i64>` push to silently do nothing.
- **`p_vec_*` extern declarations** (`src/lower.rs`): Added 23 `p_vec_*`
  extern function declarations so the codegen uses the correct extern
  calling convention (no env_ptr) instead of treating them as Fides
  user functions.
- **`GradBegin`/`GradEnd`/`Backward` match arms** (6 files): Added
  non-exhaustive pattern fix for ML3 autodiff IR instructions in
  `codegen_c.rs`, `interp.rs`, `pe.rs`, `pir.rs`, `wasm.rs`, `x86.rs`.
- **Impact:** `Vec<i64>` push/pop/get works with `--opt` (default). 3
  bonus passes (test_iter, test_vec_math, test_parallel_concurrent).
  Skip list reduced from 7 to 4. 149/149 normal tests pass on all 3
  backends.

### New Features (Issue 45-49: val/var, auto-deref, exhaustiveness, UAF, closures)

- **Issue 46/47: `val` immutability enforcement** (`src/typechecker.rs`): The
  typechecker now enforces that `val` variables cannot be reassigned. Fixed
  247 `val`→`var` conversions in stdlib and test files where variables were
  declared `val` but later reassigned (loop counters, accumulators, etc.).
  Automated fix script at `/tmp/fix_val_var2.py` finds assignments anywhere
  in lines (not just line-start) using regex for compound assignments
  (`+=`, `-=`, etc.) and simple `=` (excluding `==`, `!=`, `<=`, `>=`).
- **Issue 45: `&mut T` auto-deref for method calls** (`src/typechecker.rs` +
  `src/lower.rs`): Methods on `&mut T` and `&T` now auto-deref to the inner
  type. The typechecker unwraps `Type::Ref { inner, .. }` before method
  lookup. The lowerer loads through `Ptr(Ptr(...))` references before
  dispatching. Test: `test_refs_mut_methods.fd` — `c.increment()` on
  `&mut Counter` works without explicit deref.
- **Issue 49: Non-exhaustive match checking** (`src/typechecker.rs`): Both
  `Stmt::Match` and `Expr::Match` now check exhaustiveness. For enums, all
  variants must be covered (or a wildcard/ident catch-all). For bools, both
  `true` and `false` must be covered. Emits a **warning** (not error) to
  avoid breaking existing partial matches. Test: `test_match_exhaustive.fd`.
- **Interpreter UAF detection** (`src/interp.rs`): `RegionEnd` now marks all
  addresses allocated in that region as freed. `Load`/`Store` check the
  `freed_addresses` set and panic in debug mode if accessing freed memory.
  `RegionBegin` pushes to a `region_stack`; `Alloca` records addresses in
  the current region. This catches use-after-free bugs at interpreter time.
- **Closures in borrow checker** (`src/borrowck.rs`): `Expr::Closure` is now
  handled — the body is checked in a new scope. Closure params are local to
  the closure scope (no borrow tracking needed for params).
- **RwLock + AtomicU64 stdlib** (`stdlib/sync.fd`): Added `sync_rwlock_*`
  functions (create, read_lock/unlock, write_lock/unlock) and `atomic_u64_*`
  wrapper functions (new, load, store, fetch_add, cas).
- **Drop trait + compiler-inserted drops** (`src/lower.rs` + `src/ir.rs`):
  When a struct has `impl T { fn drop(self: &mut T) }`, the compiler
  automatically calls `T_drop` at every function exit point (return, implicit
  return, `?` error propagation). `IrFunction.drop_locals` tracks
  `(alloca_ptr, struct_name)` pairs for Drop-eligible locals. `emit_drops()`
  emits calls in reverse declaration order (LIFO, matching Rust). The drop
  call includes the dummy env_ptr argument expected by user functions.
  Test: `test_drop.fd` — `Logger` with `drop` method, "Logger dropped"
  printed at function exit on all 3 backends.

### Audit Fixes (Same-Pattern Bugs Found During Code Review)

- **`p_vec_set` unsafe clamp** (`src/interp.rs`): Same bug as Issue 31 —
  clamped index after panic check, silently writing to wrong element in
  unsafe mode. Fixed: unsafe mode does raw access.
- **`p_shared_alloc` / `p_allocator_alloc` negative size** (`src/codegen_c.rs`):
  Same bug as Issue 32. Fixed: returns 0 for negative sizes.
- **`p_file_size` ftell failure** (`src/codegen_c.rs`): Same bug as Issue 33.
  Fixed: returns -1 if ftell < 0.
- **`TxnEnd` / `RegionEnd` after terminator** (`src/lower.rs`): Same bug as
  Issue 48. Fixed: skipped when block already has a terminator.
- **`val_vars` scope leak** (`src/typechecker.rs`): `val_vars` was a flat
  `HashSet` that never cleaned up on `pop_scope()`. Fixed: now per-scope
  (`Vec<HashSet<String>>`), cleaned up on scope pop.
- **`stmt_has_break_or_continue` missing block types** (`src/comptime.rs`):
  Didn't recurse into `RegionBlock`, `Sandbox`, `Txn`, `GradBlock`. Fixed.
- **Interpreter `p_malloc` negative cast** (`src/interp.rs`): Cast negative
  i64 to huge u64. Fixed: returns 0 for negative sizes.
- **RwLock TOCTOU race** (`stdlib/sync.fd`): `read_lock`/`read_unlock` used
  load+store (non-atomic) for reader count. Fixed: uses `p_atomic_add`.
- **UAF re-entrant region collision** (`src/interp.rs`): `region_addresses`
  shared the same set for re-entrant calls with the same region name. Fixed:
  uses unique keys (`name#depth`).
- **Interpreter `region_stack` cleanup** (`src/interp.rs`): Early returns
  inside region blocks left stale entries on `region_stack`. Fixed:
  `run_function` saves/restores `region_stack` depth.

### New Features (String Concat, Transitive Imports, New Stdlib Modules)

- **String concatenation with `+`** (`src/lower.rs`): `str + str` now lowers
  to `p_str_concat` call. Works with literals, variables, and inline function
  call results. `infer_binop_type` returns `Ptr(U8)` for `str + str` (both
  operands must be str — `str + i64` remains pointer arithmetic).
  `lookup_func_ret_ty` falls back to `func_ret_types` for functions not yet
  lowered, fixing type inference for inline calls returning `str`.
- **Transitive `use` imports** (`src/compiler.rs`): `use` imports in stdlib
  modules are automatically resolved. Importing `clinical` (which has
  `use math`) will also load `math`. Worklist algorithm with cycle detection.
- **`p_str_concat` and `p_string_slice`** (`src/interp.rs`): Both now use
  `value_to_string()` which handles `Value::Str`, `Value::Ptr`, and
  `Value::Int` (string pointers materialized from `Value::Str`).
- **C codegen CallPtr void dispatch fix** (`src/codegen_c.rs`): CallPtr
  dispatch now checks each candidate function's actual return type (from
  `fn_ret_types`) instead of the CallPtr's expected type. Aggregate return
  types (structs returned via output pointer) are handled as void to
  prevent "invalid use of void expression" C compile errors.
- **New stdlib modules** (34 new modules, total 69):
  - Industry: `clinical.fd`, `crypto.fd`, `storage.fd`, `ecu.fd`, `scada.fd`,
    `swift.fd`, `grid.fd`, `medical.fd`, `ota.fd`, `arinc.fd`, `can.fd`
  - Data structures: `string.fd`, `fmt.fd`, `bits.fd`, `sort.fd`, `hash.fd`,
    `iter.fd`, `stack.fd`, `queue.fd`, `buffer.fd`, `collections.fd`,
    `result.fd`, `random.fd`, `path.fd`, `units.fd`, `uuid.fd`
  - System: `fs.fd`, `io.fd`, `os.fd`, `sys.fd`
  - ML: `kv_cache.fd`, `quant.fd`, `dp.fd`, `shard.fd`
- **`val` vs `var`**: Loop counters and accumulators MUST use `var`. `val`
  reassignment now emits a warning (Issue 46 fixed). Will be promoted to an
  error after the stdlib is fully migrated to `var` for mutables.
- **Bucket 1 Codegen Fixes** (8 issues fixed, pre-self-host baseline):
  - Issue 20: Stack alignment padding for odd callee-saved count (`codegen_x64.rs`)
  - Issue 31: `p_vec_get` unsafe mode does raw access, no silent clamp (`interp.rs`)
  - Issue 32: `p_malloc` rejects negative sizes, returns NULL (`codegen_c.rs`)
  - Issue 33: `p_file_read` checks `ftell` for failure (`codegen_c.rs`)
  - Issue 44: Comptime loop unrolling skips loops with break/continue (`comptime.rs`)
  - Issue 45: Auto-deref `&T`/`&mut T` for method resolution (`typechecker.rs`)
  - Issue 46: `val` reassignment emits warning, tracked in `val_vars` set (`typechecker.rs`)
  - Issue 48: `UnsafeEnd` skipped when block already has terminator (`lower.rs`)
- **`@deterministic`/`@crashfree`**: Removed from functions using floating-point
  or string operations — these violate the annotation constraints.
- **`Account<State>` typestate** (`stdlib/banking.fd`): Compile-time state
  transition enforcement for account states (Active/Frozen/Closed).

### New Features (ML3/ML8/Tensor: Autodiff, Kernel Specialization, Tensor Fixes)

- **ML3: Tape-Based Reverse-Mode Autodiff** (`src/ir.rs`, `src/typechecker.rs`,
  `src/interp.rs`, `src/autograd.rs`): Added `Diff` effect to the effect system.
  Tensor operations (matmul, add, relu, etc.) produce `Diff` effect.
  `@differentiable` annotation with compile-time verification: no
  IO/Net/Spawn/Diverge (breaks gradient computation). `verify_differentiable()`
  in typechecker. **Runtime autograd engine implemented**: tape-based
  reverse-mode AD in the interpreter. `GradBlock`/`GradEnd`/`Backward` IR
  instructions. `autograd_tape`, `grad_vars`, `autograd_mode` state in
  `Interpreter`. Tensor builtins (`p_tensor_add`, `p_tensor_mul`, `relu`,
  `sigmoid`, etc.) record operations on the tape when autograd mode is
  enabled. `backward()` traverses the tape in reverse, computing gradients
  via local derivative functions. `p_tensor_grad` retrieves computed
  gradients. `p_grad_get`/`p_grad_zero` manage gradient state. Tests:
  `test_autodiff.fd` (pass — typechecker), `test_autodiff_fail.fd` (IO
  violation, expected fail), `test_autodiff_grad.fd` (pass — runtime gradient
  computation verified on interpreter + C codegen). **C codegen autograd
  implemented**: `ag_tape`/`ag_record`/`ag_accum_grad` static data structures
  in generated C. Tensor builtins call `ag_record()` when `ag_mode` is on.
  `p_autograd_backward()` walks tape in reverse. `p_grad_get`/`p_grad_zero`
  manage gradient state. `GradBegin`/`GradEnd`/`Backward` IR instructions
  emit `p_autograd_begin()`/`p_autograd_end()`/`p_autograd_backward(NULL)`.
  `test_ml_full.fd` (pass — all ML features tested together on interpreter + C codegen).
- **ML8: comptime Kernel Specialization** (`src/ir.rs`, `src/lower.rs`,
  `src/pir.rs`, `src/gpu.rs`): `@kernel(x,y,z)` annotation tracked through
  AST→IR→L1→L2→L3. SPIR-V codegen selects `@kernel` function as entry point
  with `OpExecutionMode LocalSize`. PTX codegen emits `.maxntid x,y,z`.
  `comptime gen` generates specialized kernels at compile time. Test:
  `test_kernel_specialization.fd` (pass — @kernel function, comptime name gen).
- **ML9: KV Cache with Region Memory** (`src/ast.rs`, `src/parser.rs`,
  `src/ir.rs`, `src/typechecker.rs`, `src/lower.rs`, `src/interp.rs`,
  `src/tensor.rs`): `region kv_cache r with budget(N) [window(W)] { body }`
  syntax for bounded KV cache regions. `Stmt::RegionBlock` extended with
  `is_kv_cache: bool` and `window_size: Option<u32>`. `Inst::RegionBegin/End`
  extended with `is_kv_cache` and `window_size`. KV cache regions **must**
  have a budget (typechecker rejects without). `KvCache` struct in
  `src/tensor.rs` with append/get/len/clear/evict_oldest. Builtins:
  `p_kv_cache_new/append/get/len/clear/evict`. Stdlib: `stdlib/kv_cache.fd`.
  Tests: `test_kv_cache.fd` (pass), `test_kv_cache_fail.fd` (no budget, fail).
- **ML10: Real Quantization** (`src/typechecker.rs`, `src/interp.rs`,
  `src/tensor.rs`, `src/lower.rs`): `@quantized` annotation verified by
  typechecker (no IO/Net/Spawn/Diverge, no dequantize_q8 calls). Quantization
  builtins: `p_quantize_q8`, `p_dequantize_q8`, `p_qmatmul`,
  `p_find_quant_params`. Symmetric quantization in `src/tensor.rs`.
  Stdlib: `stdlib/quant.fd`. Tests: `test_quantization.fd` (pass),
  `test_quantization_fail.fd` (@quantized with dequantize, fail).
- **ML11: Gradient Checkpointing** (`src/typechecker.rs`, `src/interp.rs`):
  `@checkpoint` annotation verified by typechecker. Must also be
  `@differentiable` and `@deterministic`. Rejects IO/Net/Spawn/Diverge.
  **Runtime checkpointing implemented**: `checkpoint_mode` flag in
  `Interpreter`. When active, `AutogradOp::Relu`/`Sigmoid` store input data
  (not results) on the tape, and `relu_backward`/`sigmoid_backward` recompute
  activations from inputs during the backward pass — trading compute for
  memory. `p_checkpoint_begin`/`p_checkpoint_end` builtins toggle the mode.
  Tests: `test_checkpoint.fd` (pass on interpreter + C codegen),
  `test_checkpoint_fail.fd` (missing @deterministic, fail). **C codegen
  checkpoint implemented**: `ag_checkpoint_mode` flag in generated C.
  `p_checkpoint_begin`/`p_checkpoint_end` toggle the flag. Relu/sigmoid
  store input data (not results) when checkpoint mode is active.
- **ML12: Differential Privacy** (`src/ir.rs`, `src/typechecker.rs`,
  `src/interp.rs`, `src/parser.rs`, `src/lower.rs`): `@dp(epsilon=N)` annotation
  verified by typechecker (must have Noise effect, no IO/Net). New
  `Effect::Noise` in effect system. Noise builtins: `p_dp_noise`,
  `p_gaussian_noise`, `p_dropout`. Parser supports float annotation args.
  **Runtime epsilon budget tracking implemented**: `dp_epsilon_budget` and
  `dp_epsilon_spent` state in `Interpreter`. `p_dp_budget_reset(epsilon)`
  initializes the budget. `p_dp_budget_check(cost, threshold)` accumulates
  spent epsilon and panics if the total exceeds the threshold — enforcing
  the privacy budget at runtime. Stdlib: `stdlib/dp.fd`. Tests:
  `test_dp.fd` (pass on interpreter; C codegen skip — float formatting),
  `test_dp_fail.fd` (no Noise effect, fail). **C codegen DP implemented**:
  `p_dp_noise` adds Gaussian noise via Box-Muller transform. `p_dp_budget_reset`/
  `p_dp_budget_check` track epsilon budget. `p_gaussian_noise`/`p_dropout`
  implemented. (test_dp skipped on C due to %g vs Rust {} float formatting.)
- **ML13: Tensor Parallelism** (`src/ir.rs`, `src/typechecker.rs`,
  `src/interp.rs`, `src/lower.rs`): `@shard` annotation verified by
  typechecker (must have Comm effect, no IO/Net). New `Effect::Comm` in
  effect system. Communication builtins: `p_all_reduce`, `p_broadcast`,
  `p_all_gather`, `p_scatter`, `p_send_tensor`, `p_recv_tensor`.
  **Simulated multi-rank runtime**: `tp_rank` and `tp_world_size` state in
  `Interpreter` (single-rank: rank=0, world_size=1). `p_tp_rank()`/
  `p_tp_world_size()` return the current rank and world size. `p_all_reduce`
  is identity in single-rank mode (returns input unchanged). Stdlib:
  `stdlib/shard.fd`. Tests: `test_shard.fd` (pass on interpreter + C codegen),
  `test_shard_fail.fd` (no Comm effect, fail). **C codegen TP implemented**:
  `p_tp_rank`/`p_tp_world_size` return static values (0/1). `p_all_reduce`/
  `p_broadcast`/`p_all_gather`/`p_scatter` are identity (clone tensor).
  `p_send_tensor` is no-op. `p_recv_tensor` returns zero tensor of given shape.
- **ML14: LoRA/Adapter Hot-Swap** (`src/typechecker.rs`): `@adapter`
  annotation verified by typechecker (no IO/Net/Spawn/Diverge — pure weight
  transformations). Works with `persist` for hot-swap. Test:
  `test_lora.fd` (pass — LoRA forward + persist adapter weights).
- **Tensor Stdlib Fixes** (`src/interp.rs`): Fixed `p_tensor_*` functions to
  handle raw Fides Vecs (i64 pointers to 24-byte memory blocks from
  `stdlib/core.fd`). Added `read_raw_vec_i64()` and `arg_to_vec_i64()`
  helpers. `p_tensor_shape` now returns raw Fides Vec instead of
  `Value::Vec`. `test_tensor.fd`, `test_shape.fd`, `test_shape_verify.fd`
  now pass (removed from skip list).

**153/153 normal tests pass on interpreter + PE, 153/153 on C codegen.** The 3 self-host blockers
(enum payloads, generic Vec<T>, recursive Box<T>) all pass on interpreter,
C codegen, and PE emitter. All stdlib gaps for self-hosting are filled
(str_bytes, str_split, str_parse_hex, file_write_bytes, etc.).

### New Features (Phase 14: Compiler Optimizations — LEA, Alias Analysis, ABI, Incremental)

- **F-4C: LEA Instruction Selection** (`src/codegen_x64.rs`): `gen_field_addr`
  and `gen_elem_addr` now emit `leaq` for offset calculations instead of
  `shlq`+`addq` sequences. Field access uses `leaq off(base), dst`; element
  access uses `leaq (base, idx, scale), dst`. Fewer instructions, same result.
- **I-7A: Borrow-Based Alias Analysis** (`src/ir.rs`, `src/lower.rs`,
  `src/pir.rs`, `src/opt.rs`): Added `BorrowKind` enum (Shared/Mutable/None)
  to `ValueMeta`, propagated through IR→L1→L2 lowering. The alias analysis
  pass in `opt.rs` uses `borrow_kinds` to determine that a shared borrow
  cannot alias a mutable borrow, allowing more store-load forwarding and
  code motion. `borrow_non_aliasing()` helper integrated into Phase 1
  (predecessor replay + main loop) and Phase 2 (cross-region reordering).
- **I-6A: Linear Type Stack Promotion**: Linear types are promoted to stack
  slots, reducing heap allocation pressure.
- **F-4A: x86-64 ABI Compliance** (`src/codegen_x64.rs`): `gen_call_abi`
  handles the System V AMD64 ABI: integer args in RDI/RSI/RDX/RCX/R8/R9,
  float args in XMM0-XMM7, struct args by value, stack alignment to 16
  bytes before calls. `gen_prologue_abi` saves XMM argument registers for
  extern functions. The old `gen_call` was removed as dead code.
- **S-1B: Incremental Compilation** (`src/inc.rs`): In-memory
  `IncrementalCache` stores optimized L2 modules keyed by
  (source_path, source_hash, opt_flags). `build_executable_with_lto()`
  checks the cache before running the full pipeline; on a hit, it skips
  IR lowering, L1→L2 lowering, and L2 optimization. A global cache
  (`global_cache()`) is shared across calls within a process, benefiting
  LSP server, watch mode, and the test harness. 7 unit tests in `inc.rs`.
- **S-1C: Function-Level Parallelism** (`src/opt.rs`): `parallel_functions()`
  helper uses `std::thread::scope` to process function chunks in parallel.
  19 of 22 optimization passes are parallelized (all except `pass_inline`,
  `pass_devirt`, `pass_partial_eval` which need cross-function info). For
  modules with < 4 functions, runs sequentially to avoid thread overhead.
  Also fixed the loop vectorizer cost model to correctly exclude condition
  recomputation and induction variable instructions from the cost calculation.

### New Features (Self-Hosting Blockers — 3 of 5 resolved)

Three blockers preventing the Fides compiler from being rewritten in Fides
have been resolved. All verified on interpreter + C codegen + PE emitter with
zero regressions (153/153 tests pass on interpreter + PE, 153/153 on C codegen).

- **Typed enum payloads** (`src/typechecker.rs` + `src/lower.rs`): Enum
  variant payloads now keep their declared type in `match` bindings instead
  of collapsing to `i32`. `Node::Leaf(t: Token) => t.text` works. Added
  `enum_variants`/`enum_type_params` maps, `pattern_binding_types()` with
  type-param substitution, `pattern_binding_ir_types()` using real IR types
  (aggregates as `Ptr(Struct(..))`). Test: `test_enum_payload_types.fd`.
- **Real generics at value level** (`src/typechecker.rs` + `src/lower.rs` +
  `src/ir.rs`): `Vec<T>`/`HashMap<K,V>` carry actual element types from `let`
  annotations. `val v: Vec<Token> = Vec::new()` works. `Vec::get()` returns
  `Ptr(Struct(..))` for struct elements via `word_repr()`. Added `let_hint`
  threading, `is_ptr_like()` on `IrType`, `Vec<X>`/`HashMap<K,V>` compatibility
  in `types_compatible()`. Test: `test_generic_vec_typed.fd`.
- **Recursive types via Box<T>** (`src/lower.rs`): `Box<T>` works as real
  indirection. `enum Expr { Add(Box<Expr>, Box<Expr>) }` compiles and runs.
  Added `maybe_deref_box()` to match lowerer — loads through Box (either
  `Ptr(Ptr(Struct))` from `Box::new` or `Generic("Box", [Struct])` from typed
  params) before pattern matching. Test: `test_recursive_box.fd` — recursive
  `eval` and `count_nodes` on `(1+2)*(3+4)` = 21 with 7 nodes.

### New Features (Phase 13.5: Documentation & Coverage — scribe + veil)

- **scribe** (`src/scribe.rs` + `src/bin/scribe.rs`): Documentation generator for
  Fides. Parses `.fd` source files, extracts `///` doc comments, and generates
  HTML or Markdown documentation. Supports single files and directories.
  Commands: `scribe <file.fd> [--format html|markdown] [--output <dir>]`.
  Output: styled HTML with sidebar navigation, per-item sections, function
  signatures, parameter tables, struct fields, enum variants, impl methods.
- **veil** (`src/veil.rs` + `src/bin/veil.rs`): Code coverage tool for Fides.
  Instruments the interpreter with a debug hook to track which functions, source
  lines, and IR instructions are executed. Reports coverage as text or HTML.
  Commands: `veil <file.fd> [--format text|html] [--output <dir>]`.
  Output: summary cards (function/line/instruction coverage %), per-function
  breakdown table, source listing with covered/uncovered line markers.
- **Library target** (`src/lib.rs`): Added `[lib]` target to Cargo.toml so that
  `fidesc`, `pacte`, `scribe`, and `veil` binaries share the same codebase via
  `use fidesc::module` imports.

### New Features (Phase 13.4: Package Manager — pacte)

- **pacte** (`src/pacte.rs` + `src/bin/pacte.rs`): Package manager for Fides.
  Commands: `init`, `add`, `install`, `remove`, `list`, `lock`, `update`,
  `search`, `publish`. Manifest: `pacte.toml`. Lock file: `pacte.lock`.
  Modules installed to `pacte_modules/`.
- **Dependency types**: Path dependencies (local files or directories),
  Git dependencies (clone with `--depth 1`, optional `--branch`/`--tag`),
  Registry dependencies (future — not yet implemented).
- **Lock file**: Records resolved commit hashes for git deps, canonical
  paths for path deps. `pacte lock` regenerates from installed modules.
- **Zero external dependencies**: Built-in TOML parser/writer, uses `git`
  CLI for cloning. No Rust crate dependencies added.
- **Build**: `cargo build --release --bin pacte` produces the `pacte` binary.

### New Features (Rc/Arc Smart Pointers)

- **Rc<T> / Arc<T>** (`src/typechecker.rs` + `src/lower.rs` + `src/interp.rs` +
  `src/codegen_c.rs`): Reference-counted smart pointers with manual refcount
  management. Since Fides lacks automatic destructors, the API is explicit:
  - `Rc::new(x)` / `Arc::new(x)` — allocate `[refcount:i64 | value:T]` on the
    heap, set refcount=1, return pointer to value (offset +8).
  - `rc.clone()` / `arc.clone()` — increment refcount, return same pointer.
  - `rc.drop()` / `arc.drop()` — decrement refcount, free if zero.
  - `rc.get()` / `arc.get()` — load the contained value.
  - `rc.count()` / `arc.count()` — read the current refcount.
- **Memory layout**: `[refcount: i64][value: T]` — refcount at `ptr-8`, value
  at `ptr`. The user-facing pointer points to the value, never the header.
- **Runtime functions**: `p_rc_incr(rc_addr, val) -> i64` and
  `p_rc_decr(rc_addr, val) -> void`. Both Rc and Arc use these at the IR level;
  the C codegen implements them with non-atomic ops (Rc) or atomic ops (Arc).
  The interpreter uses `self.memory` (byte-addressable heap) for refcount
  storage. `p_atomic_sub` also added for completeness.
- **Type system**: `Rc<T>` and `Arc<T>` are `Type::Generic` in the typechecker
  and `IrType::Opaque("Rc")` / `IrType::Opaque("Arc")` in the IR. They are
  opaque pointers at runtime — any two instantiations of the same wrapper type
  are compatible (like Vec/HashMap).
- **Test**: `tests/test_rc_arc.fd` — tests basic refcount lifecycle (new,
  clone, drop, get, count) for both Rc and Arc, plus struct values.

### New Features (Phase 8A: Vec/Mat Math Types)

- **vec2/vec3/vec4 types** (`stdlib/math.fd`): First-class vector types with
  methods: add, sub, scale, dot, cross (vec3 only), length, normalize, lerp.
  Constructor functions: `vec2(x,y)`, `vec3(x,y,z)`, `vec4(x,y,z,w)`.
- **mat4 type** (`stdlib/math.fd`): 4x4 matrix with identity, mul,
  transform_vec4, transform_vec3. Construction functions: `translate(vec3)`,
  `mat_scale(scalar)`.
- **quat type** (`stdlib/math.fd`): Quaternion with `from_axis_angle(axis, angle)`
  and `rotate_vec3(v)`.
- **Operator overloading** (`src/typechecker.rs` + `src/lower.rs`): `vec + vec`,
  `vec - vec`, `vec * scalar`, `scalar * vec`, `vec / scalar` all work via
  compiler-rewritten method calls to `vecN_add`, `vecN_sub`, `vecN_scale`.
- **Swizzle access** (`src/typechecker.rs` + `src/lower.rs`): `.xyz`, `.rgb`,
  `.stpq` swizzle patterns on vec types. Supports 2-4 components, reordering
  (`.yzx`), and repetition (`.xxx`). Desugars to field loads + struct
  construction at compile time — zero runtime cost.
- **Free functions** (`stdlib/math.fd`): `radians(degrees)`, `degrees(radians)`,
  `lerp(a, b, t)`, `distance(a, b)`.
- **Test**: `tests/test_vec_math.fd` — now a BONUS PASS on interpreter + PE.

### New Features (Tier 1: Quick Wins)

- **@constant_time annotation** (`src/typechecker.rs`): Security annotation that
  enforces constant-time execution: no early returns, no data-dependent branches,
  no IO/Net/Spawn effects. Fail tests: `test_constant_time_fail`,
  `test_constant_time_fail2`. Working test: `test_constant_time.fd`.
- **Select statement** (`src/ast.rs`, `src/parser.rs`): `select { case ch => x => { body } default => { body } }`
  polls all channels and executes the first ready one. If none ready and no
  default, blocks until one is ready. New `Stmt::Select` variant and `SelectCase` struct.
- **Option/Result combinator methods** (`stdlib/core.fd`): Added `map`, `and_then`,
  `or_else`, `unwrap_or_else`, `expect` to both Option and Result. These accept
  function pointers (`fn(i64) -> i64`) for lazy evaluation.
- **Vec iterator adapters** (`stdlib/vec.fd`): Added `take`, `skip`, `zip`,
  `enumerate`, `flat_map`, `chunk`, `window`, `dedup`, `sort_by`, `contains_val`
  as both Vec methods and free functions (`vec_take`, `vecSkip`, etc.).
- **Channel wrapper in sync.fd** (`stdlib/sync.fd`): Added `sync_channel_create`,
  `sync_channel_send`, `sync_channel_recv`, `sync_channel_try_recv`,
  `sync_channel_close`, `sync_channel_is_closed` wrapper functions and
  `SYNC_CHAN_PENDING` constant. These wrap the existing `p_chan_*` runtime
  functions, providing a CSP-style channel API under the `sync` module.
  Both interpreter and C codegen backends support these.
- **Stack traces on panic** (`src/interp.rs` + `src/codegen_c.rs`): Panics now
  print a call stack trace showing the chain of function calls that led to the
  panic. The interpreter maintains a `call_stack: Vec<String>` that is
  pushed/popped on function entry/exit, and captures the trace at the point of
  panic (before unwinding). C codegen uses a global `p_call_stack[]` array
  with `p_call_stack_push()`/`p_call_stack_pop()` calls instrumented at
  function entry/exit. Example output:
  ```
  Panic: Vec index out of bounds
  stack trace:
      → Vec_get
      ↳ called from inner
      ↳ called from outer
      ↳ called from main
  ```
- **Linter command** (`fidesc lint <file.fd>`): Runs lint checks and reports
  warnings without failing compilation. Currently detects:
  - Unused functions (defined but never called, excluding `main`, `pub`, `@comptime`, and `__`-prefixed)
  - Unused imports (`use` statements where none of the module's functions are called)
  Lint warnings are also generated during normal compilation (stored in `tc.warnings`).
- **Benchmark command** (`fidesc bench <file.fd> [-n <count>] [--ccompile]`):
  Runs a file N times (default 10) and reports timing statistics (mean, median,
  min, max, stddev, total). Supports two modes:
  - Interpreter (default): runs through the Fides interpreter
  - C native (`--ccompile`): compiles to C with gcc -O2, then runs the native binary
  Program output is suppressed during benchmarking; only timing is reported.

### New Features (Security Hardening)

- **Binary hardening (C codegen)**: All C codegen output now compiled with
  `-fstack-protector-strong` (stack canaries), `-fPIE -pie` (ASLR),
  `-Wl,-z,relro -Wl,-z,now` (full RELRO), `-Wl,-z,noexecstack` (NX stack),
  `-fno-strict-overflow`. No opt-in required — all binaries are hardened.
- **@constant_time annotation**: Prevents timing side-channel attacks
  (Spectre, Meltdown, cache timing). Enforces: no early returns, no IO/Net,
  no allocation, no calls to non-@constant_time functions (transitive).
  Implementation: `verify_constant_time()` in typechecker.rs.
- **@tainted annotation**: Marks parameters as untrusted user input. Prevents
  tainted values from flowing into SQL/shell/HTML/path sinks without
  sanitization. Implementation: `verify_tainted()` in typechecker.rs.
- **Param.annotations field**: Added `annotations: Vec<Annotation>` to `Param`
  in ast.rs to support per-parameter annotations like @tainted.
- **New tests**: test_constant_time.fd (pass), test_constant_time_fail.fd
  (expected fail — early return), test_constant_time_fail2.fd (expected fail
  — IO in body).
- **New doc**: docs/security.md — comprehensive security hardening guide.

### New Features (Cross-Compilation Phase)

- **ARM64 cross-compilation** (`--target arm64` or `--target arm64-linux`):
  - `src/arm64_enc.rs` — ARM64 (AArch64) instruction encoder that converts
    assembly text to 4-byte machine code. Supports arithmetic, load/store,
    branches (B, B.cond, BL), ADRP+ADD:lo12, SVC, and more.
  - `src/runtime_arm64.rs` — ARM64 Linux runtime using raw SVC syscalls
    (write=64, exit=93, mmap=222). Provides p_print_str, p_print_int,
    p_print_bool, p_panic, p_malloc, p_to_str, p_str_concat, Vec ops.
  - Linker emits `EM_AARCH64` (183) machine type for ARM64 executables.
  - 17 encoder unit tests + 9 runtime unit tests all pass.

- **RISC-V cross-compilation** (`--target riscv` or `--target riscv64-linux`):
  - `src/riscv_enc.rs` — RV64 instruction encoder supporting R-type, I-type,
    S-type, B-type, J-type, U-type instructions. Includes pseudo-instructions
    (li, mv, neg, not, seqz, snez, j, call, return, bgez, beqz, bnez).
  - `src/runtime_riscv.rs` — RISC-V Linux runtime using raw ECALL syscalls
    (write=64, exit=93, mmap=222). Same function set as ARM64 runtime.
  - Linker emits `EM_RISCV` (243) machine type and handles JAL (type 20)
    and branch (type 21) PC-relative relocations.
  - 14 encoder unit tests + 7 runtime unit tests all pass.

- **Target triple parsing**: `parse_target_triple()` in `main.rs` normalizes
  target strings. Accepted formats: `x86-64`, `arm64`, `aarch64`, `riscv`,
  `riscv64`, `arm64-linux`, `aarch64-linux`, `riscv64-linux`,
  `x86-64-windows`, `x64-windows`, `windows`, etc.

- **Windows cross-compilation** (`--target x86-64-windows`):
  - Uses the existing `src/pe.rs` PE emitter (6,392 lines) which generates
    x86-64 Windows .exe files directly from IR with kernel32.dll imports.
  - Windows x64 calling convention (RCX, RDX, R8, R9), shadow space,
    PE import tables, and entry point handling all built in.
  - Wired into the unified `build` command via `--target x86-64-windows`.

- **Linker target-awareness**: `link_raw_with_runtime_for_target()` function
  links user code + runtime for any target. Relocation types 10-15 for ARM64,
  20-21 for RISC-V.

### New Features (Phase A: Real Async/Concurrency)

- **Executor runtime** (Phase A1): `p_executor_new`, `p_executor_spawn`,
  `p_executor_block_on`, `p_executor_run_all`, `p_task_poll`, `p_task_is_ready`.
  Tasks run as real OS threads. The executor tracks tasks and can join them.
  `p_task_poll` blocks until the task completes (A1). `PENDING_SENTINEL`
  (i64::MIN + 1) reserved for non-blocking poll results.
- **Non-blocking poll + round-robin scheduling** (Phase A2):
  `p_executor_poll_all` (non-blocking, returns count of pending tasks),
  `p_task_poll_nonblocking` (returns value if ready, PENDING_SENTINEL if not).
  Enables concurrent task interleaving: spawn N tasks, poll all until done.
  Thread results stored in `p_task_results[]` array to prevent double-join.
- **Non-blocking IO** (Phase A3): `p_net_connect`, `p_net_read`, `p_net_write`,
  `p_net_close`, `p_fs_read_async`. Async IO operations spawn threads that
  perform the IO and return Future handles. C codegen uses real sockets/files
  with pthreads. Interpreter simulates IO with sleep + fake results.
- **New tests**: `test_async_executor`, `test_async_interleave`, `test_async_io`.
- **Bounded MPSC channels**: `p_chan_create`, `p_chan_send`, `p_chan_recv`,
  `p_chan_try_recv`, `p_chan_close`, `p_chan_is_closed`. Interpreter uses
  `Arc<Mutex<ChannelState>>` with spin-yield blocking. C codegen uses
  pthread mutex + condition variables with proper blocking semantics.
  `PENDING_SENTINEL` (i64::MIN + 1) returned by `p_chan_try_recv` when empty.
  Channels are shared across threads via `channels: Arc<Mutex<HashMap>>`.
  `stdlib/channel.fd` provides `channel_create`, `channel_send`, `channel_recv`,
  `channel_try_recv`, `channel_close`, `channel_is_closed`, and `CHAN_CLOSED()`.
  Test: `tests/test_channel.fd`.
- **Codegen benchmark suite** (`benchmarks/bench_codegen.sh`): Compares Fides
  ccompile output against hand-written C compiled with gcc -O2. Tests fib(30),
  loop sum, nested loops, function call overhead, Collatz (branch-heavy), and
  compilation speed. Results show Fides ccompile achieves ~1.0x runtime parity
  with gcc -O2 (the C backend benefits from gcc's optimizer).
- **Optimizer: @deterministic CSE/GVN** (S-1B): CSE and GVN passes now treat
  calls to `@deterministic` user functions as eliminable. Duplicate calls to
  the same `@deterministic` function with identical arguments within a block
  are replaced with a reference to the first result. This extends CSE/GVN
  beyond runtime helpers (`p_add`, `p_vec_get`, etc.) to user-annotated
  functions. Unit tests: `s1b_cse_eliminates_duplicate_deterministic_call`,
  `s1b_gvn_eliminates_duplicate_deterministic_call`.

### New Features (Phase 13: Debugging Tools)

- **Interpreter debugger** (`fidesc debug <file.fd>`): Interactive debugger that
  runs the Fides interpreter step-by-step. Commands: `break`, `step`, `next`,
  `continue`, `print`, `backtrace`, `list`, `ir`, `ir current`, `effects`,
  `regions`, `capabilities`, `watch`, `reload`, `record`, `quit`.
  Implementation: `src/debugger.rs` + debug hook in `src/interp.rs`.
  - Breakpoints: `break main:10` or `break func_name`
  - Variable inspection: `print x` or `print` (all locals)
  - Source display: `list` shows source with `→` marker at current line
  - IR display: `ir` shows IR with `→` at current instruction; `ir current` shows just the current instruction
  - Effects panel: `effects` shows function effects and annotations (@wrapping, @deterministic, etc.)
  - Regions panel: `regions` shows memory regions (Heap/Stack) and stack allocation count
  - Capabilities panel: `capabilities` shows annotations and required capabilities from effects
  - Watch variables: `watch x` prints on change
- **Record/replay** (`fidesc replay <trace.fdtrace>`): Records execution trace
  and replays it step-by-step. Trace format: `func_name block_id inst_id source_line [value]`.
  Implementation: `TraceEntry` struct in `src/interp.rs`, `save_trace`/`load_trace`/`replay_trace` in `src/debugger.rs`.
- **Hot-reload** (`reload` command in debugger): Re-reads the source file, recompiles,
  and restarts execution. Preserves breakpoints and watches. Uses `__debugger_reload__`
  sentinel to signal the outer loop.
- **Post-mortem debugging** (`fidesc run <file.fd> --post-mortem`): Enables trace
  recording during execution. On panic, automatically saves the trace to
  `<file>.fdtrace` and prints a replay command. Uses `interpret_source_with_trace`
  in `src/compiler.rs`.
- **Native code debugger** (`fidesc autopsy <file.fd>`): Compiles Fides source
  to a native binary (via C codegen + gcc -g -O0) and runs it under ptrace (Linux).
  Commands: `run`, `step`, `continue`, `regs`, `quit`. Shows CPU registers
  (RAX-R15, RIP, RSP, RBP). Implementation: `src/native_debugger.rs` using `libc` ptrace.
- **Debug hook in interpreter**: `debug_hook` field in `Interpreter` struct —
  a closure called before each instruction execution. Can block (e.g. waiting for
  user input) and returns `DebugAction::Continue`, `InjectReturn`, or `InjectPanic`.
  `RunResult::Paused` variant added for future pause/resume support.

### New Features (Phase 12: Diagnostic System)

- **Unified diagnostic system** (`src/diag.rs`): Structured error/warning/note/help
  output with error codes (E001-E120), source context display (shows the offending
  line with a caret `^^^`), and color output (red errors, yellow warnings, cyan
  notes/help — auto-detected from TTY).
- **Error codes**: Every error category has a code (E001-E004 lexer, E020-E023
  parser, E040-E051 typechecker, E080-E081 borrow checker, E090-E096 safety/contract,
  E110 effects, E120 misc). Displayed as `error[E044]: ...`.
- **Source context**: Errors show `--> file:line:col` and the source line with a
  caret pointing at the error location. Example:
  ```
  error[E044]: Type error (line 2): cannot assign str to variable 'x' of type i64.
    --> test.fd:2:1
    |     val x: i64 = "hello"
    | ^
    = help: change the type annotation or the expression.
  ```
- **Lexer error collection**: Lexer errors are now collected in `Lexer.errors`
  instead of printed directly via `eprintln!`. This allows LSP and other tools
  to access lexer errors programmatically.
- **Typechecker line tracking**: Added `current_line` field to `TypeChecker`,
  updated from statement line numbers. Common errors (undefined variable,
  undefined function, type mismatch, struct field, if condition, arg count)
  now include line numbers.
- **Error enrichment pipeline** (`compiler.rs`): `enrich_errors()` post-processes
  error strings from typechecker/borrow checker/parser, adding source context,
  error codes, and help text. `classify_error()` maps error strings to `ErrCode`
  categories. `suggest_did_you_mean()` provides "did you mean 'X'?" suggestions
  using Levenshtein distance.
- **"Did you mean?" suggestions** (`typechecker.rs`): The typechecker now provides
  "did you mean 'X'?" suggestions for:
  - Undefined variables/functions (via `suggestion_suffix()`)
  - Wrong method names on Vec/HashMap/String (via `method_suggestion_suffix()`)
  - Wrong struct field names (via `field_suggestion_suffix()`)
  - Unknown struct/type names (via `type_name_suggestion_suffix()`)
  All suggestions use edit distance (threshold ≤ 3 or prefix match).
- **Consolidated Levenshtein** (`diag.rs` + `typechecker.rs`): The typechecker's
  `edit_distance()` now delegates to `crate::diag::levenshtein()` (a single,
  memory-efficient two-row implementation) instead of maintaining a duplicate
  full-matrix implementation.
- **Colored output in main.rs**: `print_errors()` function auto-detects TTY and
  adds colored `error:` prefix. Rendered Diag strings (containing `error[E...]`)
  are printed as-is; legacy error strings get the colored prefix.

### New Features (Phase 10B)

- **HashMap/String stdlib migration**: `HashMap` and `String` are now implemented
  entirely in `stdlib/core.fd` using raw pointers (`i64` addresses, `*mut u8`).
  The lowerer dispatches `HashMap::new()`, `String::from()`, and all methods to
  Fides stdlib functions (`HashMap_new`, `HashMap_insert`, `String_from`,
  `String_len`, etc.) instead of compiler builtins (`p_map_*`, `p_string_*`).
  The interpreter materializes `Value::Str` arguments into memory when Fides
  functions expect `i64` pointers. The C codegen `Print` instruction calls
  `Vec_to_str`/`HashMap_to_str` Fides functions for formatting. The `core`
  module implicitly has all capabilities (allowing stdlib functions to call
  `p_panic` for bounds checks). `HashMap_to_str` sorts keys lexicographically
  for deterministic output. `@wrapping` annotation used on hash functions to
  allow wrap-around arithmetic.

### New Features (Phase 8B)

- **ECS (Entity-Component-System)**: `component Position { x: f64, y: f64 }` declares
  a component (desugars to struct). `system MoveSystem(dt: f64) { query(Position, Velocity) { ... } }`
  declares a system (desugars to function). `query(CompA, CompB) { body }` iterates
  over entities with all listed components. ECS runtime: `__ecs_create_entity`,
  `__ecs_entity_count`, `__ecs_has`, `__ecs_get`, `__ecs_add`, `__ecs_remove`.
- **State Machines**: `state_machine PlayerState { state Idle { transition "move" to Moving } ... }`
  desugars to an enum + `<SM>_initial()` and `<SM>_handle(state, event)` functions.
- **Event Handlers**: `on player_died(name: str) { ... }` desugars to `__on_<event_name>` function.
- **New keywords**: `component`, `system`, `query`, `state_machine`, `state`, `transition`, `on`.
  Keywords used as identifiers (e.g. `database.query()`, `var match: bool`) are handled via `expect_ident_or_keyword()`.
  `TokenKind::is_word_keyword()` distinguishes word-like keywords (e.g. `match`, `query`) from punctuation tokens (e.g. `=`, `;`) to prevent punctuation from being accepted as identifiers.
- **New stdlib modules**: `stdlib/ecs.fd`, `stdlib/events.fd`.
- **New test**: `tests/test_phase8b.fd` — ECS, state machine, and event handler tests.

### New Features (Phase 7.7)

- **Iterators**: `for x in vec`, `for k in map`, `for i in 0..N` — Vec, HashMap,
  and Range iteration with nested Vec support. HashMap iterates over keys.
- **Error chaining**: `?` operator on `Option<T>` and `Result<T, E>` propagates
  errors. `try { ... }` blocks catch errors. Option/Result methods: unwrap,
  unwrap_or, is_some, is_none, is_ok, is_err.
- **Conditional compilation**: `@cfg(platform = "linux")`, `@cfg(platform =
  "windows")`, `@cfg(platform = "macos")`, `@cfg(debug)`, `@cfg(release)`.
  Filters out functions and structs at compile time.
- **Derive macros**: `@derive(Debug)` generates `debug_str()` method.
  `@derive(Clone)` generates `clone()` method. Applied to structs.
- **Serde**: `@derive(Serialize)` generates `to_json()` method.
  `@derive(Deserialize)` generates `from_json()` method.
- **Dynamic loading**: `p_dl_open(path)`, `p_dl_sym(handle, symbol)`,
  `p_dl_close(handle)`, `p_dl_call(func_ptr, arg)`. Real dlopen/dlsym in C
  codegen, simulated in interpreter.

## New Features (Phase 11)

- **Generic Enums**: `Option<T>` and `Result<T, E>` are now declared with type
  parameters in `stdlib/core.fd`. The parser accepts `enum Foo<T, U> { ... }`
  and `struct Foo<T> { ... }`. The typechecker treats `Option` and `Option<T>`
  as compatible (uniform i64 representation).
- **LSP Server**: `fidesc lsp` runs a Language Server Protocol server over
  stdio. Supports `initialize`, `textDocument/didOpen`, `didChange`, `didClose`,
  and `publishDiagnostics`. No external dependencies — uses a built-in JSON
  parser/serializer.
- **New Stdlib Modules**:
  - `stdlib/math.fd`: Math functions (sqrt, sin, cos, floor, ceil, pow, fabs, constants)
  - `stdlib/env.fd`: Environment variable access (env_get, env_exists)
  - `stdlib/args.fd`: Command-line arguments (args_len, args_get, args_program_name)
  - `stdlib/process.fd`: Process control (process_exit, process_system)
  - `stdlib/regex.fd`: Simple wildcard pattern matching (regex_matches, regex_find, etc.)

Expected-fail tests (35):
test_trait_verify_fail, test_budget_fail, test_sandbox_fail,
test_deterministic_fail, test_field_privacy_fail, test_money_linear_fail,
test_money_conserve_fail, test_banking_double_credit_fail,
test_banking_lost_money_fail, test_loop_linear_fail, test_gas_fail,
test_crashfree_fail, test_typed_strings_fail,
test_init_guarantee_fail, test_init_guarantee_cond_fail,
test_param_queries_fail, test_medical_fail, test_contracts_fail,
test_safety_annotations_fail, test_safety_no_audit_fail,
test_borrowck_fail, test_async_fail,
test_constant_time_fail, test_constant_time_fail2

Known-fail tests: 6 real bugs remain (3 parser + 3 C codegen). The 153/153
normal tests all pass on all three backends.

Features now present in the baseline: tuple destructuring, file I/O
(`stdlib/file.fd`), `txn {}` blocks (with interpreter rollback), loop-aware linear types, `@gas`
annotation, `@crashfree` annotation, `@audit` annotation (with full param trail), banking stdlib,
money stdlib, LogicalTime stdlib (`stdlib/time.fd`), typed strings
(`stdlib/typed_strings.fd` — F67: SQLString/ShellString/HTMLString/JSONString/PathString),
init guarantee (F63: `var` keyword (without initializer) + definite assignment analysis),
raw pointer types (`*mut T` / `*const T` with `unsafe {}` blocks),
memoization stdlib (`stdlib/memo.fd`),
checked arithmetic lock (F64: overflow = panic, always on),
`@wrapping` annotation (F65: escape hatch for wrap-around arithmetic),
bounds checking lock (F62: always on, skippable in `unsafe {}`),
parameterized queries (F68: no string concat on typed strings),
`@medical` annotation (F48: IEC 62304 — no alloc, no recursion, no while, must be @audit),
given/ensure contracts (F10: Design by Contract — preconditions verified at call sites),
composable safety primitives (F44/F52/F56: @no-alloc, @no-recursion, @no-unbounded, @crashfree,
@no-indirect, @requires-audit — @safety-critical, @autosar, @medical are aliases that expand to these;
@dal-a was merged into @medical, @no-panic is internal-only — users write @crashfree),
async/await (Phase 7.6 + Phase A: `async fn`, `await <expr>`, `spawn <expr>` — Future<T> type, typechecker enforces Future<T> for await. Phase A added real executor runtime: `p_executor_new`/`p_executor_spawn`/`p_executor_block_on`/`p_executor_run_all`/`p_task_poll`/`p_task_is_ready`/`p_executor_poll_all`/`p_task_poll_nonblocking` (A1+A2), non-blocking IO: `p_net_connect`/`p_net_read`/`p_net_write`/`p_net_close`/`p_fs_read_async` (A3). Tasks run as real OS threads with non-blocking poll and round-robin scheduling. State machine transform for stackless coroutines still pending),
borrow checker (Phases 1-6 complete: `--borrowck=off|warn|on` — Rules 1-3 active, field-wise borrows, two-phase borrows, NLL, `on` by default),
generic enums/structs (`enum Option<T> { ... }`, `struct Foo<T> { ... }` — type_params in AST, parser, typechecker, lowerer),
LSP server (`fidesc lsp` — JSON-RPC over stdio, initialize/didOpen/didChange/diagnostics, no external deps),
new stdlib modules (`stdlib/math.fd`, `stdlib/env.fd`, `stdlib/args.fd`, `stdlib/process.fd`, `stdlib/regex.fd`, `stdlib/sync.fd`, `stdlib/channel.fd`, `stdlib/vec.fd`, `stdlib/ecs.fd`, `stdlib/events.fd`, `stdlib/parallel.fd`, plus game stdlib: `stdlib/tilemap.fd`, `stdlib/sprite.fd`, `stdlib/flowfield.fd`, `stdlib/streaming.fd`, `stdlib/scene.fd`, `stdlib/physics.fd`, `stdlib/audio.fd`, `stdlib/input.fd`).

**Note:** 153/153 normal tests pass on interpreter + PE, 153/153 on C codegen. The test runner separates normal tests (159, should pass) from skip-list tests (0: all previously skipped tests now pass) and known-fail tests (6: real bugs).

### Vec Higher-Order Methods (vec.fd)

- **`stdlib/vec.fd`** provides higher-order Vec methods: `map`, `filter`, `fold`,
  `for_each`, `any`, `all`, `find`, `position`, plus iterator adapters: `take`,
  `skip`, `zip`, `enumerate`, `flat_map`, `chunk`, `window`, `dedup`, `sort_by`,
  `contains_val`. These are top-level functions (`Vec_map`, `Vec_filter`, etc.)
  that forward to `p_vec_*` runtime functions. Iterator adapters are also
  available as free functions (`vec_take`, `vec_skip`, etc.).
- **`stdlib/core.fd`** provides basic Vec operations: `Vec_new`, `Vec_push`,
  `Vec_pop`, `Vec_len`, `Vec_get`, `Vec_set`, etc. These are Fides functions
  using raw pointer arithmetic on a `{ data, len, cap }` struct. Also provides
  Option/Result combinator methods: `map`, `and_then`, `or_else`,
  `unwrap_or_else`, `expect`.
- **Interpreter `p_vec_*` runtime**: `p_vec_new` allocates a 24-byte struct
  (`{ data, len, cap }`) in interpreter memory and stores a `Value::Vec` in a
  separate side-table (`vec_side_table`). This separates raw struct fields
  (for Fides pointer arithmetic) from the high-level `Value::Vec` (for
  efficient interpreter operations). `p_vec_push`/`p_vec_len`/`p_vec_get` etc.
  use `lookup_vec_value()` to find the `Value::Vec` from either `Value::Int`
  (address) or `Value::Vec` directly.
- **Lowerer dummy env**: All user functions get a `__env` param (first param).
  The lowerer prepends a dummy `env_ptr` (ValueId(0)) to calls. Runtime
  functions (`p_vec_*`, `p_map_*`, `__ecs_*`) that are NOT user functions
  (not in `module.functions`) do NOT get the dummy env. The `is_runtime`
  check in `lower.rs` distinguishes: `is_user_func = module.functions.contains_key(callee)`,
  `is_runtime = !is_user_func && (callee.starts_with("p_") || callee.starts_with("__ecs_"))`.

**Before making any change:** run `./verify.sh` to confirm you're starting from a known-good state.

## HIGH-RISK FILES — Always run `./verify.sh` after editing these

These files are where silent errors are most dangerous. Edits here can break
everything with no warning. Always run the full verification suite after
touching them.

| File | Risk | Why |
|------|------|-----|
| `src/regalloc.rs` | **CRITICAL** | Linear scan allocator (~1100 LOC). F-1A: constant rematerialization (`Location::Remat`). F-1D: effect-aware register allocation — calls to pure functions (in `L3Module.pure_functions`) only clobber RAX, not all caller-saved registers. I-2D: calls to `@no-alloc` functions (in `L3Module.no_alloc_functions`) also only clobber RAX. `@medical` functions included in no-alloc set. Any change here affects EVERY function. |
| `src/peephole.rs` | **CRITICAL** | 9 passes, always on in `build`. A wrong transform silently corrupts all output. |
| `src/elf64.rs` | **HIGH** | ELF64 header bytes, section layout, relocations (R_X86_64_PC32/PLT32/64). Off-by-one = corrupt binary. |
| `src/pe.rs` | **HIGH** | PE header bytes, import table, section layout. Off-by-one = corrupt .exe. |
| `src/coff.rs` | **HIGH** | COFF object file format. Same byte-level risk as PE/ELF. |
| `src/macho.rs` | **HIGH** | Mach-O object file format. Same byte-level risk. |
| `src/codegen_x64.rs` | **HIGH** | PIR L3 → x86-64 instruction encoding. ModRM/SIB bytes, REX prefixes, ABI rules. |
| `src/codegen_c.rs` | **HIGH** | IR → C99. ABI casts, struct offsets, function renaming. 153/153 test parity (0 skip). Phase 10B: Print instruction calls Fides Vec_to_str/HashMap_to_str for formatting. Channels: p_chan_* with pthread mutex+condvar. Binary hardening: stack canaries, PIE/ASLR, RELRO, NX stack. Stack traces on panic. Rc/Arc runtime via p_rc_incr/p_rc_decr. ML3: Autograd tape (ag_tape, ag_record, p_autograd_backward) with reverse-mode AD for add/sub/mul/matmul/scale/relu/sigmoid/sum/mean. ML11: Checkpoint mode stores input data for relu/sigmoid backward. ML12: DP noise/budget builtins. ML13: TP builtins (identity in single-rank). ML9: KV cache with sliding window eviction. ML10: Quantization (quantize/dequantize/qmatmul/find_quant_params). Tensor printing via p_tensor_to_str with %.4f for non-integers. Dlopen simulated to match interpreter (sequential handles). |
| `src/pir.rs` | **HIGH** | PIR L1/L2/L3 transforms. A bad lowering propagates to all backends. |
| `src/lower.rs` | **MEDIUM** | AST → SSA IR. Changes here affect interpreter + all backends. `runtime_effects()` helper sets Write/Alloc/IO effects on `p_*` builtin calls — without this, the optimizer DCE's side-effecting calls. `p_vec_*` extern declarations (23 functions) ensure correct calling convention. |
| `src/typechecker.rs` | **MEDIUM** | Types + regions + effects + capabilities. Wrong rejection = broken code; wrong acceptance = silent miscompilation. |
| `src/borrowck.rs` | **LOW** | Borrow checker (Phases 1-6 complete). Separate pass after typecheck. `--borrowck=on` by default; `off`/`warn` modes available. Field-wise borrows, two-phase borrows, NLL (non-lexical lifetimes via last-use analysis). |
| `src/opt.rs` | **MEDIUM** | PIR optimizer. 23 passes: const-fold, CSE, GVN, SROA, DSE, DCE, inline, unroll, TCO, alias-analysis, effect-reorder, partial-eval, vectorize, strength-reduce, devirt, jump-thread, loop-rotate, loop-vec, effect-opt, region-pool, reassociate, memoize. 22 in default pass list (region-pool via `--region-pool`). `--opt` is enabled by default in `build` (use `--no-opt` to disable). `--lto` enables aggressive cross-module inlining (larger inline limits). PGO has been removed. S-1A pass skipping: `pass_relevant_for_module()` skips passes that can't help (no loops → skip unroll, no calls → skip inline, etc.). Inline cost model (F-3F): cost = blocks×5 + insts, with `is_pure` (50% bonus), `is_crashfree` (25%), `is_deterministic` (10%), capped at 75%. Sandbox-aware inlining (I-3C): sandbox call sites use 6 blocks / 40 insts vs normal 3/20. Sandbox-aware devirt (I-3C): devirtualize DynCalls in sandbox blocks to pure impls. Region bulk free (I-1C): `pass_region_pool` emits `p_region_free` before returns. @crashfree code hoisting (I-4B): `pass_effect_optimize` hoists @crashfree + effectively-pure calls out of loops. @gas-guided unrolling (I-4C): `pass_unroll` uses max_unroll=64 when `gas_limit.is_some()` (vs normal 8). Annotations propagated: `is_wrapping`, `is_deterministic`, `is_crashfree`, `is_no_alloc`, `is_medical`, `gas_limit`. I-6B linear type alias analysis: `linear_structs` propagated from AST → IrModule → L1Module → L2Module; `alias_analysis_function` tracks `linear_ptrs` and uses them to prove non-aliasing (stores through linear pointers can't invalidate loads from other pointers). |
| `src/comptime.rs` | **LOW** | Comptime evaluation engine. Evaluates `comptime <expr>`, `comptime { block }`, `comptime reflect_op(T)`, `comptime gen { block }` at compile time. I-5A: Aggressive comptime evaluation — auto-evaluates calls to `comptime fn` functions with all-literal args (no explicit `comptime` wrapper needed). I-5D: Comptime loop unrolling — unrolls `for` loops with literal bounds (≤16 iterations, ≤16 body stmts) at AST level. Also eliminates dead `if` branches with literal conditions. |
| `src/runtime_linux.rs` | **HIGH** | Hand-encoded x86-64 machine code (raw bytes). No assembler — every byte is manual. |
| `src/linker.rs` | **MEDIUM** | Relocation resolution. Wrong offset = crash or silent corruption. |
| `src/interp.rs` | **MEDIUM** | The reference oracle (~4400 LOC). If this is wrong, all backends "match" but produce wrong results. `p_vec_new` uses `vec_side_table` to separate raw struct fields from `Value::Vec`. `run_function` auto-detects missing `__env` param and inserts dummy. |

## LOWER-RISK FILES

| File | Risk | Why |
|------|------|-----|
| `src/lexer.rs` | LOW | Tokenizer — errors are usually loud (parse failures). |
| `src/parser.rs` | LOW | Recursive descent — errors are loud. |
| `src/ast.rs` | LOW | Data definitions — changes propagate visibly. |
| `src/token.rs` | LOW | Token enum — adding tokens is safe. |
| `src/mono.rs` | LOW | Monomorphization — errors surface as missing functions. |
| `src/comptime.rs` | LOW | Compile-time eval — errors are loud. |
| `src/main.rs` | LOW | CLI dispatch — adding commands is safe. |
| `src/compiler.rs` | LOW | Pipeline orchestration — changes are visible. Phase 12: `enrich_errors()` post-processes errors with source context + error codes. |
| `src/diag.rs` | LOW | Diagnostic system — structured error/warning/note/help with error codes, source context, color. Zero external deps. |
| `src/lsp.rs` | LOW | LSP server — JSON-RPC over stdio, no external deps. |
| `stdlib/*.fd` | LOW | Fides source — errors surface as test failures. 69 stdlib modules: core, vec, math, string, fmt, bits, sort, hash, iter, stack, queue, buffer, collections, result, random, path, units, uuid, crypto, storage, clinical, medical, ecu, scada, swift, grid, ota, banking, kv_cache, quant, dp, shard, fs, io, os, sys, ecs, events, parallel, sync, channel, db, http, json, shape, simd, tensor, money, alloc, file, time, typed_strings, memo, privacy, env, args, process, regex, tilemap, sprite, flowfield, streaming, scene, physics, audio, input, arinc, can. |

## Known Bugs (Do Not Re-Introduce)

1. **Register allocator clobbering** (`regalloc.rs`): Previously, constants not
   loaded into registers before use could be clobbered by intervening
   instructions. **Fixed by F-1A (Constant Rematerialization):** constants are
   now rematerialized at each use site via `Location::Remat(i64)` instead of
   being spilled and reloaded. The L2 optimizer's `effect_reorder` pass was also
   fixed to respect memory dependencies and keep terminators last. Test with
   `--no-opt` when changing regalloc to isolate issues.

2. **Debug `eprintln!` statements** in `compiler.rs` and `elf64.rs`: There are
   temporary debug prints (DBG ASM, DBG ALU) added during debugging sessions.
   These should be removed before committing but are safe to leave during
   active debugging.

3. **PIR L2 GetFieldPtr offset** (`pir.rs` line ~1240): `offset: *field_index * 8`
   uses `field_index * 8` as the byte offset. This is **correct by design** —
   `compute_struct_layout()` in `lower.rs` deliberately assigns every field an
   8-byte slot (uniform representation), so `field_index * 8` always matches the
   real offset in `StructLayout`. This is not a bug; it's a design choice. If
   struct layouts are ever changed to use packed/sized fields, this code must be
   updated to look up real offsets from `struct_layouts` (as `codegen_c.rs` does).

4. **Native codegen bugs** (not optimizer-related): All previously failing
   native ELF64 tests (`test_collections`, `test_db`, `test_match_range`,
   `test_matrix_mul`) are now FIXED. The root cause was a single wrong REX
   prefix byte in `p_json_at` (`0x49` instead of `0x4C` — see changelog for
   details). The `--opt` flag produces identical output to non-opt for all
   tests except 3 closure tests that are broken in both paths.

## Coding Conventions

1. **Codegen output**: use the `wln!` macro for formatted output in
   `codegen_c.rs` (defined at top of that file).

2. **Error handling**: return `Result<_, Vec<String>>` for compiler errors
   (multiple errors collected). Don't use `panic!` for user-facing errors.

3. **Test naming**: `test_<feature>.fd` for feature tests, `test_pe_<feature>.fd`
   for PE-emitter-specific tests. Keep the skip list in `tests/harness.rs`
   updated when adding tests for unimplemented features.

4. **Adding a new test**: create `tests/test_<name>.fd`, run `./verify.sh --save`
   to update the baseline. If the test exercises a not-yet-implemented feature,
   add it to `is_skip_test()` in `tests/harness.rs`.

5. **Adding a new backend stage**: add a golden file capture in
   `scripts/gen_golden.sh` and a check in `scripts/bisect.sh`.

## Debugging Workflow (When Tests Break)

1. **Don't panic.** Run `./verify.sh` to see exactly which tests regressed.

2. **Bisect the layer.** For a failing test, run:
   ```bash
   ./scripts/bisect.sh <test_name>
   ```
   This tells you whether the regression is in lowering, PIR, codegen, or
   peephole.

3. **Check if it's a real regression or a "was passing for the wrong reason".**
   Compare the failing test's golden output to the new output. If the new
   output is *correct* and the golden was *wrong*, the test was passing by
   accident — update the golden file.

4. **Make the smallest possible fix.** Don't refactor while fixing. One change,
   one commit, one `./verify.sh` run.

5. **If you must make a large change**, gate it behind a flag:
   ```rust
   const NEW_BEHAVIOR: bool = true;  // flip to false to test old path
   ```
   Then test both ways before removing the flag.

## What To Do Before Every Session

```bash
# 1. Confirm baseline
./verify.sh

# 2. If baseline doesn't match, someone changed something — investigate before
#    building on top of a broken state.

# 3. If you're starting fresh and there's no baseline:
./verify.sh --save
```

## What To Do After Every Change

```bash
# 1. Run verification
./verify.sh

# 2. If regressions: bisect to find the layer
./scripts/bisect.sh <failing_test>

# 3. If no regressions and tests improved: update baseline
./verify.sh --save

# 4. If you changed codegen/regalloc/peephole/elf64/pe: also regenerate golden
#    files so future bisects are accurate
./scripts/gen_golden.sh
```

## CRITICAL RULES — NEVER DO THESE

1. **NEVER run `git stash`.** This can lose the user's work. Ever. No exceptions.
   If you need to compare against original code, use `git diff` or `git show`
   on specific files — never stash the working tree.

2. **NEVER run `git checkout` on files the user has modified.** This overwrites
   their changes. Use `git diff` to see what changed instead.

3. **NEVER run `git reset --hard` or `git clean`.** These are destructive and
   will wipe uncommitted work.

4. **NEVER force-push or rewrite git history.** No `git push --force`,
   no `git rebase` that rewrites shared commits.

5. **NEVER delete files you did not create.** If a file is in the way, ask the
   user — don't `rm` it.

6. **NEVER use `rm -rf` on project directories.** Ever.

If you need to see the original state of a file, use `git show HEAD:<path>`
or `git diff <path>` — these are read-only and safe.

## File Map (Quick Reference)

| File | Role |
|------|------|
| `src/lexer.rs` | Tokenizer |
| `src/parser.rs` | Recursive descent parser |
| `src/typechecker.rs` | Types + regions + effects + capabilities |
| `src/borrowck.rs` | Borrow checker (alias analysis, `--borrowck=off\|warn\|on`) |
| `src/ir.rs` | Single-level SSA IR |
| `src/lower.rs` | AST → SSA IR |
| `src/pir.rs` | Multi-level PIR (L1/L2/L3) |
| `src/opt.rs` | PIR optimizer (enabled by default in `build`, `--lto` for aggressive inlining) |
| `src/peephole.rs` | 9-pass peephole (always on in `build`) |
| `src/regalloc.rs` | Linear scan register allocator (⚠️ known bug) |
| `src/mono.rs` | Generic monomorphization |
| `src/comptime.rs` | Compile-time eval + reflection |
| `src/pe.rs` | PE emitter (Windows) |
| `src/elf64.rs` | ELF64 emitter (Linux) |
| `src/macho.rs` | Mach-O emitter (macOS) |
| `src/coff.rs` | COFF emitter (Windows) |
| `src/linker.rs` | Integrated linker |
| `src/runtime_linux.rs` | Hand-encoded Linux runtime (raw syscalls) |
| `src/codegen_x64.rs` | PIR L3 → x86-64 assembly |
| `src/codegen_arm64.rs` | PIR L3 → ARM64 assembly |
| `src/codegen_riscv.rs` | PIR L3 → RISC-V assembly |
| `src/codegen_c.rs` | IR → C99 source |
| `src/wasm.rs` | PIR → WebAssembly |
| `src/gpu.rs` | PIR → SPIR-V |
| `src/embedded.rs` | FixedBufferAllocator, RealTime effect, HAL |
| `src/hotreload.rs` | Native JIT + function-level change detection |
| `src/cparser.rs` | C header parser → Fides FFI bindings |
| `src/compiler.rs` | Pipeline orchestration |
| `src/lsp.rs` | LSP server (JSON-RPC over stdio) |
| `src/main.rs` | CLI entry point |
