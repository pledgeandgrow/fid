# RL17: random-choice scheduling on the C path too

llvm-roadmap RL17 (concurrency testing of the runtime: thousands of task and
thread interleavings under a scheduler that picks randomly). The native
runtimes have had `FIDES_SCHED_RANDOM=<seed>` since Nov 16
(`stdlib/pe_runtime/conc.fd`, `stdlib/linux_runtime/conc.fd`, "Random-choice
scheduling"). On Nov 17 the C runtime got the same mode.

## The C runtime's random mode

`compiler/codegen_c.fd`, section "Roadmap RL17" (before `__p_co_round`):

- `FIDES_SCHED_RANDOM=<seed>` (a decimal number, not 0) turns it on; unset, 0
  or not a number: off, and every hook costs one test of a global.
- Each pass of the scheduler (`__p_co_round`) runs the tasks in a random
  order (`__p_rnd_shuffle`, Fisher-Yates over `__p_cos`). The list is no
  longer kept in creation order in this mode; a task woken from a sleep or the
  poller goes in wherever the insertion stops.
- `__p_rnd_point` at every atomic (`p_atomic_load/store/cas/add/sub`), mutex
  lock, channel send/receive/try-receive and at a thread's start: inside a
  task, it gives up its turn one time in two when another task exists (never
  while the task holds a mutex: the other tasks run on the same thread and
  would block on it; `__p_rnd_held` counts them); outside a task (main, an OS
  thread) it sleeps 0 or 1 ms one time in four.
- The choices come from xorshift64 seeded from the seed; each thread draws its
  own sequence (seed and the order threads first draw), so a program whose
  tasks all run on one thread replays its schedule exactly from the seed. With
  several OS threads the OS still decides part of the interleaving.
- Under `P_NO_CTX` (WebAssembly, the sandbox) tasks run to their end when
  entered: only the order of the pass is random.

## Tests

- `tests/sched/sched_order.fd`: six tasks record which one ran each step
  through an atomic counter; the output is the schedule. Default C scheduler:
  `012345012345012345`.
- `tests/bootstrap/test_runtime_sched_random.fd` (Nov 16): its output must
  not change under any seed.
- `tests/sched/sched-random-check.sh` builds both with `ccompile` and `emit`,
  runs N seeds (default 300) and checks: the invariant program's three lines
  for every seed, 18 records for every seed, at least N/4 distinct schedules,
  and (C) the same schedule twice for seeds 1-20, the round-robin order with
  the mode off. Without the random mode the C path gives one schedule and
  fails.
- `python scripts/verify/interleave.py --fid $FID --sched-random --backends
  ccompile,emit --programs tests/bootstrap/test_runtime_sched_random.fd -n
  2000` passes FIDES_SCHED_RANDOM to every backend (ccompile included).

## Runs (Nov 17)

Made with the Nov 16 compiler's `dump-c`, this runtime text patched in (the
compiler itself was not rebuilt for these runs), and the native runtimes as
they are in the tree:

| Path | Seeds | test_runtime_sched_random wrong | distinct schedules of sched_order | same seed, same schedule |
|---|---|---|---|---|
| C, Linux (WSL 6.6, gcc, ucontext) | 3000 | 0 | 3000 of 3000 | yes (50 seeds) |
| C, Windows (zig cc, fibers) | 2000 | 0 | 2000 of 2000 | yes |
| pe_ir, Windows (`emit`, Nov 16 runtime) | 300 (invariant: 12) | 0 | 300 of 300 | no (a worker beside main) |

On pe_ir a random run of test_runtime_sched_random takes ~16 s: the native
mode sleeps 1 ms at one atomic in eight, and the program does 8000 atomics
(Windows sleeps round up). The C path's tasks yield instead of sleeping, so a
run there stays in milliseconds. Native Linux was not run: the Nov 16
compiler's Linux executables do not start against today's runtime.

## Left

- The fid compiler emits no loop safepoint (`p_co_tick`, the Rust compiler's
  M5 hook at loop headers): the native runtimes' "random preemption at loop
  safepoints" (`pert_co_tick`) and an equivalent on the C path never run in a
  fid-built program. A task switches only at yields, awaits, sleeps,
  channel and socket waits, and (random mode) at atomics, mutexes and
  channels. Adding the safepoint is a lowering change (compiler/lower.fd).
- Every switch point enumerated (as CHESS or loom do) is not done.
