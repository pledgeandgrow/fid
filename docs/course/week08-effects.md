# Week 8 — Effect systems I: inference, declarations, polymorphism

**Reading:** book chapter 5, up to "Capabilities" (`docs/book/05-effects.md`);
reference §9.1 and §9.4; `docs/reference/language-guide.md` §5.1.

**After this week you can:** say what an effect system adds to a type
system; predict the effects the compiler infers for a function; declare
effects and read the errors when a body breaks them; explain
effect-polymorphic higher-order functions; write a small effect handler.

## 1. Types say what, effects say how

`fn f(x: i64) -> i64` says what `f` takes and returns. It does not say
whether `f` prints, reads a file, opens a socket, starts a thread or
panics. Those are its **effects**. An effect system tracks them in the
type checker, so that "this function only computes" is a fact the
compiler checks rather than a comment.

Fid's effects (reference §9.1):

| Effect | Produced by |
|---|---|
| `IO` | printing, files, directories, processes, clocks, environment, stdin |
| `Net` | sockets, HTTP, TLS, DNS |
| `Spawn` | threads and processes |
| `Diverge` | `panic` |
| `Alloc` | the runtime's allocation builtins |
| `Read`, `Write`, `Diff`, `Noise`, `Comm` | KV cache, autodiff, differential privacy, multi-device builtins |

Effects are **inferred**: each function gets the union of the effects of
the builtins it calls and of the functions it calls, computed to a
fixpoint over the call graph (`TypeChecker_compute_func_effects`). Nobody
has to write them.

A function may **declare** its effects after the return type: `!IO`,
`!{IO, Net}`, or `!{}` for none. A declaration is checked: an inferred
effect that is not declared is an error. `!{}` is how Fid says *pure*.
There is no `@pure` annotation, and the compiler says so rather than
ignoring it, because every annotation name is checked:

```fides
// pure_fail.fd
@pure
fn sq(x: i64) -> i64 { return x * x; }

fn main() -> i64 {
    println(int_to_str(sq(3)));
    return 0;
}
```

```
$ fid check pure_fail.fd
error: Unknown annotation '@pure' on 'sq': no compiler pass consumes this name
    and it will be silently ignored. Check the spelling.
```

## 2. Functional core, imperative shell

A common design is to keep all decisions in effect-free functions and
all I/O in a thin layer that calls them. Declaring the core `!{}` turns
the design into a checked property:

```fides
// core_shell.fd
// The functional core: no effects, declared and checked.
fn celsius_to_f(c: i64) -> i64 !{} {
    return c * 9 / 5 + 32;
}

fn classify(f: i64) -> str !{} {
    if f < 50 { return "cold"; }
    if f < 77 { return "mild"; }
    return "hot";
}

fn fold(v: Vec<i64>, f: fn(i64, i64) -> i64, init: i64) -> i64 {
    var acc: i64 = init;
    for x in v { acc = f(acc, x); }
    return acc;
}

fn add(a: i64, b: i64) -> i64 { return a + b; }
fn max(a: i64, b: i64) -> i64 { if a > b { return a; } return b; }

fn summary(temps: Vec<i64>) -> str !{} {
    val hottest: i64 = fold(temps, max, -1000);
    val mean: i64 = fold(temps, add, 0) / temps.len();
    return "mean " + int_to_str(celsius_to_f(mean)) + "F, hottest " + classify(celsius_to_f(hottest));
}

// The imperative shell: all the IO is here.
fn report(day: str, temps: Vec<i64>) -> void !IO {
    println(day + ": " + summary(temps));
}

fn main() -> i64 {
    var mon: Vec<i64> = Vec::new();
    mon.push(8);
    mon.push(14);
    mon.push(11);
    report("mon", mon);
    var tue: Vec<i64> = Vec::new();
    tue.push(22);
    tue.push(31);
    report("tue", tue);
    return 0;
}
```

```
$ fid run core_shell.fd
mon: mean 51F, hottest mild
tue: mean 78F, hottest hot
```

Effects are **transitive**: a helper's `println` is the caller's `IO`, at
any depth. This is where the system earns its keep, because the print is
two calls away from the declaration it breaks:

```fides
// hidden_io_fail.fd
fn trace(s: str) {
    println("trace: " + s);
}

fn scale(x: i64) -> i64 {
    trace("scale");
    return x * 10;
}

fn total(a: i64, b: i64) -> i64 !{} {
    return scale(a) + scale(b);
}

fn main() -> i64 {
    println(int_to_str(total(1, 2)));
    return 0;
}
```

```
$ fid check hidden_io_fail.fd
error[E110]: Effect error: function 'total' has inferred effect '!IO' but only declared effects not including it
```

A panic is an effect too (`Diverge`), so a function declared `!{}` cannot
panic explicitly. (Overflow and bounds checks are not counted as
`Diverge`; week 9's `@no-panic` covers those.)

```fides
// diverge_fail.fd
fn checked_div(a: i64, b: i64) -> i64 !{} {
    if b == 0 { panic("division by zero"); }
    return a / b;
}

fn main() -> i64 {
    println(int_to_str(checked_div(10, 2)));
    return 0;
}
```

```
$ fid check diverge_fail.fd
error[E110]: Effect error: function 'checked_div' has inferred effect '!Diverge' but only declared effects not including it
```

The fix is to say so, `-> i64 !Diverge`, or better to return a `Result`.

## 3. Effect polymorphism

What are the effects of `fold`? It depends on `f`. A naive system would
give `fold` every effect `f` *could* have, and then no pure function could
call it. Fid gives a higher-order function the effects of the function
passed **at each call** (`TypeChecker_register_ho`,
`TypeChecker_passed_fn_effects`): in `core_shell.fd`, `fold(temps, max, …)`
is pure because `max` is. Pass a printing function and the same call is
`IO`:

```fides
// ho_fail.fd
fn fold(v: Vec<i64>, f: fn(i64, i64) -> i64, init: i64) -> i64 {
    var acc: i64 = init;
    for x in v { acc = f(acc, x); }
    return acc;
}

fn noisy_add(a: i64, b: i64) -> i64 {
    println("adding " + int_to_str(b));
    return a + b;
}

fn sum(v: Vec<i64>) -> i64 !{} {
    return fold(v, noisy_add, 0);
}

fn main() -> i64 {
    var v: Vec<i64> = Vec::new();
    v.push(1);
    println(int_to_str(sum(v)));
    return 0;
}
```

```
$ fid check ho_fail.fd
error[E110]: Effect error: function 'sum' has inferred effect '!IO' but only declared effects not including it
```

The precise rule: a call through a function *parameter* adds nothing to
the callee; each call of the callee adds the effects of the argument (a
named function's inferred effects, a closure body's, nothing when the
caller forwards its own parameter). A function value the compiler cannot
trace, such as one read from a struct field, counts as every effect.

## 4. What the effect check does not do

Read reference §9.1 for the full list. In short: unknown effect names in
a declaration are accepted (a typo is a silent no-op); `impl` methods are
not checked against their own declarations, only when a declared function
calls them; a function without a declaration is not checked at all; and
`Alloc` is not produced by `Vec` and string operations written in Fid in
`core.fd`, so `!{}` does not mean "allocates nothing" (`@no-alloc` does,
week 9).

## 5. Effect handlers

Algebraic effects go one step further: an effect is an operation the
code *performs* (`do E.op(args)`) and a handler further out *interprets*.
The handler receives the rest of the block as a continuation `cont`;
`resume(cont, v)` runs it, and not resuming abandons it (an exception):

```fides
// handlers.fd
fn main() -> i64 {
    handle {
        println("step 1");
        do Log.note(1);
        println("step 2");
        do Log.note(2);
        println("done");
    } with {
        Log.note(n) => {
            println("  (note " + int_to_str(n) + ")");
            resume(cont, 0);
        }
    }
    handle {
        println("validating");
        do Check.reject(7);
        println("never printed");
    } with {
        Check.reject(code) => {
            println("rejected with code " + int_to_str(code));
        }
    }
    return 0;
}
```

```
$ fid run handlers.fd
step 1
  (note 1)
step 2
  (note 2)
done
validating
rejected with code 7
```

Fid's handlers are an experiment and small: local to one function,
integer arguments only, `do` is a statement. In the Nov 16 build only a
`do` written directly in the `handle` block captures the rest of the
block; a `do` nested in an `if` or a loop runs its handler and then
always continues, as if it resumed. Koka, Eff and OCaml 5 show the full
design.

## Exercises

The lab for this week is [Lab 8 — A pure core, an effectful shell, and closures](labs/lab08-effects/README.md).

1. Add a `debug: bool` parameter to `summary` that prints intermediate
   values. What happens to its declaration, and to `report`'s? Redesign
   so the core stays `!{}` and the shell does the printing.
2. Write `map_vec(v: Vec<i64>, f: fn(i64) -> i64) -> Vec<i64>` and call it
   from a `!{}` function with a closure that captures a multiplier. Then
   with a closure that prints. Explain each result with the rule of §3.
3. Put a function value in a struct field and call it from a `!{}`
   function. What does the compiler infer, and why is that the only safe
   answer?
4. Read the test programs under `tests/bootstrap/effects/` and find one
   effect the inference misses or over-approximates (§4 is a start).
   Write a minimal program that shows it, and say whether the miss is
   sound (it can only reject good programs) or unsound.

## Discussion

Java's checked exceptions are an effect system for one effect, and most
Java programmers learned to hate them. Why? What does Fid do differently
(inference, polymorphism, opt-in declarations), and does it avoid the
same fate?
