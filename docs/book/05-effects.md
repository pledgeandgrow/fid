# 5. Effects and annotations

A Fid function's type says more than what it takes and returns. The
compiler also works out what it *does*: print, open files, touch the
network, allocate, panic. That set is the function's **effects**. You can
write it down, and when you do, the compiler holds the body to it.

On top of effects sit **annotations** — `@crashfree`, `@no-alloc`,
`@wrapping`, `@secret` and the rest. Some are checks the compiler proves
on every build, some change how code is generated, a few are recorded and
not yet checked. This chapter says which is which.

The contract behind all of it is `docs/reference/reference.md` §9 and §11
and `docs/reference/safety-rules.md`.

## Effects are inferred

You never have to write an effect. The compiler infers one set per
function, from the builtins its body calls and from every function those
call, to a fixpoint:

| Effect | Comes from |
|---|---|
| `IO` | printing, files, directories, processes, clocks, environment, stdin, OS randomness |
| `Net` | sockets, HTTP, TLS, DNS |
| `Spawn` | starting threads or processes |
| `Diverge` | `panic` (`p_panic`) |
| `Alloc` | string `+` and f-strings, creating and growing a `Vec` or `HashMap`, the runtime's allocation builtins (`p_vec_new`, `p_map_insert`, `p_string_concat`, …) |
| `Read`, `Write`, `Diff`, `Noise`, `Comm` | the KV-cache, autodiff, differential-privacy and multi-device builtins |

A function calling `println` has `IO`; so does every function that calls
it, however deep.

## Declaring effects

Write the set after the return type: `!IO`, `!{IO, Net}`, or `!{}` for
none. A top-level function with a declaration may not have an inferred
effect it did not declare:

```fides
// effects.fd
fn add(a: i64, b: i64) -> i64 !{} {
    return a + b;
}

fn greet(name: str) -> void !{IO, Alloc} {
    println("hello, " + name);
}

fn main() -> i64 {
    greet("effects");
    println(int_to_str(add(2, 3)));
    return 0;
}
```

```
$ fid run effects.fd
hello, effects
5
```

Now put a `println` inside `add` — or call a helper that prints, which is
the same thing to the checker:

```fides
fn log_line(s: str) -> void {
    println(s);
}

fn add(a: i64, b: i64) -> i64 !{} {
    log_line("adding");
    return a + b;
}
```

```
error: Effect error: function 'add' has inferred effect '!IO' but only declared effects not including it
```

`!{}` is the useful one: it says "this function does no I/O, touches no
network, starts nothing, allocates nothing and never panics". A `panic` inside one is
`Diverge`:

```
error: Effect error: function 'check' has inferred effect '!Diverge' but only declared effects not including it
```

so a function that may panic says so:

```fides
fn check(x: i64) -> i64 !Diverge {
    if x < 0 { panic("negative"); }
    return x;
}
```

`greet` declares `Alloc` because `"hello, " + name` builds a new string.
Since Nov 19 (known-bugs 2g.65) `Alloc` is inferred from string `+`,
f-strings, `Vec::new`, `push` and the other `Vec` and `HashMap` methods,
so a `!{}` function builds no Vecs or strings. `@no-alloc` (below) says
the same as an annotation.

### Functions that take functions

A higher-order function gets its effects **per call**, from the function
you pass. `apply` below has no effect of its own; calling it with a pure
function keeps the caller pure, calling it with a printing one does not:

```fides
// apply.fd
fn apply(f: fn(i64) -> i64, x: i64) -> i64 {
    return f(x);
}

fn inc(x: i64) -> i64 { return x + 1; }

fn noisy_inc(x: i64) -> i64 {
    println("inc " + int_to_str(x));
    return x + 1;
}

fn pure_twice(x: i64) -> i64 !{} {
    return apply(inc, apply(inc, x));     // fine: inc is pure
}

fn main() -> i64 {
    println(int_to_str(pure_twice(40)));
    println(int_to_str(apply(noisy_inc, 1)));
    return 0;
}
```

```
$ fid run apply.fd
42
inc 1
2
```

Change `pure_twice` to call `apply(noisy_inc, x)` and it fails with the
same `inferred effect '!IO'` error. A call through a function value the
compiler cannot trace (one read out of a struct, say) counts as every
effect.

### What the effect check does not do

- **Declared names are not validated.** `!{IO, Nett}` compiles; the typo
  is just a name nothing produces.
- **Methods are not checked against their own declaration.** A method's
  declared effects are compared only when a function *with* a declaration
  calls it.
- **Undeclared means unchecked.** A function with no `!…` has whatever
  effects it infers; nothing is reported.

## Capabilities: who may do IO at all

Effects describe a function. **Capabilities** restrict a program. Write
`with { … }` on an import:

```fides
// caps.fd
use file with { IO, FS };

fn main() -> i64 {
    println("reading config");
    val text: str = file_read("app.toml");
    println(int_to_str(str_len(text)));
    return 0;
}
```

As soon as any import has a `with` clause the whole program is in
**strict mode**: a call that needs a capability nobody granted is an
error. Grant only `{ IO }` above and the `file_read` call is rejected:

```
error: Capability error: 'file_read()' requires 'FS' capability (not granted to module 'main')
```

(followed today by the same complaint about calls inside `stdlib/file.fd`
itself: grant the module what it needs.)

The checked capabilities are `IO` (the print functions), `FS`
(`read_file`, `write_file`, `file_open`, `file_read`, `file_write`), `Net`
(`http_get`, `http_post`, `socket_*`), `Spawn` (`spawn`, `exec`,
`system`) and `Alloc` (`p_malloc`, `p_free`, arena save/restore). The
check is **per call, by name**: it does not follow the call graph, so a
helper of yours that prints is not itself flagged. Treat capabilities as a
coarse fence, and effects as the precise tool.

## sandbox: run code with nothing granted

`sandbox { … }` revokes everything, whatever the imports grant — and
unlike capabilities it *does* follow calls. Anything whose inferred
effects include `IO`, `Net` or `Spawn` is refused, and so is code whose
effects cannot be known (C functions, function values, inline assembly):

```fides
// sandbox_fail.fd
fn score(s: str) -> i64 {
    return str_len(s) * 10;
}

fn report(s: str) -> i64 {
    println("scoring " + s);
    return str_len(s);
}

fn main() -> i64 {
    var total: i64 = 0;
    sandbox {
        total = score("plugin");
        total = total + report("plugin");   // rejected
    }
    println(int_to_str(total));
    return 0;
}
```

```
error: Sandbox error: 'report()' has effect '!IO' -- all capabilities are revoked inside sandbox, including in the functions it calls
```

Delete the `report` line and it prints `60`. Pure computation, including
allocation, is allowed in a sandbox.

## Annotations

Annotations go before a top-level `fn` or `struct`, before a parameter,
or before a `val`/`var`. Every name is checked: a misspelt annotation is
an error, not a silent no-op. Two limits to know up front:

- **Not on methods.** `@x fn` inside an `impl` block is a parse error.
  Put the logic you want checked in a free function and call it from the
  method.
- **Not with `async`/`comptime`/`interrupt`** on the same function.

### Safety rules (checked at compile time)

These are proved by the type checker on every build — `fid run`,
`ccompile` and `emit` alike:

| Annotation | The function may not… |
|---|---|
| `@no-alloc` | allocate: no `Vec::new`, `push`, `insert`, `clone`, string `+`, allocating builtin |
| `@no-recursion` | call itself, directly or through others |
| `@no-unbounded` | contain a `while` loop (counted `for i in a..b` is fine) |
| `@no-panic` | `unwrap`/`expect`/`panic`, divide by anything but a non-zero literal, use checked `+ - *` (unless `@wrapping`), index unchecked (`v.get(i)`) |
| `@no-indirect` | call through a function value, closure or `dyn` |
| `@crashfree` | = `@no-panic` + `@no-unbounded` |
| `@bounded-stack[(N)]` | reach recursion or a call through a function value; with `N`, call deeper than `N` frames |
| `@firmware` | call anything OS-dependent (files, sockets, processes, threads, environment) |
| `@deterministic` | have `IO`/`Net`/`Spawn`/`Diverge` effects or use floating point |
| `@gas(N)` | cost more than `N` (a static count; loops with literal bounds count per trip) |

They are **transitive**: an annotated function may only call functions
that carry at least the same rules. This is the rule people hit first:

```fides
fn helper(x: i64) -> i64 { return x + 1; }

@crashfree
fn total(x: i64) -> i64 {
    return helper(x);
}
```

```
error: Safety error (crashfree): function 'total' is safety-annotated but calls 'helper',
    which has no safety annotations.
```

`@crashfree` is strict, and it is meant to be. Overflow counts as a
panic, so even `x + 1` is refused unless the function is also `@wrapping`;
division needs a literal divisor, and a guard such as `if b == 0` does not
count as proof. What passes is code like this:

```fides
// crashfree.fd
@crashfree
fn digit(x: i64) -> i64 {
    if x < 0 { return 0; }
    return x % 10;
}

@crashfree
fn bucket(x: i64) -> i64 {
    if x < 0 { return 0; }
    if x > 1000 { return 9; }
    return digit(x / 100);
}

@crashfree @wrapping
fn mix(x: i64) -> i64 {
    return x * 31 + 7;
}

fn main() -> i64 {
    println(int_to_str(bucket(457)));
    println(int_to_str(mix(9223372036854775807)));
    return 0;
}
```

```
$ fid run crashfree.fd
4
9223372036854775784
```

`@medical`/`@dal-a`, `@safety-critical` and `@autosar` are bundles of the
primitives above (see the reference §11). They are checks, not
certification: `fid qualify x.fd` prints the evidence as JSON, and its
verdict is always `NOT_QUALIFIED`.

### Arithmetic: `@wrapping`

Integer `+`, `-`, `*` and negation panic on overflow at the declared
width (`Panic: Integer overflow on addition: …`). In a `@wrapping`
function they wrap instead — what hashes and checksums want. Division by
zero still panics:

```fides
@wrapping
fn fnv1a(s: str) -> i64 {
    var h: i64 = 1469598103934665603;
    var i: i64 = 0;
    while i < str_len(s) {
        h = (h ^ str_byte_at(s, i)) * 1099511628211;
        i = i + 1;
    }
    return h;
}
```

### Secrets: `@secret`, `@declassify`, `@zeroize`

`@secret` on a parameter, local or function marks data that must never
reach an output: no print, log, file, socket, global, spawned task, call
through a function value, or unmarked return — directly or through any
helper. A branch on a secret taints everything inside it, because *which*
line prints reveals the secret. The one sanctioned exit is a function
marked `@declassify`:

```fides
// secret.fd
@declassify
fn password_ok(@secret stored: str, attempt: str) -> bool {
    return stored == attempt;
}

fn login(@secret stored: str, attempt: str) -> void {
    if password_ok(stored, attempt) {
        println("welcome");
    } else {
        println("denied");
    }
}

fn main() -> i64 {
    @zeroize val pw: str = "hunter2" + "!";
    login(pw, "guess");
    login(pw, "hunter2!");
    return 0;
}
```

```
$ fid run secret.fd
denied
welcome
```

Replace the `password_ok` call with a debug helper and a direct compare,
and both leaks are reported:

```
error: Security error (@secret) (line 6): in 'login', a secret value is passed to 'debug_dump', which outputs its parameter 's'
error: Security error (@secret) (line 8): in 'login', output 'println' runs inside a branch or loop that depends on a secret
```

`@zeroize` on a local overwrites it (and the bytes of its `str` or `Vec`)
with zeros when the function returns. The C backend wipes the heap bytes;
the interpreter wipes nothing, and native Windows builds wipe the variable
but not yet its heap bytes. `@sensitive` is the same check for personal
data, released through `stdlib/privacy.fd`. Known gaps (match guards,
early returns, some file writes) are listed under SR-13 in
`safety-rules.md`.

`@constant_time` is the companion for timing: no branch, early return,
index or division on secret data. `fid` checks the source, not the
generated machine code.

### Annotations on structs

| Annotation | Meaning |
|---|---|
| `@send` | values may cross to another thread (chapter 8); a struct whose fields are all sendable is `@send` without it |
| `@linear` | each value is used exactly once: not twice, not never, not in a loop |
| `@repr(C)` | C field layout, for FFI |
| `@packed` | no padding between fields |
| `@derive(Debug, Clone, Serialize, Default, Deserialize, PartialEq)` | generates `S_debug_str`, `S_clone`, `S_to_json`, `S_default`, `S_from_json`, `S_equals` |

### Recorded but not checked

`@sync`, `@test`, `@bounded`, `@sanitizer`, `@sink`, `@panic_handler`
(hosted builds) and `@cpu_local` are accepted by the name check and read
by no pass. `ensure` postconditions parse and are not checked; `given`
preconditions are checked only for calls whose arguments are constants.
Don't rely on any of these for safety.

## Effect handlers

`handle { … } with { … }` runs a block in which `do Effect.op(args)` (a
statement) jumps to the matching handler. The handler receives the
arguments as `i64`, and `resume(cont, v)` continues the block after the
`do`. Without `resume`, the block ends there:

```fides
// handler.fd
fn main() -> i64 {
    handle {
        println("before");
        do Log.write(7);
        println("after");
    } with {
        Log.write(code) => {
            println("handler saw " + int_to_str(code));
            resume(cont, 0);
        }
    }
    handle {
        println("before");
        do Log.write(8);
        println("never printed");
    } with {
        Log.write(code) => {
            println("handler stops at " + int_to_str(code));
        }
    }
    return 0;
}
```

```
$ fid run handler.fd
before
handler saw 7
after
before
handler stops at 8
```

Handlers are local to one function and carry only integers; `do` is not an
expression, so it cannot return a value into the block. They are an
experiment, not the way to structure I/O.

## Common mistakes

- **Declaring `!IO` on a function that builds strings.** String `+`,
  f-strings and `push` are `Alloc`: declare `!{IO, Alloc}`.
- **Annotating a method.** Annotations are not allowed inside `impl`;
  write a free function.
- **One `@crashfree` function in a sea of plain ones.** It can only call
  other safety-annotated functions. Start at the leaves and work up, or
  annotate less.
- **Guarding a division and expecting `@crashfree` to see it.** Only a
  non-zero literal divisor counts. Divide by a constant, or drop the
  annotation for that function.
- **Treating capabilities as transitive.** They are checked per call by
  name. Use `sandbox` (which does follow calls) or `!{…}` declarations
  when you need the guarantee to hold through helpers.
- **Misspelling an effect.** `!{Io}` is accepted and means nothing. The
  names are `IO`, `Net`, `Spawn`, `Diverge`, `Alloc`.
- **Printing a secret "just for debugging".** The secrecy check follows
  helpers; it will find it. Return a `@declassify`d summary instead.
