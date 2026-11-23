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

Status Nov 23 (after the Nov 22-23 fix batches, built and run on run, C,
emit, nat and the tooling checks): 125 entries, 103 done `[x]`, 8 partly
done `[~]` (BG35, BG40, BG45, BG46, BG60, BG61, BG75, BG110: each names
what is left), 14 open. Open from the review: BG62 and BG63 (language work,
BG122), BG85 (mono.fd), BG108 (ssa.fd, critical). Open and found while
fixing: BG115-BG124.

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
- [~] BG35. **The import cache (CB1) drops `comptime splice` from imported
      modules on a hit.** compiler.fd:659-671 (`cached_parse`): `g_saw_splice`
      is never set from a cached AST. Trigger: the second build of a program
      importing a module that uses `comptime splice`. Confidence: high.
      Partly fixed Nov 22-23 (compiler.fd `cached_parse`: a module that saw a
      `comptime splice` is never kept in the import cache);
      tests/tooling/impcache-splice-check.sh passes. Left: the merged AST
      cache (BG123).
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
      test_socket_write_closed_peer pass on run, C and emit.
- [~] BG40. **C runtime `p_vec_push/set/insert` ignore element codes and shared
      buffers** (heap overflow on `Vec<u8>`, abort on a shared Vec).
      codegen_c.fd:11620-11644, 16288-16293. Confidence: medium-high.
      Partly fixed Nov 22-23 (codegen_c.fd: push, set and insert go through
      the element code and the share box as core's Vec does); test
      test_cback_vec_push_codes pass on run, C and emit. Left: remove,
      reverse, sort, pop and truncate (BG120).
- [x] BG41. **C runtime `p_read_line` splits lines longer than 4095 bytes.**
      codegen_c.fd:12688-12696. Confidence: high.
      Fixed Nov 22-23 (codegen_c.fd: `p_read_line` reads a whole line); test
      test_read_line_long pass on run, C and emit.
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
- [~] BG45. **Spawn scopes join detached threads; `p_thread_join` leaves the
      handle so it is joined twice** (UB; use after free on Windows).
      codegen_c.fd:15681-15691, 15668, 15900-15907. Confidence: high.
      Partly fixed Nov 22-23 (codegen_c.fd: a scope joins only the threads it
      started, `p_thread_join` clears the handle, `p_co_detach` lets a thread
      future go); test test_spawn_scope_outer_thread pass on run, C and emit.
      Left: the lowering half (BG119).
- [~] BG46. **Awaiting the same future twice gives two owners of one result**
      (double free). lower.fd `Expr::Await` (~18248); codegen_c.fd:13524.
      Confidence: medium-high.
      Partly fixed Nov 22-23 (lower.fd: a future awaited more than once gives
      each await an owner of its own); test test_await_twice_owners pass on
      run, C and emit. Left: a single await inside a loop (BG123).
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
- [~] BG60. **Comparisons are not type-checked** (beyond str vs number), and
      `1 < x < 10` parses as `(1 < x) < 10`, always true. typechecker.fd:20049-20077;
      parser.fd:6944. Confidence: high.
      Partly fixed Nov 22-23 (typechecker.fd `TypeChecker_check_comparison`:
      chained comparisons, ordering bools, a bool with a str, and two
      different structs or enums are errors); tests test_cmp_chain_fail,
      test_cmp_user_types_fail, test_cmp_ok pass on run, C and emit. Left:
      numbers of different types (`i64 < f64`, `u8 == i64`) are still
      accepted; the runtime layer and compiler/ modules are exempt.
- [~] BG61. **An integer literal of 2^64 or more wraps silently; one above
      i64::MAX fits an `i64`.** compiler/lexer.fd:1445-1455;
      typechecker.fd:14506. Confidence: high.
      Partly fixed Nov 22-23 (lexer.fd `lex_int_too_big`: a literal of 2^64 or
      more is an error); test test_int_literal_too_big_fail pass on run, C and
      emit. Left: a literal that does not fit its declared type (BG117).
- [ ] BG62. **Comptime arithmetic treats `u64` as signed.**
      comptime_eval.fd:5111-5203. Confidence: medium.
- [ ] BG63. **Inline `asm!` does not need `unsafe`.** typechecker.fd:21494-21518.
      Confidence: high.
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
      test_file_write_nul pass on run, C and emit.
- [x] BG69. **`p_process_run` is not thread-safe and truncates long commands.**
      codegen_c.fd:12781-12811. Confidence: high.
      Fixed Nov 22-23 (codegen_c.fd `p_process_run`: thread-safe, no command
      length limit); test test_process_run_threads pass on run, C and emit.
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
      told apart by sign); test test_future_negative_value pass on run, C and
      emit.
- [x] BG74. **Interpreter channels ignore their capacity; a send never waits.**
      interp.fd:11589-11641. Confidence: high.
      Fixed Nov 22-23 (interp.fd: a send waits on a full channel); test
      test_chan_send_waits_full pass on run, C and emit.
- [~] BG75. **Closures and `dyn Trait` values count as `@send`.**
      typechecker.fd:18348, 18196. Confidence: medium.
      Partly fixed Nov 22-23 (typechecker.fd: a `dyn Trait` is not @send, and
      a closure literal given to a spawned call has its captures checked);
      test test_spawn_closure_capture_fail pass on run, C and emit. Left: a
      closure held in a variable (a `fn` type) still counts as @send.
- [x] BG76. **On one worker, `task_run()` from inside a task runs a nested
      scheduler pass over the same list.** codegen_c.fd:15297-15306,
      14216-14238. Confidence: medium.
      Fixed Nov 22-23 (codegen_c.fd: `task_run()` inside a task starts no
      nested pass); test test_task_run_in_task pass on run, C and emit.
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
      whole length and report errors); test test_file_write_nul pass on run, C
      and emit.
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
- [ ] BG85. **mono name mangling can give two instantiations one name**
      (array size dropped; tuples not delimited). compiler/mono.fd:452-466.
      Confidence: medium.
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
      test_task_await_cycle_fail pass on run, C and emit.
- [x] BG97. **`p_json_parse`/`p_json_dump` turn nested objects into invalid
      JSON.** codegen_c.fd:17700-17706. Confidence: high.
      Fixed Nov 22-23 (codegen_c.fd: nested objects kept valid); test
      test_cback_json_nested pass on run, C and emit.
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

- [ ] BG108. **Critical. The mid-end SSA round trip miscompiles a loop that
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
- [~] BG110. **The native runtimes still cap threads at 256** (the C
      runtime's table is chunked since BG9; pe_runtime/linux_runtime keep a
      fixed table). Confidence: medium (from the BG9 fix's notes).
      Partly fixed Nov 22-23 (stdlib/pe_runtime/conc.fd and
      linux_runtime/conc.fd: the thread table is chunked as the C runtime's);
      tests test_native_spawn_many, test_spawn_many pass on run, C, emit and
      nat. Left: the pe_ir runtime may join a thread twice (BG123).
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

- [ ] BG115. **High. A bare `Some(v)` / `None` pattern is accepted and binds
      `v` as i64.** reference.md §(enums) says a bare `Some(5)` is an
      undefined function, but as a match pattern `Some(v)` matches the
      Option's tag and `v` gets type i64 whatever the payload is: with
      `get() -> Option<str>`, `Some(v) => println(int_to_str(v))` prints
      `some 0`, and `v == "x"` is rejected as "i64 with str". Either the
      pattern resolves to `Option::Some` with the payload's type, or it is
      an error. Seen in three new tests (test_cparser_casts,
      test_read_line_long: rewritten to `Option::Some`). Confidence: high.
- [ ] BG116. **`s.len()` on a `str` is accepted by `fid run` and rejected
      by `ccompile` and `emit`** ("cannot call method 'len' on type str").
      One program, two answers from the checker depending on the path;
      `str_len(s)` works on all. Trigger: test_pe_str_join_n before its
      Nov 23 rewrite. Confidence: high.
- [ ] BG117. **BG61's type-checker half**: the lexer rejects a literal over
      64 bits now (test_int_literal_too_big_fail), but a literal that fits
      in 64 bits and not in its declared type (`val b: u8 = 300;`) is not
      checked yet.
- [ ] BG118. **The lock-order check runs only in `fid check`**, not in
      `run`/`ccompile`/`emit`, so test_lockcheck_cycle_fail can never
      fail to compile on those paths. compiler/lockcheck.fd.
- [ ] BG119. **BG45's lowering half**: `Lowerer_lower_spawn_detached`
      detaches green tasks only, so `spawn detached f()` on a thread inside
      a scope is still joined at the scope's end. The C runtime's
      `p_co_detach` accepts a thread future now; pe_ir's and nat_ir's do
      not, so lower.fd cannot call it for threads yet.
- [ ] BG120. **BG40 leftovers**: the C runtime's `p_vec_remove`,
      `reverse`, `sort`, `pop` and `truncate` still ignore element codes and
      shared buffers (push/set/insert were fixed).
- [ ] BG121. **cparser maps `long` / `unsigned long` to i32/u32 on every
      target**: wrong on 64-bit Linux and macOS (LP64). The C++ path
      (`cpx_base_of`) maps `long double` to double: values cross the shim
      right, a `long double*` does not.
- [ ] BG122. **BG63 / BG62 need language work**: BG63 (asm outside
      `unsafe`) needs about 12 asm tests wrapped in `unsafe` and the docs
      changed with it; BG62 needs typed comptime values.
- [ ] BG123. **Small leftovers from the Nov 22 batch**: lsp.fd keeps
      `var program`, which would let the BG20 exemption for the compiler
      go; BG35's merged AST cache is not covered; bindings in an
      `inline for` body leak into the enclosing scope; `fid fmt` prints
      `impl<T>` as `impl < T >`; BG46 is fixed for repeated awaits but not
      for a single await inside a loop; BG60 and BG75 are partly done (see
      their notes); BG110's pe_ir runtime may join a thread twice.
- [ ] BG124. **`fid fmt --check` reports a CRLF file as unformatted.** On a
      Windows checkout (`core.autocrlf=true`) scripts/fmt-check.sh fails
      373 of 468 files that are formatted; the formatter writes LF, and the
      check compares bytes. Also: `fid fmt <file>` rewrites the file in
      place with no `--check`, which a probe redirecting its output
      (`fid fmt f.fd > out`) does not expect.
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

## Found and fixed while reviewing (Nov 22)

- [x] BG100. **`Int_to_str_into` (LF13 stack strings) printed garbage for
      positive numbers and returned a string with no header in front of it.**
      stdlib/core.fd: a positive `x` was never negated (`f"v = {42}"` printed
      `v = ,.`), and the digits were returned at `p + 1` while the header was
      written at `buf`. Fixed in the working tree (digits moved to `buf + 16`,
      `v` kept <= 0); checked on run, C, pe_ir and --nat with 0, -7, i64::MIN
      and i64::MAX.
