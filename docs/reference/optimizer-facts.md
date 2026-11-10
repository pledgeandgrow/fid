# Language facts the optimizer uses

Fides promises things about programs that C does not: a reference cannot
escape, a value has one owner, a counted loop's index is in bounds. The
compiler turns some of these promises into facts it optimizes with. Each
fact below says what the optimizer assumes, which rule of the language or
the compiler makes it true, where the code is, and the test that fails if
the rule stops holding. A fact without a rule that enforces it is a
miscompilation waiting to happen, so a fact is listed here only once its
rule exists. (docs/roadmap/llvm-roadmap.md, Track LF, LF25.)

| Fact | Guaranteed by | Used in | Breaks |
|------|---------------|---------|--------|
| A reference never outlives its target and is never stored, returned from a local or sent to a task (LF2) | the escape check, `compiler/escape.fd` (session 29) | reasoning about loads through references | `test_escape_*_fail.fd` (rejected), `test_free_escape.fd` |
| `v.get(i)` / `v.set(i, x)` inside `while i < v.len()` or `for i in 0..v.len()` is in bounds (LF7) | the counted-loop analysis, `compiler/checks.fd` `peir_counted_loop` | bounds checks dropped (C, native) | `test_counted_loop.fd` |
| An add, sub or mul whose operands' ranges keep the result inside i64 cannot overflow (LF8) | integer ranges, `checks.fd` `peir_range_ops` (constants, `% c`, `& c`, narrow types, locals' stores) | overflow checks dropped (C, native) | `test_opt_ranges.fd`, `test_opt_ranges_pop_fail.fd` |
| Inside a block entered only from a branch on `x < n`, `x <= n - 1`; the fact holds through jumps to blocks with that single way in (LF9) | `checks.fd` `peir_range_refine`, `peir_range_scan` (single predecessor, inherited through `Jump`) | as LF8 (`n - 1` after `if n < 2 { return }`) | `test_opt_ranges.fd` |
| A widening of a `u8`/`i16`/... value lies in its type's range (LF12) | `checks.fd` `peir_type_range` on loads and `IntExtend` | as LF8 | `test_opt_ranges.fd` |
| A Vec only ever filled with `Vec::filled(n, x)` and never shortened has at least `n` elements (LF8) | `checks.fd` `peir_filled_vecs` / `peir_filled_lengths` (stores and uses checked) | bounds checks dropped (sieve) | `test_vec_filled.fd`, `bench/fides/sieve.fd` |
| A value has one owner, dropped at its scope's end; reading borrows (LF15) | ownership lowering, proposal 0004, `compiler/lower.fd` "Ownership" | a share or count immediately undone is not made: Str/Box dup-drop pairs, a Vec or map shared only to be read (`lw_rc_cancel_pairs`, `LOWER_CANCEL_SHARES`) | `tests/leaks/*.fd` (`scripts/leak-check.sh`) |
| A string variable only ever assigned literals holds nothing counted (LF17) | literals are `[P_STR_STATIC][len]`; `Str_dup`/`Str_drop` leave them alone (`stdlib/core.fd`) | its counts and drop removed (`lw_rc_literal_slot_counts`, `LOWER_LITERAL_SLOTS`) | `tests/leaks/ownership.fd` |
| A local Vec whose value only reaches core's Vec functions never leaves its function (LF13) | every keep of a Vec goes through `Vec_share` (a new header); the pass checks every store and use of the slot (`lw_rc_stack_vecs_fn`) | its header lives on the stack (`Vec_init_typed`, `Vec_drop_inplace`; `LOWER_STACK_VECS`) | `test_vec_filled.fd` (`local_sum`), `tests/leaks/value_semantics.fd` |
| A `@zeroize` local's clearing store is kept (LF19) | the lowering emits it at scope exit; iropt's `dse` (opt-in, `FID_IROPT_ON=dse`) never touches `zeroize_locals`; rules.fd and the e-graph never remove a store | dead-store elimination skips it | `test_return_order.fd` (`@zeroize`) |

## Rules for adding a fact

1. The rule comes first: a check that rejects programs breaking it, or a
   lowering that makes it true by construction.
2. A test that fails when the rule is broken (a `_fail` test for a rejected
   program, a leak program for ownership), named in the table.
3. A switch to turn the use off (`LOWER_*` in `lower.fd`, `FID_IROPT_OFF`,
   `FID_NAT_OFF`), so a wrong fact can be bisected.

## Not yet facts

Alias facts from `&mut` and from ownership (LF1, LF3–LF6) and
pattern-match facts (LF23) are written in `compiler/iropt_me.fd` (the alias
query `ime_addr`, `loadhoist`, `forward`, `matchfacts`) but only run with
`FID_IROPT_ON`, and the alias query assumes an unmarked raw pointer
parameter is a disjoint `&mut` (docs/file-analysis/iropt_me.md): not facts
until that is fixed and they run on the suite. Since Nov 9 lower.fd sets
`borrow_kind` on reference parameters (LF1, part). Facts through function
boundaries (LF10), check hoisting (LF11, needs loop versioning) and
`Option`/`Result` in registers (LF24) are open; see the roadmap.
