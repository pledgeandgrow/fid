# green_thread.fd — cooperative green threads (386 LOC)

## Role

`GreenThread`/`GreenThreadScheduler` (in-process simulation),
`gt_global_*` interpreter-facing API, and `green_thread_c_runtime()`
— a ~130-line embedded C source with real x86-64 context switching
(`gt_save_context`/`gt_restore_context` via inline asm, 64KB stacks,
256 threads, function dispatch table). Port of `src/green_thread.rs`
(600 LOC — dead there too: zero `green_thread::` refs in `src/`).

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

- Stub constants verified by reading every `gt_global_*` body.
- **Re-verified 2026-10-23:** F1–F4 all still stand. `p_gt_*` in interp does call
  `gt_global_*` — but `gt_global_spawn` returns 0 without evaluating the fn
  (:214-216), `run_all` returns 0, `is_done` true. And `codegen_c.fd` still has
  zero `p_gt_*` implementations → C-path link failure on any gt program.
- `p_gt_*` absence in codegen_c verified by grep; lower externs
  confirmed at :1768-1774.
- Dead-in-Rust confirmed: no `green_thread::`/`p_gt_` references
  in `src/`.
- Net: interp = silent no-op spawn; C = link failure; runtime = dead.

## Next

`hotreload.fd`.
