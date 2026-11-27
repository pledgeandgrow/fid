# VF10: the conformance suite, Nov 17

`tests/conformance/` grew from 104 tests to 1,592: at least one test for
every sentence of [docs/reference/reference.md](../reference/reference.md)
that states a rule, in every section with rules (§1 to §13; §14 and §15
list differences and other documents, and hold no rules of their own). Each
file names its section on its first line and quotes the sentence it checks;
the expected result is taken from the reference, not from what the compiler
does. Where the reference itself is out of date (the compiler was changed on
purpose after it was written), the test follows the compiler and says so in
its header, and the reference change is listed below under "Reference out of
date".

## Running it

    python scripts/verify/expect.py --fid $FID tests/conformance          # run, ccompile, emit
    python scripts/verify/expect.py --fid $FID --backends nat,x86_64-linux tests/conformance
    python scripts/verify/conformance_coverage.py --min 1000              # tests per section
    python scripts/verify/conf-known.py -- --fid $FID --backends run tests/conformance
    python scripts/verify/conf-known.py --log conf.log                    # a saved expect.py output

- `conf-known.py` sorts expect.py's results against
  `tests/conformance/known-gaps.txt` (the gaps of the table below that
  still fail, Nov 19: 44 files): NEW (fails, not listed: the run fails),
  KNOWN, FIXED (listed but passes: take the line out; `--strict` fails
  until it is).

- `expect.py` reads `//>` (an output line), `//! panic` (exit 1 after the
  `//>` lines), `//! error: words` (rejected by `fid check` with a message
  containing the words) and `//@ backends:`. It skips the directory
  `tests/conformance/_modules/`, which holds the four modules the §2.2, §2.9
  and §3.4 tests `use` (`conf_geom`, `conf_geom2`, `conf_priv`,
  `conf_narrow`).
- A panic test prints a line first: `expect.py` takes a `run` that exits
  non-zero with "error" on stderr and nothing on stdout for a compile error.
- `conformance_coverage.py` counts the tests per section (with rejections
  and panics) and exits 1 when a section with rules has none, or with
  `--min N` when there are fewer than N tests.
- Expected-output lines are compared after stripping trailing blanks, so no
  test prints a line that ends in a space.

## Numbers (Nov 17, stage-0 compiler of Nov 16, HEAD stdlib)

The compiler that ran them is the Nov 16 stage 0
(`scratchpad/ws/stage0.exe`) with `stdlib/` from `git archive HEAD` (the
working tree's `core.fd` calls `p_region_word`, which that compiler does not
have). Today's compiler changes are not in it, so some failures below are
already fixed in the tree (marked "fixed in the tree").

| Section | Tests | of which rejections | panics |
|---|---|---|---|
| §1 lexical | 296 | 42 | 0 |
| §2 items | 192 | 48 | 1 |
| §3 types | 230 | 48 | 48 |
| §4 expressions | 275 | 45 | 1 |
| §5 statements | 129 | 30 | 1 |
| §6 patterns | 39 | 11 | 0 |
| §7 generics | 26 | 8 | 0 |
| §8 memory | 130 | 43 | 24 |
| §9 effects | 33 | 18 | 0 |
| §10 concurrency | 34 | 4 | 0 |
| §11 annotations | 87 | 51 | 0 |
| §12 `@cfg` | 12 | 4 | 0 |
| §13 prelude | 109 | 2 | 4 |
| all | 1,592 | 354 | 79 |

(`conformance_coverage.py` prints the exact numbers per subsection.)

- `run` (the interpreter): 1,517 of 1,592 pass with the Nov 16 compiler;
  each of the 75 failures is in the table below (7 of them are fixed in
  the tree already).
- `ccompile` and `emit`: a sample of 37 files (every fourth file, cut short
  on a loaded machine) passed on both; the 21
  `run` failures checked on `ccompile` fail there too except 4
  (interpreter-only: CF11, CF12 ×2, CF30). The whole suite on every backend
  is for the validation run with the final compiler.

## What fails, and why

Each finding has an ID here (CF = conformance finding) and a row in
[miscompile-log.md](miscompile-log.md). "all" means `run` and `ccompile`
both fail it (emit was not checked for each).

| ID | Test(s) | What | Backends |
|---|---|---|---|
| M2 | `s4_7_nested_if_value` | an `if` whose branch value is another `if` is typed `void` (§4.7) | all (type check) |
| M3 | `s3_5_unsigned_println` | `println` of a u64 above i64::MAX prints it signed | fixed in the tree (2g.72) |
| M5 | `s3_2_array_index_read`, `_write`, `_loop`, `s3_2_const_param_length_index`, `s5_4_for_array_elements`, `s8_1_array_copy`, `s8_1_array_pass_by_value`, `s8_2_array_oob_panics` | a local `[T; N]` initialised from a literal: `run` reads the address, the C does not compile (`int64_t[N] v = 0`) | run, ccompile |
| D21 | `s4_6_float_saturate_*` (7) | float to integer out of range or NaN does not saturate | all (known) |
| D22 | `s13_3_map_str_keys_sorted`, `_bytewise`, `s13_3_map_values_in_key_order` | `str` keys of `keys()` are not in byte order | all (known) |
| CF1 | `s1_10_semicolon_after_item` | a `;` after an item (`struct P { .. };`, `fn f() {..};`) is a parse error; §1.10 says it is consumed | all (parser) |
| CF2 | `s2_struct_literal_forward` | `Dev { handle: h }` before `struct Dev` is declared: parse error at `:` (§2's own example; the field name is a word keyword) | all (parser) |
| CF3 | `s2_8_alias_vec` | `type Ints = Vec<i64>`: no method `push` on `Ints` (§2.8, §3.3 "equal types after aliases are resolved") | all (type check) |
| CF4 | `s2_9_alias_qualified_call` | `use _modules::conf_geom as g;` then `g::area(..)`: undefined function (§2.9) | all |
| CF5 | `s2_9_version_pin_rejected` | `use math = "1.0";` is accepted (§2.9: a parse error) | all |
| CF6 | `s2_9_use_in_body_does_nothing` | `use a::b;` in a function body: parse error at `;` (§2.9: parsed, does nothing) | all |
| CF7 | `s2_11_state_enum_eq` | `==` on a `state` machine's enum is false for the same variant (no `==` synthesized for the desugared enum; §2.3) | all |
| CF8 | `s3_2_const_param_length` | `for x in a` over a const-parameter array `[i64; N]`: "arithmetic requires numeric types ... [i64; N]" | all (type check) |
| CF9 | `s3_2_box_type` | `Box::new(41)` is a `Box<i8>` (§7.2: a literal given for a type parameter is an `i64`) | all (type check) |
| CF10 | `s3_2_rc_type`, `s13_3_rc_count_clone`, `s13_3_arc_get` | `Rc`/`Arc` methods: `run` "Unknown function 'Rc_get'", C "too many arguments" (§13.3) | all |
| CF11 | `s3_3_widen_i8_to_i64` | an `i8` −5 given to an `i64` reads 251 (zero- not sign-extended) | run |
| CF12 | `s3_3_int_literal_to_float`, `_int_var_to_float`, `_int_arg_to_float_param`, `_tuple_elementwise_int_to_float` | an integer given where a float is declared is not converted (its bits are read as a float) | run (C right) |
| CF13 | `s3_5_unsigned_fstring_u64` | `f"{u}"` of a u64 above i64::MAX prints it signed | all |
| CF15 | `s4_3_tuple_literal` | elements of a destructured tuple literal lose their type: a `bool` prints 1 (`run`), a `str` prints an address (C) | all |
| CF16 | `s4_8_pipe_method` | `x \|> o.m(a)`: `run` prints garbage, the C has one argument too many (§4.8) | all |
| CF17 | `s4_9_try_str_value`, `s4_9_try_bool_value` | a `try` block's `str`/`bool` value comes back as a word | all |
| CF18 | `s4_11_reflect_field_names`, `_enum_variants` | `comptime field_names(T)` / `enum_variants(E)` are typed `[str; 0]`: a `for` walks nothing | all |
| CF19 | `s5_1_shadow_in_inner_block` | `val x = 1; { val x = 2; } println(x)` prints 2: a shadowing declaration in an inner block writes the outer variable | all |
| CF20 | `s5_3_vec_param_assign_rejected` | assigning a by-value `Vec` parameter (`v = Vec::new()`) is accepted (§5.3: "by assignment to it") | all (type check) |
| CF21 | `s5_5_defer_in_loop_body` | a `defer` in a loop body runs once per iteration at function exit; §5.5 says defers attach by position | all |
| CF22 | `s8_6_budget_vec_new_counted` | a `Vec` local from `Vec::new()` counts 56 bytes; §8.6 says 24 + 24 | all (type check) |
| CF23 | `s8_6_gpu_needs_budget` | `region gpu r { }` without a budget is accepted (§8.6: same check as `kv_cache`) | all |
| CF24 | `s11_no_alloc_concat_rejected` | string `+` in a `@no-alloc` function is accepted (§11 names it) | all |
| CF25 | `s11_derive_debug_str` | `@derive(Debug)`'s `D_debug_str` shows `D(a: ?)` | all |
| CF26 | `s12_cfg_debug_removes_item`, `s12_cfg_riscv_arch_never`, `s12_cfg_all_must_hold` | a call of a function `@cfg` removed passes `fid check`; `run` then panics "Unknown function" (the filter runs after type checking) | all |
| CF27 | `s12_cfg_struct` | two `@cfg` structs of one name: the type checker keeps the first, whichever holds | all |
| CF28 | `s13_2_result_ok_err`, `_map_err`, `_context`, `_with_context` | the `str` error of `err()`, `map_err`, `context` prints as a word | all |
| CF29 | `s13_3_vec_for_each`, `_fold`, `_any_all`, `_position`, `_find`, `_map`, `_filter` | the closure methods of `Vec` (§13.3) are missing: "Unknown function 'Vec_fold'" / undeclared in the C | all |
| CF30 | `s13_3_vec_of_str` | `contains("a")` on a `Vec<str>` is false (compares addresses) | run |
| CF31 | `s13_3_map_str_keys_print` | `println` of a `Vec<str>` prints addresses | all |
| CF32 | `s11_annotation_on_param` | a `@public` parameter of a `@constant_time` function is taken for secret: "conditional branch on secret data" (the IR checks keep no `@public`: checks.fd, nat_ir's ng_ct_check). Fixed Nov 27 (c437509b, 5c5d0644 and the index fix of that evening): the type checker puts the marks down by the parameter's index in the source, checks.fd `ct_source_param` gives the IR checks that index (the lowered IR has the result pointer and the environment in front; the native paths drop the environment) | ccompile, emit |
| CF33 | `s2_1_main_needs_no_return` | `main` without a `return` exits 1073872504 | emit |
| CF34 | `s4_10_capture_by_reference` | a closure capturing by reference: exit 1, no output | emit |
| CF35 | `s4_10_closure_str` | a `str` a closure captures prints as an address | ccompile, emit |
| CF36 | `s4_11_comptime_if_generic` | `comptime if` in a generic function prints nothing | emit |
| CF37 | `s8_2_null_deref_panics` | a null dereference does not trap: exit 0 and a value printed. Fixed Nov 27: the C path compiles with `-fno-delete-null-pointer-checks` (main.fd `cc_flags`), so a load through null faults as on the other backends | ccompile |
| — | `s1_4_q4_names_variable`, `_q8_`, `s1_4_kw_handle_field_assign`, `s2_7_top_level_val_rejected` (message), `s4_1_call_result_called`, `_closure_called_inline`, `_vec_element_called` | fixed in the tree (stdlib-agent bugs 13, 36, 9, 38) | — |

K1 (a keyword-named function called in an expression) is fixed: its two
tests pass, and so do the 30 `s1_4_kw_fn_*` tests (every word keyword as a
function called inside an expression). D1 was settled by LG1 (§8.1 says
values; `s8_1_vec_is_handle.fd` expects 1).

## Reference out of date

The compiler changed on purpose after these sentences were written; the
tests follow the compiler and the reference should say:

- §1.6: digits with an exponent and no fraction (`1e5`, `5e3`) are a float
  literal (known-bugs 2g.18, lexer.fd). `s1_6_exponent_without_fraction.fd`.
- §2.4 and §11: annotations may be written on the methods of an `impl` block
  (CU3, parser.fd); not in a trait impl. `s2_4_annotation_on_method.fd`,
  `s2_4_annotation_on_trait_impl_method_rejected.fd`.
- §6: nested constructor patterns (`Some(Ok(x))`) are allowed (LG8,
  parser.fd `pd_desugar_arms`); tuple, struct and `|` patterns are not.
  `s6_nested_pattern.fd`, `s6_nested_pattern_inner_none.fd`.
- §10.1: a struct, enum or tuple that holds a string or a collection is not
  a valid spawn argument (its copies would share it; CA5): pass the strings
  and Vecs as arguments of their own. `s10_1_spawn_struct_of_send_ok.fd`
  uses a struct of numbers.
- §12: `debug` holds under `--debug` (`release` otherwise); the
  architecture arguments name the target's architecture (`cfg_arch_is`,
  x86_64 on the host), not the host's; `feature = "f"` holds for a feature
  ark turned on. The arch tests run on the x86_64 backends only
  (`//@ backends:`).
- §5.1 says nothing of a field of a `val` struct: the compiler lets it be
  assigned (`s5_1_val_field_assign.fd`, "It does not freeze the value").

## Nov 27 (night): the whole suite on three backends

With release/fid-v27.exe (the compiler of Nov 25) on run, ccompile and
emit (10:06 to 10:48, `scripts/verify/expect.py`, then
`scripts/verify/conf-known.py`): 1,607 of 1,617 files pass. The 15 gaps
listed in tests/conformance/known-gaps.txt all pass and are taken out of
it: s2_8_alias_vec (CF3), s2_9_alias_qualified_call (CF4),
s2_9_version_pin_rejected (CF5), s2_9_use_in_body_does_nothing (CF6),
s3_2_const_param_length (CF8), s8_1_array_copy and
s8_1_array_pass_by_value (M5), s8_6_budget_vec_new_counted (CF22),
s8_6_gpu_needs_budget (CF23), s11_no_alloc_concat_rejected,
s11_derive_debug_str, s12_cfg_debug_removes_item, s12_cfg_all_must_hold,
s12_cfg_riscv_arch_never (CF26) and s12_cfg_struct (CF27).

Ten failures were not listed. Four are tests of the night's fixes and
pass with the build of 09:17 (s2_8_alias_cycle_rejected,
s4_2_bitwise_float_rejected, s4_2_str_order_rejected,
s8_4_read_while_mut_borrowed_rejected). Six still fail with that build:
CF32 to CF37 above (build/verify-nov27/vf10/).
