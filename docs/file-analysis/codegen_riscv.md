# codegen_riscv.fd — PIR L3 → RISC-V 64 assembly

Re-audited 2026-11-09 (previous audit 2026-10-22/23).

- **Path:** `compiler/codegen_riscv.fd`
- **LOC:** 554
- **Role:** instruction selector of the old PIR path for RV64: a clone of
  codegen_arm64 with RISC-V mnemonics (vregs `5 + id % 23`, fixed
  256-byte frame). Port of `src/codegen_riscv.rs`. The working RISC-V
  backend is now nat_ir (`fid emit --target riscv64-linux`).

## Where it is called

| Caller | What |
|--------|------|
| `main.fd` `cmd_codegen` | `generate_asm_riscv(l3)` for `--target riscv|riscv64` |
| `compiler.fd` `build_executable` | riscv branch — unreachable from the CLI (`build --native --target riscv*` goes to nat_ir) |

Only live command: `fid codegen f.fd --target riscv [--elf o]` (the
`--elf` object is empty, elf64.md 2).

## Findings

All from code inspection (no RISC-V probe run; the arm64 twin was probed
and shows the same pattern, see codegen_arm64.md).

1. **F1 Load/Store lost `Operand::Slot` — STILL TRUE** (only `AddrOfSlot`,
   :212).
2. **F2 non-VReg operands → `t0`, which is also vreg 0's register —
   STILL TRUE** (:57, :508-510).
3. **F3 `return` instead of `ret` — STILL TRUE** (:412).
4. **F4 StackAlloc `sub reg, sp, imm` — STILL TRUE** (:232).
5. **F5 `lui` without `+0x800`, 32-bit only — STILL TRUE** (:320-325).
6. **F6 `fcvt` on integer registers — STILL TRUE** (:495).
7. **F7 `% 23` aliasing — STILL TRUE** (:51).
8. **F8 TryUnwrap `beqz` on the pointer — STILL TRUE** (:292).
9. **F9 misc (CallPtr `i < 7`, GlobalAddr → 0, frame 256) — STILL TRUE**
   (:440, :216, :184).

## Fix order

Retire with the PIR path (roadmap MI17); not worth fixing.

## Verification state

- Read: the sites above. No probe.
