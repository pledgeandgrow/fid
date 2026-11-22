# Feature notes

Consolidated Nov 22: language and checker features, one section per roadmap item.

## Operators on structs and units of measure (CL10)

Complaint CL10 ("metres added to seconds"): units of measure checked at
compile time and free at run time. This page describes what `fid` accepts.
The code is in `compiler/typechecker.fd` (section "units of measure") and
`compiler/lower.fd` (`Lowerer_binop_plan`, `Lowerer_lower_dim_operator`).
The dimension arithmetic is in `compiler/ast.fd` (`dim_*`).

### Operators on any struct

`a + b`, `a - b`, `a * b`, `a / b` and `a % b`, where `a` is a struct, call
`a.add(b)`, `a.sub(b)`, `a.mul(b)`, `a.div(b)` and `a.rem(b)`. These are
Rust's names. The method takes `self` and one operand, which need not be
the same struct (`Money * 3`). The result has the method's return type.

```fid
struct P { x: i64, y: i64 }
impl P {
    fn add(self: P, o: P) -> P { return P { x: self.x + o.x, y: self.y + o.y }; }
}
val c: P = a + b;          // a.add(b)
```

- A missing method is an error that names it: ``no operator `-` on struct
  'P': it has no method `sub` ``.
- A method whose parameter does not accept the operand is an error:
  ``Meters.add` takes Meters, got Seconds``.
- **A struct on the right only.** When `a` is not a struct and `b` is,
  `a op b` calls `b.radd(a)`, `b.rsub(a)`, `b.rmul(a)`, `b.rdiv(a)` or
  `b.rrem(a)`, so `4 * money` is `money.rmul(4)`. The right operand is
  evaluated first, because it is the receiver.
- **Compound assignment.** `x += e` on a struct variable is `x = x + e`,
  and the same holds for `-=`, `*=`, `/=` and `%=`. It follows the same
  rules and gives the same errors. A field works too: `body.pos += step`
  is `body.pos = body.pos + step` when `body.pos` is a variable or a field
  path. Before this, a compound assignment on a struct did arithmetic on
  the struct's address.
- `vec2`/`vec3`/`vec4` keep their own operators: `+` and `-` take another
  vec of the same size, and `*` and `/` take a scalar.

### Units: dimensions computed by the compiler

A struct marked `@dimension(...)` is a unit. Its exponents are given over
the SI bases `m`, `kg`, `s`, `A`, `K`, `mol` and `cd`, each between -8 and
7. A bare base means exponent 1. A unit holds exactly one `f64`.

```fid
@dimension(m = 1)                 struct Len    { value: f64 }
@dimension(s = 1)                 struct Time   { value: f64 }
@dimension(m = 1, s = -1)         struct Speed  { value: f64 }
@dimension(m = 1, kg = 1, s = -2) struct Force  { value: f64 }
@dimension(m = 2, kg = 1, s = -2) struct Energy { value: f64 }

val e: Energy = f * d;      // Force * Len: m^2 kg s^-2
val v: Speed  = far / t;    // Len / Time
val r: f64    = ten / two;  // Len / Len cancels: a plain f64
val k: Len    = 5.0 * d;    // an f64 scales a unit from either side
val hz: Freq  = 1.0 / half; // f64 / unit: the inverse unit
```

The rules:

| Expression | Result |
|---|---|
| `unit * unit`, `unit / unit` | the unit whose dimension is the sum (difference) of the operands' dimensions; an `f64` when they cancel |
| `unit * f64`, `unit / f64`, `f64 * unit` | the same unit |
| `f64 / unit` | the unit of the inverse dimension |
| `unit + unit`, `unit - unit`, `unit % unit` | the same unit, only when both are the same unit |
| `-unit` | the same unit, its value negated |
| `unit < unit` (`<=`, `>`, `>=`) | `bool`, comparing the values; both must be the same unit |
| `unit == unit` | `bool`, by content as on any struct (CL9) |

No function has to be written for each pair of units. At run time a unit
is its `f64`: `f * d` lowers to `Energy { value: f.value * d.value }`.

Errors:

- Units of different dimensions do not add: ``units of different
  dimensions (m and s) do not add``.
- No unit declares the result's dimension. The error names the dimension
  and the annotation to write, for example `@dimension(m = 1, kg = 1, s = -1)`.
- More than one unit declares it (energy and torque are both
  m^2 kg s^-2). The result is then ambiguous. Give the left unit a `mul`
  (or `div`) method that says which unit it makes.
- An integer combined with a unit (`2 * d`). Write `2.0 * d`.
- A unit compared with another unit or a number: ``a unit compares only
  with a value of its own unit (Len)``.
- A malformed annotation (a name that is not an SI base, a base given
  twice, an exponent out of range), or a unit that does not hold exactly
  one `f64`.

A method still wins when it takes the operand. `Area.mul(self, k: f64)`
serves `a * 2.0`, and `a * len` still uses the dimension rule. A unit's
own `add` that takes something else keeps its error.

### Tests

- `tests/bootstrap/test_frontend_struct_ops.fd`, `_fail`: operator methods.
- `tests/bootstrap/test_frontend_units_ops.fd`, `_fail`: `stdlib/units.fd`'s
  impls.
- `tests/bootstrap/test_frontend_units_dims.fd`: the dimension rule,
  scaling from either side, the inverse unit, a fitting method, `rmul`.
- `tests/bootstrap/test_frontend_units_dims_{add,ambiguous,annot,field,int}_fail.fd`
  and `test_frontend_units_dims_fail.fd`: the errors above.
- `tests/bootstrap/test_frontend_struct_compound.fd`, `_fail`,
  `test_frontend_units_compound_fail.fd`: compound assignment on struct
  variables and fields.
- `tests/bootstrap/test_frontend_units_compare.fd`, `_fail`: comparisons
  and negation of units.

### Not done

- `stdlib/units.fd` does not mark its units with `@dimension` yet. Its
  derived-unit functions (`travel`, `force`, ...) still do that work.
- Ordering on structs that are not units (`p < q`) is not an operator.
  It still compares addresses: write a `cmp` method (CL9).
- Generic structs: an operator on `Pair<T>` is not looked up.

## Events and contracts (`@event`, `@emit`, `@contract`)

Roadmap Phase 9 (blockchain), feature F34. Checked by
`TypeChecker_verify_emit` (compiler/typechecker.fd); the event bodies are
written by `mn_event_body` (compiler/mono.fd).

### Events

```fid
@event
fn Transfer(from: i64, to: i64, amount: i64) {}
```

An `@event` function declares an event. Its body is empty: the compiler
writes it. Each call appends one record to the event log (standard
output), numbered in emission order across the whole program:

```
emit #1 Transfer(i64,i64,i64) topic=0x6607873a from=1 to=2 amount=50
```

The topic is the FNV-1a (32-bit) hash of the canonical signature
`Name(type,...)`: a log reader identifies an event by its signature. The
counter is the module global `__emit_seq` that mono adds when a program
declares an event.

### Declaring what a function emits

```fid
@emit(Transfer, Approval)
fn pay(l: &mut Ledger, amount: i64) -> bool { ... }
```

`@emit(...)` lists the events a function can emit through everything it
calls. The compiler checks:

| Rule | Error |
|------|-------|
| E1 an event is a free, non-generic, non-async function with no result and an empty body | `Event error: @event 'X' has a body` (and: is generic, is async, has a result type) |
| E1 its parameters are integers, `bool` or `str` | `... has parameter 'v' of type f64, which an event cannot carry` |
| E1 no two events share a topic | `events 'A' and 'B' have the same signature topic` |
| E1 an event is not a method | `method 'T.m' is an @event` |
| E2 every listed name is an event; the list is not empty | `declares @emit(X), but 'X' is not an @event function` |
| E3 a function without `@emit`/`@contract` (or a method without `@emit`) emits no event directly | `'f' emits 'X' without declaring it` |
| E4 a function with `@emit`/`@contract` emits, through everything it calls, exactly what it lists | `'f' can emit 'X' (through 'g'), which its @emit does not list`; `'f' declares @emit(X) but never emits it` |

Calls are followed through free functions (the call graph of
`pf_reach`), not through method calls: an event emitted inside a method is
checked against that method's own `@emit` (E3), but a function calling the
method does not see it.

### Contracts

```fid
@contract
@deterministic
@gas(5000)
@emit(Transfer, Approval)
fn pay(l: &mut Ledger, amount: i64) -> bool { ... }
```

A `@contract` function is a smart-contract entry point. Besides E4:

| Rule | Error |
|------|-------|
| C1 it is `@deterministic` (no IO, Net, Spawn, floating point) | `Contract error: @contract 'f' is not @deterministic` |
| C1 it has `@gas(N)` (checked by `TypeChecker_verify_gas`) | `... has no gas limit` |
| C2 no call path leads back into it before it returns (reentrancy) | `... can call itself again before it returns (through 'g'): reentrancy` |
| C3 it makes no call through a function value, anywhere it reaches | `... calls 'hook', a function value (in 'g')` |
| a contract is a free function, not a method | `method 'T.m' is a @contract` |

Tests: `test_emit` (the log, in order; a rejected payment emits nothing),
`test_emit_undeclared_fail`, `test_emit_transitive_fail`,
`test_emit_unused_fail`, `test_emit_unknown_fail`,
`test_emit_event_body_fail`, `test_emit_event_float_fail`,
`test_contract_nondeterministic_fail`, `test_contract_gas_fail`,
`test_contract_reentrancy_fail`, `test_contract_fnvalue_fail`.

Not done: events with indexed fields or a binary (ABI) encoding; the log
goes to standard output, not to a chain's receipt store; reentrancy through
method calls or trait objects is not followed.

## Typed symbols from run-time library loading

Roadmap L6C. A C function found at run time can be bound with a function
type and called through it:

```fid
val libm: i64 = p_dl_load("libm.so.6");          // ucrtbase.dll on Windows
val sq: fn(f64) -> f64 = p_dl_find(libm, "sqrt");
println(int_to_str((sq(2.25) * 100.0) as i64));  // 150
```

- **Checking** (compiler/typechecker.fd `TypeChecker_check_dl_sym`, for
  `p_dl_find` and the simulated `p_dl_sym`): the lookup takes the declared
  function type; its parameters and result must be C types (integers,
  floats, bool, str, raw pointers, void). Every call through the binding is
  checked against that type (`TypeChecker_check_fn_value_args`). Without a
  declared function type the lookup is the `i64` address, as before.
- **Calling** (compiler/lower.fd `Lowerer_dl_fn_let_type`): such a `let` is
  lowered as `addr as fn(..)`, the function value whose env holds the marker
  `__fid_c_fnptr` and the address. A call through it, or through a
  parameter it is passed to, calls the address as a plain C function, with
  no env and each argument in its own C type (compiler/codegen_c.fd,
  `CallPtr`).

### Backends

- C (`fid ccompile`): works (tests/bootstrap/test_dlsym_typed_call.fd).
- Interpreter (`fid run`): cannot call machine code, and stops with a
  message saying so. It also has no `p_dl_load`/`p_dl_find`.
- Native (`fid emit`, pe_ir and nat_ir): binding works, a call does not yet:
  their `CallPtr` passes the env first and does not know the marker (the
  marker's address is 0 there).

### Limits

Only a `let`/`val` bound straight from the lookup is made a C function
pointer. Assigning a lookup to an existing variable, returning it, or
storing it in a field is not; write `p_dl_find(lib, name) as fn(..)` there.

## Taint flow, lock order, and the aerospace and serverless checks

Five checks in `compiler/typechecker.fd` (and one rewrite in
`compiler/mono.fd`). They run in `fid` only; `fidesc` is frozen.

| Check | Rule | Rejected by | Accepted by |
|---|---|---|---|
| Taint flow (rm-P12) | A `@tainted` parameter's value reaches no sink before a sanitizer, and is not returned unsanitized | `test_taint_flow_var_fail`, `test_taint_flow_callee_fail`, `test_taint_flow_return_fail`, `test_tainted_param_fail` | `test_taint_flow`, `test_taint_flow_sanitized_call` |
| Lock order (CA4) | Locks are taken in one order across the call graph; a cycle is a compile error | `test_lock_order_fail`, `test_lock_order_param_fail`, `test_lock_order_sync_fail` (three locks, stdlib/sync, `with_read_lock`) | `test_lock_order`, `test_lock_order_sync` (`defer`red unlock, `spawn` under a held lock), `test_threads` |
| `@redundant` (F45) | Dual-channel execution with a voter; the function must be safe to run twice | `test_redundant_io_fail`, `test_redundant_global_fail`, `test_redundant_result_fail` | `test_redundant` |
| `@partitioned(P)` (F47) | ARINC 653 spatial partitioning: no global shared by two partitions, no direct call between them | `test_partitioned_shared_fail`, `test_partitioned_call_fail` | `test_partitioned` |
| `@serverless` (F32) | A handler writes no global, starts no task, and reads no global that runs code at start-up | `test_serverless_state_fail`, `test_serverless_coldstart_fail`, `test_serverless_spawn_fail` | `test_serverless` |

### Taint flow (`TypeChecker_verify_tainted`, `tf_*`)

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

### Lock order (`TypeChecker_verify_lock_order`, `lo_*`)

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

### `@redundant` (F45)

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

### `@partitioned(P)` (F47)

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

### `@serverless` (F32)

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
