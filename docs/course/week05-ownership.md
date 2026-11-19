# Week 5 — Memory safety I: ownership

**Reading:** book chapter 4, up to "Borrowing" (`docs/book/04-ownership.md`);
proposal 0004 (`docs/proposals/0004-ownership-and-layout.md`), sections
"Ownership" and "Alternatives"; reference §8.1, §8.3 and §8.7.

**After this week you can:** state Fid's ownership rule; predict when a
value is copied, shared and freed; explain why reference counting needs no
cycle collector in Fid; use `--leaks` to check a program frees everything;
use `@linear` for values that must be used exactly once.

## 1. The problem every systems language solves

Heap memory must be given back exactly once, and never used after. The
classic answers:

| Answer | Who decides when to free | Cost |
|---|---|---|
| Manual (`malloc`/`free`, C) | the programmer | use-after-free, double free, leaks |
| Tracing GC (Java, Go) | a collector, at run time | pauses, memory overhead, no control over timing |
| Ownership and lifetimes (Rust) | the compiler, from annotations | the programmer writes and maintains lifetimes |
| Reference counting (Swift, Python) | counts at run time | count updates; cycles leak without a collector |

Fid's answer (proposal 0004) is a single rule, applied by the compiler,
with counts for shared buffers. It works without a cycle collector
because of a restriction you will meet next week: references cannot be
stored, so a heap value can never reach itself.

## 2. The rule

1. Every value of a managed type (`str`, `Vec`, `HashMap`, and anything
   that holds one) has exactly one owner.
2. A variable owns its value. Reading it borrows it.
3. Keeping a value somewhere else (another variable, a field, an element,
   a return value) takes a copy, or moves it if that was the variable's
   last use.
4. A variable's value is dropped when its scope ends; temporaries at the
   end of their statement.
5. Parameters are borrowed: the callee neither counts nor drops them. A
   callee that changes a parameter copies it first.
6. A write to a buffer that two owners share copies the buffer first
   (copy-on-write). So a "copy" costs a count update until someone writes.

The lowering inserts every `dup` and `drop` (`compiler/lower.fd`, section
"Ownership", `LOWER_RC`). There is no `free` in safe Fid code.

```fides
// owners.fd
struct Inventory { owner: str, items: Vec<str> }

fn with_item(inv: Inventory, item: str) -> Inventory {
    var copy = inv;            // the parameter is borrowed: changing it needs a copy
    copy.items.push(item);
    return copy;
}

fn count(inv: &Inventory) -> i64 { return inv.items.len(); }

fn main() -> i64 {
    var a: Inventory = Inventory { owner: "ada", items: Vec::new() };
    a.items.push("lamp");
    val b: Inventory = with_item(a, "map");
    var c: Inventory = b;
    c.owner = "grace";
    c.items.set(0, "rope");
    println(a.owner + ": " + int_to_str(count(&a)) + ", first " + a.items.get(0));
    println(b.owner + ": " + int_to_str(count(&b)) + ", first " + b.items.get(0));
    println(c.owner + ": " + int_to_str(count(&c)) + ", first " + c.items.get(0));
    return 0;
}
```

```
$ fid run owners.fd
ada: 1, first lamp
ada: 2, first lamp
grace: 2, first rope
```

Three owners, three independent inventories: no change through one is
visible through another, though until `push` and `set` they shared their
buffers. This is *value semantics* (proposal 0001): reasoning about a
variable never requires knowing who else holds "the same" object.

`--leaks` builds the program through C with a recording allocator and
reports every block still allocated at exit:

```
$ CC="zig cc" fid run owners.fd --leaks
...
ada: 1, first lamp
ada: 2, first lamp
grace: 2, first rope
fid --leaks: every block was freed
```

`scripts/leak-check.sh` runs the programs of `tests/leaks/` this way on
every release: the ownership rule is tested, not only described.

## 3. Drop methods

A struct with a method `drop` gets it called for each local declared with
that struct type, last declared first. Today that happens when the
**function** returns, on every path, not at the end of an inner block:

```fides
// scopes.fd
struct Guard { name: str }

impl Guard {
    pub fn drop(self: Guard) {
        println("release " + self.name);
    }
}

fn step(n: i64) {
    val g: Guard = Guard { name: "step " + int_to_str(n) };
    if n == 2 {
        println("early return from step 2");
        return;
    }
    println("finishing step " + int_to_str(n));
}

fn main() -> i64 {
    val outer: Guard = Guard { name: "outer" };
    step(1);
    step(2);
    {
        val inner: Guard = Guard { name: "inner" };
        println("in block");
    }
    println("after block");
    return 0;
}
```

```
$ fid run scopes.fd
finishing step 1
release step 1
early return from step 2
release step 2
in block
after block
release inner
release outer
```

Compare with Rust and C++, where `inner` is destroyed at the closing brace.
Neither choice is unsafe here; the difference is *when* a resource comes
back. Exercise 3 asks when that matters.

## 4. Linear types: exactly once

Counting makes copies cheap. Some values must not be copied at all, and
must not be forgotten: a transaction must be committed or rolled back,
once. A struct marked `@linear` *moves*, and the type checker counts uses
(reference §8.7):

```fides
// tx.fd
@linear
struct Tx { id: i64, amount: i64 }

fn begin(id: i64, amount: i64) -> Tx { return Tx { id: id, amount: amount }; }

fn commit(t: Tx) -> i64 {
    println("commit " + int_to_str(t.id) + ": " + int_to_str(t.amount));
    return t.amount;
}

fn rollback(t: Tx) -> i64 {
    println("rollback " + int_to_str(t.id));
    return 0;
}

fn main() -> i64 {
    var balance: i64 = 100;
    val a: Tx = begin(1, 30);
    balance = balance - commit(a);
    val b: Tx = begin(2, 500);
    if b.amount > balance {
        balance = balance - rollback(b);
    } else {
        balance = balance - commit(b);
    }
    println("balance " + int_to_str(balance));
    return 0;
}
```

```
$ fid run tx.fd
commit 1: 30
rollback 2
balance 70
```

Reading a field (`b.amount`) does not consume `b`; passing `b` to a
function does. Both branches of the `if` consume it, so it counts as used.
Using it twice, or on one path only, is rejected:

```fides
// tx_twice_fail.fd
@linear
struct Tx { id: i64, amount: i64 }

fn commit(t: Tx) -> i64 { return t.amount; }

fn main() -> i64 {
    val a: Tx = Tx { id: 1, amount: 30 };
    val x: i64 = commit(a);
    val y: i64 = commit(a);
    return x + y;
}
```

```
$ fid check tx_twice_fail.fd
error: Linear type error (line 10): variable 'a' was already consumed (moved), or a part of it was.
...
```

```fides
// tx_lost_fail.fd
@linear
struct Tx { id: i64, amount: i64 }

fn commit(t: Tx) -> i64 { return t.amount; }

fn main() -> i64 {
    val a: Tx = Tx { id: 1, amount: 30 };
    if a.id == 2 { return commit(a); }
    return 0;
}
```

```
$ fid check tx_lost_fail.fd
error[E050]: Linear type error (line 8): variable 'a' is linear but was never consumed.
...
```

Linear types are the "affine plus must-use" discipline of linear logic
applied to resources. `stdlib/money.fd` and `stdlib/banking.fd` use them
so that money is neither duplicated nor lost.

## Exercises

The lab for this week is [Lab 5 — A handle that must be closed exactly once](labs/lab05-linear/README.md).

1. Draw, for each line of `owners.fd`'s `main`, the owners of each buffer
   and its count. Where is the first real copy of `a.items`' buffer made?
2. Write a function that builds a `Vec<str>` of a million strings and
   returns only its length. Run it with `--leaks`. Then return the `Vec`
   from a function and drop it in `main`: still no leak?
3. Give an example where releasing a lock at function exit instead of at
   the end of an inner block changes the behaviour of a concurrent
   program. How would you write it in Fid so that it does not matter?
4. Model a file handle as a `@linear` struct with `open`, `read_all` and
   `close`. Show that forgetting `close` on an error path is rejected.
   What does `read_all` have to return so that the handle can still be
   closed afterwards?

## Discussion

Proposal 0004 rejects a tracing collector because "pauses break
`@realtime`, `@firmware` and constant-time code", and Rust-style lifetimes
because "every user pays in annotations". What does Fid pay instead? Look
at the restriction in week 6 and the count updates in week 11's layouts.
