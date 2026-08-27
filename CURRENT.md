# Current Focus

> Single source of truth for "what are we doing right now."
> Read this first every session. Updated after every milestone change.
> Last updated: Aug 27, 2026.

## Milestone: Self-Hosting Readiness (Phase 10)

**Status:** Language ready (3/3 language blockers resolved). Stdlib ready.
Issue 47 fixed. One strategic decision remaining before rewrite can start.

## What's Done

- 3 language-level self-host blockers resolved (typed enum payloads, real
  generics, recursive Box<T>) — all pass on all 3 backends
- Stdlib gaps filled (string ops, binary IO, Result/Option API, StringBuilder)
- Pre-flight audit complete (Aug 27):
  - Skip list cleaned: 37 of 42 skipped tests actually PASS on all 3
    backends — moved to normal list. Only 5 real skips remain (4 C codegen
    missing-builtins + 1 design difference).
  - Known-fail list cleaned: 4 of 10 known-fail tests actually PASS on all
    3 backends — moved to normal list. 6 real bugs remain.
  - Expected-fail list: 35/35 correctly fail — clean
- **Issue 47 FIXED** (Aug 27): `val` struct field assignment now rejected.
  `Stmt::FieldAssign` in typechecker checks `val_vars_per_scope` and emits
  a type error if the object was declared `val`. Test: `/tmp/test_issue47.fd`.
- **test_drop removed from known-fail** — passes on all 3 backends.

## Current Test Baseline (Aug 27, after skip/known-fail cleanup)

```
Normal tests:  159 run
  Interpreter:  146/146 passed
  C Codegen:    146/146 matched  (all normal tests pass; test_autodiff_grad + test_ml_full in skip list)
  PE Emitter:   146/146 valid
Skip list:      7  (5 C codegen missing-builtins: kv_cache, quant, dp, shard, autodiff_grad, ml_full + 1 design difference: dlopen)
Known-fail:     6  (test_game_stdlib, test_iter, test_vec_math,
                    test_parallel_concurrent, test_time, test_time_edge)
Expected-fail:  35 (correctly fail — error detection tests)
```

## What's Left Before Self-Host Rewrite

### Must Decide (strategy)
1. **Native runtime stubs** — argv, file I/O, process execution in
   `runtime_linux.rs` for PE emitter path. NOT needed for C codegen bootstrap
   (libc provides these). Decide: C-codegen-only bootstrap OR implement stubs.
   - C codegen path: works now, depends on gcc/clang forever
   - PE emitter path: needs runtime stubs, but fully self-contained

### Should Do (hygiene — before rewrite, not blocking)
2. Fix 6 C codegen missing-builtins (test_kv_cache, test_quantization,
   test_dp, test_shard, test_autodiff_grad, test_ml_full) — add ML runtime
   builtins and `p_send_tensor` decls, fix `void*` from `int` type mismatches.
   Or accept as skip-list items.
3. Fix 3 parser bugs (test_game_stdlib, test_time, test_time_edge) —
   all hit "expected identifier but got 'LParen'" parse errors.
4. Reconcile doc contradiction: roadmap-limitations.md says "3/5 blockers"
   vs roadmap.md "all 3 blockers." Reality: 3 language blockers done,
   binary IO done, native runtime remaining = 4/5.
6. Write a 500+ line Fides program (JSON parser or mini-lexer) to test
   scale/deep recursion/cross-module before committing to full rewrite

## Deferred (Do Not Start)

- Playground website (Astro, post-self-host)
- Package registry (Next.js, post-self-host)
- New language features (add on Rust until self-host lands)
- Industry domain stdlibs (Phase 9 — medical, blockchain, robotics)

## Gate

`verify.sh` must pass clean before self-host rewrite begins.
Current: 146/146 normal tests pass on interpreter, 146/146 on C codegen,
146/146 on PE emitter. 7 skipped, 6 known-fail (real bugs).
