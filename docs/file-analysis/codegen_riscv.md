# codegen_riscv.fd — PIR L3 → RISC-V (RV64) asm selector (554 LOC)

> **Re-verified 2026-10-23:** all headline findings confirmed — Slot-operand regression, `return` not `ret` (:412), immediates→t0 catch-all (:508), `sub reg,sp,imm` invalid, lui +0x800 miss, vreg%23 wrap. Still accurate.

## Role

Third target in the multi-arch backend: `L3Module → RV64 assembly text` →
`assemble_riscv` → `runtime_parts_riscv` → `linker`. Port of
`src/codegen_riscv.rs` (388 LOC). Structurally a clone of `codegen_arm64.fd`
with RISC-V mnemonics — and shares most of its defects.

## Where it's called

| Caller | Site | What |
|--------|------|------|
| `compiler/compiler.fd:52` | `use compiler::codegen_riscv` | import |
| `compiler/compiler.fd:1764` | `generate_asm_riscv(l3)` | `--target riscv`/`riscv64` — the only call site |

Exported helpers (`reg_name`, `riscv_arg_reg`, `riscv_escape_string`,
`riscv_operand_to_reg`, `riscv_cast_kind_eq`) have no external callers.

## Findings

### F1 — `Load`/`Store` lost `Operand::Slot` handling — HIGH (port regression)

Same regression as codegen_arm64: Rust `src/codegen_riscv.rs:204-232`
matches `ptr`/`Operand::Slot(s)` → `ld/sd reg, -N(fp)`. The Fides port
(:234-243) routes all pointers through `riscv_operand_to_reg` → Slot → `"t0"`
→ **every stack-slot load/store emits `ld/sd …, 0(t0)`** — through
uninitialized scratch. Fix: restore the `Operand::Slot` match arms.

### F2 — `riscv_operand_to_reg` maps 6/7 Operand variants → `"t0"` — HIGH (inherited, extra-sharp here)

Only `Operand::VReg` survives; Slot/ImmI/ImmF/ImmStr/ImmFunc/ImmNull → `"t0"`.
Worse than arm64's `"x9"`: **t0 is a live allocated register** — `reg_name`
maps vreg 0 → t0 — so materializing an immediate/slot into "t0" **clobbers a
live vreg**. `Ret(ImmI)` → `mv a0, t0`; call args `mv a0, t0`; `ImmFunc` →
`jalr t0`. Same inherited design flaw, more damage because the scratch
overlaps the allocation pool.

### F3 — `gen_ret` emits `    return` (not `ret`) — HIGH (inherited)

:412 — `return` is not a RISC-V mnemonic; the assembler rejects every
function containing a Ret. Identical in src:270. Correct form: `ret`
(=`jalr zero, ra, 0`).

### F4 — `StackAlloc` emits `sub reg, sp, imm` — invalid — MEDIUM (inherited)

:232 `sub ` + reg + `, sp, ` + int — **RISC-V `sub` is register-register
only**; the immediate form must be `addi reg, sp, -N`. Won't assemble.
Identical in src:202.

### F5 — `lui`/`addi` immediate loading is wrong twice — MEDIUM (inherited)

`gen_mov` (:320-326): `hi = v >> 12` — missing the `+0x800` rounding, so any
immediate with bit 11 set is **off by 0x1000** (addi's sign-extension eats
the carry). Also: `lui`+`addi` only materializes 32 bits — **any immediate
above 2³² silently truncates** (Rust truncates further via `hi as i32`).
Immediates needing the full 64-bit path (li-pseudo expansion / li.s
sequences) aren't handled. Identical in src:148-153.

### F6 — `fcvt.s.l`/`fcvt.l.s` on GPR names — MEDIUM (inherited)

`gen_cast` (:495-497): `fcvt` instructions require FP registers (f0-f31);
`d`/`s` here are integer ABI names (`a0`, `t0`, …) → **invalid asm** + no
integer↔FP register-file move even if it assembled (`fmv.w.x`/`fmv.x.w`
needed). Identical in src:329-330.

### F7 — Register wrap + scratch aliasing — MEDIUM (inherited)

`reg_name`: `mapped = 5 + (idx % 23)` → vregs 0-22 → t0-t6,s0-s11,a0-a7,s2+
(x5-x27). vreg ≥23 **wraps and aliases** (vreg 23→t0 = vreg 0's register)
→ silent register collisions in big functions. Same in src:44.

### F8 — `TryUnwrap` semantic divergence — LOW-MEDIUM (inherited)

:292 `beqz v, err` — checks **pointer==0**. The C backend checks the tag:
`*(int64_t*)v == 0`. A non-null Result pointer with tag=err → C says err,
RISC-V says ok — divergent semantics across backends.

### F9 — Misc — LOW (all inherited)

- `string_consts`/`float_consts` never populated → `.rodata` emission dead.
- `riscv_arg_reg(i≥8)` → `"a0"` fallback (args 9+ dropped anyway — AAPCS-style
  stack passing unimplemented); `gen_call_ptr` caps `i<7` (arg 7 dropped —
  inconsistent with `call`/`dyn_call`'s `<8`).
- `.globl main` vestigial (:145); `GlobalAddr` → `li d, 0` (:219) = every
  global at address 0.
- `ElemAddr` hardcodes 8-byte elements; `sext.w` is fine for i32→i64 but
  IntTrunc (`mv` only, :493) doesn't actually mask — relies on later loads.
- Bin op lowering is correct: `Gt`→`slt r,l`, `LtEq`→`!(r<l)`, `GtEq`→`!(l<r)`,
  `Eq`→`sub;seqz` — the comparator math is right (unlike arm64's bitfield
  swap in the encoder).
- `Br` uses `bnez` — correct conditional; label scheme `.L<fn><ctr>_b<id>`
  consistent with the encoder's expectations.
- Escape helper covers only `\` and `"` — newlines in string consts would
  break `.asciz` — but the const tables are never populated anyway (dead
  code layered on dead code).

## Compound story

`--target riscv` is broken at the same three layers as arm64:
selector emits wrong asm (`return`, `sub`-imm, `fcvt`, `[t0]` operands) →
`riscv_enc`/`assembler` can't parse or mis-encodes → linker + runtime
unverified. Priority fixes: `return`→`ret`, restore Slot arms, materialize
immediates into a *non-allocated* scratch (or real operand selection),
fix `sub`→`addi -`, fix `lui` hi computation, real FP regs for fcvt.

## Verification state

- Call sites/exhaustiveness verified; 22 L3Inst arms all covered.
- Cannot execute E2E — upstream `--target` chain already dead; all findings
  grounded in line-by-line Rust-vs-Fides comparison.
- Notably **the comparator/branch logic is correct** — this file's lowerings
  are less buggy than codegen_arm64's where it differs (Shl/Shr/comparisons
  fine); the damage concentrates in operand materialization + ret/sub/fcvt.
