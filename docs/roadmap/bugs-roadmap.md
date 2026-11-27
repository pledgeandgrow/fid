# Bugs roadmap

A list of defects in the self-hosted compiler (`compiler/*.fd`), found by
reading the code on Nov 22 2026: ten review passes over the 56 modules that
`compiler/main.fd` reaches (the 24 modules nothing reaches were left out).
Each entry names the place, what is wrong, and an input that triggers it.

How far to trust it:
- **Not run.** Every entry comes from reading the source; none was
  confirmed by building or running a program, except where an entry says so.
  "Confidence" says how sure the reviewer was.
- **New only.** Entries already in `docs/reference/known-bugs.md` (§2g) or
  in `docs/file-analysis/` were left out, and so were the bugs fixed in the
  working tree on Nov 22 (strict string headers, M10 in the stdlib, the
  generic-struct Vec push, the nat `@constant_time` and DLL fixes).
- **Line numbers** are those of Nov 22 evening; `lower.fd` was being edited
  during the review, so its function names are the reliable reference.

Severity: **critical** = silent wrong code or a safety guarantee broken;
**high** = a crash or wrong result on a plausible program, or a check that
accepts an unsafe program; **medium** = narrower conditions; **low** =
latent (no current caller) or minor.

Counts at the review: 17 critical, 38 high, 34 medium, 17 low (106 open
entries), plus one found and fixed during the review (BG100).

Status Nov 24 (after the Nov 22-24 fix batches, built and run on run, C,
emit, nat and the tooling checks): 127 entries, 127 done `[x]`, 0 open.
The Nov 24 batch closed the last of the review list: BG85, BG108 (in
ssa.fd `ssa_materialize`), BG115, BG116 (already consistent), BG121,
BG123, and BG126 (fixed-array returns routed through sret) — and the
last three partly-done marks: BG40's remaining `p_vec_clone` and
`p_vec_zeroize` element-code gaps, BG45 and BG46.

Nov 24 review: 114 new entries, BG128-BG241 (22 critical, 51 high, 29
medium, 12 low); see "Found by the Nov 24 whole-repo review" below.
Status Nov 25 (the Nov 24 and Nov 25 fixes, run with the Nov 25 fid on
run, C, emit, and --nat / `fid wasm` where an entry names them): 71 done
`[x]`, 8 partly done `[~]` (BG159, BG165, BG166, BG182, BG183, BG185,
BG204, BG227), 35 open. BG242-BG246, found Nov 25, are open.
Status Nov 27 (checkbox count at be36317, the v0.8.5-unstable commit):
BG128-BG241: 80 done `[x]`, 3 partly done `[~]` (BG183, BG204, BG227), 31
open; BG242-BG246 open. The whole file: 207 `[x]`, 3 `[~]`, 36 `[ ]`.

Status markers as in the other roadmaps: `[ ]` open, `[~]` partly done,
`[x]` done.

## Critical

- [x] BG1. **`name[i] = v` is not type-checked.** compiler/typechecker.fd:19183
      (`Stmt::IndexAssign`) checks only that the name exists: not that the
      target can be indexed, the index is a number, or the value matches the
      element type. Lowering (lower.fd `Lowerer_lower_index_assign_stmt`) then
      emits `GetElemPtr` + `Store` on an `i64` or `str` variable: a store to
      memory in safe code. Trigger: `var p: i64 = 4096; p[0] = 7;`,
      `v[0] = 42` with `v: Vec<str>`. Confidence: high (checked twice).
      Fixed Nov 22: `TypeChecker_check_index_assign` (target a Vec or array,
      an integer index, the value against the element type); tests
      test_index_assign_{target,value,index}_fail, test_index_assign_ok.
- [x] BG2. **`*x = v` is accepted for any `x`.** typechecker.fd:19217
      (`Stmt::DerefAssign`): only RawPtr (needs unsafe) and `&T` (rejected)
      are matched, everything else falls to `_ => {}`; the value is not
      checked against the pointee type. Trigger: `var a: i64 = 4096; *a = 5;`,
      `*r = "text"` with `r: &mut i64`. Confidence: high (checked twice).
      Fixed Nov 22: `TypeChecker_check_deref_value` / `_check_deref_other`
      (`&mut T` value checked, Box<T> accepted, anything else rejected; the
      compiler's own modules keep a struct/enum exception for
      comptime_eval.fd:2981); tests test_deref_assign_{nonref,value}_fail,
      test_deref_assign_checked. Not done: a raw pointer's value against its
      pointee.
- [x] BG3. **Range analysis keeps a join-block fact across a back edge when
      the join block itself stores the local**, so overflow checks are
      dropped (C, pe_ir and nat). compiler/checks.fd:2752-2790
      (`peir_join_stored` skips block `b`). Trigger: `loop { n = n * 3; if n <
      0 { println("wrapped"); return; } }` prints "wrapped" instead of
      panicking. Confidence: high.
      Fixed Nov 22 (`peir_join_stored` counts a store in the join block);
      test_range_backedge_store.
- [x] BG4. **Counted-loop proof drops `i = i + 1`'s overflow check when the
      step is in an inner loop.** checks.fd:686-731 (`peir_counted_loop`)
      lacks the "runs once per header test" check that `peir_counted_down`
      (:1336) and `iro_iv_bounded` have. Trigger: an outer `while i < n` with
      `i = i + 1` inside a `while j < 5`. Confidence: high.
      Fixed Nov 22 (`peir_counted_loop` requires the step to run once per
      header test); test_counted_loop_inner_step.
- [x] BG5. **Range analysis ignores `@wrapping`:** bounds checks are removed
      and pe_ir uses unsigned `%`/`/` on negative values. checks.fd:3366-3428,
      4591-4614, 3604-3616 assume checked arithmetic everywhere. Trigger: in a
      `@wrapping` fn, `s = s + 7046029254386353131; v.set(s % 10, i)` writes
      index -3. Confidence: high.
      Fixed Nov 22 (`peir_fn_wraps`: in a `@wrapping` function, and in the
      runtime functions compiled without overflow checks, an Add/Mul that may
      overflow gives no bound); test_range_wrapping.
- [x] BG6. **Assigning to a type-parameter field of a generic instance takes
      no owner and drops nothing** (double free, leak; a struct field is
      stored as a stack address that is later `p_free`d). lower.fd
      `Stmt::FieldAssign` (~11512) via `Lowerer_rc_field_ast_type`, which does
      not substitute the instance's type arguments. Trigger: `var h: H<str> =
      H { v: "a" + "b" }; val s = "x" + "y"; h.v = s;`. Confidence: high (str).
      Fixed Nov 22 (lower.fd FieldAssign takes the instance's type arguments,
      a struct field gets a heap copy; typechecker.fd BG111 made the
      assignment type-check); test_generic_field_assign_owner passes on run, C
      and emit.
- [x] BG7. **`v[i]` / `for x in v` on a `Vec<f32>` reads the wrong 32 bits in
      C.** compiler/codegen_c.fd:6889-6891 (`cg_vec_get_word`) puns the low 32
      bits; stores keep the f64 bits (the OP21 fix reached the call path only).
      Trigger: `v.push(1.5 as f32); println(v[0])` prints 0. Confidence: high.
      Fixed Nov 22-23: codegen_c.fd reads the element word's F64 bits (the
      OP21 layout), pe_ir.fd's movq is 0F 7E and its inlined Vec paths leave
      f32 to the call (BG109); test test_vec_f32_index passes on run, C, emit
      and nat.
- [x] BG8. **On one worker, a task blocking on a channel hangs the thread.**
      codegen_c.fd:16018-16025 / 16055-16062: with `__p_mn_on()` false
      (FIDES_WORKERS=1, FIDES_SCHED_RANDOM, one CPU) `p_chan_recv` sleeps on
      the condition variable on the only thread. Trigger: an async consumer
      receiving before main sends, run with FIDES_WORKERS=1. Confidence: high.
      Fixed Nov 22 (`__p_chan_coop_wait`: a blocked task yields to the
      scheduler on one worker); test_chan_one_worker.
- [x] BG9. **The thread/future handle table overflows after 256 spawns and
      is not thread-safe.** codegen_c.fd:13520-13522, 15613, 15882, 15628,
      15769: `int idx = p_thread_handle_count++;` unbounded, never reused, not
      atomic. Trigger: 300 `spawn`/`await` in a loop. Confidence: high.
      Fixed Nov 22 (chunked, locked handle table `__p_ft_*`, slots reused);
      test_spawn_many. The native runtimes' own 256-thread cap is BG110.
- [x] BG10. **Core's raw-memory functions are callable without `unsafe`.**
      typechecker.fd:16538 (`tc_raw_memory_builtin`) lists p_malloc/p_free/...
      but not stdlib/core.fd's `pub fn p_mem_copy`, `p_copy_words`,
      `p_str_free`. Trigger: `p_mem_copy(4096, 8192, 64);` in safe code.
      Confidence: high (type-checks).
      Fixed Nov 22 (`tc_core_raw_memory_fn`: p_mem_copy, p_copy_words,
      p_zero_words, p_str_copy_n, p_str_free, p_str_append, p_str_adopt; the
      stdlib call sites wrapped); tests test_core_raw_memory_fail,
      test_core_str_free_fail, test_core_raw_memory_unsafe.
- [x] BG11. **A variant pattern without its enum takes its tag from the
      wrong enum.** lower.fd `Lowerer_lower_match_expr` (~15937) and
      `Lowerer_lower_match` (~19866): `X(n)` scans every enum with no break
      (last wins) while payload types come from the first; the scrutinee's
      type is not used. Trigger: `enum A { X(i64), Y(i64) } enum B { Y(i64),
      X(i64) }`, `match A::X(5) { X(n) => .., Y(n) => .. }` takes Y.
      Confidence: high.
      Fixed Nov 22 (the scrutinee's enum first:
      `Lowerer_scrutinee_pattern_enum`, `Lowerer_pattern_variant_index`);
      test_match_bare_variant_scrutinee.
- [x] BG12. **Borrow check: a reader hidden under a cast, closure, tuple,
      `is` or pipe does not keep the borrow alive.** compiler/borrowck.fd:1496
      (`collect_var_uses_expr`) and 2349 (`bc_never_read`). Trigger: `val e:
      &i64 = &v[0]; v.push(2); println(int_to_str((*e) as i64));` is accepted
      (dangling read). Confidence: high.
      Fixed Nov 22 (readers under casts, closures, tuples, `is` and pipes
      counted); test_borrow_hidden_read_fail.
- [x] BG13. **Two-phase borrows accept `g(&mut a, &a)`.** borrowck.fd:2287-2332
      (`create_borrow`: the shared temporary is released before the reserved
      `&mut` is activated), 2876-2881. The reverse order is rejected.
      Confidence: high.
      Fixed Nov 22, narrowed: `g(&mut a, &a)` and `g(&mut s, &s.x)` are the
      documented two-phase rule (CM2, test_borrowck.fd Phase 4) and memory-
      safe (they point at the variable). The unsafe case, a shared reference
      into the Vec's buffer (`g(&mut v, &v[0])`, `&s.v[i]`), is rejected
      (`bc_expr_through_index`, `ref_into_elem`);
      test_borrow_two_phase_shared_fail. Not done: `g(&mut s.x, &s)`
      (comptime_eval.fd passes `&mut prog.funcs, .., &prog`).
- [x] BG14. **A changing method through a field path is never borrow-checked.**
      borrowck.fd:2956-2967 (MethodCall: `receiver_path` only for Ident),
      2581-2603 (For: `iter_var` only an Ident). Trigger: `val r = &s.items;
      s.items.push(2); println(r.len())`; `for x in s.items { s.items.push(x) }`.
      Confidence: high.
      Fixed Nov 22 (MethodCall and For take a field path); tests
      test_borrow_field_method_fail, test_borrow_for_field_fail.
- [x] BG15. **A loop body is borrow-checked once**, so a borrow made late in
      an iteration is not seen by the next. borrowck.fd:2563-2615 (While, For,
      Loop: no second pass). Trigger: `while .. { v.push(i); print(*e); e =
      &v[0]; }`. Confidence: high.
      Fixed Nov 22 (a second pass over a loop body when the first leaves a
      borrow carried into the next iteration); test_borrow_loop_carried_fail.
- [x] BG16. **Escape check: `o.unwrap()` on an `Option<&T>` loses its roots**,
      so a reference to a local can be returned. compiler/escape.fd:1878-1925
      (`esc_method_call`: roots flow only when the method's return type
      contains a reference, and `T` does not before mono). Trigger: `return
      Option::Some(&x).unwrap();` with `x` a local. Confidence: high.
      Fixed Nov 22 (`esc_holder_type`: a method of Option, Result or a struct
      with lifetime parameters whose result is or holds a type parameter,
      `Option_unwrap -> T`, passes the receiver's roots on);
      test_escape_unwrap_fail.
- [x] BG17. **Function-typed arguments: parameter variance is reversed.**
      typechecker.fd:23112-23126 (`TypeChecker_arg_compatible`, Func arm):
      `fn(&mut T)` is accepted where `fn(&T)` is expected (a callback mutates
      through a shared reference). Confidence: high.
      Fixed Nov 22 (`TypeChecker_fn_param_compatible`: a `&mut` callback
      parameter where `&`/a value is expected is rejected, `&T` where `&mut T`
      is expected accepted); tests test_fn_param_variance_fail,
      test_fn_param_variance.

## High

- [x] BG18. **Method calls check neither argument count nor types.**
      typechecker.fd:22720-22760, 22791-22854 (`TypeChecker_check_method_call`),
      22029-22038 (associated fns: count only). Trigger: `P::make("str");
      p.set(); p.set("a", 1, 2);`. Confidence: high.
      Fixed Nov 22-23 (typechecker.fd: method and associated-fn arguments
      checked for count and type); tests test_method_args_{count,type}_fail,
      test_assoc_fn_arg_type_fail, test_method_args_ok pass on run, C and
      emit.
- [x] BG19. **`if`/`match` as values: branch types are never compared, and an
      expression `if` does not check its condition.** typechecker.fd:21027-21050,
      21188-21243. Trigger: `val n: i64 = if true { 5 } else { "five" };`.
      Confidence: high.
      Fixed Nov 22-23 (typechecker.fd: branch types compared, the condition of
      an expression `if` checked; the lowering half is BG125); tests
      test_if_value_branch_types_fail, test_if_value_cond_fail,
      test_match_value_arm_types_fail, test_if_match_value_ok pass on run, C
      and emit.
- [x] BG20. **Use-before-assignment (F63) is bypassed by `match`, `while`,
      `for` and expression `if`** (only `Stmt::If` merges `uninit_vars`).
      typechecker.fd:19323, 19338, 19435, 21027. Trigger: `var x: i64; match k
      { 1 => { x = 5; } _ => {} } println(x);`. Confidence: high.
      Fixed Nov 22-23 (typechecker.fd: `match`, `while`, `for` and expression
      `if` merge `uninit_vars`); tests
      test_init_{match,while,for,if_value}_fail pass on run, C and emit.
      lsp.fd keeps an exemption (BG123).
- [x] BG21. **`@constant_time` methods are never source-checked.**
      typechecker.fd:7669-7718 (`TypeChecker_verify_constant_time` loops over
      `program.funcs` only). Trigger: an impl method `@constant_time fn eq(..)`
      with an early return on a secret is accepted. Confidence: high.
      Fixed Nov 22-23 (typechecker.fd: impl methods go through the
      @constant_time source check); test test_ct_method_early_return_fail pass
      on run, C and emit.
- [x] BG22. **Effect inference ignores method calls; method and trait-impl
      verifiers are missing.** typechecker.fd:24243-24255, 4418-4472, 4623,
      4658, 8453: `@deterministic`, declared effects and `@realtime` are
      evaded through one helper that calls `.log()`. Confidence: high.
      Fixed Nov 22-23 (typechecker.fd: method calls feed effect inference;
      methods and trait impls verified); tests test_det_method_io_fail,
      test_det_through_method_fail pass on run, C and emit.
- [x] BG23. **`?` accepts an error of a different type and copies its bits.**
      typechecker.fd:21311-21340, 17166; lower.fd `Lowerer_lower_try_expr`.
      Trigger: `g()?` with `g: Result<i64, i64>` inside a `Result<i64, str>`
      function; the `str` payload is the address 5. Confidence: high (accepted).
      Fixed Nov 22-23 (typechecker.fd: `?` requires the same error type); test
      test_try_error_type_fail pass on run, C and emit.
- [x] BG24. **The race check and `@send` skip `parallel { }` branches**, which
      run on OS threads. typechecker.fd:19710-19723; lower.fd
      `Lowerer_spawn_call` / `Lowerer_lift_parallel_branch`. Trigger: `parallel
      { | bump(); | bump(); }` with `bump` writing a global. Confidence: high.
      (Found by two passes.)
      Fixed Nov 22-23 (typechecker.fd: `parallel { }` branches are race-check
      threads, their arguments @send); tests
      test_parallel_{race,global_write,send}_fail pass on run, C and emit.
- [x] BG25. **The race check misses field writes and method calls on a global.**
      typechecker.fd:19238-19322 (FieldAssign records no write; only 19130 and
      19205 do). Trigger: `G.n = G.n + 1` or `G.push(1)` in a spawned fn.
      Confidence: high.
      Fixed Nov 22-23 (typechecker.fd: field writes and method calls on a
      global count as writes); tests test_race_field_write_fail,
      test_race_global_push_fail pass on run, C and emit.
- [x] BG26. **The race check does not follow method calls.**
      typechecker.fd:24681-24693 (`collect_called_funcs_expr`), 3870-3877.
      Trigger: `c.bump()` writing a global from a spawned worker. Confidence: high.
      Fixed Nov 22-23 (typechecker.fd: the race check follows method calls);
      test test_race_method_call_fail pass on run, C and emit.
- [x] BG27. **`inline for` does not substitute the loop variable in f-strings,
      match, tuples and other forms, and ignores shadowing.**
      compiler/comptime_eval.fd:3429-3561, 3353-3427. Trigger: `val i = 100;
      inline for i in 0..3 { println(f"{i}"); }` prints 100 three times.
      Confidence: high.
      Fixed Nov 22-23 (comptime_eval.fd: substitution in f-strings, match,
      tuples; shadowing respected); test test_comptime_inline_for_subst pass
      on run, C and emit. Left: bindings in the body leak into the enclosing
      scope (BG123).
- [x] BG28. **Lowering a closure discards the enclosing function's `defer`s and
      parameter types.** lower.fd:16321-16341 (`Lowerer_lower_closure_expr`) →
      `Lowerer_lower_func_body` resets `defer_stack` and
      `current_func_param_types`, never restored. Trigger: `defer { println(
      "cleanup"); } val f = |a: i64| a + 1;` prints no "cleanup". Confidence: high.
      Fixed Nov 22-23 (lower.fd `Lowerer_lower_closure_expr` saves and
      restores `defer_stack` and the parameter types); test
      test_closure_keeps_defer pass on run, C and emit.
- [x] BG29. **Move-on-last-use ignores match guards**, so a guard reads a moved
      value. compiler/cowcheck.fd:806-814, 913-921, 1089-1098. Trigger: `v.push(s);
      match n { 1 if s == "ab" => .. }`. Confidence: high.
      Fixed Nov 22-23 (cowcheck.fd: guards count as uses); test
      test_match_guard_last_use pass on run, C and emit.
- [x] BG30. **`+=` on a `str` that is not an owned local adds the pointers**
      (global, match binding, struct field, Vec element). lower.fd:11028-11034,
      11158-11174, 11537-11545, 9651-9661. Trigger: `p.name += "!"`. Confidence: high.
      Fixed Nov 22-23 (lower.fd: `+=` on any str place concatenates); test
      test_str_add_assign_places pass on run, C and emit.
- [x] BG31. **lockcheck merges per-function value ids into one graph**: false
      cycles stop `fid check`, real cross-function cycles are missed.
      compiler/lockcheck.fd:165-171, 397-499. Confidence: high.
      Fixed Nov 22-23 (lockcheck.fd: locks named per function, a global by its
      name, a parameter by what the caller passes); test_lockcheck_names_ok
      passes `fid check` (the Nov 22 compiler reported a false cycle on it),
      and test_lockcheck_cycle_fail is still rejected.
- [x] BG32. **Value-form `match` lacks the statement form's copy fix for
      changed struct bindings** (double free). lower.fd
      `Lowerer_lower_match_expr` (no `Lowerer_mb_owns_copy`). Confidence: medium.
      Fixed Nov 22-23 (lower.fd: the value form of `match` makes the copy too;
      its result type is BG125); test test_match_value_binding_changed pass on
      run, C and emit.
- [x] BG33. **@secret: a helper writing an argument into a `&mut` parameter is
      not summarised.** compiler/secrecy.fd:649-663, 1094. Trigger: `put(&mut v,
      key); println(v.get(0));`. Confidence: high.
      Fixed Nov 22-23 (secrecy.fd: a helper writing an argument into a `&mut`
      parameter is summarised); test test_secret_ref_param_write_fail pass on
      run, C and emit.
- [x] BG34. **Imported modules lose `layer`/`model`/`component`/`system`/
      `state_machine`/`on` declarations when merged.** compiler/compiler.fd:9000-9161
      (`merge_programs`). Trigger: `use shapes;` with a `component` in
      shapes.fd. Confidence: high.
      Fixed Nov 22-23 (compiler.fd `merge_programs` keeps the declarations);
      test test_import_component_decls (with decl_lib/shapes.fd) pass on run,
      C and emit.
- [x] BG35. **The import cache (CB1) drops `comptime splice` from imported
      modules on a hit.** compiler.fd:659-671 (`cached_parse`): `g_saw_splice`
      is never set from a cached AST. Trigger: the second build of a program
      importing a module that uses `comptime splice`. Confidence: high.
      Partly fixed Nov 22-23 (compiler.fd `cached_parse`: a module that saw a
      `comptime splice` is never kept in the import cache);
      tests/tooling/impcache-splice-check.sh passes. Fixed Nov 24
      (compiler.fd `astcache_save`: the merged AST artifact is not written
      when a library source — core or a runtime file — mentions `splice`,
      so a decoded merged AST can never replay a splice silently).
- [x] BG36. **A reused cached artifact is not executable on Linux/macOS.**
      compiler/inc.fd:1233; main.fd:5360-5363 (no chmod). Trigger: `fid run x.fd
      --leaks` twice: "Permission denied". Confidence: high.
      Fixed Nov 22-23 (inc.fd, main.fd: the reused artifact is made
      executable); tests/tooling/path-spaces-check.sh passes.
- [x] BG37. **The lowered-IR cache salt misses constants and type aliases, and
      a named cache file has no compiler identity.** compiler/ir_ser.fd:1314-1368,
      1410-1441. Trigger: `emit --ir-cache`, change a `const`, emit again.
      Confidence: medium.
      Fixed Nov 22-23 (ir_ser.fd: the salt covers constants and type aliases,
      a named cache file carries the compiler identity);
      tests/tooling/ircache-const-check.sh passes.
- [x] BG38. **Interpreter: same-width unsigned→signed casts keep the unsigned
      value.** compiler/interp.fd:4776-4803 (`Interpreter_cast_value`). Trigger:
      `200u8 as i8` stays 200 under `fid run`, then `+ 1` reports overflow.
      Confidence: high.
      Fixed Nov 22-23: interp.fd `Interpreter_cast_value` wraps a same-width
      cast, and pe_ir.fd re-extends a narrow BitCast result
      (`peir_norm_narrow`, M7); test test_same_width_int_cast pass on run, C
      and emit.
- [x] BG39. **C runtime `p_socket_write` raises SIGPIPE** (a server dies when a
      client disconnects). codegen_c.fd:16533-16540 (`write`, no MSG_NOSIGNAL).
      Confidence: high.
      Fixed Nov 22-23 (codegen_c.fd: no SIGPIPE from `p_socket_write`); test
      test_socket_write_closed_peer pass on C (a ccompile-only test).
- [x] BG40. **C runtime `p_vec_push/set/insert` ignore element codes and shared
      buffers** (heap overflow on `Vec<u8>`, abort on a shared Vec).
      codegen_c.fd:11620-11644, 16288-16293. Confidence: medium-high.
      Partly fixed Nov 22-23 (codegen_c.fd: push, set and insert go through
      the element code and the share box as core's Vec does); test
      test_cback_vec_push_codes pass on C (a ccompile-only test). Fixed Nov 24
      (codegen_c.fd: `p_vec_pop`, `p_vec_remove`, `p_vec_reverse`,
      `p_vec_sort` and `p_vec_truncate` delegate to core's Vec functions —
      element size and shared-buffer ownership handled there — and
      core.fd `vec_raw_reverse` swaps a struct element through a copy, as
      its address is not its value); test test_cback_vec_ops_codes pass on C (a ccompile-only test). Also Nov 24: `p_vec_clone` keeps the element
      code and type id (a coded Vec cloned to word slots — an inline
      struct's element address stored raw, pointing into the original's
      buffer) and `p_vec_zeroize` sizes the element as `__p_sens_esz`
      does — an inline-struct code (>= 16) wiped `cap * code` bytes,
      past the buffer's end. A `@zeroize Vec<P>` test with a second Vec
      after it prints 365/done on C — the trailing buffer intact.
- [x] BG41. **C runtime `p_read_line` splits lines longer than 4095 bytes.**
      codegen_c.fd:12688-12696. Confidence: high.
      Fixed Nov 22-23 (codegen_c.fd: `p_read_line` reads a whole line); test
      test_read_line_long pass on C (a ccompile-only test).
- [x] BG42. **SCCP folds a same-width BitCast without normalising**, so a later
      IntExtend is wrong. compiler/iropt.fd:4649-4666. Trigger: `(x as i32) as
      i64` of `u32::MAX` gives 4294967295, not -1 (emit). Confidence: high.
      Fixed Nov 22-23 (iropt.fd: SCCP normalises a same-width BitCast); test
      test_sccp_bitcast_normalise pass on run, C and emit.
- [x] BG43. **Inlining substitutes I64 literals for `u64` parameters**, turning
      unsigned compares signed. iropt.fd:1787-1794; compiler/iropt_me.fd:872-879.
      Confidence: medium.
      Fixed Nov 22-23 (iropt.fd, iropt_me.fd: an inlined literal takes the
      parameter's type); test test_inline_unsigned_param pass on run, C and
      emit.
- [x] BG44. **Function cache: a cached `@constant_time` callee makes a
      recompiled `@constant_time` caller fail** ("not @constant_time").
      compiler/nat_ir.fd:30873-30877, 37824-37826 (`nc_skel_fn` never adds the
      label). Confidence: high.
      Fixed Nov 22-23 (nat_ir.fd `nc_skel_fn` puts a replayed @constant_time
      function's label in `ct_labels`); tests/nattools/nat_fncache_ct_check.sh
      passes on x86-64, AArch64 and RISC-V (version B: 273-274 cache hits, one
      function compiled, no complaint; the x86-64 images run under WSL). The
      check builds both versions from one path: the cache salt covers the
      input file's name.
- [x] BG45. **Spawn scopes join detached threads; `p_thread_join` leaves the
      handle so it is joined twice** (UB; use after free on Windows).
      codegen_c.fd:15681-15691, 15668, 15900-15907. Confidence: high.
      Partly fixed Nov 22-23 (codegen_c.fd: a scope joins only the threads it
      started, `p_thread_join` clears the handle, `p_co_detach` lets a thread
      future go); test test_spawn_scope_outer_thread pass on C (a ccompile-only test).
      Fixed Nov 24 (lower.fd: `spawn detached f()` calls `p_co_detach` for a
      thread's future too; pe_runtime/conc.fd and linux_runtime/conc.fd
      `pert_co_detach` take a thread future, and their executor scans join
      only threads the scope started); test
      test_spawn_scope_detached_thread pass on C and emit.
- [x] BG46. **Awaiting the same future twice gives two owners of one result**
      (double free). lower.fd `Expr::Await` (~18248); codegen_c.fd:13524.
      Confidence: medium-high.
      Partly fixed Nov 22-23 (lower.fd: a future awaited more than once gives
      each await an owner of its own); test test_await_twice_owners pass on
      run, C and emit. Fixed Nov 24 (lower.fd: the await counter walks loop
      bodies at a depth — one `await x` in a loop's body or condition
      counts as awaited again); test test_await_loop_owners pass on run, C
      and emit.
- [x] BG47. **Comptime rejects valid `u64` literals** in any function holding a
      comptime expression. comptime_eval.fd:460-463 (`ct_check_int_fit`).
      Trigger: `val m: u64 = 18446744073709551615; val k = comptime 2 + 3;`.
      Confidence: high.
      Fixed Nov 22-23 (comptime_eval.fd `ct_check_int_fit` takes u64
      literals); test test_comptime_u64_literal pass on run, C and emit.
- [x] BG48. **pe_ir allocator spinlock RIP-relative operands miss the trailing
      imm8** (the lock is a misaligned 8-byte RMW at abase+25 overlapping the
      free lists). compiler/pe_ir_stubs.fd:4622-4647; pe_ir_enc.fd:2012.
      Confidence: high on the arithmetic.
      Fixed Nov 22-23 (pe_ir_stubs.fd: the RIP-relative spinlock operands
      count the trailing imm8); test test_pe_alloc_lock_threads pass on run, C
      and emit.
- [x] BG49. **pe_ir `p_str_concat` reads through a null right operand.**
      pe_ir_stubs.fd:5685-5692 (copies len+1 from NULL; C treats null as "").
      Confidence: high on the mechanism.
      Fixed Nov 22-23 (pe_ir_stubs.fd: a null operand of `p_str_concat` is
      ""); test test_pe_str_concat_null pass on run, C and emit.
- [x] BG50. **`fid serve` keeps C codegen flags (`--leaks`, `-g`, profile)
      between requests.** compiler/main.fd:4974, 4998-5015; resets at 7194-7219
      and compiler.fd:3262 miss `g_c_*`. Confidence: high.
      Fixed Nov 22-23 (main.fd, codegen_c.fd: the `g_c_*` flags are reset per
      request); tests/tooling/serve-request-state-check.sh passes.
- [x] BG51. **Build stamp ignores the `--profile-use` profile,
      `ark_modules/.features` and `ark.toml`.** compiler/pgo.fd:368;
      compiler.fd:9226-9279, 10119. Confidence: high.
      Fixed Nov 22-23 (compiler.fd, pgo.fd: the stamp covers the profile,
      `ark_modules/.features` and `ark.toml`);
      tests/tooling/stamp-inputs-check.sh passes.
- [x] BG52. **emit/build report success when writing the output fails.**
      main.fd:8448-8449, 4358, 5544, 7314, 7757, 8279, 9389. Confidence: high.
      Fixed Nov 22-23 (main.fd: a failed write is an error with a non-zero
      exit); tests/tooling/emit-write-fail-check.sh passes.
- [x] BG53. **LSP turns the document URI into a path without decoding it**
      (`%3A`, `%20`): every sibling `use` is unresolved on Windows.
      compiler/lsp.fd:409-411, 2966-2968. Confidence: high.
      Fixed Nov 22-23 (lsp.fd: the URI is percent-decoded);
      tests/tooling/lsp-uri-check.sh passes.
- [x] BG54. **Paths are not quoted in C-compiler and child command lines**:
      a path with a space breaks the build. main.fd:5387, 5389, 5409, 5372,
      1162, 2387, 2512, 4444, 4547, 4486, 4574, 8478-8485. Confidence: high.
      Fixed Nov 22-23 (main.fd: paths quoted in the C-compiler and child
      command lines); tests/tooling/path-spaces-check.sh passes.
- [x] BG101. **`fid fmt` changes string literals inside f-string
      interpolations** (silent meaning change). compiler/fmt.fd:355-371
      (`fmt_tokenize` ends an f-string at the first `"`), 1587, 1638; the
      token-equality guard uses the same tokenizer, so it passes. Trigger:
      `println(f"{m.get("k")}");` becomes `f"{m.get(" k ")}"`. Confidence: high.
      Fixed Nov 22-23 (fmt.fd `fmt_tokenize` follows interpolations); fixture
      tests/fmt/literals_in.fd formats to literals.fd, which passes `--check`.

## Medium

- [x] BG55. **Checked add/sub/mul pass a value through the address scratch
      register across MIR instructions; a far spill reload overwrites it**
      (RISC-V, AArch64: missed or false overflow). nat_ir.fd:29505-29529,
      1139-1189. Confidence: medium.
      Fixed Nov 22-23 (nat_ir.fd: the checked operations keep their value out
      of the address scratch); test test_nat_checked_pressure passes on nat,
      run, C and emit.
- [x] BG56. **Function cache key ignores `is_constant_time` and
      `secret_values`.** nat_ir.fd:36998-37093 (`nc_fn_hash`). Confidence: medium.
      Fixed Nov 22-23 (nat_ir.fd `nc_fn_hash` hashes `is_constant_time` and
      `secret_values`). Checked by reading; no test of its own.
- [x] BG57. **Load-after-store forwarding goes stale when a float operand is
      moved through the forwarded scratch.** nat_ir.fd:20801-20856. Confidence:
      medium.
      Fixed Nov 22-23 (nat_ir.fd: forwarding drops a fact when a float operand
      moves through the scratch); test test_nat_float_ldfwd passes on nat,
      run, C and emit.
- [x] BG58. **A method's effects overwrite a same-named free function's.**
      typechecker.fd:4468, 4219. Trigger: `impl G { fn update(..) { println } }`
      plus `fn update(x)` called from `@deterministic` code is rejected.
      Confidence: high.
      Fixed Nov 22-23 (typechecker.fd: method effects kept apart from free
      functions'); test test_effects_method_same_name pass on run, C and emit.
- [x] BG59. **`@no-recursion`/`@bounded-stack` miss recursion through methods.**
      typechecker.fd:5450-5463, 6491-6512. Confidence: high.
      Fixed Nov 22-23 (typechecker.fd: recursion through methods found); test
      test_no_recursion_method_fail pass on run, C and emit.
- [x] BG60. **Comparisons are not type-checked** (beyond str vs number), and
      `1 < x < 10` parses as `(1 < x) < 10`, always true. typechecker.fd:20049-20077;
      parser.fd:6944. Confidence: high.
      Partly fixed Nov 22-23 (typechecker.fd `TypeChecker_check_comparison`:
      chained comparisons, ordering bools, a bool with a str, and two
      different structs or enums are errors); tests test_cmp_chain_fail,
      test_cmp_user_types_fail, test_cmp_ok pass on run, C and emit. Fixed
      Nov 24 (check_comparison: two numbers of different types — `i64 <
      f64`, `u8 == i64` — are an error, as is a literal that cannot fit the
      other side's int type; a literal that can adapt is still fine);
      tests test_cmp_numeric_fail, test_cmp_int_float_fail and
      test_cmp_ok pass on run, C and emit. The runtime layer and
      compiler/ modules are exempt.
- [x] BG61. **An integer literal of 2^64 or more wraps silently; one above
      i64::MAX fits an `i64`.** compiler/lexer.fd:1445-1455;
      typechecker.fd:14506. Confidence: high.
      Partly fixed Nov 22-23 (lexer.fd `lex_int_too_big`: a literal of 2^64 or
      more is an error); test test_int_literal_too_big_fail pass on run, C and
      emit. Fixed Nov 24 (typechecker.fd `TypeChecker_check_value_fits` runs
      where a declared type meets a literal and only a literal slipped
      through: call arguments, `return`, struct-literal fields, enum
      payloads and const initializers); tests test_literal_arg_fail,
      test_literal_return_fail, test_literal_field_fail,
      test_literal_const_fail and test_literal_enum_fail pass on run, C
      and emit.
- [x] BG62. **Comptime arithmetic treats `u64` as signed.**
      comptime_eval.fd:5111-5203. Confidence: medium.
      Fixed Nov 24 (comptime_eval.fd: a `val x: u64` inside comptime keeps
      an unsigned tag — `ct_untag_type`/`ct_int_untag` — so shifts,
      division and comparisons run on the bits unsigned and `~` masks to
      the declared width; `ct_check_int_fit` takes bits already inside an
      unsigned width); test test_comptime_u64 pass on run, C and emit.
- [x] BG63. **Inline `asm!` does not need `unsafe`.** typechecker.fd:21494-21518.
      Confidence: high.
      Fixed Nov 24 (typechecker.fd: `asm!` outside an `unsafe { }` block or
      an `@unsafe` function is E097; the in-tree asm tests, the bare-metal
      examples and scripts/pe-win/asm_process.fd carry `@unsafe` now;
      reference.md §4.4/§8.5 document it); test test_asm_unsafe_fail is
      rejected on run; the wrapped tests pass on run.
- [x] BG64. **lockcheck never sees `p_mutex_lock`, and its call-graph pass
      propagates almost nothing.** lockcheck.fd:75-77, 446-466. Confidence: high.
      Fixed Nov 22-23 (lockcheck.fd: `p_mutex_lock` is a lock, and the
      call-graph pass runs to a fixed point). Checked by reading: a probe
      locking (a, b) then (b, a) through `p_mutex_lock` is rejected by the
      type checker's E083 on the Nov 22 compiler too, so no test isolates the
      IR check.
- [x] BG65. **The `?` error path skips every drop when the Result owns
      nothing, and skips `@persist` saves.** lower.fd:16152-16176. Confidence: high.
      Fixed Nov 22-23 (lower.fd: the `?` error path runs the drops and
      `@persist` saves); test test_try_error_drops pass on run, C and emit.
- [x] BG66. **Interpreter: f32 values are never rounded to single precision.**
      interp.fd:2577-2587, 4818-4830, 4519-4535. Confidence: high.
      Fixed Nov 22-23 (interp.fd rounds f32 values to single precision); test
      test_f32_single_rounding pass on run, C and emit.
- [x] BG67. **Interpreter: u64 → f64 is converted as signed.** interp.fd:3814-3825.
      Confidence: high.
      Fixed Nov 22-23: interp.fd converts u64 as unsigned, and so does
      pe_ir.fd (`peir_u64_to_float` for an IntToFloat from U64; it converted
      as signed); test test_u64_to_f64_cast pass on run, C and emit.
- [x] BG68. **`p_dir_list` can overflow its buffer if the directory changes
      between its two passes.** codegen_c.fd:13336-13384. Confidence: high.
      Fixed Nov 22-23 (codegen_c.fd `p_dir_list` bounds its second pass); test
      test_file_write_nul pass on C (a ccompile-only test).
- [x] BG69. **`p_process_run` is not thread-safe and truncates long commands.**
      codegen_c.fd:12781-12811. Confidence: high.
      Fixed Nov 22-23 (codegen_c.fd `p_process_run`: thread-safe, no command
      length limit); test test_process_run_threads pass on C (a ccompile-only test).
- [x] BG70. **`fid serve` shares the in-memory IR cache across targets, @cfg and
      borrow modes.** compiler.fd:5628-5645, 13957; inc.fd:680-690. Confidence:
      medium.
      Fixed Nov 22-23 (compiler.fd: the in-memory IR cache key covers target,
      @cfg and borrow mode); tests/tooling/serve-request-state-check.sh
      passes.
- [x] BG71. **`ccompile --inc` builds and links for the host, not `--target`.**
      main.fd:4401-4404, 4486-4498, 4520-4523, 4574-4587. Confidence: medium.
      Fixed Nov 22-23 (main.fd: `--inc` builds and links for `--target`);
      tests/tooling/inc-target-check.sh passes.
- [x] BG72. **C header parser drops casts in `#define` constants**
      (`((uint32_t)-1)` → -1). compiler/cparser.fd:2169-2175. Confidence: high.
      Fixed Nov 22-23 (cparser.fd: casts in `#define` constants kept); test
      test_cparser_casts pass on run, C and emit. Its emit run needed
      cparser.fd `cp_load_file` to read through `read_file` (the raw string
      had no header), which also fixed test_cparser_include on emit.
- [x] BG73. **Awaiting an eager future that holds a negative value joins an
      unrelated thread.** codegen_c.fd:13524-13535; lower.fd:18357-18359,
      15252-15254. Confidence: high.
      Fixed Nov 22-23 (codegen_c.fd, lower.fd: a thread future is tagged, not
      told apart by sign); test test_future_negative_value pass on C (a ccompile-only test).
- [x] BG74. **Interpreter channels ignore their capacity; a send never waits.**
      interp.fd:11589-11641. Confidence: high.
      Fixed Nov 22-23 (interp.fd: a send waits on a full channel); test
      test_chan_send_waits_full pass on run and C (not run on emit).
- [x] BG75. **Closures and `dyn Trait` values count as `@send`.**
      typechecker.fd:18348, 18196. Confidence: medium.
      Partly fixed Nov 22-23 (typechecker.fd: a `dyn Trait` is not @send, and
      a closure literal given to a spawned call has its captures checked);
      test test_spawn_closure_capture_fail pass on run, C and emit. Fixed
      Nov 24 (typechecker.fd `is_send_type`: a `fn` type is not @send — the
      value may be a closure whose captured state two threads would
      share); test test_spawn_fn_var_fail pass on run, C and emit.
- [x] BG76. **On one worker, `task_run()` from inside a task runs a nested
      scheduler pass over the same list.** codegen_c.fd:15297-15306,
      14216-14238. Confidence: medium.
      Fixed Nov 22-23 (codegen_c.fd: `task_run()` inside a task starts no
      nested pass); test test_task_run_in_task pass on C (a ccompile-only test).
- [x] BG77. **@secret: an early `return`/`break` under a secret branch leaks
      through control flow.** secrecy.fd:752-764, 783, 733. Confidence: high.
      Fixed Nov 22-23 (secrecy.fd: an early return or break under a secret
      branch is a leak); test test_secret_early_return_fail pass on run, C and
      emit.
- [x] BG78. **`select` stores the received value into an outer variable of
      the same name.** lower.fd:9786-9797 (`Lowerer_lower_select_stmt`).
      Confidence: high.
      Fixed Nov 22-23 (lower.fd `Lowerer_lower_select_stmt` binds a new
      variable); test test_select_var_shadow pass on run, C and emit.
- [x] BG79. **Borrow check does not descend into tuples, `is` or comptime
      blocks; `LetTuple` records no borrow.** borrowck.fd:3203-3206.
      Confidence: high.
      Fixed Nov 22-23 (borrowck.fd descends into tuples, `is` and comptime
      blocks; `LetTuple` records its borrow); test test_borrow_tuple_fail pass
      on run, C and emit.
- [x] BG80. **A raw pointer converts to a float or narrow integer, and a float
      to a pointer.** typechecker.fd:14892-14909. Confidence: high.
      Fixed Nov 22-23 (typechecker.fd: no raw pointer to a float or a narrow
      integer, no float to a pointer); tests test_rawptr_float_fail,
      test_rawptr_narrow_fail pass on run, C and emit.
- [x] BG81. **`p_file_write`/`p_file_append` stop at the first NUL;
      `p_file_write` hides write errors.** codegen_c.fd:12657-12680.
      Confidence: high.
      Fixed Nov 22-23 (codegen_c.fd: `p_file_write`/`p_file_append` write the
      whole length and report errors); test test_file_write_nul pass on C (a ccompile-only test).
- [x] BG82. **A by-value struct parameter reassigned in the body leaks the
      new value.** lower.fd:6650-6702, 11136-11153. Confidence: medium.
      Fixed Nov 22-23 (lower.fd: the reassigned parameter's new value is
      dropped); test test_struct_param_reassign pass on run, C and emit.
- [x] BG83. **`peir_guard_ops` reads `args.get(at + 1)` before testing
      `at >= 0`** (compiler panic on a zero-argument call in a guarded
      block). checks.fd:2089. Confidence: medium.
      Fixed Nov 22-23 (checks.fd `peir_guard_ops` tests `at` first); test
      test_guard_zero_arg_call pass on run, C and emit.
- [x] BG102. **`fid fmt` breaks named lifetimes: `&'a i64` becomes `&' a i64`**
      (then an unterminated char literal). fmt.fd:336-352, 559-561, 1633.
      Trigger: `fid fmt tests/bootstrap/test_frontend_lifetime.fd`. Confidence: high.
      Fixed Nov 22-23 (fmt.fd: a lifetime is one token); fixture
      tests/fmt/literals passes.
- [x] BG103. **`fid fmt` splits `'\xHH'` and `'\u{...}'` char literals.**
      fmt.fd:336-352 (expects the closing quote at i + 3). Confidence: high.
      Fixed Nov 22-23 (fmt.fd: `'\xHH'` and `'\u{...}'` read to the closing
      quote); fixture tests/fmt/literals passes.
- [x] BG104. **scribe never documents multi-line impl methods.**
      compiler/scribe.fd:414-427 (`parse_impl` counts the line's braces before
      testing depth 1). Confidence: high.
      Fixed Nov 22-23 (scribe.fd `parse_impl` tests the depth before counting
      the line); test test_scribe_braces pass on run, C and emit.
- [x] BG105. **scribe counts braces inside strings, char literals and
      comments**, so items are swallowed or cut short. scribe.fd:236-257 and
      the struct/enum/impl/trait parsers. Trigger: a body with `return "{";`.
      Confidence: high.
      Fixed Nov 22-23 (scribe.fd skips strings, char literals and comments
      when counting braces); test test_scribe_braces pass on run, C and emit.
- [x] BG106. **veil leaves the methods of generic structs out of coverage.**
      compiler/veil.fd:307-341 (`Stack__push__i64` does not match `Stack_push`).
      Confidence: high.
      Fixed Nov 22-23 (veil.fd: instances matched to their template, and a
      generic template no longer counted as an uncalled function);
      tests/tooling/veil-generic-check.sh passes (2/2 functions).

## Low

- [x] BG84. **Comptime unroller drops all iterations of a range ending at
      i64::MAX.** comptime_eval.fd:3037-3043. Confidence: high.
      Fixed Nov 22-23 (comptime_eval.fd: a range ending at i64::MAX unrolled
      without overflow); test test_comptime_unroll_i64_max pass on run, C and
      emit.
- [x] BG85. **mono name mangling can give two instantiations one name**
      (array size dropped; tuples not delimited). compiler/mono.fd:452-466.
      Confidence: medium.
      Fixed Nov 24 (mono.fd `mangle_type`: `[T; N]` keeps its N, and the
      tuple and ArrayConst parts are joined with their count — `tup2_a_b` —
      so `i64_i64` and `i64i64` can no longer collide; single-arg
      `pick__i64`/`Vec__i64` shapes unchanged). The `sz<T>` probe then
      surfaced a separate generic-array return-code bug (a single `[i64;3]`
      instantiation comes back zeroed on run): filed as BG126.
- [x] BG86. **Function-cache instruction hash runs list fields together**
      (Asm outs/ins, Generic args). nat_ir.fd:36919-36932, 36640-36648.
      Confidence: high (collision), low (in practice).
      Fixed Nov 22-23 (nat_ir.fd: list fields hashed with their counts).
      Checked by reading; no test of its own.
- [x] BG87. **Linker `R_ARM64_ADD_LO12` patch mask wipes Rd and Rn.**
      compiler/linker.fd:937 (`0xFFC00003`, should be `0xFFC003FF`). No current
      producer. Confidence: high.
      Fixed Nov 22-23 (linker.fd mask 0xFFC003FF). Checked by reading; there
      is still no producer.
- [x] BG88. **`PeirEnc_cvttsd2si_r64` extends the GPR with REX.B instead of
      REX.R.** compiler/pe_ir_enc.fd:1934-1940. Latent. Confidence: high.
      Fixed Nov 22-23 (pe_ir_enc.fd REX.R); test test_pe_enc_sse_bytes passes
      on run, C, emit and nat.
- [x] BG89. **pe_ir SSE encoders never emit REX for xmm8-15** (incl. the
      callee-saved save/restore). pe_ir_enc.fd:689, 698, 706, 1792-1972.
      Latent. Confidence: high.
      Fixed Nov 22-23 (pe_ir_enc.fd: REX for xmm8-15); test
      test_pe_enc_sse_bytes passes on run, C, emit and nat.
- [x] BG90. **`PeirEnc_vex2`/`vaddpd_rr`/`vmulpd_rr` build a wrong VEX
      prefix.** pe_ir_enc.fd:530-563. Dead code. Confidence: high.
      Fixed Nov 22-23 (pe_ir_enc.fd VEX prefix); test test_pe_enc_sse_bytes
      passes on run, C, emit and nat.
- [x] BG91. **Byte-register encoders skip the REX needed for spl/bpl/sil/dil.**
      pe_ir_enc.fd:1317, 1272, 1379. Latent. Confidence: high.
      Fixed Nov 22-23 (pe_ir_enc.fd: REX for spl/bpl/sil/dil); test
      test_pe_enc_sse_bytes passes on run, C, emit and nat.
- [x] BG92. **`PeirEnc_mov_mem_reg8` turns an rbp/r13 base into a RIP-relative
      store.** pe_ir_enc.fd:1430-1444. Latent. Confidence: high.
      Fixed Nov 22-23 (pe_ir_enc.fd: an rbp/r13 base takes a displacement);
      test test_pe_enc_sse_bytes passes on run, C, emit and nat.
- [x] BG93. **pe_ir `p_str_join_n` returns an uncounted block and uses
      strlen.** pe_ir_stubs.fd:8702-8794. Confidence: medium.
      Fixed Nov 22-23 (pe_ir_stubs.fd `p_str_join_n` returns a counted string,
      no strlen); test test_pe_str_join_n pass on run, C and emit.
- [x] BG94. **Printing a Vec or HashMap leaks the formatted string** (C).
      codegen_c.fd:8141-8154, 7506, 7517, 7566. Confidence: high.
      Fixed Nov 22-23 (codegen_c.fd: the formatted text is dropped after
      printing); test test_print_collection_drop pass on run, C and emit, and
      scripts/leak-check.sh on it reports every block freed (the Nov 22
      compiler leaked three blocks).
- [x] BG95. **C header parser maps `wchar_t` and `long double` to the wrong
      widths on Linux/macOS.** cparser.fd:1668, 1600-1601. Confidence: high.
      Fixed Nov 22-23 (cparser.fd: `wchar_t` and `long double` per target);
      test test_cparser_casts pass on run, C and emit. `long` on LP64 is
      BG121.
- [x] BG96. **Interpreter: two tasks awaiting each other hang where compiled
      code panics.** interp.fd:11034-11049. Confidence: medium.
      Fixed Nov 22-23 (interp.fd: a cycle of awaiting tasks panics); test
      test_task_await_cycle_fail pass on run and C (not run on emit).
- [x] BG97. **`p_json_parse`/`p_json_dump` turn nested objects into invalid
      JSON.** codegen_c.fd:17700-17706. Confidence: high.
      Fixed Nov 22-23 (codegen_c.fd: nested objects kept valid); test
      test_cback_json_nested pass on C (a ccompile-only test).
- [x] BG98. **@secret: `panic`, `assert` and `exit` are not sinks.**
      secrecy.fd:131. Confidence: high.
      Fixed Nov 22-23 (secrecy.fd: `panic`, `assert` and `exit` are sinks);
      test test_secret_assert_sink_fail pass on run, C and emit.
- [x] BG99. **`parallel { | x = f(..) }` with a `str` x leaks x's old value.**
      lower.fd:~11713-11805 (`lw_par_word`). Confidence: medium.
      Fixed Nov 22-23 (lower.fd `lw_par_word` drops the old value); test
      test_parallel_str_result pass on run, C and emit.

- [x] BG107. **`fid fmt` joins a block comment to the token before it**
      (`a / /* c */ b` becomes `a //* c */ b`); the token guard refuses the
      file, so nothing is corrupted. fmt.fd:1805-1826. Confidence: high.
      Fixed Nov 22-23 (fmt.fd keeps a space before a block comment); fixture
      tests/fmt/literals passes.

## Found while fixing the criticals (Nov 22)

- [x] BG108. **Critical. The mid-end SSA round trip miscompiles a loop that
      rotates its variables** (pe_ir and nat; C and `run` are right).
      compiler/ssa.fd `ssa_round_trip`: the phis it builds are right
      (FID_SSA_DUMP), but the function after it is not: translation
      validation (`fid emit --validate`) rejects the `ssa` pass on it, and
      FID_IROPT_OFF=ssa gives the right output. Trigger: `var a = 1; var b = 2; var c = 3; var d = 4;
      while t < 3 { val tmp = a + (b ^ c) + d; d = c; c = b + 100; b = a;
      a = tmp; println(a, b, c, d); t = t + 1; }`: the second line is `112 6
      101 102` under `run` and C, `204 6 106 102` on emit (`c = b + 100`
      reads the new b). It is why SHA-1/SHA-256
      based tests fail on emit and nat (test_std_webrtc, test_std_dtls,
      test_std_srtp, test_std_ecdsa, test_std_jose*, test_std_hpke,
      test_std_ssh, test_std_x509, test_stdlib_secret_keys, ...): the
      compression loop rotates a..e. The Nov 18 fid and release/fid.exe do
      the same, so it predates this work. Confidence: high (reproduced).
      Fixed Nov 24 (ssa.fd `ssa_materialize` copies a phi's reads through
      temporaries before the writes — a swap between two phis no longer
      reads the new value): the trigger program prints `112 6 101 102` on
      emit, `fid emit --validate` reports 0 rejections on it and on
      test_hmac_sha512, and test_hmac_sha512 passes on emit.
- [x] BG109. **pe_ir: `println` of an f32 and an f32 stored into a Vec word
      lost the value** (`movq rcx, xmm0` / `movq rax, xmm0` encoded as
      `66 48 0F D6`, which is `movq xmm, xmm`). compiler/pe_ir.fd
      `peir_pun_f32_i64` and the Print arm. Fixed Nov 22 (0F 7E): `println`
      of an f32 checked on emit. Also: pe_ir's inlined Vec_get/Vec_push/
      Vec_set moved the raw word with no f32 conversion; an f32 element now
      takes the call path (written Nov 22, not yet built). Trigger:
      test_vec_f32_index.fd on emit.
      Built and checked Nov 23: test_vec_f32_index passes on run, C, emit and
      nat.
- [x] BG110. **The native runtimes still cap threads at 256** (the C
      runtime's table is chunked since BG9; pe_runtime/linux_runtime keep a
      fixed table). Confidence: medium (from the BG9 fix's notes).
      Partly fixed Nov 22-23 (stdlib/pe_runtime/conc.fd and
      linux_runtime/conc.fd: the thread table is chunked as the C runtime's);
      tests test_native_spawn_many, test_spawn_many pass on run, C, emit and
      nat. Fixed Nov 24 (both `pe_join`s claim the join by compare-and-swap
      on the slot's state word — a second caller waits for the result the
      first stores instead of joining the thread again).
- [x] BG111. **Field assignment on a generic struct instance was rejected
      by the type checker** (`h.v = s` with `h: H<str>`: "cannot assign
      field 'v' on the generic type parameter H<str>", CE9's message for a
      `Type::Generic` object). typechecker.fd `Stmt::FieldAssign` now checks
      it against the declared field type with the instance's arguments
      (BG6's test needs it). Fixed Nov 22; test_generic_field_assign_owner
      passes on run, C and emit.
- [x] BG112. **The read side of `x[i]` accepts any non-Vec named or generic
      type** (HashMap, a struct, a type parameter) and gives `never` with no
      error; the write side (BG1) rejects them now. typechecker.fd
      `Expr::Index`.
      Fixed Nov 22-23 (typechecker.fd: `x[i]` on a HashMap, struct, enum or
      type parameter is an error); test test_index_read_map_fail pass on run,
      C and emit.
- [x] BG113. **Core's raw-memory writers without the `p_` prefix are
      callable in safe code** (`vec_st`, `Str_drop`, `Box_drop_t`,
      `Vec_init_typed`): BG10 gated only the `p_` ones.
      Fixed Nov 22-23 (typechecker.fd: core's raw-memory writers without the
      `p_` prefix need `unsafe` too); test test_core_str_drop_fail pass on
      run, C and emit.
- [x] BG114. **scripts/verify/expect.py keeps the indentation of an
      expected-output block** (`//   sizes ...` expects two spaces), so the
      allocator tests (test_alloc_reuse_zero, test_mem_return, ...) fail
      under it while scripts/tier1.sh passes them.
      Fixed Nov 22-23 (expect.py strips the common indentation of the block);
      test_alloc_reuse_zero, test_mem_return and test_mem_return_reuse pass
      under it.

## Found while fixing the rest (Nov 23)

- [x] BG115. **High. A bare `Some(v)` / `None` pattern is accepted and binds
      `v` as i64.** reference.md §(enums) says a bare `Some(5)` is an
      undefined function, but as a match pattern `Some(v)` matches the
      Option's tag and `v` gets type i64 whatever the payload is: with
      `get() -> Option<str>`, `Some(v) => println(int_to_str(v))` prints
      `some 0`, and `v == "x"` is rejected as "i64 with str". Either the
      pattern resolves to `Option::Some` with the payload's type, or it is
      an error. Seen in three new tests (test_cparser_casts,
      test_read_line_long: rewritten to `Option::Some`). Confidence: high.
      Fixed Nov 24 (typechecker.fd `pattern_binding_types` resolves the
      enum through `resolve_pattern_enum`, so a bare `Some(v)` on an
      `Option<str>` scrutinee finds Option, and the generic-arg lookup uses
      the resolved name, not the pattern's empty one): `v` binds `str`,
      `v == "x"` typechecks, `int_to_str(v)` is rejected.
- [x] BG116. **`s.len()` on a `str` is accepted by `fid run` and rejected
      by `ccompile` and `emit`** ("cannot call method 'len' on type str").
      One program, two answers from the checker depending on the path;
      `str_len(s)` works on all. Trigger: test_pe_str_join_n before its
      Nov 23 rewrite. Confidence: high.
      Already fixed when re-checked Nov 24: `s.len()` on a `str` is rejected
      on run, ccompile and emit alike ("cannot call method 'len' on type
      str"); `str_len` works on all. No divergence left.
- [x] BG117. **BG61's type-checker half**: the lexer rejects a literal over
      64 bits now (test_int_literal_too_big_fail), but a literal that fits
      in 64 bits and not in its declared type (`val b: u8 = 300;`) is not
      checked yet.
      Fixed Nov 24 with BG61 (see its note): `check_value_fits` runs at
      every position a declared type takes a literal.
- [x] BG118. **The lock-order check runs only in `fid check`**, not in
      `run`/`ccompile`/`emit`, so test_lockcheck_cycle_fail can never
      fail to compile on those paths. compiler/lockcheck.fd.
      Fixed Nov 24 (main.fd `cmd_build` runs `c_lockcheck_errors` after
      lowering on every build path); test_lockcheck_cycle_fail fails to
      compile on run, C and emit.
- [x] BG119. **BG45's lowering half**: `Lowerer_lower_spawn_detached`
      detaches green tasks only, so `spawn detached f()` on a thread inside
      a scope is still joined at the scope's end. The C runtime's
      `p_co_detach` accepts a thread future now; pe_ir's and nat_ir's do
      not, so lower.fd cannot call it for threads yet.
      Fixed Nov 24 with BG45 (see its note): `pert_co_detach` takes a
      thread future in both native runtimes, and lower.fd emits the call
      for any spawned call.
- [x] BG120. **BG40 leftovers**: the C runtime's `p_vec_remove`,
      `reverse`, `sort`, `pop` and `truncate` still ignore element codes and
      shared buffers (push/set/insert were fixed).
      Fixed Nov 24 with BG40 (see its note): all five delegate to core's
      Vec functions; `vec_raw_reverse`'s struct swap is fixed too, and the
      interpreter's `p_vec_reverse`/`insert`/`remove`/`contains`/`first`/
      `last`/`extend`/`to_str`/`sort` had the same raw-word bug —
      `p_vec_reverse` on a `Vec<u8>` wrote 8-byte words into a byte buffer
      and corrupted the heap, and the mutating `p_vec_*` calls skipped
      `vec_own` so `var b = a; p_vec_reverse(b)` reversed `a`'s buffer too
      (interp.fd: all go through `interp_vec_ld`/`st`/`esz` and
      `interp_vec_own`, the share-box check core's `vec_own` does; a struct
      element is copied out before a remove's shift).
- [x] BG121. **cparser maps `long` / `unsigned long` to i32/u32 on every
      target**: wrong on 64-bit Linux and macOS (LP64). The C++ path
      (`cpx_base_of`) maps `long double` to double: values cross the shim
      right, a `long double*` does not.
      Fixed Nov 24 (cparser.fd `cp_resolve`: `long`/`unsigned long` are
      i32/u32 only when `cp_on_windows()`, i64/u64 under LP64;
      `cpx_base_of` maps `long double` to `ldouble` off-Windows like the C
      path, and a `ldouble` pointer/ref is rejected while a by-value one
      stays `double`); `fid parse-header` on a `long`/`long double*`
      header emits i32 with the LLP64 note on Windows.
- [x] BG122. **BG63 / BG62 need language work**: BG63 (asm outside
      `unsafe`) needs about 12 asm tests wrapped in `unsafe` and the docs
      changed with it; BG62 needs typed comptime values.
      Fixed Nov 24 with BG62 and BG63 (see their entries): the asm tests
      carry `@unsafe` and the reference documents it; comptime values
      carry a type tag (`ct_untag_type`).
- [x] BG123. **Small leftovers from the Nov 22 batch**: lsp.fd keeps
      `var program`, which would let the BG20 exemption for the compiler
      go; bindings in an `inline for` body leak into the enclosing scope.
      (BG35's merged AST cache, BG46's await-in-a-loop, BG60/BG75's
      leftover checks, BG110's pe_ir double-join and `fid fmt`'s
      `impl < T >` were done Nov 24 — see their entries.)
      Fixed Nov 24: lsp.fd's six `var program: Program;` sites initialize
      with `program_new()` and the `compiler` module no longer keeps the
      BG20 unassigned-var exemption (typechecker.fd); an `inline for`'s
      per-iteration body is wrapped in `if true { ... }` so its `val`s
      stay scoped to the iteration (substitute.fd) — `for i` bindings and
      body temporaries no longer leak, and assignments to outer `var`s
      still work; `fid fmt`'s `impl < T >` is under BG124.
- [x] BG124. **`fid fmt --check` reports a CRLF file as unformatted.** On a
      Windows checkout (`core.autocrlf=true`) scripts/fmt-check.sh fails
      373 of 468 files that are formatted; the formatter writes LF, and the
      check compares bytes. Also: `fid fmt <file>` rewrites the file in
      place with no `--check`, which a probe redirecting its output
      (`fid fmt f.fd > out`) does not expect.
      Fixed Nov 24 (main.fd `cmd_fmt`: the comparison strips `\r`, so a
      CRLF checkout is not a formatting difference; plain `fid fmt` prints
      to stdout and `-w`/`--write` rewrites the file; fmt.fd
      `generic_closes` takes `impl`'s type-parameter list as always a
      generic — no more `impl < T >`; fmt-check.sh uses `-w` for the
      in-place checks and stdout for the fixture diffs); a formatted CRLF
      file passes `fmt --check` and an unformatted one fails it.
- [x] BG125. **An `if` or `match` used as a value took its type from one
      branch.** lower.fd `Lowerer_infer_match_type` used the first
      expression statement of any arm, so `PR::Ok(u) => { change(&mut u);
      u.funcs.len() }` typed the match as the void call and the value was
      lost (0: test_match_value_binding_changed), and `if c { small_u8 }
      else { big_i64 }` stored the i64 into a byte (300 gave 44:
      test_if_match_value_ok). Fixed Nov 23: the arms' tail values are
      joined (`lw_join_value_ty`: the wider integer, a bare literal takes
      the other branch's type) and each branch value is extended to the
      slot's type (`Lowerer_branch_value_to`). Both are older than the
      Nov 22 batch (the Nov 22 compiler printed 44 too).

- [x] BG126. **A function returning a fixed-size array returned a pointer
      into its own dead stack frame.** Not generic-specific: `fn idc(a:
      [i64;3]) -> [i64;3] { return a; }` printed `0 20 30` on `run` (the
      interpreter frees and reuses frame allocas), garbage on `emit`, and
      crashed `ccompile` ("Invalid memory access at 0x8") — codegen_c's
      `is_aggregate_ret` already treated `IrType::Array` as an
      out-parameter return, so the caller pushed an sret argument the
      callee never expected. Fixed Nov 24: arrays joined the existing
      sret convention (lower.fd — `is_struct_ret` accepts `IrType::Array`,
      `emit_sret_copy` emits one `StructCopy` of elem*count bytes, and
      every call site — named, method, assoc, CallPtr, async, spawn
      adapter, pipe-forward and `await` — allocates or heap-blocks the
      buffer); interp CallPtr, nat_ir `ng_is_struct_ty`/`ng_callptr_sret`
      and codegen_c `StructCopy` sizing learned the same. The same test
      passes on run, C and emit (`idc([10,20,30])` → `10 20 30`,
      `mkc()` → `7 8 9`; generic `id<T>` over `[T;3]`/`[T;4]` too);
      struct and tuple sret unchanged.

- [x] BG127. **`fid run` corrupted its simulated heap instead of
      panicking at the interpreter's memory limit.** `Memory_alloc_in`
      returns 0 when the 127 MB `INTERP_MEM_LIMIT` is used up, but of the
      ~30 `Memory_alloc` callers only `p_malloc` and the alloca path
      check it: `Memory_store_cstr` wrote the string header at block 0
      and reported the string's bytes at address 16 (and vec, map and
      struct allocs did the same elsewhere). test_alloc_thread_heaps hits
      it — ~32 MB of interned strings a call (cstr blocks are never
      reused), so the heap was gone by call four, the vec's elements were
      the word 16, and `Str_drop`/`Str_dup` dereferenced `16 - 16 = 0`:
      "Invalid memory access (null or dangling pointer)". ccompile and
      emit pass the test; the failure is run-only and older than the
      Nov 23 fid. Fixed Nov 24: `Memory` gains `oom_hit`, set by
      `Memory_alloc_in`'s two exhausted-heap returns and checked after
      every instruction in `Interpreter_run_function` (as `lim_hit_want`
      is), so the program stops with "Out of memory: the interpreter
      holds at most 133169152 bytes" — the panic p_malloc's OOM already
      raised — instead of running on a fabricated address;
      `Memory_store_cstr` returns 0 on a failed block instead of writing
      a header at address 0.

## Found by the Nov 24 whole-repo review

Found on Nov 24 by fifteen review passes over the compiler, the three
runtimes and the standard library (two rounds; each pass checked its
findings against this file, known-bugs.md §2g and docs/file-analysis/),
with the self-hosted compiler built that day (release/fid-dev.exe). Unlike
the Nov 22 review, nearly every entry was confirmed by running a program:
"Confirmed Nov 24" names how; "Read only" marks the few that were not.
Probe programs are not in the repo. Line numbers move: the function
names are the reference.

### Critical

- [x] BG128. **The payload of a generic enum constructor is never
      type-checked.** typechecker.fd `TypeChecker_check_expr` `Expr::EnumLit`
      returns `Named(enum)` without its type arguments, and
      `TypeChecker_check_enum_constructor` skips a payload that mentions a
      type parameter ("checked after monomorphization"; nothing does). So
      `Option::Some(x)`, `Result::Ok/Err(x)` and a user `W::V(x)` accept any
      payload. Trigger: `fn h() -> Result<i64, str> { return Result::Ok("no");
      }`, then `v + 1` on the Ok value: `check` passes, `run` prints 4113, C
      prints an address; `val o: Option<str> = Option::Some(5)` panics "a str
      without a string header". Also through a variable, a user `enum W<T>`,
      an argument and `Vec<Option<str>>.push`. Confirmed Nov 24 (run, C).
      Confidence: high.
      Fixed Nov 24 (typechecker.fd `TypeChecker_check_enum_constructor`: the
      payload is checked against the enum's type arguments, so
      `Result::Ok("no")` for a `Result<i64, str>` is "Result::Ok field 1
      expects 'i64', got 'str'"); test_bg128_enum_payload and the six
      test_bg128_*_fail (return, let, var, argument, push, user enum) pass on
      run, C and emit (Nov 25).
- [x] BG129. **A match pattern may name a different enum than the scrutinee.**
      `TypeChecker_check_enum_pattern` / `TypeChecker_resolve_pattern_enum`
      use the enum the pattern names and never compare it with the scrutinee's
      type: `B::P(s)` against an `A` value matches on the tag number and binds
      `s` with B's payload type. Trigger: `enum A { X(i64), Y } enum B {
      P(str), Q }`, `match A::X(5) { B::P(s) => { println(s); } _ => {} }`:
      accepted; `run` prints an empty line, C panics "Invalid memory access at
      0x5". A literal pattern of the wrong type (`match n_i64 { "a" => ..}`)
      is accepted too and never matches. Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (typechecker.fd `TypeChecker_check_enum_pattern`: a variant
      of another enum than the scrutinee's, and a literal pattern of the wrong
      type, are errors); test_bg129_match_patterns and
      test_bg129_match_{other_enum,literal_type,bool_int}_fail pass on run, C
      and emit (Nov 25).
- [x] BG130. **`@crashfree`, `@no-panic`, `@no-alloc`, `@no-recursion`,
      `@conserve` and `@gas` do not look inside f-strings or enum literals.**
      typechecker.fd `scan_expr_for_calls` (used by `check_safety_body`,
      `verify_safety_annotations`, `gas_total`, `tc_conserve_calls`) has no
      arm for `FString`, `EnumLit`, `NamedArg`, `Await`, `Spawn`, `Yield` or
      `IsCheck`; `scan_expr_for_division` and the panic-risk scan miss
      `EnumLit` and `FString`. Triggers, all accepted: `@crashfree fn cf(n:
      i64) -> str { return f"v={boom(n)}"; }` (panics), `return Option::Some(a
      / b);` (division by zero), `f"{v[b]}"` (out of bounds), `@no-recursion`
      recursing inside an f-string, `@no-alloc` returning `f"value {n}"`.
      Confirmed Nov 24 (run). Confidence: high.
      Fixed Nov 24 (typechecker.fd `scan_expr_for_calls`,
      `scan_expr_for_division` and the panic-risk scan: arms for f-strings,
      enum literals, named arguments and `is`); test_bg130_safety_scans and
      the seven test_bg130_*_fail pass on run, C and emit (Nov 25).
- [x] BG131. **A `Vec` of `Option<T>` or `Result<T, E>` stores the address of
      a reused stack slot.** lower.fd, the Vec-method argument path before
      `Lowerer_vec_mem_size`: `Lowerer_generic_inst_struct_ir` gives a layout
      only for the program's generic structs and returns `I64` for a built-in
      generic enum, so the push lowers to `Vec_push` of the enum literal's
      alloca address instead of `Vec_push_mem` (C: `Vec_push(env, vec,
      (int64_t)v10)` with `v10 = alloca(16)`). Every push stores the same
      slot, and the drop frees words never allocated. Trigger: `var w:
      Vec<Option<i64>>`, push `Option::Some(i * 10)` for i in 0..3, sum the
      Some values: want 30; `run` prints 60 then "Panic: Double free", C 60
      then "Invalid free", emit 30 then "Invalid free". `Vec<Option<str>>` and
      `Vec<Result<..>>` the same; release/fid.exe of Nov 23 too; no test uses
      these types. Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (lower.fd `Lowerer_generic_inst_struct_ir`: a built-in
      generic enum gets its layout, so the push is `Vec_push_mem` of the
      value); test_bg131_vec_option_push passes on run, C and emit (Nov 25).
- [x] BG132. **Range analysis keeps a value fact across a join reached by a
      forward edge laid out later (an early `break`), so an overflow check is
      dropped.** checks.fd `peir_join_set` treats every predecessor not yet
      scanned as a loop back edge and keeps a value fact when
      `peir_join_stored` finds no redefinition walking back; a parameter has
      no definition in any block, so the fact always survives, though the
      `break` path never carries it. Trigger: `fn f(x: i64, y: i64) -> i64 {
      var k: i64 = 0; while x < 10 { if k == y { break; } k = k + 1; } return
      x - 10; }` with `f(i64::MIN, 0)`: `run` panics on the subtraction, the C
      build prints 9223372036854775798 (`fid explain`: "removed sub overflow
      check (range analysis)"). Native emit kept the check in this case. Fix
      direction: drop the fact when that predecessor is not dominated by the
      join. Confirmed Nov 24 (dump-c + zig cc). Confidence: high.
      Fixed Nov 24 (checks.fd `peir_join_set`: only a predecessor the join
      dominates counts as a back edge, so the `break` edge empties the fact);
      test_range_break_join_fail passes on run, C and emit (Nov 25).
- [x] BG133. **`spawn` of a function with an `f64` parameter or result gives
      wrong values on C.** codegen_c.fd: `p_thread_spawn(name, n, v1, v2)`
      passes a `double` into the variadic arguments (`fn_param_types` of
      `p_thread_spawn` covers only its first two parameters, so `cast_arg`
      never bit-casts it); the runtime reads every argument with `va_arg(ap,
      int64_t)`, calls the target through `int64_t (*)(void*, int64_t, ...)`
      and takes the result from rax where an f64 function returns in xmm0.
      Trigger: `fn work(x: f64, n: i64) -> i64 { return (x * 2.0) as i64 + n;
      }`, `await spawn work(2.5, 1)` prints 1 (want 6); `await spawn
      workf(1.5)` returning `x * 3.0` prints 0.000…0695 (want 4.5). Confirmed
      Nov 24. Confidence: high.
      Fixed Nov 24 (codegen_c.fd `cg_spawn_word_arg`, `cg_thunk_native`: a
      float argument goes as its bits by the parameter's type, and such a
      function is started through a thunk that converts the words and the
      result; bool/i32 results too); test_spawn_float_args passes on run, C
      and emit (Nov 25).
- [x] BG134. **pe_ir: an f32 operation with an f64 constant operand panics the
      compiler, or (allocator off) computes 0.** pe_ir.fd
      `PeirGen_gen_float_binop`, `is_f32` path: lowering emits `BinOp(..,
      F32)` with an f64 `ConstFloat` operand (`z * 4.0`, `acc + 0.5`, `w <
      0.5`); pe_ir loads both with `movss` and never converts the f64 one.
      With the constant in an xmm register `PeirEnc_emit_mem_rbp` panics
      "pe_ir: a register value used as memory"; with `FID_PE_OFF=ra` the low
      32 bits of the double (0.0) are used. Trigger: `var z: f32 = 1.25; val
      r: f32 = z * 4.0; println(r as f64);` (run 5).
      tests/bootstrap/test_isel_f32_mixed.fd fails to emit. nat_ir has
      `ng_float_mixed`. Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (pe_ir.fd `PeirGen_gen_float_binop`: an f32 operation with
      an f64 operand is computed in f64 and narrowed, as C);
      test_pe_f32_mixed_ops and test_isel_f32_mixed pass on run, C and emit
      and --nat (Nov 25).
- [x] BG135. **pe_ir: a struct result from a call through a function value is
      garbage.** pe_ir.fd `PeirGen_gen_inst` `Inst::CallPtr`: lowering puts
      the out-pointer at `args[0]`, before `__env`, but CallPtr always loads
      the env into rcx and shifts every argument up one, so the callee gets
      the env as its sret pointer and vice versa (nat_ir `ng_callptr_sret`, C
      and wasm were fixed). Trigger: `fn mk(x: i64, y: i64) -> P`, `fn pick(k:
      i64) -> fn(i64, i64) -> P { return mk; }`, `val q: P = pick(0)(12,
      13);`: emit prints 0 / 5368828464, run and C 12 / 13; from a `Vec<fn(..)
      -> P>` 0 0. tests/bootstrap/test_isel_fnval_sret.fd fails on emit.
      Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (pe_ir.fd `Inst::CallPtr`: a struct, enum or array result's
      out-pointer goes after `__env`, as the callee expects);
      test_isel_fnval_sret passes on run, C and emit and --nat (Nov 25).
- [x] BG136. **u64 to f64/f32 is converted as signed on the machine layer and
      on wasm (BG67 fixed only interp and pe_ir).** nat_ir.fd
      `Inst::IntToFloat(value, _, to)` drops the source type and
      `ng_i2f`/`ng_i2f_fr` always emit `cvtsi2sd`/`scvtf`/`fcvt.d.l` (AArch64
      needs `ucvtf`, RISC-V `fcvt.d.lu`); wasm.fd `Inst::IntToFloat` always
      emits `f64.convert_i64_s` (0xB9) / `f32.convert_i64_s` (0xB4) instead of
      0xBA/0xB5. Trigger: `fn conv(a: u64) -> f64 { return a as f64; }`,
      `conv(18446744073709551615)`: run 18446744073709551616, x86_64-linux /
      `--nat` / wasm -1; `as f32` wrong too. Confirmed Nov 24. Confidence:
      high.
      Fixed Nov 24 (nat_ir.fd `Inst::IntToFloat` keeps the source type: x86-64
      halves keeping the low bit and doubles, AArch64 `ucvtf`, RISC-V
      `fcvt.d.lu`; wasm.fd `f64/f32.convert_i64_u`);
      test_u64_to_float_unsigned passes on run, C and emit, --nat and `fid
      wasm` (Nov 25; x86_64-linux not run: no Linux host here).
- [x] BG137. **`fid wasm` does not narrow f64 values used as f32.** wasm.fd
      `Inst::Store` writes the low 4 bytes of an f64 `ConstFloat` into an
      `*f32` slot, and f64 arguments to f32 parameters and f64 operands of f32
      arithmetic are not demoted either (nat_ir has `ng_store_f64_to_f32`, C
      `*(float*)p = d`). Trigger: `val ff: f32 = 0.1; println(ff); val a: f32
      = 1.5; val b: f32 = a * 2.0; println(b); println(b > a);`: run and C
      `0.10000000149011612`, `3`, `true`; wasm
      `-0.0000000000000000000000158…`, `0`, `false`; `half(3.0)` gives NaN.
      Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (wasm.fd: an f64 stored, passed or used as f32 is narrowed
      with `f32.demote_f64`); test_f32_from_f64_values passes on run, C and
      emit and `fid wasm` (Nov 25).
- [x] BG138. **Escape check: a closure can return a reference to its own
      captured variable.** escape.fd `esc_closure` checks the closure as its
      own function where `&a` of a captured local does not count as local, and
      `esc_call` gives the call of a closure value no roots, so the caller may
      return or keep it. Trigger: `var a: i64 = 41; val f = fn() -> &i64 =>
      &a; println(int_to_str(*f()));`: `check` passes, `run` prints 0 (want 41
      or a rejection); returned from `mk()` and read after another call it
      prints 5400. Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (escape.fd `esc_closure`: `&` of a captured variable is a
      reference to the closure's own copy, which it cannot return);
      test_escape_closure_captured_ref_fail and _returned_fail are rejected,
      test_borrow_init_defer_valid passes on run, C and emit (Nov 25).
- [x] BG139. **Borrow check: a reborrow through a shared reference does not
      keep the original borrowed.** borrowck.fd
      `maybe_record_borrow_from_expr` / `extract_field_path`: `val r = &w[0]`
      with `w = &v` records a borrow of `w`, not `v`, so the borrow of `v`
      ends at `w`'s last use and `v.push` is allowed while `r` points into the
      old buffer. Trigger: `val w: &Vec<i64> = &v; val r: &i64 = &w[0];`, push
      100000 times, `println(int_to_str(*r))`: passes `check`, run and emit
      print 0 (want 111). Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (borrowck.fd `maybe_record_borrow_from_expr`: `&w[0]` with
      `w = &v` borrows v); test_borrow_reborrow_through_ref_fail is rejected
      and test_borrow_init_defer_valid passes on run, C and emit (Nov 25).
- [x] BG140. **Borrow check: a read inside `defer` counts at the `defer`
      statement, not at the scope's end.** borrowck.fd last-use computation
      walks a `Stmt::Defer` body where it appears, so the borrow ends at the
      `defer` line and later mutations are allowed; the deferred read then
      runs on freed memory. Trigger: `val r = &v[0]; defer {
      println(int_to_str(*r)); }`, then push 100000 times: passes `check`, run
      and emit print 0 (want 111). Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (borrowck.fd last-use computation: a read inside a `defer`
      body counts at the scope's end); test_borrow_defer_read_fail is rejected
      and test_borrow_init_defer_valid passes on run, C and emit (Nov 25).
- [x] BG141. **Borrow check: a borrow made inside an `if`/block/`match`
      initializer is not kept.** `val r = if c { &v[0] } else { &v[1] };` and
      `val r = { &v[0] };` pass `check`, and after `v` grows `run`/emit print
      0. docs/file-analysis/borrowck.md lists "a borrow nested in an
      initializer is not promoted" as LOW and says the borrow is kept to the
      scope's end; it is in fact unsound. Confirmed Nov 24 (if and block
      forms; match form passes check, not run with reallocation). Confidence:
      high.
      Fixed Nov 24 (borrowck.fd: a reference made in an `if`, block or `match`
      initializer is the variable's borrow);
      test_borrow_{if,block,match}_init_ref_fail are rejected and
      test_borrow_init_defer_valid passes on run, C and emit (Nov 25).
- [x] BG142. **Public consts and globals of the same name in two modules are
      merged into one.** compiler.fd `merge_programs` merges consts and
      globals by bare name (the later wins) and `cc_find_module_conflicts`
      checks only structs. Trigger: `ma.fd` `pub const K: i64 = 1; pub fn a()
      -> i64 { return K; }`, `mb.fd` the same with `K = 2` and `b`, main
      prints `ma::a()` and `mb::b()`: run, build and C print `2 2` (want `1
      2`); with `pub var COUNT` of 100 and 200 incremented by each module,
      `201 202` (want `101 201`): one shared counter. Confirmed Nov 24.
      Confidence: high.
      Fixed Nov 24 (compiler.fd `cc_qualify_shadowed_values`: each module
      keeps its own consts and globals; `ma::K` names ma's copy);
      test_module_same_names (`1 2`, `101 201 102`) and
      test_module_same_names_ambiguous_fail pass on run, C and emit (Nov 25).
- [x] BG143. **Type aliases of the same name in two modules: one silently
      replaces the other; same-named enums give misleading errors.**
      compiler.fd `merge_programs` (type alias and enum loops) and
      `cc_find_module_conflicts` (neither covered). Trigger: `ma.fd` `pub type
      Num = i64; pub fn a() -> Num { val n: Num = 7; return n / 2; }`, `mb.fd`
      `pub type Num = f64; ...`: run and build print `3.5` for `ma::a()` (want
      3): ma's integer division became float division. Two `pub enum Kind`
      with different variants give "enum 'Kind' has no variant 'X'" instead of
      a duplicate-definition error. Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (compiler.fd `merge_programs` / `cc_find_module_conflicts`:
      type aliases are kept per module, same-named enums are a
      duplicate-definition error); test_module_same_names (`3 1.5`) and
      test_module_same_enum_fail pass on run, C and emit (Nov 25).
- [x] BG144. **Trait impl methods are named `Type_method` whatever the trait,
      so same-named methods collide.** lower.fd `Lowerer_lower_program`
      registers and lowers an impl method as `ti.type_name + "_" +
      method.name`: two traits with a method of the same name on one type
      become one function (the last lowered wins), and so does an inherent
      `impl S { fn show }` beside `impl Show for S`; nothing reports it.
      Trigger: `cap A { fn m(self) -> i64; } cap B { fn m(self, k: i64) ->
      str; }`, both implemented for `S`: `(s as dyn A).m()` prints "b" under
      `run` (with the same signature in both traits it returns B's value:
      `a.m()` and `b.m()` both give 2), and C fails (`S_m` defined once,
      called with both signatures); an inherent `show() -> i64` beside the
      trait's prints "trait" (run, C). Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (lower.fd `Lowerer_tq_setup`/`lw_tq_resolve`, mono.fd
      `instantiate`'s `__trait_bound`: a trait method that collides with
      another trait's or an inherent method of the type gets its own name);
      test_bg144_trait_method_collision passes on run, C and emit (Nov 25).
- [x] BG145. **A field written through `&mut` to a generic-struct instance is
      silently not stored.** lower.fd `Lowerer_lower_stmt_inner`,
      `Stmt::FieldAssign`, takes the struct name only from `Ptr(Struct)` or
      `Struct`; a `&mut Slot<T>` lowers to `Ptr(Generic Slot<..>)`, the name
      stays "" and the store is skipped with no error (the IR of
      `Slot__put__f64` is a bare `Return`). Trigger: `fn put2(s: &mut
      Slot<i64>, x: i64) { s.v = x; s.n = s.n + 1; }` changes nothing; a
      generic `impl<T> Slot<T> { fn put(self: &mut Slot<T>, x: T) }` the same:
      run, C and emit print the old values. Confirmed Nov 24. Confidence:
      high.
      Fixed Nov 24 (lower.fd `Stmt::FieldAssign`: the struct name is also
      taken from `Ptr(Generic ..)`); test_bg145_generic_field_assign passes on
      run, C and emit (Nov 25).
- [x] BG146. **`x is Variant` checks nothing and takes the variant's index
      from any enum.** typechecker.fd `Expr::IsCheck` only checks the operand
      and returns `bool`; lower.fd `Expr::IsCheck` searches every enum for the
      variant name and the last match wins. Triggers: with `enum Light { Red,
      Green }` and `enum Flag { Green, Red }`, `Light::Red is Red` is false
      (run, C, emit); with a user `enum Reply { Err(str), Ok(i64) }`,
      `Reply::Ok(3) is Ok` is false; `e is Zzz` (no such variant) is true; `5
      is A` on an i64 loads a tag from address 5. Confirmed Nov 24.
      Confidence: high.
      Fixed Nov 24 (typechecker.fd `Expr::IsCheck` needs an enum that has the
      variant; lower.fd tests the variant of the operand's own enum);
      test_bg146_is_variant and
      test_bg146_is_{not_enum,other_enum_variant,unknown_variant}_fail pass on
      run, C and emit (Nov 25).
- [x] BG147. **The DNS client accepts any reply as the answer.**
      stdlib/dns.fd: the query ID is the constant 24817 (`dns_qid`), and
      `dns_query_records` / `dns_parse_records` never compare the reply's ID,
      QR bit, RCODE, question section or answer owner names, nor the sender
      address `net_recv_from` returns. The first UDP datagram from anyone
      becomes the A/AAAA answer and goes into `DnsCache` (spoofing, cache
      poisoning). Trigger: a reply with ID 0x1234, QR=0, RCODE=3 (NXDOMAIN),
      no question and one answer `evil A 6.6.6.6`: `answers: 1`, `6.6.6.6`
      (want rejected). Confirmed Nov 24 (run). Confidence: high.
      Fixed Nov 24 (stdlib/dns.fd: a random query ID, and the reply's ID, QR,
      RCODE, question and answer names are checked); test_std_dns_reply_check
      and test_lib_dns pass on run, C and emit (Nov 25).
- [x] BG148. **C runtime `p_mutex_lock` does not exclude green tasks from each
      other (Windows) and deadlocks them (Linux).** codegen_c.fd
      `p_mutex_lock` with the Windows shim `typedef CRITICAL_SECTION
      pthread_mutex_t`: a CRITICAL_SECTION is recursive, so two tasks on one
      thread both enter it and the locked section interleaves across
      `task_sleep`; the default Linux pthread mutex is not recursive, so the
      second task on the thread deadlocks. Trigger: three `task_go("holder")`
      tasks each doing 5 x `p_mutex_lock(M); v = load; task_sleep(2); store v
      + 1; p_mutex_unlock(M)`: C on Windows prints 5 at FIDES_WORKERS=1 (want
      15); three `async fn` holders print 5 at w1 and 13 at w4; `--target
      x86_64-linux` hangs at w1. Native Linux prints 15. Confirmed Nov 24.
      Confidence: high.
      Fixed Nov 24 (codegen_c.fd `__p_mutex_t`: the lock is a flag with its
      holder under a short internal mutex, and a task that finds it held gives
      its turn); test_mutex_tasks (FIDES_WORKERS=1) and test_mutex_tasks_mn
      (4) pass on C (Nov 25; the Linux C build not run: no Linux host here).
- [x] BG149. **An assignment to a captured variable inside a closure is
      dropped, right-hand side included.** lower.fd `Lowerer_lower_stmt`,
      `Stmt::Assign`, `OptValueId::None` arm: a captured name is neither an
      alloca nor a local of the closure, and that arm neither stores nor
      evaluates the expression, while reads keep coming from the environment;
      `check` accepts it. Trigger: `var count = 10; val f = fn(x: i64) -> i64
      { count = side(); count += x; return count; }; f(1)`: run, C and emit
      print 10 and never call `side()` (want 6 with a by-value copy, and the
      call made); `count = count + x; return count` returns 10. Confirmed Nov
      24. Confidence: high.
      Fixed Nov 24 (lower.fd `Lowerer_assign_capture`: the closure's copy in
      its environment takes the value, by-value captures as reference §4.10,
      and the right-hand side is evaluated); test_bg149_closure_capture_assign
      passes on run, C and emit (Nov 25).

### High

- [x] BG150. **`/=` and `%=` are invisible to `@no-panic`/`@crashfree` and
      `@constant_time`.** typechecker.fd `scan_stmt_for_division` reads only
      the right-hand side of `Stmt::Assign`/`IndexAssign`/`FieldAssign`, never
      the `AssignOp`; the secret-division check of `verify_constant_time` has
      the same gap. Trigger: `@no-panic fn np(a: i64, b: i64) -> i64 { var x =
      a; x /= b; return x; }` is accepted and panics "Division by zero"; `%=`
      gives "Modulo by zero"; `@constant_time` accepts `x /= k` and rejects `x
      = x / k`. Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (typechecker.fd `scan_stmt_for_division` and
      `verify_constant_time` read `/=` and `%=`);
      test_bg150_div_assign_literal and
      test_bg150_{div_assign_nopanic,mod_assign_crashfree,ct_div_assign}_fail
      pass on run, C and emit (Nov 25).
- [x] BG151. **A call through a local or struct-field function value adds no
      effects, so `!{}` and `@deterministic` are bypassed.** typechecker.fd
      `TypeChecker_scan_expr_for_effects`, `Expr::Call`, looks the callee up
      by name only; a local `f` has no entry, and taking `noisy` as a value
      adds nothing (`TypeChecker_passed_fn_effects` covers only arguments of
      higher-order calls; reference §9.1 says other function values get every
      effect). Trigger: `fn noisy() -> i64 { println("io"); return 1; }`, `fn
      p() -> i64 !{} { val f = noisy; return f(); }`: accepted, prints "io";
      same through `val g = s.f; g(3)` and under `@deterministic`. Confirmed
      Nov 24. Confidence: high.
      Fixed Nov 24 (typechecker.fd `TypeChecker_scan_expr_for_effects`: a call
      through a local takes the bound function's or closure's effects, an
      unknown function value every effect); test_bg151_local_fn_effects and
      test_bg151_{local,field}_fn_effects_fail pass on run, C and emit (Nov
      25).
- [x] BG152. **`@no-indirect` (SR-5) and `@no-recursion` are bypassed by a
      local function value.** typechecker.fd `check_safety_body` flags only
      calls to names in `fn_params`; a call to a local such as `f` is neither
      a parameter nor a known function and is skipped. Triggers: `@no-indirect
      fn ni(n: i64) -> i64 { val f = inc; return f(n); }`, `@safety-critical`
      calling `val g = s.f; g(n)`, `@no-recursion fn r(n: i64) -> i64 { val f
      = r; ... return f(n - 1); }`: all accepted (`@bounded-stack` does reject
      the local value). Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (typechecker.fd `check_safety_body`: a local bound to a
      function is checked as a call of it, other function values as indirect
      calls); test_bg152_local_fn_safety and
      test_bg152_{no_indirect_local,no_recursion_local,safety_critical_field}_fail
      pass on run, C and emit (Nov 25).
- [x] BG153. **An argument typed as a bare type parameter in a generic impl
      method is never checked.** typechecker.fd
      `TypeChecker_check_method_args` skips a parameter when
      `!is_single_upper_name(raw)` fails, testing the unsubstituted type, so
      any `x: T` is skipped although its substituted type is known. Trigger:
      `struct Cell<T> { v: T }` with `fn put(self: &mut Cell<T>, x: T)`, `var
      c: Cell<i64> = ..; c.put("oops")`: accepted, run and C print 2;
      `Option<i64>.unwrap_or("d")` and `Result<i64, str>.unwrap_or("q")` are
      accepted and mono instantiates `Option__unwrap_or__str`, which panics "a
      str without a string header". Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (typechecker.fd `TypeChecker_check_method_args`: a `T`
      parameter is checked with the receiver's substituted type);
      test_bg153_generic_param_arg and
      test_bg153_{generic_param_arg,option_unwrap_or}_fail pass on run, C and
      emit (Nov 25).
- [x] BG154. **HashMap method arguments are never checked; Vec `insert`,
      `contains` and the `get` index are not either.** typechecker.fd
      `TypeChecker_check_method_call`: the HashMap branch returns result types
      for `insert`/`get`/`contains`/`get_or`/`remove` without comparing
      arguments with the key and value types or counting them; the Vec branch
      checks the stored element only for `push`/`set`. Triggers: `HashMap<str,
      i64>.insert("a", "b")`, `HashMap<i64, i64>.insert("a", 2)` accepted and
      run; `m.insert(5, "x")` on a `HashMap<str, i64>` panics at run time;
      `Vec<str>.insert(0, 12345)` then `get` panics "a str without a string
      header" (run, C); `Vec<i64>.contains("abc")` and `v.get("x")` accepted.
      Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (typechecker.fd `TypeChecker_check_method_call`: HashMap
      keys and values and Vec `insert`/`contains`/`get` arguments are checked
      and counted); test_bg154_collection_args and the five test_bg154_*_fail
      pass on run, C and emit (Nov 25).
- [x] BG155. **An expression-bodied closure's declared return type is not
      compared with its body.** typechecker.fd `TypeChecker_check_expr`,
      `Expr::Closure`: the body's type is used only to infer, then overwritten
      by the declared type; only explicit `return` statements are checked.
      Trigger: `val f = fn(x: i64) -> str => x; println(f(3));`: accepted,
      `run` panics "a str without a string header" in `__closure_N`. Confirmed
      Nov 24. Confidence: high.
      Fixed Nov 24 (typechecker.fd `Expr::Closure`: an expression body is
      compared with the declared return type); test_bg155_closure_expr_body
      and _fail pass on run, C and emit (Nov 25).
- [x] BG156. **`!b` on a bool and `await` of a `Future<bool>` are typed `i64`
      in the IR.** lower.fd gives the `UnaryOp::Not` result and the bool
      `p_future_poll` result type i64, so C's `Inst::Print` emits
      `p_print_int`: `val b: bool = idi(3) < 1; println(!b);` prints `true`
      under `run` and `1` on C; `!(x < 1.0)` the same; `println(await spawn
      wb(false))` prints `1`. Confirmed Nov 24 (run vs C). Confidence: high.
      Fixed Nov 24 (lower.fd: `!b` and a bool `p_future_poll` result are typed
      bool); test_bg156_not_bool_await passes on run, C and emit (Nov 25).
- [x] BG157. **An `f32` inside an f-string prints as an integer bit pattern.**
      lower.fd `Lowerer_lower_fstring_expr` sends only `IrType::F64` parts to
      `p_float_to_str`; an F32 part goes to `p_to_str`. Trigger: `val ff: f32
      = 1.25; println(f"{ff}");` prints `4608308318706860032` on run and C and
      `0` on wasm; want `1.25`. Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (lower.fd `Lowerer_lower_fstring_expr`: an F32 part is
      widened and goes to `p_float_to_str`); test_bg157_fstring_f32 passes on
      run, C and emit, --nat and `fid wasm` (Nov 25).
- [x] BG158. **`peir_range_refine` refines a local from a load made before a
      store to it in the branch condition.** checks.fd `peir_range_refine`,
      the `Inst::Load(p3, _)` arm records the edge fact on local p without
      checking that p is not stored between that load and the branch
      (`peir_square_local` has that check). Trigger: `if x < (if reset { x =
      100; ten } else { ten }) { if x >= 0 { r = x * 100000000000000000; } }`
      with `g(3, true)`: `run` panics on the multiplication, C prints
      -8446744073709551616. Confirmed Nov 24 (dump-c + zig cc). Confidence:
      high.
      Fixed Nov 24 (checks.fd `peir_range_refine`: no edge fact on a local
      stored between the load and the branch); test_range_load_store_cond_fail
      passes on run, C and emit (Nov 25).
- [x] BG159. **`spawn` with more than 8 arguments silently drops the extras on
      C.** codegen_c.fd `p_thread_spawn`/`p_executor_spawn` store `i <
      num_args && i < 8`, and `__p_thread_run`'s default case calls with 8
      arguments, with no compile error. Trigger: `await spawn
      w8(1,2,3,4,5,6,7,8,9)` with `w8` returning `a+…+h + i*100`: run 936, C
      36. Confirmed Nov 24. Confidence: high.
      Partly fixed Nov 24 (codegen_c.fd `p_thread_spawn`/`p_executor_spawn`:
      more than 8 arguments panic "spawn: a function started by name takes at
      most 8 arguments" at run time instead of being dropped). Left (Nov 25):
      the trigger is still accepted by `check`, C panics where run prints 936,
      and emit prints 536874169636 (pe_ir garbles the ninth argument too);
      want a compile error or support.
      Fixed Nov 27 by rejecting: calls with 9+ arguments already work on every
      backend; the 8-word limit is the runtimes' task block, so typechecker.fd
      (TypeChecker_check_task_arity/async_arity/par_branch_arity) refuses a
      spawn, an async fn or a parallel branch with more than 8, with a Fix line;
      lower.fd runs a parallel branch reading more than 8 locals in place.
      test_spawn_eight_args and three *_fail tests pass (Nov 27, fid built from the Nov 27 tree, scratchpad fb4: 21/21 batch tests pass on run, C, emit; 6/6 on --nat).
- [x] BG160. **C runtime `p_http_parse_request` overflows the heap on a long
      path and reads past the end without a CRLF.** codegen_c.fd
      `p_http_parse_request__raw`: `pos += snprintf(result + pos, cap - pos,
      ...)` adds what snprintf would have written, so a path over ~970 bytes
      pushes `pos` past the 1024-byte `cap` and `cap - pos` wraps; with no
      `\r\n`, `p = line_end + 2` steps past the NUL. Triggers: `"GET /" + 1000
      x "a" + " HTTP/1.1\r\n\r\nhello"` (ASan: heap-buffer-overflow WRITE),
      `"GET / HTTP/1.1"` (global-buffer-overflow READ); with a Host header the
      truncated result makes `p_json_get` over-read. Peer-controlled input.
      Confirmed Nov 24 (gcc -fsanitize=address on the generated C under WSL).
      Confidence: high.
      Fixed Nov 24 (codegen_c.fd `p_http_parse_request__raw`: bounded writes
      and no step past a missing CRLF; the native parser in
      stdlib/pe_runtime/data.fd fixed Nov 25); test_http_parse_request_bounds
      passes on run, C and emit (Nov 25).
- [x] BG161. **C runtime `p_json_pretty` overflows the heap on nested JSON.**
      codegen_c.fd `p_json_pretty__raw`: the buffer is `strlen*4 + 64` bytes;
      the `{ [ } ] , :` branches write a newline plus `indent*2` spaces with
      no capacity check, and indentation grows with the square of the depth.
      Trigger: `{"a": ...}` nested 60 deep around `1`: the Windows build exits
      127; ASan: heap-buffer-overflow WRITE past a 1508-byte block. Confirmed
      Nov 24. Confidence: high.
      Fixed Nov 24 (codegen_c.fd: JSON is read into a tree of values and
      written from it, `_jv_*`; the pretty printer writes from the tree);
      test_json_runtime_values (60-deep nesting pretty-printed) passes on run,
      C and emit (Nov 25; ASan not rerun).
- [x] BG162. **C runtime `p_json_parse`/`p_json_dump` lose string-ness and
      break on escaped quotes.** codegen_c.fd `_json_dump_obj` decides quoting
      from what the value looks like (digits, `-`, `true`/`false`/`null`
      written bare), and the `p_json_parse__raw` scanners end a string at the
      first `"` with no escape handling. Triggers:
      `{"zip":"02134","flag":"true"}` dumps as `{"flag":true,...,"zip":02134}`
      (invalid JSON, a string turned boolean); `{"q":"say \"hi\" now","z":1}`
      dumps as `{" now":1,"q":"say \"}`. BG97 fixed nesting only. Confirmed
      Nov 24 (run vs C). Confidence: high.
      Fixed Nov 24 (codegen_c.fd `_jv_*`: recursive descent keeps a string a
      string and reads `\"`); test_json_runtime_values (`"02134"`, `"true"`,
      `say \"hi\" now`) passes on run, C and emit (Nov 25).
- [x] BG163. **Native runtime `p_json_parse`/`p_json_dump` still break nested
      values.** stdlib/pe_runtime/data.fd `pert_json_parse` (a flat scan): an
      object or array value is copied as a number up to the next `, } ]` or
      space, the scan continues inside it and lifts inner keys to the top
      level, and strings end at the first `"`. data.fd is merged into nat_ir
      builds too. Triggers: `{"list": [1, 2, 3], "k": -4}` dumps as
      `{"k":-4,"list":"[1"}` on emit; `{"o": {"p": 2}}` as `{":":"","o":"{"}`.
      BG97 fixed only codegen_c (its test is `//@ backends: ccompile`).
      Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (stdlib/pe_runtime/data.fd `ped_jv_*`: the same recursive
      descent as C and the interpreter); test_json_runtime_values (nested
      objects and arrays) passes on run, C and emit (Nov 25).
- [x] BG164. **pe_ir: a function with more than 8 interpolated strings and a
      bool interpolation panics "a str without a string header".** After
      `LOWER_STACK_STR_MAX` (8) LF13 stack strings, later f-strings fall back
      to heap builtins; with a bool part the program panics on emit. Trigger:
      `println(f"p = ({x}, {x})"); val ok: bool = x == 1; println(f"a={ok}
      c"); println(f"b {x}");`: emit prints two lines then panics; run, C and
      `--nat` print three. Persists with every FID_PE_OFF switch and
      FID_IROPT_OFF=all; tests/bootstrap/test_frontend_fstring.fd fails the
      same way on emit. Suspected: pe_ir_stubs.fd `PeirGen_gen_to_str_bool`
      returns a headerless `p_malloc` "true"/"false". Confirmed Nov 24 (cause
      not pinned). Confidence: high (bug), medium (cause).
      Fixed Nov 24 (pe_ir_stubs.fd `PeirGen_gen_to_str_bool`: a headered
      string); test_peir_fstring_bool_heap and test_frontend_fstring pass on
      run, C and emit (Nov 25).
- [x] BG165. **nat_ir: an f64 value reaching an f32 parameter, return value or
      `Vec<f32>` element becomes 0.** Lowering leaves a float literal
      `ConstFloat ... f64` where the target is f32 (`fn mk() -> f32 { return
      2.5; }`); nat_ir narrows only in `Inst::Store` (`ng_store_f64_to_f32`)
      and `p_f32_to_bits`, not at call arguments, `Return` or the
      `Vec_push/set/insert` and index-store calls, so the low 32 bits of the
      double are used. Triggers: `fn takes(x: f32) -> f64 { return x as f64;
      }` with `takes(1.5)`, `mk()`, `v.push(1.5)`, `w.set(0, 3.5)`,
      `w.insert(0, 4.5)`, `w[1] = 5.5` on a `Vec<f32>`: run 1.5 … 5.5,
      x86_64-linux and `--nat` 0 each (`1.5 as f32` is right). Existing tests
      always write `as f32`. Confirmed Nov 24. Confidence: high.
      Partly fixed Nov 24 (nat_ir.fd: an f64 is narrowed at call arguments,
      returns and Vec<f32> stores). Left (Nov 25): on --nat
      test_f32_from_f64_values still prints 0 for `takes(1.5)` and `mk()` and
      3 for `sum3(1.5, 3, 5.0)` (want 1.5, 2.5, 9.5); with
      FID_IROPT_OFF=inline,inline2,inlineclo every line is right, so the
      inlined call loses the narrowing (pe_ir had the same, fixed Nov 25 by
      peir_callee_params). x86_64-linux not run: no Linux host here.
      Fixed Nov 27: iropt.fd iro_inline_function and iropt_me.fd ime_inline_site
      convert an f64 argument/result to the f32 parameter/return when inlining
      (BitCast F64->F32). test_f32_from_f64_values, test_f32_inline_narrow,
      test_f32_inline2_narrow pass on run, C, emit and --nat (Nov 27, fid built from the Nov 27 tree, scratchpad fb4: 21/21 batch tests pass on run, C, emit; 6/6 on --nat).
- [x] BG166. **nat_ir hands Fides functions to C as callbacks with its own
      convention.** A Fides function's address goes to C unchanged with no
      SysV entry adapter, but inside the layer floats travel in integer
      registers and stack arguments have the layer's layout;
      docs/reference/argument-registers.md says the layer never hands Fides
      functions to C. Trigger (x86_64-linux, dynamic link): C `double
      cb(double (*f)(double, int64_t), double x) { return f(x, 3) + f(x, 4);
      }`, Fides `fn mycb(x: f64, n: i64) -> f64 { return x * (n as f64); }`,
      `cb(mycb, 2.5)` prints 5 (want 17.5); C calling an 8-integer-argument
      Fides function gets garbage in arguments 7 and 8. Pointer/int callbacks
      of up to 6 arguments work (why qsort passes). `--nat` Windows untested.
      Confirmed Nov 24. Confidence: high.
      Fixed in the code Nov 24 (nat_ir.fd `ng_cbx_label`,
      `ng_gen_cbx_adapters`: an entry adapter, System V / Win64 / AAPCS64 /
      lp64d, for each program function handed to C). Left:
      tests/nattools/machine_c_callback_check.sh needs a Linux host (or WSL)
      to run the builds; not run on Nov 25 (no Linux distribution here).
      Checked Nov 27 on Windows: machine_c_callback_check.sh has an
      x86_64-windows lane (`emit --nat` against cbk.dll, default, nofregs,
      nocbadapt, peir): 0 failures with the Nov 27 fid; FID_NAT_OFF=cbadapt
      fails as it should. The Linux lanes are built, not run (no Linux here).
- [x] BG167. **`fid wasm`: string literals have no string header, so dropping
      or copying one panics.** wasm.fd `wg_str` writes the bytes and a NUL
      into the data segment with no `[P_STR_STATIC][len]` words, and core's
      strict `Str_drop`/`Str_dup` (`P_STR_STRICT`) panic on a headerless
      string as soon as ownership drops a literal-valued temporary or owner.
      Trigger: `fn f(n: i64) -> str { return "abc"; }` with `println(f(1));
      println(str_repeat("ab", 3));`: wasm prints `abc` then "Panic: a str
      without a string header"; a struct with a literal field panics at scope
      end. Confirmed Nov 24 (node scripts/wasm-run.mjs). Confidence: high.
      Fixed Nov 24 (wasm.fd `wg_str`: a literal gets its `[P_STR_STATIC][len]`
      words); test_wasm_str_literal_header passes on run, C and emit and `fid
      wasm` (Nov 25).
- [x] BG168. **`fid wasm` keeps the unsigned bits on a same-width
      unsigned-to-signed cast (BG38 fixed interp and pe_ir only).** wasm.fd
      `Inst::BitCast`, integer branch, just loads the value: a zero-extended
      `u16`/`u8` becomes `i16`/`i8` with no `i64.extend16_s`/`extend8_s`.
      Trigger: `val g: u16 = 65535; println(g as i16); val q: u8 = 250;
      println(q as i8);`: run and C -1, -6; wasm 65535, 250. Confirmed Nov 24.
      Confidence: high.
      Fixed Nov 24 (wasm.fd `Inst::BitCast`: a same-width unsigned-to-signed
      cast sign-extends with `i64.extend16_s`/`extend8_s`);
      test_wasm_narrow_sign_cast passes on run, C and emit and `fid wasm` (Nov
      25).
- [x] BG169. **`fid build-x86` panics on every program.** compiler.fd
      `build_x86_linux` → `emit_object_file` → elf64.fd
      `ElfEmitter_encode_mov` (and `parse_dollar_imm`) call `str_parse_int` on
      the `$imm` text; core's `String_parse_int` overflows on the decimal
      i64::MIN the x86.fd backend writes (see the `String_parse_int` entry).
      Trigger: `fid build-x86 h.fd -o h` on `fn main() -> i64 { return 3; }`:
      "Panic: Integer overflow on addition" in `String_parse_int ←
      ElfEmitter_encode_mov`. known-bugs 2g.16 says only "untested". Confirmed
      Nov 24. Confidence: high (crash), medium (which immediate).
      Fixed Nov 24 (elf64.fd `elf_parse_int`: the `$imm` text is read with
      i64::MIN included, not through core's `str_parse_int`); `fid build-x86`
      on the trigger builds an x86-64 ELF (Nov 25; not run: no Linux host
      here).
- [x] BG170. **@secret: the right operand of `&&`/`||` is not checked under
      the secret condition.** secrecy.fd `sec_expr`, `Expr::Binary`, only
      unions labels and pushes no pc for the right operand, which runs only
      depending on the secret left side (the `if`-expression form is
      rejected). Trigger: `fn noisy() -> bool { println("hi"); return true;
      }`, `fn check(@secret pin: i64) { val b = pin == 1234 && noisy(); }`:
      passes `check`, prints "hi" only when the pin matches. Confirmed Nov 24.
      Confidence: high.
      Fixed Nov 24 (secrecy.fd `sec_expr`: the right operand of `&&`/`||` is
      checked under the left operand's label); test_secret_and_rhs_fail and
      test_secret_or_rhs_fail are rejected (Nov 25).
- [x] BG171. **@secret: pipe-forward into a helper skips the helper's
      summary.** secrecy.fd `sec_expr`, `Expr::PipeForward`, unions labels
      instead of applying the call summary as `sec_call` does. Trigger: `fn
      show(x: i64) { println(int_to_str(x)); }`, `fn check(@secret pin: i64) {
      pin |> show; }`: passes `check`, `run` prints the secret; `show(pin)` is
      rejected. Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (secrecy.fd `Expr::PipeForward` applies the call summary as
      `sec_call` does); test_secret_pipe_fail is rejected (Nov 25).
- [x] BG172. **@secret: `p_system` is not an output sink.** secrecy.fd
      `sec_is_output_sink` lacks `p_system` (and, by reading, `p_exec`,
      `p_setenv`, `p_serial_write`, `p_log_user`, `p_db_exec`). Trigger: `fn
      check(@secret pin: i64) { p_system("echo " + int_to_str(pin)); }`:
      passes `check`, `run` prints 4242. Confirmed Nov 24 for `p_system`.
      Confidence: high.
      Fixed Nov 24 (secrecy.fd `sec_is_output_sink`: `p_system`, `p_exec`,
      `p_setenv`, `p_serial_write`, `p_db_exec`, `p_process_run`,
      `p_spawn_process`); test_secret_system_fail is rejected (Nov 25).
- [x] BG173. **Private consts and globals of the same name in two modules give
      a false visibility error.** Same root as the merge entry above: only one
      of the two survives, still marked with its module, and the other
      module's code then fails `typechecker.fd`'s visibility check. Triggers:
      private `const K` in `ma` and `mb`, each used by its own module:
      "Visibility error: 'K' is private to module 'ma' (not accessible from
      'mb')"; private `var COUNT` the same; the main file's own `const LIMIT =
      5` beside a module's private `LIMIT` is rejected too. Valid programs
      rejected. Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 with BG142 (compiler.fd `cc_qualify_shadowed_values`:
      private consts and globals of one name in two modules, or the program
      and a module, each stay their module's); test_module_same_names (`100
      200 5`) passes on run, C and emit (Nov 25).
- [x] BG174. **`fid fmt` joins newline-separated statements onto one line, and
      `-w` can write a program that means something else.** fmt.fd printer
      loop breaks lines only after `; { }` and some commas, and `fmt_tokenize`
      keeps newlines only as blank-line markers, so a block of semicolon-free
      statements (reference §1.10: semicolons are optional) prints on one
      line; `fmt_same_tokens` does not see newlines, but the parser does
      (parser.fd: a `(` on the next line starts a statement). Trigger: `val z
      = id(4)` with `(z)` on the next line: runs; after `fid fmt a.fd -w` it
      becomes `val z = id(4)(z)` and fails ("undefined variable 'z'").
      reference.md's own semicolon-free example formats to one line; LSP
      formatting returns the same. Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (fmt.fd: the printer keeps a line break the source had
      where a statement can end; `fmt_tokenize` records which tokens began a
      line); the tests/fmt fixtures (semicolons_in.fd to semicolons.fd, with
      `val z = id(4)` / `(z)`) format as expected, and the trigger keeps `(z)`
      on its own line and runs (Nov 25).
- [x] BG175. **core `String_parse_int` (`str_parse_int`) panics on i64::MIN
      and on long digit strings.** stdlib/core.fd builds the positive
      magnitude with checked arithmetic and no digit limit, so
      `"-9223372036854775808"` and any value over 19 digits overflow and
      panic, where its doc says it returns 0 on failure. It is the root of the
      `build-x86` crash and of the HTTP Content-Length crash (separate
      entries); four reviewers hit it independently. Confirmed Nov 24 (run,
      C). Confidence: high.
      Fixed Nov 24 (stdlib/core.fd `String_parse_int`: the magnitude is built
      negative with a digit limit, i64::MIN parses, an overflow returns 0);
      test_std_parse_int_overflow passes on run, C and emit (Nov 25).
- [x] BG176. **HTTP server: one Content-Length header crashes the process.**
      stdlib/http.fd `http_read_message` hands the raw Content-Length to
      `str_parse_int`, which panics on a long value (`url_port_of` guards
      against exactly this, VF15). Trigger: a request with `Content-Length:
      99999999999999999999`: "Panic: Integer overflow on multiplication" in
      `String_parse_int ← http_read_message`, exit 1: any client can stop the
      server. Confirmed Nov 24 (ccompile, a fake recv hook). Confidence: high.
      Fixed Nov 24 (stdlib/http.fd `http_read_message`: Content-Length read as
      digits with a bound, not through `str_parse_int`; a value over
      HTTP_MAX_BODY is answered 413); test_std_http_framing passes on C and
      emit (Nov 25).
- [x] BG177. **HTTP message framing is not validated, so requests can be
      smuggled.** stdlib/http.fd `http_read_message`, `hc_chunked`,
      `http_header`: duplicate Content-Length headers (the first wins), a
      Transfer-Encoding other than exactly `chunked` (`gzip, chunked` falls
      back to Content-Length; RFC 9112 requires a 400 and closing), a negative
      Content-Length (read as 0), and a chunk not followed by CRLF are all
      accepted, and the leftover body is read as the next request on the
      keep-alive connection. Triggers: `Content-Length: 0` + `Content-Length:
      27` with body `GET /smuggled ...` gives two requests; `Content-Length:
      -5` the same; `3\r\nabcGARBAGE\r\n0\r\n\r\n` accepted as `abc`. The risk
      is behind a proxy that frames differently. Confirmed Nov 24 (ccompile).
      Confidence: high.
      Fixed Nov 24 (stdlib/http.fd `http_read_message`, `hc_chunked`:
      duplicate or negative Content-Length, a Transfer-Encoding other than
      `chunked` and a chunk without its CRLF are rejected and the connection
      closed); test_std_http_framing passes on C and emit (Nov 25).
- [x] BG178. **JSON: the validator recurses with no depth limit, so the
      never-panicking API panics.** stdlib/json.fd
      `j_scan_val_n`/`j_scan_arr_n`/`j_scan_obj_n`, under `json_is_valid` and
      every `*_safe` accessor (`json_index` has `JSON_MAX_DEPTH`; this path
      does not). Trigger: `json_type_safe(str_repeat("[", 16000000))` (a size
      `HTTP_MAX_BODY` allows): "Panic: Stack overflow" on C; about 333 nested
      `[` already hit the recursion limit under `run`. Want false / "error".
      Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (stdlib/json.fd `j_scan_val_n`/`arr_n`/`obj_n`: a depth
      limit, as `json_index`); test_std_json_depth passes on run, C and emit
      (Nov 25).
- [x] BG179. **`Vec<f64>.sort()` orders negative numbers backwards.**
      stdlib/core.fd `Vec_sort` / `vec_raw_sort` compare the raw i64 bit
      patterns, which reverses the order of negative doubles (2g.57 is the
      `str` case). Trigger: `[-1.5, -2.5, 1.0, -0.5].sort()` gives `-0.5 -1.5
      -2.5 1` (want `-2.5 -1.5 -0.5 1`), run and C. Confirmed Nov 24.
      Confidence: high.
      Fixed Nov 24 (stdlib/core.fd `Vec_sort`/`vec_raw_sort` compare f64
      elements as floats; lower.fd passes the element type for `.sort()`);
      test_std_vec_sort_f64 and test_bg179_vec_sort_f64_method pass on run, C
      and emit (Nov 25).
- [x] BG180. **f-string float precision gives wrong digits for large values,
      and fill characters are ignored.** stdlib/core.fd `fstr_fmt_f64`,
      `__fstr_pad`, `__fstr_width0` (uncommitted work in the tree on Nov 24):
      `((av * scale) + 0.5) as i64` saturates once `av * 10^prec` passes
      ~9.2e18 and prints the saturated value; the `[fill]` of reference §1.8
      is never read; a `0` fill is taken as the width's start. Triggers:
      `f"{1e17:.2}"` gives `92233720368547758.07`, `f"{10.0:.18}"`
      `9.223372036854775807`, `f"{7:*>5}"` `    7` (want `****7`),
      `f"{7:0>5}"` `7` (want `00007`). By reading, `fstr_fmt_i64` computes `0
      - v` (overflow on i64::MIN). Confirmed Nov 24 (run, C). Confidence:
      high.
      Fixed Nov 27: `fstr_fmt_f64` makes the integer part and the fraction
      apart (below 2^53 the integer part as an i64 and the fraction scaled;
      from 2^53 the value is whole and `float_to_str` has its digits), so
      nothing saturates. `__fstr_pad` reads `[fill][<|>|^]`
      (`__fstr_body`), and the flags, the width and the conversion letter
      are read after it, so a fill byte is neither a flag nor the width.
      `fstr_fmt_i64` takes its magnitude as a u64. Halves still round away
      from zero (0.125 at two decimals is 0.13). Left: digits past about the
      16th significant one are not the exact decimal expansion (0.1 at 18
      decimals prints `0.100000000000000000`). Test
      test_bg180_fstring_precision_fill (the four triggers, 1e22, the
      smallest i64 with a spec, a fraction that rounds up to a whole, fill
      with right, left and center alignment, with a sign, with a
      conversion, a fill that is `0` or `x`): passes on run, ccompile and
      emit with the fid-dev of Nov 25; the 59 other f-string programs of
      tests/bootstrap and tests/conformance pass. Found on the way: BG247.
- [x] BG181. **`==` compares raw words for `Option`/generic enums, tuples,
      arrays and Vec fields.** lower.fd `Expr::Binary` Eq/Neq:
      `vec_eq_fn_for(Lowerer_infer_expr_ast_type(..))` does not see a field
      access, `__eq_<Enum>` exists only for non-generic enums, and tuples and
      arrays reach a plain `BinOp`; the type checker accepts all of them.
      Triggers, each false on run, C and emit: `Option::Some(3) ==
      Option::Some(3)`, `(1, "a") == (1, "a")`, `[1, 2] == [1, 2]`, `p.v ==
      q.v` with equal Vecs (locals `a == b` are true), an `Option` field
      inside a struct `==`; so `@derive(PartialEq)` on a struct with a Vec
      field returns false for equal values. Confirmed Nov 24. Confidence:
      high.
      Fixed Nov 24 (lower.fd `Expr::Binary` Eq/Neq: field accesses, generic
      enums, tuples and arrays compare by content); test_bg181_eq_parts passes
      on run, C and emit (Nov 25).
- [x] BG182. **`?.` is typed `i64` unless the receiver is a bare struct.**
      typechecker.fd `Expr::OptionalFieldAccess` looks up only `Type::Named`;
      an `Option<P>` (Generic) falls through to `return Type::I64`, even for a
      field that does not exist. Trigger: `a: Option<P>`, `val f: f64 = a?.f`
      prints 4612811918334230528 on run and 2 on C; `a?.s` (a str) gives an
      address; `a?.zzz` is accepted and gives 0. Confirmed Nov 24. Confidence:
      high.
      Partly fixed Nov 24 (typechecker.fd `Expr::OptionalFieldAccess` looks
      through `Option<P>`: a missing field or a wrong type is an error);
      test_bg182_optional_field (i64 fields) and its two _fail pass on run, C
      and emit (Nov 25). Left: an f64 field still reads wrong in the lowering,
      `val f: f64 = a?.f` with f = 2.5 prints 4612811918334230528 on run, 2 on
      C, 4612811918334230500 on emit; a str field (`a?.s`) is right.
      Fixed Nov 27 (lower.fd OptionalFieldAccess: the struct comes from the
      receiver's Option payload; f64/f32 fields use their own type).
      test_bg182_optional_float_field passes (Nov 27, fid built from the Nov 27 tree, scratchpad fb4: 21/21 batch tests pass on run, C, emit; 6/6 on --nat).
- [~] BG183. **`dyn` of a generic impl fails at run time.** With `impl<T> Show
      for Slot<T>`, `Slot<i64> as dyn Show` then `.show()`: run panics
      "DynCall: no method 'show' on type 'Slot'", the C build exits 127, emit
      gives "Illegal instruction". lower.fd `Expr::Cast` / `Type::Dyn` builds
      `MakeDyn` with the bare name `Slot`, while mono names the methods
      `Slot__show__i64`. Confirmed Nov 24. Confidence: high.
      Partly fixed Nov 24 (lower.fd `Lowerer_dyn_generic_name`: each instance
      gets forwarders `Slot__i64_show` that every backend's dispatch finds);
      test_bg183_dyn_generic_impl passes on run, C and emit (Nov 25). Left:
      the forwarders exist only for an instance whose method is also called
      directly (as the test does); the entry's trigger (`Slot<i64> as dyn
      Show` then `.show()`, no direct call) still panics "DynCall: no method
      'show' on type 'Slot'" on run and does not build on C or emit (Nov 25).
      Nov 27: mono.fd instantiates every method of a generic trait impl cast
      to dyn (mono_dyn_methods), reach.fd keeps them (rc_dyn_instance); the
      inline `(q as dyn T).m()` builds (BG184's note). test_bg183_dyn_generic_nocall
      and test_bg184_dyn_cast_inline pass (Nov 27, fid built from the Nov 27 tree, scratchpad fb4: 21/21 batch tests pass on run, C, emit; 6/6 on --nat). Left: a trait default method a
      generic impl does not override.
- [x] BG184. **A trait method returning a struct, called through `dyn`,
      crashes.** lower.fd `DynCall` lowering passes no result pointer for a
      struct return. Trigger: `cap Corner { fn corner(self) -> V2; }`, `(q as
      dyn Corner).corner()`: run reports "Invalid memory access" in
      `Sq_corner`, C exits 3, emit panics. Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (lower.fd `Lowerer_dyn_sret_wrapper`: the call goes to
      `<Type>_<method>__dsret`, which gives the method a heap block, and the
      struct is copied out); test_bg184_dyn_struct_result passes on run, C and
      emit, and so does the trigger through `val d = q as dyn Corner;
      d.corner()` with no direct call (Nov 25). The inline form `(q as dyn
      Corner).corner()` does not build for any result type (an i64 one too: C
      "call to undeclared function 'dyn_get'"); not this bug.
- [x] BG185. **A struct literal built in a generic method stores the address
      of the by-value `self`.** In `impl<T> Slot<T> { fn wrap(self) ->
      Slot<Slot<T>> { return Slot { v: self, n: 42 }; } }` the generated C
      stores the parameter's pointer into `v` (`*(int64_t*)v4 = (int64_t)v2;`)
      instead of the heap copy a literal elsewhere makes (BG6's path).
      Trigger: `a.wrap().get()` gives `{v: 7, n: 7}` instead of `{7, 3}` on
      run and C; the same literal built in `main` reads right. Related to the
      `Vec<Option<T>>` stack-slot entry, a different site. Confirmed Nov 24.
      Confidence: high (result), medium (path).
      Partly fixed Nov 24 (lower.fd `Lowerer_lower_struct_lit_expr`: a
      type-parameter field given a struct is copied to the heap for an
      instance that owns nothing too); test_bg185_generic_lit_self passes on
      run, C and emit (Nov 25). Left: the entry's trigger, `Slot { v: 7, n: 3
      }.wrap().get()`, still gives `7 7` on run, C and emit (want `7 3`).
      Fixed Nov 27 (mono.fd mono_gm_ret_type: a call of a user generic impl's
      method takes the method's substituted return type).
      test_bg185_chained_generic_call passes (Nov 27, fid built from the Nov 27 tree, scratchpad fb4: 21/21 batch tests pass on run, C, emit; 6/6 on --nat). The whole suite was not
      rerun after this change (stopped Nov 27).
- [x] BG186. **`@linear` values are duplicated or lost through expressions and
      discarded results.** typechecker.fd `TypeChecker_consume_linear_in_expr`
      has no arm for enum literals, f-strings, if/match expressions and
      similar forms. Triggers: `val o = Option::Some(t); use_tok(t);` then
      using the payload too is accepted (used twice);
      `println(f"{use_tok(t)}"); use_tok(t);` accepted; a bare
      `Option::Some(t)` is reported "never consumed"; a linear result thrown
      away (`mk();`, `id(t);`) accepted; shadowing `val t = Tok{..}; val t =
      Tok{..};` drops the first silently. Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (typechecker.fd `TypeChecker_consume_linear_in_expr`: enum
      literals, f-strings, if/match values, discarded linear results and
      shadowing); test_bg186_linear_exprs and the five test_bg186_*_fail pass
      on run, C and emit (Nov 25).
- [x] BG187. **`@linear` branches are merged only for an `if` statement.**
      Only `Stmt::If` in `TypeChecker_check_stmt_inner` saves and intersects
      `consumed_vars`; `match` arms just accumulate. Unsafe programs accepted:
      consuming in one `match` arm (lost on the other), a `return` inside an
      arm unchecked, a match expression with one consuming arm, and `if c {
      use_tok(t); } use_tok(t);` (consumed twice; reference §8.7's rule allows
      it). Valid programs rejected: consuming in every match arm, an
      if-expression consuming in both branches. Confirmed Nov 24. Confidence:
      high.
      Fixed Nov 24 (typechecker.fd: `match` arms and if-expressions save and
      merge `consumed_vars` as `Stmt::If` does); test_bg187_linear_branches
      and the four test_bg187_*_fail pass on run, C and emit (Nov 25).
- [x] BG188. **Every enum counts as `@send`, whatever its payload.**
      typechecker.fd `TypeChecker_is_send_type` returns true for any enum, and
      an enum literal is typed plain `Option`, so it takes that branch too
      (reference §10.1 lists "enums" with no condition). Accepted by `check`:
      `spawn work(Msg::H(h))` with `h` holding a `*mut i64`; an enum payload
      holding `fn(i64) -> i64` (BG75's closure check bypassed); `spawn
      work(Option::Some(h))`. Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (typechecker.fd `TypeChecker_is_send_type`: an enum is
      @send when every variant's payload is, an enum literal typed with its
      payload); test_bg188_send_payloads and
      test_bg188_{enum_rawptr,enum_fn,option_rawptr}_send_fail pass on run, C
      and emit (Nov 25).
- [x] BG189. **PostgreSQL SCRAM: AuthenticationOk is accepted before the
      server proves it knows the password.** stdlib/postgres.fd
      `pg_startup_step` returns 0 on `code == 0` in every state, so a server
      that asked for SCRAM-SHA-256 can skip SASLContinue/SASLFinal and
      `pg_scram_verify_server` never runs: mutual authentication is defeated
      (libpq refuses this). Trigger: messages R(10, "SCRAM-SHA-256"), R(0), Z:
      steps return `0 0 1`, `c.ok = true`. Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (stdlib/postgres.fd `pg_startup_step`: once SCRAM-SHA-256
      started, AuthenticationOk counts only after SASLFinal carried the right
      signature, `PgScram.verified`); test_bg189_pg_scram_ok passes on run, C
      and emit (Nov 25).
- [x] BG190. **inflate: a truncated stream decompresses into junk up to the
      output cap, and errors return partial output.** stdlib/compress.fd
      `br_bit` returns 0 past the end and `z_block` never checks for end of
      input, so with a dynamic table whose all-zero code is a literal, a
      stream cut after its header decodes literals until `max_out`; every
      failure path returns the partial output (the doc says empty on error);
      hitting the bomb guard returns `max_out + 1` bytes as data. Trigger: 13
      bytes `5,192,33,9,0,0,0,0,32,255,175,22,0` (zlib gives 9 bytes, not
      finished): `inflate_from(d, 0, 100000)` returns 100001 bytes; through
      `gzip_decompress`/`inflate` the cap is 268M elements (~2 GB of
      `Vec<i64>`); a truncated stored block `1,10,0,245,255,'A','B'` returns 2
      bytes with no error. Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (stdlib/compress.fd `br_bit`/`z_block`: end of input is an
      error, and every failure, the cap included, returns the empty result);
      test_bg190_inflate_truncated passes on run, C and emit (Nov 25).
- [x] BG191. **INI: a value containing a newline injects keys and sections on
      reload.** stdlib/ini.fd `ini_needs_quotes` / `ini_serialize` wrap a
      value containing ` ` in quotes, but `ini_parse` splits lines first, so
      the quotes protect nothing, and `ini_get` returns the first match, so an
      injected key shadows the real one. Trigger: `ini_set(doc, "user",
      "name", "bob role = admin ")`, `ini_set(doc, "user", "role", "guest")`,
      serialize, parse: `role` reads `admin`. Also lost on a round trip: a
      value `"quoted"` comes back unquoted, a leading tab is dropped.
      Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (stdlib/ini.fd `ini_serialize`/`ini_parse`: a quoted value
      escapes backslash, quote, LF and CR and the parser undoes them; a value
      starting with a quote or a tab is quoted); test_bg191_ini_newline passes
      on run, C and emit (Nov 25).
- [x] BG192. **Redis RESP decoder: a hostile reply panics the process, and
      attribute chains bypass the depth limit.** stdlib/redis.fd
      `rs_parse_int`'s guard `v > 922337203685477580` lets `…580 * 10 + 8`
      through, `rs_item`'s MAP/ATTR `n * 2` is unchecked, and an attribute
      recurses at the same depth so `RESP_MAX_DEPTH` never applies. Triggers:
      `:9223372036854775808  ` and the valid `:-9223372036854775808` panic
      "Integer overflow on addition"; `%5000000000000000000  ` panics on
      multiplication (run, C); 2000 x `|0  ` exceeds the interpreter's
      recursion limit. Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (stdlib/redis.fd `rs_parse_int` accumulates negative, so
      i64::MIN parses and nothing overflows; counts over `RESP_MAX_COUNT` are
      a bad length; each attribute counts one level); test_bg192_resp_int
      passes on run, C and emit (Nov 25).
- [x] BG193. **protobuf: a length varint near 2^63 panics the reader.**
      stdlib/protobuf.fd `pb_read_bytes` / `pb_skip` (not `@wrapping`) test
      `r.pos + n > r.end`, which overflows for a huge `n`, so the bounds check
      itself traps on an untrusted LEN field. Trigger: bytes `0A FF FF FF FF
      FF FF FF FF 7F`, then `pb_next` + `pb_skip`: "Panic: Integer overflow on
      addition: 10 + 9223372036854775807" (want `ok = false`). Confirmed Nov
      24 (run). Confidence: high.
      Fixed Nov 24 (stdlib/protobuf.fd `pb_read_bytes`/`pb_skip` test `n > end
      - pos`); test_bg193_pb_len passes on run, C and emit (Nov 25).
- [x] BG194. **pe_runtime: a channel send or receive inside a green task
      blocks the whole OS thread.** stdlib/pe_runtime/conc.fd `pert_chan_recv`
      / `pert_chan_send`: a full or empty channel calls `pe_k_cond_wait`
      (`SleepConditionVariableSRW`), which sleeps the worker thread instead of
      suspending the task; an `async fn` starts on its caller's thread, so a
      receive in it blocks `main` and the sender never runs. linux_runtime
      (`pe_chan_wait` → `pe_co_leave`) and C (BG8) suspend the task. Trigger:
      `async fn waiter(ch) -> Future<i64> { return channel_recv(ch); }`, `val
      f = waiter(ch); channel_send(ch, 7); println(await f);`: run,
      x86_64-linux and C print 7, pe_ir hangs every time; two `task_go` tasks
      on one channel hang at FIDES_WORKERS=1. Confirmed Nov 24. Confidence:
      high.
      Fixed Nov 24 (stdlib/pe_runtime/conc.fd `pert_chan_recv`/`send`: a green
      task waits parked, not on the OS thread); test_bg194_chan_task_wait
      passes on run, C and emit and --nat (Nov 25).
- [x] BG195. **pe_runtime: a mutex held across a task suspension deadlocks the
      worker.** stdlib/pe_runtime/conc.fd `pert_mutex_lock` is an SRW lock
      taken by the OS thread: task A locks on worker W and sleeps, task B on W
      blocks W itself in `AcquireSRWLockExclusive`, and A is woken onto a
      queue no free worker serves. linux_runtime's `pert_mutex_lock` relaxes
      so that a task holding it across a yield can release it. Trigger: the
      three-holder probe of the C runtime mutex entry: pe_ir hangs at
      FIDES_WORKERS=1 and 2, prints 15 at 4; native Linux 15 at 1 and 4.
      Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (stdlib/pe_runtime/conc.fd `pert_mutex_lock`: a task waits
      for the mutex parked and the holder releases it from any worker);
      test_bg195_mutex_task_sleep passes on emit (Nov 25).
- [x] BG196. **Both native runtimes: `spawn scope` waits for every green task
      in the program.** pe_runtime/conc.fd `pert_executor_run_all` waits for
      `pert_live > 0` and linux_runtime/conc.fd's calls `pe_co_drain()`: BG45
      limited the thread half to the scope's own threads, but the task half
      still waits for all live tasks, including ones started before the scope,
      unrelated ones, and the calling task when the scope is inside one. C
      joins only the scope's (reference §10.4). Triggers: `async fn outer(n) {
      spawn scope { val t = spawn work(n); r = await t; } return r + 1; }`
      awaited from main: run and C print 41, pe_ir and x86_64-linux hang; an
      async watcher polling a flag set after the scope: native prints 42 then
      hangs. Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (pe_runtime/conc.fd and linux_runtime/conc.fd
      `p_executor_run_all`: a scope counts the tasks started inside it and
      waits for those only); test_bg196_scope_own_tasks passes on run, C and
      emit and --nat, which runs linux_runtime (Nov 25).
- [x] BG197. **Effect handlers: the continuation loses writes and `return`,
      and `do` in a loop hangs.** lower.fd
      `Lowerer_lower_do_with_continuation` / `Lowerer_create_continuation`
      turn the rest of the `handle` block into a by-value closure (reference
      §9.4 says `cont` is the rest of the block), so the captured-assignment
      entry drops writes in it and a `return` in it only leaves the `__cont_N`
      closure. Triggers: `do E.tick(1); count = 3; println(count)` prints
      `count=0` (run, C, emit); `do E.op(1); return 5;` with a resuming
      handler makes `f()` return 7; `while i < 3 { do E.op(i); i = i + 1; }`
      with a resuming handler never ends. Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (lower.fd `Lowerer_lower_do_with_continuation`: the rest of
      the `handle` block runs in place, so writes, `return` and a loop around
      `do` work); test_bg197_effect_cont_in_place passes on run, C and emit
      (Nov 25).
- [x] BG198. **A `defer` body is resolved in the scope of each exit.**
      lower.fd `Lowerer_emit_defers` / `Stmt::Defer` keep only the AST and
      lower it again at every return or function end, so names resolve to
      whatever is in scope there: a variable of the block holding the `defer`
      has already been dropped, and a shadowing variable around the `return`
      is picked up. Triggers: `if k > 0 { var v = Vec::new(); v.push(k); defer
      { println(v.len()) } } return 0;`: run "Vec index out of bounds", C and
      emit "Invalid memory access"; with a str it prints empty; `var x = 1;
      defer { println(x) }; if true { val x = 99; return 0; }` prints 99 on
      every backend (want 1). Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (lower.fd `Lowerer_emit_defers`: a `defer` body reads the
      variables in scope at its `defer` statement, `Lowerer_defer_capture`);
      test_bg198_defer_scope passes on run, C and emit (Nov 25).
- [x] BG199. **A `return` out of `spawn scope { }` skips the join.** lower.fd
      `Stmt::SpawnScope` emits `p_executor_run_all` only at the body's
      fall-through end, so a `return`, `break` or `?` inside the scope leaves
      with its tasks still running (reference §10.4, CA6: no task is left
      behind) and leaks the executor. Trigger: two tasks that
      `task_sleep(300)` then `p_atomic_add(total, n)`, spawned in a scope,
      then `if early { return 5; }`: C and emit see `total` 0 on the early
      return (want 3). Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (lower.fd `Stmt::SpawnScope`: a `return`, `break` or `?`
      out of the scope joins first); test_bg199_spawn_scope_exit passes on
      run, C and emit (Nov 25).
- [x] BG200. **`await` does not restore f32, narrow signed integer or non-call
      future results.** lower.fd `Expr::Await` converts back only `Ptr`, `F64`
      and `Struct` results, and finds the future's type only when the operand
      is a call, method call or identifier; anything else gets the raw word of
      `p_future_poll` (the `Future<bool>` case is the `!b` entry). Triggers:
      `await r(10)` on a `Future<f32>` (10/4) prints 0 on run and 2 on C (want
      2.5); `(await fa) as i64` on a `Future<i32>` of -15 prints 4294967281 on
      C; `total + await fs.get(i)` over a `Vec<Future<f64>>` and `await (if c
      { a } else { b })` print bit patterns on every backend; `await
      ws2.get(0)` on a `Future<str>` leaks the string. Confirmed Nov 24.
      Confidence: high.
      Fixed Nov 24 (lower.fd `Expr::Await`: f32, narrow signed and other
      results converted back, the future's type found for any operand);
      test_bg200_await_types passes on run, C and emit (Nov 25).

### Medium

- [ ] BG201. **Bitwise and shift operators are accepted on floats.**
      typechecker.fd `Expr::Binary` arithmetic path: two numeric operands give
      `F64` with no integer requirement for `& | ^ << >>` (and `<<=`).
      Trigger: `var f: f64 = 1.5; val g = f & 2.0; println(g);` passes
      `check`, `run` prints 0 and `ccompile` fails ("C compilation failed":
      `&` on doubles); `f <<= 2` panics "Arithmetic error (float, int)" under
      `run`. reference.md §4.2's wording covers these operators too and needs
      the same correction. Confirmed Nov 24. Confidence: high.
- [ ] BG202. **`<` and `>` on two `str` values are accepted, and the backends
      disagree.** The type checker accepts an ordering of two strings, which
      reference.md §4.2 does not define (only `==`). `fid run` panics
      "Arithmetic error (str, str)"; C compares the addresses (`"abc" < "abd"`
      true, `"b" > "abc"` false). Either reject it or define it as a byte
      comparison everywhere. Confirmed Nov 24. Confidence: high.
- [ ] BG203. **`Option::Some(x)` built from a `for x in v` loop variable leaks
      its payload.** lower.fd `Lowerer_lower_for` (Vec branch) /
      `Lowerer_lower_for_vec` record no AST type for the loop variable, so
      `Lowerer_enum_lit_ast_type` types the literal `Option<i64>` (owns
      nothing) and never registers it as a temporary, while the payload still
      gets a `Str_dup`. Trigger: `for x in v { total = total +
      show(Option::Some(x)); }` over a `Vec<str>`: `run --leaks` reports one
      string per iteration never freed; `return Option::Some(x)` in the loop
      leaks one. A `while` loop with `v.get(i)` frees everything. Confirmed
      Nov 24. Confidence: high.
- [~] BG204. **Every C runtime `p_json_parse` leaks its tree.** codegen_c.fd
      `p_json_parse__raw` returns `_json_dump_obj(j)` and frees neither `j`,
      its key/value arrays nor the `strdup`'d entries (libc blocks, invisible
      to `--leaks`); `p_json_dump`, `get` and `pretty` re-parse, so each leaks
      too. Evidence: LeakSanitizer, 1000 rounds of
      `p_json_dump(p_json_parse(...))`: 515,536 bytes in 15,001 allocations;
      also 999 counted strings from `p_str_alloc_c`, probably the parse result
      typed as a JSON handle and never dropped (lowering). Confirmed Nov 24.
      Confidence: high.
      Partly fixed Nov 24 (codegen_c.fd `_jv_free`: the C runtime's parse tree
      is freed); test_json_runtime_values (1000 parses) passes on run, C and
      emit (Nov 25). Left: the counted string `p_json_parse` returns is never
      dropped: `run --leaks` on 1000 x `p_json_dump(p_json_parse(..))` reports
      1000 blocks (36000 bytes) never freed.
      Nov 27: a p_json_parse/get/at result passed straight to a JSON reader is
      a statement temporary (lower.fd lw_json_reader); C's p_json_at owns its
      string. test_bg204_json_parse_drop and test_json_runtime_values: `run
      --leaks` every block freed. Left: `val j = p_json_parse(..)` in a
      variable (typed i64) still leaks one string per parse.
- [ ] BG205. **C runtime `p_socket_poll` uses `select`/`FD_SET` without
      checking the fd against `FD_SETSIZE`.** codegen_c.fd `p_socket_poll`,
      POSIX branch: on Linux an fd of 1024 or more writes past the stack
      `fd_set`, so a server holding more than 1024 descriptors corrupts its
      stack on each poll (stdlib/async.fd readiness, the runtime's own
      nonblocking write loops). Fix direction: `poll()`, as the WASI branch
      does. Read only (an fd of 1500 landed in the neighbouring set, no ASan
      report). Confidence: high.
- [ ] BG206. **Linux runtime `pert_str_to_float` is not correctly rounded.**
      stdlib/linux_runtime/base.fd: up to 18 digits are packed into an i64, a
      mantissa over 2^53 is rounded by `mant as f64` and again by the
      power-of-ten multiply or divide, and exponents over 22 loop on `* 10.0`,
      rounding each step. About 10% of printed doubles do not read back
      natively: 200 of 2000 `str_parse_float(float_to_str(x)) != x` on
      x86_64-linux and `--nat` (0 under `run`), e.g. "1.7976931348623157e308"
      reads 1.7976931348623145e308. Confirmed Nov 24. Confidence: high.
- [ ] BG207. **`--nat` Windows console input: a lone CR read is reported as
      end of input.** stdlib/linux_runtime/win.fd `pe_win_read` strips CR
      bytes on a console handle and returns the count left; `pert_read_line`
      reads one byte per call, so the CR of "abc\r\n" comes back as a 0-byte
      read, read_line sets the EOF flag after the first line, and the next
      call returns an empty line. Read only (needs a real console).
      Confidence: medium.
- [ ] BG208. **disasm: `vzeroupper` (VEX `C5 F8 77`) swallows the next
      instruction.** disasm.fd, VEX branch of the x86 decoder: with no table
      row, the length assumes a ModRM byte follows, which `0F 77` does not
      have. Evidence: `test_simd_native.fd` built `--target x86_64-linux --cpu
      x86-64-v3` disassembles `c5 f8 77 4d 39` as `.byte 0xc5 (vex)` and loses
      `cmp r12, r14`; with nop padding it swallowed 5 bytes. Affects `fid
      disasm`, `explain` and the disasm checks. Confirmed Nov 24. Confidence:
      high.
- [ ] BG209. **F-string: an escaped brace becomes an interpolation.** lexer.fd
      `read_fstring` decodes `\xHH`/`\u{..}` outside braces into the raw text,
      and parser.fd `parse_fstring_parts` then takes that `{` as a part start.
      Trigger: `val a = 42; println("\x7Ba\x7D"); println(f"brace \x7Ba\x7D
      end");` prints `{a}` then `brace 42 end` (same with `\u{7b}`): the only
      way to write a literal brace silently changes meaning. Confirmed Nov 24.
      Confidence: high.
- [ ] BG210. **F-string: sub-lexer errors are dropped, and `:` or `}` inside a
      char literal is mis-split.** parser.fd `parse_fstring_parts` never reads
      `sub_lexer.tokenize()` errors and its `:` scan tracks `"` but not `'`;
      lexer.fd `read_fstring` counts braces without skipping char literals.
      Triggers: `f"{c == ':'}"` with `c = 58` prints `false`; `f"{c == '}'}"`
      prints `false'}`; `f"{a + '}"` prints 7 (want an error); `f"{"\q"}"` is
      accepted though a plain string rejects `\q`. Confirmed Nov 24.
      Confidence: high.
- [ ] BG211. **F-string: a `}` or `{` inside a string literal within the
      braces breaks lexing.** lexer.fd `read_fstring`'s brace loop (and
      `parse_fstring_parts`) does not skip nested string literals. Trigger:
      `println(f"{str_len("a}c")}");` gives "unterminated string" on a later
      line; `"a{c"` gives "unterminated f-string": a valid program is
      rejected. BG101 fixed only fmt.fd. Confirmed Nov 24. Confidence: high.
- [ ] BG212. **Comptime does not evaluate float/int casts, so arithmetic on
      them fails.** comptime_eval.fd `evaluate_with_env`, `Expr::Cast`: a cast
      of a non-int value returns `Cast(v, ty)` unchanged and an int-to-f64
      cast stays a `Cast` node. Triggers: `comptime fn avg(a: i64, b: i64) ->
      f64 { return (a + b) as f64 / 2.0; }` with `comptime avg(3, 4)` ("cannot
      evaluate binop on non-literal"), `comptime (-(7.9 as i64))` ("cannot
      negate non-number"); reference §4.11 makes both fatal. Confirmed Nov 24.
      Confidence: high.
- [ ] BG213. **C header parser evaluates unsigned macro arithmetic as
      signed.** cparser.fd `CpEval`/`CpExpr` drop the `u`/`U` suffix and
      unsigned operands: `#define UDIV (-1u / 2)` gives 0 (C: 2147483647),
      `UNEG (-1u)` -1 (C: 4294967295), `MASK (~0u)` -1 (C: 4294967295). Also
      dropped with no note: `COND (A > 5 ? 1 : 2)`, `CMP (3 < 2)`, `0b1010`
      literals. Confirmed Nov 24 (`fid parse-header`). Confidence: high.
- [x] BG214. **HTTP: a cut-off body is passed to the handler as a complete
      request.** stdlib/http.fd `hc_take`, `hc_chunked`, `http_read_message`:
      when input ends or times out mid-body, `hc_take` returns "" but `method`
      stays set, and `hc_line` returning "" is parsed as a 0-size chunk, so
      the handler runs with an empty body (the comments say a broken message
      gives `method` ""). Triggers: `POST /a ... Content-Length:
      10\r\n\r\nhel` then EOF; `Transfer-Encoding: chunked\r\n\r\n5\r\nhel`
      then EOF: both give `[POST /a] body=<>`. Confirmed Nov 24. Confidence:
      high.
      Fixed Nov 24 (stdlib/http.fd `hc_take`, `hc_chunked`,
      `http_read_message`: a body cut off by end of input or a timeout leaves
      `method` ""); test_std_http_framing passes on C and emit (Nov 25).
- [x] BG215. **UTF-8 decoders accept overlong, surrogate and out-of-range
      sequences.** stdlib/encoding.fd `utf8_decode` skips the checks
      `utf8_valid_bytes` does (its doc promises U+FFFD), and stdlib/unicode.fd
      `uni_to_codepoints` accepts any `C0`–`DF` lead byte, so a byte-level
      `../` filter can be bypassed and normalising produces the real `../`.
      Triggers: `E0 80 AF` decodes to 47 (`/`), `ED A0 80` to a surrogate, `F4
      90 80 80` to 1114112; `uni_lower`/`uni_nfc` of `.. C0 AF x` return
      `../x` while `utf8_valid` says false. Confirmed Nov 24 (run).
      Confidence: high.
      Fixed Nov 27: encoding.fd `utf8_seq_ok` (continuation bytes and the
      first one's range) is now what both `utf8_valid_bytes` and
      `utf8_decode` check, and an invalid sequence gives U+FFFD and consumes
      one byte; unicode.fd `uni_to_codepoints` takes no lead byte below C2
      and checks the second byte through `uni_lead_ok`. Test
      test_bg215_utf8_strict (the four triggers, the lowest encodings, and
      `uni_lower`/`uni_nfc` of `.. C0 AF x`): passes on run, ccompile and
      emit with the fid-dev of Nov 25; test_encoding, test_lib_unicode,
      test_std_jni and test_std_push unchanged.
- [x] BG216. **`tls_crypto::x25519` gets RFC 7748's second test vector
      wrong.** stdlib/tls_crypto.fd `x25519` does not mask bit 255 of u before
      `mod p` (RFC 7748 §5), and its swap is a secret-dependent `if`;
      stdlib/x25519.fd (used by QUIC) is right, but this one stays public in
      the module tls13, tls_record and quic import. Trigger: §5.2 vector 2
      gives `d5f33573…` (want `95cbde94…`); vector 1 is right. Confirmed Nov
      24 (ccompile). Confidence: high.
      Fixed Nov 27: `tls_crypto::x25519` now calls x25519.fd's
      `x25519_scalarmult` (constant time in the scalar, bit 255 of u
      ignored); the module's own BigInt ladder and its six field helpers are
      gone, and nothing in the tree called the old function. Test
      test_bg216_tls_crypto_x25519 (RFC 7748 section 5.2 vectors 1 and 2,
      then the scalar 9 with u = 9 and with vector 2's u, both checked
      against an independent ladder): passes on run, ccompile and emit with
      the fid-dev of Nov 25; test_crypto_tls_vectors,
      test_stdlib_secret_keys and test_stdlib_tls13_schedule unchanged.
- [x] BG217. **decimal: a negative scale prints the wrong value.**
      stdlib/decimal.fd `dec_round` and `dec_div` accept and keep a negative
      scale, and `dec_to_str` prints the mantissa for any scale <= 0 without
      multiplying by 10^-scale. Triggers: `dec_round(dec_parse("1234"), -2,
      DEC_HALF_UP)` prints 12 (want 1200); `dec_div(1000, 3, -1, DEC_HALF_UP)`
      prints 33 (want 330). Confirmed Nov 24. Confidence: high.
      Fixed Nov 27: `dec_to_str` writes the mantissa followed by `-scale`
      zeros (zero stays `0`); `dec_from_units` with a negative scale now
      multiplies instead of dropping the factor. The arithmetic was right
      and is unchanged. Test test_bg217_decimal_negative_scale (the two
      triggers, ties both ways, a negative value, zero, add and mul with a
      negative scale): passes on run, ccompile and emit with the fid-dev of
      Nov 25; test_decimal and test_std_tariff unchanged.
- [ ] BG218. **Two `impl Show for P` in one file are accepted; the later one
      wins.** The CL7 conflict check (compiler.fd) compares only modules.
      Confirmed Nov 24. Confidence: high.
- [ ] BG219. **A generic function with a bound taken as a value skips the
      bound and fails at run time.** `val f = need; f(5)` with `fn need<T:
      A>(x: T)` is accepted and panics "CallPtr: null function value" (C exits
      127). Confirmed Nov 24. Confidence: high.
- [x] BG220. **A generic struct whose fields are all `@send` is rejected by
      `spawn`.** typechecker.fd `TypeChecker_is_send_type`, Generic arm,
      allows only built-in types, so `spawn work(w)` with `w: Wrap<i64>` is
      rejected. A valid program rejected. Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (typechecker.fd `TypeChecker_is_send_type`, Generic arm: a
      generic struct is @send when its substituted fields are);
      test_bg188_send_payloads (`spawn` of a `Wrap<i64>`) passes on run, C and
      emit (Nov 25).
- [x] BG221. **`dns_build_query` mis-encodes a fully qualified name and long
      labels.** stdlib/dns.fd: `dns_build_query("example.com.", 1)` emits an
      empty label for the trailing dot, so the name ends early and the
      following bytes read as QTYPE=0, QCLASS=256 plus a stray byte; a label
      over 63 bytes is silently truncated, so a different name is queried.
      Confirmed Nov 24. Confidence: high.
      Fixed Nov 24 (stdlib/dns.fd `dns_build_query`: a trailing dot ends the
      name, a label over 63 bytes is rejected); test_std_dns_reply_check
      passes on run, C and emit (Nov 25).
- [x] BG222. **gzip and zlib integrity checks are skipped.**
      stdlib/compress.fd `gzip_decompress` never checks CRC32 or ISIZE and
      `zlib_decompress` ignores Adler-32 ("advisory"), so with the
      partial-output behaviour above a corrupted archive decompresses
      "successfully". Read only (plus the stored-block probe). Confidence:
      high.
      Fixed Nov 24 with BG190 (stdlib/compress.fd: gzip checks CRC-32 and
      ISIZE, zlib Adler-32); test_bg190_inflate_truncated (CRC and ISIZE
      flipped, trailer cut) passes on run, C and emit (Nov 25).
- [x] BG223. **url: `url_resolve` builds broken or lossy URLs, and the path is
      decoded too early.** stdlib/url.fd `url_resolve` rebuilds the authority
      from `base.host` without brackets or userinfo, and `url_parse`
      percent-decodes the path (`u.path = url_decode(rest)`), so `%2F` becomes
      a real `/` that `url_to_str` cannot restore. Triggers:
      `http://[::1]:8080/a/b` + `c` gives `http://::1:8080/a/c`;
      `http://user:pw@a.com/b/c` + `d` gives `http://a.com/b/d` (RFC 3986
      §5.2.2 keeps the authority); `http://h/a%2Fb/c` round-trips to
      `http://h/a/b/c`; `..%2F..%2Fetc%2Fpasswd` parses to
      `/files/../../etc/passwd`; `//g` gives `http://g/`. Confirmed Nov 24.
      Confidence: high.
      Fixed Nov 27: `Url.path` is the path as written, percent-encoded
      (`url_decode` gives the decoded form), as RFC 3986 section 3.3 reads it
      and as Python, JavaScript and Rust's `url` crate do. `url_path_escape`
      percent-encodes what a path cannot hold and keeps valid escapes;
      `url_to_str`, `url_resolve` and http_client.fd's request target use
      it. `url_resolve` builds the authority of either side with
      `url_authority` (userinfo, IPv6 brackets, port), and writes the
      reference's path only when it wrote one, so `//g` gives `http://g`
      (RFC 3986 section 5.3). Readers of `url_parse`: http_client.fd (its
      request target is now the path as written) and test_lib_url, whose
      "parse full" now expects `/a%20b/c`. Test
      test_bg223_url_path_and_authority (the five triggers, both branches
      with a userinfo and an IPv6 host, a path set by hand, a lone `%`):
      passes on run, ccompile and emit with the fid-dev of Nov 25;
      test_lib_url (RFC 3986 section 5.4 included), test_lib_http_client,
      test_verify_vf15_parsers and p1_url_port_overflow unchanged. The
      source reached main inside be363179 ("0.8.5 unstable", another
      session's commit of the same tree) with the test's last expected line
      still wrong; the next commit corrects it. Left as it was, the same
      class: userinfo and fragment are still percent-decoded at parse.
- [x] BG224. **xml and toml have no nesting limit, so deep input overflows the
      stack.** stdlib/xml.fd `x_children`/`x_ns_open` and stdlib/toml.fd
      `tp_value`/`tp_array` have no depth counter (the json validator entry is
      the same class). Triggers: `<a>` x 400k (~1.2 MB) gives "Panic: Stack
      overflow" on C; toml `x = [` x 1M the same; both hit the recursion limit
      at depth 1000 under `run`. yaml flow `[` was inconclusive. Confirmed Nov
      24. Confidence: high.
      Fixed Nov 27: xml.fd `XML_MAX_DEPTH` and toml.fd `TOML_MAX_DEPTH`, both
      128, counted in the parser state. XML: past the limit a strict parse
      fails ("elements nested deeper than 128") and a lenient one keeps the
      element without its content. TOML: `tp_value` refuses a deeper array or
      inline table ("value nested deeper than 128"), so toml_load fails and
      toml_parse skips the line. 128 and not JSON's 512: a tree of nodes is
      destroyed recursively too, and under `fid run` a tree 150 deep is
      parsed, copied and destroyed where one 200 deep is not (the recursion
      limit is 1000). Test test_bg224_xml_toml_depth_limit (128 and 129 deep,
      150 unclosed elements, inline tables, the lenient parser): passes on
      run (59 s), ccompile and emit with the fid-dev of Nov 25. The entry's
      large inputs, once through ccompile and emit, in about a second:
      400,000 `<a>` parse lenient, a million `[` is refused, and 20,000
      siblings or array items are not depth. test_lib_xml, test_xml_spec,
      test_lib_toml, test_toml_spec, test_toml_suite_spec,
      test_verify_vf15_parsers, test_std_behavior_tree and test_std_openadr
      unchanged. Two tests near this change fail for other reasons:
      test_std_svd on emit, because the native Windows backend lacks
      p_volatile_load16/p_volatile_store16 (the compiler names them, and the
      test makes no xml or toml call); test_xml_dtd_ns passes on ccompile
      and emit and did not finish on the interpreter in 900 s (its entity
      bomb expands up to 8 MB there). Not looked at: yaml.
- [ ] BG225. **The interpreter's mutex does not exclude between tasks
      either.** `fid run` prints 5 (want 15) for the same three-holder probe
      as the C runtime mutex entry: a task holding `p_mutex_lock` across
      `task_sleep` lets the others in. Confirmed Nov 24. Confidence: high.
- [x] BG226. **Both native runtimes hang on an await cycle, and pe_runtime's
      deadlock detector cannot fire for it.** pe_runtime/conc.fd
      `pert_poller_main` counts every task blocked on a join as a waiter, and
      `looks_dead` needs `waiters == 0`, so "Tasks are deadlocked: each one
      awaits another" is unreachable for exactly that case; linux's
      `pe_co_await` has no detector.
      tests/bootstrap/test_task_await_cycle_fail.fd (`//@ backends:
      run,ccompile`) prints "before" and hangs on pe_ir and x86_64-linux,
      where run and C panic (BG96's behaviour). Confirmed Nov 24. Confidence:
      high.
      Fixed Nov 24 (pe_runtime/conc.fd `pert_poller_main`: the detector fires
      once nothing else can run; linux_runtime's `pe_co_await` panics on the
      await that closes a ring); test_bg226_await_cycle3_fail passes on run, C
      and emit and --nat, and test_task_await_cycle_fail panics "Tasks are
      deadlocked: each one awaits another" on emit and --nat (Nov 25).
- [~] BG227. **pe_runtime: after a task leaves blocked, its worker may finish
      it a second time.** stdlib/pe_runtime/conc.fd `pert_worker_main` (and
      `pert_co_async_v` after `pe_co_enter`) reads `t + 80` after the swap; by
      then the poller may have requeued the task and another worker run it to
      completion and called `pe_co_finish_task` (the code's comment says it
      "may already be running again on another worker"). A second finish
      decrements `pert_live` twice (scopes and `task_run` return early), frees
      the context twice and puts its stack in the cache twice (two later tasks
      share one stack); for a detached task it reads freed memory. C parks
      only after the switch. Read only: 20 x 5000 `task_go` with
      `task_sleep(0)` at 16 workers did not reproduce it. Confidence: medium
      (mechanism).
      Fixed in the code Nov 24 (stdlib/pe_runtime/conc.fd: a task writes how
      it left, 2 finished, 1 runnable, 0 blocked, and whoever resumes it reads
      that, not `t + 80` after the swap). Left: no test reproduces the race
      (the review's stress run did not either).
      Nov 27: test_bg227_task_wake_race.fd (emit, 300 x 256 tasks) passes,
      but it also passes with the bug put back, so it is a stress test, not a
      reproduction. Left: a test that catches the race.
- [ ] BG228. **Every owned argument of an async-task call leaks.** lower.fd,
      the call path for names in `self.async_tasks`: the caller gives the task
      an owner of each str, Vec or map argument (`Lowerer_rc_dup` /
      `Lowerer_rc_own_value`), but parameters are borrowed, so the task body
      never drops them, and a handed-over temporary is no longer dropped by
      the caller; the heap copy of a struct argument in adapted tasks has no
      free either (read only). Trigger: `fn start() -> Future<str> { val s =
      "hello" + int_to_str(42); return word(s); }` with a yielding `async fn
      word(s: str)`: output right, `--leaks` reports the string and Vec blocks
      never freed. `spawn f(s, v)` on threads is clean. Confirmed Nov 24.
      Confidence: high.
- [ ] BG229. **Interpreter: each `txn` keeps a full copy of interpreter
      memory.** interp.fd `Inst::TxnBegin` pushes
      `Memory_clone(&self.memory)`, which the lowering never uses (it rolls
      back itself, `Lowerer_txn_abort`), and memory still grows by about one
      copy per txn entered, even after `TxnEnd`; `break`/`continue`/`return`
      out of a txn skip `TxnEnd`. Trigger: a 20,000-element Vec, then `while i
      < N { txn { n = n + 1; } }`: peak 43 MB without txn, 228 MB at N=30,
      2,277 MB at N=300 under `fid run`; C is unaffected. Confirmed Nov 24.
      Confidence: high (growth).

### Low

- [ ] BG230. **`opt ?? <new value>` leaks the default when the result is
      kept.** lower.fd `Expr::Coalesce` in `Lowerer_lower_expr`: on the None
      path the default is claimed, but the merged result is a plain load, so
      `val r = ...` duplicates it and the default's owner is never dropped
      (the comment says it is "left to leak rather than drop a borrowed Some
      value"). Trigger: `val r2 = get(false) ?? ("d" + int_to_str(9));`: `run
      --leaks` reports 1 block never freed. Confirmed Nov 24. Confidence:
      high.
- [ ] BG231. **C runtime `p_read_stdin(-1)` writes into a 0-byte block.**
      codegen_c.fd `p_read_stdin__raw` calls `malloc(n + 1)` = `malloc(0)`,
      skips the read loop and writes `buf[0] = 0`; other negative `n` are
      caught only through the NULL return. Read only. Confidence: medium.
- [ ] BG232. **disasm has no row for several instructions the encoders emit.**
      disasm.fd encoding table: x86 `blsmsk`/`blsi` (TG7; only `blsr` has a
      row); AArch64 pre/post-indexed `ldr`/`str` (TG8, e.g. `f8008528`), FP
      `ldr/str d`, `ldp/stp d8, d9`, `bic v.16b`, `dup v.2d, v.d[i]`, `mov d,
      v.d[i]`; RISC-V `vsll.vi`, `vmseq.vi`, `vmerge.vim`, `vslidedown.vi`,
      `vfmv.f.s`, `vfmv.v.f` (TG11). They print as `.byte`/`.word` (length
      kept). docs/file-analysis/disasm.md says each encoder has a row.
      Confirmed Nov 24. Confidence: high.
- [ ] BG233. **disasm one-byte forms ignore the 0x66 operand-size prefix and
      REX.B.** disasm.fd one-byte opcode cases: `66 B8 34 12` decodes as `mov
      eax, 0x90901234` (imm32, wrong length; want `mov ax, 0x1234`), `66 99`
      as `cdq` (want `cwd`), `49 90` as `nop` (want `xchg r8, rax`), `F3 90`
      as `nop` (want `pause`). The compiler does not emit these; `disasm
      --raw` and `asm!` output can contain them. Confirmed Nov 24. Confidence:
      high.
- [ ] BG234. **The ELF `.text` section header spans code, data and the RW
      segment.** nat_enc.fd `nat_elf_debug_sections` writes one `.text` header
      flagged WAX over the whole image even after nat_elf_img splits it into
      R-E and RW PT_LOADs, so `readelf -l` maps `.text` to no segment and
      `objdump -d` disassembles data as code. Tooling only. Confirmed Nov 24.
      Confidence: high.
- [ ] BG235. **Lock order: locks taken inside a closure body are not
      checked.** lockcheck.fd `lockcheck_module` and the type checker's
      `lo_summary` add no edges for a call through a closure value. Trigger:
      `main` takes `a` then `b`, a closure `f` takes `b` then `a`, `f()`
      called: `check` passes; the same code as a named or spawned function is
      rejected. A real deadlock only when the closure runs on another thread.
      Confirmed Nov 24. Confidence: high.
- [ ] BG236. **mono: function-type mangling is ambiguous.** mono.fd
      `mangle_type`, `Type::Func`: `"fn_" + params + "_" + ret` with no count,
      so `fn(fn(i64) -> f64, i64) -> i64` and `fn(fn(i64, f64) -> i64) -> i64`
      both become `id__fn_fn_i64_f64_i64_i64` (one instance for two `T`). No
      wrong output today (every function value is one word); BG85 gave tuples
      and arrays a count but not `Func`. Confirmed Nov 24 (dump-ir).
      Confidence: high.
- [ ] BG237. **`fid fmt --check file.fd` (flag before the file) reads the flag
      as the file name** ("could not read file: --check", exit 1); `fid fmt
      file.fd --check` works. Confirmed Nov 24. Confidence: high.
- [x] BG238. **`Money_print` writes negative amounts wrong.** stdlib/money.fd:
      `Money_new(-5)` prints `$0.0-5` and `-105` prints `$-1.0-5` (want
      `-$0.05`, `-$1.05`). Confirmed Nov 24. Confidence: high.
      Fixed Nov 27: `Money_print` writes the sign once, in front, and prints
      the absolute dollars and cents (it negates after the split by 100, so
      the smallest i64 does not overflow). Test test_bg238_money_negative
      (-5, -105, 0, 1234, 7, the smallest i64): passes on run, ccompile and
      emit with the fid-dev of Nov 25; the 13 other money tests unchanged.
- [x] BG239. **`hkdf_expand` does not reject a length over 255 * 32.**
      stdlib/tls_crypto.fd `hkdf_expand`: the counter byte reaches 256 and the
      output stops following RFC 5869, where it should be an error. Read only.
      Confidence: high.
      Fixed Nov 27: `hkdf_expand` panics on a length over 255 * 32 ("more
      than 255 * 32 bytes asked for (RFC 5869)"). Test
      test_bg239_hkdf_expand_limit_fail (RFC 5869 vector 1, then 8161
      bytes): passes on run, ccompile and emit with the fid-dev of Nov 25;
      test_crypto_tls_vectors, test_std_kdf, test_stdlib_secret_keys and
      test_stdlib_tls13_schedule unchanged on ccompile and emit. The limit
      itself (8160 bytes, equal to a shorter expansion over their common
      length) was checked once through ccompile: not a suite test, 255 HMACs
      do not finish in the interpreter.
- [ ] BG240. **A cycle of type aliases overflows the checker's stack.** `type
      A = B; type B = A;` recurses in `TypeChecker_resolve_type` until a stack
      overflow instead of an error. Confirmed Nov 24. Confidence: high.
- [ ] BG241. **`select` drops a received value equal to its "not ready"
      sentinel.** lower.fd `Lowerer_lower_select_stmt` uses the in-band value
      -9223372036854775807 for "nothing ready"; a received value equal to it
      has already been taken from the channel and is treated as nothing.
      Trigger: send -9223372036854775807, then 5: the first `select` prints
      "nothing ready" on run and C and the value is gone. Confirmed Nov 24.
      Confidence: high.

## Found while fixing the Nov 25 batch (TL2/TL4/TL5, the six bugs)

- [ ] BG242. **The type checker accepts a `str` for an `i64` parameter.**
      Seen at a `str_find_char(s, "x")` call (the second parameter is the
      character as `i64`): `fid check` gives no error and the call passes the
      string's address. Found Nov 25 while adding TL2 hints. Confidence:
      medium (the exact rule that lets it through is not located yet).
- [ ] BG243. **`tests/tooling/dbg_decl_lines_check` fails: an inlined
      function has no DWARF subprogram**, so its declaration line is missing
      from the debug info. Found Nov 25 (TL5). Same cause on pe_ir: `fid emit
      -g` without `--debug` inlines `scale`/`area` in test_pe_dbg_vars.fd,
      so lines 29, 30 and 35 have no `.debug_line` row and a breakpoint
      there is never hit (pe_dbg_vars_check.sh: 10 passed, 1 failed, the
      same with the Nov 25 stage 0, so not new). `--debug -g` passes every
      check (25/25 values through the DWARF, 22/22 through the PDB).
      Confidence: high.
- [ ] BG244. **`ccompile -g` names C variables `v0`..`vN`**, so a C debugger
      (gdb, lldb, Visual Studio on the C build) shows IR value numbers, not
      the Fides names. pe_ir's PDB and nat_ir's DWARF carry the names (TL5).
      Found Nov 25. Confidence: high.
- [ ] BG245. **nat_ir's DWARF location lists cover only one-byte ranges**:
      a variable is visible at the instruction that defines it and not over
      the range it lives in, so `p x` in gdb on a Linux native build often
      says `<optimized out>`. Found Nov 25 (TL5). Confidence: medium.
- [ ] BG246. **A call through a function pointer (`CallPtr`) does not
      convert an `f64` argument to an `f32` parameter** on pe_ir: the direct
      call path does since the Nov 25 f32 fix (`peir_callee_params`), the
      indirect one has no parameter types to look at. Found Nov 25.
      Confidence: medium.

## Found by the Nov 27 night run (docs/roadmap/goal-workflow.md)

- [x] BG247. **The default Windows backend's `float_to_str` loses the low
      digits of a large value.** `println(f"{1e25}")` prints
      `10000000000000001000000000` through `fid emit`; `fid run`,
      `ccompile`, `emit --nat` and the exact value of the double give
      `10000000000000000905969664`. 1e17, 1e20, 1e22 and
      123456789012345678.0 print right there, so the conversion keeps 17
      significant digits and writes zeros past them. The conversion is
      `pert_float_to_str` of stdlib/pe_runtime/base.fd (the machine layer's,
      in stdlib/linux_runtime/base.fd, is right). Found Nov 27 while fixing
      BG180, whose test keeps to the values every backend prints right.
      Confidence: high (seen on run, ccompile, emit and emit --nat).
      Cause: `pert_to_str_float` asked the C runtime's printf for the
      digits, and msvcrt keeps 17 significant digits of a value this large.
      Fixed Nov 27: from 2^53 the value is whole, and
      `pe_big_integer_text` makes its digits here (the decimal digits of
      the 53 significant bits, doubled as many times as the exponent says:
      at most 971 doublings of 309 digits). Test
      test_bg247_float_to_str_large (1e25 and -1e25, 2^53, 2^64, 1e25 at
      two decimals, a fraction just under 2^53, 1e300, the largest double,
      whose 309 digits are exact on all three backends): passes on run,
      ccompile and emit with the fid-dev of Nov 25; the 51 other float and
      f-string programs of tests/bootstrap pass (test_bg182_optional_float_field
      fails as before: BG182, another session's); the leak gate and the
      checked allocator pass.
- [ ] BG248. **`--nat` Windows: a call through a function value made from a
      C function's address returns 0.** test_fix_fnval_c_ptr expects 7, 12
      and 5 and says so in its header; pe_ir prints it, `emit --nat` prints
      0 for the first. (The interpreter cannot run the test: native FFI.)
      Found Nov 27 by the whole-suite comparison of MI5; seen twice. Cause
      not located. Confidence: high.
- [ ] BG249. **`--nat` Windows runs out of memory on bench_sleepers** where
      pe_ir runs it: "Panic: out of memory (327680 bytes requested)" in
      main. Found Nov 27 by the same comparison; seen twice. Cause not
      located (the heap does not grow as pe_ir's does, or the program asks
      past its limit). Confidence: high.
- [ ] BG250. **pe_ir hangs on test_read_line_long.** The program runs
      itself with a file of long lines on its standard input (BG41): the
      pe_ir image printed one line and did not finish in 60 s, twice, and
      again by hand with stdin closed; the `--nat` image and the
      interpreter finish. Found Nov 27 by the same comparison. Cause not
      located. Confidence: high.
- [ ] BG251. **The machine layer on Windows lacks five things pe_ir has**,
      so as many suite programs build only through pe_ir: `p_str_join_n`
      (test_pe_str_join_n), `p_inb`/`p_outb` (test_std_x86_cpu), the C
      imports `GetSystemInfo` (test_lib_sys) and `pledge_asset_load_mesh`
      (test_lib_asset), and the bare-metal builtins `p_serial_init`,
      `p_hlt` (test_kernel_hello): "not available in native Windows
      executables yet" or "undefined symbol". Found Nov 27 by the same
      comparison. Confidence: high.

## Found and fixed while reviewing (Nov 22)

- [x] BG100. **`Int_to_str_into` (LF13 stack strings) printed garbage for
      positive numbers and returned a string with no header in front of it.**
      stdlib/core.fd: a positive `x` was never negated (`f"v = {42}"` printed
      `v = ,.`), and the digits were returned at `p + 1` while the header was
      written at `buf`. Fixed in the working tree (digits moved to `buf + 16`,
      `v` kept <= 0); checked on run, C, pe_ir and --nat with 0, -7, i64::MIN
      and i64::MAX.
