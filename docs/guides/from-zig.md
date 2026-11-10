# Coming from Zig

Zig and Fides share a goal — honest systems code with no hidden
allocations — and arrive from opposite directions: Zig removes safety
to expose control; Fides keeps ownership/borrow safety and lets you
*annotate* your way down to the same control.

## Syntax map

| Zig | Fides |
|---|---|
| `fn f(a: i64) i64` | `fn f(a: i64) -> i64` |
| `const x = 1` / `var x` | `val x = 1` / `var x` |
| `comptime` | `comptime` — exists, but a smaller evaluator (`compiler/comptime_eval.fd`) |
| `@This()`, `@intCast` | plain casts `x as T` — fewer builtins, more library |
| `struct { ... }` | `struct X { ... }` — nominal, with `impl` blocks |
| `error{...}` + `!T` | `Result`/`Option` + `panic` — errors are values or traps, not error-sets |
| `defer` | none — explicit cleanup, `@zeroize` for secrets |
| `Allocator` params | none — `p_malloc`/`p_free` runtime, ownership inserts frees |
| `std.fmt.print` | `println(...)` |
| `@safety`/`ReleaseSmall` | annotations: `@crashfree`, `@no-alloc`, `@packed`, `@repr(C)`, `@simd` |
| `zig build` | `fid emit` (native PE/ELF), `fid ccompile` (via C) |

## The big inversion

Zig: everything is unsafe by default, `std` is optional, you write the
allocator plumbing. Fides: everything is borrow-checked by default
(`--borrowck=on`), the runtime owns allocation, and you opt *out* of
specific checks with annotations.

- `@no-alloc` on a function asserts it doesn't allocate (used by the
  optimizer as a fact; a compile contract, not a runtime check).
- `@wrapping` opts into wrap-around arithmetic for a function.
- `@packed`/`@repr(C)` give exact/`extern` layouts — the Fides answer
  to `packed struct`/`extern struct`.
- Inline assembly exists with clobber lists (50+ x86 ops) — see
  `compiler/pe.fd` and `docs/reference/reference.md`.

## Where Fides is lower-level than Zig, and where it isn't

Lower-level / more exposed:
- Direct PE/ELF emission with no C toolchain (`fid emit`), bare-metal
  `--no-std`, `@interrupt`/`@firmware`/`@cpu_local` for OS work,
  constant-time verification of generated asm (`fid`-level CT check).

Not lower-level:
- No `comptime` metaprogramming parity. Fides's `comptime` evaluates
  constants; it is not Zig's arbitrary compile-time code execution.
- No allocator parameterization convention — the runtime's allocator
  is fixed (per-thread free lists); if you need arenas, `use arena;`
  explicitly.
- References can't escape (no storing `&T`), which Zig would never
  restrict. Indices and owned containers are the idiom.

## Generics

Both monomorphize. `fn id<T>(x: T) -> T` exists; bounds come from `cap`
requirements rather than trait bounds syntax-heavy generics. The
monomorphizer (`compiler/mono.fd`) stamps per-type copies, same as Zig.

## Porting checklist

- `error{}` unions → `Result` or `panic` (decide: is it recoverable?).
- `defer` → explicit calls or `@zeroize`.
- allocator arguments → delete; use owned `Vec`/stdlib containers.
- `comptime` type-building → generics + `cap`, or runtime dispatch.
- `@setRuntimeSafety(false)` regions → `@crashfree`/`@wrapping`
  annotations + `fid run --checked-alloc` during debug.
