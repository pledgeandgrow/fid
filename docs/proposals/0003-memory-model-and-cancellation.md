# 0003: A memory model for atomics, and structured cancellation

- **Status:** accepted
- **Decision:** accepted on 2026-11-02 (competitive LG11, LG12).

## Problem

`p_atomic_*` exist, but nothing says what a thread may observe: whether an
atomic store makes earlier plain writes visible to a thread that reads it, and
in which order. Code that is correct on x86 can break on ARM. And a task can
be cancelled, but cancelling a parent does not cancel its children, and a
deadline does not travel with the work.

## Change

**Memory model** — the C++11 model, which C, Rust, Swift and Zig share:

- A data race on a non-atomic location is impossible in safe code: a global
  written by a thread needs a lock or an atomic (already checked), and values
  sent to a task are `@send` and copied (proposal 0001).
- Atomics: `atomic_load(p, order)`, `atomic_store(p, v, order)`,
  `atomic_swap`, `atomic_compare_swap`, `atomic_add`, `atomic_fence(order)`,
  with `order` one of `Relaxed`, `Acquire`, `Release`, `AcqRel`, `SeqCst`
  (default `SeqCst`). A `Release` store and an `Acquire` load that reads it
  make every write before the store visible after the load.
- Locks, channels, `spawn` and `await` are synchronization points: they
  release and acquire.
- Every backend maps orders to real instructions (C11 `<stdatomic.h>`, `lock`
  prefixed x86 instructions and `mfence` natively, `ldar`/`stlr` on ARM64).

**Structured cancellation:**

- `scope { ... }` owns the tasks spawned in it: it does not end until they all
  have, and an error or panic in one cancels the others.
- `cancel_scope(deadline) { ... }`: cancelling the scope or reaching the
  deadline cancels every task under it, recursively.
- A cancelled task stops at its next suspension point (`await`, channel
  operation, sleep, socket wait) with `Err(Cancelled)`; `defer` blocks run.
- `with_timeout(ms, f)` is `cancel_scope` with a deadline.

## What it breaks

`p_atomic_*` without an order mean `SeqCst`, as most do today; nothing else.

## Alternatives

A Go-like model (sequential consistency for everything synchronized, no
orders): simpler, but loses `Relaxed` counters and `Acquire`/`Release`
publication, which lock-free code needs.

## Implementation

Runtime first (both C generators: `<stdatomic.h>` under C11, MSVC
`_Interlocked*` otherwise; the PE runtime's `pert_atomic_*`), then the
stdlib `sync` API, then the cancellation tree in the task runtime (each task
records its scope; cancel walks the tree; suspension points check the flag).
Tests: message passing with `Release`/`Acquire` on threads, a counter with
`Relaxed`, a scope whose child panics, nested deadlines, cancellation during a
socket wait, on all backends.
