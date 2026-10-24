# Secrets (`@secret`)

`@secret` marks data that must never be revealed: keys, passwords, tokens,
patient records. The compiler proves that no secret, and nothing computed
from one, reaches an output.

## Marking secrets

```fides
// A parameter: this function receives a secret and promises not to leak it.
fn sign(@secret key: str, msg: str) -> i64 { ... }

// A local.
@secret val key = load_from_vault();

// A function whose result is secret wherever it is called.
@secret
fn load_key() -> str { ... }
```

## What counts as a leak

| Leak | Example |
|------|---------|
| Output, logging, network, files | `println(key)`, `log_info(key)`, `p_socket_send(s, key)` |
| Through any helper | `fn show(x: str) { println(x); }` then `show(key)` |
| Through containers and copies | `v.push(key); println(v.get(0))`, `S { k: key }`, `val y = key + "!"` |
| Through control flow | `if key == "x" { println("yes"); }` (which line prints reveals the key) |
| Globals | `g_last = key;` |
| Other tasks | `spawn work(key)` |
| Calls through function values | `callback(key)` (the callee is unknown) |
| Return values | `return key;` from a function not marked `@secret` or `@declassify` |
| Writes through `&mut` parameters | `*out = key;` |

Code inside nested blocks, loops, closures and `unsafe { }` is checked like
any other code.

## Releasing a value on purpose

Some results derived from a secret are meant to be public, such as whether a
password matched. Write the release as a function marked `@declassify`;
callers treat its result as public:

```fides
@declassify
fn password_matches(@secret stored: str, attempt: str) -> bool {
    return stored == attempt;
}
```

Every release point is then easy to find and review (`grep @declassify`).

## How it works

Every value carries a label: whether it may hold a secret, and which of the
function's parameters it may come from. A parameter label that reaches an
output means the function outputs that parameter; that becomes part of the
function's summary, together with which parameters reach its result and
whether it produces output at all. Summaries are inferred for every function
to a fixpoint, so a secret passed to a helper is caught at the call, however
deep the helper chain. Inside a branch or loop whose condition depends on a
secret, everything carries that secret too.

The pass is `src/secrecy.rs` (Rust bootstrap) and `compiler/secrecy.fd`
(self-hosted). It runs after type checking and the reference escape check.

## Limits

The analysis is conservative: a variable's label is the union of everything
it is ever assigned, so a variable reused for a secret and then for public
data stays secret. Lengths are not treated specially (`key.len()` is
secret). Timing is a separate concern: use `@constant_time` for code whose
running time must not depend on a secret.
