# Week 9 — Effect systems II: proving what a function cannot do

**Reading:** book chapter 5, "Annotations" (`docs/book/05-effects.md`);
reference §11; `docs/reference/safety-rules.md` (every rule with its
negative test).

**After this week you can:** use `@crashfree`, `@no-alloc`,
`@no-recursion`, `@wrapping` and `@deterministic` and say exactly what
each proves; explain why the rules must be transitive; write code that
passes them; find the edge of what the checker sees.

## 1. From effects to properties

Week 8's effects say what a function *does* to the outside world. Safety
annotations make claims about its *execution*: it cannot panic, cannot
loop forever, cannot allocate, cannot recurse, gives the same output for
the same input. Each is a static check in the type checker
(`check_safety_body`, `TypeChecker_verify_safety_annotations`,
`TypeChecker_verify_deterministic`), on every build and back end.

| Annotation | The function may not |
|---|---|
| `@no-panic` | `unwrap`/`expect`/`panic`, divide by a value not proven non-zero, use checked `+ - *` (unless `@wrapping`), index unchecked |
| `@no-unbounded` | contain a `while` loop (a `for` over a range is bounded) |
| `@crashfree` | = `@no-panic` + `@no-unbounded` |
| `@no-alloc` | allocate (`Vec::new`, `push`, string `+`, allocating builtins) |
| `@no-recursion` | call itself, directly or through others |
| `@no-indirect` | call through a function value or `dyn` |
| `@bounded-stack(N)` | reach recursion or indirect calls; or call deeper than `N` frames |
| `@deterministic` | have `IO`/`Net`/`Spawn`/`Diverge` effects or use floating point |
| `@wrapping` | (a permission, not a restriction) integer `+ - *` wrap instead of panicking |

Bundles name common combinations: `@safety-critical`, `@autosar`,
`@medical`/`@dal-a` (reference §11). They are checks, not certification:
`fid qualify x.fd` prints the evidence and always concludes
`NOT_QUALIFIED`.

## 2. Code that passes

A controller step, the kind of code that runs in an interrupt or a
control loop, where a panic, an allocation or unbounded recursion is a
failure of the device:

```fides
// controller.fd
// A thermostat step: proved not to panic, loop forever, allocate or recurse.
@crashfree @no-alloc @no-recursion
fn clamp(x: i64, lo: i64, hi: i64) -> i64 {
    if x < lo { return lo; }
    if x > hi { return hi; }
    return x;
}

@crashfree @no-alloc @no-recursion @wrapping
fn error_term(target: i64, measured: i64) -> i64 {
    return target - measured;
}

@crashfree @no-alloc @no-recursion
fn heater_power(target: i64, measured: i64) -> i64 {
    val e: i64 = clamp(error_term(target, measured), -100, 100);
    if e <= 0 { return 0; }
    return e / 4;
}

@deterministic
fn schedule(hour: i64) -> i64 {
    if hour < 6 { return 160; }
    if hour < 22 { return 210; }
    return 170;
}

fn main() -> i64 {
    for hour in 0..24 {
        if hour % 6 == 0 {
            val target: i64 = schedule(hour);
            println("hour " + int_to_str(hour) + ": target " + int_to_str(target)
                + ", power " + int_to_str(heater_power(target, 185)));
        }
    }
    return 0;
}
```

```
$ fid run controller.fd
hour 0: target 160, power 0
hour 6: target 210, power 6
hour 12: target 210, power 6
hour 18: target 210, power 6
```

Three things to notice:

- `error_term` subtracts, which could overflow, so it is `@wrapping`. On
  a value the clamp then bounds, wrapping is a decision, written down.
- `e / 4` divides by a non-zero literal: allowed. `total / n` is not,
  even after `if n == 0 { return 0; }`: the checker does not use the
  guard (see below).
- `heater_power` calls `clamp` and `error_term`, which carry the same
  rules. That is required.

## 3. Transitivity

A function with a safety rule may only call functions that have the same
rule. Without that, `@no-alloc` on a function would say nothing about
the allocation in its helper, the same lesson as transitive effects in
week 8. The errors, one per rule:

```fides
// rules_fail.fd
fn helper(x: i64) -> i64 { return x + 1; }

@crashfree
fn average(total: i64, n: i64) -> i64 {
    return total / n;
}

@crashfree
fn uses_helper(x: i64) -> i64 {
    return helper(x);
}

@no-alloc
fn record(log: &mut Vec<i64>, x: i64) {
    log.push(x);
}

@no-recursion
fn fact(n: i64) -> i64 {
    if n <= 1 { return 1; }
    return n * fact(n - 1);
}

fn main() -> i64 {
    return 0;
}
```

```
$ fid check rules_fail.fd
error[E097]: Safety error (crashfree): function 'average' uses '/'.
    Division/modulo by zero aborts at runtime.
    Crashfree functions cannot use '/' unless the divisor is proven non-zero.
    Fix: use a checked-divide helper, or remove the safety annotation.
error[E097]: Safety error (crashfree): function 'uses_helper' is safety-annotated but calls 'helper',
    which has no safety annotations.
    A safety-annotated function can only call other safety-annotated functions.
    Fix: add safety annotations to 'helper', or remove the call from 'uses_helper'.
error[E097]: Safety error (safety-annotated): function 'record' is @no-alloc but uses .push(), which allocates.
    Dynamic allocation is forbidden — deterministic memory usage required.
    Fix: use a fixed-size array or a buffer passed in, or remove the safety annotation.
error[E097]: Safety error (safety-annotated): function 'fact' is @no-recursion but calls itself.
    Recursion is forbidden — stack overflow is a safety risk.
    Fix: convert to an iterative loop, or remove the safety annotation.
```

## 4. Determinism

`@deterministic` promises the same output for the same input: no I/O, no
network, no threads, no panic (week 8's effects do that part), and no
floating point, whose results can differ between CPUs and compilers
(fused multiply-add, x87 versus SSE). It is meant for consensus code,
replays and reproducible builds:

```fides
// det_fail.fd
@deterministic
fn scaled(x: i64) -> f64 {
    return (x as f64) * 1.5;
}

@deterministic
fn stamp(x: i64) -> i64 {
    println("at " + int_to_str(x));
    return x;
}

fn main() -> i64 {
    return 0;
}
```

```
$ fid check det_fail.fd
error[E097]: Determinism error: function 'scaled' is @deterministic but uses floating-point.
...
error[E097]: Determinism error: function 'stamp' is @deterministic but has effect '!IO'.
...
```

## 5. Soundness and precision

A static check can be wrong in two directions. It is **sound** if every
program it accepts has the property, and **precise** if it accepts every
program that has it. No checker of an interesting property is both
(Rice's theorem). Fid's safety rules choose soundness where they can,
and the price is precision:

- `@crashfree` rejects `total / n` even behind `if n == 0 { return 0; }`:
  the checker has no range analysis for divisors.
- `@no-unbounded` rejects every `while`, including ones that obviously
  terminate.

Where they are *not* sound is worth more attention. Two examples to
start from, both found while writing these notes against the Nov 16
build: the float check of `@deterministic` looks for float *literals*, so
a function that only casts (`(a as f64) / (b as f64)`) passes; and a
method cannot carry an annotation at all (reference §11), so the logic to
be checked must live in a free function.

## Exercises

The lab for this week is [Lab 9 — Functions the compiler proves cannot crash](labs/lab09-crashfree/README.md).

1. Write `safe_div(a: i64, b: i64) -> Option<i64>` that `@crashfree`
   accepts. Then an `average(v: &Vec<i64>) -> Option<i64>` that it
   accepts. Which `Vec` methods could you use, and which not?
2. Make `fact` pass `@no-recursion` and `@crashfree` for `n` in `0..=20`,
   returning `Option<i64>` for anything else.
3. Add `@bounded-stack(3)` to `heater_power`. Does it pass? Add a fourth
   level of calls and read the error.
4. Show the soundness gap of §5 with a program, and write the rule that
   would close it (which kinds of expression must the float scan look
   at?). Find `scan_for_floats` in `compiler/typechecker.fd` and check
   your answer against it.
5. `fid qualify controller.fd` prints JSON evidence. Which parts of it
   would a certification authority ask for that the compiler cannot
   produce?

## Discussion

Safety standards such as MISRA C, DO-178C and ISO 26262 forbid recursion,
dynamic allocation after start-up and unbounded loops in the most
critical code. They are enforced by review and external tools. What
changes when the compiler enforces them on every build? What is lost
when a project needs an exception to a rule?
