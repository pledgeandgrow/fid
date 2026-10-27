# codegen_arm64.fd — PIR L3 → ARM64 asm selector (557 LOC)

> **Re-verified 2026-10-23:** all headline findings confirmed — immediates→x9 catch-all (:509-514), `return` not `ret`, movz/movk stops at bit 47, vreg%29 aliasing, CallPtr arg-7 drop. Still accurate.

## Role

The **live** ARM64 backend (unlike dead `codegen.fd`): instruction selector
consuming `L3Module` + register allocation, emitting ARM64 asm text. Port of
`src/codegen_arm64.rs` (394 LOC).

Pipeline: `L3Module → generate_asm_arm64 → asm text → assemble_arm64 → linker`.
Output feeds the encoder analyzed in run 2 — so this file's bugs are masked
behind arm64_enc's `fp`/`&mut Vec`/`mov x29,sp` failures.

## Where it's called

| Caller | Site | What |
|--------|------|------|
| `compiler/compiler.fd:49` | `use compiler::codegen_arm64` | import |
| `compiler/compiler.fd:1697` | `generate_asm_arm64(l3)` | `--target arm64` / `aarch64` — the only call site |

No other callers. Exported helpers (`reg_name`, `vreg_to_reg`, `arm_escape_string`,
`strip_x_prefix`, `cast_kind_eq`) have no external users.

## Findings

### F1 — `Load`/`Store` lost `Operand::Slot` handling — HIGH (port regression)

`src/codegen_arm64.rs:212-237` matches `ptr` on `Operand::Slot(s)` →
`ldr/str d, [fp, #-N]`. The Fides port (:234-243) routes **all** pointer
operands through `arm_operand_to_reg`, which returns `"x9"` for non-VReg →
**every stack-slot load/store emits `ldr d, [x9]` / `str v, [x9]`** — reads/
writes through uninitialized scratch. Slots are the most common operand kind
in L3 code. Strict regression vs the Rust reference.

### F2 — `arm_operand_to_reg` maps 6/7 Operand variants → `"x9"` — HIGH (inherited)

`Operand` (pir.fd:731): VReg, Slot, ImmI, ImmF, ImmStr, ImmFunc, ImmNull.
Only VReg survives; everything else → `x9` (uninitialized scratch). Consequences:

- **Immediate args in calls**: `foo(5)` → `mov x0, x9` — passes garbage.
- **`Ret(ImmI(42))`** → `mov x0, x9` — the canonical `return 42` returns garbage.
- `Br`/`Load`/`Store`/`TryUnwrap`/`Print`/`ElemAddr`/`FieldAddr` immediates → x9.
- `ImmFunc` → `blr x9` (closure/fn-ptr calls crash); `ImmNull` → `cmp x9,#0`.
- `ImmStr` → x9 — **and** `string_consts`/`float_consts` are never populated
  anywhere in the file (:120-121 declared, only read at :154-170) → `.rodata`
  emission is dead code; string literals have no path to a label at all.

Identical `_ => "x9"` in Rust (:366-370) → inherited, but Fides made it worse
by deleting the Slot special-cases (F1).

### F3 — `gen_ret` emits `    return`, not `ret` — HIGH (inherited)

:416 `Arm64CodeGen_wl(self, "    return")` — `return` is not an ARM64 mnemonic;
the assembler/encoder rejects every function ending with Ret. Identical in
`src/codegen_arm64.rs:274`. **Every L3 program fails to assemble here** even
before the encoder's own bugs — the arm64 target never stood a chance.

### F4 — `vreg_to_reg` wrap-around aliasing — MEDIUM (inherited)

`vreg.id % 29` → vreg 29 → x0, vreg 30 → x1… — virtual regs ≥29 **alias** low
registers silently (x29/x30 are never reached, so FP/LR are safe but distinct
vregs collide). Any function needing >29 vregs miscompiles. Same in Rust.

### F5 — `movz/movk` misses bits 48-63 — MEDIUM (inherited)

`gen_mov` (:315-331) emits movz + up to 2 movk → covers bits 0-47 only.
`-1` → `0x0000FFFFFFFFFFFF` (top 16 bits zeroed); any imm >2⁴⁸ truncates.
Fides added `v < 0` guards for negatives — but still only 3 chunks.
Rust identical structure → inherited; needs a 4th movk (`lsl #48`).

### F6 — Fixed 256-byte frame + `StackAlloc` shrinks sp permanently — LOW (inherited)

:182 `frame_size = 256` regardless of actual locals ("simplified"). Slot
offsets beyond 256B collide with saved fp/lr. `StackAlloc` emits `sub reg, sp, #bytes`
— mutates sp mid-body; `gen_ret` restores `mov sp, x29` so it balances, but
nested allocs accumulate — fragile.

### F7 — Arg-count drops + misc — LOW (inherited)

- `gen_call_ptr` uses `i < 7` while `gen_call`/`gen_dyn_call` use `< 8` —
  CallPtr silently drops arg index 7 (Rust same `take(7)` — likely wrong).
- Args 9+ dropped on all call forms (AAPCS stack passing unimplemented).
- `GlobalAddr` → `mov d, #0` (:219) — **every global's address is 0** →
  null-pointer store/load (Rust identical).
- `gen_bin`: no `else` — a missed BinOp emits nothing (exhaustive today:
  18 ops all handled).
- `Cast::IntTrunc` emits `w`-regs (:497) — does arm64_enc parse `w8`? If not,
  another assembler failure downstream.
- `.globl main` vestigial (:145) — no `main` symbol may exist (funcs are named).
- `strip_x_prefix` returns `"0"` for non-x names → `fcvtzs d0, d0` mishaps.
- `label_counter` shared across functions — label names `.L<func><N>_b<id>`
  stay unique since N increments per function ✓ (but cross-function refs fine).
- `NUM_REGS=29` comment "(X29=FP…)" misleading since modulo never yields x29.

## Compound story

The arm64 target is dead **three layers deep**:
1. `compile --target arm64` → this selector emits `return` + `[x9]` everywhere
   (F1-F3) — wrong asm even where instructions exist.
2. `assemble_arm64` → `Unknown register: fp` (prologue `mov x29, sp`/`add d, fp, #…`
   from :179,:213) — encoder rejects, or under the interp emits 0 bytes.
3. Even if those pass, encoder mis-encodes `mov x29,sp` (ORR+XZR→x29=0),
   drops register-offset addressing, and the linker's arm64 branch delta is
   off by 4 — plus `.rodata` discarded.

Fix order: F3 (`return`→`ret`) + F1 (restore Slot arms) + F3-materialize
immediates (F2) + 4th movk (F5) — then re-verify against arm64_enc fixes.

## Verification state

- Callers/exhaustiveness verified by grep; all 18 BinOp + 5 CastKind + 22
  L3Inst variants covered.
- Cannot execute E2E: upstream `fp` failure blocks every program; fidesc
  interp also blocks ast-importers (in-progress split_gt work).
- All findings grounded in direct Rust-vs-Fides line comparison.
