# Borrows across calls (LG3)

What the borrow checker (compiler/borrowck.fd) records for
`val r = f(args)` and `val r = recv.m(args)`, in addition to the escape
check (compiler/escape.fd) that already proves a returned reference comes
from a parameter. Reference: docs/reference/reference.md § 8.4.

## Which arguments the result borrows (`bc_sig_ties`)

- A signature with lifetimes: the parameters carrying one of the result's
  lifetimes (`fn pick<'a>(x: &'a i64, y: &'a i64) -> &'a i64`: both;
  `fn first<'a>(x: &'a i64, y: &i64) -> &'a i64`: `x` only; `'static`:
  none).
- A signature without lifetimes whose result is a reference: every
  reference parameter (`fn only(p: &i64) -> &i64`: `p`). This is sound
  (the result cannot borrow from anything else); write lifetimes to tie it
  to fewer.
- A method: parameter 0 is the receiver. `fn peek_a<'a>(self: &'a Holder)
  -> &'a i64` makes `val r = h.peek_a()` a borrower of `h`, mutably when
  `self` is `&mut`. Methods are found by name (`.m`); a name in several
  impls or trait impls takes the union of their ties. A method is also
  found under `Type_m` for a call written as a function.

## What each argument makes the result borrow (`bc_tie_one`)

- `&path` / `&mut path`: that path, shared or mutably, and what `path`
  borrows when it is a variable holding borrows (`read(&view)`).
- a variable that holds a shared borrow: what it borrows
  (`val q = &a; val r = only(q)`: `r` borrows `a` after `q` is dead).
- a variable that holds a mutable borrow: the borrow is handed over
  (`bc_hand_over_mut`). `val m = &mut a; val r = id_mut(m)`: `r` holds
  the `&mut a` until its last use, also after `m`'s; if `r` ends first and
  `m` is used again, `m` holds it again (`bc_mut_release_handed`).
- for a receiver: its path (as `self` is: `&mut self` mutably) and what it
  borrows; a receiver bound to a reference (`val q = &a; q.m()`) passes on
  what it points to and is not borrowed itself (`ref_vars`).
- `val w = v` where `v` holds borrows: `w` holds the same (a `&mut` moves).

## Methods that only read their receiver

`recv.m(..)` changes `recv` unless `m` is a builtin reader
(`bc_read_method`) or every user method called `m` (impls, trait impls,
trait declarations) takes `self` as `&` or by value (a struct value is a
copy): `bc_user_read_method`, from `g_bc_meth_kind`. A builtin changing
method's name (`push`, `insert`, `set`, ... `bc_builtin_mut_method`) stays
a change whatever a user method of that name takes.

## Lifetimes on trait methods

`cap Pick { fn pick<'a>(self: &'a Self, other: &i64) -> &'a i64; }`: the
trait parser keeps the lifetimes as on a function (`@lifetime` on the
parameters and on the `TraitMethod`, checked by `pd_check_lifetimes`). An
impl's method may tie its result only to parameters the trait's ties it to
(`TypeChecker_check_impl_lifetimes`, when either side writes lifetimes):
a caller through the trait relies on the trait's signature. The borrow
checker adds the trait's ties to `.m` (`bc_trait_sig_ties`).

## Structs with lifetime parameters

`struct View<'a> { r: &'a i64, k: i64 }` may hold a reference in a field
(the parser keeps `@lifetime("'a")` on the struct; a field's lifetime must
be declared; `impl<'a> View<'a>` lets its methods use `'a`; `View<'a>` is
the type `View`). A value of such a struct is held like a reference
(compiler/escape.fd, `g_esc_lt_structs`): it is rooted where its fields
point, may be a local, a parameter (`View<'a>` or `&View<'a>`, not
`&mut View`), a result (`fn make<'a>(x: &'a i64) -> View<'a>`) and a field
of another such struct, and is never stored (Vec, HashMap, Box, global,
another struct, a task). `v.r` points where `v`'s references do; `v.r = &b`
roots `v` at `b` too. In the borrow checker `val v = View { r: &a, .. }`
makes `v` a borrower of `a` (`bc_record_struct_borrow`, also `v.r = &b`:
`bc_record_field_borrow`), and a function returning one ties its result to
its arguments as for a reference.

Tests: `test_frontend_lifetime_mut` / `_mut_fail` / `_mut_back_fail`,
`test_frontend_method_read`, `test_frontend_method_mut_fail`,
`test_frontend_trait_lifetime` / `_fail` / `_undeclared_fail`,
`test_frontend_lifetime_struct` / `_struct_borrow_fail` /
`_struct_call_fail` / `_struct_block_fail` / `_struct_return_fail` /
`_struct_store_fail`, `test_frontend_lifetime_field_undeclared_fail`,
`test_frontend_ref_field_fail`.

Each tied argument is its own borrow: `r` holds the first, `r#1`, `r#2`,
... the others, and all of them end when `r` does (its last use, its scope,
or its reassignment; `release_borrow_by`). Diagnostics name `r`.

Tests: `test_frontend_lifetime_multi` (passes: unrelated changes while
the result is live, and changes after its last use),
`test_frontend_lifetime_multi_fail` (second tied argument),
`test_frontend_lifetime_method_fail` (receiver),
`test_frontend_lifetime_elided_fail` (no lifetimes),
`test_frontend_lifetime_chain_fail` (a borrow passed on).

Not done: reads are not checked against a mutable borrow (using `m`
while `r = id_mut(m)` lives is not an error); a struct's lifetime
parameters are not matched by name against a signature's (a `View<'b>`
argument and `View<'a>` parameter are one lifetime each, by position of
the parameter, not of the struct's parameters); generic structs with
lifetimes are not instantiated by lifetime.
