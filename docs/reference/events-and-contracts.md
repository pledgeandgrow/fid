# Events and contracts (`@event`, `@emit`, `@contract`)

Roadmap Phase 9 (blockchain), feature F34. Checked by
`TypeChecker_verify_emit` (compiler/typechecker.fd); the event bodies are
written by `mn_event_body` (compiler/mono.fd).

## Events

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

## Declaring what a function emits

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

## Contracts

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
