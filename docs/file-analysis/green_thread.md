# green_thread.fd — cooperative green-thread scheduler

- **Path:** `compiler/green_thread.fd` — module `compiler::green_thread`
- **LOC:** 207 (2026-11-09). Rust `src/green_thread.rs`: 600 (dead there).
- **Role:** thread records and scheduler bookkeeping (`GreenThread`,
  `GreenThreadScheduler`) for the interpreter's `p_gt_*` builtins. It does
  not run anything itself: the interpreter resolves the function by name
  and runs it.
- **Status:** partly live: live under `fid run` through interp.fd; no other
  backend implements `p_gt_*`.
- **Re-audited:** 2026-11-09.

## Where it is called

| Caller | Site | What |
|---|---|---|
| `compiler/interp.fd` | :26 import; field `gt_scheduler` (:874, :941); :3849-3936 | `p_gt_spawn` → `GreenThreadScheduler_spawn`; `p_gt_yield` → `_yield_current`; `p_gt_is_done` / `_get_result` / `_current`; `p_gt_reset` → `_new`. `p_gt_run_all` runs threads itself (does not call `_run_all`). |
| `compiler/compiler.fd` | :43 | `use compiler::green_thread`: import only, nothing used. |
| `compiler/lower.fd` | :2263-2269 | declares the seven `p_gt_*` externs |
| `tests/selfhost/test_spawn_gt.fd` | | runs `p_gt_*` programs inside the interpreter |

Reached from: `fid run` only (via interp.fd). Not used by `stdlib/task.fd`
(that is `p_co_*`, a different runtime).

Unused public functions: `GreenThreadScheduler_finish_current`,
`_has_ready_threads`, `_run_all` (no caller anywhere).

## What it does

- `GreenThreadScheduler_new` (:85) makes 256 empty slots; `spawn` (:99)
  fills the next slot (slots are never reused: after 256 spawns it returns
  -1 until `p_gt_reset`) and appends the id to `ready_queue`.
- `yield_current` (:114) marks the current thread SUSPENDED, appends it to
  the queue, then pops from the front (O(n) shift) until it finds a READY or
  SUSPENDED thread, marks it RUNNING and makes it current.
- `is_done` / `get_result` read the slot.
- Constants `GREEN_THREAD_STACK_SIZE` (65536), `MAX_GREEN_THREADS` (256),
  states `GT_READY/RUNNING/SUSPENDED/DONE` (0-3).

## Findings

### Verified live (2026-11-09, `s18.exe run`)

1. **NEW, HIGH — a thread that yields twice makes the next thread never
   run.** `p_gt_run_all` (interp.fd:3872-3916) runs threads in id order and
   only runs those still READY or SUSPENDED. `yield_current` (:114-140)
   picks the next queued thread and marks it RUNNING without running it.
   The queue still holds every spawned id (spawn appends, `run_all` never
   pops), so the first yield hands back the yielding thread itself, but the
   second yield hands out the next thread; `run_all` then skips it. Probe:
   `w1` calls `p_gt_yield()` twice, `w2` spawned after it:
   `is_done(w2) == false`, `get_result(w2) == 0`. With one yield both finish
   (11 and 22). Fix: make `yield` a no-op inside `run_all` (threads run to
   their end anyway), or have `run_all` drain `ready_queue`.
2. **FIXED (was F1, HIGH) — `gt_global_*` stubs.** Deleted; spawn now runs
   the function (verified 2026-10-23 and again here: one-yield probe gives
   real results).
3. **FIXED (was F3) — stranded C runtime blob.** Deleted.
4. **CHANGED (was F4) — scheduler never instantiated.** It is the
   interpreter's `gt_scheduler`; but its own `_run_all` (:179-199) is still a
   stub that marks every thread DONE with result 0, and nothing calls it.

### From code inspection

5. **STILL TRUE, HIGH — no `p_gt_*` outside the interpreter.** No
   definition in `codegen_c.fd`, `stdlib/pe_runtime/`, or
   `stdlib/linux_runtime/` (grep, 2026-11-09). A `p_gt_*` program built
   with `ccompile` / `emit` has undefined symbols (not linked here).
6. **NEW, LOW — `yield` re-queues without bound.** Every yield appends to
   `ready_queue`; nothing removes finished ids except the pop in `yield`
   itself, so the queue grows with each `spawn` and yield.
7. **NEW, INFO — stale comment.** :175-177 says "The C runtime handles real
   context switching via inline asm"; there is no such C runtime.
8. **NEW, INFO — unused import** in compiler.fd:43.

## Fix order

1. Finding 1 (scheduler / `run_all` disagreement).
2. Decide whether `p_gt_*` stays: either implement it in the C and native
   runtimes (they already have `p_co_*` green threads, stdlib/task.fd) or
   remove the builtins and this module in favour of `p_co_*`.
3. Delete the unused `_run_all` / `_finish_current` / `_has_ready_threads`
   and the stale comment; drop the import in compiler.fd.

## Verification state

- 2026-11-09: whole file read; interp.fd's `p_gt_*` block read; probes with
  one and two yields run under `fid run`. C/native link failure not run
  (inspection only).
