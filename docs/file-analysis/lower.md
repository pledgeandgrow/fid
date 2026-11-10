# lower.fd — AST → SSA IR lowering, with ownership (proposal 0004)

> **Re-audited 2026-11-09** against the working tree (lower.fd has uncommitted
> edits; another session may be editing it). Probes ran with the latest
> self-built compiler (`s18.exe`) on test files outside the repository.

**Path:** `compiler/lower.fd` — **14,023 lines** (was 6,930 on Oct 22).
358 functions. Imports `ast`, `cowcheck`, `ir`, `pe_ir`.

**Role:** lowers the checked, monomorphized AST into the `Inst` IR of
`ir.fd`. Builds the `IrModule` (layouts, externs, strings, globals), mangles
impl/trait methods, resolves named/default arguments, computes function
effects, and since Nov 7 inserts every ownership operation (dup, drop, move)
of proposal 0004. The entry point is `lower(program)` (lower.fd:2822).

## Callers

| Caller | Site | Reached from |
|--------|------|--------------|
| `compiler.fd` `lower_to_ir_module` | :2014 | `ir`/`dump-ir`, `pir`, `opt` (main.fd:462/546/617); `interpret_source*` (`run`, `test`, `bench`, `profile`); `veil`; `hotreload` (dead) |
| `compiler.fd` `lower_to_ir_module_with_path` | :2027 | `ccompile`/`compile`/`dump-c`/`split-c` (via `ccompile_source_with_path` :1826), `run` (:2082), `codegen`, `gpu`, `verify`, `sourcemap` (main.fd:2221/2322/2678/2719), `build_x86_linux` (:2382) |
| `compiler.fd` `compile_to_wasm` | :1863 | `wasm` |
| `compiler.fd` `build_executable` | :2135 | `build --native` |
| `main.fd` `cmd_build` | :2885, :2923, :2976, :3057, :3152 | `build` / `emit` (pe_ir PE, ELF, bare, nat_ir Linux / `--nat`) |
| `debugger.fd` | :794 | nothing imports debugger.fd (dead) |

`tests/selfhost/test_lower.fd` and `test_mut.fd` import it directly.

## Switches (all `const`, no environment variable)

| Name | Value | Effect |
|------|-------|--------|
| `LOWER_RC` (:12230) | **true** | Proposal 0004 ownership: a variable owns its value and is dropped at scope end; reads borrow; keeping a value takes an owner (`Lowerer_rc_own_value`); temporaries are dropped at statement end; parameters are borrowed. Off: the old auto-drop passes run instead. |
| `LOWER_BOX_RC` (:12234) | true | `Box::new` makes a counted block (`Box_dup`, `Box_drop_t`). Off: a Box is never freed. |
| `LOWER_INLINE_STRUCTS` (:12239) | true | Structs go into a Vec's buffer through `Vec_push_mem`/`Vec_set_mem`/`Vec_insert_mem` (size ≤ 232 words). Off: each struct element is a heap copy. |
| `LOWER_STACK_VECS` (:13432) | true | LF13: a local Vec that never leaves its function keeps its 40-byte header on the stack (`Vec_init_typed`, `Vec_drop_inplace`). |
| `LOWER_CANCEL_SHARES` (:13664) | true | LF15: a `Vec_share`/`HashMap_share` only read before its drop is not made. |
| `LOWER_LITERAL_SLOTS` (:13666) | true | LF17: string slots that only hold literals are not counted. |
| `LOWER_COW` (:7357) | **false** | Proposal 0001 copy-on-write shares and struct Vec-field shares. Superseded by `LOWER_RC`; must stay off (double frees). |
| `LOWER_ARG_SHARE` (:7363) | false | Callers pass a share of each by-value Vec argument. Only acts with `LOWER_COW`. |

`RC_NONE..RC_BOX` (:12241-12246) are kind codes, not switches.

## What it does now (features the Oct doc missed)

- **Ownership section** (:12220-14023): kind of a type (`Lowerer_rc_kind`),
  dup/drop (`Str_dup`/`Str_drop`, `Vec_share`/`Vec_drop`,
  `HashMap_share`/`HashMap_drop`, `Box_dup`/`Box_drop_t`, generated
  `__own_dup_N`/`__own_drop_N` per struct/enum and the `__p_mem_drop` /
  `__p_mem_dup` dispatchers), temporaries per statement (`Lowerer_lower_stmt`
  wrapper :5777 → `Lowerer_rc_flush`), scopes and loops, move on last use
  (`Lowerer_rc_move_last`, `Lowerer_rc_try_move`), borrow instead of copy
  (`Lowerer_rc_borrow_copy_ok`), parameters copied at entry only when the
  body changes them (:4224-4246).
- **Post passes in `lower()`** (:2822): `lw_rc_cancel_pairs` (dup+drop pairs,
  LF15, LF17) and `lw_rc_stack_vecs` (LF13).
- Value types: `Vec::filled` (`Vec_filled_typed`, :5246), integer-key HashMap
  detection, struct elements stored inline, element type ids (16 + index,
  `P_TID_BOX` for boxes).
- Concurrency: async task adapters for f64/struct arguments
  (`Lowerer_emit_task_adapter`, H9), `parallel` branches lifted to threads
  (`Lowerer_lift_parallel_branch`, H7), `spawn scope` / `cancel scope`
  (:6563, :6572), `await` restores f64/struct/pointer results (:10075).
- Effects: `do` with a continuation closure and `resume` (L3A,
  `Lowerer_lower_do_with_continuation` :4535, `Stmt::Resume` :6553); a
  `handle` ending a function gives its value (`Lowerer_lower_tail_handle`).
- `&T`/`&mut T` parameters tag `ValueMeta.borrow_kind` (LF1, :4129).
- Source line on every value (`Lowerer_emit_inst` :1250), C callbacks
  (`Lowerer_c_callback`), borrowed `&str` (M11), `p_str_concat_int` fusion,
  `select` statement, if/match as values (`lw_value_tail`).

## Findings — verified live

### HIGH — `asm!` with a `clobber` inside a loop crashes every compiled backend

Lowering encodes a clobber as an input operand with value id -1
(`StringValueId_new("CL:" + reg, -1)`, :10186). `checks.fd`
`peir_inst_operands` passes the -1 on and `peir_loop_facts` does
`Vec_set(-1)`. Probe (`while` loop holding `asm!("nop", clobber("rax"))`):
`fid run` prints 3; `dump-c` and `emit` (default PE and `--nat`) panic
"Vec index out of bounds" in `peir_loop_facts`. The native Linux path
rejects any clobber ("in(\"CL:rax\") is not a 64-bit register"), and
`irverify` reports the -1 as "defined nowhere" (see irverify.md). Fix: give
clobbers their own list in `Inst::Asm`, or have every consumer skip ids < 0.

### Fixed since Oct 23

- `await` / `spawn` under `run`: FIXED. `spawn f(21)` then `await` prints 42
  (interp.fd now runs `p_thread_spawn`/`p_future_poll`).
- `Expr::Coalesce` unreachable / unbound block params: FIXED. `??` lowers
  through a result slot (:9792); `o ?? 7` prints 7. No code calls
  `Lowerer_add_block_param` any more (:1427 is unused).
- `borrow_kind` never set: CHANGED. Reference parameters are tagged (:4129);
  `Expr::Ref` allocas still are not.
- `Stmt::Resume`/`SpawnScope`/`CancelScope` missing: FIXED, all three lower.
- Ownership end to end: a Vec of structs with str and Vec fields plus a Box,
  `run --leaks` (CC=zig cc): "every block was freed"; `var b = a; b.push(2)`
  leaves `a` at length 1 under `run` and `emit`.

### STILL TRUE

- `str` slicing (`s[1..3]`): the typechecker still rejects it ("cannot index
  type str"); lower has no `p_string_slice`.

## Findings — code inspection

### MEDIUM — calls emitted with an empty effect set

`p_thread_spawn` (:4872, :6361, :6405, :10148), `p_future_poll`,
`p_executor_new/run_all/cancel` (:6566-6576), `Vec_set` for `v[i] = x` and
`v[i] op= x` (:5613, :5642), `Vec_get`, `p_vec_get` (:9407), `p_malloc` (6
sites) and `p_chan_try_recv` (:5715) get `EffectSet_new()`.
`runtime_effects` (:354) is applied only on the method/assoc/call paths
(:6822, :6944, :7974). iropt's DCE never drops calls, so nothing is removed,
but `rules.fd` `ru_ext_effects` builds a callee's visible effects from
`inst_effects`: a function whose only effect is a spawn looks pure to
`purecse`. A probe (`start()` = `spawn work()`, called twice) reported
"0 pure calls merged" on aarch64, so no miscompile was seen.

### LOW — `spawn scope` does not use its executor

`spawn` inside a scope still emits `p_thread_spawn`; `p_executor_spawn` is
registered (:2254) but never emitted. The join works only because the C
runtime's `p_executor_run_all` joins every thread ever spawned and ignores
`exec` (codegen_c.fd:6114). With C the probe printed 6 three times. The PE
backend (`emit`) refuses these programs (`pe_ctx_swap`).

### LOW — `Expr::Index` treats any `I64` receiver as a Vec

STILL TRUE (:9366): `ir_type_eq(arr_ty, IrType::I64)` → `p_vec_get`.
Latent (the typechecker rejects indexing non-collections).

### LOW — `use compiler::pe_ir` only for `peir_inst_operands`

lower.fd:35 imports pe_ir (and so pe_ir_enc, pe_ir_stubs) to reach a
function that lives in checks.fd. That is the import AGENTS.md item 17 warns
about. `use compiler::checks` is enough.

### INFO — dead code kept behind switches

`lw_auto_drop_vecs`/`lw_auto_drop_strs` and helpers (:3223-3896, about 670
lines) run only with `LOWER_RC` off. `Lowerer_emit_struct_share` (:1328),
`Lowerer_emit_vec_share`, `Lowerer_readonly_copy*` and the copy-on-write
parts of `Lowerer_emit_inst` run only with `LOWER_COW` on.

### INFO — stale comment on `LOWER_COW`

The doc comment (:7347-7356) says "On since Nov 6"; the value is false and
the Ownership section says it is replaced.

### Not re-checked / unchanged inherited notes

- `Expr::ComptimeReflect` → `ConstInt(0)` (:10163) and `Expr::Yield`
  evaluating its operand only (:10156): unchanged, not probed.
- Without a handler, `do` evaluates its arguments and continues (:4555).

## Fix order

1. Asm clobber encoding (crash on every compiled backend).
2. Give the direct `Inst::Call` sites their `runtime_effects`.
3. `use compiler::checks` instead of `pe_ir`; delete or isolate the
   `LOWER_COW`/auto-drop code once proposal 0004 is final.
4. Emit `p_executor_spawn` inside `spawn scope`.

## Verification state

Static: all switches, `lower()`, the Ownership section headers, the
statement wrapper, parameter copies and the old findings' sites read in
current source. Live (s18.exe): asm crash (dump-c, emit, emit --nat),
await/spawn and `??` under `run`, `--leaks` clean, value semantics, spawn
scope under C. Not run: the test suites.
