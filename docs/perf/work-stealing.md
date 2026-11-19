# Work stealing: per-worker lock-free run queues (competitive RT10)

Where: `stdlib/pe_runtime/conc.fd` "Run queues" (native Windows, pe_ir,
the default `fid emit`) and `stdlib/linux_runtime/conc.fd` "Worker rings"
(the machine layer: Linux AArch64 / RISC-V 64 / x86-64 and `--nat` Windows).

## Before

Every worker had a local queue, but all queues (local and global) were
linked lists under one lock (`pert_wq` in pe_runtime; one global list under
`pe_q_lock` in linux_runtime). A spawn, a yield, a pop and a steal each took
that lock, and a steal took one task.

## Now

- Each worker owns a bounded ring of 256 task slots, a Chase-Lev deque taken
  FIFO (as Go's `runq` and Tokio's local queue). Only its worker pushes, at
  `bottom`, with plain stores and one fenced store that publishes the slot
  (`pe_atomic_store` is `xchg`; `nat_atomic_store` is `stlr` + `dmb` / fenced
  on RISC-V). Its worker and thieves take at `top` by compare-and-swap, so a
  slot is claimed once; no lock is taken. FIFO rather than the paper's
  owner-LIFO: a task that yields goes behind the others on its ring.
- A worker whose ring is dry takes the global queue's head and its share of
  the rest (one worker's part, at most half a ring) in one lock hold, then
  steals half of a sibling's ring in one CAS: the slots are copied to its own
  ring's unpublished end, then claimed; a failed claim copies again. Victims
  are tried from a rotating start.
- A full ring moves its older half (claimed first, then read: only the owner
  writes slots) and the new task to the global queue in one lock hold.
- The global queue holds what threads that are not workers queue (the poller,
  the blocking pool, main), and in linux_runtime every sleeper (the scan
  that skips sleepers until they are due stays on the global queue) and
  everything in random mode (RL17).
- Parking: a worker counts itself idle (a locked add, a barrier), looks at
  the rings once more, and only then sleeps. A push publishes its slot before
  it reads the idle count, so either the pusher wakes the worker or the
  worker sees the task.
- Deadlock detector (pe_runtime): unchanged two-cycle rule; "queue empty" is
  now the global queue plus a sum over the rings. A task made but not yet
  queued, or run inline by `async` on a thread that is not a worker, no
  longer looks stuck (`pert_offq`): main spawning 200,000 tasks while the
  workers drained the queue panicked with "Tasks are deadlocked" before.
- Task registry (pe_runtime): a finished task's registry slot is in its
  block ([29]); finishing scanned the registry, which made tasks finishing
  out of spawn order quadratic (the tree phase below: 72 us a task at
  200,000 tasks, 4 us after).

The C runtime (`compiler/codegen_c.fd`) has no shared run queue to replace:
its tasks are a cooperative scheduler per OS thread (`P_THREAD_LOCAL`
`__p_cos`), so they never move between cores. Giving it M:N scheduling is a
separate, larger change.

## Tests and benchmark

- `tests/bootstrap/test_task_steal.fd`: fan-out from one task (5,000 tasks
  through a 256-slot ring: spills and steals), a spawn tree (8,191 tasks),
  3,000 tasks from main through the global queue, each yielding once.
  Prints counts and sums; a lost or duplicated task changes them, a lost
  wake-up hangs or panics.
- `tests/ctx_switch/steal_bench.fd [tasks] [work]`: the same three shapes
  with timings (ns per task) and a check of every result.

Run each many times and with several worker counts:

    $FID emit tests/bootstrap/test_task_steal.fd -o steal.exe
    for i in $(seq 50); do ./steal.exe | md5sum; done | sort | uniq -c
    for w in 1 2 3 16; do FIDES_WORKERS=$w ./steal.exe | md5sum; done
    FIDES_SCHED_RANDOM=7 ./steal.exe        # slow (random 1 ms sleeps), same output
    $FID emit tests/bootstrap/test_task_steal.fd --nat -o steal_nat.exe
    NATIVE=1 FID=$FID JOBS=4 bash scripts/cross-test.sh aarch64-linux

    $FID emit tests/ctx_switch/steal_bench.fd -o steal_bench.exe
    ./steal_bench.exe 200000 100
    FIDES_WORKERS=1 ./steal_bench.exe 200000 100

## Measurements (Nov 17, stage-0 compiler with today's stdlib)

Measured on a 16-thread Windows machine that was saturated by other builds
(several compilers at once, memory exhausted: some runs died with "out of
memory"), so these numbers are noisy and only show the order of magnitude.

`steal_bench.exe 200000 100`, pe_runtime, ns per task, three runs each:

| phase   | shared lock (before) | rings (now)          |
|---------|----------------------|----------------------|
| fan-out | 4538 / 5614 / 4988   | 5094 / 4350 / 4596   |
| tree    | 4057 / 3680 / 4724   | 4234 / 4000 / 4151   |
| global  | 15530 / 66765 / crash| 18124 / 36301 / 11501|

("before" here is the old queues with this change's registry and detector
fixes, so only the queues differ; without the registry fix the tree phase
was 72,000-108,000 ns per task, and the global phase always panicked with
a false deadlock.) At about 4-5 us a task the cost is dominated by making
and finishing a task (its stack, the registry lock `pert_reg`, the
`pert_done` broadcast at each finish), not by the run queues: the queue
change does not show above the noise here. To see the queues alone,
measure on a quiet machine, and then the next step is the per-task locks.

Correctness (pe_runtime): `test_task_steal` 20/20 runs identical, with
`FIDES_WORKERS` 1, 2, 3, 16 (5 runs each) identical, `FIDES_SCHED_RANDOM=7`
identical (112 s: random sleeps); test_tasks, test_task_handoff,
test_tasks_10k, test_runtime_sched_random, test_machine_tasks_once match
their expected output 5/5; the async tests and test_pe_task_order,
test_spawn_scope give the same output on 5 runs.

linux_runtime: built with `--nat` (the same machine layer on Windows
x86-64), `test_task_steal` 9/9 identical with the default and 1, 2, 3, 8
workers. That needed `FID_NAT_OFF=purecse`: the stage-0 compiler's pure-call
CSE (compiler/rules.fd) merges two `task_run()` calls (and two
`sync_atomic_load(g)` of the same global) in one function, so the second
`task_run()` returned at once, with the old queues as with the new ones.
The Linux targets were not run: stage 0 cannot build them with today's
runtime (`undefined symbol: nat_tls_set` from its x86-64 and AArch64 ELF).
