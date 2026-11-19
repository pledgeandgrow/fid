# c2fd: C to Fid

`tools/c2fd.fd` translates one C99 source file into Fid source. It is a
best-effort migration aid, not a C compiler: the output keeps the C program's
structure, names and comments, and every place it could not translate
faithfully carries a `// TODO(c2fd): ...` comment next to the closest
translation it found. Review the output before using it.

## Usage

```bash
# build it once (about 30 s), then translate
CC="zig cc" fid ccompile tools/c2fd.fd -o c2fd.exe
./c2fd.exe input.c > output.fd

# the interpreter accepts it too, but it is very slow on an 8,500-line
# program: use the built one
fid run tools/c2fd.fd input.c > output.fd
```

The translation goes to stdout. On stderr c2fd says how many `TODO(c2fd)`
notes the output holds. Run the result from the repository root (fid finds
`stdlib/` there): `fid run output.fd` or `fid ccompile output.fd -o out`.

## What it reads

- Functions, prototypes (dropped: Fid needs none), globals, `static` locals
  (they become globals named `<function>_<name>`).
- `struct` and `union` (a union becomes a struct, with a TODO: its fields no
  longer share memory), `enum` (constants and a `type` alias, since C enum
  values are integers and the code does arithmetic and comparisons on them),
  `typedef` (an arithmetic or function-pointer typedef becomes a `type`
  alias; a struct typedef names the struct).
- The preprocessor: `#include` (dropped; library calls are mapped),
  `#define` of a constant expression (a `const`), any other macro, with or
  without parameters (expanded where it is used, `#` and `##` included),
  `#undef`, `#if`/`#ifdef`/`#ifndef`/`#elif`/`#else`/`#endif` (evaluated
  with `defined`, integers and the macros' values).
- Every statement: `if`/`else`, `while`, `do`-`while`, `for`, `switch`,
  `break`, `continue`, `return`; `goto` and labels become TODO comments.
- Every operator, casts, `sizeof`, string and character literals,
  initializer lists (designated ones too), and assignments, `++` and `--`
  inside expressions: they are hoisted into statements in C's order, and
  where C would skip them (the right side of `&&`/`||`, a branch of `?:`)
  the hoisted statements are put under the same condition.

## C patterns and their Fid form

| C | Fid |
|---|---|
| `int`, `long`, `char`, `short` | `i64`; `unsigned` is `u32`, `unsigned long` and `size_t` are `u64`, `unsigned char` is `u8`; a function doing unsigned arithmetic is `@wrapping` |
| `double`, `float` | `f64` |
| `int a[N]`, `int a[] = {...}` | `Vec<i64>` (`Vec::filled(N, 0)`, or pushes) |
| `T *a = malloc(n * sizeof(T))`, `calloc`, `realloc(a, ...)` | `Vec<T>`; `free(a)` becomes a comment |
| a malloc'd array freed in the block that declares it (`free(a);` a statement of that block) | the statements from the declaration to the free become `region r_a { ... }`; allocations whose lifetimes overlap share one region (an arena); a variable declared inside and used after the free is declared before the region |
| a local pointer into an array (`int *p = a + k; p++; *p; p[i]; *(p + i); q - p; p < end`) | an `i64` index into the Vec: `var p: i64 = k`, `a[p]`, `a[p + i]`, `p += 1`, `q - p`, `p < end` (also a `char *` into a written char array) |
| a pointer parameter that is moved (`p++`), or that callers point into the middle of an array (`f(a + 2)`, `f(&a[i])` for one used as an array) | `p: &mut Vec<T>` (`&Vec<T>` when `const`) and its index `p_at`: a parameter when callers pass an offset (`f(&mut a, 2)`), else `var p_at: i64 = 0` |
| `const char *s` (only read) | `str`; `s[i]` and `*s` are `c_char(s, i)`, `s++` is `s = c_skip(s, 1)` |
| a char array or `char *` parameter that is written | `Vec<i64>` of bytes with its NUL / `&mut Vec<i64>` |
| `T *p` parameter used as an array | `&mut Vec<T>` (`&Vec<T>` when `const`) |
| `T *p` parameter used as one value, `struct S *p` parameter | `&mut T` (`&T` when `const`); a caller's `&x` becomes `&mut x` |
| linked structures (`struct Node *next`), pointers compared with `NULL`, stored, freed or moved | raw pointers `*mut T` in `@unsafe` functions; `malloc(sizeof(T))` is `p_malloc(comptime struct_size(T)) as *mut T`, `free` is `p_free` |
| `for (i = a; i < b; i++)` (the body leaves `i` and `b` alone) | `for i in a..b` (`..=` for `<=`) |
| other `for` loops | `init; while cond { body; step }`, with `step` before each `continue` |
| `do { } while (c)` | `loop { ...; if !c { break; } }` |
| `switch` | an `if`/`else if` chain; case labels that share code are joined with `\|\|`; a case that falls through gets the next case's statements repeated; a `break` nested in a case makes the chain a `loop { ... break; }` |
| `printf`, `fprintf(stdout/stderr)`, `puts`, `putchar`, `sprintf` | `print`/`println` of an f-string (or `+` when an argument holds quotes or braces); widths and precisions through small helpers |
| `FILE *f = fopen(path, "r")`, `fgetc`, `fgets`, `feof`, `fclose` | the `CFile` reader (the whole file is read by `c_fopen`) |
| `main(int argc, char **argv)` | `fn main() -> i64` with `argv` from `c_args()` |
| `strlen`, `strcmp`, `strncmp`, `strcpy`, `strcat`, `strchr`, `strstr`, `atoi`, `isdigit`..., `abs`, `exit`, `assert`, `sqrt`... | `str_len`, `str_compare`, core's `char_is_*`, `abs`, `p_exit`, `assert`, `math_*` (with `use math`), or a `c_` helper |
| `if (n)`, `!p`, `while (*s)` | an explicit test: `n != 0`, `(p as i64) == 0`, `c_char(s, 0) != 0` |

The helpers a translation uses (`c_char`, `c_fmt_f`, `CFile`, ...) are
written at the end of the output, so the result has no dependency on c2fd.

## Known gaps

- `goto` and labels: a TODO; the code must be restructured by hand.
- A union read through another member than the one written (type punning).
- Pointers into arrays become indexes (the table above) only when c2fd can
  follow every value the pointer takes: it must start from and stay in one
  array (a local array, a malloc'd one or an array parameter), and must not
  be compared with `NULL`, returned, freed, kept in a field, handed to a C
  library function, or have its own address taken. Otherwise it stays a raw
  pointer, and arithmetic on raw pointers (linked structures, `void *`) is
  written with 8-byte elements and a TODO.
- Two cursor arguments over the same array passed to one call would give
  `&mut a` twice, which Fid's borrow checker rejects.
- Regions come only from a `malloc`/`calloc` of an array in a declaration
  and a `free` of it as a later statement of the same block; a raw `malloc`
  (a linked node) keeps its `p_free`, and a block with a `case` or a label
  between the two gets no region.
- Two pointers to the same buffer: a `str` or `Vec` is a value in Fid, so a
  write through one copy is not seen through the other.
- `char` is signed in C; bytes read from a string are 0 to 255 here.
- `sizeof` assumes an LP64 target (`long` is 8 bytes); a struct's size is
  Fid's layout (`comptime struct_size`).
- `strlen` and the other size results are `i64`, not `size_t`.
- Writing files, `scanf`, `time`, `rand`, `qsort` and most of the C library
  beyond the table above: kept as calls with a TODO.
- Bit-fields become whole fields; `printf`'s `%e` and `%g` are approximate;
  the `+` flag is ignored.
- A global of an unsigned type is read as `(g as u64)`: fid reads unsigned
  globals as signed otherwise (see the compiler bug below).

## Compiler issues met while writing it

- A global `u64` reads as signed: `var g: u64 = 18446744073709551615;`
  then `println(g)` prints `-1` and `g >> 60` is `-1` (a local copy gives
  `15`), on `fid run` and on the C path. c2fd works around it.
- A Fid function named like a C library function (`accept`, `perror`)
  breaks `fid ccompile`: the generated C declares it with the library's name,
  and the C compiler reports conflicting types.
- `swap(&mut v[i], &mut v[j])` is rejected by the borrow checker (two `&mut`
  of `v` in one statement); c2fd copies the elements into locals and writes
  them back after the call.

## Tests

`tests/c2fd/` holds the C programs, their golden translations (`.fd`) and
`check.sh`, which compiles each C program, runs it, translates it, and runs
the translation with `fid run` and through `fid ccompile`, comparing the
outputs. `musl_bits.c` is real-world code: four functions of musl libc (MIT),
unchanged. `gaps.c` shows the cases above that need manual edits.
`cursors.c` checks pointers into arrays as indexes (locals, moved and
offset parameters, recursion on `a + 1`, two pointers from both ends, char
buffers) and `regions.c` malloc/free lifetimes as regions (one array, two
freed together, one per loop iteration with a `break`, a char buffer).
