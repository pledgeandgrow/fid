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

## Current Test Baseline (Aug 27, after runtime effects fix)

```
Normal tests:  159 run
  Interpreter:  149/149 passed
  C Codegen:    149/149 matched
  PE Emitter:   149/149 valid
Skip list:      4  (3 C codegen missing-builtins: kv_cache, quant, dp + 1 design difference: dlopen)
Known-fail:     6  (test_game_stdlib, test_iter, test_vec_math,
                    test_parallel_concurrent, test_time, test_time_edge)
Expected-fail:  35 (correctly fail — error detection tests)
Bonus passes:   3  (test_iter, test_vec_math, test_parallel_concurrent
                    now pass on interp/PE — were known-fail)
```

### Runtime Effects Fix (Aug 27)

**Bug:** `Vec_push` and other side-effecting runtime calls were silently
removed by the optimizer's dead-call elimination pass. The lowerer set
empty `EffectSet` on `p_vec_push` etc. calls, so `Vec_push` was marked
`is_pure=true` and all its call sites were DCE'd.

**Fix:** Added `runtime_effects()` in `src/lower.rs` — returns Write/Alloc/IO
effects for known runtime builtins. Also added `p_vec_*` extern declarations
and `GradBegin`/`GradEnd`/`Backward` match arms in 6 backend files.

**Result:** Vec push/pop/get works with `--opt`. 3 bonus passes. Skip list
reduced from 7 to 4.

## What's Left Before Self-Host Rewrite

### Decision Made: C Codegen Bootstrap
1. **C codegen bootstrap** — Start self-host rewrite using C codegen backend.
   Works today (149/149 tests pass, zero blockers). gcc/clang provides argv,
   file I/O, process execution — no runtime stubs needed.
   - PE emitter rewrite is **Phase 10B** (post-bootstrap): fix Issues 67-69,
     rewrite pe.rs in Fides, switch default backend to PE for zero-dependency
     deployment. C backend stays as fallback (like Zig).
   - See `docs/roadmap/roadmap.md` Phase 10B for full plan.

### Should Do (hygiene — before rewrite, not blocking)
2. Fix 3 C codegen missing-builtins (test_kv_cache, test_quantization,
   test_dp) — add ML runtime builtins, fix float formatting differences.
   Or accept as skip-list items.
3. Fix 3 parser bugs (test_game_stdlib, test_time, test_time_edge) —
   all hit "expected identifier but got 'LParen'" parse errors.
4. Reconcile doc contradiction: roadmap-limitations.md says "3/5 blockers"
   vs roadmap.md "all 3 blockers." Reality: 3 language blockers done,
   binary IO done, native runtime remaining = 4/5.
6. Write a 500+ line Fides program (JSON parser or mini-lexer) to test
   scale/deep recursion/cross-module before committing to full rewrite

## Deferred (Do Not Start)

- PE emitter rewrite (Phase 10B — after C codegen self-host lands)
- Playground website (Astro, post-self-host)
- Package registry (Next.js, post-self-host)
- New language features (add on Rust until self-host lands)
- Industry domain stdlibs (Phase 9 — medical, blockchain, robotics)

## Gate

`verify.sh` must pass clean before self-host rewrite begins.
Current: 149/149 normal tests pass on interpreter, 149/149 on C codegen,
149/149 on PE emitter. 4 skipped, 6 known-fail (3 parser + 3 C codegen).
3 bonus passes (iter, vec_math, parallel_concurrent on interp/PE).
