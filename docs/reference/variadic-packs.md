# Variadic packs (comptime tuples)

Roadmap L8E. A variadic parameter whose type is a type parameter that types
nothing else is a *pack*: each argument keeps its own type.

```fid
fn show_all<T>(label: str, ...xs: T) -> str {
    var out: str = label + " (" + int_to_str(xs.len()) + "):";
    for x in xs {
        out = out + " " + describe(x);   // describe's overload for x's type
    }
    return out;
}

show_all("mixed", 1, "two", true)
```

A variadic parameter of a concrete type (`...ns: i64`) is not a pack: its
arguments are packed into a `Vec<i64>` at run time.

## How it is compiled

Before type checking (compiler/mono.fd, section "L8E: variadic packs",
called from `mono_desugar_variadics`):

- A call with `m` arguments after the fixed ones calls the clone
  `show_all__pack<m>`, whose pack is `m` parameters `xs__0: T__0` ..
  `xs__<m-1>: T__<m-1>`. Ordinary monomorphization then makes one instance
  per call shape (the argument types).
- In the clone's body:

| Written | Becomes |
|---|---|
| `xs.len()` | the constant `m` |
| `xs.get(K)`, `K` a literal below `m` | `xs__K` |
| `for x in xs { B }` | `B` once per element, `x` bound to `xs__k`, `T` read as `T__k` |
| `f(.., xs)`, `f` a pack or a Vec variadic function | `f(.., xs__0, .., xs__<m-1>)` |

- `break` and `continue` of an unrolled `for x in xs` act as in a loop
  (`mono_pack_unroll_jumps`): each copy runs in a one-pass loop
  `for __pk_once_N in 0..1`, so `continue` ends that copy; `break` also sets
  a flag `__pk_brk_N` that skips the copies after it. A `break` of a loop
  inside the body stays that loop's.
- Any other use of `xs` (`xs.get(i)` with a variable `i`, `xs[i]`, `xs`
  passed to an ordinary parameter) is an error, since the elements have no
  single type:

```
Type error (in 'pick'): the pack 'xs' (`...xs: T`) has no single type: its elements each have their own.
```

## Limits

- Packs on methods, and calls of a pack function with named arguments, are
  not supported (the call is left as written and the checker rejects it).
- A local variable named like the pack inside the pack function's body is
  read as the pack.
- `fid lint` and the no-stdlib build (`--bare`, `--elf`, `--boot`) run `mono_desugar_variadics` but do
  not read its errors; a misused pack there reports `undefined variable`.

Tests: tests/bootstrap/test_variadic_pack.fd, test_variadic_pack_jumps.fd,
test_variadic_pack_forward.fd, test_variadic_pack_index_fail.fd,
test_variadic_pack_forward_fail.fd.
