# File Analysis: compiler/ir.fd

> > **FIXED 2026-10-23 (v2):** `StructCopy` added to `inst_effects` (Write) and `inst_value_refs` (dst+src pushed) — no longer invisible to optimizer. Other catch-all `_ => {}` gaps and O(N²) linear lookups remain.

> *Superseded re-verify:* all findings confirmed — `inst_effects` catch-all `_ => {}` (:1163) returns *empty* EffectSet (worse than reported: StructCopy isn't the only silent case — any new instruction is invisible to the optimizer), StructCopy absent from inst_value_refs, O(N²) linear lookups confirmed (:1509-1528).

**Run:** 26/52 | **LOC:** 2,551 (src/ir.rs: 1,811, 141% — Vec-entry structs + Opt* enums + constructor shims account for the growth) | **Status:** LIVE — central type definitions


> C5/C6 fixed 2026-10-23 (v3): verify Branch field order corrected; DCE keeps ConstFnRef-address-taken fns on indirect calls; get_inst/get_meta O(1) index-by-id fast path.

## Where it's called

The foundational module — `IrModule`, `IrFunction`, `Inst`, `IrType`, `ValueMeta`, `EffectSet`, `Region`, `BasicBlock`. Imported by **17 modules**: ast-adjacent every backend (codegen_c/x64/pe/wasm), pipeline (compiler/lower/opt/pir/regalloc/interp), and consumers (main/debugger/hotreload/inc/veil/autograd).

Verified faithful pieces (1:1 with Rust): `lower_type`/`box_lower_type` (all 22 Type arms identical, incl. documented `ArrayConst→0`, `I128→I64`), `ir_type_size`/`type_align`/`type_size` (Struct→0 via layout lookup — the known backend pain point), `ir_type_eq` (hand-rolled structural equality — correct, incl. recursive Ptr/Array/Generic), `eval_binop_int` (full int op coverage + width truncation, div-zero→None), `intern_string` (dedup → `.str_N`), `add_function` replace-on-duplicate, `emit`/`new_value`/`new_block` plumbing.

## Findings

### HIGH (live) — `inst_effects` drops `StructCopy` → functions look pure

`inst_effects` (:1128-1163) has no `StructCopy` arm → falls to `_ => {}` → **empty effects**. Rust maps `Inst::StructCopy → Effect::Write` (ir.rs, arm exists). `lower.fd:2457` folds `inst_effects` into `func_effects` for every function — a function whose work is `s2 = s1` struct memcpy reports **no Write effect** → opt's effect-driven passes (DCE, effect-reorder, @crashfree hoisting, pure-call elision) treat it as side-effect-free. Live: every lowered function gets the wrong effect set.

### MEDIUM — `inst_value_refs` also misses `StructCopy` (and has no fallback)

:1228-1329 — no `StructCopy` arm AND no `_` wildcard → returns empty refs for a memcpy whose dst+src ARE value references. Same class as the S-17 `Copy` omission AGENTS.md documents for Rust's L2. Currently only latent — nothing calls `inst_value_refs` except dead `verify` — but it will bite whoever wires verification or a def-use pass.

### MEDIUM — `IrModule_verify` misvalidates `Branch` (positional destructure bug)

:2337 — `Inst::Branch(then_b, else_b, _)` destructures positionally, but the variant order is `Branch(cond, then_block, else_block)` (interp.fd:1274, `inst_value_refs` :1263 both treat field 0 as cond). So verify checks **cond's value-id as a then-block** and **then_block as the else-block**; the real else target is never validated. Rust uses named fields `{ then, else_ }` (ir.rs:1762) — correct. **Port regression.** Also missing Rust's second check — "all ValueId references are defined" (defined_values+params pass) — the Fides verify checks only block structure/targets/terminators, not use-def integrity. Dead today (Rust calls it at main.rs:1434; the port dropped the call site), which is the only reason it hasn't produced false errors.

### MEDIUM — `IrModule_dead_function_elimination` strips indirect calls if wired

Fides-added (:1968, no Rust counterpart), never called. BFS roots at `main`/`main_entry`/`p_*`/externs and scans only `Inst::Call` targets — **`CallPtr`/`DynCall` targets are invisible** → any function reachable only through a function pointer, closure, or trait dispatch gets removed. `IrModule_optimize` (:1952) wires it in — which is exactly why `optimize` can never safely be enabled: it const-folds AND strips closure-called functions in one pass.

### MEDIUM (perf, architectural) — Vec-replaces-HashMap on the hot path

Every lookup is a linear scan: `IrFunction_get_inst` (:1509), `IrFunction_get_meta` (:1520), `IrModule_get_function` (:1815), `IrModule_get_struct` (:1716), `IrFunction_lookup_local` (:1496), `intern_string` (:1727). Rust uses `HashMap<ValueId, _>` throughout → O(1). The cost lands exactly on the interpreter's per-instruction hot path: each executed inst does `get_inst` (O(N)) + for BinOp 2× `get_meta` (O(N) each) + `get_function` per Call (O(F)) + `get_meta` per GetFieldPtr/Call. A 2,000-inst function pays ~2,000 comparisons per executed instruction — **O(N²) interpretation, likely the dominant cost of `fidesc run`**. The interp additionally does `get_meta` twice per BinOp *just* to detect `Ptr(U8)` string comparisons (interp.fd:978-1008). Fix without redesign: keep Vecs sorted by id and binary-search, or index `insts`/`value_metas` directly by id (ids are dense `next_value` counters — `value_metas` is already id-ordered by construction at :1435-1439, so `get_meta(id)` could be `value_metas.get(id)` — O(1) today).

### Inherited / noted

- `ir_type_size(Struct)→0` — by design (needs module layout lookup); callers using `ir_type_size` instead of `IrModule_type_size` get 0 — the run-8/run-10 GetElemPtr/ElemAddr bugs trace to this API split.
- `Inst` variants: full 1:1 coverage vs Rust (42 variants incl. TryUnwrap/DynCall/GradBegin/Backward/Persist/Txn/Region).
- ~800 LOC of `Opt*`/`String*`/`ValueId*` entry structs + `*_new` shims — correct-by-construction Vec-map adaptation; the verbosity is the tax, not a defect.

## Verdict

The data-model core is solid — types, equality, layouts, lowering all verified 1:1. The real findings are (1) the **live** `StructCopy` effect hole feeding wrong `func_effects` to the optimizer, and (2) the linear-scan lookups that make `fidesc run` quadratic — `get_meta`/`get_inst` can be O(1) index-by-id *today* since ids are dense counters. `verify` and `dead_function_elimination` are dead code with latent bugs; fix them before ever wiring them, not after.
