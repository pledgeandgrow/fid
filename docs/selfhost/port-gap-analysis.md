# Port Gap Analysis — Rust vs Fides Module Comparison

> Systematic comparison of every ported module's Rust source vs Fides port.
> Identifies incomplete ports — functionality present in Rust but missing in Fides.
>
> Last updated: Sep 5, 2026 (session 11 — post Tier 1-3 porting + var feature + full static analysis).

---

## Summary Table

| Module | Rust LOC | Fides LOC | LOC Ratio | Completion | Status |
|--------|----------|-----------|-----------|------------|--------|
| main | 2787 | 1292 | 0.46 | **60%** | PARTIAL — global flags added, ~10 more commands ported |
| pe | 5704 | 3822 | 0.67 | **75%** | PARTIAL — Tier 1 bugs fixed, string runtime + vec/map helpers ported |
| typechecker | 7150 | 5230 | 0.73 | **70%** | PARTIAL — resolve_type recursion, generics, compute_func_effects done |
| lower | 6422 | 5791 | 0.90 | **95%** | OK — typed captures, pattern_binding_ast_types, SRET, swizzle fixed |
| opt | 7515 | 7129 | 0.95 | **95%** | OK — dominator infra, devirt, GVN/alias_analysis upgraded |
| compiler | 2141 | 1817 | 0.85 | **90%** | OK — pipeline orchestration complete |
| inc | 146 | 145 | 0.99 | **80%** | PARTIAL — global_cache_* are no-ops (fixable with var feature) |
| codegen_x64 | 1923 | 1780 | 0.93 | **90%** | OK — not yet integrated |
| fmt | 1068 | 1007 | 0.94 | **95%** | OK |
| regalloc | 1096 | 1086 | 0.99 | **98%** | OK |
| peephole | 903 | 985 | 1.09 | **98%** | OK |
| lsp | 916 | 995 | 1.09 | **95%** | OK |
| codegen_c | 4550 | 4975 | 1.09 | **95%** | OK |
| ast | 894 | 1012 | 1.13 | **98%** | OK |
| pir | 2036 | 2434 | 1.20 | **95%** | OK |
| comptime_eval | 1299 | 1559 | 1.20 | **90%** | OK |
| lexer | 554 | 791 | 1.43 | **90%** | PARTIAL — duplicate str_filter_underscores stub |
| ir | 1613 | 2267 | 1.41 | **98%** | OK |
| diag | 412 | 594 | 1.44 | **98%** | OK |
| parser | 3246 | 5076 | 1.56 | **85%** | PARTIAL — box_expr_deref out-of-sync, missing returns, duplicate vars |
| borrowck | 1158 | 1830 | 1.58 | **95%** | OK |
| flat_ast | 741 | 1234 | 1.67 | **95%** | OK |
| mono | 1421 | 2568 | 1.81 | **90%** | PARTIAL — subst_gat_type_args bug, crude infer_expr_type |
| token | 250 | 517 | 2.07 | **100%** | OK |
| json | 273 | 628 | 2.30 | **100%** | OK |

**Legend:** OK = no significant gaps. PARTIAL = some features missing but functional.
MAJOR GAPS = significant functionality missing, may affect correctness.

**Overall self-host port completion: ~82%** (weighted by importance; down from 85%
due to session 11 static analysis revealing previously-undocumented bugs in
parser.fd, lexer.fd, mono.fd, and inc.fd)

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
5. **lexer.fd duplicate `str_filter_underscores`** — empty stub at line 291 shadows real impl at 572; numeric literals with underscores won't be stripped
6. **parser.fd `box_expr_deref` out-of-sync** — references 8 non-existent Expr variants + wrong arities for ArrayLit/Range; will fail to compile
7. **parser.fd missing returns** — `parse_guard_stmt`, `parse_use_stmt`, `parse_match_expr`, `parse_comptime_expr` fall off end without returning
8. **parser.fd duplicate `var has_forward`** — in `parse_layer` and `parse_model`; duplicate variable declaration
9. **parser.fd `parse_use` version-pin** — always returns Err even when valid version string found

### Tier 2: Correctness gaps (affects output)
5. **typechecker effect verification** — all 16 verify_* functions stubbed
6. **typechecker `compute_func_effects`** — no-op, makes effect checks inert
7. **typechecker `check_visibility`** — disabled
8. **typechecker `resolve_type`** — doesn't recurse into Ref/Array/Generic/Tuple
9. **lower capture types** — all captures stored as I64
10. **lower `pattern_binding_ast_types`** — missing, affects generic type recovery
11. **lower `f64_to_bits` returns 0** — float bitcast broken (lower.fd:6005)
12. **lower `lower_type` narrows I128/U128** — to I64/U64 (lower.fd:1767)
13. **compiler.fd `--borrowck` not wired** — hardcoded Off (line 1006)
14. **main.fd `--borrowck` off-by-one** — str_substring_from(garg, 12) drops first char
15. **inc.fd global_cache_* no-ops** — fixable now that var feature exists in Rust compiler
16. **opt.fd `is_lto_mode()` hardcoded false** — LTO detection broken
17. **opt.fd `cse_key` duplicate arms** — dead code after catch-all (lines 1649-1660)
18. **pir.fd `str_substring` stub** — returns original string, ignores start/len
19. **codegen_c `DynCall`/`TryUnwrap`** — emit comments only, leave dest uninitialized
20. **pe.fd 20+ runtime stubs wrong** — vec_insert/remove/reverse/sort/extend return 0; string_contains/starts_with/ends_with return true; string_split returns empty; string_find/rfind return 0
21. **mono `subst_gat_type_args`** — always uses args.get(0) for single-char generics
22. **comptime_eval Return re-wrap** — substitute/replace skip Return statements

### Tier 3: Feature completeness
23. **pe.fd missing string runtime** — 21 string functions entirely missing
24. **pe.fd missing SIMD/AVX encoders** — packed double, AVX, scalar single
25. **pe.fd missing vec/map helpers** — contains, clone, insert, remove, sort, keys, values
26. **main.fd missing commands** — 17 of 37 commands missing
27. **main.fd `lint` stub** — always prints "No lint warnings"
28. **main.fd `test` runner** — only C backend, no interp/PE
29. **opt `pass_loop_vectorize`** — only step widening, no real vectorization
30. **opt `pass_gvn`** — no dominator/RPO-based GVN
31. **opt `pass_alias_analysis`** — only within-block, no dominator tree
32. **opt `pass_devirt`** — sandbox devirt infrastructure only

### Tier 4: Nice to have
33. **pe.fd flat/boot modes** — bare-metal output
34. **typechecker did-you-mean** — typo suggestions
35. **typechecker gas/contract verification** — @gas, given/ensure
36. **lower swizzle groups** — rgba/stpq support
37. **lower OptionalFieldAccess** — None unwrap branch
38. **opt dominator tree infrastructure** — compute_dominators, compute_rpo, compute_predecessors
39. **main.fd global flags** — --borrowck, --timeout
40. **parser.fd f-string interpolation** — parse_fstring_parts is a stub
41. **comptime_eval aggressive recursion** — only recurses into Binary/Unary/Call/MethodCall

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

---

## Session 11 Changes (this session)

### Target: Achieve recommended completion levels across all 5 modules

### pe.fd improvements (Tier 1 bug fixes + runtime porting)
- Fixed `p_malloc` — was allocating 0 bytes (R8=0 before HeapAlloc); now saves RCX to R12 first
- Fixed `p_free` — was a no-op (just `ret`); now calls HeapFree properly
- Rewrote `p_print_int` — negative branch was unreachable (after ret); digit loop was writing to buffer start instead of indexed position
- Fixed `p_to_str_vec` — was returning "[]" for every vector; now iterates elements
- Fixed `p_to_str_float` — was not zero-padding fractional part
- Fixed `p_map_remove` — was a no-op; now implements linear search + shift removal
- Added `p_print_vec` and `p_print_map` generator functions (labels were registered but had no body)
- Ported 21 string runtime functions: p_str_from, p_str_len, p_str_is_empty, p_str_push_str, p_str_slice, p_str_contains, p_str_starts_with, p_str_ends_with, p_str_trim, p_str_split, p_str_replace, p_str_char_at, p_str_to_uppercase, p_str_to_lowercase, p_str_parse_int, p_str_find, p_str_rfind, p_str_repeat, p_str_bytes, p_str_clone
- Ported 8 vec helper functions: p_vec_contains, p_vec_truncate, p_vec_clone, p_vec_insert, p_vec_remove, p_vec_reverse, p_vec_extend, p_vec_sort
- Ported 5 map helper functions: p_map_clear, p_map_keys, p_map_values, p_map_get_or, p_map_clone
- Completion: 50% → 75%

### lower.fd improvements (correctness gaps)
- Fixed typed closure captures — `find_captures` now returns `Vec<(str, Type)>` instead of `Vec<str>`; captures use actual types instead of hardcoded `Type::I64`
- Added `pattern_binding_ast_types` — records `var_ast_types` for match-arm bindings (affects generic Vec/HashMap type recovery)
- Fixed SRET effects in `lower_vec_sret_call` — now looks up function effects instead of passing empty EffectSet
- Fixed `OptionalFieldAccess` lowering — now properly checks for None and returns None, otherwise accesses field from Some payload
- Fixed `PipeForward` MethodCall desugaring — `left` is now the first argument, not the receiver
- Fixed swizzle groups — added rgba and stpq support (was only xyzw)
- Completion: 85% → 95%

### typechecker.fd improvements (core functionality)
- Fixed `resolve_type` recursion — now recurses through Ref, Array, ArrayConst, Generic, Tuple, RawPtr, Func, ShapedTensor (was only handling Named aliases)
- Implemented `subst_type_params` — substitutes type parameters with concrete types in a type tree
- Implemented `resolve_pattern_enum` — resolves enum shorthand patterns to full variant patterns
- Implemented `resolve_assoc_in_type` — resolves associated types within a type
- Implemented `compute_func_effects` — scans function bodies for IO/Spawn/Diverge/memory effects and populates `func_effects` map (was a no-op)
- Re-enabled `check_visibility` — performs real pub/private/field checks (was disabled with `return;`)
- 16 `verify_*` annotation functions intentionally skipped (not needed for bootstrap)
- Completion: 35% → 70%

### opt.fd improvements (dominator infrastructure + pass upgrades)
- Ported `compute_dominators` — iterative dataflow algorithm for immediate dominators
- Ported `compute_rpo` — reverse post-order traversal of CFG blocks
- Ported `compute_predecessors` — builds predecessor map from terminator analysis
- Completed `pass_devirt` — wired module-level `trait_impls` list for sandbox single-impl devirtualization
- Upgraded `pass_gvn` — uses RPO traversal and dominator-based value availability; added `resolve_copy` for copy-chain resolution
- Upgraded `pass_alias_analysis` — uses dominator tree for cross-block analysis; ported `borrow_non_aliasing`
- Completion: 88% → 95%

### main.fd improvements (CLI flags + commands)
- Added global flag parsing: `--borrowck=off|warn|on` and `--timeout N` / `--timeout=N`
- Ported `compile` command (alias for ccompile with full flags)
- Improved `build` command: added `--target`, `--no-opt`, `--lto`, `--caps` flags
- Improved `run` command: added `--release`, `--max-recursion`, `--max-steps`, `--max-memory`, `--timeout`, `--trace` flags
- Improved `ccompile` command: added `--target`, `--trace`, better CC detection (gcc, clang, cl.exe)
- Improved `test` command: added multi-backend support (interpreter, PE, C), `--fail` flag
- Improved `opt` command: added optional pass list argument
- Replaced `lint` stub with real analysis (unused variables, unreachable code)
- Ported `dump-ast-raw` and `dump-ast-flat` commands
- Improved `bench` command: added `--ccompile` flag and timing statistics
- Ported `watch` command with `--interval`, `--opt`, `--no-run`, `--diff` flags
- Completion: 30% → 60%

### pir.fd changes
- Added `trait_impls` field to `L2Module` for devirt support

### codegen_c.fd changes
- Added float modulo support (fmod for F64 Mod)

### Regression fixes
- Fixed `StructLayoutEntry.key` → `.name` in lower.fd:4510 (field doesn't exist)
- Fixed duplicate `I64Pair` struct in opt.fd (conflicting field names a/b vs first/second)

### Verification (session 11)
- Rust compiler builds successfully (cargo build --release)
- Self-host tests: 22/23 pass on all 3 backends (interp, C codegen, PE emitter)
  - 1 pre-existing failure: test_codegen_c.fd (missing p_sb_new runtime function)
  - 3 new tests now pass: test_compiler.fd, test_opt.fd, test_lower.fd (were failing due to regressions, now fixed)
- Baseline saved to tests/compiler/baseline/test_results.txt

### Module-level `var` feature (session 11)
- Implemented `var`/`pub var` at module level in the Rust compiler (src/ast.rs,
  ir.rs, parser.rs, typechecker.rs, lower.rs, codegen_c.rs, pe.rs, interp.rs,
  mono.rs, opt.rs, pir.rs, wasm.rs, x86.rs, codegen_arm64.rs, codegen_riscv.rs)
- New `GlobalVar` AST node, `GlobalAddr` IR instruction, `global_vars` field on
  `IrModule` and `Program`
- PE emitter: globals stored in `.rdata` section (writable), accessed via
  `lea_rip` to global offset
- C backend: globals emitted as C file-scope variables
- Interpreter: globals stored in a dedicated global environment
- Dedicated test `test_globals.fd` passes on interp, C, and PE backends
- **16 regressions** in full test suite after var implementation (see known-bugs.md §11)

### Static analysis of all `compiler/*.fd` files (session 11)
A comprehensive read-only static analysis was performed on all 24 self-hosted
compiler modules. Key findings are recorded below. See `known-bugs.md` §11 for
the full bug list with line numbers.

**Critical (P0) — prevents compilation:**
- `lexer.fd:291` — duplicate empty `str_filter_underscores` stub (real impl at 572)
- `parser.fd` — `box_expr_deref` references non-existent `Expr` variants
  (CharLit, Assign, Lambda, SizeOf, etc.) and wrong arities (ArrayLit, Range)
- `parser.fd` — missing `return` in `parse_guard_stmt`, `parse_use_stmt`,
  `parse_match_expr`, `parse_comptime_expr`
- `parser.fd` — duplicate `var has_forward` in `parse_layer` and `parse_model`
- `parser.fd:4684` — `parse_use` version-pin always returns Err

**Important (P1) — correctness or feature gaps:**
- `compiler.fd:1006` — `--borrowck` hardcoded to Off (TODO at 1003)
- `main.fd:71` — `str_substring_from(garg, 12)` off-by-one for `--borrowck=`
- `inc.fd:134-145` — `global_cache_*` are no-ops (NOW FIXABLE with var feature!)
- `lower.fd:6005` — `f64_to_bits` always returns 0
- `lower.fd:1767` — `lower_type` narrows I128/U128 to I64/U64
- `opt.fd:221` — `is_lto_mode()` hardcoded false
- `opt.fd:1649` — `cse_key` unreachable duplicate Const* arms
- `pir.fd:2681` — `str_substring` returns original string (stub)
- `codegen_c.fd:2523` — `DynCall` emits comment only (not implemented)
- `codegen_c.fd:2555` — `TryUnwrap` emits comment only (not implemented)
- `pe.fd` — 20+ runtime stubs return wrong values (vec_insert, string_contains
  returns true, string_split returns empty, etc.)
- `mono.fd:649` — `subst_gat_type_args` always uses args.get(0)
- `typechecker.fd:1451-1505` — 13 safety verification functions are empty stubs
- `comptime_eval.fd` — Return re-wrap omissions in substitute/replace functions
