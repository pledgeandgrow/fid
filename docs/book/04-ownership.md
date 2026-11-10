# 4. Ownership and memory

This is the chapter that decides whether Fides fits your brain. Read it
even if you skim everything else.

## One owner

Every value has exactly one owner. Assigning or passing by value *moves*:

```fides
var a: Vec<i64> = Vec::new();
a.push(1);
var b = a;               // a has moved — using a here is an error
```

When the owner goes out of scope, the value is freed. No GC, no
`free()` calls in your code, no leaks from forgetting.

## Borrowing

`&T` reads, `&mut T` mutates — checked at compile time:

```fides
fn total(v: &Vec<i64>) -> i64 {
    var s: i64 = 0;
    var i: i64 = 0;
    while i < v.len() { s = s + v.get(i); i = i + 1; }
    return s;
}

fn bump(v: &mut Vec<i64>) -> void {
    v.push(1);                      // allowed — exclusive borrow
}
```

The rules (K4/K5 in the checker): many `&` or one `&mut`, never both;
no borrow outlives its owner.

`--borrowck=on` is the default. `=warn` downgrades errors to warnings;
`=off` disables — don't ship with it off.

## The rule that surprises everyone: references can't escape

A `&T` cannot be stored in a struct, returned from a function, sent to a
task, or captured by a closure that outlives its scope. `compiler/escape.fd`
enforces it.

```fides
struct Ref { p: &Vec<i64> }         // ERROR: references can't be stored
fn first(v: &Vec<i64>) -> &i64      // ERROR: can't return references
```

That's why there are no lifetime annotations: a borrow lives only inside
the borrower's scope, so there is nothing to name.

### What to do instead

| You wanted | Do this |
|---|---|
| Struct holds a reference | store an index, an owned copy, or the owner itself |
| Return part of a collection | return the index or an owned `str`/`Vec` |
| Share a big buffer | pass `&buf` down the call tree — borrows flow inward, not out |
| Persistent handle | return an owned id/key; look it up |

It is a real restriction — the payoff is that dangling pointers are
impossible by construction, and the checker stays simple enough to
verify a whole bootstrap with.

## What gets freed when

- Locals free at scope end (and *provably finished* ones earlier — the
  compiler inserts frees where it can prove last use, C5–C8 work).
- `Vec`/`HashMap`/`str`/`StringBuilder` own their heaps — dropping the
  owner drops the heap.
- `use arena;` for bump-scoped work; `use rc;` when you truly need shared
  ownership.

## Checking your own program

```bash
fid run x.fd --leaks            # report blocks never freed, by allocation site
fid run x.fd --checked-alloc    # fault on use-after-free / past-end access
fid audit x.fd [--stdlib]       # list the unsafe code the program reaches
```

`--checked-alloc` is the closest thing to ASan; run it in tests.
