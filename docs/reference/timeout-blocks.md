# Timeout blocks: `timeout(ms) { } else { }`

Complaint-roadmap CA8 ("timeouts are bolted on"). A block that gives up
waiting after a time, without handing a token to every wait inside it.

```fid
use task;

fn fetch_all() -> i64 {
    timeout(500) {
        // every wait in here (and in what it calls) that respects
        // cancellation gives up 500 ms after the block started
        val ok: bool = task_sleep_cancellable(2000, task_current_token());
    } else {
        println("gave up");   // the deadline passed while the body ran
    }
    return 0;
}
```

## What it does

- `timeout(ms) { body }` makes a token that cancels itself `ms`
  milliseconds after the block starts (the token of stdlib/task.fd's
  "Cancellation tokens", CA7) and installs it in the running task's
  **token slot** for the body. A wait that respects cancellation finds it
  there (`p_task_token()`, `task_current_token()`), also in functions the
  body calls.
- The body always runs. Cancellation is a checkpoint, not a kill (CA7's
  decision): a wait returns early, the code after it decides what to do.
- `else { late }` is optional. It runs after the body when the deadline
  passed, or the token was cancelled, while the body ran — also when no
  wait noticed it (the body just took too long).
- When the body ends — at its end or by `return`, `break`, `continue` or
  `?` — the slot gets back the token it held before. Blocks nest: the
  inner block's token records the outer one as its parent.
- Every task has a slot of its own, empty (0) when the task starts: a task
  started inside a block does not inherit its deadline. Code outside tasks
  has one slot per thread.
- The time is an integer number of milliseconds (`timeout("x") { }` is a
  type error). `timeout` is a contextual word: only `timeout( .. ) {`
  starts a block, so functions, variables and fields named `timeout` keep
  working (tests/bootstrap/test_timeout_name.fd).

## How it is built

- Parser (`parse_timeout_stmt`): ordinary statements, so every pass sees
  ordinary code. `timeout(ms) { body }` becomes
  `if __timeout_block(ms) { body }`; with `else`,
  `if true { if __timeout_block(ms) { body } if __timeout_expired() { late } }`.
  The type checker types both markers as `bool` (and checks `ms`); to the
  checks the body is conditional code, which is conservative.
- Lowering (`Lowerer_lower_timeout`, lower.fd "CA8: timeout blocks"): the
  token is three words from `p_malloc`: `[cancelled][deadline ms][parent]`,
  stored and read with `p_atomic_*`. Enter: deadline = `p_clock_mono_ms() +
  ms` (a monotonic clock, C6: a change of the wall clock moves no
  deadline), `parent = p_task_set_token(tok)`. Leave: "late" = cancelled or
  `now >= deadline`, then `p_task_set_token(parent)` and `p_free(tok)`.
  Return, break, continue and `?` leave the blocks they jump out of
  (`Lowerer_to_unwind`).
- Runtimes: `p_task_token()` and `p_task_set_token(t) -> old` read and set
  a word of the running task (C `__p_co.tok`, native task block words,
  the interpreter's task), or the thread's slot outside tasks.

## Limits

- The block's token lives as long as the block: do not keep
  `task_current_token()` past it (it is freed at the block's end).
- Waits that ignore tokens (a blocking `sync_channel_recv`, a raw
  `p_co_sleep`) still ignore the block; see stdlib/task.fd for the waits
  that respect it.
- Inside a txn block, `abort;` and effect handlers' jumps out of a timeout
  block do not put the old token back (only return, break, continue and
  `?` do).

Tests: tests/bootstrap/test_timeout_block.fd (run, ccompile, emit),
test_timeout_block_type_fail.fd, test_timeout_name.fd.
