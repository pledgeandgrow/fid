# green_thread.fd — cooperative green threads (207 LOC)

> **CONSOLIDATED + VERIFIED LIVE 2026-10-23 (v2):** the three-layer fiction
> is one real layer now —
> * `gt_global_*` constant stubs **deleted**; `green_thread_c_runtime`
>   130-line C blob **deleted** (F3's name-mismatch stranded code gone).
> * `GreenThread.func_id` → `func_name: str` — matches the C backend's
>   name→fnptr dispatch model.
> * `interp.fd` owns an `Interpreter.gt_scheduler`; `p_gt_spawn` queues a
>   READY thread, `p_gt_run_all` resolves the name and **executes** it,
>   `p_gt_is_done`/`p_gt_get_result` return real state (F1, F4 fixed).
>   `p_thread_spawn`/`p_future_poll`/`p_executor_*` given the same real
>   treatment — `spawn f(x)`/`await` now return actual results.
> * VERIFIED: spawn/await → 41; multi-spawn `9 16 25` → 50 aggregate.
> * F2 (no C backend for `p_gt_*`) **remains**: C-compiled programs calling
>   gt builtins still fail to link — pending a C-runtime table or
>   runtime.fd fold-in (Phase 2).

## Role

`GreenThread`/`GreenThreadScheduler` — the single scheduler layer, driven
by `compiler/interp.fd`'s `p_gt_*` handlers (lazy cooperative semantics:
spawn queues, run_all executes by name). Port of `src/green_thread.rs`
(600 LOC — dead there too).

## Where it's called

| Caller | Site | What |
|--------|------|------|
| `compiler/interp.fd` | :26 import, :2618-2653 | `p_gt_spawn`/`yield`/`run_all`/`is_done`/`get_result`/`current`/`reset` builtins → `gt_global_*` — **live on the interp path**. |
| `compiler/compiler.fd` | :40 | `use compiler::green_thread` — import only, no calls. |

`lower.fd:1768-1774` declares all seven `p_gt_*` externs — any program
using gt builtins or `spawn` lowers to these symbols.

## Findings

### F1 — `gt_global_*` are all stubs — spawn never runs — HIGH (live)

The seven functions the interpreter actually calls return constants:
`gt_global_spawn` → `0`, `is_done` → `true`, `get_result` → `0`,
`current` → `-1`, `yield`/`run_all`/`init`/`reset` → no-ops
(:210-245). `p_gt_spawn` in the interpreter hands back a fake ID and
**the spawned function is never evaluated** — spawn is pure fiction:
`spawn f(5)` silently drops `f`, `get_result` says `0`, `is_done`
says `true`. The comment ("interpreter runs eagerly") misrepresents
it — there's no eager call anywhere.

### F2 — `p_gt_*` has no C backend — HIGH (link failure)

`codegen_c.fd` implements none of the `p_gt_*` symbols — any program
calling `p_gt_spawn`/`p_gt_yield`/… compiles to C that **fails to
link** (undefined symbol). The whole `p_gt_*` vocabulary is
Fides-port-added (absent from src/ entirely).

### F3 — `green_thread_c_runtime` never emitted + wrong names — MEDIUM

The real implementation (asm context switch, dispatch table,
`gt_spawn`/`gt_yield`/`gt_run_all`) is stranded: nothing calls
`green_thread_c_runtime()` — and even if it were emitted, it defines
`gt_*` while `lower.fd` declares `p_gt_*` — a name-mismatch stub
either way. Dead code carrying real effort.

### F4 — `GreenThreadScheduler` never instantiated — MEDIUM

The actual simulation (round-robin ready queue, state machine,
256-thread cap) is bypassed entirely — `gt_global_*` don't create or
touch a scheduler. `run_all` (:179-199) also just marks every thread
`GT_DONE` with `result=0` — even the simulation never "runs" a thread;
`yield_current` re-queues correctly but pops via O(n) shift.

## Verification state

- **LIVE-VERIFIED 2026-10-23:** self-hosted compiler runs `spawn worker(20)`
  → `await f` → 41 (real result, not 0); three spawned computations →
  `9 16 25` → aggregate 50. `is_done`/`get_result` read actual scheduler
  state. tests/selfhost/test_spawn_gt.fd covers 4 cases.
- F2 still open: `codegen_c.fd` has zero `p_gt_*` → C-path link failure on
  gt programs (needs a name→fnptr table like `p_thread_spawn` uses).
- Dead-in-Rust confirmed: no `green_thread::`/`p_gt_` refs in `src/`.

## Next

`hotreload.fd`.
