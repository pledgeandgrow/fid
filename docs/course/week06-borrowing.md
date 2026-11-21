# Week 6 — Memory safety II: borrowing and second-class references

**Reading:** book chapter 4 from "Borrowing" (`docs/book/04-ownership.md`);
reference §8.4; `docs/reference/language-guide.md` §4.4 and §4.6.

**After this week you can:** state the borrow rules and the escape rules;
explain why second-class references need no lifetime annotations in most
code, and what lifetime annotations add when you write them; turn a
rejected program into an accepted one without `unsafe`.

## 1. Why references at all

Week 5's rule copies (lazily) whenever a value is kept. A function that
must *change* its caller's value, or read a large one without counting
it, takes a reference instead: `&T` to read, `&mut T` to change. Two
checks keep references safe:

- the **borrow checker** (`compiler/borrowck.fd`) forbids conflicting
  access while a reference is in use;
- the **escape checker** (`compiler/escape.fd`) forbids a reference from
  outliving what it points to.

## 2. The borrow rules

1. `&mut x` is an error while any other borrow of `x` (or of a related
   path such as a field of `x`) is active.
2. `&x` is an error while `&mut x` is active.
3. Changing `x` is an error while a borrow of `x` is active.

A borrow ends at the borrower's **last use**, not at the end of the block
(non-lexical lifetimes). Borrows of different fields (`&p.left`,
`&p.right`) do not conflict.

Rule 1 is what prevents aliasing bugs such as this transfer, where `from`
and `to` would be the same account:

```fides
// alias_fail.fd
fn transfer(from: &mut i64, to: &mut i64, n: i64) {
    *from = *from - n;
    *to = *to + n;
}

fn main() -> i64 {
    var acct: i64 = 100;
    transfer(&mut acct, &mut acct, 10);
    println(int_to_str(acct));
    return 0;
}
```

```
$ fid check alias_fail.fd
error[E080]: borrow error in main (line...
...
```

The full message reads "cannot create &mut acct: a mutable borrow is
already reserved on it or a related path", and names the first `&mut acct`
as the holder. (The Nov 16 build prints `line 0` for this conflict, which
is on line 9.) Because two `&mut` can never alias, the optimizer may keep
`*from` in a register across the write to `*to`. This is the same
argument that lets Fortran and Rust compilers vectorize loops C compilers
must be careful with.

## 3. The escape rules: references are second-class

A reference may be passed to a call, held in a local, and returned when it
comes from a reference parameter. It may **not** be stored: not in a
struct field, an enum, a `Vec`, a `Box`, a global, a closure that escapes,
or another task. Nor may it outlive the block of what it points to. The
one exception is a struct with a lifetime parameter,
`struct View<'a> { r: &'a i64 }`: it may hold a reference in a field, and
a value of it is then held like a reference (reference §8.4).

What is allowed covers most uses of references:

```fides
// pick.fd
struct Pair { left: i64, right: i64 }

fn pick(p: &mut Pair, left: bool) -> &mut i64 {
    if left { return &mut p.left; }
    return &mut p.right;
}

fn smaller<'a>(a: &'a Pair, b: &'a Pair) -> &'a Pair {
    if a.left + a.right <= b.left + b.right { return a; }
    return b;
}

fn main() -> i64 {
    var p: Pair = Pair { left: 1, right: 2 };
    val r: &mut i64 = pick(&mut p, false);
    *r = *r + 40;
    println(int_to_str(p.left) + " " + int_to_str(p.right));
    val q: Pair = Pair { left: 5, right: 5 };
    val s: &Pair = smaller(&p, &q);
    println("smaller sums to " + int_to_str(s.left + s.right));
    val a: &i64 = &p.left;
    val b: &i64 = &p.right;
    println(int_to_str(*a + *b));
    return 0;
}
```

```
$ fid run pick.fd
1 42
smaller sums to 10
43
```

`pick` returns a reference into its parameter: allowed, and the caller's
`p` stays borrowed while `r` is used. Without the `'a` on `smaller`, the
compiler infers from the body that the result borrows from both
parameters; with it, the signature says so and the body is checked
against it.

What is rejected:

```fides
// keep_fail.fd
fn main() -> i64 {
    var seen: Vec<&i64> = Vec::new();
    var x: i64 = 1;
    seen.push(&x);
    return 0;
}
```

```
$ fid check keep_fail.fd
error: Reference error (line 3): in 'main', local 'seen' has type 'Vec<&i64>', which stores a reference; references can only be passed to calls, held in locals and returned
...
error: Reference error (line 5): in 'main', a reference cannot be passed to method 'push', which may keep it
...
```

```fides
// scope_fail.fd
fn main() -> i64 {
    var a: i64 = 1;
    var r: &i64 = &a;
    {
        var b: i64 = 2;
        r = &b;
    }
    println(int_to_str(*r));
    return 0;
}
```

```
$ fid check scope_fail.fd
error: Reference error (line 7): in 'main', 'r' would outlive 'b': 'b' is freed at the end of its block while 'r' is still in scope
...
```

A closure that captures a reference is treated like the reference:

```fides
// closure_fail.fd
fn make_reader() -> fn() -> i64 {
    var total: i64 = 41;
    val r: &i64 = &total;
    return fn() -> i64 { return *r + 1; };
}

fn main() -> i64 {
    val read = make_reader();
    val n: i64 = read();
    println(int_to_str(n));
    return 0;
}
```

```
$ fid check closure_fail.fd
error: Reference error (line 5): in 'make_reader', 'make_reader' returns a reference to local 'total', which is freed when it returns
...
```

And a lifetime annotation is a promise the body must keep:

```fides
// lifetime_fail.fd
fn first<'a>(x: &'a i64, y: &i64) -> &'a i64 {
    return y;
}

fn main() -> i64 {
    val a: i64 = 1;
    val b: i64 = 2;
    println(int_to_str(*first(&a, &b)));
    return 0;
}
```

```
$ fid check lifetime_fail.fd
error: Reference error (line 3): in 'first', 'first' returns a borrow of parameter 'y', but its signature ties the result to 'a; give 'y' that lifetime or return a borrow of a parameter that has it
...
```

## 4. What the restriction buys

Since no heap value can hold a reference, **references only point down
the call stack**, into frames that are still live. That gives three
things at once:

- no reference can dangle, and the check is local to one function plus
  the signatures it calls;
- only a struct that holds a reference needs a lifetime parameter, so
  most programs never write a lifetime;
- the heap graph has no reference edges, so it cannot have cycles through
  references, and reference counting (week 5) frees everything.

The cost is expressiveness: an iterator or parser struct over borrowed
data (`struct View<'a>`) lives only in locals and calls, never in a `Vec`
or on the heap, and there is no graph of `&Node`. The idioms
that replace them (book chapter 4, "What to do instead"): store an index
or key, keep an owned copy, or pass the reference down the call tree.

## Exercises

The lab for this week is [Lab 6 — A graph without references between nodes](labs/lab06-graph/README.md).

1. Fix each `_fail.fd` file of this week without `unsafe`, keeping what
   the program prints the same (or explaining why it cannot be kept).
2. Write a tokenizer that returns a `Vec<Token>` where each token records
   the start and end offset of its text in the input, instead of a
   reference to it. Compare its memory use with one that stores an owned
   `str` per token.
3. A graph of nodes: write `struct Graph { nodes: Vec<Node> }` with edges
   as indexes. Implement depth-first search. What does the borrow checker
   say if you hold `&mut g.nodes` while calling a method on `g`?
4. Find in `compiler/borrowck.fd` where the "last use" of a borrower is
   computed. Construct a program whose borrow ends earlier than its
   block, and one where a borrow inside a loop must last for the whole
   loop.

## Discussion

Second-class references are an old idea (Algol 60's call-by-name
parameters, Ada's `in out`, Swift's `inout`). Rust made references
first-class and paid with lifetimes. Which programs from your own
experience would be hard to write under Fid's rule? Which would become
simpler?
