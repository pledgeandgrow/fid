[All modules](index.md)

# task -- Fides Documentation

## Contents

- [task_go](#task_go) (function)
- [task_yield](#task_yield) (function)
- [task_run](#task_run) (function)
- [task_sleep](#task_sleep) (function)
- [task_wait_read](#task_wait_read) (function)
- [task_wait_write](#task_wait_write) (function)
- [task_detach](#task_detach) (function)
- [task_done](#task_done) (function)
- [task_result](#task_result) (function)
- [task_pool_call](#task_pool_call) (function)
- [TaskScope](#TaskScope) (struct)
- [task_scope_new](#task_scope_new) (function)
- [task_scope_go](#task_scope_go) (function)
- [task_scope_flag_ptr](#task_scope_flag_ptr) (function)
- [task_scope_cancelled](#task_scope_cancelled) (function)
- [task_scope_cancel](#task_scope_cancel) (function)
- [task_join_timeout](#task_join_timeout) (function)
- [task_scope_join](#task_scope_join) (function)
- [task_scope_cancel_join](#task_scope_cancel_join) (function)
- [task_timeout](#task_timeout) (function)
- [TASK_CANCEL_SLICE:](#TASK_CANCEL_SLICE:) (const)
- [task_token_new](#task_token_new) (function)
- [task_current_token](#task_current_token) (function)
- [task_token_with_timeout](#task_token_with_timeout) (function)
- [task_token_cancel](#task_token_cancel) (function)
- [task_token_cancelled](#task_token_cancelled) (function)
- [task_token_remaining](#task_token_remaining) (function)
- [task_sleep_cancellable](#task_sleep_cancellable) (function)
- [task_wait_read_cancellable](#task_wait_read_cancellable) (function)
- [task_wait_write_cancellable](#task_wait_write_cancellable) (function)
- [task_recv_cancellable](#task_recv_cancellable) (function)
- [task_join_cancellable](#task_join_cancellable) (function)
- [task_scope_sleep](#task_scope_sleep) (function)

## task_go

Start the function `name`, which takes one i64, as a task. It starts on a
worker as soon as one is free. Returns the task.


*Example (native only) -- run as a test by `scripts/doctest.py`:*

```fides
fn square(x: i64) -> i64 { return x * x; }
fn main() -> i64 {
    val t = task_go("square", 7);
    task_run();
    assert(task_done(t));
    assert_eq(task_result(t), 49);
    return 0;
}
```

```fides
pub fn task_go(name: str, arg: i64) -> i64 {
```

**Parameters:**

- `name`: `str`
- `arg`: `i64`

**Returns:** `i64`

*Defined at line 32*

---

## task_yield

Let the other tasks run. Outside a task it does nothing.

```fides
pub fn task_yield() -> void {
```

**Returns:** `void`

*Defined at line 37*

---

## task_run

Wait until every task has finished, tasks they start included.
Returns how many tasks there have been.

```fides
pub fn task_run() -> i64 {
```

**Returns:** `i64`

*Defined at line 43*

---

## task_sleep

Let `ms` milliseconds pass; the other tasks run meanwhile.

```fides
pub fn task_sleep(ms: i64) -> void {
```

**Parameters:**

- `ms`: `i64`

**Returns:** `void`

*Defined at line 48*

---

## task_wait_read

Wait until the socket `fd` has data, at most `timeout_ms` (-1: no limit). The
other tasks run meanwhile. Returns false on a timeout.

```fides
pub fn task_wait_read(fd: i64, timeout_ms: i64) -> bool {
```

**Parameters:**

- `fd`: `i64`
- `timeout_ms`: `i64`

**Returns:** `bool`

*Defined at line 54*

---

## task_wait_write

Wait until the socket `fd` can take data, at most `timeout_ms` (-1: no limit).

```fides
pub fn task_wait_write(fd: i64, timeout_ms: i64) -> bool {
```

**Parameters:**

- `fd`: `i64`
- `timeout_ms`: `i64`

**Returns:** `bool`

*Defined at line 59*

---

## task_detach

Nobody will read this task's result: free it when it finishes.

```fides
pub fn task_detach(t: i64) -> void {
```

**Parameters:**

- `t`: `i64`

**Returns:** `void`

*Defined at line 64*

---

## task_done

True once the task has finished.

```fides
pub fn task_done(t: i64) -> bool {
```

**Parameters:**

- `t`: `i64`

**Returns:** `bool`

*Defined at line 69*

---

## task_result

What the task returned.

```fides
pub fn task_result(t: i64) -> i64 {
```

**Parameters:**

- `t`: `i64`

**Returns:** `i64`

*Defined at line 74*

---

## task_pool_call

Run the program function `name` (same signature rules as spawn targets)
on the scheduler's blocking-call pool while this task parks. For wrapping
syscalls that can neither be polled nor made nonblocking. Off a task it
just calls the function. `args` is an i64 array of arguments; returns what
the function returned.

```fides
pub fn task_pool_call(name: str, nargs: i64, args: i64) -> i64 {
```

**Parameters:**

- `name`: `str`
- `nargs`: `i64`
- `args`: `i64`

**Returns:** `i64`

*Defined at line 83*

---

## TaskScope

A group of tasks started together, joined together.

```fides
pub struct TaskScope {
```

**Fields:**

| Name | Type |
|------|------|
| `tasks` | `Vec<i64>` |
| `flag` | `i64,  // shared cancel flag (0/1); one per scope` |

*Defined at line 102*

---

## task_scope_new

An empty scope.

```fides
pub fn task_scope_new() -> TaskScope {
```

**Returns:** `TaskScope`

*Defined at line 109*

---

## task_scope_go

Start `name(arg)` as a child of the scope.

```fides
pub fn task_scope_go(s: &mut TaskScope, name: str, arg: i64) -> i64 {
```

**Parameters:**

- `s`: `&mut TaskScope`
- `name`: `str`
- `arg`: `i64`

**Returns:** `i64`

*Defined at line 116*

---

## task_scope_flag_ptr

The scope's cancel flag as an i64 address: hand it to a child's arg (or
pack it into a struct) so the child can poll
`sync_atomic_load(flag_ptr) != 0` and stop early.

```fides
pub fn task_scope_flag_ptr(s: &TaskScope) -> i64 {
```

**Parameters:**

- `s`: `&TaskScope`

**Returns:** `i64`

*Defined at line 125*

---

## task_scope_cancelled

True once someone cancelled the scope. Children poll this.

```fides
pub fn task_scope_cancelled(s: &TaskScope) -> bool {
```

**Parameters:**

- `s`: `&TaskScope`

**Returns:** `bool`

*Defined at line 130*

---

## task_scope_cancel

Ask every child (cooperatively) to stop. Does not wait.

```fides
pub fn task_scope_cancel(s: &mut TaskScope) -> void {
```

**Parameters:**

- `s`: `&mut TaskScope`

**Returns:** `void`

*Defined at line 135*

---

## task_join_timeout

Wait until the task finishes, at most `ms` (-1: no limit).
Returns its result, or -1 on a timeout; the task keeps running.
Poll granularity is ~1 ms.

```fides
pub fn task_join_timeout(t: i64, ms: i64) -> i64 {
```

**Parameters:**

- `t`: `i64`
- `ms`: `i64`

**Returns:** `i64`

*Defined at line 142*

---

## task_scope_join

Wait for every child of the scope. Returns the results in spawn order.
Children that spawned more children are covered too: task_run drains the
scheduler before the per-handle results are read.

```fides
pub fn task_scope_join(s: &mut TaskScope) -> Vec<i64> {
```

**Parameters:**

- `s`: `&mut TaskScope`

**Returns:** `Vec<i64>`

*Defined at line 167*

---

## task_scope_cancel_join

Cancel the scope, then wait for every child to observe the flag and end.
Returns the results of children that finished (spawn order; a child that
ignores cancellation still has its result once it ends).

```fides
pub fn task_scope_cancel_join(s: &mut TaskScope) -> Vec<i64> {
```

**Parameters:**

- `s`: `&mut TaskScope`

**Returns:** `Vec<i64>`

*Defined at line 181*

---

## task_timeout

Run `name(arg)` as a task and wait at most `ms` for it. Returns the
result, or -1 on a timeout (the task keeps running).

```fides
pub fn task_timeout(name: str, arg: i64, ms: i64) -> i64 {
```

**Parameters:**

- `name`: `str`
- `arg`: `i64`
- `ms`: `i64`

**Returns:** `i64`

*Defined at line 188*

---

## TASK_CANCEL_SLICE:

How often a cancellable wait looks at its token, in ms.

```fides
pub const TASK_CANCEL_SLICE: i64 = 10;
```

*Defined at line 216*

---

## task_token_new

A new token, not cancelled, with no deadline. Three words, as a
`timeout(ms) { }` block's token: the third, its parent, is 0 (none).

```fides
pub fn task_token_new() -> i64 {
```

**Returns:** `i64`

*Defined at line 220*

---

## task_current_token

The token of the innermost `timeout(ms) { }` block around the running
code (its task's token slot), 0 outside any. Valid only inside that
block: the block frees it at its end (docs/reference/timeout-blocks.md).

```fides
pub fn task_current_token() -> i64 {
```

**Returns:** `i64`

*Defined at line 231*

---

## task_token_with_timeout

A token that cancels itself `ms` milliseconds from now. Its deadline is
on p_clock_mono_ms's clock (C6), as a timeout block's: a change of the
wall clock neither fires it early nor holds it back.

```fides
pub fn task_token_with_timeout(ms: i64) -> i64 {
```

**Parameters:**

- `ms`: `i64`

**Returns:** `i64`

*Defined at line 246*

---

## task_token_cancel

Cancel the token: every wait on it returns.

```fides
pub fn task_token_cancel(tok: i64) -> void {
```

**Parameters:**

- `tok`: `i64`

**Returns:** `void`

*Defined at line 257*

---

## task_token_cancelled

True once the token, or a token it nests in (its parent chain: an
inner timeout block's token names the outer one), is cancelled or past
its deadline. The chain is followed 64 deep at most, as the runtimes do.

```fides
pub fn task_token_cancelled(tok: i64) -> bool {
```

**Parameters:**

- `tok`: `i64`

**Returns:** `bool`

*Defined at line 266*

---

## task_token_remaining

Milliseconds left before the nearest deadline of the token and the
tokens it nests in (-1: no deadline; 0 once cancelled).

```fides
pub fn task_token_remaining(tok: i64) -> i64 {
```

**Parameters:**

- `tok`: `i64`

**Returns:** `i64`

*Defined at line 286*

---

## task_sleep_cancellable

Sleep `ms`, or less if the token is cancelled first. True when the
whole time passed, false when the token stopped it.

```fides
pub fn task_sleep_cancellable(ms: i64, tok: i64) -> bool {
```

**Parameters:**

- `ms`: `i64`
- `tok`: `i64`

**Returns:** `bool`

*Defined at line 326*

---

## task_wait_read_cancellable

task_wait_read that gives up when `tok` is cancelled: 1 the socket has
data, 0 timed out, -1 cancelled.

```fides
pub fn task_wait_read_cancellable(fd: i64, timeout_ms: i64, tok: i64) -> i64 {
```

**Parameters:**

- `fd`: `i64`
- `timeout_ms`: `i64`
- `tok`: `i64`

**Returns:** `i64`

*Defined at line 365*

---

## task_wait_write_cancellable

task_wait_write that gives up when `tok` is cancelled (1, 0, -1 as
task_wait_read_cancellable).

```fides
pub fn task_wait_write_cancellable(fd: i64, timeout_ms: i64, tok: i64) -> i64 {
```

**Parameters:**

- `fd`: `i64`
- `timeout_ms`: `i64`
- `tok`: `i64`

**Returns:** `i64`

*Defined at line 371*

---

## task_recv_cancellable

The next value of channel `ch`; None once the token is cancelled or the
channel is closed and empty.

```fides
pub fn task_recv_cancellable(ch: i64, tok: i64) -> Option<i64> {
```

**Parameters:**

- `ch`: `i64`
- `tok`: `i64`

**Returns:** `Option<i64>`

*Defined at line 377*

---

## task_join_cancellable

Wait until task `t` finishes or the token is cancelled: true when it
finished (task_result has its result).

```fides
pub fn task_join_cancellable(t: i64, tok: i64) -> bool {
```

**Parameters:**

- `t`: `i64`
- `tok`: `i64`

**Returns:** `bool`

*Defined at line 396*

---

## task_scope_sleep

Sleep `ms` unless the scope is cancelled first: true when the whole time
passed.

```fides
pub fn task_scope_sleep(s: &TaskScope, ms: i64) -> bool {
```

**Parameters:**

- `s`: `&TaskScope`
- `ms`: `i64`

**Returns:** `bool`

*Defined at line 411*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
