# Week 7 — Memory safety III: regions, unsafe code and its tools

**Reading:** reference §8.5 (unsafe), §8.6 (regions);
`docs/reference/language-guide.md` §4, "What Safe Code Promises";
proposal 0005 (`docs/proposals/0005-region-arenas.md`); book chapter 4,
"Checking your own program".

**After this week you can:** say exactly what Fid promises for a program
without `unsafe`; use regions to bound a computation's memory; write a
small `unsafe` abstraction with a safe interface and justify it; find
memory errors in `unsafe` code with `--leaks` and `--checked-alloc`.

## 1. The safe-code promise

> A program with no `unsafe` block cannot corrupt memory. A memory bug is
> rejected at compile time or stops with a panic.

Weeks 5 and 6 gave most of the argument: ownership frees each value once,
references cannot dangle, `&mut` cannot alias. Bounds and overflow checks
(week 1) turn the rest into panics. The language guide lists what does
*not* hold yet; read that list as carefully as the promise. Two items
matter for this week: imported modules are trusted, and the standard
library contains `unsafe` code that has not all been audited (week 10
measures it with `fid audit`).

## 2. Regions

`region name { … }` is a scope with a name. Three things come with it
(reference §8.6):

- the collections declared in it that never leave it are freed at its end;
- `region name with budget(N) { … }` adds up, **at compile time**, the
  sizes of the region's locals and of its heap allocations of known size,
  and rejects the program when the sum exceeds `N` bytes. An allocation
  that can grow (`push`, string `+`, an allocation in a loop, a call that
  allocates) cannot be bounded, so it is an error inside a budget;
- a C build with `FID_REGION_ARENA=1` gives each region an arena: what is
  allocated in it comes back in one step at its end, and a value kept
  outside the region is copied out first (proposal 0005).

A typical use is per-frame or per-request scratch memory:

```fides
// frame.fd
fn checksum(v: &Vec<i64>) -> i64 {
    var s: i64 = 0;
    for i in 0..v.len() { s = s + v.get(i) * (i + 1); }
    return s;
}

fn main() -> i64 {
    var total: i64 = 0;
    for frame in 0..3 {
        region scratch {
            var samples: Vec<i64> = Vec::new();
            for i in 0..1000 { samples.push((frame * 7 + i) % 13); }
            total = total + checksum(&samples);
        }
    }
    println("total " + int_to_str(total));
    region tick with budget(64) {
        val a: i64 = 5;
        val b: i64 = 7;
        val name: str = "tick";
        total = a * b;
    }
    println(int_to_str(total));
    return 0;
}
```

```
$ fid run frame.fd
total 9024015
35
```

The `tick` region holds two `i64` (8 bytes each) and a `str` (16): 32 of
its 64 bytes. A budget is a static proof that a piece of code does not
grow the heap, which `@realtime` and embedded code need:

```fides
// budget_fail.fd
fn main() -> i64 {
    var n: i64 = 0;
    region tick with budget(32) {
        val a: i64 = 1;
        val b: i64 = 2;
        var log: Vec<i64> = Vec::new();
        log.push(a + b);
        n = log.len();
    }
    return n;
}
```

```
$ fid check budget_fail.fd
error: Budget error (line 4): region 'tick' used ...
...
error: Budget error (line 4): region 'tick' with budget(32) allocates on the heap: '.push()'.
...
```

## 3. unsafe

Some code needs memory the type system does not describe: an allocator,
a ring buffer over raw words, a device register, a C library's struct.
Inside `unsafe { }` (or an `@unsafe` function) the program may read and
write through raw pointers (`*mut T`), call `p_malloc`/`p_free`, make a
pointer from an integer, and call C. Everywhere else these are errors:

```fides
// deref_fail.fd
fn peek(addr: i64) -> i64 {
    val p: *mut i64 = addr as *mut i64;
    return *p;
}

fn main() -> i64 {
    return peek(0);
}
```

```
$ fid check deref_fail.fd
error[E097]: Safety error (line 4): reading through a raw pointer needs an `unsafe` block.
...
```

The discipline that makes `unsafe` code manageable is **encapsulation**:
keep the `unsafe` blocks small, inside functions whose *safe* signature
cannot be used to break them, and write down why each block is correct
in a `// SAFETY:` comment (`fid audit` counts the blocks that lack one):

```fides
// ring.fd
// A ring of 8 i64 slots in memory the program manages itself.
struct Ring { base: i64, head: i64, count: i64 }

fn ring_new() -> Ring {
    var base: i64 = 0;
    // SAFETY: the block is freed exactly once, by ring_free
    unsafe { base = p_malloc(64); }
    return Ring { base: base, head: 0, count: 0 };
}

fn ring_push(r: &mut Ring, x: i64) {
    val slot: i64 = (r.head + r.count) % 8;
    // SAFETY: slot is in 0..8 and the block holds 8 words
    unsafe {
        val p: *mut i64 = (r.base + slot * 8) as *mut i64;
        *p = x;
    }
    if r.count < 8 { r.count = r.count + 1; } else { r.head = (r.head + 1) % 8; }
}

fn ring_sum(r: &Ring) -> i64 {
    var s: i64 = 0;
    for i in 0..r.count {
        // SAFETY: (head + i) % 8 is a slot that ring_push wrote
        unsafe {
            val p: *mut i64 = (r.base + ((r.head + i) % 8) * 8) as *mut i64;
            s = s + *p;
        }
    }
    return s;
}

fn ring_free(r: Ring) {
    // SAFETY: base came from p_malloc in ring_new
    unsafe { p_free(r.base); }
}

fn main() -> i64 {
    var r: Ring = ring_new();
    for i in 1..11 { ring_push(&mut r, i); }
    println("last 8 sum to " + int_to_str(ring_sum(&r)));
    ring_free(r);
    return 0;
}
```

```
$ fid run ring.fd
last 8 sum to 52
```

The abstraction is not airtight, and finding out why is exercise 1:
`Ring` is a plain struct, so it can be copied and freed twice, and
nothing stops a caller from using it after `ring_free`.

## 4. When unsafe code is wrong

The runtime catches some mistakes on every build. A second free of the
same block panics:

```fides
// dfree.fd
fn main() -> i64 {
    // SAFETY: none; the second free is the bug this example shows
    unsafe {
        val p: i64 = p_malloc(32);
        p_free(p);
        println("freed once");
        p_free(p);
    }
    println("not reached");
    return 0;
}
```

```
$ fid run dfree.fd
freed once
Panic: Double free...
...
$ echo $?
1
```

A read past the end of a block, or after a free, is not caught by a
normal build. `--checked-alloc` builds through C with each block on its
own guarded pages, so the bad access stops the program where it happens:

```fides
// overrun.fd
fn main() -> i64 {
    var s: i64 = 0;
    // SAFETY: none; the loop reads one word past the block, on purpose
    unsafe {
        val base: i64 = p_malloc(64);
        for i in 0..9 {
            val p: *mut i64 = (base + i * 8) as *mut i64;
            s = s + *p;
        }
        p_free(base);
    }
    println(int_to_str(s));
    return 0;
}
```

```
$ CC="zig cc" fid run overrun.fd --checked-alloc
...
Panic: Invalid memory access (null or dangling pointer) at ...
...
$ echo $?
1
```

| Tool | Finds | Cost |
|---|---|---|
| `fid run x.fd --leaks` | blocks never freed, by allocation site | a C build; a recording allocator |
| `fid run x.fd --checked-alloc` | use after free, overrun, double free | a C build; one page or more per block |
| `fid audit x.fd` | every `unsafe` block reachable, with or without `// SAFETY:` | none: static |

## Exercises

The lab for this week is [Lab 7 — Taking the unsafe code out of a ring buffer](labs/lab07-ring/README.md).

1. Make `ring.fd`'s abstraction safe: mark `Ring` `@linear` so that it
   must be freed exactly once and cannot be copied. Which functions must
   change their signatures, and why does `ring_push` still work?
2. Replace the ring's `p_malloc` by a `Vec<i64>` of 8 elements. What
   happens to the `unsafe` blocks? Measure both versions with
   `fid explain` (bounds checks kept or removed).
3. Find the largest budget-free piece of `frame.fd` you can put under
   `region ... with budget(N)`, and the smallest `N` that compiles. Explain
   each byte of `N`.
4. Run `frame.fd` built with `CC="zig cc" FID_REGION_ARENA=1 fid ccompile`
   and without. Is the output the same? Read proposal 0005 and explain
   what would go wrong if `total` were a `Vec` that the region pushed to.

## Discussion

"Unsafe code is trusted" moves the proof obligation from the compiler to
the programmer. How should a code review treat a `// SAFETY:` comment?
What would it take to check one mechanically (look at
`docs/verification/` for what the project already proves)?
