# Waits that respect cancellation, and the task chain of a panic

Complaint-roadmap CA7 (with the slot of CA8) and CA9, in the runtimes.

## The token slot

Every task has a token slot, and so does the code outside tasks on each
thread. `p_task_token()` reads it and `p_task_set_token(t)` replaces it,
giving back what it held. A `timeout(ms) { }` block (CA8,
docs/reference/timeout-blocks.md) puts its own token there for the length of
the block. A new task starts with an empty slot, whatever its starter's
holds.

A token is three words: `[cancelled (0/1)][deadline in ms on
p_clock_now_ms's clock, 0 for none][the token the slot held before]`. A
token counts as cancelled when its first word is set, when its deadline has
passed, or when either holds for a token further down the chain (word 2).
So an inner block also stops at an outer block's deadline.

| Runtime | Slot of a task | Slot outside tasks |
|---|---|---|
| C (compiler/codegen_c.fd) | `__p_co.tok` | `__p_tok_outside` (thread-local) |
| pe_ir (stdlib/pe_runtime/conc.fd) | word [30] of the task block | TLS slot `pert_tls_tok`, made on first use |
| machine layer (stdlib/linux_runtime/conc.fd) | word [28] of the task block | word [10] of the thread block (base.fd `pe_tls`) |
| interpreter (compiler/interp.fd) | `ITask.token` | `g_itask_tok_outside` |

The interpreter never passes these calls on to the runtime it runs on. That
runtime's slot belongs to the interpreter itself.

## The waits

When the slot holds a token, the builtin waits wait in slices: at most
10 ms, and never past the first deadline on the chain. Between slices they
check the token and give up once it is cancelled:

| Wait | Result when the token stops it |
|---|---|
| `p_co_sleep(ms)` (`task_sleep`) | returns 1 (0: the whole time passed) |
| `await f` (`p_future_poll` of a task) | returns 0; the awaited task goes on, and nothing waits for it any more |
| `p_chan_recv` (`sync_channel_recv`) | the pending value, as for a closed, empty channel |
| `p_chan_send` (`sync_channel_send`) | false, as for a closed channel |
| `p_co_wait_fd` (`task_wait_read/_write`, `p_co_recv`, `p_co_accept`) | 0, not ready |

With an empty slot every wait behaves as it did before: one load and one
test.

While a wait sleeps in slices, the other tasks keep running. A task gives
up its turn. Code outside tasks runs the tasks queued on its thread in the
C runtime and the machine layer, and sleeps in pe_ir, where workers run the
tasks.

A cancelled `await` gives the zero word. That word means nothing for a
future whose result is a string, a struct or a Vec. Check `timeout`'s `else`
(or the token) before using such a result.

Thread joins (`p_thread_join`, or awaiting a `spawn` that runs on an OS
thread) still wait to the end.

The library forms in stdlib/task.fd ("Cancellation tokens":
`task_sleep_cancellable` and the others) take a token as an argument and
work the same way.

Tests: `tests/bootstrap/test_task_wait_cancel.fd`, which runs on every
backend; `tests/cancel/send_full.fd`, compiled backends only, because the
interpreter's channels have no capacity; and `tests/cancel/check.sh`, which
runs both on each backend.

## The task chain of a panic (CA9)

After a panic's trace, the runtime prints the tasks the panicking one was
running for:

    Panic: b failed
    ...
    task chain: b <- a <- main

To build the chain, it starts at the running task and takes the task that
awaits it. If no task awaits it, it takes the task whose turn it runs in, as
with an `async` call that has not suspended yet. It stops at the code
outside tasks: `main`, or `(thread)` off the main thread (the C runtime and
the machine layer). If neither task exists, it ends with `(started by f)`,
the task that started it. A trailing `__task` (an adapted task function) is
left out of the names.

| Runtime | Records | Prints |
|---|---|---|
| C | `__p_co` name, waiter, parent_name | `__p_co_print_chain`, through `__p_trace_tasks` |
| machine layer | task block [25] name, [26] waiter, [27] starter's name (1: a thread), [29] the task it runs inside | `pe_co_print_chain(2)` from base.fd `pert_panic` and `pe_on_signal` |
| pe_ir | task block [31] name, [32] starter's name, [33] waiter; [15] the task it runs inside | `pert_task_chain()`, called by pe_ir's `p_panic` stub (compiler/pe_ir_stubs.fd `PeirGen_gen_panic`) |

Test: `tests/bootstrap/test_task_chain_fail.fd`. The C path is checked by
tests/memprof/check.sh and the native paths by tests/cancel/check.sh.
