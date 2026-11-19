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
| Inside a block entered only from a branch on `x < n`, `x <= n - 1`; the fact holds through jumps to blocks with that single way in (LF9), through branches that say nothing about it, and after a join all of whose ways in carry a fact on the same local or value (their hull; Nov 15) | `checks.fd` `peir_range_refine`, `peir_range_scan`, `peir_edge_fact`, `peir_join_fact` (a store to the local ends its fact; back edges not seen yet give none) | as LF8 (`n - 1` after `if n < 2 { return }`; `FID_IROPT_OFF=rangejoin` turns the join part off) | `test_opt_ranges.fd`, `test_midend_range_join.fd`, `test_midend_range_join_fail.fd` |
| A widening of a `u8`/`i16`/... value lies in its type's range (LF12) | `checks.fd` `peir_type_range` on loads and `IntExtend` | as LF8 | `test_opt_ranges.fd` |
| A Vec only ever filled with `Vec::filled(n, x)` and never shortened has at least `n` elements (LF8) | `checks.fd` `peir_filled_vecs` / `peir_filled_lengths` (stores and uses checked) | bounds checks dropped (sieve) | `test_vec_filled.fd`, `bench/fides/sieve.fd` |
| A value has one owner, dropped at its scope's end; reading borrows (LF15) | ownership lowering, proposal 0004, `compiler/lower.fd` "Ownership" | a share or count immediately undone is not made: Str/Box dup-drop pairs, a Vec or map shared only to be read (`lw_rc_cancel_pairs`, `LOWER_CANCEL_SHARES`), across blocks along straight chains (`LOWER_CANCEL_ACROSS`) and across if/else joins, branches and loops for copies of parameters and of local variables that are only read (`lw_rc_cancel_param_slots`: `LOWER_CANCEL_PARAM_SLOTS`, `LOWER_CANCEL_LOCAL_SLOTS`; docs/reference/rc-cancel.md) | `tests/leaks/*.fd` (`scripts/leak-check.sh`), `tests/rc_cancel/check.sh` |
| A string variable only ever assigned literals holds nothing counted (LF17) | literals are `[P_STR_STATIC][len]`; `Str_dup`/`Str_drop` leave them alone (`stdlib/core.fd`) | its counts and drop removed (`lw_rc_literal_slot_counts`, `LOWER_LITERAL_SLOTS`) | `tests/leaks/ownership.fd` |
| A local Vec whose value only reaches core's Vec functions never leaves its function (LF13) | every keep of a Vec goes through `Vec_share` (a new header); the pass checks every store and use of the slot (`lw_rc_stack_vecs_fn`) | its header lives on the stack (`Vec_init_typed`, `Vec_drop_inplace`; `LOWER_STACK_VECS`) | `test_vec_filled.fd` (`local_sum`), `tests/leaks/value_semantics.fd` |
| A `@constant_time` function's code is checked by the backend, function by function (LF18) | `pe_ir.fd` `peir_ct_check`, `nat_ir.fd` `ng_ct_check` (not the C path) | iropt never inlines a `@constant_time` function into one without the annotation (`iro_body`, `ime_inline_function`) and never lowers switches in one (`ime_function_passes`) | `test_midend_ct_inline.fd`, `test_constant_time_*_fail.fd` |
| A `@zeroize` local's clearing store is kept (LF19) | the lowering emits it at scope exit; iropt's `dse` (opt-in, `FID_IROPT_ON=dse`) never touches `zeroize_locals`; rules.fd and the e-graph never remove a store | dead-store elimination skips it | `test_return_order.fd` (`@zeroize`) |
| A call to a function that, with everything it calls, reads, writes, allocates and prints nothing gives the same value for the same arguments, and changes nothing (WP9, OP25) | computed from the IR, not trusted from annotations: `iropt.fd` `iro_effect_masks` (each function's own effects outside its frame joined with its callees', a fixpoint over recursion; `@crashfree`/`@no-alloc` take away a panic/allocation under the @wrapping trust model) | merged under dominance (`rules.fd` `ru_pure_cse`); moved out of a loop's header when only quiet instructions come before it there, a panic included (`iro_licm_pure_call`, `iro_licm_loop`; `FID_IROPT_OFF=licmcall`) | `test_midend_effects_deep.fd`, `test_midend_licm_header.fd`, `test_midend_licm_header_fail.fd` |

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
query `ime_addr`, `loadhoist`, `forward`, `matchfacts`, and since Nov 15
the overwritten stores `dse` drops, `ime_overwritten_stores`) but only run with
`FID_IROPT_ON`, and the alias query assumes an unmarked raw pointer
parameter is a disjoint `&mut` (docs/file-analysis/iropt_me.md): not facts
until that is fixed and they run on the suite. Since Nov 9 lower.fd sets
`borrow_kind` on reference parameters (LF1, part). Facts through function
boundaries (LF10), check hoisting (LF11, needs loop versioning) and
`Option`/`Result` in registers (LF24) are open; see the roadmap.
