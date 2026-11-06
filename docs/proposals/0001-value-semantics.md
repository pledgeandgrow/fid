# 0001: Values, copy-on-write collections, equality by content

- **Status:** accepted; partly implemented. `==` by content is done.
  Copy-on-write is written and not on in the verified compiler: today
  `var b = a` on a `Vec` still aliases (Nov 6, see below).
- **Decision:** accepted on 2026-11-02 (release M4, M5). `Vec`, `str` and
  `HashMap` are copy-on-write values; structs are values; `==` on a struct
  compares its fields.

## Problem

What `b = a` means depends on the type and on whether it is an assignment or a
call, and the three backends agree only because they share the same mistakes:

```fides
struct P { x: i64, xs: Vec<i64> }
fn bump(p: P) { p.x = 9; p.xs.push(7); }
fn grow(v: Vec<i64>) { v.push(5); }

var a: P = P { x: 1, xs: Vec::new() };
var b: P = a;  b.x = 2;      // a.x is still 1: assignment copies
bump(a);                     // a.x is now 9: a call shares
var v: Vec<i64> = Vec::new();
var w: Vec<i64> = v;  w.push(1);   // v.len() is 1: a Vec is shared
grow(v);                           // v.len() is 2
```

And `==` compares a struct's address and a `Vec`'s identity, so two equal
points are unequal. Without one rule the compiler cannot know when a value is
dead, which is why it cannot free everything (release C5, C6).

## Change

1. **Every type is a value.** Assigning, passing, returning and storing a value
   gives the receiver its own value: changing it never changes the original.
   Structs are copied (a struct of words is cheap to copy).
2. **`Vec`, `str`, `HashMap` are copy-on-write.** A copy shares the buffer and
   adds one to its reference count; the first change to a shared buffer copies
   it (Swift's model). Copies are O(1) until one side writes.
3. **A function changes the caller's value only through `&mut`.**
   `fn grow(v: &mut Vec<i64>)`, called as `grow(&mut v)`. Changing a by-value
   parameter changes the function's own copy; the compiler warns when such a
   change is never read (the call does nothing visible).
4. **Freeing:** the buffer is freed when its reference count reaches zero: at
   the end of the last owner's scope, on reassignment, or when the struct or
   collection holding it is freed. No collector, no leak for any `Vec`, `str`,
   `HashMap` or struct value, including `Vec<str>` elements.
5. **`==` and `!=`** compare by content: numbers and `str` by value, structs
   field by field, enums by variant and payload, `Vec` and `HashMap` element by
   element. A struct that holds a function value or a raw pointer has no `==`
   unless it implements `eq`. `same(a, b)` asks whether two values share a
   buffer.

## Status of the implementation (Nov 3, 2026)

- Done: `==` by content for structs and enums in both compilers; every
  parameter a function changes for its caller is `&mut` (403 parameters in
  345 functions, 2,232 edits across `compiler/`, `stdlib/`, tests and
  examples, by `scratchpad/cow/migrate.py` with the type checker finding the
  rest); a `&mut` parameter read where a value is wanted is read through
  (checker, escape check and both lowerings); the borrow checker treats
  read-only methods (`len`, `get`, ...) as reads. Behaviour is unchanged:
  every test prints what it printed before, and the compiler builds itself.
- Found while migrating: 44 places mutate an element through `.get(i)`
  (`self.symbols.get(idx).size = n`) and a few locals copied out of a
  collection are changed and never written back. Under copy-on-write the
  first must still mutate in place (as Swift's `arr[i].x = 1`), and a
  `Vec<Struct>` clone must copy its structs: a Vec of structs holds pointers
  to them today. So the copy-on-write switch needs structs to be values too
  (copied on assignment, on passing and in a cloned collection; element
  access a place), which is the next step and a change to the struct layout
  and call convention of every backend.

Nov 3, later:

- Done: `==` on two `Vec`s, and on a struct's `Vec` field, compares element
  by element (`Vec_eq_words` for numbers and bools, `Vec_eq_strs` for
  strings); a `Vec` of structs still compares by identity.
- Copy-on-write for `Vec` (share on assignment, copy before the first write:
  `Vec_share`, `vec_own` in core, `Lowerer_lower_value`) is written and behind
  `LOWER_COW` in `compiler/lower.fd`, off. On, it gives the right answers on
  the test programs (the example above prints 1, 0, 0, 1), but the compiler
  built with it crashes: its own source relies on shared handles in the ways
  listed above. So the next step is unchanged: structs as values, element
  access as a place, then turning `LOWER_COW` on. Shared data is never freed
  yet (no count word); the count is part of that step.

Nov 4–6:

- Copy-on-write lowering is written for `Vec`, `HashMap` and the `Vec` fields
  of copied structs, behind `LOWER_COW` (`compiler/lower.fd`), which is off
  in the verified compiler. With it on, the test programs get value semantics
  on all three backends (interpreter, C, native). The first version made
  the compiler compiling itself run out of memory in the type checker
  (`Vec_share_deep` copied every struct of a `Vec<Struct>` at each
  assignment from a place). Since Nov 6 shares are lazy (the owner box
  records the element kind and `vec_own` copies the elements at the first
  write; the compiler calls `vec_own` before a write into an element),
  counted (the box is freed with its last owner) and skipped for copies that
  are never written, and a variable's last use is moved. With it on, the
  compiler compiling itself reaches its fixpoint (Nov 6), at a peak of
  4.7 GB against 1.8 GB off. Next: the full suite with it on, then on by
  default; then free the shares held by struct copies at scope end.
- Freeing without counts (item 4, in part): the compiler frees what it can
  prove is finished: `Vec`/`HashMap` locals, a `Vec<str>`/`Vec<Vec>` that
  owns its elements, collections declared in a `region` at its end, and
  string temporaries (`test_mem_owned_drop.fd`). Everything else lives until
  exit (a leak, never a dangling value). There is no GC.
- Not done: the count word (item 2 of the implementation), structs copied on
  pass, `same(a, b)`.

## What it breaks

Code that changes a collection it received by value and expects the caller to
see it: 152 functions in `compiler/*.fd` and 35 in the stdlib (436 changes).
Each becomes a `&mut` parameter. Code that compares structs or `Vec`s with `==`
and meant identity (rare; found by the tests). This is a breaking change before
1.0, so it ships without an edition, with a migration warning first.

## Alternatives

- **Move semantics** (Rust): no counts, exact freeing, but every use after a
  move is an error and `.clone()` is everywhere; most borrow-checker friction.
- **Shared handles** (today, Go and Java): no migration, but aliasing bugs stay
  and freeing needs a collector or counts on every handle anyway.

## Implementation

1. **Warning first** (both compilers): "changes the by-value parameter 'v'; the
   caller will not see it: take `&mut`" — the list to migrate. Migrate the
   stdlib and `compiler/*.fd` to `&mut` while semantics are still shared, so
   the compiler keeps building itself at every step.
2. **Runtime:** a count word in the `Vec`, `str` and `HashMap` headers
   (core's `Vec_*` is the only implementation on every backend); `retain` on
   copy, `release` at scope end and on overwrite (the lowering already has drop
   points for `@drop` types and freed strings); `make_unique` before a write.
3. **Structs:** copied on pass as on assignment (the call ABI passes a copy;
   `&`/`&mut` pass the address).
4. **Equality:** synthesized `__eq_T` per type that is compared (the Rust work
   in progress: only in the module defining the type, only when compared, none
   for `Vec`/`HashMap` handles, which get element-wise `Vec_eq`).
5. Tests: one per row of the example above on all three backends, a leak test
   (`fid run --leaks` reports nothing) for each value kind, and the bootstrap
   fixpoint.
