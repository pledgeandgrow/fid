# Coming from C

Fides is what a C programmer reaches for when they want C's output —
a standalone native binary with predictable layout — but not C's
UB-driven debugging sessions. If you write firmware, parsers, or
daemons in C, this is the shortest distance.

## Syntax map

| C | Fides |
|---|---|
| `int f(int a)` | `fn f(a: i64) -> i64` |
| `int x = 1` | `val x = 1` or `var x = 1` |
| `struct P { int x; }` | `struct P { x: i64 }` |
| `p->x` / `p.x` | `p.x` — no `->`; field access is uniform |
| `malloc`/`free` | owned values + `use arena;`/`p_malloc` when you must |
| `char*` | `str` (immutable, length-tagged) or `Vec<u8>`-style buffers |
| `NULL` | `Option::None` — can't deref without checking |
| `return -1` errors | `Option`/`Result`, or `panic` for fatal |
| `switch` | `match x { ... }` |
| `goto cleanup` | none — early `return` + explicit cleanup |
| `#include` | `use module;` — real modules, not text inclusion |
| `-DNDEBUG` asserts | `panic` stays in release; checks are semantic |
| `gcc x.c -o x` | `fid emit x.fd` (native) or `fid ccompile x.fd` (via C) |

## What replaces the dangerous idioms

| C idiom | Fides answer |
|---|---|
| `T* out` param | return the value, or `&mut` borrowed out-param |
| buffer + len pairs | `str`/`Vec` carry their length; bounds-checked |
| tagged unions by hand | `enum`/`Option`/`Result` with payload + `match` |
| `void*` polymorphism | generics (`fn f<T>(x: T)`) or `dyn Cap` objects |
| macro generics | real generics, monomorphized |
| vtables by hand | `cap` + `impl Cap for T` + `x as dyn T` |
| `setjmp`/error codes | `panic` (trace + nonzero exit) / `Option`/`Result` |

## The C things that map directly

- `@repr(C)` — exact C ABI layout for FFI/hardware structs.
- `@packed` — `__attribute__((packed))`: field_align=1, no padding.
- `@cpu_local` — per-CPU variables via GS segment (kernel-grade).
- Inline asm with clobber lists — 50+ x86 instructions in `compiler/pe.fd`.
- `--no-std` — bare-metal mode, no core.fd: freestanding like `-ffreestanding`.
- `p_inb`/`p_outb`/MMIO-style volatile access (`use volatile;`).
- Constant-time enforcement: post-codegen CT verifier scans the emitted
  asm for data-dependent branches/memory ops (`test_ct_asm.fd`).
- FFI: `fid parse-header <h>` generates bindings from a C header.

## Ownership — the part that isn't C

Fides inserts `free` calls for you, based on ownership: a value has one
owner, dies when the owner does, and `&`/`&mut` borrows are checked so
no use-after-free compiles. Concretely, this rejects:

```fides
var v: Vec<i64> = Vec::new();
var p = &v;            // ok, borrow
// can't store p in a struct, can't return it, can't send to a task
```

References never escape — so the compiler can guarantee no dangling
pointers without a GC or a lifetime-annotation language. The C habit
of returning `char*` into a caller's buffer translates to returning
owned `str`/`Vec` or writing into a `&mut` parameter.

## A realistic port

```c
// C
int count_lines(const char* path) {
    FILE* f = fopen(path, "r");
    if (!f) return -1;
    int n = 0; char buf[4096];
    while (fgets(buf, sizeof buf, f)) n++;
    fclose(f); return n;
}
```

```fides
// Fides
use core;
use file;
use string;

fn count_lines(path: str) -> i64 {
    if !file_exists(path) { return -1; }
    var text: str = file_read(path);
    var n: i64 = 0;
    var i: i64 = 0;
    while i < str_len(text) {
        if str_byte_at(text, i) == 10 { n = n + 1; }
        i = i + 1;
    }
    return n;
}
```

No `fclose`, no buffer size, no null check on `text` — `file_read`
returns an owned string that frees itself.

## Watch out for

- `str` is immutable and length-tagged; in-place byte munging uses
  `Vec`/buffer APIs, not pointer arithmetic.
- `i64` is the default integer; narrower/unsigned types exist but the
  idiom is i64-until-you-need-layout (then `@repr(C)`/`@packed`).
- `panic` instead of `assert(false)`+`abort()` — it prints a source
  trace. `@crashfree` marks code that must never panic.
