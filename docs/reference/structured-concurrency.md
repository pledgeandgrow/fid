# Structured concurrency: no task left behind

Complaint roadmap CA6 ("goroutine and task leaks"). A task cannot outlive
the function that started it unless the program says so by name.

`fid` accepts a `spawn f(x)` (a thread, or a green task for an async
function) in three cases:

| Form | Accepted because |
|------|------------------|
| `val t = spawn f(x); ... await t` | the handle is read later in the function: awaited, joined, passed on, returned or stored |
| `spawn scope { spawn f(x); ... }` | the scope joins every task its body starts (§10.4 of reference.md) |
| `spawn detached f(x)` | the task is let go by name |

Anything else is an error:

```
spawn work(1);                  // the handle is dropped
val t = spawn work(2);          // `t` is never read again
```

```
error: Concurrency error (line 12): the task started here in 'main' is never
awaited, joined or detached, so it could outlive the function that started it.
    Fix: keep its handle and await it (`val t = spawn f(x); ... await t`), start
    it inside `spawn scope { ... }` (the scope joins its tasks), or detach it by
    name: `spawn detached f(x)`.
```

## `spawn detached`

`detached` is a contextual word: only `spawn detached <name>` is the detach
form, so `spawn detached(5)` still spawns a call of a function named
`detached`. The parser makes the form the call
`__spawn_detached(spawn f(x))` (compiler/parser.fd, `parse_unary`); every
check of a spawn (arguments `@send`, copies for the new thread, effects)
applies to the inner `spawn` as before. Its value is `void`: there is no
handle to await.

Lowering (compiler/lower.fd `Lowerer_lower_spawn_detached`) starts the task
as `spawn f(x)` does. A green task (an async function started through its
word adapter: one with an `f64` or struct argument) is then detached with
`p_co_detach`, so its block is freed when it finishes. A thread runs to its
end with nobody waiting for it; note that the end of a `spawn scope` joins
every thread the program has started, detached ones included (the C
runtime's `p_executor_run_all`).

## How the check works

compiler/typechecker.fd: `Stmt::Expr` of a `spawn` outside a scope is an
error at once. A `val`/`var` initialised by a `spawn` outside a scope is
recorded (`ca6_names`, `ca6_lines`); a read of that name anywhere later in
the function (the `Expr::Ident` arm of `check_expr`) clears it, and
`TypeChecker_check_func_body` reports the ones left at the end of the body.
`ca6_scope_depth` counts the `spawn scope` bodies around the statement.

Limits: a handle assigned to an existing variable (`h = spawn f(x)`) is not
followed; reading a variable of the same name after shadowing counts as a
read; calling an async function directly (which also starts a task) and
dropping its future is not checked.

Tests: tests/bootstrap/test_spawn_detached.fd (accepted, runs),
test_spawn_unjoined_fail.fd and test_spawn_unjoined_handle_fail.fd
(rejected), test_spawn_scope.fd (a scope).
