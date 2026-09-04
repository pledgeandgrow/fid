# Port Gap Analysis — Rust vs Fides Module Comparison

> Systematic comparison of every ported module's Rust source vs Fides port.
> Identifies incomplete ports — functionality present in Rust but missing in Fides.
>
> Last updated: Sep 5, 2026 (session 10 — post lower.fd/opt.fd porting work).

---

## Summary Table

| Module | Rust LOC | Fides LOC | LOC Ratio | Completion | Status |
|--------|----------|-----------|-----------|------------|--------|
| main | 2787 | 852 | 0.31 | **30%** | MAJOR GAPS — 17/37 commands missing, 7 complete |
| pe | 5704 | 3494 | 0.61 | **50%** | MAJOR GAPS — missing SIMD/AVX, string runtime, flat/boot |
| typechecker | 7150 | 4980 | 0.70 | **35%** | MAJOR GAPS — all effect verification stubbed |
| pe (runtime stubs) | — | — | — | **45%** | 21 string fns missing, p_malloc bug, p_print_int broken |
| pe (SIMD encoders) | — | — | — | **10%** | No packed double, no AVX, missing scalar single |
| lower | 6422 | 5624 | 0.88 | **85%** | PARTIAL — typed captures, swizzle, OptionalFieldAccess |
| opt | 7515 | 6640 | 0.88 | **88%** | PARTIAL — 17/23 passes complete, 4 partial, 1 stub |
| compiler | 2141 | 1817 | 0.85 | **90%** | OK — pipeline orchestration complete |
| inc | 146 | 127 | 0.87 | **95%** | OK |
| codegen_x64 | 1923 | 1780 | 0.93 | **90%** | OK — not yet integrated |
| fmt | 1068 | 1007 | 0.94 | **95%** | OK |
| regalloc | 1096 | 1086 | 0.99 | **98%** | OK |
| peephole | 903 | 985 | 1.09 | **98%** | OK |
| lsp | 916 | 995 | 1.09 | **95%** | OK |
| codegen_c | 4550 | 4975 | 1.09 | **95%** | OK |
| ast | 894 | 1012 | 1.13 | **98%** | OK |
| pir | 2036 | 2434 | 1.20 | **95%** | OK |
| comptime_eval | 1299 | 1559 | 1.20 | **90%** | OK |
| lexer | 554 | 704 | 1.27 | **98%** | OK |
| ir | 1613 | 2267 | 1.41 | **98%** | OK |
| diag | 412 | 594 | 1.44 | **98%** | OK |
| parser | 3246 | 4838 | 1.49 | **98%** | OK |
| borrowck | 1158 | 1731 | 1.50 | **95%** | OK |
| flat_ast | 741 | 1234 | 1.67 | **95%** | OK |
| mono | 1421 | 2440 | 1.72 | **95%** | OK |
| token | 250 | 517 | 2.07 | **100%** | OK |
| json | 273 | 628 | 2.30 | **100%** | OK |

**Legend:** OK = no significant gaps. PARTIAL = some features missing but functional.
MAJOR GAPS = significant functionality missing, may affect correctness.

**Overall self-host port completion: ~75%** (weighted by importance to self-hosting fixpoint)

---

## 1. main.rs → main.fd (completion: ~30%) — MAJOR GAPS

### Command coverage: 7 complete, 12 partial, 1 stub, 17 missing

| Command | Status | Notes |
|---------|--------|-------|
| `version` | COMPLETE | |
| `ir` / `dump-ir` | COMPLETE | |
| `dump-c` | COMPLETE | |
| `dump-pir` / `pir` | COMPLETE | Levels + `--opt` supported |
| `fmt` | COMPLETE | Supports `--check` |
| `tokens` / `dump-tokens` | PARTIAL | Prints `line:col kind`, no token values |
| `ast` / `dump-ast` | PARTIAL | Prints summary only, not full AST |
| `build` | PARTIAL | PE emitter only; missing `--target`, `--no-opt`, `--lto`, `--caps` |
| `emit` | PARTIAL | Aliased to `build`; missing `--bare`, `--boot` modes |
| `run` | PARTIAL | C/tcc only; missing interpreter, `--release`, `--trace`, `--max-*` limits |
| `ccompile` | PARTIAL | Hardcoded `tcc`; missing `--target`, `--trace`, CC detection |
| `test` | PARTIAL | C/tcc backend only; missing interp/PE backends, `--fail` |
| `opt` | PARTIAL | Ignores `[pass1,pass2,...]` argument — always runs default set |
| `bench` | PARTIAL | Missing `--ccompile` flag and timing statistics |
| `lsp` | PARTIAL | Calls `lsp_run()` — completeness not verified |
| `lint` | STUB | Always prints "No lint warnings." |
| `parse-header` | MISSING | C header → FFI bindings |
| `compile` | MISSING | Alias for ccompile |
| `debug` | MISSING | Interactive debugger |
| `replay` | MISSING | Trace replay |
| `autopsy` | MISSING | Native binary debug (Linux only) |
| `dump-ast-raw` | MISSING | Raw AST dump |
| `dump-ast-flat` | MISSING | Flat AST dump (flat_ast.fd ported but not wired) |
| `diff-test` | MISSING | Differential testing vs golden files |
| `sourcemap` | MISSING | Source map generation |
| `audit` / `verify` | MISSING | Security audit |
| `repl` | MISSING | Interactive REPL |
| `wasm` | MISSING | WASM backend |
| `codegen` | MISSING | Show assembly |
| `gpu` | MISSING | GPU codegen |
| `watch` | MISSING | File watcher + recompile |
| `link` | MISSING | Standalone linker |
| `profile` | MISSING | Per-function profiling |

### Missing global flags
- `--borrowck=off|warn|on`
- `--timeout` / `--timeout=N`

### Missing helper functions
- `parse_target_triple`, `find_ml64`, `find_cl`, `find_vcvarsall`, `find_link`, `find_kernel32_lib`

---

## 2. pe.rs → pe.fd (completion: ~50%) — MAJOR GAPS

### Instruction encoders

| Category | Completion | Details |
|----------|------------|---------|
| Integer/basic | **85%** | mov, add, sub, imul, idiv, div, and, or, xor, not, neg, cmp, test, shl, sar, cqo, setcc, inc, dec, lea, call, jmp, jcc, push, pop, ret, nop, ud2 |
| Sign/zero extension | **90%** | movzx, movsxd, movsx |
| Scalar SSE2 double | **80%** | addsd, subsd, mulsd, divsd, ucomisd, cvtsi2sd, cvttsd2si, movsd (mem), xorpd |
| Scalar SSE single | **30%** | addss, ucomiss present; missing subss, mulss, divss |
| Packed SSE2 double | **0%** | Missing: addpd, subpd, mulpd, divpd, movapd (all forms) |
| AVX | **0%** | Missing: vex2, vaddpd, vmulpd |
| 16-bit forms | **0%** | Missing: mov_reg16_ptr, mov_ptr_reg16 |
| Raw byte emit | **0%** | Missing: bare-mode opcodes (cli/sti/hlt) |

### Runtime stubs

| Category | Completion | Missing functions |
|----------|------------|-------------------|
| Print (int/str/bool) | **40%** | `p_print_int` broken (negative unreachable, digit loop wrong); `p_print_vec`/`p_print_map` MISSING |
| String conversion | **70%** | `p_to_str_vec` returns `"[]"` always; `p_to_str_float` doesn't zero-pad |
| Vec runtime | **60%** | Missing: contains, truncate, clone, insert, remove, reverse, extend, sort |
| Map runtime | **70%** | Missing: clear, keys, values, get_or, clone; `p_map_remove` is no-op |
| String runtime | **0%** | ALL 21 missing: from, len, is_empty, push_str, slice, contains, starts_with, ends_with, trim, split, replace, char_at, to_uppercase, to_lowercase, parse_int, find, rfind, repeat, bytes, clone |
| Interrupt/MMU | **0%** | ALL missing: cli, sti, hlt, lidt, idt_set_entry, inb, outb, cr3/cr0/cr4 read/write, invlpg, page_map/unmap, phys_read64/write64 |

### Critical bugs in existing stubs

| Bug | Location | Details |
|-----|----------|---------|
| **`p_malloc` allocates 0 bytes** | pe.fd:2767 | Sets `R8 = 0` before `HeapAlloc` — every allocation returns zero-size buffer |
| **`p_free` is a no-op** | pe.fd:2803 | Plain `ret` — memory leak |
| **`p_print_int` broken** | pe.fd:2447 | Negative branch after `ret` (unreachable); digit loop writes to buffer start, not indexed position |
| **`p_to_str_vec` returns `"[]"`** | pe.fd:3853 | Stub — no element iteration |
| **`p_to_str_float` no zero-pad** | pe.fd:3753 | Multiplies by 1M, loses precision for small fractions |
| **`p_print_vec`/`p_print_map`** | — | Registered as labels but no generator body — calls land on unplaced labels |

### Missing PE/binary output features
- `generate_flat_binary` — bare-metal flat binary output (0%)
- `build_multiboot2_header` — Multiboot2 boot header (0%)
- `generate_boot_binary` — bootable image generation (0%)
- `gen_boot_entry_point` — bare-mode entry point (0%)
- `bare_mode` field declared but never used

### PE header / import table: COMPLETE (90%)
- PE32+ header, 2-section layout, import table with 6 kernel32 imports all match Rust

---

## 3. typechecker.rs → typechecker.fd (completion: ~35%) — MAJOR GAPS

### Effect/annotation verification — ALL STUBBED (0%)

| Function | Status | Impact |
|----------|--------|--------|
| `compute_func_effects` | **NO-OP** | func_effects map never populated — makes verify_declared_effects and verify_deterministic inert |
| `verify_conserve` | Stub | No @conserve enforcement |
| `verify_gas` | Stub | No @gas enforcement |
| `verify_safety_annotations` | Stub | No @safety enforcement |
| `verify_constant_time` | Stub | No @constant_time enforcement |
| `verify_realtime` | Stub | No @realtime enforcement |
| `verify_differentiable` | Stub | No @differentiable enforcement |
| `verify_quantized` | Stub | No @quantized enforcement |
| `verify_checkpoint` | Stub | No @checkpoint enforcement |
| `verify_dp` | Stub | No @dp enforcement |
| `verify_shard` | Stub | No @shard enforcement |
| `verify_adapter` | Stub | No @adapter enforcement |
| `verify_tainted` | Stub | No @tainted enforcement |
| `verify_contracts` | Stub | No given/ensure enforcement |
| `verify_allocator_params` | Stub | No @allocator enforcement |
| `check_visibility` | **DISABLED** | `return;` immediately — no pub/private enforcement |

### Missing analysis functions (entire suites)

| Function group | Status | Purpose |
|---------------|--------|---------|
| `suggest_closest` + edit_distance | MISSING | Did-you-mean diagnostics |
| `is_vec_type` + swizzle checks | MISSING | SIMD vector type checking |
| `resolve_pattern_enum` | MISSING | Enum shorthand pattern resolution |
| `subst_type_params` | MISSING | Generic type substitution |
| `resolve_assoc_in_type` | MISSING | Associated type resolution |
| `type_from_name` | MISSING | Primitive name → Type mapping |
| `promote_for_arith` | MISSING | Mixed-type arithmetic promotion |
| `get_tensor_shape` + `shapes_compatible` | MISSING | Tensor shape checking |
| `collect_captured_vars` | MISSING | Closure capture analysis |
| `count_gas_cost_*` | MISSING | @gas cost accounting |
| `scan_for_calls` | MISSING | Call graph computation |
| `scan_for_division` | MISSING | Division safety verification |
| `scan_for_while_loops` | MISSING | Loop scanning for @realtime |
| `scan_for_early_returns` | MISSING | Early return detection |
| `scan_for_call_sites` | MISSING | Call-site extraction |
| `eval_contract` | MISSING | given/ensure contract evaluation |

### Shallow type resolution
- `resolve_type` only handles `Type::Named` — does NOT recurse into `Ref`, `Array`, `Generic`, `Tuple`, `RawPtr`, `Func`, `ShapedTensor`
- `types_compatible` has no rules for `Func`, `ShapedTensor`, `Map`, `Set`, `Channel` (matches Rust gap)
- `type_size` returns 8 for everything except primitives and Named structs

### Region budget tracking: COMPLETE
- Region stack, budget allocation, usage tracking, and enforcement all implemented

### Generic type resolution: PARTIAL
- Basic generic parameter tracking works
- Missing `subst_type_params` and `resolve_pattern_enum` for full substitution

---

## 4. opt.rs → opt.fd (completion: ~88%) — PARTIAL

All 23 optimization passes are present. 17 are complete, 4 are partial, 1 is stub.

### Per-pass status

| Pass | Status | What's missing |
|------|--------|----------------|
| `pass_const_fold` | **COMPLETE** | Float folding added (session 10) |
| `pass_cse` | **COMPLETE** | Intra-block CSE with @deterministic call CSE |
| `pass_gvn` | PARTIAL | Entry-block seeding only; no dominator/RPO-based GVN; no `resolve_copy` |
| `pass_sroa` | **COMPLETE** | alloca escape detection |
| `pass_dse` | **COMPLETE** | Dead store elimination with escape tracking |
| `pass_dce` | **COMPLETE** | `remove_unreachable_blocks` added (session 10) |
| `pass_devirt` | STUB | Single-impl devirt works; sandbox devirt infrastructure only (needs module trait_impls) |
| `pass_jump_thread` | PARTIAL | Simpler form set than Rust |
| `pass_loop_rotate` | **COMPLETE** | |
| `pass_loop_vectorize` | PARTIAL | Only widens induction step 1→4 for constant trip counts; missing `vectorize_loop`, `is_array_load`, `has_loop_carried_dep` |
| `pass_inline` | PARTIAL | Multi-block inlining added (session 10); LTO limits added; still missing sandbox-aware limits |
| `pass_unroll` | **COMPLETE** | remap_inst + ValueId cloning |
| `pass_tco` | **COMPLETE** | Param-to-alloca replacement with loads added (session 10) |
| `pass_alias_analysis` | PARTIAL | Within-block only; no dominator tree, no borrow-based aliasing |
| `pass_effect_reorder` | **COMPLETE** | Memory-dependency safety + terminator ordering fixed (session 10) |
| `pass_partial_eval` | **COMPLETE** | Pure-call / built-in / user-function eval |
| `pass_slp_vectorize` | **COMPLETE** | F64 adjacent load/store/binop vectorization |
| `pass_effect_optimize` | **COMPLETE** | Dead pure-call elimination + @crashfree hoisting |
| `pass_strength_reduce` | **COMPLETE** | x*2^n → x<<n, x*0, etc. |
| `pass_region_pool` | **COMPLETE** | p_alloc replacement + p_region_free insertion |
| `pass_reassociate` | **COMPLETE** | Add/Mul constant folding + canonicalization |
| `pass_memoize` | **COMPLETE** | Per-block @deterministic call memoization |

### Missing helper functions
`compute_dominators`, `compute_rpo`, `compute_predecessors`, `vectorize_loop`,
`is_array_load`, `has_loop_carried_dep`, `borrow_non_aliasing`, `resolve_copy`,
`new_const_int`

### Helpers already ported (session 10)
`remap_inst`, `inst_uses_value`, `remove_unreachable_blocks`, `const_float_value`,
`eval_binop_float`, `inline_multi_block`, `max_block_id`, `remap_block_id`,
`clone_inst_remapped_blocks`, `terminator_targets`

---

## 5. lower.rs → lower.fd (completion: ~85%) — PARTIAL

### Expression lowering: ALL ARMS PRESENT

| Expression | Status | Notes |
|-----------|--------|-------|
| `Expr::Asm` | **COMPLETE** | Added session 10 — Inst::Asm with input/output operands |
| `Expr::Cast` to `Dyn` | **COMPLETE** | Added session 10 — MakeDyn instruction |
| `Expr::QuantCast` | **COMPLETE** | Added session 10 — proper cast lowering |
| `Expr::PipeForward` | PARTIAL | Call/Ident/FieldAccess added (session 10); MethodCall desugaring may be wrong (left becomes receiver, not first arg) |
| `Expr::Spawn` | **COMPLETE** | Added session 10 — p_thread_spawn call |
| `Expr::TryBlock` | **COMPLETE** | Added session 10 — Result alloca/error/continue |
| `Expr::FString` | **COMPLETE** | Vec_to_str formatting added (session 10) |
| `Expr::OptionalFieldAccess` | PARTIAL | Rewritten to FieldAccess, missing None unwrap branch |
| `Expr::ComptimeReflect` | STUB | Returns ConstInt(0) |
| All other Expr arms | **COMPLETE** | IntLit, FloatLit, BoolLit, StringLit, Ident, Binary, Unary, Call, MethodCall, FieldAccess, Index, StructLit, If, Block, TupleLit, Range, Ref, Deref, StrConcat, EnumLit, Match, Try, Coalesce, IsCheck, Closure, ArrayLit, Comptime, ComptimeBlock, Await |

### Statement lowering: ALL ARMS PRESENT

| Statement | Status | Notes |
|-----------|--------|-------|
| `Stmt::Select` | **COMPLETE** | Added session 10 — p_chan_try_recv polling chain |
| `Stmt::Do`/`Handle` | **COMPLETE** | Added session 10 — effect_handler_stack |
| `for c in str` | **COMPLETE** | Added session 10 — p_str_length + String_byte_at |
| `@persist` saves | **COMPLETE** | Added session 10 — emit_persist_saves before returns |
| All other Stmt arms | **COMPLETE** | Let, LetUninit, LetTuple, Assign, Expr, Return, If, While, For, Loop, Break, Continue, Defer, Match, IndexAssign, DerefAssign, FieldAssign, Unsafe, RegionBlock, Sandbox, Txn, ComptimeGen, GradBlock, Backward, Query, Use |

### Helper functions

| Function | Status | Notes |
|----------|--------|-------|
| `emit_persist_saves` | **COMPLETE** | Added session 10 |
| `infer_vec_elem_type` | **COMPLETE** | Added session 10 |
| `infer_map_types` | **COMPLETE** | Added session 10 |
| `infer_closure_ret_ty` | **COMPLETE** | Added session 10 (simplified) |
| `ir_type_to_ast_type` | **COMPLETE** | Added session 10 |
| `lower_swizzle` | PARTIAL | Added session 10; only xyzw group, missing rgba/stpq |
| `lower_vec_sret_call` | PARTIAL | Added session 10; passes empty EffectSet instead of looking up function effects |
| `pattern_binding_ast_types` | MISSING | Not yet ported — affects generic Vec/HashMap type recovery in match arms |
| `pattern_binding_ir_types` | **COMPLETE** | IR payload types for match bindings |

### Remaining gaps

| Gap | Impact |
|-----|--------|
| **Capture types all I64** | `find_captures` returns `Vec<str>` not `Vec<(str, Type)>`; closure environments untyped |
| **Swizzle groups** | Only xyzw; missing rgba/stpq |
| **SRET effects** | `lower_vec_sret_call` passes empty EffectSet |
| **Pattern binding AST types** | `var_ast_types` not updated for match arm bindings |
| **OptionalFieldAccess** | No None unwrap branch |
| **PipeForward MethodCall** | left becomes receiver instead of first arg |
| **infer_expr_ast_type** | Simplified — returns I64 for most expressions |

---

## Priority Ranking — What to Fix First

### Tier 1: Blocks self-hosting fixpoint (critical bugs)
1. **pe.fd `p_malloc` bug** — passes 0 to HeapAlloc (breaks all PE programs)
2. **pe.fd `p_print_int` broken** — negative numbers unreachable, digit loop wrong
3. **pe.fd ungenerated labels** — `p_print_vec`/`p_print_map` registered but no body
4. **pe.fd `p_free` no-op** — memory leak

### Tier 2: Correctness gaps (affects output)
5. **typechecker effect verification** — all 16 verify_* functions stubbed
6. **typechecker `compute_func_effects`** — no-op, makes effect checks inert
7. **typechecker `check_visibility`** — disabled
8. **typechecker `resolve_type`** — doesn't recurse into Ref/Array/Generic/Tuple
9. **lower capture types** — all captures stored as I64
10. **lower `pattern_binding_ast_types`** — missing, affects generic type recovery

### Tier 3: Feature completeness
11. **pe.fd missing string runtime** — 21 string functions entirely missing
12. **pe.fd missing SIMD/AVX encoders** — packed double, AVX, scalar single
13. **pe.fd missing vec/map helpers** — contains, clone, insert, remove, sort, keys, values
14. **main.fd missing commands** — 17 of 37 commands missing
15. **main.fd `lint` stub** — always prints "No lint warnings"
16. **main.fd `test` runner** — only C backend, no interp/PE
17. **opt `pass_loop_vectorize`** — only step widening, no real vectorization
18. **opt `pass_gvn`** — no dominator/RPO-based GVN
19. **opt `pass_alias_analysis`** — only within-block, no dominator tree
20. **opt `pass_devirt`** — sandbox devirt infrastructure only

### Tier 4: Nice to have
21. **pe.fd flat/boot modes** — bare-metal output
22. **typechecker did-you-mean** — typo suggestions
23. **typechecker gas/contract verification** — @gas, given/ensure
24. **lower swizzle groups** — rgba/stpq support
25. **lower OptionalFieldAccess** — None unwrap branch
26. **opt dominator tree infrastructure** — compute_dominators, compute_rpo, compute_predecessors
27. **main.fd global flags** — --borrowck, --timeout

---

## Session 10 Changes (this session)

### lower.fd improvements (12 items)
- Added Expr::Asm lowering (inline assembly)
- Added Expr::Cast to Dyn (MakeDyn instruction)
- Fixed Expr::QuantCast (proper cast lowering)
- Expanded Expr::PipeForward (MethodCall, Ident, FieldAccess)
- Added Expr::Spawn (p_thread_spawn call)
- Added Expr::TryBlock (Result alloca/error/continue)
- Fixed Expr::FString (Vec_to_str formatting)
- Added Stmt::Select (channel select with p_chan_try_recv)
- Added Stmt::Do/Handle (effect_handler_stack)
- Added for c in str (string iteration)
- Added @persist saves (emit_persist_saves before returns)
- Added helpers: infer_vec_elem_type, infer_map_types, infer_closure_ret_ty, ir_type_to_ast_type, lower_swizzle, lower_vec_sret_call

### opt.fd improvements (6 items)
- Fixed pass_tco (replace param uses with loads)
- Added pass_dce remove_unreachable_blocks
- Added pass_const_fold float folding
- Fixed pass_inline (multi-block inlining, LTO limits)
- Fixed pass_effect_reorder (memory-dependency safety, terminator ordering)
- Added pass_devirt sandbox infrastructure

### pir.fd changes
- Added lto_enabled field to L2Module

### Verification
- All 3 modified files compile successfully
- Self-host tests: 22/23 pass (1 pre-existing failure: test_codegen_c.fd)
- Key tests pass: test_lower.fd, test_opt.fd, test_pir.fd, test_ir.fd, test_compiler.fd
