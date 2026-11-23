# Tasks in `fid run`: interleaving and cancellation (LG11)

`fid run` used to run a task to its end the moment it started: the
interpreter had one call stack, so a task that slept, waited on a channel or
on a cancellation token kept the whole program waiting, and a token could
only stop a task that had not started. Since LG11 the interpreter's tasks
interleave as the C and native runtimes' do.

## What switches

A task runs until it waits, then another runs. The waits are the
checkpoints of `stdlib/task.fd`, the same on every backend:

| Call | Interpreter builtin | Switches |
|---|---|---|
| `task_yield()` | `p_co_yield` | always (in a task) |
| `task_sleep(ms)`, every `*_cancellable` wait | `p_co_sleep` | always; the time really passes |
| `await f`, joining a `spawn` | `p_future_poll`, `p_task_poll`, `p_thread_join`, `p_executor_block_on` | until the task has ended |
| `task_run()` | `p_co_run` | until every task has ended |
| a channel receive with nothing to receive | `p_chan_recv` | until a value comes, the channel is closed, or nobody is left to send |
| `p_chan_try_recv`, `p_task_is_ready`, `p_task_poll_nonblocking` that find nothing | | one turn for the others |
| `task_wait_read/_write` | `p_co_wait_fd` | one turn (the interpreter's sockets are simulated and always ready) |

`task_go` (`p_co_go`) queues the task: it starts at the next switch, as the
C runtime starts it. An `async fn` call (`p_co_async`) and `spawn f(x)`
(`p_thread_spawn`) enter the task at once; it runs until it first waits and
then its caller goes on.

## How

`compiler/interp.fd`, section "Interleaved tasks". Each interpreted task
runs on a task of the C runtime fid itself is built with
(`interp_task_entry`), so its interpreter frames live on a stack of their
own. Only one runs at a time (the C runtime's tasks share one thread). The
interpreter's state is shared; what belongs to one call chain (`InterpCtx`:
the call stack, the allocas of its frames, its regions, its unsafe depth,
the running task) is set aside in the locals of the call that switches and
put back when it resumes (`Interpreter_host_switch`).

An interpreted call takes about 90 KB of the C stack in a fid built at -O0,
so a task needs a stack as deep as the main thread's (256 MB, as
`scripts/bootstrap.sh` gives main). The C runtime has
`p_co_stack(bytes)` for that (the stack of the tasks started from then on;
only address space is reserved, pages are committed as the stack grows;
`MAP_NORESERVE` on Linux). fid is built by the previous fid, whose runtime
may not have it, so the interpreter asks for it by name:
`p_thread_spawn("__p_co_stack", 1, bytes)` answers

- the size asked before (>= 0) from a runtime that has it: tasks
  interleave, `--max-recursion` limits a task as it limits main;
- -1 from an older C runtime (no such function): tasks interleave on its
  default task stack (1 MB on Windows), each task's calls kept within 640 KB
  of it (`Interpreter_itask_stack_check`: past that, `Panic: Task stack
  exhausted ...`); where that is too little (256 KB on Linux and macOS)
  tasks run to their end when they start, as before;
- -2 from the native runtimes and from a C runtime without a task switch
  (WebAssembly): tasks run to their end when they start. The native
  runtimes run tasks on several workers at once, which the interpreter's
  shared state does not allow.

A run recorded with `fid run --record` (and its replay) also keeps the old
order: a replay needs one.

At most 256 tasks have a stack at a time; more `task_go` tasks wait for one
to end (`test_tasks.fd` starts 10,000), and an async call or `spawn` past
that runs to its end at once.

## No kill: cancellation stops a task at its next checkpoint

Fid has no preemptive kill, on any backend. A task stopped mid-way would
skip the drops of what it owns (a file, a lock, a buffer), and nothing
unwinds (reference §8.2: no drop or cleanup code runs after a panic either).
Cancelling is a request:

- `task_token_cancel(tok)`, a token's deadline (`task_token_with_timeout`),
  or a scope's flag (`task_scope_cancel`) is seen by the task at its next
  checkpoint: `task_sleep_cancellable` returns false,
  `task_recv_cancellable` returns `None`, `task_wait_*_cancellable` and
  `task_join_cancellable` say so;
- the task then returns along its normal path, so its drops and its `defer`
  blocks run (`test_task_interleave.fd`, part 5);
- a task that never reaches a checkpoint is not stopped: it runs to its
  end, and joining it waits for it.

This is the "forced stop at the next checkpoint": the stop happens where the
task already expects to wait, and it leaves the task in a state its own code
cleans up.

## Tests

`tests/bootstrap/test_task_interleave.fd` (every backend: `fid run`,
`fid ccompile`, `fid emit`): two tasks' prints interleave around their
sleeps, a task waits for values main sends after starting it, a cancelled
token stops a sleeping task at once, a deadline ends a receive, a cancelled
task's `defer` runs, an async call returns to its caller at its first wait,
and a spawned function waits on a channel main fills afterwards. Before
LG11 `fid run` printed `a1 a2 b1`, `ping-pong: none none`,
`cancelled: 1 fast false` and a garbage consumer value.
