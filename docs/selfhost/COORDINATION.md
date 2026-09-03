# Coordination — Parallel Agent Sessions

> **READ THIS FIRST, BEFORE ANY OTHER FILE.**
> This is the single source of truth for what is being worked on, by whom,
> and what architectural decisions have been made. Without this, parallel
> agents will conflict and undo each other's work.
>
> **Last updated:** Sep 3, 2026

---

## How to Use This File

### Before starting work
1. Read this file completely.
2. Check "Active Sessions" — are any sessions working on files you need?
3. Check "Architectural Decisions" — has a direction changed since you last read the docs?
4. Add your session to "Active Sessions" with the files you plan to touch.
5. Read `CURRENT.md` for the current blocker.

### During work
1. If you make an architectural decision, add it to "Architectural Decisions" immediately.
2. If you change which files you're editing, update your session entry.
3. If you discover something important, add it to "Key Findings".

### After finishing
1. Run `./verify.sh` and `./verify-selfhost.sh`.
2. Update `CURRENT.md` with what you did.
3. Remove your session from "Active Sessions".
4. Update relevant docs in `docs/selfhost/` if you learned something new.

---

## Active Sessions

> Format: `[Session ID] — [Task] — [Files being edited] — [Status]`
> Add your session when you start, remove when you finish.

| Session | Task | Files | Status |
|---------|------|-------|--------|
| (none) | — | — | — |

> **If a file is listed here, DO NOT edit it in another session.**
> Wait for the session to finish, or coordinate with the user.

---

## Architectural Decisions

> **Every architectural decision must be recorded here with a date.**
> If a decision is reversed, mark it as `[REVERSED]` and add the new decision.
> This is what prevents agents from following outdated docs.

### [D1] Enum payload storage — Sep 3, 2026

**Decision:** Enum payload fields that are structs/enums are stored as **pointers** (8 bytes each), not inline data.

**Rationale:**
- The C backend treats `IrType::Struct(_)` as a pointer type (`struct Foo_s*`).
- `Inst::Load` with a struct type reads only 8 bytes (the pointer).
- Storing inline struct data requires `memcpy` via `Inst::Store` with explicit `store_ty`, which is complex and error-prone.
- Storing pointers is simpler: every payload slot is 8 bytes, `Load` reads the pointer, field access works through the pointer.

**What this means for the code:**
1. `compute_enum_layout` / `Lowerer_make_enum_layout`: every field gets 8 bytes, regardless of type. `payload_slots = max_variant_arity` (number of fields), not `max_payload_size / 8`.
2. `EnumLit`: struct-typed args are heap-allocated (or already pointers from SRET), and the pointer is stored in the payload slot.
3. `pattern_binding_ir_types`: returns `word_repr(lower_type(t))` for all fields — `Struct(_)` becomes `Ptr(Struct(_))`. **[DONE in `src/lower.rs` by user]**
4. Match extraction: `Inst::Load` reads the 8-byte pointer. No `load_enum_field` needed.
5. `load_enum_field` in `src/lower.rs`: can be simplified to just `Inst::Load` for all types, or removed.

**Status:** `src/lower.rs` `pattern_binding_ir_types` changed to use `word_repr` for all types. `compute_enum_layout` and `EnumLit` in `src/lower.rs` still use inline storage — **INCONSISTENT — needs to be updated**.

**What needs to change:**
- `src/lower.rs` `compute_enum_layout`: change to 8 bytes per field (not struct size)
- `src/lower.rs` `EnumLit`: store struct args as pointers (single slot), not multi-slot inline copy
- `src/lower.rs` `load_enum_field`: remove or simplify (just `Load`)
- `compiler/lower.fd` `Lowerer_make_enum_layout`: change to 8 bytes per field
- `compiler/lower.fd` `EnumLit`: store struct args as pointers
- `compiler/lower.fd` match extraction: use `Load` (already does — no change needed)

**Previous direction [REVERSED]:** Store struct data inline in enum payloads, use `load_enum_field()` + `memcpy` to extract. This was documented in `docs/selfhost/calling-conventions.md` and `docs/selfhost/known-bugs.md` but is now outdated.

### [D2] SRET calling convention — Feb 2026

**Decision:** Functions returning structs use the SRET (struct return via hidden out-parameter) convention.

**Status:** Implemented in both `src/lower.rs` and `compiler/lower.fd`. Working correctly.

### [D3] `box_expr_unbox` in `compiler/lower.fd` is out of sync — Feb 2026

**Decision:** Must be reconciled with `compiler/ast.fd` canonical `Expr` enum.

**Status:** Not yet done. `box_expr_unbox` lists variants that don't exist in `ast.fd` (`Lambda`, `Throw`, `TryUnwrap`, `Assign`, `CompoundAssign`, `ConstExpr`, `ComptimeGen`, `QuantCast`, `Await`, `Spawn`).

---

## Key Findings

> Discoveries that affect the direction. Add new findings at the top.

### [F3] Fides lowerer `EnumLit` does not do multi-slot copy — Sep 3, 2026

`compiler/lower.fd` line 3337 stores each enum arg at field index `ai + 1` as a single 8-byte value:
```fides
var data_ptr: i64 = Lowerer_emit_inst(self, Inst::GetFieldPtr(alloca, ai + 1, IrType::I64));
var _s2: i64 = Lowerer_emit_inst(self, Inst::Store(data_ptr, arg_val, OptIrType::None));
```

The Rust lowerer (`src/lower.rs` lines 4650-4668) does multi-slot copy for struct-typed args. The Fides lowerer does not. This means the Fides lowerer already loses struct data on enum construction, not just extraction.

**With the pointer-based approach [D1], this bug goes away** — every arg is a single 8-byte pointer, so single-slot store is correct.

### [F2] `c_type(IrType::Struct(name))` returns a pointer type — Feb 2026

`compiler/codegen_c.fd` line 146:
```fides
IrType::Struct(name) => { return "struct " + name + "_s*"; }
```

This means `Inst::Load` with `IrType::Struct(_)` reads 8 bytes (a pointer), not the full struct. This is the root cause of the enum variant extraction bug.

**With the pointer-based approach [D1], this is correct** — the payload slot stores a pointer, and `Load` reads it.

### [F1] `Inst::Store` in Fides codegen does not emit `memcpy` for structs — Feb 2026

`compiler/codegen_c.fd` line 1961 ignores the `store_ty` parameter and treats all stores as scalar. The Rust codegen (`src/codegen_c.rs` lines 1498-1556) handles `IrType::Struct(name)` with `memcpy`.

**With the pointer-based approach [D1], this doesn't matter** — struct stores are always 8-byte pointer stores.

---

## Pre-Flight Checklist (Every Session)

Before touching any file:

- [ ] Read this file (`docs/selfhost/COORDINATION.md`)
- [ ] Read `CURRENT.md`
- [ ] Check "Active Sessions" — no conflicts with your planned files
- [ ] Check "Architectural Decisions" — no direction changes since last time
- [ ] Run `./verify.sh` to confirm baseline
- [ ] Add your session to "Active Sessions"

---

## File Ownership Map

> Which files are "owned" by which concern. If you're working on one of these
> files, you need to understand the full context.

| File | Concern | Coordinated by |
|------|---------|----------------|
| `src/lower.rs` | Enum layout, SRET, match extraction | This file |
| `compiler/lower.fd` | Fides lowerer — must mirror `src/lower.rs` | This file |
| `src/codegen_c.rs` | C codegen — struct store, SRET | This file |
| `compiler/codegen_c.fd` | Fides C codegen — must mirror `src/codegen_c.rs` | This file |
| `compiler/ast.fd` | Canonical AST — changes require updating `box_*_unbox` everywhere | This file |
| `CURRENT.md` | Current status — update after every milestone | Session that completes a milestone |

---

## Session Protocol

### Starting a session
1. Read this file + `CURRENT.md` + `AGENTS.md`.
2. Pick a task from `CURRENT.md` "Next steps" or from the roadmap.
3. Add your session to "Active Sessions" with the files you'll touch.
4. Run `./verify.sh`.

### Making architectural decisions
1. Add the decision to "Architectural Decisions" with a date and rationale.
2. If it reverses a previous decision, mark the old one as `[REVERSED]`.
3. Update any docs that reference the old direction.
4. Commit the decision before proceeding with implementation.

### Finishing a session
1. Run `./verify.sh` and `./verify-selfhost.sh`.
2. Update `CURRENT.md` with what you did and the new state.
3. Update relevant docs in `docs/selfhost/`.
4. Remove your session from "Active Sessions".
5. Commit your changes.

---

## Common Mistakes to Avoid

1. **Following outdated docs.** Always check "Architectural Decisions" first. The docs in `docs/selfhost/` may be stale.

2. **Editing files another session is working on.** Check "Active Sessions" first.

3. **Changing direction without recording it.** If you reverse an architectural decision, record it here immediately so other agents don't follow the old direction.

4. **Not running `./verify.sh` before and after.** You might be starting from a broken state, or you might break something without knowing.

5. **Updating `src/lower.rs` without updating `compiler/lower.fd`.** These must stay in sync. If you change one, the other needs the same change.

6. **Updating `src/codegen_c.rs` without updating `compiler/codegen_c.fd`.** Same — these must stay in sync.
