# pir.fd — PIR L1/L2/L3 transforms

> > **FIXED 2026-10-23 (v2):** `StructCopy` at :1125 now lowers to `L1Inst::Store(dst, src, OptIrType::Some(ty), {write})` matching src/pir.rs:801 (was `skip→ConstInt(0)`). `codegen_x64::gen_store` already handles struct stores via `gen_struct_copy`. `is_pure` now sees Write effects from the ir.fd fix. `borrow_kinds` still unconsumed (opt.md).

> *Superseded re-verify:* confirmed — StructCopy still dropped at L1 (:1125 → `ConstInt(0)`), `is_pure` propagation still carries the missing-effects bug into the optimizer. `borrow_kinds` still populated but consumed nowhere (compounds opt.md).

- **Path:** `compiler/pir.fd` — **2,719 LOC** (Rust `src/pir.rs` = 2,262, ~120%)
- **Role:** the three-stage PIR lowering — `lower_ir_to_l1` (region/effect/capability annotation), `lower_l1_to_l2` (region resolution + drop), `lower_l2_to_l3` (vreg assignment, stack-frame/slot layout, materialization) — plus `generate_pir` pretty-printers for `pir`/`dump-pir`.

## Where it is called

| Caller | Site | Mechanism |
|---|---|---|
| `compiler.fd` | :1669-1678 | `lower_ir_to_l1` → `lower_l1_to_l2` → (opt) → `lower_l2_to_l3` — every `build` path |
| `main.fd` | :450-514, :1760s | `pir`/`dump-pir`/`ir` diagnostics |

Live on every compiled build; `generate_pir` backs the diagnostic dumps.

## Verified faithful (code inspection)

- All three lowerings are structurally 1:1 — inst-for-inst mapping tables, same erasures: `ConstFnRef`→`ConstStr` (:1057/Rust:761), `PersistLoad`→`Copy(default)` (:1111/:893), `PersistSave`→`Copy(0,I64)` (:1114/:897).
- Metadata harvest + propagation complete: `sandbox_values`, `linear_values`, `borrow_kinds`, `gas_limit`, `is_*` flags all collected from `ValueMeta` (:1160-1190) and carried through `L1Function`/`L2Function` field-for-field (:1226-1235, :1457-1469). The pipeline *would* deliver them — run-30's lower-side gap and run-36's opt-side gap are the actual breaks.
- `lower_l2_inst_to_l3` covers every `L2Inst` arm including `Copy` (:1971), `TryUnwrap`, `Print`, `Panic`, all casts — `S-17` parity.
- `FieldAddr` uses real struct layout when the base resolves to a named struct (:1914-1929) — not blind `index*8`.
- `materialize_slot`/`AddrOfSlot`/`StackAlloc` fallback chain correct; vregs sorted for deterministic slot assignment (S-17 determinism).

## Findings

### MEDIUM — `StructCopy` silently dropped at L1 *(port regression, currently latent)*

pir.fd:1125 — `Inst::StructCopy` → `skip = true` + `L1Inst::ConstInt(0, I64)` placeholder. Rust :801 lowers it to a real `L1Inst::Store` (a typed memcpy with Write effect).

Latent today because `lower.fd` **never emits** `StructCopy` (zero sites — it uses `Inst::Store(ptr, val, Some(Struct))` at :5007 instead). But if any path ever produces it, the copy evaporates: no code, no effect, no ref. Third leg of the StructCopy hole (ir.fd: no effects arm + no refs arm; here: no lowering).

### MEDIUM — `is_pure` inherits the effects hole *(propagation of run-26)*

`L2Function.is_pure = EffectSet_is_pure(&f.effects)` (:1457) — and `func_effects` folds `inst_effects`, which omits `StructCopy` (run 26). Any struct-memcpy-only function produced downstream would be marked pure → DCE strips it. Latent for the same reason as above, but it's the *propagation point* — the ir.fd metadata gap becomes an optimizer-visible wrongness here.

### Inherited — `ConstStr` name-collision → `ImmFunc`

`lower_l2_inst_to_l3` (:1866-1870): a `ConstStr` whose text equals a function name emits `Operand::ImmFunc` instead of `ImmStr`. So `var s = "main"` while `fn main` exists → the "string" is actually a function address. Same heuristic in Rust (:1460) — shared latent bug, only safe because `ConstFnRef`→`ConstStr` sites are the intended producers and user literals rarely collide with fn names.

### Inherited — block params carried but never bound

`params: b.params` at :1201/:1445/:1797 — params get typed `VReg`s at :1684-1791, but no predecessor ever moves values into them (`Jump`/`Branch` carry no args at any level — IR, L1, L2, or L3). Matches Rust exactly — a shared design gap: phi-less SSA where block params are declared, typed, and never bound. Consistent with run-30's interp-side finding (Void-prefilled reads).

## Fix order

1. Lower `StructCopy` to a `Store` (or keep `skip` but emit the `Store` explicitly) — 5 lines; makes the variant safe if produced.
2. Fix `inst_effects`/`inst_value_refs` in ir.fd — the actual upstream break feeding the `is_pure` wrongness.
3. (Feature decision) whether `Jump`/`Branch` should gain arg lists to make block params real — matches Rust, so parity argues to leave it.

## Verification state

- No live runs — three lowering functions audited arm-by-arm against `src/pir.rs` (inst tables, erasure arms, metadata propagation lists); the `ConstFnRef`/`Persist*`/`StructCopy` erasures compared literally; param-vreg handling traced through L2→L3.
