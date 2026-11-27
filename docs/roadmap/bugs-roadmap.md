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
Status Nov 27 (night run, 10:15): BG128-BG241: 97 done `[x]`, 3
partly `[~]` (BG183, BG204, BG227), 14 open; BG242 done, BG243-BG246
open; BG247 done; BG248-BG257: BG253, BG254, BG255 and BG257 done, 6
open. The whole file: 230 `[x]`, 3 `[~]`, 24 `[ ]`.
Nov 27 review: 109 new entries, BG258-BG366 (3 critical, 25 high, 46
medium, 35 low), all open; see "Found by the Nov 27 whole-repo review"
below.
Checkbox count over the whole file, Nov 27 evening: **230 `[x]`, 27
`[~]`, 232 `[ ]`** (the [~] count grew as entries were split during
triage; BG489 was added after the count).
Nov 27 second run: 122 new entries, BG367-BG488 (4 critical, 41 high,
51 medium, 26 low), all open; see "Found by the Nov 27 whole-repo
review, second run" below.

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
      Nov 27 evening batch: the default-method leftover already works on the
      Nov 27 fid (fill_default_trait_methods runs before mono);
      test_bg183_dyn_generic_default passes on run, ccompile, emit, nat. Not
      verified: the compiler with these changes was not built (stopped;
      another agent takes it over), so compiler-side fixes have not run.
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

- [x] BG201. **Bitwise and shift operators are accepted on floats.**
      typechecker.fd `Expr::Binary` arithmetic path: two numeric operands give
      `F64` with no integer requirement for `& | ^ << >>` (and `<<=`).
      Trigger: `var f: f64 = 1.5; val g = f & 2.0; println(g);` passes
      `check`, `run` prints 0 and `ccompile` fails ("C compilation failed":
      `&` on doubles); `f <<= 2` panics "Arithmetic error (float, int)" under
      `run`. reference.md §4.2's wording covers these operators too and needs
      the same correction. Confirmed Nov 24. Confidence: high.
      Nov 27 (night): the expression form is done and validated with the
      build of 06:35 (tree 2e3a15e9): test_bg201_float_bitwise_fail is
      rejected ("`&` takes integers, got f64"), test_bg201_int_bitwise and
      tests/conformance/s4_2_bitwise_float_rejected pass on run, ccompile
      and emit; reference §4.2 says so. The compound form: its first check
      (f148a589) also rejected a plain `x = ...` on a float, which the
      build of 07:10 showed on stdlib/core.fd:8662 and 30 places of the
      compiler's own source; 647b4368 makes it look at the five compound
      operators only. Validated with the build of 07:41 (16 units at -O2,
      release/fid-night.exe of 16,785,408 bytes): test_bg201_float_shift_
      assign_fail is rejected ("`<<` takes integers, got f64"),
      test_bg201_int_bitwise and tests/conformance/s4_2_bitwise_float_
      rejected pass on run, ccompile and emit, and `check compiler/main.fd`
      is clean (no errors, 26 s). 165 of the 169 related programs
      (conformance s4_2, s2_8, s7; *trait*, *generic*, *alias*) pass; the
      4 others are helper files (diagnostics/generic_call_site_lib.fd,
      trait_conflict/shape_*.fd) that fail the same with the compiler of
      Nov 25.
- [x] BG202. **`<` and `>` on two `str` values are accepted, and the backends
      disagree.** The type checker accepts an ordering of two strings, which
      reference.md §4.2 does not define (only `==`). `fid run` panics
      "Arithmetic error (str, str)"; C compares the addresses (`"abc" < "abd"`
      true, `"b" > "abc"` false). Either reject it or define it as a byte
      comparison everywhere. Confirmed Nov 24. Confidence: high.
      Fixed Nov 27 (night) (b03479b8): `< > <= >=` on two `str` are a
      type error in a program ("cannot order two str values with `<`";
      Fix: `str_compare(a, b)` gives -1, 0 or 1); reference §4.2.
      test_bg202_str_order_fail rejected, test_bg202_str_compare and
      tests/conformance/s4_2_str_order_rejected pass on run, ccompile and
      emit with the build of 06:35; 164 of the 169 related programs
      (conformance s4_2, s2_8, s7; *trait*, *generic*, *alias*) pass, the
      5 others are helper files that fail the same with the compiler of
      Nov 25.
- [~] BG203. **`Option::Some(x)` built from a `for x in v` loop variable leaks
      its payload.** lower.fd `Lowerer_lower_for` (Vec branch) /
      `Lowerer_lower_for_vec` record no AST type for the loop variable, so
      `Lowerer_enum_lit_ast_type` types the literal `Option<i64>` (owns
      nothing) and never registers it as a temporary, while the payload still
      gets a `Str_dup`. Trigger: `for x in v { total = total +
      show(Option::Some(x)); }` over a `Vec<str>`: `run --leaks` reports one
      string per iteration never freed; `return Option::Some(x)` in the loop
      leaks one. A `while` loop with `v.get(i)` frees everything. Confirmed
      Nov 24. Confidence: high.
      Nov 27 evening batch: lower.fd Expr::EnumLit
      (Lowerer_enum_lit_temp_type); test_bg203_for_var_some (leak test). Not
      verified: the compiler with these changes was not built (stopped;
      another agent takes it over), so compiler-side fixes have not run.
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
      Nov 27 evening batch: lower.fd: p_json_parse/get/at results are
      statement temporaries typed as strings (lw_json_fresh_call; the Nov 27
      reader special case removed); test_bg204_json_var_drop (leak test). Not
      verified: the compiler with these changes was not built (stopped;
      another agent takes it over), so compiler-side fixes have not run.
- [~] BG205. **C runtime `p_socket_poll` uses `select`/`FD_SET` without
      checking the fd against `FD_SETSIZE`.** codegen_c.fd `p_socket_poll`,
      POSIX branch: on Linux an fd of 1024 or more writes past the stack
      `fd_set`, so a server holding more than 1024 descriptors corrupts its
      stack on each poll (stdlib/async.fd readiness, the runtime's own
      nonblocking write loops). Fix direction: `poll()`, as the WASI branch
      does. Read only (an fd of 1500 landed in the neighbouring set, no ASan
      report). Confidence: high.
      Nov 27 evening batch: codegen_c.fd p_socket_poll uses poll() on POSIX;
      test_bg205_socket_poll_fd_range (needs a Linux C build). Not verified:
      the compiler with these changes was not built (stopped; another agent
      takes it over), so compiler-side fixes have not run.
- [x] BG206. **Linux runtime `pert_str_to_float` is not correctly rounded.**
      stdlib/linux_runtime/base.fd: up to 18 digits are packed into an i64, a
      mantissa over 2^53 is rounded by `mant as f64` and again by the
      power-of-ten multiply or divide, and exponents over 22 loop on `* 10.0`,
      rounding each step. About 10% of printed doubles do not read back
      natively: 200 of 2000 `str_parse_float(float_to_str(x)) != x` on
      x86_64-linux and `--nat` (0 under `run`), e.g. "1.7976931348623157e308"
      reads 1.7976931348623145e308. Confirmed Nov 24. Confidence: high.
      Nov 27 (night), fixed (a88c72b3): `pert_str_to_float` is exact now.
      The digits make a big integer and a power of ten; the leading 57
      bits of the quotient come from a division bit by bit, with the
      remainder and the digits not read; normals and subnormals are
      rounded to the nearest even; 0 on underflow, inf on overflow, the
      sign of a zero kept. At most 800 significant digits are read
      exactly (the most a midpoint of two doubles has is 767); a
      non-zero digit beyond them only says "a little more", which
      cannot turn a rounding around (such a text has over 490 fraction
      digits). Gate, with release/fid-v27.exe through `emit --nat`
      (tests/nattools/str_to_float_exact_check.sh; its texts and their
      bits are made by Python's float() with
      tests/nattools/gen_str_to_float_exact.py): 2,000 random doubles
      printed and read back, 0 wrong (1,989 wrong with the runtime
      before, on this machine); 1,107 texts (the ends of the range,
      zero and its sign, midpoints and one digit either side,
      subnormals, decimals of 1 to 25 and of 17 digits over the whole
      exponent range, long integers, texts with over 800 significant
      digits) all as Python's float() (751 differed before). The 15
      float, f-string, decimal and JSON tests of tests/bootstrap pass
      on the native backend. This runtime is the machine layer's:
      `run`, ccompile and pe_ir do not go through it.
- [~] BG207. **`--nat` Windows console input: a lone CR read is reported as
      end of input.** stdlib/linux_runtime/win.fd `pe_win_read` strips CR
      bytes on a console handle and returns the count left; `pert_read_line`
      reads one byte per call, so the CR of "abc\r\n" comes back as a 0-byte
      read, read_line sets the EOF flag after the first line, and the next
      call returns an empty line. Read only (needs a real console).
      Confidence: medium.
      Nov 27 (night): not tried here: stdin is redirected in every
      check on this machine, so a fix could not be validated against
      the console it is about (goal-workflow.md row 47).
      Nov 27 evening batch: linux_runtime/win.fd pe_win_read reads again when
      only CRs are left (pe_win_drop_cr);
      tests/nattools/bg207_console_read_check.sh PASS with the Nov 27 fid. Not
      verified: the compiler with these changes was not built (stopped;
      another agent takes it over), so compiler-side fixes have not run.
- [x] BG208. **disasm: `vzeroupper` (VEX `C5 F8 77`) swallows the next
      instruction.** disasm.fd, VEX branch of the x86 decoder: with no table
      row, the length assumes a ModRM byte follows, which `0F 77` does not
      have. Evidence: `test_simd_native.fd` built `--target x86_64-linux --cpu
      x86-64-v3` disassembles `c5 f8 77 4d 39` as `.byte 0xc5 (vex)` and loses
      `cmp r12, r14`; with nop padding it swallowed 5 bytes. Affects `fid
      disasm`, `explain` and the disasm checks. Confirmed Nov 24. Confidence:
      high.
      Fixed Nov 27 (night): the VEX branch reads 0F 77 itself (`vzeroupper`,
      `vzeroall` by the L bit), three bytes, before the row lookup that took
      the next byte for a ModRM. Check tests/tooling/disasm-forms-check.sh
      (`c5 f8 77 4d 39 f4` is `vzeroupper` then `cmp r12, r14`; `c5 fc 77` is
      `vzeroall`): fails both with the compiler of Nov 25, passes with the
      one built from this tree (7b06ce65).
- [~] BG209. **F-string: an escaped brace becomes an interpolation.** lexer.fd
      `read_fstring` decodes `\xHH`/`\u{..}` outside braces into the raw text,
      and parser.fd `parse_fstring_parts` then takes that `{` as a part start.
      Trigger: `val a = 42; println("\x7Ba\x7D"); println(f"brace \x7Ba\x7D
      end");` prints `{a}` then `brace 42 end` (same with `\u{7b}`): the only
      way to write a literal brace silently changes meaning. Confirmed Nov 24.
      Confidence: high.
      Nov 27 evening batch: lexer.fd read_fstring keeps a decoded brace as `\`
      + byte, parser.fd reads it as text; test_bg209_fstring_escaped_brace.
      Not verified: the compiler with these changes was not built (stopped;
      another agent takes it over), so compiler-side fixes have not run.
- [~] BG210. **F-string: sub-lexer errors are dropped, and `:` or `}` inside a
      char literal is mis-split.** parser.fd `parse_fstring_parts` never reads
      `sub_lexer.tokenize()` errors and its `:` scan tracks `"` but not `'`;
      lexer.fd `read_fstring` counts braces without skipping char literals.
      Triggers: `f"{c == ':'}"` with `c = 58` prints `false`; `f"{c == '}'}"`
      prints `false'}`; `f"{a + '}"` prints 7 (want an error); `f"{"\q"}"` is
      accepted though a plain string rejects `\q`. Confirmed Nov 24.
      Confidence: high.
      Nov 27 evening batch: lexer.fd lex_fstr_expr_end skips char literals;
      parser.fd reports sub-lexer errors (parse_fstr_lex_fix);
      test_bg210_fstring_char_literal, two _fail tests. Not verified: the
      compiler with these changes was not built (stopped; another agent takes
      it over), so compiler-side fixes have not run.
- [~] BG211. **F-string: a `}` or `{` inside a string literal within the
      braces breaks lexing.** lexer.fd `read_fstring`'s brace loop (and
      `parse_fstring_parts`) does not skip nested string literals. Trigger:
      `println(f"{str_len("a}c")}");` gives "unterminated string" on a later
      line; `"a{c"` gives "unterminated f-string": a valid program is
      rejected. BG101 fixed only fmt.fd. Confirmed Nov 24. Confidence: high.
      Nov 27 evening batch: lexer.fd lex_fstr_expr_end shared by lexer, parser
      and fmt.fd; test_bg211_fstring_string_brace, tests/fmt/fstring_braces*;
      reference.md 1.8 updated. Not verified: the compiler with these changes
      was not built (stopped; another agent takes it over), so compiler-side
      fixes have not run.
- [x] BG212. **Comptime does not evaluate float/int casts, so arithmetic on
      them fails.** comptime_eval.fd `evaluate_with_env`, `Expr::Cast`: a cast
      of a non-int value returns `Cast(v, ty)` unchanged and an int-to-f64
      cast stays a `Cast` node. Triggers: `comptime fn avg(a: i64, b: i64) ->
      f64 { return (a + b) as f64 / 2.0; }` with `comptime avg(3, 4)` ("cannot
      evaluate binop on non-literal"), `comptime (-(7.9 as i64))` ("cannot
      negate non-number"); reference §4.11 makes both fatal. Confirmed Nov 24.
      Confidence: high.
      Fixed Nov 27 (night): `evaluate_with_env`'s `Expr::Cast` makes an
      integer to a float a `FloatLit`, and a float to an integer the
      truncated value (toward zero, as at run time; unsigned targets tag it;
      a float out of the integer's range is "comptime cast: ... does not fit
      in ...", as comptime overflow is); a float to a float is itself.
      Tests test_bg212_comptime_cast (both triggers, 7.9 and -7.9 to i64, a
      fraction, an unsigned target, a float to a float) and
      test_bg212_comptime_cast_range_fail: pass on run, ccompile and emit
      with the compiler built from this tree (7b06ce65); the f-string,
      lifetime, borrow and comptime programs already in the tree pass with
      it as with the compiler of Nov 25 (321 of 324; the three fail with
      both: test_frontend_comptime_if_assign, s4_11_comptime_if_generic and
      s8_2_null_deref_panics).
- [~] BG213. **C header parser evaluates unsigned macro arithmetic as
      signed.** cparser.fd `CpEval`/`CpExpr` drop the `u`/`U` suffix and
      unsigned operands: `#define UDIV (-1u / 2)` gives 0 (C: 2147483647),
      `UNEG (-1u)` -1 (C: 4294967295), `MASK (~0u)` -1 (C: 4294967295). Also
      dropped with no note: `COND (A > 5 ? 1 : 2)`, `CMP (3 < 2)`, `0b1010`
      literals. Confirmed Nov 24 (`fid parse-header`). Confidence: high.
      Nov 27 evening batch: cparser.fd typed evaluation (uns/wide, common
      type, ?:, comparisons, #if in 64 bits); test_bg213_cparser_unsigned
      passes on run, ccompile, emit (it imports compiler::cparser from
      source). Not verified: the compiler with these changes was not built
      (stopped; another agent takes it over), so compiler-side fixes have not
      run.
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
- [x] BG218. **Two `impl Show for P` in one file are accepted; the later one
      wins.** The CL7 conflict check (compiler.fd) compares only modules.
      Confirmed Nov 24. Confidence: high.
      Fixed Nov 27 (night) (9ec4af52, compiler.fd
      `cc_find_trait_impl_conflicts`): a trait impl defined twice in the
      user program, or twice in one imported module, is an error ("is
      defined twice in this program"). test_bg218_duplicate_impl_fail is
      rejected with the build of 06:35; the trait_conflict tests of the
      related run are as before.
- [x] BG219. **A generic function with a bound taken as a value skips the
      bound and fails at run time.** `val f = need; f(5)` with `fn need<T:
      A>(x: T)` is accepted and panics "CallPtr: null function value" (C exits
      127). Confirmed Nov 24. Confidence: high.
      Fixed Nov 27 (night) (f0973224): a generic function taken as a
      value is a type error ("the generic function 'need' cannot be taken
      as a value"). test_bg219_generic_fn_value_fail rejected,
      test_bg219_generic_fn_call passes on run, ccompile and emit with the
      build of 06:35.
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
      Later on Nov 27 (night): the whole WebAssembly suite showed the
      test overflowing the stack there before its first line: the XML
      parser's frames overflow the WebAssembly stack at 64 levels (48
      parse). The XML limit follows the backend (`xml_max_depth`: 48 on
      WebAssembly, XML_MAX_DEPTH elsewhere); TOML parses 128 arrays and
      64 inline tables deep on WebAssembly, so its limit stays
      (`toml_max_depth`). The test prints what holds on every backend and
      passes on wasm too (scripts/wasm-test.sh on it: 1 passed).
- [~] BG225. **The interpreter's mutex does not exclude between tasks
      either.** `fid run` prints 5 (want 15) for the same three-holder probe
      as the C runtime mutex entry: a task holding `p_mutex_lock` across
      `task_sleep` lets the others in. Confirmed Nov 24. Confidence: high.
      Nov 27 evening batch: interp.fd p_mutex_lock waits for the holder
      (Interpreter_mutex_held); test_bg225_interp_mutex_tasks. Not verified:
      the compiler with these changes was not built (stopped; another agent
      takes it over), so compiler-side fixes have not run.
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
      Nov 27 evening batch: pe_runtime/conc.fd FIDES_TEST_WAKE_DELAY hook;
      test_bg227_wake_delay fails 3/3 with the old done-flag read put back,
      passes with the fix (emit, checked on the Nov 27 fid since the runtime
      is read at emit time). Not verified: the compiler with these changes was
      not built (stopped; another agent takes it over), so compiler-side fixes
      have not run.
- [~] BG228. **Every owned argument of an async-task call leaks.** lower.fd,
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
      Nov 27 evening batch: lower.fd task word adapter drops owned args
      (Lowerer_task_owned_param, Lowerer_emit_task_adapter);
      test_bg228_task_arg_owners (leak test). Not verified: the compiler with
      these changes was not built (stopped; another agent takes it over), so
      compiler-side fixes have not run.
- [~] BG229. **Interpreter: each `txn` keeps a full copy of interpreter
      memory.** interp.fd `Inst::TxnBegin` pushes
      `Memory_clone(&self.memory)`, which the lowering never uses (it rolls
      back itself, `Lowerer_txn_abort`), and memory still grows by about one
      copy per txn entered, even after `TxnEnd`; `break`/`continue`/`return`
      out of a txn skip `TxnEnd`. Trigger: a 20,000-element Vec, then `while i
      < N { txn { n = n + 1; } }`: peak 43 MB without txn, 228 MB at N=30,
      2,277 MB at N=300 under `fid run`; C is unaffected. Confirmed Nov 24.
      Confidence: high (growth).
      Nov 27 evening batch: interp.fd txn keeps a write log instead of
      Memory_clone (Memory_put, Interpreter_txn_rollback; debugger.fd
      follows); test_bg229_txn_memory. Not verified: the compiler with these
      changes was not built (stopped; another agent takes it over), so
      compiler-side fixes have not run.
### Low

- [~] BG230. **`opt ?? <new value>` leaks the default when the result is
      kept.** lower.fd `Expr::Coalesce` in `Lowerer_lower_expr`: on the None
      path the default is claimed, but the merged result is a plain load, so
      `val r = ...` duplicates it and the default's owner is never dropped
      (the comment says it is "left to leak rather than drop a borrowed Some
      value"). Trigger: `val r2 = get(false) ?? ("d" + int_to_str(9));`: `run
      --leaks` reports 1 block never freed. Confirmed Nov 24. Confidence:
      high.
      Nov 27 evening batch: lower.fd Expr::Coalesce owns both paths
      (Lowerer_coalesce_own_type); test_bg230_coalesce_default_owner (leak
      test). Not verified: the compiler with these changes was not built
      (stopped; another agent takes it over), so compiler-side fixes have not
      run.
- [x] BG231. **C runtime `p_read_stdin(-1)` writes into a 0-byte block.**
      codegen_c.fd `p_read_stdin__raw` calls `malloc(n + 1)` = `malloc(0)`,
      skips the read loop and writes `buf[0] = 0`; other negative `n` are
      caught only through the NULL return. Read only. Confidence: medium.
      Nov 27 (night): `p_read_stdin__raw` returns NULL for a negative
      `n` (cb8f8b69, codegen_c.fd). Validated with the build of 07:41
      (release/fid-night.exe): test_bg231_read_stdin_negative
      (`//@ backends: ccompile`; its marker stood inside the expected
      block until 5c86e3dd) passes, and the program compiled once more
      by hand prints `true` with stdin closed. The interpreter and the
      native backends are not changed: the entry is about the C runtime.
- [x] BG232. **disasm has no row for several instructions the encoders emit.**
      disasm.fd encoding table: x86 `blsmsk`/`blsi` (TG7; only `blsr` has a
      row); AArch64 pre/post-indexed `ldr`/`str` (TG8, e.g. `f8008528`), FP
      `ldr/str d`, `ldp/stp d8, d9`, `bic v.16b`, `dup v.2d, v.d[i]`, `mov d,
      v.d[i]`; RISC-V `vsll.vi`, `vmseq.vi`, `vmerge.vim`, `vslidedown.vi`,
      `vfmv.f.s`, `vfmv.v.f` (TG11). They print as `.byte`/`.word` (length
      kept). docs/file-analysis/disasm.md says each encoder has a row.
      Confirmed Nov 24. Confidence: high.
      Nov 27 (night), fixed (48a42687): rows for x86 `blsmsk` and
      `blsi` (beside `blsr`); the AArch64 pre/post-index `ldr`, `str`,
      `ldrsw`, `ldrh`, `strh`, `ldrsh`, `ldrb`, `strb`, `ldrsb` (24
      rows, with a signed 9-bit offset field), the SIMD&FP `ldr`/`str`
      d and s and `ldp`/`stp` d, `bic v.16b`, `dup v.2d, v.d[i]` and
      `mov d, v.d[i]`; the RISC-V `vsll.vi`, `vmseq.vi`, `vmerge.vim`,
      `vslidedown.vi`, `vfmv.f.s`, `vfmv.v.f` (two 5-bit immediate
      fields). Every byte pattern is llvm-mc's (clang 21) for the text
      expected. Validated with the build of 09:17 (16 units at -O2,
      release/fid-night.exe): tests/tooling/disasm-forms-check.sh 30 of
      30 (23 of them BG232; the build of 08:47 fails 21 of them);
      tests/nattools/enc_table_unit.fd, which now lists the encoders
      that had no row and the words nat_ir.fd writes itself: table ok,
      a64 ok, rv64 ok, x64 ok on run, ccompile and emit, and with the
      tree's source; scripts/disasm-check.sh: 18 ok, and test_machine_fast on
      AArch64 has 0 words undecoded where LLVM decodes 2,957 (8
      before).
- [x] BG233. **disasm one-byte forms ignore the 0x66 operand-size prefix and
      REX.B.** disasm.fd one-byte opcode cases: `66 B8 34 12` decodes as `mov
      eax, 0x90901234` (imm32, wrong length; want `mov ax, 0x1234`), `66 99`
      as `cdq` (want `cwd`), `49 90` as `nop` (want `xchg r8, rax`), `F3 90`
      as `nop` (want `pause`). The compiler does not emit these; `disasm
      --raw` and `asm!` output can contain them. Confirmed Nov 24. Confidence:
      high.
      Fixed Nov 27 (night): the register field of `mov r, imm` and its `z`
      immediate follow the 0x66 prefix (16-bit, two bytes); `99` is `cwd`
      under 0x66; `90` is `pause` after F3 and `xchg r8, rax` with REX.B.
      Check tests/tooling/disasm-forms-check.sh (the four triggers, each
      with its length, and what they are without the prefix; 7 checks):
      fails 6 of 7 with the compiler of Nov 25, passes all 7 with the one
      built from this tree (7b06ce65).
- [~] BG234. **The ELF `.text` section header spans code, data and the RW
      segment.** nat_enc.fd `nat_elf_debug_sections` writes one `.text` header
      flagged WAX over the whole image even after nat_elf_img splits it into
      R-E and RW PT_LOADs, so `readelf -l` maps `.text` to no segment and
      `objdump -d` disassembles data as code. Tooling only. Confirmed Nov 24.
      Confidence: high.
      Nov 27 evening batch: nat_enc.fd nat_elf_debug_sections writes .text
      (AX) and .data (WA) over the two PT_LOADs; scripts/image-lint.py checks
      it (self-test case added). Not verified: the compiler with these changes
      was not built (stopped; another agent takes it over), so compiler-side
      fixes have not run.
- [~] BG235. **Lock order: locks taken inside a closure body are not
      checked.** lockcheck.fd `lockcheck_module` and the type checker's
      `lo_summary` add no edges for a call through a closure value. Trigger:
      `main` takes `a` then `b`, a closure `f` takes `b` then `a`, `f()`
      called: `check` passes; the same code as a named or spawned function is
      rejected. A real deadlock only when the closure runs on another thread.
      Confirmed Nov 24. Confidence: high.
      Nov 27 evening batch: typechecker.fd walks closure bodies in the
      enclosing lock names; lockcheck.fd lc_callptr_target;
      test_bg235_closure_lock_order_fail, _call_held_fail, _ok (the
      type-checker part checked by running its sources with fid run). Not
      verified: the compiler with these changes was not built (stopped;
      another agent takes it over), so compiler-side fixes have not run.
- [x] BG236. **mono: function-type mangling is ambiguous.** mono.fd
      `mangle_type`, `Type::Func`: `"fn_" + params + "_" + ret` with no count,
      so `fn(fn(i64) -> f64, i64) -> i64` and `fn(fn(i64, f64) -> i64) -> i64`
      both become `id__fn_fn_i64_f64_i64_i64` (one instance for two `T`). No
      wrong output today (every function value is one word); BG85 gave tuples
      and arrays a count but not `Func`. Confirmed Nov 24 (dump-ir).
      Confidence: high.
      Fixed Nov 27 (night) (0fcd0261): each part of a function type
      carries its length (`fn_2_16_fn_1_3_i64_3_f64_3_i64_3_i64` and
      `fn_1_22_fn_2_3_i64_3_f64_3_i64_3_i64`). Checked directly on
      `mangle_type`: tests/nattools/mangle_func_unit.fd, which passes
      through `fid run` (it runs the tree's own mono.fd) with the
      compilers of Nov 25 and Nov 27. tests/selfhost/test_mono.fd no
      longer compiles for reasons of its own (BG253), so its mangle lines
      moved there (ac4ae268). With the build of 07:41 that unit test
      passes on run, ccompile and emit.
- [x] BG237. **`fid fmt --check file.fd` (flag before the file) reads the flag
      as the file name** ("could not read file: --check", exit 1); `fid fmt
      file.fd --check` works. Confirmed Nov 24. Confidence: high.
      Fixed Nov 27 (night) (b03479b8): `cmd_fmt` takes the file wherever
      it stands among the flags. tests/tooling/fmt-flag-order-check.sh:
      6 of 6 with the build of 06:35.
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
- [x] BG240. **A cycle of type aliases overflows the checker's stack.** `type
      A = B; type B = A;` recurses in `TypeChecker_resolve_type` until a stack
      overflow instead of an error. Confirmed Nov 24. Confidence: high.
      Fixed Nov 27 (night) (b03479b8): `TypeChecker_resolve_type` follows
      aliases in a loop, and a cycle is an error where the aliases are
      registered ("type alias 'A' resolves to itself through 1 alias(es):
      a cycle of type aliases"); reference §2.8. test_bg240_alias_cycle_
      fail and test_bg240_alias_cycle_three_fail rejected,
      test_bg240_alias_chain and tests/conformance/s2_8_alias_cycle_
      rejected pass on run, ccompile and emit with the build of 06:35. An
      alias of a reference type is still rejected: BG252.
- [~] BG241. **`select` drops a received value equal to its "not ready"
      sentinel.** lower.fd `Lowerer_lower_select_stmt` uses the in-band value
      -9223372036854775807 for "nothing ready"; a received value equal to it
      has already been taken from the channel and is treated as nothing.
      Trigger: send -9223372036854775807, then 5: the first `select` prints
      "nothing ready" on run and C and the value is gone. Confirmed Nov 24.
      Confidence: high.
      Nov 27 evening batch: lower.fd select uses p_chan_try_recv_into
      (LOWER_SELECT_READY_OUT), added to the C, pe and linux runtimes and
      interp.fd; test_bg241_select_sentinel_value. Not verified: the compiler
      with these changes was not built (stopped; another agent takes it over),
      so compiler-side fixes have not run.
## Found while fixing the Nov 25 batch (TL2/TL4/TL5, the six bugs)

- [x] BG242. **The type checker accepts a `str` for an `i64` parameter.**
      Seen at a `str_find_char(s, "x")` call (the second parameter is the
      character as `i64`): `fid check` gives no error and the call passes the
      string's address. Found Nov 25 while adding TL2 hints. Confidence:
      medium (the exact rule that lets it through is not located yet).
      Nov 27 (night): no longer reproduces. A `str` for an `i64`
      parameter is an error ("function 'take' parameter 1 expects 'i64',
      got 'str'"): test_bg242_str_for_int_param_fail, rejected with the
      build of 06:35 and with the compiler of Nov 25. The entry's
      `str_find_char` no longer exists in the library. Which change
      closed it was not looked up.
- [~] BG243. **`tests/tooling/dbg_decl_lines_check` fails: an inlined
      function has no DWARF subprogram**, so its declaration line is missing
      from the debug info. Found Nov 25 (TL5). Same cause on pe_ir: `fid emit
      -g` without `--debug` inlines `scale`/`area` in test_pe_dbg_vars.fd,
      so lines 29, 30 and 35 have no `.debug_line` row and a breakpoint
      there is never hit (pe_dbg_vars_check.sh: 10 passed, 1 failed, the
      same with the Nov 25 stage 0, so not new). `--debug -g` passes every
      check (25/25 values through the DWARF, 22/22 through the PDB).
      Confidence: high.
      Nov 27 evening batch: iropt.fd records inline sites (g_iro_inl_sites);
      pe_ir.fd nops with line rows for folded inlined lines,
      DW_TAG_inlined_subroutine (pe_ir_enc.fd/nat_enc.fd abbrevs 21-22);
      FID_PE_OFF=dbginl; pe_dbg_vars_check.sh extended. Left: nat_ir and
      inline2 do not record inlines; no PDB S_INLINESITE. Not verified: the
      compiler with these changes was not built (stopped; another agent takes
      it over), so compiler-side fixes have not run.
- [~] BG244. **`ccompile -g` names C variables `v0`..`vN`**, so a C debugger
      (gdb, lldb, Visual Studio on the C build) shows IR value numbers, not
      the Fides names. pe_ir's PDB and nat_ir's DWARF carry the names (TL5).
      Found Nov 25. Confidence: high.
      Nov 27 evening batch: codegen_c.fd cg_dbg_c_names names C locals after
      the Fides variables under -g only; tests/tooling/c-dbg-names-check.sh.
      Not verified: the compiler with these changes was not built (stopped;
      another agent takes it over), so compiler-side fixes have not run.
- [~] BG245. **nat_ir's DWARF location lists cover only one-byte ranges**:
      a variable is visible at the instruction that defines it and not over
      the range it lives in, so `p x` in gdb on a Linux native build often
      says `<optimized out>`. Found Nov 25 (TL5). Confidence: medium.
      Nov 27 evening batch: nat_ir.fd ng_dbg_live_ranges keeps a register
      value covered until the register may be written (ng_dbg_clobbers);
      FID_NAT_OFF=dbghold; tests/nattools/dbg_hold_check.py. Not verified: the
      compiler with these changes was not built (stopped; another agent takes
      it over), so compiler-side fixes have not run.
- [~] BG246. **A call through a function pointer (`CallPtr`) does not
      convert an `f64` argument to an `f32` parameter** on pe_ir: the direct
      call path does since the Nov 25 f32 fix (`peir_callee_params`), the
      indirect one has no parameter types to look at. Found Nov 25.
      Confidence: medium.
      Nov 27 evening batch: lower.fd Lowerer_fnval_float_args converts f64/f32
      per parameter of a function value call; test_bg246_fnptr_f32_arg. Not
      verified: the compiler with these changes was not built (stopped;
      another agent takes it over), so compiler-side fixes have not run.
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
- [~] BG248. **`--nat` Windows: a call through a function value made from a
      C function's address returns 0.** test_fix_fnval_c_ptr expects 7, 12
      and 5 and says so in its header; pe_ir prints it, `emit --nat` prints
      0 for the first. (The interpreter cannot run the test: native FFI.)
      Found Nov 27 by the whole-suite comparison of MI5; seen twice. Cause
      not located. Confidence: high.
      Nov 27 evening batch: nat_ir.fd ng_gen_call_std: program functions
      handed to a pert_c_* stand-in get the C entry adapter
      (ng_c_over_takes_cbx); test_fix_fnval_c_ptr. Not verified: the compiler
      with these changes was not built (stopped; another agent takes it over),
      so compiler-side fixes have not run.
- [~] BG249. **`--nat` Windows runs out of memory on bench_sleepers** where
      pe_ir runs it: "Panic: out of memory (327680 bytes requested)" in
      main. Found Nov 27 by the same comparison; seen twice. Cause not
      located (the heap does not grow as pe_ir's does, or the program asks
      past its limit). Confidence: high.
      Nov 27 (night): the cause is located. A task's stack is one
      mapping of PE_TASK_STACK + PE_GUARD = 327,680 bytes (conc.fd
      `pe_task_stack_new`), and on Windows the runtime's `mmap` is
      VirtualAlloc with MEM_COMMIT: each stack is charged in full as it
      is made. The 100,000 sleepers ask for 32.8 GB of commit at once,
      and the next stack is refused. Seen with release/fid-v27.exe
      through `emit --nat`: 5,000, 20,000, 40,000, 55,000 and 70,000
      tasks run (70,000 is 22.9 GB), 100,000 panics with "327680 bytes
      requested", the size of one stack. On Linux the same mapping is
      filled only as it is used. Fix, not written: on Windows reserve a
      task stack and commit its top part only; the guard handler of
      win.fd commits a reserved page that faults and still reports the
      overflow at the guard page. It needs a hook from
      `pe_task_stack_new` (conc.fd, shared with Linux) and the task
      tests on the native backend: goal-workflow.md row 46.
      Nov 27 (night), tried for 25 minutes; not validated, nothing
      kept. Written and run through `emit --nat`: the mapping reserved
      with its top 20 KB committed (8 KB, a guard page, 8 KB under it,
      for the exception to be delivered on), a flag win.fd's mmap knows
      (asked for by `pe_task_stack_new` where win.fd sets a global),
      and `pe_win_grow_stack` in the exception handler (64 KB more at
      a time, guard page and band under it; the stack's end one step
      above the guard region, reported as an overflow). What the system
      does with it: it does not grow such a stack itself (its
      STACK_OVERFLOW is raised for the guard page, whatever StackLimit
      the TEB has), and while the TEB describes the task's stack a
      guard page set by the handler is never raised again (a task
      recursed on through it to reserved pages and died). With the
      TEB's three stack words left 0 for such tasks the guard pages
      raise plain exceptions: growth followed step by step (depth 2400
      to 7000) in a build with trace prints in the handler, and failed
      at the first or second step in builds without them (depth 300
      and 1000 ran, 2400 and more did not), with caches off as well.
      Not understood; the trace calls themselves take about 70 KB of
      the stack they report on. The experiment is in build/tmp/bg249/
      (conc.lazy.fd, win.lazy.fd, g2.template.fd). Next: a debugger
      (cdb or WinDbg) or a note in memory instead of prints; then the
      task tests on the native backend.
      Nov 27 evening batch: linux_runtime conc.fd/win.fd: task stacks reserved
      with a guard page and grown by the system (PE_MMAP_TASK_STACK,
      pe_win_task_stack); bench_sleepers and test_bg249_task_stack_grow pass
      on nat with the Nov 27 fid (runtime read at emit time). Not verified:
      the compiler with these changes was not built (stopped; another agent
      takes it over), so compiler-side fixes have not run.
- [~] BG250. **pe_ir hangs on test_read_line_long.** The program runs
      itself with a file of long lines on its standard input (BG41): the
      pe_ir image printed one line and did not finish in 60 s, twice, and
      again by hand with stdin closed; the `--nat` image and the
      interpreter finish. Found Nov 27 by the same comparison. Cause not
      located. Confidence: high.
      Nov 27 evening batch: pe_runtime/base.fd pert_read_line returns null at
      end of input, grows its buffer, frees it; test_read_line_long passes on
      ccompile, emit, nat with the Nov 27 fid. Not verified: the compiler with
      these changes was not built (stopped; another agent takes it over), so
      compiler-side fixes have not run.
- [~] BG251. **The machine layer on Windows lacks five things pe_ir has**,
      so as many suite programs build only through pe_ir: `p_str_join_n`
      (test_pe_str_join_n), `p_inb`/`p_outb` (test_std_x86_cpu), the C
      imports `GetSystemInfo` (test_lib_sys) and `pledge_asset_load_mesh`
      (test_lib_asset), and the bare-metal builtins `p_serial_init`,
      `p_hlt` (test_kernel_hello): "not available in native Windows
      executables yet" or "undefined symbol". Found Nov 27 by the same
      comparison. Confidence: high.
      Nov 27 evening batch: linux_runtime base.fd/sys.fd/win.fd:
      pert_str_join_n, port I/O and serial stubs, pledge_asset,
      GetSystemInfo/GlobalMemoryStatusEx/GetCurrentProcessId; the five
      programs build on nat with the Nov 27 fid. Not verified: the compiler
      with these changes was not built (stopped; another agent takes it over),
      so compiler-side fixes have not run.
- [~] BG252. **An alias of a reference type cannot be dereferenced.**
      `struct A { x: i64 }` `type Ref = &A;` `fn f(r: Ref) -> i64 { return
      *r.x; }` is rejected ("cannot dereference non-reference type Ref"),
      with the compiler of Nov 25 as with that of Nov 27 (night). Found
      Nov 27 (night) while writing BG240's test, which keeps to aliases of
      value types. Confidence: high.
      Nov 27 evening batch: typechecker.fd resolves the alias in Deref,
      DerefAssign, FieldAssign, check_shared_change; escape.fd
      g_esc_ref_aliases; test_bg252_ref_alias_deref, _shared_fail. Not
      verified: the compiler with these changes was not built (stopped;
      another agent takes it over), so compiler-side fixes have not run.
- [x] BG253. **tests/selfhost/test_mono.fd no longer compiles** ("no
      method 'len' on struct 'GenericFuncMap'"), with the compiler of Nov
      25 as with that of Nov 27 (night): the monomorphizer's own test
      checks nothing. Found Nov 27 (night) through BG236. Confidence:
      high.
      Nov 27 (night), fixed: the two maps are structs with a name index
      since SP10 (GenericFuncMap, GenericMethodMap: `entries` and
      `index`, filled through `GenericFuncMap_push` and
      `GenericMethodMap_push`); the test built them as plain Vecs and
      asked them for `len`. With release/fid-v27.exe the test passes
      on run (3 s), ccompile and emit: "All tests passed!". The
      mangling checks it once had are in tests/nattools/
      mangle_func_unit.fd (BG236).
- [x] BG254. **`use transform3d;` does not compile: a parse error.**
      stdlib/transform3d.fd 146:12 (`m00: r.m00,` on the line after
      `return mat4 {`): "unexpected token 'Colon'". Through it
      scripts/check-stdlib.sh fails for transform3d, lidar and pointcloud,
      with the compiler of Nov 25 as with that of Nov 27 (night). Found
      Nov 27 (night) by that check (390 of 403 modules build on their
      own). Confidence: high.
      Nov 27 (night), fixed in the library (1ae02075): the cause is the
      parser's, BG257 (a literal of a struct of another module parses
      only while its name begins with a capital, and math's is `mat4`).
      tf3_to_mat4 starts from mat4::identity() and sets its fields.
      transform3d, lidar and pointcloud build on their own with the
      compiler of Nov 25 and with that of 07:41. Test
      test_bg254_transform3d (the three modules, the identity and a
      translation, and back): passes on run, ccompile and emit with
      both compilers.
- [x] BG255. **`use quic;` does not compile: a `@secret` error in the
      library's own code.** stdlib/quic.fd 359, `quic_updated_keys`: "a
      secret value is passed to `quic_keys_from_secret`, which outputs
      its parameter 'aead'". Through it scripts/check-stdlib.sh fails for
      quic, quic_conn, quic_frame, quic_tls, quic_udp, http3 and
      http3_udp, with the compiler of Nov 25 as with that of Nov 27
      (night). Found Nov 27 (night) by that check. Confidence: high
      (which side is wrong, the library or the check, is not decided).
      Nov 27 (night), fixed in the library (dbb3347c, aea3663b): the
      library was wrong. The AEAD id is public, but it was read out of
      `old`, which is secret as a whole, and so reached
      `quic_keys_from_secret`'s `aead`, a parameter the callee's
      conditions depend on (the checker counts that as an output).
      `quic_updated_keys` takes the id apart, as a third parameter; its
      three call sites in quic_conn.fd pass quic_tls_suite_aead(c.tls.
      suite), and test_stdlib_quic_protect's RFC 9001 A.5 sample calls
      it so. quic, quic_conn, quic_frame, quic_tls, quic_udp, http3 and
      http3_udp build on their own with the compiler of Nov 25 and with
      that of 07:41. The five QUIC and HTTP/3 tests (test_stdlib_quic_
      conn, _frames, _more, _protect and test_stdlib_http3) pass on
      ccompile and emit with both; none compiled before. New test
      test_bg255_quic_key_update (the header-protection key kept, key
      and IV from the next secret): passes on run, ccompile and emit
      with both.
- [~] BG256. **`use parallel;` does not compile on its own: a
      thread-safety error.** stdlib/parallel.fd 138: "argument 5 of the
      spawned call has type fn(i64) -> i64, which is not @send". Through
      it scripts/check-stdlib.sh fails, with the compiler of Nov 25 as
      with that of Nov 27 (night). Found Nov 27 (night) by that check.
      Confidence: high.
      Nov 27 (night): not fixed; it needs a decision about the language.
      A function value is not @send, as a spawn argument and as a
      variable the spawn expression uses ("spawn captures variable of
      type fn(i64) -> i64 which is not @send"). The cast `f as i64`
      gives no address to call (4194304 in `fid run`), and `fw as
      fn(i64) -> i64` is the C function pointer of `fid ccompile`:
      called, it is "Invalid memory access" on every compiled backend.
      So the workers cannot be given `f`: par_map, par_for_each,
      par_reduce and par_sum_by need a function value without captures
      to be sendable (a type or an annotation the checker can prove),
      or another way to hand a task its code. test_parallel and
      test_parallel_concurrent are among the 20 programs the checker
      rejects on every backend through this module.
      Nov 27 evening batch: stdlib/parallel.fd:
      par_map/par_for_each/par_reduce/par_sum_by run sequentially (the @send
      check is right); test_bg256_use_parallel passes on run, ccompile, emit
      with the Nov 27 fid. Not verified: the compiler with these changes was
      not built (stopped; another agent takes it over), so compiler-side fixes
      have not run.
- [x] BG257. **A literal of a struct of another module does not parse
      while its name begins with a lowercase letter.** `use math;`,
      then in a function `return vec2 { x: 1.0, y: 2.0 };`: "Parse
      error: unexpected token 'Colon'"; the same with the first field
      on the next line, and with `val v = vec2 { .. }`. A capitalised
      name (`S { .. }`, `Transform3 { .. }`) parses. Cause: parser.fd
      `pd_field_init_follows` accepts `{ ident :` only after a capital
      (parser.fd:8337 and 8904), and a module is parsed without the
      struct names of its imports (compiler.fd:3606, empty names; the
      main file is given core's only, compiler.fd:5464). Found Nov 27
      (night) through BG254; the compiler of Nov 25 behaves the same.
      Confidence: high. Fix directions: give a module's parser the
      struct names of its imports (a token scan of their sources), or
      let `{ ident :` open a literal after any name (no block starts
      with `ident :`); then the whole suite. math's lowercase structs
      (vec2, vec3, vec4, mat4, quat) are the ones most met.
      Nov 27 (night), fixed (e1c82060): `pd_field_init_follows` no
      longer asks for a capital: `{ ident :` after any name opens the
      literal, as the comment at its call site already said (no block
      starts with `ident :`). Validated with the build of 08:47 (16
      units at -O2, release/fid-night.exe): test_bg257_lowercase_
      struct_lit (math's `vec2` after `return`, on the next line and
      on the same line, and after `val`; `mat4`; a loop and a condition
      on a lowercase variable still take a block) passes on run,
      ccompile and emit; `check compiler/main.fd` is clean (28 s); the
      front end's verdict on all 3,724 files of tests/bootstrap and
      tests/conformance is the build of 07:41's, file for file (2,724
      accepted, 1,000 rejected, 0 differ: build/verify-nov27/
      verdicts.log, the scripts frontend-verdicts.sh and
      frontend-verdicts-diff.sh); scripts/check-stdlib.sh with the
      build of 09:17: 400 compiled, 1 failed. stdlib/transform3d.fd keeps
      the form BG254 gave it: the compiler of Nov 25 must build it.

## Found and fixed while reviewing (Nov 22)

- [x] BG100. **`Int_to_str_into` (LF13 stack strings) printed garbage for
      positive numbers and returned a string with no header in front of it.**
      stdlib/core.fd: a positive `x` was never negated (`f"v = {42}"` printed
      `v = ,.`), and the digits were returned at `p + 1` while the header was
      written at `buf`. Fixed in the working tree (digits moved to `buf + 16`,
      `v` kept <= 0); checked on run, C, pe_ir and --nat with 0, -7, i64::MIN
      and i64::MAX.

## Found by the Nov 27 whole-repo review

Found on Nov 27 by five review passes, one per area: the front end (lexer,
parser, typechecker, borrowck, escape, mono, comptime); the backends
(lowering, iropt, e-graph, C, pe_ir, the machine layer), compared against
`fid run`; the runtime (core.fd, interp.fd, the pe_ir and machine-layer
runtimes, the C runtime); the standard library; and the tools (main.fd
commands, fmt, LSP, scribe/veil/ark, tools/, scripts/, CI workflows, the
installer). Each pass checked its findings against this file (BG1-BG257),
known-bugs.md §2g and the bugs being fixed at the time. Where two passes
found the same bug, their findings are merged into one entry ("found by two
passes").

Probes used the zig build of the Nov 27 tree from 01:25. Its sources are
older than the working tree in 26 compiler/*.fd files, so each entry's
function was diffed against the working tree, and an entry is kept only
where that code is unchanged. Borrow-read findings were dropped for this
reason, since the LG3 commits came after that build. "Confirmed Nov 27"
names the backends a probe ran on: run = `fid run`, ccompile = C,
emit = pe_ir, --nat = `emit --nat`. Linux targets were not run. "Read only"
marks the few entries no probe could reach. The probe programs are not in
the repo. Function names are the reference; line numbers move.

### Critical

- [ ] BG258. **An assignment to a match-pattern binding is silently dropped:
      `Some(x) => { x = x * 2; println(x); }` prints the old value.**
      compiler/lower.fd Lowerer_lower_match (EnumVariant arm:
      `Lowerer_insert_alloca(self, binding.name, - 1);
      Lowerer_bind_local(self, binding.name, field_val);` ~22517: the binding
      is an SSA value with no slot): the type checker lets a binding be
      assigned (it is not a `val`), but lowering gives the binding alloca id
      -1, so `x = ...` / `x += ...` store nowhere and every later read of `x`
      sees the payload. Silent wrong values, no diagnostic. Trigger: `enum E {
      A(i64), B } fn main() -> i64 { val e = E::A(3); match e { E::A(x) => { x
      = x * 2; println(x); x += 1; println(x); } E::B => { } } return 0; }` ->
      prints 3, 3 on fid run (and 3 on ccompile for the Option form); should
      print 6, 7 (or reject the assignment). Confirmed Nov 27 (fid run: 3 / 3;
      ccompile with `Option::Some(x)`: 3). Confidence: high.
- [ ] BG259. **Machine layer: narrow-integer arithmetic in an inlined function
      with a literal argument is folded at 64 bits, so `u8 200 + 100` gives
      300 with no overflow panic.** compiler/egraph.fd eg_function (BinOp node
      built with `ots = eg_int_ty(eg_value_ty(f, a))`, the operand type, not
      the instruction's type `t`) + compiler/iropt.fd iro_inline_function
      (BG43 fix bit-casts only same-width arguments; a wider I64 literal for a
      u8/i16/i32 parameter is mapped straight in): lowering passes `a4(200)`'s
      literal as `ConstInt(200, i64)`. The inliner maps the u8 parameter to
      that i64 constant (the BitCast is added only when `ir_type_size(aty) ==
      ir_type_size(pty)`). The body's `BinOp(Add, 200:i64, 100:i64) ; u8` then
      reaches the e-graph, which types the node by its operands ("i64"), folds
      300 (in range for i64) and replaces the u8 result with 300. Neither the
      overflow panic nor narrowing to 8 bits happens. pe_ir and C (no e-graph)
      are right. FID_NAT_OFF=egraph, FID_EGRAPH_OFF=fold or
      FID_IROPT_OFF=inline,inline2 each make it panic correctly. Trigger: `fn
      a4(x: u8) -> u8 { val y: u8 = x + 100; return y; } fn main() {
      println(f"a4 {a4(200)}"); }` -- `emit --nat` prints `a4 300`;
      run/ccompile/emit panic "Integer overflow on addition". Same for `x * 2`
      (u8 200 -> 400), i16 `x + 1000` of 32000 -> 33000, i32 `x * 16` of 2^28
      -> 4294967296. Confirmed Nov 27 (emit --nat prints
      300/400/33000/4294967296; run, ccompile, emit panic). Confidence: high.
- [ ] BG260. **pe_ir negates an f32 by flipping bit 63, so `-x` of an f32
      keeps its sign (`-2.0` gives 2, `1.0 / -0.0` gives +inf).**
      compiler/pe_ir.fd PeirGen (instruction lowering), `Inst::UnaryOp(op,
      operand, ty)` -> `UnaryOp::Neg` float branch: the float branch loads the
      operand with movsd, XORs it with `1 << 63` (the f64 sign bit) and, for
      F32, stores the low 32 bits back with movss. An f32 lives in the low 32
      bits, whose sign bit is bit 31, so the stored value is unchanged. C, run
      and --nat negate correctly. Trigger: `fn idf(x: f32) -> f32 { return x;
      } fn main() { val neg: f32 = -idf(2.0); println(f"neg {neg}"); val nz:
      f32 = -idf(0.0); println(f"{idf(1.0) / nz}"); }` -- emit prints `neg 2`
      and `inf`; run/ccompile/--nat print `neg -2` and `-inf`. Confirmed Nov
      27 (emit wrong; run, ccompile, emit --nat right). Confidence: high.

### High

- [ ] BG261. **A const's initializer is never type-checked: `const C: i64 =
      "abc"` compiles and `C` is a str.** compiler/typechecker.fd
      TypeChecker_check_program (the BG117 loop after "Check function bodies"
      calls only TypeChecker_check_value_fits on cdef.expr): the only check on
      a ConstDef is the literal-fit check; the initializer's type is never
      computed or compared with cdef.ty, so any expression of any type is
      accepted and substituted at each use with the wrong declared type.
      Trigger: `const C: i64 = "abc"; const B: bool = 5; fn main() -> i64 {
      println(C); println(B); return 0; }` -> `fid run` prints `abc` and `5`;
      should be a type error (cannot assign str to i64 / i8 to bool).
      Confirmed Nov 27 (fid run: abc / 5, exit 0). Confidence: high.
- [ ] BG262. **A global's initializer is neither type-checked nor fit-checked:
      `var G: i64 = "str"` prints a pointer.** compiler/typechecker.fd
      TypeChecker_check_program (no loop over program.globals checks .init
      against .ty; globals are only scanned for called functions, N5 ~27074,
      and cold-start cost ~12165): reference §2.7/§3.4 say a declaration's
      value must have the declared type and literals must fit; for globals
      nothing compares init's type with the declared type or calls
      TypeChecker_check_value_fits. Trigger: `var G: i64 = "str"; var H: bool
      = 7; var U: u8 = 700; fn main() -> i64 { println(G); println(H);
      println(U); return 0; }` -> fid run prints `5384` (the string's
      address), `true`, `188`; each should be a type error. Confirmed Nov 27
      (fid run: 5384 / true / 188). Confidence: high.
- [ ] BG263. **Default parameter values are never type-checked: `fn h(x: i64 =
      "hello")` passes a pointer, `x: u8 = 500` holds 500.**
      compiler/typechecker.fd (defaults are only recorded, fn_pdefaults /
      TypeChecker_register_program counts required params; no check of the
      default expr against the param type); lowering supplies the default as
      is: the default expression is substituted at each call that omits the
      argument without any type or fit check, so a u8 parameter can hold 500
      and an i64 parameter a str address; a bool parameter receives a float.
      Trigger: `fn h(x: i64 = "hello") -> i64 { return x; } fn k(b: bool =
      3.5) -> bool { return b; } fn j(x: u8 = 500) -> i64 { return x as i64; }
      fn main() -> i64 { println(h()); println(k()); println(j()); return 0;
      }` -> fid run prints `4112`, `3.5`, `500`; each default should be
      rejected. Confirmed Nov 27 (fid run: 4112 / 3.5 / 500). Confidence:
      high.
- [ ] BG264. **A nested constructor pattern defeats the exhaustiveness check:
      `Some(Ok(x))` + `None` compiles and panics at run time on
      `Some(Err(..))`.** compiler/parser.fd Parser_pd_desugar_arms (LG8
      nested-pattern desugaring): an arm `Some(Ok(x))` is rewritten to
      `Some(__np) => match __np { Ok(x) => .., _ => <rest arms, else
      panic("nested match: no arm matched")> }`. The synthetic `_` arm makes
      every inner match exhaustive, and the outer `Some(..)` arm covers all of
      `Some`, so TypeChecker_check_exhaustive never sees that `Some(Err(_))`
      is uncovered (reference §6: a match on an enum must cover every
      variant). Trigger: `fn f(o: Option<Result<i64, str>>) -> i64 { match o {
      Option::Some(Result::Ok(x)) => { return x; } Option::None => { return 0;
      } } return 9; } fn main() -> i64 {
      println(f(Option::Some(Result::Err("bad")))); return 0; }` -> fid check
      passes; fid run panics "nested match: no arm matched"; should be
      "non-exhaustive match ... missing Result::Err". Confirmed Nov 27 (fid
      run: Panic: nested match: no arm matched). Confidence: high.
- [ ] BG265. **A `-> !` function may return normally, and its callers then
      fall off their end with no value.** compiler/typechecker.fd
      TypeChecker_check_func_body (Type::Never sets returns_value = false:
      nothing checks that a `-> !` body never reaches its end or a `return`);
      stmts_always_leave treats a call of a `!` function as leaving: reference
      §2.1: `-> !` means the function never returns. Nothing enforces it,
      while the missing-return check trusts it, so a caller whose last
      statement is the call is accepted and returns garbage. Trigger: `fn
      die() -> ! { println("not dying"); } fn pick(x: i64) -> i64 { if x > 0 {
      return 1; } die(); } fn main() -> i64 { println(pick(-1));
      println("after"); return 0; }` -> run prints `not dying`, `void`,
      `after`; ccompile prints `not dying`, `0`, `after`. Should be an error:
      'die' is declared `-> !` but can return. Confirmed Nov 27 (fid run and
      ccompile). Confidence: high.
- [ ] BG266. **Comptime arithmetic on `i8`/`i16`/`i32` is done at 64 bits:
      `comptime f(50000)` with `fn f(x: i32) -> i32 { x * x }` gives
      2500000000.** compiler/comptime_eval.fd: only unsigned operands get
      width-aware arithmetic (ct_eval_uint_binop, BG62); signed values go
      through ct_add_checked / ct_mul_checked etc., which check i64 only
      (ct_ovf: "does not fit in i64"): reference §4.11: comptime arithmetic is
      checked and overflow is an error, never a silently wrong value. At run
      time the same call panics "Integer overflow on multiplication"; at
      compile time it yields a value outside i32 that the program then holds
      in an i32 result. (Also: for a u8 overflow the error text says "100 +
      200 does not fit in i64", naming the wrong type.). Trigger: `comptime fn
      f(x: i32) -> i32 { return x * x; } fn rf(x: i32) -> i32 { return x * x;
      } fn main() -> i64 { val a = comptime f(50000); println(a); val n: i32 =
      50000; println(rf(n)); return 0; }` -> prints 2500000000, then the
      run-time call panics "Integer overflow on multiplication: 50000 *
      50000". The comptime call should be a comptime overflow error. Confirmed
      Nov 27 (fid run). Confidence: high.
- [ ] BG267. **An f64 literal given to an f32 enum payload is stored
      unconverted, so `E::F(2.5)` and `Option::Some(1.5)` of an `Option<f32>`
      read back as 0 on every backend.** compiler/lower.fd Lowerer_lower_expr,
      `Expr::EnumLit` (the non-struct payload branch:
      `Inst::GetFieldPtr(alloca, field_idx, arg_ty)` + `Inst::Store(data_ptr,
      arg_val, OptIrType::None)`): the payload is stored with its own IR type
      (F64 for a float literal) and never converted to the declared payload
      type, unlike call arguments (the BG165/BG246 fixes). The binding then
      reads 32 bits of an f64 word: 2.5 = 0x4004000000000000, low half 0. A
      computed f32 (`E::F(y)`, `Option::Some(x / 2.0)`) is right; a `match` on
      the Option gives the same 0. Trigger: `enum E { F(f32), D(f64) } fn
      show(e: E) -> str { match e { E::F(x) => { return f"F {x}"; } E::D(d) =>
      { return f"D {d}"; } } } fn main() { println(show(E::F(2.5))); val o2:
      Option<f32> = Option::Some(1.5); println(f"{o2.unwrap()}"); }` prints `F
      0` and `0` (want `F 2.5`, `1.5`). Confirmed Nov 27 (run, ccompile, emit,
      emit --nat; found by two passes). Confidence: high.
- [ ] BG268. **`HashMap<K, f32>` `get`/`get_or` return the raw stored word, so
      an f32 map value reads as an integer bit pattern on every backend.**
      compiler/lower.fd Lowerer_map_values_f64 (only `IrType::F64` /
      `Type::F64` values are bit-cast back with p_f64_from_bits after
      `HashMap_get*`): the type checker types `m.get(k)` of a `HashMap<i64,
      f32>` as f32, but lowering converts the i64 word core's HashMap_get
      returns only for f64 maps; for f32 the word is converted as an integer.
      run/C/pe_ir give the f64 bits of the value (insert widened it), --nat
      the f32 bits, so the backends also disagree with each other.
      `m.values()` of the same map is right. Trigger: `fn main() { var m:
      HashMap<i64, f32> = HashMap::new(); val y: f32 = 3.5; m.insert(1, y);
      val g: f32 = m.get(1); println(f"map {g}"); }` -- run/ccompile/emit
      print `map 4615063718147915776`, --nat `map 1080033280` (want `map
      3.5`); `m.get_or("zz", 2.5)` on a `HashMap<str, f32>` gives
      `4612811918334230528`. A `HashMap<i64, f64>` is right. Confirmed Nov 27
      (run, ccompile, emit, emit --nat; found by two passes). Confidence:
      high.
- [ ] BG269. **A float literal in a tuple literal stays f64 where the tuple
      type says f32: `return (a, 2.5)` for `(f32, f32)` reads 0 on every
      backend.** compiler/lower.fd Lowerer_lower_expr
      `Expr::TupleLit(elements)` (element IR types taken from the lowered
      values: an f64 ConstFloat for the literal; no conversion to the expected
      f32 element): the tuple is laid out from its element values' types, so
      the literal's slot holds f64 bits; the destructuring `val (c, d) =
      mk2()` reads it as f32 (low 32 bits, 0 for 2.5). A non-literal f32
      element is right. Same class as the enum-payload finding, separate code
      path. Trigger: `fn mk2() -> (f32, f32) { val a: f32 = 1.5; return (a,
      2.5); } fn main() { val (c, d) = mk2(); println(f"{c} {d}"); }` -- all
      four backends print `1.5 0` (want `1.5 2.5`). Confirmed Nov 27 (run,
      ccompile, emit, emit --nat). Confidence: high.
- [ ] BG270. **A field of type parameter `T = f32` in a generic struct is read
      as an integer word: `G { v: idf(3.5) }.v` prints a bit pattern, and
      every backend prints a different one.** compiler/lower.fd
      Lowerer_lower_field_access_expr (type-parameter field: `match want {
      IrType::F64 => p_f64_from_bits ..., IrType::Ptr(_) => ... }`, no F32
      case) and Lowerer_lower_struct_lit_expr (a type-parameter field's value
      is turned into bits only for `IrType::F64`): the type checker accepts
      `val x: f32 = g.v`, but the IR value stays i64: `x + idf(1.0)` is
      integer addition and the f-string prints an integer. The word itself
      differs per backend (run: the f64 bits 4615063718147915776, C: 3 (a
      numeric conversion), pe_ir/--nat: the f32 bits 1080033280). `G<f64>` is
      right (2.5). Trigger: `struct G<T> { v: T } fn idf(x: f32) -> f32 {
      return x; } fn main() { val g = G { v: idf(3.5) }; val x: f32 = g.v;
      println(f"{x} {x + idf(1.0)}"); }` -- run `4615063718147915776
      4615063718147915776`, ccompile `3 4`, emit/--nat `1080033280 1080033280`
      (want `3.5 4.5`). Confirmed Nov 27 (all four backends wrong, and they
      disagree). Confidence: high.
- [ ] BG271. **`Option<f32>.unwrap_or(1.5)` is instantiated as
      `Option__unwrap_or__f64`, so a `Some` payload's f32 bits are read as an
      f64 (7.5 prints 5.38e-315).** compiler/mono.fd (generic method
      instantiation of core `Option::unwrap_or(self: Option<T>, default: T)`:
      T taken from the f64 literal argument, not from the receiver's
      `Option<f32>`): dump-ir shows `fn Option__unwrap_or__f64(%0, %1:
      *struct.Option, %2: f64) -> f64` for a receiver declared `Option<f32>`;
      the payload word holds f32 bits (stored by `Some(idf(7.5))`), which the
      f64 instance returns as a denormal double. With an f32 variable as the
      default the f32 instance is used and the result is right. Trigger: `fn
      idf(x: f32) -> f32 { return x; } fn main() { val o: Option<f32> =
      Option::Some(idf(7.5)); println(f"{o.unwrap_or(1.5)}"); }` -- prints
      0.000...05382699284 on run, ccompile, emit; 0.000...026602657194 on
      --nat (want 7.5). Integers too: `val o: Option<i32> =
      Option::Some(id32(-42)); o.unwrap_or(5)` gives 4294967254 on all four
      backends (the i64 instance reads the zero-extended i32 word).
      `Result<i32, str>` too: `Result::Ok(id32(-9)).unwrap_or(1)` gives
      4294967287 on run/C/--nat and 140728898420727 (junk upper bits) on emit.
      Confirmed Nov 27 (all four backends; dump-ir shows the f64 instance).
      Confidence: high.
- [ ] BG272. **`spawn` of a function returning f32 gives 0 after `await` on
      every compiled backend (pe_ir also loses an f32 parameter).**
      compiler/lower.fd Lowerer_emit_task_adapter (parameters: `match lty {
      IrType::F64 => p_f64_from_bits ... }`; result: `match ret_ty {
      IrType::F64 => p_f64_to_bits ... }`, no F32 case) vs. `Expr::Await`,
      whose BG200 F32 case reads the word as an f64's bits and narrows it: the
      adapter hands an f32 result back as the raw register word (f32 bits, or
      whatever the backend leaves), while await decodes the word as f64 bits:
      the result is 0 (a denormal narrowed). The parameter side has no F32
      case either; on pe_ir the f32 argument arrives as 0. `fid run` is right;
      spawn of i8/u16 arguments is right. Trigger: `fn pin(a: f32) -> i64 {
      return (a * 100.0) as i64; } fn pout(b: i64) -> f32 { return (b as f32)
      * 0.5; } fn idf(x: f32) -> f32 { return x; } fn main() { val t = spawn
      pin(idf(1.25)); println(f"param {await t}"); val t2 = spawn pout(7); val
      r: f32 = await t2; println(f"ret {r}"); }` -- run `param 125 / ret 3.5`;
      ccompile and --nat `param 125 / ret 0`; emit `param 0 / ret 0`.
      Confirmed Nov 27 (tree diff of Lowerer_emit_task_adapter: only BG228
      ownership lines added; no float change). Confidence: high.
- [ ] BG273. **`opt ?? d` on an `Option<i8/i16/i32>` or `Option<bool>` used
      directly (not bound to a typed variable) gives the payload's
      zero-extended word: `Some(-1i8) ?? 0` is 255, `Some(-42i32) ?? 5` is
      4294967254.** compiler/lower.fd Lowerer_coalesce_ir_type (an Option
      payload that is not a float or str gives `IrType::I64`) used by
      `Expr::Coalesce` lowering, which loads the payload slot with that type:
      (known-bugs 2g.54 was the str/float half of this and is fixed: that is
      what Lowerer_coalesce_ir_type handles; the narrow-integer and bool
      payloads were left out.) the payload word of a narrow signed integer
      holds its bits zero-extended (as the probes show), and the coalesce
      result is typed i64 instead of the payload type, so it is never
      sign-extended: a negative narrow value prints and compares as a large
      positive number. `val b: i32 = oi ?? 5` happens to be right (the store
      truncates). Same on all backends. Trigger: `fn id32(x: i32) -> i32 {
      return x; } fn id8(x: i8) -> i8 { return x; } fn main() { val oi:
      Option<i32> = Option::Some(id32(-42)); val o8: Option<i8> =
      Option::Some(id8(-1)); println(f"{oi ?? 5} {o8 ?? 0}"); }` -- all four
      backends print `4294967254 255` (want `-42 -1`). An `Option<bool>` is
      worse: `val o: Option<bool> = Option::Some(true); println(f"{o ??
      false}")` prints `1` on run/C and junk on the native backends (emit
      `5368810753`, --nat `4295425`: the upper bytes of the payload word are
      not defined for a bool), want `true`. Confirmed Nov 27 (run, ccompile,
      emit, --nat; function identical in the tree). Confidence: high.
- [ ] BG274. **`Rc::new(x)` / `Arc::new(x)` of a float keeps only its integer
      part: `Rc::new(4.25).get()` is 4.0 on every backend.** compiler/lower.fd
      Lowerer_lower_rc_new (`as_word = ir_type_is_float(val_ty)` then
      `Lowerer_lower_cast(self, v_lw, val_ty, IrType::I64)`) and
      Lowerer_lower_rc_method (`get`: `Lowerer_lower_cast(self, res,
      IrType::I64, t_ir)`): the comments say the word is "a float's bits", but
      `Lowerer_lower_cast` F64 -> I64 is the numeric conversion (truncation),
      and I64 -> F64 on `get` converts back numerically. The fraction is lost
      (f32 too: `Rc::new(idf(4.5)).get()` prints 4; `Arc::new(-2.75)` gives
      -2). Lowerer_box_new is right (Box<f64> prints 1.25). Trigger: `fn
      idd(x: f64) -> f64 { return x; } fn main() { val rd =
      Rc::new(idd(4.25)); val x: f64 = rd.get(); println(f"{x} {x + 0.5}"); }`
      -- all four backends print `4 4.5` (want `4.25 4.75`). Confirmed Nov 27
      (run, ccompile, emit, --nat; both functions identical in the tree).
      Confidence: high.
- [ ] BG275. **Rc/Arc methods on a local whose type is inferred (`val r =
      Rc::new(5); val r2 = r.clone();`) are lowered as plain calls: `r2.get()`
      becomes `Vec_get`, `a.count()` an unknown function `count`.**
      compiler/lower.fd Lowerer_lower_method_call (the Rc/Arc branch needs
      `Lowerer_resolve_type(Lowerer_infer_expr_ast_type(receiver))` to be
      `Rc<..>`/`Arc<..>`; a local bound without an annotation has no recorded
      AST type): the type checker knows the type (`fid check`: no errors), but
      lowering skips the Rc branch: `.get()` falls through to the Vec method
      path and emits `Vec_get(env, handle)` with no index; `.count()` becomes
      a call to a function named `count`. The conformance test
      s13_3_rc_count_clone.fd passes only because it writes `val b: Rc<i64> =
      a.clone();`. Trigger: `fn main() { val r = Rc::new(5); val r2 =
      r.clone(); println(f"{r2.get()} {r.count()}"); }` -- run: `Panic: Vec
      index out of bounds` in Vec_get; ccompile: C compilation failed
      (`int64_t Vec_get(void*, int64_t, int64_t)` called with two arguments);
      emit and --nat: `Panic: Invalid memory access` (want `5 2`). `val a =
      Rc::new(41); val b = a.clone(); println(b.count());`: run `Panic:
      Unknown function 'count'`, ccompile `call to undeclared function
      'count'`, emit/--nat do not build. With `Rc<i64>` annotations both print
      the right values. Confirmed Nov 27 (run, ccompile, emit, emit --nat;
      found by two passes). Confidence: high (behaviour), medium (the exact
      inference function).
- [ ] BG276. **A typed `const` is lowered as its bare literal (i64): `const
      CU: u8 = 200; CU + CU` wraps to 144 with no overflow panic, and `const
      CB: u64 = u64::MAX` prints -1 and compares `CB > 1` false.**
      compiler/lower.fd Lowerer_lower_expr, `Expr::Ident` "Check if constant
      (pc)": `Lowerer_lookup_constant(self, name)` -> `return
      Lowerer_lower_expr(self, const_expr);` (the declared type of the const
      is not applied): the type checker gives `CU` its declared type u8 (so
      `val x: u8 = CU + CU` is accepted), but lowering substitutes the
      initializer expression, an i64 ConstInt: the add is an i64 BinOp
      (dump-ir: `%3 = BinOp ; i64`), 400 is stored into the u8 and truncated;
      a u64 const is a signed i64 everywhere (printing, `>`, `/`, `>>`).
      Reference §3.5: overflow panics at the declared width; u64 compares and
      prints unsigned. Trigger: `const CU: u8 = 200; const CB: u64 =
      18446744073709551615; fn main() { val x: u8 = CU + CU; println(f"{x}
      {CB} {CB > 1}"); }` -- all four backends print `144 -1 false` (want an
      overflow panic; with the add removed `18446744073709551615 true`).
      Confirmed Nov 27 (run, ccompile, emit, --nat; the lowering lines are the
      same in the tree). Confidence: high.
- [ ] BG277. **`x?` on an `Option<i8>` (by the code, any narrow integer
      payload of Option/Result) yields the zero-extended word: `Some(-3i8)?`
      is 253 on run/C and junk on the native backends.** compiler/lower.fd
      Lowerer_lower_try_expr (the unwrapped value's `ok_ty` is the payload's
      IR type only when `ir_type_is_float(first_ir)`; otherwise `IrType::I64`,
      then `Inst::Load(GetFieldPtr(tval, 1, ok_ty), ok_ty)`): the payload of a
      narrow integer is read as a full i64 word and never sign-extended or
      narrowed, so `val z = o8()?; z as i64` is 253 for -3. On pe_ir and --nat
      the payload word's upper bytes are not defined for a narrow value, so
      the result is junk (96638681570 / 43732450 in the probe). Same root
      shape as the `??` finding (Lowerer_coalesce_ir_type), separate function.
      Trigger: `fn id8(x: i8) -> i8 { return x; } fn o8() -> Option<i8> {
      return Option::Some(id8(-3)); } fn use8() -> Option<i64> { val z =
      o8()?; return Option::Some((z as i64) * 10); } fn main() {
      println(f"{use8().unwrap()}"); }` -- run and ccompile print 2530, emit
      96638681570, --nat 43732450 (want -30). Confirmed Nov 27 (all four
      wrong, the native two with junk; function identical in the tree).
      Confidence: high.
- [ ] BG278. **Every `v.clone()` (and `v.map(f)`) of a Vec leaks the new Vec,
      bound to a variable or not.** compiler/lower.fd Lowerer_rc_note_call /
      lw_rc_fresh_handle_call, and the method lowering of Vec `clone`/`map`
      (Lowerer_rc_own_value takes a share of it): lw_rc_fresh_handle_call
      lists Vec_clone, but Lowerer_rc_note_call registers the result as the
      statement's temporary only when the call's IrType is Generic/Opaque
      "Vec"; the method form `a.clone()` (and core's `impl Vec` map) is not
      treated as a fresh owned value. So `var b = a.clone();` is given a
      second owner with Vec_share (a 16-byte share box) and the clone's own
      owner is never dropped: its 40-byte header, its buffer (and, for a Vec
      of structs/strings, every element it holds) leak; as a temporary
      (`a.clone().len()`, `v.extend(a.clone())`, `a.map(f).len()`) nothing
      drops it at all. HashMap clone/keys and str_split in the same positions
      are freed. Trigger: `fn main() { var a: Vec<i64> = Vec::new();
      a.push(1); var b = a.clone(); println(f"{b.len()}"); }` -> `fid run p.fd
      --leaks`: "3 block(s), 120 bytes, never freed: 64 bytes Vec_push <-
      Vec_clone <- main; 40 bytes Vec_new <- Vec_new_typed <- Vec_clone; 16
      bytes p_malloc_uninit <- Vec_share"; should be "every block was freed".
      `val e: i64 = a.clone().len();` and `val m: i64 = a.map(dbl).len();`
      leak 104 bytes each. With `struct P { name: str, n: i64 }`, `var w:
      Vec<P> = v.clone();` also leaks every element's string. Confirmed Nov 27
      (`fid run --leaks`, which builds through C); `m.clone().len()`,
      `m.keys().len()`, `str_split(..).len()` are freed. Confidence: high.
- [ ] BG279. **Printing a `Vec` or `HashMap` of f64, f32, bool, u64 or enums
      shows the raw words: `[1.5, -2.25]` prints as `[4609434218613702656,
      -4611123068473966592]`.** stdlib/core.fd Vec_to_str and HashMap_to_str
      (used by `println(v)` and `f"{v}"`): Vec_to_str formats every element
      that is not a str or a Vec with `p_to_str(elem)`, an integer formatter
      (it switches only on P_TID_STR / P_TID_VEC): a double's bits print as an
      i64, bools as 1/0, a u64 above i64::MAX as signed (reference §3.5: u64
      prints unsigned), an enum element as its box address. HashMap_to_str has
      the same gap for values. A `Vec<f32>` prints f64 bits on run/C/pe_ir and
      f32 bits on --nat. reference.md §1.8 lists a Vec as a value an f-string
      can show. Trigger: `fn main() { var h: Vec<f64> = Vec::new();
      h.push(1.5); h.push(-2.25); println(h); var b: Vec<bool> = Vec::new();
      b.push(true); println(f"{b}"); var u: Vec<u64> = Vec::new();
      u.push(18446744073709551615); println(f"{u}"); var n: HashMap<str, f64>
      = HashMap::new(); n.insert("a", 2.5); println(n); }` ->
      `[4609434218613702656, -4611123068473966592]`, `[1]`, `[-1]`, `{a:
      4612811918334230528}`; want `[1.5, -2.25]`, `[true]`,
      `[18446744073709551615]`, `{a: 2.5}`. Confirmed Nov 27 (run, ccompile,
      emit, emit --nat print the same wrong text; found by two passes).
      Confidence: high.
- [ ] BG280. **A `HashMap<f64, V>` (or f32 key) is accepted, then treated as a
      string-keyed map: every compiled backend crashes in hm_hash, `fid run`
      merges the keys.** compiler/lower.fd Lowerer_map_has_int_keys /
      lw_is_int_type (only integer and bool keys select the integer-key map;
      `HashMap_new_typed`'s integer flag likewise); stdlib/core.fd hm_hash /
      p_str_hash: the type checker accepts any key type (reference.md §13.3
      names no restriction). For a float key Lowerer_map_has_int_keys answers
      false, so HashMap_insert/get hash and compare the key as a string
      address: the double's bits are dereferenced as a pointer. The
      interpreter does not crash but keeps one key of three. Either the
      checker should reject float keys or the map should hash them by bits.
      Trigger: `fn main() { var m: HashMap<f64, i64> = HashMap::new();
      m.insert(1.5, 1); m.insert(-2.0, 2); m.insert(0.5, 3);
      println(f"{m.get(1.5)} {m.get(-2.0)} {m.contains(2.5)} {m.len()}"); }`
      -> run: `3 3 true 1` (want `1 2 false 3`); ccompile, emit, emit --nat:
      `Panic: Invalid memory access (null or dangling pointer) at
      0xFFFFFFFFFFFFFFFF` in hm_hash <- hm_put <- HashMap_insert. Confirmed
      Nov 27 (run, ccompile, emit, emit --nat; found by two passes).
      Confidence: high.
- [ ] BG281. **`regex_split` drops the input text when the pattern can match
      the empty string.** stdlib/regex.fd regex_split: on a zero-width match
      the loop pushes `str_slice(s, i, found)` and then sets `i = found + 1`,
      so the byte at `found` is skipped and never becomes part of the next
      piece (regex_replace copies that byte into `out`; regex_split has no
      equivalent). Every byte at a zero-width match position is lost. Trigger:
      `regex_split("abc", "x*")` -> ['','','',''] (Python:
      ['','a','b','c','']); `regex_split("one two", "\b")` ->
      ['','ne','','wo'] (the 'o' and 't' are gone; Python: ['','one','
      ','two','']); `regex_split("ab1cd", "\d*")` -> six empty strings.
      Confirmed Nov 27 (fid run). Confidence: high.
- [ ] BG282. **HPACK bomb: `hpack_decode` has no decoded-size limit, and
      HTTP/2 checks the header list only after decoding a 256 KiB block.**
      stdlib/hpack.fd hpack_decode; stdlib/http2.fd h2_on_header_block:
      hpack_decode appends every field to r.names/r.vals with no bound on the
      decoded size; a one-byte indexed field (0x80|62) copies a 4 KiB
      dynamic-table entry. h2_on_header_block calls hpack_decode on blocks up
      to H2_MAX_HEADER_BLOCK (262144 bytes) and only afterwards compares
      against H2_OUR_MAX_HEADER_LIST (65536). A peer sending one 4064-byte
      literal then 262,000 one-byte references makes the server build about 1
      GB of header strings per HEADERS frame (the classic HPACK bomb,
      CVE-2016-6581); the 431 comes after the memory is spent. Trigger: a
      6,006-byte block (literal "x"=4000 x 'a' with incremental indexing, then
      2000 bytes of 0xBE) -> `hpack_decode` ok=true, 2001 fields, decoded size
      8,070,033 bytes (1,343x amplification); the limit is never consulted
      inside the decoder. Confirmed Nov 27 (fid run, hpack_decode); the
      http2.fd order (decode, then size check) read only. Confidence: high.
- [ ] BG283. **J1939 PDU1/PDU2 are swapped, so `j1939_pgn` gives the wrong PGN
      for every message.** stdlib/can.fd j1939_is_pdu1, j1939_is_pdu2,
      j1939_pgn (and the comment above j1939_id): SAE J1939-21 defines PDU1
      (destination-specific, PS = destination address, not part of the PGN) as
      PF 0..239 and PDU2 (broadcast, PS = group extension, part of the PGN) as
      PF 240..255. The module has it backwards: `j1939_is_pdu1` tests `pf >=
      240` and `j1939_pgn` drops PS when `pf >= 240` and keeps it below.
      Broadcast PGNs lose their low byte and destination addresses leak into
      the PGN. Trigger: `j1939_pgn(0x0CF00400)` (EEC1, engine speed) -> 61440
      (0xF000), should be 61444 (0xF004), and `j1939_is_pdu1` says true;
      `j1939_pgn(0x18EA17F9)` (Request to address 0x17) -> 59927 (0xEA17),
      should be 59904 (0xEA00). Confirmed Nov 27 (fid run). Confidence: high.
- [x] BG284. **Every `RELEASE=1 scripts/bootstrap.sh` build is reported stale
      by release-fresh.sh, so release.yml's check and build-installer.sh
      always refuse it.** scripts/bootstrap.sh (RELEASE block: `bash
      scripts/build-info.sh` + `trap 'bash scripts/build-info.sh --reset'
      EXIT`, then `release-fresh.sh --write` at the end);
      scripts/release-fresh.sh source_hash: release-fresh.sh's source_hash
      covers every compiler/*.fd, including compiler/build_info.fd
      (build-info.sh's own hash excludes that file, release-fresh's does not).
      bootstrap.sh writes the stamp while build_info.fd still holds the
      release commit/date/version, then the EXIT trap resets build_info.fd to
      empty values. Every later check (release.yml "release/fid.exe is this
      source's release build", build-installer.sh's `release-fresh.sh
      release/fid.exe`) hashes the reset file and prints "is stale: compiler/
      or stdlib/ changed since it was built", exit 1. With FIDES_NO_REBUILD=1
      (release.yml) the installer build errors out; without it,
      build-installer rebuilds (3 stages) and then fails "still not the
      stamped release build". (Fix: leave compiler/build_info.fd out of
      source_hash, or stamp after the reset.). Trigger: in a scratch copy of
      compiler/, stdlib/, scripts/{release-fresh,build-info,bootstrap}.sh with
      a dummy release/fid.exe, the bootstrap order: `bash
      scripts/build-info.sh; bash scripts/release-fresh.sh --write
      release/fid.exe; bash scripts/build-info.sh --reset; bash
      scripts/release-fresh.sh release/fid.exe` -> "release/fid.exe is stale
      ...", exit 1. Confirmed Nov 27 (the script sequence run on a copy of the
      tree; a real 3-stage bootstrap not run). Confidence: high.
      Fixed Nov 27 (scripts/release-fresh.sh source_hash leaves out
      compiler/build_info.fd, as build-info.sh's own hash does, and the
      `+changes` mark ignores it): the same sequence on a copy of the tree
      now says "is the release build of this source", exit 0, and an edit to
      stdlib/core.fd still makes it stale, exit 1.
- [ ] BG285. **`ark install` runs shell commands written into an ark.toml (or
      ark.lock) field: git branch/tag, commit and URL go to p_system
      unescaped.** compiler/ark.fd ark_install_git_at (`cmd + " --branch " +
      gref`, `"checkout --quiet " + locked`), ark_q (just wraps in `"..."`),
      also lines ~1620-1631 and 2126 (`checkout --quiet --detach " + commit`):
      every git step is a p_system string (cmd.exe on Windows, sh elsewhere).
      `gref` and the locked commit are appended with no quoting at all, and
      ark_q only adds double quotes, without escaping `"`, `$(..)`, backquotes
      or `%VAR%`. So a branch, tag, commit or git URL in a manifest is shell
      syntax. Dependencies are resolved through their own manifests
      (ark_resolve_reqs, the "a -> b" chains), so a pact's ark.toml anywhere
      in the tree can run commands on the machine of whoever installs a
      project that pulls it in. Trigger: in an `ark init` project add
      `[dependencies]` `evil = { git = "https://invalid.example/none.git",
      branch = "x & echo INJECTED> inj.txt & rem" }`; `fid ark install` -> the
      clone fails, and inj.txt containing "INJECTED" is created in the project
      directory. Confirmed Nov 27 (fb4 fid.exe on Windows, harmless payload;
      ark.fd identical in the tree). Confidence: high.

### Medium

- [ ] BG286. **A literal or wider integer assigned to a field, an index or
      through `*r` is not fit/narrowing-checked: `s.a = 1000` on a `u8` field
      stores 232.** compiler/typechecker.fd TypeChecker_check_stmt_inner,
      Stmt::FieldAssign / Stmt::IndexAssign / Stmt::DerefAssign
      (TypeChecker_check_value_fits is called for Let ~20263, return ~20444,
      plain Assign ~20543, struct literal fields ~21557, call args ~22280, but
      not these three): reference §3.4 says a literal given to an integer type
      must fit; the plain assignment `x = 300` and `k += 400` are rejected,
      but the same literal through a field, an element or a reference is
      silently truncated. The wider-type check is missing there too: `var big:
      i64 = 5000000000; s.a = big;` on an `i32` field is accepted and prints
      705032704, while `S { a: big }` and `var c: i32 = big` are E079.
      Trigger: `struct S { a: u8 } fn main() -> i64 { var s = S { a: 1 }; s.a
      = 1000; println(s.a); var v: Vec<u8> = Vec::new(); v.push(1); v[0] =
      513; println(v[0]); var x: u8 = 3; val r = &mut x; *r = 260; println(x);
      return 0; }` -> prints 232 / 1 / 4; should be three "literal does not
      fit in u8" errors. Confirmed Nov 27 (fid run: 232 / 1 / 4). Confidence:
      high.
- [ ] BG287. **`Vec<u8>.push(999)` / `Vec<i8>.push(200)` is accepted and
      truncated: Vec method arguments skip the literal fit check.**
      compiler/typechecker.fd TypeChecker_check_expr_inner, Expr::MethodCall
      on Vec (push/insert/set element argument; no
      TypeChecker_check_value_fits call): the element argument of a Vec method
      is type-compared but never fit-checked, so an out-of-range literal is
      stored truncated (BG154 is about insert/contains/get types, not this).
      Trigger: `fn main() -> i64 { var v: Vec<u8> = Vec::new(); v.push(999);
      v.push(-5); println(v[0]); println(v[1]); var w: Vec<i8> = Vec::new();
      w.push(200); println(w[0]); return 0; }` -> run and ccompile both print
      231 / 251 / -56; should be "the literal does not fit in u8/i8".
      Confirmed Nov 27 (fid run and ccompile: 231 / 251 / -56). Confidence:
      high.
- [ ] BG288. **A closure with a declared result type may fall off its end:
      `fn(y: i64) -> str { if y > 0 { return "pos"; } }` compiles.**
      compiler/typechecker.fd TypeChecker_check_func_body (the E047 "can reach
      the end of its body without returning" check, ~13482) runs for named
      functions and methods only, never for Expr::Closure bodies: reference
      §2.1 says a function declared to return a value must not reach the end
      of its body; closures with `-> T` are not checked, so the fall-through
      path returns garbage: `void` under run, 0 / "" from C, and a str-typed
      result makes `fid run` panic in Str_drop. Trigger: `fn main() -> i64 {
      val f = fn(y: i64) -> i64 { if y > 0 { return 1; } }; println(f(-1));
      val g = fn(y: i64) -> str { if y > 0 { return "pos"; } };
      println(g(-1)); return 0; }` -> run prints `void`, `void`, then "Panic:
      Arithmetic error (void, int)" in Str_drop; ccompile prints 0 and an
      empty line. Should be E047 for both closures (a named fn with the same
      body is rejected). Confirmed Nov 27 (fid run and ccompile). Confidence:
      high.
- [ ] BG289. **`&mut` of a `val` is accepted, so a `val` can be changed
      through a reference or a `&mut` parameter.** compiler/typechecker.fd
      Expr::Ref / AddrOf with mut (no TypeChecker_is_val test; the only `val`
      check is in Stmt::Assign ~20593): reference §5.1: `val` binds a name
      that cannot be assigned again. Taking `&mut x` of a `val x` is not
      rejected, and writing through it changes the val. Trigger: `fn bump(p:
      &mut i64) { *p = *p + 100; } fn main() -> i64 { val x = 5; val r = &mut
      x; *r = 6; println(x); bump(&mut x); println(x); return 0; }` -> prints
      6, 106; `x = 6` on the same val is rejected with E044. Confirmed Nov 27
      (fid run). Confidence: high.
- [ ] BG290. **The names bound by `val (a, b) = t` are assignable.**
      compiler/typechecker.fd Stmt::LetTuple (does not record the names in
      val_vars_per_scope; Stmt::Let does, ~20296): a destructuring `val` is
      treated as `var`: `a = 10` and `b += 5` are accepted. Trigger: `fn
      main() -> i64 { val (a, b) = (1, 2); a = 10; b += 5; println(a);
      println(b); return 0; }` -> prints 10, 7; should be "cannot assign to
      'a': it is declared with `val`". Confirmed Nov 27 (fid run). Confidence:
      high.
- [ ] BG291. **A `for` variable, closure parameter or match binding that
      shadows an outer `val` is taken for the `val`: a valid assignment is
      rejected.** compiler/typechecker.fd TypeChecker_is_val (looks up
      val_vars_per_scope; only Stmt::Let with `var` records the shadowing
      `!name`, for/closure params/pattern bindings record nothing): these
      bindings are assignable when nothing shadows them (`for i in 0..2 { i =
      7; }`, `fn(x: i64) -> i64 { x = x + 1; .. }` compile), but with a `val
      x` in an enclosing scope the inner `x = ..` is reported as assigning the
      outer val. Trigger: `fn main() -> i64 { val x = 1; for x in 0..3 { x = x
      + 10; } val f = fn(x: i64) -> i64 { x = x + 1; return x; };
      println(f(4)); return 0; }` -> two E044 "cannot assign to 'x': it is
      declared with `val`"; the same program without the outer `val x`
      compiles and runs. Confirmed Nov 27 (fid check). Confidence: high.
- [ ] BG292. **Visibility covers only functions, consts and globals: a private
      struct, enum, type alias or method of another module is usable.**
      compiler/typechecker.fd TypeChecker_check_visibility is called only from
      the const/global/function name lookups (~21680-21699) and the call path
      (~22222); struct literals, enum constructors, type names and method
      calls never call it: reference §2.9: "an item without `pub` used from
      another module is an error". In a module `mymod` with `struct Priv { a:
      i64 }`, `enum PrivE { One, Two(i64) }`, `type PrivAlias = i64;` and
      `impl Pt { fn hid(self) -> i64 {..} }` (all without pub), the main
      program can build `Priv { a: 3 }`, `PrivE::Two(4)`, declare `val z:
      PrivAlias = 3` and call `p.hid()`; only `secret()` and `HIDDEN` are
      rejected. Trigger: mymod.fd = `module mymod; pub struct Pt { pub x: i64,
      y: i64 } pub fn mk() -> Pt { return Pt { x: 1, y: 2 }; } struct Priv {
      a: i64 } enum PrivE { One, Two(i64) } type PrivAlias = i64; impl Pt { fn
      hid(self) -> i64 { return self.y; } }`; p.fd = `use mymod; fn main() ->
      i64 { var p = mk(); println(p.hid()); val r = Priv { a: 3 }; val e =
      PrivE::Two(4); val z: PrivAlias = 3; println(z); return 0; }` ->
      compiles and runs (prints 2, 3). Confirmed Nov 27 (fid run). Confidence:
      high.
- [ ] BG293. **A private field of another module's struct can be written and
      initialised: only reads are checked.** compiler/typechecker.fd (the E078
      "field ... is private" check is in Expr::FieldAccess only;
      Stmt::FieldAssign and struct-literal field inits (~21557) skip it):
      reference §2.2 makes a non-pub field private to its module. From another
      module `p.y = 99` and `Pt { x: 5, y: 6 }` (setting private `y`) are
      accepted, so the module's invariants on `y` can be broken from outside
      although `p.y` cannot be read. Trigger: with mymod.fd above: `use mymod;
      fn main() -> i64 { var p = mk(); p.y = 99; println(p.hid()); var q = Pt
      { x: 5, y: 6 }; println(q.hid()); return 0; }` -> prints 99, 6;
      `println(p.y)` in the same file is E078. Confirmed Nov 27 (fid run).
      Confidence: high.
- [ ] BG294. **`!{}` is bypassed by a method call through `dyn Trait` or
      through a generic bound `T: Trait`.** compiler/typechecker.fd
      TypeChecker_scan_expr_for_effects (Expr::MethodCall adds only
      tc_eff_method_allocs; the BG22 "through a method it calls" resolution
      needs a concrete receiver type) and TypeChecker_verify_declared_effects:
      since BG22, `l.log()` on a concrete `L` makes a `!{}` caller an E110
      error. The same call on a `dyn Logger` or on a `T: Logger` contributes
      no effect, although every impl of the trait method (here the only one)
      prints. Reference §9.1: an inferred effect not declared is an error.
      (Inside `sandbox { }` the dyn call is also accepted statically and only
      the run-time revocation stops it.). Trigger: `cap Logger { fn log(self)
      -> i64; } struct L { n: i64 } impl Logger for L { pub fn log(self: L) ->
      i64 { println("side effect"); return 1; } } fn via_dyn(d: dyn Logger) ->
      i64 !{} { return d.log(); } fn via_generic<T: Logger>(t: T) -> i64 !{} {
      return t.log(); } fn main() -> i64 { val l = L { n: 1 };
      println(via_dyn(l as dyn Logger)); println(via_generic(l)); return 0; }`
      -> compiles, prints "side effect" twice; `fn direct(l: L) -> i64 !{} {
      return l.log(); }` is rejected. Confirmed Nov 27 (fid run). Confidence:
      high.
- [ ] BG295. **`x |> f` (pipe into a bare function name) adds none of `f`'s
      effects: a `!{}` function can print.** compiler/typechecker.fd
      TypeChecker_scan_expr_for_effects, Expr::PipeForward (~27007) scans
      `right` as an expression; a bare Expr::Ident (~26730) adds nothing, so
      the call it denotes is never looked up: reference §4.8: `x |> f` is
      `f(x)`. `f(x)` makes the caller inherit `!IO`; `x |> f` does not, so the
      declared-effects check passes. (The sandbox check does catch the same
      pipe, so only the effect inference misses it.). Trigger: `fn loud(x:
      i64) -> i64 { println("loud"); return x; } fn via_pipe() -> i64 !{} {
      return 5 |> loud; } fn main() -> i64 { println(via_pipe()); return 0; }`
      -> prints `loud`, `5`; should be E110 "inferred effect '!IO'". Confirmed
      Nov 27 (fid run). Confidence: high.
- [ ] BG296. **`0x`, `0b`, `0o` with no valid digit lex as the literal 0; with
      optional `;` `val m = 0b2;` silently gives 0.** compiler/lexer.fd
      Lexer_read_number (radix branches: the digit loop may consume nothing,
      then str_parse_hex / parse_radix of "" gives 0 and a token is returned):
      an empty radix literal is not an error, and a digit outside the radix
      ends the literal; since semicolons are optional (§1.10) the rest becomes
      a separate expression statement, so a typo in a binary/octal/hex
      constant compiles to 0 with no diagnostic. Trigger: `fn main() -> i64 {
      val a = 0b2; println(a); val b = 0o8; println(b); val c = 0x;
      println(c); return 0; }` -> prints 0, 0, 0 (fid run, exit 0); should be
      a lex error ("binary literal needs a digit" / "invalid digit '2' in
      binary literal"). Confirmed Nov 27 (fid run). Confidence: high.
- [ ] BG297. **`break` and `continue` outside any loop are accepted and
      silently do nothing.** compiler/typechecker.fd
      TypeChecker_check_stmt_inner, Stmt::Break / Stmt::Continue
      (~20936-20946: the optional value is checked, `self.loop_depth` is never
      tested although the field is kept for loops): reference §5.4: break and
      continue apply to the innermost loop. With no loop there is nothing to
      leave, yet the statement compiles and is dropped by lowering, so code
      the author meant to skip runs. Trigger: `fn h(x: i64) -> i64 { if x > 0
      { break; } println("after break"); return 1; } fn main() -> i64 {
      println(h(5)); continue; println("end"); return 0; }` -> run and
      ccompile print `after break`, `1`, `end`; should be "`break` outside a
      loop" / "`continue` outside a loop". Confirmed Nov 27 (fid run,
      ccompile). Confidence: high.
- [ ] BG298. **`fid run`: a global `f32` with a float initializer reads as
      0.** compiler/interp.fd Interpreter_init_globals (+ eval_global_init):
      `eval_global_init` gives `Value::Float(v)` for a FloatLit and
      Interpreter_init_globals stores it with `Memory_store_f64` whatever
      `entry.ty` is; an f32 global is then loaded as 4 bytes, the low half of
      the f64 (0 for 1.5). Compiled backends store the f32. Trigger: `var gf:
      f32 = 1.5; fn main() { println(f"{gf}"); gf = gf + 1.0;
      println(f"{gf}"); }` -- run prints `0` then `1`; ccompile/emit/--nat
      print `1.5` then `2.5`. Confirmed Nov 27. Confidence: high.
- [ ] BG299. **--nat: an f64 literal passed to `Vec<f32>.contains` or
      `Vec::filled` is not narrowed: `contains(1.5)` is false for an element
      that is there, `Vec::filled(3, 1.5)` is all zeros.** compiler/nat_ir.fd
      ng_f32_arg_mask (the BG165 fix narrows the literal for
      Vec_push/set/insert and index stores; Vec_contains and Vec_filled are
      not in its list); stdlib/core.fd Vec_contains compares words: on the
      machine layer a Vec<f32> stores 4-byte f32 bits; the literal reaches
      Vec_contains / Vec_filled_typed as f64 bits, so no element matches and
      filled copies the low half of the double (0 for 1.5). run, C and pe_ir
      narrow it; an f32 variable argument works. Trigger: `fn main() { var g:
      Vec<f32> = Vec::new(); g.push(1.5); val k: f32 = 1.5;
      println(f"{g.contains(1.5)} {g.contains(k)}"); var f: Vec<f32> =
      Vec::filled(3, 1.5); println(f"{f.get(0)}"); }` -> run, ccompile, emit:
      `true true` / `1.5`; emit --nat: `false true` / `0`. Confirmed Nov 27
      (run, ccompile, emit, emit --nat; found by two passes). Confidence:
      high.
- [ ] BG300. **`fid run`: a closure reading a captured `f32` gets 0.**
      compiler/interp.fd Interpreter exec, `Inst::Store(ptr, value, sty_opt)`
      (an explicit store type is honoured only when it is a Struct; a scalar
      `Some(F32)` is ignored and the slot's pointee type, i64 from
      `GetElemPtr(env, idx, I64)`, is used) — fed by compiler/lower.fd closure
      environment building, which stores a float capture with `cap_store_ty =
      Some(F32)` ("a float capture is stored as a float"): the f32 capture is
      written into the environment as an i64 word, then the closure body loads
      it as an f32: 0. f64 captures and narrow-integer captures are right; C,
      pe_ir and --nat are right. Trigger: `fn idf(x: f32) -> f32 { return x; }
      fn main() { val cap: f32 = idf(4.5); val clo = fn(m: f32) -> f32 {
      return cap * m; }; val clo4 = fn() -> f32 { return cap; };
      println(f"{clo(2.0)} {clo4()}"); }` -- run prints `0 0`;
      ccompile/emit/--nat print `9 4.5`. Confirmed Nov 27 (run wrong, the
      three compiled backends right; the Store code's struct-only branch is
      unchanged in the tree apart from memory-API renames). Confidence: high
      (behaviour), medium (exact line).
- [ ] BG301. **`Vec<f32>.sort()` still sorts raw words (BG179 was fixed for
      `Vec<f64>` only): negatives come out backwards, and on --nat after every
      positive.** compiler/lower.fd method-call lowering, the `Vec_sort` ->
      `Vec_sort_f64` redirect (tests `IrType::F64` / `Type::F64` only);
      stdlib/core.fd vec_raw_sort / vec_sz_sort compare words: an f32 element
      type never takes Vec_sort_f64, so the integer sort orders the words: on
      run/C/pe_ir (f64 bit words, signed compare) negative values come out in
      descending order; on --nat (4-byte f32 codes compared unsigned) every
      negative float sorts above every positive one. Trigger: `fn idf(x: f32)
      -> f32 { return x; } fn main() { var v: Vec<f32> = Vec::new();
      v.push(idf(-1.25)); v.push(idf(0.5)); v.push(idf(9.0));
      v.push(idf(-7.5)); v.sort(); println(f"{v[0]} {v[1]} {v[2]} {v[3]}"); }`
      -- run/ccompile/emit `-1.25 -7.5 0.5 9`, --nat `0.5 9 -1.25 -7.5` (want
      `-7.5 -1.25 0.5 9`). Confirmed Nov 27 (run, ccompile, emit, emit --nat;
      found by two passes). Confidence: high.
- [ ] BG302. **An f32 with a format specifier (`f"{x:.3}"`, `{x:8.2}`) prints
      the value's bit pattern as an integer on every backend (BG157 fixed only
      the plain `{x}`).** compiler/lower.fd Lowerer_lower_fstr_fmt (dispatches
      on `ir_type_eq(e_ty, IrType::F64)` to the float formatter; an F32 falls
      to the integer formatter): the f32 is not widened to f64 before core's
      fstr_fmt_* float path, so its word is formatted as an integer: f64 bits
      on run/C/pe_ir, f32 bits on --nat. Trigger: `fn idf(x: f32) -> f32 {
      return x; } fn main() { val f = idf(1.5); println(f"[{f:.3}]
      [{f:8.2}]"); }` -- run/ccompile/emit print `[4609434218613702656]
      [4609434218613702656]`, --nat `[1069547520] [1069547520]` (want `[1.500]
      [    1.50]`). Confirmed Nov 27 (all four; function identical in the
      tree). Confidence: high.
- [ ] BG303. **`f"{m}"` with a HashMap prints the map's handle address as a
      number (and an enum, Option or tuple prints nothing).**
      compiler/typechecker.fd TypeChecker_check_fstring_part (accepts
      "HashMap" explicitly; returns early for enums/tuples); the f-string
      lowering in compiler/lower.fd (no HashMap_to_str for a map part):
      TypeChecker_check_fstring_part lets `Vec` and `HashMap` through as
      having a text form, and only rejects structs. A Vec part goes through
      Vec_to_str, but a HashMap part is converted as an integer, so the output
      is the handle's address (different per backend). Enum, Option and tuple
      parts pass the checker too (not a struct) and print an empty string.
      reference.md §1.8 says a part "may be text, a number, a bool or a Vec"
      and that a value with no text form is an error. `println(m)` prints `{5:
      5}` correctly. Trigger: `fn main() { var m: HashMap<i64, i64> =
      HashMap::new(); m.insert(5, 5); println(m); println(f"x {m}"); val o:
      Option<i64> = Option::Some(4); println(f"o {o}"); }` -> `{5: 5}` then `x
      6152` (run), `x 2312982495304` (C), `x 8388840` (emit), `x 812659080`
      (--nat); `o ` (empty). Should print `x {5: 5}` or be rejected. Confirmed
      Nov 27 (run, ccompile, emit, emit --nat all print an address;
      Option/enum/tuple empty on run). Confidence: high.
- [ ] BG304. **`str_parse_float` of "inf", "infinity", "nan" and hex text
      ("0x10") is inf/NaN/16 on `fid run` and C but 0 on both native Windows
      backends.** stdlib/pe_runtime/base.fd pert_str_to_float (msvcrt `atof`);
      stdlib/linux_runtime/base.fd pert_str_to_float (own parser, used by
      `--nat` and the Linux targets); C runtime `p_str_to_float` (strtod):
      core's str_parse_float is p_str_to_float. The interpreter and the C
      runtime parse with strtod semantics (inf, infinity, nan, hex floats),
      pe_ir calls msvcrt atof, which has none of them, and the machine-layer
      runtime's correctly rounded parser (BG206) accepts only decimal
      digits/exponent. The same program gives different numbers per backend.
      Trigger: `fn main() { println(float_to_str(str_parse_float("inf")) + " "
      + float_to_str(str_parse_float("nan")) + " " +
      float_to_str(str_parse_float("0x10")) + " " +
      float_to_str(str_parse_float("infinity"))); }` -> run and ccompile: `inf
      NaN 16 inf`; emit and emit --nat: `0 0 0 0`. Confirmed Nov 27 (run,
      ccompile, emit, emit --nat). Confidence: high.
- [ ] BG305. **`f"{x:.N}"` prints NaN as "0.00" and infinity as "inf.0" /
      "-inf.000".** stdlib/core.fd fstr_fmt_f64: with a precision,
      fstr_fmt_f64 never tests for NaN or infinity: NaN fails `v < 0.0` and
      `av >= two53`, so `av as i64` gives 0 and the fraction 0 -> "0.00";
      infinity takes the `av >= two53` branch, ipart = float_to_str(inf) =
      "inf", and the ".000" fraction is appended anyway. Without a precision
      the same values print "NaN"/"inf". Trigger: `fn main() { var z: f64 =
      0.0; val n: f64 = z / z; val i: f64 = 1.0 / z; println(f"[{n:.2}]
      [{i:.1}] [{0.0 - i:.3}] [{i:8.2}]"); }` -> `[0.00] [inf.0] [-inf.000] [
      inf.00]`; should be `[NaN] [inf] [-inf] [     inf]` (or the NaN/inf text
      the unformatted path prints). Confirmed Nov 27 (identical on run,
      ccompile, emit, emit --nat). Confidence: high.
- [ ] BG306. **`str_parse_int_opt` returns `Some(0)` for a number outside i64
      instead of None.** stdlib/string.fd str_parse_int_opt: it checks only
      that every byte after an optional sign is a digit, then returns
      `Some(str_parse_int(s))`; str_parse_int returns 0 for an out-of-range
      value (BG175), so the Option API meant to tell failure from a value
      reports a valid 0. Trigger: `use string; ...`
      `str_parse_int_opt("9223372036854775808")` and
      `str_parse_int_opt("99999999999999999999")` -> `Some(0)` (want None);
      "-9223372036854775808" -> Some(i64::MIN) correctly. Confirmed Nov 27
      (run, ccompile, emit, emit --nat identical). Confidence: high.
- [ ] BG307. **Any program that uses `rope_substring` (stdlib string) cannot
      be built by the default Windows backend: pe_ir has no
      `p_string_substring`.** stdlib/string.fd rope_substring (the only stdlib
      caller of p_string_substring); pe_ir / stdlib/pe_runtime (no
      pert_string_substring): rope_substring calls the legacy builtin
      p_string_substring, which the interpreter, the C runtime and the machine
      layer provide but pe_ir does not, so `fid emit` stops with a PE
      generation error. core's str_slice does the same job on every backend.
      Trigger: `use string; fn main() { val r =
      rope_substring(rope_new("hello"), 1, 3); println(rope_to_str(r)); }` ->
      run, ccompile, emit --nat: `el`; `fid emit p.fd -o p.exe`: "PE
      generation errors: PE backend: not available in native Windows
      executables: p_string_substring (build with `fidesc ccompile` instead)",
      exit 1. Confirmed Nov 27 (run, ccompile, --nat print `el`; emit fails).
      Confidence: high.
- [ ] BG308. **`m.get(k)` for a missing key of a map whose values are Vecs or
      structs crashes every compiled backend, while `fid run` returns an empty
      value.** stdlib/core.fd HashMap_get (returns the word 0 for a missing
      key, also for a struct-valued map instead of a slot address); the
      interpreter's simulated memory reads address 0 as zeros: HashMap_get's
      "not found" result is 0. For an i64 or str value that reads as 0 / "",
      but for a `Vec` value it is a null handle and for a struct value
      (hm_mem_size > 0) a null slot address; the caller then reads through it.
      Compiled code faults; the interpreter answers len 0 / zero fields, so a
      program tested with `fid run` crashes once built. Trigger: `struct P {
      x: i64, s: str } fn main() { var w: HashMap<str, P> = HashMap::new();
      w.insert("a", P { x: 1, s: "y" }); val p: P = w.get("q"); println(f"x
      {p.x}"); }` -> run: `x 0`; ccompile, emit, emit --nat: `Panic: Invalid
      memory access (null or dangling pointer) at 0x0`. Same with
      `HashMap<str, Vec<i64>>`: `w.get("q").len()` is 0 on run, a fault at 0x8
      compiled. Confirmed Nov 27 (all four backends). Confidence: high.
- [ ] BG309. **`pop()` on an empty `Vec<Vec<T>>` gives a null Vec: len 0 on
      `fid run`, a fault on every compiled backend.** stdlib/core.fd Vec_pop
      (returns 0 when empty); compiler/lower.fd Lowerer_rc_popped_mem (OW6
      substitutes a zeroed value only for struct elements): an empty pop
      returns the word 0. OW6 made that safe for a Vec of structs (a zeroed
      struct slot), but a Vec element (and a HashMap value, entry above) stays
      a null handle; Vec_len reads header word 1 at address 8. The interpreter
      reads it as 0, so the two disagree. Trigger: `fn main() { var vv:
      Vec<Vec<i64>> = Vec::new(); val x: Vec<i64> = vv.pop(); println(f"len
      {x.len()}"); }` -> run: `len 0`; ccompile/emit --nat: `Panic: Invalid
      memory access (null or dangling pointer) at 0x8`; emit: same panic.
      Confirmed Nov 27 (all four backends). Confidence: high.
- [ ] BG310. **`bigint_from_i64(i64::MIN)` panics with an integer overflow.**
      stdlib/bigint.fd bigint_from_i64: negates the argument with `v = 0 - v`
      (checked arithmetic, not @wrapping) before splitting into limbs, so the
      one value whose negation does not fit panics instead of giving
      -9223372036854775808. Trigger: `use bigint; fn main() {
      println(bigint_to_str(bigint_from_i64(-9223372036854775807 - 1))); }` ->
      "Panic: Integer overflow on subtraction: 0 - -9223372036854775808"
      (should print -9223372036854775808). Confirmed Nov 27 (fid run).
      Confidence: high.
- [ ] BG311. **`bigint_from_str` silently drops non-digit characters instead
      of returning 0 for invalid input.** stdlib/bigint.fd bigint_from_str:
      the doc says "Invalid input -> 0", but the loop only adds characters 0-9
      and skips everything else, so "12a34" parses as 1234, "1e9" as 19,
      "1,000" as 1000. A malformed number is turned into a different valid
      one. Trigger: `println(bigint_to_str(bigint_from_str("12a34")));` ->
      1234 (doc: 0). Confirmed Nov 27 (fid run). (Divmod checked against
      Python on 36 pairs of 10-39 digit numbers: all right.). Confidence:
      high.
- [ ] BG312. **semver `^` and `~` with a partial version are not prefix
      bounds: `^0` matches only 0.0.0, `^0.0` only 0.0.0, `~1` only 1.0.x.**
      stdlib/semver.fd sv_match_token (with sv_pad): the module comment says
      "Partial versions in comparators: missing fields default to 0 and the
      bound becomes a prefix bound for ^ and ~", but sv_match_token pads the
      version with sv_pad first ("0" -> "0.0.0", "1" -> "1.0.0") and then
      computes the upper bound from the padded fields, so the information that
      minor/patch were missing is lost: `^0` -> <0.0.1, `^0.0` -> <0.0.1, `~1`
      -> <1.1.0 (npm/cargo: <1.0.0, <0.1.0, <2.0.0). Trigger:
      `semver_satisfies_str("0.5.0", "^0")` -> false;
      `semver_satisfies_str("0.0.5", "^0.0")` -> false;
      `semver_satisfies_str("1.5.0", "~1")` -> false (all should be true).
      Confirmed Nov 27 (fid run). Confidence: high.
- [ ] BG313. **JWT `nbf`/`iat`/`exp` written in exponent form are read as tiny
      numbers: a not-yet-valid token is accepted.** stdlib/jose.fd jwt_num
      (used by jwt_check_claims): jwt_num checks the claim is a JSON number,
      then converts it with `str_parse_int(v)`, which stops at the first
      non-digit. A NumericDate is any JSON number (RFC 7519 §2), so `2e9`
      becomes 2 and `1.9e9` becomes 1: `nbf`/`iat` checks then pass for a
      token not valid until 2033, and an `exp` of 1.9e9 makes a valid token
      "expired". Trigger: `jwt_check_claims("{\"exp\": 1800000000, \"nbf\":
      2e9}", &jwt_options(1700000000, "", ""))` -> "" (accepted; should be
      "token not yet valid"); same with `"iat": 3e9` -> "" (should be "token
      issued in the future"). Confirmed Nov 27 (fid run). Confidence: high.
- [ ] BG314. **`dec_parse("1e999999")` spends 37 s of CPU: an 8-byte number
      string is a denial of service.** stdlib/decimal.fd dec_parse: the
      exponent loop stops only once `e >= 100000`, so it accepts exponents up
      to 999,999, and a positive exponent is applied eagerly with
      `bigint_mul(m, dec_pow10(-scale))`, building a million-digit BigInt with
      schoolbook multiplication. Any parser that hands untrusted text to
      dec_parse (a JSON amount, a form field) can be stalled this way. A
      negative exponent costs nothing up front but makes the scale huge, so
      later arithmetic blows up the same way. Trigger: `val d =
      dec_parse("1e999999");` built with `fid ccompile` -> 37,182 ms (1e5000:
      1 ms, 1e20000: 18 ms). Also, contrary to its doc ("anything else is 0"),
      `dec_parse("12abc")` gives 12 and `dec_parse("1.2.3")` gives 1.2.
      Confirmed Nov 27 (C backend, timing above; the "12abc" result on fid
      run). Confidence: high.
- [ ] BG315. **`x509_matches_host` rejects a certificate for an IPv6 host
      written the usual way ("::1", "2001:db8::1").** stdlib/x509.fd
      x509_matches_host (with x509_ip_text): an iPAddress SAN is turned into
      text by x509_ip_text as eight uncompressed groups
      ("2001:db8:0:0:0:0:0:1", "0:0:0:0:0:0:0:1") and compared with
      `str_equal` against the host string. Any host in RFC 5952 form (zero
      runs compressed with "::"), which is how URLs and resolvers write it,
      never matches, so TLS/QUIC (quic_tls.fd uses it for server_name) refuses
      a valid certificate. Trigger: openssl cert with
      `subjectAltName=IP:2001:db8::1,IP:::1`; `x509_matches_host(c,
      "2001:db8::1")` -> false, `x509_matches_host(c, "::1")` -> false; only
      "2001:db8:0:0:0:0:0:1" -> true. Confirmed Nov 27 (fid run, certificate
      made with openssl req -x509). Confidence: high.
- [ ] BG316. **`tar_parse` panics on a header whose base-256 size (or any
      numeric field) does not fit in i64.** stdlib/tar.fd tar_field_num
      (base-256 branch; also tar_dec for pax "size"/"uid"/... and the pax
      record length in tar_pax_parse): the GNU base-256 form is accumulated
      with checked `v = v * 256 + b` over all 11 following bytes (a 12-byte
      size field holds up to 2^95), so a hostile header traps the process
      instead of making tar_parse return `err`. tar_dec and tar_pax_parse's `n
      = n * 10 + ...` overflow the same way on a long digit string. Trigger:
      take `tar_header(&tar_file("a", [65], 420))`, set byte 124 to 0x80 and
      bytes 125..135 to 0xFF, `tar_fix_checksum`, pad, `tar_parse(h)` ->
      "Panic: Integer overflow on multiplication: 72057594037927935 * 256"
      (should return err "truncated data ..." or "bad size"). Confirmed Nov 27
      (fid run). Confidence: high.
- [ ] BG317. **`mail_build` lets body text forge MIME parts: the boundary is
      predictable and never checked against the content.** stdlib/smtp.fd
      mail_build / sm_text_part: boundaries are "=_alt_"/"=_mix_" plus 12 hex
      digits from a xorshift seeded by the `seed` argument (a fixed 2463534242
      when seed is 0), so they are the same for every message built with the
      same seed, and an ASCII body is sent 7bit as it is (the "=" that would
      be escaped is only escaped on the quoted-printable path). Nothing checks
      that the text does not contain "--<boundary>", so user-supplied text (a
      contact form, a comment notification) can close the text part and add
      its own parts (HTML, fake attachments). Trigger: build once to read the
      boundary (=_mix_eebaaa1e0369 for seed 0), then `mail_set_text(&mut m,
      "hello\n--=_mix_eebaaa1e0369\nContent-Type: text/html\n\n<b>forged
      part</b>\n")` and `mail_build(&m, 1700000000, 0, 0)` -> the message has
      a second, forged text/html part before the real attachment. Confirmed
      Nov 27 (fid run, output shows the injected part between the real
      boundaries). Confidence: high.
- [ ] BG318. **OBD-II: `obd_request_to_ecu` sends to the ECU's response ID,
      and requests/decoders omit the ISO 15765-2 length byte.** stdlib/can.fd
      obd_request_to_ecu; obd_engine_rpm_request / obd_vehicle_speed_request /
      obd_coolant_temp_request / obd_read_dtcs and the obd_decode_* functions:
      physical OBD request IDs are 0x7E0..0x7E7 and the ECUs answer on
      0x7E8..0x7EF; obd_request_to_ecu uses `OBD_RESPONSE_BASE + ecu` (0x7E8 +
      ecu), the reply ID, so the request is addressed to no ECU. The request
      frames are also built without the single-frame PCI byte (the module's
      own can_frame_standard example has it: [0x02, 0x01, 0x0C]), so a real
      ECU reads [0x01, 0x0C] as "1 byte: 0x0C"; the decoders likewise expect
      0x41 at byte 0 where a real reply has the length byte (e.g. [0x04, 0x41,
      0x0C, A, B]) and so return -1 for every genuine answer. Trigger:
      `obd_request_to_ecu(0, 1, 0x0C)` -> id 2024 (0x7E8; should be 0x7E0);
      `obd_engine_rpm_request()` -> bytes [1, 12] (should be [2, 1, 12]).
      Confirmed Nov 27 (fid run); the wire-format part checked against ISO
      15765-4 by reading. Confidence: high.
- [ ] BG319. **YAML "billion laughs": `yaml_load` expands aliases with no
      limit, so a few hundred bytes exhaust memory.** stdlib/yaml.fd yaml_load
      (alias handling: "an alias copies the anchored node"): every `*name`
      copies all entries of the anchored node into the flat entry list, with
      no cap on the total number of entries or on alias expansions (grep finds
      no limit/budget near the anchor code). Nested anchors multiply: each
      extra 31-byte line `x: &x [*w, *w, *w, *w, *w, *w]` multiplies the
      output by 6. A config file or API body read with yaml_load can make the
      process run out of memory. Trigger: six lines `a: &a [x,x,x,x,x,x]`, `b:
      &b [*a x6]` ... `f: &f [*e x6]` (180 bytes) -> `yaml_load` ok=true,
      67,182 entries; 10 such lines (~300 bytes) would be about 87 million
      entries. Confirmed Nov 27 (C backend: 180 bytes -> 67,182 entries; fid
      run: 4 levels, 118 bytes -> 1,864 entries). Confidence: high.
- [ ] BG320. **safetensors: a hostile shape panics `st_tensor` ("All access is
      bounds-checked" says the header).** stdlib/safetensors.fd st_tensor
      (also st_elems): the element count is built with checked `total = total
      * d` over the header's shape, and then `total * dsz`; nothing bounds the
      dimensions first, so a model file whose header says shape [2^32, 2^32,
      4] traps the loader instead of returning found=false. Trigger: header
      `{"t":{"dtype":"F32","shape":[4294967296,4294967296,4],"data_offsets":[0,4]}}`
      -> `st_open` ok, `st_tensor(b, s, "t")` -> "Panic: Integer overflow on
      multiplication: 4294967296 * 4294967296". Confirmed Nov 27 (fid run).
      Confidence: high.
- [ ] BG321. **`fid doc` / `fid scribe` exit 0 when the input cannot be read
      (message on stdout).** compiler/main.fd cmd_scribe (compiler/scribe.fd
      process_directory returns "Cannot read ..."): cmd_scribe prints whatever
      process_directory returns and always `return 0`; cmd_veil right below it
      checks the "Cannot read " prefix and exits 1 on stderr, cmd_scribe does
      not (the standalone tools/scribe.fd -> scribe_main does: `if
      scribe_failed(message) { ... return 1; }`, so `scribe.exe` and `fid doc`
      disagree; scripts/stdlib-docs.sh's "fid doc failed" branch can never
      fire). A docs CI step (`fid doc src/ --output site`) with a typo'd path
      passes. Trigger: `fid doc nofile.fd; echo $?` and `fid scribe nofile.fd;
      echo $?` -> stdout "Cannot read nofile.fd", exit 0 (want stderr, exit
      1). Confirmed Nov 27 (fb4 fid.exe: exit=0 out="Cannot read nofile.fd").
      Confidence: high.
- [ ] BG322. **`fid doctest` on a missing file or a mistyped path passes with
      "0 examples", exit 0.** compiler/main.fd cmd_doctest: a non-directory
      input is pushed to `files` unchecked; `c_file_read` of a missing file
      gives "" and scribe_doctests finds nothing, so the summary is "0
      examples | 0 run | 0 failed" and the exit code is 0 (n_fail == 0). A CI
      doctest step pointing at a renamed module silently tests nothing.
      Trigger: `fid doctest nofile.fd; echo $?` -> "doctest: 0 examples | 0
      run | 0 failed | 0 skipped(native)", exit 0 (want "cannot read", exit
      1). Confirmed Nov 27 (fb4 fid.exe). Confidence: high.
- [ ] BG323. **The installer's .fd file association runs `fid.exe "<file>"`,
      which is "Unknown command": double-clicking a .fd never runs it.**
      installer/fides.nsi Section "Associate .fd files" (SecAssoc);
      compiler/main.fd main dispatch (no `.fd`-as-command case): the open
      command is `'"$INSTDIR\fid.exe" "%1"'` (no `run`). main.fd takes the
      first non-flag argument as the command; a path matches none of the
      `command == ...` cases, so it prints "Unknown command: C:\...\x.fd" plus
      the usage and returns 1, and the console window closes at once.
      DESC_Assoc promises "Open .fd files with Fid when double-clicked.".
      Trigger: `fid C:\path\hello.fd` -> stdout "Unknown command:
      C:\...\hello.fd", usage on stderr, exit 1 (the association needs
      `"$INSTDIR\fid.exe" run "%1"`, or main.fd should treat a `.fd` path as
      `run`). Confirmed Nov 27 (fb4 fid.exe with the Windows path of hello.fd:
      exit 1, "Unknown command"). Confidence: high.
- [ ] BG324. **`fid lint` never reports an unused import of a stdlib module
      (`use json;` with no use: "No lint warnings");
      tests/tooling/lint-style-check.sh's "unused import 'string'" step
      fails.** compiler/typechecker.fd TypeChecker_lint_unused_imports, run
      from compiler/compiler.fd build path (`tc.run_lints(program)`) for
      lint_with_imports: a file with `use` goes through lint_with_imports ->
      the real build, which runs the lints on the MERGED program.
      TypeChecker_lint_unused_imports collects `called` from every function of
      `program.funcs`, i.e. also the imported module's own bodies; a module
      whose functions call each other (json, math, time, string, ...) always
      has a called function with its prefix or with
      TypeChecker_get_item_module == the module, so `any_used` is always true.
      AGENTS.md ("lint ... unused imports") and the `fid fix` "remove the
      unused import" fix (lsp.fd fix_for, scripts/tl-check.sh) depend on this
      warning. Trigger: `use json;\n\nfn main() {\n    println(1);\n}` -> `fid
      lint` prints "No lint warnings." (same for `use math;`, `use time;`);
      `fid lint tests/bootstrap/test_style_lint.fd` prints its 5 style
      warnings but not the "warning: unused import: 'string'" that
      lint-style-check.sh line 65 requires. Confirmed Nov 27 (fb4 fid.exe, run
      in the repo root and on copies). Confidence: high.
- [ ] BG325. **LSP rename leaves every use inside an f-string interpolation
      under the old name (locals and functions); references misses them for
      locals.** compiler/lsp.fd lsp_code_occ (used by handle_rename /
      lsp_local_occ), also fix_rename_line (fid fix's snake_case rename):
      lsp_code_occ treats `f"` as the identifier `f` followed by an ordinary
      string: `in_str` is set at the `"` and the whole literal, its `{...}`
      interpolations included, is skipped. So a rename edits the declaration
      and the plain uses but not `f"{count}"` / `f"{total(5)}"`; the result no
      longer compiles (or, with an outer binding of the old name, silently
      reads that one). For a function, textDocument/references DOES list the
      f-string use (another path), so references and rename disagree.
      fix_rename_line has the same skip, which is why `fid fix` refuses ("kept
      ... it would not leave fewer errors") every snake_case rename of a
      variable used in an f-string. Trigger: r.fd = `fn total(a: i64) -> i64 {
      return a + 1; }` / `fn main() { val count = total(2); println(f"count is
      {count} and {total(5)}"); }` (one statement per line). LSP rename at
      `count` (line 5 char 8) -> one edit, line 5 only; references -> line 5
      only. Rename at `total` (line 0) -> edits lines 0 and 5, not line 6;
      references -> lines 0, 5 AND 6. Confirmed Nov 27 (fb4 `fid lsp` over
      stdio with initialize/didOpen/rename/references). Confidence: high.
- [ ] BG326. **`fid profile p.fd -o out` on Windows reports the OLD `out` when
      it already exists (the new profile is left in fides-profile.folded).**
      compiler/main.fd cmd_profile (`p_rename("fides-profile.folded", out)`,
      result ignored); C runtime p_rename is plain `rename()`: Windows
      `rename()` does not replace an existing file (compiler.fd imp_save
      already works around this: "Windows' rename does not replace a file").
      cmd_profile ignores the failure, then reads `out` and prints the report
      and "Collapsed stacks: out" from the stale file. A second profiling run
      with the same -o shows the first run's profile. (The build's
      _fides_profile_tmp.c/.exe are also left in the cwd.). Trigger: fib(30)
      program; `echo "stale_old_profile;main 999" > prof.txt; fid profile p.fd
      -o prof.txt` -> report "1 samples ... 100% main", prof.txt unchanged;
      the real samples (main;fib;fib;...) are in ./fides-profile.folded.
      Confirmed Nov 27 (fb4 fid.exe, CC="zig cc", Windows). Confidence: high.
- [ ] BG327. **`fid qualify -o out.json` writes invalid JSON: every escaped
      string is quoted twice (`"source": ""q.fd""`).** compiler/main.fd
      cmd_qualify (lines ~6055, 6064, 6083) with compiler/json.fd
      json_escape_string: json_escape_string already returns the string WITH
      its surrounding quotes (it appends char 34 first and last), but
      cmd_qualify writes `"\"" + json_escape_string(x) + "\""` for "source",
      each entry of "errors" and each rule's "requirement". The "safety
      evidence" file (the input a certification pipeline is meant to read)
      fails any JSON parser. Trigger: q.fd = an `@crashfree fn safe_add(a:
      i64, b: i64) -> i64 { return a + b; }` called from main; `fid qualify
      q.fd -o q.json` -> "Wrote q.json"; q.json line 4 is `"source":
      ""q.fd"",`; Python json.load: "Expecting ',' delimiter: line 4 column
      15". (Any program: "source" is always written this way.) Confirmed Nov
      27 (fb4 fid.exe + python json). Confidence: high.
- [ ] BG328. **`fid veil --format html` marks lines of never-called user
      functions "covered" (and blank/`}` lines "uncovered"): core.fd's line
      numbers are matched against the user's file.** compiler/veil.fd
      generate_html_report / line_in_set (and run_with_coverage, which since
      AD1 keeps every lowered function, core included; vl_keep_user is never
      called): data.all_lines/covered_lines hold (function, line) entries for
      core's functions as well as the user's; line_in_set compares only the
      line number (`s1 == line_num`, the function `s0` is ignored). So
      whenever core code at line N ran (Vec_push, HashMap_insert, ...), line N
      of the user's source is painted covered, whatever is there; core's
      executable lines make the user's blank lines and `}` "uncovered". The
      report shows dead user code as tested. Trigger: v2.fd = a main that does
      `v.push(1); v.push(2); m.insert("a", 1); println(v.len() + m.len());`
      followed by 300 never-called `fn never_<i>(x: i64) -> i64 { return x +
      <i>; }` (3 lines each). `fid veil v2.fd --format html --output h2` -> 4
      `return x + ..` lines and fn headers (e.g. line 540 `fn never_178`, 843,
      867, 897) have class "covered"; 98 `}` lines have class "uncovered".
      Confirmed Nov 27 (fb4 fid.exe; veil.fd functions identical in the tree).
      Confidence: high.
- [ ] BG329. **`fid run --timeout N` (and the global `--timeout`) is accepted
      and never enforced: an endless program runs forever.** compiler/main.fd
      cmd_run (`run_timeout = get_flag_int(n, start, "--timeout", g_timeout)`
      is only printed under -v); main's global flag loop ("TODO: --timeout
      requires spawning a watcher thread ... we store the value but cannot
      enforce it"): run's usage line advertises `[--timeout N]` next to
      --max-steps/--max-memory, which do stop the program; run_timeout is
      never passed to the interpreter or used for a watchdog. A script relying
      on `fid run --timeout` to bound a test hangs instead (only cmd_reduce
      uses g_timeout). Trigger: loop.fd = `fn main() { var i: i64 = 0; while
      true { i = i + 1; } }` -> `timeout 25 fid run loop.fd --timeout 2` is
      killed by the outer timeout after 25 s (exit 124); `--max-steps 1000` on
      the same file stops at once (exit 1). Confirmed Nov 27 (fb4 fid.exe;
      cmd_run's flag code identical in the tree). Confidence: high.
- [ ] BG330. **The LSP server speaks byte columns where LSP means UTF-16 code
      units: after any non-ASCII character on a line,
      references/rename/diagnostic ranges land in the wrong place.**
      compiler/lsp.fd (all positions: word_at_position, lsp_code_occ/I64Triple
      columns, location_json, fix_pos_json; the initialize result announces no
      `positionEncoding`): the server computes columns with str_byte_at
      offsets into the UTF-8 text and sends them as `character`; it never
      negotiates `positionEncoding` (lsp.fd has no "utf-16"/"positionEncoding"
      at all), so clients use the default UTF-16. On a line with `"héé"`
      before a name, the range is 2 units too far right per 2-byte character;
      a rename then rewrites the wrong characters (here `ame;` instead of
      `name`), and an incoming cursor position is read as a byte offset.
      Trigger: u.fd line 3 = `    val s = "héélo" + name;` (UTF-8). references
      at `name` (line 1 char 9) -> line 2 range 25..29; the UTF-16 column of
      `name` is 23 (byte column 25). Confirmed Nov 27 (fb4 `fid lsp` over
      stdio). Confidence: high.
- [ ] BG331. **Any `--` on a fid command line restarts command dispatch after
      it: `fid watch f.fd run -- args` (watch's documented form) and `fid run
      tools/cas.fd -- root port` (cas.fd's documented usage) both fail
      "Unknown command".** compiler/main.fd main (the "Two invocation modes"
      loop: the first `--` anywhere sets `start = i + 1`); cmd_watch (`[--
      <args>]` in its usage, parses `seen_dd`); tools/cas.fd header: the loop
      that lets `fidesc run main.fd -- ccompile ...` run the compiler under
      the interpreter scans ALL of argv for `--`, so in the native fid a `--`
      meant for the command (watch's extra arguments, a program's arguments)
      makes the word after it the command. cmd_watch's `--` handling can never
      be reached. (`fid run x.fd a b` passes arguments; with `--` it breaks.).
      Trigger: args.fd prints arg_at(0..3). `fid run args.fd alpha beta` ->
      "args.fd alpha beta -"; `fid run args.fd -- alpha beta` -> "Unknown
      command: alpha" + usage, exit 1; `fid watch args.fd run -- alpha` ->
      "Unknown command: alpha"; `fid run cas.fd -- croot 7391` -> "Unknown
      command: croot". Confirmed Nov 27 (fb4 fid.exe; main's loop identical in
      the tree). Confidence: high.

### Low

- [ ] BG332. **A float exponent with no digits is accepted: `1.5e`, `2.0e+`,
      `3.25E-` are floats.** compiler/lexer.fd Lexer_read_number (fraction
      branch: after 'e'/'E' and an optional sign the digit loop may consume
      nothing): reference §1.6: the exponent is `e`/`E`, an optional sign and
      digits, and for `1e` the lexer already requires digits (2g.18). After a
      fraction it does not, so `1.5e` is 1.5 and `1.5e-` swallows the `-`.
      Trigger: `fn main() -> i64 { val a = 1.5e; println(a); val b = 2.0e+;
      println(b); return 0; }` -> prints 1.5 and 2; should be a lex error.
      Confirmed Nov 27 (fid run). Confidence: high.
- [ ] BG333. **`\u{...}` escapes are not checked to be a Unicode scalar value:
      `"\u{}"`, `"\u{D800}"`, `"\u{FFFFFF}"`, `'\u{110000}'` are accepted.**
      compiler/lexer.fd Lexer_read_hex_value (until_brace: no minimum digit
      count, no range check) and its callers in the string / char literal
      readers (~302, ~525, ~613): reference §1.7: a char literal's value is
      the Unicode scalar value. An empty `\u{}` is a zero byte, a surrogate or
      a value above 0x10FFFF is encoded as is, so string literals hold invalid
      UTF-8 (`"\u{D800}"` is 3 bytes, `"\u{FFFFFF}"` 4) and `'\u{110000}'` is
      1114112. Side note in the same lexer: the "unknown escape sequence \'"
      error for a string literal says in its Fix line that `\'` is one of the
      escapes. Trigger: `fn main() -> i64 { println('\u{110000}');
      println(str_len("\u{D800}")); println(str_len("\u{}"));
      println(str_len("\u{FFFFFF}")); return 0; }` -> 1114112, 3, 1, 4; each
      escape should be a lex error. Confirmed Nov 27 (fid run). Confidence:
      high.
- [ ] BG334. **Any function or method named `main` is exempt from the
      missing-return check: `impl App { fn main(self) -> str { if .. { return
      "pos"; } } }` compiles.** compiler/typechecker.fd
      TypeChecker_check_func_body (`if returns_value && func.name != "main" &&
      !stmts_always_leave(func.body)`, ~13481) tests the bare name only, not
      "the program's entry function": reference §2.1 exempts `main` (the entry
      point) only; the test also exempts a method `App.main` (and a `main` in
      an imported module), whose fall-through path returns no value. Trigger:
      `struct App { n: i64 } impl App { fn main(self) -> str { if self.n > 0 {
      return "pos"; } } } fn main() -> i64 { val a = App { n: -1 };
      println(a.main()); return 0; }` -> run prints `void`, ccompile prints an
      empty line; should be E047. Confirmed Nov 27 (fid run, ccompile).
      Confidence: high.
- [ ] BG335. **A struct literal may name the same field twice; the later value
      silently wins.** compiler/typechecker.fd struct-literal check
      (Expr::StructLit, ~21540-21560: each field init is type- and fit-checked
      against its declaration, duplicates are never looked for); parser.fd
      parse_struct_lit_expr: `P { x: 1, x: 5, y: 2 }` is accepted and builds x
      = 5; a typo (`x` written for `y`) goes unreported, the more so because a
      field left out is not reported either (§2.2). Trigger: `struct P { x:
      i64, y: i64 } fn main() -> i64 { val p = P { x: 1, x: 5, y: 2 };
      println(p.x); return 0; }` -> run and ccompile print 5; should be "field
      'x' given twice". Confirmed Nov 27 (fid run, ccompile). Confidence:
      high.
- [ ] BG336. **`keys()` of a `HashMap<u64, V>` is ordered as signed: keys at
      or above 2^63 come before 1 (reference/semantics-2 §20.2 M-order:
      integer keys ascending by value).** stdlib/core.fd HashMap_keys (integer
      keys sorted with `Vec_sort(result)`, an i64 heapsort; HashMap_values
      follows the same order): the key words are compared as i64, so u64 keys
      >= 2^63 sort as negatives. No unsigned case exists. Same on every
      backend. Trigger: `fn idu64(x: u64) -> u64 { return x; } fn main() { var
      mu: HashMap<u64, i64> = HashMap::new();
      mu.insert(idu64(18446744073709551615), 1); mu.insert(idu64(1), 2);
      mu.insert(idu64(9223372036854775808), 3); var ku = ""; for k in
      mu.keys() { ku = ku + f"{k},"; } println(ku); }` -- prints
      `9223372036854775808,18446744073709551615,1,` on run, ccompile, emit,
      --nat (want `1,9223372036854775808,18446744073709551615,`). Confirmed
      Nov 27 (all four). Confidence: high.
- [ ] BG337. **An integer converted to f32 is rounded twice (to f64, then to
      f32) on run, C and --nat; only pe_ir rounds once as semantics-2 §10
      F32-cast requires.** compiler/codegen_c.fd gen_inst `Inst::IntToFloat`
      (`dst = (double)v;`, the f32 destination then narrows the double);
      compiler/nat_ir.fd ng_i2f_u (`ng_cvt_i2d` = cvtsi2sd, then
      `ng_x64_fstore0(.., to_f32)` narrows); the interpreter converts through
      f64 the same way: for an integer above 2^53 the double rounding can land
      on the other f32 neighbour. n = 2^54 + 2^30 + 1 must round to 2^54 +
      2^31 (the remainder 2^30 + 1 is above half the f32 spacing 2^31);
      through f64 it first becomes 2^54 + 2^30, an exact tie, which rounds to
      even: 2^54. Trigger: `fn id64(x: i64) -> i64 { return x; } fn main() {
      val f: f32 = id64(18014399583223809) as f32; println(f"{(f as f64) as
      i64}"); }` -- run, ccompile, --nat print 18014398509481984; emit prints
      18014400656965632 (correct). Same for u64. Confirmed Nov 27 (ng_i2f_u
      identical in the tree; the C line is the same in both trees).
      Confidence: high.
- [ ] BG338. **`p_str_to_int` wraps on overflow on the machine layer (`--nat`,
      Linux targets) where run/C/pe_ir saturate.**
      stdlib/linux_runtime/base.fd pert_str_to_int: `v = v * 10 + digit` with
      no overflow test (the runtime is unchecked), and `0 - v` for the sign;
      the C runtime uses atoll, pe_ir _strtoi64 and the interpreter saturating
      conversion, which clamp to i64::MAX/MIN. Trigger: `fn main() {
      println(f"{p_str_to_int("9223372036854775808")}
      {p_str_to_int("-9223372036854775809")}
      {p_str_to_int("99999999999999999999")}"); }` -> run/C/emit:
      `9223372036854775807 -9223372036854775808 9223372036854775807`; emit
      --nat: `-9223372036854775808 9223372036854775807 7766279631452241919`.
      Confirmed Nov 27 (all four backends). Confidence: high.
- [ ] BG339. **`f"{x:.N}"` rounds the double twice, so values just below a
      half round up (0.0055 -> "0.006").** stdlib/core.fd fstr_fmt_f64: the
      fraction is `((av - ip) * 10^prec + 0.5) as i64`: the multiply is
      rounded to a double before the +0.5, so a value whose exact binary value
      is below the half-way point (0.0055 is 0.00549999...) can become exactly
      x.5 and round up. printf("%.3f") / Rust / JS toFixed give "0.005". About
      6% of the 3-decimal half-way values from 0.0005 to 20 are wrong in a
      brute-force check of the same formula. Trigger: `fn main() {
      println(f"[{0.0055:.3}] [{0.0195:.3}]"); }` -> `[0.006] [0.020]`;
      correctly rounded is `[0.005] [0.019]`. Confirmed Nov 27 (run, ccompile,
      emit, emit --nat all print 0.006 / 0.020). Confidence: high.
- [ ] BG340. **`--nat` Windows file primitives disagree with pe_ir/C/run on
      the same machine: `p_file_exists` of a directory is true and `p_rename`
      onto an existing file replaces it.** stdlib/linux_runtime/base.fd
      pert_file_exists (pe_open of a directory succeeds: win.fd pe_win_open ->
      pe_win_open_dir); stdlib/linux_runtime/win.fd pe_win_syscall renameat2
      (MoveFileEx with flags 3 = REPLACE_EXISTING|COPY_ALLOWED): the C runtime
      and pe_runtime test existence with fopen("rb") (false for a directory on
      Windows) and rename with rename()/MoveFile (fails when the target
      exists); the machine layer's Windows shim emulates Linux, so stdlib
      fs.fd `fs_rename` silently overwrites the target only on --nat. Trigger:
      `fn main() { p_mkdir("d"); println(f"{p_file_exists("d")}");
      p_file_write("a.txt", "22"); p_file_write("b.txt", "1");
      println(f"{p_rename("a.txt", "b.txt")} {p_file_size("b.txt")}"); }` ->
      run/C/emit: `false`, `-1 1`; emit --nat: `true`, `0 2`. Confirmed Nov 27
      (all four backends, Windows). Confidence: high.
- [ ] BG341. **`char_to_str` / `StringBuilder.append_char` of a non-ASCII
      character literal keep only its low byte (invalid UTF-8).**
      stdlib/core.fd char_to_str, byte_to_str, StringBuilder::append_char (all
      p_char_to_str): reference.md §1.7 makes `'c'` the Unicode scalar value
      (`'€'` is 8364), but char_to_str and append_char pass it to
      p_char_to_str, which stores `c & 255` as one byte on every backend:
      `char_to_str('€')` is the single byte 0xAC, `append_char('é')` appends
      0xE9, neither is valid UTF-8 nor equal to the literal "€"/"é". Nothing
      rejects or encodes a value above 127. Trigger: `fn main() { val e: i64 =
      '€'; val s: str = char_to_str(e); println(f"{e} {str_len(s)}
      {str_byte_at(s, 0)} {s == "€"}"); }` -> `8364 1 172 false`; want a
      3-byte UTF-8 string equal to "€" (or a rejection of values above 255).
      Confirmed Nov 27 (identical on run, ccompile, emit, emit --nat).
      Confidence: high.
- [ ] BG342. **BG175's siblings: `str_parse_int_range` panics on a long digit
      run and `str_parse_hex` silently wraps past 16 digits.** stdlib/core.fd
      str_parse_int_range, str_parse_hex: BG175 made str_parse_int return 0
      (its documented failure) for a value outside i64. str_parse_int_range
      still does `result = result * 10 + (b - 48)` under checked arithmetic,
      so 20 digits panic "Integer overflow on multiplication" (the lexer-style
      helper is public, "Returns 0" is not said but a panic in a parser is the
      BG175 class). str_parse_hex is `@wrapping` and keeps shifting:
      "1ffffffffffffffff" (17 digits) gives -1 and "10000000000000000" (2^64)
      gives 0, though its doc says "Returns 0 on failure", with no way to tell
      overflow from a value. Trigger: `fn main() {
      println(f"{str_parse_int("99999999999999999999")}");
      println(f"{str_parse_hex("1ffffffffffffffff")}
      {str_parse_hex("10000000000000000")}");
      println(f"{str_parse_int_range("99999999999999999999", 0, 20)}"); }` ->
      `0`, `-1 0`, then `Panic: Integer overflow on multiplication` in
      str_parse_int_range. Confirmed Nov 27 (run, ccompile, emit, emit --nat
      identical). Confidence: high.
- [ ] BG343. **A `HashMap<Vec<T>, V>` keys by the Vec handle's address, so two
      equal Vecs are two keys.** stdlib/core.fd hm_hash / hm_eq (a Vec key is
      neither an int key, a str key nor a struct key: compared as a word);
      compiler/lower.fd Lowerer_map_has_int_keys: `==` on two Vecs compares
      their elements, but the map hashes and compares a Vec key as its handle
      word, so `m.insert(a, 1); m.insert(b, 2)` with `a == b` gives two
      entries, and a lookup with an equal Vec built elsewhere misses. Nothing
      rejects Vec keys. (Tuple, struct and enum keys compare by value.).
      Trigger: `fn main() { var m: HashMap<Vec<i64>, i64> = HashMap::new();
      var a: Vec<i64> = Vec::new(); a.push(1); var b: Vec<i64> = Vec::new();
      b.push(1); println(f"{a == b}"); m.insert(a, 1); m.insert(b, 2);
      println(f"{m.len()}"); }` -> `true` then `2`; want `true`, `1`.
      Confirmed Nov 27 (run, ccompile, emit, emit --nat print the same: len
      counts equal Vecs twice). Confidence: high.
- [ ] BG344. **`int_to_str_padded` puts the zeros in front of the minus sign:
      `int_to_str_padded(-7, 4)` is "00-7".** stdlib/string.fd
      int_to_str_padded (str_pad_left of int_to_str(n) with '0'): the doc says
      "with a minimum width (zero-padded)"; padding the text "-7" on the left
      with '0' gives "00-7", which is not a number (str_parse_int("00-7") is
      0). f"{n:04}" (core __fstr_pad_zero) handles the sign correctly
      ("-007"). Trigger: `use string; fn main() {
      println(int_to_str_padded(-7, 4)); }` -> `00-7`; want `-007`. Confirmed
      Nov 27 (run, ccompile, emit, emit --nat). Confidence: high.
- [ ] BG345. **`StrView.parse_int` panics on a long digit run and on i64::MIN
      instead of returning its fallback / the value.** stdlib/string.fd
      StrView::parse_int: the value is built positive with checked `n = n * 10
      + (c - 48)`, so 20 digits panic "Integer overflow on multiplication",
      and "-9223372036854775808" panics too (9223372036854775808 does not fit
      before the sign is applied); the doc promises "the decimal integer the
      view holds ... or `fallback`". BG175 fixed the same pattern in core's
      String_parse_int. Trigger: `use string; fn main() {
      println(f"{str_view_all("99999999999999999999").parse_int(-1)}"); }` ->
      `Panic: Integer overflow on multiplication` in StrView_parse_int (run,
      ccompile, --nat); want `-1`. Same panic for
      `str_view_all("-9223372036854775808").parse_int(-1)` (want i64::MIN).
      Confirmed Nov 27 (run, ccompile, emit --nat; emit cannot build the
      probe, see BG307). The MIN case: Read only (same
      loop), confidence high.
- [ ] BG346. **unicode.fd's encoder writes invalid UTF-8 for surrogates and
      values past U+10FFFF, and `uni_len` counts invalid input differently
      from `uni_to_codepoints`.** stdlib/unicode.fd uni_encode_cp, uni_len:
      after BG215 the decoder (uni_to_codepoints) turns each invalid byte into
      U+FFFD, but uni_encode_cp only maps cp < 0 to U+FFFD: a surrogate
      (U+D800 -> ED A0 80) and 0x110000 (-> F4 90 80 80) are written as byte
      sequences the module's own decoder rejects (3 and 4 replacement
      characters), and cp >= 0x200000 writes a 0xF8 lead byte. uni_len
      ("Number of codepoints") steps by the lead byte without validating, so a
      stray continuation byte counts as the lead of a 2-byte sequence and a
      truncated sequence as one codepoint. Trigger: `use unicode; fn main() {
      var v: Vec<i64> = Vec::new(); v.push(128); v.push(128); v.push(97); val
      bad: str = str_from_bytes(v); println(f"{uni_len(bad)}
      {uni_to_codepoints(bad).len()}");
      println(f"{uni_to_codepoints(uni_encode_cp(1114112)).len()}
      {uni_to_codepoints(uni_encode_cp(55296)).len()}"); }` -> `2 3` and `4
      3`; want equal counts, and `1 1` (U+FFFD) for the encoder's invalid
      inputs. Confirmed Nov 27 (run, ccompile, emit, emit --nat identical).
      Confidence: high.
- [ ] BG347. **`semver_compare` returns -31 (not -1/0/1) for alphanumeric
      pre-release identifiers.** stdlib/semver.fd sv_pre_compare: for two
      non-numeric identifiers it returns `str_compare(xa, xb)` as it is, which
      is a byte difference, although semver_compare's doc promises "-1 if a <
      b, 0 if equal precedence, 1 if a > b". A caller testing
      `semver_compare(a, b) == -1` gets the wrong answer. Trigger:
      `semver_compare_str("1.0.0-Beta", "1.0.0-alpha")` -> -31. Confirmed Nov
      27 (fid run). Also: numeric identifiers above i64 compare equal
      (`"1.0.0-99999999999999999999"` vs `"...98"` -> 0) because they go
      through str_parse_int; SemVer compares them numerically at any size.
      Confidence: high.
- [ ] BG348. **`fx_parse` overflows on Q31 input with 11 decimals and returns
      a wrong value.** stdlib/fixed_point.fd fx_parse: the fraction loop keeps
      up to 11 digits (`den < 100000000000`), then computes `num << frac`;
      with frac = 31 and an 11-digit `num` the shift exceeds 2^63 and wraps,
      so the fraction is garbage. Trigger: `fx_parse("0.12345678901", 31)` ->
      80653995 (the 9- and 10-digit inputs give the right 265121436).
      Confirmed Nov 27 (fid run). Confidence: high.
- [ ] BG349. **`x509_matches_host` lets a DNS SAN wildcard match an IPv4
      address literal.** stdlib/x509.fd x509_matches_host / x509_label_match:
      the host is tried against every dNSName whatever it is; RFC 6125 §6.2.1
      / RFC 9525 say an IP address is matched only against iPAddress SANs.
      x509_label_match treats "1.0.0.1" as labels, so a dNSName "*.0.0.1" (or
      "1.0.0.1" as a DNS name) authenticates the IP 1.0.0.1. Trigger: same
      certificate with `DNS:*.0.0.1`; `x509_matches_host(c, "1.0.0.1")` ->
      true (should be false: no iPAddress SAN 1.0.0.1). Confirmed Nov 27 (fid
      run). Confidence: high.
- [ ] BG350. **`mime_param` splits quoted parameter values at ';' and returns
      a half-quoted value.** stdlib/mime.fd mime_param: the parameter list is
      cut with `str_split(params, ";")` before quotes are looked at, so a
      quoted-string value holding ';' (legal per RFC 2045/7231) is broken in
      two: the first half keeps its opening quote (the closing-quote test
      fails), and the second half can be read as a separate parameter (`a="x;
      name=evil"` makes `name` return `evil"`). Backslash escapes inside
      quotes are not decoded either. Trigger: `mime_param("text/plain;
      charset=\"utf-8\"; name=\"a;b.txt\"", "name")` -> `"a` (should be
      `a;b.txt`); `mime_param("text/plain; name=\"a\\\"b\"", "name")` ->
      `a\"b` (should be `a"b`). Confirmed Nov 27 (fid run). (multipart.fd's
      own mp_header_param handles both correctly.). Confidence: high.
- [ ] BG351. **`h_unescape` turns `&#0;`, surrogates and code points above
      U+10FFFF into NUL / invalid UTF-8.** stdlib/html.fd h_unescape: numeric
      character references are UTF-8 encoded as given; the HTML standard maps
      0, U+D800..U+DFFF and values above 0x10FFFF to U+FFFD. Here `&#0;`
      yields a NUL byte, `&#xD800;` the bytes ED A0 80 and `&#x110000;` F4 90
      80 80, neither valid UTF-8. Trigger:
      `h_unescape("&#0;&#x110000;&#xD800;")` -> bytes 00, F4 90 80 80, ED A0
      80 (od -c of the output). Confirmed Nov 27 (fid run). Confidence: high.
- [ ] BG352. **`json_path` treats a non-numeric segment on an array as index
      0.** stdlib/json.fd json_path: for an array the segment goes through
      `str_parse_int(seg)`, which gives 0 for text that is not a number, so a
      path that names a key on an array (a schema mismatch) silently returns
      the first element instead of "". Trigger: `json_path("[10,20]", "x")` ->
      "10" (should be ""). Confirmed Nov 27 (fid run). Confidence: high.
- [ ] BG353. **safetensors: a tensor with a zero-length dimension is
      rejected.** stdlib/safetensors.fd st_tensor: every dimension must be `>
      0` (`if d <= 0 { return t; }`), but the format allows empty tensors
      (shape [0, 4] with data_offsets [a, a]); such entries, which real
      checkpoints contain, read as missing/malformed. Trigger: header
      `{"z":{"dtype":"F32","shape":[0,4],"data_offsets":[0,0]}}` ->
      `st_tensor(...).found` false (should be true, 0 elements). Confirmed Nov
      27 (fid run). Confidence: high.
- [ ] BG354. **WebSocket client handshake check is lax: any "101" in the reply
      and a case-folded Sec-WebSocket-Accept pass, while a valid header
      written without the single space fails.** stdlib/ws.fd ws_handshake: the
      reply is accepted when `str_contains(resp, "101")` (anywhere: a
      "Content-Length: 101" on a 200 reply qualifies) and
      `str_contains(str_to_lower(resp), "sec-websocket-accept: " +
      str_to_lower(accept))`. Lower-casing both sides makes the base64 accept
      value case-insensitive (RFC 6455 §4.1 requires the exact value), and the
      literal ": " means a correct `Sec-WebSocket-Accept:<value>` or one with
      two spaces (OWS is optional and variable) is refused. The status line
      itself and the Upgrade/Connection headers are never checked. Trigger: a
      server answering `HTTP/1.1 200 OK\r\nContent-Length:
      101\r\nSec-WebSocket-Accept: <accept, any case>\r\n\r\n` is taken as an
      upgraded WebSocket; a real server sending
      `Sec-WebSocket-Accept:<accept>` is rejected. Read only (needs a socket
      peer). Confidence: high (the two str_contains tests are the whole
      check).
- [ ] BG355. **`asn1_get_time` accepts impossible dates (Feb 31, Apr 31) and
      normalises them, and GeneralizedTime without seconds.** stdlib/asn1.fd
      asn1_get_time (used for X.509 notBefore/notAfter and CRL times): the day
      is only checked against 1..31, never against the month, so
      "20250231000000Z" is accepted as 2025-03-03; the seconds are optional
      for GeneralizedTime too, although DER/RFC 5280 §4.1.2.5.2 require
      YYYYMMDDHHMMSSZ. A malformed validity date is silently read as a
      different date instead of rejecting the certificate. Trigger:
      GeneralizedTime "20250231000000Z" -> Some(1740960000), the same as
      "20250303000000Z"; UTCTime "250431000000Z" -> Some(1746057600) (May 1);
      GeneralizedTime "202503030000Z" (no seconds) -> Some(1740960000).
      Confirmed Nov 27 (fid run; elements built with asn1_enc_tlv and read
      with asn1_read_der). Confidence: high.
- [ ] BG356. **`RingBuffer::new(0).push(x)` panics with modulo by zero.**
      stdlib/collections.fd RingBuffer::push: push computes `(self.head +
      self.len) % self.capacity` before testing whether the buffer is full, so
      a zero-capacity ring (try_push and is_full handle it) traps; the doc
      says push "overwrites the oldest element when full". Trigger: `var r =
      RingBuffer::new(0); r.push(1);` -> "Panic: Modulo by zero" (try_push
      returns false, is_full true). Confirmed Nov 27 (fid run). Confidence:
      high.
- [ ] BG357. **`cookie_domain_match` suffix-matches IP addresses, so a cookie
      for "168.1.5" is sent to 192.168.1.5.** stdlib/cookie.fd
      cookie_domain_match: RFC 6265 §5.1.3 allows the "host ends with ." +
      domain" form only when the host is a host name, not an IP address; the
      function applies the suffix rule to any string, so a Domain attribute
      naming the tail of an address matches many hosts (10.0.0.1, 20.0.0.1,
      ... for "0.0.1"). Trigger: `cookie_domain_match("0.0.1", "10.0.0.1")` ->
      true; `cookie_domain_match("168.1.5", "192.168.1.5")` -> true (both
      should be false; only exact equality applies to IPs). Confirmed Nov 27
      (fid run). Confidence: high.
- [ ] BG358. **`fid fmt -w` reports "formatted", exit 0, when the file could
      not be written.** compiler/main.fd cmd_fmt (write_mode branch):
      `p_file_write(input, formatted);` result is ignored; the message and
      `return 0` follow regardless. A pre-commit hook running `fid fmt -w` on
      a read-only (or locked, or on a full disk) file believes it was
      formatted. Trigger: `cp f.fd ro.fd; attrib +R ro.fd; fid fmt ro.fd -w;
      echo $?` (f.fd unformatted) -> "ro.fd: formatted", exit 0, ro.fd
      byte-identical to before (want an error, exit 1). Confirmed Nov 27 (fb4
      fid.exe; cmp shows the file unchanged). Confidence: high.
- [ ] BG359. **After a stage-0 pin update, bootstrap.sh keeps the previously
      fetched stage 0 without a warning (it checks the binary against the pin
      copied beside it, not the repo's pin).** scripts/bootstrap.sh stage-0
      selection (`PIN="$PINNED.sha256"; [ -f "$PIN" ] ||
      PIN="scripts/stage0.sha256"`); scripts/stage0-fetch.sh (`cp "$pin"
      "$canon.sha256"`, early "already matches pin" exit): stage0-fetch.sh
      copies the pin it verified to release/stage0/fid(.exe).sha256.
      bootstrap.sh, when release/stage0/fid exists, never calls stage0-fetch
      again and compares the binary with that COPIED pin, which always
      matches. When scripts/<asset>.sha256 is updated in git (as in 0cec7477
      "pin the macOS stage-0 seeds"), every machine and cached CI runner that
      fetched before keeps building from the old stage 0 and the "does not
      match ... pin it again" warning never fires. Also on Linux/macOS,
      stage0-fetch's early `exit 0` ("already matches pin") checks only
      release/stage0/fid-<os>-<arch>, not the canonical release/stage0/fid it
      would copy, so a missing canonical file stays missing and bootstrap then
      runs a nonexistent STAGE0. Trigger: fetch a stage 0, then change
      scripts/fid-linux-x64.sha256 (or stage0.sha256) to another hash and run
      bootstrap.sh -> "stage 0: release/stage0/fid", no warning. Read only.
      Confidence: high.
- [ ] BG360. **scribe names every typed constant with a trailing colon
      (`LIMIT:`), in headings, anchors and the contents; the shipped
      docs/stdlib pages have them (35 pages).** compiler/scribe.fd
      extract_name_after (called by scribe_parse_const with prefix "const "):
      the name ends at `( < { space ;` only, not at `:`; a constant is always
      written `const NAME: T = v;`, so the name comes out as "NAME:". The item
      is titled `LIMIT:`, its anchor is `id="LIMIT:"` / `href="#LIMIT:"`, and
      the search index carries the colon. docs/stdlib (generated by
      scripts/stdlib-docs.sh) has `## ARINC_PARTITION_NORMAL:` etc. in 35
      module pages. Trigger: `/// The limit.\npub const LIMIT: i64 = 10;` ->
      `fid scribe m.fd --output o` -> `<section id="LIMIT:"><h3>LIMIT:</h3>`;
      `grep -rlE "^#+ [A-Z_0-9]+:$" docs/stdlib/*.md | wc -l` -> 35. Confirmed
      Nov 27 (fb4 fid.exe; scribe.fd identical in the tree). Confidence: high.
- [ ] BG361. **`fid doc` ("as scribe") ignores scribe's `-o`/`-f` and any
      second input, writing HTML into ./docs/ instead.** compiler/main.fd
      cmd_scribe (get_flag_value "--format"/"--output" only) vs
      compiler/scribe.fd scribe_main (`--format`|`-f`, `--output`|`-o`,
      "Unknown argument" error): the usage line says `fid doc <file.fd|dir>
      ... Generate documentation (as scribe)`, and scribe.exe takes `-o dir -f
      markdown`; cmd_scribe silently drops them and uses the defaults (html,
      `docs`). Run from a project root this writes into the project's own
      docs/ directory. Trigger: `fid doc m.fd -o outx -f markdown` ->
      "Generated docs/m.html (2 items)", no outx/ (scribe.exe m.fd -o outx -f
      markdown writes outx/m.md). Confirmed Nov 27 (fb4 fid.exe). Confidence:
      high.
- [ ] BG362. **`fid run` panic traces (and the --crash-file report `fid
      inspect` prints) put each function next to the line of its CALLER: "c at
      line 6" when line 6 is in b.** compiler/interp.fd
      Interpreter_format_call_stack: call_site_lines[i] is the line in frame
      i-1 where frame i was called; the formatter prints frame i's name with
      that line, so every line in the trace is shifted one frame up: the
      innermost function gets the line of its call in the caller, each caller
      gets the line of its own caller's call, and main gets none. A user jumps
      to `line 6` of c and finds b's body. Trigger: c(x) = `return 10 / x;`
      (line 2), b calls c at line 6, a calls b at line 10, main calls a at
      line 15. `fid run cr2.fd` -> "Panic: Division by zero at line 2 / → c at
      line 6 / ↳ called from b at line 10 / ↳ called from a at line 15 / ↳
      called from main" (want c at line 2, b at line 6, a at line 10, main at
      line 15). The crash report written by `--crash-file` and shown by `fid
      inspect` carries the same lines. Confirmed Nov 27 (fb4 fid.exe,
      interpreter). Confidence: medium that it is unintended (the lines are
      consistently call sites; the `→ c at line 6` form still reads as a line
      in c, and main never gets its line).
- [ ] BG363. **tools/cas.fd (the .fidcache remote cache server) cannot be
      started the way it says (`fid run tools/cas.fd`): the interpreter has no
      p_co_accept.** tools/cas.fd header ("fid run tools/cas.fd -- [root]
      [port]"); compiler/interp.fd (no p_co_accept FFI); stdlib/http.fd
      http_accept_task: besides BG331, under `fid run` the
      server prints "fidcas: serving ..." and then the accept task panics "FFI
      function 'p_co_accept' ... is not available in the interpreter", so
      every client gets connection refused while the banner says it is
      serving. Built with `fid ccompile tools/cas.fd`, it works (a PUT/GET of
      a binary with NUL bytes round-trips exactly). Trigger: `fid run cas.fd
      croot 7391` -> stdout "fidcas: serving croot on http://0.0.0.0:7391",
      stderr "Panic: FFI function 'p_co_accept' ... at line 1039 / →
      http_accept_task"; `curl http://127.0.0.1:7391/health` -> rc 7
      (refused). Confirmed Nov 27 (fb4 fid.exe; the ccompile'd server checked
      too). Confidence: high.
- [ ] BG364. **`fid fmt` / `fmt -w` / `fmt --check` never refuse a file that
      does not lex or parse: `-w` rewrites it ("formatted", exit 0); the LSP
      formatting request has not even the token guard.** compiler/main.fd
      cmd_fmt (no parse/lex error check before format_source; only
      fmt_same_tokens); compiler/lsp.fd handle_formatting (format_source
      result sent as a whole-document edit, no fmt_same_tokens check): an
      editor's format-on-save in the middle of an edit (unterminated string,
      unclosed call) reindents the half-written file and reports success;
      `--check` says "would reformat" rather than an error, so a CI gate
      reports a formatting problem for a file that does not compile.
      handle_formatting skips the token comparison cmd_fmt does, so any
      formatter defect the guard catches on the command line (the
      BG101-BG107/BG174 class) is applied silently by the LSP. Trigger:
      bad2.fd = `fn main() {` / `  val s = "unterminated;` / `  if x { foo(` /
      `}` / `fn other() { return 1; }`; `fid check bad2.fd` -> E001
      unterminated string; `fid fmt bad2w.fd -w` (a copy) -> "bad2w.fd:
      formatted", exit 0, line 2 reindented. Confirmed Nov 27 for the CLI (fb4
      fid.exe); the LSP part Read only (handle_formatting code). Confidence:
      high.
- [x] BG365. **release.yml's Linux archive is built without FIDES_VERSION, so
      a manual (workflow_dispatch) release names a different version than its
      Windows installer.** .github/workflows/release.yml job `linux`, step
      "Release bootstrap, 3 stages, fixpoint checked (A10)" (env has CC and
      RELEASE only); scripts/build-info.sh version order: the windows job
      passes `FIDES_VERSION: ${{ steps.version.outputs.version }}` (from the
      dispatch input or the tag) to bootstrap.sh; the linux job computes no
      version and passes none, so build-info.sh falls back to a VERSION file
      or `git describe --tags --exact-match HEAD`. On a manual run with
      `version: 1.0.1` (no tag on HEAD) the installer's fid says 1.0.1 and the
      archive's `bin/fid version` says "1.0.0+dev"; both are published
      together by the publish job. Trigger: compare the env blocks of the two
      "Release bootstrap" steps in release.yml (windows: FIDES_VERSION set;
      linux: none). Read only. Confidence: high.
      Fixed Nov 27 (release.yml: the linux job has the windows job's
      Version step and passes FIDES_VERSION to bootstrap.sh). Not run: the
      workflow runs only on a v* tag or a manual dispatch.
- [ ] BG366. **The installer lets the user untick "Standard Library" (fid then
      cannot build anything) and an update over an older install keeps the
      stdlib modules the new release removed.** installer/fides.nsi Section
      "Standard Library" SecStdlib (no `SectionIn RO`, unlike SecCore),
      `SetOutPath "$INSTDIR\stdlib"` + `File /r "..\stdlib\*.fd"` with no
      prior `RMDir /r`: core.fd is required by every command (compiler.fd
      stdlib_dir looks for <exe dir>/stdlib/core.fd); the components page
      offers the section as optional, so an install without it is a fid.exe
      that fails every build. On an update (release.yml itself tests
      installing over the previous release), File /r only adds/overwrites:
      modules deleted or moved since (e.g. kernel.fd/uefi.fd, moved to
      fides-os) stay in $INSTDIR\stdlib and keep resolving `use kernel;`
      against stale code; the uninstaller's RMDir /r is the only cleanup.
      Trigger: install with the "Standard Library" component unticked, then
      `fid run hello.fd` (no core.fd); or install over an older release and
      look for modules in $INSTDIR\stdlib that the new release no longer has.
      Read only. Confidence: high.

## Found by the Nov 27 whole-repo review, second run

Found later on Nov 27 by six more review passes. Each covered an area the
first run touched lightly or not at all:
- the deeper checkers: traits, generics, safety annotations, sandbox,
  locks, patterns and `?`;
- the backends on features other than f32 and narrow integers: arrays,
  spawn, default and named arguments, variadics, the optimizer switches,
  wasm;
- the system builtins of the four runtimes: files, environment,
  processes, stdin, and leaks in common patterns;
- stdlib crypto, encodings and compression;
- stdlib protocols and domain modules: network, industrial, databases,
  formats, math, physics, media;
- the remaining commands and tools: `fid test`, ark, repl, audit,
  notebook, LSP hover, fmt, scripts.

Same rules as the first run:
- **New only.** Every pass checked its findings against BG1-BG366 and
  known-bugs.md §2g. Where two passes found the same bug, it is one entry
  ("found by two passes").
- **Stale probe compiler.** The probe compiler is the 01:25 build. A
  compiler entry cites only functions that are unchanged in the working
  tree.
- **Current stdlib.** Stdlib and runtime code is read from the repo at
  run time, so those probes are current.
- **Backend names.** run = `fid run`, ccompile = C, emit = pe_ir,
  --nat = `emit --nat`, wasm = `fid wasm` under Node. Linux targets were
  not run.
- **Line numbers.** Line numbers move; function names are the reference.

### Critical

- [ ] BG367. **Safe code can make a function value out of any integer (`12345
      as fn(i64) -> i64`) and call it: a jump to an arbitrary address with no
      `unsafe`.** compiler/typechecker.fd TypeChecker_check_cast with
      tc_cast_dst_is_value (the cast-needs-unsafe rule covers a reference,
      str, struct, enum, collection, tuple, array or tensor destination;
      Type::Func is none of these, so `int as fn(..)` is a safe cast):
      Reference §4.6 and §8.5 keep every cast that makes something usable out
      of a raw word behind `unsafe`; the one exemption, `n as *mut i64`, is
      argued safe because "a raw pointer cannot be read or written without
      `unsafe`". A function value, unlike a raw pointer, is called with no
      `unsafe`: `val g = 12345 as fn(i64) -> i64; g(3)` compiles in safe code
      and the C build jumps to 0x3039 ("Invalid memory access ... at 0x3039"),
      pe_ir likewise faults; with a chosen address this is arbitrary code
      execution from code the compiler calls safe (and `fid audit` / SR safety
      evidence would not list it). Trigger: `fn main() { val g = 12345 as
      fn(i64) -> i64; println(g(3)); }` -> `fid check` no errors; `fid
      ccompile` exe: "Panic: Invalid memory access (null or dangling pointer)
      at 0x3039"; `fid emit` exe: "Invalid memory access". Should be "casting
      an integer to fn(i64) -> i64 needs an `unsafe` block". Confirmed Nov 27
      (`fid check`, `fid ccompile`, `fid emit` pe_ir; `fid run` refuses at run
      time). TypeChecker_check_cast and tc_cast_dst_is_value are unchanged
      between fb4 and the repo. Confidence: high.
- [ ] BG368. **A method's `self` may be declared with any type (`impl A { fn
      m(self: B) }`, `self: i64`): the receiver's memory is reinterpreted, so
      safe code reads an integer field as a str or a Vec.**
      compiler/typechecker.fd TypeChecker_register_program (impl registration,
      ~2395-2420: the method's parameter types, `self`'s included, are taken
      as written; nothing compares `self`'s type with the impl's type) and
      TypeChecker_check_impl_sig (for a trait impl compares only "has self"
      and the other parameters): `impl A { fn m(self: B) -> str { return
      self.b; } }` with `struct A { a: i64 }`, `struct B { b: str }` compiles;
      `a.m()` hands A's handle to code that reads it as a B. `fid run` and
      pe_ir panic "a str without a string header" (the i64 field read as a
      str); with `struct B { b: Vec<i64> }`, `self.b.len()` on pe_ir prints
      4899916394579099648 (a header read from an arbitrary address), and
      `self: i64` gives the struct's address (`a.m()` = 5353). The same holds
      in a trait impl (`impl Sh for A { fn sh(self: B) -> str { .. } }`). Only
      `self`, `self: A`, `&A`, `&mut A` (and Self) should be accepted.
      Trigger: `struct A { a: i64, n: i64 } struct B { b: Vec<i64> } impl A {
      fn m(self: B) -> i64 { return self.b.len(); } } fn main() { val a = A {
      a: 4198400, n: 2 }; println(a.m()); }` -> `fid check` no errors; `fid
      emit` exe prints 4899916394579099648. Expected: "self of a method of A
      must be A". Confirmed Nov 27 (`fid run`, `fid emit` pe_ir). Neither
      fb4's nor the repo's typechecker.fd has any check of the declared `self`
      type (the registration code is the same). Confidence: high.
- [ ] BG369. **A `[T; N]` struct field holds only the array's address, so a
      struct built with an array literal and returned from a function reads
      the callee's dead frame.** compiler/lower.fd Lowerer_make_struct_layout
      (default layout: every non-struct field is one 8-byte word, so an
      `IrType::Array` field gets 8 bytes; also the @repr(C) branch's `_ =>
      8`), and the struct-literal / field-read lowering that stores and loads
      that word: reference §8.1 says structs and arrays are values (assignment
      and by-value passing copy them), and BG126 fixed this for a bare array
      result. A struct field of array type is laid out as one word holding the
      address of the array's storage (test_c_dyn_array_field.fd documents "a
      `[T; N]` struct field is one word, the array's address"), so (a) `return
      S { a: [1, 2, 3, 4], .. }` stores the address of the literal's stack
      slot in the callee, which is gone after the return; (b) `var s2 = s;`
      copies the word, so s and s2 share the elements; (c) `var q = s2.a; q[1]
      = 50;` writes into s2.a (and s.a). A @repr(C) struct with an array field
      also has the wrong size for C. (b)/(c) are the struct-field form of the
      open miscompile M21 (`var b = a` on an array,
      docs/verification/miscompile-log.md; the plain-local case of M21 now
      prints right with fb4); (a), the dangling result, is not covered by M21
      or BG126. Trigger: (a) `struct S { a: [i64; 4], n: i64 } fn mk() -> S {
      return S { a: [1, 2, 3, 4], n: 9 }; } fn clobber() -> i64 { var z: [i64;
      8] = [7, 7, 7, 7, 7, 7, 7, 7]; return z[0] + z[7]; } fn main() { val s =
      mk(); println(clobber()); println(s.a[0] + s.a[3]); }` should print 14,
      5: run prints 4, C -1, emit 5368726023, --nat 272891540. (b) `var s = S
      { a: [1, 2, 3, 4], n: 9 }; var s2 = s; var q = s2.a; q[1] = 50;
      println(s2.a[1]); s2.a = q; println(s.a[1]);` should print 2, 2: every
      backend prints 50, 50. A tuple element does the same: `fn tup(k: i64) ->
      ([i64; 2], i64) { val a: [i64; 2] = [k, k + 1]; return (a, 5); }`, `val
      (x, y) = tup(3);` then another call, `x[0] + x[1] + y` should be 12: run
      5373, C 4, emit 9663871125, --nat 277130547, wasm 5064654320496300552.
      Confirmed Nov 27 (run, ccompile, emit, emit --nat; outputs above).
      Lowerer_make_struct_layout and Lowerer_held_ir unchanged between fb4 and
      the repo. Confidence: high.
- [ ] BG370. **MAVLink signing is bypassed: a parser given a key still accepts
      unsigned v1/v2 frames, and replayed signed frames.** stdlib/mavlink.fd
      mav_try_frame (with mav_parser_set_key): mav_parser_set_key sets
      `check_sig = true`, but mav_try_frame checks the signature only inside
      `if sig > 0` (incompat flag 0x01 present). A frame without the signing
      flag, and every v1 frame (stx 254), goes straight to
      `p.frames.push(fr)`, so an attacker without the key just sends unsigned
      commands (COMMAND_LONG etc.). The MAVLink 2 signing spec says unsigned
      packets are rejected once signing is on (unless the application
      explicitly accepts them), and that a signed packet whose timestamp is
      not newer than the last one of its (sysid, compid, link_id) stream is a
      replay and dropped; there is no timestamp state at all, so a captured
      signed frame can be replayed forever. The module comment ("checks
      signatures when given a key") reads as protection the parser does not
      give. Trigger: `var p = mav_parser_new(mav_common_defs());
      mav_parser_set_key(&mut p, key32);` then `mav_parser_feed(&mut p,
      mav_frame_v2(&m,1,1,0))` -> 1 frame, `mav_frame_v1(..)` -> 1 frame,
      sig_errors 0; feeding the same `mav_frame_v2_signed(..)` twice ->
      accepted both times. Should be 0 for the unsigned ones and the replay.
      Confirmed Nov 27 (fid run: "unsigned accepted with key set: v2=1 v1=1
      sig_errors=0", "signed=1 replayed=1"). Confidence: high.

### High

- [ ] BG371. **A turbofish call of a generic function with a trait bound is
      always rejected ("Unknown annotation '@__trait_bound'"), even when the
      type implements the trait.** compiler/mono.fd (generic instantiation:
      BG144 pushes `Annotation_new("__trait_bound", [type, trait])` onto
      temp_func.annotations); compiler/typechecker.fd check_one_annotation_fd
      / annotation_target_mask (no entry for `__trait_bound`): For every bound
      `T: Trait` instantiated with a named type, mono adds an internal
      `__trait_bound` annotation to the instance. On the turbofish path the
      instance (`pr__Pt`) is then annotation-checked by the type checker,
      whose annotation_target_mask returns 0 for that name, so it reports
      "Unknown annotation ... no compiler pass consumes this name" as an
      error. The same call without the turbofish compiles. Reference §7.2
      documents `f::<T>(args)` as supported. A side effect: a turbofish with a
      type that does NOT implement the bound is never given the real bound
      error (`Q does not implement Show`), only this internal message (or, if
      the body uses the method, "no method 'show' on struct 'Q'" in the
      instantiated body). Trigger: `cap Show { fn show(self) -> str; } struct
      Pt { x: i64 } impl Show for Pt { fn show(self: Pt) -> str { return "pt";
      } } fn pr<T: Show>(v: T) -> str { return v.show(); } fn main() { val p =
      Pt { x: 1 }; println(pr::<Pt>(p)); }` -> "error[E078]: Unknown
      annotation '@__trait_bound' on 'pr__Pt'", exit 1. `pr(p)` prints "pt".
      Confirmed Nov 27 (`fid run`/`check`). mono.fd's `__trait_bound` push and
      typechecker.fd annotation_target_mask are unchanged between fb4 and the
      repo. Confidence: high.
- [ ] BG372. **Two `struct` (or `enum`) definitions of one name in a module
      are accepted; the type checker uses the first and lowering the second,
      so the program computes garbage.** compiler/compiler.fd
      cc_find_duplicate_funcs (the E060 "Duplicate definition" check walks
      only `prog.funcs`; structs, enums, consts, globals and impl methods are
      never checked): The only same-module duplicate check is for free
      functions. A second `struct Pt` is silently kept: the checker resolves
      `Pt` to the first declaration, lowering to the last, so a field access
      is type-checked with one type and lowered with another. A duplicate
      `enum` produces a cascade of misleading errors ("enum 'E' has no variant
      'C'", a non-exhaustive match in the generated `__eq_E`) pointing at the
      first declaration instead of one "defined twice" error. Trigger: `struct
      Pt { x: i64 } struct Pt { x: f64 } fn main() { val p = Pt { x: 3 }; val
      k: i64 = p.x + 1; println(k); }` -> prints 4616189618054758400 (the bits
      of 4.0); should be "struct 'Pt' is defined twice". With `struct Pt { x:
      str }` as the second, `println(p.x + 1)` prints an empty line. Confirmed
      Nov 27 (`fid run`). cc_find_duplicate_funcs is unchanged between fb4 and
      the repo. Confidence: high.
- [ ] BG373. **`@no-alloc` accepts `s += t` on strings and `Vec::filled(n,
      x)`, both of which allocate.** compiler/typechecker.fd
      TypeChecker_check_expr_inner Expr::Binary (the in_no_alloc string-`+`
      error is raised only for a BinOp::Add expression, never for Stmt::Assign
      with AssignOp::AddAssign); tc_alloc_func_names (lists "Vec_filled_typed"
      but not "Vec_filled", the name scan_expr_for_calls records for
      `Vec::filled(..)`): SR-1 (docs/reference/safety-rules.md) forbids
      "string `+`" and allocating builtins in a `@no-alloc` function. `s = s +
      y` is rejected ("uses string `+`, which allocates") but `s += y`, which
      lowers to the same concatenation, passes; the effect scanner already
      knows `s += t` allocates (TypeChecker_scan_expr_for_effects, 2g.65 adds
      "Alloc"), the safety checker does not. `Vec::filled(100, 0)` is recorded
      as the call "Vec_filled", which is not in the alloc list, and passes
      too. Trigger: `@no-alloc fn f(x: str, y: str) -> str { var s = x; s +=
      y; return s; } fn main() { println(f("q", "r")); }` -> `fid check`: "no
      errors" (with `s = s + y`: E097). `@no-alloc fn f(x: i64) -> i64 { val v
      = Vec::filled(100, 0); return v.len(); }` -> "no errors". Confirmed Nov
      27 (`fid check`). The in_no_alloc string-`+` check (one occurrence) and
      tc_alloc_func_names are unchanged between fb4 and the repo. Confidence:
      high.
- [ ] BG374. **`@constant_time` accepts `a == b` on secret strings,
      `v.contains(secret)`, and a local closure that branches and returns
      early on a secret.** compiler/typechecker.fd TypeChecker_ct_check_one
      (calls collected with scan_stmts_for_calls while g_scan_dot_methods is
      off, so builtin methods are plain names that are no user function and
      pass; Binary Eq on str is no call); ct_secret_dependence /
      ct_scan_early_returns (do not enter Expr::Closure bodies);
      compiler/checks.fd peir_ct_check (the backend IR check also lets these
      through): SR-14 says a `@constant_time` function has "no
      secret-dependent branch, early return or index". `str ==` compares byte
      by byte and stops at the first difference (the textbook MAC/password
      timing leak), and `Vec.contains` stops at the first match; both run in
      the function on secret parameters and are accepted. A closure defined in
      the function, `fn(x: i64) -> i64 { if x > 3 { return 1; } return 0; }`,
      called with the secret, is accepted too. The J7 machine-code check
      (pe_ir) also builds all three. Trigger: `@constant_time @wrapping fn
      f(a: str, b: str) -> bool { return a == b; } fn main() {
      println(f("abc", "abd")); }` -> `fid check`: no errors; `fid emit`
      builds and runs ("false"). Same for `fn f(a: Vec<i64>, b: i64) -> bool {
      return a.contains(b); }` and `fn f(a: i64, b: i64) -> i64 { val g =
      fn(x: i64) -> i64 { if x > 3 { return 1; } return 0; }; return g(a); }`.
      Expected: an E098 error like the one `if a == b` gets. Confirmed Nov 27
      (`fid check`, `fid emit` pe_ir). TypeChecker_verify_constant_time and
      ct_secret_dependence are unchanged between fb4 and the repo;
      TypeChecker_ct_check_one and peir_ct_check changed only by the CF32
      @public parameter lines (not the call/method handling). Confidence:
      high.
- [ ] BG375. **`sandbox { }` lets IO through a `dyn Trait` method call or a
      generic function whose bound method does IO; a compiled program prints
      inside the sandbox.** compiler/typechecker.fd
      TypeChecker_check_expr_inner, Expr::MethodCall (the `sandbox_depth > 0`
      TypeChecker_sandbox_reject is only in the concrete-struct branch, after
      TypeChecker_method_find); TypeChecker_check_sandbox_call (for `g(a)` it
      takes g's inferred effects, which miss a bound method's IO): Reference
      §9.3 / SR-15: inside `sandbox` a call to "a method of a user type that
      does" IO is an error, and "compiled programs rely on the compile-time
      check". `d.t()` on `d: dyn T` and `g(a)` with `fn g<X: T>(x: X) -> i64 {
      return x.t(); }` (where A's `t` prints) both pass `fid check`. `fid run`
      stops it at run time (p_sandbox_enter), but `fid emit` builds an exe
      that prints "5" from inside the sandbox. This is the sandbox counterpart
      of BG294 (which is about `!{}` declarations): a different checker and a
      security boundary, and §9.3 already handles unknown receivers for C
      reachability but not for IO. Trigger: `cap T { fn t(self) -> i64; }
      struct A { a: i64 } impl T for A { fn t(self: A) -> i64 { println(5);
      return 1; } } fn g<X: T>(x: X) -> i64 { return x.t(); } fn main() { val
      a = A { a: 1 }; sandbox { val r = g(a); } }` -> `fid check` no errors;
      `fid emit` exe prints 5. Same with `val d = a as dyn T; sandbox { val r
      = d.t(); }`. A direct `a.t()` in the sandbox is rejected (E091).
      Confirmed Nov 27 (`fid check`, `fid run` runtime panic, `fid emit` pe_ir
      prints). TypeChecker_check_sandbox_call is unchanged between fb4 and the
      repo. Confidence: high.
- [ ] BG376. **`@no-unbounded` (and so `@crashfree`, `@medical`,
      `@safety-critical`, `@autosar`, `@realtime`) accepts `loop { }`: only
      `while` is looked for.** compiler/typechecker.fd scan_stmt_for_while
      (Stmt::While pushes a site; Stmt::Loop only recurses into its body and
      pushes nothing); used by check_safety_body (~6575) and
      TypeChecker_realtime_one (~9069): SR-3 in
      docs/reference/safety-rules.md: "No `while` or `loop` without a bound".
      `loop` is a real statement (Stmt::Loop) and an unconditional one, yet a
      `@crashfree fn f(x: i64) -> i64 { loop { } return x; }` passes: the
      function the annotation promises never hangs spins forever. The same
      scan feeds @realtime's "contains a 'while' loop" check. Trigger:
      `@no-unbounded fn f(x: i64) -> i64 { var i: i64 = 0; loop { i = i + 1;
      if i > x { break; } } return i; } fn main() { println(f(3)); }` -> `fid
      check` no errors, `fid run` prints 4; `@crashfree fn f(x: i64) -> i64 {
      loop { } return x; }` -> no errors. With `while true { }` both are
      rejected. Confirmed Nov 27 (`fid check`, `fid run`). scan_stmt_for_while
      is unchanged between fb4 and the repo. Confidence: high.
- [ ] BG377. **`@realtime` accepts allocation (string `+`, f-strings, `clone`,
      `Box::new`), `spawn`/`await`, dynamic dispatch through `dyn`, and
      indirect recursion.** compiler/typechecker.fd TypeChecker_realtime_one
      with tc_rt_forbidden_call (a fixed name list scanned over
      scan_stmts_for_calls output: Expr::StrConcat/f-strings record no call
      there, `.clone()` is recorded as "clone" which is not in the list,
      Box_new is not in it, `spawn` is Expr::Spawn not a call named "spawn",
      `p_dyn_call` exists only in the IR; recursion is tested only as `c ==
      func.name`): Reference §11: `@realtime` = "No IO/Net/Spawn effect, no
      while, no recursion, no allocating, blocking or dynamic-dispatch call".
      Each of these is accepted: `return a + b` on strs; `return f"{x}"`;
      `v.clone()`; `Box::new(x)`; `val h = spawn work(x); return await h;`
      (starts a thread and blocks on it); `d.t()` on `d: dyn T`; `f` calls `g`
      calls `f`. Only the names literally in the list (`push`, `Vec_new`,
      `println`, ...) and direct self-calls are caught. Trigger: `fn work(x:
      i64) -> i64 { return x * 2; } @realtime fn f(x: i64) -> i64 { val h =
      spawn work(x); return await h; } fn main() { println(f(1)); }` -> `fid
      check` no errors (likewise for each construct in WHAT; `v.push(x)` is
      rejected E099). Confirmed Nov 27 (`fid check`). TypeChecker_realtime_one
      and tc_rt_forbidden_call are unchanged between fb4 and the repo.
      Confidence: high.
- [ ] BG378. **`@no-indirect` and `@no-recursion` are still bypassed by a
      function value taken from a call or method result (`val g = v.get(0);
      g(x)`, `val g = mk(); g(x)`, `v.get(0)(x)`): BG152's fix covers only
      some initializers.** compiler/typechecker.fd tc_let_is_fn_value (an
      untyped `val` counts as a function value only when its initializer is
      Expr::Ident, Closure, FieldAccess or Index; a Call/MethodCall result is
      not), used by tc_collect_lets in check_safety_body: BG152 (fixed) made a
      call through a local function value an indirect call. The fix recognises
      the local only by the syntactic shape of its initializer, so `val g =
      v.get(0)` or `val g = mk()` (mk returns `fn(i64) -> i64`) is not a local
      function value, and `g(x)` is then skipped entirely (neither a known
      function nor a parameter). `@no-recursion fn f(x: i64) -> i64 { var v:
      Vec<fn(i64) -> i64> = Vec::new(); v.push(f); val g = v.get(0); if x <= 0
      { return 0; } return g(x - 1); }` recurses and is accepted; so is
      `@no-indirect` with `val g = mk(); return g(x);` (all callees
      @no-indirect). The type checker knows g's type is `fn(i64) -> i64`; the
      safety scan does not use it. Trigger: `@no-indirect fn inc(x: i64) ->
      i64 { return x + 1; } @no-indirect fn mk() -> fn(i64) -> i64 { return
      inc; } @no-indirect fn f(x: i64) -> i64 { val g = mk(); return g(x); }
      fn main() { println(f(1)); }` -> `fid check` no errors, `fid run` prints
      2. With `val g: fn(i64) -> i64 = mk();` (typed) it is rejected.
      Confirmed Nov 27 (`fid check`, `fid run`). tc_let_is_fn_value is
      unchanged between fb4 and the repo. Confidence: high.
- [ ] BG379. **A typed pattern binding (`Option::Some(v: str)`) is never
      checked against the payload's type: the checker types `v` as the
      payload, lowering as the annotation, so the program reads an integer as
      a string or a float (wrong output, a crash on pe_ir).**
      compiler/typechecker.fd TypeChecker_check_stmt_inner Stmt::Match,
      Pattern::EnumVariant arm (declares each binding with
      TypeChecker_pattern_binding_types' payload type and ignores Binding.ty);
      TypeChecker_check_enum_pattern (checks the arity, not the written
      types); lowering takes the written `ty`: Reference §6 allows "optionally
      with a type name (`Some(v: i64)`)". On an `Option<i64>`,
      `Option::Some(v: str)` and `Option::Some(v: f64)` are accepted with no
      error; then `v` is lowered as the written type: `println(v)` prints an
      empty line (`fid run`) or faults ("Invalid memory access", pe_ir), and
      `v: f64` prints 3 as a denormal and makes `val w: i64 = v + 1`
      4607182418800017408. The checker meanwhile treats `v` as i64
      (`str_len(v)` is rejected: "expects 'str', got 'i64'"), so the two
      passes disagree on the variable's type. Trigger: `fn main() { val o:
      Option<i64> = Option::Some(3); match o { Option::Some(v: str) => {
      println(v); } _ => { println("o"); } } }` -> `fid run`: empty line; `fid
      emit` exe: "Panic: Invalid memory access". Should be a type error
      (binding annotated str, payload i64). Confirmed Nov 27 (`fid run`, `fid
      emit` pe_ir). TypeChecker_pattern_binding_types and
      TypeChecker_check_enum_pattern are unchanged between fb4 and the repo.
      Confidence: high.
- [ ] BG380. **An integer pattern on an `f64` scrutinee is accepted, and the
      three backends disagree: `fid run` panics, C matches, pe_ir does not.**
      compiler/typechecker.fd TypeChecker_check_literal_pattern (BG129's check
      accepts the arm whenever TypeChecker_types_compatible(scrutinee, i64),
      which holds for f64); lowering compares the pattern word with the
      float's bits on pe_ir, the C path converts, the interpreter refuses:
      Reference §6: "Integer patterns are non-negative literals"; a `match` on
      a float has no defined pattern form. `match x { 3 => .. }` with `x: f64
      = 3.0` passes the checker; `fid run` then panics "Arithmetic error
      (float, int)", `fid ccompile` prints "three", `fid emit` prints "o" (it
      compares 3 with the bits of 3.0). The same check also lets a literal
      that cannot fit the scrutinee through (`match x_u8 { 300 => .. }`, an
      arm that can never match, without the "can never match" error BG129
      gives for a str pattern). Trigger: `fn main() { val x: f64 = 3.0; match
      x { 3 => { println("three"); } _ => { println("o"); } } }` -> run:
      "Panic: Arithmetic error (float, int)"; ccompile: three; emit: o. Should
      be a type error (or one defined result). Confirmed Nov 27 (`fid run`,
      `fid ccompile`, `fid emit` pe_ir). TypeChecker_check_literal_pattern is
      unchanged between fb4 and the repo. Confidence: high.
- [ ] BG381. **`?` on an `Option` inside a function returning `Result<T, E>`
      (and on a `Result` inside one returning `Option`) is accepted: `None`
      becomes an `Err` holding the word 0, so a struct error faults on
      pe_ir.** compiler/typechecker.fd TypeChecker_check_try_error_type
      (compares error types only when the operand is a Result; an Option
      operand gives from_err = Infer and returns early, and a Result operand
      in an Option function gives to_err = Infer): Reference §4.5: on None or
      Err, `e?` "returns from the enclosing function with that value". A
      `None` is no value of `Result<i64, E>`, yet `val x = a()?;` with `a() ->
      Option<i64>` in `fn b() -> Result<i64, E>` compiles; at run time b
      returns `Err(<0>)`. With `E = str` the error reads as an empty string;
      with `struct E { code: i64, msg: str }`, `e.code` prints 0 and `e.msg`
      is a null dereference: `fid emit` exe panics "Invalid memory access".
      The opposite direction (`Result`'s `?` in an `Option` function) silently
      drops the error into `None`. BG-era check (the comment above the
      function) fixed only Result-to-Result with differing E. Trigger: `struct
      E { code: i64, msg: str } fn a() -> Option<i64> { return Option::None; }
      fn b() -> Result<i64, E> { val x = a()?; return Result::Ok(x); } fn
      main() { match b() { Result::Ok(v) => { println(v); } Result::Err(e) =>
      { println(e.code); println(e.msg); } } }` -> run: "0" and an empty line;
      emit: "0" then "Panic: Invalid memory access". Should be a type error.
      Confirmed Nov 27 (`fid run`, `fid emit` pe_ir).
      TypeChecker_check_try_error_type is unchanged between fb4 and the repo.
      Confidence: high.
- [ ] BG382. **`@gas(N)` counts nothing for a method call: `s.heavy()`
      (inherent or trait method) is free, however much it costs.**
      compiler/typechecker.fd gas_total (callees from scan_stmts_for_calls
      with g_scan_dot_methods off: a method call is recorded under its bare
      name, "heavy"); TypeChecker_verify_gas / gas_collect register methods as
      "S_heavy" (the comment: "Impl methods are named Type_method"), so
      gas_index("heavy") is -1 and the call adds 0: SR-9 / reference §11: "The
      body's static cost ... plus its callees' must not exceed N". The
      function costs are keyed by `Type_method` but method calls are looked up
      by the bare method name, so no method call is ever charged. `@gas(20) fn
      f(s: S) -> i64 { return s.heavy(); }` passes although `heavy` costs
      ~12,000 (the same body as a free function `heavy(s)` gives "Gas error:
      ... total gas cost is 12011"). Same for a trait-impl method. (`x |>
      heavy` is also free, the pipe form of BG295.). Trigger: `struct S { a:
      i64 } impl S { fn heavy(self) -> i64 { var t: i64 = 0; for i in 0..1000
      { t = t + i * i + i * 3 + 7; } return t; } } @gas(20) fn f(s: S) -> i64
      { return s.heavy(); } fn main() { println(f(S { a: 1 })); }` -> `fid
      check` no errors. With a free `fn heavy(x: i64)` and `return heavy(s);`:
      E093 "total gas cost is 12011". Confirmed Nov 27 (`fid check`).
      gas_total and TypeChecker_verify_gas are unchanged between fb4 and the
      repo. Confidence: high.
- [ ] BG383. **`spawn f(x)` of a plain (non-async) function that returns a
      struct or a fixed array passes no result slot: the callee gets its
      arguments shifted and every backend crashes (`fid run` sees the argument
      as `void`).** compiler/lower.fd Lowerer_spawn_call (and Expr::Spawn in
      Lowerer_lower_expr): a struct/array-returning function is lowered with a
      hidden first parameter (sret, then __env, then the user parameters:
      Lowerer_lower_function ~7305-7330). Lowerer_spawn_call emits
      `p_thread_spawn(callee, n, args...)` with the user arguments only, and
      the runtime calls `callee(env, a0, ..)`, so the callee reads `env` as
      its sret pointer and the count/garbage as its first parameter. Only
      `async fn` tasks go through Lowerer_emit_task_adapter, which allocates
      the sret block; a plain `spawn` never does (Lowerer_spawn_wrapper even
      says it gives up when the result "is not one word"). The type checker
      accepts the program. Trigger: `struct R2 { a: i64, c: i64 } fn mk2(n:
      i64) -> R2 { println(n); return R2 { a: n, c: n + 1 }; } fn main() { val
      t = spawn mk2(7); val r = await t; println(r.a + r.c); }` prints `void`
      then fails on run; C prints 9223372036854775804 then "Invalid memory
      access"; emit and --nat print 0 then "Invalid memory access"; `fid wasm`
      traps "null function or function signature mismatch". Same with `fn
      arr(n: i64) -> [i64; 3]` and `spawn arr(5)`. The same function as `async
      fn mk2(..) -> Future<R2>` gives 15 everywhere. Confirmed Nov 27 (run,
      ccompile, emit, emit --nat as above). Lowerer_spawn_call and
      Lowerer_spawn_wrapper unchanged between fb4 and the repo. Confidence:
      high.
- [ ] BG384. **An array value pushed into a `Vec<[T; N]>` or put in an enum
      payload keeps only the address of the local array: the elements are lost
      or garbage after the function returns or the loop goes on.**
      compiler/lower.fd Lowerer_rc_kind (a `Type::Array` is RC_NONE, so
      Lowerer_rc_own_value / Lowerer_rc_tid keep the bare word) and the
      enum-variant construction / `Vec_push` lowering of an array value: an
      array value is lowered as the address of its stack slot (`word_repr`
      makes `IrType::Array` a pointer). Keeping it in a container (Vec
      element, enum payload) goes through the ownership helpers, which treat
      an array as a plain word: no copy of the elements is made, so the
      container holds a pointer into a frame slot that is reused by the next
      loop iteration or dies at the return. Same root cause family as the
      struct-field entry BG369, different paths (Vec element code 0, enum
      payload store). Trigger: `fn mkv() -> Vec<[i64; 3]> { var v: Vec<[i64;
      3]> = Vec::new(); var i = 0; while i < 3 { val a: [i64; 3] = [i, i * 10,
      i * 100]; v.push(a); i = i + 1; } return v; }` then `val v = mkv(); val
      x = v.get(1); val y = v.get(2); println(x[1] + y[2]);` should print 210:
      run 0, C 608656424416, --nat 4 (emit 210). `enum E { A([i64; 3]), B } fn
      mke(k: i64) -> E { val a: [i64; 3] = [k, k + 1, k + 2]; return E::A(a);
      }`, matching `mke(5)` and printing `a[0] + a[2]` (after another call)
      should print 12: run 7, C -1, emit 0, --nat 272891511. A closure
      capturing a local array escapes the same way: `fn mka(k: i64) -> fn(i64)
      -> i64 { val a: [i64; 3] = [k, k * 2, k * 3]; return fn(i: i64) -> i64 {
      return a[i]; }; }`, `mka(5)(2)` after another call should be 15: run 15,
      C 140697569361960, emit 1, --nat 7 (Lowerer_find_captures takes the
      slot's IrType, a pointer). Confirmed Nov 27 (run, ccompile, emit, emit
      --nat). Lowerer_rc_kind and Lowerer_rc_tid unchanged between fb4 and the
      repo. Confidence: high.
- [ ] BG385. **`spawn f(p)` with a plain struct argument hands the thread the
      address of the caller's local, not a copy: a later write by the caller
      shows in the thread, and a handle returned from the function that
      spawned reads a dead frame.** compiler/lower.fd Lowerer_spawn_call: for
      each argument Lowerer_spawn_call copies only str/Vec/HashMap values
      (`p_send_copy` for tid 1/2/3); a struct whose fields need no owner has
      RC_NONE, so `out.push(nv)` passes the value as lowered, which for a
      struct is `Ptr(Struct)`: the address of the caller's slot.
      `p_thread_spawn` hands that word to the thread, which reads the struct
      through it while the caller goes on.
      tests/bootstrap/test_spawn_owned_struct_fail.fd states the intended rule
      ("a spawned thread gets a whole copy of each argument"), and the
      async-task path (Lowerer_emit_task_adapter's caller in the Expr::Call
      lowering) does make a heap copy (`StructCopy` into `p_malloc`); the
      plain spawn path does not. Trigger: `struct P { x: i64, y: i64 } fn
      slowsum(p: P) -> i64 { var s = 0; var i = 0; while i < 20000000 { s = s
      + i % 3; i = i + 1; } return p.x + p.y + (s - s); } fn start(k: i64) ->
      Future<i64> { val p = P { x: k, y: k }; val t = spawn slowsum(p); return
      t; } fn clob() -> i64 { var z: [i64; 16] = [9, 9, 9, 9, 9, 9, 9, 9, 9,
      9, 9, 9, 9, 9, 9, 9]; return z[0] + z[15]; } fn main() { var p = P { x:
      1, y: 2 }; val t = spawn slowsum(p); p.x = 100; println(await t); val t2
      = start(5); println(clob()); println(await t2); }` should print 3, 18,
      10: ccompile prints 102, 18, 14390; emit and --nat 102, 18, 10 (the
      first is the caller's later write; the C third value is the dead frame);
      wasm (which runs the task at the spawn) prints 3, 18, 10. Confirmed Nov
      27 (ccompile, emit, --nat, wasm; `fid run` timed out on the 20M-step
      loop). Lowerer_spawn_call unchanged between fb4 and the repo.
      Confidence: high.
- [ ] BG386. **A fixed-array result through a function value (`val g = mk;
      g(3)`, a closure `fn(k) -> [i64; 3]`, `apply(mk, 4)`) is garbage on
      emit, --nat and wasm: the IR optimizer's closure devirtualisation puts
      the env before the result pointer.** compiler/iropt.fd
      iro_cp_direct_args (used by iro_closure_devirt, iro_hof_specialize_in
      (hofspec, ~7102) and iro_fdv_rewrite (fielddevirt, ~7877)): since BG126
      an array result is returned through a hidden result pointer, and
      lower.fd's CallPtr lowering puts that pointer in args[0] for an
      `IrType::Array` result exactly as for a Struct (pe_ir peir_callptr_sret
      and nat_ir ng_callptr_sret both accept Array). When the mid-end turns
      such a CallPtr into a direct Call, iro_cp_direct_args moves args[0]
      before the env only when the result type is `IrType::Struct`; for an
      Array result it emits (env, result pointer, args..), so the callee
      writes its result through the env and the caller reads an uninitialised
      buffer. On by default (closuredevirt, hofspec are default passes). `fid
      run` and ccompile (no mid-end) are right. Trigger: `fn mk(k: i64) ->
      [i64; 3] { return [k, k * 2, k * 3]; } fn main() { val g = mk; val b =
      g(3); println(b[0] + b[1] + b[2]); }` should print 18: run/C 18, emit 0
      (or 5197968 in a bigger program), --nat 277175094; with
      FID_IROPT_OFF=closuredevirt emit/--nat print 18. `fn apply(g: fn(i64) ->
      [i64; 3], k: i64) -> [i64; 3] { return g(k); }` with `apply(mk, 4)[2]`
      needs FID_IROPT_OFF=closuredevirt,hofspec to give 12. A closure `val f =
      fn(k: i64) -> [i64; 3] { return [k, k + 10, k + 20]; }; f(2)` sums to 36
      on run/C, 5197968 / 0 on emit / --nat. Confirmed Nov 27 (run, ccompile,
      emit, --nat; bisected with FID_IROPT_OFF). iro_cp_direct_args and
      iro_closure_devirt unchanged between fb4 and the repo. Confidence: high.
- [ ] BG387. **A trait method returning a fixed array, called through `dyn`,
      crashes on run, C, emit and --nat (and gives a wrong value on wasm):
      BG184's fix covers struct results only.** compiler/lower.fd
      Lowerer_lower_method_call, the `dyn:` receiver branch (`match dyn_ret {
      IrType::Struct(_) => { Lowerer_dyn_sret_all(..); DynCall(.. method +
      "__dsret" ..) } _ => {} }`): the implementing method `H_trio` returns
      its array through a hidden result pointer (Lowerer_lower_function makes
      `IrType::Array` results sret since BG126), but the dyn call path only
      routes `IrType::Struct` results through the `__dsret` wrapper
      (Lowerer_dyn_sret_wrapper); an array result falls to the plain DynCall
      with (receiver, args), so the method takes the receiver as its result
      pointer and the next word as `self`: it writes through a wild pointer.
      Trigger: `struct H { n: i64 } cap Tri { fn trio(self) -> [i64; 3]; }
      impl Tri for H { pub fn trio(self: H) -> [i64; 3] { return [self.n,
      self.n + 1, self.n + 2]; } } fn main() { val h = H { n: 4 }; val d = h
      as dyn Tri; val a = d.trio(); println(a[0] + a[2]); }` should print 10:
      `fid run` panics "Invalid memory access ... H_trio", the C exe exits 3,
      emit and --nat panic "Invalid memory access" in H_trio, wasm prints 2.
      The same method through a generic bound `T: Tri` or a direct call is
      right. Confirmed Nov 27 (all five backends). Lowerer_lower_method_call,
      Lowerer_dyn_sret_all and Lowerer_dyn_sret_wrapper unchanged between fb4
      and the repo. Confidence: high.
- [ ] BG388. **`spawn f(..)` ignores default parameters and named arguments:
      `spawn f(1)` for `fn f(a: i64, b: i64 = 10)` passes no `b`, and `spawn
      g(b: 2, a: 1)` passes the values in source order.** compiler/lower.fd
      Lowerer_lower_expr, `Expr::Spawn(e)` → `Expr::Call(callee, args)` arm
      (and Lowerer_spawn_call, which takes those raw args): a plain call
      resolves its arguments first (`Lowerer_resolve_call_args(self, callee,
      args)`: named arguments to positions, defaults filled in), but the spawn
      arm lowers `args.get(ai)` as written and hands `args.len()` values to
      `p_thread_spawn`. The type checker accepts both forms (as for a call),
      so the thread runs `f` with a missing argument (garbage register/slot)
      or with the named values swapped. Trigger: `fn f(a: i64, b: i64 = 10) ->
      i64 { return a * 100 + b; } fn g(a: i64, b: i64) -> i64 { return a * 100
      + b; } fn main() { println(f(1)); val t = spawn f(1); println(await t);
      val t2 = spawn g(b: 2, a: 1); println(await t2); }` should print 110,
      110, 102: run prints 110, 1, 201; C 110 then "Integer overflow on
      addition" in f; emit and --nat 110, 100, 201; wasm traps "null function
      or function signature mismatch". A `parallel` branch has the same defect
      (it starts through the same spawn lowering): `parallel { | x = f(2); | y
      = g(b: 3, a: 4); }` gives x = 2 on run and 200 on emit/--nat (210
      expected) and y = 304 everywhere (403 expected). Confirmed Nov 27 (all
      five backends). The Expr::Spawn arm is identical in fb4 and the repo;
      Lowerer_spawn_call unchanged. Confidence: high.
- [ ] BG389. **A method call that leaves out a parameter with a default
      (`c.m(2)` for `fn m(self, a: i64, b: i64 = 7)`) is accepted and then
      called without it: `fid run` faults, the C does not compile, the native
      backends read a stale register.** compiler/lower.fd
      Lowerer_lower_method_call (arguments lowered as written,
      `arg_vals.push(Lowerer_lower_expr(self, args.get(ai)))`);
      Lowerer_resolve_call_args, which fills defaults, has a single caller,
      the plain-call lowering (~16961): reference §2.1 says a call may leave
      out trailing parameters that have defaults and that lowering supplies
      the default; the type checker accepts the shorter method call, but only
      plain function calls go through Lowerer_resolve_call_args. A method call
      passes one argument too few to `C_m(env, self, a, b)`. Trigger: `struct
      C { k: i64 } impl C { fn m(self, a: i64, b: i64 = 7) -> i64 { return
      self.k + a * 10 + b; } } fn main() { val c = C { k: 1000 };
      println(c.m(2)); }` should print 1027: run panics "Invalid memory
      access" in C_m; ccompile: "too few arguments to function call, expected
      4, have 3"; emit prints 2020, --nat 1021, wasm 1020. An associated
      function called by its path is no better: `impl C { fn add(a: i64, b:
      i64 = 2) -> i64 { return a + b; } fn mk(k: i64 = 5) -> C { return C { k:
      k }; } }`, `C::add(1)` should be 3 and `C::mk().k` 5: run prints 1 and
      0, emit 65537 and 9663806840, --nat 105553183376201 and 4294967295 (and
      `C::m(c, 2)` gives 2020 / 1021). Confirmed Nov 27 (all five backends).
      Lowerer_resolve_call_args unchanged between fb4 and the repo;
      Lowerer_lower_method_call unchanged (checked above). Confidence: high.
- [ ] BG390. **A variadic method (`fn tot(self, ...ns: i64)`) called with
      several arguments is accepted, but the arguments are never packed into
      the Vec: `fid run`/wasm drop them, emit and --nat crash in Vec_len.**
      compiler/lower.fd Lowerer_lower_method_call (arguments lowered one by
      one); Lowerer_pack_variadic is called only from Lowerer_lower_call_expr
      (the plain-call path, "L8E: the extra arguments of a variadic function,
      packed into a Vec"): reference §2.1 says a concrete-typed variadic
      parameter receives the extra arguments packed into a `Vec<T>`. For a
      free function the call lowering packs them; a method call passes the raw
      words instead, so the method's `ns` parameter (a Vec) is the first extra
      argument (1 → address 0x1/0x9) and the others are surplus arguments.
      Trigger: `struct C { k: i64 } impl C { fn tot(self, ...ns: i64) -> i64 {
      var s = self.k; for n in ns { s = s + n; } return s; } } fn main() { val
      c = C { k: 100 }; println(c.tot(1, 2)); }` should print 103: run and
      wasm print 100, emit panics "Invalid memory access", --nat "Invalid
      memory access at 0x9" in Vec_len called from C_tot. The free function
      `fn sum(...ns: i64)` with `sum(1, 2, 3)` (also through `spawn`) gives 6
      everywhere. Confirmed Nov 27 (run, emit, --nat, wasm).
      Lowerer_lower_method_call, Lowerer_pack_variadic and
      Lowerer_lower_call_expr unchanged between fb4 and the repo. Confidence:
      high.
- [ ] BG391. **Default parameter values of a generic function are lost at
      monomorphization: `idd(1)` for `fn idd<T>(x: T, k: i64 = 3) -> i64`
      passes 0, and named arguments to a generic function get 0 for the
      defaulted ones.** compiler/mono.fd instantiate (the parameter copy:
      `Param_new(name, resolved, is_pub)` then only `annotations` and
      `is_variadic` are copied; `has_default`/`default_expr` are dropped);
      also subst_expr_part2's Expr::Closure arm copies params the same way:
      Lowerer_lower_program registers each function's defaults from
      `func.params.get(pi).has_default` / `default_expr`, and
      Lowerer_resolve_call_args fills a missing argument from that table,
      writing `Expr::IntLit(0)` for a parameter without one. A generic call is
      renamed to its instance (`idd__i64`), whose Func comes from
      `instantiate` with every `has_default` false, so the "default" filled in
      is the literal 0. The type checker accepts the short call against the
      generic declaration, so the program compiles and silently uses 0.
      Trigger: `fn idd<T>(x: T, k: i64 = 3) -> i64 { return k; } fn gsum<T>(a:
      T, b: i64 = 4, c: i64 = 5) -> i64 { return b * 10 + c; } fn main() {
      println(idd(1)); println(idd(1, 9)); println(gsum(1, c: 8));
      println(gsum("x", b: 2)); }` should print 3, 9, 48, 25: run, ccompile,
      emit, --nat and wasm all print 0, 9, 8, 20. Confirmed Nov 27 (all five
      backends). mono.fd instantiate unchanged between fb4 and the repo.
      Confidence: high.
- [ ] BG392. **Every call of a user `impl` method that returns a `str` or a
      `Vec` (any owned value) leaks the returned value (bound to a variable or
      used as a temporary); the same body as a free function does not leak.
      `StringBuilder.to_str()` leaks on every call for the same reason.**
      compiler/lower.fd Lowerer_rc_note_call -> Lowerer_rc_ret_type (looks the
      callee up in own_ret_index); Lowerer_lower_program registers impl
      methods only under their bare name (`Lowerer_insert_func_ret_type(self,
      method.name, ..)`, ~4242/4267) while the call is emitted with the
      mangled name `Type_method` (lowered at ~4485 without registering it):
      Lowerer_emit_inst calls Lowerer_rc_note_call(id, "P_name", ..);
      Lowerer_rc_ret_type("P_name") finds nothing (Type::Infer), so the call's
      result is not made a temporary of the statement. `val a: str =
      p.name();` then takes a second owner with Str_dup (Lowerer_rc_own_value:
      not a temporary) and only the copy is dropped; `println(p.name())` drops
      nothing. The returned string leaks every call. Not BG278 (that is core's
      Vec clone/map, whose names are in lw_rc_fresh_handle_call but fail the
      IrType test). Lowerer_rc_note_call differs from fb4 only by the BG204
      json test; Lowerer_rc_ret_type / Lowerer_insert_func_ret_type /
      Lowerer_emit_inst are unchanged. Trigger: `struct P { x: i64 } impl P {
      fn name(self: &P) -> str { return f"P{self.x}"; } } fn pname(p: &P) ->
      str { return f"P{p.x}"; } fn main() { val p = P { x: 5 }; val a: str =
      p.name(); println(a); println(p.name()); val b: str = pname(&p);
      println(b); }` with `fid run p.fd --leaks` -> "2 block(s), 38 bytes,
      never freed ... P_name <- main"; pname's result is freed. Same with `var
      sb = StringBuilder::new(); sb.append_str("q"); val d: str =
      sb.to_str();` -> "StringBuilder_to_str <- main" leaked. A method
      returning `Vec<i64>` (`fn items(self: &P) -> Vec<i64>`, `val v =
      p.items();`) leaks the Vec header, its buffer and a Vec_share box (probe
      l24.fd). Confirmed Nov 27 (`fid run --leaks`, probes l4.fd, l5.fd,
      l6.fd). Cited functions unchanged since fb4 apart from the json line.
      Confidence: high.
- [ ] BG393. **An enum pattern against a scrutinee that is not an enum is
      accepted: `match v.pop() { Option::Some(x) => .. }` on a `Vec<str>`
      matches nothing, on a `Vec<i64>` binds the tag word, and `match s: str {
      Option::Some(y) => .. }` crashes.** compiler/typechecker.fd
      TypeChecker_check_enum_pattern with TypeChecker_scrutinee_enum (returns
      "" for any scrutinee that is not a program enum — "a type parameter, an
      unknown, a number" — and the BG129 check is then skipped): BG129 rejects
      a pattern naming a different enum than the scrutinee's, but when the
      scrutinee is a primitive or str no check runs at all, so a program that
      believes `Vec.pop()` returns an Option (it returns the element: core
      Vec_pop) type-checks. The lowering then reads the scrutinee's word as an
      enum tag/payload: for `Vec<str>` neither arm matches (silently nothing
      happens), for `Vec<i64>` with 7 popped it prints "some 0", for a `str`
      value `fid run` panics "Invalid memory access". Both functions unchanged
      since fb4. Trigger: `fn main() { var w: Vec<i64> = Vec::new();
      w.push(7); match w.pop() { Option::Some(x) => { println(f"some {x}"); }
      Option::None => { println("none"); } } val s: str = "hi"; match s {
      Option::Some(y) => { println("s"); } Option::None => { println("n"); } }
      }` -> `fid check`: no errors; `fid run`: `some 0`, then "Panic: Invalid
      memory access (null or dangling pointer) at line 5". Want a type error
      ("the value matched is a i64, not an Option"). Confirmed Nov 27 (`fid
      check` and `fid run`, probes v1.fd, m1.fd). Functions unchanged since
      fb4. Confidence: high.
- [ ] BG394. **pe_ir: `read_stdin(n)` never returns None at the end of input
      (it gives Some("") forever), so the usual `while` loop over `read_stdin`
      chunks never ends.** stdlib/pe_runtime/base.fd pert_read_stdin (returns
      `p_str_adopt(buf)` with `got == 0` at EOF; sets pert_stdin_eof_flag but
      never returns the null str): core read_stdin maps a 0 result to None
      ("or None at its end"); the C runtime and the machine layer return null
      when nothing was read. BG250 fixed exactly this for pert_read_line (its
      comment: "it gave \"\" there, so `while read_line()` never ended"), but
      pert_read_stdin still returns an empty counted string at EOF. It also
      measures the buffer with p_str_adopt, i.e. up to the first NUL (see the
      read_file entry: binary stdin loses bytes on run, C and pe_ir; --nat
      keeps them). Trigger: `fn main() { var total: i64 = 0; var chunks: i64 =
      0; var go: bool = true; while go && chunks < 1000000 { match
      read_stdin(4) { Option::Some(c) => { total = total + str_len(c); chunks
      = chunks + 1; } Option::None => { go = false; } } } println(f"total
      {total} chunks {chunks}"); }` with a 10-byte file on stdin ->
      run/C/--nat: `total 10 chunks 3`; emit (pe_ir): `total 10 chunks
      1000000` (without the cap: an endless loop). With `ab\0cdefgh` on stdin:
      --nat `total 9`, run/C/pe_ir `total 7`. Confirmed Nov 27 (all four
      backends, probe i3.fd). Runtime read from the repo. Confidence: high.
- [ ] BG395. **X.509 name constraints are bypassed through the subject CN: a
      name-constrained CA can issue a SAN-less leaf with CN=<any host>,
      `x509_verify_chain` accepts it and `x509_matches_host` matches the host
      from the CN.** stdlib/x509.fd x509_nc_names (only SAN
      dNSName/rfc822Name/iPAddress, subject emails and the DN are checked) vs
      x509_matches_host (falls back to `c.common_name` when the certificate
      has no SAN): x509_matches_host treats the CN as a DNS name when there is
      no SAN, but x509_nc_names never offers the CN to the dNSName
      constraints, so a permitted subtree (".example.test") or excluded
      subtree does not apply to it. A CA constrained to one domain can
      therefore mint a certificate that this library accepts for any other
      host. OpenSSL (since 1.1.0) checks a host-like CN against DNS
      constraints when there is no SAN and rejects the chain ("permitted
      subtree violation"). Trigger: openssl: intermediate with
      `nameConstraints=critical,permitted;DNS:.example.test` under root1; leaf
      "/CN=bank.test" with no SAN. `x509_verify_chain([leaf, inter], [root1],
      now)` -> ok=true, `x509_matches_host(leaf, "bank.test")` -> true. The
      same leaf with SAN DNS:bank.test is correctly rejected ("name not
      permitted: dns:bank.test"). `openssl verify` rejects both. Confirmed Nov
      27 (C backend; probe r2/crypto/p8.fd, certs r2/crypto/pki/int.pem,
      bad.pem, bad2.pem). Confidence: high.
- [ ] BG396. **gRPC decompression bomb: the 4 MiB message limit is checked on
      the compressed bytes only; `gr_unpack` gunzips with no limit (up to 256
      MB, i.e. 2 GiB of Vec<i64>), so a 7.8 KB request reaches the handler as
      an 8 MB message.** stdlib/grpc.fd gr_unpack
      (`out.push(gzip_decompress(d.messages.get(i)))`), used by
      grpc_server_handle and grpc_client_read; stdlib/compress.fd
      gzip_decompress (hard-coded 268435456 cap, no *_limit variant):
      grpc_deframer_new(s.max_message) enforces GRPC_MAX_MESSAGE on the framed
      (compressed) length, then the payload is inflated with compress.fd's
      fixed 256 MB cap. gRPC's max receive size is about the decompressed
      message; here a peer can send a message that inflates ~1000:1 and make
      the server (or a client reading a response) allocate hundreds of MB (8
      bytes per output byte) per call. Trigger: server with one handler; body
      = grpc_frame(gzip_compress_level(8,000,000 zero bytes, 9), true) = 7,799
      compressed bytes with grpc-encoding gzip -> "handler got message of
      8000000 bytes", http 200, grpc-status 0 (limit 4,194,304). Should be
      RESOURCE_EXHAUSTED. Confirmed Nov 27 (C backend; probe
      r2/crypto/p15.fd). Confidence: high.
- [ ] BG397. **A by-value `self: T` struct parameter can be changed (field
      assignment or an `&mut self` method call) and the change leaks into the
      caller's variable, against reference.md §5.3 ("a by-value parameter of
      type Vec, HashMap or a struct may not be changed") and §8.1 copy
      semantics; stdlib buffer.fd depends on the leak.**
      compiler/typechecker.fd (function-body setup: `if aggregate &&
      func.params.get(vpi).name != "self"` keeps `self` out of
      value_param_names, so TypeChecker_check_param_change / E079 never fires
      for it); compiler/cowcheck.fd cow_first_change / cow_expr_changes (only
      Vec-changing method names count, so lower.fd's "parameters are borrowed;
      one the body changes is copied first" never copies `self` for
      `self.inc()`), and lower.fd's assigned-parameter copy does not cover a
      field assignment on `self`: for any other by-value struct parameter `fn
      f(p: P) { bump(&mut p); }` is rejected with E079. For `self: P` the same
      body is accepted, no entry copy is made, and since the struct is passed
      by address the callee writes the caller's struct: `fn setx(self: Q) ->
      i64 { self.x = 5; ... }` sets the caller's q.x to 5; `fn byval(self: Q)
      { self.inc(); }` (inc takes &mut self) adds 10 to the caller's q.x; a
      Vec field pushed through `&mut self` is visible in the caller.
      stdlib/buffer.fd relies on it: `read_string_lp(self: ByteCursor)` and
      `read_remaining_string(self: ByteCursor)` advance the caller's cursor
      via `self.read_u32_le()` / `self.read_bytes()`, which only works because
      of this bug. Trigger: `struct Q { x: i64 } impl Q { fn inc(self: &mut Q)
      { self.x = self.x + 10; } fn byval(self: Q) -> i64 { self.inc(); return
      self.x; } fn setx(self: Q) -> i64 { self.x = 5; return self.x; } }` then
      `var q = Q { x: 1 }; q.byval();` -> q.x = 11; `q.setx();` -> q.x = 5.
      Same with a `Vec` field (`self.v.push(9)` through an &mut-self method:
      caller's p.v.len() becomes 1). Expected: E079 (or a private copy leaving
      q.x = 1). Confirmed Nov 27 (fid run and ccompile, identical).
      TypeChecker_check_param_change and the `!= "self"` exclusion are
      unchanged between fb4/compiler and the repo; compiler/cowcheck.fd is
      byte-identical. Confidence: high.
- [ ] BG398. **`dnp3_objhdr_decode` reads past the buffer and panics on a
      truncated object header (and accepts length < 5 link frames).**
      stdlib/dnp3.fd dnp3_objhdr_decode (also dnp3_link_parse):
      dnp3_objhdr_decode checks only `pos + 3 <= b.len()` (group, variation,
      qualifier), then reads the range/count bytes for qualifiers
      0x00/0x01/0x07/0x08/0x17/0x28 with `b.get(p)`, `b.get(p + 1)`, ...
      without checking them against b.len(), so a request from the wire that
      ends right after the qualifier panics the outstation ("Vec index out of
      bounds"). Also start > stop gives a negative `count` with ok=true, and
      an unknown qualifier is ok=true with count 0. dnp3_link_parse accepts a
      LENGTH octet below 5 (IEEE 1815 §9.2.4.1.3: 5..255): `plen = length - 5`
      is negative, the block loop is skipped and the frame is ok. Trigger:
      `use dnp3;` `var h = Vec::new(); h.push(1); h.push(2); h.push(0);
      dnp3_objhdr_decode(&h, 0);` -> Panic: Vec index out of bounds (want
      ok=false). A link frame 05 64 00 C4 01 00 02 00 + valid header CRC:
      `dnp3_link_parse` ok=true func=4. Confirmed Nov 27 (fid run: "Panic: Vec
      index out of bounds ... called from dnp3_objhdr_decode"; "len0 ok=true
      func=4"). CRC-16/DNP itself checked correct (check value 0xEA82).
      Confidence: high.
- [ ] BG399. **Markdown XSS: `md_safe_url` lets `java<TAB>script:` and a
      leading control character through, which browsers run as javascript:
      links.** stdlib/markdown.fd md_safe_url (used by md_inline for
      [text](url), ![alt](src) and autolinks): md_safe_url lower-cases and
      str_trim's the URL, then tests `str_starts_with(t, "javascript:")` (also
      vbscript:, data:). The WHATWG URL parser a browser applies to href/src
      first removes every ASCII tab and newline anywhere in the URL and strips
      leading/trailing C0 controls and spaces, so "java\tscript:alert(1)",
      "java\nscript:..." (md_safe_url called directly) or
      "\x01javascript:alert(1)" are javascript: URLs to the browser but pass
      the filter; md_escape does not touch tab or control bytes. The module
      header promises "HTML is escaped, so untrusted Markdown cannot inject
      markup". Trigger: `md_to_html("[a](java\tscript:alert(1))")` -> `<p><a
      href="java      script:alert(1)">a</a></p>`; `md_to_html("[b](" +
      byte_to_str(1) + "javascript:alert(1))")` ->
      `href="\x01javascript:alert(1)"`. Both should become `href="#"` (strip
      tab/LF/CR and C0 controls before the scheme test, or allow-list
      http/https/mailto/relative). Confirmed Nov 27 (fid run, bytes checked
      with od -c). Confidence: high.
- [ ] BG400. **PNG and BMP decoders panic on hostile dimensions (checked i64
      overflow in the size arithmetic) instead of failing, and PNG inflates
      IDAT with no limit tied to width x height.** stdlib/image.fd
      image_png_decode (`stride = w * bpp`, `raw = zlib_decompress(idat)`,
      `raw.len() < h * (stride + 1)`), image_bmp_decode (`off + row * h >
      d.len()`): width and height come straight from the file as unsigned
      32-bit values and are multiplied without a range check: PNG `stride = w
      * bpp` then `h * (stride + 1)`; BMP `row = (w * bytes_px + 3) / 4 * 4`
      then `row * h`. With w = h = 2^31-1 (the largest values the PNG spec
      allows) the product passes 2^63 and the checked multiplication panics,
      so a 60-byte file kills the program that tries to decode it (the
      functions document "`width` 0 on failure"). The PNG IDAT stream is also
      inflated with zlib_decompress's fixed 256 MB cap although the decoder
      knows it needs exactly h * (stride + 1) bytes, so a tiny file forces a
      large allocation before the size check. Trigger: PNG with IHDR
      2147483647 x 2147483647, depth 8, colour type 6 and a tiny valid IDAT ->
      "Panic: Integer overflow on multiplication: 2147483647 * 8589934589" in
      image_png_decode; BMP with biWidth 0xFFFFFFFF, biHeight 0x7FFFFFFF ->
      "Panic: Integer overflow on multiplication: 12884901888 * 2147483647" in
      image_bmp_decode; a 7,844-byte 1x1 PNG whose IDAT is zlib(8,000,000 zero
      bytes) decodes "width 1" after inflating all 8 MB (want: width 0 for the
      first two, inflate capped at h*(stride+1) for the third). Confirmed Nov
      27 (fid run and C, all three; found by two passes). Confidence: high.
- [ ] BG401. **physics rays use a 10-step Newton square root that has not
      converged: a direction of length 1000 is "normalized" to 0.77, so hit
      distances are wrong (500 reported as 648).** stdlib/physics.fd
      physics_ray_new, physics_ray_vs_circle, physics_circle_distance
      (distance of circles 1000 apart also comes out 1296): both compute sqrt
      with `x = d; repeat 10 { x = (x + d / x) * 0.5 }`. Started at x = d,
      Newton only halves x while x >> sqrt(d), so 10 steps are far from enough
      once d is more than ~1000: for d = 1e6 the result is 1296 instead of
      1000. physics_ray_new then leaves the direction unnormalized (dir_x
      0.771 for (1000, 0)), and physics_ray_vs_aabb / physics_ray_vs_circle,
      which assume |dir| = 1, return distances scaled by 1/0.771. For a
      tangent ray the discriminant is 0 and the iteration computes 0/0 = NaN,
      so a grazing hit is reported as a miss. math_sqrt exists (stdlib/math)
      and is what raycast.fd uses. Trigger: `physics_ray_new(0.0, 0.0, 1000.0,
      0.0, 10000.0)` -> dir_x 0.7714908857815286; vs `physics_aabb_new(500.0,
      -10.0, 100.0, 20.0)` -> 648.0957963534393 (want 500); a unit ray vs
      circle (1000,0) r=200 -> 797.595 (want 800); unit ray along y=0 vs
      circle (10,5) r=5 (tangent) -> -1 (want 10). Confirmed Nov 27 (fid run;
      expected values by hand/Python math.sqrt). Confidence: high.
- [ ] BG402. **Gorilla timestamps: a delta-of-delta of exactly 64, 256 or 2048
      decodes as -64, -256, -2048 (silent data corruption).**
      stdlib/timeseries.fd ts_gorilla_append (bucket ranges) vs
      ts_gorilla_decode (ts_sext): the encoder puts dod in the 7/9/12-bit
      buckets for the paper's ranges [-63, 64], [-255, 256], [-2047, 2048],
      writing the value's low bits in two's complement, but the decoder
      sign-extends those bits (ts_sext), whose range is [-64, 63], [-256,
      255], [-2048, 2047]. The upper end of each bucket therefore comes back
      negated, and every later timestamp is off (the error is carried in
      `delta`). The module promises "every i64 time ... round-trips"; ok stays
      true. Trigger: times 0, 10, 84 (dod 64), 414 (dod 256), 2792 (dod 2048),
      any values: `ts_gorilla_decode(ts_gorilla_encode(&s))` gives 0, 10, -44,
      -354, -2712. Confirmed Nov 27 (fid run). Confidence: high.
- [ ] BG403. **SunSpec meter model 203 is decoded at wrong register offsets:
      frequency reads real power, power reads phase B, voltage reads phase C,
      energy reads per-phase totals.** stdlib/sunspec.fd sunspec_meter (and
      its doc comment): sunspec_meter takes `off` as the first data point
      (sunspec_walk_models sets data_off = pos + 2, after ID and L), but its
      offsets are those of the ID-based numbering for some points and neither
      for others: V_SF at +15, Hz at +16, Hz_SF at +17, W at +18, W_SF at +22,
      TotWhExp at +38, TotWhImp at +42, TotWh_SF at +54, voltage at +8. Per
      the SunSpec model_203 definition (sunspec/models json, verified) the
      data offsets are A 0, A_SF 4, PhV 5, V_SF 13, Hz 14, Hz_SF 15, W 16,
      W_SF 20, TotWhExp 36-37, TotWhImp 44-45, TotWh_SF 52. So on a real meter
      every value except the current is a different point with a different
      scale factor: frequency = total W, power = WphB, voltage = PhVphC,
      exported = TotWhExpPhA, imported = TotWhExpPhC. (sunspec_inverter's
      101/103 offsets are right.). Trigger: a 203 data block laid out per the
      spec with A=10, PhV=230 (phases 231/229/232), Hz=50, W=1000,
      TotWhExp=5000, TotWhImp=7000, all SF 0: `sunspec_meter(&r, 0)` -> A=10
      V=232 Hz=1000 W=0 exp=0 imp=0. Confirmed Nov 27 (fid run; layout checked
      against github.com/sunspec/models json/model_203.json). Confidence:
      high.
- [ ] BG404. **DLMS A-XDR decoder: no bounds checks (truncated value panics),
      an attacker-chosen element count loops for ever, and a bad element
      inside a structure is reported ok.** stdlib/dlms.fd dlms_decode,
      axdr_read_len: dlms_decode reads fixed-size values, octet/visible
      strings (`b.get(p + i)` for i < length) and date-times without comparing
      against b.len(), so a value cut short panics "Vec index out of bounds".
      For a structure/array it takes the count from axdr_read_len (up to 127
      length bytes, no limit) and runs `while k < v.count { e = dlms_decode(b,
      cur); cur = e.next; }`: past the end each call returns at once with next
      = cur and ok = false, the loop neither stops nor fails, so a 10-byte
      array header with count 2^62 hangs the meter reader. The element
      results' `ok` is never looked at, so `02 01 63` (a structure whose
      element has unknown tag 99) decodes ok=true. There is also no depth
      limit on nested structures. Trigger: `dlms_decode` of [9, 10, 1, 2]
      (octet-string of 10, 2 bytes present) -> Panic: Vec index out of bounds;
      of [1, 0x88, 0x3F, 0xFF x7] -> still running after 30 s (killed by
      timeout); of [2, 1, 99] -> ok=true count=1. Confirmed Nov 27 (fid run,
      all three). Confidence: high.
- [ ] BG405. **LwM2M Write ignores the request URI: a PUT to one resource can
      write any resource of any object, and SenML values bypass the resource's
      defined type.** stdlib/lwm2m.fd lwm2m_handle_request (PUT/POST branch),
      lwm2m_senml_write, lwm2m_tlv_write: lwm2m_handle_request checks that
      /obj/inst of the URI exists, then for SenML calls `lwm2m_senml_write(s,
      payload)`, which writes every record at the path its own "bn"/"n" names:
      the URI's object, instance and resource are never compared, so a Write
      to /3/0/14 can set /3/0/0 or create /5/0/1 (Firmware Update package URI)
      in an object instance the URI check never saw. For TLV the URI's
      resource id is dropped (`lwm2m_tlv_write(s, obj, inst, ...)`), so a TLV
      carrying resource 0 written to /3/0/14 lands on resource 0.
      lwm2m_senml_write also stores "vs"/"vb"/"vlo"/"vd" whatever the
      definition says (the `ty` it looks up is used only for "v"), so a string
      replaces an Integer resource; and a pack that fails half-way returns
      4.00 with its first records already applied. OMA LwM2M 1.1 §6.3 (Device
      Management interface): the Write targets the object instance / resource
      in the URI. Trigger: device store with /3/0/0 "ACME" and /3/0/9 = 80;
      PUT /3/0/14, Content-Format 110, payload
      `[{"n":"/3/0/0","vs":"EVIL"},{"n":"/5/0/1","vs":"http://x/fw"},{"n":"/3/0/9","vs":"str
      in int"}]` -> 2.04 Changed, manufacturer=EVIL, /5/0/1 created, battery
      level (Integer) now a string. Confirmed Nov 27 (fid run: "code=2.04
      manufacturer=EVIL /5/0/1=http://x/fw battery=str in int"). Confidence:
      high.
- [ ] BG406. **IEC 61850 GOOSE/SV decoders panic on a truncated or lying BER
      length (any station-bus multicast frame can kill a subscriber), and
      report no error.** stdlib/iec61850.fd ber_read, goose_decode_pdu,
      sv_decode (ber_str, ber_get_uint): ber_read checks only `pos + 2 <=
      b.len()`; the long-form length bytes are read with `b.get(p)` unchecked,
      and the returned `next = content + length` is never compared to b.len().
      goose_decode_pdu / sv_decode then read `length` bytes from `content`
      (ber_str, ber_get_uint, the allData copy), and the 0x84 timestamp case
      reads 8 bytes whatever its length is, so a field whose length runs past
      the buffer panics "Vec index out of bounds". GOOSE and SV are
      unauthenticated Ethernet multicast, so any device on the process bus can
      send such a frame. GoosePdu/SvAsdu also have no ok/error field: an empty
      or garbage buffer decodes to an all-zero PDU the caller cannot tell from
      a real one. Trigger: `goose_decode_pdu` of 61 04 80 10 41 42 (gocbRef
      claims 16 bytes, 2 present) -> Panic: Vec index out of bounds in ber_str
      (want a decode error). Confirmed Nov 27 (fid run). Confidence: high.
- [ ] BG407. **DDS/RTPS reader hangs on one GAP or HEARTBEAT datagram with a
      far sequence number (one loop step per skipped sample).** stdlib/dds.fd
      rtps_reader_receive (GAP and HEARTBEAT branches), rtps_reader_store,
      rtps_reader_advance: a GAP is applied with `while sn < sub.set_base {
      rtps_reader_store(r, sn, Vec::new(), true); sn = sn + 1; }` and a
      HEARTBEAT with `while r.next < sub.first { rtps_reader_store(..);
      rtps_reader_advance(r); }`, i.e. one iteration (and one pending entry)
      per sequence number, with both bounds taken from the datagram unchecked.
      A GAP whose gapList base is 2^40, or a HEARTBEAT with firstSN 2^40,
      keeps the reader busy for ~10^12 steps (and grows pend_* for the GAP).
      RTPS is UDP; the writer's GUID prefix is public (SPDP multicast), so any
      host on the network can send it. A range should be applied by moving
      `next` (and dropping pending entries below it), not by enumerating it.
      Trigger: reader matched to writer W; feed `rtps_header(W) +
      rtps_sub_gap(rid, wid, 1, 1099511627776, [])` to rtps_reader_receive ->
      still running after 40 s (killed by timeout); expected: next = 2^40 at
      once. Confirmed Nov 27 (fid run, GAP case; the HEARTBEAT loop is the
      same shape, read only). Confidence: high.
- [ ] BG408. **HTTP server: whitespace before the colon hides a framing
      header, so `Content-Length : 27` is ignored and its body is served as
      the next request (smuggling; not covered by BG177's fix).**
      stdlib/http.fd http_parse_head (header name = bytes up to ':'),
      http_read_message (hs_same(name, "content-length" /
      "transfer-encoding")): http_parse_head stores the field name as
      `str_slice(head, at, colon)` without rejecting trailing SP/HTAB, and
      drops lines without a colon silently. http_read_message's BG177 checks
      compare names exactly (case-insensitively), so "Content-Length " /
      "Transfer-Encoding " count as unknown headers: the request is taken as
      having no body, and the bytes after the head are parsed as a new request
      on the keep-alive connection. RFC 9112 §5.1: "A server MUST reject, with
      a response status code of 400 (Bad Request), any received request
      message that contains whitespace between a header field name and colon"
      — exactly because proxies that strip the space frame the message
      differently (the classic CL/TE desync). Trigger: test_std_http_framing's
      fake-connection harness, feed "POST /a HTTP/1.1\r\nContent-Length :
      27\r\n\r\nGET /smuggled HTTP/1.1\r\n\r\n" -> [POST /a] body=<> then [GET
      /smuggled] (want a 400 and close). Confirmed Nov 27 (ccompile, output as
      stated). Confidence: high.
- [ ] BG409. **CoAP Block2: an over-long Block option from the wire gives a
      block number of 2^59, and `coap_block2_respond` / `coap_block_asm_add`
      panic on `num * size`; the reserved SZX 7 is accepted as 2048-byte
      blocks.** stdlib/coap.fd coap_block2_respond, coap_block_asm_add,
      coap_block_slice (with coap_get_uint / coap_bytes_uint,
      coap_block_size): coap_decode accepts an option value of any length, and
      coap_bytes_uint folds up to 8 (or more, wrapping) bytes into an i64, so
      the Block2 value of a request or response is attacker-chosen up to 2^63.
      RFC 7959 §2.2 limits Block1/Block2 to 0-3 bytes (NUM < 2^20) and says
      SZX 7 is reserved and MUST give 4.00 Bad Request; neither is checked.
      coap_block2_respond then computes `num * size` with checked arithmetic
      and panics, so one GET with an 8-byte Block2 option crashes a server
      that serves block-wise as documented (`coap_block2_respond(&mut resp,
      &body, coap_block_num(v), coap_block_szx(v))`); coap_block_asm_add does
      the same `coap_block_num(v) * size` on a server's response on the client
      side. A value with the top bit set gives a negative NUM, which
      coap_block_slice indexes as body.get(-2048). Trigger: decode 40 01 00 01
      D8 0A 7F7F7F7F7F7F7F7F (CON GET, Block2 = 8 bytes of 0x7F): decoded
      ok=true, block "574200121902233591/1/2048"; `coap_block2_respond(&mut
      resp, &body, coap_block_num(v), coap_block_szx(v))` -> Panic: Integer
      overflow on multiplication: 574200121902233591 * 2048 (want false / 4.00
      Bad Option). Confirmed Nov 27 (fid run). Confidence: high.
- [ ] BG410. **`fid test` (the bootstrap suite runner) fails every test on
      Windows: it runs the built program as `tests/_fides_test_tmp.exe`, which
      cmd.exe reads as the command `tests`.** compiler/main.fd cmd_test
      (Backend 1: `exe_name = "tests/_fides_test_tmp.exe"` then
      `p_system(exe_name)`): on a non-POSIX host the C program is compiled to
      "tests/_fides_test_tmp.exe" and run with `p_system(exe_name)` (system()
      -> cmd /c). cmd.exe takes a `/` in the first word as a switch: it tries
      to run `tests` with the switch `/_fides_test_tmp.exe` ("'tests' is not
      recognized as an internal or external command"), exit 1, so `c_ok` is
      false for every program that compiles. The whole suite is reported
      "[FAIL] ... (C, PE ok)" and `fid test` exits 1. (The cleanup uses `del
      tests\...` with backslashes, the run does not.) AGENTS.md says "`fid
      test` runs the bootstrap suite itself". Trigger: a folder with
      tests/bootstrap/test_ok.fd = `fn main() { println("hi"); }`; `CC="zig
      cc" fid test` -> cmd prints "'tests' n'est pas reconnu en tant que
      commande interne...", "[FAIL] test_ok.fd (C, PE ok)", "C codegen: 0
      passed, 2 failed", exit 1 (want PASS). `cmd /c "tests/w.exe"` fails the
      same way for any exe. Confirmed Nov 27 (fb4 fid.exe, Windows, zig cc).
      cmd_test is unchanged between fb4/compiler and the repo. Confidence:
      high.
- [ ] BG411. **`ark install` writes outside ark_modules/: a dependency's own
      ark.toml may name a pact `../../x`, and ark copies it to
      `ark_modules/../../x(.fd)`.** compiler/ark.fd ark_install_picks ->
      ark_install_path_in (`dest = base + "/" + name`, `dest2 = base + "/" +
      name + ".fd"`; also ark_pact_dest_in, the git checkout dest,
      ark_cmd_remove's ark_bin_drop_iface(ark_base(..) + "/" + dep_name +
      ".fd")); manifest parsing takes any key of [dependencies] as the pact
      name: no pact name is checked to be a plain name (no `/`, `\`, `..`,
      drive letter). A transitive dependency (a git or path pact you depend
      on) can list `../../pwned = { path = "payload.fd" }`; ark_resolve picks
      it "(for evil)" and ark_install_path_in writes
      `ark_modules/../../pwned.fd`, i.e. two directories above the project's
      ark_modules/. For a directory source the copy is `xcopy /E /I /Y` (or
      `cp -R`) of the pact's tree onto `ark_modules/<name>`, so a hostile
      package can drop or overwrite files in any directory reachable by a
      relative path (`../../../.config/...`) just by being installed; no shell
      metacharacters are needed (BG285 is the different, shell-injection
      hole). Trigger: evil/ark.toml: `[package] name = "evil" ...`
      `[dependencies]` `../../pwned = { path = "payload.fd" }`; app/: `fid ark
      init app; fid ark add evil --path ../evil; fid ark install` ->
      "Installing ../../pwned (for evil)... OK (path)", and the file `<parent
      of app>/pwned.fd` now exists (want: "invalid pact name '../../pwned'",
      nothing written). Confirmed Nov 27 (fb4 fid.exe `fid ark`, Windows:
      ark1/pwned.fd created beside app/). compiler/ark.fd is identical in fb4
      and the repo. Confidence: high.

### Medium

- [ ] BG412. **A method defined twice (in one `impl` block or in two `impl`
      blocks of the same type), and a `const`/global defined twice, are
      accepted silently; for methods the later wins, for consts/globals the
      first.** compiler/compiler.fd cc_find_duplicate_funcs (checks
      `prog.funcs` only; impl methods and consts/globals are not in it);
      BG218's cc_find_trait_impl_conflicts covers only two *trait* impls:
      `impl Pt { fn get(self) -> i64 { return 1; } fn get(self) -> i64 {
      return 2; } }` compiles and `p.get()` is 2; the same with the two `get`s
      in two `impl Pt` blocks. `const N: i64 = 1; const N: i64 = 2;` compiles
      and N is 1; `var G: i64 = 5; var G: i64 = 6;` gives 5. Each should be an
      E060 "defined twice" error like two free functions are. Trigger: see
      WHAT; `fid run` prints 2 (methods) and 1 / 5 (const/global), exit 0.
      Confirmed Nov 27 (`fid run`). cc_find_duplicate_funcs unchanged between
      fb4 and the repo. Confidence: high.
- [ ] BG413. **The lock-order check (SR-18) never sees the standard library's
      own mutex type: `SyncMutex.lock()` in opposite orders, even in one
      function, passes `fid check`.** compiler/lockcheck.fd lc_name_of (a lock
      handle that is a field load, `self.handle`, is "no named lock") with
      lc_is_lock (only p_mutex_lock / sync_mutex_lock calls); the type
      checker's lo_summary likewise; stdlib/sync.fd SyncMutex::lock
      (`sync_mutex_lock(self.handle)`): docs/reference/safety-rules.md SR-18:
      "Locks are taken in one order across the call graph". stdlib/sync.fd's
      `SyncMutex` (the @send mutex type programs are meant to use) locks
      through `self.handle`, a field, so every lock taken through it is
      unnamed and adds no edge. `a.lock(); b.lock(); ...; b.lock(); a.lock();`
      in `main`, or the classic two-thread `t1(a, b)` / `t2(a, b)` in opposite
      orders, is accepted, while the same code with raw `p_mutex_lock(a)`
      handles is E083 "Deadlock error". The limitation is written only in
      lockcheck.fd's header comment, not in SR-18 or the reference. Trigger:
      `use sync; fn main() { val a = SyncMutex::new(); val b =
      SyncMutex::new(); a.lock(); b.lock(); b.unlock(); a.unlock(); b.lock();
      a.lock(); a.unlock(); b.unlock(); println("ok"); }` -> `fid check`: no
      errors. The p_mutex_lock version: E083. Confirmed Nov 27 (`fid check`).
      lc_name_of and lc_is_lock are unchanged between fb4 and the repo (stdlib
      read from the repo). Confidence: high.
- [ ] BG414. **A match guard is never type-checked as a condition: `3 if "yes"
      =>`, a struct or an f64 guard compile and the arm silently never runs.**
      compiler/typechecker.fd TypeChecker_check_stmt_inner Stmt::Match ("//
      Check guard": `TypeChecker_check_expr(self, g)`, result discarded) and
      the value-form match (~28404, same); no bool/integer test like the E045
      one `if` conditions get: Reference §4.2/§5.4: an `if`/`while` condition
      must be bool, i32 or i64 (`if "yes" { }` is E045 "if condition must be
      bool, got str"). The guard `if expr` of a match arm is the same kind of
      condition but its type is ignored: a str guard, a struct guard (`3 if p
      =>`) and a float guard (`3 if 2.5 =>`) are all accepted and the arm is
      skipped at run time with no diagnostic. Trigger: `struct P { a: i64 } fn
      main() { val x: i64 = 3; val p = P { a: 0 }; match x { 3 if p => {
      println("struct guard"); } 3 if 2.5 => { println("float guard"); } _ =>
      { println("o"); } } }` -> `fid run` prints "o", no error; `val r = match
      x { 3 if "yes" => 1, _ => 2 };` compiles (r = 2). Confirmed Nov 27 (`fid
      run`/`check`). The guard code in TypeChecker_check_stmt_inner is
      unchanged between fb4 and the repo (the function's diff touches only
      BG201/BG252 lines). Confidence: high.
- [ ] BG415. **`<`/`>` on two values of the same struct or the same enum are
      accepted and order addresses or words; `fid run` and pe_ir give opposite
      answers (`E::A < E::B` and `E::B < E::A` are both true on run, both
      false on pe_ir).** compiler/typechecker.fd TypeChecker_check_comparison
      (BG60's partial fix rejects ordering bools and two *different*
      structs/enums; two of the same type pass); lowering orders the handle
      words: Reference §4.2 defines `< > <= >=` for numbers (ordering strs is
      an error, BG202) and §2.2/semantics.md define only `==` on structs. `P {
      x: 5 } < P { x: 2 }` is accepted: `fid run` prints true (it compares the
      heap addresses), pe_ir prints false; for a unit-only `enum E { A, B }`,
      `fid run` prints true for both `E::B < E::A` and `E::A < E::B`, pe_ir
      false for both. No defined result, and the backends disagree; it should
      be the same kind of error BG60's fix gives for two different structs.
      Trigger: `enum E { A, B } fn main() { println(E::B < E::A); println(E::A
      < E::B); }` -> run: true true; emit: false false. `struct P { x: i64 }
      ... println(a < b); println(b < a);` with a.x=5, b.x=2 -> run: true
      false; emit: false true. Confirmed Nov 27 (`fid run`, `fid emit` pe_ir).
      TypeChecker_check_comparison is unchanged between fb4 and the repo.
      Confidence: high.
- [ ] BG416. **A `@linear` value is lost on the early return of `?` without an
      error (an explicit `return` on the same path is rejected).**
      compiler/typechecker.fd TypeChecker_linear_check_return (the "lost on
      the path through the `return`" check) is called only from Stmt::Return
      (~20455); Expr::Try, which also returns from the function on None/Err,
      never calls it: Reference §8.7: a linear value must be used exactly
      once, and "leaving its scope without using it is an error". `val t = T {
      v: 1 }; val x = g()?; return Option::Some(eat(t) + x);` loses `t`
      whenever `g()` is None, yet passes `fid check`, while `if c { return 0;
      } return eat(t);` is rejected with "It is lost on the path through the
      `return`". The same holes exist for any `?` between a linear value's
      creation and its consumption. Trigger: `@linear struct T { v: i64 } fn
      eat(t: T) -> i64 { return t.v; } fn g() -> Option<i64> { return
      Option::None; } fn f() -> Option<i64> { val t = T { v: 1 }; val x =
      g()?; return Option::Some(eat(t) + x); } fn main() { println(f() ?? 7);
      }` -> `fid check` no errors, `fid run` prints 7 (t never consumed).
      Confirmed Nov 27 (`fid check`, `fid run`).
      TypeChecker_linear_check_return and its single call site are unchanged
      between fb4 and the repo. Confidence: high.
- [ ] BG417. **A `region r with budget(N)` does not count `s += t` or
      `Vec::filled(n, x)`: a region of budget 64 may append 16 KB of text or
      allocate a 100,000-element Vec.** compiler/typechecker.fd
      TypeChecker_check_expr_inner Expr::Binary (TypeChecker_budget_heap(self,
      "string `+`", -1) only for a BinOp::Add expression; Stmt::Assign with
      AssignOp::AddAssign never charges the budget); the budget's
      allocating-call list (no "Vec_filled"): Reference §8.6: in a budgeted
      region "string `+`, an allocating builtin, ..." with no size known at
      compile time is an error. `s = s + "x..."` in a loop is rejected
      ("allocates on the heap: string `+`"), `s += "x..."` in the same loop is
      accepted, and so is `val v = Vec::filled(100000, 0);`. Same root
      mechanism as BG373 (the `+=` statement form is never
      seen as a concatenation), but a separate guarantee and check
      (TypeChecker_budget_heap / TypeChecker_budget_call). Trigger: `fn main()
      { var s: str = "a"; region r with budget(64) { var i: i64 = 0; while i <
      1000 { s += "xxxxxxxxxxxxxxxx"; i = i + 1; } } println(str_len(s)); }`
      -> `fid check` no errors (with `s = s + ...`: E078 Budget error).
      `region r with budget(64) { val v = Vec::filled(100000, 0);
      println(v.len()); }` -> no errors. Confirmed Nov 27 (`fid check`). The
      Expr::Binary budget call is the same in fb4 and the repo. Confidence:
      high.
- [ ] BG418. **The C backend cannot compile a program with a global fixed
      array: it writes `static int64_t[4] _g_GA = 0;`.** compiler/codegen_c.fd
      emit_globals (with c_type_global / gen_global_init_expr): emit_globals
      special-cases only `IrType::Struct` (`= {0}`); every other global is
      written `static <c_type_global(ty)> <name> = <init>;`. For an
      `IrType::Array` the C type is `int64_t[4]`, which C does not allow
      before the name, and the array-literal initialiser is given as `0`
      (gen_global_init_expr has no array case); the global-init code then
      assigns `_g_GA = 0;`, which C rejects too (an array is not assignable).
      `fid run`, emit and --nat build and run the same program. Trigger: `var
      GA: [i64; 4] = [1, 2, 3, 4]; fn seta(i: i64, v: i64) { GA[i] = v; } fn
      main() { seta(2, 30); println(GA[0] + GA[2]); }` — run/emit/--nat print
      31; `fid ccompile` fails: "brackets are not allowed here", "array
      initializer must be an initializer list", "array type 'int64_t[4]' is
      not assignable". Confirmed Nov 27 (ccompile with zig cc; run, emit,
      --nat print 31). emit_globals, c_type_global, gen_global_init_expr
      unchanged between fb4 and the repo. Confidence: high.
- [ ] BG419. **`fid wasm`: a narrowing cast to a signed type is not
      sign-extended, so `(-3 as i64) as i8` prints 253 and `(-300 as i16) as
      i8` 212 (M7, fixed on pe_ir and --nat, is still there on the WASM
      backend).** compiler/wasm.fd wg_gen_inst, the `Inst::IntTrunc(value, _,
      to)` arm: for every target width the arm only masks (`i64.const
      0xFF/0xFFFF/0xFFFFFFFF; i64.and`), the same code for I8 as for U8, I16
      as U16, I32 as U32. The module header says narrow integers are
      normalised by extension, and the IntExtend arm uses
      `i64.extend8_s/16_s/32_s` (0xC2/0xC3/0xC4) for signed sources, but
      IntTrunc to I8/I16/I32 never sign-extends, so the value is the
      zero-extended pattern. miscompile-log M7 fixed this in pe_ir.fd and
      nat_ir.fd (`movsx`, NEXT_SB/SH/SW); the WASM backend was not changed. A
      comparison (`k < 0`) still comes out right, printing and arithmetic do
      not. Trigger: `fn main() { val q: i16 = -300; println(q as i8); val x:
      i64 = -3; println(x as i8); val y: i64 = -70000; println(y as i16); val
      z: i64 = -5000000000; println(z as i32); }` — run, emit and --nat print
      -44 -3 -4464 -705032704; `fid wasm` + `node scripts/wasm-run.mjs` prints
      212 253 61072 3589934592. Confirmed Nov 27 (run, emit, --nat, wasm under
      Node v24). The IntTrunc arm of wg_gen_inst is identical in fb4 and the
      repo (compared after stripping CRLF; the rest of wasm.fd grew since
      fb4). Confidence: high.
- [ ] BG420. **The opt-in `simplify` pass folds a float compared with itself:
      `x != x` becomes false and `x == x` / `x <= x` true, so a NaN test
      (`isnan(x) = x != x`) is always false with FID_IROPT_ON=simplify (or
      `all`).** compiler/iropt.fd iro_identities (called by iro_simplify), the
      "comparisons of a value with itself" branch, guarded by iro_int_ty(t):
      the guard is meant to keep the self-comparison identities to integers
      ("a pointer compared to itself: still total" is the separate
      else-branch), but `t` of a comparison BinOp is its result type,
      `IrType::Bool`, and iro_int_ty returns true for Bool. So `l == r`
      comparisons of an f64/f32 value are folded too, which is wrong for NaN
      (semantics-2 §5 F-cmp: every comparison with NaN is false except `!=`).
      tests/bootstrap/test_verify_semantics_ext.fd line F3 changes from "F3
      true false false false" to "F3 false true false false" under
      FID_IROPT_ON=all. Trigger: `fn isnan(x: f64) -> bool { return x != x; }
      fn main() { val z: f64 = 0.0; println(isnan(z / z)); val n = z / z;
      println(n <= n); }` — emit / --nat print true, false; with
      FID_IROPT_ON=simplify both print false, true. Confirmed Nov 27 (emit and
      --nat, default vs FID_IROPT_ON=simplify; also the semantics test's F3
      line with FID_IROPT_ON=all). iro_identities, iro_int_ty and iro_simplify
      unchanged between fb4 and the repo. Confidence: high.
- [ ] BG421. **`fid wasm`: a call through a function value whose result is a
      fixed array passes the env as the result pointer (BG135's fix covered
      pe_ir and nat_ir for arrays, wasm only for structs).** compiler/wasm.fd
      wg_gen_inst, the `Inst::CallPtr(fn_ptr, args, cp_ret, _, _)` arm
      (`cp_sret` is set only for `IrType::Struct`): lower.fd gives a CallPtr
      with an `IrType::Array` result its result pointer in args[0] (BG126);
      pe_ir's peir_callptr_sret and nat_ir's ng_callptr_sret treat Struct and
      Array alike, but the WASM arm matches only `IrType::Struct(_)`, so for
      an array it pushes (env, result pointer, ..): the callee's sret
      parameter is the env and its env the result pointer. Independent of the
      mid-end bug above: with FID_IROPT_OFF=closuredevirt,hofspec emit and
      --nat are right and wasm still is not. Trigger: the same `val g = mk;
      val b = g(3); println(b[0] + b[1] + b[2]);` prints 8650776 under `fid
      wasm` + node, with or without FID_IROPT_OFF=closuredevirt (18 expected);
      a struct result through a function value is right on wasm. Confirmed Nov
      27 (wasm under Node v24). wg_gen_inst unchanged between fb4 and the repo
      (CRLF stripped). Confidence: high.
- [ ] BG422. **`comptime f(1)` of a function with a default parameter is
      rejected ("expects 2 args, got 1"), and `comptime h(b: 3, a: 4)` with
      named arguments fails ("cannot evaluate binop on non-literal").**
      compiler/comptime_eval.fd call_comptime_func: call_comptime_func
      requires `args.len() == func.params.len()` and binds `args.get(i)` to
      parameter i as written: defaults are not filled in and named arguments
      (`Expr::NamedArg`) are neither reordered nor unwrapped, so the NamedArg
      node itself becomes the parameter's value. The same call without
      `comptime` is valid (reference §2.1: lowering supplies defaults and
      reorders named arguments), so a valid program is rejected at build time.
      (The implicit evaluation of a `comptime fn` with literal arguments,
      `g(2)`, falls back to the run-time call and is right.). Trigger: `fn
      f(a: i64, b: i64 = 10) -> i64 { return a * 100 + b; } fn h(a: i64, b:
      i64) -> i64 { return a * 100 + b; } fn main() { val x = comptime f(1);
      println(x); println(comptime h(b: 3, a: 4)); }` — `fid run` and `fid
      emit` stop with "error[E046]: comptime error (line 5): comptime function
      'f' expects 2 args, got 1" and "comptime error (line 8): cannot evaluate
      binop on non-literal in comptime fn 'h'"; 110 and 403 expected.
      Confirmed Nov 27 (run, emit). call_comptime_func unchanged between fb4
      and the repo. Confidence: high.
- [ ] BG423. **`==` on two `HashMap`s (and on structs holding one) compares
      the handles, so a struct is not even equal to its own copy: `val q = p;
      p == q` is false.** compiler/lower.fd vec_eq_fn_for (only `Vec<..>` gets
      a content comparison; a HashMap falls to the plain word BinOp) and
      compiler/compiler.fd synthesize_struct_eq (a field compares with `==`,
      or with vec_eq_fn_for's function): proposal 0001
      (docs/proposals/0001-value-semantics.md rule 5) says `==` compares by
      content, "`Vec` and `HashMap` element by element", and reference §8.1
      says a HashMap behaves as a value: assigning one gives an independent
      value (a new handle sharing the data, `HashMap_share`). The type checker
      accepts `==` on maps, but lowering compares the handle words, so two
      maps with the same entries are unequal and, because a copy gets its own
      handle, a struct with a map field compares unequal to a copy of itself.
      Trigger: `struct M { m: HashMap<str, i64>, n: i64 } fn main() { var a:
      HashMap<str, i64> = HashMap::new(); a.insert("x", 1); var b:
      HashMap<str, i64> = HashMap::new(); b.insert("x", 1); println(a == b);
      val p = M { m: a, n: 1 }; val q = p; println(p == q); }` prints false,
      false (true, true expected). Confirmed Nov 27 on `fid run` (compiled
      backends not built: no commit memory left at the time; the lowering is
      shared). vec_eq_fn_for unchanged between fb4 and the repo. Confidence:
      high (that it disagrees with proposal 0001; the reference itself does
      not define map `==`).
- [ ] BG424. **A default parameter value is evaluated in the caller's scope: a
      caller's local that shadows a global the default names is used instead
      (`fn g(x: i64, k: i64 = LIMIT)` called from a function with `var LIMIT =
      500` gets 500).** compiler/lower.fd Lowerer_resolve_call_args (fills a
      missing argument with `defaults.get(ai)`, the default's AST, which the
      caller then lowers in its own scope); defaults are registered as raw
      expressions in Lowerer_lower_program
      (`defaults.push(func.params.get(pi).default_expr)`): the default
      expression is pasted into the call site and resolved against the
      caller's locals, not the callee's declaration scope. A name in the
      default (a global, a constant, or one of the callee's earlier
      parameters) binds to whatever the caller has under that name: a local
      `LIMIT` hides the global `LIMIT`, and `b: i64 = a * 2` reads the
      caller's `a` (the type checker does not look at defaults at all, BG263,
      so the second form is not rejected either). Same result on every backend
      since the substitution happens before lowering. Trigger: `var LIMIT: i64
      = 10; fn f(a: i64, b: i64 = a * 2) -> i64 { return a * 1000 + b; } fn
      g(x: i64, k: i64 = LIMIT) -> i64 { return x + k; } fn h(n: i64) -> i64 {
      var LIMIT = 500; return g(n); } fn main() { val a = 100; println(f(1));
      println(h(1)); }` prints 1200 and 501 on run and emit; `h(1)` must be 11
      (and `f(1)` 1002, or a compile error). Confirmed Nov 27 (run, emit).
      Lowerer_resolve_call_args unchanged between fb4 and the repo.
      Confidence: high.
- [ ] BG425. **A struct field default is resolved where the literal is
      written: `struct Cfg { port: i64 = PORT }` built with `Cfg { .. }` in a
      function that has a local `PORT` takes the local.** compiler/mono.fd
      mn_fill_struct_defaults (copies `f.default_expr` into the literal as a
      FieldInit: `out.push(FieldInit_new(f.name, f.default_expr))`): reference
      §2.2: "The default expression is written in at each such literal during
      monomorphization ... so it is evaluated once per literal". Written in as
      an unresolved AST, its names are then looked up in the scope of the
      function that contains the literal, so any local, parameter or match
      binding with the name of the global/constant the default refers to
      silently replaces it. The struct declaration's meaning depends on where
      it is used. Same mechanism as BG424,
      separate code path. Trigger: `var PORT: i64 = 8080; struct Cfg { port:
      i64 = PORT, name: str = "srv" } fn mk() -> Cfg { val PORT = 1; return
      Cfg { .. }; } fn main() { println(mk().port); }` prints 1 on run and
      emit; 8080 expected. Confirmed Nov 27 (run, emit).
      mn_fill_struct_defaults unchanged between fb4 and the repo. Confidence:
      high.
- [ ] BG426. **`read_file` of a file holding a zero byte returns the text cut
      at the first zero on `fid run`, C and pe_ir (the whole file on --nat),
      though `p_file_write` writes the zero (BG81) and reference.md §1.8 makes
      strings counted.** compiler/codegen_c.fd (the emitted C `p_file_read` ->
      `p_str_own_c`, which takes `strlen` of a headerless buffer);
      compiler/interp.fd p_file_read (delegates to the host's C runtime);
      compiler/pe_ir_stubs.fd PeirGen_gen_file_read: C `p_file_read__raw`
      freads `rd` bytes into a p_malloc buffer but returns only the pointer;
      `p_str_own_c` sees no header and uses `strlen(s)`, so the counted string
      stops at the first NUL. The interpreter forwards to the same host
      runtime. pe_ir gives the same. The machine layer's `pert_file_read`
      (stdlib/linux_runtime/base.fd) allocates `pe_new_str(size)` and keeps
      the full length, so backends disagree. A write/read round trip of any
      string with a zero byte loses data. Same text in fb4's codegen_c.fd.
      Trigger: `fn main() { p_file_write("b.dat", "ab\0cd");
      println(f"{p_file_size("b.dat")}"); match read_file("b.dat") {
      Option::Some(s) => { println(f"{str_len(s)}"); } Option::None => {} } }`
      -> run/C/emit: `5` then `2`; emit --nat: `5` then `5`. Want 5
      everywhere. Confirmed Nov 27 (all four backends, probe f1.fd).
      codegen_c.fd p_file_read lines same in fb4. Confidence: high.
- [ ] BG427. **Machine layer: `p_file_write` / `p_file_append` report success
      (0) when nothing is written; on --nat Windows writing to a directory
      returns 0.** stdlib/linux_runtime/base.fd pe_write_file (ignores the
      outcome of pe_write_all, which itself returns nothing on a failed
      write); stdlib/linux_runtime/win.fd pe_win_open (opens a directory with
      pe_win_open_dir whatever the access flags): BG81 made the C runtime
      return -1 on a failed write/close, but pe_write_file returns 0 whenever
      open succeeded: `pe_write_all` stops silently on `w <= 0` and has no
      result. On --nat Windows the open of a directory with
      O_WRONLY|O_CREAT|O_TRUNC succeeds (pe_win_open sends any directory to
      pe_win_open_dir), the write then fails, and p_file_write says 0. On
      Linux targets any short write (ENOSPC, EIO, EPIPE on a FIFO) is likewise
      reported as success. Not BG340 (that one is p_file_exists/p_rename).
      Trigger: `fn main() { p_mkdir("dd"); println(f"{p_file_write("dd", "x")}
      {p_file_append("dd", "x")}"); }` -> run/C/emit: `-1 -1`; emit --nat: `0
      0` (probes f1.fd, f3.fd). Confirmed Nov 27 (all four backends, probe
      f1.fd). Runtime read from the repo. Confidence: high.
- [ ] BG428. **`fid run`: `env_var` of a variable set to the empty string and
      `arg_at` of an empty argument are None (C and both native backends:
      Some("")).** compiler/interp.fd Interpreter_call_runtime2, `p_getenv`
      and `p_argv` (return `Value::Str(ev2)` / `Value::Str(p_argv(..))` for a
      "" value); stdlib/core.fd env_var / arg_at (`if r == 0 { return
      Option::None; }`): known-bugs 2g.42's class: an empty interpreter
      `Value::Str` compares equal to 0, so core's `r == 0` test takes an empty
      value for "not set". 2g.42 fixed it for p_file_read only
      (interp_empty_read gives a distinct empty buffer); p_getenv and p_argv
      still hand back a plain empty Str. A program that tells "unset" from
      "set to empty" (`FOO= prog`), or takes `""` as a positional argument,
      behaves differently under `fid run`. Same code in fb4 (the only
      difference in Interpreter_call_runtime2 is an unrelated Memory_set_block
      line). Trigger: `fn main() { match env_var("FID_EMPTY") {
      Option::Some(v) => { println("some"); } Option::None => {
      println("none"); } } match arg_at(1) { Option::Some(a) => {
      println("arg"); } Option::None => { println("noarg"); } } }` run as
      `FID_EMPTY= fid run p.fd ''` -> `none` `noarg`; ccompile/emit/emit --nat
      exe with `FID_EMPTY= ./p.exe ''` -> `some` `arg`. Confirmed Nov 27 (all
      four backends, probe e1.fd). Function unchanged since fb4 in these
      branches. Confidence: high.
- [ ] BG429. **On Windows every backend passes UTF-8 path text to the ANSI
      file API: `p_file_write("é.txt", ..)` creates `Ã©.txt`, an existing
      `ü.txt` cannot be opened, and non-ASCII arguments and environment values
      arrive as one-byte code-page text, not UTF-8.** compiler/codegen_c.fd
      (emitted C runtime: fopen/getenv/argv, the narrow CRT calls);
      stdlib/pe_runtime/base.fd (pert_file_* over the `A` kernel32/msvcrt
      entry points); stdlib/linux_runtime/win.fd pe_win_open/pe_win_path
      (CreateFileA path), and the argv/environment snapshot: source files and
      string literals are UTF-8 (reference.md §1), so a literal path holds
      UTF-8 bytes, but no runtime converts them (no
      MultiByteToWideChar/CreateFileW, no UTF-8 activeCodePage manifest): the
      bytes are read in the ANSI code page. Files with non-ASCII names are
      created under a mojibake name and existing ones are "missing";
      `arg_at`/`env_var` of "é" give the single cp1252 byte 0xE9 (`str_len`
      1), which is invalid UTF-8. All four backends do the same, so nothing in
      the test suite notices. Trigger: in bash: `echo pre > uni_ü.txt`, then
      `fn main() { p_file_write("uni_é.txt", "hi"); match arg_at(1) {
      Option::Some(a) => { println(f"{str_len(a)}"); } Option::None => {} }
      match read_file("uni_ü.txt") { Option::Some(a) => { println(a); }
      Option::None => { println("none"); } } }` run with argument `é` ->
      prints `1`, `none`, and the directory now holds `uni_Ã©.txt`; want `2`,
      `pre`, `uni_é.txt`. Confirmed Nov 27 (run, C, emit, emit --nat on
      Windows: identical, probe u1.fd). Confidence: high.
- [ ] BG430. **pe_ir: `StringBuilder.to_str()` stops at the first zero byte,
      so everything appended after a string holding a NUL is lost (run, C and
      --nat keep it).** stdlib/pe_runtime/base.fd pert_sb_to_str
      (`p_malloc(len + 1)` + `p_str_adopt(out)`); stdlib/core.fd p_str_adopt
      (a headerless buffer is measured with its NUL): pert_sb_append keeps the
      byte count (`str_len(s)`, the counted length) and copies the zero, but
      pert_sb_to_str hands a plain p_malloc buffer to p_str_adopt, which finds
      no string header and takes the length up to the first NUL. `sb.len()`
      (core's count) then disagrees with `str_len(sb.to_str())`. The C runtime
      and the machine layer return the whole builder. Trigger: `fn main() {
      var sb = StringBuilder::new(); sb.append_str("ab");
      sb.append_str("x\0y"); sb.append_int(-12); val s: str = sb.to_str();
      println(f"{sb.len()} {str_len(s)}"); }` -> run/C/--nat: `8 8`; emit
      (pe_ir): `8 3`. Confirmed Nov 27 (all four backends, probe sb1.fd).
      Runtime read from the repo. Confidence: high.
- [ ] BG431. **A closure that captures a `str` or `Vec` (or map) leaks the
      captured value: the environment takes an owner and no closure is ever
      dropped.** compiler/lower.fd Lowerer_make_fn_env (`cap_val =
      Lowerer_rc_dup(self, cap_val, cap_own_ty)` for every RC capture, env
      from `p_malloc`), and the ownership rules (nothing drops a value of
      function type / its env): proposal 0004 makes the env keep its own owner
      of each captured string/Vec/map ("A closure may outlive the scope it was
      made in"), but no code path releases a closure env
      (LOWER_STATIC_FN_ENVS's comment already notes that envs are never freed;
      it only removed the capture-less case). The original's count never
      returns to zero, so the captured value leaks with its buffer, and a
      closure made in a loop leaks once per iteration. Lowerer_make_fn_env is
      unchanged since fb4. Trigger: `fn main() { var i: i64 = 5; val pre: str
      = f"pre{i}"; val c = fn(k: i64) -> str { return pre + int_to_str(k); };
      println(c(1)); var v: Vec<i64> = Vec::new(); v.push(3); val c2 = fn(k:
      i64) -> i64 { return v.len() + k; }; println(c2(1)); }` with `fid run
      p.fd --leaks` -> 4 blocks never freed: the string made at the `val pre`
      line, `Vec_push`'s buffer and two `Vec_share` boxes at the `val c2`
      line. Without the closures: every block freed. Confirmed Nov 27 (`fid
      run --leaks`, probe l7.fd). Function unchanged since fb4. Confidence:
      high.
- [ ] BG432. **On Windows `p_file_size` of a file of 2 GiB or more is -1
      ("missing") on run, C and pe_ir: the size comes from 32-bit `ftell`
      (--nat gives the real size).** compiler/codegen_c.fd (emitted
      `p_file_size`: `fseek(f, 0, SEEK_END); int64_t sz = ftell(f);`, also
      `p_file_read__raw`'s `long sz = ftell(f)`); stdlib/pe_runtime/base.fd
      pert_file_size (`fn ftell as pe_c_ftell(f: i64) -> i32`, fseek with a
      32-bit offset); compiler/interp.fd p_file_size (the host's C runtime):
      on Win64 `long` is 32 bits, so ftell fails (-1) past 2^31-1 and the
      result reads as "no such file". stdlib code that sizes a file before
      reading it (fs.fd, tar, safetensors loaders) then treats a large file as
      absent; read_file of such a file is likewise NULL on C. The machine
      layer uses a 64-bit lseek (SetFilePointerEx) and is right;
      `_fseeki64`/`_ftelli64` are what pe_runtime already uses for
      p_file_read_at. The C text is the same in fb4. Trigger: in bash
      `truncate -s 3G big.bin`, then `fn main() {
      println(f"{p_file_size("big.bin")} {p_file_exists("big.bin")}"); }` ->
      run/C/emit: `-1 true`; emit --nat: `3221225472 true`. Confirmed Nov 27
      (all four backends, probe fs1.fd, 3 GiB file deleted afterwards).
      Confidence: high.
- [ ] BG433. **A `Vec::new()` written directly as a call argument
      (`fill(Vec::new())`, `m.insert("g", Vec::new())` for a `HashMap<str,
      Vec<str>>`) is made with no element type, so the strings later pushed
      into it (or into a Vec gotten from it) are never freed.**
      compiler/lower.fd Lowerer_lower_owned_new / Lowerer_rc_new_coll (the
      typed `Vec_new_typed` is chosen only where the target type is known: let
      with a declared type, assignment, return, struct field — ~10474, 12503,
      12694, 12917, 13085, 18760, 26367); call and method-call arguments lower
      `Vec::new()` as the untyped Vec_new: an untyped Vec's element type id is
      i64, so when it (or a share/copy of its buffer, which keeps the code) is
      dropped, its elements are not dropped as strings. `var g: Vec<str> =
      groups.get("g"); g.push(f"m{1}")` after `groups.insert("g", Vec::new())`
      leaks every pushed string; `var e: Vec<str> = Vec::new();
      groups.insert("g", e);` does not. Lowerer_lower_owned_new and
      Lowerer_rc_new_coll are unchanged since fb4. Trigger: `fn fill(v:
      Vec<str>) -> Vec<str> { var w: Vec<str> = v; w.push(f"x{1}"); return w;
      } fn main() { val r: Vec<str> = fill(Vec::new()); println(r.len()); }`
      with `fid run p.fd --leaks` -> "1 block(s), 19 bytes, never freed ...
      fill <- main, allocated at line 3"; with `var e: Vec<str> = Vec::new();
      fill(e)`: every block freed. Same with `var groups: HashMap<str,
      Vec<str>> = HashMap::new(); groups.insert("g", Vec::new()); var g:
      Vec<str> = groups.get("g"); g.push(f"m{1}");`. Confirmed Nov 27 (`fid
      run --leaks`, probes l16.fd, l19.fd, l20.fd). Functions unchanged since
      fb4. Confidence: high.
- [ ] BG434. **`return match x { .. => f"..", .. }` and `return if c { f".." }
      else { f".." }` leak the returned string on every call (the same value
      bound with `val r = ..; return r;` does not).** compiler/lower.fd
      Lowerer_lower_stmt, Stmt::Return (`Lowerer_rc_try_move` fails, then
      `Lowerer_lower_owned_new(self, re, self.own_ret_ty)` ->
      Lowerer_lower_keep + Lowerer_rc_own_value): an `if`/`match` used as a
      value yields the arm's value through the branch merge, which is not
      registered as a temporary of the statement; Lowerer_rc_own_value
      therefore gives the caller a second owner (Str_dup) and nothing drops
      the arm's own string. In a `val` initializer the same expression is
      handled (no leak), so only the direct `return <if/match>` form, common
      in small "label"/"name" functions, leaks. The Stmt::Return block is
      identical in fb4. Trigger: `fn label(i: i64) -> str { return match i { 0
      => f"zero{i}", _ => f"many{i}" }; } fn label2(i: i64) -> str { return if
      i > 0 { f"pos{i}" } else { f"neg{i}" }; } fn main() { val a: str =
      label(3); println(a); val b: str = label2(3); println(b); }` with `fid
      run p.fd --leaks` -> "2 block(s), 43 bytes, never freed": label (line 1)
      and label2 (line 2). With `val r: str = match ..; return r;`: every
      block freed. Confirmed Nov 27 (`fid run --leaks`, probes l21.fd, l22.fd,
      l23.fd). Return branch unchanged since fb4. Confidence: high.
- [ ] BG435. **pe_ir's `p_dir_list` still has BG68's heap overflow: it sizes
      the buffer in one FindFirstFile pass and fills it in a second, unbounded
      pass, so a file created in between writes past the block.**
      stdlib/pe_runtime/base.fd pert_dir_list (first loop sums `need`, `out =
      p_malloc(need)`, second loop `pe_set_byte(out, pos + ci, ..)` with no
      `pos < need` test): BG68 was fixed in the C runtime only (codegen_c.fd
      p_dir_list "bounds its second pass"). The default Windows backend's
      runtime walks the directory twice with FindFirstFileA/FindNextFileA and
      copies every name the second walk sees: a name added (or a longer name
      replacing a shorter one) by another process or thread between the walks
      overruns the heap block by up to 260 bytes per new entry. The machine
      layer's pert_dir_list (linux_runtime/sys.fd) reads the names once into a
      Vec and is safe. Trigger: list a directory with p_dir_list in a loop on
      an `emit` build while another process keeps creating files with long
      names in it; any second-pass name beyond the first pass's total is
      written past `out`. (An in-process `spawn` does not run concurrently
      with the loop, so I could not provoke the race deterministically here.)
      Read only (code read Nov 27, stdlib from the repo; compared with the
      fixed C runtime and with linux_runtime/sys.fd). Confidence: high.
- [ ] BG436. **`fid run` never gets back the memory of replaced strings: a
      loop that keeps reassigning a string variable exhausts the interpreter's
      127 MB heap ("Out of memory: the interpreter holds at most 133169152
      bytes") at a few KB of live data.** compiler/interp.fd — the
      interpreted-memory copies of host strings (Memory_store_cstr: a block
      per new host string, its bytes address marked "never reused";
      Memory_cstr_cached only reuses the copy of the *same* host string) and
      the Str_drop shortcut in the call path ("A string the interpreter holds
      (Value::Str) is not in its memory ... return Void"), so the drop of a
      string value frees neither the host string's memory copy nor the
      size-class block: symptom (certain): memory use grows with the total
      bytes of all strings ever made, not with the live ones. `s = s + "xyz"`
      6,500 times works, 8,000 times (a 24 KB string; ~96 MB cumulative) dies;
      `s = base + "y"` with a 3 KB `base` dies after <60,000 iterations
      although only two 3 KB strings are ever live. The compiled backends free
      the old values (`--leaks`: every block freed). The interpreter is the
      project's reference oracle (AGENTS.md), and any long-running or
      text-heavy program — the compiler's own tests under `fid run` — hits the
      limit. Mechanism (likely, from the code): each fresh string value that
      is stored to or passed through interpreted memory gets a
      Memory_store_cstr copy that no drop reaches. Memory_store_cstr /
      Memory_alloc_in differ from fb4 only by the BG229 txn bookkeeping
      (Memory_set_block / Memory_set_head / Memory_put wrappers). Trigger: `fn
      main() { var s: str = ""; var i: i64 = 0; while i < 8000 { s = s +
      "xyz"; i = i + 1; } println(str_len(s)); }` -> `fid run`: "Panic: Out of
      memory: the interpreter holds at most 133169152 bytes at line 1"; with
      6500 it prints 19500; ccompile/emit: 24000. Confirmed Nov 27 (`fid run`,
      probes ic.fd at 3000/5000/6500/8000/9000/10000/30000 and ic4.fd).
      Mechanism read only. Confidence: high (symptom), medium (exact
      mechanism).
- [ ] BG437. **`x509_verify_chain` rejects a valid chain when two trust
      anchors share a subject name (root re-key / renewal): it tries only the
      first anchor whose name matches and fails with "bad signature" (or
      "trust anchor expired") without trying the others.** stdlib/x509.fd
      x509_verify_chain (last-certificate branch: the second `while k <
      anchors.len()` loop returns on the first `x509_names_chain` hit); same
      pattern in x509_verify_chain_crl (issuer = first anchor with that name,
      so the CRL signature is checked against the wrong key): for the last
      certificate, the loop returns `x509_fail("bad signature")` if the first
      name-matching anchor's key does not verify, and `x509_fail("trust anchor
      expired")` if that anchor is expired, instead of continuing with the
      next anchor. CAs routinely have several roots with the same subject
      (re-keyed or renewed roots; an expired old root next to its successor in
      a trust store), so the result depends on anchor order. OpenSSL `verify`
      accepts the same chain. Trigger: openssl: two self-signed P-256 roots
      "/CN=Same Root" (root1, root2), leaf signed by root2.
      `x509_verify_chain([leaf], [root1, root2], now)` -> ok=false err="bad
      signature"; `[root2, root1]` -> ok=true. `openssl verify -CAfile
      root1+root2 leaf.pem` -> OK. Confirmed Nov 27 (ccompile/C backend; probe
      r2/crypto/p7.fd with r2/crypto/pki/*.pem). Confidence: high.
- [ ] BG438. **Machine-layer runtime (`emit` for Linux targets, `--nat`
      Windows, darwin, wasi): `p_os_random_u64` ignores a failed getrandom and
      returns uninitialised heap bytes instead of panicking, so
      `crypto_random_bytes`, `uuid_v4`, key and nonce generation silently get
      non-random values.** stdlib/linux_runtime/base.fd pert_os_random_u64
      (`pe_sys(PE_SYS_GETRANDOM, buf, 8, 0);` result discarded, then
      `pe_ld(buf)` of a fresh `pert_malloc(8)` block): every other backend
      treats a failing OS generator as fatal: the C runtime (codegen_c.fd
      p_os_random_u64: rand_s / read of /dev/urandom checked, `p_panic("no
      random source in the operating system")`) and pe_runtime
      pert_os_random_u64 (rand_s checked, same panic), and crypto.fd documents
      "Panics when the system has none". The machine-layer version never looks
      at the syscall result, although its shims do report failure: win.fd
      returns -5 when BCryptGenRandom fails, wasi.fd returns the WASI error,
      and on Linux the syscall itself fails with ENOSYS/EPERM under seccomp
      profiles that block getrandom (older container runtimes) or EINTR while
      it waits for entropy at boot. The 8 bytes are then whatever the
      allocator left in the block, typically a predictable value, and
      x25519/ed25519/ECDSA keys, HPKE/Noise ephemerals and WebSocket masks
      built from it are weak with no error anywhere. Trigger: read-only (no
      Linux runner here): compare `pert_os_random_u64` in
      stdlib/linux_runtime/base.fd with stdlib/pe_runtime/base.fd and the C
      runtime text in compiler/codegen_c.fd. Fix: check `pe_sys(...) == 8` and
      `p_panic` otherwise (loop on a short read / -EINTR). Read only (stdlib
      is current). Confidence: high.
- [ ] BG439. **`oidc_half_hash` hashes with SHA-256 for the *384 algorithms
      (ES384, HS384), so `oidc_validate_id_token` rejects every valid
      ES384/HS384 ID token that carries an at_hash.** stdlib/oauth2.fd
      oidc_half_hash (used by oidc_validate_id_token's at_hash check): OIDC
      Core §3.1.3.6 defines at_hash as the left half of the hash "used in the
      alg Header Parameter of the ID Token's JOSE Header" (SHA-384 for
      ES384/HS384/PS384/RS384). The function only distinguishes "*512 or
      EdDSA" from everything else, so for ES384 it returns the 16-byte left
      half of SHA-256 instead of the 24-byte left half of SHA-384. jose.fd
      accepts ES384 and HS384 (jose_alg_hash -> EC_SHA384), so such a token
      passes the signature check and then fails "at_hash mismatch". Trigger:
      `oidc_half_hash("ES384", "abc-access-token")` ->
      "PdkM--iZlIdDdi-0vOgawA" (= SHA-256 half, same as for "RS256"); the
      correct value is base64url(SHA-384(token)[0..24]) =
      "qijbNZLJUenLNKW0AjL6YizBFf1l_j4t" (Python hashlib). Confirmed Nov 27
      (fid run). Confidence: high.
- [ ] BG440. **gRPC: a compressed message whose gzip data is corrupt is
      delivered to the handler as an empty (all-defaults) message and the call
      succeeds.** stdlib/grpc.fd gr_unpack (the result of gzip_decompress is
      pushed without checking for failure; gzip_decompress returns an empty
      Vec on any error): gzip_decompress signals every error (bad magic,
      corrupt deflate, CRC/ISIZE mismatch) by returning an empty Vec;
      gr_unpack cannot tell that from a valid empty message and pushes it, so
      grpc_server_handle runs the handler with an empty protobuf (every field
      at its default) and answers grpc-status 0; grpc_client_read likewise
      hands the application an empty response message. gRPC requires INTERNAL
      for a message that fails to decompress. Trigger: same server; body =
      grpc_frame(40 junk bytes, true) with grpc-encoding gzip -> "handler got
      message of 0 bytes", http 200, grpc-status 0. Confirmed Nov 27 (C
      backend; probe r2/crypto/p15.fd). Confidence: high.
- [ ] BG441. **SRTP/SRTCP: every unauthenticated packet with a new SSRC
      permanently adds per-SSRC state (two 64-entry replay windows and five
      counters) before its tag is checked, so forged packets grow the context
      without bound and slow every later lookup.** stdlib/srtp.fd
      srtp_unprotect and srtcp_unprotect (call `srp_slot(c, ssrc)` first),
      srp_slot (appends a slot for an unknown SSRC; linear search):
      srtp_unprotect's doc promises "State (ROC, highest sequence number,
      replay window) changes only for a packet that authenticates", but
      srp_slot creates the SSRC's slot (rocs/seqs/started/tops/wins and the
      RTCP copies, 2 x Vec::filled(64)) as a side effect of the lookup, before
      authentication. Media ports receive unauthenticated UDP from anyone; a
      flood of junk packets with distinct SSRCs costs memory per packet
      forever, and since srp_slot scans the SSRC list linearly every
      legitimate packet then gets slower (quadratic total work). Trigger:
      context SRTP_AES128_CM_SHA1_80; 5,000 junk 32-byte RTP packets with SSRC
      0..4999 -> all rejected ("authentication failed"), yet `c.ssrcs.len()` =
      5000 and `c.wins.len()` = 5000 afterwards. Should stay 0 (look up
      without creating; create only after the tag verifies). Confirmed Nov 27
      (C backend; probe r2/crypto/p17.fd). Confidence: high.
- [ ] BG442. **`gzip_decompress` returns only the first member of a
      multi-member gzip file and silently drops the rest (concatenated .gz
      files, pigz/bgzip output, appended logs).** stdlib/compress.fd
      gzip_decompress (inflates once from `pos`, checks that member's
      CRC-32/ISIZE, returns `z.out`; bytes after `z.end + 8` are never looked
      at): RFC 1952 §2.2: "A gzip file consists of a series of 'members'", and
      decompressors (gzip -d, zlib's gunzip, Python's gzip.decompress)
      concatenate them. Here the second and later members are ignored without
      an error, so the caller gets a silently truncated result that passes
      every integrity check. gzip_decompress_str has the same behaviour.
      Trigger: Python `gzip.compress(b'hello ') + gzip.compress(b'world')`
      (Python's gzip.decompress gives b'hello world'); `gzip_decompress` of
      the same bytes -> len 6, "hello ". Expected 11 bytes "hello world" (or
      at least an error for the trailing data). Confirmed Nov 27 (fid run).
      Confidence: high.
- [ ] BG443. **LoRaWAN `lw_data_decode` ignores the caller's 32-bit frame
      counter whenever its low 16 bits differ from the frame's, and checks the
      MIC with the frame's raw 16-bit FCnt instead, so a replayed old frame
      decodes with mic_ok = true (even when passed lw_fcnt_extend's -1
      "replay" result).** stdlib/lorawan.fd lw_data_decode (`fr.fcnt = fcnt32;
      if (fcnt32 & 0xFFFF) != lw_get_le(f, 6, 2) { fr.fcnt = lw_get_le(f, 6,
      2); }`, then MIC and decryption use fr.fcnt): the doc says the frame is
      decoded "with the full counter fcnt32 (from lw_fcnt_extend)". LoRaWAN's
      replay protection is that the MIC (B0 block) covers the full 32-bit
      counter, so an old frame must fail under the current counter. Instead a
      mismatch silently switches to a counter whose high 16 bits are 0: any
      frame originally sent with FCnt < 65536 verifies again later, whatever
      counter the network server passes, including the -1 that lw_fcnt_extend
      returns for a replay/too-large gap. Trigger: encode an uplink at FCnt
      100 (lw_frame + lw_data_encode); `lw_fcnt_extend(70000, 100)` -> -1;
      `lw_data_decode(nk, ak, frame, -1)` -> ok=true mic_ok=true fcnt=100;
      `lw_data_decode(nk, ak, frame, 70100)` -> mic_ok=true fcnt=100. Expected
      mic_ok=false (or ok=false) when fcnt32 does not end in the frame's FCnt.
      Confirmed Nov 27 (fid run; probe r2/crypto/p25.fd). Confidence: high.
- [ ] BG444. **QUIC's TLS 1.3 client accepts an RSASSA-PKCS1-v1_5 signature
      (schemes 0x0401/0x0501/0x0601) in the server's CertificateVerify, which
      RFC 8446 §4.4.3 forbids.** stdlib/quic_tls.fd qhs_client_cv ->
      qhs_cv_verify -> quic_tls_scheme_verify (the 0x0401/0x0501/0x0601
      branches call x509_rsa_pkcs1_verify); the ClientHello builder offers
      0x0401/0x0501/0x0601 in signature_algorithms "(qhs_cv_verify checks)":
      RFC 8446 §4.4.3: "RSA signatures MUST use an RSASSA-PSS algorithm,
      regardless of whether RSASSA-PKCS1-v1_5 algorithms appear in
      'signature_algorithms'"; §4.2.3 says the PKCS#1 code points "refer
      solely to signatures which appear in certificates ... and are not
      defined for use in signed TLS handshake messages". The handshake check
      uses the same scheme table as certificate-chain checks, so a server (or
      an attacker holding a PKCS#1 signing oracle for the server's RSA key,
      e.g. a TLS 1.2 endpoint sharing it) can complete a TLS 1.3/QUIC
      handshake with a v1.5 signature, losing the domain separation PSS gives
      TLS 1.3. The client should send alert illegal_parameter for those
      schemes in CertificateVerify (keeping them only for
      signature_algorithms_cert / chain checks). Trigger: read-only: a
      CertificateVerify with scheme 0x0401 and an RSA PKCS#1 v1.5 signature
      over the TLS 1.3 CV content from an RSA-certificate server reaches
      `x509_rsa_pkcs1_verify` and, if it verifies, `t.state = QTS_WAIT_FIN`.
      Read only. Confidence: high.
- [ ] BG445. **`iec104_parse` accepts an APDU length below 4 (or above 253)
      and decodes the control field from the next frame's bytes.**
      stdlib/iec104.fd iec104_parse: the length octet is taken as is: `total =
      2 + length`, and the 4 control octets are always read from b[2..6] even
      when `length` is 0..3, i.e. from beyond the frame (the next APDU in the
      stream). IEC 60870-5-104 §5: the APDU length is 4..253, and S/U frames
      are exactly 4. With length 0 the parser returns ok=true, an I-frame
      whose N(S) is made of the following frame's start and length bytes, and
      total_len 2, so a stream reader then re-parses from the middle of the
      next frame. S- and U-formats are also accepted with any length (their
      trailing bytes are dropped silently) and a U-frame with several function
      bits set is accepted. Trigger: bytes 68 00 68 04 07 00 00 00 ->
      `iec104_parse` ok=true format=I ns=564 total_len=2 (want ok=false, "bad
      length"). Confirmed Nov 27 (fid run: "len0: ok=true format=0 ns=564
      total=2"; APCI_I is 0). Confidence: high.
- [ ] BG446. **iec104 sequence state accepts any N(S)/N(R): lost I-frames go
      unnoticed and an ack of unsent frames jams the send window.**
      stdlib/iec104.fd iec104_on_recv_i, iec104_on_recv_s: iec104_on_recv_i
      sets `s.vr = (ns + 1) & 0x7FFF` whatever `ns` is, without comparing it
      to the expected `s.vr` (IEC 60870-5-104 §5.1: a sequence-number error
      must close the connection), so a gap of lost frames is silently skipped.
      Both functions set `s.va = nr` without checking `va <= nr <= vs` (mod
      2^15); an N(R) that acknowledges frames never sent makes `(vs - va) &
      0x7FFF` about 32764, so iec104_can_send is false forever and has_unacked
      stays true (t1 fires), instead of the error the standard requires.
      Trigger: `var s = iec104_state_new(12, 8, 15, 10, 20);
      iec104_on_send(&mut s, 0); iec104_on_recv_s(&mut s, 5, 1);` ->
      can_send=false, has_unacked=true; `iec104_on_recv_i(&mut s2, 0, 0, 1);
      iec104_on_recv_i(&mut s2, 7, 0, 2);` -> vr=8 with no error. Confirmed
      Nov 27 (fid run output as stated). Confidence: high.
- [ ] BG447. **`image_bmp_decode` rejects every top-down BMP (negative
      height): the sign is fixed after the negativity test.** stdlib/image.fd
      image_bmp_decode: the height is read with img_u32le (0..2^32-1), so the
      first test `if h < 0 { top_down = true; h = 0 - h; }` can never fire;
      the second, `if h > 2147483647 { h = h - 4294967296; top_down = true;
      }`, turns it into the negative value and leaves it negative, so the
      following `h <= 0` check returns the "bad" 0x0 image. biHeight < 0
      (top-down rows) is valid BMP and common (many encoders and screen grabs
      write it); the comment "32-bit negatives: top-down rows" shows it was
      meant to be supported. Trigger: a 1x2 24-bit BI_RGB BMP with biHeight =
      -2 (0xFFFFFFFE): `image_bmp_decode` -> width 0, height 0; the same file
      with biHeight = 2 decodes (1x2). Confirmed Nov 27 (fid run: "bottom-up:
      w=1 h=2", "top-down: w=0 h=0"). Confidence: high.
- [ ] BG448. **ARINC 653 ports enforce neither the configured message size nor
      the port direction, and a queuing port can never be drained.**
      stdlib/arinc.fd arinc_queuing_port_send, arinc_sampling_port_write,
      arinc_queuing_port_receive: arinc_queuing_port_send checks only `count
      >= max_nb_message`; a message longer than `max_message_size`, or a send
      on an ARINC_PORT_DESTINATION port, returns ARINC_NO_ERROR (ARINC 653
      SEND_QUEUING_MESSAGE: INVALID_CONFIG for a length above the configured
      maximum, INVALID_MODE for a destination port; the module defines both
      codes and never returns them). arinc_sampling_port_write likewise stores
      any length on any port. arinc_queuing_port_receive returns the oldest
      message without removing it ("Fides Vec API limitation"), so after
      max_nb_message sends the port reports PORT_FULL forever and a receiver
      keeps reading the same first message. For a module whose header ties it
      to DO-178C DAL-A partition communication these are the checks the API
      exists for. Trigger: `var p = arinc_queuing_port_create("cmd", 4, 2,
      ARINC_PORT_DESTINATION, ARINC_PORT_FIFO);` then send a 5-byte Vec ->
      NO_ERROR; `arinc_sampling_port_write` of 3 bytes on a 2-byte destination
      port -> NO_ERROR. Confirmed Nov 27 (fid run: "NO_ERROR" for both).
      Confidence: high.
- [ ] BG449. **`dlms_hdlc_parse` panics on a short frame and never checks the
      format field's length against the frame.** stdlib/dlms.fd
      dlms_hdlc_parse (with decode_addr): only `b.len() >= 7` and the two
      flags are checked; the addresses, control byte and HCS are then read by
      position (`b.get(p)`, `b.get(p + 1)`) with no bound, so a 7-byte frame
      with 1-byte addresses reads the HCS from beyond the closing flag and
      panics. The 11-bit frame length in the format field is decoded into
      `frame_len` but never compared with b.len() - 2, so a frame whose length
      field disagrees with its size is accepted when the CRCs match (IEC
      62056-46 / ISO 13239 framing uses the length to delimit frames).
      Trigger: `dlms_hdlc_parse` of 7E A0 07 03 21 93 7E -> Panic: Vec index
      out of bounds (want ok=false, "frame too short"). Confirmed Nov 27 (fid
      run). Confidence: high.
- [ ] BG450. **MySQL binary-protocol TIME values come back as "1 02:00:00"
      (not "26:00:00") and lose their microseconds, unlike the same column
      through mysql_query.** stdlib/mysql.fd mysql_parse_binary_row
      (MYSQL_TYPE_TIME branch): for a prepared statement's TIME the decoder
      writes `days + " " + hh:mm:ss`, a form MySQL never produces (TIME is
      shown as [-]hhh:mm:ss, hours = days*24 + hh, range
      -838:59:59..838:59:59, which is what the text protocol, and so
      mysql_query, returns), and for the 12-byte form it reads the microsecond
      field and throws it away (`mysql_get_le(&mut r, 4);` with no use) while
      the DATETIME branch right above appends ".uuuuuu". So the same row read
      with mysql_execute and mysql_query gives different strings. Trigger:
      binary row with two TIME columns: [len 8, neg 0, days 1, 02:00:00] and
      [len 12, 0 days, 12:34:56, micro 789000] -> "1 02:00:00" and "12:34:56"
      (want "26:00:00" and "12:34:56.789000"). Confirmed Nov 27 (fid run).
      Confidence: high.
- [ ] BG451. **INI keys and section names are serialized unescaped: a key with
      ':' or '=' comes back split, and a key or section with a newline or ']'
      injects sections and keys (BG191 fixed only values).** stdlib/ini.fd
      ini_serialize (`out + e.key + " = " + v`, `"\n[" + secs.get(s) +
      "]\n"`), ini_set: BG191's fix quotes values (ini_needs_quotes /
      ini_quote), but keys and section names are written as they are and
      ini_set accepts anything. ini_parse splits a line at the first ':' or
      '=', so key "c:path" with value "1" reloads as key "c" = "path = 1"; a
      key "nick\n[admin]\nrole" reloads as a new [admin] section holding role
      = x (the same injection BG191 closed for values); a section "a]b"
      reloads as section "a". The serializer should reject or escape these.
      Trigger: `ini_set(&mut doc, "prefs", "c:path", "1"); ini_set(&mut doc,
      "prefs", "nick\n[admin]\nrole", "x"); ini_set(&mut doc, "a]b", "k",
      "v");` then `ini_parse(ini_serialize(doc))` -> [prefs] c = "path = 1",
      [admin] role = x, [a] k = v. Confirmed Nov 27 (fid run). Confidence:
      high.
- [ ] BG452. **`resp_encode_inline` joins arguments raw: a space splits an
      argument and a CR LF in one injects a second Redis command.**
      stdlib/redis.fd resp_encode_inline: the inline encoder writes each
      argument with rs_put_text, separated by a space, and ends with CR LF; it
      neither quotes nor rejects arguments holding SP, TAB, CR, LF or quotes.
      Redis's inline protocol splits on whitespace (with "..." quoting) and
      ends the command at the line break, so an argument value turns into
      extra arguments or extra commands. It has the same Vec<str> signature as
      the binary-safe resp_encode_command, and unlike resp_put_simple it
      carries no "must not hold CR or LF" warning. Trigger:
      `resp_encode_inline(["SET", "greeting", "hello world\r\nFLUSHALL"])` ->
      the 36 bytes `SET greeting hello world\r\nFLUSHALL\r\n`: a 4-argument
      SET followed by FLUSHALL. Want quoting (`"hello world\r\n..."` with
      escapes) or an error. Confirmed Nov 27 (fid run; bytes checked).
      Confidence: high.
- [ ] BG453. **JSON Schema `multipleOf` accepts every negative number: -3 is a
      "multiple of 2".** stdlib/jsonschema.fd js_check_number (multipleOf):
      the remainder is computed as `r = q - (q as i64 as f64)` with q = x / d;
      `as i64` truncates toward zero, so for a negative non-multiple r is
      negative (-0.5 for -3 / 2) and the test `r > 0.000000001 && r <
      0.999999999` never fires. Any negative instance passes multipleOf,
      whatever the divisor (JSON Schema 2020-12 §6.2.1: valid only if the
      division gives an integer). Python's jsonschema rejects -3 for
      multipleOf 2. Trigger:
      `jsonschema_valid("{\"type\":\"integer\",\"multipleOf\":2}", "-3")` ->
      true (also -7; "3" -> false); `jsonschema_valid("{\"multipleOf\":0.5}",
      "-0.75")` -> true while "0.75" -> false. Confirmed Nov 27 (fid run).
      Confidence: high.
- [ ] BG454. **`fid test` on Linux/macOS reads every test from
      "tests\bootstrap\tests/bootstrap/<name>.fd": all of them "(could not
      read)".** compiler/main.fd cmd_test (POSIX listing `ls
      tests/bootstrap/*.fd > tests/_fides_test_list.txt`, then `test_path =
      "tests\bootstrap\\" + fname`): on a POSIX host the listing comes from
      `ls tests/bootstrap/*.fd`, which prints each name WITH its directory
      ("tests/bootstrap/test_x.fd"; `dir /b` on Windows prints the bare name).
      cmd_test then (a) strips only ".fd" to get the stem, so the stem
      "tests/bootstrap/test_x" never matches the expected_fail or skip lists
      (every expected-fail test runs as a normal test, `--fail` finds "No
      expected-fail tests"), and (b) builds `test_path = "tests\bootstrap\\" +
      fname` with a backslash, giving
      "tests\bootstrap\tests/bootstrap/test_x.fd", which does not exist on
      POSIX: c_file_read gives "" and each file is reported "[FAIL] ... (could
      not read)". So the suite runner reports 0 passed on every POSIX host.
      Trigger: on Linux in the repo root: `fid test` -> "[FAIL]
      tests/bootstrap/test_hello.fd (could not read)" for every file, exit 1;
      `fid test --fail` -> "No expected-fail tests found in tests/bootstrap/".
      Read only (POSIX host needed). Confidence: high (`ls dir/*.fd` prints
      the dir prefix; the path concat is literal). cmd_test unchanged since
      fb4.
- [ ] BG455. **`fid test` knows only a hard-coded list of 74 expected-fail
      tests and never reads `// expect-error`: the other ~510 expected-fail
      tests are run as normal tests and counted failures, and an expected-fail
      "pass" is any failure at all.** compiler/main.fd cmd_test
      (`expected_fail.push(...)` x74; `is_fail =
      str_vec_contains_str(expected_fail, stem)`; fail_only branch `if !c_ok {
      [PASS] }`): tests/bootstrap has 586 files with a `// expect-error` line
      (631 named *_fail.fd); cmd_test's list has 74 names, so every newer
      expected-fail test (test_bg210_fstring_bad_escape_fail.fd, ...) is
      treated as a program that must build and exit 0 -> "[FAIL]", while `fid
      test --fail` skips them entirely. For the 74 it does run, it only checks
      that the C build or run did not succeed: the expected message is never
      compared (the Rust harness and scripts/diag-tests-check.py match the `//
      expect-error:` text), so a test that should fail with a type error but
      now crashes the compiler, or fails for an unrelated reason, still
      passes. Trigger: tests/bootstrap/test_new_fail.fd = `// expect-error:
      undefined variable` / `fn main() { println(zz); }`; `fid test` ->
      "[FAIL] test_new_fail.fd (C+PE)", counted in "2 failed", exit 1 (want:
      recognised as expected-fail and matched against "undefined variable").
      Confirmed Nov 27 (fb4 fid.exe, Windows). cmd_test unchanged since fb4.
      Confidence: high.
- [ ] BG456. **`fid test <file>` finds tests by scanning lines, not the
      program: a test written `@cold fn test_x()` is silently never run (the
      file "passes"), and a static method or commented-out `fn test_*()`
      breaks the build.** compiler/main.fd test_fns (after str_trim and an
      optional "pub ", the line must start with "fn test_" and "()" must
      follow the name), cmd_test_file (harness `if which == "all" || which ==
      "<name>" { <name>(); }`): test_fns looks at raw lines. (1) A test whose
      line starts with an annotation (`@cold fn test_x()`, `@slow fn ...`, any
      `@attr fn` on one line, which the parser accepts) is not collected: it
      is neither run nor counted, so a failing test of that form gives "1
      passed, 0 failed", exit 0. (2) Conversely an indented `fn test_helper()
      -> i64` inside an `impl` block, or a `fn test_old()` inside `/* ... */`,
      is collected and the harness calls it as a free function; the harness
      then fails to type-check ("undefined function 'test_helper'") and every
      test of the file is lost ("fid test: the build failed", exit 1).
      Trigger: (1) an.fd = `fn test_plain() { assert(1 + 1 == 2); }` and
      `@cold fn test_annotated() { assert(1 + 1 == 3); }` -> `fid check an.fd`
      "no errors"; `fid test an.fd` -> "ok test_plain", "1 passed, 0 failed",
      exit 0 (want test_annotated run and FAIL). (2) t.fd with `fn test_a() {
      ... }` and `impl S { fn test_helper() -> i64 { return 1; } }` ->
      "error[E041] ... undefined function 'test_helper' -->
      _fides_test_main.fd:42:36", "fid test: the build failed". Confirmed Nov
      27 (fb4 fid.exe, CC=zig cc, both cases). test_fns and cmd_test_file
      unchanged since fb4. Confidence: high.
- [ ] BG457. **verify-selfhost.sh compares failures by test name only, not by
      backend: a new failure on one backend is hidden by an old failure of the
      same test on another, and the gate passes.** verify-selfhost.sh (results
      "OUTPUT=...${name}: FAIL\n" for all three backends; CURRENT_FAILS /
      BASELINE_FAILS = `grep ": FAIL"`; NEW_FAILS loop `echo "$BASELINE_FAILS"
      | grep -qF "$line"`): each backend section writes the same "test_x.fd:
      FAIL" line, with no backend in it; the comparison greps the FAIL lines
      out of their sections. So once a test fails on any backend in the
      baseline (the saved baseline has
      probe_diag/probe_emit/probe_hm5/probe_pe_ir_stubs/test_pe_ir_gen/test_spawn_gt
      failing), it may fail on every other backend too and NEW_FAILS stays
      empty: the script prints "No regressions -- some tests fixed!" and exits
      0. The reverse (fixed on one backend, broken on another) is reported as
      nothing at all. Trigger: baseline = test_x.fd FAIL under "C Codegen"
      only; current run = test_x.fd FAIL under "Interpreter" and "PE Emitter",
      PASS under C. The script's own comparison code (run as is on those two
      texts) gives CURRENT_FAILS = two "test_x.fd: FAIL" lines, NEW_FAILS = ""
      -> exit 0 (want: the interpreter and native regressions listed, exit 1).
      Confirmed Nov 27 (the comparison block of verify-selfhost.sh run in bash
      on the two texts above). Confidence: high.
- [ ] BG458. **`fid repl` cannot evaluate an expression unless `tcc` is on
      PATH (Windows), and never on Linux/macOS: it ignores $CC and runs
      `_fides_repl_tmp` without `./`.** compiler/main.fd cmd_repl (expression
      branch: `build_cmd = "tcc -O2 -w -o " + exe_name ...` / `"gcc -O2 ..."`,
      then `p_system(exe_name)` with exe_name "_fides_repl_tmp.exe" /
      "_fides_repl_tmp"): every non-definition line is wrapped in a main,
      compiled to C, and then built with a hard-coded compiler: `tcc` on
      Windows, `gcc` on POSIX, whatever CC says (cmd_bench was fixed for
      exactly this, 2g.62: "it used to call `tcc` ... whatever CC said";
      ccompile picks $CC, gcc, clang, tcc, cl). Neither build's exit status is
      checked. On POSIX the program is then started as the bare word
      `_fides_repl_tmp`, which the shell looks up in PATH, not in the current
      directory, so even with gcc present nothing is printed ("command not
      found"). The REPL's tests (tests/tooling/repl-*-check.sh) say "Nothing
      here runs a line through C", so this is untested: the REPL's main job,
      evaluating `1 + 2`, does not work on a machine with zig cc / gcc only.
      Trigger: `CC="zig cc"`; `printf '1 + 2\n:quit\n' | fid repl` -> "'tcc'
      is not recognized as an internal or external command",
      "'_fides_repl_tmp.exe' is not recognized ...", no `3` printed (want 3).
      Confirmed Nov 27 (fb4 fid.exe, Windows, no tcc on PATH; the POSIX half
      read only, high confidence). cmd_repl unchanged since fb4.
- [ ] BG459. **`fid audit` (and `--strict`) does not see an `unsafe{` block
      written without a space: a program doing malloc, raw stores and free
      reports "0 unsafe site(s)".** compiler/main.fd audit_is_unsafe_block
      (`str_find(rest, "unsafe {")`, else only `t == "unsafe"`), used by
      cmd_audit: cmd_audit finds unsafe code by text: a line is an unsafe
      block only if it contains exactly "unsafe {" (one space) or is the bare
      word `unsafe`. The language accepts `unsafe{` (and `unsafe  {`, a tab,
      ...), which the checker treats as a normal unsafe block, but the audit
      skips it, so `fid audit --strict` (exit 1 on any unjustified site, the
      gate for "unsafe code a program reaches", AGENTS.md) passes on code with
      unjustified unsafe blocks. The tokenizer, not a substring, should
      decide. Trigger: a.fd = `fn main() { var m: i64 = 0; unsafe{ m =
      p_malloc(8); } var r: i64 = 0; unsafe{ val q: * mut i64 = m; * q = 41; r
      = * q; p_free(m); } println(r + 1); }` (one statement per line) -> `fid
      run` prints 42; `fid audit a.fd --strict` -> "0 unsafe site(s) outside
      the stdlib, 0 without a SAFETY: comment", exit 0. With `unsafe {` it
      reports the site and exits 1. Confirmed Nov 27 (fb4 fid.exe).
      audit_is_unsafe_block and cmd_audit unchanged since fb4. Confidence:
      high.
- [ ] BG460. **`fid ir` / `fid dump-ir` reject every program with a `use`
      (stdlib or a sibling module): "undefined function", "--> <stdin>".**
      compiler/main.fd cmd_ir (`lower_to_ir_module(source)`),
      compiler/compiler.fd lower_to_ir_module -> build_program(source) (no
      path): cmd_ir reads the file and lowers it through lower_to_ir_module,
      which builds with no source path, so imports are not resolved (neither
      `use string;` nor a module beside the file) and errors are attributed to
      "<stdin>". Every other dump command (dump-c, sourcemap, verify, opt,
      explain) and `run` go through the *_with_path variants and work on the
      same file. AGENTS.md's debugging workflow ("Bisect the layer ...
      dump-ir") and known-bugs.md's command table ("`ir` / `dump-ir` |
      COMPLETE") rely on it. Trigger: s.fd = `use string;` / `fn main() {
      println(str_pad_left("7", 3, 48)); }` -> `fid run s.fd` prints 007; `fid
      dump-ir s.fd` -> "error[E041]: Type error (line 4): undefined function
      'str_pad_left' --> <stdin>:4:13", exit 1. Same with `use mylib;` of a
      sibling mylib.fd. Confirmed Nov 27 (fb4 fid.exe). cmd_ir,
      lower_to_ir_module and build_program unchanged since fb4. Confidence:
      high.
- [ ] BG461. **LSP hover answers nothing in any file with a `use`: it builds
      the document without its path, so even core's `int_to_str` has no
      hover.** compiler/lsp.fd handle_hover (`build_program_with_path(text,
      OptStr::None)`, then the fallback `parse_program_only(text)`);
      handle_completion and handle_signature_help build the same way (read
      only): handle_hover type-checks the document with no source path, so a
      `use` (a stdlib module or a sibling file) cannot be resolved and the
      build fails; the fallback parses only the document itself, which has
      neither the imported functions nor core's. So in every file that imports
      anything, hovering a call of an imported function, of a core function
      (`int_to_str`, `println` helpers) or of anything not declared in the
      file returns `null`, while the diagnostics of the same document
      (publish_diagnostics, which passes the path: BG53's fix) are clean and
      definition finds a sibling module's function. Without the `use` line the
      same hover shows `**fn int_to_str(n: i64) -> str**`. Trigger: s.fd =
      `use string;` / `fn main() { println(str_pad_left("7", 3, 48));
      println(int_to_str(5)); }`; LSP initialize, didOpen (diagnostics: []),
      hover at `str_pad_left` (3:14) and at `int_to_str` (4:14) -> both
      `"result": null`; the same file without `use string;` and with only
      int_to_str -> hover gives the signature. Confirmed Nov 27 (fb4 `fid lsp`
      over stdio, Python driver). compiler/lsp.fd is identical in fb4 and the
      repo. Confidence: high.
- [ ] BG462. **`fid notebook` writes the source into HTML unescaped (a string
      `"<script>...</script>"` in the program runs in the browser) and
      evaluates nothing, though its usage says "Each code block is evaluated,
      output rendered inline".** compiler/main.fd cmd_notebook (`html + "<div
      class=\"cell code\">" + line + "</div>"`, the same for `///` lines and
      for `<title>`/`<h1>` from the file name): every source line is trimmed
      and pasted between `<div>` tags with no `&`, `<`, `>` escaping. Ordinary
      code (`if a < 2 {`) produces broken markup, and any `<script>`/`<img
      onerror=...>` text in a string literal or comment of the rendered file
      becomes live HTML: rendering someone else's .fd as a notebook and
      opening it runs their script in the viewer's browser. Nothing is run (no
      interpreter call: the "output rendered inline" of the help text never
      exists), each line is its own "cell", and indentation is lost
      (str_trim). Trigger: n.fd = `/// Compares <b>two</b> numbers` / `fn
      main() { val a = 1; if a < 2 { println("<script>alert(1)</script>"); }
      }` (one statement per line) -> `fid notebook n.fd` writes n.fd.html with
      `<div class="cell code">println("<script>alert(1)</script>");</div>` and
      `<div class="cell code">if a < 2 {</div>` verbatim, no output cell (want
      `&lt;script&gt;`, and the run's output, or no claim of it). Confirmed
      Nov 27 (fb4 fid.exe). cmd_notebook unchanged since fb4. Confidence:
      high.

### Low

- [ ] BG463. **`i64::MIN % -1` (and `i32`/`i8` MIN % -1) gives 0 on every
      backend, while the formal semantics and the fuzzer's C oracle say it
      panics.** docs/verification/semantics.md §3 integer table ("a % b |
      panic if τ signed, a = min(τ), b = −1") and scripts/fuzz/gen_vs_c.py
      `rem_{t}` (line ~802: `if (b == 0 || (a == MIN && b == -1)) panic();`)
      vs compiler/pe_ir.fd PeirGen_gen_int_div (`is_mod` with divisor -1: `xor
      rdx, rdx`), compiler/wasm.fd wg_divmod (`is_mod`: `i64.const 0`), and
      the C/interp/nat paths, which all yield 0: semantics.md says "A rule
      changed here must be changed there" (gen_vs_c.py) and both say the
      remainder panics; the backends deliberately special-case b == -1 to give
      0 without a panic (pe_ir's comment: "idiv also traps on MIN / -1, so -1
      never reaches it"). One of the two is wrong; as it stands a fuzz program
      that reaches `MIN % -1` is reported as a miscompile on every backend,
      and a program relying on the documented panic gets 0. (reference.md §3.5
      says nothing about MIN % -1.). Trigger: `fn md(a: i64, b: i64) -> i64 {
      return a % b; } fn main() { val m: i64 = -9223372036854775807 - 1;
      println(md(m, -1)); }` prints 0 on run, ccompile, emit, --nat and wasm;
      semantics.md / gen_vs_c.py expect a panic. Same with an i32 and an i8
      MIN. Confirmed Nov 27 (all five backends print 0). PeirGen_gen_int_div
      and wg_divmod unchanged between fb4 and the repo. Confidence: high.
- [ ] BG464. **`==` on two `Vec<f64>` compares bit patterns: `[0.0] == [-0.0]`
      is false and `[NaN] == [NaN]` is true.** compiler/lower.fd vec_eq_fn_for
      (`Type::F64 => "Vec_eq_words"`) with stdlib/core.fd Vec_eq_words
      (`Vec_get(a, i) != Vec_get(b, i)` on the raw words, and `if a == b {
      return true; }` for a shared buffer): proposal 0001 rule 5 makes `==` on
      a Vec element-by-element, and semantics-2 §5 F-cmp says float `==` is
      the IEEE comparison (−0.0 == 0.0; NaN equals nothing). Vec_eq_words
      compares the elements as i64 words, which is right for integers and
      bools but not for f64: the two zeros differ in bit 63 and a NaN has one
      bit pattern. (A struct with an f64 field compares right: `__eq_`
      compares the field as a float.). Trigger: `fn main() { val z: f64 = 0.0;
      var a: Vec<f64> = Vec::new(); a.push(z); var b: Vec<f64> = Vec::new();
      b.push(-z); println(a == b); var c: Vec<f64> = Vec::new(); c.push(z /
      z); var d: Vec<f64> = Vec::new(); d.push(z / z); println(c == d); }`
      prints false, true; true, false expected. Confirmed Nov 27 on `fid run`
      (the compiled backends call the same core Vec_eq_words; emit could not
      be built at the time: the machine was out of commit memory).
      vec_eq_fn_for and Vec_eq_words unchanged between fb4 and the repo.
      Confidence: high.
- [ ] BG465. **--nat Windows: `env_var` is case-sensitive, so
      `env_var("FOOBAR")` misses a variable the environment holds as `FooBar`
      (every other backend finds it, as Windows does).**
      stdlib/linux_runtime/base.fd pert_getenv (byte-exact compare of `name`
      against the environ snapshot the Windows shim builds): Windows
      environment names are case-insensitive (getenv / GetEnvironmentVariable
      ignore case; the system block holds `Path`, `windir`, `ComSpec`), and
      the C runtime and pe_ir follow that. The machine layer reuses the Linux
      lookup over the environment it copied at start-up, comparing bytes
      exactly, so on --nat Windows `env_var("PATH")` is None when the block
      says `Path` (a process started by Explorer or a fresh cmd), and so on.
      Trigger: `fn main() { match env_var("fid_long") { Option::Some(v) => {
      println("set"); } Option::None => { println("none"); } } }` with
      `FID_LONG=x` exported -> run/C/emit: `set`; emit --nat: `none`.
      Confirmed Nov 27 (all four backends, probe e1.fd). Runtime read from the
      repo. Confidence: high.
- [ ] BG466. **`random_int_range(min, max)` panics with an integer overflow
      when the range spans more than i64::MAX values (`random_int_range(0,
      i64::MAX)`, `random_int_range(i64::MIN, i64::MAX)`).** stdlib/random.fd
      random_int_range: the bound is computed as `max - min + 1` with checked
      arithmetic (the function is not @wrapping): `0, i64::MAX` overflows on
      the `+ 1`, `i64::MIN, i64::MAX` already on the subtraction. The doc
      promises "a random i64 in [min, max]" for any min <= max; the full-range
      case needs `next()` directly, the others an unsigned span. Trigger: `use
      random; fn main() { println(f"{random_int_range(0, 9223372036854775807)
      >= 0}"); }` -> every backend: "Panic: Integer overflow on addition"
      (run: "9223372036854775807 + 1 at line 174"). Confirmed Nov 27 (run, C,
      emit, emit --nat; found by two passes). Stdlib read from the repo.
      Confidence: high.
- [ ] BG467. **`fid run`: `p_file_read_at` (stdlib fs.fd's streaming File
      reads) reads the whole file into an i64 per byte, so reading 10 bytes
      near the end of a 3 GiB file panics "out of memory (allocating
      4294967304 bytes)".** compiler/interp.fd p_file_read_at branch (`var
      whole: Vec<i64> = p_file_read_bytes(fpath);` then slices it): the
      comment says the host may lack p_file_read_at so "the host reads the
      file and the slice is taken here: the same bytes, at the cost of reading
      the whole file" — but the cost is 8 bytes of memory per file byte, so
      the streaming API that exists for large files fails under `fid run` on
      any file of a few hundred MB, where every compiled backend reads only
      the bytes asked for (C, pe_ir, --nat all return 2 bytes here). Trigger:
      `truncate -s 3G big.bin`; `fn main() { val t = p_file_read_at("big.bin",
      3221225470, 10); println(f"{t.len()}"); }` -> run: "Panic: out of memory
      (allocating 4294967304 bytes)"; ccompile/emit/emit --nat: `2`. Confirmed
      Nov 27 (all four backends, probe fs1.fd). The branch is in
      Interpreter_call_runtime (file builtins); not touched by the fb4->repo
      diff of this area (the p_file_read_at block text is the same).
      Confidence: high.
- [ ] BG468. **--nat Windows: `p_system` (and `p_process_run`'s code) returns
      a negative exit code zero-extended: `exit -1` gives 4294967295, a
      crashed child 3221225477 instead of -1073741819.**
      stdlib/linux_runtime/win.fd, the syscall-1000 shell runner used by
      sys.fd pe_sh (`var r: i64 = pe_win_ld32(code); ... return r;` after
      GetExitCodeProcess): Windows exit codes are 32-bit and C's `system()` /
      pe_ir hand them back as a signed int (`-1`, `-1073741819` for an access
      violation). The machine layer reads the DWORD with pe_win_ld32
      (zero-extending) and never sign-extends, so a program that tests
      `p_system(cmd) < 0` or compares with a known NTSTATUS sees a different
      number on --nat only. Trigger: `fn main() { println(f"{p_system("exit
      -1")}"); }` -> run/C/emit: `-1`; emit --nat: `4294967295`. Confirmed Nov
      27 (all four backends, probe p1.fd). Runtime read from the repo.
      Confidence: high.
- [ ] BG469. **`p_dir_list` order depends on the backend: the machine layer
      sorts names byte by byte, the C runtime (all platforms), the interpreter
      and pe_ir return the file system's order (NTFS: case-insensitive; ext4
      readdir: hash order).** stdlib/linux_runtime/sys.fd pert_dir_list
      ("Insertion sort: the order does not depend on the file system");
      compiler/codegen_c.fd emitted `p_dir_list__raw` (FindFirstFileA /
      readdir, no sort); stdlib/pe_runtime/base.fd pert_dir_list
      (FindFirstFileA, no sort); compiler/interp.fd p_dir_list (the host's C
      runtime): the machine layer documents an order independent of the file
      system, but the other three runtimes do not sort, so the same program
      lists a directory differently under `fid run`/ccompile/emit than under
      --nat (and on Linux a C build's order is whatever readdir gives,
      differing between machines and file systems). Programs that diff or hash
      a listing (build tools, tests) get backend-dependent results. Trigger:
      `fn main() { p_mkdir("d4"); p_file_write("d4/B.txt", "1");
      p_file_write("d4/a.txt", "1"); p_file_write("d4/_u.txt", "1");
      p_file_write("d4/Z.txt", "1"); println(str_replace(p_dir_list("d4"),
      "\n", ",")); }` -> run/C/emit (Windows): `a.txt,B.txt,Z.txt,_u.txt`;
      emit --nat: `B.txt,Z.txt,_u.txt,a.txt`. Confirmed Nov 27 (all four
      backends, probe f4.fd). The C text is the same in fb4. Confidence: high.
- [ ] BG470. **The type checker accepts a call to any `p_`-prefixed name as a
      runtime builtin returning i64, so a misspelt or nonexistent builtin
      (`p_bogus_xyz(1)`, and `p_remove` / `p_time_now`, which reference.md
      §9.1 lists) passes `fid check` and only fails later, differently on each
      backend.** compiler/typechecker.fd TypeChecker_check_expr_inner, the
      call branch "Check for runtime builtin functions (p_*)" (`if
      str_starts_with(callee, "p_") { ... return Type::I64; }` with no lookup
      of the name, its arity or its argument types);
      docs/reference/reference.md §9.1 effect table (names p_remove,
      p_time_now, p_env_get that no runtime implements): the branch
      special-cases the result type of some known builtins and returns i64 for
      every other `p_` name, without checking that a runtime has it. The
      program then: `fid run` panics "Unknown function 'p_remove'"; ccompile
      fails inside clang ("call to undeclared function 'p_remove'", C source
      left behind); `emit` stops with "PE backend: not available in native
      Windows executables: p_bogus_xyz (build with `fidesc ccompile` instead)"
      — advice that cannot work since the name exists nowhere. Same branch in
      fb4. Trigger: `fn main() { p_file_write("rm_me.txt", "x");
      println(f"{p_remove("rm_me.txt")}"); }` -> `fid check`: "no errors";
      `fid run`: "Panic: Unknown function 'p_remove' at line 1"; ccompile:
      clang error; emit / emit --nat: build fails.
      `println(f"{p_bogus_xyz(1)}")` behaves the same. Confirmed Nov 27
      (check, run, ccompile, emit, emit --nat; probes rm1.fd, rm2.fd, rm3.fd).
      Branch unchanged since fb4. Confidence: high.
- [ ] BG471. **`hash_mix64` is documented as the splitmix64 finalizer but uses
      arithmetic shifts, so it computes a different function; its own doc
      example (`hash_mix64(0) == -280303742238208354`) is false.**
      stdlib/hash.fd hash_mix64 (same construction in stdlib/random.fd sm64,
      "SplitMix64"): splitmix64 uses logical right shifts (`z ^ (z >>> 30)`
      ...); on a signed i64 Fides `>>` is arithmetic (`-16 >> 2` is -4), so
      negative intermediates are sign-filled. hash_mix64(0) is
      2447048557682778308 on fid run and C, real splitmix64(0) is
      0xE220A8397B1DCDAF (-2152535657050944081), and the doc example claims a
      third value, so the `/// Example:` assert fails. Anyone porting a
      splitmix64-seeded table or sequence gets different numbers. Trigger:
      `use hash; fn main() { println(f"{hash_mix64(0)}"); }` ->
      2447048557682778308 (run and ccompile); doc says -280303742238208354;
      reference splitmix64 gives -2152535657050944081 (Python with logical
      shifts). Fix: mask after each shift (`(z >> 30) & 0x3FFFFFFFF`) or use
      u64. Confirmed Nov 27 (fid run, C backend). Confidence: high.
- [ ] BG472. **The `uuid_parse_opt` doc example does not compile (statement
      `match` with expression arms), so `scripts/doctest.py` fails on
      stdlib/uuid.fd; the module header also says uuid_v4 is "not
      cryptographically secure" though it reads OS entropy.** stdlib/uuid.fd
      uuid_parse_opt doc comment (`/// Example:` at line 166), module header
      lines 3-5: the example is `match uuid_parse_opt("zz") { Option::Some(u)
      => println("bad"), Option::None => println("ok") }` as a statement;
      reference.md §(match, line ~1005) says "A statement `match x { 1 => f(),
      _ => g(), }` is a parse error", and fid rejects it. Separately the
      header comment ("Uses the runtime's random number generator (not
      cryptographically secure). For secure UUIDs, use a crypto-secure RNG.")
      contradicts uuid_v4's own doc and code (p_os_random_u64), steering users
      away from a function that is fine. Trigger: `python3 scripts/doctest.py
      <dir with uuid.fd> --fid fid.exe` -> "FAIL uuid.fd:7 Compilation error";
      the snippet alone gives "Parse error at 4:28: expected { but got
      'println'". (The same doctest run over 27 crypto/encoding modules, 90
      examples, fails only this one and hash.fd's hash_mix64 example.)
      Confirmed Nov 27 (fid run via scripts/doctest.py). Confidence: high.
- [ ] BG473. **`chacha20poly_decrypt` (tls_crypto) and `cb_gcm_decrypt`
      (crypto_bind) return an empty Vec both for a forged ciphertext and for a
      genuine empty plaintext, so a caller cannot tell an authentication
      failure from an authenticated empty message.** stdlib/tls_crypto.fd
      chacha20poly_decrypt ("returns plaintext Vec, or empty on tag
      mismatch"); stdlib/crypto_bind.fd cb_gcm_decrypt ("empty Vec on failure
      (tag mismatch included)"), likewise crypto_bind_unix.fd: an AEAD open
      must report authentication separately from the data
      (chacha20poly1305.fd's AeadOpened and aes_gcm.fd's GcmOpened do, with
      `ok`). These two return only the bytes; for a zero-length plaintext
      (legal: keepalives, empty records, empty HPKE/Noise payloads) the
      successful result is identical to the failure result. A caller that maps
      "empty" to "empty message" accepts any forged 16-byte tag as a valid
      empty message; one that maps it to "failure" rejects genuine empty
      messages. Trigger: key 0..31, nonce 0..11, empty AAD:
      `chacha20poly_decrypt(key, nonce, [], chacha20poly_encrypt(key, nonce,
      [], []))` -> len 0; with the tag's first byte flipped -> len 0. Same
      with cb_gcm_decrypt over BCrypt (sealed 16 bytes; genuine -> 0, forged
      -> 0). Confirmed Nov 27 (C backend, Windows BCrypt). Confidence: high.
- [ ] BG474. **`ws_recv` accepts frames RFC 6455 requires a peer to fail on,
      and silently merges two messages when a new data frame arrives inside a
      fragmented one.** stdlib/ws.fd ws_recv: the frame loop never checks
      RSV1-3 (must be 0 without an extension, §5.2), reserved opcodes 3-7 and
      0xB-0xF (§5.2), that control frames have FIN set and at most 125 bytes
      (§5.5), that a server receives only masked frames and a client only
      unmasked ones (§5.1, the masking that protects intermediaries), or UTF-8
      in text messages (§8.1). In the reassembly branch `else { first_op = op;
      ... acc.push(...) }` a text/binary frame that arrives while a fragmented
      message is still open (FIN=0 seen, no final continuation yet) is
      appended to the same buffer, so two messages come back as one with the
      second one's opcode; §5.4 says this must fail the connection. (BG354
      covers only the handshake.). Trigger: read-only (needs a socket peer):
      server-side ws_recv on bytes `01 03 'a' 'b' 'c'` (text, FIN=0, unmasked)
      then `82 02 'x' 'y'` (binary, FIN=1, unmasked) returns one frame, opcode
      2, bytes "abcxy", instead of failing (unmasked client frames,
      interleaved message). Read only. Confidence: high.
- [ ] BG475. **`crypto_sha256_bytes` writes only the low 32 bits of the
      message bit length, so SHA-256 (and HMAC/PBKDF2/HKDF built on it) is
      wrong for inputs of 512 MiB or more.** stdlib/crypto.fd
      crypto_sha256_bytes (padding: four literal `padded.push(0)` for the high
      word, then `(bit_len >> 24) & 0xFF` ... for the low word): FIPS 180-4
      §5.1.1 appends the 64-bit big-endian bit length. bit_len = msg_len * 8
      exceeds 2^32 from 2^29 bytes on; the code drops bits 32..63 ("we use
      32-bit length * 8 ... High 32 bits (0 for short messages)"), so the
      digest of any message >= 536,870,912 bytes is computed over a wrong
      final block. The other hashes in the stdlib (kdf_sha1, sha512_bytes,
      ec_sha384, md5) all write the full 64-bit length. Latent today (a
      Vec<i64> of that size is 4 GiB) but the function is the SHA-256 every
      module uses (TLS, SSH, HKDF, PBKDF2, file checksums in ota.fd). Trigger:
      read-only (needs a 512 MiB input): e.g. `crypto_sha256_bytes` of 2^29
      zero bytes should be the SHA-256 Python gives for b"\0" * 2**29; here
      the length block encodes 0 instead of 2^32 bits. Fix: push `(bit_len >>
      56) & 255` ... `(bit_len >> 32) & 255` instead of the four zeros. Read
      only. Confidence: high.
- [ ] BG476. **encoding.fd `utf8_len` was left out of the BG215 fix: it still
      counts an overlong/surrogate/out-of-range sequence as one character, so
      `utf8_len(s) != utf8_decode(s).len()` and its own doc ("Invalid bytes
      each count as one (replacement) character") is false.**
      stdlib/encoding.fd utf8_len (skips `utf8_seq_len(b0)` bytes without
      calling utf8_seq_ok, which utf8_valid_bytes and utf8_decode share since
      BG215): for a lead byte with a plausible length but invalid continuation
      range (E0 9F 80 overlong, ED A0 80 surrogate, F0 8F 80 80 overlong, F4
      90 80 80 > U+10FFFF) utf8_len advances over the whole sequence and
      counts 1, while utf8_decode emits one U+FFFD per byte (3 or 4). Code
      that sizes a buffer or slices by utf8_len and then indexes utf8_decode's
      result is off. This is the encoding.fd sibling of BG346 (unicode.fd) — a
      different module and function, missed when BG215 added utf8_seq_ok.
      Trigger: `utf8_len(enc_bytes_to_str(hex_decode_bytes("e09f80")))` -> 1,
      `utf8_decode(...)` -> 3 code points; same for "eda080" (1 vs 3),
      "f08f8080" (1 vs 4), "f4908080" (1 vs 4). Valid input agrees. Confirmed
      Nov 27 (fid run). Confidence: high.
- [ ] BG477. **SSH: after the first key exchange, an SSH_MSG_NEWKEYS that
      arrives before a re-key's ECDH (or outside any re-key) is accepted,
      re-installs the old keys as "new" and completes the key exchange,
      because pending_in_key is never cleared.** stdlib/ssh.fd ssh_on_newkeys
      (guard `if s.pending_in_key.len() != 64` only), ssh_kex_done /
      ssh_send_kexinit (neither clears pending_in_key / pending_out_key),
      ssh_feed (NEWKEYS dispatched in any state): the guard is meant to reject
      "NEWKEYS before the key exchange", but it only works for the first
      exchange: from then on pending_in_key holds the previous exchange's
      64-byte key. A NEWKEYS received after the peer's KEXINIT but before
      KEX_ECDH_REPLY/INIT sets in_key to the old key, sets got_newkeys, and
      (with sent_newkeys) runs ssh_kex_done, so the inbound direction never
      moves to the re-key's keys; a NEWKEYS with no re-key in progress
      likewise bumps kex_count and, with strict KEX, resets in_seq to 0. RFC
      4253 §7.3 and OpenSSH treat NEWKEYS outside the exchange as a protocol
      error. Trigger: read-only: after ssh_ready, peer sends KEXINIT then
      NEWKEYS (no ECDH): ssh_on_newkeys finds pending_in_key.len() == 64 and
      proceeds. Fix: clear pending keys in ssh_kex_done and ssh_send_kexinit,
      or track the exchange state explicitly. Read only. Confidence: high
      (logic; not exercised).
- [ ] BG478. **`cbor_diag` prints a float of magnitude >= 2^53 as a bare
      integer ("340282346638528859811704183484516925440", 1e300 as 301
      digits), so the diagnostic notation names an integer, not a float.**
      stdlib/cbor.fd cbor_float_diag (the ".0" suffix is added only below
      2^53; above it float_to_str's plain integral digits are returned): RFC
      8949 §8 diagnostic notation distinguishes floats by a decimal point or
      exponent (Appendix A: `3.4028234663852886e+38`, `1.0e+300`); a digit
      string without either is an unsigned integer (a bignum here). Every
      integral float of 2^53 or more therefore prints as a different CBOR
      item; the module header only warns that the last digits may differ.
      Trigger: `cbor_put_float(&mut o, 3.4028234663852886e+38)` (encodes
      correctly, fa7f7fffff) then `cbor_diag` ->
      "340282346638528859811704183484516925440"; 1.0e+300 (fb7e37e43c8800759c)
      -> a 301-digit integer. Expected "3.4028234663852886e+38" / "1.0e+300"
      (or at least a trailing ".0"). Confirmed Nov 27 (fid run). Confidence:
      high.
- [ ] BG479. **inflate accepts a dynamic block whose Huffman code lengths are
      over-subscribed (an invalid prefix code) and decodes it, where zlib
      rejects the stream ("invalid literal/lengths set").** stdlib/compress.fd
      huff_build (counts lengths and builds the tables with no Kraft check),
      z_dynamic / z_inflate (use the table as is): RFC 1951 code lengths must
      describe a prefix code; zlib's inflate_table (and puff, which this
      decoder follows) rejects over-subscribed sets and incomplete ones
      (except a single distance code). Here an over-subscribed set silently
      makes some symbols unreachable and decoding continues, so the same bytes
      mean "error" to zlib/browsers/proxies and data to this decoder: a parser
      differential for anything that inspects or filters compressed content
      (gzip HTTP bodies, PNG IDAT, zlib-wrapped formats). Trigger:
      Python-built raw deflate stream: dynamic block, literal lengths {65:1,
      256:1, 66:2} (1/2+1/2+1/4 > 1), one distance code, data bits 0,0,1 ->
      `zlib.decompress(raw, -15)` raises "invalid literal/lengths set";
      `inflate(raw)` returns 2 bytes "4141" ("AA"). Expected: empty (error).
      Confirmed Nov 27 (fid run; probe r2/crypto/p27.fd, data
      r2/crypto/over.hex). Confidence: high.
- [ ] BG480. **A `fn bench_*()` that prints anything is reported as
      "10000000000 runs  0 ns/op": the timing is read from the bench's whole
      stdout.** compiler/main.fd cmd_test_file (bench loop: `us =
      str_parse_int(str_trim(c_file_read("_fides_bench_out.txt")))`, `if us >=
      200000 || reps >= 1073741824 ... else if us < 1000 { reps = reps * 100;
      }`): the harness prints the elapsed microseconds after the bench's own
      output, into the same file; str_parse_int of "warming up\n123" is 0, so
      `us < 1000` always holds and reps grows x100 (1, 100, ... 1e8, 1e10).
      The limit test `reps >= 1073741824` runs only after a run, so the last
      run is made with reps = 10^10 (a real bench then runs for hours), and
      the report divides the parsed 0 by it: "0 ns/op". Trigger: b.fd = `var
      G: i64 = 0;` `fn bench_print() { G = G + 1; if G == 1 { println("warming
      up"); } }` plus any test_; `fid test b.fd` -> "bench_print  10000000000
      runs  0 ns/op" (want the real time per op; the bench's output kept apart
      from the timing line). Confirmed Nov 27 (fb4 fid.exe, CC=zig cc).
      cmd_test_file unchanged since fb4. Confidence: high.
- [ ] BG481. **`fid explain --alloc` says "no heap allocations" for a function
      whose string `+` allocates: it knows `str_concat`, not the
      `p_str_concat` / `p_str_concat_int` the lowering calls.**
      compiler/checks.fd explain_alloc_kind (name list: "Str_dup",
      "str_concat", "String_from", `*_to_str`, ...), used by explain_allocs: a
      string `a + b` the LF13 stack area cannot hold is lowered to
      `p_str_concat` (and `s + int_to_str(n)` to `p_str_concat_int`), both
      counted heap strings (lower.fd lists p_str_concat among the allocating
      builtins and the fresh-string producers). explain_alloc_kind matches the
      name "str_concat", which nothing calls, so these sites are never listed;
      a function whose only allocation is such a concat is reported as "no
      heap allocations", and CP2's "where a function allocates" misses the
      commonest heap allocation of string code. (Str_concat_into results that
      fit the stack area are rightly not listed.). Trigger: s.fd = `fn
      greet(name: str, n: i64) -> str { val a = "hello " + name; val b = f"{a}
      #{n}"; return b + "!"; }` + a main printing `greet("bob", 3)`; `fid
      explain s.fd --alloc greet` -> "no heap allocations", while `fid explain
      s.fd --stages greet` shows `%31 = call p_str_concat(%29, %30): *u8 ;
      line 4` (the returned heap string). Likewise main of `val msg = "sum " +
      int_to_str(s);` (p_str_concat_int). Confirmed Nov 27 (fb4 fid.exe).
      explain_alloc_kind / explain_allocs unchanged since fb4. Confidence:
      high.
- [ ] BG482. **`ark remove` of a source pact leaves ark_modules/<name>.fd in
      place (and `ark install` keeps it), so `use <name>;` still builds after
      the dependency is gone.** compiler/ark.fd ark_cmd_remove (only
      `ark_bin_drop_iface(...)`, which deletes a file only when
      ark_bin_is_iface says it is a prebuilt interface); ark_install_picks
      (installs what ark.toml lists, never prunes ark_modules/): remove
      rewrites ark.toml and drops a binary interface, nothing else. The
      installed copy of a path/git/registry pact stays in ark_modules/,
      ark.lock keeps its entry until the next install, and the next `ark
      install` ("No pacts to install.") empties ark.lock but still leaves the
      copy. Since `use` finds pacts in ark_modules/, the program keeps
      compiling against a dependency the manifest no longer has: it builds on
      this machine and fails on a clean checkout. Trigger: `fid ark init app;
      fid ark add libx --path ../libx/libx.fd; fid ark install; fid ark remove
      libx; fid ark install` -> ark_modules/libx.fd still there; main.fd =
      `use libx;` `fn main() { println(lx()); }` -> `fid run main.fd` prints
      8, exit 0 (want: the pact's copy removed, `use libx` unresolved).
      Confirmed Nov 27 (fb4 `fid ark`, Windows). compiler/ark.fd identical in
      fb4 and the repo. Confidence: high.
- [ ] BG483. **`fid --help`, `fid help` and `fid -h` print "Unknown command"
      and exit 1; the usage text never lists `lsp`, `repl`, `ark`, `notebook`
      or `test-rules`.** compiler/main.fd main (command search skips every
      `--` argument, so `--help` leaves `start == n` and `command = p_argv(n)`
      = ""; no `help`/`-h` case), print_usage: `fid --help` prints "Unknown
      command: " with an empty name, then the usage on stderr, exit 1 (`ark
      help`/`--help` work: ark_main has the case). The usage lists ~45
      commands but not `fid lsp` (the editors' command, syntax.md), `fid
      repl`, `fid ark` (docs/reference/ark.md: "`fid ark <command>`"), `fid
      notebook` or `fid test-rules`, all dispatched by main. A global flag
      alone (`fid --borrowck=off`) gives the same empty "Unknown command: ".
      Trigger: `fid --help; echo $?` -> stdout "Unknown command: " (nothing
      after it), exit 1; `fid help` -> "Unknown command: help", exit 1; `fid
      2>&1 | grep -c 'fid lsp\|fid repl\|fid ark'` -> 0. Confirmed Nov 27 (fb4
      fid.exe). main's dispatch and print_usage unchanged since fb4 (main.fd
      differs from fb4 only in cmd_fmt and cmd_ccompile). Confidence: high.
- [ ] BG484. **`fid fmt` writes `opt ??0` (no space after `??`) and `val(a, b)
      = ...` for a destructuring `val`, and that becomes the style `fmt
      --check` enforces.** compiler/fmt.fd token spacing (try_multi_punct
      knows `??` as one 2-char operator; where its right-hand space and the
      space after `val` before `(` are lost was not traced further):
      formatting `println(opt ?? 0);` gives `println(opt ??0);`, and `val (t0,
      t1) = (1, "x");` gives `val(t0, t1) = (1, "x");`. Both still parse and
      run the same (meaning kept, idempotent), but the formatter's output is
      what fmt --check and scripts/fmt-check.sh require of the tree, so
      correctly spaced code would be reported unformatted once it uses `??` or
      a tuple `val`. Trigger: t2.fd containing `val opt: Option<i64> =
      Option::Some(4);` `println(opt ?? 0);` and `val (t0, t1) = (1, "x");` ->
      `fid fmt t2.fd` prints `println(opt ??0);` and `val(t0, t1) = (1,
      "x");`; `fid run` output identical before/after. Confirmed Nov 27 (fb4
      fid.exe). fmt.fd differs from fb4 only in the f-string brace scan
      (BG211), not in operator spacing. (Also checked: 33 tests/bootstrap/*.fd
      copies formatted, run before/after and formatted twice: all identical
      and idempotent, one 60 s timeout under load.). Confidence: high.
- [ ] BG485. **On the Linux release archive `fid explain-error E044` prints
      only "docs/reference/error-codes.md has its meaning": the archive ships
      no docs/, and that is the text explain-error reads.**
      .github/workflows/release.yml job `linux`, step "Pack
      fid-x86_64-linux.tar.gz" (bin/fid, stdlib/, runtime/, LICENSE only);
      compiler/compiler.fd explain_error_text / error_codes_doc_path (looks
      for <stdlib>/../docs/reference/error-codes.md, $FID_HOME, ./docs, <exe
      dir>/docs): explain-error has no built-in text: it prints the code's
      section of docs/reference/error-codes.md, and falls back to one line
      naming that file when it cannot find it. The Windows installer ships
      docs\ (Section "Documentation"), the Linux tarball does not, so on an
      installed Linux fid every code gives the fallback, which points the user
      at a file that is not on the machine. error-codes.md says "`fid
      explain-error E044` (or `fid explain E044`) prints a code's entry here",
      and every error message's code is meant to be looked up this way (TL2).
      Trigger: the archive's layout (bin/fid.exe + stdlib/, no docs/, FID_HOME
      unset, run outside a checkout) -> `bin/fid explain-error E044` prints
      "E044 (TypeMismatch): docs/reference/error-codes.md has its meaning, an
      example and the fix." (want the "## E044 — Type mismatch" entry with its
      example and Fix). Confirmed Nov 27 (fb4 fid.exe copied into that layout
      on Windows; the release.yml pack step read). explain_error_text
      unchanged since fb4. Confidence: high.
- [ ] BG486. **`fid run p.fd --crash-report` reports the user program's own
      panic as a crash of fid ("=== fid crash report === ... Please report
      it"), and `--reduce` then shrinks the user's program.** compiler/main.fd
      crash_report_run (`panic_at` = the first stderr line containing
      "Panic"/"panicked"/...; `crashed = panic_at >= 0 || rc < 0 || rc > 1`):
      the child's stderr is scanned for "Panic" with no regard to whose panic
      it is. Under `fid run` the interpreted program's runtime panic ("Panic:
      Vec index out of bounds at line 4") is on that same stderr, so every
      panicking program run with --crash-report gets a compiler crash report:
      "stage: before the first stage", the program's trace lines printed as
      "in fid:", a `fid reduce ... --match "Panic: ..."` command and "Please
      report it with the reduced program"; with `--reduce` the reducer runs on
      the user's (correct) program. RL20 means the flag for compiler crashes
      ("the stage and the function it failed in"). Trigger: p.fd = `fn main()
      { println("start"); val v: Vec<i64> = Vec::new(); println(v[3]); }` ->
      `fid run p.fd --crash-report` prints the program's panic, then "=== fid
      crash report ===", "stage: before the first stage ...", "in fid: →
      main", "reduce: fid reduce p.fd --cmd ...", "Please report it ...", exit
      1 (want: the program's panic only; no fid crash). Confirmed Nov 27 (fb4
      fid.exe). crash_report_run unchanged since fb4. Confidence: high.
- [ ] BG487. **`fid fmt --check` can never pass on a CRLF file with a
      multi-line string literal (and `-w` says "formatted" every time): the
      BG124 comparison strips the CRs inside the string too.**
      compiler/main.fd cmd_fmt (`var same: bool = formatted ==
      str_replace(source, "\r", "");`): the formatter writes LF line endings
      but keeps a string literal's bytes as they are, so a literal that spans
      a CRLF line break keeps its `\r` in `formatted`. The "same" test removes
      every CR from the source, including that one, so formatted (with the CR)
      never equals the stripped source: `--check` reports "would reformat"
      (exit 1) on the file fmt itself just wrote, and each `-w` rewrites it
      and says "formatted". Only the line-ending CRs should be ignored (or
      both sides normalised the same way). A Windows checkout with
      core.autocrlf=true turns every multi-line string of the tree into this
      case. Trigger: crlf.fd = `fn main() {\r\n    val s = "a\r\nb";\r\n
      println(s);\r\n}\r\n`; `fid fmt crlf.fd -w` -> "formatted"; `fid fmt
      crlf.fd --check` -> "would reformat", exit 1; `fid fmt crlf.fd` (stdout)
      is byte-identical to the file; again `-w` -> "formatted". The LF version
      of the same file -> "already formatted". Confirmed Nov 27 (fb4 fid.exe).
      The `same` line is unchanged between fb4 and the repo (cmd_fmt differs
      only in its argument parsing, BG237). Confidence: high.
- [ ] BG488. **The LSP server ignores the shutdown handshake: `exit` without a
      prior `shutdown` ends with status 0, and requests after `shutdown` are
      still served.** compiler/lsp.fd lsp_run (method dispatch: `"shutdown"`
      only answers `null`; `"exit"` always `return 0`; no shutdown flag): the
      LSP spec says the server exits with 0 only if `shutdown` was received
      first, else with 1, and that requests after `shutdown` get an
      InvalidRequest error. lsp_run keeps no state: `exit` returns 0 either
      way, and every request after `shutdown` is handled normally. A client
      (or editor test) cannot tell an orderly stop from a server that was
      killed mid-session, and a buggy client that sends edits after shutdown
      gets answers. Trigger: over stdio: initialize, didOpen, hover, then
      `exit` with no `shutdown` -> the process ends with exit code 0 (want 1).
      Confirmed Nov 27 (fb4 `fid lsp`, Python driver: "exit code: 0"); the
      after-shutdown half read only. compiler/lsp.fd identical in fb4 and the
      repo. Confidence: high.
- [ ] BG489. **An enum-variant pattern against a scrutinee that is no enum
      still checks clean and crashes at runtime — the BG129 guard only fires
      when the scrutinee is a known enum.** compiler/typechecker.fd
      TypeChecker_check_enum_pattern (~26038): the BG129 fix resolves the
      pattern's enum, then errors only under `if scrut != "" && scrut !=
      ename`, where `scrut` comes from TypeChecker_scrutinee_enum. For a
      scrutinee of any non-enum type (`str`, `i64`, a Vec) that helper
      returns `""`, so the guard is skipped entirely; the pattern then
      passes the variant-arity check below and lowering emits a tag read
      and a payload load against a value that has neither. Safe code gets
      past `fid check` and dereferences whatever the first word of the
      scrutinee is. Same hole BG129 fixed for enum-vs-enum, still open for
      enum-vs-anything-else; the entry stays a soundness bug, not a
      diagnostic nicety. Trigger: `fn main() -> i64 { val s: str =
      "hello"; match s { Result::Ok(v: i64) => { println(v); return v; } _
      => { return 0; } } }`: `fid check` reports no error; `fid run`
      crashes "Panic: Invalid memory access (null or dangling pointer)".
      Confirmed Nov 27 (`fid check` clean, `fid run` crash, release
      fid.exe); TypeChecker_check_enum_pattern read in the current source
      and the `scrut == ""` path confirmed open. Confidence: high.
- [ ] BG490. **A generic call whose argument is a method call takes the
      type parameter from the method's receiver, not its result:
      `assert_eq(0, q.len())` with `q: Vec<str>` is instantiated as
      `assert_eq__Vec__str`.** compiler/mono.fd / compiler/typechecker.fd
      (inference of a call's type arguments from an argument that is
      itself a call): `fid explain k3.fd --stages main` lowers both
      `assert_eq(q, q)` and `assert_eq(0, q.len())` to
      `call assert_eq__Vec__str(...)`, and the second one hands the
      literal `0` and the length to a body that compares two vectors.
      It passes while both words happen to compare equal as vectors and
      crashes otherwise: `val p = args_positionals(); assert_eq(0,
      p.len()); println("ok");` (`use args;`) dies "Panic: Invalid
      memory access (null or dangling pointer)" on run, emit and
      ccompile with every build since the Nov 25 tree (release/fid-dev.exe
      of Nov 25 21:50, the Nov 27 builds); the pinned Nov 21 stage 0
      prints "ok". Found by the doc examples step of CI (`scripts/doctest.py
      stdlib`: stdlib/args.fd example 16), which no hosted runner has
      reached from Nov 23 to Nov 27 (the build step before it died).
      `assert_eq(args_positionals_len(), 0)` (a plain call) is right: the
      receiver's type is taken when the argument is a method call. The
      font example that failed in the same step is not this bug: a call
      of a plain function is lowered as `assert_eq__i64`, and that
      example only had a wrong constant. The args example was rewritten
      as `assert(args_positionals_len() == ps.len())` so the doc step
      does not wait for the fix; the bug stays. Same family as BG271
      (the parameter taken from the wrong argument).
      Confirmed Nov 27 on release/fid-night.exe and release/stage0/fid.exe;
      the IR read with `fid explain --stages main`. Confidence: high.
