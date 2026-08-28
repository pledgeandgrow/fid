# Current Focus

> Single source of truth for "what are we doing right now."
> Read this first every session. Updated after every milestone change.
> Last updated: Aug 28, 2026.

## ⚠️ WHERE WE STOPED (Aug 28 — pick up here tomorrow)

**Failing tests to investigate:** The user reverted the opaque-value-to-i64
conversion in `src/interp.rs` (Inst::Call). The previous fix converted
`Value::Vec`/`Map`/`Tensor`/`Json`/`Dyn` to their i64 handles when calling
user functions (since user functions like `Vec_len` expect i64 params but
the lowerer types Vec fields as Generic/Opaque). That fix was reverted —
need to check which tests fail without it and find a better approach.

**Also:** String comparison fix in interp.rs (BinOp Eq/Neq for Ptr(U8)) and
GetFieldPtr Int-as-Ptr fix may still be in place — verify they don't break.

**Action items:**
1. Run `./target/release/fidesc test` and identify which tests fail
2. Determine if the opaque-value conversion is needed or if the lowerer
   should type these params as i64 instead
3. Fix failing tests
4. Run `./verify.sh` to confirm clean baseline

## Milestone: Self-Hosting Readiness (Phase 10)

**Status:** Language ready (3/3 language blockers resolved). Stdlib ready.
All tests pass on all 3 backends. Ready to start rewrite.

**See `docs/roadmap/selfhost-roadmap.md`** for the 10-phase self-hosting plan.

## Test Baseline

**See `AGENTS.md` → "Current Baseline"** for the authoritative test counts.
All 153/153 normal tests pass on interpreter, C codegen, and PE emitter.
0 skip, 0 known-fail, 265/265 unit tests.

## What's Left Before Self-Host Rewrite

### Decision Made: C Codegen Bootstrap
1. **C codegen bootstrap** — Start self-host rewrite using C codegen backend.
   Works today (153/153 tests pass, zero blockers). gcc/clang provides argv,
   file I/O, process execution — no runtime stubs needed.
   - PE emitter rewrite is **Phase 10B** (post-bootstrap): fix Issues 67-69,
     rewrite pe.rs in Fides, switch default backend to PE for zero-dependency
     deployment. C backend stays as fallback (like Zig).
   - See `docs/roadmap/roadmap.md` Phase 10B for full plan.

### Should Do (hygiene — before rewrite, not blocking)
2. Write a 500+ line Fides program (JSON parser or mini-lexer) to test
   scale/deep recursion/cross-module before committing to full rewrite

## Deferred (Do Not Start)

- PE emitter rewrite (Phase 10B — after C codegen self-host lands)
- Playground website (Astro, post-self-host)
- Package registry (Next.js, post-self-host)
- New language features (add on Rust until self-host lands)
- Industry domain stdlibs (Phase 9 — medical, blockchain, robotics)

## Gate

`verify.sh` must pass clean before self-host rewrite begins.
Current: 153/153 on all 3 backends. 0 skip, 0 known-fail. 136 warnings
(dead code stubs for future features).
