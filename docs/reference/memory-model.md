# Memory model

What one thread may see of another's writes. Fides uses the C++11 model, as C,
Rust, Swift and Zig do (proposal 0003).

## What the compiler checks against data races

A data race is two threads touching the same location, at least one writing,
with nothing ordering them. The goal is that safe Fides cannot have one. What
is checked today (`fid`):

- A global written by a function that runs on another thread must be written
  under a lock or through an atomic: the compiler rejects the program
  otherwise ("Race error ... writes the global ... with no lock").
- A value sent to another thread (`spawn`, `p_thread_spawn`, a channel) must be
  `@send`: numbers, `bool`, `str`, enums, `@send` structs, structs whose fields
  are all `@send`, and `Vec`/`HashMap`/`Option`/`Result`/`Box`/`Future`/
  `Mutex`/`Channel` of such types. References and raw pointers are not
  (`test_spawn_send_fail.fd`, `test_race_ptr_fail.fd`).
- Shared memory between threads is meant to be `p_shared_alloc` blocks used
  through atomics, or data behind a mutex.

Not checked: `Vec`, `HashMap` and `str` are handles, so a `Vec` passed to a
spawned call is shared with the caller, and writes to it from both threads are
not detected. Copy-on-write (proposal 0001) would make it the thread's own
copy; it is written behind `LOWER_COW` and switched off.

`unsafe` code that shares raw memory without these is responsible for its own
ordering.

## Synchronization

These operations order memory: everything a thread wrote before a *release*
is visible to a thread after the matching *acquire*.

| Release | Matching acquire |
|---|---|
| Unlocking a mutex | Locking it next |
| Sending on a channel | Receiving that value |
| `spawn f(x)` | The start of `f` |
| The end of a task | `await` on it, or the end of its `spawn scope` |
| An atomic store with `Release`, `AcqRel` or `SeqCst` | An atomic load with `Acquire`, `AcqRel` or `SeqCst` that reads it |

## Atomics

`use atomic;` gives `Atomic` (one shared 64-bit word) and the orders:

- `Relaxed`: the operation is atomic, and orders nothing else. For counters.
- `Release` (stores) / `Acquire` (loads): publication, as in the table above.
- `AcqRel`: both, for read-modify-write operations (`swap`, `compare_swap`,
  `fetch_add`).
- `SeqCst`: as `AcqRel`, and all `SeqCst` operations form one order that
  every thread agrees on.

```fides
use atomic;
val data: Atomic = atomic_new(0);
val ready: Atomic = atomic_new(0);
// writer
data.store(42, Order::Relaxed);
ready.store(1, Order::Release);
// reader
while ready.load(Order::Acquire) == 0 {}
println(data.load(Order::Relaxed));   // always 42
```

`memory_fence(order)` orders the accesses around it as an atomic with that
order would.

## On each backend

Today every atomic operation runs as `SeqCst`, which is always correct (a
stronger order than asked for):

| Backend | Load | Store | Read-modify-write |
|---|---|---|---|
| C (gcc, clang, zig) | C11 `atomic_load` (seq_cst) | `atomic_store` (seq_cst) | `atomic_fetch_add`, `atomic_compare_exchange_strong` |
| C (MSVC) | `InterlockedCompareExchange64` | `InterlockedExchange64` | `InterlockedExchangeAdd64`, `InterlockedCompareExchange64` |
| Native x86-64 | `mov` | `xchg` | `lock xadd`, `lock cmpxchg` |
| Interpreter | one thread at a time | | |

On x86-64 a `mov` load and an `xchg` store are sequentially consistent, so the
native path loses nothing. Lowering `Relaxed`, `Acquire` and `Release` to
cheaper instructions matters on ARM64 (`ldar`/`stlr` instead of full
barriers) and is left for that backend.
