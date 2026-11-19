# Specialization and const generics in structs

Roadmap L8H (specialization) and L8A (const generic structs). The checks
are in `fid` (the self-hosted compiler); `fidesc` is frozen without them.

## Specializing a generic function

A generic function and definitions of the same name for narrower types can
live side by side in one file:

```fid
fn describe<T>(x: T) -> str { return "any"; }
fn describe(x: i64) -> str { return "i64 " + int_to_str(x); }
fn describe<T>(v: Vec<T>) -> str { return "vec of " + int_to_str(v.len()); }
fn describe(v: Vec<i64>) -> str { return "vec of i64"; }
```

A call is typed against the most general definition and runs the body of
the most specific definition whose parameters take the argument types:
`describe(5)` the i64 one, `describe(true)` the generic one, a `Vec<str>`
the `Vec<T>` one, a `Vec<i64>` the `Vec<i64>` one. A call inside a generic
function is decided per instantiation (`fn wrap<T>(x: T) { describe(x) }`
runs the i64 body for `wrap(7)`).

- Most specific wins: a definition is more specific than another when the
  other's parameters take its parameter types, but not the reverse.
- A tie is an error. With `pick<B>(a: i64, b: B)` and `pick<A>(a: A, b:
  i64)`, `pick(1, 2)` is taken by both and neither is more specific:

  ```
  the call of 'pick' with (i64, i64) is taken by two definitions, neither
  more specific than the other: pick<B>(i64, B) and pick<A>(A, i64)
  ```

- A specialization must return what the general definition returns for
  those types, or the call is an error.

How: parser.fd `pd_name_specializations` keeps the name on the most
general definition and renames the others `name__spec<k>`; the choice is
ast.fd `spec_choose`; the checker reports ties
(`TypeChecker_check_spec_call`); mono.fd `instantiate` takes the chosen
body.

## Specializing methods of a generic impl

The same holds for methods. Next to `impl<T> Wrap<T>`, an impl of the
struct for narrower arguments (`impl Wrap<i64>`, `impl Wrap<str>`,
`impl<U> Wrap<Vec<U>>`, `impl Wrap<Vec<i64>>`) may define a method the
generic impl has; `w.show()` runs the method of the most specific impl
whose type takes the receiver's type, and the generic impl's otherwise.
Two impls that both take the receiver, neither more specific than the
other, are an error:

```
the call of method 'which' on Pair<i64, i64> is taken by two impls,
neither more specific than the other: impl<B> Pair<i64, B> and
impl<A> Pair<A, i64>
```

A narrower impl may also have methods of its own (`impl Wrap<i64> { fn
twice(self) -> i64 }`, `impl<U> Wrap<Vec<U>> { fn first(self) -> U }`).
They exist for receivers of that type only (`twice` on a `Wrap<bool>` is
an error), and the impl's parameters come from the receiver (`first` on a
`Wrap<Vec<str>>` returns a str). The order of the impls in the file does
not matter.

How: parser.fd `pd_impl_specializations` keeps each impl's type
(`Wrap<i64>`), types `self` of a narrower impl's methods as that type and
renames a method the generic impl also has `name__spec<k>`; the checker
compares the impls (`TypeChecker_check_mspec_call`) and checks a narrower
impl's own methods (`TypeChecker_narrow_method_call`); mono.fd
`mono_method_inst` takes the chosen body.

## Specializing trait impls

Beside the generic impl of a trait for a whole struct (`impl<T> Show for
Wrap<T>`), an impl of the trait for a narrower type (`impl Show for
Wrap<i64>`, `impl<U> Show for Wrap<Vec<U>>`) gives the trait's methods for
that type. A call `w.show()`, a call through a bound (`fn render<T:
Show>(x: T) { x.show() }`) and a default method of the trait that calls
`self.show()` all run the method of the most specific impl whose type
takes the receiver's, the generic impl's otherwise. A method the narrower
impl leaves out is the generic impl's (or the trait's default). Ties are
an error, as for impls:

```
the call of method 'which' on Pair<i64, i64> is taken by two impls,
neither more specific than the other: impl<B> Pair<i64, B> and
impl<A> Pair<A, i64>
```

The generic impl stays the trait's impl for every type (bounds, default
methods, associated types). So a narrower impl may not bind associated
types, every method must take `self` (the receiver's type chooses it), and
every method must be one of the trait's:

```
method 'shout' of `impl Show for Wrap<i64>` is not a method of trait 'Show'
method 'make' of `impl Make for Wrap<i64>` has no `self`: a specialization
is chosen by the receiver's type
```

An impl of a trait for a narrower type with no generic impl of the trait
for the struct (`impl Show for Wrap<i64>` alone) gives the methods to that
type only: a `Wrap<bool>` has no `show`.

How: parser.fd reads the `for` type of each trait impl
(`g_pd_timpl_selfs`); `pd_trait_impl_specializations` turns a narrower
trait impl with a generic one in the file into a narrower impl of the
struct (`impl Wrap<i64>`) whose methods are renamed `name__spec<k>`, so the
method impl machinery above chooses between them; the checker compares the
impls also when the generic one is a trait impl
(`TypeChecker_narrow_method_call` calls `TypeChecker_check_mspec_call`).

## Specializations in another module

A module may specialize a generic definition of a module it imports: a
function (the generic one must be `pub`), a method of a generic impl, or a
generic trait impl:

```fid
use spec_lib::wrap_lib;          // pub fn describe<T>(x: T), impl<T> Wrap<T>,
                                 // impl<T> Show for Wrap<T>
fn describe(x: i64) -> str { .. }
impl Wrap<i64> { pub fn kind(self) -> str { .. } }
impl Show for Wrap<str> { fn show(self) -> str { .. } }
```

The narrower definition must be strictly more specific than the generic
one (the generic one's parameters take its types). Each call runs the most
specific definition, wherever it is made: a call inside the library
(`describe_twice<T>` calling `describe(x)`) with an i64 runs the program's
`describe`. A definition of the same name that the generic one does not
take stays a duplicate definition, and one in a module that does not import
the generic's module is not a specialization.

How: parser.fd `pd_cross_module_specializations` renames the narrower
definitions `name__spec<k>` (k after every number of that name in any
module) before compiler.fd's duplicate checks and its renaming of
shadowed imports; `merge_programs` keeps a module's generic impl of a
struct beside another module's narrower impl (`pd_impl_is_narrowed`).

## Const generic structs

A struct may take a number as a parameter: `struct Matrix<T, const N:
usize>` (or `N: i64`). `Matrix<i64, 3>` and `Matrix<i64, 2>` are different
types: passing one for the other is an error. A function generic over N
gets it from its argument's type, or from the declared type its result is
assigned to, and reads N as a constant in its body:

```fid
fn trace<const N: usize>(m: Matrix<i64, N>) -> i64 {
    var s: i64 = 0;
    for i in 0..N {
        s = s + m.data.get(i * N + i);
    }
    return s;
}
val e: Matrix<i64, 4> = identity("id4");   // fn identity<N: i64>(..) -> Matrix<i64, N>
```

An impl takes const parameters too, `impl<T, const N: usize> Matrix<T, N>`,
and every method reads N as the number of the receiver's type; `impl<N:
i64> Matrix<i64, N>` adds methods for i64 matrices of any size.

## Limits

- A specialization in another module needs the compiler.fd hooks named
  above (`pd_cross_module_specializations` before the duplicate checks,
  `pd_impl_is_narrowed` in `merge_programs`).
- A trait method without `self` (`fn make() -> Self`) is not specialized,
  and a narrower trait impl may not bind associated types.
- Trait objects (`w as dyn Show`) of a generic struct take the generic
  impl's methods.
- A method two narrower impls define and the generic impl does not (or
  with no generic impl) is not specialized: it is two definitions of one
  method, and calls do not choose between them by the receiver's type.
  Give the generic impl the method to specialize it.
- A `[T; N]` field in a struct is not supported; turbofish with a number
  (`f::<3>()`) is not parsed.

Tests: tests/bootstrap/test_specialization.fd,
test_specialization_ambiguous_fail.fd, test_impl_specialization.fd,
test_impl_specialization_ambiguous_fail.fd, test_impl_narrow_methods.fd,
test_impl_narrow_receiver_fail.fd, test_trait_impl_specialization.fd,
test_trait_impl_specialization_ambiguous_fail.fd,
test_trait_impl_spec_not_in_trait_fail.fd,
test_trait_impl_spec_no_self_fail.fd,
test_trait_impl_narrow_receiver_fail.fd,
test_cross_module_specialization.fd (with spec_lib/wrap_lib.fd),
test_const_generic_struct.fd,
test_const_generic_struct_size_fail.fd, test_const_generic_impl.fd.
