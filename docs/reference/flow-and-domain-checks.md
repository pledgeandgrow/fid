# Taint flow, lock order, and the aerospace and serverless checks

Five checks in `compiler/typechecker.fd` (and one rewrite in
`compiler/mono.fd`). They run in `fid` only; `fidesc` is frozen.

| Check | Rule | Rejected by | Accepted by |
|---|---|---|---|
| Taint flow (rm-P12) | A `@tainted` parameter's value reaches no sink before a sanitizer, and is not returned unsanitized | `test_taint_flow_var_fail`, `test_taint_flow_callee_fail`, `test_taint_flow_return_fail`, `test_tainted_param_fail` | `test_taint_flow`, `test_taint_flow_sanitized_call` |
| Lock order (CA4) | Locks are taken in one order across the call graph; a cycle is a compile error | `test_lock_order_fail`, `test_lock_order_param_fail`, `test_lock_order_sync_fail` (three locks, stdlib/sync, `with_read_lock`) | `test_lock_order`, `test_lock_order_sync` (`defer`red unlock, `spawn` under a held lock), `test_threads` |
| `@redundant` (F45) | Dual-channel execution with a voter; the function must be safe to run twice | `test_redundant_io_fail`, `test_redundant_global_fail`, `test_redundant_result_fail` | `test_redundant` |
| `@partitioned(P)` (F47) | ARINC 653 spatial partitioning: no global shared by two partitions, no direct call between them | `test_partitioned_shared_fail`, `test_partitioned_call_fail` | `test_partitioned` |
| `@serverless` (F32) | A handler writes no global, starts no task, and reads no global that runs code at start-up | `test_serverless_state_fail`, `test_serverless_coldstart_fail`, `test_serverless_spawn_fail` | `test_serverless` |

## Taint flow (`TypeChecker_verify_tainted`, `tf_*`)

A port of `fidesc`'s `verify_taint_propagation`, with calls followed into
user functions.

- **Source:** a parameter marked `@tainted`. Each such parameter is checked
  on its own, so the error names it.
- **Propagation:** through `val`/`var` and assignment, every expression form
  (arithmetic, concatenation, f-strings, struct, array and enum literals,
  `if`/`match` values, casts), and into a variable that a tainted value is
  stored in (`v.push(t)`, `o.f = t`, `a[i] = t`). A plain assignment at the
  top level of the function replaces the variable's taint. Inside a branch
  or a loop, an assignment can only add taint. Loop bodies are walked twice,
  so taint carried from one iteration to the next is seen.
- **Calls:** a user function is analysed with the taint of its arguments,
  memoised per function and argument mask. Its result is tainted if a
  tainted parameter reaches its return. A builtin that is not a sink returns
  the taint of its arguments.
- **Sanitizers:** functions marked `@sanitizer`, and the names `sql_escape`,
  `html_escape`, `shell_escape`, `path_normalize`, `sanitize`,
  `sanitize_input`, `escape_html`, `escape_sql`, `escape_shell` and
  `normalize_path`. A sanitizer's result is clean.
- **Sinks:** functions marked `@sink`; output (`print`, `println`, `eprintln`,
  `p_print*`, `log_*`); files and directories (`p_file_*`, `p_fs_write*`,
  `p_dir_*`, `write_file`, `file_read`/`file_write`/`file_append`); the
  network (`p_socket_*`, `p_net_*`, `p_http_*`, `p_tls_*`, `p_dns_*`);
  processes (`p_process_*`, `p_system`, `p_spawn_process`, `process_system`,
  `shell_cmd`); and the SQL and HTML builders (`sql_query`, `sql_literal`,
  `db_query`, `db_exec`, `html_literal`). A sink reached inside a callee is
  reported with the callee's name.
- **Return:** returning a value derived from a `@tainted` parameter is an
  error. (Function-level `@tainted` is rejected as misplaced, so no function
  is allowed to return taint.)
- **Not tracked:** control dependence. Taint is about integrity, so a branch
  on untrusted input does not taint what the branch does. Secrecy, where
  control dependence matters, is `@secret` (`compiler/secrecy.fd`). Also not
  tracked: closures' bodies, impl methods (a method call returns the taint of
  its receiver and arguments, and is never a sink), and recursion beyond the
  first level (a recursive call with the same mask is assumed to return its
  taint and reach no sink).

Earlier, a `@tainted` parameter made the whole function reject any IO or Net
effect (release L1). The data-flow check rejects only real flows: printing a
sanitized value, or branching on a sanitized length, is accepted
(`test_taint_flow`).

## Lock order (`TypeChecker_verify_lock_order`, `lo_*`)

Each function gets a summary: the locks it takes, directly or through its
callees, and the order edges "holding A, it takes B".

- **Locks** are named by the expression the lock call receives: a global, a
  parameter (replaced by the caller's argument when the summary is applied),
  a local of the function, or a field of one of these.
- **Lock calls:** `p_mutex_lock`, `sync_mutex_lock`, `sync_rwlock_read_lock`
  and `sync_rwlock_write_lock`, with their unlocks. `with_lock(m, f)` and its
  rwlock forms take `m` around the named function `f`.
- **Threads:** a `spawn`ed call and each branch of `parallel { }` start with
  no lock held.
- **Branches:** a lock that a branch or loop body takes and does not release
  is not held after the body. An unlock in a `defer` block does not release
  the lock before the function ends (the lock stays held for what follows).
- **Cycles:** the order edges between named locks form a graph. The first
  cycle found is reported with each edge's function and line. An edge that
  comes from a parameter also names the call that supplied the locks.
- **Cost:** summaries are recomputed round by round until none changes. They
  only grow, and there is no recursion over the call graph. A program
  without locks takes one round, and each round is linear in program size.
- **Not seen:** locks held through a pointer (`p_mutex_lock(*p)`), locks in
  collections, lock methods, and function values other than `with_lock`'s
  named function. Taking the same lock twice is not reported. Taking a
  non-recursive mutex twice deadlocks at run time, and the run-time detector
  ("Tasks are deadlocked") still covers it.

## `@redundant` (F45)

`compiler/mono.fd` (`mn_push_redundant`) turns `f` into three functions:

- `f_chan_a` and `f_chan_b`, two copies of the body;
- `f`, a voter that calls both with the same arguments, compares the
  results, and calls `p_panic` (a fail-safe stop) when they differ.

`TypeChecker_verify_redundant` requires that running the body twice is sound:

- the result is a scalar (an integer, a float or a bool);
- the function is not generic and not `async`;
- no parameter is `&mut` or `*mut`;
- the function has no IO, Net, Spawn, Write, Noise or Comm effect;
- nothing it reaches writes a global or starts a task.

**Not done:** placing the two channels on separate cores (core affinity). Both
channels run on the calling thread, one after the other.

## `@partitioned(P)` (F47)

`@partitioned(P)` marks a function as an entry of partition `P`. Without an
argument, the partition is named after the function.

The code of a partition is its entries plus every function they reach that
is not itself a partition entry. That shared library code is counted in
every partition that reaches it.

Two rules are checked:

- A mutable global reached (read or written) by the code of two partitions
  is shared memory between them, which is an error.
- A partition that calls another partition's entry is an error. Data passes
  through ARINC 653 queuing and sampling ports (`stdlib/arinc.fd`), handed
  in as values.

**Not done:** temporal partitioning, which needs a partition schedule
emitted by the backend. Memory reached through pointers or references that
the scheduler passes to two partitions is also not checked.

## `@serverless` (F32)

A `@serverless` function is a request handler. The checks cover the handler
and every function it reaches.

- **No global writes.** An instance can be recycled, frozen or run many times
  at once, so module state does not reliably survive between requests.
- **No tasks.** A `spawn`, or a thread or process spawn, is rejected because
  the frozen instance may never finish the task. Structured
  `spawn scope { }` and `parallel { }` are allowed.
- **No start-up code.** A handler may not read a global whose initializer
  runs code at start-up, because every cold start pays for it before the
  first request. Allowed initializers are literals, constants, expressions
  of these, empty constructors (`X_new()`) and `comptime` values.

**Not done:** a cold-start benchmark of an emitted handler, and the build
side of F32 (tree-shaking for a handler entry point, WASM output). Both
belong to the driver and the backends.
