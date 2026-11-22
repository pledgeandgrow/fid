# 4. Ownership and memory

This is the chapter that decides whether Fides fits your brain. Read it
even if you skim everything else.

## Every variable owns its value

In Fid a variable owns what it holds, and nothing else does. Numbers,
`bool`, structs, enums, tuples and arrays are copied when you assign them
or pass them by value. `Vec`, `HashMap` and `str` *behave* as if they
were copied too: the data is shared until one side writes, and the first
write copies it. So there are no moves to track and no aliasing to
surprise you:

```fides
// values2.fd
struct Account { name: str, history: Vec<i64> }

fn main() -> i64 {
    var a: Vec<i64> = Vec::new();
    a.push(1);
    var b = a;              // b is its own Vec; the data is shared until a write
    b.set(0, 99);
    b.push(2);
    println("a: " + int_to_str(a.len()) + " element(s), a[0] = " + int_to_str(a.get(0)));
    println("b: " + int_to_str(b.len()) + " element(s), b[0] = " + int_to_str(b.get(0)));

    var s: Account = Account { name: "ada", history: Vec::new() };
    var t: Account = s;     // copying a struct copies what it holds
    t.history.push(5);
    t.name = "grace";
    println(s.name + " has " + int_to_str(s.history.len()) + " entries");
    println(t.name + " has " + int_to_str(t.history.len()) + " entries");
    return 0;
}
```

```
$ fid run values2.fd
a: 1 element(s), a[0] = 1
b: 2 element(s), b[0] = 99
ada has 0 entries
grace has 1 entries
```

When a variable's scope ends, what it owns is freed: there is no garbage
collector and no `free()` in your code. Keeping a value somewhere (a
field, a `Vec`, a return value) gives that place its own owner; a
temporary is freed at the end of its statement. Copying is cheap, because
nothing is copied until somebody writes.

The one exception is a struct marked `@linear`: it *moves*, must be used
exactly once, and is for handles that must not be duplicated or forgotten
(a transaction, a connection; reference.md §8.7).

## Borrowing

A by-value parameter is the caller's value, copied on write: the callee
can't change the caller's variable. To let it, pass a reference. `&T`
reads, `&mut T` may change:

```fides
// borrow.fd
fn total(v: &Vec<i64>) -> i64 {
    var s: i64 = 0;
    var i: i64 = 0;
    while i < v.len() { s = s + v.get(i); i = i + 1; }
    return s;
}

fn bump_all(v: &mut Vec<i64>) {
    var i: i64 = 0;
    while i < v.len() {
        v.set(i, v.get(i) + 1);
        i = i + 1;
    }
}

fn add_one(v: Vec<i64>) -> Vec<i64> {
    var w = v;              // a by-value parameter is the caller's data, copied on write
    w.push(1);
    return w;
}

fn main() -> i64 {
    var v: Vec<i64> = Vec::new();
    v.push(1);
    v.push(2);
    v.push(3);
    println("total " + int_to_str(total(&v)));
    bump_all(&mut v);
    println("after bump " + int_to_str(total(&v)));
    val w: Vec<i64> = add_one(v);
    println(int_to_str(v.len()) + " and " + int_to_str(w.len()));
    return 0;
}
```

```
$ fid run borrow.fd
total 6
after bump 9
3 and 4
```

Methods and fields read through a reference by themselves (`v.len()`,
`p.x`); `*r` reads the value a reference points to.

The borrow checker holds you to two rules: many `&` or one `&mut` at a
time, and nothing changes a value while a reference to it is in use. A
borrow lasts until its last use, not to the end of the block:

```fides
// conflict_fail.fd
fn main() -> i64 {
    var v: Vec<i64> = Vec::new();
    v.push(1);
    val r: &Vec<i64> = &v;
    v.push(2);                       // error: v is borrowed by r
    println(int_to_str(r.len()));
    return 0;
}
```

```
$ fid check conflict_fail.fd
...
error[E080]: borrow error in main (line 6): cannot mutate v: shared borrow(s) are active on it or its fields
    Held by: 'r' (&v, line 5).
    Fix: use 'r' for the last time before this line, or declare it in an inner block { ... } that ends first.
...
$ echo $?
1
```

Moving `println(int_to_str(r.len()));` above `v.push(2);` fixes it: `r`'s
last use is then before the change. `--borrowck=warn` turns these errors
into warnings while you port code; don't ship with it.

## The rule that surprises everyone: references are second-class

A reference can be passed to a call, held in a local, and returned when
it comes from a reference parameter. It can never be *kept*: not in a
struct field, a `Vec`, a global, a task, or a closure that outlives what
it points to. That is why most Fid code needs no lifetime annotations,
and why a reference can never dangle. The one exception is a struct with
a lifetime parameter, `struct View<'a> { r: &'a i64 }`: it may hold a
reference, and is then held like a reference itself (reference §8.4).

```fides
// refs.fd
struct Point { x: i64, y: i64 }

fn x_of(p: &Point) -> &i64 { return &p.x; }

fn bigger<'a>(a: &'a Point, b: &'a Point) -> &'a Point {
    if a.x + a.y >= b.x + b.y { return a; }
    return b;
}

fn main() -> i64 {
    val p: Point = Point { x: 3, y: 4 };
    val q: Point = Point { x: 1, y: 9 };
    val rx: &i64 = x_of(&p);
    println("x is " + int_to_str(*rx));
    val big: &Point = bigger(&p, &q);
    println("bigger has y = " + int_to_str(big.y));
    return 0;
}
```

```
$ fid run refs.fd
x is 3
bigger has y = 9
```

The compiler works out from `x_of`'s body that its result points into
`p`. A lifetime name such as `'a` is optional: it says the same thing in
the signature, and then the body is checked against it.

What it rejects:

```fides
// store_fail.fd
struct Holder { r: &i64 }            // error: no lifetime parameter to hold it

fn main() -> i64 {
    var a: i64 = 1;
    val h: Holder = Holder { r: &a };
    return 0;
}
```

```
$ fid check store_fail.fd
...
error: Reference error (line 0): in 'Holder', field 'Holder.r' has type '&i64', which stores a reference; references can only be passed to calls, held in locals and returned; to hold one, give the struct a lifetime parameter: `struct Holder<'a> { r: &'a T }`
...
```

```fides
// dangle_fail.fd
fn make() -> &i64 {
    var x: i64 = 5;
    return &x;                       // error: x is freed when make returns
}

fn main() -> i64 {
    println(int_to_str(*make()));
    return 0;
}
```

```
$ fid check dangle_fail.fd
...
error: Reference error (line 4): in 'make', 'make' returns a reference to local 'x', which is freed when it returns
...
```

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

- A variable's value is freed when its scope ends; a temporary at the end
  of its statement. A `Vec<str>` or a struct frees what it holds with it.
- A struct with a `drop` method gets it called when a variable declared
  with that struct type goes out of scope, the last declared first:

```fides
// drop.fd
struct Conn { id: i64 }

impl Conn {
    pub fn drop(self: Conn) {
        println("closing " + int_to_str(self.id));
    }
}

fn work() {
    val a: Conn = Conn { id: 1 };
    val b: Conn = Conn { id: 2 };
    println("working with " + int_to_str(a.id) + " and " + int_to_str(b.id));
}

fn main() -> i64 {
    work();
    println("done");
    return 0;
}
```

```
$ fid run drop.fd
working with 1 and 2
closing 2
closing 1
done
```

- `region r { ... }` is a scope whose collections are freed at its end
  (a C build with `FID_REGION_ARENA=1` gives it an arena, freed in one
  step); `use arena;` does the same for code you manage by hand; `use rc;`
  is for the rare value that really has several owners (reference.md
  §8.6, `stdlib/arena.fd`, `stdlib/rc.fd`).
- Nothing runs after a panic: the operating system takes the memory back
  (chapter 6).
- Memory you take yourself in `unsafe` code (`p_malloc`) is yours to free.

## Checking your own program

Three commands find memory bugs. `--leaks` and `--checked-alloc` build the
program through C, so they need a C compiler (`CC="zig cc"`, `cl` or
`tcc`):

```fides
// leaky.fd
use string;

fn main() -> i64 {
    var names: Vec<str> = Vec::new();
    var i: i64 = 0;
    while i < 3 {
        names.push("user" + int_to_str(i));
        i = i + 1;
    }
    println(str_join(names, ","));
    // SAFETY: a deliberate leak, to show what --leaks reports
    unsafe {
        val raw: *mut u8 = p_malloc(64) as *mut u8;
    }
    return 0;
}
```

```
$ CC="zig cc" fid run leaky.fd --leaks
...
user0,user1,user2
fid --leaks: 2 block(s), 98 bytes, never freed; by where they were allocated:
        64 bytes in      1 block(s): main, allocated at leaky.fd:14
        34 bytes in      1 block(s): p_malloc_uninit <- p_str_alloc_uninit <- str_from_bytes <- str_join, allocated at leaky.fd:11
```

The 64 bytes are the deliberate leak. The 34 bytes are a bug of the
Nov 21 tree: the string `str_join` returns is never freed, even when it
is bound to a local. Without both it would end with `fid --leaks: every
block was freed`: the other strings and the `Vec` are freed by the
compiler.

```fides
// uaf.fd
fn main() -> i64 {
    // SAFETY: none; this reads a block after freeing it, on purpose
    unsafe {
        val p: *mut i64 = p_malloc(16) as *mut i64;
        *p = 7;
        p_free(p as i64);
        println(int_to_str(*p));
    }
    return 0;
}
```

```
$ CC="zig cc" fid run uaf.fd --checked-alloc
...
Panic: Invalid memory access (null or dangling pointer) at ...
...
$ echo $?
1
```

`--checked-alloc` puts every block on its own guarded pages, so a read
after free or past the end stops the program where it happens: the
closest thing to ASan. Run your tests with it. `fid audit x.fd` lists the
`unsafe` code a program reaches and which of it has no `// SAFETY:`
comment (`--stdlib` counts the standard library's too).

## Common mistakes

- **Expecting `var b = a;` to share a `Vec`.** It doesn't: `b` is its own
  value. To let a function change your `Vec`, pass `&mut v`; to share one
  `Vec` between owners on purpose, use `rc`.
- **`for x in v` when `v` is a `&Vec<i64>`.** Today `x` gets the type
  `&Vec<i64>` there and arithmetic on it is a type error; write
  `for x in *v`, or walk it by index as `total` does.
- **Keeping a reference.** A struct field, a `Vec<&T>`, a global, or a
  task argument of reference type is an error. Store an index, a key, or
  an owned copy instead (the table above).
- **Changing a value while a reference to it is still used.** Use the
  reference for the last time first, or put it in an inner block.
- **`fid run --leaks` without a C compiler.** It stops with "C
  compilation failed"; set `CC`.
- **Reading `--leaks` output of a program that panicked.** Nothing is
  freed after a panic, so everything live at that point is listed. Look
  at leaks of runs that end normally.
